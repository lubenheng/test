#include "gpio_hal.h"

#include "gpio_type.h"   

#include "gpio_reg.h"

#include <assert.h>

#include <stddef.h>



// 端口有效性检查（内部函数）

static bool gpio_port_valid(gpio_port_e port) {

    return (port < GPIO_PORT_MAX);

}



// 引脚有效性检查（内部函数）

static bool gpio_pin_valid(gpio_port_e port, gpio_pin_e pin) {

    if (port >= GPIO_PORT_MAX || pin >= GPIO_PIN_NUM_MAX) return false;

    if (port == GPIO_PORT_P4 && pin >= 6) return false;  // P4仅支持0~5

    return true;

}



// 1. 模拟模式配置（对应IO->ANA寄存器）

void gpio_analog_mode_cfg(gpio_port_e port, gpio_pin_e pin, gpio_analog_mode_e mode) {

    assert(gpio_port_valid(port) && gpio_pin_valid(port, pin));



    uint32_t bit = 0;

    // 按端口计算ana寄存器的位偏移（P0:0~7, P1:8~15, P2:16~23, P3:24~31）

    switch(port){

        case GPIO_PORT_P0: bit = pin;       break;

        case GPIO_PORT_P1: bit = pin + 8;   break;

        case GPIO_PORT_P2: bit = pin + 16;  break;

        case GPIO_PORT_P3: bit = pin + 24;  break;

        default: return; // P4无模拟模式

    }



    if(mode == GPIO_ANALOG_ENABLE){

        IO_2810->ANA.w |= (1UL << bit);

    }else{

        IO_2810->ANA.w &= ~(1UL << bit);

    }

}



// 2. 复用功能配置（对应IO->EN / IO->EN2寄存器）

void gpio_alt_func_cfg(gpio_port_e port, gpio_pin_e pin, gpio_alt_func_e alt) {

    assert(gpio_port_valid(port) && gpio_pin_valid(port, pin));



    // 计算当前引脚在en/en2寄存器中的位偏移（P0:0~7, P1:8~15, P2:16~23, P3:24~31, P4:32~37）

    uint32_t bit_pos = 0;

    switch(port) {

        case GPIO_PORT_P0: bit_pos = pin;        break;

        case GPIO_PORT_P1: bit_pos = pin + 8;    break;

        case GPIO_PORT_P2: bit_pos = pin + 16;   break;

        case GPIO_PORT_P3: bit_pos = pin + 24;   break;

        case GPIO_PORT_P4: bit_pos = pin + 32;   break;

        default: return;

    }



    // 关闭当前引脚的复用功能（en对应低32位，en2对应高32位）

//    if (bit_pos < 32) {

//        IO_2810->EN.w &= ~(1UL << bit_pos);

//    } else {

//        IO_2810->EN2.w &= ~(1UL << (bit_pos - 32));

//    }



    // 关闭模拟模式，确保GPIO模式优先级

    gpio_analog_mode_cfg(port, pin, GPIO_ANALOG_DISABLE);



    // 根据alt配置对应复用功能（需根据芯片手册补充实际外设映射）

    switch (alt) {

        case GPIO_ALT_FUNC_0: // 普通GPIO模式（默认）

            break;

        case GPIO_ALT_FUNC_1: // UART示例

            if (bit_pos < 32) {

                IO_2810->EN.w |= (1UL << bit_pos);

            } else {

                IO_2810->EN2.w |= (1UL << (bit_pos - 32));

            }

            break;

        case GPIO_ALT_FUNC_2: // I2C示例

            if (bit_pos < 32) {

                IO_2810->EN.w |= (2UL << bit_pos); // 假设I2C对应en寄存器的2位偏移

            } else {

                IO_2810->EN2.w |= (2UL << (bit_pos - 32));

            }

            break;

        case GPIO_ALT_FUNC_3: // SPI示例

            if (bit_pos < 32) {

                IO_2810->EN.w |= (3UL << bit_pos); // 假设SPI对应en寄存器的3位偏移

            } else {

                IO_2810->EN2.w |= (3UL << (bit_pos - 32));

            }

            break;

        default:

            break;

    }

}



// 3. 上下拉配置（对应IO->PU/IO->PD/IO->P4R0寄存器）

