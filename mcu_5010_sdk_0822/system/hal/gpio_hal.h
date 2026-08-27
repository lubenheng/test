#ifndef GPIO_HAL_H
#define GPIO_HAL_H

                        

#include "gpio_type.h"
#include "gpio_reg.h"

void gpio_init(gpio_settings_config_t *gpio_settings_config);


// void gpio_init_to_default(void);
 void gpio_direction_cfg(gpio_port_e gpio_port, gpio_pin_e gpio_pin, gpio_dir_e gpio_dir);
// void gpio_output_mode_cfg(gpio_port_e gpio_port, gpio_pin_e gpio_pin, gpio_output_mode_e gpio_output_mode);
// void gpio_input_mode_cfg(gpio_port_e gpio_port, gpio_pin_e gpio_pin, gpio_input_mode_e gpio_input_mode);
 void gpio_pull_cfg(gpio_port_e gpio_port, gpio_pin_e gpio_pin, gpio_pull_cfg_e pull_cfg);
 void gpio_output_set(gpio_port_e gpio_port, gpio_pin_e gpio_pin, gpio_level_e level);
// bool gpio_output_get(gpio_port_e gpio_port, gpio_pin_e gpio_pin);
// gpio_level_e gpio_input_get(gpio_port_e gpio_port, gpio_pin_e gpio_pin);
 void gpio_interrupt_cfg(gpio_port_e gpio_port, gpio_pin_e gpio_pin, gpio_interrupt_cfg_info_t *gpio_interrupt_cfg_info);
// bool gpio_interrupt_get_flag(gpio_port_e gpio_port, gpio_pin_e gpio_pin);
// void gpio_interrupt_disable(gpio_port_e gpio_port, gpio_pin_e gpio_pin);
// void gpio_interrupt_flag_clear(gpio_port_e gpio_port, gpio_pin_e gpio_pin);
// gpio_interrupt_type_e gpio_interrupt_type_get(gpio_port_e gpio_port, gpio_pin_e gpio_pin);
 void gpio_toggle(gpio_port_e gpio_port, gpio_pin_e gpio_pin);
 void gpio_alterfunc_cfg(gpio_port_e gpio_port, gpio_pin_e gpio_pin, gpio_alt_e gpio_alt);
 void gpio_strength_cfg(gpio_port_e gpio_port, gpio_pin_e gpio_pin, gpio_strength_e gpio_strength);

// void gpio_a7_init(void);
// void pins_mux_init(uint32_t pinCount, gpio_settings_config_t *config);
// void gpio_pin_cfg_init(void);

gpio_int_source_e gpio_drv_status_flag_get(PORT_TypeDef *gpio_ptr);


#endif
