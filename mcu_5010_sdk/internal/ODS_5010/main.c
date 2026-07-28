#include "platform_common.h"

#include "test.h"
#include "gpio_test.h"
//#include "device_resource.h"
#include "flash_hal.h"
#include "gpio_hal.h"
#include "mcpwm_hal.h"

#include "uart_reg.h"
#include "uart_hal.h"
//#include "clock_hal.h"
//#include "clock_hal_cfg.h"
#include "cmdline_internal.h"
//#include "rstgen_hal.h"
//#include "system_ARMCM0plus.h"

#include "ADC.h"
#include "user.h"
#include "adc.h"
//#include "ADC_test.h"
//#include "TIM_test.h"
//static UART_TypeDef * const g_uart_bases[UART_INSTANCE_MAX_COUNT] = UART_BASE_ADDRS;

#include "dma_hal.h"
#include "TIM_test.h"
#include "dma_test.h"
#include "i2c_test.h"
#include "dma.h"
#include "dma_cmdline.h"

#include "mcpwm_reg.h"
#include "TIM_hal.h"



uint16_t mcpwm0_callback_cnt = 0;
void MCPWM0_Call_Back(void)
{
	
	
	mcpwm0_callback_cnt ++;
	
	UartView(mcpwm0_callback_cnt+1000, mcpwm0_callback_cnt+2000, mcpwm0_callback_cnt+3000, mcpwm0_callback_cnt+4000);
	
//	if(mcpwm0_callback_cnt > 100)
//	{
//		mcpwm0_callback_cnt = 0;
//	
//		UART_TypeDef *base = g_uart_bases[0];
//	

//		base->DR_STR.DATA = 0x11;
//		
//		base->DR_STR.DATA = 0x22;
//		base->DR_STR.DATA = 0x33;
//		base->DR_STR.DATA = 0x44;
//		base->DR_STR.DATA = 0x55;
//		base->DR_STR.DATA = 0x66;
//		base->DR_STR.DATA = 0x77;
//		base->DR_STR.DATA = 0x88;
//		base->DR_STR.DATA = 0x99;
//		while(base->SR_STR.TXFULL){};
//		base->DR_STR.DATA = 0xaa;
//		while(base->SR_STR.TXFULL){};
//		base->DR_STR.DATA = 0xbb;
//		while(base->SR_STR.TXFULL){};
//		base->DR_STR.DATA = 0xcc;
//		while(base->SR_STR.TXFULL){};
//		base->DR_STR.DATA = 0xdd;
//		while(base->SR_STR.TXFULL){};
//		base->DR_STR.DATA = 0xee;
//		while(base->SR_STR.TXFULL){};
//		base->DR_STR.DATA = 0xff;
//		while(base->SR_STR.TXFULL){};
//		base->DR_STR.DATA = 0xa1;
//		while(base->SR_STR.TXFULL){};
//		base->DR_STR.DATA = 0xa2;
//		while(base->SR_STR.TXFULL){};
//		base->DR_STR.DATA = 0xa3;
//		while(base->SR_STR.TXFULL){};
//		base->DR_STR.DATA = 0xa4;
//		while(base->SR_STR.TXFULL){};
//		base->DR_STR.DATA = 0xa5;
//		while(base->SR_STR.TXFULL){};
//		base->DR_STR.DATA = 0xa6;
//		
//		
//	}

//	if(mcpwm0_callback_cnt > 100)
//	{
//		mcpwm0_callback_cnt = 0;
//	
//		UART_TypeDef *base = g_uart_bases[0];
//		
//		while(base->SR_STR.TXEMPT == 0U){};
//		base->DR_STR.DATA = 0x11;
//		while(base->SR_STR.TXEMPT == 0U){};
//		base->DR_STR.DATA = 0x22;
//		while(base->SR_STR.TXEMPT == 0U){};
//		base->DR_STR.DATA = 0x33;
//		while(base->SR_STR.TXEMPT == 0U){};
//		base->DR_STR.DATA = 0x44;
//		while(base->SR_STR.TXEMPT == 0U){};
//		base->DR_STR.DATA = 0x55;
//		while(base->SR_STR.TXEMPT == 0U){};
//		base->DR_STR.DATA = 0x66;
//		while(base->SR_STR.TXEMPT == 0U){};
//		base->DR_STR.DATA = 0x77;
//	}
	
}

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
 *@brief configure the gpio of UART1 
 *@param[in] none 
 *@return    none
 */
static void uart1_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;

	  /* UART1_RX, PTA3  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_3;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* UART1_TX, PTA4  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_4;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
}


/**
 *@brief configure the gpio of general gpio
 *@param[in] none 
 *@return    none
 */
static void group1_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;

//	/* GPIO, PTA2 PIN  */
//    setting_cfg.gpio_port = GPIO_A;
//    setting_cfg.gpio_pin = GPIO_PIN_2;
//    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
//    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
//    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
//   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//    gpio_init(&setting_cfg);


