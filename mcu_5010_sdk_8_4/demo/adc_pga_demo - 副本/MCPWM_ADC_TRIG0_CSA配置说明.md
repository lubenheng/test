# MCPWM ADC_TRIG0 硬件触发 ADC 采样 CSA/PGA 配置说明

## 一、概述

本文档描述如何使用 MCPWM 的 **ADC_TRIG0** 事件硬件触发 ADC，对 CSA（Current Sense Amplifier / 运算放大器 PGA）的三个采样通道及一个预留通道进行顺序采样。

### 硬件拓扑
```
                    ┌──────────────────────────────┐
                    │         MCU (ODS_5010)        │
                    │                               │
 CSA_CH0 ──────────►│ CSA → PGA → ADC_CH0 (SEQ0)   │
 CSA_CH1 ──────────►│ CSA → PGA → ADC_CH1 (SEQ1)   │
 CSA_CH2 ──────────►│ CSA → PGA → ADC_CH2 (SEQ2)   │
  预留通道          │        → ADC_CH7 (SEQ3)      │
                    │                               │
 MCPWM CNT0 ───────►│ TMR0 匹配 → ADC_TRIG0 触发   │
 (PWM时基)          │                               │
                    └──────────────────────────────┘
```

### 转换序列设计

| 序列编号 | ADC通道 | 采样对象 | 说明 |
|----------|---------|----------|------|
| **SEQ0** | ADC_CH0 | CSA通道0 | 运放采样通道0，对应CSA输入通道0 |
| **SEQ1** | ADC_CH1 | CSA通道1 | 运放采样通道1，对应CSA输入通道1 |
| **SEQ2** | ADC_CH2 | CSA通道2 | 运放采样通道2，对应CSA输入通道2 |
| **SEQ3** | ADC_CH7 | 预留通道 | 预留扩展通道，可用于其他模拟信号 |

- **4个序列均属于同一个采样组（Group0）**
- **统一由 MCPWM_TRIG0 信号触发**
- **触发一次，按顺序转换 SEQ0→SEQ1→SEQ2→SEQ3**

---

## 二、核心寄存器/API 说明

### 2.1 CSA (运算放大器) API

| API | 功能 | 参数 |
|-----|------|------|
| `CSA_ENABLE(ENABLE)` | 使能 CSA 模块 | `ENABLE` / `DISABLE` |
| `CSA_InputCH_SWMode(mode)` | 通道切换模式 | `CSA_FIXED_CH_MODE` (固定)/ `CSA_AUTO_DIFF_MODE` / `CSA_AUTO_SINGLE_MODE` |
| `CSA_SET_InputCH(ch)` | 选择输入通道 (固定模式下) | `CSA_FIXED_INPUT_CH0/CH1/CH2` |
| `CSA_SET_PGAGAIN(gain)` | 设置 PGA 增益 | x1 / x2 / x4 / x8 / x12 / x16 / x32 / x64 |

### 2.2 ADC 配置 API

| API | 功能 |
|-----|------|
| `ADC_Clk_Enable()` | 使能 ADC 时钟 |
| `ADC_ENABLE(ENABLE)` | 使能 ADC 模块 |
| `ADC_SetSampleGrpMode(mode)` | 设置分组模式 `SampleGrpMode_1` (1组) |
| `ADC_SetGrpChCount(grp, num)` | 设置指定组包含的通道数量 |
| `ADC_SEQx_CFG(seq, ch)` | 配置序列x对应的ADC通道 |
| `ADC_SetGrpTrigSource(grp, src)` | 设置组的触发源 (`ADC_GrpTrig_MCPWM_TRIG0`) |
| `ADC_SetCLK_STPT(cnt)` | 采样时钟配置 |
| `ADC_SetCLK_COVT(cnt)` | 转换时钟配置 |
| `ADC_SetCLK_INIT(cnt)` | 初始化时钟配置 |
| `ADC_SetCLK_SAPT(cnt)` | 采样保持时钟配置 |
| `ADC_SetCLK_TrigDelay(cnt)` | 触发延时配置 |
| `ADC_GetResult(seq)` | 读取指定序列的转换结果 |

### 2.3 MCPWM 触发配置 API

| API | 功能 |
|-----|------|
| `MCPWM_ch012_init()` | 初始化 MCPWM CH0/1/2（时基0） |
| `MCPWM_SetTrigCnt(MCPWM_TIG_TMR0, cnt)` | 设置 TMR0 触发比较值（触发点） |
| `MCPWM0_PRT = 0x0000DEAD` | 解锁 MCPWM 保护寄存器 |
| `MCPWM0_PRT = 0x0000CAFE` | 锁定 MCPWM 保护寄存器 |

### 2.4 MCPWM 触发点计算

