#include "platform_common.h"


#include "cmdline_internal.h"
#include "can_cmdline.h"
#include "can_test.h"



int cmd_can_function1_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[0]);
	
	uint32_t para1 =  get_arg_ulong(argv[1]);
	uint32_t para2 =  get_arg_ulong(argv[2]);
	uint32_t para3 =  get_arg_ulong(argv[3]);

	/* add your code */
	
	can_func1_test(test_case, para1, para2);
	
		
	INFO("can_func1_test PASS\n");
	return 0;
}



int cmd_can_function2_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[0]);
	
	uint32_t para1 =  get_arg_ulong(argv[1]);
	uint32_t para2 =  get_arg_ulong(argv[2]);
	uint32_t para3 =  get_arg_ulong(argv[3]);

	/* add your code */
	
	can_func2_test(test_case, para1, para2);
	
	
	
		
	INFO("can_func2_test PASS\n");
	return 0;

}
