#ifndef MCPWM_TEST_H
#define MCPWM_TEST_H

#include "mcpwm_type.h"

/****************GPIO Init********************/
void mcpwm0_gpio_init(void);
void mcpwm1_gpio_init(void);



/****************MCPWM0********************/
void mcpwm0_callback_duty_test(mcpwm0_int_source_e source);
void mcpwm_func1_test(uint32_t test_case, uint32_t para1, uint32_t para2);
void mcpwm_func2_test(uint32_t test_case, uint32_t para1, uint32_t para2);



/****************MCPWM1********************/
void mcpwm1_callback_duty_test(mcpwm1_int_source_e source);



/****************MCPWM cmdline test********************/
void mcpwm_soft_init_copy(void);
void mcpwm_parameter_set(int argc, char *argv[]); /* 图 14-7MCPWM 时序 TH<n>0 和 TH<n>1-中心对齐模式 */
void mcpwm_center_align_test(int argc, char *argv[]); /* 图 14-7MCPWM 时序 TH<n>0 和 TH<n>1-中心对齐模式 */
void mcpwm_center_align_push_pull_test(int argc, char *argv[]);/* 图 14-8 MCPWM 时序 TH<n>0 和 TH<n>1-中心对齐推挽模式 */
void mcpwm_edge_align_test(int argc, char *argv[]);/* 图 14-9MCPWM 时序边沿对齐模式 */
void mcpwm_edge_align_push_pull_test(int argc, char *argv[]);/* 图 14-10MCPWM 时序 TH<n>0 和 TH<n>1 边沿对齐推挽模式 */

/****************BLDC cmdline test********************/
void mcpwm0_HPWM_LON_openloop_test(void);
void mcpwm0_complement_HPWM_LON_openloop_test(void);
void mcpwm0_HON_LPWM_openloop_test(void);
void mcpwm0_complement_HON_LPWM_openloop_test(void);

void mcpwm0_bldc_hall_uvw_test(uint16_t hall_motor_enbale, uint32_t motor_sector, uint16_t duty);/* 有感HALL 控制通过HALL硬件模块中断进行BLDC电机控制 */


void mcpwm0_HPWM_LON_openloop_mask_test(void); //????
void mcpwm0_complement_HPWM_LON_openloop_mask_test(void);
void mcpwm0_HON_LPWM_openloop_mask_test(void);
void mcpwm0_complement_HON_LPWM_openloop_mask_test(void);


void mcpwm0_HPWM_LON_hall_uvw_test(void);

void mcpwm_mask_control_combine(void);

/****************中断扫描测试********************/
void mcpwm0_interrupt_test_callback(mcpwm0_int_source_e source);
void mcpwm1_interrupt_test_callback(mcpwm1_int_source_e source);

void mcpwm_interrupt_source_test(int argc, char *argv[]);
void mcpwm_channel_n_p_inverse_test(int argc, char *argv[]);
void mcpwm_channel_swap_test(int argc, char *argv[]);
void mcpwm_output_default_value_test(int argc, char *argv[]);

/****************MCPWM故障输入********************/
void mcpwm_io_fault_test(int argc, char *argv[]);

#endif

