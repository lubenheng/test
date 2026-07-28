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

 
/****************************************************************************/
/** \file motor_control.c
**
**	History:
**	
*****************************************************************************/
/****************************************************************************/
/*	include files
*****************************************************************************/

//#include "user_control.h"


#include "bldc_motor_config.h"
#include "bldc_motor_state.h"
#include "bldc_motor_control.h"

//#include "motor_hall.h"
#include "bldc_public_function.h"
#include "bldc_motor_mlib.h"

//#include "user_config.h"
#include "bldc_public_debug.h"
#include "bldc_user_bsp.h"


Motor_Control_t strMCtr;
Motor_Start_t strMStart;


PidPI_t strPid;

int32_t K_x = (655); //滤波系数 == 32768/50r/m,小于50转误差时，输出不变化
u8 new_flag_x = 0;//本次数据变化方向
u8 num_x = 0;//滤波计数器

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments	 : 
* Return Value : 
***********************************************************************************************************************/

void MC_CtrVariable_Init(void)
{

#if 1
    /*失步保护参数设置**********************************************/
    strMErr.LoseStepProtectEn = LosStep_Protect_En;
    strMErr.LoseStepProtect_CntSet = LosStep_Protect_CntSet;
    strMErr.LoseStepProtect_Cnt = 0;
    strMErr.LoseStepProtect_Flag = 0;
    strMErr.LoseStepProtect_MoveBit = 1;

#endif


#if 1
    /*缺相保护参数设置**********************************************/

    strMErr.LackPhaseProtectEn = LackPhase_Protect_En;
    strMErr.LackPhaseProtect_Delta = LackPhase_Protect_Delta;

#endif


#if 1
    /*堵转保护参数设置**********************************************/

    strMErr.BlockProtectEn = Block_Protect_En;
    strMErr.BlockProtectErr_Cnt = 0;
    
    strMErr.BlockProtect_Cnt = 0;
    strMErr.BlockProtectDutyH_CntSet = Block_Protect_HighDuty_FiltMS*((float)strHW.Fre_MSM/strHW.SToMS);
    strMErr.BlockProtectDutyL_CntSet = Block_Protect_LowDuty_FiltMS*((float)strHW.Fre_MSM/strHW.SToMS);
    strMErr.BlockProtectDuty_Q15 = (32768*Block_Protect_LowDuty);

	strMErr.ForceChangePhaseEn = ForceChangePhase_En;
    strMErr.ForceChangePhase_T0Cnt = ForceChangePhase_CntSet;
#endif


#if 0  //jack
/****************************************************************************
*IPD 参数初始化
*
*
*****************************************************************************/
	strMStart.IPD.CheckTime = MotorStart_IPD_PulseTime; // us
    strMStart.IPD.ChargeTime= MotorStart_IPD_ChargeTime; // us
    strMStart.IPD.IdleTime = MotorStart_IPD_IdleTime; // us
    
    
    strMStart.IPD.GetPlaceMode = MotorStart_IPD_Drive_Set;

	strMStart.IPD.CheckFstSector = 10;
	strMStart.IPD.CheckFstSectorData = 0;
	strMStart.IPD.CheckSndSector = 10;
	strMStart.IPD.CheckSndSectorData = 0;

	strMStart.IPD.Phase3FstSector = 10;
	strMStart.IPD.Phase3FstSectorData = 0;
    strMStart.IPD.Phase3SndSector = 10;;
    strMStart.IPD.Phase3SndSectorData = 0;
    strMStart.IPD.Phase3SndDeltaFstData  = 0;

    strMStart.IPD.Phase2FstSector = 10;
    strMStart.IPD.Phase2FstSectorData = 0;
    strMStart.IPD.Phase2SndSector = 10;
    strMStart.IPD.Phase2SndSectorData = 0;
    strMStart.IPD.Phase2SndDeltaFstData = 0;

	strMStart.IPD.DrivePhase = MotorStart_IPD_Drive_Set;
	strMStart.IPD.DrivePhase2Mode = MotorStart_IPD_Drive_Phase2;
	strMStart.IPD.DrivePhase3Mode = MotorStart_IPD_Drive_Phase3;
	strMStart.IPD.DriveRePhaseEn = MotorStart_IPD_PulseRephaseEn;

        
    strMStart.IPD.IBusPeakScanCh = ADC1_IBusPeak_Convert;
    strMStart.IPD.IBusPeakDataCh = ADC1_IBusPeak_Result;
    strMStart.IPD.Phase2SectorMove = 5;
    strMStart.IPD.SndDeltaFstMinAdc = 15;
    
#endif


	strMCtr.MotorState = MOTOR_STOP;
	strMCtr.RotorState = SECTOR_STOP;
	strMCtr.RotorSector =  1;
	strMCtr.gMotorEn = 0;
	strMCtr.gMotorDir = 0;

	strMCtr.AimSet_Q15 = 0;
	
	strMCtr.LowFreData = PWM_Fre_Base;
	strMCtr.HighFreData = PWM_Fre_High;

	strMCtr.LowFrePeriod = (uint16_t)(strHW.Fre_EpwmCK/strMCtr.LowFreData)>>1;
	strMCtr.LowFrePeriod_Us= 1000000/strMCtr.LowFreData;
	strMCtr.LowFreDutyMin = PWM_Duty_MinLimit;
	strMCtr.LowFreDutyMax = strMCtr.LowFrePeriod ;
	strMCtr.LowFreDutyRun = 0;

	strMCtr.HighFreDivideLowFre_Q15 = (uint32_t)(strMCtr.LowFreData*32768/strMCtr.HighFreData);
	//strMCtr.HighFrePeriod = (u16)((((float)1000UL*Fre_MCU)/strMCtr.HighFreData))>>1;
	strMCtr.HighFrePeriod = (uint16_t)(strMCtr.LowFrePeriod*strMCtr.HighFreDivideLowFre_Q15>>15);
	strMCtr.HighFrePeriod_Us = 1000000/strMCtr.HighFreData;

	strMCtr.LoadFreData = strMCtr.LowFreData;
	strMCtr.LoadFrePeriod = strMCtr.LowFrePeriod ;
	strMCtr.LoadFreDuty = strMCtr.LowFreDutyMin;

	strMCtr.SetFreData = strMCtr.LowFreData;
	strMCtr.SetFreDataLast = 0;

	strMCtr.LowFreRiseFreDuty = (uint32_t)((float)strMCtr.LowFreDutyMin*strMCtr.HighFreData/strMCtr.LowFreData)*2.5;
	strMCtr.LowFreFallFreDuty = (uint32_t)((float)strMCtr.LowFreDutyMin*strMCtr.HighFreData/strMCtr.LowFreData)*2.2;


	strMCtr.DutySet_Q15= 0;
	strMCtr.DutyBuf_Q15 = 0;
	strMCtr.DutyRun_Q15 = 0;
	strMCtr.DutyBufPercentx100 = 0;
	strMCtr.DutyLimitMax_Q15 = DutyLimit_Max_Q15;
	strMCtr.DutyLimitMin_Q15 = DutyLimit_Min_Q15;


	strMCtr.DutyBufDeal_Fre = DutyBuf_Deal_Fre;
	strMCtr.DutyBufDeal_FiltCnt = (uint16_t)((float)strHW.Fre_MSM/strMCtr.DutyBufDeal_Fre);

	strMCtr.DutyBufStep_Q15 = DutyBuf_Step_Q15;
	strMCtr.DutyBufAim_Q15 = 0;

	strMCtr.DutyRunStepAddOpen_Q15 = DutyRun_StepAdd_Open_Q15;
	strMCtr.DutyRunStepAddClosed_Q15 = DutyRun_StepAdd_Closed_Q15;
	strMCtr.DutyRunStepReduce_Q15 = DutyRun_StepReduce_Q15;


	// rpm
	strMCtr.RpmSet = 0;
	strMCtr.gRpmUpdate = 0;
	strMCtr.Rpm =0;
	strMCtr.RpmLast = 0;
	strMCtr.RpmTemp = 0;
	strMCtr.RpmFilter = 0;

	strMCtr.RpmSetMax = LoopSpeed_AimSet_Max;
	strMCtr.RpmSetMin = LoopSpeed_AimSet_Min;
	
	strMCtr.DutySetMax = LoopNone_Duty_Max;
	strMCtr.DutySetMin = LoopNone_Duty_Min;

	strMCtr.gRpmBlock = 0;
	strMCtr.RpmBlockZtoZ_T0Cnt = 0;
	strMCtr.RpmBlockCtoZ_T0Cnt = 0;
	
	/*60s000ms000us/极对数/扇区数*/
	strMCtr.RpmDivisor = ((float)60000000*Fre_Timer/1000000/( strHW.MotorPolePair)) ;


	strMCtr.AcmpIRQEn_Flag = 0;//ModeCmpIntEn


	strMCtr.SectorCtoZ_T0Cnt = 0;
	strMCtr.SectorCtoZLast_T0Cnt = 0;
	strMCtr.SectorZtoC_T0Cnt = 0;

	strMCtr.SectorZtoZ_T0Cnt = 0; //上次过零到 当前扇区时间
	strMCtr.SectorZtoZLast_T0Cnt = 0; // 上上次过零到上次过零扇区时间
	strMCtr.SectorCtoC_T0Cnt = 0; // 上次换相到当前换相扇区时间
	strMCtr.SectorCtoCLast_T0Cnt = 0;// 上上次换相到上次换相扇区时间

	strMCtr.SectorZtoZ2sAvg_T0Cnt = 0;
	strMCtr.SectorCtoC2sAvg_T0Cnt = 0;
	strMCtr.Sector1Angle_T0Cnt = 0;  // 1 degree
	for(uint8_t sectori = 0;sectori<8;sectori++)
	{
		strMCtr.SectorCtoCTab[sectori] = 0;
	}  


	strMCtr.MaskTime_AcmpIRQ_Angle = MaskTime_HighSpeed_Angle;
	strMCtr.ShiftTime_AcmpIRQ_Angle = ShiftTime_HighSpeed_Angle; 
	strMCtr.MaskTime_Angle = MaskTime_LowSpeed_Angle;               // 0~30;
	strMCtr.ShiftTime_Angle = ShiftTime_LowSpeed_Angle;              // 0~30;
	strMCtr.MaskTime_T0Cnt = 0;
	strMCtr.MaskTimeMax_T0Cnt = MaskTime_Max_US*((float)strHW.Fre_SectorTimer/strHW.SToUS);
	strMCtr.MaskTimeMin_T0Cnt= MaskTime_Min_US*((float)strHW.Fre_SectorTimer/strHW.SToUS);
	strMCtr.ShiftTime_T0Cnt = 0;
	strMCtr.ShiftTimeMax_T0Cnt = ShiftTime_Max_US*((float)strHW.Fre_SectorTimer/strHW.SToUS);
	strMCtr.ShiftTimeMin_T0Cnt= ShiftTime_Min_US*((float)strHW.Fre_SectorTimer/strHW.SToUS);
	strMCtr.SectorMax_T0Cnt = SectorTIme_Max_US*((float)strHW.Fre_SectorTimer/strHW.SToUS);   // sectortime max
	strMCtr.SectorMin_T0Cnt = SectorTIme_Min_US*((float)strHW.Fre_SectorTimer/strHW.SToUS);
	strMCtr.MaskTimeOLDis = MaskTime_OpenLoop_Dis;/*置1后，开环屏蔽时间由角度计算*/
	strMCtr.MaskTimeOLoop_T0Cnt = MaskTime_OpenLoop_US*((float)strHW.Fre_SectorTimer/strHW.SToUS);
	strMCtr.ShiftTimeOLoop_T0Cnt = ShiftTime_OpenLoop_US*((float)strHW.Fre_SectorTimer/strHW.SToUS);

	strMCtr.ShiftDis_Flag = 0;
	strMCtr.ShiftDis_Cnt = 0;
	strMCtr.ShiftDis_CntSet = ShiftTime_Dis_CntSet;
	strMCtr.ShiftDisFactor_T0Cnt = ShiftTime_Dis_SectorTime_US*((float)strHW.Fre_SectorTimer/strHW.SToUS);
	strMCtr.ShiftEnFactor_T0Cnt = ShiftTime_En_SectorTime_US*((float)strHW.Fre_SectorTimer/strHW.SToUS);

	strMCtr.BlockShiftLimEn_Flag = Block_ShiftLim_En;
	strMCtr.BlockShiftLim_Flag = 0;
	strMCtr.BlockShiftLimClr_Cnt = 0;
	strMCtr.BlockShiftLimClr_CntSet= Block_ShiftLim_Clr_CntSet;


	/* mask set*/
	strMCtr.gCmpMaskEn = 0;
	strMCtr.gCmpMaskEnd = 0;
	strMCtr.gCmpMaskStart = 0;

	/*close loop filter*/
	strMCtr.gSectorCloseLoop = 0;
	strMCtr.SectorCloseLoop_Cnt = 0;
	strMCtr.SectorCloseLoop_CntSet = Sector_CloseLoop_CntSet; // open count

	/*Brake*/
	strMCtr.SBrakeEn = SBrake_En;  /*软刹车*/
	strMCtr.HBrakeEn = HBrake_En; /*硬刹车*/
	
	strMCtr.SBrakeDutyAdd_Q15= Sbrake_Duty_Add_Q15;
	strMCtr.SBrakeDutyMin_Q15 = Sbrake_Duty_Min_Q15;
	strMCtr.SBrakeDutyMax_Q15= Sbrake_Duty_Max_Q15;
	strMCtr.SBrakeDutyRun_Q15 = 0;
	strMCtr.SBrakeDutyWrite_Q15 = 0;
	
	strMCtr.SBrakeTime_10KCnt = Sbrake_Time_MS*((float)strHW.Fre_MSM/strHW.SToMS);
	strMCtr.HBrakeTIme_10KCnt = HBrake_Time_MS*((float)strHW.Fre_MSM/strHW.SToMS); 
	strMCtr.StopWaitTime_10KCnt = NoBrake_Time_MS*((float)strHW.Fre_MSM/strHW.SToMS);

	/*Motion*/
	strMCtr.MotionEn = Motion_En;
	strMCtr.MotionMaskTime_T0Cnt = Motion_MaskTime_US*((float)strHW.Fre_SectorTimer/strHW.SToUS);
	strMCtr.MotionShiftTime_T0Cnt = Motion_ShiftTime_US*((float)strHW.Fre_SectorTimer/strHW.SToUS);
	strMCtr.MotionNowSector_T0Cnt  = 0;
	strMCtr.MotionLastSector_T0Cnt = 0;
	strMCtr.MotionLast2Sector_T0Cnt = 0;
	strMCtr.MotionPass_Cnt = 0;
	strMCtr.MotionPass_CntSet = Motion_Pass_CntSet;
	strMCtr.MotionIRQRecord_Cnt = 0;
	strMCtr.MotionPassLoopRunSet = Motion_Pass_LoopRun_Set;
	strMCtr.MotionSectorMax_T0Cnt= Motion_SectorTime_Lim_US*((float)strHW.Fre_SectorTimer/strHW.SToUS);

	strMCtr.MotionWaitMin_10KCnt = Motion_CheckTime_Min_MS*((float)strHW.Fre_MSM/strHW.SToMS);
	strMCtr.MotionWaitAdd_10KCnt = Motion_CheckTime_OnceIRQ_Add_MS*((float)strHW.Fre_MSM/strHW.SToMS);
	strMCtr.MotionBrake_Flag = 0;
	strMCtr.MotionBrakeMin_10KCnt = Motion_BrakeTime_Min_MS*((float)strHW.Fre_MSM/strHW.SToMS);
	strMCtr.MotionBrakeAdd_10KCnt = Motion_BrakeTime_OnceIRQ_Add_MS*((float)strHW.Fre_MSM/strHW.SToMS);



	strMCtr.CoexistCmp3Ad = 1;/*比较器使能时，3AD是否关闭*/

	strMCtr.AdcScanOLFallRatio_Q15 = __Q15(0.47);
	strMCtr.AdcScanOLRiseRatio_Q15 = __Q15(0.53);
	strMCtr.AdcScanCLFallRatio_Q15 = __Q15(0.48);
	strMCtr.AdcScanCLRiseRatio_Q15 = __Q15(0.52);
	strMCtr.AdcScanFallRatio_Q15 = strMCtr.AdcScanOLFallRatio_Q15;
	strMCtr.AdcScanRiseRatio_Q15 = strMCtr.AdcScanOLRiseRatio_Q15;

	strMCtr.AdcScanCrossZeroEnd= 0;

	strMCtr.AcmpScanCrossZeroEnd =  0;
	strMCtr.AcmpScanResult = 0;

	strMCtr.AcmpIRQDisFactor_T0Cnt = 0;
	strMCtr.AcmpIRQEnFactor_T0Cnt = 0;
	strMCtr.AcmpIRQEn_PeriodCnt = AcmpIRQ_En_Period_Cnt;
	strMCtr.AcmpIRQDis_PeriodCnt = AcmpIRQ_Dis_Period_Cnt;

	strMCtr.AcmpIRQEn_Cnt = 0;
	strMCtr.AcmpIRQEn_CntSet = AcmpIRQ_En_CntSet;

	strMCtr.AcmpIRQEnHighFreData_T0Cnt = strMCtr.HighFrePeriod_Us*strMCtr.AcmpIRQEn_PeriodCnt*((float)strHW.Fre_SectorTimer/1000000);
	strMCtr.AcmpIRQEnLowFreData_T0Cnt = strMCtr.LowFrePeriod_Us*strMCtr.AcmpIRQEn_PeriodCnt*((float)strHW.Fre_SectorTimer/1000000);

	strMCtr.AcmpIRQDisHighFreData_T0Cnt = strMCtr.HighFrePeriod_Us*strMCtr.AcmpIRQDis_PeriodCnt*((float)strHW.Fre_SectorTimer/1000000);
	strMCtr.AcmpIRQDisLowFreData_T0Cnt = strMCtr.LowFrePeriod_Us*strMCtr.AcmpIRQDis_PeriodCnt*((float)strHW.Fre_SectorTimer/1000000);

	strMCtr.AcmpIRQCrossZero_Flag = 0; // IRQ 中断过零标志位
	strMCtr.AcmpIRQResult = 0; // IRQ 比较器结果
	strMCtr.AcmpIRQCrossZeroEnd = 0;


	strMCtr.AntiphaseRunState = 0;
	strMCtr.AntiphaseScanResult= 0;
	strMCtr.AntiphaseEn_Flag = 0;
	strMCtr.AntiphaseDisDuty_Q15 = 6553;  /*0.2*/
	strMCtr.AntiphaseEnDuty_Q15= 5898;/*0.18*/

   	strMCtr.IBusAvgCh_Adc = 0;
    strMCtr.IBusAvg_Adc = 0;
    strMCtr.IBusAvg_Ax100 = 0;
    strMCtr.gIBusAvgLimEn = IBus_Avg_Lim_En;
    strMCtr.IBusAvgLim_Ax100 = IBus_Avg_Lim_A*100;
    strMCtr.IBusAvgLim_Adc = IBus_Ax100ConvertToAdc(strMCtr.IBusAvgLim_Ax100);

    strMCtr.IBusPeakCh_Adc = 0;
    strMCtr.IBusPeak_Ax100 = 0;
    strMCtr.IBusPeak_Adc = 0;
    strMCtr.gIBusPeakLimEn = IBus_Peak_Lim_En;
    strMCtr.IBusPeakLim_Ax100 = IBus_Peak_Lim_A*100;
    strMCtr.IBusPeakLim_Adc = IBus_Ax100ConvertToAdc(strMCtr.IBusPeakLim_Ax100);

	strMCtr.gIBusLimit = 0;
    strMCtr.IBusOffsetRef_Adc = (uint32_t)strHW.IBusOffsetRef_Vx100*strHW.AdcMax_Adc/strHW.AdcRef_Vx100;


	strMCtr.MotorState_Cnt = 0; 

	strMCtr.ColdStartMode = MotorStart_Mode;
	strMCtr.DriveMode = LoopMode_Set;

	strMCtr.HighVoltMode = 0;
	strMCtr.HighVoltSetV_x100 = 220*100; //220 000mv
	strMCtr.HighVoltSetAdc = VBus_Vx100ConvertToAdc(strMCtr.HighVoltSetV_x100);



}


