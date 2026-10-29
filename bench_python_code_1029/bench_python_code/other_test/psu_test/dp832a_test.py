# -*- coding: utf-8 -*-
"""
RIGOL DP832A 三通道电源实测脚本（走 instrument_set 的真实调用链）

用途
----
验证新接入的 DP832A 从初始化 → 设定 → 上电 → 回读 → 关断的整条链路：
  1. psu_init()：只建立连接，不复位、不改动任何通道输出；
     脚本先记录初始状态，再自行关断三通道以获得已知起点；
  2. 三通道分别设定不同电压 / 电流限值，并回读设定值确认写进去了；
  3. psu_output_all_on()：三通道同时输出，回读输出状态；
  4. 测量三通道实际电压 / 电流 / 功率，与设定值对比；
  5. 默认测试结束自动关断三通道输出（加 --keep-on 可保持输出，便于用
     万用表逐路测量）。

默认输出 1.2 V / 3.3 V / 5.0 V（CH3 量程上限为 5 V），限流 0.5 A。
注意：DP832A 在收到输出开启命令后，端口电压约需 1~2 s 才真正建立，
      故默认等待 2.5 s 再测量；若负载较重导致电流顶到限流值，电源进入
      CC 模式，实测电压会低于设定值，这属于正常现象，脚本只提示不判失败。

运行
----
    python other_test/psu_test/dp832a_test.py
    python other_test/psu_test/dp832a_test.py --voltages 1.2,2.5,3.3 --currents 0.3,0.3,0.3
    python other_test/psu_test/dp832a_test.py --keep-on          # 测完保持输出
    python other_test/psu_test/dp832a_test.py --protect          # 同时设置 OVP / OCP

    # 指定设备（总线上有多台 VISA 仪器时）
    python other_test/psu_test/dp832a_test.py --resource "USB0::0x1AB1::0x0E11::DP8C123456789::INSTR"
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

CHANNELS = (1, 2, 3)
# 各通道电压上限（DP832A：CH1/CH2 30 V，CH3 5 V）
CH_VOLT_MAX = {1: 30.0, 2: 30.0, 3: 5.0}


def check(name, cond, detail=""):
    """打印一条检查结果，返回是否通过。"""
    print(f"   {'✅' if cond else '❌'} {name}" + (f"：{detail}" if detail else ""))
    return bool(cond)


def _parse_list(text, cast, name):
    """把 "1.2,3.3,5.0" 解析成 3 个数的列表。"""
    try:
        values = [cast(p.strip()) for p in text.split(",") if p.strip()]
    except ValueError:
        raise SystemExit(f"❌ 参数 --{name} 格式错误：{text}（应为逗号分隔的 3 个数字）")
    if len(values) != 3:
        raise SystemExit(f"❌ 参数 --{name} 需要 3 个值（对应 CH1/CH2/CH3）：{text}")
    return dict(zip(CHANNELS, values))


def _fmt(value, digits, unit):
    return "—" if value is None else f"{value:.{digits}f} {unit}"


def main():
    ap = argparse.ArgumentParser(description="RIGOL DP832A 三通道电压输出测试")
    ap.add_argument("--voltages", default="1.2,3.3,5.0",
                    help="三通道电压设定，逗号分隔（默认 1.2,3.3,5.0）")
    ap.add_argument("--currents", default="0.5,0.5,0.5",
                    help="三通道电流限值 A，逗号分隔（默认 0.5,0.5,0.5）")
    ap.add_argument("--resource", default=None, help="VISA 资源地址，默认自动搜索")
    ap.add_argument("--settle", type=float, default=2.5,
                    help="上电后等待稳定时间 s（默认 2.5；DP832A 端口电压建立约需 1~2 s）")
    ap.add_argument("--protect", action="store_true", help="同时设置 OVP / OCP")
    ap.add_argument("--keep-on", action="store_true", help="测试结束后保持输出（默认自动关断）")
    a = ap.parse_args()

    volts = _parse_list(a.voltages, float, "voltages")
    currs = _parse_list(a.currents, float, "currents")
    for ch in CHANNELS:
        if not (0.0 <= volts[ch] <= CH_VOLT_MAX[ch]):
            raise SystemExit(f"❌ CH{ch} 电压 {volts[ch]} V 超出量程（0~{CH_VOLT_MAX[ch]:g} V）")

    passed = True
    outputs_off = False
    inited = False
    try:
        # ── 1. 初始化 ──────────────────────────────────────────────
        print("=" * 78)
        print("1) 初始化")
        print("=" * 78)
        inited = instr.psu_init(resource_string=a.resource)
        passed &= check("psu_init()", inited, "仅建立连接，不复位、不改动输出")
        if not inited:
            print("   ❌ 电源初始化失败，跳过后续设定与上电步骤")
            return 1
        idn = instr.psu_get_idn()
        passed &= check("psu_get_idn()", bool(idn), str(idn))

        # 初始化不会改动通道输出，这里先记录原状态，再由脚本自己关断以取得已知起点
        prev = {ch: instr.psu_output_state(ch) for ch in CHANNELS}
        print("   初始化前的通道状态（未改动）："
              + "，".join(f"CH{ch}={'ON' if st else 'OFF' if st is False else '?'}"
                         for ch, st in prev.items()))
        passed &= check("psu_output_all_off()", instr.psu_output_all_off(),
                        "由脚本关断，建立已知起点")

        # ── 2. 设定三通道不同电压 ──────────────────────────────────
        print()
        print("=" * 78)
        print("2) 设定三通道电压 / 电流限值")
        print("=" * 78)
        for ch in CHANNELS:
            ok = instr.psu_set_channel(ch, voltage=volts[ch], current=currs[ch])
            print(f"   CH{ch}: {volts[ch]:.3f} V / 限流 {currs[ch]:.3f} A"
                  f"  {'✅' if ok else '❌ 设置失败'}")
            passed &= ok

        instr.psu_print_state("   设定回读：")

        for ch in CHANNELS:
            v_set = instr.psu_get_voltage(ch)
            i_set = instr.psu_get_current(ch)
            passed &= check(f"CH{ch} 电压设定回读 = {volts[ch]:.3f} V",
                            v_set is not None and abs(v_set - volts[ch]) <= 0.01,
                            _fmt(v_set, 3, "V"))
            passed &= check(f"CH{ch} 电流设定回读 = {currs[ch]:.3f} A",
                            i_set is not None and abs(i_set - currs[ch]) <= 0.01,
                            _fmt(i_set, 3, "A"))

        if a.protect:
            print("\n   设置 OVP / OCP（OVP = 设定电压 +20%，OCP = 限流 +10%）...")
            for ch in CHANNELS:
                ovp = min(volts[ch] * 1.2, CH_VOLT_MAX[ch] * 1.1)
                ocp = min(currs[ch] * 1.1, 3.3)
                ok = instr.psu_set_ovp(ch, voltage=round(ovp, 2), enable=True)
                ok &= instr.psu_set_ocp(ch, current=round(ocp, 2), enable=True)
                print(f"   CH{ch}: OVP {ovp:.2f} V / OCP {ocp:.2f} A {'✅' if ok else '❌'}")
                passed &= ok

        # ── 3. 三通道同时输出 ──────────────────────────────────────
        print()
        print("=" * 78)
        print("3) 打开三通道输出")
        print("=" * 78)
        for ch in CHANNELS:
            passed &= check(f"psu_output_on(CH{ch})", instr.psu_output_on(ch))
        for ch in CHANNELS:
            passed &= check(f"CH{ch} 输出回读为 ON", instr.psu_output_state(ch) is True)

        # ── 4. 回读实测值 ─────────────────────────────────────────
        print()
        print("=" * 78)
        print(f"4) 实测（等待 {a.settle:g} s 稳定）")
        print("=" * 78)
        time.sleep(a.settle)
        instr.psu_print_state("   实测：")

        print("\n   {:<4} {:>10} {:>10} {:>10} {:>10}".format(
            "通道", "设定电压", "实测电压", "实测电流", "实测功率"))
        for ch in CHANNELS:
            v, i, p = instr.psu_measure(ch)
            print("   CH{:<2} {:>9} {:>9} {:>9} {:>9}".format(
                ch, _fmt(volts[ch], 3, "V"), _fmt(v, 3, "V"),
                _fmt(i, 3, "A"), _fmt(p, 3, "W")))

            if v is None:
                passed &= check(f"CH{ch} 电压测量", False, "查询失败")
                continue

            # 电压是否稳在设定值附近；偏差大通常是负载把电流顶到限流值（CC）
            tol = max(0.05, abs(volts[ch]) * 0.02)
            if abs(v - volts[ch]) <= tol:
                print(f"        ✅ CH{ch} 电压在设定值 ±{tol:.2f} V 内")
            elif i is not None and abs(i) >= currs[ch] * 0.98:
                print(f"        ℹ️  CH{ch} 处于 CC 限流状态（电流 {i:.3f} A ≈ 限流值），"
                      f"电压被拉低属正常")
            else:
                print(f"        ⚠️  CH{ch} 实测电压 {v:.3f} V 偏离设定 {volts[ch]:.3f} V，"
                      f"请检查接线 / 负载")

        # ── 5. 关断 ───────────────────────────────────────────────
        print()
        print("=" * 78)
        print("5) 关断三通道输出" if not a.keep_on else "5) 保持输出（--keep-on）")
        print("=" * 78)
        if not a.keep_on:
            passed &= check("psu_output_all_off()", instr.psu_output_all_off())
            outputs_off = True
            for ch in CHANNELS:
                passed &= check(f"CH{ch} 输出回读为 OFF", instr.psu_output_state(ch) is False)
        else:
            print("   ℹ️  三通道仍在上电，测量完成后请手动关断（或再次运行脚本不带 --keep-on）")

    finally:
        print()
        print("=" * 78)
        print("6) 清理")
        print("=" * 78)
        if not a.keep_on and not outputs_off:
            try:
                if instr.psu_output_all_off():
                    print("   已强制关闭三通道输出")
            except Exception:
                pass
        if inited:
            try:
                instr.psu_close(output_off=not a.keep_on)
            except Exception:
                pass
            print("   已关闭电源连接" + ("" if a.keep_on else "（本脚本开启的通道已关断）"))

    print()
    print("=" * 78)
    print("全部通过 ✅" if passed else "存在失败项 ❌")
    print("=" * 78)
    return 0 if passed else 1


if __name__ == "__main__":
    sys.exit(main())
