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
/** 
**
** 
**
** History:
** 
*****************************************************************************/
#ifndef __BLDC_MOTORCONTROL_H__
#define __BLDC_MOTORCONTROL_H__
/*****************************************************************************/
/* Include files */
/*****************************************************************************/
//#include "cms32m5xxx.h"
#include "bldc_public_function.h"

typedef enum 
{		
    MOTOR_STOP = 0,           //停止状态
    MOTOR_MOTION = 1,		      // 查询状态
    MOTOR_POSITION = 2,	      // 测电感
    MOTOR_RUN = 3, 		       	// 闭环
    MOTOR_SBRAKE = 4,         //软刹车
    MOTOR_HBRAKE = 5,         //硬刹车
    MOTOR_BRAKE2STOP = 6,     //刹车到停止过度
    MotorStart_Drag_OPENLOOP = 7,    
    MOTOR_READY = 8,    
    MOTOR_STOPWAIT = 9,    
    MOTOR_MOTION_FAIL = 10,		      // 查询状态
    MOTOR_FOC_OPEN,
    MOTOR_FOC_CLOSE,
}MC_StateMachine_e;

typedef enum 
{		
    SECTOR_STOP = 0,           //停止状态
    SECTOR_WAIT_CHANGE = 1,
    SECTOR_WAIT_END_MASK = 2,
    SECTOR_AFTER_MASK = 3,
    SECTOR_IN_MASK = 4,
    SECTOR_IN_MOTION = 5,
}MC_StateSector_e;







