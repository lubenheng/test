//==========================================================================//
/*****************************************************************************
*-----------------------------------------------------------------------------
* @file    mc_foc.c
* @author  7
* @version 
* @date    
* @brief   
*---------------------------------------------------------------------------//
*****************************************************************************/
//==========================================================================//

//---------------------------------------------------------------------------/
//	include files
//---------------------------------------------------------------------------/
#include "mcu_driver.h"
#include "motor_driver.h"
#include "user.h"

#include "ADC_Reg.h"
#include "ADC_Type.h"
#include "ADC.h"
#include "ADC_test.h"
//---------------------------------------------------------------------------/
//	Local pre-processor symbols/macros('#define')
//---------------------------------------------------------------------------/

//---------------------------------------------------------------------------/
//	Local variable  definitions
//---------------------------------------------------------------------------/
PID_Components        	PID_ID 								= {0};  /* D轴电流环 */

PID_Components        	PID_IQ 								= {0};	/* Q轴电流环*/

//PID_Components        	PID_WL 								= {0};	/* 速度/功率外环*/

struct_SinCos	  				stru_SinCos 					= {0};	/* SinCos计算结构体 */

struct_Curr_abc		 			stru_Curr_abc 				= {0};	/* 马达abc三相电流 */

struct_Clark	  				stru_Curr_alphabeta 	= {0};	/* alpha、beta坐标系电流 */

struct_Park		  				stru_Curr_dq 					= {0};	/* d、q坐标系电流 */

struct_Park		  				stru_Curr_dq_ref 			= {0};	/* d、q坐标系参考电流 */

struct_RevPark	  			stru_Volt_alphabeta 	= {0};	/* alpha、beta坐标系电压 */

struct_Volt_dq	  			stru_Volt_dq 					= {0};	/* d、q坐标系电压 */

//struct_Arctan     			stru_Arctan 					= {0};  /* 反正切计算 */ 

struct_SVPWM        		stru_SVM_Calc 				= {0}; 	/* 空间矢量结构体 */ 



//---------------------------------------------------------------------------/
//	Global variable definitions(declared in header file with 'extern')
//---------------------------------------------------------------------------/

//---------------------------------------------------------------------------/
//	Local function prototypes('static')
//---------------------------------------------------------------------------/
extern const uint16_t 		Q15Sqrt_Table[677];
extern const int16_t 		Sin_Cos_Table[256];

//uint8_t bBLDCSwitchFlag = 0;
//uint8_t bBLDCFristFlag  = 0;

//===========================================================================/
//***** definitions  end ****************************************************/
//===========================================================================/

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_ControlPara_Init
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_ControlPara_Init(void)
{
#if 1
	//------------------------------------------------------------------------/	
	//变量初始化

	stru_Volt_dq.Ud 					= 0;
	stru_Volt_dq.Uq 					= 0;             
	stru_Curr_abc.Ia 					= 0;
	stru_Curr_abc.Ib 					= 0;
	stru_Curr_abc.Ic 					= 0;	
	stru_Volt_alphabeta.Alpha = 0;
	stru_Volt_alphabeta.Beta  = 0;
	stru_SinCos.Sin						= 0;
	stru_SinCos.Cos						= 32767;		
	stru_Curr_dq_ref.Id 			= 0;
	stru_Curr_dq_ref.Iq 			= 0;

	
	//------------------------------------------------------------------------/	
	//变量初始化
	stru_FOC.Elec_Angle 			= 0;
	stru_FOC.Offset_Angle 		= 0;	
	stru_FOC.Elec_We 					= 0;

	//------------------------------------------------------------------------/		
	//PID参数初始化
	PID_ID.Integral 			= 0;
	PID_ID.Out 						= 0;
	PID_ID.KiSum 					= 0;
	PID_IQ.Integral 			= 0;
	PID_IQ.Out 						= 0;
	PID_IQ.KiSum 					= 0;	
	
	
	stru_Test.Drag_Ref		= stru_Test.Ref_Final >> 1;
	stru_Test.OmegaE			= 0;

#endif
}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_Read_Current
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void  FOC_Read_Current(void)
{

}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_Overcurrent_Check
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_Overcurrent_Check(void)
{

}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_Brake_Control
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_Brake_Control(void)
{

}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_Start_Check
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_Start_Check(void)
{

}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_Stop_Check
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_Stop_Check(void)
{
	
}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_Control_Init
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_Control_Init(void)
{
	//-----------------------------------------------------------------------//
	// 参数初始化 
	FOC_ControlPara_Init();

	//GET_ADC_REF_VAL();

#if 1	
	Brige_Output_Off()
#else	
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */
	//INFO("before:  %x\n",REG32(0x40010CB0));
	delay_1us(1);
	MCPWM0_FAIL012 &= ~BIT6;
	//INFO("after:  %x\n",REG32(0x40010CB0));
	delay_1us(1);
	MCPWM0_CH_DEF = (MCPWM0_CH_DEF & ~MCPWM0_CH_DEF_CH012_MASK) | MCPWM0_CH_DEF_CH012_SET(0x0); 
	MCPWM0_PRT = 0x0;
#endif
	//-----------------------------------------------------------------------//
	// 启动

	if(Flag.MC_RunStop )

	{	
		Brige_Output_On();
		
		MOTOR_STATE = MC_START;
	}

	//-----------------------------------------------------------------------//
	
}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_Control_Charge
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_Control_Charge(void)
{	

}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_Wind_Check
* Description    : 顺逆风检测	
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_Wind_Check(void)
{	

}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_Control_Wind
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_Control_Wind(void)
{

}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_Control_Align
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_Control_Align(void)
{

}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_Control_Start
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/

