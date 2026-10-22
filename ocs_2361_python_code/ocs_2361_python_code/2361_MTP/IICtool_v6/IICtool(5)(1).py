import time
import struct
# -*- coding: utf-8 -*-
import tkinter as tk
from tkinter import ttk, scrolledtext, messagebox, filedialog
import serial
import json
import os
import sys
import ctypes
import threading
import queue
import traceback
from openpyxl import Workbook, load_workbook
from openpyxl.styles import Font
import re

CONFIG_FILE = "i2c_config.json"
DEFAULT_CONFIG = {
    "port": "COM13",
    "slave_addr": "0x52"
}

# ======================== 配置加载/保存 ========================
def load_config():
    if os.path.exists(CONFIG_FILE):
        try:
            with open(CONFIG_FILE, 'r', encoding='utf-8') as f:
                return json.load(f)
        except:
            pass
    return DEFAULT_CONFIG.copy()

def save_config(port, slave_addr):
    config = {
        "port": str(port).strip(),
        "slave_addr": str(slave_addr).strip()
    }
    try:
        with open(CONFIG_FILE, 'w', encoding='utf-8') as f:
            json.dump(config, f, indent=4)
    except Exception as e:
        print(f"保存配置失败: {e}")

# ======================== I2C 控制器 ========================
class I2CController:
    LINE_FUN1_IIC_AddStart = 10
    LINE_FUN1_IIC_NoStart = 11
    LINE_FUN2_IIC_AddStop = 5
    LINE_FUN2_IIC_NoStop = 6

    def __init__(self, port, slave_addr, baudrate=921600):
        self.port = port
        self.slave_addr = slave_addr
        self.baudrate = baudrate
        self.ser = None
        self.initialized = False
        self.dll = None
        self.dll_ready = False
        self.usb_index = 0
        self.read_timeout_ms = 20
        self.read_frame_gap_s = 0.0
        # 关闭读后恢复时钟，避免在STOP后追加额外时钟导致时序变长
        self.post_read_recovery_clocks = 0
        # A5读后延迟STOP：默认连续接收到7字节再STOP（可改为6）
        self.stop_after_total_read_bytes = 1

    def open(self):
        if self.ser and self.ser.is_open:
            return True
        try:
            self.ser = serial.Serial(self.port, self.baudrate, timeout=1)
            return True
        except Exception as e:
            raise Exception(f"打开串口失败: {e}")

    def close(self):
        if self.ser and self.ser.is_open:
            self.ser.close()
        if self.dll and self.dll_ready:
            try:
                self.dll.CloseUsb(self.usb_index)
            except Exception:
                pass
        self.dll_ready = False
        self.initialized = False

    def _find_dll_path(self):
        if getattr(sys, 'frozen', False):
            base_dir = os.path.dirname(sys.executable)
        else:
            base_dir = os.path.dirname(os.path.abspath(__file__))

        candidates = [
            os.path.join(base_dir, "USB2UARTSPIIICDLL.dll"),
            os.path.join(base_dir, "_internal", "USB2UARTSPIIICDLL.dll"),
            # 保底兼容旧的运行路径习惯
            os.path.join(os.getcwd(), "USB2UARTSPIIICDLL.dll"),
            os.path.join(os.getcwd(), "_internal", "USB2UARTSPIIICDLL.dll")
        ]
        
        for path in candidates:
            if os.path.exists(path):
                return path
        return None

    def _ensure_dll_ready(self, log_callback=None):
        if self.dll_ready:
            return

        dll_path = self._find_dll_path()
        if not dll_path:
            raise Exception("未找到 USB2UARTSPIIICDLL.dll，请确认 DLL 路径")

        # Python 3.8+ 显式加入 DLL 目录，避免依赖库解析失败。
        dll_dir = os.path.dirname(dll_path)
        if hasattr(os, 'add_dll_directory'):
            try:
                os.add_dll_directory(dll_dir)
            except Exception:
                pass

        self.dll = ctypes.WinDLL(dll_path)

        self.dll.OpenUsb.argtypes = [ctypes.c_uint]
        self.dll.OpenUsb.restype = ctypes.c_int
        self.dll.CloseUsb.argtypes = [ctypes.c_uint]
        self.dll.CloseUsb.restype = ctypes.c_int
        self.dll.ConfigIICParam.argtypes = [ctypes.c_uint, ctypes.c_uint, ctypes.c_uint]
        self.dll.ConfigIICParam.restype = ctypes.c_int
        self.dll.IICSendData.argtypes = [ctypes.c_ubyte, ctypes.c_ubyte, ctypes.POINTER(ctypes.c_ubyte), ctypes.c_uint, ctypes.c_uint]
        self.dll.IICSendData.restype = ctypes.c_int
        self.dll.IICRcvData.argtypes = [ctypes.c_ubyte, ctypes.POINTER(ctypes.c_ubyte), ctypes.c_uint, ctypes.c_uint]
        self.dll.IICRcvData.restype = ctypes.c_int
        self.dll.IICSendAndRcvData.argtypes = [ctypes.c_ubyte, ctypes.c_ubyte, ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(ctypes.c_ubyte), ctypes.c_uint, ctypes.c_uint, ctypes.c_uint]
        self.dll.IICSendAndRcvData.restype = ctypes.c_int

        self.dll.InternalListInitAllLine.argtypes = [ctypes.c_uint]
        self.dll.InternalListInitAllLine.restype = ctypes.c_int
        self.dll.InternalListSetLine.argtypes = [
            ctypes.c_uint, ctypes.c_uint, ctypes.c_uint, ctypes.c_uint,
            ctypes.POINTER(ctypes.c_ubyte), ctypes.c_uint,
            ctypes.POINTER(ctypes.c_ubyte), ctypes.c_uint,
            ctypes.c_uint, ctypes.c_uint, ctypes.c_uint, ctypes.c_uint
        ]
        self.dll.InternalListSetLine.restype = ctypes.c_int
        self.dll.InternalListDisableLine.argtypes = [ctypes.c_uint, ctypes.c_uint]
        self.dll.InternalListDisableLine.restype = ctypes.c_int
        self.dll.InternalListTsetOnce.argtypes = [ctypes.POINTER(ctypes.c_uint), ctypes.c_uint]
        self.dll.InternalListTsetOnce.restype = ctypes.c_int
        self.dll.InternalListReceive.argtypes = [ctypes.POINTER(ctypes.c_ubyte), ctypes.c_uint, ctypes.c_uint]
        self.dll.InternalListReceive.restype = ctypes.c_int
        self.dll.InternalListStop.argtypes = [ctypes.c_uint]
        self.dll.InternalListStop.restype = ctypes.c_int

        ret = self.dll.OpenUsb(self.usb_index)
        if ret < 0:
            raise Exception(f"OpenUsb 失败，返回值: {ret}")

        # rate=6 对应 100K（按官方示例），clkSLevel=1000
        ret = self.dll.ConfigIICParam(8, 4000, self.usb_index)
        if ret < 0:
            raise Exception(f"ConfigIICParam 失败，返回值: {ret}")

        self.dll_ready = True
        if log_callback:
            log_callback(f"DLL已就绪: {dll_path}")

    def _dll_i2c_send(self, start_bit, stop_bit, payload, log_callback=None):
        send_buf = (ctypes.c_ubyte * len(payload))(*payload)
        ret = self.dll.IICSendData(int(start_bit), int(stop_bit), send_buf, len(payload), self.usb_index)
        if ret < 0:
            raise Exception(f"IICSendData 失败，返回值: {ret}")
        if log_callback:
            log_callback(f"→ DLL IICSendData(start={start_bit}, stop={stop_bit}): {' '.join(f'{b:02X}' for b in payload)}")

    def _dll_i2c_send_and_recv(self, start_bit, stop_bit, payload, recv_len, log_callback=None):
        send_buf = (ctypes.c_ubyte * len(payload))(*payload)
        recv_buf = (ctypes.c_ubyte * recv_len)()
        ret = self.dll.IICSendAndRcvData(
            int(start_bit), int(stop_bit), send_buf, recv_buf,
            len(payload), recv_len, self.usb_index
        )
        if ret < 0:
            raise Exception(f"IICSendAndRcvData 失败，返回值: {ret}")
        result = bytes(recv_buf)
        if log_callback:
            log_callback(
                f"→ DLL IICSendAndRcvData(start={start_bit}, stop={stop_bit}, send={' '.join(f'{b:02X}' for b in payload)}, recv={recv_len})"
            )
        return result

    def _dll_i2c_stop_only(self, log_callback=None):
        """仅发送STOP，不追加任何数据位，避免引入额外时钟。"""
        send_buf = (ctypes.c_ubyte * 0)()
        ret = self.dll.IICSendData(0, 1, send_buf, 0, self.usb_index)
        if ret < 0:
            raise Exception(f"IICSendData(STOP-only) 失败，返回值: {ret}")
        if log_callback:
            log_callback("→ DLL IICSendData(start=0, stop=1): STOP-only")

    def _ensure_idle_bus(self, log_callback=None):
        """确保总线空闲并将 SCL 拉高。"""
        if not self.dll_ready:
            return
        try:
            self._dll_i2c_stop_only(log_callback=None)
        except Exception:
            # 如果直接 STOP 无法完成，先发一个额外时钟再重试一次。
            try:
                self._dll_i2c_send(0, 1, [0xFF], log_callback=None)
            except Exception:
                pass
            try:
                self._dll_i2c_stop_only(log_callback=None)
            except Exception as e:
                if log_callback:
                    log_callback(f"!! 确保总线空闲失败: {e}")
                raise

    def _call_with_timeout(self, func, timeout_ms):
        q = queue.Queue(maxsize=1)

        def _worker():
            try:
                q.put((True, func()))
            except Exception as e:
                q.put((False, e))

        t = threading.Thread(target=_worker, daemon=True)
        t.start()
        t.join(timeout_ms / 1000.0)
        if t.is_alive():
            raise TimeoutError(f"IIC读取超时，超过{timeout_ms}ms")

        ok, payload = q.get_nowait()
        if ok:
            return payload
        raise payload

    def _recover_bus_default(self, log_callback=None):
        """超时后尝试恢复IIC空闲状态，释放SDA/SCL到默认状态。"""
        if log_callback:
            log_callback("!! 读超时，开始总线恢复（释放SDA/SCL）")

        # 1) 尝试多次补STOP，先快速结束当前传输状态
        for _ in range(3):
            try:
                if self.dll_ready:
                    self._dll_i2c_send(0, 1, [0x00], log_callback=None)
            except Exception:
                pass

        # 2) 发送0xFF并带STOP，给出额外时钟脉冲，尽量让从机释放SDA
        try:
            if self.dll_ready:
                self._dll_i2c_send(0, 1, [0xFF], log_callback=None)
        except Exception:
            pass

        # 3) 关闭并重新打开USB，再配置IIC参数
        try:
            if self.dll:
                self.dll.CloseUsb(self.usb_index)
        except Exception:
            pass

        self.dll_ready = False

        try:
            if self.dll:
                ret = self.dll.OpenUsb(self.usb_index)
                if ret < 0:
                    raise Exception(f"OpenUsb恢复失败: {ret}")
                ret = self.dll.ConfigIICParam(8, 4000, self.usb_index)
                if ret < 0:
                    raise Exception(f"ConfigIICParam恢复失败: {ret}")
                self.dll_ready = True
                if log_callback:
                    log_callback("!! 总线恢复完成：SDA/SCL已回默认空闲")
        except Exception as e:
            if log_callback:
                log_callback(f"!! 总线恢复失败: {e}")

    def _send_recovery_clocks(self, clock_count=9, log_callback=None):
        """发送恢复时钟脉冲，默认9个；支持更多脉冲并在末尾统一STOP。"""
        if not self.dll_ready:
            return
        if clock_count <= 0:
            return

        # I2C 1字节传输对应9个时钟（8数据位+1ACK/NACK位）
        byte_count = (int(clock_count) + 8) // 9
        try:
            # 通过连续发送0xFF产生时钟串；仅最后一次发送STOP，减少额外起始条件
            for i in range(byte_count):
                start_bit = 1 if i == 0 else 0
                stop_bit = 1 if i == (byte_count - 1) else 0
                self._dll_i2c_send(start_bit, stop_bit, [0xFF], log_callback=None)
            if log_callback:
                log_callback(f"→ 已追加恢复时钟脉冲: 约{byte_count * 9}个（目标{clock_count}个）")
        except Exception as e:
            if log_callback:
                log_callback(f"!! 追加恢复时钟失败: {e}")

    def recover_bus_manual(self, log_callback=None):
        """手动触发总线恢复：主动释放SDA/SCL并重置IIC控制器状态。"""
        if not self.initialized:
            raise Exception("请先初始化I2C")
        if not self.dll_ready:
            self._ensure_dll_ready(log_callback=log_callback)
        if log_callback:
            log_callback("→ 手动触发总线恢复（主动拉高/释放SDA、SCL）")
        self._recover_bus_default(log_callback=log_callback)

    def _dll_i2c_recv(self, stop_bit, recv_len, log_callback=None):
        def _recv_call():
            recv_buf = (ctypes.c_ubyte * recv_len)()
            ret = self.dll.IICRcvData(int(stop_bit), recv_buf, recv_len, self.usb_index)
            if ret < 0:
                raise Exception(f"IICRcvData 失败，返回值: {ret}")
            return bytes(recv_buf)

        try:
            result = self._call_with_timeout(_recv_call, self.read_timeout_ms)
        except TimeoutError:
            self._recover_bus_default(log_callback=log_callback)
            raise

        if log_callback:
            log_callback(f"← DLL IICRcvData(stop={stop_bit}): {' '.join(f'{b:02X}' for b in result)}")
        return result


    def _dll_i2c_two_start_read_ack_stop_immediate(self, tx1, tx2, recv_len, log_callback=None):
        """连续执行：START+tx1(不STOP) -> START+tx2(不STOP) -> 读recv_len并ACK最后字节 -> 立即STOP-only。"""
        tx1 = [int(b) & 0xFF for b in tx1]
        tx2 = [int(b) & 0xFF for b in tx2]
        recv_len = int(recv_len)

        if recv_len < 0:
            raise Exception("recv_len 不能小于0")
        if not tx1:
            raise Exception("tx1 不能为空")
        if not tx2:
            raise Exception("tx2 不能为空")

        def _call():
            send1 = (ctypes.c_ubyte * len(tx1))(*tx1)
            send2 = (ctypes.c_ubyte * len(tx2))(*tx2)
            recv_buf = (ctypes.c_ubyte * recv_len)()
            
            # 还是保持发 STOP (1, 1)，并且两者之间去除了构造 ctypes 的 Python 层面多余操作
            ret1 = self.dll.IICSendData(1, 1, send1, len(tx1), self.usb_index) 
            ret2 = self.dll.IICSendAndRcvData(1, 1, send2, recv_buf, len(tx2), recv_len, self.usb_index)
            if ret1 < 0:
                raise Exception(f"IICSendData(phase1) 失败，返回值: {ret1}")

            if ret2 < 0:
                raise Exception(f"IICSendAndRcvData 失败，返回值: {ret2}")
            
            return bytes(recv_buf)
        try:
            result = self._call_with_timeout(_call, self.read_timeout_ms)
        except TimeoutError:
            self._recover_bus_default(log_callback=log_callback)
            raise

        if log_callback:
            log_callback(
                f"← 连续ACK+STOP完成: phase1={' '.join(f'{b:02X}' for b in tx1)} | "
                f"phase2={' '.join(f'{b:02X}' for b in tx2)} -> 读{recv_len}字节(ACK)后立即STOP: "
                f"{' '.join(f'{b:02X}' for b in result)}"
            )
        return result



    def init_i2c(self, log_callback=None):
        """发送I2C初始化命令 (0x03)"""
        if not self.ser or not self.ser.is_open:
            self.open()
        init_cmd = bytearray([0x03, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00])
        if log_callback:
            log_callback(f"→ 发送 I2C 初始化命令: {init_cmd.hex().upper()}")
        self.ser.write(init_cmd)
        time.sleep(0.001)
        resp = self.ser.read(6)
        if log_callback:
            log_callback(f"← 初始化响应: {resp.hex().upper() if resp else '无'}")
        self.initialized = True
        return resp






    def send_custom_write_frame(self, tx_bytes, log_callback=None):
        """IIC全8位直发：START后按输入字节原样发送并STOP（例如25...）"""
        if not self.initialized:
            raise Exception("请先初始化I2C")
        if not tx_bytes:
            raise Exception("写帧不能为空")

        self._ensure_dll_ready(log_callback=log_callback)
        frame = [int(b) & 0xFF for b in tx_bytes]
        if log_callback:
            log_callback(f"→ 全8位直发(不处理R/W位): {' '.join(f'{b:02X}' for b in frame)}")
        self._dll_i2c_send(1, 1, frame, log_callback=log_callback)

    def read_stop_then_start(self, tx_before_stop, tx_after_start, rx_len, log_callback=None):
        """读时序：先发送不STOP，再START发送，再读取并按配置收尾。"""
        if not self.initialized:
            raise Exception("请先初始化I2C")
        if not tx_before_stop:
            raise Exception("读阶段1发送字节不能为空")
        if rx_len < 0:
            raise Exception("读取字节数不能小于0")

        self._ensure_dll_ready(log_callback=log_callback)

        def read_one(enable_log):
            cb = log_callback if enable_log else None
            tx1 = [int(b) & 0xFF for b in tx_before_stop]
            tx2 = [int(b) & 0xFF for b in tx_after_start] if tx_after_start else []

            if rx_len == 0:
                if cb:
                    cb(f"→ 读阶段1(无读取): START发送(不STOP): {' '.join(f'{b:02X}' for b in tx1)}")
                self._dll_i2c_send(1, 0, tx1, log_callback=cb)
                return b""

            if not tx2:
                if cb:
                    cb("→ 读阶段2: 未提供A5读地址帧，无法继续读取")
                return None

            # 关键时序段：同一底层调用链内完成两次START+读4字节(ACK)+立即STOP，避免中间延时。
            raw = self._dll_i2c_two_start_read_ack_stop_immediate(tx1, tx2, rx_len, log_callback=None)
            if len(raw) < rx_len:
                return None

            # 按配置决定是否追加恢复时钟；默认关闭以避免拉长时序。
            if self.post_read_recovery_clocks > 0:
                self._send_recovery_clocks(self.post_read_recovery_clocks, log_callback=cb)

            if cb:
                cb(
                    f"→ 连续时序完成: phase1={' '.join(f'{b:02X}' for b in tx1)} | "
                    f"phase2={' '.join(f'{b:02X}' for b in tx2)} -> 读{rx_len}字节后STOP"
                )
                cb(f"  读取数据: {' '.join(f'{b:02X}' for b in raw)}")
            return raw

        try:
            return read_one(enable_log=True)
        except Exception:
            # 仅异常时做重恢复（补STOP+重开USB），避免正常读每次都重置控制器
            self._recover_bus_default(log_callback=log_callback)
            raise

