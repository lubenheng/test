#ifndef CLOCK_HAL_H
#define CLOCK_HAL_H

#include "clock_type.h"
#include "ODS_5010.h"

#define CLOCK_HAL_STATUS 0      //查看rcc的hsi和pllclk时钟源的状态，目前没用到（读取寄存器无响应）

/**********************************************************
 * rcc mco control
***********************************************************/
void rcc_hal_mco_clock_select(clock_mco_select sel);
void rcc_hal_mco_div(clock_mco_div div);




/**********************************************************
 * rcc system clock control
***********************************************************/
void rcc_hal_sys_select_clock(clock_sys_select sel);
void rcc_hal_sys_div_fclk(clock_sys_fclk_div div);
void rcc_hal_sys_div_pclk(clock_sys_pclk_div div);
void rcc_hal_sys_change_clock(clock_sys_select sel, clock_sys_fclk_div fdiv, clock_sys_pclk_div pdiv);
clock_sys_select rcc_hal_sys_clock_status(void);



clk_erc_e clock_freq_get(module_name_e module_name, uint32_t *freq);
clk_erc_e clock_module_clk_cfg(const module_clk_cfg_t *module_clk_cfg);


void flash_clk_enable(void);
void gpio_clk_enable(void);

void ADC_Clk_Disable(void);
void ADC_Clk_Enable(void);

void wdog_clk_enable(void);
void qep_clk_enable(void);
void hall_clk_enable(void);

void HWMATH_Clk_Enable(void);
void HWMATH_Clk_Disable(void);



void i2c_clk_enable(void);
void spi_clk_enable(void);
void uart_clk_enable(void);
void can_clk_enable(void);
void mcpwm_clk_enable(void);




void TIM13_Clk_Enable(void);
void TIM14_Clk_Enable(void);
void TIM15_Clk_Enable(void);
void TIM16_Clk_Enable(void);
void TIM13_Clk_Disable(void);
void TIM14_Clk_Disable(void);
void TIM15_Clk_Disable(void);
void TIM16_Clk_Disable(void);





#endif  /*  CLOCK_HAL_H */
