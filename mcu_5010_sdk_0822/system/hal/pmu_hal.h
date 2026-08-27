#ifndef PMU_HAL_H
#define PMU_HAL_H


#include "syscfg_type.h"
#include "gpio_hal.h"
#include "pmu_type.h"  
#include "pmu.h"

void pmu_rtc_clk_cfg(pmu_rtc_clk_sel_e pm_rtc_clk_sel);
void pmu_hib_sw_enter(void);
void pmu_deepsleep_now(void);
void pmu_deepsleep_on_exit(void);
void pmu_sleep_now(void);
void pmu_sleep_on_exit(void);
void pmu_hib_gpio_wake_cfg(hib_gpio_wake_cfg_t *gpio_wake_cfg);
void pmu_lptimer_wake_cfg(hib_lptimer_wake_cfg_t *lptimer_wake_cfg);
void pmu_hib_rstn_wake_cfg(void);
void pmu_hib_gpio_wake_close(hib_gpio_wake_source_e gpio_wake_source);
void pmu_hib_gpio_wake_close_all(void);
void pmu_rstn_enable(bool enable);
void pmu_lptimer_enable(bool enable);
void pmu_lvd_5v_switch(bool enable);
void pmu_lvw_switch(bool enable);
void pmu_lvd_3v3_switch(bool enable);
void pmu_hib_init(hib_cfg_t *hib_cfg);
void pmu_init(void);
void pmu_wakeup_interrupt_find(void);

/** @}*/ /* End of addtogroup ingroup_pmu_hal */ 


#endif
