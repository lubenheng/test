#include "platform_common.h"
#include <stdlib.h>

#include "test.h"

#include "trgmux_hal.h"
#include "trgmux_test.h"
#include "trgmux_reg.h"
#include "gpio_hal.h"

#include "wdog_hal.h"
#include "uart_hal.h"
#include "uart_test.h"
#include "uart_reg.h"
#include "uart.h"
#include "rstgen_hal.h"


static gpio_settings_config_t uart0_arr0_cfg[2U] =
{
    {
			  //RX PTC2
        .gpio_port = GPIO_C,
        .gpio_pin = GPIO_PIN_2,
        .gpio_alt = GPIO_ALT_FUNC_4,
        .gpio_dir = GPIO_DIR_OUTPUT,
        .pull_cfg = GPIO_PULL_DISABLE,
        .gpio_strength = GPIO_STRENGTH_LOW_FAST,
    },
    {
			  //TX PTC3
        .gpio_port = GPIO_C,
        .gpio_pin = GPIO_PIN_3,
        .gpio_alt = GPIO_ALT_FUNC_4,
        .gpio_dir = GPIO_DIR_OUTPUT,
        .pull_cfg = GPIO_PULL_DISABLE,
        .gpio_strength = GPIO_STRENGTH_LOW_FAST,

    },
};

static gpio_settings_config_t uart0_arr1_cfg[2U] =
{
    {
			  //RX PTB0
        .gpio_port = GPIO_B,
        .gpio_pin = GPIO_PIN_0,
        .gpio_alt = GPIO_ALT_FUNC_2,
        .gpio_dir = GPIO_DIR_OUTPUT,
        .pull_cfg = GPIO_PULL_DISABLE,
        .gpio_strength = GPIO_STRENGTH_LOW_FAST,
    },
    {
			  //TX PTB1
        .gpio_port = GPIO_B,
        .gpio_pin = GPIO_PIN_1,
        .gpio_alt = GPIO_ALT_FUNC_2,
        .gpio_dir = GPIO_DIR_OUTPUT,
        .pull_cfg = GPIO_PULL_DISABLE,
        .gpio_strength = GPIO_STRENGTH_LOW_FAST,

    },
};

static gpio_settings_config_t uart0_arr2_cfg[2U] =
{
    {
			  //RX PTA2
        .gpio_port = GPIO_A,
        .gpio_pin = GPIO_PIN_2,
        .gpio_alt = GPIO_ALT_FUNC_6,
        .gpio_dir = GPIO_DIR_OUTPUT,
        .pull_cfg = GPIO_PULL_DISABLE,
        .gpio_strength = GPIO_STRENGTH_LOW_FAST,
    },
    {
			  //TX PTA3
        .gpio_port = GPIO_A,
        .gpio_pin = GPIO_PIN_3,
        .gpio_alt = GPIO_ALT_FUNC_6,
        .gpio_dir = GPIO_DIR_OUTPUT,
        .pull_cfg = GPIO_PULL_DISABLE,
        .gpio_strength = GPIO_STRENGTH_LOW_FAST,

    },
};

static gpio_settings_config_t uart1_arr0_cfg[2U] =
{
    {
			  //RX PTD6
        .gpio_port = GPIO_D,
        .gpio_pin = GPIO_PIN_6,
        .gpio_alt = GPIO_ALT_FUNC_2,
        .gpio_dir = GPIO_DIR_OUTPUT,
        .pull_cfg = GPIO_PULL_DISABLE,
        .gpio_strength = GPIO_STRENGTH_LOW_FAST,
    },
    {
			  //TX PTD7
        .gpio_port = GPIO_D,
        .gpio_pin = GPIO_PIN_7,
        .gpio_alt = GPIO_ALT_FUNC_2,
        .gpio_dir = GPIO_DIR_OUTPUT,
        .pull_cfg = GPIO_PULL_DISABLE,
        .gpio_strength = GPIO_STRENGTH_LOW_FAST,

    },
};

static gpio_settings_config_t uart1_arr1_cfg[2U] =
{
    {
			  //RX PTC8
        .gpio_port = GPIO_C,
        .gpio_pin = GPIO_PIN_8,
        .gpio_alt = GPIO_ALT_FUNC_2,
        .gpio_dir = GPIO_DIR_OUTPUT,
        .pull_cfg = GPIO_PULL_DISABLE,
        .gpio_strength = GPIO_STRENGTH_LOW_FAST,
    },
    {
			  //TX PTC9
        .gpio_port = GPIO_C,
        .gpio_pin = GPIO_PIN_9,
        .gpio_alt = GPIO_ALT_FUNC_2,
        .gpio_dir = GPIO_DIR_OUTPUT,
        .pull_cfg = GPIO_PULL_DISABLE,
        .gpio_strength = GPIO_STRENGTH_LOW_FAST,

    },
};

static gpio_settings_config_t uart1_arr2_cfg[2U] =
{
    {
			  //RX PTC6
        .gpio_port = GPIO_C,
        .gpio_pin = GPIO_PIN_6,
        .gpio_alt = GPIO_ALT_FUNC_2,
        .gpio_dir = GPIO_DIR_OUTPUT,
        .pull_cfg = GPIO_PULL_DISABLE,
        .gpio_strength = GPIO_STRENGTH_LOW_FAST,
    },
    {
			  //TX PTC7
        .gpio_port = GPIO_C,
        .gpio_pin = GPIO_PIN_7,
        .gpio_alt = GPIO_ALT_FUNC_2,
        .gpio_dir = GPIO_DIR_OUTPUT,
        .pull_cfg = GPIO_PULL_DISABLE,
        .gpio_strength = GPIO_STRENGTH_LOW_FAST,

    },
};


static gpio_settings_config_t uart0_cts0_cfg[1U] = 
{
	{
	    .gpio_port            = GPIO_C,
	    .gpio_pin             = GPIO_PIN_8,
	    .gpio_alt             = GPIO_ALT_FUNC_6,
	    .gpio_dir             = GPIO_DIR_INPUT,
	    .pull_cfg             = GPIO_PULL_DISABLE,
	    .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
	},
};

static gpio_settings_config_t uart0_cts1_cfg[1U] = 
{
	{
	    .gpio_port            = GPIO_A,
	    .gpio_pin             = GPIO_PIN_0,
	    .gpio_alt             = GPIO_ALT_FUNC_6,
	    .gpio_dir             = GPIO_DIR_INPUT,
	    .pull_cfg             = GPIO_PULL_DISABLE,
	    .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
	},
};


static gpio_settings_config_t uart1_cts0_cfg[1U] = 
{
	{
	    .gpio_port            = GPIO_A,
	    .gpio_pin             = GPIO_PIN_6,
	    .gpio_alt             = GPIO_ALT_FUNC_6,
	    .gpio_dir             = GPIO_DIR_OUTPUT,
	    .pull_cfg             = GPIO_PULL_DISABLE,
	    .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
	},
};

static gpio_settings_config_t uart1_cts1_cfg[1U] = 
{
	{
	    .gpio_port            = GPIO_E,
	    .gpio_pin             = GPIO_PIN_2,
	    .gpio_alt             = GPIO_ALT_FUNC_6,
	    .gpio_dir             = GPIO_DIR_OUTPUT,
	    .pull_cfg             = GPIO_PULL_DISABLE,
	    .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
	},
};

static gpio_settings_config_t uart0_rts0_cfg[1U] = 
{
	{
	    .gpio_port            = GPIO_C,
	    .gpio_pin             = GPIO_PIN_9,
	    .gpio_alt             = GPIO_ALT_FUNC_6,
	    .gpio_dir             = GPIO_DIR_OUTPUT,
	    .pull_cfg             = GPIO_PULL_DISABLE,
	    .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
	},
};

