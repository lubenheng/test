/****************************************************************************/
/** \file main.c
**
**  GPIO demo for ODS_5010 (Cortex-M0+)
**  PA0-PA15,PB0-PB15,PC0-PC8
*****************************************************************************/
#include "platform_common.h"
#include "test.h"
#include "gpio_hal.h"
#include "clock.h"
#include "clock_hal.h"
#include "uart_hal.h"
#include "uart_hal_cfg.h"
#include "test.h"

#include "cmdline_internal.h"

/* PA5复用为MCO引脚时可以接入的时钟源有：
      系统时钟频率、HSI频率、PLLCLK频率、HSE频率、LSI频率 
   可以将示波器接入MCO引脚查看输出的时钟频率*/

/* 系统时钟可接入频率有：HSI、HSE、PLLCLK */

/* 这里需要说明一下，系统时钟的FCLK分频和PCLK分频无法通过MCO引脚查看，只能通过延时函数查看 */

/* 所以以下MCO轮流切换时钟源展示，
  然后切换MCO的时钟源为系统时钟，系统时钟的时钟源轮流切换为HSI、HSE、PLLCLK，通过MCO引脚查看
  通过延时函数查看FCLK和PCLK的分频情况
*/

//MCO时钟切换不会对程序造成影响，只会输出选择的时钟源
clock_mco_select mco_clk_sel = CLOCK_MCO_SYS;  //MCO引脚时钟源选择
clock_mco_div mco_clk_div = CLOCK_MCO_DIV1;  //MCO引脚时钟分频选择1 - 256

//系统时钟配置会影响所有外设时钟，包括延时函数等，所以外设配置之前必须要配置好时钟
//外设配置之后必须切换时钟那根据配置之后的时钟频率做适配，最好重新初始化一下
//以下是UART已经配置好的情况下切换时钟，每次切换时钟都初始化UART
sys_clk_config_t sys_clk_sel_s = {
  .sys_clk_sel = CLOCK_SYS_PLLCLK1,  //系统时钟源选择，注意，如果需要切到HSE,需要外接晶振
  .fclk_div = CLOCK_SYS_FCLK_DIV1,  //系统时钟的FCLK分频
  .pclk_div = CLOCK_SYS_PCLK_DIV1,  //系统时钟的PCLK分频
  .pll_mult = 1,  //PLL倍频，系统时钟源为PLLCLK时有效
  .pll_div = 2,   //PLL分频，系统时钟源为PLLCLK时有效
};   //系统时钟配置结构体


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
    setting_cfg.gpio_pin = GPIO_PIN_13;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* UART0_TX, PTA2  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_14;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
}
/**
 *@brief configure the clock
 *@param[in] none
 *@return    none
 */
static void clock_mco_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;
		
    //PTA5
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_5;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_1;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

}

static void gpio_output_init(void)
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
    gpio_output_set(GPIO_C, GPIO_PIN_6, GPIO_LOW_LEVEL);

}



int main(void)
{
    delay_1us(200000);
	 
	  /* Add Clock Enable Code Here */
    flash_clk_enable();
	  gpio_clk_enable();
    uart_clk_enable();
    RCC_Init();
    /* Add peripheral initialization code here */
	  uart0_gpio_init();
    clock_mco_gpio_init();
    gpio_output_init();
	
	
    rcc_hal_osc_enable_all();		//使能所有时钟源
	

	  (void)uart_init(UART_CONSOLE_INSTANCE, &uart0_init_config);
	  uart_transmit_enable(UART_CONSOLE_INSTANCE, true);
    cmdline_init();
		
	  INFO("\n SDK start running...%s,%s,%s\n", FW_VER, COMPILER_DATE, COMPILER_TIME);


	  __enable_irq();						  		/* 使能全部中断 */
	  //__disable_irq();					    /* 使能全部中断 */

    rcc_hal_mco_clock_select(mco_clk_sel);		//MCO引脚时钟源选择
    //rcc_hal_mco_div(mco_clk_div);             //MCO引脚时钟分频选择

    uint32_t mcu_clk = 0U;
	  (void)clock_freq_get(SCG_SYS, &mcu_clk);
	  INFO("MCU_CLK = %dMHz\n", mcu_clk/1000000U);
	  
	//rcc_hal_sys_change_clock(&sys_clk_sel_s);	//系统时钟源选择

 
    while(1)
    {
				cmdline_handle_polling();
        gpio_output_set(GPIO_C, GPIO_PIN_6, GPIO_HIGH_LEVEL);
        delay_1us(1000000);

        gpio_output_set(GPIO_C, GPIO_PIN_6, GPIO_LOW_LEVEL);
        delay_1us(1000000);
    }
}
