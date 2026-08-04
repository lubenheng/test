# ODS32M501x TIMER触发ADC采样运放（CSA/PGA）配置与验证

---

## 一、TIMER触发ADC采样运放的配置步骤

当前项目使用 **TIMER13** 作为 ADC 的硬件触发源，触发 ADC 对 CSA 运放输出进行采样。完整配置流程（6步，以现有代码 `adc_timer_trig_csa_demo()` 为参考）：

### 第1步：GPIO初始化（模拟输入引脚）

在 `adc_gpio_init()` 中：
- **PTA7** = CSP0 (CSA同相输入端)，配置为模拟输入 `GPIO_ALT_FUNC_0`
- **PTA8** = CSN0 (CSA反相输入端)，配置为模拟输入 `GPIO_ALT_FUNC_0`
- **PTB12** = ADC_CH7 预留通道，配置为模拟输入 `GPIO_ALT_FUNC_0`
- **PTC6** = 测试输出引脚（用于示波器观察TIMER13触发频率）

```c
gpio_settings_config_t setting_cfg;
setting_cfg.gpio_alt      = GPIO_ALT_FUNC_0;
setting_cfg.gpio_dir      = GPIO_DIR_INPUT;
setting_cfg.pull_cfg      = GPIO_PULL_DISABLE;
setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
gpio_init(&setting_cfg);
```

### 第2步：CSA运放使能与配置

```c
void CSA_Init(void)
{
    // 2.1 使能ANAC模拟控制总开关及CSA模拟电路
    //     ANAC_ENCR(0x40007800): bit0=ANA_EN, bit17=ANA_CSA_EN
    (*(volatile uint32_t *)0x40007800UL) |= (1UL << 0) | (1UL << 17);

    // 2.2 使能数字侧CSA模块
    CSA_ENABLE(ENABLE);

    // 2.3 使能运放放大器
    CSA->CR0.CSA_AMP_EN = 1;

    // 2.4 设置为固定通道模式
    CSA_InputCH_SWMode(CSA_FIXED_CH_MODE);

    // 2.5 选择正/负输入端: CSP=GPIO7(00), CSN=GND(11)
    //     同相端接PTA7信号源，反相端接地
    CSA->CR0.CSA_CSP = 0;   // 同相端: CSP0
    CSA->CR0.CSA_CSN = 3;   // 反相端: 内部接地

    // 2.6 设置PGA增益
    CSA_SET_PGAGAIN(CSA_PGA_GAIN);  // 当前设为 x8
}
```

### 第3步：ADC模块初始化

```c
// 3.1 使能ADC时钟
ADC_Clk_Enable();

// 3.2 使能ADC模块
ADC_ENABLE(ENABLE);

// 3.3 设置采样组模式为模式1（单组模式）
ADC_SetSampleGrpMode(SampleGrpMode_1);

// 3.4 配置Group0为4通道采样
ADC_SetGrpChCount(ADC_Sample_Group_0, 4);
ADC_SetGrpChCount(ADC_Sample_Group_1, 0);
ADC_SetGrpChCount(ADC_Sample_Group_2, 0);
ADC_SetGrpChCount(ADC_Sample_Group_3, 0);

// 3.5 配置采样序列: SEQ0→CH0, SEQ1→CH1, SEQ2→CH2, SEQ3→CH7
ADC_SEQx_CFG(ADC_SEQ0, ADC_CH0);   // CSA通道0 → ADC_CH0
ADC_SEQx_CFG(ADC_SEQ1, ADC_CH1);   // CSA通道1 → ADC_CH1
ADC_SEQx_CFG(ADC_SEQ2, ADC_CH2);   // CSA通道2 → ADC_CH2
ADC_SEQx_CFG(ADC_SEQ3, ADC_CH7);   // 预留通道

// 3.6 设置触发源为 TIMER13_TRIG
ADC_SetGrpTrigSource(ADC_Sample_Group_0, ADC_GrpTrig_TMR13_TRIG);

// 3.7 配置ADC采样时钟时序
ADC_SetCLK_STPT(4);      // 采样启动延时
ADC_SetCLK_COVT(2);      // 转换时间
ADC_SetCLK_INIT(0);      // 初始化时间
ADC_SetCLK_SAPT(13);     // 采样时间
ADC_SetCLK_TrigDelay(2); // 触发延时

// 3.8 使能ADC Group0转换完成中断
ADC_Enable_IrqSrc(ADC_IRQSRC_S0CP);
NVIC_SetPriority(ADC_IRQn, 2);
NVIC_ClearPendingIRQ(ADC_IRQn);
NVIC_EnableIRQ(ADC_IRQn);
```

