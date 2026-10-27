# -*- coding: utf-8 -*-
"""
RIGOL DG1022Z 双通道函数/任意波形发生器 程控驱动

基于 DG1000Z 编程手册（PGB09006-1110）编写，兼容 DG1022Z/DG1032Z/DG1062Z。

通信方式：USB (USBTMC)，基于 pyvisa / SCPI 协议

支持功能：
  - 输出方波 (PWM)：频率、占空比、高/低电平、幅度、偏置
  - 双通道独立控制（CH1 / CH2）
  - 输出开关控制
  - 通道跟踪（CH2 跟踪 CH1，支持同相/反相）
  - 负载阻抗设置
  - 输出状态/参数查询

手册参考命令:
  :SOURce[<n>]:APPLy:SQUare [<freq>][,<amp>[,<offset>[,<phase>]]]
  :SOURce[<n>]:FUNCtion[:SHAPe] {SIN|SQU|RAMP|PULSE|NOISE|DC|USER}
  :SOURce[<n>]:FUNCtion:SQUare:DCYCle <percent>
  :SOURce[<n>]:FREQuency[:FIXed] <freq>
  :SOURce[<n>]:VOLTage[:LEVel][:IMMediate][:AMPLitude] <amp>
  :SOURce[<n>]:VOLTage[:LEVel][:IMMediate]:OFFSet <offset>
  :SOURce[<n>]:VOLTage[:LEVel][:IMMediate]:HIGH <voltage>
  :SOURce[<n>]:VOLTage[:LEVel][:IMMediate]:LOW  <voltage>
  :OUTPut[<n>][:STATe] {ON|1|OFF|0}
  :OUTPut[<n>]:LOAD {<ohms>|INFinity|MINimum|MAXimum}
  :SOURce[<n>]:TRACK {ON|OFF|INVerted}
"""

import sys
import time
from pathlib import Path

import pyvisa

# 将项目根目录加入 sys.path，确保独立运行时能找到 instrument 包
_project_root = str(Path(__file__).parents[2])
if _project_root not in sys.path:
    sys.path.insert(0, _project_root)

from instrument.platform.serial_comm import SerialComm as _SerialComm
from instrument.platform import serial_comm as _serial
from instrument.visa_backend import find_resource, get_resource_manager

# 全局变量：信号发生器实例
_sig_gen_instance = None
_sig_gen_rm = None

# ══════════════════════════════════════════════
# 内部辅助函数
# ══════════════════════════════════════════════

def _check_instance():
    """检查实例是否已初始化"""
    if _sig_gen_instance is None:
        raise RuntimeError("信号发生器未初始化，请先调用 init_signal_generator()")


def _scpi_cmd(command, channel=None):
    """
    发送 SCPI 写命令（无返回值）。

    若指定 channel（1/2），自动拼接 ":SOURce{channel}:" 前缀。
    例如: _scpi_cmd("FREQuency 1000", channel=1) -> ":SOURce1:FREQuency 1000"
    """
    _check_instance()
    if channel is not None:
        _sig_gen_instance.write(f":SOURce{channel}:{command}")
    else:
        _sig_gen_instance.write(command)


def _scpi_query(query, channel=None):
    """
    发送 SCPI 查询命令并返回去除空白后的字符串。

    若指定 channel（1/2），自动拼接 ":SOURce{channel}:" 前缀。
    """
    _check_instance()
    if channel is not None:
        return _sig_gen_instance.query(f":SOURce{channel}:{query}").strip()
    else:
        return _sig_gen_instance.query(query).strip()


# ══════════════════════════════════════════════
# 初始化 / 关闭
# ══════════════════════════════════════════════

def _match_dg1000z(rm, resources):
    """通过 *IDN? 查询匹配 DG1000Z 系列设备（DG1022Z/DG1032Z/DG1062Z）"""
    for res in resources:
        try:
            temp_inst = rm.open_resource(res)
            temp_inst.timeout = 2000
            idn = temp_inst.query("*IDN?").strip()
            temp_inst.close()
            if any(kw in idn for kw in ["DG1022", "DG1032", "DG1062", "DG10"]):
                print(f"  找到信号发生器：{idn} @ {res}")
                return res
        except Exception:
            continue
    return None


