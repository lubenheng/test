#include "platform_common.h"

#include "test.h"

#include "gpio_reg.h"
#include "gpio_hal.h"
#include "gpio_test.h"
#include "cmdline_internal.h"


//gpio_test_info_t g_gpio_test_info;
//gpio_output_status_t g_gpio_output_status;

//static const module_name_e g_port_module_name_arr[] = {MODULE_PORTA, MODULE_PORTB, MODULE_PORTC, MODULE_PORTD, MODULE_PORTE};

//bool either_rise_flag;
//bool either_fall_flag;




static void gpio_clk_init(gpio_port_e gpio_port)
{
//	DEV_ASSERT(gpio_port < GPIO_PORT_MAX);
//	
//	module_clk_cfg_t module_clk_cfg;
//	
//	module_clk_cfg.module_name = g_port_module_name_arr[gpio_port];
//	module_clk_cfg.pclk_dsleep_gating_en = GATING_ON;
//	module_clk_cfg.fclk_div = PER_DIVIDE_1;
//	
//	(void)clock_module_clk_cfg(&module_clk_cfg);
}

void gpio_clk_init_all(void)
{
	gpio_clk_init(GPIO_A);
	gpio_clk_init(GPIO_B);
	gpio_clk_init(GPIO_C);
//	gpio_clk_init(GPIO_D);
//	gpio_clk_init(GPIO_E);
}





/**
 *@brief configure the gpio of general gpio
 *@param[in] none 
 *@return    none
 */
void group_a_gpio_toggle_init(void)
{
	gpio_settings_config_t setting_cfg;

	/* GPIO, PTA0 PIN  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_0;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

//    /* GPIO, PTA1 PIN  */
//    setting_cfg.gpio_port = GPIO_A;
//    setting_cfg.gpio_pin = GPIO_PIN_1;
//    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
//    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
//    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
//   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//    gpio_init(&setting_cfg);

//    /* GPIO, PTA2 PIN  */
//    setting_cfg.gpio_port = GPIO_A;
//    setting_cfg.gpio_pin = GPIO_PIN_2;
//    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
//    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
//    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
//   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//    gpio_init(&setting_cfg);

    /* GPIO, PTA3 PIN  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_3;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTA4 PIN  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_4;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTA5 PIN  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_5;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTA6 PIN  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_6;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTA7 PIN  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_7;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
    
    /* GPIO, PTA8 PIN  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_8;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTA9 PIN  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_9;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

   /* GPIO, PTA10 PIN  */
   setting_cfg.gpio_port = GPIO_A;
   setting_cfg.gpio_pin = GPIO_PIN_10;
   setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
   setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
   setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
   setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
  setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
   gpio_init(&setting_cfg);

    /* GPIO, PTA11 PIN  */
   setting_cfg.gpio_port = GPIO_A;
   setting_cfg.gpio_pin = GPIO_PIN_11;
   setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
   setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
   setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
   setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
  setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
   gpio_init(&setting_cfg);

   /* GPIO, PTA12 PIN  */
   setting_cfg.gpio_port = GPIO_A;
   setting_cfg.gpio_pin = GPIO_PIN_12;
   setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
   setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
   setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
   setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
  setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
   gpio_init(&setting_cfg);

   /* GPIO, PTA13 PIN  */
   setting_cfg.gpio_port = GPIO_A;
   setting_cfg.gpio_pin = GPIO_PIN_13;
   setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
   setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
   setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
   setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
  setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
   gpio_init(&setting_cfg);

   /* GPIO, PTA14 PIN  */
   setting_cfg.gpio_port = GPIO_A;
   setting_cfg.gpio_pin = GPIO_PIN_14;
   setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
   setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
   setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
   setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
  setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
   gpio_init(&setting_cfg);

   /* GPIO, PTA15 PIN  */
//    setting_cfg.gpio_port = GPIO_A;
//    setting_cfg.gpio_pin = GPIO_PIN_15;
//    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
//    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
//    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
//   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//    gpio_init(&setting_cfg);

}


