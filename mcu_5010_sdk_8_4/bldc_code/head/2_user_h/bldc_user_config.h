#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__
#include "bldc_main.h"


/****************************************************************************
*(1)
*上电延时,烧录口复用时使用
*
*****************************************************************************/
#define Power_On_Delay_En                 	                    (1ul)
#define Power_On_Delay_Cnt                 	                    (2000ul)/*ms*/
/****************************************************************************/


/****************************************************************************
*(2)
*硬件参数
*
*****************************************************************************/
#define Fre_MCU                      	                    	(64000000)/*mcu主频HZ*/
#define Fre_SysTick                   	                    	(10000)/*系统定时器频率HZ*/

#define Fre_Timer                      	                        (Fre_MCU/16)/*单位:HZ*/
#define Fre_UState                      	                    (1000)/*单位:HZ*/
#define Fre_MState                      	                	(10000)/*单位:HZ*/

#define HW_VBus_Gain                                            (11.00)/*母线电压倍数*/
#define HW_VBus_VDD                                             (5.00)/*VDD电压*/
#define HW_Adc_Max                                              (4096)/*ADC总值*/

#define HW_IBus_Res                                             (0.005)/*母线电流采样电阻unit:om*/
    
#define HW_IBus_Gain                                            (16.00)/*电流放大倍数*/

#define HW_IBus_OffsetRef                                       (2.58)/*unit:v*/

#define HW_Motor_PolePair                                       (2)/*电机极对数*/

/****************************************************************************/


/****************************************************************************
*(10)
*母线电压保护
*
*****************************************************************************/
#define VBus_Protect_En                                         (1)/*母线电压保护*/

#define VBus_Stb_UVP_V                                          (15.00)/*unit:v*/
#define VBus_Stb_UVP_FiltMS                                     (200)/*unit:ms*/
                    
#define VBus_Run_UVP1_V                                         (13.50)/*unit:v*/
#define VBus_Run_UVP1_FiltMS                                    (200)/*unit:ms*/

#define VBus_Run_UVP2_V                                         (13.00)/*unit:v*/
#define VBus_Run_UVP2_FiltMS                                    (100)/*unit:ms*/
    
#define VBus_OVP_V                                              (25.00)/*unit:v*/
#define VBus_OVP_FiltMS                                         (10)/*unit:ms*/
/****************************************************************************/


/****************************************************************************
*(11)
*母线电流保护
*
*****************************************************************************/
#define IBus_Avg_Protect_En                                     (1)/*母线均值电流保护*/

#define IBus_Avg_OCP1_A                                         (45.00)/*unit:a*/ 
#define IBus_Avg_OCP1_FiltMS                                    (2000)/*unit:ms*/

#define IBus_Avg_OCP2_A                                         (55.00)/*unit:a*/ 
#define IBus_Avg_OCP2_FiltMS                                    (1000)/*unit:ms*/

#define IBus_Avg_OCP3_A                                         (65.00)/*unit:a*/ 
#define IBus_Avg_OCP3_FIltMS                                    (500)/*unit:ms*/


#define IBus_Peak_Protect_En                                    (1)/*母线峰值电流保护*/

#define IBus_Peak_OCP1_A                                        (90.00)/*unit:a*/ 
#define IBus_Peak_OCP1_FiltMS                                   (10)/*unit:ms*/

#define IBus_Avg_Lim_En                                         (0)/*母线均值限流*/
#define IBus_Avg_Lim_A                                          (5.00)/*unit:a*/


#define IBus_Peak_Lim_En                                        (0)/*母线峰值限流*/
#define IBus_Peak_Lim_A                                         (20.00)/*unit:a*/


#define IBus_Peak_CheckMos_A                                    (10.00)/*unit:a*//*用于上电检测MOS直通*/  

/****************************************************************************/  


/****************************************************************************
*(12)
*温度保护
*
*****************************************************************************/
#define Ntc_Mos_Protect_En                                      (0)/*MOS温度保护*/

#define Ntc_Mos_Ref_Res                                         (10000)/*unit:om*/

