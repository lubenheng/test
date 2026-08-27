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

	//设置偏置电压 (vdd33/2)
	CSA_SET_REF(CSA_REF_BUILD(CSA_REF_2V5, CSA_REF_DIV2));

	//设置放大倍数
	CSA_SET_PGAGAIN(CSA_PGA_GAIN_X8);
	
}

/*----------------------------------------------------------------------------
 * CSA 过流(OC)比较器初始化  —— 在 CSA_Init() 之后调用
 * 配置 OC 阈值(DAC)、检测信号、迟滞、消抖，最后使能 OC。
 * 注: 阈值 CSA_DAC_OUT_VALUE 需按实际采样电阻/增益/电流点标定(见 分析.md §五)。
 *----------------------------------------------------------------------------*/
void CSA_OC_Init(void)
{
	/* 1) OC 阈值来源: 内部 DAC(量程 + 8bit 阈值码) */
	CSA->MISC_CSACR0.RW_CSA_DAC_EN   = CSA_DAC_EN;
	CSA->MISC_CSACR0.RW_CSA_DAC_REF  = CSA_DAC_REF_SEL;
	CSA->MISC_CSACR0.RW_CSA_DAC_OUT  = CSA_DAC_OUT_VALUE;

	/* 2) OC 比较器参数(先配参数后使能, 避免误动作) */
	CSA->MISC_CSACR1.RW_CSA_OC_SEL   = CSA_OC_SEL_VALUE;   /* csp2: 与固定通道CH2一致 */
	CSA->MISC_CSACR1.RW_CSA_OC_HYS   = CSA_OC_HYS_VALUE;   /* 迟滞, 抑制噪声 */
	CSA->MISC_CSACR1.RW_CSA_OC_NG_EN = CSA_OC_NG_EN;       /* OC 指示消抖使能 */
	CSA->MISC_CSACR1.RW_CSA_OC_NG    = CSA_OC_NG_TIME;     /* 消抖时长 */

	/* 3) 使能 OC 检测 */
	CSA->MISC_CSACR1.RW_CSA_OC_EN    = CSA_OC_EN;
	
	*(volatile uint32_t*)0x40007C14 |= (1 << 10);//0x40007C14[10]=1
}

/*----------------------------------------------------------------------------
 * CSA 过流中断初始化  —— 使能 NVIC IRQ23(数据手册 CSA 过流中断)
 * 数据手册: CSA 中断 = IRQ 23(ISER bit23), 向量偏移 0x9C。
 * 中断服务函数见 isr.c 的 CSA_IRQHandler(向量表 index 39)。
 *----------------------------------------------------------------------------*/
void CSA_OC_IRQ_Init(void)
{
    NVIC_SetPriority(CSA_IRQn, 2);      /* IRQ23=CSA, 优先级 0~3 */
    NVIC_ClearPendingIRQ(CSA_IRQn);
    NVIC_EnableIRQ(CSA_IRQn);
	
	
}

