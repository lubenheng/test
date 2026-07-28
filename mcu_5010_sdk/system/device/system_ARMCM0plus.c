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


volatile uint32_t g_systick_timeout = 0;
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
 *@brief   SysTick-based microsecond delay (1us per unit).
 *@note    Uses Cortex-M0+ SysTick COUNTFLAG polling (no interrupt required).
 *         Works even before __enable_irq() is called.
 *         Precision is hardware-guaranteed, independent of compiler optimisation.
 *@param[in]  val : number of 1us units to delay
 *@return   none
 */
void delay_1us(uint32_t val)
{
    if (val == 0) return;

    /* Configure SysTick for 1us tick (polling mode, no interrupt) */
    uint32_t tick_per_us = SYS_CORE_CLK / 1000000U;
    SysTick->LOAD = tick_per_us - 1;
    SysTick->VAL  = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |   /* HCLK */
                    SysTick_CTRL_ENABLE_Msk;         /* enabled, no TICKINT */

    while (val)
    {
        /* Wait for COUNTFLAG: set when counter reaches 0 */
        while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)) {}
        val--;
    }

    SysTick->CTRL = 0;   /* disable SysTick */
}


/**
 *@brief   SysTick-based millisecond delay (1ms per unit).
 *@note    Uses Cortex-M0+ SysTick COUNTFLAG polling (no interrupt required).
 *         Works even before __enable_irq() is called.
 *         When SYS_CORE_CLK > 24 MHz, 1ms reload exceeds 24-bit LOAD,
 *         so we loop 1000 x 1us instead.
 *@param[in]  val : number of 1ms units to delay
 *@return   none
 */
void delay_ms(uint32_t val)
{
    if (val == 0) return;

    /* Configure SysTick for 1us tick */
    uint32_t tick_per_us = SYS_CORE_CLK / 1000000U;
    SysTick->LOAD = tick_per_us - 1;
    SysTick->VAL  = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_ENABLE_Msk;         /* no TICKINT */

    while (val)
    {
        /* 1 ms = 1000 us */
        for (uint32_t us = 0; us < 1000; us++)
        {
            while (!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk)) {}
        }
        val--;
    }

    SysTick->CTRL = 0;
}
