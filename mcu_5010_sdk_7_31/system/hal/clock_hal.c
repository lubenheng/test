#include "platform_common.h"

#include "clock_reg.h"
#include "device_resource.h"
#include "clock_type.h"
#include "clock.h"
#include "clock_hal.h"


#include "clock_cmdline.h"
#include "uart_hal.h"
#include "uart_hal_cfg.h"


/**********************************************************
 * rcc peripheral base pointer
***********************************************************/
static RCC_TypeDef *rcc_ptr = (RCC_TypeDef *)RCC_BASE;
/**********************************************************
 * rcc mco control
***********************************************************/
#if 1
/**
 *@brief Select the MCO output clock source
 *@param[in] sel: clock_mco_select, MCO clock source (SYS/HSI/PLLCLK/HSE/LSI)
 *@return    none
 */
void rcc_hal_mco_clock_select(clock_mco_select sel)
{
	rcc_drv_mco_clock_select(rcc_ptr, sel);
}

/**
 *@brief MCO clock division
 *@param[in] div: Please enter a value:
						CLOCK_MCO_DIV1,
						CLOCK_MCO_DIV2,
						CLOCK_MCO_DIV4,
						CLOCK_MCO_DIV8,
						CLOCK_MCO_DIV16,
						CLOCK_MCO_DIV32,
						CLOCK_MCO_DIV64,
						CLOCK_MCO_DIV128,
						CLOCK_MCO_DIV256
 *@return    none
 */
void rcc_hal_mco_div(clock_mco_div div)
{
	switch (div)
	{
	
	case CLOCK_MCO_DIV1:
		rcc_drv_mco_div(rcc_ptr, 0);
		break;
	case CLOCK_MCO_DIV2:
		rcc_drv_mco_div(rcc_ptr, 1);
		break;
	case CLOCK_MCO_DIV4:
		rcc_drv_mco_div(rcc_ptr, 2);
		break;
	case CLOCK_MCO_DIV8:
		rcc_drv_mco_div(rcc_ptr, 3);
		break;
	case CLOCK_MCO_DIV16:
		rcc_drv_mco_div(rcc_ptr, 4);
		break;
	case CLOCK_MCO_DIV32:
		rcc_drv_mco_div(rcc_ptr, 5);
		break;
	case CLOCK_MCO_DIV64:
		rcc_drv_mco_div(rcc_ptr, 6);
		break;
	case CLOCK_MCO_DIV128:
		rcc_drv_mco_div(rcc_ptr, 7);
		break;
	case CLOCK_MCO_DIV256:
		rcc_drv_mco_div(rcc_ptr, 8);
		break;
	default:
		break;
	}
}

#endif



/**********************************************************
 * rcc system clock control
***********************************************************/
#if 1
/**
 *@brief Internal system clock selection
 *@param[in] sel:Clock_HSI  Clock_HSE   Clock_PLLCLK1/2
 *@return none
 */
void rcc_hal_sys_select_clock(clock_sys_select sel)
{
	rcc_drv_sys_select_clock(rcc_ptr, sel);
}

/**
 *@brief System Clock FCLK division
 *@param[in] div: Please enter a value:
						CLOCK_SYS_FCLK_DIV1,
						CLOCK_SYS_FCLK_DIV2,
						CLOCK_SYS_FCLK_DIV3,
						CLOCK_SYS_FCLK_DIV4,
						CLOCK_SYS_FCLK_DIV5,
						CLOCK_SYS_FCLK_DIV6,
						CLOCK_SYS_FCLK_DIV7,
						CLOCK_SYS_FCLK_DIV8,
						CLOCK_SYS_FCLK_DIV9,
						CLOCK_SYS_FCLK_DIV10,
						CLOCK_SYS_FCLK_DIV11,
						CLOCK_SYS_FCLK_DIV12,
						CLOCK_SYS_FCLK_DIV13,
						CLOCK_SYS_FCLK_DIV14,
						CLOCK_SYS_FCLK_DIV15,
						CLOCK_SYS_FCLK_DIV16,
 *@return    none
 */
