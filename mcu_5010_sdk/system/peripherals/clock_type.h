#ifndef CLOCK_TYPE_H
#define CLOCK_TYPE_H

#include "ODS_5010.h"


/** peripheral clock type information */
//typedef struct
//{
//	module_name_e module_name;
//	peripheral_clk_type_e peripheral_clk_type;
//}module_feature_t;


/** function clock selection */
typedef enum
{
	APB_CLK = 0U, /*!< fclk source from APB0 or APB1 clock which determined by the APB connection of the correspond module */
	HSC_CLK		  /*!< the clock source is selected as OSC for CAN&WDOG while other modules secleted as HSC clock path */
}fclk_sel_e;


/** osc reference clock select */
typedef enum
{
	OSC_INT = 0U,	/*!< osc is from internal oscillator */
	OSC_EXT_REF		/*!< osc is from external reference clock, internal oscillator is bypassed */
}osc_type_e;

/** system clock source */
typedef enum
{
	SYS_HIRC = 0U,		/*!< system clock is from HIRC, frequency is 24MHz */
	SYS_OSC_INT,      	/*!< system clock is from internal oscillator */
	SYS_OSC_EXT_REF,    /*!< system clock is from external reference clock */
	SYS_LIRC,			/*!< system clock is from LIRC */
	SYS_SRC_MAX,       	/*!< invalid value */
}sys_clk_src_e;

typedef enum
{
	DISABLE_CLOCK = 0U, /*!< disable clock */
	ENABLE_CLOCK  	    /*!< enable clock */
}clk_ctl_e;

typedef enum
{
	GATING_OFF = 0U, /*!< when enters deep sleep, disable gate(clock is still runing) */
	GATING_ON        /*!< when enters deep sleep, enable gate(clock will be disabled) */
}clk_dsleep_gating_en_e;

/** module which controls gate independently in deepsleep state */
typedef enum
{
	GATE_AHB_MTX = 0U,
	GATE_SRAM,
	GATE_AHB2APB0,
	GATE_AHB2APB1,
	GATE_CODE_FLASH,
	RESERVED,
	GATE_HIRC,
	GATE_OSC,
	GATE_DATA_FLASH
}system_dsleep_gate_e;



/* clock error code */
typedef enum
{
	ERC_CLK_NO_ERROR = 0U,
	ERC_CLK_SYS_SRC_INVALID,   /*!< sys clock source invalid */
	ERC_CLK_OSC_FRE_OUT_RANGE, /*!< osc fre is out of range, range: 8-40MHz */
	ERC_CLK_DIV_INVALID,	   /*!< apb1,apb0, trace_clk divider is out of range */
	ERC_CLK_MODULE_INVALID,    /*!< module name enum value is invalid */
	ERC_CLK_DSLEEP_INVALID,    /*!< dsleep gate enum value is in valid */
	ERC_CLK_CMU_INVALID,	   /*!< cmu module enum value is invalid */
	ERC_CLK_HSC_SEL_INVALID,   /*!< hscclk enum value is invalid */
	ERC_CLK_FR_SEL_INVALID,    /*!< frclk enum value is invalid */
	ERC_CLK_OSC_NOT_READY      /*!< oscillator not ready when switching system clock source */
}clk_erc_e;

typedef enum
{
	CLOCK_MCO_SYS = 0U,
	MCO_RESERVED1,
	CLOCK_MCO_HSI,
	CLOCK_MCO_PLLCLK,
	CLOCK_MCO_HSE,
	CLOCK_MCO_LSI
}clock_mco_select;

typedef enum
{
	CLOCK_SYS_HSI = 0U,
	CLOCK_SYS_HSE,
	CLOCK_SYS_PLLCLK1,
	CLOCK_SYS_PLLCLK2,
	CLOCK_SYS_RESERD
}clock_sys_select;

