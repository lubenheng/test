//===========================================================================//
/*****************************************************************************
*-----------------------------------------------------------------------------
* @file    interrupt.c
* @author  CMS Motor Control Team
* @version 第二代风机平台
* @date    2023年2月
* @brief   
*---------------------------------------------------------------------------//
*****************************************************************************/
//===========================================================================//

//---------------------------------------------------------------------------/
//	include files
//---------------------------------------------------------------------------/
#include "mcu_driver.h"
#include "motor_driver.h"
#include "user.h"

//---------------------------------------------------------------------------/
//	Local pre-processor symbols/macros('#define')
//---------------------------------------------------------------------------/
struct {
    s16 data0;
    s16 data1;
    s16 data2;
    s16 data3;
    s16 data4;
    s16 data5;
} Rttstru;
//---------------------------------------------------------------------------/
//	Local variable  definitions
//---------------------------------------------------------------------------/

//---------------------------------------------------------------------------/
//	Global variable definitions(declared in header file with 'extern')
//---------------------------------------------------------------------------/

//---------------------------------------------------------------------------/
//	Local function prototypes('static')
//---------------------------------------------------------------------------/
void FOC_User_Control(void);

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : EPWM_IRQHandler
* Description    : R1:30us/64M(2~3us_week)(2us_comm)
* Function Call  : EPWM_IRQHandler中断函数
* Input Paragram :
* Return Value   : 31us 单电阻
*-----------------------------------------------------------------------------
******************************************************************************/
void EPWM_IRQHandler(void)
{  
	#if ( Speed_Control_Mode == Commu_Control )
	UART0->END = 0x0;	
	#endif
	//-----------------------------------------------------------------------/	
	// 清除标志
	EPWM_ClearZIFn_Flag();

	//-----------------------------------------------------------------------/	
	// 电流读取
	stru_Sample.ADVal[CH_IP_A] = (uint32_t)ADC->DATA[ADC_DATA_CHA];
	stru_Sample.ADVal[CH_IP_B] = (uint32_t)ADC->DATA[ADC_DATA_CHB];	
	FOC_SingleCurrent_Calc();

	stru_Sample.ADVal[CH_VBUS] = (uint32_t)ADC->DATA[ADC_DATA_VBUS];
	Basic.Vbus								 = FRACMPY_Q10( stru_Sample.ADVal[CH_VBUS] , stru_Sample.G_Vb );

	//-----------------------------------------------------------------------/	
	// FOC控制流程
	FOC_Control_Process();

	//-----------------------------------------------------------------------/	
	// 用户进程
	FOC_User_Control();		
	//---------------------------------- -------------------------------------/	
	
}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : ADC_IRQHandler
* Description    : 
* Function Call  : ADC_IRQHandler中断函数
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void ADC_IRQHandler(void)
{
	#if ( Speed_Control_Mode == Commu_Control )
	UART0->END = 0x0;	
	#endif
	//-----------------------------------------------------------------------/		
	// ADC采样完成中断
	if(ADC->MIS & ADC_SCAN_CHA)
	{
		// 清除中断标志
		ADC_ClearIntFlag_CHA();
	
		// 电流读取
		FOC_Read_Current();
		
		// FOC控制流程
		FOC_Control_Process();	
		
		// 用户进程
		FOC_User_Control();
		
	}
	//清所有中断标志
	else
	{
		ADC_ClearAllInt_Flag();
	}		
}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : ACMP_IRQHandler
* Description    :
* Function Call  : 故障刹车比较器中断
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void ACMP_IRQHandler(void)
{
	#if ( Speed_Control_Mode == Commu_Control )
	UART0->END = 0x0;	
	#endif
	//-----------------------------------------------------------------------/		
	// 清除中断标志 
	ACMP_ClearIntFlag( ACMP1 );
	
	// 关闭输出 
	User_Motor_Off();
	
	// 短路标志置位
	Fault_ShortCircuit = 1;
	
	// 参数初始化
	FOC_ControlPara_Init();
}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : SysTick_Handler
* Description    :
* Function Call  : SysTick_Handler中断
* Input Paragram : 140us
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void SysTick_Handler(void)
{	
	#if ( Speed_Control_Mode == Commu_Control )
	UART0->END = 0x0;	
	#endif	
	//--------------------------------------------------------------------------/
	SysTick->CTRL|=SysTick_CTRL_COUNTFLAG_Msk;
	
	TimeFlag_1ms = 1;

	//--------------------------------------------------------------------------/
	// 电机故障检测 	
	MC_MotorFault_Check();
	
	//--------------------------------------------------------------------------/
	// 电机初始状态任务
	MC_MotorInit_Process();
	
	//--------------------------------------------------------------------------/
	// 电机转速计算 
	MC_MotorSpeed_Calc();
	
	//--------------------------------------------------------------------------/	
	// 电机调速模块控制 
	MC_Speed_Control();

	//--------------------------------------------------------------------------/	
	// 查询捕获完成标志位
	if((CCP->RIS & (0x1UL<<(CAP3+8)))? 1:0)
	{
		Capture.OverFlowFlag = 0;
		Capture.OverFlowTime = 0;
		CCP_ClearCAPMode2IntFlag(CAP3);
		Capture.CompleteFlag = 1; //置捕获完成标志;
	}

	//--------------------------------------------------------------------------/
	// 时序相关处理 
	if(Time.Charge_Count > 0)      	Time.Charge_Count --;
	if(Time.Motor_PowerDown > 0)    Time.Motor_PowerDown --;

	//--------------------------------------------------------------------------/		
}

