/****************************************************************************/
/** \file main.c
**
**  PGA demo for ODS_5010 (Cortex-M0+)
**  
**
**  功能描述:
**  
**
*****************************************************************************/
#include "platform_common.h"
#include "test.h"
#include "gpio_hal.h"
#include "clock.h"
#include "uart_hal.h"
#include "uart_hal_cfg.h"
#include "adc_demo_test.h"

#include "cmdline_internal.h"


/**
 *@brief configure the gpio of UART0 
 *@param[in] none 
 *@return    none
 */
static void uart0_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;

	/* UART0_RX, PTA1  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_1;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* UART0_TX, PTA2  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_2;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
}


int main(void)
{
    delay_1us(200000);
    flash_clk_enable();
    gpio_clk_enable();
    uart_clk_enable();
	
    TIM13_Clk_Enable();

#ifdef UART
	uart0_gpio_init();
	
    (void)uart_init(UART_CONSOLE_INSTANCE, &uart0_init_config);
    uart_transmit_enable(UART_CONSOLE_INSTANCE, true);
#ifdef CMDLINE_SUPPORT
	cmdline_init();
#endif /*CMDLINE_SUPPORT*/

    INFO("\n SDK start running...%s,%s,%s\n", FW_VER, COMPILER_DATE, COMPILER_TIME);
    uint32_t mcu_clk = 0U;
    (void)clock_freq_get(SCG_SYS, &mcu_clk);
    INFO("MCU_CLK = %dMHz\n", mcu_clk/1000000U);
#endif


	/* CSA 初始化 */
	CSA_Init();
	
	INFO("CSA Init OK (Gain=x%d, Mode=Fixed_CH)\r\n", (1 << CSA_PGA_GAIN));

   
    while (1)
    {

    }
}

