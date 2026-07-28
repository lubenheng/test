#ifndef TIM_HAL_H
#define TIM_HAL_H
#include "TIM_Reg.h"
#include "ODS_5010.h"
/*-----------------------计数器----------------------------*/
void TIM_hal_Counter_init(void);
void TIM_hal_Counter_set_div(uint8_t div);
uint32_t TIM_hal_Counter_num(void);
void tim_hal_th_set(uint32_t th);
void tim_hal_cntdir_set(uint8_t dir);

/*------------------------捕获-----------------------------*/
void TIM_hal_Capture_init(void);
uint32_t tim_hal_ch0_capnum(void);
uint32_t tim_hal_ch1_capnum(void);


/*------------------------比较-----------------------------*/
void TIM_hal_Compare_init(void);
void tim_hal_compn_set(uint8_t chx, uint32_t duty);
void tim_hal_compn_pol_set(uint8_t chx, uint8_t pol);
void tim_hal_cnt_mid_set(uint8_t mid);
void tim_hal_onetrig(void);
float tim_hal_ch0_duty(void);
float tim_hal_ch1_duty(void);


/*------------------------混合-----------------------------*/
void TIM_hal_mix_init(void);


/*--------------------common func-------------------------*/
int8_t tim_hal_change(uint8_t timx);
uint8_t tim_hal_irq_status(void);
void tim_hal_irq_disable(void);
void tim_hal_init(void);
void tim_hal_disable(void);
void tim_hal_enable(void);
void tim_hal_para_display(void);
void tim_hal_init(void);
void tim_hal_flt(uint8_t flt);



void HAL_TIM_DMA_compare_Init(void);
void HAL_TIM_caputer_Init(void);

#endif
