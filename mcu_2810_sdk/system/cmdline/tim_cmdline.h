#ifndef TIM_CMDLINE_H
#define TIM_CMDLINE_H
#include "gpio_hal.h" 


int cmd_timx_toggle_test(int argc, char *argv[]);
void gpio_test_pin(void);
void tim0_init(uint32_t periodx)  ;
void tim5_init(uint32_t periodx) ;
void tim6_init(uint32_t periodx) ;
void tim3_init(uint32_t periodx)  ;

#define TIM_CMDLINE_MENU \
		{"cmd_timx_toggle_test",cmd_timx_toggle_test,  " cmd_timx_toggle_test"}, \
	 



#endif
