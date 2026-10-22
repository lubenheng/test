# -*- coding: utf-8 -*-
import tkinter as tk
from tkinter import ttk


def create_mtp_tab(app, notebook):
    frame = ttk.Frame(notebook, padding=6)
    notebook.add(frame, text="MTP测设模式")

    ttk.Label(frame, text="ADDR(MTP Byte, 0~511):").grid(row=0, column=0, sticky='e')
    ttk.Entry(frame, textvariable=app.mtp_addr_var, width=8).grid(row=0, column=1, sticky='w')
    ttk.Label(frame, text="Byte Data(HEX):").grid(row=0, column=2, sticky='e')
    ttk.Entry(frame, textvariable=app.mtp_data_var, width=8).grid(row=0, column=3, sticky='w')

    mtp_btn_frame = ttk.Frame(frame)
    mtp_btn_frame.grid(row=1, column=0, columnspan=6, pady=10)
    ttk.Button(mtp_btn_frame, text="Open MTP EN", command=app.do_mtp_open_en, width=12).pack(side='left', padx=4)
    ttk.Button(mtp_btn_frame, text="Close MTP EN", command=app.do_mtp_close_en, width=12).pack(side='left', padx=4)
    ttk.Button(mtp_btn_frame, text="Read Byte", command=app.do_mtp_read_byte, width=10).pack(side='left', padx=4)
    ttk.Button(mtp_btn_frame, text="Program Byte", command=app.do_mtp_program_byte, width=12).pack(side='left', padx=4)
    ttk.Button(mtp_btn_frame, text="Sector Erase", command=app.do_mtp_sector_erase, width=12).pack(side='left', padx=4)
    ttk.Button(mtp_btn_frame, text="Chip Erase", command=app.do_mtp_chip_erase, width=12).pack(side='left', padx=4)

    mtp_batch_frame = ttk.Frame(frame)
    mtp_batch_frame.grid(row=2, column=0, columnspan=6, pady=4)
    ttk.Button(mtp_batch_frame, text="生成MTP模板", command=app.generate_mtp_template, width=12).pack(side='left', padx=4)
    ttk.Button(mtp_batch_frame, text="全片擦写+校验", command=app.mtp_full_program_with_crc, width=16).pack(side='left', padx=4)
    ttk.Button(mtp_batch_frame, text="读出MTP并导出", command=app.read_mtp_and_export_template, width=16).pack(side='left', padx=4)

    mtp_sector_frame = ttk.Frame(frame)
    mtp_sector_frame.grid(row=3, column=0, columnspan=6, pady=6)
    ttk.Label(mtp_sector_frame, text="操作扇区(0~15):").pack(side='left')
    ttk.Entry(mtp_sector_frame, textvariable=app.mtp_target_sector_var, width=6).pack(side='left', padx=4)
    ttk.Label(mtp_sector_frame, text="写入统一数据(Hex):").pack(side='left')
    ttk.Entry(mtp_sector_frame, textvariable=app.mtp_sector_val_var, width=6).pack(side='left', padx=4)
    ttk.Button(mtp_sector_frame, text="写本扇区(不擦除)", command=app.write_mtp_sector_data, width=16).pack(side='left', padx=6)
    ttk.Button(mtp_sector_frame, text="读本扇区并导出", command=app.read_and_export_mtp_sector, width=16).pack(side='left', padx=4)

