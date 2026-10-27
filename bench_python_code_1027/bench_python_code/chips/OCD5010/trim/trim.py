"""
OCD5010 修调值数据逻辑

修调项 → (逻辑地址, bit起始, bit宽度)
对应寄存器 ANAC_TRM1 (0x8000) 和 ANAC_TRM2 (0x8004) 的字段定义。
每个修调值在寄存器中的 bit 位置与打包后的 32 位字完全一致。

写入目标: OTP 4 个烧写区域 (每个区域 8 word, 详见 otp_slots.py 的 OTP_SLOTS)
    ANAC_TRM1 打包值 → 区域 VTRIM1 word
    ANAC_TRM2 打包值 → 区域 VTRM2  word
    区域起始 TAG 写 ANA_TAG=0x00796783; 区域1~4 依次为 0x8000/0x8008/0x8010/0x8018
    ★ 2026-09: 区域固定烧写 8 word, 新增 ADCTRM/ANACINFO0~3:
      ANACINFO0 存 vref_1d2v 修调校准后的 VREF 输出电压 (浮点, 单位 mV,
      IEEE 754 单精度, 见 read_vref_mv / otp_slots.float_to_word);
      ADCTRM/ANACINFO1~3 值待定 → 默认 0, 由调用方传 info_words 字典指定
      (见 otp_slots.write_slot / burn_trim_words)

结构参考: chips/ocd2810/trim/trim.py
"""

import os

# =====================================================================
# ANAC_TRM1 (Flash 地址 0x8000) 各修调字段
# =====================================================================
# | 字段名         | bit位    | 宽度 | 说明               |
# |----------------|----------|------|--------------------|
# | REF_VTRM       | 0-4      | 5    | 参考电压微调        |
# | REF_ITRM       | 5-8      | 4    | 参考电流微调        |
# | LDO12_VTRM     | 9-11     | 3    | LDO12电压微调       |
# | SLP12_VTRM     | 12-13    | 2    | 休眠LDO12电压微调   |
# | SLP_REF_VTRM   | 14-17    | 4    | 休眠参考电压微调    |
# | ADC_VREFH_VTRM | 18-20    | 3    | ADC高参考电压微调   |
# | CSA_OS_VTRM    | 21-25    | 5    | CSA失调电压微调     |
# | RESERVED       | 26-31    | 6    | 保留位              |
#
# ANAC_TRM2 (Flash 地址 0x8004) 各修调字段
# =====================================================================
# | 字段名         | bit位    | 宽度 | 说明               |
# |----------------|----------|------|--------------------|
# | HSI_FTRM       | 0-6      | 7    | HSI频率微调         |
# | LSI_FTRM       | 7-13     | 7    | LSI频率微调         |
# | TJ_TTRM        | 14-18    | 5    | 温度传感器温度微调  |
# | TS_ITRM        | 19-24    | 6    | 温度传感器电流微调  |
# | BG_TTRM        | 25-30    | 6    | 带隙基准温度微调    |
# | RESERVED       | 31       | 1    | 保留位              |

# =====================================================================
# 修调项映射表
# key = Excel B 列（寄存器地址名）
# value = (逻辑地址, bit_start, bit_len)  —— 逻辑地址 0x8000=ANAC_TRM1, 0x8004=ANAC_TRM2
# 实际写入 OTP 槽位 (VTRIM1/VTRM2 word), 槽位见 otp_slots.py 的 OTP_SLOTS
# =====================================================================
TRIM_FLASH_MAP = {
    # ── ANAC_TRM1 (0x8000) ──
    "RW_REF_VTRM":       (0x8000, 0,  5),   # bit0-4
    "RW_REF_ITRM":       (0x8000, 5,  4),   # bit5-8
    "RW_LDO12_VTRM":     (0x8000, 9,  3),   # bit9-11
    "RW_SLP12_VTRM":     (0x8000, 12, 2),   # bit12-13
    "RW_SLP_REF_VTRM":   (0x8000, 14, 4),   # bit14-17
    "RW_ADC_VREFH_VTRM": (0x8000, 18, 3),   # bit18-20
    "RW_CSA_OS_VTRM":    (0x8000, 21, 5),   # bit21-25

    # ── ANAC_TRM2 (0x8004) ──
    "RW_HSI_FTRM":       (0x8004, 0,  7),   # bit0-6
    "RW_LSI_FTRM":       (0x8004, 7,  7),   # bit7-13
    "RW_TJ_TTRM":        (0x8004, 14, 5),   # bit14-18
    "RW_TS_ITRM":        (0x8004, 19, 6),   # bit19-24
    "RW_BG_TTRM":        (0x8004, 25, 6),   # bit25-30
}

# 地址名称
ADDR_NAME = {
    0x8000: "ANAC_TRM1",
    0x8004: "ANAC_TRM2",
}


def read_trim_values(xlsx_path):
    """
    从 trim_best_value.xlsx 读取已通过(PASS)的修调值。

    Excel 格式（由 tests.py 中的 save_trim_to_excel 生成）:
        A列: 测试项名称 (如 "vref_1d2v")
        B列: 寄存器地址名 (如 "RW_REF_VTRM")  ← 以此列作为 key
        C列: 寄存器地址(Hex)
        D列: 最佳TRIM值
        E列: 测试结果 (PASS/FAIL)

    返回:
        { "RW_REF_VTRM": 12, "RW_HSI_FTRM": 34, ... }
        仅包含已 PASS 且存在于 TRIM_FLASH_MAP 中的项。
        返回 None 表示文件不存在。
    """
    import openpyxl

    if not os.path.exists(xlsx_path):
        print(f"❌ 文件不存在: {xlsx_path}")
        return None

    wb = openpyxl.load_workbook(xlsx_path)
    ws = wb.active
    name_to_trim = {}

    for row in ws.iter_rows(min_row=2, values_only=True):
        if len(row) < 5:
            continue
        test_name = row[0]    # A列: 测试项名称
        reg_name = row[1]     # B列: 寄存器地址名（用作 key）
        best_trim = row[3]    # D列: 最佳 TRIM 值
        test_result = row[4]  # E列: PASS/FAIL

        if reg_name in TRIM_FLASH_MAP and best_trim is not None and test_result == "PASS":
            name_to_trim[reg_name] = int(best_trim)
            print(f"  ▶ {reg_name:20s} (test: {test_name}) = {best_trim}")

    wb.close()
    return name_to_trim