void gpio_pull_cfg(gpio_port_e port, gpio_pin_e pin, gpio_pull_cfg_e pull) {

    assert(gpio_port_valid(port) && gpio_pin_valid(port, pin));



    if (port == GPIO_PORT_P4) {

        // P4上下拉：IO->P4R0[21:16]（上拉）、IO->P4R0[29:24]（下拉）

        uint32_t pu_mask = (1UL << pin) << 16;

        uint32_t pd_mask = (1UL << pin) << 24;

        switch (pull) {

            case GPIO_PULL_UP:

                IO_2810->P4R0.w |= pu_mask;

                IO_2810->P4R0.w &= ~pd_mask;

                break;

            case GPIO_PULL_DOWN:

                IO_2810->P4R0.w |= pd_mask;

                IO_2810->P4R0.w &= ~pu_mask;

                break;

            default: // 禁用上下拉

                IO_2810->P4R0.w &= ~(pu_mask | pd_mask);

                break;

        }

    } else {

        // P0~P3上下拉：IO->PU/IO->PD寄存器（位偏移：port*8 + pin）

        uint8_t bit_pos = (port << 3) + pin;

        switch (pull) {

            case GPIO_PULL_UP:

                IO_2810->PU.w |= (1UL << bit_pos);

                IO_2810->PD.w &= ~(1UL << bit_pos);

                break;

            case GPIO_PULL_DOWN:

                IO_2810->PD.w |= (1UL << bit_pos);

                IO_2810->PU.w &= ~(1UL << bit_pos);

                break;

            default: // 禁用上下拉

                IO_2810->PU.w &= ~(1UL << bit_pos);

                IO_2810->PD.w &= ~(1UL << bit_pos);

                break;

        }

    }

}



// 4. 驱动强度配置（对应IO->DS0/IO->DS1/IO->P4R0寄存器）

void gpio_strength_cfg(gpio_port_e port, gpio_pin_e pin, gpio_strength_e strength) {

    assert(gpio_port_valid(port) && gpio_pin_valid(port, pin));



    if (port == GPIO_PORT_P4) {

        // P4驱动强度：IO->P4R0[5:0]（DS0）和[13:8]（DS1）

        uint32_t mask = (1UL << pin);

        switch (strength) {

            case GPIO_STRENGTH_LOW:

                IO_2810->P4R0.Pad4_ds0 &= ~mask;

                IO_2810->P4R0.Pad4_ds1 &= ~mask;

                break;

            case GPIO_STRENGTH_MID:

                IO_2810->P4R0.Pad4_ds0 &= ~mask;

                IO_2810->P4R0.Pad4_ds1 |= mask;

                break;

            case GPIO_STRENGTH_HIGH:

                IO_2810->P4R0.Pad4_ds0 |= mask;

                IO_2810->P4R0.Pad4_ds1 |= mask;

                break;

            default:

                break;

        }

        return;

    }



    // P0~P3驱动强度：IO->DS0 + IO->DS1组合

    uint8_t bit_pos = (port << 3) + pin;

    switch (strength) {

        case GPIO_STRENGTH_LOW:

            IO_2810->DS0.w &= ~(1UL << bit_pos);

            IO_2810->DS1.w &= ~(1UL << bit_pos);

            break;

        case GPIO_STRENGTH_MID:

            IO_2810->DS0.w &= ~(1UL << bit_pos);

            IO_2810->DS1.w |= (1UL << bit_pos);

            break;

        case GPIO_STRENGTH_HIGH:

            IO_2810->DS0.w |= (1UL << bit_pos);

            IO_2810->DS1.w |= (1UL << bit_pos);

            break;

        default:

            break;

    }

}



// 5. 输入模式配置（对应IO->ST/IO->P4R1寄存器）

void gpio_input_mode_cfg(gpio_port_e port, gpio_pin_e pin, gpio_input_mode_e mode) {

    assert(gpio_port_valid(port) && gpio_pin_valid(port, pin));



    if (port == GPIO_PORT_P4) {

        // P4输入模式：IO->P4R1[21:16]

        uint32_t mask = (1UL << pin) << 16;

        if (mode == GPIO_INPUT_MODE_SCHMITT) {

            IO_2810->P4R1.w |= mask;

        } else {

            IO_2810->P4R1.w &= ~mask;

        }

    } else {

        // P0~P3输入模式：IO->ST寄存器（位偏移：port*8 + pin）

        uint8_t bit_pos = (port << 3) + pin;

        if (mode == GPIO_INPUT_MODE_SCHMITT) {

            IO_2810->ST.w |= (1UL << bit_pos);

        } else {

            IO_2810->ST.w &= ~(1UL << bit_pos);

        }

    }

}



