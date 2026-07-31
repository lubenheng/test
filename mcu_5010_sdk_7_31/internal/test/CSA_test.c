#include "platform_common.h"

#include "test.h"

#include "CSA_Reg.h"

#include "CSA_Type.h"
#include "CSA.h"
#include "CSA_test.h"


#include "gpio_type.h"
#include "gpio_hal.h"
#include "para_define.h"

//-------------------------------------------------------------//
// function test



/*****************************************************************************
 ** \brief	CSA_Test_Init
 **					ADC MCPWM触发测试初始化
 ** \param [in] 
					none
 ** \return  none
 ** \note	 
*****************************************************************************/


void CSA_Test_Init(void)
{
	CSA_ENABLE(ENABLE);
	
	#if (Single_Shunt == Config_Shunt_Mode)	
	{
		CSA_InputCH_SWMode(CSA_FIXED_CH_MODE);
		CSA_SET_InputCH(CSA_FIXED_INPUT_CH3);	
	}
	#else
	{
		CSA_InputCH_SWMode(CSA_Test_Mode);
	}	
	#endif
	
	*(int32_t *)0x40015804 = 0x11;
	*(int32_t *)0x4800083C = 0xfff00000;
	*(int32_t *)0x48000840 = 0x0000000f;	
}

void CSA_gpio_init(void)
{
//	gpio_settings_config_t setting_cfg;



//	/* PTC8 PIN  */
//	setting_cfg.gpio_port = GPIO_C;
//	setting_cfg.gpio_pin = GPIO_PIN_8;
//	setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
//	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
//	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
//	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//	gpio_init(&setting_cfg);



}

