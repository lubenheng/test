# ADC PGA Demo 代码配置分析报告

> 芯片：ODS32M501x (Cortex-M0+)
> 项目：`adc_pga_demo` — TIMER13 触发 ADC 采样 CSA/PGA 运放输出
> 日期：2026-08-04

---

## 一、CSA 配置专项分析

### 1.1 权威寄存器位域（以 UM 文档 MISC_CSACR0 Table 368 为准）

MISC_CSACR0 (`0x40007810`) 在 UM 中的完整位域定义：

| 位域 | 复位 | 名称 | 描述 |
|------|------|------|------|
| [0] | 0 | RW_ANA_CSA_EN | CSA 电流采样模块使能: 1=使能, 0=关闭 |
| [1] | 1 | RW_CSA_AMP_EN | CSA 运放使能: 1=使能, 0=不使能 |
| [3:2] | 0 | RW_CSA_MODE | CSA 工作模式 |
| [5:4] | 0 | RW_CSA_CSP | 同相输入: `00=GPIO7, 01=GPIO9, 10=GPIO11, 11=GND` |
| [7:6] | 0 | RW_CSA_CSN | 反相输入: `00=GPIO8, 01=GPIO10, 10=GPIO12, 11=GND` |
| [10:8] | 0 | RW_CSA_GAIN | `000=1x, 001=2x, 010=4x, 011=8x, 100=12x, 101=16x, 110=32x, 111=64x` |
| [11] | 1 | RW_CSA_DAC_EN | DAC 使能: 1=使能, 0=不使能 |
| [16:12] | 0x1F | RW_CSA_REF | CSA 偏置电压 |
| [22:20] | 0 | RW_CSA_DAC_REF | DAC 量程 |
| [31:24] | 0 | RW_CSA_DAC_OUT | DAC 输入信号 |

> **UM 布局与 `CSA_Reg.h` (`CSA_TypeDef`) 对比**：bit0~bit7 完全一致（`CSA_EN`=ANA_CSA_EN, `AMP_EN`=AMP_EN, `MODE`=MODE, `CSP`=CSP, `CSN`=CSN）。唯一差异是 `CSA_Reg.h` 将 bit[11:8] 全部定义为 `CSA_GAIN`（4bit），而 UM 将 bit[11] 独立为 `DAC_EN`、bit[10:8] 为 `CSA_GAIN`（3bit）。以下分析**以 UM 为准**。

---

### 1.2 代码实现分析（`CSA_Init()`）

`CSA_Init()` **两次写入同一寄存器**（均为 `0x40007810 = CSA_BASE`）：

#### 第1次：硬编码直接写入 `0x79`

```c
(*(volatile uint32_t *)0x40007810UL) = (1UL << 0)   // bit0=1  → 代码注释称 CSA_AMP_EN
                                     | (0UL << 1)   // bit1=0
                                     | (3UL << 3)   // bit4:3=11 → 代码注释称 CSA_CSN=GND
                                     | (3 << 5);    // bit7:5=011 → 代码注释称 CSA_GAIN=x8
// 写入值 = 0x00000079
```

代码注释使用的是**完全错误的 ANAC_CSA1 位域布局**（bit0=AMP_EN, bit2:1=CSP, bit4:3=CSN, bit7:5=GAIN）。

**以 UM 权威布局解码 `0x79`（0b01111001）**：

| 位域（UM） | bit | 值 | 实际含义 |
|------------|-----|-----|----------|
| ANA_CSA_EN | [0] | 1 | CSA 模块使能 ✅ |
| AMP_EN | [1] | 0 | **运放禁用** ❌ |
| MODE | [3:2] | 10 | **MODE=2（非固定通道模式）** ❌ |
| CSP | [5:4] | 11 | **CSP=GND（同相接GND）** ❌ |
| CSN | [7:6] | 01 | **CSN=GPIO10** ❌ |
| GAIN | [10:8] | 000 | **增益=1x** ❌ |
| DAC_EN | [11] | 0 | **DAC禁用** ❌ |

**结论：第1次硬编码写入由错误的位域注释产生，按 UM 布局解码是"CSA运放禁用、正端接地、负端接GPIO10、1倍增益"——完全无效的配置。**

#### 第2次：HAL API 逐位写入（修复配置）

