/****************************************************************************/
/**
* @file    mc_pic.c
* @author  7
* @version V2.0.0
* @date    01-03-2018
* @brief   PI Calculate
**
*****************************************************************************/

/****************************************************************************/
/*	include files
*****************************************************************************/
#include "motor_driver.h"
#include "user.h"

/****************************************************************************/
/*	Local pre-processor symbols/macros('#define')
*****************************************************************************/
#define   WEAK_EKP                 	(10000)
#define   WEAK_EKI                 	(300)
#define		WEAK_EOUT_MAX							(0)
#define		WEAK_EOUT_MIN							(-16384)

/****************************************************************************/
/*	Global variable definitions(declared in header file with 'extern')
*****************************************************************************/
s32 PI_SpeedLoop_Kd 	= 56300;

/****************************************************************************/
/*	Local type definitions('typedef')
*****************************************************************************/

/****************************************************************************/
/*	Local variable  definitions('static')
*****************************************************************************/

/****************************************************************************/
/*	Local function prototypes('static')
*****************************************************************************/

/****************************************************************************/
/*	Function implementation - global ('extern') and local('static')
*****************************************************************************/

/*****************************************************************************
 ** \brief	PI控制参数初始化
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void FOC_PID_Parameter_Init(void)
{


	PID_ID.Integral 			= 0;
	PID_ID.Out 						= 0;
	PID_ID.KiSum 					= 0;


	PID_IQ.Integral 			= 0;
	PID_IQ.Out 						= 0;
	PID_IQ.KiSum 					= 0;


}

/*****************************************************************************
* Function Name  : Function_PI_Cal
* Description    : PI控制器
* Function Call  : 
* Input Paragram : 
* Return Value   : none
* note           : 
* Version        : V0.1    2025/04/13    新建		
******************************************************************************/
void Function_PI_Cal(PID_Components *PI,s32 Ref, s32 Feedbk)
{
	PI->Error = Ref - Feedbk;
	PI->Integral += PI->Ki * PI->Error ;

	if(PI->Integral > (PI->Upper_Integral << 15) )		PI->Integral = PI->Upper_Integral<< 15  ;
	if(PI->Integral < (PI->Lower_Integral << 15) )		PI->Integral = PI->Lower_Integral<< 15 ;
	
	s32 outbuf;
	outbuf = ((PI->Kp >> 3) * PI->Error >> 12) + (PI->Integral >> 15) ;
	if(outbuf > PI->Upper_Output )		outbuf = PI->Upper_Output ;
	if(outbuf < PI->Lower_Output )		outbuf = PI->Lower_Output ;	
	PI->Out = outbuf;
}


/**************************** END OF FILE *******************************/
