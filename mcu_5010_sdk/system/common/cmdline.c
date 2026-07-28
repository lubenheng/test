
/*******************************************************************************
 * Include Files
 ******************************************************************************/
#include "platform_common.h"
#include "ctype.h"
#include "pmu_hal.h"
#include "uart_hal.h"
#include "cmdline.h"
#include "rstgen_hal.h"
#include "gpio_hal.h"

#ifdef FW_FOTA_ENABLE
static int cmd_fota_glabal_reset(int argc, char *argv[]);
static int cmd_code_region(int argc, char *argv[]);
static int cmd_bootloader_jump_to_app(int argc, char *argv[]);
static int cmd_app_jump_to_bootloader(int argc, char *argv[]);
#endif



/*******************************************************************************
 * Variables
 ******************************************************************************/
/** the buffer used to save received characters */
static char g_cmd_buf[CMD_BUF_SIZE];
/** the control structure used to record cmdline information */
static cmdline_ctl g_cmdline_ctl = {0};
/** Table of command */
const cmdline_entry menu[] = {
    {"cmd_help",                    cmd_help,       			" Display list of commands"},
	{"mem_read", 	            	mem_read, 	   				" mem_read:reg_address,len"},
	{"mem_write",		        	mem_write, 					" mem_write:reg_address,len,val0,val1..."},	
    #ifdef FW_FOTA_ENABLE
    {"cmd_code_region",			    cmd_code_region, 			" code_region:bootloader or app"},
	{"cmd_bootloader_jump_to_app",	cmd_bootloader_jump_to_app,	" bootloader_jump_to_app"},
    {"cmd_app_jump_to_bootloader",	cmd_app_jump_to_bootloader,	" app_jump_to_bootloader"},	
    {"cmd_fota_glabal_reset",       cmd_fota_glabal_reset,      " cmd_fota_glabal_reset"},	
    #endif
    {0,0,0}
};


/*******************************************************************************
* cmdline Control Functions
******************************************************************************/ 
/**
 *@brief the callback function of uart RX
 *@param[in] *driver_state: point to the runtime state of uart
 *@param[in] event: the event which occurred when the callback was invoked 
 *@param[in] *user_data: the point to the related character
 *@return    none
 */
void cmdline_rx_callback(void *driver_state, uart_event_e event, void *user_data)
{
        UNUSED(user_data);
	uart_state_t *uart_state = (uart_state_t *)driver_state;
	uint8_t cc = *(uart_state->rx_buff-1);

	if (UART_EVENT_RX_FULL == event)
	{
		/* mo matter what character the uart receive, we must reset the rx_size to 1 */
		++uart_state->rx_size;

		/* if the backspace key was pressed. If there are any characters already in the buffer, then delete the last.*/
		if (cc == '\b' || cc == 0x7F)
		{
			if (g_cmdline_ctl.cmdline_buff_cnt)
			{
				g_cmdline_ctl.cmdline_buff_cnt--;
			}
			
			return;
		}

		/* if a LF(\n) or a CR(\r) is received, it means a command is received completely */
		if ((cc == '\r') || (cc == '\n'))
		{
			g_cmdline_ctl.ready_flag = true;
			/* remove the last received '\r' or '\n'*/
			*(uart_state->rx_buff-1) = 0;
			
			return;			
		}

		/*receive a character into command buffer */
		if (g_cmdline_ctl.cmdline_buff_cnt < (CMD_BUF_SIZE - 1))
		{
			g_cmd_buf[g_cmdline_ctl.cmdline_buff_cnt] = cc;
			g_cmdline_ctl.cmdline_buff_cnt++;
		}
		/*if command buffer is full, reset the command buffer */
		else
		{
			memset((uint8_t *)g_cmd_buf, 0, sizeof(g_cmd_buf));
			g_cmdline_ctl.cmdline_buff_cnt =  0;
		}
	}		
}

/**
 *@brief the initialized function of cmdline mechanism
 *@param[in] none
 *@return    none
 */
void cmdline_init(void)
{
	uart_rx_callback_install(UART_CONSOLE_INSTANCE, cmdline_rx_callback, NULL);
	uart_interrupt_handle_cfg(UART_CONSOLE_INSTANCE, uart_irq_handler);
	uart_receive_data(UART_CONSOLE_INSTANCE, (uint8_t *)g_cmd_buf, 1);
}

