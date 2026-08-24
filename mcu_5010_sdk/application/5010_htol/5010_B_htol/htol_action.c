#include "htol_action.h"
#include "htol_action_cfg.h"
#include "ADC_Reg.h"
#include "ADC_Type.h"
#include "adc.h"


/* 根据 IO36(PC4) ADC 电压确定 I2C 从机地址（实现在文件下方，htol_init 中先调用） */
void i2c_slave_addr_get(void);


/*===========================================================================*/
/* HTOL 多流程测试框架：                                                       */
/*   1. 每个 A<->B 测试流程写成 uint8_t xxx_test(void) 函数（本侧实现，内部负责 */
/*      与对端同步延时后执行并返回结果），返回 1=通过 0=失败                    */
/*   2. 在 htol_test_table 中登记 {name, cmd, ack, pfn, via_i2c}：              */
/*      cmd=B 通过 UART1 下发的触发指令，ack=A 应答 B 的字符串；via_i2c=1 的     */
/*      流程（如 STM32 单独校验 B）指令/应答改经 I2C1 与 STM32 交互             */
/*   3. B 侧 htol_run_all_flows() 逐个下发注册表中的指令并执行本侧流程；         */
/*      A 侧 htol_wait_cmd_and_run() 循环等待 B 的指令并分发到对应流程           */
/*   4. UART1 互测是循环里的第一个流程：B 发 "uart1_test" -> A 应答              */
/*      "uart1_over"。一次完整往返同时覆盖 B->A 与 A->B 两条链路，收到应答       */
/*      即 UART1 链路正常；握手失败（A 未应答）时框架会上报该流程 FAIL          */
/*===========================================================================*/


static const htol_test_entry_t htol_test_table[] =
{
    /*   name,     cmd,           ack,           pfn             via_i2c */
    {  "UART1",  "uart1_test",  "uart1_over",  uart1_test,       0},   /* UART1 互测：A<->B 链路双向校验（循环第一个流程） */
    {  "TIMER",  "tim_test",    "tim_over",    TIM_test,         0},   /* TIMER PWM 双向互测（A<->B，UART1 握手） */
    {  "STM32",  "stm32_check", "stm32_ack",   STM32_check_test, 1},   /* STM32 单独校验 B（B<->STM32，I2C 握手） */
    {NULL, NULL, NULL, NULL}                                            /* 表结束标志，不要删 */
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
    i2c_clk_enable();               /* I2C1 从机时钟 */
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
    INFO("Uart0_init_successful\n");
    delay_1us(50000);
    static const uint8_t uart1_preamble[] = {0xAAU, 0xAAU, 0xAAU};
    (void)uart_send_data_polling(UART_CONSOLE_INSTANCE1, uart1_preamble, sizeof(uart1_preamble));
    uart1_printf("Uart1_init_successful\n");
	delay_1us(100000);
    __enable_irq();                         /* 使能全局中断（TIM14/15/16 的 CH0/CH1 中断才会触发） */

    /* I2C 从机地址由电压档位决定：先读 IO36(PC4) ADC 电压并设置从机地址，再初始化 I2C1 从机 */
    i2c_slave_addr_get();
    (void)i2c_slave_init(I2C_INSTANCE1, &i2c_slave_config, &i2c_slave_msg);

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

    /* TIMER14-CH0(PB13) TIMER15-CH0(PB15) TIMER16-CH0(PC1)CAPTURE（互测）*/
    /* TIMER13-CH0(PC3) - STM32 CAP。
       TIMER16_CH0 的另一个复用脚 PB9(IO25, STM32 CH1N 输入)不在初始化里配置，
       由 htol_tim16_ch0_mux_stm32() 在 STM32_check_test 开头切换，互测用回 PC1，
       保证同一时刻只有 PC1 或 PB9 挂在 TIM16_CH0 捕获输入上（同通道多引脚同时
       AF10 会"线与"进捕获输入，详见 mux 函数注释） */
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
    setting_cfg.gpio_port = GPIO_B;
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

    /* I2C1 从机：IO11(PA11)=I2C1_SDA, IO12(PA12)=I2C1_SCL，AFS4（开漏双向，外设功能接管） */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_11;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_4;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
    setting_cfg.gpio_pin = GPIO_PIN_12;
    gpio_init(&setting_cfg);

    /*-----------------------------------Analog Function Pings---------------------------------------*/
    /* 电压采样地址识别脚：IO36(PA4) = ADC_IN20，I2C 从机地址档位识别输入 */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_4;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
}

