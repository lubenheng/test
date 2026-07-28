#ifndef ODS_5010_H
#define ODS_5010_H

/* -------------------------  ODS5010 Chip Characteristics	------------------------ */

/*
Core:                  Cortex-M0+
Max Frequency(MHz):    96/80/64/48

-------------------------  Memory	------------------------ 

Code Flash Size(KB):    128
Data Flash Size(KB):    0
Sram Size(KB):          16
Cache(KB):              no cache

-------------------------Key Features	------------------------ 
Uart/Lin:               2
Spi:                    2
I2c:                    3
PWM(ch):                1
Adc:                    1
TRGT: 					1		
GPT: 					1			
*/

/******************************************************************************/
/*                         SRAM memory map                              */
/******************************************************************************/
#define SRAM0_BASE						(0x20000000UL) 
#define SRAM_SIZE						(0x00004000UL) 

/*the size will be parsed from flash info*/
/*SRAM0(IBUS/DBUS->TCMC->SRAM0)*/
// #define TCM_IRAM0_BASE				(0x1FFFC000UL) 
// #define TCM_IRAM0_SIZE				(0x00004000UL) 



/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/

/* BUS Base Address */
#define CODE_FLASH_APB_BASE             (0x40022000UL)		
#define DATA_FLASH_APB_BASE				(0x20020000UL) //TODO
#define APB0_BASE						(0x40000000UL) //TODO
#define APB1_BASE						(0x40000000UL)
#define APB2_BASE						(0x40010000UL)
#define AHB_BASE						(0x40020000UL)
#define GPIO_BASE						(0x48000000UL)
/* Peripheral memory map */

/* APB1 Peripheral */
#define TIM3_BASE  						(APB1_BASE + 0x0400UL)
#define TIM4_BASE  						(APB1_BASE + 0x0800UL)
#define TIM5_BASE  						(APB1_BASE + 0x0C00UL)
#define TIM6_BASE  						(APB1_BASE + 0x1000UL)
#define TIM7_BASE  						(APB1_BASE + 0x1400UL)


#define TIM13_BASE  					(APB1_BASE + 0x1C00UL)
#define TIM14_BASE  					(APB1_BASE + 0x2000UL)

#define RTC_BASE  						(APB1_BASE + 0x2800UL)
#define IWDG_BASE  						(APB1_BASE + 0x3000UL)

#define SPI0_BASE						(APB1_BASE + 0x3800UL)

#define UART0_BASE						(APB1_BASE + 0x4000UL)
#define UART1_BASE						(APB1_BASE + 0x4400UL)

#define I2C0_BASE						(APB1_BASE + 0x5400UL)
#define I2C1_BASE						(APB1_BASE + 0x5800UL)
#define CAN1_BASE						(AHB_BASE  + 0x1000UL)

#define MATH_BASE  						(APB1_BASE + 0x6000UL)
#define HALL_BASE						(APB1_BASE + 0x6400UL)
#define QEP_BASE						(APB1_BASE + 0x6430UL)
#define ANALOG_BASE						(APB1_BASE + 0x7800UL)

#define PWR_BASE						(APB1_BASE + 0x7000UL)

/* OCA Peripheral */
#define CSA_BASE						(APB1_BASE + 0x7810UL)


/* APB2 Peripheral */
#define EXTI_BASE						(APB2_BASE + 0x0400UL)

#define MCPWM0_BASE						(APB2_BASE + 0x0C00UL)

#define ADC_BASE						(APB2_BASE + 0x2400UL)

#define TIM1_BASE						(APB2_BASE + 0x2C00UL)
#define SPI1_BASE						(APB2_BASE + 0x3800UL)

#define TIM15_BASE						(APB2_BASE + 0x4000UL)
#define TIM16_BASE						(APB2_BASE + 0x4400UL)


/* AHB Peripheral */
#define DMA_BASE						(APB1_BASE + 0x0UL)
#define RCC_BASE						(AHB_BASE + 0x0000UL)
#define SYSCFG_BASE						(AHB_BASE + 0x0400UL)
#define PMU_BASE						(AHB_BASE + 0x0500UL)
#define FMC_BASE						(AHB_BASE + 0x2000UL)
#define CRC_BASE						(AHB_BASE + 0x3000UL)


/* GPIO Peripheral */
#define GPIOA_BASE						(GPIO_BASE + 0x0UL)
#define GPIOB_BASE						(GPIO_BASE + 0x0400UL)
#define GPIOC_BASE						(GPIO_BASE + 0x0800UL)



/* -------------------------MCU	CLOCK Definition   ------------------------ */

