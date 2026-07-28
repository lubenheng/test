#include <stdio.h>
#include <string.h>
#include "CMSDK_CM0.h"
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
#include "motor_control.h"
#include "para_define.h"

#include "hardinit.h"
#include "gpio_cmdline.h"
#include "main.h"
#include "gpio_hal.h"
#include "adc_hal.h"
 
#include "cmdline_internal.h"
#include "interrupt_cmdline.h"
#include "pmu.h"
#include "softinit.h" 
int a=0,b=0,c=0,d=0,e=0,f=0;



void GPIO_Toggle_Test(void)
{
	gpio_cfg_t gpio_cfg;
	gpio_cfg_t gpio_cfg1;
    
    gpio_cfg.port        = GPIO_PORT_P0;          // 端口P1
    gpio_cfg.pin         = GPIO_PIN_3;           // 
    gpio_cfg.dir         = GPIO_DIR_OUTPUT;      // 输出方向
    gpio_cfg.pull        = GPIO_PULL_DISABLE;    // 禁用上下拉
    gpio_cfg.strength    = GPIO_STRENGTH_MAX;   // 强驱动（确保输出能力）
	                      //GPIO_STRENGTH_MAX
	                      //GPIO_STRENGTH_LOW
    gpio_cfg.input_mode  = GPIO_INPUT_MODE_CMOS; // CMOS输入模式
    gpio_cfg.analog_mode = GPIO_ANALOG_DISABLE;  // 禁用模拟模式
    gpio_cfg.alt_func    = GPIO_ALT_FUNC_0;      // 普通GPIO模式
    gpio_cfg.int_cfg.callback = NULL;            // 禁用中断

   
    gpio_init(&gpio_cfg);

    gpio_output_set(GPIO_PORT_P0, GPIO_PIN_3, GPIO_LOW_LEVEL);
	/////////////////////////////////////////////////////////////////////////
	gpio_cfg1.port        = GPIO_PORT_P0;          // 端口P1
    gpio_cfg1.pin         = GPIO_PIN_6;           // 
    gpio_cfg1.dir         = GPIO_DIR_OUTPUT;      // 输出方向
    gpio_cfg1.pull        = GPIO_PULL_DISABLE;    // 禁用上下拉
    gpio_cfg1.strength    = GPIO_STRENGTH_LOW;   // 强驱动（确保输出能力）
	                      //GPIO_STRENGTH_MAX
	                      //GPIO_STRENGTH_LOW
    gpio_cfg1.input_mode  = GPIO_INPUT_MODE_CMOS; // CMOS输入模式
    gpio_cfg1.analog_mode = GPIO_ANALOG_DISABLE;  // 禁用模拟模式
    gpio_cfg1.alt_func    = GPIO_ALT_FUNC_0;      // 普通GPIO模式
    gpio_cfg1.int_cfg.callback = NULL;            // 禁用中断

   
    gpio_init(&gpio_cfg1);

    gpio_output_set(GPIO_PORT_P0, GPIO_PIN_6, GPIO_LOW_LEVEL);
	 
    while (1)
    {
			gpio_toggle(GPIO_PORT_P0, GPIO_PIN_3);
			delay_ms(1000);
			gpio_toggle(GPIO_PORT_P0, GPIO_PIN_3);
			delay_ms(1000);
			gpio_toggle(GPIO_PORT_P0, GPIO_PIN_6);
			delay_ms(1000);
			gpio_toggle(GPIO_PORT_P0, GPIO_PIN_6);
			delay_ms(1000);
    } 


}
void GPIO_OUTPUT_Test(void)
{
	gpio_cfg_t gpio_cfg;
	
    
    gpio_cfg.port        = GPIO_PORT_P2;          // 端口P1
    gpio_cfg.pin         = GPIO_PIN_0;           // 
    gpio_cfg.dir         = GPIO_DIR_OUTPUT;      // 输出方向
    gpio_cfg.pull        = GPIO_PULL_DISABLE;    // 禁用上下拉
    gpio_cfg.strength    = GPIO_STRENGTH_LOW;   // 强驱动（确保输出能力）
	                      //GPIO_STRENGTH_MAX
	                      //GPIO_STRENGTH_LOW
    gpio_cfg.input_mode  = GPIO_INPUT_MODE_CMOS; // CMOS输入模式
    gpio_cfg.analog_mode = GPIO_ANALOG_DISABLE;  // 禁用模拟模式
    gpio_cfg.alt_func    = GPIO_ALT_FUNC_0;      // 普通GPIO模式
    gpio_cfg.int_cfg.callback = NULL;            // 禁用中断

   
    gpio_init(&gpio_cfg);

    gpio_output_set(GPIO_PORT_P2, GPIO_PIN_0, GPIO_HIGH_LEVEL);
	
}

