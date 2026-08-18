/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  
  *  硬件连接（见 TMC2209_STM32F103_控制方案分析.md）：
  *    PA0 → ENN     使能（低有效）
  *    PA1 → STEP    TIM2_CH2 PWM（频率可调）
  *    PA2 → DIR     方向
  *    PA3 → MS1     微步 LSB（0）
  *    PA4 → MS2     微步 MSB（0）
  *    PDN_UART → GND（不使用 UART，启用静止自动降流）
  *    VREF 开路（=2.5V 满量程电流），RSENSE 按电机选型
  *
  *  
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "step_motor_contorl_hardware.h"
#include "uart_control.h"   /* 使用 MOTOR_CTRL_MODE 控制方式宏 */

/* USER CODE BEGIN Private define */

#define MOTOR_TIM_CLK_HZ   72000000u   /* TIM2 时钟（72MHz） */
#define MOTOR_TIM_PSC      99u         /* TIM2 实际预分频（CubeMX 配置，勿改） */

/* 频率换算（计入 PSC=99）：实际 f = 72M/((PSC+1)*(ARR+1)) = 72M/(100*(ARR+1))
 * ARR = 72M/(100*f) - 1；f = 72M/(100*(ARR+1)) */
#define MOTOR_FREQ_TO_ARR(hz)  ((uint32_t)(MOTOR_TIM_CLK_HZ / ((MOTOR_TIM_PSC + 1u) * (uint32_t)(hz)) - 1u))
#define MOTOR_ARR_TO_FREQ(arr) ((uint32_t)(MOTOR_TIM_CLK_HZ / ((MOTOR_TIM_PSC + 1u) * ((uint32_t)(arr) + 1u))))

#define MOTOR_START_HZ     20u        /* 缓升起始频率(Hz)（≥12Hz 避开 MicroPlyer 静止判定） */
#define MOTOR_MAX_HZ_LIMIT 1000u      /* 最高频率保护 */
#define ARR_STEP           5u      /* 每次中断 ARR 调整步长（约 0.35s 完成 20→800Hz） */

// 速度控制变量
volatile uint32_t current_arr = MOTOR_FREQ_TO_ARR(MOTOR_START_HZ);  /* 当前 ARR */
volatile uint32_t target_arr  = MOTOR_FREQ_TO_ARR(MOTOR_START_HZ);  /* 目标 ARR（初始=起始） */
volatile uint8_t  ramp_active = 0;      /* 缓升标志 */
volatile uint8_t  stop_pending = 0;     /* 停机请求 */

/* USER CODE END Private define */

/* USER CODE BEGIN 0 */



/* USER CODE END 0 */



/**
  * @brief  缓升启动/变速：设置目标频率并缓升逼近
  * @param  target_hz 目标 STEP 频率(Hz)
  * @retval None
  */
void Motor_Start(uint32_t target_hz)
{
  if (target_hz < MOTOR_START_HZ)
  {
    target_hz = MOTOR_START_HZ;
  }
  if (target_hz > MOTOR_MAX_HZ_LIMIT)
  {
    target_hz = MOTOR_MAX_HZ_LIMIT;
  }
  target_arr = MOTOR_FREQ_TO_ARR(target_hz);
  stop_pending = 0u;
  ramp_active = 1u;

  if (!(TIM2->CR1 & TIM_CR1_CEN))
  {
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);   /* 仅停止状态才启动 PWM */
  }
  HAL_TIM_Base_Start_IT(&htim3);                /* 启动/恢复缓升节拍 */
}

/**
  * @brief  缓降停止：缓降到起始低速后停机
  * @retval None
  */
void Motor_Stop(void)
{
  target_arr = MOTOR_FREQ_TO_ARR(MOTOR_START_HZ);
  stop_pending = 1u;
  ramp_active = 1u;
  HAL_TIM_Base_Start_IT(&htim3);
}

