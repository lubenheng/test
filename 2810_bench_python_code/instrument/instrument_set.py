"""
instrument_set.py - 硬件设备统一调用接口层

本脚本封装了以下硬件模块的函数调用，提供统一的接口给其他脚本使用：
  - SWD_2810_CMD: 寄存器读写（modify_register_range, read_register_32bit）
  - serial_comm: 串口通信（初始化、发送、接收、关闭）
  - Keysight_34465A: 万用表电压/电流测量
  - TBS_2204B: 示波器频率测量

使用方法：
    其他脚本仅需 import instrument_set，通过本模块间接调用硬件设备。
"""

import sys
from pathlib import Path

# 将项目根目录加入 sys.path，确保 instrument.fullpath 导入可用
_instr_parent = str(Path(__file__).parent.parent)
if _instr_parent not in sys.path:
    sys.path.insert(0, _instr_parent)

from instrument.debug import SWD_2810_CMD as _swd
from instrument.multimeter import Keysight_34465A as _dmm
from instrument.platform import serial_comm as _serial

from instrument.oscilloscope_api import DSOX_3024G as _osc


# =====================================================================
# SWD 寄存器操作
# =====================================================================

def swd_unlock_mu5():
    """
    MU5 解密准备流程（复位、halt、关闭WDT、等待解密完成）。
    调用后 CPU 处于 halt 状态。
    """
    _swd.unlock_mu5_decryption()

def swd_init():
    """
    初始化 SWD 连接。

    关闭已有 JLink 连接并重新连接 SWD，同时初始化各扇区对象。
    在调用其他 SWD 函数前必须先调用此函数。

    返回：
        bool - 初始化成功返回 True，失败返回 False
    """
    try:
        _swd.init_all()
        print("✅ SWD 初始化成功")
        swd_unlock_mu5()
        return True
    except Exception as e:
        print(f"❌ SWD 初始化失败：{e}")
        return False


def swd_read_register(reg_addr, return_hex=False):
    """
    读取指定地址的 32bit 寄存器值。

    参数：
        reg_addr: int/str - 寄存器地址（支持 0x40000180 / "0x40000180" / 1073741184）
        return_hex: bool - True 返回 hex 字符串，False 返回十进制整数
    返回：
        int/str/None - 寄存器值，失败返回 None
    """
    return _swd.read_register_32bit(reg_addr, return_hex=return_hex)


def swd_modify_register(reg_addr, value, start_bit, end_bit):
    """
    修改寄存器的指定位段（读-改-写）。

    参数：
        reg_addr: int/str - 寄存器地址
        value: int - 要写入的数值
        start_bit: int - 要修改的最低位 (0~31)
        end_bit: int - 要修改的最高位(0~31, 需 >= start_bit)
    """
    _swd.modify_register_range(reg_addr, value, start_bit, end_bit)


def swd_write_register(reg_addr, reg_values):
    """
    直接写入寄存器的值（全量覆盖，不保留原有值）。

    参数：
        reg_addr: int - 寄存器地址
        reg_values: int/list - 要写入的值（单个值或列表）
    """
    _swd.write_register(reg_addr, reg_values)


# =====================================================================
# 串口通信
# =====================================================================

def serial_init(port=None, baudrate=9600, timeout=2):
    """
    初始化串口连接。

    参数：
        port: str/None - 串口端口（如 "COM3"），None 则自动检测（使用第一个可用串口）
        baudrate: int - 波特率（默认 9600）
        timeout: int - 超时时间（秒，默认 2）
    返回：
        bool - 初始化成功返回 True，失败返回 False
    """
    return _serial.serial_init(port, baudrate, timeout) is not None


def serial_auto_init(baudrate=9600, timeout=2):
    """
    自动轮询所有可用串口，尝试初始化第一个可用的串口。

    遍历系统所有串口，逐个尝试打开，成功即停止。
    适用于不确定目标设备占用哪个串口的场景。

    参数：
        baudrate: int - 波特率（默认 9600）
        timeout: int - 超时时间（秒，默认 2）
    返回：
        bool - 找到可用串口并初始化成功返回 True，全部失败返回 False
    """
    ports = _serial.SerialComm.list_serial_ports()
    if not ports:
        print("错误: 未找到任何可用串口")
        return False

    print(f"检测到串口: {ports}，正在逐个尝试...")
    for p in ports:
        print(f"  尝试 {p} ...", end=" ")
        if _serial.serial_init(p, baudrate, timeout) is not None:
            print("成功")
            return True
        print("失败")
        _serial.serial_close()

    print("错误: 所有串口均无法连接")
    return False


