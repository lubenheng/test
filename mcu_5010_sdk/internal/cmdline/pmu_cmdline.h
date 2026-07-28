#ifndef PMU_CMDLINE_H
#define PMU_CMDLINE_H



int cmd_pmu_function1_test(int argc, char *argv[]);
int cmd_pmu_function2_test(int argc, char *argv[]);

int cmd_pmu_gpio_wakeup_test(int argc, char *argv[]);
int cmd_pmu_lptimer_wakeup_test(int argc, char *argv[]);
int cmd_pmu_rstn_wakeup_test(int argc, char *argv[]);



#define PMU_CMDLINE_MENU 	{"cmd_pmu_gpio_wakeup_test", 		cmd_pmu_gpio_wakeup_test,		"  gpio_source,wake_mode,int_enable"},\
							{"cmd_pmu_lptimer_wakeup_test", 	cmd_pmu_lptimer_wakeup_test,	"  cmd_pmu_lptimer_wakeup_test"},\
							{"cmd_pmu_rstn_wakeup_test", 		cmd_pmu_rstn_wakeup_test,		"  cmd_pmu_rstn_wakeup_test"},\
											
#endif /* PMU_CMDLINE_H */

