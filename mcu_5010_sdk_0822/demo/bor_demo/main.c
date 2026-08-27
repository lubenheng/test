/****************************************************************************/
/** \file main.c
**
**  BOR (Brown-Out Reset / 掉电复位) demo for ODS_5010 (Cortex-M0+)
**
**  功能：
**    1. 上电时读取 RCC->SYSRSTSR，判断上次复位是否由 BOR(掉电复位) 引起，
**       打印后清除复位标志，避免污染下次判断；
**    2. 使能并配置 ANAC 模块的 VDD33 掉电复位检测
**       (VDD33_BOR_EN / RSTN_BOR33_EN / ANA_NG_EN / 阈值 / 故障动作 / 锁存)；
**    3. 主循环轮询 ANAC_RO.VDD33_BOR_LAT 掉电锁存状态与 ANAC_IR 中断标志。
**
**  验证方法：
**    用可调电源给 VDD33 供电，将电压从 3.3V 向下调、越过所设阈值后芯片应立即复位；
**    重新上电后串口会打印 "上次复位由掉电复位(Brown-Out Reset)引起"。
**
**  注意：BOR 是复位型保护，软件无法自触发真正的掉电，必须外部拉低 VDD33 触发。
*****************************************************************************/
#include "platform_common.h"
#include "test.h"
#include "gpio_hal.h"
#include "clock.h"
#include "uart_hal.h"
#include "uart_hal_cfg.h"
#include "test.h"

#include "cmdline_internal.h"

#include "anac_reg.h"        /* ANAC_TypeDef: BOR/PVD 寄存器定义 */
#include "clock_type.h"      /* CLOCK_RST_FLAG_* 复位来源标志 */


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
 *@brief enable the clock of UART0
 *@param[in] none
 *@return    none
 */
static void uart0_clk_init(void)
{
//	module_clk_cfg_t module_clk_cfg;
//
//	module_clk_cfg.module_name = MODULE_UART0;
//	module_clk_cfg.fclk_sel = APB_CLK;
//	module_clk_cfg.fclk_div = PER_DIVIDE_1;
//	module_clk_cfg.pclk_dsleep_gating_en = GATING_ON;
//	module_clk_cfg.fclk_dsleep_gating_en = GATING_ON;
//
//	(void)clock_module_clk_cfg(&module_clk_cfg);
}

/**
 *@brief configure GPIO_C pin 6/7/8 as push-pull outputs
 *@param[in] none
 *@return    none
 */
