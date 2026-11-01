# -*- coding: utf-8 -*-
"""拆分后格式与空白页回归检查：页面设置、页眉、编号、图片、多节文档。"""
import importlib.util
import sys
import tempfile
from pathlib import Path

from docx import Document
from docx.enum.section import WD_ORIENT, WD_SECTION
from docx.oxml import OxmlElement
from docx.oxml.ns import qn
from docx.shared import Cm, Inches

ROOT = Path(__file__).resolve().parent.parent
spec = importlib.util.spec_from_file_location("word_merge", ROOT / "word_merge.pyw")
wm = importlib.util.module_from_spec(spec)
sys.modules["word_merge"] = wm
spec.loader.exec_module(wm)

PIC = ROOT / "tests" / "data" / "pic.png"
errors: list[str] = []


def check(label, actual, expected):
    ok = actual == expected
    print(f"  [{'OK' if ok else 'FAIL'}] {label}: {actual!r} (期望 {expected!r})")
    if not ok:
        errors.append(f"{label}: {actual!r} != {expected!r}")


def page_count(app, path: Path) -> int:
    doc = app.Documents.Open(str(path), ReadOnly=True)
    try:
        doc.Repaginate()
        return int(doc.ComputeStatistics(2))
    finally:
        doc.Close(False)


def build_landscape_master(tmp: Path) -> Path:
    """第 2 章横向、页边距不同、页眉不同，且章节之间用分节符分隔。"""
    doc = Document()
    doc.sections[0].header.paragraphs[0].text = "第1章页眉"
    doc.add_heading("第1章 概述", level=1)
    doc.add_paragraph("第一章正文。")

    section = doc.add_section(WD_SECTION.NEW_PAGE)
    section.orientation = WD_ORIENT.LANDSCAPE
    width, height = section.page_width, section.page_height
    section.page_width, section.page_height = height, width
    section.left_margin = Cm(3)
    section.header.is_linked_to_previous = False
    section.header.paragraphs[0].text = "第2章页眉"
    doc.add_heading("第2章 系统时钟", level=1)
    doc.add_paragraph("第二章正文。")

    section = doc.add_section(WD_SECTION.NEW_PAGE)
    doc.add_heading("第3章 附录", level=1)
    doc.add_paragraph("第三章正文。")

    path = tmp / "master_landscape.docx"
    doc.save(str(path))
    return path


def build_trailing_image_master(tmp: Path) -> Path:
    """第 1 章以图片结尾，图片段落后面才是分页符。"""
    doc = Document()
    doc.add_heading("第1章 概述", level=1)
    doc.add_paragraph("第一章正文。")
    doc.add_picture(str(PIC), width=Inches(1))
    p = doc.add_paragraph()
    run = p.add_run()
    br = OxmlElement("w:br")
    br.set(qn("w:type"), "page")
    run._r.append(br)
    doc.add_heading("第2章 系统时钟", level=1)
    doc.add_paragraph("第二章正文。")
    path = tmp / "master_image.docx"
    doc.save(str(path))
    return path


def build_multisection_master(tmp: Path) -> Path:
    """第 1 章内部还有一节（纵向 → 横向），第 2 章另起一节。"""
    doc = Document()
    doc.add_heading("第1章 概述", level=1)
    doc.add_paragraph("第一章第一节正文。")
    section = doc.add_section(WD_SECTION.NEW_PAGE)
    section.orientation = WD_ORIENT.LANDSCAPE
    width, height = section.page_width, section.page_height
    section.page_width, section.page_height = height, width
    doc.add_paragraph("第一章第二节（横向）正文。")
    doc.add_section(WD_SECTION.NEW_PAGE)
    doc.add_heading("第2章 系统时钟", level=1)
    doc.add_paragraph("第二章正文。")
    path = tmp / "master_multisection.docx"
    doc.save(str(path))
    return path


