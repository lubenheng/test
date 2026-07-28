#include "comp_hal.h"

/*============================================================================*/
/*                    辅助宏: COMP 实例寄存器指针                              */
/*============================================================================*/

/** @brief 获取 COMPx 指定偏移的寄存器指针 */
#define COMP_REGP(inst, offs)   ((volatile uint32_t *)((inst ## _BASE) + (offs)))

/** @brief 获取 COMPx CTRL0 寄存器指针 (offset 0x00) */
#define COMP_CTRL0(inst)        COMP_REGP(inst, 0x00UL)

/** @brief 获取 COMPx STAT 寄存器指针 (offset 0x04) */
#define COMP_STAT(inst)         COMP_REGP(inst, 0x04UL)

/** @brief 获取 COMPx INTCTL 寄存器指针 (offset 0x08) */
#define COMP_INTCTL(inst)       COMP_REGP(inst, 0x08UL)

/** @brief 获取 COMPx ITCFG 寄存器指针 (offset 0x0C) */
#define COMP_ITCFG(inst)        COMP_REGP(inst, 0x0CUL)

/*============================================================================*/
/*                    内部辅助函数                                           */
/*============================================================================*/

static inline uint32_t comp_get_base(comp_inst_e inst)
{
    switch (inst) {
        case COMP_INST_0: return COMP0_BASE;
        case COMP_INST_1: return COMP1_BASE;
        case COMP_INST_2: return COMP2_BASE;
        case COMP_INST_3: return COMP3_BASE;
        case COMP_INST_4: return COMP4_BASE;
        case COMP_INST_5: return COMP5_BASE;
        default:          return 0;
    }
}

/*============================================================================*/
/*                    时钟使能                                                */
/*============================================================================*/

/**
 * @brief 使能 CMP 模块时钟
 *
 * 操作 DIGCTRL clk_en0 寄存器 (0x40002408) bit 31 (cmp_CLK_EN)。
 */
void comp_clk_enable(void)
{
    *DIGCTRL_CLK_EN0_REG |= DIGCTRL_CMP_CLK_EN;
}

/**
 * @brief 禁用 CMP 模块时钟
 *
 * 清除 DIGCTRL clk_en0 寄存器 (0x40002408) bit 31。
 */
void comp_clk_disable(void)
{
    *DIGCTRL_CLK_EN0_REG &= ~DIGCTRL_CMP_CLK_EN;
}

/*============================================================================*/
/*                    CMP 使能/禁用                                           */
/*============================================================================*/

/**
 * @brief 使能 CMP0/1/2 整体模块
 *
 * 操作 COMP_GLB_CTRL0 寄存器 (0x40004408) bit 0 (COMP012_EN)。
 * 同时使能 CMP0、CMP1、CMP2 三个比较器。
 */
void comp012_enable(void)
{
    *COMP_GLB_CTRL0 |= CTRL0_COMP012_EN;
}

/**
 * @brief 禁用 CMP0/1/2 整体模块
 *
 * 清除 COMP_GLB_CTRL0 寄存器 (0x40004408) bit 0。
 */
void comp012_disable(void)
{
    *COMP_GLB_CTRL0 &= ~CTRL0_COMP012_EN;
}

/**
 * @brief 使能 CMP4 比较器
 *
 * 操作 COMP_GLB_CTRL0 寄存器 (0x40004408) bit 7 (COMP4_EN)。
 */
void comp4_enable(void)
{
    *COMP_GLB_CTRL0 |= CTRL0_CMP4_EN;
}

/**
 * @brief 禁用 CMP4 比较器
 */
void comp4_disable(void)
{
    *COMP_GLB_CTRL0 &= ~CTRL0_CMP4_EN;
}

/**
 * @brief 使能 CMP5 比较器
 *
 * 操作 COMP_GLB_CTRL1 寄存器 (0x40004410) bit 3 (COMP5_EN)。
 */
void comp5_enable(void)
{
    *COMP_GLB_CTRL1 |= CTRL1_CMP5_EN;
}

/**
 * @brief 禁用 CMP5 比较器
 */
void comp5_disable(void)
{
    *COMP_GLB_CTRL1 &= ~CTRL1_CMP5_EN;
}

/*============================================================================*/
/*                    CMPG 工作模式                                           */
/*============================================================================*/

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
void comp_set_cmpg_mode(uint32_t mode)
{
    uint32_t reg_val = *COMP_GLB_CTRL0;
    reg_val &= ~CTRL0_CMPG_MODE_Msk;
    reg_val |= ((mode & 0x03) << CTRL0_CMPG_MODE_Pos);
    *COMP_GLB_CTRL0 = reg_val;
}