/**
 *@brief the process function of the received character
 *@param[in] none
 *@return    none
 */
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
		/* If there is a space, then replace it with a zero, and set the flag to search for the next argument. */
		if ((*p_cmd == ' ')||(*p_cmd == '#'))
		{
			*p_cmd = 0;
			find_flag = 1;
		}
		/* Otherwise it is not a space, so it must be a character that is part of an argument */
		else
		{
			/* If bFindArg is set, then that means we are looking for the start */
			/* of the next argument. */
			if (find_flag)
			{
				/* As long as the maximum number of arguments has not been */
				/* reached, then save the pointer to the start of this new arg */
				/* in the argv array, and increment the count of args, argc. */
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
	/* If one or more arguments was found, then process the command. */
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
	else /* no arguments was found */
	{
		return CMDLINE_TOO_FEW_ARGS;
	}

    /* one or more arguments was found, but no process meet the command. */
	return CMDLINE_BAD_CMD;
}

/**
 *@brief the handle function of cmdline mechanism
 *@param[in] none
 *@return    none
 */
void cmdline_handle(void)
{
	int ret;
	char info[100];

	if (g_cmdline_ctl.ready_flag == false)
	{
		return;
	}

	uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)"cmd:>", strlen("cmd:>"));
	uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)g_cmd_buf, g_cmdline_ctl.cmdline_buff_cnt);
	uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)"\r\n", strlen("\r\n"));
	
	/* Pass the line from the cmd_rw_buff to the command processor. */
	/* It will be parsed and valid commands executed. */
	ret = cmdline_process((uint8_t *)g_cmd_buf, (cmdline_entry *)(&menu[0]));

	/* Handle the case of bad command. */
    if (ret == CMDLINE_BAD_CMD)
	{
		uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)"Bad command!\n", strlen("Bad command!\n"));
	}
	else if (ret == CMDLINE_TOO_MANY_ARGS)
	{
		uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)"Too many arguments for command processor!\n", strlen("Too many arguments for command processor!\n"));
	}
	else if (ret == CMDLINE_TOO_FEW_ARGS)
	{
		uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)"No arguments for command processor!!\n", strlen("No arguments for command processor!!\n"));
	}
	else if (ret != 0)
	{			
		/* Otherwise the command was executed.	Print the error code if one was returned. */
		snprintf(info, 100, "Command returned error code %d\n", ret);
		uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)info, strlen(info));
	}

	/* enable the uart tx for INFO() FUNCTION */
	uart_transmit_enable(UART_CONSOLE_INSTANCE, true);
	
	/*reset the uart_state->rx_buff to the first location of g_cmd_buf*/
	uart_rx_buffer_set(UART_CONSOLE_INSTANCE, (uint8_t *)g_cmd_buf, 1u);
	
    /*clean cmd_rw_buff and g_console_port uart receive fifo */
    memset((uint8_t *)g_cmd_buf, 0, sizeof(g_cmd_buf));

    g_cmdline_ctl.cmdline_buff_cnt = 0;
    g_cmdline_ctl.ready_flag = false;
}

/**
 *@brief the callback function of "help" commmand in cmdline mechanism
 *@param[in] argc: parameter number
 *@param[in] *argv: the pointer to the parameter
 *@return    callback function execute result status:
 *				- ERR_TEST_DONE, callback function execute successfully
 *				- ERR_TEST_FAIL, callback function execute fail
 *				- ERR_TEST_ARGC, the number of parameter is invalid
 *				- ERR_TEST_PARA, the parameter is wrong
 */
int cmd_help(int argc, char *argv[])
{
	UNUSED(argc);
	UNUSED(argv);
	char info[100];
	cmdline_entry *entry = (cmdline_entry *)(&menu[0]);

	uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)"\nAvailable commands\n", strlen("\nAvailable commands\n"));
	uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)"------------------\n", strlen("------------------\n"));

    while (entry->cmd)
    {
    	snprintf(info, 100, "%s%s\n", entry->cmd, entry->help);
		uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)info, strlen(info));

        entry++;
    }

    return 0;
}

