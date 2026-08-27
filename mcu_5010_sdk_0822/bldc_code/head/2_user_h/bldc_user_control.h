/*******************************************************************************
* Copyright (C) 2019 China Micro Semiconductor Limited Company. All Rights Reserved.
*
* This software is owned and published by:
* CMS LLC, No 2609-10, Taurus Plaza, TaoyuanRoad, NanshanDistrict, Shenzhen, China.
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with CMS
* components. This software is licensed by CMS to be adapted only
* for use in systems utilizing CMS components. CMS shall not be
* responsible for misuse or illegal use of this software for devices not
* supported herein. CMS is providing this software "AS IS" and will
* not be responsible for issues arising from incorrect user implementation
* of the software.
*
* This software may be replicated in part or whole for the licensed use,
* with the restriction that this Disclaimer and Copyright notice must be
* included with each copy of this software, whether used in part or whole,
* at all times.
*/
/*****************************************************************************/
/** \file main.h
**
** 
**
** History:
** 
*****************************************************************************/
#ifndef __USER_CONTROL_H__
#define __USER_CONTROL_H__
/*****************************************************************************/
/* Include files */
/*****************************************************************************/
//#include "cms32m5xxx.h"
#include "bldc_public_function.h"


/*tool state*/
typedef enum 
{
    TOOL_STOP = 0,	
    TOOL_RUN = 1,
    TOOL_ERROR_STOP = 2,	
    TOOL_POWER_DOWN = 3,
    TOOL_STOPPING = 4,
    TOOL_PRESTOP = 5,
    TOOL_READY = 6,
}UC_StateMachine_e;


typedef union
{
    uint32_t word;
    struct
    {
        uint32_t    gToolEn:1;
        uint32_t    gToolDir:1;
		uint32_t    gToolTrigger:1;
		uint32_t    gAutoStopEn;
		
		uint32_t    gAutoStop;
		uint32_t    gLLOLock:1;/*防飞车锁定标志*/
		uint32_t    gVRLock:1;/*红外锁定标志*/
    }bits;
}UC_Flag_u;



typedef struct 
{
    UC_StateMachine_e	ToolState;

	UC_Flag_u	flag;
		
    uint8_t		ToolGear;     /*工具档位*/
    uint8_t		MotorState;

    int32_t		TriggerMin_Percent;
    int32_t		TriggerMax_Percent;
    int32_t		TriggerMin_Q15;
    int32_t		TriggerMax_Q15;
    int32_t		RateDivTrigger_Q15;
    int32_t		RateMax_Q15;
    int32_t		RateMin_Q15;
    int32_t		RateSet_Q15;
    
    int32_t		AimSet_Q15;

    uint32_t	ToolState_Cnt;   
    uint32_t	SleepDelay_CntSet;


    uint8_t		WrenchAutoStop;
    uint16_t	WrenchAutoStopMs;
    uint16_t	WrenchAutoStopSectorCnt;
}User_Control_t;

