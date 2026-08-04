#include "platform_common.h"

#include "clock_reg.h"
#include "clock_type.h"
#include "device_resource.h"
#include "clock.h"


/**********************************************************
 * rcc mco clock control
***********************************************************/
#if 1

/**
 *@brief Select the clock source for MCO
 *@param[in] rcc_ptr: Defined rcc variable
*@param[in] sel: The types of MCO clock sources are as follows:
                MCO_CLOCK_SYS
	            MCO_RESERVED1
	            MCO_CLOCK_HSI
	            MCO_CLOCK_PLLCLK
	            MCO_CLOCK_HSE
	            MCO_CLOCK_LSI
 *@return none
 */
void rcc_drv_mco_clock_select(RCC_TypeDef *rcc_ptr, clock_mco_select sel)
{
    switch (sel)
    {
    case CLOCK_MCO_SYS:
        rcc_ptr->MCOCR_STR.MCOSRC = 0;
        break;
    case CLOCK_MCO_HSI:
        rcc_ptr->MCOCR_STR.MCOSRC = 2;
        break;
    case CLOCK_MCO_PLLCLK:
        rcc_ptr->MCOCR_STR.MCOSRC = 3;
        break;
    case CLOCK_MCO_HSE:
        rcc_ptr->MCOCR_STR.MCOSRC = 4;
        break;
    case CLOCK_MCO_LSI:
        rcc_ptr->MCOCR_STR.MCOSRC = 5;
        break;
    default:
        break;
    }
}

/**
 *@brief MCO Clock division
 *@param[in] *rcc_ptr: Defined rcc variable
 *@param[in] div: Please enter a value from 0 to 8, 
                  corresponding to a division ratio from 1 to 512.
 *@return    none
 */
void rcc_drv_mco_div(RCC_TypeDef *rcc_ptr, uint8_t div)
{
		//close old div num
		rcc_ptr->MCOCR_STR.MCOPRE &= 0x0;

		//enter new div num
        rcc_ptr->MCOCR_STR.MCOPRE |= div;
}

/**
 *@brief Get the current MCO clock source
 *@param[in] rcc_ptr: Defined rcc variable
 *@return    clock_mco_select: current MCO clock source
 */
clock_mco_select rcc_drv_get_mco_clock_status(RCC_TypeDef *rcc_ptr)
{
    clock_mco_select sel = CLOCK_MCO_SYS;

    switch (rcc_ptr->MCOCR_STR.MCOSRC)
    {
    case 0: sel = CLOCK_MCO_SYS;       break;
    case 2: sel = CLOCK_MCO_HSI;       break;
    case 3: sel = CLOCK_MCO_PLLCLK;    break;
    case 4: sel = CLOCK_MCO_HSE;       break;
    case 5: sel = CLOCK_MCO_LSI;       break;
    default:                           break;
    }

    return sel;
}

/**
 *@brief Get the current MCO division ratio
 *@param[in] rcc_ptr: Defined rcc variable
 *@return    Current MCO division ratio (MCOPRE), output freq = source / 2^MCOPRE
 */
uint8_t rcc_drv_get_mco_div(RCC_TypeDef *rcc_ptr)
{
    /* 与 rcc_drv_mco_div() 写入的字段保持一致 */
    return (uint8_t)(rcc_ptr->MCOCR_STR.MCOPRE);
}

/**
 *@brief Get the current MCO output frequency
 *@param[in] rcc_ptr: Defined rcc variable
 *@return    MCO output frequency in Hz (source / 2^MCOPRE)
 */
uint32_t rcc_drv_get_mco_freq(RCC_TypeDef *rcc_ptr)
{
    uint32_t src_freq = 0U;

    switch (rcc_drv_get_mco_clock_status(rcc_ptr))
    {
    case CLOCK_MCO_SYS:    src_freq = rcc_drv_get_sysclk_freq(rcc_ptr); break;
    case CLOCK_MCO_HSI:    src_freq = HIRC_FREQ;                        break;
    case CLOCK_MCO_PLLCLK: src_freq = rcc_drv_pll_freq(rcc_ptr);        break;
    case CLOCK_MCO_HSE:    src_freq = OSC_CURRENT_FREQ;                 break;
    case CLOCK_MCO_LSI:    src_freq = LIRC_FREQ;                        break;
    default:                                                           break;
    }

    return src_freq >> rcc_drv_get_mco_div(rcc_ptr);
}
#endif


/**********************************************************
 * rcc system clock & PLL control
***********************************************************/
#if 1
/**
 *@brief Internal system clock selection
 *@param[in] *rcc_ptr: Defined rcc variable
 *@param[in] sel: 
            	CLOCK_SYS_HSI    :内部时钟32MHz
	            CLOCK_SYS_HSE    :外部晶振
	            CLOCK_SYS_PLLCLK1：内部PLL时钟
	            CLOCK_SYS_PLLCLK2：内部PLL时钟

 *@return    none
 */
void rcc_drv_sys_select_clock(RCC_TypeDef *rcc_ptr,clock_sys_select sel)
{
    switch (sel)
    {
    case CLOCK_SYS_HSI:
        rcc_ptr->SYSCKCR.SYSCKSW = 0;
        break;
    case CLOCK_SYS_HSE:
        rcc_ptr->SYSCKCR.SYSCKSW = 1;
        break;
    case CLOCK_SYS_PLLCLK1:
    case CLOCK_SYS_PLLCLK2:
        rcc_ptr->SYSCKCR.SYSCKSW = 2;
        break;
    default:
        break;
    }
}

/**
 *@brief Check which source the system clock is using
 *@param[in] rcc_ptr: Defined rcc variable
 *@return    Returns the enumeration type of the clock source, as follows:
                CLOCK_SYS_HSI
                CLOCK_SYS_HSE
                CLOCK_SYS_PLLCLK1
 */
clock_sys_select rcc_drv_sys_clock_status(RCC_TypeDef *rcc_ptr)
{
    if(rcc_ptr->SYSCKCR.SYSCKSW == 0)
	{
        return CLOCK_SYS_HSI;
    } 
	else if(rcc_ptr->SYSCKCR.SYSCKSW == 1)
	{
        return CLOCK_SYS_HSE;
    } 
	else if(rcc_ptr->SYSCKCR.SYSCKSW == 2)
	{
        return CLOCK_SYS_PLLCLK1;
    } 
	else if(rcc_ptr->SYSCKCR.SYSCKSW == 3)
	{
        return CLOCK_SYS_PLLCLK2;
    }
	else
	{
	    return CLOCK_SYS_RESERD;
	}
}

/**
 *@brief Divide the system clock to get fclk
 *@param[in] *rcc_ptr: Defined rcc variable
 *@param[in] div: 0x0~0xf(0~15)
 *@return    none
 */
void rcc_drv_sys_div_fclk(RCC_TypeDef *rcc_ptr, uint8_t div)
{
    //FCLK = SYSCLK / (HPRE + 1)
    rcc_ptr->SYSCKCR.HPRE = div;
}

/**
 *@brief Divide fclk to get pclk
 *@param[in] *rcc_ptr: Defined rcc variable
 *@param[in] div: 0x0~0xf(0~15)
 *@return    none
 */
void rcc_drv_sys_div_pclk(RCC_TypeDef *rcc_ptr, uint8_t div)
{
    //PCLK = FCLK / (PPRE + 1)
    rcc_ptr->SYSCKCR.PPRE = div;
}

/**
 *@brief Get the current FCLK division ratio
 *@param[in] rcc_ptr: Defined rcc variable
 *@return    Current FCLK division ratio, valid range 0x0~0xf (FCLK = SYSCLK / (value + 1))
 */
uint8_t rcc_drv_get_fclk_div(RCC_TypeDef *rcc_ptr)
{
    /* 与 rcc_drv_sys_div_fclk() 写入的字段保持一致 */
    return (uint8_t)(rcc_ptr->SYSCKCR.HPRE);
}

/**
 *@brief Get the current PCLK division ratio
 *@param[in] rcc_ptr: Defined rcc variable
 *@return    Current PCLK division ratio, valid range 0x0~0xf (PCLK = FCLK / (value + 1))
 */
uint8_t rcc_drv_get_pclk_div(RCC_TypeDef *rcc_ptr)
{
    /* 与 rcc_drv_sys_div_pclk() 写入的字段保持一致 */
    return (uint8_t)(rcc_ptr->SYSCKCR.PPRE);
}

