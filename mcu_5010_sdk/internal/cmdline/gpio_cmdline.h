#ifndef GPIO_CMDLINE_H
#define GPIO_CMDLINE_H


int cmd_gpio_function1_test(int argc, char *argv[]);
int cmd_gpio_function2_test(int argc, char *argv[]);



#define GPIO_CMDLINE_MENU 	{"cmd_gpio_function1_test",	        cmd_gpio_function1_test,		"  cmd_gpio_function1_test"},\
							{"cmd_gpio_function2_test", 	cmd_gpio_function2_test,	"  cmd_gpio_function2_test"},
											
#endif /* GPIO_CMDLINE_H */

