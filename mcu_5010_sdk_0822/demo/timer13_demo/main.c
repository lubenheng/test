/****************************************************************************/
/** \file main.c
**
**  timer13 demo for ODS_5010 (Cortex-M0+)
**  PB11-CH0,PB12-CH1
**  三种模式：COMPARE（双通道 PWM 输出，默认）/ CAPTURE（双通道输入捕获）/ COUNTER（计数）
**  修改 main.c 顶部的 tim_mode 即可切换模式。
*****************************************************************************/
#include "platform_common.h"
#include "gpio_hal.h"
#include "clock_hal.h"
#include "TIM_hal.h"

typedef enum
{
    COMPARE = 1,
    CAPTURE = 2,
    COUNTER = 3
} tim_mode_t;

/*
 * 计数器模式时，计数器计数到门限值然后从0开始就会触发过0中断，中断服务函数里面就会让PC6翻转一次
 * 输入捕获模式时，CH0 上升沿捕获 + CH1 下降沿捕获，产生捕获时就会触发中断，
   捕获后计数器清零，CH0/CH1 捕获到的值会存储在 CMP0/CMP1 寄存器里面，中断服务函数里面就会让PC6翻转一次
 * 输出比较模式时，CH0/CH1（PB11/PB12） 输出 PWM 波形，PWM 波形的占空比由 CMP0/CMP1 寄存器决定
*/

tim_mode_t tim_mode = COMPARE;
/*TIMER13 配置，取值与 SDK 中 TIM_hal/TIM_cmdline 示例保持一致*/
TIMx_config_t tim_cfg = {
    /*------------ 计数器基本配置（TIM_CFG 寄存器） ------------*/
    .tim_div         = TIMER_DIV_32,          /* 时钟分频：32MHz/32 = 1MHz 计数时钟 */
    .tim_clk_src     = TIMERx_INNERCLK,       /* 时钟源：内部系统时钟 */
    .tim_cnt_dir     = TIMERx_CNTR_UP,        /* 计数方向：向上计数 */
    .tim_cnt_mode    = TIMERx_EDGE_MODE,      /* 计数模式：边沿(非对称)计数 */

    /*------------ 通道 0 配置 ------------*/
    .tim_ch0_mode     = TIMERx_MODE_PWM,      /* CH0 模式：PWM 输出 */
    .tim_ch0_pol      = TIMERx_IOPOL_POSITIVE,/* CH0 输出极性：cnt < CMP 输出高电平 */
    .tim_ch0_cap_src  = TIMERx_SRC_IO0,       /* CH0 捕获源 */
    .tim_ch0_rise_cap = 0,                    /* CH0 上升沿捕获使能 */
    .tim_ch0_fall_cap = 0,                    /* CH0 下降沿捕获使能 */
    .tim_ch0_clr_en   = 0,                    /* CH0 捕获后计数器清零使能 */

    /*------------ 通道 1 配置 ------------*/
    .tim_ch1_mode     = TIMERx_MODE_PWM,      /* CH1 模式：PWM 输出 */
    .tim_ch1_pol      = TIMERx_IOPOL_POSITIVE,/* CH1 输出极性：cnt < CMP 输出高电平 */
    .tim_ch1_cap_src  = TIMERx_SRC_IO1,       /* CH1 捕获源 */
    .tim_ch1_rise_cap = 0,                    /* CH1 上升沿捕获使能 */
    .tim_ch1_fall_cap = 0,                    /* CH1 下降沿捕获使能 */
    .tim_ch1_clr_en   = 0,                    /* CH1 捕获后计数器清零使能 */

    /*------------ 定时数值 ------------*/
    .tim_th          = 1000,                  /* 周期：1MHz/1000 = 1kHz */
    .tim_cmp0        = 500,                   /* CH0 占空比：500/1000 = 50% */
    .tim_cmp1        = 700,                   /* CH1 占空比：700/1000 = 70% */

    /*------------ 输出/中断 ------------*/
    .tim_moe         = 1,                     /* 主输出使能 */
    .tim_irq_en      = TIMERx_IRQ_ZERO,       /* 中断使能（COUNTER/CAPTURE 模式用到，按需修改） */
};



/**
 *@brief configure GPIO_B pin 11/12 for TIM13 channel 0/1（TIM 引脚 GPIO 复用由外部自行初始化）
 *@param[in] mode: 当前演示模式（CAPTURE 配置为输入捕获复用，其余配置为输出比较复用）
 *@return    none
 */
static void tim_gpio_init(tim_mode_t mode)
{
    gpio_settings_config_t setting_cfg;

    /* PB11: CH0 */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_11;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_10;
    setting_cfg.gpio_dir = (mode == CAPTURE) ? GPIO_DIR_INPUT : GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* PB12: CH1 */
    setting_cfg.gpio_pin = GPIO_PIN_12;
    gpio_init(&setting_cfg);
}

static void gpio_demo_init(void)
{
    gpio_settings_config_t setting_cfg;

    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_6;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
}



int main(void)
{
    delay_1us(200000);

    /* Add Clock Enable Code Here */
    flash_clk_enable();
    gpio_clk_enable();
    TIM13_Clk_Enable();
	
	gpio_demo_init();

    if(tim_mode == COUNTER){
        /* 计数模式：仅配置分频/门限/计数方向等 */
        tim_cfg.tim_irq_en        = TIMERx_IRQ_ZERO;
        TIM_hal_init(TIMER13, &tim_cfg);
    } else if(tim_mode == CAPTURE){
        /* 双通道输入捕获：CH0 上升沿捕获 + CH1 下降沿捕获，捕获后计数器清零 */
        tim_cfg.tim_ch0_mode      = TIMERx_MODE_CAP;
        tim_cfg.tim_ch1_mode      = TIMERx_MODE_CAP;
        tim_cfg.tim_ch0_cap_src   = TIMERx_SRC_IO0;
        tim_cfg.tim_ch1_cap_src   = TIMERx_SRC_IO1;
        tim_cfg.tim_ch0_rise_cap  = 1;
        tim_cfg.tim_ch1_fall_cap  = 1;
        tim_cfg.tim_ch0_clr_en    = 1;
        tim_cfg.tim_ch1_clr_en    = 1;
        tim_cfg.tim_irq_en        = (TIMERx_IRQ_CH0 | TIMERx_IRQ_CH1);
        tim_gpio_init(tim_mode);
        TIM_hal_init(TIMER13, &tim_cfg);
    } else if(tim_mode == COMPARE){
        /* 双通道 PWM 输出：CH0 占空比 500/1000，CH1 占空比 700/1000 */
        tim_cfg.tim_ch0_mode      = TIMERx_MODE_PWM;
        tim_cfg.tim_ch1_mode      = TIMERx_MODE_PWM;
        tim_cfg.tim_ch0_pol       = TIMERx_IOPOL_POSITIVE;
        tim_cfg.tim_ch1_pol       = TIMERx_IOPOL_POSITIVE;
        tim_cfg.tim_irq_en        = 0;    /* 纯 PWM 输出，不使能中断（与旧版一致） */
        tim_gpio_init(tim_mode);
        TIM_hal_init(TIMER13, &tim_cfg);
    }

    while(1)
    {
		
    }
}
