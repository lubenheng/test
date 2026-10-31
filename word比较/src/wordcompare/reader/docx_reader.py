"""docx 读取器：把文档解析成块序列。"""

from __future__ import annotations

import hashlib
import zipfile
from pathlib import Path

from docx import Document as load_docx
from docx.text.paragraph import Paragraph as DocxParagraph

from ..model import Block, Cell, Paragraph, Table
from ..options import Options
from ..normalize import assign_norms
from .flatten import collect_runs, parse_xml, qn
from .numbering import NumberingResolver
from .styles import StyleResolver

_P = qn("w:p")
_TBL = qn("w:tbl")
_TR = qn("w:tr")
_TC = qn("w:tc")
_SDT = qn("w:sdt")
_SDT_CONTENT = qn("w:sdtContent")


class DocxReader:
    def __init__(self, path: str | Path, options: Options | None = None):
        self.path = Path(path)
        self.options = options or Options()
        self._doc = load_docx(str(self.path))
        numbering_root, styles_root = self._read_aux_xml()
        self._numbering = NumberingResolver(numbering_root, styles_root)
        self._styles = StyleResolver(styles_root)
        self._image_cache: dict[str, str] = {}

    # ------------------------------------------------------------ numbering
    def _read_aux_xml(self):
        numbering_root = styles_root = None
        try:
            with zipfile.ZipFile(self.path) as zf:
                names = set(zf.namelist())
                if "word/numbering.xml" in names:
                    numbering_root = parse_xml(zf.read("word/numbering.xml"))
                if "word/styles.xml" in names:
                    styles_root = parse_xml(zf.read("word/styles.xml"))
        except (zipfile.BadZipFile, KeyError, OSError) as exc:
            raise ValueError(f"无法读取 {self.path}，请确认是 .docx 格式：{exc}") from exc
        return numbering_root, styles_root

    # ---------------------------------------------------------------- images
    def _resolve_image(self, rid: str) -> str:
        if rid in self._image_cache:
            return self._image_cache[rid]
        digest = ""
        try:
            part = self._doc.part.related_parts.get(rid)
            if part is not None and hasattr(part, "blob"):
                digest = hashlib.sha1(part.blob).hexdigest()[:16]
        except Exception:  # noqa: BLE001 - 损坏的关系不应中断整个比较
            digest = ""
        self._image_cache[rid] = digest
        return digest

    # ------------------------------------------------------------------ read
    def read(self) -> tuple[list[Block], str]:
        blocks = []
        body = self._doc.element.body
        # 根据选项决定读哪些内容
        blocks = self._parse_children(body)

        if not self.options.compare_tables:
            blocks = [b for b in blocks if b.kind == "paragraph"]

        # TODO: 后续增加页眉、脚注、批注等（FULL 支持）

        assign_norms(blocks, self.options)
        title = (self._doc.core_properties.title or "").strip() or self.path.name
        return blocks, title

    @property
    def document(self):
        """底层 python-docx Document；修订稿写回需要同一棵 XML 树。"""
        return self._doc

    def _parse_children(self, parent) -> list[Block]:
        blocks: list[Block] = []
        for child in parent:
            tag = child.tag
            if tag == _P:
                blocks.append(self._parse_paragraph(child))
            elif tag == _TBL:
                blocks.append(self._parse_table(child))
            elif tag in (_SDT, _SDT_CONTENT):
                blocks.extend(self._parse_children(child))
            # w:sectPr / w:bookmarkStart 等忽略
        return blocks

    # ------------------------------------------------------------- paragraph
    def _parse_paragraph(self, p_el) -> Paragraph:
        runs, image_rids = collect_runs(p_el, self._resolve_image)
        images = [d for d in image_rids if d]
        pPr = p_el.find(qn("w:pPr"))

        style_id, style_name = self._style_of(p_el)
        alignment = indent = None
        numbering = ""
        num_id = None
        ilvl = 0

        if pPr is not None:
            jc = pPr.find(qn("w:jc"))
            if jc is not None:
                alignment = jc.get(qn("w:val"))
            ind = pPr.find(qn("w:ind"))
            if ind is not None:
                left = ind.get(qn("w:left")) or ind.get(qn("w:start"))
                if left is not None:
                    try:
                        indent = int(left) / 20
                    except ValueError:
                        indent = None
            num_id, ilvl = self._numbering_of(pPr, style_id)
        else:
            num_id, ilvl = self._numbering_of(None, style_id)

        if num_id:
            numbering = self._numbering.next_prefix(num_id, ilvl)

        self._apply_effective_format(runs, style_id)

        return Paragraph(
            runs=runs,
            style=style_name,
            alignment=alignment,
            indent_left=indent,
            numbering=numbering,
            num_id=num_id,
            ilvl=ilvl,
            images=images,
            el=p_el,
        )

    def _style_of(self, p_el) -> tuple[str, str]:
        style_id = ""
        pPr = p_el.find(qn("w:pPr"))
        if pPr is not None:
            pstyle = pPr.find(qn("w:pStyle"))
            if pstyle is not None:
                style_id = pstyle.get(qn("w:val")) or ""
        name = style_id
        try:
            name = DocxParagraph(p_el, self._doc).style.name or style_id
        except Exception:  # noqa: BLE001
            pass
        return style_id, name

    def _apply_effective_format(self, runs, paragraph_style_id: str) -> None:
        """把段落样式 / 字符样式里继承来的格式补到 run 上。"""
        base = self._styles.run_props(paragraph_style_id)
        for run in runs:
            props = dict(base)
            rPr = run.rPr_el
            if rPr is not None:
                rstyle = rPr.find(qn("w:rStyle"))
                if rstyle is not None:
                    props.update(self._styles.run_props(rstyle.get(qn("w:val"))))
            for key, value in run.props().items():
                if value is not None:       # 直接格式优先
                    props[key] = value
            for key, value in props.items():
                setattr(run, key, value)

    def _numbering_of(self, pPr, style_id: str) -> tuple[str | None, int]:
        if pPr is not None:
            numPr = pPr.find(qn("w:numPr"))
            if numPr is not None:
                nid = numPr.find(qn("w:numId"))
                lvl = numPr.find(qn("w:ilvl"))
                if nid is not None and nid.get(qn("w:val")) not in (None, "0"):
                    return nid.get(qn("w:val")), int(lvl.get(qn("w:val"))) if lvl is not None else 0
        style_num = self._numbering.style_numbering(style_id) if style_id else None
        return style_num if style_num else (None, 0)

    # ----------------------------------------------------------------- table
    def _parse_table(self, tbl_el) -> Table:
        table = Table(el=tbl_el)
        max_cols = 0
        for ri, tr in enumerate(tbl_el.findall(_TR)):
            row: list[Cell] = []
            col = 0
            for tc in tr.findall(_TC):
                tcPr = tc.find(qn("w:tcPr"))
                span = 1
                vmerge = None
                if tcPr is not None:
                    gs = tcPr.find(qn("w:gridSpan"))
                    if gs is not None:
                        try:
                            span = int(gs.get(qn("w:val")))
                        except (TypeError, ValueError):
                            span = 1
                    vmerge = tcPr.find(qn("w:vMerge"))

                is_continue = (vmerge is not None
                               and vmerge.get(qn("w:val")) in (None, "continue"))
                blocks = [] if is_continue else self._parse_children(tc)
                row.append(Cell(blocks=blocks, row=ri, col=col, colspan=span,
                                vmerge_continue=is_continue, el=tc))
                col += span
            max_cols = max(max_cols, col)
            table.rows.append(row)
        table.n_cols = max_cols
        return table
