#include "platform_common.h"

#include "test.h"

#include "i2c_reg.h"
#include "i2c.h"
#include "i2c_hal.h"
#include "i2c_test.h"
#include "cmdline_internal.h"

#include "clock_type.h"
#include "gpio_hal.h"

void i2c_func1_test(uint32_t test_case, uint32_t para1, uint32_t para2)
{
}

void i2c_func2_test(uint32_t test_case, uint32_t para1, uint32_t para2)
{
}

#define I2C_PIN I2C0_SEL1
/** Table of base addresses for I2C instances. */
//static I2C_TypeDef *const i2c_bases[I2C_INSTANCE_MAX_COUNT] = I2C_BASE_ADDRS;
//static bool g_random_test_flag = true;




i2c_master_config_t g_i2cm_config =
    {
        .master_mode = I2C_FAST_MODE,
        .i2c_master_pin_config = I2C_2PIN_OPEN_DRAIN,

        .i2c_speed = 300000UL,
        .i2c_high_speed = 0u,
        .master_code = 0x00,

        .master_int_handle = i2c_master_int_handle_test,
        .master_interrupt_priority = 0,
};

i2c_master_msg_t g_i2c_master_msg =		      //use
    {
        .addr_10bit = false,
        .slave_addr = 0x17,
		.slave_reg_addr = 0x0,

        .send_data = NULL,
        .rec_data = NULL,

        .rec_data_len = 8u,
        .send_data_len = 8u,
};

i2c_slave_config_t g_i2cs_config =
    {
        .slave_mode = I2C_FAST_MODE,

        .addr_10bit = false,
        .addr_config = I2C_ADDR_MATCH_0_7BIT,
        .slave_addr0 = 0x17,
        .slave_addr1 = 0x33,

        .slave_int_handle = NULL,
        .slave_interrupt_priority = 0,
#if FEATURE_HAS_DMA_ENABLE
        .slave_use_dma = false,
        .tx_channel = DMA_CH_1,
        .rx_channel = DMA_CH_0,
#endif
};

i2c_slave_msg_t g_i2c_slave_msg =
    {
        .rec_data_len = 8U,
        .send_data_len = 8u,
};

#if FEATURE_HAS_DMA_ENABLE
uint32_t g_i2c_dma_rx_buf[8u]; // AHB_SRAM_ATTRIBUTES;

uint32_t g_i2c_dma_tx_buf[] =
    {
        (uint32_t)(0u << 8U) + (uint32_t)0x1,
        (uint32_t)(0u << 8U) + (uint32_t)0x2,
        (uint32_t)(0u << 8U) + (uint32_t)0x3,
        (uint32_t)(0u << 8U) + (uint32_t)0x1,
        (uint32_t)(0u << 8U) + (uint32_t)0x2,
        (uint32_t)(0u << 8U) + (uint32_t)0x3,
        (uint32_t)(0u << 8U) + (uint32_t)0x1,
        (uint32_t)(0u << 8U) + (uint32_t)0x2,
};

i2c_dma_config_t g_i2c_rx_dma_cfg =
    {
        .i2c_dma_req = I2C1_DMA_RX,

        .i2c_dma_sar = 0u,
        .i2c_dma_dar = 0u,
        .dma_bcr = 32u,
#if FEATURE_HAS_DMA_ENABLE
        .i2c_used_dma_instance = DMA_INSTANCE0,
        .i2c_used_dma_channel = DMA_CH_0,

        .i2c_dma_interrupt_handle = NULL,
#endif
};

i2c_dma_config_t g_i2c_tx_dma_cfg =
    {
        .i2c_dma_req = I2C1_DMA_TX,

        .i2c_dma_sar = 0u,
        .i2c_dma_dar = 0u,
        .dma_bcr = 32u,
#if FEATURE_HAS_DMA_ENABLE
        .i2c_used_dma_instance = DMA_INSTANCE0,
        .i2c_used_dma_channel = DMA_CH_1,

        .i2c_dma_interrupt_handle = NULL,
#endif
};
#endif

bool g_py02_unlock = false;
bool g_i2c_start_read = true;
bool g_i2c_rx_finish = false;
bool g_i2c_tx_finish = false;
bool g_i2c_pressure_test_finish = false;


void i2c0_gpio_init()      //use
{
    gpio_settings_config_t gpio_settings_config;

    /* J3B54 I2C0_SCL PA6*/
    gpio_settings_config.gpio_port = GPIO_A;
    gpio_settings_config.gpio_pin = GPIO_PIN_6;
    gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_4;
    gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
    gpio_settings_config.gpio_strength = GPIO_STRENGTH_MODE_1;
    gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&gpio_settings_config);

    /* J2A18 I2C0_SDA PA5 */
    gpio_settings_config.gpio_port = GPIO_A;
    gpio_settings_config.gpio_pin = GPIO_PIN_5;
    gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_4;
    gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
    gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
    gpio_settings_config.gpio_strength = GPIO_STRENGTH_MODE_1;
    gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&gpio_settings_config);
}

