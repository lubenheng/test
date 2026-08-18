# UART 控制方案与代码修改建议
> **目标**：USART3 与上位机通信（控制指令），USART1 与 TMC2209 单线 UART 通信（寄存器配置）；新增 `uart_control.c/.h`；用宏定义切换控制方式。
> **状态**：本文件为**修改建议**，尚未修改代码。

---

## 一、概述

当前工程：
- `USART1`（PA9/PA10，9600，带 DMA-TX + 中断）——未用于 TMC2209
- `USART3`（PB10/PB11，115200，无中断/DMA）——未使用
- `step_motor_contorl_hardware.c`：缓升启停控制（`Motor_Init/Start/Stop/GetSpeedHz/IsRunning`）
- TMC2209 的 PDN_UART 当前**接 GND**（不使用 UART）

本次改造：
```
上位机 ──USART3(115200,PB10/PB11)──► STM32F103C8T6 ──USART1(115200,PA9/PA10)──► TMC2209 PDN_UART
   指令解析(启停/变速/查询)                 uart_control.c                单线UART配置寄存器
```

---

## 二、硬件连接修改

| 接口 | 引脚 | 连接 | 说明 |
|---|---|---|---|
| **USART1 → TMC2209** | PA9(TX) → **1kΩ** → PDN_UART(14) | 单线 UART 发送 | 1kΩ 限流隔离（TX 空闲高电平与 TMC2209 应答不冲突） |
| | PA10(RX) → PDN_UART(14) 直连 | 单线 UART 接收 | 与 TX 同一节点；发送时会收到自己回显（忽略） |
| **USART3 → 上位机** | PB10(TX) → 上位机 RX | 上位机指令发送 | 连接 USB-TTL 等 |
| | PB11(RX) → 上位机 TX | 上位机指令接收 | 同上 |

> ⚠️ **PDN_UART 不再接地**：启用 UART 后需通过 `GCONF.pdn_disable=1` 屏蔽 PDN 引脚的"静止降流"功能（否则与通信冲突），静止电流改用 IHOLD 寄存器控制。

---

## 三、宏定义设计（控制方式选择）

新增 `uart_control.h` 中定义：

```c
/* ================= 控制方式宏 ================= */
#define CTRL_MODE_LOCAL   0u   /* 本地控制：上电自动启动（当前行为） */
#define CTRL_MODE_UART    1u   /* 上位机控制：等待 USART3 指令 */
#define MOTOR_CTRL_MODE   CTRL_MODE_UART   /* ← 切换控制方式 */

/* ================= TMC2209 通信使能 ================= */
#define TMC2209_UART_EN   1u   /* 1=用 USART1 配置 TMC2209；0=纯硬件(PDN接地) */

/* ================= TMC2209 单线 UART 参数 ================= */
#define TMC2209_BAUD      115200u     /* USART1 波特率（TMC2209 自动检测） */
#define TMC2209_ADDR      0u          /* 从机地址 = MS1 + 2*MS2（当前 0） */

/* ================= 上位机协议 ================= */
#define UART_CMD_BUF_SIZE 32u         /* 指令缓冲 */
```

**控制方式宏的行为差异**：
| 宏 | Motor_Init 行为 | 主循环 |
|---|---|---|
| `MOTOR_CTRL_MODE = CTRL_MODE_LOCAL` | 上电缓升启动（当前行为） | 不做指令解析 |
| `MOTOR_CTRL_MODE = CTRL_MODE_UART` | 仅使能+TMC2209配置，**不启动** | 轮询 USART3 指令，调用 Motor_Start/Stop 等 |

---

## 四、文件规划（新增 uart_control）

```
USER/Inc/uart_control.h    ← 宏定义、函数声明
USER/Src/uart_control.c    ← UART 控制实现（USART3 指令 + TMC2209 寄存器读写）
```

> 加入 Keil 工程：`MDK-ARM/step_motor_test.uvprojx` 的 USER 组中新增 `../USER/Src/uart_control.c`（与 step_motor_contorl_hardware.c 并列）。

