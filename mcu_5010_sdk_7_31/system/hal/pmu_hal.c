#include "platform_common.h"

#include "pmu_reg.h"
#include "syscfg_reg.h"
#include "device_resource.h"
#include "pmu_hal.h"
#include "pmu.h"




//static PMU_INT_HANDLE g_pmu_interrupt_handle = NULL;

/**
 *@brief config rtc_clk source
 *@param[in] pm_rtc_clk_sel
 *@return    none
*/
void pmu_rtc_clk_cfg(pmu_rtc_clk_sel_e pm_rtc_clk_sel)
{
	// PMU_TypeDef *pmu_ptr = PMU;
	
	// pmu_drv_rtc_clk_cfg(pmu_ptr, pm_rtc_clk_sel);
}

/**
 *@brief interface to enter hibernate mode now
 *@param[in] none
 *@return    none
*/
void pmu_hib_sw_enter(void)
{
	// PMU_TypeDef  *pmu_ptr = PMU;
	
	// pmu_drv_hib_sw_enter(pmu_ptr);
}

/**
 *@brief interface to enter deepsleep mode now
 *@param[in] none
 *@return    none
*/
void pmu_deepsleep_now(void)
{
	PMU_SleepDeepEnable();
	PMU_SleepOnExitDisable();

	__DSB();
	__WFI();
}

/**
 *@brief interface to enter deepsleep mode, now and every time when mcu on exit
 *@param[in] none
 *@return    none
*/
void pmu_deepsleep_on_exit(void)
{
	PMU_SleepDeepEnable();
	PMU_SleepOnExitEnable();

	__DSB();
	__WFI();
}

/**
 *@brief interface to enter sleep mode now
 *@param[in] none
 *@return    none
*/
void pmu_sleep_now(void)
{
	// PMU_SleepDeepDisable();
	// PMU_SleepOnExitDisable();

	// __DSB();
	// __WFI();
}

/**
 *@brief interface to enter sleep mode, now and every time when mcu on exit
 *@param[in] none
 *@return    none
*/
void pmu_sleep_on_exit(void)
{
	// PMU_SleepDeepDisable();
	// PMU_SleepOnExitEnable();

	// __DSB();
	// __WFI();
}

void pmu_wakeup_interrupt_find()
{
	PMU_TypeDef  *pmu_ptr = PMU;
	volatile uint32_t wake_ir_tmp;

	//在芯片唤醒时，可以在进入deepsleep前向Flash特定位置写入特定密钥
	//并结合中断标志位判断唤醒

	wake_ir_tmp = pmu_ptr->WAKEIR.w;
	volatile uint32_t interrupt_flag = (wake_ir_tmp >> 16) ; // FLAG

	if(interrupt_flag&0x1) 	   //低功耗唤醒产生lptimer Flag，配置IE产生对应中断
	{
		INFO("find lptimer wakeup flag \n");
		wake_ir_tmp |= BIT0;
		wake_ir_tmp &= ~(0xffff0000); //中断寄存器标志位存在w1c，因此flag位只能写0，寄存器使能位写对应值
		pmu_ptr->WAKEIR.w = wake_ir_tmp;
		pmu_ptr->WAKEIR.GPIO_IE = 1U;
	}
	if((interrupt_flag>>1)&0x1)//低功耗唤醒产生gpio Flag，配置IE产生对应中断
	{
		INFO("find gpio wakeup flag \n");
		wake_ir_tmp |= BIT1;
		wake_ir_tmp &= ~(0xffff0000); //中断寄存器标志位存在w1c，因此flag位只能写0，寄存器使能位写对应值
		pmu_ptr->WAKEIR.w = wake_ir_tmp;
	}
	if((interrupt_flag>>2)&0x1)//低功耗唤醒产生rstn Flag，配置IE产生对应中断
	{
		INFO("find rstn wakeup flag \n");
		wake_ir_tmp |= BIT2;
		wake_ir_tmp &= ~(0xffff0000); //中断寄存器标志位存在w1c，因此flag位只能写0，寄存器使能位写对应值
		pmu_ptr->WAKEIR.w = wake_ir_tmp;
	}

}



