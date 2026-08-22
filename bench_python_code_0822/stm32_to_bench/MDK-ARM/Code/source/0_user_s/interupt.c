#include "interupt.h"
#include "hard_init.h"
int16_t adc_Inject_cnt;
int16_t TIM_cnt;

//ADC×¢ÈëÖÐ¶Ï
void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc)
{
	
if (hadc->Instance == ADC2)
    {
			adc_Inject_cnt++;
    }
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM15)
	{
		TIM_cnt++;
		if(TIM_cnt==100)
		{
			TIM_cnt=0;
			LED_Toggle(1);
		}
	
	}

}
//breakÖÐ¶Ï
void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim)
{
	 if (htim->Instance == TIM1)
	 {
		
		 
	 
	 }
	 
}
