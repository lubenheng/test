#include "platform_common.h"

#include "uart_reg.h"
#include "device_resource.h"
#include "uart_hal.h"
#include "uart.h"



/*******************************************************************************
 * Variables
 ******************************************************************************/ 
/** Tables to save UART interrupt handle. */
static UART_INT_HANDLE g_uart_interrupt_handle[UART_INSTANCE_MAX_COUNT] = {NULL};

/** Table of base addresses for UART instances. */
static UART_TypeDef * const g_uart_bases[UART_INSTANCE_MAX_COUNT] = UART_BASE_ADDRS;


/*******************************************************************************
* Public UART Control Functions
******************************************************************************/
/**
 *@brief set the number of bits per character in the uart controller and the priority rank is 10bits>7bits>8/9bits.
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] bit_number_per_char: number of bits per char
 *@param[in] parity_en: enable or disable parity bit
 *@return    none
 */
void uart_drv_bits_number_perchar_set(UART_TypeDef * uart_ptr, uart_bit_number_per_char_e bit_number_per_char, bool parity_en)
{
    uint32_t temp_bit_count_per_char = (uint32_t)bit_number_per_char;
    if (true == parity_en)
    {
        temp_bit_count_per_char += 1U;
    }

	// if (temp_bit_count_per_char == (uint32_t)UART_7_BITS_PER_CHAR)
	// {
	// 	uart_ptr->CTRL.w = (uart_ptr->CTRL.w & ~UART_CTRL_B7_MASK) | ((uint32_t)1U << UART_CTRL_B7_POS);
	// }
    // else if (temp_bit_count_per_char == (uint32_t)UART_10_BITS_PER_CHAR)
    // {
    //     uart_ptr->BAUD.w = (uart_ptr->BAUD.w & ~UART_BAUD_B10_MASK) | ((uint32_t)1U << UART_BAUD_B10_POS);
    // }
    // else
    // {
    //     /* config 8-bit (M=0) or 9-bits (M=1) */
    //     uart_ptr->CTRL.w = (uart_ptr->CTRL.w & ~UART_CTRL_B9_MASK) | ((temp_bit_count_per_char - 1U) << UART_CTRL_B9_POS);
    //     /* clear M10 to make sure not 10-bit mode */
    //     uart_ptr->BAUD.w &= ~UART_BAUD_B10_MASK;
    // }

    
    if (temp_bit_count_per_char == (uint32_t)UART_8_BITS_PER_CHAR) // M10:M9=MODE�?00  8+0
	{
		//uart_ptr->CTRL.w = (uart_ptr->CTRL.w & ~UART_CTRL_B7_MASK) | ((uint32_t)1U << UART_CTRL_B7_POS);
        uart_ptr->CR_STR.w = (uart_ptr->CR_STR.w & ~UART_CTRL_M10_MASK) | ((uint32_t)0U << UART_CTRL_M10_POS);
        uart_ptr->CR_STR.w = (uart_ptr->CR_STR.w & ~UART_CTRL_M9_MASK)  | ((uint32_t)0U << UART_CTRL_M9_POS);
	}
    else if (temp_bit_count_per_char == (uint32_t)UART_9_BITS_PER_CHAR) // M10:M9=MODE�?01  9 or 8+1
	{
		//uart_ptr->CTRL.w = (uart_ptr->CTRL.w & ~UART_CTRL_B7_MASK) | ((uint32_t)1U << UART_CTRL_B7_POS);
        uart_ptr->CR_STR.w = (uart_ptr->CR_STR.w & ~UART_CTRL_M10_MASK) | ((uint32_t)0U << UART_CTRL_M10_POS);
        uart_ptr->CR_STR.w = (uart_ptr->CR_STR.w & ~UART_CTRL_M9_MASK)  | ((uint32_t)1U << UART_CTRL_M9_POS);
	}
    else    // M10:M9=MODE�?10/11
    {
        uart_ptr->CR_STR.w = (uart_ptr->CR_STR.w & ~UART_CTRL_M10_MASK) | ((uint32_t)1U << UART_CTRL_M10_POS);
        uart_ptr->CR_STR.w = (uart_ptr->CR_STR.w & ~UART_CTRL_M9_MASK)  | ((uint32_t)0U << UART_CTRL_M9_POS);
    }

}

