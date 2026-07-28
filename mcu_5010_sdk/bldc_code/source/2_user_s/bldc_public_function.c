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
#include "bldc_public_function.h"
//#include "user_control.h"
//#include "user_config.h"
//#include "user_bsp.h"

//#include "acmp.h"
//#include "adc1.h"
//#include "adc0.h"
//#include "epwm.h"
HW_Para_t 			strHW;
User_Error_t 		strUErr;
Motor_Error_t 		strMErr;



uint16_t gTwoDriverTab[2][6] =  
{	
    DRIVE_A_B, // 驱动AB读取C相，CB变为AC，C从高下降到低 
    DRIVE_A_C, // 驱动AC读取B相，AB变为BC，B从低上升到高 
    DRIVE_B_C, // 驱动BC读取A相，AC变为BA，A从高下降到低 
    DRIVE_B_A, // 驱动BA读取C相，BC变为CA，C从低上升到高  
    DRIVE_C_A, // 驱动CA读取B相，BA变为CB，B从高下降到低 
    DRIVE_C_B, // 驱动CB读取A相，CA变为AB，A从低上升到高 

    DRIVE_B_A, // 
    DRIVE_C_A, 
    DRIVE_C_B, 
    DRIVE_A_B, 
    DRIVE_A_C, 
    DRIVE_B_C	
};

uint16_t gTwoDriverReTab[2][6] =  
{	
    DRIVE_Adown_pwm_Bup_mask, // 驱动AB读取C相，CB变为AC，C从高下降到低 
    DRIVE_Adown_pwm_Cup_mask, // 驱动AC读取B相，AB变为BC，B从低上升到高 
    DRIVE_Bdown_pwm_Cup_mask, // 驱动BC读取A相，AC变为BA，A从高下降到低 
    DRIVE_Bdown_pwm_Aup_mask, // 驱动BA读取C相，BC变为CA，C从低上升到高  
    DRIVE_Cdown_pwm_Aup_mask, // 驱动CA读取B相，BA变为CB，B从高下降到低 
    DRIVE_Cdown_pwm_Bup_mask, // 驱动CB读取A相，CA变为AB，A从低上升到高 

    DRIVE_Bdown_pwm_Aup_mask, // 驱动BA读取C相，BC变为CA，C从低上升到高  
    DRIVE_Cdown_pwm_Aup_mask, // 驱动CA读取B相，BA变为CB，B从高下降到低 
    DRIVE_Cdown_pwm_Bup_mask, // 驱动CB读取A相，CA变为AB，A从低上升到高 
    DRIVE_Adown_pwm_Bup_mask, // 驱动AB读取C相，CB变为AC，C从高下降到低 
    DRIVE_Adown_pwm_Cup_mask, // 驱动AC读取B相，AB变为BC，B从低上升到高 
    DRIVE_Bdown_pwm_Cup_mask, // 驱动BC读取A相，AC变为BA，A从高下降到低 
};


uint16_t ipdDriveTabDr3[6] = 
{	
    PDRIVE_C_AB, // 0and1 240d
    PDRIVE_CA_B, // 1and2 300d
    PDRIVE_A_BC, // 2and3 0d
    PDRIVE_AB_C, // 3and4 60d
    PDRIVE_B_CA, // 4and5 120d
    PDRIVE_BC_A  // 5and6 180d
};

uint16_t ipdDriveTabDr2[6] = 
{	
    PDRIVE_C_A, // 0 210d
    PDRIVE_C_B, // 1 270d
    PDRIVE_A_B, // 2 330d
    PDRIVE_A_C, // 3 30d
    PDRIVE_B_C, // 4 90d
    PDRIVE_B_A  // 5 150d
};


/*比较器输入选择*/

uint8_t  SetCMPTabF[6] =
{
//    BEMF_W,	// ab- C
//    BEMF_V,	// ac- B
//    BEMF_U,	// bc- A
//    BEMF_W,	// ba- C
//    BEMF_V,	// ca- B
//    BEMF_U	// cb- A
};


uint8_t GetADCTabP[6] =
{
//    ADC_U_Result,	// ab- C
//    ADC_U_Result,	// ac- B
//    ADC_V_Result,	// bc- A
//    ADC_V_Result,	// ba- C
//    ADC_W_Result,	// ca- B
//    ADC_W_Result,	// cb- A
};


uint8_t GetADCTabN[6] =
{
//    ADC_V_Result,	// ab- C
//    ADC_W_Result,	// ac- B
//    ADC_W_Result,	// bc- A
//    ADC_U_Result,	// ba- C
//    ADC_U_Result,	// ca- B
//    ADC_V_Result,	// cb- A
};

