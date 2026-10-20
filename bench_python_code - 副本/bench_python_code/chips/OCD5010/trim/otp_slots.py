"""
OCD5010 OTP 4 次烧写槽位管理

OTP 区域 0x8000~0x803F (64 Word)，前 0x8000~0x801F 划分为 4 个烧写区域
(每个区域 8 word, 参考《FLASH信息表》/ decoded_flash_intro.txt):
  区域1: TAG=0x8000  VTRIM1=0x8001  VTRM2=0x8002  ADCTRM=0x8003  ANACINFO0~3=0x8004~0x8007
  区域2: TAG=0x8008  VTRIM1=0x8009  VTRM2=0x800A  ADCTRM=0x800B  ANACINFO0~3=0x800C~0x800F
  区域3: TAG=0x8010  VTRIM1=0x8011  VTRM2=0x8012  ADCTRM=0x8013  ANACINFO0~3=0x8014~0x8017
  区域4: TAG=0x8018  VTRIM1=0x8019  VTRM2=0x801A  ADCTRM=0x801B  ANACINFO0~3=0x801C~0x801F

每个区域起始地址写入 ANA_TAG = 0x00796783。boot 按 0x8018→0x8010→0x8008→0x8000
的顺序检查各区域 TAG，遇到 0x00796783 即加载该区域的模拟值
(VTRIM1/VTRM2/ADCTRM/ANACINFO0~3 → 影子寄存器)。
所以烧写必须按区域1→2→3→4 顺序进行，一颗芯片共 4 次烧写机会。

★ 2026-09: 区域从 3 word 扩为 8 word —— 新增 ADCTRM + ANACINFO0/1/2/3，
  写入方式与 trim 相同 (flash_otp_write 按地址写 32 位 word)。
  其中 ANACINFO0 存放 vref_1d2v 修调校准后的 VREF 输出电压 (浮点, IEEE 754
  单精度编码, 见 float_to_word/word_to_float); ADCTRM/ANACINFO1~3 值待定 →
  默认写 0 (0→1 编程, 写 0 为无操作不占熔丝)；调用方传 info_words 字典可覆盖。
  回读: VTRIM1/2 与 ANACINFO0/1/2/3 见下方影子寄存器地址; ADCTRM 回读地址
  待确认, 先只写不验证。

★ 槽位占用判定 (2026-08 确认 — 以 ANA_TAG 回读为准):
  按用户要求, 槽位占用改为**顺序回读 4 个槽位起始 TAG 地址**判定:
    槽位1 TAG=0x8000 / 槽位2 TAG=0x8008 / 槽位3 TAG=0x8010 / 槽位4 TAG=0x8018
  逐个读回, 若等于 ANA_TAG=0x00796783 说明该槽位已烧写过, 继续读下一槽位;
  第一个**不等于** ANA_TAG 的槽位即下一个烧写目标 (全新芯片全部为 0x0)。
  ★ 不再以烧写记录 Excel 的 PASS 次数作为槽位判定依据
    (Excel 记录仅保留作时间线归档, 见 program_records.py)。
  - 写前只须清 OTPCR[16]=OTPLOCK (写 OTPKEYR 密钥; CR[24]=OPTLOCK 是
    INFO/OPT 区锁, 与 OTP 区无关, 官方流程不动 CR)

参考: 《FLASH信息表》/ ODS5010 FLASH简介.docx / flash_test.c / flash_hal.c
"""

import os
import struct
import time

from chips.OCD5010.trim.flash_write import flash_otp_read, flash_otp_write

# ANA_TAG 值，boot 据此识别有效槽位 (写入槽位起始 word)
ANA_TAG = 0x00796783
MAX_BURN = 4

# OTP 空白默认值: 0→1 编程 (反熔丝型) 空白为 0x0。
# ★ 2026-08: 槽位占用判定以回读 TAG 为准 (== ANA_TAG 即已烧写, 见 read_otp_status)。
OTP_BLANK = 0x00000000

