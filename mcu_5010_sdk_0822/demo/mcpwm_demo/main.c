/****************************************************************************/
/** \file main.c
**
**  MCPWM demo for ODS_5010 (Cortex-M0+)
**  输出中心对齐 PWM，频率与占空比均可调:
**    - 先输出一组编译期可调的稳定配置(便于示波器精确测量)
**    - 随后主循环运行时扫动: 占空比 10%~90% 往复, 每扫到 90% 切换一次频率
**  输出引脚: CH0P=PTA6, CH1P=PTA7, CH2P=PTA8 (复用功能15)
**            互补 N 相 CH0N/1N/2N = PTA9/10/11, 本 demo 未使用
*****************************************************************************/
#include "platform_common.h"
#include "test.h"
#include "gpio_hal.h"
#include "clock.h"
#include "clock_hal.h"
#include "uart_hal.h"
#include "uart_hal_cfg.h"

#include "mcpwm_reg.h"   /* MCPWM0_TH0 / MCPWM0_TH00 等寄存器宏 */
#include "mcpwm_hal.h"   /* MCPWM_ch012_init / EPWM0_OutPut */
#include "mcpwm.h"       /* MCU_CLK 等配置宏 */

#include "cmdline_internal.h"

/* ---------- 编译期可调参数 ---------- */
#define DEMO_PWM_FREQ_HZ      10000U   /* 初始 PWM 频率, 建议 1k~100kHz */
#define DEMO_PWM_DUTY_PERCENT 50U      /* 初始占空比, 0~100 */

/* 运行时频率表: 主循环扫到 90% 后切换到下一档, 演示频率可调 */
static const uint32_t s_freq_table[] = {1000U, 5000U, 10000U, 20000U};
#define FREQ_TABLE_NUM (sizeof(s_freq_table) / sizeof(s_freq_table[0]))


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
 *@brief configure PTA6/7/8 as MCPWM CH0P/CH1P/CH2P output (复用功能15)
 *@param[in] none
 *@return    none
 */
static void mcpwm_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;

    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_15;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;

    setting_cfg.gpio_pin = GPIO_PIN_6;   /* CH0P */
    gpio_init(&setting_cfg);
    setting_cfg.gpio_pin = GPIO_PIN_7;   /* CH1P */
    gpio_init(&setting_cfg);
    setting_cfg.gpio_pin = GPIO_PIN_8;   /* CH2P */
    gpio_init(&setting_cfg);
}

/**
 *@brief 设置 MCPWM 时基0 PWM 频率
 *@param[in] freq_hz: PWM 频率(Hz)
 *@return    实际频率(Hz)
 *@note  中心对齐模式: 计数周期 2*TH0, TH0 = (MCLK/2)/freq; 频率过低 TH0 溢出时自动 8 分频
 */
static uint32_t mcpwm_freq_set(uint32_t freq_hz)
{
    uint32_t period_cnt = (MCU_CLK >> 1) / freq_hz;

    MCPWM0_PRT = 0x0000DEAD;               /* 解锁 MCPWM 写保护 */
    if (period_cnt > 65535U)
    {
        MCPWM0_TCLK |= (BIT0 + BIT1);      /* MCPWM 时钟 8 分频 */
        period_cnt >>= 3;
    }
    else
    {
        MCPWM0_TCLK &= ~(BIT0 + BIT1);
    }
    MCPWM0_TH0 = period_cnt;               /* 时基0周期门限值 = PWM 周期 */
    MCPWM0_PRT = 0x0000CAFE;               /* 上锁写保护 */

    uint32_t actual = (MCU_CLK >> 1) / period_cnt;
    if ((MCPWM0_TCLK & (BIT0 + BIT1)) != 0U) { actual >>= 3; }
    return actual;
}

/**
 *@brief 设置 CH0/CH1/CH2 占空比 (三通道同步)
 *@param[in] duty_pct: 占空比, 0~100
 *@note  中心对齐互补输出: N 相比较值取负、P 相取正,
 *       与 MCPWM_ch012_init 默认 ±TH0/2(50%) 的写法一致
 */
static void mcpwm_duty_set(uint32_t duty_pct)
{
    uint32_t th0 = MCPWM0_TH0;
    uint32_t cmp = (duty_pct * 32768U / 100U) * th0 >> 15;  /* 0~100% -> 0~TH0 */
    if (cmp > th0) { cmp = th0; }
    int32_t d = (int32_t)cmp;

    MCPWM0_PRT = 0x0000DEAD;               /* 解锁 MCPWM 写保护 */
    MCPWM0_TH00 = (uint16_t)(-d);          /* CH0N 比较值 */
    MCPWM0_TH01 = (uint16_t)( d);          /* CH0P 比较值 */
    MCPWM0_TH10 = (uint16_t)(-d);          /* CH1N 比较值 */
    MCPWM0_TH11 = (uint16_t)( d);          /* CH1P 比较值 */
    MCPWM0_TH20 = (uint16_t)(-d);          /* CH2N 比较值 */
    MCPWM0_TH21 = (uint16_t)( d);          /* CH2P 比较值 */
    MCPWM0_PRT = 0x0000CAFE;               /* 上锁写保护 */
}


int main(void)
{
    delay_1us(200000);

	/* Add Clock Enable Code Here */
	flash_clk_enable();
	gpio_clk_enable();
	uart_clk_enable();
	mcpwm_clk_enable();

	/* Add peripheral initialization code here */
	uart0_gpio_init();
	mcpwm_gpio_init();

	(void)uart_init(UART_CONSOLE_INSTANCE, &uart0_init_config);
	uart_transmit_enable(UART_CONSOLE_INSTANCE, true);
	cmdline_init();

	INFO("\n SDK start running...%s,%s,%s\n", FW_VER, COMPILER_DATE, COMPILER_TIME);
	__enable_irq();

	/* MCPWM 时基0 三通道中心对齐互补 PWM 初始化(默认10kHz/50%),
	   内部已使能 MOE 输出与计数器 */
	MCPWM_ch012_init();
	EPWM0_OutPut(ENABLE);

	/* 先输出一组编译期配置的稳定 PWM, 便于示波器精确测量 */
	uint32_t freq = mcpwm_freq_set(DEMO_PWM_FREQ_HZ);
	mcpwm_duty_set(DEMO_PWM_DUTY_PERCENT);
	INFO("MCPWM CH0P/CH1P/CH2P on PTA6/PTA7/PTA8, freq = %dHz, duty = %d%%\n",
	     (int)freq, (int)DEMO_PWM_DUTY_PERCENT);

	delay_1us(3000000);   /* 3s, 期间示波器先测量到稳定的初始配置 */

	/* 运行时演示: 占空比 10%~90% 往复扫动, 每扫到 90% 切换一次频率 */
	uint32_t freq_idx = 0U;
	int32_t  duty     = 10;
	int32_t  dir      = 1;

	while (1)
	{
		cmdline_handle_polling();
		mcpwm_duty_set((uint32_t)duty);
		INFO("freq = %dHz, duty = %d%%\n", (int)freq, (int)duty);
		delay_1us(200000);   /* 0.2s */

		duty += dir * 10;
		if (duty >= 90)
		{
			dir = -1;
			freq_idx = (freq_idx + 1U) % FREQ_TABLE_NUM;
			freq = mcpwm_freq_set(s_freq_table[freq_idx]);
		}
		else if (duty <= 10)
		{
			dir = 1;
		}
	}
}
