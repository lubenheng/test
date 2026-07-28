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
 
 
#include "cmdline_internal.h"
 
#include "I2C_hal.h"
#include "softinit.h"

 
#define I2C_Txbuff_len 64
#define I2C_Rxbuff_len I2C_Txbuff_len

uint8_t I2C_txbuf[I2C_Txbuff_len] = {2,2,2,2,2,2,3,4,5,6,7,8,9,10,11,12};
uint8_t I2C_rxbuf[I2C_Txbuff_len] ;
int  debug_flag=0;
int main(void)
{
	delay_ms(4000);
//	if(debug_flag==1)
//	{
	
	PMU->wr_key = 0x55aaaa55;
	PMU->cfg_normal = 0<<0 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 0<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10; //DRV3 V1.4
 
	Uart_cmdline_Init();
	
	
	HAL_I2C_Init(&hi2c0);
	for(int i =0;i<I2C_Txbuff_len;i++)
	{
		I2C_txbuf[i] = i;
	}
	
	INFO_e("success========== \r\n");
	while (1)
	{ 
		cmdline_handle_polling();

	}
//}
}