#define CLOCK  				((CLOCK_TypeDef*)CLKGEN_BASE)
#define CMU0				((CMU_TypeDef *)(CLKGEN_BASE+0x100))
#define CMU1				((CMU_TypeDef *)(CLKGEN_BASE+0x124))


 /** Array initializer of CMU peripheral base addresses */
#define CMU_BASE_ADDRS      {CMU0, CMU1}

/** clock modules */
typedef enum
{
	/** pcg */
	MODULE_CODE_FLASH = 0UL,
	MODULE_DATA_FLASH,
	MODULE_ADC0,
	MODULE_GPT0, /*!< function clock is apb0 or hsc clock */
	MODULE_GPT1, /*!< function clock is apb0 or hsc clock */
	MODULE_UART0,/*!< function clock is apb0 or hsc clock */
	MODULE_UART1,/*!< function clock is apb1 or hsc clock */
	MODULE_I2C0, /*!< function clock is apb0 or hsc clock */
	MODULE_I2C1, /*!< function clock is apb1 or hsc clock */
	MODULE_SPI0, /*!< function clock is apb0 or hsc clock */
	MODULE_SPI1, /*!< function clock is apb1 or hsc clock */
	MODULE_CAN1, /*!< function clock is apb1 or osc clock */
	MODULE_MCPWM0, /*!< function clock is sys or hsc clock */
	MODULE_MCPWM1, /*!< function clock is sys or hsc clock */
	MODULE_TRGT0, /*!< function clock is apb1 or hsc clock */
	MODULE_CRC0,  
	MODULE_PORTA,
	MODULE_PORTB,
	MODULE_PORTC,
	MODULE_SCM,
	MODULE_WDG,
	MODULE_EWDG,
	MODULE_RTC,
	MODULE_HIB,
	MODULE_PCG_NUM, /*!< used to get the PCG number automatically */

	/*clock types correspond system*/
	SCG_SYS, 
	SCG_HSC,
	SCG_OSC,
	SCG_APB0,
	SCG_APB1,
	SCG_LIRC,
}module_name_e;

#define CLOCK_PCG_NUM			MODULE_PCG_NUM

/* WITH_PCLK_FCLKAPB1: peripheral has fclk_APB1&pclk
 * WITH_PCLK_FCLKAPB0: peripheral has fclk_APB0&pclk
 * WITH_PCLK_APB0: peripheral only has pclk, fclk from APB0
 * WITH_PCLK_APB1: peripheral only has pclk, fclk from APB1*/
#define CLOCK_PCG_FEATURES    {	{MODULE_CODE_FLASH,	WITH_PCLK_APB0}, 			{MODULE_DATA_FLASH, WITH_PCLK_APB0},\
								{MODULE_ADC0, 		WITH_PCLK_APB0}, 		 	{MODULE_GPT0, 	WITH_PCLK_FCLKAPB0},\
								{MODULE_GPT1,		WITH_PCLK_FCLKAPB0},	 	{MODULE_UART0, 	WITH_PCLK_FCLKAPB0},\
								{MODULE_UART1,  	WITH_PCLK_FCLKAPB1},		{MODULE_I2C0, 	WITH_PCLK_FCLKAPB0},\
								{MODULE_I2C1, 		WITH_PCLK_FCLKAPB1},		{MODULE_I2C2, 	WITH_PCLK_FCLKAPB1},\
								{MODULE_SPI0, 		WITH_PCLK_FCLKAPB0}, 		{MODULE_SPI1, 	WITH_PCLK_FCLKAPB1},\
								{MODULE_CAN1, 		WITH_PCLK_FCLKAPB1},		{MODULE_PWM0, 	WITH_HCLK_FCLKSYS},\
								{MODULE_PWM1, 		WITH_HCLK_FCLKSYS},			{MODULE_PWM2, 	WITH_HCLK_FCLKSYS},\
								{MODULE_TRGT0,  	WITH_PCLK_FCLKAPB1},		{MODULE_CRC0, 	WITH_PCLK_APB1},\
								{MODULE_PORTA,		WITH_PCLK_APB0}, 			{MODULE_PORTB,  WITH_PCLK_APB0},\
								{MODULE_PORTC, 		WITH_PCLK_APB0}, 			{MODULE_PORTD,  WITH_PCLK_APB0},\
								{MODULE_PORTE, 		WITH_PCLK_APB0},			{MODULE_SCM, 	WITH_PCLK_APB0},\
								{MODULE_WDG, 		WITH_PCLK_APB0}, 			{MODULE_EWDG, 	WITH_PCLK_APB0},\
								{MODULE_RTC, 		WITH_PCLK_APB0}, 			{MODULE_HIB, 	WITH_PCLK_APB0} }

