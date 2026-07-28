#ifndef CLOCK_CMDLINE_H
#define CLOCK_CMDLINE_H
 
#include "stdint.h"
 
void Clock_Close(void);
void Clock_Open(void);
void Clock_Close_All(void);
void Clock_Open_All(void);
int cmd_clock_test(int argc, char *argv[]);
void module_enable(uint8_t cmd);


#define CLOCK_CMDLINE_MENU \
		{"cmd_clock_test",cmd_clock_test,  " cmd_clock_test"}, 



#endif