# (槽位号, TAG地址, VTRIM1地址, VTRM2地址,
#  ADCTRM地址, ANACINFO0地址, ANACINFO1地址, ANACINFO2地址, ANACINFO3地址)
OTP_SLOTS = [
    (1, 0x8000, 0x8001, 0x8002, 0x8003, 0x8004, 0x8005, 0x8006, 0x8007),
    (2, 0x8008, 0x8009, 0x800A, 0x800B, 0x800C, 0x800D, 0x800E, 0x800F),
    (3, 0x8010, 0x8011, 0x8012, 0x8013, 0x8014, 0x8015, 0x8016, 0x8017),
    (4, 0x8018, 0x8019, 0x801A, 0x801B, 0x801C, 0x801D, 0x801E, 0x801F),
]

# OTP 0x8020~0x8023 为 CHIP INFO/UUID 区。该芯片无编号 (UUID 区空白/不可用),
# 仅保留地址作为区域说明, 不用于芯片识别。
UUID_ADDRS = [0x8020, 0x8021, 0x8022, 0x8023]

# 烧写后 reboot 生效时，各 word 映射到的影子寄存器 (参考 decoded_flash_intro.txt)
ANAC_TRM1_ADDR  = 0x40007C20   # VTRIM1     → ANAC_TRM1 (ANAC 偏移 0x20)
ANAC_TRM2_ADDR  = 0x40007C24   # VTRM2      → ANAC_TRM2 (ANAC 偏移 0x24)
ANAC_INFO0_ADDR = 0x40007C28   # ANACINFO0  → ANAC INFO_0 (偏移 0x28)
ANAC_INFO1_ADDR = 0x40007C2C   # ANACINFO1  → ANAC INFO_1 (偏移 0x2C)
ANAC_INFO2_ADDR = 0x40007C30   # ANACINFO2  → ANAC INFO_2 (偏移 0x30)
ANAC_INFO3_ADDR = 0x40007C34   # ANACINFO3  → ANAC INFO_3 (偏移 0x34)
# ★ ADCTRM 影子寄存器地址待确认 (文档记 0x40012480, 用户要求回读先留空) →
#   只写入不验证 (见 INFO_SHADOW_ADDRS)。

# 区域内新增的 5 个信息 word 名称 (除 TAG/VTRIM1/VTRM2 外)
INFO_WORD_NAMES = ["ADCTRM", "ANACINFO0", "ANACINFO1", "ANACINFO2", "ANACINFO3"]

# 信息 word 名称 → 影子寄存器 (reboot 后回读验证用; ADCTRM 暂缺, 只写不验)
INFO_SHADOW_ADDRS = {
    "ANACINFO0": ANAC_INFO0_ADDR,
    "ANACINFO1": ANAC_INFO1_ADDR,
    "ANACINFO2": ANAC_INFO2_ADDR,
    "ANACINFO3": ANAC_INFO3_ADDR,
}

# =====================================================================
# ANACINFO0: VREF 修调校准后的输出电压 (float → 32 位字)
# =====================================================================
# ★ 2026-09: 每个区域 ANACINFO0 (0x8004 / 0x800C / 0x8014 / 0x801C) 存放
#   "vref_1d2v" 修调校准后实测的 VREF 输出基准电压。该电压是**浮点数**,
#   烧写前按 IEEE 754 单精度浮点格式编码成 32 位字 (单位: **mV**, 如
#   1200.3 mV → 0x4496099A) 写入 OTP; 回读时按同样格式解码。
#   OTP 是 0→1 一次性编程, 每区域各 word 只写一次, 浮点位模式不影响后续烧写。
def float_to_word(value):
    """把浮点电压 (单位 mV) 编码为 IEEE 754 单精度 32 位字 (ANACINFO0 用)。

    例: 1200.3 mV → 0x4496099A; 1200.0 mV → 0x44960000。
    传 None 时返回 0 (表示未测到, 缺省写 0 不占熔丝)。
    编码格式如需调整 (如 V/定点), 只改这里与 word_to_float 两个函数即可。
    """
    if value is None:
        return 0
    return struct.unpack("<I", struct.pack("<f", float(value)))[0]


def word_to_float(word):
    """把 ANACINFO0 回读的 32 位字解码回浮点电压 (单位 mV)。"""
    return struct.unpack("<f", struct.pack("<I", word & 0xFFFFFFFF))[0]


