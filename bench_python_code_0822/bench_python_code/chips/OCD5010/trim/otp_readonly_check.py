"""
OCD5010 OTP 只读诊断 —— 验证「解锁 OTPKEY 后 OTPDR 能否读回真实内容」

背景: 真机运行曾出现 4 个槽位 TAG 全部读出 0x0, 导致烧写中止。
      经排查, OTP 默认锁定 (CR[18]=OPTLOCK), 未解锁时读 OTPDR 恒为 0x0。
      本脚本在 flash_otp_read 内置了解锁流程, 此诊断仅做只读探测:
        - 读 4 个槽位 TAG (0x8000/0x8008/0x8010/0x8018)
        - 读 CHIP INFO/UUID 区 (0x8020~0x8023)
        - 检查 CR[18] 解锁状态

判定标准:
  ① 全部 TAG 读出 0xFFFFFFFF (空白)  → 解锁+读取正常, 可以走正式烧写流程
  ② 部分 TAG 读出 0x00796783          → 读取正常, 且该槽位已烧写过
  ③ 全部 TAG 仍读出 0x0               → 芯片确认无 OTP 读回通路, 需改用记录/影子寄存器方案

用法 (需 J-Link 连接, 请用 UTF-8 控制台运行):
    python chips/OCD5010/trim/otp_readonly_check.py
"""

import os
import sys
import time
from pathlib import Path

# 将项目根目录加入 sys.path（确保独立运行时能找到其他模块）
_project_root = str(Path(__file__).parents[3])
if _project_root not in sys.path:
    sys.path.insert(0, _project_root)

from chips.OCD5010.trim.flash_write import flash_otp_read
from instrument.debug import SWD_CMD

# 4 个槽位 TAG 地址 + UUID 区
TAG_ADDRS = [0x8000, 0x8008, 0x8010, 0x8018]
UUID_ADDRS = [0x8020, 0x8021, 0x8022, 0x8023]
ANA_TAG = 0x00796783


def main():
    print("=" * 58)
    print("  OCD5010 OTP 只读诊断 (验证解锁读 OTP)")
    print("=" * 58)

    # ── 1. 初始化 SWD ──
    print("\n[1/3] 初始化 SWD 连接...")
    SWD_CMD.init_all()
    time.sleep(0.2)
    print("  SWD 就绪")

    # ── 2. 检查 OTP 解锁状态 (CR[18]=OPTLOCK) ──
    print("\n[2/3] 检查 OTP 解锁状态...")
    SWD_CMD.write_register(0x40020058, 1)          # 使能 FLASH 时钟
    time.sleep(0.001)
    SWD_CMD.write_register(0x4002204C, 0xA1B2C3D4)  # 解锁 key1
    SWD_CMD.write_register(0x4002204C, 0x5D6C7B8A)  # 解锁 key2
    time.sleep(0.001)
    cr_val = SWD_CMD.read_register_32bit(0x40022018)
    if cr_val is None:
        print("  ❌ 无法读取 CR 寄存器, 请检查 SWD 连接")
        SWD_CMD.write_register(0x4002204C, 0x0)
        SWD_CMD.write_register(0x40020058, 0)
        return
    if cr_val & (1 << 18):
        print(f"  ⚠️  CR[18]=OPTLOCK 仍为 1 → 解锁失败! CR=0x{cr_val:08X}")
    else:
        print(f"  ✅ CR[18]=OPTLOCK 已清除 (0), 解锁成功! CR=0x{cr_val:08X}")
    SWD_CMD.write_register(0x4002204C, 0x0)        # 恢复锁定 (只读探测, 不留解锁态)
    SWD_CMD.write_register(0x40020058, 0)

    # ── 3. 读取 4 个槽位 TAG + UUID ──
    print("\n[3/3] 读取槽位 TAG / UUID (flash_otp_read 内部带解锁)...")
    print()
    print("  ┌────────┬─────────────┬─────────────────┬──────────┐")
    print("  │  地址   │  读出值      │   含义          │  判定    │")
    print("  ├────────┼─────────────┼─────────────────┼──────────┤")

    read_results = []
    for addr in TAG_ADDRS:
        val = flash_otp_read(addr)
        if val is None:
            print(f"  │ 0x{addr:04X} │ 读取失败    │ SWD 通信异常     │  ❌      │")
            read_results.append(None)
            continue
        if val == ANA_TAG:
            meaning = "已烧写 (ANA_TAG)"
            verdict = "已用"
        elif val == 0xFFFFFFFF:
            meaning = "空白 (0xFFFFFFFF)"
            verdict = "空白"
        elif val == 0x0:
            meaning = "全 0 (未解锁/无读通路)"
            verdict = "⚠️ 异常"
        else:
            meaning = "未知值"
            verdict = "?"
        print(f"  │ 0x{addr:04X} │ 0x{val:08X} │ {meaning:<13} │ {verdict:<6} │")
        read_results.append(val)

    print("  ├────────┴─────────────┴─────────────────┴──────────┤")
    uuid_words = [flash_otp_read(a) for a in UUID_ADDRS]
    uuid_str = "".join(f"{w:08X}" if w is not None else "XXXXXXXX" for w in uuid_words)
    print(f"  │ UUID (0x8020~0x8023) = {uuid_str} │")
    print("  └────────────────────────────────────────────────────┘")

    # ── 结论 ──
    print("\n  ── 结论 ──")
    all_ok = read_results and all(v is not None for v in read_results)
    if all_ok and all(v == 0xFFFFFFFF for v in read_results):
        print("  ✅ 解锁 + 读取正常: 4 个槽位全空白, 可安全执行正式烧写流程")
    elif all_ok and any(v == ANA_TAG for v in read_results):
        n = sum(1 for v in read_results if v == ANA_TAG)
        print(f"  ✅ 解锁 + 读取正常: 检测到 {n} 个槽位已烧写")
    elif all_ok and all(v == 0x0 for v in read_results):
        print("  ❌ 4 个 TAG 仍全 0x0 → 芯片确认无 OTP 读回通路!")
        print("     需改用方案: 烧写次数以 Excel 记录为准 + 烧写后 reboot")
        print("     读 ANAC_TRM1/2 (0x40007C20/24) 影子寄存器做验证。")
    else:
        print("  ⚠️ 结果不一致, 请结合上方表格人工判断")


if __name__ == "__main__":
    main()
    input("\n按回车键退出...")