/****************************************************************************
 ** \brief	TIMER1_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void TIMER1_IRQHandler(void)
{
	#if ( Speed_Control_Mode == Commu_Control )
	UART0->END = 0x0;	
	#endif
	//-----------------------------------------------------------------------/		
	TMR1->ICLR = TMR_ICLR_ICLR_Msk;
	if( MOTOR_STATE == MC_START )
	{
		//--------------------------------------------------------------------------/
		if( stru_BLDC_BEMF.bTIM1State == 0 )  //找BEMF过零点
		{
			TMR1->LOAD                = 0xFFFFFFFF;
			stru_BLDC_BEMF.bTIM1State = 1;
		}	
		//--------------------------------------------------------------------------/
		else if( stru_BLDC_BEMF.bTIM1State == 2 )  //AD检测过零或者强制换相 发生后产生中断
		{
      stru_BLDC_BEMF.bIndexNow = bBLDCIndexNextArr[stru_BLDC_BEMF.bIndexNow];		
			EPWM->LOCK   = EPWM_LOCK_P1B_WRITE_KEY;			
			if(Config.MotorDir == FR_CW)
				EPWM->MASK = EPWM_MaskArr_CW[stru_BLDC_BEMF.bIndexNow]; 
			else		
				EPWM->MASK = EPWM_MaskArr_CCW[stru_BLDC_BEMF.bIndexNow]; 		
			EPWM->LOCK   = 0;
      
      if(stru_BLDC_BEMF.wMaskTime > stru_BLDC_BEMF.wMaskTimeMin)
      {
        stru_BLDC_BEMF.wMaskTime = (stru_BLDC_BEMF.wMaskTime*665)>>10; // 0.65
      }
      
      if(stru_BLDC_BEMF.wMaskTimeInit < stru_BLDC_BEMF.wMaskTimeMin)
      {
        TMR1->LOAD = stru_BLDC_BEMF.wMaskTimeMin; // 延时500us，避开续流点检测BEMF过零点
      }
      else
      {
        TMR1->LOAD = stru_BLDC_BEMF.wMaskTime;
      }
     
			stru_BLDC_BEMF.hForceSwitchCount = 0;
			stru_BLDC_BEMF.bSwitchPhaseFlag  = 0;
			stru_BLDC_BEMF.bTIM1State        = 0;
			
		}
	}
  //--------------------------------------------------------------------------/  
}



/****************************************************************************
 ** \brief	UART0_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
** \note: Rx :0xFA 0x5E OnOff PowerH	PowerL 0xBD

****************************************************************************/
void UART0_IRQHandler(void)
{
//	stru_CommData.RxBuff[stru_CommData.RxOder] = (u8)UART0->RBR ;
//	stru_CommData.IR_Flag = 1;
//	UART0->END = 0x0;

//		if(stru_CommData.RxOder == 0)
//		{
//			// 开始接收新数据 清除接收完成标志，开始下一步
//			if(stru_CommData.RxBuff[stru_CommData.RxOder] == RxData_Haeder1)	
//			{
//				stru_CommData.RxOder ++;
//				stru_CommData.RxFlag = 0;
//			}			
//		}
//		else if (stru_CommData.RxOder == 1)
//		{
//			if(stru_CommData.RxBuff[stru_CommData.RxOder] == RxData_Haeder2)			
//			{	stru_CommData.RxOder ++;}
//			// 标志字节异常，清零
//			else
//			{	stru_CommData.RxOder = 0;			}
//		}
//		else if(stru_CommData.RxOder == User_Comm_RXLength -1)
//		{
//			// 接收完成，清零，标志置位
//			if(stru_CommData.RxBuff[5] == RxData_END)
//			{
//				stru_CommData.RxOder = 0;
//				stru_CommData.RxFlag = 1;
//				stru_CommData.RxData[2] = stru_CommData.RxBuff[2];
//				stru_CommData.RxData[3] = stru_CommData.RxBuff[3];
//				stru_CommData.RxData[4] = stru_CommData.RxBuff[4];			
//			}
//			else
//			{
//				stru_CommData.RxOder = 0;	
//			}
//		}
//		else
//		{
//			stru_CommData.RxOder ++;
//		}
		
}
/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : UART_VIEW
* Description    : 串口数据打印
* Function Call  : 
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void UART_VIEW(void)
{
  //UartView(stru_VoltComp_alphabeta.Alpha,stru_VoltComp_alphabeta.Beta,stru_Volt_alphabeta.Alpha,stru_Volt_alphabeta.Beta);
}
void RTT_VIEW(s16 view1,s16 view2,s16 view3)
{
    Rttstru.data0 = view1;
    Rttstru.data1 = view2;   //stru_bemf.Speed
    Rttstru.data2 = view3;

}
/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_User_Control
* Description    : 用户操作模块
* Function Call  : 
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/

void FOC_User_Control(void)
{
	#if (UART_CONFIG_MODE == UART_ON)
	{
		//UartView(stru_VoltComp_alphabeta.Alpha,stru_VoltComp_alphabeta.Beta,stru_Volt_alphabeta.Alpha,stru_Volt_alphabeta.Beta);
	}
	#endif	
	
	#if (UART_CONFIG_MODE == RTT_ON)
	{
		RTT_VIEW(stru_FOC.Elec_Angle,Basic.Mech_Speed >> 3,stru_Curr_abc.Ia);
    SEGGER_RTT_Write(1, &Rttstru, 6);
	}
	#endif	
	
	// 串口数据发送
	#if ( Speed_Control_Mode == Commu_Control )
	{
		Uart_Rx_React();
		Uart_Tx_React();

	}
	#endif
}

/******************************** END OF FILE *******************************/
