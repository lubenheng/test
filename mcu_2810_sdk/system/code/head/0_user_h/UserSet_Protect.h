//===========================================================================//
/*****************************************************************************/
//---------------------------------------------------------------------------//
/** \file
*  File Name  : UserSet_Fault.h
*  Author     : CMS Motor Control Team
*  Version    : 第二代风机软件平台
*  Date       : 2023年2月
** Description: 本页参数为故障保护相关参数。
**              该版本适用于吸尘器/扫地宝等产品。
//---------------------------------------------------------------------------//
******************************************************************************/
//===========================================================================//

#ifndef  __UserSet_Fault__
#define  __UserSet_Fault__

//============================================================================/
// 代码对齐方式：Edit--Configuration--Editor--C/C++Files--Tab size = 2
//----------------------------------------------------------------------------/

//===========================================================================//

// 重启次数设置为72次，可无限次重启

//----------------------------------------------------------------------------/
// 硬件过流(短路保护)
#define		HARDOVCUR_PROTECT_VALUE								(16.0)									// 硬件过流					单位：A */ 

#define		HARDOVCUR_RESTART_ENABLE							(1)                     // 保护后重启使能		0,不使能；1，使能
#define		HARDOVCUR_RESTART_GAP									(1000)                 // 重启间隔时间 		单位：ms
#define		HARDOVCUR_RESTART_NUM									(1)                     // 重启次数     		单位：次

//----------------------------------------------------------------------------/
// 软件过流
#define		OVERPHASECUR_PROTECT_ENABLE						(1)                     // 软件过流保护			0,不使能；1，使能
#define		OVERPHASECUR_PROTECT_VALUE						(12.0)                  // 软件过流保护值		单位：A 

#define		OVERPHASECUR_RESTART_ENABLE						(1)                     // 保护后重启使能		0,不使能；1，使能
#define		OVERPHASECUR_RESTART_GAP							(7000)                  	// 重启间隔时间 		单位：ms
#define		OVERPHASECUR_RESTART_NUM							(2)                     // 重启次数					单位：次

//----------------------------------------------------------------------------/
// 电压保护
#define		VBUS_PROTECT_ENABLE										(1)                     // 电压保护					0,不使能；1，使能

#define		RUN_OVER_VOLT_VALUE										(25.0)            			// 运行过压保护电压值		单位：V
#define		RUN_UNDER_VOLT_VALUE									(9.5)            				// 运行欠压保护电压值		单位：V
#define		RUN_VBUS_PROTECT_TIME									(500)             			// 运行过欠压判别时间		单位：ms

#define		VBUS_RECOVER_ENABLE										(1)                     // 电压保护自行恢复	0,不使能；1，使能
#define		OV_RECOVER_VALUE											(24.0)                  // 过压恢复电压值		单位：V 
#define		UV_RECOVER_VALUE											(11.5)                  // 欠压恢复电压值		单位：V
#define		VBUS_RECOVER_TIME											(500)                   // 过欠压恢复延时		单位：ms

//----------------------------------------------------------------------------/
// 堵转保护
#define		BLOCK_PROTECT_ENABLE									(1)                     // 堵转保护					0,不使能；1，使能
#define		BLOCK_SPEED_MAX												(3000)                // 堵转保护最大转速	单位：rpm   须小于 MOTOR_SPEED_BASE * 2
#define		BLOCK_SPEED_MIN												(100)                  // 堵转保护最小转速	单位：rpm
#define		BLOCK_PROTECT_TIME										(200)                   // 堵转判别时间			单位：ms

#define		BLOCK_RESTART_ENABLE									(1)                     // 保护后重启使能		0,不使能；1，使能
#define		BLOCK_RESTART_GAP											(100)                  // 重启间隔时间 		单位：ms
#define		BLOCK_RESTART_NUM											(5)                     // 重启次数					单位：次

