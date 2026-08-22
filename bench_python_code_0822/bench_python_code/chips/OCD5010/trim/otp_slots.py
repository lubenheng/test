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

★ 烧写次数判定 (重要，2026-08 真机确认):
  ODS5010 的 OTP **没有读回通路** —— OTPDR 只写不可读，即使解锁 OTPKEY，
  读 OTPDR 也恒为 0x0 (厂商 SDK 的 OTP 读函数为空 / flash_test.c 的"OTP读测试"
  实际在读 MAIN_FLASH，详见 flash_hal.c 与 flash_test.c)。因此:
    - OTP 读取只作参考信息，不再作为烧写次数判定依据
    - 烧写次数以 chip_program_records.xlsx 记录为准 (按芯片编号)
    - 芯片编号: 优先读 OTP UUID (若能读回)，否则操作员手动输入
    - 写后验证: flash_otp_write 内部 PG 错误标志检查 + (可选) reboot 后
      读 ANAC_TRM1/2 影子寄存器 (0x40007C20/0x40007C24) 与写入值比对

参考: 《FLASH信息表》/ ODS5010 FLASH简介.docx / flash_test.c / flash_hal.c
"""

import os
import time

from chips.OCD5010.trim.flash_write import flash_otp_read, flash_otp_write

# ANA_TAG 值，boot 据此识别有效槽位
ANA_TAG = 0x00796783
MAX_BURN = 4

# (槽位号, TAG地址, VTRIM1地址, VTRM2地址)
OTP_SLOTS = [
    (1, 0x8000, 0x8001, 0x8002),
    (2, 0x8008, 0x8009, 0x800A),
    (3, 0x8010, 0x8011, 0x8012),
    (4, 0x8018, 0x8019, 0x801A),
]

# 芯片 128 位 UUID 存放地址 (OTP 0x8020~0x8023)
UUID_ADDRS = [0x8020, 0x8021, 0x8022, 0x8023]

# 烧写后 reboot 生效时，VTRIM1/VTRM2 映射到的影子寄存器 (ANAC 模块)
ANAC_TRM1_ADDR = 0x40007C20
ANAC_TRM2_ADDR = 0x40007C24


def read_otp_status():
    """
    尽力读取 4 个槽位 TAG 地址，仅作参考信息。

    由于 OTP 无读回通路，读出值不可信 (解锁后读 OTPDR 仍恒为 0x0)，
    此函数不再用于判定烧写次数，只用于向操作员展示读取情况。

    返回: {
        'count': OTP 可读时按 TAG 统计的次数, 不可读时为 None,
        'readable': OTP 读取是否可靠 (全部读 0 / 读取失败时为 False),
        'slots': [ {slot, tag_addr, tag_val, used}, ... ]
    }
    """
    slots = []
    count = 0
    read_fail = False
    all_zero = True
    for slot_no, tag_addr, _, _ in OTP_SLOTS:
        tag_val = flash_otp_read(tag_addr)
        if tag_val is None:
            read_fail = True
            tag_val = 0xFFFFFFFF
        if tag_val != 0x0:
            all_zero = False
        used = (tag_val == ANA_TAG)
        slots.append({'slot': slot_no, 'tag_addr': tag_addr, 'tag_val': tag_val, 'used': used})
        if used:
            count += 1
    # 真实 OTP 不可能 4 个 TAG 全为 0x0 (空白=0xFFFFFFFF, 已写=0x00796783)。
    # 全 0 说明 OTP 无读回通路, 不能据此判定烧写次数。
    readable = (not read_fail) and (not all_zero)
    return {'count': count if readable else None,
            'readable': readable, 'slots': slots}


def read_chip_uid():
    """
    读取 OTP 0x8020~0x8023 的 128 位 UUID 作为芯片唯一标识。

    由于 OTP 无读回通路，读出 0x0 / 读取失败时返回 None (视为不可读)。

    返回: 32 位大写十六进制字符串；不可读返回 None；全空返回 "(UUID空白)"。
    """
    words = [flash_otp_read(a) for a in UUID_ADDRS]
    if any(w is None for w in words):
        return None
    # OTPDR 无读回通路时读出 0x0，视为不可读
    if any(w == 0x0 for w in words):
        return None
    if all(w == 0xFFFFFFFF for w in words):
        return "(UUID空白)"
    return "".join(f"{w:08X}" for w in words)


def get_chip_identity():
    """
    获取芯片身份标识 (用于烧写记录归档)。

    优先自动读 OTP UUID；OTP 不可读时要求操作员手动输入芯片编号。
    """
    uid = read_chip_uid()
    if uid and uid != "(UUID空白)":
        print(f"  📇 已自动识别芯片 UUID: {uid}")
        return uid

    if uid == "(UUID空白)":
        print("  ⚠️ OTP 可读但 UUID 区为空白 (0xFFFFFFFF)，请手动输入芯片编号")
    else:
        print("  ⚠️ OTP 无读回通路，无法自动读取 UUID，请手动输入芯片编号")
    while True:
        try:
            s = input("     芯片编号 (用于烧写记录归档, 例: CHIP-001): ").strip()
        except (EOFError, KeyboardInterrupt):
            print("\n  已取消")
            return None
        if s:
            return s
        print("     编号不能为空，请重新输入")


def get_next_slot(count):
    """
    按烧写次数返回下一个可写槽位 (次数以记录为准)。

    返回: (slot, status)；无可用槽位时 slot 为 None。
    """
    if count is None or count >= MAX_BURN:
        return None
    return OTP_SLOTS[count]


def write_slot(slot, vtrim1_word, vtrm2_word):
    """
    向指定槽位写入 TAG(0x00796783) + VTRIM1 + VTRM2。

    ⚠️ 由于 OTP 无读回通路，不做写前空白检查/写后回读 (读 OTPDR 恒为 0x0)。
       写入正确性由 flash_otp_write 内部的 PG 错误标志 (SR[1]=ERRF) 保证，
       与厂商 SDK 的校验方式一致。返回 True/False。
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
            print(f"  ❌ 写入 {name} OTP[{hex(addr)}] 失败 (PG 错误或超时)")
            return False
        print(f"  ✅ {name} OTP[{hex(addr)}] = 0x{val:08X} 编程完成 (无 PG 错误)")
    return True


