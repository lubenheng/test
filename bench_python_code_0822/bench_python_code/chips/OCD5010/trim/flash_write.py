import time

# 统一使用 instrument.debug 包路径导入，与 instrument_set.swd_init() 共享同一模块实例
from instrument.debug import SWD_CMD

# =====================================================================
# TRIM 修调值映射
# 参考: chips/ocd2810/trim/trim.py —— OCD2810使用 Info Flash + JFlash 写入，
#       OCD5010使用 SWD 写入 OTP 槽位 (每颗芯片 4 次烧写机会)，原理相同：
#       将各修调字段按位打包成 32 位字后写入。
#
# 对应芯片寄存器 ANAC_TRM1 (0x8000) 和 ANAC_TRM2 (0x8004) 的字段定义。
# 打包字中的 bit 位置与寄存器完全一致；烧写时写入槽位的 VTRIM1/VTRM2 word。
# OTP 区域 0x8000~0x803F, 当 OTPAR=0 时对应 OTP 地址 0x8000 (参考《FLASH信息表》)。
# 4 个烧写槽位及 TAG 判定逻辑见 otp_slots.py。
# =====================================================================
# ANAC_TRM1 (0x8000):
#   [31:26] RESERVED     | [25:21] CSA_OS_VTRM  | [20:18] ADC_VREFH_VTRM
#   [17:14] SLP_REF_VTRM | [13:12] SLP12_VTRM   | [11:9]  LDO12_VTRM
#   [8:5]   REF_ITRM     | [4:0]   REF_VTRM
#
# ANAC_TRM2 (0x8004):
#   [31]    RESERVED      | [30:25] BG_TTRM  | [24:19] TS_ITRM
#   [18:14] TJ_TTRM       | [13:7]  LSI_FTRM | [6:0]   HSI_FTRM
# =====================================================================
TRIM_FIELDS = {
    # ---- ANAC_TRM1 (0x8000) ----
    "REF_VTRM":       (0x8000, 0,  5),    # bit0-4:   参考电压微调
    "REF_ITRM":       (0x8000, 5,  4),    # bit5-8:   参考电流微调
    "LDO12_VTRM":     (0x8000, 9,  3),    # bit9-11:  LDO12电压微调
    "SLP12_VTRM":     (0x8000, 12, 2),    # bit12-13: 休眠LDO12电压微调
    "SLP_REF_VTRM":   (0x8000, 14, 4),    # bit14-17: 休眠参考电压微调
    "ADC_VREFH_VTRM": (0x8000, 18, 3),    # bit18-20: ADC高参考电压微调
    "CSA_OS_VTRM":    (0x8000, 21, 5),    # bit21-25: CSA失调电压微调
    # ---- ANAC_TRM2 (0x8004) ----
    "HSI_FTRM":       (0x8004, 0,  7),    # bit0-6:   HSI频率微调
    "LSI_FTRM":       (0x8004, 7,  7),    # bit7-13:  LSI频率微调
    "TJ_TTRM":        (0x8004, 14, 5),    # bit14-18: 温度传感器温度微调
    "TS_ITRM":        (0x8004, 19, 6),    # bit19-24: 温度传感器电流微调
    "BG_TTRM":        (0x8004, 25, 6),    # bit25-30: 带隙基准温度微调
}

ADDR_NAMES = {0x8000: "ANAC_TRM1", 0x8004: "ANAC_TRM2"}


def flash_main_write(addr, data):
    print(f"写入 {hex(data)} 到 {hex(addr)}")
    time.sleep(0.1)

    SWD_CMD.write_register(0x40020058, 1)
    time.sleep(0.001)

    #第一步，校验地址是否在0x0 - 0x01FFDC范围内
    if addr < 0x0 or addr > 0x01FFDC:
        print("地址超出范围！")
        return False
    #第二步，解锁
    SWD_CMD.write_register(0x40022008, 0x45670123)
    SWD_CMD.write_register(0X40022008, 0xCDEF89AB)
    time.sleep(0.1)
    #第三步，向CR寄存器写入1进入写操作
    SWD_CMD.write_register(0X40022018, 0X01)
    time.sleep(0.1)
    #第四步，写入数据
    SWD_CMD.write_register(0x08000000 + addr, data)
    time.sleep(0.1)

    #第五步，加锁
    SWD_CMD.write_register(0x40022008, 0x0)
    time.sleep(0.1)

    #关闭时钟
    SWD_CMD.write_register(0x40020058, 0)
    time.sleep(0.001)

    #回读数据，查看是否一致
    print(f"回读地址:{hex(addr)}，数据：{hex(SWD_CMD.read_register_32bit(addr))}")


