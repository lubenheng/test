"""
OCD5010 OTP 只读诊断 —— 验证解锁流程 + 读取锁/状态寄存器

★ 2026-08 实测修正 — 重要!
  OTP **无寄存器级读回**。FLC_DR(0x24) 只是 OTPDR(0x48) 的写锁存只读视图,
  实测证据: 只写过 0x8000 一处后, 所有地址 (4槽TAG + UUID) 的 DR 读回都等于
  最后写入的 OTPDR 值 (0x00796783)。SDK 的 OTP 读路径 (flash_hal.c) 也是空的。
  因此本脚本第 3 步读出的 DR 值**不能**用于判断槽位是否占用/空白,
  槽位占用以烧写记录 Excel 为准 (见 otp_slots.burn_trim_words)。

本脚本做只读探测 (写入仅用于解锁/清锁存, 不编程 OTP):
  1. 读 4 个槽位 TAG + CHIP INFO/UUID 区 (DR=写锁存, 仅诊断用)
  2. 检查 OTPCR[16]=OTPLOCK 解锁密钥是否正确 (实测 0xA1B2C3D4/0x5D6C7B8A 可清)
  3. 读取 OTPCR[16]/CR[24] 锁状态 (CR[24]=OPTLOCK 为 INFO/OPT 区锁, 与 OTP 区无关)

用途: 诊断 OTP 解锁密钥是否有效 (仅要求 OTPCR[16]=0)。

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

# 4 个槽位 TAG 地址 + UUID 区 (仅供诊断读取, DR=写锁存)
TAG_ADDRS = [0x8000, 0x8008, 0x8010, 0x8018]
UUID_ADDRS = [0x8020, 0x8021, 0x8022, 0x8023]


def main():
    print("=" * 58)
    print("  OCD5010 OTP 只读诊断 (验证解锁读 OTP)")
    print("=" * 58)

    # ── 1. 初始化 SWD ──
    print("\n[1/3] 初始化 SWD 连接...")
    SWD_CMD.init_all()
    time.sleep(0.2)
    print("  SWD 就绪")

    # ── 2. 检查 OTP 解锁状态 + FLASH 锁/状态寄存器 (诊断 ECCERR 根因) ──
    print("\n[2/3] 检查 OTP 解锁状态 + 锁/状态寄存器...")
    SWD_CMD.modify_register_range(0x40020058, 1, 0, 0)          # 使能 FLASH 时钟
    time.sleep(0.001)

    # 写密钥前先读一次锁/状态 (原始复位态)
    cr0 = SWD_CMD.read_register_32bit(0x40022018)    # FLC_CR
    otpcr0 = SWD_CMD.read_register_32bit(0x40022040) # FLC_OTPCR
    sr0 = SWD_CMD.read_register_32bit(0x40022010)    # FLC_SR
    print(f"  写密钥前: CR=0x{cr0:08X} (OPTLOCK[24]={1 if cr0 & (1<<24) else 0}, "
          f"LOCK[16]={1 if cr0 & (1<<16) else 0}, PG[0]={1 if cr0 & 1 else 0})")
    print(f"  写密钥前: OTPCR=0x{otpcr0:08X} (OTPLOCK[16]={1 if otpcr0 & (1<<16) else 0})")
    print(f"  写密钥前: SR=0x{sr0:08X}")

    SWD_CMD.write_register(0x4002204C, 0xA1B2C3D4)  # 解锁 key1
    SWD_CMD.write_register(0x4002204C, 0x5D6C7B8A)  # 解锁 key2
    time.sleep(0.001)
    otpcr = SWD_CMD.read_register_32bit(0x40022040)   # FLC_OTPCR
    if otpcr is None:
        print("  ❌ 无法读取 OTPCR 寄存器, 请检查 SWD 连接")
        SWD_CMD.write_register(0x4002204C, 0x0)
        SWD_CMD.modify_register_range(0x40020058, 0, 0, 0)
        return
    if otpcr & (1 << 16):
        print(f"  ⚠️  OTPCR[16]=OTPLOCK 仍为 1 → 解锁失败! OTPCR=0x{otpcr:08X}")
    else:
        print(f"  ✅ OTPCR[16]=OTPLOCK 已清除 (0), 解锁成功! OTPCR=0x{otpcr:08X}")
    SWD_CMD.write_register(0x4002204C, 0x0)        # 恢复锁定 (只读探测, 不留解锁态)
    SWD_CMD.modify_register_range(0x40020058, 0, 0, 0)

    # ── 3. 读取 4 个槽位 TAG + UUID (仅诊断, DR=写锁存) ──
    print("\n[3/3] 读取槽位 TAG / UUID (flash_otp_read 内部带解锁)...")
    print("  ★ 注意: 读回的是 FLC_DR=OTPDR 写锁存, 不是 OTP 阵列内容,")
    print("    所有地址往往相同; 本步仅用于诊断锁存, 不能判断槽位占用/空白。")
    print()
    print("  ┌────────┬─────────────┬─────────────────────┬──────────┐")
    print("  │  地址   │  读出值      │   含义              │  判定    │")
    print("  ├────────┼─────────────┼─────────────────────┼──────────┤")

    read_results = []
    for addr in TAG_ADDRS:
        val = flash_otp_read(addr)
        if val is None:
            print(f"  │ 0x{addr:04X} │ 读取失败    │ SWD 通信异常        │  ❌      │")
            read_results.append(None)
            continue
        # 所有值都只是 OTPDR 写锁存, 不再解释为阵列内容
        print(f"  │ 0x{addr:04X} │ 0x{val:08X} │ DR=OTPDR 写锁存 (非阵列) │  仅诊断  │")
        read_results.append(val)

    print("  ├────────┴─────────────┴─────────────────────┴──────────┤")
    uuid_words = [flash_otp_read(a) for a in UUID_ADDRS]
    uuid_str = "".join(f"{w:08X}" if w is not None else "XXXXXXXX" for w in uuid_words)
    print(f"  │ UUID (0x8020~0x8023) = {uuid_str} │")
    print("  └──────────────────────────────────────────────────────┘")

    # ── 结论 ──
    print("\n  ── 结论 ──")
    all_ok = read_results and all(v is not None for v in read_results)
    if not all_ok:
        print("  ❌ 部分地址读取失败 → 请检查 SWD 连接后重试")
    else:
        print("  ⚠️ 上述读回值均为 OTPDR 写锁存 (DR=锁存, 非阵列内容),")
        print("     无法据此判断槽位占用/空白。")
        print("  ✅ 若前面显示 OTPCR[16]=OTPLOCK 已清除 (0), 则 OTP 解锁正确;")
        print("     (CR[24]=OPTLOCK 是 INFO/OPT 区锁, 与 OTP 区无关, 无需理会)")
        print("     槽位占用请以烧写记录 Excel (PASS 次数) 为准,")
        print("     真实生效验证请 reboot 后运行 verify_shadow_registers.py 读影子寄存器。")


if __name__ == "__main__":
    main()
    input("\n按回车键退出...")