def _get_burn_count_from_records(chip_id):
    """从 chip_program_records.xlsx 查询芯片已烧写次数 (无记录返回 0)。"""
    from chips.OCD5010.trim.program_records import get_chip_burn_count
    return get_chip_burn_count(chip_id)


def burn_trim_words(vtrim1_word, vtrm2_word):
    """
    将 VTRIM1/VTRM2 两个 32 位字烧写到下一个可用 OTP 槽位，并记录到 Excel。

    主流程:
        1. 识别芯片 (OTP UUID 或操作员输入编号)
        2. 尽力读 OTP TAG 作参考 (不阻塞)
        3. 烧写次数 = OTP 可读时以 OTP 为准, 否则以 Excel 记录为准
        4. 操作员确认后烧写 (TAG+VTRIM1+VTRM2, PG 错误校验)
        5. 写入烧写记录 / 更新芯片信息

    返回: (是否成功, 本次槽位号 or None, 芯片已烧写次数 or None)
    """
    from chips.OCD5010.trim.program_records import append_burn_record, upsert_chip_info

    # ── 1. 识别芯片 ──
    print("\n  【芯片识别】")
    chip_id = get_chip_identity()
    if not chip_id:
        print("❌ 未获取芯片编号，中止!")
        return False, None, None

    # ── 2. 尽力读 OTP 作参考 ──
    status = read_otp_status()
    if status['readable']:
        otp_count = status['count']
        print(f"  (参考) OTP TAG 可读, 按 TAG 统计已烧写 {otp_count}/{MAX_BURN} 次")
        for s in status['slots']:
            mark = "已写入" if s['used'] else "未写入"
            print(f"    槽位{s['slot']}: TAG {hex(s['tag_addr'])} = {hex(s['tag_val'])} [{mark}]")
    else:
        otp_count = None
        print("  (参考) OTP 无读回通路 (TAG 读回 0x0)，烧写次数改以记录为准")

    # ── 3. 判定烧写次数 ──
    rec_count = _get_burn_count_from_records(chip_id)
    if otp_count is not None and otp_count != rec_count:
        print(f"  ⚠️ OTP 按 TAG 统计 ({otp_count}) 与记录 ({rec_count}) 不一致，以 OTP 为准")
    count = otp_count if otp_count is not None else rec_count

    print(f"\n  【烧写状态】芯片 [{chip_id}] 已烧写 {count}/{MAX_BURN} 次 (来自{'OTP TAG' if otp_count is not None else 'Excel记录'})")

    slot = get_next_slot(count)
    if slot is None:
        print("❌ 该芯片 4 个 OTP 槽位已全部烧写，无法再烧写!")
        append_burn_record(chip_id, "已烧满拒绝", count, None, None, None, None)
        return False, None, count

    slot_no, tag_addr, v1_addr, v2_addr = slot
    print(f"\n  本次将烧写到槽位 {slot_no}: TAG={hex(tag_addr)}, VTRIM1={hex(v1_addr)}, VTRM2={hex(v2_addr)}")
    print(f"    VTRIM1 = 0x{vtrim1_word:08X}")
    print(f"    VTRM2  = 0x{vtrm2_word:08X}")

    # ── 4. 操作员确认 (OTP 不可读, 必须人工确认芯片身份) ──
    print("\n  ⚠️ OTP 无读回通路，写前无法自动检查槽位空白，请确认:")
    print(f"     - 芯片编号 [{chip_id}] 正确 (即上一颗芯片的编号记录无误)")
    print(f"     - 该芯片确实只烧写过 {count} 次")
    try:
        resp = input("  确认写入? (输入 yes 继续, 其他任意键取消): ").strip()
    except (EOFError, KeyboardInterrupt):
        resp = ""
    if resp.lower() != "yes":
        print("  ❌ 操作员取消，未写入")
        append_burn_record(chip_id, "人工取消", count, None, None, None, None)
        return False, None, count

    # ── 5. 烧写 ──
    ok = write_slot(slot, vtrim1_word, vtrm2_word)
    burn_no = count + 1
    if ok:
        append_burn_record(chip_id, "PASS", burn_no, burn_no, slot, vtrim1_word, vtrm2_word)
        upsert_chip_info(chip_id, burn_no)
        print(f"\n  ✅ 槽位 {slot_no} 烧写成功! 芯片 [{chip_id}] 已烧写 {burn_no}/{MAX_BURN} 次")
        print("     记录已写入 chip_program_records.xlsx")
    else:
        append_burn_record(chip_id, "FAIL", count, None, None, None, None)
        print(f"\n  ❌ 槽位 {slot_no} 烧写失败!")
    return ok, (slot_no if ok else None), burn_no if ok else count


def reboot_and_verify(vtrim1_word, vtrm2_word):
    """
    烧写后 reboot 使 OTP 生效, 并读 ANAC_TRM1/2 影子寄存器比对验证。

    ⚠️ 需要 RCC->RSTKEY 寄存器地址 (0x0000AB56 触发 reboot)。当前 SDK 未提供
    RCC 基地址, 由外部调用者注入或手动 reboot (断电重上电) 后运行
    verify_shadow_registers.py。

    返回: (是否一致, ANAC_TRM1 读出值, ANAC_TRM2 读出值)
    """
    from instrument.debug import SWD_CMD

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
