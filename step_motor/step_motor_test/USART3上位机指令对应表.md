# USART3 上位机快速设置功能对应表
> **接口**：USART3（PB10=TX / PB11=RX），115200-8-N-1
> **指令格式**：`命令 [参数]`，以 `\r\n` 结束（无回显输入回显）
> **状态**：✅=已在 `uart_control.c` 实现；📋=建议后续新增

---

## 一、通信基础

| 项目 | 值 |
|---|---|
| 串口 | USART3，PB10(TX)/PB11(RX) |
| 波特率 | 115200，8 数据位，无校验，1 停止位 |
| 指令结束符 | `\r\n`（也接受单独 `\n`） |
| 应答结束符 | `\r\n` |
| 编码 | ASCII 文本 |

**应答格式**：
```
成功：OK[:数据]
失败：ERR[:原因]
```

---

## 二、功能对应表（主表）

| # | 功能 | 指令 | 参数/格式 | 示例 | 应答示例 | 说明 |
|---|---|---|---|---|---|---|
| 1 | **缓升启动/变速** | `START` | `START <freq>` | `START 800` | `OK START` | freq：100~20000Hz，缓升逼近 |
| 2 | **缓降停止** | `STOP` | `STOP` | `STOP` | `OK STOP` | 缓降到起始低速后停机 |
| 3 | **查询状态** | `STATUS` | `STATUS` | `STATUS` | `OK:800Hz RUN` | 当前频率 + RUN/STOP |
| 4 | **指令帮助** | `HELP` | `HELP` | `HELP` | `CMD: START...` | 指令列表 |
| 5 | **设置方向** | `DIR` | `DIR <0/1>` | `DIR 1` | `OK DIR=1` | 建议停机后设置 |
| 6 | **设置微步** | `MICRO` | `MICRO <mres>` | `MICRO 5` | `OK MICRO=5(1/8)` | mres：0=256…5=8…8=fullstep |
| 7 | **设置运行电流** | `IRUN` | `IRUN <n>` | `IRUN 24` | `OK IRUN=24` | 0~31（1/32~32/32），建议 16~31 |
| 8 | **设置静止电流** | `IHOLD` | `IHOLD <n>` | `IHOLD 12` | `OK IHOLD=12` | 0~31 |
| 9 | **斩波模式** | `CHOP` | `CHOP <0/1>` | `CHOP 0` | `OK CHOP=StealthChop` | 0=StealthChop，1=SpreadCycle |
| 10 | **PWM 频率** | `PWMFREQ` | `PWMFREQ <0-3>` | `PWMFREQ 2` | `OK PWMFREQ=2` | PWMCONF.bit17:16，见 2.3 |
| 11 | **TOFF 关断时间** | `TOFF` | `TOFF <0-15>` | `TOFF 5` | `OK TOFF=5` | CHOPCONF.bit3:0，StealthChop推荐2~3 |
| 12 | **HDEC 滞环低值** | `HDEC` | `HDEC <0-15>` | `HDEC 5` | `OK HDEC=5` | CHOPCONF.bit10:7（HEND），HEND+HSTRT≤16 |
| 13 | **读 StallGuard** | `SG` | `SG` | `SG` | `OK:512` | 0~510，越大负载越小 |
| 14 | **读驱动状态** | `DRV` | `DRV` | `DRV` | `OK:0x00000000` | DRV_STATUS 十六进制 |
| 15 | **读 IOIN** | `IOIN` | `IOIN` | `IOIN` | `OK:VERSION=0x21` | 含芯片版本 |
| 16 | **读寄存器** | `REGR` | `REGR <addr>` | `REGR 6C` | `OK:0x15000003` | addr 十六进制（0x00~0x70） |
| 17 | **写寄存器** | `REGW` | `REGW <addr> <val>` | `REGW 00 C0` | `OK REGW` | val 为 32 位十六进制 |

### 2.2 参数对应配置详表

**① `START <freq>` — 频率 / 转速换算**（1.8° 电机，200 全步/转）

| freq (Hz) | 1/8 微步 | 1/16 微步 | 1/32 微步 |
|---|---|---|---|
| 200 | 7.5 RPM | 3.75 RPM | 1.9 RPM |
| 800 | 30 RPM | 15 RPM | 7.5 RPM |
| 2000 | 75 RPM | 37.5 RPM | 18.8 RPM |
| 8000 | 300 RPM | 150 RPM | 75 RPM |

