#ifndef ADC_DEMO_TEST_H
#define ADC_DEMO_TEST_H

#include "platform_common.h"
#include "mcpwm_type.h"
#include "adc_hal.h"
#include "CSA_Type.h"
#include "CSA.h"
#include "TIM_Reg.h"
#include "TIM_Type.h"
#include "TIM.h"

/*----------------------------------------------------------------------------
   CSA/PGA 配置
 *----------------------------------------------------------------------------*/
#define CSA_PGA_GAIN            CSA_PGA_GAIN_X8      /* PGA增益: x8 */
#define CSA_INPUT_MODE          CSA_FIXED_CH_MODE     /* 固定通道模式 */

/*----------------------------------------------------------------------------
   CSA 过流(OC)保护配置  —— 对应 MISC_CSACR0/CSACR1, 由 CSA_OC_Init() 使用
 *----------------------------------------------------------------------------*/
#define CSA_OC_EN               (1U)     /* RW_CSA_OC_EN   : 1=使能 OC 检测 */
#define CSA_OC_SEL_VALUE        (2U)     /* RW_CSA_OC_SEL  : 2=csp2(GPIO11/12, 与固定通道CH2对应) */
#define CSA_OC_HYS_VALUE        (2U)     /* RW_CSA_OC_HYS  : 2=24mV 迟滞 */
#define CSA_OC_NG_EN            (1U)     /* RW_CSA_OC_NG_EN: 1=消抖使能 */
#define CSA_OC_NG_TIME          (1U)     /* RW_CSA_OC_NG   : 1=1us 消抖时长 */
#define CSA_DAC_EN              (1U)     /* RW_CSA_DAC_EN  : 1=DAC 使能(提供OC阈值) */
#define CSA_DAC_REF_SEL         (0U)     /* RW_CSA_DAC_REF : 0=1.2V 量程 */
#define CSA_DAC_OUT_VALUE       (171U)   /* RW_CSA_DAC_OUT : 8bit 阈值码(需按硬件标定) */

/*----------------------------------------------------------------------------
   ADC demo 配置
 *----------------------------------------------------------------------------*/
#define ADC_DEMO_CHANNEL    ADC_CH7       /* 预留通道  -> ADC_CH7 */


/* CSA对应的ADC通道 */
#define ADC_CSA_CH0             ADC_CH0    /* CSA通道0 -> ADC_CH0 */
#define ADC_CSA_CH1             ADC_CH1    /* CSA通道1 -> ADC_CH1 */
#define ADC_CSA_CH2             ADC_CH2    /* CSA通道2 -> ADC_CH2 */


extern mcpwm_shadow        stru_mcpwm_shadow;
extern uint32_t            adc_val;
extern struct_AdcTest_OL   stru_AdcTest_OL;

/* CSA 采样结果缓存 */
extern int32_t adc_csa_ch0_val;
extern int32_t adc_csa_ch1_val;
extern int32_t adc_csa_ch2_val;
extern int32_t adc_reserved_ch_val;

/* 零偏校准 */
#define CALIB_SAMPLES   64          /* 校准采样次数 */
extern int32_t g_offset_ch0;
extern int32_t g_offset_ch1;
extern int32_t g_offset_ch2;
extern int32_t g_offset_ch7;
extern uint8_t g_calib_done;

/* CSA 过流中断标志(在 isr.c 定义, 由 CSA_IRQHandler 中断置位) */
extern volatile uint8_t g_csa_oc_flag;

void adc_gpio_init(void);
void mcpwm0_gpio_init(void);
void CSA_Init(void);
void CSA_OC_Init(void);
void CSA_OC_IRQ_Init(void);         /* CSA 过流中断使能(IRQ23) */
void ADC_HWTrig_PWM_Config(int InitCnt);
void aadc_mcpwm_trigger_demo(void);
void adc_mcpwm_trig0_csa_demo(void);
void adc_timer_trig_csa_demo(void);
void print_all_channels(void);
void TIMER_ADC_Init(void);
static void csa_pin_make_analog(uint32_t pin_mask);
void csa_gpio_init(void);

#endif
