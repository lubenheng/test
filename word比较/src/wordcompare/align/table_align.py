"""表格对齐：行级对齐 + 单元格内容递归比较。"""

from __future__ import annotations

from ..model import CellDiff, RowPair, Table, TableDiff
from ..options import Options
from .block_align import align_sequences
from .fingerprint import similarity, text_hash


def table_similarity(left: Table, right: Table) -> float:
    if not left.rows and not right.rows:
        return 0.0
    a = " \n".join(left.row_texts())
    b = " \n".join(right.row_texts())
    content = similarity(a, b, fast=True)
    r1, r2 = len(left.rows), len(right.rows)
    shape = 2.0 * min(r1, r2) / (r1 + r2) if (r1 + r2) else 1.0
    return 0.7 * content + 0.3 * shape


def align_tables(left: Table, right: Table, opts: Options) -> TableDiff:
    left_rows = left.row_texts()
    right_rows = right.row_texts()
    pairs = align_sequences(
        [text_hash("r:" + t) for t in left_rows],
        [text_hash("r:" + t) for t in right_rows],
        lambda i, j: similarity(left_rows[i], right_rows[j], fast=True),
        threshold=opts.similarity_threshold,
        max_cells=opts.max_dp_cells,
    )

    diff = TableDiff(left=left, right=right)
    for kind, i, j, sim in pairs:
        if kind == "delete":
            diff.row_pairs.append(RowPair(left_idx=i, right_idx=None, kind="delete"))
        elif kind == "insert":
            diff.row_pairs.append(RowPair(left_idx=None, right_idx=j, kind="insert"))
        else:
            row_kind = "equal" if (kind == "equal" or left_rows[i] == right_rows[j]) else "modify"
            rp = RowPair(left_idx=i, right_idx=j, kind=row_kind, similarity=sim)
            if row_kind == "modify":
                rp.cells = _align_row_cells(left, right, i, j, opts)
                if all(c.kind == "equal" for c in rp.cells):
                    rp.kind = "equal"
            diff.row_pairs.append(rp)
    return diff


def _align_row_cells(left: Table, right: Table, li: int, ri: int,
                     opts: Options) -> list[CellDiff]:
    from .block_align import align_blocks  # 循环导入保护

    cells: list[CellDiff] = []
    n_cols = max(left.n_cols, right.n_cols)
    for col in range(n_cols):
        lc = left.cell_at(li, col)
        rc = right.cell_at(ri, col)
        if lc is None and rc is None:
            continue
        if lc is None:
            cells.append(CellDiff(left=None, right=rc, kind="insert"))
            continue
        if rc is None:
            cells.append(CellDiff(left=lc, right=None, kind="delete"))
            continue
        if lc.vmerge_continue or rc.vmerge_continue:
            same = lc.vmerge_continue and rc.vmerge_continue
            cells.append(CellDiff(left=lc, right=rc, kind="equal" if same else "modify"))
            continue
        items = align_blocks(lc.blocks, rc.blocks, opts)
        changed = any(it.changed for it in items)
        cells.append(CellDiff(
            left=lc, right=rc, items=items,
            kind="modify" if changed else "equal",
        ))
    return cells