MCPWM 时基0（CNT0）工作于**增减计数模式**（中心对齐）：
```
CNT0 范围:  0 → EPWM_PERIOD → 0 → -EPWM_PERIOD → 0 → ...
           (递增)            (递减)           (递增)
```
- **TMR0** 寄存器在 CNT0 计数匹配时产生 ADC_TRIG0 事件
- `EPWM_PERIOD = MCU_CLK / EPWM_FREQ / 2`
- 示例（MCU_CLK=48MHz, EPWM_FREQ=10kHz）：EPWM_PERIOD = 48M/10k/2 = 2400
- 触发点建议设置在 PWM 周期中间位置，避开开关噪声

---

## 三、配置代码实现

### 3.1 `adc_demo_test.h` 修改

```c
#ifndef ADC_DEMO_TEST_H
#define ADC_DEMO_TEST_H

#include "platform_common.h"
#include "mcpwm_type.h"
#include "adc_hal.h"
#include "CSA_Type.h"
#include "CSA.h"

/*----------------------------------------------------------------------------
   CSA/PGA 配置
 *----------------------------------------------------------------------------*/
#define CSA_PGA_GAIN            CSA_PGA_GAIN_X8      /* PGA增益: x8 */
#define CSA_INPUT_MODE          CSA_FIXED_CH_MODE     /* 固定通道模式 */

/*----------------------------------------------------------------------------
   ADC demo 配置
 *----------------------------------------------------------------------------*/
#define ADC_DEMO_CHANNEL        ADC_CH7
#define ADC_DEMO_GPIO_PORT      GPIO_B
#define ADC_DEMO_GPIO_PIN       GPIO_PIN_12

/* CSA对应的ADC通道 */
#define ADC_CSA_CH0             ADC_CH0    /* CSA通道0 → ADC_CH0 */
#define ADC_CSA_CH1             ADC_CH1    /* CSA通道1 → ADC_CH1 */
#define ADC_CSA_CH2             ADC_CH2    /* CSA通道2 → ADC_CH2 */
#define ADC_RESERVED_CH         ADC_CH7    /* 预留通道  → ADC_CH7 */

extern mcpwm_shadow        stru_mcpwm_shadow;
extern uint32_t            adc_val;
extern struct_AdcTest_OL   stru_AdcTest_OL;

/* CSA 采样结果缓存 */
extern uint32_t adc_csa_ch0_val;
extern uint32_t adc_csa_ch1_val;
extern uint32_t adc_csa_ch2_val;
extern uint32_t adc_reserved_ch_val;

void adc_gpio_init(void);
void mcpwm0_gpio_init(void);
void CSA_Init(void);
void ADC_HWTrig_PWM_Config(int InitCnt);
void adc_mcpwm_trig0_csa_demo(void);

#endif
```

### 3.2 `adc_demo_test.c` 修改

