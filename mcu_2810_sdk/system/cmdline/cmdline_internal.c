#include "string.h"
#include "stdbool.h"
#include "stdio.h"
#include "cmdline_internal.h"
#include "test.h" 
#include "uart.h"
#include "utility.h"
 
//#define UART0_SPI
#ifdef GPIO_TEST
#include "gpio_cmdline.h"
#endif

#ifdef interrupt_TEST
#include "interrupt_cmdline.h"
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
#define UART0_SPI

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
#include "uart_cmd.h"
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
 
const cmdline_entry menu[] = {
	{"cmd_help",                 	cmd_help,       			"  Display list of commands"},
	{"cmd_mem_read", 	            cmd_mem_read, 	   			"  cmd_mem_read:reg_address,len"},
	{"get_version", 	            get_version, 	   			"  get_version"},
	{"cmd_mem_write",		        cmd_mem_write, 				"  cmd_mem_write:reg_address,len,val0,val1..."},
////	{"cmd_pulse_us",				cmd_pulse_us, 				"  us"},
////	{"cmd_mem_write_cd",		    cmd_mem_write_cd, 			" cmd_mem_write_cd:reg_address,len,val0,val1..."},
////	{"cmd_query_idn",		        cmd_query_idn, 				" getc board id"},
////	{"cmd_global_reset",		    cmd_global_reset, 			" global reset"},
//	{"cmd_gpio_output_set", 		cmd_gpio_output_set,		"  cmd_gpio_output_set: a, 0, high"},
//	{"cmd_gpio_toggle", 		    cmd_gpio_toggle,		    "  cmd_gpio_toggle: a, 0"},
//	{"cmd_gpio_init",						cmd_gpio_init,          "  port,pin,alt:dir,a,0,0,0"},        
////	{"cmd_sleep_now",			    cmd_sleep_now,				" cmd_sleep_now"},	
//	{"cmd_deep_sleep_now",			cmd_deep_sleep_now,		    " cmd_deep_sleep_now"},	
	
#ifndef ROM_CODE

#ifdef SCM_TCMC_TEST
	{"cmd_sram_test", 							cmd_sram_test, 							" sram_test"},
#endif

#ifdef GPIO_TEST
	GPIO_CMDLINE_MENU
#endif

#ifdef interrupt_TEST
	interrupt_CMDLINE_MENU
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
//	I2C_CMDLINE_MENU
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

int get_version(int argc, char *argv[])
{
	uint32_t val;
//	uint32_t reg_addr = (FOC->ro_emf1 & (0xFFFF << 10));  //??16??
	uint32_t reg_addr = (DIGCTRL->rst_info);
	val = (DIGCTRL->rst_info)  >> 4;
	
	INFO_e("0x%08x:0x%08x\n", reg_addr, val);
	
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
		INFO_e("wrong cmd_format:%s addr len\n", argv[0]);

		return ERR_TEST_ARGC;
	}

	reg_addr = get_arg_ulong(argv[1]);
	len = get_arg_ulong(argv[2]);
	if (reg_addr&0x03)
	{
        INFO_e("ERR:address must be aligned with 4!\n");
        return ERR_TEST_ARGC;
	}

	while (len)
	{
		val = REG32(reg_addr);
        INFO_e("0x%08x:0x%08x\n", reg_addr, val);

        reg_addr += 4;
        len--;
    }

    INFO_e("PASS\n");

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
        INFO_e("wrong cmd_format:%s addr len data\n", argv[0]);
        return ERR_TEST_ARGC;
    }

	reg_addr = get_arg_ulong(argv[1]);
	if (reg_addr&0x03)
	{
        INFO_e("ERR:address must be aligned with 4!\n");
        return ERR_TEST_ARGC;
	}

    while (len)
    {
        val = get_arg_ulong(argv[argc - len]);
        WRITE_REG_32(reg_addr, val);

        reg_addr += 4;
        len--;
    }

	INFO_e("PASS\n");
    return ERR_TEST_DONE;
}


// ¨ªa2?¨¦¨´?¡Â¡êoUART1??¡À¨²¡ê¡§?¨²HAL?a3?¨º??¡¥???t?D?¡§¨°?¡ê?
UART_TypeDef Uart1_Handle_Struct;
UART_TypeDef Uart2_Handle_Struct;

