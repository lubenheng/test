//===========================================================================//
/*****************************************************************************/
//---------------------------------------------------------------------------//
/** \file
*  File Name  : user_control.h
*  Author     : CMS Motor Control Team
*  Version    : 第二代风机软件平台
*  Date       : 2023年2月
** Description: 本页参数为调速电机控制常用参数。
**              该版本适用于吸尘器/扫地宝等产品。
//---------------------------------------------------------------------------//
******************************************************************************/
//===========================================================================//

#ifndef  __USER_CONTROL_H__
#define  __USER_CONTROL_H__

//===========================================================================//
// 代码对齐方式：Edit--Configuration--Editor--C/C++Files--Tab size = 2
//---------------------------------------------------------------------------//

// 硬件：  CMS32M6534_XX 吸尘器板
// 软件：  CMS32M6534QFN40_V100_20230512
// 电机库：CMS_65_FOCB_V100
#include "base_define.h"

//
#define Cfg_emf_coef_min                   (368) 


//ALIAN_CONFIG配置为1时，使能预定位；ALIAN_CONFIG配置为低电平时，关闭预定位
#define ALIAN_ENABLE		 		                (1)
#define ALIAN_DISABLE 		                	(0)
#define ALIAN_CONFIG 			                  (ALIAN_ENABLE)


//是否开启高速模式
#define HIGH_SPEED_MODE_ENABLE		 	        (1)
#define HIGH_SPEED_MODE_DISABLE 		        (0)
#define HIGH_SPEED_MODE_CONFIG 		          (HIGH_SPEED_MODE_DISABLE)

//是否开启电压折算，开启时Ud/Ud输出最大值为FOC->Udc，不开启时为32767.
#define UDC_CONVERT_ENABLE		            	(1)
#define UDC_CONVERT_DISABLE 		            (0)
#define UDC_CONVERT_CONFIG 			            (UDC_CONVERT_DISABLE)


//是否开启电压折算，开启时Ud/Ud输出最大值为FOC->Udc，不开启时为32767.
#define HIGH_SPEED_DEBUG_ENABLE		      	  (1)
#define HIGH_SPEED_DEBUG_DISABLE 		      (0)
#define HIGH_SPEED_DEBUG_CONFIG 		      (HIGH_SPEED_DEBUG_DISABLE)

//===========================================================================//
/****************************<< 1、调速参数设定 >>****************************/
//===========================================================================//

//---------------------------------------------------------------------------//
/****************************<< 调速方式选择 >>*******************************/
#define  FAST_Control                       (0)                      // 上电直接运行
#define  PWM_Control                        (1)                      // PWM占空比信号调速
#define  PWM_AD_Control                     (2)                      // PWMAD信号调速
#define  Debug_Control                      (3)                      // 调试用，给固定值
#define  Button_Control                     (4)                      //按键调速模式
#define  Commu_Control                      (5)                      //串口通信调速
#define  Speed_Control_Mode                 (Debug_Control)

//---------------------------------------------------------------------------//
/****************************<< 目标值设定 >>*********************************/
// 设定功率或转速值
#define  TARGET_MIN                         (25)                     // 最小运行功率 单位：W
#define  TARGET_MAX                         (300)                    // 最大运行功率 单位：W

//----------------------------------按键设定-----------------------------------/
#define  LV1_POWER                          (60)            				// 1挡功率
#define  LV2_POWER                          (200)           				// 2挡功率
#define  LV3_POWER                          (200)           				// 3档功率

#define  GEAR_OFF							(0)											 // 关机档位
#define  GEAR_1								(1)											 // 1档
#define  GEAR_2								(2)											 // 2档
#define  GEAR_3								(3)											 // 3档