def init_signal_generator(resource_string=None):
    """
    初始化 DG1022Z 信号发生器。

    参数：
        resource_string: str/None - VISA 资源地址（如 "USB0::0x1AB1::0x0642::DG1A123456789::INSTR"），
                         None 则自动搜索第一个匹配的 DG1022 系列设备。
    返回：
        bool - 初始化成功返回 True，失败返回 False
    """
    global _sig_gen_instance, _sig_gen_rm
    try:
        address = resource_string
        if address is None:
            # 依次用默认后端(走系统已装 NI-VISA 等驱动)和 '@py' 纯 Python 后端查找设备,
            # 以"能否实际匹配到信号发生器"为准, 而不是"是否枚举到任意资源"
            _sig_gen_rm, address = find_resource(_match_dg1000z)
        else:
            _sig_gen_rm = get_resource_manager()

        if address is None:
            print("未找到 RIGOL DG1000Z 系列信号发生器！请检查 USB 连接。")
            return False

        _sig_gen_instance = _sig_gen_rm.open_resource(address)
        _sig_gen_instance.timeout = 5000
        _sig_gen_instance.write("*RST")
        time.sleep(0.5)

        idn = _sig_gen_instance.query("*IDN?").strip()
        print(f"信号发生器初始化成功：{idn}")
        return True

    except Exception as e:
        print(f"信号发生器初始化失败：{str(e)}")
        close_signal_generator()
        return False


def close_signal_generator():
    """关闭信号发生器连接，释放 VISA 资源。"""
    global _sig_gen_instance, _sig_gen_rm
    try:
        if _sig_gen_instance:
            _sig_gen_instance.close()
    except Exception:
        pass
    # 不关闭 rm：ResourceManager 是每个 VISA 库共享的单例，关掉会终结其它仪器的会话
    _sig_gen_instance = None
    _sig_gen_rm = None
    print("信号发生器连接已关闭")


# ══════════════════════════════════════════════
# 波形类型设置
# ══════════════════════════════════════════════

def set_waveform(wave_shape, channel=1):
    """
    设置指定通道的输出波形类型。

    SCPI 命令参考：[:SOURce[<n>]]:FUNCtion[:SHAPe] <name>

    参数：
        wave_shape: str - 波形类型（不区分大小写），可选：
            "SIN" - 正弦波
            "SQU" | "SQUare" - 方波
            "RAMP" - 斜波
            "PULSe" - 脉冲波
            "NOISe" - 噪声
            "DC" - 直流
            "USER" - 用户自定义
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        bool - 成功返回 True
    """
    # 统一为标准名称（手册定义）
    NORMALIZED = {
        "SIN": "SIN",
        "SQU": "SQU",
        "SQUARE": "SQU",
        "RAMP": "RAMP",
        "PULSE": "PULSE",
        "NOISE": "NOISE",
        "DC": "DC",
        "USER": "USER",
    }
    key = wave_shape.upper()
    if key not in NORMALIZED:
        valid = list(NORMALIZED.keys())
        raise ValueError(f"不支持的波形类型：{wave_shape}，可选：{valid}")

    _scpi_cmd(f"FUNCtion {NORMALIZED[key]}", channel=channel)
    return True


# ══════════════════════════════════════════════
# PWM (方波) 参数控制
# ══════════════════════════════════════════════

def set_pwm(frequency=1000, duty_cycle=50, amplitude=3.3, offset=1.65, channel=1):
    """
    一键配置方波输出 (PWM) 的所有参数。

    等效于依次调用：
      set_waveform(SQU) -> set_frequency() -> set_duty_cycle()
      -> set_amplitude() -> set_offset()

    参数：
        frequency: float - 频率（Hz，默认 1000）
        duty_cycle: float - 占空比（%，0~100，默认 50）
        amplitude: float - 幅度 Vpp（默认 3.3）
        offset: float - 直流偏置 V（默认 1.65，即 3.3Vpp 的中点）
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        bool - 配置成功返回 True
    """
    _check_instance()

    # 先设置波形，再逐个配置参数
    _scpi_cmd("FUNCtion SQU", channel=channel)
    time.sleep(0.03)
    set_frequency(frequency, channel=channel)
    set_duty_cycle(duty_cycle, channel=channel)
    set_amplitude(amplitude, channel=channel)
    set_offset(offset, channel=channel)
    return True


def set_frequency(frequency, channel=1):
    """
    设置指定通道的输出频率。

    SCPI 命令：[:SOURce[<n>]]:FREQuency[:FIXed] <freq>

    参数：
        frequency: float - 频率值（Hz，方波上限 DG1022Z=5MHz）
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        bool - 成功返回 True
    """
    _scpi_cmd(f"FREQuency {frequency}", channel=channel)
    return True