/**
 *@brief One-stop system clock configuration: clock source + FCLK/PCLK dividers
 *@param[in] rcc_ptr:   Defined rcc variable
 *@param[in] sel:       System clock source (CLOCK_SYS_HSI/HSE/PLLCLK1/PLLCLK2)
 *@param[in] fclk_div:  FCLK divider, valid range 0x0~0xf, FCLK = SYSCLK / (fclk_div + 1)
 *@param[in] pclk_div:  PCLK divider, valid range 0x0~0xf, PCLK = FCLK / (pclk_div + 1)
 *@return    Clock configure result status:
 *                 - ERC_CLK_SYS_SRC_INVALID: clock source invalid
 *                 - ERC_CLK_DIV_INVALID: divider out of range
 *                 - ERC_CLK_NO_ERROR: success
 */
clk_erc_e rcc_drv_sys_clock_config(RCC_TypeDef *rcc_ptr,
                                   clock_sys_select sel,
                                   uint8_t fclk_div, uint8_t pclk_div)
{
    /* 校验时钟源：CLOCK_SYS_RESERD 是无效哨兵值 */
    if (sel >= CLOCK_SYS_RESERD)
    {
        return ERC_CLK_SYS_SRC_INVALID;
    }

    /* 校验分频系数：HPRE/PPRE 均为 4 位，最大值 0xf */
    if ((fclk_div > 0x0FU) || (pclk_div > 0x0FU))
    {
        return ERC_CLK_DIV_INVALID;
    }

    /* 先设分频、再切时钟源，避免切换瞬间出现过高频率 */
    rcc_ptr->SYSCKCR.HPRE = fclk_div;
    rcc_ptr->SYSCKCR.PPRE = pclk_div;
    rcc_drv_sys_select_clock(rcc_ptr, sel);

    return ERC_CLK_NO_ERROR;
}

/**
 *@brief Switch system clock source safely (verify target ready before switching)
 *@param[in] rcc_ptr: Defined rcc variable
 *@param[in] sel:     Target system clock source (CLOCK_SYS_HSI/HSE/PLLCLK1/PLLCLK2)
 *@return    Clock configure result status:
 *                 - ERC_CLK_SYS_SRC_INVALID: clock source invalid
 *                 - ERC_CLK_OSC_NOT_READY: target oscillator not ready (HSE not stable / PLL not locked)
 *                 - ERC_CLK_NO_ERROR: success
 */
clk_erc_e rcc_drv_sys_switch_clock(RCC_TypeDef *rcc_ptr, clock_sys_select sel)
{
    if (sel >= CLOCK_SYS_RESERD)
    {
        return ERC_CLK_SYS_SRC_INVALID;
    }

    /* 切源前先确认目标时钟源就绪，否则系统时钟会丢失/跑飞 */
    switch (sel)
    {
    case CLOCK_SYS_HSE:
        /* HSE 晶振起振需要时间，未就绪时禁止切换 */
        if (rcc_drv_hse_ready(rcc_ptr) == 0U)
        {
            return ERC_CLK_OSC_NOT_READY;
        }
        break;
    case CLOCK_SYS_PLLCLK1:
    case CLOCK_SYS_PLLCLK2:
        /* 需要 PLL 已使能且锁定 */
        if ((rcc_ptr->PLLCR0.PLLEN == 0U) || (rcc_drv_pll_ready(rcc_ptr) == 0U))
        {
            return ERC_CLK_OSC_NOT_READY;
        }
        break;
    default:
        /* HSI 为内部 RC，上电即就绪，无需等待 */
        break;
    }

    rcc_drv_sys_select_clock(rcc_ptr, sel);
    return ERC_CLK_NO_ERROR;
}

/**
 *@brief Get the current system clock frequency (SYSCLK)
 *@param[in] rcc_ptr: Defined rcc variable
 *@return    System clock frequency in Hz:
 *                 - HSI source: HIRC_FREQ (内部 RC 频率，见 sys_config.h)
 *                 - HSE source: OSC_CURRENT_FREQ (外部晶振频率)
 *                 - PLL source: PLL_FREQ (未定义时按 HSI 近似)
 */
uint32_t rcc_drv_get_sysclk_freq(RCC_TypeDef *rcc_ptr)
{
    uint32_t sysclk = HIRC_FREQ;

    switch (rcc_drv_sys_clock_status(rcc_ptr))
    {
    case CLOCK_SYS_HSI:
        sysclk = HIRC_FREQ;
        break;
    case CLOCK_SYS_HSE:
        sysclk = OSC_CURRENT_FREQ;
        break;
    case CLOCK_SYS_PLLCLK1:
    case CLOCK_SYS_PLLCLK2:
#ifdef PLL_FREQ
        sysclk = PLL_FREQ;
#else
        /* ASIC 模式未定义 PLL_FREQ，按 HSI 频率近似 */
        sysclk = HIRC_FREQ;
#endif
        break;
    default:
        break;
    }

    return sysclk;
}

/**
 *@brief Get the current FCLK frequency
 *@param[in] rcc_ptr: Defined rcc variable
 *@return    FCLK frequency in Hz (FCLK = SYSCLK / (HPRE + 1))
 */
uint32_t rcc_drv_get_fclk_freq(RCC_TypeDef *rcc_ptr)
{
    return (uint32_t)(rcc_drv_get_sysclk_freq(rcc_ptr) /
                      (rcc_drv_get_fclk_div(rcc_ptr) + 1U));
}

/**
 *@brief Get the current PCLK frequency
 *@param[in] rcc_ptr: Defined rcc variable
 *@return    PCLK frequency in Hz (PCLK = FCLK / (PPRE + 1))
 */
uint32_t rcc_drv_get_pclk_freq(RCC_TypeDef *rcc_ptr)
{
    return rcc_drv_get_fclk_freq(rcc_ptr) /
           (rcc_drv_get_pclk_div(rcc_ptr) + 1U);
}

/**********************************************************
 * PLL 控制（PLLCR0 / PLLCR1）
 *   推荐顺序：先关 PLL → 配置倍频/分频 → 使能 → 等待锁定 → 切换系统时钟
***********************************************************/

/**
 *@brief Enable/Disable the PLL
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    none
 */
void rcc_drv_pll_enable(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->PLLCR0.PLLEN = 1U;
}

//这里由于设计问题，PLLRDY位置1之后会关闭PLL，PLLEN置0不能关闭PLL，等设计那边修改之后这里再修改point
void rcc_drv_pll_disable(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->PLLCR0.PLLRDY = 1U;
}

/**
 *@brief Configure the PLL multiplier and output divider
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@param[in] mult: PLLMULT, 5-bit multiplier, valid range 0x00~0x1F
 *@param[in] div:  PLLDIV, 3-bit output divider, valid range 0x00~0x07
 *@return    Clock configure result status:
 *                 - ERC_CLK_DIV_INVALID: parameter out of range
 *                 - ERC_CLK_NO_ERROR: success
 *@note   配置前建议先关闭 PLL，避免配置中间态导致输出频率异常
 */
clk_erc_e rcc_drv_pll_config(RCC_TypeDef *rcc_ptr, uint8_t mult, uint8_t div)
{
    if ((mult > 0x1FU) || (div > 0x07U))
    {
        return ERC_CLK_DIV_INVALID;
    }

    rcc_ptr->PLLCR1.PLLDIV = div;
    rcc_ptr->PLLCR1.PLLMULT = mult;

    return ERC_CLK_NO_ERROR;
}

/**
 *@brief Get the PLL lock ready flag
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    RO_PLL_RDY: 1 = PLL 已锁定，0 = 未锁定
 */
uint8_t rcc_drv_pll_ready(RCC_TypeDef *rcc_ptr)
{
    /* RO_PLL_RDY = 1 表示 PLL 已锁定，可作为系统时钟源 */
    return (rcc_ptr->PLLCR0.RO_PLL_RDY);
}

/**
 *@brief Get the PLL error latch flag
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    RO_PLL_ERR_LAT: 1 = PLL 出现错误被锁存，0 = 无错误
 */
uint8_t rcc_drv_pll_lock_error(RCC_TypeDef *rcc_ptr)
{
    return (rcc_ptr->PLLCR0.RO_PLL_ERR_LAT);
}

/**
 *@brief Wait for the PLL to lock, with timeout
 *@param[in] rcc_ptr:    Pointer to RCC peripheral registers
 *@param[in] timeout_us: Wait timeout in microseconds
 *@return    Clock configure result status:
 *                 - ERC_CLK_OSC_NOT_READY: timeout, PLL not locked
 *                 - ERC_CLK_NO_ERROR: success
 */
