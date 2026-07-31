#include "platform_common.h"

#include "test.h"

#include "gpio_hal.h"
#include "uart_hal.h"

#include "spi_reg.h"
#include "spi_hal.h"
#include "spi_test.h"
#include "cmdline_internal.h"
#include "spi_type.h"








void spi_func1_test(uint32_t test_case, uint32_t para1, uint32_t para2)
{




}	



void spi_func2_test(uint32_t test_case, uint32_t para1, uint32_t para2)
{




}	


#ifdef SPI_TEST

void spi0_gpio_init()
{
#if 1
    gpio_settings_config_t setting_cfg;

	/* SCLK, PTC0 J2_B8  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_9;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_5;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    setting_cfg.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_EITHER_EDGE;
    setting_cfg.gpio_interrupt_cfg_info.priority = 1;
    gpio_init(&setting_cfg);

    /* MISO, PTC1 J2_B7  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_10;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_5;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    setting_cfg.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_EITHER_EDGE;
    setting_cfg.gpio_interrupt_cfg_info.priority = 1;
    gpio_init(&setting_cfg);

    /* MOSI, PTC2 J2_B12  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_11;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_5;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    setting_cfg.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_EITHER_EDGE;
    setting_cfg.gpio_interrupt_cfg_info.priority = 1;
    gpio_init(&setting_cfg);

	/* CSN, PTC3 J3_B79  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_8;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_5;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    setting_cfg.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_EITHER_EDGE;
    setting_cfg.gpio_interrupt_cfg_info.priority = 1;
    gpio_init(&setting_cfg);

#endif
}

transmit_data send_data_test_buffer = {0};
transmit_data recv_data_test_buffer = {0};


//#include "platform_common.h"

//#include "test.h"

//#include "gpio_hal.h"
//#include "spi_hal.h"
//#include "spi_test.h"
//#include "spi_reg.h"
//#include "spi.h"
//#include "uart_hal.h"
//#include "rstgen_hal.h"
//#include "trgmux_hal.h"
//#include "trgmux_test.h"

static gpio_settings_config_t spi0_arr0_cfg[7U] = {
//    {
//        .gpio_port            = GPIO_D,
//        .gpio_pin             = GPIO_PIN_15,
//        .gpio_alt             = GPIO_ALT_FUNC_4,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_B,
//        .gpio_pin             = GPIO_PIN_4,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_D,
//        .gpio_pin             = GPIO_PIN_16,
//        .gpio_alt             = GPIO_ALT_FUNC_4,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_B,
//        .gpio_pin             = GPIO_PIN_5,
//        .gpio_alt             = GPIO_ALT_FUNC_4,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_B,
//        .gpio_pin             = GPIO_PIN_5,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_E,
//        .gpio_pin             = GPIO_PIN_6,
//        .gpio_alt             = GPIO_ALT_FUNC_2,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_D,
//        .gpio_pin             = GPIO_PIN_7,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
};


static gpio_settings_config_t spi0_arr1_cfg[7U] = {
//    {
//        .gpio_port            = GPIO_B,
//        .gpio_pin             = GPIO_PIN_2,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_E,
//        .gpio_pin             = GPIO_PIN_2,
//        .gpio_alt             = GPIO_ALT_FUNC_2,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_B,
//        .gpio_pin             = GPIO_PIN_3,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_B,
//        .gpio_pin             = GPIO_PIN_5,
//        .gpio_alt             = GPIO_ALT_FUNC_4,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_B,
//        .gpio_pin             = GPIO_PIN_5,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_E,
//        .gpio_pin             = GPIO_PIN_6,
//        .gpio_alt             = GPIO_ALT_FUNC_2,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_D,
//        .gpio_pin             = GPIO_PIN_7,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
};

static gpio_settings_config_t spi0_arr2_cfg[7U] = {
//    {
//        .gpio_port            = GPIO_E,
//        .gpio_pin             = GPIO_PIN_0,
//        .gpio_alt             = GPIO_ALT_FUNC_2,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_E,
//        .gpio_pin             = GPIO_PIN_2,
//        .gpio_alt             = GPIO_ALT_FUNC_2,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_E,
//        .gpio_pin             = GPIO_PIN_1,
//        .gpio_alt             = GPIO_ALT_FUNC_2,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_B,
//        .gpio_pin             = GPIO_PIN_5,
//        .gpio_alt             = GPIO_ALT_FUNC_4,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_B,
//        .gpio_pin             = GPIO_PIN_5,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_E,
//        .gpio_pin             = GPIO_PIN_6,
//        .gpio_alt             = GPIO_ALT_FUNC_2,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_D,
//        .gpio_pin             = GPIO_PIN_7,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
};

static gpio_settings_config_t spi1_arr0_cfg[7U] = {
//    {
//        .gpio_port            = GPIO_D,
//        .gpio_pin             = GPIO_PIN_0,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_D,
//        .gpio_pin             = GPIO_PIN_2,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_D,
//        .gpio_pin             = GPIO_PIN_1,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_D,
//        .gpio_pin             = GPIO_PIN_3,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_A,
//        .gpio_pin             = GPIO_PIN_6,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_E,
//        .gpio_pin             = GPIO_PIN_7,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_B,
//        .gpio_pin             = GPIO_PIN_12,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
};

static gpio_settings_config_t spi1_arr1_cfg[7U] = {
//    {
//        .gpio_port            = GPIO_E,
//        .gpio_pin             = GPIO_PIN_9,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_E,
//        .gpio_pin             = GPIO_PIN_0,
//        .gpio_alt             = GPIO_ALT_FUNC_5,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_E,
//        .gpio_pin             = GPIO_PIN_3,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_E,
//        .gpio_pin             = GPIO_PIN_1,
//        .gpio_alt             = GPIO_ALT_FUNC_5,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_A,
//        .gpio_pin             = GPIO_PIN_6,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_E,
//        .gpio_pin             = GPIO_PIN_7,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
//    {
//        .gpio_port            = GPIO_B,
//        .gpio_pin             = GPIO_PIN_12,
//        .gpio_alt             = GPIO_ALT_FUNC_3,
//        .gpio_dir             = GPIO_DIR_OUTPUT,
//        .pull_cfg             = GPIO_PULL_DISABLE,
//        .gpio_strength        = GPIO_STRENGTH_LOW_FAST,
//        
//    },
};




#define WREN				(0x06)
#define WRDI				(0x04)
#define RDSR				(0x05)
#define WRSR				(0x01)
#define READ				(0x03)
#define WRITE				(0x02)
#define FSTRD				(0x0b)
#define RDID				(0x9f)
#define SPI_INSTABCE_NUM	(SPI_INSTANCE0)

extern bool trigger_enable;
extern bool host_enable;

static SPI_TypeDef * spi_base[SPI_INSTANCE_MAX_COUNT] = SPI_BASE_ADDRS;
#if (SPI_INSTANCE_MAX_COUNT > 1)
static SPI_TypeDef * spi1_base = SPI1;
#endif
static SPI_TypeDef * spi0_base = SPI0;
uint8_t AHB_SRAM_DATA_ATTRIBUTES slaver_deepsleep_data_receive[BUFFER_SIZE];
uint8_t AHB_SRAM_DATA_ATTRIBUTES slaver_deepsleep_data_send[BUFFER_SIZE];

uint8_t master_data_receive[BUFFER_SIZE];	
uint8_t master_data_send[BUFFER_SIZE];
uint8_t slave_data_send[BUFFER_SIZE];
uint8_t slave_data_receive[BUFFER_SIZE];

void spi_reset_buffer()
{
	for (uint32_t i = 0; i < BUFFER_SIZE; i++)
	{
		master_data_receive[i] = 0;
		master_data_send[i] = i%256;
		slave_data_receive[i] = 0;
		slave_data_send[i] = (BUFFER_SIZE - i - 1)%256;
	}
}


static void spi_pcs_pin_config(spi_pcs_e which_pcs, gpio_settings_config_t *spi_arr_cfg)
{
#if 0
	switch (which_pcs)
	{
		case SPI_PCS0:
			gpio_init(&spi_arr_cfg[3]);
			break;
		case SPI_PCS1:
			gpio_init(&spi_arr_cfg[4]);
			break;
		case SPI_PCS2:
			gpio_init(&spi_arr_cfg[5]);
			break;
		case SPI_PCS3:
			gpio_init(&spi_arr_cfg[6]);
			break;
		default:
			break;
	}		
#endif //old version reserved
}


static void spi_gpio_init(spi_instance_e spi_instance, 
							spi_mux_sel_e pin_mux, 
							spi_transfer_width_e spi_transfer_width, 
							spi_pcs_e which_pcs)
{
#if 0
	
	if (spi_instance == SPI_INSTANCE0) 
	{
		switch(pin_mux)
		{
			case SPI_MUX_SEL0:
				pins_mux_init(3U, spi0_arr0_cfg);
				spi_pcs_pin_config(which_pcs, spi0_arr0_cfg);
				
				break;
			case SPI_MUX_SEL1:
				pins_mux_init(3U, spi0_arr1_cfg);
				spi_pcs_pin_config(which_pcs, spi0_arr1_cfg);
				
				break;
			case SPI_MUX_SEL2:
				pins_mux_init(3U, spi0_arr2_cfg);
				spi_pcs_pin_config(which_pcs, spi0_arr2_cfg);
				
				break;				
			default:
				break;
		}		
	}
	else if (spi_instance == SPI_INSTANCE1)
	{
		switch(pin_mux)
		{
			case SPI_MUX_SEL0:
				pins_mux_init(3U, spi1_arr0_cfg);
				spi_pcs_pin_config(which_pcs, spi1_arr0_cfg);
				
				break;
			case SPI_MUX_SEL1:
				pins_mux_init(3U, spi1_arr1_cfg);
				spi_pcs_pin_config(which_pcs, spi1_arr1_cfg);
				
				break;			
			default:
				break;
		}			
	}
	else
	{
		//do nothing
	}
	#endif //old version reserved
}
							
void spi_slave_gpio_init(spi_instance_e spi_instance, spi_mux_sel_e pin_mux, spi_slave_config_t *config)
{
	DEV_ASSERT(spi_instance < SPI_INSTANCE_MAX_COUNT);
	DEV_ASSERT(pin_mux < SPI_MUX_MAX);
	DEV_ASSERT(config != NULL);

	spi_gpio_init(spi_instance, pin_mux, config->spi_transfer_width, config->which_pcs);
}

void spi_master_gpio_init(spi_instance_e spi_instance, spi_mux_sel_e pin_mux, spi_master_config_t *config)
{
	DEV_ASSERT(spi_instance < SPI_INSTANCE_MAX_COUNT);
	DEV_ASSERT(pin_mux < SPI_MUX_MAX);
	DEV_ASSERT(config != NULL);

	spi_gpio_init(spi_instance, pin_mux, config->spi_transfer_width, config->which_pcs);
}


void spi_clk_init(spi_instance_e spi_instance)
{
	DEV_ASSERT(spi_instance < SPI_INSTANCE_MAX_COUNT);
	module_clk_cfg_t module_clk_cfg;

	module_name_e spi_module[] = {MODULE_SPI0, MODULE_SPI1};	
	module_clk_cfg.module_name = spi_module[spi_instance];
	module_clk_cfg.fclk_sel = APB_CLK;
	module_clk_cfg.pclk_dsleep_gating_en = GATING_ON;
	module_clk_cfg.fclk_dsleep_gating_en = GATING_ON;
	module_clk_cfg.fclk_div = PER_DIVIDE_1;

	clock_module_clk_cfg(&module_clk_cfg);
}


void spi_reset_default_config(void)
{
	#if 0
	spi_master_init_config.bits_per_sec = 1000000U;
	spi_master_init_config.which_pcs = SPI_PCS1;
	spi_master_init_config.pcs_polarity = SPI_PCS_ACTIVE_LOW;
	spi_master_init_config.is_pcs_continuous = false;
	spi_master_init_config.frame_size = 32;
	spi_master_init_config.spi_sck_phase = SPI_SCK_PHASE_1ST_EDGE; 
	spi_master_init_config.spi_sck_polarity = SPI_SCK_ACTIVE_HIGH;
	spi_master_init_config.lsb_first = true;
	spi_master_init_config.call_back = NULL;
	spi_master_init_config.call_back_param = NULL;
	spi_master_init_config.spi_transfer_width = SPI_SINGLE_BIT;
	spi_master_init_config.byte_swap = false;
	spi_master_init_config.pin_cfg = SPI_SDI_IN_SDO_OUT;

	spi_slave_init_config.which_pcs = SPI_PCS1;
	spi_slave_init_config.pcs_polarity = SPI_PCS_ACTIVE_LOW;
	spi_slave_init_config.is_pcs_continuous = false;
	spi_slave_init_config.frame_size = 32;
	spi_slave_init_config.spi_sck_phase = SPI_SCK_PHASE_1ST_EDGE;
	spi_slave_init_config.spi_sck_polarity = SPI_SCK_ACTIVE_HIGH;
	spi_slave_init_config.lsb_first = true;
	spi_slave_init_config.call_back = NULL;
	spi_slave_init_config.call_back_param = NULL;
	spi_slave_init_config.spi_transfer_width = SPI_SINGLE_BIT;
	spi_slave_init_config.byte_swap = false;
	spi_slave_init_config.pin_cfg = SPI_SDI_IN_SDO_OUT;
	#endif
	
}


static void temp_spi_gpio_init()
{
	#if 0
	gpio_settings_config_t setting_cfg;

	/* SPI0, PCS1, PTB5 */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_5;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_3;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_LOW_FAST;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

	/* SPI0, SCK, PTD15 */
    setting_cfg.gpio_port = GPIO_D;
    setting_cfg.gpio_pin = GPIO_PIN_15;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_4;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_LOW_FAST;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

	/* SPI0, SDI, PTD16 */
    setting_cfg.gpio_port = GPIO_D;
    setting_cfg.gpio_pin = GPIO_PIN_16;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_4;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_LOW_FAST;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* SPI0, SDO, PTB4 */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_4;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_3;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_LOW_FAST;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
      
    /* SPI0, PCS2, PTE6 */
    setting_cfg.gpio_port = GPIO_E;
    setting_cfg.gpio_pin = GPIO_PIN_6;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_LOW_FAST;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
    
    /* SPI0, PCS3, PTD7 */
    setting_cfg.gpio_port = GPIO_D;
    setting_cfg.gpio_pin = GPIO_PIN_7;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_3;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_LOW_FAST;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
	
	#endif //old version reserved
}


