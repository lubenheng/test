# -*- coding: utf-8 -*-
"""CRC_Test.py —— MTP 批量模板 CRC 校验工具（以 crc.py 为准）

用途
    选择一个 MTP 批量操作 Excel 模板（.xlsx），计算该模板**将被写入芯片**的
    32 位 CRC 校验码，并与模板里 `PROGRAM(CRC)` 行声明值比对。

口径（依据现场控制台工具 + 已确认的工程约定）
    · 算法   : crc.py 的 calculate_crc32_iter
               poly=0x04C11DB7, init=0xFFFFFFFF, LSB 优先, xorout=0
    · 范围   : **只计入 ADDR + INDEX*4 < 416 的行**（即 phys < 416）。
               phys >= 416 的行仍会被烧录，但不参与 CRC。
               计入的字节按地址升序去重后拼成字节流。
    · CRC 区 : 结果写入地址 507~510，小端
               [507]=crc&0xFF, [508]=crc>>8, [509]=crc>>16, [510]=crc>>24
    · 存表   : 模板中用一行 `PROGRAM(CRC)`（ADDR=507, INDEX=0）记录该值，
               D~G 四列 = [31:24][23:16][15:8][7:0]

    校验值以上述口径为准（★ 主输出）。另外并列给出若干**仅供诊断**的口径：
    不设地址上限的全部 PROGRAM 行、4~506 全区间（未定义字节按 0x00 / 0xFF 推算）、
    旧 UI 硬编码的 4~315 —— 便于交叉判断。

边界
    · 只做纯计算，不加载 USB2UARTSPIIICDLL.dll、不打开串口、不修改任何文件
    · 需要与脚本同目录的 crc.py / iic_ops.py / mtp_batch_ops.py

用法
    python CRC_Test.py                    直接弹出文件选择框（GUI）
    python CRC_Test.py <模板.xlsx>         指定文件，仍以 GUI 展示
    python CRC_Test.py <模板.xlsx> --cli   不启动 GUI，报告打印到标准输出
    python CRC_Test.py <模板.xlsx> --cli --detail   附 512 字节 hex 镜像
"""

import os
import re
import sys
import time

_HERE = os.path.dirname(os.path.abspath(__file__))
if _HERE not in sys.path:
    sys.path.insert(0, _HERE)

from openpyxl import load_workbook

from crc import calculate_crc32_iter
from iic_ops import IICLogic

try:
    # 复用生产路径的解析函数，保证"工具算出的值 == 烧录器写进芯片的值"
    from mtp_batch_ops import MTPBatchLogic
    _HAS_BATCH = True
except Exception:                                    # 缺依赖时降级为纯本地解析
    MTPBatchLogic = None
    _HAS_BATCH = False


# ==================== 常量（与生产逻辑 / 现场工具对齐） ====================

SHEET_PREFERRED = "MTP批量模板"
EXPECTED_HEADERS = ["OPERATION", "ADDR(基地址)", "INDEX(Word偏移)",
                    "[31:24]", "[23:16]", "[15:8]", "[7:0]", "物理地址(计算)"]

MTP_SIZE = 512

# —— ★ CRC 计入范围上限：只计入 ADDR + INDEX*4 < 416 的行 ——
# 即 phys >= 416 的行仍会被烧录，但不参与 CRC 计算（数据区上限为地址 415）
CRC_ADDR_LIMIT = 416

# —— CRC 校验范围：与 MTP 控制台命令 `crc 4 506` 一致 ——
CRC_LO = 4
CRC_HI = 506
CRC_LEN = CRC_HI - CRC_LO + 1          # 503

# —— CRC 结果保存区 ——
CRC_RSV_LO = 507
CRC_RSV_HI = 510

# —— 擦除态字节（`bp` 批量烧录先擦除扇区，未写到的字节即为该值）——
ERASE_FILL = 0xFF

# —— 旧 IICtool_v6 GUI 的固定回读区间，仅作对照 ——
LEGACY_LO = 4
LEGACY_HI = 315

# 写入 4 字节需要 phys+3 <= 511
PHYS_MAX_SAFE = 508

# PROGRAM(CRC) 行识别：操作名含 CRC
_CRC_OP_RE = re.compile(r"CRC", re.IGNORECASE)


# ==================== 工具函数 ====================

