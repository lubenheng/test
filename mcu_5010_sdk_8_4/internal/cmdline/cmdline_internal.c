#include "platform_common.h"

#include "test.h"
#include "ctype.h"

#include "uart_reg.h"
#include "uart_hal.h"
#include "uart.h"
#include "pmu_hal.h"

#include "gpio_hal.h"



#ifdef GPIO_TEST
#include "gpio_cmdline.h"
#endif


#ifdef UART_TEST
#include "uart_cmdline.h"
#endif


#ifdef FLASH_TEST
#include "flash_cmdline.h"
//#include "flash_test.h"
//#include "gpio_hal.h"
//#include "gpio_reg.h"
#endif

#ifdef MCPWM_TEST
//#include "trgmux_hal.h"
//#include "pwm_hal.h"
//#include "pwm_test.h"
#include "mcpwm_cmdline.h"
#endif

#ifdef ADC_TEST
//#include "adc_test.h"
#include "adc_cmdline.h"
#endif

#ifdef UART_CMDLINE
#include "uart_cmdline.h"
#endif

#ifdef HWMATH_TEST
#include "HWMath_cmdline.h"
#endif

#ifdef RTC_TEST
#include "rtc_cmdline.h"
#endif

#ifdef WDOG_TEST
#include "wdog_cmdline.h"
#endif

#ifdef I2C_TEST
#include "i2c_cmdline.h"
#endif

#ifdef SPI_TEST
#include "spi_cmdline.h"
#endif

#ifdef CAN_TEST
#include "can_cmdline.h"
#endif

#ifdef HALL_TEST
#include "hall_cmdline.h"
#endif

#ifdef QEP_TEST
#include "qep_cmdline.h"
#endif

#ifdef DMA_TEST
#include "dma_cmdline.h"
#endif

#ifdef CLOCK_TEST
#include "clock_cmdline.h"
#endif

#ifdef TIM_TEST
#include "TIM_cmdline.h"
#endif

#ifdef PMU_TEST
#include "pmu_cmdline.h"
#endif

#ifdef MODULE_TEST
#include "module_cmdline.h"
#endif





#include "cmdline_internal.h"

#ifdef CMDLINE_SUPPORT

//static char g_cmd_buf[CMD_BUF_SIZE];
//static cmdline_ctl g_cmdline_ctl = {0};

char g_cmd_buf[CMD_BUF_SIZE];
cmdline_ctl g_cmdline_ctl = {0};


const cmdline_entry menu[] = {
	{"cmd_help",                 			cmd_help,       			"  Display list of commands"},
	{"cmd_mem_read", 	           	 cmd_mem_read, 	   			"  cmd_mem_read:reg_address,len"},
	{"get_version", 	          		 	 get_version, 	   			"  get_version"},
	{"cmd_mem_write",		      	  cmd_mem_write, 				"  cmd_mem_write:reg_address,len,val0,val1..."},
//	{"cmd_pulse_us",					cmd_pulse_us, 				"  us"},
//	{"cmd_mem_write_cd",		    cmd_mem_write_cd, 			" cmd_mem_write_cd:reg_address,len,val0,val1..."},
//	{"cmd_query_idn",		        	cmd_query_idn, 				" getc board id"},
//	{"cmd_global_reset",		    	cmd_global_reset, 			" global reset"},
	{"cmd_gpio_output_set", 		cmd_gpio_output_set,		"  cmd_gpio_output_set: a, 0, high"},
	{"cmd_gpio_toggle", 		  	  cmd_gpio_toggle,		    "  cmd_gpio_toggle: a, 0"},
	{"cmd_gpio_init",					cmd_gpio_init,          "  port,pin,alt:dir,a,0,0,0"},        
//	{"cmd_sleep_now",			    cmd_sleep_now,				" cmd_sleep_now"},	
	{"cmd_deep_sleep_now",			cmd_deep_sleep_now,		    " cmd_deep_sleep_now"},	
	
#ifndef ROM_CODE

#ifdef SCM_TCMC_TEST
	{"cmd_sram_test", 							cmd_sram_test, 							" sram_test"},
#endif

#ifdef GPIO_TEST
	GPIO_CMDLINE_MENU
#endif

#ifdef MCPWM_TEST
	MCPWM_CMDLINE_MENU
#endif

#ifdef UART_TEST
	UART_CMDLINE_MENU
#endif

#ifdef FLASH_TEST
	FLASH_CMDLINE_MENU
#endif

#ifdef ADC_TEST
	ADC_CMDLINE_MENU
#endif

#ifdef HWMATH_TEST
	HWMATH_CMDLINE_MENU
#endif

#ifdef CAN_TEST
	CAN_CMDLINE_MENU
#endif

#ifdef SPI_TEST
	SPI_CMDLINE_MENU
#endif

#ifdef I2C_TEST
	I2C_CMDLINE_MENU
#endif

#ifdef RTC_TEST
#include "rtc_cmdline.h"
#endif

#ifdef WDOG_TEST
#include "wdog_cmdline.h"
#endif

#ifdef WDOG_TEST
	WDOG_CMDLINE_MENU
#endif

#ifdef RTC_TEST
	RTC_CMDLINE_MENU
#endif

#ifdef QEP_TEST
	QEP_CMDLINE_MENU
#endif

#ifdef HALL_TEST
	HALL_CMDLINE_MENU
#endif

#ifdef DMA_TEST
	DMA_CMDLINE_MENU
#endif

#ifdef CLOCK_TEST
	CLOCK_CMDLINE_MENU
#endif

#ifdef TIM_TEST
	TIM_CMDLINE_MENU
#endif

#ifdef PMU_TEST
	PMU_CMDLINE_MENU
#endif

#ifdef MODULE_TEST
	MODULE_CMDLINE_MENU
#endif


#endif
 	{0,0,0}
};




