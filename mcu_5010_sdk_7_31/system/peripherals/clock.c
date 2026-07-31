#include "platform_common.h"

#include "clock_reg.h"
#include "clock_type.h"
#include "device_resource.h"
#include "clock.h"


/**********************************************************
 * rcc mco clock control
***********************************************************/
#if 1

/**
 *@brief Select the clock source for MCO
 *@param[in] rcc_ptr: Defined rcc variable
*@param[in] sel: The types of MCO clock sources are as follows:
                MCO_CLOCK_SYS
	            MCO_RESERVED1
	            MCO_CLOCK_HSI
	            MCO_CLOCK_PLLCLK
	            MCO_CLOCK_HSE
	            MCO_CLOCK_LSI  
 *@return none
 */
void rcc_drv_mco_clock_select(RCC_TypeDef *rcc_ptr, clock_mco_select sel)
{
    switch (sel)
    {
    case CLOCK_MCO_SYS:
        rcc_ptr->MCOCR_STR.MCOSRC = 0;
        break;
    case CLOCK_MCO_HSI:
        rcc_ptr->MCOCR_STR.MCOSRC = 2;
        break;
    case CLOCK_MCO_PLLCLK:
        rcc_ptr->MCOCR_STR.MCOSRC = 3;
        break;
    case CLOCK_MCO_HSE:
        rcc_ptr->MCOCR_STR.MCOSRC = 4;
        break;
    case CLOCK_MCO_LSI:
        rcc_ptr->MCOCR_STR.MCOSRC = 5;
        break;
    default:
        break;
    }
}

/**
 *@brief MCO Clock division
 *@param[in] *rcc_ptr: Defined rcc variable
 *@param[in] div: Please enter a value from 0 to 8, 
                  corresponding to a division ratio from 1 to 512.
 *@return    none
 */
void rcc_drv_mco_div(RCC_TypeDef *rcc_ptr, uint8_t div)
{
		//close old div num
		rcc_ptr->MCOCR_STR.MCOPRE &= 0x0;
	
		//enter new div num
        rcc_ptr->MCOCR_STR.MCOPRE |= div;
}
#endif


/**********************************************************
 * rcc system clock control
***********************************************************/
#if 1
/**
 *@brief Internal system clock selection
 *@param[in] *rcc_ptr: Defined rcc variable
 *@param[in] sel: 
            	CLOCK_SYS_HSI    :内部时钟32MHz
	            CLOCK_SYS_HSE    :外部晶振
	            CLOCK_SYS_PLLCLK1：内部PLL时钟
	            CLOCK_SYS_PLLCLK2：内部PLL时钟

 *@return    none
 */
void rcc_drv_sys_select_clock(RCC_TypeDef *rcc_ptr,clock_sys_select sel)
{
    switch (sel)
    {
    case CLOCK_SYS_HSI:
        rcc_ptr->SYSCKCR.SYSCKSW = 0;
        break;
    case CLOCK_SYS_HSE:
        rcc_ptr->SYSCKCR.SYSCKSW = 1;
        break;
    case CLOCK_SYS_PLLCLK1:
    case CLOCK_SYS_PLLCLK2:
        rcc_ptr->SYSCKCR.SYSCKSW = 2;
        break;
    default:
        break;
    }
}

/**
 *@brief Check which source the system clock is using
 *@param[in] rcc_ptr: Defined rcc variable
 *@return    Returns the enumeration type of the clock source, as follows:
                CLOCK_SYS_HSI
                CLOCK_SYS_HSE
                CLOCK_SYS_PLLCLK1
 */
clock_sys_select rcc_drv_sys_clock_status(RCC_TypeDef *rcc_ptr)
{
    if(rcc_ptr->SYSCKCR.SYSCKSW == 0)
	{
        return CLOCK_SYS_HSI;
    } 
	else if(rcc_ptr->SYSCKCR.SYSCKSW == 1)
	{
        return CLOCK_SYS_HSE;
    } 
	else if(rcc_ptr->SYSCKCR.SYSCKSW == 2)
	{
        return CLOCK_SYS_PLLCLK1;
    } 
	else if(rcc_ptr->SYSCKCR.SYSCKSW == 3)
	{
        return CLOCK_SYS_PLLCLK1;
    } 
	else
	{
	    return CLOCK_SYS_RESERD;
	}
}

/**
 *@brief Divide the system clock to get fclk
 *@param[in] *rcc_ptr: Defined rcc variable
 *@param[in] div: 0x0~0xf(0~15)
 *@return    none
 */
void rcc_drv_sys_div_fclk(RCC_TypeDef *rcc_ptr, uint8_t div)
{
    //FCLK = SYSCLK / (HPRE + 1)
    rcc_ptr->SYSCKCR.HPRE = div;
}

/**
 *@brief Divide fclk to get pclk
 *@param[in] *rcc_ptr: Defined rcc variable
 *@param[in] div: 0x0~0xf(0~15)
 *@return    none
 */
void rcc_drv_sys_div_pclk(RCC_TypeDef *rcc_ptr, uint8_t div)
{
    //PCLK = FCLK / (PPRE + 1)
    rcc_ptr->SYSCKCR.PPRE = div;
}

