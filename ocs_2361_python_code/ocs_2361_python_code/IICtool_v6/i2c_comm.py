import time
import struct
import serial
import os
import sys
import ctypes
import threading
import queue
import math


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
        self.iic_speed_code = 0x06  # 默认 100K
        self.iic_speed_freq = 1000
        # PWM 基准时钟（Hz），若厂商文档不同请按文档设置此值
        self.pwm_base_clock = 48000000
        self.read_timeout_ms = 20
        self.read_frame_gap_s = 0.0
        # 关闭读后恢复时钟，避免在STOP后追加额外时钟导致时序变长
        self.post_read_recovery_clocks = 0
        # A5读后延迟STOP：默认连续接收到7字节再STOP（可改为6）
        self.stop_after_total_read_bytes = 1

    def close(self):
        """关闭串口和释放资源"""
        if self.ser:
            try:
                if getattr(self.ser, 'is_open', False):
                    self.ser.close()
            except Exception:
                pass
            self.ser = None
        # 释放 DLL 资源，确保下次能重新 OpenUsb
        try:
            if getattr(self, 'dll', None) is not None:
                try:
                    if getattr(self, 'dll_ready', False):
                        try:
                            self.dll.CloseUsb(self.usb_index)
                        except Exception:
                            pass
                except Exception:
                    pass
                try:
                    del self.dll
                except Exception:
                    self.dll = None
        except Exception:
            pass
        self.initialized = False
        self.dll_ready = False

    def open(self):
        """打开串口，若已打开则复用"""
        if self.ser is None or not getattr(self.ser, 'is_open', False):
            try:
                self.ser = serial.Serial(self.port, self.baudrate, timeout=0.2)
            except Exception as e:
                raise Exception(f"串口打开失败: {e}")

    def _find_dll_path(self):
        base_dir = getattr(sys, '_MEIPASS', None) or os.path.dirname(sys.executable)
        if not os.path.exists(base_dir):
            base_dir = os.path.dirname(os.path.abspath(__file__))
        candidates = [
            os.path.join(base_dir, "USB2UARTSPIIICDLL.dll"),
            os.path.join(base_dir, "_internal", "USB2UARTSPIIICDLL.dll"),
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

        # SPI 相关函数原型
        try:
            self.dll.ConfigSPIParam.argtypes = [ctypes.c_uint, ctypes.c_uint, ctypes.c_uint, ctypes.c_uint]
            self.dll.ConfigSPIParam.restype = ctypes.c_int
            self.dll.SPISetCS0.argtypes = [ctypes.c_ubyte, ctypes.c_uint]
            self.dll.SPISetCS0.restype = ctypes.c_int
            self.dll.SPISetCS1.argtypes = [ctypes.c_ubyte, ctypes.c_uint]
            self.dll.SPISetCS1.restype = ctypes.c_int
            self.dll.SPISendAndRcvData.argtypes = [ctypes.c_ubyte, ctypes.c_ubyte, ctypes.c_ubyte, ctypes.c_uint, ctypes.c_ubyte, ctypes.c_ubyte, ctypes.POINTER(ctypes.c_ubyte), ctypes.POINTER(ctypes.c_ubyte), ctypes.c_uint, ctypes.c_uint, ctypes.c_uint]
            self.dll.SPISendAndRcvData.restype = ctypes.c_int
            self.dll.SPISendData.argtypes = [ctypes.c_uint, ctypes.c_uint, ctypes.POINTER(ctypes.c_ubyte), ctypes.c_uint, ctypes.c_uint]
            self.dll.SPISendData.restype = ctypes.c_int
            self.dll.SPIRcvData.argtypes = [ctypes.c_uint, ctypes.c_uint, ctypes.POINTER(ctypes.c_ubyte), ctypes.c_uint, ctypes.c_uint]
            self.dll.SPIRcvData.restype = ctypes.c_int
            # PWM & IO 原型（用于直接生成 PWM 或设置 IO）
            try:
                self.dll.PWMOut.argtypes = [ctypes.c_uint, ctypes.c_uint, ctypes.c_uint, ctypes.c_uint, ctypes.c_uint]
                self.dll.PWMOut.restype = ctypes.c_int
                self.dll.PWMClose.argtypes = [ctypes.c_uint]
                self.dll.PWMClose.restype = ctypes.c_int
                self.dll.IOSetAndRead.argtypes = [ctypes.c_uint, ctypes.c_uint, ctypes.c_uint, ctypes.c_uint]
                self.dll.IOSetAndRead.restype = ctypes.c_int
            except Exception:
                # 若 DLL 版本不含这些接口，忽略以便后续调用抛出明确错误
                pass
        except Exception:
            # 在部分旧版本 DLL 中可能不存在 SPI 接口声明，忽略错误
            pass

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
            try:
                del self.dll
            except Exception:
                self.dll = None
            raise Exception(f"OpenUsb 失败，返回值: {ret}")

        # 使用存储的速率（默认100K，可通过set_iic_speed修改）
        ret = self.dll.ConfigIICParam(self.iic_speed_code, self.iic_speed_freq, self.usb_index)
        if ret < 0:
            try:
                try:
                    self.dll.CloseUsb(self.usb_index)
                except Exception:
                    pass
            except Exception:
                pass
            try:
                del self.dll
            except Exception:
                self.dll = None
            raise Exception(f"ConfigIICParam 失败，返回值: {ret}")

        self.dll_ready = True
        if log_callback:
            log_callback(f"DLL已就绪: {dll_path}")

    def spi_send_sck_internal(self, pulse_count=25, total_us=500, log_callback=None):
        if getattr(self, 'initialized', False):
            # 已初始化状态下，记录日志并返回
            try:
                pass
            except Exception:
                pass
            self.log(f"SPI_SCK | 发送完成: {pulse_count} 脉冲")
        if not getattr(self, 'dll_ready', False):
            raise Exception('DLL 未就绪，无法使用 InternalList')

        if pulse_count <= 0:
            return

        byte_count = (int(pulse_count) + 8) // 9
        try:
            ret = self.dll.InternalListInitAllLine(1)
            if ret < 0:
                raise Exception(f'InternalListInitAllLine 失败: {ret}')

            tx_buf = (ctypes.c_ubyte * byte_count)(*([0xFF] * byte_count))
            null_ptr = ctypes.POINTER(ctypes.c_ubyte)()

            ret = self.dll.InternalListSetLine(
                0, 0, 0, 0,
                tx_buf, byte_count,
                null_ptr, 0,
                0, 0, 0, 0
            )
            if ret < 0:
                raise Exception(f'InternalListSetLine 失败: {ret}')

            trigger = (ctypes.c_uint * 1)(1)
            ret = self.dll.InternalListTsetOnce(trigger, self.usb_index)
            if ret < 0:
                raise Exception(f'InternalListTsetOnce 失败: {ret}')

            try:
                if hasattr(self.dll, 'InternalListStop'):
                    try:
                        self.dll.InternalListStop(self.usb_index)
                    except Exception:
                        pass
            except Exception:
                pass

            if log_callback:
                log_callback(f"→ SPI(SCK) 通过 InternalList 发送完成: {pulse_count} 脉冲, 约 {total_us} µs")
        except Exception as e:
            if log_callback:
                log_callback(f"!! spi_send_sck_internal 失败: {e}")
            raise

    def spi_send_sck_via_spisenddata(self, pulse_count=25, total_us=500, log_callback=None):
        if getattr(self, 'initialized', False):
            raise Exception('I2C 已初始化，SPI 与 I2C 不能同时使用')

        self._ensure_dll_ready(log_callback=log_callback)

        if not getattr(self, 'dll_ready', False):
            raise Exception('DLL 未就绪，无法使用 SPISendData')

        if pulse_count <= 0:
            return

        try:
            ret = self.dll.ConfigSPIParam(0, 0, 0, self.usb_index)
            if ret < 0:
                if log_callback:
                    log_callback(f"!! ConfigSPIParam 返回 {ret}")
        except Exception as e:
            if log_callback:
                log_callback(f"!! ConfigSPIParam 调用失败: {e}")

        byte_count = (int(pulse_count) + 7) // 8
        send_buf = (ctypes.c_ubyte * byte_count)(*([0xFF] * byte_count))

        try:
            ret = self.dll.SPISendData(0, 0, send_buf, byte_count, self.usb_index)
            if ret < 0:
                raise Exception(f"SPISendData 失败: {ret}")
            if log_callback:
                log_callback(f"→ SPISendData 发送完成: {byte_count} 字节，约 {byte_count*8} 个时钟（目标 {pulse_count}）")
        except Exception as e:
            if log_callback:
                log_callback(f"!! spi_send_sck_via_spisenddata 失败: {e}")
            raise

    # ----------------- PWM / IO 封装方法 -----------------
    def pwm_start(self, prescaler, period, pulse, pulse_num, log_callback=None):
        """启动 PWM 输出（直接调用 DLL 的 PWMOut）。

        参数按 DLL 定义：prescaler, period, pulse, PulseNum
        """
        self._ensure_dll_ready(log_callback=log_callback)
        try:
            ret = self.dll.PWMOut(int(prescaler), int(period), int(pulse), int(pulse_num), self.usb_index)
            if ret < 0:
                raise Exception(f"PWMOut 返回错误: {ret}")
            if log_callback:
                log_callback(f"→ PWM 已启动: prescaler={prescaler}, period={period}, pulse={pulse}, num={pulse_num}")
            return ret
        except Exception as e:
            if log_callback:
                log_callback(f"!! pwm_start 失败: {e}")
            raise



    def pwm_close(self, log_callback=None):
        """停止 PWM 输出（调用 DLL 的 PWMClose）。"""
        self._ensure_dll_ready(log_callback=log_callback)
        try:
            ret = self.dll.PWMClose(self.usb_index)
            if ret < 0:
                raise Exception(f"PWMClose 返回错误: {ret}")
            if log_callback:
                log_callback("→ PWM 已停止")
            return ret
        except Exception as e:
            if log_callback:
                log_callback(f"!! pwm_close 失败: {e}")
            raise

    def io_set_and_read(self, ionum, iodir, iobit, log_callback=None):
        """设置 IO 并返回当前 IO 状态（调用 DLL 的 IOSetAndRead）。

        - ionum: IO 编号
        - iodir: 方向（读/写 等，按 DLL 说明）
        - iobit: 要写入的位值
        返回：DLL 返回值（>=0 表示成功，具体含义参见说明文档）
        """
        self._ensure_dll_ready(log_callback=log_callback)
        try:
            ret = self.dll.IOSetAndRead(int(ionum), int(iodir), int(iobit), self.usb_index)
            if ret < 0:
                raise Exception(f"IOSetAndRead 返回错误: {ret}")
            if log_callback:
                log_callback(f"→ IOSetAndRead: IONum={ionum}, IODir={iodir}, IOBit={iobit} -> {ret}")
            return ret
        except Exception as e:
            if log_callback:
                log_callback(f"!! io_set_and_read 失败: {e}")
            raise

    def iic_send_sck_via_iic(self, pulse_count=35, total_us=500, speed_label=None, log_callback=None):
        """
        使用 IIC 的 SCL（CLK）来产生时钟脉冲。

        行为：
        - 若未指定 `speed_label`，则根据 `pulse_count/total_us` 计算目标频率并选择最近的速率标签；
        - 直接设置该速率并通过 DLL 的 IICSendData 发送必要字节以产生脉冲；
        - 不做多次尝试或回退，也不自动恢复之前的速率；发生错误将抛出异常。

        参数：
        - pulse_count: 目标时钟脉冲数
        - total_us: 期望总时长（微秒），仅用于计算所选速率
        - speed_label: 可选，显式指定速率标签（如 '100k'），优先于自动选择
        """
        if pulse_count <= 0:
            return

        # ensure DLL ready
        self._ensure_dll_ready(log_callback=log_callback)

        # 选择或使用指定的速率标签（不做多次尝试）
        if speed_label is None:
            try:
                desired = int(pulse_count * 1_000_000 / max(int(total_us), 1))
            except Exception:
                desired = 50000

            speed_candidates = [
                (400000, '400k'),
                (200000, '200k'),
                (100000, '100k'),
                (80000, '80k'),
                (50000, '50k'),
                (20000, '20k'),
                (10000, '10k'),
                (5000, '5k'),
                (1000, '1k')
            ]
            speed_label = min(speed_candidates, key=lambda x: abs(x[0] - desired))[1]

        # 设置速率（如失败，将抛出异常）
        self.set_iic_speed(speed_label)

        # 计算要发送的字节数并直接发送（每字节近似产生8个时钟）
        byte_count = (int(pulse_count) + 7) // 8
        if byte_count <= 0:
            byte_count = 1
        payload = [0xFF] * byte_count

        # 直接通过 DLL 发送（不尝试其它回退方法）
        self._dll_i2c_send(0, 0, payload, log_callback=log_callback)
        if log_callback:
            log_callback(f"→ IIC CLK 方法完成: 发送 {byte_count} 字节 (近似 {byte_count*8} 个时钟)，目标 {pulse_count}，速率 {speed_label}")

    def uart_send_sck_via_tx(self, pulse_count=25, total_us=500, log_callback=None):
        if getattr(self, 'initialized', False):
            raise Exception('I2C 已初始化，SPI 与 I2C 不能同时使用')

        if self.ser is None or not getattr(self.ser, 'is_open', False):
            try:
                self.open()
            except Exception as e:
                raise Exception(f"打开串口失败: {e}")

        target_sck = 50000
        target_baud = target_sck * 2

        candidates_baud = [int(target_baud), 115200, 100000, 57600, 230400, 921600]
        orig_baud = getattr(self.ser, 'baudrate', None)
        chosen_baud = None
        for b in candidates_baud:
            try:
                self.ser.baudrate = b
                chosen_baud = b
                break
            except Exception:
                continue

        if chosen_baud is None:
            raise Exception('无法设置合适的串口波特率以生成 SCK 脉冲')

        bits_needed = pulse_count * 2
        byte_count = (bits_needed + 7) // 8
        send_bytes = bytes([0xAA] * byte_count)

        try:
            try:
                self.ser.reset_input_buffer()
                self.ser.reset_output_buffer()
            except Exception:
                pass

            written = self.ser.write(send_bytes)
            self.ser.flush()
            if log_callback:
                log_callback(f"→ UART TX 发送 {written} 字节 (baud={chosen_baud})，目标脉冲 {pulse_count}")
        except Exception as e:
            if log_callback:
                log_callback(f"!! uart_send_sck_via_tx 失败: {e}")
            raise
        finally:
            try:
                if orig_baud is not None:
                    self.ser.baudrate = orig_baud
            except Exception:
                pass

    def set_iic_speed(self, speed_str):
        self._ensure_dll_ready()
        if not self.dll_ready:
            raise Exception("DLL未就绪，无法设置速率")
        
        speed_map = {
            "1k": 0x00,
            "5k": 0x01,
            "10k": 0x02,
            "20k": 0x03,
            "50k": 0x04,
            "80k": 0x05,
            "100k": 0x06,
            "200k": 0x07,
            "400k": 0x08
        }
        key = speed_str.lower()
        if key not in speed_map:
            raise ValueError(f"不支持的速率: {speed_str}")
        
        speed_code = speed_map[key]
        self.iic_speed_code = speed_code
        self.iic_speed_freq = 1000
        
        ret = self.dll.ConfigIICParam(self.iic_speed_code, self.iic_speed_freq, self.usb_index)
        if ret < 0:
            raise Exception(f"ConfigIICParam 速率设置失败，返回值: {ret}")

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

        for _ in range(3):
            try:
                if self.dll_ready:
                    self._dll_i2c_send(0, 1, [0x00], log_callback=None)
            except Exception:
                pass

        try:
            if self.dll_ready:
                self._dll_i2c_send(0, 1, [0xFF], log_callback=None)
        except Exception:
            pass

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
                ret = self.dll.ConfigIICParam(self.iic_speed_code, self.iic_speed_freq, self.usb_index)
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

        byte_count = (int(clock_count) + 8) // 9
        try:
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
        if not self.ser or not self.ser.is_open:
            self.open()
        init_cmd = bytearray([0x03, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00])
        if log_callback:
            log_callback(f"→ 发送 I2C 初始化命令: {init_cmd.hex().upper()}")
        self.ser.write(init_cmd)
        resp = self.ser.read(6)
        if log_callback:
            log_callback(f"← 初始化响应: {resp.hex().upper() if resp else '无'}")
        self.initialized = True
        return resp

    def send_custom_write_frame(self, tx_bytes, log_callback=None):
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

            raw = self._dll_i2c_two_start_read_ack_stop_immediate(tx1, tx2, rx_len, log_callback=None)
            if len(raw) < rx_len:
                return None

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
            self._recover_bus_default(log_callback=log_callback)
            raise