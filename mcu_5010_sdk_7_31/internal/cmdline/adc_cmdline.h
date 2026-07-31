#ifndef ADC_CMDLINE_H
#define ADC_CMDLINE_H

int cmd_adc_function1_test(int argc, char *argv[]);
int cmd_adc_function2_test(int argc, char *argv[]);



#define ADC_CMDLINE_MENU 	{"cmd_adc_function1_test",	        cmd_adc_function1_test,		"  cmd_adc_function1_test"},\
							{"cmd_adc_function2_test", 			cmd_adc_function2_test,		"  cmd_adc_function2_test"},
											
#endif /* ADC_CMDLINE_H */

