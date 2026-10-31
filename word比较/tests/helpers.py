"""测试辅助：模拟 Word 的"接受全部修订"。"""

from __future__ import annotations

import zipfile
from pathlib import Path

from lxml import etree

from wordcompare.reader.flatten import qn


def paragraph_texts(path: str | Path, accept_all: bool = False) -> list[str]:
    """按文档顺序取出所有段落文本（含表格单元格内的段落）。

    ``accept_all=True`` 时模拟 Word 的接受全部修订：
    * 删除所有 ``w:del``（含段落标记删除）
    * 展开 ``w:ins``
    * 段落标记被删除且内容为空的段落，视为与下一段合并后消失
    """
    root = etree.fromstring(zipfile.ZipFile(path).read("word/document.xml"))
    body = root.find(qn("w:body"))

    marked_deleted: set = set()
    if accept_all:
        for p_el in body.iter(qn("w:p")):
            pPr = p_el.find(qn("w:pPr"))
            if pPr is None:
                continue
            rPr = pPr.find(qn("w:rPr"))
            if rPr is not None and rPr.find(qn("w:del")) is not None:
                marked_deleted.add(p_el)

        for del_el in list(root.iter(qn("w:del"))):
            del_el.getparent().remove(del_el)

        for ins_el in list(root.iter(qn("w:ins"))):
            parent = ins_el.getparent()
            if parent is None:
                continue
            index = parent.index(ins_el)
            for child in list(ins_el):
                ins_el.remove(child)
                parent.insert(index, child)
                index += 1
            parent.remove(ins_el)

    texts: list[str] = []
    for p_el in body.iter(qn("w:p")):
        text = "".join(t.text or "" for t in p_el.iter(qn("w:t")))
        if p_el in marked_deleted and not text.strip():
            continue
        texts.append(text)
    return texts
