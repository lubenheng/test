#include "test.h"
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
#include "clock_test.h"

/*��ʱ����*/
#include "gpio_hal.h"

#endif


#ifdef TIM_TEST
#include "TIM_test.h"
#endif

#ifdef SPI_TEST
//#include "spi_test.h"
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

	/*1.Ѱ���Ƿ��ǵ͹��Ļ���****************************/

 

	//__disable_irq();	
	
	/*2.ģ���ʼ��****************************/

	
	/*3.�ж�ʹ�������ȼ�����*******************/
	
	
	/*4.�����������**************************/
	
	
    /*5.ע��ص�����**************************/

	
#endif	

#ifdef GPIO_TEST

	/* ADD Code Here */
	
	/*1.GPIO��ʼ��****************************/
	 

	
	/*2.ģ���ʼ��****************************/

	
	/*3.�ж�ʹ�������ȼ�����*******************/
	
	
	/*4.�����������**************************/
	
	
    /*5.ע��ص�����**************************/
#endif		


#ifdef HALL_TEST

	/* ADD Code Here */
	
	/*1.HALL��ʼ��****************************/
	 
	
	/*2.ģ���ʼ��****************************/
 
 
	/*3.�ж�ʹ�������ȼ�����*******************/
	 
	
	/*4.�����������**************************/
	
	
    /*5.ע��ص�����**************************/
	 

	
#endif	


#ifdef QEP_TEST
	/* ADD Code Here */
	/*1.QEP��ʼ��****************************/
	 
	
	/*2.ģ���ʼ��****************************/
 
	
	/*3.�ж�ʹ�������ȼ�����*******************/
	 
	/*4.�����������**************************/
	
    /*5.ע��ص�����**************************/
 
#endif	


#ifdef WDOG_TEST

	/* ADD Code Here */
	
	/*1.WDOG��ʼ��****************************/
	iwdg_init(8000); 	
	/*2.ģ���ʼ��****************************/

	
	/*3.�ж�ʹ�������ȼ�����*******************/
	
	
	/*4.�����������**************************/
	
	
    /*5.ע��ص�����**************************/

	
#endif	

#ifdef CLOCK_TEST

	/* ADD Code Here */
	
	/*1.CLOCK��ʼ��****************************/

	/*Clock test GPIO��ʼ��*/
 
	/*2.ģ���ʼ��****************************/

	
	/*3.�ж�ʹ�������ȼ�����*******************/
	
	
	/*4.�����������**************************/
	
	
    /*5.ע��ص�����**************************/

	
#endif	
	
#ifdef ADC_TEST

	/* ADD Code Here */
 	
	/*1.GPIO��ʼ��****************************/
 
	/*2.ģ���ʼ��****************************/
 
	/*3.�ж�ʹ�������ȼ�����*******************/
	
	
	/*4.�����������**************************/
	
	
    /*5.ע��ص�����**************************/
		
		
#endif		

#ifdef SPI_TEST

	/* ADD Code Here */
 
	/*1.GPIO��ʼ��****************************/
 

	
	/*2.ģ���ʼ��****************************/

	
	/*3.�ж�ʹ�������ȼ�����*******************/
 
	/*4.�����������**************************/
	
	
    /*5.ע��ص�����**************************/

	
#endif	

#ifdef I2C_TEST

	/* ADD Code Here */
	
	/*1.GPIO��ʼ��****************************/
 
	/*2.ģ���ʼ��****************************/

	
	/*3.�ж�ʹ�������ȼ�����*******************/
 
//	
	/*4.�����������**************************/
	
	
    /*5.ע��ص�����**************************/

	
#endif	

#ifdef CAN_TEST

	/* ADD Code Here */
	
	/*1.GPIO��ʼ��****************************/

	
	/*2.ģ���ʼ��****************************/
 

	
	/*3.�ж�ʹ�������ȼ�����*******************/
	
	
	/*4.�����������**************************/
	
	
    /*5.ע��ص�����**************************/

	
#endif	

#ifdef LIN_TEST

	/* ADD Code Here */
	
	/*1.GPIO��ʼ��****************************/

	
	/*2.ģ���ʼ��****************************/

	
	/*3.�ж�ʹ�������ȼ�����*******************/
	
	
	/*4.�����������**************************/
	
	
    /*5.ע��ص�����**************************/

	
#endif	

#ifdef INFO_TEST

	/* ADD Code Here */
	
	/*1.GPIO��ʼ��****************************/

	
	/*2.ģ���ʼ��****************************/

	
	/*3.�ж�ʹ�������ȼ�����*******************/
	
	
	/*4.�����������**************************/
	
	
    /*5.ע��ص�����**************************/

	
#endif	

#ifdef SRAM_TEST

	/* ADD Code Here */
	
	/*1.GPIO��ʼ��****************************/
	//volatile int32_t 
	
	/*2.ģ���ʼ��****************************/
 
	
	/*3.�ж�ʹ�������ȼ�����*******************/
	
	
	/*4.�����������**************************/
	
	
    /*5.ע��ص�����**************************/

	
#endif	






#ifdef MODULE_TEST

	/* ADD Code Here */
	
	/*1.GPIO��ʼ��****************************/

	
	/*2.ģ���ʼ��****************************/

	
	/*3.�ж�ʹ�������ȼ�����*******************/
	
	
	/*4.�����������**************************/
	
	
    /*5.ע��ص�����**************************/

	
#endif	

#ifdef DMA_TEST
 
	/* ADD Code Here */
	
	/*1.GPIO��ʼ��****************************/

	
	/*2.ģ���ʼ��****************************/

	
	/*3.�ж�ʹ�������ȼ�����*******************/
	
	
	/*4.�����������**************************/
	
	
    /*5.ע��ص�����**************************/

	
#endif	
}