static gpio_settings_config_t uart0_rts1_cfg[1U] = 
{
	{
	    .gpio_port            = GPIO_A,
	    .gpio_pin             = GPIO_PIN_1,
	    .gpio_alt             = GPIO_ALT_FUNC_6,
	    .gpio_dir             = GPIO_DIR_OUTPUT,
	    .pull_cfg             = GPIO_PULL_DISABLE,
	    .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
	},
};

static gpio_settings_config_t uart1_rts0_cfg[1U] = 
{
	{
	    .gpio_port            = GPIO_A,
	    .gpio_pin             = GPIO_PIN_7,
	    .gpio_alt             = GPIO_ALT_FUNC_6,
	    .gpio_dir             = GPIO_DIR_OUTPUT,
	    .pull_cfg             = GPIO_PULL_DISABLE,
	    .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
	},
};

static gpio_settings_config_t uart1_rts1_cfg[1U] = 
{
	{
	    .gpio_port            = GPIO_E,
	    .gpio_pin             = GPIO_PIN_6,
	    .gpio_alt             = GPIO_ALT_FUNC_6,
	    .gpio_dir             = GPIO_DIR_OUTPUT,
	    .pull_cfg             = GPIO_PULL_DISABLE,
	    .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
	},
};

static UART_TypeDef * uart_base[UART_INSTANCE_MAX_COUNT] = UART_BASE_ADDRS;
uint8_t buffer8[7] = {0x55, 0x11, 0x44, 0x44, 0x44, 0x44, 0x44};
uint8_t trig_test_data[12] = {0x00,0x00,0x74,0x68,0x20,0x69,0x73,0x20,0x4F,0x4B,0x21,0x0A};

uint8_t buffer_rx[10] = {0};
uint8_t buffer_tx[10] = {0xC1, 0x03, 0xC2, 0x03, 0xC3, 0x03, 0xC4, 0x03, 0xC5, 0x03};
uint8_t buffer_tx_irda[4] = {0x55, 0x55, 0x55, 0x55};

uint8_t tx_size = 10U;
uint8_t rx_size = 10U;




static void pins_mux_drv_init(uint32_t pin_count, gpio_settings_config_t *config)
{
    uint32_t i;
    
    for (i = 0U; i < pin_count; i++)
    {
        gpio_init(&config[i]);     
    }
}


/**
 *@brief initialize the pin of uart
 *@param[in] instance: uart instance number
 *@param[in] pin_mux: pin mux select, one of <UART_MUX_SEL0, UART_MUX_SEL1, UART_MUX_SEL2, UART_MUX_SEL3>
 *@return    none
 */
 static void uart_gpio_init(uart_instance_e instance, uart_mux_sel_e pin_mux)
{
	DEV_ASSERT(instance < UART_INSTANCE_MAX_COUNT);
	DEV_ASSERT(pin_mux < UART_MUX_MAX);


	if (instance == UART_INSTANCE0)
	{
		switch(pin_mux)
		{
			case UART_MUX_SEL0:
				pins_mux_drv_init(2U, uart0_arr0_cfg);
				break;
			case UART_MUX_SEL1:
				pins_mux_drv_init(2U, uart0_arr1_cfg);
				break;
			case UART_MUX_SEL2:
				pins_mux_drv_init(2U, uart0_arr2_cfg);
				break;
			default:
				INFO("input param error\n");
				break;
		}
	}
	else if (instance == UART_INSTANCE1)
	{
		switch(pin_mux)
		{
			case UART_MUX_SEL0:
				pins_mux_drv_init(2U, uart1_arr0_cfg);
				break;
			case UART_MUX_SEL1:
				pins_mux_drv_init(2U, uart1_arr1_cfg);
				break;
			case UART_MUX_SEL2:
				pins_mux_drv_init(2U, uart1_arr2_cfg);
				break;
			default:
				INFO("input param error\n");
				break;
		}
	}
}

static void uart_clk_init(uart_instance_e instance)
{
	module_clk_cfg_t module_clk_cfg;
	

	module_name_e module_name[] = {MODULE_UART0, MODULE_UART1};
	
	module_clk_cfg.module_name = module_name[instance];
	module_clk_cfg.fclk_sel = APB_CLK;
	module_clk_cfg.fclk_div = PER_DIVIDE_1;
	module_clk_cfg.pclk_dsleep_gating_en = GATING_ON;
	module_clk_cfg.fclk_dsleep_gating_en = GATING_ON;
	
	clock_module_clk_cfg(&module_clk_cfg);
}

void uart_reset_default_config(void)
{
	uart1_init_config.baud_rate = 9600;
	uart1_init_config.parity_mode = UART_PARITY_DISABLED;
	uart1_init_config.stop_bit_number = UART_ONE_STOP_BIT;
	uart1_init_config.bit_number_per_char = UART_8_BITS_PER_CHAR;
	uart1_init_config.tx_bit_parity = UART_POLARITY_NORMAL;
	uart1_init_config.rx_bit_parity = UART_POLARITY_NORMAL;
	uart1_init_config.bit_order = UART_LSB_FIRST;
	uart1_init_config.autobaud_en = FUNC_DISABLE;
}

void uart_reset_rx_buffer(void)
{
	for (uint8_t i = 0; i < rx_size; i++)
	{
		buffer_rx[i] = 0;
	}
}

#if (MCU_TYPE == MCU_SA32A12)
void static uart1_cd_test_gpio_init(void)
{
	gpio_settings_config_t setting_cfg;
	//RX PTC6
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_6;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_LOW_FAST;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    //TX PTC7
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_7;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_LOW_FAST;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    //RTS PTA7
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_7;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_6;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_LOW_FAST;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);  
}


void uart_over_run_test(void)
{
	uart_reset_default_config();
	UART_TypeDef *base = uart_base[UART_INSTANCE1];
	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	
	uart_init(UART_INSTANCE1, &uart1_init_config);
	uart_receive_enable(UART_INSTANCE1, true);
	base->FIFO.RX_FIFO_EN = 1U;

	while(1)
	{
		;
	}
}

void uart_busy_flag_test(void)
{
	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	
	uart_init(UART_INSTANCE1, &uart1_init_config);
	uart_receive_enable(UART_INSTANCE1, true);
	uart_interrupt_handle_cfg(UART_INSTANCE1, uart_irq_handler);
	uart_receive_data(UART_INSTANCE1, buffer_rx, 4);
	delay_ms(100);

	while(1)
	{
		;
	}
}

void nmi_interrupt_test()
{
	gpio_settings_config_t setting_cfg;

    /* PTD3, 54 */
    setting_cfg.gpio_port = GPIO_D;
    setting_cfg.gpio_pin = GPIO_PIN_3;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_7;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_LOW_FAST;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
}

void uart_mutual_test(uint32_t baudrate,
					uint32_t parity_mode,
					uint32_t stop_bit_number,
					uint32_t bit_number_per_char,
					uint32_t tx_bit_parity,
					uint32_t rx_bit_parity,
					uint32_t bit_order)
{
	uart_reset_default_config();
	uart1_init_config.baud_rate = baudrate;
	uart1_init_config.parity_mode = (uart_parity_mode_e)parity_mode;
	uart1_init_config.stop_bit_number = (uart_stop_bit_count_e)stop_bit_number;
	uart1_init_config.bit_number_per_char = (uart_bit_number_per_char_e)bit_number_per_char;
	uart1_init_config.tx_bit_parity = (uart_tx_rx_polarity_e)tx_bit_parity;
	uart1_init_config.rx_bit_parity = (uart_tx_rx_polarity_e)rx_bit_parity;
	uart1_init_config.bit_order = (uart_bit_order_e)bit_order;
	
	//UART1 receive data
	uart_gpio_init(UART_INSTANCE1, UART_MUX_SEL1);
	uart_clk_init(UART_INSTANCE1);
	uart_init(UART_INSTANCE1, &uart1_init_config);
	uart_interrupt_handle_cfg(UART_INSTANCE1, uart_irq_handler);
	uart_receive_data(UART_INSTANCE1, buffer_rx, 4);

	//UART0 send data
	uart_gpio_init(UART_INSTANCE0, UART_MUX_SEL1);
	uart_clk_init(UART_INSTANCE0);
	
	uart_init(UART_INSTANCE0, &uart1_init_config);
	uart_interrupt_handle_cfg(UART_INSTANCE0, uart_irq_handler);
	uart_send_data(UART_INSTANCE0, buffer_tx, 4);

	delay_ms(100);
	for (int i = 0U; i < 4; i++)
	{
		if(buffer_rx[i] != 0)
		{
			INFO("%x ", buffer_rx[i]);
			buffer_rx[i] = 0;
		}
	}
	INFO("{test_ok!}");
}