//----------------------------------------------------------------------------/
// 缺相保护
#define		PHASELOSS_PROTECT_ENABLE							(1)                     // 缺相保护					0,不使能；1，使能
#define		PHASELOSS_CUR_MIN											(0.1)                   // 缺相电流阈值			单位：A
#define		PHASELOSS_PROTECT_TIME								(200)                   // 缺相判别时间			单位：ms

#define		PHASELOSS_RESTART_ENABLE							(1)                     // 保护后重启使能		0,不使能；1，使能
#define		PHASELOSS_RESTART_GAP									(5000)                  // 重启间隔时间			单位：ms
#define		PHASELOSS_RESTART_NUM									(3)                     // 重启次数					单位：次

//----------------------------------------------------------------------------/
// 温度保护
// 说明：填写对应温度的NTC阻值,
//   注：适用于上拉电阻+NTC的电路结构，其他电路请自行修改 TEMP_VALUE 计算式
//       NTC规格不同，阻值不同，温度 - 阻值数据请查询数据手册

#define		TEMP_PROTECT_ENABLE										(0)                     // 温度保护					0,不使能；1，使能

#define		PULLUP_Res_Value                      (10.0)                  // NTC上拉电阻阻值 单位：kΩ 
#define		TEMP_VALUE(NTC_VALUE)        					_Q15((5.0*NTC_VALUE/(PULLUP_Res_Value+NTC_VALUE))/HW_ADC_REF) 

#define		OVER_TEMP_VALUE												(TEMP_VALUE(0.860))     // 高温保护电压值		单位：kΩ 		105 ℃
#define		UNDER_TEMP_VALUE											(TEMP_VALUE(59.589))    // 低温保护电压值		单位：kΩ 		-17 ℃
#define		TEMP_PROTECT_TIME											(1500)                  // 高低温保护延时		单位：ms

#define		TEMP_RECOVER_ENABLE										(1)                     // 温度保护自行恢复	0,不使能；1，使能
#define		OTEMP_RECOVER_VALUE										(TEMP_VALUE(1.673))     // 高温恢复温度值		单位：kΩ 		105 ℃
#define		UTEMP_RECOVER_VALUE										(TEMP_VALUE(42.889))    // 低温恢复温度值		单位：kΩ 		-10 ℃
#define		TEMP_RECOVER_TIME											(1500)                  // 高低温恢复延时		单位：ms
						
//----------------------------------------------------------------------------/
// 启动失败保护
#define		STARTFAIL_PROTECT_TIME								(5000)                  // 启动失败判别时间	单位：ms

#define		STARTFAIL_RESTART_ENABLE							(1)                     // 保护后重启使能		0,不使能；1，使能
#define		STARTFAIL_RESTART_GAP									(5000)                  // 重启间隔时间			单位：ms
#define		STARTFAIL_RESTART_NUM									(3)                     // 重启次数					单位：次

//----------------------------------------------------------------------------/
// 母线均值电流保护
#define		OVERIBUS_PROTECT_ENABLE								(1)                     // 母线电流保护			0,不使能；1，使能
#define		OVERIBUS_PROTECT_Value								(5.9)                  // 母线电流保护值		单位：A
#define		OVERIBUS_PROTECT_TIME									(2000)                  // 启动失败判别时间	单位：ms

#define		OVERIBUS_RESTART_ENABLE								(1)                     // 保护后重启使能		0,不使能；1，使能
#define		OVERIBUS_RESTART_GAP									(5000)                  // 重启间隔时间			单位：ms
#define		OVERIBUS_RESTART_NUM									(2)                     // 重启次数					单位：次

//----------------------------------------------------------------------------/
// 错误复位设置
#define		FAULTCLEAR_SIGNRESET_EN								(0)                     // 重置控制信号清除所有故障、重启次数	
#define		FAULTCLEAR_RUN_EN											(1)                     // 运行成功后清除重启次数
#define 	FAULTCLEAR_RUN_Time										(12000)								  // 运行成功清除等待时间 单位：ms

#endif

