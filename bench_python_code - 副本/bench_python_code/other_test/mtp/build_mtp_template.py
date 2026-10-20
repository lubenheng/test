# -*- coding: utf-8 -*-
"""
把工程内的 MTP 配置表 (chips/OCD2361/mtp_config_table.py) 输出成 Excel 表格，
供程序「菜单 [32] MTP 全片擦除+烧录+校验」直接加载，也便于在 Excel 中查看/对照。

数据来源（默认）：chips/OCD2361/mtp_config_table.py
    在该文件中查看/修改 MTP_CONFIG_ROWS，运行本脚本即可重新生成表格。
    python other_test/mtp/build_mtp_template.py
    python other_test/mtp/build_mtp_template.py --from-source   # 改从原始 xlsx 读取

输入格式：MTP_CONFIG_ROWS = [(ADDR, INDEX, B31, B23, B15, B7, 模块号), ...]

输出：MTP_配置寄存器（测试版本2）_烧录模板.xlsx
      · 工作表 "MTP批量模板"：程序可直接读取（表头 2 行 + PROGRAM 数据）
        列：OPERATION / ADDR / INDEX / [31:24] / [23:16] / [15:8] / [7:0]
            / 物理地址 / 寄存器ADDR / 计入CRC
      · 工作表 "说明"：CRC、地址分布、被排除的非 PROGRAM 行
"""

import os
import sys
from pathlib import Path

import openpyxl
from openpyxl.styles import Alignment, Border, Font, PatternFill, Side
from openpyxl.utils import get_column_letter

# 与 main.py 一致：Windows 控制台默认 GBK，直接 print ✅/❌ 会抛 UnicodeEncodeError
if os.name == 'nt':
    os.system('chcp 65001 > nul')
    sys.stdout.reconfigure(encoding='utf-8')
    sys.stderr.reconfigure(encoding='utf-8')

ROOT = Path(__file__).resolve().parents[2]
SRC = ROOT / "MTP_配置寄存器（测试版本2）.xlsx"
DST = ROOT / "MTP_配置寄存器（测试版本2）_烧录模板.xlsx"

sys.path.insert(0, str(ROOT))
sys.path.insert(0, str(ROOT / "instrument" / "IIC"))
from iic_ops import IICLogic                      # noqa: E402
from crc import calculate_crc32                   # noqa: E402
from mtp_batch_ops import MTPBatchLogic           # noqa: E402

SHEET_MAIN = "MTP批量模板"
CRC_ADDR_LIMIT = 416          # CRC 覆盖 phys < 416
CRC_RSV_LO, CRC_RSV_HI = 507, 510

HEADERS = ["OPERATION", "ADDR(基地址)", "INDEX(Word偏移)",
           "[31:24]", "[23:16]", "[15:8]", "[7:0]", "物理地址(计算)", "寄存器ADDR(原表)",
           "计入CRC"]
NOTES = ["READ/PROGRAM/ERASE_SECTOR", "如 0~511 (MTP基地址)", "Word偏移(1代表偏4字节)",
         "对应物理地址 ADDR+INDEX*4 +3 (HEX)", "对应物理地址 ADDR+INDEX*4 +2 (HEX)",
         "对应物理地址 ADDR+INDEX*4 +1 (HEX)", "对应物理地址 ADDR+INDEX*4 +0 (HEX)",
         "自动公式: =B列+C列*4", "寄存器模块地址(仅供参考)",
         "Y=计入 CRC，空=只烧录（phys ≥ 416）"]

# 表格样式
HEADER_FONT = Font(bold=True, color="FFFFFF")
HEADER_FILL = PatternFill("solid", fgColor="305496")
NO_CRC_FILL = PatternFill("solid", fgColor="FFF2CC")   # phys ≥ 416：只烧录不进 CRC
MONO_FONT = Font(name="Consolas")
THIN = Side(style="thin", color="BFBFBF")
BORDER = Border(left=THIN, right=THIN, top=THIN, bottom=THIN)
CENTER = Alignment(horizontal="center", vertical="center")
COL_WIDTHS = (18, 14, 18, 10, 10, 10, 10, 16, 18, 10)