# ======================== GUI 界面 ========================
class MTPController:
    """
    2361 芯片 MTP (多次可编程存储器) 测试模式操作逻辑类。
    负责所有与 MTP 相关的底层地址映射、指令封装和时序控制，并与具体通信接口解耦。
    """
    def __init__(self, write_func, read_func, log_callback=None):
        """
        初始化 MTP 控制器。
        
        参数:
            write_func (callable): 关联底层 IIC 写操作的函数，需满足签名 `write_func(addr: int, index: int, data: list[int])`。
                                   用于把 4 字节载荷发给 I2C。
            read_func (callable): 关联底层 IIC 读操作的函数，需满足签名 `read_func(addr: int, index: int) -> list[int]`。
                                  用于从 i2c 读回 4 字节数据。
            log_callback (callable, optional): 用于输出操作日志的方法。如果有此方法控制台可以看到执行状态。
        """
        self.write_i2c = write_func
        self.read_i2c = read_func
        self.log = log_callback if log_callback else (lambda msg: None)

    def _addr_index_from_mtp(self, mtp_byte_addr):
        """
        [内部方法] 将 2361 芯片内的 9 位/11 位 Byte 线性地址，折算成代码 IIC 使用的 5 位 addr 与 6 位 index。
        
        参数:
            mtp_byte_addr (int): MTP 内部存储字节地址 (如：4, 32, ... 511)
            
        返回:
            tuple (addr, index): 转换后的 IIC 控制位，addr 和 index 结合后相当于向 11 位目标地址发指令。
        """
        # MTP地址向右移 6 位，取出高 5 位作为 addr
        addr = (mtp_byte_addr >> 6) & 0x1F
        # 取 MTP 地址的低 6 位作为 index
        index = mtp_byte_addr & 0x3F
        return addr, index

    def _convert_32bit_to_bytes(self, val_32):
        """
        [内部方法] 芯片要求的指令（如控制位/等待发送的数据）大都是 32 位的，此函数将其转为 4 字节列表发送。
        
        参数:
            val_32 (int): 待发送的 32 位整型数据，例如指令 0x100、0x800 等。
            
        返回:
            list[int]: 包含 4 Byte 的列表，以大端方式排列（高字节在前）。
        """
        return [(val_32 >> 24) & 0xFF, (val_32 >> 16) & 0xFF, (val_32 >> 8) & 0xFF, val_32 & 0xFF]

    def open_mtp_en(self):
        """
        开启 MTP 使能 (Open MTP EN) —— 操作前提权限。
        在读/写/擦除 前必须调用此方法。对应写入地址 `11110_000000` (即 addr=30, index=0) 的 `apb_wdat[8]`。
        """
        self.log("MTP: 开启 MTP 使能 (open mtp en)")
        addr, index = self._addr_index_from_mtp(0)
        # 固定操作地址: 30(11110), 0(000000), 写入数据 32'h100 (给 apb_wdat[8] 置 1)
        self.write_i2c(30, 0, self._convert_32bit_to_bytes(0x100))

    def close_mtp_en(self):
        """
        关闭 MTP 使能 (Close MTP EN) —— 操作结束收尾。
        完成对 MTP 的全部读/写/擦动作后，必须关闭权限，以防异常。
        """
        self.log("MTP: 关闭 MTP 使能 (close mtp en)")
        # 向相同操作地址写入全 0，解除 apb_wdat[8] 的使能信号
        self.write_i2c(30, 0, self._convert_32bit_to_bytes(0x0))

    def read_byte(self, mtp_byte_addr):
        """
        读出指定地址的单字节数据。
        
        参数:
            mtp_byte_addr (int): MTP 目标首地址，例如第 5 Byte。
            
        返回:
            list[int]: 返回由读取得到的底层 4 个字节载荷表示的数据（从机 rdat 数据）。
        """
        self.log(f"MTP: 读取地址 {mtp_byte_addr}")
        addr, index = self._addr_index_from_mtp(mtp_byte_addr)
        
        # 步骤 1: 根据文档向该目标地址写入读指令 32'h200 (给 apb_wdat[9] 置 1 产生读取触发脉冲, rd_trig)
        self.write_i2c(addr, index, self._convert_32bit_to_bytes(0x200))
        
        # 步骤 2: 发出读脉冲后硬件已准备好数据，执行标准 I2C `读` 命令将 rdat 返回
        rdat = self.read_i2c(addr, index)
        return rdat

    def erase_chip(self):
        """
        全片擦除。烧录任何内容前如不需要保留旧数据，需将整个 MTP 全部擦除。
        """
        self.log("MTP: 执行全片擦除...")
        # 写入全片擦除命令：这里写入地址为 16 (10000_000000)，载荷为 0x1000 (使 apb_wdat[12] 为 1)
        self.write_i2c(16, 0, self._convert_32bit_to_bytes(0x1000))
        self.log("MTP: 强制等待 300 ms...")
        # 按照芯片要求强制执行硬件等待。系统需要 300 毫秒擦除介质
        time.sleep(0.3)

    def erase_sector(self, sector):
        """
        按扇区(Sector)擦除。MTP 共有 16 个扇区（编号 0-15），每个 32 字节。
        
        参数:
            sector (int): 扇区编号，有效范围是 [0, 15]。
        """
        if not (0 <= sector <= 15):
             raise ValueError("Sector 必须在 0~15 之间")
        self.log(f"MTP: 擦除扇区 {sector}...")
        
        # 扇区偏移乘 32(即左移 5位) 即是它的基础字节偏移地址
        mtp_base_addr = sector << 5
        addr, index = self._addr_index_from_mtp(mtp_base_addr)
        
        # Data: 0x800 (1000_0000_0000)，把 apb_wdat[11] 置 1 ，发出单扇区擦除脉冲。
        self.write_i2c(addr, index, self._convert_32bit_to_bytes(0x800))
        # 提供硬件擦除所需时间 (一般100毫秒比较保险)
        time.sleep(0.1)

    def program_byte(self, mtp_byte_addr, byte_val):
        """
        往 MTP 的指定字节地址写入（烧录）数据。
        **先决条件**：烧录前请务必确认目标扇区或整个芯片已执行过擦除操作！
        
        参数:
            mtp_byte_addr (int): 目标存储地址 (0-511)。
            byte_val (int): 需要写入该地址的一个 8-bit(Byte) 的实际数据参数。
        """
        self.log(f"MTP: 烧录地址 {mtp_byte_addr} 数据 0x{byte_val:02X}")
        addr, index = self._addr_index_from_mtp(mtp_byte_addr)
        
        # Data: 烧录使能是给 `apb_wdat[10]` 写 1(也就是 0x400), 并且要把烧入数据放到低 8 位 `apb_wdat[7:0]` 中。
        wdata = 0x400 | (byte_val & 0xFF)
        self.write_i2c(addr, index, self._convert_32bit_to_bytes(wdata))
        # 提供写入单个字节需要的微秒级延时，确保存入硬件晶体管
        time.sleep(0.01)

    def calculate_crc32(self, data_bytes):
        """
        辅助函数：计算 CRC-32 校验。
        用于对读取出的指定 MTP 段（例如 4 - 315 位）作完整性比对计算。
        
        参数:
            data_bytes (list[int]/bytes): 被验证位串。
            
        返回:
            int: 32 位整型、正向、无输出反转的完整 CRC32 校验值。
        """
        import binascii
        # binascii.crc32 返回的就是正向且标准的无翻转 CRC，& 0xFFFFFFFF 用于将 Python 带符计算变无符号。
        crc = binascii.crc32(bytes(data_bytes)) & 0xFFFFFFFF
        return crc









