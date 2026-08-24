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

 void MCPWM0_IRQHandler(void)
 {

	
 }

/****************************************************************************
 ** \brief	MCPWM2_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 7
****************************************************************************/
 void MCPWM1_IRQHandler(void)
 {

	
 }

 #if 0
/****************************************************************************
 ** \brief	I2C0_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 8
****************************************************************************/
/**
*@brief  Implementation of I2C0_IRQHandler.
*@param  none
*@return none
*/
void I2C0_IRQHandler(void)      //use
{    
		I2C_TypeDef *i2c_ptr = i2c_addr_get((uint32_t)0);

		
        if(i2c_ptr->IC_INTR_STAT.R_TX_EMPTY){
            if(g_i2c_master.tx_length == 0){    //当缓冲区为空时关闭FIFO为空中断
                i2c_ptr->IC_INTR_MASK.M_TX_EMPTY = 0;
                (void)i2c_ptr->IC_CLR_INTR; // 清除全部可清除标志位
				return;
            }
            if(g_i2c_master.i2c_transmit_status == true){  
                i2cm_drv_tdata_send(i2c_ptr, I2C_MASTER_CMD_WRITE, g_i2c_master.tx_data[I2C_DEFAULT_INDEX_0]);
            } else {
                i2cm_drv_tdata_send(i2c_ptr, I2C_MASTER_CMD_READ, g_i2c_master.tx_data[I2C_DEFAULT_INDEX_0]);
            }
            g_i2c_master.tx_data++;
            g_i2c_master.tx_length--;
        }

        
        
        if(i2c_ptr->IC_INTR_STAT.R_RX_FULL){    //接收缓冲区有数据
            *(g_i2c_master.rx_data) = i2c_ptr->IC_DATA_CMD.DAT;
            if(g_i2c_master.rx_length == 0){    //收到指定长度的数据
                i2c_ptr->IC_INTR_MASK.M_RX_FULL = 0;
                (void)i2c_ptr->IC_CLR_INTR; // 清除全部可清除标志位
                return;
            }
            g_i2c_master.rx_data++;
            g_i2c_master.rx_length--;
            
        }
        (void)i2c_ptr->IC_CLR_INTR; // 清除全部可清除标志位
#if 0

		I2C_TypeDef *i2c_ptr = i2c_addr_get((uint32_t)0);
		(void)i2c_ptr->IC_CLR_INTR; // 清除全部可清除标志位
		(void)i2c_ptr->IC_CLR_RX_UNDER; // 清除RX_UNDER标志位
		if (i2c_ptr->IC_CON.MASTER_MODE == 1)
		{
			i2c_master_int_handle((uint32_t)0);
		}
		else
		{
			i2c_slave_int_handle((uint32_t)0);
		}
#endif
}





/****************************************************************************
 ** \brief	I2C1_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 9
****************************************************************************/
void I2C1_IRQHandler(void)      //use
{    
    I2C_TypeDef *i2c_ptr = i2c_addr_get((uint32_t)0);
    if(i2c_ptr->IC_INTR_STAT.R_TX_EMPTY){
        if(g_i2c_master.tx_length == 0){    //当缓冲区为空时关闭FIFO为空中断
            i2c_ptr->IC_INTR_MASK.M_TX_EMPTY = 0;
            (void)i2c_ptr->IC_CLR_INTR; // 清除全部可清除标志位
			return;
        }
        if(g_i2c_master.i2c_transmit_status == true){  
            i2cm_drv_tdata_send(i2c_ptr, I2C_MASTER_CMD_WRITE, g_i2c_master.tx_data[I2C_DEFAULT_INDEX_0]);
        } else {
            i2cm_drv_tdata_send(i2c_ptr, I2C_MASTER_CMD_READ, g_i2c_master.tx_data[I2C_DEFAULT_INDEX_0]);
        }
        g_i2c_master.tx_data++;
        g_i2c_master.tx_length--;
    }
    
    
    if(i2c_ptr->IC_INTR_STAT.R_RX_FULL){    //接收缓冲区有数据
        *(g_i2c_master.rx_data) = i2c_ptr->IC_DATA_CMD.DAT;
        if(g_i2c_master.rx_length == 0){    //收到指定长度的数据
            i2c_ptr->IC_INTR_MASK.M_RX_FULL = 0;
            (void)i2c_ptr->IC_CLR_INTR; // 清除全部可清除标志位
            return;
        }
        g_i2c_master.rx_data++;
        g_i2c_master.rx_length--;
        
    }
    (void)i2c_ptr->IC_CLR_INTR; // 清除全部可清除标志位
#if 0
    I2C_TypeDef *i2c_ptr = i2c_addr_get((uint32_t)1);
		if (i2c_ptr->IC_CON.MASTER_MODE == 1)
		{
			i2c_master_int_handle((uint32_t)1);
		}
		else
		{
			i2c_slave_int_handle((uint32_t)1);
		}
#endif
}

#endif

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

