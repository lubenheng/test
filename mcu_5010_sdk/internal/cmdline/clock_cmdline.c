#include "platform_common.h"


#include "cmdline_internal.h"
#include "clock_cmdline.h"
#include "clock_test.h"
//#include "clock_type.h"

#include "clock_hal.h"

int cmd_clock_function1_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[0]);
	
	//uint32_t para1 =  get_arg_ulong(argv[1]);
	uint32_t para2 =  get_arg_ulong(argv[2]);
	uint32_t para3 =  get_arg_ulong(argv[3]);

	/* add your code */
	
	clock_func1_test(test_case, para3, para2);
	
	//切换引脚的时钟源测试时钟并分频
	if(0 == strncmp(argv[1], "sysclk", 6)){
		rcc_hal_mco_clock_select(CLOCK_MCO_SYS);
	} else if(0 == strncmp(argv[1], "hsi", 3)){
		rcc_hal_mco_clock_select(CLOCK_MCO_HSI);
	} else if(0 == strncmp(argv[1], "pllclk", 6)){
		rcc_hal_mco_clock_select(CLOCK_MCO_PLLCLK);
	} else if(0 == strncmp(argv[1], "hse", 3)){
		rcc_hal_mco_clock_select(CLOCK_MCO_HSE);
	} else if(0 == strncmp(argv[1], "lsi", 3)){
		rcc_hal_mco_clock_select(CLOCK_MCO_LSI);
	} else{
		INFO("enter error!!\n");
		return 0;
	}
	switch (para2)
	{
	case 1:
		rcc_hal_mco_div(CLOCK_MCO_DIV1);
		break;
	case 2:
		rcc_hal_mco_div(CLOCK_MCO_DIV2);
		break;
	case 4:
		rcc_hal_mco_div(CLOCK_MCO_DIV4);
		break;
	case 8:
		rcc_hal_mco_div(CLOCK_MCO_DIV8);
		break;
	case 16:
		rcc_hal_mco_div(CLOCK_MCO_DIV16);
		break;
	case 32:
		rcc_hal_mco_div(CLOCK_MCO_DIV32);
		break;
	case 64:
		rcc_hal_mco_div(CLOCK_MCO_DIV64);
		break;
	case 128:
		rcc_hal_mco_div(CLOCK_MCO_DIV128);
		break;
	case 256:
		rcc_hal_mco_div(CLOCK_MCO_DIV256);
		break;
	default:
		INFO("input num is error!!\n");
		break;
	}

	INFO("PASS\n");
	return 0;
}



int cmd_clock_function2_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[0]);
	
	//uint32_t para1 =  get_arg_ulong(argv[1]);
	uint32_t para2 =  get_arg_ulong(argv[2]);
	uint32_t para3 =  get_arg_ulong(argv[3]);


//	clock_func2_test(test_case, para3, para2);

	/* add your code */

	//切换系统时钟
	/*
	 * 系统时钟分频并修改波特率，该场景适合用在高时钟频率，低波特率的场景。
	 * 时钟频率越低、波特率越高，UART的波特率寄存器参数越小，当时钟频率很低、波特率很高时，对应的参数无法满足要求只能等于0，
	 * 导致UART配置出来的波特率不匹配，UART发送乱码。
	*/
	if(0 == strncmp(argv[1], "hsi", 3)){
		rcc_hal_sys_change_clock(CLOCK_SYS_HSI, (clock_sys_fclk_div)para2, (clock_sys_pclk_div)para3);
	} else if(0 == strncmp(argv[1], "hse", 3)){
		rcc_hal_sys_change_clock(CLOCK_SYS_HSE, (clock_sys_fclk_div)para2, (clock_sys_pclk_div)para3);
	} else if(0 == strncmp(argv[1], "pllclk", 6)){
		rcc_hal_sys_change_clock(CLOCK_SYS_PLLCLK1, (clock_sys_fclk_div)para2, (clock_sys_pclk_div)para3);
	} else if(0 == strncmp(argv[1], "test", 4)){
		clock_func2_test(test_case, para2, para3);
	} else{
		INFO("Input error!!\n");
		return 0;
	}

	INFO("PASS\n");
	return 0;

}
