/****************************************************************************/
/** \file main.c
**
**  QEP demo for ODS_5010 (Cortex-M0+)
**  正交编码器接口 QEP 示例：读取增量式编码器位置/转向，并演示上溢/下溢中断。
**
**  引脚接线（对应 GPIO管脚复用功能表单.xls 的"引脚定义(New)"方案）：
**    QEP_A（编码器 A 相） -> PTA12    复用功能 13，输入
**    QEP_B（编码器 B 相） -> PTA13    复用功能 13，输入
**    QEP_Z（编码器 Z 索引相）-> PTA14  复用功能 13，输入
**    对应连接器 J3_B68/J3_B69/J3_B70（电机 HALL 接口）。
**
**    注：若在 FPGA Rev0（旧引脚方案）上运行，QEP 信号在 PTA15/PTB0/PTB1，
**        复用功能 3，与芯片验证 Roadtest_A21_QEP_5010_FPGA 一致。
**
**  功能说明：
**    - 正交 AB 相计数模式 QEP_Mode_T1_T2，配合 QEP_FE_CNT_EN 在上升+下降沿
**      都计数（四倍频），用于普通增量式编码器。
**    - 计数器向上计数到门限 QEP_TH 触发上溢中断；向下计数到 0 触发下溢中断。
**    - 中断由 qep_hal.c 中的 QEP_IRQHandler 统一处理，通过 g_qep_int_callback
**      回调到本文件统计中断次数。
**    - 主循环通过 TIM_QEP_GetCount() 读取当前计数值，串口周期性打印。
**    - 编码器反向时计数值递减，通过相邻两次采样值可判断转向。
**
**  注：QEP/QEP_BASE 寄存器宏定义在 ODS_5010.h，由 platform_common.h
**      间接包含；本 demo 直接在主函数中配置 QEP（便于观察每个参数），
**      也可改为调用 qep_hal.c 提供的 QEP_Init() 一键初始化。
*****************************************************************************/
#include "platform_common.h"
#include "gpio_hal.h"
#include "clock_hal.h"
#include "uart_hal.h"
#include "uart_hal_cfg.h"

#include "qep.h"        /* TIM_QEPInit / TIM_QEP_GetCount 等 */
#include "qep_hal.h"    /* g_qep_int_callback / QEP_Init */

#include "cmdline_internal.h"

/* ---------- 编译期可调参数 ---------- */
#define QEP_TH_VALUE    1000U    /* 计数门限：计数器在 0~TH 之间往返, 达到 TH 或 0 触发中断 */
#define QEP_PRINT_MS    500U     /* 串口打印周期 (ms), 用 delay_1us 粗略延时 */

/* 编码器中断统计（在 QEP_IRQHandler -> g_qep_int_callback 中更新） */
static volatile uint32_t g_qep_up_int_cnt    = 0;  /* 上溢中断次数：计数达到 TH（正转） */
static volatile uint32_t g_qep_down_int_cnt  = 0;  /* 下溢中断次数：计数减到 0 （反转） */
static volatile uint32_t g_qep_int_total_cnt = 0;  /* 中断总次数 */

/**
 *@brief configure the gpio of UART0
 *@param[in] none
 *@return    none
 */
static void uart0_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;

    /* UART0_RX, PTA1  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_1;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* UART0_TX, PTA2  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_2;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
}

/**
 *@brief configure QEP_A/B/Z pins as encoder signal inputs
 *@param[in] none
 *@return    none
 *@note  QEP_A=PTA12, QEP_B=PTA13, QEP_Z=PTA14, 复用功能 13
 *       （New 引脚方案; 旧 FPGA 方案为 PTA15/PTB0/PTB1 + AF3）
 */
static void qep_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;

    /* QEP_A, PTA12 */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_12;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_13;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* QEP_B, PTA13 */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_13;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_13;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* QEP_Z, PTA14 */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_14;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_13;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
}

/**
 *@brief QEP 中断回调：由 qep_hal.c 的 QEP_IRQHandler 调用
 *@param[in] source: 中断源, 见 qep_type.h 的 qep_int_source_e
 *                    QEP_UP_0=0: 上溢(计数到 TH)   QEP_OF_1=1: 下溢(计数到 0)
 *@return    none
 */
static void qep_int_callback(qep_int_source_e source)
{
    if (source == QEP_UP_0)
    {
        g_qep_up_int_cnt++;
    }
    else if (source == QEP_OF_1)
    {
        g_qep_down_int_cnt++;
    }
    g_qep_int_total_cnt++;
}