```c
#include "platform_common.h"

#include "gpio_hal.h"
#include "ADC_Reg.h"
#include "ADC_Type.h"
#include "adc.h"
#include "adc_hal.h"
#include "mcpwm_hal.h"
#include "mcpwm_reg.h"
#include "mcpwm.h"
#include "CSA_Reg.h"
#include "CSA_Type.h"
#include "CSA.h"
#include "adc_demo_test.h"

mcpwm_shadow  stru_mcpwm_shadow;

uint32_t adc_val;
uint32_t adc_csa_ch0_val;
uint32_t adc_csa_ch1_val;
uint32_t adc_csa_ch2_val;
uint32_t adc_reserved_ch_val;

/* ADC 测试结构体 */
struct_AdcTest_OL stru_AdcTest_OL =
{
    .STPT_CNT   = 5,
    .COVT_CNT   = 2,
    .SAPT_CNT   = 13,
    .INIT_CNT   = 3,
    .DELAY_CNT  = 15,

    .INTR_SRC   = ADC_IRQSRC_S0CP,

    .GroupMode  = 1,              /* 仅使用1组模式 */
    .TigChNum   = {4, 0, 0, 0},   /* Group0: 4个通道 */
    .TigSrc     = {0, 0, 0, 0},   /* Group0: MCPWM_TRIG0 */
    .TigCnt     = {EPWM_PERIOD/2, 0, 0, 0},  /* 触发点在PWM周期中点 */
    .Seq        = {
        ADC_CH0, ADC_CH1, ADC_CH2, ADC_CH7,   /* SEQ0~3: CSA三通道 + 预留 */
        ADC_CH4, ADC_CH5, ADC_CH6, ADC_CH8,
        ADC_CH9, ADC_CH10,ADC_CH11,ADC_CH12,
        ADC_CH13,ADC_CH14,ADC_CH15,ADC_CH3
    }
};

/*----------------------------------------------------------------------------
 * ADC GPIO 初始化 (PB12 作为ADC输入)
 *----------------------------------------------------------------------------*/
void adc_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;
    setting_cfg.gpio_port     = ADC_DEMO_GPIO_PORT;
    setting_cfg.gpio_pin      = ADC_DEMO_GPIO_PIN;
    setting_cfg.gpio_alt      = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir      = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg      = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
}

/*----------------------------------------------------------------------------
 * MCPWM GPIO 初始化 (PTA6-PTA11 => CH0P/N, CH1P/N, CH2P/N)
 *----------------------------------------------------------------------------*/
void mcpwm0_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;

    /* MCPWM_CH0P, PTA6 PIN */
    setting_cfg.gpio_port     = GPIO_A;
    setting_cfg.gpio_pin      = GPIO_PIN_6;
    setting_cfg.gpio_alt      = GPIO_ALT_FUNC_15;
    setting_cfg.gpio_dir      = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg      = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* MCPWM_CH1P, PTA7 PIN */
    setting_cfg.gpio_port     = GPIO_A;
    setting_cfg.gpio_pin      = GPIO_PIN_7;
    setting_cfg.gpio_alt      = GPIO_ALT_FUNC_15;
    setting_cfg.gpio_dir      = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg      = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* MCPWM_CH2P, PTA8 PIN */
    setting_cfg.gpio_port     = GPIO_A;
    setting_cfg.gpio_pin      = GPIO_PIN_8;
    setting_cfg.gpio_alt      = GPIO_ALT_FUNC_15;
    setting_cfg.gpio_dir      = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg      = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* MCPWM_CH0N, PTA9 PIN */
    setting_cfg.gpio_port     = GPIO_A;
    setting_cfg.gpio_pin      = GPIO_PIN_9;
    setting_cfg.gpio_alt      = GPIO_ALT_FUNC_15;
    setting_cfg.gpio_dir      = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg      = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* MCPWM_CH1N, PTA10 PIN */
    setting_cfg.gpio_port     = GPIO_A;
    setting_cfg.gpio_pin      = GPIO_PIN_10;
    setting_cfg.gpio_alt      = GPIO_ALT_FUNC_15;
    setting_cfg.gpio_dir      = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg      = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* MCPWM_CH2N, PTA11 PIN */
    setting_cfg.gpio_port     = GPIO_A;
    setting_cfg.gpio_pin      = GPIO_PIN_11;
    setting_cfg.gpio_alt      = GPIO_ALT_FUNC_15;
    setting_cfg.gpio_dir      = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg      = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
}

/*----------------------------------------------------------------------------
 * CSA (PGA/运算放大器) 初始化
 *   - 使能CSA模块
 *   - 配置为固定通道模式
 *   - 设置PGA增益
 *----------------------------------------------------------------------------*/
void CSA_Init(void)
{
    /* 使能CSA模块 */
    CSA_ENABLE(ENABLE);

    /* 设置为固定通道模式 */
    CSA_InputCH_SWMode(CSA_FIXED_CH_MODE);

    /*
     * CSA有三个独立的采样通道，分别对应ADC_CH0/1/2
     * 在固定模式下，通过CSA_SET_InputCH()选择当前激活的通道
     * 注意：此处设置为CH0作为初始化默认值
     * 实际采样时ADC会依次读取CH0/CH1/CH2的ADC结果
     */
    CSA_SET_InputCH(CSA_FIXED_INPUT_CH0);

    /* 设置PGA增益 (示例: x8) */
    CSA_SET_PGAGAIN(CSA_PGA_GAIN);
}

/*----------------------------------------------------------------------------
 * ADC 硬件触发配置 (MCPWM TRIG0)
 *   停止MCPWM计数器，设置初始值，然后重新启动
 *----------------------------------------------------------------------------*/
void ADC_HWTrig_PWM_Config(int InitCnt)
{
    /* 1. 停止MCPWM计数器 (TCLK bit[7:6]=00) */
    MCPWM0_TCLK &= ~(0x03 << 6);

    /* 2. 设置计数器初始值 */
    MCPWM0_CNT0 = (uint16_t)(InitCnt);
    MCPWM0_CNT1 = (uint16_t)(InitCnt);

    /* 3. 加载CNT0/CNT1更新 (UPDATE bit[11]=1, bit[27]=1) */
    MCPWM0_UPDATE |= (0x01 << 11);
    MCPWM0_UPDATE |= (0x01 << 27);

    /* 4. 启动MCPWM计数器 (TCLK bit[6]=1) */
    MCPWM0_TCLK |= (0x01 << 6);
}

/*----------------------------------------------------------------------------
 * MCPWM ADC_TRIG0 触发 CSA 采样 DEMO
 *
 * 执行流程:
 *   1. 使能并配置CSA (PGA运放)
 *   2. 使能并配置ADC
 *      - 分组模式: 1组 (SampleGrpMode_1)
 *      - Group0 包含4个通道: ADC_CH0/CH1/CH2/CH7
 *      - SEQ0=CH0 (CSA通道0), SEQ1=CH1 (CSA通道1)
 *      - SEQ2=CH2 (CSA通道2), SEQ3=CH7 (预留通道)
 *      - 触发源: MCPWM_TRIG0
 *   3. 初始化MCPWM
 *      - 配置TMR0触发比较值
 *      - 配置CNT0初始值和周期
 *   4. MCPWM计数到TMR0时自动产生ADC_TRIG0信号
 *   5. ADC_TRIG0触发ADC对Group0的4个序列依次转换
 *   6. 主循环中读取ADC结果
 *----------------------------------------------------------------------------*/
void adc_mcpwm_trig0_csa_demo(void)
{
    INFO("=== ADC_MCPWM_TRIG0_CSA_DEMO START ===\r\n");

    /*==================== 第一步: CSA 初始化 ====================*/
    CSA_Init();
    INFO("CSA Init OK (Gain=x%d, Mode=Fixed_CH)\r\n",
         (1 << CSA_PGA_GAIN));  /* CSA_PGA_GAIN_X8 = 3, 1<<3=8 */

    /*==================== 第二步: ADC 初始化 ====================*/
    ADC_Clk_Enable();
    ADC_ENABLE(ENABLE);

    /*
     * 分组模式: 仅使用1组
     * SampleGrpMode_1 = 0x1 → 只有 Group0 参与转换
     * Group1/2/3 禁用，节省功耗和转换时间
     */
    ADC_SetSampleGrpMode(SampleGrpMode_1);

    /*
     * 设置每组通道数
     * Group0: 4个通道 (CH0/CH1/CH2/CH7)
     * Group1/2/3: 未使用 → 0
     */
    ADC_SetGrpChCount(ADC_Sample_Group_0, 4);
    ADC_SetGrpChCount(ADC_Sample_Group_1, 0);
    ADC_SetGrpChCount(ADC_Sample_Group_2, 0);
    ADC_SetGrpChCount(ADC_Sample_Group_3, 0);

    /*
     * ★ 核心配置: 转换序列映射
     *
     * 序列号    ADC通道    采样对象
     * SEQ0  →   ADC_CH0      CSA通道0 (运放输出)
     * SEQ1  →   ADC_CH1      CSA通道1 (运放输出)
     * SEQ2  →   ADC_CH2      CSA通道2 (运放输出)
     * SEQ3  →   ADC_CH7      预留扩展通道
     *
     * MCPWM_TRIG0触发一次 → 自动按SEQ0→SEQ1→SEQ2→SEQ3顺序转换
     */
    ADC_SEQx_CFG(ADC_SEQ0, ADC_CSA_CH0);      /* SEQ0 → CH0 (CSA0) */
    ADC_SEQx_CFG(ADC_SEQ1, ADC_CSA_CH1);      /* SEQ1 → CH1 (CSA1) */
    ADC_SEQx_CFG(ADC_SEQ2, ADC_CSA_CH2);      /* SEQ2 → CH2 (CSA2) */
    ADC_SEQx_CFG(ADC_SEQ3, ADC_RESERVED_CH);   /* SEQ3 → CH7 (预留) */

    /*
     * 其余12个序列保留默认配置（也可不配，Group0只用前4个序列）
     */
    ADC_SEQx_CFG(ADC_SEQ4,  ADC_CH4);
    ADC_SEQx_CFG(ADC_SEQ5,  ADC_CH5);
    ADC_SEQx_CFG(ADC_SEQ6,  ADC_CH6);
    ADC_SEQx_CFG(ADC_SEQ7,  ADC_CH8);
    ADC_SEQx_CFG(ADC_SEQ8,  ADC_CH9);
    ADC_SEQx_CFG(ADC_SEQ9,  ADC_CH10);
    ADC_SEQx_CFG(ADC_SEQ10, ADC_CH11);
    ADC_SEQx_CFG(ADC_SEQ11, ADC_CH12);
    ADC_SEQx_CFG(ADC_SEQ12, ADC_CH13);
    ADC_SEQx_CFG(ADC_SEQ13, ADC_CH14);
    ADC_SEQx_CFG(ADC_SEQ14, ADC_CH15);
    ADC_SEQx_CFG(ADC_SEQ15, ADC_CH3);

    /*
     * ★ 触发源配置: Group0 使用 MCPWM_TRIG0
     *   ADC_GrpTrig_MCPWM_TRIG0 = 0
     */
    ADC_SetGrpTrigSource(ADC_Sample_Group_0, ADC_GrpTrig_MCPWM_TRIG0);
    ADC_SetGrpTrigSource(ADC_Sample_Group_1, ADC_GrpTrig_MCPWM_TRIG0);
    ADC_SetGrpTrigSource(ADC_Sample_Group_2, ADC_GrpTrig_MCPWM_TRIG0);
    ADC_SetGrpTrigSource(ADC_Sample_Group_3, ADC_GrpTrig_MCPWM_TRIG0);

    /* ADC 时钟配置 (与原始demo保持一致) */
    ADC_SetCLK_STPT(4);       /* 采样时间 */
    ADC_SetCLK_COVT(2);       /* 转换时间 */
    ADC_SetCLK_INIT(0);       /* 初始化时间 */
    ADC_SetCLK_SAPT(13);      /* 采样保持时间 */
    ADC_SetCLK_TrigDelay(2);  /* 触发延时 */

    INFO("ADC Init OK (Group0: 4ch, SEQ0-3: CH0/CH1/CH2/CH7, Trig: MCPWM_TRIG0)\r\n");

    /*==================== 第三步: MCPWM 初始化 ====================*/
    MCPWM_ch012_init();
    mcpwm0_gpio_init();

    NVIC_SetPriority(MCPWM0_IRQn, 2);
    NVIC_ClearPendingIRQ(MCPWM0_IRQn);
    NVIC_EnableIRQ(MCPWM0_IRQn);

    /* 解锁保护寄存器 */
    MCPWM0_PRT = 0x0000DEAD;

    /* 触发源23选择时基0 (TCLK bit[5:4]=00) */
    MCPWM0->TCLK &= ~(0x3 << 4);
    MCPWM0->TCLK |= (0x0 << 4);

    /* 使能TMR0自动更新 (AUEN bit[8]=1) */
    MCPWM0->AUEN &= ~(0x3 << 24);
    MCPWM0->AUEN |= (0x3 << 24);

    /* 锁定保护寄存器 */
    MCPWM0_PRT = 0x0000CAFE;

    /*
     * ★ 设置采样触发点
     *
     * TMR0 = EPWM_PERIOD/2
     * 在PWM周期中点触发ADC采样，避开开关噪声
     *
     * 例如: MCU_CLK=48MHz, EPWM_FREQ=10kHz
     *       EPWM_PERIOD = 48M / 10k / 2 = 2400
     *       TMR0 = 2400 / 2 = 1200
     *       在CNT0从0递增到1200时产生TRIG0事件
     */
    MCPWM_SetTrigCnt(MCPWM_TIG_TMR0, stru_AdcTest_OL.TigCnt[0]);
    MCPWM_SetTrigCnt(MCPWM_TIG_TMR1, stru_AdcTest_OL.TigCnt[1]);
    MCPWM_SetTrigCnt(MCPWM_TIG_TMR2, stru_AdcTest_OL.TigCnt[2]);
    MCPWM_SetTrigCnt(MCPWM_TIG_TMR3, stru_AdcTest_OL.TigCnt[3]);

    /*
     * ★ 配置MCPWM计数器初始值并启动
     *   从 -EPWM_HALFPERIOD 开始计数（PWM周期起始点）
     */
    ADC_HWTrig_PWM_Config(-EPWM_HALFPERIOD);

    INFO("MCPWM Init OK (TMR0 trigger at CNT0=%d, PWM_FREQ=%dHz)\r\n",
         stru_AdcTest_OL.TigCnt[0], EPWM_FREQ);
    INFO("=== ADC_MCPWM_TRIG0_CSA_DEMO READY ===\r\n");
}
```