void rcc_hal_sys_div_fclk(clock_sys_fclk_div div)
{
	switch (div)
	{
	case CLOCK_SYS_FCLK_DIV1:
		rcc_drv_sys_div_fclk(rcc_ptr, 0);
		break;
	case CLOCK_SYS_FCLK_DIV2:
		rcc_drv_sys_div_fclk(rcc_ptr, 1);
		break;
	case CLOCK_SYS_FCLK_DIV3:
		rcc_drv_sys_div_fclk(rcc_ptr, 2);
		break;
	case CLOCK_SYS_FCLK_DIV4:
		rcc_drv_sys_div_fclk(rcc_ptr, 3);
		break;
	case CLOCK_SYS_FCLK_DIV5:
		rcc_drv_sys_div_fclk(rcc_ptr, 4);
		break;
	case CLOCK_SYS_FCLK_DIV6:
		rcc_drv_sys_div_fclk(rcc_ptr, 5);
		break;
	case CLOCK_SYS_FCLK_DIV7:
		rcc_drv_sys_div_fclk(rcc_ptr, 6);
		break;
	case CLOCK_SYS_FCLK_DIV8:
		rcc_drv_sys_div_fclk(rcc_ptr, 7);
		break;
	case CLOCK_SYS_FCLK_DIV9:
		rcc_drv_sys_div_fclk(rcc_ptr, 8);
		break;
	case CLOCK_SYS_FCLK_DIV10:
		rcc_drv_sys_div_fclk(rcc_ptr, 9);
		break;
	case CLOCK_SYS_FCLK_DIV11:
		rcc_drv_sys_div_fclk(rcc_ptr, 10);
		break;
	case CLOCK_SYS_FCLK_DIV12:
		rcc_drv_sys_div_fclk(rcc_ptr, 11);
		break;
	case CLOCK_SYS_FCLK_DIV13:
		rcc_drv_sys_div_fclk(rcc_ptr, 12);
		break;
	case CLOCK_SYS_FCLK_DIV14:
		rcc_drv_sys_div_fclk(rcc_ptr, 13);
		break;
	case CLOCK_SYS_FCLK_DIV15:
		rcc_drv_sys_div_fclk(rcc_ptr, 14);
		break;
	case CLOCK_SYS_FCLK_DIV16:
		rcc_drv_sys_div_fclk(rcc_ptr, 15);
		break;
	default:
		break;
	}
}

/**
 *@brief System Clock PCLK division
 *@param[in] div: Please enter a value:
						CLOCK_SYS_PCLK_DIV1,
						CLOCK_SYS_PCLK_DIV2,
						CLOCK_SYS_PCLK_DIV3,
						CLOCK_SYS_PCLK_DIV4,
						CLOCK_SYS_PCLK_DIV5,
						CLOCK_SYS_PCLK_DIV6,
						CLOCK_SYS_PCLK_DIV7,
						CLOCK_SYS_PCLK_DIV8,
						CLOCK_SYS_PCLK_DIV9,
						CLOCK_SYS_PCLK_DIV10,
						CLOCK_SYS_PCLK_DIV11,
						CLOCK_SYS_PCLK_DIV12,
						CLOCK_SYS_PCLK_DIV13,
						CLOCK_SYS_PCLK_DIV14,
						CLOCK_SYS_PCLK_DIV15,
						CLOCK_SYS_PCLK_DIV16
 *@return    none
 */
void rcc_hal_sys_div_pclk(clock_sys_pclk_div div)
{
	switch (div)
	{
	case CLOCK_SYS_PCLK_DIV1:
		rcc_drv_sys_div_pclk(rcc_ptr, 0);
		break;
	case CLOCK_SYS_PCLK_DIV2:
		rcc_drv_sys_div_pclk(rcc_ptr, 1);
		break;
	case CLOCK_SYS_PCLK_DIV3:
		rcc_drv_sys_div_pclk(rcc_ptr, 2);
		break;
	case CLOCK_SYS_PCLK_DIV4:
		rcc_drv_sys_div_pclk(rcc_ptr, 3);
		break;
	case CLOCK_SYS_PCLK_DIV5:
		rcc_drv_sys_div_pclk(rcc_ptr, 4);
		break;
	case CLOCK_SYS_PCLK_DIV6:
		rcc_drv_sys_div_pclk(rcc_ptr, 5);
		break;
	case CLOCK_SYS_PCLK_DIV7:
		rcc_drv_sys_div_pclk(rcc_ptr, 6);
		break;
	case CLOCK_SYS_PCLK_DIV8:
		rcc_drv_sys_div_pclk(rcc_ptr, 7);
		break;
	case CLOCK_SYS_PCLK_DIV9:
		rcc_drv_sys_div_pclk(rcc_ptr, 8);
		break;
	case CLOCK_SYS_PCLK_DIV10:
		rcc_drv_sys_div_pclk(rcc_ptr, 9);
		break;
	case CLOCK_SYS_PCLK_DIV11:
		rcc_drv_sys_div_pclk(rcc_ptr, 10);
		break;
	case CLOCK_SYS_PCLK_DIV12:
		rcc_drv_sys_div_pclk(rcc_ptr, 11);
		break;
	case CLOCK_SYS_PCLK_DIV13:
		rcc_drv_sys_div_pclk(rcc_ptr, 12);
		break;
	case CLOCK_SYS_PCLK_DIV14:
		rcc_drv_sys_div_pclk(rcc_ptr, 13);
		break;
	case CLOCK_SYS_PCLK_DIV15:
		rcc_drv_sys_div_pclk(rcc_ptr, 14);
		break;
	case CLOCK_SYS_PCLK_DIV16:
		rcc_drv_sys_div_pclk(rcc_ptr, 15);
		break;
	default:
		break;
	}
}


