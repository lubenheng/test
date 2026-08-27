/****************************************************************************/
/** \file main.c
**
**  UART0 demo for ODS_5010 (Cortex-M0+)
**  UART0 配置初始化 + 基础收发(发送用轮询,接收方式编译期可选)。
**
**  接收方式由 RX_MODE 宏选择:
**      RX_MODE_POLLING   - 轮询接收
**      RX_MODE_INTERRUPT - 中断接收
**      RX_MODE_DMA       - DMA 接收
*****************************************************************************/
#include "platform_common.h"
#include "test.h"
#include "gpio_hal.h"
#include "clock.h"
#include "uart_hal.h"
#include "uart_hal_cfg.h"
#include "uart_reg.h"   /* UART_TypeDef 寄存器定义(UART0->CR_STR 等) */

/* 接收方式选择: 0=轮询  1=中断  2=DMA, 在此处切换 */
#define RX_MODE_POLLING     0
#define RX_MODE_INTERRUPT   1
#define RX_MODE_DMA         2
#define RX_MODE             RX_MODE_POLLING

#if (RX_MODE == RX_MODE_DMA)
#include "dma_hal.h"
#include "dma.h"
#endif

#define RX_DATA_LEN         8U   /* 每次接收的字节数 */

#if (RX_MODE != RX_MODE_DMA)
static uint8_t g_rx_buf[RX_DATA_LEN];   /* 轮询/中断模式的接收缓冲区 */
#endif

#if (RX_MODE == RX_MODE_INTERRUPT)
static volatile bool g_rx_done = false;   /* 收满一帧后由 ISR 置位 */
static volatile uint8_t g_rx_idx = 0U;    /* 当前已收到的字节数 */
#endif

#if (RX_MODE == RX_MODE_DMA)
static const DMA_Channel_e g_dma_ch = DMA_CHANNEL_0;
static dma_ch_config_t g_dma_rx_cfg;
static uint8_t g_rx_buf_dma[RX_DATA_LEN];
#endif


/**
 *@brief configure the gpio of UART0
 *@param[in] none
 *@return    none
 */
static void uart0_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;

	/* UART0_RX, PB5 (与 HTOL 板实际使用的复用一致) */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_1;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* UART0_TX, PB6 (与 HTOL 板实际使用的复用一致) */
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
 *@brief enable the clock of UART0
 *@param[in] none
 *@return    none
 */
static void uart0_clk_init(void)
{
	/* UART0 时钟已由 uart_clk_enable() 使能, 如需配置分频/门控可在此添加 */
}

/**
 *@brief 把收到的数据打印出来, 再原样回发(轮询发送)
 *@param[in] buf: 待回显数据
 *@param[in] len: 数据长度
 *@return    none
 */
static void uart_echo(const uint8_t *buf, uint32_t len)
{
    uint32_t i;

    INFO("[RX] ");
    for (i = 0U; i < len; i++)
    {
        INFO("%02X ", (unsigned int)buf[i]);
    }
    INFO("\n");

    (void)uart_send_data_polling(UART_INSTANCE0, buf, len);
    INFO("[TX echo done]\n");
}

#if (RX_MODE == RX_MODE_POLLING)
/**
 *@brief 轮询接收, 阻塞直到收满 len 字节
 *@param[in] buf: 接收缓冲区
 *@param[in] len: 期望接收长度
 *@return    none
 */
static void uart_rx_polling(uint8_t *buf, uint32_t len)
{
    uint8_t ch = 0U;
    uint32_t i = 0U;

    INFO("Please send %u bytes...\n", len);
    while (i < len)
    {
        if (uart_receive_data_polling_for_cmdline(UART_INSTANCE0, &ch, 1U))
        {
            buf[i++] = ch;
        }
    }
}
#endif /* RX_MODE_POLLING */

