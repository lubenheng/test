#include "htol_action.h"
#include "htol_action_cfg.h"


/*===========================================================================*/
/* HTOL 多流程测试框架：                                                       */
/*   1. 每个 A<->B 测试流程写成 uint8_t xxx_test(void) 函数（本侧实现，内部负责 */
/*      与对端同步延时 200ms 后执行并返回结果），返回 1=通过 0=失败             */
/*   2. 在 htol_test_table 中登记 {name, cmd, ack, pfn}：                       */
/*      cmd=B 通过 UART1 下发的触发指令，ack=A 应答 B 的字符串                  */
/*   3. A 侧 htol_wait_cmd_and_run() 循环等待 B 的指令并分发到对应流程；        */
/*      B 侧 htol_run_all_flows() 逐个下发注册表中的指令并执行本侧流程          */
/*   4. UART1 互测是循环里的第一个流程：B 发 "uart1_test" -> A 应答              */
/*      "uart1_over"，一次完整往返同时覆盖 B->A 与 A->B 两条链路，收到指令       */
/*      即 UART1 链路正常（A 侧结果不单独上报，由 B 侧统一经 I2C1 上传 STM32）  */
/*===========================================================================*/


static const htol_test_entry_t htol_test_table[] =
{
    /*   name,     cmd,          ack,          pfn       */
    {  "UART1",  "uart1_test", "uart1_over",  uart1_test},   /* UART1 互测：B 发指令->A 应答即链路正常（循环第一个流程） */
    {  "TIMER",  "tim_test",   "tim_over",    TIM_test},     /* TIMER PWM 双向互测 */
    {NULL, NULL, NULL, NULL}                                  /* 表结束标志，不要删 */
};

/**
 *@brief HTOL 测试流程执行器：顺序执行 htol_test_table 中注册的所有测试
 *@param[in] none
 *@return    none
 */
void Htol_test_runner(void)
{
    const htol_test_entry_t *entry = htol_test_table;
    uint32_t pass_cnt = 0U;
    uint32_t fail_cnt = 0U;

    uart1_printf("[HTOL] ==== Start HTOL test ====\n");

    while (entry->pfn != NULL)
    {
        uint8_t ret;

        uart1_printf("[HTOL] %s test running ...\n", entry->name);
        ret = entry->pfn();

        if (ret == 1U)
        {
            pass_cnt++;
            uart1_printf("[HTOL] %s test PASS\n", entry->name);
        }
        else
        {
            fail_cnt++;
            uart1_printf("[HTOL] %s test FAIL\n", entry->name);
        }

        entry++;
    }

    uart1_printf("[HTOL] ==== HTOL test done: %u pass, %u fail ====\n",
                 (unsigned)pass_cnt, (unsigned)fail_cnt);
}


/* htol clock enable and init */
void htol_clock_enable(void)
{
    gpio_clk_enable();
    ADC_Clk_Enable();
    qep_clk_enable();
    hall_clk_enable();
    HWMATH_Clk_Enable();
    uart_clk_enable();
    mcpwm_clk_enable();
    TIM13_Clk_Enable();
    TIM14_Clk_Enable();
    TIM15_Clk_Enable();
    TIM16_Clk_Enable();
}

void htol_init(void)
{
    Htol_all_gpio_init();

    (void)uart_init(UART_CONSOLE_INSTANCE, &uart0_init_config);
    (void)uart_init(UART_CONSOLE_INSTANCE1, &uart1_init_config);

	uart_transmit_enable(UART_CONSOLE_INSTANCE, true);
    uart_transmit_enable(UART_CONSOLE_INSTANCE1, true);

	cmdline_init();
    INFO("Uart_init_successful\n");
    delay_1us(50000);
    static const uint8_t uart1_preamble[] = {0xAAU, 0xAAU, 0xAAU};
    (void)uart_send_data_polling(UART_CONSOLE_INSTANCE1, uart1_preamble, sizeof(uart1_preamble));
    uart1_printf("Uart_init_successful\n");

    __enable_irq();                         /* 使能全局中断（TIM14/15/16 的 CH0/CH1 中断才会触发） */

    Htol_tim_init();
}


