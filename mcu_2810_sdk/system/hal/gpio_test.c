////#include "gpio_hal.h"
////#include "gpio_typpe.h"
////#include "gpio_reg.h"
////#include <stdio.h>
//#include "gpio_test.h"
////uint16_t test_cnt;
////void gpio_output_test(void)//这里以P13作为输出测试代码
////{
////		gpio_cfg_t gpio_cfg = {0};

////    // 配置P13为输出模式
////    gpio_cfg.port        = GPIO_PORT_P0;          // 端口P1
////    gpio_cfg.pin         = GPIO_PIN_7;           // 
////    gpio_cfg.dir         = GPIO_DIR_OUTPUT;      // 输出方向
////    gpio_cfg.pull        = GPIO_PULL_DISABLE;    // 禁用上下拉
////    gpio_cfg.strength    = GPIO_STRENGTH_MID;   // 强驱动（确保输出能力）
////    gpio_cfg.input_mode  = GPIO_INPUT_MODE_CMOS; // CMOS输入模式
////    gpio_cfg.analog_mode = GPIO_ANALOG_DISABLE;  // 禁用模拟模式
////    gpio_cfg.alt_func    = GPIO_ALT_FUNC_0;      // 普通GPIO模式
////    gpio_cfg.int_cfg.callback = NULL;            // 禁用中断

////    // 初始化GPIO
////    gpio_init(&gpio_cfg);

////    // 输出高电平（3.3V，手册P43~44输出电平规范）
////    gpio_output_set(GPIO_PORT_P0, GPIO_PIN_7, GPIO_HIGH_LEVEL);
////	 
////	 
////	     // 配置P13为输出模式
////    gpio_cfg.port        = GPIO_PORT_P1;          // 端口P1
////    gpio_cfg.pin         = GPIO_PIN_5;           // PIN
////    gpio_cfg.dir         = GPIO_DIR_OUTPUT;      // 输出方向
////    gpio_cfg.pull        = GPIO_PULL_DISABLE;    // 禁用上下拉
////    gpio_cfg.strength    = GPIO_STRENGTH_MID;   // 强驱动（确保输出能力）
////    gpio_cfg.input_mode  = GPIO_INPUT_MODE_CMOS; // CMOS输入模式
////    gpio_cfg.analog_mode = GPIO_ANALOG_DISABLE;  // 禁用模拟模式
////    gpio_cfg.alt_func    = GPIO_ALT_FUNC_0;      // 普通GPIO模式
////    gpio_cfg.int_cfg.callback = NULL;            // 禁用中断

////    // 初始化GPIO
////    gpio_init(&gpio_cfg);

////    // 输出高电平（3.3V，手册P43~44输出电平规范）
////    gpio_output_set(GPIO_PORT_P1, GPIO_PIN_5, GPIO_HIGH_LEVEL);
////}


////void GPIO_input_test(void)
////{
////		gpio_cfg_t gpio_cfg = {0};
////		gpio_cfg.port        = GPIO_PORT_P1;
////    gpio_cfg.pin         = GPIO_PIN_3;
////    gpio_cfg.alt_func    = GPIO_ALT_FUNC_0;          // 普通 GPIO
////    gpio_cfg.analog_mode = GPIO_ANALOG_DISABLE;      // 数字模式
////    gpio_cfg.input_mode  = GPIO_INPUT_MODE_SCHMITT;  // 施密特输入（抗噪）
////    gpio_cfg.pull        =	GPIO_PULL_UP;           // 外部电路决定上下拉
////    gpio_cfg.strength    = GPIO_STRENGTH_LOW;        // 输入方向时无效
////    gpio_cfg.dir         = GPIO_DIR_INPUT;
////    gpio_init(&gpio_cfg);

