"""
OCD5010 芯片烧写记录 Excel

文件: Save_Results/OCD5010/chip_program_records.xlsx
  Sheet "烧写记录": 每次烧写/拒绝的明细 (追加行)
  Sheet "芯片信息": 每颗芯片的汇总 (按芯片编号 upsert)

★ ODS5010 的 OTP 无读回通路 (真机确认), 烧写次数无法从芯片读出。
  因此烧写次数以本 Excel 记录为准, 芯片编号由操作员输入 (OTP UUID 可读时自动识别)。
"""

import os
from datetime import datetime

from openpyxl import Workbook, load_workbook

# Save_Results/OCD5010/ 目录
SAVE_DIR = os.path.normpath(
    os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "Save_Results", "OCD5010")
)
RECORDS_FILE = os.path.join(SAVE_DIR, "chip_program_records.xlsx")

RECORDS_HEADER = ["时间", "芯片UID", "结果", "已烧写次数", "本次烧写次数", "OTP槽位",
                  "TAG地址", "VTRIM1地址", "VTRIM1值", "VTRM2地址", "VTRM2值"]
CHIP_HEADER = ["芯片UID", "已烧写次数", "最近烧写时间", "状态"]


def _ensure_workbook():
    """打开或创建记录 Excel, 返回 Workbook。"""
    os.makedirs(SAVE_DIR, exist_ok=True)
    if os.path.exists(RECORDS_FILE):
        return load_workbook(RECORDS_FILE)
    wb = Workbook()
    wb.active.title = "烧写记录"
    wb.active.append(RECORDS_HEADER)
    wb.create_sheet("芯片信息").append(CHIP_HEADER)
    return wb


def _now():
    return datetime.now().strftime("%Y-%m-%d %H:%M:%S")


def append_burn_record(chip_uid, result, used_count, burn_no, slot, vtrim1, vtrm2):
    """
    追加一行烧写记录。

    参数:
        chip_uid: 芯片 UUID 字符串 (或 None)
        result: "PASS" / "FAIL" / "已烧满拒绝"
        used_count: 本次操作后芯片已烧写次数
        burn_no: 本次烧写是第几次 (失败/拒绝时可为 None)
        slot: (槽位号, tag_addr, vtrim1_addr, vtrm2_addr) 或 None
        vtrim1 / vtrm2: 打包后的 32 位值
    """
    wb = _ensure_workbook()
    ws = wb["烧写记录"]

    if slot:
        slot_no, tag_addr, v1_addr, v2_addr = slot
    else:
        slot_no = tag_addr = v1_addr = v2_addr = None

    row = [
        _now(),
        chip_uid if chip_uid else "未知",
        result,
        used_count if used_count is not None else "",
        burn_no if burn_no is not None else "",
        slot_no if slot_no is not None else "",
        f"0x{tag_addr:04X}" if tag_addr is not None else "",
        f"0x{v1_addr:04X}" if v1_addr is not None else "",
        f"0x{vtrim1:08X}" if vtrim1 is not None else "",
        f"0x{v2_addr:04X}" if v2_addr is not None else "",
        f"0x{vtrm2:08X}" if vtrm2 is not None else "",
    ]
    ws.append(row)
    wb.save(RECORDS_FILE)
    print(f"  📝 已记录到 {os.path.basename(RECORDS_FILE)} (烧写记录)")


def get_chip_burn_count(chip_id):
    """
    从"芯片信息"表查询某芯片的已烧写次数 (烧写次数以记录为准)。

    参数:
        chip_id: 芯片编号 / UUID 字符串

    返回: 已烧写次数 (0~4)；无该芯片记录时返回 0。
    """
    if not chip_id:
        return 0
    wb = _ensure_workbook()
    ws = wb["芯片信息"]
    for row in ws.iter_rows(min_row=2, values_only=True):
        if row[0] == chip_id:
            try:
                return int(row[1]) if row[1] is not None else 0
            except (ValueError, TypeError):
                return 0
    return 0


def upsert_chip_info(chip_uid, used_count):
    """按芯片UID upsert 一行芯片汇总信息 (UID / 已烧写次数 / 最近时间 / 状态)。"""
    if not chip_uid:
        return
    wb = _ensure_workbook()
    ws = wb["芯片信息"]
    ts = _now()
    status = "可继续烧写" if used_count < 4 else "已烧满"

    for row in ws.iter_rows(min_row=2):
        if row[0].value == chip_uid:
            row[1].value = used_count
            row[2].value = ts
            row[3].value = status
            break
    else:
        ws.append([chip_uid, used_count, ts, status])
    wb.save(RECORDS_FILE)
    print(f"  📝 已更新 {os.path.basename(RECORDS_FILE)} (芯片信息)")


if __name__ == "__main__":
    # 自测: 生成一个示例记录
    print(f"记录文件: {RECORDS_FILE}")
    append_burn_record("ABCDEF0123456789ABCDEF0123456789", "PASS", 1, 1,
                       (1, 0x8000, 0x8001, 0x8002), 0x00000000, 0x00000000)
    upsert_chip_info("ABCDEF0123456789ABCDEF0123456789", 1)
    print("自测完成")
