# -*- coding: utf-8 -*-
"""
测试结果记录（txt）与 txt → Excel 转换

用途
----
1. 测试过程中把每一项的 **实测值 + 判定范围（Min/Typ/Max）+ 结果** 追加写入
   `Save_Results/measure_Results/测试结果_<芯片>_<时间>.txt`
   （制表符分隔，边测边写，异常退出也不丢数据）；
2. 菜单里选择该 txt，转换成排好版的 `.xlsx`（含汇总页）。

目录约定
--------
    Save_Results/measure_Results/   测试项结果（txt / 转换后的 xlsx / 测量数据导出）
    Save_Results/MTP_Table/         MTP 配置表 txt 与烧录模板 xlsx

txt 格式（自描述，人工可读、程序可解析）
--------------------------------------
    # 芯片: OCD2361
    # 记录开始: 2026-10-21 15:00:00
    # 列: 时间  测试项  实测值  单位  Min  Typ  Max  结果  备注
    时间\t测试项\t实测值\t单位\tMin\tTyp\tMax\t结果\t备注
    2026-10-21 15:00:01\tVBG_Voltage\t1250.123\tmV\t1235.0\t1250.0\t1265.0\tPASS\t修调 3

结果列取值：PASS / FAIL / NO-STD（无判定标准）/ MEASURED（只测量不判定）
"""

import os
import re
import re
import sys
import time
import unicodedata
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]          # bench_python_code/
SAVE_ROOT = ROOT / "Save_Results"                   # 所有产出物的根目录
RESULT_DIR = SAVE_ROOT / "measure_Results"          # 测试项结果（txt / xlsx / 测量数据）
MTP_DIR = SAVE_ROOT / "MTP_Table"                   # MTP 配置表 txt 与烧录模板 xlsx

COLUMNS = ["时间", "测试项", "实测值", "单位", "Min", "Typ", "Max", "结果", "备注"]

#: 结果列取值 -> 中文说明（打印/Excel 用）
RESULT_LABELS = {
    "PASS": "通过",
    "FAIL": "未通过",
    "NO-STD": "无判定标准",
    "MEASURED": "仅测量",
}

# 当前会话的 txt 路径；由 start_session() 设置，未设置时 record() 自动创建
_session = {"path": None, "chip": "", "count": 0}

# 数据行首列的时间格式（2026-10-21 16:00:00）
_TIME_RE = re.compile(r"^\d{4}-\d{2}-\d{2}[ T]\d{2}:\d{2}")


# =====================================================================
# 记录
# =====================================================================

def _fmt(value, digits=4):
    """数值格式化：None → 空串，float → 保留 4 位并去掉多余 0。"""
    if value is None:
        return ""
    if isinstance(value, str):
        return value
    try:
        text = f"{float(value):.{digits}f}"
    except (TypeError, ValueError):
        return str(value)
    return text.rstrip("0").rstrip(".") if "." in text else text


def start_session(chip_label="", prefix="测试结果"):
    """开始一次新的结果记录，返回 txt 路径（已写入文件头）。"""
    RESULT_DIR.mkdir(parents=True, exist_ok=True)
    stamp = time.strftime("%Y%m%d_%H%M%S")
    chip_part = f"_{chip_label}" if chip_label else ""
    path = RESULT_DIR / f"{prefix}{chip_part}_{stamp}.txt"

    with path.open("w", encoding="utf-8") as f:
        f.write(f"# 芯片: {chip_label or '-'}\n")
        f.write(f"# 记录开始: {time.strftime('%Y-%m-%d %H:%M:%S')}\n")
        f.write(f"# 列: {'  '.join(COLUMNS)}\n")
        f.write(f"# 结果取值: {', '.join(f'{k}={v}' for k, v in RESULT_LABELS.items())}\n")
        f.write("\t".join(COLUMNS) + "\n")

    _session.update(path=path, chip=chip_label, count=0)
    return path


def current_path():
    """当前记录文件路径（未开始记录返回 None）。"""
    return _session["path"]


