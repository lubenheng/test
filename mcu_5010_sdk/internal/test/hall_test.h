#ifndef HALL_TEST_H
#define HALL_TEST_H

#include "hall_reg.h"
#include "hall_type.h"

/****************GPIO Init********************/
void hall_gpio_init(void);
void hall_debug_gpio_init(void);

void g_motor_ctrl_hall_callback(hall_int_source_e source);


void hall_func1_test(uint32_t test_case, uint32_t para1, uint32_t para2);
void hall_func2_test(uint32_t test_case, uint32_t para1, uint32_t para2);
void hall_software_trigger_interupt_test(void);
void hall_soft_trig_start_test(void);

#endif