void uart_debug_mode_test(bool debug_en)
{
	UART_TypeDef *base = uart_base[UART_INSTANCE1];
	uart_reset_default_config();
	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	
	uart_init(UART_INSTANCE1, &uart1_init_config);
	base->CTRL.SLEEP_DIS = debug_en;
	uart_interrupt_handle_cfg(UART_INSTANCE1, uart_irq_handler);
	uart_receive_data(UART_INSTANCE1, buffer_rx, 4);

	while(1)
	{
		for (int i = 0U; i < 4; i++)
		{
			if(buffer_rx[i] != 0)
			{
				INFO("%x ", buffer_rx[i]);
				buffer_rx[i] = 0;
			}
		}
	}
	
}

//break send length test
void uart_break_send_test(uint32_t test_case)
{
	uart_reset_default_config();
	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	
	uart_init(UART_INSTANCE1, &uart1_init_config);

	switch(test_case)
	{
		case 1:
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 1);
			for(int j=0;j<5000;j++){}
			uart_break_send(UART_INSTANCE1, UART_BREAK_CHAR_10_BIT_MINIMUM);
			break;
		case 2:
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 1);
			for(int j=0;j<5000;j++){}
			uart_break_send(UART_INSTANCE1, UART_BREAK_CHAR_13_BIT_MINIMUM);
			break;
		case 3:
			uart1_init_config.stop_bit_number = UART_TWO_STOP_BIT;
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 1);
			for(int j=0;j<5000;j++){}
			uart_break_send(UART_INSTANCE1, UART_BREAK_CHAR_10_BIT_MINIMUM);
			break;
		case 4:
			uart1_init_config.stop_bit_number = UART_TWO_STOP_BIT;
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 1);
			for(int j=0;j<5000;j++){}
			uart_break_send(UART_INSTANCE1, UART_BREAK_CHAR_13_BIT_MINIMUM);
			break;
		default:
			break;
	}
}

void uart_idle_send_test(uint32_t idle_send_method)
{
	uart_reset_default_config();
	UART_TypeDef *base = uart_base[UART_INSTANCE1];
	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	
	uart_init(UART_INSTANCE1, &uart1_init_config);

	switch(idle_send_method)
	{
		case 1:
			uart_transmit_enable(UART_INSTANCE1, true);
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 1);
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 1);
			uart_transmit_enable(UART_INSTANCE1, false);
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 1);
			break;
		case 2:
			uart1_init_config.tx_bit_parity = UART_POLARITY_REVERSED;
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 1);
			break;
		case 3:
			uart_transmit_enable(UART_INSTANCE1, true);
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 1);
			base->DATA.w = 0x00002200;
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 1);
			break;
		default:
			break;
	}
}

void uart_idle_detect_test(uint32_t test_case, uint32_t para1, uint32_t para2)
{
	uart_reset_default_config();
	UART_TypeDef *base = uart_base[UART_INSTANCE1];
	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	
	uart_init(UART_INSTANCE1, &uart1_init_config);

	switch(test_case)
	{
		case 1:
			uart_idle_len_set(UART_INSTANCE1, UART_8_IDLE_CHAR, UART_STOP_BIT);
			INFO("STAT:%#x\n", base->STAT.w);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 1);
			delay_ms(1000);
			INFO("receive data:%x\n", buffer_rx[0]);
			INFO("STAT:%#x\n", base->STAT.w);
			break;
		case 5:
			uart_idle_len_set(UART_INSTANCE1, (uart_idle_len_e)para1, (uart_idle_detect_point_e)para2);
			INFO("STAT:%#x\n", base->STAT.w);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 1);
			while(!uart_status_flag_get(UART_INSTANCE1, UART_IDLE_LINE_DETECT))
			{;}
			uart_send_data_polling(UART_INSTANCE1, buffer8, 1);	
			INFO("receive data %x\n", buffer_rx[0]);
			INFO("STAT:%#x\n", base->STAT.w);
		default:
			break;
	}

}

void uart_pin_test(uint32_t instance, uint32_t pin_sel)
{
	uart_reset_default_config();
	uart_reset_rx_buffer();

	uart_gpio_init((uart_instance_e)instance, (uart_mux_sel_e)pin_sel);
	uart_clk_init((uart_instance_e)instance);
	uart_init((uart_instance_e)instance, &uart1_init_config);
	uart_interrupt_handle_cfg((uart_instance_e)instance, uart_irq_handler);
	
	uart_receive_data((uart_instance_e)instance, buffer_rx, rx_size);
	delay_ms(1000);
	uart_send_data_polling((uart_instance_e)instance, buffer_tx, tx_size);
	delay_ms(1000);
	

	for (uint32_t i = 0U; i < rx_size; i++)
	{
		INFO("%#x ", buffer_rx[i]);
	}
	INFO("\n");
	for (uint32_t i = 0U; i < rx_size; i++)
	{
		if(buffer_rx[i] != buffer_tx[i])
		{
			INFO("{test_fail!}");
			return;
		}
	}

	INFO("{test_ok!}");
}


void uart_cts_pin_init(uart_instance_e instance, uint32_t cts_pin_sel)
 {
	if (instance == UART_INSTANCE0)
	{
		switch(cts_pin_sel)
		{
			case 0U:
				pins_mux_drv_init(1U, uart0_cts0_cfg);
				break;
			case 1U:
				pins_mux_drv_init(1U, uart0_cts1_cfg);
				break;
			default:
				INFO("input param error\n");
				break;
		}
	}
	else if (instance == UART_INSTANCE1)
	{
		switch(cts_pin_sel)
		{
			case 0U:
				pins_mux_drv_init(1U, uart1_cts0_cfg);
				break;
			case 1U:
				pins_mux_drv_init(1U, uart1_cts1_cfg);
				break;
			default:
				INFO("input param error\n");
				break;
		}
	}
 }

 void uart_rts_pin_init(uart_instance_e instance, uint32_t rts_pin_sel)
  {
	 if (instance == UART_INSTANCE0)
	 {
		 switch(rts_pin_sel)
		 {
			 case 0U:
				 pins_mux_drv_init(1U, uart0_rts0_cfg);
				 break;
			 case 1U:
				 pins_mux_drv_init(1U, uart0_rts1_cfg);
				 break;
			 default:
				 INFO("input param error\n");
				 break;
		 }
	 }
	 else if (instance == UART_INSTANCE1)
	 {
		 switch(rts_pin_sel)
		 {
			 case 0U:
				 pins_mux_drv_init(1U, uart1_rts0_cfg);
				 break;
			 case 1U:
				 pins_mux_drv_init(1U, uart1_rts1_cfg);
				 break;
			 default:
				 INFO("input param error\n");
				 break;
		 }
	 }
  }



