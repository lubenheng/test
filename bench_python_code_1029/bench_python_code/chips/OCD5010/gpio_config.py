"""
OCD5010 GPIO 测试前置配置

PA5/PA6 配置为 输出 + 复用 GPIO_ALT_FUNC_14 (=0xE)。
trim 测量时模拟信号经 RW_DTP_MUX 输出到 GPIO5 (部分经 GPIO6),
需要先把 GPIO 控制器这一层配好: 开时钟 → 方向输出 → 复用 AF14。

寄存器级说明见 gpio_pa5_pa6_altfunc14_register_notes.md:
  1. AHB_CLK_EN (0x40020058) |= 0x00070000       开 GPIOA/B/C 时钟
  2. OEC_STR    (0x48000018) |= 0x20|0x40         方向: 输出使能 clear
  3. OES_STR    (0x48000014) |= 0x20|0x40         方向: 输出使能 set
  4. AFC_STR    (0x4800004C) |= 0x20|0x40         复用功能控制置位 (保险写法)
  5. AF0_STR    (0x4800003C) 清 bits[27:20] 后写   PA5/PA6 各 4bit = 0xE

由需要 PA5/PA6 输出的测试在开头调用 (幂等, 可重复调用)。
"""

import time


# GPIOA 基址 0x48000000
_AHB_CLK_EN = 0x40020058    # bit16/17/18 = GPIOA/B/C 时钟使能
_GPIOA_OEC  = 0x48000018    # 输出使能 clear, bit[n]=pin n
_GPIOA_OES  = 0x48000014    # 输出使能 set,   bit[n]=pin n
_GPIOA_AFC  = 0x4800004C    # 复用功能控制,   bit[n]=pin n, 写1 清复用/配合 AFx
_GPIOA_AF0  = 0x4800003C    # 复用功能 0,     pin0~7 各占 4bit

_ALT_FUNC_14 = 0xE          # GPIO_ALT_FUNC_14 枚举值
_PA5_PA6_MASK = (1 << 5) | (1 << 6)     # 0x60


def init_pa5_pa6_altfunc14():
    """
    测试前置: 开启 GPIOA 时钟, 配置 PA5/PA6 为 输出 + 复用 ALT_FUNC_14。

    顺序与 SDK gpio_init 一致: 先方向、后复用; 复用前先写 AFC_STR 置位
    (硬件要求, 见笔记第 7 节)。全部为位操作, 幂等。

    返回: True=配置完成 (读回 AF0_STR 校验 PA5/PA6 = 0xE)
    """
    from instrument.debug import SWD_CMD

    print("  [GPIO 前置] 配置 PA5/PA6 为 输出 + 复用 ALT_FUNC_14...")

    # 1. 开启 GPIOA/B/C 时钟 (AHB_CLK_EN bits 16-18, 保留其他位)
    SWD_CMD.modify_register_range(_AHB_CLK_EN, 0x7, 16, 18)
    time.sleep(0.5)

    # 2. 方向: 输出使能 (与 SDK gpio_drv_direction_cfg 一致, 先 OEC 后 OES)
    SWD_CMD.write_register(_GPIOA_OEC, _PA5_PA6_MASK)
    SWD_CMD.write_register(_GPIOA_OES, _PA5_PA6_MASK)
    time.sleep(0.001)

    # 3. 复用功能控制置位 (保险写法, 保证后续 AF0 写入生效)
    SWD_CMD.write_register(_GPIOA_AFC, _PA5_PA6_MASK)
    time.sleep(0.001)

    # 4. AF0_STR: PA5/PA6 各占4bit字段(bits[23:20]=PA5, bits[27:24]=PA6), 分别写 0xE
    SWD_CMD.modify_register_range(_GPIOA_AF0, _ALT_FUNC_14, 20, 23)
    SWD_CMD.modify_register_range(_GPIOA_AF0, _ALT_FUNC_14, 24, 27)
    time.sleep(0.001)

    # 校验: 读回 AF0_STR 确认 PA5/PA6 = 0xE
    check = SWD_CMD.read_register_32bit(_GPIOA_AF0)
    pa5 = (check >> 20) & 0xF if check is not None else -1
    pa6 = (check >> 24) & 0xF if check is not None else -1
    print(f"    AF0_STR = 0x{check:08X} (PA5={pa5:#x}, PA6={pa6:#x})" if check is not None
          else "    AF0_STR 读取失败")
    ok = (pa5 == _ALT_FUNC_14 and pa6 == _ALT_FUNC_14)
    if ok:
        print("  ✅ PA5/PA6 已配置为 ALT_FUNC_14")
    else:
        print("  ❌ PA5/PA6 复用配置校验失败, 请检查 SWD/寄存器")

    # GPIO 配置稳定延时: 等待寄存器写完成后再开始测量
    time.sleep(0.05)
    return ok


if __name__ == "__main__":
    init_pa5_pa6_altfunc14()
