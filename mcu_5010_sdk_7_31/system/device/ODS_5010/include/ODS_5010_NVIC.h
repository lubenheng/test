#ifndef ODS_5010_NVIC_H
#define ODS_5010_NVIC_H


/* -------------------------  Interrupt Number Definition	------------------------ */
  
 typedef enum
 {
	NotAvail_IRQn				  = -128,    /*!<  Not available device specific interrupt */	
 	/* -------------------  Processor Exceptions Numbers  ----------------------------- */
	Reset_IRQn					= -15,		/*!<  1 Reset Vector, invoked on Power up and warm reset*/
	NonMaskableInt_IRQn			= -14,		/*!<  2 Non Maskable Interrupt */
	HardFault_IRQn				= -13,		/*!<  3 HardFault Interrupt */
	MemoryManagement_IRQn		= -12,		/*!<  4 Memory Management Interrupt */
	BusFault_IRQn				= -11,		/*!<  5 Bus Fault Interrupt */
	UsageFault_IRQn				= -10,		/*!<  6 Usage Fault Interrupt */
	SVCall_IRQn					=  -5,		/*!< 11 SV Call Interrupt */
	DebugMonitor_IRQn			=  -4,		/*!< 12 Debug Monitor Interrupt */
	PendSV_IRQn					=  -2,		/*!< 14 Pend SV Interrupt */
	SysTick_IRQn				=  -1,		/*!< 15 System Tick Interrupt */
 
	/* -------------------  Processor Interrupt Numbers  ------------------------------ */
	HIB_IRQn					=   0U,
	CODE_FLASH_IRQn				=   1U, 
	DATA_FLASH_IRQn				=   2U,
	RSTGEN_IRQn					=   3U,
	CMU_IRQn					=   4U,
	ECC_IRQn					=   5U,
	WDOG_IRQn					=   6U,
	EWDOG_IRQn					=   7U,
	RTC_IRQn					=   8U,
	RTC_SECOND_IRQn				=   9U,
	I2C0_IRQn					=   10U,
	I2C1_IRQn					=   11U,
	UART0_IRQn					=   12U,
	UART1_IRQn					=   13U,
	ADC0_REG_IRQn				=   14U,
	ADC0_INJA_IRQn				=   15U,
	ADC0_INJB_IRQn				=   16U,
	I2C2_IRQn					=   17U,
	SPI0_IRQn					=   18U,
	SPI1_IRQn					=   19U,
	GPT0_IRQn					=   20U,
	GPT1_IRQn					=   21U,
	GPIO_ORed_IRQn				=   22U, /*!< PORTA~PORTE OR'ed interrupt*/
	TRGT_ORed_IRQn 			   	=   23U, /*!< TRGT Interrupt CH0~CH3*/
	PWM0_IRQn					=   24U,
	PWM1_IRQn					=   25U,
	PWM2_IRQn					=   26U,
	CAN0_ERROR_IRQn			   	=   27U, /* bus off or bus off done or bus error or bus error fd or bus tx warning or bus rx warning */
	CAN0_WAKE_UP_IRQn			=   28U, /* wake match or wake in or wake to */
	CAN0_ORed_0_15_MB_IRQn		=   29U, /* OR'ed Message buffer (0-15) */
	CAN0_ORed_16_31_MB_IRQn		=   30U, /* OR'ed Message buffer (16-31) */
	CAN0_ORed_IRQn				=   31U, 
	DMA0_IRQn					   ,
 } IRQn_Type;

/* ----------------Configuration of the Cortex-M0 Processor---------------- */
#define __CM0_REV                 0x0201    /*!< Core Revision r2p1                               */
#define __NVIC_PRIO_BITS          2         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used     */
#define __MPU_PRESENT             0         /*!< MPU present or not                               */


#endif /*SA32A12_NVIC_H*/

