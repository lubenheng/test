#ifndef DMA_REG_H
#define DMA_REG_H
#include "ODS_5010.h"
#include "stdint.h"

// 寄存器访问宏（与GPIO模板完全一致）
//#define REG32(addr)          (*((volatile uint32_t *)(addr)))

// -------------------------- DMA基地址定义（指定0x4000 0000UL） --------------------------
//#define DMA_BASE             (0x4000 0000UL)  // 核心指定DMA基地址
#define DMA_Type                 ((DMA_TypeDef *)DMA_BASE)
 
// -------------------------- 通用枚举定义（PDF原始定义） --------------------------
// DMA传输状态枚举
typedef enum
{
    DMA_IDLE,           // 无操作
    DMA_INPROGRESS,     // 传输中
    DMA_ENDT1,          // 测试1完成
    DMA_ENDT2,          // 测试2完成
    DMA_ENDT,           // 测试1+2完成
    DMA_COMPLETED,      // 所有测试完成
    DMA_FAILED          // 传输出错
} DmaStatusType;

// DMA数据宽度枚举
typedef enum
{
    BYTE,               // 8位
    HWORD,              // 16位
    WORD                // 32位
} DmaSizeType;

// 函数返回值枚举
typedef enum
{
    DMA_ERROR = 0,
    DMA_SUCCESS = !DMA_ERROR
} ErrStatus;

// -------------------------- 结构体定义（PDF原始定义） --------------------------
// DMA请求结构体
typedef struct
{
    uint8_t  test_ptr;
    uint8_t  test_num;
    uint16_t cnt1_index;
    uint16_t cnt2_index;
    uint16_t tx_index;
    uint16_t rx_index;
    DmaStatusType status1;
    DmaStatusType status2;
    DmaSizeType   size1;
    DmaSizeType   size2;
} DmaReqType;

// DMA数据缓冲区结构体
typedef struct
{
    uint32_t data[256];
} DMA_Data;

// -------------------------- 寄存器位掩码宏（PDF原始+GPIO风格） --------------------------
// 控制寄存器通用位掩码
#define DMA_CTRL_EN          ((uint32_t)0x00000001)  /* bit0: 通道使能 */
#define DMA_CTRL_SOINC       ((uint32_t)0x00000002)  /* bit1: 源地址递增 */
#define DMA_CTRL_DEINC       ((uint32_t)0x00000004)  /* bit2: 目的地址递增 */
#define DMA_CTRL_DEKEEP      ((uint32_t)0x00000000)  /* 目的地址保持 */
#define DMA_CTRL_SOSIZE_B    ((uint32_t)0x00000000)  /* bit3-4: 源宽度-字节 */
#define DMA_CTRL_SOSIZE_HW   ((uint32_t)0x00000008)  /* bit3-4: 源宽度-半字 */
#define DMA_CTRL_SOSIZE_W    ((uint32_t)0x00000010)  /* bit3-4: 源宽度-字 */
#define DMA_CTRL_SOBURST_S   ((uint32_t)0x00000000)  /* bit5-6: 源突发-单传输 */
#define DMA_CTRL_SOBURST_4I  ((uint32_t)0x00000020)  /* bit5-6: 源突发-4个 */
#define DMA_CTRL_SOBURST_8I  ((uint32_t)0x00000040)  /* bit5-6: 源突发-8个 */
#define DMA_CTRL_SOBURST16I  ((uint32_t)0x00000060)  /* bit5-6: 源突发-16个 */
#define DMA_CTRL_DESIZE_B    ((uint32_t)0x00000000)  /* bit7-8: 目的宽度-字节 */
#define DMA_CTRL_DESIZE_HW   ((uint32_t)0x00000080)  /* bit7-8: 目的宽度-半字 */
#define DMA_CTRL_DESIZE_W    ((uint32_t)0x00000100)  /* bit7-8: 目的宽度-字 */
#define DMA_CTRL_PL_LOW      ((uint32_t)0x00000000)  /* bit9-10: 优先级-低 */
#define DMA_CTRL_PL_MID      ((uint32_t)0x00000200)  /* bit9-10: 优先级-中 */
#define DMA_CTRL_PL_HIGH     ((uint32_t)0x00000400)  /* bit9-10: 优先级-高 */
#define DMA_CTRL_PL_HIGHER   ((uint32_t)0x00000600)  /* bit9-10: 优先级-最高 */
#define DMA_CTRL_MEM2MEM     ((uint32_t)0x00000800)  /* bit11: 内存到内存 */
#define DMA_CTRL_PER2MEM     ((uint32_t)0x00000000)  /* bit13: 外设到内存 */
#define DMA_CTRL_MEM2PER     ((uint32_t)0x00002000)  /* bit13: 内存到外设 */
#define DMA_CTRL_RT      ((uint32_t)0x00004000)  /* bit14: 重触发 */
 