static void Htol_all_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;

    /*-----------------------------------Digital Function Pings---------------------------------------*/
	/* UART1_RX, PB2 --- UART1_TX, PB3  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_2;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_3;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* UART0_RX, PB5 --- UART0_TX, PB6  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_5;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.gpio_pin = GPIO_PIN_6;
    gpio_init(&setting_cfg);

    /* GPIO_OUT_PP, PA14*/
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_14;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* TIMER14-CH0(PB13) TIMER15-CH0(PB15) TIMER16-CH0(PC1) TIMER13-CH0(PC3) CAPTURE。
       TIMER16_CH0 的另一个复用脚 PB9(IO25) 是 STM32 单独检 B 用的（B 侧代码），
       A 没有该流程，不得把 PB9 也配成 AF10：同一通道两个引脚同时 AF10 会把信号
       "线与"进捕获输入，实测导致 A 的 T16 捕获到畸变波形 */
    setting_cfg.gpio_port = GPIO_B;
	setting_cfg.gpio_pin = GPIO_PIN_13;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_10;
	setting_cfg.gpio_dir = GPIO_DIR_INPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);
    setting_cfg.gpio_pin = GPIO_PIN_15;
    gpio_init(&setting_cfg);
    setting_cfg.gpio_port = GPIO_C;
	setting_cfg.gpio_pin = GPIO_PIN_1;
    gpio_init(&setting_cfg);
    setting_cfg.gpio_pin = GPIO_PIN_3;
    gpio_init(&setting_cfg);

    /* TIMER14-CH1(PB14) TIMER15-CH1(PC0) TIMER16-CH1(PC2) COMPARE*/
    setting_cfg.gpio_port = GPIO_B;        /* PB14（原笔误写成 GPIO_C+GPIO_PIN_14=PC14，PC14 无 TIM14_CH1 复用，输出会丢失） */
    setting_cfg.gpio_pin = GPIO_PIN_14;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_10;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_0;
    gpio_init(&setting_cfg);
	setting_cfg.gpio_pin = GPIO_PIN_2;
    gpio_init(&setting_cfg);

    /*-----------------------------------Analog Function Pings---------------------------------------*/

}

/* uart code */
void uart1_printf(const char *fmt, ...)
{
    char tmp[128];
    va_list args;

    va_start(args, fmt);
    (void)vsnprintf(tmp, sizeof(tmp), fmt, args);
    va_end(args);

    (void)uart_send_data_polling(UART_CONSOLE_INSTANCE1, (const uint8_t *)tmp, (uint32_t)strlen(tmp));
}

/**
 *@brief UART1 互测（A 侧，作为循环流程表的第一项）：
 *       框架已完成握手：B 发 "uart1_test" -> A 应答 "uart1_over"。
 *       一次完整往返同时覆盖 B->A 与 A->B 两条链路，A 收到指令即 UART1 链路正常，
 *       无需再做额外动作（A 侧结果不单独上报，由 B 侧统一经 I2C1 上传 STM32）。
 *@param[in] none
 *@return    1 正常执行完一轮
 */
uint8_t uart1_test(void)
{
    return 1U;
}



/* tim code */

/**
 *@brief TIM14/15/16 混合初始化：CH0 输入捕获 PWM、CH1 输出 PWM，并开启 CH0/ZERO 中断
 *@param[in] none
 *@return    none
 */
