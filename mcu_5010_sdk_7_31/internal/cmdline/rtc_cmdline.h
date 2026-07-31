#ifndef RTC_CMDLINE_H
#define RTC_CMDLINE_H



int cmd_rtc_function1_test(int argc, char *argv[]);
int cmd_rtc_function2_test(int argc, char *argv[]);



#define RTC_CMDLINE_MENU 	{"cmd_rtc_function1_test",	cmd_rtc_function1_test,	"  cmd_rtc_function1_test"},\
							{"cmd_rtc_function2_test", 	cmd_rtc_function2_test,	"  cmd_rtc_function2_test"},
											
#endif /* RTC_CMDLINE_H */

