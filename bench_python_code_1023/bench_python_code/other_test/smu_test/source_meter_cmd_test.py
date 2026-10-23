# -*- coding: utf-8 -*-
"""
KEITHLEY 2460 源表命令实测工具（TIA 灌流用到的 SCPI 逐条验证）

用途
----
把驱动里用到的每一条 SCPI 单独发给仪器，读错误队列，确认：
  · 哪些命令本机固件接受；
  · 被拒绝的是哪一条、错误码是什么；
  · 源表实际配置（源功能 / 源电平 / 限值 / 输出）回读是否与预期一致；
  · 可选：真正打开输出、灌一个小电流并回读 V/I，再关闭输出。

所有探测默认 **输出保持关闭**，只有加 --output-test 才会打开输出，
且默认只用 1 mA / 1 V 限压（安全值）；要灌 200 mA 必须显式加 --current 0.2。

运行
----
    python other_test/smu_test/source_meter_cmd_test.py                 # 只探测命令，不输出
    python other_test/smu_test/source_meter_cmd_test.py --output-test   # 1 mA / 1 V 实际灌流验证
    python other_test/smu_test/source_meter_cmd_test.py --output-test --current 0.2 --vlimit 5

    # 指定设备（本机有两个 2460 资源时）
    python other_test/smu_test/source_meter_cmd_test.py --resource "USB0::0x05E6::0x2460::04616368::INSTR"
"""

import argparse
import os
import sys
import time
from pathlib import Path

if os.name == "nt":
    os.system("chcp 65001 > nul")
    sys.stdout.reconfigure(encoding="utf-8")
    sys.stderr.reconfigure(encoding="utf-8")

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT))

import pyvisa  # noqa: E402


# ----------------------------------------------------------------------------
# 仪器连接
# ----------------------------------------------------------------------------

def list_2460_resources():
    """列出总线上所有 2460 资源，并返回 [(资源, IDN 或错误)]。"""
    rm = pyvisa.ResourceManager()
    found = []
    for res in rm.list_resources():
        if "0x05E6::0x2460" not in res and "2460" not in res:
            continue
        try:
            inst = rm.open_resource(res)
            inst.timeout = 3000
            inst.read_termination = "\n"
            inst.write_termination = "\n"
            idn = inst.query("*IDN?").strip()
            inst.close()
            found.append((res, idn))
        except Exception as e:
            found.append((res, f"(无法通信: {type(e).__name__})"))
    return rm, found


def open_2460(rm, resource=None):
    """打开 2460。resource 为 None 时自动选第一台能查通 IDN 的。"""
    _, found = list_2460_resources()
    print("总线上找到的 2460：")
    for res, idn in found:
        print(f"   {res}  →  {idn}")
    print()

    if resource:
        addr = resource
    else:
        usable = [r for r, idn in found if "KEITHLEY" in idn.upper()]
        if not usable:
            raise RuntimeError("未找到可通信的 KEITHLEY 2460")
        addr = usable[0]

    inst = rm.open_resource(addr)
    inst.timeout = 10000
    inst.read_termination = "\n"
    inst.write_termination = "\n"
    print(f"已连接：{addr}")
    print(f"IDN   ：{inst.query('*IDN?').strip()}\n")
    return inst, addr


# ----------------------------------------------------------------------------
# 命令探测
# ----------------------------------------------------------------------------

def drain_errors(inst, max_count=20):
    """读空错误队列，返回 [(code, message)]。"""
    errors = []
    for _ in range(max_count):
        try:
            raw = inst.query(":SYSTem:ERRor?").strip()
        except Exception as e:
            errors.append((-999, f"查询错误队列失败: {e}"))
            break
        code_txt = raw.split(",", 1)[0].strip()
        try:
            code = int(code_txt)
        except ValueError:
            code = -999
        if code == 0:
            break
        errors.append((code, raw))
    return errors


def probe_write(inst, cmd, label):
    """发一条写命令，返回 (是否接受, 错误描述)。"""
    drain_errors(inst)
    try:
        inst.write(cmd)
        inst.query("*OPC?")
    except Exception as e:
        return False, f"{type(e).__name__}: {e}"
    errors = drain_errors(inst)
    if errors:
        return False, errors[0][1]
    return True, ""


def probe_query(inst, cmd, label):
    """发一条查询命令，返回 (是否成功, 返回或错误)。"""
    drain_errors(inst)
    try:
        resp = inst.query(cmd).strip()
    except Exception as e:
        errs = drain_errors(inst)
        detail = errs[0][1] if errs else f"{type(e).__name__}: {e}"
        return False, detail
    return True, resp