/**
 *@brief the callback function of "mem_read" commmand in cmdline mechanism
 *@param[in] argc: parameter number
 *@param[in] *argv: the pointer to the parameter
 *@return    callback function execute result status:
 *				- ERR_TEST_DONE, callback function execute successfully
 *				- ERR_TEST_FAIL, callback function execute fail
 *				- ERR_TEST_ARGC, the number of parameter is invalid
 *				- ERR_TEST_PARA, the parameter is wrong
 */
int mem_read(int argc, char *argv[])
{
	uint32_t reg_addr;
	uint32_t val;
	uint32_t len;
	char info[100];

	/* Check the validity of parameter count */
	if (argc < 3)
	{
		snprintf(info, 100, "wrong cmd_format:%s addr len\n", argv[0]);
		uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)info, strlen(info));

		return ERR_TEST_ARGC;
	}

	reg_addr = get_arg_ulong(argv[1]);
	len = get_arg_ulong(argv[2]);
	if (reg_addr&0x03)
	{
		uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)"ERR:address must be aligned with 4!\n", strlen("ERR:address must be aligned with 4!\n"));
        return ERR_TEST_ARGC;
	}
	
#if defined ( USE_GCC )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
#endif

	while (len)
	{
		val = REG32(reg_addr);
		snprintf(info, 100, "0x%08x:0x%08x\n", reg_addr, val);
		uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)info, strlen(info));

        reg_addr += 4;
        len--;
    }
#if defined ( USE_GCC )
#pragma GCC diagnostic pop
#endif

	uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)"PASS\n", strlen("PASS\n"));

    return ERR_TEST_DONE;
}

/**
 *@brief the callback function of "mem_write" commmand in cmdline mechanism
 *@param[in] argc: parameter number
 *@param[in] *argv: the pointer to the parameter
 *@return    callback function execute result status:
 *				- ERR_TEST_DONE, callback function execute successfully
 *				- ERR_TEST_FAIL, callback function execute fail
 *				- ERR_TEST_ARGC, the number of parameter is invalid
 *				- ERR_TEST_PARA, the parameter is wrong
 */
int mem_write(int argc, char *argv[])
{
    uint32_t reg_addr;
    uint32_t val;
    int len;
	char info[100];

	/*argv will be:cmd address len values*/
    len = get_arg_ulong(argv[2]);

	/* Check the validity of parameter count */
	if (argc < (len+3))
    {
    	snprintf(info, 100, "wrong cmd_format:%s addr len data\n", argv[0]);
        uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)info, strlen(info));
        return ERR_TEST_ARGC;
    }

	reg_addr = get_arg_ulong(argv[1]);
	if (reg_addr&0x03)
	{
		uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)"ERR:address must be aligned with 4!\n", strlen("ERR:address must be aligned with 4!\n"));
        return ERR_TEST_ARGC;
	}
	
    while (len)
    {
        val = get_arg_ulong(argv[argc - len]);
        WRITE_REG_32(reg_addr, val);
        reg_addr += 4;
        len--;
    }
	
	uart_send_data_polling(UART_CONSOLE_INSTANCE, (uint8_t *)"PASS\n", strlen("PASS\n"));
    return ERR_TEST_DONE;
}




#ifdef FW_FOTA_ENABLE

static int cmd_fota_glabal_reset(int argc, char *argv[])
{
    /* trigger a software global reset */
    fota_glabal_reset();
    
    return 0;
}

static int cmd_code_region(int argc, char *argv[])
{
#ifdef  APP_CODE
	INFO("UART receive interrupt in APP region\n");
#else
	INFO("UART receive interrupt in Bootloader region\n");
#endif
	
	return 0;	
}

extern void jump_to_app(uint32_t app_start_addr);
extern void jump_to_bootloader(uint32_t bootloader_start_addr);

static int cmd_bootloader_jump_to_app(int argc, char *argv[])
{
	INFO("bootloader code jump to app code \n");
	jump_to_app(FW_APP_START_ADDRESS);
    return 0;
}

static int cmd_app_jump_to_bootloader(int argc, char *argv[])
{
    __disable_irq();
    
    INFO("app code jump to bootloader code \n");
	jump_to_bootloader(FW_BOOT_START_ADDRESS);	
    return 0;
}
#endif