def serial_send(data):
    """
    通过串口发送数据。

    参数：
        data: str/bytes - 要发送的数据
    返回：
        int - 发送的字节数，失败返回 -1
    """
    return _serial.serial_send(data)


def serial_receive(size=1024):
    """
    从串口接收数据。

    参数：
        size: int - 要读取的最大字节数（默认 1024）
    返回：
        bytes - 接收到的数据
    """
    return _serial.serial_receive(size)


def serial_send_and_receive(data, response_size=1024, delay=0.01):
    """
    发送数据并等待响应（组合操作）。

    参数：
        data: str/bytes - 要发送的数据
        response_size: int - 预期响应大小（默认 1024）
        delay: float - 发送后等待时间（秒，默认 0.01）
    返回：
        bytes - 响应数据
    """
    inst = _serial.get_serial_instance()
    if inst is None:
        print("错误: 串口未初始化")
        return b''
    return inst.send_and_receive(data, response_size, delay)


def serial_close():
    """
    关闭串口连接。
    """
    _serial.serial_close()


def serial_list_ports():
    """
    列出所有可用的串口。

    返回：
        list - 可用串口端口名列表
    """
    return _serial.SerialComm.list_serial_ports()


def serial_list_ports_detailed():
    """
    列出所有可用串口的详细信息（端口名、描述、硬件ID）。

    返回：
        list - 每个元素为 (device, description, hwid) 的元组列表
    """
    ports = []
    try:
        import serial.tools.list_ports
        for p in serial.tools.list_ports.comports():
            ports.append((p.device, p.description, p.hwid))
    except Exception as e:
        print(f"获取串口详细信息失败: {e}")
    return ports


def serial_find_ports_by_keyword(keyword):
    """
    通过关键词搜索所有匹配的串口（返回全部匹配项，而非第一个）。

    参数：
        keyword: str - 关键词（不区分大小写，如 "CH340"）
    返回：
        list - 匹配的串口 (device, description) 列表
    """
    return _serial.SerialComm.find_serial_ports_by_description(keyword)


def serial_find_port(keyword):
    """
    通过关键词搜索串口。

    参数：
        keyword: str - 关键词（不区分大小写，如 "USB"、"COM"）
    返回：
        str/None - 匹配的串口名，未找到返回 None
    """
    return _serial.SerialComm.find_serial_port_by_description(keyword)


def serial_get_port():
    """
    获取当前已初始化的串口号。

    返回：
        str/None - 串口名（如 "COM3"），未初始化或失败返回 None
    """
    inst = _serial.get_serial_instance()
    if inst:
        return inst.port
    return None


# =====================================================================
# Keysight 34465A 万用表
# =====================================================================

def dmm_init():
    """
    初始化 Keysight 34465A 万用表。

    返回：
        bool - 初始化成功返回 True,失败返回 False
    """
    return _dmm.init_keysight_34465a()


def dmm_measure_voltage(voltage_type="DC", count=3, range_val="10V",
                        resolution=0.00001, limit_low=0, limit_upp=0):
    """
    测量电压（多采样点）。

    参数：
        voltage_type: str - "DC" 或 "AC"
        count: int - 采样数
        range_val: str - 量程（如 "10V", "100V", "1V" 等）
        resolution: float - 分辨率
        limit_low: float - 下限阈值
        limit_upp: float - 上限阈值
    返回：
        list - 电压测量值列表（单位 V），失败返回空列表
    """
    return _dmm.read_keysight_voltage(
        voltage_type=voltage_type,
        count=count,
        range_val=range_val,
        resolution=resolution,
        limit_low=limit_low,
        limit_upp=limit_upp,
    )


def dmm_measure_single_voltage(voltage_type="DC", range_val="10V",
                               resolution=0.00001, limit_low=0, limit_upp=0):
    """
    单次测量电压，返回值为 mV。

    参数：
        voltage_type: str - "DC" 或 "AC"
        range_val: str - 量程（如 "10V", "100V", "1V" 等）
        resolution: float - 分辨率
        limit_low: float - 下限阈值
        limit_upp: float - 上限阈值
    返回：
        float/None - 电压值（mV），失败返回 None
    """
    return _dmm.read_single_voltage(
        voltage_type=voltage_type,
        range_val=range_val,
        resolution=resolution,
        limit_low=limit_low,
        limit_upp=limit_upp,
    )


