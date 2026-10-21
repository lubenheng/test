import time

# 统一使用 instrument.debug 包路径导入，与 instrument_set.swd_init() 共享同一模块实例
from instrument.debug import SWD_CMD

# =====================================================================
# TRIM 修调值映射
# 参考: chips/ocd2810/trim/trim.py —— OCD2810使用 Info Flash + JFlash 写入，
#       OCD5010使用 SWD 写入 OTP 区域 (每颗芯片 4 次烧写机会)，原理相同：
#       将各修调字段按位打包成 32 位字后写入。
#
# 对应芯片寄存器 ANAC_TRM1 (0x8000) 和 ANAC_TRM2 (0x8004) 的字段定义。
# 打包字中的 bit 位置与寄存器完全一致；烧写时写入区域的 VTRIM1/VTRM2 word。
# OTP 区域 0x8000~0x803F, 当 OTPAR=0 时对应 OTP 地址 0x8000 (参考《FLASH信息表》)。
# 4 个烧写区域 (每区 8 word) 及 TAG 判定逻辑见 otp_slots.py。
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

# =====================================================================
# FLASH 控制器寄存器 (基地址 0x4002_2000, 参考 FLC_RegInfo_Errata 勘误表 v0.0.2)
#   FLC_OTPCR : [16]=OTPLOCK(OTP编程锁, rw 软件位), [0]=OTPPG(OTP编程触发, wo)
#   FLC_OTPAR : [6:0]=ADDR(OTP区内单元偏移, 0x8000→0)
#   FLC_OTPDR : [31:0]=OTPDATA (OTP 写数据, rw = 写锁存)
#   FLC_DR    : [31:0]=OTPDR 的只读视图 —— ★ 写锁存, 不是 OTP 阵列回读!
#   FLC_OTPKEYR: 依次写 0xA1B2C3D4 / 0x5D6C7B8A 解锁 OTPLOCK(实测有效)
#   FLC_CR[24]=OPTLOCK / FLC_KEYR / FLC_OPTKEYR: 属于 INFO/OPT 区(0x1FFF_F800),
#     与 OTP 区(0x8000)无关, OTP 写流程不需要也不应动 CR。
# ★ OTP 空白默认 = 0x00000000 (0→1 编程, 反熔丝型), 不是 0xFFFFFFFF!
#   回读只能 reboot 后读影子寄存器 (ANAC_TRM1/2/ADCTRM/ANACINFO/SYSCFG), 无寄存器级读回。
# =====================================================================
FLC_AHBCKE    = 0x40020058   # AHBCKE[0]=FLCCKE 使能 FLASH 控制器时钟
FLC_SR        = 0x40022010   # 状态: [0]RDYF, [1]ERRF, [16]EOP, [17]PGERR, [18]ECCERR, [19]TO (rw1c)
FLC_CR        = 0x40022018   # 控制: [0]PG, [16]LOCK, [24]OPTLOCK —— 属 INFO/OPT 区锁, 与 OTP 区无关
FLC_KEYR      = 0x40022008   # 主 flash 解锁密钥 (WO): 0x45670123 / 0xCDEF89AB (INFO/OPT 区用)
FLC_OPTKEYR   = 0x4002200C   # OPT 锁(CR[24]=OPTLOCK)解锁密钥 (WO)
FLC_DR        = 0x40022024   # ★ DR = OTPDR(0x48) 写数据锁存的只读视图, 不是 OTP 阵列回读!
FLC_OTPCR     = 0x40022040   # OTP 控制: [16]OTPLOCK(rw), [0]OTPPG(wo)
FLC_OTPAR     = 0x40022044   # OTP 地址: [9:0]ADDR (偏移 = OTP地址 - 0x8000)
FLC_OTPDR     = 0x40022048   # OTP 写数据 (rw) —— 写锁存, 读 DR 即读它
FLC_OTPKEYR   = 0x4002204C   # OTP 解锁密钥 (WO): 0xA1B2C3D4 / 0x5D6C7B8A (实测可清 OTPCR[16])
OTP_UNLOCK_KEY1 = 0xA1B2C3D4
OTP_UNLOCK_KEY2 = 0x5D6C7B8A

