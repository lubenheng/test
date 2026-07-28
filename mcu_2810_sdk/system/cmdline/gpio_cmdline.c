#include "gpio_cmdline.h"
#include "utility.h"
#include "softinit.h"

/**
 * @file gpio_cmdline.c
 * @brief GPIO ???????
 *
 * ????? GPIO ?????????,???? GPIO ??/????????
 * ??????????????????????? GPIO ???????
 */

/**
 * @brief GPIO ????????
 *
 * @param argc ??????
 * @param argv ??????,????????:
 *  - argv[0] : case2,????,???????
 *  - argv[1] : GPIO ????(gpio_port_e)
 *  - argv[2] : GPIO ????(gpio_pin_e)
 *
 * @return 0 ??,-1 ??????
 *
 * @note ?????????????????? 100 ?,
 *       ??????? 1ms,???????????
 */
int cmd_gpio_toggle_test(int argc, char *argv[])
{
	if(argc != 3)
	{
		INFO_e("input error \n");
		return -1;
	}
	uint8_t case2 = get_arg_ulong(argv[0]);
	gpio_port_e GPIO_PORT = (gpio_port_e) get_arg_ulong(argv[1]);
	gpio_pin_e GPIO_PIN = (gpio_pin_e) get_arg_ulong(argv[2]);
	GPIO_Output_Init(GPIO_PORT,(gpio_pin_e)GPIO_PIN);
	
	
	for(uint16_t i = 0; i < 100; i++)
	{
		gpio_toggle(GPIO_PORT, GPIO_PIN);

		delay_ms(1);
	}
	INFO_e("GPIO OK\r\n");
	return 0;	
}


/**
 * @brief GPIO ????????????
 *
 * @param argc ??????
 * @param argv ??????,????????:
 *  - argv[0] : case2,????,???????
 *  - argv[1] : GPIO ????(gpio_port_e)
 *
 * @return 0 ??,-1 ??????
 *
 * @note ????? P4,????? P4 ? 0~5 ??;
 *       ????????????????????? 100 ??
 */
int cmd_gpio_port_toggle_test(int argc, char *argv[])
{
	if(argc != 2)
	{
		INFO_e("input error \n");
		return -1;
	}
	uint8_t case2 = get_arg_ulong(argv[0]);
	gpio_port_e GPIO_PORT = (gpio_port_e) get_arg_ulong(argv[1]);
	if(GPIO_PORT == GPIO_PORT_P4)
	{
		for(int i =0;i<6;i++)
		{
			GPIO_Output_Init(GPIO_PORT,(gpio_pin_e) i); 		// P4 ??? 0~5 ??
		}
	}
	else
	{
		for(int i =0;i<GPIO_PIN_NUM_MAX;i++)
		{
			GPIO_Output_Init(GPIO_PORT,(gpio_pin_e) i);		 
		}
	}
 
	INFO_e("GPIO OK\r\n");
	if(GPIO_PORT != GPIO_PORT_P4)
	{
		for(uint16_t i = 0; i < 100; i++)
		{
			gpio_toggle(GPIO_PORT,GPIO_PIN_0);
			gpio_toggle(GPIO_PORT,GPIO_PIN_1);
			gpio_toggle(GPIO_PORT,GPIO_PIN_2);
			gpio_toggle(GPIO_PORT,GPIO_PIN_3);
			gpio_toggle(GPIO_PORT,GPIO_PIN_4);
			gpio_toggle(GPIO_PORT,GPIO_PIN_5);
			gpio_toggle(GPIO_PORT,GPIO_PIN_6);
			gpio_toggle(GPIO_PORT,GPIO_PIN_7);
			delay_ms(1);
		}
	}
	else
	{
		for(uint16_t i = 0; i < 100; i++)
		{
			gpio_toggle(GPIO_PORT,GPIO_PIN_0);
			gpio_toggle(GPIO_PORT,GPIO_PIN_1);
			gpio_toggle(GPIO_PORT,GPIO_PIN_2);
			gpio_toggle(GPIO_PORT,GPIO_PIN_3);
			gpio_toggle(GPIO_PORT,GPIO_PIN_4);
			gpio_toggle(GPIO_PORT,GPIO_PIN_5);
			delay_ms(1);
		}
	}
	return 0;	
}

