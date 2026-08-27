#include "platform_common.h"
#include "device_resource.h"

#include "test.h"

#include "TIM_Reg.h"

#include "TIM_Type.h"
#include "TIM.h"
#include "TIM_hal.h"
#include "TIM_test.h"

#include "gpio_test.h"
#include "gpio_hal.h"

#include "clock_hal.h"
#include "clock.h"


//更改占空比时是否需要按百分比修改(按百分比修改时，更改门限值时，CH0、CH1对应CMP0、CMP1寄存器也会更改匹配当前门限值和占空比)
//0表示不按百分比修改，那么修改门限值时门限值小于CMP0、CMP1的值时PWM消失，需要手动输入修改CMP0、CMP1的值小于门限值时PWM才会出现
#define DUTY_SET 0




TIMx_TypeDef *time13 = TIMER13;
TIMx_TypeDef *time14 = TIMER14;
TIMx_TypeDef *time15 = TIMER15;
TIMx_TypeDef *time16 = TIMER16;
TIMx_TypeDef *timex = TIMER13;
gpio_settings_config_t gpio_init_b;

IRQn_Type IRQ_TIMx = TIM13_IRQn;

/**
 *@brief Enable timer interrupts by bit mask
 *@param[in] timex: point to timer base address (TIMER13/TIMER14/TIMER15/TIMER16)
 *@param[in] irq_mask: interrupt enable bit mask
				TIMERx_IRQ_CH0:bit0
				TIMERx_IRQ_CH1:bit1
				TIMERx_IRQ_ZERO:bit2
				TIMERx_IRQ_FAIL:bit3
 *@return    none
 */
static void tim_hal_irq_en_mask(TIMx_TypeDef *timex, uint8_t irq_mask)
{
	Timer_IRQ_Enable(timex, irq_mask);
	
}

/*------------------------统一初始化-----------------------------*/
#if 1
/**
 *@brief	 TIM 统一初始化：将计数/输入捕获/输出比较等功能的配置统一收敛到 TIMx_config_t 结构体中，
 *          需要哪些功能就在结构体中配置，本函数一次完成所有 TIM 寄存器初始化。
 *          注意：TIM 引脚的 GPIO 复用需在调用本函数前自行初始化。
 *@param[in] timex: point to timer base address (TIMER13/TIMER14/TIMER15/TIMER16)
 *@param[in] config: TIM 配置结构体指针（字段说明见 TIM_Type.h 中 TIMx_config_t 注释）
 *@return    none
 */
