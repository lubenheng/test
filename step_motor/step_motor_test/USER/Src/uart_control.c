/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    uart_control.c
  * @brief   UART 控制层实现
  *           - USART1 单线 UART 读写 TMC2209 寄存器（电流/微步/斩波/StallGuard）
  *           - USART3 与上位机通信（START/STOP/STATUS 等指令）
  *           - 宏 MOTOR_CTRL_MODE 切换本地/上位机控制方式
  ******************************************************************************
  * @note    TMC2209 单线 UART：PA9(TX)→1kΩ→PDN_UART，PA10(RX)直连；
  *          启用前需把 USART1 波特率改为 115200（本文件 Init 中自动重配）。
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "step_motor_contorl_hardware.h"
#include "uart_control.h"

/* USER CODE BEGIN Private define */

/* TMC2209 帧常数 */
#define TMC2209_SYNC       0x05u   /* 同步头 */
#define TMC2209_REPLY      0xFFu   /* 应答同步 */

/* =====================================================================
 * 推荐初始化配置（与《TMC2209_UART寄存器配置函数设计.md》一致）
 * 功能注释：
 *  GCONF  : pdn_disable=1（UART 模式必备，屏蔽 PDN 降流功能）
 *           mstep_reg_select=1（微步由 CHOPCONF.MRES 寄存器决定）
 *  IHOLD_IRUN : IHOLDDELAY=1（静止降流平滑过渡档位）
 *               IRUN=16（运行电流刻度 17/32）
 *               IHOLD=4（静止电流刻度 5/32，约 16%）
 *  TPOWERDOWN : 20（静止 20×2^18/fCLK ≈ 0.44s 后降流）
 *  CHOPCONF : MRES=5（1/8 微步）
 *             TBL=2（比较器消隐 32 clocks）、TOFF=2（慢衰减 2）
 *             HSTRT=0、HEND=0（滞环 0）
 *             intpol 依 TMC2209_MOVE_MODE：STEP=0（电流阶梯=MRES）、VACTUAL=1（插值平滑）
 *  PWMCONF : pwm_autoscale=1（自动幅值调节）
 *            pwm_autograd=1（自动梯度调节）
 *            pwm_freq=1（%01: fPWM=2/683 fCLK）
 *            PWM_LIM=12（自动幅值上限）
 *            PWM_REG=8（调节环梯度）
 *            PWM_OFS=36（幅值偏移）
 *            PWM_GRAD=0（梯度 0，靠 autograd 自动）
 *  TPWMTHRS : 0（禁用 StealthChop→SpreadCycle 自动切换，全程 StealthChop）
 * ===================================================================== */
#define TMC2209_GCONF_DEF      0x000000C0u   /* pdn_disable(6) + mstep_reg_select(7) */
#define TMC2209_IHOLD_IRUN_DEF 0x00011004u   /* IHOLDDELAY(19:16)=1, IRUN(12:8)=16, IHOLD(4:0)=4 */
#define TMC2209_TPOWERDOWN_DEF 20u
#if (TMC2209_MOVE_MODE == TMC2209_MOVE_STEP)
#define TMC2209_CHOPCONF_DEF   0x05010002u   /* MRES=5, intpol=0, TBL=2, TOFF=2（STEP 模式：电流阶梯=MRES） */
#else
#define TMC2209_CHOPCONF_DEF   0x15010002u   /* MRES=5, intpol=1, TBL=2, TOFF=2（VACTUAL 模式：插值平滑） */
#endif
#define TMC2209_PWMCONF_DEF    0xC80D0024u   /* autoscale=1, autograd=1, freq=1, LIM=12, REG=8, OFS=36, GRAD=0 */
#define TMC2209_TPWMTHRS_DEF   0u            /* 禁用斩波自动切换 */

/* START 频率限幅（浮点，Hz） */
#define TMC2209_MIN_HZ         0.01f       /* 频率下限（0.01Hz 极低速） */
#define TMC2209_MAX_HZ         200.0f      /* 频率上限 */

/* VACTUAL 运动状态（仅 UART1+GND 连接，无 STEP/DIR 引脚） */
static uint8_t  s_mres    = 5u;   /* 当前 MRES（默认 1/8），START 换算 VACTUAL 用 */
static uint32_t s_vactual = 0u;   /* 当前 VACTUAL 值，STATUS 回显用 */

/* USER CODE END Private define */

/* USER CODE BEGIN 0 */