/**
 *@brief 初始化 QEP 模块：正交四倍频计数 + 上溢/下溢中断
 *@param[in] none
 *@return    none
 *@note  也可直接调用 qep_hal.c 的 QEP_Init() 完成等价初始化
 */
static void qep_module_init(void)
{
    TIM_QEPInitTypeDef TIM_QEPInitStruct;

    TIM_QEPStructInit(&TIM_QEPInitStruct);               /* 结构体初始化 */

    TIM_QEPInitStruct.QEP_FE_CNT_EN = ENABLE;            /* 上升+下降沿都计数(四倍频), 配合 T1_T2 模式 */
    TIM_QEPInitStruct.QEP_Mode      = QEP_Mode_T1_T2;    /* 正交 AB 相计数模式 */
    TIM_QEPInitStruct.QEP_ClockDiv  = QEP_CLK_DIV1;      /* 编码器时钟 1 分频 */
    TIM_QEPInitStruct.QEP_Filter    = 0;                 /* 输入滤波系数 */
    TIM_QEPInitStruct.QEP_TH        = QEP_TH_VALUE;      /* 计数门限 */
    TIM_QEPInitStruct.QEP_ZEC       = ENABLE;            /* Z 信号边沿清零 QEP 计数器使能 */
    TIM_QEPInitStruct.QEP_ZLC       = DISABLE;           /* Z 信号电平清零 QEP 计数器使能 */
    TIM_QEPInitStruct.QEP_ZPC       = ENABLE;            /* Z 信号高电平/上升沿清零使能 */
    TIM_QEPInitStruct.QEP_ZNC       = DISABLE;           /* Z 信号低电平/下降沿清零使能 */
    TIM_QEPInitStruct.QEP_IRQEna    = QEP_IRQEna_UnderFlow | QEP_IRQEna_OverFlow; /* 上溢+下溢中断 */

    TIM_QEPInit(QEP, &TIM_QEPInitStruct);                /* 写入配置寄存器 */
    TIM_QEPCmd(QEP, ENABLE);                             /* QEP 模块使能 */
}

/**
 *@brief 使能 QEP 中断并注册回调
 *@param[in] none
 *@return    none
 *@note  QEP_IRQHandler 由 qep_hal.c 提供（中断号 19, 向量表固定），
 *       中断服务函数会调用 g_qep_int_callback 回调
 */
static void qep_int_enable(void)
{
    g_qep_int_callback = qep_int_callback;               /* 注册回调 */

    NVIC_SetPriority(QEP_IRQn, 1);
    NVIC_ClearPendingIRQ(QEP_IRQn);
    NVIC_EnableIRQ(QEP_IRQn);
}

int main(void)
{
    delay_1us(200000);

    /* 外设时钟使能 */
    flash_clk_enable();
    gpio_clk_enable();
    uart_clk_enable();
    qep_clk_enable();                                    /* QEP 模块时钟使能 */

    /* 引脚初始化 */
    uart0_gpio_init();                                   /* 串口调试引脚 */
    qep_gpio_init();                                     /* QEP 编码器引脚 */

    /* 串口初始化(控制台输出) */
    (void)uart_init(UART_CONSOLE_INSTANCE, &uart0_init_config);
    uart_transmit_enable(UART_CONSOLE_INSTANCE, true);
    cmdline_init();

    INFO("\n SDK start running...%s,%s,%s\n", FW_VER, COMPILER_DATE, COMPILER_TIME);
    INFO("QEP demo: PTA12(A)/PTA13(B)/PTA14(Z), mode=T1_T2 x4, TH=%d\n", (int)QEP_TH_VALUE);

    /* QEP 模块初始化 + 中断使能 */
    qep_module_init();
    qep_int_enable();

    __enable_irq();                                      /* 使能全部中断 */

    /* 编码器计数采样 */
    uint16_t last_cnt = TIM_QEP_GetCount(QEP);

    while (1)
    {
        cmdline_handle_polling();

        uint16_t cur_cnt = TIM_QEP_GetCount(QEP);        /* 读取当前计数值 */
        int16_t  diff    = (int16_t)(cur_cnt - last_cnt); /* 相邻两次差值, >0 正转, <0 反转 */
        last_cnt         = cur_cnt;

        INFO("QEP_CNT=%5d  diff=%+5d  INT_up=%u  INT_down=%u  total=%u\n",
             (int)cur_cnt, (int)diff,
             (unsigned)g_qep_up_int_cnt, (unsigned)g_qep_down_int_cnt,
             (unsigned)g_qep_int_total_cnt);

        delay_1us(QEP_PRINT_MS * 1000U);                 /* 打印周期(ms) */
    }
}