### 第4步：TIMER13初始化（产生周期性ADC触发信号）

```c
void TIMER_ADC_Init(void)
{
    // 4.1 清除TIMER13所有配置
    Timer_drv_Setting_Clean_all(TIMER13);

    // 4.2 设置时钟分频: 32MHz / 8 = 4MHz
    TIMER_Set_CLKDIV(TIMER13, TIMER_DIV_8);

    // 4.3 选择内部时钟源
    Timer_Set_ClkSrc(TIMER13, TIMERx_INNERCLK);

    // 4.4 自动重载模式
    Timer_Set_RLMode(TIMER13, TIMERx_AUTO_RELOAD);

    // 4.5 向上计数
    Timer_Set_CNTR_Dir(TIMER13, TIMERx_CNTR_UP);

    // 4.6 边沿对齐模式
    Timer_Set_Center_Mode(TIMER13, TIMERx_EDGE_MODE);

    // 4.7 禁用影子寄存器
    Timer_Set_Shadow(TIMER13, TIMERx_SHADOW_REG_DIS);

    // 4.8 CH0设置为PWM输出模式
    TIMER_Set_CH_Mode(TIMER13, TIMERx_CH0, TIMERx_MODE_PWM);

    // 4.9 设置周期值: TH=3998, 周期=3999 → 采样率 = 4MHz/3999 ≈ 1kHz
    Timer_Set_ThVal(TIMER13, 3999 - 1);

    // 4.10 设置比较值（占空比）
    Timer_Set_Comp0_Val(TIMER13, 1);

    // 4.11 设置ADC触发事件源: CH0比较匹配事件
    Timer_Set_EvtSrc(TIMER13, TIMER13_CH0_CAPCOM_EVT);

    // 4.12 使能TIMER13 CH0中断（可选，用于观察触发频率）
    Timer_IRQ_Enable(TIMER13, TIMERx_IRQ_CH0);
    NVIC_SetPriority(TIM13_IRQn, 1);
    NVIC_ClearPendingIRQ(TIM13_IRQn);
    NVIC_EnableIRQ(TIM13_IRQn);

    // 4.13 更新寄存器并启动定时器
    Timer_Reg_Update(TIMER13);
    Timer_Enable(TIMER13);
}
```

**采样率计算公式：**

```
采样率 = 32MHz / (CLK_DIV × (TH+1)) = 32MHz / (8 × 3999) ≈ 1kHz
```

> **时钟说明**：当前项目使用 FPGA 模式（`sys_config.h`），`SYS_CORE_CLK = HIRC_FREQ = 32MHz`。TIMER13 定时器时钟来源于系统内核时钟（HIRC），经 DIV_8 分频后得到 4MHz 计数时钟。

### 第5步：ADC中断服务（零偏校准 + 电压转换）

```c
void ADC_IRQHandler(void)
{
    static uint16_t calib_cnt = 0;
    static int64_t sum_ch0 = 0, sum_ch1 = 0, sum_ch2 = 0, sum_ch7 = 0;
    uint32_t raw0, raw1, raw2, raw3;

    // 5.1 读取中断源
    uint32_t irq_src = ADC_Get_IrqSrc();

    if (irq_src & (1UL << ADC_IRQSRC_S0CP))
    {
        // 5.2 写1清零中断标志（必须在读取数据前清除）
        ADC->IR.S0CDIF = 1;

        // 5.3 读取4个通道原始值
        raw0 = ADC_GetResult(0);
        raw1 = ADC_GetResult(1);
        raw2 = ADC_GetResult(2);
        raw3 = ADC_GetResult(3);

        // 5.4 前64次采样: 零偏校准（求平均偏移量）
        if (calib_cnt < CALIB_SAMPLES)
        {
            sum_ch0 += (int32_t)raw0;
            sum_ch1 += (int32_t)raw1;
            sum_ch2 += (int32_t)raw2;
            sum_ch7 += (int32_t)raw3;
            calib_cnt++;

            if (calib_cnt >= CALIB_SAMPLES)
            {
                g_offset_ch0 = (int32_t)(sum_ch0 / CALIB_SAMPLES);
                g_offset_ch1 = (int32_t)(sum_ch1 / CALIB_SAMPLES);
                g_offset_ch2 = (int32_t)(sum_ch2 / CALIB_SAMPLES);
                g_offset_ch7 = (int32_t)(sum_ch7 / CALIB_SAMPLES);
                g_calib_done = 1;
            }
        }
        else
        {
            // 5.5 校准后: mV = (raw - offset) * 2500 / 4095
            //     Vref=2.5V, 12位ADC分辨率=4096
            adc_csa_ch0_val     = (((int32_t)raw0 - g_offset_ch0) * 2500) / 4095;
            adc_csa_ch1_val     = (((int32_t)raw1 - g_offset_ch1) * 2500) / 4095;
            adc_csa_ch2_val     = (((int32_t)raw2 - g_offset_ch2) * 2500) / 4095;
            adc_reserved_ch_val = (((int32_t)raw3 - g_offset_ch7) * 2500) / 4095;
        }
    }
}
```

### 第6步：主循环调用顺序

```c
int main(void)
{
    // 6.1 系统时钟和外设时钟使能
    delay_1us(2000);
    flash_clk_enable();
    gpio_clk_enable();
    uart_clk_enable();
    TIM13_Clk_Enable();

    // 6.2 串口初始化（用于输出调试信息）
    uart0_gpio_init();
    uart_init(UART_CONSOLE_INSTANCE, &uart0_init_config);
    uart_transmit_enable(UART_CONSOLE_INSTANCE, true);

    // 6.3 GPIO初始化
    adc_gpio_init();

    // 6.4 CSA运放初始化
    CSA_Init();

    // 6.5 TIMER+ADC初始化（ADC中断使能在内部完成）
    adc_timer_trig_csa_demo();

    // 6.6 主循环: 等待校准完成后打印结果
    while (1)
    {
        if (g_calib_done)
        {
            print_all_channels();
        }
    }
}
```

---

## 二、手动设置固定增益的步骤

### 2.1 增益宏定义位置

在 `adc_demo_test.h` 中：

```c
#define CSA_PGA_GAIN   CSA_PGA_GAIN_X8   // 当前增益: x8
```

### 2.2 可用增益选项

| 宏定义 | 值 | 增益倍数 | 适用场景 |
|--------|-----|----------|----------|
| `CSA_PGA_GAIN_X1` | 0x00 | 1x | 大电流/小采样电阻 |
| `CSA_PGA_GAIN_X2` | 0x01 | 2x | |
| `CSA_PGA_GAIN_X4` | 0x02 | 4x | |
| `CSA_PGA_GAIN_X8` | 0x03 | 8x | 中等电流 |
| `CSA_PGA_GAIN_X12` | 0x04 | 12x | |
| `CSA_PGA_GAIN_X16` | 0x05 | 16x | |
| `CSA_PGA_GAIN_X32` | 0x06 | 32x | 小电流/大采样电阻 |
| `CSA_PGA_GAIN_X64` | 0x07 | 64x | 极小电流 |

