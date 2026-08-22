"""
OCD5010 烧写生效验证 —— reboot 后读 ANAC_TRM1/2 影子寄存器与记录比对

背景: ODS5010 的 OTP 无读回通路, 烧写后无法直接回读 OTP。OTP 值在芯片
      reboot (断电重上电 / 固件 RCC->RSTKEY=0xAB56 复位) 后由 boot 加载到
      影子寄存器:
        VTRIM1 → ANAC_TRM1 (0x40007C20)
        VTRM2  → ANAC_TRM2 (0x40007C24)
      本脚本读这两个寄存器, 与 chip_program_records.xlsx 中该芯片最近一次
      PASS 烧写记录的 VTRIM1/VTRM2 比对, 确认烧写已生效。

用法 (需 J-Link 连接, 请用 UTF-8 控制台运行):
    python chips/OCD5010/trim/verify_shadow_registers.py [芯片编号]
    不带编号时交互式输入。

注意: 必须先完成 reboot (断电重上电), 再运行本脚本。
"""

import os
import sys
import time
from pathlib import Path

_project_root = str(Path(__file__).parents[3])
if _project_root not in sys.path:
    sys.path.insert(0, _project_root)

from chips.OCD5010.trim.otp_slots import ANAC_TRM1_ADDR, ANAC_TRM2_ADDR
from instrument.debug import SWD_CMD

RECORDS_FILE = os.path.normpath(
    os.path.join(_project_root, "Save_Results", "OCD5010", "chip_program_records.xlsx")
)


def _last_pass_trim_values(chip_id):
    """从烧写记录表找该芯片最近一次 PASS 的 VTRIM1/VTRM2。返回 (v1, v2) 或 None。"""
    import openpyxl
    if not os.path.exists(RECORDS_FILE):
        return None
    wb = openpyxl.load_workbook(RECORDS_FILE, read_only=True)
    ws = wb["烧写记录"]
    # 表头: 时间/芯片UID/结果/已烧写次数/本次烧写次数/OTP槽位/TAG地址/
    #        VTRIM1地址/VTRIM1值/VTRM2地址/VTRM2值
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
            best = (n, row[8], row[10])   # 已烧写次数, VTRIM1值, VTRM2值
    wb.close()
    if best is None:
        return None
    try:
        return (int(str(best[1]), 16), int(str(best[2]), 16))
    except ValueError:
        return None


def main():
    chip_id = sys.argv[1] if len(sys.argv) > 1 else None

    print("=" * 58)
    print("  OCD5010 烧写生效验证 (读影子寄存器)")
    print("=" * 58)
    print("  请确认芯片已 reboot (断电重上电或固件复位) 后再运行本脚本。")
    print()

    if not chip_id:
        chip_id = input("  芯片编号 (与烧写时输入一致): ").strip()

    expected = _last_pass_trim_values(chip_id)
    if expected is None:
        print(f"❌ 记录中未找到芯片 [{chip_id}] 的 PASS 烧写记录，无法比对")
        return

    print(f"\n  芯片 [{chip_id}] 最近一次 PASS 烧写: VTRIM1=0x{expected[0]:08X} VTRM2=0x{expected[1]:08X}")
    print("  初始化 SWD...")
    SWD_CMD.init_all()
    time.sleep(0.3)

    v1 = SWD_CMD.read_register_32bit(ANAC_TRM1_ADDR)
    v2 = SWD_CMD.read_register_32bit(ANAC_TRM2_ADDR)
    print(f"\n  ANAC_TRM1 (0x{ANAC_TRM1_ADDR:08X}) = {hex(v1) if v1 is not None else 'None'}")
    print(f"  ANAC_TRM2 (0x{ANAC_TRM2_ADDR:08X}) = {hex(v2) if v2 is not None else 'None'}")

    ok1 = v1 is not None and v1 == expected[0]
    ok2 = v2 is not None and v2 == expected[1]
    print("\n  ── 结论 ──")
    if ok1 and ok2:
        print("  ✅ ANAC_TRM1/TRM2 与写入值一致 → OTP 烧写已生效!")
    elif ok1:
        print("  ⚠️  VTRIM1 一致但 VTRM2 不一致 → 可能槽位值被更早烧写覆盖, 或读取异常")
    elif ok2:
        print("  ⚠️  VTRM2 一致但 VTRIM1 不一致 → 同上")
    else:
        print("  ⚠️  与写入值不一致 → 可能: 尚未 reboot / 槽位被更早烧写覆盖 / SWD 未重连")
        print("     若首次烧写(槽位1), 请确认已断电重上电后重试")


if __name__ == "__main__":
    main()
    input("\n按回车键退出...")