void spi_common_test(uint32_t which_pcs,
					uint32_t pcs_polarity,
					uint32_t frame_size,
					uint32_t spi_sck_phase,
					uint32_t spi_sck_polarity,
					uint32_t lsb_first,
					uint32_t byte_swap)
{
	
	spi_reset_default_config();
	spi_master_init_config.which_pcs = (spi_pcs_e)which_pcs;
	spi_master_init_config.pcs_polarity = (spi_pcs_polarity_e)pcs_polarity;
	spi_master_init_config.frame_size = (uint16_t)frame_size;
	spi_master_init_config.spi_sck_phase = (spi_sck_phase_e)spi_sck_phase;
	spi_master_init_config.spi_sck_polarity = (spi_sck_polarity_e)spi_sck_polarity;
	spi_master_init_config.lsb_first = (bool)lsb_first;
	spi_master_init_config.byte_swap = (bool)byte_swap;

	spi_slave_init_config.which_pcs = (spi_pcs_e)which_pcs;
	spi_slave_init_config.pcs_polarity = (spi_pcs_polarity_e)pcs_polarity;
	spi_slave_init_config.frame_size = (uint16_t)frame_size;
	spi_slave_init_config.spi_sck_phase = (spi_sck_phase_e)spi_sck_phase;
	spi_slave_init_config.spi_sck_polarity = (spi_sck_polarity_e)spi_sck_polarity;
	spi_slave_init_config.lsb_first = (bool)lsb_first;
	spi_slave_init_config.byte_swap = (bool)byte_swap;

	uint16_t i;
	uint8_t master_data_receive[BUFFER_SIZE];	
	uint8_t master_data_send[BUFFER_SIZE];
	uint8_t slaver_data_send[BUFFER_SIZE];
	uint8_t slaver_data_receive[BUFFER_SIZE];

	/* Initialize the data buffer */
	for (i = 0U; i < BUFFER_SIZE; i++)
	{
		master_data_send[i] = i;
		slaver_data_send[i] = BUFFER_SIZE - i - 1U; 	
		master_data_receive[i] = 0U;	
		slaver_data_receive[i] = 0U;					
	}
#if (SPI_INSTANCE_MAX_COUNT > 1)
	//SPI1 as slave
	spi_clk_init(SPI_INSTANCE1);
	spi_slave_gpio_init(SPI_INSTANCE1, SPI_MUX_SEL0, &spi_slave_init_config);
	spi_slave_init(SPI_INSTANCE1, &spi_slave_init_config);
	spi_slave_transfer(SPI_INSTANCE1, slaver_data_send, slaver_data_receive, BUFFER_SIZE);
#endif
	//SPI0 as master
	spi_clk_init(SPI_INSTANCE0);
	spi_master_gpio_init(SPI_INSTANCE0, SPI_MUX_SEL0, &spi_master_init_config);
	spi_master_init(SPI_INSTANCE0, &spi_master_init_config);
	spi_master_transfer(SPI_INSTANCE0, master_data_send, master_data_receive, BUFFER_SIZE);

	delay_ms(1000);
	
	for (i = 0U; i < BUFFER_SIZE; i++)
	{
		INFO("master_data_receive[%d]=%#d--slaver_data_send[%d]=%#d\n", i, master_data_receive[i],i, slaver_data_send[i]);
	}
	
	for (i = 0U; i < BUFFER_SIZE; i++)
	{
		INFO("slaver_data_receive[%d]=%#d--master_data_send[%d]=%#d\n", i, slaver_data_receive[i],i, master_data_send[i]);
	}
}

