"""Word 文档比较引擎。

对外入口::

    from wordcompare import compare
    result = compare("a.docx", "b.docx")
"""

from .engine import compare
from .cli import main
from .options import Options
from .model import DiffResult, DiffItem, Paragraph, Table, Run

__all__ = ["compare", "Options", "DiffResult", "DiffItem", "Paragraph", "Table", "Run", "main"]
__version__ = "0.1.0"