/**
 *@brief set parity mode in the uart controller
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] parity_mode_type: parity mode
 *@return    none
 */
void uart_drv_parity_mode_set(UART_TypeDef *uart_ptr, uart_parity_mode_e parity_mode_type)
{
    // uart_ptr->CTRL.w = (uart_ptr->CTRL.w & ~UART_CTRL_PAR_EN_MASK) | (((uint32_t)parity_mode_type >> 1U) << UART_CTRL_PAR_EN_POS);
    // uart_ptr->CTRL.w = (uart_ptr->CTRL.w & ~UART_CTRL_PAR_ODD_MASK) | (((uint32_t)parity_mode_type & 1U) << UART_CTRL_PAR_ODD_POS);
    uart_ptr->CR_STR.w = (uart_ptr->CR_STR.w & ~UART_CTRL_PAR_EN_MASK) | (((uint32_t)parity_mode_type >> 1U) << UART_CTRL_PAR_EN_POS);
    uart_ptr->CR_STR.w = (uart_ptr->CR_STR.w & ~UART_CTRL_PAR_ODD_MASK) | (((uint32_t)parity_mode_type & 1U) << UART_CTRL_PAR_ODD_POS);
}

/**
 *@brief get an 8-bit character 
 *@param[in]  *uart_ptr: point to the base address of uart instance
 *@param[out] *read_data: point to the data read from receiver
 *@return     none 
 */
void uart_drv_get_char(const UART_TypeDef *uart_ptr, uint8_t *read_data)
{
    DEV_ASSERT(read_data != NULL);

	if (uart_ptr != NULL)
	{
  	  //*read_data = (uint8_t)uart_ptr->DATA.DATA;
        *read_data = (uart_ptr->DR_STR.DATA&0xFF);
	}
}

/**
 *@brief get a 9-bit or 10-bit character. 
 *@param[in]  *uart_ptr: point to the base address of uart instance
 *@param[out] *read_data: point to the data read from receiver
 *@return     none 
 */
void uart_drv_get_char9_10(const UART_TypeDef *uart_ptr, uint16_t *read_data)
{
	 DEV_ASSERT(read_data != NULL);

	 if (uart_ptr != NULL)
	 {
  	   //*read_data = (uint16_t)uart_ptr->DATA.w;
		  *read_data = (uint16_t)uart_ptr->DR_STR.DATA;
	 }
}

/**
 *@brief enable or disable various interrupt sources of uart module
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] int_src: interrupt sources
 *@param[in] enable: enable interrupt sources or not
 *@return    none 
 */
void uart_drv_interrupt_source_enable(UART_TypeDef *uart_ptr, uart_interrupt_e int_src, bool enable)
{
    // uint32_t reg = (uint32_t)(int_src) >> UART_SHIFT;
    // uint32_t int_reg_offset = (uint16_t)(int_src);

    // switch (reg)
    // {
    //     case UART_BAUD_REG_ID:
    //         uart_ptr->BAUD.w = (uart_ptr->BAUD.w & ~(1UL << int_reg_offset)) | ((enable ? 1U : 0UL) << int_reg_offset);
    //         break;
    //     case UART_CTRL_REG_ID:
    //         uart_ptr->CTRL.w = (uart_ptr->CTRL.w & ~(1UL << int_reg_offset)) | ((enable ? 1U : 0UL) << int_reg_offset);
    //         break;
    //     case UART_FLOWCFG_REG_ID:
    //         uart_ptr->FLOWCFG.w = (uart_ptr->FLOWCFG.w & ~(1UL << int_reg_offset)) | ((enable ? 1UL : 0UL) << int_reg_offset);
    //         break;
    //     case UART_FIFO_REG_ID:
    //         uart_ptr->FIFO.w = (uart_ptr->FIFO.w & (~FEATURE_UART_FIFO_REG_FLAGS_MASK & ~(1UL << int_reg_offset))) \
	// 							|((enable ? 1UL : 0UL) << int_reg_offset);
    //         break;
	// 	case UART_AUTOBAUD_REG_ID:
	// 		uart_ptr->AUTOBAUD.w = (uart_ptr->AUTOBAUD.w & (~FEATURE_UART_AUTOBAUD_REG_FLAGS_MASK & ~(1UL << int_reg_offset))) \
	// 							|((enable ? 1UL : 0UL) << int_reg_offset);
	// 		break;
    //     default :
    //         /* Invalid parameter: return */
    //         break;
    // }
}

