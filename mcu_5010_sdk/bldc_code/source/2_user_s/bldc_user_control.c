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
/** \file user_control.c
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


#include "bldc_public_function.h"
#include "bldc_user_control.h"
#include "bldc_user_config.h"
#include "bldc_public_debug.h"

#include "bldc_user_bsp.h"
//#include "bldc_user_input.h"
//#include "bldc_user_output.h"
#include "bldc_main.h"
//#include "bldc_user_fmc.h"




User_Input_t		strInput;
User_Control_t    	strUCtr;
User_Output_t     	strOutput;





/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void UC_Variable_Init(void)
{


/*IBus Parameter Init*/

    strInput.IBusPeakOffset_Adc= 0;
    strInput.IBusAvgOffset_Adc= 0;
    strInput.IBusOffsetRef_Adc = (uint32_t)strHW.IBusOffsetRef_Vx100*strHW.AdcMax_Adc/strHW.AdcRef_Vx100;

    strInput.IBusAvgCh_Adc= 0;
    strInput.IBusAvg_Adc = 0;
    strInput.IBusAvg_Ax100= 0;
    strInput.IBusAvg_Ax100Filter= 0;
    strInput.IBusAvgLimEn_Flag = IBus_Avg_Lim_En;
    strInput.IBusAvgLim_Ax100 = IBus_Avg_Lim_A*100;
    strInput.IBusAvgLim_Adc = IBus_Ax100ConvertToAdc(strInput.IBusAvgLim_Ax100);

    strInput.IBusPeakCh_Adc= 0;
    strInput.IBusPeak_Ax100= 0;
    strInput.IBusPeak_Adc= 0;
    strInput.IBusPeakLimEn_Flag = IBus_Peak_Lim_En;
    strInput.IBusPeakLim_Ax100 = IBus_Peak_Lim_A*100;
    strInput.IBusPeakLim_Adc = IBus_Ax100ConvertToAdc(strInput.IBusPeakLim_Ax100);


/*VBus Parameter Init*/
    strInput.VBus_Vx100= 0;
    strInput.VBus_Vx100Filter= 0;
    strInput.VBus_Adc= 0;
    strInput.VBus_AdcFilter= 0; 
    
    strInput.VPhaseMax_Adc = 0;
    strInput.VPhaseMaxTemp_Adc = 0;
/*NTC Parameter Init*/
    strInput.NtcMos_Adc= 0;
    strInput.NtcBat_Adc= 0;

/*Trigger Parameter Init*/
    strInput.Trigger_Adc= 0;
    strInput.Trigger_Q15 = 0;
    


    strInput.TriggerOn_Q15 = Tool_Start_Percent;
    strInput.TriggerOff_Q15 = Tool_Stop_Percent;

/*SW Parameter Init*/
    strInput.SwDir_Flag = 0;
    strInput.SwDirCWValue = Tool_Dir_CW_Value;
    strInput.SwDirCCWValue = Tool_Dir_CCW_Value;
    strInput.SwWrench_Flag = 0;



    #if 1
    /*电流保护参数设置**********************************************/

    strInput.gIBusAvgProtectEn = IBus_Avg_Protect_En;
    strInput.gIBusPeakProtectEn = IBus_Peak_Protect_En;
    strInput.IBusAvgOCP1_Cnt = 0;
    strInput.IBusAvgOCP2_Cnt = 0;
    strInput.IBusAvgOCP3_Cnt = 0;
    strInput.IBusPeakOCP1_Cnt = 0;

    strInput.IBusAvgOCP1_CntSet = IBus_Avg_OCP1_FiltMS*((float)strHW.Fre_USM/strHW.SToMS);
    strInput.IBusAvgOCP2_CntSet = IBus_Avg_OCP2_FiltMS*((float)strHW.Fre_USM/strHW.SToMS);
    strInput.IBusAvgOCP3_CntSet = IBus_Avg_OCP3_FIltMS*((float)strHW.Fre_USM/strHW.SToMS);
    strInput.IBusPeakOCP1_CntSet = IBus_Peak_OCP1_FiltMS*((float)strHW.Fre_USM/strHW.SToMS);

    strInput.IBusAvgOCP1_Ax100 = (IBus_Avg_OCP1_A*100);
    strInput.IBusAvgOCP2_Ax100 = (IBus_Avg_OCP2_A*100);
    strInput.IBusAvgOCP3_Ax100 = (IBus_Avg_OCP3_A*100);
    strInput.IBusPeakOCP1_Ax100 = (IBus_Peak_OCP1_A*100);
    strInput.IBusMosCheck_Ax100 = (IBus_Peak_CheckMos_A*100);

    strInput.IBusAvgOCP1_Adc = IBus_Ax100ConvertToAdc(strInput.IBusAvgOCP1_Ax100);
    strInput.IBusAvgOCP2_Adc = IBus_Ax100ConvertToAdc(strInput.IBusAvgOCP2_Ax100);
    strInput.IBusAvgOCP3_Adc = IBus_Ax100ConvertToAdc(strInput.IBusAvgOCP3_Ax100);
    strInput.IBusPeakOCP1_Adc = IBus_Ax100ConvertToAdc(strInput.IBusPeakOCP1_Ax100);