def set_duty_cycle(duty_cycle, channel=1):
    """
    设置指定通道的方波占空比。

    SCPI 命令：[:SOURce[<n>]]:FUNCtion:SQUare:DCYCle <percent>

    参数：
        duty_cycle: float - 占空比百分比（0~100）
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        bool - 成功返回 True
    """
    if not (0 <= duty_cycle <= 100):
        raise ValueError(f"占空比超出范围（0~100）：{duty_cycle}")
    _scpi_cmd(f"FUNCtion:SQUare:DCYCle {duty_cycle}", channel=channel)
    return True


def set_amplitude(amplitude, channel=1):
    """
    设置指定通道的输出幅度（Vpp）。

    SCPI 命令：[:SOURce[<n>]]:VOLTage[:LEVel][:IMMediate][:AMPLitude] <amp>

    参数：
        amplitude: float - 峰峰值电压（V，DG1022Z 最大约 10Vpp，与负载阻抗有关）
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        bool - 成功返回 True
    """
    _scpi_cmd(f"VOLTage:LEVel:IMMediate:AMPLitude {amplitude}", channel=channel)
    return True


def set_offset(offset, channel=1):
    """
    设置指定通道的直流偏置电压。

    SCPI 命令：[:SOURce[<n>]]:VOLTage[:LEVel][:IMMediate]:OFFSet <offset>

    参数：
        offset: float - 直流偏置（V，受幅度限制）
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        bool - 成功返回 True
    """
    _scpi_cmd(f"VOLTage:LEVel:IMMediate:OFFSet {offset}", channel=channel)
    return True


def set_high_level(voltage, channel=1):
    """
    直接设置指定通道的高电平电压。

    SCPI 命令：[:SOURce[<n>]]:VOLTage[:LEVel][:IMMediate]:HIGH <voltage>

    参数：
        voltage: float - 高电平电压（V）
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        bool - 成功返回 True
    """
    _scpi_cmd(f"VOLTage:LEVel:IMMediate:HIGH {voltage}", channel=channel)
    return True


def set_low_level(voltage, channel=1):
    """
    直接设置指定通道的低电平电压。

    SCPI 命令：[:SOURce[<n>]]:VOLTage[:LEVel][:IMMediate]:LOW <voltage>

    参数：
        voltage: float - 低电平电压（V）
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        bool - 成功返回 True
    """
    _scpi_cmd(f"VOLTage:LEVel:IMMediate:LOW {voltage}", channel=channel)
    return True


def set_high_low_levels(high_voltage, low_voltage, channel=1):
    """
    同时设置指定通道的高电平和低电平电压。

    SCPI 命令：先设 HIGH 再设 LOW（手册要求顺序）。

    参数：
        high_voltage: float - 高电平（V）
        low_voltage: float - 低电平（V）
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        bool - 成功返回 True
    """
    set_high_level(high_voltage, channel=channel)
    set_low_level(low_voltage, channel=channel)
    return True


def set_load_impedance(impedance, channel=1):
    """
    设置指定通道的输出负载阻抗。

    SCPI 命令：[:SOURce[<n>]]:OUTPut:LOAD|<ohms>|INFinity
    手册中 OUTPut:LOAD 和 OUTPut:IMPedance 为同义命令。

    参数：
        impedance: str/float - "HIGHZ" / "INFinity"（高阻）或 50/75/100（Ω）
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        bool - 成功返回 True
    """
    if isinstance(impedance, str) and impedance.upper() in ("HIGHZ", "INF", "INFINITY"):
        imp_str = "INFinity"
    else:
        imp_str = str(impedance)
    _sig_gen_instance.write(f":OUTPut{channel}:LOAD {imp_str}")
    return True


# ══════════════════════════════════════════════
# 输出控制
# ══════════════════════════════════════════════

def output_on(channel=1):
    """
    打开指定通道的输出。

    SCPI 命令：:OUTPut[<n>][:STATe] ON

    参数：
        channel: int - 通道号（1 或 2，默认 1）
    """
    _sig_gen_instance.write(f":OUTPut{channel}:STATe ON")
    return True


def output_off(channel=1):
    """
    关闭指定通道的输出。

    SCPI 命令：:OUTPut[<n>][:STATe] OFF

    参数：
        channel: int - 通道号（1 或 2，默认 1）
    """
    _sig_gen_instance.write(f":OUTPut{channel}:STATe OFF")
    return True


