#ifndef __HTOL_ACTION_H
#define __HTOL_ACTION_H

#include "platform_common.h"
#include "test.h"
#include "gpio_hal.h"
#include "clock.h"
#include "uart_hal.h"
#include "uart_hal_cfg.h"
#include "i2c_hal.h"
#include "TIM_hal.h"
#include "cmdline_internal.h"

#include <stdio.h>
#include <stdarg.h>
#include <string.h>


/* TIM 捕获数据结构：在 isr.c 中由中断更新，主循环读取打印。
   各测量时刻：TIM13/14/15/16 为同一时钟源驱动，采样点对齐。 */
typedef struct
{
    volatile uint32_t freq;    /* 捕获频率 Hz（采集窗口结束后由主流程 Tim_cap_finalize 计算） */
    volatile uint32_t period;  /* 周期（用于计算） */
    volatile uint32_t duty;    /* 占空比 %（采集窗口结束后由主流程 Tim_cap_finalize 计算） */
    volatile uint32_t on_t;    /* 高电平段时间（计数）：ISR 在每个周期结算沿更新，供主流程算 duty */
    volatile uint8_t  ready;   /* 1 = 中断已更新本轮数据，主循环可读打印 */
    /* 以下为 ISR 内部状态，主循环不需要修改 */
    volatile uint32_t ovf;     /* 捕获溢出计数（ZERO 中断累计，模拟 TH+1 计长周期） */
    uint32_t prev_abs;         /* 上一次捕获的绝对计数值 */
    uint32_t half_t;           /* 半个周期（用于计算） */
    uint8_t  edge_n;           /* 0=取前一次捕获值 1=取本次捕获值 */
    uint8_t  got;              /* 是否已捕获到一个基准沿 */
    /* 捕获输入引脚（中断判沿极性用）：每个周期结算沿采样该脚电平，判断本沿是
       上升还是下降，从而确定高电平段是哪一半。TIM16 输入随流程切换（互测 PC1 /
       STM32 流程 PB9），由 htol_tim16_ch0_mux_*() 同步更新 */
    PORT_TypeDef *pin_port;
    uint8_t       pin_bit;
} tim_cap_t;

extern tim_cap_t g_tim13_cap;
extern tim_cap_t g_tim14_cap;
extern tim_cap_t g_tim15_cap;
extern tim_cap_t g_tim16_cap;



void Htol_test_runner(void);
void htol_clock_enable(void);
void htol_init(void);
static void Htol_all_gpio_init(void);
void uart1_printf(const char *fmt, ...);
uint8_t uart1_test(void);
static void Htol_tim_init(void);
uint8_t TIM_test(void);
uint8_t STM32_check_test(void);
void htol_run_all_flows(void);

#endif