def record(item, value=None, unit="", min_v=None, typ_v=None, max_v=None,
           result="MEASURED", note=""):
    """追加一条记录。

    参数：
        item:   str - 测试项名（与校准标准表 A 列一致，如 VBG_Voltage）
        value:  float/None - 实测值
        unit:   str - 实测值单位（mV / mA / Hz / Ω）
        min_v/typ_v/max_v: float/None - 判定范围（与 value 同单位）
        result: str - PASS / FAIL / NO-STD / MEASURED
        note:   str - 备注（修调值、异常原因等）
    返回：
        Path - 记录文件路径；写入失败返回 None（不影响测试本身）
    """
    try:
        if _session["path"] is None:
            start_session(_session["chip"])
        row = [
            time.strftime("%Y-%m-%d %H:%M:%S"),
            item,
            _fmt(value),
            unit or "",
            _fmt(min_v),
            _fmt(typ_v),
            _fmt(max_v),
            result,
            str(note).replace("\t", " ").replace("\n", " "),
        ]
        with Path(_session["path"]).open("a", encoding="utf-8") as f:
            f.write("\t".join(row) + "\n")
        _session["count"] += 1
        return Path(_session["path"])
    except Exception as e:
        print(f"   ⚠️ 测试结果记录失败（不影响测试）：{type(e).__name__}: {e}")
        return None


def session_summary():
    """返回 (路径, 条数)；未记录时返回 (None, 0)。"""
    return _session["path"], _session["count"]


# =====================================================================
# 批量执行后的结果汇总打印
# =====================================================================

#: 结果严重程度排序（数字越大越严重），汇总时取一组记录里最严重的那个
_RESULT_RANK = {"PASS": 0, "MEASURED": 1, "NO-STD": 2, "FAIL": 3}

#: 逐点 / 逐档扫描写入的记录名，汇总时合并成一项，避免刷屏。
#: (匹配逐点记录名的正则, 汇总后的显示名)
_POINT_FAMILIES = [
    (re.compile(r"^ADC_IN_\d+mV$"), "ADC_IN_扫描点"),
    (re.compile(r"^VBG_Voltage_trim\d+$"), "VBG 修调扫描"),
]
_FAMILY_NAMES = {name for _pattern, name in _POINT_FAMILIES}


def _family(item):
    """把逐点记录（ADC_IN_200mV、VBG_Voltage_trim3 …）归到同一个汇总项。"""
    for pattern, family in _POINT_FAMILIES:
        if pattern.match(item):
            return family
    return item


def _disp_width(text):
    """显示宽度：中日韩字符按 2 列算，用于终端对齐。"""
    return sum(2 if unicodedata.east_asian_width(ch) in ("W", "F") else 1
               for ch in str(text))


def _pad(text, width, align="left"):
    """按显示宽度补空格（中文占两列）。"""
    text = str(text)
    fill = max(0, width - _disp_width(text))
    return (" " * fill + text) if align == "right" else (text + " " * fill)


def _group_rows(rows):
    """把记录按测试项归组，保持首次出现顺序。

    返回：[{"item": 名, "rows": [row, ...]}, ...]
    """
    groups, index = [], {}
    for row in rows:
        key = _family(row["测试项"])
        if key not in index:
            index[key] = len(groups)
            groups.append({"item": key, "rows": []})
        groups[index[key]]["rows"].append(row)
    return groups


def _group_value_text(group_rows):
    """一组记录的实测值列：单条直接显示，多条显示 "a / b" 或范围。"""
    vals = [r["实测值"] for r in group_rows if r["实测值"]]
    if not vals:
        return "-"
    if len(vals) == 1:
        return vals[0]
    if len(vals) <= 4:
        return " / ".join(vals)
    try:
        nums = sorted(float(v) for v in vals)
        return f"{len(vals)} 点 {_fmt(nums[0])}~{_fmt(nums[-1])}"
    except ValueError:
        return f"{len(vals)} 点"


