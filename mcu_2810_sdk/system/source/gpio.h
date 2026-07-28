#ifndef H_GPIO
#define H_GPIO


#include "CMSDK_CM0.h"

typedef enum{
	ENABLE,
	DISABLE
}GPIO_ENABLE;

void GPIO_RST_Set(GPIO_ENABLE en);
void GPIO_RST_Pin_Set(void);


//extern uint8_t GPIO_In(uint32_t msk);
//extern void GPIO_Out(uint32_t msk, uint8_t lvl);
//extern void GPIO_SetMode(uint32_t msk, uint8_t mode);
//void GPIO_P05_interrupt_init(void);
//void GPIO_P15_interrupt_init(void);
//void GPIO_P25_interrupt_init(void);
//void GPIO_P35_interrupt_init(void);
//void GPIO_P45_interrupt_init(void);//P45 é?éy??′￥·￠?D??

#endif

