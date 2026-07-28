#ifndef __GPIO_REG_H__
#define __GPIO_REG_H__

#include <stdint.h>
#include "gpio_type.h"
#define __IO  volatile
// 外设基地址（手册P10 外设基地址表）
#define GPIO_BASE_2810       0x40010000UL  // GPIO模块基地址
#define IO_BASE_2810         0x40001400UL  // IO扩展模块基地址
#pragma anon_unions   



// GPIO核心寄存器结构体（手册P53~54 GPIO寄存器表）
typedef struct {
    union {
        __IO uint32_t w;                // 32位整体访问
        struct {
            __IO uint32_t Gpio0_in : 8;  // P0输入电平
            __IO uint32_t Gpio1_in : 8;  // P1输入电平
            __IO uint32_t Gpio2_in : 8;  // P2输入电平
            __IO uint32_t Gpio3_in : 8;  // P3输入电平
        };
    } IN_STR;                          // 0x00: 输入电平寄存器

    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Gpio0_out : 8; // P0输出电平
            __IO uint32_t Gpio1_out : 8; // P1输出电平
            __IO uint32_t Gpio2_out : 8; // P2输出电平
            __IO uint32_t Gpio3_out : 8; // P3输出电平
        };
    } OUT_STR;                         // 0x04: 输出电平寄存器

    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Gpio0_out_en : 8; // P0输出使能
            __IO uint32_t Gpio1_out_en : 8; // P1输出使能
            __IO uint32_t Gpio2_out_en : 8; // P2输出使能
            __IO uint32_t Gpio3_out_en : 8; // P3输出使能
        };
    } OUT_EN_STR;                      // 0x08: 输出使能寄存器

    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Int_en0 : 8;  // P0中断使能
            __IO uint32_t Int_en1 : 8;  // P1中断使能
            __IO uint32_t Int_en2 : 8;  // P2中断使能
            __IO uint32_t Int_en3 : 8;  // P3中断使能
        };
    } INT_EN_STR;                      // 0x0C: 中断使能寄存器

    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Int_type0 : 8;  // P0中断类型：0=电平，1=边沿
            __IO uint32_t Int_type1 : 8;  // P1中断类型
            __IO uint32_t Int_type2 : 8;  // P2中断类型
            __IO uint32_t Int_type3 : 8;  // P3中断类型
        };
    } INT_TYPE_STR;                    // 0x10: 中断类型寄存器

    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Int_pol0 : 8;   // P0中断极性：电平触发时0=低/1=高，边沿触发时0=下降沿/1=上升沿
            __IO uint32_t Int_pol1 : 8;
            __IO uint32_t Int_pol2 : 8;
            __IO uint32_t Int_pol3 : 8;
        };
    } INT_POL_STR;                     // 0x14: 中断极性寄存器

    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Int_clr0 : 8;   // P0中断清除/状态（写0清除，读返回中断状态）
            __IO uint32_t Int_clr1 : 8;
            __IO uint32_t Int_clr2 : 8;
            __IO uint32_t Int_clr3 : 8;
        };
    } INT_CLR_STR;                     // 0x18: 中断清除/状态寄存器

    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Int_edge0 : 8;  // P0双边沿触发使能（仅边沿模式有效）
            __IO uint32_t Int_edge1 : 8;
            __IO uint32_t Int_edge2 : 8;
            __IO uint32_t Int_edge3 : 8;
        };
    } INT_EDGE_STR;                    // 0x1C: 双边沿触发使能寄存器

    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Gpio4_in   : 6;   // P4输入电平（位5:0）
            __IO uint32_t rsv0       : 2;
            __IO uint32_t Gpio4_out  : 6;   // P4输出电平（位13:8）
            __IO uint32_t rsv1       : 2;
            __IO uint32_t Gpio4_out_en : 6; // P4输出使能（位21:16）
            __IO uint32_t rsv2       : 2;
        };
    } P4_CFG;                          // 0x20: P4配置寄存器（p4_cfg）

    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Int_en4    : 6;   // P4中断使能（位5:0）
            __IO uint32_t rsv3       : 2;
            __IO uint32_t Int_type4  : 6;   // P4中断类型（位13:8）
            __IO uint32_t rsv4       : 2;
            __IO uint32_t Int_pol4   : 6;   // P4中断极性（位21:16）
            __IO uint32_t rsv5       : 2;
            __IO uint32_t Int_edge4  : 6;   // P4双边沿使能（位29:24）
            __IO uint32_t rsv6       : 2;
        };
    } P4_INT_SET;                      // 0x24: P4中断设置寄存器

    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Int_clr4   : 6;   // P4中断清除/状态（位5:0）
            __IO uint32_t rsv7       : 2;
            __IO uint32_t Cmp4_int_clr : 1; // CMP4中断清除（位7）
            __IO uint32_t rsv8       : 24;
        };
    } P4_INT_CLR;                      // 0x28: P4中断清除/状态寄存器
} GPIO_TypeDef_2810;