/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void MC_GetIBusOffset(void)
{
#if 0
	
	uint8_t i;
	uint16_t sumAvg = 0;
	uint16_t sumPeak = 0;

	 __disable_irq(); /*全局中断禁止*/
	

	for(i=0;i<16;i++)
	{
		ADCH_Convert_Wait_End(ADC1_IBusPeak_Convert);
		sumPeak += ADCH_GetResult(ADC1_IBusPeak_Result); //电流峰值
		sumAvg +=  ADCH_GetResult(ADC1_IBusAvg_Result); //电流平均值
	}
	
	strMCtr.IBusPeakOffset_Adc = (sumPeak >> 4);
	strMCtr.IBusAvgOffset_Adc = (sumAvg >> 4);
	if(
	  (strMCtr.IBusPeakOffset_Adc < (strMCtr.IBusOffsetRef_Adc>>1))
	||(strMCtr.IBusPeakOffset_Adc > (strMCtr.IBusOffsetRef_Adc + (strMCtr.IBusOffsetRef_Adc>>1)))
	||(strMCtr.IBusAvgOffset_Adc < (strMCtr.IBusOffsetRef_Adc>>1))
	||(strMCtr.IBusAvgOffset_Adc > (strMCtr.IBusOffsetRef_Adc + (strMCtr.IBusOffsetRef_Adc>>1)))
	)	
	{
		
		strUErr.userErr.bits.gIBusOffset = 1;

	}  

#endif  
}





