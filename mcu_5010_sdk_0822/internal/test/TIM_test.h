#ifndef TIMx_TEST_H
#define TIMx_TEST_H

#define PWMOUT_TIMER      (TIMER_TIM13)
#define PWM_TIMER_CH      (TIMERx_CH0)
#define PWM_TIMER_DIV      (TIMER_DIV_1)
#define PWM_FRE      			(1000)
#define PWM_POL      			(1000)

#define CAP_TIMER      (TIMER_TIM14)
#define CAP_TIMER_CH      (TIMERx_CH0)



void TIM_func1_test(uint32_t test_case, uint32_t para1, uint32_t para2);
void TIM_func2_test(uint32_t test_case, uint32_t para1, uint32_t para2);


typedef enum{
    TIM_TEST_CHANGE_DIV = 0U,
    TIM_TEST_CHANGE_CYCLE,
    TIM_TEST_CHANGE_DUTY,
}TIM_Test_change_Type;




#endif