## 五、uart_control 接口设计

```c
/* uart_control.h */
#ifndef __UART_CONTROL_H
#define __UART_CONTROL_H

#include "main.h"

/* ---- 控制方式 / TMC2209 通信宏 ---- */
/* （见第三节，宏定义放本文件顶部） */

/* ---- 接口 ---- */
void UART_Control_Init(void);        /* ① 配置 USART1 为 115200；② 若使能则配置 TMC2209 寄存器；③ 启动 USART3 接收 */
void UART_Control_Task(void);        /* 主循环轮询：解析 USART3 上位机指令并执行 */
void UART_Control_SendString(const char *s);   /* USART3 发送（上位机） */

/* ---- TMC2209 单线 UART（内部实现，供 Init 使用）---- */
uint8_t TMC2209_WriteReg(uint8_t reg, uint32_t val);   /* 写寄存器 */
uint8_t TMC2209_ReadReg(uint8_t reg, uint32_t *val);   /* 读寄存器 */

#endif
```

`UART_Control_Task()` 建议在 `main()` 主循环 `while(1)` 中调用（轮询 USART3 接收缓冲）。

---

## 六、TMC2209 单线 UART（USART1）协议实现要点

### 6.1 帧格式（CRC 校验，以数据手册 4 章为准）
```
写：   [0x05][addr][0x80|reg][d3][d2][d1][d0][crc]        （8 字节）
读请求：[0x05][addr][reg][crc]                             （4 字节，手册 4.1.2）
应答： [0x05][0xFF][reg][d3][d2][d1][d0][crc]              （8 字节）
其中 addr=MS1+2*MS2=0
```

### 6.2 CRC8（数据手册 4.2 节，CRC8-ATM）
- 多项式 x^8+x^2+x+1 = 0x07，初值 0，**按 LSB→MSB 逐位处理**（含 sync 与地址字节）
- 覆盖范围：写帧前 7 字节；读请求前 3 字节；应答前 7 字节
```c
static uint8_t TMC2209_CRC8(const uint8_t *data, uint8_t len)
{
  uint8_t crc = 0u, i, j;
  for (i = 0u; i < len; i++)
  {
    uint8_t cur = data[i];
    for (j = 0u; j < 8u; j++)
    {
      if (((crc >> 7u) & 1u) ^ (cur & 1u)) crc = (uint8_t)((crc << 1u) ^ 0x07u);
      else                                 crc = (uint8_t)(crc << 1u);
      cur >>= 1u;
    }
  }
  return crc;
}
```
```c
static uint8_t TMC2209_CRC8(uint8_t crc, uint8_t data)
{
  crc ^= data;
  for (uint8_t i = 0; i < 8; i++)
  {
    crc = (crc & 0x80u) ? ((crc << 1u) ^ 0x07u) : (crc << 1u);
  }
  return crc;
}
```

### 6.3 初始化 TMC2209（TMC2209_UART_EN=1 时）
```c
TMC2209_WriteReg(0x00, 0x000000C0);  /* GCONF: pdn_disable(6)=1, mstep_reg_select(7)=1 */
TMC2209_WriteReg(0x10, (2u<<16)|(24u<<8)|12u);  /* IHOLD=12, IRUN=24, IHOLDDELAY=2 */
TMC2209_WriteReg(0x6C, 0x15000003);  /* CHOPCONF: intpol(28)=1, MRES(27:24)=5→1/8, toff=3 */
TMC2209_ReadReg(0x02, &ifcnt);       /* 读 IFCNT 校验通信（值=3+ 表示写成功） */
```

