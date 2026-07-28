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
#include <stdint.h>
#include  "para_define.h"
#include  "user_control.h"
/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define') */
/*****************************************************************************/
#define	EPWM_ZIFn_Flag				    (0x1UL)
#define	EPWM_ClearZIFn_Flag()		  { EPWM->ICLR |= EPWM_ZIFn_Flag;}
#define	EPWM_ClearAllInt_Flag()		{ EPWM->ICLR |= 0xFFFFFFFF;}
#define	ADC_ClearIntFlag_CH2()	{ ADC->LOCK = ADC_LOCK_WRITE_KEY; ADC->ICLR |= ADC_CH_2_MSK; ADC->LOCK = 0x00;}
#define	ADC_ClearIntFlag_CH3()	{ ADC->LOCK = ADC_LOCK_WRITE_KEY; ADC->ICLR |= ADC_CH_3_MSK; ADC->LOCK = 0x00;}

//#define	ADC_ClearIntFlag_CHA()	{ ADC->LOCK = ADC_LOCK_WRITE_KEY; ADC->ICLR |= ADC_SCAN_CHA; ADC->LOCK = 0x00;}

#if (Config_Shunt_Mode == Single_Shunt)

	#define		ADC_INTER_CH                       (ADC_SCAN_CHA)		

#elif (Config_Shunt_Mode == Double_Shunt)

	#if (ADC_SCAN_CHA > ADC_SCAN_CHB)
		#define		ADC_INTER_CH                       (ADC_SCAN_CHA)				
	#else
		#define		ADC_INTER_CH                       (ADC_SCAN_CHB)	
	#endif

#elif (Config_Shunt_Mode == Treble_Shunt)

	#if ((ADC_SCAN_CHA > ADC_SCAN_CHB) && (ADC_SCAN_CHA > ADC_SCAN_CHC))
		#define		ADC_INTER_CH                       (ADC_SCAN_CHA)				
	#elif (ADC_SCAN_CHB > ADC_SCAN_CHC)
		#define		ADC_INTER_CH                       (ADC_SCAN_CHB)	
	#else
		#define		ADC_INTER_CH                       (ADC_SCAN_CHC)	
	#endif

#endif

#define	ADC_ClearIntFlag_CHA() 	{ ADC->LOCK = ADC_LOCK_WRITE_KEY; ADC->ICLR |= ADC_INTER_CH; ADC->LOCK = 0x00;}

//---------------------------------------------------------------------------//
// ADC_DATA_CHA 意义为采样通道 1 ，ADC_DATA_CHB意义为采样通道2，与相线对应关系不完全固定
// 67xx以下通道无需变更，仅需设置PGA通道与相序
#if (Config_Shunt_Mode == Single_Shunt)

	#define  ADC_DATA_CHA                       (ADC_CH_0)
	#define  ADC_SCAN_CHA                       (ADC_CH_0_MSK)

	#define  ADC_DATA_CHB                       (ADC_CH_1)
	#define  ADC_SCAN_CHB                       (ADC_CH_1_MSK)

#elif (Config_Shunt_Mode == Double_Shunt)

	#if (PGA_CH == PGA_1_2)
		#define  ADC_DATA_CHA                       (ADC_CH_2)			
		#define  ADC_SCAN_CHA                       (ADC_CH_2_MSK)	

		#define  ADC_DATA_CHB                       (ADC_CH_3)
		#define  ADC_SCAN_CHB                       (ADC_CH_3_MSK)

	#elif (PGA_CH == PGA_1_3)
		#define  ADC_DATA_CHA                       (ADC_CH_2)			
		#define  ADC_SCAN_CHA                       (ADC_CH_2_MSK)	

		#define  ADC_DATA_CHB                       (ADC_CH_4)
		#define  ADC_SCAN_CHB                       (ADC_CH_4_MSK)

	#elif (PGA_CH == PGA_2_3)
		#define  ADC_DATA_CHA                       (ADC_CH_3)			
		#define  ADC_SCAN_CHA                       (ADC_CH_3_MSK)	

		#define  ADC_DATA_CHB                       (ADC_CH_4)
		#define  ADC_SCAN_CHB                       (ADC_CH_4_MSK)

	#endif

#elif (Config_Shunt_Mode == Treble_Shunt)

	#define  ADC_DATA_CHA                       (ADC_CH_2)			
	#define  ADC_SCAN_CHA                       (ADC_CH_2_MSK)	

	#define  ADC_DATA_CHB                       (ADC_CH_3)
	#define  ADC_SCAN_CHB                       (ADC_CH_3_MSK)

	#define  ADC_DATA_CHC                       (ADC_CH_4)
	#define  ADC_SCAN_CHC                       (ADC_CH_4_MSK)

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
void System_Init(void);
void WDT_Init(void);
int32_t HWDIV_Div(int32_t x, int32_t y);
void UART0_Init(void);
void UART1_init(void);
void PUM_init(void);
void HALL_Init(void);
void ADC_Init(void);
void MUD_init(void);
void PGA_init(void);
void FOC_Init(void);
void EPWM_init(uint32_t arr,uint16_t deadtime);
void Driver_init(void);
#ifdef __cplusplus
}
#endif

#endif /* __HARDINIT_H__ */





