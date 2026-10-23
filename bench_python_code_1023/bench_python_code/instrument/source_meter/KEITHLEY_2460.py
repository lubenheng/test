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
            _write_checked(":SOURce:FUNCtion VOLT", "源功能")
            _write_checked(f":SOURce:VOLTage {source_level}", "源电压值")
            _write_first_ok([f":SOURce:VOLTage:ILIMit {compliance}",
                             f":SENSe:CURRent:PROTection {compliance}"], "电流限值")
            _write_checked(":SOURce:VOLTage:RANGe:AUTO ON", "电压量程")
        else:
            _write_checked(":SOURce:FUNCtion CURR", "源功能")
            _write_checked(f":SOURce:CURRent {source_level}", "源电流值")
            _write_first_ok([f":SOURce:CURRent:VLIMit {compliance}",
                             f":SENSe:VOLTage:PROTection {compliance}"], "电压限值")
            _write_checked(":SOURce:CURRent:RANGe:AUTO ON", "电流量程")

        # 5. 测量功能与量程
        sense_cmds = ([':SENSe:FUNCtion "VOLTage"', ':SENSe:FUNCtion "VOLT"']
                      if sense == "VOLT" else
                      [':SENSe:FUNCtion "CURRent"', ':SENSe:FUNCtion "CURR"'])
        _write_first_ok(sense_cmds, "测量功能")
        _write_first_ok([f":SENSe:{sense}:RANGe:AUTO ON",
                         f":SENSe:{sense}:RANGe:AUTO 1"], "测量量程")

        # 6. 2 线 / 4 线
        _write_checked(f":SYSTem:RSENse {'ON' if four_wire else 'OFF'}", "四线测量")

        # 7. 安全状态：输出关闭
        if _write_first_ok([":OUTPut:STATe OFF", ":OUTPut OFF"], "关闭输出") is None:
            print("   ❌ 初始化时无法关闭输出，请手动确认面板 OUTPUT 已灭")

        # 8. 错误队列检查
        errors = _drain_errors()
        if errors:
            print("⚠️  源表报告以下错误：")
            for e in errors:
                print(f"   {e}")
        else:
            print("✅ 源表配置完成（输出已关闭）")
        print_source_state()

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


