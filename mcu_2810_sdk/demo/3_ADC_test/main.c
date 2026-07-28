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

void P13_24MHz_Clock_Output(void)
{
    volatile uint32_t *io_ana   = (volatile uint32_t *)(IO_BASE_2810 + 0x1C);    // IO_ANA 寄存器 @ 0x4000141C
    volatile uint32_t *ana_en   = (volatile uint32_t *)(ANA_CTRL_BASE + 0x00);   // ANA_CTRL_EN @ 0x40005000
    volatile uint32_t *ana_sel  = (volatile uint32_t *)(ANA_CTRL_BASE + 0x08);   // ANA_CTRL_OTHERS @ 0x40005008
		volatile uint32_t *ana_24TRIM   = (volatile uint32_t *)(ANA_CTRL_BASE + 0x0C);   // ANA_24TRIM @ 0x4000500C
    // 步骤1: 使能 P1.3(P13) 模拟功能
    // Pad1_ana_en 在 IO->ANA 寄存器的 [15:8] 位
    // P1.3 对应 (8 + 3) = bit 11
    *io_ana |= (1UL << 11);

    // 步骤2: 设置 RW_TEST0_SEL[18:16] = 110b（选择 OSC 24M 输出）
    *ana_sel &= ~(0x7UL << 16);         // 先清除 bits[18:16]
    *ana_sel |= (0x6UL << 16);          // 设置 110b

    // 步骤3: 使能 RW_TEST0_EN[15] = 1（使能测试输出）
    *ana_en |= (1UL << 15);
	// 步骤4:设置休调值
	*ana_24TRIM = (*ana_24TRIM & ~(0x3F << 8)) | (0x27 << 8);
}

void adc_init()
{
	 uint32_t tmp;
	 //ADC外设时钟
	 DIGCTRL->clk_en0 |= BIT20; 
	
	 //PGA引脚模拟功能
  //P15  A0_output  ADC_4
	IO->ana |= BIT13;   
	
	ANACTRL->EN_b.ADC=1;
	
	// VREF已在Vref_init()中使能，此处不再重复配置
	//ANACTRL->EN_b.VHALF=1;	
	//delay_ms(200);
	
	//P16  A1P
	//P17  A1N
	//P30  A1_output  ADC0
	
	//P31  A2P
	//P32  A2N
	//P33  A2_output  ADC1
	
	    // 打开CPU侧中断使能
//    NVIC->ISER[0] = BIT17;

//    // 中断使能
//    ADCCTRL->cr |= BIT1;

    // 数据对齐方式（0：右对齐，1：次高位对齐）
  //  ADCCTRL->cr |= BIT2;

    // ADC使能
    ADCCTRL->cr |= BIT7;


	 // ADC采样通道选择(Iu(CH0),Iv(CH1),Iw(CH4),Udc(CH2))
	 //  IO->en2 &= ~BIT6;
	 ADCCTRL->msk |= BIT6;

    // ADC group0的采样时长（至少0.125us）
//    tmp = ADCCTRL->scyc;
//    tmp &= ~(0xf << 0);
//    tmp |= 2 << 0;
//    ADCCTRL->scyc = tmp;
//	
//		// ADC group1的采样时长（至少0.125us）
//    tmp = ADCCTRL->scyc;
//    tmp &= ~(0xf << 4);
//    tmp |= 2 << 4;
//    ADCCTRL->scyc = tmp;
//		
//		//		// ADC group3的采样时长（至少0.125us）
//    tmp = ADCCTRL->scyc;
//    tmp &= ~(0xf << 10);
//    tmp |= 2 << 10;
//    ADCCTRL->scyc = tmp;
		

}
void pga_init()
{
	 //PGA外设时钟---无数字时钟
	
	
	 //PGA引脚模拟功能
	//P13  A0P
	//P14  A0N
  //P15  A0_output
	IO->ana |= BIT11|BIT12|BIT13;   

	
	//P16  A1P
	//P17  A1N
	//P30  A1_output
	
	//P31  A2P
	//P32  A2N
	//P33  A2_output
	
	 // 使能PGA
		ANACTRL->EN_b.AMP0=1;
		ANACTRL->EN_b.AMP1=1;
		ANACTRL->EN_b.AMP2=1;		
	 
	 // 使能PGA
		ANACTRL->EN_b.PGA0=1;
		ANACTRL->EN_b.PGA1=1;
		ANACTRL->EN_b.PGA2=1;	
	
	 //设置运放放大倍数为8倍
	  ANACTRL->OTHERS_b.PGA0_GAIN=2;
	  ANACTRL->OTHERS_b.PGA1_GAIN=2;
	  ANACTRL->OTHERS_b.PGA2_GAIN=2;

}
void Vref_init(void)
{
    uint32_t tmp;                                                               // 临时变量，用于读写寄存器

    // (0x40002800+0x1c) Pad1_ana_en: 配置PAD模拟功能 (bits 8-15)
    IO->ana |= 0xFF << 8;                                                       // Pad1_ana_en = 0xFF, 使能PAD模拟功能

    // (0x40005000) VREF_EN: VREF使能 bit 8
    ANACTRL->EN_b.VREF = 1;                                                     // VREF_EN = 1, VREF使能

    // (0x40005000) VHALF_EN: VHALF使能 bit 7
    ANACTRL->EN_b.VHALF = 1;                                                    // VHALF_EN = 1, VHALF使能

    // (0x40005008) VREF_SEL: VREF选择 bits [1:0] = 0x03
    tmp = ANACTRL->OTHERS;                                                      // 读取OTHERS寄存器当前值
    tmp &= ~(0x03 << 0);                                                        // 清除VREF_SEL位域 bits[1:0]
    tmp |= (0x03 << 0);                                                         // 设置VREF_SEL = 0x03
    ANACTRL->OTHERS = tmp;                                                      // 写回OTHERS寄存器

    // (0x4000500C) VBG_TRIM: 带隙调整 bits [26:24] = 0x7
    tmp = ANACTRL->TRM;                                                         // 读取TRM寄存器当前值
    tmp &= ~(0x07 << 24);                                                       // 清除VBG_TRIM位域 bits[26:24]
    tmp |= (0x07 << 24);                                                        // 设置VBG_TRIM = 0x7
    ANACTRL->TRM = tmp;                                                         // 写回TRM寄存器

    // (0x4000500C) VREF_TRIM: VREF调整 bits [5:0] = 0x20
    tmp = ANACTRL->TRM;                                                         // 读取TRM寄存器当前值
    tmp &= ~(0x3F << 0);                                                        // 清除VREF_TRIM位域 bits[5:0]
    tmp |= (0x29 << 0);                                                         // 设置VREF_TRIM = 0x20
    ANACTRL->TRM = tmp;                                                         // 写回TRM寄存器

   // delay_ms(200);                                                              // 延时200ms，等待VREF稳定
}

