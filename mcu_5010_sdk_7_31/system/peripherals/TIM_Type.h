#ifndef TIMx_TYPE_H
#define TIMx_TYPE_H


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

#define	TIMERx_IRQ_CH0	(0)
#define	TIMERx_IRQ_CH1	(1)
#define	TIMERx_IRQ_ZERO	(2)
#define	TIMERx_IRQ_FAIL	(3)


#endif
