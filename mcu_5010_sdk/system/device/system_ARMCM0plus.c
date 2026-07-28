/**************************************************************************//**
 * @file     system_ARMCM0plus.c
 * @brief    CMSIS Device System Source File for
 *           ARMCM0plus Device
 * @version  V1.0.0
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

 
#include "ARMCM0plus.h"

#include "sys_config.h"

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define  XTAL            (50000000UL)     /* Oscillator frequency */

#define  SYSTEM_CLOCK    (XTAL / 2U)


uint32_t g_unit_1us = SYS_CORE_CLK/1000000U;
uint32_t g_unit_ms  = SYS_CORE_CLK/1000U;

/*----------------------------------------------------------------------------
  System Core Clock Variable
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = SYSTEM_CLOCK;  /* System Core Clock Frequency */


/*----------------------------------------------------------------------------
  System Core Clock update function
 *----------------------------------------------------------------------------*/
void SystemCoreClockUpdate (void)
{
  SystemCoreClock = SYSTEM_CLOCK;
}

/*----------------------------------------------------------------------------
  System initialization function
 *----------------------------------------------------------------------------*/
void SystemInit (void)
{

#if defined (__VTOR_PRESENT) && (__VTOR_PRESENT == 1U)
  SCB->VTOR = (uint32_t) &(__VECTOR_TABLE);
#endif

  SystemCoreClock = SYSTEM_CLOCK;
}


/**
*@brief 	 : delay time of 10us unit.
*@note		 : This delay is implemented by the cost time of the instruction execution, 
* 				the delay time had been fine tuned and verified on the MDK and IAR, 
* 				please fine tune it again when change to other Compliers or other
* 				verions of MDK or IAR
*@param[in]  : number of delay time unit. unit: 10us
*@return	 : none
*/
void delay_1us(uint32_t val)
{
	while (val)
	{
		for (uint32_t i=0; i<g_unit_1us; i++)
		{
			__NOP();
		}

		val--;
	}

}


/**
*@brief 	 : delay time of 1ms unit.
*@note		 : This delay is implemented by the cost time of the instruction execution, 
* 				the delay time had been fine tuned and verified on the MDK and IAR, 
* 				please fine tune it again when change to other Compliers or other
* 				verions of MDK or IAR
*@param[in]  : number of delay time unit. unit: 1ms
*@return	 : none
*/
void delay_ms(uint32_t val)
{
	while (val)
	{
		for (uint32_t i=0; i<g_unit_ms; i++)
		{
			__NOP();
		}

		val--;
	}
	
}
