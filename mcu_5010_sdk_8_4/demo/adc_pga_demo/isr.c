/****************************************************************************/
/** \file isr.c
**
**  Trimmed exception handlers for the GPIO demo. GPIO_A/B/C_IRQHandler are
**  NOT redefined here: system/hal/gpio_hal.c already provides real
**  implementations, and startup_ARMCM0plus.s exports every other peripheral
**  IRQHandler as [WEAK] falling through to Default_Handler, so they don't
**  need stubs here either.
*****************************************************************************/
#include "platform_common.h"
#include "adc_demo_test.h"
#include "ADC_Type.h"
#include "ADC_Reg.h"
#include "adc.h"
#include "TIM_Reg.h"
#include "TIM_Type.h"
#include "TIM.h"
#include "gpio_hal.h"
uint8_t irq_status;
static uint8_t pb_level = 0;
/****************************************************************************/
/*	Local pre-processor symbols/macros('#define')
*****************************************************************************/

/****************************************************************************/
/*	Global variable definitions(declared in header file with 'extern')
*****************************************************************************/
//extern void DelayTime_ms(unsigned int delay);

/****************************************************************************/
/*	Local type definitions('typedef')
*****************************************************************************/

/****************************************************************************/
/*	Local variable  definitions('static')
*****************************************************************************/

/****************************************************************************/
/*	Local function prototypes('static')
*****************************************************************************/

/****************************************************************************/
/*	Function implementation - global ('extern') and local('static')
*****************************************************************************/
     
/****************************************************************************
 ** \brief	NMI_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void NMI_Handler(void)
{
	//INFO("IWDT interrupt! \n");
}

/****************************************************************************
 ** \brief	HardFault_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/

void HardFault_Handler(void)
{
	//INFO("Hardfault! \n");
}

/****************************************************************************
 ** \brief	SVC_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void SVC_Handler(void)
{
	
}

/****************************************************************************
 ** \brief	PendSV_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void PendSV_Handler(void)
{
	
}

/****************************************************************************
 ** \brief	SysTick_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void SysTick_Handler(void)
{

	
}

/****************************************************************************
 ** \brief	PMU_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 0
****************************************************************************/
// void PMU_IRQHandler(void)
// {
//	
// }
/****************************************************************************
 ** \brief	Code_Flash_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 1
****************************************************************************/
void Code_Flash_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	UART0_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 2
****************************************************************************/
//void UART0_IRQHandler(void)
//{
//	
//}

/****************************************************************************
 ** \brief	UART1_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 3
****************************************************************************/
//void UART1_IRQHandler(void)
//{
//	
//}

 /****************************************************************************
  ** \brief	SPI0_IRQHandler
  **
  ** \param [in]  none   
  ** \return none
  ** \note 	number 4
 ****************************************************************************/
 void SPI0_IRQHandler(void)
 {
	
 }

 /****************************************************************************
  ** \brief	SPI1_IRQHandler
  **
  ** \param [in]  none   
  ** \return none
  ** \note 	number 5
 ****************************************************************************/
 void SPI1_IRQHandler(void)
 {
	
 }

/****************************************************************************
 ** \brief	MCPWM1_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 6
****************************************************************************/

// void MCPWM0_IRQHandler(void)
// {

//	
// }

/****************************************************************************
 ** \brief	MCPWM2_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 7
****************************************************************************/
// void MCPWM1_IRQHandler(void)
// {

//	
// }

/****************************************************************************
 ** \brief	I2C0_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 8
****************************************************************************/
void I2C0_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	I2C1_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 9
****************************************************************************/
void I2C1_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	GPIO_A_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 10
****************************************************************************/
// void GPIO_A_IRQHandler(void)
// {
//	
// }

/****************************************************************************
 ** \brief	GPIO_B_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 11
****************************************************************************/
// void GPIO_B_IRQHandler(void)
// {
//	
// }

/****************************************************************************
 ** \brief	GPIO_C_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 12
****************************************************************************/
// void GPIO_C_IRQHandler(void)
// {
//	
// }