/* -------------------------ODS5010	FLASH Definition   ------------------------ */
/** Peripheral code flash base pointer */
#define CODE_FLASH_PTR			((FLASH_TypeDef *) CODE_FLASH_APB_BASE)
#define FLASH_PTR			((FLASH_TypeDef *) CODE_FLASH_APB_BASE)
/** Peripheral data flash base pointer */
#define DATA_FLASH_PTR			((FLASH_TypeDef *) DATA_FLASH_APB_BASE)

/** number of instances of the FLASH module: code flash and data flash. */
#define FLASH_INSTANCE_MAX_COUNT				2U
/** number of flash base addrs array. */
#define FLASH_PTR_NUM							2U
/** Array initializer of flash peripheral base addresses */
#define FLASH_BASE_ADDRS                        { CODE_FLASH_PTR, DATA_FLASH_PTR}
/** Interrupt vectors for the flash peripheral type */
#define FLASH_IRQS  							{ CODE_FLASH_IRQn, DATA_FLASH_IRQn }

/** wait w/r/e operation finish max time */
/** ms, theoretical write time:30~34us, read time:25ns */
#define FLASH_WR_TIME_OUT				5U
/** ms , theoretical chip erase time:30~40ms */
#define FLASH_CHIP_ERASE_TIME_OUT		50U
/** ms , theoretical sector erase time:2~3ms*/
#define FLASH_SECTOR_ERASE_TIME_OUT	    10U
/** ms , theoretical info erase time:2~3ms*/
#define FLASH_INFO_ERASE_TIME_OUT		10U
/** ms , theoretical info refresh time:4ms*/
#define FLASH_INFO_REFRESH_TIME_OUT		400U

/** flash mode freq range */
#define FALSH_FAST_FREQ			72000000U
#define FALSH_MIDDLE_FREQ		57500000U
#define FALSH_SLOW_FREQ			43000000U

/**code flash w/r len once, unit: word */
#define CODE_FLASH_RW_LEN				1U
/**data flash w/r len once, unit: word */
#define DATA_FLASH_RW_LEN				1U
/** code flash w/r len once, unit: byte */
#define CODE_FLASH_RW_LEN_BYTE			4U
/** data flash w/r len once, unit: byte */
#define DATA_FLASH_RW_LEN_BYTE			4U
/** code flash sector mask value (4Bytes) */
#define CODE_FLASH_SLOT_MASK						0x3UL
/** code flash sector mask value (4Bytes) */
#define DATA_FLASH_SLOT_MASK						0x3UL

/** code flash slot number: 128*/
#define CODE_FLASH_SLOT_NUMBER	128U
/** data flash slot number: 128 */
#define DATA_FLASH_SLOT_NUMBER	128U

/**code flash physical base address, unit: byte */
#define CODE_FLASH_PHYSICAL_BASE_ADDRESS				0UL
/**data flash physical base address, unit: byte */
#define DATA_FLASH_PHYSICAL_BASE_ADDRESS				0x20020000UL

/** data flash sector offset bit: 9, unit:bytes */
#define DATA_FLASH_SECTOR_OFFSET_BIT		9UL
/** data flash sector mask value: 512, unit:bytes */
#define DATA_FLASH_SECTOR_MASK_VALUE		511U

/**data flash sector number */
#define DATA_FLASH_SECTOR_NUMBER				64
/**code flash sector number, unit: byte */
#define CODE_FLASH_SECTOR_NUMBER				256


 /* 128K byte - 8*4byte, the unit is byte 32760 */
 /* 127K byte, the unit is byte 32512 */
#define MAIN_FLASH_MAX_SIZE 					(32760UL<<2U)

 /* 4*4byte, the unit is byte */
#define INFO_USER_FLASH_MAX_SIZE				(4UL<<2U)

 /* 4*4byte, the unit is byte */
#define INFO_VENDOR_FLASH_MAX_SIZE				(4UL<<2U)

 /* 64*4byte, the unit is byte */
#define OTP_FLASH_MAX_SIZE						(64UL<<0U)

/* -------------------------MCU	PMU Definition   ------------------------ */

//#define PMU                  ((PMU_TypeDef *) PMU_BASE)

/* -------------------------MCU	UART Definition   ------------------------ */
 
#define UART0				((UART_TypeDef *)UART0_BASE)
#define UART1				((UART_TypeDef *)UART1_BASE)
 
/** Number of instances of the UART module. */
#define UART_INSTANCE_MAX_COUNT					(2U)