/* ---- 上位机接收缓冲（USART3） ---- */
static char     s_rx_buf[UART_CMD_BUF_SIZE];
static uint8_t  s_rx_len = 0u;
TMC2209_RegSnapshot_t snap;
/* ---- TMC2209 单线 UART 时序 ---- */
#define TMC2209_TIMEOUT  20u   /* ms */

/**
  * @brief  CRC8 计算（数据手册 4.2 节：CRC8-ATM，多项式 x^8+x^2+x+1=0x07，初值 0，
  *         按 LSB→MSB 逐位处理输入字节，含 sync 与地址字节）
  * @param  data 数据指针（计算其前 len 字节，不含 CRC 字节本身）
  * @param  len  参与 CRC 的字节数
  * @retval 计算出的 CRC 字节
  */
static uint8_t TMC2209_CRC8(const uint8_t *data, uint8_t len)
{
  uint8_t crc = 0u;
  uint8_t i, j;

  for (i = 0u; i < len; i++)
  {
    uint8_t cur = data[i];
    for (j = 0u; j < 8u; j++)
    {
      /* 串行算法：if ((crc>>7) ^ (当前输入位)) crc=(crc<<1)^0x07; else crc<<=1; */
      if (((crc >> 7u) & 1u) ^ (cur & 1u))
      {
        crc = (uint8_t)((crc << 1u) ^ 0x07u);
      }
      else
      {
        crc = (uint8_t)(crc << 1u);
      }
      cur >>= 1u;
    }
  }
  return crc;
}

/**
  * @brief  清空 USART1 RX 缓冲（丢弃自回显残留）
  */
static void TMC2209_ClearRx(void)
{
  volatile uint8_t d;
  while (__HAL_UART_GET_FLAG(&huart1, UART_FLAG_RXNE) != RESET)
  {
    d = (uint8_t)(huart1.Instance->DR & 0xFFu);  /* 读走数据 */
  }
  (void)d;
}

/**
  * @brief  写 TMC2209 寄存器
  * @param  reg 寄存器地址（0x00~0x70）
  * @param  val 32 位数据
  * @retval 1=成功，0=失败
  */
uint8_t TMC2209_WriteReg(uint8_t reg, uint32_t val)
{
  uint8_t tx[8];

  tx[0] = TMC2209_SYNC;
  tx[1] = TMC2209_ADDR;
  tx[2] = (uint8_t)(0x80u | reg);       /* 写标志 */
  tx[3] = (uint8_t)(val >> 24u);
  tx[4] = (uint8_t)(val >> 16u);
  tx[5] = (uint8_t)(val >> 8u);
  tx[6] = (uint8_t)(val);
  tx[7] = TMC2209_CRC8(tx, 7u);         /* CRC 覆盖前 7 字节（sync..data0） */

  TMC2209_ClearRx();                    /* 清残留自回显 */
  if (HAL_UART_Transmit(&huart1, tx, 8u, TMC2209_TIMEOUT) != HAL_OK)
  {
    UART_Control_SendString("WR:TX-FAIL\r\n");   /* USART1 发送失败 */
    return 0u;
  }
  /* 写帧 TMC2209 不应答，自回显留在 RX，等待下次 ClearRx 清掉 */
  return 1u;
}

/**
  * @brief  读 TMC2209 寄存器
  * @param  reg 寄存器地址
  * @param  val 读回数据指针
  * @retval 1=成功，0=失败
  */