clk_erc_e rcc_drv_pll_wait_ready(RCC_TypeDef *rcc_ptr, uint32_t timeout_us)
{
    uint32_t timeout = timeout_us;

    while (rcc_drv_pll_ready(rcc_ptr) == 0U)
    {
        if (--timeout == 0U)
        {
            return ERC_CLK_OSC_NOT_READY;
        }
    }

    return ERC_CLK_NO_ERROR;
}

/**
 *@brief Get the PLL output frequency
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    PLL output frequency in Hz
 *@note   PLL_FREQ 已定义时直接返回宏值；否则按 PLLCLK = PLLSRC × PLLMULT / PLLDIV 估算，
 *        PLL 输入源暂取当前系统时钟源（HSI/HSE），精确公式待 datasheet 确认。
 */
uint32_t rcc_drv_pll_freq(RCC_TypeDef *rcc_ptr)
{
#ifdef PLL_FREQ
    return PLL_FREQ;
#else
    uint32_t src_freq;
    uint8_t  mult = (uint8_t)(rcc_ptr->PLLCR1.PLLMULT);
    uint8_t  div  = (uint8_t)(rcc_ptr->PLLCR1.PLLDIV);

    switch (rcc_drv_sys_clock_status(rcc_ptr))
    {
    case CLOCK_SYS_HSE:
        src_freq = OSC_CURRENT_FREQ;
        break;
    default:
        src_freq = HIRC_FREQ;
        break;
    }

    if (div == 0U)
    {
        return src_freq * mult;   /* 无分频，避免除零 */
    }
    return src_freq * mult / div;
#endif
}

/**
 *@brief One-stop system clock configuration: PLL + source + FCLK/PCLK dividers
 *@param[in] rcc_ptr:   Pointer to RCC peripheral registers
 *@param[in] sel:       System clock source (CLOCK_SYS_HSI/HSE/PLLCLK1/PLLCLK2)
 *@param[in] pll_mult:  PLL multiplier (PLLMULT), ignored if target is not PLL
 *@param[in] pll_div:   PLL output divider (PLLDIV), ignored if target is not PLL
 *@param[in] fclk_div:  FCLK divider, valid range 0x0~0xf, FCLK = SYSCLK / (fclk_div + 1)
 *@param[in] pclk_div:  PCLK divider, valid range 0x0~0xf, PCLK = FCLK / (pclk_div + 1)
 *@return    Clock configure result status:
 *                 - ERC_CLK_SYS_SRC_INVALID: clock source invalid
 *                 - ERC_CLK_DIV_INVALID: divider out of range
 *                 - ERC_CLK_OSC_NOT_READY: PLL not locked within timeout
 *                 - ERC_CLK_NO_ERROR: success
 */
clk_erc_e rcc_drv_sys_clock_full_config(RCC_TypeDef *rcc_ptr,
                                        clock_sys_select sel,
                                        uint8_t pll_mult, uint8_t pll_div,
                                        uint8_t fclk_div, uint8_t pclk_div)
{
    clk_erc_e erc = ERC_CLK_NO_ERROR;

    /* 校验时钟源：CLOCK_SYS_RESERD 是无效哨兵值 */
    if (sel >= CLOCK_SYS_RESERD)
    {
        return ERC_CLK_SYS_SRC_INVALID;
    }

    /* 校验分频系数：HPRE/PPRE 均为 4 位，最大值 0xf */
    if ((fclk_div > 0x0FU) || (pclk_div > 0x0FU))
    {
        return ERC_CLK_DIV_INVALID;
    }

    /* 目标为 PLL：先配置、使能并等待锁定，再切换系统时钟源 */
    if ((sel == CLOCK_SYS_PLLCLK1) || (sel == CLOCK_SYS_PLLCLK2))
    {
        erc = rcc_drv_pll_config(rcc_ptr, pll_mult, pll_div);
        if (erc != ERC_CLK_NO_ERROR)
        {
            return erc;
        }
        rcc_drv_pll_enable(rcc_ptr);
        erc = rcc_drv_pll_wait_ready(rcc_ptr, RCC_PLL_LOCK_TIMEOUT_US);
        if (erc != ERC_CLK_NO_ERROR)
        {
            return erc;
        }
    }

    /* 先设分频、再切时钟源，避免切换瞬间出现过高频率 */
    rcc_ptr->SYSCKCR.HPRE = fclk_div;
    rcc_ptr->SYSCKCR.PPRE = pclk_div;
    rcc_drv_sys_select_clock(rcc_ptr, sel);

    return ERC_CLK_NO_ERROR;
}

#endif

/**********************************************************
 * rcc Oscillator Control
 * 振荡器控制：HSI/LSI/HSE/LSE 的使能、旁路、故障检测与就绪查询
 * HSI: 内部 32MHz RC，上电默认时钟源
 * LSI: 内部 32KHz RC，通常用作 IWDT 喂狗时钟（电机保护必需）
 * HSE: 外部高速晶振/外部时钟输入
 * LSE: 外部低速晶振，典型 32.768KHz
***********************************************************/
#if 1

/**
 *@brief Enable the HSI (internal 32MHz RC oscillator)
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    none
 */
void rcc_drv_hsi_enable(RCC_TypeDef *rcc_ptr)
{
    /* HSIEN = 1: 使能内部 32MHz RC 时钟 */
    rcc_ptr->HSICR.HSIEN = 1;
}

/**
 *@brief Disable the HSI (internal 32MHz RC oscillator)
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    none
 */
void rcc_drv_hsi_disable(RCC_TypeDef *rcc_ptr)
{
    /* HSIEN = 0: 关闭内部 32MHz RC 时钟 */
    rcc_ptr->HSICR.HSIEN = 0;
}

/**
 *@brief Get the HSI ready flag
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    HSIRDY: 1 = HSI 已稳定，0 = 未就绪（需等待）
 */
uint8_t rcc_drv_hsi_ready(RCC_TypeDef *rcc_ptr)
{
    /* HSIRDY 为只读就绪标志 */
    return (rcc_ptr->HSICR.HSIRDY);
}

/**
 *@brief Enable the LSI (internal 32KHz RC oscillator)
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    none
 */
void rcc_drv_lsi_enable(RCC_TypeDef *rcc_ptr)
{
    /* LSIEN = 1: 使能内部 32KHz RC 时钟（IWDT 喂狗时钟） */
    rcc_ptr->LSICR_STR.LSIEN = 1;
}

/**
 *@brief Disable the LSI (internal 32KHz RC oscillator)
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    none
 */
void rcc_drv_lsi_disable(RCC_TypeDef *rcc_ptr)
{
    /* LSIEN = 0: 关闭内部 32KHz RC 时钟 */
    rcc_ptr->LSICR_STR.LSIEN = 0;
}

/**
 *@brief Get the LSI ready flag
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    LSIRDY: 1 = LSI 已稳定，0 = 未就绪（需等待）
 */
uint8_t rcc_drv_lsi_ready(RCC_TypeDef *rcc_ptr)
{
    return (rcc_ptr->LSICR_STR.LSIRDY);
}

/**
 *@brief Enable the HSE (external high-speed oscillator)
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    none
 */
void rcc_drv_hse_enable(RCC_TypeDef *rcc_ptr)
{
    /* HSEEN = 1: 使能外部晶振 */
    rcc_ptr->HSECR.HSEEN = 1;
}

/**
 *@brief Disable the HSE (external high-speed oscillator)
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    none
 */
void rcc_drv_hse_disable(RCC_TypeDef *rcc_ptr)
{
    /* HSEEN = 0: 关闭外部晶振 */
    rcc_ptr->HSECR.HSEEN = 0;
}

/**
 *@brief Select HSE bypass mode (external clock input or crystal)
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@param[in] bypass: 1 = 外部时钟输入模式(HSEXSE=1)，0 = 正常晶振模式
 *@return    none
 */
void rcc_drv_hse_bypass(RCC_TypeDef *rcc_ptr, uint8_t bypass)
{
    /* HSEXSE: 1 = 旁路晶振，从外部引脚直接输入时钟 */
    rcc_ptr->HSECR.HSEXSE = (bypass) ? 1 : 0;
}

/**
 *@brief Enable/Disable HSE failure detection
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@param[in] en: 1 = 使能 HSE 故障检测，0 = 关闭
 *@return    none
 */