def read_otp_status():
    """
    顺序回读 4 个槽位起始 TAG 地址, 判定哪些槽位已烧写。

    判定标准 (2026-08 确认):
      - 逐个读 0x8000 / 0x8008 / 0x8010 / 0x8018 四个 TAG 地址
      - 读回 == ANA_TAG (0x00796783) → 该槽位已烧写 (used=True)
      - 读回 != ANA_TAG → 该槽位空白, 即下一个烧写目标
      - 全新芯片 4 个 TAG 全部为空白 (0x0 或非 ANA_TAG)

    返回: {
        'count': 已烧写槽位数 (0~4),
        'readable': SWD 通信是否正常,
        'slots': [ {slot, tag_addr, tag_val, used: bool}, ... ]
    }
    """
    slots = []
    count = 0
    read_fail = False
    for slot_no, tag_addr, *_ in OTP_SLOTS:
        tag_val = flash_otp_read(tag_addr)
        if tag_val is None:
            read_fail = True
        used = (tag_val == ANA_TAG)
        if used:
            count += 1
        slots.append({'slot': slot_no, 'tag_addr': tag_addr,
                      'tag_val': tag_val, 'used': used})
    return {'count': count, 'readable': not read_fail, 'slots': slots}


def readback_all_slots():
    """
    逐个回读 4 个区域全部 32 个 word (TAG/VTRIM1/VTRM2/ADCTRM/ANACINFO0~3) 并打印到日志。

    这是写入流程的诊断步骤: 把当前 OTP 各槽位的 FLC_DR 读回值全部显示在日志上,
    供操作员在确认烧写前直观判断槽位是否真的空白 (2026-08-24 用户需求:
    "在写入流程中将回读数据显示在日志上, 需要查看情况后再做判断")。

    ⚠️ 读回的是 FLC_DR(0x24)。该寄存器是 OTPDR 的写锁存只读视图, 是否真实反映
      OTP 阵列内容目前存疑 (见 flash_write.py 顶部注释):
        - 若 32 个地址读回全部为同一个值 → DR 是纯锁存, 无法反映阵列, 只能以
          reboot 后读影子寄存器 (ANAC_TRM1/2 等) 为准;
        - 若各地址读回值各不相同 / 已烧写槽位读出非 0x0 → DR 很可能真实反映
          阵列内容, 可据此判断槽位占用 (与 read_otp_status 的 TAG 判定互补)。

    返回: dict {slot_no: (tag, vtrim1, vtrm2, adctrm, info0, info1, info2, info3)};
          读失败项为 None
    """
    print("\n  【OTP 详细读回】4 区域 × 8 word (TAG/VTRIM1/VTRM2/ADCTRM/ANACINFO0~3) "
          "= 32 word (读 FLC_DR, quiet 模式)")
    result = {}
    for slot_no, tag_addr, v1_addr, v2_addr, adctrm_addr, i0, i1, i2, i3 in OTP_SLOTS:
        tag = flash_otp_read(tag_addr, quiet=True)
        v1 = flash_otp_read(v1_addr, quiet=True)
        v2 = flash_otp_read(v2_addr, quiet=True)
        adctrm = flash_otp_read(adctrm_addr, quiet=True)
        info0 = flash_otp_read(i0, quiet=True)
        info1 = flash_otp_read(i1, quiet=True)
        info2 = flash_otp_read(i2, quiet=True)
        info3 = flash_otp_read(i3, quiet=True)
        result[slot_no] = (tag, v1, v2, adctrm, info0, info1, info2, info3)

        def _fmt(v):
            return "0x%08X" % v if v is not None else "None"

        print(f"    区域{slot_no}: TAG[0x{tag_addr:04X}]={_fmt(tag)}  "
              f"VTRIM1[0x{v1_addr:04X}]={_fmt(v1)}  VTRM2[0x{v2_addr:04X}]={_fmt(v2)}\n"
              f"            ADCTRM[0x{adctrm_addr:04X}]={_fmt(adctrm)}  "
              f"INFO0[0x{i0:04X}]={_fmt(info0)}  INFO1[0x{i1:04X}]={_fmt(info1)}\n"
              f"            INFO2[0x{i2:04X}]={_fmt(info2)}  INFO3[0x{i3:04X}]={_fmt(info3)}")
    return result


