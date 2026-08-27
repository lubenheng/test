#include "platform_common.h"


#include "cmdline_internal.h"
#include "TIM_cmdline.h"
#include "TIM_hal.h"
#include "TIM_test.h"
#include "gpio_hal.h"

/*各功能初始化时的默认配置，取值与旧版 HAL 初始化中的硬编码值保持一致*/
static TIMx_config_t g_tim_cnt_cfg =
{
	.tim_div       = TIMER_DIV_32,
	.tim_th        = 1000000,
	.tim_cnt_dir   = TIMERx_CNTR_UP,
	.tim_cnt_mode  = TIMERx_EDGE_MODE,
	.tim_irq_en    = TIMERx_IRQ_ZERO,
};

static TIMx_config_t g_tim_cmp_cfg =
{
	.tim_moe       = 1,
	.tim_ch0_mode  = TIMERx_MODE_PWM,
	.tim_ch0_pol   = TIMERx_IOPOL_POSITIVE,
	.tim_ch1_mode  = TIMERx_MODE_PWM,
	.tim_ch1_pol   = TIMERx_IOPOL_POSITIVE,
	.tim_div       = TIMER_DIV_32,
	.tim_th        = 1000,
	.tim_cmp0      = 500,
	.tim_cmp1      = 700,
};

static TIMx_config_t g_tim_cap_cfg =
{
	.tim_moe          = 1,
	.tim_ch0_mode     = TIMERx_MODE_CAP,
	.tim_ch0_rise_cap = 1,
	.tim_ch0_clr_en   = 1,
	.tim_ch0_cap_src  = TIMERx_SRC_IO0,
	.tim_ch1_mode     = TIMERx_MODE_CAP,
	.tim_ch1_fall_cap = 1,
	.tim_ch1_cap_src  = TIMERx_SRC_IO0,
	.tim_clk_src      = TIMERx_INNERCLK,
	.tim_div          = TIMER_DIV_32,
	.tim_cnt_dir      = TIMERx_CNTR_UP,
	.tim_cnt_mode     = TIMERx_EDGE_MODE,
	.tim_th           = 1000000,
	.tim_irq_en       = (TIMERx_IRQ_CH0 | TIMERx_IRQ_CH1),
};

static TIMx_config_t g_tim_mix_cfg =
{
	.tim_moe          = 1,
	.tim_ch0_mode     = TIMERx_MODE_PWM,
	.tim_ch0_pol      = TIMERx_IOPOL_POSITIVE,
	.tim_ch1_mode     = TIMERx_MODE_CAP,
	.tim_ch1_fall_cap = 1,
	.tim_ch1_cap_src  = TIMERx_SRC_IO1,
	.tim_cmp0         = 50000,
	.tim_div          = TIMER_DIV_32,
	.tim_th           = 999999,
	.tim_irq_en       = (TIMERx_IRQ_CH1 | TIMERx_IRQ_ZERO),
};

/* TIM 引脚 GPIO 复用（与旧版 HAL 初始化中的硬编码值保持一致，按需自行调整）
 * mode: 0-计数(无需GPIO)  1-输出比较  2-输入捕获  3-混合 */
static void tim_cmdline_gpio_init(uint8_t mode)
{
	gpio_settings_config_t gpio_cfg;

	gpio_cfg.gpio_port = GPIO_C;
	gpio_cfg.pull_cfg = GPIO_PULL_DISABLE;
	gpio_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	gpio_cfg.gpio_interrupt_cfg_info.int_handle = NULL;

	if(0 == mode){
		return;
	} else if(1 == mode){/* cmp：CH0/CH1 输出，复用 10 */
		gpio_cfg.gpio_alt = GPIO_ALT_FUNC_10;
		gpio_cfg.gpio_dir = GPIO_DIR_OUTPUT;
		gpio_cfg.gpio_pin = GPIO_PIN_11;
		gpio_init(&gpio_cfg);
		gpio_cfg.gpio_pin = GPIO_PIN_12;
		gpio_init(&gpio_cfg);
	} else if(2 == mode){/* cap：仅 CH0 输入，复用 1 */
		gpio_cfg.gpio_alt = GPIO_ALT_FUNC_1;
		gpio_cfg.gpio_dir = GPIO_DIR_INPUT;
		gpio_cfg.gpio_pin = GPIO_PIN_11;
		gpio_init(&gpio_cfg);
	} else if(3 == mode){/* mix：CH0 输出、CH1 输入，复用 1 */
		gpio_cfg.gpio_alt = GPIO_ALT_FUNC_1;
		gpio_cfg.gpio_pin = GPIO_PIN_11;
		gpio_cfg.gpio_dir = GPIO_DIR_OUTPUT;
		gpio_init(&gpio_cfg);
		gpio_cfg.gpio_pin = GPIO_PIN_12;
		gpio_cfg.gpio_dir = GPIO_DIR_INPUT;
		gpio_init(&gpio_cfg);
	}
}

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
	TIMx_TypeDef *timex = NULL;
	if(0 != tim_cmdline_tim_change(argv[1], &timex)){
		INFO("input argv1 error!!\n");
		return -1;
	}
	tim_cmdline_execute(timex, tim_cmdline_operation(argv[2], timex), tim_cmdline_ch_select(argv[3]), argv[4], para5);


	delay_1us(10000);
	tim_hal_para_display(timex);
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