#: 驱动实际会发送的写命令（与 KEITHLEY_2460.py 一一对应）
WRITE_PROBES = [
    ("*RST", "复位"),
    (":SOURce:FUNCtion CURR", "① 选择电流源"),
    (":SOURce:FUNCtion VOLT", "   选择电压源"),
    (":SOURce:CURRent:RANGe:AUTO ON", "   电流量程自动"),
    (":SOURce:CURRent:VLIMit 5.0", "③ 电压限值（2460 写法）"),
    (":SENSe:VOLTage:PROTection 5.0", "③ 电压限值（2400 写法，回退）"),
    (':SENSe:FUNCtion "VOLTage"', "② 电压档（长写法）"),
    (':SENSe:FUNCtion "VOLT"', "② 电压档（短写法，回退）"),
    (":SENSe:VOLTage:RANGe:AUTO ON", "   测量电压量程自动"),
    (":SOURce:CURRent 0.001", "④ 源电流 1 mA"),
    (":SOURce:VOLTage 0.0", "   源电压 0 V"),
    (":SOURce:VOLTage:ILIMit 0.001", "   电流限值（电压源用）"),
    (":SENSe:CURRent:PROTection 0.001", "   电流限值（2400 写法）"),
    (":SYSTem:RSENse OFF", "   两线测量"),
    (":OUTPut:STATe OFF", "⑤ 输出关（安全）"),
    (":OUTPut:STATe ON", "⑤ 输出开（探测后立即关）"),
    (":DISPlay:ENABle ON", "   [已删除] 显示开"),
    (":FORMat:ELEMents VOLT,CURR,STAT", "   [已删除] 回读元素"),
    (":INITiate:CONTinuous ON", "   [已删除] 连续测量"),
]

#: 驱动实际会发送的查询命令
QUERY_PROBES = [
    ("*IDN?", "型号"),
    (":OUTPut:STATe?", "输出状态"),
    (":SOURce:FUNCtion?", "源功能"),
    (":SOURce:CURRent?", "源电流"),
    (":SOURce:CURRent:VLIMit?", "电压限值"),
    (":SENSe:FUNCtion?", "测量功能"),
    (":MEASure:VOLTage?", "测量电压"),
    (":MEASure:CURRent?", "测量电流"),
    (":READ?", "单次读数"),
    (":FETCh?", "取最新读数"),
]


def run_probes(inst):
    """逐条探测写/查询命令，打印结果表。"""
    results = []

    print("=" * 88)
    print("写命令探测（输出默认关闭，探测过程中会短暂 OFF 一次 ON 后立即再 OFF）")
    print("=" * 88)
    # 先确保输出关闭，避免探测 :OUTPut:STATe ON 时真的带电
    probe_write(inst, ":OUTPut:STATe OFF", "安全准备")
    probe_write(inst, ":SOURce:CURRent 0.001", "安全准备：源电流降到 1 mA")

    for cmd, label in WRITE_PROBES:
        ok, detail = probe_write(inst, cmd, label)
        # 探测 :OUTPut:STATe ON 之后立刻关掉
        if cmd == ":OUTPut:STATe ON":
            probe_write(inst, ":OUTPut:STATe OFF", "关闭输出")
        results.append((label, cmd, ok, detail))
        mark = "✅" if ok else "❌"
        print(f"{mark} {label:<28} {cmd}")
        if not ok:
            print(f"      错误：{detail}")

    print()
    print("=" * 88)
    print("查询命令探测")
    print("=" * 88)
    for cmd, label in QUERY_PROBES:
        ok, resp = probe_query(inst, cmd, label)
        results.append((label, cmd, ok, resp))
        mark = "✅" if ok else "❌"
        print(f"{mark} {label:<28} {cmd:<34} → {resp}")

    return results


# ----------------------------------------------------------------------------
# 实际输出验证
# ----------------------------------------------------------------------------

def state_snapshot(inst):
    """回读源表当前配置。"""
    snap = {}
    for key, cmd in (
        ("function", ":SOURce:FUNCtion?"),
        ("current", ":SOURce:CURRent?"),
        ("voltage", ":SOURce:VOLTage?"),
        ("vlimit", ":SOURce:CURRent:VLIMit?"),
        ("output", ":OUTPut:STATe?"),
        ("sense", ":SENSe:FUNCtion?"),
    ):
        ok, resp = probe_query(inst, cmd, key)
        snap[key] = resp if ok else f"(查询失败: {resp})"
    return snap


def print_snapshot(snap, prefix="   "):
    print(f"{prefix}源功能={snap.get('function')}  源电流={snap.get('current')}  "
          f"源电压={snap.get('voltage')}  电压限值={snap.get('vlimit')}  "
          f"测量功能={snap.get('sense')}  输出={snap.get('output')}")