def write_slot(slot, vtrim1_word, vtrm2_word, info_words=None):
    """
    向指定区域写入 8 个 word: TAG(0x00796783) + VTRIM1 + VTRM2 + ADCTRM + ANACINFO0~3。

    参数:
        slot: OTP_SLOTS 中的 9 元组 (槽位号, TAG, VTRIM1, VTRM2, ADCTRM, INFO0~3 地址)
        vtrim1_word / vtrm2_word: 打包后的 ANAC_TRM1/TRM2 32 位值
        info_words: 可选 dict { "ADCTRM": 0x..., "ANACINFO0": 0x..., ... }。
            值待定 → 缺省的 key 写 0 (0→1 编程, 写 0 为无操作不占熔丝)。
            ★ 2026-09: 区域固定烧写 8 个 word, 不传 info_words 时新增 word 全写 0。

    ★ 不做 DR 空白检查 —— 槽位占用由调用方 (burn_trim_words) 依据回读 TAG
    (== ANA_TAG) 判定。写后由 flash_otp_write 检查 SR (PGERR/TO 硬错误)。
    返回 True/False。
    """
    (slot_no, tag_addr, v1_addr, v2_addr,
     adctrm_addr, i0_addr, i1_addr, i2_addr, i3_addr) = slot
    info = info_words or {}
    items = [
        ("ANA_TAG",    tag_addr,    ANA_TAG),
        ("VTRIM1",     v1_addr,     vtrim1_word),
        ("VTRM2",      v2_addr,     vtrm2_word),
        ("ADCTRM",     adctrm_addr, info.get("ADCTRM", 0)),
        ("ANACINFO0",  i0_addr,     info.get("ANACINFO0", 0)),
        ("ANACINFO1",  i1_addr,     info.get("ANACINFO1", 0)),
        ("ANACINFO2",  i2_addr,     info.get("ANACINFO2", 0)),
        ("ANACINFO3",  i3_addr,     info.get("ANACINFO3", 0)),
    ]

    print(f"  ── 区域 {slot_no}: 写入 OTP 8 个 word ──")

    for name, addr, val in items:
        if not flash_otp_write(addr, val):
            print(f"  ❌ 写入 {name} OTP[{hex(addr)}] 失败")
            return False
        print(f"  ✅ {name} OTP[{hex(addr)}] = 0x{val:08X} 完成")
    return True


