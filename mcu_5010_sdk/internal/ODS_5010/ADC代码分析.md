# ADC 相关代码执行路径及当前状态深入分析

## 1. ADC 编译开关与当前生效路径

### 1.1 编译时确定的宏开关（adc_test.h 第 4-7 行）

```c
#define ADC_Normal_Sample_Test  (0)                           // 常规采样测试模式
#define ADC_INTR_TEST           (1)                           // 中断测试模式
#define ADC_APP_TEST            (2)                           // 应用测试模式
#define ADC_Test_Mode           (ADC_Normal_Sample_Test)      // ← 当前选择：常规采样测试
```

### 1.2 各子功能开关状态（adc_test.h）

| 宏定义 | 值 | 状态 | 说明 |
|--------|-----|------|------|
| `ADC_Test_Mode` | `ADC_Normal_Sample_Test (0)` | ✅ 常规 | 当前激活的测试主模式 |
| `ADC_MCPWM_TRG_GROUP` | `SampleGrpMode_1` | ✅ 固定 | ADC 分组模式：4 个独立触发组 |
| `ADC_CONFIG_CNT_TEST` | `0` | ❌ | 在线修改 ADC 时钟参数 |
| `ADC_STATEIO_TEST` | `0` | ❌ | 启用 ADC 状态信号 IO 输出 |
| `ADC_STATE_SIG_TEST` | `0` | ❌ | 软件触发后的 RDY 信号监控 |
| `ADC_SOFTTRIG_TEST` | `1` | ✅ | **软件触发模式（SW_TRIG 替代 MCPWM 触发）** |
| `ADC_SOFTTRIG_ERR_TEST` | `0` | ❌ | 软件触发错误注入测试（依赖 ADC_SOFTTRIG_TEST） |
| `ADC_MCPWM_TRG_TEST` | `0` | ❌ | MCPWM 触发模式（当前 SOFTTRIG=1 时无效） |
| `ADC_SEQNUM_CHANGE_TEST` | `1` | ✅ | **运行时修改各分组通道数** |
| `ADC_GROUPMODE_CHANGE_TEST` | `1` | ✅ | **运行时修改分组模式** |
| `ADC_TRIGCNT_CHANGE_TEST` | `1` | ✅ | **运行时修改 MCPWM 触发计数值** |
| `ADC_SEQCH_CHANGE_TEST` | `0` | ❌ | 运行时修改 SEQx 通道映射 |
| `ADC_TRIGSRC_CHANGE_TEST` | `0` | ❌ | 运行时修改触发源选择 |
| `ADC_Err_Test` | `0` | ❌ | 触发目标错误中断测试 |

> **关键结论**：当前编译产物中，ADC 工作在 **常规采样测试模式**，支持**运行时修改分组数、分组模式、触发计数**三种在线配置功能，但**不支持在线修改通道映射和触发源切换**。

---

## 2. ADC 完整执行路径（从上电到主循环）

### 2.1 执行路径时序图

```
main() 入口（main.c:359）
│
├─ [1] delay_1us(2000)                          // 2ms 上电等待
├─ [2] flash_clk_enable()                       // Flash 控制器时钟
├─ [3] System_Init()                            // 系统初始化
│
├─ [4] 时钟使能（main.c:368-381）
│     ├── gpio_clk_enable()
│     ├── mcpwm_clk_enable()
│     ├── uart_clk_enable()
│     ├── HWMATH_Clk_Enable()
│     ├── ADC_Clk_Enable()        ← ADC 时钟使能
│     ├── TIM13_Clk_Enable()
│     ├── TIM14_Clk_Enable()
│     ├── TIM15_Clk_Enable()
│     └── TIM16_Clk_Enable()
│
├─ [5] UART1/UART0 初始化                        // 串口初始化
├─ [6] rram_gpio_bypass()                       // OTP 旁路配置
│
├─ [7] module_test()（test.c:168）              ← ADC 初始化入口
│     │
│     │  // ============ MCPWM 模块初始化（先执行）============
│     │  [MCPWM_TEST]
│     ├── mcpwm0_gpio_init()                    // 配置 MCPWM0 6 路 GPIO
│     ├── mcpwm1_gpio_init()                    // 配置 MCPWM1 6 路 GPIO
│     ├── MCPWM_ch012_init()                    // 三相 PWM 初始化
│     ├── MCPWM_ch345_init()                    // MCPWM1 初始化
│     ├── mcpwm_soft_init_copy()                // 保存初始寄存器值
│     │
│     │  // ============ ADC 模块初始化 ============
│     │  [ADC_TEST]
│     ├── ADC_gpio_init()                       ← 步骤A: GPIO 初始化
│     ├── ADC_Test_Init()                       ← 步骤B: ADC 模块初始化
│     └── CSA_Test_Init()                       ← 步骤C: CSA 初始化
│
├─ [8] __enable_irq()                            // 使能全局中断
│
└─ [9] while(1) 主循环
      └── cmdline_handle_polling()              // 串口命令行轮询
```

