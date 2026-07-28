//==========================================================================//
/*****************************************************************************
*-----------------------------------------------------------------------------
* @file    mc_foc.c
* @author  CMS Motor Control Team
* @version 第二代风机平台
* @date    2023年2月
* @brief
*---------------------------------------------------------------------------//
*****************************************************************************/
//==========================================================================//

//---------------------------------------------------------------------------/
//	include files
//---------------------------------------------------------------------------/
#include "CMSDK_CM0.h"

#include "mcu_driver.h"
#include "motor_driver.h"
#include "user.h"
#include "user_control.h"
#define MOTOR_IF       (1)	//IF强拖
#define MOTOR_CLOSE    (0)	//闭环
#define MOTOR_RUN_STATE		(MOTOR_IF)
//---------------------------------------------------------------------------/
//	Local pre-processor symbols/macros('#define')
//---------------------------------------------------------------------------/

//---------------------------------------------------------------------------/
//	Local variable  definitions
//---------------------------------------------------------------------------/
//PID_Components        		PID_ID 								= {0};  /* D轴电流环 */

//PID_Components        		PID_IQ 								= {0};	/* Q轴电流环*/

//PID_Components        		PID_WL 								= {0};	/* 速度/功率外环*/

//struct_SinCos	  				stru_SinCos 						= {0};	/* SinCos计算结构体 */

//struct_Curr_abc		 		stru_Curr_abc 						= {0};	/* 马达abc三相电流 */

//struct_Clark	  				stru_Curr_alphabeta 				= {0};	/* alpha、beta坐标系电流 */

struct_Park		  			stru_Curr_dq 						= {0};	/* d、q坐标系电流 */

struct_Park		  			stru_Curr_dq_ref 					= {0};	/* d、q坐标系参考电流 */

//struct_RevPark	  			stru_Volt_alphabeta 				= {0};	/* alpha、beta坐标系电压 */

//struct_Volt_dq	  			stru_Volt_dq 						= {0};	/* d、q坐标系电压 */

//struct_Arctan     			stru_Arctan 						= {0};  /* 反正切计算 */

//struct_SVPWM        			stru_SVM_Calc 						= {0}; 	/* 空间矢量结构体 */

//struct_SinCos	  				stru_UdqSinCos 						= {0};	/* SinCos计算结构体 */

//struct_RevPark	  			stru_VoltComp_alphabeta 			= {0};	/* alpha、beta坐标系电压 */

//struct_PI						stru_PI_ID						    = {0};	/* D轴电流环PI */

//struct_PI						stru_PI_IQ 							= {0};  /* Q轴电流环PI */

//---------------------------------------------------------------------------/
//	Global variable definitions(declared in header file with 'extern')
//---------------------------------------------------------------------------/
volatile uint16_t       hBrakeDuty            = 0;		/* 刹车占空比 */
volatile uint32_t       wFOCBrakeCount        = 0;
volatile uint32_t       wSwitchHoldTime       = 0;
uint8_t                 bFristFlag            = 0;
uint32_t                wMotorLockSwitchTime  = 0;
uint8_t                 bBLDCSwitchFlag       = 0;
uint8_t                 bBLDCFristFlag        = 0;

extern struct_smo_est               		   stru_smo_est;
extern struct_SinCos						   stru_SinCos_PLL; /* 三角函数计算 */
extern const uint16_t Q15Sqrt_Table[677];
//---------------------------------------------------------------------------/
//	Local function prototypes('static')
//---------------------------------------------------------------------------/
//extern const uint16_t 	Q15Sqrt_Table[677];
//extern const int16_t 		Sin_Cos_Table[256];
//extern s16 VoltDropTh;
//===========================================================================/
//***** definitions  end ****************************************************/
//===========================================================================/

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_Start_Init
* Description    :
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   :
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_Start_Init(void)
{


}



