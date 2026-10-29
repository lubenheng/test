# -*- coding: utf-8 -*-
"""
RIGOL DP832A 三通道可编程直流电源 程控驱动

基于 DP800 系列编程手册编写，兼容 DP831A / DP832A / DP813A 等 DP8 系列机型。

通信方式：USB (USBTMC) / LAN (VXI-11)，基于 pyvisa / SCPI 协议。

设备能力（DP832A）：
  CH1: 0~30 V / 0~3 A
  CH2: 0~30 V / 0~3 A
  CH3: 0~5  V / 0~3 A

支持功能：
  - 三通道独立设定电压 / 电流限值
  - 三通道输出开关与状态查询
  - 电压 / 电流 / 功率回读
  - OVP（过压保护）、OCP（过流保护）设置
  - 初始化后强制输出关闭（安全状态）

手册参考命令:
  *RST; *IDN?; *OPC?
  :SOURce<channel>:VOLTage <volt>          ; 设定电压
  :SOURce<channel>:CURRent <amp>           ; 设定电流限值
  :SOURce<channel>:VOLTage?                ; 回读电压设定值
  :SOURce<channel>:CURRent?                ; 回读电流设定值
  :OUTPut CH<channel>,{ON|OFF}             ; 输出开关
  :OUTPut? CH<channel>                     ; 输出状态查询
  :MEASure:VOLTage? CH<channel>            ; 实测电压
  :MEASure:CURRent? CH<channel>            ; 实测电流
  :MEASure:POWEr? CH<channel>              ; 实测功率
  :SOURce<channel>:VOLTage:PROTection <v>  ; OVP 阈值
  :SOURce<channel>:VOLTage:PROTection:STATe {ON|OFF}
  :SOURce<channel>:CURRent:PROTection <a>  ; OCP 阈值
  :SOURce<channel>:CURRent:PROTection:STATe {ON|OFF}
  :SYSTem:ERRor?                           ; 错误队列

不同固件对个别命令的写法略有差异（如输出开关、测量查询），
本驱动对这类命令按候选列表依次尝试，并以错误队列确认是否真的被接受。
"""

import sys
import time
from pathlib import Path

import pyvisa

# 将项目根目录加入 sys.path，确保独立运行时能找到 instrument 包
_project_root = str(Path(__file__).parents[2])
if _project_root not in sys.path:
    sys.path.insert(0, _project_root)

from instrument.visa_backend import find_resource, get_resource_manager

# 全局变量：保存电源连接实例与 ResourceManager，避免重复初始化
psu_inst = None
rm = None

# 本会话中由本驱动打开的通道（关闭连接时只关这些，不去动操作员原本已使能的通道）
_enabled_by_us = set()

# 初始化时记录的通道原始状态（仅用于打印提示，不用于自动恢复）
_initial_state = {}

# *IDN? 匹配关键字：DP800 系列型号均以 DP8 开头（DP831A/DP832A/DP813A...）
_IDN_KEYWORDS = ("DP8",)

# 各通道能力上限（DP832A）。用于参数校验，避免下发超量程指令被仪器拒绝或触发保护。
CHANNEL_LIMITS = {
    1: {"voltage": 30.0, "current": 3.0},
    2: {"voltage": 30.0, "current": 3.0},
    3: {"voltage": 5.0, "current": 3.0},
}

_OUTPUT_ON = ("ON", "1", "TRUE")


# ══════════════════════════════════════════════
# 内部辅助函数
# ══════════════════════════════════════════════

def _require_instance():
    """电源未初始化时抛异常（供需要返回数值的函数使用）。"""
    if not psu_inst:
        raise RuntimeError("电源未初始化，请先调用 init_power_supply()")