void spi_timing_figure_test(uint32_t baud_rate)
{
	spi_reset_default_config();
	spi_master_init_config.bits_per_sec = baud_rate;

	uint16_t i;
	uint8_t master_data_receive[BUFFER_SIZE];	
	uint8_t master_data_send[BUFFER_SIZE];
	uint8_t slaver_data_send[BUFFER_SIZE];
	uint8_t slaver_data_receive[BUFFER_SIZE];

	/* Initialize the data buffer */
	for (i = 0U; i < BUFFER_SIZE; i++)
	{
		master_data_send[i] = 0x55;
		slaver_data_send[i] = 0xaa; 	
		master_data_receive[i] = 0U;	
		slaver_data_receive[i] = 0U;					
	}
#if (SPI_INSTANCE_MAX_COUNT > 1)
	//SPI1 as slave
	spi_clk_init(SPI_INSTANCE1);
	spi_slave_gpio_init(SPI_INSTANCE1, SPI_MUX_SEL0, &spi_slave_init_config);
	spi_slave_init(SPI_INSTANCE1, &spi_slave_init_config);
	spi_slave_transfer(SPI_INSTANCE1, slaver_data_send, slaver_data_receive, BUFFER_SIZE);
#endif
	//SPI0 as master
	spi_clk_init(SPI_INSTANCE0);
	spi_master_gpio_init(SPI_INSTANCE0, SPI_MUX_SEL0, &spi_master_init_config);
	spi_master_init(SPI_INSTANCE0, &spi_master_init_config);
	spi_master_transfer(SPI_INSTANCE0, master_data_send, master_data_receive, BUFFER_SIZE);

	delay_ms(1000);
	
	for (i = 0U; i < BUFFER_SIZE; i++)
	{
		INFO("master_data_receive[%d]=%#d--slaver_data_send[%d]=%#d\n", i, master_data_receive[i],i, slaver_data_send[i]);
	}
	
	for (i = 0U; i < BUFFER_SIZE; i++)
	{
		INFO("slaver_data_receive[%d]=%#d--master_data_send[%d]=%#d\n", i, slaver_data_receive[i],i, master_data_send[i]);
	}
}

void spi_loopback_test()
{
	spi_reset_default_config();
	spi_master_init_config.which_pcs = SPI_PCS1;
	spi_master_init_config.pin_cfg = SPI_SDI_IN_OUT;

	uint16_t i;
	uint8_t master_data_receive[BUFFER_SIZE];	
	uint8_t master_data_send[BUFFER_SIZE];

	/* Initialize the data buffer */
	for (i = 0U; i < BUFFER_SIZE; i++)
	{
		master_data_send[i] = i;	
		master_data_receive[i] = 0U;						
	}

	spi_clk_init(SPI_INSTANCE0);
	spi_master_gpio_init(SPI_INSTANCE0, SPI_MUX_SEL0, &spi_master_init_config);
	spi_master_init(SPI_INSTANCE0, &spi_master_init_config);
	spi_master_transfer(SPI_INSTANCE0, master_data_send, master_data_receive, BUFFER_SIZE);

	delay_ms(10000);

	for (i = 0U; i < BUFFER_SIZE; i++)
	{
		INFO("master_data_receive[%d]=%#d--master_data_send[%d]=%#d\n", i, master_data_receive[i],i, master_data_send[i]);
	}
}



/**
 *@brief enable write, set WEL to 1.
 *@param[in] none
 *@return    none
 */
void fram_wren_command()
{
	uint8_t master_data_send[1];
	master_data_send[0] = WREN;
	
	spi_master_transfer(SPI_INSTABCE_NUM, master_data_send, NULL, 1);

	delay_ms(10);
}

/**
 *@brief disable write, set WEL to 0.
 *@param[in] none
 *@return    none
 */
void fram_wrdi_command()
{
	uint8_t master_data_send[1];
	master_data_send[0] = WRDI;
	
	spi_master_transfer(SPI_INSTABCE_NUM, master_data_send, NULL, 1);

	delay_ms(10);
}

/**
 *@brief read status register.
 *@param[in] none
 *@return    none
 */
uint8_t fram_rdsr_command()
{
	uint8_t master_data_send[2];
	uint8_t master_data_receive[2];
	
	master_data_send[0] = RDSR;
	master_data_send[1] = 0x00;
	
	spi_master_transfer(SPI_INSTABCE_NUM, master_data_send, master_data_receive, 2);
	delay_ms(10);

	return master_data_receive[1];
}

/**
 *@brief write status register.
 *@param[in] none
 *@return    none
 */
void fram_wrsr_command(uint8_t status)
{
	uint8_t master_data_send[2];
	master_data_send[0] = WRSR;
	master_data_send[1] = status;

	spi_master_transfer(SPI_INSTABCE_NUM, master_data_send, NULL, 2);
	delay_ms(10);
}

