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
/** \file motor_state.c
**
**	History:
**	
*****************************************************************************/
/****************************************************************************/
/*	include files
*****************************************************************************/

#include "bldc_motor_config.h"
#include "bldc_motor_state.h"
#include "bldc_motor_control.h"
#include "bldc_motor_mlib.h"

#include "bldc_motor_hall.h"


#include "bldc_user_control.h"
//#include "user_config.h"

#include "bldc_public_debug.h"
#include "bldc_public_function.h"






/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments	 : 
* Return Value : 
***********************************************************************************************************************/

uint8_t	PositionDrag;

uint16_t openLoopDutyMax = 6550;
uint16_t openLoopRunCnt = 0;
uint16_t openLoopRunLoseStep = 100;
uint32_t testMaskTime = 0;
uint32_t testForceTime = 40000;
uint32_t testForceTimeMax = ForceChangePhase_CntSet;

void MC_Machine(void) 
{	

        switch(strMCtr.MotorState)	
        {	
            case MOTOR_STOP:
                MC_PwmMask_Set(DRIVER_OFF);	

                Bemf_AcmpDisableInt();
                Bemf_AcmpClearIntFlag();
                
                Timer_MC_Stop();
                Timer_MC_INT_Disable();
                
                Timer_Record_Stop();
                Timer_Record_INT_Disable();
                /*换相次数清0*/
                strMCtr.WrenchAutoStopSectorCnt = 0;

                if(
                (strMCtr.gMotorEn == 1)
                &&(strUErr.userErr.word ==0)
                &&(strMErr.motorErr.word ==0)
                )
                {	
                    strMCtr.MotorState = MOTOR_READY;
                }
            break;

            case MOTOR_READY:	
            
                Debug_Signal_Low();
                strMStart.dragVFEn = 0;
                strMCtr.MotorState_Cnt = 0;
                //MC_DriveVariable_Init();  

                MC_Rpm_Init();
                
				ADCH_HWTG_UVW_Clr();
				strMCtr.MotionPass_Flag = 1; 
				
				strMCtr.AimSet_Q15 = 0;

				strMCtr.RpmSet = 0;
				strMCtr.DutySet_Q15= 0;
				strMCtr.DutyBuf_Q15 = 0;
				strMCtr.DutyRun_Q15 = 0;
				strMCtr.DutyBufAim_Q15= 0;
				strMCtr.DutyBufPercentx100 = 0;

                strMCtr.gSectorCloseLoop = 0;
                strMCtr.SectorCloseLoop_Cnt = 0;

                strMCtr.AcmpIRQEn_Flag = 0;
                strMCtr.ShiftDis_Cnt= 0;
                strMCtr.ShiftDis_Flag = 0;


                strMCtr.AdcScanCrossZeroEnd= 0;
                strMCtr.AcmpIRQCrossZeroEnd = 0;
				strMCtr.AcmpScanCrossZeroEnd = 0;
    
                strMCtr.MotionPass_Cnt = 0;
                strMCtr.MotionIRQRecord_Cnt= 0;
                strMErr.LoseStepProtect_Cnt = 0;
				strMCtr.OpenloopDutyLim= 1638;
				strMErr.ForceChangePhase_T0Cnt = ForceChangePhase_CntSet;
                strMCtr.SetFreData = strMCtr.LowFreData;
                MC_PwmDutyWrite(strMCtr.DutyRun_Q15);
				testMaskTime = 8000;
				openLoopRunCnt = 0;
                
                if(
                (strMCtr.gMotorEn == 0)
                ||(strUErr.userErr.word !=0)
                ||(strMErr.motorErr.word !=0)
                )
                {	
                    strMCtr.MotorState = MOTOR_STOP;
                }
                else  
                {   
                
                    if(strSensor.ModeEn)
                    {
                        MC_SensorMode_Start();
                    }
                    else if(strMCtr.MotionEn)
                    {                               
                        MC_HotStart_Init();
                        strMCtr.MotorState = MOTOR_MOTION; 
                    }
                    else
                    {
                        strMCtr.MotorState = MOTOR_POSITION;
                    }
                }

            break;

            case MOTOR_MOTION:	          
                strMCtr.MotorState_Cnt++;
                if(
                (strMCtr.gMotorEn == 0)
                ||(strUErr.userErr.word !=0)
                ||(strMErr.motorErr.word !=0)
                )
                {   
                    strMCtr.MotorState_Cnt = 0;
                    strMCtr.MotorState = MOTOR_STOP;
                }
                else
                {
                    strMCtr.MotionWait_10KCnt = strMCtr.MotionWaitMin_10KCnt+strMCtr.MotionIRQRecord_Cnt*strMCtr.MotionWaitAdd_10KCnt;
                
                    if(strMCtr.MotorState_Cnt >strMCtr.MotionWait_10KCnt)
                    {
                        Bemf_AcmpDisableInt();
                        Bemf_AcmpClearIntFlag();
                        Timer_MC_Stop();
                        Timer_MC_INT_Disable();
                        strMCtr.MotionBrake_Flag = 1;
                        strMCtr.MotorState_Cnt = 0;
                        strMCtr.MotorState = MOTOR_MOTION_FAIL;
                    }
                }
            break;
                
            case MOTOR_MOTION_FAIL:               
                strMCtr.MotorState_Cnt++;

                if(
                (strMCtr.gMotorEn == 0)
                ||(strUErr.userErr.word !=0)
                ||(strMErr.motorErr.word !=0)
                )
                {   
                    strMCtr.MotorState = MOTOR_STOP;
                }
                else
                {
                    MC_PwmMask_Set(DRIVER_ABCL);

                    if(strMCtr.MotionIRQRecord_Cnt == 0)
                    {
						if(strMCtr.MotorState_Cnt > (10))
                        {
                            strMCtr.MotorState_Cnt = 0;
                            strMCtr.MotorState =MOTOR_POSITION;
                        }
                    }
                    else
                    {
						if(strMCtr.MotorState_Cnt > (strMCtr.MotionBrakeMin_10KCnt + strMCtr.MotionIRQRecord_Cnt*strMCtr.MotionBrakeAdd_10KCnt))
                        {
                            strMCtr.MotorState_Cnt = 0;
                            strMCtr.MotorState =MOTOR_POSITION;
                        }
                    }
                    

                }
            break;

            case MOTOR_POSITION:	
                MC_ColdStart_Init();

                if(
                (strMCtr.gMotorEn == 0)
                ||(strUErr.userErr.word !=0)
                ||(strMErr.motorErr.word !=0)
                )
                {	
                    strMCtr.MotorState = MOTOR_STOP;
                }
                else
                {

                    if(strMCtr.ColdStartMode == MotorStart_IPD)
                    {
                    	if(strMStart.IPD.gEnd == 0)
                    	{
							return;
                    	}
                    	
						if(strMStart.IPD.gResult)
					    {                            
					        MC_PwmMask_Set(gTwoDriverTab[strMCtr.gMotorDir][strMCtr.RotorSector]);  
					        Bemf_AcmpSetPosCh_F_Run();

					        strMCtr.SectorZtoZ_T0Cnt = 40000;
					        strMCtr.SectorZtoZLast_T0Cnt= 40000;

					        Timer_Record_Write(strMCtr.SectorZtoZLast_T0Cnt);

					        strMCtr.RotorState = SECTOR_WAIT_END_MASK;
					        Timer_MC_Write_Start(800);
					        Timer_MC_INT_Enable();
					        strMCtr.MotorState = MOTOR_RUN;
					    } 
					    else
					    {
							strMCtr.MotorState = MOTOR_STOP;
					    }

					    strMStart.IPD.gEnd = 0;/*只能放到最后，防止主循环IPD连续进入两次*/
 
                    }
                    else if(strMCtr.ColdStartMode == MotorStart_Drag)
                    {
                        //strMStart.dragVFEn = 1;
                        strMCtr.MotorState = MotorStart_Drag_OPENLOOP;
                        PositionDrag = 0;
                    }
                    else if(strMCtr.ColdStartMode == MotorStart_LowVolt)
                    {
						strMCtr.MotorState = MotorStart_Drag_OPENLOOP;
                    }
                    else
                    {
                        strMCtr.MotorState = MOTOR_STOP;
                    }
                }
            break;
                
            case MotorStart_Drag_OPENLOOP:           

                if(
                (strMCtr.gMotorEn == 0)
                ||(strUErr.userErr.word !=0)
                ||(strMErr.motorErr.word !=0)
                )
                {
                    strMCtr.MotorState = MOTOR_STOP;
                }
                else
                {
                	//MC_Start_Position_By_Drag();

						MC_SquareWave_drag();
                	
                }
            break;

            case MOTOR_RUN:	

				if(strMCtr.MotionPass_Flag)
				{
					ADCH_HWTG_UVW_Set();
					strMCtr.MotionPass_Flag = 0;
				}
				
                MC_PwmDutySet();

				/*堵转发生在屏蔽后到过零前*/
                strMCtr.RpmBlockZtoZ_T0Cnt = Timer_Record_Read();

                if(strMCtr.RpmBlockZtoZ_T0Cnt > strMCtr.SectorZtoC_T0Cnt)
                {
					strMCtr.RpmBlockCtoZ_T0Cnt =  strMCtr.RpmBlockZtoZ_T0Cnt -strMCtr.SectorZtoC_T0Cnt;
                }
                else
                {
					strMCtr.RpmBlockCtoZ_T0Cnt = 0;
                }
        	 
        		
                if(
                (strMCtr.RpmBlockCtoZ_T0Cnt >(strMCtr.SectorCtoZ_T0Cnt<<1))
                &&(strMCtr.gRpmUpdate == 0)
                &&(strMCtr.RpmBlockCtoZ_T0Cnt > 200)
                )
                {
                    strMCtr.gRpmBlock= 1; 
                    strMCtr.gRpmUpdate = 1;

                }

                if(
                (strMCtr.gMotorEn == 0)
                ||(strUErr.userErr.word !=0)
                ||(strMErr.motorErr.word !=0)
                )
                {	
                    strMCtr.MotorState = MOTOR_STOP;
                    strMCtr.RotorState = SECTOR_STOP;

                    strMCtr.MotorState_Cnt = 0;
                    strMCtr.SBrakeDutyRun_Q15 = strMCtr.SBrakeDutyMin_Q15;


                    strMCtr.SetFreData = strMCtr.LowFreData;
                    MC_PwmMask_Set(DRIVER_OFF);

                    Bemf_AcmpDisableInt();
                    Bemf_AcmpClearIntFlag();
                    Timer_MC_Stop();
                    Timer_MC_INT_Disable();
              
            
                    if((strMErr.motorErr.word !=0)||(strUErr.userErr.word !=0))
                    {
                        strMCtr.MotorState = MOTOR_STOPWAIT;
                    }
                    else if(strMCtr.SBrakeEn == 1)
                    {
                        strMCtr.MotorState = MOTOR_SBRAKE;
                    }
                    else if(strMCtr.HBrakeEn == 1)
                    {
                        strMCtr.MotorState = MOTOR_HBRAKE;
                    }
                    else
                    {
                        strMCtr.MotorState = MOTOR_STOP;
                    }
     
                }

                MC_PwmDutyWrite(strMCtr.DutyRun_Q15);
                MC_CheckBlock();

            break;


            case MOTOR_SBRAKE:            

 
				strMCtr.SBrakeDutyRun_Q15 += strMCtr.SBrakeDutyAdd_Q15;
                	
                if(strMCtr.SBrakeDutyRun_Q15 > strMCtr.SBrakeDutyMax_Q15)
                {
                    strMCtr.SBrakeDutyRun_Q15 = strMCtr.SBrakeDutyMax_Q15;
                }
            
                /*加载下管占空比要与加载上管占空比取反*/
                if(strMCtr.SBrakeDutyRun_Q15<=32768)
                {
					strMCtr.SBrakeDutyWrite_Q15 = 32768 - strMCtr.SBrakeDutyRun_Q15;
                }
                else
                {	
					strMCtr.SBrakeDutyWrite_Q15 = 0;
                }
                	
                MC_PwmDutyWrite(strMCtr.SBrakeDutyWrite_Q15);
                
                MC_PwmMask_Set(DRIVER_ABCLPWM);

                if((strMErr.motorErr.word !=0)||(strUErr.userErr.word !=0))
                {
                    strMCtr.MotorState_Cnt = 0;
                    strMCtr.MotorState = MOTOR_STOPWAIT;
                }
                else 
                {
                    if((strMCtr.gMotorEn)&&(strMCtr.MotionEn))
                    {           
                        strMCtr.MotorState_Cnt = 0;
                        strMCtr.MotorState = MOTOR_STOP;           
                    }
                    else    if(++strMCtr.MotorState_Cnt >  strMCtr.SBrakeTime_10KCnt)  
                    {				
                        strMCtr.MotorState_Cnt = 0;
                        if(strMCtr.HBrakeEn == 1)
                        {
                            strMCtr.MotorState = MOTOR_HBRAKE;
                        }
                        else
                        {
                            strMCtr.MotorState = MOTOR_STOP;
                        }
                    }	
                }
   
            break;	

            case MOTOR_HBRAKE:
           
                MC_PwmMask_Set(DRIVER_ABCL);

                if((strMErr.motorErr.word !=0)||(strUErr.userErr.word !=0))
                {
                    strMCtr.MotorState_Cnt = 0;
                    strMCtr.MotorState = MOTOR_STOPWAIT;
                }
                else 
                {
                    if((strMCtr.gMotorEn)&&(strMCtr.MotionEn))
                    {    
                        strMCtr.MotorState_Cnt = 0;
                        strMCtr.MotorState = MOTOR_STOP;           
                    }
                    else    if(++strMCtr.MotorState_Cnt >  strMCtr.HBrakeTIme_10KCnt) 
                    {				
                        strMCtr.MotorState_Cnt = 0;
                        strMCtr.MotorState = MOTOR_STOP;
                    }	
                }

            break;

            case MOTOR_STOPWAIT:
           
                MC_PwmMask_Set(DRIVER_OFF);
                if(++strMCtr.MotorState_Cnt >  strMCtr.StopWaitTime_10KCnt)
                {
                    strMCtr.MotorState_Cnt = 0;
                    strMCtr.MotorState = MOTOR_STOP;
                }

            break;

            default:			
                MC_PwmMask_Set(DRIVER_OFF);

            break;

        }
}