/**
 *@brief Switch system clock source and reinit UART accordingly.
 *@param[in] sel: system clock source to switch to
 *@param[in] fdiv: fclk division after the switch
 *@param[in] pdiv: pclk division after the switch
 *@return    none
 */
void rcc_hal_sys_change_clock(clock_sys_select sel, clock_sys_fclk_div fdiv, clock_sys_pclk_div pdiv)
{
	delay_1us(10000);		//留点时间(最好是阻塞等待)
	//阻塞等待需要系统时钟的串口完成数据收发时再转换系统时钟源
	rcc_drv_sys_select_clock(rcc_ptr, sel);
	rcc_hal_sys_div_fclk(fdiv);
	rcc_hal_sys_div_pclk(pdiv);

	//disable uart before clock source changes
	uart_transmit_enable(UART_CONSOLE_INSTANCE, false);
	uart_receive_enable(UART_CONSOLE_INSTANCE, false);
	uart_hal_status_rxbusy_set(UART_CONSOLE_INSTANCE, false);

	//reset uart and reinit with new clock
	rcc_drv_rst_uart_0(rcc_ptr);
	(void)uart_init(UART_CONSOLE_INSTANCE, &uart0_init_config);

	//re-enable uart
	uart_transmit_enable(UART_CONSOLE_INSTANCE, true);
	uart_receive_enable(UART_CONSOLE_INSTANCE, true);
	uart_hal_status_rxbusy_set(UART_CONSOLE_INSTANCE, true);
}

/**
 *@brief Get the current system clock source
 *@param[in] none
 *@return    clock_sys_select: current system clock source
 */
clock_sys_select rcc_hal_sys_clock_status(void)
{
	return rcc_drv_sys_clock_status(rcc_ptr);
}




#endif

/**
 *@brief	  get scg and pcg frequency
 *@param[in]  module_name: including scg and peripheral module
 *@param[out] *freq: pointer, points to acquired frequency
 *@return	  clock configure result status:
 *                 - ERC_CLK_MODULE_INVALID
 *                 - ERC_CLK_NO_ERROR
 */
clk_erc_e clock_freq_get(module_name_e module_name, uint32_t *freq)
{
	//clk_erc_e clk_erc = clock_drv_freq_get(module_name, freq); 
	*freq = SYS_CORE_CLK;
	
	uint8_t fclk_num = ((rcc_ptr->SYSCKCR.w >> 24) & 0x0f) + 1;
	uint8_t pclk_num = ((rcc_ptr->SYSCKCR.w >> 16) & 0x0f) + 1;

	if(rcc_drv_sys_clock_status(rcc_ptr) == CLOCK_SYS_HSI)
	{
		*freq = HIRC_FREQ;
		*freq = *freq / fclk_num;
		*freq = *freq / pclk_num;
	}
	else if(rcc_drv_sys_clock_status(rcc_ptr) == CLOCK_SYS_PLLCLK1)
	{
		*freq = PLL_FREQ;
		*freq = *freq / fclk_num;
		*freq = *freq / pclk_num;
	}
	
	return ERC_CLK_NO_ERROR;
}


/**
 *@brief	 configure peripheral module clock
 *@param[in] *module_clk_cfg: pointer, points to peripheral module clock configure parameters
 *@return    clock configure result status:
 *                 - ERC_CLK_MODULE_INVALID,
 *                 - ERC_CLK_DIV_INVALID,
 *                 - ERC_CLK_NO_ERROR
 */
clk_erc_e clock_module_clk_cfg(const module_clk_cfg_t *module_clk_cfg)
{
//	 clk_erc_e clk_erc = clock_drv_module_clk_cfg(module_clk_cfg); 
//	 return clk_erc;
	
	return (clk_erc_e)0;
}

