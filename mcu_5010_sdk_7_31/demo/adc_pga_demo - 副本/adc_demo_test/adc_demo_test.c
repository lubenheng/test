#include "platform_common.h"

#include "gpio_hal.h"
#include "ADC_Reg.h"
#include "ADC_Type.h"
#include "adc.h"
#include "adc_hal.h"
#include "mcpwm_hal.h"
#include "mcpwm_reg.h"
#include "mcpwm.h"
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
    setting_cfg.gpio_port     = ADC_DEMO_GPIO_PORT;
    setting_cfg.gpio_pin      = ADC_DEMO_GPIO_PIN;
    setting_cfg.gpio_alt      = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir      = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg      = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
}

void mcpwm0_gpio_init(void)
{
	gpio_settings_config_t setting_cfg;

	/* MCPWM_CH0P, PTA6 PIN  */
	setting_cfg.gpio_port = GPIO_A;
	setting_cfg.gpio_pin = GPIO_PIN_6;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_15;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);


	/* MCPWM_CH1P, PTA7 PIN  */
	setting_cfg.gpio_port = GPIO_A;
	setting_cfg.gpio_pin = GPIO_PIN_7;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_15;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);


	/* MCPWM_CH2P, PTA8 PIN  */
	setting_cfg.gpio_port = GPIO_A;
	setting_cfg.gpio_pin = GPIO_PIN_8;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_15;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);



	/* MCPWM_CH0N, PTA9 PIN  */
	setting_cfg.gpio_port = GPIO_A;
	setting_cfg.gpio_pin = GPIO_PIN_9;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_15;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);

	/* MCPWM_CH1N, PTA10 PIN  */
	setting_cfg.gpio_port = GPIO_A;
	setting_cfg.gpio_pin = GPIO_PIN_10;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_15;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);

	/* MCPWM_CH2N, PTA11 PIN  */
	setting_cfg.gpio_port = GPIO_A;
	setting_cfg.gpio_pin = GPIO_PIN_11;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_15;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);

}

void ADC_HWTrig_PWM_Config(int InitCnt)
{
	//1.stop mcpwm cntr
	MCPWM0_TCLK &= ~(0x03<<6);


	MCPWM0_CNT0 = (uint16_t)(InitCnt);
	MCPWM0_CNT1 = (uint16_t)(InitCnt);

	MCPWM0_UPDATE |= (0x01<<11);
	MCPWM0_UPDATE |= (0x01<<27);


	MCPWM0_TCLK |= (0x01<<6);

}

