# -*- coding: utf-8 -*-
"""
串口通信模块
提供串口初始化、发送、接收和关闭功能
可以在多个模块中共享同一个串口实例
"""

import serial
import serial.tools.list_ports
import time
import sys
import os

# 全局串口实例
_serial_instance = None

class SerialComm:
    """串口通信类"""

    def __init__(self, port=None, baudrate=9600, timeout=2):
        """
        初始化串口

        Args:
            port: 串口端口，如 'COM3'，如果为None则自动检测
            baudrate: 波特率，默认9600
            timeout: 超时时间（秒），默认2
        """
        self.ser = None
        self.port = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.is_open = False

    def init_serial(self, port=None, baudrate=None, timeout=None):
        """
        初始化串口连接

        Args:
            port: 串口端口，如果为None则使用self.port
            baudrate: 波特率，如果为None则使用self.baudrate
            timeout: 超时时间，如果为None则使用self.timeout

        Returns:
            0: 成功
            -1: 失败
        """
        if port is not None:
            self.port = port
        if baudrate is not None:
            self.baudrate = baudrate
        if timeout is not None:
            self.timeout = timeout

        # 如果端口未指定，尝试自动检测
        if self.port is None:
            ports = self.list_serial_ports()
            if ports:
                self.port = ports[0]
                print(f"自动检测到串口: {self.port}")
            else:
                print("错误: 未找到可用的串口")
                return -1

        try:
            self.ser = serial.Serial(
                port=self.port,
                baudrate=self.baudrate,
                timeout=self.timeout
            )

            # 清空缓冲区
            self.ser.reset_input_buffer()
            self.ser.reset_output_buffer()

            self.is_open = True
            print(f"串口 {self.port} 已打开，波特率: {self.baudrate}")
            return 0

        except serial.SerialException as e:
            print(f"串口打开失败: {e}")
            return -1

    def send_data(self, data):
        """
        发送数据到串口

        Args:
            data: 要发送的数据，可以是字符串或字节

        Returns:
            发送的字节数，失败返回-1
        """
        if not self.is_open or self.ser is None:
            print("错误: 串口未打开")
            return -1

        try:
            if isinstance(data, str):
                data = data.encode('utf-8', 'ignore')

            sent = self.ser.write(data)
            self.ser.flush()  # 确保数据发送完成
            return sent

        except Exception as e:
            print(f"发送数据失败: {e}")
            return -1

    def receive_data(self, size=256):
        """
        从串口接收数据

        Args:
            size: 要读取的最大字节数

        Returns:
            接收到的数据（字节），失败返回空字节
        """
        if not self.is_open or self.ser is None:
            print("错误: 串口未打开")
            return b''

        try:
            data = self.ser.read(size)
            return data
        except Exception as e:
            print(f"接收数据失败: {e}")
            return b''

    def send_and_receive(self, data, response_size=256, delay=0.01):
        """
        发送数据并等待响应

        Args:
            data: 要发送的数据
            response_size: 预期响应大小
            delay: 发送后等待的时间（秒）

        Returns:
            响应数据（字节）
        """
        sent = self.send_data(data)
        if sent <= 0:
            return b''

        time.sleep(delay)
        return self.receive_data(response_size)

    def send_only(self, data, expect=None, ack_timeout=0.05, drain=True):
        """只发送命令，不按固定长度等待响应（命令类指令专用）。

        为什么需要它：
            self.ser.read(size) 会**一直阻塞到读满 size 字节或串口超时**（默认 2 s）。
            固件对 spi_cmd_set_val 这类命令只回一行短文本（如 "input pass\\n"，约 12 字节），
            所以 send_and_receive(…, response_size=256) 每条命令都要白等满 2 s，
            扫描几百个点时这就是主要耗时。

        本方法发送后：
            · 给了 expect：在 ack_timeout 内拼接收到的数据，一旦出现 expect 立即返回 True；
            · 没给 expect：只在 ack_timeout 内收一下已经到达的数据（清缓冲，避免堆积），
              不等待、不做判定。

        Args:
            data: str/bytes - 要发送的数据
            expect: str/None - 期望出现在应答里的关键字（如 "input pass"）
            ack_timeout: float - 等应答的最长时间（秒，默认 0.05）
            drain: bool - True 时把超时后仍残留的输入清掉，避免影响下一条命令

        Returns:
            bool - 给了 expect 时表示是否收到；没给 expect 时表示发送是否成功
        """
        if not self.is_open or self.ser is None:
            print("错误: 串口未打开")
            return False

        if isinstance(data, str):
            data = data.encode('utf-8', 'ignore')

        try:
            self.ser.write(data)
            self.ser.flush()
        except Exception as e:
            print(f"发送数据失败: {e}")
            return False

        if expect is None:
            # 不需要应答：发完直接清掉固件的回显（"input pass" 等），
            # 不做任何读取等待，单条命令约 1~3 ms
            if drain:
                try:
                    self.ser.reset_input_buffer()
                except Exception:
                    pass
            return True

        old_timeout = self.ser.timeout
        try:
            self.ser.timeout = ack_timeout
            got = b""
            deadline = time.time() + ack_timeout
            target = expect.encode("utf-8", "ignore")
            while time.time() < deadline:
                chunk = self.ser.read(self.ser.in_waiting or 1)
                if not chunk:
                    break
                got += chunk
                if target in got:
                    return True
            return False
        except Exception as e:
            print(f"接收数据失败: {e}")
            return False
        finally:
            self.ser.timeout = old_timeout
            if drain:
                try:
                    self.ser.reset_input_buffer()
                except Exception:
                    pass

    def close(self):
        """关闭串口"""
        if self.ser is not None and self.is_open:
            self.ser.close()
            self.is_open = False
            print(f"串口 {self.port} 已关闭")

    @staticmethod
    def list_serial_ports():
        """
        列出所有可用的串口

        Returns:
            可用串口列表
        """
        ports = []
        try:
            available_ports = list(serial.tools.list_ports.comports())
            for port in available_ports:
                ports.append(port.device)
        except Exception as e:
            print(f"获取串口列表失败: {e}")

        return ports

    @staticmethod
    def find_serial_port_by_description(keyword):
        """
        通过描述信息查找串口

        Args:
            keyword: 关键词（不区分大小写）

        Returns:
            匹配的串口设备名，未找到返回None
        """
        try:
            available_ports = list(serial.tools.list_ports.comports())
            for port in available_ports:
                if keyword.lower() in port.description.lower():
                    return port.device
        except Exception as e:
            print(f"查找串口失败: {e}")

        return None

    @staticmethod
    def find_serial_ports_by_description(keyword):
        """
        通过描述信息查找所有匹配的串口。

        Args:
            keyword: 关键词（不区分大小写）

        Returns:
            list - 匹配的串口列表，每项为 (device, description)
        """
        results = []
        try:
            available_ports = list(serial.tools.list_ports.comports())
            for port in available_ports:
                if keyword.lower() in port.description.lower():
                    results.append((port.device, port.description))
        except Exception as e:
            print(f"查找串口失败: {e}")

        return results


