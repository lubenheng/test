#include <stdio.h>
#include <string.h>
#include <math.h>
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
#include "para_define.h"
#include "hardinit.h"
#include "main.h"
#include "gpio_hal.h"
#include "tim_hal.h"
#include <stdint.h>
#include "motor_Control.h"




void delay_ms(uint32_t ms)
{
    uint32_t load = 24000; // 24MHz / 1000 = 24000
    // 配置SysTick
    SysTick->LOAD = load - 1; // 因为计数到0才触发，所以减1
    SysTick->VAL = 0;         // 清当前值
    SysTick->CTRL = (1 << 2) | (1 << 0); // 使用内部时钟，使能计数，不使能中断

    for (uint32_t i = 0; i < ms; i++)
    {
        while (!(SysTick->CTRL & (1 << 16))); // 等待COUNTFLAG置位
    }

    SysTick->CTRL = 0; // 关闭SysTick
}

int debug_flag = 0;
int main(void)
{
    delay_ms(2000);
   
	  if(debug_flag == 1)
    {
        PMU->wr_key = 0x55aaaa55;
        PMU->cfg_normal = 0 << 0 | 1 << 2 | 1 << 3 | 1 << 4 | 0 << 5 | 0 << 6 | 1 << 7 | 1 << 8 | 1 << 9 | 1 << 10; //DRV3 V1.4
        System_Init();
        IO->ana &= ~(0xffU << 24);
        GPIO->out_en |= BIT7;
        UART1_init();
        ADC_Init();
        PGA_init();
				Get_ADC_Offset();
			  FOC_Init();
        timer3_init(3000);
			 	Driver_init();
			  Brige_Output_On();
			 
    }
    while (1)
    {
        GPIO->out |= BIT7;
    }
}