static void gpio_demo_pins_init(void)
{
    gpio_settings_config_t setting_cfg;

		//PTC6
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_6;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

		//PTC7
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_7;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

		//PTC8
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
 *@brief 读取上次复位来源，判断是否由 BOR 引起，打印后清除复位标志
 *@param[in] none
 *@return    none
 *@note  SYSRSTSR 为 rw1c，写 1 清除对应标志
 */
static void bor_show_last_reset(void)
{
    uint32_t rst_st = rcc_drv_get_reset_status(RCC);

    INFO("[BOR] SYSRSTSR = 0x%08x\n", rst_st);

    if (rst_st & CLOCK_RST_FLAG_BOR)
    {
        INFO("[BOR] 上次复位由掉电复位(Brown-Out Reset)引起\n");
    }
    if (rst_st & CLOCK_RST_FLAG_POR)
    {
        INFO("[BOR] 上次复位由上电复位(POR)引起\n");
    }
    if (rst_st & CLOCK_RST_FLAG_LVD)
    {
        INFO("[BOR] 上次复位由低压检测(LVD)引起\n");
    }
    if (rst_st & CLOCK_RST_FLAG_PIN)
    {
        INFO("[BOR] 上次复位由 NRST 引脚复位引起\n");
    }

    /* 清除复位标志，避免污染下次复位来源判断 */
    rcc_drv_clear_reset_flag(RCC, CLOCK_RST_FLAG_ALL);
}

/**
 *@brief 使能并配置 VDD33 掉电复位(BOR)检测
 *@param[in] none
 *@return    none
 *@note  具体阈值电压档位请查 ANAC 数据手册；阈值不要设得离 3.3V 太近，避免正常波动误复位
 */
static void bor_init(void)
{
    ANAC_TypeDef *anac = ANA_CONTROL;

    /* 1. 模拟模块总使能 + VDD33 掉电复位使能 */
    anac->ANAC_ENCR.ANA_EN        = 1U;   /* bit0  模拟模块总使能 */
    anac->ANAC_ENCR.VDD33_BOR_EN  = 1U;   /* bit7  VDD33 掉电复位使能 */
    anac->ANAC_ENCR.RSTN_BOR33_EN = 1U;   /* bit13 VDD33 掉电复位输出(RSTN)使能 */
    anac->ANAC_ENCR.ANA_NG_EN     = 1U;   /* bit22 模拟故障检测总使能 */

    /* 2. 配置 BOR 阈值与故障动作 */
    anac->ANAC_PVD.VDD33_BOR_VTH   = 0x4U;   /* bit8-10  阈值，000最低 ~ 111最高 */
    anac->ANAC_PVD.VDD33_BOR_NG_EN = 1U;     /* bit11    掉电故障检测使能 */
    anac->ANAC_PVD.VDD33_BOR_NG    = 0x3U;   /* bit12-13 故障配置：11=触发复位 */

    /* 3. 清除历史锁存状态 */
    anac->ANAC_PVD.VDD33_BOR_LAT_CLR = 1U;
    anac->ANAC_PVD.VDD33_BOR_LAT_CLR = 0U;

    INFO("[BOR] BOR 已使能, VDD33_BOR_VTH = %u\n", anac->ANAC_PVD.VDD33_BOR_VTH);
}

/**
 *@brief 轮询 BOR 掉电锁存状态与中断标志
 *@param[in] none
 *@return    none
 *@note  VDD33_BOR_LAT 为只读锁存，置位后需软件写 VDD33_BOR_LAT_CLR 清除
 */
static void bor_monitor(void)
{
    ANAC_TypeDef *anac = ANA_CONTROL;

    /* 锁存状态：VDD33 是否发生过掉电 */
    if (anac->ANAC_RO.VDD33_BOR_LAT)
    {
        INFO("[BOR] 检测到 VDD33 掉电 (VDD33_BOR_LAT=1)\n");
        anac->ANAC_PVD.VDD33_BOR_LAT_CLR = 1U;   /* 清锁存，继续观察 */
        anac->ANAC_PVD.VDD33_BOR_LAT_CLR = 0U;
    }

    /* 中断标志：读 1 清除 */
    if (anac->ANAC_IR.IR_VDD33_BOR_NG)
    {
        INFO("[BOR] BOR 故障中断标志置位 (IR_VDD33_BOR_NG)\n");
        (void)anac->ANAC_IR.w;   /* 读取清除中断标志 */
    }
}



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

#ifdef CMDLINE_SUPPORT
	cmdline_init();
#endif /*CMDLINE_SUPPORT*/


	INFO("\n SDK start running...%s,%s,%s\n", FW_VER, COMPILER_DATE, COMPILER_TIME);

	uint32_t mcu_clk = 0U;
	(void)clock_freq_get(SCG_SYS, &mcu_clk);
	INFO("MCU_CLK = %dMHz\n", mcu_clk/1000000U);
#endif /*UART*/

 //ram_gpio_bypass();
	gpio_demo_pins_init();

	/* ---- BOR demo ---- */
	bor_show_last_reset();    /* 1. 判断上次复位来源并清标志 */
	bor_init();               /* 2. 使能并配置 BOR */

#ifdef CMD_MODULE_TEST
	module_test();
#endif

	__enable_irq();						  		/* 使能全部中断 */
	//__disable_irq();					    /* 使能全部中断 */


    while(1)
    {
				cmdline_handle_polling();
				bor_monitor();                /* 3. 轮询掉电锁存/中断标志 */

        gpio_output_set(GPIO_C, GPIO_PIN_6, GPIO_HIGH_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_7, GPIO_HIGH_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_8, GPIO_HIGH_LEVEL);
        delay_1us(500);

        gpio_output_set(GPIO_C, GPIO_PIN_6, GPIO_LOW_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_7, GPIO_LOW_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_8, GPIO_LOW_LEVEL);
        delay_1us(500);
    }
}
