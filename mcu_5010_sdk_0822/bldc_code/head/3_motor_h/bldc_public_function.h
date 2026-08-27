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
#ifndef __BLDC_PUBLIC_FUNCTION_H__
#define __BLDC_PUBLIC_FUNCTION_H__
/*****************************************************************************/
/* Include files */
/*****************************************************************************/
//#include "cms32m5xxx.h"
#include "stdint.h"
#include "types.h"

/* PWM output channel should disable mask output */
#define AH_PWM								                (0X003Dul)	//0011 1101  /*AH-CH0P-BIT1,AL-CH0N-BIT0 */
#define BH_PWM								                (0x0037ul)	//0011 0111	 /*BH-CH1P-BIT3,BL-CH1N-BIT2 */
#define CH_PWM								                (0x001Ful)  //0001 1111	 /*CH-CH2P-BIT5,CL-CH0N-BIT4 */
#define AL_PWM								                (0X003Eul)	//0011 1110	
#define BL_PWM								                (0x003Bul)	//0011 1011	
#define CL_PWM								                (0x002Ful)  //0010 1111

#define ABH_PWM								                (0X0035ul)	//0011 0101			
#define ACH_PWM								                (0x001Dul)	//0001 1101			
#define BCH_PWM								                (0x0017ul)  //0001 0111

#define AHL_PWM								                (0X003C)    //0011 1100
#define BHL_PWM								                (0X0033)    //0011 0011
#define CHL_PWM								                (0X000F)    //0000 1111


/*up bridge024*/
#define ABCH								                (0x2A00)
/*down bridge135*/
#define ABCL      							                (0x1500)


#define AH									                (0x0200ul)  //0000 0010		
#define BH									                (0x0800ul)	//0000 1000			
#define CH									                (0x2000ul)  //0010 0000
#define AL									                (0x0100ul) 	//0000 0001			
#define BL									                (0x0400ul)  //0000 0100		
#define CL									                (0x1000ul)	//0001 0000

#define DRIVER_OFF							                (0X003F)
#define DRIVER_ABCL      					                (0X003F | AL | BL | CL)
#define DRIVER_ABCH							                (0X003F | AH | BH | CH)
#define DRIVER_PWM_ON						                (0X0000)

#define DRIVER_AL      						                (0X003F | AL)
#define DRIVER_BL      						                (0X003F | BL)
#define DRIVER_CL      						                (0X003F | CL)

#define DRIVER_AH      						                (0X003F | AH)
#define DRIVER_BH      						                (0X003F | BH)
#define DRIVER_CH      						                (0X003F | CH)


#define DRIVER_ABCLPWM						                (0X003F&AL_PWM & BL_PWM& CL_PWM)
#define DRIVER_ABCHPWM						                (0X003F&AH_PWM & BH_PWM& CH_PWM)



/*2P shang guan mask xia guan mask*/
#define MASK_DRIVE_A_B							            (0X003F |AH | BL)
#define MASK_DRIVE_A_C							            (0X003F |AH | CL)
#define MASK_DRIVE_B_C							            (0X003F |BH | CL)
#define MASK_DRIVE_B_A							            (0X003F |BH | AL)
#define MASK_DRIVE_C_A							            (0X003F |CH | AL)
#define MASK_DRIVE_C_B							            (0X003F |CH | BL)

/*3P shang guan mask xia guan mask */
#define DEGAUSS_DRIVE_CA_B						            (0X003F | CH | AH | BL)
#define DEGAUSS_DRIVE_A_BC						            (0X003F | AH | BL | CL)
#define DEGAUSS_DRIVE_AB_C						            (0X003F | AH | BH | CL)
#define DEGAUSS_DRIVE_B_CA						            (0X003F | BH | CL | AL)
#define DEGAUSS_DRIVE_BC_A						            (0X003F | BH | CH | AL)
#define DEGAUSS_DRIVE_C_AB						            (0X003F | CH | AL | BL)

/*2p shang guan hbpwm  xia guan hbpwm*/

#define DRIVE_AHLPWM_BHLPWM						            (AHL_PWM & BHL_PWM)
#define DRIVE_AHLPWM_CHLPWM						            (AHL_PWM & CHL_PWM)
#define DRIVE_BHLPWM_CHLPWM						            (BHL_PWM & CHL_PWM)
#define DRIVE_BHLPWM_AHLPWM						            (BHL_PWM & AHL_PWM)
#define DRIVE_CHLPWM_AHLPWM						            (CHL_PWM & AHL_PWM)
#define DRIVE_CHLPWM_BHLPWM						            (CHL_PWM & BHL_PWM)

