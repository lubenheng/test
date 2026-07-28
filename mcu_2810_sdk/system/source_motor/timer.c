#include <stdio.h>
#include <string.h>
#include "CMSDK_CM0.h"
#include "system_CMSDK_CM0.h"
#include "core_cm0.h"
#include "Common.h"
#include "timer.h"

//--------------------------------------------
volatile uint8_t timer0_done = 1;

//--------------------------------------------
void timer0_start(uint32_t reload)
{
    DIGCTRL->clk_en0 |= BIT24;
    NVIC_EnableIRQ(tim0_irq);
    TIM0->reload_val = reload;
    TIM0->en = 3;
    timer0_done = 0;
}

//--------------------------------------------
void timer0_wait(uint32_t reload)
{
    timer0_start(reload);

    while (timer0_done == 0)
        __ISB();

    TIM0->en = 0;
    timer0_done = 0;
    NVIC_DisableIRQ(tim0_irq);
    DIGCTRL->clk_en0 &= ~BIT24;
}

//--------------------------------------------
void timer0_stop(void)
{
    TIM0->en = 0;
    timer0_done = 0;
    NVIC_DisableIRQ(tim0_irq);
    DIGCTRL->clk_en0 &= ~BIT24;
}

void timer2_init(uint32_t reload)
{
    DIGCTRL->clk_en0 |= BIT26;
		TIM2->TIM2_CR0_b.T2MOD=1;
		TIM2->TIM2_CR0_b.T2PSC=1;
		TIM2->TIM2_CR1_b.T2IFE=1;	
	
		TIM2->TIM2_CR1_b.cfg_t2iffe=1;	
		TIM2->TIM2_CR1_b.cfg_t2ipfe=1;	
		TIM2->TIM2_CR1_b.cfg_t2irfe=1;	
	
	
	
		TIM2->TIM2_DR=12400;
	  TIM2->TIM2_ARR=reload;	
		TIM2->TIM2_CR1|=0x9;
	
    NVIC_EnableIRQ(tim2_irq);
		NVIC_SetPriority(tim2_irq,1);
}

void timer2_Capture(void)
{
    DIGCTRL->clk_en0 |= BIT26;
    IO->en  |= 1;	
		
		TIM2->TIM2_CR1_b.T2CEN=0;
		TIM2->TIM2_CR0=0x40;
	
	  TIM2->TIM2_CR1=0;
	  TIM2->TIM2_CR1=0x1c;
	
	  TIM2->TIM2_DR=0;
	  TIM2->TIM2_CNTR=0;
	  TIM2->TIM2_ARR=0;
    NVIC_EnableIRQ(tim2_irq);
	  NVIC_SetPriority(tim2_irq,2);
	  TIM2->TIM2_CR1_b.T2CEN=1;
}

void timer3_init(uint32_t reload)
{
    DIGCTRL->clk_en0 |= BIT27;
    TIM3->cr0 = 0x71; //0x71 -----3MHz CLK
    TIM3->dr = 1000;
    TIM3->arr = reload;
    TIM3->cr1 = 0x709;

    NVIC_EnableIRQ(tim3_irq);
    NVIC_SetPriority(tim3_irq, 1);
}

void timer4_init(void) // 43ms
{

    DIGCTRL->clk_en0 |= BIT28;
	  TIM4->cr0  = 0x81;
	  TIM4->dr  =  1000;
	  TIM4->arr  = 12000;
	  TIM4->cr1  = 0x109;
	
    NVIC_EnableIRQ(tim4_irq);
    NVIC_SetPriority(tim4_irq,1);
	
}
void timer5_init(uint32_t reload)
{
	  DIGCTRL->clk_en0 |= BIT21;
    NVIC_EnableIRQ(tim5_irq);
    TIM5->reload_val = reload;
    TIM5->en         = 3;
  
}
void timer6_init(uint32_t reload)
{
	  DIGCTRL->clk_en0 |= BIT22;
    NVIC_EnableIRQ(tim6_irq);
    TIM6->reload_val = reload;
    TIM6->en         = 3;

}


void timer4_PWMinit(void)
{
//	  GPIO->out_en |= BIT4;
    DIGCTRL->clk_en0 |= BIT28;
		IO->en|=BIT4;
	  TIM4->cr0  = 0x1;
  	TIM4->arr  = 12000;
	  TIM4->dr   = 6000;
	
	  TIM4->cr1  = 0x701;
    NVIC_EnableIRQ(tim4_irq);
    NVIC_SetPriority(tim4_irq, 1);
}
void timer4_FGinit(void)
{
	
    DIGCTRL->clk_en0 |= BIT28;
		IO->en|=BIT4;
	  TIM4->cr0  = 0x91;
	  TIM4->dr  =  1000;
	  TIM4->arr  = 12000;
	  TIM4->cr1  = 0x701;
	
    NVIC_EnableIRQ(tim4_irq);
    NVIC_SetPriority(tim4_irq,1);
}