// 6. 方向配置（对应GPIO->OUT_EN_STR/GPIO->P4_CFG寄存器）

void gpio_direction_cfg(gpio_port_e port, gpio_pin_e pin, gpio_dir_e dir) {

    assert(gpio_port_valid(port) && gpio_pin_valid(port, pin));



    if (port == GPIO_PORT_P4) {

        // P4方向：GPIO->P4_CFG.Gpio4_out_en (位21:16)

        uint32_t mask = (1UL << pin);

        if (dir == GPIO_DIR_OUTPUT) {

            GPIO_2810->P4_CFG.Gpio4_out_en |= mask;

        } else {

            GPIO_2810->P4_CFG.Gpio4_out_en &= ~mask;

        }

    } else {

        // P0~P3方向：GPIO->OUT_EN_STR寄存器对应位

        uint32_t mask = (1UL << pin);

        switch (port) {

            case GPIO_PORT_P0:

                dir == GPIO_DIR_OUTPUT ? (GPIO_2810->OUT_EN_STR.Gpio0_out_en |= mask) : 

                                        (GPIO_2810->OUT_EN_STR.Gpio0_out_en &= ~mask);

                break;

            case GPIO_PORT_P1:

                dir == GPIO_DIR_OUTPUT ? (GPIO_2810->OUT_EN_STR.Gpio1_out_en |= mask) : 

                                        (GPIO_2810->OUT_EN_STR.Gpio1_out_en &= ~mask);

                break;

            case GPIO_PORT_P2:

                dir == GPIO_DIR_OUTPUT ? (GPIO_2810->OUT_EN_STR.Gpio2_out_en |= mask) : 

                                        (GPIO_2810->OUT_EN_STR.Gpio2_out_en &= ~mask);

                break;

            case GPIO_PORT_P3:

                dir == GPIO_DIR_OUTPUT ? (GPIO_2810->OUT_EN_STR.Gpio3_out_en |= mask) : 

                                        (GPIO_2810->OUT_EN_STR.Gpio3_out_en &= ~mask);

                break;

            default:

                break;

        }

    }

}


// 7. GPIO初始化（整合所有配置项）

void gpio_init(gpio_cfg_t *cfg) {

    assert(cfg != NULL);

    assert(gpio_port_valid(cfg->port) && gpio_pin_valid(cfg->port, cfg->pin));



    // 1. 配置复用功能（优先确保GPIO模式）

    gpio_alt_func_cfg(cfg->port, cfg->pin, cfg->alt_func);



    // 2. 配置模拟模式

    gpio_analog_mode_cfg(cfg->port, cfg->pin, cfg->analog_mode);



    // 3. 配置输入模式

    gpio_input_mode_cfg(cfg->port, cfg->pin, cfg->input_mode);



    // 4. 配置上下拉

    gpio_pull_cfg(cfg->port, cfg->pin, cfg->pull);



    // 5. 配置驱动强度

    gpio_strength_cfg(cfg->port, cfg->pin, cfg->strength);



    // 6. 配置方向（输入/输出）

    gpio_direction_cfg(cfg->port, cfg->pin, cfg->dir);

    if (cfg->int_cfg.callback != NULL) {

        gpio_int_config(cfg);

    }

    

    // 7. 初始化输出电平为低

    gpio_output_set(cfg->port, cfg->pin, GPIO_LOW_LEVEL);     

}



// 8. 输出电平配置（对应GPIO->OUT_STR/GPIO->P4_CFG寄存器）