uint8_t TMC2209_ReadReg(uint8_t reg, uint32_t *val)
{
  uint8_t  tx[4];   /* 读请求 4 字节：sync+addr+reg+crc（数据手册 4.1.2） */
  uint8_t  rx[12];  /* 前4=自回显（丢弃），后8=TMC2209 应答 */
  char     s[48];

  tx[0] = TMC2209_SYNC;
  tx[1] = TMC2209_ADDR;
  tx[2] = reg;                              /* 读标志（无 0x80） */
  tx[3] = TMC2209_CRC8(tx, 3u);             /* CRC 覆盖前 3 字节 */

  TMC2209_ClearRx();
  if (HAL_UART_Transmit(&huart1, tx, 4u, TMC2209_TIMEOUT) != HAL_OK)
  {
    UART_Control_SendString("RD:TX-FAIL\r\n");       /* ① 发送失败 */
    return 0u;
  }
  /* 接收 12 字节：前 4 是自回显（丢弃），后 8 是 TMC2209 应答
     （应答：sync + 0xFF + reg + data3..0(MSB先) + crc） */
  if (HAL_UART_Receive(&huart1, rx, 12u, TMC2209_TIMEOUT) != HAL_OK)
  {
    UART_Control_SendString("RD:RX-TIMEOUT\r\n");    /* ② TMC2209 无应答 */
    return 0u;
  }
  if ((rx[4] != TMC2209_SYNC) || (rx[5] != TMC2209_REPLY) || (rx[6] != reg))
  {
    /* ③ 应答头异常（可能自回显/应答错位，或帧格式不符） */
    sprintf(s, "RD:HDR %02X %02X %02X\r\n", rx[4], rx[5], rx[6]);
    UART_Control_SendString(s);
    return 0u;
  }
  if (TMC2209_CRC8(&rx[4], 7u) != rx[11])   /* 应答 CRC 覆盖前 7 字节 */
  {
    UART_Control_SendString("RD:CRC-FAIL\r\n");      /* ④ CRC 校验失败 */
    return 0u;
  }
  *val = ((uint32_t)rx[7] << 24u) | ((uint32_t)rx[8] << 16u)
       | ((uint32_t)rx[9] << 8u) | (uint32_t)rx[10];
  return 1u;
}

/**
  * @brief  读取全部可读寄存器（上电默认快照）
  * @param  snap 快照指针
  * @retval 1=全部成功，0=任一失败
  */
uint8_t TMC2209_ReadAllRegs(TMC2209_RegSnapshot_t *snap)
{
  if (snap == NULL)
  {
    return 0u;
  }
  if (!TMC2209_ReadReg(0x00u, &snap->gconf))      return 0u;
  if (!TMC2209_ReadReg(0x01u, &snap->gstat))      return 0u;
  if (!TMC2209_ReadReg(0x02u, &snap->ifcnt))      return 0u;
  if (!TMC2209_ReadReg(0x06u, &snap->ioin))       return 0u;
  if (!TMC2209_ReadReg(0x12u, &snap->tstep))      return 0u;
  if (!TMC2209_ReadReg(0x41u, &snap->sg_result))  return 0u;
  if (!TMC2209_ReadReg(0x6Cu, &snap->chopconf))   return 0u;
  if (!TMC2209_ReadReg(0x6Fu, &snap->drv_status)) return 0u;
  if (!TMC2209_ReadReg(0x70u, &snap->pwmconf))    return 0u;
  return 1u;
}

/* USER CODE END 0 */

/* =====================================================================
 * TMC2209 功能配置封装（以《TMC2209_UART寄存器配置函数设计.md》为准）
 * ===================================================================== */

/**
  * @brief  TMC2209 初始化：仅写入推荐配置（**不做读取校验**，
  *         通信状态由上位机 `REGR`/`IOIN` 指令确认）
  * @param  irun  运行电流刻度（0~31，建议 16~31）
  * @param  ihold 静止电流刻度（0~31）
  * @retval 1=写帧全部发出；0=某个写帧发送失败（HAL 超时）
  */
uint8_t TMC2209_Init(uint8_t irun, uint8_t ihold)
{
  uint8_t ok = 1u;

#if (TMC2209_UART_EN == 1u)
  /* 直接写入推荐配置（不做读取校验） */
  ok &= TMC2209_WriteReg(0x00u, TMC2209_GCONF_DEF);         /* GCONF：pdn_disable+mstep_reg_select */
  ok &= TMC2209_WriteReg(0x10u, (1u << 16u)                 /* IHOLD_IRUN：IHOLDDELAY=1 */
                                | ((uint32_t)(irun & 0x1Fu) << 8u)   /* IRUN */
                                | (uint32_t)(ihold & 0x1Fu));        /* IHOLD */
  ok &= TMC2209_WriteReg(0x11u, TMC2209_TPOWERDOWN_DEF);    /* TPOWERDOWN */
  ok &= TMC2209_WriteReg(0x13u, TMC2209_TPWMTHRS_DEF);      /* TPWMTHRS=0（禁用斩波切换） */
  ok &= TMC2209_WriteReg(0x6Cu, TMC2209_CHOPCONF_DEF);      /* CHOPCONF：MRES=5, TBL=2, TOFF=2 */
  ok &= TMC2209_WriteReg(0x70u, TMC2209_PWMCONF_DEF);       /* PWMCONF：autoscale/autograd 等 */
#endif

  return ok;
}

