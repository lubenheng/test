#include "tim_cmdline.h"
#include "utility.h" 
#include "softinit.h"
#include "tim_hal.h"
// ==================== TIM0 �жϴ��� ====================
void tim0_callback(tim_inst_e instance)
{
    if (instance == TIM_INST_0) 
		{
        gpio_toggle(GPIO_PORT_P1, GPIO_PIN_7);
    }
}
void tim5_callback(tim_inst_e instance)
{
    if (instance == TIM_INST_5) 
		{
        gpio_toggle(GPIO_PORT_P1, GPIO_PIN_7);
    }
}
void tim6_callback(tim_inst_e instance)
{
    if (instance == TIM_INST_6) 
		{
        gpio_toggle(GPIO_PORT_P1, GPIO_PIN_7);
    }
}
void tim3_callback(tim_inst_e instance)
{
    if (instance == TIM_INST_3) 
		{
        gpio_toggle(GPIO_PORT_P1, GPIO_PIN_7);
    }
}
void tim0_Handler(void)
{
    tim_irq_handler(TIM_INST_0);
		 
}

void tim3_Handler(void)
{
//    gpio_toggle(GPIO_PORT_P1, GPIO_PIN_7);
		tim_irq_handler(TIM_INST_3); 
}

void tim5_Handler(void)
{
    tim_irq_handler(TIM_INST_5);
		 
}
void tim6_Handler(void)
{
    tim_irq_handler(TIM_INST_6);
		 
}

int cmd_timx_toggle_test(int argc, char *argv[])
{
	uint8_t case2 = get_arg_ulong(argv[0]);
	uint8_t Timx = get_arg_ulong(argv[1]);
	uint32_t Periodx = get_arg_ulong(argv[2]); 	
  gpio_test_pin();
	switch(Timx)
	{
		case 0:tim0_init(Periodx);break;
		case 3:tim3_init(Periodx);break;
		case 5:tim5_init(Periodx);break;
		case 6:tim6_init(Periodx);break;
	}
	
	
	
	
	INFO_e("Tim%d OK\r\n",Timx);
	 
	return 0;	
}

void gpio_test_pin(void)
{
	 
//	PMU->wr_key = 0x55AAAA55;

//	PMU->cfg_normal = 0<<0 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 0<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10; //DRV3 V1.4
//	static uint8_t mode = 3;
	// ��ʼ�� GPIO P1.7 ��Ϊ��������ڹ۲��жϷ�ת
	gpio_cfg_t gpio_cfg;
	gpio_cfg.port        = GPIO_PORT_P1;
	gpio_cfg.pin         = GPIO_PIN_7;
	gpio_cfg.dir         = GPIO_DIR_OUTPUT;
	gpio_cfg.pull        = GPIO_PULL_DISABLE;
	gpio_cfg.strength    = GPIO_STRENGTH_MID;
	gpio_cfg.input_mode  = GPIO_INPUT_MODE_CMOS;
	gpio_cfg.analog_mode = GPIO_ANALOG_DISABLE;
	gpio_cfg.alt_func    = GPIO_ALT_FUNC_0;
	gpio_cfg.int_cfg.callback = NULL;
	gpio_init(&gpio_cfg);
	gpio_output_set(GPIO_PORT_P1, GPIO_PIN_7, GPIO_HIGH_LEVEL);
}

void tim0_init(uint32_t periodx) 
{
    tim_base_handle_t htim;
    htim.instance   = TIM_INST_0;
    htim.period     = periodx - 1;              // 24Mhz������Ҫ����Ƶ��
    htim.enable_update_it  = true;
    htim.enable_compare_it = false;

    tim_base_init(&htim);
    tim_register_callback(TIM_INST_0, tim0_callback);
    tim_base_start_it(TIM_INST_0, TIM_IT_UPDATE);

    NVIC_EnableIRQ(tim0_irq);
    NVIC_SetPriority(tim0_irq, 3);
}


void tim3_init(uint32_t periodx)   //1Khz
{
    tim_base_handle_t htim;
    htim.instance   = TIM_INST_3;
    htim.period     = periodx-1;
//    htim.prescaler  = 0;//24
    htim.clock_div  = TIM_CLK_DIV_1;   //48mhz
    htim.mode       = TIM_MODE_Ouput_ALIGNED;
    htim.opm        = TIM_OPM_DISABLE;
    htim.ocm        = TIM_OCM_DISABLE;
    htim.enable_update_it  = true;
    htim.enable_compare_it = false;

    tim_base_init(&htim);
    tim_register_callback(TIM_INST_3, tim3_callback);
    tim_base_start_it(TIM_INST_3, TIM_IT_UPDATE);

    NVIC_EnableIRQ(tim3_irq);
    NVIC_SetPriority(tim3_irq, 3);
}

void tim5_init(uint32_t periodx) 
{
    tim_base_handle_t htim;
    htim.instance   = TIM_INST_5;
    htim.period     = periodx - 1;              // 24Mhz������Ҫ����Ƶ��
    htim.enable_update_it  = true;
    htim.enable_compare_it = false;

    tim_base_init(&htim);
    tim_register_callback(TIM_INST_5, tim5_callback);
    tim_base_start_it(TIM_INST_5, TIM_IT_UPDATE);

    NVIC_EnableIRQ(tim5_irq);
    NVIC_SetPriority(tim5_irq, 3);
}

void tim6_init(uint32_t periodx) 
{
    tim_base_handle_t htim;
    htim.instance   = TIM_INST_6;
    htim.period     = periodx - 1;              // 24Mhz������Ҫ����Ƶ��
    htim.enable_update_it  = true;
    htim.enable_compare_it = false;

    tim_base_init(&htim);
    tim_register_callback(TIM_INST_6, tim6_callback);
    tim_base_start_it(TIM_INST_6, TIM_IT_UPDATE);

    NVIC_EnableIRQ(tim6_irq);
    NVIC_SetPriority(tim6_irq, 3);
}