// ?¨¹¨¢?DD?¨®¨º??o3???¡ä¨®D?
#define CMD_BUF_SIZE 128
// ¨¨????¨¹¨¢?DD?¨®¨º??o3???
char g_cmd_buf[CMD_BUF_SIZE];

// ?¨¹¨¢?DD?????¨¢11¨¬?
// ¨®?¨®¨²?????¨®¨º?3¡è?¨¨?¡é?¨¹¨¢?¨º?¡¤?¡Á?¡À?o?
typedef struct {
    volatile uint32_t cmdline_buff_cnt;  // ¦Ì¡À?¡ã?¨®¨º?¦Ì?¡Á?¡¤???¨ºy
    volatile bool ready_flag;            // ?¨¹¨¢??¨®¨º?¨ª¨º3¨¦¡À¨º??¡ê¡§¨º?¦Ì???3¦Ì?atrue¡ê?
} cmdline_ctl;

// ¨¨????¨¹¨¢?DD????¡À?¨¢?¡ê?3?¨º??¡¥?a0
cmdline_ctl g_cmdline_ctl = {0};




/*********************************************************************
* o¡¥¨ºy??¡êoUart_cmdline_Init
* 1|  ?¨¹¡êoUSART1 3?¨º??¡¥¡ê¡§PB6 TX¡ê?PB7 RX¡ê?¡ê?¨®?¨®¨²?¨¹¨¢?DD???£¤
* 2?  ¨ºy¡êo?T
* ¡¤¦Ì???¦Ì¡êo?T
********************************************************************/
void Uart_cmdline_Init(void)
{
	uart1_init();
//	uart1Tx_init();
	uart1Rx_init();
}

/*********************************************************************
* o¡¥¨ºy??¡êofputc
* 1|  ?¨¹¡êo???¡§?¨°printf¡ê?¨º1¨®?USART1¡¤¡é?¨ª¨ºy?Y
* 2?  ¨ºy¡êoch ¨°a¡¤¡é?¨ª¦Ì?¡Á?¡¤?
* ¡¤¦Ì???¦Ì¡êo¡¤¦Ì??¡¤¡é?¨ª¦Ì?¡Á?¡¤?
********************************************************************/
int fputc(int ch, FILE *f)
{
	#ifdef UART0_SPI
		// ¦Ì¨¨¡äy??¡¤¡é?¨ªFIFO¡¤??¨²??Tx_full = 0
    // ??¦Ì???¡ä??¡Â¡êoInt_clr ??¡ä??¡Â bit3 = Tx_full
    while ((UART0->int_clr & (1 << 3)) != 0);
    
    // D¡ä¨¨?¨ºy?Y¦Ì? DAT ??¡ä??¡Â
    UART0->dat = (uint8_t)ch;
    
    return ch;
	#else
    // ¦Ì¨¨¡äy??¡¤¡é?¨ªFIFO¡¤??¨²??Tx_full = 0
    // ??¦Ì???¡ä??¡Â¡êoInt_clr ??¡ä??¡Â bit3 = Tx_full
    while ((UART1->int_clr & (1 << 3)) != 0);
    
    // D¡ä¨¨?¨ºy?Y¦Ì? DAT ??¡ä??¡Â
    UART1->dat = (uint8_t)ch;
    
    return ch;
	#endif
}

/*********************************************************************
* o¡¥¨ºy??¡êouart_read_byte
* 1|  ?¨¹¡êo¡ä¨®USART1?¨¢¨¨?1¡Á??¨²¨ºy?Y¡ê¡§?¨¢¨¨?DR??¡ä??¡Â¡ê?
* 2?  ¨ºy¡êo?T
* ¡¤¦Ì???¦Ì¡êo?¨¢¨¨?¦Ì?¦Ì?¡Á??¨²
********************************************************************/
static uint8_t uart_read_byte(void)
{
	#ifdef UART0_SPI
	return (uint8_t)(UART0->dat & 0xFFU);
	#else
  return (uint8_t)(UART1->dat & 0xFFU);
	#endif
	
    
}