void rcc_drv_hse_fail_detect(RCC_TypeDef *rcc_ptr, uint8_t en)
{
    /* HSEDETE: 使能后晶振失效可配合 SYSRSTE.HSEFAILRSTE 触发系统复位 */
    rcc_ptr->HSECR.HSEDETE = (en) ? 1 : 0;
}

/**
 *@brief Get the HSE ready flag
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    HSERDY: 1 = HSE 已稳定，0 = 未就绪（晶振起振需要时间）
 */
uint8_t rcc_drv_hse_ready(RCC_TypeDef *rcc_ptr)
{
    return (rcc_ptr->HSECR.HSERDY);
}

/**
 *@brief Enable the LSE (external low-speed oscillator)
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    none
 */
void rcc_drv_lse_enable(RCC_TypeDef *rcc_ptr)
{
    /* LSEEN = 1: 使能外部低速晶振，典型 32.768KHz */
    rcc_ptr->LSECR.LSEEN = 1;
}

/**
 *@brief Disable the LSE (external low-speed oscillator)
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    none
 */
void rcc_drv_lse_disable(RCC_TypeDef *rcc_ptr)
{
    /* LSEEN = 0: 关闭外部低速晶振 */
    rcc_ptr->LSECR.LSEEN = 0;
}

/**
 *@brief Select LSE bypass mode (external clock input or crystal)
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@param[in] bypass: 1 = 外部时钟输入模式(LSEXSE=1)，0 = 正常晶振模式
 *@return    none
 */
void rcc_drv_lse_bypass(RCC_TypeDef *rcc_ptr, uint8_t bypass)
{
    /* LSEXSE: 1 = 旁路晶振，从外部引脚直接输入时钟 */
    rcc_ptr->LSECR.LSEXSE = (bypass) ? 1 : 0;
}

/**
 *@brief Get the LSE ready flag
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    LSERDY: 1 = LSE 已稳定，0 = 未就绪（需等待）
 */
uint8_t rcc_drv_lse_ready(RCC_TypeDef *rcc_ptr)
{
    return (rcc_ptr->LSECR.LSERDY);
}

/**
 *@brief Wait for the HSI to become ready, with timeout
 *@param[in] rcc_ptr:    Pointer to RCC peripheral registers
 *@param[in] timeout_us: Wait timeout in microseconds
 *@return    Clock configure result status:
 *                 - ERC_CLK_OSC_NOT_READY: timeout, HSI not ready
 *                 - ERC_CLK_NO_ERROR: success
 */
clk_erc_e rcc_drv_hsi_wait_ready(RCC_TypeDef *rcc_ptr, uint32_t timeout_us)
{
    uint32_t timeout = timeout_us;

    while (rcc_drv_hsi_ready(rcc_ptr) == 0U)
    {
        if (--timeout == 0U)
        {
            return ERC_CLK_OSC_NOT_READY;
        }
    }

    return ERC_CLK_NO_ERROR;
}

/**
 *@brief Wait for the LSI to become ready, with timeout
 *@param[in] rcc_ptr:    Pointer to RCC peripheral registers
 *@param[in] timeout_us: Wait timeout in microseconds
 *@return    Clock configure result status:
 *                 - ERC_CLK_OSC_NOT_READY: timeout, LSI not ready
 *                 - ERC_CLK_NO_ERROR: success
 */
clk_erc_e rcc_drv_lsi_wait_ready(RCC_TypeDef *rcc_ptr, uint32_t timeout_us)
{
    uint32_t timeout = timeout_us;

    while (rcc_drv_lsi_ready(rcc_ptr) == 0U)
    {
        if (--timeout == 0U)
        {
            return ERC_CLK_OSC_NOT_READY;
        }
    }

    return ERC_CLK_NO_ERROR;
}

/**
 *@brief Wait for the HSE to become ready, with timeout
 *@param[in] rcc_ptr:    Pointer to RCC peripheral registers
 *@param[in] timeout_us: Wait timeout in microseconds
 *@return    Clock configure result status:
 *                 - ERC_CLK_OSC_NOT_READY: timeout, HSE not ready (晶振未起振)
 *                 - ERC_CLK_NO_ERROR: success
 */
clk_erc_e rcc_drv_hse_wait_ready(RCC_TypeDef *rcc_ptr, uint32_t timeout_us)
{
    uint32_t timeout = timeout_us;

    while (rcc_drv_hse_ready(rcc_ptr) == 0U)
    {
        if (--timeout == 0U)
        {
            return ERC_CLK_OSC_NOT_READY;
        }
    }

    return ERC_CLK_NO_ERROR;
}

/**
 *@brief Wait for the LSE to become ready, with timeout
 *@param[in] rcc_ptr:    Pointer to RCC peripheral registers
 *@param[in] timeout_us: Wait timeout in microseconds
 *@return    Clock configure result status:
 *                 - ERC_CLK_OSC_NOT_READY: timeout, LSE not ready
 *                 - ERC_CLK_NO_ERROR: success
 */
clk_erc_e rcc_drv_lse_wait_ready(RCC_TypeDef *rcc_ptr, uint32_t timeout_us)
{
    uint32_t timeout = timeout_us;

    while (rcc_drv_lse_ready(rcc_ptr) == 0U)
    {
        if (--timeout == 0U)
        {
            return ERC_CLK_OSC_NOT_READY;
        }
    }

    return ERC_CLK_NO_ERROR;
}

#endif


/**********************************************************
 * rcc peripheral clock enable / reset / clock source control
 * 外设时钟使能、复位与外设时钟源选择统一控制
 *   核心：rcc_drv_periph_* 统一接口，按 rcc_periph_e 枚举映射到各寄存器位
 *   兼容：原有 rcc_drv_enable_* / rcc_drv_rst_* 保留为薄封装
 *   PCKS0：ADC0/MCPWM0/CAN0 的功能时钟源选择
 *   注：复位寄存器为 wo，若 datasheet 确认为"写1自动清零"的脉冲复位，
 *   则 rcc_drv_periph_reset_release() 无需调用（保留以防电平式复位）
***********************************************************/
#if 1

/**********************************************************
 * 统一接口：外设时钟使能 / 关闭 / 复位 / 复位释放
***********************************************************/

/**
 *@brief Enable peripheral clock (set CLK_EN bit)
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@param[in] periph:  rcc_periph_e, peripheral to enable
 *@return    none
 */
void rcc_drv_periph_clk_enable(RCC_TypeDef *rcc_ptr, rcc_periph_e periph)
{
    switch (periph)
    {
    case RCC_PERIPH_FLC:     rcc_ptr->AHB_CLK_EN.FLC_CLK_EN = 1;     break;
    case RCC_PERIPH_GPIOA:   rcc_ptr->AHB_CLK_EN.GPIOA_CLK_EN = 1;   break;
    case RCC_PERIPH_GPIOB:   rcc_ptr->AHB_CLK_EN.GPIOB_CLK_EN = 1;   break;
    case RCC_PERIPH_GPIOC:   rcc_ptr->AHB_CLK_EN.GPIOC_CLK_EN = 1;   break;
    case RCC_PERIPH_ADC0:    rcc_ptr->ANA_CLK_EN.ADC0_CLK_EN = 1;    break;
    case RCC_PERIPH_MATH:    rcc_ptr->DIG_CLK_EN.MATH_CLK_EN = 1;    break;
    case RCC_PERIPH_HALL:    rcc_ptr->DIG_CLK_EN.HALL_CLK_EN = 1;    break;
    case RCC_PERIPH_QEP:     rcc_ptr->DIG_CLK_EN.QEP_CLK_EN = 1;     break;
    case RCC_PERIPH_IWDT:    rcc_ptr->DIG_CLK_EN.IWDT_CLK_EN = 1;    break;
    case RCC_PERIPH_UART0:   rcc_ptr->COM_CLK_EN.UART0_CLK_EN = 1;   break;
    case RCC_PERIPH_UART1:   rcc_ptr->COM_CLK_EN.UART1_CLK_EN = 1;   break;
    case RCC_PERIPH_SPI0:    rcc_ptr->COM_CLK_EN.SPI0_CLK_EN = 1;    break;
    case RCC_PERIPH_SPI1:    rcc_ptr->COM_CLK_EN.SPI1_CLK_EN = 1;    break;
    case RCC_PERIPH_IIC0:    rcc_ptr->COM_CLK_EN.IIC0_CLK_EN = 1;    break;
    case RCC_PERIPH_IIC1:    rcc_ptr->COM_CLK_EN.IIC1_CLK_EN = 1;    break;
    case RCC_PERIPH_TIM13:   rcc_ptr->TIM_CLK_EN.TIM13_CLK_EN = 1;   break;
    case RCC_PERIPH_TIM14:   rcc_ptr->TIM_CLK_EN.TIM14_CLK_EN = 1;   break;
    case RCC_PERIPH_TIM15:   rcc_ptr->TIM_CLK_EN.TIM15_CLK_EN = 1;   break;
    case RCC_PERIPH_TIM16:   rcc_ptr->TIM_CLK_EN.TIM16_CLK_EN = 1;   break;
    case RCC_PERIPH_MCPWM0:  rcc_ptr->TIM_CLK_EN.MCPWM0_CLK_EN = 1;  break;
    default: break;
    }
}

