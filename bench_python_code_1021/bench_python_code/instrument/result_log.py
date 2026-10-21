# -*- coding: utf-8 -*-
"""
测试结果记录（txt）与 txt → Excel 转换

用途
----
1. 测试过程中把每一项的 **实测值 + 判定范围（Min/Typ/Max）+ 结果** 追加写入
   `Save_Results/测试结果_<芯片>_<时间>.txt`（制表符分隔，边测边写，异常退出也不丢数据）；
2. 菜单里选择该 txt，转换成排好版的 `.xlsx`（含汇总页）。

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
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]          # bench_python_code/
RESULT_DIR = ROOT / "Save_Results"

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