### 3.3 `main.c` 修改

```c
/****************************************************************************/
/** \file main.c
**
**  ADC demo for ODS_5010 (Cortex-M0+)
**  MCPWM ADC_TRIG0 触发 CSA/PGA 采样
**
**  功能描述:
**    - 使用MCPWM的ADC_TRIG0事件硬件触发ADC
**    - ADC转换序列: SEQ0→CH0(CSA0), SEQ1→CH1(CSA1),
**                    SEQ2→CH2(CSA2), SEQ3→CH7(预留)
**    - 主循环中持续读取并打印4个通道的ADC结果
**
*****************************************************************************/
#include "platform_common.h"

#include "gpio_hal.h"
#include "uart_hal.h"
#include "uart_hal_cfg.h"
#include "adc.h"
#include "adc_demo_test.h"

static void uart0_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;
    setting_cfg.gpio_port     = GPIO_A;
    setting_cfg.gpio_pin      = GPIO_PIN_1;
    setting_cfg.gpio_alt      = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir      = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg      = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    setting_cfg.gpio_port     = GPIO_A;
    setting_cfg.gpio_pin      = GPIO_PIN_2;
    setting_cfg.gpio_alt      = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir      = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg      = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
}

int main(void)
{
    delay_1us(2000);
    flash_clk_enable();
    gpio_clk_enable();
    uart_clk_enable();
    mcpwm_clk_enable();

#ifdef UART
    uart0_gpio_init();

    (void)uart_init(UART_CONSOLE_INSTANCE, &uart0_init_config);
    uart_transmit_enable(UART_CONSOLE_INSTANCE, true);

    INFO("\n SDK start running...%s,%s,%s\n", FW_VER, COMPILER_DATE, COMPILER_TIME);
    uint32_t mcu_clk = 0U;
    (void)clock_freq_get(SCG_SYS, &mcu_clk);
    INFO("MCU_CLK = %dMHz\n", mcu_clk/1000000U);
#endif

    /* ADC GPIO初始化 (PB12) */
    adc_gpio_init();

    /*
     * ★ 核心调用: MCPWM ADC_TRIG0 触发 CSA 采样
     *   内部完成:
     *     1. CSA/PGA 初始化 (使能、增益配置)
     *     2. ADC 初始化 (Group0: 4ch, SEQ0-3: CH0/1/2/7, 触发源=MCPWM_TRIG0)
     *     3. MCPWM 初始化 (PWM输出 + TMR0触发点配置)
     *     4. MCPWM计数器启动，开始产生TRIG0事件
     */
    adc_mcpwm_trig0_csa_demo();

    /*
     * 主循环: 持续读取ADC结果并打印
     *
     * ADC_GetResult(0) → 读取SEQ0结果 = ADC_CH0 = CSA通道0
     * ADC_GetResult(1) → 读取SEQ1结果 = ADC_CH1 = CSA通道1
     * ADC_GetResult(2) → 读取SEQ2结果 = ADC_CH2 = CSA通道2
     * ADC_GetResult(3) → 读取SEQ3结果 = ADC_CH7 = 预留通道
     */
    while (1)
    {
        adc_csa_ch0_val     = ADC_GetResult(0);   /* SEQ0: CSA通道0 */
        adc_csa_ch1_val     = ADC_GetResult(1);   /* SEQ1: CSA通道1 */
        adc_csa_ch2_val     = ADC_GetResult(2);   /* SEQ2: CSA通道2 */
        adc_reserved_ch_val = ADC_GetResult(3);   /* SEQ3: 预留通道 */

        INFO("CSA_CH0=%5d  CSA_CH1=%5d  CSA_CH2=%5d  CH7=%5d\r\n",
             adc_csa_ch0_val,
             adc_csa_ch1_val,
             adc_csa_ch2_val,
             adc_reserved_ch_val);

        delay_ms(100);
    }
}
```

