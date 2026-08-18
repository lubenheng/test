/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    step_motor_contorl_hardware.h
  * @brief   步进电机 TMC2209 硬件控制层（纯 STEP/DIR，不使用 UART）
  ******************************************************************************
  * @note    本文件为用户代码，与 CubeMX 生成代码分离；
  *          STM32F103C8T6 + TMC2209，STEP/DIR 硬件控制模式。
  ******************************************************************************
  */
/* USER CODE END Header */
#ifndef __STEP_MOTOR_CONTORL_HARDWARE_H
#define __STEP_MOTOR_CONTORL_HARDWARE_H

#include "main.h"

/* =====================================================================
 * TMC2209 步进电机硬件控制层（纯 STEP/DIR，不使用 UART）
 * ---------------------------------------------------------------------
 *   STEP    : TIM2_CH2 PWM，频率缓升/缓降（TIM3 中断内 ARR 逐步逼近 target_arr）
 *   DIR     : PA2，方向
 *   MS1/MS2 : PA3/PA4，微步（0/0 → 1/8）
 *   ENN     : PA0，使能（低有效）
 *   用法    : Motor_Init() → 缓升启动到默认频率；
 *             Motor_Start(hz) / Motor_Stop() 控制启停与变速
 * ===================================================================== */

/* 上电初始化并使能电机（缓升启动到默认频率） */
void Motor_Init(void);

/* 启动/变速：设置目标 STEP 频率(Hz)，ARR 缓升/缓降逼近 target_arr */
void Motor_Start(uint32_t target_hz);

/* 停止：缓降到起始低速后停止 PWM */
void Motor_Stop(void);

/* 获取当前 STEP 频率(Hz) */
uint32_t Motor_GetSpeedHz(void);

/* 查询运行状态：1=运行中，0=已停止 */
uint8_t Motor_IsRunning(void);

/* 设置方向：0=正向，1=反向（建议停机后设置） */
void Motor_SetDir(uint8_t dir);

#endif /* __STEP_MOTOR_CONTORL_HARDWARE_H */