```c
CSA_ENABLE(ENABLE);                    // → 设置 bit[0] ANA_CSA_EN=1
CSA->CR0.CSA_AMP_EN = 1;              // → 设置 bit[1] AMP_EN=1
CSA_InputCH_SWMode(CSA_FIXED_CH_MODE); // → 设置 bit[3:2] MODE=00
CSA->CR0.CSA_CSP = 0;                 // → 设置 bit[5:4] CSP=00 (GPIO7)
CSA->CR0.CSA_CSN = 3;                 // → 设置 bit[7:6] CSN=11 (GND)
CSA_SET_PGAGAIN(CSA_PGA_GAIN_X8);     // → 设置 bit[11:8]=0011
```

`CSA_Reg.h` 的 `CSA_TypeDef` 位域与 UM 的 bit0~bit9 完全一致。每个位域赋值执行 RMW，逐位覆盖第1次写入的错误值。

#### 最终寄存器值推导（RMW 逐步跟踪）

1. 硬编码 `0x79` → 寄存器 = `0x79`（错误）
2. `CSA_ENABLE` set bit0 → `0x79`（不变）
3. `AMP_EN` set bit1 → `0x7B`
4. `MODE` clear bits3:2 → `0x73`
5. `CSP` clear bits5:4 → `0x63`
6. `CSN` set bits7:6=11 → `0xE3`
7. `GAIN` set bits11:8=0011 → **`0x0309`**

**最终 HAL 输出 `0x00000309`，各字段正确。第1次硬编码写入被逐位覆盖，是死代码。**

### 1.3 🔴 `CSA_SET_PGAGAIN` 与 UM 的 GAIN/DAC_EN 位域冲突

`CSA_Reg.h` 将 `CSA_GAIN` 定义为 4bit `[11:8]`，但 UM 定义为 `GAIN[10:8]` + `DAC_EN[11]`。`CSA_SET_PGAGAIN(0x03)` 写入 bit11:8=0011：

| 按 CSA_Reg.h | 按 UM | 影响 |
|-------------|-------|------|
| GAIN[11:8]=0011 → 8x | GAIN[10:8]=011 → 8x ✅, DAC_EN[11]=0 ❌ | DAC 被意外禁用 |

**UM 中 DAC_EN 复位值为 1**，但 `CSA_SET_PGAGAIN` 将其写成 0。若 CSA 放大器的 DAC 偏置需要此使能位，则可能影响精度。但目前 CH7 采样正常，说明 DAC 偏置在此场景下非必需或硅片实现与 UM 有差。

### 1.4 配置正确性汇总

| 位域（UM） | 最终 HAL 写入值 | 期望值 | 状态 |
|-----------|---------------|--------|------|
| ANA_CSA_EN | 1 | 1 | ✅ |
| AMP_EN | 1 | 1 | ✅ |
| MODE | 00 (FIXED) | 00 | ✅ |
| CSP | 00 (GPIO7) | GPIO7 | ✅ |
| CSN | 11 (GND) | GND | ✅ |
| GAIN | 011 (x8) | x8 | ✅ |
| DAC_EN | 0 | 1 (复位值) | ⚠️ 被 GAIN 位域覆盖 |

### 1.5 MISC_CSACR1 配置

MISC_CSACR1 (`0x40007814`) 包含 CSA 输出滤波和过流检测配置，**代码未显式配置，使用复位值**：

| 位域 | 复位值 | 含义 |
|------|--------|------|
| RW_CSA_LPF | `00` | 输出滤波截止频率 = 200Hz |
| RW_CSA_LPF_SEL | `0` | 片上电容滤波 |
| RW_CSA_OC_EN | `1` | 过流检测使能 |
| RW_CSA_OC_SEL | `000` | 检测信号源 = CSP0 |

### 1.6 CSA 增益对应表

| 宏定义 | 值 | 增益 |
|--------|-----|------|
| `CSA_PGA_GAIN_X1` | 0x00 | 1x |
| `CSA_PGA_GAIN_X2` | 0x01 | 2x |
| `CSA_PGA_GAIN_X4` | 0x02 | 4x |
| `CSA_PGA_GAIN_X8` | 0x03 | 8x |
| `CSA_PGA_GAIN_X12` | 0x04 | 12x |
| `CSA_PGA_GAIN_X16` | 0x05 | 16x |
| `CSA_PGA_GAIN_X32` | 0x06 | 32x |
| `CSA_PGA_GAIN_X64` | 0x07 | 64x |

