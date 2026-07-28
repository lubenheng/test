#include <stdio.h>
#include <string.h>
#include "CMSDK_CM0.h"
#include "system_CMSDK_CM0.h"
#include "core_cm0.h"
#include "Common.h"
#include "uart.h"
#include "gpio.h"
#include "qspi.h"
#include "wdt.h"
#include "timer.h"
#include "mdu.h"
#include <stdarg.h>
#include "INIT.h"
#include "para_define.h"
#include "hardinit.h"
#include "main.h"
#include "gpio_hal.h"
#include "tim_hal.h"

#define TIM_MODE 	TIM1_MODE
#define TIM0_MODE 1//用于tim0、5、6
#define TIM1_MODE 2
#define TIM2_MODE 3
#define TIM4_MODE 4//用于tim3、4

// ==================== 全局变量 ====================
volatile uint32_t timer_irq_count = 0;   // 统一的中断计数（可选）

// 各定时器独立计数（保留原有风格）
volatile uint32_t sys_tick0 = 0;
volatile uint32_t tim1_tick = 0;
volatile uint32_t tim2_sys_tick = 0;
volatile uint32_t sys_tick = 0;

// ==================== TIM0 中断处理 ====================
void tim0_callback(tim_inst_e instance)
{
    if (instance == TIM_INST_0) {
        sys_tick0++;
        gpio_toggle(GPIO_PORT_P1, GPIO_PIN_7);
    }
}

void tim0_Handler(void)
{
    tim_irq_handler(TIM_INST_0);
}

void tim0_init(void) 
{
    tim_base_handle_t htim;
    htim.instance   = TIM_INST_0;
    htim.period     = 24000 - 1;              // 根据需要调整频率
    htim.enable_update_it  = true;
    htim.enable_compare_it = false;

    tim_base_init(&htim);
    tim_register_callback(TIM_INST_0, tim0_callback);
    tim_base_start_it(TIM_INST_0, TIM_IT_UPDATE);

    NVIC_EnableIRQ(tim0_irq);
    NVIC_SetPriority(tim0_irq, 3);
}

// ==================== TIM1 中断处理 ====================
void tim1_callback(tim_inst_e instance)
{
    if (instance == TIM_INST_1) {
        tim1_tick++;
        gpio_toggle(GPIO_PORT_P1, GPIO_PIN_7);
    }
}

void tim1_Handler(void)
{
    tim_irq_handler(TIM_INST_1);
}

void tim1_init(void)
{
    tim_base_handle_t htim;
    htim.instance        = TIM_INST_1;
    htim.period          = 3000;
    htim.clock_div       = TIM_CLK_DIV_8;
    htim.enable_update_it = true;
    htim.mode            = TIM_MODE_Ouput_ALIGNED;
    htim.opm             = TIM_OPM_DISABLE;
    htim.ocm             = TIM_OCM_DISABLE;
    htim.enable_compare_it = false;

    tim_base_init(&htim);
    tim_register_callback(TIM_INST_1, tim1_callback);
    tim_base_start_it(TIM_INST_1, TIM_IT_UPDATE);   // 启动定时器

    NVIC_EnableIRQ(TIM1_IRQn);
    NVIC_SetPriority(TIM1_IRQn, 1);
}

// ==================== TIM2 中断处理 ====================
void tim2_callback(tim_inst_e instance)
{
    if (instance == TIM_INST_2) {
        tim2_sys_tick++;
        gpio_toggle(GPIO_PORT_P1, GPIO_PIN_7);
    }
}

void tim2_Handler(void)
{
    tim_irq_handler(TIM_INST_2);
}

void tim2_init(void)
{
    tim_base_handle_t htim;
    htim.instance   = TIM_INST_2;
    htim.period     = 2999;
    htim.prescaler  = 0;
    htim.clock_div  = TIM_CLK_DIV_8;
    htim.mode       = TIM_MODE_Ouput_ALIGNED;
    htim.opm        = TIM_OPM_DISABLE;
    htim.ocm        = TIM_OCM_DISABLE;
    htim.enable_update_it  = true;
    htim.enable_compare_it = false;

    tim_base_init(&htim);
    tim_register_callback(TIM_INST_2, tim2_callback);
    tim_base_start_it(TIM_INST_2, TIM_IT_UPDATE);

    NVIC_EnableIRQ(TIM2_IRQn);
    NVIC_SetPriority(TIM2_IRQn, 3);
}

// ==================== TIM4 中断处理 ====================
void tim4_callback(tim_inst_e instance)
{
    if (instance == TIM_INST_4) {
        sys_tick++;
        gpio_toggle(GPIO_PORT_P1, GPIO_PIN_7);
    }
}

void tim4_Handler(void)
{
    tim_irq_handler(TIM_INST_4);
}

void tim4_init(void)   //1Khz
{
    tim_base_handle_t htim;
    htim.instance   = TIM_INST_4;
    htim.period     = 2999;
    htim.prescaler  = 0;
    htim.clock_div  = TIM_CLK_DIV_8;   //3Mhz / 3000 
    htim.mode       = TIM_MODE_Ouput_ALIGNED;
    htim.opm        = TIM_OPM_DISABLE;
    htim.ocm        = TIM_OCM_DISABLE;
    htim.enable_update_it  = true;
    htim.enable_compare_it = false;

    tim_base_init(&htim);
    tim_register_callback(TIM_INST_4, tim4_callback);
    tim_base_start_it(TIM_INST_4, TIM_IT_UPDATE);

    NVIC_EnableIRQ(tim4_irq);
    NVIC_SetPriority(tim4_irq, 3);
}

// ==================== 主函数 ====================
int main(void)
{
    soft_delay_1ms(1000);

    PMU->wr_key = 0x55AAAA55;
    PMU->cfg_normal = 0<<0 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 0<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10; // DRV3 V1.4
    static uint8_t mode = 1;
    // 初始化 GPIO P1.7 作为输出，用于观察中断翻转
    gpio_cfg_t gpio_cfg;
    gpio_cfg.port = GPIO_PORT_P1;
    gpio_cfg.pin = GPIO_PIN_7;
    gpio_cfg.dir         = GPIO_DIR_OUTPUT;
    gpio_cfg.pull        = GPIO_PULL_DISABLE;
    gpio_cfg.strength    = GPIO_STRENGTH_MID;
    gpio_cfg.input_mode  = GPIO_INPUT_MODE_CMOS;
    gpio_cfg.analog_mode = GPIO_ANALOG_DISABLE;
    gpio_cfg.alt_func    = GPIO_ALT_FUNC_0;
    gpio_cfg.int_cfg.callback = NULL;
    gpio_init(&gpio_cfg);
    gpio_output_set(GPIO_PORT_P1, GPIO_PIN_7, GPIO_HIGH_LEVEL);

		//外设PLL，48/2=24Mhz
		
    switch (mode) {
        case TIM0_MODE:
            tim0_init();   //24Mhz
            break;
        case TIM1_MODE:
            tim1_init();
            break;
        case TIM2_MODE:
            tim2_init();
            break;
        case TIM4_MODE:
            tim4_init();
            break;
        default:
            // 默认使用 TIM0
            tim0_init();
            break;
    }

    __enable_irq();

    while (1) {
        // 主循环空闲，中断中翻转 GPIO
    }
}