typedef struct 
{
    UC_Error_u	userErr;
    
    uint16_t	IBusPeakOffset_Adc;
    uint16_t 	IBusAvgOffset_Adc;
    uint16_t 	IBusOffsetRef_Adc;

    uint16_t 	IBusAvgCh_Adc;
    uint16_t  	IBusAvg_Adc;  // chAdc - offsetAdc
    int32_t   	IBusAvg_Ax100;
    int32_t  	IBusAvg_Ax100Filter;
    uint8_t 	IBusAvgLimEn_Flag;
    uint32_t  	IBusAvgLim_Ax100;
    uint16_t  	IBusAvgLim_Adc;

    
    uint16_t 	IBusPeakCh_Adc;
    uint16_t 	IBusPeak_Adc;// chAdc - offsetAdc
    int32_t   	IBusPeak_Ax100;
    uint8_t 	IBusPeakLimEn_Flag;
    uint32_t 	IBusPeakLim_Ax100;
    uint16_t 	IBusPeakLim_Adc;
	int8_t 		IBusLimit_Flag;

   
    // i current
    uint8_t		gIBusAvgProtectEn;
    uint16_t   	IBusAvgOCP1_Cnt; /*计数使用局部变量*/
    uint16_t  	IBusAvgOCP1_CntSet;
    uint16_t    IBusAvgOCP1_Adc;
    uint16_t    IBusAvgOCP1_Ax100;

    uint16_t    IBusAvgOCP2_Cnt;
    uint16_t    IBusAvgOCP2_CntSet;   
    uint16_t    IBusAvgOCP2_Adc;
    uint16_t    IBusAvgOCP2_Ax100;

    uint16_t    IBusAvgOCP3_Cnt;
    uint16_t    IBusAvgOCP3_CntSet;
    uint16_t    IBusAvgOCP3_Adc;
    uint16_t    IBusAvgOCP3_Ax100;

    uint8_t     gIBusPeakProtectEn;

    uint16_t    IBusPeakOCP1_Cnt;
    uint16_t    IBusPeakOCP1_CntSet;
    uint16_t    IBusPeakOCP1_Adc;
    uint16_t    IBusPeakOCP1_Ax100;

	uint16_t    IBusMosCheck_Ax100;
    // U volt
    uint8_t     gVBusProtectEn;
	uint32_t 	VBus_Vx100;
	uint32_t  	VBus_Vx100Filter;
	uint16_t  	VBus_Adc;
	uint16_t  	VBus_AdcFilter;  // vBus filter
	
    uint16_t    VBusRunUVP1_Cnt;
    uint16_t    VBusRunUVP1_CntSet;
    uint16_t    VBusRunUVP1_Adc;
    uint16_t    VBusRunUVP1_Vx100;

    uint16_t    VBusRunUVP2_Cnt;
    uint16_t    VBusRunUVP2_CntSet;
    uint16_t    VBusRunUVP2_Adc;
    uint16_t    VBusRunUVP2_Vx100;

    uint16_t    VBusStbUVP_Cnt;
    uint16_t    VBusStbUVP_CntSet;
    uint16_t    VBusStbUVP_Adc;
    uint16_t    VBusStbUVP_Vx100;

    uint16_t    VBusOVP_Cnt;
    uint16_t    VBusOVP_CntSet;
    uint16_t    VBusOVP_Adc;
    uint16_t    VBusOVP_Vx100;

	uint16_t  	VPhaseMaxTemp_Adc;
    uint16_t  	VPhaseMax_Adc;
	uint16_t  	PhaseA_Adc;
	uint16_t  	PhaseB_Adc;
	uint16_t 	PhaseC_Adc;

    // NTC 

    uint8_t     gNtcMosProtectEn;
    uint8_t     gNtcBatProtectEn;
  	uint16_t 	NtcMos_Adc;
	uint16_t  	NtcBat_Adc;
	
    uint16_t    NtcMosUTP_Adc; 
    uint16_t    NtcMosUTPClr_Adc;
    uint32_t    NtcMosUTP_Res; 
    uint32_t    NtcMosUTPClr_Res;
    
    uint16_t    NtcMosErr_Cnt;
    uint16_t    NtcMosErr_CntSet;
    
    uint16_t    NtcMosOTP_Adc;      
    uint16_t    NtcMosOTPClr_Adc;
    uint32_t    NtcMosOTP_Res;      
    uint32_t    NtcMosOTPClr_Res;
    uint32_t    NtcMosRef_Res;
        
    uint16_t    NtcMosErrClr_Cnt;
    uint16_t    NtcMosErrClr_CntSet;

    uint16_t    NtcBatUTP_Adc;
    uint16_t    NtcBatUTPClr_Adc;
    uint32_t    NtcBatUTP_Res;
    uint32_t    NtcBatUTPClr_Res;
    
    uint16_t    NtcBatErr_Cnt;
    uint16_t    NtcBatErr_CntSet;
    uint16_t    NtcBatOTP_Adc;
    uint16_t    NtcBatOTPClr_Adc;  
    uint32_t    NtcBatOTP_Res;
    uint32_t    NtcBatOTPClr_Res;  
    uint32_t    NtcBatRef_Res;  
        
    uint16_t    NtcBatErrClr_Cnt;
    uint16_t    NtcBatErrClr_CntSet;

	uint16_t  	Trigger_Adc;
    int32_t  	Trigger_Q15;

    uint16_t  	TriggerOn_Percent;
    uint16_t 	TriggerOff_Percent;
    int32_t   	TriggerOn_Q15;
    int32_t   	TriggerOff_Q15;


    uint8_t  	SwDir_Flag;
    uint8_t  	SwDirCWValue;
    uint8_t  	SwDirCCWValue;
    
    uint8_t   	SwWrench_Flag;
}User_Input_t;


/*output */
typedef struct 
{
    uint8_t     LedDeal;
	uint8_t		LedRunMode;/**/
    uint16_t	LedRunTimes;	/*led循环次数*/
    uint16_t	LedRunCnt_Once;/*led一次几闪*/



    uint8_t     gSocUpdate;
    uint8_t     gSocDislay;
}User_Output_t;




extern User_Input_t		strInput;
extern User_Control_t 	strUCtr;
extern User_Output_t  	strOutput;




void UC_Variable_Init(void);
void UC_GetIBusOffset(void);

void UC_ErrData_Update(void);
void UC_StateMachine_Handler(void);
uint32_t  UC_GetRateFromTrigger(int32_t itriggerAdcNow_Q15);
void UC_MC_Update( void);


#endif /*  */