def read_vref_mv(xlsx_path):
    """
    从 trim_best_value.xlsx 读取 vref_1d2v 修调校准后的 VREF 输出电压 (浮点, mV)。

    save_trim_to_excel (tests.py) 会把 VREF 实测电压写入第 6 列
    ("最佳输出电压", 单位 mV)。ANACINFO0 (OTP 0x8004/0x800C/...) 需要存放该
    电压的浮点值 (mV), 供 program_trim.py 用 otp_slots.float_to_word 编码后
    烧入 (IEEE 754 单精度, 单位 mV)。

    返回: VREF 输出电压 (float, 单位 mV) 或 None (未测到 / 未 PASS / 无电压列)。
    """
    import openpyxl

    if not os.path.exists(xlsx_path):
        return None
    wb = openpyxl.load_workbook(xlsx_path)
    ws = wb.active
    vref_mv = None
    for row in ws.iter_rows(min_row=2, values_only=True):
        if len(row) < 6:
            continue
        if row[0] == "vref_1d2v" and row[4] == "PASS" and row[5] is not None:
            try:
                vref_mv = float(row[5])   # 单位已是 mV
            except (ValueError, TypeError):
                vref_mv = None
            break
    wb.close()
    return vref_mv


def read_adctrm_word(xlsx_path):
    """
    从 trim_best_value.xlsx 读取 ADC_CAL_TRIM 已保存的 ADCTRM 打包值
    (bit0~11=GAIN, bit16~27=OFFSET —— 与 ADC_CALIB 寄存器位布局相反,
    见 tests.py ADC_CAL_TRIM 保存逻辑)。

    返回: 32 位整数 (D列"最佳TRIM值") 或 None (未测到/未 PASS)。
    """
    import openpyxl

    if not os.path.exists(xlsx_path):
        return None
    wb = openpyxl.load_workbook(xlsx_path)
    ws = wb.active
    word = None
    for row in ws.iter_rows(min_row=2, values_only=True):
        if len(row) < 5:
            continue
        if row[0] == "adc_cal_trim" and row[1] == "ADCTRM" and row[4] == "PASS" and row[3] is not None:
            try:
                word = int(row[3])
            except (ValueError, TypeError):
                word = None
            break
    wb.close()
    return word


def build_trim_values(name_to_trim):
    """
    根据 TRIM_FLASH_MAP 将各字段值按位组装。

    参数:
        name_to_trim: { "RW_REF_VTRM": 12, ... }

    返回:
        { 0x8000: 0xABCDEF01, 0x8004: 0x12345678 }
        每个逻辑地址对应一个完整的 32 位值。
        烧写时写入 OTP 槽位的 VTRIM1/VTRM2 word (槽位见 otp_slots.py)。
    """
    # 按地址分组，逐位组装
    addr_values = {}  # { addr: 32bit_value }

    for reg_name, (addr, bit_start, bit_len) in TRIM_FLASH_MAP.items():
        if reg_name not in name_to_trim:
            continue

        value = name_to_trim[reg_name]

        # 校验值是否在有效范围内
        max_val = (1 << bit_len) - 1
        if value < 0 or value > max_val:
            print(f"  ⚠️ {reg_name} 值 {value} 超出 {bit_len} 位范围 (0~{max_val}), 跳过")
            continue

        # 初始化该地址的值（如果尚未初始化）
        if addr not in addr_values:
            addr_values[addr] = 0

        # 将 value 放入对应的 bit 位置（保留其他位不变）
        mask = ((1 << bit_len) - 1) << bit_start
        addr_values[addr] = (addr_values[addr] & ~mask) | ((value & ((1 << bit_len) - 1)) << bit_start)

        addr_name = ADDR_NAME.get(addr, f"0x{addr:04X}")
        print(f"    将 {reg_name:20s} = {value:3d} (0x{value:02X}) → {addr_name} bit{bit_start}~{bit_start+bit_len-1}")

    return addr_values


def main():
    """独立测试：读取 Excel 并打印组装结果"""
    script_dir = os.path.dirname(os.path.abspath(__file__))
    xlsx_path = os.path.normpath(
        os.path.join(script_dir, "..", "..", "..", "Save_Results", "OCD5010", "trim_best_value.xlsx")
    )

    print("=" * 58)
    print("  OCD5010 TRIM 组装测试")
    print("=" * 58)
    print(f"  Excel: {xlsx_path}")

    name_to_trim = read_trim_values(xlsx_path)
    if not name_to_trim:
        print("⚠️  未读取到有效的 TRIM 值")
        return

    addr_values = build_trim_values(name_to_trim)
    if not addr_values:
        print("⚠️  组装失败，无有效值")
        return

    print(f"\n  {'=' * 58}")
    print("  组装结果:")
    for addr in sorted(addr_values.keys()):
        name = ADDR_NAME.get(addr, f"0x{addr:04X}")
        print(f"    {name} (0x{addr:04X}) = 0x{addr_values[addr]:08X}")
    print(f"  {'=' * 58}")


if __name__ == "__main__":
    main()
