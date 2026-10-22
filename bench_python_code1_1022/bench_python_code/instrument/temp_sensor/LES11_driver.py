# -*- coding: utf-8 -*-
"""
LES11-MC-Dx USB温度传感器驱动
Modbus RTU协议，默认地址1，波特率9600，None/8/1

自动识别：设备使用CH340/CH341系列USB转串口，端口描述与其他CH34x设备
（如OCD5010、ocd2810测试板）无法区分，因此通过主动发送Modbus读命令探测，
只有真正的温度模块会返回CRC校验通过的应答，从而自动锁定端口，无需手动
在设备管理器里查COM口。
"""

import time
from instrument.platform.serial_comm import SerialComm

SERIAL_KEYWORD = "CH34"
DEFAULT_BAUDRATE = 9600
DEFAULT_ADDR = 1


def crc16_modbus(data: bytes) -> int:
    crc = 0xFFFF
    for b in data:
        crc ^= b
        for _ in range(8):
            if crc & 1:
                crc = (crc >> 1) ^ 0xA001
            else:
                crc >>= 1
    return crc


def build_read_temp_cmd(addr=DEFAULT_ADDR, channels=1):
    frame = bytes([addr, 0x03, 0x00, 0x00, 0x00, channels])
    crc = crc16_modbus(frame)
    return frame + bytes([crc & 0xFF, (crc >> 8) & 0xFF])


def parse_temp_response(resp: bytes, channels: int):
    """解析应答，返回长度为channels的温度列表（℃），传感器缺失/损坏为None。"""
    expected_len = 3 + channels * 2 + 2
    if len(resp) < expected_len:
        return None

    body, recv_crc = resp[:-2], resp[-2] | (resp[-1] << 8)
    if crc16_modbus(body) != recv_crc:
        return None

    temps = []
    for i in range(channels):
        hi, lo = resp[3 + i * 2], resp[4 + i * 2]
        raw = (hi << 8) | lo
        if raw == 0x7FFF:
            temps.append(None)
        elif raw & 0x8000:
            temps.append(-(((~raw) & 0xFFFF) + 1) / 10.0)
        else:
            temps.append(raw / 10.0)
    return temps


class LES11TempSensor:
    """LES11-MC-Dx 温度传感器驱动，独立串口实例，不与全局串口单例共用。"""

    def __init__(self, channels=1, addr=DEFAULT_ADDR, baudrate=DEFAULT_BAUDRATE):
        self.channels = channels
        self.addr = addr
        self.baudrate = baudrate
        self.ser = None
        self.port = None

    def _probe_port(self, device: str) -> bool:
        probe = SerialComm(device, baudrate=self.baudrate, timeout=0.5)
        if probe.init_serial() != 0:
            return False

        cmd = build_read_temp_cmd(self.addr, self.channels)
        resp = probe.send_and_receive(cmd, response_size=3 + self.channels * 2 + 2, delay=0.05)
        ok = parse_temp_response(resp, self.channels) is not None
        probe.close()
        return ok

    def auto_connect(self, keyword=SERIAL_KEYWORD) -> bool:
        """遍历所有匹配keyword的串口，探测出真正响应Modbus读温度命令的那个。"""
        candidates = SerialComm.find_serial_ports_by_description(keyword)
        for device, desc in candidates:
            print(f"探测 {device} ({desc}) ...")
            if self._probe_port(device):
                self.ser = SerialComm(device, baudrate=self.baudrate, timeout=1)
                self.ser.init_serial()
                self.port = device
                print(f"✅ 温度传感器已锁定: {device}")
                return True

        print(f"❌ 未找到响应的温度传感器（已探测 {len(candidates)} 个 {keyword} 串口）")
        return False

    def read_temperatures(self):
        """返回温度列表（℃），长度为self.channels，缺失/损坏通道为None。失败返回None。"""
        if self.ser is None or not self.ser.is_open:
            print("错误: 温度传感器未连接")
            return None

        cmd = build_read_temp_cmd(self.addr, self.channels)
        resp = self.ser.send_and_receive(cmd, response_size=3 + self.channels * 2 + 2, delay=0.05)
        return parse_temp_response(resp, self.channels)

    def close(self):
        if self.ser is not None:
            self.ser.close()
            self.ser = None


if __name__ == "__main__":
    sensor = LES11TempSensor(channels=1)
    if sensor.auto_connect():
        for _ in range(5):
            print(sensor.read_temperatures())
            time.sleep(1)
        sensor.close()