#if (RX_MODE == RX_MODE_INTERRUPT)
/**
 *@brief UART0 中断接收处理, 挂接到 uart.c 的 UART0_IRQHandler 分发链
 *
 *        数据到达的中断源用 RXDFFIE(接收FIFO到达水位中断)+RXWATER 水位;
 *        数据判断用 SR_STR.RXEMPT==0("FIFO 有数据", 与轮询接收同一判据),
 *        不再依赖 HAL uart_irq_handler 里 RXOVF/RXOVIE 的"溢出"中断语义。
 */
static void uart0_rx_isr(uart_instance_e instance)
{
    (void)instance;
    UART_TypeDef *base = UART0;

    /* 把 FIFO 中当前可读的字节全部取走 */
    while (base->SR_STR.RXEMPT == 0U)
    {
        if (g_rx_idx < RX_DATA_LEN)
        {
            g_rx_buf[g_rx_idx++] = (uint8_t)base->DR_STR.DATA;
        }
        else
        {
            (void)base->DR_STR.DATA;   /* 帧已收满, 丢弃多余字节 */
        }
    }

    /* 清中断标志(写1清0) */
    base->IR_STR.w = 0xFFFFFFFFUL;

    if (g_rx_idx >= RX_DATA_LEN)
    {
        g_rx_done = true;
        /* 收满后关中断, 等 main 回显完再重新武装, 避免覆盖缓冲区 */
        base->IER_STR.RXDFFIE = 0U;
    }
}
#endif /* RX_MODE_INTERRUPT */

#if (RX_MODE == RX_MODE_DMA)
/**
 *@brief 配置 UART0 RX FIFO + DMA 通道, 一次性接收 RX_DATA_LEN 字节
 *@param[in] none
 *@return    none
 */
static void uart_dma_rx_init(void)
{
    memset(&g_dma_rx_cfg, 0, sizeof(g_dma_rx_cfg));

    /* UART 侧: 使能 RX FIFO + RX DMA 请求, 水位=1(每收1字节请求一次DMA) */
    UART0->CR_STR.RXFE       = 1U;
    UART0->CR_STR.RXE        = 1U;
    UART0->CR_STR.RDMAE      = 1U;
    UART0->WATER_STR.RXWATER = 1U;

    /* DMA 通道配置: 外设 DR 寄存器 -> 内存 */
    g_dma_rx_cfg.dma_ch                   = g_dma_ch;
    g_dma_rx_cfg.DMA_Trigger_periph       = DMA_Trigger_UART0_RX;
    g_dma_rx_cfg.src_addr                 = (uint32_t)&UART0->DR_STR;
    g_dma_rx_cfg.dst_addr                 = (uint32_t)g_rx_buf_dma;
    g_dma_rx_cfg.src_inc                  = Addr_Disable;          /* 外设地址固定 */
    g_dma_rx_cfg.dst_inc                  = Addr_Enable;           /* 内存地址自增 */
    g_dma_rx_cfg.dir                      = DMA_DIR_PERIPH_TO_MEM;
    g_dma_rx_cfg.burst_len                = DMA_BURST_1_UNIT;
    g_dma_rx_cfg.dma_data_width_src_width = DMA_BITSIZE_8BIT;
    g_dma_rx_cfg.dma_data_width_dst_width = DMA_BITSIZE_8BIT;
    g_dma_rx_cfg.transfer_size            = RX_DATA_LEN;
    g_dma_rx_cfg.circular_mode            = DMA_MODE_NORMAL;       /* 一次完成即停止 */
    g_dma_rx_cfg.priority                 = DMA_PRIORITY_MID;

    /* 先关通道再改寄存器, 恢复默认触发源, 然后配置并使能 */
    DMA_div_chx_Disable(g_dma_ch);
    DMA_div_TriggerSource_Clean();
    HAL_DMA_StructerInit(&g_dma_rx_cfg);
    DMA_div_chx_Enable(g_dma_ch);
}

/**
 *@brief 新一轮 DMA 接收前重新装载配置(通道会自停, 需要重新武装)
 *@param[in] none
 *@return    none
 */
