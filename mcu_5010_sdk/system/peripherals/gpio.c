#include "platform_common.h"

#include "gpio_reg.h"
#include "device_resource.h"
#include "gpio_hal.h"
#include "gpio.h"

/**
 *@brief config GPIO as default
 *@param[in] gpio_ptr: point to address of gpio
 *@return    none
*/
void gpio_drv_init_to_default(PORT_TypeDef *gpio_ptr)
{

}

/**
 *@brief config gpio as input or output
 *@param[in] gpio_ptr: point to address of gpio
 *@param[in] gpio_pin: gpio pin number
 *@param[in] gpio_dir: gpio input or output
 *                      - GPIO_DIR_INPUT
 *                      - GPIO_DIR_OUTPUT
 *@return    none
*/
void gpio_drv_direction_cfg(PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin, gpio_dir_e gpio_dir)
{
    DEV_ASSERT(NULL != gpio_ptr);
    DEV_ASSERT(gpio_pin < GPIO_PIN_NUM_MAX);

    if (gpio_dir == GPIO_DIR_INPUT)
    {
        // gpio_ptr->ITEC_STR.w  |= (0x01UL<<gpio_pin);
        // gpio_ptr->ITES_STR.w  |= (0x01UL<<gpio_pin);
        gpio_ptr->IEC_STR.w |= (0x01UL<<gpio_pin);
        gpio_ptr->IES_STR.w |= (0x01UL<<gpio_pin);
    }
    else
    {
        gpio_ptr->OEC_STR.w |= (0x01UL<<gpio_pin);
        gpio_ptr->OES_STR.w |= (0x01UL<<gpio_pin);
    }
}

/**
 *@brief gpio pull up or pull down config
 *@param[in] gpio_ptr: point to address of gpio
 *@param[in] gpio_pin: gpio pin number
 *@param[in] pull_cfg: pull up or pull down configuration
 *                      - GPIO_PULL_UP
 *                      - GPIO_PULL_DOWN
 *                      - GPIO_PULL_DISABLE
 *@return    none
*/
void gpio_drv_pull_cfg(PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin, gpio_pull_cfg_e pull_cfg)
{

}

/**
 *@brief gpio interrupt config
 *@param[in] gpio_ptr: point to address of gpio
 *@param[in] gpio_pin: gpio pin number
 *@param[in] gpio_interrupt_cfg_info: point to struct include interrupt config information
 *@return    none
*/
void gpio_drv_interrupt_cfg(PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin, const gpio_interrupt_cfg_info_t *gpio_interrupt_cfg_info)
{
    DEV_ASSERT(NULL != gpio_ptr);
    DEV_ASSERT(gpio_pin < GPIO_PIN_NUM_MAX);
    DEV_ASSERT(NULL != gpio_interrupt_cfg_info);

    /* write 1 to clear gpio interrupt type */
    gpio_ptr->ITTC_STR.w |= (0x01UL<<gpio_pin);
    
    uint32_t interrupt_cfg_offset;
    
    if (gpio_pin < GPIO_PIN_8)
    {
        interrupt_cfg_offset = (uint32_t)gpio_pin*4U;
        gpio_ptr->ITT0_STR.w = (gpio_ptr->ITT0_STR.w & (~(PORTA_INTTYPE_INTTYPE0_MASK<<interrupt_cfg_offset)))
                                |(gpio_interrupt_cfg_info->interrupt_type<<interrupt_cfg_offset);
    }
    else
    {
        interrupt_cfg_offset = (gpio_pin - GPIO_PIN_8)*4U;
        gpio_ptr->ITT1_STR.w = (gpio_ptr->ITT1_STR.w & (~(PORTA_INTTYPE_INTTYPE0_MASK<<interrupt_cfg_offset)))
                                |((uint32_t)gpio_interrupt_cfg_info->interrupt_type<<interrupt_cfg_offset);
    }

    gpio_ptr->ITSR_STR.w = (0x01UL<<gpio_pin); /*clear the correspond pin's interrupt status*/
    gpio_ptr->ITES_STR.w |= (0x01UL<<gpio_pin);  /*enable the correspond pin's interrupt*/
}

/**
 *@brief : gpio alternative fuction config
 *@param[in] gpio_ptr: point to address of gpio
 *@param[in] gpio_pin: gpio pin number
 *@param[in] gpio_alt gpio alternal function number
 *@return  none
*/
void gpio_drv_alterfunc_cfg(PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin, gpio_alt_e gpio_alt)
{
    DEV_ASSERT(NULL != gpio_ptr);
    DEV_ASSERT(gpio_pin < GPIO_PIN_NUM_MAX);

    uint32_t alterfunc_cfg_offset;
	
	/* write 0->1 operate AF0_STR/AF1_STR directly, write 1->0 operate need write 1 to AFC_STR clear first, then write AF0_STR/AF1_STR value	*/
	gpio_ptr->AFC_STR.w |= (0x01UL<<gpio_pin);
    
    if (gpio_pin < GPIO_PIN_8)
    {
        alterfunc_cfg_offset = gpio_pin*4U;                      
        gpio_ptr->AF0_STR.w  = (gpio_ptr->AF0_STR.w & (~(PORTA_ALTFUNC_OUTALT0_MASK<<alterfunc_cfg_offset)))
                                |(gpio_alt<<alterfunc_cfg_offset);  
    }
    else
    {
        alterfunc_cfg_offset = (gpio_pin - GPIO_PIN_8)*4U;
        gpio_ptr->AF1_STR.w = (gpio_ptr->AF1_STR.w & ~(PORTA_ALTFUNC_OUTALT0_MASK<<alterfunc_cfg_offset))
                                 |(gpio_alt<<alterfunc_cfg_offset);
    }
		

}