/*2p shang guan dlpwm xia guan mask*/

#define DLDRIVE_AHPWM_B						                (AH_PWM | BL)
#define DLDRIVE_AHPWM_C						                (AH_PWM | CL)
#define DLDRIVE_BHPWM_C						                (BH_PWM | CL)
#define DLDRIVE_BHPWM_A						                (BH_PWM | AL)
#define DLDRIVE_CHPWM_A						                (CH_PWM | AL)
#define DLDRIVE_CHPWM_B						                (CH_PWM | BL)

/*3p shang guan dlpwm xia guan mask*/

#define DLDRIVE_AHPWM_BC					                (AH_PWM | BL|CL)
#define DLDRIVE_ABHWPM_C								    (ABH_PWM |CL)
#define DLDRIVE_BHPWM_AC								    (BH_PWM | AL|CL)
#define DLDRIVE_BCHPWM_A								    (BCH_PWM |AL)
#define DLDRIVE_CHPWM_AB								    (CH_PWM | AL|BL)
#define DLDRIVE_ACHPWM_B								    (ACH_PWM |BL)


/*POSITION CHECK USE*/
#define PDRIVE_A_BC								            (0X003F | AH | BL | CL)
#define PDRIVE_B_CA								            (0X003F | BH | CL | AL)
#define PDRIVE_C_AB								            (0X003F | CH | AL | BL)
#define PDRIVE_AB_C								            (0X003F | AH | BH | CL)
#define PDRIVE_BC_A								            (0X003F | BH | CH | AL)
#define PDRIVE_CA_B								            (0X003F | CH | AH | BL)

#define PDRIVE_A_B								            (0X003F |AH | BL)
#define PDRIVE_A_C								            (0X003F |AH | CL)
#define PDRIVE_B_C								            (0X003F |BH | CL)
#define PDRIVE_B_A								            (0X003F |BH | AL)
#define PDRIVE_C_A								            (0X003F |CH | AL)
#define PDRIVE_C_B								            (0X003F |CH | BL)


#if(1)
#define DRIVE_A_B								            (AH_PWM | BL)
#define DRIVE_A_C								            (AH_PWM | CL)
#define DRIVE_B_C								            (BH_PWM | CL)
#define DRIVE_B_A								            (BH_PWM | AL)
#define DRIVE_C_A								            (CH_PWM | AL)
#define DRIVE_C_B								            (CH_PWM | BL)
#elif(0)
#define DRIVE_A_B								            (AHL_PWM | BL)
#define DRIVE_A_C								            (AHL_PWM | CL)
#define DRIVE_B_C								            (BHL_PWM | CL)
#define DRIVE_B_A								            (BHL_PWM | AL)
#define DRIVE_C_A								            (CHL_PWM | AL)
#define DRIVE_C_B								            (CHL_PWM | BL)
#elif(0)
#define DRIVE_A_B								            (AH | BL_PWM)
#define DRIVE_A_C								            (AH | CL_PWM)
#define DRIVE_B_C								            (BH | CL_PWM)
#define DRIVE_B_A								            (BH | AL_PWM)
#define DRIVE_C_A								            (CH | AL_PWM)
#define DRIVE_C_B								            (CH | BL_PWM)
#else
#define DRIVE_A_B								            (AH | BHL_PWM)
#define DRIVE_A_C								            (AH | CHL_PWM)
#define DRIVE_B_C								            (BH | CHL_PWM)
#define DRIVE_B_A								            (BH | AHL_PWM)
#define DRIVE_C_A								            (CH | AHL_PWM)
#define DRIVE_C_B								            (CH | BHL_PWM)

#endif

// 用于PWM_OFF时候，下管由于互补打开，这时候反相只需要将原下管对应上管打开
#if(0)
#define DRIVE_Adown_pwm_Bup_mask						    (AHL_PWM | BH)
#define DRIVE_Adown_pwm_Cup_mask						    (AHL_PWM | CH)
#define DRIVE_Bdown_pwm_Cup_mask                            (BHL_PWM | CH)
#define DRIVE_Bdown_pwm_Aup_mask                            (BHL_PWM | AH)
#define DRIVE_Cdown_pwm_Aup_mask                            (CHL_PWM | AH)
#define DRIVE_Cdown_pwm_Bup_mask                            (CHL_PWM | BH)
#elif(1)
#define DRIVE_Adown_pwm_Bup_mask                            (0X003F|AL | BH)
#define DRIVE_Adown_pwm_Cup_mask                            (0X003F|AL | CH)
#define DRIVE_Bdown_pwm_Cup_mask                            (0X003F|BL | CH)
#define DRIVE_Bdown_pwm_Aup_mask                            (0X003F|BL | AH)
#define DRIVE_Cdown_pwm_Aup_mask                            (0X003F|CL | AH)
#define DRIVE_Cdown_pwm_Bup_mask                            (0X003F|CL | BH)
#endif




