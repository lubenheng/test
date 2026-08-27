/****************************************************************************/
/** \file main.c
**
**  HIRC demo for ODS_5010 (Cortex-M0+)
**  演示高频内部RC时钟(HIRC, 驱动层即HSI):
**   1. 使能 HIRC 并等待就绪
**   2. MCO 将 HIRC 引出到 PTA5, 示波器可直接测量HIRC频率
**   3. 系统时钟源显式切换为 HIRC(HSI) ，MCO 将 系统时钟 引出到 PTA5 也可以测量HIRC频率（系统时钟默认配置情况下）
**   4. PTC6/7/8 翻转作为运行指示
*****************************************************************************/
#include "platform_common.h"
#include "test.h"
#include "gpio_hal.h"
#include "clock.h"
#include "clock_hal.h"
#include "uart_hal.h"
#include "uart_hal_cfg.h"

#include "cmdline_internal.h"


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
 *@brief configure PTA5 as MCO output, 示波器在此引脚直接测量 HIRC(HSI) 频率
 *@param[in] none
 *@return    none
 */
static void mco_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;

    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_5;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_1;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
}

/**
 *@brief configure PTC6/7/8 as push-pull outputs
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


int main(void)
{
    delay_1us(200000);

	/* Add Clock Enable Code Here */
	flash_clk_enable();
	gpio_clk_enable();
	uart_clk_enable();

	/* Add peripheral initialization code here */
	uart0_gpio_init();
	mco_gpio_init();
	gpio_output_init();

	/* 使能 HIRC(驱动层即 HSI) 并等待就绪, 校验返回码 */
	clk_erc_e erc = rcc_hal_osc_enable(CLOCK_OSC_HSI, 1000U);

	(void)uart_init(UART_CONSOLE_INSTANCE, &uart0_init_config);
	uart_transmit_enable(UART_CONSOLE_INSTANCE, true);
	cmdline_init();

	INFO("HIRC(HSI) enable: %s\n", (erc == ERC_CLK_NO_ERROR) ? "OK" : "FAIL");
	INFO("\n SDK start running...%s,%s,%s\n", FW_VER, COMPILER_DATE, COMPILER_TIME);
	__enable_irq();

	/* MCO 将 HIRC 引出到 PTA5, 示波器测量对照 */
	rcc_hal_mco_clock_select(CLOCK_MCO_HSI);    //MCO输出HIRC时钟
	rcc_hal_mco_div(CLOCK_MCO_DIV1);
	INFO("MCO(HIRC) freq = %dHz\n", rcc_hal_get_mco_freq());

	/* 系统时钟源显式切换为 HIRC(HSI), 内部会重初始化 UART 保证波特率正确
    （这里默认的波特率为115200，建议将波特率改为9600） */
	sys_clk_config_t sys_clk_sel = {
		.sys_clk_sel = CLOCK_SYS_HSI,
		.fclk_div    = CLOCK_SYS_FCLK_DIV1,
		.pclk_div    = CLOCK_SYS_PCLK_DIV1,
		.pll_mult    = 0,
		.pll_div     = 0,
	};
	rcc_hal_sys_change_clock(&sys_clk_sel);

	uint32_t mcu_clk = 0U;
	(void)clock_freq_get(SCG_SYS, &mcu_clk);
	INFO("MCU_CLK(HIRC) = %dMHz\n", mcu_clk/1000000U);

    while(1)
    {
		cmdline_handle_polling();
        gpio_output_set(GPIO_C, GPIO_PIN_6, GPIO_HIGH_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_7, GPIO_HIGH_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_8, GPIO_HIGH_LEVEL);
        delay_1us(500000);

        gpio_output_set(GPIO_C, GPIO_PIN_6, GPIO_LOW_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_7, GPIO_LOW_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_8, GPIO_LOW_LEVEL);
        delay_1us(500000);
    }
}