def parse_source():
    """从原始 xlsx 读取，返回 (program_rows, other_rows)。

    program_rows: [(addr, index, [b31, b23, b15, b7], mod_addr)]
    other_rows:   [(row_no, 原始单元格列表)]  —— 非 PROGRAM 行（如 WRITETHENREAD）
    """
    ws = openpyxl.load_workbook(SRC, data_only=True)[SHEET_MAIN]
    program, other = [], []
    for r in range(3, ws.max_row + 1):
        cells = [ws.cell(r, c).value for c in range(1, 11)]   # A~J，J 为「寄存器ADDR」
        if all(v is None or str(v).strip() == "" for v in cells[:3]):
            continue                      # 空行：跳过
        op = str(cells[0]).strip().upper() if cells[0] else ""
        if op != "PROGRAM":
            other.append((r, cells))
            continue
        addr = int(cells[1])
        index = int(cells[2])
        data = [IICLogic.parse_hex_byte(cells[c], f"第{r}行第{c+1}列")
                for c in range(3, 7)]
        program.append((addr, index, data, cells[9]))
    return program, other


def parse_from_module():
    """从 chips/OCD2361/mtp_config_table.py 读取数据。

    返回与 parse_source() 相同的结构。OTHER_ROWS 直接沿用。
    """
    sys.path.insert(0, str(ROOT))
    from chips.OCD2361 import mtp_config_table as cfg

    program = [(addr, index, [b31, b23, b15, b7], mod)
               for addr, index, b31, b23, b15, b7, mod in cfg.MTP_CONFIG_ROWS]
    other = [(r, [op, addr, index, *data])
             for r, op, addr, index, *data in cfg.OTHER_ROWS] if getattr(cfg, "OTHER_ROWS", None) else []
    return program, other


def compute_crc(program):
    """按烧录口径算 CRC：声明字节 ∩ phys<416，crc.py(LSB 优先)。"""
    buffer = bytearray(512)
    burn_addrs = set()
    for addr, index, data, _ in program:
        phys = addr + index * 4
        buffer[phys:phys + 4] = bytes([data[3], data[2], data[1], data[0]])
        burn_addrs.update(range(phys, phys + 4))
    burn_addrs = sorted(burn_addrs)
    crc_addrs = [a for a in burn_addrs if a < CRC_ADDR_LIMIT]
    final_crc = calculate_crc32(bytes(buffer[a] for a in crc_addrs))
    return burn_addrs, crc_addrs, final_crc


def _style_range(ws, row_from, row_to, col_from, col_to):
    """给区域加边框 + 居中。"""
    for r in range(row_from, row_to + 1):
        for c in range(col_from, col_to + 1):
            cell = ws.cell(r, c)
            cell.border = BORDER
            cell.alignment = CENTER


def build_main_sheet(wb, program, final_crc):
    """生成程序可读取的 "MTP批量模板" 工作表。"""
    ws = wb.active
    ws.title = SHEET_MAIN

    ws.append(HEADERS)
    ws.append(NOTES)
    for c in range(1, len(HEADERS) + 1):
        cell = ws.cell(1, c)
        cell.font = HEADER_FONT
        cell.fill = HEADER_FILL
        cell.alignment = CENTER
        cell.border = BORDER
        ws.cell(2, c).font = Font(italic=True, color="808080")
        ws.cell(2, c).alignment = CENTER
        ws.cell(2, c).border = BORDER

    row = 3
    prev_addr = None
    for addr, index, data, mod in program:
        phys = addr + index * 4
        in_crc = phys < CRC_ADDR_LIMIT
        ws.append(["PROGRAM", addr, index,
                   f"{data[0]:02X}", f"{data[1]:02X}", f"{data[2]:02X}", f"{data[3]:02X}",
                   phys, mod, "Y" if in_crc else ""])
        for c in range(1, len(HEADERS) + 1):
            cell = ws.cell(row, c)
            cell.border = BORDER
            cell.alignment = CENTER
            if c in (4, 5, 6, 7):
                cell.font = MONO_FONT
            if not in_crc:
                cell.fill = NO_CRC_FILL
        # 不同 ADDR 之间加一条上边框，视觉分块
        if prev_addr is not None and addr != prev_addr:
            for c in range(1, len(HEADERS) + 1):
                cur = ws.cell(row, c)
                cur.border = Border(left=THIN, right=THIN, bottom=THIN,
                                    top=Side(style="medium", color="000000"))
        prev_addr = addr
        row += 1

    # CRC 行：仅作记录，解析时因 OPERATION != PROGRAM 被忽略
    ws.append(["PROGRAM(CRC)", 507, 0,
               f"{(final_crc >> 24) & 0xFF:02X}", f"{(final_crc >> 16) & 0xFF:02X}",
               f"{(final_crc >> 8) & 0xFF:02X}", f"{final_crc & 0xFF:02X}",
               507, "自动生成", ""])
    for c in range(1, len(HEADERS) + 1):
        cell = ws.cell(row, c)
        cell.border = BORDER
        cell.alignment = CENTER
        cell.font = Font(bold=True, color="C00000")
    for c in (4, 5, 6, 7):
        ws.cell(row, c).font = Font(bold=True, color="C00000", name="Consolas")

    for i, width in enumerate(COL_WIDTHS, start=1):
        ws.column_dimensions[get_column_letter(i)].width = width

    ws.freeze_panes = "A3"                 # 固定表头两行
    ws.auto_filter.ref = f"A1:{get_column_letter(len(HEADERS))}{row - 1}"


