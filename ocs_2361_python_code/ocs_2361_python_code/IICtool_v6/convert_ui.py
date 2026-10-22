#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
简单 Tkinter 界面：选择 MTP 文件、配置寄存器文件、执行转换（默认不输出 report）。

运行：
  c:/.../.venv/Scripts/python.exe convert_ui.py
"""

from __future__ import annotations
import os
import tkinter as tk
from tkinter import ttk, filedialog, messagebox

from apply_numeric_overlay import apply_overlay


class ConverterUI(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title('MTP 数值覆盖工具')
        self.geometry('700x140')

        frm = ttk.Frame(self, padding=10)
        frm.pack(fill='both', expand=True)

        # Target file
        ttk.Label(frm, text='目标 MTP 文件:').grid(row=0, column=0, sticky='w')
        self.target_var = tk.StringVar(value='MTP_预定位_IF_FOC150K五段式SP备份20260424.xlsx')
        ttk.Entry(frm, textvariable=self.target_var, width=70).grid(row=0, column=1, padx=6)
        ttk.Button(frm, text='浏览', command=self.browse_target).grid(row=0, column=2)

        # Source file
        ttk.Label(frm, text='配置寄存器文件:').grid(row=1, column=0, sticky='w')
        self.source_var = tk.StringVar(value='2361_配置寄存器_V0.4.xlsx')
        ttk.Entry(frm, textvariable=self.source_var, width=70).grid(row=1, column=1, padx=6)
        ttk.Button(frm, text='浏览', command=self.browse_source).grid(row=1, column=2)

        # Options
        self.report_var = tk.BooleanVar(value=False)
        ttk.Checkbutton(frm, text='生成 report (CSV)', variable=self.report_var).grid(row=2, column=1, sticky='w')

        # Convert button
        ttk.Button(frm, text='开始转换', command=self.on_convert).grid(row=3, column=1, pady=10)

        self.status = tk.StringVar(value='就绪')
        ttk.Label(frm, textvariable=self.status).grid(row=4, column=0, columnspan=3, sticky='w')

    def browse_target(self):
        p = filedialog.askopenfilename(title='选择目标 MTP 文件', filetypes=[('Excel 文件', '*.xlsx;*.xlsm;*.xls')])
        if p:
            self.target_var.set(p)

    def browse_source(self):
        p = filedialog.askopenfilename(title='选择配置寄存器文件', filetypes=[('Excel 文件', '*.xlsx;*.xlsm;*.xls')])
        if p:
            self.source_var.set(p)

    def on_convert(self):
        target = self.target_var.get()
        source = self.source_var.get()
        # 直接使用默认配置文件
        cfg = 'config.py'
        module_filter = None
        out_default = os.path.splitext(target)[0] + '.converted.xlsx'
        out = filedialog.asksaveasfilename(title='保存为', defaultextension='.xlsx', initialfile=os.path.basename(out_default), filetypes=[('Excel 文件','*.xlsx')])
        if not out:
            return
        report = None
        if self.report_var.get():
            report = os.path.splitext(out)[0] + '.report.csv'

        try:
            self.status.set('处理中...')
            self.update_idletasks()
            res = apply_overlay(target, source, out, None, None, report, module_filter, cfg)
            msg = f"完成：更新 {res.get('updated')} 行，追加源行 {res.get('appended_source')} 行。"
            messagebox.showinfo('转换完成', msg)
            self.status.set(msg)
        except Exception as e:
            messagebox.showerror('错误', str(e))
            self.status.set('出错')


if __name__ == '__main__':
    app = ConverterUI()
    app.mainloop()