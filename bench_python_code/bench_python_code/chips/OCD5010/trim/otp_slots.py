"""
OCD5010 OTP 4 次烧写槽位管理

OTP 区域 0x8000~0x803F (64 Word)，前 0x8000~0x801F 划分为 4 个烧写槽位:
  槽位1: TAG=0x8000  VTRIM1=0x8001  VTRM2=0x8002
  槽位2: TAG=0x8008  VTRIM1=0x8009  VTRM2=0x800A
  槽位3: TAG=0x8010  VTRIM1=0x8011  VTRM2=0x8012
  槽位4: TAG=0x8018  VTRIM1=0x8019  VTRM2=0x801A

每个槽位起始地址写入 ANA_TAG = 0x00796783。boot 按 0x8018→0x8010→0x8008→0x8000
的顺序检查各槽位 TAG，遇到 0x00796783 即加载该槽位的 VTRIM1/VTRM2 修调值。
所以烧写必须按槽位1→2→3→4 顺序进行，一颗芯片共 4 次烧写机会。

★ 槽位占用判定 (2026-08 实测修正 — 重要!):
  OTP **无寄存器级读回**。FLC_DR(0x24) 只是 OTPDR(0x48) 的写锁存只读视图 ——
  实测证据: 只写过 0x8000 一处后, 所有地址 (4槽TAG + UUID) 的 DR 读回都等于
  最后写入的 OTPDR 值 (0x00796783)。SDK 的 OTP 读路径 (flash_hal.c) 也是空的。
  因此:
    - 不能读 ANA_TAG 判断槽位是否占用 (早期方案不可行)
    - 槽位占用以烧写记录 Excel 中该芯片 (UID="无编号") 的 PASS 次数为准:
      本次写入槽位 = PASS 次数 + 1
    - 真实 OTP 内容只能 reboot 后读影子寄存器验证 (ANAC_TRM1/2 等)
    - 写前只须清 OTPCR[16]=OTPLOCK (写 OTPKEYR 密钥; CR[24]=OPTLOCK 是
      INFO/OPT 区锁, 与 OTP 区无关, 官方流程不动 CR)

参考: 《FLASH信息表》/ ODS5010 FLASH简介.docx / flash_test.c / flash_hal.c
"""

import os
import time

from chips.OCD5010.trim.flash_write import flash_otp_read, flash_otp_write

# ANA_TAG 值，boot 据此识别有效槽位 (写入槽位起始 word)
ANA_TAG = 0x00796783
MAX_BURN = 4

# OTP 空白默认值: 0→1 编程 (反熔丝型) 空白为 0x0。
# ★ 2026-08 实测: OTP 无寄存器级读回 (DR=写锁存), 无法读回判定空白/占用,
#   本常量仅作参考, 槽位占用以烧写记录 Excel 为准。
OTP_BLANK = 0x00000000

# (槽位号, TAG地址, VTRIM1地址, VTRM2地址)
OTP_SLOTS = [
    (1, 0x8000, 0x8001, 0x8002),
    (2, 0x8008, 0x8009, 0x800A),
    (3, 0x8010, 0x8011, 0x8012),
    (4, 0x8018, 0x8019, 0x801A),
]

# OTP 0x8020~0x8023 为 CHIP INFO/UUID 区。该芯片无编号 (UUID 区空白/不可用),
# 仅保留地址作为区域说明, 不用于芯片识别。
UUID_ADDRS = [0x8020, 0x8021, 0x8022, 0x8023]

# 烧写后 reboot 生效时，VTRIM1/VTRM2 映射到的影子寄存器 (ANAC 模块)
ANAC_TRM1_ADDR = 0x40007C20
ANAC_TRM2_ADDR = 0x40007C24


