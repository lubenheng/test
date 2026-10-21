"""
OCD5010 模块使能控制脚本
通过模块名/编号控制对应使能寄存器的置位/清位（整体置位操作）

用法:
    python module_enable.py <编号/名称/范围> [...] <1|0>
    python module_enable.py --list            # 列出所有支持的模块

示例:
    python module_enable.py 3 1               # 使能 ADC（编号3）
    python module_enable.py 1-15 1            # 使能编号1~15的模块
    python module_enable.py 16-18 0           # 关闭 GPIOA/B/C
    python module_enable.py ADC 1             # 使能 ADC（名称）
    python module_enable.py HSE HSI PLL 1     # 批量使能
"""
import sys
import time
from pathlib import Path

# 确保 project root 在 sys.path 中
_project_root = str(Path(__file__).parents[3])
if _project_root not in sys.path:
    sys.path.insert(0, _project_root)

from instrument.debug import SWD_CMD


# ============================================================
# PLL 时钟配置表
# PLL基准时钟 = HSI 16MHz
# PLL输出频率 = 16MHz x (PLLMULT + 5) / PLLDIV
# ============================================================
PLL_FREQ_TABLE = [
    # (显示名称, 目标频率MHz, PLLMULT, PLLDIV, PLLCR1写入值, SYSCKSW值)
    ("PLL_48M",  48, 1, 2, 0x0102, 0b10),
    ("PLL_64M",  64, 3, 2, 0x0302, 0b10),
    ("PLL_80M",  80, 5, 2, 0x0502, 0b10),
    ("PLL_96M",  96, 7, 2, 0x0702, 0b10),
]

# PLL 相关寄存器地址
PLLCR0_ADDR = 0x40020040   # PLL控制寄存器0 (bit0=PLLEN, bit15=PLLRDY, bit17=RO_PLL_RDY)
PLLCR1_ADDR = 0x40020044   # PLL控制寄存器1 (bit[2:0]=PLLDIV, bit[12:8]=PLLMULT)
SYSCKCR_ADDR = 0x40020050  # 系统时钟控制寄存器 (bit[1:0]=SYSCKSW)


def configure_pll_clock(pll_config):
    """
    配置系统时钟为指定PLL频率

    Args:
        pll_config: PLL_FREQ_TABLE中的一个元组 (名称, 频率, mult, div, pllcr1_val, syscksw)
    Returns:
        True=成功, False=失败
    """
    name, freq_mhz, mult, div, pllcr1_val, syscksw = pll_config

    print(f"\n{'='*50}")
    print(f"  配置系统时钟为 {name} ({freq_mhz} MHz)")
    print(f"  PLLMULT={mult}, PLLDIV={div}, PLLCR1=0x{pllcr1_val:04X}")
    print(f"{'='*50}")

    # Step 1: 关闭PLL (写PLLRDY=1来关闭)
    print("\n[Step 1] 关闭PLL...")
    pllcr0 = SWD_CMD.read_register_32bit(PLLCR0_ADDR)
    if pllcr0 is None:
        print("  ✗ 读取PLLCR0失败!")
        return False
    SWD_CMD.write_register(PLLCR0_ADDR, pllcr0 | (1 << 15))
    time.sleep(0.05)
    print(f"  ✓ PLLCR0: 0x{pllcr0:08X} -> 0x{pllcr0 | (1 << 15):08X}")

    # Step 2: 配置PLL倍频/分频
    print("\n[Step 2] 配置PLL倍频/分频...")
    # 先清零PLLMULT和PLLDIV位域，再写入新值
    pllcr1 = SWD_CMD.read_register_32bit(PLLCR1_ADDR)
    if pllcr1 is None:
        print("  ✗ 读取PLLCR1失败!")
        return False
    pllcr1 = (pllcr1 & ~0x1F07) | pllcr1_val  # 清除bit[12:8]和bit[2:0]后写入
    SWD_CMD.write_register(PLLCR1_ADDR, pllcr1)
    time.sleep(0.05)
    print(f"  ✓ PLLCR1: 0x{pllcr1:08X}")

    # Step 3: 使能PLL
    print("\n[Step 3] 使能PLL...")
    pllcr0 = SWD_CMD.read_register_32bit(PLLCR0_ADDR)
    if pllcr0 is None:
        print("  ✗ 读取PLLCR0失败!")
        return False
    SWD_CMD.write_register(PLLCR0_ADDR, pllcr0 | (1 << 0))
    time.sleep(0.05)
    print(f"  ✓ PLLCR0: 0x{pllcr0:08X} -> 0x{pllcr0 | 1:08X}")

    # Step 4: 等待PLL锁定 (查询RO_PLL_RDY直到为1)
    print("\n[Step 4] 等待PLL锁定...")
    for retry in range(200):
        pllcr0 = SWD_CMD.read_register_32bit(PLLCR0_ADDR)
        if pllcr0 is None:
            print("  ✗ 读取PLLCR0失败!")
            return False
        if (pllcr0 >> 17) & 1:  # RO_PLL_RDY = bit[17]
            print(f"  ✓ PLL已锁定 (retry={retry})")
            break
        time.sleep(0.01)
    else:
        print("  ✗ 等待PLL锁定超时!")
        #return False

    # Step 5: 切换系统时钟源为PLL
    print(f"\n[Step 5] 切换系统时钟源为PLL (SYSCKSW={syscksw:#04b})...")
    sysckcr = SWD_CMD.read_register_32bit(SYSCKCR_ADDR)
    if sysckcr is None:
        print("  ✗ 读取SYSCKCR失败!")
        return False
    sysckcr = (sysckcr & ~0x3) | syscksw  # 清除bit[1:0]后写入
    SWD_CMD.write_register(SYSCKCR_ADDR, sysckcr)
    time.sleep(0.05)
    print(f"  ✓ SYSCKCR: 0x{sysckcr:08X}")

    # 回读验证
    readback_pllcr1 = SWD_CMD.read_register_32bit(PLLCR1_ADDR)
    readback_sysckcr = SWD_CMD.read_register_32bit(SYSCKCR_ADDR)
    print(f"\n[验证] PLLCR1=0x{readback_pllcr1:08X}, SYSCKCR=0x{readback_sysckcr:08X}")
    print(f"[完成] 系统时钟已切换为 {name} ({freq_mhz} MHz)")
    return True