static void Htol_tim_init(void)
{
    /* 先切换 IRQ_TIMx，TIM_hal_init 中的 NVIC 配置会使用当前 IRQ_TIMx */
    (void)tim_hal_change(1);              /* TIMER14 */
    TIM_hal_init(TIMER14, &g_tim14_cfg);

    (void)tim_hal_change(2);              /* TIMER15 */
    TIM_hal_init(TIMER15, &g_tim15_cfg);

    (void)tim_hal_change(3);              /* TIMER16 */
    TIM_hal_init(TIMER16, &g_tim16_cfg);

    /* 记录各定时器 CH0 捕获输入脚（中断里判沿极性用）。A 无 STM32 流程，
       TIM16 固定用 PC1（PB9 是 B 侧 STM32 流程专用脚，A 不得配 AF10 以免线与） */
    g_tim14_cap.pin_port = GPIO_PORTB; g_tim14_cap.pin_bit = GPIO_PIN_13;
    g_tim15_cap.pin_port = GPIO_PORTB; g_tim15_cap.pin_bit = GPIO_PIN_15;
    g_tim16_cap.pin_port = GPIO_PORTC; g_tim16_cap.pin_bit = GPIO_PIN_1;
}

/* 清空一次测量的中断内部状态，让新的一轮测量从基准沿重新开始。
   测量结果 freq/duty/period 也一并清零：若本轮窗口内没采到边沿（比如对端信号
   未接上），上报的是 0 而不是上一轮残留旧值，避免"假 PASS"。 */
static void Tim_cap_reset(tim_cap_t *m)
{
    m->got      = 0U;
    m->ovf      = 0U;
    m->ready    = 0U;
    m->edge_n   = 0U;
    m->freq     = 0U;
    m->duty     = 0U;
    m->period   = 0U;
    m->on_t     = 0U;
}

/* 采集窗口结束后由主流程调用：把 ISR 里存的原始 period/on_t 换算成 freq/duty。
   32 位除法（M0+ 上编成 __aeabi_uidiv 软件除法，每次上百周期）放主流程而不是
   ISR 里，避免拉长中断导致高频捕获丢沿。 */
static void Tim_cap_finalize(tim_cap_t *m)
{
    if (m->period != 0U) {
        m->freq = 32000000UL / m->period;          /* 计数时钟 32MHz（TIMER_DIV_1） */
        m->duty = (m->on_t * 100UL) / m->period;
    } else {
        m->freq = 0U;
        m->duty = 0U;
    }
}

/*===========================================================================*/
/* 多流程指令分发框架（A 侧）：                                              */
/*   B 通过 UART1 下发指令 -> A 匹配 -> 应答 ack -> 执行对应流程            */
/*===========================================================================*/

static uint8_t htol_rx_buf[HTOL_CMD_MAX_LEN];   /* 指令匹配滚动窗口 */

/* 逐字节更新滚动窗口，尝试匹配注册表内任一指令；返回匹配到的表项，无匹配返回 NULL */
static const htol_test_entry_t *htol_cmd_lookup(uint8_t ch)
{
    const htol_test_entry_t *entry = htol_test_table;

    memmove(&htol_rx_buf[0], &htol_rx_buf[1], sizeof(htol_rx_buf) - 1U);
    htol_rx_buf[sizeof(htol_rx_buf) - 1U] = ch;

    while (entry->pfn != NULL)
    {
        uint32_t len = (uint32_t)strlen(entry->cmd);
        if (len != 0U && len <= sizeof(htol_rx_buf))
        {
            if (memcmp(&htol_rx_buf[sizeof(htol_rx_buf) - len], entry->cmd, len) == 0U)
            {
                return entry;
            }
        }
        entry++;
    }
    return NULL;
}

/**
 *@brief 循环等待 B 通过 UART1 下发的测试指令（超时 10s）。
 *       匹配到指令后：应答 ack -> 打印流程开始 -> 执行对应测试流程（500ms 同步在流程内部）
 *@param[in] none
 *@return    1=执行了一个流程，0=超时未收到有效指令
 */
