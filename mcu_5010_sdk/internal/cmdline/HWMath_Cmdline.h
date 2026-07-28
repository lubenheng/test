#ifndef HWMATH_CMDLINE_H
#define HWMATH_CMDLINE_H



//int cmd_mcpwm_function1_test(int argc, char *argv[]);
//int cmd_mcpwm_function2_test(int argc, char *argv[]);

int cmd_hwmath_test_div(int argc, char *argv[]);
int cmd_hwmath_test_mod(int argc, char *argv[]);
int cmd_hwmath_test_sqrt(int argc, char *argv[]);
int cmd_hwmath_test_sincos(int argc, char *argv[]);
int cmd_hwmath_test_sin(int argc, char *argv[]);
int cmd_hwmath_test_cos(int argc, char *argv[]);
int cmd_hwmath_test_Err(int argc, char *argv[]);

#define HWMATH_CMDLINE_MENU 	{"cmd_hwmath_test_div",	   cmd_hwmath_test_div,    "cmd_hwmath_test_div"},     \
							                {"cmd_hwmath_test_mod", 	 cmd_hwmath_test_mod,    " cmd_hwmath_test_mod"},    \
							                {"cmd_hwmath_test_sqrt", 	 cmd_hwmath_test_sqrt,   " cmd_hwmath_test_sqrt"},   \
							                {"cmd_hwmath_test_sincos", cmd_hwmath_test_sincos, " cmd_hwmath_test_sincos"}, \
							                {"cmd_hwmath_test_sin",    cmd_hwmath_test_sin,    " cmd_hwmath_test_sin"},    \
							                {"cmd_hwmath_test_cos",    cmd_hwmath_test_cos,    " cmd_hwmath_test_cos"},		 \
							                {"cmd_hwmath_test_Err",    cmd_hwmath_test_Err,    " cmd_hwmath_test_Err"},

#endif /* HWMATH_CMDLINE_H */
