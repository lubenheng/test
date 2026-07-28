#ifndef __COMP_HAL_H__
#define __COMP_HAL_H__

#include <stdint.h>
#include <stdbool.h>

/*============================================================================*/
/*                    寄存器基地址定义                                         */
/*============================================================================*/

/* APB 基地址 (CMSDK_CM0.h 已定义 APB_BASE = 0x40000000) */
#include "CMSDK_CM0.h"

/* COMP 全局寄存器 (CMP 模块: 0x40004400) */
#define COMP_GLB_BASE               (APB_BASE + 0x4400UL)

/* COMP 各实例基地址 (每个 COMP 占用 0x100) */
#define COMP0_BASE                  (APB_BASE + 0x6400UL)
#define COMP1_BASE                  (APB_BASE + 0x6500UL)
#define COMP2_BASE                  (APB_BASE + 0x6600UL)
#define COMP3_BASE                  (APB_BASE + 0x6700UL)
#define COMP4_BASE                  (APB_BASE + 0x6800UL)
#define COMP5_BASE                  (APB_BASE + 0x6900UL)

/* 全局寄存器偏移 */
#define COMP_GLB_CMPHYS_OFFS        (0x04UL)    /* 0x40004404 - CMPG 迟滞 */
#define COMP_GLB_CTRL0_OFFS         (0x08UL)    /* 0x40004408 - 全局控制 0 */
#define COMP_GLB_CTRL1_OFFS         (0x10UL)    /* 0x40004410 - 全局控制 1 */

/* 各 COMP 内部寄存器偏移 */
#define COMP_CTRL0_OFFS             (0x00UL)    /* 控制寄存器 */
#define COMP_STAT_OFFS              (0x04UL)    /* 状态寄存器 */
#define COMP_INTCTL_OFFS            (0x08UL)    /* 中断控制 */
#define COMP_ITCFG_OFFS             (0x0CUL)    /* 中断配置 */

/* 系统级寄存器基地址 */
#define IO_BASE                     (APB_BASE + 0x1400UL)  /* 0x40001400 */
#define DIGCTRL_BASE                (APB_BASE + 0x2400UL)  /* 0x40002400 */

/*============================================================================*/
/*                    寄存器指针 (volatile uint32_t *)                        */
/*============================================================================*/

/* 全局寄存器指针 */
#define COMP_GLB_CMPHYS             ((volatile uint32_t *)(COMP_GLB_BASE + COMP_GLB_CMPHYS_OFFS))
#define COMP_GLB_CTRL0              ((volatile uint32_t *)(COMP_GLB_BASE + COMP_GLB_CTRL0_OFFS))
#define COMP_GLB_CTRL1              ((volatile uint32_t *)(COMP_GLB_BASE + COMP_GLB_CTRL1_OFFS))

/* 系统级寄存器指针 */
#define IO_ANA_REG                  ((volatile uint32_t *)(IO_BASE + 0x1CUL))        /* 0x4000141C */
#define ANACTRL_EN_REG              ((volatile uint32_t *)(ANACTRL_BASE + 0x00UL))   /* 0x40005000 */
#define ANACTRL_OTHERS_REG          ((volatile uint32_t *)(ANACTRL_BASE + 0x08UL))   /* 0x40005008 */
#define DIGCTRL_CLK_EN0_REG         ((volatile uint32_t *)(DIGCTRL_BASE + 0x08UL))   /* 0x40002408 */

/* 各 COMP 实例寄存器指针 */
#define COMP_REG(inst, offs)        ((volatile uint32_t *)((inst ## _BASE) + (offs)))

/*============================================================================*/
/*                    COMP 全局控制寄存器位域                                  */
/*============================================================================*/

/* COMP_GLB_CMPHYS (0x40004404) */
#define CMPHYS_CMP0_HYST_Msk        (0x07UL << 0)       /* bits 2:0  */
#define CMPHYS_CMP0_HYST_Pos        (0)
#define CMPHYS_CMP1_HYST_Msk        (0x07UL << 7)       /* bits 9:7  */
#define CMPHYS_CMP1_HYST_Pos        (7)
#define CMPHYS_CMP2_HYST_Msk        (0x07UL << 10)      /* bits 12:10 */
#define CMPHYS_CMP2_HYST_Pos        (10)

/* COMP_GLB_CTRL0 (0x40004408) */
#define CTRL0_COMP012_EN            (0x1UL << 0)         /* bit 0: CMP0/1/2 整体使能 */
#define CTRL0_CMPG_MODE_Msk         (0x03UL << 5)        /* bits 6:5: CMPG 模式 */
#define CTRL0_CMPG_MODE_Pos         (5)
#define CTRL0_CMP4_EN               (0x1UL << 7)         /* bit 7: CMP4 使能 */

/* COMP_GLB_CTRL1 (0x40004410) */
#define CTRL1_CMP5_EN               (0x1UL << 3)         /* bit 3: CMP5 使能 */