def burn_trim_words(vtrim1_word, vtrm2_word, info_words=None):
    """
    将 VTRIM1/VTRM2 及 ADCTRM/ANACINFO0~3 (值待定, 默认 0) 烧写到空白 OTP 区域，
    并记录到 Excel。

    参数:
        vtrim1_word / vtrm2_word: 打包后的 ANAC_TRM1/TRM2 32 位值
        info_words: 可选 dict { "ADCTRM": 0x..., "ANACINFO0": 0x..., ... }。
            缺省 key 写 0; 固定烧写区域全部 8 个 word (见 write_slot)。

    ★ 2026-08 槽位判定 + 容错:
      - 槽位判定: 顺序回读 4 个槽位起始 TAG 地址 (0x8000/0x8008/0x8010/0x8018),
        读回 == ANA_TAG(0x00796783) 说明该槽位已烧写过。
      - 容错: 烧写失败(如 PGERR=已烧过/编程错误)不终止, **继续顺序尝试下一个
        空白槽位**, 直到成功烧写到某一个或全部槽位都尝试完。因此即便槽位1
        已烧过, 也能自动接着烧槽位2/3/4。
      - 全新芯片 4 槽全空白, 从槽位1开始尝试。

    主流程:
        1. 顺序回读 4 个槽位 TAG → 列出所有空白槽位
        2. 对每个空白槽位 (按槽位1→2→3→4): 操作员确认后烧写
           (TAG+VTRIM1+VTRM2+ADCTRM+ANACINFO0~3, PGERR/TO 硬错误校验); 失败则继续下一个
        3. 第一个烧写成功的槽位即本次结果; 仅它追加烧写记录 (UID 记 "无编号")
        4. 提示用 reboot_and_verify() 复位后读影子寄存器确认生效

    返回: (是否成功, 本次槽位号 or None, 已烧写次数 or None)
    """
    from chips.OCD5010.trim.program_records import append_burn_record

    chip_uid = "无编号"
    info = info_words or {}

    # ── 1. 顺序回读 4 个槽位 TAG, 找出空白槽位 ──
    status = read_otp_status()
    print("\n  【槽位判定】顺序回读 4 个槽位起始 TAG 地址 (== ANA_TAG 表示已烧写)")
    for s in status['slots']:
        tag_str = "0x%08X" % s['tag_val'] if s['tag_val'] is not None else "None"
        state = "已烧写" if s['used'] else "空白"
        print(f"    槽位{s['slot']} TAG[0x{s['tag_addr']:04X}] = {tag_str}  ({state})")

    # ── 1.5 详细回读: 4 区域全部 32 word 显示在日志上 (诊断用, 便于操作员判断) ──
    readback_all_slots()

    if not status['readable']:
        print("  ❌ SWD 读回异常, 无法判定槽位占用, 终止烧写!")
        return False, None, None

    used_count = status['count']
    # 空白槽位列表 (按槽位顺序), 按序逐个尝试
    blank_slots = [s['slot'] for s in status['slots'] if not s['used']]
    if not blank_slots:
        print(f"  ❌ 4 个槽位 TAG 全部为 ANA_TAG, 已烧满 {MAX_BURN}/{MAX_BURN}, 无法再烧写!")
        return False, None, used_count

    print(f"\n  待烧写槽位 (空白): {blank_slots} —— 将按顺序尝试, 失败自动继续下一个")
    print(f"    VTRIM1 = 0x{vtrim1_word:08X}")
    print(f"    VTRM2  = 0x{vtrm2_word:08X}")
    for name in INFO_WORD_NAMES:
        display = f"0x{info.get(name, 0):08X}"
        if name == "ANACINFO0" and info.get("ANACINFO0"):
            display += f"  (VREF 电压 ≈ {word_to_float(info['ANACINFO0']):.1f} mV)"
        elif info.get(name):
            pass   # 已有实际测量值, 不附加"值待定"提示
        else:
            display += "   (值待定, 缺省 0)"
        print(f"    {name:9s} = {display}")

    # ── 2. 逐个空白槽位尝试烧写 ──
    for slot_no in blank_slots:
        slot = OTP_SLOTS[slot_no - 1]      # slot_no=1→OTP_SLOTS[0] ...
        _, tag_addr, v1_addr, v2_addr, adctrm_addr, i0_addr, i1_addr, i2_addr, i3_addr = slot
        print(f"\n  ⚠️ OTP 一次性可编程, 请确认本次烧写到槽位 {slot_no}:")
        print(f"     TAG={hex(tag_addr)}, VTRIM1={hex(v1_addr)}, VTRM2={hex(v2_addr)}, "
              f"ADCTRM={hex(adctrm_addr)}, ANACINFO0~3={hex(i0_addr)}~{hex(i3_addr)}")
        try:
            resp = input("     确认写入? (输入 yes 继续, 其他任意键跳过此槽位): ").strip()
        except (EOFError, KeyboardInterrupt):
            resp = ""
        if resp.lower() != "yes":
            print(f"  ⏭️  操作员跳过槽位 {slot_no} (未写入, 不记录 Excel)")
            continue

        # 烧写 (仅烧写成功才写入 Excel 记录)
        ok = write_slot(slot, vtrim1_word, vtrm2_word, info_words=info_words)
        if ok:
            burn_no = slot_no
            append_burn_record(chip_uid, "PASS", burn_no, burn_no, slot,
                               vtrim1_word, vtrm2_word, info_words=info_words)
            print(f"\n  ✅ 槽位 {slot_no} 烧写完成! 已写入 {slot_no}/{MAX_BURN} 个槽位")
            print("     记录已写入 chip_program_records.xlsx (仅归档, 槽位判定不再以它为准)")
            # 可选: 立即 reboot 并读影子寄存器 (权威回读: OTP 加载到 ANAC_TRM1/2 等)
            try:
                resp_verify = input("     是否现在 reboot 并读影子寄存器验证? (y/N): ").strip()
            except (EOFError, KeyboardInterrupt):
                resp_verify = ""
            if resp_verify.lower() in ("y", "yes"):
                reboot_and_verify(vtrim1_word, vtrm2_word, info_words=info_words)
            else:
                print("  ⚠️ 请 reboot (断电重上电或 reboot_and_verify) 后读影子寄存器确认生效")
            return True, slot_no, burn_no
        else:
            print(f"\n  ❌ 槽位 {slot_no} 烧写失败, 继续尝试下一个空白槽位...")

    # ── 3. 所有空白槽位都失败/跳过 ──
    print(f"\n  ❌ 全部空白槽位 {blank_slots} 均未烧写成功 (已烧过/跳过/编程错误)")
    return False, None, used_count