void TIM_hal_init(TIMx_TypeDef *timex, TIMx_config_t *config)
{
	if(timex != TIMER13 && timex != TIMER14
		&& timex != TIMER15 && timex != TIMER16){
		return;
	}

	/*----------- 计数器基本配置（TIM_CFG 寄存器） -----------*/
	Timer_Set_ClkSrc(timex, config->tim_clk_src);
	TIMER_Set_CLKDIV(timex, config->tim_div);
	Timer_Set_CNTR_Dir(timex, config->tim_cnt_dir);
	Timer_Set_Center_Mode(timex, config->tim_cnt_mode);
	Timer_Set_RLMode(timex, config->tim_reload_mode);
	Timer_Set_Shadow(timex, config->tim_shadow);
	Timer_Set_GateEN(timex, config->tim_gate_en);
	Timer_Set_EXT_ON(timex, config->tim_ext_start_en);

	/*----------- 门限值（TH 寄存器） -----------*/
	Timer_Set_ThVal(timex, config->tim_th);

	/*----------- 通道配置（FAE / CHxM / CHxPOL / 捕获） -----------*/
	timex->FAE.MOE = config->tim_moe;

	TIMER_Set_CH_Mode(timex, TIMERx_CH0, config->tim_ch0_mode);
	TIMER_Set_CH_Mode(timex, TIMERx_CH1, config->tim_ch1_mode);
	TIMER_Set_IOPOL(timex, TIMERx_CH0, config->tim_ch0_pol);
	TIMER_Set_IOPOL(timex, TIMERx_CH1, config->tim_ch1_pol);

	//CH0 捕获配置
	if(config->tim_ch0_rise_cap){
		Timer_Ch0_En_RiseCap(timex);
	}
	if(config->tim_ch0_fall_cap){
		Timer_Ch0_En_FallCap(timex);
	}
	if(config->tim_ch0_clr_en){
		Timer_Enable_Cap0_Clr(timex);
	}
	TIMER_Set_InputSrc(timex, TIMERx_CH0, config->tim_ch0_cap_src);

	//CH1 捕获配置
	if(config->tim_ch1_rise_cap){
		Timer_Ch1_En_RiseCap(timex);
	}
	if(config->tim_ch1_fall_cap){
		Timer_Ch1_En_FallCap(timex);
	}
	if(config->tim_ch1_clr_en){
		Timer_Enable_Cap1_Clr(timex);
	}
	TIMER_Set_InputSrc(timex, TIMERx_CH1, config->tim_ch1_cap_src);

	/*----------- 比较值（CMP0/CMP1 寄存器） -----------*/
	Timer_Set_Comp0_Val(timex, config->tim_cmp0);
	Timer_Set_Comp1_Val(timex, config->tim_cmp1);

	/*----------- 事件源 / 滤波（非 0 才配置，0 为默认不生效） -----------*/
	if(config->tim_evt_src){
		Timer_Set_EvtSrc(timex, config->tim_evt_src);
	}
	if(config->tim_filt){
		Timer_Set_FiltCoeff(timex, config->tim_filt);
	}

	/*----------- 中断配置（tim_irq_en 非 0 才使能 NVIC） -----------*/
	if(config->tim_irq_en){
		tim_hal_irq_en_mask(timex, config->tim_irq_en);
		NVIC_SetPriority(IRQ_TIMx, 2); /*TIMER13优先级配置 */
		NVIC_ClearPendingIRQ(IRQ_TIMx);  /*清除之前的配置*/
		NVIC_EnableIRQ (IRQ_TIMx);       /*使能中断*/
	}

	/*----------- 使能定时器 -----------*/
	Timer_Enable(timex);
}



/**
*@brief Timer clock division
*@param[in] div:
				TIMER_DIV_1			TIMER_DIV_16
				TIMER_DIV_2			TIMER_DIV_32
				TIMER_DIV_4			TIMER_DIV_64
				TIMER_DIV_8			TIMER_DIV_128
*@return    none
*/
void TIM_hal_Counter_set_div(TIMx_TypeDef *timex, uint8_t div)
{
	if(div > 7){
		return;
	}
	TIMER_Set_CLKDIV(timex, div);
}

/**
*@brief Get the counter's count value
*@param[in] none
*@return    count value
*/
uint32_t TIM_hal_Counter_num(TIMx_TypeDef *timex)
{
	return (timex->CNT);
}

/**
*@brief Change the counter's threshold value (period value)
*@param[in] timex: point to timer base address (TIMER13/TIMER14/TIMER15/TIMER16)
*@param[in] th:Custom threshold value: 0 ~ 2^32
*@return    none
*/
void tim_hal_th_set(TIMx_TypeDef *timex, uint32_t th)
{
	uint8_t num1 = tim_hal_ch0_duty(timex) * 100;
	uint8_t num2 = tim_hal_ch1_duty(timex) * 100;

	//INFO("xxx:%d  yyy:%d", num1, num2);

	Timer_Set_ThVal(timex, th);

#if DUTY_SET
	tim_hal_compn_set(timex, 0, num1);
	tim_hal_compn_set(timex, 1, num2);
#endif

}