/**
 *@brief read memory cell.
 *@param[in] none
 *@return    none
 */
void fram_read_command(uint16_t data_addr, uint8_t *data_receive, uint32_t data_size)
{
	uint8_t master_data_send[100];
	uint8_t master_data_receive[100];
	master_data_send[0] = READ;
	master_data_send[1] = (data_addr&0xff00)>>8U;
	master_data_send[2] = data_addr&0x00ff;

	spi_master_transfer(SPI_INSTABCE_NUM, master_data_send, master_data_receive, data_size+3);
	delay_ms(10);
	
	for (uint32_t i = 0; i < data_size; i++)
	{
		data_receive[i] = master_data_receive[3+i];
	}
}

/**
 *@brief write memory cell.
 *@param[in] none
 *@return    none
 */
void fram_write_command(uint16_t data_addr, uint8_t *data_write, uint32_t data_size)
{
	uint8_t master_data_send[100];
	master_data_send[0] = WRITE;
	master_data_send[1] = (data_addr&0xff00)>>8U;
	master_data_send[2] = data_addr&0x00ff;

	for (uint32_t i = 0; i < data_size; i++)
	{
		master_data_send[3+i] = data_write[i];
	}

	spi_master_transfer(SPI_INSTABCE_NUM, master_data_send, NULL, data_size+3);

	delay_ms(10);
}

/**
 *@brief fast read memory cell.
 *@param[in] none
 *@return    none
 */
void fram_fstrd_command(uint16_t data_addr, uint8_t *data_receive, uint32_t data_size)
{
	uint8_t master_data_send[100];
	master_data_send[0] = FSTRD;
	master_data_send[1] = (data_addr&0xff00)>>8U;
	master_data_send[2] = data_addr&0x00ff;
	master_data_send[3] = 0xff;

	spi_master_transfer(SPI_INSTABCE_NUM, master_data_send, data_receive, data_size+4);

	delay_ms(1000);
}

/**
 *@brief read device ID.
 *@param[in] none
 *@return    none
 */
uint32_t fram_rdid_command()
{
	uint32_t id;
	uint8_t master_data_send[5] = {0x00, 0x00, 0x00, 0x00, 0x00};
	uint8_t master_data_receive[5];
	master_data_send[0] = RDID;
	
	spi_master_transfer(SPI_INSTABCE_NUM, master_data_send, master_data_receive, 5);
	delay_ms(100);

	id = (master_data_receive[1]<<24) | (master_data_receive[2]<<16) | (master_data_receive[3]<<8) | master_data_receive[4];
	
	return id;
}

void spi_fram_test()
{
#if 0
	gpio_settings_config_t setting_cfg;
    /* SPI1, PCS0, PTD3, 45, connect to shunming*/
    setting_cfg.gpio_port = GPIO_D;
    setting_cfg.gpio_pin = GPIO_PIN_3;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_3;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_HIGH_FAST;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
	/* SPI1, PCS1, PTA6, 38 ,connect to fushitong*/
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_6;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_3;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_HIGH_FAST;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
    /* SPI1, SCK, PTD0, 2 */
    setting_cfg.gpio_port = GPIO_D;
    setting_cfg.gpio_pin = GPIO_PIN_0;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_3;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_HIGH_FAST;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
    /* SPI1, SDI, PTD1, 1 */
    setting_cfg.gpio_port = GPIO_D;
    setting_cfg.gpio_pin = GPIO_PIN_1;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_3;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_HIGH_FAST;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
    /* SPI1, SDO, PTD2, 46 */
    setting_cfg.gpio_port = GPIO_D;
    setting_cfg.gpio_pin = GPIO_PIN_2;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_3;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_HIGH_FAST;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

	uint16_t data_addr = 0x00;
	uint8_t  data_write[4] = {0x11, 0x22, 0x33, 0x44};
	uint8_t  data_receive[7] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	uint32_t data_size = 4;

	spi_reset_buffer();
	//spi_gpio_toggle_init();
	spi_reset_default_config();
	spi_master_init_config.bits_per_sec = 20000000;
	spi_master_init_config.is_pcs_continuous = true;
	spi_master_init_config.frame_size = 8;
	spi_master_init_config.lsb_first = false;

	//SPI1 as MASTER
	spi_clk_init(SPI_INSTANCE1);
	spi_master_init(SPI_INSTANCE1, &spi_master_init_config);

	spi_pcs_set(SPI_INSTANCE1, SPI_PCS0, SPI_PCS_ACTIVE_LOW);
	fram_wren_command();
	fram_write_command(data_addr, data_write, data_size);

	spi_pcs_set(SPI_INSTANCE1, SPI_PCS1, SPI_PCS_ACTIVE_LOW);
	fram_wren_command();
	fram_write_command(data_addr, data_write, data_size);

	uint32_t time = 0;
	while(1)
	{
		if ((time%2) == 0)
		{
			/* connect to shunming*/
			spi_pcs_set(SPI_INSTANCE1, SPI_PCS0, SPI_PCS_ACTIVE_LOW);
		}
		else
		{
			/* connect to fushitong*/
			spi_pcs_set(SPI_INSTANCE1, SPI_PCS1, SPI_PCS_ACTIVE_LOW);
		}
		
		delay_ms(1U);
		fram_read_command(data_addr, data_receive, data_size);
		delay_ms(1);
	
		for (uint32_t i = 0; i < 4; i++)
		{
			if (data_write[i] != data_receive[i])
			{
				INFO("test error at loop%d\n", time);
				for (i = 0U; i < 4; i++)
				{
					INFO("data_receive[%d]=%#d\n", i, data_receive[i]);
				}
				return;
			}
		}

		for (uint32_t i = 0; i < 4; i++)
		{
			data_receive[i] = 0;
		}

		gpio_toggle(GPIO_D, GPIO_PIN_16);
		
		INFO("loop%d test success\n", time);
		time = time + 1;
	}
#endif //old version reserved
}


//mannual test
void spi_cont_cmd_test(uint32_t frame_size)
{
	#if 0
	
    spi_reset_default_config();
	spi_reset_buffer();
	
    spi_clk_init(SPI_INSTANCE0);

    spi_master_init_config.is_pcs_continuous = true;
    spi_master_init_config.frame_size = frame_size;
    
    temp_spi_gpio_init();
    spi_master_init(SPI_INSTANCE0, &spi_master_init_config);
    INFO("master TCR : %#x\n", spi0_base->TCR.w);
    spi_master_transfer(SPI_INSTANCE0, master_data_send, master_data_receive, BUFFER_SIZE);
    delay_ms(1000);
	
	#endif //old version reserved
}

void spi_timing_test(uint32_t SCK2PCS, uint32_t PCS2SCK, uint32_t SCK2SCK, uint32_t PCS2CMD, uint32_t SCKHLD, uint32_t SCKSTP, bool cont_en)
{
	#if 0
	
	spi_reset_default_config();
	spi_reset_buffer();
	
    spi_clk_init(SPI_INSTANCE1);
	
	
	spi_master_init_config.which_pcs = SPI_PCS0;
	spi_master_init_config.is_pcs_continuous = cont_en;
	spi_master_init(SPI_INSTANCE1, &spi_master_init_config);
	spi1_base->CCR0.SCK2PCS = SCK2PCS;
	spi1_base->CCR0.PCS2SCK = PCS2SCK;
	spi1_base->CCR1.SCK2SCK = SCK2SCK;
	spi1_base->CCR1.PCS2CMD = PCS2CMD;
	spi1_base->CCR1.SCKHLD = SCKHLD;
	spi1_base->CCR1.SCKSTP = SCKSTP;
	spi1_base->TCR.PRESC = 6;
	spi_master_transfer(SPI_INSTANCE1, master_data_send, master_data_receive, BUFFER_SIZE);

	delay_ms(10000);
	
	#endif //old version reserved
}