/**
  * @brief  设置电流
  * @param  irun/ihold 0~31；iholddelay 0~15
  */
uint8_t TMC2209_SetCurrent(uint8_t irun, uint8_t ihold, uint8_t iholddelay)
{
  uint32_t v = ((uint32_t)(iholddelay & 0x0Fu) << 16u)
             | ((uint32_t)(irun & 0x1Fu) << 8u)
             | (uint32_t)(ihold & 0x1Fu);
  return TMC2209_WriteReg(0x10u, v);
}

/**
  * @brief  设置 PWM 频率（PWMCONF.bit17:16，数据手册 Rev.1.03）
  *         %00: fPWM=2/1024 fCLK（≈23.4kHz@12MHz）
  *         %01: fPWM=2/683  fCLK（≈35.1kHz@12MHz，默认）
  *         %10: fPWM=2/512  fCLK（≈46.9kHz@12MHz）
  *         %11: fPWM=2/410  fCLK（≈58.5kHz@12MHz）
  * @param  freq 0~3
  * @retval 1=成功，0=写帧失败
  */
uint8_t TMC2209_SetPwmFreq(uint8_t freq)
{
  uint32_t v;
  if (freq > 3u)
  {
    freq = 1u;   /* 非法回退默认 %01 */
  }
  /* 读当前 PWMCONF 只改 bit17:16；读失败用默认值，保证发送写帧 */
  if (!TMC2209_ReadReg(0x70u, &v))
  {
    v = TMC2209_PWMCONF_DEF;
  }
  v = (v & ~((uint32_t)0x3u << 16u)) | ((uint32_t)freq << 16u);
  return TMC2209_WriteReg(0x70u, v);
}

/**
  * @brief  设置 TOFF（CHOPCONF.bit3:0，慢衰减关断时间）
  *         StealthChop 推荐 toff=2~3；SpreadCycle 推荐 toff=5。
  *         该设置也限制最高斩波频率；toff=0 关闭所有驱动（软件禁用）。
  * @param  toff 0~15
  * @retval 1=成功，0=写帧失败
  */
uint8_t TMC2209_SetToff(uint8_t toff)
{
  uint32_t v;
  if (toff > 15u)
  {
    toff = 3u;   /* 非法回退默认 */
  }
  /* 读当前 CHOPCONF 只改 bit3:0；读失败用默认值，保证发送写帧 */
  if (!TMC2209_ReadReg(0x6Cu, &v))
  {
    v = TMC2209_CHOPCONF_DEF;
  }
  v = (v & ~0x0Fu) | (uint32_t)(toff & 0x0Fu);
  return TMC2209_WriteReg(0x6Cu, v);
}

/**
  * @brief  设置 HEND/HDEC（CHOPCONF.bit10:7，滞环低值）
  *         HEND 越大，SpreadCycle 下斩波滞环越大（电流纹波更平滑）。
  *         注意：数据手册要求 **HEND + HSTRT ≤ 16**。
  * @param  hend 0~15（默认 0）
  * @retval 1=成功，0=写帧失败
  */
uint8_t TMC2209_SetHend(uint8_t hend)
{
  uint32_t v;
  if (hend > 15u)
  {
    hend = 0u;   /* 非法回退默认 0 */
  }
  /* 读当前 CHOPCONF 只改 bit10:7；读失败用默认值，保证发送写帧 */
  if (!TMC2209_ReadReg(0x6Cu, &v))
  {
    v = TMC2209_CHOPCONF_DEF;
  }
  v = (v & ~((uint32_t)0xFu << 7u)) | ((uint32_t)hend << 7u);
  return TMC2209_WriteReg(0x6Cu, v);
}

/**
  * @brief  设置微步（寄存器方式，需 GCONF.mstep_reg_select=1）
  *         直接写完整 CHOPCONF 寄存器值（不做读改写），配置完全确定。
  *         CHOPCONF(0x6C) 值：
  *           TMC2209_MOVE_STEP  模式：intpol=0 → 0x0{mres}000003（电流阶梯=MRES）
  *           TMC2209_MOVE_VACTUAL 模式：intpol=1 → 0x1{mres}000003（插值平滑）
  *         intpol(28)（插值到256微步）、MRES(27:24)、TBL(16:15)=0、
  *         HSTRT(6:4)=0、HEND(10:7)=0、TOFF(3:0)=3
  * @param  mres 0=1/256, 1=1/128, 2=1/64, 3=1/32, 4=1/16,
  *              5=1/8, 6=1/4, 7=1/2, 8=fullstep
  * @retval 1=成功，0=写帧失败
  */