void i2c1_gpio_init()      //use
{

}

void gpio_a2_init(void)
{
    // gpio_settings_config_t gpio_settings_config;

    // gpio_settings_config.gpio_port = GPIO_A;
    // gpio_settings_config.gpio_pin = GPIO_PIN_2;
    // gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_1;
    // gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
    // gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
    // gpio_settings_config.gpio_strength = GPIO_STRENGTH_HIGH_FAST;
    // gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
    // gpio_init(&gpio_settings_config);
}

#if FEATURE_HAS_DMA_ENABLE
/*i2c DMA mode print array*/
void print_dma_array(char *log_header, void *data, uint32_t len)
{
    uint8_t *tmp = (uint8_t *)data;

    INFO("%s:\n[", log_header);

    for (uint32_t i = 0; i < len; i++)
    {
        INFO("0x%02x ", tmp[i * 4]);
    }
    INFO("]\n");
}
#endif

/*i2c inthandle mode print array*/
void print_int_array(char *log_header, void *data, uint32_t len)
{
    uint8_t *tmp = (uint8_t *)data;

    INFO("%s:\n[", log_header);

    for (uint32_t i = 0; i < len; i++)
    {
        INFO("0x%02x ", tmp[i]);
    }
    INFO("]\n");
}

#if 0
static void gpioa2_toggle(void)
{
    // gpio_toggle(GPIO_A, GPIO_PIN_2);
    // gpio_toggle(GPIO_A, GPIO_PIN_2);
}
#endif

/**
 *@brief Initialize clock for i2c module.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@return    none
 */
static void i2c_clock_init(i2c_instance_e i2c_instance)
{
#if 1
	module_clk_cfg_t module_clk_cfg;
	uint32_t i2c_clk = 0u;

	const module_name_e clock_module_name[I2C_INSTANCE_MAX_COUNT] = {MODULE_I2C0, MODULE_I2C1};

	module_clk_cfg.module_name = clock_module_name[i2c_instance];

	module_clk_cfg.fclk_sel = APB_CLK;
	module_clk_cfg.fclk_div = PER_DIVIDE_1;
	module_clk_cfg.pclk_dsleep_gating_en = GATING_ON;
	module_clk_cfg.fclk_dsleep_gating_en = GATING_ON;

	clock_module_clk_cfg(&module_clk_cfg);
	clock_freq_get(module_clk_cfg.module_name, &i2c_clk);
#endif
}

/**
 *@brief Initialize the i2c SCL&SDA port.
 *@param[in] i2c_port_sel: gpio pin for i2c
 *@return    none
 */
void i2c_gpio_init(i2c_port_sel_e i2c_port_sel)
{
#if 0
	gpio_settings_config_t gpio_settings_config;
	
	switch (i2c_port_sel)
	{
		case I2C0_SEL0:
			/* I2C0_SCL1: PTA3, ALT:3 47*/
			gpio_settings_config.gpio_port = GPIO_A;
			gpio_settings_config.gpio_pin = GPIO_PIN_3;
			gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_3;
			gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
			gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
			gpio_settings_config.gpio_strength = GPIO_STRENGTH_HIGH_FAST;
			gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
			gpio_init(&gpio_settings_config);
			
			/* I2C0_SDA1: PTA2, ALT:3 48*/
			gpio_settings_config.gpio_port = GPIO_A;
			gpio_settings_config.gpio_pin = GPIO_PIN_2;
			gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_3;
			gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
			gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
			gpio_settings_config.gpio_strength = GPIO_STRENGTH_HIGH_FAST;
			gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
			gpio_init(&gpio_settings_config);
			break;
		case I2C0_SEL1:
			/* I2C0_SCL0: PTC3, ALT:6 20*/
			gpio_settings_config.gpio_port = GPIO_C;
			gpio_settings_config.gpio_pin = GPIO_PIN_3;
			gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_6;
			gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
			gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
			gpio_settings_config.gpio_strength = GPIO_STRENGTH_HIGH_FAST;
			gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
			gpio_init(&gpio_settings_config);
			
			/* I2C0_SDA0: PTC2, ALT:6 21*/
			gpio_settings_config.gpio_port = GPIO_C;
			gpio_settings_config.gpio_pin = GPIO_PIN_2;
			gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_6;
			gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
			gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
			gpio_settings_config.gpio_strength = GPIO_STRENGTH_HIGH_FAST;
			gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
			gpio_init(&gpio_settings_config);
			break;
		case I2C1_SEL0:
			/* I2C1_SCL0: PTD5, ALT:5 */
			gpio_settings_config.gpio_port = GPIO_D;
			gpio_settings_config.gpio_pin = GPIO_PIN_5;
			gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_5;
			gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
			gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
			gpio_settings_config.gpio_strength = GPIO_STRENGTH_HIGH_FAST;
			gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
			gpio_init(&gpio_settings_config);
			
			/* I2C1_SDA0: PTC1, ALT:5 */
			gpio_settings_config.gpio_port = GPIO_C;
			gpio_settings_config.gpio_pin = GPIO_PIN_1;
			gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_5;
			gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
			gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
			gpio_settings_config.gpio_strength = GPIO_STRENGTH_HIGH_FAST;
			gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
			gpio_init(&gpio_settings_config);
			break;
		case I2C1_SEL1:
			/* I2C1_SCL1: PTE1, ALT:4  93 */
			gpio_settings_config.gpio_port = GPIO_E;
			gpio_settings_config.gpio_pin = GPIO_PIN_1;
			gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_4;
			gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
			gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
			gpio_settings_config.gpio_strength = GPIO_STRENGTH_HIGH_FAST;
			gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
			gpio_init(&gpio_settings_config);
			
			/* I2C1_SDA1: PTE0, ALT:4  94 */
			gpio_settings_config.gpio_port = GPIO_E;
			gpio_settings_config.gpio_pin = GPIO_PIN_0;
			gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_4;
			gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
			gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
			gpio_settings_config.gpio_strength = GPIO_STRENGTH_HIGH_FAST;
			gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
			gpio_init(&gpio_settings_config);
			break;	
		default:
			return;
	}
#endif
}

