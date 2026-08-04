#include "platform_common.h"

#include "wdog_reg.h"
#include "device_resource.h"
#include "wdog_hal.h"
#include "wdog.h"

/*******************************************************************************
 * Variables
 ******************************************************************************/
/** Table of base addresses for IWDG instances. */
 static IWDG_TypeDef *setting_iwdg = IWDG;

/*******************************************************************************
* Public UART Control Functions
******************************************************************************/
/**
 *@brief Initialize watchdog
 *@param[in] ms: You can customize the watchdog timer during initialization, unit is ms
 *@return    none
*/
void iwdg_init(uint32_t ms)
{
    NVIC_SetPriority(NonMaskableInt_IRQn, 2); /*TIMER1????? */
	NVIC_ClearPendingIRQ(NonMaskableInt_IRQn);  /*???????*/
	NVIC_EnableIRQ (NonMaskableInt_IRQn);       /*????*/

    //unlock
    //setting_iwdg->LR_STR.w &= 0x0;
    setting_iwdg->LR_STR.w = 0x1ACCE551;

    //enable
    setting_iwdg->CR.EN = 0x1;
	
    //config reg
    ms *= 32;
    setting_iwdg->RLR.RL = ms;

    //lock
    setting_iwdg->LR_STR.w = 0x0;
}

/**
 *@brief feed wdg
 *@param[in] us: set your own idwg feeding time, unit is ms
 *@return    none
*/
void feed_iwdg(uint32_t ms)
{
    //unlock
    setting_iwdg->LR_STR.w = 0x1ACCE551;

    //set RL value
    ms *= 32;
    setting_iwdg->KR.KEY = ms;
	  setting_iwdg->RLR.RL = ms;

    //lock
    setting_iwdg->LR_STR.w = 0x0;
}

/**
 *@brief check iwdg status
 *@param[in] none
 *@return    0: not to zero;  1: having to zero
*/
uint8_t iwdg_status(void)
{
	if(setting_iwdg->SR.HDF == 1)
		return 1;
	else
		return 0;
}

/**
 *@brief return iwdg count
 *@param[in] none
 *@return    count
*/
uint32_t iwdg_counts(void)
{
	return setting_iwdg->CNTR.CNT;
}



