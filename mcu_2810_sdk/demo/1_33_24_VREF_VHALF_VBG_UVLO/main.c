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

int16_t VREFSEL=0x03UL; // 设置00=4.5V，01=5V, 10=3V,11=4V

#define IO_BASE_2810        0x40001400UL
#define ANA_CTRL_BASE       0x40005000UL


void P13_33KHz_Clock_Output(void)
{
		volatile uint32_t *io_ana   = (volatile uint32_t *)(IO_BASE_2810 + 0x1C);    // IO_ANA 寄存器 @ 0x4000141C
		volatile uint32_t *VREF_en   = (volatile uint32_t *)(ANA_CTRL_BASE + 0x00);   // ANA_CTRL_EN @ 0x40005000
		volatile uint32_t *VHALF_en   = (volatile uint32_t *)(ANA_CTRL_BASE + 0x00);   // ANA_CTRL_EN @ 0x40005000
		volatile uint32_t *ADC_en   = (volatile uint32_t *)(ANA_CTRL_BASE + 0x00);   // ANA_CTRL_EN @ 0x40005000
		volatile uint32_t *ana_en   = (volatile uint32_t *)(ANA_CTRL_BASE + 0x00);
		volatile uint32_t *Test1_en   = (volatile uint32_t *)(ANA_CTRL_BASE + 0x00);
		volatile uint32_t *VREF_sel  = (volatile uint32_t *)(ANA_CTRL_BASE + 0x08);   // ANA_CTRL_OTHERS @ 0x40005008
		volatile uint32_t *ana_sel  = (volatile uint32_t *)(ANA_CTRL_BASE + 0x08);
		volatile uint32_t *RW_LVW_SEL  = (volatile uint32_t *)(ANA_CTRL_BASE + 0x08);
		volatile uint32_t *RW_LVR_SEL  = (volatile uint32_t *)(ANA_CTRL_BASE + 0x08);
		volatile uint32_t *Test1_SEL  = (volatile uint32_t *)(ANA_CTRL_BASE + 0x08);
		volatile uint32_t *ana_33KTRIM   = (volatile uint32_t *)(ANA_CTRL_BASE + 0x0C); 
		volatile uint32_t *VREF_TRIM   = (volatile uint32_t *)(ANA_CTRL_BASE + 0x0C);   // ANA_24TRIM @ 0x4000500C
		volatile uint32_t *VBG_TRIM   = (volatile uint32_t *)(ANA_CTRL_BASE + 0x0C);   // ANA_24TRIM @ 0x4000500C
		volatile uint32_t *ana_24TRIM   = (volatile uint32_t *)(ANA_CTRL_BASE + 0x0C);   // ANA_24TRIM @ 0x4000500C
		volatile uint32_t *RW_OSC_CHANGE_EN= (volatile uint32_t *)(ANA_CTRL_BASE);
	
	// 步骤1: 使能 P1.3(P13) 模拟功能
    // Pad1_ana_en 在 IO->ANA 寄存器的 [15:8] 位
    // P1.3 对应 (8 + 3) = bit 11
    *io_ana |= (0xFFFFFFUL << 8);

    // 步骤2: 设置 RW_TEST0_SEL[18:16] = 110b（选择 OSC 24M 输出）
    *VREF_sel &= ~(0x3UL << 0);         // 先清除 bits[3:0]
//	
//	/////////////////////////////////////////////
    *VREF_sel |= (VREFSEL << 0);          // 设置00=4.5V，01=5V, 10=3V,11=4V
//	/////////////////////////////////////////////
	
	*ana_sel &= ~(0x7UL << 16);         // 先清除 bits[18:16]
    *ana_sel |= (0x7UL << 16);          // 设置 111b引脚p13输出33KHZ，设置 110b引脚p13输出24MHZHZ，设置 101b引脚p13输出UVLO_INT，
//		
//	*RW_LVW_SEL &= ~(0x1UL << 4);       // 先清除 bits[0]//INT
//	*RW_LVW_SEL |= (0x1UL << 4);          // 设置 1
//	
//	*RW_LVR_SEL &= ~(0x3UL << 2);       // 先清除 bits[0]//RSTN
//	*RW_LVR_SEL |= (0x0UL << 2);          // 设置 1
	
	*Test1_SEL &= ~(0xfUL << 19);       // 先清除 bits[0]//RSTN
	*Test1_SEL |= (0x04UL << 19);          // 设置 1
		
		
    // 步骤3: 使能 RW_TEST0_EN[15] = 1（使能测试输出）
    *VREF_en |= (0x1UL << 8);
	*VHALF_en |= (0x1UL << 7);
	*ADC_en |= (0x1UL << 1);
	*ana_en |= (0x1UL << 15);
	*Test1_en |= (0x1UL << 16);
//	*RW_OSC_CHANGE_EN |= (0x1UL << 0);
//	
//	// 步骤4:设置休调值
	*VREF_TRIM = (*VREF_TRIM & ~(0x3F << 0)) | (0x29 << 0);//25:0X1d
//		// 步骤4:设置休调值
	*VBG_TRIM = (*VREF_TRIM & ~(0x07 << 24)) | (0x06<< 24);//0X07//
//		// 步骤4:设置休调值
	*ana_33KTRIM = (*ana_33KTRIM & ~(0x3F << 16)) | (0x45 << 16);//0x45
//		// 步骤4:设置休调值
	*ana_24TRIM = (*ana_24TRIM & ~(0x3F << 8)) | (0x37 << 8);//0x29
}
////////////////////////////////////////////////////////////////////////////////////////


int debug_flag=0;
int main(void)
{
	delay_ms(1000);
	
	
	PMU->wr_key = 0x55aaaa55;
	PMU->cfg_normal = 0<<0 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 0<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10; //DRV3 V1.4
    // 配置 P13(P1.3) 引脚输出 24MHz 时钟
    P13_33KHz_Clock_Output();
	while (1)
	{ 
		
	}
}
	