/**
*@brief Select the counting direction of the counter
*@param[in] timex: point to timer base address (TIMER13/TIMER14/TIMER15/TIMER16)
*@param[in] dir:	0:Count Up		1:Count down
*@return    none
*/
void tim_hal_cntdir_set(TIMx_TypeDef *timex, uint8_t dir)
{
	if(0 == dir){
		Timer_Set_CNTR_Dir(timex,TIMERx_CNTR_UP);
	} else if(1 == dir){
		Timer_Set_CNTR_Dir(timex,TIMERx_CNTR_DOWN);
	}
}

#endif


/*------------------------捕获-----------------------------*/
#if 1
/* 捕获初始化统一由 TIM_hal_init(timex, config) 完成 */


/**
*@brief In capture mode, obtain the value when a capture occurs on ch0
*@param[in] none
*@return    ch0 capture value
*/
uint32_t tim_hal_ch0_capnum(TIMx_TypeDef *timex)
{
	return (timex->CMP0);
}

/**
*@brief In capture mode, obtain the value when a capture occurs on ch1
*@param[in] timex: point to timer base address (TIMER13/TIMER14/TIMER15/TIMER16)
*@return    ch1 capture value
*/
uint32_t tim_hal_ch1_capnum(TIMx_TypeDef *timex)
{
	return (timex->CMP1);
}

#endif

/*------------------------比较-----------------------------*/
#if 1
/* 比较初始化统一由 TIM_hal_init(timex, config) 完成 */

/**
 *@brief	 Change the duty cycle of the selected channel
 *@param[in] chx:Specified channel	(0:ch0   1:ch1)
 *@param[in] duty: Duty cycle value (1~100)
 *@return	 none
 */
void tim_hal_compn_set(TIMx_TypeDef *timex, uint8_t chx, uint32_t duty)
{

#if DUTY_SET
	uint32_t th = timex->TH + 1;
	if(0 == chx){
		Timer_Set_Comp0_Val(timex, th * duty / 100);
		Timer_Set_Comp0_Val(timex, duty);
	}
	if(1 == chx){
		Timer_Set_Comp1_Val(timex, th * duty / 100);
		Timer_Set_Comp1_Val(timex, duty);
	}
#else
	if(0 == chx){
		Timer_Set_Comp0_Val(timex, duty);
	}
	if(1 == chx){
		Timer_Set_Comp1_Val(timex, duty);
	}
#endif
}

/**
 *@brief	 Change the initial level of the specified channel
 *@param[in] chx:Specified channel	(0:ch0   1:ch1)
 *@param[in] pol: Duty cycle value (0 or 1)
 *@return	 none
 */
void tim_hal_compn_pol_set(TIMx_TypeDef *timex, uint8_t chx, uint8_t pol)
{
	if(0 == chx){
		TIMER_Set_IOPOL(timex,TIMERx_CH0,pol);
	}
	if(1 == chx){
		TIMER_Set_IOPOL(timex,TIMERx_CH1,pol);
	}
}

/**
 *@brief	 Select the counting mode of the counter
 *@param[in] mid:	0:Edge counting		1:central counting
 *@return	 none
 */
void tim_hal_cnt_mid_set(TIMx_TypeDef *timex, uint8_t mid)
{
	if(0 == mid){
		Timer_Set_Center_Mode(timex, TIMERx_EDGE_MODE);
	} else if(5 == mid){
		Timer_Set_Center_Mode(timex, TIMERx_SYMMETRIC_MODE);
	}
}

/**
 *@brief	 Open one_trig mode
 *@param[in] none
 *@return	 none
 */
void tim_hal_onetrig(TIMx_TypeDef *timex)
{
	Timer_Single_Trig(timex);
}

/**
 *@brief Calculate the duty cycle of the PWM output on CH0
 *@param[in] none
 *@return    duty value
 */
float tim_hal_ch0_duty(TIMx_TypeDef *timex)
{
	uint32_t th = timex->TH + 1;
	uint32_t cmp = timex->CMP0;
	return ((float)cmp / (float)th);
}