/**
 *@brief return whether a certain uart interrupt source is enabled or disabled
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] int_src: interrupt sources
 *@return    none
 */
bool uart_drv_interrupt_source_status_get(const UART_TypeDef *uart_ptr, uart_interrupt_e int_src)
{
    // uint32_t reg = (uint32_t)(int_src) >> UART_SHIFT;
    // bool ret_val = false;

    // switch (reg)
    // {
    //     case UART_BAUD_REG_ID:
    //         ret_val = (((uart_ptr->BAUD.w >> (uint16_t)(int_src)) & 1U) > 0U);
    //         break;
    //     case UART_CTRL_REG_ID:
    //         ret_val = (((uart_ptr->CTRL.w >> (uint16_t)(int_src)) & 1U) > 0U);
    //         break;
    //     case UART_FLOWCFG_REG_ID:
    //         ret_val = (((uart_ptr->FLOWCFG.w >> (uint16_t)(int_src)) & 1U) > 0U);
    //         break;
    //     case UART_FIFO_REG_ID:
    //         ret_val = (((uart_ptr->FIFO.w >> (uint16_t)(int_src)) & 1U) > 0U);
    //         break;
	// 	case UART_AUTOBAUD_REG_ID:
	// 		ret_val = (((uart_ptr->AUTOBAUD.w >> (uint16_t)(int_src)) & 1U) > 0U);
	// 		break;
    //     default :
    //         /* Invalid parameter: return */
    //         break;
    // }

    // return ret_val;
	return 0; //delete later
}

/**
 *@brief get the state of a status flag 
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] status_flag: the status flag to query
 *@return    the state of a status flag 
 */
bool uart_drv_status_flag_get(const UART_TypeDef *uart_ptr, uart_status_flag_e status_flag)
{
    // uint32_t reg = (uint32_t)(status_flag) >> UART_SHIFT;
    // bool ret_val = false;

    // switch (reg)
    // {
    //     case UART_STAT_REG_ID:
    //         ret_val = (((uart_ptr->STAT.w >> (uint16_t)(status_flag)) & 1U) > 0U);
    //         break;
    //     case UART_DATA_REG_ID:
    //         ret_val = (((uart_ptr->DATA.w >> (uint16_t)(status_flag)) & 1U) > 0U);
    //         break;
    //     case UART_FLOWCFG_REG_ID:
    //         ret_val = (((uart_ptr->FLOWCFG.w >> (uint16_t)(status_flag)) & 1U) > 0U);
    //         break;
    //     case UART_FIFO_REG_ID:
    //         ret_val = (((uart_ptr->FIFO.w >> (uint16_t)(status_flag)) & 1U) > 0U);
    //         break;
	// 	case UART_AUTOBAUD_REG_ID:
	// 		ret_val = (((uart_ptr->AUTOBAUD.w >> (uint16_t)(status_flag)) & 1U) > 0U);
	// 		break;
    //     default:
    //         /* Invalid parameter: return */
    //         break;
    // }

    // return ret_val;
	return 0; //delete later
}