void spi_pin_test(uint32_t master_instance, uint32_t master_pin_sel, uint32_t master_pcs, uint32_t slave_instance, uint32_t slave_pin_sel, uint32_t slave_pcs)
{
	spi_reset_default_config();
	spi_reset_buffer();
	
	spi_master_init_config.which_pcs = (spi_pcs_e)master_pcs;
	spi_slave_init_config.which_pcs = (spi_pcs_e)slave_pcs;

	//slave
	spi_clk_init((spi_instance_e)slave_instance);
	spi_slave_gpio_init((spi_instance_e)slave_instance, (spi_mux_sel_e)slave_pin_sel, &spi_slave_init_config);
	spi_slave_init((spi_instance_e)slave_instance, &spi_slave_init_config);
	spi_slave_transfer((spi_instance_e)slave_instance, slave_data_send, slave_data_receive, 12);

	//master
	spi_clk_init((spi_instance_e)master_instance);
	spi_master_gpio_init((spi_instance_e)master_instance, (spi_mux_sel_e)master_pin_sel, &spi_master_init_config);
	spi_master_init((spi_instance_e)master_instance, &spi_master_init_config);
	spi_master_transfer((spi_instance_e)master_instance, master_data_send, master_data_receive, 12);

	delay_ms(2000);
	
	for (uint32_t i = 0U; i < 12; i++)
	{
		INFO("master_data_receive[%d]=%#d--slave_data_send[%d]=%#d\n", i, master_data_receive[i],i, slave_data_send[i]);
	}
	
	for (uint32_t i = 0U; i < 12; i++)
	{
		INFO("slave_data_receive[%d]=%#d--master_data_send[%d]=%#d\n", i, slave_data_receive[i],i, master_data_send[i]);
	}

	for (uint32_t i = 0U; i < 12; i++)
	{
		if ((master_data_receive[i] != slave_data_send[i]) || (slave_data_receive[i] != master_data_send[i]))
		{
			INFO("test fail!\n");
			return;
		}
	}

	INFO("test success!\n");
}


void spi_debug_test(bool debug_en)
{
	#if 0
	
	spi_reset_default_config();
	spi_reset_buffer();
		
	spi_clk_init(SPI_INSTANCE0);
	temp_spi_gpio_init();
	
	spi_slave_init(SPI_INSTANCE0, &spi_slave_init_config);
	spi0_base->CR.DBG_EN = debug_en;
	spi_slave_transfer(SPI_INSTANCE0, slave_data_send, slave_data_receive, BUFFER_SIZE);

	while(1)
	{
		delay_ms(10);
	}
	
	#endif
}

void spi_stall_mode_test(bool stall_en)
{
	spi_reset_default_config();
	spi_reset_buffer();
	spi_master_init_config.frame_size = 64;

	spi_clk_init(SPI_INSTANCE0);
	temp_spi_gpio_init();
	spi_master_init(SPI_INSTANCE0, &spi_master_init_config);
	spi_stall_mode_set(SPI_INSTANCE0, (func_switch_e)stall_en);
	spi_master_transfer(SPI_INSTANCE0, master_data_send, master_data_receive, BUFFER_SIZE);
	delay_ms(1000);
}

//*********************************************************************************************************
//auto test
void spi_common_cd_test(uint32_t which_pcs,
					uint32_t pcs_polarity,
					uint32_t frame_size,
					uint32_t spi_sck_phase,
					uint32_t spi_sck_polarity,
					uint32_t lsb_first,
					uint32_t byte_swap)
{
	spi_reset_default_config();
	spi_reset_buffer();

	spi_master_init_config.which_pcs = (spi_pcs_e)which_pcs;
	spi_master_init_config.pcs_polarity = (spi_pcs_polarity_e)pcs_polarity;
	spi_master_init_config.frame_size = (uint16_t)frame_size;
	spi_master_init_config.spi_sck_phase = (spi_sck_phase_e)spi_sck_phase;
	spi_master_init_config.spi_sck_polarity = (spi_sck_polarity_e)spi_sck_polarity;
	spi_master_init_config.lsb_first = (bool)lsb_first;
	spi_master_init_config.byte_swap = (bool)byte_swap;

	spi_master_init_config.which_pcs = SPI_PCS1;

	//SPI0 as master
	spi_clk_init(SPI_INSTANCE0);
	/* SPI0, PCS1, PTB5 18*//* SPI0, SCK, PTD15 15*//* SPI0, SDI, PTD16 14*//* SPI0, SDO, PTB4 19*/
	temp_spi_gpio_init();
	spi_master_init(SPI_INSTANCE0, &spi_master_init_config);
	spi_master_transfer(SPI_INSTANCE0, master_data_send, master_data_receive, BUFFER_SIZE);

	delay_ms(1000);

	for (uint8_t i = 0U; i < BUFFER_SIZE; i++)
	{   
		if (master_data_receive[i] != slave_data_send[i])
		{
			INFO("{test_fail!}");
			return;
		}
	}
	
	INFO("{test_ok!}");
}

void spi_circule_fifo_cd_test()
{
	spi_reset_default_config();
	spi_reset_buffer();

	//SPI0 as master
		
	spi_clk_init(SPI_INSTANCE0);
	temp_spi_gpio_init();
	spi_master_init(SPI_INSTANCE0, &spi_master_init_config);
	spi_circule_fifo_enable(SPI_INSTANCE0, FUNC_ENABLE, 4);
	spi_master_transfer(SPI_INSTANCE0, master_data_send, master_data_receive, BUFFER_SIZE);

	delay_ms(1000);
}


static void spi_match_cd_test_callback(void *driver_state, spi_event_e event, void *user_data)
{
	if (event == SPI_EVENT_DATA_MATCH)
	{
		INFO("{test_ok!}");
	}
}


