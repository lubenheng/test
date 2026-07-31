#ifndef WDOG_HAL_H
#define WDOG_HAL_H


#include "wdog_type.h"

void iwdg_init(uint32_t ms);
void feed_iwdg(uint32_t ms);
uint8_t iwdg_status(void);
uint32_t iwdg_counts(void);

#endif