/**
 *@brief Initialize the i2c master host request port, i2c master will trigger by gpio if I2CM_CFG0[HOST_REQ_EN] is true.
         host request input is pin HREQ
 *@param[in] i2c_port_sel: gpio pin for i2c
 *@return    none
 */
void i2c_hreq_gpio_init(i2c_port_sel_e i2c_port_sel)
{
#if 0
	gpio_settings_config_t gpio_settings_config;
	
	switch (i2c_port_sel)
	{
		case I2C0_SEL0:
			/* I2C0_HREQ0: PTB11, ALT:3 */
			gpio_settings_config.gpio_port = GPIO_E;
			gpio_settings_config.gpio_pin = GPIO_PIN_1;
			gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_3;
			gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
			gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
			gpio_settings_config.gpio_strength = GPIO_STRENGTH_HIGH_FAST;
			gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
			gpio_init(&gpio_settings_config);
			break;
		case I2C1_SEL0:
			/* I2C0_HREQ1: PTE1, ALT:3 */
			gpio_settings_config.gpio_port = GPIO_C;
			gpio_settings_config.gpio_pin = GPIO_PIN_5;
			gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_4;
			gpio_settings_config.gpio_dir = GPIO_DIR_INPUT;
			gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
			gpio_settings_config.gpio_strength = GPIO_STRENGTH_HIGH_FAST;
			gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
			gpio_init(&gpio_settings_config);
			break;
		default:
			return;
	}
#endif
}

/**
 *@brief Initialize the i2c SCL&SDA port.
 *@param[in] i2c_instance_e: i2c_instance_e
 *@return    none
 */
void i2c_cd_gpio_init(i2c_instance_e i2c_instance)
{
}

#if 0
static void i2c_master_pressure_handle(i2c_event_e master_event)
{

	switch (master_event)
	{
		case I2C_TX_FINISH:

			break;
		case I2C_RX_FINISH:
			/* re-initialize buffer if need to continue to receive */
//			print_int_array("master rx", g_i2c_master_msg.rec_data, g_i2c_master_msg.rec_data_len);
//			print_int_array("master tx", g_i2c_master_msg.send_data, g_i2c_master_msg.send_data_len);
			for (uint32_t i=0; i<g_i2c_master_msg.rec_data_len; i++)
			{
				if ((uint8_t)g_i2c_master_msg.rec_data[i] != (uint8_t)(~(g_i2c_master_msg.send_data[i])))
				{
					g_random_test_flag = false;
				}
			}
			if (g_random_test_flag == false)
			{
				INFO("test {fail}! \n");
				g_random_test_flag = true;
			}
			else
			{
				INFO("test {success}! \n");
			}

			i2cm_buffer_init(&g_i2c_master_msg);
			break;
		case I2C_ERROR:
			INFO("cmd_i2c_test set {fail} $$\n");
			/* re-initialize buffer */
			i2cm_buffer_init(&g_i2c_master_msg);
			break;
		case I2C_NO_EVENT:
		default:
			break;
	}

}
#endif