/**
 *@brief clear an individual status flag 
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] status_flag: the status flag to clear
 *@return    whether clear the status flag successfully or not:
 *				- STATUS_SUCCESS, clear the status flag successfully
 *				- STATUS_ERROR, clear the status flag fail
 */
status_t uart_drv_status_flag_clear(UART_TypeDef *uart_ptr, uart_status_flag_e status_flag)
{
    // status_t return_code = STATUS_SUCCESS;

    // switch(status_flag)
    // {
    //     /* These flags are cleared automatically by other uart operations
    //      * and cannot be manually cleared, return error code */
    //     case UART_TX_DATA_REG_EMPTY:
    //     case UART_TX_COMPLETE:
    //     case UART_RX_DATA_REG_FULL:
    //     case UART_RX_ACTIVE:
    //     case UART_NOISE_IN_CURRENT_WORD:
    //     case UART_PARITY_ERR_IN_CURRENT_WORD:
    //         return_code = STATUS_ERROR;
    //         break;

    //     case UART_IDLE_LINE_DETECT:
    //         uart_ptr->STAT.w = (uart_ptr->STAT.w & (~FEATURE_UART_STAT_REG_FLAGS_MASK)) | UART_STAT_IDLE_FLAG_MASK;
    //         break;

    //     case UART_RX_OVERRUN:
    //         uart_ptr->STAT.w = (uart_ptr->STAT.w & (~FEATURE_UART_STAT_REG_FLAGS_MASK)) | UART_STAT_RX_OVERRUN_FLAG_MASK;
    //         break;

    //     case UART_NOISE_DETECT:
    //         uart_ptr->STAT.w = (uart_ptr->STAT.w & (~FEATURE_UART_STAT_REG_FLAGS_MASK)) | UART_STAT_NOISE_FLAG_MASK;
    //         break;

    //     case UART_FRAME_ERR:
    //         uart_ptr->STAT.w = (uart_ptr->STAT.w & (~FEATURE_UART_STAT_REG_FLAGS_MASK)) | UART_STAT_STOP_ERR_FLAG_MASK;
    //         break;

    //     case UART_PARITY_ERR:
    //         uart_ptr->STAT.w = (uart_ptr->STAT.w & (~FEATURE_UART_STAT_REG_FLAGS_MASK)) | UART_STAT_PAR_ERR_FLAG_MASK;
    //         break;

    //     case UART_LIN_BREAK_DETECT:
    //         uart_ptr->STAT.w = (uart_ptr->STAT.w & (~FEATURE_UART_STAT_REG_FLAGS_MASK)) | UART_STAT_BRK_DET_FLAG_MASK;
    //         break;

    //     case UART_RX_ACTIVE_EDGE_DETECT:
    //         uart_ptr->STAT.w = (uart_ptr->STAT.w & (~FEATURE_UART_STAT_REG_FLAGS_MASK)) | UART_STAT_RX_ACT_DEGE_FLAG_MASK;
    //         break;
		
    //     case UART_MATCH_ADDR_ONE:
    //         uart_ptr->STAT.w = (uart_ptr->STAT.w & (~FEATURE_UART_STAT_REG_FLAGS_MASK)) | UART_STAT_MATCH1_FLAG_MASK;
    //         break;
		
    //     case UART_MATCH_ADDR_TWO:
    //         uart_ptr->STAT.w = (uart_ptr->STAT.w & (~FEATURE_UART_STAT_REG_FLAGS_MASK)) | UART_STAT_MATCH2_FLAG_MASK;
    //         break;
		
    //     case UART_FIFO_TX_OF:
    //         uart_ptr->FIFO.w = (uart_ptr->FIFO.w & (~FEATURE_UART_FIFO_REG_FLAGS_MASK)) | UART_FIFO_TX_OVFLOW_FLAG_MASK;
    //         break;
		
    //     case UART_FIFO_RX_UF:
    //         uart_ptr->FIFO.w = (uart_ptr->FIFO.w & (~FEATURE_UART_FIFO_REG_FLAGS_MASK)) | UART_FIFO_RX_UNFLOW_FLAG_MASK;
    //         break;
	// 	case UART_AUTO_SUCC:
	// 		uart_ptr->AUTOBAUD.w = (uart_ptr->AUTOBAUD.w & (~FEATURE_UART_AUTOBAUD_REG_FLAGS_MASK)) | UART_AUTOBAUD_AUTO_SUCC_FLAG_MASK;
	// 		break;
    //     default:
    //         return_code = STATUS_ERROR;
    //         break;
    // }
	
	// __DSB();/*wait clearing flag completed*/

    // return (return_code);
	
	return (status_t)(0); //delete later
}