### 6.4 USART1 单线收发关键点
- **波特率改为 115200**（当前 9600）：TMC2209 自动波特率检测，115200 更稳妥（9600 也支持但接近下限）。
- 收发共用 PA9/PA10 节点：发送后 **PA10 会收到自己回显**，`HAL_UART_Receive` 时忽略前 8 字节自回显，或直接用 `HAL_UART_Transmit` 发送、`HAL_UART_Receive` 轮询接收应答。
- 发送写帧后无需等待应答（TMC2209 只回读请求）；读帧后 TMC2209 在 SENDDELAY（默认 8 位时间）后应答。
- 帧间隔 ≥ 8 位时间；连续帧间留 ≥1ms。

---

## 七、上位机（USART3）通信协议

### 7.1 建议指令集（文本，`\r\n` 结尾）
| 指令 | 示例 | 动作 |
|---|---|---|
| `START <freq>` | `START 800` | `Motor_Start(800)`（缓升到 800Hz） |
| `STOP` | `STOP` | `Motor_Stop()`（缓降停机） |
| `STATUS` | `STATUS` | 回传 `OK:<freq>Hz RUN/STOP` |
| `DIR <0/1>` | `DIR 1` | 改方向（需先停） |
| `HELP` | `HELP` | 回传指令说明 |

### 7.2 接收实现（轮询，无中断）
```c
/* 建议在 uart_control.c 内维护接收缓冲 */
static char s_rx_buf[UART_CMD_BUF_SIZE];
static uint8_t s_rx_len = 0;

/* 主循环中调用：逐个字节接收，遇 \n 视为一条完整指令 */
void UART_Control_Task(void)
{
  uint8_t c;
  while (HAL_UART_Receive(&huart3, &c, 1u, 0u) == HAL_OK)
  {
    if (c == '\n' || c == '\r')
    {
      if (s_rx_len > 0) { UART_Control_Parse(s_rx_buf, s_rx_len); s_rx_len = 0; }
    }
    else if (s_rx_len < UART_CMD_BUF_SIZE - 1u) { s_rx_buf[s_rx_len++] = (char)c; }
  }
}
```

### 7.3 指令解析（按 `MOTOR_CTRL_MODE` 生效）
```c
static void UART_Control_Parse(char *cmd, uint8_t len)
{
  if (strncmp(cmd, "START", 5) == 0)  { uint32_t hz = atoi(cmd+6); Motor_Start(hz); ... }
  else if (strncmp(cmd, "STOP", 4) == 0)  { Motor_Stop(); ... }
  else if (strncmp(cmd, "STATUS", 6) == 0){ UART_Control_SendStatus(); }
  ...
}
```

### 7.4 状态回传
```c
void UART_Control_SendStatus(void)
{
  char s[40];
  sprintf(s, "OK:%luHz %s\r\n", (unsigned long)Motor_GetSpeedHz(),
          Motor_IsRunning() ? "RUN" : "STOP");
  UART_Control_SendString(s);
}
```

## 八、与现有缓升控制集成

`uart_control` 调用现有 `step_motor_contorl_hardware` 的接口，二者解耦：

```
UART3 指令 → UART_Control_Task → UART_Control_Parse
                                  ├─ Motor_Start(hz)   （缓升）
                                  ├─ Motor_Stop()
                                  └─ Motor_GetSpeedHz()/Motor_IsRunning() （状态回传）
```

### 控制方式宏对 Motor_Init 的影响（建议）
在 `step_motor_contorl_hardware.c` 的 `Motor_Init()` 末尾按宏分支：
```c
/* 当前：无宏时上电直接缓升启动 */
void Motor_Init(void)
{
  ... /* 方向/微步/使能 */
#if (MOTOR_CTRL_MODE == CTRL_MODE_LOCAL)
  Motor_Start(MOTOR_MAX_HZ_DEFAULT);   /* 本地：上电自动启动 */
#else
  /* UART 模式：仅使能，等待上位机 START 指令 */
#endif
}
```
> 为减少对现有文件的改动，可把 `MOTOR_CTRL_MODE` 判断放在 `uart_control.c` 中：`UART_Control_Init()` 里按模式决定是否调用 `Motor_Start`。

