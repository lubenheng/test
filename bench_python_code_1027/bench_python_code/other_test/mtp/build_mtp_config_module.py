# -*- coding: utf-8 -*-
"""
把 MTP 配置寄存器表（.xlsx）转换成工程内可直接查看/编辑的 Python 数据模块。

源表：MTP_配置寄存器（测试版本2）.xlsx
输出：chips/OCD2361/mtp_config_table.py

生成的内容是一个普通的 Python 列表，可在 IDE 里直接查看、搜索、diff、修改；
修改后运行本脚本可重新校验 CRC，或直接在菜单 [32] 使用配套的 Excel 模板。

运行：
    python other_test/mtp/build_mtp_config_module.py
"""

import os
import sys
from pathlib import Path

import openpyxl

if os.name == 'nt':
    os.system('chcp 65001 > nul')
    sys.stdout.reconfigure(encoding='utf-8')
    sys.stderr.reconfigure(encoding='utf-8')

ROOT = Path(__file__).resolve().parents[2]
SRC = ROOT / "MTP_配置寄存器（测试版本2）.xlsx"
DST = ROOT / "chips" / "OCD2361" / "mtp_config_table.py"

sys.path.insert(0, str(ROOT))
sys.path.insert(0, str(ROOT / "instrument" / "IIC"))
from iic_ops import IICLogic                      # noqa: E402
from crc import calculate_crc32                   # noqa: E402

SHEET = "MTP批量模板"
CRC_ADDR_LIMIT = 416          # CRC 覆盖 phys < 416
CRC_RSV_LO, CRC_RSV_HI = 507, 510


def parse_source():
    """读取源表的 PROGRAM 行，返回 (rows, others)。

    rows:   [(addr, index, b31, b23, b15, b7, module)]
    others: [(row_no, operation, [7 个单元格])] —— 非 PROGRAM 行
    """
    ws = openpyxl.load_workbook(SRC, data_only=True)[SHEET]
    rows, others = [], []
    for r in range(3, ws.max_row + 1):
        cells = [ws.cell(r, c).value for c in range(1, 11)]
        if all(v is None or str(v).strip() == "" for v in cells[:3]):
            continue
        op = str(cells[0]).strip().upper() if cells[0] else ""
        if op != "PROGRAM":
            others.append((r, op, cells[:7]))
            continue
        addr = int(cells[1])
        index = int(cells[2])
        data = [IICLogic.parse_hex_byte(cells[c], f"第{r}行第{c+1}列")
                for c in range(3, 7)]
        rows.append((addr, index, *data, cells[9]))
    return rows, others


def main():
    rows, others = parse_source()

    # 与烧录流程同口径：声明字节 ∩ phys<416，按地址升序
    addrs = sorted({addr + index * 4 + k
                    for addr, index, *_ in rows for k in range(4)})
    crc_addrs = [a for a in addrs if a < CRC_ADDR_LIMIT]
    buf = bytearray(512)
    for addr, index, b31, b23, b15, b7, _ in rows:
        phys = addr + index * 4
        buf[phys:phys + 4] = bytes([b7, b15, b23, b31])
    final_crc = calculate_crc32(bytes(buf[a] for a in crc_addrs))

    lines = [
        '# -*- coding: utf-8 -*-',
        '"""',
        'MTP 配置寄存器表（测试版本2）',
        '',
        '由 other_test/mtp/build_mtp_config_module.py 从',
        f'    {SRC.name}',
        '转换生成，可直接在本文件中查看与修改。',
        '',
        '数据格式：',
        '    (ADDR, INDEX, B31, B23, B15, B7, 模块号)',
        '    B31..B7 依次对应寄存器 [31:24] [23:16] [15:8] [7:0]',
        '    物理字节地址 phys = ADDR + INDEX*4',
        '',
        '修改后可直接调用 compute_crc() 得到 CRC32；',
        '需要烧录时用 other_test/mtp/build_mtp_template.py 由本表反向生成 Excel 模板。',
        '"""',
        '',
        f'CRC_ADDR_LIMIT = {CRC_ADDR_LIMIT}      # CRC 覆盖 phys < 416',
        f'CRC_RSV_ADDRS = ({CRC_RSV_LO}, {CRC_RSV_LO + 1}, {CRC_RSV_LO + 2}, {CRC_RSV_HI})  # 小端存放',
        f'BUILD_CRC = 0x{final_crc:08X}       # 生成时按本表算出的 CRC32',
        '',
        '# (ADDR, INDEX, B31, B23, B15, B7, 模块号)',
        'MTP_CONFIG_ROWS = [',
    ]
    prev_addr = None
    for addr, index, b31, b23, b15, b7, mod in rows:
        if prev_addr is not None and addr != prev_addr:
            lines.append('')
        prev_addr = addr
        lines.append(
            f'    (0x{addr:02X}, {index:2d}, 0x{b31:02X}, 0x{b23:02X}, '
            f'0x{b15:02X}, 0x{b7:02X}, {str(mod):>4}),   # phys={addr + index * 4}'
        )
    lines += [
        ']',
        '',
        '',
        'def iter_bytes(rows=None):',
        '    """展开成 {物理地址: 字节值} 字典。"""',
        '    out = {}',
        '    for addr, index, b31, b23, b15, b7, *_ in (rows or MTP_CONFIG_ROWS):',
        '        phys = addr + index * 4',
        '        out[phys] = b7',
        '        out[phys + 1] = b15',
        '        out[phys + 2] = b23',
        '        out[phys + 3] = b31',
        '    return out',
        '',
        '',
        'def compute_crc(rows=None):',
        '    """按烧录口径算 CRC32：声明字节 ∩ phys<CRC_ADDR_LIMIT，crc.py(LSB 优先)。"""',
        '    import sys',
        '    from pathlib import Path',
        '    _iic = Path(__file__).resolve().parents[2] / "instrument" / "IIC"',
        '    if str(_iic) not in sys.path:',
        '        sys.path.insert(0, str(_iic))',
        '    from crc import calculate_crc32',
        '    items = iter_bytes(rows)',
        '    addrs = sorted(a for a in items if a < CRC_ADDR_LIMIT)',
        '    return calculate_crc32(bytes(items[a] for a in addrs))',
        '',
        '',
        'if __name__ == "__main__":',
        '    crc = compute_crc()',
        '    print(f"行数 {len(MTP_CONFIG_ROWS)}，字节 {len(iter_bytes())}，CRC32 = 0x{crc:08X}")',
        '    print("与生成时一致" if crc == BUILD_CRC else "⚠️ 与生成时的 CRC 不一致，请核对表内容")',
        '',
    ]

    if others:
        lines += [
            '',
            '# 源表中未烧录的行（原样保留，供参考）',
            '# (源行号, OPERATION, [31:24], [23:16], [15:8], [7:0])',
            'OTHER_ROWS = [',
        ]
        for r, op, cells in others:
            vals = ', '.join(repr(v) for v in (cells[1], cells[2], cells[3], cells[4], cells[5], cells[6]))
            lines.append(f'    ({r}, {op!r}, {vals}),')
        lines.append(']')

    DST.write_text('\n'.join(lines) + '\n', encoding='utf-8')
    print(f"PROGRAM 行 {len(rows)}，字节 {len(addrs)}，CRC 覆盖 {len(crc_addrs)}，CRC32 = 0x{final_crc:08X}")
    print(f"✅ 已生成：{DST}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
