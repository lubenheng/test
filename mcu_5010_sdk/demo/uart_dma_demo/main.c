/****************************************************************************/
/** \file main.c
**
**  UART0 + DMA demo for ODS_5010 (Cortex-M0+)
**  演示 UART0 通过 DMA 完成收发回环(Echo):
**    - 每轮循环串口提示本轮需要发送的字节数
**    - DMA RX: UART0_DR -> 内存 (CH1, 触发源 UART0_RX), 接收指定字节数
**    - DMA TX: 内存 -> UART0_DR (CH0, 触发源 UART0_TX), 把收到的数据原样回显
**    - PTC6/7/8 翻转作为运行指示
**  注: 本 demo 将 UART0 同时用作控制台打印和 DMA 收发对象,
**      因此 UART 的 TX/RX DMA 使能位(TDMAE/RDMAE)只在对应传输阶段短暂打开,
**      传输结束后立即关闭, 避免与控制台轮询发送(INFO)相互干扰。
*****************************************************************************/
#include "platform_common.h"
#include "test.h"
#include "gpio_hal.h"
#include "clock.h"
#include "clock_hal.h"
#include "uart_hal.h"
#include "uart_hal_cfg.h"
#include "uart_reg.h"   /* UART_TypeDef 寄存器定义(UART0->CR_STR 等) */

#include "dma_hal.h"
#include "dma.h"

/* ---------- 可调参数 ---------- */
#define RX_BUF_LEN       16U   /* RX 缓冲区大小(每轮允许接收的最大字节数) */

/* 每轮提示上位机发送的字节数, 依次循环以演示变长 DMA 收发 */
static const uint8_t g_rx_len_seq[] = {4U, 8U, 12U, 16U};
static uint8_t g_rx_len_idx = 0U;

/* ---------- DMA 通道分配 (TX/RX 各占一个通道) ---------- */
#define DMA_TX_CH        DMA_CHANNEL_0
#define DMA_RX_CH        DMA_CHANNEL_1

static uint8_t g_rx_buf[RX_BUF_LEN];   /* DMA RX 目的缓冲区, 兼作回显数据源 */

static dma_ch_config_t g_dma_tx_cfg;
static dma_ch_config_t g_dma_rx_cfg;


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
 *@brief configure PTC6/7/8 as push-pull outputs (运行指示)
 *@param[in] none
 *@return    none
 */
static void gpio_output_init(void)
{
    gpio_settings_config_t setting_cfg;

    /* PTC6 */
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_6;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* PTC7 */
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_7;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* PTC8 */
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_8;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
}

/**
 *@brief 等待 DMA 通道传输完成(剩余计数归零)
 *@param[in] ch: DMA 通道号
 *@param[in] timeout_ms: 超时时间(ms)
 *@return    true=完成, false=超时
 */
static bool uart_dma_wait_complete(DMA_Channel_e ch, uint32_t timeout_ms)
{
    uint32_t cnt = 0U;

    while (HAL_DMA_GetRemainingCnt(ch) != 0U)
    {
        if (++cnt > (timeout_ms * 1000U))
        {
            return false;
        }
    }
    return true;
}

/**
 *@brief UART0 DMA TX: 内存缓冲 -> UART0 DR 寄存器
 *@param[in] buf: 待发送数据
 *@param[in] len: 发送长度
 *@return    true=发送完成, false=超时
 *@note  期间控制台打印(INFO)应暂停, 避免与 TX DMA 抢占 FIFO
 */
