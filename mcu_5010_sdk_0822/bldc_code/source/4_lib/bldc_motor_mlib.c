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
/** \file motor_mlib.c
**
**	History:
**	
*****************************************************************************/
/****************************************************************************/
/*	include files
*****************************************************************************/

//#include "motor_control.h"



#include "bldc_public_function.h"

//#include "user_config.h"
#include "bldc_public_debug.h"
#include "bldc_motor_mlib.h"
#include "bldc_motor_control.h"



uint8_t  ipdDriTab[6] = 
{
    0 ,
    3 ,
    1 ,
    4 ,
    2 ,
    5  
};
uint8_t  ipdDriTabRev[6] =   
{
    3 ,
    0 ,
    4 ,
    1 ,
    5 ,
    2  
};

uint8_t gNextSectorTab[2][6]= 
{ 	
    1, 2, 3, 4, 5, 0,
    5, 0, 1, 2, 3, 4		
};




uint8_t gLastSectorTab[2][6]= 
{ 	
	5, 0, 1, 2, 3, 4,
	1, 2, 3, 4, 5, 0
			
};//CW=0, CCW=1




uint8_t  SetCMPTabN[6] =
{
    2,	// ab- C
    3,	// ac- B
    3,	// bc- A
    1,	// ba- C
    1,	// ca- B
    2	// cb- A
};

uint8_t  SetCMPTabP[6] =
{
    1,	// ab- C
    1,	// ac- B
    2,	// bc- A
    2,	// ba- C
    3,	// ca- B
    3	// cb- A
};

#if (0) // 2026-01-07 jack add 尼得科电机
uint8_t g_Hall_NextSectorTab[2][7] =
    {
        0, 0, 2, 1, 4, 5, 3,
        0, 3, 5, 4, 1, 2, 0};

#elif(1)  // 2026-01-08 jack add 杰康美电机
uint8_t g_Hall_NextSectorTab[2][7] = { 
    0, 2, 0, 1, 4, 3, 5,
    0, 5, 3, 4, 1, 0, 2};
#elif(0)   //其他电机



#endif

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void MC_CrossZero_AcmpIRQ(void)
{

	

}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void MC_CrossZero_AcmpScan(void)
{



}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void MC_CrossZero_AdcScan(void)
{   

	
}





/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void MC_FindZero_Deal(void)
{


}




