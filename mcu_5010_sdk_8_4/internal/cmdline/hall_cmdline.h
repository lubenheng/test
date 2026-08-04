#ifndef HALL_CMDLINE_H
#define HALL_CMDLINE_H



int cmd_hall_function1_test(int argc, char *argv[]);
int cmd_hall_function2_test(int argc, char *argv[]);
int cmd_hall_soft_trig_start_test(int argc, char *argv[]);



#define HALL_CMDLINE_MENU 	{"cmd_hall_function1_test",			cmd_hall_function1_test,		"  cmd_hall_function1_test"},\
							{"cmd_hall_function2_test", 		cmd_hall_function2_test,		"  cmd_hall_function2_test"},\
							{"cmd_hall_soft_trig_start_test", 	cmd_hall_soft_trig_start_test,	"  cmd_hall_soft_trig_start_test"},
											
#endif /* HALL_CMDLINE_H */