#define Set_AH_Pwm								       		(0X003Dul)	//0011 1101			
#define Set_BH_Pwm								        	(0x0037ul)	//0011 0111			
#define Set_CH_Pwm								          	(0x001Ful)  //0001 1111
#define Set_AL_Pwm								           	(0X003Eul)	//0011 1110			
#define Set_BL_Pwm								         	(0x003Bul)	//0011 1011			
#define Set_CL_Pwm								        	(0x002Ful)  //0010 1111
#define Set_All_Pwm								          	(0x0000ul)  //0001 1111
#define Set_ABCH_Pwm								    	(Set_AH_Pwm&Set_BH_Pwm&Set_CH_Pwm)  //0001 1111
#define Set_ABCL_Pwm								     	(Set_AL_Pwm&Set_BL_Pwm&Set_CL_Pwm)  //0001 1111



#define Set_AH_Mask								         	(0X0002ul)	//0000 0010
#define Set_BH_Mask								          	(0X0004ul)	//0000 1000
#define Set_CH_Mask							             	(0X0020ul)  //0010 0000
#define Set_AL_Mask								         	(0X0001ul)	//0000 0001
#define Set_BL_Mask								         	(0X0004ul)	//0000 0100	
#define Set_CL_Mask								          	(0X0010ul)  //0001 0000
#define Set_All_Mask								     	(0X3F00ul)  //0011 1111



#define Mask_AH_On									        (0x0001ul)  //0000 0001////TODO
#define Mask_BH_On									        (0x0004ul)	//0000 0100////TODO	
#define Mask_CH_On									        (0x0010ul)  //0001 0000////TODO
#define Mask_AL_On									        (0x0002ul) 	//0000 0010////TODO	
#define Mask_BL_On									       	(0x0008ul)  //0000 1000////TODO
#define Mask_CL_On									    	(0x0020ul)	//0010 0000////TODO

#define Mask_ABCL_On									   	(Mask_AL_On|Mask_BL_On|Mask_CL_On)	//0010 0000
#define Mask_ABCH_On										(Mask_AH_On|Mask_BH_On|Mask_CH_On)	//0010 0000
#define Mask_All_Off									  	(0x003Ful)	//0010 0000

/*新的组合方式*/
#define Driver_Set_AH_Pwm__Mask_BL_On						((Set_All_Mask&Set_AH_Pwm)|(Mask_BL_On))
#define Driver_Set_AH_Pwm__Mask_CL_On						((Set_All_Mask&Set_AH_Pwm)|(Mask_CL_On))
#define Driver_Set_BH_Pwm__Mask_CL_On						((Set_All_Mask&Set_BH_Pwm)|(Mask_CL_On))
#define Driver_Set_BH_Pwm__Mask_AL_On						((Set_All_Mask&Set_BH_Pwm)|(Mask_AL_On))
#define Driver_Set_CH_Pwm__Mask_AL_On						((Set_All_Mask&Set_CH_Pwm)|(Mask_AL_On))
#define Driver_Set_CH_Pwm__Mask_BL_On						((Set_All_Mask&Set_CH_Pwm)|(Mask_BL_On))

/*上管互补输出PWM，仅互补模式可以使用*/
#define Driver_Set_AH_Pwm_Set_AL_Pwm__Mask_BL_On			((Set_All_Mask&Set_AH_Pwm&Set_AL_Pwm)|(Mask_BL_On))
#define Driver_Set_AH_Pwm_Set_AL_Pwm__Mask_CL_On			((Set_All_Mask&Set_AH_Pwm&Set_AL_Pwm)|(Mask_CL_On))
#define Driver_Set_BH_Pwm_Set_BL_Pwm__Mask_CL_On			((Set_All_Mask&Set_BH_Pwm&Set_BL_Pwm)|(Mask_CL_On))
#define Driver_Set_BH_Pwm_Set_BL_Pwm__Mask_AL_On			((Set_All_Mask&Set_BH_Pwm&Set_BL_Pwm)|(Mask_AL_On))
#define Driver_Set_CH_Pwm_Set_CL_Pwm__Mask_AL_On			((Set_All_Mask&Set_CH_Pwm&Set_CL_Pwm)|(Mask_AL_On))
#define Driver_Set_CH_Pwm_Set_CL_Pwm__Mask_BL_On			((Set_All_Mask&Set_CH_Pwm&Set_CL_Pwm)|(Mask_BL_On))


