#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
extract_addr_index.py

从源 Excel 提取寄存器的物理 ADDR（MTP 地址）和 index（Word 偏移），
并输出 CSV 或 XLSX 映射表，格式为：sheet, src_row, src_word, index_candidate, reg_addr, reg_index, module_key

用法:
    python extract_addr_index.py --input 源文件.xlsx --output addr_index_map.csv --xlsx addr_index_map.xlsx --config config.py

脚本不会修改模板或输出工作簿；只读源文件并生成映射文件。
"""

from __future__ import annotations
import argparse
import importlib.util
import os
import csv
from openpyxl import load_workbook, Workbook


def load_config_module(path: str):
    spec = importlib.util.spec_from_file_location("cfg", path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def as_int_or_none(v):
    if v is None:
        return None
    if isinstance(v, int):
        return v
    try:
        return int(v)
    except Exception:
        pass
    try:
        s = str(v).strip()
        m = __import__('re').match(r'^\s*([0-9]+)', s)
        if m:
            return int(m.group(1))
        if s.lower().startswith('0x'):
            return int(s, 16)
        if __import__('re').fullmatch(r'[0-9a-fA-F]+', s):
            return int(s, 16)
        return int(float(s))
    except Exception:
        return None


def to_hex_byte(v):
    if v is None:
        return ''
    if isinstance(v, int):
        return f"{v & 0xFF:02X}"
    s = str(v).strip()
    if s == '':
        return ''
    s2 = s.lower().replace('0x', '').replace(' ', '')
    if __import__('re').fullmatch(r'[0-9a-f]+', s2):
        try:
            iv = int(s2, 16)
            return f"{iv & 0xFF:02X}"
        except Exception:
            pass
    try:
        iv = int(float(s))
        return f"{iv & 0xFF:02X}"
    except Exception:
        return ''


def extract(input_xlsx: str, config_py_path: str, out_csv: str | None, out_xlsx: str | None):
    cfg = load_config_module(config_py_path)
    module_map = getattr(cfg, 'MODULE_MAP', {})
    register_map = getattr(cfg, 'REGISTER_MAP', {})

    wb_in = load_workbook(input_xlsx, data_only=True)
    sheet_names = list(wb_in.sheetnames)
    # 如果存在专门的批量表，则只处理该表
    if '批量操作模板' in sheet_names:
        sheet_names = ['批量操作模板']

    rows = []
    processed = 0
    for sn in sheet_names:
        ws = wb_in[sn]
        for src_row_idx, row in enumerate(ws.iter_rows(values_only=True), start=1):
            if not row:
                continue
            # 仅处理含有至少一个字节字段的行（源表字节应在第4..7列）
            if len(row) < 7:
                continue
            b4 = to_hex_byte(row[3])
            b3 = to_hex_byte(row[4])
            b2 = to_hex_byte(row[5])
            b1 = to_hex_byte(row[6])
            if not (b4 or b3 or b2 or b1):
                continue

            src_word = as_int_or_none(row[1]) if len(row) >= 2 else None

            # index_candidate: 优先列3 (C)，回退到列2
            index_candidate = None
            if len(row) >= 3 and row[2] is not None:
                try:
                    index_candidate = int(row[2])
                except Exception:
                    index_candidate = None
            if index_candidate is None and len(row) >= 2 and row[1] is not None:
                try:
                    index_candidate = int(row[1])
                except Exception:
                    index_candidate = None
            if index_candidate is None:
                index_candidate = 0

            module_key = None
            module_info = None
            if src_word is not None:
                for mk, info in module_map.items():
                    if info.get('word_addr') == src_word:
                        module_key = mk
                        module_info = info
                        break

            if module_info is not None:
                base_addr = module_info.get('base')
            else:
                base_addr = src_word if src_word is not None else 0

            # 计算寄存器物理地址 (MTP addr)
            try:
                phys = int(base_addr) + int(index_candidate) * 4
            except Exception:
                phys = None

            # 尝试从 REGISTER_MAP 中解析 reg_index（如果有匹配的 module base）
            reg_index = None
            canonical_module = None
            for k, info in register_map.items():
                if info.get('base') == base_addr:
                    canonical_module = k
                    break
            if canonical_module and phys is not None:
                regs = register_map[canonical_module]['regs']
                for idx, tup in enumerate(regs):
                    if len(tup) >= 3 and tup[2] == phys:
                        reg_index = idx
                        break

            module_candidate = None
            for i in range(7, min(len(row), 12)):
                if row[i] not in (None, ''):
                    module_candidate = str(row[i])
                    break

            rows.append({
                'sheet': sn,
                'src_row': src_row_idx,
                'src_word': src_word,
                'index_candidate': index_candidate,
                'module_key': module_key,
                'module_candidate': module_candidate,
                'base_addr': base_addr,
                'reg_index': reg_index,
                'reg_addr': phys,
                'b4': b4, 'b3': b3, 'b2': b2, 'b1': b1,
            })
            processed += 1

    if out_csv:
        with open(out_csv, 'w', newline='', encoding='utf-8') as f:
            writer = csv.writer(f)
            hdr = ['sheet','src_row','src_word','index_candidate','module_key','module_candidate','base_addr','reg_index','reg_addr','b4','b3','b2','b1']
            writer.writerow(hdr)
            for r in rows:
                writer.writerow([r.get(c) for c in hdr])

    if out_xlsx:
        wb = Workbook()
        out_ws = wb.active
        out_ws.title = 'addr_index_map'
        hdr = ['sheet','src_row','src_word','index_candidate','module_key','module_candidate','base_addr','reg_index','reg_addr','b4','b3','b2','b1']
        out_ws.append(hdr)
        for r in rows:
            out_ws.append([r.get(c) for c in hdr])
        wb.save(out_xlsx)

    return processed


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--input', '-i', default='2361_配置寄存器_V0.2.xlsx')
    p.add_argument('--output', '-o', default='addr_index_map.csv', help='CSV 输出路径')
    p.add_argument('--xlsx', dest='xlsx', default=None, help='可选 XLSX 输出路径')
    p.add_argument('--config', '-c', default='config.py')
    args = p.parse_args()

    input_path = os.path.abspath(args.input)
    config_path = os.path.abspath(args.config)

    if not os.path.exists(input_path):
        raise FileNotFoundError(f'input not found: {input_path}')
    if not os.path.exists(config_path):
        raise FileNotFoundError(f'config.py not found: {config_path}')

    processed = extract(input_path, config_path, args.output, args.xlsx)
    print(f'Processed rows: {processed}')
    print(f'Wrote: {args.output}' + (f', {args.xlsx}' if args.xlsx else ''))


if __name__ == '__main__':
    main()
