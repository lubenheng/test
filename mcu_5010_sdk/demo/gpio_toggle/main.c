/****************************************************************************/
/** \file main.c
**
**  GPIO demo for ODS_5010 (Cortex-M0+)
**  PA0-PA15,PB0-PB15,PC0-PC8
*****************************************************************************/
#include "platform_common.h"
#include "test.h"
#include "gpio_hal.h"
#include "clock.h"
#include "uart_hal.h"
#include "uart_hal_cfg.h"
#include "test.h"

#include "cmdline_internal.h"



/* GPIO引脚配置表: 端口名, 起始引脚, 结束引脚 */
typedef struct {
    const char *port;
    uint8_t pin_start;
    uint8_t pin_end;
} gpio_pin_group_t;

static const gpio_pin_group_t gpio_pin_groups[] = {
    {"A",  2, 14},   /* PA2 ~ PA14 */
    {"B",  1, 15},   /* PB1 ~ PB15 */
    {"C",  0,  6},   /* PC0 ~ PC6  */
};

int main(void)
{
    delay_1us(200000);

    flash_clk_enable();
    gpio_clk_enable();

    while(1)
    {
        /* 所有GPIO引脚同时翻转 */
        for (uint32_t g = 0; g < sizeof(gpio_pin_groups)/sizeof(gpio_pin_groups[0]); g++)
        {
            const gpio_pin_group_t *group = &gpio_pin_groups[g];
            for (uint8_t pin = group->pin_start; pin <= group->pin_end; pin++)
            {
                gpio_toggle((gpio_port_e)(GPIO_A + g), (gpio_pin_e)(GPIO_PIN_0 + pin));
            }
        }
    }
}

/*
        gpio_output_set(GPIO_C, GPIO_PIN_6, GPIO_HIGH_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_7, GPIO_HIGH_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_8, GPIO_HIGH_LEVEL);
        delay_1us(100);

        gpio_output_set(GPIO_C, GPIO_PIN_6, GPIO_LOW_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_7, GPIO_LOW_LEVEL);
        gpio_output_set(GPIO_C, GPIO_PIN_8, GPIO_LOW_LEVEL);
        delay_1us(100);

*/