void svpwm_test_init(void)
{
    // 1. 使能相关时钟
    DIGCTRL->clk_en0 |= BIT10;   // FOC 时钟
    DIGCTRL->clk_en0 |= BIT29;   // DRIVER 时钟
    DIGCTRL->clk_en0 |= BIT9;    // MDU 时钟（FOC 依赖）

    // 2. 配置 GPIO 引脚为 PWM 输出 (假设 P2.0~P2.5)
    IO->en |= 0x3F;   // 使能 P20~P25 的 TIMER2 功能
		//IO->en |= 0xFFF;   // 使能 P20~P25 的 TIMER2 功能
    // 3. DRIVER 模块配置
    DRV->arr = 2400;               // 48MHz / 2400 = 20kHz（中央对齐）
    DRV->dtr = 11;                 // 死区时间 (11+1)/24MHz = 500ns
    DRV->cmr = 0x03F;             // 六路输出使能，极性默认0
    DRV->cr = BIT0   // OE: 输出使能
            | BIT2   // MESEL: FOC 驱动模式
            | BIT3   // OCS: 占空比来源为 FOC
            | BIT5   // FOCEN: FOC 模块使能
            | BIT7;  // DRVEN: 计数器使能
    DRV->out = BIT7;               // MOE: 主输出使能

	  /*
    0：不产生中断
     1：计数器向上计数时产生中断
    2：计数器向下计数时产生中断
    3：计数器无论向上还是向下，都产生中断
    */
    DRV->sr |= BIT1; 
    NVIC_EnableIRQ(dcif_irq);
    NVIC_SetPriority(dcif_irq, 2);
	
}

int debug_flag=1;
int main(void)
{
	delay_ms(1000);
	
	if(debug_flag==1)
	{
	PMU->wr_key = 0x55aaaa55;
	PMU->cfg_normal = 0<<0 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 0<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10; //DRV3 V1.4

IO->ana &= ~(0xffU << 24);
  GPIO->out_en |= BIT7;
	 // 配置 P13(P1.3) 引脚输出 24MHz 时钟
    P13_24MHz_Clock_Output();
	svpwm_test_init();
		
	Vref_init();
	adc_init(); 
	//pga_init();
		
		
	}
	while (1)
	{ 
		
	}
}
	