def _normalize_channel(channel):
    """把通道号统一成 int（支持 1 / "1" / "CH1" / "ch1"），并校验 1~3。"""
    if isinstance(channel, str):
        s = channel.strip().upper()
        if s.startswith("CH"):
            s = s[2:]
        try:
            channel = int(s)
        except ValueError:
            raise ValueError(f"无效的通道号：{channel}（可选 1/2/3 或 CH1/CH2/CH3）")
    channel = int(channel)
    if channel not in CHANNEL_LIMITS:
        raise ValueError(f"无效的通道号：{channel}（DP832A 只有 CH1/CH2/CH3）")
    return channel


def _check_range(channel, voltage=None, current=None):
    """校验电压/电流是否在该通道允许范围内，超范围直接报错，不发指令。"""
    limits = CHANNEL_LIMITS[channel]
    if voltage is not None and not (0.0 <= voltage <= limits["voltage"]):
        raise ValueError(f"CH{channel} 电压超出范围（0~{limits['voltage']:g} V）：{voltage}")
    if current is not None and not (0.0 <= current <= limits["current"]):
        raise ValueError(f"CH{channel} 电流超出范围（0~{limits['current']:g} A）：{current}")


def _drain_errors(max_count=10):
    """读取并打印 :SYSTem:ERRor? 队列，直到 0,"No error"。

    返回：
        list[str] - 所有非零错误信息；无错误返回空列表
    """
    errors = []
    for _ in range(max_count):
        try:
            err = psu_inst.query(":SYSTem:ERRor?").strip()
        except Exception as e:
            errors.append(f"查询错误队列失败：{e}")
            break
        # 标准返回格式：<code>,"<message>"
        code = err.split(",", 1)[0].strip()
        if code == "0":
            break
        errors.append(err)
    return errors


def _write_checked(cmd, context):
    """发送一条 SCPI 并立即检查错误队列，确认仪器真的接受了这条命令。

    返回：
        bool - True=命令被接受；False=发送异常或被仪器报错
    """
    try:
        psu_inst.write(cmd)
        psu_inst.query("*OPC?")
    except Exception as e:
        print(f"❌ [{context}] 发送 `{cmd}` 失败：{e}")
        return False

    errors = _drain_errors()
    if errors:
        print(f"❌ [{context}] `{cmd}` 被仪器拒绝：")
        for e in errors:
            print(f"      {e}")
        return False
    return True


def _write_first_ok(candidates, context, quiet=False):
    """依次尝试候选命令，返回第一条被仪器接受的命令；全部被拒返回 None。

    用于不同固件写法有差异的命令（如输出开关 :OUTPut CH1,ON /
    :OUTPut:STATe ON,CH1），避免一处写法不对导致整段配置失效。
    """
    for cmd in candidates:
        try:
            psu_inst.write(cmd)
            psu_inst.query("*OPC?")
        except Exception:
            continue
        if not _drain_errors():
            if not quiet and cmd != candidates[0]:
                print(f"   {context}：`{cmd}`（回退写法）")
            return cmd
    return None


def _query_first_ok(candidates):
    """依次尝试候选查询命令，返回第一条成功返回的字符串；全部失败返回 None。

    查询失败（如命令不支持）产生的错误会在最后统一清掉。
    """
    for cmd in candidates:
        try:
            value = psu_inst.query(cmd).strip()
        except Exception:
            continue
        if value:
            _drain_errors()      # 清掉查询本身可能产生的错误
            return value
    _drain_errors()
    return None


def _query_float(candidates):
    """候选查询里取第一个能转成 float 的值，失败返回 None。"""
    value = _query_first_ok(candidates)
    if value is None:
        return None
    try:
        return float(value)
    except ValueError:
        return None


def _query_output_state(channel):
    """查询通道输出开关状态（不是设定值）。"""
    value = _query_first_ok([f":OUTPut? CH{channel}",
                             f":OUTPut:STATe? CH{channel}"])
    if value is None:
        return None
    return value.upper().strip('"') in _OUTPUT_ON