#define Driver_Set_All_Pwm									((Set_All_Mask&Set_All_Pwm) | (Mask_All_Off))
#define Driver_Set_ABCH_Pwm									((Set_All_Mask&Set_ABCH_Pwm) | (Mask_All_Off))
#define Driver_Set_ABCL_Pwm									((Set_All_Mask&Set_ABCL_Pwm) | (Mask_All_Off))

#define Driver_Set_All_Mask__Mask_AH_On     				((Set_All_Mask) | (Mask_AH_On))
#define Driver_Set_All_Mask__Mask_BH_On     				((Set_All_Mask) | (Mask_BH_On))
#define Driver_Set_All_Mask__Mask_CH_On     				((Set_All_Mask) | (Mask_CH_On))
#define Driver_Set_All_Mask__Mask_AL_On     				((Set_All_Mask) | (Mask_AL_On))
#define Driver_Set_All_Mask__Mask_BL_On     				((Set_All_Mask) | (Mask_BL_On))
#define Driver_Set_All_Mask__Mask_CL_On     				((Set_All_Mask) | (Mask_CL_On))

#define Driver_Set_All_Mask__Mask_ABCL_On     				((Set_All_Mask) | (Mask_ABCL_On))
#define Driver_Set_All_Mask__Mask_ABCH_On					((Set_All_Mask) | (Mask_ABCH_On))







typedef union
{
    uint32_t word;
    struct
    {
        uint32_t 		gSys100us:1;
        uint32_t 		gSys1ms:1;
		uint32_t 		gSys10ms:1;
		uint32_t		gOnceInitEnd:1;
    }bits;
}Timer_Flag_e;


//work type +xxx +data type

typedef struct 
{
    uint16_t			AdcMax_Adc;
    uint16_t			AdcRef_Vx100;
    
    uint16_t			VBusGain_x100;
    uint16_t			VBusVBase_Vx100;


    uint16_t			IBusRes_OMx1000;
    uint16_t			IBusGain_x100;
    uint16_t			IBusOffsetRef_Vx100;
    uint16_t			IBusABase_Ax100;
	uint8_t				MotorPolePair;

    Timer_Flag_e		flag;

    uint16_t 			Fre_USM;  /*主循环频率*/
    uint16_t 			Fre_MSM; /*电机状态机频率*/
    uint32_t 			Fre_SectorTimer;/*扇区定时器频率*/
    uint32_t 			Fre_EpwmCK;

    uint32_t 			SToUS;/*1s等于1000 000us*/
    uint32_t 			SToMS;/*1s等于1000ms*/

}HW_Para_t;


extern HW_Para_t 		strHW;



/*tool error */

typedef union
{
    uint32_t word;
    struct
    {
        uint32_t 		gIBusAcmpOCP:1;//母线过流/短路保护
        uint32_t 		gIBusPeakOCP1:1;//母线峰值过流保护1
        uint32_t 		gIBusAvgOCP1:1;//母线均值过流保护1	    
        uint32_t 		gIBusAvgOCP2:1;//母线均值过流保护2	   
        
        uint32_t 		gIBusAvgOCP3:1;//母线均值过流保护3	   
        uint32_t 		gVBusRunUVP1:1;//母线运行欠压保护1	   	    
        uint32_t 		gVBusRunUVP2:1;//母线运行欠压保护2			
        uint32_t 		gVBusStbUVP:1;//母线待机欠压保护	
        
        uint32_t 		gVBusOVP:1;//母线过压保护	
        uint32_t 		gLVD:1;//LVD复位异常
        uint32_t 		gNtcMos:1;//MOS温度异常
        uint32_t 		gNtcBat:1;//BATTERY温度异常
        
        uint32_t 		gIBusOffset:1;//母线电流偏置异常
        uint32_t 		gMosBridgeH:1;//MOS上桥异常
        uint32_t 		gMosBridgeL:1;//MOS下桥异常
        uint32_t 		gWDTOver:1;//看门狗溢出异常
        
        uint32_t 		gHardFault:1;//HardFault异常

    }bits;
}UC_Error_u;    