/**
 *@brief 	 configure flash clock
 *@param[in] none
 *@return    none
 */
void flash_clk_enable(void)
{
	
	REG32(0x40020058) = 1U;   // FLC Clock Enable
	
}


/**
 *@brief 	 configure gpio clock
 *@param[in] none
 *@return    none
 */
void gpio_clk_enable(void)
{
	
	REG32(0x40020058)  |= (BIT16 + BIT17 + BIT18); //[16]-> GPI0A_CLOCK_ENABLE：[17]]-> GPIOB_CLOCK_ENABLE：[18]-> GPIOC_CLOCK_ENABLE
	
}


void ADC_Clk_Enable(void)
{
	REG32(0x4002005C) |= BIT0; //ADC0 CLK EN
}
void ADC_Clk_Disable(void)
{
	REG32(0x4002005C) &= ~(0x01<<0); //ADC0 CLK EN
}

/**
 *@brief 	 configure wdog clock
 *@param[in] none
 *@return    none
 */
void wdog_clk_enable(void)
{
	REG32(0x40020060)  |= BIT16; //WDOG CLK EN

}

/**
 *@brief 	 configure qep clock
 *@param[in] none
 *@return    none
 */
void qep_clk_enable(void)
{
	//REG32(0x40020060) |= BIT1;  
	REG32(0x40020060) |= BIT2;

}

/**
 *@brief 	 configure hall clock
 *@param[in] none
 *@return    none
 */
void hall_clk_enable(void)
{
	//REG32(0x40020060) |= BIT1; 
	REG32(0x40020060) |= BIT1; 

}

void HWMATH_Clk_Enable(void)
{
	REG32(0x40020060) |= 0x01<<0; //HWMATH CLK EN
}
void HWMATH_Clk_Disable(void)
{
	REG32(0x40020060) &= ~(0x01<<0); //HWMATH CLK EN
}


/**
 *@brief 	 configure i2c clock
 *@param[in] none
 *@return    none
 */
void i2c_clk_enable(void)
{
	REG32(0x40020064) |= (BIT24 + BIT25); 
}


/**
 *@brief 	 configure spi clock
 *@param[in] none
 *@return    none
 */
void spi_clk_enable(void)
{
	REG32(0x40020064) |= (BIT16 + BIT17); 
}

/**
 *@brief 	 configure uart clock
 *@param[in] none
 *@return    none
 */
void uart_clk_enable(void)
{
	REG32(0x40020064) |= (BIT8 + BIT9); //LPUART0/1 CLK EN
}

/**
 *@brief 	 configure can clock
 *@param[in] none
 *@return    none
 */
void can_clk_enable(void)
{

	REG32(0x40020064) |= BIT0; 
}

/**
 *@brief 	 configure mcpwm clock
 *@param[in] none
 *@return    none
 */
void mcpwm_clk_enable(void)
{
	
	REG32(0x40020068) |= BIT24; //MCPWM CLK EN
	
}

/**
 *@brief 	 configure tim13 clock
 *@param[in] none
 *@return    none
 */
void TIM13_Clk_Enable(void)
{
	REG32(0x40020068) |= BIT13; //TIM13 CLK EN
}

void TIM13_Clk_Disable(void)
{
	REG32(0x40020068) &= ~(0x01<<13); //TIM13 CLK EN
}


/**
 *@brief 	 configure tim14 clock
 *@param[in] none
 *@return    none
 */
void TIM14_Clk_Enable(void)
{
	REG32(0x40020068) |= 0x01<<14; //TIM14 CLK EN
}

void TIM14_Clk_Disable(void)
{
	REG32(0x40020068) &= ~(0x01<<14); //TIM14 CLK EN
}

/**
 *@brief 	 configure tim15 clock
 *@param[in] none
 *@return    none
 */
void TIM15_Clk_Enable(void)
{
	REG32(0x40020068) |= 0x01<<15; //TIM15 CLK EN
}

void TIM15_Clk_Disable(void)
{
	REG32(0x40020068) &= ~(0x01<<15); //TIM15 CLK EN
}

/**
 *@brief 	 configure tim16 clock
 *@param[in] none
 *@return    none
 */
void TIM16_Clk_Enable(void)
{
	REG32(0x40020068) |= 0x01<<16; //TIM16 CLK EN
}

void TIM16_Clk_Disable(void)
{
	REG32(0x40020068) &= ~(0x01<<16); //TIM16 CLK EN
}











