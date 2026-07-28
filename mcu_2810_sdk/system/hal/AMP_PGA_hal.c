#include "AMP_PGA_hal.h"

/*============================================================================*/
/*                    寄存器指针 (volatile uint32_t *)                        */
/*============================================================================*/

#define ANACTRL_EN_REG              ((volatile uint32_t *)(ANACTRL_BASE + 0x00UL))
#define ANACTRL_OTHERS_REG          ((volatile uint32_t *)(ANACTRL_BASE + 0x08UL))
#define FOC_CTRL1_REG               ((volatile uint32_t *)(FOC_BASE + 0x04UL))
#define FOC_RO_AZ0_REG              ((volatile uint32_t *)(FOC_BASE + 0x88UL))
#define FOC_RO_AZ12_REG             ((volatile uint32_t *)(FOC_BASE + 0x8CUL))

/*============================================================================*/
/*                    内部辅助函数：获取 AMP 使能位掩码                        */
/*============================================================================*/

static inline uint32_t amp_get_en_mask(amp_inst_e inst)
{
    switch (inst) {
        case AMP_INST_0: return ANACTRL_EN_AMP0_Msk;
        case AMP_INST_1: return ANACTRL_EN_AMP1_Msk;
        case AMP_INST_2: return ANACTRL_EN_AMP2_Msk;
        default:         return 0;
    }
}

/*============================================================================*/
/*                    内部辅助函数：获取 PGA 使能/增益位掩码                   */
/*============================================================================*/

static inline uint32_t pga_get_en_mask(amp_pga_inst_e inst)
{
    switch (inst) {
        case AMP_PGA_INST_0: return ANACTRL_EN_PGA0_Msk;
        case AMP_PGA_INST_1: return ANACTRL_EN_PGA1_Msk;
        case AMP_PGA_INST_2: return ANACTRL_EN_PGA2_Msk;
        default:             return 0;
    }
}

static inline uint32_t pga_get_gain_mask(amp_pga_inst_e inst)
{
    switch (inst) {
        case AMP_PGA_INST_0: return ANACTRL_OTHERS_PGA0_GAIN_Msk;
        case AMP_PGA_INST_1: return ANACTRL_OTHERS_PGA1_GAIN_Msk;
        case AMP_PGA_INST_2: return ANACTRL_OTHERS_PGA2_GAIN_Msk;
        default:             return 0;
    }
}

static inline uint32_t pga_get_gain_pos(amp_pga_inst_e inst)
{
    switch (inst) {
        case AMP_PGA_INST_0: return 8;
        case AMP_PGA_INST_1: return 10;
        case AMP_PGA_INST_2: return 12;
        default:             return 0;
    }
}

/*============================================================================*/
/*                    AMP 运放控制                                            */
/*============================================================================*/

/**
 * @brief 使能指定 AMP 运放模块
 *
 * 操作 ANACTRL EN 寄存器 (0x40005000) 对应位:
 *   AMP_INST_0 → bit 3 (RW_AMP0_EN)
 *   AMP_INST_1 → bit 4 (RW_AMP1_EN)
 *   AMP_INST_2 → bit 5 (RW_AMP2_EN)
 *
 * @param inst AMP 实例 (AMP_INST_0/1/2)
 */
void amp_enable(amp_inst_e inst)
{
    uint32_t mask = amp_get_en_mask(inst);
    if (mask) {
        *ANACTRL_EN_REG |= mask;
    }
}

/**
 * @brief 禁用指定 AMP 运放模块
 *
 * 操作 ANACTRL EN 寄存器 (0x40005000) 对应位。
 *
 * @param inst AMP 实例 (AMP_INST_0/1/2)
 */
void amp_disable(amp_inst_e inst)
{
    uint32_t mask = amp_get_en_mask(inst);
    if (mask) {
        *ANACTRL_EN_REG &= ~mask;
    }
}

/**
 * @brief 使能 AMP_CMP 模块
 *
 * 置位 ANACTRL EN 寄存器 (0x40005000) bit 6 (RW_AMP_CMP_EN)。
 * 该位控制模拟比较器与运放的整体使能。
 */
void amp_cmp_enable(void)
{
    *ANACTRL_EN_REG |= ANACTRL_EN_AMP_CMP_Msk;
}

/**
 * @brief 禁用 AMP_CMP 模块
 *
 * 清零 ANACTRL EN 寄存器 (0x40005000) bit 6 (RW_AMP_CMP_EN)。
 */
void amp_cmp_disable(void)
{
    *ANACTRL_EN_REG &= ~ANACTRL_EN_AMP_CMP_Msk;
}

/*============================================================================*/
/*                    PGA 控制                                                */
/*============================================================================*/

