#ifndef WDT_H
#define WDT_H


void bootWDTOpen(bool en);
void rldWDT(uint32_t rld);
void feedWDT(void);
void appWDTOpen(uint8_t wdtEn, uint8_t wdt_time);


#define WDT_SHORT_WAIT  1600 
#define WDT_LONG_WAIT   16000
#define APP_WDTEN_VALUE 0x0A 




#endif