/*============================================================================*/
/*                    CMP 迟滞设置                                            */
/*============================================================================*/

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
void comp_set_hysteresis(uint32_t hyst0, uint32_t hyst1, uint32_t hyst2)
{
    uint32_t reg_val = *COMP_GLB_CMPHYS;

    /* CMP0 bits 2:0 */
    reg_val &= ~CMPHYS_CMP0_HYST_Msk;
    reg_val |= ((hyst0 & 0x07) << CMPHYS_CMP0_HYST_Pos);

    /* CMP1 bits 9:7 */
    reg_val &= ~CMPHYS_CMP1_HYST_Msk;
    reg_val |= ((hyst1 & 0x07) << CMPHYS_CMP1_HYST_Pos);

    /* CMP2 bits 12:10 */
    reg_val &= ~CMPHYS_CMP2_HYST_Msk;
    reg_val |= ((hyst2 & 0x07) << CMPHYS_CMP2_HYST_Pos);

    *COMP_GLB_CMPHYS = reg_val;
}

/*============================================================================*/
/*                    CMP 中断设置                                            */
/*============================================================================*/

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
void comp_set_it_mode(comp_inst_e inst, uint32_t trig, bool en)
{
    uint32_t base = comp_get_base(inst);
    if (!base) return;

    volatile uint32_t *itcfg = (volatile uint32_t *)(base + 0x0CUL);
    uint32_t reg_val = 0;

    reg_val |= (trig & 0x03);           /* TRIG_SEL bits 1:0 */
    if (en) {
        reg_val |= (0x1UL << 2);        /* INT_EN bit 2 */
    }

    *itcfg = reg_val;
}

/**
 * @brief 清除比较器中断标志
 *
 * 操作 COMPx INTCTL 寄存器 (offset 0x08) bit 0 (写1清除)。
 *
 * @param inst 比较器实例 (COMP_INST_0~5)
 */
void comp_clear_it(comp_inst_e inst)
{
    uint32_t base = comp_get_base(inst);
    if (!base) return;

    volatile uint32_t *intctl = (volatile uint32_t *)(base + 0x08UL);
    *intctl = 0x1;     /* 写1清除中断 */
}

/*============================================================================*/
/*                    比较器输出路由                                          */
/*============================================================================*/

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
void comp_map_to_p13(uint32_t comp_sel)
{
    uint32_t reg_val = *ANACTRL_OTHERS_REG;
    reg_val &= ~ANACTRL_TEST0_SEL_Msk;
    reg_val |= ((comp_sel & 0x07) << ANACTRL_TEST0_SEL_Pos);
    *ANACTRL_OTHERS_REG = reg_val;
}

/**
 * @brief 映射比较器输出到 CXO/CXOS 测试引脚
 *
 * 操作 IO MUX 将比较器输出路由至 P03 (CXOS) 或 P44 (CXO) 引脚。
 *
 * @param pin_sel 引脚选择:
 *   0 - 路由至 P03 (CXOS)
 *   1 - 路由至 P44 (CXO)
 *
 * @note P03 在功能7 (CXOS), P44 在功能6 (CXO)。
 *       此操作需要配置对应 GPIO 的模拟/功能复用寄存器，此处仅作占位。
 */
void comp_map_to_cxo(uint32_t pin_sel)
{
    /* CXO/CXOS 路由需要配置 IO MUX, 当前未实现具体寄存器操作 */
    (void)pin_sel;
}

/*============================================================================*/
/*                    读取比较器输出                                          */
/*============================================================================*/

/**
 * @brief 读取指定比较器的输出电平
 *
 * 操作 COMPx STAT 寄存器 (offset 0x04) bit 0 (OUTPUT)。
 *
 * @param inst 比较器实例 (COMP_INST_0~5)
 * @return 0=低电平, 1=高电平
 */
uint32_t comp_read_output(comp_inst_e inst)
{
    uint32_t base = comp_get_base(inst);
    if (!base) return 0;

    volatile uint32_t *stat = (volatile uint32_t *)(base + 0x04UL);
    return ((*stat) & 0x01);
}

/*============================================================================*/
/*                    CMP 测试函数                                            */
/*============================================================================*/

/**
 * @brief CMP 测试函数 (选择 CMP0/1/2 配置输出到 P13)
 *
 * 根据形参选择配置的比较器:
 *   comp_test(0) - CMP0 迟滞 +5mV, CMP0 输出到 P13
 *   comp_test(1) - CMP1 迟滞 +5mV, CMP1 输出到 P13
 *   comp_test(2) - CMP2 迟滞 +5mV, CMP2 输出到 P13
 *
 * 寄存器操作:
 *   0x4000141C 0xff 8 15   - P1 模拟使能
 *   0x4000141C 0xff 16 23  - P2 模拟使能
 *   0x4000141C 0xff 24 31  - P3 模拟使能
 *   0x40004408 0x1 0 0     - CMP0/1/2 使能
 *   0x40004408 0x1 7 7     - CMP4 使能
 *   0x40004410 0x1 3 3     - CMP5 使能
 *   0x40004408 0x02 5 6    - 三差分比较器模式
 *   0x40005000 0x01 15 15  - RW_TEST0_EN
 *   0x40005000 0x1 6 6     - RW_AMP_CMP_EN
 *   0x40002408 0x01 31 31  - cmp_CLK_EN
 *   迟滞: 可选 COMP0/1/2 各 +5mV (CMPG_HYST_5M_POS)
 *   输出: 可选 COMP0/1/2 输出到 P13 (TEST0_SEL=1/2/3)
 *
 * @param cmp_sel 比较器选择: 0=CMP0, 1=CMP1, 2=CMP2
 */
