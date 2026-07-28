/*****************************************************************************/
/** \file mc_pic.h
**
** 
**
** History:
** 
*****************************************************************************/

#ifndef __MC_PIC_H__
#define __MC_PIC_H__

/*****************************************************************************/
/* Include files */
/*****************************************************************************/
//#include  "base_define.h"

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define') */
/*****************************************************************************/


/*****************************************************************************/
/* Global type definitions ('typedef') */
/*****************************************************************************/
typedef struct 
{ 	
  s32 Kp;
  s32 Ki;
  s32 Lower_Output;     //Lower Limit for Output limitation
  s32 Upper_Output;     //Lower Limit for Output limitation
  s32 Lower_Integral;   //Lower Limit for Integral term limitation
  s32 Upper_Integral;   //Lower Limit for Integral term limitation
  s32 Integral;
  s32 Out;
  s32 KiSum;
  s32 Error;
} PID_Components;


typedef struct 
{ 	
  s32 Kp;
  s32 Ki;
	
  s32 Error;	
  s32 Integral;
  s32 Out;

  s32 Output_Min;     
  s32 Output_Max;     

	
} struct_PI;
/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source) */
/*****************************************************************************/
extern  PID_Components   PID_ID, PID_IQ, PID_WL, PID_FW;
extern  s32 PI_SpeedLoop_Kd;

/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source) */
/*****************************************************************************/
void 	FOC_PID_Parameter_Init(void);
void Function_PI_Cal(PID_Components *PI,s32 Ref, s32 Feedbk);
#endif	/* __MC_PIC_H__ */


