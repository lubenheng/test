#ifndef __BLDC_MOTOR_STATE_H_
#define __BLDC_MOTOR_STATE_H_

//#include "cms32m5xxx.h"


void MC_UC_Update( void);

void MC_Machine(void) ;
void MC_ColdStart_Init(void);
void MC_ColdStart_Handler(void);
void MC_HotStart_Init(void);
void MC_Start_Position_By_Drag(void);
void MC_SquareWave_drag(void);
#endif

