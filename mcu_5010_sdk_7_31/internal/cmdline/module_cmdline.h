#ifndef MODULE_CMDLINE_H
#define MODULE_CMDLINE_H



int cmd_module_function1_test(int argc, char *argv[]);
int cmd_module_function2_test(int argc, char *argv[]);



#define MODULE_CMDLINE_MENU 	{"cmd_module_function1_test",	cmd_module_function1_test,	"  cmd_module_function1_test"},\
								{"cmd_module_function2_test", 	cmd_module_function2_test,	"  cmd_module_function2_test"},
											
#endif /* MODULE_CMDLINE_H */