def _write_checked(cmd, context):
    """发送一条 SCPI 并立即检查错误队列，确认仪器真的接受了这条命令。

    与“先发一堆命令、最后统一查错误”相比，这样能精确定位是哪条命令被拒绝、
    仪器报了什么，避免出现“输出打开了但源值没写进去”的静默失败。

    返回：
        bool - True=命令被接受；False=发送异常或被仪器报错
    """
    try:
        smu_inst.write(cmd)
        smu_inst.query("*OPC?")
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

    用于不同固件/命令集写法有差异的项（如限值命令 VLIMit / PROTection、
    测量功能 "VOLTage" / "VOLT"），避免一处写法不对导致整段配置失效。
    """
    for cmd in candidates:
        try:
            smu_inst.write(cmd)
            smu_inst.query("*OPC?")
        except Exception:
            continue
        if not _drain_errors():
            if not quiet and cmd != candidates[0]:
                print(f"   {context}：`{cmd}`（回退写法）")
            return cmd
    return None


def query_source_state():
    """回读源表当前配置（源功能/电平/限值/输出状态），便于核对是否真的生效。

    返回：
        dict - 各字段的字符串值；查询失败的字段值为 "(查询失败: ...)"
    """
    if not smu_inst:
        return {}
    state = {}
    for key, cmd in (
        ("function", ":SOURce:FUNCtion?"),
        ("current", ":SOURce:CURRent?"),
        ("voltage", ":SOURce:VOLTage?"),
        ("vlimit", ":SOURce:CURRent:VLIMit?"),
        ("ilimit", ":SOURce:VOLTage:ILIMit?"),
        ("output", ":OUTPut:STATe?"),
    ):
        try:
            state[key] = smu_inst.query(cmd).strip()
        except Exception as e:
            state[key] = f"(查询失败: {e})"
    _drain_errors()      # 清掉查询（如非当前模式下的限值读取）产生的错误
    return state


def print_source_state(prefix="   源表配置："):
    """打印 query_source_state() 的回读结果。

    只显示当前源模式真正生效的那一项限值：电流源看电压限值，电压源看电流限值。
    （另一项在非当前模式下回读的是仪器的无关默认值，打印出来反而容易误判。）
    """
    state = query_source_state()
    if not state:
        print(f"{prefix}源表未初始化")
        return
    func = str(state.get("function", "")).upper()
    if "CURR" in func:
        detail = f"源电流={state.get('current')}，电压限值={state.get('vlimit')}"
    else:
        detail = f"源电压={state.get('voltage')}，电流限值={state.get('ilimit')}"
    print(f"{prefix}源功能={state.get('function')}，{detail}，输出={state.get('output')}")


def set_source_current(current, voltage_limit=None):
    """
    切换到电流源模式并设置输出电流与电压限值（compliance）。

    注意：本函数只改配置，不改变输出开关状态；输出需另调 output_on()。
    每条 SCPI 都单独校验，失败时打印具体是哪条被拒，调用方可据此中止流程。

    参数：
        current: float - 源电流（A），可为负
        voltage_limit: float/None - 电压限值（V），None 表示保持当前设置
    返回：
        bool - 所有配置项都已生效返回 True
    """
    try:
        _require_instance()
        ok = True
        ok &= _write_checked(":SOURce:FUNCtion CURR", "源功能")
        ok &= _write_checked(":SOURce:CURRent:RANGe:AUTO ON", "电流量程")
        ok &= _write_checked(f":SOURce:CURRent {current}", "源电流值")

        if voltage_limit is not None:
            cmd = _write_first_ok([
                f":SOURce:CURRent:VLIMit {voltage_limit}",
                f":SENSe:VOLTage:PROTection {voltage_limit}",
            ], "电压限值")
            if cmd is None:
                print("   ❌ 电压限值设置失败：仪器既不接受 :SOURce:CURRent:VLIMit，"
                      "也不接受 :SENSe:VOLTage:PROTection")
                ok = False

        # 源电流时测量电压
        if _write_first_ok([':SENSe:FUNCtion "VOLTage"', ':SENSe:FUNCtion "VOLT"'],
                           "测量功能") is None:
            print("   ❌ 测量功能设置失败：仪器不接受 :SENSe:FUNCtion \"VOLTage\"")
            ok = False
        if _write_first_ok([":SENSe:VOLTage:RANGe:AUTO ON", ":SENSe:VOLTage:RANGe:AUTO 1"],
                           "电压量程") is None:
            print("   ❌ 测量电压量程自动设置失败")
            ok = False

        if not ok:
            print("   ⚠️  电流源配置未全部生效，源表当前配置回读：")
            print_source_state("      ")
        return ok
    except Exception as e:
        print(f"❌ 设置电流源失败：{str(e)}")
        return False


def set_source_voltage(voltage, current_limit=None):
    """
    切换到电压源模式并设置输出电压与电流限值（compliance）。

    注意：本函数只改配置，不改变输出开关状态；输出需另调 output_on()。
    每条 SCPI 都单独校验。

    参数：
        voltage: float - 源电压（V），可为负
        current_limit: float/None - 电流限值（A），None 表示保持当前设置
    返回：
        bool - 所有配置项都已生效返回 True
    """
    try:
        _require_instance()
        ok = True
        ok &= _write_checked(":SOURce:FUNCtion VOLT", "源功能")
        ok &= _write_checked(":SOURce:VOLTage:RANGe:AUTO ON", "电压量程")
        ok &= _write_checked(f":SOURce:VOLTage {voltage}", "源电压值")

        if current_limit is not None:
            cmd = _write_first_ok([
                f":SOURce:VOLTage:ILIMit {current_limit}",
                f":SENSe:CURRent:PROTection {current_limit}",
            ], "电流限值")
            if cmd is None:
                print("   ❌ 电流限值设置失败：仪器既不接受 :SOURce:VOLTage:ILIMit，"
                      "也不接受 :SENSe:CURRent:PROTection")
                ok = False

        # 源电压时测量电流
        if _write_first_ok([':SENSe:FUNCtion "CURRent"', ':SENSe:FUNCtion "CURR"'],
                           "测量功能") is None:
            print("   ❌ 测量功能设置失败：仪器不接受 :SENSe:FUNCtion \"CURRent\"")
            ok = False
        if _write_first_ok([":SENSe:CURRent:RANGe:AUTO ON", ":SENSe:CURRent:RANGe:AUTO 1"],
                           "电流量程") is None:
            print("   ❌ 测量电流量程自动设置失败")
            ok = False

        if not ok:
            print("   ⚠️  电压源配置未全部生效，源表当前配置回读：")
            print_source_state("      ")
        return ok
    except Exception as e:
        print(f"❌ 设置电压源失败：{str(e)}")
        return False


# 灌流默认值：电流源 200 mA，电压限值 5 V
SOURCE_CURRENT_A = 0.2
SOURCE_VLIMIT_V = 5.0


def source_current_on(current=SOURCE_CURRENT_A, voltage_limit=SOURCE_VLIMIT_V):
    """只做五步：选电流源 → 选电压档（测电压）→ 电压限值 → 源电流 → 上电。

    专供「给某个脚灌固定电流、用电压档读回电压」的场景（如 TIA 灌 200 mA）。
    不设置数值量程/连续测量/显示等其它配置，也不改变其它已配置项。

    发送的 SCPI（每条单独校验，被拒会打印具体命令与仪器错误码）：
        :SOURce:FUNCtion CURR
        :SENSe:FUNCtion "VOLTage"          ← 电压档
        :SOURce:CURRent:VLIMit 5.0         ← 电压限值 5 V（部分固件用 :SENSe:VOLTage:PROTection）
        :SOURce:CURRent 0.2                ← 源 200 mA
        :OUTPut:STATe ON                   ← 上电，并回读确认

    参数：
        current: float - 源电流（A），默认 0.2（200 mA）
        voltage_limit: float - 电压限值（V），默认 5
    返回：
        bool - 五步全部生效返回 True
    """
    try:
        _require_instance()
        print(f"   源表指令：电流源 → 电压档 → {current * 1000:.0f} mA，"
              f"限压 {voltage_limit:g} V → 上电")
        ok = True

        # ① 选电流源
        ok &= _write_checked(":SOURce:FUNCtion CURR", "选择电流源")

        # ② 选电压档（源电流时测量电压）
        if _write_first_ok([':SENSe:FUNCtion "VOLTage"', ':SENSe:FUNCtion "VOLT"'],
                           "选择电压档") is None:
            print("   ❌ 选择电压档失败：仪器不接受 :SENSe:FUNCtion \"VOLTage\"")
            ok = False

        # ③ 电压限值
        if _write_first_ok([f":SOURce:CURRent:VLIMit {voltage_limit}",
                            f":SENSe:VOLTage:PROTection {voltage_limit}"],
                           "电压限值") is None:
            print(f"   ❌ 电压限值 {voltage_limit:g} V 设置失败")
            ok = False

        # ④ 源电流
        ok &= _write_checked(f":SOURce:CURRent {current}", "设置源电流")

        # ⑤ 上电：打开输出并回读确认
        if not output_on():
            ok = False

        if not ok:
            print("   ⚠️  源表五步未全部生效，当前配置：")
            print_source_state("      ")
        return ok
    except Exception as e:
        print(f"❌ 源表上电失败：{str(e)}")
        return False


def output_on():
    """
    打开源表输出，并回读确认确实已打开。

    返回：
        bool - 成功返回 True
    """
    try:
        _require_instance()
        if _write_first_ok([":OUTPut:STATe ON", ":OUTPut ON"], "打开输出") is None:
            print("   ❌ 打开输出失败：仪器不接受 :OUTPut ON")
            return False
        state = query_output_state()
        if state is not True:
            print(f"   ❌ 打开输出后回读 :OUTPut:STATe? = {state}，输出未生效")
            return False
        return True
    except Exception as e:
        print(f"❌ 打开输出失败：{str(e)}")
        return False


def output_off():
    """
    关闭源表输出，并回读确认确实已关闭。

    返回：
        bool - 成功返回 True
    """
    try:
        _require_instance()
        if _write_first_ok([":OUTPut:STATe OFF", ":OUTPut OFF"], "关闭输出") is None:
            print("   ❌ 关闭输出失败：仪器不接受 :OUTPut OFF")
            return False
        state = query_output_state()
        if state is not False:
            print(f"   ⚠️  关闭输出后回读 :OUTPut:STATe? = {state}")
            return False
        return True
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


def _parse_first_float(resp):
    """从 "1.23e-3,2.4e-1,..." 这类返回里取第一个数值。"""
    for token in str(resp).replace(";", ",").split(","):
        try:
            return float(token.strip())
        except ValueError:
            continue
    return None


def _read_measurement(function):
    """读一次测量值：优先 :MEASure:<func>?，不被支持时退回 设测量功能 + :READ?。

    参数：
        function: str - "VOLT" 或 "CURR"
    返回：
        float/None - 读数；失败返回 None
    """
    cmd = ":MEASure:VOLTage?" if function == "VOLT" else ":MEASure:CURRent?"
    try:
        return float(smu_inst.query(cmd).strip())
    except Exception:
        _drain_errors()

    long_name = "VOLTage" if function == "VOLT" else "CURRent"
    if _write_first_ok([f':SENSe:FUNCtion "{long_name}"'], "测量功能", quiet=True) is None:
        return None
    try:
        return _parse_first_float(smu_inst.query(":READ?").strip())
    except Exception as e:
        print(f"❌ 读取 {function} 失败：{e}")
        _drain_errors()
        return None


def measure_once():
    """
    读一次电压和电流。

    读数来源优先级：
      1. `:MEASure:VOLTage?` / `:MEASure:CURRent?`
      2. 上述查询不被支持 →「设置测量功能 + :READ?」

    返回：
        (voltage, current) - 电压(V)、电流(A)；失败返回 (None, None)
    """
    try:
        _require_instance()

        sense_before = _current_sense_function()
        try:
            v = _read_measurement("VOLT")
            i = _read_measurement("CURR")
        finally:
            if sense_before == "VOLT":
                _write_first_ok([':SENSe:FUNCtion "VOLTage"', ':SENSe:FUNCtion "VOLT"'],
                                "恢复测量功能", quiet=True)
            elif sense_before == "CURR":
                _write_first_ok([':SENSe:FUNCtion "CURRent"', ':SENSe:FUNCtion "CURR"'],
                                "恢复测量功能", quiet=True)
        if v is None or i is None:
            print(f"⚠️  源表读数不完整：V={v}，I={i}")
        return v, i
    except Exception as e:
        print(f"❌ 测量失败：{str(e)}")
        _drain_errors()
        return None, None


def measure_voltage():
    """
    测量电压（不改变测量功能，前面板保持当前显示）。

    返回：
        float/None - 电压(V)，失败返回 None
    """
    try:
        _require_instance()
        return _read_measurement("VOLT")
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
    # 不关闭 rm：ResourceManager 是每个 VISA 库共享的单例，关掉会把万用表/示波器/
    # 电源的会话一起终结（Invalid session handle）。
    smu_inst = None
    rm = None
    if not quiet and was_open:
        print("✅ 源表连接已关闭")


# ------------------- 调用示例 -------------------
if __name__ == "__main__":
    if init_source_meter(source_function="VOLT", source_level=0.0, compliance=1e-3):
        print(f"输出状态：{'ON' if query_output_state() else 'OFF'}")
    close_source_meter()
