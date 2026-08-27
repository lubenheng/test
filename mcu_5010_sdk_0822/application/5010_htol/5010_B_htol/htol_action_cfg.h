#ifndef __HTOL_ACTION_CFG_H
#define __HTOL_ACTION_CFG_H

typedef uint8_t (*htol_test_fn_t)(void);

/* 测试流程的注册表项。
   name：测试项名称（打印用）
   cmd ：B 下发的触发指令（A 收到即执行本流程；via_i2c=1 时写入 I2C TX 缓冲供 STM32 轮询识别）
   ack ：应答字符串（via_i2c=0 为 A 应答 B；via_i2c=1 为 STM32 应答 B 的内容）
   pfn ：本侧测试流程函数（框架在指令握手后调用；内部负责 500ms 同步与执行）
   via_i2c：1 = 指令/应答经 I2C1 与 STM32 交互（B<->STM32 流程，如 STM32 单独校验 B），
            0 = 经 UART1 与 A 交互（A<->B 流程） */
typedef struct
{
    const char     *name;
    const char     *cmd;
    const char     *ack;
    htol_test_fn_t  pfn;
    uint8_t         via_i2c;
} htol_test_entry_t;

/* UART1 指令/应答字符串最大长度，新增流程的 cmd/ack 不得超过该值 */
#define HTOL_CMD_MAX_LEN   16U

/*===========================================================================*/
/* TIMER 双向互测（流程二，A<->B，UART1 握手）期望参数：                        */
/*   A/B 各输出 1kHz/50% PWM，CH0 捕获校验频率/占空比                          */
/*===========================================================================*/
#define TIMER_EXP_FREQ_HZ   1000U   /* 期望频率：A/B 输出 1kHz 标准 PWM */
#define TIMER_FREQ_TOL_PCT  10U     /* 频率允许偏差 ±10% */
#define TIMER_EXP_DUTY_PCT  50U     /* 期望占空比：双向 PWM 均 50% */
#define TIMER_DUTY_TOL_PCT  10U     /* 占空比允许偏差 ±10%（绝对值） */

/*===========================================================================*/
/* STM32 单独校验 OCD5010_B（流程三）参数：B 为 I2C1 从机，STM32 为 I2C 主机。  */
/*   1. B 将测试指令写入 i2c_slave_send_buf，STM32 作为主机轮询读取识别        */
/*   2. STM32 应答（单次 I2C 写事务写入 i2c_slave_rec_buf）后，双方同步延时     */
/*      500ms 再开始执行流程                                                  */
/*   3. 执行：STM32 从 IO25(PWM_INN)/IO35(PWM_IN) 输出互补 PWM，B 开启         */
/*      TIMER16_CH0/TIMER13_CH0 捕获并校验频率/占空比                          */
/*   4. B 将结果写回 i2c_slave_send_buf 供 STM32 读取，IO14(LED) 亮=通过        */
/*   5. 互补 PWM 占空比：CH1(PA8->IO35/TIM13_CH0) 为 STM32_CHECK_EXP_DUTY_PCT， */
/*      CH1N(PA7->IO25/TIM16_CH0) 反相，占空比 = 100% - STM32_CHECK_EXP_DUTY_PCT */
/*===========================================================================*/
#define STM32_CHECK_EXP_FREQ_HZ   5000U   /* 期望频率：STM32 输出 10kHz 标准 PWM */
#define STM32_CHECK_FREQ_TOL_PCT  10U      /* 频率允许偏差 ±10% */
#define STM32_CHECK_EXP_DUTY_PCT  50U      /* 期望占空比：CH1 为 20%（CH1N 反相为 80%） */
#define STM32_CHECK_DUTY_TOL_PCT  10U      /* 占空比允许偏差 ±10%（绝对值） */

/* STM32 每 400ms 轮询一次 I2C 读取发送缓冲。结果写入后必须保留至少一个轮询周期，
   否则下一流程一开始就会清空/覆盖发送缓冲，STM32 永远读不到本次结果。
   取值须 ≥ HTOL_POLL_INTERVAL_MS(400)。B 侧延时实测约为名义值 6 倍，300ms 名义
   实际保留约 1.8s。STM32 流程比较特殊：STM32 回 ack 后要阻塞 500ms+3000ms 输出
   PWM，期间不轮询，PWM 结束(ack+3.5s)才恢复读缓冲；B 在 ack+~2.2s 写结果，故保持
   窗口必须延伸到 ack+3.5s 之后（本值 1.8s 使窗口到 ack+4.0s），再配合 STM32 侧
   PWM 结束后立即主动轮询，保证结果不被下一轮 UART1 结果覆盖。
   结果格式为 "R <序号> <内容>"：首字节 'R' 供 STM32 识别为测试数据（与指令
   "stm32_check" 区分），序号由 STM32 去重，同一序号只转发一次。 */
#define HTOL_RESULT_HOLD_MS       300U


