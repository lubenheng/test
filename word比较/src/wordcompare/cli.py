"""命令行入口。

    python -m wordcompare a.docx b.docx -o out
"""

from __future__ import annotations

import argparse
import sys
import time
from pathlib import Path

from .engine import compare
from .options import Options
from .output.docx_redline import write_redline
from .output.html_report import write_report
from .output.summary import write_summary


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="wordcompare",
        description="Word 文档比较：块对齐 + 行内 diff + 格式 diff",
    )
    parser.add_argument("left", help="基准文档 .docx")
    parser.add_argument("right", help="对比文档 .docx")
    parser.add_argument("-o", "--out", default="output", help="输出目录（默认 output）")

    parser.add_argument("--no-html", action="store_true", help="不生成 HTML 报告")
    parser.add_argument("--no-redline", action="store_true", help="不生成修订 docx")
    parser.add_argument("--no-summary", action="store_true", help="不生成统计摘要")

    parser.add_argument("--threshold", type=float, default=0.5,
                        help="块相似度阈值，低于此值判为新增/删除（默认 0.5）")
    parser.add_argument("--ignore-case", action="store_true", help="忽略大小写")
    parser.add_argument("--normalize-punctuation", action="store_true",
                        help="全角标点与半角标点视为相同")
    parser.add_argument("--keep-whitespace", action="store_true",
                        help="不折叠空白（默认折叠）")
    parser.add_argument("--no-text", action="store_true",
                        help="不比较文字内容（只报告格式变化）")
    parser.add_argument("--no-format", action="store_true", help="不比较格式")
    parser.add_argument("--no-tables", action="store_true", help="不比较表格")
    parser.add_argument("--ignore-inherited", action="store_true",
                        help="忽略'未设置 vs 显式设置'这类格式差异")

    parser.add_argument("--author", default="文档比较", help="修订作者名")
    parser.add_argument("--quiet", action="store_true", help="不打印统计")
    return parser


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)

    left, right = Path(args.left), Path(args.right)
    for path in (left, right):
        if not path.exists():
            print(f"错误：找不到文件 {path}", file=sys.stderr)
            return 2
        if path.suffix.lower() == ".doc":
            print(f"错误：{path.name} 是旧版 .doc，请先另存为 .docx", file=sys.stderr)
            return 2

    opts = Options(
        similarity_threshold=args.threshold,
        ignore_case=args.ignore_case,
        normalize_punctuation=args.normalize_punctuation,
        ignore_whitespace=not args.keep_whitespace,
        compare_text=not args.no_text,
        compare_tables=not args.no_tables,
        format_diff=not args.no_format,
        report_inherited=not args.ignore_inherited,
        redline_author=args.author,
    )

    started = time.perf_counter()
    try:
        result = compare(left, right, opts)
    except Exception as exc:  # noqa: BLE001 - CLI 中统一转成可读错误
        print(f"比较失败：{type(exc).__name__}: {exc}", file=sys.stderr)
        return 1
    elapsed = time.perf_counter() - started

    out_dir = Path(args.out)
    out_dir.mkdir(parents=True, exist_ok=True)
    stem = f"{left.stem}__vs__{right.stem}"

    produced: list[Path] = []
    if not args.no_html:
        produced.append(write_report(result, out_dir / f"{stem}.html", opts))
    if not args.no_summary:
        produced.append(write_summary(result, out_dir / f"{stem}.md",
                                      out_dir / f"{stem}.csv"))
    if not args.no_redline:
        produced.append(write_redline(result, out_dir / f"{stem}.修订.docx", opts))

    if not args.quiet:
        stats = result.stats
        print(f"比对完成，用时 {elapsed:.2f}s")
        print(f"  相似度 {stats.similarity:.1%}  "
              f"新增 {stats.inserted}  删除 {stats.deleted}  "
              f"修改 {stats.modified}  格式变更 {stats.format_changes}  "
              f"表格变更 {stats.tables_changed}")
        for path in produced:
            print(f"  → {path}")
    return 0



if __name__ == "__main__":  # pragma: no cover
    raise SystemExit(main())