def read_otp_status():
    """
    [诊断] 读取 4 个槽位 TAG 地址的 FLC_DR (OTPDR 写锁存视图)。

    ★ 2026-08 实测: FLC_DR(0x24) 返回 OTPDR(0x48) 写锁存, 不是 OTP 阵列内容!
    所有地址的读回都等于最后写入 OTPDR 的值 (实测: 只写 0x8000 后 4槽+UUID
    全部读回 0x00796783)。因此本结果**不能**用于判断槽位占用/空白。
    槽位占用以烧写记录 Excel 为准 (见 burn_trim_words)。

    返回: {
        'count': 恒 0 (已弃用, 不再判定槽位占用),
        'readable': SWD 通信是否正常,
        'slots': [ {slot, tag_addr, tag_val(锁存), used: 恒 False}, ... ]
    }
    """
    slots = []
    read_fail = False
    for slot_no, tag_addr, _, _ in OTP_SLOTS:
        tag_val = flash_otp_read(tag_addr)
        if tag_val is None:
            read_fail = True
        slots.append({'slot': slot_no, 'tag_addr': tag_addr,
                      'tag_val': tag_val, 'used': False})
    return {'count': 0, 'readable': not read_fail, 'slots': slots}


def write_slot(slot, vtrim1_word, vtrm2_word):
    """
    向指定槽位写入 TAG(0x00796783) + VTRIM1 + VTRM2 三个 word。

    ★ 不做 DR 空白检查 —— DR 只是 OTPDR 写锁存, 不是阵列回读, 无法判断槽位
    是否已占用。槽位占用由调用方 (burn_trim_words) 依据烧写记录 Excel 判定。
    写后由 flash_otp_write 检查 SR (PGERR/TO 硬错误)。返回 True/False。
    """
    slot_no, tag_addr, v1_addr, v2_addr = slot
    items = [
        ("ANA_TAG", tag_addr, ANA_TAG),
        ("VTRIM1",  v1_addr,  vtrim1_word),
        ("VTRM2",   v2_addr,  vtrm2_word),
    ]

    print(f"  ── 槽位 {slot_no}: 写入 OTP 3 个 word ──")

    for name, addr, val in items:
        if not flash_otp_write(addr, val):
            print(f"  ❌ 写入 {name} OTP[{hex(addr)}] 失败")
            return False
        print(f"  ✅ {name} OTP[{hex(addr)}] = 0x{val:08X} 完成")
    return True


def burn_trim_words(vtrim1_word, vtrm2_word):
    """
    将 VTRIM1/VTRM2 两个 32 位字烧写到下一个空白 OTP 槽位，并记录到 Excel。

    ★ 2026-08 实测修正: OTP 无寄存器级读回 (FLC_DR=写锁存, 不是阵列内容),
    槽位占用**不能**读 ANA_TAG 判定。改为以烧写记录 Excel 中该芯片
    (UID="无编号") 的 PASS 次数为准:
        本次写入槽位 = PASS 次数 + 1
    每次成功烧写都会追加一条 PASS 记录, 计数递增。

    主流程:
        1. 从 Excel 统计该芯片已烧写 (PASS) 次数 → 本次槽位 = 次数+1
        2. 操作员确认后烧写 (TAG+VTRIM1+VTRM2, PGERR/TO 硬错误校验)
        3. 仅烧写成功时写一条烧写记录 (芯片 UID 记 "无编号")
        4. 提示用 reboot_and_verify() 复位后读影子寄存器确认生效

    返回: (是否成功, 本次槽位号 or None, 已烧写次数 or None)
    """
    from chips.OCD5010.trim.program_records import append_burn_record, get_chip_used_slots

    chip_uid = "无编号"
    used_count = get_chip_used_slots(chip_uid)

    print("\n  【槽位判定】依据烧写记录 Excel 中 [" + chip_uid + "] 的 PASS 次数")
    print(f"  已烧写 {used_count}/{MAX_BURN} 个槽位")
    print("  ⚠️ 注意: OTP 无寄存器级读回 (DR=写锁存), 无法读 ANA_TAG 判定占用;")
    print("     槽位占用以 Excel 烧写记录为准, 请确保记录与实物一致。")
    if used_count >= MAX_BURN:
        print(f"  ❌ 已烧写 {MAX_BURN}/{MAX_BURN} 个槽位, 无法再烧写!")
        append_burn_record(chip_uid, "已烧满拒绝", used_count, None, None, None, None)
        return False, None, used_count

    slot = OTP_SLOTS[used_count]      # used_count=0→槽位1, 1→槽位2 ...
    slot_no, tag_addr, v1_addr, v2_addr = slot
    print(f"\n  本次将烧写到槽位 {slot_no}: TAG={hex(tag_addr)}, VTRIM1={hex(v1_addr)}, VTRM2={hex(v2_addr)}")
    print(f"    VTRIM1 = 0x{vtrim1_word:08X}")
    print(f"    VTRM2  = 0x{vtrm2_word:08X}")

    # ── 2. 操作员确认 (OTP 一次性可编程, 人工复核后再烧) ──
    print("\n  ⚠️ OTP 一次性可编程, 请确认:")
    print(f"     - 本次将写入槽位 {slot_no} (依据 Excel 第 {used_count + 1} 次烧写)")
    try:
        resp = input("  确认写入? (输入 yes 继续, 其他任意键取消): ").strip()
    except (EOFError, KeyboardInterrupt):
        resp = ""
    if resp.lower() != "yes":
        print("  ❌ 操作员取消，未写入 (不记录 Excel)")
        return False, None, used_count

    # ── 3. 烧写 (仅烧写成功才写入 Excel 记录) ──
    ok = write_slot(slot, vtrim1_word, vtrm2_word)
    burn_no = used_count + 1
    if ok:
        append_burn_record(chip_uid, "PASS", burn_no, burn_no, slot, vtrim1_word, vtrm2_word)
        print(f"\n  ✅ 槽位 {slot_no} 烧写完成! 已写入 {burn_no}/{MAX_BURN} 个槽位")
        print("     记录已写入 chip_program_records.xlsx")
        print("  ⚠️ 请 reboot (断电重上电或 reboot_and_verify) 后读影子寄存器确认生效")
    else:
        print(f"\n  ❌ 槽位 {slot_no} 烧写失败，未写入 Excel")
    return ok, (slot_no if ok else None), burn_no if ok else used_count


