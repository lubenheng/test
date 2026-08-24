/****************************************************************************/
/** \file isr.c
**
**  Trimmed exception handlers for the GPIO demo. GPIO_A/B/C_IRQHandler are
**  NOT redefined here: system/hal/gpio_hal.c already provides real
**  implementations, and startup_ARMCM0plus.s exports every other peripheral
**  IRQHandler as [WEAK] falling through to Default_Handler, so they don't
**  need stubs here either.
*****************************************************************************/
#include "platform_common.h"
#include "TIM_hal.h"
#include "htol_action.h"


/****************************************************************************/
/*	Local pre-processor symbols/macros('#define')
*****************************************************************************/

/****************************************************************************/
/*	Global variable definitions(declared in header file with 'extern')
*****************************************************************************/
//extern void DelayTime_ms(unsigned int delay);

/****************************************************************************/
/*	Local type definitions('typedef')
*****************************************************************************/

/****************************************************************************/
/*	Local variable  definitions('static')
*****************************************************************************/

/****************************************************************************/
/*	Local function prototypes('static')
*****************************************************************************/

/****************************************************************************/
/*	Function implementation - global ('extern') and local('static')
*****************************************************************************/
     
/****************************************************************************
 ** \brief	NMI_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void NMI_Handler(void)
{
	//INFO("IWDT interrupt! \n");
}

/****************************************************************************
 ** \brief	HardFault_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/

void HardFault_Handler(void)
{
	//INFO("Hardfault! \n");
}

/****************************************************************************
 ** \brief	SVC_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void SVC_Handler(void)
{
	
}

/****************************************************************************
 ** \brief	PendSV_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void PendSV_Handler(void)
{
	
}

/****************************************************************************
 ** \brief	SysTick_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note
****************************************************************************/
void SysTick_Handler(void)
{

	
}

/****************************************************************************
 ** \brief	PMU_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 0
****************************************************************************/
// void PMU_IRQHandler(void)
// {
//	
// }
/****************************************************************************
 ** \brief	Code_Flash_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 1
****************************************************************************/
void Code_Flash_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	UART0_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 2
****************************************************************************/
//void UART0_IRQHandler(void)
//{
//	
//}

/****************************************************************************
 ** \brief	UART1_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 3
****************************************************************************/
//void UART1_IRQHandler(void)
//{
//	
//}

 /****************************************************************************
  ** \brief	SPI0_IRQHandler
  **
  ** \param [in]  none   
  ** \return none
  ** \note 	number 4
 ****************************************************************************/
 void SPI0_IRQHandler(void)
 {
	
 }

 /****************************************************************************
  ** \brief	SPI1_IRQHandler
  **
  ** \param [in]  none   
  ** \return none
  ** \note 	number 5
 ****************************************************************************/
 void SPI1_IRQHandler(void)
 {
	
 }

/****************************************************************************
 ** \brief	MCPWM1_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 6
****************************************************************************/

 void MCPWM0_IRQHandler(void)
 {

	
 }

/****************************************************************************
 ** \brief	MCPWM2_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 7
****************************************************************************/
 void MCPWM1_IRQHandler(void)
 {

	
 }

 #if 0
/****************************************************************************
 ** \brief	I2C0_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 8
****************************************************************************/
void I2C0_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	I2C1_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 9
****************************************************************************/
void I2C1_IRQHandler(void)
{
	
}

#endif

/****************************************************************************
 ** \brief	GPIO_A_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 10
****************************************************************************/
// void GPIO_A_IRQHandler(void)
// {
//	
// }

/****************************************************************************
 ** \brief	GPIO_B_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 11
****************************************************************************/
// void GPIO_B_IRQHandler(void)
// {
//	
// }

/****************************************************************************
 ** \brief	GPIO_C_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 12
****************************************************************************/
// void GPIO_C_IRQHandler(void)
// {
//	
// }

/****************************************************************************
 ** \brief	TIM13_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 13
****************************************************************************/
void TIM13_IRQHandler(void)
{


}

/****************************************************************************
 ** \brief	TIM14_IRQHandler ~ TIM16_IRQHandler
 **
 ** \param [in]  none
 ** \return none
 ** \note 	number 14 ~ 16
**
 ** 三个定时器 CH0 都是输入捕获 PWM，测量逻辑完全一致，统一在 tim_capture_isr()
 ** 中实现，这里只做转发。中断里只算结果不打印（阻塞式 uart1_printf 会拉长 ISR
 ** 导致漏捕获）；打印统一放 TIM_test() 主流程。
****************************************************************************/
tim_cap_t g_tim14_cap;
tim_cap_t g_tim15_cap;
tim_cap_t g_tim16_cap;