/**
 *@brief enable or disable the uart error interrupt source
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] enable: enable or disable error interrupt source
 *@return    none
 */
void uart_drv_error_interrupts_source_enable(UART_TypeDef *uart_ptr, bool enable)
{
    /* Configure the error interrupts */
//    uart_drv_interrupt_source_enable(uart_ptr, UART_INT_RX_OVERRUN, enable);
//    uart_drv_interrupt_source_enable(uart_ptr, UART_INT_PARITY_ERR_FLAG, enable);
//    uart_drv_interrupt_source_enable(uart_ptr, UART_INT_NOISE_ERR_FLAG, enable);
//    uart_drv_interrupt_source_enable(uart_ptr, UART_INT_FRAME_ERR_FLAG, enable);
}

/**
 *@brief send data used for INFO FUNCTION
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] ch: the data to send
 *@return    none
 */
void uart_drv_fput_char(UART_TypeDef *uart_ptr, uint8_t ch)
{
//	while (!(uart_drv_status_flag_get(uart_ptr, UART_TX_DATA_REG_EMPTY)))
//     {
//       ;
//     }

	/* SR_STR.TXFULL 为只读寄存器会实时更�? */ 
	/* IR_STR.TXOVF中TX_FIFO满后的标志位，需要在�?���?��1才能清除 */
	while(uart_ptr->SR_STR.TXFULL){}

	//while(uart_ptr->SR_STR.TXEMPT == 1U){}
	
	uart_ptr->DR_STR.DATA = (((uint32_t)ch) & 0x3FFU);
}

/**
 *@brief specify interrupt handler for a uart instance
 *@param[in] instance: uart instance number
 *@param[in] handle: interrupt handler
 *@return    none
 */
void uart_drv_interrupt_handle_cfg(uart_instance_e instance, const UART_INT_HANDLE handle)
{
	 g_uart_interrupt_handle[instance] = handle;
}

 /**
  *@brief the interrupt handler of uart0's tx&rx 
  *@param[in] none
  *@return    none
  */
uint16_t uart0_intp_cnt = 0;
 void UART0_IRQHandler(void)
 {
	uart0_intp_cnt++;
	 
	UART_TypeDef *base = g_uart_bases[UART_INSTANCE0];
	 
	 base->IR_STR.w = 0xFFFFFFFFUL;
	 
// 	if (NULL != g_uart_interrupt_handle[UART_INSTANCE0])
// 	{
// 		g_uart_interrupt_handle[UART_INSTANCE0](UART_INSTANCE0);		
// 	}
 }

 /**
  *@brief the interrupt handler of uart1's tx&rx 
  *@param[in] none
  *@return    none
  */
 uint16_t uart1_intp_cnt = 0;
 void UART1_IRQHandler(void)
 {
	uart1_intp_cnt++;
	 
	UART_TypeDef *base = g_uart_bases[UART_INSTANCE1];
	 
	base->IR_STR.w = 0xFFFFFFFFUL;
	 
// 	if (NULL != g_uart_interrupt_handle[UART_INSTANCE1])
// 	{
// 		g_uart_interrupt_handle[UART_INSTANCE1](UART_INSTANCE1);		
// 	}
 }