#define  START_BUTTON_LV                    (GEAR_1)              //按键控制模式下上电初始档位
#define  MAX_BUTTON_LV                    	(GEAR_2)              //按键控制模式最大档位
//---------------------------------------------------------------------------//
/****************************<< 串口调速设置 >>*********************************/
#define	RxData_Haeder1						(0xFA)									// 串口帧头1
#define	RxData_Haeder2						(0x5E)									// 串口帧头2
#define	RxData_END								(0xBD)									// 串口帧尾
#define User_Comm_TXLength					(15)										// 发送数据长度
#define User_Comm_RXLength					(6)											// 接收数据长度
//---------------------------------------------------------------------------//
/****************************<< PWM信号输入逻辑 >>****************************/
#define  PWM_POSITIVE_LOGIC                 (0)                      // 正占空比输入
#define  PWM_NEGATIVE_LOGIC                 (1)					             // 负占空比输入
#define  PWM_INPUT_MODE                     (PWM_POSITIVE_LOGIC)

//---------------------------------------------------------------------------//
/****************************<< PWM调速占空比参数 >>**************************/
#define  PWM_DUTY_STOP                      (70)                     // 停止占空比 单位：0.1%
#define  PWM_DUTY_START                     (100)                    // 启动占空比 单位：0.1%
#define  PWM_DUTY_MAX                       (1000)                    // 最大占空比 单位：0.1%

//---------------------------------------------------------------------------//
/****************************<< PWM_AD电压调速参数 >>*************************/
#define  PWM_AD_STOP                        (4.8)                    // 停止电压 单位：V
#define  PWM_AD_START                       (4.5)                    // 启动电压 单位：V
#define  PWM_AD_MAX                         (0.2)                    // 最大电压 单位：V

//===========================================================================//
/****************************<< 2、电机及硬件参数设定 >>**********************/
//===========================================================================//

//---------------------------------------------------------------------------//
/****************************<< 电机参数 >>***********************************/
#if (0)  //12V落地扇
#define  MOTOR_SPEED_MAX                    (4000) 					         // 电机最大机械转速 单位：rpm
#define  MOTOR_PAIRS                        (5)                      // 电机极对数 单位：Poles
#define  MOTOR_RS                           (1.913)                  // 电机相电阻 单位：Ω
#define  MOTOR_LS                           (0.0014)                 // 电机相电感 单位：H
#define  K_EMF                      	    	(3.564)				           // 电机相电感 单位：H
#endif

#if (0)  //24V Nedic落地扇 20250922 轻 浦东
#define  MOTOR_SPEED_MAX                    (4000) 					         // 电机最大机械转速 单位：rpm
#define  MOTOR_PAIRS                        (5)                      // 电机极对数 单位：Poles
#define  MOTOR_RS                           (1.95)                   // 电机相电阻 单位：Ω
#define  MOTOR_LS                           (0.00135)                // 电机相电感 单位：H
#define  K_EMF                      	      (4.31)				            // 反电动势常数EMF_factor (V/Krpm)
#endif

#if (0)  //24V Nedic落地扇 20251022 重 浦东
#define  MOTOR_SPEED_MAX                    (8000) 					         // 电机最大机械转速 单位：rpm
#define  MOTOR_PAIRS                        (5)                       // 电机极对数 单位：Poles
#define  MOTOR_RS                           (0.260)                   // 电机相电阻 单位：Ω
#define  MOTOR_LS                           (0.000455)                // 电机相电感 单位：H
#define  K_EMF                      	      (4.31)				            // 反电动势常数EMF_factor (V/Krpm)
#endif

#if (1)  //24V 美的落地扇 散装
#define  MOTOR_SPEED_MAX                    (EPWM_FREQ) 					        // 电机最大机械转速 单位：rpm
#define  MOTOR_PAIRS                        (5)                       // 电机极对数 单位：Poles
#define  MOTOR_RS                           (1.5)                     // 电机相电阻 单位：Ω
#define  MOTOR_LS                           (0.00325)                 // 电机相电感 单位：H
#define  K_EMF                      	    (4.310)				            // 反电动势常数EMF_factor (V/Krpm)
#endif

#if (0)  //12V 擦窗电机
#define  MOTOR_SPEED_MAX                    (240000) 					         // 电机最大机械转速 单位：rpm
#define  MOTOR_PAIRS                        (1)                      // 电机极对数 单位：Poles
#define  MOTOR_RS                           (0.172)                   // 电机相电阻 单位：Ω
#define  MOTOR_LS                           (0.000040)                // 电机相电感 单位：H
#define  K_EMF                      	    	(4.31)				            // 反电动势常数EMF_factor (V/Krpm)
#endif