# 全局函数，便于其他模块调用

def serial_init(port=None, baudrate=9600, timeout=2):
    """
    全局串口初始化函数

    Args:
        port: 串口端口
        baudrate: 波特率
        timeout: 超时时间

    Returns:
        SerialComm实例，失败返回None
    """
    global _serial_instance

    if _serial_instance is not None and _serial_instance.is_open:
        print("串口已经初始化")
        return _serial_instance

    _serial_instance = SerialComm(port, baudrate, timeout)
    if _serial_instance.init_serial() == 0:
        return _serial_instance
    else:
        _serial_instance = None
        return None


def serial_send(data):
    """
    全局串口发送函数

    Args:
        data: 要发送的数据

    Returns:
        发送的字节数，失败返回-1
    """
    global _serial_instance

    if _serial_instance is None or not _serial_instance.is_open:
        print("错误: 串口未初始化")
        return -1

    return _serial_instance.send_data(data)


def serial_receive(size=256):
    """
    全局串口接收函数

    Args:
        size: 要读取的最大字节数

    Returns:
        接收到的数据
    """
    global _serial_instance

    if _serial_instance is None or not _serial_instance.is_open:
        print("错误: 串口未初始化")
        return b''

    return _serial_instance.receive_data(size)


def serial_close():
    """全局串口关闭函数"""
    global _serial_instance

    if _serial_instance is not None:
        _serial_instance.close()
        _serial_instance = None


def get_serial_instance():
    """获取全局串口实例"""
    return _serial_instance


# 测试代码
if __name__ == "__main__":
    print("串口通信模块测试")
    print("可用串口:", SerialComm.list_serial_ports())

    # 测试自动检测串口
    ser = serial_init(baudrate=115200)
    if ser is not None:
        # 发送测试数据
        test_data = "AT\r\n"
        print(f"发送: {test_data}")
        sent = serial_send(test_data)
        print(f"发送字节数: {sent}")

        # 接收响应
        time.sleep(0.1)
        response = serial_receive()
        if response:
            print(f"接收: {response}")

        # 关闭串口
        serial_close()
    else:
        print("串口初始化失败")