def _group_result_text(group_rows):
    """一组记录的结果列：结果一致时直接显示，不一致时显示最差 + 计数。"""
    results = [r["结果"] or "MEASURED" for r in group_rows]
    counts = {}
    for r in results:
        counts[r] = counts.get(r, 0) + 1
    worst = max(counts, key=lambda r: _RESULT_RANK.get(r, 0))
    if len(counts) == 1:
        return worst
    return f"{worst}({counts[worst]}/{len(results)})"


def summarize_since(start_index=0):
    """读取本次批量执行新追加的记录并汇总。

    参数：
        start_index: int - 批量开始前 session_summary() 的条数，
                     只统计这之后写入的记录
    返回：
        (rows, groups, stats) - rows: 新增记录；
                                groups: 按测试项归组后的列表；
                                stats: {"PASS": n, ...} 各结果计数
    """
    path, count = session_summary()
    if not path:
        return [], [], {}
    try:
        _meta, all_rows = read_log(path)
    except Exception:
        return [], [], {}
    rows = all_rows[max(0, int(start_index or 0)):count]
    stats = {}
    for r in rows:
        key = r["结果"] or "MEASURED"
        stats[key] = stats.get(key, 0) + 1
    return rows, _group_rows(rows), stats


def print_run_summary(start_index=0, title="测试结果汇总"):
    """批量执行结束后，按测试项打印本次运行的测量结果汇总。

    参数：
        start_index: int - 批量开始前 session_summary()[1]，只汇总之后的记录
        title:       str - 标题文字
    返回：
        True - 有汇总内容并已打印；False - 本次没有任何结果记录（不打印）
    """
    rows, groups, stats = summarize_since(start_index)
    if not groups:
        return False

    w_idx, w_item, w_cnt, w_val, w_unit, w_res = 4, 30, 5, 26, 6, 13
    print("\n" + "=" * (w_idx + w_item + w_cnt + w_val + w_unit + w_res + 6))
    print(f"📊 {title}（本次新记录 {len(rows)} 条，按测试项合并为 {len(groups)} 项）")
    print("=" * (w_idx + w_item + w_cnt + w_val + w_unit + w_res + 6))
    print(_pad("序号", w_idx, "right") + "  " + _pad("测试项", w_item)
          + _pad("条数", w_cnt, "right") + "  " + _pad("实测值", w_val, "right")
          + "  " + _pad("单位", w_unit) + _pad("结果", w_res))
    print("-" * (w_idx + w_item + w_cnt + w_val + w_unit + w_res + 6))

    fails = []
    for i, g in enumerate(groups, 1):
        grow = g["rows"]
        result_txt = _group_result_text(grow)
        units = sorted({r["单位"] for r in grow if r["单位"]})
        print(_pad(i, w_idx, "right") + "  " + _pad(g["item"], w_item)
              + _pad(len(grow), w_cnt, "right") + "  "
              + _pad(_group_value_text(grow), w_val, "right") + "  "
              + _pad("/".join(units) or "-", w_unit) + _pad(result_txt, w_res))
        if result_txt.startswith("FAIL"):
            fails.append(g)

    print("-" * (w_idx + w_item + w_cnt + w_val + w_unit + w_res + 6))
    parts = [f"{RESULT_LABELS.get(key, key)} {stats[key]}"
             for key in ("PASS", "FAIL", "NO-STD", "MEASURED") if stats.get(key)]
    print(f"   记录合计 {len(rows)} 条：" + "，".join(parts))

    if fails:
        print(f"\n   ❌ 未通过明细（{len(fails)} 项）：")
        for g in fails:
            for r in g["rows"]:
                if (r["结果"] or "") != "FAIL":
                    continue
                limits = ""
                if r["Min"] or r["Max"]:
                    limits = f"，标准 {r['Min'] or '-'} ~ {r['Max'] or '-'} {r['单位']}"
                elif r["Typ"]:
                    limits = f"，Typ {r['Typ']} {r['单位']}"
                note = f"，{r['备注']}" if r["备注"] else ""
                name = g["item"]
                if name in _FAMILY_NAMES:      # 逐点 / 逐档项带上具体的点
                    name = r["测试项"]
                print(f"      · {name}：{r['实测值'] or '-'} {r['单位']}"
                      f"{limits}{note}")
    print("=" * (w_idx + w_item + w_cnt + w_val + w_unit + w_res + 6))
    return True