/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments  : 
* Return Value : 
***********************************************************************************************************************/
void MC_Rpm_Init(void)
{    
	// rpm
	strMCtr.gRpmUpdate = 0;
	strMCtr.Rpm =0;
	strMCtr.RpmLast = 0;
	strMCtr.RpmTemp = 0;
	strMCtr.RpmFilter =0;

	strMCtr.RpmSetMax = LoopSpeed_AimSet_Max;
	strMCtr.RpmSetMin = LoopSpeed_AimSet_Min;


	strMCtr.gRpmBlock = 0;
	strMCtr.RpmBlockZtoZ_T0Cnt = 0;
	strMCtr.RpmBlockCtoZ_T0Cnt = 0;
	
	for(uint8_t sectori = 0;sectori<8;sectori++)
	{
		strMCtr.SectorCtoCTab[sectori] = 0;
	}


    strPid.kp = (LoopSpeed_PID_Kp);
    strPid.ki = (LoopSpeed_PID_Ki);
    strPid.error = 0;
    strPid.error1 = 0;
    strPid.error2 = 0;
    strPid.refValue = 0;
    strPid.output = 0;
    strPid.lowerLimitOutput = strMCtr.DutyLimitMin_Q15;
    strPid.upperLimitOutput = strMCtr.DutyLimitMax_Q15;          
    strPid.pTemp = 0;
    strPid.iTemp = 0;
    strPid.deltaUk = 0; //deltaUk = Uk - Uk_1;  Uk为位置式PI输出
    strPid.deltaOut = 0;
    strPid.rpmSetPwmDuty = 0;

	strPid.errorIntegral= 0;
	strPid.KpDuty = 0;
	strPid.KiDuty = 0;
	strPid.OutDuty= 0;

	K_x= (655); //滤波系数
	new_flag_x=0;//本次数据变化方向
	num_x=0;//滤波计数器
}



