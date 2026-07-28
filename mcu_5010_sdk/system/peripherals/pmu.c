#include "platform_common.h"

#include "syscfg_reg.h"
#include "pmu_reg.h"
#include "gpio_hal.h"
#include "pmu_type.h"
#include "pmu.h"

// #if   defined ( __CC_ARM ) /*for MDK*/
// #pragma push
// #pragma O0 /*use this method to realize the delay time to the same when compiled on MDK*/
// #endif

// #if defined ( __ICCARM__ )
// #pragma optimize=none
// #endif

// #if defined ( USE_GCC )
// #pragma GCC push_options
// #pragma GCC optimize ("Os")
// #endif

// #if defined (__ghs__)
// #pragma ghs ZO
// #endif

//static void time_delay_for_padkeep_clear(void)
//{
//		for (uint32_t i=0; i<30; i++)
//		{
//			__NOP();
//			__NOP();
//		}
//}

// #if   defined ( __CC_ARM ) /*for MDK*/
// #pragma pop
// #endif

// #if defined ( __ICCARM__ )
// #pragma optimize=none
// #endif

// #if defined ( USE_GCC )
// #pragma GCC pop_options
// #endif

// #if defined (__ghs__)
// #pragma ghs Odefault
// #endif

/**
 *@brief  hibnerate interrupt handle
 *@param[in] pmu_ptr: point to address of pmu
 *@return    none
*/
void hib_drv_handle(PMU_TypeDef *pmu_ptr)
{
	// uint32_t int_stat = pmu_ptr->HIB_INTST.w;
	
	// pmu_ptr->HIB_INTST.w = int_stat;/*write 1 clear*/
	
	// __DSB();/*wait clearing flag completed*/
	
	// /*interrupt, then check fw hib en, int is not open to user*/
	// if ((pmu_ptr->HIB_INTEN.w & int_stat) != 0u)
	// {
	// 	if (pmu_ptr->HIB_CFG.FW_HIB_EN == 1U)
	// 	{
	// 		pmu_ptr->HIB_CTRL.FW_HIB = 1U;
	// 	}
	// }
}

/**
 *@brief interface to enter hibernate mode now
 *@param[in] pmu_ptr: point to address of pmu
 *@return    none
*/
void pmu_drv_hib_sw_enter(PMU_TypeDef *pmu_ptr)
{
	// DEV_ASSERT(NULL != pmu_ptr);
	
	// if ((pmu_ptr->HIB_CFG.TIMER_WAKE_EN|pmu_ptr->HIB_CFG.BUTTON_WAKE_EN|pmu_ptr->HIB_CFG.RTC_WAKE_EN) != 0u)
	// {
	// 	PMU_SleepDeepDisable();
	// 	PMU_SleepOnExitDisable();
		
	// 	pmu_ptr->HIB_CFG.HIB_EN = 1U;
		
	// 	__DSB();
	// 	__WFI();
	// }
}

