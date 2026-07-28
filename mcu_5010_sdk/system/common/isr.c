/****************************************************************************/
/** \file isr.c
**
**	History:
**	
*****************************************************************************/
/****************************************************************************/
/*	include files
*****************************************************************************/
#include "platform_common.h"



#include "mcpwm_reg.h"
//#include "device_resource.h"
//#include "mcpwm_hal.h"
//#include "mcpwm.h"
#include "gpio_hal.h"
#include "TIM_hal.h"
#include "DMA_hal.h"
//extern void MCPWM0_Call_Back(void);

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
	INFO("IWDT interrupt! \n");
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
	INFO("Hardfault! \n");
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
uint32_t g_systick_count = 0;
void SysTick_Handler(void)
{
	if(g_systick_count == 0)
	{
		gpio_output_set(GPIO_C,GPIO_PIN_8,GPIO_HIGH_LEVEL);    // 高电平

	}
	else if(g_systick_count >= 1U)
	{
		gpio_output_set(GPIO_C,GPIO_PIN_8,GPIO_LOW_LEVEL);    // 低电平
		// 关闭SysTick
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
	
		NVIC_ClearPendingIRQ(SysTick_IRQn);
		NVIC_DisableIRQ (SysTick_IRQn);
	}
	
	g_systick_count++;  // 每进入一次中断，计数器加1[6](@ref)
	
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

// /****************************************************************************
//  ** \brief	SPI0_IRQHandler
//  **
//  ** \param [in]  none   
//  ** \return none
//  ** \note 	number 4
// ****************************************************************************/
// void SPI0_IRQHandler(void)
// {
	
// }

// /****************************************************************************
//  ** \brief	SPI1_IRQHandler
//  **
//  ** \param [in]  none   
//  ** \return none
//  ** \note 	number 5
// ****************************************************************************/
// void SPI1_IRQHandler(void)
// {
	
// }

/****************************************************************************
 ** \brief	MCPWM1_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 6
****************************************************************************/
// uint16_t mcpwm0_intp_cnt_tmp = 0;
// uint16_t mcpwm0_intp_cnt_tmp1 = 0;
// void MCPWM0_IRQHandler(void)
// {
// 	mcpwm0_intp_cnt_tmp++;
// 	MCPWM0_IF0 = 0xFFFFFFFFUL;
//	if(mcpwm0_intp_cnt_tmp == 1000)
//	{
//		mcpwm0_intp_cnt_tmp = 0;
//		mcpwm0_intp_cnt_tmp1++;
//		INFO("mcpwm0_intp_cnt %d \n",mcpwm0_intp_cnt_tmp1);
//	}
//	
// }

/****************************************************************************
 ** \brief	MCPWM2_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 7
****************************************************************************/
// uint16_t mcpwm1_intp_cnt = 0;
// void MCPWM1_IRQHandler(void)
// {
// 	mcpwm1_intp_cnt++;
// 	//MCPWM0_IF1 = 0xFFFFFFFFUL;
	
// 	(*((volatile uint32_t *)(0x40010C8C))) = 0xFFFFFFFFUL;
	
// }

/****************************************************************************
 ** \brief	I2C0_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 8
****************************************************************************/
//void I2C0_IRQHandler(void)
//{
//	
//}

/****************************************************************************
 ** \brief	I2C1_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 9
****************************************************************************/
//void I2C1_IRQHandler(void)
//{
//	
//}

/****************************************************************************
 ** \brief	GPIO_A_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 10
****************************************************************************/
// void GPIO_A_IRQHandler(void)
// {
	
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
	uint8_t status = tim_hal_irq_status();
	uint32_t cmp0 = tim_hal_ch0_capnum()+1;
	uint32_t cmp1 = tim_hal_ch1_capnum()+1;
	INFO("flag:%d cmp0:%d cmp1:%d duty:%f\n", status, cmp0, cmp1, (((float)cmp1 / (float)cmp0) * 100));

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
	uint8_t status = tim_hal_irq_status();
	uint32_t cmp0 = tim_hal_ch0_capnum()+1;
	uint32_t cmp1 = tim_hal_ch1_capnum()+1;
	INFO("flag:%d cmp0:%d cmp1:%d duty:%f\n", status, cmp0, cmp1, (((float)cmp1 / (float)cmp0) * 100));
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
	uint8_t status = tim_hal_irq_status();
	uint32_t cmp0 = tim_hal_ch0_capnum()+1;
	uint32_t cmp1 = tim_hal_ch1_capnum()+1;
 
	INFO("flag:%d cmp0:%d cmp1:%d duty:%f\n", status, cmp0, cmp1, (((float)cmp1 / (float)cmp0) * 100));
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
	uint8_t status = tim_hal_irq_status();
	uint32_t cmp0 = tim_hal_ch0_capnum()+1;
	uint32_t cmp1 = tim_hal_ch1_capnum()+1;
	INFO("flag:%d cmp0:%d cmp1:%d duty:%f\n", status, cmp0, cmp1, (((float)cmp1 / (float)cmp0) * 100));
}

///****************************************************************************
// ** \brief	ADC_IRQHandler
// **
// ** \param [in]  none   
// ** \return none
// ** \note 	number 17
//****************************************************************************/
//void ADC_IRQHandler(void)
//{
//	
//}

