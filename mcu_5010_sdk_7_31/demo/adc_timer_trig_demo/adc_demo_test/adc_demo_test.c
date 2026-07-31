#include "platform_common.h"

#include "gpio_hal.h"
#include "ADC_Reg.h"
#include "ADC_Type.h"
#include "adc.h"
#include "adc_hal.h"
#include "TIM_Reg.h"
#include "TIM_Type.h"
#include "TIM.h"
#include "adc_demo_test.h"


uint32_t adc_val;


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


void pb_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;
	
	
    setting_cfg.gpio_port = GPIO_C;
    setting_cfg.gpio_pin = GPIO_PIN_6;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

}

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

    Timer_Set_ThVal(TIMER13, 3999 - 1);

    Timer_Set_Comp0_Val(TIMER13, 1);

    Timer_Set_EvtSrc(TIMER13, TIMER13_CH0_CAPCOM_EVT);
	
    Timer_IRQ_Enable(TIMER13, TIMERx_IRQ_CH0);

    NVIC_SetPriority(TIM13_IRQn, 1);
    NVIC_ClearPendingIRQ(TIM13_IRQn);
    NVIC_EnableIRQ(TIM13_IRQn);
    
    Timer_Reg_Update(TIMER13);

    Timer_Enable(TIMER13);
}


void adc_timer_trigger_demo(void)
{
    INFO("ADC_TIMER_DEMO--START\r\n");

    
    ADC_Clk_Enable();
    ADC_ENABLE(ENABLE);

    ADC_SetSampleGrpMode(SampleGrpMode_4);

    ADC_SetGrpChCount(ADC_Sample_Group_0, 1);  
    ADC_SetGrpChCount(ADC_Sample_Group_1, 0);   
    ADC_SetGrpChCount(ADC_Sample_Group_2, 0);
    ADC_SetGrpChCount(ADC_Sample_Group_3, 0);

    
    ADC_SEQx_CFG(ADC_SEQ0,  ADC_CH7);
    ADC_SEQx_CFG(ADC_SEQ1,  ADC_CH1);
    ADC_SEQx_CFG(ADC_SEQ2,  ADC_CH2);
    ADC_SEQx_CFG(ADC_SEQ3,  ADC_CH3);
    ADC_SEQx_CFG(ADC_SEQ4,  ADC_CH4);
    ADC_SEQx_CFG(ADC_SEQ5,  ADC_CH5);
    ADC_SEQx_CFG(ADC_SEQ6,  ADC_CH6);
    ADC_SEQx_CFG(ADC_SEQ7,  ADC_CH0);
    ADC_SEQx_CFG(ADC_SEQ8,  ADC_CH8);
    ADC_SEQx_CFG(ADC_SEQ9,  ADC_CH9);
    ADC_SEQx_CFG(ADC_SEQ10, ADC_CH10);
    ADC_SEQx_CFG(ADC_SEQ11, ADC_CH11);
    ADC_SEQx_CFG(ADC_SEQ12, ADC_CH12);
    ADC_SEQx_CFG(ADC_SEQ13, ADC_CH13);
    ADC_SEQx_CFG(ADC_SEQ14, ADC_CH14);
    ADC_SEQx_CFG(ADC_SEQ15, ADC_CH15);

    ADC_SetGrpTrigSource(ADC_Sample_Group_0, ADC_GrpTrig_TMR13_TRIG);
    ADC_SetGrpTrigSource(ADC_Sample_Group_1, ADC_GrpTrig_TMR13_TRIG);
    ADC_SetGrpTrigSource(ADC_Sample_Group_2, ADC_GrpTrig_TMR13_TRIG);
    ADC_SetGrpTrigSource(ADC_Sample_Group_3, ADC_GrpTrig_TMR13_TRIG);

    ADC_SetCLK_STPT(4);
    ADC_SetCLK_COVT(2);
    ADC_SetCLK_INIT(0);
    ADC_SetCLK_SAPT(13);
    ADC_SetCLK_TrigDelay(2);

    TIMER_ADC_Init();

    
}