def configure_hsi_clock():
    """
    切换系统时钟为HSI (SYSCKSW=00b)
    """
    print(f"\n{'='*50}")
    print("  切换系统时钟为 HSI")
    print(f"{'='*50}")

    sysckcr = SWD_CMD.read_register_32bit(SYSCKCR_ADDR)
    if sysckcr is None:
        print("  ✗ 读取SYSCKCR失败!")
        return False
    sysckcr = sysckcr & ~0x3  # 清除bit[1:0]，SYSCKSW=00b选择HSI
    SWD_CMD.write_register(SYSCKCR_ADDR, sysckcr)
    time.sleep(0.05)

    readback = SWD_CMD.read_register_32bit(SYSCKCR_ADDR)
    print(f"  ✓ SYSCKCR: 0x{readback:08X} (SYSCKSW=00b)")
    print(f"[完成] 系统时钟已切换为 HSI")
    return True


def ask_clock_source():
    """
    在SWD初始化后询问用户选择系统时钟源
    - HSI: 切换系统时钟为HSI (SYSCKSW=00b)
    - PLL: 进一步询问选择哪个PLL频率，然后配置
    """
    print("\n" + "=" * 50)
    print("  系统时钟源选择")
    print("=" * 50)
    print("  HSI = 内部32MHz RC时钟")
    print("  PLL = 锁相环倍频时钟 (需配置)")
    print("=" * 50)

    while True:
        try:
            choice = input("\n请选择系统时钟 [HSI/PLL] (直接回车默认HSI): ").strip().upper()
        except (EOFError, KeyboardInterrupt):
            print("\n默认使用HSI")
            configure_hsi_clock()
            return

        if choice in ("", "HSI"):
            configure_hsi_clock()
            return

        if choice == "PLL":
            # 显示PLL频率选项
            print("\n可用PLL时钟频率:")
            print("-" * 45)
            for i, (name, freq, mult, div, _, _) in enumerate(PLL_FREQ_TABLE, 1):
                print(f"  [{i}] {name} - {freq} MHz  "
                      f"(PLLMULT={mult}, PLLDIV={div})")
            print("-" * 45)

            while True:
                try:
                    freq_choice = input("请选择PLL频率 [1-4]: ").strip()
                except (EOFError, KeyboardInterrupt):
                    print("\n取消PLL配置，使用默认HSI")
                    return

                if freq_choice.isdigit() and 1 <= int(freq_choice) <= len(PLL_FREQ_TABLE):
                    pll_config = PLL_FREQ_TABLE[int(freq_choice) - 1]
                    configure_pll_clock(pll_config)
                    return
                else:
                    print(f"[错误] 请输入 1~{len(PLL_FREQ_TABLE)} 之间的数字")
        else:
            print("[错误] 请输入 HSI 或 PLL")


