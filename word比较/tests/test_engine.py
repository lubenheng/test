"""端到端测试：python tests/test_engine.py"""

from __future__ import annotations

import sys
from pathlib import Path

# Windows 中文控制台默认是 GBK，无法输出 ✓/✗ 等符号
for _stream in (sys.stdout, sys.stderr):
    if hasattr(_stream, "reconfigure"):
        _stream.reconfigure(encoding="utf-8", errors="replace")

ROOT = Path(__file__).resolve().parent.parent
sys.path.insert(0, str(ROOT / "src"))
sys.path.insert(0, str(ROOT))

from helpers import paragraph_texts  # noqa: E402

from wordcompare import Options, compare  # noqa: E402
from wordcompare.output.docx_redline import write_redline  # noqa: E402
from wordcompare.output.html_report import write_report  # noqa: E402
from wordcompare.output.summary import write_summary  # noqa: E402

OUT = ROOT / "tests" / "_tmp"


def build() -> tuple[Path, Path]:
    from make_fixtures import main as make_fixtures
    return make_fixtures(OUT)


def test_alignment_and_stats(left: Path, right: Path) -> None:
    result = compare(left, right)
    stats = result.stats

    kinds = [item.kind for item in result.items]
    assert kinds.count("delete") == 1, kinds          # 第二条被删除
    assert kinds.count("insert") == 3, kinds          # 第五条 + 新列表项 + 第六条
    assert stats.tables_changed == 1, stats.to_dict()
    assert stats.modified >= 3, stats.to_dict()
    assert 0.6 < stats.similarity < 0.9, stats.similarity

    # 文字修改必须带行内 diff
    changed = [it for it in result.items
               if it.kind == "modify" and it.left is not None
               and it.right is not None and it.left.kind == "paragraph"]
    assert changed, "没有识别出段落修改"
    first = min(changed, key=lambda it: it.similarity)
    assert first.inline is not None
    assert any(seg.op == "del" for seg in first.inline)
    assert any(seg.op == "ins" for seg in first.inline)
    print(f"  ✓ 对齐与统计：{stats.to_dict()}")


def test_format_only_change(left: Path, right: Path) -> None:
    result = compare(left, right)
    fmt_items = [it for it in result.items if it.format_changes]
    assert fmt_items, "没有识别出仅格式变化的段落"
    props = {ch.prop for it in fmt_items for ch in it.format_changes}
    assert "bold" in props, props
    fmt_item = fmt_items[0]
    assert fmt_item.left.text == fmt_item.right.text
    print(f"  ✓ 格式变更：{[(c.prop, c.old, c.new) for c in fmt_item.format_changes]}")


def test_list_numbering_stable(left: Path, right: Path) -> None:
    """插入一个列表项后，原有两条不应被误判为修改。"""
    result = compare(left, right)
    list_texts = {"首付款：合同签订后支付", "尾款：验收通过后支付"}
    for item in result.items:
        if item.left is not None and item.left.kind == "paragraph" \
                and item.left.text in list_texts:
            assert item.kind == "equal", (item.kind, item.left.text, item.left.numbering)
    print("  ✓ 自动编号未污染对齐结果")


def test_html_report(left: Path, right: Path) -> None:
    result = compare(left, right)
    out = write_report(result, OUT / "report.html")
    html = out.read_text(encoding="utf-8")
    assert 'class="del"' in html and 'class="ins"' in html
    assert "tcell-changed" in html
    assert "格式变更" in html
    print(f"  ✓ HTML 报告：{out}（{len(html)} 字节）")


def test_summary(left: Path, right: Path) -> None:
    result = compare(left, right)
    md = write_summary(result, OUT / "summary.md", OUT / "summary.csv")
    text = md.read_text(encoding="utf-8")
    assert "整体相似度" in text and "变更明细" in text
    assert (OUT / "summary.csv").exists()
    print(f"  ✓ 统计摘要：{md}")


def test_redline_accept_equals_right(left: Path, right: Path) -> None:
    """核心不变量：接受全部修订后应与新稿完全一致。"""
    result = compare(left, right)
    out = write_redline(result, OUT / "redline.docx")
    accepted = paragraph_texts(out, accept_all=True)
    expected = paragraph_texts(right)
    assert accepted == expected, "\n".join(
        f"{i}: {a!r} != {b!r}"
        for i, (a, b) in enumerate(zip(accepted, expected)) if a != b
    )
    print(f"  ✓ 修订稿可接受：{out}（{len(accepted)} 段）")


