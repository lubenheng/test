# -*- coding: utf-8 -*-
"""
转换脚本：将 `2361_配置寄存器_V0.2.xlsx` 转为与 `电机.xlsx` 相同的 MTP 模板格式。

用法:
    python convert_2361_to_motor.py 
或
    python convert_2361_to_motor.py --input 源文件.xlsx --output 目标文件.xlsx

说明：
- 脚本会加载同目录下的 `config.py` 中的 `REGISTER_MAP`，用于解析模块基地址。
- 在源 Excel 中，脚本会扫描所有表、找出含有 4 个字节字段（[31:24]..[7:0]）的行，并尝试从附近列推断模块名和索引。
- 输出表头与 `电机.xlsx` 结构兼容：`OPERATION, ADDR(基地址), INDEX(Word偏移), [31:24],..., [7:0], ..., 寄存器index, 寄存器模块, 寄存器模块地址(WORDS)`。

此脚本以容错为主——会尽量匹配模块名（不严格区分大小写），并记录无法匹配的行以供人工复核。
"""

from __future__ import annotations
import argparse
import importlib.util
import os
import re
import shutil
import csv
from openpyxl import Workbook, load_workbook


def load_config_module(path: str):
    spec = importlib.util.spec_from_file_location("cfg", path)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


def to_hex_byte(v):
    if v is None:
        return ''
    if isinstance(v, (int,)):
        return f"{v & 0xFF:02X}"
    s = str(v).strip()
    if s == '':
        return ''
    # allow decimal-looking but really hex strings, strip 0x
    s2 = s.lower().replace('0x', '').replace(' ', '')
    # if contains non-hex digits, try decimal
    if re.fullmatch(r'[0-9a-f]+', s2):
        try:
            iv = int(s2, 16)
            return f"{iv & 0xFF:02X}"
        except Exception:
            pass
    # fallback: try int cast
    try:
        iv = int(float(s))
        return f"{iv & 0xFF:02X}"
    except Exception:
        return ''


def find_module_key(name: str, register_map: dict):
    if not name:
        return None
    n = str(name).strip().lower()
    # direct match
    for k in register_map.keys():
        if k.lower() == n:
            return k
    # substring match
    for k in register_map.keys():
        if k.lower() in n or n in k.lower():
            return k
    # some common mappings
    aliases = {
        'motor': 'motor_top',
        'foc': 'foc_top',
        'driver': 'driver',
        'diver': 'driver',
        'pid': 'pid',
        'timer4': 'timer4',
        'ana_ctrl': 'ana_ctrl',
        'ana_internal': 'ana_internal',
        'crc': 'crc_value',
    }
    for short, mapped in aliases.items():
        if short in n:
            if mapped in register_map:
                return mapped
    return None


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
        # 如果像 "4 #sym:DRIVER_BASE" 这样的带注释字符串，提取前导数字
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


def find_header_cols(ws):
    # assume header is first row
    first = next(ws.iter_rows(min_row=1, max_row=1, values_only=True))
    def find_col(keywords):
        for i, val in enumerate(first):
            if val is None:
                continue
            v = str(val).lower()
            for k in keywords:
                if k in v:
                    return i + 1
        return None

    cols = {}
    cols['op'] = find_col(['operation', 'op', '操作']) or 1
    cols['addr'] = find_col(['addr', '基地址', '地址']) or 2
    cols['index'] = find_col(['index', '偏移', 'word']) or 3
    cols['b4'] = find_col(['31:24', '[31', '31:24']) or 4
    cols['b3'] = find_col(['23:16', '[23', '23:16']) or 5
    cols['b2'] = find_col(['15:8', '[15', '15:8']) or 6
    cols['b1'] = find_col(['7:0', '[7', '7:0']) or 7
    cols['regindex'] = find_col(['寄存器index', '寄存器 index', 'regindex']) or 11
    cols['regmodule'] = find_col(['寄存器模块', '模块']) or 12
    cols['module_addr'] = find_col(['寄存器模块地址', '模块地址', 'words']) or 13
    cols['matched'] = find_col(['matched', '匹配', '是否匹配'])
    cols['header_len'] = len(first)
    return cols