/* IO ana (0x4000141C) */
#define IO_ANA_P1_EN                (0xFFUL << 8)        /* bits 15:8  */
#define IO_ANA_P2_EN                (0xFFUL << 16)       /* bits 23:16 */
#define IO_ANA_P3_EN                (0xFFUL << 24)       /* bits 31:24 */

/* ANACTRL EN (0x40005000) */
#define ANACTRL_EN_AMP_CMP          (0x1UL << 6)         /* bit 6 */
#define ANACTRL_EN_TEST0            (0x1UL << 15)        /* bit 15 */

/* DIGCTRL clk_en0 (0x40002408) */
#define DIGCTRL_CMP_CLK_EN          (0x01UL << 31)       /* bit 31 */

/* ANACTRL OTHERS (0x40005008) TEST0_SEL */
#define ANACTRL_TEST0_SEL_Msk       (0x07UL << 16)       /* bits 18:16 */
#define ANACTRL_TEST0_SEL_Pos       (16)

/*============================================================================*/
/*                    CMPG 迟滞电压编码                                       */
/*============================================================================*/

#define CMPG_HYST_NONE       (0x0U)  /* 无迟滞 */
#define CMPG_HYST_2M5        (0x1U)  /* ±2.5mV */
#define CMPG_HYST_5M_NEG     (0x2U)  /* -5mV */
#define CMPG_HYST_5M_POS     (0x4U)  /* +5mV */
#define CMPG_HYST_5M         (0x3U)  /* ±5mV */
#define CMPG_HYST_10M_NEG    (0x5U)  /* -10mV */
#define CMPG_HYST_10M_POS    (0x6U)  /* +10mV */
#define CMPG_HYST_10M        (0x7U)  /* ±10mV */

/*============================================================================*/
/*                    CMPG 工作模式编码                                       */
/*============================================================================*/

#define CMPG_MODE_3CMP_NORES  (0x0U)  /* 无内置电阻三比较器 */
#define CMPG_MODE_3CMP_RES    (0x1U)  /* 有内置电阻三比较器 */
#define CMPG_MODE_3DIFF       (0x2U)  /* 三差分比较器 */
#define CMPG_MODE_DUAL        (0x3U)  /* 双比较器 */

/*============================================================================*/
/*                    枚举类型定义                                             */
/*============================================================================*/

/** @brief 比较器实例 */
typedef enum {
    COMP_INST_0 = 0,
    COMP_INST_1,
    COMP_INST_2,
    COMP_INST_3,
    COMP_INST_4,
    COMP_INST_5,
    COMP_INST_MAX
} comp_inst_e;

/** @brief 比较器输出值 */
typedef enum {
    COMP_OUTPUT_LOW  = 0,
    COMP_OUTPUT_HIGH = 1
} comp_output_e;

/*============================================================================*/
/*                    函数声明                                                 */
/*============================================================================*/

/*---------- 时钟使能 ----------*/

/**
 * @brief 使能 CMP 模块时钟
 *
 * 操作 DIGCTRL clk_en0 寄存器 (0x40002408) bit 31 (cmp_CLK_EN)。
 */
void comp_clk_enable(void);

/**
 * @brief 禁用 CMP 模块时钟
 *
 * 清除 DIGCTRL clk_en0 寄存器 (0x40002408) bit 31。
 */
void comp_clk_disable(void);

/*---------- CMP 使能/禁用 ----------*/

/**
 * @brief 使能 CMP0/1/2 整体模块
 *
 * 操作 COMP_GLB_CTRL0 寄存器 (0x40004408) bit 0 (COMP012_EN)。
 * 同时使能 CMP0、CMP1、CMP2 三个比较器。
 */
void comp012_enable(void);

/**
 * @brief 禁用 CMP0/1/2 整体模块
 *
 * 清除 COMP_GLB_CTRL0 寄存器 (0x40004408) bit 0。
 */
void comp012_disable(void);

/**
 * @brief 使能 CMP4 比较器
 *
 * 操作 COMP_GLB_CTRL0 寄存器 (0x40004408) bit 7 (COMP4_EN)。
 */
void comp4_enable(void);

/**
 * @brief 禁用 CMP4 比较器
 */
void comp4_disable(void);

/**
 * @brief 使能 CMP5 比较器
 *
 * 操作 COMP_GLB_CTRL1 寄存器 (0x40004410) bit 3 (COMP5_EN)。
 */
void comp5_enable(void);

/**
 * @brief 禁用 CMP5 比较器
 */
void comp5_disable(void);

/*---------- CMPG 工作模式 ----------*/

/**
 * @brief 设置 CMPG 比较器组工作模式
 *
 * 操作 COMP_GLB_CTRL0 寄存器 (0x40004408) bits 6:5。
 *
 * @param mode 模式编码:
 *   0 (CMPG_MODE_3CMP_NORES) - 无内置电阻三比较器
 *   1 (CMPG_MODE_3CMP_RES)   - 有内置电阻三比较器
 *   2 (CMPG_MODE_3DIFF)      - 三差分比较器
 *   3 (CMPG_MODE_DUAL)       - 双比较器
 */
