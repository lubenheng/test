#ifndef ADC_H
#define ADC_H

//#include "stdint.h"

void ADC_ENABLE(u8 state);
void ADC_SetGrpTrigSource	(uint32_t SampGrp,uint32_t TriSrc);
void ADC_SetSampleGrpMode	(uint32_t state);
void ADC_SetGrpChCount	(uint32_t SampGrp,uint32_t ChannelNum);
void ADC_SWTrig	(void);
void ADC_SEQx_CFG(uint32_t Sqex,uint32_t Chx);
uint32_t ADC_GetResult(u8 Sqex);
void ADC_Set_AWChannel(uint32_t Sqex);
void ADC_Set_AW_HTH(uint32_t HTH);
void ADC_Set_AW_LTH(uint32_t LTH);
void ADC_SetCLK_STPT(uint32_t STPT_cnt);
void ADC_SetCLK_COVT(uint32_t COVT_cnt);
void ADC_SetCLK_INIT(uint32_t INIT_cnt);
void ADC_SetCLK_SAPT(uint32_t SAPT_cnt);
uint32_t ADC_Get_AllState(void);
void ADC_SetCLK_TrigDelay(uint32_t Delay_cnt);
void ADC_Enable_IrqSrc(uint32_t IrqSrc);
void ADC_Disable_IrqSrc(uint32_t IrqSrc);
uint32_t ADC_Get_IrqSrc(void);
#endif

