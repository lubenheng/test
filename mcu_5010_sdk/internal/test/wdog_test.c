#include "platform_common.h"

#include "test.h"

#include "wdog_reg.h"
#include "wdog_hal.h"
#include "wdog_test.h"
#include "cmdline_internal.h"





void wdog_func1_test(uint32_t test_case, uint32_t para1, uint32_t para2)
{

    INFO("Number: %d\n", para1);
    feed_iwdg(para1);


}	



void wdog_func2_test(uint32_t test_case, uint32_t para1, uint32_t para2)
{




}	