### 2.2 步骤 A：ADC_gpio_init()（adc_test.c:55-143）

当前编译生效的 GPIO 配置（`ADC_STATEIO_TEST=0` 时不编译 PTA0/PTA1 配置）：

**始终编译的部分**：
```c
// PTB7 → GPIO 输出（ALT_FUNC_0）
// PTB8 → GPIO 输出（ALT_FUNC_0）
```
这两个引脚用于 MCPWM 回调中的 GPIO 翻转标记（`gpio_output_set(GPIO_B, PIN_7, level)`），实际上是 **MCPWM 中断回调的进入/退出标记**，与 ADC 无直接关系。

**被 #if 排除的部分**（不会编译进固件）：
- `ADC_STATEIO_TEST=0`：PTA0（READY）、PTA1（START）不配置为 ALT_FUNC_14 ADC 状态信号输出
- `ADC_STATE_SIG_TEST=0`：PTB8 不参与 RDY 信号轮询

**结论**：当前编译仅配置 PTB7、PTB8 为 GPIO 输出（MCPWM 回调调试用），**没有配置任何 ADC 专用的调试 GPIO**。

### 2.3 步骤 B：ADC_Test_Init() 分支选择（adc_test.c:982-1037）

当前 `ADC_Test_Mode = ADC_Normal_Sample_Test (0)`，编译后只保留一个路径：

```c
void ADC_Test_Init(void)
{
    #if (ADC_Test_Mode == ADC_Normal_Sample_Test)  // (0) == (0) → TRUE
    {
        ADC_MCPWMTrig_Test_Init();   // ← 编译后只执行此函数
    }
    // #elif (ADC_Test_Mode == ADC_INTR_TEST)  → (0) == (1) → FALSE，不编译
    // #elif (ADC_Test_Mode == ADC_APP_TEST)   → (0) == (2) → FALSE，不编译
}
```

> 根据 `.lst` 编译列表验证：`#if (((0)) == (0))` 条件为真，`ADC_MCPWMTrig_Test_Init()` 被编译；`#elif (((0)) == (1))` 和 `#elif (((0)) == (2))` 条件为假，对应代码不编译进固件。

---

## 3. ADC_MCPWMTrig_Test_Init() 完整执行分析（当前实际执行路径）

### 3.1 函数调用链

```
module_test()  (test.c:168)
  └── ADC_Test_Init()  (adc_test.c:982)
        └── ADC_MCPWMTrig_Test_Init()  (adc_test.c:160)   ← 当前唯一可到达的 ADC 初始化
```

### 3.2 执行步骤详解

#### 子步骤 1：ADC 使能（adc_test.c:165）
```c
ADC_ENABLE(ENABLE);    // 使能 ADC 模块电源
```

#### 子步骤 2：设置分组模式（adc_test.c:167）
```c
ADC_SetSampleGrpMode(ADC_MCPWM_TRG_GROUP);
// ADC_MCPWM_TRG_GROUP = SampleGrpMode_1
// 含义：4 个采样分组，每组由独立的触发源触发
```

