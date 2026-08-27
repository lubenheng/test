#include "platform_common.h"


#include "cmdline_internal.h"
#include "hall_cmdline.h"
#include "hall_test.h"



int cmd_hall_function1_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[0]);
	
	uint32_t para1 =  get_arg_ulong(argv[1]);
	uint32_t para2 =  get_arg_ulong(argv[2]);
	uint32_t para3 =  get_arg_ulong(argv[3]);

	/* add your code */
	
	hall_func1_test(test_case, para1, para2);
	
		
	INFO("PASS\n");
	return 0;
}


int cmd_hall_function2_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[0]);
	
	uint32_t para1 =  get_arg_ulong(argv[1]);
	uint32_t para2 =  get_arg_ulong(argv[2]);
	uint32_t para3 =  get_arg_ulong(argv[3]);

	/* add your code */
	hall_func2_test(test_case, para1, para2);
		
	INFO("PASS\n");
	return 0;
}


int cmd_hall_soft_trig_start_test(int argc, char *argv[])
{
	/* add your code */
	hall_soft_trig_start_test();
		
	INFO("cmd_hall_software_trigger_interupt PASS\n");

	return 0;
}