void cmdline_rx_callback(void *driverState, uart_event_e event, void *userData)
{
	uart_state_t *uart_state = (uart_state_t *)driverState;
	uint8_t cc = *(uart_state->rx_buff-1);

	if (UART_EVENT_RX_FULL == event)
	{
		if (cc == '\b' || cc == 0x7F)
		{
			// if the backspace key was pressed.
			// If there are any characters already in the buffer, then delete the last.
			if (g_cmdline_ctl.cmdline_buff_cnt)
			{
				g_cmdline_ctl.cmdline_buff_cnt--;
			}

			++uart_state->rx_size;
			return;

		}
		//	See if a newline character was received.
		if ((cc == '\r') || (cc == '\n'))
		{
			// If the character is a CR, then it may be followed by a LF which
			//	should be paired with the CR.	So remember that a CR was received.
			if (g_cmdline_ctl.cmdline_buff_cnt != 0)
			{
				g_cmdline_ctl.ready_flag = true;
			}

			*(uart_state->rx_buff-1) = 0; /*remove the last received '\r' or '\n'*/
			++uart_state->rx_size;
			return;
		}

		// Adjust the length back by 1 to leave space for the trailing null terminator.
		if (g_cmdline_ctl.cmdline_buff_cnt < (CMD_BUF_SIZE - 1))
		{
			g_cmd_buf[g_cmdline_ctl.cmdline_buff_cnt] = cc;
			g_cmdline_ctl.cmdline_buff_cnt++;

			++uart_state->rx_size;
		}
		else
		{
			memset((uint8_t *)g_cmd_buf, 0, sizeof(g_cmd_buf));
			g_cmdline_ctl.cmdline_buff_cnt =  0;

			++uart_state->rx_size;
		}
	}
}





void cmdline_rx_callback_polling(uint8_t *rx_buff)
{
	uint8_t cc = *(rx_buff-1);

	if (cc == '\b' || cc == 0x7F)
	{
		// if the backspace key was pressed.
		// If there are any characters already in the buffer, then delete the last.
		if (g_cmdline_ctl.cmdline_buff_cnt)
		{
			g_cmdline_ctl.cmdline_buff_cnt--;
		}

		return;

	}
	//	See if a newline character was received.
	if (cc == '\r')
	{
		*(rx_buff-1) = 0; /*remove the last received '\r' or '\n'*/
		return;
	}
	
	if (cc == '\n')
	{
		// If the character is a CR, then it may be followed by a LF which
		//	should be paired with the CR.	So remember that a CR was received.
		if (g_cmdline_ctl.cmdline_buff_cnt != 0)
		{
			g_cmdline_ctl.ready_flag = true;
		}

		*(rx_buff-1) = 0; /*remove the last received '\r' or '\n'*/
		return;
	}

	// Adjust the length back by 1 to leave space for the trailing null terminator.
	if (g_cmdline_ctl.cmdline_buff_cnt < (CMD_BUF_SIZE - 1))
	{
		g_cmd_buf[g_cmdline_ctl.cmdline_buff_cnt] = cc;
		g_cmdline_ctl.cmdline_buff_cnt++;
	}
	else
	{
		memset((uint8_t *)g_cmd_buf, 0, sizeof(g_cmd_buf));
		g_cmdline_ctl.cmdline_buff_cnt =  0;
	}
	
}



