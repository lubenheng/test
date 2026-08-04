#include "platform_common.h"

#include "gpio_hal.h"

#include "adc_reg.h"
#include "device_resource.h"
#include "adc_hal.h"
#include "adc.h"
#include "ADC_Type.h"

extern struct_AdcTest_OL stru_AdcTest_OL;
void ADC_Data_Check(void);
void ADCTest_Config_OL(void);
/****************************************************************************
 ** \brief	ADC_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 17
****************************************************************************/
u8 AdcState;
void ADC_IRQHandler(void)
{

	gpio_output_set(GPIO_B,GPIO_PIN_8,GPIO_HIGH_LEVEL);
//	ADC_Data_Check();
	
	stru_AdcTest_OL.INTR_SRC_DONE = ADC->IR.Word & ADC->IE.Word;
	ADC->IR.Word = 0xFFFFFFFFUL;
	
	//ADCTest_Config_OL();
	gpio_output_set(GPIO_B,GPIO_PIN_8,GPIO_LOW_LEVEL);
}