/**
 *@brief Calculate the duty cycle of the PWM output on CH1
 *@param[in] none
 *@return    duty value
 */
float tim_hal_ch1_duty(TIMx_TypeDef *timex)
{
	uint32_t th = timex->TH + 1;
	uint32_t cmp = timex->CMP1;
	return ((float)cmp / (float)th);
}

#endif

/*------------------------混合-----------------------------*/
/* 混合模式初始化统一由 TIM_hal_init(timex, config) 完成 */

/*--------------------common test func-------------------------*/
#if 1

/**
 *@brief	 Change timer
 *@param[in] timx:
				0:TIMER13
				1:TIMER14
				2:TIMER15
				3:TIMER16
 *@return	 none
 */
int8_t tim_hal_change(uint8_t timx)
{
	/*
	timex->TIM_CFG.word &= 0x0;
	timex->TH &= 0x0;
	timex->CMP0 &= 0x0;
	timex->CMP1 &= 0x0;
	timex->CNT &= 0x0;
	timex->EVTSRC &= 0x0;
	timex->FILT &= 0x0;
	timex->IER.word &= 0x0;
	timex->IFR.word &= 0x0;
	*/
	switch (timx)
	{
	case 0:
		
		timex = time13;
		IRQ_TIMx = TIM13_IRQn;
		return 0;
	case 1:
		timex = time14;
		IRQ_TIMx = TIM14_IRQn;
		return 0;
	case 2:
		timex = time15;
		IRQ_TIMx = TIM15_IRQn;
		return 0;
	case 3:
		timex = time16;
		IRQ_TIMx = TIM16_IRQn;
		return 0;
	default:
		return -1;
	}
}

/**
 *@brief	 Get all timer interrupt flags
 *@param[in] none
 *@return    all flag,Flag bits corresponding to the return value
 			 bit0:TIMERx_IRQ_FLAG_CH0
			 bit1:TIMERx_IRQ_FLAG_CH1
			 bit2:TIMERx_IRQ_FLAG_ZERO
			 bit3:TIMERx_IRQ_FLAG_FAIL
 */
uint8_t tim_hal_irq_status(TIMx_TypeDef *timex)
{
	return (Timer_drv_IRQ_Status_Get(timex));
}

uint8_t tim_hal_irq_en_status(TIMx_TypeDef *timex)
{
	return (Timer_drv_IRQ_Status_En_Get(timex));
}

/**
 *@brief	 Disable all TIM interrupts and clear the flag bits
 *@param[in] timex: point to timer base address (TIMER13/TIMER14/TIMER15/TIMER16)
 *@return    none
 */
void tim_hal_irq_disable(TIMx_TypeDef *timex)
{
	Timer_drv_IRQ_Disable(timex);
}

/**
 *@brief	 Reset/clear all settings of the TIM（重新初始化前调用）
 *@param[in] timex: point to timer base address (TIMER13/TIMER14/TIMER15/TIMER16)
 *@return    none
 */
void tim_hal_reset(TIMx_TypeDef *timex)
{
	Timer_drv_Setting_Clean(timex);
}

/**
 *@brief	 Disable TIM
 *@param[in] timex: point to timer base address (TIMER13/TIMER14/TIMER15/TIMER16)
 *@return    none
 */
void tim_hal_disable(TIMx_TypeDef *timex)
{
	Timer_Disable(timex);
}

/**
 *@brief	 enable TIM
 *@param[in] timex: point to timer base address (TIMER13/TIMER14/TIMER15/TIMER16)
 *@return    none
 */
void tim_hal_enable(TIMx_TypeDef *timex)
{
	Timer_Enable(timex);
}

/**
 *@brief	 Display the basic parameters of the timer
 *@param[in] timex: point to timer base address (TIMER13/TIMER14/TIMER15/TIMER16)
 *@return    none
 */
