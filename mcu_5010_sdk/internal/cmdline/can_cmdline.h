#ifndef CAN_CMDLINE_H
#define CAN_CMDLINE_H



int cmd_can_function1_test(int argc, char *argv[]);
int cmd_can_function2_test(int argc, char *argv[]);



#define CAN_CMDLINE_MENU 	{"cmd_can_function1_test",	cmd_can_function1_test,	"  cmd_can_function1_test"},\
							{"cmd_can_function2_test", 	cmd_can_function2_test,	"  cmd_can_function2_test"},
											
#endif /* CAN_CMDLINE_H */