void spi_match_cd_test(uint32_t match_type, bool match_only, uint32_t match0, uint32_t match1)
{
	spi_reset_default_config();
	spi_reset_buffer();
	spi_clk_init(SPI_INSTANCE0);
	temp_spi_gpio_init();

	switch(match_type)
	{
		case 1:
			spi_master_init_config.frame_size = 64;
			spi_master_init(SPI_INSTANCE0, &spi_master_init_config);
			spi_callback_install(SPI_INSTANCE0, spi_match_cd_test_callback, NULL);
			spi_match_type_set(SPI_INSTANCE0, SPI_FIST_WORD_MATCH0_OR_MATCH1, match0, match1, match_only);
			spi_interrupt_source_enable(SPI_INSTANCE0, SPI_DATA_MATCH, true);
			spi_master_transfer(SPI_INSTANCE0, master_data_send, master_data_receive, BUFFER_SIZE);
			break;
		case 2:
			spi_master_init_config.frame_size = 64;
			spi_master_init(SPI_INSTANCE0, &spi_master_init_config);
			spi_callback_install(SPI_INSTANCE0, spi_match_cd_test_callback, NULL);
			spi_match_type_set(SPI_INSTANCE0, SPI_ANY_WORD_MATCH0_OR_MATCH1, match0, match1, match_only);
			spi_interrupt_source_enable(SPI_INSTANCE0, SPI_DATA_MATCH, true);
			spi_master_transfer(SPI_INSTANCE0, master_data_send, master_data_receive, BUFFER_SIZE);
			break;
		case 3:
			spi_master_init_config.frame_size = 64;
			spi_master_init(SPI_INSTANCE0, &spi_master_init_config);
			spi_callback_install(SPI_INSTANCE0, spi_match_cd_test_callback, NULL);
			spi_match_type_set(SPI_INSTANCE0, SPI_FIST_MATCH0_AND_SECOND_MATCH1, match0, match1, match_only);
			spi_interrupt_source_enable(SPI_INSTANCE0, SPI_DATA_MATCH, true);
			spi_master_transfer(SPI_INSTANCE0, master_data_send, master_data_receive, BUFFER_SIZE);
			break;
		case 4:
			spi_master_init_config.frame_size = 96;
			spi_master_init(SPI_INSTANCE0, &spi_master_init_config);
			spi_callback_install(SPI_INSTANCE0, spi_match_cd_test_callback, NULL);
			spi_match_type_set(SPI_INSTANCE0, SPI_ANY_MATCH0_AND_NEXT_MATCH1, match0, match1, match_only);
			spi_interrupt_source_enable(SPI_INSTANCE0, SPI_DATA_MATCH, true);
			spi_master_transfer(SPI_INSTANCE0, master_data_send, master_data_receive, BUFFER_SIZE);
			break;
		case 5:
			spi_master_init_config.frame_size = 64;
			spi_master_init(SPI_INSTANCE0, &spi_master_init_config);
			spi_callback_install(SPI_INSTANCE0, spi_match_cd_test_callback, NULL);
			spi_match_type_set(SPI_INSTANCE0, SPI_FIST_AND_MATCH1_EQUAL_MATCH0_AND_MATCH1, match0, match1 , match_only);
			spi_interrupt_source_enable(SPI_INSTANCE0, SPI_DATA_MATCH, true);
			spi_master_transfer(SPI_INSTANCE0, master_data_send, master_data_receive, BUFFER_SIZE);
			break;
		case 6:
			spi_master_init_config.frame_size = 64;
			spi_master_init(SPI_INSTANCE0, &spi_master_init_config);
			spi_callback_install(SPI_INSTANCE0, spi_match_cd_test_callback, NULL);
			spi_match_type_set(SPI_INSTANCE0, SPI_ANY_AND_MATCH1_EQUAL_MATCH0_AND_MATCH1, match0, match1, match_only);
			spi_interrupt_source_enable(SPI_INSTANCE0, SPI_DATA_MATCH, true);
			spi_master_transfer(SPI_INSTANCE0, master_data_send, master_data_receive, BUFFER_SIZE);
			break;
		default:
			break;
	}
}


void spi_remainder_mode_cd_test(uint32_t test_case)
{
	spi_reset_default_config();
	spi_reset_buffer();
	spi_clk_init(SPI_INSTANCE0);
	temp_spi_gpio_init();

	switch(test_case)
	{
		case 1:
			spi_slave_init_config.frame_size = 32;
			spi_slave_init(SPI_INSTANCE0, &spi_slave_init_config);
			spi_remainder_mode_set(SPI_INSTANCE0, true);
			spi_slave_transfer(SPI_INSTANCE0, slave_data_send, slave_data_receive, BUFFER_SIZE);
			delay_ms(2000);
			for (uint8_t i = 0U; i < 7; i++)
			{
				if (slave_data_receive[i] != master_data_send[i])
				{
					INFO("{test_fail!}");
					return;
				}
			}
			INFO("{test_ok!}");
			break;
		case 2:
			spi_slave_init_config.frame_size = 56;
			spi_slave_init(SPI_INSTANCE0, &spi_slave_init_config);
			spi_remainder_mode_set(SPI_INSTANCE0, true);
			spi_slave_transfer(SPI_INSTANCE0, slave_data_send, slave_data_receive, BUFFER_SIZE);
			delay_ms(2000);
			for (uint8_t i = 0U; i < 6; i++)
			{
				if (slave_data_receive[i] != master_data_send[i])
				{
					INFO("{test_fail!}");
					return;
				}
			}
			INFO("{test_ok!}");
			break;
		default:
			break;
	}
}


void spi_deepsleep_cd_test(uint32_t test_case, bool pclk_gate, bool fclk_gate, bool doze_en)
{
	#if 0
	if (test_case == 5)
	{
		for (uint8_t i = 0; i < BUFFER_SIZE; i++)
		{
			if (slaver_deepsleep_data_receive[i] != master_data_send[i])
			{
				INFO("{test_fail!}");
				return;
			}
		}
		INFO("{test_ok!}");
		return;
	}
	
	spi_reset_default_config();
	spi_reset_buffer();
	temp_spi_gpio_init();
	
	module_clk_cfg_t module_clk_cfg;
	module_clk_cfg.module_name = MODULE_SPI0;
	module_clk_cfg.fclk_sel = APB_CLK;
	module_clk_cfg.pclk_dsleep_gating_en = (clk_dsleep_gating_en_e)pclk_gate;
	module_clk_cfg.fclk_dsleep_gating_en = (clk_dsleep_gating_en_e)fclk_gate;
	module_clk_cfg.fclk_div = PER_DIVIDE_1;
	clock_module_clk_cfg(&module_clk_cfg);

	for (uint16_t i = 0u; i < BUFFER_SIZE; i++)
	{	
		slaver_deepsleep_data_receive[i] = 0U;
		slaver_deepsleep_data_send[i] = BUFFER_SIZE-i-1U; 
	}

	switch(test_case)
	{
		case 1:
			spi_slave_init(SPI_INSTANCE0, &spi_slave_init_config);
			spi0_base->CR.SPEN = 0U;
			spi0_base->CR.DOZ_EN = doze_en;
			spi0_base->CR.SPEN = 1U;
			spi_slave_transfer(SPI_INSTANCE0, slaver_deepsleep_data_send, slaver_deepsleep_data_receive, BUFFER_SIZE);
			break;
		case 2:
			spi_slave_init(SPI_INSTANCE0, &spi_slave_init_config);
			spi_slave_transfer(SPI_INSTANCE0, slaver_deepsleep_data_send, slaver_deepsleep_data_receive, BUFFER_SIZE);
			break;
		case 3:
			spi_slave_init(SPI_INSTANCE0, &spi_slave_init_config);
			spi_slave_transfer(SPI_INSTANCE0, slaver_deepsleep_data_send, slaver_deepsleep_data_receive, BUFFER_SIZE);
			break;
		default:
			break;
	}
	
	#endif //old version reserved
}

static void spi_continus_mode_cd_test_callback(void *driver_state, spi_event_e event, void *user_data)
{
	if (event == SPI_EVENT_FRAME_COMPLETE)
	{
		INFO("{test_ok!}");
	}
}

void spi_continus_mode_cd_test(uint32_t test_case)
{
	spi_reset_default_config();
	spi_reset_buffer();
	spi_clk_init(SPI_INSTANCE0);
	temp_spi_gpio_init();

	switch(test_case)
	{
		case 1:
			spi_master_init_config.is_pcs_continuous = true;
			spi_master_init(SPI_INSTANCE0, &spi_master_init_config);
			spi_callback_install(SPI_INSTANCE0, spi_continus_mode_cd_test_callback, NULL);
			spi_interrupt_source_enable(SPI_INSTANCE0, SPI_FRAME_COMPLETE, true);
			spi_master_transfer(SPI_INSTANCE0, master_data_send, master_data_receive, BUFFER_SIZE);
			delay_ms(2000);
            for(uint8_t i = 0U; i < BUFFER_SIZE; i++)
            {
                if(slave_data_send[i] != master_data_receive[i])
                {
                    INFO("{test_fail!}");
                    return;
                }
            }
            INFO("{test_ok!}");
            break;
		case 2:
			spi_slave_init_config.is_pcs_continuous = true;
			spi_slave_init(SPI_INSTANCE0, &spi_slave_init_config);
			spi_slave_transfer(SPI_INSTANCE0, slave_data_send, slave_data_receive, BUFFER_SIZE);
			break;
		case 3:
			spi_slave_init_config.is_pcs_continuous = true;
			spi_slave_init(SPI_INSTANCE0, &spi_slave_init_config);
			spi_slave_transfer(SPI_INSTANCE0, slave_data_send, slave_data_receive, BUFFER_SIZE);
			break;
		default:
			break;
	}
}