/**
 *@brief init one gpio for gpio wake
 *@param[in] gpio_wake_source
 *@return    none
*/
static void pmu_hib_gpio_init(hib_gpio_wake_source_e gpio_wake_source)
{
	gpio_settings_config_t gpio_settings_config;
	switch (gpio_wake_source)
	{
		case HIB_GPIO_WAKE_SOURCE_PTA3:
			gpio_settings_config.gpio_port = GPIO_A;
			gpio_settings_config.gpio_pin = GPIO_PIN_3;
			break;
		case HIB_GPIO_WAKE_SOURCE_PTA4:
			gpio_settings_config.gpio_port = GPIO_A;
			gpio_settings_config.gpio_pin = GPIO_PIN_4;
			break;
		case HIB_GPIO_WAKE_SOURCE_PTA13:
			gpio_settings_config.gpio_port = GPIO_A;
			gpio_settings_config.gpio_pin = GPIO_PIN_13;
			break;
		case HIB_GPIO_WAKE_SOURCE_PTA14:
			gpio_settings_config.gpio_port = GPIO_A;
			gpio_settings_config.gpio_pin = GPIO_PIN_14;
			break;	
		case HIB_GPIO_WAKE_SOURCE_PTB2:
			gpio_settings_config.gpio_port = GPIO_B;
			gpio_settings_config.gpio_pin = GPIO_PIN_2;
			break;
		case HIB_GPIO_WAKE_SOURCE_PTB3:
			gpio_settings_config.gpio_port = GPIO_B;
			gpio_settings_config.gpio_pin = GPIO_PIN_3;
			break;
		case HIB_GPIO_WAKE_SOURCE_PTB4:
			gpio_settings_config.gpio_port = GPIO_B;
			gpio_settings_config.gpio_pin = GPIO_PIN_4;
			break;
		case HIB_GPIO_WAKE_SOURCE_PTB5:
			gpio_settings_config.gpio_port = GPIO_B;
			gpio_settings_config.gpio_pin = GPIO_PIN_5;
			break;
		case HIB_GPIO_WAKE_SOURCE_PTB12:
			gpio_settings_config.gpio_port = GPIO_B;
			gpio_settings_config.gpio_pin = GPIO_PIN_12;
			break;
		case HIB_GPIO_WAKE_SOURCE_PTB13:
			gpio_settings_config.gpio_port = GPIO_B;
			gpio_settings_config.gpio_pin = GPIO_PIN_13;
			break;
		case HIB_GPIO_WAKE_SOURCE_PTC3:
			gpio_settings_config.gpio_port = GPIO_C;
			gpio_settings_config.gpio_pin = GPIO_PIN_3;
			break;
		case HIB_GPIO_WAKE_SOURCE_PTC4:
			gpio_settings_config.gpio_port = GPIO_C;
			gpio_settings_config.gpio_pin = GPIO_PIN_4;
			break;

		default:
			break;
	}
	
	gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_0;
	gpio_settings_config.gpio_dir = GPIO_DIR_INPUT;
	gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
	gpio_settings_config.gpio_strength = GPIO_STRENGTH_MODE_1;	
	gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&gpio_settings_config);	
}

/**
 *@brief config to enable gpio wake from hibenate
 *@param[in] gpio_wake_cfg: point to gpio wake related information
 *@return    none
*/
void pmu_hib_gpio_wake_cfg(hib_gpio_wake_cfg_t *gpio_wake_cfg)
{
	DEV_ASSERT(NULL != gpio_wake_cfg);
	
	PMU_TypeDef  *pmu_ptr = PMU;
	
	pmu_hib_gpio_init(gpio_wake_cfg->gpio_wake_source);
	pmu_drv_hib_gpio_wake_cfg(pmu_ptr, gpio_wake_cfg);

	/* clear flag and config interrupt */
	pmu_ptr->WAKEIR.w |=  ( (1U << 16) | (1U << 17) | (1U << 18) );
	pmu_ptr->WAKEIR.GPIO_IE = gpio_wake_cfg->gpio_wake_interrupt;
}