int8_t tim_cmdline_tim_change(char *argx, TIMx_TypeDef **timex)
{
	if(0 == strncmp(argx, "TIMER13", 7)){
		*timex = TIMER13;
		return tim_hal_change(0);
	}else if(0 == strncmp(argx, "TIMER14", 7)){
		*timex = TIMER14;
		return tim_hal_change(1);
	}else if(0 == strncmp(argx, "TIMER15", 7)){
		*timex = TIMER15;
		return tim_hal_change(2);
	}else if(0 == strncmp(argx, "TIMER16", 7)){
		*timex = TIMER16;
		return tim_hal_change(3);
	} else{
		return 1;
	}
}

int tim_cmdline_operation(char *argx, TIMx_TypeDef *timex)
{
	if(0 == strncmp(argx, "cnt_init", 8)){
		tim_hal_reset(timex);
		tim_hal_irq_disable(timex);
		tim_hal_disable(timex);
		tim_cmdline_gpio_init(0);
		TIM_hal_init(timex, &g_tim_cnt_cfg);
		return 0;
	} else if(0 == strncmp(argx, "cmp_init", 8)){
		tim_hal_reset(timex);
		tim_hal_irq_disable(timex);
		tim_hal_disable(timex);
		tim_cmdline_gpio_init(1);
		TIM_hal_init(timex, &g_tim_cmp_cfg);
		return 0;
	} else if(0 == strncmp(argx, "cap_init", 8)){
		tim_hal_reset(timex);
		tim_hal_irq_disable(timex);
		tim_hal_disable(timex);
		tim_cmdline_gpio_init(2);
		TIM_hal_init(timex, &g_tim_cap_cfg);
		return 0;
	} else if(0 == strncmp(argx, "mix_init", 8)){
		tim_hal_reset(timex);
		tim_hal_irq_disable(timex);
		tim_hal_disable(timex);
		tim_cmdline_gpio_init(3);
		TIM_hal_init(timex, &g_tim_mix_cfg);
		return 0;
	} else if(0 == strncmp(argx, "tim_enable", 10)){
		tim_hal_enable(timex);
		return 0;
	} else if(0 == strncmp(argx, "tim_disable", 11)){
		tim_hal_disable(timex);
		return 0;
	} else if(0 == strncmp(argx, "one_trig", 8)){
		tim_hal_onetrig(timex);
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

int8_t tim_cmdline_execute(TIMx_TypeDef *timex, int option, int8_t chx, char *ext1, uint32_t ext2)
{
	switch (option)
	{
	case 0:
		break;
	case 1:
		TIM_hal_Counter_set_div(timex, ext2);
		break;
	case 2:
		if(0 == strncmp(ext1, "up", 2)){
			tim_hal_disable(timex);
			tim_hal_cntdir_set(timex, 0);
			tim_hal_enable(timex);

		} else if(0 == strncmp(ext1, "down", 4)){
			tim_hal_disable(timex);
			tim_hal_cntdir_set(timex, 1);
			tim_hal_enable(timex);
		} else{
			return -1;
		}
		break;
	case 3:
		if(0 == strncmp(ext1, "edge", 4)){
			tim_hal_cnt_mid_set(timex, 0);
		} else if(0 == strncmp(ext1, "mid", 3)){
			tim_hal_cnt_mid_set(timex, 5);
		} else{
			return -1;
		}
		break;
	case 4:
		tim_hal_disable(timex);
		tim_hal_th_set(timex, ext2);
		tim_hal_enable(timex);
		break;
	case 5:
		if((0 != chx) && (1 != chx)){
			INFO("select chx error!!\n");
			return -1;
		}
		if(0 == strncmp(ext1, "posity", 6)){
			tim_hal_compn_pol_set(timex, chx, 1);
		} else if(0 == strncmp(ext1, "negative", 8)){
			tim_hal_compn_pol_set(timex, chx, 0);
		} else {
			return -1;
		}
		break;
	case 6:
		if((0 != chx) && (1 != chx)){
			INFO("select chx error!!\n");
			return -1;
		}
		tim_hal_disable(timex);
		tim_hal_compn_set(timex, chx, ext2);
		tim_hal_enable(timex);
		break;
	case 7:
		if(ext2 > 255){
			return -1;
		}
		tim_hal_flt(timex, ext2);
		break;
	default:
		break;
	}
	return 0;
}
