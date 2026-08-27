#ifndef ADC_DEMO_TEST_H
#define ADC_DEMO_TEST_H

#include "platform_common.h"
#include "TIM_Reg.h"
#include "TIM_Type.h"
#include "TIM.h"
#include "adc_hal.h"

/*----------------------------------------------------------------------------
   ADC demo 配置
 *----------------------------------------------------------------------------*/

extern uint32_t            adc_val;
extern struct_AdcTest_OL   stru_AdcTest_OL;

void adc_gpio_init(void);
void pb_gpio_init(void);
void adc_timer_trigger_demo(void);
void TIMER_ADC_Init(void);
void ADC_ConfigDirect(uint8_t vref_range,
                      uint8_t vrefh_to_vdd33,
                      uint8_t vrefh_en,
                      uint8_t adc_clk_sel,
                      uint8_t adc_en);
#endif

