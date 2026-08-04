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
	/* clock_mco_div 枚举值(0~8)与 MCOPRE 寄存器编码一致，直接透传 */
	rcc_drv_mco_div(rcc_ptr, (uint8_t)div);
}

/**
 *@brief Get the current MCO clock source
 *@param[in] none
 *@return    clock_mco_select: current MCO clock source (SYS/HSI/PLLCLK/HSE/LSI)
 */
clock_mco_select rcc_hal_get_mco_clock_status(void)
{
	return rcc_drv_get_mco_clock_status(rcc_ptr);
}

/**
 *@brief Get the current MCO division ratio
 *@param[in] none
 *@return    Current MCO division ratio (0~8, output freq = source / 2^div)
 */
uint8_t rcc_hal_get_mco_div(void)
{
	return rcc_drv_get_mco_div(rcc_ptr);
}

/**
 *@brief Get the current MCO output frequency
 *@param[in] none
 *@return    MCO output frequency in Hz (source / 2^div)
 */
uint32_t rcc_hal_get_mco_freq(void)
{
	return rcc_drv_get_mco_freq(rcc_ptr);
}

#endif



/**********************************************************
 * rcc oscillator control
 * 振荡器使能：HSI/LSI/HSE/LSE/PLL 的统一使能接口
***********************************************************/
#if 1

/**
 *@brief Enable one clock oscillator and wait until it is ready
 *@param[in] osc: Please enter a value:
                        CLOCK_OSC_HSI,  内部 32MHz RC 振荡器
                        CLOCK_OSC_LSI,  内部 32KHz RC 振荡器
                        CLOCK_OSC_HSE,  外部高速晶振
                        CLOCK_OSC_LSE,  外部低速晶振
                        CLOCK_OSC_PLL,  内部 PLL（需先配置好倍频/分频）
 *@param[in] timeout_us: Wait timeout in microseconds
 *@return    Clock configure result status:
 *                 - ERC_CLK_SYS_SRC_INVALID: osc invalid
 *                 - ERC_CLK_OSC_NOT_READY: oscillator not ready within timeout
 *                 - ERC_CLK_NO_ERROR: success
 */
clk_erc_e rcc_hal_osc_enable(clock_osc_e osc, uint32_t timeout_us)
{
	clk_erc_e erc = ERC_CLK_NO_ERROR;

	if (osc >= CLOCK_OSC_MAX)
	{
		return ERC_CLK_SYS_SRC_INVALID;
	}

	switch (osc)
	{
	case CLOCK_OSC_HSI:
		rcc_drv_hsi_enable(rcc_ptr);
		erc = rcc_drv_hsi_wait_ready(rcc_ptr, timeout_us);
		break;
	case CLOCK_OSC_LSI:
		rcc_drv_lsi_enable(rcc_ptr);
		erc = rcc_drv_lsi_wait_ready(rcc_ptr, timeout_us);
		break;
	case CLOCK_OSC_HSE:
		rcc_drv_hse_enable(rcc_ptr);
		erc = rcc_drv_hse_wait_ready(rcc_ptr, timeout_us);
		break;
	case CLOCK_OSC_LSE:
		rcc_drv_lse_enable(rcc_ptr);
		erc = rcc_drv_lse_wait_ready(rcc_ptr, timeout_us);
		break;
	case CLOCK_OSC_PLL:
		rcc_drv_pll_enable(rcc_ptr);
		erc = rcc_drv_pll_wait_ready(rcc_ptr, timeout_us);
		break;
	default:
		erc = ERC_CLK_SYS_SRC_INVALID;
		break;
	}

	return erc;
}


void rcc_hal_osc_enable_all(void)
{
	uint16_t wait_time = 1000;
	rcc_drv_hsi_enable(rcc_ptr);
	rcc_drv_hsi_wait_ready(rcc_ptr, wait_time);
	rcc_drv_pll_enable(rcc_ptr);
	rcc_drv_pll_wait_ready(rcc_ptr, wait_time);
	rcc_drv_lsi_enable(rcc_ptr);
	rcc_drv_lsi_wait_ready(rcc_ptr, wait_time);
	rcc_drv_hse_enable(rcc_ptr);
	rcc_drv_hse_wait_ready(rcc_ptr, wait_time);
	rcc_drv_lse_enable(rcc_ptr);
	rcc_drv_lse_wait_ready(rcc_ptr, wait_time);
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
	/* clock_sys_fclk_div 枚举值(0~15)与 HPRE 寄存器编码一致，直接透传 */
	rcc_drv_sys_div_fclk(rcc_ptr, (uint8_t)div);
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
	/* clock_sys_pclk_div 枚举值(0~15)与 PPRE 寄存器编码一致，直接透传 */
	rcc_drv_sys_div_pclk(rcc_ptr, (uint8_t)div);
}


