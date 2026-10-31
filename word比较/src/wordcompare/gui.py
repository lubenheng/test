"""图形界面：双击 exe 时使用。

只做一件事——选两个文件、点按钮、打开报告。所有比较逻辑复用 CLI 的同一条
流水线，保证 GUI 与命令行结果完全一致。
"""

from __future__ import annotations

import os
import queue
import subprocess
import sys
import threading
import webbrowser
from pathlib import Path

import tkinter as tk
from tkinter import filedialog, messagebox, ttk

from .engine import compare
from .options import Options
from .output.docx_redline import write_redline
from .output.html_report import write_report
from .output.summary import write_summary

DOC_TYPES = [("Word 文档", "*.docx"), ("所有文件", "*.*")]


class App:
    def __init__(self, root: tk.Tk):
        self.root = root
        self.queue: queue.Queue[tuple[str, object]] = queue.Queue()
        self.produced: list[Path] = []

        root.title("Word 文档比较")
        root.geometry("720x600")
        root.minsize(640, 540)

        self.left_var = tk.StringVar()
        self.right_var = tk.StringVar()
        self.out_var = tk.StringVar(value=str(Path.cwd() / "输出"))
        self.threshold_var = tk.DoubleVar(value=0.5)
        self.text_var = tk.BooleanVar(value=True)
        self.format_var = tk.BooleanVar(value=True)
        self.table_var = tk.BooleanVar(value=True)
        self.redline_var = tk.BooleanVar(value=True)

        self._build()
        self._poll()

    # ------------------------------------------------------------------ UI
    def _build(self) -> None:
        pad = {"padx": 14}
        outer = ttk.Frame(self.root, padding=16)
        outer.pack(fill="both", expand=True)

        self._file_row(outer, 0, "基准文档（旧稿）", self.left_var)
        self._file_row(outer, 1, "对比文档（新稿）", self.right_var)

        out_row = ttk.Frame(outer)
        out_row.grid(row=2, column=0, sticky="ew", pady=(8, 0), **pad)
        out_row.columnconfigure(1, weight=1)
        ttk.Label(out_row, text="输出目录", width=16, anchor="w").grid(row=0, column=0)
        ttk.Entry(out_row, textvariable=self.out_var).grid(row=0, column=1, sticky="ew")
        ttk.Button(out_row, text="浏览…", width=8,
                   command=self._pick_out_dir).grid(row=0, column=2, padx=(6, 0))

        scope = ttk.LabelFrame(outer, text="比较范围", padding=10)
        scope.grid(row=3, column=0, sticky="ew", pady=(14, 0), **pad)
        ttk.Checkbutton(scope, text="文本：正文与单元格的文字差异",
                        variable=self.text_var).grid(row=0, column=0, sticky="w")
        ttk.Checkbutton(scope, text="格式：加粗、字号、颜色、对齐、编号等",
                        variable=self.format_var).grid(row=1, column=0, sticky="w",
                                                       pady=(6, 0))
        ttk.Checkbutton(scope, text="表格：表格内容与行列变化",
                        variable=self.table_var).grid(row=2, column=0, sticky="w",
                                                      pady=(6, 0))

        opts = ttk.LabelFrame(outer, text="选项", padding=10)
        opts.grid(row=4, column=0, sticky="ew", pady=(10, 0), **pad)

        ttk.Label(opts, text="相似度阈值").grid(row=0, column=0, sticky="w")
        ttk.Scale(opts, from_=0.2, to=0.9, variable=self.threshold_var,
                  orient="horizontal", length=200).grid(row=0, column=1, padx=8)
        self.threshold_label = ttk.Label(opts, text="0.50")
        self.threshold_label.grid(row=0, column=2, sticky="w")
        self.threshold_var.trace_add(
            "write",
            lambda *_: self.threshold_label.configure(
                text=f"{self.threshold_var.get():.2f}"))

        ttk.Checkbutton(opts, text="生成带修订的 .docx",
                        variable=self.redline_var).grid(row=1, column=0,
                                                        columnspan=3, sticky="w",
                                                        pady=(8, 0))

        self.run_btn = ttk.Button(outer, text="开始比较", command=self._start)
        self.run_btn.grid(row=5, column=0, pady=(14, 0), **pad, sticky="ew")

        log_frame = ttk.LabelFrame(outer, text="运行日志", padding=8)
        log_frame.grid(row=6, column=0, sticky="nsew", pady=(12, 0), **pad)
        log_frame.rowconfigure(0, weight=1)
        log_frame.columnconfigure(0, weight=1)

        self.log = tk.Text(log_frame, height=8, wrap="word", state="disabled",
                           font=("Consolas", 9))
        self.log.grid(row=0, column=0, sticky="nsew")
        scroll = ttk.Scrollbar(log_frame, orient="vertical",
                               command=self.log.yview)
        scroll.grid(row=0, column=1, sticky="ns")
        self.log.configure(yscrollcommand=scroll.set)

        actions = ttk.Frame(outer)
        actions.grid(row=7, column=0, sticky="ew", pady=(10, 0), **pad)
        self.open_btn = ttk.Button(actions, text="打开报告", state="disabled",
                                   command=self._open_report)
        self.open_btn.pack(side="left")
        self.dir_btn = ttk.Button(actions, text="打开输出目录", state="disabled",
                                  command=self._open_dir)
        self.dir_btn.pack(side="left", padx=8)
        ttk.Button(actions, text="退出", command=self.root.destroy).pack(side="right")

        outer.columnconfigure(0, weight=1)
        outer.rowconfigure(6, weight=1)
        self.root.bind("<Return>", lambda _e: self._start())

    def _file_row(self, parent, row: int, label: str, var: tk.StringVar) -> None:
        frame = ttk.Frame(parent)
        frame.grid(row=row, column=0, sticky="ew", pady=(0 if row == 0 else 8, 0),
                   padx=14)
        frame.columnconfigure(1, weight=1)
        ttk.Label(frame, text=label, width=16, anchor="w").grid(row=0, column=0)
        ttk.Entry(frame, textvariable=var).grid(row=0, column=1, sticky="ew")
        ttk.Button(frame, text="选择…", width=8,
                   command=lambda: self._pick_doc(var)).grid(row=0, column=2,
                                                             padx=(6, 0))

    # ------------------------------------------------------------- handlers
    def _pick_doc(self, var: tk.StringVar) -> None:
        path = filedialog.askopenfilename(title="选择 Word 文档",
                                          filetypes=DOC_TYPES)
        if not path:
            return
        if path.lower().endswith(".doc"):
            messagebox.showwarning("格式不支持",
                                   "这是旧版 .doc，请先用 Word 另存为 .docx。")
            return
        var.set(path)

    def _pick_out_dir(self) -> None:
        path = filedialog.askdirectory(title="选择输出目录")
        if path:
            self.out_var.set(path)

    def _start(self) -> None:
        left, right = Path(self.left_var.get()), Path(self.right_var.get())
        if not left.is_file() or not right.is_file():
            messagebox.showwarning("缺少文件", "请先选择两份 .docx 文档。")
            return
        if not (self.text_var.get() or self.format_var.get()
                or self.table_var.get()):
            messagebox.showwarning("比较范围", "请至少勾选一项比较范围。")
            return
        self.produced.clear()
        self.open_btn.configure(state="disabled")
        self.dir_btn.configure(state="disabled")
        self.run_btn.configure(state="disabled", text="比较中…")
        self._log("开始比较…")

        opts = Options(
            similarity_threshold=round(float(self.threshold_var.get()), 2),
            compare_text=bool(self.text_var.get()),
            compare_tables=bool(self.table_var.get()),
            format_diff=bool(self.format_var.get()),
        )
        args = (left, right, Path(self.out_var.get()),
                opts, bool(self.redline_var.get()))
        threading.Thread(target=self._run, args=args, daemon=True).start()

    def _run(self, left: Path, right: Path, out_dir: Path,
             opts: Options, redline: bool) -> None:
        try:
            result = compare(left, right, opts)
            out_dir.mkdir(parents=True, exist_ok=True)
            stem = f"{left.stem}__vs__{right.stem}"

            produced = [write_report(result, out_dir / f"{stem}.html", opts),
                        write_summary(result, out_dir / f"{stem}.md",
                                      out_dir / f"{stem}.csv")]
            if redline:
                produced.append(write_redline(result, out_dir / f"{stem}.修订.docx", opts))
            self.queue.put(("done", (result, produced)))
        except Exception as exc:  # noqa: BLE001 - GUI 中统一转为提示
            self.queue.put(("error", f"{type(exc).__name__}: {exc}"))

    # ---------------------------------------------------------------- polling
    def _poll(self) -> None:
        try:
            while True:
                kind, payload = self.queue.get_nowait()
                if kind == "done":
                    self._finish(*payload)
                elif kind == "error":
                    self._fail(payload)
        except queue.Empty:
            pass
        self.root.after(120, self._poll)

    def _finish(self, result, produced: list[Path]) -> None:
        self.produced = produced
        stats = result.stats
        self._log("")
        self._log(f"相似度 {stats.similarity:.1%}　"
                  f"新增 {stats.inserted}　删除 {stats.deleted}　"
                  f"修改 {stats.modified}　格式变更 {stats.format_changes}")
        for path in produced:
            self._log(f"→ {path}")

        self.run_btn.configure(state="normal", text="开始比较")
        self.open_btn.configure(state="normal")
        self.dir_btn.configure(state="normal")
        if self.produced:
            self._open_report()

    def _fail(self, message: str) -> None:
        self.run_btn.configure(state="normal", text="开始比较")
        self._log(f"出错：{message}")
        messagebox.showerror("比较失败", message)

    # ---------------------------------------------------------------- actions
    def _log(self, text: str) -> None:
        self.log.configure(state="normal")
        self.log.insert("end", text + "\n")
        self.log.see("end")
        self.log.configure(state="disabled")

    def _open_report(self) -> None:
        html = next((p for p in self.produced if p.suffix == ".html"), None)
        if html is not None:
            webbrowser.open(html.resolve().as_uri())

    def _open_dir(self) -> None:
        target = Path(self.out_var.get())
        if not target.is_dir():
            return
        if sys.platform.startswith("win"):
            os.startfile(str(target))          # noqa: S606
        elif sys.platform == "darwin":
            subprocess.Popen(["open", str(target)])
        else:
            subprocess.Popen(["xdg-open", str(target)])


def _hide_console() -> None:
    """打包成 exe 双击运行时，把随附的控制台窗口收起来。"""
    if not getattr(sys, "frozen", False) or not sys.platform.startswith("win"):
        return
    try:
        from ctypes import windll
        windll.kernel32.GetConsoleWindow.restype = windll.kernel32.GetConsoleWindow  # noqa: E501
        hwnd = windll.kernel32.GetConsoleWindow()
        if hwnd:
            windll.user32.ShowWindow(hwnd, 0)   # SW_HIDE
    except Exception:  # noqa: BLE001
        pass


def main() -> int:
    # 先建窗口再隐藏控制台：万一 tkinter 初始化失败，控制台里还留有线索
    root = tk.Tk()
    _hide_console()
    try:
        # 高 DPI 下界面不糊
        if sys.platform.startswith("win"):
            from ctypes import windll
            windll.shcore.SetProcessDpiAwareness(1)
    except Exception:  # noqa: BLE001
        pass
    App(root)
    try:
        root.mainloop()
    except KeyboardInterrupt:
        pass
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