void tim_hal_para_display(TIMx_TypeDef *timex)
{
	INFO("div:%d th:%d cmp0:%d cmp1:%d dt1:%f dt2:%f\n",
		timex->TIM_CFG.CKDIV, timex->TH, timex->CMP0, timex->CMP1, tim_hal_ch0_duty(timex), tim_hal_ch1_duty(timex));
}

/**
 *@brief	 Change the input filter width
 *@param[in] timex: point to timer base address (TIMER13/TIMER14/TIMER15/TIMER16)
 *@param[in] flt: 0 ~ 255
 *@return    none
 */
void tim_hal_flt(TIMx_TypeDef *timex, uint8_t flt)
{
	Timer_Set_FiltCoeff(timex, flt);
}

void tim_hal_cnt_set(TIMx_TypeDef *timex, uint32_t cnt_num)
{
	timex->CNT = cnt_num;
}

#endif


// pjh add   timer13 ch01
void HAL_TIM_DMA_compare_Init(void)
{	
	gpio_init_b.gpio_port = GPIO_B;
	gpio_init_b.gpio_pin = GPIO_PIN_11;
	gpio_init_b.gpio_alt = GPIO_ALT_FUNC_10;
	gpio_init_b.gpio_dir = GPIO_DIR_OUTPUT;
	gpio_init_b.pull_cfg = GPIO_PULL_DISABLE;
	gpio_init_b.gpio_strength = GPIO_STRENGTH_MODE_1;
	gpio_init_b.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&gpio_init_b);

	gpio_init_b.gpio_port = GPIO_B;
	gpio_init_b.gpio_pin = GPIO_PIN_12;
	gpio_init_b.gpio_alt = GPIO_ALT_FUNC_10;
	gpio_init_b.gpio_dir = GPIO_DIR_OUTPUT;
	gpio_init_b.pull_cfg = GPIO_PULL_DISABLE;
	gpio_init_b.gpio_strength = GPIO_STRENGTH_MODE_1;
	gpio_init_b.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&gpio_init_b);
//============================================================//
	gpio_init_b.gpio_port = GPIO_B;
	gpio_init_b.gpio_pin = GPIO_PIN_13;
	gpio_init_b.gpio_alt = GPIO_ALT_FUNC_10;
	gpio_init_b.gpio_dir = GPIO_DIR_OUTPUT;
	gpio_init_b.pull_cfg = GPIO_PULL_DISABLE;
	gpio_init_b.gpio_strength = GPIO_STRENGTH_MODE_1;
	gpio_init_b.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&gpio_init_b);

	gpio_init_b.gpio_port = GPIO_B;
	gpio_init_b.gpio_pin = GPIO_PIN_14;
	gpio_init_b.gpio_alt = GPIO_ALT_FUNC_10;
	gpio_init_b.gpio_dir = GPIO_DIR_OUTPUT;
	gpio_init_b.pull_cfg = GPIO_PULL_DISABLE;
	gpio_init_b.gpio_strength = GPIO_STRENGTH_MODE_1;
	gpio_init_b.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&gpio_init_b);
