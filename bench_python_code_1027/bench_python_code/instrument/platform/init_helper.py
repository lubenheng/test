# -*- coding: utf-8 -*-
"""
初始化结果检查工具

提供统一的初始化成功/失败处理和用户交互逻辑，
供芯片层 init_all() 调用。
"""

import sys


def check_init(device_name, ok, confirm_prompt=None):
    """检查设备初始化结果。

    成功 ✅ 时打印信息并返回 True；
    失败 ❌ 时打印原因，然后提示「按任意键继续，N 退出」——
    默认继续（跳过该设备），只有明确输入 N 才退出程序。

    参数：
        device_name: str - 设备名（如 "SWD"、"万用表"）
        ok: bool - 初始化是否成功
        confirm_prompt: str - 非空时，在初始化失败后打印该提示，
                           用于提醒核对驱动与实际设备是否一致
                           （同类型设备有多个驱动可选的场景）
    返回：
        bool - True 表示初始化成功；
               False 表示失败但操作员选择继续（相关功能可能不可用）。
               操作员输入 N 时直接 exit 进程，不返回。
    """
    if ok:
        print(f"✅ {device_name} 初始化成功")
        return True

    print(f"❌ {device_name} 初始化失败")
    if confirm_prompt:
        print(f"   {confirm_prompt}")

    # 「任意键继续，N 退出」：除非明确输入 N，否则跳过该设备继续初始化后续设备
    try:
        resp = input(f"   {device_name} 初始化失败，按任意键继续，N 退出: ").strip().lower()
    except EOFError:
        # 非交互环境（stdin 被重定向）拿不到输入，按「继续」处理，避免直接抛异常
        print()
        resp = ""
    if resp in ("n", "no", "q", "quit"):
        print("程序退出。")
        sys.exit(1)

    print(f"⚠️  已跳过 {device_name}，后续相关操作可能不可用。")
    return False
