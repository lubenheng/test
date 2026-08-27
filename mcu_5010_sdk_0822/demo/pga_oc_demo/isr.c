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
#include "CSA_Reg.h"   
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

    
    if (irq_status & TIMERx_IRQ_CH0)
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



void CSA_IRQHandler(void)
{
   *(volatile uint32_t*)0x40007C14 &= ~(1 << 10);//0x40007C14[10]=0
    /* PB8 翻转: 每进入一次 CSA 中断翻转一次(示波器/万用表观察) */
   
    gpio_output_set(GPIO_B, GPIO_PIN_8,GPIO_HIGH_LEVEL);

	*(volatile uint32_t*)0x40007C14 |= (1 << 10);//0x40007C14[10]=1
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