# ============================================================
# 模块使能配置表（有序列表，编号从 1 开始）
# 格式: (名称, 寄存器绝对地址, 使能位号, 模块全名, 备注)
# ============================================================
MODULE_LIST = [
    # --- 【RRAM(存储)】 ---
    # 1
    ("RRAM",       0x40020058,  0, "RRAM / Flash控制器(FMC)", "存储(RRAM)由Flash控制器管理。关闭后不可访问Flash/RRAM"),
    # 2
    ("FMC",        0x40020058,  0, "RRAM / Flash控制器(FMC)", "RRAM别名"),

    # --- 【模拟部分】 ---
    # 3
    ("ADC",        0x40007C00, 19, "ADC",              "ADC模拟前端电源开关(重要)"),
    # 4
    ("CSA",        0x40007C00, 17, "CSA(电流采样放大)", "CSA全局模拟使能"),
    # 5
    ("Bandgap",    0x40007C00, 23, "带隙基准(Bandgap)",  "带隙基准电源开关"),
    # 6
    ("VREF",       0x40007C00, 24, "参考电压(VREF)",    "参考电压电源开关"),
    # 7
    ("LDO12",      0x40007C00, 25, "LDO12",            "LDO12电源"),
    # 8
    ("PVD",        0x40007C00, 14, "PVD/LVD检测",       "电源电压检测"),
    # 9
    ("TJ",         0x40007C00, 15, "结温检测(TJ)",       "结温过温检测"),
    # 10
    ("TS",         0x40007C00, 16, "温度传感器(TS)",     "外部温度传感器(NTC)"),
    # 11
    ("HSI",        0x40007C00,  26, "HSI振荡器(32MHz)",   "内部32MHz RC时钟"),
    # 12
    ("HSE",        0x40007C00,  20, "HSE振荡器",          "外部高速晶振"),
    # 13
    ("LSI",        0x40007C00,  3, "LSI振荡器(32KHz)",   "内部32KHz低速时钟"),
    # 14
    ("PLL",        0x40007C00,  21, "PLL",               "锁相环(倍频时钟源)"),
    # 15
    ("ANA_EN",     0x40007C00,  0, "模拟模块总开关",     "模拟部分总电源开关,关闭后全部模拟模块掉电"),

    # --- 【数字部分】 ---
    # 16
    ("GPIOA",      0x40020058, 16, "GPIO A", ""),
    # 17
    ("GPIOB",      0x40020058, 17, "GPIO B", ""),
    # 18
    ("GPIOC",      0x40020058, 18, "GPIO C", ""),
    # 19
    ("CAN",        0x40020064,  0, "CAN",   ""),
    # 20
    ("UART0",      0x40020064,  8, "UART0",  ""),
    # 21
    ("UART1",      0x40020064,  9, "UART1",  ""),
    # 22
    ("SPI0",       0x40020064, 16, "SPI0",   ""),
    # 23
    ("SPI1",       0x40020064, 17, "SPI1",   ""),
    # 24
    ("I2C0",       0x40020064, 24, "I2C0",   ""),
    # 25
    ("I2C1",       0x40020064, 25, "I2C1",   ""),
    # 26
    ("TIM13",      0x40020068, 13, "TIM13",  ""),
    # 27
    ("TIM14",      0x40020068, 14, "TIM14",  ""),
    # 28
    ("TIM15",      0x40020068, 15, "TIM15",  ""),
    # 29
    ("TIM16",      0x40020068, 16, "TIM16",  ""),
    # 30
    ("MCPWM",      0x40020068, 24, "MCPWM(电机PWM)", "复杂模块, 运行由时基/更新等多寄存器控制"),
    # 31
    ("HALL",       0x40020060,  1, "HALL(霍尔)", ""),
    # 32
    ("QEP",        0x40020060,  2, "QEP(正交编码)", ""),
    # 33
    ("HWMATH",     0x40020060,  0, "HWMATH(硬件数学)", "无独立模块使能位"),
    # 34
    ("IWDG",       0x40020060, 16, "IWDG(看门狗)", ""),
    # 35
    ("ADC_CLK",    0x4002005C,  0, "ADC控制(数字接口时钟)", "ADC数字接口时钟"),
]