def run_output_test(inst, current_a, vlimit_v, settle_s):
    """真正打开输出、灌流、回读 V/I、关闭输出。返回是否全部通过。"""
    print()
    print("=" * 88)
    print(f"实际输出验证：电流源 {current_a * 1000:.3f} mA，电压限值 {vlimit_v:g} V")
    print("=" * 88)
    print("⚠️  请确认源表输出端当前接的是安全的负载（不要接在被测芯片的敏感引脚上）")
    try:
        input("   回车开始，Ctrl+C 取消 ... ")
    except (EOFError, KeyboardInterrupt):
        print("\n   已取消")
        return False

    ok = True
    # 五步
    steps = [
        (":SOURce:FUNCtion CURR", "选择电流源"),
        (':SENSe:FUNCtion "VOLTage"', "选择电压档"),
        (f":SOURce:CURRent:VLIMit {vlimit_v}", "设置电压限值"),
        (f":SOURce:CURRent {current_a}", "设置源电流"),
        (":OUTPut:STATe ON", "上电"),
    ]
    for cmd, label in steps:
        good, detail = probe_write(inst, cmd, label)
        print(f"   {'✅' if good else '❌'} {label:<12} {cmd}"
              + ("" if good else f"  → {detail}"))
        ok &= good

    try:
        time.sleep(settle_s)
        print()
        print("   上电后回读：")
        print_snapshot(state_snapshot(inst))

        # 源表自身读数
        v_ok, v_resp = probe_query(inst, ":MEASure:VOLTage?", "V")
        i_ok, i_resp = probe_query(inst, ":MEASure:CURRent?", "I")
        print(f"   源表读数：V = {v_resp if v_ok else '(失败: ' + v_resp + ')'}，"
              f"I = {i_resp if i_ok else '(失败: ' + i_resp + ')'}")

        if v_ok and i_ok:
            v = float(v_resp.split(",")[0])
            i = float(i_resp.split(",")[0])
            i_ratio = abs(i) / current_a if current_a else 0
            print(f"   实测电流 / 设定电流 = {i_ratio * 100:.1f}%")
            if i_ratio < 0.5:
                if abs(v) >= vlimit_v * 0.9:
                    print("   ⚠️  电压已顶到限压：回路不通（输出端开路或接触不良）")
                elif abs(v) < 0.05:
                    print("   ⚠️  端电压≈0：输出端短路，或输出实际未打开")
                else:
                    print("   ⚠️  电流明显小于设定值：请检查负载与限压")
            else:
                print("   ✅ 电流已按设定值灌出")
    finally:
        good, _ = probe_write(inst, ":OUTPut:STATe OFF", "关闭输出")
        print(f"   {'✅' if good else '❌'} 关闭输出")
        print_snapshot(state_snapshot(inst))
        ok &= good

    return ok


# ----------------------------------------------------------------------------
# main
# ----------------------------------------------------------------------------

def main():
    ap = argparse.ArgumentParser(description="KEITHLEY 2460 命令实测")
    ap.add_argument("--resource", default=None, help="指定 VISA 资源地址")
    ap.add_argument("--output-test", action="store_true", help="实际打开输出灌流验证")
    ap.add_argument("--current", type=float, default=0.001, help="灌流电流 A（默认 0.001）")
    ap.add_argument("--vlimit", type=float, default=1.0, help="电压限值 V（默认 1.0）")
    ap.add_argument("--settle", type=float, default=1.0, help="上电后等待秒数")
    a = ap.parse_args()

    rm = pyvisa.ResourceManager()
    inst = None
    try:
        inst, addr = open_2460(rm, a.resource)
        results = run_probes(inst)

        rejected = [r for r in results if not r[2]]
        print()
        print("=" * 88)
        if rejected:
            print(f"被仪器拒绝的命令 {len(rejected)} 条：")
            for label, cmd, ok, detail in rejected:
                print(f"   {label:<28} {cmd}")
                print(f"      {detail}")
        else:
            print("所有探测命令均被仪器接受")
        print("=" * 88)

        # 恢复安全状态
        probe_write(inst, ":SOURce:CURRent 0.0", "源电流归零")
        probe_write(inst, ":OUTPut:STATe OFF", "输出关闭")

        out_ok = True
        if a.output_test:
            out_ok = run_output_test(inst, a.current, a.vlimit, a.settle)
        else:
            print("\n（未加 --output-test，跳过实际灌流验证）")

        print()
        print(f"结果：命令探测 {'通过' if not rejected else '有被拒命令'}，"
              f"输出验证 {'通过' if out_ok else ('未执行' if not a.output_test else '失败')}")
        return 0 if not rejected and out_ok else 1
    finally:
        if inst is not None:
            try:
                inst.write(":OUTPut:STATe OFF")
            except Exception:
                pass
            try:
                inst.close()
            except Exception:
                pass
        try:
            rm.close()
        except Exception:
            pass
        print("连接已关闭（输出已强制关闭）")


if __name__ == "__main__":
    sys.exit(main())
