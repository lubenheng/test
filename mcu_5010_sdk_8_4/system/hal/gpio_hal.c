#include "platform_common.h"

#include "gpio_reg.h"
#include "device_resource.h"
#include "gpio_hal.h"
#include "gpio.h"


#define GPIO_NVIC_DISABLE   0UL

static PORT_TypeDef * const GPIO_PORTS[GPIO_PORT_MAX] = GPIO_BASE_ADDRS;
static IRQn_Type gpio_irqn[GPIO_PORT_MAX] = GPIO_IRQS;
/** gpio interrupt handle for each port*/
static GPIO_INT_HANDLE g_gpio_int_handle[GPIO_PORT_MAX] = {NULL, NULL, NULL};

//static const gpio_pin_e a_gpio_pin_arr[] = { \
//  GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7, \
//  GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10, GPIO_PIN_11, GPIO_PIN_12, GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15, \
//};


/**
 *@brief config all gpio as default
 *@param[in] none
 *@return    none
*/
//void gpio_init_to_default(void)
//{
    //gpio_drv_init_to_default(GPIO_PORTS[GPIO_A]);
    //gpio_drv_init_to_default(GPIO_PORTS[GPIO_B]);
    //gpio_drv_init_to_default(GPIO_PORTS[GPIO_C]);
    //gpio_drv_init_to_default(GPIO_PORTS[GPIO_D]);
//    //gpio_drv_init_to_default(GPIO_PORTS[GPIO_E]);
//}

/**
 *@brief get gpio port address
 *@param[in] gpio_port_sel
 *@return    gpio_ptr: point to gpio_port_sel address
*/
static inline PORT_TypeDef *gpio_port_addr_get(gpio_port_e gpio_port_sel)
{
    DEV_ASSERT(gpio_port_sel < GPIO_PORT_MAX);

    return GPIO_PORTS[gpio_port_sel];
}

/**
 *@brief config gpio as input or output
 *@param[in] gpio_port: one of gpio_port_e
 *@param[in] gpio_pin: gpio pin number
 *@param[in] gpio_dir: output or input
 *@return    none
*/
void gpio_direction_cfg(gpio_port_e gpio_port, gpio_pin_e gpio_pin, gpio_dir_e gpio_dir)
{
    PORT_TypeDef *gpio_ptr = gpio_port_addr_get(gpio_port);

    gpio_drv_direction_cfg(gpio_ptr, gpio_pin, gpio_dir);
}

/**
 *@brief config gpio output mode as push_pull, open_drain or open_source
 *@param[in] gpio_port: one of gpio_port_e
 *@param[in] gpio_pin: gpio pin number
 *@param[in] gpio_output_mode: push_pull, open_drain or open_source
 *@return    none
*/
//void gpio_output_mode_cfg(gpio_port_e gpio_port, gpio_pin_e gpio_pin, gpio_output_mode_e gpio_output_mode)
//{
//    PORT_TypeDef *gpio_ptr = gpio_port_addr_get(gpio_port);

//    gpio_drv_output_mode_cfg(gpio_ptr, gpio_pin, gpio_output_mode);
//}

/**
 *@brief config gpio input mode as schmitt(hysteresis) or cmos(non-hysteresis)
 *@param[in] gpio_port: one of gpio_port_e
 *@param[in] gpio_pin: gpio pin number
 *@param[in] gpio_input_mode: schmitt(hysteresis) or cmos(non-hysteresis)
 *@return    none
*/
//void gpio_input_mode_cfg(gpio_port_e gpio_port, gpio_pin_e gpio_pin, gpio_input_mode_e gpio_input_mode)
//{
//    PORT_TypeDef *gpio_ptr = gpio_port_addr_get(gpio_port);

//    gpio_drv_input_mode_cfg(gpio_ptr, gpio_pin, gpio_input_mode);
//}

/**
 *@brief gpio pull up or pull down config
 *@param[in] gpio_port: one of gpio_port_e
 *@param[in] gpio_pin: gpio pin number
 *@param[in] pull_cfg: configure to pull up or pull down
 *@return    none
*/
void gpio_pull_cfg(gpio_port_e gpio_port, gpio_pin_e gpio_pin, gpio_pull_cfg_e pull_cfg)
{
//    PORT_TypeDef *gpio_ptr = gpio_port_addr_get(gpio_port);
//    
//    gpio_drv_pull_cfg(gpio_ptr, gpio_pin, pull_cfg);
}

/**
 *@brief set gpio output voltage high or low
 *@param[in] gpio_port: one of gpio_port_e
 *@param[in] gpio_pin: gpio pin number
 *@param[in] level: configure gpio pin output high or low
 *@return    none
*/
void gpio_output_set(gpio_port_e gpio_port, gpio_pin_e gpio_pin, gpio_level_e level)
{
   PORT_TypeDef *gpio_ptr = gpio_port_addr_get(gpio_port);
   
   gpio_drv_output_set(gpio_ptr, gpio_pin, level);
}

