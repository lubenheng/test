#ifndef _HARD_INIT_H
#define _HARD_INIT_H

#include "main.h"

typedef enum
{
    LED1 = 1,//drive err
    LED2 = 2,//phase loss
    LED3 = 3,//block
    LED4 = 4,//sc
    LED5 = 5 //mos err
} LED_TypeDef;
#define REGULAR_CH_NUM     6    // 规则组：6路（普通数据）
#define INJECTED_CH_NUM    4    // 注入组：4路（三相电流+母线电流）
#define ADC_REGULAR_BUFFER_SIZE 128
extern uint16_t adc_reg_buf[REGULAR_CH_NUM];    // 规则组
extern uint16_t adc_inj_buf[INJECTED_CH_NUM];   // 注入组

void LED_On(int led);
void LED_On(int led);
void LED_Toggle(int led);
void mc_PWM_init(void);
void mc_adc_init(void);
void mc_break_init(void);
void As5047p_SPI_init(void);
void mc_TIM_init(void);
#endif
