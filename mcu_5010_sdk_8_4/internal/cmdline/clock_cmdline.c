#include "platform_common.h"


#include "cmdline_internal.h"
#include "clock_cmdline.h"
#include "clock_test.h"
//#include "clock_type.h"

#include "clock_hal.h"

#include "trim_test.h"

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

	sys_clk_config_t sys_clk_cfg;
	sys_clk_cfg.pclk_div = (clock_sys_pclk_div)para3;
	sys_clk_cfg.fclk_div = (clock_sys_fclk_div)para2;
	sys_clk_cfg.pll_mult = 0;  // Set default value
	sys_clk_cfg.pll_div = 0;   // Set default value
//	clock_func2_test(test_case, para3, para2);

	/* add your code */

	//切换系统时钟
	/*
	 * 系统时钟分频并修改波特率，该场景适合用在高时钟频率，低波特率的场景。
	 * 时钟频率越低、波特率越高，UART的波特率寄存器参数越小，当时钟频率很低、波特率很高时，对应的参数无法满足要求只能等于0，
	 * 导致UART配置出来的波特率不匹配，UART发送乱码。
	*/
	if(0 == strncmp(argv[1], "hsi", 3)){
		sys_clk_cfg.sys_clk_sel = CLOCK_SYS_HSI;
		rcc_hal_sys_change_clock(&sys_clk_cfg);
	} else if(0 == strncmp(argv[1], "hse", 3)){
		sys_clk_cfg.sys_clk_sel = CLOCK_SYS_HSE;
		rcc_hal_sys_change_clock(&sys_clk_cfg);
	} else if(0 == strncmp(argv[1], "pllclk", 6)){
		sys_clk_cfg.sys_clk_sel = CLOCK_SYS_PLLCLK1;
		rcc_hal_sys_change_clock(&sys_clk_cfg);
	} else if(0 == strncmp(argv[1], "test", 4)){
		clock_func2_test(test_case, para2, para3);
	} else{
		INFO("Input error!!\n");
		return 0;
	}

	INFO("PASS\n");
	return 0;

}


int cmd_trim_test(int argc, char *argv[])
{
	
	if(!strncmp(argv[1], "init_test", 9)){
		//初始化指定测试项
		if(!strncmp(argv[2], "VBG", 3))
			VBG_Trim_Init();
		else if(!strncmp(argv[2], "VREF", 4))
			VREF_Trim_Init();
		else if(!strncmp(argv[2], "SLP_VREF", 8))
			SLP_VREF_Trim_Init();
		else if(!strncmp(argv[2], "LDO12", 5))
			LDO12_Trim_Init();
		else if(!strncmp(argv[2], "SLP_LDO12", 9))
			SLP_LDO12_Trim_Init();
		else if(!strncmp(argv[2], "ADC_VREFH", 9))
			ADC_VREFH_Trim_Init();
		else if(!strncmp(argv[2], "HSI", 3))
			HSI_Trim_Init();
		else if(!strncmp(argv[2], "LSI", 3))
			LSI_Trim_Init();
		else
			return -1;
	} else if(!strncmp(argv[1], "set_trim", 8)){
		//指定测试项的trim值写入
		uint8_t trim_value = (uint8_t)get_arg_ulong(argv[3]);

		if(!strncmp(argv[2], "VBG", 3))
			VBG_Trim_Set(trim_value);
		else if(!strncmp(argv[2], "VREF", 4))
			VREF_Trim_Set(trim_value);
		else if(!strncmp(argv[2], "SLP_VREF", 8))
			SLP_VREF_Trim_Set(trim_value);
		else if(!strncmp(argv[2], "LDO12", 5))
			LDO12_Trim_Set(trim_value);
		else if(!strncmp(argv[2], "SLP_LDO12", 9))
			SLP_LDO12_Trim_Set(trim_value);
		else if(!strncmp(argv[2], "ADC_VREFH", 9))
			ADC_VREFH_Trim_Set(trim_value);
		else if(!strncmp(argv[2], "HSI", 3))
			HSI_Trim_Set(trim_value);
		else if(!strncmp(argv[2], "LSI", 3))
			LSI_Trim_Set(trim_value);
		else
			return -1;
		
	} else if(!strncmp(argv[1], "end_test", 8)){
		//结束测试项
		trim_test_end();
	} else {
		return -1;
	}

	INFO("PASS\n");
	return 0;
}








