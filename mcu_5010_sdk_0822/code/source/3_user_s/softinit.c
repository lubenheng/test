//==========================================================================//
/*****************************************************************************
*-----------------------------------------------------------------------------
* @file    softinit.c
* @author  7
* @version 
* @date    
* @brief   
*---------------------------------------------------------------------------//
**********************************************1******************************/
//==========================================================================//

//---------------------------------------------------------------------------/
//	include files
//---------------------------------------------------------------------------/
#include "mcu_driver.h"
#include "motor_driver.h"
#include "user.h"

//---------------------------------------------------------------------------/
//	Local pre-processor symbols/macros('#define')
//---------------------------------------------------------------------------/

//---------------------------------------------------------------------------/
//	Local variable  definitions
//---------------------------------------------------------------------------/
struct_Test							stru_Test;
//---------------------------------------------------------------------------/
//	Global variable definitions(declared in header file with 'extern')
//---------------------------------------------------------------------------/
//ByteFlag Time_Flag;

//---------------------------------------------------------------------------/
//	Local function prototypes('static')
//---------------------------------------------------------------------------/

/*****************************************************************************
 ** \brief	MC_SampPara_Init
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void MC_SampPara_Init(void)
{
	stru_Sample.G_Ip 	= _Q10(SW_GAIN_IPHASE);
	stru_Sample.G_Vb 	= _Q10(SW_GAIN_VBUS);
	stru_Sample.G_Ib 	= _Q10(SW_GAIN_IBUS);	
	stru_Sample.G_Sp 	= _Q10(SW_GAIN_SPEED);
	stru_Sample.G_Vc 	= _Q10(SW_GAIN_VCTRL);

	stru_Sample.G_Pw 	= _Q15((((float)HW_VBUS_SDR*SW_GAIN_VBUS)/HW_ADC_REF)*((HW_IBUS_RES*HW_AMP_GAIN_IBUS*SW_GAIN_IBUS)/HW_ADC_REF));

	stru_Sample.G_Power = (s32)SW_GAIN_POWER;	

	
	GET_ADC_REF_VAL();
}


/*****************************************************************************
 ** \brief	MC_CtrltPara_Init
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void MC_CtrltPara_Init(void)
{	
	Config.Mode_Shunt			= Config_Shunt_Mode;
	Config.Mode_Svpwm			= Config_SVPWM_Mode;

	Config.MotorDir				= Config_Motor_FR;

}

/*****************************************************************************
 ** \brief	MC_BasePara_Init
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void MC_BasePara_Init(void)
{

//	Basic.Freq_EPWM 		= EPWM_FREQ;
//	Basic.Tpwm_us 			= (2*1000000)/Basic.Freq_EPWM;
//	Basic.MotorVolt 		= (float)MOTOR_VOLT_BASE;
//	Basic.MotorCurr 		= (float)((HW_ADC_REF-HW_AMP_REF_IP)/(HW_AMP_GAIN_IP*HW_PHASE_RES));	
//	Basic.Ke 						= (float)MOTOR_Ke;
//	Basic.We_Ke 				= (s32)(MOTOR_Ke_K*MOTOR_WE_BASE);
//	Basic.We 						= _Q15((float)(MOTOR_SPEED_BASE)/(((float)EPWM_FREQ*60)/((float)FOC_ERAMP*2*MOTOR_PAIRS)));
}

/*****************************************************************************
 ** \brief	MC_FOCPara_Init
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void MC_FOCPara_Init(void)
{	
	
	stru_FOC.EPWM_Period 					= (u16)EPWM_PERIOD;
	stru_FOC.Vs_Max 						= _Q15(MAX_VOLTAGE_VECTOR);	

	stru_SVM_Calc.Cnt_Period		= EPWM_PERIOD;
	stru_SVM_Calc.Ts_Max				= _Q15(MAX_VOLTAGE_VECTOR);	
	stru_SVM_Calc.T_SAMWIN			= (EPWM_TGDLY + SAMPLE_HOLD) *32767/EPWM_PERIOD;
	stru_SVM_Calc.Cnt_Delay			= EPWM_TGDLY;
	
//	stru_FOC.Curr_Iq_Max 					= FOC_RUN_CURRENT_MAX;
//	stru_FOC.Curr_Iq_Min 					= FOC_RUN_CURRENT_MIN;

	//----------------------------------------------------------------------------/
	// 测试模式
	#if (Config_Test_Mode == VF_Test_Mode)
	{
		stru_Test.Ref_Final = Openloop_Volt_Max;	
	}
	#elif (Config_Test_Mode == IF_Test_Mode)
	{
		stru_Test.Ref_Final = I_Value(Openloop_Curr_Max);	
	}
	#endif
	stru_Test.Drag_Ref = stru_Test.Ref_Final >> 1;
	stru_Test.OmegaEMax			= Openloop_Speed_Max * MOTOR_PAIRS *_Q16_VAL / ( 60 * EPWM_FREQ);
	stru_Test.Delta_Ref			= (stru_Test.Ref_Final >> 1) / stru_Test.OmegaEMax + 1;
	stru_Test.DeltaCnt			= (int32_t)((float)Openloop_AccTime *  (uint32_t)EPWM_FREQ / stru_Test.OmegaEMax);
	if(stru_Test.DeltaCnt < 1)	stru_Test.DeltaCnt = 1;	
	
	//------------------------------------------------------------------------/
	// D轴电流环参数
	PID_ID.Kp 						= FOC_DKP;
	PID_ID.Ki 						= FOC_DKI;
	PID_ID.Lower_Output 	= FOC_DOUT_MIN;
	PID_ID.Upper_Output 	= FOC_DOUT_MAX;
	PID_ID.Lower_Integral 	= FOC_DOUT_MIN;
	PID_ID.Upper_Integral 	= FOC_DOUT_MAX;
	PID_ID.Integral 			= 0;
	PID_ID.Out 						= 0;
	PID_ID.KiSum 					= 0;

	//------------------------------------------------------------------------/
	// Q轴电流环参数	
	PID_IQ.Kp 						= FOC_QKP;
	PID_IQ.Ki 						= FOC_QKI;
	PID_IQ.Lower_Output 	= FOC_QOUT_MIN;
	PID_IQ.Upper_Output 	= FOC_QOUT_MAX;
	PID_IQ.Lower_Integral 	= FOC_QOUT_MIN;
	PID_IQ.Upper_Integral 	= FOC_QOUT_MAX;
	PID_IQ.Integral 			= 0;
	PID_IQ.Out 						= 0;
	PID_IQ.KiSum 					= 0;	
}

/*****************************************************************************
 ** \brief	MC_LPFPara_Init
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void MC_LPFPara_Init(void)
{
//	// Lpf PARA
//	Para.Lpf.Samp_Ibus 	= LPF_SAMP_IBUS;
//	Para.Lpf.Samp_Vbus 	= LPF_SAMP_VBUS;
//	Para.Lpf.Samp_Vctrl = LPF_SAMP_VCTRL;
//	Para.Lpf.Samp_Vtem 	= LPF_SAMP_TEMP;
//	Para.Lpf.Speed_Val 	= LPF_SPEED_VAL;
//	Para.Lpf.Power_Val 	= LPF_POWER_VAL;	
}




/*****************************************************************************
 ** \brief	MC_Para_Init
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void MC_Para_Init(void)
{
	MC_CtrltPara_Init();	
	MC_FOCPara_Init();	
	MC_SampPara_Init();
	
}

/*****************************************************************************
 ** \brief	__EI
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
#if 0
void __EI(void)
{
	/* Enable EPWM_INT */	
	#if (Config_Shunt_Mode == Single_Shunt)
	{
		EPWM->ICLR |= 0xFFFFFFFF;
		EPWM_EnableZeroInt(EPWM_CH_0_MSK);	
		NVIC_EnableIRQ(EPWM_IRQn);
		NVIC_SetPriority(EPWM_IRQn,1);	
	}
	#endif	

	/* Enable ADC1_INT */	
	#if (Config_Shunt_Mode == Double_Shunt)
	{
		ADC1_EnableChannelInt(ADC1_SCAN_CHA);
		NVIC_EnableIRQ(ADC1_IRQn);
		NVIC_SetPriority(ADC1_IRQn,1);	
	}
	#endif
	#if (Config_Shunt_Mode == Treble_Shunt)
	{
		ADC1_EnableChannelInt(ADC1_SCAN_CHA);
		NVIC_EnableIRQ(ADC1_IRQn);
		NVIC_SetPriority(ADC1_IRQn,1);	
	}
	#endif	
	
	/* Enable TMR0_INT */
	#if (Config_Start_Mode == Start_Hall)
	{	
		TMR_EnableOverflowInt(TMR0);
		TMR_ClearOverflowIntFlag(TMR0);
		NVIC_EnableIRQ(TMR0_IRQn);	
		NVIC_SetPriority(TMR0_IRQn,2);
	}
	#endif
	
	/* Enable UART1_INT */
	#if (Config_Comm_Mode == Comm_UI)	
	{
		UART_EnableIRQ(UART_RBRIE_EN_Msk);  
		NVIC_EnableIRQ(UART1_IRQn); 
		NVIC_SetPriority(UART1_IRQn,2);	
	}
	#endif

	/* Enable SysTick_INT */	
	NVIC_EnableIRQ(SysTick_IRQn);
	NVIC_SetPriority(SysTick_IRQn,3);
	
	/* Enable ACMP_INT */
	NVIC_EnableIRQ(ACMP_IRQn);
	NVIC_SetPriority(ACMP_IRQn,0);
	#if (ACMP_CH == ACMP0)
	ACMP_ClearIntFlag(ACMP0);
	ACMP_EnableInt(ACMP0);
	#else
	ACMP_ClearIntFlag(ACMP1);
	ACMP_EnableInt(ACMP1);	
	#endif

	#if (Config_CCP_Capture == CCP_Capture_Enable)	
	CCP_EnableCAPMode0Int(CCP_CAPTURE_MODULE);		/*开启CCP模块捕获模式0中断*/
	NVIC_EnableIRQ(CCP_IRQn);		
	NVIC_SetPriority(CCP_IRQn,0);					/*优先级0~3， 0最高、3最低*/			
	#endif	


}