// 掩码/清除/状态寄存器位掩码
#define DMA_MASK0            ((uint32_t)0x00000001)  /* bit0: CH0中断掩码 */
#define DMA_MASK1            ((uint32_t)0x00000002)  /* bit1: CH1中断掩码 */
#define DMA_MASK2            ((uint32_t)0x00000004)  /* bit2: CH2中断掩码 */
#define DMA_MASK3            ((uint32_t)0x00000008)  /* bit3: CH3中断掩码 */
#define DMA_MASK4            ((uint32_t)0x00000010)  /* bit4: CH4中断掩码 */
#define DMA_MASK5            ((uint32_t)0x00000020)  /* bit5: CH5中断掩码 */

#define DMA_CLEAR0           ((uint32_t)0x00000001)  /* bit0: CH0标志清除 */
#define DMA_CLEAR1           ((uint32_t)0x00000002)  /* bit1: CH1标志清除 */
#define DMA_CLEAR2           ((uint32_t)0x00000004)  /* bit2: CH2标志清除 */
#define DMA_CLEAR3           ((uint32_t)0x00000008)  /* bit3: CH3标志清除 */
#define DMA_CLEAR4           ((uint32_t)0x00000010)  /* bit4: CH4标志清除 */
#define DMA_CLEAR5           ((uint32_t)0x00000020)  /* bit5: CH5标志清除 */

#define DMA_STATUS0          ((uint32_t)0x00000001)  /* bit0: CH0传输状态 */
#define DMA_STATUS1          ((uint32_t)0x00000002)  /* bit1: CH1传输状态 */
#define DMA_STATUS2          ((uint32_t)0x00000004)  /* bit2: CH2传输状态 */
#define DMA_STATUS3          ((uint32_t)0x00000008)  /* bit3: CH3传输状态 */
#define DMA_STATUS4          ((uint32_t)0x00000010)  /* bit4: CH4传输状态 */
#define DMA_STATUS5          ((uint32_t)0x00000020)  /* bit5: CH5传输状态 */

// DMA外设绑定宏（PDF原始定义）
#define DMA_SPI0_TX          0
#define DMA_SPI0_RX          1
#define DMA_SPI1_TX          2
#define DMA_SPI1_RX          3
#define DMA_SPI2_TX          4
#define DMA_SPI2_RX          5
#define DMA_SPI3_TX          6
#define DMA_SPI3_RX          7
#define DMA_SPI4_TX          8
#define DMA_SPI4_RX          9

 
#define DMAAPB_OK            0
#define DMAAPB_NOK           1
#define DATA_As              0xAAAA
#define DATA_5s              0x5555
#define DATA_0s              0x0000
#define DATA_Fs              0xFFFF
#define MASK16               0xFFFF

