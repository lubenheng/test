"""
chip_b 芯片参数与初始化接口（模板）

新芯片参照此文件实现自己的初始化流程。
"""

# =====================================================================
# 芯片标识（显示在 main.py 菜单标题中）
# =====================================================================
CHIP_LABEL = "Chip-B"

# =====================================================================
# JLink / 串口参数（按实际芯片修改）
# =====================================================================
JLINK_DEVICE = "YOUR_DEVICE_NAME"
JLINK_SPEED = 1000
SERIAL_KEYWORD = "CH340"
SERIAL_BAUDRATE = 115200


def init_all(instr):
    """芯片全套初始化（main.py 入口调用）

    参数:
        instr: instrument_set 模块引用
    返回:
        bool - 全部初始化成功返回 True
    """
    # ================================================================
    # 在这里实现芯片特有的初始化流程:
    #   1. 串口初始化
    #   2. SWD 连接 + 解密
    #   3. 万用表初始化
    #   4. 示波器初始化
    #   5. 其他芯片专属初始化
    # ================================================================
    print(f"🔌 正在初始化 {CHIP_LABEL}...")

    # 示例：串口初始化
    ports = instr.serial_find_ports_by_keyword(SERIAL_KEYWORD)
    if ports:
        instr.serial_init(ports[0][0], baudrate=SERIAL_BAUDRATE)

    # 示例：SWD 初始化
    instr.swd_init()

    # 示例：万用表 + 示波器
    instr.dmm_init()
    instr.osc_init()

    return True
