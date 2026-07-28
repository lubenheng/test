
#include <stdint.h>
#include "CMSDK_CM0.h"



#define XTAL    (32000000UL)            /* Oscillator frequency               */



uint32_t SystemFrequency = XTAL;    /*!< System Clock Frequency (Core Clock)  */
uint32_t SystemCoreClock = XTAL;    /*!< Processor Clock Frequency            */



void SystemCoreClockUpdate (void)            /* Get Core Clock Frequency      */
{
    SystemCoreClock = XTAL;
}



void SystemInit (void)
{
    SystemCoreClock = XTAL;
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

// Keil 编译器专用：禁止优化此函数（M0/M0+ 都通用）
#pragma O0
void soft_delay_1ms(uint32_t val)
{
    // 48MHz M0/M0+ 精准 1ms
    const uint32_t g_unit_ms = 12000;

    while (val)
    {
        for (uint32_t i = 0; i < g_unit_ms; i++)
        {
            __NOP();
        }
        val--;
    }
}