---

## 四、代码执行流程

```
┌─────────────────────────────────────────────────────────────────┐
│                    系统上电 / 复位                               │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│  1. 时钟使能                                                      │
│     flash_clk_enable() / gpio_clk_enable() / uart_clk_enable()  │
│     mcpwm_clk_enable()                                           │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│  2. UART / GPIO 初始化 (调试输出)                                  │
│     uart0_gpio_init() / uart_init()                              │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│  3. ADC GPIO 初始化                                               │
│     adc_gpio_init() → PB12 配置为 ADC 输入                        │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│  4. adc_mcpwm_trig0_csa_demo()  ← 核心初始化函数                   │
│                                                                 │
│  4.1 CSA_Init()                                                  │
│      ├─ CSA_ENABLE(ENABLE)          使能CSA模块                   │
│      ├─ CSA_InputCH_SWMode(FIXED)   固定通道模式                  │
│      ├─ CSA_SET_InputCH(CH0)        设置初始输入通道              │
│      └─ CSA_SET_PGAGAIN(x8)         设置PGA增益                  │
│                                                                 │
│  4.2 ADC 初始化                                                  │
│      ├─ ADC_Clk_Enable()            使能ADC时钟                  │
│      ├─ ADC_ENABLE(ENABLE)          使能ADC模块                  │
│      ├─ ADC_SetSampleGrpMode(1)     1组模式                      │
│      ├─ ADC_SetGrpChCount(G0,4)     Group0: 4个通道              │
│      ├─ ADC_SEQx_CFG(SEQ0,CH0)     配置转换序列                   │
│      ├─ ADC_SEQx_CFG(SEQ1,CH1)                                   │
│      ├─ ADC_SEQx_CFG(SEQ2,CH2)                                   │
│      ├─ ADC_SEQx_CFG(SEQ3,CH7)                                   │
│      ├─ ADC_SetGrpTrigSource(G0,    ★ 触发源=MCPWM_TRIG0        │
│      │        ADC_GrpTrig_MCPWM_TRIG0)                           │
│      └─ ADC_SetCLK_xxx()            配置采样时钟                 │
│                                                                 │
│  4.3 MCPWM 初始化                                                │
│      ├─ MCPWM_ch012_init()          初始化CH0/1/2 PWM           │
│      ├─ mcpwm0_gpio_init()          GPIO配置(PWM输出引脚)        │
│      ├─ NVIC配置                    MCPWM0中断                   │
│      ├─ MCPWM0_PRT=0x0000DEAD       解锁保护寄存器               │
│      ├─ TCLK配置                    触发源选择时基0              │
│      ├─ AUEN配置                    使能自动更新                 │
│      ├─ MCPWM0_PRT=0x0000CAFE       锁定保护寄存器               │
│      ├─ MCPWM_SetTrigCnt(TMR0,      ★ 设置TRIG0触发点           │
│      │        EPWM_PERIOD/2)                                     │
│      └─ ADC_HWTrig_PWM_Config()     ★ 设置CNT0初始值并启动      │
│                                     MCPWM计数器开始运行           │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│  5. MCPWM 自动运行 (硬件自动触发)                                  │
│                                                                 │
│     MCPWM CNT0 计数:                                              │
│     0 → ... → TMR0(EPWM_PERIOD/2) → ... → EPWM_PERIOD            │
│                         │                                        │
│                         ▼                                        │
│               ★ ADC_TRIG0 事件产生                               │
│                         │                                        │
│                         ▼                                        │
│     ADC 自动开始转换:                                              │
│     SEQ0(CH0→CSA0) → SEQ1(CH1→CSA1) → SEQ2(CH2→CSA2)            │
│     → SEQ3(CH7→预留)                                             │
│                         │                                        │
│                         ▼                                        │
│     转换结果存入ADC结果寄存器                                       │
│     (每个序列对应独立的RESULT寄存器)                                │
└─────────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│  6. 主循环 (无限循环)                                              │
│                                                                 │
│     while(1) {                                                   │
│         ADC_GetResult(0) → CSA_CH0 值                            │
│         ADC_GetResult(1) → CSA_CH1 值                            │
│         ADC_GetResult(2) → CSA_CH2 值                            │
│         ADC_GetResult(3) → CH7 预留值                            │
│                                                                 │
│         INFO("CSA_CH0=%d CSA_CH1=%d CSA_CH2=%d CH7=%d\n", ...)   │
│                                                                 │
│         delay_ms(100)                                            │
│     }                                                            │
└─────────────────────────────────────────────────────────────────┘
```

