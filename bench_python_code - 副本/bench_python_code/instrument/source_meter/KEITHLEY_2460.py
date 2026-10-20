# -*- coding: utf-8 -*-
"""
KEITHLEY 2460 SourceMeter 源表驱动（初始化连接版）

通信方式：USB (USBTMC) / LAN (VXI-11 / Socket)，基于 pyvisa / SCPI 协议。

当前只实现连接与安全初始化，参考 instrument/oscilloscope_api/ 的写法：
  - init_source_meter(): 自动搜索 2460 → *RST 复位 → 配置源/测量功能 → 输出关闭
  - close_source_meter(): 先 :OUTP OFF 再释放 VISA 资源
  - get_idn(): 查询 *IDN?

后续可在此基础上补充 set_source_level / output_on / measure_once 等函数。

手册参考命令:
  *RST; *IDN?; *OPC?
  :SOURce:FUNCtion {VOLTage|CURRent}
  :SOURce:VOLTage[:LEVel][:IMMediate][:AMPLitude] <volt>
  :SOURce:VOLTage:ILIMit <amp>        (源电压时的电流限值)
  :SOURce:CURRent[:LEVel][:IMMediate][:AMPLitude] <amp>
  :SOURce:CURRent:VLIMit <volt>       (源电流时的电压限值)
  :SENSe:FUNCtion "CURRent"|"VOLTage"
  :SYSTem:RSENse {ON|OFF}             (4 线/2 线)
  :OUTPut[:STATe] {ON|OFF}
  :SYSTem:ERRor?
"""

import sys
from pathlib import Path

import pyvisa

# 将项目根目录加入 sys.path，确保独立运行时能找到 instrument 包
_project_root = str(Path(__file__).parents[2])
if _project_root not in sys.path:
    sys.path.insert(0, _project_root)

from instrument.visa_backend import find_resource, get_resource_manager

# 全局变量：保存源表连接实例与 ResourceManager，避免重复初始化
smu_inst = None
rm = None

# *IDN? 匹配关键字：仅按型号匹配，避免把总线上其它 Keithley 仪器（如 2400/DMM）误认成 2460
_IDN_KEYWORDS = ("2460", "2461")