/**
 *@brief config to enable gpio wake from hibenate
 *@param[in] lptimer_wake_cfg: point to lptimer wake related information
 *@return    none
*/
void pmu_lptimer_wake_cfg(hib_lptimer_wake_cfg_t *lptimer_wake_cfg)
{
	//DEV_ASSERT(NULL != lptimer_wake_cfg);
	
	PMU_TypeDef  *pmu_ptr = PMU;
	
	// pmu_hib_gpio_init(gpio_wake_cfg->gpio_wake_source);
	// pmu_drv_hib_gpio_wake_cfg(pmu_ptr, gpio_wake_cfg);

	/* set lptimer timer, uint:second */
	pmu_ptr->LPTMRTH = (LIRC_FREQ*lptimer_wake_cfg->wakeup_time_second);
	
	
	/* clear flag and config interrupt */
	pmu_ptr->WAKEIR.w |=  ( (1U << 16) | (1U << 17) | (1U << 18) );
	pmu_ptr->WAKEIR.LPTMR_IE = lptimer_wake_cfg->lptimer_wake_interrupt;

	/* enable lptimer wakeup */
	//pmu_ptr->WAKECR.LPTMR_EN = 1U;
}

/**
 *@brief config to enable gpio wake from hibenate
 *@param[in] none
 *@return    none
*/
void pmu_hib_rstn_wake_cfg()
{
	PMU_TypeDef  *pmu_ptr = PMU;

	/* clear flag and config interrupt */
	pmu_ptr->WAKEIR.w |=  ( (1U << 16) | (1U << 17) | (1U << 18) );
	pmu_ptr->WAKEIR.RETN_IE = 1U;
}

/**
 *@brief disable gpio wake function for one gpio
 *@param[in] gpio_wake_source
 *@return    none
*/
void pmu_hib_gpio_wake_close(hib_gpio_wake_source_e gpio_wake_source)
{
	PMU_TypeDef  *pmu_ptr = PMU;
	
	pmu_drv_hib_gpio_wake_close(pmu_ptr,gpio_wake_source);
}

/**
 *@brief disable gpio wake function for all gpio
 *@param[in] pmu_ptr: point to address of pmu
 *@return none
*/
void pmu_hib_gpio_wake_close_all(void)
{	
	PMU_TypeDef  *pmu_ptr = PMU;
	
	pmu_drv_hib_gpio_wake_close_all(pmu_ptr);
}

/**
 *@brief enable/disable button wake
 *@param[in]   enable: 
 *				ture: enbale lirc
 *				false: disable lirc
 *@return    none
*/
void pmu_rstn_enable(bool enable)
{
	PMU_TypeDef  *pmu_ptr = PMU;
	
	pmu_drv_rstn_enable(pmu_ptr, enable);			
}

/**
 *@brief enable/disable button wake
 *@param[in]   enable: 
 *				ture: enbale lirc
 *				false: disable lirc
 *@return    none
*/
void pmu_lptimer_enable(bool enable)
{
	PMU_TypeDef  *pmu_ptr = PMU;
	
	pmu_drv_lptimer_enable(pmu_ptr, enable);			
}

/**
 *@brief enable/disable Analog LVD 5V
 *@param[in]   enable: 
 *				ture: enbale Analog LVD 5V
 *				false: disable Analog LVD 5V
 *@return    none
*/
void pmu_lvd_5v_switch(bool enable)
{
	// PMU_TypeDef  *pmu_ptr = PMU;
	
	// pmu_drv_lvd_5v_switch(pmu_ptr, enable);			
}