void FOC_Control_Start(void)
{	
	#if (Config_Test_Mode == PWM_Test_Mode)
	{
		MCPWM0_PRT = 0x0000DEAD;
		MCPWM0->TH20 = -(stru_FOC.EPWM_Period * 204 >> 10);
		MCPWM0->TH21 =  (stru_FOC.EPWM_Period * 204 >> 10);
		MCPWM0->TH10 = -(stru_FOC.EPWM_Period * 307 >> 10);
		MCPWM0->TH11 =  (stru_FOC.EPWM_Period * 307 >> 10);
		MCPWM0->TH00 = -(stru_FOC.EPWM_Period * 409 >> 10);
		MCPWM0->TH01 =  (stru_FOC.EPWM_Period * 409 >> 10);		
		MCPWM0_PRT		= 0x0000CAFE;
	}
	#else
	{
		//=======================================================================/
		// 角度处理
		stru_FOC.Elec_We = stru_Test.OmegaE;
		stru_FOC.Elec_Angle += stru_FOC.Elec_We;
		if(stru_FOC.Elec_Angle > 65535) stru_FOC.Elec_Angle -=65536;
		if(stru_FOC.Elec_Angle < 0) 		stru_FOC.Elec_Angle +=65536;
		//=======================================================================/
		// 计算角度正余弦
		 stru_SinCos 	= FOC_SinCos_Calc(stru_FOC.Elec_Angle);	

		
		//=======================================================================/
		// 参考给定			
		static int16_t DragACCcnt;

		//加速
		if(++DragACCcnt > stru_Test.DeltaCnt)
		{
			DragACCcnt = 0;
			
			if(stru_Test.OmegaE < stru_Test.OmegaEMax)			stru_Test.OmegaE +=1;
			if(stru_Test.OmegaE > stru_Test.OmegaEMax)			stru_Test.OmegaE -=1;	
			
			if(stru_Test.Drag_Ref <= stru_Test.Ref_Final - stru_Test.Delta_Ref)
				stru_Test.Drag_Ref += stru_Test.Delta_Ref;
			else if (stru_Test.Drag_Ref >= stru_Test.Ref_Final + stru_Test.Delta_Ref)
				stru_Test.Drag_Ref -= stru_Test.Delta_Ref;
			else
				stru_Test.Drag_Ref = stru_Test.Ref_Final;
		}
			

		//=======================================================================/
		// DQ电流环计算		
		#if (Config_Test_Mode == VF_Test_Mode)
		{
			stru_Volt_dq.Ud = 0;
			stru_Volt_dq.Uq = stru_Test.Drag_Ref;
		
		}
		#elif (Config_Test_Mode == IF_Test_Mode)
		{
			stru_Curr_dq_ref.Id = 0;
			stru_Curr_dq_ref.Iq = stru_Test.Drag_Ref;
			
			Function_PI_Cal(&PID_ID,stru_Curr_dq_ref.Id,stru_Curr_dq.Id );
			Function_PI_Cal(&PID_IQ,stru_Curr_dq_ref.Iq,stru_Curr_dq.Iq );

			stru_Volt_dq.Ud = PID_ID.Out;
			stru_Volt_dq.Uq = PID_IQ.Out;		
		}
		#endif	

		//=======================================================================/
		// Park逆变换
		stru_Volt_alphabeta.Alpha = ((stru_SinCos.Cos * stru_Volt_dq.Ud)>>15)  -  ((stru_SinCos.Sin * stru_Volt_dq.Uq)>>15);
		stru_Volt_alphabeta.Beta  = ((stru_SinCos.Sin * stru_Volt_dq.Ud)>>15)  +  ((stru_SinCos.Cos * stru_Volt_dq.Uq)>>15);

		//=======================================================================/
		// 计算空间矢量
		#if (Single_Shunt == Config_Shunt_Mode)	
		{		
			FOC_SingleShunt_Calc(&stru_SVM_Calc, stru_Volt_alphabeta);			
		}	
		#else
		{
			FOC_DoubleShunt_Calc(&stru_SVM_Calc, stru_Volt_alphabeta);
		}
		#endif

	}
	#endif
	
	//-----------------------------------------------------------------------/
	//停机
	if(Flag.MC_RunStop == 0)	
	{
		Brige_Output_Off();
		
		MOTOR_STATE 					= MC_INIT;
	}
	
	//=======================================================================/
}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_Control_BLDC_Start
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_Control_BLDC_Start(void)
{
	
	
}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_Control_Switch
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_Control_Switch(void)
{

}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_Control_Run
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_Control_Run(void)
{

}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_Control_Stop
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_Control_Stop(void)
{	

	Brige_Output_Off();

	MOTOR_STATE                   = MC_INIT;
			
}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_Control_Brake
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_Control_Brake(void)
{

	

}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_Control_Process
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_Cur_Sample(void)
{
	#if (Config_Shunt_Mode == Single_Shunt)
	{
		stru_Sample.ADVal[CH_IP_A] = ADC->DATA[ADC_DATA_CHA];
		stru_Sample.ADVal[CH_IP_B] = ADC->DATA[ADC_DATA_CHB];
		
		stru_Sample.ARR[CH_IP_A] = ((stru_Sample.ADVal[CH_IP_A] - stru_Sample.REF[CH_IP_A]) * stru_Sample.G_Ip) >> 10;
		stru_Sample.ARR[CH_IP_B] = ((stru_Sample.ADVal[CH_IP_B] - stru_Sample.REF[CH_IP_B]) * stru_Sample.G_Ip) >> 10;
		
		FOC_SingleCurrent_Calc(stru_SVM_Calc.Sector_Last,stru_Sample.ARR[CH_IP_A],stru_Sample.ARR[CH_IP_B],&stru_Curr_abc);	
		

	}
	#elif (Config_Shunt_Mode == Double_Shunt)
	{
		stru_Sample.ADVal[CH_IP_A] = ADC->DATA[ADC_DATA_CHA];
		stru_Sample.ADVal[CH_IP_B] = ADC->DATA[ADC_DATA_CHB];
		
		stru_Sample.ARR[CH_IP_A] = ((stru_Sample.ADVal[CH_IP_A] - stru_Sample.REF[CH_IP_A]) * stru_Sample.G_Ip) >> 10;
		stru_Sample.ARR[CH_IP_B] = ((stru_Sample.ADVal[CH_IP_B] - stru_Sample.REF[CH_IP_B]) * stru_Sample.G_Ip) >> 10;
	
		stru_Curr_abc.Ia = - stru_Sample.ARR[CH_IP_A];
		stru_Curr_abc.Ib = - stru_Sample.ARR[CH_IP_B];
		stru_Curr_abc.Ic =  stru_Sample.ARR[CH_IP_A] + stru_Sample.ARR[CH_IP_B];
	
	}
	#else
	{
		stru_Sample.ADVal[CH_IP_A] = ADC->DATA[ADC_DATA_CHA];
		stru_Sample.ADVal[CH_IP_B] = ADC->DATA[ADC_DATA_CHB];
		stru_Sample.ADVal[CH_IP_C] = ADC->DATA[ADC_DATA_CHC];		
	
		stru_Sample.ARR[CH_IP_A] = ((stru_Sample.ADVal[CH_IP_A] - stru_Sample.REF[CH_IP_A]) * stru_Sample.G_Ip) >> 10;
		stru_Sample.ARR[CH_IP_B] = ((stru_Sample.ADVal[CH_IP_B] - stru_Sample.REF[CH_IP_B]) * stru_Sample.G_Ip) >> 10;
		stru_Sample.ARR[CH_IP_C] = ((stru_Sample.ADVal[CH_IP_C] - stru_Sample.REF[CH_IP_C]) * stru_Sample.G_Ip) >> 10;
	
		stru_Curr_abc.Ia = - stru_Sample.ARR[CH_IP_A];
		stru_Curr_abc.Ib = - stru_Sample.ARR[CH_IP_B];
		stru_Curr_abc.Ic = - stru_Sample.ARR[CH_IP_C];
	
	}
	#endif
	
	//-----------------------------------------------------------------------/
	// 计算Alpha、Beta电流
	stru_Curr_alphabeta.Alpha = stru_Curr_abc.Ia;	
	stru_Curr_alphabeta.Beta  = (9459 * (stru_Curr_abc.Ia + stru_Curr_abc.Ib*2)>>14);   

	// 计算D、Q电流
	stru_Curr_dq.Id = ((stru_SinCos.Cos * stru_Curr_alphabeta.Alpha)>>15)  + ((stru_SinCos.Sin * stru_Curr_alphabeta.Beta)>>15);  
	stru_Curr_dq.Iq = ((stru_SinCos.Cos * stru_Curr_alphabeta.Beta )>>15)  - ((stru_SinCos.Sin * stru_Curr_alphabeta.Alpha)>>15);
	
	//输入电流采样	
	stru_Sample.ADVal[CH_VBUS] = ADC->DATA[ADC_DATA_VBUS];		
	stru_Sample.ADVal[CH_IBUS] = ADC->DATA[ADC_DATA_IBUS];				
	stru_Sample.ADVal[CH_VCTR] = ADC->DATA[ADC_DATA_CTRL];	

}
	