#if 0
static void i2c_slave_pressure_handle(i2c_event_e slave_event)
{

	switch (slave_event)
	{
		case I2C_TX_FINISH:
//            INFO("slave tx finish {tx_success} $$\n");
			/* re-initialize buffer or update tx if need to continue to transmit */
			i2cs_buffer_init(&g_i2c_slave_msg);
			break;
		case I2C_RX_FINISH:
			/* process received data */
//			print_int_array("slave rx", g_i2c_slave_msg.rec_data, g_i2c_slave_msg.rec_data_len);
			for (uint32_t i=0; i<g_i2c_slave_msg.rec_data_len; i++)
			{
				g_i2c_slave_msg.send_data[i] = (~g_i2c_slave_msg.rec_data[i]);
			}
			/* re-initialize buffer if need to continue to receive */
			i2cs_buffer_init(&g_i2c_slave_msg);
			break;
		case I2C_ERROR:
			INFO("slave error\n");
			break;
		case I2C_NO_EVENT:
		default:
			break;
	}
}
#endif

void i2c_master_int_handle_test(i2c_event_e master_event)
{
#if 0
	switch (master_event)
	{
		case I2C_TX_FINISH:
			if (g_i2c_start_read == true)
			{
				//if need to restart, user should comment i2cm_end_transfer() in tx int_handle
				//which will send stop cmd.
//				delay_1us(100);
//      i2cm_receive_data(I2C_INSTANCE1, &g_i2c_master_msg);
				INFO("cmd_i2c_test set {write_success} $$\n");
			}
			else
			{
				i2cm_end_transfer(I2C_INSTANCE1, true, false);
			}
			break;
		case I2C_RX_FINISH:
			/* re-initialize buffer if need to continue to receive */
			i2cm_buffer_init(&g_i2c_master_msg);
			print_int_array("master rx", g_i2c_master_msg.rec_data, g_i2c_master_msg.rec_data_len);
                        INFO("cmd_i2c_test set {read_success} $$\n");
			break;
		case I2C_ERROR:
			INFO("i2c master fifo error {fail} $$\n");
			/* re-initialize buffer */
			i2cm_buffer_init(&g_i2c_master_msg);
			break;
                case I2C_DATA_MATCH:
			INFO("{data match}\n");
			break;
                case I2C_PIN_LOW:
                        INFO(" {enter pin low} \n");
			break;
		case I2C_NO_EVENT:
		default:
			break;
	}
#endif
}

void i2c_slave_int_handle_test(i2c_event_e slave_event)
{
#if 0
	switch (slave_event)
	{
		case I2C_TX_FINISH:
                        INFO("slave tx finish {tx_success} $$\n");
			/* re-initialize buffer or update tx if need to continue to transmit */
			i2cs_buffer_init(&g_i2c_slave_msg);
			break;
		case I2C_RX_FINISH:
			/* process received data */
#ifdef UART
			print_int_array("slave rx", g_i2c_slave_msg.rec_data, g_i2c_slave_msg.rec_data_len);
#endif
			/* re-initialize buffer if need to continue to receive */
			i2cs_buffer_init(&g_i2c_slave_msg);
			break;
		case I2C_ERROR:
			INFO("slave fifo error int\n");;
			break;
                case I2C_SMBUS_ALERT:
                        INFO("{smbus}\n");
			break;
                case I2C_GENERAL_CALL:
                        INFO("{gen_call}\n");
			break;
                case I2C_ADDR_MATCH:
                        INFO("{addmatch}\n");
			break;
		case I2C_NO_EVENT:
		default:
			break;
	}
#endif
}

void i2c_master_write_test(i2c_instance_e i2c_instance)
{
#if 0
	{
		/* 3) master send */
		for (uint32_t i=0; i<g_i2c_master_msg.send_data_len; i++)
		{
			g_i2c_master_msg.send_data[i] = i;
			g_i2c_master_msg.rec_data[i] = 0U;
		}
		i2cm_send_data(i2c_instance, &g_i2c_master_msg);
	}
#endif
}

void i2c_master_randomnum_trans_test(i2c_instance_e i2c_instance)
{
#if 0
		i2c_clock_init(i2c_instance);
		i2c_cd_gpio_init(i2c_instance);

    g_i2cm_config.master_int_handle = i2c_master_pressure_handle;
    i2c_master_init(i2c_instance, &g_i2cm_config);

    for (uint32_t i=0; i<100; i++)
    {
    	for (uint32_t j=0; j<g_i2c_master_msg.send_data_len; j++)
    	{
    		g_i2c_master_msg.send_data[j] = rand()%256;
    	}
    	i2cm_send_data(i2c_instance, &g_i2c_master_msg);
    	delay_ms(10);
    	i2cm_receive_data(i2c_instance, &g_i2c_master_msg);
    	delay_ms(10);
    }
#endif
}

