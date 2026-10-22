#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
apply_numeric_overlay.py

按数值匹配覆盖操作：
- 在目标文件（默认：MTP_预定位_IF_FOC150K五段式SP备份20260424.xlsx）中，使用 J 列（寄存器 ADDR）和 K 列（寄存器 index）作为匹配键；
- 在源文件（默认：2361_配置寄存器_V0.4.xlsx）中，使用 B 列（ADDR）和 C 列（INDEX）作为匹配键；
- 若两者数值完全相等，则把源文件行的 D/E/F/G 列值覆盖写入目标文件对应行的 D/E/F/G 列；
- 结果保存为新的 XLSX 文件，不修改原始文件。

用法例子：
  c:/.../.venv/Scripts/python.exe apply_numeric_overlay.py \
      --target MTP_预定位_IF_FOC150K五段式SP备份20260424.xlsx \
      --source 2361_配置寄存器_V0.4.xlsx \
      --out MTP_预定位_IF_FOC150K五段式SP备份20260424.updated.xlsx \
      --report overlay_report.csv

"""

from __future__ import annotations
import argparse
import csv
import os
import importlib.util
from openpyxl import load_workbook
from copy import copy as _copy


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
        import re
        m = re.match(r'^\s*([0-9]+)', s)
        if m:
            return int(m.group(1))
        if s.lower().startswith('0x'):
            return int(s, 16)
        if re.fullmatch(r'[0-9a-fA-F]+', s):
            return int(s, 16)
        return int(float(s))
    except Exception:
        return None


def load_config_module(path: str):
    spec = importlib.util.spec_from_file_location("cfg", path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def build_source_lookup(source_path: str, sheet_name: str | None = None):
    wb = load_workbook(source_path, data_only=True)
    # 如果未指定sheet_name，且存在多个sheet，优先选择"批量操作模板"
    if not sheet_name:
        if '批量操作模板' in wb.sheetnames:
            ws = wb['批量操作模板']
        else:
            ws = wb.active
    else:
        ws = wb[sheet_name] if sheet_name in wb.sheetnames else wb.active

    lookup = {}
    duplicates = []
    for r_idx, row in enumerate(ws.iter_rows(values_only=True), start=1):
        # B 列 (index 1) = ADDR, C 列 (index 2) = INDEX
        addr = as_int_or_none(row[1]) if len(row) >= 2 else None
        idx = as_int_or_none(row[2]) if len(row) >= 3 else None
        if addr is None or idx is None:
            continue
        key = (addr, idx)
        # D/E/F/G 列为 row[3:7]
        defg = [row[3] if len(row) >= 4 else None,
                row[4] if len(row) >= 5 else None,
                row[5] if len(row) >= 6 else None,
                row[6] if len(row) >= 7 else None]
        if key in lookup:
            duplicates.append((key, r_idx))
            # keep first occurrence, ignore duplicates
            continue
        lookup[key] = {'row': r_idx, 'defg': defg}
    return lookup, duplicates


def apply_overlay(target_path: str, source_path: str, out_path: str, target_sheet: str | None = None, source_sheet: str | None = None, report_csv: str | None = None, module_filter: str | None = None, config_path: str = 'config.py'):
    lookup, duplicates = build_source_lookup(source_path, source_sheet)
    tb_wb = load_workbook(target_path, data_only=True)
    # 如果未指定target_sheet，且存在多个sheet，优先选择"批量操作模板"
    if not target_sheet:
        if '批量操作模板' in tb_wb.sheetnames:
            tb_ws = tb_wb['批量操作模板']
        else:
            tb_ws = tb_wb.active
    else:
        tb_ws = tb_wb[target_sheet] if target_sheet in tb_wb.sheetnames else tb_wb.active

    updated = 0
    missing = 0
    rows_report = []
    used_source_keys = set()

    # if module_filter provided, compute set of MTP addresses for that module
    module_mtp_set = None
    if module_filter:
        if not os.path.exists(config_path):
            raise FileNotFoundError(f'config not found: {config_path}')
        cfg = load_config_module(config_path)
        register_map = getattr(cfg, 'REGISTER_MAP', {})
        if module_filter in register_map:
            regs = register_map[module_filter].get('regs', [])
            module_mtp_set = {tup[2] for tup in regs if len(tup) >= 3 and tup[2] is not None}
        else:
            # try MODULE_MAP fallback
            module_map = getattr(cfg, 'MODULE_MAP', {})
            info = module_map.get(module_filter)
            if info:
                base = info.get('base')
                for k, v in register_map.items():
                    if v.get('base') == base:
                        regs = v.get('regs', [])
                        module_mtp_set = {tup[2] for tup in regs if len(tup) >= 3 and tup[2] is not None}
                        break

    # iterate through target rows starting from row 2 (skip header)
    orig_max = tb_ws.max_row
    for r in range(2, orig_max + 1):
        addr_cell = tb_ws.cell(row=r, column=10)  # J 列
        idx_cell = tb_ws.cell(row=r, column=11)   # K 列
        addr = as_int_or_none(addr_cell.value)
        idx = as_int_or_none(idx_cell.value)
        if addr is None or idx is None:
            rows_report.append({'row': r, 'addr': addr_cell.value, 'index': idx_cell.value, 'status': 'no_key'})
            continue

        # if module filter applied, skip rows not in module_mtp_set
        if module_mtp_set is not None and addr not in module_mtp_set:
            rows_report.append({'row': r, 'addr': addr, 'index': idx, 'status': 'skipped_module'})
            continue
        key = (addr, idx)
        src = lookup.get(key)
        if not src:
            missing += 1
            rows_report.append({'row': r, 'addr': addr, 'index': idx, 'status': 'not_found'})
            continue
        defg = src['defg']
        # write to D/E/F/G (cols 4..7)
        tb_ws.cell(row=r, column=4).value = defg[0]
        tb_ws.cell(row=r, column=5).value = defg[1]
        tb_ws.cell(row=r, column=6).value = defg[2]
        tb_ws.cell(row=r, column=7).value = defg[3]
        # mark matched in M 列 (col 13)
        tb_ws.cell(row=r, column=13).value = 'YES'
        used_source_keys.add(key)
        updated += 1
        rows_report.append({'row': r, 'addr': addr, 'index': idx, 'status': 'updated', 'src_row': src['row']})

    # After scanning target rows, append any source rows that were not used
    appended_source = 0
    # load source workbook (with styles) to copy formatting
    src_wb_for_style = load_workbook(source_path, data_only=False)
    # 选择正确的source sheet（优先选择"批量操作模板"）
    if not source_sheet:
        if '批量操作模板' in src_wb_for_style.sheetnames:
            src_ws_for_style = src_wb_for_style['批量操作模板']
        else:
            src_ws_for_style = src_wb_for_style.active
    else:
        src_ws_for_style = src_wb_for_style[source_sheet] if source_sheet in src_wb_for_style.sheetnames else src_wb_for_style.active
    # determine number of columns to copy: use max of target cols and source cols, ensure at least 13 (to include M)
    needed_cols = max(tb_ws.max_column, src_ws_for_style.max_column, 13)
    for key, src in lookup.items():
        if key in used_source_keys:
            continue
        # if module filter applied, only append source rows that belong to module_mtp_set
        if module_mtp_set is not None:
            src_addr = key[0]
            if src_addr not in module_mtp_set:
                continue
        addr_val, idx_val = key
        src_row_idx = src.get('row')
        # append empty row first
        tb_ws.append([None] * needed_cols)
        appended_idx = tb_ws.max_row
        # copy source row cells (values and basic styles) into appended row
        try:
            src_cells = tuple(src_ws_for_style.iter_rows(min_row=src_row_idx, max_row=src_row_idx))[0]
        except Exception:
            src_cells = []
        for c in range(1, needed_cols + 1):
            new_cell = tb_ws.cell(row=appended_idx, column=c)
            if c <= len(src_cells):
                s_cell = src_cells[c - 1]
                new_cell.value = s_cell.value
                try:
                    new_cell.number_format = _copy(s_cell.number_format)
                except Exception:
                    pass
                try:
                    new_cell.font = _copy(s_cell.font)
                    new_cell.fill = _copy(s_cell.fill)
                    new_cell.border = _copy(s_cell.border)
                    new_cell.alignment = _copy(s_cell.alignment)
                    new_cell.protection = _copy(s_cell.protection)
                except Exception:
                    pass
        # ensure J (10) = ADDR and K (11) = INDEX are set to original source numeric values
        tb_ws.cell(row=appended_idx, column=10).value = addr_val
        tb_ws.cell(row=appended_idx, column=11).value = idx_val
        appended_source += 1
        rows_report.append({'row': appended_idx, 'addr': addr_val, 'index': idx_val, 'status': 'appended_source', 'src_row': src_row_idx})

    tb_wb.save(out_path)

    # write CSV report if requested
    if report_csv:
        with open(report_csv, 'w', newline='', encoding='utf-8') as f:
            writer = csv.writer(f)
            writer.writerow(['target_row', 'addr', 'index', 'status', 'source_row'])
            for it in rows_report:
                writer.writerow([it.get('row'), it.get('addr'), it.get('index'), it.get('status'), it.get('src_row')])

    return {'updated': updated, 'missing': missing, 'duplicates': duplicates, 'appended_source': appended_source, 'total_checked': len(rows_report)}


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--target', '-t', default='MTP_预定位_IF_FOC150K五段式SP备份20260424.xlsx')
    p.add_argument('--source', '-s', default='2361_配置寄存器_V0.4.xlsx')
    p.add_argument('--out', '-o', default=None)
    p.add_argument('--target-sheet', dest='target_sheet', default=None)
    p.add_argument('--source-sheet', dest='source_sheet', default=None)
    p.add_argument('--report', dest='report', default=None)
    args = p.parse_args()

    target_path = os.path.abspath(args.target)
    source_path = os.path.abspath(args.source)
    out_path = os.path.abspath(args.out) if args.out else os.path.splitext(target_path)[0] + '.updated.xlsx'

    if not os.path.exists(target_path):
        raise FileNotFoundError(f'target not found: {target_path}')
    if not os.path.exists(source_path):
        raise FileNotFoundError(f'source not found: {source_path}')

    res = apply_overlay(target_path, source_path, out_path, args.target_sheet, args.source_sheet, args.report)
    print(f"Wrote: {out_path}")
    print(f"Updated rows: {res['updated']}, Missing keys: {res['missing']}, Duplicates in source: {len(res['duplicates'])}")
    if res['duplicates']:
        print('Duplicate keys in source (kept first occurrence):')
        for d in res['duplicates'][:20]:
            print(' ', d)


if __name__ == '__main__':
    main()
