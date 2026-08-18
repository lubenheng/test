/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    uart_control.h
  * @brief   UART 控制层：USART1 读写 TMC2209 寄存器 + USART3 上位机指令
  ******************************************************************************
  * @note    用户代码，与 CubeMX 生成代码分离。
  ******************************************************************************
  */
/* USER CODE END Header */
#ifndef __UART_CONTROL_H
#define __UART_CONTROL_H

#include "main.h"

/* =====================================================================
 * 控制方式宏（切换本地/上位机控制）
 * ===================================================================== */
#define CTRL_MODE_LOCAL   0u   /* 本地：上电自动缓升启动 */
#define CTRL_MODE_UART    1u   /* 上位机：等待 USART3 指令控制 */
#define MOTOR_CTRL_MODE   CTRL_MODE_UART   /* ← 在此选择控制方式 */

/* =====================================================================
 * 运动控制方式选择（影响电流阶梯行为）
 * ===================================================================== */
#define TMC2209_MOVE_VACTUAL  0u   /* A：VACTUAL 内部脉冲(0x22)，电流恒 256 细分(平滑) */
#define TMC2209_MOVE_STEP     1u   /* B：STEP 引脚(PA1/TIM2 PWM)+intpol=0，电流阶梯=MRES */
#define TMC2209_MOVE_MODE     TMC2209_MOVE_STEP   /* ← 在此选择运动控制方式 */

/* =====================================================================
 * TMC2209 单线 UART（USART1）参数
 * ===================================================================== */
#define TMC2209_UART_EN   1u          /* 1=用 USART1 配置 TMC2209；0=仅硬件模式 */
#define TMC2209_BAUD      115200u     /* USART1 波特率（TMC2209 自动检测） */
#define TMC2209_ADDR      0u          /* 从机地址 = MS1 + 2*MS2（当前 0） */

/* =====================================================================
 * 上位机（USART3）协议参数
 * ===================================================================== */
#define UART_CMD_BUF_SIZE 32u         /* 指令缓冲长度 */

/* =====================================================================
 * TMC2209 寄存器快照（全部可读寄存器）
 * ===================================================================== */
typedef struct
{
  uint32_t gconf;      /* 0x00 GCONF       RW */
  uint32_t gstat;      /* 0x01 GSTAT       R/WC */
  uint32_t ifcnt;      /* 0x02 IFCNT       R  */
  uint32_t ioin;       /* 0x06 IOIN        R  */
  uint32_t tstep;      /* 0x12 TSTEP       R  */
  uint32_t sg_result;  /* 0x41 SG_RESULT   R  */
  uint32_t chopconf;   /* 0x6C CHOPCONF    RW */
  uint32_t drv_status; /* 0x6F DRV_STATUS  R  */
  uint32_t pwmconf;    /* 0x70 PWMCONF     RW */
} TMC2209_RegSnapshot_t;

/* =====================================================================
 * 接口
 * ===================================================================== */
void UART_Control_Init(void);                 /* 初始化：USART1 波特率 + TMC2209 配置 + USART3 就绪 */
void UART_Control_Task(void);                 /* 主循环轮询：解析 USART3 上位机指令 */
void UART_Control_SendString(const char *s);  /* USART3 发送（上位机） */

/* ---- TMC2209 寄存器读写 ---- */
uint8_t TMC2209_WriteReg(uint8_t reg, uint32_t val);
uint8_t TMC2209_ReadReg(uint8_t reg, uint32_t *val);
uint8_t TMC2209_ReadAllRegs(TMC2209_RegSnapshot_t *snap);

/* ---- TMC2209 功能配置封装 ---- */
uint8_t TMC2209_Init(uint8_t irun, uint8_t ihold);   /* 先读所有寄存器 → 写推荐配置 */
uint8_t TMC2209_SetCurrent(uint8_t irun, uint8_t ihold, uint8_t iholddelay);
uint8_t TMC2209_SetMicrosteps(uint8_t mres);
uint8_t TMC2209_SetChopperMode(uint8_t spread);
uint8_t TMC2209_SetPwmFreq(uint8_t freq);      /* PWMCONF.bit17:16：0=%00,1=%01(默认),2=%10,3=%11 */
uint8_t TMC2209_SetToff(uint8_t toff);         /* CHOPCONF.bit3:0：慢衰减关断时间 0~15（StealthChop推荐2~3） */
uint8_t TMC2209_SetHend(uint8_t hend);         /* CHOPCONF.bit10:7：HEND/HDEC 滞环低值 0~15（HEND+HSTRT≤16） */
uint8_t TMC2209_SetStallThreshold(uint8_t sgthrs);
uint8_t TMC2209_GetStallResult(uint16_t *sg);
uint8_t TMC2209_ReadStatus(uint32_t *drv_status);
uint8_t TMC2209_ReadIOIN(uint32_t *ioin);

/* ---- 仅 UART1+GND 连接：方向/运动均通过寄存器控制 ---- */
uint8_t TMC2209_SetDir(uint8_t dir);              /* 写 GCONF.shaft(bit3)：0=正, 1=反 */
uint8_t TMC2209_StartSpeed(float step_hz, uint8_t mres); /* 写 VACTUAL(0x22)：内部脉冲发生器，频率支持小数 */
uint8_t TMC2209_StopSpeed(void);                  /* VACTUAL=0 停止 */

#endif /* __UART_CONTROL_H */