uint8_t TMC2209_SetMicrosteps(uint8_t mres)
{
  uint32_t v;
  if (mres > 8u)
  {
    mres = 5u;   /* 非法值回退 1/8 */
  }
#if (TMC2209_MOVE_MODE == TMC2209_MOVE_STEP)
  /* STEP 引脚模式：intpol=0，电流阶梯 = MRES（每电周期 4×微步数 个阶梯） */
  v = 0x00000003u | ((uint32_t)mres << 24u);
#else
  /* VACTUAL 模式：intpol=1，插值到 256 细分（平滑，阶梯数恒为 256） */
  v = 0x10000003u | ((uint32_t)mres << 24u);
#endif
  return TMC2209_WriteReg(0x6Cu, v);
}

/**
  * @brief  分数 → MRES 换算（1/div 微步 → mres）
  *         关系：每全步微步数 = 2^(8-mres) = div → mres = 8 - log2(div)
  * @param  div 微步数（1/4 → 4、1/16 → 16、1/256 → 256）
  * @retval mres 0~8
  */
static uint8_t TMC2209_DivToMres(uint16_t div)
{
  uint8_t mres = 8u;
  if (div == 0u)
  {
    return 5u;   /* 非法 → 1/8 */
  }
  while (div > 1u)
  {
    div >>= 1u;
    if (mres > 0u)
    {
      mres--;
    }
  }
  return mres;
}

/**
  * @brief  选择斩波模式
  * @param  spread 1=SpreadCycle（高动态），0=StealthChop（静音）
  */
uint8_t TMC2209_SetChopperMode(uint8_t spread)
{
  uint32_t v;
  if (!TMC2209_ReadReg(0x00u, &v))
  {
    return 0u;
  }
  if (spread)
  {
    v |= (1u << 2u);    /* en_SpreadCycle=1 */
  }
  else
  {
    v &= ~(1u << 2u);
  }
  return TMC2209_WriteReg(0x00u, v);
}

/**
  * @brief  设置 StallGuard 阈值（无传感器堵转检测）
  * @param  sgthrs 0~255；SG_RESULT ≤ 2×SGTHRS 判定堵转
  */
uint8_t TMC2209_SetStallThreshold(uint8_t sgthrs)
{
  return TMC2209_WriteReg(0x40u, (uint32_t)sgthrs);
}

/**
  * @brief  读取 StallGuard 结果
  * @param  sg 结果指针（越大负载越小）
  */
uint8_t TMC2209_GetStallResult(uint16_t *sg)
{
  uint32_t v;
  if (!TMC2209_ReadReg(0x41u, &v))
  {
    return 0u;
  }
  *sg = (uint16_t)(v & 0x3FFu);
  return 1u;
}

/**
  * @brief  读取驱动状态（DRV_STATUS）
  * @param  drv_status 状态值指针（otpw/ot/s2g/s2vs/ola/olb/stealth/stst 等）
  */
uint8_t TMC2209_ReadStatus(uint32_t *drv_status)
{
  return TMC2209_ReadReg(0x6Fu, drv_status);
}

/**
  * @brief  读取 IOIN（输入状态与芯片版本）
  * @param  ioin 值指针（bit0 ENN、bit2 MS1、bit3 MS2、bit4 DIAG、bit6 PDN_UART、
  *               bit7 STEP、bit8 SPREAD_EN、bit9 DIR、bit31:24 VERSION=0x21）
  * @retval 1=成功，0=失败
  */
uint8_t TMC2209_ReadIOIN(uint32_t *ioin)
{
  return TMC2209_ReadReg(0x06u, ioin);
}

/**
  * @brief  设置方向（写 GCONF.shaft，仅 UART1 模式，不依赖 DIR 引脚）
  * @param  dir 0=正向(shaft=0)，1=反向(shaft=1)
  */
uint8_t TMC2209_SetDir(uint8_t dir)
{
  uint32_t v;
  if (!TMC2209_ReadReg(0x00u, &v))
  {
    v = TMC2209_GCONF_DEF;
  }
  if (dir)
  {
    v |= (1u << 3u);   /* shaft=1 反向 */
  }
  else
  {
    v &= ~(1u << 3u);
  }
  return TMC2209_WriteReg(0x00u, v);
}