void uart_cts_pin_test(uint32_t instance, uint32_t cts_pin_sel)
{
	uart_reset_default_config();
	uart_clk_init((uart_instance_e)instance);
	uart1_cd_test_gpio_init();
	uart_cts_pin_init((uart_instance_e)instance, cts_pin_sel);
	uart_init((uart_instance_e)instance, &uart1_init_config);
	uart_interrupt_handle_cfg((uart_instance_e)instance, uart_irq_handler);

	uart_tx_cts_set((uart_instance_e)instance, FUNC_ENABLE, UART_CTS_N, UART_START_OF_CHAR);
	uart_send_data((uart_instance_e)instance, buffer_tx, 2);
}

void uart_rts_pin_test(uint32_t instance, uint32_t rts_pin_sel)
{
	uart_reset_default_config();
	uart_clk_init((uart_instance_e)instance);
	uart1_cd_test_gpio_init();
	uart_rts_pin_init((uart_instance_e)instance, rts_pin_sel);
	uart_init((uart_instance_e)instance, &uart1_init_config);
	uart_interrupt_handle_cfg((uart_instance_e)instance, uart_irq_handler);

	uart_rx_rts_set((uart_instance_e)instance, FUNC_ENABLE, 0);
	uart_receive_enable((uart_instance_e)instance, true);
}



void uart_pin_config_clear(uint32_t port, uint32_t pin)
{
	gpio_settings_config_t setting_cfg;

    setting_cfg.gpio_port = (gpio_port_e)port;
    setting_cfg.gpio_pin = (gpio_pin_e)pin;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_1;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_LOW_FAST;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);	
}

static volatile uint32_t g_time = 0U;
uint8_t rts_buffer_rx[12] = {0};
uint8_t rts_buffer_tx[12] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x0D, 0x0A};
static void uart_rx_rts_receive_test_callback(void *driver_state, uart_event_e event, void *user_data)
{
	(void)driver_state;
	(void)user_data;
	
	switch (event)
	{
		case UART_EVENT_RX_FULL:
			g_time++;
			INFO("the %d time receive data finish!\n", g_time);
			for (int i = 0U; i < 12; i++)
			{
				if(rts_buffer_rx[i] != rts_buffer_tx[i])
				{
					for (int j = 0U; j < 12; j++)
					{
						INFO("%#x ", rts_buffer_rx[j]);
					}
					INFO("the %d time receive data error!\n", g_time);
					return;
				}
				rts_buffer_rx[i] = 0U;
			}
			(void)uart_rx_buffer_set(UART_INSTANCE1, rts_buffer_rx, 12);
			break;
		case UART_EVENT_END_TRANSFER:
			(void)INFO("uart receive data over!\n");
			break;
		case UART_EVENT_ERROR:
			(void)INFO("uart receive data fail!\n");
			break;
		default:
			break;
	}  
}


void uart_rx_rts_receive_test(uint32_t test_case)
{
	uart_reset_default_config();
	uart_reset_rx_buffer();

	uart1_init_config.baud_rate = 2000000;

	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	
	uart_init(UART_INSTANCE1, &uart1_init_config);
	uart_interrupt_handle_cfg(UART_INSTANCE1, uart_irq_handler);
	uart_rx_callback_install(UART_INSTANCE1, uart_rx_rts_receive_test_callback, NULL);
	
	gpio_settings_config_t gpio_settings_config;
	gpio_settings_config.gpio_port = GPIO_C;
	gpio_settings_config.gpio_pin = GPIO_PIN_13;
	gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_4;
	gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
	gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
	gpio_settings_config.gpio_strength = GPIO_STRENGTH_LOW_FAST;
	gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&gpio_settings_config);

	switch(test_case)
	{
		case 1:
			uart_rx_rts_set(UART_INSTANCE1, FUNC_ENABLE, 0);
			uart_receive_data(UART_INSTANCE1, rts_buffer_rx, 12);
			break;
		case 2:
			uart_rx_rts_set(UART_INSTANCE1, FUNC_ENABLE, 2);
			uart_rx_fifo_enable(UART_INSTANCE1, FUNC_ENABLE, 1, UART_2_WORD_SET_FULL);
			uart_receive_data(UART_INSTANCE1, rts_buffer_rx, 12);
			break;
		default:
			break;
	}
}

//***********************************************************************************************
//AUTO TEST
void uart_common_cd_test(uint32_t baudrate,
						uint32_t parity_mode,
						uint32_t stop_bit_number,
						uint32_t bit_number_per_char,
						uint32_t tx_bit_parity,
						uint32_t rx_bit_parity,
						uint32_t bit_order)
{
	uart_reset_default_config();
	uart_reset_rx_buffer();
	uart1_init_config.baud_rate = baudrate;
	uart1_init_config.parity_mode = (uart_parity_mode_e)parity_mode;
	uart1_init_config.stop_bit_number = (uart_stop_bit_count_e)stop_bit_number;
	uart1_init_config.bit_number_per_char = (uart_bit_number_per_char_e)bit_number_per_char;
	uart1_init_config.tx_bit_parity = (uart_tx_rx_polarity_e)tx_bit_parity;
	uart1_init_config.rx_bit_parity = (uart_tx_rx_polarity_e)rx_bit_parity;
	uart1_init_config.bit_order = (uart_bit_order_e)bit_order;

	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	
	uart_init(UART_INSTANCE1, &uart1_init_config);
	uart_interrupt_handle_cfg(UART_INSTANCE1, uart_irq_handler);
	uart_receive_data_polling(UART_INSTANCE1, buffer_rx, rx_size);

	for (int i = 0U; i < rx_size; i++)
	{
		INFO("%#x ", buffer_rx[i]);
	}
	INFO("\n");
	for (int i = 0U; i < rx_size; i++)
	{
		if(buffer_rx[i] != buffer_tx[i])
		{
			INFO("{test_fail!}");
			return;
		}
	}

	INFO("{test_ok!}");
}

void uart_common_send_cd_test(uint32_t baudrate,
						uint32_t parity_mode,
						uint32_t stop_bit_number,
						uint32_t bit_number_per_char,
						uint32_t tx_bit_parity,
						uint32_t rx_bit_parity,
						uint32_t bit_order)
{
	uart_reset_default_config();
	uart_reset_rx_buffer();
	uart1_init_config.baud_rate = baudrate;
	uart1_init_config.parity_mode = (uart_parity_mode_e)parity_mode;
	uart1_init_config.stop_bit_number = (uart_stop_bit_count_e)stop_bit_number;
	uart1_init_config.bit_number_per_char = (uart_bit_number_per_char_e)bit_number_per_char;
	uart1_init_config.tx_bit_parity = (uart_tx_rx_polarity_e)tx_bit_parity;
	uart1_init_config.rx_bit_parity = (uart_tx_rx_polarity_e)rx_bit_parity;
	uart1_init_config.bit_order = (uart_bit_order_e)bit_order;

	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
		
	uart_init(UART_INSTANCE1, &uart1_init_config);
	uart_interrupt_handle_cfg(UART_INSTANCE1, uart_irq_handler);
	uart_send_data_polling(UART_INSTANCE1, buffer_tx, tx_size);
}
				
