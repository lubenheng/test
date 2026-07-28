#include <stdio.h>
#include <string.h>
#include "CMSDK_CM0.h"
#include "core_cm0.h"
#include "Common.h"
#include "uart.h"
#include "gpio.h"
#include "qspi.h"
#include "wdt.h"
#include "timer.h"
#include "mdu.h"
#include <stdarg.h>
#include "INIT.h"

#include "para_define.h"

#include "hardinit.h"

#include "main.h"
#include "gpio_hal.h"
#include "softinit.h"
int a,b,c,d,e;
//=============================================================================
// P13(P1.3) 引脚输出 24MHz 时钟配置
//
// 根据 OCS-QA-Q2-010-07 Final Test specifications V2.1.0 文档
// OSC24M 测试校准章节（文档第2679~2780行）：
//
// 寄存器配置：
//   Pad1_ana_en (0x4000141C[11]) = 1B    → P1.3(P13) 模拟功能使能
//   RW_TEST0_SEL (0x40005008[18:16]) = 110B → 选择 OSC 24M 时钟源输出到 P13
//   RW_TEST0_EN  (0x40005000[15]) = 1B      → 测试输出使能
//
// 注：24MHz 振荡器经内部 2048 分频后输出到 P13，
//     实际输出频率 = 24MHz / 2048 ≈ 11.71875KHz
//=============================================================================

#define IO_BASE_2810        0x40001400UL
#define ANA_CTRL_BASE       0x40005000UL

// ANA_CTRL_EN 寄存器（0x40005000）各使能位定义
#define RW_OSC_CHANGE_EN_POS    (0)   // bit[0]: OSC 时钟切换使能
#define RW_ADC_EN_POS           (1)   // bit[1]: ADC 使能
#define RW_DAC_EN_POS           (2)   // bit[2]: DAC 使能
#define RW_AMP0_EN_POS          (3)   // bit[3]: 运放0使能
#define RW_AMP1_EN_POS          (4)   // bit[4]: 运放1使能
#define RW_AMP2_EN_POS          (5)   // bit[5]: 运放2使能
#define RW_AMP_CMP_EN_POS       (6)   // bit[6]: 运放比较器使能
#define RW_VHALF_EN_POS         (7)   // bit[7]: VHALF使能
#define RW_VREF_EN_POS          (8)   // bit[8]: VREF使能
#define RW_LDO_SWITCH_EN_POS    (9)   // bit[9]: LDO切换使能
#define RW_HBIAS_EN_POS         (10)  // bit[10]: HBIAS使能
#define RW_PGA0_EN_POS          (11)  // bit[11]: PGA0使能
#define RW_PGA1_EN_POS          (12)  // bit[12]: PGA1使能
#define RW_PGA2_EN_POS          (13)  // bit[13]: PGA2使能
#define RW_IPM_EN_POS           (14)  // bit[14]: IPM使能
#define RW_TEST0_EN_POS         (15)  // bit[15]: 测试输出0使能
#define RW_TEST1_EN_POS         (16)  // bit[16]: 测试输出1使能



/**
 * @brief 配置 P13(P1.3) 引脚输出 24MHz 时钟（经2048分频后约11.71875KHz）
 * 
 * 配置流程：
 * 1. 使能 P1.3 引脚模拟功能（Pad1_ana_en[11] = 1）
 * 2. 选择测试信号源为 OSC 24M（RW_TEST0_SEL[18:16] = 110B）
 * 3. 使能测试输出（RW_TEST0_EN[15] = 1B）
 */
void P13_24MHz_Clock_Output(void)
{
    volatile uint32_t *io_ana   = (volatile uint32_t *)(IO_BASE_2810 + 0x1C);    // IO_ANA 寄存器 @ 0x4000141C
    volatile uint32_t *ana_en   = (volatile uint32_t *)(ANA_CTRL_BASE + 0x00);   // ANA_CTRL_EN @ 0x40005000
    volatile uint32_t *ana_sel  = (volatile uint32_t *)(ANA_CTRL_BASE + 0x08);   // ANA_CTRL_OTHERS @ 0x40005008
	volatile uint32_t *ana_24TRIM   = (volatile uint32_t *)(ANA_CTRL_BASE + 0x0C);   // ANA_24TRIM @ 0x4000500C
    // 步骤1: 使能 P1.3(P13) 模拟功能
    // Pad1_ana_en 在 IO->ANA 寄存器的 [15:8] 位
    // P1.3 对应 (8 + 3) = bit 11
    *io_ana |= (1UL << 11);

    // 步骤2: 设置 RW_TEST0_SEL[18:16] = 110b（选择 OSC 24M 输出）
    *ana_sel &= ~(0x7UL << 16);         // 先清除 bits[18:16]
    *ana_sel |= (0x6UL << 16);          // 设置 110b

    // 步骤3: 使能 RW_TEST0_EN[15] = 1（使能测试输出）
    *ana_en |= (1UL << 15);
	// 步骤4:设置休调值
	*ana_24TRIM = (*ana_24TRIM & ~(0x3F << 8)) | (0x27 << 8);
}
////////////////////////////////////////////////////////////////////////////////////////


/**
 * @brief 主函数
 * 
 * 功能：
 * 1. 初始化 PMU 电源管理单元
 * 2. 配置 P13(P1.3) 引脚输出 24MHz 内部振荡器时钟（经2048分频）
 * 3. 进入无限循环
 */
int main(void)
{
	
	
 delay_ms(1000);
    PMU->wr_key = 0x55aaaa55;
    PMU->cfg_normal = 0<<0 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 0<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10; //DRV3 V1.4
		
    // 配置 P13(P1.3) 引脚输出 24MHz 时钟
    P13_24MHz_Clock_Output();

    while (1)
    {
        // 主循环空转，P13 持续输出时钟信号
    } 
}
