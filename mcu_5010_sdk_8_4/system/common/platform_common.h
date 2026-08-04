#ifndef PLATFORM_COMMOM_H
#define PLATFORM_COMMOM_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "sys_config.h"
//#include "core_cm0plus.h" 
//#include "system_ARMCM0plus.h"

#include "ARMCM0plus.h"



#include "types.h"
#include "utility.h"
#include "version.h"
#include "status.h"
#include "dev_assert.h"
#include "console.h"

#include "clock_hal.h"
#include "clock_hal_cfg.h"
#if defined (USE_GCC) || defined (USE_GHS)
#include "sy_printf.h"
#endif

#include "device_resource.h"
#if   defined ( __CC_ARM )
#define SY_WEAK __WEAK
#elif defined ( __ICCARM__ )
#define SY_WEAK __WEAK
#elif defined ( __GNUC__ )
#define SY_WEAK __WEAK
#elif defined ( __ghs__ )
#define SY_WEAK __WEAK
#else
#error "invalid compiler"
#endif

#endif /* PLATFORM_COMMOM_H */
 