def _manipulate_output(channel, on):
    """下发输出开关命令并回读确认，同时记录本会话改变过状态的通道。"""
    state = "ON" if on else "OFF"
    cmd = _write_first_ok([f":OUTPut CH{channel},{state}",
                           f":OUTPut:STATe {state},CH{channel}",
                           f":SOURce{channel}:OUTPut {state}"],
                          f"CH{channel} 输出{state}")
    if cmd is None:
        print(f"   ❌ CH{channel} 输出{state}失败：仪器不接受任何已知写法")
        return False
    actual = _query_output_state(channel)
    if actual is not None and actual != on:
        print(f"   ❌ CH{channel} 输出{state}后回读为 {'ON' if actual else 'OFF'}")
        return False
    if on:
        _enabled_by_us.add(channel)
    else:
        _enabled_by_us.discard(channel)
    return True


# ══════════════════════════════════════════════
# 初始化 / 关闭
# ══════════════════════════════════════════════

def _match_rigol_dp800(rm, resources):
    """遍历 VISA 资源，打开并查询 *IDN?，匹配 RIGOL DP800 系列电源。

    不能仅凭资源字符串判断，必须实际能通信并读到 IDN。
    """
    for res in resources:
        try:
            temp_inst = rm.open_resource(res)
            temp_inst.timeout = 2000
            temp_inst.read_termination = '\n'
            temp_inst.write_termination = '\n'
            idn = temp_inst.query("*IDN?").strip()
            temp_inst.close()
        except Exception:
            continue

        idn_upper = idn.upper()
        if any(kw in idn_upper for kw in _IDN_KEYWORDS):
            print(f"找到电源：{idn} @ {res}")
            return res
    return None


def init_power_supply(resource_string=None, reset=False, force_output_off=False):
    """
    初始化 RIGOL DP832A 电源：仅建立通信连接，**不改变仪器当前状态**。

    默认行为（reset=False, force_output_off=False）：
      - 不发送 *RST，电压/电流/保护设置全部保持原样；
      - 不改变任何通道的输出开关，操作员已使能的通道继续正常输出；
      - 只做只读查询（*IDN? / 三通道状态），把当前状态打印出来供核对。

    需要的话可以显式要求改动：
        init_power_supply(reset=True)              # 复位到出厂设置（会关输出）
        init_power_supply(force_output_off=True)   # 强行关闭三通道输出

    参数：
        resource_string: str/None - VISA 资源地址，None 则自动搜索 DP8 系列。
                         例："USB0::0x1AB1::0x0E11::DP8C123456789::INSTR"
        reset: bool - True 发送 *RST 复位（会关闭输出、参数回默认）。
                      False（默认）保持仪器当前设置
        force_output_off: bool - True 初始化时强行关闭三通道输出；
                                 False（默认）保持各通道现有开关状态
    返回：
        bool - 初始化成功返回 True，失败返回 False
    """
    global psu_inst, rm
    try:
        _enabled_by_us.clear()
        _initial_state.clear()

        # 1. 查找设备：默认后端(NI-VISA 等) → '@py' 纯 Python 后端
        print("正在搜索 RIGOL DP800 系列电源...")
        if resource_string is None:
            rm, address = find_resource(_match_rigol_dp800)
        else:
            rm = get_resource_manager()
            address = resource_string

        if not address:
            print("❌ 未找到 RIGOL DP800 系列电源！请检查 USB/LAN 连接。")
            return False

        # 2. 打开连接
        psu_inst = rm.open_resource(address)
        psu_inst.timeout = 10000
        psu_inst.read_termination = '\n'
        psu_inst.write_termination = '\n'

        # 3. 读身份；仅当显式要求时才复位（*RST 会清掉电压/电流/保护设置并关输出）
        if reset:
            psu_inst.write("*RST")
        psu_inst.query("*OPC?")
        idn = psu_inst.query("*IDN?").strip()

        # 清空上一次会话遗留的错误（*RST 不清错误队列），避免误报本次配置错误
        _drain_errors()

        if not any(kw in idn.upper() for kw in _IDN_KEYWORDS):
            print(f"⚠️  设备型号可能不是 DP8 系列：{idn}")
        print(f"✅ 电源初始化成功：{idn}")

        # 4. 记录当前状态（只读），不主动改动
        for ch in CHANNEL_LIMITS:
            _initial_state[ch] = _query_output_state(ch)

        # 5. 仅在显式要求时改变输出状态
        if force_output_off:
            off_ok = True
            for ch in CHANNEL_LIMITS:
                off_ok &= _manipulate_output(ch, False)
            if not off_ok:
                print("   ❌ 无法关闭全部通道，请手动确认面板 OUTPUT 已灭")
            _enabled_by_us.clear()      # 关输出是本次会话主动做的，不再视为“操作员的通道”

        errors = _drain_errors()
        if errors:
            print("⚠️  电源报告以下错误：")
            for e in errors:
                print(f"   {e}")

        on_channels = [ch for ch, st in _initial_state.items() if st]
        if on_channels:
            print(f"ℹ️  当前已使能的通道：{'/'.join(f'CH{ch}' for ch in on_channels)}"
                  "（保持原状态，未做改动）")
        else:
            print("ℹ️  当前三通道输出均为关闭（保持原状态，未做改动）")
        print_state()

        return True

    except Exception as e:
        print(f"❌ 电源初始化异常：{str(e)}")
        close_power_supply(quiet=True)
        return False