void aadc_mcpwm_trigger_demo(void)
{

	INFO("ADC_DEMO--START\r\n");
	//------------------------------------------------------------------------/
	// ADC INIT
	//enable adc
	 ADC_Clk_Enable();
	ADC_ENABLE(ENABLE);
	// set group mode
	ADC_SetSampleGrpMode(SampleGrpMode_4);

	// set group number
	ADC_SetGrpChCount(ADC_Sample_Group_0,1);
	ADC_SetGrpChCount(ADC_Sample_Group_1,3);
	ADC_SetGrpChCount(ADC_Sample_Group_2,0);
	ADC_SetGrpChCount(ADC_Sample_Group_3,0);


	// set seq
	ADC_SEQx_CFG(ADC_SEQ0,ADC_CH7);
	ADC_SEQx_CFG(ADC_SEQ1,ADC_CH1);
	ADC_SEQx_CFG(ADC_SEQ2,ADC_CH2);
	ADC_SEQx_CFG(ADC_SEQ3,ADC_CH3);

	ADC_SEQx_CFG(ADC_SEQ4,ADC_CH4);
	ADC_SEQx_CFG(ADC_SEQ5,ADC_CH5);
	ADC_SEQx_CFG(ADC_SEQ6,ADC_CH6);
	ADC_SEQx_CFG(ADC_SEQ7,ADC_CH0);

	ADC_SEQx_CFG(ADC_SEQ8,ADC_CH8);
	ADC_SEQx_CFG(ADC_SEQ9,ADC_CH9);
	ADC_SEQx_CFG(ADC_SEQ10,ADC_CH10);
	ADC_SEQx_CFG(ADC_SEQ11,ADC_CH11);

	ADC_SEQx_CFG(ADC_SEQ12,ADC_CH12);
	ADC_SEQx_CFG(ADC_SEQ13,ADC_CH13);
	ADC_SEQx_CFG(ADC_SEQ14,ADC_CH14);
	ADC_SEQx_CFG(ADC_SEQ15,ADC_CH15);
	// set trigger

	ADC_SetGrpTrigSource(ADC_Sample_Group_0,ADC_GrpTrig_MCPWM_TRIG0);
	ADC_SetGrpTrigSource(ADC_Sample_Group_1,ADC_GrpTrig_MCPWM_TRIG1);
	ADC_SetGrpTrigSource(ADC_Sample_Group_2,ADC_GrpTrig_MCPWM_TRIG2);
	ADC_SetGrpTrigSource(ADC_Sample_Group_3,ADC_GrpTrig_MCPWM_TRIG3);


	ADC_SetCLK_STPT(4);
	ADC_SetCLK_COVT(2);
	ADC_SetCLK_INIT(0);
	ADC_SetCLK_SAPT(13);
	ADC_SetCLK_TrigDelay(2);

	//------------------------------------------------------------------------/
	// MCPWM INIT


	MCPWM_ch012_init();
	mcpwm0_gpio_init();

	NVIC_SetPriority(MCPWM0_IRQn, 2); /*TIMER0优先级配置 */
	NVIC_ClearPendingIRQ(MCPWM0_IRQn);
	NVIC_EnableIRQ (MCPWM0_IRQn);

	// 触发源23设置
	MCPWM0_PRT = 0x0000DEAD;
	// 23选择时基0
	MCPWM0->TCLK &= ~(0x3<<4);
	MCPWM0->TCLK |= (0x0<<4);
	// 自动更新
	MCPWM0->AUEN &= ~(0x3 << 24);
	MCPWM0->AUEN |= (0x3 << 24);
	MCPWM0_PRT		= 0x0000CAFE;

	// 设置采样触发点。

	MCPWM_SetTrigCnt(MCPWM_TIG_TMR0,stru_AdcTest_OL.TigCnt[0]);
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR1,stru_AdcTest_OL.TigCnt[1]);
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR2,stru_AdcTest_OL.TigCnt[2]);
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR3,stru_AdcTest_OL.TigCnt[3]);

	ADC_HWTrig_PWM_Config(-EPWM_HALFPERIOD);

	INFO("ADC_DEMO--OK\r\n");

}

/*----------------------------------------------------------------------------
 * CSA (PGA/运算放大器) 初始化
 *----------------------------------------------------------------------------*/
void CSA_Init(void)
{
	CSA_ENABLE(ENABLE);
	CSA->CR0.CSA_AMP_EN = 1;
	CSA_InputCH_SWMode(CSA_FIXED_CH_MODE);
	CSA_SET_InputCH(CSA_FIXED_INPUT_CH0);
	CSA_SET_PGAGAIN(CSA_PGA_GAIN);
}

/*----------------------------------------------------------------------------
 * MCPWM ADC_TRIG0 触发 CSA 采样 DEMO
 *   Group0: 4ch, SEQ0-3: CH0/1/2/7, 触发源: MCPWM_TRIG0
 *----------------------------------------------------------------------------*/