void cmdline_init(void)
{
	uart_rx_callback_install(UART_CONSOLE_INSTANCE, cmdline_rx_callback, NULL);
	uart_interrupt_handle_cfg(UART_CONSOLE_INSTANCE, uart_irq_handler);
	uart_receive_data(UART_CONSOLE_INSTANCE, (uint8_t *)g_cmd_buf, 1);
}

int cmdline_process(uint8_t *cmd, cmdline_entry *entry)
{
	static char *argv[CMDLINE_MAX_ARGS + 1];
	uint8_t *p_cmd;
	int argc;
	int find_flag = 1;

	argc = 0;
	p_cmd = cmd;

	while (*p_cmd)
	{
		//
		// If there is a space, then replace it with a zero, and set the flag
		// to search for the next argument.
		//
		if ((*p_cmd == ' ')||(*p_cmd == '#'))
		{
			*p_cmd = 0;
			find_flag = 1;
		}

		//
		// Otherwise it is not a space, so it must be a character that is part
		// of an argument.
		//
		else
		{
			//
			// If bFindArg is set, then that means we are looking for the start
			// of the next argument.
			//
			if (find_flag)
			{
				//
				// As long as the maximum number of arguments has not been
				// reached, then save the pointer to the start of this new arg
				// in the argv array, and increment the count of args, argc.
				//
				if (argc < CMDLINE_MAX_ARGS)
				{
					argv[argc] = (char *)p_cmd;
					argc++;
					find_flag = 0;
				}
				else
				{
					return CMDLINE_TOO_MANY_ARGS;
				}
			}
		}

		p_cmd++;
	}

	//
	// If one or more arguments was found, then process the command.
	//
	if (argc)
	{
		while (entry->cmd)
		{
			if (!strcmp((char *)argv[0], entry->cmd))
			{
				return (int)(entry->pfn_cmd(argc, argv));
			}

			entry++;
		}
	}
	else // no arguments was found
	{
		return CMDLINE_TOO_FEW_ARGS;
	}

    //one or more arguments was found, but no process meet the command.
	return CMDLINE_BAD_CMD;
}

