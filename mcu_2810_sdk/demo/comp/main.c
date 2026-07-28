#include <stdio.h>
#include <string.h>
#include "CMSDK_CM0.h"
#include "system_CMSDK_CM0.h"
#include "core_cm0.h"
#include "Common.h"
#include "uart.h"
#include "gpio.h"
#include "qspi.h"
#include "wdt.h"
#include "timer.h"
#include "mdu.h"
#include <stdarg.h>
#include "INIT.h"

#include "para_define.h"

#include "hardinit.h"

#include "main.h"
#include "gpio_hal.h"
#include "comp_hal.h"
#define GPIO_MODE 3
#define OUTPUT_MODE 1
#define INPUT_MODE 	2
#define TOGGLE_MODE 3
#define IO_INTERUPT_MODE 4
void GPIO_Demo(void);
void delay_ms(uint32_t ms)
{
    uint32_t load = 24000; // 24MHz / 1000 = 24000
    // 配置SysTick
    SysTick->LOAD = load - 1; // 因为计数到0才触发，所以减1
    SysTick->VAL = 0;         // 清当前值
    SysTick->CTRL = (1 << 2) | (1 << 0); // 使用内部时钟，使能计数，不使能中断
    for (uint32_t i = 0; i < ms; i++) {
						while (!(SysTick->CTRL & (1 << 16))); // 等待COUNTFLAG置位
    }
    SysTick->CTRL = 0; // 关闭SysTick
}
uint16_t test_cnt;
void GPIO_Output_Test(void)//这里以P13作为输出测试代码
{
		gpio_cfg_t gpio_cfg ;

    // 配置P13为输出模式
    gpio_cfg.port        = GPIO_PORT_P0;          // 端口
    gpio_cfg.pin         = GPIO_PIN_7;           // 
    gpio_cfg.dir         = GPIO_DIR_OUTPUT;      // 输出方向
    gpio_cfg.pull        = GPIO_PULL_DISABLE;    // 禁用上下拉
    gpio_cfg.strength    = GPIO_STRENGTH_MID;   // 强驱动（确保输出能力）
    gpio_cfg.input_mode  = GPIO_INPUT_MODE_CMOS; // CMOS输入模式
    gpio_cfg.analog_mode = GPIO_ANALOG_DISABLE;  // 禁用模拟模式
    gpio_cfg.alt_func    = GPIO_ALT_FUNC_0;      // 普通GPIO模式
    gpio_cfg.int_cfg.callback = NULL;            // 禁用中断

    // 初始化GPIO
    gpio_init(&gpio_cfg);

    // 输出高电平（3.3V，手册P43~44输出电平规范）
    gpio_output_set(GPIO_PORT_P0, GPIO_PIN_7, GPIO_HIGH_LEVEL);
	 
	 
	  // 配置P13为输出模式
    gpio_cfg.port        = GPIO_PORT_P1;          // 端口
    gpio_cfg.pin         = GPIO_PIN_5;           // PIN
    gpio_cfg.dir         = GPIO_DIR_OUTPUT;      // 输出方向
    gpio_cfg.pull        = GPIO_PULL_DISABLE;    // 禁用上下拉
    gpio_cfg.strength    = GPIO_STRENGTH_MID;   // 强驱动（确保输出能力）
    gpio_cfg.input_mode  = GPIO_INPUT_MODE_CMOS; // CMOS输入模式
    gpio_cfg.analog_mode = GPIO_ANALOG_DISABLE;  // 禁用模拟模式
    gpio_cfg.alt_func    = GPIO_ALT_FUNC_0;      // 普通GPIO模式
    gpio_cfg.int_cfg.callback = NULL;            // 禁用中断

    // 初始化GPIO
    gpio_init(&gpio_cfg);

    // 输出高电平（3.3V，手册P43~44输出电平规范）
    gpio_output_set(GPIO_PORT_P1, GPIO_PIN_5, GPIO_HIGH_LEVEL);
}


void GPIO_Input_Test(void)
{
		gpio_cfg_t gpio_cfg ;
		gpio_cfg.port        = GPIO_PORT_P1;
    gpio_cfg.pin         = GPIO_PIN_3;
    gpio_cfg.alt_func    = GPIO_ALT_FUNC_0;          // 普通 GPIO
    gpio_cfg.analog_mode = GPIO_ANALOG_DISABLE;      // 数字模式
    gpio_cfg.input_mode  = GPIO_INPUT_MODE_SCHMITT;  // 施密特输入（抗噪）
    gpio_cfg.pull        =	GPIO_PULL_UP;           // 外部电路决定上下拉
    gpio_cfg.strength    = GPIO_STRENGTH_LOW;        // 输入方向时无效
    gpio_cfg.dir         = GPIO_DIR_INPUT;
    gpio_init(&gpio_cfg);

		
		gpio_cfg.port        = GPIO_PORT_P0;          // 端口P1
    gpio_cfg.pin         = GPIO_PIN_7;           // 引脚3（P13）
    gpio_cfg.dir         = GPIO_DIR_OUTPUT;      // 输出方向
    gpio_cfg.pull        = GPIO_PULL_DISABLE;    // 禁用上下拉
    gpio_cfg.strength    = GPIO_STRENGTH_MID;   // 强驱动（确保输出能力）
    gpio_cfg.input_mode  = GPIO_INPUT_MODE_CMOS; // CMOS输入模式
    gpio_cfg.analog_mode = GPIO_ANALOG_DISABLE;  // 禁用模拟模式
    gpio_cfg.alt_func    = GPIO_ALT_FUNC_0;      // 普通GPIO模式
    gpio_cfg.int_cfg.callback = NULL;            // 禁用中断

    // 初始化GPIO
    gpio_init(&gpio_cfg);
		
		gpio_output_set(GPIO_PORT_P0, GPIO_PIN_7, GPIO_LOW_LEVEL);
		
		while(1)
		{
			if(gpio_input_get(GPIO_PORT_P1,GPIO_PIN_3)==0)
			{
				
				gpio_toggle(GPIO_PORT_P0, GPIO_PIN_7);
				delay_ms(1000);
				gpio_toggle(GPIO_PORT_P0, GPIO_PIN_7);
				delay_ms(1000);
				test_cnt++;
			}
		
		}

}

