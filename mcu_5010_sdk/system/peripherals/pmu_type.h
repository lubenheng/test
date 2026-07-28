#ifndef PMU_TYPE_H
#define PMU_TYPE_H

/** CFG register default value*/
#define PMU_CFG_INIT 0x5005BF00
#define HIB_GPIO_WAKE_MODE_MASK 0x07U
#define HIB_GPIO_WAKE_WIDTH_MASK 0x03U

#define PMU_GPIOWAKE_MODE_POS        ((uint32_t)0x3)
#define PMU_GPIOWAKE_WIDTH_POS        ((uint32_t)0x2)

typedef void (*PMU_INT_HANDLE)(void);

/** digital LDO voltage*/
typedef enum
{
	VDDCORE_1P2 = 0UL,
	VDDCORE_1P25,
	VDDCORE_1P3,
	VDDCORE_1P15,
}pmu_ldo1p2_sel_e;

/** interrupt type*/
typedef enum
{
	HIB_INT_DISABLE = 0UL,
	HIB_INT_ENABLE  = 1UL,
}hib_interrupt_enable;


/** rtc clock*/
typedef enum
{
	PMU_RTC_CLK_LIRC_DIV4 = 0UL,	/*!< 131.072/4 = 32.768KHZ */
	PMU_RTC_CLK_IO_INPUT		/*!< PTA7 */
}pmu_rtc_clk_sel_e;

/** length of pbtn low time to wake from hibernate*/
typedef enum
{
	HIB_PBTN_125MS = 0UL,
	HIB_PBTN_250MS,
	HIB_PBTN_500MS,
	HIB_PBTN_1S,
	HIB_PBTN_2S,
	HIB_PBTN_4S,
	HIB_PBTN_8S,
	HIB_PBTN_10MS,
	HIB_PBTN_WAKE_INVALID
}hib_pbtn_wake_e;

/** length of gpio level to wake from hibernate*/
typedef enum
{
	HIB_GPIO_WAKE_75US = 0UL,	/*!< LIRC 10clk cycle*/
	HIB_GPIO_WAKE_150US,		/*!< LIRC 20clk cycle*/
	HIB_GPIO_WAKE_500US,		/*!< LIRC 64clk cycle*/
	HIB_GPIO_WAKE_1MS,			/*!< LIRC 128clk cycle*/
}hib_gpio_wake_width_e;

/** gpio wake type to wake from hibernate*/
typedef enum
{
	//HIB_GPIO_WAKE_ASYNC_RISING_EDGE = 0UL,		/*!< no requirment for level width after rising edge*/
	//HIB_GPIO_WAKE_ASYNC_FALLING_EDGE,			/*!< no requirment for level width after falling edge*/
	
	// HIB_GPIO_WAKE_SYNC_FALLING_EDGE,			/*!< low level over 1lirc clk after falling edege*/
	// HIB_GPIO_WAKE_SYNC_RISING_EDGE,			/*!< high level over 1lirc clk after rising edege*/
	
	HIB_GPIO_WAKE_HIGH_LEVEL = 0UL,
	HIB_GPIO_WAKE_LOW_LEVEL,
}hib_gpio_wake_mode_e;

/** gpio source to wake from hibernate*/
typedef enum
{
	HIB_GPIO_WAKE_SOURCE_PTA3 = 0UL,
	HIB_GPIO_WAKE_SOURCE_PTA4,
	HIB_GPIO_WAKE_SOURCE_PTA13,
	HIB_GPIO_WAKE_SOURCE_PTA14,
	HIB_GPIO_WAKE_SOURCE_PTB2,
	HIB_GPIO_WAKE_SOURCE_PTB3,
	HIB_GPIO_WAKE_SOURCE_PTB4,
	HIB_GPIO_WAKE_SOURCE_PTB5,
	HIB_GPIO_WAKE_SOURCE_PTB12,
	HIB_GPIO_WAKE_SOURCE_PTB13,
	HIB_GPIO_WAKE_SOURCE_PTC3,
    HIB_GPIO_WAKE_SOURCE_PTC4,

	HIB_GPIO_WAKE_SOURCE_MAX,
}hib_gpio_wake_source_e;

/** pbtn interrupt type*/
typedef enum
{
	HIB_INT_PBTN_NEGEDGE_EN = 1UL,
	HIB_INT_PBTN_POSEDGE_EN = 2UL,
	HIB_INT_PBTN_EITHER_EN = 4UL,
}hib_int_type_e;

typedef struct
{
	gpio_port_e gpio_port;
	gpio_pin_e gpio_pin;
}hib_gpio_info_t;


typedef struct
{
	hib_gpio_wake_source_e gpio_wake_source;
	hib_gpio_wake_mode_e gpio_wake_mode;
	hib_interrupt_enable gpio_wake_interrupt;
}hib_gpio_wake_cfg_t;


typedef struct
{
	uint32_t wakeup_time_second;
	hib_interrupt_enable lptimer_wake_interrupt;
}hib_lptimer_wake_cfg_t;

/** hibernate config*/
typedef struct
{
	bool is_pbtn_enter_hib;				/*!< if pbtn can enter hibernate*/

	bool is_timer_exit_hib;				/*!< if timer can exit hibernate*/
	uint32_t pbtn_deglitch;
	uint32_t wake_lirc_clock;
	bool is_rtc_exit_hib;				/*!< if RTC can exit hibernate*/

	bool is_pbtn_exit_hib;				/*!< if pbtn can exit hibernate*/
	hib_pbtn_wake_e pbtn_wake_time;
	
	PMU_INT_HANDLE pmu_interrupt_handle; /*!< pmu interrupt handle function*/
	uint16_t pmu_interrupt_priority;	/*!< pmu interrupt priority*/	
}hib_cfg_t;


#endif /*  End of PMU_REG_H */

