#ifndef TIM_HAL_H
#define TIM_HAL_H
#include "TIM_Reg.h"
#include "ODS_5010.h"
#include "TIM_Type.h"
/*-----------------------统一初始化----------------------------*/
void TIM_hal_init(TIMx_TypeDef *timex, TIMx_config_t *config);

/*-----------------------计数器----------------------------*/
void TIM_hal_Counter_set_div(TIMx_TypeDef *timex, uint8_t div);
uint32_t TIM_hal_Counter_num(TIMx_TypeDef *timex);
void tim_hal_th_set(TIMx_TypeDef *timex, uint32_t th);
void tim_hal_cntdir_set(TIMx_TypeDef *timex, uint8_t dir);

/*------------------------捕获-----------------------------*/
uint32_t tim_hal_ch0_capnum(TIMx_TypeDef *timex);
uint32_t tim_hal_ch1_capnum(TIMx_TypeDef *timex);


/*------------------------比较-----------------------------*/
void tim_hal_compn_set(TIMx_TypeDef *timex, uint8_t chx, uint32_t duty);
void tim_hal_compn_pol_set(TIMx_TypeDef *timex, uint8_t chx, uint8_t pol);
void tim_hal_cnt_mid_set(TIMx_TypeDef *timex, uint8_t mid);
void tim_hal_onetrig(TIMx_TypeDef *timex);
float tim_hal_ch0_duty(TIMx_TypeDef *timex);
float tim_hal_ch1_duty(TIMx_TypeDef *timex);


/*--------------------common func-------------------------*/
int8_t tim_hal_change(uint8_t timx);
uint8_t tim_hal_irq_status(TIMx_TypeDef *timex);
uint8_t tim_hal_irq_en_status(TIMx_TypeDef *timex);
void tim_hal_irq_disable(TIMx_TypeDef *timex);
void tim_hal_reset(TIMx_TypeDef *timex);
void tim_hal_disable(TIMx_TypeDef *timex);
void tim_hal_enable(TIMx_TypeDef *timex);
void tim_hal_para_display(TIMx_TypeDef *timex);
void tim_hal_flt(TIMx_TypeDef *timex, uint8_t flt);
void tim_hal_cnt_set(TIMx_TypeDef *timex, uint32_t cnt_num);



void HAL_TIM_DMA_compare_Init(void);
void HAL_TIM_caputer_Init(void);

#endif
