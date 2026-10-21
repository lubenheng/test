# -*- coding: utf-8 -*-
"""
instr_cli.py - 仪器统一命令行入口, 独立于 Claude Code, 任何人在终端里都能直接用。

对 instrument/instrument_set.py 做白名单封装, 输出 JSON, 方便人工调用或被脚本/CI 调用。

用法(在仓库任意目录执行均可, 脚本会自动定位仓库根目录):
    python instrument/instr_cli.py list
    python instrument/instr_cli.py visa
    python instrument/instr_cli.py run osc_init osc_measure_frequency osc_close
    python instrument/instr_cli.py run sig_init 'sig_set_pwm:{"frequency":1000}' sig_close

`run` 按顺序执行多个调用(同一进程内共享连接), 每项格式 `函数名` 或 `函数名:{json kwargs}`。
一次进程 = 一次会话: 仪器连接不跨进程保留, 所以 init / 测量 / close 要写在同一条 run 里。
DANGEROUS 集合内的函数(输出使能、写寄存器等)必须加 --allow-dangerous, 且应已获用户确认。
"""
import argparse
import json
import sys
from pathlib import Path

# 仓库根目录 = 本文件所在 instrument/ 目录的上一级, 不依赖调用者所在目录或本文件被谁引用
ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT))

SAFE = {
    "osc_init", "osc_auto_set", "osc_measure_frequency", "osc_close", "osc_freq",
    "dmm_init", "dmm_measure_voltage", "dmm_measure_single_voltage", "dmm_measure_single_current",
    "dmm_measure_resistance", "dmm_measure_single_resistance", "dmm_release",
    "dmm_voltage_once", "dmm_current_once", "dmm_resistance_once",
    "sig_init", "sig_close", "sig_get_idn", "sig_get_params", "sig_output_state",
    "sig_get_frequency", "sig_get_duty_cycle", "sig_get_amplitude", "sig_get_offset",
    "sig_get_high_level", "sig_get_low_level",
    "smu_init", "smu_close", "smu_get_idn", "smu_output_state", "smu_measure",
    "smu_measure_voltage", "smu_display_voltage", "smu_display_current",
    "smu_query_state", "smu_print_state",
    "serial_list_ports", "serial_list_ports_detailed", "serial_find_ports_by_keyword",
    "swd_init", "swd_close", "swd_read_register",
}
# 会改变被测板/仪器输出状态
DANGEROUS = {
    "sig_set_pwm", "sig_set_frequency", "sig_set_duty_cycle", "sig_set_amplitude", "sig_set_offset",
    "sig_set_high_low", "sig_output_on", "sig_output_off", "sig_output_both_on", "sig_output_both_off",
    "sig_set_waveform", "sig_apply_square", "sig_set_ch2_follow_ch1", "sig_set_ch2_independent",
    "sig_set_voltage_couple",
    "swd_write_register", "swd_modify_register",
    "serial_init", "serial_auto_init", "serial_send", "serial_send_and_receive", "serial_receive", "serial_close",
    "smu_set_current", "smu_set_voltage", "smu_output_on", "smu_output_off",
    "smu_source_current_on",
}
CLOSERS = {"osc": "osc_close", "dmm": "dmm_release", "sig": "sig_close", "smu": "smu_close", "swd": "swd_close"}


def out(obj):
    print(json.dumps(obj, ensure_ascii=False, default=str))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("action", choices=["list", "visa", "run"])
    ap.add_argument("calls", nargs="*")
    ap.add_argument("--allow-dangerous", action="store_true")
    a = ap.parse_args()

    if a.action == "list":
        out({"safe": sorted(SAFE), "dangerous": sorted(DANGEROUS)})
        return
    if a.action == "visa":
        from instrument.visa_backend import get_resource_manager
        rm = get_resource_manager()
        info = {}
        for r in rm.list_resources():
            try:
                i = rm.open_resource(r, open_timeout=2000)
                i.timeout = 2000
                info[r] = i.query("*IDN?").strip()
                i.close()
            except Exception as e:
                info[r] = f"(no IDN: {type(e).__name__})"
        out({"resources": info})
        return

    plan = []
    for c in a.calls:
        name, _, js = c.partition(":")
        if name not in SAFE | DANGEROUS:
            out({"ok": False, "error": f"unknown function {name}"})
            sys.exit(2)
        if name in DANGEROUS and not a.allow_dangerous:
            out({"ok": False, "error": f"{name} is DANGEROUS; confirm with user then rerun with --allow-dangerous"})
            sys.exit(3)
        plan.append((name, json.loads(js) if js else {}))

    from instrument import instrument_set as instr
    results, failed = [], False
    try:
        for name, kw in plan:
            try:
                results.append({"call": name, "kwargs": kw, "result": getattr(instr, name)(**kw)})
            except Exception as e:
                results.append({"call": name, "kwargs": kw, "error": f"{type(e).__name__}: {e}"})
                failed = True
                break
    finally:
        # 失败时也尽量释放已打开的连接
        names = [n for n, _ in plan]
        for prefix, closer in CLOSERS.items():
            if any(n.startswith(prefix + "_") for n in names) and closer not in names:
                try:
                    getattr(instr, closer)()
                except Exception:
                    pass
    out({"ok": not failed, "results": results})
    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main()