void gpio_output_set(gpio_port_e port, gpio_pin_e pin, gpio_level_e level) {

    assert(gpio_port_valid(port) && gpio_pin_valid(port, pin));



    if (port == GPIO_PORT_P4) {

        // P4输出电平：GPIO->P4_CFG.Gpio4_out (位13:8)

        uint32_t mask = (1UL << pin);

        if (level == GPIO_HIGH_LEVEL) {

            GPIO_2810->P4_CFG.Gpio4_out |= mask;

        } else {

            GPIO_2810->P4_CFG.Gpio4_out &= ~mask;

        }

    } else {

        // P0~P3输出电平：GPIO->OUT_STR寄存器对应位

        uint32_t mask = (1UL << pin);

        switch (port) {

            case GPIO_PORT_P0:

                level == GPIO_HIGH_LEVEL ? (GPIO_2810->OUT_STR.Gpio0_out |= mask) : 

                                          (GPIO_2810->OUT_STR.Gpio0_out &= ~mask);

                break;

            case GPIO_PORT_P1:

                level == GPIO_HIGH_LEVEL ? (GPIO_2810->OUT_STR.Gpio1_out |= mask) : 

                                          (GPIO_2810->OUT_STR.Gpio1_out &= ~mask);

                break;

            case GPIO_PORT_P2:

                level == GPIO_HIGH_LEVEL ? (GPIO_2810->OUT_STR.Gpio2_out |= mask) : 

                                          (GPIO_2810->OUT_STR.Gpio2_out &= ~mask);

                break;

            case GPIO_PORT_P3:

                level == GPIO_HIGH_LEVEL ? (GPIO_2810->OUT_STR.Gpio3_out |= mask) : 

                                          (GPIO_2810->OUT_STR.Gpio3_out &= ~mask);

                break;

            default:

                break;

        }

    }

}



// 9. 输入电平读取（对应GPIO->IN_STR/GPIO->P4_CFG寄存器）

gpio_level_e gpio_input_get(gpio_port_e port, gpio_pin_e pin) {

    assert(gpio_port_valid(port) && gpio_pin_valid(port, pin));



    if (port == GPIO_PORT_P4) {

        // P4输入电平：GPIO->P4_CFG.Gpio4_in (位5:0)

        return (GPIO_2810->P4_CFG.Gpio4_in & (1UL << pin)) ? GPIO_HIGH_LEVEL : GPIO_LOW_LEVEL;

    } else {

        // P0~P3输入电平：GPIO->IN_STR寄存器对应位

        uint32_t mask = (1UL << pin);

        switch (port) {

            case GPIO_PORT_P0:

                return (GPIO_2810->IN_STR.Gpio0_in & mask) ? GPIO_HIGH_LEVEL : GPIO_LOW_LEVEL;

            case GPIO_PORT_P1:

                return (GPIO_2810->IN_STR.Gpio1_in & mask) ? GPIO_HIGH_LEVEL : GPIO_LOW_LEVEL;

            case GPIO_PORT_P2:

                return (GPIO_2810->IN_STR.Gpio2_in & mask) ? GPIO_HIGH_LEVEL : GPIO_LOW_LEVEL;

            case GPIO_PORT_P3:

                return (GPIO_2810->IN_STR.Gpio3_in & mask) ? GPIO_HIGH_LEVEL : GPIO_LOW_LEVEL;

            default:

                return GPIO_LOW_LEVEL;

        }

    }

}



// 10. 电平翻转（基于读取-修改-写入）

void gpio_toggle(gpio_port_e port, gpio_pin_e pin) {

    assert(gpio_port_valid(port) && gpio_pin_valid(port, pin));

    

    gpio_level_e curr_level;

    

    if (port == GPIO_PORT_P4) {

        // 读取输出电平：从 GPIO->P4_CFG.Gpio4_out (位13:8)

        curr_level = (GPIO_2810->P4_CFG.Gpio4_out & (1UL << pin)) ? GPIO_HIGH_LEVEL : GPIO_LOW_LEVEL;

    } else {

        uint32_t mask = (1UL << pin);

        switch (port) {

            case GPIO_PORT_P0:

                curr_level = (GPIO_2810->OUT_STR.Gpio0_out & mask) ? GPIO_HIGH_LEVEL : GPIO_LOW_LEVEL;

                break;

            case GPIO_PORT_P1:

                curr_level = (GPIO_2810->OUT_STR.Gpio1_out & mask) ? GPIO_HIGH_LEVEL : GPIO_LOW_LEVEL;

                break;

            case GPIO_PORT_P2:

                curr_level = (GPIO_2810->OUT_STR.Gpio2_out & mask) ? GPIO_HIGH_LEVEL : GPIO_LOW_LEVEL;

                break;

            case GPIO_PORT_P3:

                curr_level = (GPIO_2810->OUT_STR.Gpio3_out & mask) ? GPIO_HIGH_LEVEL : GPIO_LOW_LEVEL;

                break;

            default:

                return;

        }

    }

    

    gpio_output_set(port, pin, (curr_level == GPIO_HIGH_LEVEL) ? GPIO_LOW_LEVEL : GPIO_HIGH_LEVEL);

}



