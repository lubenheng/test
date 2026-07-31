#include "platform_common.h"

#include "test.h"

#include "qep_reg.h"
#include "qep.h"
#include "qep_hal.h"
#include "qep_test.h"
#include "cmdline_internal.h"

#include "gpio_type.h"
#include "gpio_hal.h"




/* 
argv[1](ZEC)     : Z 信号边沿清零 QEP 计数器使能     0->disable     1->enable
argv[2](ZLC)     : Z 信号电平清零 QEP 计数器使能     0->disable     1->enable
argv[3](ZPC)     : Z 信号清零极性选择：高电平/上升沿清零使能    0->disable     1->enable
argv[4](ZNC）    : Z 信号清零极性选择：低电平/下降沿清零使能    0->disable     1->enable
argv[5](Mode)    : 00->counting on T1   01->counting on T1 & T2   10->CCW+SIGN，符号加脉冲信号计数模式   11->CCW+CW， CCW+CW 双脉冲信号计数模式
argv[6](FE_CNT_EN) : CCW+SIGN/CCW+CW 两种模式下，是否在下降沿进行计数（上升沿总是计数）   0->disable     1->enable
argv[7](ClockDiv)  : QEP时钟分频 2^n(0->1分频，1->2分频，2->4分频)
argv[8](Filter)    : QEP_Filter (滤波系数)【若QEP_SAP = 0x6；QEPCLKDIV = 0x2; 8×6×4倍系统时钟的滤波（8位系统自带）】
argv[9](TH)	       : 计数门限 TH【编码器向上计数（增）到TH值后，再次向上计数会导致计数器回到0。编码器向下计数（减）到0值后，再次向下计数会导致计数器回到TH】
argv[10](IRQEna)   : BIT0 【下溢出中断使能，高电平有效】 BIT1 【上溢出中断使能，高电平有效】

------------------1-2-3-4-5-6-7-8---9--10/////
cmd_qep_mode_test 1 0 1 0 1 1 0 0 4096  3/////  
*/

void qep_mode_test(int argc, char *argv[])
{

    if((argc < 11U)||(argc > 13U)) //argv最大值加1
	{
		INFO("wrong cmd_format:%s\n", argv[0]);
		return;
	}
	
	for(uint16_t i = 1; i < argc; i++){INFO("argv[%d]:%s\n", i,argv[i]);}

    TIM_QEPCmd(QEP, DISABLE); 					   /* QEP0模块除能 */

 	TIM_QEPInitTypeDef TIM_QEPInitStruct;
	TIM_QEPStructInit(&TIM_QEPInitStruct);		   /* Timer结构体初始化*/
    /* QEP0_CFG */ 
    TIM_QEPInitStruct.QEP_ZEC  = get_arg_ulong(argv[1]);    /* Z 信号清零极性选择：低电平/下降沿清零使能 */ 
    TIM_QEPInitStruct.QEP_ZLC  = get_arg_ulong(argv[2]);    /* Z 信号清零极性选择：高电平/下降沿清零使能 */
    TIM_QEPInitStruct.QEP_ZPC  = get_arg_ulong(argv[3]);    /* Z 信号边沿清零 QEP 计数器使能 */
    TIM_QEPInitStruct.QEP_ZNC  = get_arg_ulong(argv[4]);    /* Z 信号电平清零 QEP 计数器使能 */
    TIM_QEPInitStruct.QEP_Mode = get_arg_ulong(argv[5]);    /* 编码器模式选择 */
    TIM_QEPInitStruct.QEP_FE_CNT_EN = get_arg_ulong(argv[6]);  //CCW+SIGN/CCW+CW 两种模式下，是否在下降沿进行计数（上升沿总是计数）
    
    TIM_QEPInitStruct.QEP_ClockDiv  = get_arg_ulong(argv[7]);  /* 编码器时钟分频 */
    TIM_QEPInitStruct.QEP_Filter    = get_arg_ulong(argv[8]);  /* 输入信号滤波系数 */
    /* QEP TH */
    TIM_QEPInitStruct.QEP_TH       = get_arg_ulong(argv[9]);  /* 编码器计数周期 */
    /* QEP IE */
    TIM_QEPInitStruct.QEP_IRQEna = get_arg_ulong(argv[10]);    /* 中断使能 */

    TIM_QEPInit(QEP, &TIM_QEPInitStruct);
	TIM_QEPCmd(QEP, ENABLE); 					   			/* QEP0模块使能 */
}


void qep_func1_test(uint32_t test_case, uint32_t para1, uint32_t para2)
{
}	


void qep_func2_test(uint32_t test_case, uint32_t para1, uint32_t para2)
{
}	

void qep_gpio_init()
{
    // gpio_settings_config_t setting_cfg;

	// /* GPIO, QEP_A, PTA15 PIN  */
    // setting_cfg.gpio_port = GPIO_A;
    // setting_cfg.gpio_pin = GPIO_PIN_15;
    // setting_cfg.gpio_alt = GPIO_ALT_FUNC_3;
    // setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    // setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    // setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    // setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    // setting_cfg.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_EITHER_EDGE;
    // setting_cfg.gpio_interrupt_cfg_info.priority = 1;
    // gpio_init(&setting_cfg);

    // /* GPIO, QEP_B, PTB0 PIN  */
    // setting_cfg.gpio_port = GPIO_B;
    // setting_cfg.gpio_pin = GPIO_PIN_0;
    // setting_cfg.gpio_alt = GPIO_ALT_FUNC_3;
    // setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    // setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    // setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    // setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    // setting_cfg.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_EITHER_EDGE;
    // setting_cfg.gpio_interrupt_cfg_info.priority = 1;
    // gpio_init(&setting_cfg);

    // /* GPIO, QEP_Z, PTB1 PIN  */
    // setting_cfg.gpio_port = GPIO_B;
    // setting_cfg.gpio_pin = GPIO_PIN_1;
    // setting_cfg.gpio_alt = GPIO_ALT_FUNC_3;
    // setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    // setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    // setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    // setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    // setting_cfg.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_EITHER_EDGE;
    // setting_cfg.gpio_interrupt_cfg_info.priority = 1;
    // gpio_init(&setting_cfg);
}

uint16_t g_qep_callback_cnt = 0;
void g_qep_callback(qep_int_source_e souce)
{
    g_qep_callback_cnt++;
}