/**
  * @brief  通过 VACTUAL 内部脉冲发生器启动/调速（仅 UART1 模式，无 STEP 引脚）
  * @param  step_hz 等效 STEP 频率(Hz)，支持小数（如 0.01Hz）
  * @param  mres    当前微步 MRES（0~8；每 STEP = 2^(8-mres) 个 1/256 微步）
  * @retval 1=成功，0=失败
  */
uint8_t TMC2209_StartSpeed(float step_hz, uint8_t mres)
{
  float    us;
  uint32_t vactual;

  if (mres > 8u)
  {
    mres = 8u;
  }
  if (step_hz < TMC2209_MIN_HZ)
  {
    step_hz = TMC2209_MIN_HZ;
  }
  if (step_hz > TMC2209_MAX_HZ)
  {
    step_hz = TMC2209_MAX_HZ;
  }
  /* 等效 1/256 微步速率：us = step_hz × 2^mres
     （微步数 = 2^(8-mres)，每微步 = 256/2^(8-mres) = 2^mres 个 1/256 微步） */
  us = step_hz * (float)(1u << mres);
  /* VACTUAL = us / (fCLK/2/2^23) = us × 2^24 / fCLK  (fCLK=12MHz)
     0.01Hz 级别 → us 很小 → 用浮点保证精度，四舍五入取整 */
  vactual = (uint32_t)((us * 16777216.0f) / 12000000.0f + 0.5f);
  if (vactual == 0u)
  {
    vactual = 1u;   /* 极低速时 VACTUAL 至少=1（VACTUAL=0 表示停止） */
  }
  if (vactual > 0x7FFFFFu)
  {
    vactual = 0x7FFFFFu;   /* 24 位有符号上限 */
  }
  s_vactual = vactual;
  return TMC2209_WriteReg(0x22u, vactual);
}

/**
  * @brief  停止（写 VACTUAL=0，内部脉冲发生器停止）
  */
uint8_t TMC2209_StopSpeed(void)
{
  s_vactual = 0u;
  return TMC2209_WriteReg(0x22u, 0u);
}

/* =====================================================================
 * 上位机（USART3）指令处理
 * ===================================================================== */

/**
  * @brief  USART3 发送字符串（上位机）
  */
void UART_Control_SendString(const char *s)
{
  if (s == NULL)
  {
    return;
  }
  while (*s != '\0')
  {
    uint8_t c = (uint8_t)*s++;
    if (HAL_UART_Transmit(&huart3, &c, 1u, 10u) != HAL_OK)
    {
      break;
    }
  }
}

