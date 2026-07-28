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
 
#include "pmu_hal.h"
 

int main(void)
{
	soft_delay_1ms(1000);

	PMU->wr_key = 0x55aaaa55;
	PMU->cfg_normal = 0<<0 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 0<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10; //DRV3 V1.4
 
	Uart_cmdline_Init();
	
	INFO_e("success========== \r\n");
 
//	PMU_Set_CPU_24M();  //3.5s
//	PMU_Set_CPU_48M();
//	PMU_Set_CPU_12M_RCO();     //FPGA没有时钟树   
	PMU_Config_Sleep();
//	SCB->AIRCR |= ( 1 << 2);
//	PMU_Enter_DeepSleep();
//  GPIO_RST_Set(ENABLE);
 

	uint32_t cnt = 0;
	while (1)
	{ 
		while(cnt < 0xFFFFFFU)
		{
			cnt ++;
		}
		cnt = 0;
		INFO_e("test PMU CLK \r\n");
 
		cmdline_handle_polling();
	}
}
 