/**
 *@brief configure the gpio of general gpio
 *@param[in] none 
 *@return    none
 */
void group_b_gpio_toggle_init(void)
{
    gpio_settings_config_t setting_cfg;

	/* GPIO, PTB0 PIN  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_0;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTB1 PIN  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_1;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTB2 PIN  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_2;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTB3 PIN  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_3;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTB4 PIN  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_4;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTB5 PIN  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_5;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTB6 PIN  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_6;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTB7 PIN  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_7;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
    
    /* GPIO, PTB8 PIN  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_8;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTB9 PIN  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_9;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTB10 PIN  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_10;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

     /* GPIO, PTB11 PIN  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_11;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTB12 PIN  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_12;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTB13 PIN  */
	setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_13;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTB14 PIN  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_14;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTB15 PIN  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_15;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
}

/**
 *@brief configure the gpio of general gpio
 *@param[in] none 
 *@return    none
 */
void group_c_gpio_toggle_init(void)
{
    gpio_settings_config_t setting_cfg;

	/* GPIO, PTC0 PIN  */
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_0;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTC1 PIN  */
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_1;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTC2 PIN  */
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_2;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTC3 PIN  */
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_3;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTC4 PIN  */
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_4;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTC5 PIN  */
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_5;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTC6 PIN  */
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_6;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTC7 PIN  */
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_7;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
    
    /* GPIO, PTC8 PIN  */
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_8;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* GPIO, PTC9 PIN  */
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_9;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
}


/**
 *@brief configure the gpio of general gpio
 *@param[in] none 
 *@return    none
 */
void group_gpio_output_set(gpio_port_e gpio_port,gpio_level_e level)
{
	if(gpio_port == GPIO_A)
	{
    	gpio_output_set(gpio_port, GPIO_PIN_1, level);
    	gpio_output_set(gpio_port, GPIO_PIN_2, level);
    	gpio_output_set(gpio_port, GPIO_PIN_3, level);
    	gpio_output_set(gpio_port, GPIO_PIN_4, level);
    	gpio_output_set(gpio_port, GPIO_PIN_5, level);
    	gpio_output_set(gpio_port, GPIO_PIN_6, level);
    	gpio_output_set(gpio_port, GPIO_PIN_7, level);
    	gpio_output_set(gpio_port, GPIO_PIN_8, level);
    	gpio_output_set(gpio_port, GPIO_PIN_9, level);
    	gpio_output_set(gpio_port, GPIO_PIN_10, level);
    	gpio_output_set(gpio_port, GPIO_PIN_11, level);
    	gpio_output_set(gpio_port, GPIO_PIN_12, level); 
    	gpio_output_set(gpio_port, GPIO_PIN_13, level); 
    	gpio_output_set(gpio_port, GPIO_PIN_14, level);
//    	gpio_output_set(gpio_port, GPIO_PIN_15, level);	//SWDIO	
	}
	else if(gpio_port == GPIO_B)
	{   
//		gpio_output_set(gpio_port, GPIO_PIN_1, level); //SWCLK
//    	gpio_output_set(gpio_port, GPIO_PIN_2, level); //RSTN
    	gpio_output_set(gpio_port, GPIO_PIN_3, level);
    	gpio_output_set(gpio_port, GPIO_PIN_4, level);
    	gpio_output_set(gpio_port, GPIO_PIN_5, level);
    	gpio_output_set(gpio_port, GPIO_PIN_6, level);
    	gpio_output_set(gpio_port, GPIO_PIN_7, level);
    	gpio_output_set(gpio_port, GPIO_PIN_8, level);
    	gpio_output_set(gpio_port, GPIO_PIN_9, level);
    	gpio_output_set(gpio_port, GPIO_PIN_10, level);
    	gpio_output_set(gpio_port, GPIO_PIN_11, level);
    	gpio_output_set(gpio_port, GPIO_PIN_12, level);
    	gpio_output_set(gpio_port, GPIO_PIN_13, level);
    	gpio_output_set(gpio_port, GPIO_PIN_14, level);
    	gpio_output_set(gpio_port, GPIO_PIN_15, level);		
		
	}
	else if(gpio_port == GPIO_C)
	{
	    gpio_output_set(gpio_port, GPIO_PIN_1, level);
    	gpio_output_set(gpio_port, GPIO_PIN_2, level);
    	gpio_output_set(gpio_port, GPIO_PIN_3, level);
    	gpio_output_set(gpio_port, GPIO_PIN_4, level);
    	gpio_output_set(gpio_port, GPIO_PIN_5, level);
    	gpio_output_set(gpio_port, GPIO_PIN_6, level);
    	gpio_output_set(gpio_port, GPIO_PIN_7, level);
    	gpio_output_set(gpio_port, GPIO_PIN_8, level);
    	gpio_output_set(gpio_port, GPIO_PIN_9, level);
	
	}
	else
	{
		// do nothing 
	}

}

