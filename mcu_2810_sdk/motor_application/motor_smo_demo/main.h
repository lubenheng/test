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
#ifndef __MAIN_H__
#define __MAIN_H__
#ifdef __cplusplus
extern "C"
{
#endif
/*****************************************************************************/
/* Include files */
/*****************************************************************************/
/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define') */
/*****************************************************************************/


/****************************<< Demo_type  >>********************************/
#define	  Motor                              (0)                     // 电机VF工程代码
#define	  Motor_vf                           (1)                     // 电机VF工程代码
#define	  Motor_if                           (2)                     // 电机IF工程代码
#define	  Motor_smo                          (3)                     // 电机SNO工程代码
#define   Demo_type                         (Motor_smo)               // 选择当前工程代码


#if (Demo_type==Motor_vf)
	//IF_CONFIG配置为1时，Ud/Uq由电流闭环给定；IF_CONFIG配置为0时，Ud/Uq人为给定；
	#define IF_ENABLE		 		                   (1) //IF
	#define IF_DISABLE 				                 (0) //VF
	#define IF_CONFIG 				                 (IF_DISABLE) 
#endif

#if(Demo_type==Motor_if)
	//IF_CONFIG配置为1时，Ud/Uq由电流闭环给定；IF_CONFIG配置为0时，Ud/Uq人为给定；
	#define IF_ENABLE		 		                   (1) //IF
	#define IF_DISABLE 				                 (0) //VF
	#define IF_CONFIG 				                 (IF_ENABLE) 
#endif

#if(Demo_type==Motor_smo)		
	//IF_CONFIG配置为1时，Ud/Uq由电流闭环给定；IF_CONFIG配置为0时，Ud/Uq人为给定；
	#define IF_ENABLE		 		                   (1) //IF
	#define IF_DISABLE 				                 (0) //VF
	#define IF_CONFIG 				                 (IF_ENABLE) 
#endif



/*****************************************************************************/
/* Global type definitions ('typedef') */
/*****************************************************************************/

/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source) */
/*****************************************************************************/


/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source) */
/*****************************************************************************/

#ifdef __cplusplus
}
#endif
extern float pwm_freq, fe_real_base, fe_line_base, fe_rad_base;
extern float Uadc_ref, U_att_factor, Iadc_ref, I_amp_factor, Rsample, Ld, Lq, Ts, Rs, Ki, Ku,pub_dem, pub_factor, EK1, EK2, EK3, EK4;
#endif /* __MAIN_H__ */