////		
////		gpio_cfg.port        = GPIO_PORT_P1;          // 端口P1
////    gpio_cfg.pin         = GPIO_PIN_4;           // 引脚3（P13）
////    gpio_cfg.dir         = GPIO_DIR_OUTPUT;      // 输出方向
////    gpio_cfg.pull        = GPIO_PULL_DISABLE;    // 禁用上下拉
////    gpio_cfg.strength    = GPIO_STRENGTH_MID;   // 强驱动（确保输出能力）
////    gpio_cfg.input_mode  = GPIO_INPUT_MODE_CMOS; // CMOS输入模式
////    gpio_cfg.analog_mode = GPIO_ANALOG_DISABLE;  // 禁用模拟模式
////    gpio_cfg.alt_func    = GPIO_ALT_FUNC_0;      // 普通GPIO模式
////    gpio_cfg.int_cfg.callback = NULL;            // 禁用中断

////    // 初始化GPIO
////    gpio_init(&gpio_cfg);
////		
////		gpio_output_set(GPIO_PORT_P1, GPIO_PIN_4, GPIO_LOW_LEVEL);
////		
////		while(1)
////		{
////			if(gpio_input_get(GPIO_PORT_P1,GPIO_PIN_3)==0)
////			{
////				
////				gpio_output_set(GPIO_PORT_P1, GPIO_PIN_4, GPIO_HIGH_LEVEL);
////				test_cnt++;
////			}
////		
////		}

////}

////static void key_callback(gpio_port_e port, gpio_pin_e pin)
////{
////    
////    gpio_toggle(GPIO_PORT_P1, GPIO_PIN_4);
////   test_cnt++;
////}
////void IO_interputp_test(void)
////{
////		gpio_cfg_t gpio_cfg = {0};
////		gpio_cfg.port        = GPIO_PORT_P1;
////    gpio_cfg.pin         = GPIO_PIN_3;
////    gpio_cfg.alt_func    = GPIO_ALT_FUNC_0;          // 普通 GPIO
////    gpio_cfg.analog_mode = GPIO_ANALOG_DISABLE;      // 数字模式
////    gpio_cfg.input_mode  = GPIO_INPUT_MODE_SCHMITT;  // 施密特输入（抗噪）
////    gpio_cfg.pull        =	GPIO_PULL_UP;           // 外部电路决定上下拉
////    gpio_cfg.strength    = GPIO_STRENGTH_LOW;        // 输入方向时无效
////    gpio_cfg.dir         = GPIO_DIR_INPUT;
////		gpio_cfg.int_cfg.type = 1;//边沿触发
////		gpio_cfg.int_cfg.pol = 0;//下降沿触发
////		gpio_cfg.int_cfg.priority = 0;                       // 优先级（需单独配置 NVIC）
////    gpio_cfg .int_cfg.enable   = true;                   // 使能中断
////    gpio_cfg.int_cfg.callback =   key_callback;      		//调用回调函数
////		
////    gpio_init(&gpio_cfg);

////		gpio_cfg.port        = GPIO_PORT_P1;          // 端口P1
////    gpio_cfg.pin         = GPIO_PIN_4;           // 引脚3（P13）
////    gpio_cfg.dir         = GPIO_DIR_OUTPUT;      // 输出方向
////    gpio_cfg.pull        = GPIO_PULL_DISABLE;    // 禁用上下拉
////    gpio_cfg.strength    = GPIO_STRENGTH_MID;   // 强驱动（确保输出能力）
////    gpio_cfg.input_mode  = GPIO_INPUT_MODE_CMOS; // CMOS输入模式
////    gpio_cfg.analog_mode = GPIO_ANALOG_DISABLE;  // 禁用模拟模式
////    gpio_cfg.alt_func    = GPIO_ALT_FUNC_0;      // 普通GPIO模式
////    gpio_cfg.int_cfg.callback = NULL;            // 禁用中断

////    // 初始化GPIO
////    gpio_init(&gpio_cfg);
////}
//////P0中断服务函数
////void int1_Handler(void)
////{
////	 gpio_int_irq_handler(GPIO_PORT_P1);
////	
////}
