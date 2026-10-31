"""数据模型：文档结构、差异结果。

设计要点：
* Block 只保留"比较所需"的语义信息，原始 XML 元素挂在 ``el`` 上供修订稿写回。
* 段落文本不包含自动编号前缀（编号变化属于格式变更），避免插入一条列表项
  导致后续所有条目被误判为"修改"。
"""

from __future__ import annotations

from dataclasses import dataclass, field
from typing import Any


# --------------------------------------------------------------------------
# 文档结构
# --------------------------------------------------------------------------
@dataclass
class Run:
    """一段格式一致的连续文字。"""

    text: str
    bold: bool | None = None
    italic: bool | None = None
    underline: bool | None = None
    strike: bool | None = None
    font: str | None = None
    size: float | None = None          # 磅
    color: str | None = None           # 十六进制，如 "FF0000"
    highlight: str | None = None
    rPr_el: Any = field(default=None, repr=False, compare=False)

    def props(self) -> dict[str, Any]:
        return {
            "bold": self.bold,
            "italic": self.italic,
            "underline": self.underline,
            "strike": self.strike,
            "font": self.font,
            "size": self.size,
            "color": self.color,
            "highlight": self.highlight,
        }


@dataclass
class Paragraph:
    runs: list[Run] = field(default_factory=list)
    style: str = ""
    alignment: str | None = None       # left/center/right/both...
    indent_left: float | None = None   # 磅
    numbering: str = ""                # 还原出的自动编号前缀，如 "1.2."
    num_id: str | None = None
    ilvl: int = 0
    images: list[str] = field(default_factory=list)   # 图片内容哈希
    el: Any = field(default=None, repr=False, compare=False)

    # 由 reader 填充
    norm: str = ""
    hash: str = ""
    kind: str = field(default="paragraph", init=False)

    @property
    def text(self) -> str:
        return "".join(r.text for r in self.runs)

    @property
    def display_text(self) -> str:
        return f"{self.numbering}{self.text}" if self.numbering else self.text


@dataclass
class Cell:
    blocks: list["Block"] = field(default_factory=list)
    row: int = 0
    col: int = 0
    colspan: int = 1
    rowspan: int = 1
    vmerge_continue: bool = False      # 纵向合并的延续格
    el: Any = field(default=None, repr=False, compare=False)

    @property
    def text(self) -> str:
        return "\n".join(b.text for b in self.blocks)

    @property
    def norm(self) -> str:
        return "\n".join(getattr(b, "norm", b.text) for b in self.blocks)


@dataclass
class Table:
    rows: list[list[Cell]] = field(default_factory=list)
    n_cols: int = 0
    el: Any = field(default=None, repr=False, compare=False)
    norm: str = ""
    hash: str = ""
    kind: str = field(default="table", init=False)

    @property
    def text(self) -> str:
        return "\n".join("\t".join(c.text for c in row) for row in self.rows)

    def row_texts(self) -> list[str]:
        return ["\t".join(c.norm for c in row) for row in self.rows]

    def cell_at(self, row_idx: int, col: int) -> Cell | None:
        if not (0 <= row_idx < len(self.rows)):
            return None
        for cell in self.rows[row_idx]:
            if cell.col == col:
                return cell
        return None


Block = Paragraph | Table


# --------------------------------------------------------------------------
# 差异结果
# --------------------------------------------------------------------------
@dataclass
class Segment:
    """行内差异片段。op: equal | del | ins"""

    op: str
    text: str


@dataclass
class FormatChange:
    scope: str            # "文字" | "段落"
    prop: str             # 属性代码，如 "bold"
    old: str              # 展示值，如 "关" / "继承"
    new: str
    sample: str = ""      # 受影响的文字样例
    count: int = 1


@dataclass
class CellDiff:
    left: Cell | None
    right: Cell | None
    items: list["DiffItem"] = field(default_factory=list)
    kind: str = "equal"


@dataclass
class RowPair:
    left_idx: int | None
    right_idx: int | None
    kind: str                       # equal | insert | delete | modify
    similarity: float = 1.0
    cells: list[CellDiff] = field(default_factory=list)


@dataclass
class TableDiff:
    left: Table
    right: Table
    row_pairs: list[RowPair] = field(default_factory=list)

    def pair_by_left_row(self, idx: int) -> RowPair | None:
        for rp in self.row_pairs:
            if rp.left_idx == idx:
                return rp
        return None

    def pair_by_right_row(self, idx: int) -> RowPair | None:
        for rp in self.row_pairs:
            if rp.right_idx == idx:
                return rp
        return None


@dataclass
class DiffItem:
    kind: str                        # equal | insert | delete | modify
    left: Block | None = None
    right: Block | None = None
    similarity: float = 1.0
    inline: list[Segment] | None = None
    format_changes: list[FormatChange] = field(default_factory=list)
    table: TableDiff | None = None

    @property
    def changed(self) -> bool:
        return self.kind != "equal"


@dataclass
class Stats:
    inserted: int = 0
    deleted: int = 0
    modified: int = 0
    format_changes: int = 0
    tables_changed: int = 0
    left_blocks: int = 0
    right_blocks: int = 0
    similarity: float = 1.0

    def to_dict(self) -> dict[str, Any]:
        return {
            "inserted": self.inserted,
            "deleted": self.deleted,
            "modified": self.modified,
            "format_changes": self.format_changes,
            "tables_changed": self.tables_changed,
            "left_blocks": self.left_blocks,
            "right_blocks": self.right_blocks,
            "similarity": round(self.similarity, 4),
        }

    @property
    def total_changes(self) -> int:
        return self.inserted + self.deleted + self.modified


@dataclass
class DiffResult:
    left_path: str
    right_path: str
    items: list[DiffItem] = field(default_factory=list)
    stats: Stats = field(default_factory=Stats)
    left_title: str = ""
    right_title: str = ""
    # 读取阶段的 python-docx Document 对象：修订稿写回必须复用同一棵 XML 树
    left_doc: Any = field(default=None, repr=False, compare=False)
    right_doc: Any = field(default=None, repr=False, compare=False)