/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments  : 
* Return Value : 
***********************************************************************************************************************/

void MC_Get_Rpm(void)
{   
//  a = 6.28*f*t*32768  t=0.001s f=?1hz
//    static int32_t MotorRpmLast; 
	strMCtr.SectorCtoCTab[6] = 0;
	for(uint8_t sectori = 0;sectori<8;sectori++)
	{
		strMCtr.SectorCtoCTab[6] += strMCtr.SectorCtoCTab[sectori];
	}
	
    if(strMCtr.gRpmBlock)
    {
        /*转速计算*/
         __disable_irq(); /*全局中断禁止*/
        //strMCtr.RpmTemp =  HWDIV_Div(strMCtr.RpmDivisor, (strMCtr.RpmBlockCtoZ_T0Cnt*12));  //jack
        /*转速计算*/
        __enable_irq(); /*全局中断使能*/
        strMCtr.gRpmBlock = 0;
    }
    else
    {
        /*转速计算*/
         __disable_irq(); /*全局中断禁止*/
        //strMCtr.RpmTemp =  HWDIV_Div(strMCtr.RpmDivisor, (strMCtr.SectorZtoZ2sAvg_T0Cnt*6)); //jack
        /*转速计算*/
       __enable_irq(); /*全局中断使能*/
    }


    strMCtr.RpmLast = strMCtr.Rpm;
	//MotorRpmLast  = (strMCtr.RpmTemp +MotorRpmLast)>>1;
    strMCtr.Rpm = strMCtr.RpmTemp;

    //strMCtr.RpmFilter = strMCtr.RpmFilter + (2050*(strMCtr.Rpm -strMCtr.RpmFilter )>>15);

	strMCtr.Rpm = filter_1_x(strMCtr.Rpm,strMCtr.RpmLast);
}







