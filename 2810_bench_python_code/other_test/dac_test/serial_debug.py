#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
串口通信调试脚本
用于诊断为什么Python脚本不能修改DAC电压
"""

import sys
import os
import time

# 添加项目路径
current_dir = os.path.dirname(os.path.abspath(__file__))
project_root = os.path.dirname(os.path.dirname(current_dir))
if project_root not in sys.path:
    sys.path.insert(0, project_root)

try:
    from instrument.platform.serial_comm import (
        serial_init,
        serial_send,
        serial_receive,
        serial_close,
        SerialComm
    )
    print("成功导入串口通信模块")
except ImportError as e:
    print(f"导入串口通信模块失败: {e}")
    sys.exit(1)

def compare_with_serial_tool():
    """模拟串口助手的行为"""
    print("=== 串口通信调试 ===")

    # 1. 列出可用串口
    ports = SerialComm.list_serial_ports()
    print(f"可用串口: {ports}")

    if not ports:
        print("未找到可用串口")
        return

    # 2. 选择串口
    port = input(f"选择串口 (默认 {ports[0]}): ").strip()
    if not port:
        port = ports[0]

    # 3. 选择波特率
    baudrate = input("输入波特率 (默认 115200): ").strip()
    if not baudrate:
        baudrate = 115200
    else:
        baudrate = int(baudrate)

    # 4. 初始化串口
    print(f"初始化串口 {port}, 波特率 {baudrate}...")
    ser = serial_init(port=port, baudrate=baudrate, timeout=2)
    if not ser:
        print("串口初始化失败")
        return

    try:
        # 5. 测试不同终止符
        terminators = [
            ('\\r\\n', '\r\n'),
            ('\\n', '\n'),
            ('\\r', '\r'),
            ('无', '')
        ]

        print("\n测试不同终止符:")
        for name, term in terminators:
            print(f"\n终止符: {name}")
            cmd = f"spi_cmd_set_val Ori 32768{term}"
            print(f"发送命令: {cmd}")

            # 显示原始字节
            cmd_bytes = cmd.encode('utf-8')
            hex_str = ' '.join([f'{b:02X}' for b in cmd_bytes])
            print(f"原始字节(HEX): {hex_str}")

            sent = serial_send(cmd)
            print(f"发送字节数: {sent}")

            # 等待响应
            time.sleep(0.1)
            response = serial_receive(1024)
            if response:
                response_hex = ' '.join([f'{b:02X}' for b in response])
                response_str = response.decode('utf-8', errors='ignore').strip()
                print(f"收到响应: {response_str}")
                print(f"响应字节(HEX): {response_hex}")
            else:
                print("无响应")

            time.sleep(0.5)  # 等待DAC稳定

        # 6. 测试多个参数值
        print("\n\n测试多个参数值:")
        test_values = [0, 16384, 32768, 49152, 65535]

        for val in test_values:
            print(f"\n参数值: {val}")
            cmd = f"spi_cmd_set_val Ori {val}\r\n"  # 使用\r\n
            print(f"发送命令: {cmd.strip()}")

            cmd_bytes = cmd.encode('utf-8')
            hex_str = ' '.join([f'{b:02X}' for b in cmd_bytes])
            print(f"原始字节(HEX): {hex_str}")

            sent = serial_send(cmd)
            print(f"发送字节数: {sent}")

            time.sleep(0.1)
            response = serial_receive(1024)
            if response:
                response_str = response.decode('utf-8', errors='ignore').strip()
                print(f"收到响应: {response_str}")
            else:
                print("无响应")

            time.sleep(0.5)  # 等待DAC稳定

        # 7. 测试串口助手的精确行为
        print("\n\n模拟串口助手精确行为:")
        print("请用串口助手发送以下命令，观察响应:")
        print("1. spi_cmd_set_val Ori 32768\\r\\n")
        print("2. 注意观察是否有响应")
        print("3. 记录响应内容")

        # 8. 接收模式测试
        print("\n进入接收模式测试...")
        print("现在请用串口助手发送命令，Python脚本将显示接收到的数据")
        print("按Ctrl+C退出")

        try:
            while True:
                response = serial_receive(1024)
                if response:
                    response_hex = ' '.join([f'{b:02X}' for b in response])
                    response_str = response.decode('utf-8', errors='ignore').strip()
                    print(f"收到数据: {response_str}")
                    print(f"原始字节: {response_hex}")
                time.sleep(0.1)
        except KeyboardInterrupt:
            print("\n退出接收模式")

    finally:
        # 清理
        serial_close()
        print("串口已关闭")

def check_serial_config():
    """检查串口配置"""
    print("=== 串口配置检查 ===")

    import serial
    import serial.tools.list_ports

    ports = list(serial.tools.list_ports.comports())
    print(f"检测到 {len(ports)} 个串口:")

    for i, p in enumerate(ports):
        print(f"{i+1}. {p.device}: {p.description}")

    if ports:
        # 尝试打开第一个串口查看配置
        port = ports[0].device
        try:
            ser = serial.Serial(port, timeout=1)
            print(f"\n串口 {port} 配置:")
            print(f"  波特率: {ser.baudrate}")
            print(f"  数据位: {ser.bytesize}")
            print(f"  停止位: {ser.stopbits}")
            print(f"  奇偶校验: {ser.parity}")
            print(f"  流控制: XON/XOFF={ser.xonxoff}, RTS/CTS={ser.rtscts}, DTR/DSR={ser.dsrdtr}")
            ser.close()
        except Exception as e:
            print(f"打开串口失败: {e}")

if __name__ == "__main__":
    print("串口通信调试工具")
    print("1. 比较与串口助手的行为")
    print("2. 检查串口配置")
    print("3. 退出")

    choice = input("请选择 (1-3): ").strip()

    if choice == "1":
        compare_with_serial_tool()
    elif choice == "2":
        check_serial_config()
    else:
        print("退出")