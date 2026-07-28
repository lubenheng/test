#ifndef _WDOG_HAL_H
#define _WDOG_HAL_H
#include "CMSDK_CM0.h"

/**
 * @brief  看门狗寄存器枚举
 */
typedef enum {
    Reload_val_e,     // 重装载值寄存器
    en_e,             // 使能寄存器
    rst_e,            // 复位/控制寄存器
    reload_bak_e      // 重装载备份寄存器
} wdog_reg;

void Wdog_Feed(void);
uint32_t Wdog_Read_Cnt(void);
uint8_t Wdog_Read_Fail(void);
uint32_t Wdog_Read_Reg(wdog_reg reg);

void Wdog_Reload_Set(uint32_t Value, uint8_t En);
void Wdog_Interrupt_Set(void);
void Wdog_Status_Clean(void);
uint8_t Wdog_Status_Get(void);
 
#endif  // _WDOG_HAL_H
