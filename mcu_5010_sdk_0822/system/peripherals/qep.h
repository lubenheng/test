
#include "qep_type.h"
#include "qep_reg.h"   /* TIM_QEPTypeDef (寄存器结构体) */

/*QEP初始化*/
void TIM_QEPInit(TIM_QEPTypeDef *QEPx, TIM_QEPInitTypeDef *TIM_QEPInitStruct);
void TIM_QEPStructInit(TIM_QEPInitTypeDef *TIM_QEPInitStruct);
void TIM_QEPCmd(TIM_QEPTypeDef *QEPx, FuncState state);
uint16_t TIM_QEP_GetCount(TIM_QEPTypeDef *QEPx);
void QEP_ClearIRQFlag(TIM_QEPTypeDef *QEPx, uint32_t tempFlag);
u16 QEP_GetIRQFlag(TIM_QEPTypeDef *QEPx, u32 QEP_IF_x);
u16 QEP_GetFlag(TIM_QEPTypeDef *QEPx, u32 QEP_IF_x);