/**
 * @brief 使能指定 PGA 可编程增益放大器
 *
 * 操作 ANACTRL EN 寄存器 (0x40005000) 对应位:
 *   AMP_PGA_INST_0 → bit 11 (RW_PGA0_EN)
 *   AMP_PGA_INST_1 → bit 12 (RW_PGA1_EN)
 *   AMP_PGA_INST_2 → bit 13 (RW_PGA2_EN)
 *
 * @param inst PGA 实例 (AMP_PGA_INST_0/1/2)
 */
void pga_enable(amp_pga_inst_e inst)
{
    uint32_t mask = pga_get_en_mask(inst);
    if (mask) {
        *ANACTRL_EN_REG |= mask;
    }
}

/**
 * @brief 禁用指定 PGA 可编程增益放大器
 *
 * 操作 ANACTRL EN 寄存器 (0x40005000) 对应位。
 *
 * @param inst PGA 实例 (AMP_PGA_INST_0/1/2)
 */
void pga_disable(amp_pga_inst_e inst)
{
    uint32_t mask = pga_get_en_mask(inst);
    if (mask) {
        *ANACTRL_EN_REG &= ~mask;
    }
}

/**
 * @brief 设置 PGA 增益
 *
 * 操作 ANACTRL OTHERS 寄存器 (0x40005008) 对应位域:
 *   PGA0 → bits 9:8   偏移 8
 *   PGA1 → bits 11:10 偏移 10
 *   PGA2 → bits 13:12 偏移 12
 *
 * 增益编码: 00=4X, 01=8X, 10=12X, 11=16X
 *
 * @param inst  PGA 实例 (AMP_PGA_INST_0/1/2)
 * @param gain  增益值 (0~3): 0=4X, 1=8X, 2=12X, 3=16X
 */
void pga_set_gain(amp_pga_inst_e inst, uint32_t gain)
{
    uint32_t mask = pga_get_gain_mask(inst);
    uint32_t pos  = pga_get_gain_pos(inst);

    if (mask) {
        uint32_t reg_val = *ANACTRL_OTHERS_REG;
        reg_val &= ~mask;                            /* 清除原增益 */
        reg_val |= ((gain & 0x03) << pos);           /* 设置新增益 */
        *ANACTRL_OTHERS_REG = reg_val;
    }
}

/*============================================================================*/
/*                    PGA 调零控制                                            */
/*============================================================================*/

/**
 * @brief 使能/禁用 PGA 调零功能 (Auto-Zero)
 *
 * 操作 FOC CTRL1 寄存器 (0x40004C04):
 *   cfg_az_sel (bits 19:17): 位图选择要调零的 PGA
 *   cfg_az_en  (bit 16):     调零使能
 *
 * 使能后 PGA 输入端短接至 0，通过 ADC 测量输出偏置电压。
 * 调零前应调用 pga_auto_zero_set_wait() 设置等待时间。
 * 调零结果通过 pga_read_offset_pga0/1/2() 读取。
 *
 * @param inst   PGA 实例 (AMP_PGA_INST_0/1/2)
 * @param enable true=使能调零, false=禁用
 */
void pga_auto_zero_enable(amp_pga_inst_e inst, bool enable)
{
    if (enable) {
        uint32_t reg_val = *FOC_CTRL1_REG;

        /* cfg_az_sel (bits 19:17): 选择要调零的 PGA (位图) */
        reg_val &= ~FOC_CTRL1_AZ_SEL_Msk;
        reg_val |= ((uint32_t)(1 << (uint32_t)inst) << FOC_CTRL1_AZ_SEL_Pos);

        /* cfg_az_en (bit 16): 使能调零 */
        reg_val |= FOC_CTRL1_AZ_EN_Msk;

        *FOC_CTRL1_REG = reg_val;
    } else {
        /* 清除使能位 */
        *FOC_CTRL1_REG &= ~FOC_CTRL1_AZ_EN_Msk;
    }
}

/**
 * @brief 设置 PGA 调零等待时钟周期数
 *
 * 操作 FOC CTRL1 寄存器 (0x40004C04) cfg_az_wait_num (bits 23:20)。
 * 从使能调零到 ADC 发起采样之间的等待时间，时钟频率 48MHz。
 *
 * @param wait_num 等待周期数 (0~15, 单位: 48M 时钟周期)
 */
void pga_auto_zero_set_wait(uint32_t wait_num)
{
    uint32_t reg_val = *FOC_CTRL1_REG;
    reg_val &= ~FOC_CTRL1_AZ_WAIT_NUM_Msk;
    reg_val |= ((wait_num & 0x0F) << FOC_CTRL1_AZ_WAIT_NUM_Pos);
    *FOC_CTRL1_REG = reg_val;
}

/*============================================================================*/
/*                    PGA 调零偏置值读取                                      */
/*============================================================================*/