void cmdline_handle(void)
{
	int ret;

	if (g_cmdline_ctl.ready_flag == false)
	{
		return;
	}

	INFO("cmd handle\n");
	uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)"cmd:>", 5);
	uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)g_cmd_buf, g_cmdline_ctl.cmdline_buff_cnt);
	uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)"\r\n", 2);
	uart_transmit_enable(UART_CONSOLE_INSTANCE, true);

	// Pass the line from the cmd_rw_buff to the command processor.
	// It will be parsed and valid commands executed.
	ret = cmdline_process((uint8_t *)g_cmd_buf, (cmdline_entry *)(&menu[0]));

	// Handle the case of bad command.
    if (ret == CMDLINE_BAD_CMD)
	{
		INFO("Bad command!\n");
	}
	else if (ret == CMDLINE_TOO_MANY_ARGS)
	{
		INFO("Too many arguments for command processor!\n");
	}
	else if (ret == CMDLINE_TOO_FEW_ARGS)
	{
		INFO("No arguments for command processor!!\n");
	}
	else if (ret != 0)
	{
		// Otherwise the command was executed.	Print the error code if one was returned.
		INFO("Command returned error code %d\n", ret);
	}

	/*reset the uart_state->rx_buff to the first location of g_cmd_buf*/
	uart_rx_buffer_set(UART_CONSOLE_INSTANCE, (uint8_t *)g_cmd_buf, 1u);

    //clean cmd_rw_buff and g_console_port uart receive fifo
    memset((uint8_t *)g_cmd_buf, 0, sizeof(g_cmd_buf));
    //uart_empty_fifo(UART0);

    g_cmdline_ctl.cmdline_buff_cnt = 0;
    g_cmdline_ctl.ready_flag = false;
	
}
void cmdline_handle_polling(void)
{
	
	bool flag;
	uint8_t *uart_tem_point = (uint8_t *)g_cmd_buf;
	uint32_t tem_time = 0;
	while(g_cmdline_ctl.ready_flag == false)
	{
		flag = uart_receive_data_polling_for_cmdline(UART_CONSOLE_INSTANCE, (uint8_t *)uart_tem_point, 1);
		if (flag == true)
		{
			tem_time = 0;
			uart_tem_point++;
			cmdline_rx_callback_polling(uart_tem_point);
		}
		else
		{
			tem_time++;
			if (tem_time > 1000)
			{
				return;
			}
		}
	}
	
	int ret;

	if (g_cmdline_ctl.ready_flag == false)
	{
		/*reset the uart_state->rx_buff to the first location of g_cmd_buf*/
		uart_rx_buffer_set(UART_CONSOLE_INSTANCE, (uint8_t *)g_cmd_buf, 1u);

		//clean cmd_rw_buff and g_console_port uart receive fifo
		memset((uint8_t *)g_cmd_buf, 0, sizeof(g_cmd_buf));
		//uart_empty_fifo(UART0);

		g_cmdline_ctl.cmdline_buff_cnt = 0;
			
		return;
	}

	
	uart_receive_enable(UART_CONSOLE_INSTANCE,false); // jack add

	
	
	INFO("cmd handle\n");
	uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)"cmd:>\n", 5);
	uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)g_cmd_buf, g_cmdline_ctl.cmdline_buff_cnt);
	uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)"\r\n", 3);
	uart_transmit_enable(UART_CONSOLE_INSTANCE, true);

	// Pass the line from the cmd_rw_buff to the command processor.
	// It will be parsed and valid commands executed.
	ret = cmdline_process((uint8_t *)g_cmd_buf, (cmdline_entry *)(&menu[0]));

	// Handle the case of bad command.
    if (ret == CMDLINE_BAD_CMD)
	{
		INFO("Bad command!\n");
	}
	else if (ret == CMDLINE_TOO_MANY_ARGS)
	{
		INFO("Too many arguments for command processor!\n");
	}
	else if (ret == CMDLINE_TOO_FEW_ARGS)
	{
		INFO("No arguments for command processor!!\n");
	}
	else if (ret != 0)
	{
		// Otherwise the command was executed.	Print the error code if one was returned.
		INFO("Command returned error code %d\n", ret);
	}

	/*reset the uart_state->rx_buff to the first location of g_cmd_buf*/
	uart_rx_buffer_set(UART_CONSOLE_INSTANCE, (uint8_t *)g_cmd_buf, 1u);

    //clean cmd_rw_buff and g_console_port uart receive fifo
    memset((uint8_t *)g_cmd_buf, 0, sizeof(g_cmd_buf));
    //uart_empty_fifo(UART0);

    g_cmdline_ctl.cmdline_buff_cnt = 0;
    g_cmdline_ctl.ready_flag = false;
	
}


int cmd_help(int argc, char *argv[])
{
	cmdline_entry *entry = (cmdline_entry *)(&menu[0]);

    INFO("\nAvailable commands\n");
    INFO("------------------\n");

    while (entry->cmd)
    {
        INFO("%s%s\n", entry->cmd, entry->help);

        entry++;
    }

    return 0;
}

int cmd_query_idn(int argc, char *argv[])
{
	INFO("target_0\n");

	return 0;
}

int cmd_global_reset(int argc, char *argv[])
{
	//rstgen_reset_ctrl(RSTGEN_GLOBAL_RESET, RSTGEN_RST_ENABLE);
	
	return 0;
}