/*===========================================================================*/
/* TIMER16_CH0 输入引脚切换：                                                */
/*   TIMER16_CH0 在 GPIO 复用表单里有多个可选脚，本板用到两个互斥脚：         */
/*     - PC1 (IO33)：A/B 互测流程，捕获 A 的 TIM16_CH0 PWM                     */
/*     - PB9 (IO25)：STM32 流程，捕获 STM32 的 CH1N 互补 PWM                  */
/*   GPIO 的 AF 配置按引脚独立（AF0_STR/AF1_STR 每引脚 4bit 字段），没有通道   */
/*   "输入源选择"的仲裁位；同一通道的两个引脚同时 AF10 会把两路信号直接"线与"  */
/*   进捕获输入，捕获到的是叠加后的畸变波形（实测互测时 T16 测出 3577/939Hz  */
/*   等乱值，且互测/STM32 两个流程读数相同，正是同一输入被两脚共用的特征）。   */
/*   因此每个流程开始前必须只挂一个引脚，另一个还原为普通 GPIO（AF0）。        */
/*===========================================================================*/

/* 互测流程：PC1 作 TIM16_CH0 捕获输入，PB9 还原为普通 GPIO（下拉防悬空） */
static void htol_tim16_ch0_mux_ab(void)
{
    gpio_settings_config_t c;

    memset(&c, 0, sizeof(c));
    c.gpio_alt = GPIO_ALT_FUNC_10;
    c.gpio_dir = GPIO_DIR_INPUT;
    c.pull_cfg = GPIO_PULL_DISABLE;
    c.gpio_strength = GPIO_STRENGTH_MODE_1;
    c.gpio_interrupt_cfg_info.int_handle = NULL;

    c.gpio_port = GPIO_C;
    c.gpio_pin  = GPIO_PIN_1;            /* PC1 = TIM16_CH0（互测） */
    gpio_init(&c);

    g_tim16_cap.pin_port = GPIO_PORTC;   /* 同步：TIM16_CH0 捕获输入 = PC1 */
    g_tim16_cap.pin_bit  = GPIO_PIN_1;

    c.gpio_alt   = GPIO_ALT_FUNC_0;      /* PB9 还原为普通 GPIO 输入 */
    c.pull_cfg   = GPIO_PULL_DOWN;
    c.gpio_port  = GPIO_B;
    c.gpio_pin   = GPIO_PIN_9;
    gpio_init(&c);
}

/* STM32 流程：PB9 作 TIM16_CH0 捕获输入，PC1 还原为普通 GPIO（下拉防悬空） */
static void htol_tim16_ch0_mux_stm32(void)
{
    gpio_settings_config_t c;

    memset(&c, 0, sizeof(c));
    c.gpio_alt = GPIO_ALT_FUNC_10;
    c.gpio_dir = GPIO_DIR_INPUT;
    c.pull_cfg = GPIO_PULL_DISABLE;
    c.gpio_strength = GPIO_STRENGTH_MODE_1;
    c.gpio_interrupt_cfg_info.int_handle = NULL;

    c.gpio_port = GPIO_B;
    c.gpio_pin  = GPIO_PIN_9;            /* PB9 = TIM16_CH0（STM32 CH1N） */
    gpio_init(&c);

    g_tim16_cap.pin_port = GPIO_PORTB;   /* 同步：TIM16_CH0 捕获输入 = PB9 */
    g_tim16_cap.pin_bit  = GPIO_PIN_9;

    c.gpio_alt   = GPIO_ALT_FUNC_0;      /* PC1 还原为普通 GPIO 输入 */
    c.pull_cfg   = GPIO_PULL_DOWN;
    c.gpio_port  = GPIO_C;
    c.gpio_pin   = GPIO_PIN_1;
    gpio_init(&c);
}