def close_power_supply(output_off=True, quiet=False):
    """
    关闭电源连接，释放 VISA 资源。

    默认只关闭**本会话中由本驱动打开的通道**，操作员原本已使能的通道保持不动，
    避免程序退出时把别人正在用的电断掉。

    参数：
        output_off: bool - True 关闭本会话由本驱动开启的通道（默认）；
                           False 什么都不断，仅断开通信连接
        quiet: bool - True 不打印过程信息（初始化失败清理时用）
    """
    global psu_inst, rm
    closed = []
    if output_off and psu_inst:
        for ch in sorted(_enabled_by_us):
            try:
                if _write_first_ok([f":OUTPut CH{ch},OFF",
                                    f":OUTPut:STATe OFF,CH{ch}",
                                    f":SOURce{ch}:OUTPut OFF"], "关闭输出", quiet=True):
                    closed.append(ch)
            except Exception:
                pass
    _enabled_by_us.clear()
    try:
        if psu_inst:
            psu_inst.close()
    except Exception:
        pass
    # 不关闭 rm：ResourceManager 是每个 VISA 库共享的单例，关掉会把万用表/示波器/
    # 源表的会话一起终结（Invalid session handle）。
    psu_inst = None
    rm = None
    if not quiet:
        if closed:
            print(f"电源连接已关闭（已关闭本会话开启的通道："
                  f"{'/'.join(f'CH{ch}' for ch in closed)}）")
        else:
            print("电源连接已关闭（未改动任何通道输出）")


def get_idn():
    """
    查询电源 *IDN? 信息。

    返回：
        str/None - IDN 字符串，未初始化或查询失败返回 None
    """
    if not psu_inst:
        print("❌ 电源未初始化，请先调用 init_power_supply()")
        return None
    try:
        return psu_inst.query("*IDN?").strip()
    except Exception as e:
        print(f"❌ 查询 IDN 失败：{str(e)}")
        return None


# ══════════════════════════════════════════════
# 电压 / 电流设定
# ══════════════════════════════════════════════