/****************************************************************************
 ** \brief	TIM13_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 13
****************************************************************************/
void TIM13_IRQHandler(void)
{
 irq_status = Timer_drv_IRQ_Status_Get(TIMER13);

    
    if (irq_status & (1U << TIMERx_IRQ_CH0))
    {
        pb_level = !pb_level;
        gpio_output_set(GPIO_C, GPIO_PIN_6, pb_level?GPIO_HIGH_LEVEL:GPIO_LOW_LEVEL);
       
    }

}

/****************************************************************************
 ** \brief	TIM14_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 14
****************************************************************************/
void TIM14_IRQHandler(void)
{

}

/****************************************************************************
 ** \brief	TIM15_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 15
****************************************************************************/
void TIM15_IRQHandler(void)
{

}

/****************************************************************************
 ** \brief	TIM16_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 16
****************************************************************************/
void TIM16_IRQHandler(void)
{

}

/****************************************************************************
 ** \brief	ADC_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 17
****************************************************************************/
void ADC_IRQHandler(void)
{
	static uint16_t calib_cnt = 0;
	static int64_t sum_ch0 = 0;
	static int64_t sum_ch1 = 0;
	static int64_t sum_ch2 = 0;
	static int64_t sum_ch7 = 0;
	uint32_t raw0, raw1, raw2, raw3;

	/* 1. 读取 ADC 中断源 */
	uint32_t irq_src = ADC_Get_IrqSrc();

	/* 2. Group0 转换完成中断 */
	if (irq_src & (1UL << ADC_IRQSRC_S0CP))
	{
		/* 写1清零 S0CDIF 中断标志位，必须在读取数据前清除 */
		ADC->IR.S0CDIF = 1;

		raw0 = ADC_GetResult(0);
		raw1 = ADC_GetResult(1);
		raw2 = ADC_GetResult(2);
		raw3 = ADC_GetResult(3);
		

		/* 3. 初始化阶段: 零偏校准 */
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
				INFO("Calibration done! Offset: CH0=%d CH1=%d CH2=%d CH7=%d\r\n",
					 g_offset_ch0, g_offset_ch1, g_offset_ch2, g_offset_ch7);
			}
		}
		else
		{
			/*
			 * 4. 校准后: 读取原始值 → 扣除零偏 → 转换为电压(mV)
			 *    Vref=2.5V, 12位ADC: mV = (raw - offset) * 2500 / 4095
			 */
			adc_csa_ch0_val     = (((int32_t)raw0 - g_offset_ch0) * 2500) / 4095;
			adc_csa_ch1_val     = (((int32_t)raw1 - g_offset_ch1) * 2500) / 4095;
			adc_csa_ch2_val     = (((int32_t)raw2 - g_offset_ch2) * 2500) / 4095;
			adc_reserved_ch_val = (((int32_t)raw3 - g_offset_ch7) * 2500) / 4095;
		}
	}
}

/****************************************************************************
 ** \brief	HALL_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 18
****************************************************************************/
 void HALL_IRQHandler(void)
 {

	
 }

/****************************************************************************
 ** \brief	QEP_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 19
****************************************************************************/
void QEP_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	DMA_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 20
****************************************************************************/
void DMA_IRQHandler(void)
{			

}

/****************************************************************************
 ** \brief	Reserved21_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 21
****************************************************************************/
void Reserved21_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	Reserved22_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 22
****************************************************************************/
void Reserved22_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	Reserved23_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 23
****************************************************************************/
void Reserved23_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	Reserved24_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 24
****************************************************************************/
void Reserved24_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	Reserved25_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 25
****************************************************************************/
void Reserved25_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	Reserved26_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 26
****************************************************************************/
void Reserved26_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	Reserved27_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 27
****************************************************************************/
void Reserved27_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	Reserved28_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 28
****************************************************************************/
void Reserved28_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	Reserved29_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 29
****************************************************************************/
void Reserved29_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	Reserved30_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 30
****************************************************************************/
void Reserved30_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	Reserved31_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 31
****************************************************************************/
void Reserved31_IRQHandler(void)
{
	
}

