#ifndef WDT_H
#define WDT_H

#include <stdint.h>
#include <stdbool.h>
#include "CMSDK_CM0.h"

void bootWDTOpen(bool en);
void rldWDT(uint32_t rld);
void feedWDT(void);
void appWDTOpen(uint8_t wdtEn, uint8_t wdt_time);


#define WDT_SHORT_WAIT  1600 
#define WDT_LONG_WAIT   16000
#define APP_WDTEN_VALUE 0x0A 



/************************* º¯ÊýÉùÃ÷ *************************/
void bootWDTOpen(bool en);
void rldWDT(uint32_t rld);
void feedWDT(void);
void appWDTOpen(uint8_t wdtEn, uint8_t wdt_time);

//void Wdog_Feed(void);
//uint32_t Wdog_Read_Cnt(void);
//uint8_t Wdog_Read_Fail(void);
//uint32_t Wdog_Read_Reg(wdog_reg reg);

//void Wdog_Reload_Set(uint32_t Value, uint8_t En);
//void Wdog_Interrupt_Set(void);
//void Wdog_Status_Clean(void);
//uint8_t Wdog_Status_Get(void);
#endif