---

## 五、关键设计要点

### 5.1 为什么使用1组模式（SampleGrpMode_1）

| 模式 | 说明 | 适用场景 |
|------|------|----------|
| SampleGrpMode_1 | 仅 Group0 激活 | 单一触发源，单次转换 |
| SampleGrpMode_4 | Group0-3 全部激活 | 多触发源，不同时机触发 |

本方案仅使用 **ADC_TRIG0** 一个触发源，所有4个通道统一由 TRIG0 触发进行顺序转换，因此使用1组模式即可，配置更简洁，功耗更低。

### 5.2 转换序列与CSA通道的对应关系

```
ADC硬件内部连接:
  CSA 输出0 ───► ADC_CH0 (模拟输入通道0)
  CSA 输出1 ───► ADC_CH1 (模拟输入通道1)
  CSA 输出2 ───► ADC_CH2 (模拟输入通道2)
  外部引脚   ───► ADC_CH7 (模拟输入通道7, PB12)

ADC转换序列:
  TRIG0触发 → 多路选择器切换至CH0 → 采样 → 转换 → 存SEQ0_RESULT
           → 多路选择器切换至CH1 → 采样 → 转换 → 存SEQ1_RESULT
           → 多路选择器切换至CH2 → 采样 → 转换 → 存SEQ2_RESULT
           → 多路选择器切换至CH7 → 采样 → 转换 → 存SEQ3_RESULT
```