/**
 *@brief config gpio output mode as push_pull, open_drain or open_source
 *@param[in] gpio_ptr: point to address of gpio
 *@param[in] gpio_pin: gpio pin number
 *@param[in] gpio_output_mode: push_pull, open_drain or open_source
 *@return    none
*/
void gpio_drv_output_mode_cfg(PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin, gpio_output_mode_e gpio_output_mode)
{

}

/**
 *@brief config gpio input mode as schmitt(hysteresis) or cmos(non-hysteresis)
 *@param[in] gpio_ptr: point to address of gpio
 *@param[in] gpio_pin: gpio pin number
 *@param[in] gpio_input_mode: schmitt(hysteresis) or cmos(non-hysteresis)
 *@return    none
*/
void gpio_drv_input_mode_cfg(PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin, gpio_input_mode_e gpio_input_mode)
{

}

/**
 *@brief set gpio output voltage high or low
 *@param[in] gpio_ptr: point to address of gpio
 *@param[in] gpio_pin: gpio pin number
 *@param[in] level: output voltage level
 *                      - GPIO_LOW_LEVEL
 *                      - GPIO_HIGH_LEVEL
 *@return    none
*/
inline void gpio_drv_output_set(PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin, gpio_level_e level)
{
    DEV_ASSERT(NULL != gpio_ptr);
    DEV_ASSERT(gpio_pin < GPIO_PIN_NUM_MAX);
    
    if (level == GPIO_HIGH_LEVEL)
    {
        gpio_ptr->DRS_STR.w |= (0x01UL<<gpio_pin);
    }
    else
    {
        gpio_ptr->DRC_STR.w |= (0x01UL<<gpio_pin);
    }
}

/**
 *@brief get gpio dataout bit 
 *@param[in] gpio_ptr: point to address of gpio
 *@param[in] gpio_pin: gpio pin number
 *@return    gpio dataout bit
*/
//static inline bool gpio_drv_output_get(const PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin)
//{
    // DEV_ASSERT(NULL != gpio_ptr);
    // DEV_ASSERT(gpio_pin < GPIO_PIN_NUM_MAX);

    // return (1U == ((gpio_ptr->DATAOUT.w & ((uint32_t)1U << gpio_pin)) >> gpio_pin)); 	
//}

/**
 *@brief get gpio input voltage high or low
 *@param[in] gpio_ptr: point to address of gpio
 *@param[in] gpio_pin: gpio pin number
 *@return    gpio datain bit
*/
//static inline gpio_level_e gpio_drv_input_get(const PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin)
//{
    // DEV_ASSERT(NULL != gpio_ptr);
    // DEV_ASSERT(gpio_pin < GPIO_PIN_NUM_MAX);
    
    // return (1U == (gpio_ptr->DATAIN.w & (0x01UL << gpio_pin))\
    //                              ? GPIO_HIGH_LEVEL : GPIO_LOW_LEVEL);
//}


/**
 *@brief gpio toggle once
 *@param[in] gpio_ptr: point to address of gpio
 *@param[in] gpio_pin: gpio pin number
 *@return    none
*/
inline void gpio_drv_toggle(PORT_TypeDef *gpio_ptr, gpio_pin_e gpio_pin)
{
     DEV_ASSERT(NULL != gpio_ptr);
     DEV_ASSERT(gpio_pin < GPIO_PIN_NUM_MAX);
    
     //gpio_ptr->DATAOUT.w ^= (0x01UL << gpio_pin);
	gpio_ptr->DRT_STR.w = (0x01UL << gpio_pin);
}



gpio_int_source_e gpio_drv_status_flag_get(PORT_TypeDef *gpio_ptr)
{
	uint16_t interrupt_flag = gpio_ptr->ITSR_STR.ITS;
    
	// 方法1: 从低位到高位查找第一个为1的位
	// int find_first_set_bit_low_to_high(uint16_t value)

	for (int i = 0U; i < 16U; i++)
	{
		if (interrupt_flag & (1 << i))
		{
			return (gpio_int_source_e)(i); // 返回位位置（0-15）
		}
	}
	return GPIO_INT_Fault; // 没有找到为1的位
}