static bool uart_dma_tx_send(const uint8_t *buf, uint32_t len)
{
    memset(&g_dma_tx_cfg, 0, sizeof(g_dma_tx_cfg));

    g_dma_tx_cfg.dma_ch                    = DMA_TX_CH;
    g_dma_tx_cfg.DMA_Trigger_periph        = DMA_Trigger_UART0_TX;
    g_dma_tx_cfg.src_addr                  = (uint32_t)buf;
    g_dma_tx_cfg.dst_addr                  = (uint32_t)&UART0->DR_STR;
    g_dma_tx_cfg.src_inc                   = Addr_Enable;          /* 内存地址自增 */
    g_dma_tx_cfg.dst_inc                   = Addr_Disable;         /* 外设地址固定 */
    g_dma_tx_cfg.dir                       = DMA_DIR_MEM_TO_PERIPH;
    g_dma_tx_cfg.burst_len                 = DMA_BURST_1_UNIT;
    g_dma_tx_cfg.dma_data_width_src_width  = DMA_BITSIZE_8BIT;
    g_dma_tx_cfg.dma_data_width_dst_width  = DMA_BITSIZE_8BIT;
    g_dma_tx_cfg.transfer_size             = (uint16_t)len;
    g_dma_tx_cfg.circular_mode             = DMA_MODE_NORMAL;      /* 一次完成即停止 */
    g_dma_tx_cfg.priority                  = DMA_PRIORITY_MID;

    DMA_div_chx_Disable(DMA_TX_CH);
    DMA_div_TriggerSource_Clean();
    HAL_DMA_StructerInit(&g_dma_tx_cfg);

    /* UART 侧: 使能 TX FIFO + TX DMA 请求, 水位=1(空出一格请求一次DMA) */
    UART0->CR_STR.TXFE        = 1U;
    UART0->CR_STR.TDMAE       = 1U;
    UART0->WATER_STR.TXWATER  = 1U;

    DMA_div_chx_Enable(DMA_TX_CH);

    if (!uart_dma_wait_complete(DMA_TX_CH, 1000U))
    {
        DMA_div_chx_Disable(DMA_TX_CH);
        UART0->CR_STR.TDMAE = 0U;
        return false;
    }

    /* 先停通道再等 TX FIFO 排空(保证所有字节已移出发送) */
    DMA_div_chx_Disable(DMA_TX_CH);
    while (UART0->SR_STR.TXEMPT != 1U) {}

    /* 关闭 TX DMA, 恢复轮询发送路径 */
    UART0->CR_STR.TDMAE = 0U;
    return true;
}

/**
 *@brief UART0 DMA RX: UART0 DR 寄存器 -> 内存缓冲
 *@param[out] buf: 接收缓冲区
 *@param[in]  len: 期望接收长度
 *@return     true=接收完成, false=超时
 */
static bool uart_dma_rx_receive(uint8_t *buf, uint32_t len)
{
    /* 清空 RX FIFO 中可能残留的数据(上次超时/多发的字节), 避免计入本轮 */
    while (UART0->SR_STR.RXEMPT == 0U)
    {
        (void)UART0->DR_STR.DATA;
    }

    memset(&g_dma_rx_cfg, 0, sizeof(g_dma_rx_cfg));

    g_dma_rx_cfg.dma_ch                    = DMA_RX_CH;
    g_dma_rx_cfg.DMA_Trigger_periph        = DMA_Trigger_UART0_RX;
    g_dma_rx_cfg.src_addr                  = (uint32_t)&UART0->DR_STR;
    g_dma_rx_cfg.dst_addr                  = (uint32_t)buf;
    g_dma_rx_cfg.src_inc                   = Addr_Disable;         /* 外设地址固定 */
    g_dma_rx_cfg.dst_inc                   = Addr_Enable;          /* 内存地址自增 */
    g_dma_rx_cfg.dir                       = DMA_DIR_PERIPH_TO_MEM;
    g_dma_rx_cfg.burst_len                 = DMA_BURST_1_UNIT;
    g_dma_rx_cfg.dma_data_width_src_width  = DMA_BITSIZE_8BIT;
    g_dma_rx_cfg.dma_data_width_dst_width  = DMA_BITSIZE_8BIT;
    g_dma_rx_cfg.transfer_size             = (uint16_t)len;
    g_dma_rx_cfg.circular_mode             = DMA_MODE_NORMAL;      /* 一次完成即停止 */
    g_dma_rx_cfg.priority                  = DMA_PRIORITY_MID;

    DMA_div_chx_Disable(DMA_RX_CH);
    DMA_div_TriggerSource_Clean();
    HAL_DMA_StructerInit(&g_dma_rx_cfg);

    /* UART 侧: 使能 RX FIFO + RX DMA 请求, 水位=1(每收1字节请求一次DMA) */
    UART0->CR_STR.RXFE        = 1U;
    UART0->CR_STR.RDMAE       = 1U;
    UART0->WATER_STR.RXWATER  = 1U;

    DMA_div_chx_Enable(DMA_RX_CH);

    if (!uart_dma_wait_complete(DMA_RX_CH, 5000U))
    {
        DMA_div_chx_Disable(DMA_RX_CH);
        UART0->CR_STR.RDMAE = 0U;
        return false;
    }

    /* 关闭 RX DMA */
    DMA_div_chx_Disable(DMA_RX_CH);
    UART0->CR_STR.RDMAE = 0U;
    return true;
}


