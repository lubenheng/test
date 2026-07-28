#ifndef WDOG_CMDLINE_H
#define WDOG_CMDLINE_H



int cmd_wdog_function1_test(int argc, char *argv[]);
int cmd_wdog_function2_test(int argc, char *argv[]);



#define WDOG_CMDLINE_MENU 	{"cmd_wdog_function1_test",	cmd_wdog_function1_test,	"  cmd_wdog_function1_test"},\
							{"cmd_wdog_function2_test", 	cmd_wdog_function2_test,	"  cmd_wdog_function2_test"},
											
#endif /* WDOG_CMDLINE_H */