# =====================================================================
# 读取 / 转换
# =====================================================================

def read_log(path):
    """解析结果 txt。

    返回：
        (meta, rows) - meta: {键: 值}（来自 # 开头的头部行）
                       rows: [dict]，键为 COLUMNS 中的列名
    """
    meta, rows = {}, []
    path = Path(path)
    with path.open("r", encoding="utf-8", errors="replace") as f:
        for line in f:
            line = line.rstrip("\n").rstrip("\r")
            if not line.strip():
                continue
            if line.startswith("#"):
                body = line.lstrip("#").strip()
                if ":" in body:
                    k, v = body.split(":", 1)
                    meta[k.strip()] = v.strip()
                continue
            parts = [p.strip() for p in line.split("\t")]
            if parts == COLUMNS:
                continue                      # 表头行
            # 数据行第一列必须是时间（2026-10-21 16:00:00），其余行一律忽略，
            # 避免把别的文本（表头变体、说明）当成数据
            if not _TIME_RE.match(parts[0]):
                continue
            if len(parts) < len(COLUMNS):     # 容错：补齐缺失列
                parts += [""] * (len(COLUMNS) - len(parts))
            rows.append(dict(zip(COLUMNS, parts[:len(COLUMNS)])))
    return meta, rows


def txt_to_excel(txt_path, xlsx_path=None):
    """把结果 txt 转成 Excel（两个工作表：明细 / 汇总）。

    返回：
        (ok, 路径或错误信息)
    """
    try:
        from openpyxl import Workbook
        from openpyxl.styles import Alignment, Font, PatternFill
        from openpyxl.utils import get_column_letter
    except Exception as e:
        return False, f"未安装 openpyxl：{e}"

    try:
        meta, rows = read_log(txt_path)
    except Exception as e:
        return False, f"读取失败：{type(e).__name__}: {e}"

    if not rows:
        return False, "文件中没有有效的数据行（可能选错了文件）"

    txt_path = Path(txt_path)
    if xlsx_path is None:
        xlsx_path = txt_path.with_suffix(".xlsx")
    xlsx_path = Path(xlsx_path)
    replaced = xlsx_path.exists()
    xlsx_path.parent.mkdir(parents=True, exist_ok=True)

    header_font = Font(bold=True, color="FFFFFF")
    header_fill = PatternFill("solid", fgColor="305496")
    pass_fill = PatternFill("solid", fgColor="E2EFDA")
    fail_fill = PatternFill("solid", fgColor="FFC7CE")
    other_fill = PatternFill("solid", fgColor="FFF2CC")

    try:
        wb = Workbook()

        # ---------- 明细 ----------
        ws = wb.active
        ws.title = "测试结果"
        ws.append(COLUMNS)
        for col in range(1, len(COLUMNS) + 1):
            cell = ws.cell(row=1, column=col)
            cell.font = header_font
            cell.fill = header_fill
            cell.alignment = Alignment(horizontal="center", vertical="center")

        for row in rows:
            values = []
            for key in COLUMNS:
                v = row.get(key, "")
                if key == "实测值" or key in ("Min", "Typ", "Max"):
                    try:
                        v = float(v) if v not in ("", None) else ""
                    except ValueError:
                        pass
                if key == "结果":
                    v = RESULT_LABELS.get(v, v)
                values.append(v)
            ws.append(values)

            result_key = row.get("结果", "")
            fill = pass_fill if result_key == "PASS" else (
                fail_fill if result_key == "FAIL" else
                other_fill if result_key in ("NO-STD", "MEASURED") else None)
            if fill is not None:
                for col in range(1, len(COLUMNS) + 1):
                    ws.cell(row=ws.max_row, column=col).fill = fill

        ws.freeze_panes = "A2"
        ws.auto_filter.ref = ws.dimensions
        widths = [20, 26, 12, 8, 12, 12, 12, 10, 30]
        for i, w in enumerate(widths[:len(COLUMNS)], start=1):
            ws.column_dimensions[get_column_letter(i)].width = w

        # ---------- 汇总 ----------
        ws2 = wb.create_sheet("汇总")
        ws2.append(["芯片", meta.get("芯片", "-")])
        ws2.append(["记录开始", meta.get("记录开始", "-")])
        ws2.append(["转换时间", time.strftime("%Y-%m-%d %H:%M:%S")])
        ws2.append(["源文件", txt_path.name])
        ws2.append([])

        total = len(rows)
        counts = {}
        for row in rows:
            counts[row.get("结果", "")] = counts.get(row.get("结果", ""), 0) + 1
        ws2.append(["总条数", total])
        for key in ("PASS", "FAIL", "NO-STD", "MEASURED"):
            if counts.get(key):
                ws2.append([RESULT_LABELS[key], counts[key]])
        ws2.append([])

        # 每个测试项最后一次结果（倒序遍历，首次遇到即最新）
        ws2.append(["测试项", "最后结果", "实测值", "单位", "判定范围", "时间", "备注"])
        for col in range(1, 8):
            cell = ws2.cell(row=ws2.max_row, column=col)
            cell.font = header_font
            cell.fill = header_fill
        order, last = [], {}
        for row in rows:                      # 顺序按首次出现，结果取最后一次
            name = row["测试项"]
            if name not in last:
                order.append(name)
            last[name] = row
        for name in order:
            row = last[name]
            lo, hi = row.get("Min", ""), row.get("Max", "")
            rng = f"{lo} ~ {hi}" if lo or hi else "-"
            ws2.append([
                row["测试项"],
                RESULT_LABELS.get(row.get("结果", ""), row.get("结果", "")),
                row.get("实测值", ""),
                row.get("单位", ""),
                rng,
                row.get("时间", ""),
                row.get("备注", ""),
            ])
        for i, w in enumerate([26, 12, 12, 8, 22, 20, 30], start=1):
            ws2.column_dimensions[get_column_letter(i)].width = w

        wb.save(xlsx_path)
        if replaced:
            print(f"   （已覆盖同名文件 {xlsx_path.name}）")
        return True, str(xlsx_path)
    except Exception as e:
        return False, f"写入 Excel 失败：{type(e).__name__}: {e}"


