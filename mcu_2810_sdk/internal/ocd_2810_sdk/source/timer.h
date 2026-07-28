#ifndef TIMER_H
#define TIMER_H

#include "CMSDK_CM0.h"
void timer0_start(uint32_t reload);
void timer0_wait(uint32_t reload);
void timer0_stop(void);
void timer2_init(uint32_t reload);
void timer3_init(uint32_t reload);
void timer4_init(void);
extern volatile uint8_t  timer0_done;

void timer5_init(uint32_t reload);
void timer6_init(uint32_t reload);
void timer2_Capture(void);
void timer4_PWMinit(void);//43ms
#endif