/** clock oscillator type for unified enable interface (rcc_hal_osc_enable) */
typedef enum
{
	CLOCK_OSC_HSI = 0U,  /*!< ?? 32MHz RC ??? */
	CLOCK_OSC_LSI,       /*!< ?? 32KHz RC ??? */
	CLOCK_OSC_HSE,       /*!< ?????? */
	CLOCK_OSC_LSE,       /*!< ??????  */
	CLOCK_OSC_PLL,       /*!< ?? PLL */
	CLOCK_OSC_MAX        /*!< ???,?????? */
}clock_osc_e;

typedef enum
{
	CLOCK_MCO_DIV1 = 0U,
	CLOCK_MCO_DIV2,
	CLOCK_MCO_DIV4,
	CLOCK_MCO_DIV8,
	CLOCK_MCO_DIV16,
	CLOCK_MCO_DIV32,
	CLOCK_MCO_DIV64,
	CLOCK_MCO_DIV128,
	CLOCK_MCO_DIV256,
}clock_mco_div;


typedef enum
{
	CLOCK_SYS_FCLK_DIV1 = 0U,
	CLOCK_SYS_FCLK_DIV2,
	CLOCK_SYS_FCLK_DIV3,
	CLOCK_SYS_FCLK_DIV4,
	CLOCK_SYS_FCLK_DIV5,
	CLOCK_SYS_FCLK_DIV6,
	CLOCK_SYS_FCLK_DIV7,
	CLOCK_SYS_FCLK_DIV8,
	CLOCK_SYS_FCLK_DIV9,
	CLOCK_SYS_FCLK_DIV10,
	CLOCK_SYS_FCLK_DIV11,
	CLOCK_SYS_FCLK_DIV12,
	CLOCK_SYS_FCLK_DIV13,
	CLOCK_SYS_FCLK_DIV14,
	CLOCK_SYS_FCLK_DIV15,
	CLOCK_SYS_FCLK_DIV16
}clock_sys_fclk_div;

typedef enum
{
	CLOCK_SYS_PCLK_DIV1 = 0U,
	CLOCK_SYS_PCLK_DIV2,
	CLOCK_SYS_PCLK_DIV3,
	CLOCK_SYS_PCLK_DIV4,
	CLOCK_SYS_PCLK_DIV5,
	CLOCK_SYS_PCLK_DIV6,
	CLOCK_SYS_PCLK_DIV7,
	CLOCK_SYS_PCLK_DIV8,
	CLOCK_SYS_PCLK_DIV9,
	CLOCK_SYS_PCLK_DIV10,
	CLOCK_SYS_PCLK_DIV11,
	CLOCK_SYS_PCLK_DIV12,
	CLOCK_SYS_PCLK_DIV13,
	CLOCK_SYS_PCLK_DIV14,
	CLOCK_SYS_PCLK_DIV15,
	CLOCK_SYS_PCLK_DIV16
}clock_sys_pclk_div;



/** peripheral clock divider */
typedef enum
{
	PER_DIVIDE_1 = 1U,
	PER_DIVIDE_2,
	PER_DIVIDE_3,
	PER_DIVIDE_4,
	PER_DIVIDE_5,
	PER_DIVIDE_6,
	PER_DIVIDE_7,
	PER_DIVIDE_8,
	PER_DIVIDE_9,
	PER_DIVIDE_10,	
	PER_DIVIDE_11,	
	PER_DIVIDE_12,	
	PER_DIVIDE_13,	
	PER_DIVIDE_14,		
	PER_DIVIDE_15,
	PER_DIVIDE_16,	
}peripheral_clk_div_e;


/** peripheral clock & reset control peripheral list */
typedef enum
{
	RCC_PERIPH_FLC = 0,
	RCC_PERIPH_GPIOA,
	RCC_PERIPH_GPIOB,
	RCC_PERIPH_GPIOC,
	RCC_PERIPH_ADC0,
	RCC_PERIPH_MATH,
	RCC_PERIPH_HALL,
	RCC_PERIPH_QEP,
	RCC_PERIPH_IWDT,
	RCC_PERIPH_UART0,
	RCC_PERIPH_UART1,
	RCC_PERIPH_SPI0,
	RCC_PERIPH_SPI1,
	RCC_PERIPH_IIC0,
	RCC_PERIPH_IIC1,
	RCC_PERIPH_TIM13,
	RCC_PERIPH_TIM14,
	RCC_PERIPH_TIM15,
	RCC_PERIPH_TIM16,
	RCC_PERIPH_MCPWM0,
	RCC_PERIPH_MAX
} rcc_periph_e;