def build_trim_word(trim_values, addr):
    """
    将指定 Flash 地址的所有修调字段组装成 32 位值。

    参考: chips/ocd2810/trim/trim.py 的 build_flash_data()
    OCD2810 将各修调值按位打包到 256 字节的 Info Flash 数据块中；
    此处简化为将同一地址的修调字段打包到一个 32 位字中。

    参数:
        trim_values: { "REF_VTRM": 12, "HSI_FTRM": 34, ... }
        addr: 0x8000 或 0x8004

    返回: 组装好的 32 位整数值
    """
    word = 0
    for name, (field_addr, bit_start, bit_len) in TRIM_FIELDS.items():
        if field_addr != addr:
            continue
        if name not in trim_values:
            continue

        v = trim_values[name]
        max_val = (1 << bit_len) - 1
        if v < 0 or v > max_val:
            print(f"  ⚠️ {name} 值 {v} 超出 {bit_len} 位范围 (0~{max_val}), 跳过")
            continue

        mask = ((1 << bit_len) - 1) << bit_start
        word = (word & ~mask) | ((v & ((1 << bit_len) - 1)) << bit_start)
        print(f"    {name:20s} = {v:3d} (0x{v:02X}) → {ADDR_NAMES.get(addr, hex(addr))} bit{bit_start}~{bit_start+bit_len-1}")

    return word


def flash_write_trims(trim_values):
    """
    将各修调项按位打包后烧写到下一个可用 OTP 槽位 (TAG + VTRIM1 + VTRM2)。

    参考 OCD2810 的 program_trim.py 流程:
      OCD2810: 读Excel → build_flash_data() → JFlash loadbin 写入 Info Flash
      OCD5010: 传入修调值 → build_trim_word() → burn_trim_words() 烧写 OTP 槽位

    OTP 4 个烧写槽位 (参考《FLASH信息表》/decoded_flash_intro.txt):
      槽位1: TAG=0x8000  VTRIM1=0x8001  VTRM2=0x8002
      槽位2: TAG=0x8008  VTRIM1=0x8009  VTRM2=0x800A
      槽位3: TAG=0x8010  VTRIM1=0x8011  VTRM2=0x8012
      槽位4: TAG=0x8018  VTRIM1=0x8019  VTRM2=0x801A
    每个槽位起始 TAG 写入 ANA_TAG=0x00796783，boot 才能识别并加载该槽位修调值。
    烧写前自动读取 4 个槽位 TAG 判断已烧写次数，按顺序选择下一可用槽位。

    参数:
        trim_values: {
            "REF_VTRM": 12,       # ANAC_TRM1 字段
            "REF_ITRM": 8,
            "LDO12_VTRM": 3,
            "SLP12_VTRM": 2,
            "SLP_REF_VTRM": 7,
            "ADC_VREFH_VTRM": 4,
            "CSA_OS_VTRM": 15,
            "HSI_FTRM": 50,       # ANAC_TRM2 字段
            "LSI_FTRM": 60,
            "TJ_TTRM": 20,
            "TS_ITRM": 30,
            "BG_TTRM": 40,
        }
        只需要传入需要写入的字段即可，缺失的字段保持为 0。

    返回: True 表示成功
    """
    print("=" * 60)
    print("  OCD5010 TRIM 修调值写入 OTP 槽位")
    print("=" * 60)

    # 按逻辑地址打包
    vtrim1 = build_trim_word(trim_values, 0x8000)   # ANAC_TRM1
    vtrm2  = build_trim_word(trim_values, 0x8004)   # ANAC_TRM2
    print(f"  ANAC_TRM1 (VTRIM1) = 0x{vtrim1:08X}")
    print(f"  ANAC_TRM2 (VTRM2)  = 0x{vtrm2:08X}")

    # 初始化 SWD（如尚未初始化）
    if SWD_CMD.read_register_32bit(0x40020058) is None:
        print("\n  SWD 未初始化，执行初始化...")
        SWD_CMD.init_all()
        time.sleep(0.2)

    # 烧写到下一个可用槽位 (自动判定烧写次数 + 记录 Excel)
    from chips.OCD5010.trim.otp_slots import burn_trim_words
    ok, slot_no, count = burn_trim_words(vtrim1, vtrm2)

    print(f"\n  {'=' * 60}")
    if ok:
        print(f"  ✅ TRIM 修调值写入 OTP 槽位 {slot_no} 完成！掉电不丢失。")
        print(f"     该芯片已烧写 {count}/4 次，复位后自动加载该槽位修调值到 ANAC_TRM1/ANAC_TRM2")
    else:
        print("  ❌ TRIM 修调值写入 OTP 失败")
    print(f"  {'=' * 60}")
    return ok