// -------------------------- 联合体定义（GPIO模板风格+PDF位段） --------------------------
// DMA通道控制寄存器位段（CH0-CH5通用）
typedef union
{  
    struct
    {
        __IO uint32_t CHEN      : 1;  /* bit0: 通道使能 */
        __IO uint32_t SINC      : 1;  /* bit1: 源地址递增 */
        __IO uint32_t DINC      : 1;  /* bit2: 目的地址递增 */
        __IO uint32_t SSIZE     : 2;  /* bit3-4: 源数据宽度 */
        __IO uint32_t SBURST    : 2;  /* bit5-6: 源突发长度 */
        __IO uint32_t DSIZE     : 2;  /* bit7-8: 目的数据宽度 */
        __IO uint32_t PL        : 2;  /* bit9-10: 通道优先级 */
        __IO uint32_t M2M       : 1;  /* bit11: 内存到内存模式 */
        __IO uint32_t RESV12    : 1;  /* bit12: 保留位 */
        __IO uint32_t DIR       : 1;  /* bit13: 传输方向 */
        __IO uint32_t RT  		  : 1;  /* bit14: 重触发 */
        __IO uint32_t RESV15_17 : 17;  /* bit15-31: 保留位 */
    } Bits;
		__IO uint32_t w;
		
} DMACHx_CTL_STR;

// DMA通道外设绑定寄存器1（CH0-CH3）
//typedef union
//{  
//    struct
//    {
//        __IO uint32_t CH0SEL  : 6;  /* bit0-5: CH0外设选择 */
//        __IO uint32_t RESV6_7 : 2;  /* bit6-7: 保留位 */
//        __IO uint32_t CH1SEL  : 6;  /* bit8-13: CH1外设选择 */
//        __IO uint32_t RESV14_15:2;  /* bit14-15: 保留位 */
//        __IO uint32_t CH2SEL  : 6;  /* bit16-21: CH2外设选择 */
//        __IO uint32_t RESV22_23:2;  /* bit22-23: 保留位 */
//        __IO uint32_t CH3SEL  : 6;  /* bit24-29: CH3外设选择 */
//        __IO uint32_t RESV30_31:2;  /* bit30-31: 保留位 */
//    } Bits;
//		__IO uint32_t w;
//} DMA_CHCFG1_STRx;

//// DMA通道外设绑定寄存器2（CH4-CH5）
//typedef union
//{
//    struct
//    {
//        __IO uint32_t CH4SEL  : 6;  /* bit0-5: CH4外设选择 */
//        __IO uint32_t RESV6_7 : 2;  /* bit6-7: 保留位 */
//        __IO uint32_t CH5SEL  : 6;  /* bit8-13: CH5外设选择 */
//        __IO uint32_t RESV14_31:18; /* bit14-31: 保留位 */
//    } Bits;
//		__IO uint32_t w;
//} DMA_CHCFG2_STRx;