// wakeup test
void uart_wakeup_cd_test(uint32_t wakeup_method)
{
	uart_reset_default_config();
	uart_reset_rx_buffer();
	UART_TypeDef *base = uart_base[UART_INSTANCE1];
	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	
	uart_init(UART_INSTANCE1, &uart1_init_config);
	
	switch(wakeup_method)
	{
		case 1:
			//idle wakeup test. 
			uart_wakeup_cfg(UART_INSTANCE1, UART_IDLE_WAKE_UP_WITH_IDLE_FLAG);
			uart_standby_mode_enter(UART_INSTANCE1, true);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 1);
			if ((buffer_rx[0] == buffer_tx[1]) && (base->CTRL.STANDBY == 0))
			{
				INFO("{test_ok!}");
			}
			else
			{
				INFO("{test_fail!}");
			}
			break;
		case 2:
			//bit mark wakeup test.
			uart_wakeup_cfg(UART_INSTANCE1, UART_BIT_MARK_WAKE_UP);
			uart_standby_mode_enter(UART_INSTANCE1, true);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 1);
			if ((buffer_rx[0] == buffer_tx[0]) && (base->CTRL.STANDBY == 0))
			{
				INFO("{test_ok!}");
			}
			else
			{
				INFO("{test_fail!}");
			}
			break;
		case 3:
			//match wakeup test.
			uart_wakeup_cfg(UART_INSTANCE1, UART_MATCH_WAKE_UP);
			uart_match1_set(UART_INSTANCE1, true, buffer_tx[1]);
			uart_standby_mode_enter(UART_INSTANCE1, true);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 1);
			if ((buffer_rx[0] == buffer_tx[0]) && (base->CTRL.STANDBY == 0))
			{
				INFO("{test_ok!}");
			}
			else
			{
				INFO("{test_fail!}");
			}
			break;
		default:
			break;
	}
}

void uart_match_cd_test(uint32_t match_method)
{
	uart_reset_default_config();
	uart_reset_rx_buffer();
	UART_TypeDef *base = uart_base[UART_INSTANCE1];
	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	
	uart_init(UART_INSTANCE1, &uart1_init_config);

	switch(match_method)
	{
		case 1:
			//idle address match
			uart_match_type_set(UART_INSTANCE1, UART_IDLE_ADDR_MATCH);
			uart_match1_set(UART_INSTANCE1, true, buffer_tx[1]);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 2);
			if ((buffer_rx[0] == buffer_tx[1]) && (buffer_rx[1] == buffer_tx[2]) && (base->STAT.MATCH1_FLAG == 1))
			{
				INFO("{test_ok!}");
			}
			else
			{
				INFO("{test_fail!}");
			}
			break;
		case 2:
			//bit mark match.
			uart_match_type_set(UART_INSTANCE1, UART_BIT_MARK_ADDR_MATCH);
			uart_match1_set(UART_INSTANCE1, true, buffer_tx[2]);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 2);
			if ((buffer_rx[0] == buffer_tx[2]) && (buffer_rx[1] == buffer_tx[3]) && (base->STAT.MATCH1_FLAG == 1))
			{
				INFO("{test_ok!}");
			}
			else
			{
				INFO("{test_fail!}");
			}
			break;
		case 3:
			//match on and off.
			uart_match_type_set(UART_INSTANCE1, UART_ADDR_MATCH_ON_OFF);
			uart_match1_set(UART_INSTANCE1, true, buffer_tx[0]);
			uart_match2_set(UART_INSTANCE1, true, buffer_tx[2]);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 4);
			if ((buffer_rx[0] == buffer_tx[0]) && (buffer_rx[1] == buffer_tx[1]) &&
				(buffer_rx[2] == buffer_tx[0]) && (buffer_rx[3] == buffer_tx[1]) && (base->STAT.MATCH1_FLAG == 1))
			{
				INFO("{test_ok!}");
			}
			else
			{
				INFO("{test_fail!}");
			}
			break;
		default:
			break;
	}
}

void uart_single_wire_loop_mode_cd_test(uint32_t mode)
{
	uart_reset_default_config();
	uart_reset_rx_buffer();
	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	
	uart_init(UART_INSTANCE1, &uart1_init_config);
	uart_interrupt_handle_cfg(UART_INSTANCE1, uart_irq_handler);

	switch(mode)
	{
		case 1:
			//loop mode test.
			uart_work_mode_cfg(UART_INSTANCE1, UART_LOOP_MODE, UART_TXD_AS_INPUT);
			uart_receive_data(UART_INSTANCE1, buffer_rx, 2);
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 2);
			delay_ms(100);
			if ((buffer_rx[0] == buffer_tx[0]) && (buffer_rx[1] == buffer_tx[1]))
			{
				INFO("{test_ok!}");
			}
			else
			{
				INFO("{test_fail!}");
			}
			break;
		case 2:
			//single wire test1
			uart_work_mode_cfg(UART_INSTANCE1, UART_SINGLE_WIRE_MODE, UART_TXD_AS_INPUT);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 2);
			if ((buffer_rx[0] == buffer_tx[0]) && (buffer_rx[1] == buffer_tx[1]))
			{
				INFO("{test_ok!}");
			}
			else
			{
				INFO("{test_fail!}");
			}
			break;
		case 3:
			//single wire mode test2.
			gpio_settings_config_t setting_cfg;
		    //TX PTC7
		    setting_cfg.gpio_port = GPIO_C;
		    setting_cfg.gpio_pin = GPIO_PIN_7;
		    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
		    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
		    setting_cfg.pull_cfg = GPIO_PULL_UP;
		    setting_cfg.gpio_strength = GPIO_STRENGTH_LOW_FAST;
		    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
		    gpio_init(&setting_cfg);
			uart_work_mode_cfg(UART_INSTANCE1, UART_SINGLE_WIRE_MODE, UART_TXD_AS_OUTPUT);
			uart_send_data(UART_INSTANCE1, buffer_tx, 2);
			break;
		default:
			break;
	}
}

void uart_infrared_mode_cd_test(uint32_t test_case)
{
	uart_reset_default_config();
	uart_reset_rx_buffer();
	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	
	uart_init(UART_INSTANCE1, &uart1_init_config);
	uart_interrupt_handle_cfg(UART_INSTANCE1, uart_irq_handler);
	uart_irda_enable(UART_INSTANCE1, true, UART_1_DIVIDE_OSR_BIT);

	switch(test_case)
	{
		case 1:
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 2);
			if ((buffer_rx[0] == buffer_tx[0]) && (buffer_rx[1] == buffer_tx[1]))
			{
				INFO("{test_ok!}");
			}
			else
			{
				INFO("{test_fail!}");
			}
			break;
		case 2:
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 2);
			break;
		default:
			break;
	}
}


void uart_autobaud_cd_test()
{
	uart_reset_default_config();
	uart_reset_rx_buffer();
	uart1_init_config.autobaud_en = FUNC_ENABLE;
	
	UART_TypeDef *base = uart_base[UART_INSTANCE1];
	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	
	uart_init(UART_INSTANCE1, &uart1_init_config);
	
	uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 2);
	
	if ((buffer_rx[0] == buffer_tx[0]) && (buffer_rx[1] == buffer_tx[1]) && base->AUTOBAUD.AUTO_SUCC_FLAG == 1)
	{
		INFO("{test_ok!}");
	}
	else
	{
		INFO("{test_fail!}");
	}
}


void uart_re_autobaud_cd_test()
{
	uart_reset_default_config();
	uart_reset_rx_buffer();
	uart1_init_config.autobaud_en = FUNC_ENABLE;

	UART_TypeDef *base = uart_base[UART_INSTANCE1];
	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	
	uart_init(UART_INSTANCE1, &uart1_init_config);
	
	//the first autobaud
	uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 2);
	if ((buffer_rx[0] == buffer_tx[0]) && (buffer_rx[1] == buffer_tx[1]) && base->AUTOBAUD.AUTO_SUCC_FLAG == 1)
	{
		//INFO("{test_ok!}");
	}
	else
	{
		INFO("{test_fail!}");
		return;
	}

	//clear autobaud result
	uart_autobaud_result_clear(UART_INSTANCE1);
	//the second autobaud
	uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 2);
	if ((buffer_rx[0] == buffer_tx[2]) && (buffer_rx[1] == buffer_tx[3]) && base->AUTOBAUD.AUTO_SUCC_FLAG == 1)
	{
		INFO("{test_ok!}");
	}
	else
	{
		INFO("{test_fail!}");
	}
}