# 用名称快速查找的字典（自动构建）
MODULE_TABLE = {item[0]: item[1:] for item in MODULE_LIST}


def list_modules():
    """打印所有支持的模块列表（带编号）"""
    print("=" * 75)
    print(f"{'编号':<6} {'模块名':<12} {'寄存器地址':<14} {'位号':<6} {'全名':<25} {'备注'}")
    print("=" * 75)
    for i, (name, addr, bit, full_name, note) in enumerate(MODULE_LIST, 1):
        print(f"[{i:<3}] {name:<12} {hex(addr):<14} [{bit:<3}] {full_name:<25} {note}")
    print("=" * 75)


def _lookup_module(token: str):
    """
    查找模块配置，支持名称或编号
    返回 (reg_addr, bit_pos, full_name, note) 或 None
    """
    # 尝试按编号查找
    if token.isdigit():
        idx = int(token) - 1  # 编号从 1 开始
        if 0 <= idx < len(MODULE_LIST):
            item = MODULE_LIST[idx]
            return item[1:]  # (reg_addr, bit_pos, full_name, note)
        return None

    # 按名称查找（精确匹配）
    if token in MODULE_TABLE:
        return MODULE_TABLE[token]

    # 大小写不敏感匹配
    key = token.upper()
    for name, info in MODULE_TABLE.items():
        if name.upper() == key:
            return info
    return None


def _expand_token(token: str) -> list:
    """
    展开输入 token，支持范围语法
    "3"     -> ["3"]
    "1-5"   -> ["1", "2", "3", "4", "5"]
    "ADC"   -> ["ADC"]
    """
    if "-" in token and not token.startswith("-"):
        parts = token.split("-", 1)
        if len(parts) == 2 and parts[0].isdigit() and parts[1].isdigit():
            start, end = int(parts[0]), int(parts[1])
            if start <= end:
                return [str(i) for i in range(start, end + 1)]
    return [token]


def module_enable(module_names: list, enable: bool):
    """
    逐个控制模块的使能寄存器置位/清位，每次操作间隔100ms

    Args:
        module_names: 模块名列表（如 ["ADC", "CSA", "Bandgap"]）
        enable: True=使能(置1), False=关闭(清0)
    """
    action = "使能" if enable else "关闭"
    all_ok = True
    total = len(module_names)

    for i, name in enumerate(module_names):
        target = _lookup_module(name)
        if target is None:
            print(f"[错误] 未找到模块: '{name}'")
            all_ok = False
            continue

        reg_addr, bit_pos, full_name, note = target

        # 读取当前值
        old_val = SWD_CMD.read_register_32bit(reg_addr)
        if old_val is None:
            print(f"  [{i+1}/{total}] ✗ {action} [{bit_pos}] {full_name}  (读取寄存器失败!)")
            all_ok = False
            continue

        # 置位或清位
        if enable:
            new_val = old_val | (1 << bit_pos)
        else:
            new_val = old_val & ~(1 << bit_pos)

        # 写入
        SWD_CMD.write_register(reg_addr, new_val)

        # 回读验证
        readback = SWD_CMD.read_register_32bit(reg_addr)
        if readback is None:
            print(f"  [{i+1}/{total}] ✗ {action} [{bit_pos}] {full_name}  (回读寄存器失败!)")
            all_ok = False
            continue
        bit_state = (readback >> bit_pos) & 1
        expected = 1 if enable else 0
        status = "✓" if bit_state == expected else "✗"

        if bit_state != expected:
            all_ok = False

        print(f"  [{i+1}/{total}] {status} {action} [{bit_pos}] {full_name}  "
              f"({hex(reg_addr)}: {hex(old_val)} -> {hex(new_val)}, 回读={hex(readback)})")

        # 批量操作时每个模块间隔100ms
        if i < total - 1:
            time.sleep(0.1)

    print()
    if all_ok:
        print(f"[结果] ✓ 全部{action}成功 ({total}个模块)")
    else:
        print(f"[结果] ✗ 部分{action}失败!")
    return all_ok


def print_usage():
    """打印使用说明"""
    print(__doc__)