uint8_t  GetADCTabF[6] =
{
//    ADC_W_Result,	// ab- C
//    ADC_V_Result,	// ac- B
//    ADC_U_Result,	// bc- A
//    ADC_W_Result,	// ba- C
//    ADC_V_Result,	// ca- B
//    ADC_U_Result	// cb- A
};

uint16_t SetADCTabF[6] =
{
//    ADC_W_Convert,	// ab- C
//    ADC_V_Convert,	// ac- B
//    ADC_U_Convert,	// bc- A
//    ADC_W_Convert,	// ba- C
//    ADC_V_Convert,	// ca- B
//    ADC_U_Convert	// cb- A
};

uint16_t SetADCTabN[6] =
{
//    ADC_V_Convert,	// ab- C
//    ADC_W_Convert,	// ac- B
//    ADC_W_Convert,	// bc- A
//    ADC_U_Convert,	// ba- C
//    ADC_U_Convert,	// ca- B
//    ADC_V_Convert	// cb- A
};

uint16_t SetADCTabP[6] =
{
//    ADC_U_Convert,	// ab- C
//    ADC_U_Convert,	// ac- B
//    ADC_V_Convert,	// bc- A
//    ADC_V_Convert,	// ba- C
//    ADC_W_Convert,	// ca- B
//    ADC_W_Convert	// cb- A
};




uint32_t adcChSum = 0;
uint32_t hardWareTriggerCh = 0;
uint32_t hardWareTriggerCmp1 = 0;
uint32_t hardWareTriggerCmp0 = 0;





/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

