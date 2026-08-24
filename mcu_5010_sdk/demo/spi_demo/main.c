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
#include "spi_hal.h"

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


/**
 *@brief configure GPIO_C pin 1/2/3/4 to spi function
 *@param[in] none
 *@return    none
 */
static void spi_demo_pins_init(void)
{
    gpio_settings_config_t setting_cfg;
		
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_1;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_5;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    setting_cfg.gpio_pin = GPIO_PIN_2;
    gpio_init(&setting_cfg);

    setting_cfg.gpio_pin = GPIO_PIN_3;
    gpio_init(&setting_cfg);

    setting_cfg.gpio_pin = GPIO_PIN_4;
    gpio_init(&setting_cfg);
}

typedef enum
{
    SPI_MASTER_DEMO = 0,
    SPI_SLAVE_DEMO
}spi_mode_t;

spi_mode_t spi_mode = SPI_MASTER_DEMO;

SPI_MasterHandleTypeDef spim_config = {
    .baseConfig = {
        .CPOL = 0,
        .CPHA = 0,
        .IsLsbFirst = 0,
        .AddrLenByte = 0,   //地址长度选择1字节长度
        .DataLenBit = 7
    },
    .transConfig = {        //CMD和ADDR全部失能，纯数据传输
        .IsCmdEn = 0,
        .IsAddrEn = 0,
        .CmdCode = 0,       //CMD使能时设置命令值
        .TransAddr = 0      //ADDR使能时设置命令值
    },
    .fifoDmaConfig = {
        .TxThres = 5UL,
        .RxThres = 1UL
    },
    .timingConfig = {
        .SCLK_SPEED = 100000
    }
}; 

SPI_SlaveHandleTypeDef spis_config = {
    .baseConfig = {
        .CPOL = 0,
        .CPHA = 0,
        .IsLsbFirst = 0,              // MSB优先
        .DataLenBit = 7,              // 8bit数据位宽
        .IsMosiBiDirEn = 0,
    },
    .fifoDmaConfig = {
    	.TxThres = 4, // TX FIFO阈值
    	.RxThres = 1  // RX FIFO阈值
    }
};

uint8_t send_data_buf[32] = {1, 2, 3, 4, 5, 6, 7, 8};
uint8_t recv_data_buf[32] = {0};

transmit_data send_data = {
    .data = send_data_buf,
    .data_len = 32
};

transmit_data recv_data = {
    .data = recv_data_buf,
    .data_len = 32
};

int main(void)
{
    delay_1us(200000);
	 
	/* Add Clock Enable Code Here */
    flash_clk_enable();
	gpio_clk_enable();
    uart_clk_enable();
    spi_clk_enable();

	uart0_gpio_init();
	(void)uart_init(UART_CONSOLE_INSTANCE, &uart0_init_config);
	uart_transmit_enable(UART_CONSOLE_INSTANCE, true);
	cmdline_init();

	INFO("\n SDK start running...%s,%s,%s\n", FW_VER, COMPILER_DATE, COMPILER_TIME);

	uint32_t mcu_clk = 0U;
	(void)clock_freq_get(SCG_SYS, &mcu_clk);
	INFO("MCU_CLK = %dMHz\n", mcu_clk/1000000U);

	spi_demo_pins_init();

    if(spi_mode == SPI_MASTER){
        spi_hal_master_init(SPI_INSTANCE0, &spim_config);
	    delay_1us(1000);
        //发送和接收数据同时进行
        spi_hal_master_send_recv_data(SPI_INSTANCE0, &send_data, &recv_data);
        delay_1us(1000);
        INFO("recv data: %s\n", recv_data_buf);
    } else if(spi_mode == SPI_SLAVE){
        spi_hal_slave_init(SPI_INSTANCE0, &spis_config);
        //将需要发送的数据预留在数据缓冲区
        spi_hal_slave_send_data(SPI_INSTANCE0, &send_data);
        //指定SPI数据接收缓冲区
        spi_hal_slave_recv_buf_set(recv_data_buf);
        delay_1us(1000);
        //延时这段时间可以使用主机向芯片发送数据或接收数据
        delay_1us(5000);
        INFO("recv data: %s\n", recv_data_buf);
    }
    
    while(1)
    {
		
    }
}
