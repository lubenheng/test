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
clock_mco_select rcc_drv_get_mco_clock_status(RCC_TypeDef *rcc_ptr);
uint8_t rcc_drv_get_mco_div(RCC_TypeDef *rcc_ptr);
uint32_t rcc_drv_get_mco_freq(RCC_TypeDef *rcc_ptr);


/**********************************************************
 * rcc oscillator control
***********************************************************/
void rcc_drv_hsi_enable(RCC_TypeDef *rcc_ptr);
void rcc_drv_hsi_disable(RCC_TypeDef *rcc_ptr);
uint8_t rcc_drv_hsi_ready(RCC_TypeDef *rcc_ptr);
clk_erc_e rcc_drv_hsi_wait_ready(RCC_TypeDef *rcc_ptr, uint32_t timeout_us);
void rcc_drv_lsi_enable(RCC_TypeDef *rcc_ptr);
void rcc_drv_lsi_disable(RCC_TypeDef *rcc_ptr);
uint8_t rcc_drv_lsi_ready(RCC_TypeDef *rcc_ptr);
clk_erc_e rcc_drv_lsi_wait_ready(RCC_TypeDef *rcc_ptr, uint32_t timeout_us);
void rcc_drv_hse_enable(RCC_TypeDef *rcc_ptr);
void rcc_drv_hse_disable(RCC_TypeDef *rcc_ptr);
void rcc_drv_hse_bypass(RCC_TypeDef *rcc_ptr, uint8_t bypass);
void rcc_drv_hse_fail_detect(RCC_TypeDef *rcc_ptr, uint8_t en);
uint8_t rcc_drv_hse_ready(RCC_TypeDef *rcc_ptr);
clk_erc_e rcc_drv_hse_wait_ready(RCC_TypeDef *rcc_ptr, uint32_t timeout_us);
void rcc_drv_lse_enable(RCC_TypeDef *rcc_ptr);
void rcc_drv_lse_disable(RCC_TypeDef *rcc_ptr);
void rcc_drv_lse_bypass(RCC_TypeDef *rcc_ptr, uint8_t bypass);
uint8_t rcc_drv_lse_ready(RCC_TypeDef *rcc_ptr);
clk_erc_e rcc_drv_lse_wait_ready(RCC_TypeDef *rcc_ptr, uint32_t timeout_us);


/**********************************************************
 * rcc system clock & PLL control
***********************************************************/
void rcc_drv_sys_select_clock(RCC_TypeDef *rcc_ptr,clock_sys_select sel);
void rcc_drv_sys_div_fclk(RCC_TypeDef *rcc_ptr, uint8_t div);
void rcc_drv_sys_div_pclk(RCC_TypeDef *rcc_ptr, uint8_t div);
clock_sys_select rcc_drv_sys_clock_status(RCC_TypeDef *rcc_ptr);
uint8_t rcc_drv_get_fclk_div(RCC_TypeDef *rcc_ptr);
uint8_t rcc_drv_get_pclk_div(RCC_TypeDef *rcc_ptr);
clk_erc_e rcc_drv_sys_clock_config(RCC_TypeDef *rcc_ptr, clock_sys_select sel, uint8_t fclk_div, uint8_t pclk_div);
clk_erc_e rcc_drv_sys_switch_clock(RCC_TypeDef *rcc_ptr, clock_sys_select sel);
uint32_t rcc_drv_get_sysclk_freq(RCC_TypeDef *rcc_ptr);
uint32_t rcc_drv_get_fclk_freq(RCC_TypeDef *rcc_ptr);
uint32_t rcc_drv_get_pclk_freq(RCC_TypeDef *rcc_ptr);

/* PLL */
void rcc_drv_pll_enable(RCC_TypeDef *rcc_ptr);
void rcc_drv_pll_disable(RCC_TypeDef *rcc_ptr);
clk_erc_e rcc_drv_pll_config(RCC_TypeDef *rcc_ptr, uint8_t mult, uint8_t div);
uint8_t rcc_drv_pll_ready(RCC_TypeDef *rcc_ptr);
uint8_t rcc_drv_pll_lock_error(RCC_TypeDef *rcc_ptr);
clk_erc_e rcc_drv_pll_wait_ready(RCC_TypeDef *rcc_ptr, uint32_t timeout_us);
uint32_t rcc_drv_pll_freq(RCC_TypeDef *rcc_ptr);
clk_erc_e rcc_drv_sys_clock_full_config(RCC_TypeDef *rcc_ptr, clock_sys_select sel,
                                        uint8_t pll_mult, uint8_t pll_div,
                                        uint8_t fclk_div, uint8_t pclk_div);


