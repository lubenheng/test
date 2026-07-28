#include <stdio.h>
#include <string.h>
#include "CMSDK_CM0.h"
#include "core_cm0.h"
#include "Common.h"
#include "uart.h"
#include "gpio.h"
#include "qspi.h"
#include "wdt.h"
#include "timer.h"
#include "mdu.h"
#include <stdarg.h>
#include "INIT.h"
#include "motor_control.h"
#include "para_define.h"

#include "hardinit.h"

#include "main.h"
#include "gpio_hal.h"
#include "adc_hal.h"
 
#include "cmdline_internal.h"
 
#include "pmu.h"
#include "softinit.h" 
int a,b,c,d,e;
//串口需要从P06-Txd2s  A79		P07 Rxd2s  A80更改 P02  A74 TXDS  P03  A75RXDS 功能2   uart0
void SPI_Uart_modified(void)
{
	gpio_cfg_t 	GPIO_Init_Struct;
	
	GPIO_Init_Struct.alt_func	        =	GPIO_ALT_FUNC_0;
	GPIO_Init_Struct.analog_mode      =	GPIO_ANALOG_DISABLE;
	GPIO_Init_Struct.dir              =	GPIO_DIR_OUTPUT;
	GPIO_Init_Struct.pin              =	GPIO_PIN_2;
	GPIO_Init_Struct.port             =	GPIO_PORT_P0;
	GPIO_Init_Struct.pull             =	GPIO_PULL_DISABLE;
	GPIO_Init_Struct.strength         =	GPIO_STRENGTH_LOW;
	gpio_init(&GPIO_Init_Struct);
	GPIO_Init_Struct.alt_func	        =	GPIO_ALT_FUNC_0;
	GPIO_Init_Struct.analog_mode      =	GPIO_ANALOG_DISABLE;
	GPIO_Init_Struct.dir              =	GPIO_DIR_OUTPUT;
	GPIO_Init_Struct.pin              =	GPIO_PIN_3;
	GPIO_Init_Struct.port             =	GPIO_PORT_P0;
	GPIO_Init_Struct.pull             =	GPIO_PULL_DISABLE;
	GPIO_Init_Struct.strength         =	GPIO_STRENGTH_LOW;
	gpio_init(&GPIO_Init_Struct);
	
	
	uint32_t tmp;
	DIGCTRL->clk_en0 |= 0x00004000U; // 1：PLL时钟(48MHz)
	// 开启时钟门控
	DIGCTRL->clk_en0 |= 0x00080000U;
	// 配置uart设备
	tmp = UART0->thr;
	tmp &= ~(0xff << 0);
	tmp |= 11 | 1 << 4;
	UART0->thr = tmp;
 
	UART0->baud = (uint32_t)((float)16 + 0.5);
	tmp = UART0->en;
	tmp |= 1;
	UART0->en |= tmp;

//	IO_2810->EN.pad_txd2s_en = 1;
//	IO_2810->EN.pad_rxd2s_en = 1;
	IO_2810->EN.pad_txds_en = 1;
	IO_2810->EN.pad_rxds_en = 1;
//	UART0->en &=~0x02;
//	UART0->en &=~0x80;    
	
	//Rx
	uint32_t      tmp1;  
 
	tmp1 = UART0->thr;
	tmp1 &= ~(0xff<<8);
	tmp1 |= (11 | 1<<4)<<8;
	UART0->thr   = tmp1;

	tmp1 = UART0->en;
	tmp1 &= ~(0x3f<<8);
	tmp1 |= 0x2c<<8 | 1<<6 	;
	UART0->en = tmp1;
	
}

int debug_flag=0;
int main(void)
{
	delay_ms(4000);
	
//	if(debug_flag==1)
//	{
	PMU->wr_key = 0x55aaaa55;
	PMU->cfg_normal = 0<<0 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 0<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10; //DRV3 V1.4
//	 SPI_Uart_modified();
	
	Uart_cmdline_Init();
	GPIO_RST_Set(ENABLE);
	INFO_e("success========== \r\n");
	COMP0_interrupt_init();
	while (1)
	{ 
//		if(debug_flag==1)
//		{
//			TSD_interrupt_init();
//			debug_flag=0;
//		}
//			
		
		cmdline_handle_polling();
		//	INFO_e("success=====333===== \r\n");
	}
}
//}