当前配置 `CSA_PGA_GAIN_X8 = 0x03`，与 UM 表一致。

---

## 二、ADC 采样通道配置分析

### 2.1 采样序列（`adc_timer_trig_csa_demo()`）

```c
ADC_SEQx_CFG(ADC_SEQ0, ADC_CH0);  // CHSEQx=0 → CSA_VSEN
ADC_SEQx_CFG(ADC_SEQ1, ADC_CH1);  // CHSEQx=1 → CSA_VSEN
ADC_SEQx_CFG(ADC_SEQ2, ADC_CH2);  // CHSEQx=2 → CSA_VSEN
ADC_SEQx_CFG(ADC_SEQ3, ADC_CH7);  // CHSEQx=7 → PC3 (外部GPIO)
```

### 2.2 ADC_MUX 通道映射表（UM Table 330）

| CHSEQx | ADC_MUX（信号源） |
|--------|-------------------|
| 00000 (0) | **CSA_VSEN** |
| 00001 (1) | **CSA_VSEN** |
| 00010 (2) | **CSA_VSEN** |
| 00011 (3) | **CSA_VLPF** |
| 00100 (4) | PB12 |
| 00101 (5) | PB13 |
| 00110 (6) | PB14 |
| 00111 (7) | **PC3** ← ADC_CH7 |
| 01000 (8) | PC4 |
| ... | ... |
| 01101 (13) | VDD33_VSEN |
| 01110 (14) | TJ_REF |
| 01111 (15) | VREF_1P20 |
| 10000 (16) | PA0 |
| ... | ... |
| 11101 (29) | PA13 |
| 11110 (30) | PA14 |

### 2.3 问题：CH0/CH1/CH2 全部采样同一信号

| ADC序列 | CHSEQx | 实际信号源 | 预期 |
|---------|--------|-----------|------|
| SEQ0 | 0 | CSA_VSEN | CH0 |
| SEQ1 | 1 | **CSA_VSEN**（同SEQ0） | CH1（实际同CH0） |
| SEQ2 | 2 | **CSA_VSEN**（同SEQ0） | CH2（实际同CH0） |
| SEQ3 | 7 | PC3（滑变测试） | ✅ CH7 正确 |

**根因**：CSA 工作在固定通道模式（`CSA_FIXED_CH_MODE`），只有一路运放输出。ADC 的 CHSEQx=0/1/2 均映射到 `CSA_VSEN`，三次采样读出同一电压值。

### 2.4 其他发现

| 问题 | 详情 |
|------|------|
| PB12 GPIO 配置但未采样 | `adc_gpio_init()` 配置 PB12 → ADC_CH4，但采样序列无 CH4 |
| PTA8 注释错误 | 注释标注"CSA_CSP 反相输入端"，实际 CSA_CSP 是正相输入（PTA7），PTA8 未使用 |
| `__enable_irq()` 被注释 | `main.c:68`，但 CH7 采样正常说明启动代码已使能 |
| `stru_AdcTest_OL` 死代码 | 结构体定义在 `.c:34-52` 但未被使用 |
| TIM13 ISR 未清中断标志 | `isr.c:275` 读状态后未写 1 清零 |
| ADC ISR 先清标志后读数据 | `isr.c:346-348` 先 `S0CDIF=1` 再 `ADC_GetResult()` |

---

## 三、TIMER13 触发 ADC 采样时序

```c
#define TIMER_ADC_DIV  TIMER_DIV_8
#define TIMER_ADC_TH   3999
// 采样率 = 32MHz / 8 / 3999 ≈ 1kHz
```

| 参数 | 值 |
|------|-----|
| 定时器时钟 | 32MHz |
| 分频 | DIV_8 → 4MHz |
| 重装载值 TH | 3999 |
| 采样率 | ≈ 1kHz |
| 触发模式 | TIMERx_MODE_PWM |
| 触发事件 | TIMER13_CH0_CAPCOM_EVT |

---

## 四、ADC 时序参数

```c
ADC_SetCLK_STPT(4);      // START脉冲宽度
ADC_SetCLK_COVT(2);      // 转换时间
ADC_SetCLK_INIT(0);      // 初始化延时
ADC_SetCLK_SAPT(13);     // 采样完成后MUX切换时间
ADC_SetCLK_TrigDelay(2); // 触发延迟
```