/***********************************************************************************************************************
* Function Name: IPD 准备函数
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
uint8_t ipdtemp[10];     
uint8_t ipdtempCnt = 0; 
uint8_t MC_DectectPosition_Start(void)
{
    uint8_t ret;
	
#if 0 //jack
	
    uint8_t rotorSectorNow = 7;
    uint8_t rotorSectorLast = 7;
    uint8_t ipdOKCnt = 0;
    uint8_t ipdRun = 1;

    ret=1;


	for(ipdtempCnt = 0;ipdtempCnt<10;ipdtempCnt++)
	{
		ipdtemp[ipdtempCnt] = 0;
	}
	ipdtempCnt= 0;
	ADCH_HardwareTrigger_Disable();
	//MC_PwmMask_Set(DRIVER_ABCL);	
	//Delay_MSx(2);
	while(ipdRun)
	{
		MC_PwmMask_Set(DRIVER_ABCL);	
		Delay_MSx(2);
		ret = MC_IPD_6Pulse();
		if(ret)
		{
			rotorSectorNow = strMCtr.RotorSector;
			ipdtemp[ipdtempCnt] = rotorSectorNow;
			ipdtempCnt++;
			if(ipdtempCnt>=10)
				ipdtempCnt = 0;
			if(rotorSectorNow == rotorSectorLast)
			{
				ipdOKCnt++;
			}
			else if(ipdOKCnt)
			{
				ipdOKCnt--;
			}
			//if(ipdOKCnt>=0)
			//{
				ipdRun = 0;
			//}
			rotorSectorLast = rotorSectorNow;
		}
		else
		{
			ipdRun = 0;
		}

	}

	//strMCtr.RotorSector+=1;
	if(strMCtr.RotorSector>5)
	{
		strMCtr.RotorSector = 0;
	}
	ADCH_HardwareTrigger_Enable();
   
#endif //

    return ret;

}

/***********************************************************************************************************************
* Function Name:  p f n adcget
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
        

void MC_3PhaseAdcRead(u8 adIndex)
{
    //adIndex
    // 0 第一次读取PWM ON反电动势
    // 1 第二次读取PWM OFF反电动势
#if 0 //jack
    
    if(strMCtr.gMotorDir == 0)
    {		
        if(adIndex == 0)
        {
            strMCtr.AdcScanPOn_Adc = ADCH_GetResult(GetADCTabP[strMCtr.RotorSector]);
            strMCtr.AdcScanFOn_Adc = ADCH_GetResult(GetADCTabF[strMCtr.RotorSector]);
            strMCtr.AdcScanNOn_Adc = ADCH_GetResult(GetADCTabN[strMCtr.RotorSector]);
        }
        else
        {		
            strMCtr.AdcScanPOff_Adc = ADCH_GetResult(GetADCTabP[strMCtr.RotorSector]);
            strMCtr.AdcScanFOff_Adc = ADCH_GetResult(GetADCTabF[strMCtr.RotorSector]);
            strMCtr.AdcScanNOff_Adc = ADCH_GetResult(GetADCTabN[strMCtr.RotorSector]);
        }						
    }
    else
    {
        if(adIndex == 0)
        {
            strMCtr.AdcScanPOn_Adc = ADCH_GetResult(GetADCTabN[strMCtr.RotorSector]);
            strMCtr.AdcScanFOn_Adc = ADCH_GetResult(GetADCTabF[strMCtr.RotorSector]);
            strMCtr.AdcScanNOn_Adc = ADCH_GetResult(GetADCTabP[strMCtr.RotorSector]);
        }
        else
        {
            strMCtr.AdcScanPOff_Adc = ADCH_GetResult(GetADCTabP[strMCtr.RotorSector]);
            strMCtr.AdcScanFOff_Adc = ADCH_GetResult(GetADCTabF[strMCtr.RotorSector]);
            strMCtr.AdcScanNOff_Adc = ADCH_GetResult(GetADCTabN[strMCtr.RotorSector]);			
        }
    }
    
	strMCtr.VPhaseMaxTemp_Adc = strMCtr.AdcScanPOn_Adc;
    if(strMCtr.VPhaseMaxTemp_Adc  < strMCtr.AdcScanNOn_Adc)
    {
        strMCtr.VPhaseMaxTemp_Adc = strMCtr.AdcScanNOn_Adc;
    }
    if(strMCtr.VPhaseMaxTemp_Adc  < strMCtr.AdcScanFOn_Adc)
    {
        strMCtr.VPhaseMaxTemp_Adc = strMCtr.AdcScanFOn_Adc;
    }
	strMCtr.VPhaseMax_Adc = strMCtr.VPhaseMaxTemp_Adc;
	
	#endif
}



/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments	 : 
* Return Value : 
***********************************************************************************************************************/

void MC_DutyBuf_Set(void)
{   

	static uint16_t pidAdjustCnt = 0;


    if(strMCtr.MotorState!= MOTOR_RUN)
    {
        strMCtr.DutyBuf_Q15 = 0;
        return;     
    }   
    
	if(strMCtr.gRpmUpdate == 1)
    {
        strMCtr.gRpmUpdate= 0;
        MC_Get_Rpm();
    }

	if(strMCtr.gSectorCloseLoop == 0)
	{
	    //strMCtr.DutyBuf_Q15 = 0;
	    //return;  
	}

    if(strMCtr.DriveMode == LoopMode_Speed) 
    {

		if(++pidAdjustCnt >= strMCtr.DutyBufDeal_FiltCnt)
		{                   
			pidAdjustCnt = 0;
			
			strMCtr.RpmSet = strMCtr.RpmSetMin + ((strMCtr.RpmSetMax-strMCtr.RpmSetMin)*strMCtr.AimSet_Q15>>15);

			if(strMCtr.RpmSet>strMCtr.RpmSetMax)
			{
				strMCtr.RpmSet = strMCtr.RpmSetMax;
			}
			else if(strMCtr.RpmSet<strMCtr.RpmSetMin)
			{
				strMCtr.RpmSet = strMCtr.RpmSetMin;
			}
			
	        
            if(strMCtr.RpmSet !=strPid.refValue)
            {
                strPid.refValue = strMCtr.RpmSet;        
            }

        	if(strMCtr.gSectorCloseLoop)
        	{
				#if 0//jack	
				strMCtr.DutyBufAim_Q15 = MC_PI_Handler(&strPid,strMCtr.Rpm);  
				#endif
        	}
			else
			{
				strMCtr.DutyBufAim_Q15 = 3276;
			}
		} 
    }
    else if(strMCtr.DriveMode == LoopMode_None) /*开环模式*/
    {        
		if(++pidAdjustCnt >= strMCtr.DutyBufDeal_FiltCnt)
		{                   
			pidAdjustCnt = 0;
            //strMCtr.DutySet_Q15 = strMCtr.AimSet_Q15;
			strMCtr.DutySet_Q15 = strMCtr.DutySetMin + ((strMCtr.DutySetMax-strMCtr.DutySetMin)*strMCtr.AimSet_Q15>>15);
			
	        if(strMCtr.DutyBufAim_Q15 < strMCtr.DutySet_Q15)
	        {			
                if((strMCtr.DutyBufAim_Q15+ strMCtr.DutyBufStep_Q15) <strMCtr.DutySet_Q15)
                {
                    strMCtr.DutyBufAim_Q15 += strMCtr.DutyBufStep_Q15;
                }
                else
                {
                    strMCtr.DutyBufAim_Q15 = strMCtr.DutySet_Q15;
                }
	        }
	    	else if(strMCtr.DutyBufAim_Q15 > strMCtr.DutySet_Q15)
	    	{  
                if((strMCtr.DutyBufAim_Q15) >(strMCtr.DutyBufStep_Q15+strMCtr.DutySet_Q15))
                {
                    strMCtr.DutyBufAim_Q15 -= strMCtr.DutyBufStep_Q15;
                }
                else
                {
                    strMCtr.DutyBufAim_Q15 = strMCtr.DutySet_Q15;
                }
	    	}	
		}
    }
              

    strMCtr.DutyBuf_Q15  = strMCtr.DutyBufAim_Q15;


    if(strMCtr.DutyBuf_Q15 > strMCtr.DutyLimitMax_Q15)
    {
        strMCtr.DutyBuf_Q15 = strMCtr.DutyLimitMax_Q15;
    }
    else    if(strMCtr.DutyBuf_Q15 < strMCtr.DutyLimitMin_Q15)
    {
        strMCtr.DutyBuf_Q15 = strMCtr.DutyLimitMin_Q15;
    }       
    
	strMCtr.DutyBufPercentx100 = strMCtr.DutyBuf_Q15*100>>15;
}