void spi_match_only_cd_test(uint32_t test_case)
{
	spi_reset_default_config();
	spi_reset_buffer();
	spi_clk_init(SPI_INSTANCE0);
	temp_spi_gpio_init();

	switch(test_case)
	{
		case 1:
			spi_slave_init_config.frame_size = 64;
			spi_slave_init(SPI_INSTANCE0, &spi_slave_init_config);
			spi_match_type_set(SPI_INSTANCE0, SPI_FIST_WORD_MATCH0_OR_MATCH1, 0x07060504, 0x0b0a0908, true);
			//spi_interrupt_source_enable(SPI_INSTANCE0, SPI_DATA_MATCH, true);
			spi_slave_transfer(SPI_INSTANCE0, slave_data_send, slave_data_receive, BUFFER_SIZE);
			delay_ms(2000);
			for (uint8_t i = 0U; i < BUFFER_SIZE; i++)
			{
				INFO("{%d}", slave_data_receive[i]);
			}
			break;
		case 2:
			spi_slave_init_config.frame_size = 64;
			spi_slave_init(SPI_INSTANCE0, &spi_slave_init_config);
			spi_match_type_set(SPI_INSTANCE0, SPI_ANY_WORD_MATCH0_OR_MATCH1, 0x07060504, 0x0b0a0908, true);
			//spi_interrupt_source_enable(SPI_INSTANCE0, SPI_DATA_MATCH, true);
			spi_slave_transfer(SPI_INSTANCE0, slave_data_send, slave_data_receive, BUFFER_SIZE);
			delay_ms(2000);
			for (uint8_t i = 0U; i < BUFFER_SIZE; i++)
			{
				INFO("{%d}", slave_data_receive[i]);
			}
			break;
		case 3:
			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			break;
		default:
			break;
	}
}


void spi_mask_cd_test(uint32_t test_case)
{
    spi_reset_default_config();
	spi_reset_buffer();
	
    spi_clk_init(SPI_INSTANCE0);
    temp_spi_gpio_init();

    switch(test_case)
    {
        case 1:
		    spi_master_init(SPI_INSTANCE0, &spi_master_init_config);
		    spi_master_transfer(SPI_INSTANCE0, master_data_send, NULL, BUFFER_SIZE);
			break;
		case 2:
		    spi_master_init(SPI_INSTANCE0, &spi_master_init_config);
		    spi_master_transfer(SPI_INSTANCE0, NULL, master_data_receive, BUFFER_SIZE);
		    delay_ms(1000);
			for(uint8_t i = 0; i < BUFFER_SIZE; i++)
            {
            	//INFO("%d\n", master_data_receive[i]);
                if(master_data_receive[i] != slave_data_send[i])
                {
                    INFO("{test_fail!}");
                    return;
                }
            }
            INFO("{test_ok!}");
			break;
		case 3:
			spi_slave_init(SPI_INSTANCE0, &spi_slave_init_config);
			spi_slave_transfer(SPI_INSTANCE0, NULL, slave_data_receive, BUFFER_SIZE);
			delay_ms(1000);
			for(uint8_t i = 0; i < BUFFER_SIZE; i++)
            {
            	//INFO("%d\n", slave_data_receive[i]);
                if(slave_data_receive[i] != master_data_send[i])
                {
                    INFO("{test_fail!}");
                    return;
                }
            }
            INFO("{test_ok!}");
			break;
		case 4:
			spi_slave_init(SPI_INSTANCE0, &spi_slave_init_config);
			spi_slave_transfer(SPI_INSTANCE0, slave_data_send, NULL, BUFFER_SIZE);
			break;	
		default:
			break;
    } 
}



void spi_trig_mux_cd_test(uint32_t instance, uint32_t trig_num)
{
#if 0
	
	trigger_enable = true;
	spi_reset_default_config();
	spi_reset_buffer();
		
	spi_clk_init((spi_instance_e)instance);
	temp_spi_gpio_init();
	spi_master_init((spi_instance_e)instance, &spi_master_init_config);
	spi_master_transfer((spi_instance_e)instance, master_data_send, master_data_receive, 4);
	trgmux_spi_test((trgmux_spi_channel_e)TRGMUX_SPI0_TRIGGER_INPUT, (peripheral_trgmux_src_e)trig_num);
	
	delay_ms(100);
	for(uint32_t i = 0U; i < 4; i++)
    {
        //INFO("master_data_receive[%d]=%#d--slave_data_send[%d]=%#d\n", i, master_data_receive[i], i, slave_data_send[i]);
        if (master_data_receive[i] != slave_data_send[i])
		{
			INFO("{test_fail!}");
			trigger_enable = false;
			delay_ms(100);
			rstgen_reset_ctrl(RSTGEN_GLOBAL_RESET, RSTGEN_RST_ENABLE);
			return;
		}
    }
    INFO("\n");
	INFO("{test_ok!}");
	delay_ms(10);
	trigger_enable = false;
	rstgen_reset_ctrl(RSTGEN_GLOBAL_RESET, RSTGEN_RST_ENABLE);
	
#endif
}

void spi_host_request_cd_test(void)
{
	host_enable = true;

	spi_reset_default_config();
	spi_reset_buffer();
	spi_clk_init(SPI_INSTANCE0);
	temp_spi_gpio_init();

	spi_master_init_config.which_pcs = SPI_PCS0;
	spi_master_init_config.spi_transfer_width = SPI_TWO_BIT;
	spi_master_init(SPI_INSTANCE0, &spi_master_init_config);
	spi_master_transfer(SPI_INSTANCE0, master_data_send, NULL, BUFFER_SIZE);

	host_enable = false;
}

static void spi_half_duplex_gpio_init()
{
//	gpio_settings_config_t setting_cfg;

//	/* SPI0, PCS0, PTB5 */
//    setting_cfg.gpio_port = GPIO_B;
//    setting_cfg.gpio_pin = GPIO_PIN_5;
//    setting_cfg.gpio_alt = GPIO_ALT_FUNC_4;
//    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
//    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//    setting_cfg.gpio_strength = GPIO_STRENGTH_LOW_FAST;
//    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//    gpio_init(&setting_cfg);

//	/* SPI0, SCK, PTD15 */
//    setting_cfg.gpio_port = GPIO_D;
//    setting_cfg.gpio_pin = GPIO_PIN_15;
//    setting_cfg.gpio_alt = GPIO_ALT_FUNC_4;
//    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
//    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//    setting_cfg.gpio_strength = GPIO_STRENGTH_LOW_FAST;
//    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//    gpio_init(&setting_cfg);

//	/* SPI0, SDI, PTD16 */
//    setting_cfg.gpio_port = GPIO_D;
//    setting_cfg.gpio_pin = GPIO_PIN_16;
//    setting_cfg.gpio_alt = GPIO_ALT_FUNC_4;
//    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
//    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//    setting_cfg.gpio_strength = GPIO_STRENGTH_LOW_FAST;
//    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//    gpio_init(&setting_cfg);

//    /* SPI0, SDO, PTB4 */
//    setting_cfg.gpio_port = GPIO_B;
//    setting_cfg.gpio_pin = GPIO_PIN_4;
//    setting_cfg.gpio_alt = GPIO_ALT_FUNC_3;
//    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
//    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//    setting_cfg.gpio_strength = GPIO_STRENGTH_LOW_FAST;
//    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//    gpio_init(&setting_cfg);
//    
//    /* SPI0, PCS2, PTE6 */
//    setting_cfg.gpio_port = GPIO_E;
//    setting_cfg.gpio_pin = GPIO_PIN_6;
//    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
//    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
//    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//    setting_cfg.gpio_strength = GPIO_STRENGTH_LOW_FAST;
//    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//    gpio_init(&setting_cfg);
//    
//    /* SPI0, PCS3, PTD7 */
//    setting_cfg.gpio_port = GPIO_D;
//    setting_cfg.gpio_pin = GPIO_PIN_7;
//    setting_cfg.gpio_alt = GPIO_ALT_FUNC_3;
//    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
//    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//    setting_cfg.gpio_strength = GPIO_STRENGTH_LOW_FAST;
//    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//    gpio_init(&setting_cfg);
}