/***********************************************************************************************************************
* Function Name: 静止启动前初始化变量
* Description  : 
* Arguments	 : 
* Return Value : 
***********************************************************************************************************************/

void MC_ColdStart_Init(void)
{
    Debug_Signal_Low();
    
    strMCtr.AcmpIRQEn_Flag = 0;
    strMCtr.RotorState = SECTOR_WAIT_END_MASK;
    strMCtr.AcmpIRQEn_Cnt= 0;
    strMCtr.AdcScanCrossZeroEnd= 0;
    strMCtr.AcmpIRQCrossZeroEnd = 0;
    strMCtr.AcmpScanCrossZeroEnd = 0;
    
    
    strMCtr.gSectorCloseLoop = 0;
    strMCtr.SectorCloseLoop_Cnt = 0;

}
/***********************************************************************************************************************
* Function Name: 静止启动
* Description  : 
* Arguments	 : 
* Return Value : 
***********************************************************************************************************************/

void MC_ColdStart_Handler(void)
{
	if(strMStart.IPD.gEnd == 0)
	{
		if(strMCtr.MotorState == MOTOR_POSITION)
		{
			if(strMCtr.ColdStartMode == MotorStart_IPD)
			{
				strMStart.IPD.gResult = MC_DectectPosition_Start();
				strMStart.IPD.gEnd = 1;
			}
		}
	}
}
/***********************************************************************************************************************
* Function Name:  滑行启动前初始化变量
* Description  : 
* Arguments	 : 
* Return Value : 
***********************************************************************************************************************/