// ==================== 中断相关函数 ====================

// 中断回调函数数组（按端口和引脚存储）

static void (*int_callback[GPIO_PORT_MAX][GPIO_PIN_NUM_MAX])(gpio_port_e port, gpio_pin_e pin) = { NULL };



// 获取端口在中断寄存器中的位偏移（P0~P3:0~31, P4单独处理）

static uint32_t gpio_int_bit_pos(gpio_port_e port, gpio_pin_e pin) {

    switch (port) {

        case GPIO_PORT_P0: return pin;          // 0~7

        case GPIO_PORT_P1: return pin + 8;      // 8~15

        case GPIO_PORT_P2: return pin + 16;     // 16~23

        case GPIO_PORT_P3: return pin + 24;     // 24~31

        case GPIO_PORT_P4: return pin;          // P4 有独立寄存器，此处仅用于索引，不用于位运算

        default: return 0;

    }

}



// 配置 GPIO 中断（需先配置引脚为输入模式）

void gpio_int_config(gpio_cfg_t *cfg) {

    assert(cfg != NULL);

    assert(gpio_port_valid(cfg->port) && gpio_pin_valid(cfg->port, cfg->pin));

    assert(cfg->int_cfg.callback != NULL);  // 回调函数不能为空



    uint32_t bit = gpio_int_bit_pos(cfg->port, cfg->pin);



    // 存储回调函数

    int_callback[cfg->port][cfg->pin] = cfg->int_cfg.callback;



    // P4 的中断控制寄存器与 P0~P3 不同，单独处理

    if (cfg->port == GPIO_PORT_P4) {

        // P4 中断寄存器为 P4_INT_SET (0x09) 和 P4_INT_CLR (0x0A)

        uint32_t mask = (1UL << cfg->pin);



        // 中断类型（电平/边沿）：0=电平，1=边沿

        if (cfg->int_cfg.type == 1) {

            GPIO_2810->P4_INT_SET.w |= (mask << 8);  // Int_type4 在 bit8~13

        } else {

            GPIO_2810->P4_INT_SET.w &= ~(mask << 8);

        }



        // 中断极性（电平触发时：0=低，1=高；边沿触发时：0=下降沿，1=上升沿）

        if (cfg->int_cfg.pol == 1) {

            GPIO_2810->P4_INT_SET.w |= (mask << 16); // Int_pol4 在 bit16~21

        } else {

            GPIO_2810->P4_INT_SET.w &= ~(mask << 16);

        }



        // 双边沿触发使能（仅边沿模式有效）

        if (cfg->int_cfg.type == 1 && cfg->int_cfg.pol == 2) { // 假设 pol=2 表示双边沿

            GPIO_2810->P4_INT_SET.w |= (mask << 24);

        } else {

            GPIO_2810->P4_INT_SET.w &= ~(mask << 24);

        }



        // 清除可能的历史中断标志（写 0 清除）

        GPIO_2810->P4_INT_CLR.w = (GPIO_2810->P4_INT_CLR.w & ~mask) | (0 << cfg->pin);



        // 使能中断（Int_en4 在 bit0~5）

        if (cfg->int_cfg.enable) {

            GPIO_2810->P4_INT_SET.w |= mask;

        } else {

            GPIO_2810->P4_INT_SET.w &= ~mask;

        }

        return;

    }



    // P0~P3 中断配置

    // 1. 设置中断类型（电平/边沿）

    if (cfg->int_cfg.type == 1) {

        GPIO_2810->INT_TYPE_STR.w |= (1UL << bit);

    } else {

        GPIO_2810->INT_TYPE_STR.w &= ~(1UL << bit);

    }



    // 2. 设置极性（电平触发时：0=低，1=高；边沿触发时：0=下降沿，1=上升沿）

    if (cfg->int_cfg.pol == 1) {

        GPIO_2810->INT_POL_STR.w |= (1UL << bit);

    } else {

        GPIO_2810->INT_POL_STR.w &= ~(1UL << bit);

    }



    // 3. 双边沿使能（仅边沿模式有效）

    if (cfg->int_cfg.type == 1 && cfg->int_cfg.pol == 2) { // pol=2 表示双边沿（自定义）

        GPIO_2810->INT_EDGE_STR.w |= (1UL << bit);

    } else {

        GPIO_2810->INT_EDGE_STR.w &= ~(1UL << bit);

    }



    // 4. 清除可能的历史中断标志（写 0 清除）

    gpio_int_clear_flag(cfg->port, cfg->pin);



    // 5. 使能/禁止中断

    if (cfg->int_cfg.enable) {

        GPIO_2810->INT_EN_STR.w |= (1UL << bit);

    } else {

        GPIO_2810->INT_EN_STR.w &= ~(1UL << bit);

    }

}



