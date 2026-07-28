#ifndef QEP_CMDLINE_H
#define QEP_CMDLINE_H



int cmd_qep_function1_test(int argc, char *argv[]);
int cmd_qep_function2_test(int argc, char *argv[]);

int cmd_qep_mode_test(int argc, char *argv[]);



#define QEP_CMDLINE_MENU 	{"cmd_qep_mode_test", 		cmd_qep_mode_test,			"  cmd_qep_mode_test"},\
							{"cmd_qep_function1_test",	cmd_qep_function1_test,		"  cmd_qep_function1_test"},\
							{"cmd_qep_function2_test", 	cmd_qep_function2_test,		"  cmd_qep_function2_test"},
											
#endif /* QEP_CMDLINE_H */