// IO 寄存器结构体
typedef struct {
    // 0x00: DS0 寄存器
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Pad0_ds0 : 8;   // P0 输出增强[0]
            __IO uint32_t Pad1_ds0 : 8;   // P1 输出增强[0]
            __IO uint32_t Pad2_ds0 : 8;   // P2 输出增强[0]
            __IO uint32_t Pad3_ds0 : 8;   // P3 输出增强[0]
        };
    } DS0;

    // 0x01: DS1 寄存器
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Pad0_ds1 : 8;   // P0 输出增强[1]
            __IO uint32_t Pad1_ds1 : 8;   // P1 输出增强[1]
            __IO uint32_t Pad2_ds1 : 8;   // P2 输出增强[1]
            __IO uint32_t Pad3_ds1 : 8;   // P3 输出增强[1]
        };
    } DS1;

    // 0x02: PU 寄存器
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Pad0_pu : 8;   // P0 上拉使能
            __IO uint32_t Pad1_pu : 8;   // P1 上拉使能
            __IO uint32_t Pad2_pu : 8;   // P2 上拉使能
            __IO uint32_t Pad3_pu : 8;   // P3 上拉使能
        };
    } PU;

    // 0x03: PD 寄存器
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Pad0_pd : 8;   // P0 下拉使能
            __IO uint32_t Pad1_pd : 8;   // P1 下拉使能
            __IO uint32_t Pad2_pd : 8;   // P2 下拉使能
            __IO uint32_t Pad3_pd : 8;   // P3 下拉使能
        };
    } PD;

    // 0x04: MSK 寄存器
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Pad0_msk : 8;   // P0 唤醒掩码
            __IO uint32_t Pad1_msk : 8;   // P1 唤醒掩码
            __IO uint32_t Pad2_msk : 8;   // P2 唤醒掩码
            __IO uint32_t Pad3_msk : 8;   // P3 唤醒掩码
        };
    } MSK;

    // 0x05: POL 寄存器
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Pad0_pol : 8;   // P0 唤醒极性
            __IO uint32_t Pad1_pol : 8;   // P1 唤醒极性
            __IO uint32_t Pad2_pol : 8;   // P2 唤醒极性
            __IO uint32_t Pad3_pol : 8;   // P3 唤醒极性
        };
    } POL;

    // 0x06: ST 寄存器
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Pad0_st : 8;   // P0 施密特使能
            __IO uint32_t Pad1_st : 8;   // P1 施密特使能
            __IO uint32_t Pad2_st : 8;   // P2 施密特使能
            __IO uint32_t Pad3_st : 8;   // P3 施密特使能
        };
    } ST;

    // 0x07: ANA 寄存器
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t rsv0        : 8;   // 保留
            __IO uint32_t Pad1_ana_en : 8;   // P1 模拟功能使能
            __IO uint32_t Pad2_ana_en : 8;   // P2 模拟功能使能
            __IO uint32_t Pad3_ana_en : 8;   // P3 模拟功能使能
        };
    } ANA;

    // 0x08: P4R0 寄存器
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Pad4_ds0 : 6;   // P4 输出增强[0]
            __IO uint32_t rsv1     : 2;   // 保留
            __IO uint32_t Pad4_ds1 : 6;   // P4 输出增强[1]
            __IO uint32_t rsv2     : 2;   // 保留
            __IO uint32_t Pad4_pu  : 6;   // P4 上拉使能
            __IO uint32_t rsv3     : 2;   // 保留
            __IO uint32_t Pad4_pd  : 6;   // P4 下拉使能
            __IO uint32_t rsv4     : 2;   // 保留
        };
    } P4R0;

    // 0x09: P4R1 寄存器
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Pad4_msk        : 6;   // P4 唤醒掩码
            __IO uint32_t rsv5            : 2;   // 保留
            __IO uint32_t Pad4_pol        : 6;   // P4 唤醒极性
            __IO uint32_t rsv6            : 2;   // 保留
            __IO uint32_t Pad4_st         : 6;   // P4 施密特使能
            __IO uint32_t rsv7            : 4;   // 保留
            __IO uint32_t Pad4_ana_en_2   : 1;   // P42 模拟功能使能
            __IO uint32_t rsv8            : 5;   // 保留
        };
    } P4R1;

    // 0x0A: EN 寄存器（功能使能）
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t pad_tim2_en    : 1;   // TIM2 使能
            __IO uint32_t pad_tim2s_en   : 1;   // TIM2S 使能
            __IO uint32_t pad_tim3_en    : 1;   // TIM3 使能
            __IO uint32_t pad_tim3s_en   : 1;   // TIM3S 使能
            __IO uint32_t pad_tim4_en    : 1;   // TIM4 使能
            __IO uint32_t pad_tim4s_en   : 1;   // TIM4S 使能
            __IO uint32_t pad_cxo_en     : 1;   // CXO 使能
            __IO uint32_t pad_cxos_en    : 1;   // CXOS 使能
            __IO uint32_t pad_hal0_en    : 1;   // HAL0 使能
            __IO uint32_t pad_hal0s_en   : 1;   // HAL0S 使能
            __IO uint32_t pad_hal1_en    : 1;   // HAL1 使能
            __IO uint32_t pad_hal1s_en   : 1;   // HAL1S 使能
            __IO uint32_t pad_hal2_en    : 1;   // HAL2 使能
            __IO uint32_t pad_hal2s_en   : 1;   // HAL2S 使能
            __IO uint32_t pad_qepa_en    : 1;   // QEPA 使能（无效配置）
            __IO uint32_t pad_qepb_en    : 1;   // QEPB 使能（无效配置）
            __IO uint32_t pad_txd_en     : 1;   // TXD 使能
            __IO uint32_t pad_txds_en    : 1;   // TXDS 使能
            __IO uint32_t pad_rxd_en     : 1;   // RXD 使能
            __IO uint32_t pad_rxds_en    : 1;   // RXDS 使能
            __IO uint32_t pad_txd2_en    : 1;   // TXD2 使能
            __IO uint32_t pad_txd2s_en   : 1;   // TXD2S 使能
            __IO uint32_t pad_rxd2_en    : 1;   // RXD2 使能
            __IO uint32_t pad_rxd2s_en   : 1;   // RXD2S 使能
            __IO uint32_t pad_swd_clk_en : 1;   // SWD_CLK 使能
            __IO uint32_t pad_swd_sda_en : 1;   // SWD_SDA 使能
            __IO uint32_t pad_dbg_en     : 1;   // DBG 使能
            __IO uint32_t pad_rstn_en    : 1;   // RSTN 使能
            __IO uint32_t pad_scl_en     : 1;   // SCL 使能
            __IO uint32_t pad_scls_en    : 1;   // SCLS 使能
            __IO uint32_t pad_sda_en     : 1;   // SDA 使能
            __IO uint32_t pad_sdas_en    : 1;   // SDAS 使能
        };
    } EN;

    // 0x0B: EN2 寄存器（功能使能2）
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t pad_Eu_en          : 1;   // Eu ADC 采样使能
            __IO uint32_t pad_Eus_en         : 1;   // Eu ADC 采样使能替换
            __IO uint32_t pad_Ev_en          : 1;   // Ev ADC 采样使能
            __IO uint32_t pad_Evs_en         : 1;   // Ev ADC 采样使能替换
            __IO uint32_t pad_Ew_en          : 1;   // Ew ADC 采样使能
            __IO uint32_t pad_Ews_en         : 1;   // Ew ADC 采样使能替换
            __IO uint32_t pad_Udc_from_in_en : 1;   // Udc 内部分压使能
            __IO uint32_t Pad_c4ns_en        : 1;   // C4NS 使能
            __IO uint32_t Pad_c0ns_en        : 1;   // C0NS 使能
            __IO uint32_t Pad_c2ns_en        : 1;   // C2NS 使能
            __IO uint32_t Pad_c5ps_en        : 1;   // C5PS 使能
            __IO uint32_t Pad_c1ns_en        : 1;   // C1NS 使能
            __IO uint32_t pad_sh_u_fpga      : 1;   // FPGA 测试 U 路采样保持控制
            __IO uint32_t pad_sh_v_fpga      : 1;   // FPGA 测试 V 路采样保持控制
            __IO uint32_t pad_nss_en         : 1;   // SPI NSS 使能
            __IO uint32_t pad_sclk_en        : 1;   // SPI SCLK 使能
            __IO uint32_t pad_mosi_en        : 1;   // SPI MOSI 使能
            __IO uint32_t pad_miso_en        : 1;   // SPI MISO 使能
            __IO uint32_t rsv9               : 14;  // 保留
        };
    } EN2;

    // 0x0C: 保留
    __IO uint32_t RESERVED0;

    // 0x0D: DBG 寄存器
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t cfg_dbg0_mux : 5;   // DBG 脚测试信号输出选通
            __IO uint32_t rsv10        : 3;   // 保留
            __IO uint32_t cfg_dbg1_mux : 5;   // CXO 脚测试信号输出选通
            __IO uint32_t rsv11        : 19;  // 保留
        };
    } DBG;
} IO_TypeDef_2810;
#define IO_2810                ((IO_TypeDef_2810 *)           IO_BASE_2810  	    )

// GPIO全局指针（直接指向基地址）
#define GPIO_2810    ((GPIO_TypeDef_2810 *)GPIO_BASE_2810)

// 端口偏移量（P0~P4的寄存器组偏移，手册P53寄存器地址连续分布）
static const uint32_t port_offset[GPIO_PORT_MAX] = {
    0x000UL,  // P0: GPIO_BASE + 0x00
    0x000UL,  // P1: 共享GPIO核心寄存器（通过位域区分）
    0x000UL,  // P2: 共享GPIO核心寄存器
    0x000UL,  // P3: 共享GPIO核心寄存器
    0x000UL   // P4: 通过IO_P4R0/IO_P4R1配置
};

#endif  // __GPIO_REG_H__