def reboot_and_verify(vtrim1_word, vtrm2_word, info_words=None):
    """
    烧写后 reboot 使 OTP 生效, 并读影子寄存器比对验证。

    官方验证流程 (Roadtest_A13_FLASH_5010): 烧写 → 复位 → 读影子寄存器。
    复位通过 SWD 的 swd_reset() (NVIC AIRCR SYSRESETREQ) 触发, 无需知道
    RCC->RSTKEY 地址; reboot 后重新 init SWD 再读影子寄存器。

    验证对象 (VTRIM1/2 + ANACINFO0~3; ★ ADCTRM 回读地址待确认, 只写不验):
      VTRIM1    → ANAC_TRM1   (0x40007C20)
      VTRM2     → ANAC_TRM2   (0x40007C24)
      ANACINFO0 → ANAC INFO_0 (0x40007C28)
      ANACINFO1 → ANAC INFO_1 (0x40007C2C)
      ANACINFO2 → ANAC INFO_2 (0x40007C30)
      ANACINFO3 → ANAC INFO_3 (0x40007C34)

    参数:
        vtrim1_word / vtrm2_word: 打包后的 ANAC_TRM1/TRM2 32 位值
        info_words: 可选 dict; 用于给出 ANACINFO0~3 的期望值 (缺省 0)
    返回: (是否全部一致, 各寄存器读出值 dict)
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
            return False, None
    except Exception as e:
        print(f"  ⚠️ SWD 复位异常: {e}, 请手动断电重上电后运行 verify")
        return False, None

    time.sleep(0.5)
    SWD_CMD.init_all()
    time.sleep(0.5)

    info = info_words or {}
    reads = {}
    ok = True

    # VTRIM1/VTRM2
    val1 = SWD_CMD.read_register_32bit(ANAC_TRM1_ADDR)
    val2 = SWD_CMD.read_register_32bit(ANAC_TRM2_ADDR)
    reads["ANAC_TRM1"] = val1
    reads["ANAC_TRM2"] = val2
    ok1 = (val1 == vtrim1_word) and (val2 == vtrm2_word)
    ok = ok and ok1
    print(f"  ANAC_TRM1 = {hex(val1) if val1 is not None else 'None'} (期望 {hex(vtrim1_word)})")
    print(f"  ANAC_TRM2 = {hex(val2) if val2 is not None else 'None'} (期望 {hex(vtrm2_word)})")

    # ANACINFO0~3 (ADCTRM 回读地址待确认, 只写不验)
    for name, shadow_addr in INFO_SHADOW_ADDRS.items():
        expected = info.get(name, 0)
        val = SWD_CMD.read_register_32bit(shadow_addr)
        reads[name] = val
        match = (val == expected)
        ok = ok and match
        mark = "✅" if match else "❌"
        extra = ""
        if name == "ANACINFO0" and val is not None:
            extra = f"   (VREF ≈ {word_to_float(val):.1f} mV)"
        print(f"  {name:10s} [{hex(shadow_addr)}] = {hex(val) if val is not None else 'None'} "
              f"(期望 {hex(expected)}) {mark}{extra}")

    if ok:
        print("  ✅ 影子寄存器与写入值一致, 烧写生效确认!")
    else:
        print("  ⚠️ 影子寄存器与写入值不一致 (可能未 reboot, 或 reboot 后 SWD 未重连)")
    return ok, reads


if __name__ == "__main__":
    # 自测: 仅演示判定逻辑 (需要真实 SWD 连接才能读 OTP)
    print("此模块依赖 SWD 连接，请通过 program_trim.py / main.py 调用。")
    print("4 个区域 (每区 8 word): " + ", ".join(
        f"区域{n}(TAG={hex(t)})" for n, t, *_ in OTP_SLOTS))
