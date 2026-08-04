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
#ifndef __BLDC_PUBLIC_DEBUG_H__
#define __BLDC_PUBLIC_DEBUG_H__
/*****************************************************************************/
/* Include files */
/*****************************************************************************/
//#include "cms32m5xxx.h"

#include "stdint.h"
#include "types.h"

/***********************************DEBUG************************************/
#define MOTOR_MODE_NON_TEST                     0/*无测试*/
#define MOTOR_MODE_TEST_DRIVE_HB                1/*互补输出波形*/
#define MOTOR_MODE_TEST_DRIVE_HX                2/*强制换相波形*/
#define MOTOR_MODE_TEST_IPD                     3/*IPD定位测试*/
#define MOTOR_MODE_TEST_DELAY                   4/*延时函数测试*/
#define MOTOR_TEST_MODE                         MOTOR_MODE_NON_TEST
/****************************************************************************/

/***********************************SIGNAL***********************************/
#define TEST_SIGNAL1_EN                         1                               /*cmp0 on = high off = low*/

#if(TEST_SIGNAL1_EN)

#define TEST_SIGNAL1                            GPIO3->DO_f.P0//GPIO4->DO_f.P4 

#define TEST_SIGNAL1_HIGH                       TEST_SIGNAL1 = 1
#define TEST_SIGNAL1_LOW                        TEST_SIGNAL1 = 0
#define TEST_SIGNAL1_INVERT                     TEST_SIGNAL1 = ~TEST_SIGNAL1
#else

#define TEST_SIGNAL1_HIGH                
#define TEST_SIGNAL1_LOW               
#define TEST_SIGNAL1_INVERT           
#endif
/****************************************************************************/


extern unsigned int 	ucChkDirTB[11];


void Debug_Manage(void);
void Debug_Epwm_HuBuShuChu(void);
void Debug_Epwm_DingShiHuanXiang(void);
void Debug_Delay_Check(void);
void Debug_IPD_Check(void);



void Debug_Signal_High(void);
void Debug_Signal_Low(void);
void Debug_ToolStartMotor(void);
void  Delay_1US_Debug(unsigned int timer);

void UC_CheckMos(void);
uint8_t UC_CheckSignlaDriver(void);
void HW_CheckSelf_Handler(void);
#endif /*  */
