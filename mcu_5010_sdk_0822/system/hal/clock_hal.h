#ifndef CLOCK_HAL_H
#define CLOCK_HAL_H

#include "clock_type.h"
#include "ODS_5010.h"

#define CLOCK_HAL_STATUS 0      //查看rcc的hsi和pllclk时钟源的状态，目前没用到（读取寄存器无响应）

void RCC_Init(void);

/**********************************************************
 * rcc mco control
***********************************************************/
void rcc_hal_mco_clock_select(clock_mco_select sel);
void rcc_hal_mco_div(clock_mco_div div);
clock_mco_select rcc_hal_get_mco_clock_status(void);
uint8_t rcc_hal_get_mco_div(void);
uint32_t rcc_hal_get_mco_freq(void);

/**********************************************************
 * rcc oscillator control
***********************************************************/
clk_erc_e rcc_hal_osc_enable(clock_osc_e osc, uint32_t timeout_us);
void rcc_hal_osc_enable_all(void);



/**********************************************************
 * rcc system clock control
***********************************************************/
void rcc_hal_sys_select_clock(clock_sys_select sel);
void rcc_hal_sys_div_fclk(clock_sys_fclk_div div);
void rcc_hal_sys_div_pclk(clock_sys_pclk_div div);
void rcc_hal_sys_change_clock(sys_clk_config_t *sys_clk_cfg);
clock_sys_select rcc_hal_sys_clock_status(void);
uint8_t rcc_hal_get_fclk_div(void);
uint8_t rcc_hal_get_pclk_div(void);
uint32_t rcc_hal_get_sysclk_freq(void);
uint32_t rcc_hal_get_fclk_freq(void);
uint32_t rcc_hal_get_pclk_freq(void);
clk_erc_e rcc_hal_sys_clock_config(clock_sys_select sel, clock_sys_fclk_div fclk_div, clock_sys_pclk_div pclk_div);
clk_erc_e rcc_hal_sys_switch_clock(clock_sys_select sel);

/* PLL */
void rcc_hal_pll_enable(void);
void rcc_hal_pll_disable(void);
clk_erc_e rcc_hal_pll_config(uint8_t mult, uint8_t div);
uint8_t rcc_hal_pll_ready(void);
uint8_t rcc_hal_pll_lock_error(void);
clk_erc_e rcc_hal_pll_wait_ready(uint32_t timeout_us);
uint32_t rcc_hal_pll_freq(void);
clk_erc_e rcc_hal_sys_clock_full_config(clock_sys_select sel, uint8_t pll_mult, uint8_t pll_div, clock_sys_fclk_div fclk_div, clock_sys_pclk_div pclk_div);



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