void GPIO_UP_DOWN_Test(void)
{
	gpio_cfg_t gpio_cfg ;
	gpio_cfg.port        = GPIO_PORT_P0;
    gpio_cfg.pin         = GPIO_PIN_3;
    gpio_cfg.alt_func    = GPIO_ALT_FUNC_0;          // 普通 GPIO
    gpio_cfg.analog_mode = GPIO_ANALOG_DISABLE;      // 数字模式
    gpio_cfg.input_mode  = GPIO_INPUT_MODE_SCHMITT;  // 施密特输入（抗噪）
	
	////////
    gpio_cfg.pull        =	GPIO_PULL_DOWN;           // 外部电路决定上下拉
//	GPIO_PULL_DISABLE = 0,  // 禁用上下拉
//    GPIO_PULL_UP,           // 上拉
//    GPIO_PULL_DOWN,         // 下拉
//    GPIO_PULL_MAX           // 上下拉配置最大值
	
    gpio_cfg.strength    = GPIO_STRENGTH_LOW;        // 输入方向时无效
    gpio_cfg.dir         = GPIO_DIR_INPUT;
    gpio_init(&gpio_cfg);

    // 初始化GPIO
    gpio_init(&gpio_cfg);
		
	

}


void GPIO_Input_Test(void)
{
	gpio_cfg_t gpio_cfg ;
	gpio_cfg.port        = GPIO_PORT_P2;
    gpio_cfg.pin         = GPIO_PIN_0;
    gpio_cfg.alt_func    = GPIO_ALT_FUNC_0;          // 普通 GPIO
    gpio_cfg.analog_mode = GPIO_ANALOG_DISABLE;      // 数字模式
    gpio_cfg.input_mode  = GPIO_INPUT_MODE_SCHMITT;  // 施密特输入（抗噪）
    gpio_cfg.pull        =	GPIO_PULL_DOWN;           // 外部电路决定上下拉
    gpio_cfg.strength    = GPIO_STRENGTH_LOW;        // 输入方向时无效
    gpio_cfg.dir         = GPIO_DIR_INPUT;
    gpio_init(&gpio_cfg);

		
	gpio_cfg.port        = GPIO_PORT_P0;          // 端口P0
    gpio_cfg.pin         = GPIO_PIN_3;           // 引脚3（P03）
    gpio_cfg.dir         = GPIO_DIR_OUTPUT;      // 输出方向
    gpio_cfg.pull        = GPIO_PULL_DISABLE;    // 禁用上下拉
    gpio_cfg.strength    = GPIO_STRENGTH_MID;   // 强驱动（确保输出能力）
    gpio_cfg.input_mode  = GPIO_INPUT_MODE_SCHMITT; // CMOS输入模式
    gpio_cfg.analog_mode = GPIO_ANALOG_DISABLE;  // 禁用模拟模式
    gpio_cfg.alt_func    = GPIO_ALT_FUNC_0;      // 普通GPIO模式
    gpio_cfg.int_cfg.callback = NULL;            // 禁用中断

    // 初始化GPIO
    gpio_init(&gpio_cfg);
		
		gpio_output_set(GPIO_PORT_P0, GPIO_PIN_3, GPIO_LOW_LEVEL);
		
		while(1)
		{
			if(gpio_input_get(GPIO_PORT_P1,GPIO_PIN_3)==0)
			{
				
				gpio_output_set(GPIO_PORT_P0, GPIO_PIN_3, GPIO_LOW_LEVEL);
				
			}
			if(gpio_input_get(GPIO_PORT_P1,GPIO_PIN_3)==1)
			{
				
				gpio_output_set(GPIO_PORT_P0, GPIO_PIN_3, GPIO_HIGH_LEVEL);
				
			}
	
		}

}


int debug_flag=1;
int main(void)
{
	delay_ms(1000);
	
	if(debug_flag==1)
	{
	PMU->wr_key = 0x55aaaa55;
	PMU->cfg_normal = 0<<0 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 0<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10; //DRV3 V1.4

	//GPIO_Input_Test();
	GPIO_Toggle_Test();
	//GPIO_UP_DOWN_Test();
	//GPIO_OUTPUT_Test();
	while (1)
	{ 

	}
}
	}