/**
 *@brief get gpio dataout bit 
 *@param[in] gpio_port: one of gpio_port_e
 *@param[in] gpio_pin: gpio pin number
 *@return    gpio dataout bit
*/
//bool gpio_output_get(gpio_port_e gpio_port, gpio_pin_e gpio_pin)
//{
//    PORT_TypeDef *gpio_ptr = gpio_port_addr_get(gpio_port);
//    
//    return gpio_drv_output_get(gpio_ptr, gpio_pin);
//}

/**
 *@brief get gpio input voltage high or low
 *@param[in] gpio_port: one of gpio_port_e
 *@param[in] gpio_pin: gpio pin number
 *@return    input level
*/
//gpio_level_e gpio_input_get(gpio_port_e gpio_port, gpio_pin_e gpio_pin)
//{
//    PORT_TypeDef *gpio_ptr = gpio_port_addr_get(gpio_port);
//    
//    return (gpio_drv_input_get(gpio_ptr, gpio_pin));
//}

/**
 *@brief gpio handle function
 *@param[in] int_port: one of gpio_port_e
 *@return    none
*/
//static void gpio_isr(gpio_port_e int_port)
//{
//    DEV_ASSERT(int_port < GPIO_PORT_MAX);

//    volatile bool int_flag;
//    
//    for (uint32_t pin = GPIO_PIN_0; pin < GPIO_PIN_NUM_MAX; pin++)
//    {
//        int_flag = gpio_drv_interrupt_get_flag(GPIO_PORTS[int_port], a_gpio_pin_arr[pin]);
//        
//        gpio_drv_interrupt_flag_clear(GPIO_PORTS[int_port], a_gpio_pin_arr[pin]);
//        
//        if (int_flag&&(g_gpio_int_handle[int_port] != NULL))/*if not enable, status won't be set*/
//        {
//            g_gpio_int_handle[int_port](pin);/*send status info*/
//        }
//    }
//}

/**
 *@brief all of gpio ports interrupt handle function
 *@param[in] none
 *@return    none
*/
//void GPIO_ORed_IRQHandler(void)
//{
//    gpio_isr(GPIO_A);
//    gpio_isr(GPIO_B);
//    gpio_isr(GPIO_C);
//    gpio_isr(GPIO_D);
//    gpio_isr(GPIO_E);
//}

/**
 *@brief gpio interrupt config
 *@param[in] gpio_port: one of gpio_port_e
 *@param[in] gpio_pin: gpio pin number
 *@param[in] gpio_interrupt_cfg_info: point to interrupt config information
 *@return    none
*/
void gpio_interrupt_cfg(gpio_port_e gpio_port, gpio_pin_e gpio_pin, gpio_interrupt_cfg_info_t *gpio_interrupt_cfg_info)
{
   DEV_ASSERT(NULL != gpio_interrupt_cfg_info);

   PORT_TypeDef *gpio_ptr = gpio_port_addr_get(gpio_port);
   
   DEV_ASSERT(NULL != gpio_ptr);
   
   gpio_direction_cfg(gpio_port, gpio_pin, GPIO_DIR_INPUT);

   g_gpio_int_handle[gpio_port] = gpio_interrupt_cfg_info->int_handle;

   gpio_drv_interrupt_cfg(gpio_ptr, gpio_pin, gpio_interrupt_cfg_info);
   
   if (GPIO_NVIC_DISABLE == NVIC_GetEnableIRQ(gpio_irqn[gpio_port]))
   {
       NVIC_ClearPendingIRQ(gpio_irqn[gpio_port]);
       NVIC_EnableIRQ(gpio_irqn[gpio_port]);
       NVIC_SetPriority(gpio_irqn[gpio_port],gpio_interrupt_cfg_info->priority);
   }
}

/**
 *@brief get gpio interrupt flag
 *@param[in] gpio_port: one of gpio_port_e
 *@param[in] gpio_pin: gpio pin number
 *@return    interrupt status exclude other gpios
*/
//bool gpio_interrupt_get_flag(gpio_port_e gpio_port, gpio_pin_e gpio_pin)
//{
//    const PORT_TypeDef *gpio_ptr = gpio_port_addr_get(gpio_port);
//    
//    DEV_ASSERT(NULL != gpio_ptr);
//    
//    return (gpio_drv_interrupt_get_flag(gpio_ptr, gpio_pin));
//}