### 2.3 修改固定增益的方法

只需修改 `adc_demo_test.h` 中的宏定义：

```c
// 例如修改为 x16 增益:
#define CSA_PGA_GAIN   CSA_PGA_GAIN_X16
```

`CSA_SET_PGAGAIN()` 函数会将增益值同时写入：
- **CSA->CR0.CSA_GAIN**（数字接口侧，4bit，支持 1x~64x）
- **ANAC->ANAC_CSA1.CSA_GAIN**（模拟控制侧，3bit，支持 1x~8x；高增益 x12~x64 仅通过数字侧控制）

### 2.4 固定通道模式配置

在 `CSA_Init()` 中手动指定正/负输入端：

```c
CSA_InputCH_SWMode(CSA_FIXED_CH_MODE);  // 固定通道模式

CSA->CR0.CSA_CSP = 0;   // 正端: CSP0 (PTA7)
CSA->CR0.CSA_CSN = 3;   // 负端: CSN=GND (内部接地)
```

可选输入通道：

| 值 | 输入对 | ADC映射 |
|----|--------|---------|
| `CSA_FIXED_INPUT_CH0` (0x00) | CSP0/CSN0 | ADC_CH0 |
| `CSA_FIXED_INPUT_CH1` (0x01) | CSP1/CSN1 | ADC_CH1 |
| `CSA_FIXED_INPUT_CH2` (0x02) | CSP2/CSN2 | ADC_CH2 |
| `CSA_FIXED_INPUT_CH3` (0x03) | CSP3/CSN3 | — |

---

## 三、采样验证运放增益功能正确性

### 3.1 验证原理

运放增益公式：

```
V_ADC = (V_CSP - V_CSN) × GAIN
```

- V_CSP：PTA7 输入的同相端电压
- V_CSN：反相端电压（当前接地 = 0V）
- GAIN：设定的 PGA 增益
- V_ADC：ADC 转换后的电压值（参考电压 2.5V）

ADC 转换公式：

```
V_mV = (raw - offset) × 2500 / 4095
```

### 3.2 验证方案

**硬件连接：**

| 引脚 | 功能 | 连接 |
|------|------|------|
| PTA7 | CSP0 (同相输入) | 接外部可调电压源 (0~300mV) |
| PTA8 | CSN0 (反相输入) | 接地 (GND) |
| PTC6 | TIMER13 CH0输出 | 接示波器观察 1kHz 触发波形 |
| UART0 TX/RX | 串口输出 | 接串口工具查看 mV 值 |

**测试步骤与预期结果：**

| 测试编号 | 增益设置 | PTA7输入电压 | 理论ADC输出 | 判断标准 |
|----------|----------|-------------|------------|----------|
| 1 | x8 | 0mV (接地) | ≈0mV | 校准后偏移接近0 |
| 2 | x8 | 100mV | ≈800mV | 误差 < 5% |
| 3 | x16 | 100mV | ≈1600mV | 误差 < 5% |
| 4 | x4 | 100mV | ≈400mV | 误差 < 5% |
| 5 | x8 | 200mV | ≈1600mV | 误差 < 5% |
| 6 | x32 | 50mV | ≈1600mV | 误差 < 5% |
| 7 | x64 | 25mV | ≈1600mV | 误差 < 5% |

**误差率计算：**

```
误差率 = |V_实测 - V_理论| / V_理论 × 100% < 5%
```

### 3.3 验证操作流程

1. 修改 `adc_demo_test.h` 中的 `CSA_PGA_GAIN` 宏为目标增益
2. 重新编译项目并烧录到芯片
3. 通过串口观察校准完成后的 mV 输出值
4. 切换不同增益和输入电压，记录并对比输出值
5. 确保放大后电压不超过 ADC 参考电压（2.5V），否则饱和

