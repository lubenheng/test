#include "platform_common.h"


#include "cmdline_internal.h"
#include "wdog_cmdline.h"
#include "wdog_test.h"



int cmd_wdog_function1_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[0]);
	
	uint32_t para1 =  get_arg_ulong(argv[1]);
	uint32_t para2 =  get_arg_ulong(argv[2]);
	uint32_t para3 =  get_arg_ulong(argv[3]);

	/* add your code */

	for(uint16_t i=0;i< 5;i++)
	{
		INFO("argv[%d]:%s\n", i,argv[i]);
	}
	
	wdog_func1_test(test_case, para1, para2);
	
	
	INFO("PASS\n");
	return 0;
}



int cmd_wdog_function2_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[0]);
	
	uint32_t para1 =  get_arg_ulong(argv[1]);
	uint32_t para2 =  get_arg_ulong(argv[2]);
	uint32_t para3 =  get_arg_ulong(argv[3]);

	/* add your code */
	
	wdog_func2_test(test_case, para1, para2);
	
	
	
		
	INFO("PASS\n");
	return 0;

}