/**
 *@brief Disable peripheral clock (clear CLK_EN bit)
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@param[in] periph:  rcc_periph_e, peripheral to disable
 *@return    none
 */
void rcc_drv_periph_clk_disable(RCC_TypeDef *rcc_ptr, rcc_periph_e periph)
{
    switch (periph)
    {
    case RCC_PERIPH_FLC:     rcc_ptr->AHB_CLK_EN.FLC_CLK_EN = 0;     break;
    case RCC_PERIPH_GPIOA:   rcc_ptr->AHB_CLK_EN.GPIOA_CLK_EN = 0;   break;
    case RCC_PERIPH_GPIOB:   rcc_ptr->AHB_CLK_EN.GPIOB_CLK_EN = 0;   break;
    case RCC_PERIPH_GPIOC:   rcc_ptr->AHB_CLK_EN.GPIOC_CLK_EN = 0;   break;
    case RCC_PERIPH_ADC0:    rcc_ptr->ANA_CLK_EN.ADC0_CLK_EN = 0;    break;
    case RCC_PERIPH_MATH:    rcc_ptr->DIG_CLK_EN.MATH_CLK_EN = 0;    break;
    case RCC_PERIPH_HALL:    rcc_ptr->DIG_CLK_EN.HALL_CLK_EN = 0;    break;
    case RCC_PERIPH_QEP:     rcc_ptr->DIG_CLK_EN.QEP_CLK_EN = 0;     break;
    case RCC_PERIPH_IWDT:    rcc_ptr->DIG_CLK_EN.IWDT_CLK_EN = 0;    break;
    case RCC_PERIPH_UART0:   rcc_ptr->COM_CLK_EN.UART0_CLK_EN = 0;   break;
    case RCC_PERIPH_UART1:   rcc_ptr->COM_CLK_EN.UART1_CLK_EN = 0;   break;
    case RCC_PERIPH_SPI0:    rcc_ptr->COM_CLK_EN.SPI0_CLK_EN = 0;    break;
    case RCC_PERIPH_SPI1:    rcc_ptr->COM_CLK_EN.SPI1_CLK_EN = 0;    break;
    case RCC_PERIPH_IIC0:    rcc_ptr->COM_CLK_EN.IIC0_CLK_EN = 0;    break;
    case RCC_PERIPH_IIC1:    rcc_ptr->COM_CLK_EN.IIC1_CLK_EN = 0;    break;
    case RCC_PERIPH_TIM13:   rcc_ptr->TIM_CLK_EN.TIM13_CLK_EN = 0;   break;
    case RCC_PERIPH_TIM14:   rcc_ptr->TIM_CLK_EN.TIM14_CLK_EN = 0;   break;
    case RCC_PERIPH_TIM15:   rcc_ptr->TIM_CLK_EN.TIM15_CLK_EN = 0;   break;
    case RCC_PERIPH_TIM16:   rcc_ptr->TIM_CLK_EN.TIM16_CLK_EN = 0;   break;
    case RCC_PERIPH_MCPWM0:  rcc_ptr->TIM_CLK_EN.MCPWM0_CLK_EN = 0;  break;
    default: break;
    }
}

/**
 *@brief Assert peripheral reset (write 1 to RST bit)
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@param[in] periph:  rcc_periph_e, peripheral to reset
 *@return    none
 */
void rcc_drv_periph_reset(RCC_TypeDef *rcc_ptr, rcc_periph_e periph)
{
    switch (periph)
    {
    case RCC_PERIPH_FLC:     rcc_ptr->AHBRSSTR.FLC = 1;         break;
    case RCC_PERIPH_GPIOA:   rcc_ptr->AHBRSSTR.GPIOARST = 1;    break;
    case RCC_PERIPH_GPIOB:   rcc_ptr->AHBRSSTR.GPIOBRST = 1;    break;
    case RCC_PERIPH_GPIOC:   rcc_ptr->AHBRSSTR.GPIOCRST = 1;    break;
    case RCC_PERIPH_ADC0:    rcc_ptr->ANARST.ADC_RST = 1;       break;
    case RCC_PERIPH_MATH:    rcc_ptr->DIGRST.MATHRST = 1;       break;
    case RCC_PERIPH_HALL:    rcc_ptr->DIGRST.HALLRST = 1;       break;
    case RCC_PERIPH_QEP:     rcc_ptr->DIGRST.QEPRST = 1;        break;
    case RCC_PERIPH_IWDT:    rcc_ptr->DIGRST.IWDTRST = 1;       break;
    case RCC_PERIPH_UART0:   rcc_ptr->COMRST.UART0RST = 1;      break;
    case RCC_PERIPH_UART1:   rcc_ptr->COMRST.UART1RST = 1;      break;
    case RCC_PERIPH_SPI0:    rcc_ptr->COMRST.SPI0RST = 1;       break;
    case RCC_PERIPH_SPI1:    rcc_ptr->COMRST.SPI1RST = 1;       break;
    case RCC_PERIPH_IIC0:    rcc_ptr->COMRST.IIC0RST = 1;       break;
    case RCC_PERIPH_IIC1:    rcc_ptr->COMRST.IIC1RST = 1;       break;
    case RCC_PERIPH_TIM13:   rcc_ptr->TIMRST.TIM13RST = 1;      break;
    case RCC_PERIPH_TIM14:   rcc_ptr->TIMRST.TIM14RST = 1;      break;
    case RCC_PERIPH_TIM15:   rcc_ptr->TIMRST.TIM15RST = 1;      break;
    case RCC_PERIPH_TIM16:   rcc_ptr->TIMRST.TIM16RST = 1;      break;
    case RCC_PERIPH_MCPWM0:  rcc_ptr->TIMRST.MCPWM0RST = 1;     break;
    default: break;
    }
}

/**
 *@brief Release peripheral reset (write 0 to RST bit)
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@param[in] periph:  rcc_periph_e, peripheral to release reset
 *@return    none
 */
void rcc_drv_periph_reset_release(RCC_TypeDef *rcc_ptr, rcc_periph_e periph)
{
    switch (periph)
    {
    case RCC_PERIPH_FLC:     rcc_ptr->AHBRSSTR.FLC = 0;         break;
    case RCC_PERIPH_GPIOA:   rcc_ptr->AHBRSSTR.GPIOARST = 0;    break;
    case RCC_PERIPH_GPIOB:   rcc_ptr->AHBRSSTR.GPIOBRST = 0;    break;
    case RCC_PERIPH_GPIOC:   rcc_ptr->AHBRSSTR.GPIOCRST = 0;    break;
    case RCC_PERIPH_ADC0:    rcc_ptr->ANARST.ADC_RST = 0;       break;
    case RCC_PERIPH_MATH:    rcc_ptr->DIGRST.MATHRST = 0;       break;
    case RCC_PERIPH_HALL:    rcc_ptr->DIGRST.HALLRST = 0;       break;
    case RCC_PERIPH_QEP:     rcc_ptr->DIGRST.QEPRST = 0;        break;
    case RCC_PERIPH_IWDT:    rcc_ptr->DIGRST.IWDTRST = 0;       break;
    case RCC_PERIPH_UART0:   rcc_ptr->COMRST.UART0RST = 0;      break;
    case RCC_PERIPH_UART1:   rcc_ptr->COMRST.UART1RST = 0;      break;
    case RCC_PERIPH_SPI0:    rcc_ptr->COMRST.SPI0RST = 0;       break;
    case RCC_PERIPH_SPI1:    rcc_ptr->COMRST.SPI1RST = 0;       break;
    case RCC_PERIPH_IIC0:    rcc_ptr->COMRST.IIC0RST = 0;       break;
    case RCC_PERIPH_IIC1:    rcc_ptr->COMRST.IIC1RST = 0;       break;
    case RCC_PERIPH_TIM13:   rcc_ptr->TIMRST.TIM13RST = 0;      break;
    case RCC_PERIPH_TIM14:   rcc_ptr->TIMRST.TIM14RST = 0;      break;
    case RCC_PERIPH_TIM15:   rcc_ptr->TIMRST.TIM15RST = 0;      break;
    case RCC_PERIPH_TIM16:   rcc_ptr->TIMRST.TIM16RST = 0;      break;
    case RCC_PERIPH_MCPWM0:  rcc_ptr->TIMRST.MCPWM0RST = 0;     break;
    default: break;
    }
}

