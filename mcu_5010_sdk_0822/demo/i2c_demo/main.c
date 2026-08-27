/****************************************************************************/
/** \file main.c
**
**  i2c demo for ODS_5010 (Cortex-M0+)
**  PA3-SDA, PA4-SCL
*****************************************************************************/
#include "platform_common.h"
#include "test.h"
#include "gpio_hal.h"
#include "clock.h"
#include "uart_hal.h"
#include "uart_hal_cfg.h"
#include "test.h"
#include "i2c_hal.h"

#include "cmdline_internal.h"

/* I2C发送和接收全部使用中断来收发，需要开启中断*/

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
 *@brief init i2c gpio
 *@param[in] none
 *@return    none
 */
static void i2c_gpio_init(void)
{
	gpio_settings_config_t setting_cfg;

	/* I2C_SDA, PTA3  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_3;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_4;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* I2C_SCL, PTA4  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_4;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_4;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
	
}


i2c_master_config_t i2c_master_config = {
    .master_mode = I2C_FAST_MODE,
    .i2c_speed = 100000U,
    .master_interrupt_priority = 1U
};

uint8_t i2c_write_data[32] = {0};
uint8_t i2c_read_data[32] = {0};

i2c_master_msg_t i2c_master_msg = {
    .addr_10bit = false,
    .slave_addr = 0x50U,
    .slave_reg_addr = 0x00U,
    .send_data = i2c_write_data,
    .rec_data = i2c_read_data,
    .send_data_len = 32U,
    .rec_data_len = 32U,
};

int main(void)
{
    delay_1us(200000);

	/* Add Clock Enable Code Here */
    flash_clk_enable();
	gpio_clk_enable();
	uart_clk_enable();
    i2c_clk_enable();

    /* uart cmdline init*/
	  uart0_gpio_init();
	  (void)uart_init(UART_CONSOLE_INSTANCE, &uart0_init_config);
    uart_transmit_enable(UART_CONSOLE_INSTANCE, true);
    cmdline_init();
	  INFO("\n SDK start running...%s,%s,%s\n", FW_VER, COMPILER_DATE, COMPILER_TIME);
	  uint32_t mcu_clk = 0U;
	  (void)clock_freq_get(SCG_SYS, &mcu_clk);
	  INFO("MCU_CLK = %dMHz\n", mcu_clk/1000000U);

    /* i2c init */
    i2c_gpio_init();
   
    i2c_master_init(I2C_INSTANCE0, &i2c_master_config);

    /* start code*/
    strcpy((char *)i2c_write_data, "I2C Master Test Data");
    i2c_master_msg.send_data_len = strlen((char *)i2c_write_data);
    i2c_master_msg.rec_data_len = 32U;
    i2cm_send_data(I2C_INSTANCE0, &i2c_master_msg);
    delay_1us(1000);
    i2cm_receive_data(I2C_INSTANCE0, &i2c_master_msg);
    delay_1us(500);
    INFO("I2C Master Recv Data: %s\n", i2c_read_data);
    
    

	  __enable_irq();						  		/* 使能全部中断 */
	  //__disable_irq();					    /* 使能全部中断 */
	
 
    while(1)
    {
				cmdline_handle_polling();
    }
}
