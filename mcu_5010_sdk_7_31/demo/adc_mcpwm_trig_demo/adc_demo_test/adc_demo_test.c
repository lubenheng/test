#include "platform_common.h"

#include "gpio_hal.h"
#include "ADC_Reg.h"
#include "ADC_Type.h"
#include "adc.h"
#include "adc_hal.h"
#include "mcpwm_hal.h"
#include "mcpwm_reg.h"
#include "mcpwm.h"
#include "adc_demo_test.h"


mcpwm_shadow  stru_mcpwm_shadow;

uint32_t adc_val;
struct_AdcTest_OL stru_AdcTest_OL =
{
	.STPT_CNT = 5,
	.COVT_CNT = 2,
	.SAPT_CNT = 13,
	.INIT_CNT = 3,
	.DELAY_CNT = 15,

	.INTR_SRC = ADC_IRQSRC_S0CP,

	.GroupMode = 2,
	.TigChNum = {3,4,3,1},
	.TigSrc = {0,1,2,3},
	.TigCnt = {-(s16)EPWM_PERIOD /8 + 12,(s16)EPWM_PERIOD/4,(s16)EPWM_PERIOD/2, EPWM_PERIOD*3/4},
	.Seq = {ADC_CH0,ADC_CH1,ADC_CH2,ADC_CH3,	\
					ADC_CH4,ADC_CH5,ADC_CH6,ADC_CH7,	\
					ADC_CH8,ADC_CH9,ADC_CH10,ADC_CH11,	\
					ADC_CH12,ADC_CH13,ADC_CH14,ADC_CH15}
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
