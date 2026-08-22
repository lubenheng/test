"""
OCD5010 芯片参数与初始化接口

芯片专属的参数和初始化流程都放在这里。
instrument/ 是纯通用层，不做任何芯片特定的操作。
"""

from instrument.platform.serial_init import serial_find_and_init
from instrument.platform.init_helper import check_init

# =====================================================================
# 芯片标识
# =====================================================================
CHIP_LABEL = "OCD5010"

# =====================================================================
# JLink 烧录参数
# =====================================================================
JLINK_DEVICE = "DRV3_INFO"       # JLink 识别的设备名
JLINK_SPEED = 1000               # SWD 时钟 (kHz)

# =====================================================================
# 串口参数
# =====================================================================
SERIAL_KEYWORD = "CH34"          # 用于自动搜索串口（匹配 CH340/CH341/CH343 系列，设备实际为 CH343）
SERIAL_BAUDRATE = 115200         # 标准波特率（原 121500 为非标准值，CH343 不支持）

# =====================================================================
# SWD 初始化（含芯片解密）
# =====================================================================

def init_swd(instr):
    """
    初始化 SWD 连接。

    OCD5010 暂无可执行解密流程，仅建立 SWD 连接。
    后续如需专属解密在此处追加即可。

    参数：
        instr: instrument_set 模块引用
    返回：
        bool - SWD 初始化成功返回 True
    """
    return instr.swd_init()

# =====================================================================
# 全套初始化（main.py 入口调用）
# =====================================================================

def init_all(instr):
    """OCD5010 全套初始化：SWD + 解密 → 外接设备（串口、万用表、示波器）

    参数：
        instr: instrument_set 模块引用
    返回：
        bool - 全部初始化成功返回 True
    """
    check_init("SWD", init_swd(instr))
    check_init("串口", serial_find_and_init(keyword=SERIAL_KEYWORD, baudrate=SERIAL_BAUDRATE))
    check_init("万用表", instr.dmm_init(),
               confirm_prompt=f"当前万用表驱动：{instr._dmm.__name__.split('.')[-1]}，"
                             "请确认与实际连接的设备一致")
    check_init("示波器", instr.osc_init(),
               confirm_prompt=f"当前示波器驱动：{instr._osc.__name__.split('.')[-1]}，"
                             "请确认与实际连接的设备一致")
    return True
