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
#include "SPI_hal.h"
#include "SPI_reg.h"
extern spi_reg_t *const volatile SPI;

int main(void)
{
	
	delay_ms(4000);
	
	
	PMU->wr_key = 0x55AAAA55;
	PMU->cfg_normal = 0<<0 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 0<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10; //DRV3 V1.4
 
//	Uart_cmdline_Init();
	SPI_Uart_modified();   //1.5Mhz
	GPIO_RST_Set(ENABLE);
	INFO_e("success========== \r\n");
	
	while (1)
	{ 
 
		cmdline_handle_polling();
 
		
	}
}
 