/**
 * @brief GPIO ?????????
 *
 * @param argc ??????
 * @param argv ??????,???????
 *
 * @return 0 ??
 *
 * @note ???? P0/P1/P3 ??????P2 ?????? P4 ? 0~5 ???????,
 *       ????????????? 1000 ?,???? GPIO ????????
 */
int cmd_gpio_port_all_test(int argc, char *argv[])
{
	uint8_t case2 = get_arg_ulong(argv[0]);
 
 
	for(int i =0;i<6;i++)
	{
		GPIO_Output_Init(GPIO_PORT_P4,(gpio_pin_e) i); 		// P4 ??? 0~5 ??
	}
 
	 
	for(int i =0;i<GPIO_PIN_NUM_MAX;i++)
	{
		GPIO_Output_Init(GPIO_PORT_P0,(gpio_pin_e) i);		
		GPIO_Output_Init(GPIO_PORT_P1,(gpio_pin_e) i);
		GPIO_Output_Init(GPIO_PORT_P3,(gpio_pin_e) i);		
	}
	GPIO_Output_Init(GPIO_PORT_P2,GPIO_PIN_0);
	GPIO_Output_Init(GPIO_PORT_P2,GPIO_PIN_1);
	GPIO_Output_Init(GPIO_PORT_P2,GPIO_PIN_2);
	GPIO_Output_Init(GPIO_PORT_P2,GPIO_PIN_3);
	GPIO_Output_Init(GPIO_PORT_P2,GPIO_PIN_4);
	
//	GPIO_Output_Init(GPIO_PORT_P2,GPIO_PIN_5);
	GPIO_Output_Init(GPIO_PORT_P2,GPIO_PIN_6);
//	GPIO_Output_Init(GPIO_PORT_P2,GPIO_PIN_7);
	
	 for(uint16_t i = 0; i < 1000; i++)
		{
			gpio_toggle(GPIO_PORT_P0,GPIO_PIN_0);
			gpio_toggle(GPIO_PORT_P0,GPIO_PIN_1);
			gpio_toggle(GPIO_PORT_P0,GPIO_PIN_2);
			gpio_toggle(GPIO_PORT_P0,GPIO_PIN_3);
			gpio_toggle(GPIO_PORT_P0,GPIO_PIN_4);
			gpio_toggle(GPIO_PORT_P0,GPIO_PIN_5);
			gpio_toggle(GPIO_PORT_P0,GPIO_PIN_6);
			gpio_toggle(GPIO_PORT_P0,GPIO_PIN_7);
			
			gpio_toggle(GPIO_PORT_P1,GPIO_PIN_0);
			gpio_toggle(GPIO_PORT_P1,GPIO_PIN_1);
			gpio_toggle(GPIO_PORT_P1,GPIO_PIN_2);
			gpio_toggle(GPIO_PORT_P1,GPIO_PIN_3);
			gpio_toggle(GPIO_PORT_P1,GPIO_PIN_4);
			gpio_toggle(GPIO_PORT_P1,GPIO_PIN_5);
			gpio_toggle(GPIO_PORT_P1,GPIO_PIN_6);
			gpio_toggle(GPIO_PORT_P1,GPIO_PIN_7);
			
			gpio_toggle(GPIO_PORT_P2,GPIO_PIN_0);
			gpio_toggle(GPIO_PORT_P2,GPIO_PIN_1);
			gpio_toggle(GPIO_PORT_P2,GPIO_PIN_2);
			gpio_toggle(GPIO_PORT_P2,GPIO_PIN_3);
			gpio_toggle(GPIO_PORT_P2,GPIO_PIN_4);
//			gpio_toggle(GPIO_PORT_P0,GPIO_PIN_5);
			gpio_toggle(GPIO_PORT_P2,GPIO_PIN_6);
//			gpio_toggle(GPIO_PORT_P0,GPIO_PIN_7);
			
			gpio_toggle(GPIO_PORT_P3,GPIO_PIN_0);
			gpio_toggle(GPIO_PORT_P3,GPIO_PIN_1);
			gpio_toggle(GPIO_PORT_P3,GPIO_PIN_2);
			gpio_toggle(GPIO_PORT_P3,GPIO_PIN_3);
			gpio_toggle(GPIO_PORT_P3,GPIO_PIN_4);
			gpio_toggle(GPIO_PORT_P3,GPIO_PIN_5);
			gpio_toggle(GPIO_PORT_P3,GPIO_PIN_6);
			gpio_toggle(GPIO_PORT_P3,GPIO_PIN_7);
			
			gpio_toggle(GPIO_PORT_P4,GPIO_PIN_0);
			gpio_toggle(GPIO_PORT_P4,GPIO_PIN_1);
			gpio_toggle(GPIO_PORT_P4,GPIO_PIN_2);
			gpio_toggle(GPIO_PORT_P4,GPIO_PIN_3);
			gpio_toggle(GPIO_PORT_P4,GPIO_PIN_4);
			gpio_toggle(GPIO_PORT_P4,GPIO_PIN_5);
			gpio_toggle(GPIO_PORT_P4,GPIO_PIN_6);
			gpio_toggle(GPIO_PORT_P4,GPIO_PIN_7);
			delay_ms(1);
	 
	}
	INFO_e("GPIO OK\r\n");
	return 0;	
}


/**
 * @brief ??? GPIO ????????????
 *
 * @param GPIO_PORT GPIO ??
 * @param GPIO_PIN GPIO ??
 *
 * @note ?????????????????????????
 */
void GPIO_Output_Init(gpio_port_e GPIO_PORT,gpio_pin_e GPIO_PIN)
{
	gpio_cfg_t 	GPIO_Init_Struct;
	
	GPIO_Init_Struct.alt_func	        =	GPIO_ALT_FUNC_0;
	GPIO_Init_Struct.analog_mode      =	GPIO_ANALOG_DISABLE;
	GPIO_Init_Struct.dir              =	GPIO_DIR_OUTPUT;
	GPIO_Init_Struct.pin              =	GPIO_PIN;
	GPIO_Init_Struct.port             =	GPIO_PORT;
	GPIO_Init_Struct.pull             =	GPIO_PULL_UP;
	GPIO_Init_Struct.strength         =	GPIO_STRENGTH_LOW;
	gpio_init(&GPIO_Init_Struct);
}

/**
 * @brief ??? GPIO ????????????
 *
 * @param GPIO_PORT GPIO ??
 * @param GPIO_PIN GPIO ??
 *
 * @note ??????????????????????????
 */
void GPIO_INput_Init(gpio_port_e GPIO_PORT,gpio_pin_e GPIO_PIN)
{
	gpio_cfg_t 	GPIO_Init_Struct;
	
	GPIO_Init_Struct.alt_func	        =	GPIO_ALT_FUNC_0;
	GPIO_Init_Struct.analog_mode      =	GPIO_ANALOG_ENABLE;
	GPIO_Init_Struct.dir              =	GPIO_DIR_INPUT;
	GPIO_Init_Struct.pin              =	GPIO_PIN;
	GPIO_Init_Struct.port             =	GPIO_PORT;
	GPIO_Init_Struct.pull             =	GPIO_PULL_DISABLE;
	GPIO_Init_Struct.strength         =	GPIO_STRENGTH_LOW;
	gpio_init(&GPIO_Init_Struct);
}