void adc_mcpwm_trig0_csa_demo(void)
{
	INFO("=== ADC_MCPWM_TRIG0_CSA_DEMO START ===\r\n");

	/* CSA 初始化 */
	CSA_Init();
	INFO("CSA Init OK (Gain=x%d, Mode=Fixed_CH)\r\n", (1 << CSA_PGA_GAIN));

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
	ADC_SEQx_CFG(ADC_SEQ4,  ADC_CH4);
	ADC_SEQx_CFG(ADC_SEQ5,  ADC_CH5);
	ADC_SEQx_CFG(ADC_SEQ6,  ADC_CH6);
	ADC_SEQx_CFG(ADC_SEQ7,  ADC_CH8);
	ADC_SEQx_CFG(ADC_SEQ8,  ADC_CH9);
	ADC_SEQx_CFG(ADC_SEQ9,  ADC_CH10);
	ADC_SEQx_CFG(ADC_SEQ10, ADC_CH11);
	ADC_SEQx_CFG(ADC_SEQ11, ADC_CH12);
	ADC_SEQx_CFG(ADC_SEQ12, ADC_CH13);
	ADC_SEQx_CFG(ADC_SEQ13, ADC_CH14);
	ADC_SEQx_CFG(ADC_SEQ14, ADC_CH15);
	ADC_SEQx_CFG(ADC_SEQ15, ADC_CH3);

	ADC_SetGrpTrigSource(ADC_Sample_Group_0, ADC_GrpTrig_MCPWM_TRIG0);
	ADC_SetGrpTrigSource(ADC_Sample_Group_1, ADC_GrpTrig_MCPWM_TRIG0);
	ADC_SetGrpTrigSource(ADC_Sample_Group_2, ADC_GrpTrig_MCPWM_TRIG0);
	ADC_SetGrpTrigSource(ADC_Sample_Group_3, ADC_GrpTrig_MCPWM_TRIG0);

	ADC_SetCLK_STPT(4);
	ADC_SetCLK_COVT(2);
	ADC_SetCLK_INIT(0);
	ADC_SetCLK_SAPT(13);
	ADC_SetCLK_TrigDelay(2);

	/* 使能ADC中断用于零偏校准 */
	ADC_Enable_IrqSrc(ADC_IRQSRC_S0CP);
	NVIC_SetPriority(ADC_IRQn, 1);
	NVIC_ClearPendingIRQ(ADC_IRQn);
	NVIC_EnableIRQ(ADC_IRQn);
	INFO("ADC Init OK (Group0:4ch CH0/1/2/7, Trig:MCPWM_TRIG0, IRQ:S0CP)\r\n");

	/* MCPWM 初始化 */
	MCPWM_ch012_init();
	mcpwm0_gpio_init();
	NVIC_SetPriority(MCPWM0_IRQn, 2);
	NVIC_ClearPendingIRQ(MCPWM0_IRQn);
	NVIC_EnableIRQ(MCPWM0_IRQn);

	MCPWM0_PRT = 0x0000DEAD;
	MCPWM0->TCLK &= ~(0x3 << 4);
	MCPWM0->TCLK |= (0x0 << 4);
	MCPWM0->AUEN &= ~(0x3 << 24);
	MCPWM0->AUEN |= (0x3 << 24);
	MCPWM0_PRT = 0x0000CAFE;

	MCPWM_SetTrigCnt(MCPWM_TIG_TMR0, stru_AdcTest_OL.TigCnt[0]);
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR1, stru_AdcTest_OL.TigCnt[1]);
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR2, stru_AdcTest_OL.TigCnt[2]);
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR3, stru_AdcTest_OL.TigCnt[3]);

	ADC_HWTrig_PWM_Config(-EPWM_HALFPERIOD);
	INFO("MCPWM Init OK (TMR0=%d, PWM=%dHz)\r\n",
		 stru_AdcTest_OL.TigCnt[0], EPWM_FREQ);
	INFO("=== DEMO READY ===\r\n");
}

/*----------------------------------------------------------------------------
 * 打印4通道电压值 (V) - ISR中已完成零偏校准+mV转换
 *----------------------------------------------------------------------------*/
void print_all_channels(void)
{
	INFO("CH0=%d.%03dV CH1=%d.%03dV CH2=%d.%03dV CH7=%d.%03dV\r\n",
		 adc_csa_ch0_val / 1000,     adc_csa_ch0_val % 1000,
		 adc_csa_ch1_val / 1000,     adc_csa_ch1_val % 1000,
		 adc_csa_ch2_val / 1000,     adc_csa_ch2_val % 1000,
		 adc_reserved_ch_val / 1000, adc_reserved_ch_val % 1000);
}