"""
OCD5010 TRIM 值写入 OTP 工具 (4 槽位自动选择)

从 Save_Results/OCD5010/trim_best_value.xlsx 读取各修调项的最佳值，
按位组装成 VTRIM1/VTRM2 两个 32 位值，烧写到下一个可用 OTP 槽位:
  槽位1: TAG=0x8000  VTRIM1=0x8001  VTRM2=0x8002
  槽位2: TAG=0x8008  VTRIM1=0x8009  VTRM2=0x800A
  槽位3: TAG=0x8010  VTRIM1=0x8011  VTRM2=0x8012
  槽位4: TAG=0x8018  VTRIM1=0x8019  VTRM2=0x801A
每个槽位起始 TAG 写入 ANA_TAG=0x00796783，boot 才能识别并加载该槽位修调值。

烧写次数判定 (2026-08 修正): OTP **无寄存器级读回** (DR=写锁存), 槽位占用
以烧写记录 Excel 中 "无编号" 的 PASS 次数为准 (见 otp_slots.burn_trim_words)。
★ OTP 是 0→1 编程 (反熔丝型), 空白默认 0x0, 不是 0xFFFFFFFF! 该芯片无编号，
不做芯片识别。烧满 4 次后提示无法再烧写。

[临时功能 2026-08] 未测到的修调字段 (未测量/测试失败/暂不测试, 如 BG_TTRM)
组装时为 0; 烧 0 会覆盖芯片当前原值。故对**不在 name_to_trim 中**的字段,
从 ANAC 影子寄存器 (0x40007C20/0x40007C24) 读回当前值合并进烧写值。
可用 MERGE_UNMEASURED_FROM_REG 开关关闭。

芯片烧写记录: Save_Results/OCD5010/chip_program_records.xlsx
  Sheet "烧写记录": 每次烧写/拒绝明细 (芯片 UID 记 "无编号"，仅作时间线归档)

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

from chips.OCD5010.trim.trim import read_trim_values, build_trim_values, TRIM_FLASH_MAP, ADDR_NAME
from chips.OCD5010.trim.otp_slots import burn_trim_words, MAX_BURN

# [临时] 烧写时把未测到的修调字段从 ANAC 影子寄存器读回原值并合并。
# 测量完善后置 False 即可关闭该临时行为。
MERGE_UNMEASURED_FROM_REG = True


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


def merge_unmeasured_fields(vtrim1, vtrm2, name_to_trim):
    """
    [临时功能] 将未测到的修调字段从 ANAC 影子寄存器读回原值并合并。

    背景: 某些 trim 项未测量(或测试失败/暂不测试, 如 BG_TTRM)时, read_trim_values
    不会把它们收进来, 组装后这些字段的 bit 为 0。直接烧 0 会把芯片当前正在用的
    原值覆盖掉。本函数对 **不在 name_to_trim 中** 的字段 (即未测到、组装为 0 的),
    读取对应 ANAC 影子寄存器 (0x40007C20=VTRIM1 / 0x40007C24=VTRM2) 当前值,
    把该字段的 bit 合并进待烧写的 32 位值。

    说明:
      - 已测到 (PASS, 含实测值为 0) 的字段一律保留测量值, 不合并。
      - 读寄存器失败时该字段保持 0 并告警。
      - 临时功能, 由模块顶部 MERGE_UNMEASURED_FROM_REG 控制, 后续可移除。

    参数:
        vtrim1: 组装后的 ANAC_TRM1 值 (0x8000)
        vtrm2:  组装后的 ANAC_TRM2 值 (0x8004)
        name_to_trim: read_trim_values 返回的 {字段名: 测量值}
    返回:
        (新vtrim1, 新vtrm2, 是否发生合并,
         合并明细列表 merged_fields: [(字段名, 寄存器地址名, bit起始, bit长度, 寄存器读回值), ...])
    """
    from chips.OCD5010.trim.otp_slots import ANAC_TRM1_ADDR, ANAC_TRM2_ADDR
    from instrument.debug import SWD_CMD

    addr_to_shadow = {0x8000: ANAC_TRM1_ADDR, 0x8004: ANAC_TRM2_ADDR}
    words = {0x8000: vtrim1, 0x8004: vtrm2}
    changed = False
    merged_fields = []   # 记录哪些字段来自寄存器合并 (用于烧写前明细)

    for reg_name, (addr, bit_start, bit_len) in TRIM_FLASH_MAP.items():
        if reg_name in name_to_trim:
            continue  # 已测到, 保留测量值 (即使是 0)

        shadow_addr = addr_to_shadow[addr]
        reg_val = SWD_CMD.read_register_32bit(shadow_addr)
        if reg_val is None:
            print(f"  ⚠️ 读取 {ADDR_NAME[addr]} 影子寄存器失败, {reg_name} 保持 0")
            continue

        field_val = (reg_val >> bit_start) & ((1 << bit_len) - 1)
        mask = ((1 << bit_len) - 1) << bit_start
        words[addr] = (words[addr] & ~mask) | ((field_val & ((1 << bit_len) - 1)) << bit_start)
        changed = True
        merged_fields.append((reg_name, ADDR_NAME[addr], bit_start, bit_len, field_val))
        print(f"  🔄 {reg_name:20s} 未测到 → 从 {ADDR_NAME[addr]} 寄存器读回 {field_val} (0x{field_val:02X}), "
              f"已合并进 bit{bit_start}~{bit_start + bit_len - 1}")

    return words[0x8000], words[0x8004], changed, merged_fields


def program_trim_values():
    """
    主流程:
        1. 读取 trim_best_value.xlsx
        2. 按 TRIM_FLASH_MAP 组装 VTRIM1/VTRM2
        3. 初始化 SWD
        3.5 [临时] 未测到的字段从 ANAC 影子寄存器读回原值合并 (MERGE_UNMEASURED_FROM_REG)
        4. 依据烧写记录 Excel 的 PASS 次数选下一个槽位 (该芯片无编号, 不做芯片识别)，
           确认后烧写 TAG+VTRIM1+VTRM2
        5. 仅烧写成功时追加一条烧写记录到 chip_program_records.xlsx (芯片 UID 记 "无编号")
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

    # ── 步骤 3.5 (临时): 未测到的字段从 ANAC 影子寄存器读回原值合并 ──
    if MERGE_UNMEASURED_FROM_REG:
        print("\n  [临时] 合并未测到的修调字段 (从 ANAC 影子寄存器读回原值)...")
        vtrim1, vtrm2, merged, merged_fields = merge_unmeasured_fields(vtrim1, vtrm2, name_to_trim)
        if merged:
            print(f"  ✅ 合并后: ANAC_TRM1 (VTRIM1) = 0x{vtrim1:08X}")
            print(f"            ANAC_TRM2 (VTRM2)  = 0x{vtrm2:08X}")

            # 烧写确认前打印合并字段明细
            print("\n  ── 本次烧写由寄存器合并的字段明细 (未测到 → 保留芯片原值) ──")
            print("     字段             寄存器原值       → 合并进")
            for reg_name, addr_name, bit_start, bit_len, field_val in merged_fields:
                print(f"     {reg_name:20s} {field_val:3d} (0x{field_val:02X})    {addr_name} bit{bit_start}~{bit_start + bit_len - 1}")
            print("  ─────────────────────────────────────────────────────────")
            print("     以上字段未测到, 取芯片当前寄存器原值合并烧写。")
        else:
            print("  ℹ️ 所有字段均已测到, 无需合并")
    else:
        print("  ⏭️ 合并功能已关闭 (MERGE_UNMEASURED_FROM_REG=False)")

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
