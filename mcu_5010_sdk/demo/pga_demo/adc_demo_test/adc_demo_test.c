#include "platform_common.h"

#include "gpio_hal.h"
#include "ADC_Reg.h"
#include "ADC_Type.h"
#include "adc.h"
#include "adc_hal.h"
#include "mcpwm_hal.h"
#include "mcpwm_reg.h"
#include "mcpwm.h"
#include "TIM_Reg.h"
#include "TIM_Type.h"
#include "TIM.h"
#include "CSA_Reg.h"
#include "CSA_Type.h"
#include "CSA.h"
#include "adc_demo_test.h"

/* 将指定 PA 引脚清复用为 AF0(GPIO) 并禁用数字 I/O */
  static void csa_pin_make_analog(uint32_t pin_mask)
  {
      (*(volatile uint32_t *)0x4800004C) |= pin_mask;  // PA_AFCR — 清复用
      (*(volatile uint32_t *)0x48000018) |= pin_mask;  // PA_OECR — OE=0
      (*(volatile uint32_t *)0x48000020) |= pin_mask;  // PA_ITECR — IE=0
      (*(volatile uint32_t *)0x48000060) |= pin_mask;  // PA_PUCR — PU=0
      (*(volatile uint32_t *)0x48000068) |= pin_mask;  // PA_PDCR — PD=0
      (*(volatile uint32_t *)0x48000050) |= pin_mask;  // PA_ANASR — ANA=1
  }
  
   /* CSA 全部输入通道 GPIO 模拟配置 */
  void csa_gpio_init(void)
  {
      uint32_t csa_pin_mask;
      /* PA7(bit7) | PA8(bit8) | PA9(bit9) | PA10(bit10) | PA11(bit11) | PA12(bit12) */
      csa_pin_mask = (1U << 7) | (1U << 8) | (1U << 9)
                   | (1U << 10) | (1U << 11) | (1U << 12)| (1U << 13);
      csa_pin_make_analog(csa_pin_mask);
		
		
  }
  
/*----------------------------------------------------------------------------
 * CSA (PGA/运算放大器) 初始化
 *----------------------------------------------------------------------------*/
void CSA_Init(void)
{

	csa_gpio_init();/* CSA 全部输入通道 GPIO 模拟配置 */

	/*  使能数字侧CSA模块，配置通道模式和增益 */
	CSA_ENABLE(ENABLE);
	
	/*  CSP同相端, CSN反向端 */
	// 使能输入通道
	CSA_SET_InputCH(CSA_FIXED_INPUT_CH2);
	
	// 输入通道模式
	CSA_InputCH_SWMode(CSA_FIXED_CH_MODE);
	
	
	// 输出通道

	CSA->MISC_CSACR1.RW_CSA_LPF_SEL = 1;
	CSA->MISC_CSACR1.RW_CSA_LPF_GPIO = 1;
	CSA->MISC_CSACR1.RW_CSA_LPF = 3;

	//设置偏置电压
	CSA->MISC_CSACR0.RW_CSA_REF = 0x1F;
	
	//设置放大倍数
	CSA_SET_PGAGAIN(CSA_PGA_GAIN_X8);
	
}

