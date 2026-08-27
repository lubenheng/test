#ifndef SYS_CONFIG_H
#define SYS_CONFIG_H


/* OCS_Reserved_Start */

/**********************
******Chip Mode Option ******
**********************/
//#define	ASIC_MODE
#define FPGA_TMP_CODE /*TODO: used to simplify coding temporary, must fix in future*/

/* OCS_Reserved_End */




/* OCS_Reserved_Start */
/**********************
******Chip Revision ******
**********************/
#define REVISION_A		(0x01U<<0U)  /* mean REVISION_0 */
#define REVISION_B		(0x01U<<1U)  /* mean REVISION_1 */
#define REVISION_C		(0x01U<<2U)  /* mean REVISION_2 */
#define REVISION_D		(0x01U<<3U)  /* mean REVISION_3 */

#define	CHIP_REVISION	(REVISION_A)
/* OCS_Reserved_End */


/**********************
******Chip Package Option ******
**********************/

#define	PACKAGE_64PIN		(0x01U<<0U)
#define	PACKAGE_48PIN		(0x01U<<1U)
#define	PACKAGE_32PIN		(0x01U<<2U)
#define	PACKAGE_24PIN		(0x01U<<3U)


#define CHIP_PACKAGE		PACKAGE_64PIN

/**************************
**** Chip power options*******
**************************/
#define	VDD_5V	(0x01u<<1)
#define	VDD_3V3	(0x01u<<2)

#define POWER_SUPPLY	VDD_3V3


/**************************
**** Clock Options*******
**************************/
/* OCS_Reserved_Start */
#ifdef ASIC_MODE
/* OCS_Reserved_End */
/* HIRC FREQ: 72MHz */
#define HIRC_FREQ			72000000U
/* OCS_Reserved_Start */
#else

/* HIRC FREQ: 32MHz on FPGA*/
#define HIRC_FREQ			32000000U
#define PLL_FREQ            60000000U

#define PLL_48M             48000000U
#define PLL_64M             64000000U
#define PLL_96M             96000000U

#define SYS_CLK             HIRC_FREQ



#endif
/* OCS_Reserved_End */

/* OSC:8-40MHz */
#define OSC_CURRENT_FREQ	40000000U

#define SYS_CORE_CLK		HIRC_FREQ /*M0 core clock which default source from HIRC*/

/* OCS_Reserved_Start */
#ifdef ASIC_MODE
/* OCS_Reserved_End */
#define LIRC_FREQ			32051UL
/* OCS_Reserved_Start */
#else
#define LIRC_FREQ			32051U
//#define FPGA_HIRC_40M
#endif
/* OCS_Reserved_End */



/**************************
**** Flash Options*******
**************************/


/**************************
**** UART Options*******
**************************/
#define UART
#ifdef UART
#define UART_CONSOLE_INSTANCE (UART_INSTANCE0)
#define UART_CONSOLE_INSTANCE1 (UART_INSTANCE1)
#define CMDLINE_SUPPORT

#define MSG_INFO
//#define ERR_INFO
//#define DEV_ERROR_DETECT

#endif /*UART*/


/**************************
**** MCU_TYPE *******
**************************/
#define MCU_ODS5010 (0x01U<<0U)
#define MCU_ODS5110 (0x01U<<1U)


/*MCU_PARTNUMBER_<partnumber> is defined in project*/ 
#if defined(CPU_ODS5010)
#define MCU_TYPE	MCU_ODS5010
#elif defined(CPU_ODS5110)
#define MCU_TYPE	MCU_ODS5110
#else
#error 'invalid cpu type!'
#endif



#endif /* SYS_CONFIG_H */
 
