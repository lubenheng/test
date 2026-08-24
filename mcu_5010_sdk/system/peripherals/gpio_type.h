#ifndef GPIO_TYPE_H
#define GPIO_TYPE_H

#include "stdint.h"


/**
 * @defgroup ingroup_gpio_hal GPIO Hal
 * @ingroup group_gpio_hal
 * @details This section describes the programming interface of the gpio module.
 * @{
 */

/** GPIO interrupt types*/
typedef enum{
    GPIO0_INT = 0U,                  /*!< 0,  GPIO0 发生中断事件  */
    GPIO1_INT,                       /*!< 1,  GPIO1 发生中断事件  */
    GPIO2_INT,                       /*!< 2,  GPIO2 发生中断事件  */
    GPIO3_INT,                       /*!< 3,  GPIO3 发生中断事件  */
    GPIO4_INT,                       /*!< 4,  GPIO4 发生中断事件  */
    GPIO5_INT,                       /*!< 5,  GPIO5 发生中断事件  */
    GPIO6_INT,                       /*!< 6,  GPIO6 发生中断事件  */
    GPIO7_INT,                       /*!< 7,  GPIO7 发生中断事件  */
    GPIO8_INT,                       /*!< 8,  GPIO8 发生中断事件  */
    GPIO9_INT,                       /*!< 9,  GPIO9 发生中断事件  */
    GPIO10_INT,                      /*!< 10, GPIO10 发生中断事件 */
    GPIO11_INT,                      /*!< 11, GPIO11 发生中断事件 */
    GPIO12_INT,                      /*!< 12, GPIO12 发生中断事件 */
    GPIO13_INT,                      /*!< 13, GPIO13 发生中断事件 */
    GPIO14_INT,                      /*!< 14, GPIO14 发生中断事件 */
    GPIO15_INT,                      /*!< 15, GPIO15 发生中断事件 */
    GPIO_INT_Fault,                  /*!< 16,未使用 */
} gpio_int_source_e;


typedef void (*GPIO_INT_HANDLE)(gpio_int_source_e);



typedef enum
{
        GPIO_A = 0U,
        GPIO_B,
        GPIO_C,
        GPIO_PORT_MAX
}gpio_port_e;

typedef enum
{
        GPIO_PIN_0 = 0UL,
        GPIO_PIN_1,
        GPIO_PIN_2,
        GPIO_PIN_3,
        GPIO_PIN_4,
        GPIO_PIN_5,
        GPIO_PIN_6,
        GPIO_PIN_7,
        GPIO_PIN_8,
        GPIO_PIN_9,
        GPIO_PIN_10,
        GPIO_PIN_11,
        GPIO_PIN_12,
        GPIO_PIN_13,
        GPIO_PIN_14,
        GPIO_PIN_15,
        GPIO_PIN_NUM_MAX
}gpio_pin_e;



typedef enum
{
    GPIO_ALT_FUNC_0 = 0UL,
    GPIO_ALT_FUNC_1,
    GPIO_ALT_FUNC_2,
    GPIO_ALT_FUNC_3,
    GPIO_ALT_FUNC_4,
    GPIO_ALT_FUNC_5,
    GPIO_ALT_FUNC_6,
    GPIO_ALT_FUNC_7,
    GPIO_ALT_FUNC_8,
    GPIO_ALT_FUNC_9,
    GPIO_ALT_FUNC_10,
    GPIO_ALT_FUNC_11,
    GPIO_ALT_FUNC_12,
    GPIO_ALT_FUNC_13,
    GPIO_ALT_FUNC_14,
    GPIO_ALT_FUNC_15
}gpio_alt_e;

typedef enum
{
    GPIO_INT_TYPE_LOW_LEVEL = 0U,
    GPIO_INT_TYPE_HIGH_LEVEL,
	GPIO_INT_TYPE_FALLING_EDGE,
    GPIO_INT_TYPE_RISING_EDGE,
    GPIO_INT_TYPE_EITHER_EDGE,
    GPIO_INT_TYPE_NUM_MAX
}gpio_interrupt_type_e;



/** gpio direction*/
typedef enum
{
    GPIO_DIR_INPUT = 0U,
    GPIO_DIR_OUTPUT
}gpio_dir_e;

/** gpio output mode*/
typedef enum
{
    GPIO_OUTPUT_MODE_PUSH_PULL = 0U,
    GPIO_OUTPUT_MODE_OPEN_DRAIN,
    GPIO_OUTPUT_MODE_OPEN_SOURCE,
    GPIO_OUTPUT_MODE_MAX
}gpio_output_mode_e;

/** gpio input mode*/
typedef enum
{
    GPIO_INPUT_MODE_SCHMITT = 0U,/* hysteresis */
    GPIO_INPUT_MODE_CMOS,/* non-hysteresis */
    GPIO_INPUT_MODE_MAX
}gpio_input_mode_e;

/** voltage level*/
typedef enum
{
    GPIO_LOW_LEVEL = 0U,
    GPIO_HIGH_LEVEL
}gpio_level_e;

/** gpio current strength*/
/** strength increases from "_LOW_SLOW" to "_HIGH_FAST" in the enum below*/
typedef enum
{
    GPIO_STRENGTH_HIGH_Z = 0U,
    GPIO_STRENGTH_MODE_1,
    GPIO_STRENGTH_MODE_2,
    GPIO_STRENGTH_MODE_3,
    GPIO_STRENGTH_MODE_4,
    GPIO_STRENGTH_MODE_5,
    GPIO_STRENGTH_MODE_6,
    GPIO_STRENGTH_MODE_7,
    GPIO_STRENGTH_MAX
}gpio_strength_e;

typedef enum
{
    GPIO_PULL_UP = 0U,
    GPIO_PULL_UP_PLUS,
    GPIO_PULL_DOWN,
    GPIO_PULL_DISABLE
}gpio_pull_cfg_e;

//typedef void (*GPIO_INT_HANDLE)(uint32_t pin_number);

/** gpio interrupt info*/
typedef struct
{
    gpio_interrupt_type_e interrupt_type;
    GPIO_INT_HANDLE int_handle;
    uint32_t priority;                        /*!< interrupt priority*/
}gpio_interrupt_cfg_info_t;

/** gpio config info*/
typedef struct{
    gpio_port_e gpio_port;
    gpio_pin_e gpio_pin;
    gpio_alt_e gpio_alt;/*!< gpio alternal function number*/
    gpio_dir_e gpio_dir;/*!< gpio direction*/
    gpio_output_mode_e gpio_output_mode;/*!< gpio output mode*/
    gpio_pull_cfg_e pull_cfg;
    gpio_strength_e gpio_strength;/*!< gpio current strength*/
    
    gpio_interrupt_cfg_info_t gpio_interrupt_cfg_info;/*!< gpio interrupt info*/
}gpio_settings_config_t;




#endif