/**********************************************************
 * rcc peripheral clock enable / reset / clock source control
 * 外设时钟使能与复位统一控制（统一接口 + 兼容薄封装）+ PCKS0 时钟源选择
***********************************************************/
void rcc_drv_periph_clk_enable(RCC_TypeDef *rcc_ptr, rcc_periph_e periph);
void rcc_drv_periph_clk_disable(RCC_TypeDef *rcc_ptr, rcc_periph_e periph);
void rcc_drv_periph_reset(RCC_TypeDef *rcc_ptr, rcc_periph_e periph);
void rcc_drv_periph_reset_release(RCC_TypeDef *rcc_ptr, rcc_periph_e periph);
uint8_t rcc_drv_get_periph_clk_en(RCC_TypeDef *rcc_ptr, rcc_periph_e periph);
clk_erc_e rcc_drv_periph_clk_src_select(RCC_TypeDef *rcc_ptr, rcc_clksrc_periph_e periph, periph_clk_src_e src);
clk_erc_e rcc_drv_get_periph_clk_src(RCC_TypeDef *rcc_ptr, rcc_clksrc_periph_e periph, periph_clk_src_e *src);

void rcc_drv_enable_flc(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_gpio_a(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_gpio_b(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_gpio_c(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_adc0(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_math(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_hall(RCC_TypeDef *rcc_ptr);
void rcc_drv_enable_qep(RCC_TypeDef *rcc_ptr);
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

void rcc_drv_disable_flc(RCC_TypeDef *rcc_ptr);
void rcc_drv_disable_gpio_a(RCC_TypeDef *rcc_ptr);
void rcc_drv_disable_gpio_b(RCC_TypeDef *rcc_ptr);
void rcc_drv_disable_gpio_c(RCC_TypeDef *rcc_ptr);
void rcc_drv_disable_adc0(RCC_TypeDef *rcc_ptr);
void rcc_drv_disable_math(RCC_TypeDef *rcc_ptr);
void rcc_drv_disable_hall(RCC_TypeDef *rcc_ptr);
void rcc_drv_disable_qep(RCC_TypeDef *rcc_ptr);
void rcc_drv_disable_iwdt(RCC_TypeDef *rcc_ptr);
void rcc_drv_disable_uart_0(RCC_TypeDef *rcc_ptr);
void rcc_drv_disable_uart_1(RCC_TypeDef *rcc_ptr);
void rcc_drv_disable_spi_0(RCC_TypeDef *rcc_ptr);
void rcc_drv_disable_spi_1(RCC_TypeDef *rcc_ptr);
void rcc_drv_disable_iic_0(RCC_TypeDef *rcc_ptr);
void rcc_drv_disable_iic_1(RCC_TypeDef *rcc_ptr);
void rcc_drv_disable_tim_13(RCC_TypeDef *rcc_ptr);
void rcc_drv_disable_tim_14(RCC_TypeDef *rcc_ptr);
void rcc_drv_disable_tim_15(RCC_TypeDef *rcc_ptr);
void rcc_drv_disable_tim_16(RCC_TypeDef *rcc_ptr);
void rcc_drv_disable_mcpwm0(RCC_TypeDef *rcc_ptr);

void rcc_drv_rst_flc(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_gpio_a(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_gpio_b(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_gpio_c(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_adc0(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_math(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_hall(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_qep(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_iwdt(RCC_TypeDef *rcc_ptr);
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

void rcc_drv_rst_release_flc(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_release_gpio_a(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_release_gpio_b(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_release_gpio_c(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_release_adc0(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_release_math(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_release_hall(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_release_qep(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_release_iwdt(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_release_uart_0(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_release_uart_1(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_release_spi_0(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_release_spi_1(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_release_iic_0(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_release_iic_1(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_release_tim_13(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_release_tim_14(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_release_tim_15(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_release_tim_16(RCC_TypeDef *rcc_ptr);
void rcc_drv_rst_release_mcpwm0(RCC_TypeDef *rcc_ptr);


/**********************************************************
 * rcc system reset management
***********************************************************/
uint32_t rcc_drv_get_reset_status(RCC_TypeDef *rcc_ptr);
void rcc_drv_clear_reset_flag(RCC_TypeDef *rcc_ptr, uint32_t flags);
clk_erc_e rcc_drv_reset_src_enable(RCC_TypeDef *rcc_ptr, clock_rst_src_e src, uint8_t en);
void rcc_drv_sys_reset(void);
void rcc_drv_boot_reset_retrim(RCC_TypeDef *rcc_ptr);


/**********************************************************
 * rcc chip version info (VERID)
***********************************************************/
uint32_t rcc_drv_get_verid(RCC_TypeDef *rcc_ptr);
uint8_t rcc_drv_get_verid_minor(RCC_TypeDef *rcc_ptr);
uint8_t rcc_drv_get_verid_major(RCC_TypeDef *rcc_ptr);
uint16_t rcc_drv_get_verid_feature(RCC_TypeDef *rcc_ptr);


#endif
