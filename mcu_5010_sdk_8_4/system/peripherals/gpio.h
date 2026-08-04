#ifndef GPIO_H
#define GPIO_H


void gpio_drv_init_to_default(PORT_TypeDef *gpio_ptr);
void gpio_drv_direction_cfg(PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin, gpio_dir_e gpio_dir);
void gpio_drv_output_mode_cfg(PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin, gpio_output_mode_e gpio_output_mode);
void gpio_drv_input_mode_cfg(PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin, gpio_input_mode_e gpio_input_mode);
void gpio_drv_pull_cfg(PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin, gpio_pull_cfg_e pull_cfg);
void gpio_drv_interrupt_cfg(PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin, const gpio_interrupt_cfg_info_t *gpio_interrupt_cfg_info);
gpio_interrupt_type_e gpio_drv_interrupt_type_get(const PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin);
void gpio_drv_alterfunc_cfg(PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin, gpio_alt_e gpio_alt);


void gpio_drv_output_set(PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin, gpio_level_e level);
bool gpio_drv_output_get(const PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin);
gpio_level_e gpio_drv_input_get(const PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin);
void gpio_drv_toggle(PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin);


#endif