def output_state(channel=1):
    """
    查询指定通道的输出状态。

    SCPI 命令：:OUTPut[<n>][:STATe]?

    参数：
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        bool - True=输出开启，False=输出关闭
    """
    ret = _sig_gen_instance.query(f":OUTPut{channel}:STATe?").strip()
    return ret.upper() in ("ON", "1")


def output_both_on():
    """同时打开 CH1 和 CH2 的输出。"""
    output_on(1)
    output_on(2)
    return True


def output_both_off():
    """同时关闭 CH1 和 CH2 的输出。"""
    output_off(1)
    output_off(2)
    return True


# ══════════════════════════════════════════════
# 通道跟踪（CH2 同步 / 差分 PWM）
# ══════════════════════════════════════════════

def set_ch2_follow_ch1(invert=False):
    """
    配置 CH2 跟踪 CH1（使用 TRACK 命令）。

    SCPI 命令参考: [:SOURce[<n>]]:TRACK {ON|OFF|INVerted}
    手册中 TRACK 命令用于使 CH2 输出与 CH1 同步。
    ON = CH2 跟随 CH1（同相）
    OFF = CH2 独立
    INVerted = CH2 与 CH1 反相（180deg 相位差）

    参数：
        invert: bool - True=CH2 与 CH1 反相，False=同相（默认）
    """
    mode = "INVerted" if invert else "ON"
    # TRACK 命令作用于 CH2，使其跟踪 CH1
    _scpi_cmd(f"TRACK {mode}", channel=2)
    return True


def set_ch2_independent():
    """
    将 CH2 设置为独立模式（不跟踪 CH1）。

    SCPI 命令: [:SOURce[2]]:TRACK OFF
    """
    _scpi_cmd("TRACK OFF", channel=2)
    return True


# ══════════════════════════════════════════════
# 通道耦合设置
# ══════════════════════════════════════════════

def set_voltage_couple(state=True):
    """
    设置 CH1 和 CH2 的电压耦合。

    SCPI 命令: [:SOURce[<n>]]:VOLTage:COUPle[:STATe] {ON|1|OFF|0}
    耦合后调节 CH1 电压时 CH2 自动按比例变化（需先设置耦合比例）。

    参数：
        state: bool - True=开启电压耦合，False=关闭
    """
    val = "ON" if state else "OFF"
    _scpi_cmd("VOLTage:COUPle:STATe " + val, channel=1)
    return True


# ══════════════════════════════════════════════
# 查询函数
# ══════════════════════════════════════════════

def get_idn():
    """查询设备 IDN 字符串。"""
    _check_instance()
    return _sig_gen_instance.query("*IDN?").strip()


def get_frequency(channel=1):
    """
    查询指定通道的当前输出频率（Hz）。

    SCPI 命令：[:SOURce[<n>]]:FREQuency[:FIXed]?
    """
    return float(_scpi_query("FREQuency?", channel=channel))


def get_duty_cycle(channel=1):
    """
    查询指定通道的当前方波占空比（%）。

    SCPI 命令：[:SOURce[<n>]]:FUNCtion:SQUare:DCYCle?
    """
    return float(_scpi_query("FUNCtion:SQUare:DCYCle?", channel=channel))


def get_amplitude(channel=1):
    """
    查询指定通道的当前输出幅度（Vpp）。

    SCPI 命令：[:SOURce[<n>]]:VOLTage[:LEVel][:IMMediate][:AMPLitude]?
    """
    return float(_scpi_query("VOLTage:LEVel:IMMediate:AMPLitude?", channel=channel))


def get_offset(channel=1):
    """
    查询指定通道的当前直流偏置（V）。

    SCPI 命令：[:SOURce[<n>]]:VOLTage[:LEVel][:IMMediate]:OFFSet?
    """
    return float(_scpi_query("VOLTage:LEVel:IMMediate:OFFSet?", channel=channel))


def get_high_level(channel=1):
    """
    查询指定通道的当前高电平电压（V）。

    SCPI 命令：[:SOURce[<n>]]:VOLTage[:LEVel][:IMMediate]:HIGH?
    """
    return float(_scpi_query("VOLTage:LEVel:IMMediate:HIGH?", channel=channel))


def get_low_level(channel=1):
    """
    查询指定通道的当前低电平电压（V）。

    SCPI 命令：[:SOURce[<n>]]:VOLTage[:LEVel][:IMMediate]:LOW?
    """
    return float(_scpi_query("VOLTage:LEVel:IMMediate:LOW?", channel=channel))


def get_waveform(channel=1):
    """
    查询指定通道的当前波形类型。

    SCPI 命令：[:SOURce[<n>]]:FUNCtion[:SHAPe]?
    """
    return _scpi_query("FUNCtion?", channel=channel)


