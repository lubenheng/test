#ifndef PMU_H
#define PMU_H


#include "pmu_reg.h"


/**
 *@brief config rtc_clk source
 *@param[in] pmu_ptr: point to address of pmu
 *@param[in] pm_rtc_clk_sel
 *@return    none
*/
static inline void pmu_drv_rtc_clk_cfg(PMU_TypeDef *pmu_ptr, pmu_rtc_clk_sel_e pm_rtc_clk_sel)
{
	// DEV_ASSERT(NULL != pmu_ptr);
	
	// pmu_ptr->HIB_CFG.RTCCLK_SEL = (uint32_t)pm_rtc_clk_sel;
}

/**
 *@brief disable gpio wake function for one gpio
 *@param[in] pmu_ptr: point to address of pmu
 *@param[in] gpio_wake_source
 *@return    none
*/
static inline void pmu_drv_hib_gpio_wake_close(PMU_TypeDef *pmu_ptr, hib_gpio_wake_source_e gpio_wake_source)
{
	// DEV_ASSERT(NULL != pmu_ptr);
	// DEV_ASSERT(gpio_wake_source < HIB_GPIO_WAKE_SOURCE_MAX);

	pmu_ptr->WAKECR.w &= ~(0x1UL<<gpio_wake_source);
}

/**
 *@brief disable gpio wake function for all gpio
 *@param[in] pmu_ptr: point to address of pmu
 *@return    none
*/
static inline void pmu_drv_hib_gpio_wake_close_all(PMU_TypeDef *pmu_ptr)
{
	// DEV_ASSERT(NULL != pmu_ptr);
	/* bit[11:0] is gpio wakeup enable register */
	pmu_ptr->WAKECR.w &= 0xfffff000; 
}

/**
 *@brief enable/disable button wake 
 *@param[in] pmu_ptr: point to address of pmu
 *@param[in]   enable: 
 *				ture: enbale pbtn
 *				false: disable pbtn
 *@return    none
*/
static inline void pmu_drv_rstn_enable(PMU_TypeDef *pmu_ptr, bool enable)
{
	// DEV_ASSERT(NULL != pmu_ptr);
    // if (enable)
    // {
    //     (pmu_ptr->HIB_CFG.w) |= (1U << PMU_HIB_CFG_BUTTON_WAKE_EN_POS);
    // }
    // else
    // {
    //     (pmu_ptr->HIB_CFG.w) &= ~(1U << PMU_HIB_CFG_BUTTON_WAKE_EN_POS);
    // }		
}



static inline void pmu_drv_lptimer_enable(PMU_TypeDef *pmu_ptr, bool enable)
{
    if(enable == true)
    {
        pmu_ptr->WAKECR.LPTMR_EN = 1U;
    }
    else
    {
        pmu_ptr->WAKECR.LPTMR_EN = 0U;
    }
}

/**
 *@brief enable/disable Analog LVD 5V
 *@param[in] pmu_ptr: point to address of pmu
 *@param[in]   enable: 
 *				ture: enbale Analog LVD 5V
 *				false: disable Analog LVD 5V
 *@return    none
*/
static inline void pmu_drv_lvd_5v_switch(PMU_TypeDef *pmu_ptr, bool enable)
{
	// DEV_ASSERT(NULL != pmu_ptr);
    // if (enable)
    // {
    //     (pmu_ptr->HIB_CFG.w) |= (1U << PMU_HIB_CFG_D2A_EN_LVD_5V_POS);
    // }
    // else
    // {
    //     (pmu_ptr->HIB_CFG.w) &= ~(1U << PMU_HIB_CFG_D2A_EN_LVD_5V_POS);
    // }		
}

/**
 *@brief enable/disable Analog LVW 5V 
 *@param[in] pmu_ptr: point to address of pmu
 *@param[in]   enable: 
 *				ture: enbale Analog LVW 5V 
 *				false: disable Analog LVW 5V 
 *@return    none
*/
static inline void pmu_drv_lvw_switch(PMU_TypeDef *pmu_ptr, bool enable)
{
	// DEV_ASSERT(NULL != pmu_ptr);
    // if (enable)
    // {
    //     (pmu_ptr->HIB_CFG.w) |= (1U << PMU_HIB_CFG_D2A_EN_LVW_5V_POS);
    // }
    // else
    // {
    //     (pmu_ptr->HIB_CFG.w) &= ~(1U << PMU_HIB_CFG_D2A_EN_LVW_5V_POS);
    // }		
}

/**
 *@brief enable/disable Analog LVD 3.3V
 *@param[in] pmu_ptr: point to address of pmu
 *@param[in]   enable: 
 *				ture: enbale Analog LVD 3.3V
 *				false: disable Analog LVD 3.3V
 *@return    none
*/
static inline void pmu_drv_lvd_3v3_switch(PMU_TypeDef *pmu_ptr, bool enable)
{
	// DEV_ASSERT(NULL != pmu_ptr);
    // if (enable)
    // {
    //     (pmu_ptr->HIB_CFG.w) |= (1U << PMU_HIB_CFG_D2A_EN_LVD_3V3_POS);
    // }
    // else
    // {
    //     (pmu_ptr->HIB_CFG.w) &= ~(1U << PMU_HIB_CFG_D2A_EN_LVD_3V3_POS);
    // }		
}

void hib_drv_handle(PMU_TypeDef  *pmu_ptr);
void pmu_drv_hib_sw_enter(PMU_TypeDef  *pmu_ptr);
void pmu_drv_hib_init(PMU_TypeDef  *pmu_ptr, hib_cfg_t *hib_cfg);
void pmu_drv_init(PMU_TypeDef  *pmu_ptr);
void pmu_drv_hib_gpio_wake_cfg(PMU_TypeDef *pmu_ptr, hib_gpio_wake_cfg_t *gpio_wake_cfg);



#endif /* End of PMU_H */