def set_voltage(channel, voltage):
    """
    设置指定通道的输出电压（不改变输出开关）。

    参数：
        channel: int/str - 通道号（1/2/3 或 "CH1"）
        voltage: float - 电压值（V），需在该通道量程内
    返回：
        bool - 设置成功返回 True
    """
    ch = _normalize_channel(channel)
    _check_range(ch, voltage=voltage)
    _require_instance()

    if _write_checked(f":SOURce{ch}:VOLTage {voltage}", f"CH{ch} 电压"):
        return True
    # 回退写法：先选通道，再用不带 SOURce 前缀的命令
    if _write_checked(f":INSTrument CH{ch}", f"CH{ch} 通道选择") and \
            _write_checked(f":VOLTage {voltage}", f"CH{ch} 电压"):
        print(f"   CH{ch} 电压：`:VOLTage {voltage}`（回退写法）")
        return True
    print(f"   ❌ CH{ch} 电压设置失败")
    return False


def set_current(channel, current):
    """
    设置指定通道的电流限值（CC 模式下的限流点，不改变输出开关）。

    参数：
        channel: int/str - 通道号（1/2/3 或 "CH1"）
        current: float - 电流值（A），需在该通道量程内
    返回：
        bool - 设置成功返回 True
    """
    ch = _normalize_channel(channel)
    _check_range(ch, current=current)
    _require_instance()

    if _write_checked(f":SOURce{ch}:CURRent {current}", f"CH{ch} 电流"):
        return True
    if _write_checked(f":INSTrument CH{ch}", f"CH{ch} 通道选择") and \
            _write_checked(f":CURRent {current}", f"CH{ch} 电流"):
        print(f"   CH{ch} 电流：`:CURRent {current}`（回退写法）")
        return True
    print(f"   ❌ CH{ch} 电流设置失败")
    return False


def set_channel(channel, voltage=None, current=None):
    """
    一次性设置指定通道的电压与电流限值（不改变输出开关）。

    参数：
        channel: int/str - 通道号（1/2/3 或 "CH1"）
        voltage: float/None - 电压（V），None 表示保持当前设置
        current: float/None - 电流限值（A），None 表示保持当前设置
    返回：
        bool - 全部设置成功返回 True
    """
    ch = _normalize_channel(channel)
    _check_range(ch, voltage=voltage, current=current)
    ok = True
    if voltage is not None:
        ok &= set_voltage(ch, voltage)
    if current is not None:
        ok &= set_current(ch, current)
    return ok


def apply(channel, voltage, current, enable_output=True):
    """
    一键配置：设电压 → 设电流限值 → 打开输出。

    参数：
        channel: int/str - 通道号（1/2/3）
        voltage: float - 电压（V）
        current: float - 电流限值（A）
        enable_output: bool - True 配置后打开输出（默认）
    返回：
        bool - 全部成功返回 True
    """
    ch = _normalize_channel(channel)
    if not set_channel(ch, voltage=voltage, current=current):
        return False
    if enable_output:
        return output_on(ch)
    return True


def set_ovp(channel, voltage=None, enable=True):
    """
    设置指定通道的过压保护（OVP）。

    参数：
        channel: int/str - 通道号（1/2/3）
        voltage: float/None - OVP 阈值（V），None 表示保持当前阈值
        enable: bool - True 打开 OVP（默认）
    返回：
        bool - 全部设置成功返回 True
    """
    ch = _normalize_channel(channel)
    ok = True
    if voltage is not None:
        if voltage > CHANNEL_LIMITS[ch]["voltage"] * 1.1:
            raise ValueError(
                f"CH{ch} OVP 阈值过高：{voltage}（应不超过 "
                f"{CHANNEL_LIMITS[ch]['voltage'] * 1.1:g} V）")
        ok &= _write_checked(f":SOURce{ch}:VOLTage:PROTection {voltage}",
                             f"CH{ch} OVP 阈值")
    state = "ON" if enable else "OFF"
    ok &= _write_checked(f":SOURce{ch}:VOLTage:PROTection:STATe {state}",
                         f"CH{ch} OVP 开关")
    return ok