def interactive_loop():
    """交互式循环：持续输入模块名和使能/关闭操作"""
    print("=" * 60)
    print("  OCD5010 模块使能控制 - 交互模式")
    print("=" * 60)
    print("输入格式: <编号/名称/范围> [...] <1|0>")
    print("  1 = 使能, 0 = 关闭")
    print("  支持编号、名称、范围，支持批量操作")
    print("  例: 3 1          使能 ADC")
    print("      1-15 1       使能编号1~15的模块")
    print("      16-18 0      关闭 GPIOA/B/C")
    print("      ADC CSA 1    用名称批量使能")
    print("特殊命令:")
    print("  list / ls  - 列出所有模块(带编号)")
    print("  hsi        - 切换系统时钟为HSI")
    print("  pll        - 切换系统时钟为PLL(可选频率)")
    print("  quit / q   - 退出")
    print("=" * 60)
    print()
    list_modules()

    # 初始化 SWD
    print("[信息] 正在初始化 SWD 连接...")
    SWD_CMD.init_all()
    print("[信息] SWD 初始化完成")

    # 询问系统时钟源选择 (HSI 或 PLL)
    ask_clock_source()

    while True:
        try:
            user_input = input("\n>>> ").strip()
        except (EOFError, KeyboardInterrupt):
            print("\n退出")
            break

        if not user_input:
            continue

        # 特殊命令
        if user_input.lower() in ("quit", "q", "exit"):
            print("退出")
            break
        if user_input.lower() in ("list", "ls"):
            list_modules()
            continue
        if user_input.lower() == "hsi":
            configure_hsi_clock()
            continue
        if user_input.lower() == "pll":
            # 显示PLL频率选项
            print("\n可用PLL时钟频率:")
            print("-" * 45)
            for i, (name, freq, mult, div, _, _) in enumerate(PLL_FREQ_TABLE, 1):
                print(f"  [{i}] {name} - {freq} MHz  "
                      f"(PLLMULT={mult}, PLLDIV={div})")
            print("-" * 45)
            while True:
                try:
                    freq_choice = input("请选择PLL频率 [1-4]: ").strip()
                except (EOFError, KeyboardInterrupt):
                    print("\n取消PLL配置")
                    break
                if freq_choice.isdigit() and 1 <= int(freq_choice) <= len(PLL_FREQ_TABLE):
                    pll_config = PLL_FREQ_TABLE[int(freq_choice) - 1]
                    configure_pll_clock(pll_config)
                    break
                else:
                    print(f"[错误] 请输入 1~{len(PLL_FREQ_TABLE)} 之间的数字")
            continue

        # 解析输入: "3 1" 或 "1-5 1" 或 "3 4 5 1" 或 "ADC CSA 1"
        parts = user_input.split()
        if len(parts) < 2:
            print("[错误] 格式: <编号/名称/范围> [...] <1|0>")
            print("  例: 3 1  或  1-15 1  或  11 12 13 1  或  ADC CSA 1")
            continue

        enable_str = parts[-1]
        tokens = parts[:-1]
        if enable_str not in ("0", "1"):
            print(f"[错误] 使能参数必须是 0 或 1, 收到: '{enable_str}'")
            continue

        # 展开范围语法（如 "1-5" -> ["1","2","3","4","5"]）
        module_names = []
        for t in tokens:
            module_names.extend(_expand_token(t))

        enable = enable_str == "1"
        module_enable(module_names, enable)


if __name__ == "__main__":
    if len(sys.argv) >= 2:
        # 命令行模式
        if sys.argv[1] in ("--list", "-l", "list"):
            list_modules()
            sys.exit(0)
        if len(sys.argv) < 3:
            print("[错误] 参数不足")
            print("用法: python module_enable.py <模块名> [模块名2 ...] <1|0>")
            print("      python module_enable.py HSE HSI PLL 1")
            print("      python module_enable.py --list")
            sys.exit(1)

        enable_str = sys.argv[-1]
        tokens = sys.argv[1:-1]
        if enable_str not in ("0", "1"):
            print(f"[错误] 使能参数必须是 0 或 1, 收到: '{enable_str}'")
            sys.exit(1)

        # 展开范围语法
        module_names = []
        for t in tokens:
            module_names.extend(_expand_token(t))

        SWD_CMD.init_all()
        # 询问系统时钟源选择 (HSI 或 PLL)
        print("[信息] SWD 初始化完成")
        ask_clock_source()
        success = module_enable(module_names, enable_str == "1")
        sys.exit(0 if success else 1)
    else:
        # 交互模式
        interactive_loop()
