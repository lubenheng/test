/**************************************************************************//**
 * @file     ARMCM0plus.h
 * @brief    CMSIS Core Peripheral Access Layer Header File for
 *           ARMCM0plus Device
 * @version  V5.3.1
 * @date     09. July 2018
 ******************************************************************************/
/*
 * Copyright (c) 2009-2018 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ARMCM0plus_H
#define ARMCM0plus_H

#ifdef __cplusplus
extern "C" {
#endif


/* -------------------------  Interrupt Number Definition  ------------------------ */

typedef enum IRQn
{
/* -------------------  Processor Exceptions Numbers  ----------------------------- */
  NonMaskableInt_IRQn           = -14,     /*  2 Non Maskable Interrupt */
  HardFault_IRQn                = -13,     /*  3 HardFault Interrupt */



  SVCall_IRQn                   =  -5,     /* 11 SV Call Interrupt */

  PendSV_IRQn                   =  -2,     /* 14 Pend SV Interrupt */
  SysTick_IRQn                  =  -1,     /* 15 System Tick Interrupt */

/* -------------------  Processor Interrupt Numbers  ------------------------------ */
	PMU_IRQn                    =   0,
	Code_Flash_IRQn                 =   1,
	UART0_IRQn                      =   2,
	UART1_IRQn                      =   3,
	SPI0_IRQn                       =   4,
	SPI1_IRQn                       =   5,
	MCPWM0_IRQn                     =   6,
	MCPWM1_IRQn                     =   7,
	I2C0_IRQn                       =   8,
	I2C1_IRQn                       =   9,
	GPIO_A_IRQn 					=   10,
	GPIO_B_IRQn 					=   11,
	GPIO_C_IRQn 					=   12,  
	TIM13_IRQn 						=   13,  
	TIM14_IRQn 						=   14,  
	TIM15_IRQn 						=   15,  
	TIM16_IRQn 						=   16,  
	ADC_IRQn 						=   17,  
	HALL_IRQn 						=   18,  
	QEP_IRQn 						=   19,  
	DMA_IRQn 						=   20,  
	Reserved21_IRQn 				=   21,  
	Reserved22_IRQn 				=   22,  
	Reserved23_IRQn 				=   23,  
	Reserved24_IRQn 				=   24,  
	Reserved25_IRQn 				=   25,  
	Reserved26_IRQn 				=   26,  
	Reserved27_IRQn 				=   27,  
	Reserved28_IRQn 				=   28,  
	Reserved29_IRQn 				=   29,  
	Reserved30_IRQn 				=   30,  
	Reserved31_IRQn 				=   31, 
  /* Interrupts 10 .. 31 are left out */
} IRQn_Type;


/* ================================================================================ */
/* ================      Processor and Core Peripheral Section     ================ */
/* ================================================================================ */

/* -------  Start of section using anonymous unions and disabling warnings  ------- */
#if   defined (__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined (__ICCARM__)
  #pragma language=extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
  #pragma clang diagnostic ignored "-Wreserved-id-macro"
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning 586
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif


/* --------  Configuration of Core Peripherals  ----------------------------------- */
#define __CM0PLUS_REV             0x0001U   /* Core revision r0p1 */
#define __MPU_PRESENT             0U        /* no MPU present */
#define __VTOR_PRESENT            0U        /* no VTOR present */
#define __NVIC_PRIO_BITS          2U        /* Number of Bits used for Priority Levels */
#define __Vendor_SysTickConfig    0U        /* Set to 1 if different SysTick Config is used */

#include "core_cm0plus.h"                   /* Processor and core peripherals */
#include "system_ARMCM0plus.h"              /* System Header */


/* --------  End of section using anonymous unions and disabling warnings  -------- */
#if   defined (__CC_ARM)
  #pragma pop
#elif defined (__ICCARM__)
  /* leave anonymous unions enabled */
#elif (defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
  #pragma clang diagnostic pop
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning restore
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif


#ifdef __cplusplus
}
#endif

#endif  /* ARMCM0plus_H */
