#ifndef TIM_CMDLINE_H
#define TIM_CMDLINE_H

#include "TIM_hal.h"


int cmd_TIM_function1_test(int argc, char *argv[]);
int cmd_TIM_function2_test(int argc, char *argv[]);


int8_t tim_cmdline_tim_change(char *argx, TIMx_TypeDef **timex);
int tim_cmdline_operation(char *argx, TIMx_TypeDef *timex);
int8_t tim_cmdline_ch_select(char *argx);
int8_t tim_cmdline_execute(TIMx_TypeDef *timex, int option, int8_t chx, char *ext1, uint32_t ext2);


#define TIM_CMDLINE_MENU 	{"cmd_TIM_function1_test",	cmd_TIM_function1_test,	"  cmd_TIM_function1_test"},\
								{"cmd_TIM_function2_test", 	cmd_TIM_function2_test,	"  cmd_TIM_function2_test"},
											
#endif /* TIM_CMDLINE_H */

