"""免安装运行入口：python run.py a.docx b.docx -o out

等价于 `PYTHONPATH=src python -m wordcompare ...`
"""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent / "src"))

from wordcompare.cli import main  # noqa: E402

if __name__ == "__main__":
    raise SystemExit(main())
