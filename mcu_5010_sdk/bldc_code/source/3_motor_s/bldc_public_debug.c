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
/** \file user_debug.c
**
**	History:
**	
*****************************************************************************/
/****************************************************************************/
/*	include files
*****************************************************************************/

#include "bldc_public_debug.h"
#include "bldc_public_function.h"
#include "bldc_motor_control.h"
#include "bldc_motor_mlib.h"

#include "bldc_user_control.h"
//#include "user_config.h"
//#include "user_bsp.h"





unsigned int ucChkDirTB[11] = 
{
	DRIVE_A_B,		//	0 0-	// 送AB - 检测A
	DRIVE_B_C,		//	1 2-	// 送BC		
	DRIVE_C_A,		//	2 4-	// 送CA		
	DRIVER_ABCL,
	DRIVER_ABCH,
	DRIVER_AL,             //5
	DRIVER_AH,			 //8
	DRIVER_BL,            //6
	DRIVER_BH,			 //9
	DRIVER_CL,            //7
	DRIVER_CH,			 //10	
};


/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void  Delay_1US_Debug(unsigned int timer) 
{

	while(timer)
	{
		timer--;
	}
}


/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void Debug_Epwm_HuBuShuChu(void)
{
    MC_PwmDutyWrite(16384); 
    MC_PwmMask_Set(0);
    //EPWM_Start(0x3F);
    while(1)
    {
        __disable_irq(); /*全局中断使能*/
        
    }
}



/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void Debug_Epwm_DingShiHuanXiang(void)
{
    uint16_t ucDriver = 0;
    uint16_t testSector = 0;
    MC_PwmDutyWrite(0); 
	//EPWM_Start(0x3F);

    while(1)
    {
        // __disable_irq(); /*全局中断禁止*/
        Delay_MSx(10);

        testSector = gNextSectorTab[0][testSector];
        ucDriver=gTwoDriverTab[0][testSector];
        MC_PwmMask_Set(ucDriver);
    }

}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/


void Debug_Delay_Check(void)
{
	uint16_t delayCount = 10;
    __disable_irq(); /*全局中断使能*/
    //SYS_SET_IOCFG(IOP35CFG,0);
    //GPIO_CONFIG_IO_MODE(GPIO3,GPIO_PIN_5,GPIO_MODE_OUTPUT);//19


    while(1)
    {
        //GPIO3->DO_f.P5 = 1;
        //Delay_USx(delayCount);
        Delay_MSx(delayCount);
        //GPIO3->DO_f.P5 = 0;
    }
}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void Debug_IPD_Check(void)
{
	uint8_t ret = 0;
	MC_PwmDutyWrite(0); 


	while(1)
	{
		__disable_irq(); /*全局中断禁止*/
		Delay_MSx(1500);
		strMStart.IPD.CheckDataLast[0] = strMStart.IPD.CheckData[0];
		strMStart.IPD.CheckDataLast[1] = strMStart.IPD.CheckData[1];
		strMStart.IPD.CheckDataLast[2] = strMStart.IPD.CheckData[2];
		strMStart.IPD.CheckDataLast[3] = strMStart.IPD.CheckData[3];
		strMStart.IPD.CheckDataLast[4] = strMStart.IPD.CheckData[4];
		strMStart.IPD.CheckDataLast[5] = strMStart.IPD.CheckData[5];
		//ret = MC_IPD_12Pulse();
		ret = MC_IPD_6Pulse();
		ret = ret;
	}


}