def test_identical_document(left: Path) -> None:
    result = compare(left, left)
    assert result.stats.inserted == 0
    assert result.stats.deleted == 0
    assert result.stats.modified == 0
    assert result.stats.similarity == 1.0
    assert all(it.kind == "equal" for it in result.items)
    print("  ✓ 同一文档比较无差异")


def test_threshold_option(left: Path, right: Path) -> None:
    """阈值调高应更多地把修改判为新增+删除。"""
    loose = compare(left, right, Options(similarity_threshold=0.3))
    strict = compare(left, right, Options(similarity_threshold=0.95))
    assert strict.stats.modified <= loose.stats.modified
    assert strict.stats.inserted + strict.stats.deleted >= loose.stats.inserted + loose.stats.deleted
    print(f"  ✓ 阈值生效：宽松修改 {loose.stats.modified}，严格修改 {strict.stats.modified}")


def test_compare_scope(left: Path, right: Path) -> None:
    """文本 / 格式 / 表格 三个范围开关必须各自独立生效。"""
    text_only = compare(left, right, Options(compare_text=False))
    assert text_only.stats.inserted == 0, text_only.stats.to_dict()
    assert text_only.stats.deleted == 0, text_only.stats.to_dict()
    assert text_only.stats.tables_changed == 0, text_only.stats.to_dict()
    # 只剩"第三条"的加粗变化
    assert text_only.stats.modified == 1, text_only.stats.to_dict()
    assert text_only.stats.format_changes == 1, text_only.stats.to_dict()
    assert 0.0 <= text_only.stats.similarity <= 1.0, text_only.stats.to_dict()

    no_tables = compare(left, right, Options(compare_tables=False))
    kinds = {(it.left or it.right).kind for it in no_tables.items}
    assert kinds == {"paragraph"}, kinds
    assert no_tables.stats.tables_changed == 0, no_tables.stats.to_dict()
    assert no_tables.stats.left_blocks == 8, no_tables.stats.to_dict()

    no_format = compare(left, right, Options(format_diff=False))
    assert no_format.stats.format_changes == 0, no_format.stats.to_dict()
    bold_item = next(it for it in no_format.items
                     if it.left is not None and it.right is not None
                     and getattr(it.left, "text", "") == "第三条 本合同一式两份，双方各执一份。")
    assert bold_item.kind == "equal", bold_item.kind
    print(f"  ✓ 比较范围：仅文本 {text_only.stats.to_dict()}")


def test_cli_and_content_option(left: Path, right: Path) -> None:
    """CLI 是主要入口，必须能跑通；--no-tables 选项必须真的生效。"""
    from wordcompare.cli import main as cli_main

    out = OUT / "_cli"
    code = cli_main([str(left), str(right), "-o", str(out), "--no-redline",
                     "--no-tables", "--quiet"])
    assert code == 0, code
    assert (out / f"{left.stem}__vs__{right.stem}.html").exists()

    result = compare(left, right, Options(compare_tables=False))
    blocks = [it.left or it.right for it in result.items]
    assert all(block.kind == "paragraph" for block in blocks), "--no-tables 仍读到了表格"
    assert result.stats.tables_changed == 0
    print("  ✓ CLI 可用，--no-tables 生效")


def main() -> int:
    left, right = build()
    tests = [
        ("对齐与统计", lambda: test_alignment_and_stats(left, right)),
        ("仅格式变化", lambda: test_format_only_change(left, right)),
        ("列表编号", lambda: test_list_numbering_stable(left, right)),
        ("HTML 报告", lambda: test_html_report(left, right)),
        ("统计摘要", lambda: test_summary(left, right)),
        ("修订稿接受后一致", lambda: test_redline_accept_equals_right(left, right)),
        ("同一文档", lambda: test_identical_document(left)),
        ("阈值", lambda: test_threshold_option(left, right)),
        ("比较范围", lambda: test_compare_scope(left, right)),
        ("CLI 与比较范围", lambda: test_cli_and_content_option(left, right)),
    ]
    failed = 0
    for name, fn in tests:
        try:
            fn()
        except AssertionError as exc:
            failed += 1
            print(f"  ✗ {name} 失败：{exc}")
        except Exception as exc:  # noqa: BLE001
            failed += 1
            print(f"  ✗ {name} 异常：{type(exc).__name__}: {exc}")
    print(f"\n{'全部通过' if not failed else f'{failed} 项失败'}（共 {len(tests)} 项）")
    return 1 if failed else 0


if __name__ == "__main__":
    raise SystemExit(main())