def set_ocp(channel, current=None, enable=True):
    """
    设置指定通道的过流保护（OCP）。

    参数：
        channel: int/str - 通道号（1/2/3）
        current: float/None - OCP 阈值（A），None 表示保持当前阈值
        enable: bool - True 打开 OCP（默认）
    返回：
        bool - 全部设置成功返回 True
    """
    ch = _normalize_channel(channel)
    ok = True
    if current is not None:
        if current > CHANNEL_LIMITS[ch]["current"] * 1.1:
            raise ValueError(
                f"CH{ch} OCP 阈值过高：{current}（应不超过 "
                f"{CHANNEL_LIMITS[ch]['current'] * 1.1:g} A）")
        ok &= _write_checked(f":SOURce{ch}:CURRent:PROTection {current}",
                             f"CH{ch} OCP 阈值")
    state = "ON" if enable else "OFF"
    ok &= _write_checked(f":SOURce{ch}:CURRent:PROTection:STATe {state}",
                         f"CH{ch} OCP 开关")
    return ok


# ══════════════════════════════════════════════
# 输出控制
# ══════════════════════════════════════════════

def output_on(channel):
    """
    打开指定通道输出。

    注意：命令回读为 ON 只代表输出已被使能，端口电压还需约 1~2 s 才建立，
          测量实际电压前请等待足够时间（建议 ≥2 s）。

    参数：
        channel: int/str - 通道号（1/2/3）
    返回：
        bool - 命令被接受且回读为 ON 时返回 True
    """
    _require_instance()
    return _manipulate_output(_normalize_channel(channel), True)


def output_off(channel):
    """
    关闭指定通道输出。

    参数：
        channel: int/str - 通道号（1/2/3）
    返回：
        bool - 命令被接受且回读为 OFF 时返回 True
    """
    _require_instance()
    return _manipulate_output(_normalize_channel(channel), False)


def set_output(channel, on):
    """
    设置指定通道输出开关。

    参数：
        channel: int/str - 通道号（1/2/3）
        on: bool - True=开，False=关
    返回：
        bool - 成功返回 True
    """
    return output_on(channel) if on else output_off(channel)


def output_all_on():
    """依次打开 CH1 / CH2 / CH3 输出。"""
    _require_instance()
    return all([output_on(ch) for ch in CHANNEL_LIMITS])


def output_all_off():
    """依次关闭 CH1 / CH2 / CH3 输出。"""
    _require_instance()
    return all([output_off(ch) for ch in CHANNEL_LIMITS])


def output_state(channel):
    """
    查询指定通道输出开关状态。

    参数：
        channel: int/str - 通道号（1/2/3）
    返回：
        bool/None - True=输出开启，False=输出关闭，查询失败返回 None
    """
    _require_instance()
    return _query_output_state(_normalize_channel(channel))


# ══════════════════════════════════════════════
# 测量 / 回读
# ══════════════════════════════════════════════

def measure_voltage(channel):
    """
    测量指定通道的实际输出电压。

    参数：
        channel: int/str - 通道号（1/2/3）
    返回：
        float/None - 电压（V），失败返回 None
    """
    ch = _normalize_channel(channel)
    _require_instance()
    return _query_float([f":MEASure:VOLTage? CH{ch}",
                         f":MEASure:VOLTage:DC? CH{ch}"])


def measure_current(channel):
    """
    测量指定通道的实际输出电流。

    参数：
        channel: int/str - 通道号（1/2/3）
    返回：
        float/None - 电流（A），失败返回 None
    """
    ch = _normalize_channel(channel)
    _require_instance()
    return _query_float([f":MEASure:CURRent? CH{ch}",
                         f":MEASure:CURRent:DC? CH{ch}"])


def measure_power(channel):
    """
    测量指定通道的实际输出功率。

    参数：
        channel: int/str - 通道号（1/2/3）
    返回：
        float/None - 功率（W），失败返回 None
    """
    ch = _normalize_channel(channel)
    _require_instance()
    return _query_float([f":MEASure:POWEr? CH{ch}"])


