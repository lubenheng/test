#ifndef GPIO_H
#define GPIO_H

#include "CMSDK_CM0.h"
#include <stdio.h>
#include <string.h>
#include "CMSDK_CM0.h"
#include "system_CMSDK_CM0.h"
#include "core_cm0.h"
#include "Common.h"
#include "gpio.h"
#include "gpio_reg.h"


void GPIO_RST_Set(GPIO_ENABLE en)
{
	if(en == ENABLE)
	{
		IO_2810->EN.pad_rstn_en = 1;
	}
	else
	{
		IO_2810->EN.pad_rstn_en = 0;
	}
}

 

//extern uint8_t GPIO_In(uint32_t msk);
//extern void GPIO_Out(uint32_t msk, uint8_t lvl);
//extern void GPIO_SetMode(uint32_t msk, uint8_t mode);

#endif

////{
////    uint32_t    msk2;
////
////    msk2 = (msk << 8);

////    if (lvl == 0)
////        GPIO->gpio4 &= ~msk2;
////    else if (lvl == 1)
////        GPIO->gpio4 |= msk2;
////}

//// void GPIO_SetMode(uint32_t msk, uint8_t mode)
////{
////     uint32_t    msk2;
////
////     msk2 = (msk << 16);

////    if (mode == 0)
////        GPIO->gpio4 &= ~msk2;
////    else
////        GPIO->gpio4 |= msk2;
////}

//void GPIO_P05_interrupt_init(void) // P05 ÉÏÉýÑØ´¥·¢ÖÐ¶Ï
//{
//	//	IO->ana &= ~(0xffU << 8);
//	GPIO->int_en |= BIT5;
//	GPIO->int_type |= BIT5;
//	GPIO->int_pol |= BIT5;
//	NVIC_EnableIRQ(int0_irq);
//	NVIC_SetPriority(int0_irq, 1);
//}

//void GPIO_P15_interrupt_init(void) // P15 ÉÏÉýÑØ´¥·¢ÖÐ¶Ï
//{
//	IO->ana &= ~(0xffU << 8);
//	GPIO->int_en |= BIT13;
//	GPIO->int_type |= BIT13;
//	GPIO->int_pol |= BIT13;
//	NVIC_EnableIRQ(int1_irq);
//	NVIC_SetPriority(int1_irq, 1);
//}
//void GPIO_P25_interrupt_init(void) // P25 ÉÏÉýÑØ´¥·¢ÖÐ¶Ï
//{
//	IO->ana &= ~(0xffU << 16);
//	GPIO->int_en |= BIT21;
//	GPIO->int_type |= BIT21;
//	GPIO->int_pol |= BIT21;
//	NVIC_EnableIRQ(int2_irq);
//	NVIC_SetPriority(int2_irq, 1);
//}

//void GPIO_P35_interrupt_init(void) // P35 ÉÏÉýÑØ´¥·¢ÖÐ¶Ï
//{
//	IO->ana &= ~(0xffU << 24);
//	GPIO->int_en |= BIT29;
//	GPIO->int_type |= BIT29;
//	GPIO->int_pol |= BIT29;
//	NVIC_EnableIRQ(int3_irq);
//	NVIC_SetPriority(int3_irq, 1);
//}
//void GPIO_P45_interrupt_init(void) // P45 ÉÏÉýÑØ´¥·¢ÖÐ¶Ï
//{

//	GPIO->p4_int_set |= BIT21 | BIT29;

//	NVIC_EnableIRQ(int4_irq);
//	NVIC_SetPriority(int4_irq, 1);
//}
