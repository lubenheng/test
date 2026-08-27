#include "platform_common.h"


#include "cmdline_internal.h"
#include "pmu_cmdline.h"
#include "pmu_test.h"



int cmd_pmu_function1_test(int argc, char *argv[])
{
	/* add your code */
	
	pmu_func1_test(argc,argv);
	
	INFO("cmd_pmu_function1_test PASS\n");

	return 0;
}



int cmd_pmu_function2_test(int argc, char *argv[])
{
	/* add your code */
	
	pmu_func2_test(argc,argv);
	
	INFO("cmd_pmu_function2_test PASS\n");

	return 0;
}


int cmd_pmu_gpio_wakeup_test(int argc, char *argv[])
{
	pmu_gpio_wakeup_test(argc,argv);
	return 0;
}
int cmd_pmu_lptimer_wakeup_test(int argc, char *argv[])
{
	pmu_lptimer_wakeup_test(argc,argv);
	return 0;
}
int cmd_pmu_rstn_wakeup_test(int argc, char *argv[])
{	
	pmu_rstn_wakeup_test(argc,argv);
	return 0;

}