/**********************************************************
 * 查询接口：外设时钟使能状态 / 外设时钟源选择 (PCKS0)
***********************************************************/

/**
 *@brief Get the peripheral clock enable status
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@param[in] periph:  rcc_periph_e, peripheral to query
 *@return    1 = 时钟已使能，0 = 关闭 / 无效外设
 */
uint8_t rcc_drv_get_periph_clk_en(RCC_TypeDef *rcc_ptr, rcc_periph_e periph)
{
    switch (periph)
    {
    case RCC_PERIPH_FLC:     return rcc_ptr->AHB_CLK_EN.FLC_CLK_EN;
    case RCC_PERIPH_GPIOA:   return rcc_ptr->AHB_CLK_EN.GPIOA_CLK_EN;
    case RCC_PERIPH_GPIOB:   return rcc_ptr->AHB_CLK_EN.GPIOB_CLK_EN;
    case RCC_PERIPH_GPIOC:   return rcc_ptr->AHB_CLK_EN.GPIOC_CLK_EN;
    case RCC_PERIPH_ADC0:    return rcc_ptr->ANA_CLK_EN.ADC0_CLK_EN;
    case RCC_PERIPH_MATH:    return rcc_ptr->DIG_CLK_EN.MATH_CLK_EN;
    case RCC_PERIPH_HALL:    return rcc_ptr->DIG_CLK_EN.HALL_CLK_EN;
    case RCC_PERIPH_QEP:     return rcc_ptr->DIG_CLK_EN.QEP_CLK_EN;
    case RCC_PERIPH_IWDT:    return rcc_ptr->DIG_CLK_EN.IWDT_CLK_EN;
    case RCC_PERIPH_UART0:   return rcc_ptr->COM_CLK_EN.UART0_CLK_EN;
    case RCC_PERIPH_UART1:   return rcc_ptr->COM_CLK_EN.UART1_CLK_EN;
    case RCC_PERIPH_SPI0:    return rcc_ptr->COM_CLK_EN.SPI0_CLK_EN;
    case RCC_PERIPH_SPI1:    return rcc_ptr->COM_CLK_EN.SPI1_CLK_EN;
    case RCC_PERIPH_IIC0:    return rcc_ptr->COM_CLK_EN.IIC0_CLK_EN;
    case RCC_PERIPH_IIC1:    return rcc_ptr->COM_CLK_EN.IIC1_CLK_EN;
    case RCC_PERIPH_TIM13:   return rcc_ptr->TIM_CLK_EN.TIM13_CLK_EN;
    case RCC_PERIPH_TIM14:   return rcc_ptr->TIM_CLK_EN.TIM14_CLK_EN;
    case RCC_PERIPH_TIM15:   return rcc_ptr->TIM_CLK_EN.TIM15_CLK_EN;
    case RCC_PERIPH_TIM16:   return rcc_ptr->TIM_CLK_EN.TIM16_CLK_EN;
    case RCC_PERIPH_MCPWM0:  return rcc_ptr->TIM_CLK_EN.MCPWM0_CLK_EN;
    default:                 return 0U;
    }
}

/**
 *@brief Select the functional clock source of a peripheral (PCKS0)
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@param[in] periph:  rcc_clksrc_periph_e, peripheral to configure (ADC0/MCPWM0/CAN0)
 *@param[in] src:     periph_clk_src_e, clock source (HSI/EOSC/PLL)
 *@return    Clock configure result status:
 *                 - ERC_CLK_MODULE_INVALID: periph invalid
 *                 - ERC_CLK_HSC_SEL_INVALID: src invalid
 *                 - ERC_CLK_NO_ERROR: success
 */
clk_erc_e rcc_drv_periph_clk_src_select(RCC_TypeDef *rcc_ptr, rcc_clksrc_periph_e periph, periph_clk_src_e src)
{
    if (periph >= RCC_CLKSRC_MAX)
    {
        return ERC_CLK_MODULE_INVALID;
    }
    if ((src < PERIPH_CLK_SRC_HSI) || (src > PERIPH_CLK_SRC_PLL))
    {
        return ERC_CLK_HSC_SEL_INVALID;
    }

    switch (periph)
    {
    case RCC_CLKSRC_ADC0:   rcc_ptr->PCKS0.ADC0CKS = src;   break;
    case RCC_CLKSRC_MCPWM0: rcc_ptr->PCKS0.MCPWM0CKS = src; break;
    case RCC_CLKSRC_CAN0:   rcc_ptr->PCKS0.CAN0CKS = src;   break;
    default:                                               break;
    }

    return ERC_CLK_NO_ERROR;
}

/**
 *@brief Get the functional clock source of a peripheral (PCKS0)
 *@param[in]  rcc_ptr: Pointer to RCC peripheral registers
 *@param[in]  periph:  rcc_clksrc_periph_e, peripheral to query (ADC0/MCPWM0/CAN0)
 *@param[out] *src:    periph_clk_src_e, current clock source
 *@return    Clock configure result status:
 *                 - ERC_CLK_MODULE_INVALID: periph invalid
 *                 - ERC_CLK_NO_ERROR: success
 */
clk_erc_e rcc_drv_get_periph_clk_src(RCC_TypeDef *rcc_ptr, rcc_clksrc_periph_e periph, periph_clk_src_e *src)
{
    if (periph >= RCC_CLKSRC_MAX)
    {
        return ERC_CLK_MODULE_INVALID;
    }

    switch (periph)
    {
    case RCC_CLKSRC_ADC0:   *src = (periph_clk_src_e)rcc_ptr->PCKS0.ADC0CKS;   break;
    case RCC_CLKSRC_MCPWM0: *src = (periph_clk_src_e)rcc_ptr->PCKS0.MCPWM0CKS; break;
    case RCC_CLKSRC_CAN0:   *src = (periph_clk_src_e)rcc_ptr->PCKS0.CAN0CKS;   break;
    default:                                                                 break;
    }

    return ERC_CLK_NO_ERROR;
}

/**********************************************************
 * 兼容薄封装：外设时钟使能（原有接口，内部调用统一函数）
***********************************************************/
void rcc_drv_enable_flc(RCC_TypeDef *rcc_ptr)      { rcc_drv_periph_clk_enable(rcc_ptr, RCC_PERIPH_FLC); }
void rcc_drv_enable_gpio_a(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_clk_enable(rcc_ptr, RCC_PERIPH_GPIOA); }
void rcc_drv_enable_gpio_b(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_clk_enable(rcc_ptr, RCC_PERIPH_GPIOB); }
void rcc_drv_enable_gpio_c(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_clk_enable(rcc_ptr, RCC_PERIPH_GPIOC); }
void rcc_drv_enable_adc0(RCC_TypeDef *rcc_ptr)     { rcc_drv_periph_clk_enable(rcc_ptr, RCC_PERIPH_ADC0); }
void rcc_drv_enable_math(RCC_TypeDef *rcc_ptr)     { rcc_drv_periph_clk_enable(rcc_ptr, RCC_PERIPH_MATH); }
void rcc_drv_enable_hall(RCC_TypeDef *rcc_ptr)     { rcc_drv_periph_clk_enable(rcc_ptr, RCC_PERIPH_HALL); }
void rcc_drv_enable_qep(RCC_TypeDef *rcc_ptr)      { rcc_drv_periph_clk_enable(rcc_ptr, RCC_PERIPH_QEP); }
void rcc_drv_enable_iwdt(RCC_TypeDef *rcc_ptr)     { rcc_drv_periph_clk_enable(rcc_ptr, RCC_PERIPH_IWDT); }
void rcc_drv_enable_uart_0(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_clk_enable(rcc_ptr, RCC_PERIPH_UART0); }
void rcc_drv_enable_uart_1(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_clk_enable(rcc_ptr, RCC_PERIPH_UART1); }
void rcc_drv_enable_spi_0(RCC_TypeDef *rcc_ptr)    { rcc_drv_periph_clk_enable(rcc_ptr, RCC_PERIPH_SPI0); }
void rcc_drv_enable_spi_1(RCC_TypeDef *rcc_ptr)    { rcc_drv_periph_clk_enable(rcc_ptr, RCC_PERIPH_SPI1); }
void rcc_drv_enable_iic_0(RCC_TypeDef *rcc_ptr)    { rcc_drv_periph_clk_enable(rcc_ptr, RCC_PERIPH_IIC0); }
void rcc_drv_enable_iic_1(RCC_TypeDef *rcc_ptr)    { rcc_drv_periph_clk_enable(rcc_ptr, RCC_PERIPH_IIC1); }
void rcc_drv_enable_tim_13(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_clk_enable(rcc_ptr, RCC_PERIPH_TIM13); }
void rcc_drv_enable_tim_14(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_clk_enable(rcc_ptr, RCC_PERIPH_TIM14); }
void rcc_drv_enable_tim_15(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_clk_enable(rcc_ptr, RCC_PERIPH_TIM15); }
void rcc_drv_enable_tim_16(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_clk_enable(rcc_ptr, RCC_PERIPH_TIM16); }
void rcc_drv_enable_mcpwm0(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_clk_enable(rcc_ptr, RCC_PERIPH_MCPWM0); }

