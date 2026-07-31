#ifndef CLOCK_CMDLINE_H
#define CLOCK_CMDLINE_H

int cmd_clock_function1_test(int argc, char *argv[]);
int cmd_clock_function2_test(int argc, char *argv[]);



#define CLOCK_CMDLINE_MENU 	{"cmd_clock_function1_test",	cmd_clock_function1_test,	"  cmd_clock_function1_test"},\
														{"cmd_clock_function2_test", 	cmd_clock_function2_test,	"  cmd_clock_function2_test"},
											
#endif /* CLOCK_CMDLINE_H */

