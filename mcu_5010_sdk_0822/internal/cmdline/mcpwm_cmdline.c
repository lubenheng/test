#include "platform_common.h"


#include "cmdline_internal.h"
#include "mcpwm_cmdline.h"
#include "mcpwm_test.h"



int cmd_mcpwm_function1_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[0]);
	
	uint32_t para1 =  get_arg_ulong(argv[1]);
	uint32_t para2 =  get_arg_ulong(argv[2]);
	uint32_t para3 =  get_arg_ulong(argv[3]);

	/* add your code */
	
	mcpwm_func1_test(test_case, para1, para2);
	
		
	INFO("PASS\n");
	return 0;
}



int cmd_mcpwm_function2_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[0]);
	
	uint32_t para1 =  get_arg_ulong(argv[1]);
	uint32_t para2 =  get_arg_ulong(argv[2]);
	uint32_t para3 =  get_arg_ulong(argv[3]);

	/* add your code */
	
	mcpwm_func2_test(test_case, para1, para2);
	
	
		
	INFO("PASS\n");
	return 0;

}

int cmd_mcpwm_parameter_set(int argc, char *argv[])
{

//	INFO("argc=%d\n", argc);
//	
//	for(uint16_t i=0;i< argc;i++)
//	{
//		INFO("argv[%d]:%s\n", i,argv[i]);

//	}
	mcpwm_parameter_set(argc,argv);
	
	return 0;
}

int cmd_mcpwm_interrupt_source_test(int argc, char *argv[])
{
	mcpwm_interrupt_source_test(argc,argv);
	return 0;
}

int cmd_mcpwm_channel_n_p_inverse_test(int argc, char *argv[])
{
	mcpwm_channel_n_p_inverse_test(argc,argv);
	return 0;
}


int cmd_mcpwm_channel_swap_test(int argc, char *argv[])
{
	mcpwm_channel_swap_test(argc,argv);
	return 0;
}


int cmd_mcpwm_output_default_value_test(int argc, char *argv[])
{
	mcpwm_output_default_value_test(argc,argv);
	return 0;
}

/* 图 14-7MCPWM 时序 TH<n>0 和 TH<n>1-中心对齐模式 */
int cmd_mcpwm_center_align_test(int argc, char *argv[])
{
	mcpwm_center_align_test(argc,argv);
	return 0;
}

/* 图 14-8 MCPWM 时序 TH<n>0 和 TH<n>1-中心对齐推挽模式 */
int cmd_mcpwm_center_align_push_pull_test(int argc, char *argv[])
{
	mcpwm_center_align_push_pull_test(argc,argv);
	return 0;
}

/* 图 14-9MCPWM 时序边沿对齐模式 */
int cmd_mcpwm_edge_align_test(int argc, char *argv[])
{
	mcpwm_edge_align_test(argc,argv);
	return 0;
}

/* 图 14-10MCPWM 时序 TH<n>0 和 TH<n>1 边沿对齐推挽模式 */
int cmd_mcpwm_edge_align_push_pull_test(int argc, char *argv[])
{
	mcpwm_edge_align_push_pull_test(argc,argv);
	return 0;
}


int cmd_mcpwm_io_fault_test(int argc, char *argv[])
{
	mcpwm_io_fault_test(argc,argv);
	return 0;
}