/**
 *@brief enable/disable Analog LVW 5V 
 *@param[in]   enable: 
 *				ture: enbale Analog LVW 5V 
 *				false: disable Analog LVW 5V 
 *@return    none
*/
void pmu_lvw_switch(bool enable)
{
	// PMU_TypeDef  *pmu_ptr = PMU;
	
	// pmu_drv_lvw_switch(pmu_ptr, enable);		
}

/**
 *@brief enable/disable Analog LVD 3.3V
 *@param[in] pmu_ptr: point to address of pmu
 *@param[in]   enable: 
 *				ture: enbale Analog LVD 3.3V
 *				false: disable Analog LVD 3.3V
 *@return    none
*/
void pmu_lvd_3v3_switch(bool enable)
{
	// PMU_TypeDef  *pmu_ptr = PMU;
	
	// pmu_drv_lvd_3v3_switch(pmu_ptr, enable);		
}

/**
 *@brief hibnerate interrupt handle
 *@param[in] none
 *@return    none
*/
// void HIB_CTRL_IRQHandler(void)
// {
	// PMU_TypeDef  *pmu_ptr = PMU;

	// if (NULL != g_pmu_interrupt_handle)
	// {
	// 	g_pmu_interrupt_handle();
	// }
	// else
	// {
	// 	hib_drv_handle(pmu_ptr);	
	// }
// }

/**
 *@brief hibernate related register init
 *@param[in] hib_cfg: point to parameter user defined
 *@return    none
*/
void pmu_hib_init(hib_cfg_t *hib_cfg)
{
	// PMU_TypeDef  *pmu_ptr = PMU;

	// pmu_drv_hib_init(pmu_ptr, hib_cfg);

	// /*config pbtn enter*/
	// if (hib_cfg->is_pbtn_enter_hib == true)
	// {
	// 	if (0U == NVIC_GetEnableIRQ(HIB_IRQn))
	// 	{
	// 		g_pmu_interrupt_handle = hib_cfg->pmu_interrupt_handle;
	// 		NVIC_EnableIRQ(HIB_IRQn);
	// 		NVIC_SetPriority(HIB_IRQn, hib_cfg->pmu_interrupt_priority);
	// 	}
	// }
}

/**
 *@brief pmu related register init
 *@param[in] none
 *@return    none
*/
void pmu_init(void)
{
	// PMU_TypeDef  *pmu_ptr = PMU;

	// pmu_drv_init(pmu_ptr);
}

uint32_t g_pmu_cnt = 0;
void PMU_IRQHandler(void)
{
	PMU_TypeDef *pmu_ptr = PMU;

	volatile uint32_t interrupt_flag = ((pmu_ptr->WAKEIR.w>>0) & (pmu_ptr->WAKEIR.w >> 16)); // IE&FLAG

	/* clear interupt flag */
	pmu_ptr->WAKEIR.w |= ((1U << 16) | (1U << 17) | (1U << 18));

	g_pmu_cnt++;

	if(interrupt_flag&0x1) 	   //低功耗唤醒产生lptimer Flag
	{
		INFO("lptimer wakeup interrupt \n");
	}
	if((interrupt_flag>>1)&0x1)//低功耗唤醒产生gpio Flag
	{
		pmu_ptr->WAKECR.w = (0xfffff000 & pmu_ptr->WAKECR.w); 
		INFO("gpio wakeup interrupt \n");
	}
	if((interrupt_flag>>2)&0x1)//低功耗唤醒产生rstn Flag
	{
		INFO("rstn wakeup interrupt \n");
	}


	// if (NULL != g_pmu_interrupt_handle)
	// {
	// 	g_pmu_interrupt_handle();
	// }
	// else
	// {
	// 	hib_drv_handle(pmu_ptr);
	// }
}
