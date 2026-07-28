#include <stdio.h>
#include <string.h>
#include "CMSDK_CM0.h"
#include "system_CMSDK_CM0.h"
#include "core_cm0.h"
#include "Common.h"
#include "rtc.h"



void RTC_init(void)
{
	
	
		RTC->En|=BIT0|BIT2;
		
	  RTC->int_val=1000;
	
	  RTC->En|=BIT1;
	


}




























