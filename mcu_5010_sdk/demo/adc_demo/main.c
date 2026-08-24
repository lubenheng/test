/****************************************************************************/
/** \file main.c
**
**  ADC demo for ODS_5010 (Cortex-M0+)
**  软件触发
**  
**
*****************************************************************************/
#include "platform_common.h"
#include "gpio_hal.h"
#include "clock.h"
#include "uart_hal.h"
#include "uart_hal_cfg.h"
#include "ADC_Reg.h"
#include "ADC_Type.h"
#include "adc.h"




/*----------------------------------------------------------------------------
   ADC demo 配置
 *----------------------------------------------------------------------------*/
#define ADC_BASE_ADDR       (0x40012400UL)
#define ADC_OTR_ADDR        (ADC_BASE_ADDR + 0x64UL)   /* 软件触发 (写 0x5AA5) */

uint32_t adc_val;
int start=0,i=0;
static void uart0_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_13;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_14;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	
}







static void adc_init(void)
{
    ADC_Clk_Enable();
    
    ADC_SetSampleGrpMode(SampleGrpMode_1);
    ADC_SetGrpChCount(ADC_Sample_Group_0, 16);
	ADC_SetGrpChCount(ADC_Sample_Group_1, 0);
	ADC_SetGrpChCount(ADC_Sample_Group_2, 0);
	ADC_SetGrpChCount(ADC_Sample_Group_3, 0);

    ADC_SetGrpTrigSource(ADC_Sample_Group_0, ADC_GrpTrig_SW_TRIG);
	ADC_SetGrpTrigSource(ADC_Sample_Group_1, ADC_GrpTrig_TMR13_TRIG);
	ADC_SetGrpTrigSource(ADC_Sample_Group_2, ADC_GrpTrig_TMR14_TRIG);
	ADC_SetGrpTrigSource(ADC_Sample_Group_3, ADC_GrpTrig_TMR15_TRIG);
	
    ADC_SEQx_CFG(ADC_SEQ0, ADC_CH18);
	ADC_SEQx_CFG(ADC_SEQ1, ADC_CH21);
	ADC_SEQx_CFG(ADC_SEQ2, ADC_CH22);
	ADC_SEQx_CFG(ADC_SEQ3, ADC_CH23);
	ADC_SEQx_CFG(ADC_SEQ4, ADC_CH24);
	ADC_SEQx_CFG(ADC_SEQ5, ADC_CH25);
	ADC_SEQx_CFG(ADC_SEQ6, ADC_CH26);
	ADC_SEQx_CFG(ADC_SEQ7, ADC_CH27);
	ADC_SEQx_CFG(ADC_SEQ8, ADC_CH28);
	ADC_SEQx_CFG(ADC_SEQ9, ADC_CH29);
	ADC_SEQx_CFG(ADC_SEQ10, ADC_CH30);
	ADC_SEQx_CFG(ADC_SEQ11, ADC_CH6);
	ADC_SEQx_CFG(ADC_SEQ12, ADC_CH7);
	ADC_SEQx_CFG(ADC_SEQ13, ADC_CH8);
	ADC_SEQx_CFG(ADC_SEQ14, ADC_CH9);
	ADC_SEQx_CFG(ADC_SEQ15, ADC_CH10);
	
	
    ADC_SetCLK_STPT(10);
//    ADC_SetCLK_COVT(10);
//    ADC_SetCLK_INIT(30);
//    ADC_SetCLK_SAPT(80);//
//    ADC_SetCLK_TrigDelay(2);
	ADC->CR |= (1UL << 16);             //CR.VREFH_EN = 1 
	ADC->CR |= (3UL << 4);            
	//ADC 采样时钟选择:00:  250K sps  (4us)01:  500K sps (2us)10:  1M sps (1us)11:  2M sps (0.5us)
	ADC->CR |= (0x01 << 0);//ADC使能位
	
	INFO("ADC_init_ok \r\n");
}


static void adc_software_trigger_demo(void)
{

	 /* 1. 软件触发: OTR = 0x5AA5 */
    REG32(ADC_OTR_ADDR) = 0x5AA5UL;
    

    /* 等待转换完成*/
           
    while (ADC->IR.S0CDIF == 0U)
    {

    }

        ADC->IR.S0CDIF = 1U;

        adc_val = ADC_GetResult(0);
		i++;
//		INFO("ADC_CH10= %d\r\n",adc_val);

}




int main(void)
{
    delay_1us(200000);
    flash_clk_enable();
    gpio_clk_enable();
    uart_clk_enable();

	
#ifdef UART
    uart0_gpio_init();
   
    (void)uart_init(UART_CONSOLE_INSTANCE, &uart0_init_config);
    uart_transmit_enable(UART_CONSOLE_INSTANCE, true);

    INFO("\n SDK start running...%s,%s,%s\n", FW_VER, COMPILER_DATE, COMPILER_TIME);
    uint32_t mcu_clk = 0U;
    (void)clock_freq_get(SCG_SYS, &mcu_clk);
    INFO("MCU_CLK = %dMHz\n", mcu_clk/1000000U);
#endif


    adc_init();
	




    while(1)
    {
		delay_ms(100);
//		if(start==1)
//		{
        adc_software_trigger_demo();
//		start=0;
//		}
      
    }
}