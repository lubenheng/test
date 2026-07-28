#ifndef __GPIO_TYPE_H__
#define __GPIO_TYPE_H__

#include <stdint.h>
#include <stdbool.h>

// 端口枚举（对应寄存器 P0~P4，参考gpio_reg.h port_offset定义）
typedef enum {
    GPIO_PORT_P0 = 0,
    GPIO_PORT_P1,
    GPIO_PORT_P2,
    GPIO_PORT_P3,
    GPIO_PORT_P4,
    GPIO_PORT_MAX  // 端口最大值（用于边界检查）
} gpio_port_e;

// 引脚枚举（P0~P3支持0~7，P4支持0~5，参考gpio_reg.h注释）
typedef enum {
    GPIO_PIN_0 = 0,
    GPIO_PIN_1,
    GPIO_PIN_2,
    GPIO_PIN_3,
    GPIO_PIN_4,
    GPIO_PIN_5,
    GPIO_PIN_6,
    GPIO_PIN_7,
    GPIO_PIN_NUM_MAX  // 引脚最大值（P4仅支持0~5，需单独校验）
} gpio_pin_e;

// 方向枚举（对应GPIO_OUT_EN_STR寄存器，参考gpio_reg.h P53）
typedef enum {
    GPIO_DIR_INPUT = 0,  // 输入（OUT_EN=0）
    GPIO_DIR_OUTPUT      // 输出（OUT_EN=1）
} gpio_dir_e;

// 上下拉枚举（对应IO_PU/IO_PD寄存器，参考gpio_reg.h P44）
typedef enum {
    GPIO_PULL_DISABLE = 0,  // 禁用上下拉
    GPIO_PULL_UP,           // 上拉
    GPIO_PULL_DOWN,         // 下拉
    GPIO_PULL_MAX           // 上下拉配置最大值
} gpio_pull_cfg_e;

// 驱动强度枚举（对应IO_DS0/DS1寄存器，参考gpio_reg.h P43）
typedef enum {
    GPIO_STRENGTH_LOW = 0,    // DS0=0, DS1=0（低驱动）
    GPIO_STRENGTH_MID,       // DS0=0, DS1=1（中驱动，仅P0~P3支持）
    GPIO_STRENGTH_HIGH,      // DS0=1, DS1=1（高驱动）
    GPIO_STRENGTH_HIGH_Z,    // 高阻（仅P4支持）
    GPIO_STRENGTH_MAX        // 驱动强度最大值
} gpio_strength_e;

// 输入模式枚举（对应IO_ST寄存器，参考gpio_reg.h P46）
typedef enum {
    GPIO_INPUT_MODE_CMOS = 0,   // CMOS输入模式
    GPIO_INPUT_MODE_SCHMITT,    // 施密特输入模式
    GPIO_INPUT_MODE_MAX         // 输入模式最大值
} gpio_input_mode_e;

// 模拟模式枚举（对应IO_ANA寄存器，参考gpio_reg.h P46）
typedef enum {
    GPIO_ANALOG_DISABLE = 0,  // 禁用模拟模式
    GPIO_ANALOG_ENABLE        // 启用模拟模式
} gpio_analog_mode_e;

// 复用功能枚举（对应IO_EN/EN2寄存器，参考gpio_reg.h P47）
typedef enum {
    GPIO_ALT_FUNC_0 = 0,  // 普通GPIO（默认）
    GPIO_ALT_FUNC_1,      // UART
    GPIO_ALT_FUNC_2,      // I2C
    GPIO_ALT_FUNC_3,      // SPI
    GPIO_ALT_FUNC_4,      // TIMER
    GPIO_ALT_FUNC_5,      // Hall/QEP
    GPIO_ALT_FUNC_6,      // 测试模式
    GPIO_ALT_FUNC_7,      // 模拟功能（最高优先级）
    GPIO_ALT_FUNC_MAX     // 复用功能最大值
} gpio_alt_func_e;

// 中断配置结构体（对应GPIO_INT寄存器，参考gpio_reg.h P53~54）
typedef struct {
    uint8_t type;         // 中断类型：0=电平触发，1=边沿触发
    uint8_t pol;          // 中断极性：0=低/下降沿，1=高/上升沿
    uint8_t priority;     // 中断优先级（CM0支持0~1）
    void (*callback)(gpio_port_e port, gpio_pin_e pin);  // 中断回调函数
		uint8_t enable;
} gpio_int_cfg_t;

// GPIO配置结构体（整合所有配置项）
typedef struct {
    gpio_port_e        port;        // 端口
    gpio_pin_e         pin;         // 引脚
    gpio_dir_e         dir;         // 方向
    gpio_pull_cfg_e    pull;        // 上下拉
    gpio_strength_e    strength;    // 驱动强度
    gpio_input_mode_e  input_mode;  // 输入模式
    gpio_analog_mode_e analog_mode; // 模拟模式
    gpio_alt_func_e    alt_func;    // 复用功能
    gpio_int_cfg_t     int_cfg;     // 中断配置（仅输入模式有效）
} gpio_cfg_t;

// 电平枚举（对应GPIO_OUT_STR寄存器，参考gpio_reg.h P53）
typedef enum {
    GPIO_LOW_LEVEL = 0,  // 低电平
    GPIO_HIGH_LEVEL      // 高电平
} gpio_level_e;

#endif  // __GPIO_TYPE_H__