/** Array initializer of UART peripheral base addresses */
#define UART_BASE_ADDRS		{UART0, UART1}

/** Interrupt vectors for the UART peripheral*/
#define UART_IRQS		{UART0_IRQn, UART1_IRQn}

/** Table to save UART clock names. */
#define UART_CLOCK_NAMES	{MODULE_UART0, MODULE_UART1}


/** UART instance selection */
typedef enum
{
	UART_INSTANCE0 = 0UL, /*!< UART instance 0 */
	UART_INSTANCE1 = 1UL,      /*!< UART instance 1 */
	UART_INSTANCE_INVALID = 2UL,/*!< UART instance invalid */
}uart_instance_e;

typedef enum
{
	LIN_INSTANCE0 = 0UL, /*!< LIN0 */
	LIN_INSTANCE1 = 1UL,	   /*!< LIN1 */
	LIN_INSTANCE_INVALID = 2UL,/*!< UART instance invalid */
}lin_instance_e;


/* -------------------------MCU	GPIO Definition   ------------------------ */

#define GPIO_PORTA                ((PORT_TypeDef *) GPIOA_BASE )
#define GPIO_PORTB                ((PORT_TypeDef *) GPIOB_BASE )
#define GPIO_PORTC                ((PORT_TypeDef *) GPIOC_BASE )


/** Ports address vectors for the gpio peripheral type */
#define GPIO_BASE_ADDRS	{GPIO_PORTA,GPIO_PORTB,GPIO_PORTC}
/** Interrupt vectors for the gpio peripheral type */
#define GPIO_IRQS	{GPIO_A_IRQn, GPIO_B_IRQn, GPIO_C_IRQn}


/* -------------------------ODS5010	DMA Definition   ------------------------ */
#define FEATURE_HAS_DMA_ENABLE (0)

/* -------------------------ODS5010	SPI Definition   ------------------------ */
 
 #define SPI0          ((SPI_TypeDef *) SPI0_BASE )
 #define SPI1          ((SPI_TypeDef *) SPI1_BASE )


/** Number of instances of the SPI module. */
#define SPI_INSTANCE_MAX_COUNT					(2U)

/** Array initializer of SPI peripheral base addresses */
#define SPI_BASE_ADDRS		{SPI0, SPI1}

/** Interrupt vectors for the SPI peripheral type */
#define SPI_IRQS			{SPI0_IRQn, SPI1_IRQn}

/** Table to save SPI clock names. */
#define SPI_CLOCK_NAMES		{MODULE_SPI0, MODULE_SPI1}

/** SPI instance selection */
typedef enum
{
	SPI_INSTANCE0 = 0U, /*!< SPI instance 0 */
	SPI_INSTANCE1,      /*!< SPI instance 1 */
}spi_instance_e;


/* -------------------------ODS5010	CAN Definition   ------------------------ */

/* CAN - Peripheral instance base addresses */
/** Peripheral CAN1 base pointer */
#define CAN1                                     ((CAN_Type *)CAN1_BASE)



/** Number of instances of the CAN module. */
#define CAN_INSTANCE_MAX_COUNT                       (1u)

/** Array initializer of CAN peripheral base addresses */
#define CAN_BASE_ADDRS                           { CAN1 }

/** Interrupt vectors for the CAN peripheral type */
//#define CAN_Rx_Warning_IRQS                      { CAN1_ORed_IRQn}
//#define CAN_Tx_Warning_IRQS                      { CAN1_ORed_IRQn}
//#define CAN_Bus_Off_IRQS                         { CAN1_ORed_IRQn}

//#define CAN_Wake_Up_IRQS                         { CAN1_WAKE_UP_IRQn}
//#define CAN_Error_IRQS                           { CAN1_ERROR_IRQn}
//#define CAN_ORed_0_15_MB_IRQS                    { CAN1_ORed_0_15_MB_IRQn}
//#define CAN_ORed_16_31_MB_IRQS                   { CAN1_ORed_16_31_MB_IRQn }

/** CAN instance selection. */
typedef enum
{
	CAN1_INSTANCE = 0U,		/*!< CAN instance 0 */
}can_instance_e;

///** Maximum number of Message Buffers supported for payload size 8 for CAN1 */
//#define CAN1_MAX_MB_NUM             		(32U)
///** Maximum number of Message Buffers supported for payload size 8 for any of the CAN instances */
//#define CAN_MAX_MB_NUM              		(32U)
///** Maximum number of Message Buffers supported for payload size 8 for any of the CAN instances */
//#define CAN_MAX_MB_NUM_ARRAY        		{CAN1_MAX_MB_NUM}			  


