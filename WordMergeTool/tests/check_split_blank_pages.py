# -*- coding: utf-8 -*-
"""拆分总文档后出现空白页的最小复现脚本。

构造几种常见的章间分隔方式，拆分后用 Word 统计每份文档的页数。
"""
import importlib.util
import shutil
import sys
import tempfile
from pathlib import Path

from docx import Document
from docx.enum.section import WD_SECTION
from docx.oxml import OxmlElement
from docx.oxml.ns import qn

ROOT = Path(__file__).resolve().parent.parent
spec = importlib.util.spec_from_file_location("word_merge", ROOT / "word_merge.pyw")
wm = importlib.util.module_from_spec(spec)
sys.modules["word_merge"] = wm
spec.loader.exec_module(wm)


def _page_break_paragraph(doc):
    p = doc.add_paragraph()
    run = p.add_run()
    br = OxmlElement("w:br")
    br.set(qn("w:type"), "page")
    run._r.append(br)
    return p


def _page_break_at_end_of_paragraph(doc):
    """模拟在正文段落末尾按 Ctrl+Enter。"""
    p = doc.paragraphs[-1]
    run = p.add_run()
    br = OxmlElement("w:br")
    br.set(qn("w:type"), "page")
    run._r.append(br)
    return p


def build_master(kind: str) -> Path:
    doc = Document()
    for chapter in (1, 2, 3):
        heading = doc.add_heading(f"第{chapter}章 章节{chapter}", level=1)
        doc.add_paragraph(f"第{chapter}章的正文内容。")
        doc.add_paragraph(f"第{chapter}章的第二段。")
        if chapter == 3:
            break
        if kind == "page-break-paragraph":
            _page_break_paragraph(doc)
        elif kind == "page-break-inline":
            _page_break_at_end_of_paragraph(doc)
        elif kind == "empty-paragraphs":
            doc.add_paragraph()
            doc.add_paragraph()
        elif kind == "section-break":
            # 分节符：上一章末尾段落带 sectPr，下一章另起一页
            heading.add_run()
            p = doc.paragraphs[-1]
            p.paragraph_format.page_break_before = True
    path = Path(tempfile.mkdtemp(prefix="wm_repro_")) / f"master_{kind}.docx"
    doc.save(str(path))
    return path


def build_master_section_break() -> Path:
    """用真正的分节符分隔章节。"""
    doc = Document()
    for chapter in (1, 2, 3):
        doc.add_heading(f"第{chapter}章 章节{chapter}", level=1)
        doc.add_paragraph(f"第{chapter}章的正文内容。")
        if chapter != 3:
            doc.add_section(WD_SECTION.NEW_PAGE)
    path = Path(tempfile.mkdtemp(prefix="wm_repro_")) / "master_section_break.docx"
    doc.save(str(path))
    return path


def build_master_sect_with_text() -> Path:
    """分节符落在带文字的段落上（Word 里在正文末尾插入分节符时就是这样）。"""
    from copy import deepcopy

    doc = Document()
    for chapter in (1, 2, 3):
        doc.add_heading(f"第{chapter}章 章节{chapter}", level=1)
        doc.add_paragraph(f"第{chapter}章的正文内容。")
        doc.add_paragraph(f"第{chapter}章的最后一段。")
        if chapter != 3:
            last = doc.paragraphs[-1]._p
            ppr = last.get_or_add_pPr()
            ppr.append(deepcopy(doc.element.body.find(qn("w:sectPr"))))
    path = Path(tempfile.mkdtemp(prefix="wm_repro_")) / "master_sect_with_text.docx"
    doc.save(str(path))
    return path


def page_count(app, path: Path) -> int:
    doc = app.Documents.Open(str(path), ReadOnly=True)
    try:
        doc.Repaginate()
        return int(doc.ComputeStatistics(2))
    finally:
        doc.Close(False)


def main():
    import win32com.client

    app = win32com.client.gencache.EnsureDispatch("Word.Application")
    app.Visible = False
    app.DisplayAlerts = 0
    try:
        cases = {
            "page-break-paragraph": build_master("page-break-paragraph"),
            "page-break-inline": build_master("page-break-inline"),
            "empty-paragraphs": build_master("empty-paragraphs"),
            "section-break": build_master_section_break(),
            "sect-with-text": build_master_sect_with_text(),
        }
        for kind, master in cases.items():
            out_dir = Path(tempfile.mkdtemp(prefix=f"wm_out_{kind}_"))
            print(f"\n=== {kind}: {master.name} ===")
            print(" master pages:", page_count(app, master))
            outputs = wm.split_document_by_chapters(master, out_dir, print)
            for path in outputs:
                print(f"  {path.name}: {page_count(app, path)} 页")
            shutil.rmtree(master.parent, ignore_errors=True)
            shutil.rmtree(out_dir, ignore_errors=True)
    finally:
        app.Quit()


if __name__ == "__main__":
    sys.exit(main())