//	/* GPIO, PTA3 PIN  */
//    setting_cfg.gpio_port = GPIO_A;
//    setting_cfg.gpio_pin = GPIO_PIN_3;
//    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
//    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
//    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
//   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//    gpio_init(&setting_cfg);

	// /* GPIO, PTB0 PIN  */
    // setting_cfg.gpio_port = GPIO_B;
    // setting_cfg.gpio_pin = GPIO_PIN_0;
    // setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    // setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    // setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    // setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    //setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    // gpio_init(&setting_cfg);


	// /* GPIO, PTB1 PIN  */
    // setting_cfg.gpio_port = GPIO_B;
    // setting_cfg.gpio_pin = GPIO_PIN_1;
    // setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    // setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    // setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    // setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    //setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    // gpio_init(&setting_cfg);


//	/* GPIO, PTC0 PIN  */
//    setting_cfg.gpio_port = GPIO_C;
//    setting_cfg.gpio_pin = GPIO_PIN_0;
//    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
//    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
//    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
//   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//    gpio_init(&setting_cfg);


//	/* GPIO, PTC1 PIN  */
//    setting_cfg.gpio_port = GPIO_C;
//    setting_cfg.gpio_pin = GPIO_PIN_1;
//    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
//    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
//    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
//   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//    gpio_init(&setting_cfg);

	/* GPIO, PTC6 PIN  */
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_6;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
   setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
	
	/* GPIO, PTC7 PIN  */
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_7;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
	
	
	/* GPIO, PTC8 PIN  */
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_8;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
	
	
	/* GPIO, PTC9 PIN  */
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_9;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
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
 *@brief init flash controller to default value
 *@param[in] none
 *@return    none
 */
//static void flash_default_init(void)
//{
//	
//	
//}

static void rram_gpio_bypass(void)
{
    uint32_t val = 0x00796783;
    flash_erc_e ret = ERC_FLASH_NO_ERROR;
    if(REG32(0x4002041c) == val)
    {
        INFO("rram gpio bypass success \n");
        return;
    }
    else
    {
        ret = flash_write(OTP_FLASH, 0x27, &val, 1);
			  if(ret == ERC_FLASH_NO_ERROR)
				{
					INFO("rram gpio bypass reboot start \n");
					delay_1us(100);
					//RCC->RSTKEY.w 
					REG32(0x4002000C) = 0x0000AB56; //reboot
				}
				else
				{
					INFO("rram gpio bypass failed \n");
				}
        return;
    }
}

/**
 *@brief the main function of the demo project
 *@param[in] none
 *@return    none
 */

u8 ADC_TestFlag;
#include "spi_cmdline.h"

int main(void)
{
	delay_1us(2000);
  flash_clk_enable();
	 
	/*System Init*/
	System_Init();
	
	/* Add Clock Enable Code Here */
	gpio_clk_enable();
	
	mcpwm_clk_enable();

    uart_clk_enable();
	
	HWMATH_Clk_Enable();
	 
	ADC_Clk_Enable();
	 
	TIM13_Clk_Enable();
    TIM14_Clk_Enable();
    TIM15_Clk_Enable();
    TIM16_Clk_Enable();

	delay_1us(10);
	

//	gpio_toggle(GPIO_A,GPIO_PIN_0);
//	
//	flash_default_init();
//	
//	/*init system clock at first*/
//	sys_clk_init(&g_sys_clk_config);

//	flash_clk_init();
	
	/* Add Gpio Init Code Here */
	//group1_gpio_init();
	
	uart1_gpio_init();
	
	(void)uart_init(UART_INSTANCE1, &uart1_init_config);
    
	uart_transmit_enable(UART_INSTANCE1, true);
	
	uart_receive_enable(UART_INSTANCE1,false); 
		
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

  rram_gpio_bypass();

#ifdef CMD_MODULE_TEST
	module_test();
#endif

	__enable_irq();						  		/* 使能全部中断 */
	//__disable_irq();					    /* 使能全部中断 */
	
 
	
	while(1)
	{	
		//cmdline_handle();
		cmdline_handle_polling();
		
		 
		/* 每10us 翻转一次IO口电平 */
		//gpio_output_set(GPIO_C,GPIO_PIN_6,GPIO_HIGH_LEVEL);
		//gpio_output_set(GPIO_C,GPIO_PIN_7,GPIO_HIGH_LEVEL);
		//gpio_output_set(GPIO_C,GPIO_PIN_8,GPIO_HIGH_LEVEL);
		
		//delay_1us(5000);
		//gpio_output_set(GPIO_C,GPIO_PIN_6,GPIO_LOW_LEVEL);
		//gpio_output_set(GPIO_C,GPIO_PIN_7,GPIO_LOW_LEVEL);
		//gpio_output_set(GPIO_C,GPIO_PIN_8,GPIO_LOW_LEVEL);
        //group_gpio_output_set(GPIO_A,GPIO_HIGH_LEVEL);
		delay_1us(10);
    //group_gpio_output_set(GPIO_A,GPIO_LOW_LEVEL);
    //delay_1us(10);
		
	}
}
