//==========================================================================//
/*****************************************************************************
*-----------------------------------------------------------------------------
* @file    mc_adaption.h
* @author  CMS Motor Control Team
* @version V3.0.0
* @date    2021年3月
* @brief   
*---------------------------------------------------------------------------//
*****************************************************************************/
//==========================================================================//

#ifndef  __MC_FOC_CTR_H__
#define  __MC_FOC_CTR_H__

/*****************************************************************************/
/* Include files */
/*****************************************************************************/
#include <stdint.h>
#include "base_define.h"

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define') */
/*****************************************************************************/
//typedef struct
//{
//	//
//	u8					Status;				
//	u8					Flag;					// 0静止启动、1顺风
//	uint32_t		Time;					// 顺风观测时间
//	//---------------------------------------------------//
//	// 反电势信号
//	int32_t     AD_Value[3];   /* AD采样值 */
//	int32_t			Eu;            /* U相反电动势值 Q14*/
//	int32_t			Ev;            /* V相反电动势值 Q14*/
//	int32_t			Ew;            /* W相反电动势值 Q14*/
//	int32_t			ARR[3];	

//	//---------------------------------------------------//
//	// 反电势
//	int32_t			Ea;            /* U相反电动势值 */
//	int32_t			Eb;            /* V相反电动势值 */
//	int32_t			Ec;            /* W相反电动势值 */
//	int32_t			Ealpha;        // Q14
//	int32_t			Ebeta;      	// Q14
//	int32_t			Ed;          	//	Q14
//	int32_t			Eq;      	

//	int32_t     FluxWe;     		/* 根据磁链计算的We */
//	int32_t     FluxSpeed;     /* 根据磁链计算的转速 */
//	
//	//---------------------------------------------------//
//	// 锁相环
//	int32_t			error;
//	int32_t			Ki;
//	int32_t			Kp;
//	int32_t			Ui;
//	int32_t			Q29Sum;
//	int32_t 		Out;
//	int32_t			IntegUp;
//	int32_t			IntegDown;
//	int32_t			OutUp;
//	int32_t			OutDown;
//	
//	int32_t			PLLAngle;
//	int32_t			PLL_We;
//	
//	//---------------------------------------------------//
//	// 发波电压相关
//	int32_t			Ud;          	
//	int32_t			Uq;
//	int32_t			VbusComp;				/* 电压系数*/
//	
//	// 
//	int32_t			Speed;
//}struct_Bemf;

typedef struct
{
	int32_t     AD_Value[3];   /* AD采样值 */
	int32_t			ARR[3];		
	
	int32_t			Eu;            /* U相反电动势值 */
	int32_t			Ev;            /* V相反电动势值 */
	int32_t			Ew;            /* W相反电动势值 */
	//---------------------------------------------------//
	// 反电势
	int32_t			Ea;            /* U相反电动势值 */
	int32_t			Eb;            /* V相反电动势值 */
	int32_t			Ec;            /* W相反电动势值 */
	int32_t			Ealpha;        // Q14
	int32_t			Ebeta;      	// Q14
	int32_t			Ed;          	//	Q14
	int32_t			Eq;    
	
	//---------------------------------------------------//
	int32_t			DeltaTheta;
	int32_t			Theta;         /* 反电势计算出的角度值 */
	int32_t			Angle;         /* 反电势计算出的角度值 */
       

	
	int32_t     We;            /* 反电势计算出的Ω值 */
	int32_t     Speed;         /* 反电势计算出的转速值(RPM) */

	int32_t     Time;          /* 反电势检测时间 */
	uint8_t     Status;        /* 反电势状态 */
	uint8_t     Flag;     		 /* 0静止启动、1顺风 */
	uint8_t    	Zero_Flag;     /* 反电势过零次数 */	
	uint32_t    Zero_Time;     /* 反电势过零次数 */		
	uint32_t    Speed_Time;
	uint8_t    	Angle_Status;    	
  uint8_t    	CW_Count;
  uint8_t    	CCW_Count;
  uint8_t    	Angle_CW_OK;
  uint8_t    	Angle_CCW_OK;

         	
	int32_t			Ud;          
	int32_t			Uq;
	int32_t			VbusComp;				/* 电压系数*/
}struct_Bemf;


/*****************************************************************************/
/* Global type definitions ('typedef') */
/*****************************************************************************/

//---------------------------------------------------------------------------/

/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source) */
/*****************************************************************************/
extern struct_Bemf		stru_bemf ;

/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source) */
/*****************************************************************************/
void FOC_BEMF_Init(void);
void BEMF_ParaInit(void);
void FOC_BEMF_Detection(void);
void FOC_Control_Wind(void);
void FOC_Control_Switch(void);
void FOC_Control_Transition(void);
void FOC_Control_Run(void);
void FOC_Control_Stop(void);
void FOC_Control_Align(void);
void FOC_Sensorless_Start(void);
void FOC_Control_Fault(void);
void FOC_DQCurrent_PIRegulator(void);
void FOC_FieldCurrent_PIRegulator(void);
void FOC_Control_Weakening(void);
void GET_BANDGAP_VAL(void);
#endif	/* __MC_FOC_CTR_H__ */