/** peripheral functional clock source (PCKS0 register encoding) */
typedef enum
{
	PERIPH_CLK_SRC_HSI  = 1U, /*!< 01: HSI CLK */
	PERIPH_CLK_SRC_EOSC = 2U, /*!< 10: EOSC (HSE) CLK */
	PERIPH_CLK_SRC_PLL  = 3U  /*!< 11: PLL CLK */
} periph_clk_src_e;

/** peripheral whose functional clock source can be selected via PCKS0 */
typedef enum
{
	RCC_CLKSRC_ADC0 = 0U,
	RCC_CLKSRC_MCPWM0,
	RCC_CLKSRC_CAN0,
	RCC_CLKSRC_MAX
} rcc_clksrc_periph_e;

/** system reset source enable (SYSRSTE), mapped to corresponding bit */
typedef enum
{
	CLOCK_RST_SRC_LVD = 0U,      /*!< LVD ????, SYSRSTE bit[2] */
	CLOCK_RST_SRC_LOCKUP,        /*!< CPU Core Lockup ????, SYSRSTE bit[10] */
	CLOCK_RST_SRC_IWDT,          /*!< ?????????, SYSRSTE bit[16] */
	CLOCK_RST_SRC_HSE_FAIL,      /*!< HSE ??????, SYSRSTE bit[21] */
	CLOCK_RST_SRC_PLL_UNLOCK,    /*!< PLL ??????, SYSRSTE bit[22] */
	CLOCK_RST_SRC_MAX
} clock_rst_src_e;

/** system reset source flag (SYSRSTSR), bit mask, rw1c (write 1 clear) */
typedef enum
{
	CLOCK_RST_FLAG_POR        = (1UL << 0U),  /*!< ???? (Power-On Reset) */
	CLOCK_RST_FLAG_BOR        = (1UL << 1U),  /*!< ???? (Brown-Out Reset) */
	CLOCK_RST_FLAG_LVD        = (1UL << 2U),  /*!< ??????? (LVD Reset) */
	CLOCK_RST_FLAG_PIN        = (1UL << 8U),  /*!< ?????? (NRST Pin Reset) */
	CLOCK_RST_FLAG_SYSRSTREQ  = (1UL << 9U),  /*!< ???????? */
	CLOCK_RST_FLAG_LOCKUP     = (1UL << 10U), /*!< CPU Core Lockup ?? */
	CLOCK_RST_FLAG_IWDT       = (1UL << 16U), /*!< ??????? */
	CLOCK_RST_FLAG_HSE_FAIL   = (1UL << 21U), /*!< HSE ?????? */
	CLOCK_RST_FLAG_PLL_UNLOCK = (1UL << 22U), /*!< PLL ???? */
	CLOCK_RST_FLAG_RETRIM     = (1UL << 24U), /*!< Re-trim ?? */
	CLOCK_RST_FLAG_ALL = CLOCK_RST_FLAG_POR | CLOCK_RST_FLAG_BOR | CLOCK_RST_FLAG_LVD |
	                     CLOCK_RST_FLAG_PIN | CLOCK_RST_FLAG_SYSRSTREQ | CLOCK_RST_FLAG_LOCKUP |
	                     CLOCK_RST_FLAG_IWDT | CLOCK_RST_FLAG_HSE_FAIL |
	                     CLOCK_RST_FLAG_PLL_UNLOCK | CLOCK_RST_FLAG_RETRIM
} clock_rst_flag_e;

/*rcc_drv_sys_clock_full_config() */
#define RCC_PLL_LOCK_TIMEOUT_US   (1000U)