void i2c_slave_randomnum_trans_test(i2c_instance_e i2c_instance)
{
#if 0
        i2c_clock_init(i2c_instance);
	i2c_cd_gpio_init(i2c_instance);   
	
	/* init slave to receive data */
	for (uint32_t i=0; i<8; i++)
	{
		g_i2c_slave_msg.send_data[i] = i;
		g_i2c_slave_msg.rec_data[i] = 0U;
	}
	g_i2cs_config.slave_int_handle = i2c_slave_pressure_handle;

	i2c_slave_init(i2c_instance, &g_i2cs_config, &g_i2c_slave_msg);
#endif
}
void i2c_master_mode_handle_test(i2c_instance_e i2c_instance)
{
		i2c_clock_init(i2c_instance);
		//i2c_cd_gpio_init(i2c_instance);
        
		g_i2cm_config.master_int_handle = i2c_master_int_handle_test;
//		g_i2cm_config.i2c_master_pin_config = I2C_2PIN_PUSH_PULL;
		i2c_master_init(i2c_instance, &g_i2cm_config);
						
//        g_i2c_master_msg.rec_data_len = I2C_DATALEN_TEST;
//        
//        i2c_master_write_test(i2c_instance);

}

void i2c_master_hreq_pin_test(i2c_instance_e i2c_instance, uint32_t polarity)
{
#if 0
	i2c_clock_init(i2c_instance);
//		/* hreq should be enable*/
//	i2c_hreq_gpio_init(I2C1_SEL1);
	i2c_cd_gpio_init(i2c_instance);
        
        i2c_master_init(i2c_instance, &g_i2cm_config);

        I2C_TypeDef *i2c_ptr = i2c_bases[i2c_instance];

        if (i2c_ptr != NULL)
        {       
                if (1 == polarity)
                {
                      i2c_ptr->I2CM_CFG0.HOST_REQ_POLARITY = 1U;
                }
                else
                {
                      i2c_ptr->I2CM_CFG0.HOST_REQ_POLARITY = 0U;
                }
                i2c_ptr->I2CM_CFG0.HOST_REQ_SEL = 0U;
                i2c_ptr->I2CM_CFG0.HOST_REQ_EN = FUNC_ENABLE;
        }
#endif
}

void i2c_master_trgmux_trigger_test(i2c_instance_e i2c_instance, uint8_t trg_src)
{
#if 0
	i2c_clock_init(i2c_instance);
//		/* hreq should be enable*/
	i2c_cd_gpio_init(i2c_instance);
        
        i2c_master_init(i2c_instance, &g_i2cm_config);
        I2C_TypeDef *i2c_ptr = i2c_bases[i2c_instance];

        if (i2c_ptr != NULL)
        {
                i2c_ptr->I2CM_CFG0.HOST_REQ_POLARITY = 1U;
                i2c_ptr->I2CM_CFG0.HOST_REQ_SEL = 1U;
                i2c_ptr->I2CM_CFG0.HOST_REQ_EN = FUNC_ENABLE;
        }
        for (int i=0; i<4; i++)           
        {   
                i2c_master_write_test(i2c_instance);
                trgmux_i2c_test(trgmux_i2c_input[i2c_instance], trg_source[trg_src]);
                delay_ms(1000);
        }
#endif
}

void i2c_master_gpt_trigger_test(i2c_instance_e i2c_instance, uint32_t e_flag)
{
#if 0
	i2c_clock_init(i2c_instance);
//	/* hreq should be enable*/
	i2c_cd_gpio_init(i2c_instance);
        
        i2c_master_init(i2c_instance, &g_i2cm_config);
	
        I2C_TypeDef *i2c_ptr = i2c_bases[i2c_instance];

        if (i2c_ptr != NULL)
        {
                i2c_ptr->I2CM_CFG0.HOST_REQ_POLARITY = 0U;
                i2c_ptr->I2CM_CFG0.HOST_REQ_SEL = 1U;
                i2c_ptr->I2CM_CFG0.HOST_REQ_EN = FUNC_ENABLE;
        }
        i2c_master_write_test(i2c_instance);
        delay_ms(100);
        
				trgmux_out_cfg(TRGMUX_OUT0, TRGMUX_TRGOUT_GPT0_HWTRIG_OUT);
        trgmux_output_gpio_init(TRGMUX_OUT_SEL_0_5);               //PTE11
        for (int i=13; i<15; i++)
        {
                if (1 == e_flag)
                {
                      trgmux_i2c_test(trgmux_i2c_input[i2c_instance], trg_source[i]);   
                }
                i2c_master_write_test(i2c_instance);
                delay_ms(100);
        }
#endif
}