def flash_otp_write(addr, data):
    """
    向 OTP 区域写入一个 32 位字。

    参考《FLASH信息表》/decoded_flash_intro.txt:
      OTP_FLASH: 64 Word, 地址 0x8000~0x803F
      当 OTPAR=0 时对应 OTP 地址 0x8000, 即 OTPAR = 地址 - 0x8000

    写入流程: 解锁(OTPKEY) → OTPAR写地址 → OTPDR写数据 → OTPCR.bit0=1 触发编程
    (每次写都需要 OTPCR 置1)。写完 reBoot 后修调值才会更新到对应寄存器。

    参数:
        addr: OTP 地址, 范围 0x8000~0x803F (如 0x8001/0x8002)
        data: 32 位修调值
    """
    print(f"写入 {hex(data)} 到 OTP[{hex(addr)}]")

    if addr < 0x8000 or addr > 0x803F:
        print("地址超出范围！(0x8000~0x803F)")
        return False

    # ★ 第0步：使能 FLASH 时钟（非常重要！）
    SWD_CMD.write_register(0x40020058, 1)
    time.sleep(0.001)

    # 第2步：解锁OTP（写正确的OTPKEY）
    SWD_CMD.write_register(0x4002204C, 0xA1B2C3D4)
    SWD_CMD.write_register(0x4002204C, 0x5D6C7B8A)

    # 检查是否解锁成功 - 检查 CR[18]=OPTLOCK 而不是 OTPCR[16]
    cr_val = SWD_CMD.read_register_32bit(0x40022018)
    if cr_val & (1 << 18):   # CR[18] = OPTLOCK
        print("OTP 解锁失败！")
        return False

    # 第3步：OTPAR写入地址 (OTPAR = OTP地址 - 0x8000)
    SWD_CMD.write_register(0x40022044, addr - 0x8000)

    # 第4步：OTPDR写入数据
    SWD_CMD.write_register(0x40022048, data)

    # 第5步：OTPCR.OTPPG写1启动编程
    SWD_CMD.write_register(0x40022040, 0x01)

    # 第6步：等待编程完成（检查SR[0]=RDYF 或 SR[16]=EOP）
    timeout_ms = 50
    while timeout_ms > 0:
        sr_val = SWD_CMD.read_register_32bit(0x40022010)
        if sr_val is None:
            print("读取 FLASH SR 失败！")
            return False
        if sr_val & 0x01:        # RDYF=1 表示就绪
            break
        time.sleep(0.001)
        timeout_ms -= 1

    if timeout_ms <= 0:
        print("OTP 编程超时！")
        return False

    # 检查编程错误 SR[1]=ERRF 或 SR[17]=PGERR
    sr_val = SWD_CMD.read_register_32bit(0x40022010)
    if sr_val is not None and (sr_val & 0x02):   # ERRF
        print(f"OTP 编程出错！SR=0x{sr_val:08X}")
        return False

    # 第7步：加锁
    SWD_CMD.write_register(0x4002204C, 0x0)

    #关闭时钟
    SWD_CMD.write_register(0x40020058, 0)
    time.sleep(0.001)

    # 回读（通过OTPDR读，而非0x8000+addr）
    SWD_CMD.write_register(0x40022044, addr - 0x8000)
    readback = SWD_CMD.read_register_32bit(0x40022048)
    #print("Flash的OTP区域写入数据之后需要复位一下")

    return True


