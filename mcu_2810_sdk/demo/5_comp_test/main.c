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
#include "motor_control.h"
#include "para_define.h"

#include "hardinit.h"

#include "main.h"
#include "gpio_hal.h"
#include "adc_hal.h"
 
#include "cmdline_internal.h"
 
#include "pmu.h"
#include "softinit.h" 
int a=0,b=0,c=0,d=0,e=0;

//=============================================================================
// P13(P1.3) 引脚输出 33KHz 时钟配置
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
#define COMP_BASE           0x40004400UL
#define COMP_CLK            0x40002400UL
/**
 * @brief 配置 P13(P1.3) 引脚输出 33KHz 时钟
 * 
 * 配置流程：
 * 1. 使能 P1.3 引脚模拟功能（Pad1_ana_en[11] = 1）
 * 2. 选择测试信号源为 OSC 24M（RW_TEST0_SEL[18:16] = 110B）
 * 3. 使能测试输出（RW_TEST0_EN[15] = 1B）
 */
void CMP_TEST(void)
{
	volatile uint32_t *io_ana   = (volatile uint32_t *)(IO_BASE_2810 + 0x1C);
	volatile uint32_t *CMP_EN   = (volatile uint32_t *)(COMP_BASE + 0x08);
	volatile uint32_t *RW_TEST0_EN   = (volatile uint32_t *)(ANA_CTRL_BASE);
	volatile uint32_t *cmp_CLK_EN   = (volatile uint32_t *)(COMP_CLK + 0x08);
	volatile uint32_t *cmp_MODE   = (volatile uint32_t *)(COMP_BASE + 0x08);
	volatile uint32_t *cmp_hys  = (volatile uint32_t *)(COMP_BASE + 0x04);
	volatile uint32_t *Pad_c0ns_en   = (volatile uint32_t *)(ANA_CTRL_BASE + 0x08);	
	// 步骤1: 使能 P1.3(P13) 模拟功能
    // Pad1_ana_en 在 IO->ANA 寄存器的 [15:8] 位
    // P1.3 对应 (8 + 3) = bit 11
    *io_ana |= (0xFFFFFFUL << 8);//P1模拟使能P2模拟使能P3模拟使能
	*CMP_EN |= (0x01UL << 0);//COMP 0 1 2使能
	*CMP_EN |= (0x01UL << 7);//COMP 4使能
	*RW_TEST0_EN |= (0x01UL << 15);//RW_TEST0_EN
	*RW_TEST0_EN |= (0x01UL << 6);//RW_AMP_CMP_EN
	*cmp_CLK_EN |= (0x01UL << 31);///cmp_CLK_EN
	
	*cmp_MODE &= ~(0x03UL << 5); // 先清除 //三差分比较器模式
    *cmp_MODE |= (0x02UL << 5); 
	
	*cmp_hys &= ~(0x07UL << 0); // 先清除 CMP0迟滞电压选择: 
    *cmp_hys |= (0x03UL << 0); 
	
	*cmp_hys &= ~(0x07UL << 7); // 先清除 CMP1迟滞电压选择: 
    *cmp_hys |= (0x03UL << 7); 
	
	*cmp_hys &= ~(0x01UL << 3); // 先清除 CMP3迟滞电压选择: 
    *cmp_hys |= (0x01UL << 3); 
	
	*Pad_c0ns_en &= ~(0x07UL << 16); // 先清除 //COMP1输出结果到P13，
    *Pad_c0ns_en |= (0x02UL << 16);

    
}
////////////////////////////////////////////////////////////////////////////////////////


int debug_flag=0;
int main(void)
{
	delay_ms(1000);
	
	
	PMU->wr_key = 0x55aaaa55;
	PMU->cfg_normal = 0<<0 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 0<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10; //DRV3 V1.4

	
	
    // 配置 P13(P1.3) 引脚输出 
    CMP_TEST();
	while (1)
	{ 
		
	}
}
	


