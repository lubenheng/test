#ifndef __HTOL_ACTION_CFG_H
#define __HTOL_ACTION_CFG_H

typedef uint8_t (*htol_test_fn_t)(void);

/* 每个 A<->B 测试流程的注册表项。
   name：测试项名称（打印用）
   cmd ：B 通过 UART1 下发的触发指令（A 收到即执行本流程）
   ack ：A 应答 B 的字符串（B 收到即认为 A 已就绪，随后双方同步延时）
   pfn ：本侧测试流程函数（框架在指令握手后调用；内部负责 500ms 同步与执行） */
typedef struct
{
    const char     *name;
    const char     *cmd;
    const char     *ack;
    htol_test_fn_t  pfn;
} htol_test_entry_t;

/* UART1 指令/应答字符串最大长度，新增流程的 cmd/ack 不得超过该值 */
#define HTOL_CMD_MAX_LEN   16U

/*===========================================================================*/
/* TIM 配置：TIM14/15/16 均为 CH0 输入捕获 PWM、CH1 输出 PWM，开启 CH0/ZERO 中断 */
/* TH/占空比可按实际需要修改；GPIO 复用已在 Htol_all_gpio_init() 中配置        */
/*===========================================================================*/

static TIMx_config_t g_tim14_cfg = {
    .tim_clk_src      = TIMERx_INNERCLK,       /* 时钟源：内部系统时钟 */
    .tim_div          = TIMER_DIV_1,          /* 分频：32MHz/1 = 32MHz 计数时钟 */
    .tim_cnt_dir      = TIMERx_CNTR_UP,        /* 计数方向：向上计数 */
    .tim_cnt_mode     = TIMERx_EDGE_MODE,      /* 计数模式：边沿(非对称)计数 */
    .tim_moe          = 1,                     /* 主输出使能（CH1 PWM 输出） */

    /* CH0 输入捕获 PWM */
    .tim_ch0_mode     = TIMERx_MODE_CAP,       /* CH0 捕获模式 */
    .tim_ch0_cap_src  = TIMERx_SRC_IO0,        /* CH0 捕获源：IO0 引脚 */
    .tim_ch0_rise_cap = 1,                     /* CH0 上升沿捕获 */
    .tim_ch0_fall_cap = 1,                     /* CH0 下降沿捕获 */
    .tim_ch0_clr_en   = 0,                     /* 捕获后不清零（避免扰动 CH1 的 PWM 相位） */

    /* CH1 输出 PWM */
    .tim_ch1_mode     = TIMERx_MODE_PWM,       /* CH1 PWM 输出模式 */
    .tim_ch1_pol      = TIMERx_IOPOL_POSITIVE, /* CH1 输出极性：cnt < CMP1 输出高电平 */

    .tim_th           = 32000,                  /* 周期：32MHz/32000 = 1kHz */
    .tim_cmp1         = 16000,                   /* CH1 占空比：16000/32000 = 50% */

    .tim_irq_en       = (TIMERx_IRQ_CH0 | TIMERx_IRQ_ZERO),  /* CH0 捕获 + ZERO 回绕计数（测低频必需） */
};

static TIMx_config_t g_tim15_cfg = {
    .tim_clk_src      = TIMERx_INNERCLK,
    .tim_div          = TIMER_DIV_1,
    .tim_cnt_dir      = TIMERx_CNTR_UP,
    .tim_cnt_mode     = TIMERx_EDGE_MODE,
    .tim_moe          = 1,

    .tim_ch0_mode     = TIMERx_MODE_CAP,
    .tim_ch0_cap_src  = TIMERx_SRC_IO0,
    .tim_ch0_rise_cap = 1,
    .tim_ch0_fall_cap = 1,
    .tim_ch0_clr_en   = 0,

    .tim_ch1_mode     = TIMERx_MODE_PWM,
    .tim_ch1_pol      = TIMERx_IOPOL_POSITIVE,

    .tim_th           = 32000,
    .tim_cmp1         = 16000,

    .tim_irq_en       = (TIMERx_IRQ_CH0 | TIMERx_IRQ_ZERO),
};

static TIMx_config_t g_tim16_cfg = {
    .tim_clk_src      = TIMERx_INNERCLK,
    .tim_div          = TIMER_DIV_1,
    .tim_cnt_dir      = TIMERx_CNTR_UP,
    .tim_cnt_mode     = TIMERx_EDGE_MODE,
    .tim_moe          = 1,

    .tim_ch0_mode     = TIMERx_MODE_CAP,
    .tim_ch0_cap_src  = TIMERx_SRC_IO0,
    .tim_ch0_rise_cap = 1,
    .tim_ch0_fall_cap = 1,
    .tim_ch0_clr_en   = 0,

    .tim_ch1_mode     = TIMERx_MODE_PWM,
    .tim_ch1_pol      = TIMERx_IOPOL_POSITIVE,

    .tim_th           = 32000,
    .tim_cmp1         = 16000,

    .tim_irq_en       = (TIMERx_IRQ_CH0 | TIMERx_IRQ_ZERO),
};



#endif