def _match_keithley_2460(rm, resources):
    """遍历 VISA 资源，打开并查询 *IDN?，匹配 Keithley 2460 源表。

    与示波器/信号发生器一致：不能仅凭资源字符串判断，必须实际能通信。
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
            print(f"找到源表：{idn} @ {res}")
            return res
    return None


def _drain_errors(max_count=10):
    """读取并打印 :SYSTem:ERRor? 队列，直到 0,"No error"。

    返回：
        list[str] - 所有非零错误信息；无错误返回空列表
    """
    errors = []
    for _ in range(max_count):
        try:
            err = smu_inst.query(":SYSTem:ERRor?").strip()
        except Exception as e:
            errors.append(f"查询错误队列失败：{e}")
            break
        # 标准返回格式：<code>,"<message>"
        code = err.split(",", 1)[0].strip()
        if code == "0":
            break
        errors.append(err)
    return errors


def _normalize_function(func):
    """把 VOLT/VOLTAGE/V、CURR/CURRENT/I 统一为 'VOLT' 或 'CURR'。"""
    f = str(func).strip().upper()
    if f in ("VOLT", "VOLTAGE", "V"):
        return "VOLT"
    if f in ("CURR", "CURRENT", "I"):
        return "CURR"
    raise ValueError(f"无效的功能类型：{func}（可选 'VOLT' / 'CURR'）")


def init_source_meter(resource_string=None, source_function="VOLT",
                      sense_function=None, source_level=0.0,
                      compliance=1e-3, four_wire=False):
    """
    初始化 KEITHLEY 2460 源表：搜索设备 → 复位 → 配置源/测量 → 输出关闭。

    初始化后仪器处于安全状态（OUTPut OFF，输出电平 source_level），
    不会对外输出电流/电压，需显式调用输出开启函数后才会源出。

    参数：
        resource_string: str/None - VISA 资源地址，None 则自动搜索 2460。
                         例："USB0::0x05E6::0x2460::04071234::INSTR"
        source_function: str - 源功能，"VOLT"（源电压，测电流，默认）或 "CURR"
        sense_function: str/None - 测量功能，None 则自动取另一项
                                   （源电压 → 测电流；源电流 → 测电压）
        source_level: float - 初始源电平（V 或 A，默认 0）
        compliance: float - 限值：源电压时为电流限值(A)，源电流时为电压限值(V)。
                            默认 1e-3（源电压时 = 1 mA）
        four_wire: bool - True 使用 4 线开尔文测量，False 为 2 线（默认）

    返回：
        bool - 初始化成功返回 True，失败返回 False
    """
    global smu_inst, rm
    try:
        src_func = _normalize_function(source_function)
        if sense_function is None:
            sense = "CURR" if src_func == "VOLT" else "VOLT"
        else:
            sense = _normalize_function(sense_function)
        if sense == src_func:
            raise ValueError("源功能与测量功能不能相同")

        # 1. 查找设备：默认后端(NI-VISA 等) → '@py' 纯 Python 后端
        print("正在搜索 KEITHLEY 2460 源表...")
        if resource_string is None:
            rm, address = find_resource(_match_keithley_2460)
        else:
            rm = get_resource_manager()
            address = resource_string

        if not address:
            print("❌ 未找到 KEITHLEY 2460 源表！请检查 USB/LAN 连接。")
            return False

        # 2. 打开连接
        smu_inst = rm.open_resource(address)
        smu_inst.timeout = 20000
        smu_inst.read_termination = '\n'
        smu_inst.write_termination = '\n'

        # 3. 复位并确认
        smu_inst.write("*RST")
        smu_inst.query("*OPC?")
        idn = smu_inst.query("*IDN?").strip()

        # 清空上一次会话遗留的错误（*RST 不清错误队列），避免误报本次配置错误
        _drain_errors()

        if "2460" not in idn.upper() and "2461" not in idn.upper():
            print(f"⚠️  设备型号可能不是 2460：{idn}")
        print(f"✅ 源表初始化成功：{idn}")

        # 4. 源功能与电平
        if src_func == "VOLT":
            smu_inst.write(":SOURce:FUNCtion VOLT")
            smu_inst.write(f":SOURce:VOLTage {source_level}")
            smu_inst.write(f":SOURce:VOLTage:ILIMit {compliance}")
            smu_inst.write(":SOURce:VOLTage:RANGe:AUTO ON")
        else:
            smu_inst.write(":SOURce:FUNCtion CURR")
            smu_inst.write(f":SOURce:CURRent {source_level}")
            smu_inst.write(f":SOURce:CURRent:VLIMit {compliance}")
            smu_inst.write(":SOURce:CURRent:RANGe:AUTO ON")

        # 5. 测量功能与量程
        smu_inst.write(f':SENSe:FUNCtion "{sense}"')
        smu_inst.write(f":SENSe:{sense}:RANGe:AUTO ON")

        # 6. 2 线 / 4 线
        smu_inst.write(f":SYSTem:RSENse {'ON' if four_wire else 'OFF'}")

        # 7. 安全状态：输出关闭
        smu_inst.write(":OUTPut OFF")
        smu_inst.query("*OPC?")

        # 8. 错误队列检查
        errors = _drain_errors()
        if errors:
            print("⚠️  源表报告以下错误：")
            for e in errors:
                print(f"   {e}")
        else:
            print("✅ 源表配置完成（输出已关闭）")

        return True

    except Exception as e:
        print(f"❌ 源表初始化异常：{str(e)}")
        close_source_meter(quiet=True)
        return False


def get_idn():
    """
    查询源表 *IDN? 信息。

    返回：
        str/None - IDN 字符串，未初始化或查询失败返回 None
    """
    if not smu_inst:
        print("❌ 源表未初始化，请先调用 init_source_meter()")
        return None
    try:
        return smu_inst.query("*IDN?").strip()
    except Exception as e:
        print(f"❌ 查询 IDN 失败：{str(e)}")
        return None


def query_output_state():
    """
    查询源表输出开关状态。

    返回：
        bool/None - True=输出开启，False=输出关闭，查询失败返回 None
    """
    if not smu_inst:
        print("❌ 源表未初始化，请先调用 init_source_meter()")
        return None
    try:
        return smu_inst.query(":OUTPut:STATe?").strip() in ("1", "ON")
    except Exception as e:
        print(f"❌ 查询输出状态失败：{str(e)}")
        return None


def _require_instance():
    """源表未初始化时抛异常（供需要返回数值的函数使用）。"""
    if not smu_inst:
        raise RuntimeError("源表未初始化，请先调用 init_source_meter()")


def _report_errors(context):
    """读取错误队列，有错误时打印并返回 False。"""
    errors = _drain_errors()
    if errors:
        print(f"⚠️  {context}时源表报告错误：")
        for e in errors:
            print(f"   {e}")
        return False
    return True


def set_source_current(current, voltage_limit=None):
    """
    切换到电流源模式并设置输出电流与电压限值（compliance）。

    注意：本函数只改配置，不改变输出开关状态；输出需另调 output_on()。

    参数：
        current: float - 源电流（A），可为负
        voltage_limit: float/None - 电压限值（V），None 表示保持当前设置
    返回：
        bool - 配置成功且无仪器错误返回 True
    """
    try:
        _require_instance()
        smu_inst.write(":SOURce:FUNCtion CURR")
        smu_inst.write(":SOURce:CURRent:RANGe:AUTO ON")
        smu_inst.write(f":SOURce:CURRent {current}")
        if voltage_limit is not None:
            smu_inst.write(f":SOURce:CURRent:VLIMit {voltage_limit}")
        # 源电流时测量电压
        smu_inst.write(':SENSe:FUNCtion "VOLT"')
        smu_inst.write(":SENSe:VOLTage:RANGe:AUTO ON")
        smu_inst.query("*OPC?")
        return _report_errors("设置电流源")
    except Exception as e:
        print(f"❌ 设置电流源失败：{str(e)}")
        return False


def set_source_voltage(voltage, current_limit=None):
    """
    切换到电压源模式并设置输出电压与电流限值（compliance）。

    注意：本函数只改配置，不改变输出开关状态；输出需另调 output_on()。

    参数：
        voltage: float - 源电压（V），可为负
        current_limit: float/None - 电流限值（A），None 表示保持当前设置
    返回：
        bool - 配置成功且无仪器错误返回 True
    """
    try:
        _require_instance()
        smu_inst.write(":SOURce:FUNCtion VOLT")
        smu_inst.write(":SOURce:VOLTage:RANGe:AUTO ON")
        smu_inst.write(f":SOURce:VOLTage {voltage}")
        if current_limit is not None:
            smu_inst.write(f":SOURce:VOLTage:ILIMit {current_limit}")
        # 源电压时测量电流
        smu_inst.write(':SENSe:FUNCtion "CURR"')
        smu_inst.write(":SENSe:CURRent:RANGe:AUTO ON")
        smu_inst.query("*OPC?")
        return _report_errors("设置电压源")
    except Exception as e:
        print(f"❌ 设置电压源失败：{str(e)}")
        return False


def output_on():
    """
    打开源表输出。

    返回：
        bool - 成功返回 True
    """
    try:
        _require_instance()
        smu_inst.write(":OUTPut ON")
        smu_inst.query("*OPC?")
        return _report_errors("打开输出")
    except Exception as e:
        print(f"❌ 打开输出失败：{str(e)}")
        return False


def output_off():
    """
    关闭源表输出。

    返回：
        bool - 成功返回 True
    """
    try:
        _require_instance()
        smu_inst.write(":OUTPut OFF")
        smu_inst.query("*OPC?")
        return _report_errors("关闭输出")
    except Exception as e:
        print(f"❌ 关闭输出失败：{str(e)}")
        return False


def set_sense_function(function):
    """
    设置测量功能，决定源表前面板主读数显示电压还是电流。

    参数：
        function: str - "VOLT"（显示/测量电压）或 "CURR"（电流）
    返回：
        bool - 成功返回 True
    """
    try:
        _require_instance()
        func = _normalize_function(function)
        smu_inst.write(f':SENSe:FUNCtion "{func}"')
        smu_inst.query("*OPC?")
        return _report_errors("设置测量功能")
    except Exception as e:
        print(f"❌ 设置测量功能失败：{str(e)}")
        return False


def display_voltage():
    """让源表前面板主读数显示电压（设置测量功能为电压）。"""
    return set_sense_function("VOLT")


def display_current():
    """让源表前面板主读数显示电流（设置测量功能为电流）。"""
    return set_sense_function("CURR")


def _current_sense_function():
    """查询当前测量功能，返回 'VOLT' 或 'CURR'（失败返回 None）。"""
    try:
        raw = smu_inst.query(":SENSe:FUNCtion?").strip().strip('"')
        return _normalize_function(raw.split(":")[0])
    except Exception:
        return None


def measure_once():
    """
    触发一次测量，同时返回电压和电流。

    注意：:MEASure:VOLTage? / :MEASure:CURRent? 会切换仪器的测量功能，
    最后一次查询会把前面板主读数改成对应物理量。本函数在测量结束后
    恢复调用前的测量功能，避免面板显示被意外改掉。

    返回：
        (voltage, current) - 电压(V)、电流(A)；失败返回 (None, None)
    """
    try:
        _require_instance()
        sense_before = _current_sense_function()
        try:
            v = float(smu_inst.query(":MEASure:VOLTage?").strip())
            i = float(smu_inst.query(":MEASure:CURRent?").strip())
        finally:
            if sense_before:
                smu_inst.write(f':SENSe:FUNCtion "{sense_before}"')
        return v, i
    except Exception as e:
        print(f"❌ 测量失败：{str(e)}")
        try:
            err = smu_inst.query(":SYSTem:ERRor?").strip()
            print(f"   仪器错误：{err}")
        except Exception:
            pass
        return None, None


def measure_voltage():
    """
    测量电压（不改变测量功能，前面板保持当前显示）。

    返回：
        float/None - 电压(V)，失败返回 None
    """
    try:
        _require_instance()
        return float(smu_inst.query(":MEASure:VOLTage?").strip())
    except Exception as e:
        print(f"❌ 电压测量失败：{str(e)}")
        return None


def close_source_meter(quiet=False, output_off=True):
    """
    关闭源表连接并释放 VISA 资源。

    参数：
        quiet: bool - True 时不打印提示（初始化失败清理时使用）
        output_off: bool - True 先强制关闭输出（默认，安全）；
                           False 保持输出状态不变，用于"常开"场景。
                           输出状态由仪器自身保持，关闭通信会话不会改变它。
    """
    global smu_inst, rm
    was_open = smu_inst is not None or rm is not None
    if smu_inst:
        if output_off:
            try:
                smu_inst.write(":OUTPut OFF")
                smu_inst.query("*OPC?")
            except Exception:
                pass
        try:
            smu_inst.close()
        except Exception:
            pass
    if rm:
        try:
            rm.close()
        except Exception:
            pass
    smu_inst = None
    rm = None
    if not quiet and was_open:
        print("✅ 源表连接已关闭")


# ------------------- 调用示例 -------------------
if __name__ == "__main__":
    if init_source_meter(source_function="VOLT", source_level=0.0, compliance=1e-3):
        print(f"输出状态：{'ON' if query_output_state() else 'OFF'}")
    close_source_meter()