def _fmt(v):
    return "0x%08X" % (v & 0xFFFFFFFF)


def _compress(sorted_addrs):
    """把升序地址列表压缩成区间串，如 '4~315' / '4~7, 12~15'。"""
    if not sorted_addrs:
        return "（无）"
    parts = []
    start = prev = sorted_addrs[0]
    for a in sorted_addrs[1:]:
        if a == prev + 1:
            prev = a
            continue
        parts.append("%s~%s" % (start, prev) if start != prev else "%s" % start)
        start = prev = a
    parts.append("%s~%s" % (start, prev) if start != prev else "%s" % start)
    return ", ".join(parts)


def _compress_rows(rows):
    """把行号列表压缩成 '86~91, 95~97' 形式。"""
    return _compress(sorted(set(rows)))


def _pick_sheet(wb):
    """工作表选择规则，与 mtp_full_program_with_crc_gui 一致。"""
    if len(wb.sheetnames) > 1 and SHEET_PREFERRED in wb.sheetnames:
        return wb[SHEET_PREFERRED]
    return wb.active


def _crc(data):
    """唯一权威算法：crc.py 的 LSB 优先实现。"""
    return calculate_crc32_iter(bytes(data))[0]


# ==================== 模板扫描（独立实现，绝不抛异常） ====================

def scan_template(filename):
    """扫描模板，构建 512 字节镜像并记录逐行问题。

    与生产逻辑的差异：本函数把每个异常都转成带 Excel 行号的记录，不中断。
    """
    res = {
        "ok": False, "error": None, "sheet": "", "headers": [],
        "words": [],            # [(excel_row, phys, [b31,b23,b15,b7])]
        "crc_rows": [],         # [(excel_row, addr, [b31,b23,b15,b7])]
        "first_blank": None,    # 首个 A/B/C 全空的行号
        "op_counts": {}, "issues": [], "notes": [],
    }

    try:
        wb = load_workbook(filename, data_only=True)
    except Exception as e:
        res["error"] = "无法打开 Excel 文件：%s" % e
        return res

    try:
        ws = _pick_sheet(wb)
        res["sheet"] = ws.title

        try:
            hdr = []
            for c in range(1, 9):
                v = ws.cell(row=1, column=c).value
                hdr.append(str(v).strip() if v is not None else "")
            res["headers"] = hdr
            if hdr[:7] != EXPECTED_HEADERS[:7]:
                res["notes"].append(
                    "表头与标准模板不一致（仅提示，不影响计算）\n"
                    "      期望前7列: %s\n      实际: %s"
                    % (EXPECTED_HEADERS[:7], hdr[:7]))
        except Exception as e:
            res["notes"].append("表头读取失败（不影响计算）：%s" % e)

        # 空行/注释行跳过但**不提前结束**：真实模板里 CRC 行前常有空行
        # （与现场工具 tests.py 的 _mtp_parse_excel_file 行为一致；
        #   而 mtp_batch_ops 的读取流程遇空行会 break，会漏掉空行之后的数据）
        blank_rows = []
        for r in range(3, ws.max_row + 1):
            row = [ws.cell(row=r, column=c).value for c in range(1, 8)]
            if all(c is None or str(c).strip() == "" for c in row[:3]):
                blank_rows.append(r)
                if res["first_blank"] is None:
                    res["first_blank"] = r
                continue

            op = str(row[0]).strip().upper() if row[0] is not None else ""
            if not op or op.startswith("#") or op.startswith("//"):
                continue

            res["op_counts"][op] = res["op_counts"].get(op, 0) + 1

            is_prog = (op == "PROGRAM")
            is_crc = _CRC_OP_RE.search(op) is not None

            if not (is_prog or is_crc):
                continue

            try:
                addr_mod = int(row[1]) if row[1] is not None else 0
                index_mod = int(row[2]) if row[2] is not None else 0
            except Exception as e:
                res["issues"].append((r, "错误", "ADDR/INDEX 不是整数：%s" % e))
                continue

            phys = addr_mod + (index_mod * 4)

            if is_crc:
                if not (0 <= phys <= MTP_SIZE - 1):
                    res["issues"].append(
                        (r, "警告", "CRC 行地址 %d 越界，忽略" % phys))
                    continue
                try:
                    d = [IICLogic.parse_hex_byte(row[k], EXPECTED_HEADERS[k])
                         for k in (3, 4, 5, 6)]
                except Exception as e:
                    res["issues"].append((r, "错误", "CRC 行 HEX 解析失败：%s" % e))
                    continue
                res["crc_rows"].append((r, phys, d))
                continue

            # ---- PROGRAM 行 ----
            if phys < 0:
                res["issues"].append(
                    (r, "跳过", "物理地址 %d < 0（ADDR=%d, INDEX=%d）"
                     % (phys, addr_mod, index_mod)))
                continue
            if phys > MTP_SIZE - 1:
                res["issues"].append(
                    (r, "跳过", "物理地址 %d > 511，超出 MTP 范围（生产逻辑静默跳过）" % phys))
                continue
            if phys > PHYS_MAX_SAFE:
                res["issues"].append(
                    (r, "致命", "物理地址 %d 处于 509~511 越界区间："
                                "buffer[%d] 会 IndexError，全片烧录将直接失败"
                     % (phys, phys + 3)))
                continue

            try:
                d = [IICLogic.parse_hex_byte(row[k], EXPECTED_HEADERS[k])
                     for k in (3, 4, 5, 6)]
            except Exception as e:
                res["issues"].append((r, "错误", "HEX 解析失败：%s" % e))
                continue

            res["words"].append((r, phys, d))

        if blank_rows:
            res["notes"].append(
                "空行（已跳过，不视为数据块结束）：%s"
                % ", ".join(str(x) for x in blank_rows[:20])
                + (" …" if len(blank_rows) > 20 else ""))

        res["ok"] = True
        return res
    except Exception as e:
        res["error"] = "扫描过程中出现未预期异常：%s" % e
        return res


