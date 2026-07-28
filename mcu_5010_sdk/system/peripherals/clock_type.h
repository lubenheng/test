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
	ERC_CLK_FR_SEL_INVALID     /*!< frclk enum value is invalid */
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
	/** system clk source */
	sys_clk_src_e src;

	/** osc configure */
	osc_cfg_t osc_cfg;
	
	/** system clk div system frequency = system frequency/sys_div */
	uint8_t sys_div; 	/*!< divider, Valid range is 1-128 */
	/** apb0 clk = sys clk/apb0_div */
	uint8_t apb0_div;/*!< divider, Valid range is 2-128 */
	/** apb1 clk = sys clk/apb1_div */
	uint8_t apb1_div;/*!< divider, Valid range is 2-128 */
}sys_clk_config_t;


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