static void uart_dma_rx_arm(void)
{
    DMA_div_chx_Disable(g_dma_ch);
    HAL_DMA_IT_Transfer_Clean(g_dma_ch);
    HAL_DMA_StructerInit(&g_dma_rx_cfg);
    DMA_div_chx_Enable(g_dma_ch);
}

/**
 *@brief 查询 DMA 是否收满(RX_DATA_LEN 字节)
 *@param[in] none
 *@return    true=已完成
 */
static bool uart_dma_rx_complete(void)
{
    return (HAL_DMA_GetRemainingCnt(g_dma_ch) == 0U);
}
#endif /* RX_MODE_DMA */


int main(void)
{
    delay_1us(200000);

    flash_clk_enable();

	/* Add Clock Enable Code Here */
	gpio_clk_enable();
    uart_clk_enable();

#ifdef UART
	uart0_gpio_init();
	uart0_clk_init();
	(void)uart_init(UART_CONSOLE_INSTANCE, &uart0_init_config);

	uart_transmit_enable(UART_CONSOLE_INSTANCE, true);
	uart_receive_enable(UART_CONSOLE_INSTANCE, true);

	INFO("\n SDK start running...%s,%s,%s\n", FW_VER, COMPILER_DATE, COMPILER_TIME);

	uint32_t mcu_clk = 0U;
	(void)clock_freq_get(SCG_SYS, &mcu_clk);
	INFO("MCU_CLK = %dMHz\n", mcu_clk/1000000U);

#if (RX_MODE == RX_MODE_INTERRUPT)
    NVIC_SetPriority(UART0_IRQn, 1); 		/* QEP_IRQn 优先级配置 */
    NVIC_ClearPendingIRQ(UART0_IRQn);
    NVIC_EnableIRQ (UART0_IRQn);
	/* 中断接收: 挂接自定义 RX ISR(由 uart.c 的 UART0_IRQHandler 分发) */
	uart_interrupt_handle_cfg(UART_INSTANCE0, uart0_rx_isr);
	/* 数据到达中断源: RX 水位=1(每收1字节触发)+ 到达水位中断使能。
	   注意: HAL 的 uart_receive_data 只使能 RXOVIE(溢出中断), 正常收数据不触发,
	   故中断接收不调用它, 直接在此配置正确的数据到达中断。 */
	UART0->WATER_STR.RXWATER = 1U;
	UART0->IER_STR.RXDFFIE   = 1U;
	g_rx_idx = 0U;
	INFO("UART0 interrupt RX ready (%u bytes)\n", RX_DATA_LEN);
#elif (RX_MODE == RX_MODE_DMA)
	/* DMA 接收: 直接在 main 中配置 DMA 并启动 */
	uart_dma_rx_init();
	INFO("UART0 DMA RX ready (%u bytes, DMA_CH%d)\n", RX_DATA_LEN, g_dma_ch);
#else
	INFO("UART0 polling RX ready (%u bytes)\n", RX_DATA_LEN);
#endif /* RX_MODE */
#endif /* UART */

	__enable_irq();						  		/* 使能全部中断 */

    while(1)
    {
#ifdef UART
#if (RX_MODE == RX_MODE_INTERRUPT)
        if (g_rx_done)
        {
            g_rx_done = false;
            g_rx_idx = 0U;
            uart_echo(g_rx_buf, RX_DATA_LEN);
            /* 重新武装下一帧接收 */
            UART0->IER_STR.RXDFFIE = 1U;
        }
#elif (RX_MODE == RX_MODE_DMA)
        if (uart_dma_rx_complete())
        {
            uart_echo(g_rx_buf_dma, RX_DATA_LEN);
            uart_dma_rx_arm();
        }
#else
        uart_rx_polling(g_rx_buf, RX_DATA_LEN);
        uart_echo(g_rx_buf, RX_DATA_LEN);
#endif /* RX_MODE */
#endif /* UART */
    }
}
