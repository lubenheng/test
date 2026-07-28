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
int a=0,b=0,c=0,d=0,e=0;
int f=0;


//////////////////////////////////////////////////////////////////
#define ANA_CTRL_BASE       0x40005000UL
#define DIG_CTRL_BASE       0x40002400UL
#define CMOP_BASE_2810      0x40004400UL
#define IO_BASE_2810        0x40001400UL


/**
 * @brief BEMF测试
 * 
 * 

 */
void BEMF_Test(void)
{
		volatile uint32_t *io_ana   = (volatile uint32_t *)(IO_BASE_2810 + 0x1C);    // IO_ANA 寄存器 @ 0x4000141C
		volatile uint32_t *cmp3_en   = (volatile uint32_t *)(CMOP_BASE_2810 + 0x04); 
		volatile uint32_t *CMP_CLOCK_EN   = (volatile uint32_t *)(DIG_CTRL_BASE + 0x08);
		volatile uint32_t *RW_HBIAS_EN   = (volatile uint32_t *)(ANA_CTRL_BASE);
	
	
		volatile uint32_t *cmp0mod   = (volatile uint32_t *)(CMOP_BASE_2810 + 0x08); // cmp0mod寄存器 @ 0x40004408
		volatile uint32_t *cmp0sel   = (volatile uint32_t *)(CMOP_BASE_2810 + 0x08);
		volatile uint32_t *cmp_com_sel   = (volatile uint32_t *)(CMOP_BASE_2810 + 0x08);
		volatile uint32_t *cmp0_EN  = (volatile uint32_t *)(CMOP_BASE_2810 + 0x08);
	
	
	// 步骤1: P2模拟使能
    // 内置电阻模式
    // 
    *io_ana |= (0xFFUL << 16);
	*io_ana |= (0xFFUL << 8);
	*CMP_CLOCK_EN |= (0x01UL << 31);
	*cmp3_en |= (0x01UL << 4);
	*RW_HBIAS_EN |= (0x01UL << 10);
	
	*cmp_com_sel |= (0x01UL << 1);
	*cmp0_EN |= (0x01UL << 0);
	
	*cmp0mod &= ~(0x3UL << 5);  
    *cmp0mod |= (0x01UL << 5); 

	*cmp0sel &= ~(0x3UL << 3);  
	*cmp0sel |= (0x01UL << 3);          // 设置
	
}
////////////////////////////////////////////////////////////////////////////////////////


int debug_flag=1;
int main(void)
{
	delay_ms(1000);
	if(debug_flag==1)
	{
	PMU->wr_key = 0x55aaaa55;
	PMU->cfg_normal = 0<<0 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 0<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10; //DRV3 V1.4
	
	BEMF_Test();
	
	while (1)
	{ 
		delay_ms(1000);
		f++;
	}
}
	
}