uint16_t VBus_Vx100ConvertToAdc(uint32_t voltVTemp)
{
    return ((float)voltVTemp*strHW.AdcMax_Adc/strHW.VBusVBase_Vx100);
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
uint16_t IBus_Ax100ConvertToAdc(uint32_t curtATemp)
{
    return ((float)curtATemp*strHW.AdcMax_Adc/strHW.IBusABase_Ax100);
}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
uint16_t Ntc_ResConvertToAdc(uint32_t resTemp,uint32_t resRef)
{
    return ((float)resTemp*strHW.AdcMax_Adc/(resTemp+resRef));
}


/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void Public_Parameter_Init(void)
{
/*
*VBusGain_x100          x100
*AdcRef_Vx100            x100
*IBusRes_OMx1000                x1000
*IBusGain_x100               x100
*IBusOffsetRef_Vx100          x100
*/
//	strHW.Fre_EpwmCK = Fre_MCU;
//	strHW.Fre_SectorTimer = Fre_Timer;
//	strHW.Fre_MSM = Fre_MState;
//	strHW.Fre_USM = Fre_UState;

//	strHW.SToMS= 1000;
//	strHW.SToUS = 1000000;


//	strHW.AdcMax_Adc= HW_Adc_Max;

//	strHW.VBusGain_x100 = HW_VBus_Gain*100;
//	strHW.AdcRef_Vx100 = HW_VBus_VDD*100;
//	strHW.VBusVBase_Vx100 = (uint32_t)strHW.AdcRef_Vx100*strHW.VBusGain_x100/100;


//	strHW.IBusRes_OMx1000 = HW_IBus_Res*1000;
//	strHW.IBusGain_x100 = HW_IBus_Gain*100;
//	strHW.IBusOffsetRef_Vx100= HW_IBus_OffsetRef*100;
//	strHW.IBusABase_Ax100 = (uint32_t)strHW.AdcRef_Vx100*1000*100/strHW.IBusGain_x100/strHW.IBusRes_OMx1000;


//	strHW.MotorPolePair = HW_Motor_PolePair;


//	strHW.flag.bits.gSys1ms = 0;
//	strHW.flag.bits.gSys10ms = 0;


}


/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void  Delay_USx(unsigned int timer) 
{
	while(timer)
	{
		Delay_1US_Debug(6);
		timer--;
	}
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void  Delay_MSx(unsigned int timer) 
{
	while(timer)
	{
		Delay_USx(980);
		timer--;
	}
}


/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void Bemf_AcmpClearIntFlag(void)
{
    //ACMP_ClearIntFlag(BEMF_ACMP_Set);/*clear*/
	//ACMP->ICLR |=(0x1<<BEMF_ACMP_Set);
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

uint32_t Bemf_AcmpGetResult(void)
{
//	if( ACMP0 == BEMF_ACMP_Set )
//	{
//		return ((ACMP->C0CON1 & ACMP_C0CON1_OUT_Msk)? 1:0);		
//	}
//	else if( ACMP1 == BEMF_ACMP_Set )
//	{
//		return ((ACMP->C1CON1 & ACMP_C1CON1_OUT_Msk)? 1:0);		
//	}
//	return 0;
	
    //return ACMP_GetResult(BEMF_ACMP_Set);
	
	return 0;
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void Bemf_AcmpDisableInt(void)
{
    //ACMP_DisableInt(BEMF_ACMP_Set);/*disable bemfAcmp IRQ*/
//	ACMP->LOCK = ACMP_WRITE_KEY;
//	ACMP->IMSC &= ~(0x1<<BEMF_ACMP_Set);				
//	ACMP->LOCK = 0x00;	
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void Bemf_AcmpEnableInt(void)
{
	//ACMP_EnableInt(BEMF_ACMP_Set);
//	ACMP->LOCK = ACMP_WRITE_KEY;
//	ACMP->IMSC |=(0x1<<BEMF_ACMP_Set);				
//	ACMP->LOCK = 0x00;	
}


/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void Bemf_AcmpSetEdge(void)
{
//	if((strMCtr.RotorSector&0x1) ==0x0) /*zero fall ,mask rise*/
//    {   
//        ACMP_ConfigEventAndIntMode(BEMF_ACMP_Set,ACMP_EVENT_INT_RISING);
//        ACMP_ConfigHYS(BEMF_ACMP_Set,ACMP_HYS_0_TO_1,ACMP_HYS_S_10);
//    }
//    else /*fall*/
//    {
//        ACMP_ConfigEventAndIntMode(BEMF_ACMP_Set,ACMP_EVENT_INT_FALLING);
//        ACMP_ConfigHYS(BEMF_ACMP_Set,ACMP_HYS_1_TO_0,ACMP_HYS_S_10);
//    }
}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void Bemf_AcmpSetPosCh_P(uint8_t sectorNow)
{
//    ACMP_EnableFilter(BEMF_ACMP_Set, BEMF_ACMP_Filter); 
//    ACMP_ConfigPositive(BEMF_ACMP_Set,SetCMPTabF[sectorNow]);
//    
//    if((sectorNow& 0x1) == 0x0) //Fall
//    {	
//        ACMP_ConfigEventAndIntMode(BEMF_ACMP_Set,ACMP_EVENT_INT_FALLING);
//        ACMP_ConfigHYS(BEMF_ACMP_Set,ACMP_HYS_1_TO_0,ACMP_HYS_S_10);
//    }
//    else //rise
//    {
//        ACMP_ConfigEventAndIntMode(BEMF_ACMP_Set,ACMP_EVENT_INT_RISING);
//        ACMP_ConfigHYS(BEMF_ACMP_Set,ACMP_HYS_0_TO_1,ACMP_HYS_S_10);
//    }
}


/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void Bemf_AcmpSetPosCh_F_Run(void)
{
//    ACMP_EnableFilter(BEMF_ACMP_Set, BEMF_ACMP_Filter); 
//    ACMP_ConfigPositive(BEMF_ACMP_Set,SetCMPTabF[strMCtr.RotorSector]);
//    
//    if((strMCtr.RotorSector& 0x1) == 0x0) //Fall
//    {	
//        ACMP_ConfigEventAndIntMode(BEMF_ACMP_Set,ACMP_EVENT_INT_FALLING);
//        ACMP_ConfigHYS(BEMF_ACMP_Set,ACMP_HYS_1_TO_0,ACMP_HYS_S_10);
//    }
//    else //rise
//    {
//        ACMP_ConfigEventAndIntMode(BEMF_ACMP_Set,ACMP_EVENT_INT_RISING);
//        ACMP_ConfigHYS(BEMF_ACMP_Set,ACMP_HYS_0_TO_1,ACMP_HYS_S_10);
//    }
	
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void Bemf_AcmpSetPosCh_F_Motion(void)
{
//    ACMP_EnableFilter(BEMF_ACMP_Set, ACMP_NFCLK_257_TSYS); /*根据需要调整*/
//    ACMP_ConfigPositive(BEMF_ACMP_Set,SetCMPTabF[strMCtr.RotorSector]);
//    
//    if((strMCtr.RotorSector& 0x1) == 0x0) //Fall
//    {	
//        ACMP_ConfigEventAndIntMode(BEMF_ACMP_Set,ACMP_EVENT_INT_FALLING);
//        ACMP_ConfigHYS(BEMF_ACMP_Set,ACMP_HYS_1_TO_0,ACMP_HYS_S_10);
//    }
//    else //rise
//    {
//        ACMP_ConfigEventAndIntMode(BEMF_ACMP_Set,ACMP_EVENT_INT_RISING);
//        ACMP_ConfigHYS(BEMF_ACMP_Set,ACMP_HYS_0_TO_1,ACMP_HYS_S_10);
//    }
}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void  ADCL_Convert(unsigned int ChannelMask)
{	
//	ADC0->LOCK = ADC1_LOCK_WRITE_KEY;
//	ADC0->SCAN = ChannelMask;
//	ADC0->CON2 |= ADC0_CON2_ADCST_Msk;
//	ADC0->LOCK = 0x0;
}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void  ADCH_Convert(unsigned int ChannelMask)
{	
//	ADC1->LOCK = ADC1_LOCK_WRITE_KEY;
//    ADC1->SCAN = ChannelMask;
//    ADC1->CON2 |= ADC1_CON2_ADCST_Msk;
//    ADC1->LOCK = 0x0;
}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void  ADCL_Convert_Wait_End(unsigned int ChannelMask)
{	
//	ADC0->LOCK = ADC1_LOCK_WRITE_KEY;
//	ADC0->SCAN = ChannelMask;
//	ADC0->CON2 |= ADC0_CON2_ADCST_Msk;
//	ADC0->LOCK = 0x0;
//    while(ADC0_IS_BUSY());
}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void  ADCH_Convert_Wait_End(unsigned int ChannelMask)
{	
//	ADC1->LOCK = ADC1_LOCK_WRITE_KEY;
//	ADC1->SCAN = ChannelMask;
//	ADC1->CON2 |= ADC1_CON2_ADCST_Msk;
//	ADC1->LOCK = 0x0;
//	while(ADC1_IS_BUSY());
}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
#define ADC_TEST_EN 0
uint32_t adcHTestOffset = 32768*1.1;   // 修改小数改变偏移
uint32_t adcLTestOffset = 32768*0.9;   // 修改小数改变偏移

uint32_t ADCH_GetResult(uint32_t Channel)
{
//#if ADC_TEST_EN
//    Channel = adcHTestOffset*(ADC1->DATA[Channel])>>15;
//    if(Channel>4096)
//    {
//        Channel = 4096;
//    }
//    return Channel;
//#else
//    return (ADC1->DATA[Channel]);
//#endif
	
	return 0;
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

uint32_t ADCL_GetResult(uint32_t Channel)
{
//#if ADC_TEST_EN
//    Channel = adcLTestOffset*(ADC0->DATA[Channel])>>15;
//    if(Channel>4096)
//    {
//        Channel = 4096;
//    }
//    return Channel;
//#else
//    return (ADC0->DATA[Channel]);
//#endif
	
	return 0;
}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments	 : 
* Return Value : 
***********************************************************************************************************************/

void ADCH_Wait_End(void)
{
//	while(ADC1_IS_BUSY()); 
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments	 : 
* Return Value : 
***********************************************************************************************************************/

void ADCH_HardwareTrigger_Enable(void)
{
//    ADC1_EnableChannelInt(hardWareTriggerCh);
//    ADC1_EnableHardwareTigger(hardWareTriggerCmp1);
//    ADC1_EnableHardwareTigger(hardWareTriggerCmp0);
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments	 : 
* Return Value : 
***********************************************************************************************************************/

void ADCH_HardwareTrigger_Disable(void)
{
//	ADC1_DisableHardwareTigger(hardWareTriggerCmp1);
//	ADC1_DisableHardwareTigger(hardWareTriggerCmp0);
//	ADC1_DisableChannelInt(hardWareTriggerCh);

//	ADCH_Wait_End();
}
/***********************************************************************************************************************
* Function Name:启动完成后，运行过程使能UVW采样
* Description  : 
* Arguments	 : 
* Return Value : 
***********************************************************************************************************************/

void ADCH_HWTG_UVW_Set(void)
{
//	ADC1_EnableEPWMCmp0TiggerChannel(ADC_U_Convert|ADC_V_Convert|ADC_W_Convert);
}

/***********************************************************************************************************************
* Function Name:滑行切入的时候，禁止UVW采样
* Description  : 
* Arguments	 : 
* Return Value : 
***********************************************************************************************************************/
void ADCH_HWTG_UVW_Clr(void)
{
//	ADC1_DisableEPWMCmp0TiggerChannel(ADC_U_Convert|ADC_V_Convert|ADC_W_Convert);
}
