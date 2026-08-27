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

#define TIMER_ADC_DIV     TIMER_DIV_8
#define TIMER_ADC_TH      399    /* 采样率 = 32MHz/8/3999 ≈ 1kHz */

#define ADC_BASE_ADDR       (0x40012400UL)

#define ADC_CR_ADDR         (ADC_BASE_ADDR + 0x60UL)   /* CR */

mcpwm_shadow  stru_mcpwm_shadow;

uint32_t adc_val;
int32_t adc_csa_ch0_val;
int32_t adc_csa_ch1_val;
int32_t adc_csa_ch2_val;
int32_t adc_csa_ch3_val;

/* 零偏校准 */
int32_t g_offset_ch0;
int32_t g_offset_ch1;
int32_t g_offset_ch2;
int32_t g_offset_ch3;
uint8_t g_calib_done = 0;
struct_AdcTest_OL stru_AdcTest_OL =
{
	.STPT_CNT = 5,
	.COVT_CNT = 2,
	.SAPT_CNT = 13,
	.INIT_CNT = 3,
	.DELAY_CNT = 15,

	.INTR_SRC = ADC_IRQSRC_S0CP,

	.GroupMode = 1,
	.TigChNum = {4,0,0,0},
	.TigSrc = {0,0,0,0},
	.TigCnt = {(s16)EPWM_PERIOD/2,0,0,0},
	.Seq = {ADC_CH0,ADC_CH1,ADC_CH2,ADC_CH7,	\
					ADC_CH4,ADC_CH5,ADC_CH6,ADC_CH8,	\
					ADC_CH9,ADC_CH10,ADC_CH11,ADC_CH12,	\
					ADC_CH13,ADC_CH14,ADC_CH15,ADC_CH3}
};



void adc_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;
    

    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_15;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_14;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
	
	setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_0;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
}

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
      /* PA7(bit7) | PA8(bit8) | PA9(bit9) | PA10(bit10) | PA11(bit11) | PA12(bit12) | PA13(bit13) */
      csa_pin_mask = (1U << 5) |(1U << 7) | (1U << 8) | (1U << 9)
                   | (1U << 10) | (1U << 11) | (1U << 12) | (1U << 13);
      csa_pin_make_analog(csa_pin_mask);
  }
  
/*----------------------------------------------------------------------------
 * CSA (PGA/运算放大器) 初始化
 *----------------------------------------------------------------------------*/
void CSA_Init(void)
{

	csa_gpio_init();/* CSA 全部输入通道 GPIO 模拟配置 */

	/*  CSP同相端, CSN反向端 */
	// 使能输入通道
	CSA_SET_InputCH(CSA_FIXED_INPUT_CH2);

	// 输入通道模式
	CSA_InputCH_SWMode(CSA_FIXED_CH_MODE);

	// 输出通道: LPF 使能并接到运放输出 GPIO

	CSA->MISC_CSACR1.RW_CSA_LPF_SEL = 1;
	CSA->MISC_CSACR1.RW_CSA_LPF_GPIO = 1;
	CSA->MISC_CSACR1.RW_CSA_LPF = 3;
	
	*(volatile uint32_t*)0x40007C1C = 0xA9;
	
	//设置偏置电压 (vdd33/2)
	CSA_SET_REF(CSA_REF_BUILD(CSA_REF_VDD33, CSA_REF_DIV2));

	//设置放大倍数
	CSA_SET_PGAGAIN(CSA_PGA_GAIN_X8);
	
	/*  使能数字侧CSA模块，配置通道模式和增益 */
	CSA_ENABLE(ENABLE);
}

/*----------------------------------------------------------------------------
 * TIMER13 触发 ADC 采样参数
 *----------------------------------------------------------------------------*/