/****************************************************************************
 ** \brief	HALL_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 18
****************************************************************************/
// uint32_t g_hall_cnt = 0;
// void HALL_IRQHandler(void)
// {
// 	g_hall_cnt++;
	
// }

///****************************************************************************
// ** \brief	QEP_IRQHandler
// **
// ** \param [in]  none   
// ** \return none
// ** \note 	number 19
//****************************************************************************/
//void QEP_IRQHandler(void)
//{
//	
//}

/****************************************************************************
 ** \brief	DMA_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 20
****************************************************************************/

#include "dma.h"
volatile static uint16_t MCPWM_srcaddr_cnt = 0;  // 改为0
volatile uint16_t freq_len_it = 0;
static uint8_t first_run = 1;  // 添加首次运行标志
volatile uint16_t current_th0 = 0;
volatile uint16_t th00 = 0;
volatile uint16_t th01 = 0;
//extern volatile uint8_t dma_trans_complete  ;
void DMA_IRQHandler(void)
{			
	// 检查是否是预期的DMA传输
	if(DMA_Type->DMA_DEST0_STR.DMA_Dest0 != (uint32_t)&MCPWM0->TH0)
	{
				// 遍历6个DMA通道，统一处理中断
			for (uint8_t ch_idx = 0; ch_idx < 6; ch_idx++)
			{
					DMA_Channel_e curr_ch ;
					
				// 1. 检查传输完成中断（所有通道都要判断）
				if (1 == HAL_DMA_Get_status(curr_ch, DMA_STATUS_TRANS_COMPLETE))
				{
						dma_trans_complete = 1;
						INFO("\r\n DMA CH%d TX OVER\n", curr_ch);
						HAL_DMA_IT_Transfer_Clean(curr_ch); // 清除对应通道完成标志									
				}
				
				// 2. 检查数据错误中断
				if (1 == HAL_DMA_Get_status(curr_ch, DMA_STATUS_DATA_ERROR))
				{
						INFO("DMA CH%d DATA ERROR \n", curr_ch);
						HAL_DMA_IT_Error_Clean(curr_ch); // 清除对应通道数据错误标志
				}
				
				// 3. 检查配置错误中断
				if (1 == HAL_DMA_Get_status(curr_ch, DMA_STATUS_CONFIG_ERROR))
				{
						INFO("DMA CH%d CONFIG ERROR \n", curr_ch);
						HAL_DMA_IT_Configerror_Clean(curr_ch); // 清除对应通道配置错误标志
				}
			}
			return;
	}
  // 立即清除中断标志
	DMA_Type->DMA_CLR_STR.DMA_Clr = (1 << 0);  
	// 停止DMA通道
	DMA_Type->DMA_CTL0_STR.Bits.CHEN = 0;
	
	// 只在第一次运行时配置MCPWM
	if(first_run)
	{
			first_run = 0;
			MCPWM_Dsiable(0); 
			for(int cnt = 0; cnt < 0x5F; cnt++)
			{
					;
			}
			MCPWM_Enable(0); 
	}
	
	// 保存当前TH0值用于调试
	current_th0 = MCPWM0->TH0;
 
	// 解除写保护
	MCPWM0->PRT = (uint32_t)0xDEAD;
	MCPWM0->RE = 0;  // 关闭DMA使能

	MCPWM0->TH00 = (uint16_t)((-16384 * current_th0 + 16384) >> 15);
	MCPWM0->TH01 = ((16384 * current_th0 + 16384) >> 15);
	th00 = (uint16_t)((-16384 * current_th0 + 16384) >> 15);
	th01 = ((16384 * current_th0 + 16384) >> 15);
	// 数据同步屏障
	__DSB();
	
	MCPWM0->RE = 0xFFFF;  // 只使能通道0，而不是0xFFFFFFFF
	// 短延时确保硬件更新
	for(volatile int cnt = 0; cnt < 0xFF; cnt++)
	{
			;
	}

	// 更新索引（先加，再判断）
	MCPWM_srcaddr_cnt++;
	
	// 检查是否到达数组末尾
	if(MCPWM_srcaddr_cnt >= freq_len_it)
	{            			  
			MCPWM_srcaddr_cnt = 0;
			// 到达末尾，不重新配置DMA，直接返回
			INFO("MCOWM THx Set   TH00 0x%04x, TH0=0x%04x, cnt end\n", 
					 th00, current_th0);
			return;
	}
	
	// 配置下一次DMA传输
	DMA_Type->DMA_SRC0_STR.DMA_Source0 = (uint32_t)&freq_buff[MCPWM_srcaddr_cnt];
	DMA_Type->DMA_MAX0_STR.DMA_Max0 = 1;
	DMA_Type->DMA_DEST0_STR.DMA_Dest0						 = (uint32_t)&MCPWM0->TH0;  
	// 重新使能MCPWM的DMA请求（使能特定通道）

	// 重新使能DMA通道
	DMA_Type->DMA_CTL0_STR.Bits.CHEN = 1;
	// 重新写保护
	MCPWM0->PRT = (uint32_t)0xAAAA;
	INFO("MCOWM THx Set   TH00 0x%04x, TH0=0x%04x, cnt=%d\n", 
			 th00, current_th0, MCPWM_srcaddr_cnt);
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
