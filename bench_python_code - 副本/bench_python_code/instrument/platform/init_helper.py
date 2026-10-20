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
    失败 ❌ 时询问用户是否忽略继续，用户选择退出则 exit 进程。

    参数：
        device_name: str - 设备名（如 "SWD"、"万用表"）
        ok: bool - 初始化是否成功
        confirm_prompt: str - 非空时，在初始化失败后提示用户确认
                           驱动与实际设备一致（用于同类型设备有多个驱动可选的场景）
    返回：
        bool - True 表示可继续（成功或用户忽略），
               False 不返回（用户选择退出时直接 exit）
    """
    if ok:
        print(f"✅ {device_name} 初始化成功")
        return True

    # 初始化失败 → 先确认驱动与实际设备是否匹配
    print(f"❌ {device_name} 初始化失败")
    if confirm_prompt:
        resp = input(f"   {confirm_prompt} (Y/n): ").strip().lower()
        if resp and resp != "y":
            print("程序退出。")
            sys.exit(1)

    # 再询问是否忽略继续
    resp = input(f"\n⚠️  {device_name} 初始化失败，是否忽略并继续？(y/N): ").strip().lower()
    if resp != "y":
        print("程序退出。")
        sys.exit(1)
    print(f"⚠️  已忽略 {device_name} 初始化失败，后续相关操作可能不可用。")
    return False