/** osc configure settings */
typedef struct
{
	osc_type_e osc_type;     /*!< osc type */
	clk_ctl_e clk_ctl;       /*!< enable/disable osc */
	uint32_t osc_fre; /*!< osc frequency range: 8-40MHz */
}osc_cfg_t;


/** system clock configure settings */
typedef struct
{
	clock_sys_select sys_clk_sel; /*!< system clock source select */
	clock_sys_fclk_div fclk_div;  /*!< system clock fclk divider */
	clock_sys_pclk_div pclk_div;  /*!< system clock pclk divider */
	uint8_t pll_mult;              /*!< PLL multiplier, valid when sys_clk_sel_g is PLLCLK */
	uint8_t pll_div;               /*!< PLL output divider, valid when sys_clk_sel_g is PLLCLK */
}sys_clk_config_t;

#define		PLL_DIV_1		3U		/* PLL Clock Divided by 1 (no Division) 		*/
#define		PLL_DIV_2		2U		/* PLL Clock Divided by 2 (PLL clock ¡Â 2) 		*/
#define		PLL_DIV_4		1U		/* PLL Clock Divided by 4 (PLL clock ¡Â 4) 		*/
#define		PLL_DIV_8		0U		/* PLL Clock Divided by 8 (PLL clock ¡Â 8) 		*/

#define		PLL_MUTL_5		0U		/* PLL Clock Multiplied by 5  (PLL clock x5) 	*/
#define		PLL_MUTL_6		1U		/* PLL Clock Multiplied by 6  (PLL clock x6) 	*/
#define		PLL_MUTL_7		2U		/* PLL Clock Multiplied by 7  (PLL clock x7) 	*/
#define		PLL_MUTL_8		3U		/* PLL Clock Multiplied by 8  (PLL clock x8) 	*/
#define		PLL_MUTL_9		4U		/* PLL Clock Multiplied by 9  (PLL clock x9) 	*/
#define		PLL_MUTL_10		5U		/* PLL Clock Multiplied by 10 (PLL clock x10) 	*/
#define		PLL_MUTL_11		6U		/* PLL Clock Multiplied by 11 (PLL clock x11) 	*/
#define		PLL_MUTL_12		7U		/* PLL Clock Multiplied by 12 (PLL clock x12) 	*/
#define		PLL_MUTL_13		8U		/* PLL Clock Multiplied by 13 (PLL clock x13) 	*/
#define		PLL_MUTL_14		9U		/* PLL Clock Multiplied by 14 (PLL clock x14) 	*/
#define		PLL_MUTL_15		10U		/* PLL Clock Multiplied by 15 (PLL clock x15) 	*/
#define		PLL_MUTL_16		11U		/* PLL Clock Multiplied by 16 (PLL clock x16) 	*/
#define		PLL_MUTL_17		12U		/* PLL Clock Multiplied by 17 (PLL clock x17) 	*/
#define		PLL_MUTL_18		13U		/* PLL Clock Multiplied by 18 (PLL clock x18) 	*/
#define		PLL_MUTL_19		14U		/* PLL Clock Multiplied by 19 (PLL clock x19) 	*/
#define		PLL_MUTL_20		15U		/* PLL Clock Multiplied by 20 (PLL clock x20) 	*/



/** peripheral clock configure settings */
typedef struct
{
	module_name_e module_name;
	/** functional clock source sel */
	fclk_sel_e fclk_sel;
	/** control PCLK gate in deep sleep mode, 0:disable gating 1:enable gating */
	clk_dsleep_gating_en_e pclk_dsleep_gating_en;
	/** control FCLK gate in deep sleep mode, 0:disable gating 1:enable gating */
	clk_dsleep_gating_en_e fclk_dsleep_gating_en;
	/** functional clock divider, fclk_sel&fclk_dsleep_gating_en&fclk_div is only
	  * compliant to module with function clock settings */
	peripheral_clk_div_e fclk_div;
}module_clk_cfg_t;



#endif