#endif


#if 1
    /*电压保护参数设置**********************************************/
    strInput.gVBusProtectEn = VBus_Protect_En;

    strInput.VBusRunUVP1_Cnt = 0;
    strInput.VBusRunUVP2_Cnt = 0;
    strInput.VBusStbUVP_Cnt = 0;
    strInput.VBusOVP_Cnt = 0;

    strInput.VBusRunUVP1_CntSet = VBus_Run_UVP1_FiltMS*((float)strHW.Fre_USM/strHW.SToMS);
    strInput.VBusRunUVP2_CntSet = VBus_Run_UVP2_FiltMS*((float)strHW.Fre_USM/strHW.SToMS);
    strInput.VBusStbUVP_CntSet = VBus_Stb_UVP_FiltMS*((float)strHW.Fre_USM/strHW.SToMS);
    strInput.VBusOVP_CntSet = VBus_OVP_FiltMS*((float)strHW.Fre_USM/strHW.SToMS);
    
    strInput.VBusRunUVP1_Vx100 =VBus_Run_UVP1_V*100;
    strInput.VBusRunUVP2_Vx100 = VBus_Run_UVP2_V*100;
    strInput.VBusStbUVP_Vx100 = VBus_Stb_UVP_V*100;
    strInput.VBusOVP_Vx100 = VBus_OVP_V*100;

    strInput.VBusRunUVP1_Adc =VBus_Vx100ConvertToAdc(strInput.VBusRunUVP1_Vx100);
    strInput.VBusRunUVP2_Adc = VBus_Vx100ConvertToAdc(strInput.VBusRunUVP2_Vx100);
    strInput.VBusStbUVP_Adc = VBus_Vx100ConvertToAdc(strInput.VBusStbUVP_Vx100);
    strInput.VBusOVP_Adc = VBus_Vx100ConvertToAdc(strInput.VBusOVP_Vx100);
#endif