#if (0) //12V 扫地宝电机
#define  MOTOR_SPEED_MAX                    (240000) 					         // 电机最大机械转速 单位：rpm
#define  MOTOR_PAIRS                        (1)                      // 电机极对数 单位：Poles
#define  MOTOR_RS                           (0.068)                   // 电机相电阻 单位：Ω
#define  MOTOR_LS                           (0.000018)                // 电机相电感 单位：H
#define  K_EMF                      	      (4.31)				            // 反电动势常数EMF_factor (V/Krpm)
#endif

#if (0) //24 格智美落地扇 20250925
#define  MOTOR_SPEED_MAX                    (4000) 					         // 电机最大机械转速 单位：rpm
#define  MOTOR_PAIRS                        (5)                      // 电机极对数 单位：Poles
#define  MOTOR_RS                           (2.983)                   // 电机相电阻 单位：Ω
#define  MOTOR_LS                           (0.002375)                // 电机相电感 单位：H
#define  K_EMF                      	     	(5.22)				             // 反电动势常数EMF_factor (V/Krpm)
#endif


#if (0)  //24V野火电机
#define  MOTOR_SPEED_MAX                    (4000) 					          // 电机最大机械转速 单位：rpm
#define  MOTOR_PAIRS                        (4)                       // 电机极对数 单位：Poles
#define  MOTOR_RS                           (0.8/2)                   // 电机相电阻 单位：Ω
#define  MOTOR_LS                           (0.000590/2)              // 电机相电感 单位：mH
#define  K_EMF                      	    	(4.0)				              // 电机相电感 单位：mH
#endif

#if (0)  //12V服务器（大）
#define  MOTOR_SPEED_MAX                    (EPWM_FREQ) 					          // 电机最大机械转速 单位：rpm
#define  MOTOR_PAIRS                        (3)                       // 电机极对数 单位：Poles
#define  MOTOR_RS                           (0.222/2)                   // 电机相电阻 单位：Ω
#define  MOTOR_LS                           (0.000116/2)              // 电机相电感 单位：mH
#define  K_EMF                      	    	(4.0)				              // 电机相电感 单位：mH
#endif

#if (0)  //12V服务器（小）
#define  MOTOR_SPEED_MAX                    (EPWM_FREQ) 					          // 电机最大机械转速 单位：rpm
#define  MOTOR_PAIRS                        (3)                       // 电机极对数 单位：Poles
#define  MOTOR_RS                           (0.444/2)                   // 电机相电阻 单位：Ω
#define  MOTOR_LS                           (0.000232/2)              // 电机相电感 单位：mH
#define  K_EMF                      	    (4.0)				              // 电机相电感 单位：mH
#endif

//---------------------------------------------------------------------------//
/****************************<< 硬件参数配置 >>*******************************/

//#define  HW_PHASE_RES                       (0.02)                   // 相采样电阻 单位：Ω
//#define  HW_IBUS_RES                        (0.005)                  // 母线采样电阻 单位：Ω
//#define  HW_ADC_REF                         (5.0)                    // ADC参考电压 单位：V
//#define  HW_VBUS_VOLT                       (24.0)                   // 母线电压 单位：V (注:填入电机铭牌电压值)
//#define  HW_VBUS_SDR                        (3.3/50.3)               // 母线电压采样，分压电阻比 (注:必须填入浮点数，不能为整数)
//#define  HW_VBEMF_SDR                       (3.3/50.3)               // 反电动势电压采样，分压电阻比 (注:必须填入浮点数，不能为整数)
//#define  MAX_VOLTAGE_VECTOR                 (0.98)                   // 占空比开度