def dmm_quick_measure_voltage():
    """
    快速单次测量DC电压，返回mV值。
    跳过万用表重新配置(CONFigure/触发源/采样数等)，仅执行 INIT→*TRG→FETCH 快速采集。
    适用于连续多次测量场景（如ADC/DNL扫描），可在首次 dmm_measure_single_voltage 之后调用。

    返回：
        float/None - 电压值（mV），失败返回 None
    """
    if _dmm.dmm_instance is None:
        print("错误：万用表未初始化！请先调用 dmm_init()")
        return None
    try:
        data = _dmm.dmm_instance.VoltageQuickMeasure()
        if data:
            return data[0] * 1000.0  # V → mV
        return None
    except Exception as e:
        print(f"快速电压测量失败: {e}")
        return None


def dmm_measure_single_current(current_type="DC", range_val="1A", resolution="DEF"):
    """
    单次测量电流，返回值为 mA。

    参数：
        current_type: str - "DC" 或 "AC"
        range_val: str - 量程（如 "1A", "10mA", "100mA" 等）
        resolution: str/float - 分辨率（默认 "DEF"）
    返回：
        float/None - 电流值（mA），失败返回 None
    """
    return _dmm.read_single_current(
        current_type=current_type,
        range_val=range_val,
        resolution=resolution,
    )


def dmm_release():
    """
    释放 Keysight 34465A 资源（关闭连接）。

    返回：
        bool - 释放成功返回 True，失败返回 False
    """
    return _dmm.release_keysight_resource()


# =====================================================================
# 示波器
# =====================================================================

def osc_init(probe_attenuation=10, vertical_scale=5.0, horizontal_scale=1e-6):
    """
    初始化示波器。参数：
        probe_attenuation: int/float - 探头衰减比（1X=1, 10X=10）
        vertical_scale: float - 垂直量程（V/格）
        horizontal_scale: float - 水平时基（s/格）
    """
    _osc.init_oscilloscope(
        probe_attenuation=probe_attenuation,
        vertical_scale=vertical_scale,
        horizontal_scale=horizontal_scale,
    )


def osc_auto_set():
    """
    执行示波器的 AutoSet 自动设置。

    调用后自动调整示波器的垂直/水平/触发设置以稳定显示波形。
    通常在切换测量通道或信号变化后调用。
    """
    _osc.measure_once_auto_set()


def osc_measure_frequency(channel="CH1"):
    """
    测量指定通道的频率。

    参数：
        channel: str - 通道名（"CH1"/"CH2"/"CH3"/"CH4"）
    返回：
        (freq, period, duty) - 频率(Hz), 周期(us), 占空比(%)；
        测量失败返回 (None, None, None)
    """
    return _osc.measure_once(channel)


def osc_close():
    """
    关闭示波器连接。
    """
    _osc.close_oscilloscope()


# =====================================================================
# 便捷组合函数
# =====================================================================

def dmm_voltage_once(voltage_type="DC", range_val="10V"):
    """
    快速单次电压测量（最简参数），返回 mV。

    等效于 dmm_measure_single_voltage()，但参数更精简。
    """
    return dmm_measure_single_voltage(
        voltage_type=voltage_type, range_val=range_val
    )


def dmm_current_once(current_type="DC", range_val="1A"):
    """
    快速单次电流测量（最简参数），返回 mA。
    """
    return dmm_measure_single_current(
        current_type=current_type, range_val=range_val
    )


def osc_freq(channel="CH1"):
    """
    快速测量指定通道频率（仅返回频率值，单位 Hz）。

    参数：
        channel: str - 通道名
    返回：
        float/None - 频率值（Hz），失败返回 None
    """
    freq, _, _ = _osc.measure_once(channel)
    return freq


# =====================================================================
# 模块导出列表
# =====================================================================

__all__ = [
    # SWD
    "swd_init",
    "swd_read_register",
    "swd_modify_register",
    "swd_write_register",
    "swd_unlock_mu5",
    # 串口
    "serial_init",
    "serial_auto_init",
    "serial_send",
    "serial_receive",
    "serial_send_and_receive",
    "serial_close",
    "serial_list_ports",
    "serial_find_port",
    "serial_get_port",
    # 万用表
    "dmm_init",
    "dmm_measure_voltage",
    "dmm_measure_single_voltage",
    "dmm_quick_measure_voltage",
    "dmm_measure_single_current",
    "dmm_release",
    "dmm_voltage_once",
    "dmm_current_once",
    # 示波器
    "osc_init",
    "osc_auto_set",
    "osc_measure_frequency",
    "osc_close",
    "osc_freq",
]