/**
 *@brief configure the gpio of general gpio
 *@param[in] none 
 *@return    none
 */
void group_gpio_toggle(gpio_port_e gpio_port)
{
	if(gpio_port == GPIO_A)
	{
    	//gpio_toggle(gpio_port, GPIO_PIN_1);
    	//gpio_toggle(gpio_port, GPIO_PIN_2);
    	gpio_toggle(gpio_port, GPIO_PIN_3);
    	gpio_toggle(gpio_port, GPIO_PIN_4);
    	gpio_toggle(gpio_port, GPIO_PIN_5);
    	gpio_toggle(gpio_port, GPIO_PIN_6);
    	gpio_toggle(gpio_port, GPIO_PIN_7);
    	gpio_toggle(gpio_port, GPIO_PIN_8);
    	gpio_toggle(gpio_port, GPIO_PIN_9);
    	gpio_toggle(gpio_port, GPIO_PIN_10);
    	gpio_toggle(gpio_port, GPIO_PIN_11);
    	gpio_toggle(gpio_port, GPIO_PIN_12);
    	gpio_toggle(gpio_port, GPIO_PIN_13);
    	gpio_toggle(gpio_port, GPIO_PIN_14);
//    	gpio_toggle(gpio_port, GPIO_PIN_15);//SWDIO		
	}
	else if(gpio_port == GPIO_B)
	{
//	    gpio_toggle(gpio_port, GPIO_PIN_1);//SWCLK
//    	gpio_toggle(gpio_port, GPIO_PIN_2);//RSTN
    	gpio_toggle(gpio_port, GPIO_PIN_3);
    	gpio_toggle(gpio_port, GPIO_PIN_4);
    	gpio_toggle(gpio_port, GPIO_PIN_5);
    	gpio_toggle(gpio_port, GPIO_PIN_6);
    	gpio_toggle(gpio_port, GPIO_PIN_7);
    	gpio_toggle(gpio_port, GPIO_PIN_8);
    	gpio_toggle(gpio_port, GPIO_PIN_9);
    	gpio_toggle(gpio_port, GPIO_PIN_10);
    	gpio_toggle(gpio_port, GPIO_PIN_11);
    	gpio_toggle(gpio_port, GPIO_PIN_12);
    	gpio_toggle(gpio_port, GPIO_PIN_13);
    	gpio_toggle(gpio_port, GPIO_PIN_14);
    	gpio_toggle(gpio_port, GPIO_PIN_15);	
	
	
	}
	else if(gpio_port == GPIO_C)
	{
	    gpio_toggle(gpio_port, GPIO_PIN_1);
    	gpio_toggle(gpio_port, GPIO_PIN_2);
    	gpio_toggle(gpio_port, GPIO_PIN_3);
    	gpio_toggle(gpio_port, GPIO_PIN_4);
    	gpio_toggle(gpio_port, GPIO_PIN_5);
    	gpio_toggle(gpio_port, GPIO_PIN_6);
    	gpio_toggle(gpio_port, GPIO_PIN_7);
    	gpio_toggle(gpio_port, GPIO_PIN_8);
    	gpio_toggle(gpio_port, GPIO_PIN_9);
	}
	else
	{
		// do nothing 
	}
}

    /**
 *@brief configure the gpio as uvw hall interrupt gpio
 *@param[in] none 
 *@return    none
 */