*注：具体时间取决于 ADC 工作时钟频率（32MHz 或 96MHz）。*

---

## 五、零偏校准流程（ISR）

```
初始化 → 采集64次采样 → 求平均零偏 → 校准完成标识 → 主循环打印结果
```

| 参数 | 值 |
|------|-----|
| 校准采样次数 | `CALIB_SAMPLES = 64` |
| 校准通道 | CH0/CH1/CH2/CH7 |
| 电压换算 | `mV = (raw - offset) * 2500 / 4095` |
| 参考电压 | 2.5V (假设) |
| ADC 分辨率 | 12bit (0~4095) |

---

## 六、三条硬编码寄存器赋值分析

### 6.1 地址解码

| 地址 | 基址分解 | 寄存器 |
|------|----------|--------|
| `0x40015804` | `APB2_BASE(0x40010000) + 0x5804` | 疑似 **I2C2_TAR**（I2C2 目标地址寄存器） |
| `0x4800083C` | `GPIOC_BASE(0x48000800) + 0x3C` | **GPIOC_AF0**（PC0~PC7 复用功能） |
| `0x48000840` | `GPIOC_BASE(0x48000800) + 0x40` | **GPIOC_AF1**（PC8~PC15 复用功能） |

### 6.2 GPIOC_AF0 = `0xfff00000`

每 4bit 控制一个引脚（3bit AF + 1bit 保留）：

| 引脚 | bit位域 | 值 | 复用功能 |
|------|---------|-----|----------|
| PC0~PC5 | [2:0]~[22:20] | `000` | ALT_FUNC_0 |
| PC6 | [26:24] | **`111`** | **ALT_FUNC_7** |
| PC7 | [30:28] | **`111`** | **ALT_FUNC_7** |

### 6.3 GPIOC_AF1 = `0x0000000f`

| 引脚 | bit位域 | 值 | 复用功能 |
|------|---------|-----|----------|
| PC8 | [2:0] | **`111`** | **ALT_FUNC_7** |
| PC9~PC15 | [6:4]~[30:28] | `000` | ALT_FUNC_0 |

> PC6、PC7、PC8 被配置为 **ALT_FUNC_7**（具体功能需查 GPIO 复用表单确认，可能是 I2C2 的 SCL/SDA 或其他外设）。

### 6.4 `0x40015804 = 0x11`

推测为 I2C2 的 TAR（目标地址）寄存器，写入 `0x11` 表示设置从设备地址。

---

## 七、PA7~PA12 配置为模拟功能 ALT_FUNC_1

### 7.1 寄存器地址

| 地址 | 寄存器 | 描述 |
|------|--------|------|
| `0x4800003C` | PA_AF0 | PA0~PA7 复用功能配置 |
| `0x48000040` | PA_AF1 | PA8~PA15 复用功能配置 |
| `0x4800004C` | PA_AFC | 复用功能提交/锁定 |
| `0x48000050` | PA_ANA | 模拟功能使能 |

### 7.2 寄存器位域布局

**PA_AF0**：每引脚 4bit（3bit AF + 1bit 保留）

| 引脚 | bit位域 |
|------|---------|
| PA0 | [2:0] |
| PA1 | [6:4] |
| PA2 | [10:8] |
| PA3 | [14:12] |
| PA4 | [18:16] |
| PA5 | [22:20] |
| PA6 | [26:24] |
| **PA7** | **[30:28]** |

**PA_AF1**：每引脚 4bit

| 引脚 | bit位域 |
|------|---------|
| **PA8** | **[2:0]** |
| **PA9** | **[6:4]** |
| **PA10** | **[10:8]** |
| **PA11** | **[14:12]** |
| **PA12** | **[18:16]** |
| PA13 | [22:20] |
| PA14 | [26:24] |
| PA15 | [30:28] |

**PA_ANA / PA_AFC**：每引脚 1bit

| 寄存器 | bit0~bit15 对应 |
|--------|----------------|
| PA_ANA | PA0~PA15 模拟使能 |
| PA_AFC | PA0~PA15 复用提交 |

### 7.3 配置值计算

#### PA_AF0 = `0x10000000`