class I2CGUI:
    def __init__(self, root):
        self.root = root
        root.title("2361IIC工具")
        root.geometry("700x600")
        root.resizable(True, True)

        self.config = load_config()
        self.i2c = None
        self.mtp = None
        self.log_records = []
        self.create_widgets()

        # 启动时主动拉起窗口，避免被系统放到后台导致看起来“没打开”。
        try:
            self.root.update_idletasks()
            self.root.deiconify()
            self.root.lift()
            self.root.focus_force()
            self.root.attributes('-topmost', True)
            self.root.after(200, lambda: self.root.attributes('-topmost', False))
        except Exception:
            pass

    def create_widgets(self):
        # 串口设置区域
        frame_top = ttk.LabelFrame(self.root, text="串口设置", padding=5)
        frame_top.pack(fill='x', padx=10, pady=5)

        ttk.Label(frame_top, text="串口号:").grid(row=0, column=0, sticky='w', padx=5)
        self.port_var = tk.StringVar(value=self.config["port"])
        self.port_entry = ttk.Entry(frame_top, textvariable=self.port_var, width=15)
        self.port_entry.grid(row=0, column=1, sticky='w', padx=5)

        self.init_btn = ttk.Button(frame_top, text="初始化I2C", command=self.initialize_i2c)
        self.init_btn.grid(row=0, column=2, padx=10)

        self.close_btn = ttk.Button(frame_top, text="关闭串口", command=self.close_serial, state='disabled')
        self.close_btn.grid(row=0, column=3, padx=5)

        self.status_var = tk.StringVar(value="状态: 未初始化")
        ttk.Label(frame_top, textvariable=self.status_var, foreground='blue').grid(row=1, column=0, columnspan=4, sticky='w', pady=5)

        # 添加 Notebook
        self.notebook = ttk.Notebook(self.root)
        self.notebook.pack(fill='x', padx=10, pady=5)

        # 操作区域 (Tab 1)
        frame_ops = ttk.Frame(self.notebook, padding=5)
        self.notebook.add(frame_ops, text="原始IIC操作")

        ttk.Label(frame_ops, text="ADDR (0~31):").grid(row=0, column=0, sticky='e', padx=2)
        self.addr_var = tk.StringVar(value="19")
        ttk.Entry(frame_ops, textvariable=self.addr_var, width=5).grid(row=0, column=1, sticky='w', padx=2)

        ttk.Label(frame_ops, text="INDEX (0~63):").grid(row=0, column=2, sticky='e', padx=2)
        self.index_var = tk.StringVar(value="0")
        ttk.Entry(frame_ops, textvariable=self.index_var, width=5).grid(row=0, column=3, sticky='w', padx=2)

        ttk.Label(frame_ops, text="写入数据(HEX):").grid(row=0, column=4, sticky='e', padx=2)
        self.write_data_var = tk.StringVar(value="00 00 00 00")
        ttk.Entry(frame_ops, textvariable=self.write_data_var, width=15).grid(row=0, column=5, sticky='w', padx=2)

        ttk.Label(frame_ops, text="导出地址(0-31):").grid(row=1, column=0, sticky='e', padx=2)
        self.export_addr_var = tk.StringVar(value="19")
        ttk.Entry(frame_ops, textvariable=self.export_addr_var, width=5).grid(row=1, column=1, sticky='w', padx=2)

        ttk.Label(frame_ops, text="导出索引范围(0-63):").grid(row=1, column=2, sticky='e', padx=2)
        self.export_index_range_var = tk.StringVar(value="0-0")
        ttk.Entry(frame_ops, textvariable=self.export_index_range_var, width=5).grid(row=1, column=3, sticky='w', padx=2)

        btn_frame = ttk.Frame(frame_ops)
        btn_frame.grid(row=3, column=0, columnspan=6, pady=5)
        ttk.Button(btn_frame, text="单独写", command=self.do_fixed_write, width=10).pack(side='left', padx=5)
        ttk.Button(btn_frame, text="单独读", command=self.do_fixed_read, width=10).pack(side='left', padx=5)
        ttk.Button(btn_frame, text="写后读", command=self.do_write_then_read, width=10).pack(side='left', padx=5)
        ttk.Button(btn_frame, text="生成模板", command=self.generate_template, width=10).pack(side='left', padx=5)
        ttk.Button(btn_frame, text="批量执行", command=self.import_and_execute_batch, width=10).pack(side='left', padx=5)
        ttk.Button(btn_frame, text="读数据并导出", command=self.read_data_and_export_template, width=12).pack(side='left', padx=5)

        # MTP操作区域 (Tab 2)
        self.mtp_ops_frame = ttk.Frame(self.notebook, padding=5)
        self.notebook.add(self.mtp_ops_frame, text="MTP测试模式")

        ttk.Label(self.mtp_ops_frame, text="ADDR(MTP Byte, 0~511):").grid(row=0, column=0, sticky='e', padx=2)
        self.mtp_addr_var = tk.StringVar(value="0")
        ttk.Entry(self.mtp_ops_frame, textvariable=self.mtp_addr_var, width=8).grid(row=0, column=1, sticky='w', padx=2)

        ttk.Label(self.mtp_ops_frame, text="Byte Data(HEX):").grid(row=0, column=2, sticky='e', padx=2)
        self.mtp_data_var = tk.StringVar(value="00")
        ttk.Entry(self.mtp_ops_frame, textvariable=self.mtp_data_var, width=8).grid(row=0, column=3, sticky='w', padx=2)

        ttk.Label(self.mtp_ops_frame, text="Sector(0~15):").grid(row=0, column=4, sticky='e', padx=2)
        self.mtp_sector_var = tk.StringVar(value="0")
        ttk.Entry(self.mtp_ops_frame, textvariable=self.mtp_sector_var, width=6).grid(row=0, column=5, sticky='w', padx=2)

        mtp_btn_frame = ttk.Frame(self.mtp_ops_frame)
        mtp_btn_frame.grid(row=1, column=0, columnspan=6, pady=10)

        ttk.Button(mtp_btn_frame, text="Open MTP EN", command=self.do_mtp_open_en, width=12).pack(side='left', padx=5)
        ttk.Button(mtp_btn_frame, text="Close MTP EN", command=self.do_mtp_close_en, width=12).pack(side='left', padx=5)
        ttk.Button(mtp_btn_frame, text="Read Byte", command=self.do_mtp_read_byte, width=10).pack(side='left', padx=5)
        ttk.Button(mtp_btn_frame, text="Program Byte", command=self.do_mtp_program_byte, width=12).pack(side='left', padx=5)
        ttk.Button(mtp_btn_frame, text="Sector Erase", command=self.do_mtp_sector_erase, width=12).pack(side='left', padx=5)
        ttk.Button(mtp_btn_frame, text="Chip Erase", command=self.do_mtp_chip_erase, width=12).pack(side='left', padx=5)

        # 日志区域
        frame_log = ttk.LabelFrame(self.root, text="操作日志", padding=5)
        frame_log.pack(fill='both', expand=True, padx=10, pady=5)

        self.log_text = scrolledtext.ScrolledText(frame_log, width=100, height=20, state='disabled')
        self.log_text.pack(fill='both', expand=True)

        # 底部按钮
        frame_bottom = ttk.Frame(self.root)
        frame_bottom.pack(fill='x', padx=10, pady=5)
        ttk.Button(frame_bottom, text="导出日志", command=self.export_to_excel).pack(side='left', padx=5)
        ttk.Button(frame_bottom, text="主动拉高/恢复死锁", command=self.do_recover_bus).pack(side='left', padx=5)

    def log(self, msg, record=None):
        timestamp = time.strftime('%Y-%m-%d %H:%M:%S')
        full_msg = f"[{timestamp}] {msg}"
        self.log_text.configure(state='normal')
        self.log_text.insert(tk.END, full_msg + "\n")
        self.log_text.configure(state='disabled')
        self.log_text.yview(tk.END)

        if record is None:
            self.log_records.append({
                'time': timestamp,
                'operation': '',
                'addr': '',
                'index': '',
                'write_value': '',
                'read_value': '',
                'raw_message': msg
            })
        else:
            self.log_records.append(record)

    def update_status(self, text, is_error=False):
        self.status_var.set(f"状态: {text}")
        if is_error:
            self.log(f"!! {text}")

    def get_inputs(self):
        try:
            port = self.port_var.get().strip()
            if not port:
                raise ValueError("串口号不能为空")
            slave_addr_str = str(self.config.get("slave_addr", "0x52")).strip()
            slave_addr = int(slave_addr_str, 16) if slave_addr_str else 0x52
            # 为兼容旧流程，保留返回5元组；原始IIC模式下后3项不使用
            return port, slave_addr, 0, 0, 0
        except ValueError as e:
            messagebox.showerror("输入错误", f"参数格式不正确:\n{e}")
            raise

    def initialize_i2c(self):
        try:
            port, slave_addr, _, _, _ = self.get_inputs()
            if self.i2c:
                self.i2c.close()
            self.i2c = I2CController(port, slave_addr)
            self.i2c.open()
            self.i2c.init_i2c(log_callback=self.log)
            
            self.mtp = MTPController(self._execute_write, self._execute_read, log_callback=self.log)
            
            save_config(port, f"0x{slave_addr:02X}")
            self.update_status("已初始化 (原始IIC模式)")
            self.init_btn.config(state='disabled')
            self.close_btn.config(state='normal')
            self.port_entry.config(state='disabled')
        except Exception as e:
            messagebox.showerror("初始化失败", str(e))
            self.update_status(f"初始化失败: {e}", is_error=True)

    def close_serial(self):
        if self.i2c:
            self.i2c.close()
            self.i2c = None
            self.mtp = None
        self.update_status("串口已关闭")
        self.init_btn.config(state='normal')
        self.close_btn.config(state='disabled')
        self.port_entry.config(state='normal')

    def ensure_initialized(self):
        if not self.i2c or not self.i2c.initialized:
            messagebox.showwarning("未初始化", "请先点击「初始化I2C」按钮")
            return False
        return True




    def parse_hex_bytes(self, s):
        """
        将输入的 HEX 字符串按空格或逗号切分，并转换为整数列表。
        支持 'AA BB' 格式，也支持连写格式 'AABB'。
        """
        text = str(s or "").strip()
        if not text:
            raise ValueError("原始TX不能为空")
        parts = re.split(r'[\s,]+', text)
        data = []
        for p in parts:
            if not p:
                continue
            p = p.strip().replace("0x", "").replace("0X", "")
            if not re.fullmatch(r'[0-9A-Fa-f]+', p):
                raise ValueError(f"非法HEX字节: {p}")
            # 支持连写格式，例如 9B00 -> 9B 00，A59B00 -> A5 9B 00
            if len(p) % 2 != 0:
                raise ValueError(f"HEX长度必须为偶数: {p}")
            for i in range(0, len(p), 2):
                data.append(int(p[i:i + 2], 16))
        if not data:
            raise ValueError("原始TX不能为空")
        return data

    def parse_hex_exact4(self, s):
        """
        解析必须是不超过 4 字节的 HEX 字符串，
        如果不足 4 字节，在高位（列表开头）填充 0 补齐为 4 字节。
        """
        text = str(s or "").strip().replace("0x", "").replace("0X", "")
        compact = re.sub(r'[\s,]+', '', text)
        if re.fullmatch(r'[0-9A-Fa-f]{8}', compact):
            return [int(compact[i:i + 2], 16) for i in range(0, 8, 2)]
        data = self.parse_hex_bytes(s)
        if len(data) > 4:
            raise ValueError("最多只能输入4字节数据")
        elif len(data) < 4:
            data = [0] * (4 - len(data)) + data
        return data


    def build_iic_payload(self, rw, addr, index, write_data=None):
        """
        构建 IIC 底层传输时的数据载荷与帧头（不包含设备地址 0x25 / 0xA5）。
        会对地址和索引进行位反转，拼接为底层所需的格式。
        """
        def reverse_bits(x, width):
            r = 0
            for _ in range(width):
                r = (r << 1) | (x & 1)
                x >>= 1
            return r

        def reverse_bits_32(x):
            x = ((x & 0x55555555) << 1) | ((x & 0xAAAAAAAA) >> 1)
            x = ((x & 0x33333333) << 2) | ((x & 0xCCCCCCCC) >> 2)
            x = ((x & 0x0F0F0F0F) << 4) | ((x & 0xF0F0F0F0) >> 4)
            x = ((x & 0x00FF00FF) << 8) | ((x & 0xFF00FF00) >> 8)
            x = ((x & 0x0000FFFF) << 16) | ((x & 0xFFFF0000) >> 16)
            return x

        # 0b110 -> 读, 0b010 -> 写
        if rw == 'read':
            rwcode = 0b110
        elif rw == 'write':
            rwcode = 0b010
        else:
            raise ValueError("rw must be 'read' or 'write'")

        # 将地址(5位)和索引(6位)在硬件发送前做位反转
        addr_rev = reverse_bits(addr, 5)
        index_rev = reverse_bits(index, 6)
        
        # 拼接 16 位的控制帧头 = (读写位 << 13) | (反转地址 << 8) | (反转索引 << 2)
        header_logical = (rwcode << 13) | (addr_rev << 8) | (index_rev << 2)

        high_byte = (header_logical >> 8) & 0xFF
        low_byte = header_logical & 0xFF
        
        # 返回前两个字节计算结果（首字节不算设备地址0x25）
        res = [high_byte, low_byte]
        if rw == 'write' and write_data:
            # 写模式下，把要发送的 4 字节数据当作一个 32 位整型整体反转后再转回字节流
            write_value = (write_data[0] << 24) | (write_data[1] << 16) | (write_data[2] << 8) | write_data[3]
            physical_value = reverse_bits_32(write_value)
            
            data_bytes = [
                (physical_value >> 24) & 0xFF,
                (physical_value >> 16) & 0xFF,
                (physical_value >> 8) & 0xFF,
                physical_value & 0xFF
            ]
            res.extend(data_bytes)
        return res

    def _execute_write(self, addr, index, write_data):
        """
        执行一条写指令：
        包含校验地址、构建命令载荷并发送，再打日志记录。
        """
        if not (0 <= addr <= 31):
            raise ValueError("ADDR 必须在 0~31 之间")
        if not (0 <= index <= 63):
            raise ValueError("INDEX 必须在 0~63 之间")
        if len(write_data) != 4:
            raise ValueError("写入数据必须为4字节")

        # 首发固定地址设备 ID -> 0x25
        tx = [0x25] + self.build_iic_payload('write', addr, index, write_data)
        self.i2c.send_custom_write_frame(tx, log_callback=None)
        
        orig_data_hex = ' '.join(f'{b:02X}' for b in write_data)
        self.log(
            f"WRITE | addr={addr}, index={index} | data={orig_data_hex}",
            record={
                'time': time.strftime('%Y-%m-%d %H:%M:%S'),
                'operation': 'WRITE',
                'addr': str(addr),
                'index': str(index),
                'write_value': orig_data_hex,
                'read_value': '',
                'raw_message': f'custom-write'
            }
        )

    def _execute_read(self, addr, index):
        """
        执行一条读指令：
        通过 0x25(阶段1) 和 0xA5(阶段2 读取) 发送命令读取 4 字节，数据读回后做位反转后显示。
        """
        def reverse_bits_32(x):
            x = ((x & 0x55555555) << 1) | ((x & 0xAAAAAAAA) >> 1)
            x = ((x & 0x33333333) << 2) | ((x & 0xCCCCCCCC) >> 2)
            x = ((x & 0x0F0F0F0F) << 4) | ((x & 0xF0F0F0F0) >> 4)
            x = ((x & 0x00FF00FF) << 8) | ((x & 0xFF00FF00) >> 8)
            x = ((x & 0x0000FFFF) << 16) | ((x & 0xFFFF0000) >> 16)
            return x

        if not (0 <= addr <= 31):
            raise ValueError("ADDR 必须在 0~31 之间")
        if not (0 <= index <= 63):
            raise ValueError("INDEX 必须在 0~63 之间")

        cmd_tail = self.build_iic_payload('read', addr, index)
        # 写控制帧阶段的设备地址使用 0x25
        tx1 = [0x25, cmd_tail[0], cmd_tail[1]]
        # 读数据阶段使用读取地址 0xA5
        tx2 = [0xA5]
        rx_len = 4
        # 调用核心通信接口完成带重启时序的 IIC 操作
        raw = self.i2c.read_stop_then_start(tx1, tx2, rx_len, log_callback=None)
        if raw is None or len(raw) < 4:
            self.log("READ | 无有效返回")
            return None
        
        # 将读回的数据作为物理位流对待，同SPI逻辑反转解析出逻辑原值
        physical_value = (raw[0] << 24) | (raw[1] << 16) | (raw[2] << 8) | raw[3]
        logical_value = reverse_bits_32(physical_value)
        logical_bytes = [
            (logical_value >> 24) & 0xFF,
            (logical_value >> 16) & 0xFF,
            (logical_value >> 8) & 0xFF,
            logical_value & 0xFF
        ]
        
        data_hex = ' '.join(f'{b:02X}' for b in logical_bytes)
        self.log(
            f"READ | addr={addr}, index={index} | rx={data_hex}",
            record={
                'time': time.strftime('%Y-%m-%d %H:%M:%S'),
                'operation': 'READ',
                'addr': str(addr),
                'index': str(index),
                'write_value': '',
                'read_value': data_hex,
                'raw_message': f"rx_len={rx_len}"
            }
        )
        return logical_bytes

    def do_fixed_write(self):
        if not self.ensure_initialized():
            return
        try:
            addr = int(self.addr_var.get().strip())
            index = int(self.index_var.get().strip())
            write_data = self.parse_hex_exact4(self.write_data_var.get())
            self._execute_write(addr, index, write_data)
        except Exception as e:
            messagebox.showerror("错误", str(e))

    def do_fixed_read(self):
        if not self.ensure_initialized():
            return
        try:
            addr = int(self.addr_var.get().strip())
            index = int(self.index_var.get().strip())
            self._execute_read(addr, index)
        except Exception as e:
            messagebox.showerror("错误", str(e))

    def do_write_then_read(self):
        if not self.ensure_initialized():
            return
        try:
            addr = int(self.addr_var.get().strip())
            index = int(self.index_var.get().strip())
            write_data = self.parse_hex_exact4(self.write_data_var.get())
            self._execute_write(addr, index, write_data)
            time.sleep(0.001)
            self._execute_read(addr, index)
        except Exception as e:
            messagebox.showerror("错误", str(e))

    def do_mtp_open_en(self):
        if not self.ensure_initialized() or not self.mtp: return
        try:
            self.mtp.open_mtp_en()
        except Exception as e:
            messagebox.showerror("MTP 错误", str(e))

    def do_mtp_close_en(self):
        if not self.ensure_initialized() or not self.mtp: return
        try:
            self.mtp.close_mtp_en()
        except Exception as e:
            messagebox.showerror("MTP 错误", str(e))

    def do_mtp_read_byte(self):
        if not self.ensure_initialized() or not self.mtp: return
        try:
            addr = int(self.mtp_addr_var.get().strip())
            if not (0 <= addr <= 511):
                raise ValueError("ADDR 必须在 0~511 之间")
            self.mtp.read_byte(addr)
        except Exception as e:
            messagebox.showerror("MTP 错误", str(e))

    def do_mtp_program_byte(self):
        if not self.ensure_initialized() or not self.mtp: return
        try:
            addr = int(self.mtp_addr_var.get().strip())
            data_hex = self.mtp_data_var.get().strip()
            if not data_hex: raise ValueError("请输入 Byte Data(HEX)")
            byte_val = int(data_hex, 16)
            if not (0 <= addr <= 511):
                raise ValueError("ADDR (MTP Byte) 必须在 0~511 之间")
            if not (0 <= byte_val <= 255):
                raise ValueError("Byte Data 必须在 00~FF 之间")
            self.mtp.program_byte(addr, byte_val)
        except Exception as e:
            messagebox.showerror("MTP 错误", str(e))

    def do_mtp_sector_erase(self):
        if not self.ensure_initialized() or not self.mtp: return
        try:
            sector = int(self.mtp_sector_var.get().strip())
            if not (0 <= sector <= 15):
                raise ValueError("Sector 必须在 0~15 之间")
            if messagebox.askyesno("擦除确认", f"确定要擦除扇区 {sector} 吗？"):
                self.mtp.erase_sector(sector)
        except Exception as e:
            messagebox.showerror("MTP 错误", str(e))

    def do_mtp_chip_erase(self):
        if not self.ensure_initialized() or not self.mtp: return
        try:
            if messagebox.askyesno("严重警告", "警告：即将全片擦除 (Chip Erase)！所有 MTP 数据将永久清空。\n\n确认要继续吗？"):
                self.mtp.erase_chip()
        except Exception as e:
            messagebox.showerror("MTP 错误", str(e))

    def do_recover_bus(self):
        if not self.ensure_initialized():
            return
        try:
            self.i2c.recover_bus_manual(log_callback=self.log)
            self.log(
                "BUS_RECOVER | 手动恢复完成",
                record={
                    'time': time.strftime('%Y-%m-%d %H:%M:%S'),
                    'operation': 'BUS_RECOVER',
                    'addr': '',
                    'index': '',
                    'write_value': '',
                    'read_value': '',
                    'raw_message': 'manual-bus-recover'
                }
            )
        except Exception as e:
            messagebox.showerror("错误", str(e))

    def generate_template(self):
        filename = filedialog.asksaveasfilename(
            title="保存批量操作模板",
            defaultextension=".xlsx",
            filetypes=[("Excel 文件", "*.xlsx")],
            initialfile=f"批量操作模板_{time.strftime('%Y%m%d_%H%M%S')}.xlsx"
        )
        if not filename:
            return

        try:
            wb = Workbook()
            ws = wb.active
            ws.title = "批量操作模板"

            headers = ["READ/WRITE/WRITETHENREAD", "ADDR", "INDEX", "[31:24]", "[23:16]", "[15:8]", "[7:0]"]
            ws.append(headers)

            note_row = [
                "填写操作类型（READ / WRITE / WRITETHENREAD）",
                "0~31",
                "0~63",
                "HEX (如 AA)",
                "HEX (如 BB)",
                "HEX (如 CC)",
                "HEX (如 DD)"
            ]
            ws.append(note_row)

            ws.append(["WRITE", 1, 5, "AA", "BB", "CC", "DD"])
            ws.append(["WRITETHENREAD", 2, 10, "12", "34", "56", "78"])
            ws.append(["READ", 3, 15, "", "", "", ""])

            header_font = Font(bold=True)
            for col in range(1, len(headers) + 1):
                ws.cell(row=1, column=col).font = header_font

            for col in ws.columns:
                max_length = 0
                column = col[0].column_letter
                for cell in col:
                    try:
                        if len(str(cell.value)) > max_length:
                            max_length = len(str(cell.value))
                    except:
                        pass
                ws.column_dimensions[column].width = min(max_length + 2, 25)

            wb.save(filename)
            messagebox.showinfo("成功", f"模板已生成：\n{filename}")
        except Exception as e:
            messagebox.showerror("错误", f"生成模板失败：\n{str(e)}")

    def parse_hex_byte(self, s, col_name):
        if s is None or str(s).strip() == "":
            return 0
        s = str(s).strip().upper()
        if re.fullmatch(r'[0-9A-F]{1,2}', s):
            val = int(s, 16)
            if 0 <= val <= 255:
                return val
            else:
                raise ValueError(f"{col_name} 值 {s} 超出范围 (0~255)")
        else:
            raise ValueError(f"{col_name} 必须是 1-2 位 HEX（如 'A3' 或 '5'）")

    def parse_number_list_or_range(self, text, min_val, max_val, field_name):
        if text is None:
            raise ValueError(f"{field_name} 不能为空")
        raw = str(text).strip()
        if not raw:
            raise ValueError(f"{field_name} 不能为空")

        values = []
        for part in raw.replace('，', ',').split(','):
            item = part.strip()
            if not item:
                continue
            if '-' in item:
                range_parts = item.split('-', 1)
                if len(range_parts) != 2:
                    raise ValueError(f"{field_name} 范围格式错误: {item}")
                start = int(range_parts[0].strip())
                end = int(range_parts[1].strip())
                if start > end:
                    raise ValueError(f"{field_name} 范围起始不能大于结束: {item}")
                for v in range(start, end + 1):
                    if not (min_val <= v <= max_val):
                        raise ValueError(f"{field_name} 超出范围({min_val}~{max_val}): {v}")
                    values.append(v)
            else:
                v = int(item)
                if not (min_val <= v <= max_val):
                    raise ValueError(f"{field_name} 超出范围({min_val}~{max_val}): {v}")
                values.append(v)
        if not values:
            raise ValueError(f"{field_name} 未解析到有效值")
        return values

    def read_data_and_export_template(self):
        if not self.ensure_initialized():
            return
        try:
            addr_values = self.parse_number_list_or_range(self.export_addr_var.get(), 0, 31, "ADDR")
            index_values = self.parse_number_list_or_range(self.export_index_range_var.get(), 0, 63, "INDEX")
        except Exception as e:
            messagebox.showerror("参数错误", str(e))
            return

        filename = filedialog.asksaveasfilename(
            title="保存读数据导出文件",
            defaultextension=".xlsx",
            filetypes=[("Excel 文件", "*.xlsx")],
            initialfile=f"读数据导出_{time.strftime('%Y%m%d_%H%M%S')}.xlsx"
        )
        if not filename:
            return

        headers = ["READ/WRITE/WRITETHENREAD", "ADDR", "INDEX", "[31:24]", "[23:16]", "[15:8]", "[7:0]"]
        try:
            wb = Workbook()
            ws = wb.active
            ws.title = "批量操作模板"
            ws.append(headers)

            header_font = Font(bold=True)
            for col in range(1, len(headers) + 1):
                ws.cell(row=1, column=col).font = header_font

            total_count = 0
            success_count = 0

            for addr in addr_values:
                for index in index_values:
                    total_count += 1
                    self.log(f"导出读取: addr={addr}, index={index}")
                    try:
                        raw = self._execute_read(addr, index)
                        if not raw or len(raw) < 4:
                            ws.append(["READ", addr, index, "", "", "", ""])
                            continue

                        b31, b23, b15, b7 = raw[0], raw[1], raw[2], raw[3]
                        ws.append(["READ", addr, index, f"{b31:02X}", f"{b23:02X}", f"{b15:02X}", f"{b7:02X}"])
                        success_count += 1
                        time.sleep(0.002)
                    except Exception as e:
                        self.log(f"!! 读取失败: addr={addr}, index={index}, err={e}")
                        ws.append(["READ", addr, index, "", "", "", ""])

            for col in ws.columns:
                max_length = 0
                column = col[0].column_letter
                for cell in col:
                    try:
                        cell_len = len(str(cell.value)) if cell.value is not None else 0
                        if cell_len > max_length:
                            max_length = cell_len
                    except Exception:
                        pass
                ws.column_dimensions[column].width = min(max_length + 2, 25)

            wb.save(filename)
            self.log(f"读数据并导出完成: 总计 {total_count} 条, 成功 {success_count} 条")
            messagebox.showinfo("完成", f"导出完成:\n{filename}\n总计 {total_count} 条，成功 {success_count} 条")
        except Exception as e:
            messagebox.showerror("导出失败", f"读数据并导出失败：\n{str(e)}")
            self.log(f"!! 读数据并导出异常: {e}")

    def import_and_execute_batch(self):
        if not self.ensure_initialized():
            return

        filename = filedialog.askopenfilename(
            title="选择批量操作 Excel 文件",
            filetypes=[("Excel 文件", "*.xlsx")]
        )
        if not filename:
            return

        try:
            wb = load_workbook(filename, data_only=True)
            ws = wb.active

            rows = list(ws.iter_rows(min_row=2, values_only=True))
            if not rows:
                messagebox.showwarning("警告", "Excel 中无有效数据行（从第2行开始）")
                return

            success_count = 0
            total_count = 0

            for idx, row in enumerate(rows, start=2):
                if all(cell is None or str(cell).strip() == "" for cell in row[:3]):
                    continue

                total_count += 1
                try:
                    op_str = str(row[0]).strip().upper() if row[0] else ""
                    if op_str not in ["READ", "WRITE", "WRITETHENREAD"]:
                        raise ValueError(f"操作类型必须是 READ / WRITE / WRITETHENREAD，当前值: '{op_str}'")

                    addr = int(row[1]) if row[1] is not None else 0
                    index = int(row[2]) if row[2] is not None else 0

                    if not (0 <= addr <= 31):
                        raise ValueError(f"ADDR 必须在 0~31，当前值: {addr}")
                    if not (0 <= index <= 63):
                        raise ValueError(f"INDEX 必须在 0~63，当前值: {index}")

                    write_data = []
                    if op_str in ["WRITE", "WRITETHENREAD"]:
                        b31 = self.parse_hex_byte(row[3], "[31:24]")
                        b23 = self.parse_hex_byte(row[4], "[23:16]")
                        b15 = self.parse_hex_byte(row[5], "[15:8]")
                        b7 = self.parse_hex_byte(row[6], "[7:0]")
                        write_data = [b31, b23, b15, b7]

                    self.log(f"--- 批量执行第 {total_count} 条 (Excel 第 {idx} 行) ---")
                    if op_str == "READ":
                        read_val = self._execute_read(addr, index)
                        if read_val is not None:
                            success_count += 1
                    elif op_str == "WRITE":
                        self._execute_write(addr, index, write_data)
                        success_count += 1
                    elif op_str == "WRITETHENREAD":
                        self._execute_write(addr, index, write_data)
                        time.sleep(0.001)
                        read_val = self._execute_read(addr, index)
                        if read_val is not None:
                            success_count += 1

                    time.sleep(0.005)
                except Exception as e:
                    error_msg = f"第 {idx} 行解析/执行失败: {str(e)}"
                    self.log(f"!! {error_msg}")
                    messagebox.showwarning("行执行失败", error_msg)
                    continue

            self.log(f"=== 批量执行完成：共 {total_count} 条，成功 {success_count} 条 ===")
            messagebox.showinfo("批量执行完成", f"共处理 {total_count} 条命令\n成功 {success_count} 条\n详情见日志")

        except Exception as e:
            messagebox.showerror("批量执行错误", f"执行失败：\n{str(e)}")
            self.log(f"!! 批量执行异常: {e}")

    def export_to_excel(self):
        if not self.log_records:
            messagebox.showinfo("提示", "当前无日志记录可导出")
            return
        filename = filedialog.asksaveasfilename(
            title="保存日志",
            defaultextension=".xlsx",
            filetypes=[("Excel 文件", "*.xlsx")],
            initialfile=f"I2C日志_{time.strftime('%Y%m%d_%H%M%S')}.xlsx"
        )
        if not filename:
            return
        try:
            wb = Workbook()
            ws = wb.active
            ws.title = "I2C操作日志"
            headers = ['时间', '操作类型', '地址(addr)', '索引(index)', '写入值', '读取值', '原始消息']
            ws.append(headers)
            header_font = Font(bold=True)
            for col in range(1, len(headers) + 1):
                ws.cell(row=1, column=col).font = header_font

            for record in self.log_records:
                ws.append([
                    record['time'],
                    record['operation'],
                    record['addr'],
                    record['index'],
                    record['write_value'],
                    record['read_value'],
                    record['raw_message']
                ])

            for col in ws.columns:
                max_length = 0
                column = col[0].column_letter
                for cell in col:
                    try:
                        if len(str(cell.value)) > max_length:
                            max_length = len(str(cell.value))
                    except:
                        pass
                adjusted_width = min(max_length + 2, 50)
                ws.column_dimensions[column].width = adjusted_width

            wb.save(filename)
            messagebox.showinfo("成功", f"日志已导出至：\n{filename}")
        except Exception as e:
            messagebox.showerror("导出失败", f"错误：{str(e)}")

# ========== 启动程序 ==========
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