/*********************************************************************
* o¡¥¨ºy??¡êouart_receive_data_polling_for_cmdline
* 1|  ?¨¹¡êo???¡¥¡¤?¨º??¨®¨º?1¡Á??¨²¡ä??¨²¨ºy?Y
* 2?  ¨ºy¡êo*data ?¨®¨º?¨ºy?Y¡ä?¡¤?¦Ì??¡¤
* ¡¤¦Ì???¦Ì¡êotrue ¨º?¦Ì?¨ºy?Y¡ê?false ?¡ä¨º?¦Ì?
********************************************************************/
bool uart_receive_data_polling_for_cmdline(uint8_t *data)
{
	#ifdef UART0_SPI
 // ??¦Ì???¡ä??¡Â¡êoInt_clr ??¡ä??¡Â bit7 = Rx_full
    // Rx_full=1 ¡À¨ª¨º?¨º?¦Ì?¨ºy?Y¡ê??¨¦¨°??¨¢¨¨?
    if ((UART0->int_clr & (1 << 8)) == 0U)
    {				
        *data = uart_read_byte();  // ?¨¢ DAT ??¡ä??¡Â??¦Ì?¨ºy?Y
        return true;
    }
    return false;
	#else
  // ??¦Ì???¡ä??¡Â¡êoInt_clr ??¡ä??¡Â bit7 = Rx_full
    // Rx_full=1 ¡À¨ª¨º?¨º?¦Ì?¨ºy?Y¡ê??¨¦¨°??¨¢¨¨?
    if ((UART1->int_clr & (1 << 8)) == 0U)
    {				
        *data = uart_read_byte();  // ?¨¢ DAT ??¡ä??¡Â??¦Ì?¨ºy?Y
        return true;
    }
    return false;
	#endif
	
    
}

/*********************************************************************
* o¡¥¨ºy??¡êocmdline_rx_callback_polling
* 1|  ?¨¹¡êo?¨¹¨¢?DD?¨®¨º???¦Ì¡Â¡êo¡ä|¨¤¨ª¨ª????¡é??3¦Ì?¡é??¨ª¡§¡Á?¡¤?
* 2?  ¨ºy¡êocc ¨º?¦Ì?¦Ì?¡Á?¡¤?
* ¡¤¦Ì???¦Ì¡êo?T
********************************************************************/
void cmdline_rx_callback_polling(uint8_t cc)
{
    // ¨ª????¨¹¡ä|¨¤¨ª
    if (cc == '\b' || cc == 0x7F)
    {
        if (g_cmdline_ctl.cmdline_buff_cnt > 0)
            g_cmdline_ctl.cmdline_buff_cnt--;
        return;
    }

    // ??3¦Ì/??DD¡êo?¨¹¨¢??¨¢¨º?
    if (cc == '\r' || cc == '\n')
    {
        if (g_cmdline_ctl.cmdline_buff_cnt > 0)
        {
            g_cmd_buf[g_cmdline_ctl.cmdline_buff_cnt] = 0;  // ¡Á?¡¤?¡ä??¨¢¨º?¡¤?
            g_cmdline_ctl.ready_flag = true;                // ?¨¹¨¢?¡Á?¡À?o?
        }
        return;
    }

    // ??¨ª¡§¡Á?¡¤?¡êo¡ä?¨¨??o3???
    if (g_cmdline_ctl.cmdline_buff_cnt < CMD_BUF_SIZE - 1)
    {
        g_cmd_buf[g_cmdline_ctl.cmdline_buff_cnt++] = cc;
    }
    else
    {
        // ?o3????¨²¡ê?????
        memset(g_cmd_buf, 0, CMD_BUF_SIZE);
        g_cmdline_ctl.cmdline_buff_cnt = 0;
    }
}

/*********************************************************************
* o¡¥¨ºy??¡êouart_send_data_polling
* 1|  ?¨¹¡êo???¡¥¡¤?¨º?¡¤¡é?¨ª¨°?¡ä?¨ºy?Y
* 2?  ¨ºy¡êotx_buff ¡¤¡é?¨ª?o3???¡ê?tx_size 3¡è?¨¨
* ¡¤¦Ì???¦Ì¡êo?T
********************************************************************/
void uart_send_data_polling(const uint8_t *tx_buff, uint32_t tx_size)
{
	#ifdef UART0_SPI
	for (uint32_t i = 0; i < tx_size; i++)
    {
        while ((UART0->int_clr & (1 << 3)) != 0);
        UART0->dat = tx_buff[i];
    }
	#else
    for (uint32_t i = 0; i < tx_size; i++)
    {
        while ((UART1->int_clr & (1 << 3)) != 0);
        UART1->dat = tx_buff[i];
    }
	#endif
}





