# -*- coding: utf-8 -*-
import time
import tkinter as tk
from tkinter import ttk, scrolledtext, messagebox, filedialog
import os
import sys
import serial.tools.list_ports
import threading
import traceback

from utils import CONFIG_FILE, DEFAULT_CONFIG, load_config, save_config
from i2c_comm import I2CController
from mtp_ops import MTPController
from iic_ops import IICLogic, execute_write, execute_read
from mtp_batch_ops import MTPBatchLogic, CRC_ADDR_LIMIT


class I2CGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("IIC Tool v6")
        self.root.geometry("1000x700")

        self.log_records = []
        self.config = load_config()
        self.i2c = None
        self.mtp = None

        # UI state variables
        self.port_var = tk.StringVar(value=self.config.get('port', DEFAULT_CONFIG['port']))
        self.iic_speed_var = tk.StringVar(value=self.config.get('iic_speed', DEFAULT_CONFIG['iic_speed']))

        self.addr_var = tk.StringVar(value="19")
        self.index_var = tk.StringVar(value="0")
        self.write_data_var = tk.StringVar(value="00 00 00 00")
        self.export_addr_var = tk.StringVar(value="19")
        self.export_index_range_var = tk.StringVar(value="0-0")

        self.mtp_addr_var = tk.StringVar(value="0")
        self.mtp_data_var = tk.StringVar(value="00")
        self.mtp_target_sector_var = tk.StringVar(value="0")
        self.mtp_sector_val_var = tk.StringVar(value="FF")

        self.status_var = tk.StringVar(value="状态: 未初始化")

        self.quick_rows = []

        self.create_widgets()

    def create_widgets(self):
        top_frame = ttk.Frame(self.root, padding=6)
        top_frame.pack(fill='x', padx=8, pady=6)

        ttk.Label(top_frame, text='Port:').pack(side='left')
        self.port_cmb = ttk.Combobox(top_frame, textvariable=self.port_var, width=12)
        self.port_cmb.pack(side='left', padx=4)
        self.refresh_port_btn = ttk.Button(top_frame, text='刷新', width=4,
                                           command=lambda: self.refresh_ports(auto_select=False))
        self.refresh_port_btn.pack(side='left')
        self.refresh_ports()

        ttk.Label(top_frame, text='IIC速率:').pack(side='left', padx=6)
        self.iic_speed_cmb = ttk.Combobox(top_frame, values=['1k','5k','10k','20k','50k','80k','100k','200k','400k'], textvariable=self.iic_speed_var, width=6, state='readonly')
        self.iic_speed_cmb.pack(side='left', padx=4)

        self.init_btn = ttk.Button(top_frame, text='初始化I2C', command=self.initialize_i2c)
        self.init_btn.pack(side='left', padx=6)
        self.close_btn = ttk.Button(top_frame, text='关闭串口', command=self.close_serial, state='disabled')
        self.close_btn.pack(side='left', padx=4)
        self.spi_sck_btn = ttk.Button(top_frame, text='触发SCK', command=self.trigger_spi_sck_direct)
        self.spi_sck_btn.pack(side='left', padx=4)

        ttk.Label(top_frame, textvariable=self.status_var).pack(side='right')

        self.notebook = ttk.Notebook(self.root)
        self.notebook.pack(fill='both', expand=True, padx=8, pady=6)

        # IIC 操作寄存器 tab
        frame_ops = ttk.Frame(self.notebook, padding=6)
        self.notebook.add(frame_ops, text="IIC操作寄存器")

        ttk.Label(frame_ops, text="ADDR (0~31):").grid(row=0, column=0, sticky='e', padx=2)
        ttk.Entry(frame_ops, textvariable=self.addr_var, width=6).grid(row=0, column=1, sticky='w')
        ttk.Label(frame_ops, text="INDEX (0~63):").grid(row=0, column=2, sticky='e')
        ttk.Entry(frame_ops, textvariable=self.index_var, width=6).grid(row=0, column=3, sticky='w')
        ttk.Label(frame_ops, text="写入数据(HEX):").grid(row=0, column=4, sticky='e')
        ttk.Entry(frame_ops, textvariable=self.write_data_var, width=15).grid(row=0, column=5, sticky='w')

        ttk.Label(frame_ops, text="导出地址(0-31):").grid(row=1, column=0, sticky='e')
        ttk.Entry(frame_ops, textvariable=self.export_addr_var, width=6).grid(row=1, column=1, sticky='w')
        ttk.Label(frame_ops, text="导出索引范围(0-63):").grid(row=1, column=2, sticky='e')
        ttk.Entry(frame_ops, textvariable=self.export_index_range_var, width=8).grid(row=1, column=3, sticky='w')

        btn_frame = ttk.Frame(frame_ops)
        btn_frame.grid(row=2, column=0, columnspan=6, pady=6)
        ttk.Button(btn_frame, text="单独写", command=self.do_fixed_write, width=10).pack(side='left', padx=4)
        ttk.Button(btn_frame, text="单独读", command=self.do_fixed_read, width=10).pack(side='left', padx=4)
        ttk.Button(btn_frame, text="写后读", command=self.do_write_then_read, width=10).pack(side='left', padx=4)
        ttk.Button(btn_frame, text="生成模板", command=self.generate_template, width=10).pack(side='left', padx=4)
        ttk.Button(btn_frame, text="批量执行", command=self.import_and_execute_batch, width=10).pack(side='left', padx=4)
        ttk.Button(btn_frame, text="批量读回并覆盖", command=self.import_and_update_reads, width=14).pack(side='left', padx=4)
        ttk.Button(btn_frame, text="读数据并导出", command=self.read_data_and_export_template, width=12).pack(side='left', padx=4)

        quick_frame = ttk.Frame(frame_ops, padding=4)
        quick_frame.grid(row=3, column=0, columnspan=6, pady=8, sticky='we')
        for col in range(9):
            quick_frame.columnconfigure(col, weight=1, uniform='qc')
        ttk.Label(quick_frame, text="ADDR", font=("", 8)).grid(row=0, column=0, sticky='s')
        ttk.Label(quick_frame, text="INDEX", font=("", 8)).grid(row=0, column=1, sticky='s')
        ttk.Label(quick_frame, text="[31:24]", font=("", 8)).grid(row=0, column=2, sticky='s')
        ttk.Label(quick_frame, text="[23:16]", font=("", 8)).grid(row=0, column=3, sticky='s')
        ttk.Label(quick_frame, text="[15:8]", font=("", 8)).grid(row=0, column=4, sticky='s')
        ttk.Label(quick_frame, text="[7:0]", font=("", 8)).grid(row=0, column=5, sticky='s')
        self.quick_rows = []
        for r in range(10):
            addr_var = tk.StringVar(value='0')
            index_var = tk.StringVar(value='0')
            b31 = tk.StringVar(value='00')
            b23 = tk.StringVar(value='00')
            b15 = tk.StringVar(value='00')
            b7  = tk.StringVar(value='00')
            ttk.Entry(quick_frame, textvariable=addr_var, width=5).grid(row=r + 1, column=0, sticky='ew', padx=2)
            ttk.Entry(quick_frame, textvariable=index_var, width=5).grid(row=r + 1, column=1, sticky='ew', padx=2)
            ttk.Entry(quick_frame, textvariable=b31, width=4).grid(row=r + 1, column=2, sticky='ew', padx=2)
            ttk.Entry(quick_frame, textvariable=b23, width=4).grid(row=r + 1, column=3, sticky='ew', padx=2)
            ttk.Entry(quick_frame, textvariable=b15, width=4).grid(row=r + 1, column=4, sticky='ew', padx=2)
            ttk.Entry(quick_frame, textvariable=b7,  width=4).grid(row=r + 1, column=5, sticky='ew', padx=2)
            ttk.Button(quick_frame, text="写", width=6, command=lambda ri=r: self.execute_quick_row_write(ri)).grid(row=r + 1, column=6, sticky='ew', padx=2)
            ttk.Button(quick_frame, text="读", width=6, command=lambda ri=r: self.execute_quick_row_read(ri)).grid(row=r + 1, column=7, sticky='ew', padx=2)
            ttk.Button(quick_frame, text="写后读", width=8, command=lambda ri=r: self.execute_quick_row_write_then_read(ri)).grid(row=r + 1, column=8, sticky='ew', padx=2)
            self.quick_rows.append((addr_var, index_var, b31, b23, b15, b7))

        # MTP tab
        self.mtp_ops_frame = ttk.Frame(self.notebook, padding=6)
        self.notebook.add(self.mtp_ops_frame, text="MTP测设模式")

        ttk.Label(self.mtp_ops_frame, text="ADDR(MTP Byte, 0~511):").grid(row=0, column=0, sticky='e')
        ttk.Entry(self.mtp_ops_frame, textvariable=self.mtp_addr_var, width=8).grid(row=0, column=1, sticky='w')
        ttk.Label(self.mtp_ops_frame, text="Byte Data(HEX):").grid(row=0, column=2, sticky='e')
        ttk.Entry(self.mtp_ops_frame, textvariable=self.mtp_data_var, width=8).grid(row=0, column=3, sticky='w')

        mtp_btn_frame = ttk.Frame(self.mtp_ops_frame)
        mtp_btn_frame.grid(row=1, column=0, columnspan=6, pady=10)
        ttk.Button(mtp_btn_frame, text="Open MTP EN", command=self.do_mtp_open_en, width=12).pack(side='left', padx=4)
        ttk.Button(mtp_btn_frame, text="Close MTP EN", command=self.do_mtp_close_en, width=12).pack(side='left', padx=4)
        ttk.Button(mtp_btn_frame, text="Read Byte", command=self.do_mtp_read_byte, width=10).pack(side='left', padx=4)
        ttk.Button(mtp_btn_frame, text="Program Byte", command=self.do_mtp_program_byte, width=12).pack(side='left', padx=4)
        ttk.Button(mtp_btn_frame, text="Sector Erase", command=self.do_mtp_sector_erase, width=12).pack(side='left', padx=4)
        ttk.Button(mtp_btn_frame, text="Chip Erase", command=self.do_mtp_chip_erase, width=12).pack(side='left', padx=4)

        mtp_batch_frame = ttk.Frame(self.mtp_ops_frame)
        mtp_batch_frame.grid(row=2, column=0, columnspan=6, pady=4)
        ttk.Button(mtp_batch_frame, text="生成MTP模板", command=self.generate_mtp_template, width=12).pack(side='left', padx=4)
        ttk.Button(mtp_batch_frame, text="全片擦写+校验", command=self.mtp_full_program_with_crc, width=16).pack(side='left', padx=4)
        ttk.Button(mtp_batch_frame, text="读出MTP并导出", command=self.read_mtp_and_export_template, width=16).pack(side='left', padx=4)

        mtp_sector_frame = ttk.Frame(self.mtp_ops_frame)
        mtp_sector_frame.grid(row=3, column=0, columnspan=6, pady=6)
        ttk.Label(mtp_sector_frame, text="操作扇区(0~15):").pack(side='left')
        ttk.Entry(mtp_sector_frame, textvariable=self.mtp_target_sector_var, width=6).pack(side='left', padx=4)
        ttk.Label(mtp_sector_frame, text="写入统一数据(Hex):").pack(side='left')
        ttk.Entry(mtp_sector_frame, textvariable=self.mtp_sector_val_var, width=6).pack(side='left', padx=4)
        ttk.Button(mtp_sector_frame, text="写本扇区(不擦除)", command=self.write_mtp_sector_data, width=16).pack(side='left', padx=6)
        ttk.Button(mtp_sector_frame, text="读本扇区并导出", command=self.read_and_export_mtp_sector, width=16).pack(side='left', padx=4)

        # Log area
        frame_log = ttk.LabelFrame(self.root, text="操作日志", padding=6)
        frame_log.pack(fill='both', expand=True, padx=8, pady=6)
        self.log_text = scrolledtext.ScrolledText(frame_log, width=100, height=20, state='disabled')
        self.log_text.pack(fill='both', expand=True)

        frame_bottom = ttk.Frame(self.root)
        frame_bottom.pack(fill='x', padx=8, pady=6)
        ttk.Button(frame_bottom, text="导出日志", command=self.export_to_excel).pack(side='left', padx=6)
        ttk.Button(frame_bottom, text="主动拉高/恢复死锁", command=self.do_recover_bus).pack(side='left', padx=6)

    # ==================== 工具方法 ====================

    def log(self, msg, record=None):
        timestamp = time.strftime('%Y-%m-%d %H:%M:%S')
        full_msg = f"[{timestamp}] {msg}"
        try:
            self.log_text.configure(state='normal')
            self.log_text.insert(tk.END, full_msg + "\n")
            self.log_text.configure(state='disabled')
            self.log_text.yview(tk.END)
        except Exception:
            print(full_msg)

        if record is None:
            self.log_records.append({
                'time': timestamp, 'operation': '', 'addr': '', 'index': '',
                'write_value': '', 'read_value': '', 'raw_message': msg
            })
        else:
            self.log_records.append(record)

    def update_status(self, text, is_error=False):
        self.status_var.set(f"状态: {text}")
        if is_error:
            self.log(f"!! {text}")

    def refresh_ports(self, auto_select=True):
        """扫描系统串口并更新下拉列表。

        auto_select=True （启动时）：当前值不在列表中则自动改选一个 USB 口；
        auto_select=False（点「刷新」）：只更新列表，保留用户已填写的值。
        下拉框可编辑，允许直接手动输入 COM 口（含未识别的口）。
        """
        try:
            ports = list(serial.tools.list_ports.comports())
        except Exception as e:
            self.port_cmb['values'] = []
            self.log(f"!! 扫描串口失败: {e}")
            return

        items = [p.device for p in ports]
        all_values = list(items)
        current = self.port_var.get().strip()
        # 把当前手填的值也放进候选，避免刷新后它从列表里消失
        if current and current not in all_values:
            all_values.append(current)
        self.port_cmb['values'] = all_values

        if not auto_select:
            return
        if items and current not in items:
            # 优先选中描述里带 USB 的端口
            descs = {p.device: (p.description or '') for p in ports}
            preferred = next((d for d in items if 'USB' in descs.get(d, '').upper()), items[0])
            self.port_var.set(preferred)
            self.log(f"→ 自动识别串口: {preferred}（{descs.get(preferred, '无描述')}）")

    def get_inputs(self):
        try:
            port = self.port_var.get().strip()
            if not port:
                raise ValueError("串口号不能为空")
            slave_addr_str = str(self.config.get("slave_addr", "0x52")).strip()
            slave_addr = int(slave_addr_str, 16) if slave_addr_str else 0x52
            return port, slave_addr
        except ValueError as e:
            messagebox.showerror("输入错误", f"参数格式不正确:\n{e}")
            raise

    def ensure_initialized(self):
        if not self.i2c or not self.i2c.initialized:
            messagebox.showwarning("未初始化", "请先点击「初始化I2C」按钮")
            return False
        return True

    def _do_write(self, addr, index, write_data, suppress_log=False):
        execute_write(self.i2c, addr, index, write_data, log_cb=self.log, suppress_log=suppress_log)

    def _do_read(self, addr, index):
        return execute_read(self.i2c, addr, index, log_cb=self.log)

    def initialize_i2c(self):
        try:
            port, slave_addr = self.get_inputs()
            if self.i2c:
                try: self.i2c.close()
                except Exception: pass
            self.i2c = I2CController(port, slave_addr)
            self.i2c.open()
            self.i2c.init_i2c(log_callback=self.log)
            try:
                if hasattr(self, 'iic_speed_var'):
                    self.i2c.set_iic_speed(self.iic_speed_var.get())
                    self.log(f"→ 已应用 IIC 速率: {self.iic_speed_var.get()}")
            except Exception as e:
                self.log(f"!! 设置 IIC 速率失败: {e}")

            self.mtp = MTPController(self._do_write, self._do_read, log_callback=self.log)
            self.mtp.gui_ref = self

            try:
                save_config(port, f"0x{slave_addr:02X}", iic_speed=self.iic_speed_var.get())
            except Exception:
                save_config(port, f"0x{slave_addr:02X}")

            self.update_status("已初始化 (原始IIC模式)")
            self.init_btn.config(state='disabled')
            try: self.spi_sck_btn.config(state='disabled')
            except Exception: pass
            self.close_btn.config(state='normal')
            self.port_cmb.config(state='disabled')
            try:
                if hasattr(self, 'iic_speed_cmb'):
                    self.iic_speed_cmb.config(state='disabled')
            except Exception: pass
        except Exception as e:
            messagebox.showerror("初始化失败", str(e))
            self.update_status(f"初始化失败: {e}", is_error=True)

    def close_serial(self):
        if self.i2c:
            try: self.i2c.close()
            except Exception: pass
            self.i2c = None
            self.mtp = None
        self.update_status("串口已关闭")
        try:
            self.init_btn.config(state='normal')
            self.close_btn.config(state='disabled')
            self.port_cmb.config(state='normal')
            if hasattr(self, 'iic_speed_cmb'):
                self.iic_speed_cmb.config(state='readonly')
            self.spi_sck_btn.config(state='normal')
        except Exception: pass

    def trigger_spi_sck_direct(self):
        threading.Thread(target=self._run_spi_sck_no_gui, daemon=True).start()

    def _run_spi_sck_no_gui(self, pulse_count=30, total_us=500):
        try:
            temp_i2c = self.i2c
            created_temp = False
            if temp_i2c is None:
                port, slave_addr = self.get_inputs()
                temp_i2c = I2CController(port, slave_addr)
                temp_i2c._ensure_dll_ready(log_callback=self.log)
                created_temp = True

            self.log(f"SPI_SCK_DIRECT | 开始发送 {pulse_count} 脉冲，总时长 {total_us} µs (使用 PWM)")
            try:
                base_clock = 48000000
                if total_us > 0:
                    freq = int(pulse_count * 1_000_000 / total_us)
                else:
                    freq = 1000000
                freq = max(1, min(freq, 48000000))
                required_div = base_clock / freq
                prescaler = 0
                period = max(1, min(int(round(required_div)) - 1, 65535))
                pulse = period // 2
                pulse_num = 1
                self.log(f"→ 使用参数: prescaler={prescaler}, period={period}, pulse={pulse}, pulse_num={pulse_num}")
                temp_i2c.pwm_start(prescaler, period, pulse, pulse_num, log_callback=self.log)
                try: temp_i2c._ensure_idle_bus(log_callback=self.log)
                except Exception: pass
            except Exception as e:
                self.log(f"!! SPI_SCK_DIRECT PWM 失败: {e}")

            if created_temp:
                try: temp_i2c.close()
                except Exception: pass
            self.log(f"SPI_SCK_DIRECT | 发送完成: {pulse_count} 脉冲")
        except Exception as e:
            self.log(f"!! SPI_SCK_DIRECT 失败: {e}")

    # ==================== IIC 寄存器操作 ====================

    def do_fixed_write(self):
        if not self.ensure_initialized(): return
        try:
            addr = int(self.addr_var.get().strip())
            index = int(self.index_var.get().strip())
            write_data = IICLogic.parse_hex_exact4(self.write_data_var.get())
            self._do_write(addr, index, write_data)
        except Exception as e:
            messagebox.showerror("错误", str(e))

    def do_fixed_read(self):
        if not self.ensure_initialized(): return
        try:
            addr = int(self.addr_var.get().strip())
            index = int(self.index_var.get().strip())
            self._do_read(addr, index)
        except Exception as e:
            messagebox.showerror("错误", str(e))

    def do_write_then_read(self):
        if not self.ensure_initialized(): return
        try:
            addr = int(self.addr_var.get().strip())
            index = int(self.index_var.get().strip())
            write_data = IICLogic.parse_hex_exact4(self.write_data_var.get())
            self._do_write(addr, index, write_data)
            self._do_read(addr, index)
        except Exception as e:
            messagebox.showerror("错误", str(e))

    def execute_quick_row_write(self, row_index):
        if not self.ensure_initialized(): return
        if row_index < 0 or row_index >= len(self.quick_rows): return
        addr_var, index_var, b31_var, b23_var, b15_var, b7_var = self.quick_rows[row_index]
        try:
            addr = int(addr_var.get().strip())
            index = int(index_var.get().strip())
            write_data = [
                IICLogic.parse_hex_byte(b31_var.get(), "[31:24]"),
                IICLogic.parse_hex_byte(b23_var.get(), "[23:16]"),
                IICLogic.parse_hex_byte(b15_var.get(), "[15:8]"),
                IICLogic.parse_hex_byte(b7_var.get(), "[7:0]"),
            ]
            self._do_write(addr, index, write_data)
        except Exception as e:
            self.log(f"!! 快速行 写失败: {e}")

    def execute_quick_row_write_then_read(self, row_index):
        if not self.ensure_initialized(): return
        if row_index < 0 or row_index >= len(self.quick_rows): return
        i = row_index + 1
        addr_var, index_var, b31_var, b23_var, b15_var, b7_var = self.quick_rows[row_index]
        try:
            addr = int(addr_var.get().strip())
            index = int(index_var.get().strip())
            write_data = [
                IICLogic.parse_hex_byte(b31_var.get(), f"R{i} [31:24]"),
                IICLogic.parse_hex_byte(b23_var.get(), f"R{i} [23:16]"),
                IICLogic.parse_hex_byte(b15_var.get(), f"R{i} [15:8]"),
                IICLogic.parse_hex_byte(b7_var.get(), f"R{i} [7:0]"),
            ]
            self._do_write(addr, index, write_data)
            rv = self._do_read(addr, index)
            if rv is not None:
                self.log(f"快速行{i} 写后读回: {' '.join(f'{b:02X}' for b in rv)}")
        except Exception as e:
            self.log(f"!! 快速行{i} 写后读失败: {e}")

    def execute_quick_row_read(self, row_index):
        if not self.ensure_initialized(): return
        if row_index < 0 or row_index >= len(self.quick_rows): return
        addr_var, index_var, *_ = self.quick_rows[row_index]
        try:
            addr = int(addr_var.get().strip())
            index = int(index_var.get().strip())
            rv = self._do_read(addr, index)
            if rv is not None:
                self.log(f"读回: {' '.join(f'{b:02X}' for b in rv)}")
        except Exception as e:
            self.log(f"!! 快速行 读失败: {e}")

    def execute_quick_row_read_then_write(self, row_index):
        if not self.ensure_initialized(): return
        if row_index < 0 or row_index >= len(self.quick_rows): return
        addr_var, index_var, b31_var, b23_var, b15_var, b7_var = self.quick_rows[row_index]
        i = row_index + 1
        try:
            addr = int(addr_var.get().strip())
            index = int(index_var.get().strip())
            rv = self._do_read(addr, index)
            if rv is not None:
                self.log(f"快速行{i} 读回: {' '.join(f'{b:02X}' for b in rv)}")
            write_data = [
                IICLogic.parse_hex_byte(b31_var.get(), f"R{i} [31:24]"),
                IICLogic.parse_hex_byte(b23_var.get(), f"R{i} [23:16]"),
                IICLogic.parse_hex_byte(b15_var.get(), f"R{i} [15:8]"),
                IICLogic.parse_hex_byte(b7_var.get(), f"R{i} [7:0]"),
            ]
            self._do_write(addr, index, write_data)
            self.log(f"快速行{i} 读后写完成")
        except Exception as e:
            self.log(f"!! 快速行{i} 读后写失败: {e}")

    # ==================== IIC 批量操作 ====================

    def generate_template(self):
        filename = filedialog.asksaveasfilename(
            title="保存批量操作模板", defaultextension=".xlsx",
            filetypes=[("Excel 文件", "*.xlsx")],
            initialfile=f"批量操作模板_{time.strftime('%Y%m%d_%H%M%S')}.xlsx"
        )
        if not filename: return
        success, err = IICLogic.generate_template(filename)
        if success:
            messagebox.showinfo("成功", f"模板已生成：\n{filename}")
        else:
            messagebox.showerror("错误", f"生成模板失败：\n{err}")

    def read_data_and_export_template(self):
        if not self.ensure_initialized(): return
        filename = filedialog.asksaveasfilename(
            title="保存读数据导出文件", defaultextension=".xlsx",
            filetypes=[("Excel 文件", "*.xlsx")],
            initialfile=f"读数据导出_{time.strftime('%Y%m%d_%H%M%S')}.xlsx"
        )
        if not filename: return
        success, total_c, succ_c, err = IICLogic.read_data_and_export_template(
            self.export_addr_var.get(), self.export_index_range_var.get(),
            filename, self._do_read, self.log
        )
        if success:
            self.log(f"读数据并导出完成: 总计 {total_c} 条, 成功 {succ_c} 条")
            messagebox.showinfo("完成", f"导出完成:\n{filename}\n总计 {total_c} 条，成功 {succ_c} 条")
        else:
            self.log(f"!! 读数据并导出异常: {err}")
            messagebox.showerror("导出失败", f"读数据并导出失败：\n{err}")

    def import_and_execute_batch(self):
        if not self.ensure_initialized(): return
        filename = filedialog.askopenfilename(title="选择批量操作 Excel 文件", filetypes=[("Excel 文件", "*.xlsx")])
        if not filename: return
        success, total_c, succ_c, err = IICLogic.import_and_execute_batch(
            filename, self._do_write, self._do_read, self.log
        )
        if success:
            self.log(f"=== 批量写入与校验完成：共执行 {total_c} 条，校验全部通过！ ===")
            messagebox.showinfo("批量执行结束", f"共处理 {total_c} 条命令\n全部校验通过！\n详情见日志")
        else:
            if err and "解析/执行失败" in err:
                self.log(f"!! {err}\n已依要求中止后续批量读写。")
                messagebox.showwarning("行执行中断", f"{err}\n已依要求中止后续批量读写。")
            elif err and "存在校验不通过项" in err:
                self.log(f"!! {err}")
                messagebox.showerror("批量校验失败", f"部分操作校验未通过：\n\n{err}")
            else:
                self.log(f"!! 批量执行异常: {err}")
                messagebox.showerror("批量执行错误", f"执行失败：\n{err}")

    def import_and_update_reads(self):
        if not self.ensure_initialized(): return
        filename = filedialog.askopenfilename(title="选择要读回并更新的 Excel 文件", filetypes=[("Excel 文件", "*.xlsx")])
        if not filename: return

        progress_win = tk.Toplevel(self.root)
        progress_win.title("批量读回并更新中...")
        progress_win.geometry("320x120")
        progress_win.transient(self.root)
        progress_win.grab_set()
        tk.Label(progress_win, text="正在逐行读回并更新 Excel，请稍候...").pack(pady=8)
        progress_var = tk.DoubleVar(value=0)
        pb = ttk.Progressbar(progress_win, variable=progress_var)
        pb.pack(fill='x', padx=20, pady=6)
        progress_win.update()

        def progress_cb(current, total):
            pb.configure(maximum=total)
            progress_var.set(current)
            if current % 5 == 0:
                progress_win.update()

        success, total_c, succ_c, wb, err_msg = IICLogic.import_and_update_reads(
            filename, self._do_write, self._do_read, self.log, progress_cb
        )

        try: progress_win.destroy()
        except: pass

        if not success:
            messagebox.showerror("错误", f"读回并更新失败：\n{err_msg}")
            self.log(f"!! 读回并更新发生异常: {err_msg}")
            return
        if total_c == 0 and succ_c == 0:
            messagebox.showinfo("提示", err_msg)
            return

        base = os.path.splitext(os.path.basename(filename))[0]
        save_name = filedialog.asksaveasfilename(title="保存读回结果为", defaultextension=".xlsx",
            filetypes=[("Excel 文件", "*.xlsx")], initialfile=f"{base}_readback.xlsx")
        try:
            if save_name:
                wb.save(save_name)
                messagebox.showinfo("完成", f"读回并更新完成\n已保存: {save_name}\n成功 {succ_c}/{total_c} 条")
            else:
                wb.save(filename)
                messagebox.showinfo("完成", f"读回并更新完成\n已覆盖原文件: {filename}\n成功 {succ_c}/{total_c} 条")
        except Exception as e:
            messagebox.showerror("保存异常", f"保存 Excel 失败: {e}")

    def export_to_excel(self):
        if not self.log_records:
            messagebox.showinfo("提示", "当前无日志记录可导出")
            return
        filename = filedialog.asksaveasfilename(title="保存日志", defaultextension=".xlsx",
            filetypes=[("Excel 文件", "*.xlsx")],
            initialfile=f"I2C日志_{time.strftime('%Y%m%d_%H%M%S')}.xlsx")
        if not filename: return
        try:
            from openpyxl import Workbook
            from openpyxl.styles import Font
            wb = Workbook()
            ws = wb.active
            ws.title = "I2C操作日志"
            headers = ['时间', '操作类型', '地址(addr)', '索引(index)', '写入值', '读取值', '原始消息']
            ws.append(headers)
            for col in range(1, len(headers) + 1):
                ws.cell(row=1, column=col).font = Font(bold=True)
            for record in self.log_records:
                ws.append([record['time'], record['operation'], record.get('addr', ''),
                           record.get('index', ''), record.get('write_value', ''),
                           record.get('read_value', ''), record.get('raw_message', '')])
            for col in ws.columns:
                max_length = max((len(str(cell.value or '')) for cell in col), default=0)
                ws.column_dimensions[col[0].column_letter].width = min(max_length + 2, 50)
            wb.save(filename)
            messagebox.showinfo("成功", f"日志已导出至：\n{filename}")
        except Exception as e:
            messagebox.showerror("导出失败", f"错误：{str(e)}")

    # ==================== MTP 单操作（薄包装） ====================

    def do_mtp_open_en(self):
        if not self.ensure_initialized() or not self.mtp: return
        try: self.mtp.open_mtp_en()
        except Exception as e: messagebox.showerror("MTP 错误", str(e))

    def do_mtp_close_en(self):
        if not self.ensure_initialized() or not self.mtp: return
        try: self.mtp.close_mtp_en()
        except Exception as e: messagebox.showerror("MTP 错误", str(e))

    def do_mtp_read_byte(self):
        if not self.ensure_initialized() or not self.mtp: return
        try:
            addr = int(self.mtp_addr_var.get().strip())
            if not (0 <= addr <= 511):
                raise ValueError("ADDR 必须在 0~511 之间")
            self.mtp.read_byte(addr)
        except Exception as e: messagebox.showerror("MTP 错误", str(e))

    def do_mtp_program_byte(self):
        if not self.ensure_initialized() or not self.mtp: return
        try:
            addr = int(self.mtp_addr_var.get().strip())
            data_hex = self.mtp_data_var.get().strip()
            if not data_hex: raise ValueError("请输入 Byte Data(HEX)")
            byte_val = int(data_hex, 16)
            if not (0 <= addr <= 511): raise ValueError("ADDR 必须在 0~511 之间")
            if not (0 <= byte_val <= 255): raise ValueError("Byte Data 必须在 00~FF 之间")
            self.mtp.program_byte(addr, byte_val)
        except Exception as e: messagebox.showerror("MTP 错误", str(e))

    def do_mtp_sector_erase(self):
        if not self.ensure_initialized() or not self.mtp: return
        try:
            addr = int(self.mtp_addr_var.get().strip())
            if not (0 <= addr <= 511): raise ValueError("ADDR 必须在 0~511 之间")
            sector = addr >> 5
            self.mtp.erase_sector(sector)
            self.log(f"已下发 MTP Sector {sector} 擦除指令")
        except Exception as e: messagebox.showerror("MTP 错误", str(e))

    def do_mtp_chip_erase(self):
        if not self.ensure_initialized() or not self.mtp:
            return
        if not messagebox.askyesno("确认",
                "即将执行全片擦除(Chip Erase)。\n该操作不可撤销，全片数据将清零。\n\n确认继续？"):
            return
        try:
            win = tk.Toplevel(self.root)
            win.title("全片擦除中...")
            win.geometry("340x140")
            win.transient(self.root)
            win.grab_set()
            tk.Label(win, text="正在执行 Chip Erase，请等待...").pack(pady=10)
            pv = tk.DoubleVar(value=0)
            ttk.Progressbar(win, variable=pv, maximum=60).pack(fill='x', padx=20, pady=5)
            sv = tk.StringVar(value="正在写入擦除指令...")
            tk.Label(win, textvariable=sv).pack(pady=4)
            win.update()
            threading.Thread(target=self._run_chip_erase_bg,
                             args=(win, pv, sv), daemon=True).start()
        except Exception as e:
            messagebox.showerror("MTP 错误", str(e))

    def _run_chip_erase_bg(self, win, pv, sv):
        def _finish(w, ok, err_msg=None):
            try:
                w.destroy()
            except Exception:
                pass
            if ok:
                self.log("=== MTP Chip Erase 完成 ===")
                messagebox.showinfo("Chip Erase", "全片擦除完成")
            elif err_msg:
                self.log(f"!! Chip Erase 异常: {err_msg}")
                messagebox.showerror("Chip Erase 失败", f"全片擦除异常：\n{err_msg}")
            else:
                self.log("!! Chip Erase 超时（60 次轮询）")
                messagebox.showwarning("Chip Erase 超时", "全片擦除轮询超时，请检查芯片状态")

        try:
            self.mtp.erase_chip()
            for attempt in range(1, 61):
                import time
                time.sleep(0.5)
                try:
                    st = self._do_read(31, 0)
                    busy = st[3] & 0x01 if st and len(st) >= 4 else 1
                except Exception:
                    busy = 1
                self.root.after(0, lambda a=attempt, b=busy, w=win, p=pv, s=sv: (
                    p.set(a),
                    s.set(f"擦除状态 bit0={'忙' if b else '就绪'}（轮询第 {a}/60 次）"),
                    w.update() if a % 5 == 0 else None))
                if busy == 0:
                    self.root.after(0, _finish, win, True)
                    return
            self.root.after(0, _finish, win, False)
        except Exception as e:
            self.root.after(0, _finish, win, False, str(e))

    def do_recover_bus(self):
        if not self.ensure_initialized(): return
        try:
            self.i2c.recover_bus_manual(log_callback=self.log)
            self.log("BUS_RECOVER | 手动恢复完成", record={
                'time': time.strftime('%Y-%m-%d %H:%M:%S'), 'operation': 'BUS_RECOVER',
                'addr': '', 'index': '', 'write_value': '', 'read_value': '',
                'raw_message': 'manual-bus-recover'
            })
        except Exception as e: messagebox.showerror("错误", str(e))

    # ==================== MTP 批量操作（GUI 壳 + 业务层调用） ====================

    def generate_mtp_template(self):
        filename = filedialog.asksaveasfilename(title="保存MTP操作模板", defaultextension=".xlsx",
            filetypes=[("Excel 文件", "*.xlsx")],
            initialfile=f"MTP批量模板_{time.strftime('%Y%m%d_%H%M%S')}.xlsx")
        if not filename: return
        success, err = MTPBatchLogic.generate_mtp_template(filename)
        if success:
            messagebox.showinfo("成功", f"MTP 模板已生成：\n{filename}")
        else:
            messagebox.showerror("错误", f"生成模板失败：\n{err}")

    def mtp_full_program_with_crc(self):
        if not self.ensure_initialized() or not self.mtp: return
        filename = filedialog.askopenfilename(title="选择输入Excel", filetypes=[("Excel 文件", "*.xlsx")])
        if not filename: return

        success, buffer, burn_addrs, final_crc, err = MTPBatchLogic.mtp_full_program_with_crc_gui(
            filename, self.mtp, self._do_write, self._do_read,
            IICLogic.parse_hex_byte, self.log
        )
        if not success:
            messagebox.showerror("全片烧录失败", err)
            return

        crc_cnt = sum(1 for x in burn_addrs if x < CRC_ADDR_LIMIT)
        yes = messagebox.askyesno("全片烧录 CRC",
            f"算出 CRC 校验为: 0x{final_crc:08X}\n"
            f"（算法 crc.py(LSB优先)；计入地址 0~{CRC_ADDR_LIMIT - 1} 的声明字节 {crc_cnt} 个）\n\n"
            f"共烧录 {len(burn_addrs)} 字节，其中 {len(burn_addrs) - crc_cnt} 字节"
            f"（地址 >= {CRC_ADDR_LIMIT}）直接烧写、不参与 CRC。\n\n"
            f"确认你已经手动【擦除+Open MTP EN】后再点击是开始执行。")
        if not yes: return

        # 烧录进度窗口
        progress_win = tk.Toplevel(self.root)
        progress_win.title("烧录中...")
        progress_win.geometry("300x120")
        progress_win.transient(self.root)
        progress_win.grab_set()
        tk.Label(progress_win, text="正在逐字节烧录，请稍候...").pack(pady=10)
        progress_var = tk.DoubleVar(value=0)
        pb = ttk.Progressbar(progress_win, variable=progress_var)
        pb.pack(fill='x', padx=20, pady=5)
        progress_win.update()

        def burn_progress(current, total):
            pb.configure(maximum=total)
            progress_var.set(current)
            progress_win.update()

        MTPBatchLogic.execute_mtp_burn(self.mtp, buffer, burn_addrs, self.log, burn_progress)
        progress_win.destroy()

        # 校验进度窗口
        verify_win = tk.Toplevel(self.root)
        verify_win.title("读回并比对中...")
        verify_win.geometry("300x120")
        verify_win.transient(self.root)
        verify_win.grab_set()
        tk.Label(verify_win, text="正在读回并比对，请稍候...").pack(pady=10)
        verify_var = tk.DoubleVar(value=0)
        pb_v = ttk.Progressbar(verify_win, variable=verify_var)
        pb_v.pack(fill='x', padx=20, pady=5)
        verify_win.update()

        def verify_progress(current, total):
            pb_v.configure(maximum=total)
            verify_var.set(current)
            if current % 10 == 0:
                verify_win.update()

        mismatches = MTPBatchLogic.verify_mtp_burn(self.mtp, buffer, burn_addrs, self.log, verify_progress)
        try: verify_win.destroy()
        except: pass

        if mismatches:
            self.log(f"=== MTP 烧录校验发现 {len(mismatches)} 处不一致 ===")
            summary = ''.join(
                f"Addr {m[0]}: 期望 0x{m[1]:02X}, 读回 {'None' if m[2] is None else f'0x{m[2]:02X}'}\n"
                for m in mismatches[:20]
            )
            messagebox.showwarning("校验失败", f"发现 {len(mismatches)} 处不一致，详情见日志。\n\n{summary}")
        else:
            self.log("=== MTP 烧录校验全部通过 ===")
            messagebox.showinfo("校验通过", f"所有写入字节已验证通过。\n最终 CRC 为 0x{final_crc:08X}。")
        self.log("=== MTP 物理全片 CRC 烧写完成 ===")

    def import_and_execute_mtp_batch(self):
        if not self.ensure_initialized() or not self.mtp: return
        filename = filedialog.askopenfilename(title="选择MTP批量操作 Excel 文件", filetypes=[("Excel 文件", "*.xlsx")])
        if not filename: return

        progress_win = tk.Toplevel(self.root)
        progress_win.title("MTP批量执行中...")
        progress_win.geometry("300x120")
        progress_win.transient(self.root)
        progress_win.grab_set()
        tk.Label(progress_win, text="正在执行批量操作，请稍候...").pack(pady=10)
        progress_var = tk.DoubleVar(value=0)
        pb = ttk.Progressbar(progress_win, variable=progress_var)
        pb.pack(fill='x', padx=20, pady=5)
        progress_win.update()

        def progress_cb(current, total):
            pb.configure(maximum=total)
            progress_var.set(current)
            if current % 5 == 0:
                progress_win.update()

        success, total_c, succ_c, err = MTPBatchLogic.import_and_execute_mtp_batch(
            filename, self.mtp, self.log, progress_cb
        )

        try: progress_win.destroy()
        except: pass

        if success:
            self.log(f"=== MTP批量执行结束：共执行 {total_c} 条，成功 {succ_c} 条 ===")
            messagebox.showinfo("执行结束", f"共处理 {total_c} 条MTP命令，\n其中成功 {succ_c} 条。")
        else:
            self.log(f"!! MTP批量执行异常: {err}")
            messagebox.showerror("批量执行错误", f"执行失败：\n{err}")

    def write_mtp_sector_data(self):
        if not self.ensure_initialized() or not self.mtp: return
        try:
            sector = int(self.mtp_target_sector_var.get().strip())
            val_hex = self.mtp_sector_val_var.get().strip()
            byte_val = int(val_hex, 16) & 0xFF
            if not messagebox.askyesno("提示",
                f"即将向扇区 {sector} 全部写入 0x{byte_val:02X}。\n\n请确认已【擦除】且已【开启 MTP_EN】，是否继续？"):
                return
            success, err = MTPBatchLogic.write_mtp_sector_data(self.mtp, sector, byte_val, self.log)
            if success:
                messagebox.showinfo("完成", f"扇区 {sector} 统一烧写完成。")
            else:
                messagebox.showerror("执行错误", err)
        except Exception as e:
            messagebox.showerror("执行错误", str(e))

    def read_and_export_mtp_sector(self):
        if not self.ensure_initialized() or not self.mtp: return
        try:
            sector = int(self.mtp_target_sector_var.get().strip())
            if not (0 <= sector <= 15):
                raise ValueError("扇区必须在0~15之间")
        except Exception as e:
            messagebox.showerror("参数错误", str(e))
            return
        filename = filedialog.asksaveasfilename(title=f"保存扇区{sector}读取数据", defaultextension=".xlsx",
            filetypes=[("Excel 文件", "*.xlsx")], initialfile=f"MTP_Sector{sector}_读取结果.xlsx")
        if not filename: return
        success, err = MTPBatchLogic.read_and_export_mtp_sector(self.mtp, sector, filename, self.log)
        if success:
            messagebox.showinfo("导出成功", f"扇区数据已保存至:\n{filename}")
        else:
            messagebox.showerror("执行错误", err)

    def read_mtp_and_export_template(self):
        if not self.ensure_initialized() or not self.mtp: return
        filename = filedialog.asksaveasfilename(title="保存读出MTP并导出文件", defaultextension=".xlsx",
            filetypes=[("Excel 文件", "*.xlsx")],
            initialfile=f"读出MTP并导出_{time.strftime('%Y%m%d_%H%M%S')}.xlsx")
        if not filename: return

        progress_win = tk.Toplevel(self.root)
        progress_win.title("读出MTP并导出中...")
        progress_win.geometry("300x120")
        progress_win.transient(self.root)
        progress_win.grab_set()
        tk.Label(progress_win, text="正在读取MTP并导出，请稍候...").pack(pady=10)
        progress_var = tk.DoubleVar(value=0)
        pb = ttk.Progressbar(progress_win, variable=progress_var)
        pb.pack(fill='x', padx=20, pady=5)
        progress_win.update()

        def progress_cb(current, total):
            pb.configure(maximum=total)
            progress_var.set(current)
            if current % 5 == 0:
                progress_win.update()

        try:
            success, total_c, succ_c, err = MTPBatchLogic.read_mtp_and_export_template(
                self.mtp, filename, self.log, progress_cb
            )
        finally:
            try: progress_win.destroy()
            except: pass

        if success:
            self.log(f"MTP读出并导出成功: 总计 {total_c} 条, 成功 {succ_c} 条")
            messagebox.showinfo("完成", f"导出完成:\n{filename}\n总计 {total_c} 条，成功 {succ_c} 条")
        else:
            messagebox.showerror("导出失败", f"读MTP并导出失败：\n{err}")


if __name__ == "__main__":
    try:
        root = tk.Tk()
        app = I2CGUI(root)
        root.mainloop()
    except Exception:
        err_text = traceback.format_exc()
        try:
            base_dir = os.path.dirname(sys.executable) if getattr(sys, 'frozen', False) else os.path.dirname(os.path.abspath(__file__))
            log_path = os.path.join(base_dir, "iictool_startup_error.log")
            with open(log_path, "a", encoding="utf-8") as f:
                f.write("\n" + "=" * 60 + "\n")
                f.write(time.strftime("%Y-%m-%d %H:%M:%S") + "\n")
                f.write(err_text)
            messagebox.showerror("程序启动失败", f"程序启动异常，详情见:\n{log_path}")
        except Exception:
            pass
        raise