def flash_otp_read(addr):
    """
    读取 OTP 区域一个 32 位字 (0x8000~0x803F)。

    ⚠️ 关键: OTP 默认处于锁定状态 (CR[18]=OPTLOCK), 必须先用 OTPKEY 解锁
    才能读到真实内容; 未解锁时读 OTPDR 恒返回 0x0 (导致判定烧写次数/空白检查全部失效)。
    参考 flash_hal.c 的 _flash_otp_unlock() 流程。

    通过 OTPAR 选地址 + OTPDR 读取 (与 flash_otp_write 的回读方式一致)。
    用于: 烧写前判断 4 个槽位 TAG 是否已写入 / 写后回读校验 / 读取芯片 UUID。

    参数:
        addr: OTP 地址, 范围 0x8000~0x803F

    返回: 32 位值; 读取失败返回 None
    """
    if addr < 0x8000 or addr > 0x803F:
        print(f"OTP 地址超出范围！(0x8000~0x803F): {hex(addr)}")
        return None

    # 使能 FLASH 时钟
    SWD_CMD.write_register(0x40020058, 1)
    time.sleep(0.001)

    # 解锁 OTP (OTPKEY) —— 未解锁时读 OTPDR 恒为 0
    SWD_CMD.write_register(0x4002204C, 0xA1B2C3D4)
    SWD_CMD.write_register(0x4002204C, 0x5D6C7B8A)
    time.sleep(0.001)
    cr_val = SWD_CMD.read_register_32bit(0x40022018)
    if cr_val is not None and (cr_val & (1 << 18)):
        print(f"  ⚠️ OTP 解锁失败 (CR[18]=OPTLOCK 仍为1), 读取结果可能无效: CR=0x{cr_val:08X}")

    # OTPAR 选地址 + 读 OTPDR
    SWD_CMD.write_register(0x40022044, addr - 0x8000)
    time.sleep(0.001)
    val = SWD_CMD.read_register_32bit(0x40022048)       # OTPDR

    # 加锁 + 关闭时钟
    SWD_CMD.write_register(0x4002204C, 0x0)
    SWD_CMD.write_register(0x40020058, 0)
    return val


if __name__ == "__main__":
    SWD_CMD.init_all()
    time.sleep(0.2)

    # ================================================================
    # 方式一：使用 flash_otp_write 直接写入已组装好的 32 位值 (OTP 地址)
    # ================================================================
    # flash_otp_write(0x8000, 0x00796783)   # 槽位1 TAG
    # flash_otp_write(0x8001, 0x11111111)   # 槽位1 VTRIM1
    # flash_otp_write(0x8002, 0x22222222)   # 槽位1 VTRM2

    # ================================================================
    # 方式二：使用 flash_write_trims 按修调字段名烧写（推荐）
    #
    # 参考 OCD2810 的方式：
    #   OCD2810: 各修调值 → build_flash_data() → JFlash loadbin
    #   OCD5010: 各修调值 → build_trim_word()  → burn_trim_words (自动选槽位)
    #
    # 只需传入要通过 PASS 的修调项，缺失的字段将保持为 0。
    # ⚠️ OTP 只能烧写一次，烧写后无法更改！
    # ================================================================
    trim_values = {
        # ANAC_TRM1 (0x8000) 各修调值
        "REF_VTRM":       0,   # bit0-4:   参考电压微调 (0~31)
        "REF_ITRM":       0,   # bit5-8:   参考电流微调 (0~15)
        "LDO12_VTRM":     0,   # bit9-11:  LDO12电压微调 (0~7)
        "SLP12_VTRM":     0,   # bit12-13: 休眠LDO12电压微调 (0~3)
        "SLP_REF_VTRM":   0,   # bit14-17: 休眠参考电压微调 (0~15)
        "ADC_VREFH_VTRM": 0,   # bit18-20: ADC高参考电压微调 (0~7)
        "CSA_OS_VTRM":    0,   # bit21-25: CSA失调电压微调 (0~31)

        # ANAC_TRM2 (0x8004) 各修调值
        "HSI_FTRM":       0,   # bit0-6:   HSI频率微调 (0~127)
        "LSI_FTRM":       0,   # bit7-13:  LSI频率微调 (0~127)
        "TJ_TTRM":        0,   # bit14-18: 温度传感器温度微调 (0~31)
        "TS_ITRM":        0,   # bit19-24: 温度传感器电流微调 (0~63)
        "BG_TTRM":        0,   # bit25-30: 带隙基准温度微调 (0~63)
    }
    print("\n⚠️  警告: OTP 只能烧写一次, 烧写后无法更改!")
    confirm = input("确定要烧写当前修调值到 OTP 吗? (y/N): ").strip().lower()
    if confirm == "y":
        flash_write_trims(trim_values)
    else:
        print("已取消, 未写入 OTP。")
    print("Flash的OTP区域写入数据之后需要复位一下")