static void tim_capture_isr(TIMx_TypeDef *timerx, tim_cap_t *m)
{
    /* 读取并清除 CH0/CH1/ZERO 中断标志；返回值 bit0=CH0, bit1=CH1, bit2=ZERO */
    uint8_t status = tim_hal_irq_status(timerx);

    /* ZERO：计数器回绕归零（回绕点 TH+1），回绕计数加一 */
    if (status & (1 << 2)) {
        m->ovf++;
    }

    /* CH0 输入捕获 */
    if (status & (1 << 0)) {
        uint32_t cap = tim_hal_ch0_capnum(timerx);
        uint32_t cnt = TIM_hal_Counter_num(timerx);   /* 当前计数值（ISR 刚进入，延迟很小） */
        /* 回绕模数 = TH+1，直接读寄存器。计数时钟 32MHz>>CKDIV 只在主流程
           Tim_cap_finalize 算 freq 时用到，不在 ISR 里读 */
        uint32_t mod = (uint32_t)timerx->TH + 1U;
        uint32_t cyc = m->ovf;                        /* 当前所处的计数周期 */

        /* 捕获之后若又回绕了一次（CNT 已回落到 cap 之下），则该捕获属于上一个计数周期 */
        if (cnt < cap && cyc != 0U) {
            cyc--;
        }

        /* 重建绝对计数值 = 周期序号×mod + 捕获值；abs 允许溢出，相邻相减仍正确。
           刻意用 32 位乘法：M0+ 无 64 位乘法指令，64 位软乘拖慢 ISR 会导致高频丢沿 */
        uint32_t abs = cyc * mod + cap;

        if (!m->got) {                        /* 第一个沿只建立基准，不参与计算 */
            m->prev_abs = abs;
            m->got = 1;
            return;
        }

        uint32_t delta = abs - m->prev_abs;   /* 相邻两次捕获的时间差 */
        m->prev_abs = abs;

        if (m->edge_n == 0) {
            m->half_t = delta;                /* 前半周期（高或低） */
        } else {
            uint32_t period = m->half_t + delta;   /* 完整周期 = 两半相加 */
            if (period != 0U) {
                uint32_t after_lvl = (uint32_t)((m->pin_port->DR_STR.w >> m->pin_bit) & 1U);
                uint32_t on_t = (after_lvl == 0U) ? delta : m->half_t;
                /* 中断里只存原始 period/on_t，不在 ISR 里做除法：M0+ 无硬件除法，
                   freq/duty 的 32 位除会编成 __aeabi_uidiv 软件除法（每次上百周期），
                   双路 10kHz 捕获同时触发时会把 ISR 拖长到丢沿（CMP0 被下一沿覆盖）。
                   除法统一放到采集窗口结束后的主流程 Tim_cap_finalize() */
                m->period = period;
                m->on_t   = on_t;
                m->ready  = 1;                /* 不在中断里打印，交给主流程 */
            }
        }
        m->edge_n ^= 1U;
    }
}

void TIM14_IRQHandler(void)
{
    tim_capture_isr(TIMER14, &g_tim14_cap);
}

void TIM15_IRQHandler(void)
{
    tim_capture_isr(TIMER15, &g_tim15_cap);
}

void TIM16_IRQHandler(void)
{
    tim_capture_isr(TIMER16, &g_tim16_cap);
}

/****************************************************************************
 ** \brief	ADC_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 17
****************************************************************************/
void ADC_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	HALL_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 18
****************************************************************************/
 void HALL_IRQHandler(void)
 {

	
 }

/****************************************************************************
 ** \brief	QEP_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 19
****************************************************************************/
void QEP_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	DMA_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 20
****************************************************************************/
void DMA_IRQHandler(void)
{			

}

/****************************************************************************
 ** \brief	Reserved21_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 21
****************************************************************************/
void Reserved21_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	Reserved22_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 22
****************************************************************************/
void Reserved22_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	Reserved23_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 23
****************************************************************************/
void Reserved23_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	Reserved24_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 24
****************************************************************************/
void Reserved24_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	Reserved25_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 25
****************************************************************************/
void Reserved25_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	Reserved26_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 26
****************************************************************************/
void Reserved26_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	Reserved27_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 27
****************************************************************************/
void Reserved27_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	Reserved28_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 28
****************************************************************************/
void Reserved28_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	Reserved29_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 29
****************************************************************************/
void Reserved29_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	Reserved30_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 30
****************************************************************************/
void Reserved30_IRQHandler(void)
{
	
}

/****************************************************************************
 ** \brief	Reserved31_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 31
****************************************************************************/
void Reserved31_IRQHandler(void)
{
	
}