### 3.4 预期打印输出

校准完成后，串口每轮循环打印：

```
CH0=800mV, CH1=0mV, CH2=0mV, CH7=0mV
```

（假设输入 100mV、增益 x8，CH1/CH2/CH7 未接信号源）

---

## 四、数据流总图

```
 PTA7 ─── CSP0 ─┐
                ├──► [CSA/PGA] ──► ADC_CH0 ──► ADC_SEQ0 ──┐
 GND  ─── CSN0 ─┘    GAIN=x8                              │
                                                          │
 32MHz ──► [/8] ──► [TIMER13] ──► TMR13_TRIG ────────────┤
            4MHz      TH=3999                             │
                      1kHz                                ▼
                                                   ADC Group0 4ch
                                                   SEQ0/1/2/3 采样
                                                          │
                                                          ▼
                                                   ADC_IRQHandler
                                                   零偏校准 + mV转换
                                                          │
                                                          ▼
                                                   UART打印输出
```

---

## 五、时钟配置来源

| 参数 | 值 | 定义位置 |
|------|-----|----------|
| 芯片模式 | `FPGA_TMP_CODE` (FPGA模式) | `sys_config.h` line 11 |
| HIRC_FREQ | 32MHz | `sys_config.h` line 64 |
| PLL_FREQ | 60MHz (未使用) | `sys_config.h` line 65 |
| SYS_CORE_CLK | HIRC_FREQ = 32MHz | `sys_config.h` line 73 |
| TIMER13计数时钟 | 32MHz / DIV_8 = 4MHz | `TIMER_ADC_Init()` |
| TIMER13周期 | TH+1 = 3999 | `Timer_Set_ThVal(TIMER13, 3999-1)` |
| ADC采样率 | 4MHz / 3999 ≈ 1kHz | 计算结果 |

> **注意**：`main.c` 中 `clock_freq_get(SCG_SYS, &mcu_clk)` 实际返回 `SYS_CORE_CLK = HIRC_FREQ = 32MHz`，而非 `PLL_FREQ = 60MHz`。`adc_demo_test.c` 注释中的 `48MHz` 为错误值，实际 TIMER13 驱动时钟为 32MHz。

---

## 六、关键寄存器速查

| 寄存器 | 地址 | 关键位域 | 说明 |
|--------|------|----------|------|
| ANAC_ENCR | 0x4000_7800 | bit0=ANA_EN, bit17=ANA_CSA_EN | 模拟总使能 + CSA模拟使能 |
| CSA->CR0 | CSA基址+0x00 | CSA_EN[0], CSA_AMP_EN[1], CSA_MODE[3:2], CSA_CSP[5:4], CSA_CSN[7:6], CSA_GAIN[11:8] | CSA主控寄存器 |
| ADC->IR | ADC基址+偏移 | S0CDIF | Group0转换完成中断标志（写1清零） |
| TIMER13->TH | TIMER13基址+偏移 | — | 定时器周期值 |

---

## 七、注意事项

1. **模拟使能**：CSA工作前需确保 ANAC_ENCR bit0(ANA_EN) 和 bit17(ANA_CSA_EN) 已置位
2. **ADC时钟**：CSA输出连接到ADC，需确保 `ADC_Clk_Enable()` 已调用
3. **增益边界**：放大后的电压不能超过ADC参考电压 (VREFH=2.5V)，否则结果饱和
4. **中断标志**：ADC中断服务中必须写1清零 `ADC->IR.S0CDIF` 标志位，且必须在读取数据前清除
5. **校准时机**：建议在零输入条件下进行零偏校准（输入接地或无信号）
6. **差分输入范围**：CSA的共模输入电压需在芯片规定的范围内
7. **IRQ使能**：`__enable_irq()` 在 `startup_ARMCM0plus.s` 启动文件中已调用，无需在 main 中重复调用