void hall_uvw_gpio_init(void)
{
    // gpio_settings_config_t setting_cfg;

	// /* GPIO, HALL_W, PTA15 PIN  */
    // setting_cfg.gpio_port = GPIO_A;
    // setting_cfg.gpio_pin = GPIO_PIN_15;
    // setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    // setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    // setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    // setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    // setting_cfg.gpio_interrupt_cfg_info.int_handle = gpio_a_callback_test;
    // setting_cfg.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_EITHER_EDGE;
    // setting_cfg.gpio_interrupt_cfg_info.priority = 1;
    // gpio_init(&setting_cfg);

    // /* GPIO, HALL_U, PTB0 PIN  */
    // setting_cfg.gpio_port = GPIO_B;
    // setting_cfg.gpio_pin = GPIO_PIN_0;
    // setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    // setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    // setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    // setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    // setting_cfg.gpio_interrupt_cfg_info.int_handle = gpio_b_callback_test;
    // setting_cfg.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_EITHER_EDGE;
    // setting_cfg.gpio_interrupt_cfg_info.priority = 1;
    // gpio_init(&setting_cfg);

    // /* GPIO, HALL_V, PTB1 PIN  */
    // setting_cfg.gpio_port = GPIO_B;
    // setting_cfg.gpio_pin = GPIO_PIN_1;
    // setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    // setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    // setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    // setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    // setting_cfg.gpio_interrupt_cfg_info.int_handle = gpio_b_callback_test;
    // setting_cfg.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_EITHER_EDGE;
    // setting_cfg.gpio_interrupt_cfg_info.priority = 1;
    // gpio_init(&setting_cfg);
}


void hall_abz_gpio_init(void)
{
//     gpio_settings_config_t setting_cfg;

// 	/* GPIO, Hall_A, PTA15 PIN  */
//     setting_cfg.gpio_port = GPIO_A;
//     setting_cfg.gpio_pin = GPIO_PIN_15;
//     setting_cfg.gpio_alt = GPIO_ALT_FUNC_3;
//     setting_cfg.gpio_dir = GPIO_DIR_INPUT;
//     setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//     setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
//    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//     gpio_init(&setting_cfg);

//     /* GPIO, HALL_B, PTB0 PIN  */
//     setting_cfg.gpio_port = GPIO_B;
//     setting_cfg.gpio_pin = GPIO_PIN_0;
//     setting_cfg.gpio_alt = GPIO_ALT_FUNC_3;
//     setting_cfg.gpio_dir = GPIO_DIR_INPUT;
//     setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//     setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
//    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//     gpio_init(&setting_cfg);

//     /* GPIO, HALL_Z, PTB1 PIN  */
//     setting_cfg.gpio_port = GPIO_B;
//     setting_cfg.gpio_pin = GPIO_PIN_1;
//     setting_cfg.gpio_alt = GPIO_ALT_FUNC_3;
//     setting_cfg.gpio_dir = GPIO_DIR_INPUT;
//     setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//     setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
//    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//     gpio_init(&setting_cfg);
}

uint32_t gpio_a_callback_cnt = 0;
volatile uint32_t hall_value = 0;
void gpio_a_callback_test(gpio_int_source_e source)
{
	hall_value = 1*GPIO_HALL_U + 2*GPIO_HALL_V + 4*GPIO_HALL_W;

    gpio_a_callback_cnt++;
}

uint32_t gpio_b_callback_cnt = 0;
void gpio_b_callback_test(gpio_int_source_e source)
{
    hall_value = 1*GPIO_HALL_U + 2*GPIO_HALL_V + 4*GPIO_HALL_W;

    gpio_b_callback_cnt++;
}

uint32_t gpio_c_callback_cnt = 0;
void gpio_c_callback_test(gpio_int_source_e source)
{

    gpio_c_callback_cnt++;
}
