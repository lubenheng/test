# -*- coding: utf-8 -*-
import tkinter as tk
from tkinter import ttk


def create_iic_tab(app, notebook):
    frame_ops = ttk.Frame(notebook, padding=6)
    notebook.add(frame_ops, text="IIC操作寄存器")

    ttk.Label(frame_ops, text="ADDR (0~31):").grid(row=0, column=0, sticky='e', padx=2)
    ttk.Entry(frame_ops, textvariable=app.addr_var, width=6).grid(row=0, column=1, sticky='w')

    ttk.Label(frame_ops, text="INDEX (0~63):").grid(row=0, column=2, sticky='e')
    ttk.Entry(frame_ops, textvariable=app.index_var, width=6).grid(row=0, column=3, sticky='w')

    ttk.Label(frame_ops, text="写入数据(HEX):").grid(row=0, column=4, sticky='e')
    ttk.Entry(frame_ops, textvariable=app.write_data_var, width=15).grid(row=0, column=5, sticky='w')

    ttk.Label(frame_ops, text="导出地址(0-31):").grid(row=1, column=0, sticky='e')
    ttk.Entry(frame_ops, textvariable=app.export_addr_var, width=6).grid(row=1, column=1, sticky='w')
    ttk.Label(frame_ops, text="导出索引范围(0-63):").grid(row=1, column=2, sticky='e')
    ttk.Entry(frame_ops, textvariable=app.export_index_range_var, width=8).grid(row=1, column=3, sticky='w')

    btn_frame = ttk.Frame(frame_ops)
    btn_frame.grid(row=2, column=0, columnspan=6, pady=6)
    ttk.Button(btn_frame, text="单独写", command=app.do_fixed_write, width=10).pack(side='left', padx=4)
    ttk.Button(btn_frame, text="单独读", command=app.do_fixed_read, width=10).pack(side='left', padx=4)
    ttk.Button(btn_frame, text="写后读", command=app.do_write_then_read, width=10).pack(side='left', padx=4)
    ttk.Button(btn_frame, text="生成模板", command=app.generate_template, width=10).pack(side='left', padx=4)
    ttk.Button(btn_frame, text="批量执行", command=app.import_and_execute_batch, width=10).pack(side='left', padx=4)
    ttk.Button(btn_frame, text="读数据并导出", command=app.read_data_and_export_template, width=12).pack(side='left', padx=4)

    # Quick rows (simplified 5 rows)
    quick_frame = ttk.Frame(frame_ops, padding=4)
    quick_frame.grid(row=3, column=0, columnspan=6, pady=8, sticky='we')
    app.quick_rows = []
    for r in range(5):
        addr_var = tk.StringVar(value='0')
        index_var = tk.StringVar(value='0')
        b31 = tk.StringVar(value='00')
        b23 = tk.StringVar(value='00')
        b15 = tk.StringVar(value='00')
        b7  = tk.StringVar(value='00')
        ttk.Entry(quick_frame, textvariable=addr_var, width=5).grid(row=r, column=0)
        ttk.Entry(quick_frame, textvariable=index_var, width=5).grid(row=r, column=1)
        ttk.Entry(quick_frame, textvariable=b31, width=4).grid(row=r, column=2)
        ttk.Entry(quick_frame, textvariable=b23, width=4).grid(row=r, column=3)
        ttk.Entry(quick_frame, textvariable=b15, width=4).grid(row=r, column=4)
        ttk.Entry(quick_frame, textvariable=b7,  width=4).grid(row=r, column=5)
        ttk.Button(quick_frame, text="写", width=6, command=lambda ri=r, app=app: app.execute_quick_row_write(ri)).grid(row=r, column=6)
        ttk.Button(quick_frame, text="读", width=6, command=lambda ri=r, app=app: app.execute_quick_row_read(ri)).grid(row=r, column=7)
        ttk.Button(quick_frame, text="写后读", width=8, command=lambda ri=r, app=app: app.execute_quick_row_write_then_read(ri)).grid(row=r, column=8)
        app.quick_rows.append((addr_var, index_var, b31, b23, b15, b7))

