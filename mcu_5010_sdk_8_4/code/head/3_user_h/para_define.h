
//===========================================================================//
/*****************************************************************************/
//---------------------------------------------------------------------------//
/** \file 
*  File Name  : para_define.h
*  Author     : CMS Appliction Team
*  Version    : 第二代风机软件平台
*  Date       : 09/01/2022
** Description: 本页参数为硬件相关参数。
** 
//---------------------------------------------------------------------------//
******************************************************************************/
//===========================================================================//

#ifndef  __PARA_DEFINE_H__
#define  __PARA_DEFINE_H__

#include "user_control.h"

//===========================================================================//
// 上电延时
#define  POWERON_DELAY_TIME                 (1000)

//---------------------------------------------------------------------------//
// 电阻采样方式
#define  Single_Shunt                       (0)                      // 单电阻
#define  Double_Shunt                       (1)                      // 双电阻
#define  Treble_Shunt                       (2)                      // 三电阻
#define  Config_Shunt_Mode                  (Treble_Shunt)

//---------------------------------------------------------------------------//
// SVPWM调制方式
#define  Segment_7                          (0)                      // 七段式
#define  Segment_5                          (1)                      // 五段式
#define  Config_SVPWM_Mode                  (Segment_7)

//----------------------------------------------------------------------------/
// 电机转向
#define  FR_CW                              (0)                      // 顺时针旋转
#define  FR_CCW                             (1)                      // 逆时针旋转
#define  Config_Motor_FR                    (FR_CW)

//============================================================================/
// 电机参数
#define  MOTOR_PAIRS                        (5)                      // 电机极对数 单位：Poles

#define  MOTOR_LS                           (0.028/2)                  // 电机相电感 单位：mH  注意单位
#define  MOTOR_RS                           (0.030/2)                  // 电机相电阻 单位：Ω
//---------------------------------------------------------------------------//
//硬件参数配置

#if (Config_Shunt_Mode == Single_Shunt)
#define  HW_AMP_REF_IP                      (1.25)                    // 相电流采样偏置电压 单位：V
#else
#define  HW_AMP_REF_IP                      (1.25)                    // 相电流采样偏置电压 单位：V
#endif
#define  HW_AMP_REF_IBUS                    (1.25)                    // 母线电流采样偏置电压 单位：V

#define  HW_AMP_GAIN_IP                     (5.0)                     // 相电流采样放大倍数
#define  HW_AMP_GAIN_IBUS                   (5.0)                     // Ibus电流采样放大倍数

#define  HW_PHASE_RES                       (0.020)                  // 相采样电阻 单位：Ω
#define  HW_IBUS_RES                        (0.020)                  // 母线采样电阻 单位：Ω
#define  HW_ADC_REF                         (2.5)                    // ADC参考电压 单位：V
#define  HW_VBUS_VOLT                       (24.0)                   // 母线电压 单位：V (注:填入产品铭牌工作电压)
#define  HW_VBUS_SDR                        (1.0/16.0)               // 母线电压采样，分压电阻比 (注:填入浮点数，不能填入整数)
#define		HW_VBEMF_SDR                      (1.0/11.0)						// 反电动势电压采样，分压电阻比
#define  MAX_VOLTAGE_VECTOR                 (0.90)                   // 占空比开度% (不改动)


//---------------------------------------------------------------------------//
// 模拟信号采样通道配置

#if (Config_Shunt_Mode == Single_Shunt)

// 物理通道
#define  ADC_SCAN_CHA                      (ADC_CH3)					
#define  ADC_SCAN_CHB                      (ADC_CH3)
#define  ADC_SCAN_CHC                      (ADC_CH3)

// 数据序列
#define  ADC_DATA_CHA                      (ADC_SEQ0)
#define  ADC_DATA_CHB                      (ADC_SEQ1)
#define  ADC_DATA_CHC                      (ADC_SEQ1)

//---------------------------------------------------------------------------//
#elif (Config_Shunt_Mode == Double_Shunt)

// 物理通道
#define  ADC_SCAN_CHA                      (ADC_CH0)
#define  ADC_SCAN_CHB                      (ADC_CH1)
#define  ADC_SCAN_CHC                      (ADC_CH1)

// 数据序列
#define  ADC_DATA_CHA                      (ADC_SEQ0)
#define  ADC_DATA_CHB                      (ADC_SEQ1)
#define  ADC_DATA_CHC                      (ADC_SEQ1)

#else 

// 物理通道
#define  ADC_SCAN_CHA                      (ADC_CH0)
#define  ADC_SCAN_CHB                      (ADC_CH1)
#define  ADC_SCAN_CHC                      (ADC_CH2)

// 数据序列
#define  ADC_DATA_CHA                      (ADC_SEQ0)
#define  ADC_DATA_CHB                      (ADC_SEQ1)
#define  ADC_DATA_CHC                      (ADC_SEQ2)
#endif

// 物理通道
#define  ADC_SCAN_VBUS                     (ADC_CH5)         // 母线电压低速采样
#define  ADC_SCAN_IBUS                     (ADC_CH6)         // 母线电流低速采样
#define  ADC_SCAN_CTRL                     (ADC_CH7)         // 调速信号采样

// 数据序列
#define  ADC_DATA_IBUS                     (ADC_DATA_CHC + 1)
#define  ADC_DATA_VBUS                     (ADC_DATA_IBUS + 1)
#define  ADC_DATA_CTRL                     (ADC_DATA_VBUS + 1)


//===========================================================================//
// 基值参数                                                                                          
#define  MOTOR_SPEED_BASE                   (120000UL)               // 转速基值 (RPM)   电机额定转速，观测器输出最高转速为该转速的2倍
#define  MOTOR_POWER_BASE                   (400)                    // 功率基值 (W)     注意不小于设置的最高功率,推荐为1~1.2倍                       
#define  MOTOR_VOLT_BASE                    ((float)HW_VBUS_VOLT/1.732)           
#define  MOTOR_IBUS_BASE                    ((float)MOTOR_POWER_BASE/HW_VBUS_VOLT)

//===========================================================================//
//电流环参数

//---------------------------------------------------------------------------//                                                                                 
#define  FOC_DKP                            _Q15(1.0)                // D轴PI参数：Kp
#define  FOC_DKI                            _Q15(0.01)               // D轴PI参数：Ki
#define  FOC_DOUT_MAX                       _Q15(0.5)                // D轴PI参数：最大输出
#define  FOC_DOUT_MIN                       _Q15(-0.5)               // D轴PI参数：最小输出

//---------------------------------------------------------------------------//
#define  FOC_QKP                            _Q15(1.0)                // Q轴PI参数：Kp
#define  FOC_QKI                            _Q15(0.01)               // Q轴PI参数：Ki
#define  FOC_QOUT_MAX                       _Q15(0.7)               // Q轴PI参数：最大输出(起不到限制作用) 
#define  FOC_QOUT_MIN                       _Q15(-0.7)               // Q轴PI参数：最小输出



#endif  /* __PARA_DEFINE_H__ */