/***********************************************************************************************************************
* Function Name:
* Description  : 处理运行与滑行扇区时间,失步标志判断
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void MC_CrossZeroTime_Deal(void)
{


}


/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void MC_MaskShiftTime_Deal(void)
{
  
}


/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/


void MC_ChangePhase_Deal(void)
{
  

}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void MC_MaskTime_Write(void)
{
    /*根据屏蔽方式标志位选择屏蔽方式*/
    if(strMCtr.gCmpMaskEn == 0)
    {
        Timer_MC_Write_Start(strMCtr.MaskTime_T0Cnt);
        strMCtr.RotorState = SECTOR_WAIT_END_MASK;
    }
}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void MC_MaskStateEnd_Deal(void)
{
	uint8_t cmpPassCnt = 0;
    uint8_t readCnt = 0;
    //Debug_Signal_High();

    if(strMCtr.MotorState == MOTOR_MOTION)  // 滑行检测
    {       
        Bemf_AcmpClearIntFlag();
    }
    
    strMCtr.RotorState = SECTOR_AFTER_MASK;

	if(strMCtr.AcmpIRQEn_Flag == 1)
    {
    	/*中断使能后需要判断比较器输出是否满足过零需求*/
		
        if((strMCtr.RotorSector & 0x1) == 0x0) //fall
        {      
            readCnt = 0;
	        for(;readCnt<5;readCnt++)
	        {
				if((Bemf_AcmpGetResult()) == 0)
	            {
					cmpPassCnt++;
	            } 
	            else if(cmpPassCnt)
	            {
					cmpPassCnt--;
	            }
	        }
         
        }
        else //rise   
        {   readCnt = 0;
			for(;readCnt<5;readCnt++)
	        {
				if((Bemf_AcmpGetResult()) == 1)
	            {
					cmpPassCnt++;
	            } 
	            else if(cmpPassCnt)
	            {
					cmpPassCnt--;
	            }
	        }
        }
        
		if(cmpPassCnt>=3)
		{
			MC_FindZero_Deal();
		}
		else
		{
			Bemf_AcmpEnableInt();
		}    
        //Bemf_AcmpEnableInt();
    }
    else
    {
        Bemf_AcmpDisableInt();
    }
	
	if((strMErr.ForceChangePhaseEn)&&(cmpPassCnt<3))
	{
		Timer_MC_Write_Start(strMErr.ForceChangePhase_T0Cnt);
	}

    strMCtr.MaskOverClearEn = 1;

}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void MC_MotorState_Manage(void)
{
	Timer_MC_Stop();
    
	switch(strMCtr.RotorState)
	{
		case SECTOR_WAIT_CHANGE:/*换相*/
                MC_ChangePhase_Deal();
               /*设置屏蔽*/
                MC_MaskTime_Write();
		break;
		
		case SECTOR_WAIT_END_MASK:/*屏蔽结束*/     
				     
                MC_MaskStateEnd_Deal(); 
                /*使能过零*/ 

		break;
		
		case SECTOR_AFTER_MASK:/*过零处理或强制过零处理*/
				MC_ForceChangePhase();

                
		break;    


		case SECTOR_IN_MASK:
            /*CMP1_INT_DIS();*/	


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


uint8_t MC_IPD_6Pulse(void)
{


    return 0;
}

/***********************************************************************************************************************
* Function Name: IPD 准备函数
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
uint8_t MC_IPD_12Pulse(void)
{
  return 0;
}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void MC_PwmFreSet(uint16_t pwmFre)
{
    
    if(pwmFre == strMCtr.LowFreData)
    {
        strMCtr.LoadFrePeriod  = strMCtr.LowFrePeriod;
    }
    else   if(pwmFre == strMCtr.HighFreData)
    {
        strMCtr.LoadFrePeriod  = strMCtr.HighFrePeriod;
    }
    
    strMCtr.LoadFreData = pwmFre; 
    
    Pwm_SetPeriodAndTGD();
/*
	EPWM->LOCK = EPWM_LOCK_P1B_WRITE_KEY;
    EPWM->PERIOD[4] = 0xffff & strMCtr.LoadFrePeriod;
    EPWM->PERIOD[2] = 0xffff & strMCtr.LoadFrePeriod;
    EPWM->PERIOD[0] = 0xffff & strMCtr.LoadFrePeriod;

    EPWM->PERIOD[5] = 0xffff & strMCtr.LoadFrePeriod;
    EPWM->PERIOD[3] = 0xffff & strMCtr.LoadFrePeriod;
    EPWM->PERIOD[1] = 0xffff & strMCtr.LoadFrePeriod;

	//TRG ADC1	可以直接在adc1里面考虑pwm触发设置,1<<19为向上计数

    EPWM->CMPTGD[0] = (0<<19)|(strMCtr.LoadFrePeriod - 4);
    EPWM->CMPTGD[1] = (1<<19)|(4); 
	EPWM->LOCK = 0x0;
*/
}


/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments	 : 
* Return Value : 
***********************************************************************************************************************/

void MC_PwmDutyWrite(u16   uiDutyTemp)
{   
    unsigned int  uiDutyTemp1;
    static uint8_t  pwm2ndcnt=0;
    static uint8_t  pwm3rdcnt=0;	




    if(uiDutyTemp > strMCtr.DutyLimitMax_Q15)
    {
        uiDutyTemp = strMCtr.DutyLimitMax_Q15;
    }
    
    if(uiDutyTemp < strMCtr.DutyLimitMin_Q15)
    {
        uiDutyTemp = strMCtr.DutyLimitMin_Q15;
    }

    strMCtr.LowFreDutyRun = uiDutyTemp*strMCtr.LowFreDutyMax>>15;

    if(strMCtr.LowFreDutyRun >strMCtr.LowFreDutyMax)
    {
        strMCtr.LowFreDutyRun = strMCtr.LowFreDutyMax;
    }
    else if(strMCtr.LowFreDutyRun <strMCtr.LowFreDutyMin )
    {
        strMCtr.LowFreDutyRun = strMCtr.LowFreDutyMin;
    }

    if(strMCtr.LoadFreData == strMCtr.HighFreData)
    {
        if(strMCtr.LowFreDutyRun < strMCtr.LowFreFallFreDuty)  //切换低频
        {
            if(++pwm3rdcnt>=3)
            {
                pwm3rdcnt = 0;
                strMCtr.SetFreData = strMCtr.LowFreData;
            }
        }
        else  
        {
            pwm3rdcnt = 0;
        }
    }
    else
    {
        if(strMCtr.LowFreDutyRun > strMCtr.LowFreRiseFreDuty)  //切换高频
        {
            if(++pwm2ndcnt>= 3)
            {
                pwm2ndcnt=0;
                strMCtr.SetFreData = strMCtr.HighFreData;
            }
        }
        else
        {
            pwm2ndcnt=0;
        }
    }


    if(strMCtr.SetFreData  != strMCtr.SetFreDataLast)
    {
        MC_PwmFreSet(strMCtr.SetFreData);
        strMCtr.SetFreDataLast = strMCtr.SetFreData;
    }


    strMCtr.LoadFreDuty = uiDutyTemp*strMCtr.LoadFrePeriod>>15;

    if(strMCtr.LoadFreDuty >strMCtr.LoadFrePeriod)
    {
        strMCtr.LoadFreDuty = strMCtr.LoadFrePeriod;
    }
    else if(strMCtr.LoadFreDuty <strMCtr.LowFreDutyMin )
    {
        strMCtr.LoadFreDuty = strMCtr.LowFreDutyMin;
    }
    

    uiDutyTemp1 =strMCtr.LoadFrePeriod - strMCtr.LoadFreDuty;   
    Pwm_SetDuty(uiDutyTemp1);
  /*  
	EPWM->LOCK = EPWM_LOCK_P1B_WRITE_KEY;
    EPWM->CMPADT[0]=(0xffff&(uiDutyTemp1)); 
    EPWM->CMPADT[2]=(0xffff&(uiDutyTemp1)); 
    EPWM->CMPADT[4]=(0xffff&(uiDutyTemp1)); 

    EPWM->CMPADT[1]=(0xffff&(uiDutyTemp1)); 
    EPWM->CMPADT[3]=(0xffff&(uiDutyTemp1)); 
    EPWM->CMPADT[5]=(0xffff&(uiDutyTemp1)); 

    EPWM->CON3=0x3F00;
    EPWM->LOCK = 0x0;
*/
}
