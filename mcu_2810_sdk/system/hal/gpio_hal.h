#ifndef __GPIO_HAL_H__
#define __GPIO_HAL_H__

#include "gpio_type.h"
#include "gpio_reg.h"

// 初始化 GPIO（根据配置结构体）
void gpio_init(gpio_cfg_t *cfg);

// 基本 GPIO 操作
void gpio_output_set(gpio_port_e port, gpio_pin_e pin, gpio_level_e level);
gpio_level_e gpio_input_get(gpio_port_e port, gpio_pin_e pin);
void gpio_toggle(gpio_port_e port, gpio_pin_e pin);

// 独立配置函数（通常由 gpio_init 内部调用）
void gpio_analog_mode_cfg(gpio_port_e port, gpio_pin_e pin, gpio_analog_mode_e mode);
void gpio_alt_func_cfg(gpio_port_e port, gpio_pin_e pin, gpio_alt_func_e alt);
void gpio_pull_cfg(gpio_port_e port, gpio_pin_e pin, gpio_pull_cfg_e pull);
void gpio_strength_cfg(gpio_port_e port, gpio_pin_e pin, gpio_strength_e strength);
void gpio_input_mode_cfg(gpio_port_e port, gpio_pin_e pin, gpio_input_mode_e mode);
void gpio_direction_cfg(gpio_port_e port, gpio_pin_e pin, gpio_dir_e dir);

// 中断相关函数
void gpio_int_config(gpio_cfg_t *cfg);                // 使用 gpio_cfg_t 包含中断配置
void gpio_int_enable(gpio_port_e port, gpio_pin_e pin);
void gpio_int_disable(gpio_port_e port, gpio_pin_e pin);
void gpio_int_clear_flag(gpio_port_e port, gpio_pin_e pin);
bool gpio_int_get_flag(gpio_port_e port, gpio_pin_e pin);
void gpio_int_irq_handler(gpio_port_e port);          // 统一中断处理函数
#endif  // __GPIO_HAL_H__
