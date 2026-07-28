/**************************************************************************//**
 * @file     test.c
 * @brief    Firmware driver for the atuo test
 *
 * @note
 * Copyright (C) Silergy Corp.
 *
 ******************************************************************************/
#include "platform_common.h"

#include "test.h"

#include "cmdline_internal.h"

#include "dma_test.h"
#include "dma_cmdline.h"
#include "i2c_test.h"

extern uint16_t MCPWM_Tx_buff[MCPWM_LEN];
extern uint16_t MCPWM_Rx_buff[MCPWM_LEN]; 
extern uint32_t TIMER_DMA_Tx_buff[TIMER_LEN];
extern  uint16_t tx_buf[bufsize];
#ifdef UART_TEST
#include "uart_test.h"
#endif


#ifdef GPIO_TEST
//#include "gpio_hal.h"
#include "gpio_test.h"
#endif


#ifdef FLASH_TEST
#include "flash_test.h"
#endif


#ifdef ADC_TEST
#include "adc.h"
#include "adc_test.h"	
#endif

#ifdef CSA_TEST
#include "CSA.h"
#include "CSA_test.h"	
#endif

#ifdef HWMATH_TEST
#include "HwMath_test.h"
#endif


#ifdef MCPWM_TEST
#include "mcpwm_test.h"
#include "mcpwm_hal.h"
#endif

#ifdef WDOG_TEST
#include "wdog_test.h"
#endif

#ifdef HALL_TEST
#include "hall_test.h"
#include "hall_hal.h"
#endif

#ifdef QEP_TEST
#include "qep_test.h"
#include "qep_hal.h"
#endif

#ifdef RTC_TEST
#include "rtc_test.h"
#endif

#ifdef DMA_TEST
#include "dma_test.h"
#endif

#ifdef CLOCK_TEST
#include "clock_test.h"

/*临时引入*/
#include "gpio_hal.h"

#endif


#ifdef TIM_TEST
#include "TIM_test.h"
#endif

#ifdef SPI_TEST
#include "spi_test.h"
#endif

#ifdef I2C_TEST
#include "i2c_test.h"
#endif

#ifdef CAN_TEST
#include "can.h"
#include "can_test.h"
#endif

#ifdef PMU_TEST
#include "pmu_test.h"
#endif

#ifdef SRAM_TEST

#define SRAM_TEST_LENGTH_0 36
#define SRAM_TEST_LENGTH_1 500
volatile uint32_t g_sram_test_buff0[SRAM_TEST_LENGTH_0];
volatile uint32_t g_sram_test_buff1[SRAM_TEST_LENGTH_1];
volatile uint32_t g_sram_test_buff2[SRAM_TEST_LENGTH_1];
volatile uint32_t g_sram_test_buff3[SRAM_TEST_LENGTH_1];
volatile uint32_t g_sram_test_buff4[SRAM_TEST_LENGTH_1];
volatile uint32_t g_sram_test_buff5[SRAM_TEST_LENGTH_1];
volatile uint32_t g_sram_test_buff6[SRAM_TEST_LENGTH_1];
volatile uint32_t g_sram_test_buff7[SRAM_TEST_LENGTH_1];


#endif

#ifdef TIM_TEST

	/* ADD Code Here */
	
	/*1.TIM初始化****************************/

	
	//计数器测试函数初始化
	//TIM_test_Counter_init();

	//输入捕获测试函数初始化
	//TIM_test_Capture_init();
	
	//输出比较(PWM)测试函数初始化
	//TIM_Compare_test_init();
	
	
	/*2.模块初始化****************************/

	
	/*3.中断使能与优先级设置*******************/
	
	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/

	
#endif	



#ifdef INFO_TEST
#include "INFO_test.h"
#endif

#ifdef MODULE_TEST
#include "module_test.h"
#endif