/* 指令解析与执行 */
static void UART_Control_Parse(const char *cmd)
{
  char     s[48];
  uint32_t val;
  uint16_t sg;
  uint8_t  n;

  if (strncmp(cmd, "START", 5u) == 0)
  {
    float hz = (float)atof(cmd + 6u);
    if (hz < TMC2209_MIN_HZ)
    {
      hz = TMC2209_MIN_HZ;
    }
#if (TMC2209_MOVE_MODE == TMC2209_MOVE_STEP)
    /* B：STEP 引脚模式 → TIM2 PWM 输出 STEP 脉冲（PA1），缓升启动 */
    Motor_Start((uint32_t)hz);
#else
    /* A：VACTUAL 模式 → 写 0x22 内部脉冲发生器 */
    TMC2209_StartSpeed(hz, s_mres);
#endif
    UART_Control_SendString("OK START\r\n");
  }
  else if (strncmp(cmd, "STOP", 4u) == 0)
  {
#if (TMC2209_MOVE_MODE == TMC2209_MOVE_STEP)
    Motor_Stop();              /* STEP：缓降后停止 TIM2 PWM */
#else
    TMC2209_StopSpeed();       /* VACTUAL=0 停止 */
#endif
    UART_Control_SendString("OK STOP\r\n");
  }
  else if (strncmp(cmd, "STATUS", 6u) == 0)
  {
#if (TMC2209_MOVE_MODE == TMC2209_MOVE_STEP)
    uint32_t hz = Motor_GetSpeedHz();
    uint8_t  run = Motor_IsRunning();
#else
    /* 等效 STEP 频率 = VACTUAL × (fCLK/2/2^23) / 2^mres */
    uint32_t hz = (s_vactual != 0u)
                    ? (uint32_t)(((uint64_t)s_vactual * 12000000u / 16777216u) >> s_mres)
                    : 0u;
    uint8_t run = (s_vactual != 0u) ? 1u : 0u;
#endif
    sprintf(s, "OK:%luHz %s\r\n", (unsigned long)hz, run ? "RUN" : "STOP");
    UART_Control_SendString(s);
  }
  else if (strncmp(cmd, "HELP", 4u) == 0)
  {
    UART_Control_SendString("CMD: START<hz> STOP STATUS HELP DIR<0/1> MICRO<mres> "
                            "IRUN<n> IHOLD<n> CHOP<0/1> PWMFREQ<0-3> TOFF<0-15> HDEC<0-15> "
                            "SG DRV IOIN REGR<addr> REGW<addr val>\r\n");
  }
  else if (strncmp(cmd, "DIR", 3u) == 0)
  {
    n = (uint8_t)atoi(cmd + 4u);
    TMC2209_SetDir(n);   /* 写 GCONF.shaft（不依赖 DIR 引脚） */
    sprintf(s, "OK DIR=%u\r\n", (unsigned)n);
    UART_Control_SendString(s);
  }
  else if (strncmp(cmd, "MICRO", 5u) == 0)
  {
    const char *p = cmd + 6u;
    /* 支持分数格式 "1/4"（每全步 4 微步）与整数格式（MRES 值 0~8） */
    if ((p[0] == '1') && (p[1] == '/'))
    {
      n = TMC2209_DivToMres((uint16_t)atoi(p + 2u));   /* 1/4→6, 1/16→4, 1/8→5 */
    }
    else
    {
      n = (uint8_t)atoi(p);
      if (n > 8u)
      {
        n = 5u;   /* 非法 → 1/8 */
      }
    }
    /* 直接写完整 CHOPCONF（基于默认值构造，不读改写，保证写帧发出） */
    TMC2209_SetMicrosteps(n);
    s_mres = n;   /* 更新，供 START 换算 VACTUAL */
    sprintf(s, "OK MICRO=%u\r\n", (unsigned)n);
    UART_Control_SendString(s);
  }
  else if (strncmp(cmd, "IRUN", 4u) == 0)
  {
    n = (uint8_t)atoi(cmd + 5u);
    /* 读当前 0x10 只改 IRUN 位；读失败用默认值，保证发送写帧 */
    if (!TMC2209_ReadReg(0x10u, &val))
    {
      val = TMC2209_IHOLD_IRUN_DEF;   /* 默认 IHOLDDELAY=1, IRUN=16, IHOLD=4 */
    }
    val = (val & ~0x1F00u) | ((uint32_t)(n & 0x1Fu) << 8u);
    TMC2209_WriteReg(0x10u, val);
    sprintf(s, "OK IRUN=%u\r\n", (unsigned)n);
    UART_Control_SendString(s);
  }
  else if (strncmp(cmd, "IHOLD", 5u) == 0)
  {
    n = (uint8_t)atoi(cmd + 6u);
    /* 读当前 0x10 只改 IHOLD 位；读失败用默认值，保证发送写帧 */
    if (!TMC2209_ReadReg(0x10u, &val))
    {
      val = TMC2209_IHOLD_IRUN_DEF;
    }
    val = (val & ~0x1Fu) | (uint32_t)(n & 0x1Fu);
    TMC2209_WriteReg(0x10u, val);
    sprintf(s, "OK IHOLD=%u\r\n", (unsigned)n);
    UART_Control_SendString(s);
  }
  else if (strncmp(cmd, "CHOP", 4u) == 0)
  {
    n = (uint8_t)atoi(cmd + 5u);
    /* 读当前 GCONF 改 en_SpreadCycle 位；读失败用默认值，保证发送写帧 */
    if (!TMC2209_ReadReg(0x00u, &val))
    {
      val = TMC2209_GCONF_DEF;
    }
    if (n)
    {
      val |= (1u << 2u);   /* en_SpreadCycle=1 */
    }
    else
    {
      val &= ~(1u << 2u);
    }
    TMC2209_WriteReg(0x00u, val);
    sprintf(s, "OK CHOP=%s\r\n", n ? "SpreadCycle" : "StealthChop");
    UART_Control_SendString(s);
  }
  else if (strncmp(cmd, "PWMFREQ", 7u) == 0)
  {
    n = (uint8_t)atoi(cmd + 8u);
    TMC2209_SetPwmFreq(n);   /* 写 PWMCONF.bit17:16 */
    sprintf(s, "OK PWMFREQ=%u\r\n", (unsigned)n);
    UART_Control_SendString(s);
  }
  else if (strncmp(cmd, "TOFF", 4u) == 0)
  {
    n = (uint8_t)atoi(cmd + 5u);
    TMC2209_SetToff(n);      /* 写 CHOPCONF.bit3:0 */
    sprintf(s, "OK TOFF=%u\r\n", (unsigned)n);
    UART_Control_SendString(s);
  }
  else if (strncmp(cmd, "HDEC", 4u) == 0)
  {
    n = (uint8_t)atoi(cmd + 5u);
    TMC2209_SetHend(n);      /* 写 CHOPCONF.bit10:7（HEND/HDEC） */
    sprintf(s, "OK HDEC=%u\r\n", (unsigned)n);
    UART_Control_SendString(s);
  }
  else if (strncmp(cmd, "SG", 2u) == 0)
  {
    if (TMC2209_GetStallResult(&sg))
    {
      sprintf(s, "OK:%u\r\n", (unsigned)sg);
    }
    else
    {
      sprintf(s, "ERR\r\n");
    }
    UART_Control_SendString(s);
  }
  else if (strncmp(cmd, "DRV", 3u) == 0)
  {
    if (TMC2209_ReadStatus(&val))
    {
      sprintf(s, "OK:0x%08lX\r\n", (unsigned long)val);
    }
    else
    {
      sprintf(s, "ERR\r\n");
    }
    UART_Control_SendString(s);
  }
  else if (strncmp(cmd, "IOIN", 4u) == 0)
  {
    if (TMC2209_ReadIOIN(&val))
    {
      sprintf(s, "OK:VERSION=0x%02lX\r\n", (unsigned long)(val >> 24u));
    }
    else
    {
      sprintf(s, "ERR\r\n");
    }
    UART_Control_SendString(s);
  }
  else if (strncmp(cmd, "REGR", 4u) == 0)
  {
    n = (uint8_t)strtol(cmd + 5u, NULL, 16);
    if (TMC2209_ReadReg(n, &val))
    {
      sprintf(s, "OK:0x%08lX\r\n", (unsigned long)val);
    }
    else
    {
      sprintf(s, "ERR\r\n");
    }
    UART_Control_SendString(s);
  }
  else if (strncmp(cmd, "REGW", 4u) == 0)
  {
    char    *p = (char *)cmd + 5u;
    uint8_t  addr = (uint8_t)strtol(p, &p, 16);
    uint32_t v    = (uint32_t)strtol(p, NULL, 16);
    if (TMC2209_WriteReg(addr, v))
    {
      sprintf(s, "OK REGW\r\n");
    }
    else
    {
      sprintf(s, "ERR\r\n");
    }
    UART_Control_SendString(s);
  }
  else
  {
    UART_Control_SendString("ERR\r\n");
  }
}