/**
 *@brief config gpio to enable gpio wake from hibenate
 *@param[in] pmu_ptr: point to address of pmu
 *@param[in] gpio_wake_cfg: point to gpio wake related information
 *@return    none
*/
void pmu_drv_hib_gpio_wake_cfg(PMU_TypeDef *pmu_ptr, hib_gpio_wake_cfg_t *gpio_wake_cfg)
{
	DEV_ASSERT(NULL != pmu_ptr);
	DEV_ASSERT(NULL != gpio_wake_cfg);
	DEV_ASSERT(gpio_wake_cfg->gpio_wake_source < HIB_GPIO_WAKE_SOURCE_MAX);

	switch (gpio_wake_cfg->gpio_wake_source)
	{
	case HIB_GPIO_WAKE_SOURCE_PTA3:
		/* configure wake failing or raising edge */
		pmu_ptr->WAKECR.GPIO_IN0_PTA3_EN = 1U;
		pmu_ptr->WAKECR.GPIO_IN0_PTA3_POL = gpio_wake_cfg->gpio_wake_mode;
		break;
	case HIB_GPIO_WAKE_SOURCE_PTA4:

		/* configure wake failing or raising edge */
		pmu_ptr->WAKECR.GPIO_IN1_PTA4_EN = 1U;
		pmu_ptr->WAKECR.GPIO_IN1_PTA4_POL = gpio_wake_cfg->gpio_wake_mode;
		break;
	case HIB_GPIO_WAKE_SOURCE_PTA13:

		/* configure wake failing or raising edge */
		pmu_ptr->WAKECR.GPIO_IN2_PTA13_EN = 1U;
		pmu_ptr->WAKECR.GPIO_IN2_PTA13_POL = gpio_wake_cfg->gpio_wake_mode;
		break;
	case HIB_GPIO_WAKE_SOURCE_PTA14:

		/* configure wake failing or raising edge */
		pmu_ptr->WAKECR.GPIO_IN3_PTA14_EN = 1U;
		pmu_ptr->WAKECR.GPIO_IN3_PTA14_POL = gpio_wake_cfg->gpio_wake_mode;
		break;
	case HIB_GPIO_WAKE_SOURCE_PTB2:

		/* configure wake failing or raising edge */
		pmu_ptr->WAKECR.GPIO_IN4_PTB2_EN = 1U;
		pmu_ptr->WAKECR.GPIO_IN4_PTB2_POL = gpio_wake_cfg->gpio_wake_mode;
		break;
	case HIB_GPIO_WAKE_SOURCE_PTB3:
		/* configure wake failing or raising edge */
		pmu_ptr->WAKECR.GPIO_IN5_PTB3_EN = 1U;
		pmu_ptr->WAKECR.GPIO_IN5_PTB3_POL = gpio_wake_cfg->gpio_wake_mode;
		break;
	case HIB_GPIO_WAKE_SOURCE_PTB4:
		/* configure wake failing or raising edge */
		pmu_ptr->WAKECR.GPIO_IN6_PTB4_EN = 1U;
		pmu_ptr->WAKECR.GPIO_IN6_PTB4_POL = gpio_wake_cfg->gpio_wake_mode;
		break;
	case HIB_GPIO_WAKE_SOURCE_PTB5:
		/* configure wake failing or raising edge */
		pmu_ptr->WAKECR.GPIO_IN7_PTB5_EN = 1U;
		pmu_ptr->WAKECR.GPIO_IN7_PTB5_POL = gpio_wake_cfg->gpio_wake_mode;
		break;
	case HIB_GPIO_WAKE_SOURCE_PTB12:
		/* configure wake failing or raising edge */
		pmu_ptr->WAKECR.GPIO_IN8_PTB12_EN = 1U;
		pmu_ptr->WAKECR.GPIO_IN8_PTB12_POL = gpio_wake_cfg->gpio_wake_mode;
		break;
	case HIB_GPIO_WAKE_SOURCE_PTB13:
		/* configure wake failing or raising edge */
		pmu_ptr->WAKECR.GPIO_IN9_PTB13_EN = 1U;
		pmu_ptr->WAKECR.GPIO_IN9_PTB13_POL = gpio_wake_cfg->gpio_wake_mode;
		break;
	case HIB_GPIO_WAKE_SOURCE_PTC3:
		/* configure wake failing or raising edge */
		pmu_ptr->WAKECR.GPIO_IN10_PTC3_EN = 1U;
		pmu_ptr->WAKECR.GPIO_IN10_PTC3_POL = gpio_wake_cfg->gpio_wake_mode;
		break;
	case HIB_GPIO_WAKE_SOURCE_PTC4:
		/* configure wake failing or raising edge */
		pmu_ptr->WAKECR.GPIO_IN11_PTC4_EN = 1U;
		pmu_ptr->WAKECR.GPIO_IN11_PTC4_POL = gpio_wake_cfg->gpio_wake_mode;
		break;

	default:
		break;
	}

}

