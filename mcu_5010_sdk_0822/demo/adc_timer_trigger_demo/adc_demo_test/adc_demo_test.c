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

#define ADC_BASE_ADDR       (0x40012400UL)
#define ADC_CR_ADDR         (ADC_BASE_ADDR + 0x60UL)   /* CR */

uint32_t adc_val;


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
   
    ADC_Clk_Enable();
    

    ADC_SetSampleGrpMode(SampleGrpMode_1);

    ADC_SetGrpChCount(ADC_Sample_Group_0, 1);  
    ADC_SetGrpChCount(ADC_Sample_Group_1, 0);   
    ADC_SetGrpChCount(ADC_Sample_Group_2, 0);
    ADC_SetGrpChCount(ADC_Sample_Group_3, 0);

    
    ADC_SEQx_CFG(ADC_SEQ0, ADC_CH27);
	ADC_SEQx_CFG(ADC_SEQ1, ADC_CH21);
	ADC_SEQx_CFG(ADC_SEQ2, ADC_CH22);
	ADC_SEQx_CFG(ADC_SEQ3, ADC_CH23);
	ADC_SEQx_CFG(ADC_SEQ4, ADC_CH24);
	ADC_SEQx_CFG(ADC_SEQ5, ADC_CH25);
	ADC_SEQx_CFG(ADC_SEQ6, ADC_CH26);
	ADC_SEQx_CFG(ADC_SEQ7, ADC_CH18);
	ADC_SEQx_CFG(ADC_SEQ8, ADC_CH28);
	ADC_SEQx_CFG(ADC_SEQ9, ADC_CH29);
	ADC_SEQx_CFG(ADC_SEQ10, ADC_CH30);
	ADC_SEQx_CFG(ADC_SEQ11, ADC_CH6);
	ADC_SEQx_CFG(ADC_SEQ12, ADC_CH7);
	ADC_SEQx_CFG(ADC_SEQ13, ADC_CH8);
	ADC_SEQx_CFG(ADC_SEQ14, ADC_CH9);
	ADC_SEQx_CFG(ADC_SEQ15, ADC_CH10);

    ADC_SetGrpTrigSource(ADC_Sample_Group_0, ADC_GrpTrig_TMR13_TRIG);
    ADC_SetGrpTrigSource(ADC_Sample_Group_1, ADC_GrpTrig_TMR14_TRIG);
    ADC_SetGrpTrigSource(ADC_Sample_Group_2, ADC_GrpTrig_TMR15_TRIG);
    ADC_SetGrpTrigSource(ADC_Sample_Group_3, ADC_GrpTrig_TMR16_TRIG);

     ADC_SetCLK_STPT(10);
//    ADC_SetCLK_COVT(10);
//    ADC_SetCLK_INIT(30);
//    ADC_SetCLK_SAPT(80);
//    ADC_SetCLK_TrigDelay(2);
	
	ADC_ConfigDirect(1,0,1,3,1);
	
    TIMER_ADC_Init();
	

    
}

