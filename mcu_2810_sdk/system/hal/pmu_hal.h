#ifndef _PMU_HAL_H
#define _PMU_HAL_H
#include "CMSDK_CM0.h"
typedef enum
{
	RW_ANA,    /* 模拟电源使能位 bit8 */
	RW_HFCK    /* 高速时钟使能位 bit9 */
} rw_e;

typedef enum
{
	CLK_PLL,    
	CLK_RCO    /* PLL 2分频*/
} clk_e;

 

void PMU_Unlock(void);
void PMU_Lock(void);
// 初始化 PMU (默认 42M 或 48M，根据实际主频)
void PMU_Init(void);
void PMU_Set_CPU_48M(void);
void PMU_Set_CPU_24M(void);
void PMU_Set_CPU_12M_RCO(void);
void PMU_Config_Sleep(void);
// 进入深度休眠
void PMU_Enter_DeepSleep(void);
void PMU_Get_Status(uint32_t *src, uint32_t *div);
void PMU_Dump_Reg(void);

 
 
#endif  // _PMU_HAL_H