def build_info_sheet(wb, program, other, burn_addrs, crc_addrs, final_crc, origin):
    """生成 "说明" 工作表。"""
    info = wb.create_sheet("说明")
    info.append(["项目", "内容"])
    info.cell(1, 1).font = info.cell(1, 2).font = HEADER_FONT
    info.cell(1, 1).fill = info.cell(1, 2).fill = HEADER_FILL
    for k, v in [
        ("数据来源", origin),
        ("PROGRAM 行数", len(program)),
        ("烧录字节数", f"{len(burn_addrs)}（地址 {burn_addrs[0]}~{burn_addrs[-1]}）"),
        ("CRC 覆盖字节数", f"{len(crc_addrs)}（地址 {crc_addrs[0]}~{crc_addrs[-1]}，phys<416）"),
        ("CRC32", f"0x{final_crc:08X}"),
        ("CRC 存放", f"地址 {CRC_RSV_LO}~{CRC_RSV_HI} 小端（由烧录流程自动写入）"),
        ("地址映射", "phys = ADDR + INDEX*4"),
        ("数值格式", "D~G 列统一为 2 位 HEX"),
        ("黄色行", "phys ≥ 416：只烧录，不计入 CRC"),
    ]:
        info.append([k, v])

    # 地址块分布
    info.append([])
    info.append(["地址块分布"])
    info.cell(info.max_row, 1).font = Font(bold=True)
    info.append(["ADDR", "INDEX 范围", "字数", "物理地址范围", "计入CRC"])
    blocks = {}
    for addr, index, _data, _mod in program:
        blocks.setdefault(addr, []).append(index)
    for addr in sorted(blocks):
        idxs = sorted(blocks[addr])
        start, end = addr + idxs[0] * 4, addr + idxs[-1] * 4
        info.append([f"0x{addr:02X}", f"{idxs[0]}~{idxs[-1]}", len(idxs),
                     f"{start}~{end}", "Y" if start < CRC_ADDR_LIMIT else ""])

    # 被排除的行
    if other:
        info.append([])
        info.append(["非 PROGRAM 行（未烧录，保留供参考）"])
        info.cell(info.max_row, 1).font = Font(bold=True)
        info.append(["源行号", "OPERATION", "ADDR", "INDEX", "[31:24]", "[23:16]", "[15:8]", "[7:0]"])
        for r, cells in other:
            info.append([r] + [cells[i] for i in range(7)])

    for i, width in enumerate((16, 22, 10, 18, 10), start=1):
        info.column_dimensions[get_column_letter(i)].width = width
    _style_range(info, 2, info.max_row, 1, 2)


def main():
    from_source = "--from-source" in sys.argv
    program, other = parse_source() if from_source else parse_from_module()
    origin = SRC.name if from_source else "chips/OCD2361/mtp_config_table.py"
    print(f"数据来源：{origin}")
    print(f"PROGRAM 行：{len(program)}，其它行：{len(other)}")

    burn_addrs, crc_addrs, final_crc = compute_crc(program)
    if not burn_addrs:
        print("❌ 没有可烧录的数据行")
        return 1
    print(f"烧录字节：{len(burn_addrs)}（{burn_addrs[0]}~{burn_addrs[-1]}）")
    print(f"CRC 覆盖：{len(crc_addrs)} 字节（{crc_addrs[0]}~{crc_addrs[-1]}），"
          f"CRC32 = 0x{final_crc:08X}")

    wb = openpyxl.Workbook()
    build_main_sheet(wb, program, final_crc)
    build_info_sheet(wb, program, other, burn_addrs, crc_addrs, final_crc, origin)
    try:
        wb.save(DST)
    except PermissionError:
        print(f"❌ 无法写入（文件正被 Excel 等程序占用，请先关闭）：{DST.name}")
        return 1
    print(f"✅ 已生成：{DST}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