# =====================================================================
# SWD 寄存器操作延时 (秒) —— 2026-08 烧写偶发失败, 疑似执行太快:
#   OTP 编程/解锁是硬件操作, SWD 写太快或编程未完成就读 SR 会偶发失败。
#   若仍偶发失败, 把 DELAY_PROGRAM / DELAY_UNLOCK_KEY 等值再调大。
# =====================================================================
DELAY_AFTER_CLK     = 0.010   # 使能 FLASH 时钟后
DELAY_UNLOCK_KEY    = 0.010   # OTPKEYR 每写一个密钥后 (解锁序列须有间隔)
DELAY_AFTER_UNLOCK  = 0.010   # 解锁完成后
DELAY_REG_WRITE     = 0.010   # OTPAR/OTPDR/OTPCR 等普通寄存器写后
DELAY_PROGRAM       = 0.050   # 触发 OTPPG 后等待编程完成 (原 0.01 偶发失败)
DELAY_BEFORE_RETRY  = 0.300   # 首次编程硬错误(PGERR)后重试前的等待 (电荷泵/编程电压稳定)
DELAY_AFTER_RELOCK  = 0.010   # 重新锁定后
OTP_PROGRAM_ATTEMPTS = 2      # 同地址同数据最大编程尝试次数 (首次失败自动重试一次)


def _otp_unlock(quiet=False):
    """
    解锁 OTP 编程 (仅清 OTPCR[16]=OTPLOCK)。

    ★ 2026-08-22 源码 + 解码文档确认: OTP 区 (0x8000~0x803F) 的编程锁只有
      OTPCR[16]=OTPLOCK 一个, 由 FLC_OTPKEYR(0x4C) 密钥解锁:
        依次写 0xA1B2C3D4 / 0x5D6C7B8A, 成功 → OTPCR[16]=0 (实测有效)。
      官方写流程 (flash_hal.c) 从不动 CR。CR[24]=OPTLOCK 是 INFO/OPT 区
      (0x1FFF_F800) 的锁, 与 OTP 区无关, 无需也不应去清它。

    参数:
        quiet: True 时不打印解锁过程 (批量回读 12 个 word 时避免刷屏)
    返回: True=OTP 已解锁 (OTPCR[16]=0, 可编程); False=密钥不对/通信异常
    """
    # 1. OTPKEY 解锁: 清 OTPCR[16]=OTPLOCK (实测有效)
    #    两个密钥之间要有间隔 (解锁序列太快可能不生效, 导致偶发失败)
    SWD_CMD.write_register(FLC_OTPKEYR, OTP_UNLOCK_KEY1)
    time.sleep(DELAY_UNLOCK_KEY)
    SWD_CMD.write_register(FLC_OTPKEYR, OTP_UNLOCK_KEY2)
    time.sleep(DELAY_AFTER_UNLOCK)

    cr = SWD_CMD.read_register_32bit(FLC_CR)
    otpcr = SWD_CMD.read_register_32bit(FLC_OTPCR)
    if otpcr is None:
        if not quiet:
            print("无法读取 OTPCR, 检查 SWD 连接!")
        return False

    if cr is not None:
        cr_str = f"0x{cr:08X}"
        cr_optlock = 1 if (cr & (1 << 24)) else 0
        cr_lock = 1 if (cr & (1 << 16)) else 0
    else:
        cr_str, cr_optlock, cr_lock = "None", "?", "?"
    otpcr_lock = 1 if (otpcr & (1 << 16)) else 0
    if not quiet:
        print(f"  [解锁] CR={cr_str} (OPTLOCK[24]={cr_optlock}, LOCK[16]={cr_lock} —— 为 INFO/OPT 区锁, 与 OTP 无关), "
              f"OTPCR=0x{otpcr:08X} (OTPLOCK[16]={otpcr_lock})")

    if otpcr_lock:
        if not quiet:
            print(f"  ❌ OTPCR[16]=OTPLOCK 仍为 1 → OTP 解锁失败! 密钥可能不对 (0xA1B2C3D4/0x5D6C7B8A)")
        return False
    return True