void i2c_master_trgt_trigger_test(i2c_instance_e i2c_instance, uint32_t s_ic)
{
#if 0
    uint32_t instance = 0u;
	uint32_t channel = 0u;
        
	i2c_clock_init(i2c_instance);
//		/* hreq should be enable*/
	i2c_cd_gpio_init(i2c_instance);
        
        i2c_master_init(i2c_instance, &g_i2cm_config);

        I2C_TypeDef *i2c_ptr = i2c_bases[i2c_instance];

        if (i2c_ptr != NULL)
        {
                i2c_ptr->I2CM_CFG0.HOST_REQ_POLARITY = 1U;
                i2c_ptr->I2CM_CFG0.HOST_REQ_SEL = 1U;
                i2c_ptr->I2CM_CFG0.HOST_REQ_EN = FUNC_ENABLE;
        }
        i2c_master_write_test(i2c_instance);
        delay_ms(100);
        
        if (0 == s_ic)
        {
              i2c_master_write_test(i2c_instance);
              delay_ms(100);
        }
        else
        {
              instance = ((s_ic-1)/4u - 2);
              channel = ((s_ic-1)%4u);
					
              trgmux_i2c_test(trgmux_i2c_input[i2c_instance], trg_source[s_ic]);
              trgt_timer_mode_init((trgt_instance_e)instance, (trgt_channel_e)channel, 5000, gpioa2_toggle);
              i2c_master_write_test(i2c_instance);
              delay_ms(100);
        }
#endif
}

status_t i2c_master_dmatch_test(i2c_instance_e i2c_instance,
                                i2c_master_dmatch_config_e datamatch_cfg)
{
	i2c_clock_init(i2c_instance);
	//i2c_cd_gpio_init(i2c_instance);   
  
	status_t ret = STATUS_SUCCESS;
	i2c_master_dmatch_config_t dmatch_config = 
	{
		.data0 = 0x12,
		.data1 = 0x34,
		
		.dmatch_cfg = datamatch_cfg,
	};

	/* 1) init i2c master first */
	ret = i2c_master_init(i2c_instance, &g_i2cm_config);
	
	if (ret != STATUS_SUCCESS)
	{
		INFO("ret:%d\n",ret);
	}
	else
	{
		i2cm_dmatch_init(i2c_instance, &dmatch_config);
		INFO("dmatch init done\n");
	}

	return ret;
}

void i2c_master_10address_test(i2c_instance_e i2c_instance)
{
#if 0
	i2c_clock_init(i2c_instance);
	i2c_cd_gpio_init(i2c_instance);   
        
        g_i2cm_config.master_int_handle = i2c_master_int_handle_test;
        i2c_master_init(i2c_instance, &g_i2cm_config);
                
        g_i2c_master_msg.addr_10bit = true;
        g_i2c_master_msg.slave_addr = 0x170;

        i2c_master_write_test(i2c_instance);
        
//        i2c_master_write_test(i2c_instance);
#endif
}

void i2c_master_pinlow_test(i2c_instance_e i2c_instance)
{
#if 0  
	i2c_clock_init(i2c_instance);
	i2c_cd_gpio_init(i2c_instance);   
        
        g_i2cm_config.master_int_handle = i2c_master_int_handle_test;
        i2c_master_init(i2c_instance, &g_i2cm_config);
        I2C_TypeDef *i2c_ptr = i2c_bases[i2c_instance];
        if (i2c_ptr != NULL)
        {
                i2c_ptr->I2CM_CFG3.PINLOW = 1000U;
                i2cm_drv_int_cfg(i2c_ptr, I2C_MASTER_PIN_LOW_TIMEOUT_INT, true);
        }
        
        
        i2c_master_write_test(i2c_instance);
        delay_ms(1000);
#endif
}

void i2c_master_autostop_test(i2c_instance_e i2c_instance, uint32_t t_case)
{
#if 0  
	i2c_clock_init(i2c_instance);
	i2c_cd_gpio_init(i2c_instance);   
        
        g_i2cm_config.master_int_handle = i2c_master_int_handle_test;
        i2c_master_init(i2c_instance, &g_i2cm_config);
        I2C_TypeDef *i2c_ptr = i2c_bases[i2c_instance];

        if (i2c_ptr != NULL)
        {
                if (0U == t_case)
                {
                     i2c_ptr->I2CM_CFG1.AUTOSTOP = FUNC_ENABLE;
                     i2c_ptr->I2CM_CFG0.CIRFIFO = FUNC_ENABLE; 
                }
                else
                {
                      i2c_ptr->I2CM_CFG1.AUTOSTOP = FUNC_DISABLE;
                      i2c_ptr->I2CM_CFG0.CIRFIFO = FUNC_ENABLE; 
                }
                
        }
        
        delay_ms(100);
//        i2c_master_write_test(i2c_instance);
#endif
}

