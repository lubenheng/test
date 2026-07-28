#include <stdio.h>
#include <string.h>
#include "CMSDK_CM0.h"
#include "system_CMSDK_CM0.h"
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
#include "wdog_hal.h"

 

int main(void)
{
	soft_delay_1ms(1000);

	PMU->wr_key = 0x55aaaa55;
	PMU->cfg_normal = 0<<0 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 0<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10; //DRV3 V1.4
 
	Uart_cmdline_Init();
	
	INFO_e("success========== \r\n");
	INFO_e("rst  %d \r\n",Wdog_Read_Cnt());
	
  GPIO_RST_Set(ENABLE);
	Wdog_Reload_Set(1,1);
	Wdog_Interrupt_Set();
	
	while (1)
	{ 
//		Wdog_Feed(); 
//		INFO_e("wdtxx  %d \r\n",Wdog_Read_Cnt());
		cmdline_handle_polling();
	}
}
 
