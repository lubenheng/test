#include "platform_common.h"

#include "test.h"

#include "hall_reg.h"
#include "hall.h"
#include "hall_hal.h"
#include "hall_test.h"
#include "cmdline_internal.h"

#include "gpio_type.h"
#include "gpio_hal.h"

#include "mcpwm_test.h"


void hall_func1_test(uint32_t test_case, uint32_t para1, uint32_t para2)
{




}	



void hall_func2_test(uint32_t test_case, uint32_t para1, uint32_t para2)
{




}	

void hall_soft_trig_start_test()
{
    HALL_SoftTrig_Start();
}





void hall_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;

	/* GPIO, HALL_0/W, PTA12 PIN  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_12;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_12;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    setting_cfg.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_EITHER_EDGE;
    setting_cfg.gpio_interrupt_cfg_info.priority = 1;
    gpio_init(&setting_cfg);

    /* GPIO, HALL_1/V, PTA13 PIN  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_13;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_12;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    setting_cfg.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_EITHER_EDGE;
    setting_cfg.gpio_interrupt_cfg_info.priority = 1;
    gpio_init(&setting_cfg);

    /* GPIO, HALL_2/U, PTA14 PIN  */
    setting_cfg.gpio_port = GPIO_A;
    setting_cfg.gpio_pin = GPIO_PIN_14;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_12;
    setting_cfg.gpio_dir = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    setting_cfg.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_EITHER_EDGE;
    setting_cfg.gpio_interrupt_cfg_info.priority = 1;
    gpio_init(&setting_cfg);

}

/* 用与将滤波后的HALL信号输出到引脚 */
void hall_debug_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;

	/* GPIO, HALL_0/W, PTB13 PIN  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_13;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_3;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    setting_cfg.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_EITHER_EDGE;
    setting_cfg.gpio_interrupt_cfg_info.priority = 1;
    gpio_init(&setting_cfg);

    /* GPIO, HALL_1/V, PTB14 PIN  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_14;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_3;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    setting_cfg.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_EITHER_EDGE;
    setting_cfg.gpio_interrupt_cfg_info.priority = 1;
    gpio_init(&setting_cfg);

    /* GPIO, HALL_2/U, PTB15 PIN  */
    setting_cfg.gpio_port = GPIO_B;
    setting_cfg.gpio_pin = GPIO_PIN_15;
    setting_cfg.gpio_alt = GPIO_ALT_FUNC_3;
    setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    setting_cfg.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_EITHER_EDGE;
    setting_cfg.gpio_interrupt_cfg_info.priority = 1;
    gpio_init(&setting_cfg);

    REG32(0x40015804) = 0x20; //TSEL(for debug) 

}

uint16_t g_hall_callback_cnt = 0;
uint16_t g_hall_overtime_callback_cnt = 0;
uint16_t g_hall_software_callback_cnt = 0;
uint16_t hall_motor_enable = 0;
uint16_t hall_motor_duty = 4096; // [0,32768]---->[0%,100%]
void g_motor_ctrl_hall_callback(hall_int_source_e source)
{
    uint16_t sector_tmp = 0; //通过扫描IO口得到扇区信息
	g_hall_callback_cnt++;
	
	gpio_toggle(GPIO_C,GPIO_PIN_9);

    switch (source)
    {
        case HALL_VALUE_1:
        case HALL_VALUE_2:
        case HALL_VALUE_3:
        case HALL_VALUE_4:
        case HALL_VALUE_5:
        case HALL_VALUE_6:
            mcpwm0_bldc_hall_uvw_test(hall_motor_enable,source,hall_motor_duty);
            break;
        case HALL_OVERTIME:
            g_hall_overtime_callback_cnt++;
            /* 超过时间未获取到霍尔信号边沿中断，通过扫描IO口得到扇区信息 */
            sector_tmp = 1*GPIO_HALL_U + 2*GPIO_HALL_V + 4*GPIO_HALL_W;
            mcpwm0_bldc_hall_uvw_test(hall_motor_enable,sector_tmp,hall_motor_duty);
            break;

        case HALL_SOFTWARE_TRG:
            g_hall_software_callback_cnt++;
            /* 第一次上电没有霍尔边沿信号，软件产生一次霍尔中断，超过时间未获取到霍尔信号边沿中断，通过扫描IO口得到扇区信息 */
            sector_tmp = 1*GPIO_HALL_U + 2*GPIO_HALL_V + 4*GPIO_HALL_W;
            mcpwm0_bldc_hall_uvw_test(hall_motor_enable,sector_tmp,hall_motor_duty);
            break;
        
         default:
            break;
    }
	
	
	
}