/**********************************************************
 * 兼容薄封装：外设时钟关闭（内部调用统一函数）
***********************************************************/
void rcc_drv_disable_flc(RCC_TypeDef *rcc_ptr)      { rcc_drv_periph_clk_disable(rcc_ptr, RCC_PERIPH_FLC); }
void rcc_drv_disable_gpio_a(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_clk_disable(rcc_ptr, RCC_PERIPH_GPIOA); }
void rcc_drv_disable_gpio_b(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_clk_disable(rcc_ptr, RCC_PERIPH_GPIOB); }
void rcc_drv_disable_gpio_c(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_clk_disable(rcc_ptr, RCC_PERIPH_GPIOC); }
void rcc_drv_disable_adc0(RCC_TypeDef *rcc_ptr)     { rcc_drv_periph_clk_disable(rcc_ptr, RCC_PERIPH_ADC0); }
void rcc_drv_disable_math(RCC_TypeDef *rcc_ptr)     { rcc_drv_periph_clk_disable(rcc_ptr, RCC_PERIPH_MATH); }
void rcc_drv_disable_hall(RCC_TypeDef *rcc_ptr)     { rcc_drv_periph_clk_disable(rcc_ptr, RCC_PERIPH_HALL); }
void rcc_drv_disable_qep(RCC_TypeDef *rcc_ptr)      { rcc_drv_periph_clk_disable(rcc_ptr, RCC_PERIPH_QEP); }
void rcc_drv_disable_iwdt(RCC_TypeDef *rcc_ptr)     { rcc_drv_periph_clk_disable(rcc_ptr, RCC_PERIPH_IWDT); }
void rcc_drv_disable_uart_0(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_clk_disable(rcc_ptr, RCC_PERIPH_UART0); }
void rcc_drv_disable_uart_1(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_clk_disable(rcc_ptr, RCC_PERIPH_UART1); }
void rcc_drv_disable_spi_0(RCC_TypeDef *rcc_ptr)    { rcc_drv_periph_clk_disable(rcc_ptr, RCC_PERIPH_SPI0); }
void rcc_drv_disable_spi_1(RCC_TypeDef *rcc_ptr)    { rcc_drv_periph_clk_disable(rcc_ptr, RCC_PERIPH_SPI1); }
void rcc_drv_disable_iic_0(RCC_TypeDef *rcc_ptr)    { rcc_drv_periph_clk_disable(rcc_ptr, RCC_PERIPH_IIC0); }
void rcc_drv_disable_iic_1(RCC_TypeDef *rcc_ptr)    { rcc_drv_periph_clk_disable(rcc_ptr, RCC_PERIPH_IIC1); }
void rcc_drv_disable_tim_13(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_clk_disable(rcc_ptr, RCC_PERIPH_TIM13); }
void rcc_drv_disable_tim_14(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_clk_disable(rcc_ptr, RCC_PERIPH_TIM14); }
void rcc_drv_disable_tim_15(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_clk_disable(rcc_ptr, RCC_PERIPH_TIM15); }
void rcc_drv_disable_tim_16(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_clk_disable(rcc_ptr, RCC_PERIPH_TIM16); }
void rcc_drv_disable_mcpwm0(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_clk_disable(rcc_ptr, RCC_PERIPH_MCPWM0); }

/**********************************************************
 * 兼容薄封装：外设复位（原有接口，内部调用统一函数）
***********************************************************/
void rcc_drv_rst_flc(RCC_TypeDef *rcc_ptr)         { rcc_drv_periph_reset(rcc_ptr, RCC_PERIPH_FLC); }
void rcc_drv_rst_gpio_a(RCC_TypeDef *rcc_ptr)      { rcc_drv_periph_reset(rcc_ptr, RCC_PERIPH_GPIOA); }
void rcc_drv_rst_gpio_b(RCC_TypeDef *rcc_ptr)      { rcc_drv_periph_reset(rcc_ptr, RCC_PERIPH_GPIOB); }
void rcc_drv_rst_gpio_c(RCC_TypeDef *rcc_ptr)      { rcc_drv_periph_reset(rcc_ptr, RCC_PERIPH_GPIOC); }
void rcc_drv_rst_adc0(RCC_TypeDef *rcc_ptr)        { rcc_drv_periph_reset(rcc_ptr, RCC_PERIPH_ADC0); }
void rcc_drv_rst_math(RCC_TypeDef *rcc_ptr)        { rcc_drv_periph_reset(rcc_ptr, RCC_PERIPH_MATH); }
void rcc_drv_rst_hall(RCC_TypeDef *rcc_ptr)        { rcc_drv_periph_reset(rcc_ptr, RCC_PERIPH_HALL); }
void rcc_drv_rst_qep(RCC_TypeDef *rcc_ptr)         { rcc_drv_periph_reset(rcc_ptr, RCC_PERIPH_QEP); }
void rcc_drv_rst_iwdt(RCC_TypeDef *rcc_ptr)        { rcc_drv_periph_reset(rcc_ptr, RCC_PERIPH_IWDT); }
void rcc_drv_rst_uart_0(RCC_TypeDef *rcc_ptr)      { rcc_drv_periph_reset(rcc_ptr, RCC_PERIPH_UART0); }
void rcc_drv_rst_uart_1(RCC_TypeDef *rcc_ptr)      { rcc_drv_periph_reset(rcc_ptr, RCC_PERIPH_UART1); }
void rcc_drv_rst_spi_0(RCC_TypeDef *rcc_ptr)       { rcc_drv_periph_reset(rcc_ptr, RCC_PERIPH_SPI0); }
void rcc_drv_rst_spi_1(RCC_TypeDef *rcc_ptr)       { rcc_drv_periph_reset(rcc_ptr, RCC_PERIPH_SPI1); }
void rcc_drv_rst_iic_0(RCC_TypeDef *rcc_ptr)       { rcc_drv_periph_reset(rcc_ptr, RCC_PERIPH_IIC0); }
void rcc_drv_rst_iic_1(RCC_TypeDef *rcc_ptr)       { rcc_drv_periph_reset(rcc_ptr, RCC_PERIPH_IIC1); }
void rcc_drv_rst_tim_13(RCC_TypeDef *rcc_ptr)      { rcc_drv_periph_reset(rcc_ptr, RCC_PERIPH_TIM13); }
void rcc_drv_rst_tim_14(RCC_TypeDef *rcc_ptr)      { rcc_drv_periph_reset(rcc_ptr, RCC_PERIPH_TIM14); }
void rcc_drv_rst_tim_15(RCC_TypeDef *rcc_ptr)      { rcc_drv_periph_reset(rcc_ptr, RCC_PERIPH_TIM15); }
void rcc_drv_rst_tim_16(RCC_TypeDef *rcc_ptr)      { rcc_drv_periph_reset(rcc_ptr, RCC_PERIPH_TIM16); }
void rcc_drv_rst_mcpwm0(RCC_TypeDef *rcc_ptr)      { rcc_drv_periph_reset(rcc_ptr, RCC_PERIPH_MCPWM0); }

