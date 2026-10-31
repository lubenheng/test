"""指纹与相似度。"""

from __future__ import annotations

import hashlib
import re
from collections import Counter
from difflib import SequenceMatcher

try:  # 可选加速
    from rapidfuzz import fuzz as _rf_fuzz
except ImportError:  # pragma: no cover
    _rf_fuzz = None

_CJK = "㐀-䶿一-鿿豈-﫿"
_TOKEN_RE = re.compile(rf"[{_CJK}]+|[A-Za-z0-9]+|[^\s]")


def text_hash(text: str) -> str:
    return hashlib.sha1(text.encode("utf-8")).hexdigest()[:16]


def tokenize(text: str) -> list[str]:
    """中文按字符二元组，英文按单词，标点按单字。"""
    tokens: list[str] = []
    for match in _TOKEN_RE.finditer(text):
        chunk = match.group()
        if re.fullmatch(rf"[{_CJK}]+", chunk):
            if len(chunk) == 1:
                tokens.append(chunk)
            else:
                tokens.extend(chunk[i:i + 2] for i in range(len(chunk) - 1))
        elif chunk.isalnum() and chunk.isascii():
            tokens.append(chunk.lower())
        else:
            tokens.append(chunk)
    return tokens


def _dice(a: str, b: str) -> float:
    ca, cb = Counter(tokenize(a)), Counter(tokenize(b))
    denom = sum(ca.values()) + sum(cb.values())
    if not denom:
        return 1.0 if a == b else 0.0
    return 2.0 * sum((ca & cb).values()) / denom


def similarity(a: str, b: str, fast: bool = False) -> float:
    """返回 [0, 1]。fast=True 时只做词袋比较（用于长表格）。"""
    if a == b:
        return 1.0
    if not a or not b:
        return 0.0
    dice = _dice(a, b)
    if fast:
        return dice
    if _rf_fuzz is not None:
        ratio = _rf_fuzz.ratio(a, b) / 100.0
    else:  # pragma: no cover
        ratio = SequenceMatcher(None, a, b, autojunk=False).ratio()
    # 词袋抓"改了哪些词"，序列比抓"结构像不像"，两者互补
    return 0.5 * dice + 0.5 * ratio
