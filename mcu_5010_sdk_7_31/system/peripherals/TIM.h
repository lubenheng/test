#ifndef TIMx_H
#define TIMx_H



void TIMER_Set_CLKDIV(TIMx_TypeDef* Timx,uint32_t DivCoff);
void Timer_Ch0_En_FallCap(TIMx_TypeDef* Timx);
void Timer_Ch0_En_RiseCap(TIMx_TypeDef* Timx);
void Timer_Ch0_Disable_FallCap(TIMx_TypeDef* Timx);
void Timer_Ch0_Disable_RiseCap(TIMx_TypeDef* Timx);
void TIMER_Set_IOPOL(TIMx_TypeDef* Timx,uint8_t Chx,uint32_t Pol);
void TIMER_Set_CH_Mode(TIMx_TypeDef* Timx,uint8_t Chx,uint32_t Mode);
void TIMER_Set_InputSrc(TIMx_TypeDef* Timx,uint8_t Chx,uint32_t Input);
void Timer_Ch1_En_FallCap(TIMx_TypeDef* Timx);
void Timer_Ch1_En_RiseCap(TIMx_TypeDef* Timx);
void Timer_Ch1_Disable_FallCap(TIMx_TypeDef* Timx);
void Timer_Ch1_Disable_RiseCap(TIMx_TypeDef* Timx);
void Timer_Set_ClkSrc(TIMx_TypeDef* Timx,uint8_t Cks);
void Timer_Set_RLMode(TIMx_TypeDef* Timx,uint8_t Rld);
void Timer_Set_GateEN(TIMx_TypeDef* Timx,uint8_t Gate);
void Timer_Set_EXT_ON(TIMx_TypeDef* Timx,uint8_t Exon);
void Timer_Set_CNTR_Dir(TIMx_TypeDef* Timx,uint8_t Dir);
void Timer_Set_Center_Mode(TIMx_TypeDef* Timx,uint8_t Mid);
void Timer_Single_Trig(TIMx_TypeDef* Timx);
void Timer_Set_Shadow(TIMx_TypeDef* Timx,uint8_t SD);
void Timer_Reg_Update(TIMx_TypeDef* Timx);
void Timer_Enable_Cap0_Clr(TIMx_TypeDef* Timx);
void Timer_Disable_Cap0_Clr(TIMx_TypeDef* Timx);
void Timer_Enable_Cap1_Clr(TIMx_TypeDef* Timx);
void Timer_Disable_Cap1_Clr(TIMx_TypeDef* Timx);
void Timer_Enable(TIMx_TypeDef* Timx);
void Timer_Disable(TIMx_TypeDef* Timx);
void Timer_Set_ThVal(TIMx_TypeDef* Timx,uint32_t Th);
void Timer_Set_Comp0_Val(TIMx_TypeDef* Timx,uint32_t Com0)	;
void Timer_Set_Comp1_Val(TIMx_TypeDef* Timx,uint32_t Com1);
void Timer_Set_EvtSrc(TIMx_TypeDef* Timx,uint32_t Evt);
void Timer_Set_FiltCoeff(TIMx_TypeDef* Timx,uint32_t Coeff);
void Timer_IRQ_Enable(TIMx_TypeDef* Timx,uint32_t Irq);
void Timer_IRQ_Disable(TIMx_TypeDef* Timx,uint32_t Irq);
uint8_t Timer_drv_IRQ_Status_Get(TIMx_TypeDef* Timx);
void Timer_drv_IRQ_Disable(TIMx_TypeDef* Timx);
void Timer_drv_Setting_Clean(TIMx_TypeDef* Timx);
void Timer_drv_Setting_Clean_all(TIMx_TypeDef* Timx);

#endif

