/**********************************************************
 * 兼容薄封装：外设复位释放（内部调用统一函数）
 *   注：若 datasheet 确认为"写1自动清零"的脉冲复位，此组函数无需调用
***********************************************************/
void rcc_drv_rst_release_flc(RCC_TypeDef *rcc_ptr)      { rcc_drv_periph_reset_release(rcc_ptr, RCC_PERIPH_FLC); }
void rcc_drv_rst_release_gpio_a(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_reset_release(rcc_ptr, RCC_PERIPH_GPIOA); }
void rcc_drv_rst_release_gpio_b(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_reset_release(rcc_ptr, RCC_PERIPH_GPIOB); }
void rcc_drv_rst_release_gpio_c(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_reset_release(rcc_ptr, RCC_PERIPH_GPIOC); }
void rcc_drv_rst_release_adc0(RCC_TypeDef *rcc_ptr)     { rcc_drv_periph_reset_release(rcc_ptr, RCC_PERIPH_ADC0); }
void rcc_drv_rst_release_math(RCC_TypeDef *rcc_ptr)     { rcc_drv_periph_reset_release(rcc_ptr, RCC_PERIPH_MATH); }
void rcc_drv_rst_release_hall(RCC_TypeDef *rcc_ptr)     { rcc_drv_periph_reset_release(rcc_ptr, RCC_PERIPH_HALL); }
void rcc_drv_rst_release_qep(RCC_TypeDef *rcc_ptr)      { rcc_drv_periph_reset_release(rcc_ptr, RCC_PERIPH_QEP); }
void rcc_drv_rst_release_iwdt(RCC_TypeDef *rcc_ptr)     { rcc_drv_periph_reset_release(rcc_ptr, RCC_PERIPH_IWDT); }
void rcc_drv_rst_release_uart_0(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_reset_release(rcc_ptr, RCC_PERIPH_UART0); }
void rcc_drv_rst_release_uart_1(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_reset_release(rcc_ptr, RCC_PERIPH_UART1); }
void rcc_drv_rst_release_spi_0(RCC_TypeDef *rcc_ptr)    { rcc_drv_periph_reset_release(rcc_ptr, RCC_PERIPH_SPI0); }
void rcc_drv_rst_release_spi_1(RCC_TypeDef *rcc_ptr)    { rcc_drv_periph_reset_release(rcc_ptr, RCC_PERIPH_SPI1); }
void rcc_drv_rst_release_iic_0(RCC_TypeDef *rcc_ptr)    { rcc_drv_periph_reset_release(rcc_ptr, RCC_PERIPH_IIC0); }
void rcc_drv_rst_release_iic_1(RCC_TypeDef *rcc_ptr)    { rcc_drv_periph_reset_release(rcc_ptr, RCC_PERIPH_IIC1); }
void rcc_drv_rst_release_tim_13(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_reset_release(rcc_ptr, RCC_PERIPH_TIM13); }
void rcc_drv_rst_release_tim_14(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_reset_release(rcc_ptr, RCC_PERIPH_TIM14); }
void rcc_drv_rst_release_tim_15(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_reset_release(rcc_ptr, RCC_PERIPH_TIM15); }
void rcc_drv_rst_release_tim_16(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_reset_release(rcc_ptr, RCC_PERIPH_TIM16); }
void rcc_drv_rst_release_mcpwm0(RCC_TypeDef *rcc_ptr)   { rcc_drv_periph_reset_release(rcc_ptr, RCC_PERIPH_MCPWM0); }

#endif


/**********************************************************
 * rcc system reset management
 * 系统复位管理：复位源查询/清除、复位事件使能、软件复位、boot 复位重校准
 *   注：SYSRSTE 带写保护，修改使能位时必须同步写入 RCC_SYSRSTE_KEYCODE 解锁
***********************************************************/
#if 1

/**
 *@brief Get the system reset status flags
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    SYSRSTSR 值，用 CLOCK_RST_FLAG_* 位掩码判断上次复位来源
 */
uint32_t rcc_drv_get_reset_status(RCC_TypeDef *rcc_ptr)
{
    return rcc_ptr->SYSRSTSR.w;
}

/**
 *@brief Clear the system reset status flags (rw1c, write 1 to clear)
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@param[in] flags:   CLOCK_RST_FLAG_* 位掩码，可多位置 1
 *@return    none
 */
void rcc_drv_clear_reset_flag(RCC_TypeDef *rcc_ptr, uint32_t flags)
{
    rcc_ptr->SYSRSTSR.w = (flags & CLOCK_RST_FLAG_ALL);
}

/** 复位使能源到 SYSRSTE 位的映射 */
static uint32_t rcc_drv_rst_src_bit(clock_rst_src_e src)
{
    switch (src)
    {
    case CLOCK_RST_SRC_LVD:        return 2U;   /* SYSRSTE bit[2]  */
    case CLOCK_RST_SRC_LOCKUP:     return 10U;  /* SYSRSTE bit[10] */
    case CLOCK_RST_SRC_IWDT:       return 16U;  /* SYSRSTE bit[16] */
    case CLOCK_RST_SRC_HSE_FAIL:   return 21U;  /* SYSRSTE bit[21] */
    case CLOCK_RST_SRC_PLL_UNLOCK: return 22U;  /* SYSRSTE bit[22] */
    default:                       return 0U;
    }
}

/**
 *@brief Enable/Disable a reset source event (SYSRSTE)
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@param[in] src:     clock_rst_src_e, reset source to configure
 *@param[in] en:      1 = 使能该事件触发系统复位，0 = 关闭
 *@return    Clock configure result status:
 *                 - ERC_CLK_SYS_SRC_INVALID: src invalid
 *                 - ERC_CLK_NO_ERROR: success
 *@note   SYSRSTE 带写保护，本函数会自动同步写入 RCC_SYSRSTE_KEYCODE 解锁
 */
clk_erc_e rcc_drv_reset_src_enable(RCC_TypeDef *rcc_ptr, clock_rst_src_e src, uint8_t en)
{
    uint32_t bit, val;

    if (src >= CLOCK_RST_SRC_MAX)
    {
        return ERC_CLK_SYS_SRC_INVALID;
    }

    bit = rcc_drv_rst_src_bit(src);
    val = rcc_ptr->SYSRSTE.w & 0x00FFFFFFU;     /* 保留其它使能位 */
    if (en)
    {
        val |= (1U << bit);
    }
    else
    {
        val &= ~(1U << bit);
    }

    /* 功能位与写保护密钥须同步写入 */
    rcc_ptr->SYSRSTE.w = ((uint32_t)RCC_SYSRSTE_KEYCODE << 24U) | val;

    return ERC_CLK_NO_ERROR;
}

/**
 *@brief Trigger a software system reset
 *@param[in] none
 *@return    none
 */
void rcc_drv_sys_reset(void)
{
    NVIC_SystemReset();
}

/**
 *@brief Trigger a boot reset and re-run calibration (write RSTKEY 0xAB56)
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    none
 */
void rcc_drv_boot_reset_retrim(RCC_TypeDef *rcc_ptr)
{
    rcc_ptr->RSTKEY.RETRIM = 0xAB56U;
}

#endif


/**********************************************************
 * rcc chip version info (VERID)
***********************************************************/
#if 1

/**
 *@brief Get the RCC IP version ID register value
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    VERID 原始值（bit[15:8] 主版本号, bit[7:0] 小版本号, bit[31:16] 特性）
 */
uint32_t rcc_drv_get_verid(RCC_TypeDef *rcc_ptr)
{
    return rcc_ptr->VERID.w;
}

/**
 *@brief Get the RCC IP minor version number
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    MINVN: 小版本号
 */
uint8_t rcc_drv_get_verid_minor(RCC_TypeDef *rcc_ptr)
{
    return rcc_ptr->VERID.MINVN;
}

/**
 *@brief Get the RCC IP major version number
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    MAJVN: 主版本号
 */
uint8_t rcc_drv_get_verid_major(RCC_TypeDef *rcc_ptr)
{
    return rcc_ptr->VERID.MAJVN;
}

/**
 *@brief Get the RCC IP feature flags
 *@param[in] rcc_ptr: Pointer to RCC peripheral registers
 *@return    FEATURE: 特性标识位
 */
uint16_t rcc_drv_get_verid_feature(RCC_TypeDef *rcc_ptr)
{
    return rcc_ptr->VERID.FEATURE;
}

#endif