typedef struct 
{
    MC_StateMachine_e 	MotorState;
    MC_StateSector_e 	RotorState;
	MC_Error_u	motorErr;
	
	uint8_t 	RotorSector;/*rotor place*/  
	
    uint8_t 	gMotorEn;/*电机使能标志*/
    uint8_t 	gMotorDir;/*电机方向标志*/
    uint8_t 	gSectorCloseLoop;

/*trigger set*/
	int32_t 	AimSet_Q15;     
/*PWM PARA*/
    uint16_t 	LowFreData;
    uint16_t 	LowFrePeriod;
	uint16_t 	LowFrePeriod_Us;   /*低频周期us*/
    uint16_t 	LowFreDutyMin;      
    uint16_t 	LowFreDutyMax;
    uint32_t 	LowFreDutyRun; 
	uint16_t 	LowFreRiseFreDuty;/*升频的低频占空比*/
	uint16_t 	LowFreFallFreDuty;/*降频的低频占空比*/

    uint16_t 	HighFreData;
    uint16_t 	HighFrePeriod;
	uint16_t 	HighFrePeriod_Us;   /*高频周期us*/
    uint32_t 	HighFreDivideLowFre_Q15;  

    uint16_t 	LoadFreData;/*加载到寄存器的频率*/
    uint16_t 	LoadFrePeriod;/*加载到寄存器的周期*/
    uint16_t 	LoadFreDuty;/*加载到寄存器的占空比*/

    uint16_t 	SetFreData;/*频率调整设置*/
    uint16_t 	SetFreDataLast;/**/


	uint16_t 	DutyBufDeal_Fre;
	uint16_t 	DutyBufDeal_FiltCnt;
	
	int32_t 	DutyLimitMax_Q15;
	int32_t 	DutyLimitMin_Q15;
	int32_t 	DutyBufPercentx100;

	int32_t 	DutySet_Q15;
	int32_t 	DutyBuf_Q15;
	int32_t 	DutyRun_Q15;

	int32_t 	DutyBufAim_Q15;
	int32_t 	DutyBufStep_Q15;

	int32_t 	DutyRunStepAddOpen_Q15;
	int32_t 	DutyRunStepAddClosed_Q15;
	int32_t 	DutyRunStepReduce_Q15;
	int32_t		DutySetMax;
	int32_t		DutySetMin;
	int32_t 	RpmSet;  
    uint8_t 	gRpmUpdate;
    int32_t 	Rpm;	
    int32_t 	RpmLast;
    int32_t 	RpmTemp;  
    int32_t 	RpmFilter;	
    int32_t 	RpmSetMax;	
	int32_t 	RpmSetMin;	   
    uint8_t 	gRpmBlock;
    int32_t 	RpmBlockZtoZ_T0Cnt;
	int32_t 	RpmBlockCtoZ_T0Cnt;
    uint32_t 	RpmDivisor;
    
    
    uint32_t 	SectorCtoZ_T0Cnt;/*换相点到过零点时间*/
    uint32_t 	SectorCtoZLast_T0Cnt;/*换相点到过零点时间last*/
    uint32_t 	SectorZtoC_T0Cnt;/*过零点到换相点时间*/
    uint32_t 	SectorZtoZ_T0Cnt; /*过零点到过零点时间*/
    uint32_t 	SectorZtoZLast_T0Cnt; /*过零点到过零点时间last*/
    uint32_t 	SectorZtoZ2sAvg_T0Cnt;/*上一个扇区与当前扇区平均时间*/
    uint32_t 	SectorCtoC_T0Cnt; /*换相点到换相点时间*/
    uint32_t 	SectorCtoCLast_T0Cnt;/*换相点到换相点时间last*/
    uint32_t 	SectorCtoCBeforeLast_T0Cnt;/*换相点到换相点时间last*/
    uint32_t 	SectorCtoC2sAvg_T0Cnt;/*上一个扇区与当前扇区平均时间*/
	uint32_t	SectorCtoCTab[8];
    uint32_t 	Sector1Angle_T0Cnt;/*扇区时间除以64*/
    uint32_t 	SectorMax_T0Cnt;/*扇区时间最大限制*/
    uint32_t 	SectorMin_T0Cnt;/*扇区时间最小限制*/

    

    uint8_t 	MaskTime_AcmpIRQ_Angle;/*屏蔽时间的角度*/
	uint8_t 	MaskTime_Angle;/*屏蔽时间的角度*/
    uint32_t 	MaskTime_T0Cnt;/*屏蔽时间的计数*/
    uint32_t 	MaskTimeMax_T0Cnt;/*屏蔽时间的最大计数*/
    uint32_t 	MaskTimeMin_T0Cnt;/*屏蔽时间的最小计数*/
    uint8_t		MaskTimeOLDis;
    uint32_t 	MaskTimeOLoop_T0Cnt; /*开环阶段的屏蔽时间计数*/     
    
	uint8_t 	ShiftTime_AcmpIRQ_Angle;/*移相时间的角度*/
    uint8_t 	ShiftTime_Angle;/*移相时间的角度*/
    uint32_t 	ShiftTime_T0Cnt;   /*移相时间的计数*/
    uint32_t 	ShiftTimeMax_T0Cnt;/*移相时间的最大计数*/
    uint32_t 	ShiftTimeMin_T0Cnt;/*移相时间的最小计数*/
    uint32_t 	ShiftTimeOLoop_T0Cnt;/*开环阶段的移相时间计数*/

    uint8_t 	ShiftDis_Flag;/*移相禁止标志位*/
    uint8_t 	ShiftDis_Cnt;
    uint8_t 	ShiftDis_CntSet;
    uint16_t 	ShiftDisFactor_T0Cnt;
    uint16_t 	ShiftEnFactor_T0Cnt;

    uint8_t 	BlockShiftLimEn_Flag;/*堵转限制移相时间*/
    uint8_t 	BlockShiftLim_Flag;
    uint8_t 	BlockShiftLimClr_Cnt;
    uint8_t 	BlockShiftLimClr_CntSet;
        

    uint8_t 	gCmpMaskEn;
    uint8_t 	gCmpMaskEnd;
    uint8_t 	gCmpMaskStart;


    uint16_t 	SectorCloseLoop_Cnt;
    uint16_t 	SectorCloseLoop_CntSet;



	uint8_t 	SBrakeEn;
	uint16_t 	SBrakeTime_10KCnt;
    int32_t 	SBrakeDutyMin_Q15;
    int32_t 	SBrakeDutyMax_Q15;
    int32_t 	SBrakeDutyAdd_Q15;
    int32_t 	SBrakeDutyRun_Q15;
    int32_t 	SBrakeDutyWrite_Q15;
    
	uint8_t 	HBrakeEn;
	uint16_t 	HBrakeTIme_10KCnt;

	uint16_t 	StopWaitTime_10KCnt;

    // Motion 
    uint8_t 	MotionEn;
    uint16_t 	MotionMaskTime_T0Cnt ;
    uint16_t 	MotionShiftTime_T0Cnt ;
    uint32_t 	MotionNowSector_T0Cnt ;
    uint32_t 	MotionLastSector_T0Cnt ;
    uint32_t 	MotionLast2Sector_T0Cnt ;
    
    uint8_t 	MotionPassLoopRunSet ;/*滑行通过，闭环设置*/
    uint8_t 	MotionPass_Cnt;/*滑行通过验证计数*/
    uint8_t 	MotionPass_CntSet;/*滑行通过验证滤波计数*/
	uint8_t 	MotionPass_Flag;
	
    uint8_t 	MotionIRQRecord_Cnt;/*滑行中断触发累计*/
    uint16_t 	MotionWait_10KCnt;
    uint16_t 	MotionWaitMin_10KCnt;
    uint16_t 	MotionWaitAdd_10KCnt;
    
    uint8_t 	MotionBrake_Flag;
    uint16_t 	MotionBrakeMin_10KCnt;
    uint16_t 	MotionBrakeAdd_10KCnt;
    uint16_t 	MotionSectorMax_T0Cnt;/*滑行扇区最大时间限制，限制滑行最低速*/


//  u v w adc

    uint8_t 	CoexistCmp3Ad;/*高速是否比较器与ADSCAN共存*/

	uint8_t 	MaskOverClearEn;
    uint16_t 	AdcScanPOn_Adc;
    uint16_t 	AdcScanNOn_Adc;
    uint16_t 	AdcScanFOn_Adc; 
    uint16_t 	AdcScanPOff_Adc;
    uint16_t 	AdcScanNOff_Adc;
    uint16_t 	AdcScanFOff_Adc;
    int32_t 	AdcScanOLRiseRatio_Q15;
    int32_t 	AdcScanOLFallRatio_Q15;
    int32_t 	AdcScanCLRiseRatio_Q15;
    int32_t 	AdcScanCLFallRatio_Q15;
    int32_t 	AdcScanRiseRatio_Q15;
    int32_t 	AdcScanFallRatio_Q15;
    uint8_t 	AdcScanCrossZeroEnd;/*过零阶段结束*/
    
    uint8_t 	AcmpScanResult;
    uint8_t 	AcmpScanCrossZeroEnd;/*过零阶段结束*/

	uint8_t 	AcmpIRQEn_Flag;/*比较器中断使能*/
	uint8_t 	AcmpIRQEn_Cnt;  
	uint8_t 	AcmpIRQEn_CntSet;
    uint8_t 	AcmpIRQResult;
    uint8_t 	AcmpIRQCrossZero_Flag;
    uint8_t 	AcmpIRQCrossZeroEnd;/*过零阶段结束*/

    
    uint32_t 	AcmpIRQEnFactor_T0Cnt;
    uint32_t 	AcmpIRQDisFactor_T0Cnt;
    
    //strMCtr.AcmpIRQEnFactor_T0Cnt = strMCtr.LowFrePeriod_Us*strMCtr.AcmpIRQEn_PeriodCnt;
    uint8_t 	AcmpIRQEn_PeriodCnt;   // N个PWM周期的时间换算为定时器计数值
    uint8_t 	AcmpIRQDis_PeriodCnt;   // N个PWM周期的时间换算为定时器计数值
    uint32_t 	AcmpIRQEnLowFreData_T0Cnt;
    uint32_t 	AcmpIRQEnHighFreData_T0Cnt;
    uint32_t 	AcmpIRQDisLowFreData_T0Cnt;
    uint32_t 	AcmpIRQDisHighFreData_T0Cnt;

    uint8_t 	AntiphaseEn_Flag;/*反相使能标志*/
    uint8_t 	AntiphaseRunState;
    uint8_t 	AntiphaseRunOnce;
    uint8_t 	AntiphaseScanResult;
    int32_t 	AntiphaseEnDuty_Q15;
    int32_t 	AntiphaseDisDuty_Q15;



	
    uint16_t 	WrenchAutoStopSectorCnt;/*记录电机运行扇区数*/
    uint16_t 	MotorState_Cnt;

    uint8_t 	ColdStartMode;  /*冷启动方式*/
    uint8_t 	DriveMode;/*调速模式*/
    
/*current*/
    uint8_t		gIBusAvgLimEn;
	uint16_t 	IBusAvgCh_Adc;
    uint16_t   	IBusAvg_Adc;  // chAdc - offsetAdc
    uint32_t  	IBusAvg_Ax100;
    uint32_t	IBusAvgLim_Ax100;
    uint16_t	IBusAvgLim_Adc;	

	uint8_t		gIBusPeakLimEn;
	uint16_t   	IBusPeakCh_Adc;
    uint16_t   	IBusPeak_Adc;// chAdc - offsetAdc
    int32_t  	IBusPeak_Ax100;
    uint32_t	IBusPeakLim_Ax100;
    uint16_t	IBusPeakLim_Adc;
    
	uint8_t		gIBusLimit;
    
    uint16_t 	IBusPeakOffset_Adc;
	uint16_t 	IBusAvgOffset_Adc;
	uint16_t    IBusOffsetRef_Adc;

/*volt limit*/
	uint16_t	PhaseA_Adc;
	uint16_t 	PhaseB_Adc;
	uint16_t   	PhaseC_Adc;
	uint16_t	VPhaseMaxTemp_Adc;
    uint16_t 	VPhaseMax_Adc;

    uint32_t  	OpenloopDutyLim;

	uint8_t		HighVoltMode;
	uint16_t 	HighVoltSetAdc;
    uint32_t 	HighVoltSetV_x100;  //mv 200 000
    uint16_t 	HighVoltNowAdc;
    uint32_t    HVDividend;
    uint32_t    HVDivisor; 
}Motor_Control_t;