#if 1
    /*温度保护参数设置**********************************************/


    strInput.gNtcMosProtectEn = Ntc_Mos_Protect_En;
    strInput.gNtcBatProtectEn = Ntc_Bat_Protect_En;

    strInput.NtcMosErr_Cnt= 0;
    strInput.NtcBatErr_Cnt= 0;

    strInput.NtcMosErrClr_Cnt= 0;
    strInput.NtcBatErrClr_Cnt= 0;

    strInput.NtcMosErr_CntSet= Ntc_Mos_Err_SetMS*((float)strHW.Fre_USM/strHW.SToMS);
    strInput.NtcMosErrClr_CntSet= Ntc_Mos_ErrClr_SetMS*((float)strHW.Fre_USM/strHW.SToMS);

    strInput.NtcBatErr_CntSet= Ntc_Bat_Err_SetMS*((float)strHW.Fre_USM/strHW.SToMS);       
    strInput.NtcBatErrClr_CntSet= Ntc_Bat_ErrClr_SetMS*((float)strHW.Fre_USM/strHW.SToMS);

    strInput.NtcMosOTP_Res =  Ntc_Mos_OTP_Res;
    strInput.NtcMosOTPClr_Res = Ntc_Mos_OTPClr_Res;
    strInput.NtcMosUTP_Res = Ntc_Mos_UTP_Res;
    strInput.NtcMosUTPClr_Res = Ntc_Mos_UTPClr_Res;
    strInput.NtcMosRef_Res = Ntc_Mos_Ref_Res;

    strInput.NtcBatOTP_Res= Ntc_Bat_OTP_Res;
    strInput.NtcBatOTPClr_Res = Ntc_Bat_OTPClr_Res;
    strInput.NtcBatUTP_Res= Ntc_Bat_UTP_Res;
    strInput.NtcBatUTPClr_Res= Ntc_Bat_UTPClr_Res;
    strInput.NtcBatRef_Res = Ntc_Bat_Ref_Res;

    strInput.NtcMosOTP_Adc =  Ntc_ResConvertToAdc(strInput.NtcMosOTP_Res,strInput.NtcMosRef_Res);
    strInput.NtcMosOTPClr_Adc = Ntc_ResConvertToAdc(strInput.NtcMosOTPClr_Res,strInput.NtcMosRef_Res);
    strInput.NtcMosUTP_Adc = Ntc_ResConvertToAdc(strInput.NtcMosUTP_Res,strInput.NtcMosRef_Res);
    strInput.NtcMosUTPClr_Adc = Ntc_ResConvertToAdc(strInput.NtcMosUTPClr_Res,strInput.NtcMosRef_Res);

    strInput.NtcBatOTP_Adc= Ntc_ResConvertToAdc(strInput.NtcBatOTP_Res,strInput.NtcBatRef_Res);
    strInput.NtcBatOTPClr_Adc= Ntc_ResConvertToAdc(strInput.NtcBatOTPClr_Res,strInput.NtcBatRef_Res);
    strInput.NtcBatUTP_Adc= Ntc_ResConvertToAdc(strInput.NtcBatUTP_Res,strInput.NtcBatRef_Res);
    strInput.NtcBatUTPClr_Adc= Ntc_ResConvertToAdc(strInput.NtcBatUTPClr_Res,strInput.NtcBatRef_Res);

#endif


    strUCtr.ToolState_Cnt = 0;
    strUCtr.SleepDelay_CntSet = Tool_Sleep_DelayTime;
    strUCtr.ToolState = TOOL_STOP;
    
    strUCtr.flag.bits.gToolDir = 0;
    strUCtr.flag.bits.gToolTrigger = 0;
    strUCtr.flag.bits.gToolEn = 0;

    strUCtr.ToolGear = 0;
    strUCtr.MotorState = MOTOR_STOP;
    
   
    /************调速******************/
    

    strUCtr.TriggerMin_Q15 = Tool_SpeedUp_Min_Percent;/*扳机ad值的使用下限*/
    strUCtr.TriggerMax_Q15 = Tool_SpeedUp_Max_Percent;/*扳机ad值的使用上限*/
    
    strUCtr.RateMin_Q15 = Tool_RateOfTrigger_Min_Q15;/*扳机表达占比下限*/
    strUCtr.RateMax_Q15 = Tool_RateOfTrigger_Max_Q15;/*扳机表达占比上限*/

    strUCtr.RateDivTrigger_Q15 =  (uint32_t)(((float)strUCtr.RateMax_Q15-strUCtr.RateMin_Q15)*32768/(strUCtr.TriggerMax_Q15-strUCtr.TriggerMin_Q15)); 


	strUCtr.RateSet_Q15 = 0;
    strUCtr.AimSet_Q15 = 0;
#if 0 //jack

    strUCtr.flag.bits.gAutoStopEn = (strFMC.dataRead[UC_INFO_ADRESS]&0x01);
    strUCtr.ToolGear = (strFMC.dataRead[UC_INFO_ADRESS]>>1)&0x03;

	strOutput.LedDeal = 0;
	strOutput.LedRunCnt_Once = 0;
	strOutput.LedRunTimes = 1;
	strOutput.gSocDislay = 1;
    strOutput.gSocUpdate = 1;

	
	FMC_Read_Array(8);  

#endif //jack
    
}


void UC_ErrData_Update(void)
{
	strUErr.userErr.word|= strInput.userErr.word;
	strMErr.motorErr.word|= strMCtr.motorErr.word;
}