void uart_interrupt_cd_test_rx_callback(void *driverState, uart_event_e event, void *userData)
{
	if (event == UART_EVENT_ERROR)
	{
		if (uart_status_flag_get(UART_INSTANCE1, UART_FRAME_ERR) == true)
		{
			INFO("{frame_error_test_ok!}");
		}
		else if (uart_status_flag_get(UART_INSTANCE1, UART_PARITY_ERR) == true)
		{
			INFO("{parity_error_test_ok!}");
		}
		else if (uart_status_flag_get(UART_INSTANCE1, UART_RX_OVERRUN) == true)
		{
			INFO("{rx_overrun_test_ok!}");
		}
		else if (uart_status_flag_get(UART_INSTANCE1, UART_NOISE_DETECT) == true)
		{
			INFO("{rx_noise_detect_test_ok!}");
		}
	}
	else if (event == UART_EVENT_MATCH_ADDR1)
	{
		INFO("{rx_match1_test_ok!}");
	}
	else if (event == UART_EVENT_MATCH_ADDR2)
	{
		INFO("{rx_match2_test_ok!}");
	}
	else if (event == UART_EVENT_BREAK_DETEC)
	{
		INFO("{rx_break_detect_test_ok!}");
	}
	else if (event == UART_EVENT_ACTIVE_EDG)
	{
		INFO("{rx_edg_detect_test_ok!}");
	}
	else if (event == UART_EVENT_IDLE_DETECT)
	{
		INFO("{rx_idle_detect_test_ok!}");
	}
	else if (event == UART_EVENT_UNDER_FLOW)
	{
		INFO("{rx_underflow_test_ok!}");
	}
	else if (event == UART_EVENT_AUTOBAUD)
	{
		INFO("{rx_autobaud_success_test_ok!}");
	}
}

void uart_interrupt_cd_test_tx_callback(void *driverState, uart_event_e event, void *userData)
{
	if (event == UART_EVENT_OVER_FLOW)
	{
		INFO("{tx_overflow_test_ok!}");
	}
}

void uart_interrupt_cd_test(uint32_t test_case)
{
	uart_reset_default_config();
	uart_reset_rx_buffer();
	UART_TypeDef *base = uart_base[UART_INSTANCE1];

	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	uart_interrupt_handle_cfg(UART_INSTANCE1, uart_irq_handler);		

	uart_de_init(UART_INSTANCE1);
	
	switch(test_case)
	{
		case 1:
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_rx_callback_install(UART_INSTANCE1, uart_interrupt_cd_test_rx_callback, NULL);
			uart_receive_data(UART_INSTANCE1, buffer_rx, 4);
			break;
		case 2:
			uart1_init_config.parity_mode = UART_PARITY_EVEN;
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_rx_callback_install(UART_INSTANCE1, uart_interrupt_cd_test_rx_callback, NULL);
			uart_receive_data(UART_INSTANCE1, buffer_rx, 4);
			break;
		case 3:
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_rx_callback_install(UART_INSTANCE1, uart_interrupt_cd_test_rx_callback, NULL);
			uart_receive_data(UART_INSTANCE1, buffer_rx, 4);
			uart_interrupt_source_enable(UART_INSTANCE1, UART_INT_RX_DATA_REG_FULL, false);
			break;
		case 4:
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_rx_callback_install(UART_INSTANCE1, uart_interrupt_cd_test_rx_callback, NULL);
			uart_match1_set(UART_INSTANCE1, false, 0x11);
			uart_receive_data(UART_INSTANCE1, buffer_rx, 1);
			uart_interrupt_source_enable(UART_INSTANCE1, UART_INT_MATCH_ADDR_ONE, true);
			break;
		case 5:
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_tx_callback_install(UART_INSTANCE1, uart_interrupt_cd_test_tx_callback, NULL);
			uart_interrupt_source_enable(UART_INSTANCE1, UART_INT_FIFO_TXOF, true);
			uart_tx_fifo_enable(UART_INSTANCE1, FUNC_ENABLE, 3);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 1);
			uart_send_data(UART_INSTANCE1, buffer_tx, 5);
			base->DATA.w = 0x01;
			base->DATA.w = 0x02;
			base->DATA.w = 0x03;
			break;
		case 6:
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_rx_callback_install(UART_INSTANCE1, uart_interrupt_cd_test_rx_callback, NULL);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 1);
			uart_receive_data(UART_INSTANCE1, buffer_rx, 4);
			uart_interrupt_source_enable(UART_INSTANCE1, UART_INT_FIFO_RXUF, true);
			uart_rx_fifo_enable(UART_INSTANCE1, FUNC_ENABLE, 2, UART_2_WORD_SET_FULL);
			INFO("%x",base->DATA.w);
			break;
		case 7:
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 1);
			uart_rx_callback_install(UART_INSTANCE1, uart_interrupt_cd_test_rx_callback, NULL);
			uart_break_detect_enable(UART_INSTANCE1, true);
			uart_receive_data(UART_INSTANCE1, buffer_rx, 4);
			uart_interrupt_source_enable(UART_INSTANCE1, UART_INT_LIN_BREAK_DETECT, true);
			break;
		case 8:
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_rx_callback_install(UART_INSTANCE1, uart_interrupt_cd_test_rx_callback, NULL);
			uart_idle_len_set(UART_INSTANCE1, UART_2_IDLE_CHAR, UART_START_BIT);
			uart_receive_data(UART_INSTANCE1, buffer_rx, 4);
			uart_interrupt_source_enable(UART_INSTANCE1, UART_INT_IDLE_LINE, true);
			break;
		case 9:
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_rx_callback_install(UART_INSTANCE1, uart_interrupt_cd_test_rx_callback, NULL);
			uart_interrupt_source_enable(UART_INSTANCE1, UART_INT_RX_ACTIVE_EDGE, true);
			uart_receive_data(UART_INSTANCE1, buffer_rx, 4);
			break;
		case 10:
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_rx_callback_install(UART_INSTANCE1, uart_interrupt_cd_test_rx_callback, NULL);
			uart_receive_data(UART_INSTANCE1, buffer_rx, 4);
			break;
		case 11:
			uart1_init_config.autobaud_en = FUNC_ENABLE;
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_rx_callback_install(UART_INSTANCE1, uart_interrupt_cd_test_rx_callback, NULL);
			uart_interrupt_source_enable(UART_INSTANCE1, UART_INT_AUTO_BAUD_SUCC, true);
			uart_receive_data(UART_INSTANCE1, buffer_rx, 5);
			break;
		default:
			break;
	}
}


void uart_cts_mode_cd_test(uint32_t cts_mode)
{
	if (cts_mode == 3)
	{
		uart_send_data(UART_INSTANCE1, buffer_tx, 2);
		return;
	}
	
	uart_reset_default_config();
	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	
	uart_init(UART_INSTANCE1, &uart1_init_config);
	uart_interrupt_handle_cfg(UART_INSTANCE1, uart_irq_handler);

	gpio_settings_config_t setting_cfg;
	setting_cfg.gpio_port = GPIO_E;
    setting_cfg.gpio_pin = GPIO_PIN_2;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_6;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_LOW_FAST;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

	switch(cts_mode)
	{
		case 1:
			uart_tx_cts_set(UART_INSTANCE1, FUNC_ENABLE, UART_CTS_N, UART_START_OF_CHAR);
			uart_send_data(UART_INSTANCE1, buffer_tx, 2);
			break;
		case 2:
			uart_tx_cts_set(UART_INSTANCE1, FUNC_ENABLE, UART_RX_MATCH, UART_START_OF_CHAR);
			uart_match1_set(UART_INSTANCE1, true, 0x88);
			uart_receive_data(UART_INSTANCE1, buffer_rx, 8);
			break;
		default:
			break;
	}
}


