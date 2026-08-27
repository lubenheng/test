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
/** \file hardinit.h
**
** 
**
** History:
** 
*****************************************************************************/
#ifndef __HARDINIT_H__
#define __HARDINIT_H__

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
#define	EPWM_ZIFn_Flag				    (0x1UL)
#define	EPWM_ClearZIFn_Flag()		  { EPWM->ICLR |= EPWM_ZIFn_Flag;}
#define	EPWM_ClearAllInt_Flag()		{ EPWM->ICLR |= 0xFFFFFFFF;}
#define	ADC1_ClearIntFlag_CH15()	{ ADC1->LOCK = ADC1_LOCK_WRITE_KEY; ADC1->ICLR |= ADC1_CH_15_MSK; ADC1->LOCK = 0x00;}
#define	ADC1_ClearIntFlag_CH23()	{ ADC1->LOCK = ADC1_LOCK_WRITE_KEY; ADC1->ICLR |= ADC1_CH_23_MSK; ADC1->LOCK = 0x00;}

/*****************************************************************************/
/* Global type definitions ('typedef') */
/*****************************************************************************/

/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source) */
/*****************************************************************************/


/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source) */
/*****************************************************************************/
void System_Init(void);
void WDT_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* __HARDINIT_H__ */




