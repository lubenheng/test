# -*- coding: utf-8 -*-
"""
万用表 / 示波器 / 源表 初始化测试脚本

按 OCD2361 芯片 init_all() 的顺序初始化三台仪器，逐台打印结果，
最后统一释放连接。用于新工位接线后的冒烟验证。

流程：
    万用表 (Keysight 34465A) → 示波器 (TBS 2204B) → 源表 (KEITHLEY 2460)
    每台仪器初始化后查询一次身份/状态；全部完成后再统一关闭。

注意：
    源表初始化后输出保持关闭（安全状态），本脚本不会打开输出。

运行：
    python other_test/init_test/instrument_init_test.py
"""

import os
import sys
from pathlib import Path

# 与 main.py 一致：Windows 控制台默认 GBK，直接 print ✅/❌ 会抛 UnicodeEncodeError
if os.name == 'nt':
    os.system('chcp 65001 > nul')
    sys.stdout.reconfigure(encoding='utf-8')
    sys.stderr.reconfigure(encoding='utf-8')

# 仓库根目录（bench_python_code/）加入 sys.path，支持在任意目录运行
ROOT = Path(__file__).resolve().parents[2]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from instrument import instrument_set as instr

# 示波器初始化参数（与 chip.init_all() 的默认值一致）
OSC_PROBE_ATTENUATION = 10
OSC_VERTICAL_SCALE = 5.0
OSC_HORIZONTAL_SCALE = 1e-6


def test_dmm():
    """初始化万用表，并读一次电压确认通信正常。"""
    print("\n[1/3] 万用表 (Keysight 34465A)")
    print("-" * 50)
    if not instr.dmm_init():
        print("❌ 万用表初始化失败")
        return False

    mv = instr.dmm_measure_single_voltage(voltage_type="DC", range_val="10V")
    if mv is None:
        print("⚠️  万用表初始化成功，但读取电压失败")
        return False
    print(f"✅ 万用表读数正常：DC {mv:.3f} mV")
    return True


def test_osc():
    """初始化示波器，并读一次 CH1 测量值确认通信正常。"""
    print("\n[2/3] 示波器 (TBS 2204B)")
    print("-" * 50)
    if not instr.osc_init(probe_attenuation=OSC_PROBE_ATTENUATION,
                          vertical_scale=OSC_VERTICAL_SCALE,
                          horizontal_scale=OSC_HORIZONTAL_SCALE):
        print("❌ 示波器初始化失败")
        return False

    freq, period, duty = instr.osc_measure_frequency("CH1")
    if freq is None:
        print("⚠️  示波器初始化成功，但 CH1 无有效波形（测量值为无效值）")
        return True  # 通信已通，无信号属正常情况
    print(f"✅ 示波器读数正常：CH1 {freq:.3f} Hz / {period:.3f} us / {duty:.2f} %")
    return True


def test_smu():
    """初始化源表，查询 IDN 与输出状态确认通信正常。"""
    print("\n[3/3] 源表 (KEITHLEY 2460)")
    print("-" * 50)
    if not instr.smu_init():
        print("❌ 源表初始化失败")
        return False

    idn = instr.smu_get_idn()
    out_on = instr.smu_output_state()
    print(f"✅ 源表初始化成功：{idn}")
    print(f"   输出状态：{'ON' if out_on else 'OFF'}（初始化后应为 OFF）")
    return out_on is False


def main():
    print("=" * 50)
    print("仪器初始化测试：万用表 / 示波器 / 源表")
    print("=" * 50)

    results = {}
    try:
        results["万用表"] = test_dmm()
        results["示波器"] = test_osc()
        results["源表"] = test_smu()
    finally:
        print("\n" + "=" * 50)
        instr.smu_close()
        instr.osc_close()
        instr.dmm_release()
        print("=" * 50)

    print("\n初始化结果汇总：")
    for name, ok in results.items():
        print(f"  {'✅' if ok else '❌'} {name}")

    return all(results.values())


if __name__ == "__main__":
    sys.exit(0 if main() else 1)