```
bit[30:28] = 0b001 → PA7 = ALT_FUNC_1
其余位 = 0 → PA0~PA6 = ALT_FUNC_0
```

#### PA_AF1 = `0x00011111`

```
bit[18:16] = 0b001 → PA12 = ALT_FUNC_1
bit[14:12] = 0b001 → PA11 = ALT_FUNC_1
bit[10:8]  = 0b001 → PA10 = ALT_FUNC_1
bit[6:4]   = 0b001 → PA9  = ALT_FUNC_1
bit[2:0]   = 0b001 → PA8  = ALT_FUNC_1
其余位 = 0 → PA13~PA15 = ALT_FUNC_0
```

#### PA_ANA = `0x00001F80`

```
bit[12:7] = 0b111111 → PA7~PA12 模拟输入使能
= 0x1F80
```

#### PA_AFC = `0x00001F80`

```
与 PA_ANA 相同，PA7~PA12 对应位写1提交AF配置。
```

### 7.4 汇总

| 地址 | 寄存器 | 写入值 | 作用 |
|------|--------|--------|------|
| `0x4800003C` | **PA_AF0** | `0x10000000` | PA7 → ALT_FUNC_1 |
| `0x48000040` | **PA_AF1** | `0x00011111` | PA8~PA12 → ALT_FUNC_1 |
| `0x4800004C` | **PA_AFC** | `0x00001F80` | 提交 PA7~PA12 的复用更改 |
| `0x48000050` | **PA_ANA** | `0x00001F80` | 使能 PA7~PA12 模拟输入模式 |

### 7.5 代码形式

```c
*(volatile uint32_t *)0x4800003C = 0x10000000;  // PA_AF0
*(volatile uint32_t *)0x48000040 = 0x00011111;  // PA_AF1
*(volatile uint32_t *)0x4800004C = 0x00001F80;  // PA_AFC (commit)
*(volatile uint32_t *)0x48000050 = 0x00001F80;  // PA_ANA (analog enable)
```

### 7.6 CSA 引脚对应关系

| GPIO | CSA 功能 | 说明 |
|------|----------|------|
| PA7 | CSP0 | 同相输入端 0 |
| PA8 | CSN0 | 反相输入端 0 |
| PA9 | CSP1 | 同相输入端 1 |
| PA10 | CSN1 | 反相输入端 1 |
| PA11 | CSP2 | 同相输入端 2 |
| PA12 | CSN2 | 反相输入端 2 |

---

## 八、总结

| 等级 | 问题 | 状态 |
|------|------|------|
| 🔴 | CSA_Init() 第1次硬编码写入使用错误位域布局 | 死代码，建议删除 |
| 🔴 | CH0/CH1/CH2 全部采样 CSA_VSEN 同一信号（固定通道模式） | 需确认是否为预期行为 |
| 🟡 | `CSA_SET_PGAGAIN` 意外覆盖 UM 的 DAC_EN 位 | HAL 位域与 UM 定义有差 |
| 🟡 | PB12 GPIO 配置但未采样 | 冗余 |
| 🟡 | PTA8 注释"反相输入端"有误 | 注释错误 |
| 🟡 | `__enable_irq()` 被注释 | 启动代码已使能，可清理 |
| 🟢 | `stru_AdcTest_OL` 死代码 | 建议删除 |
| 🟢 | TIM13 ISR 未显式清中断标志 | 需确认驱动行为 |
| 🟢 | ADC ISR 先清标志后读数据 | 低速率无影响 |
| ✅ | CSA 硬件配置最终值（CSP/CSN/GAIN/MODE） | 正确 |
| ✅ | CH7 外部滑变测试通道 | 工作正常 |
| ✅ | ADC 时序参数 | 合理 |
| ✅ | 零偏校准算法 | 正确 |

---

## 九、`adc_gpio_init()` 的 GPIO 配置与模拟复用功能 ALT_FUNC_1 冲突分析

### 9.1 问题背景

`adc_gpio_init()` 使用 HAL API `gpio_init()` 将 PA7、PA8 配置为：

```c
setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;   // AF = 0（GPIO 模式）
setting_cfg.gpio_dir = GPIO_DIR_INPUT;     // 数字输入模式
setting_cfg.pull_cfg = GPIO_PULL_DISABLE;  // 无上下拉
```