void spi_half_duplex_cd_test(uint32_t test_case)
{
	spi_reset_default_config();
	spi_reset_buffer();
	spi_clk_init(SPI_INSTANCE0);
    spi_half_duplex_gpio_init();
	
	switch(test_case)
	{
		case 1:
			spi_master_init_config.which_pcs = SPI_PCS0;
			spi_master_init_config.spi_transfer_width = SPI_TWO_BIT;
			spi_master_init(SPI_INSTANCE0, &spi_master_init_config);
			spi_master_transfer(SPI_INSTANCE0, master_data_send, NULL, BUFFER_SIZE);
			break;
		case 2:
			spi_master_init_config.which_pcs = SPI_PCS0;
			spi_master_init_config.spi_transfer_width = SPI_FOUR_BIT;
			spi_master_init(SPI_INSTANCE0, &spi_master_init_config);
			spi_master_transfer(SPI_INSTANCE0, master_data_send, NULL, BUFFER_SIZE);
			break;
		case 3:
			spi_slave_init_config.which_pcs = SPI_PCS0;
			spi_slave_init_config.spi_transfer_width = SPI_TWO_BIT;
			spi_slave_init(SPI_INSTANCE0, &spi_slave_init_config);
			spi_slave_transfer(SPI_INSTANCE0, NULL, slave_data_receive, BUFFER_SIZE);
			delay_ms(5000);
            for(uint32_t i = 0U; i < BUFFER_SIZE; i++)
            {
                //INFO("slaver_data_receive[%d]=%#d--master_data_send[%d]=%#d\n", i, slave_data_receive[i], i, master_data_send[i]);
                if (master_data_send[i] != slave_data_receive[i])
				{
					INFO("{test_fail!}");
					return;
				}
            }
            INFO("\n");
			INFO("{test_ok!}");
			break;
		case 4:
			spi_slave_init_config.which_pcs = SPI_PCS0;
			spi_slave_init_config.spi_transfer_width = SPI_FOUR_BIT;
			spi_slave_init(SPI_INSTANCE0, &spi_slave_init_config);
			spi_slave_transfer(SPI_INSTANCE0, NULL, slave_data_receive, BUFFER_SIZE);
			delay_ms(5000);
            for(uint32_t i = 0U; i < BUFFER_SIZE; i++)
            {
                //INFO("slaver_data_receive[%d]=%#d--master_data_send[%d]=%#d\n", i, slave_data_receive[i], i, master_data_send[i]);
                if (master_data_send[i] != slave_data_receive[i])
				{
					INFO("{test_fail!}");
					return;
				}
            }
            INFO("\n");
			INFO("{test_ok!}");
			break;
		default:
			break;
	}
}

static void spi_interrupt_cd_test_callback(void *driver_state, spi_event_e event, void *user_data)
{
	if (event == SPI_EVENT_RECEIVE_ERROR)
	{
		INFO("{spi_slave_overrun_occur!}\n");
	}

	if (event == SPI_EVENT_WORD_COMPLETE)
	{
		INFO("{spi_slave_word_complete_occur!}\n");
	}

	if (event == SPI_EVENT_FRAME_COMPLETE)
	{
		INFO("{spi_slave_frame_complete_occur!}\n");
	}
}		

static void spi_interrupt_cd_test_irq_handler(spi_instance_e spi_instance)
{
	#if 0
	
	SPI_TypeDef *base = spi_base[spi_instance];
	uint8_t transimit_words = (uint8_t)base->FSR.TFF_CNT;
	uint8_t available_space = (uint8_t)(4U - transimit_words);
	while(available_space != 0U)
	{
		base->TDR = 0x12345678;
		available_space = (uint8_t)(available_space - 1U);
	}

	if (spi_drv_status_flag_get(base, SPI_RECEIVE_ERROR))
	{
		spi_drv_status_flag_clear(base, SPI_ALL_STATUS);
		spi_drv_soft_reset(base);
		spi_interrupt_handle_cfg(SPI_INSTANCE0, NULL);
		INFO("{spi_slave_overrun_occur!}\n");
	}
	
	#endif
}

void spi_interrupt_cd_test(uint32_t test_case)
{
	spi_reset_default_config();
	spi_reset_buffer();
	spi_clk_init(SPI_INSTANCE0);
	temp_spi_gpio_init();

	switch(test_case)
	{
		case 1:
			spi_slave_init(SPI_INSTANCE0, &spi_slave_init_config);
			spi_interrupt_handle_cfg(SPI_INSTANCE0, spi_interrupt_cd_test_irq_handler);
			spi_slave_transfer(SPI_INSTANCE0, slave_data_send, slave_data_receive, BUFFER_SIZE);
			break;
		case 2:
			spi_slave_init(SPI_INSTANCE0, &spi_slave_init_config);
			spi_callback_install(SPI_INSTANCE0, spi_interrupt_cd_test_callback, NULL);
			spi_interrupt_source_enable(SPI_INSTANCE0, SPI_FRAME_COMPLETE, true);
			spi_slave_transfer(SPI_INSTANCE0, slave_data_send, slave_data_receive, BUFFER_SIZE);
			break;
		case 3:
			spi_slave_init(SPI_INSTANCE0, &spi_slave_init_config);
			spi_callback_install(SPI_INSTANCE0, spi_interrupt_cd_test_callback, NULL);
			spi_interrupt_source_enable(SPI_INSTANCE0, SPI_WORD_COMPLETE, true);
			spi_slave_transfer(SPI_INSTANCE0, slave_data_send, slave_data_receive, BUFFER_SIZE);
			break;
		default:
			break;
	}
}

void spi_interrupt_dma_request_all_instance_cd_test(uint32_t spi_instance, bool use_interrupt)
{
    spi_reset_default_config();
    spi_reset_buffer();
    spi_clk_init((spi_instance_e)spi_instance);
    spi_master_gpio_init((spi_instance_e)spi_instance, SPI_MUX_SEL0, &spi_master_init_config);

    
	spi_master_init_config.pin_cfg = SPI_SDO_IN_OUT;
    spi_master_init((spi_instance_e)spi_instance, &spi_master_init_config);
    spi_master_transfer((spi_instance_e)spi_instance, master_data_send, master_data_receive, BUFFER_SIZE);

    delay_ms(100);

    for(uint32_t i = 0U; i < BUFFER_SIZE; i++)
    {
        if(master_data_receive[i] != master_data_send[i])
        {
            INFO("{test_fail!}");
            return;
        }
    }
    INFO("{test_ok!}");
}

#endif