uint8_t htol_wait_cmd_and_run(void)
{
    uint32_t timeout_cnt = 0U;

    memset(htol_rx_buf, 0, sizeof(htol_rx_buf));   /* 清空上一轮的滚动窗口 */

    while (timeout_cnt < 10000U)   /* 10000 x 1ms = 10s */
    {
        uint8_t ch;
        if (uart_receive_data_polling_for_cmdline(UART_CONSOLE_INSTANCE1, &ch, 1U))
        {
            const htol_test_entry_t *entry = htol_cmd_lookup(ch);
            if (entry != NULL)
            {
                /* 应答 B：A 已收到指令 */
                (void)uart_send_data_polling(UART_CONSOLE_INSTANCE1,
                                             (const uint8_t *)entry->ack,
                                             (uint32_t)strlen(entry->ack));
                uart1_printf("[HTOL] %s flow started\n", entry->name);
                (void)entry->pfn();   /* 流程内部：与 B 同步延时 500ms -> 执行 -> 打印结果 */
                return 1U;
            }
        }
        delay_1us(1000U);   /* 1ms（改用 Delay_1us） */
        timeout_cnt++;
    }

    uart1_printf("[HTOL] no flow cmd from B (timeout)\n");
    return 0U;
}

/**
 *@brief TIMER PWM 双向互测（A 侧）：
 *        1. 关闭 TIMER14/15/16
 *        2. 复位计数器、清测量状态，与 B 延时 200ms
 *        3. 开启 TIMER（CH0 捕获 B 的 PWM，CH1 输出 PWM 给 B）
 *        4. 等 PWM 稳定 500ms，稳定期数据不计入判定
 *        5. 稳定期后清测量状态，采集 1s，打印各通道测量结果
 *@param[in] none
 *@return    1 正常执行完一轮
 */
uint8_t TIM_test(void)
{
    /* 1. 测试开始前关闭 TIMER，避免混入前一轮的无关波形 */
    tim_hal_disable(TIMER14);
    tim_hal_disable(TIMER15);
    tim_hal_disable(TIMER16);

    /* 2. 复位计数器、清测量状态，与 B 延时 200ms */
    tim_hal_cnt_set(TIMER14, 0);
    tim_hal_cnt_set(TIMER15, 0);
    tim_hal_cnt_set(TIMER16, 0);
    Tim_cap_reset(&g_tim14_cap);
    Tim_cap_reset(&g_tim15_cap);
    Tim_cap_reset(&g_tim16_cap);
    delay_1us(200000U);   /* 同步延时 200ms */

    /* 3. 开启 TIMER：CH0 捕获 B 的 PWM、CH1 输出 PWM 给 B（PWM 从这里开始输出） */
    tim_hal_enable(TIMER14);
    tim_hal_enable(TIMER15);
    tim_hal_enable(TIMER16);

    /* 4. 等 PWM 稳定 500ms：稳定期（含上电/相位建锁）的数据不计入判定 */
    delay_1us(500000U);   /* 稳定期 500ms */

    /* 5. 稳定期结束后清测量状态，重新从基准沿开始统计 1s。
           中断会同时读写 cap 结构，复位用临界区保护 */
    __disable_irq();
    Tim_cap_reset(&g_tim14_cap);
    Tim_cap_reset(&g_tim15_cap);
    Tim_cap_reset(&g_tim16_cap);
    __enable_irq();

    /* 6. 持续采集 1s（1kHz 下 1000 个边沿足够测准），打印各通道测量结果 */
    delay_1us(1000000U);   /* 采集窗口 1s */

    /* 窗口结束后统一换算 freq/duty（ISR 里只存原始 period/on_t，避免除法拖长中断） */
    Tim_cap_finalize(&g_tim14_cap);
    Tim_cap_finalize(&g_tim15_cap);
    Tim_cap_finalize(&g_tim16_cap);

    uart1_printf("[HTOL] TIM capture: T14 %luHz/%lu%%  T15 %luHz/%lu%%  T16 %luHz/%lu%%\n",
                 (unsigned long)g_tim14_cap.freq, (unsigned long)g_tim14_cap.duty,
                 (unsigned long)g_tim15_cap.freq, (unsigned long)g_tim15_cap.duty,
                 (unsigned long)g_tim16_cap.freq, (unsigned long)g_tim16_cap.duty);

    return 1U;
}