/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments	 : 
* Return Value : 
***********************************************************************************************************************/

void MC_PwmDutySet(void)
{ 

	if(
    ((strMCtr.gIBusAvgLimEn== 1)
    &&(strMCtr.IBusAvg_Adc > strMCtr.IBusAvgLim_Adc))
	||
	((strMCtr.gIBusPeakLimEn== 1)
	&&(strMCtr.IBusPeak_Adc > strMCtr.IBusPeakLim_Adc))
    )/*限流时刻占空比调整*/
    {
        strMCtr.gIBusLimit= 1;
    }

    if(strMCtr.gIBusLimit)
    {            
        strMCtr.gIBusLimit= 0;
        
        if((strMCtr.DutyRun_Q15) >(strMCtr.DutyRunStepReduce_Q15+strMCtr.DutyLimitMin_Q15))
        {
            strMCtr.DutyRun_Q15 -= strMCtr.DutyRunStepReduce_Q15;
        }
        else
        {
            strMCtr.DutyRun_Q15 = strMCtr.DutyLimitMin_Q15;
        }
    } 
    else 
	{
	    if(!strMCtr.gSectorCloseLoop)		
	    {			
	        if(strMCtr.DutyRun_Q15 < strMCtr.DutyBuf_Q15)
	        {			
                if((strMCtr.DutyRun_Q15+strMCtr.DutyRunStepAddOpen_Q15) <strMCtr.DutyBuf_Q15)
                {
                    strMCtr.DutyRun_Q15 += strMCtr.DutyRunStepAddOpen_Q15;
                }
                else
                {
                    strMCtr.DutyRun_Q15 = strMCtr.DutyBuf_Q15;
                }

	        }
	    	else if(strMCtr.DutyRun_Q15 > strMCtr.DutyBuf_Q15)
	    	{  
                if((strMCtr.DutyRun_Q15) >(strMCtr.DutyRunStepReduce_Q15+strMCtr.DutyBuf_Q15))
                {
                    strMCtr.DutyRun_Q15 -= strMCtr.DutyRunStepReduce_Q15;
                }
                else
                {
                    strMCtr.DutyRun_Q15 = strMCtr.DutyBuf_Q15;
                }
	    	}
	    	
	    /*限制开环最大最小值,非吸尘器类启动禁止这n条*/
	    	if(strMCtr.ColdStartMode == MotorStart_LowVolt)
	    	{
				strMCtr.DutyRun_Q15 = strMCtr.OpenloopDutyLim;
	        
				strPid.OutDuty = strMCtr.OpenloopDutyLim;
				strPid.KpDuty = strMCtr.OpenloopDutyLim;
				strPid.errorIntegral = 0;
	    	}

	    }	
	    else
	    {		
	    	if(strMCtr.DutyRun_Q15 < strMCtr.DutyBuf_Q15)
	    	{
                if((strMCtr.DutyRun_Q15+strMCtr.DutyRunStepAddClosed_Q15) <strMCtr.DutyBuf_Q15)
                {
                    strMCtr.DutyRun_Q15 += strMCtr.DutyRunStepAddClosed_Q15;
                }
                else
                {
                    strMCtr.DutyRun_Q15 = strMCtr.DutyBuf_Q15;
                }
	    	}
	    	else if(strMCtr.DutyRun_Q15 > strMCtr.DutyBuf_Q15)
	    	{
                if((strMCtr.DutyRun_Q15) >(strMCtr.DutyRunStepReduce_Q15+strMCtr.DutyBuf_Q15))
                {
                    strMCtr.DutyRun_Q15 -= strMCtr.DutyRunStepReduce_Q15;
                }
                else
                {
                    strMCtr.DutyRun_Q15 = strMCtr.DutyBuf_Q15;
                }
	    	}
	    }
	}
			

    if(strMCtr.DutyRun_Q15 < strMCtr.DutyLimitMin_Q15)
    {
    	strMCtr.DutyRun_Q15 = strMCtr.DutyLimitMin_Q15;
    }

    if(strMCtr.DutyRun_Q15 > strMCtr.DutyLimitMax_Q15)
    {
    	strMCtr.DutyRun_Q15 = strMCtr.DutyLimitMax_Q15;
    }			

}


/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

uint16_t MC_MaskOutput(uint16_t ucDriver)
{
	
	uint16_t maskData = ABCL|ABCH;
	/*去掉未用到数据位*/
	maskData = (ucDriver&maskData);

#if(PWM_Driver_Set == PWM_Driver_Up_H_Down_H)
		/*6N  驱动，全部正相输出(0x00<<8)*/
	maskData = (uint16_t)ucDriver;
	
#elif(PWM_Driver_Set == PWM_Driver_Up_L_Down_L)
		/*6P  驱动，全部反相输出(0x3F<<8)*/
	if(maskData&AH)
		maskData &= ~AH;
	else
		maskData |= AH;

	if(maskData&BH)
		maskData &= ~BH;
	else
		maskData |= BH;

	if(maskData&CH)
		maskData &= ~CH;
	else
		maskData |= CH;

	if(maskData&AL)
		maskData &= ~AL;
	else
		maskData |= AL;

	if(maskData&BL)
		maskData &= ~BL;
	else
		maskData |= BL;

	if(maskData&CL)
		maskData &= ~CL;
	else
		maskData |= CL;
	
#elif(PWM_Driver_Set == PWM_Driver_Up_H_Down_L)
		/*3N3P	驱动，下桥135反相输出(0x2A<<8)*/
	/*反相*/
	if(maskData&AL)
		maskData &= ~AL;
	else
		maskData |= AL;

	if(maskData&BL)
		maskData &= ~BL;
	else
		maskData |= BL;

	if(maskData&CL)
		maskData &= ~CL;
	else
		maskData |= CL;

	
#elif(PWM_Driver_Set == PWM_Driver_Up_L_Down_H)
		/*3P3N	驱动，上桥024反相输出(0x15<<8)*/
	/*反相*/
	if(maskData&AH)
		maskData &= ~AH;
	else
		maskData |= AH;

	if(maskData&BH)
		maskData &= ~BH;
	else
		maskData |= BH;

	if(maskData&CH)
		maskData &= ~CH;
	else
		maskData |= CH;

#endif
	return ucDriver = (ucDriver&(0x3f00))|maskData;
}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