void TIMER_ADC_Init(void)
{
   
    Timer_drv_Setting_Clean_all(TIMER13);

    TIMER_Set_CLKDIV(TIMER13, TIMER_DIV_8);

    Timer_Set_ClkSrc(TIMER13, TIMERx_INNERCLK);

    Timer_Set_RLMode(TIMER13, TIMERx_AUTO_RELOAD);

    Timer_Set_CNTR_Dir(TIMER13, TIMERx_CNTR_UP);

    Timer_Set_Center_Mode(TIMER13, TIMERx_EDGE_MODE);

    Timer_Set_Shadow(TIMER13, TIMERx_SHADOW_REG_DIS);

    TIMER_Set_CH_Mode(TIMER13, TIMERx_CH0, TIMERx_MODE_PWM);

    Timer_Set_ThVal(TIMER13, TIMER_ADC_TH - 1);

    Timer_Set_Comp0_Val(TIMER13, 1);

    Timer_Set_EvtSrc(TIMER13, TIMER13_CH0_CAPCOM_EVT);
	
    Timer_IRQ_Enable(TIMER13, TIMERx_IRQ_CH0);

    NVIC_SetPriority(TIM13_IRQn, 1);
    NVIC_ClearPendingIRQ(TIM13_IRQn);
    NVIC_EnableIRQ(TIM13_IRQn);
    
    Timer_Reg_Update(TIMER13);

    Timer_Enable(TIMER13);
}
/**
 * @brief 按位域组合参数并直接写入寄存器
 * @param vref_range     [21:20] 量程：0=1.5V,1=2.0V,2=2.5V,3=3.0V
 * @param vrefh_to_vdd33 [17]    1=VREFH接VDD33
 * @param vrefh_en       [16]    1=使能VREFH
 * @param adc_clk_sel    [5:4]   采样时钟：0=250K,1=500K,2=1M,3=2M
 * @param adc_en         [0]     1=使能ADC
 */
void ADC_ConfigDirect(uint8_t vref_range,
                      uint8_t vrefh_to_vdd33,
                      uint8_t vrefh_en,
                      uint8_t adc_clk_sel,
                      uint8_t adc_en)
{
    uint32_t val = 0;
    val |= ((uint32_t)(vref_range & 0x3) << 20);
    val |= ((uint32_t)(vrefh_to_vdd33 & 0x1) << 17);
    val |= ((uint32_t)(vrefh_en & 0x1) << 16);
    val |= ((uint32_t)(adc_clk_sel & 0x3) << 4);
    val |= ((uint32_t)(adc_en & 0x1) << 0);
    // 其他未用位保持为0
	REG32(ADC_CR_ADDR) = val;
}


/*----------------------------------------------------------------------------
 * TIMER13 触发 ADC 采样 CSA  DEMO
 *   Group0: 4ch, SEQ0-3: CH0/1/2/7, 触发源: TMR13_TRIG
 *----------------------------------------------------------------------------*/
void adc_timer_trig_csa_demo(void)
{
	
	/* ADC 初始化 */
	ADC_Clk_Enable();

	ADC_SetSampleGrpMode(SampleGrpMode_1);
	ADC_SetGrpChCount(ADC_Sample_Group_0, 4);
	ADC_SetGrpChCount(ADC_Sample_Group_1, 0);
	ADC_SetGrpChCount(ADC_Sample_Group_2, 0);
	ADC_SetGrpChCount(ADC_Sample_Group_3, 0);

	ADC_SEQx_CFG(ADC_SEQ0, ADC_CH0);
	ADC_SEQx_CFG(ADC_SEQ1, ADC_CH1);
	ADC_SEQx_CFG(ADC_SEQ2, ADC_CH2);
	ADC_SEQx_CFG(ADC_SEQ3, ADC_CH3);

	/* 触发源: TIMER13 */
	ADC_SetGrpTrigSource(ADC_Sample_Group_0, ADC_GrpTrig_TMR13_TRIG);
	ADC_SetGrpTrigSource(ADC_Sample_Group_1, ADC_GrpTrig_TMR14_TRIG);
	ADC_SetGrpTrigSource(ADC_Sample_Group_2, ADC_GrpTrig_TMR15_TRIG);
	ADC_SetGrpTrigSource(ADC_Sample_Group_3, ADC_GrpTrig_TMR16_TRIG);

	ADC_SetCLK_STPT(10);    
//	ADC_SetCLK_COVT(2);
//	ADC_SetCLK_INIT(0);
//	ADC_SetCLK_SAPT(13);
//	ADC_SetCLK_TrigDelay(2);
	ADC_ConfigDirect(1,0,1,3,1);

	/* 使能ADC中断用于零偏校准 */
	ADC_Enable_IrqSrc(ADC_IRQSRC_S0CP);
	NVIC_SetPriority(ADC_IRQn, 2);
	NVIC_ClearPendingIRQ(ADC_IRQn);
	NVIC_EnableIRQ(ADC_IRQn);
	

	TIMER_ADC_Init();
	
}



	