#define  HW_PHASE_RES                       (0.02)                   // 相采样电阻 单位：Ω
#define  HW_IBUS_RES                        (0.02)                  // 母线采样电阻 单位：Ω
#define  HW_ADC_REF                         (4.5)                    // ADC参考电压 单位：V
#define  HW_VBUS_VOLT                       (24.0)                   // 母线电压 单位：V (注:填入电机铭牌电压值)
#define  HW_VBUS_SDR                        (2.0/15.0)               // 母线电压采样，分压电阻比 (注:必须填入浮点数，不能为整数)
#define  HW_VBEMF_SDR                       (1.0/15.0)               // 反电动势电压采样，分压电阻比 (注:必须填入浮点数，不能为整数)
#define  MAX_VOLTAGE_VECTOR                 (0.90)                   // 占空比开度


//===========================================================================//
/****************************<< 3、启动参数设定 >>****************************/
//===========================================================================//

//---------------------------------------------------------------------------//
/****************************<< 启动电流参数 >>*******************************/
#define  START_CURRENT_INIT                 I_Value(10.0)            // 启动电流初始值 单位：A
#define  START_CURRENT_FINAL                I_Value(18.0)            // 启动电流最大限值 单位：A
#define  START_IRAMP                        (1)                      // 启动电流爬坡周期

//---------------------------------------------------------------------------//
/****************************<< 弦波启动参数 >>*****************************/
// 启动参数 注：启动调试主要调试 OBSE_OFFSET_ANGLE 与 OBSE_OFFSET_ANGLE 这两个参数
  // 21.6V 200W N30*0.6
#define  OBSE_OFFSET_ANGLE                  (1300)                   // 观测器补偿角度
#define  OBSE_KSLF                          (6000)                   // 观测器反电动势滤波系数

#define  OBSE_START_OMGEA                   (15)                     // 观测器启动角速度
#define  OBSE_GS_FACTOR                     _Q10(1.0)                // 观测器GS修正系数
#define  OBSE_FS_FACTOR                     (1.0)                    // 观测器FS修正系数
#define  OBSE_START_EKP                     (25000)                  // 观测器KP 
#define  OBSE_START_EKI                     (00)                     // 观测器KI 

//---------------------------------------------------------------------------//
/****************************<< 方波启动参数 >>*****************************/
#define  BLDC_START_ENABLE                  (0)                      // 方波启动使能
#define  BLDC_DUTY_SET                      _Q15(0.02)               // 启动占空比设定  小占空比启动
#define  BLDC_DUTY_MAX                      _Q15(0.10)               // 启动最大占空比
#define  BLDC_DUTY_RAMP                     (30)                     // 占空比给定快慢 单位:Tpwm  值越大给定越慢，值越小给定越快
#define  BLDC_SPEED_SWITCH                  (4000)                   // 切换闭环转速值 (切入闭环判定阈值) 单位：rpm
#define  BLDC_SWITCH_TIME                   (30)                     // 过零点换相次数 (切入闭环判定阈值)
#define  BLDC_FIRST_LOCK_TIME               (40)                     // 第一拍锁定时间  单位：ms
#define  BLDC_SWITCH_PHASE_TIME             (100)                    // 找到过零点延时换相时间 单位：us 
#define  BLDC_MASK_TIME                     (250)                    // 换相成功避开续流点延时时间 单位：us
#define  BLDC_MASK_TIME_INIT                (10000)                  // 初始屏蔽时间 单位：us
#define  FORCE_SWITCHPHASE_TIME             (700)                    // 强制换相时间  单位：Tpwm	

//---------------------------------------------------------------------------//
/****************************<< 顺风启动参数 >>*******************************/
#define  BEMF_CHECK_TIME                    (2000)                   // BEMF检测顺风态时间 单位：Tpwm
#define  BEMF_SPEED_START                   (4000)                   // BEMF检测顺风启动转速(低于该转速进入启动态) 单位：RPM
#define  BEMF_SPEED_MIN                     (10000)                  // BEMF检测顺风最小投切转速(大于该转速进入切换态) 单位：RPM
#define  BEMF_SPEED_MAX                     (40000)                  // BEMF检测顺风最大投切转速(小于该转速进入切换态) 单位：RPM
#define  BEMF_OFFSET_ANGLE                  (0)                  		// BEMF检测顺风 角度补偿 (0-65535  对应0°-360°  10923->60°)

