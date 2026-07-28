#ifndef __AMP_PGA_HAL_H__
#define __AMP_PGA_HAL_H__

#include <stdint.h>
#include <stdbool.h>

/*============================================================================*/
/*                    寄存器基地址定义                                         */
/*============================================================================*/

/* ANACTRL 模块基地址 (CMSDK_CM0.h 已定义 ANACTRL_BASE = 0x40005000) */
/* FOC 模块基地址   (CMSDK_CM0.h 已定义 FOC_BASE       = 0x40004C00) */
#include "CMSDK_CM0.h"

/* ANACTRL 寄存器偏移 */
#define ANACTRL_EN_OFFS             (0x00UL)    /* 0x40005000 - 使能寄存器 */
#define ANACTRL_OTHERS_OFFS         (0x08UL)    /* 0x40005008 - 其他配置寄存器 */

/* FOC 寄存器偏移 */
#define FOC_CTRL1_OFFS              (0x04UL)    /* 0x40004C04 - Ctrl1 寄存器 */
#define FOC_RO_AZ0_OFFS             (0x88UL)    /* 0x40004C88 - PGA0 调零结果 */
#define FOC_RO_AZ12_OFFS            (0x8CUL)    /* 0x40004C8C - PGA1/PGA2 调零结果 */

/*============================================================================*/
/*                    寄存器指针定义 (volatile uint32_t *)                     */
/*============================================================================*/

#define ANACTRL_EN                  ((volatile uint32_t *)(ANACTRL_BASE + ANACTRL_EN_OFFS))
#define ANACTRL_OTHERS              ((volatile uint32_t *)(ANACTRL_BASE + ANACTRL_OTHERS_OFFS))
#define FOC_CTRL1                   ((volatile uint32_t *)(FOC_BASE + FOC_CTRL1_OFFS))
#define FOC_RO_AZ0                  ((volatile uint32_t *)(FOC_BASE + FOC_RO_AZ0_OFFS))
#define FOC_RO_AZ12                 ((volatile uint32_t *)(FOC_BASE + FOC_RO_AZ12_OFFS))

/*============================================================================*/
/*                    ANACTRL EN 寄存器 (0x40005000) 位域                     */
/*============================================================================*/

#define ANACTRL_EN_ADC_Pos          (1)
#define ANACTRL_EN_ADC_Msk          (0x1UL << ANACTRL_EN_ADC_Pos)

#define ANACTRL_EN_AMP0_Pos         (3)
#define ANACTRL_EN_AMP0_Msk         (0x1UL << ANACTRL_EN_AMP0_Pos)

#define ANACTRL_EN_AMP1_Pos         (4)
#define ANACTRL_EN_AMP1_Msk         (0x1UL << ANACTRL_EN_AMP1_Pos)

#define ANACTRL_EN_AMP2_Pos         (5)
#define ANACTRL_EN_AMP2_Msk         (0x1UL << ANACTRL_EN_AMP2_Pos)

#define ANACTRL_EN_AMP_CMP_Pos      (6)
#define ANACTRL_EN_AMP_CMP_Msk      (0x1UL << ANACTRL_EN_AMP_CMP_Pos)

#define ANACTRL_EN_VHALF_Pos        (7)
#define ANACTRL_EN_VHALF_Msk        (0x1UL << ANACTRL_EN_VHALF_Pos)

#define ANACTRL_EN_VREF_Pos         (8)
#define ANACTRL_EN_VREF_Msk         (0x1UL << ANACTRL_EN_VREF_Pos)

#define ANACTRL_EN_PGA0_Pos         (11)
#define ANACTRL_EN_PGA0_Msk         (0x1UL << ANACTRL_EN_PGA0_Pos)

#define ANACTRL_EN_PGA1_Pos         (12)
#define ANACTRL_EN_PGA1_Msk         (0x1UL << ANACTRL_EN_PGA1_Pos)

#define ANACTRL_EN_PGA2_Pos         (13)
#define ANACTRL_EN_PGA2_Msk         (0x1UL << ANACTRL_EN_PGA2_Pos)

/*============================================================================*/
/*                    ANACTRL OTHERS 寄存器 (0x40005008) 位域 - PGA 增益      */
/*============================================================================*/

/* PGA 增益编码: 00=4X, 01=8X, 10=12X, 11=16X */
#define ANACTRL_OTHERS_PGA0_GAIN_Pos   (8)
#define ANACTRL_OTHERS_PGA0_GAIN_Msk   (0x03UL << ANACTRL_OTHERS_PGA0_GAIN_Pos)

#define ANACTRL_OTHERS_PGA1_GAIN_Pos   (10)
#define ANACTRL_OTHERS_PGA1_GAIN_Msk   (0x03UL << ANACTRL_OTHERS_PGA1_GAIN_Pos)

#define ANACTRL_OTHERS_PGA2_GAIN_Pos   (12)
#define ANACTRL_OTHERS_PGA2_GAIN_Msk   (0x03UL << ANACTRL_OTHERS_PGA2_GAIN_Pos)

/*============================================================================*/
/*                    FOC CTRL1 寄存器 (0x40004C04) 位域 - PGA 调零           */
/*============================================================================*/

#define FOC_CTRL1_AZ_EN_Pos            (16)
#define FOC_CTRL1_AZ_EN_Msk            (0x1UL << FOC_CTRL1_AZ_EN_Pos)

#define FOC_CTRL1_AZ_SEL_Pos           (17)
#define FOC_CTRL1_AZ_SEL_Msk           (0x07UL << FOC_CTRL1_AZ_SEL_Pos)

