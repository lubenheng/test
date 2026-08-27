#include "platform_common.h"


#include "cmdline_internal.h"
#include "rtc_cmdline.h"
#include "rtc_test.h"



int cmd_rtc_function1_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[0]);
	
	uint32_t para1 =  get_arg_ulong(argv[1]);
	uint32_t para2 =  get_arg_ulong(argv[2]);
	uint32_t para3 =  get_arg_ulong(argv[3]);

	/* add your code */
	
	rtc_func1_test(test_case, para1, para2);
	
		
	INFO("PASS\n");
	return 0;
}



int cmd_rtc_function2_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[0]);
	
	uint32_t para1 =  get_arg_ulong(argv[1]);
	uint32_t para2 =  get_arg_ulong(argv[2]);
	uint32_t para3 =  get_arg_ulong(argv[3]);

	/* add your code */
	
	rtc_func2_test(test_case, para1, para2);
	
	
	
		
	INFO("PASS\n");
	return 0;

}