#endif
/*****************************************************************************
 ** \brief	DI
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
#if 0
void __DI(void)
{
	NVIC_DisableIRQ(EPWM_IRQn);
	NVIC_DisableIRQ(ADC1_IRQn);
	NVIC_DisableIRQ(SysTick_IRQn);
	NVIC_DisableIRQ(UART1_IRQn);
	NVIC_DisableIRQ(TMR0_IRQn);
	NVIC_DisableIRQ(TMR1_IRQn);
	NVIC_DisableIRQ(ACMP_IRQn);
	NVIC_DisableIRQ(ADC0_IRQn);
}
#endif
/*****************************************************************************
 ** \brief	
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
//void f_DelayTime(u32 delay)
//{
//   for(; delay > 0; delay--) 
//	{
//     __nop();
//  }
//}

/*****************************************************************************
 ** \brief	
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
//void DelayTime_ms(u32 delay)
//{
//	u32 i,j;
//	
//	for(i=0; i<delay; i++)
//	{
//		for(j=0; j<10000; j++) //1ms,64M
//		{
//			;
//		}
//	}
//}




/*****************************************************************************
 ** \brief	GET_ADC_REF_VAL
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
//#define OFFSET_ERO_MAX  (0.2*4096 / HW_ADC_REF)
void GET_ADC_REF_VAL(void)
{
	stru_Sample.REF[CH_IP_A] = HW_AMP_REF_IP * 4095 / HW_ADC_REF;
	stru_Sample.REF[CH_IP_B] = stru_Sample.REF[CH_IP_A];	
	stru_Sample.REF[CH_IP_C] = stru_Sample.REF[CH_IP_A];	
	
}

/******************************** END OF FILE *******************************/