//===========================================================================//
/****************************<< 4、运行参数设定 >>****************************/
//===========================================================================//



//---------------------------------------------------------------------------//
/****************************<< 速度爬坡参数 >>*******************************/
#define  SPEED_RAMP_INC                     (53)                      // 速度爬坡曲线，加速斜率
#define  SPEED_RAMP_DEC                     (53)                      // 速度爬坡曲线，下降斜率

//---------------------------------------------------------------------------//
/****************************<< 电机运行电流参数 >>***************************/
#define  FOC_RUN_CURRENT_MIN                I_Value(0.5)             // 电机运行最小相电流峰值 单位：A
#define  FOC_RUN_CURRENT_MAX                I_Value(45.0)            // 电机运行最大相电流峰值 单位：A

//---------------------------------------------------------------------------//
/****************************<< 电机效率参数 >>*******************************/
// 相位补偿角
#define  FOC_OFFSET_ANGLE                   _Q15(0.00)               // FOC补偿角，例如补偿10° 补偿值=10/360=0.0278

//===========================================================================//
/****************************<< 5、功能选择 >>********************************/
//===========================================================================//

//---------------------------------------------------------------------------//
/****************************<< 在线仿真模式 >>*******************************/
#define  Motor_Debug_Offline                (0)                      // 关闭在线仿真
#define  Motor_Debug_Online                 (1)                      // 在线仿真
#define  Motor_Debug_Mode                   (Motor_Debug_Offline)

//---------------------------------------------------------------------------//
/****************************<< DEBUG功能 >>**********************************/
// 硬件调试 测试EPMW输出波形，同时用于启动测试
#define  Debug_Disable                      (0)                      // DEBUG模式关闭
#define  Debug_Enable                       (1)                      // DEBUG模式开启
#define  Config_Debug_Mode                  (Debug_Disable)   

//---------------------------------------------------------------------------//
/****************************<< 电机旋转方向 >>*******************************/
#define  FR_CW                              (0)                      // 顺时针旋转
#define  FR_CCW                             (1)                      // 逆时针旋转
#define  Config_Motor_FR                    (FR_CW)

//---------------------------------------------------------------------------//
/****************************<< PWM捕获功能 >>********************************/
#define  CCP_Capture_Disable                (0)                      // 捕获功能关闭
#define  CCP_Capture_Enable                 (1)                      // 捕获功能开启
#define  Config_CCP_Capture                 (CCP_Capture_Enable)
#define  CCP_CAPTURE_MODULE                 (CCP1)                   // 捕获的模块 CCP0、CCP1
#define  CCP_CAPTURE_CH                     (CCPxB)                  // 捕获的通道 CCPxA、CCPxB

//---------------------------------------------------------------------------//
/****************************<< FG输出功能 >>*********************************/
#define  CCP_PWM_Disable                    (0)                      // FG输出功能关闭
#define  CCP_PWM_Enable                     (1)                      // FG输出功能开启
#define  Config_CCP_PWM                     (CCP_PWM_Enable)
#define  CCP_PWM_MODULE                     (CCP0)                   // PWM的模块 CCP0、CCP1
#define  CCP_PWM_CH                         (CCPxB)                  // PWM的通道 CCPxA、CCPxB

//---------------------------------------------------------------------------//
/****************************<< 休眠功能 >>***********************************/
#define  Sleep_Disable                      (0)                      // 休眠功能关闭
#define  Sleep_Enable                       (1)                      // 休眠功能开启
#define  Sleep_Control_Mode                 (Sleep_Disable)

//---------------------------------------------------------------------------//
/****************************<< 停车方式 >>***********************************/
#define  BRAKE_STOP_DISABLE                 (0)                      // 自由停车
#define  BRAKE_STOP_ENABLE                  (1)                      // 刹车
#define  BRAKE_STOP_MODE                    (BRAKE_STOP_DISABLE)
 
#define  BRAKE_STOP_SPEED                   (40000)                  // 刹车转速 单位：RPM
#define  BRAKE_STOP_TIME                    (350)                    // 刹车时间 单位：ms

