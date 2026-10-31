"""统一入口：带参数走命令行，不带参数打开图形界面。

* 打包 exe：`python build_exe.py`（或 `python build_exe.py --onedir`）
* 源码运行：`python app.py`（GUI） / `python app.py a.docx b.docx`（CLI）

分发到别人机器时，任何启动失败都会写到
``%LOCALAPPDATA%\\WordCompare\\启动日志.txt`` 并弹窗提示，
避免"双击没反应、也看不到报错"。
"""

from __future__ import annotations

import os
import sys
import traceback
from datetime import datetime
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent / "src"))

APP_NAME = "WordCompare"
LOG_LIMIT = 512 * 1024


def log_path() -> Path:
    base = (os.environ.get("LOCALAPPDATA") or os.environ.get("TEMP")
            or os.path.expanduser("~"))
    return Path(base) / APP_NAME / "启动日志.txt"


def write_log(text: str) -> Path | None:
    """追加一行日志；失败时退回到 exe 同目录。"""
    candidates = [log_path()]
    try:
        candidates.append(Path(sys.executable).resolve().parent / "启动日志.txt")
    except Exception:  # noqa: BLE001
        pass
    for path in candidates:
        try:
            path.parent.mkdir(parents=True, exist_ok=True)
            if path.exists() and path.stat().st_size > LOG_LIMIT:
                path.unlink()
            with path.open("a", encoding="utf-8") as fh:
                fh.write(text)
            return path
        except Exception:  # noqa: BLE001
            continue
    return None


def show_error(title: str, message: str) -> None:
    """控制台可能已被隐藏，用系统消息框保证用户能看到。"""
    if sys.platform.startswith("win"):
        try:
            import ctypes
            ctypes.windll.user32.MessageBoxW(None, message, title, 0x10)
            return
        except Exception:  # noqa: BLE001
            pass
    print(f"{title}\n{message}", file=sys.stderr)


def _run() -> int:
    argv = sys.argv[1:]
    if argv and argv[0] not in ("--gui",):
        import wordcompare.cli as cli
        return cli.main(argv)

    from wordcompare import __version__, gui
    write_log(f"[{datetime.now():%Y-%m-%d %H:%M:%S}] 启动 v{__version__} "
              f"frozen={getattr(sys, 'frozen', False)} "
              f"exe={sys.executable} cwd={os.getcwd()}\n")
    return gui.main()


def main() -> int:
    gui_mode = not sys.argv[1:] or sys.argv[1] == "--gui"
    try:
        return _run()
    except SystemExit:
        raise
    except BaseException:  # noqa: BLE001 - 兜底：把任何启动期异常变成可读信息
        detail = traceback.format_exc()
        log = write_log(f"[{datetime.now():%Y-%m-%d %H:%M:%S}] 启动失败\n{detail}\n")
        if gui_mode:
            show_error(
                "Word 文档比较 启动失败",
                "程序启动时出错，请把下面的信息发给开发者。\n\n"
                f"{detail.strip().splitlines()[-1]}\n\n"
                f"完整日志：{log or '（无法写入磁盘）'}",
            )
        else:
            print(detail, file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
