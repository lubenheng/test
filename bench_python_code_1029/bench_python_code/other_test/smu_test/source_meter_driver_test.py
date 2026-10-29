# -*- coding: utf-8 -*-
"""
KEITHLEY 2460 驱动路径实测（走 instrument_set 的真实调用链）

与 source_meter_cmd_test.py 的区别：
  · 那个脚本逐条探测 SCPI 原始命令；
  · 本脚本只调用程序里真正会用的接口（smu_init / smu_source_current_on /
    smu_measure / smu_output_off / smu_print_state），验证驱动封装是否正确、
    终端输出是否与仪器实际状态一致。

默认用 1 mA / 1 V 限压（安全值），输出端空载时会顶到限压，属正常现象；
要验证真实负载请改 --current / --vlimit。

运行：
    python other_test/smu_test/source_meter_driver_test.py
    python other_test/smu_test/source_meter_driver_test.py --current 0.2 --vlimit 5
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

from instrument import instrument_set as instr  # noqa: E402


def check(name, cond, detail=""):
    """打印一条检查结果，返回是否通过。"""
    print(f"   {'✅' if cond else '❌'} {name}" + (f"：{detail}" if detail else ""))
    return bool(cond)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--current", type=float, default=0.001)
    ap.add_argument("--vlimit", type=float, default=1.0)
    ap.add_argument("--settle", type=float, default=1.0)
    a = ap.parse_args()

    passed = True
    try:
        print("=" * 78)
        print("1) 初始化")
        print("=" * 78)
        passed &= check("smu_init()", instr.smu_init(), "复位 + 配置 + 输出关闭")
        idn = instr.smu_get_idn()
        passed &= check("smu_get_idn()", bool(idn), str(idn))
        passed &= check("初始输出为关", instr.smu_output_state() is False,
                        f":OUTPut:STATe? = {instr.smu_output_state()}")

        print()
        print("=" * 78)
        print(f"2) 五步上电（{a.current * 1000:.3f} mA / {a.vlimit:g} V 限压）")
        print("=" * 78)
        ok = instr.smu_source_current_on(a.current, voltage_limit=a.vlimit)
        passed &= check("smu_source_current_on()", ok)

        state = instr.smu_query_state()
        print(f"   回读：{state}")
        func = str(state.get("function", "")).upper()
        passed &= check("源功能 = CURR", "CURR" in func, func)
        try:
            cur_set = float(state.get("current", "nan"))
        except ValueError:
            cur_set = float("nan")
        passed &= check("源电流 = 设定值", abs(cur_set - a.current) < a.current * 0.01 + 1e-9,
                        f"{cur_set} A（期望 {a.current}）")
        try:
            vlim_set = float(state.get("vlimit", "nan"))
        except ValueError:
            vlim_set = float("nan")
        passed &= check("电压限值 = 设定值", abs(vlim_set - a.vlimit) < 1e-6,
                        f"{vlim_set} V（期望 {a.vlimit}）")
        passed &= check("输出已打开", instr.smu_output_state() is True)

        print()
        print("=" * 78)
        print("3) 回读 V / I")
        print("=" * 78)
        time.sleep(a.settle)
        v, i = instr.smu_measure()
        passed &= check("smu_measure() 有返回", v is not None and i is not None,
                        f"V = {v} V，I = {i} A")
        if v is not None and i is not None:
            i_ratio = abs(i) / a.current if a.current else 0
            print(f"   实测电流 / 设定电流 = {i_ratio * 100:.1f}%")
            if i_ratio < 0.5:
                if abs(v) >= a.vlimit * 0.9:
                    print("   ℹ️  电压顶到限压 → 输出端开路（空载），命令链路本身正常")
                elif abs(v) < 0.05:
                    print("   ⚠️  端电压≈0 → 输出端短路，或输出未真正打开")
                else:
                    print("   ⚠️  电流明显小于设定值，请检查负载与限压")
            else:
                print("   ✅ 电流按设定值灌出")

        print()
        print("=" * 78)
        print("4) 关输出")
        print("=" * 78)
        passed &= check("smu_output_off()", instr.smu_output_off(),
                        "回读确认输出已关")
        passed &= check("输出状态为关", instr.smu_output_state() is False)

    finally:
        print()
        print("=" * 78)
        print("5) 清理")
        print("=" * 78)
        try:
            instr.smu_output_off()
        except Exception:
            pass
        instr.smu_close()
        print("   已关闭连接（输出已强制关闭）")

    print()
    print("=" * 78)
    print("全部通过 ✅" if passed else "存在失败项 ❌")
    print("=" * 78)
    return 0 if passed else 1


if __name__ == "__main__":
    sys.exit(main())
