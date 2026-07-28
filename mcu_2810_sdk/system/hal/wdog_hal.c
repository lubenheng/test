#include "wdog_hal.h"
#include "wdog_type.h"   
#include "wdog_reg.h"
#include "cmdline_internal.h"
 
 

/*===========================   pjh add ========================*/
/**
 * @brief  喂狗（通用接口，同feedWDT）
 * @param  无
 * @return 无
 */
void Wdog_Feed(void)
{
    // 等待强制重载忙信号
    while ((WDT->rst) & 0x01U)
    {
        __ISB();
    }

    // 执行重载
    WDT->rst = 1U;
}

/**
 * @brief  读取看门狗当前计数值
 * @param  无
 * @return 当前计数值
 */
uint32_t Wdog_Read_Cnt(void)
{
    // 等待读计数忙信号释放
    while (WDT->rst & 0x02U)
    {
        __ISB();
    }

    // 触发读当前值操作
    WDT->rst |= (1U << 1);
    // 返回计数值
    return WDT->cur_val;
}

/**
 * @brief  读取看门狗操作失败状态
 * @param  无
 * @return 状态值：bit0-force_reload失败，bit1-rd_cur_val失败
 */
uint8_t Wdog_Read_Fail(void)
{
    uint32_t temp = WDT->rst;
    // 提取bit30~31状态位
    temp = (temp >> 30U) & 0x03U;
    return (uint8_t)temp;
}

/**
 * @brief  读取指定看门狗寄存器值
 * @param  reg: 寄存器枚举类型
 * @return 对应寄存器的值
 */
uint32_t Wdog_Read_Reg(wdog_reg reg)
{
    switch (reg)
    {
        case Reload_val_e:
            return WDT->reload_val;
        case en_e:
            return WDT->en;
        case rst_e:
            return WDT->rst;
        case reload_bak_e:
            return WDT->reload_bak;
        default:
            return 0;
    }
}

/**
 * @brief  设置看门狗重装载值，并可选立即生效
 * @param  Value: 重装载值 单位s
 * @param  En: 1-立即生效，0-不立即生效
 * @return 默认  LRC=32K  80000默认
第二次定时结束时 才会复位
 */
void Wdog_Reload_Set(uint32_t Value, uint8_t En)
{
    // 设置重载值
    WDT->reload_val = Value << 15;

    // En=1 立即喂狗生效
    if (En)
    {
        Wdog_Feed();
    }
}

/**
 * @brief  看门狗总配置：使能WDT + 使能中断 + 使能复位
 * @param  无
 * @return 无
 */
void Wdog_Interrupt_Set(void)
{
    WDT->en |= (1U << 0);  // WDT使能
    WDT->en |= (1U << 1);  // 中断使能
    WDT->en |= (1U << 2);  // 复位使能
}

/**
 * @brief  清除看门狗状态标志
 * @param  无
 * @return 无
 */
void Wdog_Status_Clean(void)
{
    WDT->rst |= (1U << 2);
}

/**
 * @brief  获取看门狗状态标志
 * @param  无
 * @return 状态值
 */
uint8_t Wdog_Status_Get(void)
{
    uint8_t temp = (uint8_t)(WDT->rst & (1U << 2));
    temp >>= 2;
    return temp;
}

//看门狗新增中断机制（不可屏蔽中断NMI）
//，一旦定时结束，先起中断，然后自动喂一次狗（不是软件行为）。如果第二次定时结束时，发现中断没清掉，才会复位。
void NMI_Handler (void)
{
	
//	INFO_e("interrupt wdt %d   %u\r\n",Wdog_Status_Get(),Wdog_Read_Cnt());
//	Wdog_Status_Clean();
}


