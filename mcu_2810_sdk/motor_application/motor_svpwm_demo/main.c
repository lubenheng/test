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
uint32_t fuv; 
uint32_t fvw;
uint16_t duty_u;
uint16_t duty_v;
uint16_t duty_w;
uint16_t overmode=1;

int16_t duty_u1=5555;
int16_t duty_v1=5555;
int16_t duty_w1=6666;




void delay_ms(uint32_t ms)
{
    uint32_t load = 24000; // 24MHz / 1000 = 24000
    // 配置SysTick
    SysTick->LOAD = load - 1; // 因为计数到0才触发，所以减1
    SysTick->VAL = 0;         // 清当前值
    SysTick->CTRL = (1 << 2) | (1 << 0); // 使用内部时钟，使能计数，不使能中断
    for (uint32_t i = 0; i < ms; i++) {
						while (!(SysTick->CTRL & (1 << 16))); // 等待COUNTFLAG置位
    }
    SysTick->CTRL = 0; // 关闭SysTick
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

void adc_init()
{
	 uint32_t tmp;
	 //ADC外设时钟
	 DIGCTRL->clk_en0 |= BIT20; 
	
	 //PGA引脚模拟功能
  //P15  A0_output  ADC_4
	IO->ana |= BIT13;   
	
	ANACTRL->EN_b.ADC=1;
	
	ANACTRL->EN_b.VREF=1;
	//ANACTRL->EN_b.VHALF=1;	
	delay_ms(200);
	
	//P16  A1P
	//P17  A1N
	//P30  A1_output  ADC0
	
	//P31  A2P
	//P32  A2N
	//P33  A2_output  ADC1
	
	    // 打开CPU侧中断使能
    NVIC->ISER[0] = BIT17;

    // 中断使能
    ADCCTRL->cr |= BIT1;

    // 数据对齐方式（0：右对齐，1：次高位对齐）
    ADCCTRL->cr |= BIT2;

    // ADC使能
    ADCCTRL->cr |= BIT7;


	 // ADC采样通道选择(Iu(CH0),Iv(CH1),Iw(CH4),Udc(CH2))
	 //  IO->en2 &= ~BIT6;
	 ADCCTRL->msk |= BIT0 | BIT1 | BIT2 | BIT4;

    // ADC group0的采样时长（至少0.125us）
    tmp = ADCCTRL->scyc;
    tmp &= ~(0xf << 0);
    tmp |= 2 << 0;
    ADCCTRL->scyc = tmp;
	
		// ADC group1的采样时长（至少0.125us）
    tmp = ADCCTRL->scyc;
    tmp &= ~(0xf << 4);
    tmp |= 2 << 4;
    ADCCTRL->scyc = tmp;
		
		
    NVIC_EnableIRQ(tim3_irq);
    NVIC_SetPriority(tim3_irq, 1);
}



// 电角度步进值 (每循环一次增加的角度，Q15格式，65536对应360°)
#define ANGLE_STEP 256  // 256 / 65536 * 360 ≈ 1.4°/步，可调
//48 000 000/3 000 000 = 16.0
//24 000 000/1 500 000 = 16.0
//24 000 000/460 800 = 52.08
#define UART0_BAUD_RATE     (float)(52.08) 
#define UART1_BAUD_RATE     (float)(16.0)

void usart1_init()
{
    uint32_t tmp;
    DIGCTRL->clk_en0 |= BIT14; // 1：PLL时钟(48MHz)
    // 开启时钟门控
    DIGCTRL->clk_en0 |= BIT19;
    // 配置uart设备
    tmp = UART1->thr;
    tmp &= ~(0xff << 0);
    tmp |= 11 | 1 << 4;
    UART1->thr = tmp;

    // 波特率300000
    //    UART1->baud  = (uint32_t)((float)CPU_CLK/460800 + 0.5);
    UART1->baud = (uint32_t)((float)16.5);
	//  UART1->baud  = (uint32_t)((float)UART1_BAUD_RATE + 0.5);
    tmp = UART1->en;
    tmp |= 1;
    UART1->en |= tmp;
    IO_2810->EN.pad_txd2s_en = 1;
   // IO_2810->EN.pad_txd2_en = 1;
	
}
void Uart1View(int16_t view1, int16_t view2, int16_t view3, int16_t view4)
{
    uint8_t sum = 0;
    sum += (uint8_t)(view1);
    sum += (uint8_t)(view1 >> 8);
    sum += (uint8_t)(view2);
    sum += (uint8_t)(view2 >> 8);
    sum += (uint8_t)(view3);
    sum += (uint8_t)(view3 >> 8);
    sum += (uint8_t)(view4);
    sum += (uint8_t)(view4 >> 8);
    UART1->dat = (uint8_t)(view1);
    UART1->dat = (uint8_t)(view1 >> 8);
    UART1->dat = (uint8_t)(view2);
    UART1->dat = (uint8_t)(view2 >> 8);
    UART1->dat = (uint8_t)(view3);
    UART1->dat = (uint8_t)(view3 >> 8);
    UART1->dat = (uint8_t)(view4);
    UART1->dat = (uint8_t)(view4 >> 8);
    UART1->dat = sum;
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
	
	
	
    // 4. FOC 模块配置为手动模式
    FOC->ctrl0 = BIT0   // Ud 手动
                | BIT1   // Uq 手动
                | BIT2   // 角度手动
                | BIT20  // Udc 手动
                | BIT21; // 跳过 Udc 采样; 
    FOC->ctrl0 &= ~(BIT14 | BIT15);  // 关闭电流采样和角度估算
    FOC->ctrl1 = 0;
    FOC->ctrl0 |= BIT17; // BIT17 就是 cfg_s5_s7_swtch，设为1是七段式
		FOC->Ud_lim = ((uint32_t)(-32767) << 16)|((uint32_t)(32767) << 0); // Udmin | Udmax 
		FOC->Uq_lim = ((uint32_t)(-32767) << 16)|((uint32_t)(32767) << 0); // Uqmin | Uqmax
		FOC->Ud = (0<<16|5000);     // 

    FOC->Udc = 16384;         // 	母线电压最大值（15位）
    FOC->angl = 0;            // 电压矢量方向为α轴（Ud轴）

    // 6. 电压增益设为1.0（可选）
		if(overmode==1)
		{
			FOC->wr_Udc_gain = 16384;        // 电压增益1.0
			 FOC->wr_Udq_lim = ((uint32_t)(0.90 * 16384) <<16 ) | (uint32_t)(0.90 * 16384);
		}
    // 6. 等待一次硬件更新
    volatile uint32_t delay = 10000;
    while (delay--);

    // 读取初始占空比（可选）
    fuv = FOC->ro_fuv;
    fvw = FOC->ro_fvw;
    duty_u = fuv & 0x1FFF;
    duty_v = (fuv >> 16) & 0x1FFF;
    duty_w = (fvw >> 16) & 0x1FFF;
}


int debug_flag=0;

extern int16_t Iu_d;
extern int16_t Iv_d;
extern int16_t Iw_d;

int main(void)
{
	
		delay_ms(2000);
	  uint16_t angle = 0;
	if(debug_flag==1)
	{
		 PMU->wr_key = 0x55aaaa55;
    PMU->cfg_normal = 0<<0 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 0<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10; //DRV3 V1.4
	
    System_Init();
	
		IO->ana &= ~(0xffU << 24);
    GPIO->out_en |= BIT7;
		
    svpwm_test_init();
		usart1_init();
    // 电角度变量 (Q15格式，0~65535)
		adc_init();  
		pga_init();
		
		
    timer3_init(1000);
		
	}
	
    while (1)
    {
			
			GPIO->out |= BIT7;
			
			
				if(overmode==1)
				{
					// 更新电角度：不断旋转电压矢量
						angle += ANGLE_STEP;
						FOC->angl = angle;
				}
//        // 延时，控制旋转速度（20kHz PWM 周期约50us，这里延时约1ms，旋转一圈约 65536/256 * 1ms ≈ 256ms）
        for (volatile int i = 0; i < 1000; i++);
				
				
//        // 可选：实时读取占空比，用于调试（可接入J-Scope或串口）
        fuv = FOC->ro_fuv;
        fvw = FOC->ro_fvw;
        duty_u = fuv & 0x1FFF;
        duty_v = (fuv >> 16) & 0x1FFF;
        duty_w = (fvw >> 16) & 0x1FFF;
				

				
//				//Uart1View(0x1888,0x1a88,0x1988,0);
//				Uart1View(Iu_d,Iu_d,Iu_d,0);
	//			Uart1View(duty_u1,duty_v1,duty_w1,duty_w1);
    }
}