//============================================================//	
	gpio_init_b.gpio_port = GPIO_B;
	gpio_init_b.gpio_pin = GPIO_PIN_15;
	gpio_init_b.gpio_alt = GPIO_ALT_FUNC_10;
	gpio_init_b.gpio_dir = GPIO_DIR_OUTPUT;
	gpio_init_b.pull_cfg = GPIO_PULL_DISABLE;
	gpio_init_b.gpio_strength = GPIO_STRENGTH_MODE_1;
	gpio_init_b.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&gpio_init_b);

	gpio_init_b.gpio_port = GPIO_C;
	gpio_init_b.gpio_pin = GPIO_PIN_0;
	gpio_init_b.gpio_alt = GPIO_ALT_FUNC_10;
	gpio_init_b.gpio_dir = GPIO_DIR_OUTPUT;
	gpio_init_b.pull_cfg = GPIO_PULL_DISABLE;
	gpio_init_b.gpio_strength = GPIO_STRENGTH_MODE_1;
	gpio_init_b.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&gpio_init_b);
	//============================================================//	
	gpio_init_b.gpio_port = GPIO_C;
	gpio_init_b.gpio_pin = GPIO_PIN_1;
	gpio_init_b.gpio_alt = GPIO_ALT_FUNC_10;
	gpio_init_b.gpio_dir = GPIO_DIR_OUTPUT;
	gpio_init_b.pull_cfg = GPIO_PULL_DISABLE;
	gpio_init_b.gpio_strength = GPIO_STRENGTH_MODE_1;
	gpio_init_b.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&gpio_init_b);

	gpio_init_b.gpio_port = GPIO_C;
	gpio_init_b.gpio_pin = GPIO_PIN_2;
	gpio_init_b.gpio_alt = GPIO_ALT_FUNC_10;
	gpio_init_b.gpio_dir = GPIO_DIR_OUTPUT;
	gpio_init_b.pull_cfg = GPIO_PULL_DISABLE;
	gpio_init_b.gpio_strength = GPIO_STRENGTH_MODE_1;
	gpio_init_b.gpio_interrupt_cfg_info.int_handle = NULL;
	
	
	gpio_init(&gpio_init_b);
	
	
	TIMER13 ->FAE.MOE = 1;   //使能通道01
	
	TIMER_Set_CH_Mode(TIMER13, TIMERx_CH0, TIMERx_MODE_PWM);
	TIMER_Set_CH_Mode(TIMER13, TIMERx_CH1, TIMERx_MODE_PWM);  //设置通道01 为pwm模式
	TIMER_Set_IOPOL(TIMER13,TIMERx_CH0,TIMERx_IOPOL_POSITIVE); //极性  cnt<comp 为正
	TIMER_Set_IOPOL(TIMER13,TIMERx_CH1,TIMERx_IOPOL_POSITIVE);
	TIMER_Set_CLKDIV(TIMER13,TIMER_DIV_32);  //32Mhz / 32分频
	Timer_Set_ThVal(TIMER13, 1000);					 //1000Hz	  TH
	
	Timer_Set_Comp0_Val(TIMER13, 0x190);  //cnt < 500输出正极 50%  CMP0
	Timer_Set_Comp1_Val(TIMER13, 100);  //cnt <700 正极		70%
	Timer_Enable(TIMER13);
	
	TIMER14 ->FAE.MOE = 1;   //使能通道01
	
	TIMER_Set_CH_Mode(TIMER14, TIMERx_CH0, TIMERx_MODE_PWM);
	TIMER_Set_CH_Mode(TIMER14, TIMERx_CH1, TIMERx_MODE_PWM);  //设置通道01 为pwm模式
	TIMER_Set_IOPOL(TIMER14,TIMERx_CH0,TIMERx_IOPOL_POSITIVE); //极性  cnt<comp 为正
	TIMER_Set_IOPOL(TIMER14,TIMERx_CH1,TIMERx_IOPOL_POSITIVE);
	TIMER_Set_CLKDIV(TIMER14,TIMER_DIV_32);  //32Mhz / 32分频
	Timer_Set_ThVal(TIMER14, 1000);					 //1000Hz	  TH
	
	Timer_Set_Comp0_Val(TIMER14, 100);  //cnt < 500输出正极 50%  CMP0
	Timer_Set_Comp1_Val(TIMER14, 100);  //cnt <700 正极		70%
	Timer_Enable(TIMER14);
	
	TIMER15 ->FAE.MOE = 1;   //使能通道01
	
	TIMER_Set_CH_Mode(TIMER15, TIMERx_CH0, TIMERx_MODE_PWM);
	TIMER_Set_CH_Mode(TIMER15, TIMERx_CH1, TIMERx_MODE_PWM);  //设置通道01 为pwm模式
	TIMER_Set_IOPOL(TIMER15,TIMERx_CH0,TIMERx_IOPOL_POSITIVE); //极性  cnt<comp 为正
	TIMER_Set_IOPOL(TIMER15,TIMERx_CH1,TIMERx_IOPOL_POSITIVE);
	TIMER_Set_CLKDIV(TIMER15,TIMER_DIV_32);  //32Mhz / 32分频
	Timer_Set_ThVal(TIMER15, 1000);					 //1000Hz	  TH
	
	Timer_Set_Comp0_Val(TIMER15, 100);  //cnt < 500输出正极 50%  CMP0
	Timer_Set_Comp1_Val(TIMER15, 100);  //cnt <700 正极		70%
	Timer_Enable(TIMER15);
	
	TIMER16 ->FAE.MOE = 1;   //使能通道01
	
	TIMER_Set_CH_Mode(TIMER16, TIMERx_CH0, TIMERx_MODE_PWM);
	TIMER_Set_CH_Mode(TIMER16, TIMERx_CH1, TIMERx_MODE_PWM);  //设置通道01 为pwm模式
	TIMER_Set_IOPOL(TIMER16,TIMERx_CH0,TIMERx_IOPOL_POSITIVE); //极性  cnt<comp 为正
	TIMER_Set_IOPOL(TIMER16,TIMERx_CH1,TIMERx_IOPOL_POSITIVE);
	TIMER_Set_CLKDIV(TIMER16,TIMER_DIV_32);  //32Mhz / 32分频
	Timer_Set_ThVal(TIMER16, 1000);					 //1000Hz	  TH
	
	Timer_Set_Comp0_Val(TIMER16, 100);  //cnt < 500输出正极 50%  CMP0
	Timer_Set_Comp1_Val(TIMER16, 100);  //cnt <700 正极		70%
	Timer_Enable(TIMER16);
}