def pick_txt(default_dir=None, title="选择 txt（测试结果 / MTP 配置表）"):
    """弹出文件选择框，返回所选 txt 路径；取消返回 None。"""
    try:
        import tkinter as tk
        from tkinter import filedialog
    except Exception as e:
        print(f"   ❌ 无法打开文件选择框（{type(e).__name__}: {e}）")
        return None

    start_dir = Path(default_dir) if default_dir else RESULT_DIR
    if not start_dir.exists():
        start_dir = ROOT

    root = tk.Tk()
    root.withdraw()
    root.attributes("-topmost", True)
    try:
        path = filedialog.askopenfilename(
            title=title,
            initialdir=str(start_dir),
            filetypes=[("文本文件", "*.txt"), ("所有文件", "*.*")],
        )
    finally:
        root.destroy()
    return path or None


def pick_txt_and_convert(default_dir=None):
    """弹出文件选择框，选一个结果 txt 转成 Excel。返回 (ok, 消息)。"""
    path = pick_txt(default_dir, title="选择测试结果 txt（转为 Excel）")
    if not path:
        return False, "未选择文件，已取消"
    return txt_to_excel(path)


# =====================================================================
# 命令行入口（可选）
# =====================================================================

if __name__ == "__main__":
    if os.name == "nt":
        os.system("chcp 65001 > nul")
        sys.stdout.reconfigure(encoding="utf-8")

    if len(sys.argv) < 2:
        print("用法: python instrument/result_log.py <结果.txt> [输出.xlsx]")
        sys.exit(2)
    ok, msg = txt_to_excel(sys.argv[1], sys.argv[2] if len(sys.argv) > 2 else None)
    print(("✅ " if ok else "❌ ") + msg)
    sys.exit(0 if ok else 1)