// -------------------------- DMA完整寄存器结构体（完全模仿GPIO模板） --------------------------
typedef struct
{
    // -------------------------- CH0 寄存器（0x00-0x1C） --------------------------
    union{
        struct{
            __IO uint32_t DMA_Source0 : 32; /* bit31-0: 源地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_SRC0_STR;   /* 0x00 */

    union{
        struct{
            __IO uint32_t DMA_Dest0   : 32; /* bit31-0: 目的地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_DEST0_STR;  /* 0x04 */

    union{
        struct{
            __IO uint32_t DMA_Max0    : 16; /* bit15-0: 最大传输长度寄存器 */
        };
        __IO uint32_t w;
    }DMA_MAX0_STR;   /* 0x08 */

    DMACHx_CTL_STR  DMA_CTL0_STR;    /* 0x0C: 控制寄存器 */

    union{
        struct{
            __IO uint32_t DMA_SoCurr0 : 32; /* bit31-0: 源当前地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_SOCURR0_STR;/* 0x10 */

    union{
        struct{
            __IO uint32_t DMA_DeCurr0 : 32; /* bit31-0: 目的当前地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_DECURR0_STR;/* 0x14 */

    union{
        struct{
            __IO uint32_t DMA_Tcnt0   : 16; /* bit15-0: 剩余传输计数寄存器 */
        };
        __IO uint32_t w;
    }DMA_TCNT0_STR;  /* 0x18 */

    uint32_t RESERVED1C;       /* 0x1C: 保留位 */

    // -------------------------- CH1 寄存器（0x20-0x3C） --------------------------
    union{
        struct{
            __IO uint32_t DMA_Source1 : 32; /* bit31-0: 源地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_SRC1_STR;   /* 0x20 */

    union{
        struct{
            __IO uint32_t DMA_Dest1   : 32; /* bit31-0: 目的地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_DEST1_STR;  /* 0x24 */

    union{
        struct{
            __IO uint32_t DMA_Max1    : 16; /* bit15-0: 最大传输长度寄存器 */
        };
        __IO uint32_t w;
    }DMA_MAX1_STR;   /* 0x28 */

    DMACHx_CTL_STR  DMA_CTL1_STR;    /* 0x2C: 控制寄存器 */

    union{
        struct{
            __IO uint32_t DMA_SoCurr1 : 32; /* bit31-0: 源当前地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_SOCURR1_STR;/* 0x30 */

    union{
        struct{
            __IO uint32_t DMA_DeCurr1 : 32; /* bit31-0: 目的当前地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_DECURR1_STR;/* 0x34 */

    union{
        struct{
            __IO uint32_t DMA_Tcnt1   : 16; /* bit15-0: 剩余传输计数寄存器 */
        };
        __IO uint32_t w;
    }DMA_TCNT1_STR;  /* 0x38 */

    uint32_t RESERVED3C;       /* 0x3C: 保留位 */

    // -------------------------- CH2 寄存器（0x40-0x5C） --------------------------
    union{
        struct{
            __IO uint32_t DMA_Source2 : 32; /* bit31-0: 源地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_SRC2_STR;   /* 0x40 */

    union{
        struct{
            __IO uint32_t DMA_Dest2   : 32; /* bit31-0: 目的地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_DEST2_STR;  /* 0x44 */

    union{
        struct{
            __IO uint32_t DMA_Max2    : 16; /* bit15-0: 最大传输长度寄存器 */
        };
        __IO uint32_t w;
    }DMA_MAX2_STR;   /* 0x48 */

    DMACHx_CTL_STR  DMA_CTL2_STR;    /* 0x4C: 控制寄存器 */

    union{
        struct{
            __IO uint32_t DMA_SoCurr2 : 32; /* bit31-0: 源当前地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_SOCURR2_STR;/* 0x50 */

    union{
        struct{
            __IO uint32_t DMA_DeCurr2 : 32; /* bit31-0: 目的当前地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_DECURR2_STR;/* 0x54 */

    union{
        struct{
            __IO uint32_t DMA_Tcnt2   : 16; /* bit15-0: 剩余传输计数寄存器 */
        };
        __IO uint32_t w;
    }DMA_TCNT2_STR;  /* 0x58 */

    uint32_t RESERVED5C;       /* 0x5C: 保留位 */

    // -------------------------- CH3 寄存器（0x60-0x7C） --------------------------
    union{
        struct{
            __IO uint32_t DMA_Source3 : 32; /* bit31-0: 源地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_SRC3_STR;   /* 0x60 */

    union{
        struct{
            __IO uint32_t DMA_Dest3   : 32; /* bit31-0: 目的地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_DEST3_STR;  /* 0x64 */

    union{
        struct{
            __IO uint32_t DMA_Max3    : 16; /* bit15-0: 最大传输长度寄存器 */
        };
        __IO uint32_t w;
    }DMA_MAX3_STR;   /* 0x68 */

    DMACHx_CTL_STR  DMA_CTL3_STR;    /* 0x6C: 控制寄存器 */

    union{
        struct{
            __IO uint32_t DMA_SoCurr3 : 32; /* bit31-0: 源当前地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_SOCURR3_STR;/* 0x70 */

    union{
        struct{
            __IO uint32_t DMA_DeCurr3 : 32; /* bit31-0: 目的当前地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_DECURR3_STR;/* 0x74 */

    union{
        struct{
            __IO uint32_t DMA_Tcnt3   : 16; /* bit15-0: 剩余传输计数寄存器 */
        };
        __IO uint32_t w;
    }DMA_TCNT3_STR;  /* 0x78 */

    uint32_t RESERVED7C;       /* 0x7C: 保留位 */

    // -------------------------- CH4 寄存器（0x80-0x9C） --------------------------
    union{
        struct{
            __IO uint32_t DMA_Source4 : 32; /* bit31-0: 源地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_SRC4_STR;   /* 0x80 */

    union{
        struct{
            __IO uint32_t DMA_Dest4   : 32; /* bit31-0: 目的地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_DEST4_STR;  /* 0x84 */

    union{
        struct{
            __IO uint32_t DMA_Max4    : 16; /* bit15-0: 最大传输长度寄存器 */
        };
        __IO uint32_t w;
    }DMA_MAX4_STR;   /* 0x88 */

    DMACHx_CTL_STR  DMA_CTL4_STR;    /* 0x8C: 控制寄存器 */

    union{
        struct{
            __IO uint32_t DMA_SoCurr4 : 32; /* bit31-0: 源当前地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_SOCURR4_STR;/* 0x90 */

    union{
        struct{
            __IO uint32_t DMA_DeCurr4 : 32; /* bit31-0: 目的当前地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_DECURR4_STR;/* 0x94 */

    union{
        struct{
            __IO uint32_t DMA_Tcnt4   : 16; /* bit15-0: 剩余传输计数寄存器 */
        };
        __IO uint32_t w;
    }DMA_TCNT4_STR;  /* 0x98 */

    uint32_t RESERVED9C;       /* 0x9C: 保留位 */

    // -------------------------- CH5 寄存器（0xA0-0xBC） --------------------------
    union{
        struct{
            __IO uint32_t DMA_Source5 : 32; /* bit31-0: 源地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_SRC5_STR;   /* 0xA0 */

    union{
        struct{
            __IO uint32_t DMA_Dest5   : 32; /* bit31-0: 目的地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_DEST5_STR;  /* 0xA4 */

    union{
        struct{
            __IO uint32_t DMA_Max5    : 16; /* bit15-0: 最大传输长度寄存器 */
        };
        __IO uint32_t w;
    }DMA_MAX5_STR;   /* 0xA8 */

    DMACHx_CTL_STR  DMA_CTL5_STR;    /* 0xAC: 控制寄存器 */

    union{
        struct{
            __IO uint32_t DMA_SoCurr5 : 32; /* bit31-0: 源当前地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_SOCURR5_STR;/* 0xB0 */

    union{
        struct{
            __IO uint32_t DMA_DeCurr5 : 32; /* bit31-0: 目的当前地址寄存器 */
        };
        __IO uint32_t w;
    }DMA_DECURR5_STR;/* 0xB4 */

    union{
        struct{
            __IO uint32_t DMA_Tcnt5   : 16; /* bit15-0: 剩余传输计数寄存器 */
        };
        __IO uint32_t w;
    }DMA_TCNT5_STR;  /* 0xB8 */

    uint32_t RESERVEDBC;       /* 0xBC: 保留位 */

    // -------------------------- 全局控制寄存器（0xC0-0xD0） --------------------------
    union{
        struct{
            __IO uint32_t DMA_Mask    : 32; /* bit31-0: 中断掩码寄存器 */
        };
        __IO uint32_t w;
    }DMA_MASK_STR;   /* 0xC0 */

    union{
        struct{
            __IO uint32_t DMA_Clr     : 32; /* bit31-0: 中断清除寄存器 */
        };
        __IO uint32_t w;
    }DMA_CLR_STR;    /* 0xC4 */

    union{
        struct{
            __IO uint32_t DMA_Status  : 32; /* bit31-0: 中断状态寄存器 */
        };
        __IO uint32_t w;
    }DMA_STAT_STR; /* 0xC8 */

//    union{
//        struct{
//            __IO uint32_t DMA_Matrix1 : 32; /* bit31-0: 矩阵配置1寄存器 */
//        };
//        __IO uint32_t w;
//    }DMA_MATRIX1_STR;/* 0xCC */

//    union{
//        struct{
//            __IO uint32_t DMA_Matrix2 : 32; /* bit31-0: 矩阵配置2寄存器 */
//        };
//        __IO uint32_t w;
//    }DMA_MATRIX2_STR;/* 0xD0 */

    // -------------------------- 外设绑定寄存器（0xD4-0xD8） --------------------------
		union{
        struct
				{
						__IO uint32_t CH0SEL  : 6;  /* bit0-5: CH0外设选择 */
						__IO uint32_t RESV6_7 : 2;  /* bit6-7: 保留位 */
						__IO uint32_t CH1SEL  : 6;  /* bit8-13: CH1外设选择 */
						__IO uint32_t RESV14_15:2;  /* bit14-15: 保留位 */
						__IO uint32_t CH2SEL  : 6;  /* bit16-21: CH2外设选择 */
						__IO uint32_t RESV22_23:2;  /* bit22-23: 保留位 */
						__IO uint32_t CH3SEL  : 6;  /* bit24-29: CH3外设选择 */
						__IO uint32_t RESV30_31:2;  /* bit30-31: 保留位 */
				} Bits;
				__IO uint32_t w;
    }DMA_CHCFG1_STR;/* 0xCC */
		union{
            struct
						{
								__IO uint32_t CH4SEL  : 6;  /* bit0-5: CH4外设选择 */
								__IO uint32_t RESV6_7 : 2;  /* bit6-7: 保留位 */
								__IO uint32_t CH5SEL  : 6;  /* bit8-13: CH5外设选择 */
								__IO uint32_t RESV14_31:18; /* bit14-31: 保留位 */
						} Bits;
						__IO uint32_t w;
    }DMA_CHCFG2_STR;/* 0xD0 */
		
//    DMA_CHCFG1_STR   DMA_CHCFG1_STR;  /* 0xD4: CH0-CH3外设绑定 */
//    DMA_CHCFG2_STR   DMA_CHCFG2_STR;  /* 0xD8: CH4-CH5外设绑定 */

    // -------------------------- 保留位（0xDC+） --------------------------
    uint32_t RESERVEDDC; /* 后续保留位，对齐PDF2K地址空间 */

} DMA_TypeDef;

// -------------------------- 快捷访问宏（完全模仿GPIO_HALL_U/V/W写法） --------------------------
// CH0 快捷宏
#define DMA_CH0_SRC      REG32(DMA_BASE + 0x00)
#define DMA_CH0_DEST     REG32(DMA_BASE + 0x04)
#define DMA_CH0_MAX      REG32(DMA_BASE + 0x08)
#define DMA_CH0_CTL      ((DMACHx_CTL_STR*)(DMA_BASE + 0x0C))
#define DMA_CH0_SOCURR   REG32(DMA_BASE + 0x10)
#define DMA_CH0_DECURR   REG32(DMA_BASE + 0x14)
#define DMA_CH0_TCNT     REG32(DMA_BASE + 0x18)
#define DMA_CH0_EN       ((DMA_CH0_CTL->w & DMA_CTRL_EN) >> 0)
#define DMA_CH0_TCNT_REM (DMA_CH0_TCNT)

// CH1 快捷宏
#define DMA_CH1_SRC      REG32(DMA_BASE + 0x20)
#define DMA_CH1_DEST     REG32(DMA_BASE + 0x24)
#define DMA_CH1_MAX      REG32(DMA_BASE + 0x28)
#define DMA_CH1_CTL      ((DMACHx_CTL_STR*)(DMA_BASE + 0x2C))
#define DMA_CH1_SOCURR   REG32(DMA_BASE + 0x30)
#define DMA_CH1_DECURR   REG32(DMA_BASE + 0x34)
#define DMA_CH1_TCNT     REG32(DMA_BASE + 0x38)

// CH2 快捷宏
#define DMA_CH2_SRC      REG32(DMA_BASE + 0x40)
#define DMA_CH2_DEST     REG32(DMA_BASE + 0x44)
#define DMA_CH2_MAX      REG32(DMA_BASE + 0x48)
#define DMA_CH2_CTL      ((DMACHx_CTL_STR*)(DMA_BASE + 0x4C))
#define DMA_CH2_SOCURR   REG32(DMA_BASE + 0x50)
#define DMA_CH2_DECURR   REG32(DMA_BASE + 0x54)
#define DMA_CH2_TCNT     REG32(DMA_BASE + 0x58)

// CH3 快捷宏
#define DMA_CH3_SRC      REG32(DMA_BASE + 0x60)
#define DMA_CH3_DEST     REG32(DMA_BASE + 0x64)
#define DMA_CH3_MAX      REG32(DMA_BASE + 0x68)
#define DMA_CH3_CTL      ((DMACHx_CTL_STR*)(DMA_BASE + 0x6C))
#define DMA_CH3_SOCURR   REG32(DMA_BASE + 0x70)
#define DMA_CH3_DECURR   REG32(DMA_BASE + 0x74)
#define DMA_CH3_TCNT     REG32(DMA_BASE + 0x78)

// CH4 快捷宏
#define DMA_CH4_SRC      REG32(DMA_BASE + 0x80)
#define DMA_CH4_DEST     REG32(DMA_BASE + 0x84)
#define DMA_CH4_MAX      REG32(DMA_BASE + 0x88)
#define DMA_CH4_CTL      ((DMACHx_CTL_STR*)(DMA_BASE + 0x8C))
#define DMA_CH4_SOCURR   REG32(DMA_BASE + 0x90)
#define DMA_CH4_DECURR   REG32(DMA_BASE + 0x94)
#define DMA_CH4_TCNT     REG32(DMA_BASE + 0x98)

// CH5 快捷宏
#define DMA_CH5_SRC      REG32(DMA_BASE + 0xA0)
#define DMA_CH5_DEST     REG32(DMA_BASE + 0xA4)
#define DMA_CH5_MAX      REG32(DMA_BASE + 0xA8)
#define DMA_CH5_CTL      ((DMACHx_CTL_STR*)(DMA_BASE + 0xAC))
#define DMA_CH5_SOCURR   REG32(DMA_BASE + 0xB0)
#define DMA_CH5_DECURR   REG32(DMA_BASE + 0xB4)
#define DMA_CH5_TCNT     REG32(DMA_BASE + 0xB8)

// 全局寄存器快捷宏
#define DMA_MASK         REG32(DMA_BASE + 0xC0)
#define DMA_CLR          REG32(DMA_BASE + 0xC4)
#define DMA_STAT         REG32(DMA_BASE + 0xC8)
//#define DMA_CHCFG1       ((DMA_CHCFG1_STRx*)((DMA_BASE + 0xCC))
//#define DMA_CHCFG2       ((DMA_CHCFG2_STRx*)(DMA_BASE + 0xD0))

// -------------------------- 快捷操作宏（模仿GPIO位操作写法） --------------------------
// CH0 操作宏
#define DMA_CH0_ENABLE()           (DMA_CH0_CTL->w |= DMA_CTRL_EN)
#define DMA_CH0_DISABLE()          (DMA_CH0_CTL->w &= ~DMA_CTRL_EN)
#define DMA_CH0_SET_MEM2MEM()      (DMA_CH0_CTL->w |= DMA_CTRL_MEM2MEM)
#define DMA_CH0_SET_PER2MEM()      (DMA_CH0_CTL->w &= ~DMA_CTRL_MEM2PER)
#define DMA_CH0_SET_MEM2PER()      (DMA_CH0_CTL->w |= DMA_CTRL_MEM2PER)
#define DMA_CH0_SET_SRC_INC()      (DMA_CH0_CTL->w |= DMA_CTRL_SOINC)
#define DMA_CH0_CLR_SRC_INC()      (DMA_CH0_CTL->w &= ~DMA_CTRL_SOINC)
#define DMA_CH0_SET_DEST_INC()     (DMA_CH0_CTL->w |= DMA_CTRL_DEINC)
#define DMA_CH0_CLR_DEST_INC()     (DMA_CH0_CTL->w &= ~DMA_CTRL_DEINC)
#define DMA_CH0_CLEAR_FLAG()       (DMA_CLR |= DMA_CLEAR0)
#define DMA_CH0_ENABLE_INT()       (DMA_MASK |= DMA_MASK0)
#define DMA_CH0_DISABLE_INT()      (DMA_MASK &= ~DMA_MASK0)

// CH1 操作宏
#define DMA_CH1_ENABLE()     (DMA_CH1_CTL->w |= DMA_CTRL_EN)
#define DMA_CH1_DISABLE()    (DMA_CH1_CTL->w &= ~DMA_CTRL_EN)
#define DMA_CH1_CLEAR_FLAG() (DMA_CLR |= DMA_CLEAR1)
#define DMA_CH1_ENABLE_INT() (DMA_MASK |= DMA_MASK1)
#define DMA_CH1_DISABLE_INT()(DMA_MASK &= ~DMA_MASK1)

// CH2 操作宏
#define DMA_CH2_ENABLE()     (DMA_CH2_CTL->w |= DMA_CTRL_EN)
#define DMA_CH2_DISABLE()    (DMA_CH2_CTL->w &= ~DMA_CTRL_EN)
#define DMA_CH2_CLEAR_FLAG() (DMA_CLR |= DMA_CLEAR2)
#define DMA_CH2_ENABLE_INT() (DMA_MASK |= DMA_MASK2)
#define DMA_CH2_DISABLE_INT()(DMA_MASK &= ~DMA_MASK2)

// CH3 操作宏
#define DMA_CH3_ENABLE()     (DMA_CH3_CTL->w |= DMA_CTRL_EN)
#define DMA_CH3_DISABLE()    (DMA_CH3_CTL->w &= ~DMA_CTRL_EN)
#define DMA_CH3_CLEAR_FLAG() (DMA_CLR |= DMA_CLEAR3)
#define DMA_CH3_ENABLE_INT() (DMA_MASK |= DMA_MASK3)
#define DMA_CH3_DISABLE_INT()(DMA_MASK &= ~DMA_MASK3)

// CH4 操作宏
#define DMA_CH4_ENABLE()     (DMA_CH4_CTL->w |= DMA_CTRL_EN)
#define DMA_CH4_DISABLE()    (DMA_CH4_CTL->w &= ~DMA_CTRL_EN)
#define DMA_CH4_CLEAR_FLAG() (DMA_CLR |= DMA_CLEAR4)
#define DMA_CH4_ENABLE_INT() (DMA_MASK |= DMA_MASK4)
#define DMA_CH4_DISABLE_INT()(DMA_MASK &= ~DMA_MASK4)

// CH5 操作宏
#define DMA_CH5_ENABLE()     (DMA_CH5_CTL->w |= DMA_CTRL_EN)
#define DMA_CH5_DISABLE()    (DMA_CH5_CTL->w &= ~DMA_CTRL_EN)
#define DMA_CH5_CLEAR_FLAG() (DMA_CLR |= DMA_CLEAR5)
#define DMA_CH5_ENABLE_INT() (DMA_MASK |= DMA_MASK5)
#define DMA_CH5_DISABLE_INT()(DMA_MASK &= ~DMA_MASK5)

#endif // __DMA_TYPE_H__



