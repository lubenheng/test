/****************************************************************************/
/** \file main.c
**
**  GPIO demo for ODS_5010 (Cortex-M0+)
**  PA0-PA15,PB0-PB15,PC0-PC8
*****************************************************************************/
#include "platform_common.h"
#include "test.h"
#include "gpio_hal.h"
#include "clock.h"
#include "uart_hal.h"
#include "uart_hal_cfg.h"
#include "test.h"
#include "flash_hal.h"

#include "cmdline_internal.h"

/* main flash scratch area used for the write/read demo, kept away from */
/* 32KB = 0x08000		64KB = 0x10000   96KB = 0x18000   128KB = 0x20000   */
#define FLASH_DEMO_TEST_ADDR   (0x10000)  /* byte offset,  word aligned  */
#define FLASH_DEMO_TEST_LEN    (4U)       /* unit: word */


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
/**
 *@brief enable the clock of UART0
 *@param[in] none
 *@return    none
 */
static void uart0_clk_init(void)
{
//	module_clk_cfg_t module_clk_cfg;
//	
//	module_clk_cfg.module_name = MODULE_UART0;
//	module_clk_cfg.fclk_sel = APB_CLK;
//	module_clk_cfg.fclk_div = PER_DIVIDE_1; 
//	module_clk_cfg.pclk_dsleep_gating_en = GATING_ON;
//	module_clk_cfg.fclk_dsleep_gating_en = GATING_ON;
//	
//	(void)clock_module_clk_cfg(&module_clk_cfg);
}

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



/**
 *@brief demo: write a word array to main flash, read it back and verify
 *@param[in] none
 *@return    none
 */
static void flash_write_demo(void)
{
    uint32_t write_data[FLASH_DEMO_TEST_LEN] = {0x11223344U, 0x55667788U, 0x99AABBCCU, 0xDDEEFF00U};
    uint32_t read_data[FLASH_DEMO_TEST_LEN] = {0U};
    flash_erc_e erc;
    uint32_t i;

    erc = flash_write(MAIN_FLASH, FLASH_DEMO_TEST_ADDR, write_data, FLASH_DEMO_TEST_LEN);
    if (erc != ERC_FLASH_NO_ERROR)
    {
        INFO("flash_demo: write failed, erc=%d\n", erc);
        return;
    }

    erc = flash_read_by_ahb(MAIN_FLASH, FLASH_DEMO_TEST_ADDR, read_data, FLASH_DEMO_TEST_LEN);
    if (erc != ERC_FLASH_NO_ERROR)
    {
        INFO("flash_demo: read failed, erc=%d\n", erc);
        return;
    }

    for (i = 0U; i < FLASH_DEMO_TEST_LEN; i++)
    {
        if (read_data[i] != write_data[i])
        {
            INFO("flash_demo: verify mismatch at word %d, write=0x%08x, read=0x%08x\n",
                 i, write_data[i], read_data[i]);
            return;
        }
    }

    INFO("flash_demo: write/read verify passed\n");
}

int main(void)
{
    delay_1us(2000);

    flash_clk_enable();
	 
	/* Add Clock Enable Code Here */
	gpio_clk_enable();
	
  uart_clk_enable();
		
#ifdef UART 
	uart0_gpio_init();
	uart0_clk_init();
	(void)uart_init(UART_CONSOLE_INSTANCE, &uart0_init_config);
    
	uart_transmit_enable(UART_CONSOLE_INSTANCE, true);
   
#ifdef CMDLINE_SUPPORT
	cmdline_init();
#endif /*CMDLINE_SUPPORT*/

		
	INFO("\n SDK start running...%s,%s,%s\n", FW_VER, COMPILER_DATE, COMPILER_TIME);

	uint32_t mcu_clk = 0U;
	(void)clock_freq_get(SCG_SYS, &mcu_clk);
	INFO("MCU_CLK = %dMHz\n", mcu_clk/1000000U);
#endif /*UART*/

 //ram_gpio_bypass();
	gpio_demo_pins_init();

	flash_write_demo();

#ifdef CMD_MODULE_TEST
	module_test();
#endif

	__enable_irq();						  		/* 使能全部中断 */
	//__disable_irq();					    /* 使能全部中断 */
	
 
    while(1)
    {
				cmdline_handle_polling();
        gpio_output_set(GPIO_C, GPIO_PIN_6, GPIO_HIGH_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_7, GPIO_HIGH_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_8, GPIO_HIGH_LEVEL);
        delay_1us(500);

        gpio_output_set(GPIO_C, GPIO_PIN_6, GPIO_LOW_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_7, GPIO_LOW_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_8, GPIO_LOW_LEVEL);
        delay_1us(500);
    }
}
