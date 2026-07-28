#ifndef MCPWM_HAL_H
#define MCPWM_HAL_H


#include "mcpwm_type.h"


extern MCPWM0_INT_CALLBACK g_mcpwm0_int_callback;
extern MCPWM1_INT_CALLBACK g_mcpwm1_int_callback;


/*****************MCPWM0*****************/

void MCPWM_Software_Reset(void);
void MCPWM_ch012_init(void);

void ClrEPWM0_breakInStatus(void);
void registerEPWM0_DrvLevel(void);
void EPWM0_OutPut(FuncState t_state);
void EPWM0_Charge(u8 t_bPhaseABC);
void InitEPWM0ChargeEnd(void);
s16 getEpwm0DefaultSampoint0(void);
s16 getEpwm0DefaultSampoint1(void);
void MCPWM0_RegUpdate(STR_PubDatMDHD* tS_pMdToHd);


/*****************MCPWM1*****************/

void MCPWM_ch345_init(void);

void ClrEPWM1_breakInStatus(void);
void registerEPWM1_DrvLevel(void);
void EPWM0_OutPut(FuncState t_state);
void EPWM1_Charge(u8 t_bPhaseABC);
void InitEPWM1ChargeEnd(void);
s16 getEpwm1DefaultSampoint0(void);
s16 getEpwm1DefaultSampoint1(void);
void MCPWM1_RegUpdate(STR_PubDatMDHD* tS_pMdToHd);


void MCPWM0_Call_Back(void);
void MCPWM_CH_DEFAULT(uint32_t mcpwm_timer,bool enable,uint32_t value);



void mcpwm_channel_n_p_inverse_set(uint32_t channel,uint32_t channel_n_p,uint32_t inverse);
void mcpwm_channel_swap_set(uint32_t channel,uint32_t channel_swap);
#endif