def build_buffer(words, fill):
    """按 512 字节镜像布局写入 PROGRAM 数据。

    D([31:24]) → phys+3, E([23:16]) → phys+2, F([15:8]) → phys+1, G([7:0]) → phys+0
    （小端：低地址存低字节）
    """
    buf = bytearray([fill] * MTP_SIZE)
    for _row, phys, d in words:
        b31, b23, b15, b7 = d
        buf[phys + 0] = b7
        buf[phys + 1] = b15
        buf[phys + 2] = b23
        buf[phys + 3] = b31
    return buf


# ==================== 计算 ====================

def compute(path):
    scan = scan_template(path)
    t0 = time.time()

    out = {
        "path": os.path.abspath(path),
        "scan": scan,
        "crc_burn": None,         # ★ 主输出：ADDR+INDEX*4 < 408 的行，升序去重
        "crc_nolimit": None,      # 诊断：不加地址上限（全部 PROGRAM 行）
        "crc_506_ff": None,       # 诊断：4~506 全区间，未定义 = 0xFF
        "crc_506_00": None,       # 诊断：4~506 全区间，未定义 = 0x00
        "crc_declared": None,     # 诊断：全部 PROGRAM 行（含空行之后）
        "crc_legacy": None,       # 诊断：旧 GUI 口径 4~315
        "declared_crc": None,     # 模板 PROGRAM(CRC) 行声明值
        "declared_addr": None,
        "declared_row": None,
        "declared_raw": None,     # "0F 34 8C FA"
        "defined": [],
        "undefined": [],
        "declared_all": [],
        "limited": [],
        "lim_word_cnt": 0,
        "lim_bytes": 0,
        "lim_addr_lo": None,
        "lim_addr_hi": None,
        "exc_limit_rows": [],
        "exc_limit_addr_range": "",
        "preblank_rows": None,
        "preblank_lo": None,
        "preblank_word_cnt": 0,
        "postblank_rows": [],
        "postblank_addr_range": "",
        "prod_msg": "",
        "prod_crc": None,
        "fatal": False,
        "elapsed": 0.0,
    }

    if not scan["ok"]:
        out["elapsed"] = time.time() - t0
        return out

    words = scan["words"]

    # ================= ★ 主输出：计入 CRC 的行 =================
    # 规则：只计入 ADDR + INDEX*4 < CRC_ADDR_LIMIT（408）的行；
    #       字节按地址升序去重后拼接，算法用 crc.py。
    limited = [w for w in words if w[1] < CRC_ADDR_LIMIT]
    excluded_limit = [w for w in words if w[1] >= CRC_ADDR_LIMIT]

    out["lim_word_cnt"] = len(limited)
    out["exc_limit_rows"] = [w[0] for w in excluded_limit]
    exc_addrs = sorted({a for _r, p, _d in excluded_limit
                        for a in (p, p + 1, p + 2, p + 3) if 0 <= a <= MTP_SIZE - 1})
    out["exc_limit_addr_range"] = _compress(exc_addrs) if exc_addrs else ""

    lim_addrs = sorted({a for _r, p, _d in limited
                        for a in (p, p + 1, p + 2, p + 3) if 0 <= a <= MTP_SIZE - 1})
    if lim_addrs:
        buf_lim = build_buffer(limited, ERASE_FILL)
        out["crc_burn"] = _crc(bytes(buf_lim[a] for a in lim_addrs))
        out["lim_bytes"] = len(lim_addrs)
        out["lim_addr_lo"] = lim_addrs[0]
        out["lim_addr_hi"] = lim_addrs[-1]
    out["limited"] = lim_addrs

    # ================= 以下均为诊断口径 =================
    # 不限地址上限、全部 PROGRAM 行（首个空行之前）—— 即本次规则变更前的值
    fb = scan["first_blank"]
    pre = [w for w in words if fb is None or w[0] < fb] if fb else list(words)
    out["preblank_rows"] = (fb - 1) if fb else (words[-1][0] if words else None)
    out["preblank_lo"] = min((w[0] for w in pre), default=None)
    out["preblank_word_cnt"] = len(pre)
    pre_addrs = sorted({a for _r, p, _d in pre
                        for a in (p, p + 1, p + 2, p + 3) if 0 <= a <= MTP_SIZE - 1})
    if pre_addrs:
        buf_pre = build_buffer(pre, ERASE_FILL)
        out["crc_nolimit"] = _crc(bytes(buf_pre[a] for a in pre_addrs))

    buf_ff = build_buffer(words, ERASE_FILL)
    out["crc_506_ff"] = _crc(buf_ff[CRC_LO:CRC_HI + 1])
    buf_00 = build_buffer(words, 0x00)
    out["crc_506_00"] = _crc(buf_00[CRC_LO:CRC_HI + 1])

    # 全部 PROGRAM 行（含空行之后的），升序去重
    declared = set()
    for _row, phys, _d in words:
        declared.update([phys, phys + 1, phys + 2, phys + 3])
    declared &= set(range(MTP_SIZE))
    if declared:
        out["crc_declared"] = _crc(bytes(buf_ff[x] for x in sorted(declared)))

    # 旧 GUI 硬编码 4~315
    out["crc_legacy"] = _crc(buf_ff[LEGACY_LO:LEGACY_HI + 1])

    # 覆盖统计（在 4~506 区间内）
    rng = set(range(CRC_LO, CRC_HI + 1))
    out["declared_all"] = sorted(declared)
    out["defined"] = sorted(rng & declared)
    out["undefined"] = sorted(rng - declared)

    # 空行之后的、会被烧录但不计入 CRC 的行
    post = [w for w in words if fb is not None and w[0] > fb]
    out["postblank_rows"] = [w[0] for w in post]
    post_addrs = sorted({a for _r, p, _d in post for a in (p, p + 1, p + 2, p + 3)
                         if 0 <= a <= MTP_SIZE - 1})
    out["postblank_addr_range"] = (_compress(post_addrs) if post_addrs else "")

    # ---- 模板声明的 CRC ----
    if scan["crc_rows"]:
        row, addr, d = scan["crc_rows"][0]
        out["declared_row"] = row
        out["declared_addr"] = addr
        out["declared_crc"] = (d[0] << 24) | (d[1] << 16) | (d[2] << 8) | d[3]
        out["declared_raw"] = "%02X %02X %02X %02X" % tuple(d)

    # ---- 越界行 → 生产流程不可用 ----
    if any(lv == "致命" for _r, lv, _m in scan["issues"]):
        out["fatal"] = True

    # ---- 与生产函数的交叉核对（确认工具与烧录器同源） ----
    if _HAS_BATCH:
        try:
            ok, _b, phys_sorted, crc, err = \
                MTPBatchLogic.mtp_full_program_with_crc_gui(
                    path, None, None, None, IICLogic.parse_hex_byte, lambda _m: None)
            if ok:
                out["prod_crc"] = crc
                # phys_sorted 现为「烧录清单」= 模板声明的全部字节（含 >= CRC_ADDR_LIMIT）。
                # 其中计入 CRC 的只是 phys < CRC_ADDR_LIMIT 的部分。
                _ps = phys_sorted or []
                _in_crc = len([x for x in _ps if x < CRC_ADDR_LIMIT])
                _literal = len([x for x in _ps if x >= CRC_ADDR_LIMIT])
                out["prod_msg"] = ("生产 GUI 函数返回 %s（计入 CRC %d 字节: 声明行 ∩ phys<%d；"
                                   "另 %d 字节 phys>=%d 只烧录）"
                                   % (_fmt(crc), _in_crc, CRC_ADDR_LIMIT,
                                      _literal, CRC_ADDR_LIMIT))
                out["prod_crc_bytes"] = _in_crc
                out["prod_burn_bytes"] = len(_ps)
            else:
                out["prod_msg"] = "生产 GUI 函数报错：%s" % err
        except Exception as e:
            out["prod_msg"] = "生产 GUI 函数调用异常：%s" % e
    else:
        out["prod_msg"] = "未能导入 mtp_batch_ops，跳过交叉核对"

    out["elapsed"] = time.time() - t0
    return out