int cmd_mem_read(int argc, char *argv[])
{
	uint32_t reg_addr;
	uint32_t val;
	uint32_t len;

	/* Check the validity of parameter count */
	if (argc < 3)
	{
		INFO("wrong cmd_format:%s addr len\n", argv[0]);

		return ERR_TEST_ARGC;
	}

	reg_addr = get_arg_ulong(argv[1]);
	len = get_arg_ulong(argv[2]);
	if (reg_addr&0x03)
	{
        ERR("ERR:address must be aligned with 4!\n");
        return ERR_TEST_ARGC;
	}

	while (len)
	{
		val = REG32(reg_addr);
        INFO("0x%08x:0x%08x\n", reg_addr, val);

        reg_addr += 4;
        len--;
    }

    INFO("PASS\n");

    return ERR_TEST_DONE;
}

int get_version(int argc, char *argv[])
{
	uint32_t val;
	uint32_t reg_addr = 0x400207F8;
	val = REG32(reg_addr);
    INFO("0x%08x:0x%08x\n", reg_addr, val);
	
	return ERR_TEST_DONE;
}

int cmd_mem_write(int argc, char *argv[])
{
    uint32_t reg_addr;
    uint32_t val;
    uint32_t len;

	/*argv will be:cmd address len values*/
    len = get_arg_ulong(argv[2]);

	/* Check the validity of parameter count */
	if (argc < (len+3))
    {
        ERR("wrong cmd_format:%s addr len data\n", argv[0]);
        return ERR_TEST_ARGC;
    }

	reg_addr = get_arg_ulong(argv[1]);
	if (reg_addr&0x03)
	{
        ERR("ERR:address must be aligned with 4!\n");
        return ERR_TEST_ARGC;
	}

    while (len)
    {
        val = get_arg_ulong(argv[argc - len]);
        WRITE_REG_32(reg_addr, val);

        reg_addr += 4;
        len--;
    }

	INFO("PASS\n");
    return ERR_TEST_DONE;
}

int cmd_mem_write_cd(int argc, char *argv[])
{
	return cmd_mem_write(argc-2, argv);
}

static gpio_port_e gpio_get_port(char *str)
{
	//INFO("%s\n", str);
	switch (str[0])
	{
		case 'a':
		case 'A':
			return GPIO_A;
		case 'b':
		case 'B':
			return GPIO_B;
		case 'c':
		case 'C':
			return GPIO_C;
		default:
			return GPIO_PORT_MAX;
	}
}


//int	cmd_sleep_now(int argc, char *argv[])
//{
//	UNUSED(argc);
//	UNUSED(argv);
///* OCS_Reserved_Start */
//	//INFO("sleep\n");
///* OCS_Reserved_End */
//	pmu_sleep_now();

//	return ERR_TEST_DONE;
//}

int	cmd_deep_sleep_now(int argc, char *argv[])
{
	uint32_t val;
	PMU_TypeDef  *pmu_ptr = PMU;
	
	val = get_arg_ulong(argv[1]);
	if(val == 0)
	{
		INFO("rstn deep sleep\n");
		delay_1us(20);
		pmu_deepsleep_now();
	}
	else if(val == 1)
	{
		INFO("gpio deep sleep\n");
		delay_1us(20);
		pmu_deepsleep_now();
	}
	else
	{
		INFO("lptimer deep sleep\n");
		delay_1us(20);
		pmu_drv_lptimer_enable(pmu_ptr,true);
		pmu_deepsleep_now();
	}

	return ERR_TEST_DONE;
}

// int	cmd_deep_sleep_now(int argc, char *argv[])
// {
// 	UNUSED(argc);
// 	UNUSED(argv);
// /* OCS_Reserved_Start */
// 	INFO("lptimer deep sleep\n");
// /* OCS_Reserved_End */
	
// 	pmu_drv_lptimer_enable(pmu_ptr,true);

// 	pmu_deepsleep_now();

// 	return ERR_TEST_DONE;
// }

static gpio_pin_e gpio_get_pin(char *str)
{
	return (gpio_pin_e)get_arg_ulong(str);
	
#if 0
	if (strlen(str)==1U)
	{
		return (gpio_pin_e)((uint32_t)str[0]-48U);
	}
	else
	{
		return (gpio_pin_e)(((uint32_t)str[0]-48u)*10+((uint32_t)str[1]-48U));
	}
#endif
}

static gpio_level_e gpio_get_level(char *str)
{
	if (!(strncmp(str,"high",4)))
	{
		return GPIO_HIGH_LEVEL;
	}
	else
	{
		return GPIO_LOW_LEVEL;
	}
}


