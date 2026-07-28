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

#define IO_BASE_2810        0x40001400UL
#define ANA_CTRL_BASE       0x40005000UL
#define COMP_BASE           0x40004400UL
#define COMP_CLK            0x40002400UL


void PGA_TEST(void)
{
	volatile uint32_t *ADC_en   = (volatile uint32_t *)(ANA_CTRL_BASE + 0x00);
    volatile uint32_t *VREF_en   = (volatile uint32_t *)(ANA_CTRL_BASE + 0x00);   
	volatile uint32_t *VHALF_en   = (volatile uint32_t *)(ANA_CTRL_BASE + 0x00); 
	volatile uint32_t *AMP_en   = (volatile uint32_t *)(ANA_CTRL_BASE + 0x00);
	volatile uint32_t *PGA_en   = (volatile uint32_t *)(ANA_CTRL_BASE + 0x00);
	volatile uint32_t *PGA0_GAIN  = (volatile uint32_t *)(ANA_CTRL_BASE + 0x08);
	volatile uint32_t *PGA1_GAIN  = (volatile uint32_t *)(ANA_CTRL_BASE + 0x08);
	volatile uint32_t *PGA2_GAIN  = (volatile uint32_t *)(ANA_CTRL_BASE + 0x08);
	
	volatile uint32_t *IO_ANA   = (volatile uint32_t *)(IO_BASE_2810 + 0x1C);
	volatile uint32_t *RW_TEST1_SEL   = (volatile uint32_t *)(ANA_CTRL_BASE + 0x08);
	volatile uint32_t *RW_TEST1_EN   = (volatile uint32_t *)(ANA_CTRL_BASE);
	
	
    *ADC_en |= (1UL << 1);
	*VREF_en |= (1UL << 8);
	*VHALF_en |= (1UL << 7);
	*AMP_en |= (0x7UL << 3);
	*PGA_en |= (0x7UL << 11);//001:PGA0,010:PGA1,100:PGA2:
	
	*PGA0_GAIN &= ~(0x3UL << 8);         // 先清除 bits[9:8]
    *PGA0_GAIN |= (0x03UL << 8);          // 设置 00:4X 01:8X 10:12X 11；16X
		
	*PGA1_GAIN &= ~(0x3UL << 10);         // 先清除 bits[11:10]
    *PGA1_GAIN |= (0x03UL << 10);          // 设置 00:4X 01:8X 10:12X 11；16X，
		
	*PGA2_GAIN &= ~(0x3UL << 12);         // 先清除 bits[13:12]
    *PGA2_GAIN |= (0x01UL << 12);          // 设置 00:4X 01:8X 10:12X 11；16X
//P13 P14 AMP0正向输入端 和负向输入端 P15 AMP0输出
//P16 P17 AMP1正向输入端 和负向输入端 P30 AMP1输出
//P31 P32 AMP2正向输入端 和负向输入端 P33 AMP2输出

	*IO_ANA |= (0xFFUL << 16);
	*RW_TEST1_EN |= (0x01UL << 16);
	
	*RW_TEST1_SEL |= (0x04UL << 19);//RW_TEST1_SEL(通道切换)
	//0x08  0x09  0x0a  0x0b  0x0c  0x0d  0x0e
	//IB1   IB2   VCM    /    AMP0 AMP1 AMP2



}
////////////////////////////////////////////////////////////////////////////////////////


int debug_flag=0;
int main(void)
{
	delay_ms(1000);
	
	
	PMU->wr_key = 0x55aaaa55;
	PMU->cfg_normal = 0<<0 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 0<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10; //DRV3 V1.4

	
	
    // 配置 P13(P1.3) 引脚输出 
    PGA_TEST();
	while (1)
	{ 
		
	}
}
	