/* -------------------------ODS5010	I2C Definition   ------------------------ */
 
#define I2C0                  ((I2C_TypeDef *) I2C0_BASE )
#define I2C1                  ((I2C_TypeDef *) I2C1_BASE )


 /** Number of instances of the I2C module. */
#define I2C_INSTANCE_MAX_COUNT		(2U)

/** Number of DMA Request of the I2C module. */
#define I2C_DMA_REQ_MAX_COUNT		  (6U)

/** Array initializer of I2C peripheral base addresses */
#define I2C_BASE_ADDRS				{ I2C0, I2C1}

/** Interrupt vectors for the I2C peripheral type */
#define I2C_IRQS				{I2C0_IRQn, I2C1_IRQn}

/** I2C instance selection. */
typedef enum {
	I2C_INSTANCE0 = 0U,	/*!< I2C instance 0 */
	I2C_INSTANCE1,		/*!< I2C instance 1 */
}i2c_instance_e;

/** DMA for I2C request selection. */
typedef enum
{
	I2C0_DMA_RX = 0U,	/*!< Use I2C0 RX as DMA request. */
	I2C0_DMA_TX,		/*!< Use I2C0 TX as DMA request. */
	I2C1_DMA_RX,		/*!< Use I2C1 RX as DMA request. */
	I2C1_DMA_TX,		/*!< Use I2C1 TX as DMA request. */
}i2c_dma_req_e;



/* -------------------------ODS5010	RTC Definition   ------------------------ */
#define RTC						(( RTC_TypeDef *) RTC_BASE )

#define RTC_BASE_ADDR			RTC
 /** Number of interrupts of the RTC module. */
#define RTC_INTERRUPT_COUNT		(2U)

/* -------------------------ODS5010	WDOG Definition   ------------------------ */
 
#define IWDG                ((IWDG_TypeDef *) IWDG_BASE)

/* -------------------------ODS5010	CRC Definition   ------------------------ */

#define CRC                ((CRC_TypeDef *) CRC_BASE )

/* -------------------------ODS5010	RSTGEN Definition   ------------------------ */

#define RSTGEN               ((RSTGEN_TypeDef *) RSTGEN_BASE)


/* -------------------------ODS5010	ADC Definition   ------------------------ */

#define ADC                ((ADC_TypeDef *) ADC_BASE)


/* -------------------------ODS5010	HWMATH Definition   ------------------------ */

#define HWMATH                ((HWMATH_TypeDef *) MATH_BASE)


/* -------------------------ODS5010	RCC Definition   ------------------------ */
#define RCC                ((RCC_TypeDef *) RCC_BASE )

/* -------------------------ODS5010	TIMER Definition   ------------------------ */
#define TIMER13                ((TIMx_TypeDef *) TIM13_BASE )
#define TIMER14                ((TIMx_TypeDef *) TIM14_BASE )

#define TIMER15                ((TIMx_TypeDef *) TIM15_BASE )
#define TIMER16                ((TIMx_TypeDef *) TIM16_BASE )
/* -------------------------ODS5010	TRGMUX Definition   ------------------------ */

#define HALL 				  ((HALL_TypeDef *) HALL_BASE)
#define QEP				 	  ((TIM_QEPTypeDef *) QEP_BASE)

#define ANA_CONTROL			  ((ANAC_TypeDef *) ANALOG_BASE)
#define SYSCFG				  ((SYSCFG_TypeDef*)SYSCFG_BASE)
#define PMU					  ((PMU_TypeDef*)PMU_BASE)

/* -------------------------ODS5010	DMA Definition   ------------------------ */
#define DMA                 ((DMA_TypeDef *)DMA_BASE)

/* -------------------------ODS5010	OCA Definition   ------------------------ */

#define CSA                ((CSA_TypeDef *) CSA_BASE)
// #define TRGMUX               ((TRGMUX_TypeDef *) TRGMUX_BASE )

// #define TRGMUX_ADC_SOURCE_MAX_COUNT (41U)
// #define TRGMUX_TRGMUX_SOURCE_MAX_COUNT (41U)
// #define TRGMUX_DMA_SOURCE_MAX_COUNT (16U)
// #define TRGMUX_TRGT_SOURCE_MAX_COUNT (41U)
// #define TRGMUX_GPT_SOURCE_MAX_COUNT (16U)
// #define TRGMUX_PERIPHERALS_SOURCE_MAX_COUNT (16U)
// #define TRGMUX_OUTPUT_MAX_COUNT (4U)

#endif /*ODS_5010_H*/

