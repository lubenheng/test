# -*- coding: utf-8 -*-
"""
串口初始化工具 - 通过关键词搜索并初始化串口

提供交互式搜索/选择/初始化串口的功能，供芯片层调用。
底层通信依赖 serial_comm 模块。
"""

import sys
from instrument.platform import serial_comm as _serial


def serial_find_and_init(keyword="CH340", baudrate=115200):
    """通过关键词搜索串口并初始化。

    自动搜索描述信息中匹配 keyword 的串口：
      - 未找到 → 提示失败
      - 找到 1 个 → 直接初始化
      - 找到多个 → 交互式让用户选择

    初始化失败时由调用方（check_init）负责询问用户是否忽略，
    本函数仅返回成功/失败信号。

    参数：
        keyword: str - 搜索关键词（不区分大小写，如 "CH340"）
        baudrate: int - 波特率（默认 115200）

    返回：
        bool - 串口初始化成功返回 True，失败返回 False
    """
    ports = _serial.SerialComm.find_serial_ports_by_description(keyword)
    serial_ok = False

    if len(ports) == 0:
        print(f"❌ 未检测到 {keyword} 串口")
    elif len(ports) == 1:
        port = ports[0][0]
        print(f"检测到 {keyword} 串口: {port}")
        if _serial.serial_init(port, baudrate=baudrate) is not None:
            print(f"✅ 串口初始化成功, 端口: {port}")
            serial_ok = True
        else:
            print(f"❌ {port} 初始化失败")
    else:
        print(f"\n检测到多个 {keyword} 串口:")
        print("-" * 40)
        for i, (dev, desc) in enumerate(ports, 1):
            print(f"  [{i}] {dev} — {desc}")
        print("-" * 40)
        choice = input("\n请选择串口编号: ").strip()
        if choice.isdigit():
            idx = int(choice)
            if 1 <= idx <= len(ports):
                port = ports[idx - 1][0]
                print(f"\n正在初始化 {port}...")
                if _serial.serial_init(port, baudrate=baudrate) is not None:
                    print(f"✅ 串口初始化成功, 端口: {port}")
                    serial_ok = True
                else:
                    print(f"❌ {port} 初始化失败")
            else:
                print("❌ 无效编号，程序退出。")
                sys.exit(1)
        else:
            print("❌ 无效输入，程序退出。")
            sys.exit(1)

    return serial_ok