//---------------------------------------------------------------------------//
/****************************<< 调试数据查看模式 >>***********************/
#define  UART_OFF                           (0)                      // 关闭数据查看
#define  UART_ON                            (1)                      // 开启串口波形查看
#define  RTT_ON                             (2)                      // 开启RTT波形查看(量产须关闭)
#define  UART_CONFIG_MODE                   (UART_OFF)

//===========================================================================//
/****************************<< 6、保护参数设定 >>****************************/
//===========================================================================//

//---------------------------------------------------------------------------//
/****************************<< 功率/转速/电流限制参数 >>*********************/
#define  LIMIT_POWER_ENABLE                 (1)                      // 限功率使能
#define  LIMIT_POWER_VALUE                  (350)                     // 硬件限功率值 单位：W

#define  LIMIT_SPEED_ENABLE                 (1)                      // 限转速使能
#define  LIMIT_SPEED_VALUE                  (120000)                 // 限转速值 单位：RPM

#define  LIMIT_IBUS_ENABLE               		(1)                      // 限母线均值电流使能
#define  LIMIT_CURRENT_VALUE                (19.8)                   // 限母线最大电流值 单位：A

//===========================================================================//
/****************************<< 7、开环启动参数设定 >>****************************/
//===========================================================================//

//---------------------------------------------------------------------------//
/****************************<< 开环启动相关参数 >>*********************/


//--------------------------------基值-------------------------------------------/
#define  I_PHASE_BASE        					((float)(HW_ADC_REF - HW_AMP_REF_IP) / (HW_PHASE_RES * HW_AMP_GAIN_IP))  
#define  I_DC_BASE           			        ((float)POWER_DC_BASE / HW_VBUS_VOLT)							
#define  RES_PHASE_BASE        					((float)(HW_VBUS_VOLT/1.7321/I_PHASE_BASE)) 	
	
// 电机运行参数
#define  	POWER_DC_BASE													(90)									// 最大功率 (W)   
// 对齐
#define   MC_ALIGN_ANGLE                        (0*65536.0/360.0) // 对齐角度 [0,360]
#define		MC_ALIGN_D_AXIS_CURRENT								(0.8/2)									// 对齐电流 单位：A
#define		MC_ALIGN_Q_AXIS_CURRENT								(0.0)									// 对齐电流 单位：A
#define		MC_ALIGN_TIME													(10000)								// 对齐时间 单位：ms

//----------------------------------------------------------------------------/
// Start参数
#define		MC_Start_TIME													(10000)								// 启动时间 单位：ms
#define		MC_Start_Curr													(0.8*2)	//	0.6					    // 启动电流 单位：A
#define   MC_Start_Init_Speed                   (5)                   // 启动初始转速	单位：rpm

#if(HIGH_SPEED_MODE_CONFIG == HIGH_SPEED_MODE_ENABLE)
#define		MC_Start_End_Speed										(5000)//(24000)			          // 启动完成转速	单位：rpm
#else
#define		MC_Start_End_Speed										(500) 			          // 启动完成转速	单位：rpm
#endif

//SW切换参数
#define   Sw_To_Run_Error_Threshold              (200)                 //速度误差范围，在±80rpm认为为正常波动
#define   Sw_To_Run_Cnt_Threshold                (500)                //若速度误差小于阈值，且持该次数，则进行无扰动切换

//TRANSITION参数
#define		MC_Transition_Current										(0.6)		//0.6							// 运行电流 单位：A
#define  	MC_TRANSITION_COMPENSATION_CNT          (20)                  // 运行时补偿角每MC_TRANSITION_COMPENSATION_CNT减1

//RUN参数
#define		MC_Iq_Run_Current					  (0.6)//0.7			// 运行电流 单位：A  
#define		MC_Id_Run_Current					  (0)				    // 运行电流 单位：A