int main(void)
{
    delay_1us(200000);

    /* Add Clock Enable Code Here */
    flash_clk_enable();
    gpio_clk_enable();
    uart_clk_enable();

    /* Add peripheral initialization code here */
    uart0_gpio_init();
    gpio_output_init();

    (void)uart_init(UART_CONSOLE_INSTANCE, &uart0_init_config);
    uart_transmit_enable(UART_CONSOLE_INSTANCE, true);
    uart_receive_enable(UART_CONSOLE_INSTANCE, true);

    INFO("\n SDK start running...%s,%s,%s\n", FW_VER, COMPILER_DATE, COMPILER_TIME);
    INFO("uart_dma_demo: UART0 DMA demo (TX=CH0, RX=CH1)\n");
    __enable_irq();

    while (1)
    {
        uint32_t loop_len = g_rx_len_seq[g_rx_len_idx];
        uint32_t i;
        uint8_t c;

        /* 运行指示: PTC6/7/8 翻转 */
        gpio_output_set(GPIO_C, GPIO_PIN_6, GPIO_HIGH_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_7, GPIO_HIGH_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_8, GPIO_HIGH_LEVEL);

        /* ---- 提示上位机发送 loop_len 字节 ---- */
        INFO("\n=== Send %u bytes, chip will echo back ===\n", (unsigned)loop_len);

        /* ---- DMA RX: 等待接收 loop_len 字节 ----
         * 没有输入时会超时返回 false, 这里静默重新等待, 不刷屏也不换长度 */
        while (uart_dma_rx_receive(g_rx_buf, loop_len) == false)
        {
        }

        /* ---- 收到 loop_len 字节 ---- */
        /* INFO 只能发送 ASCII 字符, 原始字节(可能含 0x00/非 ASCII)不能直接打印:
         * 先按十六进制显示, 再给出可打印 ASCII 解读(不可打印字节用 '.' 代替) */
        INFO("Received %u bytes (HEX): ", (unsigned)loop_len);
        for (i = 0U; i < loop_len; i++)
        {
            INFO("%02X ", (unsigned int)g_rx_buf[i]);
        }
        INFO("\n");

        INFO("Received %u bytes (ASC): ", (unsigned)loop_len);
        for (i = 0U; i < loop_len; i++)
        {
            c = g_rx_buf[i];
            INFO("%c", (int)(((c >= 0x20U) && (c < 0x7FU)) ? (char)c : '.'));
        }
        INFO("\n");

        /* ---- DMA TX: 把收到的数据原样回显(原始二进制, 不经 INFO) ---- */
        if (uart_dma_tx_send(g_rx_buf, loop_len))
        {
            INFO("Echo done\n");
        }
        else
        {
            INFO("Echo timeout\n");
        }

        /* 接收成功, 切换到下一轮长度 */
        g_rx_len_idx++;
        if (g_rx_len_idx >= (uint8_t)(sizeof(g_rx_len_seq) / sizeof(g_rx_len_seq[0])))
        {
            g_rx_len_idx = 0U;
        }

        delay_1us(300000);   /* 0.3s */

        gpio_output_set(GPIO_C, GPIO_PIN_6, GPIO_LOW_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_7, GPIO_LOW_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_8, GPIO_LOW_LEVEL);
        delay_1us(300000);
    }
}