typedef struct 
{
	int16_t 	CheckTime;
	int16_t 	IdleTime;
	int16_t 	ChargeTime;

	uint8_t 	GetPlaceMode;
	int16_t 	CheckData[8];
	int16_t 	CheckData2P[8];
	int16_t 	CheckData3P[8];
	int16_t 	CheckDataLast[8];
	uint8_t 	CheckFstSector;
	uint8_t 	CheckSndSector;
	uint16_t 	CheckFstSectorData;
	uint16_t 	CheckSndSectorData;
		
	uint8_t 	Phase3FstSector;
	uint16_t 	Phase3FstSectorData;
	uint8_t 	Phase3SndSector;
	uint16_t 	Phase3SndSectorData;
	uint16_t 	Phase3SndDeltaFstData;

	uint8_t 	Phase2FstSector;
	uint16_t 	Phase2FstSectorData;
	uint8_t 	Phase2SndSector;
	uint16_t 	Phase2SndSectorData;
	uint16_t 	Phase2SndDeltaFstData;

	uint8_t 	DrivePhase;
	uint8_t 	DrivePhase2Mode;
	uint8_t 	DrivePhase3Mode;
	uint8_t 	DriveRePhaseEn;

	uint8_t 	ModeSelect;

	uint8_t 	Phase2SectorMove;  // 两相IPD初始位置调整参数+5为-1
	uint16_t 	SndDeltaFstMinAdc;

	uint32_t 	IBusPeakScanCh;
	uint32_t 	IBusPeakDataCh;


	uint8_t 	gEnd;
	uint8_t		gResult;
}
Motor_IPD_t;