/**
 *@brief hibernate related register init
 *@param[in] pmu_ptr: point to address of pmu
 *@param[in] hib_cfg: point to parameter user defined
 *@return    none
*/
void pmu_drv_hib_init(PMU_TypeDef *pmu_ptr, hib_cfg_t *hib_cfg)
{
// 	DEV_ASSERT(NULL != pmu_ptr);
// 	DEV_ASSERT(NULL != hib_cfg);

// //	pmu_ptr->HIB_CTRL.PADKEEP_MASK = 1U;

// 	pmu_ptr->HIB_CFG.HIB_PD_EN = 1U;
// 	pmu_ptr->HIB_CFG.PBTN_DEG_WIDTH = hib_cfg->pbtn_deglitch;
// 	pmu_ptr->HIB_CFG.PBTN_DEG_EN = 1U;
		
// 	pmu_ptr->HIB_CTRL.CG_REQ_EN = 1U;
// 	pmu_ptr->HIB_CTRL.ISO_REQ_EN = 1U;
// 	pmu_ptr->HIB_CTRL.ISO_LDO_OFF_EN = 1U;

// 	/*config pbtn exit*/
// 	if (hib_cfg->is_pbtn_exit_hib == false)
// 	{
// 		pmu_ptr->HIB_CFG.BUTTON_WAKE_EN = 0U;
// 	}
// 	else
// 	{
// 		pmu_ptr->HIB_CFG.PBTN_WAKE_CFG = (uint32_t)hib_cfg->pbtn_wake_time;
// 		pmu_ptr->HIB_CFG.BUTTON_WAKE_EN = 1U;
// 	}

// 	/*config timer exit*/
// 	if (hib_cfg->is_timer_exit_hib == false)
// 	{
// 		pmu_ptr->HIB_CFG.TIMER_WAKE_EN = 0U;
// 	}
// 	else
// 	{
// 		pmu_ptr->HIB_CFG.POWRON_TIME_CFG = 1U;
// 		pmu_ptr->TIMER_WAKE_VAL.TIMER_WAKE_VAL = hib_cfg->wake_lirc_clock;
// 		pmu_ptr->HIB_CFG.TIMER_WAKE_EN = 1U;
// 	}

// 	/*config pbtn enter*/
// 	if (hib_cfg->is_pbtn_enter_hib == true)
// 	{
// 		pmu_ptr->HIB_CFG.FW_HIB_EN = 1U;
// 		pmu_ptr->HIB_INTST.w = ~0u; /*must clear the status before enable the interrupt*/
// 		pmu_ptr->HIB_INTEN.w = (uint32_t)HIB_INT_PBTN_NEGEDGE_EN;/*other types are not supported*/
// 	}

// 	if (hib_cfg->is_rtc_exit_hib == true)
// 	{
// 		pmu_ptr->HIB_CFG.RTC_WAKE_EN = 1U;
// 	}
// 	else
// 	{
// 		pmu_ptr->HIB_CFG.RTC_WAKE_EN = 0U;
// 	}
	
}

/**
 *@brief pmu related register init
 *@param[in] pmu_ptr: point to address of pmu
 *@return none
*/
void pmu_drv_init(PMU_TypeDef  *pmu_ptr)
{
// 	DEV_ASSERT(NULL != pmu_ptr);
	
// 	pmu_ptr->HIB_CTRL.PADKEEP_CLEAR = 1U;
// 	__DSB();
// 	__ISB();

// 	/* OCS_Reserved_Start */	
// #ifdef ASIC_MODE
// 	/* OCS_Reserved_End */
// 	/*reset register to default; creat a rising edge if VDD is always lower than lvd/lvw*/
// 	pmu_ptr->HIB_CFG.w = PMU_CFG_INIT;

// 	/* OCS_Reserved_Start */	
// #endif
// 	/* OCS_Reserved_End */

// 	/*enable button wake in case entered hib without config exiting method*/
// 	/*reset_b cannot exit hib without button wake enable*/
// 	pmu_ptr->HIB_CFG.w |= PMU_HIB_CFG_BUTTON_WAKE_EN_MASK|PMU_HIB_CFG_HIB_PD_EN_MASK;
	
// 	/* time delay for padkeep clear */
// 	time_delay_for_padkeep_clear();

// 	pmu_ptr->HIB_CTRL.w = 0u;/*default*/

// /* OCS_Reserved_Start */
// #ifdef VCC_SCAN_TEST
// /* OCS_Reserved_End */
// #if (POWER_SUPPLY == VDD_5V) 
// 	pmu_ptr->HIB_CFG.w |= PMU_HIB_CFG_D2A_EN_LVD_5V_MASK|PMU_HIB_CFG_D2A_EN_LVW_5V_MASK;
// #else/*VDD=3.3V*/
// 	pmu_ptr->HIB_CFG.w |= PMU_HIB_CFG_D2A_EN_LVD_3V3_MASK;
// #endif
// /* OCS_Reserved_Start */
// #endif
// /* OCS_Reserved_End */

// 	pmu_ptr->HIB_CFG.HIB_EN = 0U;/*in case once enter hib, sleep or stop cannot entered*/
}

