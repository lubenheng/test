#ifndef INTERRUPT_CMDLINE_H
#define INTERRUPT_CMDLINE_H
#include "gpio_hal.h" 




int cmd_interrupt_test(int argc, char *argv[]);
void GPIO_P05_interrupt_init(void);
void GPIO_P15_interrupt_init(void);
void GPIO_P25_interrupt_init(void);
void GPIO_P35_interrupt_init(void);
void GPIO_P45_interrupt_init(void);

#define interrupt_CMDLINE_MENU \
		{"cmd_interrupt_test",cmd_interrupt_test,  " cmd_interrupt_test"}, \
	  {"cmd_interrupt_testQ",cmd_interrupt_test,  " cmd_interrupt_test"}, \


#endif
