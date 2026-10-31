"""格式 diff：比较 run 属性与段落属性。

只在文字相等的位置比较 run 格式，避免和"文字修改"重复计数。
"""

from __future__ import annotations

from ..model import FormatChange, Paragraph, Run, Segment
from ..options import Options

PROP_LABELS = {
    "bold": "加粗",
    "italic": "斜体",
    "underline": "下划线",
    "strike": "删除线",
    "font": "字体",
    "size": "字号",
    "color": "颜色",
    "highlight": "底纹",
    "alignment": "对齐方式",
    "style": "段落样式",
    "indent": "左缩进",
    "numbering": "编号层级",
}


def _fmt_value(prop: str, value) -> str:
    if value is None:
        return "继承"
    if isinstance(value, bool):
        return "开" if value else "关"
    if prop == "size":
        return f"{value:g} 磅"
    if prop == "indent":
        return f"{value:g} 磅"
    return str(value)


def _char_runs(para: Paragraph) -> list[Run]:
    out: list[Run] = []
    for run in para.runs:
        out.extend([run] * len(run.text))
    return out


def compare_run_format(left: Paragraph, right: Paragraph,
                       segments: list[Segment], opts: Options) -> list[FormatChange]:
    if not opts.format_diff:
        return []
    lmap, rmap = _char_runs(left), _char_runs(right)
    li = ri = 0
    buckets: dict[tuple[str, str, str], FormatChange] = {}

    for seg in segments:
        size = len(seg.text)
        if seg.op == "del":
            li += size
            continue
        if seg.op == "ins":
            ri += size
            continue

        for k in range(size):
            lp = lmap[li + k].props() if li + k < len(lmap) else None
            rp = rmap[ri + k].props() if ri + k < len(rmap) else None
            if lp is None or rp is None:
                continue
            for prop, lval in lp.items():
                rval = rp[prop]
                if lval == rval:
                    continue
                if not opts.report_inherited and (lval is None or rval is None):
                    continue
                lstr, rstr = _fmt_value(prop, lval), _fmt_value(prop, rval)
                key = (prop, lstr, rstr)
                if key in buckets:
                    buckets[key].count += 1
                else:
                    sample = seg.text[max(0, k - 3): k + 8].strip()
                    buckets[key] = FormatChange(
                        scope="文字", prop=prop,
                        old=lstr, new=rstr, sample=sample,
                    )
        li += size
        ri += size

    return list(buckets.values())


def compare_paragraph_format(left: Paragraph, right: Paragraph,
                             opts: Options) -> list[FormatChange]:
    if not opts.format_diff:
        return []
    changes: list[FormatChange] = []

    def add(prop: str, lval, rval) -> None:
        if lval == rval:
            return
        if not opts.report_inherited and (lval is None or rval is None):
            return
        changes.append(FormatChange(
            scope="段落", prop=prop,
            old=_fmt_value(prop, lval), new=_fmt_value(prop, rval),
        ))

    add("alignment", left.alignment, right.alignment)
    add("style", left.style or "正文", right.style or "正文")
    add("numbering", left.ilvl if left.num_id else None,
        right.ilvl if right.num_id else None)
    if (left.indent_left is not None or right.indent_left is not None) \
            and abs((left.indent_left or 0) - (right.indent_left or 0)) > 0.5:
        add("indent", left.indent_left or 0, right.indent_left or 0)
    if left.images != right.images:
        changes.append(FormatChange(
            scope="段落", prop="style",
            old=f"{len(left.images)} 张图片", new=f"{len(right.images)} 张图片",
        ))
    return changes


def summarize(changes: list[FormatChange]) -> str:
    parts = []
    for ch in changes:
        label = PROP_LABELS.get(ch.prop, ch.prop)
        suffix = f" ×{ch.count}" if ch.count > 1 else ""
        parts.append(f"{ch.scope}{label}: {ch.old}→{ch.new}{suffix}")
    return "；".join(parts)


def format_signature(para: Paragraph) -> tuple:
    """廉价指纹：用于跳过"文字相同且格式显然相同"的段落。

    相邻且属性相同的 run 先合并，避免同一格式被切成多个 run 造成误判。
    """
    merged: list[tuple] = []
    for run in para.runs:
        key = tuple(sorted(run.props().items()))
        if merged and merged[-1][0] == key:
            merged[-1] = (key, merged[-1][1] + len(run.text))
        else:
            merged.append((key, len(run.text)))
    return (
        tuple(merged),
        para.alignment,
        para.style,
        para.ilvl if para.num_id else None,
        round(para.indent_left or -1, 1),
        tuple(para.images),
    )