/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void Debug_Manage(void)
{
#if(MOTOR_TEST_MODE == MOTOR_MODE_TEST_DRIVE_HB)
        Debug_Epwm_HuBuShuChu();
#elif(MOTOR_TEST_MODE == MOTOR_MODE_TEST_DRIVE_HX)
        Debug_Epwm_DingShiHuanXiang();
#elif(MOTOR_TEST_MODE == MOTOR_MODE_TEST_IPD)
        Debug_IPD_Check();
#elif(MOTOR_TEST_MODE == MOTOR_MODE_TEST_DELAY)
        Debug_Delay_Check();
#endif
}
/***********************************************************************************************************************
* Function Name:
* Description  : 用于测试启停
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
u16 g_time_on = 2000;
u16 g_time_off = 2000;
u16 g_start_cnt = 0;
u16 g_restart_cnt = 0;
u16 g_Success_cnt = 0;

void Debug_ToolStartMotor(void)
{
    static u16 cnt = 0;
   
    if(strUCtr.flag.bits.gToolEn == 1)
    {
        if(cnt++ >= g_time_on)
        {
            strUCtr.flag.bits.gToolEn = 0;
            cnt = 0;
            g_time_on+=30;
            
            if(g_time_on>1500)
            {
                g_time_on=200;
            }
        }
    }
    else
    {
        if(cnt++ >= g_time_off)
        {
            if((strUErr.userErr.word == 0)&&(strMErr.motorErr.word == 0))
            {
                strUCtr.flag.bits.gToolEn = 1;
                g_Success_cnt++;
                g_start_cnt ++ ;
                cnt = 0;
                g_time_off+=20;
                if(g_time_off>1000)
                {
                    g_time_off=20;
                }
                
                strUCtr.AimSet_Q15 +=500;/*test*/
    
                if(strUCtr.AimSet_Q15>20000)
                {
                    strUCtr.AimSet_Q15 = 3300;
                }
            }
        }
    }

}


/***********************************************************************************************************************
* Function Name:
* Description  : 用于测试屏蔽结束位置
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void Debug_Signal_High(void)
{
#if TEST_SIGNAL1_EN
    //TEST_SIGNAL1_HIGH;
#endif
}
/***********************************************************************************************************************
* Function Name:
* Description  : 用于测试过零位置
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void Debug_Signal_Low(void)
{
#if TEST_SIGNAL1_EN
    //TEST_SIGNAL1_LOW;
#endif
}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void UC_CheckMos(void)
{
	uint8_t mosCheckCnt = 0;
	
	MC_PwmMask_Set(DRIVER_OFF);
	
	Delay_MSx(1);	    

	
	#if(1)	
	//MOS短路检测

	mosCheckCnt = UC_CheckSignlaDriver();
	
	if(mosCheckCnt > 0)  
	{
		// 5 7 9 开下管，测试上管，异常后报上管异常
		if((mosCheckCnt&0x01) == 1)
		{

				strUErr.userErr.bits.gMosBridgeH = 1;
			
		}
		else // 6 8 10 开上管，测试下管，异常后报下管异常
		{

				strUErr.userErr.bits.gMosBridgeL = 1;
		}
		  
		return;
	}        

	Delay_MSx(1);

	#endif

}




/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
uint8_t UC_CheckSignlaDriver(void)
{

#if(IBus_Peak_Protect_En)

    uint8_t i,j,idriver;
    uint16_t short_current_adc;

    static uint8_t cntOp = 0;

    idriver = 5;

    short_current_adc = strInput.IBusPeakOffset_Adc+ IBus_Ax100ConvertToAdc(strInput.IBusMosCheck_Ax100);

    for(j=idriver;j<11;j++)
    {
        MC_PwmMask_Set(ucChkDirTB[j]);
        Delay_USx(2);
        for(i=0;i<20;i++)	
        {

            ADCH_Convert_Wait_End(ADC1_IBusPeak_Convert);
            strInput.IBusPeak_Adc = ADCH_GetResult(ADC1_IBusPeak_Result);	

            if(MC_PWM_BrakeRead())
            {
                MC_PwmMask_Set(DRIVER_OFF);
                return j;
            }

            if((strInput.IBusPeak_Adc >= short_current_adc)  //多次判断
            ||(MC_PWM_BrakeRead()))
            {
                if(++cntOp > 3)
                {
                    MC_PwmMask_Set(DRIVER_OFF);
                    return j;
                }
            }
            else
            {
                cntOp = 0;
            }
        }
        if(j&0x01)
        {
            Delay_MSx(2);
        }
        else
        {
            Delay_USx(5);
        }

        MC_PwmMask_Set(DRIVER_OFF);
        Delay_USx(50);
    }

    MC_PwmMask_Set(DRIVER_OFF);
	#endif
	
    return 0;

}


/***********************************************************************************************************************
* Function Name:
* Description  : 用于上电自检所有内容
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void HW_CheckSelf_Handler(void)
{



 

}