> 公式：`RPM = freq / (微步数 × 200) × 60`。范围 100~20000Hz；运行中可变速（缓升逼近）。

**② `MICRO <mres>` — 微步映射**

> **支持两种格式**：整数 `MICRO <mres>`（0~8）或**分数** `MICRO 1/4`（1/2~1/256）。分数自动换算：`mres = 8 - log2(div)`。

| mres | 微步 | 分数写法 | 每 STEP 推进 | 说明 |
|---|---|---|---|---|
| 0 | 1/256 | `MICRO 1/256` | 0.007° | 最精细 |
| 1 | 1/128 | `MICRO 1/128` | 0.014° | |
| 2 | 1/64 | `MICRO 1/64` | 0.028° | |
| 3 | 1/32 | `MICRO 1/32` | 0.056° | |
| 4 | 1/16 | `MICRO 1/16` | 0.112° | |
| **5** | **1/8** | `MICRO 1/8` | **0.225°** | 默认（与引脚一致） |
| **6** | **1/4** | **`MICRO 1/4`** | **0.45°** | 高转速/高力矩 |
| 7 | 1/2 | `MICRO 1/2` | 0.9° | |
| 8 | fullstep | `MICRO 1/1` | 1.8° | 整步 |

> 需 `GCONF.mstep_reg_select=1`（初始化已置位）；`intpol=1` 时内部仍按 256 细分插值。

**③ `IRUN <n>` / `IHOLD <n>` — 电流刻度对应**

电流比例 = `(n+1)/32`；实际电流 = `(n+1)/32 × 0.325V / (RSENSE+20mΩ) / √2`

| n | 电流比例 | RSENSE=0.15Ω 时实际电流 |
|---|---|---|
| 8 | 9/32 | ≈0.39 A |
| 16 | 17/32 | ≈0.73 A |
| **24** | **25/32** | **≈1.06 A**（默认 IRUN） |
| 31 | 32/32 | ≈1.35 A（满量程） |

> 建议 `IRUN` 16~31（保证微步精度）；`IHOLD` 为静止电流（stst 后生效，约 40% 合理）；`IHOLD=0` 时可配合 `CHOPCONF.freewheel` 实现自由滑行/被动制动。

**④ `CHOP <0/1>` — 斩波模式**

| 值 | 模式 | 特点 | 适用 |
|---|---|---|---|
| 0 | **StealthChop2** | 静音、自动调节、低速扭矩优 | 低速/静音场合（默认） |
| 1 | **SpreadCycle** | 高动态、周期电流控制 | 高速/大力矩（建议 toff=5,hstrt=5） |

> 注意：SPREAD 引脚为高会**取反**该位。

**④b `PWMFREQ <0-3>` — PWM 频率**（PWMCONF.bit17:16，数据手册 Rev.1.03）

| 值 | 编码 | fPWM（fCLK=12MHz） | 说明 |
|---|---|---|---|
| 0 | %00 | 2/1024 fCLK ≈ 23.4 kHz | 最低频，纹波最大，StealthChop 静音性稍差 |
| **1** | **%01** | **2/683 fCLK ≈ 35.1 kHz** | **默认**，均衡 |
| 2 | %10 | 2/512 fCLK ≈ 46.9 kHz | 高频，纹波小 |
| 3 | %11 | 2/410 fCLK ≈ 58.5 kHz | 最高频，开关损耗最大 |

> 数据手册建议：**使用能给出良好效果的最低设置**（20~40kHz 之间）。频率越高纹波越小但开关损耗越大、发热越多。`PWMFREQ` 通过读改写 PWMCONF 仅改 bit17:16，其余位（autoscale/autograd/REG/OFS 等）保持不变。

**④c `TOFF <0-15>` / `HDEC <0-15>` — 斩波参数**（CHOPCONF）

| 指令 | 位 | 含义 | 推荐值 |
|---|---|---|---|
| `TOFF` | bit3:0 | 慢衰减关断时间（限制最高斩波频率） | StealthChop: **2~3**；SpreadCycle: **5** |
| `HDEC` | bit10:7 | HEND 滞环低值（-3~+12，越大纹波越平滑） | **0**（默认）；SpreadCycle 高动态可试 4~8 |