void UC_StateMachine_Handler(void)
{    
    switch (strUCtr.ToolState)
    {
        case TOOL_STOP:		

            strUCtr.ToolState_Cnt++;

            if(strUCtr.flag.bits.gAutoStop == 1)
            {
                if(strUCtr.flag.bits.gToolTrigger  == 0)
                {
                    strUCtr.flag.bits.gAutoStop = 0;
                }
            }
            else if((strUCtr.flag.bits.gToolTrigger  == 1)
            )
            {                      
                strUCtr.ToolState_Cnt = 0;
                strUCtr.ToolState = TOOL_READY;
            } 
            else
            {
                if(strUCtr.ToolState_Cnt>strUCtr.SleepDelay_CntSet)
                {
                    //strUCtr.gToolPowerDown = 1;
                    strUCtr.ToolState = TOOL_POWER_DOWN;  
                }
            }

            if((strUErr.userErr.word != 0)||(strMErr.motorErr.word != 0))
            {
                strUCtr.ToolState = TOOL_ERROR_STOP;  
            }
            
            strUCtr.flag.bits.gToolEn = 0;  /**/


        break;

        case TOOL_READY:  	

            /*初始化用户状态机变量*/
            
            strUCtr.ToolState_Cnt = 0;

            strUCtr.flag.bits.gToolEn = 1;    /**/
        	strUCtr.flag.bits.gToolDir = strInput.SwDir_Flag;
            strUCtr.ToolState = TOOL_RUN;   
        
        break;

        case TOOL_RUN:  	    
        
            if((strUCtr.flag.bits.gToolTrigger == 0)
            ||(strUCtr.flag.bits.gLLOLock == 1)
            ||(strUCtr.flag.bits.gVRLock == 1)
            )
            {          
                strUCtr.flag.bits.gToolEn = 0;   /**/
                strUCtr.ToolState_Cnt = 0;
                
                strUCtr.ToolState = TOOL_PRESTOP;    
            }     
            else if(strUCtr.flag.bits.gAutoStop == 1)
            {
                strUCtr.flag.bits.gToolEn = 0;   /**/
                strUCtr.ToolState_Cnt = 0;
                
                strUCtr.ToolState = TOOL_PRESTOP;    
            }
            
            if((strUErr.userErr.word != 0)||(strMErr.motorErr.word != 0))
            {
                strUCtr.ToolState = TOOL_ERROR_STOP;  
            }

            /*扳机输入转换为限制最大最小比值*/
            
            /*比值乘以设置最大值可以得到扳机对应的设置值，可以是转速可以是占空比*/

            strUCtr.RateSet_Q15 = UC_GetRateFromTrigger(strInput.Trigger_Q15);  /**/

   			if(1)/*调速受扳机变化改变*/
				strUCtr.AimSet_Q15 = strUCtr.RateSet_Q15;
			else/*如果调速恒定，则赋值定值*/
				strUCtr.AimSet_Q15 = 32768;
			

        break;

        case TOOL_STOPPING:


        break;
        

        case TOOL_PRESTOP:

            strUCtr.ToolState = TOOL_STOP;        

        break;       
        
        
        case  TOOL_ERROR_STOP:		
          
            strUCtr.ToolState_Cnt++;

        
            if(strUCtr.flag.bits.gToolTrigger  == 0)
            {
                /*清除需要恢复的异常*/
                if(strInput.userErr.word == 0)
                {
                	/*当可恢复异常电流电压温度恢复后，可根据需求清除异常*/
                    strUErr.userErr.word &= ~0x1FF;
                    /*电机异常根据需求确定是否恢复*/

                    strMCtr.motorErr.word = 0;
					strMErr.motorErr.word = 0;
                }

                
                if((strUErr.userErr.word != 0)||(strMErr.motorErr.word != 0))
                {
                    if(strUCtr.ToolState_Cnt> strUCtr.SleepDelay_CntSet)   /*松开扳机还有异常维持Tool_Sleep_DelayTime后掉电*/
                    {
                        //strUCtr.gToolPowerDown = 1;                             
                        strUCtr.ToolState_Cnt = 0;
                        
                        strUCtr.ToolState = TOOL_POWER_DOWN;
                    }
                }
                else
                {
                    strUCtr.ToolState_Cnt = 0;
                    strUCtr.ToolState = TOOL_PRESTOP;  
                }
            }
            else
            {
                strUCtr.ToolState_Cnt = 0;
            }


        break;

        case TOOL_POWER_DOWN:    
        
            //strUCtr.gToolPowerDown = 0;

#if(1)        
            if(strUCtr.flag.bits.gToolTrigger  == 1)
            {
                //PowerLock_ON; //jack
                if((strUErr.userErr.word == 0)
                &&(strMErr.motorErr.word == 0)
                )
                {
                    strUCtr.ToolState = TOOL_STOP;
                }
                else
                {
                    strUCtr.ToolState = TOOL_ERROR_STOP;
                }
            }
            else
            {
                //FMC_Write_Array(2);
                //PowerLock_OFF;  //jack    
            }
#endif



        break;

        default:
        break;
    }



    

}