/**
 * @brief 读取 PGA0 调零偏置值
 *
 * 读取 FOC Ro_az0 寄存器 (0x40004C88) bits 11:0。
 * PGA0 对应 ADC Channel 4 (单电阻母线电流 / 三电阻 W 相电流)。
 *
 * @return 12-bit ADC 偏置值 (0~4095)
 */
uint16_t pga_read_offset_pga0(void)
{
    return (uint16_t)(*FOC_RO_AZ0_REG & 0x0FFF);
}

/**
 * @brief 读取 PGA1 调零偏置值
 *
 * 读取 FOC Ro_az12 寄存器 (0x40004C8C) bits 11:0。
 * PGA1 对应 ADC Channel 0 (三电阻 U 相电流)。
 *
 * @return 12-bit ADC 偏置值 (0~4095)
 */
uint16_t pga_read_offset_pga1(void)
{
    return (uint16_t)((*FOC_RO_AZ12_REG >> 0) & 0x0FFF);
}

/**
 * @brief 读取 PGA2 调零偏置值
 *
 * 读取 FOC Ro_az12 寄存器 (0x40004C8C) bits 27:16。
 * PGA2 对应 ADC Channel 1 (三电阻 V 相电流)。
 *
 * @return 12-bit ADC 偏置值 (0~4095)
 */
uint16_t pga_read_offset_pga2(void)
{
    return (uint16_t)((*FOC_RO_AZ12_REG >> 16) & 0x0FFF);
}

/*============================================================================*/
/*                    PGA 系统初始化                                           */
/*============================================================================*/

/**
 * @brief PGA 系统初始化 (按规格配置所有使能位和默认增益)
 *
 * 寄存器操作:
 *   0x40005000 0x1 1       - ADC_EN      (bit 1)  使能 ADC
 *   0x40005000 0x3 7 8     - VHALF_EN    (bit 7)  使能 VHALF (1/2 VDD)
 *                             VREF_EN     (bit 8)  使能 VREF (内部参考电压)
 *   0x40005000 0x7 3 5     - AMP0_EN     (bit 3)  使能 AMP0 运放
 *                             AMP1_EN     (bit 4)  使能 AMP1 运放
 *                             AMP2_EN     (bit 5)  使能 AMP2 运放
 *   0x40005000 0x7 11 13   - PGA0_EN     (bit 11) 使能 PGA0
 *                             PGA1_EN     (bit 12) 使能 PGA1
 *                             PGA2_EN     (bit 13) 使能 PGA2
 *   0x40005008 0x02 8 9    - PGA0_GAIN   (bits 9:8  = 10b = 12X)
 *   0x40005008 0x02 10 11  - PGA1_GAIN   (bits 11:10 = 10b = 12X)
 *   0x40005008 0x02 12 13  - PGA2_GAIN   (bits 13:12 = 10b = 12X)
 */
void pga_system_init(void)
{
    volatile uint32_t *en     = ANACTRL_EN_REG;
    volatile uint32_t *others = ANACTRL_OTHERS_REG;

    /* 0x40005000: 使能 ADC, VHALF, VREF, AMP0~2, PGA0~2 */
    *en |= ANACTRL_EN_ADC_Msk;         /* bit 1  — ADC_EN      */
    *en |= ANACTRL_EN_AMP0_Msk;        /* bit 3  — AMP0_EN     */
    *en |= ANACTRL_EN_AMP1_Msk;        /* bit 4  — AMP1_EN     */
    *en |= ANACTRL_EN_AMP2_Msk;        /* bit 5  — AMP2_EN     */
    *en |= ANACTRL_EN_VHALF_Msk;       /* bit 7  — VHALF_EN    */
    *en |= ANACTRL_EN_VREF_Msk;        /* bit 8  — VREF_EN     */
    *en |= ANACTRL_EN_PGA0_Msk;        /* bit 11 — PGA0_EN     */
    *en |= ANACTRL_EN_PGA1_Msk;        /* bit 12 — PGA1_EN     */
    *en |= ANACTRL_EN_PGA2_Msk;        /* bit 13 — PGA2_EN     */

    /* 0x40005008: 设置 PGA0/1/2 增益为 12X (默认) */
    *others &= ~(ANACTRL_OTHERS_PGA0_GAIN_Msk |
                 ANACTRL_OTHERS_PGA1_GAIN_Msk |
                 ANACTRL_OTHERS_PGA2_GAIN_Msk);
    *others |= (0x02UL << 8);   /* PGA0 bits 9:8 = 10b = 12X */
    *others |= (0x02UL << 10);  /* PGA1 bits 11:10 = 10b = 12X */
    *others |= (0x02UL << 12);  /* PGA2 bits 13:12 = 10b = 12X */
}