def get_channel_state(channel=1):
    """
    查询指定通道的输出状态字符串。

    返回：
        str - "ON" 或 "OFF"
    """
    _check_instance()
    return _sig_gen_instance.query(f":OUTPut{channel}:STATe?").strip()


def get_all_params(channel=1):
    """
    一次性查询指定通道的所有关键参数。

    参数：
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        dict - 包含 waveform, frequency, duty_cycle, amplitude, offset,
               high_level, low_level, output_state 的字典；查询失败返回 None
    """
    try:
        return {
            "channel": channel,
            "waveform": get_waveform(channel),
            "frequency": get_frequency(channel),
            "duty_cycle": get_duty_cycle(channel),
            "amplitude": get_amplitude(channel),
            "offset": get_offset(channel),
            "high_level": get_high_level(channel),
            "low_level": get_low_level(channel),
            "output_state": get_channel_state(channel),
        }
    except Exception as e:
        print(f"查询信号发生器参数失败：{str(e)}")
        return None


# ══════════════════════════════════════════════
# APPLy 快捷命令
# ══════════════════════════════════════════════

def apply_square(frequency=1000, amplitude=3.3, offset=0, phase=0, channel=1):
    """
    使用 APPLy 快捷命令一键配置方波输出。

    SCPI 命令：[:SOURce[<n>]]:APPLy:SQUare [<freq>][,<amp>[,<offset>[,<phase>]]]

    注意：APPLy 命令会将占空比重置为默认值 50%。如需修改占空比，
          请额外调用 set_duty_cycle()。

    参数：
        frequency: float - 频率（Hz，默认 1000）
        amplitude: float - 幅度 Vpp（默认 3.3）
        offset: float - 直流偏置 V（默认 0）
        phase: float - 初始相位（度，默认 0）
        channel: int - 通道号（1 或 2，默认 1）
    返回：
        bool - 成功返回 True
    """
    _check_instance()
    _sig_gen_instance.write(
        f":SOURce{channel}:APPLy:SQUare {frequency},{amplitude},{offset},{phase}"
    )
    return True


# ══════════════════════════════════════════════
# 品牌中立别名（用于 instrument_set 切换）
# ══════════════════════════════════════════════

def init_source():
    """初始化信号发生器（品牌中立名）。"""
    return init_signal_generator()


def release_source():
    """释放信号发生器资源（品牌中立名）。"""
    close_signal_generator()


# ══════════════════════════════════════════════
# 串口通信（代理到 instrument.platform.serial_comm）
# ══════════════════════════════════════════════

def serial_init(port=None, baudrate=9600, timeout=2):
    """
    初始化串口。

    参数：
        port: str/None - 串口端口（如 "COM3"），None 则自动检测
        baudrate: int - 波特率（默认 9600）
        timeout: int - 超时时间（秒，默认 2）
    返回：
        bool - 初始化成功返回 True，失败返回 False
    """
    return _serial.serial_init(port, baudrate, timeout) is not None


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
    """关闭串口连接。"""
    _serial.serial_close()


def serial_list_ports():
    """
    列出所有可用串口。

    返回：
        list - 可用串口端口名列表
    """
    return _SerialComm.list_serial_ports()


# ══════════════════════════════════════════════
# 主函数 / 演示
# ══════════════════════════════════════════════

def main():
    """演示：输出 PWM 方波 + 串口收发"""
    # ── 可选：先初始化串口 ──
    ports = serial_list_ports()
    if ports:
        print(f"检测到串口: {ports}")
        serial_init(ports[0], baudrate=115200)
    else:
        print("未检测到串口，跳过串口部分")

    # ── 信号发生器部分 ──
    if not init_signal_generator():
        serial_close()
        return

    time.sleep(1)
    try:
        print("\n--- 配置 CH1 输出 PWM 方波 ---")
        set_pwm(frequency=1000, duty_cycle=50, amplitude=3.3, offset=1.65, channel=1)

        print("打开 CH1 输出...")
        output_on(1)

        time.sleep(0.5)
        params = get_all_params(1)
        if params:
            print("\nCH1 当前参数：")
            for k, v in params.items():
                print(f"  {k}: {v}")
        time.sleep(2)
        print("\n演示完成，关闭输出...")
        output_off(1)

    except Exception as e:
        print(f"演示异常：{str(e)}")
    finally:
        close_signal_generator()
        serial_close()

if __name__ == "__main__":
    main()