/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
uint32_t  UC_GetRateFromTrigger(int32_t itriggerAdcNow_Q15)
{
    int32_t rateNow_Q15;

    if(itriggerAdcNow_Q15 <= strUCtr.TriggerMin_Q15)
    {
        itriggerAdcNow_Q15 = strUCtr.TriggerMin_Q15;
        
        rateNow_Q15 = strUCtr.RateMin_Q15;
    }
    else if(itriggerAdcNow_Q15 >= strUCtr.TriggerMax_Q15)
    {
        itriggerAdcNow_Q15 = strUCtr.TriggerMax_Q15;
        
        rateNow_Q15 = strUCtr.RateMax_Q15;
    }
    else
    {
        rateNow_Q15 = (int32_t)(((int64_t)strUCtr.RateDivTrigger_Q15 * (itriggerAdcNow_Q15 -strUCtr.TriggerMin_Q15)) >> 15) + strUCtr.RateMin_Q15;

        if(rateNow_Q15 > strUCtr.RateMax_Q15)
        {
            rateNow_Q15 = strUCtr.RateMax_Q15;
        }
        else if(rateNow_Q15 < strUCtr.RateMin_Q15)
        {
			rateNow_Q15 = strUCtr.RateMin_Q15;
        }
    }

    return rateNow_Q15;    
}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void  UC_MC_Update( void)
{
/*上传*/
    strMCtr.gMotorEn = strUCtr.flag.bits.gToolEn;
    if(strMCtr.MotorState == MOTOR_STOP)
    {
        strMCtr.gMotorDir = strUCtr.flag.bits.gToolDir;
    }

    strMCtr.AimSet_Q15 = strUCtr.AimSet_Q15;

}


/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void UC_GetIBusOffset(void)
{
#if 0  //jack
	uint8_t i;
	uint16_t sumAvg = 0;
	uint16_t sumPeak = 0;


	 __disable_irq()/*全局中断禁止*/
	
	for(i=0;i<16;i++)
	{
		ADCH_Convert_Wait_End(ADC1_IBusPeak_Convert|ADC1_IBusAvg_Convert);
		sumPeak += ADCH_GetResult(ADC1_IBusPeak_Result); //电流峰值

		sumAvg +=  ADCH_GetResult(ADC1_IBusAvg_Result); //电流平均值
	}
	
	strInput.IBusPeakOffset_Adc = (sumPeak >> 4);
	strInput.IBusAvgOffset_Adc = (sumAvg >> 4);	

	if(
	  (strInput.IBusPeakOffset_Adc < (strInput.IBusOffsetRef_Adc>>1))
	||(strInput.IBusPeakOffset_Adc > (strInput.IBusOffsetRef_Adc + (strInput.IBusOffsetRef_Adc>>1)))
	||(strInput.IBusAvgOffset_Adc < (strInput.IBusOffsetRef_Adc>>1))
	||(strInput.IBusAvgOffset_Adc > (strInput.IBusOffsetRef_Adc + (strInput.IBusOffsetRef_Adc>>1)))
	)	
	{
			
			strUErr.userErr.bits.gIBusOffset = 1;	
	}
    

#endif 
}