void comp_set_cmpg_mode(uint32_t mode);

/*---------- CMP 迟滞设置 ----------*/

/**
 * @brief 设置 CMP0/1/2 迟滞电压
 *
 * 操作 COMP_GLB_CMPHYS 寄存器 (0x40004404):
 *   CMP0 bits 2:0, CMP1 bits 9:7, CMP2 bits 12:10
 *
 * @param hyst0  CMP0 迟滞编码 (0~7), 使用 CMPG_HYST_xxx 宏
 * @param hyst1  CMP1 迟滞编码 (0~7)
 * @param hyst2  CMP2 迟滞编码 (0~7)
 */
void comp_set_hysteresis(uint32_t hyst0, uint32_t hyst1, uint32_t hyst2);

/*---------- CMP 中断设置 ----------*/

/**
 * @brief 设置 CMP0/1/2/3 中断触发模式
 *
 * 操作 COMPx ITCFG 寄存器 (offset 0x0C):
 *   TRIG_SEL (bits 1:0): 0=上升沿, 1=下降沿, 2=双边沿
 *   INT_EN   (bit 2):    中断使能
 *
 * @param inst  比较器实例 (COMP_INST_0/1/2/3)
 * @param trig  触发模式: 0=上升沿, 1=下降沿, 2=双边沿
 * @param en    true=使能中断, false=禁用
 */
void comp_set_it_mode(comp_inst_e inst, uint32_t trig, bool en);

/**
 * @brief 清除比较器中断标志
 *
 * 操作 COMPx INTCTL 寄存器 (offset 0x08) bit 0 (写1清除)。
 *
 * @param inst 比较器实例 (COMP_INST_0~5)
 */
void comp_clear_it(comp_inst_e inst);

/*---------- 比较器输出路由 ----------*/

/**
 * @brief 映射比较器输出到 P13 测试引脚 (用于调试)
 *
 * 操作 ANACTRL OTHERS 寄存器 (0x40005008) TEST0_SEL bits 18:16。
 * 需同时使能 ANACTRL EN 的 TEST0_EN (bit 15) 和 AMP_CMP (bit 6)。
 *
 * @param comp_sel 比较器选择:
 *   0 - 无输出
 *   1 - CMP0 输出到 P13
 *   2 - CMP1 输出到 P13
 *   3 - CMP2 输出到 P13
 */
void comp_map_to_p13(uint32_t comp_sel);

/**
 * @brief 映射比较器输出到 CXO/CXOS 测试引脚
 *
 * 操作 IO MUX 将比较器输出路由至 P03 (CXOS) 或 P44 (CXO) 引脚。
 *
 * @param pin_sel 引脚选择:
 *   0 - 路由至 P03 (CXOS)
 *   1 - 路由至 P44 (CXO)
 */
void comp_map_to_cxo(uint32_t pin_sel);

/*---------- 读取比较器输出 ----------*/

/**
 * @brief 读取指定比较器的输出电平
 *
 * 操作 COMPx STAT 寄存器 (offset 0x04) bit 0 (OUTPUT)。
 *
 * @param inst 比较器实例 (COMP_INST_0~5)
 * @return 0=低电平, 1=高电平
 */
uint32_t comp_read_output(comp_inst_e inst);

/*---------- 系统初始化 ----------*/

/**
 * @brief CMP 系统初始化 (使能所有必要的模拟和时钟资源)
 *
 * 寄存器操作:
 *   0x4000141C 0xff 8-15   - P1 模拟使能
 *   0x4000141C 0xff 16-23  - P2 模拟使能
 *   0x4000141C 0xff 24-31  - P3 模拟使能
 *   0x40004408 0x1 0       - CMP0/1/2 使能
 *   0x40004408 0x02 5 6    - 三差分比较器模式
 *   0x40004408 0x1 7       - CMP4 使能
 *   0x40004410 0x1 3       - CMP5 使能
 *   0x40005000 0x01 15     - TEST0_EN
 *   0x40005000 0x1  6      - AMP_CMP_EN
 *   0x40002408 0x01 31     - cmp_CLK_EN
 */
void comp_system_init(void);

/*---------- 测试函数 ----------*/

/**
 * @brief CMP 测试函数
 *
 * 根据形参选择配置对应的比较器:
 *   comp_test(0) - 配置 CMP0 迟滞 +5mV, CMP0 输出到 P13
 *   comp_test(1) - 配置 CMP1 迟滞 +5mV, CMP1 输出到 P13
 *   comp_test(2) - 配置 CMP2 迟滞 +5mV, CMP2 输出到 P13
 *
 * @param cmp_sel 比较器选择: 0=CMP0, 1=CMP1, 2=CMP2
 */
void comp_test(uint32_t cmp_sel);

#endif /* __COMP_HAL_H__ */