def build_numbered_master(tmp: Path) -> Path:
    """标题使用多级列表编号，章号只出现在编号里。"""
    doc = Document()
    numbering = doc.part.numbering_part.element
    abstract = OxmlElement("w:abstractNum")
    abstract.set(qn("w:abstractNumId"), "90")
    for level in range(3):
        lvl = OxmlElement("w:lvl")
        lvl.set(qn("w:ilvl"), str(level))
        start = OxmlElement("w:start")
        start.set(qn("w:val"), "1")
        fmt = OxmlElement("w:numFmt")
        fmt.set(qn("w:val"), "decimal")
        text = OxmlElement("w:lvlText")
        text.set(qn("w:val"), ".".join(f"%{i + 1}" for i in range(level + 1)))
        lvl.append(start)
        lvl.append(fmt)
        lvl.append(text)
        abstract.append(lvl)
    numbering.insert(0, abstract)
    num = OxmlElement("w:num")
    num.set(qn("w:numId"), "90")
    ref = OxmlElement("w:abstractNumId")
    ref.set(qn("w:val"), "90")
    num.append(ref)
    numbering.append(num)

    for chapter in (1, 2, 3):
        p = doc.add_paragraph()
        p.style = doc.styles["Heading 1"]
        numpr = OxmlElement("w:numPr")
        ilvl = OxmlElement("w:ilvl")
        ilvl.set(qn("w:val"), "0")
        num_id = OxmlElement("w:numId")
        num_id.set(qn("w:val"), "90")
        numpr.append(ilvl)
        numpr.append(num_id)
        p._p.get_or_add_pPr().append(numpr)
        p.add_run(f"章节{chapter}")
        doc.add_paragraph(f"第{chapter}章正文。")
        if chapter != 3:
            pbreak = doc.add_paragraph()
            run = pbreak.add_run()
            br = OxmlElement("w:br")
            br.set(qn("w:type"), "page")
            run._r.append(br)
    path = tmp / "master_numbered.docx"
    doc.save(str(path))
    return path


def sect_prs(path: Path):
    doc = Document(str(path))
    return doc.element.body.findall(qn("w:sectPr")), [
        p.find(qn("w:pPr")).find(qn("w:sectPr"))
        for p in doc.element.body.findall(qn("w:p"))
        if p.find(qn("w:pPr")) is not None
        and p.find(qn("w:pPr")).find(qn("w:sectPr")) is not None
    ]


def build_header_master(tmp: Path) -> Path:
    """第1节独立页眉页脚，第2节另设页眉页脚，第3节「链接到前一节」继承第2节。"""
    doc = Document()
    first = doc.sections[0]
    first.header.paragraphs[0].text = "H1"
    first.footer.paragraphs[0].text = "F1"
    doc.add_heading("第1章 概述", level=1)
    doc.add_paragraph("第一章正文。")
    # 章内再分一节，这一节链接到前一节
    doc.add_section(WD_SECTION.NEW_PAGE)
    doc.add_paragraph("第一章第二节正文。")
    # 第二章：独立的页眉页脚
    section = doc.add_section(WD_SECTION.NEW_PAGE)
    section.header.is_linked_to_previous = False
    section.header.paragraphs[0].text = "H2"
    section.footer.is_linked_to_previous = False
    section.footer.paragraphs[0].text = "F2"
    doc.add_heading("第2章 系统时钟", level=1)
    doc.add_paragraph("第二章正文。")
    # 第三章：链接到前一节
    doc.add_section(WD_SECTION.NEW_PAGE)
    doc.add_heading("第3章 附录", level=1)
    doc.add_paragraph("第三章正文。")
    path = tmp / "master_header.docx"
    doc.save(str(path))
    return path


def build_first_page_master(tmp: Path) -> Path:
    """首页不同的页眉页脚。"""
    doc = Document()
    section = doc.sections[0]
    section.different_first_page_header_footer = True
    section.header.paragraphs[0].text = "H-ODD"
    section.first_page_header.paragraphs[0].text = "H-FIRST"
    section.footer.paragraphs[0].text = "F-ODD"
    section.first_page_footer.paragraphs[0].text = "F-FIRST"
    doc.add_heading("第1章 概述", level=1)
    doc.add_paragraph("第一章正文。")
    doc.add_section(WD_SECTION.NEW_PAGE)
    doc.add_heading("第2章 系统时钟", level=1)
    doc.add_paragraph("第二章正文。")
    path = tmp / "master_first_page.docx"
    doc.save(str(path))
    return path


def com_sections(app, path: Path):
    """用 Word 读出每节实际生效的页眉页脚文字（1=默认，2=首页）。"""
    doc = app.Documents.Open(str(path), ReadOnly=True)
    try:
        doc.Repaginate()
        result = []
        for index in range(1, doc.Sections.Count + 1):
            section = doc.Sections(index)
            result.append(
                {
                    "header": section.Headers(1).Range.Text.strip(),
                    "footer": section.Footers(1).Range.Text.strip(),
                    "first_header": section.Headers(2).Range.Text.strip(),
                    "first_footer": section.Footers(2).Range.Text.strip(),
                    "first_page": bool(section.PageSetup.DifferentFirstPageHeaderFooter),
                }
            )
        return result, int(doc.ComputeStatistics(2))
    finally:
        doc.Close(False)