/**
 *@brief disable interrupt for one gpio
 *@param[in] gpio_port: one of gpio_port_e
 *@param[in] gpio_pin: gpio pin number
 *@return    none
*/
void gpio_interrupt_disable(gpio_port_e gpio_port, gpio_pin_e gpio_pin)
{
//    PORT_TypeDef *gpio_ptr = gpio_port_addr_get(gpio_port);
//    bool is_port_inten_all_disable;
   
//    DEV_ASSERT(NULL != gpio_ptr);
   
//    is_port_inten_all_disable = gpio_drv_interrupt_disable(gpio_ptr, gpio_pin);

//    if (is_port_inten_all_disable == true)
//    {
//        NVIC_DisableIRQ(gpio_irqn[gpio_port]);
//        NVIC_ClearPendingIRQ(gpio_irqn[gpio_port]);
//    }
}

/**
 *@brief clear gpio interrupt flag
 *@param[in] gpio_port: one of gpio_port_e
 *@param[in] gpio_pin: gpio pin number
 *@return    none
*/
//void gpio_interrupt_flag_clear(gpio_port_e gpio_port, gpio_pin_e gpio_pin)
//{
//    PORT_TypeDef *gpio_ptr = gpio_port_addr_get(gpio_port);

//    gpio_drv_interrupt_flag_clear(gpio_ptr, gpio_pin);
//}

/**
 *@brief get gpio interrupt type
 *@param[in] gpio_port: one of gpio_port_e
 *@param[in] gpio_pin: gpio pin number
 *@return    gpio interrupt type
*/
//gpio_interrupt_type_e gpio_interrupt_type_get(gpio_port_e gpio_port, gpio_pin_e gpio_pin)
//{
//    PORT_TypeDef *gpio_ptr = gpio_port_addr_get(gpio_port);
//    
//    return gpio_drv_interrupt_type_get(gpio_ptr, gpio_pin);
//}

/**
 *@brief gpio toggle once
 *@param[in] gpio_port: one of gpio_port_e
 *@param[in] gpio_pin: gpio pin number
 *@return    none
*/
void gpio_toggle(gpio_port_e gpio_port, gpio_pin_e gpio_pin)
{
    PORT_TypeDef *gpio_ptr = gpio_port_addr_get(gpio_port);
    
    gpio_drv_toggle(gpio_ptr, gpio_pin);
}

/**
 *@brief gpio alternative fuction config
 *@param[in] gpio_port: one of gpio_port_e
 *@param[in] gpio_pin: gpio pin number
 *@param[in] gpio_alt: gpio alternal function number
 *@return    none
*/
void gpio_alterfunc_cfg(gpio_port_e gpio_port, gpio_pin_e gpio_pin, gpio_alt_e gpio_alt)
{
    PORT_TypeDef *gpio_ptr = gpio_port_addr_get(gpio_port);

    gpio_drv_alterfunc_cfg(gpio_ptr, gpio_pin, gpio_alt);
}

/**
 *@brief gpio strength config
 *@param[in] gpio_port: one of gpio_port_e
 *@param[in] gpio_pin: gpio pin number
 *@param[in] gpio_strength: gpio drive strength
 *@return    none
*/
void gpio_strength_cfg(gpio_port_e gpio_port, gpio_pin_e gpio_pin, gpio_strength_e gpio_strength)
{
//    PORT_TypeDef *gpio_ptr = gpio_port_addr_get(gpio_port);
//    
//    gpio_drv_strength_cfg(gpio_ptr, gpio_pin, gpio_strength);
}

/**
 *@brief gpio init
 *@param[in] gpio_settings_config: point to gpio related information
 *@return    none
*/
void gpio_init(gpio_settings_config_t *gpio_settings_config)
{
    DEV_ASSERT(NULL != gpio_settings_config);

    gpio_port_e gpio_port;
    gpio_pin_e  gpio_pin;

    gpio_port = gpio_settings_config->gpio_port;
    gpio_pin  = gpio_settings_config->gpio_pin;

    gpio_direction_cfg(gpio_port, gpio_pin, gpio_settings_config->gpio_dir);
	
    gpio_alterfunc_cfg(gpio_port, gpio_pin, gpio_settings_config->gpio_alt);
	

    //  if (GPIO_DIR_OUTPUT == gpio_settings_config->gpio_dir)
    //  {
    //      gpio_output_mode_cfg(gpio_port, gpio_pin, gpio_settings_config->gpio_output_mode);
    //  }

    //gpio_pull_cfg(gpio_port, gpio_pin, gpio_settings_config->pull_cfg);
    //gpio_strength_cfg(gpio_port, gpio_pin, gpio_settings_config->gpio_strength);

    /* config interrupt info */
   if (gpio_settings_config->gpio_interrupt_cfg_info.int_handle != NULL)
   {
       gpio_interrupt_cfg(gpio_port, gpio_pin, &gpio_settings_config->gpio_interrupt_cfg_info);
   }
   else
   {
       gpio_interrupt_disable(gpio_port, gpio_pin);
   }
}