def measure_all(channel):
    """
    一次性测量指定通道的电压 / 电流 / 功率。

    参数：
        channel: int/str - 通道号（1/2/3）
    返回：
        tuple - (voltage V, current A, power W)，失败项为 None
    """
    ch = _normalize_channel(channel)
    return measure_voltage(ch), measure_current(ch), measure_power(ch)


def get_voltage(channel):
    """
    回读指定通道的电压设定值（不是实测值）。

    参数：
        channel: int/str - 通道号（1/2/3）
    返回：
        float/None - 设定电压（V）
    """
    ch = _normalize_channel(channel)
    _require_instance()
    return _query_float([f":SOURce{ch}:VOLTage?"])


def get_current(channel):
    """
    回读指定通道的电流设定值（不是实测值）。

    参数：
        channel: int/str - 通道号（1/2/3）
    返回：
        float/None - 设定电流（A）
    """
    ch = _normalize_channel(channel)
    _require_instance()
    return _query_float([f":SOURce{ch}:CURRent?"])


def query_channel_state(channel):
    """
    回读指定通道的设定值 / 实测值 / 输出状态。

    参数：
        channel: int/str - 通道号（1/2/3）
    返回：
        dict - {channel, voltage_set, current_set, voltage, current,
                power, output}；查询失败的字段为 None
    """
    ch = _normalize_channel(channel)
    if not psu_inst:
        return {}
    return {
        "channel": ch,
        "voltage_set": get_voltage(ch),
        "current_set": get_current(ch),
        "voltage": measure_voltage(ch),
        "current": measure_current(ch),
        "power": measure_power(ch),
        "output": _query_output_state(ch),
    }


def query_all_channels():
    """
    回读三个通道的状态。

    返回：
        dict - {1: state_dict, 2: state_dict, 3: state_dict}；未初始化返回 {}
    """
    if not psu_inst:
        return {}
    return {ch: query_channel_state(ch) for ch in CHANNEL_LIMITS}


def print_state(prefix="   电源状态："):
    """打印三通道当前配置与回读值，便于核对命令是否真的生效。"""
    states = query_all_channels()
    if not states:
        print(f"{prefix}电源未初始化")
        return

    def _fmt(value, digits, unit):
        return "—" if value is None else f"{value:.{digits}f} {unit}"

    for ch, s in states.items():
        out = s.get("output")
        out_str = "ON " if out else ("OFF" if out is False else "?  ")
        print(f"{prefix}CH{ch} [{out_str}] "
              f"设定 {_fmt(s.get('voltage_set'), 3, 'V')} / "
              f"{_fmt(s.get('current_set'), 3, 'A')}，"
              f"实测 {_fmt(s.get('voltage'), 3, 'V')} / "
              f"{_fmt(s.get('current'), 3, 'A')} / "
              f"{_fmt(s.get('power'), 3, 'W')}")


# ══════════════════════════════════════════════
# 主函数 / 演示
# ══════════════════════════════════════════════

def main():
    """演示：三通道输出不同电压，保持 3 秒后关闭。"""
    if not init_power_supply():
        return

    demo = {1: (1.2, 0.5), 2: (3.3, 0.5), 3: (5.0, 0.5)}
    try:
        print("\n--- 配置三通道不同电压 ---")
        for ch, (volt, curr) in demo.items():
            set_channel(ch, voltage=volt, current=curr)
        output_all_on()

        # DP832A 收到 OUTP ON 后端口电压约需 1~2 s 才建立，测量前须等待
        time.sleep(2.0)
        print_state("\n   实测：")
        time.sleep(3.0)
        print("\n演示完成，关闭全部输出...")
    except Exception as e:
        print(f"演示异常：{str(e)}")
    finally:
        close_power_supply()


if __name__ == "__main__":
    main()
