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
        for (uint32_t g = 0; g < sizeof(gpio_pin_groups)/sizeof(gpio_pin_groups[0]); g++)
        {
            const gpio_pin_group_t *group = &gpio_pin_groups[g];
            for (uint8_t pin = group->pin_start; pin <= group->pin_end; pin++)
            {
                char pin_str[4];
                char *argv[3];

                /* 构造 argv: argv[0]="cmd", argv[1]=port, argv[2]=pin */
                argv[0] = "cmd";
                argv[1] = (char *)group->port;
                /* 将引脚号转为字符串 */
                if (pin < 10) {
                    pin_str[0] = '0' + pin;
                    pin_str[1] = '\0';
                } else {
                    pin_str[0] = '0' + pin / 10;
                    pin_str[1] = '0' + pin % 10;
                    pin_str[2] = '\0';
                }
                argv[2] = pin_str;

                cmd_gpio_toggle(3, argv);
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