/**
 *@brief gpio pta7 init
 *@param[in] none
 *@return    none
*/
//void gpio_a7_init(void)
//{
//    gpio_settings_config_t gpio_settings_config;
//    
//    gpio_settings_config.gpio_port = GPIO_A;
//    gpio_settings_config.gpio_pin = GPIO_PIN_7;
//    gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_1;
//    gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
//    gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
//    gpio_settings_config.gpio_strength = GPIO_STRENGTH_LOW_FAST;
//    gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;

//    gpio_init(&gpio_settings_config);
//}

/**
 *@brief config gpio pins 
 *@param[in] pin_count: gpio pin number need to config
 *@param[in] config: point to gpio pin config parameter 
 *@return    none
 */
//void pins_mux_init(uint32_t pin_count, gpio_settings_config_t *config)
//{
//    for (uint32_t i = 0U; i < pin_count; i++)
//    {
//        gpio_init(&config[i]);     
//    }
//}


/****************************************************************************
 ** \brief	GPIO_A_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 10
****************************************************************************/
uint32_t gpio_a_intp_cnt = 0;
void GPIO_A_IRQHandler(void)
{
	gpio_a_intp_cnt++;
	
	gpio_int_source_e gpio_a_interrupt_source;

	/* 1. 获取中断源 */
    PORT_TypeDef *gpio_ptr = gpio_port_addr_get(GPIO_A);
	gpio_a_interrupt_source = gpio_drv_status_flag_get(gpio_ptr);
	
	uint32_t interrupt_flag = gpio_ptr->ITSR_STR.w;
	
	/* 2. 清除所有中断标志位 */
    gpio_ptr->ITSR_STR.w = 0xFFFFFFFFUL;
	
	if(interrupt_flag == ((uint32_t)0x1U<<((uint32_t)gpio_a_interrupt_source)) )
	{
		//只有一个中断产生
	}
	else
	{
		//同时有多个中断产生，但是只反馈低位到高位查找第一个为1的位
	}

	/* 3. 中断回调函数 */
	if(NULL != g_gpio_int_handle[GPIO_A])
	{
		g_gpio_int_handle[GPIO_A](gpio_a_interrupt_source);
	}
	
}

/****************************************************************************
 ** \brief	GPIO_B_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 11
****************************************************************************/
uint32_t gpio_b_intp_cnt = 0;
void GPIO_B_IRQHandler(void)
{
	gpio_b_intp_cnt++;
	
	gpio_int_source_e gpio_b_interrupt_source;

	/* 1. 获取中断源 */
    PORT_TypeDef *gpio_ptr = gpio_port_addr_get(GPIO_B);
	gpio_b_interrupt_source = gpio_drv_status_flag_get(gpio_ptr);
	
	uint32_t interrupt_flag = gpio_ptr->ITSR_STR.w;
	
	/* 2. 清除所有中断标志位 */
    gpio_ptr->ITSR_STR.w = 0xFFFFFFFFUL;
	
	if(interrupt_flag == ((uint32_t)0x1U<<((uint32_t)gpio_b_interrupt_source)) )
	{
		//只有一个中断产生
	}
	else
	{
		//同时有多个中断产生，但是只反馈低位到高位查找第一个为1的位
	}

	/* 3. 中断回调函数 */
	if(NULL != g_gpio_int_handle[GPIO_B])
	{
		g_gpio_int_handle[GPIO_B](gpio_b_interrupt_source);
	}
}

/****************************************************************************
 ** \brief	GPIO_C_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 12
****************************************************************************/
uint32_t gpio_c_intp_cnt = 0;
void GPIO_C_IRQHandler(void)
{
	gpio_c_intp_cnt++;
	
	gpio_int_source_e gpio_c_interrupt_source;

	/* 1. 获取中断源 */
    PORT_TypeDef *gpio_ptr = gpio_port_addr_get(GPIO_C);
	gpio_c_interrupt_source = gpio_drv_status_flag_get(gpio_ptr);
	
	uint32_t interrupt_flag = gpio_ptr->ITSR_STR.w;
	
	/* 2. 清除所有中断标志位 */
    gpio_ptr->ITSR_STR.w = 0xFFFFFFFFUL;
	
	if(interrupt_flag == ((uint32_t)0x1U<<((uint32_t)gpio_c_interrupt_source)) )
	{
		//只有一个中断产生
	}
	else
	{
		//同时有多个中断产生，但是只反馈低位到高位查找第一个为1的位
	}

	/* 3. 中断回调函数 */
	if(NULL != g_gpio_int_handle[GPIO_C])
	{
		g_gpio_int_handle[GPIO_C](gpio_c_interrupt_source);
	}
}
