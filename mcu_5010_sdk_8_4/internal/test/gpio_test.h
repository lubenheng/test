#ifndef GPIO_TEST_H
#define GPIO_TEST_H

#include "gpio_type.h"

#define PORT_B_START_PIN GPIO_PIN_2
#define MAX_TOGGLE 3




#define GPIO_PAD_NUM 64U
static gpio_port_e g_gpio_port_arr[GPIO_PAD_NUM] = {
	GPIO_A, /*1 PTD1*/

};


typedef struct
{
	uint8_t toggle_times;
	bool is_test_finish;
}gpio_output_status_t;

typedef struct
{
	gpio_port_e gpio_input_port;
	gpio_port_e gpio_output_port;
	gpio_pin_e current_pin;
}gpio_test_info_t;


typedef enum
{
	PORT_NORMAL	= 0,
	PORT_B,
	PORT_E,
	PORT_B_E_BOTH
}gpio_port_check_e;

typedef enum
{
	PRINT_ERROR_INT = 0, /*cannot rececive intterrupt*/
	PRINT_WRONG_INT	/*interrupt parse is not expected*/
}print_type_e;

void gpio_clk_init_all(void);
void gpio_test(gpio_port_e gpio_port1, gpio_port_e gpio_port2);
void gpio_interrupt_next_cfg(void);
void gpio_a5_init(void);

void gpio_test_each_pad(gpio_port_e gpio_port, gpio_pin_e gpio_pin);
void gpio_test_connection(void);
void gpio_test_shortage(void);

void gpio_a7_init_level_cfg(gpio_level_e lvl_set);
void gpio_a7_interrupt_trigger(void);



void gpio_manual_test_int_low_lvl(void);

void gpio_default_interrupt_type_test(void);
void gpio_default_interrupt_type_test_waveform_set(void);


void group_a_gpio_toggle_init(void);
void group_b_gpio_toggle_init(void);
void group_c_gpio_toggle_init(void);
void group_gpio_toggle(gpio_port_e gpio_port);
void group_gpio_output_set(gpio_port_e gpio_port,gpio_level_e level);


void hall_uvw_gpio_init(void);
void hall_abz_gpio_init(void);

/* 中断回调函数 */
void gpio_a_callback_test(gpio_int_source_e source);
void gpio_b_callback_test(gpio_int_source_e source);
void gpio_c_callback_test(gpio_int_source_e source);

#endif

