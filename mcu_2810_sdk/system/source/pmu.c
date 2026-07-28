#include <stdio.h>
#include <string.h>
//#include "CMSDK_CM0.h"
//#include "core_cm0.h"
#include "Common.h"
#include "pmu.h"
//#include "cmdline_internal.h"
/*
*********************************************************************
* PMU 驱动
* 时钟支持：
*    48MHz (PLL 1分频)
*    42MHz (PLL 1分频)
*    24MHz (PLL 2分频)
*    21MHz (PLL 2分频)
*********************************************************************
*/

//// 解锁 PMU 寄存器  写后必须跟着操作PMU的寄存器
//void PMU_Unlock(void)
//{
//    PMU->wr_key = 0x55AAAA55;
//}

//// 锁定 PMU 寄存器
//void PMU_Lock(void)
//{
//    PMU->wr_key = 0x00000000;
//}


//// 设置 CPU 48MHz  (PLL 1分频) 
//void PMU_Set_CPU_48M(void)
//{
//    uint32_t temp = PMU->cfg_normal;
//    temp &= ~(1U << 0);    // PLL
////    temp &= ~(1U << 2);    // 1分频
//		temp |=  (1U << 2);
//    temp |=  (1U << 8);    // 模拟电源
//    temp |=  (1U << 9);    // 高速时钟
//		PMU_Unlock();
//    PMU->cfg_normal = temp;

//     
//}

//// 设置 CPU 24MHz (PLL 2分频) 
//void PMU_Set_CPU_24M(void)
//{  
//    uint32_t temp = PMU->cfg_normal;
//    temp &= ~(1U << 0);    // PLL
//    temp |=  (1U << 2);    // 2分频
//    temp |=  (1U << 8);
//    temp |=  (1U << 9);
//		PMU_Unlock();
//    PMU->cfg_normal = temp;
// 
//}

//// 设置 RCO 12MHz (2分频) 
//void PMU_Set_CPU_12M_RCO(void)
//{
//    uint32_t temp = PMU->cfg_normal;
//    temp |=  (1U << 0);    // RCO
//    temp |=  (1U << 2);    // 2分频
//    temp |=  (1U << 8);
//    temp |=  (1U << 9);
//		PMU_Unlock();
//    PMU->cfg_normal = temp;

//   
//}

//// 配置休眠模式 
//void PMU_Config_Sleep(void)
//{   
//		
//		uint32_t temp = PMU->cfg_sleep;
//		temp &= ~(1U << 0);    // PLL
//    temp &= ~(0U << 2);   
//		temp |=  (1U << 3);
//		temp &= ~(1U << 4); //MTP 休眠
//		temp |=  (1U << 5); //RAM 休眠使能
//    temp |=  (1U << 8);
//    temp &= ~(1U << 9);
//		PMU_Unlock();
//		PMU->cfg_sleep = temp;
//		PMU_Unlock();
//    PMU->cfg_dly   = (5 << 4) | 1;
//		
//}

//// 进入深度休眠
//void PMU_Enter_DeepSleep(void)
//{
//    PMU_Config_Sleep();
//    SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
//		INFO_e("sleep \r\n");
//    __WFI();
//	  INFO_e("sleep2 \r\n");
//}


//// 读取当前时钟配置
//void PMU_Get_Status(uint32_t *src, uint32_t *div)
//{
//    uint32_t cfg = PMU->cfg_normal;
//    *src = cfg & 1;
//    *div = (cfg >> 2) & 1;
//}

//// 打印所有寄存器
//void PMU_Dump_Reg(void)
//{
//    INFO_e("PMU cfg_normal: 0x%08X\n", PMU->cfg_normal);
//    INFO_e("PMU cfg_sleep : 0x%08X\n", PMU->cfg_sleep);
//    INFO_e("PMU cfg_dly   : 0x%08X\n", PMU->cfg_dly);
//}
