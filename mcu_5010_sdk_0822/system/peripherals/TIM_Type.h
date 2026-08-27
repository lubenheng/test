#ifndef TIMx_TYPE_H
#define TIMx_TYPE_H

#include <stdint.h>

#define TIMER_DIV_1		(0)
#define TIMER_DIV_2		(1)
#define TIMER_DIV_4		(2)
#define TIMER_DIV_8		(3)
#define TIMER_DIV_16		(4)
#define TIMER_DIV_32		(5)
#define TIMER_DIV_64		(6)
#define TIMER_DIV_128		(7)

#define	TIMERx_CH0		(0)
#define	TIMERx_CH1		(1)

#define	TIMER_TIM13		(0)
#define	TIMER_TIM14		(1)
#define	TIMER_TIM15		(2)
#define	TIMER_TIM16		(3)

#define	TIMERx_MODE_PWM		(0)
#define	TIMERx_MODE_CAP		(1)

#define	TIMERx_IOPOL_POSITIVE		(1)
#define	TIMERx_IOPOL_NEGAGTIVE		(0)

#define	TIMERx_SRC_IO0		(0)
#define	TIMERx_SRC_IO1		(1)
#define	TIMERx_SRC_CLU0		(2)
#define	TIMERx_SRC_CLU1		(3)
#define	TIMERx_SRC_CLU2		(4)
#define	TIMERx_SRC_CLU3		(5)
#define	TIMERx_SRC_CLU4		(6)
#define	TIMERx_SRC_IO0_XOR_IO1		(9)

#define	TIMERx_INNERCLK		(0)
#define	TIMERx_OUTSIDECLK		(1)

#define	TIMERx_AUTO_RELOAD		(0)
#define	TIMERx_EXTTRIG_RELOAD		(1)

#define	TIMERx_STOP_DISABLE		(0)
#define	TIMERx_STOP_ENABLE		(1)

#define	TIMERx_EXT_START_DIS		(0)
#define	TIMERx_EXT_START_EN		(1)

#define	TIMERx_CNTR_UP		(0)
#define	TIMERx_CNTR_DOWN		(1)


#define	TIMERx_EDGE_MODE		(0)
#define	TIMERx_SYMMETRIC_MODE		(1)

#define	TIMERx_SHADOW_REG_DIS		(0)
#define	TIMERx_SHADOW_REG_EN		(1)

#define	TIMER13_CH0_CAPCOM_EVT	(0)
#define	TIMER13_CH1_CAPCOM_EVT	(1)
#define	TIMER14_CH0_CAPCOM_EVT	(2)
#define	TIMER14_CH1_CAPCOM_EVT	(3)
#define	TIMER15_CH0_CAPCOM_EVT	(4)
#define	TIMER15_CH1_CAPCOM_EVT	(5)
#define	TIMER16_CH0_CAPCOM_EVT	(6)
#define	TIMER16_CH1_CAPCOM_EVT	(7)

#define	MCPWM_TADC0_COM_EVT	(8)
#define	MCPWM_TADC1_COM_EVT	(9)
#define	MCPWM_TADC2_COM_EVT	(10)
#define	MCPWM_TADC3_COM_EVT	(11)

#define	TIMERx_IRQ_CH0	(1U << 0)
#define	TIMERx_IRQ_CH1	(1U << 1)
#define	TIMERx_IRQ_ZERO	(1U << 2)
#define	TIMERx_IRQ_FAIL	(1U << 3)

/**
 * TIM 配置结构体
 * 字段与 TIMx_TypeDef 寄存器一一对应，取值可参考文件顶部的宏定义（TIMER_DIV_x / TIMERx_*）。
 */