/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_ControlPara_Init
* Description    :
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   :
*-----------------------------------------------------------------------------
******************************************************************************/
extern uint16_t g_smo_speed_short;
void FOC_ControlPara_Init(void)
{

    //-------------------------SW------------------
    //无扰切换角度
    stru_Para.Sw.delta_theta_openloop_to_smo = 0;
    //速度误差小于阈值计数
    stru_Para.Sw.sw_to_run_cnt = 0;

    stru_Para.Sw.HoldTime = 200;
    //对START状态的强拖角度赋初值
    stru_FOC_Start.Elec_Omega = MC_Start_Init_Speed * EPWM_FREQ >> 16;

    //补偿角初始值置0
    stru_Para.Run.Compensation = 0;

    Sw_to_Transition_Flag = 0;

    Transition_to_Run_Flag = 0;

    Start_to_Sw_Flag = 0;

    g_smo_speed_short = 0;
	
	stru_FOC.Ud = 0;
	stru_FOC.Uq = 0;
	stru_FOC.theta = 0;
	stru_FOC.observer_flag = 0; 
	stru_FOC.open_loop_omega = 0;
	stru_FOC.Vs_Square_Max = _Q15(MAX_VOLTAGE_VECTOR*MAX_VOLTAGE_VECTOR);
	stru_FOC.theta_comp = 0; //补偿角为0
	
	//----------------------------------------------------------------------------/
	stru_Curr_dq_ref.Id = 0;
	stru_Curr_dq_ref.Iq = Phy2Pu_Fun_Iphase(MC_Start_Curr);
	
	
	
	
    // 启动观测器参数初始化
    FOC_Start_Init();

    // PID参数初始化
    //FOC_PID_Parameter_Init();

    //位置观测器初始化
    FOC_RotorPosition_Init();

    // 反电动势参数初始化
    //FOC_BEMF_Init();

    // 单电阻初始化
    //FOC_SingleShunt_Init();

    //------------------------------------------------------------------------/
    //Brige_Output_Off(); //关闭桥臂输出




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
void FOC_Read_Current(void)
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
    //------------------------------------------------------------------------/
    Brige_Output_Off(); //关闭桥臂输出

    //-----------------------------------------------------------------------//
    // 参数初始化
    FOC_ControlPara_Init();

    // FOC模块使能
    DRV->cr &= ~BIT5;
    DRV->cr |= BIT5;
//	//-----------------------------------------------------------------------//
//	// 启动
//	#if (VBUS_PROTECT_ENABLE )
//	if(  Flag.MC_RunStop 																	 \
//		&& Basic.Vbus_Check > stru_Fault.Vbus.Start_UV_Value \
//		&& Basic.Vbus_Check < stru_Fault.Vbus.Start_OV_Value \
//		&& (SYSTEM_STATE == SYS_RUN))
//	#else
//	if((Flag.MC_RunStop) && (SYSTEM_STATE == SYS_RUN))
//	#endif

    //默认角度为0
    FOC->angl = 0;

    // 配置加速度
    FOC->ramp1 = 0; //+144;

    //电流赋值
    FOC->Id_req = 0;

    //电压赋值
    FOC->Ud = 0;

    if(Flag.MC_RunStop == 1)
    {
        Brige_Output_On();
        MOTOR_STATE = MC_ALIGN;
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

    //MOTOR_STATE = MC_ALIGN;

    //电流赋值
    FOC->Id_req = 0;

    //电压赋值
    FOC->Ud = 0;

    //=======================================================================/
    // 停机
    if( Flag.MC_RunStop == 0 )
    {
        MOTOR_STATE = MC_STOP;
    }
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




    //=======================================================================/
    // 停机
    if( Flag.MC_RunStop == 0 )
    {
        MOTOR_STATE = MC_STOP;
    }
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

    //=======================================================================/
    // 停机
    if( Flag.MC_RunStop == 0 )
    {
        MOTOR_STATE = MC_STOP;
    }
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
    FOC->angl = MC_ALIGN_ANGLE;

    // 配置加速度
    FOC->ramp1 = 0; //+144;

    //id电流参考值
    stru_FOC.Idq_ref.half.id =
        (stru_FOC.Idq_ref.half.id > Phy2Pu_Fun_Iphase(MC_ALIGN_D_AXIS_CURRENT)) ? (stru_FOC.Idq_ref.half.id - 1) :
        ((stru_FOC.Idq_ref.half.id < Phy2Pu_Fun_Iphase(MC_ALIGN_D_AXIS_CURRENT)) ? (stru_FOC.Idq_ref.half.id + 1) : Phy2Pu_Fun_Iphase(MC_ALIGN_D_AXIS_CURRENT));


    //iq电流参考值
    stru_FOC.Idq_ref.half.iq = 0;

    //电流赋值
    FOC->Id_req = stru_FOC.Idq_ref.word;

    stru_Para.Align.Cnt++;

    if( (stru_FOC.Idq_ref.half.id == Phy2Pu_Fun_Iphase(MC_ALIGN_D_AXIS_CURRENT)) &&
            (stru_Para.Align.Cnt >= MC_ALIGN_TIME) )
    {
        stru_Para.Align.Cnt = 0;

        /* 预定位在0度 */
        FOC->angl = 0;
			
		
		//直接使用电压开环给定
//		FOC->ctrl0 |= BIT0;
//		
//		FOC->ctrl0 |= BIT1;

        //切换到START状态
        MOTOR_STATE = MC_START;
    }

    //=======================================================================/
    // 停机
    if( Flag.MC_RunStop == 0 )
    {
        Brige_Output_Off();

        MOTOR_STATE = MC_STOP;
    }
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
uint8_t Start_to_Sw_Flag = 0;
void FOC_Control_Start(void)
{
	static u16 AccCnt = 0;
	
//	stru_FOC.Ro_Ualpha_beta.word = FOC->ro_Ualf;

//	
//	//---------------------------------------------------------------------------------------------------/
//	//获取采样电流
//	stru_FOC.Ro_Iuv.word = FOC->ro_Iuv;

	if(stru_FOC.observer_flag == 0)
	{
//			stru_FOC.theta += stru_FOC.open_loop_omega;
//			FOC->angl = stru_FOC.theta;
					//加速处理
			if(++AccCnt > stru_Para.Start.ACCCycle)
			{
					AccCnt = 0;

					if(stru_FOC_Start.Elec_Omega < stru_Para.Start.OmegaMax)	stru_FOC_Start.Elec_Omega ++;

					if(stru_FOC_Start.Elec_Omega > stru_Para.Start.OmegaMax)	stru_FOC_Start.Elec_Omega --;
			}

			
			// 角度处理
			stru_FOC.open_loop_omega = stru_FOC_Start.Elec_Omega;
			FOC->ramp1 = stru_FOC_Start.Elec_Omega;

			//id电流参考值
			stru_FOC.Idq_ref.half.id = (stru_Curr_dq_ref.Id>>1);
			stru_FOC.Idq_ref.half.iq = (stru_Curr_dq_ref.Iq>>1);
				
			//电流赋值
			FOC->Id_req = stru_FOC.Idq_ref.word;
		
	}

	
	stru_FOC.Idq_fbk.word = FOC->ro_Id;
	stru_FOC.Udq_fbk.word = FOC->ro_Ud;
	
#if 1 


		

    if( ((uint16_t)stru_FOC_Start.Elec_Omega) == ((uint16_t)stru_Para.Start.OmegaMax) )
    {
        #if(HIGH_SPEED_DEBUG_CONFIG == HIGH_SPEED_DEBUG_ENABLE)

//        if(Start_to_Sw_Flag == 1)
        #endif
//        {		
						#if(MOTOR_RUN_STATE==MOTOR_CLOSE)
            MOTOR_STATE = MC_SW;
						#endif
//        }

    }
#endif
	



    //=======================================================================/
    // 停机
    if( Flag.MC_RunStop == 0 )
    {
        MOTOR_STATE = MC_STOP;
    }

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
volatile u8 Swflag;
uint8_t Sw_to_Transition_Flag = 0;
void FOC_Control_Switch(void)
{
    #if (HIGH_SPEED_DEBUG_CONFIG == HIGH_SPEED_DEBUG_ENABLE)

    if (Sw_to_Transition_Flag == 1)
    #endif
    {
        Sw_to_Transition_Check();
    }

    //=======================================================================/
    // 停机
    if( Flag.MC_RunStop == 0 )
    {
        MOTOR_STATE = MC_STOP;
    }
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
volatile u8 Swflag;
uint8_t Transition_to_Run_Flag = 0;
void FOC_Control_Transition(void)
{
    //iq电流参考值
//	 stru_FOC.Idq.half.iq_ref =
//     (stru_FOC.Idq.half.iq_ref > Phy2Pu_Fun_Iphase(MC_Transition_Current)) ? (stru_FOC.Idq.half.iq_ref - 1) :
//     ((stru_FOC.Idq.half.iq_ref < Phy2Pu_Fun_Iphase(MC_Transition_Current)) ? (stru_FOC.Idq.half.iq_ref + 1) : Phy2Pu_Fun_Iphase(MC_Transition_Current));
//
    //id电流参考值
    stru_FOC.Idq_ref.half.id = 0;

    //电流赋值
    FOC->Id_req = stru_FOC.Idq_ref.word;
    #if(HIGH_SPEED_DEBUG_CONFIG == HIGH_SPEED_DEBUG_ENABLE)

    if (Transition_to_Run_Flag == 1U)
    #endif
    {
        Transition_to_Run_Check();
    }

    //=======================================================================/
    // 运行状态检测、关机
    if( Flag.MC_RunStop == 0 )
    {
        //切换到开环启动
        FOC->ctrl0 |= BIT2;

        MOTOR_STATE = MC_STOP;
    }
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
uint16_t ADC_sw_flag = 0;

void FOC_Control_Run(void)
{

    static u16 Comp_Cnt = 0;

    // 补偿角延时处理
    if (++Comp_Cnt > MC_RUN_COMPENSATION_CNT)
    {
        Comp_Cnt = 0;

        // 补偿角参考值
        stru_Para.Run.Compensation =
            (stru_Para.Run.Compensation > stru_Para.Run.theta_compensation_tmp) ? (stru_Para.Run.Compensation - 2) :
            ((stru_Para.Run.Compensation < stru_Para.Run.theta_compensation_tmp) ? (stru_Para.Run.Compensation + 2) : stru_Para.Run.theta_compensation_tmp);

        stru_Para.Run.theta_compensation_final = (uint16_t)(stru_Para.Run.Compensation);

        // iq电流参考值
       // stru_FOC.Idq_ref.half.iq = stru_PI_SpdLoop.Out;
			 stru_FOC.Idq_ref.half.iq = 
          (stru_FOC.Idq_ref.half.iq > stru_Para.Run.Iq_Final_Curr) ? (stru_FOC.Idq_ref.half.iq - 1) : ((stru_FOC.Idq_ref.half.iq < stru_Para.Run.Iq_Final_Curr) ? (stru_FOC.Idq_ref.half.iq + 1) : stru_Para.Run.Iq_Final_Curr);

        // id电流参考值
        // stru_FOC.Idq.half.id_ref = 0;
        stru_Para.Run.Id_Final_Curr =
            (stru_Para.Run.Id_Final_Curr > stru_Para.Run.Id_req) ? (stru_Para.Run.Id_Final_Curr - 1) : ((stru_Para.Run.Id_Final_Curr < stru_Para.Run.Id_req) ? (stru_Para.Run.Id_Final_Curr + 1) : stru_Para.Run.Id_req);

        stru_FOC.Idq_ref.half.id = (uint16_t)(stru_Para.Run.Id_Final_Curr);

        // 电流赋值
        FOC->Id_req = stru_FOC.Idq_ref.word;

        // 补偿角给定
        // 高16bit 保持为0；低16bit 赋值
        FOC->angl_comp = (0x0 << 16) | (stru_Para.Run.theta_compensation_final << 0);

    }


    //=======================================================================/
    // 运行状态检测、关机
    if( Flag.MC_RunStop == 0 )
    {
        //切换到开环启动
        FOC->ctrl0 |= BIT2;

        MOTOR_STATE = MC_STOP;
    }
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

    MOTOR_STATE = MC_INIT;
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
    // 开下管刹车
    Brige_Output_Down();
//	if( stru_FOC.Elec_We > 0 )
//	{
//		stru_FOC.Elec_We --;
//	}
//	else
//	{
//		stru_FOC.Elec_We = 0;
//	}
//	// 刹车结束
//	if( Time.Motor_PowerDown == 0 )
//	{
//		stru_FOC.Elec_We = 0;
    MOTOR_STATE = MC_INIT;
//	}
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
uint16_t smo_ek_Cnt, smo_ek_SUM=10, smo_ek_temp, smo_ek_sw_flag = 0;

void FOC_Control_Process(void)
{
		stru_FOC.Ro_Ualpha_beta.word = FOC->ro_Ualf;
	
		stru_FOC.Ro_Iuv.word = FOC->ro_Iuv;
	
	//---------------------------------------------------------------------------------------------------/
	//获取采样电流
	
    //=======================================================================//
    // 电机控制状态机
    switch(MOTOR_STATE)
    {
        //-------------------------------------------------------------------/
        // 待机状态 0
        case MC_INIT:

            FOC_Control_Init();

            break;

        //-------------------------------------------------------------------/
        // 充电状态 1
        case MC_CHARGE:



            FOC_Control_Charge();

            break;

        //-------------------------------------------------------------------/
        // 顺逆风状态 2
        case MC_WIND:

            FOC_Control_Wind();

            break;

        //-------------------------------------------------------------------/
        // 预定位状态 3
        case MC_ALIGN:

            FOC_Control_Align();

            break;

        //-------------------------------------------------------------------/
        // 启动状态	4
        case MC_START:

            #if ( BLDC_START_ENABLE )
            FOC_Control_BLDC_Start();
            #else
            FOC_Control_Start();
            #endif

            break;

        //-------------------------------------------------------------------/
        // 切换状态	 5
        case MC_SW:

            FOC_Control_Switch();

            break;

        //-------------------------------------------------------------------/
        // 切换状态	6
        case MC_TRANSITION:

            FOC_Control_Transition();

            break;

        //-------------------------------------------------------------------/
        // 运行状态	7
        case MC_RUN:

            FOC_Control_Run();

            break;

        //-------------------------------------------------------------------/
        // 停机状态	 8
        case MC_STOP:

            FOC_Control_Stop();

            break;

        //-------------------------------------------------------------------/
        // 刹车状态	 9
        case MC_BRAKE:

            FOC_Control_Brake();

            break;

        //-------------------------------------------------------------------/
        // 故障状态 10
        case MC_FAULT:

            Brige_Output_Off();
//			if( Time.Motor_PowerDown == 0 )
//			{
//				Basic.Mech_Speed = 0;
//				MOTOR_FAULT_CODE = FAULT_STARTUNDERVOLTAGE;
//				Fault_UnderVoltage = 1;
//				MOTOR_STATE      = MC_INIT;
//			}

            break;

        //-------------------------------------------------------------------/

        default:
            break;
    }

    //=======================================================================//
}

//===========================================================================//
//*********END   ************************************************************//
//===========================================================================//