#endif


/**********************************************************
 * hardware clock enable func
***********************************************************/
#if 1
void rcc_drv_enable_flc(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->AHB_CLK_EN.FLC_CLK_EN = 1;
}

void rcc_drv_enable_gpio_a(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->AHB_CLK_EN.GPIOA_CLK_EN = 1;
}

void rcc_drv_enable_gpio_b(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->AHB_CLK_EN.GPIOB_CLK_EN = 1;
}

void rcc_drv_enable_gpio_c(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->AHB_CLK_EN.GPIOC_CLK_EN = 1;
}

void rcc_drv_enable_adc0(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->ANA_CLK_EN.ADC0_CLK_EN = 1;
}

void rcc_drv_enable_math(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->DIG_CLK_EN.MATH_CLK_EN = 1;
}

//void rcc_drv_enable_posif(RCC_TypeDef *rcc_ptr)
//{
//    rcc_ptr->DIG_CLK_EN.POSIF_CLK_EN |= BIT1;
//}

void rcc_drv_enable_hall(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->DIG_CLK_EN.HALL_CLK_EN |= ENABLE;
}

void rcc_drv_enable_qep(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->DIG_CLK_EN.QEP_CLK_EN |= ENABLE;
}

void rcc_drv_enable_iwdt(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->DIG_CLK_EN.IWDT_CLK_EN = 1;
}

void rcc_drv_enable_uart_0(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->COM_CLK_EN.UART0_CLK_EN = 1;
}

void rcc_drv_enable_uart_1(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->COM_CLK_EN.UART1_CLK_EN = 1;
}

void rcc_drv_enable_spi_0(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->COM_CLK_EN.SPI0_CLK_EN = 1;
}

void rcc_drv_enable_spi_1(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->COM_CLK_EN.SPI1_CLK_EN = 1;
}

void rcc_drv_enable_iic_0(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->COM_CLK_EN.IIC0_CLK_EN = 1;
}

void rcc_drv_enable_iic_1(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->COM_CLK_EN.IIC1_CLK_EN = 1;
}

void rcc_drv_enable_tim_13(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->TIM_CLK_EN.TIM13_CLK_EN = 1;
}

void rcc_drv_enable_tim_14(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->TIM_CLK_EN.TIM14_CLK_EN = 1;
}

void rcc_drv_enable_tim_15(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->TIM_CLK_EN.TIM15_CLK_EN = 1;
}

void rcc_drv_enable_tim_16(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->TIM_CLK_EN.TIM16_CLK_EN = 1;
}

void rcc_drv_enable_mcpwm0(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->TIM_CLK_EN.MCPWM0_CLK_EN = 1;
}

#endif



/**********************************************************
 * hardware clock rst func
***********************************************************/
#if 1

/*
void clock_drv_rst_flc(RCC_TypeDef *rcc_clock_flc)
{
    rcc_clock_flc->AHBRSSTR.FLC |= BIT0;
}
*/

void rcc_drv_rst_gpio_a(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->AHBRSSTR.GPIOARST = 1;
}

void rcc_drv_rst_gpio_b(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->AHBRSSTR.GPIOBRST = 1;
}

void rcc_drv_rst_gpio_c(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->AHBRSSTR.GPIOCRST = 1;
}

void rcc_drv_rst_adc0(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->ANARST.ADC_RST = 1;
}

void rcc_drv_rst_math(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->DIGRST.MATHRST = 1;
}

//void rcc_drv_rst_posif(RCC_TypeDef *rcc_ptr)
//{
//    rcc_ptr->DIGRST.POSIFRST |= BIT1;
//}

void rcc_drv_rst_hall(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->DIGRST.HALLRST |= ENABLE;
}

void rcc_drv_rst_qep(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->DIGRST.QEPRST |= ENABLE;
}

void rcc_drv_rst_iwdt(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->DIGRST.IWDTRST = 1;
}

void rcc_drv_rst_wwdt(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->DIGRST.WWDTRST = 1;
}

void rcc_drv_rst_uart_0(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->COMRST.UART0RST = 1;
}

void rcc_drv_rst_uart_1(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->COMRST.UART1RST = 1;
}

void rcc_drv_rst_spi_0(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->COMRST.SPI0RST = 1;
}

void rcc_drv_rst_spi_1(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->COMRST.SPI1RST = 1;
}

void rcc_drv_rst_iic_0(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->COMRST.IIC0RST = 1;
}

void rcc_drv_rst_iic_1(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->COMRST.IIC1RST = 1;
}

void rcc_drv_rst_tim_13(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->TIMRST.TIM13RST = 1;
}

void rcc_drv_rst_tim_14(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->TIMRST.TIM14RST = 1;
}

void rcc_drv_rst_tim_15(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->TIMRST.TIM15RST = 1;
}

void rcc_drv_rst_tim_16(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->TIMRST.TIM16RST = 1;
}

void rcc_drv_rst_mcpwm0(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->TIMRST.MCPWM0RST = 1;
}


#endif
