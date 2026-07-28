#ifndef GPIO_CMDLINE_H
#define GPIO_CMDLINE_H
#include "gpio_hal.h" 


void GPIO_Output_Init(gpio_port_e GPIO_PORT,gpio_pin_e GPIO_PIN);

int cmd_gpio_toggle_test(int argc, char *argv[]);
int cmd_gpio_port_toggle_test(int argc, char *argv[]);
int cmd_gpio_port_all_test(int argc, char *argv[]);
void GPIO_INput_Init(gpio_port_e GPIO_PORT,gpio_pin_e GPIO_PIN);
void GPIO_Output_Init(gpio_port_e GPIO_PORT,gpio_pin_e GPIO_PIN);

#define GPIO_CMDLINE_MENU \
		{"cmd_gpio_toggle_test",cmd_gpio_toggle_test,  " cmd_gpio_toggle_test"}, \
		{"cmd_gpio_port_toggle_test",cmd_gpio_port_toggle_test,  " cmd_gpio_port_toggle_test"}, \
		{"cmd_gpio_port_all_test",cmd_gpio_port_all_test,  " cmd_gpio_port_all_test"}, \


#endif