#if(HIGH_SPEED_MODE_CONFIG==HIGH_SPEED_MODE_ENABLE)						//	高速电机补偿角较大
#define		MC_Run_Theta_Compensation			  (0)		//6000		// 运行补偿角 [0-65535]--->[0,360]
#else
#define		MC_Run_Theta_Compensation			  (0)	//8190	//24575		    // 运行补偿角 [0-65535]--->[0,360]
#endif

#define		MC_Run_Kslf							  (3000)				// 运行滑膜滤波系数
#define     MC_RUN_COMPENSATION_CNT               (20)                  //运行时补偿角每MC_RUN_COMPENSATION_CNT加1

//--------------------------------物理值到标幺转换系数-------------------------------------------/
#define  Phy2Pu_Coeff_Vbus						((float)( _Q14_VAL /HW_VBUS_VOLT))
#define  Phy2Pu_Coeff_Ibus      		  ((float)( _Q14_VAL /I_DC_BASE))
#define  Phy2Pu_Coeff_Power   		    ((float)( _Q14_VAL /POWER_DC_BASE))
#define  Phy2Pu_Coeff_Iphase					((float)( _Q15_VAL /I_PHASE_BASE))
#define  Phy2Pu_Coeff_Speed						((float)( _Q14_VAL /MOTOR_SPEED_BASE))
#define  Phy2Pu_Coeff_Vctrl						((float)( _Q15_VAL /HW_ADC_REF))
//--------------------------------物理值到标幺转换宏定义函数-------------------------------------------/
#define  Phy2Pu_Fun_Vbus(V)						(int32_t)(V *_Q14_VAL /HW_VBUS_VOLT)																// 	母线电压标幺值计算	目标Q14
#define  Phy2Pu_Fun_Ibus(A)						(int32_t)(A *_Q14_VAL /I_DC_BASE)  																	//  母线电流标幺值计算	目标Q14
#define  Phy2Pu_Fun_Power(P)					(int32_t)(P *_Q14_VAL /POWER_DC_BASE)  															//  母线功率标幺值计算	目标Q14
#define  Phy2Pu_Fun_Iphase(A)					(int32_t)(A *_Q15_VAL /I_PHASE_BASE)  															//  相电流标幺值计算		目标Q15
#define  Phy2Pu_Fun_Vctrl(V)					(int32_t)(V *_Q15_VAL /HW_ADC_REF)																	//  调速电压标幺值计算	目标Q15
#define  Phy2Pu_Fun_Speed(S)					(int32_t)(S *Phy2Pu_Coeff_Speed)																		//  转速标幺值计算			目标Q14

//--------------------------------标幺值到物理值转换系数-------------------------------------------/
#define  Pu2Phy_Coeff_Vbus						(2 * 10 * HW_VBUS_VOLT )										// VbusPU  * Pu2Phy_Coeff_Vbus >> 15 = PhyVal   100mV
#define  Pu2Phy_Coeff_Ibus						(2 * 100* I_DC_BASE )												// IbusPU  * Pu2Phy_Coeff_Vbus >> 15 = PhyVal  	10mA
#define  Pu2Phy_Coeff_Power						(2 * 10 * POWER_DC_BASE )										// PowerPU * Pu2Phy_Coeff_Vbus >> 15 = PhyVal		100mW
#define  Pu2Phy_Coeff_Iphase					(100 * I_PHASE_BASE )												// IphasePU* Pu2Phy_Coeff_Vbus >> 15 = PhyVal   10mA
#define  Pu2Phy_Coeff_Vctrl						(100 * HW_ADC_REF )													// Vctrl   * Pu2Phy_Coeff_Vbus >> 15 = PhyVal   10mV	


//---------------------------------------------------------------------------//
/****************************<< 外环PID控制参数 >>****************************/
// 速度环/功率环PI参数
#define  FOC_WKP                            _Q15(0.1)                // 速度环KP ( 0.1~2.0 ) 0.15  1.0
#define  FOC_WKI                            _Q15(0.002)              // 速度环KI ( 0.001~0.05 ) 0.007  0.2

#define  TIME_SPEED_LOOP                    (10)                     // 速度环执行周期 单位：ms


//===========================================================================//
#endif	 /* __USER_CONTROL_H__ */