/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_Control_Process
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_Control_Process(void)
{	
	//=======================================================================//
	// 电流采样	
	FOC_Cur_Sample();
	
	
	// 临时数据处理
//	for(i=0;i<16;i++)
//	{
//		ADCData[i] = ADC_GetResult(i);			
//	}
//	if(ADCData[ADC_DATA_CTRL] > 2048)
//	{
//		Flag.MC_RunStop = 1;
//	}
//	else
//	{
//		Flag.MC_RunStop = 0;
//	}
	//=======================================================================//
	// 电机控制状态机
	switch(MOTOR_STATE)
	{	
		//-------------------------------------------------------------------/	
		// 待机状态
		case MC_INIT:		
			
			FOC_Control_Init();			
		
			break;
		
		//-------------------------------------------------------------------/			
		// 充电状态
		case MC_CHARGE:	
			
			FOC_Control_Charge();	
	
			break;	
		
		//-------------------------------------------------------------------/	
		// 顺逆风状态		
		case MC_WIND:		

			FOC_Control_Wind();
		
			break;		
		
		//-------------------------------------------------------------------/	
		// 预定位状态		
		case MC_ALIGN:		
			
			FOC_Control_Align();
		
			break;
		
		//-------------------------------------------------------------------/	
		// 启动状态		
		case MC_START:
			
			FOC_Control_Start();

			break;
		
		//-------------------------------------------------------------------/	
		// 切换状态		
		case MC_SW:			

			FOC_Control_Switch();
		
			break;
		
		//-------------------------------------------------------------------/	
		// 运行状态			
		case MC_RUN:

			FOC_Control_Run();
		
			break;
		
		//-------------------------------------------------------------------/	
		// 停机状态					
		case MC_STOP:

			FOC_Control_Stop();
		
			break;
		
		//-------------------------------------------------------------------/	
		// 刹车状态			
		case MC_BRAKE:
			
			FOC_Control_Brake();
		
			break;
		
		//-------------------------------------------------------------------/		
		// 故障状态
		case MC_FAULT:


			break;

		//-------------------------------------------------------------------/	

		default:
			break;		
	}	
	//=======================================================================//
	
	//UartView(mcpwm0_callback_cnt+1000, mcpwm0_callback_cnt+2000, mcpwm0_callback_cnt+3000, mcpwm0_callback_cnt+4000);
}

//===========================================================================//
//*********END   ************************************************************//
//===========================================================================//

