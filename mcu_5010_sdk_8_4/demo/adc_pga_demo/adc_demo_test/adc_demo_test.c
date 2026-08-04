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


mcpwm_shadow  stru_mcpwm_shadow;

uint32_t adc_val;
int32_t adc_csa_ch0_val;
int32_t adc_csa_ch1_val;
int32_t adc_csa_ch2_val;
int32_t adc_reserved_ch_val;

/* 零偏校准 */
int32_t g_offset_ch0;
int32_t g_offset_ch1;
int32_t g_offset_ch2;
int32_t g_offset_ch7;
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
	
//    setting_cfg.gpio_port     = ADC_DEMO_GPIO_PORT;
//    setting_cfg.gpio_pin      = ADC_DEMO_GPIO_PIN;
//    setting_cfg.gpio_alt      = GPIO_ALT_FUNC_0;
//    setting_cfg.gpio_dir      = GPIO_DIR_INPUT;
//    setting_cfg.pull_cfg      = GPIO_PULL_DISABLE;
//    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
//    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//    gpio_init(&setting_cfg);

//    /* GPIO8 (PTA8) — CSA_CSP 反相输入端, 配置为模拟输入 */
//    setting_cfg.gpio_port     = GPIO_A;
//    setting_cfg.gpio_pin      = GPIO_PIN_8;
//    setting_cfg.gpio_alt      = GPIO_ALT_FUNC_0;
//    setting_cfg.gpio_dir      = GPIO_DIR_INPUT;
//    setting_cfg.pull_cfg      = GPIO_PULL_DISABLE;
//    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
//    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//    gpio_init(&setting_cfg);
//	
//	/* GPIO7 (PTA7) — CSA_CSP 同相输入端, 配置为模拟输入 */
//    setting_cfg.gpio_port     = GPIO_A;
//    setting_cfg.gpio_pin      = GPIO_PIN_7;
//    setting_cfg.gpio_alt      = GPIO_ALT_FUNC_0;
//    setting_cfg.gpio_dir      = GPIO_DIR_INPUT;
//    setting_cfg.pull_cfg      = GPIO_PULL_DISABLE;
//    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
//    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//    gpio_init(&setting_cfg);
	
	
    setting_cfg.gpio_port= GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_6;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
}



/*----------------------------------------------------------------------------
 * CSA (PGA/运算放大器) 初始化
 *----------------------------------------------------------------------------*/
void CSA_Init(void)
{
	/* 1. ANAC_ENCR(0x40007800): bit0=ANA_EN总使能, bit17=ANA_CSA_EN */
	(*(volatile uint32_t *)0x40007800UL) |= (1UL << 0) | (1UL << 17);

	/* 2. ANAC_CSA1(0x40007810): 模拟侧CSA放大器使能+通道+增益
	 *    bit0: CSA_AMP_EN=1  使能模拟放大器
	 *    bit1-2: CSA_CSP=0   正端CSP0(PTA7)
	 *    bit3-4: CSA_CSN=3   负端接地
	 *    bit5-7: CSA_GAIN    增益(3bit,对应1x~8x)
	 */

	/* 3. 使能数字侧CSA模块，配置通道模式和增益 */
	CSA_ENABLE(ENABLE);
	CSA->CR0.CSA_AMP_EN = 1;
	CSA_InputCH_SWMode(CSA_FIXED_CH_MODE);
	/* CSP=GPIO7(00), CSN=GND(11) — 同相端接信号源, 反向端接地 */
	//CSA_SET_InputCH(CSA_FIXED_INPUT_CH0);	
	CSA->CR0.CSA_CSP = 0;
	CSA->CR0.CSA_CSN = 3;
	CSA_SET_PGAGAIN(CSA_PGA_GAIN);
	
//	*(int32_t *)0x40015804 = 0x11;
//	*(int32_t *)0x4800083C = 0xfff00000;
//	*(int32_t *)0x48000840 = 0x0000000f;
	
	*(uint32_t *)0x4800003C = 0x10000000;  // PA7→ALT_FUNC_1
    *(uint32_t *)0x48000040 = 0x00011111;  // PA8~PA12→ALT_FUNC_1
    *(uint32_t *)0x4800004C = 0x00001F80;  // 提交
    *(uint32_t *)0x48000050 = 0x00001F80;  // 使能模拟
}

/*----------------------------------------------------------------------------
 * TIMER13 触发 ADC 采样参数
 *----------------------------------------------------------------------------*/
#define TIMER_ADC_DIV     TIMER_DIV_8
#define TIMER_ADC_TH      3999    /* 采样率 = 32MHz/8/3999 ≈ 1kHz */

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
/*----------------------------------------------------------------------------
 * TIMER13 触发 ADC 采样 CSA  DEMO
 *   Group0: 4ch, SEQ0-3: CH0/1/2/7, 触发源: TMR13_TRIG
 *----------------------------------------------------------------------------*/
void adc_timer_trig_csa_demo(void)
{
	INFO("=== ADC_TIMER_TRIG_CSA_DEMO START ===\r\n");

	

	/* ADC 初始化 */
	ADC_Clk_Enable();
	ADC_ENABLE(ENABLE);
	ADC_SetSampleGrpMode(SampleGrpMode_1);
	ADC_SetGrpChCount(ADC_Sample_Group_0, 4);
	ADC_SetGrpChCount(ADC_Sample_Group_1, 0);
	ADC_SetGrpChCount(ADC_Sample_Group_2, 0);
	ADC_SetGrpChCount(ADC_Sample_Group_3, 0);

	ADC_SEQx_CFG(ADC_SEQ0, ADC_CSA_CH0);
	ADC_SEQx_CFG(ADC_SEQ1, ADC_CSA_CH1);
	ADC_SEQx_CFG(ADC_SEQ2, ADC_CSA_CH2);
	ADC_SEQx_CFG(ADC_SEQ3, ADC_RESERVED_CH);

	/* 触发源: TIMER13 */
	ADC_SetGrpTrigSource(ADC_Sample_Group_0, ADC_GrpTrig_TMR13_TRIG);

	ADC_SetCLK_STPT(4);
	ADC_SetCLK_COVT(2);
	ADC_SetCLK_INIT(0);
	ADC_SetCLK_SAPT(13);
	ADC_SetCLK_TrigDelay(2);

	/* 使能ADC中断用于零偏校准 */
	ADC_Enable_IrqSrc(ADC_IRQSRC_S0CP);
	NVIC_SetPriority(ADC_IRQn, 2);
	NVIC_ClearPendingIRQ(ADC_IRQn);
	NVIC_EnableIRQ(ADC_IRQn);
	INFO("ADC Init OK (Group0:4ch CH0/1/2/7, Trig:TMR13, IRQ:S0CP)\r\n");

	TIMER_ADC_Init();
	INFO("TIMER13 Init OK\r\n");
	INFO("=== DEMO READY ===\r\n");
}

/*----------------------------------------------------------------------------
 * 打印4通道电压值 (mV) - ISR中已完成零偏校准+mV转换
 *----------------------------------------------------------------------------*/
void print_all_channels(void)
{
	INFO("CH0=%dmV, CH1=%dmV, CH2=%dmV, CH7=%dmV\r\n",
		adc_csa_ch0_val,
        adc_csa_ch1_val,
        adc_csa_ch2_val,
        adc_reserved_ch_val);
	}