void comp_test(uint32_t cmp_sel)
{
    /* 1. IO ana (0x4000141C): P1/P2/P3 引脚模拟使能 */
    *IO_ANA_REG |= (IO_ANA_P1_EN | IO_ANA_P2_EN | IO_ANA_P3_EN);

    /* 2. COMP_GLB_CTRL0 (0x40004408):
     *    - bit 0:      CMP0/1/2 使能
     *    - bits 6:5:   三差分比较器模式 (CMPG_MODE_3DIFF = 0x02)
     *    - bit 7:      CMP4 使能 */
    *COMP_GLB_CTRL0 &= ~CTRL0_CMPG_MODE_Msk;
    *COMP_GLB_CTRL0 |= (CTRL0_COMP012_EN |
                        (CMPG_MODE_3DIFF << CTRL0_CMPG_MODE_Pos) |
                        CTRL0_CMP4_EN);

    /* 3. COMP_GLB_CTRL1 (0x40004410): CMP5 使能 */
    *COMP_GLB_CTRL1 |= CTRL1_CMP5_EN;

    /* 4. ANACTRL EN (0x40005000): TEST0_EN + AMP_CMP 使能 */
    *ANACTRL_EN_REG |= (ANACTRL_EN_TEST0 | ANACTRL_EN_AMP_CMP);

    /* 5. DIGCTRL clk_en0 (0x40002408): cmp_CLK_EN */
    *DIGCTRL_CLK_EN0_REG |= DIGCTRL_CMP_CLK_EN;

    /* 6. 配置迟滞: 选中的比较器设为 +5mV (CMPG_HYST_5M_POS = 0x04) */
    {
        uint32_t hyst0 = 0, hyst1 = 0, hyst2 = 0;
        switch (cmp_sel) {
            case 0: hyst0 = CMPG_HYST_5M_POS; break;
            case 1: hyst1 = CMPG_HYST_5M_POS; break;
            case 2: hyst2 = CMPG_HYST_5M_POS; break;
            default: break;
        }
        comp_set_hysteresis(hyst0, hyst1, hyst2);
    }

    /* 7. 映射选中的比较器输出到 P13 (0x40005008 TEST0_SEL bits 18:16) */
    {
        uint32_t sel_val = 0;
        switch (cmp_sel) {
            case 0: sel_val = 1; break;   /* CMP0 -> P13 */
            case 1: sel_val = 2; break;   /* CMP1 -> P13 */
            case 2: sel_val = 3; break;   /* CMP2 -> P13 */
            default: break;
        }
        comp_map_to_p13(sel_val);
    }
}

/*============================================================================*/
/*                    系统初始化                                              */
/*============================================================================*/

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
void comp_system_init(void)
{
    /* 1. IO ana (0x4000141C): P1/P2/P3 引脚模拟使能 */
    *IO_ANA_REG |= (IO_ANA_P1_EN | IO_ANA_P2_EN | IO_ANA_P3_EN);

    /* 2. COMP_GLB_CTRL0 (0x40004408):
     *    - bit 0:      CMP0/1/2 使能
     *    - bits 6:5:   三差分比较器模式 (0x02)
     *    - bit 7:      CMP4 使能 */
    *COMP_GLB_CTRL0 &= ~CTRL0_CMPG_MODE_Msk;
    *COMP_GLB_CTRL0 |= (CTRL0_COMP012_EN |
                        (CMPG_MODE_3DIFF << CTRL0_CMPG_MODE_Pos) |
                        CTRL0_CMP4_EN);

    /* 3. COMP_GLB_CTRL1 (0x40004410): CMP5 使能 */
    *COMP_GLB_CTRL1 |= CTRL1_CMP5_EN;

    /* 4. ANACTRL EN (0x40005000): TEST0_EN + AMP_CMP 使能 */
    *ANACTRL_EN_REG |= (ANACTRL_EN_TEST0 | ANACTRL_EN_AMP_CMP);

    /* 5. DIGCTRL clk_en0 (0x40002408): cmp_CLK_EN */
    *DIGCTRL_CLK_EN0_REG |= DIGCTRL_CMP_CLK_EN;
}

