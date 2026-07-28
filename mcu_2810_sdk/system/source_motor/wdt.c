#include <stdio.h>
#include <string.h>
#include "CMSDK_CM0.h"
#include "system_CMSDK_CM0.h"
#include "core_cm0.h"
#include "Common.h"
#include "wdt.h"


//----------------------------------------------
void bootWDTOpen(bool en)
{
    uint8_t     tmp;

    if(en)
    {
        WDT->en  = 7;
        feedWDT();
    }
    else
    {
        WDT->en = 0;
        
        tmp = WDT->rst;
        tmp = (tmp >> 28) & 1;
        while (tmp == 1)
        {
            tmp = WDT->rst;
            tmp = (tmp >> 28) & 1;
        }
    }

    return;
}


//----------------------------------------------
void rldWDT(uint32_t rld)
{
    WDT->reload_val = rld;
    return;
}


//----------------------------------------------
void feedWDT(void)
{
    while ((WDT->rst) & 1)
    {
        __ISB();
    }

    WDT->rst = 1;

    return;
}


//----------------------------------------------
void appWDTOpen(uint8_t wdtEn, uint8_t wdt_time)
{
    uint8_t     tmp;

    if(wdtEn == APP_WDTEN_VALUE)
    {
        WDT->reload_val = (wdt_time << 10);
        WDT->en = 7;
        feedWDT();
    }
    else
    {
        WDT->en = 0;

        tmp = WDT->rst;
        tmp = (tmp >> 28) & 1;
        while (tmp == 1)
        {
            tmp = WDT->rst;
            tmp = (tmp >> 28) & 1;
        }
    }

    return;
}