static void key_callback(gpio_port_e port, gpio_pin_e pin)
{
    
    gpio_toggle(GPIO_PORT_P0, GPIO_PIN_7);
   test_cnt++;
}
void IO_interupt_Test(void)
{
		gpio_cfg_t gpio_cfg;
		gpio_cfg.port        = GPIO_PORT_P1;
    gpio_cfg.pin         = GPIO_PIN_3;
    gpio_cfg.alt_func    = GPIO_ALT_FUNC_0;          // 普通 GPIO
    gpio_cfg.analog_mode = GPIO_ANALOG_DISABLE;      // 数字模式
    gpio_cfg.input_mode  = GPIO_INPUT_MODE_SCHMITT;  // 施密特输入（抗噪）
    gpio_cfg.pull        =	GPIO_PULL_UP;           // 外部电路决定上下拉
    gpio_cfg.strength    = GPIO_STRENGTH_LOW;        // 输入方向时无效
    gpio_cfg.dir         = GPIO_DIR_INPUT;
		gpio_cfg.int_cfg.type = 1;//边沿触发
		gpio_cfg.int_cfg.pol = 0;//下降沿触发
		gpio_cfg.int_cfg.priority = 0;                       // 优先级（需单独配置 NVIC）
    gpio_cfg .int_cfg.enable   = true;                   // 使能中断
    gpio_cfg.int_cfg.callback =   key_callback;      		//调用回调函数
		
    gpio_init(&gpio_cfg);

		gpio_cfg.port        = GPIO_PORT_P0;          // 端口P1
    gpio_cfg.pin         = GPIO_PIN_7;           // 引脚3（P13）
    gpio_cfg.dir         = GPIO_DIR_OUTPUT;      // 输出方向
    gpio_cfg.pull        = GPIO_PULL_DOWN;    // 禁用上下拉
    gpio_cfg.strength    = GPIO_STRENGTH_MID;   // 强驱动（确保输出能力）
    gpio_cfg.input_mode  = GPIO_INPUT_MODE_CMOS; // CMOS输入模式
    gpio_cfg.analog_mode = GPIO_ANALOG_DISABLE;  // 禁用模拟模式
    gpio_cfg.alt_func    = GPIO_ALT_FUNC_0;      // 普通GPIO模式
    gpio_cfg.int_cfg.callback = NULL;            // 禁用中断

    // 初始化GPIO
    gpio_init(&gpio_cfg);
}
//P0中断服务函数
void int1_Handler(void)
{
	 gpio_int_irq_handler(GPIO_PORT_P1);
	

}


void GPIO_Toggle_Test(void)
{
		gpio_cfg_t gpio_cfg;

    
    gpio_cfg.port        = GPIO_PORT_P0;          // 端口P1
    gpio_cfg.pin         = GPIO_PIN_7;           // 
    gpio_cfg.dir         = GPIO_DIR_OUTPUT;      // 输出方向
    gpio_cfg.pull        = GPIO_PULL_DISABLE;    // 禁用上下拉
    gpio_cfg.strength    = GPIO_STRENGTH_MID;   // 强驱动（确保输出能力）
    gpio_cfg.input_mode  = GPIO_INPUT_MODE_CMOS; // CMOS输入模式
    gpio_cfg.analog_mode = GPIO_ANALOG_DISABLE;  // 禁用模拟模式
    gpio_cfg.alt_func    = GPIO_ALT_FUNC_0;      // 普通GPIO模式
    gpio_cfg.int_cfg.callback = NULL;            // 禁用中断

   
    gpio_init(&gpio_cfg);

    gpio_output_set(GPIO_PORT_P0, GPIO_PIN_7, GPIO_LOW_LEVEL);
	 
    while (1)
    {
			gpio_toggle(GPIO_PORT_P0, GPIO_PIN_7);
			delay_ms(1000);
			gpio_toggle(GPIO_PORT_P0, GPIO_PIN_7);
			delay_ms(1000);
    } 


}

//------------------------------------------------
void GPIO_Demo(void)
{
	
 switch (GPIO_MODE)
	 {
		case OUTPUT_MODE:
				GPIO_Output_Test();
				break;
		case INPUT_MODE:
				GPIO_Input_Test();
				break;
		case TOGGLE_MODE:
				GPIO_Toggle_Test();
				break;
		case IO_INTERUPT_MODE:
				IO_interupt_Test();
				NVIC_EnableIRQ(int1_irq);
				NVIC_SetPriority(int1_irq,0);
				break;
		default:
				// 默认使用 TIM0
			 GPIO_Output_Test();
				break;
		}
}
int main(void)
{
	/* CMP 测试: 选择 CMP0 配置并输出到 P13 */
  comp_test(2);

  soft_delay_1ms(1000);

  PMU->wr_key = 0x55aaaa55;
  PMU->cfg_normal = 0<<0 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 0<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10; // DRV3 V1.4

  //    System_Init();
	GPIO_Demo();
 
  while (1)
  {
  } 
} 