def convert(input_xlsx, output_xlsx, config_py_path, report_path=None):
    cfg = load_config_module(config_py_path)
    register_map = getattr(cfg, 'REGISTER_MAP', {})
    # read-only workbook for source values
    wb_in = load_workbook(input_xlsx, data_only=True)
    # writable workbook to annotate source rows
    wb_src = load_workbook(input_xlsx)

    # try to use 电机.xlsx as template if available — copy it to output first,
    # then open the copy and overlay matches on top of that copy.
    template_path = os.path.abspath('电机.xlsx')
    if os.path.exists(template_path):
        try:
            shutil.copyfile(template_path, output_xlsx)
            out_wb = load_workbook(output_xlsx)
            out_ws = out_wb['MTP模板'] if 'MTP模板' in out_wb.sheetnames else out_wb.active
        except Exception:
            # fallback: load template and edit in-memory, will save to output later
            out_wb = load_workbook(template_path)
            out_ws = out_wb['MTP模板'] if 'MTP模板' in out_wb.sheetnames else out_wb.active
    else:
        out_wb = Workbook()
        out_ws = out_wb.active
        out_ws.title = 'MTP模板'
        headers = [
            'OPERATION', 'ADDR(基地址)', 'INDEX(Word偏移)', '[31:24]', '[23:16]', '[15:8]', '[7:0]',
            '物理地址(计算)', None, None, '寄存器index', '寄存器模块', '寄存器模块地址(WORDS)'
        ]
        out_ws.append(headers)

    cols = find_header_cols(out_ws)
    # ensure a MATCHED column exists at end; if not, append it and recompute
    if not cols.get('matched'):
        out_ws.cell(row=1, column=cols['header_len'] + 1).value = 'MATCHED'
        cols = find_header_cols(out_ws)

    # trace records for reporting
    trace = []

    processed = 0
    unmatched = []

    # If workbook contains a sheet named '批量操作模板', only process that sheet.
    sheet_names = list(wb_in.sheetnames)
    if '批量操作模板' in sheet_names:
        sheet_names = ['批量操作模板']
    # prepare annotation columns for source workbook (per sheet)
    src_col_map = {}
    for sn in sheet_names:
        src_ws = wb_src[sn]
        first = next(src_ws.iter_rows(min_row=1, max_row=1, values_only=True))
        headers = ['MATCH_ACTION', 'MATCH_METHOD', 'FOUND_ROW', 'DIFFS']
        mapping = {}
        for i, v in enumerate(first, start=1):
            if v is None:
                continue
            vn = str(v).strip().upper()
            if vn in headers:
                mapping[vn] = i
        cur = src_ws.max_column
        for h in headers:
            if h not in mapping:
                cur += 1
                src_ws.cell(row=1, column=cur).value = h
                mapping[h] = cur
        src_col_map[sn] = mapping
    for sheet_name in sheet_names:
        ws = wb_in[sheet_name]
        for src_row_idx, row in enumerate(ws.iter_rows(values_only=True), start=1):
            if not row:
                continue
            # find candidate bytes in columns 4..7 (index 3..6)
            if len(row) < 7:
                continue
            b4 = to_hex_byte(row[3])
            b3 = to_hex_byte(row[4])
            b2 = to_hex_byte(row[5])
            b1 = to_hex_byte(row[6])
            if not (b4 or b3 or b2 or b1):
                continue

            # find module candidate for display in columns 8..12 (index 7..11)
            module_candidate = None
            for i in range(7, min(len(row), 12)):
                if row[i] not in (None, ''):
                    module_candidate = str(row[i])
                    break

            # MATCHING RULE: only match using second column (B列) numeric value
            # Interpret B列 as the module 'word_addr' (per MODULE_MAP). 名称字段仅用于显示。
            src_word = None
            if len(row) >= 2:
                src_word = as_int_or_none(row[1])

            module_key = None
            module_info = None
            if src_word is not None:
                mm = getattr(cfg, 'MODULE_MAP', {})
                for mk, info in mm.items():
                    if info.get('word_addr') == src_word:
                        module_key = mk
                        module_info = info
                        break

            # pick index candidate (Word offset) from col3 (index 2) or col2 (index 1)
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

            # determine base address via MODULE_MAP match (use numeric base only)
            base_addr = None
            module_word_addr_val = None
            if module_info is not None:
                base_addr = module_info.get('base', None)
                module_word_addr_val = module_info.get('word_addr')
            else:
                # fallback: use source B列 value if numeric
                base_addr = src_word if src_word is not None else 0

            if index_candidate is None:
                index_candidate = 0

            # compute reg_index inside module if possible (use register_map defs)
            reg_index = None
            # try to find canonical module in REGISTER_MAP by matching base
            canonical_module = None
            for k, info in register_map.items():
                if info.get('base') == base_addr:
                    canonical_module = k
                    break
            if canonical_module:
                regs = register_map[canonical_module]['regs']
                phys = base_addr + (int(index_candidate) * 4)
                for idx, tup in enumerate(regs):
                    if len(tup) >= 3 and tup[2] == phys:
                        reg_index = idx
                        break

            # try to find existing row in template by numeric addr+index
            found_cells = None
            found_row_idx = None
            match_method = None
            # 每次匹配时从原始模板文件读取，定位到模板中的行号，再在输出副本上使用相同行号进行覆盖。
            if os.path.exists(template_path):
                try:
                    temp_wb = load_workbook(template_path, data_only=True)
                    temp_ws = temp_wb['MTP模板'] if 'MTP模板' in temp_wb.sheetnames else temp_wb.active
                    temp_cols = find_header_cols(temp_ws)
                    idx_col_num_temp = temp_cols.get('regindex') or temp_cols.get('index')
                    found_row_idx = None
                    # Prefer matching by module_addr (模板的 module addr/words 列) + 寄存器 index (K 列),
                    # because 某些模板（如 driver）把基地址写在物理地址列或用 module_addr 表示模块ID。
                    mod_addr_col_temp = temp_cols.get('module_addr')
                    regmod_col_temp = temp_cols.get('regmodule')
                    for r_idx, temp_row in enumerate(temp_ws.iter_rows(min_row=2), start=2):
                        try:
                            addr_cell_temp = temp_row[temp_cols['addr'] - 1] if temp_cols['addr'] <= len(temp_row) else None
                            idx_cell_temp = temp_row[idx_col_num_temp - 1] if idx_col_num_temp and idx_col_num_temp <= len(temp_row) else None
                            module_addr_cell_temp = temp_row[mod_addr_col_temp - 1] if mod_addr_col_temp and mod_addr_col_temp <= len(temp_row) else None
                        except Exception:
                            addr_cell_temp = None
                            idx_cell_temp = None
                            module_addr_cell_temp = None

                        addr_val_temp = as_int_or_none(addr_cell_temp.value) if addr_cell_temp is not None else None
                        idx_val_temp = as_int_or_none(idx_cell_temp.value) if idx_cell_temp is not None else None
                        module_addr_val_temp = as_int_or_none(module_addr_cell_temp.value) if module_addr_cell_temp is not None else None

                        # 1) 尝试用模板的 module_addr + regindex 精确匹配（优先）
                        if module_word_addr_val is not None and module_addr_val_temp is not None:
                            if module_addr_val_temp == module_word_addr_val and (idx_val_temp == index_candidate or (reg_index is not None and idx_val_temp == reg_index)):
                                found_row_idx = r_idx
                                break

                        # 2) 回退到旧的 addr + index 匹配（向后兼容）
                        if addr_val_temp == base_addr and idx_val_temp == index_candidate:
                            found_row_idx = r_idx
                            break
                    if found_row_idx is not None:
                        rows = tuple(out_ws.iter_rows(min_row=found_row_idx, max_row=found_row_idx))
                        if rows:
                            found_cells = rows[0]
                            match_method = 'template_file_row'
                except Exception:
                    # 如果每次读取模板失败，回退到内存中扫描输出副本
                    # fallback: scan the already-copied output workbook in-memory
                    mod_addr_col_out = cols.get('module_addr')
                    idx_col_num_out = cols.get('regindex') or cols.get('index')
                    for cells in out_ws.iter_rows(min_row=2):
                        try:
                            addr_cell = cells[cols['addr'] - 1] if cols['addr'] <= len(cells) else None
                            idx_cell = cells[idx_col_num_out - 1] if idx_col_num_out and idx_col_num_out <= len(cells) else None
                            module_addr_cell = cells[mod_addr_col_out - 1] if mod_addr_col_out and mod_addr_col_out <= len(cells) else None
                        except Exception:
                            addr_cell = None
                            idx_cell = None
                            module_addr_cell = None
                        addr_val = as_int_or_none(addr_cell.value) if addr_cell is not None else None
                        idx_val = as_int_or_none(idx_cell.value) if idx_cell is not None else None
                        module_addr_val = as_int_or_none(module_addr_cell.value) if module_addr_cell is not None else None
                        # try module_addr + regindex first
                        if module_word_addr_val is not None and module_addr_val is not None:
                            if module_addr_val == module_word_addr_val and (idx_val == index_candidate or (reg_index is not None and idx_val == reg_index)):
                                found_cells = cells
                                match_method = 'out_ws_scan'
                                break
                        # fallback to addr+index
                        if addr_val == base_addr and idx_val == index_candidate:
                            found_cells = cells
                            match_method = 'out_ws_scan'
                            break
            else:
                # 模板文件不存在，直接在输出副本内存中扫描
                mod_addr_col_out = cols.get('module_addr')
                idx_col_num_out = cols.get('regindex') or cols.get('index')
                for cells in out_ws.iter_rows(min_row=2):
                    try:
                        addr_cell = cells[cols['addr'] - 1] if cols['addr'] <= len(cells) else None
                        idx_cell = cells[idx_col_num_out - 1] if idx_col_num_out and idx_col_num_out <= len(cells) else None
                        module_addr_cell = cells[mod_addr_col_out - 1] if mod_addr_col_out and mod_addr_col_out <= len(cells) else None
                    except Exception:
                        addr_cell = None
                        idx_cell = None
                        module_addr_cell = None
                    addr_val = as_int_or_none(addr_cell.value) if addr_cell is not None else None
                    idx_val = as_int_or_none(idx_cell.value) if idx_cell is not None else None
                    module_addr_val = as_int_or_none(module_addr_cell.value) if module_addr_cell is not None else None
                    # try module_addr + regindex first
                    if module_word_addr_val is not None and module_addr_val is not None:
                        if module_addr_val == module_word_addr_val and (idx_val == index_candidate or (reg_index is not None and idx_val == reg_index)):
                            found_cells = cells
                            match_method = 'out_ws_scan'
                            break
                    # fallback to addr+index
                    if addr_val == base_addr and idx_val == index_candidate:
                        found_cells = cells
                        match_method = 'out_ws_scan'
                        break

            matched_flag = (module_key is not None)

            if found_cells is not None:
                # record before values
                before = {}
                keys = ['op','addr','b4','b3','b2','b1','regindex','regmodule','module_addr','matched']
                for k in keys:
                    cnum = cols.get(k)
                    try:
                        before[k] = found_cells[cnum - 1].value if cnum and cnum <= len(found_cells) else None
                    except Exception:
                        before[k] = None

                # overlay fields; keep original where new value is empty
                if cols['op'] <= len(found_cells):
                    found_cells[cols['op'] - 1].value = 'PROGRAM'
                if cols['addr'] <= len(found_cells):
                    found_cells[cols['addr'] - 1].value = base_addr
                # always overwrite byte columns when matched; set to None to clear if source empty
                if cols['b4'] <= len(found_cells):
                    found_cells[cols['b4'] - 1].value = (b4 or None)
                if cols['b3'] <= len(found_cells):
                    found_cells[cols['b3'] - 1].value = (b3 or None)
                if cols['b2'] <= len(found_cells):
                    found_cells[cols['b2'] - 1].value = (b2 or None)
                if cols['b1'] <= len(found_cells):
                    found_cells[cols['b1'] - 1].value = (b1 or None)
                if cols['regindex'] <= len(found_cells):
                    found_cells[cols['regindex'] - 1].value = reg_index if reg_index is not None else index_candidate
                if (module_key or module_candidate) and cols['regmodule'] <= len(found_cells):
                    found_cells[cols['regmodule'] - 1].value = module_key if module_key else module_candidate
                if module_word_addr_val is not None and cols['module_addr'] <= len(found_cells):
                    found_cells[cols['module_addr'] - 1].value = module_word_addr_val
                # mark matched
                if cols.get('matched') and cols['matched'] <= len(found_cells):
                    found_cells[cols['matched'] - 1].value = 'YES' if matched_flag else 'NO'

                # record after values and diffs
                after = {}
                for k in keys:
                    cnum = cols.get(k)
                    try:
                        after[k] = found_cells[cnum - 1].value if cnum and cnum <= len(found_cells) else None
                    except Exception:
                        after[k] = None
                diffs = {k: (before.get(k), after.get(k)) for k in keys if before.get(k) != after.get(k)}

                # capture template addr/index for diagnostics
                try:
                    idx_col_out = cols.get('regindex') or cols.get('index')
                    template_addr_val = None
                    template_idx_val = None
                    if cols.get('addr') and cols['addr'] <= len(found_cells):
                        template_addr_val = as_int_or_none(found_cells[cols['addr'] - 1].value)
                    if idx_col_out and idx_col_out <= len(found_cells):
                        template_idx_val = as_int_or_none(found_cells[idx_col_out - 1].value)
                except Exception:
                    template_addr_val = None
                    template_idx_val = None

                trace.append({
                    'sheet': sheet_name,
                    'src_row': src_row_idx,
                    'src_word': src_word,
                    'index_candidate': index_candidate,
                    'module_key': module_key,
                    'module_candidate': module_candidate,
                    'base_addr': base_addr,
                    'reg_index': reg_index,
                    'match_method': match_method,
                    'found_row_idx': found_row_idx,
                    'template_addr': template_addr_val,
                    'template_index': template_idx_val,
                    'used_addr': base_addr,
                    'used_index': index_candidate,
                    'matched_flag': matched_flag,
                    'action': 'overlay',
                    'diffs': diffs,
                })
                # write back annotation into source workbook for this row
                try:
                    diffs_s = ';'.join([f"{k}:{v[0]}->{v[1]}" for k, v in diffs.items()])
                    src_ws = wb_src[sheet_name]
                    mapping = src_col_map.get(sheet_name, {})
                    if mapping:
                        src_ws.cell(row=src_row_idx, column=mapping.get('MATCH_ACTION')).value = 'overlay'
                        src_ws.cell(row=src_row_idx, column=mapping.get('MATCH_METHOD')).value = match_method
                        src_ws.cell(row=src_row_idx, column=mapping.get('FOUND_ROW')).value = found_row_idx
                        src_ws.cell(row=src_row_idx, column=mapping.get('DIFFS')).value = diffs_s
                except Exception:
                    pass
            else:
                # append new row based on template header length
                new_row = [None] * cols['header_len']
                # addr: include symbol annotation when available
                if cols['op'] - 1 < len(new_row):
                    new_row[cols['op'] - 1] = 'PROGRAM'
                if cols['addr'] - 1 < len(new_row):
                    new_row[cols['addr'] - 1] = base_addr
                if cols['index'] - 1 < len(new_row):
                    new_row[cols['index'] - 1] = index_candidate
                if cols['b4'] - 1 < len(new_row):
                    new_row[cols['b4'] - 1] = b4 or None
                if cols['b3'] - 1 < len(new_row):
                    new_row[cols['b3'] - 1] = b3 or None
                if cols['b2'] - 1 < len(new_row):
                    new_row[cols['b2'] - 1] = b2 or None
                if cols['b1'] - 1 < len(new_row):
                    new_row[cols['b1'] - 1] = b1 or None
                if cols['regindex'] - 1 < len(new_row):
                    new_row[cols['regindex'] - 1] = reg_index if reg_index is not None else index_candidate
                if cols['regmodule'] - 1 < len(new_row):
                    new_row[cols['regmodule'] - 1] = module_key if module_key else (module_candidate or None)
                if cols['module_addr'] - 1 < len(new_row):
                    new_row[cols['module_addr'] - 1] = module_word_addr_val
                # matched flag at the end
                if cols.get('matched') and cols['matched'] - 1 < len(new_row):
                    new_row[cols['matched'] - 1] = 'YES' if matched_flag else 'NO'
                out_ws.append(new_row)
                # record appended row
                appended_idx = out_ws.max_row
                appended_cells = tuple(out_ws.iter_rows(min_row=appended_idx, max_row=appended_idx))[0]
                # capture after values
                keys = ['op','addr','b4','b3','b2','b1','regindex','regmodule','module_addr','matched']
                after = {}
                for k in keys:
                    cnum = cols.get(k)
                    try:
                        after[k] = appended_cells[cnum - 1].value if cnum and cnum <= len(appended_cells) else None
                    except Exception:
                        after[k] = None
                trace.append({
                    'sheet': sheet_name,
                    'src_row': src_row_idx,
                    'src_word': src_word,
                    'index_candidate': index_candidate,
                    'module_key': module_key,
                    'module_candidate': module_candidate,
                    'base_addr': base_addr,
                    'reg_index': reg_index,
                    'match_method': 'append',
                    'found_row_idx': appended_idx,
                    'template_addr': None,
                    'template_index': None,
                    'used_addr': base_addr,
                    'used_index': index_candidate,
                    'matched_flag': matched_flag,
                    'action': 'append',
                    'diffs': {k: (None, after.get(k)) for k in keys if after.get(k) is not None}
                })
                # write appended annotation back into source workbook for this row
                try:
                    diffs = {k: (None, after.get(k)) for k in keys if after.get(k) is not None}
                    diffs_s = ';'.join([f"{k}:{v[0]}->{v[1]}" for k, v in diffs.items()])
                    src_ws = wb_src[sheet_name]
                    mapping = src_col_map.get(sheet_name, {})
                    if mapping:
                        src_ws.cell(row=src_row_idx, column=mapping.get('MATCH_ACTION')).value = 'append'
                        src_ws.cell(row=src_row_idx, column=mapping.get('MATCH_METHOD')).value = 'append'
                        src_ws.cell(row=src_row_idx, column=mapping.get('FOUND_ROW')).value = appended_idx
                        src_ws.cell(row=src_row_idx, column=mapping.get('DIFFS')).value = diffs_s
                except Exception:
                    pass

            processed += 1

            if module_key is None:
                unmatched.append((sheet_name, row))

    out_wb.save(output_xlsx)
    print(f"Processed rows: {processed}")
    print(f"Unmatched rows (module not found): {len(unmatched)} - first 10 listed below:")
    for i, t in enumerate(unmatched[:10]):
        print(i + 1, t[0], t[1])
    # save annotated source workbook
    try:
        annotated_path = os.path.splitext(input_xlsx)[0] + '_annotated.xlsx'
        wb_src.save(annotated_path)
        print(f"Wrote annotated source: {annotated_path}")
    except Exception as e:
        print('Failed to save annotated source:', e)
    # write trace report if requested
    if report_path:
        try:
            with open(report_path, 'w', newline='', encoding='utf-8') as f:
                writer = csv.writer(f)
                hdr = ['sheet','src_row','src_word','index_candidate','module_key','module_candidate','base_addr','reg_index','match_method','found_row_idx','template_addr','template_index','used_addr','used_index','matched_flag','action','diffs']
                writer.writerow(hdr)
                for r in trace:
                    diffs = r.get('diffs') or {}
                    # format diffs as col:old->new;...
                    diffs_s = ';'.join([f"{k}:{v[0]}->{v[1]}" for k, v in diffs.items()])
                    writer.writerow([
                        r.get('sheet'), r.get('src_row'), r.get('src_word'), r.get('index_candidate'),
                        r.get('module_key'), r.get('module_candidate'), r.get('base_addr'), r.get('reg_index'),
                        r.get('match_method'), r.get('found_row_idx'), r.get('template_addr'), r.get('template_index'), r.get('used_addr'), r.get('used_index'),
                        r.get('matched_flag'), r.get('action'), diffs_s
                    ])
            print(f"Wrote report: {report_path}")
        except Exception as e:
            print('Failed to write report:', e)


def main():
    p = argparse.ArgumentParser()
    p.add_argument('--input', '-i', default='2361_配置寄存器_V0.2.xlsx')
    p.add_argument('--output', '-o', default='电机_from_2361.xlsx')
    p.add_argument('--report', '-r', default=None, help='CSV path to write match/overlay report')
    p.add_argument('--config', '-c', default='config.py')
    args = p.parse_args()

    input_path = os.path.abspath(args.input)
    output_path = os.path.abspath(args.output)
    config_path = os.path.abspath(args.config)

    if not os.path.exists(input_path):
        raise FileNotFoundError(f"input not found: {input_path}")
    if not os.path.exists(config_path):
        raise FileNotFoundError(f"config.py not found: {config_path}")

    convert(input_path, output_path, config_path, args.report)


if __name__ == '__main__':
    main()
