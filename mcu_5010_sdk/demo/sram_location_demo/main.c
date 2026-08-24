/****************************************************************************/
/** \file main.c
**
**  GPIO demo for ODS_5010 (Cortex-M0+)
**  PA0-PA15,PB0-PB15,PC0-PC8
*****************************************************************************/
#include "platform_common.h"
//#include "test.h"
#include "gpio_hal.h"
//#include "clock.h"
//#include "uart_hal.h"
//#include "uart_hal_cfg.h"


//#include "cmdline_internal.h"


/**
 *@brief configure GPIO_C pin 6/7/8/9 as push-pull outputs
 *@param[in] none
 *@return    none
 */
static void gpio_demo_pins_init(void)
{
    gpio_settings_config_t setting_cfg;
		
		//PTC6 
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_6;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

		//PTC7
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_7;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

		//PTC8
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_8;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
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
	 
	/* Add Clock Enable Code Here */
	gpio_clk_enable();
	
 //ram_gpio_bypass();
	gpio_demo_pins_init();


	__enable_irq();						  		/* 使能全部中断 */
	//__disable_irq();					    /* 使能全部中断 */
	
 
    while(1)
    {
				//cmdline_handle_polling();
        gpio_output_set(GPIO_C, GPIO_PIN_6, GPIO_HIGH_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_7, GPIO_HIGH_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_8, GPIO_HIGH_LEVEL);
        delay_1us(100);

        gpio_output_set(GPIO_C, GPIO_PIN_6, GPIO_LOW_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_7, GPIO_LOW_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_8, GPIO_LOW_LEVEL);
        delay_1us(100);
    }
}