# ==================== 文本报告 ====================

def _dump_hex(buf, per_line=16):
    lines = []
    for base in range(0, MTP_SIZE, per_line):
        chunk = buf[base:base + per_line]
        lines.append("  %03X  %s" % (base, " ".join("%02X" % b for b in chunk)))
    return "\n".join(lines)


def build_report(path, include_detail=False):
    r = compute(path)
    s = r["scan"]
    L = []
    sep = "=" * 68
    thin = "-" * 68

    L.append(sep)
    L.append(" MTP 批量模板 CRC 校验报告")
    L.append(sep)
    L.append("文件      : %s" % r["path"])
    L.append("工作表    : %s" % (s["sheet"] or "（未读到）"))
    L.append("生成时间  : %s" % time.strftime("%Y-%m-%d %H:%M:%S"))
    L.append("算法      : crc.py / poly=0x04C11DB7 / init=0xFFFFFFFF / LSB 优先 / xorout=0")
    L.append("计入范围  : ADDR + INDEX*4 < %d 的行（即 phys < %d）" % (CRC_ADDR_LIMIT, CRC_ADDR_LIMIT))
    L.append("            phys >= %d 的行仍会被烧录，但不参与 CRC" % CRC_ADDR_LIMIT)
    L.append("")

    if not s["ok"]:
        L.append("!! 读取失败：%s" % s["error"])
        L.append(sep)
        return False, "\n".join(L)

    blocking = [i for i in s["issues"] if i[1] in ("致命", "错误")]

    # ---------- 校验码 ----------
    L.append(thin)
    L.append(" 校验码")
    L.append(thin)
    L.append("  ★ 模板 CRC32（计入 ADDR + INDEX*4 < %d 的行）" % CRC_ADDR_LIMIT)
    L.append("      %s    十进制 %d" % (_fmt(r["crc_burn"]), r["crc_burn"]))
    L.append("      参与计算 %d 个字节，地址 %d ~ %d，来自 %d 个 PROGRAM 行"
             % (r["lim_bytes"], r["lim_addr_lo"], r["lim_addr_hi"], r["lim_word_cnt"]))
    L.append("")
    L.append("    其他口径（仅供诊断，非本次口径）")
    L.append("      · 不设地址上限（全部 PROGRAM 行）            %s" % _fmt(r["crc_nolimit"]))
    L.append("      · %d~%d 全区间，未定义 = 0x%02X              %s"
             % (CRC_LO, CRC_HI, ERASE_FILL, _fmt(r["crc_506_ff"])))
    L.append("      · %d~%d 全区间，未定义 = 0x00               %s"
             % (CRC_LO, CRC_HI, _fmt(r["crc_506_00"])))
    L.append("      · 全部 PROGRAM 行（含空行之后，%d 字节）      %s"
             % (len(r["declared_all"]), _fmt(r["crc_declared"])))
    L.append("      · 地址 %d~%d（旧 IICtool_v6 硬编码口径）        %s"
             % (LEGACY_LO, LEGACY_HI, _fmt(r["crc_legacy"])))
    L.append("")

    if blocking or r["fatal"]:
        L.append("    ⚠️ 本模板存在 %d 处会导致生产流程中断的问题行（见下方逐行问题），"
                 % len(blocking))
        L.append("       以上 CRC 为**排除问题行后**的参考值，当前模板无法直接用于烧录。")
        L.append("")

    # ---------- 与模板声明值比对 ----------
    L.append(thin)
    L.append(" 模板声明的 CRC（PROGRAM(CRC) 行）")
    L.append(thin)
    if r["declared_crc"] is None:
        L.append("  模板中未找到 PROGRAM(CRC) 行（无声明值可比对）")
    else:
        L.append("  Excel 第 %d 行, ADDR=%d, D~G = %s"
                 % (r["declared_row"], r["declared_addr"], r["declared_raw"]))
        L.append("  声明值   : %s" % _fmt(r["declared_crc"]))
        L.append("  计算值   : %s" % _fmt(r["crc_burn"]))
        if r["declared_crc"] == r["crc_burn"]:
            L.append("  一致性   : ✅ 一致 —— 模板可复现该 CRC")
        else:
            d = r["declared_crc"] ^ r["crc_burn"]
            L.append("  一致性   : ❌ 不一致（异或 = %s）" % _fmt(d))
            L.append("             该行是**旧值**，建议更新为 %s" % _fmt(r["crc_burn"]))
    L.append("")

    # 落盘字节
    v = r["crc_burn"]
    L.append("  CRC 区落盘（按计算值，小端写地址 %d~%d）" % (CRC_RSV_LO, CRC_RSV_HI))
    L.append("      %02X %02X %02X %02X"
             % (v & 0xFF, (v >> 8) & 0xFF, (v >> 16) & 0xFF, (v >> 24) & 0xFF))
    L.append("      导出显示顺序: [31:24]=%02X [23:16]=%02X [15:8]=%02X [7:0]=%02X"
             % ((v >> 24) & 0xFF, (v >> 16) & 0xFF, (v >> 8) & 0xFF, v & 0xFF))
    L.append("")

    # ---------- 覆盖率诊断（关键） ----------
    L.append(thin)
    L.append(" 烧录范围与 CRC 覆盖")
    L.append(thin)
    L.append("  计入 CRC 的行        : %d 个 PROGRAM 行，%d 个字节（地址 %d ~ %d）"
             % (r["lim_word_cnt"], r["lim_bytes"], r["lim_addr_lo"], r["lim_addr_hi"]))
    if r["exc_limit_rows"]:
        L.append("  ⚠️ phys >= %d 而被排除的行 : 第 %s 行"
                 % (CRC_ADDR_LIMIT, _compress_rows(r["exc_limit_rows"])))
        L.append("     这些行地址 %s **会被烧录，但不计入 CRC**。"
                 % (r["exc_limit_addr_range"] or "（未知）"))
    else:
        L.append("  phys >= %d 而被排除的行      : 无" % CRC_ADDR_LIMIT)
    if r["postblank_rows"]:
        L.append("  ⚠️ 空行之后的 PROGRAM 行   : 第 %s 行"
                 % _compress_rows(r["postblank_rows"]))
        L.append("     这些行地址 %s 同样会被烧录，但不计入 CRC。"
                 % (r["postblank_addr_range"] or "（未知）"))
    else:
        L.append("  空行之后的 PROGRAM 行      : 无")
    L.append("")
    L.append("  参考：%d~%d 全区间共 %d 字节，其中模板已定义 %d、未定义 %d"
             % (CRC_LO, CRC_HI, CRC_LEN, len(r["defined"]), len(r["undefined"])))
    if r["undefined"]:
        L.append("        未定义区间 : %s" % _compress(r["undefined"]))
    L.append("")

    # ---------- 数据统计 ----------
    prog_cnt = s["op_counts"].get("PROGRAM", 0)
    others = {k: c for k, c in s["op_counts"].items() if k != "PROGRAM"}
    L.append(thin)
    L.append(" 数据统计")
    L.append(thin)
    L.append("  PROGRAM 行数        : %d" % prog_cnt)
    L.append("  PROGRAM 覆盖字节数  : %d（全 MTP 512 字节范围内）" % len(r["declared_all"]))
    if others:
        L.append("  其他 OPERATION      : %s（不参与 CRC）"
                 % ", ".join("%s×%d" % (k or "(空)", c) for k, c in sorted(others.items())))
    L.append("")

    # ---------- 交叉核对 ----------
    L.append(thin)
    L.append(" 与生产函数交叉核对")
    L.append(thin)
    L.append("  %s" % r["prod_msg"])
    if r["prod_crc"] is not None:
        if r["prod_crc"] == r["crc_burn"]:
            L.append("  ✅ 与本工具主输出一致 —— 该值即烧录器实际写入 %d~%d 的 CRC"
                     % (CRC_RSV_LO, CRC_RSV_HI))
        else:
            L.append("  ❌ 与本工具主输出不一致！")
            L.append("     工具=%s  生产=%s" % (_fmt(r["crc_burn"]), _fmt(r["prod_crc"])))
            L.append("     请检查本工具与生产逻辑的解析规则是否出现分歧")
    L.append("")

    # ---------- 逐行问题 ----------
    L.append(thin)
    L.append(" 逐行问题（Excel 行号为据）")
    L.append(thin)
    if not s["issues"]:
        L.append("  无")
    else:
        cnt = {}
        for _r, lv, _m in s["issues"]:
            cnt[lv] = cnt.get(lv, 0) + 1
        L.append("  " + " / ".join("%s %d 条" % (k, v) for k, v in
                                   sorted(cnt.items(), key=lambda x: x[0])) )
        L.append("")
        for lv in ("致命", "错误", "警告", "跳过"):
            items = [i for i in s["issues"] if i[1] == lv]
            for row_no, _lv, msg in items[:40]:
                L.append("  [%s] 第 %-4d 行  %s" % (lv, row_no, msg))
            if len(items) > 40:
                L.append("  [%s] …… 另有 %d 条未列出" % (lv, len(items) - 40))
    L.append("")

    if s["notes"]:
        L.append(thin)
        L.append(" 备注")
        L.append(thin)
        for n in s["notes"]:
            L.append("  · %s" % n)
        L.append("")

    if include_detail:
        L.append(thin)
        L.append(" 明细：512 字节镜像（未定义字节 = 0x%02X）" % ERASE_FILL)
        L.append(thin)
        L.append(_dump_hex(build_buffer(s["words"], ERASE_FILL)))
        L.append("")
        L.append("  已定义(在 CRC 范围内) : %s" % _compress(r["defined"]))
        L.append("  未定义(在 CRC 范围内) : %s" % _compress(r["undefined"]))
        L.append("")

    L.append(thin)
    L.append(" 结论：模板 CRC32 = %s" % _fmt(r["crc_burn"]))
    L.append("       范围 = ADDR + INDEX*4 < %d 的 %d 个 PROGRAM 行（%d 字节，地址 %d~%d）"
             % (CRC_ADDR_LIMIT, r["lim_word_cnt"], r["lim_bytes"],
                r["lim_addr_lo"], r["lim_addr_hi"]))
    L.append("       算法 = crc.py（poly=0x04C11DB7, init=0xFFFFFFFF, LSB 优先）")
    if r["declared_crc"] is not None and r["declared_crc"] != r["crc_burn"]:
        L.append("       ⚠️ 模板第 %d 行声明的 %s 与此不符"
                 % (r["declared_row"], _fmt(r["declared_crc"])))
    L.append("       [耗时 %.3f s]" % r["elapsed"])
    L.append(thin)

    return True, "\n".join(x for x in L if x is not None)


