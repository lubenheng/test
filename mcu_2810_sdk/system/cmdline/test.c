#include "test.h"
#ifdef UART_TEST
#include "uart_test.h"
#endif


#ifdef GPIO_TEST
//#include "gpio_hal.h"
#include "gpio_test.h"
#endif


#ifdef interrupt_TEST
//#include "gpio_hal.h"

#endif


#ifdef FLASH_TEST
#include "flash_test.h"
#endif


#ifdef ADC_TEST
#include "ADC.h"
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
//#include "dma_test.h"
#include "uart_cmd.h"
#endif

#ifdef CLOCK_TEST
//#include "clock_test.h"

/*临时引入*/
#include "gpio_hal.h"

#endif


#ifdef TIM_TEST
//#include "TIM_test.h"
#endif

#ifdef SPI_TEST
//#include "spi_test.h"
#endif

#ifdef I2C_TEST
//#include "i2c_test.h"
#endif

#ifdef CAN_TEST
#include "can.h"
#include "can_test.h"
#endif

#ifdef PMU_TEST
#include "pmu_test.h"
#endif

#ifdef SRAM_TEST
 


#endif

#ifdef TIM_TEST
 
	
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
 
	
#endif	


#ifdef PMU_TEST

	/* ADD Code Here */

	/*1.寻找是否是低功耗唤醒****************************/

 

	//__disable_irq();	
	
	/*2.模块初始化****************************/

	
	/*3.中断使能与优先级设置*******************/
	
	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/

	
#endif	

#ifdef GPIO_TEST

	/* ADD Code Here */
	
	/*1.GPIO初始化****************************/
	 

	
	/*2.模块初始化****************************/

	
	/*3.中断使能与优先级设置*******************/
	
	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/
#endif		


#ifdef HALL_TEST

	/* ADD Code Here */
	
	/*1.HALL初始化****************************/
	 
	
	/*2.模块初始化****************************/
 
 
	/*3.中断使能与优先级设置*******************/
	 
	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/
	 

	
#endif	


#ifdef QEP_TEST
	/* ADD Code Here */
	/*1.QEP初始化****************************/
	 
	
	/*2.模块初始化****************************/
 
	
	/*3.中断使能与优先级设置*******************/
	 
	/*4.软件相关配置**************************/
	
    /*5.注册回调函数**************************/
 
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
 
	/*2.模块初始化****************************/

	
	/*3.中断使能与优先级设置*******************/
	
	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/

	
#endif	
	
#ifdef ADC_TEST

	/* ADD Code Here */
 	
	/*1.GPIO初始化****************************/
 
	/*2.模块初始化****************************/
 
	/*3.中断使能与优先级设置*******************/
	
	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/
		
		
#endif		

#ifdef SPI_TEST

	/* ADD Code Here */
 
	/*1.GPIO初始化****************************/
 

	
	/*2.模块初始化****************************/

	
	/*3.中断使能与优先级设置*******************/
 
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/

	
#endif	

#ifdef I2C_TEST

	/* ADD Code Here */
	
	/*1.GPIO初始化****************************/
 
	/*2.模块初始化****************************/

	
	/*3.中断使能与优先级设置*******************/
 
//	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/

	
#endif	

#ifdef CAN_TEST

	/* ADD Code Here */
	
	/*1.GPIO初始化****************************/

	
	/*2.模块初始化****************************/
 

	
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
 
	/* ADD Code Here */
	
	/*1.GPIO初始化****************************/

	
	/*2.模块初始化****************************/

	
	/*3.中断使能与优先级设置*******************/
	
	
	/*4.软件相关配置**************************/
	
	
    /*5.注册回调函数**************************/

	
#endif	
}