void MC_HotStart_Init(void)
{

    Debug_Signal_Low();                        

    strMCtr.AcmpIRQEn_Flag = 1;
    strMCtr.RotorState = SECTOR_WAIT_END_MASK;
    
    strMCtr.AdcScanCrossZeroEnd= 0;
    strMCtr.AcmpIRQCrossZeroEnd = 0;
    strMCtr.AcmpScanCrossZeroEnd = 0;
    
    strMCtr.AcmpIRQEn_Cnt= 0;


    strMCtr.MotionBrake_Flag = 0;
    Bemf_AcmpSetPosCh_F_Motion();

    Timer_Record_Write(0); 

    Timer_MC_INT_Enable();
    Timer_MC_Write_Start(300);
}


/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void MC_UC_Update( void)
{

    /* 下载*/
    strUCtr.MotorState = strMCtr.MotorState;

    strUCtr.WrenchAutoStopSectorCnt = strMCtr.WrenchAutoStopSectorCnt;

}

/***********************************************************************************************************************
* Function Name: 3步拖动启动打IPD无法得出位置的电机
* Description  :  拖动将轻载电机拖到固定位置
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void MC_Start_Position_By_Drag(void)
{
	#define SYS_FRE 10
	static  uint8_t			phasefg = 0;
	static	uint16_t 		PositionDragTime;
	static  uint8_t			dragFlagEnd = 0;
	static  uint16_t		dragTime = 0;
	static  uint16_t		dragTimeEnd = 0;
	static 	uint16_t		dragDutyMax = 6552;
	static 	uint16_t		dragDutyLoad = 0;
	
	// 15V 15%DUTY 20MS
	if(PositionDrag == 0)
	{
		PositionDrag = 1;
		PositionDragTime = 0;
		phasefg = 1;
		dragFlagEnd = 0;
        MC_PwmMask_Set(DRIVER_OFF);
        dragTimeEnd = 500;
		dragFlagEnd = 0;
		dragTime = 0; 
		dragDutyLoad = 0;
    }
    else
    {
        PositionDragTime++;

		MC_PwmDutyWrite(dragDutyLoad);
		
        if(PositionDragTime<=(0*SYS_FRE))
        {
            MC_PwmMask_Set((u16)(gTwoDriverTab[strMCtr.gMotorDir][strMCtr.RotorSector]));
            phasefg = 0;
        }   
        else if(PositionDragTime<=(0*SYS_FRE))  //15000
        {
            if(phasefg ==0)
            {
                phasefg = 1;

				strMCtr.RotorSector = gNextSectorTab[strMCtr.gMotorDir][strMCtr.RotorSector];
            }
            MC_PwmMask_Set((u16)(gTwoDriverTab[strMCtr.gMotorDir][strMCtr.RotorSector]));
        }
        else if(PositionDragTime<=(0*SYS_FRE))  //15000
        {
            if(phasefg ==1)
            {
                phasefg = 0;
    
				strMCtr.RotorSector = gNextSectorTab[strMCtr.gMotorDir][strMCtr.RotorSector];
          	}
            
			MC_PwmMask_Set((u16)(gTwoDriverTab[strMCtr.gMotorDir][strMCtr.RotorSector]));
            
        }
        else if((PositionDragTime<=1))
        {
            MC_PwmMask_Set(DRIVER_OFF);
        }
        else
        {
			if(dragFlagEnd == 0)
			{
				//dragFlagEnd = 1;
				if(dragTime++>=dragTimeEnd)
				{
					dragTime = 0;
					/*rise the voltage*/
					dragDutyLoad += (dragDutyMax-dragDutyLoad)>>4;
			        if(dragDutyLoad>dragDutyMax)
			        {
						dragDutyLoad = dragDutyMax;
			        }

			        /*rise the fre*/
					if(dragTimeEnd < (3*SYS_FRE))
					{
						strMCtr.RotorSector = gNextSectorTab[strMCtr.gMotorDir][strMCtr.RotorSector];
						MC_PwmMask_Set(gTwoDriverTab[strMCtr.gMotorDir][strMCtr.RotorSector]);  
						dragFlagEnd= 1;
						strMCtr.DutyRun_Q15 = dragDutyLoad;
					}
					else
					{

						if((dragTimeEnd>>4) == 0)
							dragTimeEnd--;
						else
							dragTimeEnd = dragTimeEnd-(dragTimeEnd>>4);
						strMCtr.RotorSector = gNextSectorTab[strMCtr.gMotorDir][strMCtr.RotorSector];
						MC_PwmMask_Set(gTwoDriverTab[strMCtr.gMotorDir][strMCtr.RotorSector]);  
					}
				}

			}
			else
			{
				strMCtr.RotorSector = gNextSectorTab[strMCtr.gMotorDir][strMCtr.RotorSector];
		        MC_PwmMask_Set(gTwoDriverTab[strMCtr.gMotorDir][strMCtr.RotorSector]);  
		        Bemf_AcmpSetPosCh_F_Run();

		        strMCtr.SectorZtoZ_T0Cnt = 8000;
		        strMCtr.SectorZtoZLast_T0Cnt = 8000;

		        Timer_Record_Write(strMCtr.SectorZtoZLast_T0Cnt);

				Timer_MC_Stop();
				Timer_MC_INT_Disable();
				strMCtr.RotorState = SECTOR_WAIT_END_MASK;
				Timer_MC_Write_Start(2000);
				Timer_MC_INT_Enable();
				strMCtr.MotorState = MOTOR_RUN;    
	            PositionDrag = 0;  
			}
     
        }
    }

}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void MC_SquareWave_drag(void)
{
	
/*
思路是设定最终想要达到的强制换相转速，与拖动占空比，
提升转速过程中，换相时间越来越短，拖动占空比越来越大。
强制换相每次触发减少时间，且增大占空比。
*/
	//strMErr.ForceChangePhase_T0Cnt
	strMCtr.RotorSector = gNextSectorTab[strMCtr.gMotorDir][strMCtr.RotorSector];
	MC_PwmMask_Set(gTwoDriverTab[strMCtr.gMotorDir][strMCtr.RotorSector]);  
	Bemf_AcmpSetPosCh_F_Run();

	strMCtr.SectorZtoZ_T0Cnt = 800;
	strMCtr.SectorZtoZLast_T0Cnt = 800;

	Timer_Record_Write(strMCtr.SectorZtoZLast_T0Cnt);

	Timer_MC_Stop();
	Timer_MC_INT_Disable();
	strMCtr.RotorState = SECTOR_WAIT_END_MASK;
	Timer_MC_Write_Start(800 );
	Timer_MC_INT_Enable();
	strMCtr.MotorState = MOTOR_RUN;  
}



