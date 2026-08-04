#ifndef ADC_DEMO_TEST_H
#define ADC_DEMO_TEST_H

#include "platform_common.h"
#include "mcpwm_type.h"
#include "adc_hal.h"

/*----------------------------------------------------------------------------
   ADC demo 配置
 *----------------------------------------------------------------------------*/
#define ADC_DEMO_CHANNEL    ADC_CH7
#define ADC_DEMO_GPIO_PORT  GPIO_B
#define ADC_DEMO_GPIO_PIN   GPIO_PIN_12

extern mcpwm_shadow        stru_mcpwm_shadow;
extern uint32_t            adc_val;
extern struct_AdcTest_OL   stru_AdcTest_OL;

void adc_gpio_init(void);
void mcpwm0_gpio_init(void);
void ADC_HWTrig_PWM_Config(int InitCnt);
void aadc_mcpwm_trigger_demo(void);

#endif