### 5.3 MCPWM触发点选择

```
触发点 TMR0 = EPWM_PERIOD / 2

MCPWM CNT0 (中心对齐PWM, 完整周期):
        EPWM_PERIOD ─┐
                     │        ┌─────────┐
                     │        │         │
      TMR0 ──────────┼────────┼──★TRIG0  │  ← 在PWM中点触发ADC
                     │        │         │
          0 ─────────┼────────┼─────────┤
                     │        │         │
                     │        │         │
    -EPWM_PERIOD ────┘        └─────────┘

选择PWM周期中点的原因:
  - 避开PWM开关时刻的电磁干扰(EMI)
  - 采样点远离开关管导通/关断边沿
  - 电流波形在此处最稳定
```

### 5.4 CSA PGA 增益配置

```c
/* 可选增益值 */
#define CSA_PGA_GAIN_X1   0x00   /* 增益 x1   */
#define CSA_PGA_GAIN_X2   0x01   /* 增益 x2   */
#define CSA_PGA_GAIN_X4   0x02   /* 增益 x4   */
#define CSA_PGA_GAIN_X8   0x03   /* 增益 x8   */
#define CSA_PGA_GAIN_X12  0x04   /* 增益 x12  */
#define CSA_PGA_GAIN_X16  0x05   /* 增益 x16  */
#define CSA_PGA_GAIN_X32  0x06   /* 增益 x32  */
#define CSA_PGA_GAIN_X64  0x07   /* 增益 x64  */

/* 配置示例 */
CSA_SET_PGAGAIN(CSA_PGA_GAIN_X8);  /* 设置为8倍增益 */
```

---

## 六、时序图