/**
 *@brief Switch system clock source and reinit UART accordingly.
 *@param[in] sel: system clock source to switch to
 *@param[in] fdiv: fclk division after the switch
 *@param[in] pdiv: pclk division after the switch
 *@return    none
 */
void rcc_hal_sys_change_clock(sys_clk_config_t *sys_clk_cfg)
{
	delay_1us(10000);		//留点时间(最好是阻塞等待)
	//阻塞等待需要系统时钟的串口完成数据收发时再转换系统时钟源
	rcc_drv_sys_select_clock(rcc_ptr, sys_clk_cfg->sys_clk_sel);
	rcc_hal_sys_div_fclk(sys_clk_cfg->fclk_div);
	rcc_hal_sys_div_pclk(sys_clk_cfg->pclk_div);

	//PLL的倍频和分频也需要在这里修改point

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

/**
 *@brief Get the current FCLK division ratio
 *@param[in] none
 *@return    Current FCLK division ratio (0~15, FCLK = SYSCLK / (value + 1))
 */
uint8_t rcc_hal_get_fclk_div(void)
{
	return rcc_drv_get_fclk_div(rcc_ptr);
}

/**
 *@brief Get the current PCLK division ratio
 *@param[in] none
 *@return    Current PCLK division ratio (0~15, PCLK = FCLK / (value + 1))
 */
uint8_t rcc_hal_get_pclk_div(void)
{
	return rcc_drv_get_pclk_div(rcc_ptr);
}

/**
 *@brief Get the current system clock frequency (SYSCLK)
 *@param[in] none
 *@return    System clock frequency in Hz
 *                 - HSI source: HIRC_FREQ
 *                 - HSE source: OSC_CURRENT_FREQ
 *                 - PLL source: PLL_FREQ
 */
uint32_t rcc_hal_get_sysclk_freq(void)
{
	return rcc_drv_get_sysclk_freq(rcc_ptr);
}

/**
 *@brief Get the current FCLK frequency
 *@param[in] none
 *@return    FCLK frequency in Hz (FCLK = SYSCLK / (HPRE + 1))
 */
uint32_t rcc_hal_get_fclk_freq(void)
{
	return rcc_drv_get_fclk_freq(rcc_ptr);
}

/**
 *@brief Get the current PCLK frequency
 *@param[in] none
 *@return    PCLK frequency in Hz (PCLK = FCLK / (PPRE + 1))
 */
uint32_t rcc_hal_get_pclk_freq(void)
{
	return rcc_drv_get_pclk_freq(rcc_ptr);
}

/**
 *@brief One-stop system clock configuration: clock source + FCLK/PCLK dividers
 *@param[in] sel:      System clock source (CLOCK_SYS_HSI/HSE/PLLCLK1/PLLCLK2)
 *@param[in] fclk_div: FCLK divider, CLOCK_SYS_FCLK_DIV1~DIV16
 *@param[in] pclk_div: PCLK divider, CLOCK_SYS_PCLK_DIV1~DIV16
 *@return    Clock configure result status:
 *                 - ERC_CLK_SYS_SRC_INVALID
 *                 - ERC_CLK_DIV_INVALID
 *                 - ERC_CLK_NO_ERROR
 */
clk_erc_e rcc_hal_sys_clock_config(clock_sys_select sel, clock_sys_fclk_div fclk_div, clock_sys_pclk_div pclk_div)
{
	return rcc_drv_sys_clock_config(rcc_ptr, sel, (uint8_t)fclk_div, (uint8_t)pclk_div);
}

/**
 *@brief Switch system clock source safely (verify target ready before switching)
 *@param[in] sel: Target system clock source (CLOCK_SYS_HSI/HSE/PLLCLK1/PLLCLK2)
 *@return    Clock configure result status:
 *                 - ERC_CLK_SYS_SRC_INVALID
 *                 - ERC_CLK_OSC_NOT_READY: target oscillator not ready
 *                 - ERC_CLK_NO_ERROR
 */
clk_erc_e rcc_hal_sys_switch_clock(clock_sys_select sel)
{
	return rcc_drv_sys_switch_clock(rcc_ptr, sel);
}

/**********************************************************
 * rcc PLL control
***********************************************************/

/**
 *@brief Enable/Disable the PLL
 *@param[in] en: 1 = PLL 使能，0 = PLL 关闭
 *@return    none
 */
void rcc_hal_pll_enable(void)
{
	rcc_drv_pll_enable(rcc_ptr);
}

void rcc_hal_pll_disable(void)
{
	rcc_drv_pll_disable(rcc_ptr);
}

/**
 *@brief Configure the PLL multiplier and output divider
 *@param[in] mult: PLLMULT, 5-bit multiplier, valid range 0x00~0x1F
 *@param[in] div:  PLLDIV, 3-bit output divider, valid range 0x00~0x07
 *@return    Clock configure result status:
 *                 - ERC_CLK_DIV_INVALID
 *                 - ERC_CLK_NO_ERROR
 */
clk_erc_e rcc_hal_pll_config(uint8_t mult, uint8_t div)
{
	return rcc_drv_pll_config(rcc_ptr, mult, div);
}

/**
 *@brief Get the PLL lock ready flag
 *@param[in] none
 *@return    RO_PLL_RDY: 1 = PLL 已锁定，0 = 未锁定
 */
uint8_t rcc_hal_pll_ready(void)
{
	return rcc_drv_pll_ready(rcc_ptr);
}

/**
 *@brief Get the PLL error latch flag
 *@param[in] none
 *@return    RO_PLL_ERR_LAT: 1 = PLL 出现错误被锁存，0 = 无错误
 */
uint8_t rcc_hal_pll_lock_error(void)
{
	return rcc_drv_pll_lock_error(rcc_ptr);
}

/**
 *@brief Wait for the PLL to lock, with timeout
 *@param[in] timeout_us: Wait timeout in microseconds
 *@return    Clock configure result status:
 *                 - ERC_CLK_OSC_NOT_READY: timeout, PLL not locked
 *                 - ERC_CLK_NO_ERROR: success
 */
clk_erc_e rcc_hal_pll_wait_ready(uint32_t timeout_us)
{
	return rcc_drv_pll_wait_ready(rcc_ptr, timeout_us);
}

/**
 *@brief Get the PLL output frequency
 *@param[in] none
 *@return    PLL output frequency in Hz
 */
uint32_t rcc_hal_pll_freq(void)
{
	return rcc_drv_pll_freq(rcc_ptr);
}

/**
 *@brief One-stop system clock configuration: PLL + source + FCLK/PCLK dividers
 *@param[in] sel:       System clock source (CLOCK_SYS_HSI/HSE/PLLCLK1/PLLCLK2)
 *@param[in] pll_mult:  PLL multiplier (PLLMULT), ignored if target is not PLL
 *@param[in] pll_div:   PLL output divider (PLLDIV), ignored if target is not PLL
 *@param[in] fclk_div:  FCLK divider, CLOCK_SYS_FCLK_DIV1~DIV16
 *@param[in] pclk_div:  PCLK divider, CLOCK_SYS_PCLK_DIV1~DIV16
 *@return    Clock configure result status:
 *                 - ERC_CLK_SYS_SRC_INVALID
 *                 - ERC_CLK_DIV_INVALID
 *                 - ERC_CLK_OSC_NOT_READY: PLL not locked within timeout
 *                 - ERC_CLK_NO_ERROR
 */
clk_erc_e rcc_hal_sys_clock_full_config(clock_sys_select sel, uint8_t pll_mult, uint8_t pll_div, clock_sys_fclk_div fclk_div, clock_sys_pclk_div pclk_div)
{
	return rcc_drv_sys_clock_full_config(rcc_ptr, sel, pll_mult, pll_div, (uint8_t)fclk_div, (uint8_t)pclk_div);
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
		//PLL可以倍频和分频point
		*freq = PLL_FREQ;
		*freq = *freq / fclk_num;
		*freq = *freq / pclk_num;
	} else if(rcc_drv_sys_clock_status(rcc_ptr) == CLOCK_SYS_HSE)
	{
		*freq = OSC_CURRENT_FREQ;
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











