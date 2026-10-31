"""比较引擎：编排读取 → 对齐 → 行内/格式 diff → 统计。"""

from __future__ import annotations

from pathlib import Path

from .align.block_align import align_blocks
from .diff.format_diff import (
    compare_paragraph_format,
    compare_run_format,
    format_signature,
)
from .diff.inline_diff import inline_diff
from .model import DiffItem, DiffResult, Paragraph, Stats
from .options import Options
from .reader.docx_reader import DocxReader


def compare(left_path: str | Path, right_path: str | Path,
            options: Options | None = None) -> DiffResult:
    opts = options or Options()
    left_path, right_path = Path(left_path), Path(right_path)

    left_reader = DocxReader(left_path, opts)
    right_reader = DocxReader(right_path, opts)
    left_blocks, left_title = left_reader.read()
    right_blocks, right_title = right_reader.read()

    items = align_blocks(left_blocks, right_blocks, opts)
    _enrich(items, opts)

    result = DiffResult(
        left_path=str(left_path),
        right_path=str(right_path),
        items=items,
        left_title=left_title,
        right_title=right_title,
        left_doc=left_reader.document,
        right_doc=right_reader.document,
    )
    result.stats = compute_stats(result)
    return result


def _enrich(items: list[DiffItem], opts: Options) -> None:
    """为 modify 项补上行内 diff 与格式变更，并应用"比较范围"开关。"""
    for item in items:
        left, right = item.left, item.right

        # 不比较文字时，纯新增/删除的段落不算差异
        if not opts.compare_text and item.kind in ("insert", "delete"):
            item.kind = "equal"
            continue

        if item.table is not None:
            if item.kind == "modify":
                for row_pair in item.table.row_pairs:
                    for cell_diff in row_pair.cells:
                        _enrich(cell_diff.items, opts)
                _refresh_table(item.table, opts)
                if item.table.row_pairs and all(
                        rp.kind == "equal" for rp in item.table.row_pairs):
                    item.kind = "equal"
            continue

        if not (isinstance(left, Paragraph) and isinstance(right, Paragraph)):
            continue

        # 不比较文字：文字不同就无从逐字对齐格式，整项跳过
        if not opts.compare_text and left.text != right.text:
            item.kind = "equal"
            item.inline = None
            item.format_changes = []
            continue

        # 文字相同、只有格式变化的段落：先做廉价指纹比对再精确分析
        if item.kind == "equal":
            if not (opts.format_diff
                    and format_signature(left) != format_signature(right)):
                continue
            segments = inline_diff(left.text, right.text)
            changes = (compare_run_format(left, right, segments, opts)
                       + compare_paragraph_format(left, right, opts))
            if changes:
                item.kind = "modify"
                item.inline = segments
                item.format_changes = changes
            continue

        if item.kind != "modify":
            continue
        segments = inline_diff(left.text, right.text)
        item.inline = segments
        item.format_changes = (
            compare_run_format(left, right, segments, opts)
            + compare_paragraph_format(left, right, opts)
        )
        if not any(s.op != "equal" for s in segments) and not item.format_changes:
            item.kind = "equal"


def _refresh_table(table, opts: Options) -> None:
    """单元格行内 diff 更新后，重新汇总"单元格→行"的变更状态。"""
    for row_pair in table.row_pairs:
        if not opts.compare_text and row_pair.kind in ("insert", "delete"):
            # 不比较文字时，整行增删不算差异
            row_pair.kind = "equal"
            continue
        if row_pair.kind != "modify":
            continue
        for cell_diff in row_pair.cells:
            if cell_diff.items and cell_diff.left is not None \
                    and cell_diff.right is not None:
                cell_diff.kind = ("modify"
                                  if any(it.changed for it in cell_diff.items)
                                  else "equal")
        if all(cell.kind == "equal" for cell in row_pair.cells):
            row_pair.kind = "equal"


def compute_stats(result: DiffResult) -> Stats:
    stats = Stats()
    stats.left_blocks = sum(1 for it in result.items if it.left is not None)
    stats.right_blocks = sum(1 for it in result.items if it.right is not None)

    matched = 0.0
    for item in result.items:
        if item.kind == "equal":
            # 单侧数据也应按实际存在的块数计分，否则相似度会超过 100%
            matched += float(item.left is not None) + float(item.right is not None)
        elif item.kind == "modify":
            stats.modified += 1
            matched += (float(item.left is not None)
                        + float(item.right is not None)) * max(item.similarity, 0.0)
            if item.table is not None and any(
                    rp.kind != "equal" for rp in item.table.row_pairs):
                stats.tables_changed += 1
        elif item.kind == "insert":
            stats.inserted += 1
        elif item.kind == "delete":
            stats.deleted += 1
        stats.format_changes += len(item.format_changes)

    total = stats.left_blocks + stats.right_blocks
    stats.similarity = round(matched / total, 4) if total else 1.0
    return stats
