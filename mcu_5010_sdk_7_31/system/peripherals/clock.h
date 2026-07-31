#ifndef CLOCK_H
#define CLOCK_H

#include "platform_common.h"

#include "clock_reg.h"
#include "clock_type.h"
#include "device_resource.h"
#include "clock.h"

/**********************************************************
 * mco clock control
***********************************************************/
void rcc_drv_mco_clock_select(RCC_TypeDef *rcc_ptr, clock_mco_select sel);
void rcc_drv_mco_div(RCC_TypeDef *rcc_ptr, uint8_t div);


/**********************************************************
 * rcc system clock control
***********************************************************/
void rcc_drv_sys_select_clock(RCC_TypeDef *rcc_ptr,clock_sys_select sel);
void rcc_drv_sys_div_fclk(RCC_TypeDef *rcc_ptr, uint8_t div);
void rcc_drv_sys_div_pclk(RCC_TypeDef *rcc_ptr, uint8_t div);
clock_sys_select rcc_drv_sys_clock_status(RCC_TypeDef *rcc_ptr);


/**********************************************************
 * hardware clock enable func
***********************************************************/
void rcc_drv_enable_flc(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_gpio_a(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_gpio_b(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_gpio_c(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_adc0(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_math(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_posif(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_iwdt(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_uart_0(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_uart_1(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_spi_0(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_spi_1(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_iic_0(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_iic_1(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_tim_13(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_tim_14(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_tim_15(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_tim_16(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_mcpwm0(RCC_TypeDef *rcc_ptr);


/**********************************************************
 * hardware clock rst func
***********************************************************/
//void clock_drv_rst_flc(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_gpio_a(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_gpio_b(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_gpio_c(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_adc0(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_math(RCC_TypeDef *rcc_ptr);
//void rcc_drv_rst_posif(RCC_TypeDef *rcc_ptr);

void rcc_drv_rst_hall(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_qep(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_iwdt(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_wwdt(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_uart_0(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_uart_1(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_spi_0(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_spi_1(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_iic_0(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_iic_1(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_tim_13(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_tim_14(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_tim_15(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_tim_16(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_mcpwm0(RCC_TypeDef *rcc_ptr);


#endif
