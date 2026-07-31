#include "platform_common.h"

#include "test.h"

#include "clock_reg.h"
#include "clock_hal.h"
#include "clock_test.h"
#include "cmdline_internal.h"





void clock_func1_test(uint32_t test_case, uint32_t para1, uint32_t para2)
{




}	



void clock_func2_test(uint32_t test_case, uint32_t para1, uint32_t para2)
{
    uint8_t num, i;
    for(i = 0; i < para1; i++){
        switch (rcc_hal_sys_clock_status())
        {
        case CLOCK_SYS_HSI:
            INFO("Now system source is HSI!!\n");
            for(num = 1; num <= para2;num++){
                INFO("HSI: %d \n", num);
                delay_1us(50000);
            }
            INFO("HSI end !!\n");
				rcc_hal_sys_select_clock(CLOCK_SYS_PLLCLK1);
            break;
        case CLOCK_SYS_PLLCLK1:
        case CLOCK_SYS_PLLCLK2:
            INFO("Now system source is PLLCLK!!\n");
            for(num = 1; num <= para2;num++){
                INFO("PLLCLK: %d \n", num);
                delay_1us(50000);
            }
            INFO("PLLCLK end !!\n");
				rcc_hal_sys_select_clock(CLOCK_SYS_HSI);
            break;
        default:
            break;
        }
        delay_1us(200000);
    }
}