#define FOC_CTRL1_AZ_WAIT_NUM_Pos      (20)
#define FOC_CTRL1_AZ_WAIT_NUM_Msk      (0x0FUL << FOC_CTRL1_AZ_WAIT_NUM_Pos)

/*============================================================================*/
/*                    FOC 调零结果寄存器位域                                    */
/*============================================================================*/

#define FOC_RO_AZ0_DAT_Msk             (0x0FFFUL << 0)     /* bits 11:0 */
#define FOC_RO_AZ12_AZ1_DAT_Msk        (0x0FFFUL << 0)     /* bits 11:0 */
#define FOC_RO_AZ12_AZ2_DAT_Msk        (0x0FFFUL << 16)    /* bits 27:16 */

/*============================================================================*/
/*                    枚举类型定义                                             */
/*============================================================================*/

/** @brief PGA 实例选择 */
typedef enum {
    AMP_PGA_INST_0 = 0,     /**< PGA0 - ADC Channel 4 (母线/W相电流) */
    AMP_PGA_INST_1,         /**< PGA1 - ADC Channel 0 (U相电流) */
    AMP_PGA_INST_2,         /**< PGA2 - ADC Channel 1 (V相电流) */
    AMP_PGA_INST_MAX
} amp_pga_inst_e;

/** @brief AMP (运放) 实例选择 */
typedef enum {
    AMP_INST_0 = 0,
    AMP_INST_1,
    AMP_INST_2,
    AMP_INST_MAX
} amp_inst_e;

/*============================================================================*/
/*                    函数声明                                                 */
/*============================================================================*/

/*---------- AMP 运放控制 ----------*/

/**
 * @brief 使能指定 AMP 运放
 * @param inst AMP 实例 (AMP_INST_0/1/2)
 */
void amp_enable(amp_inst_e inst);

/**
 * @brief 禁用指定 AMP 运放
 * @param inst AMP 实例 (AMP_INST_0/1/2)
 */
void amp_disable(amp_inst_e inst);

/**
 * @brief 使能 AMP_CMP 模块
 */
void amp_cmp_enable(void);

/**
 * @brief 禁用 AMP_CMP 模块
 */
void amp_cmp_disable(void);

/*---------- PGA 控制 ----------*/

/**
 * @brief 使能指定 PGA
 * @param inst PGA 实例 (AMP_PGA_INST_0/1/2)
 */
void pga_enable(amp_pga_inst_e inst);

/**
 * @brief 禁用指定 PGA
 * @param inst PGA 实例 (AMP_PGA_INST_0/1/2)
 */
void pga_disable(amp_pga_inst_e inst);

/**
 * @brief 设置 PGA 增益
 * @param inst  PGA 实例 (AMP_PGA_INST_0/1/2)
 * @param gain  增益值: 0=4X, 1=8X, 2=12X, 3=16X
 */
void pga_set_gain(amp_pga_inst_e inst, uint32_t gain);

/*---------- PGA 调零 (Auto-Zero) ----------*/

/**
 * @brief 使能 PGA 调零功能
 *
 * 使能后，PGA 输入端短接至 0，测量输出偏置电压。
 * 需先调用 pga_auto_zero_set_wait() 设置等待时间。
 * 偏置结果通过 pga_read_offset_x() 读取。
 *
 * @param inst   PGA 实例 (AMP_PGA_INST_0/1/2)
 * @param enable true=使能, false=禁用
 */
void pga_auto_zero_enable(amp_pga_inst_e inst, bool enable);

/**
 * @brief 设置 PGA 调零等待时钟周期数
 *
 * 从使能到 ADC 采样发起之间的等待时间，时钟频率 48MHz。
 *
 * @param wait_num 等待周期数 (0~15)
 */
void pga_auto_zero_set_wait(uint32_t wait_num);

/**
 * @brief 读取 PGA0 调零偏置值 (ADC Channel 4)
 * @return 12-bit 偏置值
 */
uint16_t pga_read_offset_pga0(void);

/**
 * @brief 读取 PGA1 调零偏置值 (ADC Channel 0)
 * @return 12-bit 偏置值
 */
uint16_t pga_read_offset_pga1(void);

/**
 * @brief 读取 PGA2 调零偏置值 (ADC Channel 1)
 * @return 12-bit 偏置值
 */
uint16_t pga_read_offset_pga2(void);

/**
 * @brief PGA 系统初始化 (按规格配置所有使能位和默认增益)
 *
 * 寄存器配置:
 *   0x40005000 0x1 1       - ADC_EN      (bit 1)
 *   0x40005000 0x3 7 8     - VHALF_EN    (bit 7)
 *                             VREF_EN     (bit 8)
 *   0x40005000 0x7 3 5     - AMP0_EN     (bit 3)
 *                             AMP1_EN     (bit 4)
 *                             AMP2_EN     (bit 5)
 *   0x40005000 0x7 11 13   - PGA0_EN     (bit 11)
 *                             PGA1_EN     (bit 12)
 *                             PGA2_EN     (bit 13)
 *   0x40005008 0x02 8 9    - PGA0_GAIN   (bits 9:8  = 10b = 12X)
 *   0x40005008 0x02 10 11  - PGA1_GAIN   (bits 11:10 = 10b = 12X)
 *   0x40005008 0x02 12 13  - PGA2_GAIN   (bits 13:12 = 10b = 12X)
 */
void pga_system_init(void);

#endif /* __AMP_PGA_HAL_H__ */