void i2c_slave_test(i2c_instance_e i2c_instance)
{
#if 0 
	status_t ret = STATUS_SUCCESS;
        
        i2c_clock_init(i2c_instance);
	i2c_cd_gpio_init(i2c_instance);   
	
	if (g_i2cs_config.addr_10bit == true)
	{
					g_i2cs_config.addr_config = I2C_ADDR_MATCH_0_10BIT;
	}
	else
	{
					g_i2cs_config.addr_config = I2C_ADDR_MATCH_0_7BIT;
	}
        
	/* init slave to receive data */
	for (uint32_t i=0; i<8; i++)
	{
		g_i2c_slave_msg.send_data[i] = i;
		g_i2c_slave_msg.rec_data[i] = 0U;
	}
	g_i2cs_config.slave_int_handle = i2c_slave_int_handle_test;
	

	ret = i2c_slave_init(i2c_instance, &g_i2cs_config, &g_i2c_slave_msg);
	if (ret != STATUS_SUCCESS)
	{
		INFO("ret:%d\n",ret);
	}
#endif
}

void i2c_slave_randomread_test(i2c_instance_e i2c_instance)
{
#if 0
	status_t ret = STATUS_SUCCESS;
	
        uint32_t offset = g_i2c_slave_msg.rec_data[0];
        
	/* init slave to receive data */
	for (uint32_t i=0; i<g_i2c_slave_msg.send_data_len; i++)
	{
                uint32_t tem_off = i/4;
                uint32_t tem_bit = 8*(i%4);
                uint32_t temp = *((uint32_t *)(TCM_IRAM0_BASE)+offset+tem_off);
		g_i2c_slave_msg.send_data[i] = temp>>tem_bit;
//                INFO("data = 0x%02x\n", g_i2c_slave_msg.send_data[i]);
	}

	ret = i2c_slave_init(i2c_instance, &g_i2cs_config, &g_i2c_slave_msg);
	if (ret != STATUS_SUCCESS)
	{
		INFO("ret:%d\n",ret);
	}
#endif
}
#if 0
void i2c_slave_addrmatch_test(i2c_instance_e i2c_instance,
                              i2c_slave_addr_config_e addr_cfg,
                              uint32_t addr0, uint32_t addr1)
{
	status_t ret = STATUS_SUCCESS;
        
    i2c_clock_init(i2c_instance);
	//i2c_cd_gpio_init(i2c_instance);   
	
	/* init slave to receive data */
	for (uint32_t i=0; i<8; i++)
	{
		g_i2c_slave_msg.send_data[i] = i;
		g_i2c_slave_msg.rec_data[i] = 0U;
	}

        g_i2cs_config.slave_int_handle = i2c_slave_int_handle_test;
        
        g_i2cs_config.addr_config = addr_cfg;
        g_i2cs_config.slave_addr0 = addr0;
        g_i2cs_config.slave_addr1 = addr1;
        
	ret = i2c_slave_init(i2c_instance, &g_i2cs_config, &g_i2c_slave_msg);
	if (ret != STATUS_SUCCESS)
	{
		INFO("ret:%d\n",ret);
	}
        
        I2C_TypeDef *i2c_ptr = i2c_bases[i2c_instance];

        if (i2c_ptr != NULL)
        {
                i2cs_drv_int_cfg(i2c_ptr, I2C_SLAVE_ADDRESS_MATCH_0_INT, true);
                i2cs_drv_int_cfg(i2c_ptr, I2C_SLAVE_ADDRESS_MATCH_1_INT, true);
        }
}
#endif

void i2c_slave_generalcall_test(i2c_instance_e i2c_instance)
{
#if 0
	status_t ret = STATUS_SUCCESS;
        
        i2c_clock_init(i2c_instance);
	i2c_cd_gpio_init(i2c_instance);   
	
	/* init slave to receive data */
	for (uint32_t i=0; i<8; i++)
	{
		g_i2c_slave_msg.send_data[i] = i;
		g_i2c_slave_msg.rec_data[i] = 0U;
	}

        g_i2cs_config.slave_int_handle = i2c_slave_int_handle_test;
        
	ret = i2c_slave_init(i2c_instance, &g_i2cs_config, &g_i2c_slave_msg);
	if (ret != STATUS_SUCCESS)
	{
		INFO("ret:%d\n",ret);
	}
        
        I2C_TypeDef *i2c_ptr = i2c_bases[i2c_instance];

        if (i2c_ptr != NULL)
        {
                i2cs_drv_int_cfg(i2c_ptr, I2C_SLAVE_GENERAL_CALL_INT, true);
                i2c_ptr->I2CS_CFG0.GENERAL_CALL_EN = FUNC_ENABLE;
        }
#endif
}

