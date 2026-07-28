
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
