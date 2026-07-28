#ifndef _I2C_REG_H
#define _I2C_REG_H
#include "stdint.h"
//#include "core_cm0.h"

#define I2C0_BASE         (0x40002000UL)
#define I2C2_BASE         (0x40005400UL)
// 解决匿名结构体/联合体报错（关键！）
#pragma anon_unions
/* 寄存器读写权限定义（标准CMSIS） */
#define __I  const volatile    /* 只读  */
#define __O  volatile          /* 只写  */
#define __IO volatile          /* 读写  */

typedef struct
{
    // 0x00: En 寄存器
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Slv_en        : 1;  // [0] I2C slave 使能
            __IO uint32_t Mst_en        : 1;  // [1] I2C master 使能
            __IO uint32_t Cpu_occupy    : 1;  // [2] CPU 占据写端口使能
            __IO uint32_t chip_id       : 7;  // [9:3] I2C 7位ID
            __IO uint32_t Deglitch_cnt  : 6;  // [15:10] 去毛刺长度
            __IO uint32_t Int_en        : 1;  // [16] 中断总使能
            __IO uint32_t rsv           : 1;  // [17] 保留
            __IO uint32_t deglitch_en   : 1;  // [18] 去毛刺模块使能
            __IO uint32_t reserved19_31 : 13; // [31:19] 保留
        };
    } I2C_En;

    // 0x04: memOpt 寄存器
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Cpu_wr_dat    : 8;  // [7:0] CPU写数据
            __IO uint32_t Cpu_addr      : 6;  // [13:8] CPU读写地址
            __IO uint32_t reserved14_15 : 2;  // [15:14] 保留
            __O  uint32_t Cpu_wr        : 1;  // [16] CPU写触发脉冲
            __IO uint32_t reserved17_31 : 15; // [31:17] 保留
        };
    } I2C_memOpt;

    // 0x08: mst 寄存器
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Mst_clk_div       : 10; // [9:0] 主模式时钟分频
            __IO uint32_t Mst_i2c_wr        : 1;  // [10] 主模式读写控制
            __IO uint32_t Mst_i2c_addr_num  : 3;  // [13:11] 地址字节数
            __IO uint32_t Mst_i2c_dat_num   : 9;  // [22:14] 数据字节数
            __IO uint32_t reserved23_31     : 9;  // [31:23] 保留
        };
    } I2C_mst;

    // 0x0C: Mst_addr 寄存器
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Mst_addr : 32; // [31:0] 主模式通信地址
        };
    } I2C_Mst_addr;

    // 0x10: Cpu_rd_dat 寄存器
    union {
        __I  uint32_t w;
        struct {
            __I  uint32_t Cpu_rd_dat : 8;  // [7:0] CPU读数据（只读）
            __I  uint32_t reserved8_31 : 24;
        };
    } I2C_Cpu_rd_dat;

    // 0x14: slv_ro 寄存器
    union {
        __I  uint32_t w;
        struct {
            __I  uint32_t Slv_state       : 4;  // [3:0] 从机状态（只读）
            __I  uint32_t Slv_wr_stat     : 1;  // [4] 从机读写状态
            __I  uint32_t slv_byt_len     : 9;  // [13:5] 操作字节数
            __I  uint32_t slv_mem_addr    : 8;  // [21:14] 从机当前地址
            __I  uint32_t slv_pause_err   : 1;  // [22] 冲突错误标志
            __I  uint32_t reserved23_31   : 9;
        };
    } I2C_slv_ro;

    // 0x18: mst_ro 寄存器
    union {
        __I  uint32_t w;
        struct {
            __I  uint32_t mst_state      : 5;  // [4:0] 主机状态
            __I  uint32_t Mst_int_idx    : 1;  // [5] 中断原因
            __I  uint32_t reserved6_31   : 26;
        };
    } I2C_mst_ro;

    // 0x1C: Wo 寄存器
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Int_clr        : 1;  // [0] 中断清除/状态
            __IO  uint32_t Mst_trigger_wo : 1;  // [1] 主机触发
            __IO uint32_t reserved2_30   : 29;
            __IO  uint32_t Data_soft_rst  : 1;  // [31] 软件复位所有数据
        };
    } I2C_Wo;

} I2C_TypeDef;

#define I2C0    ((I2C_TypeDef *)I2C0_BASE)
#define I2C2    ((I2C_TypeDef *)I2C2_BASE)




#endif  // _I2C_REG_H