#define Ntc_Mos_OTP_Res                                         (1414)/*保护温度阻值 unit:om*/
#define Ntc_Mos_OTPClr_Res                                      (1628)/*恢复温度阻值 unit:om*/
#define Ntc_Mos_UTP_Res                                         (200225)/*保护温度阻值 unit:om*/
#define Ntc_Mos_UTPClr_Res                                      (152600)/*恢复温度阻值 unit:om*/

#define Ntc_Mos_Err_SetMS                                       (2000)/*unit:ms*/
#define Ntc_Mos_ErrClr_SetMS                                    (2000)/*unit:ms*/


#define Ntc_Bat_Protect_En                                      (0)/*battery温度保护*/

#define Ntc_Bat_Ref_Res                                         (10000)/*unit:om*/

#define Ntc_Bat_OTP_Res                                         (1414)/*保护温度阻值 unit:om*/
#define Ntc_Bat_OTPClr_Res                                      (1628)/*恢复温度阻值 unit:om*/
#define Ntc_Bat_UTP_Res                                         (200225)/*保护温度阻值 unit:om*/
#define Ntc_Bat_UTPClr_Res                                      (152600)/*恢复温度阻值 unit:om*/

#define Ntc_Bat_Err_SetMS                                      	(2000)/*unit:ms*/
#define Ntc_Bat_ErrClr_SetMS                                   	(2000)/*unit:ms*/
/****************************************************************************/


/****************************************************************************
*(13)
*扳机控制阈值
*0-100
*****************************************************************************/
#define Tool_Type_Drill                                         (0)/*电钻模式*/
#define Tool_Type_Wrench                                        (1)/*扳手模式*/
#define Tool_Type_Set                                           (Tool_Type_Drill)/*工具电钻扳手模式选择*/

#define Tool_Dir_CW_Value                                       (0)/*顺时针方向赋值*/
#define Tool_Dir_CCW_Value                                      (1)/*逆时针方向赋值*/

#define Tool_Stop_Percent                                 		((0.10)*32768)/*0-1停止电压阈值*/	
#define Tool_Start_Percent                                		((0.15)*32768)/*0-1启动电压阈值*/		

#define Tool_SpeedUp_Min_Percent		                        ((0.20)*32768)/*0-1调速最低电压阈值*/	
#define Tool_SpeedUp_Max_Percent	                            ((0.90)*32768)/*0-1调速最大电压阈值*/	

#define Tool_RateOfTrigger_Min_Q15                       		((0.00)*32768)/*调速比例设置*/
#define Tool_RateOfTrigger_Max_Q15                          	((1.00)*32768)/*调速比例设置*/	

#define Tool_Sleep_DelayTime                                    (50000)/*工具待机时间 unit:ms*/
/****************************************************************************/


/****************************************************************************
*(14)
*通用宏定义
*
*****************************************************************************/
#define _AbsFun(Value)			 								((Value)>=0?(Value):(-(Value)))  

#define __Q10(A)                     	                    	(int32_t)((float)A*1024)/*Q10 format*/
#define __Q12(A)                     	                    	(int32_t)((float)A*4096)/*Q12 format*/
#define __Q14(A)                       	                    	(int32_t)((float)A*16384)/*Q14 format*/
#define __Q15(A)                      	                    	(int32_t)((float)A*32768)/*Q15 format*/

#define __Get_Q15(Real,Base)          	                    	(int32_t)((float)Real*32768/Base)       
#define __Get_Q14(Real,Base)           	                    	(int32_t)((float)Real*16384/Base)    

#define DI()                       		                        (__disable_irq())/*全局中断禁止*/
#define EI()                          	                        (__enable_irq())/*全局中断使能*/

/****************************************************************************/


/****************************************************************************
*(15)
*应用项目参数宏定义
*
*****************************************************************************/

/*反转自停参数*/
#define CURT_CCW_AUTO_STOP      	   	                        (9000ul)     /*unit mA*/
#define TIME_AUTO_STOP                 	                        (500ul)     /*unit 1ms*/
#define TIME_AUTO_STOP_REDUCE          	                        (1ul)     /*unit 1ms*/
#define CIRCUIT_CNT_AUTO_STOP          	                        (300ul)     

/*灯指示电压*/

#define SOC3_MV                     	                        ((20.5)*100)
#define SOC2_MV                     	                        ((17.5)*100)
#define SOC1_MV                     	                        ((15.0)*100)


/****************************************************************************/


#endif
