"""
OCD5010 TRIM 值写入 OTP 工具 (4 槽位自动选择)

从 Save_Results/OCD5010/trim_best_value.xlsx 读取各修调项的最佳值，
按位组装成 VTRIM1/VTRM2 两个 32 位值，烧写到下一个可用 OTP 槽位:
  槽位1: TAG=0x8000  VTRIM1=0x8001  VTRM2=0x8002
  槽位2: TAG=0x8008  VTRIM1=0x8009  VTRM2=0x800A
  槽位3: TAG=0x8010  VTRIM1=0x8011  VTRM2=0x8012
  槽位4: TAG=0x8018  VTRIM1=0x8019  VTRM2=0x801A
每个槽位起始 TAG 写入 ANA_TAG=0x00796783，boot 才能识别并加载该槽位修调值。

烧写次数判定 (2026-08 真机确认): ODS5010 的 OTP 无读回通路 (读 OTPDR 恒为 0x0)，
因此烧写次数以 chip_program_records.xlsx 记录为准 (按芯片编号)，芯片编号由
操作员输入 (OTP UUID 可读时自动识别)。烧满 4 次后提示无法再烧写。

芯片烧写记录: Save_Results/OCD5010/chip_program_records.xlsx
  Sheet "烧写记录": 每次烧写/拒绝明细
  Sheet "芯片信息": 每颗芯片汇总

用法:
    1. 通过 main.py 菜单调用（推荐）
    2. 独立运行: python chips/OCD5010/trim/program_trim.py

参考: chips/ocd2810/trim/program_trim.py
"""

import os
import sys
import time
from pathlib import Path

# 将项目根目录加入 sys.path（确保独立运行时能找到其他模块）
_project_root = str(Path(__file__).parents[3])
if _project_root not in sys.path:
    sys.path.insert(0, _project_root)

from chips.OCD5010.trim.trim import read_trim_values, build_trim_values
from chips.OCD5010.trim.otp_slots import burn_trim_words, MAX_BURN


def ensure_swd_init():
    """
    确保 SWD 已初始化。

    使用与 instrument_set.swd_init() 相同的包路径导入 SWD_CMD，
    确保共享同一模块实例和 swd_handle。
    """
    from instrument.debug import SWD_CMD

    # read_register_32bit 内部吞掉了异常，所以要用返回值来判断
    val = SWD_CMD.read_register_32bit(0x40020058)
    if val is None:
        print("  SWD 未初始化，执行初始化...")
        SWD_CMD.init_all()
        time.sleep(0.2)
        print("  SWD 初始化完成")


def program_trim_values():
    """
    主流程:
        1. 读取 trim_best_value.xlsx
        2. 按 TRIM_FLASH_MAP 组装 VTRIM1/VTRM2
        3. 初始化 SWD
        4. 识别芯片编号 (OTP UUID 或手动输入)，以 Excel 记录判定烧写次数，
           烧写到下一可用槽位 (TAG+VTRIM1+VTRM2)，写后记录
        5. 记录芯片信息 + 烧写记录到 chip_program_records.xlsx
    """
    script_dir = os.path.dirname(os.path.abspath(__file__))

    # 定位 Save_Results/OCD5010/trim_best_value.xlsx
    xlsx_path = os.path.normpath(
        os.path.join(script_dir, "..", "..", "..", "Save_Results", "OCD5010", "trim_best_value.xlsx")
    )

    print("=" * 58)
    print("  OCD5010 TRIM 值写入 OTP (4 槽位自动选择)")
    print("=" * 58)
    print(f"  Excel 数据源:  {xlsx_path}")
    print()

    # ── 步骤 1: 读取 Excel ──
    print("[1/4] 读取修调值...")
    name_to_trim = read_trim_values(xlsx_path)
    if name_to_trim is None:
        print("❌ 无法读取 Excel 文件，终止")
        return False
    if not name_to_trim:
        print("⚠️  Excel 中未找到已 PASS 的 TRIM 值")
        print("   请先执行修调测试，确保对应测试项结果为 PASS")
        return False

    # ── 步骤 2: 按位组装 ──
    print(f"\n[2/4] 按位组装 32 位值...")
    addr_values = build_trim_values(name_to_trim)
    if not addr_values:
        print("⚠️  组装失败，无有效修调值")
        return False
    vtrim1 = addr_values.get(0x8000, 0)   # ANAC_TRM1
    vtrm2  = addr_values.get(0x8004, 0)   # ANAC_TRM2
    print(f"      ANAC_TRM1 (VTRIM1) = 0x{vtrim1:08X}")
    print(f"      ANAC_TRM2 (VTRM2)  = 0x{vtrm2:08X}")

    # ── 步骤 3: 初始化 SWD ──
    print(f"\n[3/4] 初始化 SWD 连接...")
    ensure_swd_init()
    print("  SWD 就绪")

    # ── 步骤 4: 烧写 OTP (自动判定烧写次数 + 选槽位 + 记录) ──
    print(f"\n[4/4] 烧写 OTP...")
    ok, slot_no, count = burn_trim_words(vtrim1, vtrm2)

    print(f"\n  {'=' * 58}")
    if ok:
        print(f"  ✅ 全部 TRIM 值写入 OTP 槽位 {slot_no} 完成！掉电不丢失。")
        print(f"     该芯片已烧写 {count}/{MAX_BURN} 次。复位后芯片将自动加载修调值。")
    else:
        print("  ❌ TRIM 值写入 OTP 失败/已满")
    print(f"  {'=' * 58}")
    return ok


def main():
    """供 main.py 菜单调用的入口"""
    success = program_trim_values()
    if success:
        print("\n✅ TRIM 修调值写入完成！")
    else:
        print("\n❌ TRIM 修调值写入失败")
    input("\n按回车键返回菜单...")


if __name__ == "__main__":
    success = program_trim_values()
    sys.exit(0 if success else 1)