def flash_main_write(addr, data):
    print(f"写入 {hex(data)} 到 {hex(addr)}")
    time.sleep(0.1)

    SWD_CMD.modify_register_range(0x40020058, 1, 0, 0)
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
    SWD_CMD.modify_register_range(0x40020058, 0, 0, 0)
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


def flash_write_trims(trim_values, info_words=None):
    """
    将各修调项按位打包后烧写到下一个可用 OTP 区域 (TAG + VTRIM1 + VTRM2 + ADCTRM + ANACINFO0~3)。

    参考 OCD2810 的 program_trim.py 流程:
      OCD2810: 读Excel → build_flash_data() → JFlash loadbin 写入 Info Flash
      OCD5010: 传入修调值 → build_trim_word() → burn_trim_words() 烧写 OTP 区域

    OTP 4 个烧写区域 (每个区域 8 word, 参考《FLASH信息表》/decoded_flash_intro.txt):
      区域1: TAG=0x8000  VTRIM1=0x8001  VTRM2=0x8002  ADCTRM=0x8003  ANACINFO0~3=0x8004~0x8007
      区域2: TAG=0x8008  VTRIM1=0x8009  VTRM2=0x800A  ADCTRM=0x800B  ANACINFO0~3=0x800C~0x800F
      区域3: TAG=0x8010  VTRIM1=0x8011  VTRM2=0x8012  ADCTRM=0x8013  ANACINFO0~3=0x8014~0x8017
      区域4: TAG=0x8018  VTRIM1=0x8019  VTRM2=0x801A  ADCTRM=0x801B  ANACINFO0~3=0x801C~0x801F
    每个区域起始 TAG 写入 ANA_TAG=0x00796783，boot 才能识别并加载该区域修调值。
    烧写前自动读取 4 个区域 TAG 判断已烧写次数，按顺序选择下一可用区域。

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
        info_words: 可选 dict { "ADCTRM": 0x..., "ANACINFO0": 0x..., ... }。
            ★ 2026-09: 区域固定烧写 8 word, 新增 ADCTRM/ANACINFO0~3 值待定,
            缺省写 0 (0→1 编程, 写 0 为无操作不占熔丝)。

    返回: True 表示成功
    """
    print("=" * 60)
    print("  OCD5010 TRIM 修调值写入 OTP 区域")
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

    # 烧写到下一个可用区域 (自动判定烧写次数 + 记录 Excel)
    from chips.OCD5010.trim.otp_slots import burn_trim_words
    ok, slot_no, count = burn_trim_words(vtrim1, vtrm2, info_words=info_words)

    print(f"\n  {'=' * 60}")
    if ok:
        print(f"  ✅ TRIM 修调值写入 OTP 区域 {slot_no} 完成！掉电不丢失。")
        print(f"     该芯片已烧写 {count}/4 次，复位后自动加载该区域修调值到影子寄存器")
    else:
        print("  ❌ TRIM 修调值写入 OTP 失败")
    print(f"  {'=' * 60}")
    return ok