void i2c_slave_addr_get(void)
{
    uint32_t adc_val;
    uint32_t mv;
    uint8_t  band;
    uint8_t  addr;

    /* 1. ADC 初始化：软件触发、单通道采样 IO36(PA4)=ADC_CH20 */
    ADC_ENABLE(ENABLE);
    ADC_SetSampleGrpMode(SampleGrpMode_1);
    ADC_SetGrpChCount(ADC_Sample_Group_0, 1);
    ADC_SetGrpTrigSource(ADC_Sample_Group_0, ADC_GrpTrig_SW_TRIG);
    ADC_SEQx_CFG(ADC_SEQ0, HTOL_ADC_CH);
    ADC_SetCLK_STPT(4);
    ADC_SetCLK_COVT(2);
    ADC_SetCLK_INIT(0);
    ADC_SetCLK_SAPT(13);
    ADC_SetCLK_TrigDelay(2);

    /* 2. 软件触发并等待转换完成 */
    ADC_SWTrig();
    while (ADC->IR.S0CDIF == 0U)
    {
    }
    ADC->IR.S0CDIF = 1U;                  /* 清转换完成标志 */
    adc_val = ADC_GetResult(ADC_SEQ0);

    /* 3. 换算为毫伏：counts * Vref / 4096 */
    mv = adc_val * HTOL_ADC_VREF_MV / 4096U;

    /* 4. 依据档位边界表确定档位，从机地址 = 0x50 + 档位号(0~11) */
    band = 0U;
    while (band < 11U && mv >= htol_band_thr_mv[band])
    {
        band++;
    }
    addr = (uint8_t)(HTOL_ADC_BASE_ADDR + band);

    i2c_slave_config.slave_addr0 = addr;

    uart1_printf("[HTOL] ADC=%u(%umV) -> I2C slave addr 0x%02X (band %u)\n",
                 (unsigned)adc_val, (unsigned)mv, (unsigned)addr, (unsigned)band);
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

/* 当前流程的结果字符串：由各测试函数填写，框架在测完后统一经 I2C1 上传 STM32 */
static char g_htol_result[64];

/**
 *@brief UART1 互测（B 侧，作为循环流程表的第一项）：
 *       框架已完成握手：B 下发 "uart1_test" -> A 应答 "uart1_over"。
 *       一次完整往返同时覆盖 B->A 与 A->B 两条链路，收到应答即 UART1 链路正常。
 *       此处只需填写结果（PASS），由框架统一延时后经 I2C1 上报 STM32；
 *       握手失败（A 未应答）时框架会填写 "<name> FAIL (no ack)" 上报，不走本函数。
 *@param[in] none
 *@return    1 正常执行完一轮
 */
uint8_t uart1_test(void)
{
    (void)snprintf(g_htol_result, sizeof(g_htol_result), "UART1 PASS");
    return 1U;
}



/* tim code */

/**
 *@brief TIM13/14/15/16 混合初始化：CH0 输入捕获 PWM、CH1 输出 PWM，并开启 CH0/ZERO 中断
 *@param[in] none
 *@return    none
 */
static void Htol_tim_init(void)
{
    /* 先切换 IRQ_TIMx，TIM_hal_init 中的 NVIC 配置会使用当前 IRQ_TIMx */
    (void)tim_hal_change(0);              /* TIMER13 */
    TIM_hal_init(TIMER13, &g_tim13_cfg);

    (void)tim_hal_change(1);              /* TIMER14 */
    TIM_hal_init(TIMER14, &g_tim14_cfg);

    (void)tim_hal_change(2);              /* TIMER15 */
    TIM_hal_init(TIMER15, &g_tim15_cfg);

    (void)tim_hal_change(3);              /* TIMER16 */
    TIM_hal_init(TIMER16, &g_tim16_cfg);

    /* 记录各定时器 CH0 捕获输入脚（中断里判沿极性用）。TIM16 会随流程切换
       （互测 PC1 / STM32 PB9），由两个 mux 函数在流程开始时同步更新 */
    g_tim13_cap.pin_port = GPIO_PORTC; g_tim13_cap.pin_bit = GPIO_PIN_3;
    g_tim14_cap.pin_port = GPIO_PORTB; g_tim14_cap.pin_bit = GPIO_PIN_13;
    g_tim15_cap.pin_port = GPIO_PORTB; g_tim15_cap.pin_bit = GPIO_PIN_15;
    g_tim16_cap.pin_port = GPIO_PORTC; g_tim16_cap.pin_bit = GPIO_PIN_1;

}

/* 清空一次测量的中断内部状态，让新的一轮测量从基准沿重新开始。
   测量结果 freq/duty/period 也一并清零：若本轮窗口内没采到边沿（比如信号未接上
   或时机不对），上报的就是 0 而不是上一轮残留的旧值，避免"假 PASS"。 */
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
   ISR 里，避免拉长中断导致 10kHz 双通道下丢捕获边沿。 */
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
/* 多流程指令驱动框架（B 侧）：                                              */
/*   B 通过 UART1 下发指令 -> 等待 A 应答 ack -> 执行对应流程               */
/*===========================================================================*/

static uint8_t htol_rx_buf[HTOL_CMD_MAX_LEN];   /* 应答匹配滚动窗口 */

/* 结果序号：每上传一条结果递增。STM32 端按序号去重——同一序号重复读到也只转发一次，
   从而避免结果在 I2C 发送缓冲中停留期间被 STM32 轮询多次读到而重复打印。
   上传格式 "R <序号> <结果>"：'R'(Result) 是类型前缀，STM32 以首字节 'R' 识别为
   测试数据；指令 "stm32_check" 以 's' 开头，STM32 据此区分指令与数据 */
static uint8_t g_htol_result_seq = 0U;

/* htol_pwm_chk 定义在本文件下方，此处先声明供 TIM_test/STM32_check_test 使用 */
static uint8_t htol_pwm_chk(uint32_t freq, uint32_t duty,
                            uint32_t exp_freq, uint32_t exp_duty,
                            uint32_t f_tol_pct, uint32_t d_tol_pct);

/* 逐字节更新滚动窗口，匹配 ack；匹配成功返回 1 */
static uint8_t htol_ack_matched(uint8_t ch, const char *ack)
{
    uint32_t len = (uint32_t)strlen(ack);

    memmove(&htol_rx_buf[0], &htol_rx_buf[1], sizeof(htol_rx_buf) - 1U);
    htol_rx_buf[sizeof(htol_rx_buf) - 1U] = ch;

    if (len != 0U && len <= sizeof(htol_rx_buf))
    {
        return (memcmp(&htol_rx_buf[sizeof(htol_rx_buf) - len], ack, len) == 0U) ? 1U : 0U;
    }
    return 0U;
}

/* 测试流程完成后延时 50ms，将 result 经 I2C1 从机发送缓冲上传 STM32。
   result：本流程结果字符串（如 "TIMER PASS ..." / "STM32 FAIL ..." / "UART1 PASS"），
   框架流程传 g_htol_result，独立流程（如 uart1_test）传各自的结果字符串。
   上传格式 "R <序号> <结果>"：
    - 首字节 'R' 是类型前缀，STM32 据此识别为测试数据；指令 "stm32_check" 以 's'
      开头，STM32 据此区分"流程指令"与"测试数据"。
    - 序号 g_htol_result_seq 每轮递增，STM32 按序号去重，同一序号只转发一次，
      即使结果在缓冲中停留期间被轮询读到多次也不会重复打印。
   结果写入后保留 HTOL_RESULT_HOLD_MS(≥STM32 轮询周期 400ms；B 侧延时实测偏大，
   实际保留时间远超名义值)，保证至少被读到一次。 */
static void htol_report_result_to_stm32(const char *result)
{
    delay_1us(50000U);   /* 50ms（原 100ms，缩短提速） */

    g_htol_result_seq++;   /* 每条结果带递增序号 */

    __disable_irq();
    memset(i2c_slave_send_buf, 0, sizeof(i2c_slave_send_buf));
    (void)snprintf((char *)i2c_slave_send_buf, sizeof(i2c_slave_send_buf),
                   "R %u %s", (unsigned)g_htol_result_seq, result);
    __enable_irq();

    uart1_printf("[HTOL] result to STM32: %s\n", result);

    /* 保留至少一个 STM32 轮询周期，等 STM32 读走结果后再进入下一流程。
       STM32 按序号去重，同一序号重复读到也只转发一次 */
    delay_1us(HTOL_RESULT_HOLD_MS * 1000U);   /* 结果保留时间（改用 Delay_1us） */
}

/**
 *@brief I2C 指令握手并执行流程（B 侧，用于 B<->STM32 流程）：
 *        1. 将 entry->cmd 写入 I2C 从机发送缓冲（STM32 作为主机轮询读取识别）
 *        2. 轮询 I2C 从机接收缓冲，等待 STM32 写入 entry->ack 应答（超时 10s）
 *        3. 收到应答后调用本侧流程函数执行（流程内部与 STM32 同步延时 500ms）
 *@param[in] entry 注册表项（cmd=B 上发 STM32 的指令，ack=STM32 应答，pfn=本侧流程）
 *@return    1 正常执行完一轮，0 未收到 STM32 应答（超时）
 */
static uint8_t htol_stm32_handshake_and_run(const htol_test_entry_t *entry)
{
    uint32_t timeout_cnt = 0U;
    uint32_t cmd_len = (uint32_t)strlen(entry->cmd);
    uint32_t ack_len = (uint32_t)strlen(entry->ack);

    /* 1. 清空 I2C 接收缓冲并写入本流程指令到发送缓冲。
          临界区保护：I2C 从机中断（主机读 RD_REQ / 主机写 rec_buf）在此期间被屏蔽，
          主机在读/写事务中遇到临界区会时钟拉伸等待，缓冲区更新完成后才被服务，
          从而保证主机读到的始终是完整的新内容。窗口仅 memset+memcpy，微秒级。 */
    __disable_irq();
    memset(i2c_slave_rec_buf, 0, sizeof(i2c_slave_rec_buf));
    memset(i2c_slave_send_buf, 0, sizeof(i2c_slave_send_buf));
    memcpy(i2c_slave_send_buf, entry->cmd, cmd_len);
    __enable_irq();

    /* 2. 轮询 I2C 接收缓冲，等待 STM32 的应答（10s 超时）。
           STM32 应答约定为单次 I2C 写事务写入 rec_buf，做滚动窗口匹配 */
    while (timeout_cnt < 10000U)   /* 10000 x 1ms = 10s */
    {
        uint32_t i;

        for (i = 0U; (i + ack_len) <= sizeof(i2c_slave_rec_buf); i++)
        {
            if (memcmp(&i2c_slave_rec_buf[i], entry->ack, ack_len) == 0U)
            {
                uart1_printf("[HTOL] %s flow started\n", entry->name);
                (void)entry->pfn();                  /* 流程内部：与 STM32 同步延时 500ms -> 执行 */
                htol_report_result_to_stm32(g_htol_result);       /* 测完 50ms 后经 I2C1 上传结果给 STM32 */
                return 1U;
            }
        }
        delay_1us(1000U);   /* 1ms（改用 Delay_1us，微秒单位） */
        timeout_cnt++;
    }

    uart1_printf("[HTOL] %s: no ack from STM32 (timeout)\n", entry->name);
    gpio_output_set(GPIO_A, GPIO_PIN_14, GPIO_LOW_LEVEL);   /* 未执行成功，IO14 LED 熄灭 */
    return 0U;
}

/**
 *@brief 下发一个流程的触发指令并等待应答，收到应答后调用本侧流程函数执行
 *        （流程内部与对端同步延时 500ms）。
 *        via_i2c=1 的流程（如 STM32 单独校验 B）走 I2C 握手与 STM32 交互；
 *        其余流程经 UART1 与 A 交互（最多 5 次、每次 500ms）
 *@param[in] entry 注册表项（cmd=触发指令，ack=对端应答，pfn=本侧流程）
 *@return    1 正常执行完一轮，0 未收到对端应答（超时）
 */
static uint8_t htol_send_cmd_and_run(const htol_test_entry_t *entry)
{
    uint32_t attempt;
    uint32_t timeout_cnt;
    uint8_t ack_ok = 0U;

    /* via_i2c 流程：指令/应答经 I2C1 与 STM32 交互，不走 UART1 */
    if (entry->via_i2c)
    {
        return htol_stm32_handshake_and_run(entry);
    }

    /* 非 I2C 流程开始前清空 I2C 发送缓冲：避免 STM32 轮询转发上一轮残留的结果字符串 */
    __disable_irq();
    memset(i2c_slave_send_buf, 0, sizeof(i2c_slave_send_buf));
    __enable_irq();

    memset(htol_rx_buf, 0, sizeof(htol_rx_buf));   /* 清空上一轮的滚动窗口 */

    /* 循环下发指令并等待应答，收到应答即停止重发 */
    for (attempt = 0U; attempt < 5U; attempt++)
    {
        (void)uart_send_data_polling(UART_CONSOLE_INSTANCE1,
                                     (const uint8_t *)entry->cmd,
                                     (uint32_t)strlen(entry->cmd));
        timeout_cnt = 0U;
        while (timeout_cnt < 500U)
        {
            uint8_t ch;
            if (uart_receive_data_polling_for_cmdline(UART_CONSOLE_INSTANCE1, &ch, 1U))
            {
                if (htol_ack_matched(ch, entry->ack))
                {
                    ack_ok = 1U;
                    break;
                }
            }
            delay_1us(1000U);   /* 1ms（改用 Delay_1us，微秒单位） */
            timeout_cnt++;
        }
        if (ack_ok)
        {
            break;
        }
    }

    if (!ack_ok)
    {
        uart1_printf("[HTOL] %s: no ack from A (timeout)\n", entry->name);
        /* 握手失败也属于流程结果：上报 "<name> FAIL (no ack)" 给 STM32。
           这样 A 未应答 / UART1 链路断开时，上位机也能看到具体哪个流程失败 */
        (void)snprintf(g_htol_result, sizeof(g_htol_result), "%s FAIL (no ack)", entry->name);
        htol_report_result_to_stm32(g_htol_result);
        return 0U;   /* 5 次都未收到 A 的应答，本流程未执行 */
    }

    uart1_printf("[HTOL] %s flow started\n", entry->name);
    (void)entry->pfn();                   /* 流程内部：与 A 同步延时 500ms -> 执行 */
    htol_report_result_to_stm32(g_htol_result);        /* 测完 50ms 后经 I2C1 上传结果给 STM32 */
    return 1U;
}

/**
 *@brief HTOL 测试流程驱动：顺序下发注册表内所有流程的指令并执行本侧对应流程
 *@param[in] none
 *@return    none
 */
void htol_run_all_flows(void)
{
    const htol_test_entry_t *entry = htol_test_table;

    while (entry->pfn != NULL)
    {
        (void)htol_send_cmd_and_run(entry);
        entry++;
    }
}

/**
 *@brief TIMER PWM 双向互测（B 侧）：
 *        1. 关闭 TIMER14/15/16
 *        2. 复位计数器、清测量状态，与 A 延时 200ms
 *        3. 开启 TIMER（CH0 捕获 A 的 PWM，CH1 输出 PWM 给 A）
 *        4. 等 PWM 稳定 500ms，稳定期数据不计入判定
 *        5. 稳定期后清测量状态，采集 1s，打印各通道测量结果
 *@param[in] none
 *@return    1 正常执行完一轮
 */
uint8_t TIM_test(void)
{
    uint8_t pass = 0U;

    /* 0. TIM16_CH0 切回互测引脚 PC1（PB9 还原为 GPIO）：避免上一轮 STM32 流程
           留下的 PB9 复用与本流程的 PC1 同时占用捕获输入 */
    htol_tim16_ch0_mux_ab();

    /* 1. 测试开始前关闭 TIMER，避免混入前一轮的无关波形 */
    tim_hal_disable(TIMER14);
    tim_hal_disable(TIMER15);
    tim_hal_disable(TIMER16);

    /* 2. 复位计数器、清测量状态，与 A 同步延时 200ms */
    tim_hal_cnt_set(TIMER14, 0);
    tim_hal_cnt_set(TIMER15, 0);
    tim_hal_cnt_set(TIMER16, 0);
    Tim_cap_reset(&g_tim14_cap);
    Tim_cap_reset(&g_tim15_cap);
    Tim_cap_reset(&g_tim16_cap);
    delay_1us(200000U);   /* 同步延时 200ms */

    /* 3. 开启 TIMER：CH0 捕获 A 的 PWM、CH1 输出 PWM 给 A（PWM 从这里开始输出） */
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

    /* 6. 持续采集 1s（1kHz 下 1000 个边沿足够测准），判定三路捕获结果 */
    delay_1us(1000000U);   /* 采集窗口 1s */

    /* 窗口结束后统一换算 freq/duty（ISR 里只存原始 period/on_t，避免除法拖长中断） */
    Tim_cap_finalize(&g_tim14_cap);
    Tim_cap_finalize(&g_tim15_cap);
    Tim_cap_finalize(&g_tim16_cap);

    pass = 1U;
    if (!htol_pwm_chk(g_tim14_cap.freq, g_tim14_cap.duty,
                      TIMER_EXP_FREQ_HZ, TIMER_EXP_DUTY_PCT,
                      TIMER_FREQ_TOL_PCT, TIMER_DUTY_TOL_PCT)) { pass = 0U; }
    if (!htol_pwm_chk(g_tim15_cap.freq, g_tim15_cap.duty,
                      TIMER_EXP_FREQ_HZ, TIMER_EXP_DUTY_PCT,
                      TIMER_FREQ_TOL_PCT, TIMER_DUTY_TOL_PCT)) { pass = 0U; }
    if (!htol_pwm_chk(g_tim16_cap.freq, g_tim16_cap.duty,
                      TIMER_EXP_FREQ_HZ, TIMER_EXP_DUTY_PCT,
                      TIMER_FREQ_TOL_PCT, TIMER_DUTY_TOL_PCT)) { pass = 0U; }

    uart1_printf("[HTOL] TIM capture: %s  T14 %luHz/%lu%%  T15 %luHz/%lu%%  T16 %luHz/%lu%%\n",
                 pass ? "PASS" : "FAIL",
                 (unsigned long)g_tim14_cap.freq, (unsigned long)g_tim14_cap.duty,
                 (unsigned long)g_tim15_cap.freq, (unsigned long)g_tim15_cap.duty,
                 (unsigned long)g_tim16_cap.freq, (unsigned long)g_tim16_cap.duty);

    /* 5. 填写结果字符串：流程名 + PASS/FAIL + 三路频率/占空比（框架统一上传 STM32） */
    (void)snprintf(g_htol_result, sizeof(g_htol_result),
                   "TIMER %s %lu %lu %lu %lu %lu %lu",
                   pass ? "PASS" : "FAIL",
                   (unsigned long)g_tim14_cap.freq, (unsigned long)g_tim14_cap.duty,
                   (unsigned long)g_tim15_cap.freq, (unsigned long)g_tim15_cap.duty,
                   (unsigned long)g_tim16_cap.freq, (unsigned long)g_tim16_cap.duty);

    return pass;
}

/* 校验捕获到的 PWM 频率/占空比是否在期望值与允许偏差范围内。
   期望参数按流程传入：TIMER 流程用 TIMER_EXP_*（1kHz/50%），
   STM32 流程用 STM32_CHECK_EXP_*（10kHz/CH1=20%，CH1N 反相为 80%） */
static uint8_t htol_pwm_chk(uint32_t freq, uint32_t duty,
                            uint32_t exp_freq, uint32_t exp_duty,
                            uint32_t f_tol_pct, uint32_t d_tol_pct)
{
    uint32_t f_lo = exp_freq * (100U - f_tol_pct) / 100U;
    uint32_t f_hi = exp_freq * (100U + f_tol_pct) / 100U;
    uint32_t d_lo = 0U;
    uint32_t d_hi = 100U;

    if (freq < f_lo || freq > f_hi)
    {
        return 0U;
    }

    if (d_tol_pct < exp_duty)
    {
        d_lo = exp_duty - d_tol_pct;
        d_hi = exp_duty + d_tol_pct;
    }

    if (duty < d_lo || duty > d_hi)
    {
        return 0U;
    }

    return 1U;
}

uint8_t num = 0;
/**
 *@brief STM32 单独校验 OCD5010_B（B 侧）：
 *        1. 关闭 TIMER13/16、复位计数器、清测量状态（避免混入 A/B 互测残留波形）
 *        2. 开启 TIMER13/16 CH0 捕获通道（STM32 下发的互补 PWM 输入 IO35/IO25）
 *        3. 同步延时 1000ms（STM32 应答后延时 500ms 再输出 3000ms 互补 PWM；
 *           同步延时 = STM32 的 500ms 延迟 + 500ms 稳定期，采集始于稳定后）
 *        4. 稳定期后清测量状态，持续捕获 1s，校验 TIM13/TIM16 的频率与占空比
 *        5. 填写结果字符串（框架统一延时 50ms 后经 I2C1 上传 STM32），IO14 LED 亮=通过
 *@param[in] none
 *@return    1=校验通过，0=校验失败
 */
uint8_t STM32_check_test(void)
{
    uint8_t pass;

    /* 0. TIM16_CH0 切到 STM32 引脚 PB9（PC1 还原为 GPIO）：避免互测流程留下的
           PC1 复用与 PB9 同时占用 TIM16_CH0 捕获输入。互补 PWM 由 STM32 从
           IO25(PB9=CH1N 80%) / IO35(PC3=CH1 20%) 下发 */
    htol_tim16_ch0_mux_stm32();

    /* 1. 关闭 TIMER13/14/15/16。T14/T15 是上一轮 TIMER 互测留下的（仍挂着 A 的
           1kHz PWM 在触发捕获/ZERO 中断），若不关会白白增加本流程的 ISR 负载 */
    tim_hal_disable(TIMER13);
    tim_hal_disable(TIMER14);
    tim_hal_disable(TIMER15);
    tim_hal_disable(TIMER16);
    tim_hal_cnt_set(TIMER13, 0);
    tim_hal_cnt_set(TIMER16, 0);
    Tim_cap_reset(&g_tim13_cap);
    Tim_cap_reset(&g_tim16_cap);

    /* 2. 开启捕获通道（先开通道后同步延时，STM32 一开始输出即可采到边沿） */
    tim_hal_enable(TIMER13);
    tim_hal_enable(TIMER16);

    /* 3. 与 STM32 同步延时。B 侧 delay_1us 在 FPGA 上实测慢 ~6.4 倍
           （NOP 忙等每次迭代实际 ~6.4 周期而非 1 周期，见 delay 库注释），
           名义延时 = 实际 ×6.4，故延时参数按 1/6.4 缩放：
             sync   1.2s 实际 = delay_1us(1200000/6.4) = delay_1us(188000)
           STM32 时序：应答后 HAL_Delay(500ms) 才开始输出 3s 互补 PWM，
           本同步延时覆盖 STM32 的 500ms + 500ms PWM 稳定期，
           采集窗口落在 [应答+1.2s, +2.2s]，压在 PWM 有效期内 */
    delay_1us(188000U);  /* 同步延时 ~1.2s（实际） */

    /* 4. 稳定期结束后清测量状态，重新从基准沿开始统计 ~1s。
           T13(PC3) 采 CH1=20%、T16(PB9) 采 CH1N=80%。占空比极性由 ISR 在每个
           周期结算沿采样输入脚电平判断（沿后为低 → 高电平段=delta），偶发漏沿
           造成的极性错位下一周期即自愈，无需在此采样复位瞬间电平 */
    __disable_irq();
    Tim_cap_reset(&g_tim13_cap);
    Tim_cap_reset(&g_tim16_cap);
    __enable_irq();
    /*
    while(1){
        if(g_tim16_cap.ready == 1){
            num++;
            if(num == 100){
                uart1_printf("period:%d freq:%d duty:%d%%\n", g_tim16_cap.period, g_tim16_cap.freq, g_tim16_cap.duty);
            }
            g_tim16_cap.ready = 0;
        }
    }
    */
    /* 5. 持续捕获 ~1s 实际（10kHz 下上万个边沿，测量精度足够），读取两路测量结果 */
    delay_1us(156000U);  /* 采集窗口 ~1s（实际，1000000/6.4≈156250） */

    /* 窗口结束后统一换算 freq/duty（ISR 里只存原始 period/on_t，避免除法拖长中断） */
    Tim_cap_finalize(&g_tim13_cap);
    Tim_cap_finalize(&g_tim16_cap);

    /* 5. 校验参数：T13(CH1, PA8->IO35) 期望 10kHz/20%，
          T16(CH1N, PA7->IO25) 互补反相，期望 10kHz/(100%-20%=80%) */
    pass = 1U;
    if (!htol_pwm_chk(g_tim13_cap.freq, g_tim13_cap.duty,
                      STM32_CHECK_EXP_FREQ_HZ, STM32_CHECK_EXP_DUTY_PCT,
                      STM32_CHECK_FREQ_TOL_PCT, STM32_CHECK_DUTY_TOL_PCT))
    {
        pass = 0U;
    }
    if (!htol_pwm_chk(g_tim16_cap.freq, g_tim16_cap.duty,
                      STM32_CHECK_EXP_FREQ_HZ, 100U - STM32_CHECK_EXP_DUTY_PCT,
                      STM32_CHECK_FREQ_TOL_PCT, STM32_CHECK_DUTY_TOL_PCT))
    {
        pass = 0U;
    }

    /* 采集与判定结束，关闭 T13/T16：PWM 已输出完（STM32 侧 3s 已到），结果上传
       期间不再有 10kHz 捕获中断，避免 I2C 结果读事务被 TIM 中断抢占（I2C 优先级
       3 < TIM 2）。T13/T16 保持使能反而会在 PB9/PC3 残留噪声下白耗中断 */
    tim_hal_disable(TIMER13);
    tim_hal_disable(TIMER16);

    if (pass)
    {
        gpio_output_set(GPIO_A, GPIO_PIN_14, GPIO_HIGH_LEVEL);   /* IO14 LED 点亮 */
    }
    else
    {
        gpio_output_set(GPIO_A, GPIO_PIN_14, GPIO_LOW_LEVEL);    /* IO14 LED 熄灭 */
    }

    /* 6. 填写结果字符串：流程名 + PASS/FAIL + TIM13/TIM16 频率/占空比
          （框架统一延时 50ms 后经 I2C1 上传 STM32） */
    (void)snprintf(g_htol_result, sizeof(g_htol_result),
                   "STM32 %s %lu %lu %lu %lu",
                   pass ? "PASS" : "FAIL",
                   (unsigned long)g_tim13_cap.freq, (unsigned long)g_tim13_cap.duty,
                   (unsigned long)g_tim16_cap.freq, (unsigned long)g_tim16_cap.duty);

    uart1_printf("[HTOL] STM32_check: %s  T13 %luHz/%lu%%  T16 %luHz/%lu%%\n",
                 pass ? "PASS" : "FAIL",
                 (unsigned long)g_tim13_cap.freq, (unsigned long)g_tim13_cap.duty,
                 (unsigned long)g_tim16_cap.freq, (unsigned long)g_tim16_cap.duty);

    return pass;
}
