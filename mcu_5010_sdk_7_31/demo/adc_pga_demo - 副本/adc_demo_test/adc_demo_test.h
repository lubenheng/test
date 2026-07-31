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
#define ADC_DEMO_CHANNEL    ADC_CH7
#define ADC_DEMO_GPIO_PORT  GPIO_B
#define ADC_DEMO_GPIO_PIN   GPIO_PIN_12

/* CSA对应的ADC通道 */
#define ADC_CSA_CH0             ADC_CH0    /* CSA通道0 -> ADC_CH0 */
#define ADC_CSA_CH1             ADC_CH1    /* CSA通道1 -> ADC_CH1 */
#define ADC_CSA_CH2             ADC_CH2    /* CSA通道2 -> ADC_CH2 */
#define ADC_RESERVED_CH         ADC_CH7    /* 预留通道  -> ADC_CH7 */

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

void adc_gpio_init(void);
void mcpwm0_gpio_init(void);
void CSA_Init(void);
void ADC_HWTrig_PWM_Config(int InitCnt);
void aadc_mcpwm_trigger_demo(void);
void adc_mcpwm_trig0_csa_demo(void);
void print_all_channels(void);

#endif