def reboot_and_verify(vtrim1_word, vtrm2_word):
    """
    烧写后 reboot 使 OTP 生效, 并读 ANAC_TRM1/2 影子寄存器比对验证。

    官方验证流程 (Roadtest_A13_FLASH_5010): 烧写 → 复位 → 读影子寄存器。
    复位通过 SWD 的 swd_reset() (NVIC AIRCR SYSRESETREQ) 触发, 无需知道
    RCC->RSTKEY 地址; reboot 后重新 init SWD 再读 0x40007C20/0x40007C24。

    返回: (是否一致, ANAC_TRM1 读出值, ANAC_TRM2 读出值)
    """
    from instrument.debug import SWD_CMD

    swd = SWD_CMD.swd_handle
    print("  触发 SWD 复位 (NVIC AIRCR SYSRESETREQ)...")
    try:
        if hasattr(swd, "swd_reset"):
            swd.swd_reset()
        elif hasattr(swd, "reset"):
            swd.reset()
        else:
            print("  ⚠️ SWD 句柄无 swd_reset/reset 方法, 请手动断电重上电后运行 verify")
            return False, None, None
    except Exception as e:
        print(f"  ⚠️ SWD 复位异常: {e}, 请手动断电重上电后运行 verify")
        return False, None, None

    time.sleep(0.5)
    SWD_CMD.init_all()
    time.sleep(0.5)
    val1 = SWD_CMD.read_register_32bit(ANAC_TRM1_ADDR)
    val2 = SWD_CMD.read_register_32bit(ANAC_TRM2_ADDR)
    ok = (val1 == vtrim1_word) and (val2 == vtrm2_word)
    print(f"  ANAC_TRM1 = {hex(val1) if val1 is not None else 'None'} (期望 {hex(vtrim1_word)})")
    print(f"  ANAC_TRM2 = {hex(val2) if val2 is not None else 'None'} (期望 {hex(vtrm2_word)})")
    if ok:
        print("  ✅ 影子寄存器与写入值一致, 烧写生效确认!")
    else:
        print("  ⚠️ 影子寄存器与写入值不一致 (可能未 reboot, 或 reboot 后 SWD 未重连)")
    return ok, val1, val2


if __name__ == "__main__":
    # 自测: 仅演示判定逻辑 (需要真实 SWD 连接才能读 OTP)
    print("此模块依赖 SWD 连接，请通过 program_trim.py / main.py 调用。")
    print("4 个槽位: " + ", ".join(f"槽位{n}(TAG={hex(t)})" for n, t, _, _ in OTP_SLOTS))