// 单独使能 GPIO 中断

void gpio_int_enable(gpio_port_e port, gpio_pin_e pin) {

    assert(gpio_port_valid(port) && gpio_pin_valid(port, pin));

    if (port == GPIO_PORT_P4) {

        GPIO_2810->P4_INT_SET.w |= (1UL << pin);

    } else {

        uint32_t bit = gpio_int_bit_pos(port, pin);

        GPIO_2810->INT_EN_STR.w |= (1UL << bit);

    }

}



// 单独禁止 GPIO 中断

void gpio_int_disable(gpio_port_e port, gpio_pin_e pin) {

    assert(gpio_port_valid(port) && gpio_pin_valid(port, pin));

    if (port == GPIO_PORT_P4) {

        GPIO_2810->P4_INT_SET.w &= ~(1UL << pin);

    } else {

        uint32_t bit = gpio_int_bit_pos(port, pin);

        GPIO_2810->INT_EN_STR.w &= ~(1UL << bit);

    }

}



// 清除中断标志（写 0 清除）

void gpio_int_clear_flag(gpio_port_e port, gpio_pin_e pin) {

    assert(gpio_port_valid(port) && gpio_pin_valid(port, pin));

    if (port == GPIO_PORT_P4) {

        GPIO_2810->P4_INT_CLR.w = (GPIO_2810->P4_INT_CLR.w & ~(1UL << pin)) | (0 << pin);

    } else {

        uint32_t bit = gpio_int_bit_pos(port, pin);

        uint32_t reg = GPIO_2810->INT_CLR_STR.w;

        reg &= ~(1UL << bit);

        GPIO_2810->INT_CLR_STR.w = reg;

    }

}



// 读取中断标志（是否发生中断）

bool gpio_int_get_flag(gpio_port_e port, gpio_pin_e pin) {

    assert(gpio_port_valid(port) && gpio_pin_valid(port, pin));

    if (port == GPIO_PORT_P4) {

        return (GPIO_2810->P4_INT_CLR.w & (1UL << pin)) ? true : false;

    } else {

        uint32_t bit = gpio_int_bit_pos(port, pin);

        return (GPIO_2810->INT_CLR_STR.w & (1UL << bit)) ? true : false;

    }

}



// 统一的中断服务处理函数（供具体中断服务函数调用）

void gpio_int_irq_handler(gpio_port_e port) {

    assert(gpio_port_valid(port));



    uint32_t status;

    if (port == GPIO_PORT_P4) {

        status = GPIO_2810->P4_INT_CLR.w;

        for (uint8_t pin = 0; pin < 6; pin++) {  // P4 只有 0~5

            if (status & (1UL << pin)) {

                // 清除标志

                GPIO_2810->P4_INT_CLR.w = (GPIO_2810->P4_INT_CLR.w & ~(1UL << pin)) | (0 << pin);

                // 调用回调（如果存在）

                if (int_callback[port][pin] != NULL){

                    int_callback[port][pin](port, (gpio_pin_e)pin);

                }

            }

        }

    } else {

        uint32_t bit_offset = (port == GPIO_PORT_P0) ? 0 : 

                              (port == GPIO_PORT_P1) ? 8 : 

                              (port == GPIO_PORT_P2) ? 16 : 24;

        status = (GPIO_2810->INT_CLR_STR.w >> bit_offset) & 0xFF;

        for (uint8_t pin = 0; pin < 8; pin++) {

            if (status & (1UL << pin)) {

                uint32_t bit = bit_offset + pin;

                // 清除标志

                uint32_t reg = GPIO_2810->INT_CLR_STR.w;

                reg &= ~(1UL << bit);

                GPIO_2810->INT_CLR_STR.w = reg;

                // 调用回调

                if (int_callback[port][pin] != NULL) {

                   int_callback[port][pin](port, (gpio_pin_e)pin);

                }

            }

        }

    }

}