```
MCPWM CNT0:    0 ──→ TMR0 ──→ TH ──→ TH ──→ TMR0 ──→ 0 ──→ 0 ──→ TMR0 ──→ ...
                         │                    │                    │
ADC_TRIG0:      ─────────┼────────────────────┼────────────────────┼─────────
                         ▼                    ▼                    ▼
                         
ADC转换:         [CH0→CH1→CH2→CH7]     [CH0→CH1→CH2→CH7]     [CH0→CH1→CH2→CH7]
                  ←── 转换时间 ──→       ←── 转换时间 ──→       ←── 转换时间 ──→

ADC结果更新:              ▼                    ▼                    ▼
                  SEQ0/1/2/3 RESULT    SEQ0/1/2/3 RESULT    SEQ0/1/2/3 RESULT

主循环读取:      ───────┼────────────────────┼────────────────────┼─────────
                         ▼                    ▼                    ▼
                  ADC_GetResult(0/1/2/3) ADC_GetResult(0/1/2/3) ADC_GetResult(0/1/2/3)
```

---

## 七、文件修改清单

| 序号 | 文件 | 修改内容 |
|------|------|----------|
| 1 | `adc_demo_test.h` | 新增 CSA 头文件引用、宏定义、CSA初始化函数声明、`adc_mcpwm_trig0_csa_demo()` 声明 |
| 2 | `adc_demo_test.c` | 新增 `CSA_Init()`、重构 `adc_mcpwm_trig0_csa_demo()` 使用 TRIG0+4序列 |
| 3 | `main.c` | 调用 `adc_mcpwm_trig0_csa_demo()`，主循环读取4通道结果 |
| 4 | `isr.c` | (无需修改，MCPWM0_IRQHandler 可保持不变) |

---

## 八、调试与验证

### 8.1 验证步骤

1. **编译下载**：在 Keil MDK 中编译项目，下载到目标板
2. **串口观察**：连接 UART0 (PA1/PA2)，波特率按配置，观察输出
3. **预期输出**：
   ```
   SDK start running...
   MCU_CLK = 48MHz
   === ADC_MCPWM_TRIG0_CSA_DEMO START ===
   CSA Init OK (Gain=x8, Mode=Fixed_CH)
   ADC Init OK (Group0: 4ch, SEQ0-3: CH0/CH1/CH2/CH7, Trig: MCPWM_TRIG0)
   MCPWM Init OK (TMR0 trigger at CNT0=1200, PWM_FREQ=10000Hz)
   === ADC_MCPWM_TRIG0_CSA_DEMO READY ===
   CSA_CH0= 2048  CSA_CH1= 2050  CSA_CH2= 2047  CH7= 1024
   CSA_CH0= 2049  CSA_CH1= 2051  CSA_CH2= 2048  CH7= 1025
   ...
   ```

### 8.2 常见问题排查

| 问题 | 可能原因 | 解决方案 |
|------|----------|----------|
| ADC结果全为0 | ADC未使能或触发源配置错误 | 检查 `ADC_ENABLE()` 和 `ADC_SetGrpTrigSource()` |
| ADC结果不变 | MCPWM计数器未启动 | 检查 `ADC_HWTrig_PWM_Config()` 和 TCLK bit[6] |
| CSA通道值异常 | CSA未使能或增益设置不当 | 检查 `CSA_ENABLE()` 和 `CSA_SET_PGAGAIN()` |
| 无串口输出 | UART未正确初始化 | 检查 PA1/PA2 GPIO 配置 |

---

## 九、与原方案的对比

| 项目 | 原方案 (4组全用) | 新方案 (仅TRIG0) |
|------|-----------------|-----------------|
| **分组模式** | SampleGrpMode_4 (4组) | SampleGrpMode_1 (1组) |
| **触发源** | TRIG0/1/2/3 各触发一组 | TRIG0 触发全部 |
| **有效通道** | Group0:1ch, Group1:3ch | Group0:4ch |
| **转换序列** | 16个序列全部配置 | 仅前4个序列有效 |
| **CSA集成** | 无 | 完整CSA初始化 |
| **代码复杂度** | 高 | 低 |
| **功耗** | 4组全激活 | 仅1组激活 |
| **适用场景** | 多电机复杂PWM同步采样 | 单触发源周期性采样 |

---

## 十、总结

本方案实现了基于 **MCPWM ADC_TRIG0** 的硬件触发ADC采样方案，核心要点：

1. **ADC分组**：仅使用 Group0，包含4个通道
2. **转换序列**：SEQ0→CH0(CSA0), SEQ1→CH1(CSA1), SEQ2→CH2(CSA2), SEQ3→CH7(预留)
3. **触发方式**：MCPWM CNT0 计数匹配 TMR0 时自动产生 ADC_TRIG0 硬件触发信号
4. **CSA配置**：完整初始化CSA模块，使能运放和PGA增益
5. **无需CPU干预**：触发和转换过程完全由硬件自动完成，CPU仅需在需要时读取结果