def flash_otp_write(addr, data):
    """
    向 OTP 区域写入一个 32 位字 (0x8000~0x803F)。

    流程 (对照 flash_hal.c 官方写流程 + decoded_flash_intro):
        OTPKEYR解锁 → OTPAR选地址 → OTPDR写数据 → OTPCR.OTPPG=1 触发编程 →
        短延时 → 查 SR。★ 官方流程不动 CR (CR[24]=OPTLOCK 是 INFO/OPT 区锁,
        与 OTP 区无关); 官方也不查 SR, 最终以 reboot 读影子寄存器验证。

    要点 (2026-08 源码 + 实测确认):
      - 编程前只须清 OTPCR[16]=OTPLOCK (写 OTPKEYR, 实测有效); CR 无需动。
      - ★ DR(0x24) 只是 OTPDR(0x48) 的写锁存只读视图, 不是 OTP 阵列回读!
        SDK 的 OTP 读路径是空的 (flash_hal.c OTP read 分支为空), 官方验证 =
        reboot 后读影子寄存器 (ANAC_TRM1/2 等, 见 otp_slots.reboot_and_verify)。
      - SR[17]=PGERR / [19]=TO 是硬错误; [18]=ECCERR 按良性处理
        (官方 flash_hal.c 不查 SR, 直接信任写入并通过 reboot 影子验证)。

    参数:
        addr: OTP 逻辑地址 0x8000~0x803F (如 0x8001/0x8002)
        data: 32 位修调值
    返回: True=未出现硬错误 (视为已编程, 最终以 reboot 影子验证); False=失败
    """
    if addr < 0x8000 or addr > 0x803F:
        print("地址超出范围！(0x8000~0x803F)")
        return False

    print(f"写入 {hex(data)} 到 OTP[{hex(addr)}]")

    # 第0步: 使能 FLASH 时钟 (AHBCKE[0]=FLCCKE)
    SWD_CMD.modify_register_range(FLC_AHBCKE, 1, 0, 0)
    time.sleep(DELAY_AFTER_CLK)

    # 第1步: 解锁 OTP (密钥正确则 OTPLOCK 清零; 密钥错会在此处告警)
    if not _otp_unlock():
        return False

    # 第2步~第7步: 编程 (首次硬错误自动重试一次)
    # ★ 2026-08-24 实测: 全新芯片**第一次**编程 (0→1 烧断熔丝) 常因内部编程电压/
    #   电荷泵尚未稳定而报 PGERR[17], 之后同一芯片的后续写入全部正常。故遇到硬错误
    #   时同地址同数据重试一次。OTP 重试只会把 0 再写成 1, 目标值不变, 安全。
    max_attempts = OTP_PROGRAM_ATTEMPTS
    for attempt in range(1, max_attempts + 1):
        # 清除历史 SR 粘滞标志 (EOP/PGERR/ECCERR/TO 是 rw1c, 不清会误报上次结果)
        SWD_CMD.write_register(FLC_SR, 0x000F0000)
        time.sleep(DELAY_REG_WRITE)

        # OTPAR 选写地址 (偏移 = addr - 0x8000, 0x8000→0)
        SWD_CMD.write_register(FLC_OTPAR, addr - 0x8000)
        time.sleep(DELAY_REG_WRITE)

        # OTPDR 写数据 (R/W, 设定要编程的 32 位数据)
        SWD_CMD.write_register(FLC_OTPDR, data)
        time.sleep(DELAY_REG_WRITE)

        # OTPCR.OTPPG = 1 触发编程 (wo; OTPLOCK 不会自动置位)
        SWD_CMD.write_register(FLC_OTPCR, 0x01)

        # 延时等待编程完成 (OTP 编程毫秒级; 原 0.01s 偶发失败 → 放宽到 DELAY_PROGRAM)
        time.sleep(DELAY_PROGRAM)

        # 检查编程状态 SR[16]=EOP / [17]=PGERR / [18]=ECCERR / [19]=TO / [1]=ERRF
        sr_val = SWD_CMD.read_register_32bit(FLC_SR)
        if sr_val is not None:
            print(f"  SR=0x{sr_val:08X} "
                  f"(RDYF[0]={1 if sr_val & 1 else 0}, ERRF[1]={1 if sr_val & 2 else 0}, "
                  f"EOP[16]={1 if sr_val & (1 << 16) else 0}, PGERR[17]={1 if sr_val & (1 << 17) else 0}, "
                  f"ECCERR[18]={1 if sr_val & (1 << 18) else 0}, TO[19]={1 if sr_val & (1 << 19) else 0})")

        # PGERR[17]/TO[19] = 硬性编程错误 → 重试或中止
        if sr_val is not None and (sr_val & ((1 << 17) | (1 << 19))):
            if attempt < max_attempts:
                print(f"  ↻ 第 {attempt} 次编程硬错误 (首次编程电压/电荷泵未稳定所致), "
                      f"延时 {DELAY_BEFORE_RETRY}s 后同地址同数据重试一次...")
                time.sleep(DELAY_BEFORE_RETRY)
                continue
            print(f"OTP 编程硬错误! SR=0x{sr_val:08X} (PGERR/TO), 重试后仍失败")
            return False

        # ECCERR[18]: 官方 flash_hal.c 不查 SR, 直接信任写入并 reboot 验证通过,
        # 故视作良性; 最终以 reboot+影子寄存器确认。
        if sr_val is not None and (sr_val & (1 << 18)):
            print("  ⚠️ SR ECCERR=1 (官方流程不查 SR, 视为良性; 以 reboot 影子验证为准)")
        break  # 无硬错误, 编程成功

    # 第8步: DR 读回仅确认写锁存 (DR 是 OTPDR 锁存的只读视图, 不是 OTP 阵列回读!)
    # ★ 烧写成功判定只看 SR(PGERR/TO)。DR 读回失败/不一致只提示, **不判失败**:
    #   若把 DR 读回失败当硬错误返回 False, burn_trim_words 会误判"烧写失败"
    #   而继续烧下一个槽位, 导致一次运行物理烧写多个槽位 (4 槽全写的根因)!
    #   OTP 是否真正写入, 最终以 reboot 后读影子寄存器 (reboot_and_verify) 为准。
    SWD_CMD.write_register(FLC_OTPAR, addr - 0x8000)
    time.sleep(DELAY_REG_WRITE)
    readback = SWD_CMD.read_register_32bit(FLC_DR)
    if readback is None:
        print("  ⚠️ OTPDR 锁存读回异常 (DR=写锁存, 非阵列回读; 不影响烧写结果, 以 reboot 影子验证为准)")
    elif readback == data:
        print(f"  ℹ️ OTPDR 锁存读回 = 0x{readback:08X}, 与写入值一致 (注意: DR=写锁存, 非阵列回读)")
    else:
        # 位级差异分析: 差异方向可提示"槽位是否已被占用" / "编程是否完全生效"
        keep_bits = readback & ~data     # DR 读回为1、本次写入为0 → 该位此前已烧写为1 (OTP 0→1 不可擦除)
        miss_bits = data & ~readback     # 本次写入为1、DR 读回为0 → 该位本次未编程到位
        print(f"  ⚠️ OTPDR 锁存不一致: 写入 0x{data:08X}, DR=0x{readback:08X} (不影响烧写结果, 以 reboot 影子验证为准)")
        if keep_bits:
            print(f"      · DR 比写入多出 0x{keep_bits:08X}: 这些位之前已烧写为 1 → 该 OTP 单元很可能已被占用"
                  f" (OTP 0→1 只能把 0 写成 1, 不能擦除)")
        if miss_bits:
            print(f"      · DR 比写入缺失 0x{miss_bits:08X}: 这些位本次未编程到位 → 编程未完全生效"
                  f" (SR 无硬错误时通常不影响, 仍以 reboot 影子验证为准)")

    # 第9步: 恢复锁态 + 关闭时钟 (官方 lock = OTPKEY 写 0; 最终以 reboot_and_verify 确认生效)
    SWD_CMD.write_register(FLC_OTPKEYR, 0x0)         # 重新锁定 OTP (官方 _flash_otp_lock)
    time.sleep(DELAY_REG_WRITE)
    SWD_CMD.write_register(FLC_OTPCR, 0x00010000)    # OTPCR[16]=OTPLOCK 置 1
    time.sleep(DELAY_REG_WRITE)
    SWD_CMD.modify_register_range(FLC_AHBCKE, 0, 0, 0)
    time.sleep(DELAY_AFTER_RELOCK)
    return True