/*********************************************************************
* o¡¥¨ºy??¡êocmd_help
* 1|  ?¨¹¡êo¡ã??¨²?¨¹¨¢?¡ê?¡ä¨°¨®??¨¦¨®??¨¹¨¢?¨¢D¡À¨ª
********************************************************************/
int cmd_help(int argc, char *argv[])
{
    cmdline_entry *entry = (cmdline_entry *)(&menu[0]);

    INFO_e("\nAvailable commands\n");
    INFO_e("------------------\n");

    while (entry->cmd)
    {
        INFO_e("%s%s\n", entry->cmd, entry->help);

        entry++;
    }
 
    return 0;
}



/*********************************************************************
* o¡¥¨ºy??¡êocmdline_process
* 1|  ?¨¹¡êo?a???¨¹¨¢?¡Á?¡¤?¡ä?¡ê??£¤??2¡é?¡äDD?¨¹¨¢?
********************************************************************/
int cmdline_process(uint8_t *cmd, cmdline_entry *entry)
{
    char *argv[argc_num] = {0};
    int argc = 0;
    uint8_t *p = cmd;
    int find_flag = 1;

    // ¡ã¡ä????¡¤????¨¹¨¢?o¨ª2?¨ºy
    while (*p)
    {
        if (*p == ' ' || *p == '#')
        {
            *p = 0;
            find_flag = 1;
        }
        else
        {
            if (find_flag && argc < argc_num)
            {
                argv[argc++] = (char *)p;
                find_flag = 0;
            }
        }
        p++;
    }

    if (argc == 0) return CMDLINE_TOO_FEW_ARGS;

    // 2¨¦?¨°2¡é?¡äDD?¨¹¨¢?
    while (entry->cmd)
    {
        if (!strcmp(argv[0], entry->cmd))
        {
            return entry->pfn_cmd(argc, argv);
        }
        entry++;
    }

    return CMDLINE_BAD_CMD;
}

/*********************************************************************
* o¡¥¨ºy??¡êocmdline_handle_polling
* 1|  ?¨¹¡êo?¨¹¨¢?DD?¡Â¡ä|¨¤¨ªo¡¥¨ºy¡êo?¨®¨º?¨ºy?Y?¨²?a???¨²?¡äDD?¨²????
* ?¦Ì  ?¡Â¡êo?¨²?¡Â?-?¡¤while(1)?D¦Ì¡Â¨®?
********************************************************************/
void cmdline_handle_polling(void)
{
    uint8_t ch;
    uint32_t timeout = 0;

    // ?-?¡¤?¨®¨º?¡ê??¡À¦Ì?¨º?¦Ì???3¦Ì¡ê¡§ready_flag=1¡ê?
    while (g_cmdline_ctl.ready_flag == false)
    {		
        if (uart_receive_data_polling_for_cmdline(&ch))
        {
            timeout = 0;
            cmdline_rx_callback_polling(ch);  // ¡ä|¨¤¨ª¨º?¦Ì?¦Ì?¡Á?¡¤?
        }
        else
        {
            timeout++;
            if (timeout > 1000) return;  // 3?¨º¡À¨ª?3?
        }
    }

    // ¡ä¨°¨®?¨º?¦Ì?¦Ì??¨¹¨¢?
    INFO_e("cmd handle:\n");
    uart_send_data_polling((uint8_t *)"cmd> ", 5);
    uart_send_data_polling((uint8_t *)g_cmd_buf, g_cmdline_ctl.cmdline_buff_cnt);
    uart_send_data_polling((uint8_t *)"\r\n", 2);

    // ?¡äDD?¨¹¨¢??a??
    int ret = cmdline_process((uint8_t *)g_cmd_buf, (cmdline_entry *)menu);

    // ¡ä¨ª?¨®¨¬¨¢¨º?
    if (ret == CMDLINE_BAD_CMD)        INFO_e("Bad command\n");
    if (ret == CMDLINE_TOO_MANY_ARGS)  INFO_e("Too many args\n");
    if (ret == CMDLINE_TOO_FEW_ARGS)   INFO_e("No command\n");

    // ?????o3???¡ê?¡Á?¡À???¨°?¡ä??¨®¨º?
    memset(g_cmd_buf, 0, CMD_BUF_SIZE);
    g_cmdline_ctl.cmdline_buff_cnt = 0;
    g_cmdline_ctl.ready_flag = false;
}