> ⚠️ 数据手册要求 **HEND + HSTRT ≤ 16**（当前 HSTRT=0 恒定，HDEC≤15 均满足）。
> ⚠️ **`TOFF 0` 会关闭所有桥（软件禁用）**——非预期操作。
> 两条指令均读改写 CHOPCONF，保留 MRES/intpol/TBL 等其余位；读失败自动用初始化默认值兜底。

**⑤ `DIR <0/1>` — 方向**
- `0`=正向（DIR 引脚低），`1`=反向（高）；建议停机后设置。

**⑥ 寄存器类（SG/DRV/IOIN/REGR/REGW）**
- 值即寄存器原始值/原始位定义，详见《TMC2209_UART寄存器配置函数设计.md》第三章。`REGR/REGW` 地址/值均为十六进制。

---

## 三、实现状态

> 下表指令已在 `uart_control.c` 中全部实现（`UART_Control_Parse()`），底层函数（`TMC2209_SetCurrent/SetMicrosteps/SetChopperMode/GetStallResult/ReadStatus/ReadIOIN`）与 `Motor_SetDir` 均已就绪。

| 指令 | 状态 | 对应函数 |
|---|---|---|
| `START`/`STOP`/`STATUS`/`HELP` | ✅ 已实现 | `Motor_Start/Stop/GetSpeedHz/IsRunning` |
| `DIR` | ✅ 已实现 | `Motor_SetDir`（PA2 GPIO） |
| `MICRO` | ✅ 已实现 | `TMC2209_SetMicrosteps` |
| `IRUN`/`IHOLD` | ✅ 已实现 | 读改写 `TMC2209_ReadReg/WriteReg(0x10)` |
| `CHOP` | ✅ 已实现 | `TMC2209_SetChopperMode` |
| `SG` | ✅ 已实现 | `TMC2209_GetStallResult` |
| `DRV` | ✅ 已实现 | `TMC2209_ReadStatus` |
| `IOIN` | ✅ 已实现 | `TMC2209_ReadIOIN` |
| `REGR`/`REGW` | ✅ 已实现 | `TMC2209_ReadReg/WriteReg`（调试用） |

---

## 四、指令解析实现建议（新增功能时）

```c
static void UART_Control_Parse(const char *cmd)
{
  char s[48];
  if (strncmp(cmd, "START", 5) == 0) { ... }
  else if (strncmp(cmd, "DIR", 3) == 0)
  {
    int d = atoi(cmd + 4);
    /* 停机后设置 DIR 引脚，如需要 */
    sprintf(s, "OK DIR=%d\r\n", d); UART_Control_SendString(s);
  }
  else if (strncmp(cmd, "MICRO", 5) == 0)
  {
    uint8_t mres = (uint8_t)atoi(cmd + 6);
    TMC2209_SetMicrosteps(mres);
    sprintf(s, "OK MICRO=%u\r\n", (unsigned)mres); UART_Control_SendString(s);
  }
  else if (strncmp(cmd, "IRUN", 4) == 0)
  {
    uint8_t irun = (uint8_t)atoi(cmd + 5);
    TMC2209_SetCurrent(irun, 12u, 2u);   /* IHOLD/IHOLDDELAY 保持 */
    sprintf(s, "OK IRUN=%u\r\n", (unsigned)irun); UART_Control_SendString(s);
  }
  else if (strncmp(cmd, "REGR", 4) == 0)
  {
    uint32_t val;
    if (TMC2209_ReadReg((uint8_t)strtol(cmd+5, NULL, 16), &val))
      sprintf(s, "OK:0x%08lX\r\n", (unsigned long)val);
    else sprintf(s, "ERR\r\n");
    UART_Control_SendString(s);
  }
  /* ... 其余指令同理 */
}
```

---

## 五、注意事项

1. **参数范围**：START 的 freq 建议 100~20000；IRUN/IHOLD 0~31；MICRO 0~8；越界自动限幅或报 `ERR`。
2. **方向设置**：DIR 建议在电机停止后设置，避免运行中换向冲击。
3. **REGR/REGW 为调试指令**：地址/值用十六进制（如 `REGR 6C`、`REGW 40 80`）；只写寄存器（如 0x10）读回无意义。
4. **命令间隔**：连续指令间留 ≥10ms（上位机侧），保证 USART3 轮询收完一帧。
5. **大写**：指令大小写敏感（统一大写）。
6. **与 TMC2209 区分**：USART3（PB10/PB11）只连上位机，**不要**与 TMC2209 的 PDN_UART 混接。