---

## 九、需要修改的现有文件清单

| 文件 | 修改内容 | 是否 CubeMX 文件 |
|---|---|---|
| **新增** `USER/Src/uart_control.c` | UART3 指令解析 + TMC2209 读写 | 否（新建） |
| **新增** `USER/Inc/uart_control.h` | 宏定义 + 函数声明 | 否（新建） |
| `Core/Src/usart.c` | `huart1.Init.BaudRate` 9600 → **115200** | ✅ 建议在 **CubeMX .ioc** 中改（`USART1.BaudRate=115200`）后重新生成，避免被覆盖 |
| `step_motor_test.ioc` | `USART1.BaudRate=9600 → 115200` | ✅ CubeMX 配置 |
| `Core/Src/main.c` | USER CODE 区：`UART_Control_Init()` + 主循环 `UART_Control_Task()` | 仅 USER CODE 区 |
| `MDK-ARM/step_motor_test.uvprojx` | USER 组加入 `uart_control.c` | 否 |
| `USER/Src/step_motor_contorl_hardware.c` | 可选：按 `MOTOR_CTRL_MODE` 控制是否自动启动 | 否 |

---

## 十、注意事项

1. **PDN_UART 接线改变**：启用 UART1 后 PDN_UART **不再接地**，改为 PA9 经 1kΩ、PA10 直连；且必须写 `GCONF.pdn_disable=1`，否则 PDN 的静止降流功能与通信冲突。
2. **USART1 波特率已改 115200**：CubeMX .ioc 已配置 `USART1.BaudRate=115200`，与 TMC2209 自动波特率检测匹配；`uart_control.c` 不再需要运行时重配。
3. **单线 UART 回显**：USART1 发送时 PA10 收到自回显；`TMC2209_ReadReg` 读取应答时要**丢弃前 8 字节自回显**（或发送后延时、从第 9 字节起收）。
4. **USART3 无中断**：当前 USART3 未使能 NVIC，只能**轮询接收**（`HAL_UART_Receive` 0 超时）。若需中断接收，在 CubeMX 中给 USART3 勾选 NVIC（会生成 `USART3_IRQHandler`）。
5. **printf 重定向**：`UART_Control_SendString` 若用 `sprintf`，需包含 `stdio.h`；Keil 中 `HAL_UART_Transmit` 发送即可，无需重定向 printf（除非用 `printf`）。
6. **CRC 校验**：TMC2209 帧 CRC 错误会被丢弃，读 IFCNT 可确认写帧被接受。
7. **控制方式宏切换**：`MOTOR_CTRL_MODE` 改一次即可切换本地/上位机控制；默认建议先用 `CTRL_MODE_LOCAL` 验证，再切 `CTRL_MODE_UART`。
8. **上位机指令与缓升**：`START 800` 走 `Motor_Start(800)` 缓升（6.3s），不会瞬时跳频。
9. **TMC2209 电流**：通过 UART 写 `IHOLD_IRUN` 设定电流后，VREF 可保持开路（内部基准）或按需。

---

## 十一、验证步骤

1. **先改波特率**：CubeMX 中 `USART1.BaudRate=115200`，重新生成 → 编译下载。
2. **测 TMC2209 通信**：`TMC2209_UART_EN=1` 初始化后，读 `IOIN(0x06)/VERSION(0x21)`，上位机打印或调试器确认返回正确（能读到说明单线 UART 通路正常）。
3. **测上位机指令**：`MOTOR_CTRL_MODE=CTRL_MODE_UART`，串口助手（115200）发送：
   - `START 800` → 电机缓升到 800Hz
   - `STATUS` → 回传 `OK:800Hz RUN`
   - `STOP` → 缓降停止
4. **回退验证**：改回 `CTRL_MODE_LOCAL`，确认上电自动启动不受影响。

---

*建议：按"波特率 → TMC2209 通信 → 上位机指令 → 集成"顺序逐步实施，每步编译验证。*