int cmd_gpio_output_set(int argc, char *argv[])
{
	
	gpio_settings_config_t gpio_settings_config;
	
	gpio_settings_config.gpio_port = gpio_get_port(argv[1]);
	gpio_settings_config.gpio_pin = gpio_get_pin(argv[2]);
	gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_0;
	gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
	gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
	gpio_settings_config.gpio_strength = GPIO_STRENGTH_MODE_1;
	gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
	
	gpio_init(&gpio_settings_config);
	
	gpio_output_set(gpio_get_port(argv[1]),gpio_get_pin(argv[2]),gpio_get_level(argv[3]));
	
	
	return ERR_TEST_DONE;
}

int cmd_gpio_init(int argc, char *argv[])
{
	
	gpio_settings_config_t gpio_settings_config;
	
	gpio_settings_config.gpio_port = gpio_get_port(argv[1]);
	gpio_settings_config.gpio_pin = gpio_get_pin(argv[2]);
	gpio_settings_config.gpio_alt = (gpio_alt_e)get_arg_ulong(argv[3]);
	gpio_settings_config.gpio_dir = (gpio_dir_e)get_arg_ulong(argv[4]);
	gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
	gpio_settings_config.gpio_strength = GPIO_STRENGTH_MODE_1;
	gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
	
	gpio_init(&gpio_settings_config);
	
	return ERR_TEST_DONE;
}

int cmd_gpio_toggle(int argc, char *argv[])
{	
	gpio_settings_config_t gpio_settings_config;
	
	gpio_settings_config.gpio_port = gpio_get_port(argv[1]);
	gpio_settings_config.gpio_pin = gpio_get_pin(argv[2]);
	gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_0;
	gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
	gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
	gpio_settings_config.gpio_strength = GPIO_STRENGTH_MODE_1;
	gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
	
	gpio_init(&gpio_settings_config);
	
	for(uint16_t i = 0; i < 100; i++)
	{
		gpio_toggle(gpio_get_port(argv[1]),gpio_get_pin(argv[2]));
		delay_1us(1000);
	}
	
	return ERR_TEST_DONE;
}


int cmd_gpio_toggle_times(int argc, char *argv[])
{
	gpio_settings_config_t gpio_settings_config;
	uint32_t toggle_times = get_arg_ulong(argv[3]);
	uint32_t interval = get_arg_ulong(argv[4]);

	if (toggle_times == 0)
	{
		return ERR_TEST_PARA;
	}
	/*init gpio as output low*/
	
	gpio_settings_config.gpio_port = gpio_get_port(argv[1]);
	gpio_settings_config.gpio_pin = gpio_get_pin(argv[2]);
	gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_1;
	gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
	gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
	gpio_settings_config.gpio_strength = GPIO_STRENGTH_MODE_1;
	//gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
	
	gpio_init(&gpio_settings_config);

	/*toggle*/
	for (uint32_t i=0;i<toggle_times;i++)
	{
		delay_1us(interval);
		gpio_toggle(gpio_get_port(argv[1]),gpio_get_pin(argv[2]));
	}

	return ERR_TEST_DONE;
}



/**
 * @brief  生成可配置宽度的高电平脉冲
 * @param  condition: 触发条件（通常为变量a）
 * @param  pulse_width_us: 脉冲宽度（1-1000微秒）
 * @retval 无
 */
//extern uint32_t g_systick_count;
int cmd_pulse_us(int argc, char *argv[])
{
	uint32_t high_level_time = 2; //高电平的总时间，最小为2us

	high_level_time = get_arg_ulong(argv[1]);

	for(uint16_t i=0;i< argc;i++){INFO("argv[%d]:%s\n", i,argv[i]);}
	//g_systick_count = 0;
	
	SysTick->LOAD  = (uint32_t)(high_level_time*32 - 1UL);              							/* set reload register */
  //NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
	NVIC_SetPriority (SysTick_IRQn, 0); /* set Priority for Systick Interrupt */
  SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
	
	//gpio_output_set(GPIO_C,GPIO_PIN_8,GPIO_HIGH_LEVEL);    // 输出高电平
	
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */

	return 0;
}

#endif /*CMDLINE_SUPPORT*/

