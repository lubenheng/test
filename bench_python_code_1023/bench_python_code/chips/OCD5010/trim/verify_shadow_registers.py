"""
OCD5010 烧写生效验证 —— reboot 后读影子寄存器与记录比对

背景: ODS5010 的 OTP 读回 (FLC_DR) 直接读可能仍读空白, 最可靠的生效验证
      是 reboot 后读影子寄存器。OTP 值在芯片 reboot 后由 boot 加载到:
        VTRIM1    → ANAC_TRM1   (0x40007C20)
        VTRM2     → ANAC_TRM2   (0x40007C24)
        ANACINFO0 → ANAC INFO_0 (0x40007C28)
        ANACINFO1 → ANAC INFO_1 (0x40007C2C)
        ANACINFO2 → ANAC INFO_2 (0x40007C30)
        ANACINFO3 → ANAC INFO_3 (0x40007C34)
      ★ ADCTRM 回读地址待确认, 不在验证范围 (只写不验)。
      本脚本读这些寄存器, 与 chip_program_records.xlsx 中该芯片最近一次
      PASS 烧写记录的对应值比对, 确认烧写已生效。
      ★ 该芯片无编号, 烧写记录芯片 UID 记 "无编号"。

用法 (需 J-Link 连接, 请用 UTF-8 控制台运行):
    python chips/OCD5010/trim/verify_shadow_registers.py [芯片UID]
    不带编号时交互式输入 (烧写记录 UID 为 "无编号")。

注意: 必须先完成 reboot (断电重上电 / 通过 reboot_and_verify 触发 SWD 复位),
      再运行本脚本。
"""

import os
import sys
import time
from pathlib import Path

_project_root = str(Path(__file__).parents[3])
if _project_root not in sys.path:
    sys.path.insert(0, _project_root)

from chips.OCD5010.trim.otp_slots import (
    ANAC_TRM1_ADDR, ANAC_TRM2_ADDR,
    ANAC_INFO0_ADDR, ANAC_INFO1_ADDR, ANAC_INFO2_ADDR, ANAC_INFO3_ADDR,
    word_to_float,
)
from instrument.debug import SWD_CMD

RECORDS_FILE = os.path.normpath(
    os.path.join(_project_root, "Save_Results", "OCD5010", "chip_program_records.xlsx")
)


def _last_pass_trim_values(chip_id):
    """从烧写记录表找该芯片最近一次 PASS 的 VTRIM1/VTRM2/ADCTRM/ANACINFO0~3。
    返回 dict {VTRIM1, VTRM2, ADCTRM, ANACINFO0..3} (旧记录缺列时为 None) 或 None。"""
    import openpyxl
    if not os.path.exists(RECORDS_FILE):
        return None
    wb = openpyxl.load_workbook(RECORDS_FILE, read_only=True)
    ws = wb["烧写记录"]
    # 表头: 时间/芯片UID/结果/已烧写次数/本次烧写次数/OTP槽位/TAG地址/
    #        VTRIM1地址/VTRIM1值/VTRM2地址/VTRM2值/
    #        ADCTRM地址/ADCTRM值/ANACINFO0地址/ANACINFO0值/... (2026-09 起 21 列)
    best = None
    for row in ws.iter_rows(min_row=2, values_only=True):
        if len(row) < 11:
            continue
        if row[1] != chip_id or row[2] != "PASS":
            continue
        # 取最大"已烧写次数"的那条 (最近一次)
        try:
            n = int(row[3])
        except (ValueError, TypeError):
            continue
        if best is None or n > best[0]:
            best = (n, row)   # 已烧写次数, 整行
    wb.close()
    if best is None:
        return None
    row = best[1]

    def _hex(i):
        try:
            v = row[i] if i < len(row) else None
            return int(str(v), 16) if v not in (None, "") else None
        except (ValueError, TypeError):
            return None

    return {
        "VTRIM1":    _hex(8),
        "VTRM2":     _hex(10),
        "ADCTRM":    _hex(12),
        "ANACINFO0": _hex(14),
        "ANACINFO1": _hex(16),
        "ANACINFO2": _hex(18),
        "ANACINFO3": _hex(20),
    }


def main():
    chip_id = sys.argv[1] if len(sys.argv) > 1 else None

    print("=" * 58)
    print("  OCD5010 烧写生效验证 (读影子寄存器)")
    print("=" * 58)
    print("  请确认芯片已 reboot (断电重上电或固件复位) 后再运行本脚本。")
    print()

    if not chip_id:
        chip_id = input("  芯片UID (烧写记录为 \"无编号\", 直接回车用默认): ").strip()
        if not chip_id:
            chip_id = "无编号"

    expected = _last_pass_trim_values(chip_id)
    if expected is None:
        print(f"❌ 记录中未找到芯片 [{chip_id}] 的 PASS 烧写记录，无法比对")
        return

    if expected["VTRIM1"] is not None and expected["VTRM2"] is not None:
        print(f"\n  芯片 [{chip_id}] 最近一次 PASS 烧写: "
              f"VTRIM1=0x{expected['VTRIM1']:08X} VTRM2=0x{expected['VTRM2']:08X}")
    else:
        print(f"\n  芯片 [{chip_id}] 最近一次 PASS 烧写: 记录中 VTRIM1/VTRM2 缺失 (旧记录)")
    print("  初始化 SWD...")
    SWD_CMD.init_all()
    time.sleep(0.3)

    # 验证对象: VTRIM1/2 + ANACINFO0~3 (★ ADCTRM 回读地址待确认, 不在验证范围)
    regs = [
        ("ANAC_TRM1", ANAC_TRM1_ADDR,  "VTRIM1"),
        ("ANAC_TRM2", ANAC_TRM2_ADDR,  "VTRM2"),
        ("ANACINFO0", ANAC_INFO0_ADDR, "ANACINFO0"),
        ("ANACINFO1", ANAC_INFO1_ADDR, "ANACINFO1"),
        ("ANACINFO2", ANAC_INFO2_ADDR, "ANACINFO2"),
        ("ANACINFO3", ANAC_INFO3_ADDR, "ANACINFO3"),
    ]
    all_ok = True
    print()
    for name, addr, key in regs:
        expected_val = expected.get(key)
        val = SWD_CMD.read_register_32bit(addr)
        line = f"  {name:10s} (0x{addr:08X}) = {hex(val) if val is not None else 'None'}"
        extra = ""
        if key == "ANACINFO0" and val is not None:
            extra = f"   (VREF ≈ {word_to_float(val):.1f} mV)"
        if expected_val is not None:
            match = (val == expected_val)
            all_ok = all_ok and match
            print(f"{line}  (期望 0x{expected_val:08X}) {'✅' if match else '❌'}{extra}")
        else:
            print(f"{line}  (记录中无此值, 无法比对){extra}")

    print("\n  ── 结论 ──")
    if all_ok:
        print("  ✅ 影子寄存器与写入值一致 → OTP 烧写已生效!")
    else:
        print("  ⚠️  存在不一致 → 可能: 尚未 reboot / 区域被更早烧写覆盖 / SWD 未重连")
        print("     若首次烧写(区域1), 请确认已断电重上电后重试")


if __name__ == "__main__":
    main()
    input("\n按回车键退出...")
