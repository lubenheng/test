#ifndef TEST_H
#define TEST_H


#define CMD_MODULE_TEST 
#ifdef CMD_MODULE_TEST


#define	ENABLE_UART_TEST						(0)
#define	ENABLE_GPIO_TEST						(0)
#define	ENABLE_FLASH_TEST						(0)
#define	ENABLE_MCPWM_TEST						(0)
#define	ENABLE_ADC_TEST	 		    		(0)
#define	ENABLE_HWMATH_TEST	        (0)
#define	ENABLE_WDOG_TEST	       	  (0)
#define	ENABLE_HALL_TEST	       	  (0)
#define	ENABLE_QEP_TEST	            (0)
#define	ENABLE_I2C_TEST	            (0)
#define	ENABLE_SPI_TEST	            (1)
#define	ENABLE_CAN_TEST	            (0)
#define	ENABLE_RTC_TEST	            (0)
#define	ENABLE_DMA_TEST	            (1)
#define	ENABLE_CLOCK_TEST	       	  (0)
#define	ENABLE_SRAM_TEST	       	 	(0)
#define	ENABLE_INFO_TEST	        	(0)
#define ENABLE_TIM_TEST             (0)
#define ENABLE_PMU_TEST             (0)



#define	ENABLE_MODULE_TEST	        (0)

#define	ENABLE_CSA_TEST	 		    (0)

#if ENABLE_HWMATH_TEST
    #define HWMATH_TEST
#else
    #undef HWMATH_TEST
#endif


#if ENABLE_UART_TEST
    #define UART_TEST
#else
    #undef UART_TEST
#endif


#if ENABLE_GPIO_TEST
    #define GPIO_TEST
#else
    #undef GPIO_TEST
#endif


#if ENABLE_FLASH_TEST
    #define FLASH_TEST
#else
    #undef FLASH_TEST
#endif


#if ENABLE_MCPWM_TEST
    #define MCPWM_TEST
#else
    #undef MCPWM_TEST
#endif


#if ENABLE_ADC_TEST
    #define ADC_TEST
#else
    #undef ADC_TEST
#endif

#if ENABLE_CSA_TEST
    #define CSA_TEST
#else
    #undef CSA_TEST
#endif

#if ENABLE_WDOG_TEST
    #define WDOG_TEST
#else
    #undef WDOG_TEST
#endif


#if ENABLE_HALL_TEST
    #define HALL_TEST
#else
    #undef HALL_TEST
#endif

#if ENABLE_QEP_TEST
    #define QEP_TEST
#else
    #undef QEP_TEST
#endif

#if ENABLE_RTC_TEST
    #define RTC_TEST
#else
    #undef RTC_TEST
#endif

#if ENABLE_I2C_TEST
    #define I2C_TEST
#else
    #undef I2C_TEST
#endif

#if ENABLE_SPI_TEST
    #define SPI_TEST
#else
    #undef SPI_TEST
#endif

#if ENABLE_CAN_TEST
    #define CAN_TEST
#else
    #undef CAN_TEST
#endif

#if ENABLE_DMA_TEST
    #define DMA_TEST
#else
    #undef DMA_TEST
#endif

#if ENABLE_CLOCK_TEST
    #define CLOCK_TEST
#else
    #undef CLOCK_TEST
#endif

#if ENABLE_SRAM_TEST
    #define SRAM_TEST
#else
    #undef SRAM_TEST
#endif

#if ENABLE_TIM_TEST
    #define TIM_TEST
#else
    #undef TIM_TEST
#endif

#if ENABLE_INFO_TEST
    #define INFO_TEST
#else
    #undef INFO_TEST
#endif

#if ENABLE_PMU_TEST
    #define PMU_TEST
#else
    #undef PMU_TEST
#endif


#if ENABLE_MODULE_TEST
    #define MODULE_TEST
#else
    #undef MODULE_TEST
#endif














#endif /*MODULE_TEST*/

void module_test(void);

#endif /*TEST_H*/

