#include "platform_common.h"


#include "cmdline_internal.h"
#include "TIM_cmdline.h"
#include "TIM_hal.h"
#include "TIM_test.h"

/*
 *		argv[1]			argv[2]			argv[3]			argv[4]			argv[5]					//解释
 *		定时器			操作选项		 定时器通道		  操作选择(字符)   操作选择(数字)
 *示例：
 *		TIMER13		0	cnt_init		none			none			-1					TIMER13计数功能初始化
 *		TIMER14		0	cmp_init		none			none			-1					TIMER14输出比较初始化
 *		TIMER15		0	cap_init		none			none			-1					TIMER15输入捕获初始化
 *		TIMER16		0	mix_init		none			none			-1					TIMER16混合测试初始化
 *		TIMER15		0	tim_enable		none			none			-1					TIMER15使能
 *		TIMER16		0	tim_disable		none			none			-1					TIMER16失能
 *		TIMER13		0	one_trig		none			none			-1					TIMER13进行单次发送模式（该模式需失能EN，且只在输出模式使用）
 *		TIMER13		1	change_div		none			none			32					TIMER13时钟32倍分频
 *		TIMER16		2	change_cntdir	none			up				-1					TIMER16的计数器计数方向为向上计数
 *		TIMER13			change_cntdir	none			down			-1					TIMER13的计数器计数方向为向下计数
 *		TIMER13		3	change_mid		none			edge			-1					TIMER13更改计数模式为边沿模式
 *		TIMER14			change_mid		none			mid				-1					TIMER14更改计数模式为中心模式
 *		TIMER14		4	change_cycle	none			none			1000				TIMER14更改门限值为1000
 *		TIMER14		5	change_pol		ch0				posity			-1					TIMER14更改CH0的输出极性为高电平
 *		TIMER15			change_pol		ch1				negative		-1					TIMER15更改CH1的输出极性为低电平
 *		TIMER15		6	change_duty		ch0				none			80					TIMER15更改CH0输出的PWM的占空比为(80/门限值(周期值)).输入值为0-门限值。
 *		TIMER16		7	change_flt		none			none			255					TIMER16设置滤波系数为255(最大是255，最小是0，不滤波)
*/

int cmd_TIM_function1_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[0]);
	uint32_t para5 =  get_arg_ulong(argv[5]);
	if(0 != tim_cmdline_tim_change(argv[1])){
		INFO("input argv1 error!!\n");
		return -1;
	}
	tim_cmdline_execute(tim_cmdline_operation(argv[2]), tim_cmdline_ch_select(argv[3]), argv[4], para5);


	delay_1us(10000);
	tim_hal_para_display();
	INFO("PASS\n");
	return 0;
}



int cmd_TIM_function2_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[0]);
	
	uint32_t para1 =  get_arg_ulong(argv[1]);
	uint32_t para2 =  get_arg_ulong(argv[2]);
	uint32_t para3 =  get_arg_ulong(argv[3]);

	/* add your code */
	
	TIM_func2_test(test_case, para1, para2);
	
	
	
		
	INFO("PASS\n");
	return 0;

}

int8_t tim_cmdline_tim_change(char *argx)
{
	if(0 == strncmp(argx, "TIMER13", 7)){
		return tim_hal_change(0);
	}else if(0 == strncmp(argx, "TIMER14", 7)){
		return tim_hal_change(1);
	}else if(0 == strncmp(argx, "TIMER15", 7)){
		return tim_hal_change(2);
	}else if(0 == strncmp(argx, "TIMER16", 7)){
		return tim_hal_change(3);
	} else{
		return 1;
	}
}

int tim_cmdline_operation(char *argx)
{
	if(0 == strncmp(argx, "cnt_init", 8)){
		tim_hal_init();
		tim_hal_irq_disable();
		tim_hal_disable();
		TIM_hal_Counter_init();
		return 0;
	} else if(0 == strncmp(argx, "cmp_init", 8)){
		tim_hal_init();
		tim_hal_irq_disable();
		tim_hal_disable();
		TIM_hal_Compare_init();
		return 0;
	} else if(0 == strncmp(argx, "cap_init", 8)){
		tim_hal_init();
		tim_hal_irq_disable();
		tim_hal_disable();
		TIM_hal_Capture_init();
		return 0;
	} else if(0 == strncmp(argx, "mix_init", 8)){
		tim_hal_init();
		tim_hal_irq_disable();
		tim_hal_disable();
		TIM_hal_mix_init();
		return 0;
	} else if(0 == strncmp(argx, "tim_enable", 10)){
		tim_hal_enable();
		return 0;
	} else if(0 == strncmp(argx, "tim_disable", 11)){
		tim_hal_disable();
		return 0;
	} else if(0 == strncmp(argx, "one_trig", 8)){
		tim_hal_onetrig();
		return 0;
	} else if(0 == strncmp(argx, "change_div", 10)){
		return 1;
	} else if(0 == strncmp(argx, "change_cntdir", 13)){
		return 2;
	} else if(0 == strncmp(argx, "change_mid", 10)){
		return 3;
	} else if(0 == strncmp(argx, "change_cycle", 12)){
		return 4;
	} else if(0 == strncmp(argx, "change_pol", 10)){
		return 5;
	} else if(0 == strncmp(argx, "change_duty", 11)){
		return 6;
	} else if(0 == strncmp(argx, "change_flt", 10)){
		return 7;
	}
	return 0;
}

int8_t tim_cmdline_ch_select(char *argx)
{
	if(0 == strncmp(argx, "ch0", 3)){
		return 0;
	} else if(0 == strncmp(argx, "ch1", 3)){
		return 1;
	} else {
		return -1;
	}
}

int8_t tim_cmdline_execute(int option, int8_t chx, char *ext1, uint32_t ext2)
{
	switch (option)
	{
	case 0:
		break;
	case 1:
		TIM_hal_Counter_set_div(ext2);
		break;
	case 2:
		if(0 == strncmp(ext1, "up", 2)){
			tim_hal_disable();
			tim_hal_cntdir_set(0);
			tim_hal_enable();

		} else if(0 == strncmp(ext1, "down", 4)){
			tim_hal_disable();
			tim_hal_cntdir_set(1);
			tim_hal_enable();
		} else{
			return -1;
		}
		break;
	case 3:
		if(0 == strncmp(ext1, "edge", 4)){
			tim_hal_cnt_mid_set(0);
		} else if(0 == strncmp(ext1, "mid", 3)){
			tim_hal_cnt_mid_set(5);
		} else{
			return -1;
		}
		break;
	case 4:
		tim_hal_disable();
		tim_hal_th_set(ext2);
		tim_hal_enable();
		break;
	case 5:
		if((0 != chx) && (1 != chx)){
			INFO("select chx error!!\n");
			return -1;
		}
		if(0 == strncmp(ext1, "posity", 6)){
			tim_hal_compn_pol_set(chx, 1);
		} else if(0 == strncmp(ext1, "negative", 8)){
			tim_hal_compn_pol_set(chx, 0);
		} else {
			return -1;
		}
		break;
	case 6:
		if((0 != chx) && (1 != chx)){
			INFO("select chx error!!\n");
			return -1;
		}
		tim_hal_disable();
		tim_hal_compn_set(chx, ext2);
		tim_hal_enable();
		break;
	case 7:
		if(ext2 > 255){
			return -1;
		}
		tim_hal_flt(ext2);
		break;
	default:
		break;
	}
	return 0;
}