def main():
    import win32com.client

    app = win32com.client.gencache.EnsureDispatch("Word.Application")
    app.Visible = False
    app.DisplayAlerts = 0
    tmp = Path(tempfile.mkdtemp(prefix="wm_verify_"))
    try:
        print("== 页面设置/页眉保留（第2章横向） ==")
        outputs = wm.split_document_by_chapters(
            build_landscape_master(tmp), tmp / "out_landscape", lambda *_: None
        )
        chapter2 = [p for p in outputs if p.name.startswith("2_")][0]
        doc = Document(str(chapter2))
        section = doc.sections[-1]
        check("页数", page_count(app, chapter2), 1)
        check("横向", section.orientation, WD_ORIENT.LANDSCAPE)
        check("左边距(cm)", round(section.left_margin.cm, 1), 3.0)
        check("页眉", section.header.paragraphs[0].text, "第2章页眉")
        chapter1 = [p for p in outputs if p.name.startswith("1_")][0]
        check("第1章页数", page_count(app, chapter1), 1)
        check("第1章页眉", Document(str(chapter1)).sections[-1].header.paragraphs[0].text, "第1章页眉")

        print("== 末尾图片保留 ==")
        outputs = wm.split_document_by_chapters(
            build_trailing_image_master(tmp), tmp / "out_image", lambda *_: None
        )
        chapter1 = [p for p in outputs if p.name.startswith("1_")][0]
        doc = Document(str(chapter1))
        check("页数", page_count(app, chapter1), 1)
        check("图片数量", len(doc.inline_shapes), 1)

        print("== 章内多节 ==")
        outputs = wm.split_document_by_chapters(
            build_multisection_master(tmp), tmp / "out_multi", lambda *_: None
        )
        chapter1 = [p for p in outputs if p.name.startswith("1_")][0]
        body_sects, paragraph_sects = sect_prs(chapter1)
        check("文档级 sectPr", len(body_sects), 1)
        check("段落级 sectPr", len(paragraph_sects), 1)
        check("节数", len(Document(str(chapter1)).sections), 2)
        check("页数", page_count(app, chapter1), 2)
        chapter2 = [p for p in outputs if p.name.startswith("2_")][0]
        check("第2章页数", page_count(app, chapter2), 1)
        check("第2章段落级 sectPr", len(sect_prs(chapter2)[1]), 0)

        print("== 页眉页脚 ==")
        outputs = wm.split_document_by_chapters(
            build_header_master(tmp), tmp / "out_header", lambda *_: None
        )
        expected = {
            "1_": [("H1", "F1"), ("H1", "F1")],  # 章内第二节链接到前一节
            "2_": [("H2", "F2")],
            "3_": [("H2", "F2")],  # 链接到第二章
        }
        for path in outputs:
            sections, pages = com_sections(app, path)
            key = path.name[:2]
            check(f"{key} 页数", pages, 2 if key == "1_" else 1)
            check(
                f"{key} 页眉页脚",
                [(s["header"], s["footer"]) for s in sections],
                expected[key],
            )

        print("== 首页不同 ==")
        outputs = wm.split_document_by_chapters(
            build_first_page_master(tmp), tmp / "out_first", lambda *_: None
        )
        for path in outputs:
            sections, pages = com_sections(app, path)
            section = sections[-1]
            check(f"{path.name[:2]} 首页不同", section["first_page"], True)
            check(f"{path.name[:2]} 首页页眉", section["first_header"], "H-FIRST")
            check(f"{path.name[:2]} 首页页脚", section["first_footer"], "F-FIRST")
            check(f"{path.name[:2]} 页数", pages, 1)

        print("== 多级列表编号 ==")
        outputs = wm.split_document_by_chapters(
            build_numbered_master(tmp), tmp / "out_numbered", lambda *_: None
        )
        for path in outputs:
            word = app.Documents.Open(str(path), ReadOnly=True)
            try:
                first = word.Paragraphs(1)
                check(f"{path.name} 首段编号", first.Range.ListFormat.ListString.strip(), path.name.split("_")[0])
                check(f"{path.name} 页数", int(word.ComputeStatistics(2)), 1)
            finally:
                word.Close(False)
    finally:
        app.Quit()

    print()
    if errors:
        print("存在失败项：")
        for item in errors:
            print(" -", item)
        return 1
    print("全部通过")
    return 0


if __name__ == "__main__":
    sys.exit(main())