//B12 捕获	 B14pwm
void HAL_TIM_caputer_Init(void)
{
	gpio_init_b.gpio_port = GPIO_B;
	gpio_init_b.gpio_pin = GPIO_PIN_11;
	gpio_init_b.gpio_alt = GPIO_ALT_FUNC_10;
	gpio_init_b.gpio_dir = GPIO_DIR_OUTPUT;
	gpio_init_b.pull_cfg = GPIO_PULL_DISABLE;
	gpio_init_b.gpio_strength = GPIO_STRENGTH_MODE_1;
	gpio_init_b.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&gpio_init_b);

	gpio_init_b.gpio_port = GPIO_B;
	gpio_init_b.gpio_pin = GPIO_PIN_12;
	gpio_init_b.gpio_alt = GPIO_ALT_FUNC_10;
	gpio_init_b.gpio_dir = GPIO_DIR_INPUT;
	gpio_init_b.pull_cfg = GPIO_PULL_DISABLE;
	gpio_init_b.gpio_strength = GPIO_STRENGTH_MODE_1;
	gpio_init_b.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&gpio_init_b);

	TIMER13 ->FAE.MOE = 1;
	
	TIMER_Set_CH_Mode(TIMER13, TIMERx_CH0, TIMERx_MODE_PWM);
	TIMER_Set_IOPOL(TIMER13,TIMERx_CH0,TIMERx_IOPOL_POSITIVE);
	Timer_Set_Comp0_Val(TIMER13, 50000);


	TIMER_Set_CH_Mode(TIMER13, TIMERx_CH1, TIMERx_MODE_CAP);
	Timer_Ch1_En_FallCap(TIMER13);
	TIMER_Set_InputSrc(TIMER13, TIMERx_CH1, TIMERx_SRC_IO1);
	
	TIMER_Set_CLKDIV(TIMER13,TIMER_DIV_32);
	Timer_Set_ThVal(TIMER13, 999999);
	
 
	Timer_Enable(timex);
}