typedef struct
{
    /*----------- 计数器基本配置（TIM_CFG 寄存器） -----------*/
    uint8_t tim_div;            /*!< 时钟分频：TIMER_DIV_1 ~ TIMER_DIV_128 */
    uint8_t tim_clk_src;        /*!< 时钟源：TIMERx_INNERCLK / TIMERx_OUTSIDECLK */
    uint8_t tim_cnt_dir;        /*!< 计数方向：TIMERx_CNTR_UP / TIMERx_CNTR_DOWN */
    uint8_t tim_cnt_mode;       /*!< 计数模式：TIMERx_EDGE_MODE / TIMERx_SYMMETRIC_MODE */
    uint8_t tim_reload_mode;    /*!< 重载模式：TIMERx_AUTO_RELOAD / TIMERx_EXTTRIG_RELOAD */
    uint8_t tim_shadow;         /*!< 影子寄存器：TIMERx_SHADOW_REG_DIS / TIMERx_SHADOW_REG_EN */
    uint8_t tim_gate_en;        /*!< 门控停止使能：TIMERx_STOP_DISABLE / TIMERx_STOP_ENABLE */
    uint8_t tim_ext_start_en;   /*!< 外部触发启动：TIMERx_EXT_START_DIS / TIMERx_EXT_START_EN */

    /*----------- 通道 0 配置（CH0RCE/CH0FCE/CH0M/CH0POL/CH0CAPSRC/CCLR0E） -----------*/
    uint8_t tim_ch0_mode;       /*!< 通道0模式：TIMERx_MODE_PWM / TIMERx_MODE_CAP */
    uint8_t tim_ch0_pol;        /*!< 通道0极性：TIMERx_IOPOL_POSITIVE / TIMERx_IOPOL_NEGAGTIVE */
    uint8_t tim_ch0_cap_src;    /*!< 通道0捕获源：TIMERx_SRC_IO0 ~ TIMERx_SRC_IO0_XOR_IO1 */
    uint8_t tim_ch0_rise_cap;   /*!< 通道0上升沿捕获使能 */
    uint8_t tim_ch0_fall_cap;   /*!< 通道0下降沿捕获使能 */
    uint8_t tim_ch0_clr_en;     /*!< 通道0捕获后计数器清零使能 */

    /*----------- 通道 1 配置（CH1RCE/CH1FCE/CH1M/CH1POL/CH1CAPSRC/CCLR1E） -----------*/
    uint8_t tim_ch1_mode;       /*!< 通道1模式：TIMERx_MODE_PWM / TIMERx_MODE_CAP */
    uint8_t tim_ch1_pol;        /*!< 通道1极性：TIMERx_IOPOL_POSITIVE / TIMERx_IOPOL_NEGAGTIVE */
    uint8_t tim_ch1_cap_src;    /*!< 通道1捕获源：TIMERx_SRC_IO0 ~ TIMERx_SRC_IO0_XOR_IO1 */
    uint8_t tim_ch1_rise_cap;   /*!< 通道1上升沿捕获使能 */
    uint8_t tim_ch1_fall_cap;   /*!< 通道1下降沿捕获使能 */
    uint8_t tim_ch1_clr_en;     /*!< 通道1捕获后计数器清零使能 */

    /*----------- 定时数值（寄存器） -----------*/
    uint32_t tim_th;            /*!< 门限值/周期（TH 寄存器）：0 ~ (2^32-1) */
    uint32_t tim_cmp0;          /*!< 比较值0/捕获值0（CMP0 寄存器） */
    uint32_t tim_cmp1;          /*!< 比较值1/捕获值1（CMP1 寄存器） */
    uint32_t tim_evt_src;       /*!< 事件源（EVTSRC 寄存器）：TIMER13_CH0_CAPCOM_EVT 等 */
    uint32_t tim_filt;          /*!< 输入滤波系数（FILT 寄存器）：0 ~ 255 */

    /*----------- 输出/中断配置（FAE / IER 寄存器） -----------*/
    uint8_t tim_moe;            /*!< 主输出使能（FAE.MOE） */
    uint8_t tim_irq_en;         /*!< 中断使能位掩码：TIMERx_IRQ_CH0|TIMERx_IRQ_CH1|TIMERx_IRQ_ZERO|TIMERx_IRQ_FAIL */
}TIMx_config_t;


#endif