void module_test(void)
{
#ifdef MCPWM_TEST
	
	/*1.GPIO初始化***************************/
	mcpwm0_gpio_init();
	mcpwm1_gpio_init();
	
	/*2.模块初始化***************************/
	MCPWM_ch012_init();
	MCPWM_ch345_init();
	
	/*3.中断使能与优先级设置***************************/
	NVIC_SetPriority(MCPWM0_IRQn, 2); /*TIMER0优先级配置 */
	NVIC_ClearPendingIRQ(MCPWM0_IRQn);
	NVIC_EnableIRQ (MCPWM0_IRQn);
	
	NVIC_SetPriority(MCPWM1_IRQn, 2); /*TIMER1优先级配置 */
	NVIC_ClearPendingIRQ(MCPWM1_IRQn);
	NVIC_EnableIRQ (MCPWM1_IRQn);
	
	
	/*4.软件相关配置***************************/
	mcpwm_soft_init_copy();
	
    /*5.注册回调函数***************************/
	
  	g_mcpwm0_int_callback = mcpwm0_callback_duty_test;
	//g_mcpwm1_int_callback = mcpwm1_callback_duty_test;
	/*5.中断源扫描测试***************************/
	//g_mcpwm0_int_callback = mcpwm0_interrupt_test_callback;
	//g_mcpwm1_int_callback = mcpwm1_interrupt_test_callback;
	
#endif	


#ifdef PMU_TEST

	/* ADD Code Here */

	/*1.寻找是否是低功耗唤醒****************************/

	__enable_irq();	
	NVIC_SetPriority(PMU_IRQn, 2); /*TIMER0优先级配置 */
	NVIC_ClearPendingIRQ(PMU_IRQn);
	NVIC_EnableIRQ (PMU_IRQn);
	pmu_wakeup_interrupt_find();

	//__disable_irq();	
	
	/*2.模块初始化****************************/

	
	/*3.中断使能与优先级设置*******************/
	
	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/

	
#endif	

#ifdef GPIO_TEST

	/* ADD Code Here */
	
	/*1.GPIO初始化****************************/
	hall_uvw_gpio_init();

	
	/*2.模块初始化****************************/

	
	/*3.中断使能与优先级设置*******************/
	
	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/
#endif		


#ifdef HALL_TEST

	/* ADD Code Here */
	
	/*1.HALL初始化****************************/
	hall_gpio_init();
	hall_debug_gpio_init();
	hall_clk_enable();
	
	/*2.模块初始化****************************/
	HALL_Init();
	
	/*3.中断使能与优先级设置*******************/
	NVIC_SetPriority(HALL_IRQn, 0); /* HALL_IRQn 优先级配置 */
	NVIC_ClearPendingIRQ(HALL_IRQn);
	NVIC_EnableIRQ (HALL_IRQn);
	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/
	g_hall_int_callback = g_motor_ctrl_hall_callback;

	
#endif	


#ifdef QEP_TEST
	/* ADD Code Here */
	/*1.QEP初始化****************************/
	qep_gpio_init();
	qep_clk_enable();
	
	/*2.模块初始化****************************/
	QEP_Init();
	
	/*3.中断使能与优先级设置*******************/
	NVIC_SetPriority(QEP_IRQn, 2); 		/* QEP_IRQn 优先级配置 */
	NVIC_ClearPendingIRQ(QEP_IRQn);
	NVIC_EnableIRQ (QEP_IRQn);
	/*4.软件相关配置**************************/
	
    /*5.注册回调函数**************************/
	g_qep_int_callback = g_qep_callback;
#endif	


#ifdef WDOG_TEST

	/* ADD Code Here */
	
	/*1.WDOG初始化****************************/
	iwdg_init(8000);
	
	/*2.模块初始化****************************/

	
	/*3.中断使能与优先级设置*******************/
	
	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/

	
#endif	

#ifdef CLOCK_TEST

	/* ADD Code Here */
	
	/*1.CLOCK初始化****************************/

	/*Clock test GPIO初始化*/
	gpio_settings_config_t setting_cfg;
	setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_5;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_1;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
	/*2.模块初始化****************************/

	
	/*3.中断使能与优先级设置*******************/
	
	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/

	
#endif	
	
#ifdef ADC_TEST

	/* ADD Code Here */
	
//	ADC_MCPWMTrig_Test_Init();		
	/*1.GPIO初始化****************************/

	ADC_gpio_init();	
	/*2.模块初始化****************************/
	ADC_Test_Init();
	
	CSA_Test_Init();
	/*3.中断使能与优先级设置*******************/
	
	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/
		
		
#endif		

#ifdef SPI_TEST

	/* ADD Code Here */
	spi0_gpio_init();
	/*1.GPIO初始化****************************/
	spi_clk_enable();

	

	
	/*2.模块初始化****************************/

	
	/*3.中断使能与优先级设置*******************/
//	#ifndef DMA_TEST		//使用dma  关闭spi中断
	NVIC_SetPriority(SPI0_IRQn, 0); 		/* QEP_IRQn 优先级配置 */
	NVIC_ClearPendingIRQ(SPI0_IRQn);
	NVIC_EnableIRQ (SPI0_IRQn);

	NVIC_SetPriority(SPI1_IRQn, 0); 		/* QEP_IRQn 优先级配置 */
	NVIC_ClearPendingIRQ(SPI1_IRQn);
	NVIC_EnableIRQ (SPI1_IRQn);
//	#endif
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/

	
#endif	

#ifdef I2C_TEST

	/* ADD Code Here */
	
	/*1.GPIO初始化****************************/
	i2c_clk_enable();
//	i2c0_gpio_init();
	
	/*2.模块初始化****************************/

	
	/*3.中断使能与优先级设置*******************/
//	NVIC_SetPriority(I2C0_IRQn, 1); 		/* QEP_IRQn 优先级配置 */
//	NVIC_ClearPendingIRQ(I2C0_IRQn);
//	NVIC_EnableIRQ (I2C0_IRQn);
//	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/

	
#endif	

#ifdef CAN_TEST

	/* ADD Code Here */
	
	/*1.GPIO初始化****************************/

	
	/*2.模块初始化****************************/
	#if (CAN_SAMPLE_SEL == CAN_BASIC_SAMPLE)
	CAN_Basic_Sample();
	#elif (CAN_SAMPLE_SEL == CAN_BUSOFF_SAMPLE)
	CAN_BusOff_Sample();
	#elif (CAN_SAMPLE_SEL == CAN_WKUP_SAMPLE)
	CAN_Wkup_Sample();
	#endif

	
	/*3.中断使能与优先级设置*******************/
	
	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/

	
#endif	

#ifdef LIN_TEST

	/* ADD Code Here */
	
	/*1.GPIO初始化****************************/

	
	/*2.模块初始化****************************/

	
	/*3.中断使能与优先级设置*******************/
	
	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/

	
#endif	

#ifdef INFO_TEST

	/* ADD Code Here */
	
	/*1.GPIO初始化****************************/

	
	/*2.模块初始化****************************/

	
	/*3.中断使能与优先级设置*******************/
	
	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/

	
#endif	

#ifdef SRAM_TEST

	/* ADD Code Here */
	
	/*1.GPIO初始化****************************/
	//volatile int32_t 
	
	/*2.模块初始化****************************/
	for(volatile uint32_t m=0; m< SRAM_TEST_LENGTH_0; m++)
	{
		g_sram_test_buff0[m] = 0;
	}
	for(volatile uint32_t n=0; n< SRAM_TEST_LENGTH_1; n++)
	{
		g_sram_test_buff1[n] = 0;
		g_sram_test_buff2[n] = 0;
		g_sram_test_buff3[n] = 0;
		g_sram_test_buff4[n] = 0;
		g_sram_test_buff5[n] = 0;
		g_sram_test_buff6[n] = 0;
		g_sram_test_buff7[n] = 0;
	}

	while(1)
	{
		for(volatile uint32_t i=0; i< SRAM_TEST_LENGTH_0; i++)
		{
			g_sram_test_buff0[i]+=1;
		}
		for(volatile uint32_t j=0; j< SRAM_TEST_LENGTH_1; j++)
		{
			g_sram_test_buff1[j]+=2;
			g_sram_test_buff2[j]+=3;
			g_sram_test_buff3[j]+=4;
			g_sram_test_buff4[j]+=5;
			g_sram_test_buff5[j]+=6;
			g_sram_test_buff6[j]+=7;
			g_sram_test_buff7[j]+=8;
		}
		delay_1us(10);
	}
	
	/*3.中断使能与优先级设置*******************/
	
	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/

	
#endif	






#ifdef MODULE_TEST

	/* ADD Code Here */
	
	/*1.GPIO初始化****************************/

	
	/*2.模块初始化****************************/

	
	/*3.中断使能与优先级设置*******************/
	
	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/

	
#endif	

#ifdef DMA_TEST

	for(int i = 0;i<MCPWM_LEN;i++)
	 {
			MCPWM_Tx_buff[i] = i%MCPWM_LEN;
			INFO("MCPWM_Tx_buff[%d] = %d  ",i,MCPWM_Tx_buff[i]);
	 }
	 for(int i = 0;i<TIMER_LEN;i++)
	 {
		 TIMER_DMA_Tx_buff[i] += i*10;
	 }
	 for(volatile int i = 0;i<bufsize;i++)
	 {
		 tx_buf[i] = ((uint32_t)I2C_MASTER_CMD_WRITE << 8U) + i%256;   //数据必须是8位<=FF -----数据必须包含IC_DATA_CMD.CMD位强制写命令		 
	 }
	/* ADD Code Here */
	
	/*1.GPIO初始化****************************/

	
	/*2.模块初始化****************************/

	
	/*3.中断使能与优先级设置*******************/
	
	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/

	
#endif	
}



