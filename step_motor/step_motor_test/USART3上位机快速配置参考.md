# USART3 上位机快速配置参考

> **接口**：USART3（PB10=TX / PB11=RX），115200-8-N-1，指令 `\r\n` 结尾
> **说明**：本表为常用功能的快速设置参考；功能通过写 TMC2209 寄存器实现。
> **运动控制方式**（`uart_control.h` 的 `TMC2209_MOVE_MODE`）：
> - `TMC2209_MOVE_STEP`（默认）：**STEP 引脚**（PA1/TIM2 PWM）产生脉冲 + `intpol=0` → **电流阶梯=MRES**
> - `TMC2209_MOVE_VACTUAL`：**VACTUAL(0x22)** 内部脉冲 + `intpol=1` → 电流恒 256 细分（平滑）

---

## 1. 速度设置（`START <freq>`，等效 STEP 频率）

| 目标转速 | 指令 | 等效频率 | 说明 |
|---|---|---|---|
| 7.5 RPM | `START 200` | 200Hz | 起步低速 |
| 30 RPM | `START 800` | 800Hz | 常用 |
| 75 RPM | `START 2000` | 2000Hz | |
| 150 RPM | `START 4000` | 4000Hz | |
| 300 RPM | `START 8000` | 8000Hz | 接近 42 电机上限 |

> 换算：`RPM = freq / (微步数 × 200) × 60`（1.8° 电机，200 全步/转）
> **STEP 模式**：`START` → TIM2 PWM 输出 STEP 到 PA1，**带缓升**（`Motor_Start`）。
> **VACTUAL 模式**：`START` → 写 **VACTUAL(0x22)**，**无硬件缓升**，高速请分步升频。

## 2. 方向设置（`DIR <0/1>`，写 GCONF.shaft）

```
DIR 0   → 正向（shaft=0）
DIR 1   → 反向（shaft=1）
```

## 3. 斩波模式切换（`CHOP <0/1>`，写 GCONF.en_SpreadCycle）

```
CHOP 0  → StealthChop（恒压 PWM，静音，低速扭矩优）
CHOP 1  → SpreadCycle（恒流斩波，高动态，高速/大力矩）
```

## 4. 微步细分（`MICRO <mres>`，写 CHOPCONF.MRES）

| mres | 微步 | 每 STEP 推进(1.8°) | 说明 |
|---|---|---|---|
| 0 | 1/256 | 0.007° | 最精细 |
| 1 | 1/128 | 0.014° | |
| 2 | 1/64 | 0.028° | |
| 3 | 1/32 | 0.056° | |
| 4 | 1/16 | 0.112° | 高动态常用 |
| **5** | **1/8** | **0.225°** | 默认 |
| 6 | 1/4 | 0.45° | |
| 8 | 整步 | 1.8° | |

## 5. TOFF / HEND(HDEC) 配置（`REGW 6C <完整CHOPCONF>`）

CHOPCONF 为整寄存器写，以下基于基础值 `0x15010000`（MRES=5、intpol=1、TBL=2）：
> 若已改 MRES，请自行按位叠加：toff=bit3:0、hstrt=bit6:4、hend=bit10:7、TBL=bit16:15、vsense=bit17、MRES=bit27:24、intpol=bit28。

### TOFF 设置
| TOFF 值 | 指令 | 适用 |
|---|---|---|
| 2 | `REGW 6C 0x15010002` | StealthChop 推荐 |
| 3 | `REGW 6C 0x15010003` | StealthChop 常用 |
| 5 | `REGW 6C 0x15010005` | SpreadCycle 推荐 |

### HEND(HDEC) 设置（保持 TOFF=2 基础）
| HEND 值 | 指令 | 说明 |
|---|---|---|
| 0 | `REGW 6C 0x15010002` | 无滞环（默认） |
| 1 | `REGW 6C 0x15010082` | 滞环 +1 |
| 5 | `REGW 6C 0x15010282` | 滞环 +5（平滑） |

## 6. 组合推荐

### StealthChop 静音模式（低速/静音）
```
CHOP 0
MICRO 5
REGW 6C 0x15010002     ← TOFF=2, HEND=0
START 800
```

### SpreadCycle 高动态模式（高速/大力矩）
```
CHOP 1
MICRO 4
REGW 6C 0x15010055     ← TOFF=5, HSTRT=5, HEND=0（AN001）
START 4000
```

## 7. 校验（确认写入生效）

```
REGR 6C    → OK:0x15010002   （查 CHOPCONF）
REGR 00    → OK:0x000000C0/C4（0x40 bit2=1 表示 SpreadCycle）
STATUS     → OK:xxxHz RUN    （当前速度/状态）
IOIN       → OK:VERSION=0x21 （通信确认）
```

## 8. 补充

| 指令 | 用途 |
|---|---|
| `IRUN <n>` / `IHOLD <n>` | 运行/静止电流（0~31） |
| `SG` / `DRV` | StallGuard / 驱动状态 |
| `REGW <addr> <val>` | 直接写任意寄存器（调试） |

---

*参考：TMC2209 数据手册 Rev.1.03；配置值均已按位验证。*