#### 子步骤 3：设置各分组通道数（adc_test.c:170-173）
```c
ADC_SetGrpChCount(ADC_Sample_Group_0, 1);  // 分组0：1个通道
ADC_SetGrpChCount(ADC_Sample_Group_1, 3);  // 分组1：3个通道
ADC_SetGrpChCount(ADC_Sample_Group_2, 0);  // 分组2：0个通道（不使用）
ADC_SetGrpChCount(ADC_Sample_Group_3, 0);  // 分组3：0个通道（不使用）
```
> 实际采样：共 4 个通道（SEQ0 属于 Group0，SEQ1-3 属于 Group1）

#### 子步骤 4：设置 16 个 SEQ 通道映射（adc_test.c:177-195）
```c
ADC_SEQx_CFG(ADC_SEQ0,  ADC_CH0);
ADC_SEQx_CFG(ADC_SEQ1,  ADC_CH1);
ADC_SEQx_CFG(ADC_SEQ2,  ADC_CH2);
ADC_SEQx_CFG(ADC_SEQ3,  ADC_CH3);
// ... (共16个SEQ，SEQ4→CH4, SEQ5→CH5 ... SEQ15→CH15)
```
> 一对一映射：SEQn → ADC_CHn，即按顺序扫描全部 16 个模拟输入通道

#### 子步骤 5：设置触发源（编译时分支，adc_test.c:197-215）

当前 `ADC_SOFTTRIG_TEST = 1`（**最新修改**，之前为 0）：
```c
#if (ADC_SOFTTRIG_TEST)    // (1) → 条件为真 → 编译软件触发代码
{
    ADC_SetGrpTrigSource(ADC_SOFTTRIG_GROUP, ADC_GrpTrig_SW_TRIG);
    // ADC_SOFTTRIG_GROUP = ADC_Sample_Group_0

    #if (ADC_SOFTTRIG_ERR_TEST)   // (0) → 不编译错误注入测试
    {
        ADC_SetGrpTrigSource(ADC_Sample_Group_0, ADC_GrpTrig_SW_TRIG);
        ADC_SetGrpTrigSource(ADC_Sample_Group_1, ADC_GrpTrig_SW_TRIG);
    }
    #endif
}
#else                      // 条件为假 → 不编译 MCPWM 触发代码
{
    // ADC_SetGrpTrigSource(..., ADC_GrpTrig_MCPWM_TRIG0);  ← 不编译
}
```

✅ **当前 ADC 使用软件触发（SW_TRIG），触发源分布**：
| 分组 | 触发源（初始化后） | 通道数 | 采样通道 | 说明 |
|------|-------------------|--------|----------|------|
| Group 0 | `ADC_GrpTrig_SW_TRIG` | 1 | CH0 (SEQ0) | ← `ADC_SOFTTRIG_GROUP` 指定的组 |
| Group 1-3 | **未显式配置** | 3/0/0 | CH1-3 | ⚠️ 仅在 `SOFTTRIG_ERR_TEST=1` 时配置为 SW_TRIG |

> **⚠️ 重要变更影响**：由于 `ADC_SOFTTRIG_TEST=1`，只有 `ADC_Sample_Group_0`（对应 `ADC_SOFTTRIG_GROUP`）被显式设置为软件触发。Group1~Group3 的触发源 **未被重新配置**，保持为复位默认值或之前 MCPWM 模块初始化中可能设置的 MCPWM 触发源。这意味着：
> - **Group0** 可通过软件写寄存器触发（`ADC_SWTrig()` 函数）
> - **Group1~3** 的触发行为取决于硬件默认值或 MCPWM 初始化代码的副作用，是不确定的
> - MCPWM 初始化代码（子步骤 7）仍然设置了 4 个 MCPWM 触发点的计数值，但由于触发源未配置为 MCPWM_TRIGx，这些触发点**不会自动触发 ADC 采样**

