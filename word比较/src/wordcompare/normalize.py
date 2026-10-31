"""文本归一化。

归一化只作用于 ``norm`` 文本（用于哈希与相似度），不会改动展示文本，
因此报告中看到的仍然是原文。
"""

from __future__ import annotations

import re
import unicodedata

from .options import Options
from .model import Block

# 全角字母数字 → 半角（保留全角标点，标点是否等价由独立开关控制）
_WIDTH_TABLE = {}
for _i in range(10):
    _WIDTH_TABLE[ord("０") + _i] = ord("0") + _i
for _i in range(26):
    _WIDTH_TABLE[ord("Ａ") + _i] = ord("A") + _i
    _WIDTH_TABLE[ord("ａ") + _i] = ord("a") + _i
_WIDTH_TABLE[ord("　")] = ord(" ")

# 中文标点 → 英文标点（默认关闭）
_PUNCT_TABLE = {
    ord("，"): ",", ord("。"): ".", ord("；"): ";", ord("："): ":",
    ord("？"): "?", ord("！"): "!", ord("（"): "(", ord("）"): ")",
    ord("【"): "[", ord("】"): "]", ord("“"): '"', ord("”"): '"',
    ord("‘"): "'", ord("’"): "'", ord("、"): ",", ord("—"): "-",
    ord("…"): "...", ord("《"): "<", ord("》"): ">",
}

_WS = re.compile(r"\s+")


def normalize_text(text: str, opts: Options) -> str:
    if not text:
        return ""
    if opts.normalize_width:
        text = text.translate(_WIDTH_TABLE)
    if opts.normalize_punctuation:
        text = text.translate(_PUNCT_TABLE)
    if opts.ignore_whitespace:
        text = _WS.sub(" ", text).strip()
    if opts.ignore_case:
        text = text.lower()
    # 统一 Unicode 组合形式，避免"é"两种编码被判为不同
    return unicodedata.normalize("NFC", text)


def assign_norms(blocks: list[Block], opts: Options) -> None:
    """在读取阶段为块填充 norm / hash（表格递归到单元格）。"""
    from .align.fingerprint import text_hash

    for block in blocks:
        if block.kind == "paragraph":
            block.norm = normalize_text(block.text, opts)
            block.hash = text_hash("p:" + block.norm)
        else:
            for row in block.rows:
                for cell in row:
                    if cell.vmerge_continue:
                        continue
                    assign_norms(cell.blocks, opts)
            block.norm = normalize_text(block.text, opts)
            block.hash = text_hash("t:" + block.norm)