void uart_rts_cd_test(uint32_t test_case, uint32_t para1)
{
	uart_reset_default_config();
	uart_reset_rx_buffer();
	UART_TypeDef *base = uart_base[UART_INSTANCE1];

	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	
	uart_init(UART_INSTANCE1, &uart1_init_config);
	uart_interrupt_handle_cfg(UART_INSTANCE1, uart_irq_handler);
	
	gpio_settings_config_t gpio_settings_config;
	gpio_settings_config.gpio_port = GPIO_A;
	gpio_settings_config.gpio_pin = GPIO_PIN_7;
	gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_6;
	gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
	gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
	gpio_settings_config.gpio_strength = GPIO_STRENGTH_LOW_FAST;
	gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&gpio_settings_config);

	switch(test_case)
	{
		case 1:
			uart_rx_rts_set(UART_INSTANCE1, FUNC_ENABLE, 0);
			uart_receive_enable(UART_INSTANCE1, true);
			break;
		case 2:
			uart_rx_rts_set(UART_INSTANCE1, FUNC_ENABLE, 2);
			uart_rx_fifo_enable(UART_INSTANCE1, FUNC_ENABLE, 1, UART_2_WORD_SET_FULL);
			uart_receive_enable(UART_INSTANCE1, true);
			break;
		case 3:
			uart_tx_rts_set(UART_INSTANCE1, FUNC_ENABLE, (uart_tx_rts_polarity_e)para1);
			uart_transmit_enable(UART_INSTANCE1, true);
			base->DATA.DATA = 0x01;
			base->DATA.DATA = 0x02;
			base->DATA.DATA = 0x03;
			base->DATA.DATA = 0x04;
			break;
		case 4:
			uart_tx_rts_set(UART_INSTANCE1, FUNC_ENABLE, (uart_tx_rts_polarity_e)para1);
			uart_tx_fifo_enable(UART_INSTANCE1, FUNC_ENABLE, 2);
			uart_transmit_enable(UART_INSTANCE1, true);
			base->DATA.DATA = 0x01;
			base->DATA.DATA = 0x02;
			base->DATA.DATA = 0x03;
			base->DATA.DATA = 0x04;
			break;
		default:
			break;
	}
}

void uart_break_cd_test_rx_callback(void *driverState, uart_event_e event, void *userData)
{
	if (event == UART_EVENT_BREAK_DETEC)
	{
		INFO("{rx_break_detect_test_ok!}");
	}
}


void uart_break_receive_cd_test(bool break_detect_en, uint8_t break_send_length)
{
	uart_reset_default_config();
	uart_reset_rx_buffer();

	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	uart_init(UART_INSTANCE1, &uart1_init_config);
	uart_rx_callback_install(UART_INSTANCE1, uart_break_cd_test_rx_callback, NULL);
	uart_interrupt_handle_cfg(UART_INSTANCE1, uart_irq_handler);
	uart_break_detect_enable(UART_INSTANCE1, break_detect_en);
	uart_interrupt_source_enable(UART_INSTANCE1, UART_INT_LIN_BREAK_DETECT, true);
	uart_receive_data(UART_INSTANCE1, buffer_rx, 4);
}

void uart_break_send_cd_test(uint32_t test_case)
{
	uart_reset_default_config();
	uart_reset_rx_buffer();
	UART_TypeDef *base = uart_base[UART_INSTANCE1];

	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	uart1_init_config.bit_number_per_char = UART_9_BITS_PER_CHAR;
	uart_init(UART_INSTANCE1, &uart1_init_config);

	switch(test_case)
	{
		case 1:
			uart_break_send(UART_INSTANCE1, UART_BREAK_CHAR_10_BIT_MINIMUM);
			break;
		case 2:
			uart_transmit_enable(UART_INSTANCE1, true);
			base->DATA.w = 0x00002000;	
			break;
		default:
			break;
	}
}

void uart_bit_len_send_cd_test(uint32_t test_case)
{
	uart_reset_default_config();
	uart_reset_rx_buffer();

	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	
	switch(test_case)
	{
		case 1:
			uart1_init_config.bit_number_per_char = UART_7_BITS_PER_CHAR;
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 2);
			break;
		case 2:
			uart1_init_config.bit_number_per_char = UART_8_BITS_PER_CHAR;
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 2);
			break;
		case 3:
			uart1_init_config.bit_number_per_char = UART_7_BITS_PER_CHAR;
			uart1_init_config.parity_mode = UART_PARITY_EVEN;
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 2);
			break;
		case 4:
			uart1_init_config.bit_number_per_char = UART_9_BITS_PER_CHAR;
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 2);
			break;
		case 5:
			uart1_init_config.bit_number_per_char = UART_8_BITS_PER_CHAR;
			uart1_init_config.parity_mode = UART_PARITY_EVEN;
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 2);
			break;
		case 6:
			uart1_init_config.bit_number_per_char = UART_10_BITS_PER_CHAR;
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 2);
			break;
		case 7:
			uart1_init_config.bit_number_per_char = UART_9_BITS_PER_CHAR;
			uart1_init_config.parity_mode = UART_PARITY_EVEN;
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 2);
			break;
		default:
			break;
	}
}

void uart_bit_len_receive_cd_test(uint32_t test_case)
{
	uart_reset_default_config();
	uart_reset_rx_buffer();

	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	
	switch(test_case)
	{
		case 1:
			uart1_init_config.bit_number_per_char = UART_7_BITS_PER_CHAR;
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 2);
			if ((buffer_rx[0] == 0x41) && (buffer_rx[1] == 0x03))
			{
				INFO("{test_ok!}");
			}
			else
			{
				INFO("{test_fail!}");
			}
			break;
		case 2:
			uart1_init_config.bit_number_per_char = UART_8_BITS_PER_CHAR;
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 2);
			if ((buffer_rx[0] == 0xc1) && (buffer_rx[1] == 0x03))
			{
				INFO("{test_ok!}");
			}
			else
			{
				INFO("{test_fail!}");
			}
			break;
		case 3:
			uart1_init_config.bit_number_per_char = UART_7_BITS_PER_CHAR;
			uart1_init_config.parity_mode = UART_PARITY_EVEN;
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 2);
			if ((buffer_rx[0] == 0x41) && (buffer_rx[1] == 0x03))
			{
				INFO("{test_ok!}");
			}
			else
			{
				INFO("{test_fail!}");
			}
			break;
		case 4:
			uart1_init_config.bit_number_per_char = UART_9_BITS_PER_CHAR;
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 2);
			if ((buffer_rx[0] == 0xc1) && (buffer_rx[1] == 0x01))
			{
				INFO("{test_ok!}");
			}
			else
			{
				INFO("{test_fail!}");
			}
			break;
		case 5:
			uart1_init_config.bit_number_per_char = UART_8_BITS_PER_CHAR;
			uart1_init_config.parity_mode = UART_PARITY_EVEN;
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 2);
			if ((buffer_rx[0] == 0xc1) && (buffer_rx[1] == 0x03))
			{
				INFO("{test_ok!}");
			}
			else
			{
				INFO("{test_fail!}");
			}
			break;
		case 6:
			uart1_init_config.bit_number_per_char = UART_10_BITS_PER_CHAR;
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 2);
			if ((buffer_rx[0] == 0xc1) && (buffer_rx[1] == 0x03))
			{
				INFO("{test_ok!}");
			}
			else
			{
				INFO("{test_fail!}");
			}
			break;
		case 7:
			uart1_init_config.bit_number_per_char = UART_9_BITS_PER_CHAR;
			uart1_init_config.parity_mode = UART_PARITY_EVEN;
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 2);
			if ((buffer_rx[0] == 0xc1) && (buffer_rx[1] == 0x01))
			{
				INFO("{test_ok!}");
			}
			else
			{
				INFO("{test_fail!}");
			}
			break;
		default:
			break;
	}
}