#### 子步骤 6：设置 ADC 时钟参数（adc_test.c:217-221）
```c
ADC_SetCLK_STPT(4);         // 采样时间 = 4 个 ADC 时钟周期
ADC_SetCLK_COVT(2);         // 转换时间 = 2 个 ADC 时钟周期
ADC_SetCLK_INIT(0);         // 初始化延时 = 0
ADC_SetCLK_SAPT(13);        // 采样保持时间 = 13 个 ADC 时钟周期
ADC_SetCLK_TrigDelay(2);    // 触发延时 = 2 个 ADC 时钟周期
```

#### 子步骤 7：MCPWM 初始化（触发源初始化，adc_test.c:237-262）

```c
// 1. 初始化 MCPWM 三相 PWM 通道
MCPWM_ch012_init();         // 初始化 MCPWM0 CH0/1/2 为三相 PWM
mcpwm0_gpio_init();         // 配置 MCPWM0 6 路 PWM 输出 GPIO

// 2. 配置 MCPWM 中断
NVIC_SetPriority(MCPWM0_IRQn, 2);
NVIC_ClearPendingIRQ(MCPWM0_IRQn);
NVIC_EnableIRQ (MCPWM0_IRQn);

// 3. 配置触发源 2/3（写保护模式下）
MCPWM0_PRT = 0x0000DEAD;               // 写保护解锁
MCPWM0->TCLK &= ~(0x3<<4);             // 触发源 2/3 选择时基 0（清除 bit4-5）
MCPWM0->TCLK |= (0x0<<4);              // 设置为时基 0
MCPWM0->AUEN &= ~(0x3 << 24);          // 清除自动更新位 bit24-25
MCPWM0->AUEN |= (0x3 << 24);           // 使能触发源 2/3 自动更新
MCPWM0_PRT = 0x0000CAFE;               // 写保护锁定

// 4. 设置 4 个触发点的计数值（中心对齐 PWM 模式）
MCPWM_SetTrigCnt(MCPWM_TIG_TMR0, -(EPWM_PERIOD/8) + 12);  // 约 PWM 周期 3/8 处
MCPWM_SetTrigCnt(MCPWM_TIG_TMR1,  EPWM_PERIOD/4);         // PWM 周期 1/4 处
MCPWM_SetTrigCnt(MCPWM_TIG_TMR2,  EPWM_PERIOD/2);         // PWM 周期中点
MCPWM_SetTrigCnt(MCPWM_TIG_TMR3,  EPWM_PERIOD*3/4);       // PWM 周期 3/4 处

// 5. 同步 MCPWM 初始计数与 ADC 触发
ADC_HWTrig_PWM_Config(-EPWM_HALFPERIOD);   // MCPWM 起始计数 = -PWM 半周期
```

> **⚠️ 软件触发模式下的注意事项**：上述 MCPWM 触发点虽然在子步骤 5 之后仍然被设置了计数值和自动更新，但由于 `ADC_SOFTTRIG_TEST=1`，ADC 各分组的触发源**并未配置为 MCPWM_TRIGx**，因此这些触发点**不会自动触发 ADC 采样**。MCPWM 触发点计数值仅在将来切换回 MCPWM 硬件触发模式（`ADC_SOFTTRIG_TEST=0`）时才有意义。

**触发点分布示意**（中心对齐 PWM 模式，PWM 周期 = EPWM_PERIOD，当前仅作参考配置）：

```
PWM 周期起点 (CNT = -EPWM_HALFPERIOD)
    │
    ├─ TRIG0: -(EPWM_PERIOD/8) + 12    ← 约 PWM 周期 3/8 处
    │  (触发Group0: 采样CH0)
    │
    ├─ TRIG1: EPWM_PERIOD/4            ← PWM 周期 1/4 处
    │  (触发Group1: 采样CH1/CH2/CH3)
    │
    ├─ TRIG2: EPWM_PERIOD/2            ← PWM 周期中点
    │  (触发Group2: 无通道)
    │
    └─ TRIG3: EPWM_PERIOD*3/4          ← PWM 周期 3/4 处
       (触发Group3: 无通道)
```

#### 子步骤 8：错误中断配置（编译时排除，adc_test.c:264-268）

```c
#if (ADC_Err_Test)   // (0) → 条件为假，不编译
{
    ADC_Enable_IrqSrc(ADC_IRQSRC_TGT_ERR);
}
```

---

## 4. CSA_Test_Init() 执行分析（CSA 电流采样放大器初始化）

`CSA_Test_Init()` 在 `test.c` 的 `module_test()` 中与 `ADC_Test_Init()` 并列调用（`test.c` 第 362 行）：

```c
void CSA_Test_Init(void)
{
    CSA_ENABLE(ENABLE);    // 使能 CSA 模块电源

    #if (Single_Shunt == Config_Shunt_Mode)
    {
        // 单电阻模式：固定通道 CH3
        CSA_InputCH_SWMode(CSA_FIXED_CH_MODE);
        CSA_SET_InputCH(CSA_FIXED_INPUT_CH3);
    }
    #else
    {
        // 双/三电阻模式：根据实际输入通道动态切换
        CSA_InputCH_SWMode(CSA_Test_Mode);
    }
    #endif

    // 额外调试寄存器写入（可能的测试模式 IO 配置）
    *(int32_t *)0x40015804 = 0x11;       // 未知寄存器地址
    *(int32_t *)0x4800083C = 0xfff00000; // 未知寄存器地址
    *(int32_t *)0x48000840 = 0x0000000f; // 未知寄存器地址
}
```

> **注意**：第 46-48 行直接写入三个硬编码的寄存器地址，这些可能是特定的模拟 IO 测试模式配置寄存器。具体的电阻采样模式取决于 `Config_Shunt_Mode` 宏（定义在其他头文件中）。

---

## 5. ADCTest_Config_OL() —— ADC 在线配置更新（在主循环中间接调用）

`ADCTest_Config_OL()` 是 ADC 在线参数的**配置更新函数**，由 `ADC_Task_CallBack_InPWM()` 调用（在 MCPWM 回调中执行）：

```c
void ADC_Task_CallBack_InPWM(void)
{
    FOC_Control_Process();     // FOC 电机控制算法
    ADCTest_Config_OL();      // 在线 ADC 配置更新
}
```

### 5.1 当前编译有效的在线配置操作

根据开关状态，当前编译中可执行的操作路径摘要：

| 操作 | 开关 | 状态 | 函数调用 |
|------|------|------|----------|
| 修改分组模式 | `ADC_GROUPMODE_CHANGE_TEST=1` | ✅ | `ADC_SetSampleGrpMode()` |
| 修改时钟参数 | `ADC_CONFIG_CNT_TEST=0` | ❌ | 不编译 |
| 修改分组通道数 | `ADC_SEQNUM_CHANGE_TEST=1` | ✅ | `ADC_SetGrpChCount()` ×4 |
| 修改通道映射 | `ADC_SEQCH_CHANGE_TEST=0` | ❌ | 不编译 |
| 修改触发源 | `ADC_TRIGSRC_CHANGE_TEST=0` | ❌ | 不编译 |
| 修改触发计数值 | `ADC_TRIGCNT_CHANGE_TEST=1` | ✅ | `MCPWM_SetTrigCnt()` ×4 |
| **软件触发采样** | `ADC_SOFTTRIG_TEST=1` | ✅ | `ADC_SWTrig()` + `ADCTest_Config_OL` 中的 SWTigCtrl 路径 |
| 中断测试 | `ADC_Test_Mode=0` | ❌ | 不编译 |

### 5.2 有效的在线更新流程

当 `stru_AdcTest_OL.update` 标志被设置为 1 时（通过串口命令触发），执行以下操作：

```c
if(stru_AdcTest_OL.update)
{
    gpio_output_set(GPIO_B, PIN_7, HIGH);    // 更新标记（示波器触发点）

    // [操作1] 在线修改分组模式 ✅
    ADC_SetSampleGrpMode((1 << stru_AdcTest_OL.GroupMode) - 1);

    // [操作2] 在线修改时钟参数 ❌ 跳过
    // ADC_SetCLK_INIT/STPT/COVT/SAPT/TrigDelay()

    // [操作3] 在线修改各分组通道数 ✅
    ADC_SetGrpChCount(ADC_Sample_Group_0, stru_AdcTest_OL.TigChNum[0]);
    ADC_SetGrpChCount(ADC_Sample_Group_1, stru_AdcTest_OL.TigChNum[1]);
    ADC_SetGrpChCount(ADC_Sample_Group_2, stru_AdcTest_OL.TigChNum[2]);
    ADC_SetGrpChCount(ADC_Sample_Group_3, stru_AdcTest_OL.TigChNum[3]);

    // [操作4] 在线修改通道映射 ❌ 跳过
    // for(i=0;i<16;i++) ADC_SEQx_CFG(i, stru_AdcTest_OL.Seq[i]);

    // [操作5] 在线修改触发源 ❌ 跳过
    // ADC_SetGrpTrigSource(Group_0/1/2/3, ...)

    // [操作6] 在线修改触发计数值 ✅
    MCPWM_SetTrigCnt(MCPWM_TIG_TMR0, stru_AdcTest_OL.TigCnt[0]);
    MCPWM_SetTrigCnt(MCPWM_TIG_TMR1, stru_AdcTest_OL.TigCnt[1]);
    MCPWM_SetTrigCnt(MCPWM_TIG_TMR2, stru_AdcTest_OL.TigCnt[2]);
    MCPWM_SetTrigCnt(MCPWM_TIG_TMR3, stru_AdcTest_OL.TigCnt[3]);

    stru_AdcTest_OL.update = 0;              // 清除更新标志
    gpio_output_set(GPIO_B, PIN_7, LOW);     // 完成标记
}
```

---

## 6. MCPWM 中断中的 ADC 相关操作

在 `mcpwm0_callback_duty_test()` 回调（mcpwm_test.c:48）中：

```c
void mcpwm0_callback_duty_test(mcpwm0_int_source_e source)
{
    gpio_output_set(GPIO_C, PIN_9, HIGH);   // 示波器测量：回调入口
    mcpwm0_callback_test_cnt++;

    // 当前激活路径 (#if 1)：
    FOC_Control_Process();                   // 三相 FOC 电机控制算法

    gpio_output_set(GPIO_C, PIN_9, LOW);    // 示波器测量：回调出口
    UartView(cnt, cnt, cnt, cnt);           // UART1 输出计数器
}
```

`FOC_Control_Process()` 内部调用 `ADC_Task_CallBack_InPWM()`：

```c
void ADC_Task_CallBack_InPWM(void)   // adc_test.c:871
{
    FOC_Control_Process();      // FOC 控制算法（可能读取 ADC 结果）
    ADCTest_Config_OL();        // 在线更新 ADC 配置（如果 update 标志置位）
}
```

---

## 7. 主循环中的 ADC 间接操作

### 7.1 命令行接口

ADC 相关的命令行函数（`cmdline/adc_cmdline.c`）当前为空实现：

```c
int cmd_adc_function1_test(int argc, char *argv[])
{
    bool debug_en = get_arg_ulong(argv[1]);
    INFO("PASS\n");   // 仅返回 "PASS"，无实际 ADC 操作
    return 0;
}

int cmd_adc_function2_test(int argc, char *argv[])
{
    bool debug_en = get_arg_ulong(argv[1]);
    INFO("PASS\n");   // 同上
    return 0;
}
```

> 当前无法通过串口命令直接控制 ADC 测试功能。但可以通过 `mcpwm_test` 命令修改 MCPWM 参数（周期、占空比等）来**间接影响** ADC 触发时机。

### 7.2 main.c 中的 ADC 全局变量

```c
// main.c 第 356 行
u8 ADC_TestFlag;   // 全局变量，在 main.c 中定义
```

该变量在 `main()` 函数中未被引用，`ADC_TestFlag` 声明可能被其他编译单元通过 `extern` 引用（如 `para_define.h` 或 `ADC.h` 中声明的 `global_t` 结构体的成员）。

---

## 8. 配置文件结构体默认值分析

```c
// adc_test.c 第 34-52 行
struct_AdcTest_OL stru_AdcTest_OL =
{
    .STPT_CNT = 5,                  // 采样时间=5 ADC时钟周期（实际初始化被覆盖为4）
    .COVT_CNT = 2,                  // 转换时间=2 ADC时钟周期
    .SAPT_CNT = 13,                 // 采样保持时间=13 ADC时钟周期
    .INIT_CNT = 3,                  // 初始化延时=3（实际初始化被覆盖为0）
    .DELAY_CNT = 15,                // 触发延时=15（实际初始化被覆盖为2）
    .INTR_SRC = ADC_IRQSRC_S0CP,    // 中断源：组0转换完成
    .GroupMode = 2,                 // 分组模式编号（用于在线修改）
    .TigChNum = {3, 4, 3, 1},      // 在线参数：各分组通道数
    .TigSrc = {0, 1, 2, 3},        // 在线参数：各分组触发源
    .TigCnt = {                     // 在线参数：触发点计数值
        -(s16)EPWM_PERIOD/8 + 12,
        (s16)EPWM_PERIOD/4,
        (s16)EPWM_PERIOD/2,
        EPWM_PERIOD*3/4
    },
    .Seq = {ADC_CH0, ADC_CH1, ..., ADC_CH15}   // 在线参数：16个SEQ通道映射
};
```

> **注意**：结构体中的 `STPT_CNT=5`、`INIT_CNT=3`、`DELAY_CNT=15` 等默认值与 `ADC_MCPWMTrig_Test_Init()` 中**实际写入寄存器的值不一致**（实际写入 STPT=4, INIT=0, DELAY=2），说明结构体默认值用于**在线修改时的参考值**，而**初始化时的实际值以函数中硬编码的参数为准**。

---

## 9. ADC 当前状态总结

### 9.1 硬件状态

| 项目 | 当前值/状态 |
|------|------------|
| ADC 模块电源 | ✅ 已使能（`ADC_ENABLE(ENABLE)`） |
| ADC 时钟 | ✅ 已使能（`ADC_Clk_Enable()` in main.c:376） |
| 触发模式 | ✅ **软件触发（SW_TRIG）** — 仅 Group0 显式配置为 SW_TRIG |
| 采样分组 | 分组模式1：Group0=1ch(SW), Group1=3ch(未配置), Group2=0, Group3=0 |
| 实际可采样通道 | **CH0**（仅 Group0 可通过 `ADC_SWTrig()` 触发） |
| SEQ 映射 | SEQ0-3 → CH0-3（但 Group1 SEQ1-3 触发源未配置，无法自动采样） |
| 触发时机 | **软件写寄存器触发**（`ADC_SWTrig()` 或 `stru_AdcTest_OL.SWTigCtrl` 标志触发） |
| ADC 时钟参数 | STPT=4, COVT=2, INIT=0, SAPT=13, TrigDelay=2 |
| ADC 中断 | ❌ 未使能（NVIC 未配置 `ADC_IRQn`，`ADC_ERR_TEST=0`） |
| 模拟看门狗 | ❌ 未配置（仅 `ADC_INTR_TEST` 模式使用） |

### 9.2 数据流向

```
软件触发路径（当前 ADC_SOFTTRIG_TEST=1）：
  ADCTest_Config_OL() 检测到 SWTigCtrl 标志
        │
        ▼
  ADC_SWTrig() → 写 ADC 软件触发寄存器
        │
        ▼
  ADC 开始采样 Group0：CH0 (SEQ0, 1通道)
        │
        ▼
  等待 RDY 信号（如果 ADC_STATE_SIG_TEST=1 会轮询，当前=0 跳过）
        │
        ▼
  结果存入 ADC 结果寄存器（可通过 ADC_GetResult(0) 读取 CH0 结果）
        │
        ▼
  （ADC 中断未使能，无中断触发）
        │
        ▼
  MCPWM 中断 → FOC_Control_Process() → 可能通过 ADC_GetResult() 读取结果

Group1~Group3（CH1-CH3）：
  触发源未显式配置，采样行为不确定。
  除非通过在线命令设置相应的触发源（需 ADC_TRIGSRC_CHANGE_TEST=1），否则无法采样。
```

> **关键注意**：
> - `ADC_SOFTTRIG_TEST=1` 时，MCPWM 触发点虽然设置了计数值，但**不会自动触发 ADC 采样**
> - 唯一可确定采样的通道是 **CH0 (Group0)**，通过 `ADC_SWTrig()` 函数触发
> - 每次 `ADC_SWTrig()` 调用触发一次单次采样（Group0 的 1 个 SEQ）
> - ADC 结果需要**主动轮询** `ADC_GetResult()` 或在 MCPWM 回调中通过 FOC 算法读取
> - 如果 `ADC_STATE_SIG_TEST=1`（当前=0），可启用 RDY 状态轮询来检测采样完成

### 9.3 可在线修改的参数（通过串口命令触发）

| 参数类别 | 结构体字段 | 可在线修改 | 修改函数 |
|----------|-----------|-----------|----------|
| 分组模式 | `stru_AdcTest_OL.GroupMode` | ✅ | `ADC_SetSampleGrpMode()` |
| 各分组通道数 | `stru_AdcTest_OL.TigChNum[0..3]` | ✅ | `ADC_SetGrpChCount()` | 
| MCPWM 触发计数值 | `stru_AdcTest_OL.TigCnt[0..3]` | ✅ | `MCPWM_SetTrigCnt()` |
| ADC 时钟参数 | `stru_AdcTest_OL.STPT/COVT/INIT/SAPT/DELAY` | ❌ | 未编译 |
| SEQx 通道映射 | `stru_AdcTest_OL.Seq[0..15]` | ❌ | 未编译 |
| 触发源选择 | `stru_AdcTest_OL.TigSrc[0..3]` | ❌ | 未编译 |

---

## 10. 代码质量问题/风险点

1. **ADC 中断未使能**：常规采样测试模式下中断未配置，采样结果读取依赖其他路径（MCPWM 回调中的 FOC 算法），如果 FOC 未读取则数据被丢弃
2. **命令行接口为空**：`cmd_adc_function1_test` / `cmd_adc_function2_test` 仅打印 "PASS"，无法通过命令行直接控制 ADC
3. **硬编码寄存器写入**：CSA 初始化中 3 个直接寄存器写入（`0x40015804`, `0x4800083C`, `0x48000840`）未注释说明功能
4. **调试 GPIO 混用**：PTB7/PTB8 实际由 MCPWM 回调使用，非 ADC 专用调试引脚
5. **配置值不一致**：结构体默认值与实际初始化值不同（STPT=5 vs 4, INIT=3 vs 0, DELAY=15 vs 2）
6. **`ADC_TestFlag` 全局变量**：在 main.c 中定义但 main() 中未引用，依赖外部声明使用
7. **ADC_Test_Mode 硬编码**：切换测试模式需要修改 `adc_test.h` 并重新编译，无法运行时切换

---

## 11. 文件关联总图

```
main.c (包含 ADC_Clk_Enable, u8 ADC_TestFlag)
  │
  ├── test.h (ENABLE_ADC_TEST=1 → ADC_TEST)
  │
  └── test.c (module_test)
        │
        ├── adc_test.h (ADC_Test_Mode=0, 14个子开关)
        │     │
        ├── adc_test.c (ADC_gpio_init, ADC_MCPWMTrig_Test_Init, ADCTest_Config_OL)
        │     └── ADC_Task_CallBack_InPWM → FOC_Control_Process + ADCTest_Config_OL
        │
        ├── CSA_test.c (CSA_Test_Init: CSA_ENABLE + 输入通道配置)
        │
        ├── mcpwm_test.c (mcpwm0_callback_duty_test → FOC_Control_Process)
        │
        └── adc_cmdline.c (cmd_adc_function1/2_test: 空实现)