/* ------------------------------------- i2c code ----------------------------------------*/
i2c_slave_config_t i2c_slave_config = {
    .slave_mode = I2C_FAST_MODE,
    .addr_10bit = false,
    .addr_config = I2C_ADDR_MATCH_0_7BIT,
    .slave_addr0 = 0x50U,           /* 本机从机地址 */
    .slave_addr1 = 0x33U,           /* 保留（本控制器只支持单一 IC_SAR 地址，实际只用 slave_addr0） */
    .slave_int_handle = NULL,
    .slave_interrupt_priority = 3U,   /* 低于 TIM 捕获中断(2)：STM32 每 400ms 轮询本从机会触发
                                         I2C1 ISR，若其优先级高于 TIM 会在采集窗口内抢占捕获中断
                                         造成丢沿（CMP0 被覆盖）。I2C 是低速外设（从机可时钟拉伸），
                                         降到最低优先级不影响通信 */
};

/* 自定义发送/接收缓冲区：i2c_slave_msg_t 的 send_data/rec_data 是指针，指向这里的数组。
   发送/接收均按 64 字节整包传输：STM32 主机每次 I2C 读事务全读 64 字节；
   发送缓冲在写入 payload 前已 memset 清零，payload 之后的 64 字节剩余部分为 0x00 填充。 */
uint8_t i2c_slave_send_buf[64] = {0};
uint8_t i2c_slave_rec_buf[64] = {0};

i2c_slave_msg_t i2c_slave_msg = {
    .send_data = i2c_slave_send_buf,
    .send_data_len = sizeof(i2c_slave_send_buf),
    .rec_data = i2c_slave_rec_buf,
    .rec_data_len = sizeof(i2c_slave_rec_buf),
};

/* ------------------------------------- adc code ----------------------------------------*/
/* I2C 从机地址档位识别 ADC：IO36/PA4 = ADC_IN20 = ADC_CH20，参考电压 VDD = 3.3V。
   上电先读该脚电压，按 12 档电压范围确定档位号，从机地址 = 0x50 + 档位号(0~11)。
   i2c_slave_addr_get() 在 htol_action.c 中实现。 */
#define HTOL_ADC_CH         20U           /* ADC 通道：PA4(IO36) = ADC_IN20 电压采样脚 */
#define HTOL_ADC_VREF_MV    3300U         /* ADC 参考电压：VDD = 3.3V */
#define HTOL_ADC_BASE_ADDR  0x50U         /* 档位 0(0.301V) 对应的 I2C 从机地址 */

/* 12 档名义电压(mV)：301 392 503 663 825 1097 1322 1504 1653 2200 2756 3000
   相邻两档取中点作为档位边界；写电压约 ±0.05V 浮动仍可归类到最近档位 */
static const uint16_t htol_band_thr_mv[11] = {
    347U, 448U, 583U, 744U, 961U, 1210U, 1413U, 1579U, 1927U, 2478U, 2878U
};

/*===========================================================================*/
/* TIM 配置：TIM13/14/15/16 均为 CH0 输入捕获 PWM、CH1 输出 PWM，开启 CH0/ZERO 中断 */
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

    .tim_th           = 32000,                  /* 周期：32MHz/32000 = 1kHz */
    .tim_cmp1         = 16000,                   /* CH1 占空比：16000/32000 = 50% */

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

    .tim_th           = 32000,                  /* 周期：32MHz/32000 = 1kHz */
    .tim_cmp1         = 16000,                   /* CH1 占空比：16000/32000 = 50% */

    .tim_irq_en       = (TIMERx_IRQ_CH0 | TIMERx_IRQ_ZERO),
};

/* TIM13：流程三里 STM32 单独检 B 时用 CH0 捕获 STM32 下发的 PWM（PC3 输入），
   无 CH1 输出引脚，CH1 保持 PWM 配置仅用于与其它定时器统一，不影响功能 */
static TIMx_config_t g_tim13_cfg = {
    .tim_clk_src      = TIMERx_INNERCLK,
    .tim_div          = TIMER_DIV_1,
    .tim_cnt_dir      = TIMERx_CNTR_UP,
    .tim_cnt_mode     = TIMERx_EDGE_MODE,
    .tim_moe          = 1,

    /* CH0 输入捕获 PWM */
    .tim_ch0_mode     = TIMERx_MODE_CAP,
    .tim_ch0_cap_src  = TIMERx_SRC_IO0,
    .tim_ch0_rise_cap = 1,
    .tim_ch0_fall_cap = 1,
    .tim_ch0_clr_en   = 0,

    /* CH1 输出 PWM（无引脚，保持统一配置） */
    .tim_ch1_mode     = TIMERx_MODE_PWM,
    .tim_ch1_pol      = TIMERx_IOPOL_POSITIVE,

    .tim_th           = 32000,
    .tim_cmp1         = 16000,

    .tim_irq_en       = (TIMERx_IRQ_CH0 | TIMERx_IRQ_ZERO),
};



#endif
