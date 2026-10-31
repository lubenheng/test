"""XML 层的基础操作：命名空间、开关属性、run 抽取。

这里刻意不用 python-docx 的 Paragraph.runs：它无法穿透 ``w:ins`` /
``w:hyperlink`` / ``w:fldSimple`` / ``w:sdt`` 等容器，而真实文档里这些
结构非常常见。
"""

from __future__ import annotations

from typing import Callable

from lxml import etree

from ..model import Run

W = "http://schemas.openxmlformats.org/wordprocessingml/2006/main"
A = "http://schemas.openxmlformats.org/drawingml/2006/main"
R = "http://schemas.openxmlformats.org/officeDocument/2006/relationships"
XML = "http://www.w3.org/XML/1998/namespace"

NSS = {"w": W, "a": A, "r": R}


def qn(tag: str) -> str:
    """'w:p' -> '{ns}p'"""
    prefix, local = tag.split(":", 1)
    ns = {"w": W, "a": A, "r": R, "xml": XML}[prefix]
    return f"{{{ns}}}{local}"


# 保留文本的容器：需要向下递归
_CONTAINERS = {
    qn("w:ins"), qn("w:moveTo"), qn("w:hyperlink"), qn("w:smartTag"),
    qn("w:sdt"), qn("w:sdtContent"), qn("w:fldSimple"), qn("w:customXml"),
    qn("w:dir"), qn("w:bdo"),
}
# 丢弃文本的容器：默认"接受全部修订"，删除内容不参与比较
_DROPPED = {qn("w:del"), qn("w:moveFrom")}


def toggle(rPr, name: str) -> bool | None:
    """读取 w:b / w:i / w:u 这类开关属性。None 表示未显式设置（继承）。"""
    if rPr is None:
        return None
    node = rPr.find(qn(name))
    if node is None:
        return None
    val = node.get(qn("w:val"))
    if val is None:
        return True
    return val.lower() not in ("0", "false", "off", "none")


def _run_props(rPr) -> dict:
    props: dict = {
        "bold": None, "italic": None, "underline": None, "strike": None,
        "font": None, "size": None, "color": None, "highlight": None,
    }
    if rPr is None:
        return props
    props["bold"] = toggle(rPr, "w:b")
    props["italic"] = toggle(rPr, "w:i")
    props["underline"] = toggle(rPr, "w:u")
    props["strike"] = toggle(rPr, "w:strike")
    if props["underline"] is False and rPr.find(qn("w:u")) is not None:
        props["underline"] = False

    fonts = rPr.find(qn("w:rFonts"))
    if fonts is not None:
        props["font"] = fonts.get(qn("w:eastAsia")) or fonts.get(qn("w:ascii"))

    sz = rPr.find(qn("w:sz"))
    if sz is not None:
        try:
            props["size"] = int(sz.get(qn("w:val"))) / 2
        except (TypeError, ValueError):
            pass

    color = rPr.find(qn("w:color"))
    if color is not None:
        val = color.get(qn("w:val"))
        if val and val.lower() != "auto":
            props["color"] = val.upper()

    hl = rPr.find(qn("w:highlight"))
    if hl is not None:
        props["highlight"] = hl.get(qn("w:val"))
    return props


def _run_from_el(r_el) -> tuple[Run, list[str]]:
    rPr = r_el.find(qn("w:rPr"))
    texts: list[str] = []
    images: list[str] = []
    for child in r_el:
        tag = child.tag
        if tag == qn("w:t") or tag == qn("w:delText"):
            texts.append(child.text or "")
        elif tag == qn("w:tab"):
            texts.append("\t")
        elif tag in (qn("w:br"), qn("w:cr")):
            texts.append("\n")
        elif tag == qn("w:noBreakHyphen"):
            texts.append("-")
        elif tag in (qn("w:drawing"), qn("w:pict")):
            for blip in child.iter(qn("a:blip")):
                rid = blip.get(qn("r:embed"))
                if rid:
                    images.append(rid)
    run = Run(text="".join(texts), rPr_el=rPr, **_run_props(rPr))
    return run, images


def collect_runs(p_el, image_resolver: Callable[[str], str] | None = None
                 ) -> tuple[list[Run], list[str]]:
    """按文档顺序抽取段落的 run；返回 (runs, 图片哈希列表)。"""
    runs: list[Run] = []
    image_rids: list[str] = []

    def walk(node) -> None:
        for child in node:
            tag = child.tag
            if tag == qn("w:r"):
                run, rids = _run_from_el(child)
                if run.text:
                    runs.append(run)
                image_rids.extend(rids)
            elif tag in _DROPPED:
                continue
            elif tag in _CONTAINERS:
                walk(child)
            # w:pPr、w:bookmarkStart 等直接忽略

    walk(p_el)

    images: list[str] = []
    if image_resolver is not None:
        for rid in image_rids:
            digest = image_resolver(rid)
            if digest:
                images.append(digest)
    return runs, images


def has_parent(p_el, tag: str) -> bool:
    return p_el.find(qn(tag)) is not None


def parse_xml(data: bytes | None):
    if not data:
        return None
    return etree.fromstring(data)
