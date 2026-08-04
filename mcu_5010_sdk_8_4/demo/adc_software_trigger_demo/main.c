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
#define ADC_DEMO_CHANNEL    ADC_CH7
#define ADC_DEMO_GPIO_PORT  GPIO_B
#define ADC_DEMO_GPIO_PIN   GPIO_PIN_12
#define ADC_SAMPLE_COUNT    100U


static void uart0_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_1;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_2;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_2;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
}




static void adc_gpio_init(void)
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


static void adc_init(void)
{
    ADC_Clk_Enable();
    ADC_ENABLE(ENABLE);
    ADC_SetSampleGrpMode(SampleGrpMode_1);
    ADC_SetGrpChCount(ADC_Sample_Group_0, 1);

    ADC_SetGrpTrigSource(ADC_Sample_Group_0, ADC_GrpTrig_SW_TRIG);
    ADC_SEQx_CFG(ADC_SEQ0, ADC_DEMO_CHANNEL);
    ADC_SetCLK_STPT(4);
    ADC_SetCLK_COVT(2);
    ADC_SetCLK_INIT(0);
    ADC_SetCLK_SAPT(13);
    ADC_SetCLK_TrigDelay(2);
}


static void adc_software_trigger_demo(void)
{
    uint32_t i;
    uint32_t adc_val;
    INFO("ADC_DEMO--START\r\n");
    for (i = 0U; i < ADC_SAMPLE_COUNT; i++)
    {
        ADC_SWTrig();
        while (ADC->IR.S0CDIF == 0U)
        {
            gpio_output_set(GPIO_C, GPIO_PIN_5, GPIO_HIGH_LEVEL);
        }
        gpio_output_set(GPIO_C, GPIO_PIN_5, GPIO_LOW_LEVEL);
        ADC->IR.S0CDIF = 1U;
        adc_val = ADC_GetResult(0);
        INFO("ADC_CH7[%d] = %d\r\n", i, adc_val);
    }
    INFO("ADC_DEMO--OK\r\n");
}




int main(void)
{
    delay_1us(2000);
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

    adc_gpio_init();
    adc_init();

    adc_software_trigger_demo();

    

    while(1)
    {
       
      
    }
}