void uart_extreme_speed_cd_test(uint32_t test_case)
{
	uart_reset_default_config();
	uart_reset_rx_buffer();
	UART_TypeDef *base = uart_base[UART_INSTANCE1];

	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	uart_init(UART_INSTANCE1, &uart1_init_config);
	uart_interrupt_handle_cfg(UART_INSTANCE1, uart_irq_handler);

	switch(test_case)
	{
		case 1:
			base->BAUD.OSR = 11;
			base->BAUD.BAUD_DIV_INT = 1;
			base->AUTOBAUD.BAUD_DIV_FRA = 0;
			uart_rx_fifo_enable(UART_INSTANCE1, FUNC_ENABLE, 0, UART_1_WORD_SET_FULL);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 4);

			for (int i = 0U; i < 4; i++)
			{
				INFO("%#x ", buffer_rx[i]);
			}
			INFO("\n");
			for (int i = 0U; i < 4; i++)
			{
				if(buffer_rx[i] != buffer_tx[i])
				{
					INFO("{test_fail!}");
					return;
				}
			}
			INFO("{test_ok!}");
			break;
		case 2:
			base->BAUD.OSR = 31;
			base->BAUD.BAUD_DIV_INT = 8191;
			base->AUTOBAUD.BAUD_DIV_FRA = 0;
			uart_rx_fifo_enable(UART_INSTANCE1, FUNC_ENABLE, 0, UART_1_WORD_SET_FULL);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, rx_size);
			for (int i = 0U; i < rx_size; i++)
			{
				if(buffer_rx[i] != buffer_tx[i])
				{
					INFO("{test_fail!}");
					return;
				}
			}
			INFO("{test_ok!}");
			break;
		case 3:
			base->BAUD.OSR = 11;
			base->BAUD.BAUD_DIV_INT = 1;
			base->AUTOBAUD.BAUD_DIV_FRA = 0;
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 4);
			break;
		case 4:
			base->BAUD.OSR = 31;
			base->BAUD.BAUD_DIV_INT = 8191;
			base->AUTOBAUD.BAUD_DIV_FRA = 0;
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, tx_size);
			break;
		default:
			break;
	}
}

void uart_fifo_cd_test(uint32_t test_case)
{
	uart_reset_default_config();
	uart_reset_rx_buffer();
	UART_TypeDef *base = uart_base[UART_INSTANCE1];

	uart1_cd_test_gpio_init();
	uart_clk_init(UART_INSTANCE1);
	uart_init(UART_INSTANCE1, &uart1_init_config);
	uart_interrupt_handle_cfg(UART_INSTANCE1, uart_irq_handler);

	switch(test_case)
	{
		case 1:
			uart_transmit_enable(UART_INSTANCE1, true);
			uart_tx_fifo_enable(UART_INSTANCE1, FUNC_ENABLE, 2);
			base->DATA.DATA = buffer_tx[0];
			base->DATA.DATA = buffer_tx[1];
			base->DATA.DATA = buffer_tx[2];
			base->DATA.DATA = buffer_tx[3];
			base->DATA.DATA = buffer_tx[4];
			base->DATA.DATA = buffer_tx[5];
			break;
		case 2:
			uart_receive_enable(UART_INSTANCE1, true);
			uart_rx_fifo_enable(UART_INSTANCE1, FUNC_ENABLE, 2, UART_1_WORD_SET_FULL);
			while(base->WATERMARK.RXSIZE != 4)
			{
				__NOP();
			}
			 
			for (int i = 0U; i < 4; i++)
			{
				if(base->DATA.DATA != buffer_tx[i])
				{
					INFO("{test_fail!}");
					return;
				}
			}
			INFO("{test_ok!}");
			break;
		case 3:
			uart_rx_fifo_enable(UART_INSTANCE1, FUNC_ENABLE, 2, UART_2_WORD_SET_FULL);
			uart_receive_data_polling(UART_INSTANCE1, buffer_rx, 1);
			if(buffer_rx[0] != buffer_tx[0])
			{
				INFO("{test_fail!}");
				return;
			}
			INFO("{test_ok!}");
			break;
		default:
			break;
	}
}

uint8_t AHB_SRAM_DATA_ATTRIBUTES dma_cd_send_data[4] = {0x55, 0x55, 0x55, 0x55};
uint8_t AHB_SRAM_DATA_ATTRIBUTES dma_cd_receive_data[4] = {0};
void uart_deepsleep_cd_test(uint32_t test_case, bool pclk_gate, bool fclk_gate, bool doze)
{
	if (test_case == 5)
	{
		for (uint8_t i = 0; i < 4; i++)
		{
			if (dma_cd_receive_data[i] != buffer_tx[i])
			{
				INFO("{test_fail!}");
				return;
			}
		}
		INFO("{test_ok!}");
		return;
	}

	module_clk_cfg_t module_clk_cfg;
	module_clk_cfg.module_name = MODULE_UART1;
	module_clk_cfg.fclk_sel = APB_CLK;
	module_clk_cfg.fclk_div = PER_DIVIDE_1;
	module_clk_cfg.pclk_dsleep_gating_en = (clk_dsleep_gating_en_e)pclk_gate;
	module_clk_cfg.fclk_dsleep_gating_en = (clk_dsleep_gating_en_e)fclk_gate;
	clock_module_clk_cfg(&module_clk_cfg);


	uart_reset_default_config();
	uart1_cd_test_gpio_init();
	

	switch(test_case)
	{
		case 1:
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_doze_enable(UART_INSTANCE1, doze);
			uart_interrupt_handle_cfg(UART_INSTANCE1, uart_irq_handler);
			uart_send_data_polling(UART_INSTANCE1, buffer_tx, 2);
			break;
		case 2:
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_doze_enable(UART_INSTANCE1, doze);
			uart_interrupt_handle_cfg(UART_INSTANCE1, uart_irq_handler);
			uart_receive_data_polling(UART_INSTANCE1, dma_cd_receive_data, 4);
			break;
		case 3:
			uart_init(UART_INSTANCE1, &uart1_init_config);
			uart_doze_enable(UART_INSTANCE1, doze);
			uart_interrupt_handle_cfg(UART_INSTANCE1, uart_irq_handler);
			uart_receive_data_polling(UART_INSTANCE1, dma_cd_receive_data, 4);
			break;
		default:
			break;
	}
}


//possible need mannual test, for the high level for trugger is too short
void uart_trig_mux_cd_test(uint32_t instance, uint32_t trig_num)
{
	uart_reset_default_config();
	
	//uart_gpio_init((uart_instance_e)instance, UART_MUX_SEL1);
	uart1_cd_test_gpio_init();
	uart_clk_init((uart_instance_e)instance);
	uart_init((uart_instance_e)instance, &uart1_init_config);
	uart_tx_cts_set((uart_instance_e)instance, FUNC_ENABLE, UART_TRIG_INPUT, UART_BUS_IDLE);
	uart_interrupt_handle_cfg((uart_instance_e)instance, uart_irq_handler);
	uart_send_data((uart_instance_e)instance, buffer_tx, 4);
	delay_ms(100);

	trgmux_uart_test((trgmux_uart_channel_e)(TRGMUX_UART0_TRIGGER_INPUT + instance), (peripheral_trgmux_src_e)trig_num);

	INFO("test over!\n");
	delay_ms(100);
	rstgen_reset_ctrl(RSTGEN_GLOBAL_RESET, RSTGEN_RST_ENABLE);
}
#endif



