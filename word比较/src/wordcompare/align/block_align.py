"""块级对齐。

两段式：
1. 先用哈希做精确 LCS，未变内容 100% 对齐，同时把文档切成若干"空洞"；
2. 在每个空洞内做带相似度门槛的 DP 对齐，吸收"改了一句话"的局部变动。
"""

from __future__ import annotations

from difflib import SequenceMatcher
from typing import Callable, Sequence

from ..model import Block, DiffItem
from ..options import Options
from .fingerprint import similarity

# 对齐结果元组: (kind, left_idx, right_idx, similarity)
Alignment = tuple[str, int | None, int | None, float]

SimFn = Callable[[int, int], float | None]


def align_sequences(
    left_keys: Sequence[str],
    right_keys: Sequence[str],
    sim_fn: SimFn,
    threshold: float = 0.5,
    max_cells: int = 250_000,
) -> list[Alignment]:
    """通用序列对齐：先精确锚点，再空洞内 DP。"""
    result: list[Alignment] = []
    matcher = SequenceMatcher(None, list(left_keys), list(right_keys), autojunk=False)
    for tag, i1, i2, j1, j2 in matcher.get_opcodes():
        if tag == "equal":
            for k in range(i2 - i1):
                result.append(("equal", i1 + k, j1 + k, 1.0))
        elif tag == "replace":
            result.extend(_align_hole(i1, i2, j1, j2, sim_fn, threshold, max_cells))
        elif tag == "delete":
            result.extend(("delete", i, None, 0.0) for i in range(i1, i2))
        elif tag == "insert":
            result.extend(("insert", None, j, 0.0) for j in range(j1, j2))
    return result


def _align_hole(i1: int, i2: int, j1: int, j2: int, sim_fn: SimFn,
                threshold: float, max_cells: int) -> list[Alignment]:
    n, m = i2 - i1, j2 - j1
    if n * m > max_cells:
        return _align_greedy(i1, i2, j1, j2, sim_fn, threshold)

    sims: list[list[float | None]] = [[None] * m for _ in range(n)]
    for x in range(n):
        for y in range(m):
            value = sim_fn(i1 + x, j1 + y)
            if value is not None and value >= threshold:
                sims[x][y] = value

    dp = [[0.0] * (m + 1) for _ in range(n + 1)]
    for x in range(1, n + 1):
        row, prev = dp[x], dp[x - 1]
        for y in range(1, m + 1):
            best = row[y - 1] if row[y - 1] > prev[y] else prev[y]
            s = sims[x - 1][y - 1]
            if s is not None:
                cand = prev[y - 1] + s
                if cand > best:
                    best = cand
            row[y] = best

    ops: list[Alignment] = []
    x, y = n, m
    while x > 0 or y > 0:
        if x > 0 and y > 0:
            s = sims[x - 1][y - 1]
            if s is not None and abs(dp[x][y] - (dp[x - 1][y - 1] + s)) < 1e-9:
                ops.append(("pair", i1 + x - 1, j1 + y - 1, s))
                x -= 1
                y -= 1
                continue
        if x > 0 and (y == 0 or dp[x - 1][y] >= dp[x][y - 1]):
            ops.append(("delete", i1 + x - 1, None, 0.0))
            x -= 1
        else:
            ops.append(("insert", None, j1 + y - 1, 0.0))
            y -= 1
    ops.reverse()
    return ops


def _align_greedy(i1: int, i2: int, j1: int, j2: int, sim_fn: SimFn,
                  threshold: float) -> list[Alignment]:
    """超大空洞的退化策略：反复取全局最优配对。"""
    left = list(range(i1, i2))
    right = list(range(j1, j2))
    pairs: list[tuple[int, int, float]] = []
    while left and right:
        best = (None, None, -1.0)
        for x in left:
            for y in right:
                value = sim_fn(x, y)
                if value is not None and value > best[2]:
                    best = (x, y, value)
        if best[0] is None or best[2] < threshold:
            break
        pairs.append(best)
        left.remove(best[0])
        right.remove(best[1])

    matched_left = {x: (y, s) for x, y, s in pairs}
    matched_right = {y for _, y, _ in pairs}
    ops: list[Alignment] = []
    pending_right = [j for j in range(j1, j2) if j not in matched_right]
    for i in range(i1, i2):
        if i in matched_left:
            j, s = matched_left[i]
            for jj in [p for p in pending_right if p < j]:
                ops.append(("insert", None, jj, 0.0))
            pending_right = [p for p in pending_right if p > j]
            ops.append(("pair", i, j, s))
        else:
            ops.append(("delete", i, None, 0.0))
    ops.extend(("insert", None, j, 0.0) for j in pending_right)
    return ops


# --------------------------------------------------------------------------
# 块序列
# --------------------------------------------------------------------------
def block_similarity(left: Block, right: Block, opts: Options) -> float | None:
    if left.kind != right.kind:
        return None
    if left.kind == "paragraph":
        return similarity(left.norm, right.norm)
    # 表格
    from .table_align import table_similarity
    return table_similarity(left, right)


def align_blocks(left: list[Block], right: list[Block], opts: Options) -> list[DiffItem]:
    pairs = align_sequences(
        [b.hash for b in left],
        [b.hash for b in right],
        lambda i, j: block_similarity(left[i], right[j], opts),
        threshold=opts.similarity_threshold,
        max_cells=opts.max_dp_cells,
    )

    items: list[DiffItem] = []
    for kind, i, j, sim in pairs:
        if kind == "delete":
            items.append(DiffItem(kind="delete", left=left[i]))
        elif kind == "insert":
            items.append(DiffItem(kind="insert", right=right[j]))
        elif kind == "equal":
            lblock, rblock = left[i], right[j]
            same = (lblock.kind != "paragraph"
                    or _paragraph_meta_equal(lblock, rblock))
            items.append(DiffItem(
                kind="equal" if same else "modify",
                left=lblock, right=rblock, similarity=1.0,
            ))
        else:  # pair
            lblock, rblock = left[i], right[j]
            is_equal = (lblock.kind == rblock.kind == "paragraph"
                        and lblock.norm == rblock.norm
                        and _paragraph_meta_equal(lblock, rblock))
            item = DiffItem(
                kind="equal" if is_equal else "modify",
                left=lblock,
                right=rblock,
                similarity=sim,
            )
            if item.kind == "modify" and lblock.kind == "table":
                from .table_align import align_tables
                item.table = align_tables(lblock, rblock, opts)
            items.append(item)
    return items


def _paragraph_meta_equal(left, right) -> bool:
    """文本相同但编号层级/列表不同的，仍算修改（格式变更）。"""
    return (left.num_id == right.num_id) and (left.ilvl == right.ilvl)