uint16_t MCPWM_TIMER0_MaskOutput(uint16_t ucDriver)
{
	
	uint16_t maskData = ABCL|ABCH;
	/*去掉未用到数据位*/
	maskData = (ucDriver&maskData);

#if(PWM_Driver_Set == PWM_Driver_Up_H_Down_H)
		/*6N  驱动，全部正相输出(0x00<<8)*/
	maskData = (uint16_t)ucDriver;
	
#elif(PWM_Driver_Set == PWM_Driver_Up_L_Down_L)
		/*6P  驱动，全部反相输出(0x3F<<8)*/
	if(maskData&AH)
		maskData &= ~AH;
	else
		maskData |= AH;

	if(maskData&BH)
		maskData &= ~BH;
	else
		maskData |= BH;

	if(maskData&CH)
		maskData &= ~CH;
	else
		maskData |= CH;

	if(maskData&AL)
		maskData &= ~AL;
	else
		maskData |= AL;

	if(maskData&BL)
		maskData &= ~BL;
	else
		maskData |= BL;

	if(maskData&CL)
		maskData &= ~CL;
	else
		maskData |= CL;
	
#elif(PWM_Driver_Set == PWM_Driver_Up_H_Down_L)
		/*3N3P	驱动，下桥135反相输出(0x2A<<8)*/
	/*反相*/
	if(maskData&AL)
		maskData &= ~AL;
	else
		maskData |= AL;

	if(maskData&BL)
		maskData &= ~BL;
	else
		maskData |= BL;

	if(maskData&CL)
		maskData &= ~CL;
	else
		maskData |= CL;

	
#elif(PWM_Driver_Set == PWM_Driver_Up_L_Down_H)
		/*3P3N	驱动，上桥024反相输出(0x15<<8)*/
	/*反相*/
	if(maskData&AH)
		maskData &= ~AH;
	else
		maskData |= AH;

	if(maskData&BH)
		maskData &= ~BH;
	else
		maskData |= BH;

	if(maskData&CH)
		maskData &= ~CH;
	else
		maskData |= CH;

#endif
	return ucDriver = (ucDriver&(0x3f00))|maskData;
}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

uint16_t MCPWM_TIMER1_MaskOutput(uint16_t ucDriver)
{
	
	uint16_t maskData = ABCL|ABCH;
	/*去掉未用到数据位*/
	maskData = (ucDriver&maskData);

#if(PWM_Driver_Set == PWM_Driver_Up_H_Down_H)
		/*6N  驱动，全部正相输出(0x00<<8)*/
	maskData = (uint16_t)ucDriver;
	
#elif(PWM_Driver_Set == PWM_Driver_Up_L_Down_L)
		/*6P  驱动，全部反相输出(0x3F<<8)*/
	if(maskData&AH)
		maskData &= ~AH;
	else
		maskData |= AH;

	if(maskData&BH)
		maskData &= ~BH;
	else
		maskData |= BH;

	if(maskData&CH)
		maskData &= ~CH;
	else
		maskData |= CH;

	if(maskData&AL)
		maskData &= ~AL;
	else
		maskData |= AL;

	if(maskData&BL)
		maskData &= ~BL;
	else
		maskData |= BL;

	if(maskData&CL)
		maskData &= ~CL;
	else
		maskData |= CL;
	
#elif(PWM_Driver_Set == PWM_Driver_Up_H_Down_L)
		/*3N3P	驱动，下桥135反相输出(0x2A<<8)*/
	/*反相*/
	if(maskData&AL)
		maskData &= ~AL;
	else
		maskData |= AL;

	if(maskData&BL)
		maskData &= ~BL;
	else
		maskData |= BL;

	if(maskData&CL)
		maskData &= ~CL;
	else
		maskData |= CL;

	
#elif(PWM_Driver_Set == PWM_Driver_Up_L_Down_H)
		/*3P3N	驱动，上桥024反相输出(0x15<<8)*/
	/*反相*/
	if(maskData&AH)
		maskData &= ~AH;
	else
		maskData |= AH;

	if(maskData&BH)
		maskData &= ~BH;
	else
		maskData |= BH;

	if(maskData&CH)
		maskData &= ~CH;
	else
		maskData |= CH;

#endif
	return ucDriver = (ucDriver&(0x3f00))|maskData;
}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void MC_PwmMask_Set(uint16_t ucDriver)       
{ 
    // 根据管子高导通或者低导通设置掩码输出极性
    ucDriver = MC_MaskOutput(ucDriver);

	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */

	MCPWM0_MASK_LOW = ucDriver;
	
	MCPWM0_PRT		= 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */

}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
uint8_t MC_PWM_BrakeRead(void)
{	
//	uint8_t brakeFlag = EPWM_GET_BRAKE_RIS_FLAG(); //jack
//	return brakeFlag;
	return 0;
}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void MC_LoseStep_Deal(void)
{	
    if(
    (strMErr.LoseStepProtect_Flag == 1)
    &&(strMCtr.MotorState == MOTOR_RUN)
    )
    {/* 失步与堵转移相限制*/
        strMCtr.BlockShiftLimClr_Cnt  = 0;
        strMCtr.BlockShiftLim_Flag = 1;
        strMCtr.SectorCloseLoop_Cnt = strMCtr.SectorCloseLoop_Cnt>>1;

       if(strMErr.LoseStepProtectEn == 1)/*失步计数累计*/
       {
           strMErr.LoseStepProtect_Cnt+=3;
           if(strMErr.LoseStepProtect_Cnt > strMErr.LoseStepProtect_CntSet)
           {
                strMErr.LoseStepProtect_Cnt = 0;

				strMCtr.motorErr.bits.gLosStep = 1;
           }
       }
    }
    else
    {
        if (strMErr.LoseStepProtect_Cnt>0)
        {
            strMErr.LoseStepProtect_Cnt--;
        }
        
        if(++strMCtr.BlockShiftLimClr_Cnt >=strMCtr.BlockShiftLimClr_CntSet)
        {
            strMCtr.BlockShiftLimClr_Cnt  = 0;
            strMCtr.BlockShiftLim_Flag = 0;
        }
    }

    if(!strMCtr.gSectorCloseLoop)
    {/*闭环标志置1*/
        strMCtr.SectorCloseLoop_Cnt++;
        if(strMCtr.SectorCloseLoop_Cnt >= strMCtr.SectorCloseLoop_CntSet)
        {
            strMCtr.gSectorCloseLoop = 1;	
        }
    }
    else
    {

    }

	if(strMCtr.WrenchAutoStopSectorCnt<60000)
    	strMCtr.WrenchAutoStopSectorCnt++;


}

