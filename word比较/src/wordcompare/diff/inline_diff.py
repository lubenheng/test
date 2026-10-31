"""行内 diff：字符级 + 语义清理。"""

from __future__ import annotations

from difflib import SequenceMatcher

from ..model import Segment

try:
    from diff_match_patch import diff_match_patch as _DMP
except ImportError:  # pragma: no cover
    _DMP = None

_OP = {0: "equal", -1: "del", 1: "ins"}


def inline_diff(left: str, right: str) -> list[Segment]:
    if left == right:
        return [Segment("equal", left)] if left else []

    segments: list[Segment] = []
    if _DMP is not None and len(left) + len(right) <= 200_000:
        try:
            dmp = _DMP()
            diffs = dmp.diff_main(left, right, checklines=False)
            dmp.diff_cleanupSemantic(diffs)   # 合并碎片，避免逐字的红绿噪声
            dmp.diff_cleanupEfficiency(diffs)
            segments = [Segment(_OP[op], text) for op, text in diffs if text]
        except Exception:  # noqa: BLE001 - 极端输入下回退到 difflib
            segments = []
    if not segments:
        segments = _fallback(left, right)

    return _merge(segments)


def _fallback(left: str, right: str) -> list[Segment]:
    segs: list[Segment] = []
    matcher = SequenceMatcher(None, left, right, autojunk=False)
    for tag, i1, i2, j1, j2 in matcher.get_opcodes():
        if tag == "equal":
            segs.append(Segment("equal", left[i1:i2]))
        elif tag == "delete":
            segs.append(Segment("del", left[i1:i2]))
        elif tag == "insert":
            segs.append(Segment("ins", right[j1:j2]))
        else:
            segs.append(Segment("del", left[i1:i2]))
            segs.append(Segment("ins", right[j1:j2]))
    return segs


def _merge(segments: list[Segment]) -> list[Segment]:
    merged: list[Segment] = []
    for seg in segments:
        if not seg.text:
            continue
        if merged and merged[-1].op == seg.op:
            merged[-1].text += seg.text
        else:
            merged.append(seg)
    return merged


def has_changes(segments: list[Segment]) -> bool:
    return any(s.op != "equal" for s in segments)
