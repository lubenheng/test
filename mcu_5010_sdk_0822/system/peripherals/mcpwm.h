#ifndef MCPWM_H
#define MCPWM_H

#include "mcpwm_type.h"


//---------------------------------------------------------------------------//
/****************************<< EPWM相关参数设置 >>***************************/
#define  MCU_CLK                            (SYS_CORE_CLK)             // MCU CLK
#define  EPWM_FREQ                          (10000)                    // EPWM FREQ
#define  EPWM_PERIOD                        (MCU_CLK/EPWM_FREQ/2) //32000000/10000/2 = 1600 = 0x640
#define  EPWM_HALFPERIOD                    (EPWM_PERIOD/2)
#define  EPWM_Tus                           (32.0)                   // 32: 1us
#define  EPWM_CPMTG                         (EPWM_Tus * 0.5)                    // 双电阻方案：采样提前时间 单位：us
#define  EPWM_DT                            (EPWM_Tus * 0.5)                 // 死区时间：单位：us  

#define  	SAMPLE_HOLD                      	(EPWM_Tus*1.0)                // 单电阻方案：采样保持时间
#define  	SAMPLE_STEADY                     (EPWM_Tus*1.5)                // 单电阻方案：信号（震荡）稳定时间，0.5us
#define   EPWM_TGDLY                        (SAMPLE_STEADY + EPWM_DT) 		// 单电阻方案：采样延迟时间 单位：us 



#define PRE_DRIVER_POLARITY                 P_HIGH__N_HIGH      /* 预驱预动极性设置 上管高电平有效，下管低电平有效 */
#define EPWM0_CURRENT_SAMPLE_TYPE		    (CURRENT_SAMPLE_3SHUNT)		// 单电机平台电流采样方式配置

#define EPWM1_CURRENT_SAMPLE_TYPE		    (CURRENT_SAMPLE_3SHUNT)		// 单电机平台电流采样方式配置



void MCPWM_Init(MCPWM_InitTypeDef* MCPWM_InitStruct);
void MCPWM_StructInit(MCPWM_InitTypeDef* MCPWM_InitStruct);
void PWMOutputs(FuncState t_state);
void MCPWM_ClearIRQFlag(MCPWM0CNTx CNTx,uint32_t INT_flag);
u16 MCPWM_GetIRQFlag(MCPWM0CNTx CNTx,uint32_t INT_flag);
void MCPWM_Clk_Enable(u8 Clk_Enable);
void MCPWM_DIV_Set(u8 ClockDiv);
void MCPWM_CNTx_Enable(MCPWM0CNTx CNTx, FuncState state);
void MCPWM_CH012_Init(MCPWM_InitTypeDef* MCPWM_InitStruct);
void MCPWM_CH345_Init(MCPWM_InitTypeDef* MCPWM_InitStruct);
void MCPWM_CH012_IESet(MCPWM_InitTypeDef* MCPWM_InitStruct);
void MCPWM_CH345_IESet(MCPWM_InitTypeDef* MCPWM_InitStruct);
void MCPWM_ModuleClockCmd(FuncState state);


mcpwm0_int_source_e mcpwm0_drv_status_flag_get(void);
mcpwm1_int_source_e mcpwm1_drv_status_flag_get(void);
void MCPWM_SetTrigCnt(uint32_t TMRx,int16_t TrigCnt);
#endif

