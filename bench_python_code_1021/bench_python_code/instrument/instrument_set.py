"""
instrument_set.py - 硬件设备统一调用接口层

本脚本封装了以下硬件模块的函数调用，提供统一的接口给其他脚本使用：
  - SWD_CMD: 寄存器读写（modify_register_range, read_register_32bit）
  - serial_comm: 串口通信（初始化、发送、接收、关闭）
  - Keysight_34465A / RIGOL_DM3068: 万用表电压/电流测量
  - TBS_2204B: 示波器频率测量
  - RIGOL DG1022Z: 信号发生器方波/PWM 输出（频率、占空比、幅度、偏置）
  - KEITHLEY 2460: 源表连接与安全初始化（输出关闭）

使用方法：
    其他脚本仅需 import instrument_set，通过本模块间接调用硬件设备。
"""

import sys
from pathlib import Path

# 将项目根目录加入 sys.path，确保 instrument.fullpath 导入可用
_instr_parent = str(Path(__file__).parent.parent)
if _instr_parent not in sys.path:
    sys.path.insert(0, _instr_parent)

_swd = None  # 惰性导入，在 swd_init() 中初始化
# ---------- 万用表 ----------
# 可选驱动： Keysight_34465A / RIGOL_DM3068 （切换时修改下面 import 行即可）
from instrument.multimeter import Keysight_34465A as _dmm

# ---------- 示波器 ----------
# 可选驱动：DSOX_3024G / TBS_2204B （切换时修改下面 import 行即可）
from instrument.oscilloscope_api import DSOX_3024G as _osc

from instrument.platform import serial_comm as _serial

# ---------- 信号发生器 ----------
# 驱动：RIGOL DG1022Z
from instrument.Signal_Generator import RIGOL_DG1022Z as _sig_gen

# ---------- 源表 (SourceMeter) ----------
# 驱动：KEITHLEY 2460
from instrument.source_meter import KEITHLEY_2460 as _smu

# =====================================================================
# SWD 寄存器操作
# =====================================================================

def swd_init():
    """
    初始化 SWD 连接。

    关闭已有 JLink 连接并重新连接 SWD，同时初始化各扇区对象。
    在调用其他 SWD 函数前必须先调用此函数。
    注意：芯片专属解密流程需由芯片层 chip.py 调用。

    返回：
        bool - 初始化成功返回 True，失败返回 False
    """
    global _swd
    try:
        from instrument.debug import SWD_CMD as _swd_mod
        _swd = _swd_mod
        _swd.init_all()
        print("✅ SWD 初始化成功")
        return True
    except Exception as e:
        print(f"❌ SWD 初始化失败：{e}")
        return False


def swd_close():
    """
    关闭 SWD (JLink) 连接，释放 JLink  probe。

    在需要调用外部 JLink.exe（如 jflash_auto）前调用，避免 probe 被占用冲突。
    之后可调用 swd_init() 重新连接。
    """
    try:
        _swd.swd_handle.jlink_handle.close()
        print("  SWD 连接已关闭")
    except Exception as e:
        print(f"  ⚠️  SWD 关闭异常：{e}")


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


def serial_receive(size=256):
    """
    从串口接收数据。

    参数：
        size: int - 要读取的最大字节数（默认 256）
    返回：
        bytes - 接收到的数据
    """
    return _serial.serial_receive(size)


