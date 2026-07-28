#include "platform_common.h"

#include "test.h"

#include "pmu_reg.h"
#include "pmu_hal.h"
#include "pmu_test.h"
#include "cmdline_internal.h"





void pmu_func1_test(int argc, char *argv[])
{
    INFO("pmu_func1_test run success! \n");
}	


void pmu_func2_test(int argc, char *argv[])
{
    INFO("pmu_func1_test run success! \n");

}	

void pmu_gpio_wakeup_test(int argc, char *argv[])
{
    PMU_TypeDef  *pmu_ptr = PMU;
    hib_gpio_wake_cfg_t gpio_wake_cfg;

    if (argc < 4U)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);
		return;
	}
	
	for(uint16_t i=0; i<argc; i++)
	{
		INFO("argv[%d]:%s\n", i,argv[i]);
	}
	
    gpio_wake_cfg.gpio_wake_source    = (hib_gpio_wake_source_e)get_arg_ulong(argv[1]);
    gpio_wake_cfg.gpio_wake_mode      = (hib_gpio_wake_mode_e)get_arg_ulong(argv[2]); 
    gpio_wake_cfg.gpio_wake_interrupt = (hib_interrupt_enable)get_arg_ulong(argv[3]); 	
    
    pmu_drv_lptimer_enable(pmu_ptr,false);
    
    pmu_hib_gpio_wake_cfg(&gpio_wake_cfg);

    INFO("pmu_gpio_wakeup_test run success! \n");
}

void pmu_lptimer_wakeup_test(int argc, char *argv[])
{
    PMU_TypeDef  *pmu_ptr = PMU;

    hib_lptimer_wake_cfg_t lptimer_wake_cfg;

    if (argc < 3U)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);
		return;
	}
	
	for(uint16_t i=0; i<argc; i++)
	{
		INFO("argv[%d]:%s\n", i,argv[i]);
	}
    
    pmu_drv_lptimer_enable(pmu_ptr,false);
    pmu_drv_hib_gpio_wake_close_all(pmu_ptr);
    
    lptimer_wake_cfg.wakeup_time_second     = get_arg_ulong(argv[1]);
    lptimer_wake_cfg.lptimer_wake_interrupt = (hib_interrupt_enable)get_arg_ulong(argv[2]); 	

    pmu_lptimer_wake_cfg(&lptimer_wake_cfg);
}


void pmu_rstn_wakeup_test(int argc, char *argv[])
{
    PMU_TypeDef  *pmu_ptr = PMU;

    pmu_drv_lptimer_enable(pmu_ptr,false);
    pmu_drv_hib_gpio_wake_close_all(pmu_ptr);

    pmu_hib_rstn_wake_cfg();
    INFO("pmu_rstn_wakeup_test run success! \n");
}