typedef struct 
{
    UC_Error_u          userErr;   
}User_Error_t;




 /*motor error */
typedef union
{
    uint16_t word;
    struct
    {
		uint16_t	gIPDOCP:1;//IPD短路过流
		uint16_t	gLackPhase:1;//电机缺相,任意扇区IPD值与静态偏置差异过小

		uint16_t	gBlock:1;//换相堵转异常
		uint16_t	gLosStep:1;//电机失步异常
		uint16_t	gOpenLoopErr:1;//开环拖动异常
    }bits;

}MC_Error_u;   


typedef struct 
{
    MC_Error_u	motorErr;

    // lostStep
    uint8_t		LoseStepProtectEn;
    uint16_t 	LoseStepProtect_Cnt;
    uint16_t   	LoseStepProtect_CntSet;
    uint8_t   	LoseStepProtect_Flag;
    uint8_t   	LoseStepProtect_MoveBit;


    // lackPhase
    uint8_t  	LackPhaseProtectEn;
    uint16_t 	LackPhaseProtect_Delta;
    


    // block run
    uint8_t   	BlockProtectEn;
    uint16_t 	BlockProtectDutyH_CntSet;
    uint16_t 	BlockProtectDutyL_CntSet;
    uint16_t  	BlockProtectDuty_Q15;
    uint16_t  	BlockProtect_Cnt;               // 计时
    uint16_t  	BlockProtectErr_Cnt;

	uint8_t		ForceChangePhaseEn;
	uint32_t	ForceChangePhase_T0Cnt;


}
Motor_Error_t;

extern Motor_Error_t 	strMErr;
extern User_Error_t 	strUErr;

extern uint16_t gTwoDriverTab[2][6];
extern uint16_t gTwoDriverReTab[2][6];

extern uint16_t ipdDriveTabDr3[6] ;
extern uint16_t ipdDriveTabDr2[6];


extern uint8_t SetCMPTabF[6];
extern uint16_t SetADCTabF[6];
extern uint16_t SetADCTabP[6];
extern uint16_t SetADCTabN[6];
extern uint8_t GetADCTabP[6]; 
extern uint8_t GetADCTabN[6];
extern uint8_t GetADCTabF[6];


extern uint32_t adcChSum;
extern uint32_t hardWareTriggerCh;
extern uint32_t hardWareTriggerCmp0;
extern uint32_t hardWareTriggerCmp1;


void Public_Parameter_Init(void);
uint16_t VBus_Vx100ConvertToAdc(uint32_t voltVTemp);
uint16_t IBus_Ax100ConvertToAdc(uint32_t curtATemp);
uint16_t Ntc_ResConvertToAdc(uint32_t resTemp,uint32_t resRef);
void  Delay_USx(unsigned int timer) ;
void  Delay_MSx(unsigned int timer) ;


void Bemf_AcmpClearIntFlag(void);
uint32_t Bemf_AcmpGetResult(void);
void Bemf_AcmpDisableInt(void);
void Bemf_AcmpEnableInt(void);
void Bemf_AcmpSetPosCh_P(uint8_t sectorNow);
void Bemf_AcmpSetEdge(void);
void Bemf_AcmpSetPosCh_F_Run(void);
void Bemf_AcmpSetPosCh_F_Motion(void);

void Pwm_SetPeriodAndTGD(void);
void Pwm_SetDuty(uint16_t uiDuty);

void Timer_Record_Stop(void);
void Timer_Record_INT_Disable(void);
void Timer_Record_Start(void);
uint32_t Timer_Record_Read(void);
void Timer_Record_Reset(void);
void Timer_Record_Write(uint32_t timerWrite);


void Timer_MC_Stop(void);
void Timer_MC_INT_Disable(void);
void Timer_MC_Start(void);
void Timer_MC_Write_Start(uint32_t timerWrite);
void Timer_MC_INT_Enable(void);


void  ADCL_Convert(unsigned int ChannelMask);
void  ADCH_Convert(unsigned int ChannelMask);
void  ADCH_Convert_Wait_End(unsigned int ChannelMask);
void  ADCL_Convert_Wait_End(unsigned int ChannelMask);
uint32_t ADCH_GetResult(uint32_t Channel);
uint32_t ADCL_GetResult(uint32_t Channel);


void ADCH_HardwareTrigger_Enable(void);
void ADCH_HardwareTrigger_Disable(void);
void ADCH_Wait_End(void);
void ADCH_HWTG_UVW_Set(void);
void ADCH_HWTG_UVW_Clr(void);

#endif /*  */