def flash_otp_read(addr, quiet=False):
    """
    读 OTP 地址的 FLC_DR (0x24) 值。

    ★ 2026-08 槽位判定改为回读 4 个槽位 TAG (== ANA_TAG 即已烧写), 见
      otp_slots.read_otp_status()。此函数是回读的底层实现。

    ⚠️ 技术疑点 (须实测确认): 早期实测 FLC_DR(0x24) 是 OTPDR(0x48) 的**写锁存**
      只读视图 —— 只写过 0x8000 一处后, 所有地址 (4 槽 TAG + UUID) 的 DR 读回
      全部等于最后写入的 OTPDR 值 (0x00796783)。若该结论成立, 本函数读回的
      不是 OTP 阵列内容, 4 个槽位 TAG 会读到同一个锁存值, 槽位判定会失效。
      请用全新芯片实测 4 个 TAG 读回是否各不相同 / 是否为 0x0 来验证。

    ★ 2026-08-24 实测反例: 一次运行连写 TAG=0x00796783 / VTRIM1=0x021226CE /
      VTRM2=0x53041940 后, 3 次 DR 读回分别 = 0x00796783 / 0x021226CE /
      0x4F041940, **并非同一个锁存值**。说明 DR 在各地址上读回值不同,
      可能真实反映 OTP 阵列内容 (见 otp_slots.readback_all_slots)。

    参数:
        addr: OTP 逻辑地址 0x8000~0x803F
        quiet: True 时不打印解锁过程 (批量回读 12 个 word 时避免刷屏)
    返回: FLC_DR 当前读回值; 读取失败返回 None
    """
    if addr < 0x8000 or addr > 0x803F:
        if not quiet:
            print(f"OTP 地址超出范围！(0x8000~0x803F): {hex(addr)}")
        return None

    # 使能 FLASH 时钟
    SWD_CMD.modify_register_range(FLC_AHBCKE, 1, 0, 0)
    time.sleep(DELAY_AFTER_CLK)

    # 解锁 OTP (读不锁, 失败不致命但告警)
    if not _otp_unlock(quiet):
        if not quiet:
            print("  ⚠️ OTP 解锁告警, 仍尝试读取 (读不锁; 若恒 0x0 且已烧写, 可能是密钥错误)")

    # OTPAR 选地址 + 读 FLC_DR (DOUT 只读, 真正读回)
    SWD_CMD.write_register(FLC_OTPAR, addr - 0x8000)
    time.sleep(DELAY_REG_WRITE)
    val = SWD_CMD.read_register_32bit(FLC_DR)

    # 恢复锁定 + 关闭时钟
    SWD_CMD.write_register(FLC_OTPKEYR, 0x0)
    time.sleep(DELAY_REG_WRITE)
    SWD_CMD.modify_register_range(FLC_AHBCKE, 0, 0, 0)
    time.sleep(DELAY_AFTER_RELOCK)
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
    #   OCD5010: 各修调值 → build_trim_word()  → burn_trim_words (自动选区域)
    #
    # 只需传入要通过 PASS 的修调项，缺失的字段将保持为 0。
    # ⚠️ OTP 只能烧写一次，烧写后无法更改！
    #
    # ★ 2026-09: 每个区域固定烧写 8 word (TAG+VTRIM1+VTRM2+ADCTRM+ANACINFO0~3)。
    #   ADCTRM/ANACINFO1~3 值待定 → 通过 info_words 传入 (缺省写 0)：
    #     info_words = {
    #         "ADCTRM":    0x00000000,   # 值待定
    #         "ANACINFO1": 0x00000000,
    #         "ANACINFO2": 0x00000000,
    #         "ANACINFO3": 0x00000000,
    #     }
    #   ★ ANACINFO0 自动填 vref_1d2v 校准后的 VREF 输出电压 (浮点, 见
    #     program_trim.py 步骤 4 / otp_slots.float_to_word), 手动传可覆盖。
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
    # 区域新增 word (ADCTRM/ANACINFO0~3) 的值待定 → 先不传, 烧写时缺省写 0
    info_words = None

    print("\n⚠️  警告: OTP 只能烧写一次, 烧写后无法更改!")
    confirm = input("确定要烧写当前修调值到 OTP 吗? (y/N): ").strip().lower()
    if confirm == "y":
        flash_write_trims(trim_values, info_words=info_words)
    else:
        print("已取消, 未写入 OTP。")
    print("Flash的OTP区域写入数据之后需要复位一下")