/* =====================================================================
 * 初始化 / 主循环任务
 * ===================================================================== */

/**
  * @brief  UART 控制初始化
  * @note   ① TMC2209 配置（先读快照再写入，USART1 已由 CubeMX 配置为 115200）；
  *         ② USART3（115200）就绪供上位机
  */
void UART_Control_Init(void)
{
#if (TMC2209_UART_EN == 1u)
  /* TMC2209 初始化：仅写入推荐配置（IRUN=16, IHOLD=4, IHOLDDELAY=1），不做读取校验
     返回：1=写帧全部发出, 0=发送失败 */
  if (TMC2209_Init(16u, 4u) != 0u)
  {
    UART_Control_SendString("TMC2209 CFG OK\r\n");
  }
  else
  {
    UART_Control_SendString("TMC2209 CFG FAIL\r\n");
  }
#else
  UART_Control_SendString("TMC2209 UART DISABLED\r\n");
#endif
}

/**
  * @brief  主循环轮询：接收 USART3 上位机指令并执行
  * @note   在 while(1) 中调用；遇到 \n 或 \r 视为一条完整指令
  */
void UART_Control_Task(void)
{
  uint8_t c;

  while (HAL_UART_Receive(&huart3, &c, 1u, 0u) == HAL_OK)
  {
    if ((c == '\n') || (c == '\r'))
    {
      if (s_rx_len > 0u)
      {
        s_rx_buf[s_rx_len] = '\0';
        UART_Control_Parse(s_rx_buf);
        s_rx_len = 0u;
      }
    }
    else if (s_rx_len < (UART_CMD_BUF_SIZE - 1u))
    {
      s_rx_buf[s_rx_len++] = (char)c;
    }
  }
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */



