# TMC2209 UART 寄存器配置与功能封装设计
> **背景**：UART 控制方式下，STM32 通过 **USART1 单线 UART**（PA9→1kΩ→PDN_UART、PA10 直连）**仅读写 TMC2209 寄存器**，完成电流、微步、斩波、StallGuard 等配置；运动控制仍由 STEP/DIR 硬件完成。
> **依据**：TMC2209 数据手册 Rev.1.03。本文件为**设计文档，暂不改代码**。

---

## 一、概述

```
STM32(USART1,115200) ──单线UART──► TMC2209 PDN_UART
   │ 写帧：GCONF/IHOLD_IRUN/CHOPCONF/...
   │ 读帧：GSTAT/DRV_STATUS/IOIN/SG_RESULT...
   └─ 仅读写寄存器，不参与运动控制（STEP/DIR 由 TIM2 硬件输出）
```

---

## 二、单线 UART 协议（USART1，115200 8N1）

### 2.1 帧格式（CRC8 校验，以数据手册 4 章为准）
```
写：   [0x05][addr][0x80|reg][d3][d2][d1][d0][crc]        （8 字节）
读请求：[0x05][addr][reg][crc]                             （4 字节，手册 4.1.2）
应答： [0x05][0xFF][reg][d3][d2][d1][d0][crc]              （8 字节，读应答）
addr = MS1 + 2×MS2（当前 0）
CRC 覆盖：写帧前 7 字节；读请求前 3 字节；应答前 7 字节
CRC8-ATM（手册 4.2）：多项式 x^8+x^2+x+1=0x07，初值 0，
按 LSB→MSB 逐位处理（if((crc>>7)^(inbit)) crc=(crc<<1)^0x07; else crc<<=1;）
```

### 2.2 关键点
- 发送时 PA10 收到**自回显**：写帧自回显 8 字节、读请求自回显 4 字节，读应答需丢弃自回显后再校验；
- 读帧后 TMC2209 在 SENDDELAY（默认 8 位时间）后应答；
- 帧间 ≥8 位时间；连续读写留 ≥1ms。

---

## 三、寄存器清单（可配置 / 只读）

### 3.1 可配置寄存器
| 地址 | 名称 | 类型 | 位定义（PDF 为准） |
|---|---|---|---|
| 0x00 | GCONF | RW | I_scale_analog(0)、internal_Rsense(1)、en_SpreadCycle(2)、shaft(3)、index_otpw(4)、index_step(5)、**pdn_disable(6)**、mstep_reg_select(7)、multistep_filt(8)、test_mode(9) |
| 0x03 | SLAVECONF | W | SENDDELAY(11:8) |
| 0x10 | IHOLD_IRUN | W | **IHOLD(4:0)、IRUN(12:8)、IHOLDDELAY(19:16)** |
| 0x11 | TPOWERDOWN | W | 静止→降流延时（默认 20，最小 2） |
| 0x13 | TPWMTHRS | W | StealthChop↔SpreadCycle 切换速度上限 |
| 0x14 | TCOOLTHRS | W | CoolStep/StallGuard 使能速度下限 |
| 0x22 | VACTUAL | W | 内部脉冲发生器速度（≠0 忽略 STEP） |
| 0x40 | SGTHRS | W | StallGuard 阈值（SG_RESULT≤2×SGTHRS 判堵转） |
| 0x42 | COOLCONF | W | seimin(15)、sedn(14:13)、semax(11:8)、seup(6:5) |
| 0x6C | CHOPCONF | RW | toff(3:0)、hstrt(6:4)、**hend(10:7)**、TBL(16:15)、vsense(17)、MRES(27:24)、intpol(28)、dedge(29)、diss2g(30)、diss2vs(31) |
| 0x70 | PWMCONF | RW | StealthChop 电压 PWM 参数（PWM_LIM(31)、pwm_freq、pwm_autoscale 等） |

> ⚠️ 注意：`hend` 为 **bit10:7**（4 位，滞环 -3~+12），非 9:7。

### 3.2 只读寄存器
| 地址 | 名称 | 位定义 |
|---|---|---|
| 0x01 | GSTAT | reset(0)、drv_err(1)、uv_cp(2)，**写 1 清位** |
| 0x02 | IFCNT | 成功写帧计数（校验通信） |
| 0x06 | IOIN | ENN(0)、MS1(2)、MS2(3)、DIAG(4)、PDN_UART(6)、STEP(7)、SPREAD_EN(8)、DIR(9)、VERSION(31:24)=0x21 |
| 0x12 | TSTEP | 实测 1/256 微步间隔（1/fCLK 单位，溢出=(2^20)-1） |
| 0x41 | SG_RESULT | StallGuard 读数，越大负载越小 |
| 0x6F | DRV_STATUS | **otpw(0)、ot(1)、s2ga(2)、s2gb(3)、s2vsa(4)、s2vsb(5)、ola(6)、olb(7)、t120(8)、t143(9)、t150(10)、t157(11)、CS_ACTUAL(20:16)、stealth(30)、stst(31)** |

> ⚠️ 注意：DRV_STATUS 位序以本表为准（otpw 在 bit0）。

## 四、功能封装函数设计（uart_control 内实现）

### 4.1 基础读写（底层）
```c
static uint8_t TMC2209_CRC8(uint8_t crc, uint8_t data);        /* CRC8 poly0x07 */
static uint8_t TMC2209_SendFrame(uint8_t *tx, uint8_t len);    /* USART1 发送 */
static uint8_t TMC2209_ReceiveFrame(uint8_t *rx, uint8_t len); /* 丢弃自回显后接收 */
uint8_t TMC2209_WriteReg(uint8_t reg, uint32_t val);           /* 写寄存器 */
uint8_t TMC2209_ReadReg(uint8_t reg, uint32_t *val);           /* 读寄存器 */
```
- 写：组帧 `[0x05][addr][0x80|reg][val MSB..LSB][crc]` → 发送；
- 读：组帧 `[0x05][addr][reg][0x00..0][crc]` → 发送 → 收 8 字节应答（丢弃自回显）→ 校验 crc → 取 d3..d0。

### 4.2 读取全部寄存器快照（初始化前必备）
```c
/* 所有可读寄存器快照（R 或 RW；纯 W 寄存器不可读回，读返回 0，不列入） */
typedef struct {
  uint32_t gconf;      /* 0x00 GCONF       RW */
  uint32_t gstat;      /* 0x01 GSTAT       R/WC */
  uint32_t ifcnt;      /* 0x02 IFCNT       R  */
  uint32_t ioin;       /* 0x06 IOIN        R  */
  uint32_t tstep;      /* 0x12 TSTEP       R  */
  uint32_t sg_result;  /* 0x41 SG_RESULT   R  */
  uint32_t chopconf;   /* 0x6C CHOPCONF    RW */
  uint32_t drv_status; /* 0x6F DRV_STATUS  R  */
  uint32_t pwmconf;    /* 0x70 PWMCONF     RW */
} TMC2209_RegSnapshot_t;

uint8_t TMC2209_ReadAllRegs(TMC2209_RegSnapshot_t *snap);
        /* 逐个 ReadReg 填充快照；任一失败返回 FAIL。
           调用方可打印/保存该快照，作为上电默认值与初始化后对比的依据 */
```

### 4.3 初始化 / 使能
```c
uint8_t TMC2209_Init(uint8_t irun, uint8_t ihold);       /* 配置 GCONF/IHOLD/CHOPCONF 等 */
uint8_t TMC2209_Enable(uint8_t en);                      /* CHOPCONF.toff: 0=禁用, 3=使能(StealthChop) */
uint8_t TMC2209_CheckVersion(void);                      /* 读 IOIN.VERSION==0x21 校验通信 */
```

### 4.4 电流配置
```c
uint8_t TMC2209_SetCurrent(uint8_t irun, uint8_t ihold, uint8_t iholddelay);
        /* 写 0x10：IRUN(12:8)、IHOLD(4:0)、IHOLDDELAY(19:16)；
           IRUN/IHOLD 范围 0~31（1/32~32/32），建议 IRUN 16~31 */
uint8_t TMC2209_SetVsense(uint8_t vsense);              /* CHOPCONF.vsense(17)：0=325mV,1=180mV */
uint8_t TMC2209_SetPowerDownDelay(uint8_t tpwrdwn);     /* 0x11，建议≥2 */
```

### 4.5 微步配置
```c
uint8_t TMC2209_SetMicrosteps(uint8_t mres);  /* MRES(27:24)：0=256,1=128,...,4=16,5=8,6=4,7=2,8=fullstep */
        /* 使用寄存器微步时需先置 GCONF.mstep_reg_select=1(bit7) */
uint8_t TMC2209_SetIntpol(uint8_t on);                   /* CHOPCONF.intpol(28)：1=插值到256细分 */
```

### 4.6 斩波模式选择
```c
uint8_t TMC2209_SetChopperMode(uint8_t spread);  /* GCONF.en_SpreadCycle(2)：
        0=StealthChop（静音），1=SpreadCycle（高动态）；
        注意 SPREAD 引脚为高时会取反该位 */
uint8_t TMC2209_SetToff(uint8_t toff);           /* CHOPCONF.toff(3:0)：0=关闭驱动, 3=StealthChop默认 */
```

### 4.7 静止降流 / 待机
```c
uint8_t TMC2209_ConfigStandstill(uint8_t ihold, uint8_t iholddelay); /* 0x10 中 IHOLD/IHOLDDELAY */
uint8_t TMC2209_SetPdnDisable(uint8_t en);       /* GCONF.pdn_disable(6)：UART 模式下必须=1 */
```

### 4.8 StallGuard（无传感器堵转/回零）
```c
uint8_t TMC2209_SetStallThreshold(uint8_t sgthrs);    /* 0x40，SG_RESULT≤2×SGTHRS 判堵转 */
uint8_t TMC2209_SetCoolThreshold(uint32_t tcoolthrs); /* 0x14 速度下限（TSTEP 单位） */
uint8_t TMC2209_GetStallResult(uint16_t *sg);         /* 读 0x41 */
```
> 使能 StallGuard 输出到 DIAG 需满足 `TCOOLTHRS ≤ TSTEP ≤ TPWMTHRS` 且设好 SGTHRS。

### 4.9 CoolStep（负载自适应电流）
```c
uint8_t TMC2209_SetCoolStep(uint8_t semin, uint8_t seup, uint8_t sedn, uint8_t semax, uint8_t seimin);
        /* 0x42：semin(3:0)、seup(6:5)、sedn(14:13)、semax(11:8)、seimin(15) */
```

### 4.10 状态读取
```c
uint8_t TMC2209_ReadStatus(uint32_t *gstat, uint32_t *drv);  /* 0x01/0x6F */
uint8_t TMC2209_ReadIOIN(uint32_t *ioin);                    /* 0x06 */
uint8_t TMC2209_ReadIfcnt(uint8_t *cnt);                     /* 0x02 通信校验 */
```

---

## 五、合适的初始化配置（推荐）与初始化序列

### 5.1 推荐配置表（StealthChop 静音主方案）

| 寄存器 | 建议值 | 关键位 | 依据/说明 |
|---|---|---|---|
| **GCONF** (0x00) | `0x000000C0` | pdn_disable(6)=1、mstep_reg_select(7)=1 | UART 模式必备 pdn_disable；微步由寄存器 MRES 决定 |
| **IHOLD_IRUN** (0x10) | `0x0002180C` | IHOLDDELAY(19:16)=2、IRUN(12:8)=24、IHOLD(4:0)=12 | IRUN=24→25/32 满量程；IHOLD=12→静止≈13/32（约 40%） |
| **TPOWERDOWN** (0x11) | `20` | 静止→降流延时 | 20×2^18/fCLK≈0.44s（数据手册默认 20，最小 2） |
| **CHOPCONF** (0x6C) | `0x15000003` | MRES(27:24)=5→**1/8 微步**、intpol(28)=1、toff(3:0)=3、TBL=0、vsense=0 | StealthChop 推荐（toff=3，TBL 数据手册推荐 %00，vsense 325mV） |
| **PWMCONF** (0x70) | `0xC20C0E24` | pwm_autoscale(18)=1、pwm_autograd(19)=1、PWM_REG(27:24)=2、PWM_LIM(31:28)=12、PWM_GRAD(15:8)=14、PWM_OFS(7:0)=36 | StealthChop 自动调节；GRAD=14/OFS=36 为数据手册默认 |
| **TPWMTHRS** (0x13) | `0` | 禁用 StealthChop→SpreadCycle 切换 | 全程 StealthChop（静音） |
| **TCOOLTHRS** (0x14) | `0` | 禁用 CoolStep/StallGuard 输出 | 未启用 |

**SpreadCycle 高动态变体**（高速/大力矩场景）：
| 寄存器 | 建议值 | 关键位 | 说明 |
|---|---|---|---|
| GCONF (0x00) | `0x000000C4` | +en_SpreadCycle(2)=1 | 启用 SpreadCycle |
| CHOPCONF (0x6C) | `0x15000055` | toff(3:0)=5、hstrt(6:4)=5、hend=0 | AN001 推荐的 SpreadCycle 参数 |

**电流换算**（便于按电机调 IRUN）：
```
IRMS = (CS+1)/32 × 0.325V / (RSENSE+20mΩ) / √2
例：RSENSE=0.15Ω、IRUN=24 → ≈1.06A；建议 IRUN=16~31
```

> 原则：**先读取所有寄存器当前值（上电默认快照）→ 再写入上述初始化配置**，便于排查默认状态、与初始化后对比。

```c
/* 0. 先读取所有寄存器当前值（上电默认快照），可打印/保存后核对 */
TMC2209_RegSnapshot_t snap;
TMC2209_ReadAllRegs(&snap);   /* 读 0x00/01/02/06/12/41/6C/6F/70 */

/* 1. 校验通信 */
if (TMC2209_ReadReg(0x06, &ioin) != OK || (ioin>>24)!=0x21) return FAIL;

/* 2. GCONF：pdn_disable=1（UART 模式必备）+ mstep_reg_select=1（微步走寄存器） */
TMC2209_WriteReg(0x00, 0x000000C0);   /* bit6 pdn_disable, bit7 mstep_reg_select */

/* 3. 电流：IRUN=24, IHOLD=12, IHOLDDELAY=2 */
TMC2209_WriteReg(0x10, (2u<<16)|(24u<<8)|12u);

/* 4. CHOPCONF：MRES=5(1/8微步, bit27:24), intpol=1(bit28), toff=3 */
TMC2209_WriteReg(0x6C, 0x15000003);

/* 5. 读回校验：确认写帧被接受 */
TMC2209_ReadReg(0x02, &ifcnt);        /* 期望值 = 3+ */
TMC2209_ReadReg(0x6C, &snap.chopconf);/* 读回 CHOPCONF 与写入值比对 */
```

> 上电默认值参考（数据手册）：GSTAT.reset=1、IFCNT=0、IOIN.VERSION=0x21、CHOPCONF/PWMCONF 为 OTP 出厂值、DRV_STATUS 无故障位。

---

## 六、注意事项

1. **pdn_disable 必须=1**：启用 UART1 后 PDN_UART 不再接地，若不置该位，PDN 的静止降流功能会与通信冲突。
2. **I_scale_analog=0**：用内部 2.5V 基准（VREF 开路），电流由 IRUN/IHOLD 数字设定；若 VREF 分压则 I_scale_analog=1。
3. **MRES 与引脚 MS1/MS2**：置 `mstep_reg_select=1` 后 MRES 生效；否则由引脚决定。
4. **toff=0 会关闭驱动**（所有桥关断），可作为软件使能开关（比 ENN 更直接）。
5. **DRV_STATUS 位序**：otpw 在 bit0（勿与 TMC2208/TMC5160 混淆）。
6. **通信校验**：用 IFCNT 增量确认写帧被接受；读 VERSION(0x21) 确认通路。
7. **USART1 波特率已为 115200**（CubeMX .ioc 已配置，与 TMC2209 自动波特率检测匹配）。