而 CSA 运放的模拟输入引脚（PA7~PA12）理想配置应为 **`GPIO_ALT_FUNC_1` + `PA_ANA` 使能**。

### 9.2 两种配置方式对比

| 配置项 | ALT_FUNC_0 + INPUT + NOPULL | ALT_FUNC_1 + ANA（推荐） |
|--------|----------------------------|--------------------------|
| AF 寄存器值 | `000`（GPIO 模式） | `001`（模拟复用1） |
| 数字施密特触发器 | **开启** | **关闭** |
| 模拟信号路径（PA_ANA） | **未使能** | **使能** |
| 引脚阻抗 | 高阻（无上下拉） | 高阻（纯模拟） |
| 信号对模拟精度影响 | 施密特触发器翻转噪声耦合 | 无数字干扰 |
| 静态功耗 | 略高（施密特触发器耗电） | 更低 |
| CSA 运放能否读到信号 | ✅ 可以（高阻态，电压可读） | ✅ 可以（最优） |

### 9.3 冲突判断

**存在逻辑冲突，但不一定导致当前功能失效：**

1. **AF 值冲突** — `ALT_FUNC_0` (=0x00) 与 `ALT_FUNC_1` (=0x01) 写同一个 PA_AF0/PA_AF1 寄存器域。`adc_gpio_init()` 先于 `CSA_Init()` 执行，但 `CSA_Init()` 不操作 PA_AF 寄存器，最终 AF 值为 `ALT_FUNC_0`。

2. **PA_ANA 未设置** — `gpio_init()` 不操作 PA_ANA 寄存器（`0x48000050`），代码中无任何位置写 PA_ANA。PA7 数字输入缓冲器始终开启。

3. **PA9~PA12 完全未配置** — `adc_gpio_init()` 仅配置 PA7/PA8。当前 CSA 只用 `CSP=GPIO7 + CSN=GND` 不影响运行；但切换到自动轮询或多通道时 PA9~PA12 处于未知状态。

### 9.4 为什么当前 CH7 采样正常

ALT_FUNC_0 + INPUT + NOPULL 的高阻态足以让 PA7 模拟电压被 CSA 运放读取。差异仅体现为几个 LSB 噪声、微量功耗和 EMC 性能。

### 9.5 推荐修改

```c
/*----------------------------------------------------------------------------
 * CSA 模拟引脚初始化: PA7~PA12 → ALT_FUNC_1 + Analog Enable
 *----------------------------------------------------------------------------*/
void csa_analog_pin_init(void)
{
    *(volatile uint32_t *)0x4800003C = 0x10000000;  // PA_AF0: PA7→ALT_FUNC_1
    *(volatile uint32_t *)0x48000040 = 0x00011111;  // PA_AF1: PA8~PA12→ALT_FUNC_1
    *(volatile uint32_t *)0x4800004C = 0x00001F80;  // PA_AFC: 提交 AF
    *(volatile uint32_t *)0x48000050 = 0x00001F80;  // PA_ANA: 使能模拟
}
```

或 HAL API 风格（需额外操作 PA_ANA）：

```c
gpio_settings_config_t cfg;
cfg.gpio_port = GPIO_A;
cfg.gpio_alt = GPIO_ALT_FUNC_1;
cfg.gpio_dir = GPIO_DIR_INPUT;
cfg.pull_cfg = GPIO_PULL_DISABLE;
cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
cfg.gpio_interrupt_cfg_info.int_handle = NULL;
for (int pin = 7; pin <= 12; pin++) {
    cfg.gpio_pin = (gpio_pin_e)pin;
    gpio_init(&cfg);
}
*(volatile uint32_t *)0x48000050 |= 0x00001F80;  // PA_ANA（HAL不做此操作）
```

### 9.6 总结

| 方面 | 当前状态 | 影响 |
|------|----------|------|
| AF 值 | ALT_FUNC_0 | 数字输入模式，非最优 |
| PA_ANA | 未使能 | 轻微模拟精度损失 |
| PA9~PA12 | 未初始化 | 固定通道模式无影响，切换多通道时会出问题 |
| 基本功能 | 正常 | CH7 正确，CSA 能读取 PA7 信号 |
| 建议 | 切换为 ALT_FUNC_1 + PA_ANA | 符合最佳实践，提升精度 |