typedef struct 
{
	Motor_IPD_t IPD;

	
    uint8_t 	dragVFEn;
    
}Motor_Start_t;


typedef struct PidPI
{
    int32_t 	kp; 
    int32_t 	ki; 
    int32_t 	error; 
    int32_t 	error1; 
    int32_t 	error2;
    int32_t 	refValue; 
    int32_t 	output; 
    int32_t 	proportion; 
    int64_t 	errorIntegral;   
    int32_t 	lowerLimitOutput;   //Lower Limit for Output limitation
    int32_t 	upperLimitOutput;   //Lower Limit for Output limitation
    int32_t 	maxIntegralRatio;   
    int32_t 	lowerLimitIntegral; //Lower Limit for Integral term limitation
    int32_t 	upperLimitIntegral; //Lower Limit for Integral term limitation


    int32_t 	pTemp;
    int32_t 	iTemp;

    
    int32_t 	deltaUk; //deltaUk = Uk - Uk_1;  Uk为位置式PI输出

    int32_t 	deltaOut;
    int32_t 	rpmSetPwmDuty;

    int32_t 	KpDuty;
    int32_t 	KiDuty;
    int32_t 	OutDuty;
} PidPI_t;



extern Motor_Control_t strMCtr;
extern Motor_Start_t strMStart;


extern PidPI_t strPid;
extern uint32_t testMaskTime;
extern uint32_t testForceTime;
extern uint32_t testForceTimeMax;
extern uint16_t openLoopRunCnt;
extern uint16_t openLoopRunLoseStep;
extern uint16_t openLoopDutyMax;
extern uint16_t openLoopRunCntLim;
void MC_Rpm_Init(void);

void MC_CtrVariable_Init(void);
void MC_StartVariable_Init(void);
void MC_GetIBusOffset(void);


void MC_Get_Rpm(void);

uint8_t MC_DectectPosition_Start(void);
void MC_DutyBuf_Set(void);
void MC_CheckBlock(void);
void MC_3PhaseAdcRead(uint8_t adIndex);

void MC_PwmDutySet(void);  


void MC_MaskStateEnd_Deal(void);


uint16_t MC_MaskOutput(uint16_t ucDriver);
void MC_PwmMask_Set(uint16_t ucDriver);
uint8_t MC_PWM_BrakeRead(void);
void MC_LoseStep_Deal(void);
void MC_OpenLoop_MaskDeal(void);
void MC_ForceChangePhase(void);

int32_t filter_1_x(int32_t NEW_DATA,int32_t OLD_DATA);
int32_t Div_Handler(int32_t dividend,int32_t divisor);

uint16_t MCPWM_TIMER0_MaskOutput(uint16_t ucDriver);
uint16_t MCPWM_TIMER1_MaskOutput(uint16_t ucDriver);
#endif /*  */