void i2c_slave_smbus_test(i2c_instance_e i2c_instance)
{
#if 0
	status_t ret = STATUS_SUCCESS;
        
        i2c_clock_init(i2c_instance);
	i2c_cd_gpio_init(i2c_instance);   
	
	/* init slave to receive data */
	for (uint32_t i=0; i<8; i++)
	{
		g_i2c_slave_msg.send_data[i] = i;
		g_i2c_slave_msg.rec_data[i] = 0U;
	}

        g_i2cs_config.slave_int_handle = i2c_slave_int_handle_test;
        
	ret = i2c_slave_init(i2c_instance, &g_i2cs_config, &g_i2c_slave_msg);
	if (ret != STATUS_SUCCESS)
	{
		INFO("ret:%d\n",ret);
	}
        
        I2C_TypeDef *i2c_ptr = i2c_bases[i2c_instance];

        if (i2c_ptr != NULL)
        {
                i2cs_drv_int_cfg(i2c_ptr, I2C_SLAVE_SMBUS_ALERT_RESPONSE_INT, true);
                i2c_ptr->I2CS_CFG0.ADDR_SCL_STALL = FUNC_ENABLE;
                i2c_ptr->I2CS_CFG0.SMBUS_ALERT_EN = FUNC_ENABLE;
        }
#endif
}

void i2c_burnin_test()
{
#if 0
	g_i2cm_config.master_code = I2C_ULTRAFAST_MODE;
	g_i2cm_config.master_int_handle = i2c_master_pressure_handle;
	
	i2c_clock_init(I2C_INSTANCE0);
	i2c_clock_init(I2C_INSTANCE1);
	
	i2c_master_init(I2C_INSTANCE0, &g_i2cm_config);
	i2c_master_init(I2C_INSTANCE1, &g_i2cm_config);
				
	//rx
	g_i2c_master_msg.rec_data_len = I2C_DATALEN_TEST;
	for (uint32_t i=0; i<I2C_DATALEN_TEST; i++)
	{
		g_i2c_master_msg.rec_data[i] = 0U;
	}
			
	i2c_master_write_test(I2C_INSTANCE0);
	i2c_master_write_test(I2C_INSTANCE1);

#endif
}

void i2c_pinmux_test(i2c_instance_e i2c_instance, uint32_t group)
{
#if 0 
		i2c_clock_init(i2c_instance);
		i2c_gpio_init((i2c_port_sel_e)group);
	  i2c_hreq_gpio_init((i2c_port_sel_e)group);
        
		g_i2cm_config.master_int_handle = i2c_master_int_handle_test;
		i2c_master_init(i2c_instance, &g_i2cm_config);
		
		I2C_TypeDef *i2c_ptr = i2c_bases[i2c_instance];

		if (i2c_ptr != NULL)
		{       
				i2c_ptr->I2CM_CFG0.HOST_REQ_POLARITY = 0U;
				i2c_ptr->I2CM_CFG0.HOST_REQ_SEL = 0U;
				i2c_ptr->I2CM_CFG0.HOST_REQ_EN = FUNC_ENABLE;
		}
#endif
}

void i2cs_pinmux_test(i2c_instance_e i2c_instance, uint32_t group)
{
#if 0
	i2c_clock_init(i2c_instance);
	i2c_gpio_init((i2c_port_sel_e)group);
        
	/* init slave to receive data */
	for (uint32_t i=0; i<8; i++)
	{
		g_i2c_slave_msg.send_data[i] = i;
		g_i2c_slave_msg.rec_data[i] = 0U;
	}
	g_i2cs_config.slave_int_handle = i2c_slave_int_handle_test;

	i2c_slave_init(i2c_instance, &g_i2cs_config, &g_i2c_slave_msg);
#endif
}

void i2c_clock_test(i2c_instance_e i2c_instance, bool apb_en, uint32_t pre_div, uint32_t speed)
{
#if 0
	module_clk_cfg_t module_clk_cfg;
	uint32_t i2c_clk = 0u;

	const module_name_e clock_module_name[I2C_INSTANCE_MAX_COUNT] = {MODULE_I2C0, MODULE_I2C1};
	module_clk_cfg.module_name = clock_module_name[i2c_instance];
	module_clk_cfg.fclk_sel = apb_en? APB_CLK : HSC_CLK;
	module_clk_cfg.fclk_div = (peripheral_clk_div_e)pre_div; //PER_DIVIDE_1;
	module_clk_cfg.pclk_dsleep_gating_en = GATING_ON;
	module_clk_cfg.fclk_dsleep_gating_en = GATING_ON;

	clock_module_clk_cfg(&module_clk_cfg);
	clock_freq_get(module_clk_cfg.module_name, &i2c_clk);
	
	i2c_cd_gpio_init(i2c_instance);
	
	g_i2cm_config.master_mode = I2C_FAST_MODE;
	g_i2cm_config.i2c_speed = speed;
	g_i2cm_config.i2c_high_speed = speed;
        
	g_i2cm_config.master_int_handle = i2c_master_int_handle_test;
	i2c_master_init(i2c_instance, &g_i2cm_config);
#endif
}