/**
  * @brief  获取当前 STEP 频率（计入 PSC=99）
  * @retval 当前频率(Hz)
  */
uint32_t Motor_GetSpeedHz(void)
{
  return MOTOR_ARR_TO_FREQ(current_arr);
}

/**
  * @brief  查询电机运行状态
  * @retval 1=运行中，0=已停止
  */
uint8_t Motor_IsRunning(void)
{
  return (TIM2->CR1 & TIM_CR1_CEN) ? 1u : 0u;
}

/**
  * @brief  设置方向
  * @param  dir 0=正向（DIR=0），1=反向（DIR=1）
  * @retval None
  */
void Motor_SetDir(uint8_t dir)
{
  HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin,
                    (dir != 0u) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

/**
  * @brief  上电初始化 TMC2209 相关引脚并缓升启动电机
  * @note   不接 UART，仅用默认配置（1/8 微步、VREF 开路）。
  *         设置方向/微步/使能后，缓升启动到 MOTOR_MAX_HZ_DEFAULT。
  * @retval None
  */
void Motor_Init(void)
{
  /* . 禁用电机（ENN 低有效；CubeMX 已把 PA0 初始化为高=禁用） */
  HAL_GPIO_WritePin(ENN_GPIO_Port, ENN_Pin, GPIO_PIN_SET);
  /* 1. 方向：默认正向（DIR=0） */
  HAL_GPIO_WritePin(DIR_GPIO_Port, DIR_Pin, GPIO_PIN_RESET);

  /* 2. 微步分辨率：1/8 微步（MS1=0, MS2=0），与 CubeMX 默认一致 */
  HAL_GPIO_WritePin(MS1_GPIO_Port, MS1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(MS2_GPIO_Port, MS2_Pin, GPIO_PIN_RESET);

  /* 3. 使能电机（ENN 低有效；CubeMX 已把 PA0 初始化为高=禁用） */
  HAL_GPIO_WritePin(ENN_GPIO_Port, ENN_Pin, GPIO_PIN_RESET);
#if (MOTOR_CTRL_MODE == CTRL_MODE_LOCAL)
  /* 本地控制：启动 TIM2 PWM + TIM3 缓升 */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_Base_Start_IT(&htim3);
#else
  /* UART 控制：仅使能，等待上位机 START 指令
     （UART_Control_Task → Motor_Start 中会启动 PWM + 缓升） */
#endif
}

/* USER CODE BEGIN 1 */

/**
  * @brief  HAL 定时器周期回调：TIM3 提供缓升/缓降节拍
  * @note   在中断内将 current_arr 以 ARR_STEP 步长向 target_arr 逼近，
  *         并同步更新 TIM2 ARR/CCR（CH2，50%），EGR.UG 立即生效。
  * @param  htim 定时器句柄
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	
	
if (htim->Instance == TIM3)
	{
        if (ramp_active) {
            // 缓升/缓降逻辑
            if (current_arr > target_arr) 
				{
                if (current_arr - ARR_STEP > target_arr)
                    current_arr -= ARR_STEP;
                else
                    current_arr = target_arr;
            } else if (current_arr < target_arr) 
				{
                if (current_arr + ARR_STEP < target_arr)
                    current_arr += ARR_STEP;
                else
                    current_arr = target_arr;
            } else {
                /* 已到达目标频率 */
                ramp_active = 0;
                if (stop_pending) {
                    /* 缓降到起始低速后停机 */
                    stop_pending = 0;
                    HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
                }
                /* 到达目标（含停机完成）→ 停止缓升节拍，省 CPU；
                   下次 Motor_Start/Stop 会重新 Start_IT */
                HAL_TIM_Base_Stop_IT(&htim3);
            }
            // 更新 TIM2 的 ARR 和 CCR（保持 50% 占空比）
            __HAL_TIM_SET_AUTORELOAD(&htim2, current_arr);
            __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, current_arr / 2);
        }
    }

}

/* USER CODE END 1 */