/***********************************************************************************************************************
* Function Name:
* Description  :  motor block
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void MC_CheckBlock(void)
{	
    //static uint16_t IndexLast = 0;
    if(strMErr.BlockProtectEn == 1)
    {
    	/*motor block protect*/	
    	if(strMCtr.MotorState== MOTOR_RUN)
    	{
    		strMErr.BlockProtect_Cnt++;

    		if (strMCtr.DutyRun_Q15 > strMErr.BlockProtectDuty_Q15)
    		{
    			if(strMErr.BlockProtect_Cnt >= strMErr.BlockProtectDutyH_CntSet ) // 1ms一个值
    			{
    				
                    strMErr.BlockProtect_Cnt = 0;


					strMCtr.motorErr.bits.gBlock = 1; //
    			}			
    		}
    		
    		if(strMErr.BlockProtect_Cnt >= strMErr.BlockProtectDutyL_CntSet)// 1ms一个值
    		{

				strMCtr.motorErr.bits.gBlock = 1; //

                strMErr.BlockProtect_Cnt = 0;
    		}		

    	}
    	else
    	{
    		strMErr.BlockProtect_Cnt = 0;
    		strMErr.BlockProtectErr_Cnt = 0;
    	}

    }

}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void MC_OpenLoop_MaskDeal(void)
{

	if(strMCtr.MaskTimeOLDis == 0)
	{
		strMCtr.MaskTime_T0Cnt = strMCtr.MaskTimeOLoop_T0Cnt;
	}
	
    if(strMCtr.OpenloopDutyLim >= openLoopDutyMax)//20
    {
		strMCtr.OpenloopDutyLim =openLoopDutyMax;
    }
    else
    {	/*保证低速开环能够提速*/
		 strMCtr.OpenloopDutyLim +=(openLoopDutyMax-strMCtr.OpenloopDutyLim)>>4;
    }

    strMCtr.ShiftTime_T0Cnt = strMCtr.ShiftTimeOLoop_T0Cnt;

	/*开环次数限制*/
	openLoopRunCnt+=1;
	if(openLoopRunCnt>openLoopRunLoseStep)
	{
		openLoopRunCnt = 0;
		strMCtr.motorErr.bits.gOpenLoopErr = 1;
	}
}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void MC_ForceChangePhase(void)
{	
    /*移相使能*/
	strMCtr.SectorCloseLoop_Cnt = 0;
	//MC_ChangePhase_Deal();
	//MC_FindZero_Deal(); //jack
	
	/*设置屏蔽*/
	//strMCtr.MaskTime_T0Cnt = 8000;
	//MC_MaskStateStart_Set();
	//MC_MaskTime_Write();

    if(strMCtr.OpenloopDutyLim >= openLoopDutyMax)//20
    {
		strMCtr.OpenloopDutyLim =openLoopDutyMax;
    }
    else
    {
    	//可以保证低压的时候快速拖动起来
		 strMCtr.OpenloopDutyLim +=(openLoopDutyMax-strMCtr.OpenloopDutyLim)>>2;
    }

	strMErr.ForceChangePhase_T0Cnt = strMErr.ForceChangePhase_T0Cnt-(strMErr.ForceChangePhase_T0Cnt>>4);
 
    if(strMErr.ForceChangePhase_T0Cnt<testForceTime)
    {
    	strMErr.ForceChangePhase_T0Cnt = testForceTimeMax;
	}
	
	/*开环次数限制*/
	openLoopRunCnt+=2;
	/*开环次数限制*/
	if(openLoopRunCnt>openLoopRunLoseStep)
	{
		openLoopRunCnt = 0;
		strMCtr.motorErr.bits.gOpenLoopErr = 1;
	}
}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

//用MPU6050测得数据；对x轴滤波处理

#define Threshold_1     200       //阈值1用于一阶带参滤波器，变化角度大于此值时，计数增加
#define Threshold_2     5      //阈值2用于一阶带参滤波器，计数值大于此值时，增大参数，增强滤波跟随


/*****带系数修改的一阶滤波函数

入口：NEW_DATA    新采样的角度值
      OLD_DATA    上次滤波获得的角度结果
      k           滤波系数(代表在滤波结果中的权重)
      flag        上次数据变化方向
出口：result      本次滤波角度结果
*/
int32_t filter_1_x(int32_t NEW_DATA,int32_t OLD_DATA)
{
//2*pi*f*t = f(最大转速) t函数调用周期
	static uint8_t flag	= 0;
    //角度变化方向，new_flag=1表示角度增加，=0表示角度正在减小
    if((NEW_DATA-OLD_DATA)>0)
        new_flag_x=1;
    else if((NEW_DATA-OLD_DATA)<0)
        new_flag_x=0;


    if(new_flag_x==flag)  //此次变化与前一次变化方向是否一致，相等表示角度变化方向一致
    {
        num_x++;
        if(_AbsFun((NEW_DATA-OLD_DATA))>Threshold_1)
    //当变化角度大于Threshold_1度的时候，进行计数器num快速增加，以达到快速增大K值，提高跟随性
            num_x+=1;                           

        if(num_x>Threshold_2)   //计数阈值设置，当角度递增或递减速度达到一定速率时，增大K值
        {
        	if(K_x<16384)
        	{
				K_x=K_x+655;          //0.2为K_x的增长值，看实际需要修改
        	}
            
            num_x=0;
        }
    }
    else 
    {
        num_x=0;
        K_x = 655;     //角度变化稳定时K_x值，看实际修改
    }

    //OLD_DATA=((32768-K_x)*OLD_DATA>>15)+(K_x*NEW_DATA>>15);
    OLD_DATA = OLD_DATA + ((NEW_DATA - OLD_DATA)*K_x>>15);//32768/K_x = 最小变化转速,转速变化小于50，转速变量暂时不变
    flag = new_flag_x;
    
    return OLD_DATA;
}

int32_t Div_Handler(int32_t dividend,int32_t divisor)
{        
    //return HWDIV_Div(dividend,divisor); //jack
	return 0;
}

/*************************************************
Description:PI计算
Input:  
Return: 
Others: 
*************************************************/

int32_t MC_PI_Handler( PidPI_t *pi,int32_t errorBack)
{

    pi->error = (pi->refValue - errorBack)>>0;
    
    if(pi->error > 32768)
    {
		pi->error = 32768;
    }
    else if(pi->error <(- 32768))
    {
		pi->error = -32768;
    }
    
	pi->pTemp = pi->kp*(pi->error-pi->error1);

	
	pi->iTemp = 0;

	if(pi->OutDuty >=strMCtr.DutyLimitMax_Q15)
    {
		if( pi->error < 0)
			pi->iTemp = pi->ki*(pi->error);
    }
    else    if(pi->OutDuty <= strMCtr.DutyLimitMin_Q15)
    {
		if( pi->error > 0)
			pi->iTemp = pi->ki*(pi->error);
    }
    else
    {
		pi->iTemp = pi->ki*(pi->error);
    }


	pi->output += (pi->pTemp + pi->iTemp);

	if(pi->output > 1000000000)
    {
		pi->output = 1000000000;
    }
    else    if(pi->output < (-1000000000))
    {
		pi->output = (-1000000000);
    }
    
	pi->OutDuty = pi->output>>15;

    if(pi->OutDuty>strMCtr.DutyLimitMax_Q15)
    {
		pi->OutDuty = strMCtr.DutyLimitMax_Q15;
    }
    else    if(pi->OutDuty < strMCtr.DutyLimitMin_Q15)
    {
		pi->OutDuty = strMCtr.DutyLimitMin_Q15;
    }


    pi->rpmSetPwmDuty = pi->OutDuty;
 
	
	pi->error2 = pi->error1;
	pi->error1 = pi->error;

    return pi->rpmSetPwmDuty;
}



