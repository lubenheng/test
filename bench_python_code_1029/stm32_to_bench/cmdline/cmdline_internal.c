#include "string.h"
#include "stdbool.h"
#include "stdio.h"
#include "cmdline_internal.h"
#include "test.h" 

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
/*
#define DMA_CMDLINE_MENU  \
		{"cmd_dma_modeInit_test",cmd_dma_modeInit_test,  " cmd_dma_modeInit_test"}, \



*/
const cmdline_entry menu[] = {
	{"cmd_help",                 	cmd_help,       			"  Display list of commands"},
//	{"cmd_mem_read", 	            cmd_mem_read, 	   			"  cmd_mem_read:reg_address,len"},
//	{"get_version", 	            get_version, 	   			"  get_version"},
//	{"cmd_mem_write",		        cmd_mem_write, 				"  cmd_mem_write:reg_address,len,val0,val1..."},
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


// 锟解部锟斤拷锟斤拷锟斤拷UART1锟斤拷锟斤拷锟斤拷锟紿AL锟斤拷锟绞硷拷锟斤拷募锟斤拷卸锟斤拷澹�
UART_HandleTypeDef Uart1_Handle_Struct;
UART_HandleTypeDef Uart2_Handle_Struct;

// 锟斤拷锟斤拷锟叫斤拷锟秸伙拷锟斤拷锟斤拷锟斤拷小
#define CMD_BUF_SIZE 128
// 全锟斤拷锟斤拷锟斤拷锟叫斤拷锟秸伙拷锟斤拷锟斤拷
char g_cmd_buf[CMD_BUF_SIZE];

// 锟斤拷锟斤拷锟叫匡拷锟狡结构锟斤拷
// 锟斤拷锟节硷拷录锟斤拷锟秸筹拷锟饺★拷锟斤拷锟斤拷锟角凤拷准锟斤拷锟斤拷
typedef struct {
    volatile uint32_t cmdline_buff_cnt;  // 锟斤拷前锟斤拷锟秸碉拷锟街凤拷锟斤拷锟斤拷
    volatile bool ready_flag;            // 锟斤拷锟斤拷锟斤拷锟斤拷锟缴憋拷志锟斤拷锟秸碉拷锟截筹拷为true锟斤拷
} cmdline_ctl;

// 全锟斤拷锟斤拷锟斤拷锟叫匡拷锟狡憋拷锟斤拷锟斤拷锟斤拷始锟斤拷为0
cmdline_ctl g_cmdline_ctl = {0};

/*********************************************************************
* 锟斤拷锟斤拷锟斤拷锟斤拷Uart_cmdline_Init
* 锟斤拷  锟杰ｏ拷USART2 锟斤拷始锟斤拷锟斤拷PA2 TX锟斤拷PA3 RX锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟叫斤拷锟斤拷
* 锟斤拷  锟斤拷锟斤拷锟斤拷
* 锟斤拷锟斤拷值锟斤拷锟斤拷
********************************************************************/
void Uart_cmdline_Init(void)
{
    // 使锟斤拷 USART2 锟斤拷 GPIOB 时锟斤拷
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct;

    // TX 锟斤拷锟斤拷锟斤拷锟斤拷 PA2
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;          // 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟�
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Pull = GPIO_PULLUP;              // 锟斤拷锟斤拷
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;    // 锟斤拷锟斤拷
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;     // 锟斤拷锟斤拷为 USART2
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // RX 锟斤拷锟斤拷锟斤拷锟斤拷 PA3
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    // USART2 锟斤拷始锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷
    Uart1_Handle_Struct.Instance = USART2;
    Uart1_Handle_Struct.Init.BaudRate = 115200;                // 锟斤拷锟斤拷锟斤拷115200
    Uart1_Handle_Struct.Init.WordLength = UART_WORDLENGTH_8B;  // 8位锟斤拷锟斤拷位
    Uart1_Handle_Struct.Init.StopBits = UART_STOPBITS_1;       // 1位停止位
    Uart1_Handle_Struct.Init.Parity = UART_PARITY_NONE;        // 锟斤拷校锟斤拷
    Uart1_Handle_Struct.Init.HwFlowCtl = UART_HWCONTROL_NONE;  // 锟斤拷硬锟斤拷锟斤拷锟斤拷
    Uart1_Handle_Struct.Init.Mode = UART_MODE_TX_RX;           // 锟秸凤拷模式

    // 锟斤拷始锟斤拷 USART2
    HAL_UART_Init(&Uart1_Handle_Struct);
		INFO_e("uart cmdline ready\r\n");
}

/*********************************************************************
* 锟斤拷锟斤拷锟斤拷锟斤拷fputc
* 锟斤拷  锟杰ｏ拷锟截讹拷锟斤拷printf锟斤拷使锟斤拷USART2锟斤拷锟斤拷锟斤拷锟斤拷
* 锟斤拷  锟斤拷锟斤拷ch 要锟斤拷锟酵碉拷锟街凤拷
* 锟斤拷锟斤拷值锟斤拷锟斤拷锟截凤拷锟酵碉拷锟街凤拷
********************************************************************/
int fputc(int ch, FILE *f)
{
    // 锟饺达拷锟斤拷锟酵寄达拷锟斤拷为锟斤拷 
    while ((USART2->ISR & UART_FLAG_TXE) == 0);
    // 锟斤拷要锟斤拷锟酵碉拷锟街凤拷写锟斤拷锟斤拷锟捷寄达拷锟斤拷
    USART2->TDR = (uint8_t)ch;
    return ch;
}

/*********************************************************************
* 锟斤拷锟斤拷锟斤拷锟斤拷uart_read_byte
* 锟斤拷  锟杰ｏ拷锟斤拷USART2锟斤拷取1锟街斤拷锟斤拷锟斤拷 
* 锟斤拷  锟斤拷锟斤拷锟斤拷
* 锟斤拷锟斤拷值锟斤拷锟斤拷取锟斤拷锟斤拷锟街斤拷
********************************************************************/
static uint8_t uart_read_byte(void)
{
    return (uint8_t)(USART2->RDR & 0xFFU);
}

/*********************************************************************
* 锟斤拷锟斤拷锟斤拷锟斤拷uart_receive_data_polling_for_cmdline
* 锟斤拷  锟杰ｏ拷锟斤拷询锟斤拷式锟斤拷锟斤拷1锟街节达拷锟斤拷锟斤拷锟斤拷
* 锟斤拷  锟斤拷锟斤拷*data 锟斤拷锟斤拷锟斤拷锟捷达拷诺锟街�
* 锟斤拷锟斤拷值锟斤拷true 锟秸碉拷锟斤拷锟捷ｏ拷false 未锟秸碉拷
********************************************************************/
bool uart_receive_data_polling_for_cmdline(uint8_t *data)
{
    // RXNE=1 锟斤拷示锟秸碉拷锟斤拷锟斤拷
    if ((USART2->ISR & UART_FLAG_RXNE) != 0U)
    {
        *data = uart_read_byte();
        return true;
    }
    return false;
}

/*********************************************************************
* 锟斤拷锟斤拷锟斤拷锟斤拷cmdline_rx_callback_polling
* 锟斤拷  锟杰ｏ拷锟斤拷锟斤拷锟叫斤拷锟秸回碉拷锟斤拷锟斤拷锟斤拷锟剿格、回筹拷锟斤拷锟斤拷通锟街凤拷
* 锟斤拷  锟斤拷锟斤拷cc 锟秸碉拷锟斤拷锟街凤拷
* 锟斤拷锟斤拷值锟斤拷锟斤拷
********************************************************************/
void cmdline_rx_callback_polling(uint8_t cc)
{
    // 锟剿革拷锟斤拷锟斤拷锟�
    if (cc == '\b' || cc == 0x7F)
    {
        if (g_cmdline_ctl.cmdline_buff_cnt > 0)
            g_cmdline_ctl.cmdline_buff_cnt--;
        return;
    }

    // 锟截筹拷/锟斤拷锟叫ｏ拷锟斤拷锟斤拷锟斤拷锟�
    if (cc == '\r' || cc == '\n')
    {
        if (g_cmdline_ctl.cmdline_buff_cnt > 0)
        {
            g_cmd_buf[g_cmdline_ctl.cmdline_buff_cnt] = 0;  // 锟街凤拷锟斤拷锟斤拷锟斤拷锟斤拷
            g_cmdline_ctl.ready_flag = true;                // 锟斤拷锟斤拷准锟斤拷锟斤拷
        }
        return;
    }

    // 锟斤拷通锟街凤拷锟斤拷锟斤拷锟诫缓锟斤拷锟斤拷
    if (g_cmdline_ctl.cmdline_buff_cnt < CMD_BUF_SIZE - 1)
    {
        g_cmd_buf[g_cmdline_ctl.cmdline_buff_cnt++] = cc;
    }
    else
    {
        // 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟�
        memset(g_cmd_buf, 0, CMD_BUF_SIZE);
        g_cmdline_ctl.cmdline_buff_cnt = 0;
    }
}

/*********************************************************************
* 锟斤拷锟斤拷锟斤拷锟斤拷uart_send_data_polling
* 锟斤拷  锟杰ｏ拷锟斤拷询锟斤拷式锟斤拷锟斤拷一锟斤拷锟斤拷锟斤拷
* 锟斤拷  锟斤拷锟斤拷tx_buff 锟斤拷锟酵伙拷锟斤拷锟斤拷锟斤拷tx_size 锟斤拷锟斤拷
* 锟斤拷锟斤拷值锟斤拷锟斤拷
********************************************************************/
void uart_send_data_polling(const uint8_t *tx_buff, uint32_t tx_size)
{
    for (uint32_t i = 0; i < tx_size; i++)
    {
        while ((USART2->ISR & UART_FLAG_TXE) == 0);
        USART2->TDR = tx_buff[i];
    }
}





/*********************************************************************
* 锟斤拷锟斤拷锟斤拷锟斤拷cmd_help
* 锟斤拷  锟杰ｏ拷锟斤拷锟斤拷锟斤拷锟筋，锟斤拷印锟斤拷锟斤拷锟斤拷锟斤拷锟叫憋拷
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
* 锟斤拷锟斤拷锟斤拷锟斤拷cmdline_process
* 锟斤拷  锟杰ｏ拷锟斤拷锟斤拷锟斤拷锟斤拷锟街凤拷锟斤拷锟斤拷匹锟戒并执锟斤拷锟斤拷锟斤拷
********************************************************************/
int cmdline_process(uint8_t *cmd, cmdline_entry *entry)
{
    char *argv[32] = {0};
    int argc = 0;
    uint8_t *p = cmd;
    int find_flag = 1;

    // 锟斤拷锟秸革拷指锟斤拷锟斤拷锟酵诧拷锟斤拷
    while (*p)
    {
        if (*p == ' ' || *p == '#')
        {
            *p = 0;
            find_flag = 1;
        }
        else
        {
            if (find_flag && argc < 32)
            {
                argv[argc++] = (char *)p;
                find_flag = 0;
            }
        }
        p++;
    }

    if (argc == 0) return CMDLINE_TOO_FEW_ARGS;

    // 锟斤拷锟揭诧拷执锟斤拷锟斤拷锟斤拷
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
* 锟斤拷锟斤拷锟斤拷锟斤拷cmdline_handle_polling
* 锟斤拷  锟杰ｏ拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟捷★拷锟斤拷锟斤拷锟斤拷执锟叫★拷锟斤拷锟斤拷
* 说  锟斤拷锟斤拷锟斤拷锟斤拷循锟斤拷while(1)锟叫碉拷锟斤拷
********************************************************************/
void cmdline_handle_polling(void)
{
    uint8_t ch;
    uint32_t timeout = 0;

    // 循锟斤拷锟斤拷锟秸ｏ拷直锟斤拷锟秸碉拷锟截筹拷锟斤拷ready_flag=1锟斤拷
    while (g_cmdline_ctl.ready_flag == false)
    {
        if (uart_receive_data_polling_for_cmdline(&ch))
        {
            timeout = 0;
            cmdline_rx_callback_polling(ch);  // 锟斤拷锟斤拷锟秸碉拷锟斤拷锟街凤拷
        }
        else
        {
            timeout++;
            if (timeout > 1000) return;  // 锟斤拷时锟剿筹拷
        }
    }

    // 锟斤拷印锟秸碉拷锟斤拷锟斤拷锟斤拷
    INFO_e("cmd handle:\n");
    uart_send_data_polling((uint8_t *)"cmd> ", 5);
    uart_send_data_polling((uint8_t *)g_cmd_buf, g_cmdline_ctl.cmdline_buff_cnt);
    uart_send_data_polling((uint8_t *)"\r\n", 2);

    // 执锟斤拷锟斤拷锟斤拷锟斤拷锟�
    int ret = cmdline_process((uint8_t *)g_cmd_buf, (cmdline_entry *)menu);

    // 锟斤拷锟斤拷锟斤拷示
    if (ret == CMDLINE_BAD_CMD)        INFO_e("Bad command\n");
    if (ret == CMDLINE_TOO_MANY_ARGS)  INFO_e("Too many args\n");
    if (ret == CMDLINE_TOO_FEW_ARGS)   INFO_e("No command\n");

    // 锟斤拷栈锟斤拷锟斤拷锟斤拷锟阶硷拷锟斤拷锟揭伙拷谓锟斤拷锟�
    memset(g_cmd_buf, 0, CMD_BUF_SIZE);
    g_cmdline_ctl.cmdline_buff_cnt = 0;
    g_cmdline_ctl.ready_flag = false;
}