def serial_send_and_receive(data, response_size=256, delay=0.01):
    """
    发送数据并等待响应（组合操作）。

    参数：
        data: str/bytes - 要发送的数据
        response_size: int - 预期响应大小（默认 256）
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
# 万用表
# =====================================================================

def dmm_init():
    """
    初始化万用表。

    返回：
        bool - 初始化成功返回 True,失败返回 False
    """
    return _dmm.init_dmm()


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
        list - 电压测量值列表（单位 mV），失败返回空列表
    """
    return _dmm.read_voltage(
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


def dmm_measure_resistance(resistance_type="RESistance", count=3, range_val="AUTO", resolution="DEF"):
    """
    测量电阻（多采样点）。

    参数：
        resistance_type: str - "RESistance"（2线）或 "FRESistance"（4线）
        count: int - 采样数
        range_val: str - 量程（"AUTO" / "100" / "1K" / "10K" / "100K" / "1M" / "10M" / "100M" / "1G"）
        resolution: str - 分辨率（默认 "DEF"）
    返回：
        list - 电阻测量值列表（单位 Ω），失败返回空列表
    """
    return _dmm.read_resistance(
        resistance_type=resistance_type,
        count=count,
        range_val=range_val,
        resolution=resolution,
    )


def dmm_measure_single_resistance(resistance_type="RESistance", range_val="AUTO", resolution="DEF"):
    """
    单次测量电阻，返回值为 Ω。

    参数：
        resistance_type: str - "RESistance"（2线）或 "FRESistance"（4线）
        range_val: str - 量程（"AUTO" / "100" / "1K" / "10K" / ...）
        resolution: str - 分辨率（默认 "DEF"）
    返回：
        float/None - 电阻值（Ω），失败返回 None
    """
    return _dmm.read_single_resistance(
        resistance_type=resistance_type,
        range_val=range_val,
        resolution=resolution,
    )


def dmm_release():
    """
    释放万用表资源（关闭连接）。

    返回：
        bool - 释放成功返回 True，失败返回 False
    """
    return _dmm.release_dmm()


# =====================================================================
# 示波器
# =====================================================================

def osc_init(probe_attenuation=10, vertical_scale=5.0, horizontal_scale=1e-6):
    """
    初始化示波器。参数：
        probe_attenuation: int/float - 探头衰减比（1X=1, 10X=10）
        vertical_scale: float - 垂直量程（V/格）
        horizontal_scale: float - 水平时基（s/格）
    返回：
        bool - 初始化成功返回 True，失败返回 False
    """
    return _osc.init_oscilloscope(
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
# 信号发生器 (RIGOL DG1022Z)
# =====================================================================

def sig_init():
    """
    初始化信号发生器。

    自动搜索 USB 连接的 DG1022Z 设备。若系统中存在多个 VISA 设备，
    将自动匹配第一个 DG1022 系列设备。

    返回：
        bool - 初始化成功返回 True，失败返回 False
    """
    return _sig_gen.init_signal_generator()


def sig_close():
    """
    关闭信号发生器连接，释放 VISA 资源。

    程序结束前应调用此函数释放 USB 连接。
    """
    _sig_gen.close_signal_generator()


def sig_set_pwm(frequency=1000, duty_cycle=50, amplitude=3.3, offset=1.65, channel=1):
    """
    一键配置方波 (PWM) 输出。

    参数：
        frequency: float - 频率（Hz，默认 1000）
        duty_cycle: float - 占空比（%，0~100，默认 50）
        amplitude: float - 幅度 Vpp（默认 3.3）
        offset: float - 直流偏置 V（默认 1.65，即 3.3Vpp 的中点）
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        bool - 配置成功返回 True
    """
    return _sig_gen.set_pwm(
        frequency=frequency,
        duty_cycle=duty_cycle,
        amplitude=amplitude,
        offset=offset,
        channel=channel,
    )


def sig_set_frequency(frequency, channel=1):
    """
    设置信号发生器指定通道的输出频率。

    参数：
        frequency: float - 频率值（Hz）
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        bool - 成功返回 True
    """
    return _sig_gen.set_frequency(frequency, channel=channel)


def sig_set_duty_cycle(duty_cycle, channel=1):
    """
    设置方波占空比。

    参数：
        duty_cycle: float - 占空比百分比（0~100）
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        bool - 成功返回 True
    """
    return _sig_gen.set_duty_cycle(duty_cycle, channel=channel)


def sig_set_amplitude(amplitude, channel=1):
    """
    设置输出幅度（Vpp）。

    参数：
        amplitude: float - 峰峰值电压（V）
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        bool - 成功返回 True
    """
    return _sig_gen.set_amplitude(amplitude, channel=channel)


def sig_set_offset(offset, channel=1):
    """
    设置直流偏置。

    参数：
        offset: float - 直流偏置电压（V）
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        bool - 成功返回 True
    """
    return _sig_gen.set_offset(offset, channel=channel)


def sig_set_high_low(high, low, channel=1):
    """
    直接设置高/低电平（差分 PWM 模式）。

    参数：
        high: float - 高电平电压（V）
        low: float - 低电平电压（V）
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        bool - 成功返回 True
    """
    return _sig_gen.set_high_low_levels(high, low, channel=channel)


def sig_output_on(channel=1):
    """
    打开指定通道的输出。

    参数：
        channel: int - 通道号（1 或 2，默认 1）
    """
    return _sig_gen.output_on(channel)


def sig_output_off(channel=1):
    """
    关闭指定通道的输出。

    参数：
        channel: int - 通道号（1 或 2，默认 1）
    """
    return _sig_gen.output_off(channel)


def sig_output_both_on():
    """同时打开 CH1 和 CH2 的输出。"""
    return _sig_gen.output_both_on()


def sig_output_both_off():
    """同时关闭 CH1 和 CH2 的输出。"""
    return _sig_gen.output_both_off()


def sig_output_state(channel=1):
    """
    查询指定通道输出状态。

    参数：
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        bool - True=输出开启，False=输出关闭
    """
    return _sig_gen.output_state(channel)


def sig_set_waveform(wave_shape, channel=1):
    """
    设置波形类型。

    参数：
        wave_shape: str - "SIN" / "SQUare" / "RAMP" / "PULSe" / "NOISe" / "ARB" / "DC"
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        bool - 成功返回 True
    """
    return _sig_gen.set_waveform(wave_shape, channel)


def sig_set_ch2_follow_ch1(invert=False):
    """
    配置 CH2 跟踪 CH1（同相/反相）。

    使用手册标准 TRACK 命令：
      ON        = CH2 跟踪 CH1（同相）
      INVerted  = CH2 与 CH1 反相（180deg）

    参数：
        invert: bool - True=CH2 与 CH1 反相，False=同相（默认）
    """
    return _sig_gen.set_ch2_follow_ch1(invert=invert)


def sig_set_ch2_independent():
    """
    将 CH2 设为独立模式（停止跟踪 CH1）。
    """
    return _sig_gen.set_ch2_independent()


def sig_set_voltage_couple(state=True):
    """
    设置 CH1/CH2 电压耦合。

    参数：
        state: bool - True=开启耦合，False=关闭
    """
    return _sig_gen.set_voltage_couple(state)


def sig_apply_square(frequency=1000, amplitude=3.3, offset=0, phase=0, channel=1):
    """
    使用 APPLy 快捷命令配置方波。

    注意：APPLy 会将占空比重置为 50%，如需修改需额外调用 sig_set_duty_cycle()。

    参数：
        frequency: float - 频率（Hz，默认 1000）
        amplitude: float - 幅度 Vpp（默认 3.3）
        offset: float - 直流偏置 V（默认 0）
        phase: float - 相位（度，默认 0）
        channel: int - 通道号（1 或 2，默认 1）
    """
    return _sig_gen.apply_square(
        frequency=frequency, amplitude=amplitude,
        offset=offset, phase=phase, channel=channel,
    )


def sig_get_frequency(channel=1):
    """
    查询指定通道当前频率。

    参数：
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        float - 频率值（Hz）
    """
    return _sig_gen.get_frequency(channel)


def sig_get_duty_cycle(channel=1):
    """
    查询指定通道当前占空比。

    参数：
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        float - 占空比百分比（0~100）
    """
    return _sig_gen.get_duty_cycle(channel)


def sig_get_amplitude(channel=1):
    """
    查询指定通道当前幅度。

    参数：
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        float - 幅度（Vpp）
    """
    return _sig_gen.get_amplitude(channel)


def sig_get_offset(channel=1):
    """
    查询指定通道当前偏置。

    参数：
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        float - 偏置电压（V）
    """
    return _sig_gen.get_offset(channel)


def sig_get_high_level(channel=1):
    """
    查询指定通道当前高电平电压。

    参数：
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        float - 高电平电压（V）
    """
    return _sig_gen.get_high_level(channel)


def sig_get_low_level(channel=1):
    """
    查询指定通道当前低电平电压。

    参数：
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        float - 低电平电压（V）
    """
    return _sig_gen.get_low_level(channel)


def sig_get_params(channel=1):
    """
    一次性查询指定通道的所有关键参数。

    参数：
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        dict - 包含 waveform, frequency, duty_cycle, amplitude, offset,
               output_state 的字典；失败返回 None
    """
    return _sig_gen.get_all_params(channel)


def sig_get_idn():
    """查询信号发生器 IDN 信息。"""
    return _sig_gen.get_idn()


# =====================================================================
# 源表 (KEITHLEY 2460)
# =====================================================================

def smu_init(resource_string=None, source_function="VOLT", sense_function=None,
             source_level=0.0, compliance=1e-3, four_wire=False):
    """
    初始化 KEITHLEY 2460 源表。

    自动搜索 USB/LAN 连接的 2460，复位后配置源功能、测量功能、限值，
    最后将输出置为关闭状态（安全状态）。需显式开启输出后才会源出。

    参数：
        resource_string: str/None - VISA 资源地址，None 则自动搜索
        source_function: str - "VOLT"（源电压测电流，默认）或 "CURR"
        sense_function: str/None - 测量功能，None 自动取另一项
        source_level: float - 初始源电平（V 或 A，默认 0）
        compliance: float - 限值：源电压时为电流限值(A)，源电流时为电压限值(V)
        four_wire: bool - True=4 线开尔文，False=2 线（默认）
    返回：
        bool - 初始化成功返回 True，失败返回 False
    """
    return _smu.init_source_meter(
        resource_string=resource_string,
        source_function=source_function,
        sense_function=sense_function,
        source_level=source_level,
        compliance=compliance,
        four_wire=four_wire,
    )


def smu_close(output_off=True):
    """
    关闭源表连接，释放 VISA 资源。

    参数：
        output_off: bool - True 先关闭输出（默认，安全）；
                           False 保持输出常开，仅断开通信连接
    """
    _smu.close_source_meter(output_off=output_off)


def smu_get_idn():
    """
    查询源表 *IDN? 信息。

    返回：
        str/None - IDN 字符串，未初始化或失败返回 None
    """
    return _smu.get_idn()


def smu_output_state():
    """
    查询源表输出开关状态。

    返回：
        bool/None - True=输出开启，False=输出关闭，失败返回 None
    """
    return _smu.query_output_state()


def smu_set_current(current, voltage_limit=None):
    """
    切换源表到电流源模式，设置源电流与电压限值（不改变输出开关）。

    每条 SCPI 都单独校验；返回 False 时终端会打印具体是哪条命令被仪器拒绝。

    参数：
        current: float - 源电流（A），如 10mA -> 0.01
        voltage_limit: float/None - 电压限值（V），如 5
    返回：
        bool - 全部配置生效返回 True
    """
    return _smu.set_source_current(current, voltage_limit=voltage_limit)


def smu_source_current_on(current=0.2, voltage_limit=5.0):
    """
    一条指令完成「电流源 → 电压档 → 200 mA → 5 V 限压 → 上电」。

    只发这五步，不改其它配置。用于给某个引脚灌固定电流、同时用电压档读回电压
    （如 TIA 灌 200 mA）。源表输出会保持开启，结束前必须调用 smu_output_off()。

    参数：
        current: float - 源电流（A），默认 0.2（200 mA）
        voltage_limit: float - 电压限值（V），默认 5
    返回：
        bool - 五步全部生效返回 True
    """
    return _smu.source_current_on(current=current, voltage_limit=voltage_limit)


def smu_query_state():
    """
    回读源表当前配置（源功能 / 电平 / 限值 / 输出状态 / 连续测量）。

    返回：
        dict - 各字段字符串值；源表未初始化返回 {}
    """
    return _smu.query_source_state()


def smu_print_state(prefix="   源表配置："):
    """打印源表当前配置回读，便于核对命令是否真的生效。"""
    return _smu.print_source_state(prefix=prefix)


def smu_set_voltage(voltage, current_limit=None):
    """
    切换源表到电压源模式，设置源电压与电流限值（不改变输出开关）。

    参数：
        voltage: float - 源电压（V）
        current_limit: float/None - 电流限值（A）
    返回：
        bool - 配置成功返回 True
    """
    return _smu.set_source_voltage(voltage, current_limit=current_limit)


def smu_output_on():
    """
    打开源表输出。调用前应已用 smu_set_current() / smu_set_voltage() 配置好源与限值。
    """
    return _smu.output_on()


def smu_output_off():
    """
    关闭源表输出。
    """
    return _smu.output_off()


def smu_measure():
    """
    触发一次源表测量。

    返回：
        (voltage, current) - 电压(V)、电流(A)；失败返回 (None, None)
    """
    return _smu.measure_once()


def smu_measure_voltage():
    """
    测量电压（不改变测量功能，前面板显示保持不变）。

    返回：
        float/None - 电压(V)，失败返回 None
    """
    return _smu.measure_voltage()


def smu_display_voltage():
    """
    让源表前面板主读数显示电压（将测量功能设为电压）。
    """
    return _smu.display_voltage()


def smu_display_current():
    """
    让源表前面板主读数显示电流（将测量功能设为电流）。
    """
    return _smu.display_current()


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


def dmm_resistance_once(resistance_type="RESistance", range_val="AUTO"):
    """
    快速单次电阻测量（最简参数），返回 Ω。

    等效于 dmm_measure_single_resistance()，但参数更精简。
    """
    return dmm_measure_single_resistance(
        resistance_type=resistance_type, range_val=range_val
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
    "dmm_measure_single_current",
    "dmm_measure_resistance",
    "dmm_measure_single_resistance",
    "dmm_release",
    "dmm_voltage_once",
    "dmm_current_once",
    "dmm_resistance_once",
    # 示波器
    "osc_init",
    "osc_auto_set",
    "osc_measure_frequency",
    "osc_close",
    "osc_freq",
    # 信号发生器
    "sig_init",
    "sig_close",
    "sig_set_pwm",
    "sig_set_frequency",
    "sig_set_duty_cycle",
    "sig_set_amplitude",
    "sig_set_offset",
    "sig_set_high_low",
    "sig_output_on",
    "sig_output_off",
    "sig_output_both_on",
    "sig_output_both_off",
    "sig_output_state",
    "sig_set_waveform",
    "sig_set_ch2_follow_ch1",
    "sig_set_ch2_independent",
    "sig_set_voltage_couple",
    "sig_apply_square",
    "sig_get_frequency",
    "sig_get_duty_cycle",
    "sig_get_amplitude",
    "sig_get_offset",
    "sig_get_high_level",
    "sig_get_low_level",
    "sig_get_params",
    "sig_get_idn",
    # 源表
    "smu_init",
    "smu_close",
    "smu_get_idn",
    "smu_output_state",
    "smu_set_current",
    "smu_set_voltage",
    "smu_source_current_on",
    "smu_query_state",
    "smu_print_state",
    "smu_output_on",
    "smu_output_off",
    "smu_measure",
    "smu_measure_voltage",
    "smu_display_voltage",
    "smu_display_current",
]