# ==================== GUI ====================

def run_gui(initial_path=None):
    import tkinter as tk
    from tkinter import ttk, filedialog, messagebox

    root = tk.Tk()
    root.title("MTP 批量模板 CRC 校验工具")
    root.geometry("940x660")

    state = {"path": initial_path}

    top = ttk.Frame(root, padding=6)
    top.pack(fill="x")
    var_detail = tk.BooleanVar(value=False)
    lbl_path = ttk.Label(top, text="（未选择文件）", anchor="w", foreground="#555")
    status = ttk.Label(root, text="就绪", anchor="w", relief="sunken", padding=(6, 2))

    body = ttk.Frame(root)
    body.pack(fill="both", expand=True, padx=6)
    txt = tk.Text(body, wrap="none", font=("Consolas", 10))
    ysb = ttk.Scrollbar(body, orient="vertical", command=txt.yview)
    xsb = ttk.Scrollbar(root, orient="horizontal", command=txt.xview)
    txt.configure(yscrollcommand=ysb.set, xscrollcommand=xsb.set)
    txt.grid(row=0, column=0, sticky="nsew")
    ysb.grid(row=0, column=1, sticky="ns")
    body.rowconfigure(0, weight=1)
    body.columnconfigure(0, weight=1)
    xsb.pack(fill="x", padx=6)
    status.pack(fill="x", side="bottom")

    def show(text):
        txt.configure(state="normal")
        txt.delete("1.0", "end")
        txt.insert("1.0", text)
        txt.configure(state="disabled")
        txt.see("1.0")

    def load_file(path):
        if not path:
            return
        state["path"] = path
        lbl_path.configure(text=path)
        status.configure(text="正在计算…")
        root.update_idletasks()
        try:
            ok, text = build_report(path, include_detail=var_detail.get())
        except Exception as e:
            ok, text = False, "!! 计算过程出现未预期异常：\n%s" % e
        show(text)
        status.configure(text=("完成" if ok else "失败") + "   %s" % path)

    def choose_file():
        path = filedialog.askopenfilename(
            title="选择 MTP 批量模板 Excel",
            filetypes=[("Excel 文件", "*.xlsx *.xlsm"), ("所有文件", "*.*")])
        if path:
            load_file(path)

    def toggle_detail():
        if state["path"]:
            load_file(state["path"])
        else:
            status.configure(text="请先选择文件")

    def copy_result():
        data = txt.get("1.0", "end-1c")
        if not data.strip():
            status.configure(text="没有可复制的内容")
            return
        root.clipboard_clear()
        root.clipboard_append(data)
        status.configure(text="已复制到剪贴板（%d 字符）" % len(data))

    def save_report():
        data = txt.get("1.0", "end-1c")
        if not data.strip():
            status.configure(text="没有可保存的内容")
            return
        base = os.path.splitext(os.path.basename(state["path"] or ""))[0]
        out = filedialog.asksaveasfilename(
            title="保存校验报告", defaultextension=".txt",
            initialfile="CRC_Report_%s_%s.txt" % (base or "template",
                                                 time.strftime("%Y%m%d_%H%M%S")),
            filetypes=[("文本文件", "*.txt")])
        if not out:
            return
        try:
            with open(out, "w", encoding="utf-8") as f:
                f.write(data)
            messagebox.showinfo("已保存", "报告已保存至：\n%s" % out)
            status.configure(text="已保存   %s" % out)
        except Exception as e:
            messagebox.showerror("保存失败", str(e))

    ttk.Button(top, text="选择模板文件", command=choose_file).pack(side="left")
    ttk.Button(top, text="复制结果", command=copy_result).pack(side="left", padx=(6, 0))
    ttk.Button(top, text="保存报告", command=save_report).pack(side="left", padx=(6, 0))
    ttk.Checkbutton(top, text="显示逐字节明细", variable=var_detail,
                    command=toggle_detail).pack(side="left", padx=(12, 0))
    ttk.Button(top, text="退出", command=root.destroy).pack(side="right")
    lbl_path.pack(fill="x", pady=(4, 0))

    if initial_path and os.path.isfile(initial_path):
        root.after(60, lambda: load_file(initial_path))
    else:
        root.after(150, choose_file)

    root.mainloop()


# ==================== 入口 ====================

def main(argv):
    path = None
    cli = False
    detail = False
    for a in argv[1:]:
        if a == "--cli":
            cli = True
        elif a == "--detail":
            detail = True
        elif a in ("-h", "--help"):
            print(__doc__)
            return 0
        else:
            path = a

    if cli:
        if not path:
            print("!! --cli 模式需要给出文件路径")
            return 2
        ok, text = build_report(path, include_detail=detail)
        try:
            sys.stdout.reconfigure(encoding="utf-8")
        except Exception:
            pass
        print(text)
        return 0 if ok else 1

    run_gui(path)
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
