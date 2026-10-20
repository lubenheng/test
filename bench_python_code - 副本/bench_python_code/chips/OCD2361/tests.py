"""
chip_b 测试函数（模板）

每个芯片的测试项在这里定义，并在 TEST_ITEMS 中注册。
main.py 会自动加载 TEST_ITEMS 来构建交互菜单。

TEST_ITEMS 格式:
    { "1": ("显示名称", 测试函数), ... }

其中测试函数为无参可调用对象（def func(): ...）。
"""
import re
import sys
import time
from pathlib import Path

import numpy as np
import openpyxl
from openpyxl.styles import Font

# from bench_python_code.instrument.multimeter.Keysight_34465A import read_keysight_voltage
from instrument import instrument_set as instr
from chips.OCD2361 import chip as chip_cfg
from instrument.IIC.iic_ops import execute_write, execute_read, IICLogic

# instrument/IIC 下的 mtp_ops 内部使用同目录绝对导入（from crc import ...），
# 需先把该目录加入 sys.path 才能按包路径导入；用 append 避免遮蔽同名顶层模块。
_IIC_DIR = str(Path(__file__).resolve().parents[2] / "instrument" / "IIC")
if _IIC_DIR not in sys.path:
    sys.path.append(_IIC_DIR)
from instrument.IIC.mtp_ops import MTPController
from instrument.IIC.mtp_batch_ops import MTPBatchLogic
from instrument.IIC.crc import calculate_crc32_iter

# =====================================================================
# 校准标准判定（测试项 Min./Typ./Max. 取自 2361校准标准.xlsx）
#
# 表格式：A=测试项  B=测试条件  C=Min.  D=Typ.  E=Max.  F=Unit  G=BIN
# 表中单位与测量函数返回的单位不同（表里是 V，万用表返回 mV），
# 判定前按 Unit 列统一换算到测量值的单位。
# =====================================================================

CALIB_STANDARD_PATH = (Path(__file__).resolve().parents[2]
                       / "test_configs" / "OCD2361" / "2361校准标准.xlsx")
CALIB_SHEET_NAME = "Sheet1"

# 表内单位 → 测量函数返回单位 的换算系数
#   电压: dmm_measure_single_voltage 返回 mV
#   电流: dmm_measure_single_current 返回 mA
#   频率: osc_measure_frequency     返回 Hz
_UNIT_SCALE = {
    "V": 1000.0,
    "MV": 1.0,
    "MA": 1.0,
    "UA": 0.001,
    "A": 1000.0,
    "HZ": 1.0,
    "KHZ": 1000.0,
    "MHZ": 1e6,
    "Ω": 1.0,
    "OHM": 1.0,
}

# 解析结果缓存: {(路径, 表名): {测试项: (Min., Typ., Max., Unit)}}
_CALIB_CACHE = {}


def _to_float(value):
    """把单元格值转成 float，转不了（空/表达式/±符号）返回 None。"""
    if value is None:
        return None
    try:
        return float(value)
    except (TypeError, ValueError):
        return None


def _load_calib_standard(path=CALIB_STANDARD_PATH, sheet=CALIB_SHEET_NAME):
    """读取整张校准标准表，返回 {测试项: (Min., Typ., Max., Unit)}（带缓存）。"""
    key = (str(path), sheet)
    if key in _CALIB_CACHE:
        return _CALIB_CACHE[key]

    path = Path(path)
    if not path.exists():
        raise FileNotFoundError(f"找不到校准标准表：{path}")

    wb = openpyxl.load_workbook(path, data_only=True)
    ws = wb[sheet] if sheet in wb.sheetnames else wb.active

    table = {}
    # 第 1 行是表头（测试项/测试条件/Min./Typ./Max./Unit/BIN），数据从第 2 行开始
    for row in ws.iter_rows(min_row=2, values_only=True):
        name = str(row[0]).strip() if row[0] is not None else ""
        if not name:
            continue
        unit = str(row[5]).strip() if len(row) > 5 and row[5] is not None else ""
        table[name] = (_to_float(row[2]), _to_float(row[3]), _to_float(row[4]), unit)

    _CALIB_CACHE[key] = table
    return table


def _judge_by_standard(test_item_name, value, extra_note="", limits=None):
    """按校准标准表的 Min./Max. 判定测试项是否通过。

    参数：
        test_item_name: str - 表 A 列的测试项名（与 Excel 中的写法一致）
        value: float/None - 实测值（单位取测量函数返回的单位）
        extra_note: str - 附加到结果行的说明（如修调值）
        limits: tuple/None - (Min, Max) 覆盖值，单位与 value 一致（如 mV）；
                给定时不使用校准表中的 Min./Max.，Typ 仍取自校准表
    返回：
        True/False - 判定结果；无法判定（无标准/无上下限/测量失败）返回 None
    """
    if value is None:
        print(f"❌ {test_item_name} failed：测量失败（无有效读数）")
        return None

    try:
        table = _load_calib_standard()
    except Exception as e:
        print(f"   ⚠️ 读取校准标准失败，跳过判定：{e}")
        return None

    if test_item_name not in table:
        print(f"   ⚠️ 校准标准中未找到测试项 '{test_item_name}'，跳过判定")
        return None

    min_val, typ_val, max_val, unit = table[test_item_name]
    scale = _UNIT_SCALE.get(unit.upper(), 1.0)
    if unit and unit.upper() not in _UNIT_SCALE:
        print(f"   ⚠️ 未识别的单位 '{unit}'，按 1:1 处理")

    min_m = None if min_val is None else min_val * scale
    max_m = None if max_val is None else max_val * scale
    typ_m = None if typ_val is None else typ_val * scale
    if limits is not None:
        min_m, max_m = limits
    note = f"，{extra_note}" if extra_note else ""

    if min_m is None and max_m is None:
        print(f"⚠️ {test_item_name} 无判定标准（表中 Min./Max. 均未填），"
              f"实测 {value:.3f}{note}")
        return None

    if min_m is not None and max_m is not None:
        limit_txt = f"{min_m:.4f} ~ {max_m:.4f}"
    elif min_m is not None:
        limit_txt = f">= {min_m:.4f}"
    else:
        limit_txt = f"<= {max_m:.4f}"

    passed = True
    if min_m is not None and value < min_m:
        passed = False
    if max_m is not None and value > max_m:
        passed = False

    typ_txt = f"，Typ {typ_m:.4f}" if typ_m is not None else ""
    print(f"{'✅' if passed else '❌'} {test_item_name} "
          f"{'pass' if passed else 'failed'}：判定范围 {limit_txt}{typ_txt}，"
          f"实测 {value:.4f}{note}")
    return passed


# =====================================================================
# 寄存器地址（addr, index）与写入数据 —— 请按 OCD2361 芯片手册填写
# 每个测试项的寄存器配置在其对应函数内声明，便于逐项核对
# =====================================================================


def _log_read(addr, index):
    """读回并打印 4 字节数据。"""
    data = execute_read(_get_i2c(), addr, index)
    if data is not None:
        print(f"读回 [{addr}:{index}] = {' '.join(f'{b:02X}' for b in data)}")
    else:
        print(f"读回 [{addr}:{index}] = 无有效返回")
    return data


def _get_i2c():
    """获取已初始化的 IIC 控制器实例。"""
    i2c = chip_cfg.get_i2c()
    if i2c is None:
        raise RuntimeError("IIC 未初始化，请确认已执行 IIC_init_all")
    return i2c


# =====================================================================
# 通用测试流程
# =====================================================================

def _voltage_test(name, addr, index, write_data):
    """电压类测试：写配置 → 万用表测电压(mV) → 读回校验。"""
    print(f"\n🚀 执行 {name} ...")
    i2c = _get_i2c()
    execute_write(i2c, addr=addr, index=index, write_data=write_data)
    _log_read(addr, index)
    mv = instr.dmm_measure_single_voltage()
    print(f"   电压 = {mv} mV")
    if mv is not None:
        print(f"   电压 = {mv} mV")
    else:
        print("   电压测量失败")
    print(f"✅ {name} 完成")
    return mv


def _current_test(name, addr, index, write_data):
    """电流类测试：写配置 → 万用表测电流(mA) → 读回校验。"""
    print(f"\n🚀 执行 {name} ...")
    i2c = _get_i2c()
    execute_write(i2c, addr=addr, index=index, write_data=write_data)
    ma = instr.dmm_measure_single_current()
    if ma is not None:
        print(f"   电流 = {ma} mA")
    else:
        print("   电流测量失败")
    _log_read(addr, index)
    print(f"✅ {name} 完成")
    return ma


# =====================================================================
# 测试函数 —— 每个测试项对应一个函数
# =====================================================================

def TESTMODE_EN():
    """进入测试模式使能：MTP上电 → MTP Load → (间隔1s) → 测试模式。"""
    print("\n🚀 执行 进入测试模式使能 ...")
    i2c = _get_i2c()

    mtp_power_data = [0x04, 0x91, 0x15, 0x55]
    print(f"MTP上电：写入 [25:0] = {' '.join(f'{b:02X}' for b in mtp_power_data)}")
    execute_write(i2c, addr=25, index=0, write_data=mtp_power_data)
    _log_read(25, 0)

    time.sleep(1)

    testmode_data = [0x5a, 0x5a, 0x5a, 0x5a]
    print(f"测试模式：写入 [20:15] = {' '.join(f'{b:02X}' for b in testmode_data)}（5A5A5A5A 不回读）")
    execute_write(i2c, addr=20, index=15, write_data=testmode_data)

    print("✅ 进入测试模式使能 完成")

# =====================================================================
# MTP_LOAD
# =====================================================================

def MTP_LOAD():
    """"""
    print("\n🚀 执行MTP_LOAD ...")
    i2c = _get_i2c()
    mtp_load_data = [0x5a, 0x5a, 0x5a, 0x5a]
    print(f"MTP Load：写入 [25:5] = {' '.join(f'{b:02X}' for b in mtp_load_data)}（5A5A5A5A 不回读）")
    execute_write(i2c, addr=25, index=5, write_data=mtp_load_data)

    time.sleep(1)

    testmode_data = [0x5a, 0x5a, 0x5a, 0x5a]
    print(f"测试模式：写入 [20:15] = {' '.join(f'{b:02X}' for b in testmode_data)}（5A5A5A5A 不回读）")
    execute_write(i2c, addr=20, index=15, write_data=testmode_data)

    print("✅ MTP_LOAD 完成")


# ---------------------- 电压类 ----------------------

# 修调寄存器 [20:4]：32 bit，三个修调字段共用该寄存器
#   VBGTRIM   bit[16:14]（3 bit）
#   RW_OSC_24M_TRIM bit[13:8] （6 bit）
#   RW_VREF_TRIM   bit[7:0]   （8 bit）
# 该寄存器在 MTP 中对应物理地址 440（ADDR=0x1A8, INDEX=4），
# 修调完成后把对应字段同步回 MTP 配置表，见 _sync_mtp_word_field()。
# 注意：写入必须先读-改-写，否则会清掉同寄存器里的其它字段。
VBG_TRIM_ADDR = 20
VBG_TRIM_INDEX = 4
VBG_TRIM_MAX = 8          # 3 bit，取值 0~7
VBG_TRIM_BIT_LO = 14      # VBGTRIM[2:0] 位于 32 bit 的 [16:14]
VBG_TRIM_MASK = 0x07 << VBG_TRIM_BIT_LO

OSC_TRIM_ADDR = VBG_TRIM_ADDR        # 同一个寄存器 [20:4]
OSC_TRIM_INDEX = VBG_TRIM_INDEX
OSC_TRIM_BIT_LO = 8                  # RW_OSC_24M_TRIM 位于 32 bit 的 [13:8]
OSC_TRIM_MAX = 0x3F                  # 6 bit 宽（手册写 <4:0>，表内实际值 0x20 需 6 位）
OSC_TRIM_MASK = OSC_TRIM_MAX << OSC_TRIM_BIT_LO

VREF_TRIM_ADDR = VBG_TRIM_ADDR       # 同一个寄存器 [20:4]
VREF_TRIM_INDEX = VBG_TRIM_INDEX
VREF_TRIM_BIT_LO = 0                 # RW_VREF_TRIM 位于 32 bit 的 [7:0]
VREF_TRIM_MAX = 0xFF                 # 8 bit
VREF_TRIM_MASK = VREF_TRIM_MAX << VREF_TRIM_BIT_LO

# [20:4] 在 MTP 配置表中的位置：phys = 0x1A8 + 4*4 = 440
VBG_TRIM_MPT_ADDR = 0x1A8
VBG_TRIM_MPT_INDEX = 4
OSC_TRIM_MPT_ADDR = VBG_TRIM_MPT_ADDR
OSC_TRIM_MPT_INDEX = VBG_TRIM_MPT_INDEX
VREF_TRIM_MPT_ADDR = VBG_TRIM_MPT_ADDR
VREF_TRIM_MPT_INDEX = VBG_TRIM_MPT_INDEX

# VBG 判定范围（mV）：1235 ~ 1265，覆盖校准表 VBG_Voltage 的 Min./Max.
VBG_LIMITS_MV = (1230.0, 1270.0)
VBG_TARGET_MV = 1250.0     # 修调目标值

# OSC 目标频率（Hz），与校准表 OSC_P 的 Typ 11.72 kHz 一致
OSC_TARGET_HZ = 11720.0
OSC_SCAN_SPAN = 5          # 在估算值附近 ±5 内扫描

# OSC 判定范围（Hz）：11620 ~ 11820，Typ 11.72 kHz
OSC_LIMITS_HZ = (11620.0, 11820.0)

# ADC_VREF 目标电压（mV），与校准表 ADC_VREF_Voltage 的 Typ 2.0 V 一致
ADC_VREF_TARGET_MV = 2000.0
ADC_VREF_LIMITS_MV = (1900.0, 2100.0)   # 判定范围，与校准表 Min./Max. 一致
VREF_PROBE_LO = 0          # 斜率探测点 1：修调值 0
VREF_PROBE_HI = 10         # 斜率探测点 2：修调值 10
VREF_SCAN_SPAN = 10        # 估算值不达标时，在 ±10 内扫描

# ADC_IN（菜单 14）：基准板 DAC 输出 → WO，ADC 采样
# 规格要求 WO 输入不超过 2 V，因此电压上限固定 2000 mV
ADC_IN_VMAX_MV = 2000
ADC_IN_DAC_START_MV = 200       # 默认起点（运行时可在终端修改）
ADC_IN_DAC_STEP_MV = 10         # 默认步进（运行时可在终端修改）
ADC_IN_DAC_TOL_MV = 20.0        # DAC 输出线性度容差（mV）

# ADC 采样：规格 2.13 节提到 ADCBSY / ADC4_DR<13:0>，但未给出 (ADDR, INDEX)。
# 填写后启用 ADC 列（码值与转换电压）；保持 None 则只做 DAC 线性测量。
ADCBSY_ADDR = None              # 触发转换的寄存器 ADDR（写 1 启动）
ADCBSY_INDEX = None
ADCBSY_BIT = 0                  # 触发位
ADC4_DR_ADDR = None             # ADC 结果寄存器 ADDR
ADC4_DR_INDEX = None
ADC4_DR_MASK = 0x3FFF           # ADC4_DR<13:0>
ADC_IN_VREF_MV = 2000.0         # 换算用 ADC_VREF（mV）

# 测量数据导出目录（相对 bench_python_code）
ADC_IN_EXPORT_DIR = "Save_Results"


def _adc_available():
    """ADC 寄存器是否已配置。"""
    return None not in (ADCBSY_ADDR, ADCBSY_INDEX, ADC4_DR_ADDR, ADC4_DR_INDEX)


def _adc_sample(i2c, timeout_s=0.05):
    """触发一次 ADC 转换并读取码值；未配置或失败返回 None。"""
    if not _adc_available():
        return None
    try:
        # ADCBSY 写 1 触发（读-改-写，避免破坏同寄存器其它位）
        _write_word_field(i2c, ADCBSY_ADDR, ADCBSY_INDEX,
                          ADCBSY_BIT, 0x01 << ADCBSY_BIT, 1)
        time.sleep(timeout_s)
        rd = execute_read(i2c, ADC4_DR_ADDR, ADC4_DR_INDEX)
        if not rd or len(rd) < 4:
            return None
        word = (rd[0] << 24) | (rd[1] << 16) | (rd[2] << 8) | rd[3]
        return word & ADC4_DR_MASK
    except Exception as e:
        print(f"   ⚠️ ADC 采样异常：{e}")
        return None


def _ask_int(prompt, default, lo, hi):
    """交互式读取整数，非法输入或直接回车时用默认值。"""
    try:
        text = input(f"   {prompt}（默认 {default}）: ").strip()
    except (EOFError, KeyboardInterrupt):
        print()
        return default
    if not text:
        return default
    try:
        val = int(text)
    except ValueError:
        print(f"   ⚠️ 输入无效 {text!r}，使用默认值 {default}")
        return default
    if not (lo <= val <= hi):
        print(f"   ⚠️ 超出 {lo}~{hi}，使用默认值 {default}")
        return default
    return val


def _export_adc_table(rows, path_hint):
    """把测量表导出为 xlsx，返回保存路径；失败返回 None。"""
    try:
        wb = openpyxl.Workbook()
        ws = wb.active
        ws.title = "ADC_IN测量数据"
        headers = ["设定电压(mV)", "万用表实测(mV)", "DAC偏差(mV)",
                   "ADC码值", "ADC转换电压(mV)", "ADC误差(mV)", "ADC误差(LSB)"]
        ws.append(headers)
        for c in range(1, len(headers) + 1):
            ws.cell(1, c).font = Font(bold=True)
        for r in rows:
            ws.append(r)
        for col, width in zip("ABCDEFG", (16, 18, 14, 12, 18, 14, 14)):
            ws.column_dimensions[col].width = width
        out_dir = Path(__file__).resolve().parents[2] / ADC_IN_EXPORT_DIR
        out_dir.mkdir(parents=True, exist_ok=True)
        path = out_dir / path_hint
        wb.save(path)
        return path
    except Exception as e:
        print(f"   ⚠️ 导出 Excel 失败：{e}")
        return None


def _write_word_field(i2c, addr, index, bit_lo, mask, value):
    """读-改-写：只替换 32 bit 寄存器中的指定位段，保留其它字段。

    返回：True=写入成功，False=读回失败（不写，避免破坏其它字段）。
    """
    rd = execute_read(i2c, addr, index)
    if not rd or len(rd) < 4:
        return False
    word = (rd[0] << 24) | (rd[1] << 16) | (rd[2] << 8) | rd[3]
    word = (word & ~mask) | ((value << bit_lo) & mask)
    execute_write(i2c, addr=addr, index=index, write_data=_word_to_bytes(word))
    return True


def _vbg_trim_write(i2c, trim):
    """把 3 位 VBGTRIM 写入 [20:4]，保留 OSC_TRIM 等其它字段。"""
    return _write_word_field(i2c, VBG_TRIM_ADDR, VBG_TRIM_INDEX,
                             VBG_TRIM_BIT_LO, VBG_TRIM_MASK, trim & 0x07)


def _osc_trim_write(i2c, trim):
    """把 6 位 RW_OSC_24M_TRIM 写入 [20:4] 的 bit[13:8]，保留 VBGTRIM 等其它字段。"""
    return _write_word_field(i2c, OSC_TRIM_ADDR, OSC_TRIM_INDEX,
                             OSC_TRIM_BIT_LO, OSC_TRIM_MASK, trim & OSC_TRIM_MAX)


def _osc_trim_read(i2c=None):
    """读取 [20:4] 中的 6 位 RW_OSC_24M_TRIM（bit[13:8]），失败返回 None。"""
    rd = _log_read(OSC_TRIM_ADDR, OSC_TRIM_INDEX)
    if not rd or len(rd) < 4:
        return None
    return rd[2] & OSC_TRIM_MAX


def _vref_trim_write(i2c, trim):
    """把 8 位 RW_VREF_TRIM 写入 [20:4] 的 bit[7:0]，保留其它字段。"""
    return _write_word_field(i2c, VREF_TRIM_ADDR, VREF_TRIM_INDEX,
                             VREF_TRIM_BIT_LO, VREF_TRIM_MASK, trim & VREF_TRIM_MAX)


def _vref_trim_read(i2c=None):
    """读取 [20:4] 中的 8 位 RW_VREF_TRIM（bit[7:0]），失败返回 None。"""
    rd = _log_read(VREF_TRIM_ADDR, VREF_TRIM_INDEX)
    if not rd or len(rd) < 4:
        return None
    return rd[3] & VREF_TRIM_MAX


def _vbg_trim_read(i2c=None):
    """读取 [20:4] 中的 3 位修调值，失败返回 None。"""
    rd = _log_read(VBG_TRIM_ADDR, VBG_TRIM_INDEX)
    if not rd or len(rd) < 4:
        return None
    return ((rd[1] & 0x01) << 2) | ((rd[2] >> 6) & 0x03)


def _judge_vbg(value, extra_note=""):
    """按 VBG_LIMITS_MV（1235 ~ 1265 mV）判定，Typ 取自校准表。"""
    return _judge_by_standard("VBG_Voltage", value,
                              extra_note=extra_note, limits=VBG_LIMITS_MV)


def _judge_osc(value, extra_note=""):
    """按 OSC_LIMITS_HZ（11620 ~ 11820 Hz）判定，Typ 取自校准表。"""
    return _judge_by_standard("OSC_P", value,
                              extra_note=extra_note, limits=OSC_LIMITS_HZ)


def _judge_vref(value, extra_note=""):
    """按 ADC_VREF_LIMITS_MV（1900 ~ 2100 mV）判定，Typ 取自校准表。"""
    return _judge_by_standard("ADC_VREF_Voltage", value,
                              extra_note=extra_note, limits=ADC_VREF_LIMITS_MV)


def _word_to_bytes(word):
    """32 bit -> [31:24] [23:16] [15:8] [7:0]。"""
    return [(word >> 24) & 0xFF, (word >> 16) & 0xFF, (word >> 8) & 0xFF, word & 0xFF]


def _sync_mtp_word_field(mtp_addr, mtp_index, bit_lo, mask, value, label):
    """把某个 32 bit 寄存器中的指定位段同步到 MTP 配置表。

    [20:4] 的 32 bit 对应 MTP 物理地址 440（ADDR=0x1A8, INDEX=4）。
    只改 bit_lo 起、mask 覆盖的位，其余位保持原值；440 >= 416，
    不参与 CRC，CRC32 不受影响。

    参数：
        mtp_addr / mtp_index: MTP 表中的 (ADDR, INDEX)
        bit_lo: 字段最低位
        mask:   字段掩码（已移位）
        value:  新字段值
        label:  打印用的字段名（如 "VBGTRIM"）
    返回：True 表示同步成功或无需同步，False 表示失败。
    """
    import importlib
    from chips.OCD2361 import mtp_config_table as cfg

    table_py = Path(cfg.__file__)
    row = next((r for r in cfg.MTP_CONFIG_ROWS
                if r[0] == mtp_addr and r[1] == mtp_index), None)
    if row is None:
        print(f"   ⚠️ MTP 配置表中未找到 "
              f"(0x{mtp_addr:02X}, {mtp_index})，跳过同步")
        return False

    _, _, b31, b23, b15, b7, _ = row
    word = (b31 << 24) | (b23 << 16) | (b15 << 8) | b7
    old_val = (word >> bit_lo) & (mask >> bit_lo)
    new_word = (word & ~mask) | ((value << bit_lo) & mask)
    if new_word == word:
        print(f"   MTP 配置表中 {label} 已为 {value}，无需同步")
        return True

    nb31, nb23, nb15, nb7 = _word_to_bytes(new_word)

    # 1) 改写 mtp_config_table.py 中该行（保留原有排布与注释）
    text = table_py.read_text(encoding='utf-8')
    pattern = re.compile(
        r'^(\s*\(0x%02X,\s*%d,\s*)0x[0-9A-Fa-f]{2}(,\s*)0x[0-9A-Fa-f]{2}(,\s*)'
        r'0x[0-9A-Fa-f]{2}(,\s*)0x[0-9A-Fa-f]{2}(.*)$'
        % (mtp_addr, mtp_index),
        re.MULTILINE)

    def _repl(m):
        return (f"{m.group(1)}0x{nb31:02X}{m.group(2)}0x{nb23:02X}{m.group(3)}"
                f"0x{nb15:02X}{m.group(4)}0x{nb7:02X}{m.group(5)}")

    new_text, n = pattern.subn(_repl, text)
    if n != 1:
        print(f"   ⚠️ mtp_config_table.py 中匹配到 {n} 行，未修改（预期 1 行）")
        return False
    table_py.write_text(new_text, encoding='utf-8')

    # 2) 重新生成 .txt（重新加载模块，使内存中的表与文件一致）
    importlib.reload(cfg)
    try:
        _mtp_dir = Path(__file__).resolve().parents[2] / "other_test" / "mtp"
        if str(_mtp_dir) not in sys.path:
            sys.path.append(str(_mtp_dir))
        import build_mtp_config_txt as txt_builder
        rows = [(a, i, h, k, l, m, mod) for a, i, h, k, l, m, mod in cfg.MTP_CONFIG_ROWS]
        others = [(r, op, list(vals)) for r, op, *vals in getattr(cfg, "OTHER_ROWS", [])]
        body, _crc = txt_builder.build_text(rows, others, "chips/OCD2361/mtp_config_table.py")
        txt_builder.DST.write_text(body, encoding='utf-8')
        txt_note = f"，已重新生成 {txt_builder.DST.name}"
    except Exception as e:
        txt_note = f"，但 .txt 生成失败：{e}"

    print(f"   ✅ MTP 配置表 {label}：{old_val} -> {value}"
          f"（[0x{mtp_addr:02X}:{mtp_index}] 字节 "
          f"{nb31:02X} {nb23:02X} {nb15:02X} {nb7:02X}，phys {mtp_addr + mtp_index * 4}）{txt_note}")
    return True


def _sync_vbg_trim_to_table(trim):
    """把修调后的 VBGTRIM（bit[16:14]）同步到 MTP 配置表。"""
    return _sync_mtp_word_field(VBG_TRIM_MPT_ADDR, VBG_TRIM_MPT_INDEX,
                                VBG_TRIM_BIT_LO, VBG_TRIM_MASK, trim & 0x07, "VBGTRIM")


def _sync_osc_trim_to_table(trim):
    """把修调后的 RW_OSC_24M_TRIM（bit[13:8]）同步到 MTP 配置表。"""
    return _sync_mtp_word_field(OSC_TRIM_MPT_ADDR, OSC_TRIM_MPT_INDEX,
                                OSC_TRIM_BIT_LO, OSC_TRIM_MASK,
                                trim & OSC_TRIM_MAX, "RW_OSC_24M_TRIM")


def _sync_vref_trim_to_table(trim):
    """把修调后的 RW_VREF_TRIM（bit[7:0]）同步到 MTP 配置表。"""
    return _sync_mtp_word_field(VREF_TRIM_MPT_ADDR, VREF_TRIM_MPT_INDEX,
                                VREF_TRIM_BIT_LO, VREF_TRIM_MASK,
                                trim & VREF_TRIM_MAX, "RW_VREF_TRIM")


def _osc_ensure_connected():
    """确保示波器会话可用；会话失效时重新建立连接（不关闭其它仪器的 VISA 会话）。

    返回：True=可用，False=重建失败。
    """
    osc_mod = getattr(instr, "_osc", None)
    osc_inst = getattr(osc_mod, "osc_inst", None) if osc_mod is not None else None
    if osc_inst is None:
        print("   示波器未连接，正在重新初始化 ...")
        return bool(instr.osc_init())
    try:
        osc_inst.query("*IDN?")
        return True
    except Exception as e:
        print(f"   ⚠️ 示波器会话失效（{e}），重新建立连接 ...")
        # 只重建本驱动的会话，避免 rm.close() 连带关闭万用表等其它会话
        try:
            return bool(instr.osc_init())
        except Exception as e2:
            print(f"   ❌ 示波器重连失败：{e2}")
            return False


def _osc_measure_freq(channel="CH1", retries=1):
    """测量频率；失败时保持/重建连接后重试。

    返回：(freq, period, duty)，失败返回 (None, None, None)。
    """
    for attempt in range(retries + 1):
        freq, period, duty = instr.osc_measure_frequency(channel)
        if freq is not None:
            return freq, period, duty
        if attempt < retries:
            print("   ⚠️ 频率测量失败，检查示波器连接后重试 ...")
            if not _osc_ensure_connected():
                break
            time.sleep(0.5)
    return None, None, None


def _confirm_connection(message):
    """操作员接线确认：仅提示，按任意键（回车）继续。"""
    try:
        input(f"   {message}，按任意键继续...")
    except (EOFError, KeyboardInterrupt):
        print()
        return False
    return True


def VBG_Voltage():
    """VBG 电压测试：测电压 → 判定；不通过时扫描 3 位修调值 0~7 并写回最优值。"""
    print("\n🚀 执行 VBG 电压修调测试 ...")
    print(f"   判定范围 {VBG_LIMITS_MV[0]:.0f} ~ {VBG_LIMITS_MV[1]:.0f} mV")
    _confirm_connection("请确定万用表连接WO")

    mv = _voltage_test("VBG电压", addr=20, index=9, write_data=[0x00, 0x00, 0x00, 0x20])
    time.sleep(0.5)

    if mv is None:
        print("❌ VBG 电压测量失败，跳过本次测试")
        return None

    i2c = _get_i2c()
    cur = _vbg_trim_read(i2c)
    if cur is None:
        print(" 读取 trim_internal 失败，跳过本次测试")
        return None
    print(f"   当前修调值 = {cur}")

    passed = _judge_vbg(mv, extra_note=f"修调 {cur}")
    if passed is True:
        print(f"VBG 测试通过, 测量值: {mv:.3f} mV, 修调值为: {cur}, 无需修调")
        print("✅ VBG 测试完成")
        return mv

    # 以当前修调值为基准，扫描全部 3 bit 修调值，取与目标值偏差最小者
    typ_target = VBG_TARGET_MV
    best_trim, min_err = cur, abs(mv - typ_target)
    print(f"   目标值 {typ_target:.3f} mV，开始扫描修调值 0~{VBG_TRIM_MAX - 1} ...")

    for test_trim in range(VBG_TRIM_MAX):
        if not _vbg_trim_write(i2c, test_trim):
            print(f"   修调值={test_trim}: 写入失败（回读异常），跳过")
            continue
        time.sleep(0.5)
        test_vol = instr.dmm_measure_single_voltage()
        if test_vol is None:
            print(f"   修调值={test_trim}: 电压测量失败，跳过")
            continue
        test_err = abs(test_vol - typ_target)
        mark = ""
        if test_err < min_err:
            best_trim, min_err = test_trim, test_err
            mark = "   ← 当前最优"
        print(f"   修调值={test_trim}: 电压={test_vol:.3f} mV, "
              f"误差={test_err:.3f} mV{mark}")

    # 写回最优修调值并判定
    if not _vbg_trim_write(i2c, best_trim):
        print(f"❌ VBG 修调失败：写回修调值 {best_trim} 时回读异常")
        return None
    time.sleep(0.5)
    best_vol = instr.dmm_measure_single_voltage()
    if best_vol is None:
        print(f"❌ VBG 修调失败：写回修调值 {best_trim} 后测量失败")
        return None

    passed = _judge_vbg(best_vol, extra_note=f"修调 {best_trim}")
    result = "成功" if passed is True else "失败"
    print(f"VBG 修调{result}, 测量值: {best_vol:.3f} mV, 修调值为: {best_trim}")

    # 修调完成后，把 VBGTRIM 同步到 MTP 配置表（[20:4] ↔ MTP phys 440）
    _sync_vbg_trim_to_table(best_trim)

    print("✅ VBG 测试完成")
    return best_vol


def VBG_buffer_Voltage():
    """VBG_buffer 电压测试。"""
    mv = _voltage_test("VBG_buffer电压", addr=19, index=63, write_data=[0x00, 0x00, 0x00, 0x60])
    _judge_by_standard("VBG_buffer_Voltage", mv)
    return mv

def OSC_VREF_Voltage():
    """OSC_VREF 电压测试。"""
    mv =_voltage_test("OSC_VREF电压", addr=20, index=9, write_data=[0x00, 0x00, 0x00, 0xA0])
    _judge_by_standard("OSC_VREF_Voltage", mv)
    return mv

def VPTAT_Voltage():
    """VPTAT 电压测试。"""
    mv = _voltage_test("VPTAT电压", addr=20, index=9, write_data=[0x00, 0x00, 0x00, 0xE0])
    _judge_by_standard("VPTAT_Voltage", mv)
    return mv

def VCM_Voltage():
    """VCM 电压测试。"""
    mv = _voltage_test("VCM电压", addr=20, index=9, write_data=[0x00, 0x00, 0x01, 0xE0])
    _judge_by_standard("VCM_Voltage", mv)
    return mv
def TIA_OUT1_Voltage():
    """TIA_OUT1 电压测试。"""
    mv = _voltage_test("TIA_OUT1电压", addr=20, index=9, write_data=[0x00, 0x00, 0x01, 0x37])
    _judge_by_standard("TIA_OUT1_Voltage", mv)
    return mv

def TIA_OUT2_Voltage():
    """TIA_OUT2 电压测试。"""
    mv = _voltage_test("TIA_OUT2电压", addr=20, index=9, write_data=[0x00, 0x00, 0x01, 0x77])
    _judge_by_standard("TIA_OUT2_Voltage", mv)
    return mv

def TIA_OUT3_Voltage():
    """TIA_OUT3 电压测试。"""
    mv =_voltage_test("TIA_OUT3电压", addr=20, index=9, write_data=[0x00, 0x00, 0x01, 0xB7])
    _judge_by_standard("TIA_OUT3_Voltage", mv)
    return mv

def LDO15_Voltage():
    """LDO15 电压测试。"""
    mv = _voltage_test("LDO15电压", addr=20, index=9, write_data=[0x00, 0x00, 0x02, 0x20])
    _judge_by_standard("LDO15_Voltage", mv)
    return mv

def LDO_3P3_Voltage():
    """LDO_3P3 电压测试。"""
    mv = _voltage_test("LDO_3P3电压", addr=20, index=9, write_data=[0x00, 0x00, 0x02, 0x60])
    _judge_by_standard("LDO_3P3_Voltage", mv)
    return mv

def VDD2_Voltage():
    """VDD2 电压测试。"""
    mv = _voltage_test("VDD2电压", addr=20, index=9, write_data=[0x00, 0x00, 0x02, 0xA0])
    _judge_by_standard("VDD2_Voltage", mv)
    return mv

def ADC_VREF_Voltage():
    """ADC_VREF 电压测试：测电压 → 判定；不通过时按两点斜率估算并扫描 8 位修调值。

    修调流程：
      1. 写修调值 0 读电压 V1，写修调值 10 读电压 V2，得到每步电压变化；
      2. 按 (2000 - V1) / 每步 估算使电压到 2000 mV 的修调值并实测；
      3. 实测仍在判定范围外时，在估算值 ±10 内扫描取最优，写回并再判定。

    RW_VREF_TRIM 位于 [20:4] 的 bit[7:0]，与 VBGTRIM / RW_OSC_24M_TRIM 共用寄存器，
    写入采用读-改-写，不会破坏其它字段。
    """
    print("\n🚀 执行 ADC_VREF 电压修调测试 ...")
    _confirm_connection("请确定万用表连接WO")

    # TEST_ANA<3:0> = 1011（ADC_VREF 通道）；原 00 00 02 A0 是 VDD2 的 1010
    mv = _voltage_test("ADC_VREF电压", addr=20, index=9, write_data=[0x00, 0x00, 0x02, 0xE0])
    time.sleep(0.5)

    if mv is None:
        print("❌ ADC_VREF 电压测量失败，跳过本次测试")
        return None

    i2c = _get_i2c()
    cur = _vref_trim_read(i2c)
    if cur is None:
        print(" 读取 RW_VREF_TRIM 失败，跳过本次测试")
        return None
    print(f"   当前修调值 = {cur}")

    passed = _judge_vref(mv, extra_note=f"修调 {cur}")
    if passed is True:
        print(f"ADC_VREF 测试通过, 测量值: {mv:.3f} mV, 修调值为: {cur}, 无需修调")
        print("✅ ADC_VREF 测试完成")
        return mv

    # ---- 步骤 1：两点斜率（修调值 0 与 10 各测一次）----
    tilt = {}
    for trim in (VREF_PROBE_LO, VREF_PROBE_HI):
        if not _vref_trim_write(i2c, trim):
            print(f"❌ 写入修调值 {trim} 失败（回读异常），跳过修调")
            _vref_trim_write(i2c, cur)
            return mv
        time.sleep(0.5)
        vol = instr.dmm_measure_single_voltage()
        if vol is None:
            print(f"❌ 修调值 {trim} 电压测量失败，写回原修调值")
            _vref_trim_write(i2c, cur)
            return mv
        tilt[trim] = vol
        print(f"   修调值={trim}: 电压={vol:.3f} mV")

    span = VREF_PROBE_HI - VREF_PROBE_LO
    step = (tilt[VREF_PROBE_HI] - tilt[VREF_PROBE_LO]) / span
    print(f"   斜率: 修调 {VREF_PROBE_LO}->{VREF_PROBE_HI} 时 "
          f"{tilt[VREF_PROBE_LO]:.3f}->{tilt[VREF_PROBE_HI]:.3f} mV，"
          f"每步 {step:.3f} mV")

    # ---- 步骤 2：估算修调值并实测 ----
    if abs(step) < 1e-6:
        trim_target = cur
        print("   斜率过小，无法估算，保持当前修调值")
    else:
        trim_target = VREF_PROBE_LO + round(
            (ADC_VREF_TARGET_MV - tilt[VREF_PROBE_LO]) / step)
        trim_target = max(0, min(VREF_TRIM_MAX, trim_target))
    print(f"   目标 {ADC_VREF_TARGET_MV:.0f} mV，估算修调值 {trim_target}")

    if not _vref_trim_write(i2c, trim_target):
        print(f"❌ 写入估算修调值 {trim_target} 失败，跳过修调")
        _vref_trim_write(i2c, cur)
        return mv
    time.sleep(0.5)
    est_mv = instr.dmm_measure_single_voltage()
    if est_mv is None:
        print(f"❌ 估算修调值 {trim_target} 电压测量失败，写回原修调值")
        _vref_trim_write(i2c, cur)
        return mv

    lo_mv, hi_mv = ADC_VREF_LIMITS_MV
    best_trim, best_mv = trim_target, est_mv
    print(f"   估算值实测: 电压={est_mv:.3f} mV, "
          f"误差={abs(est_mv - ADC_VREF_TARGET_MV):.3f} mV")

    if lo_mv <= est_mv <= hi_mv:
        print(f"   ✅ 估算值已落入判定范围 {lo_mv:.0f}~{hi_mv:.0f} mV，无需扫描")
    else:
        # ---- 步骤 3：估算值不达标，在 ±10 内扫描 ----
        lo = max(0, trim_target - VREF_SCAN_SPAN)
        hi = min(VREF_TRIM_MAX, trim_target + VREF_SCAN_SPAN)
        print(f"   估算值不在判定范围，扫描修调值 {lo}~{hi} ...")

        min_err = abs(est_mv - ADC_VREF_TARGET_MV)
        for test_trim in range(lo, hi + 1):
            if test_trim == trim_target:
                continue                      # 估算值已测过
            if not _vref_trim_write(i2c, test_trim):
                print(f"   修调值={test_trim}: 写入失败（回读异常），跳过")
                continue
            time.sleep(0.5)
            # 每次修改 TRIM 后立即复测电压
            test_mv = instr.dmm_measure_single_voltage()
            if test_mv is None:
                print(f"   修调值={test_trim}: 电压测量失败，跳过")
                continue
            test_err = abs(test_mv - ADC_VREF_TARGET_MV)
            mark = ""
            if test_err < min_err:
                best_trim, best_mv, min_err = test_trim, test_mv, test_err
                mark = "   ← 当前最优"
            print(f"   修调值={test_trim}: 电压={test_mv:.3f} mV, "
                  f"误差={test_err:.3f} mV{mark}")

    # ---- 写回最优修调值并复测、判定 ----
    if not _vref_trim_write(i2c, best_trim):
        print(f"❌ ADC_VREF 修调失败：写回修调值 {best_trim} 时回读异常")
        return None
    time.sleep(0.5)
    best_mv = instr.dmm_measure_single_voltage()
    if best_mv is None:
        print(f"❌ ADC_VREF 修调失败：写回修调值 {best_trim} 后测量失败")
        return None

    passed = _judge_vref(best_mv, extra_note=f"修调 {best_trim}")
    result = "成功" if passed is True else "失败"
    print(f"ADC_VREF 修调{result}, 测量值: {best_mv:.3f} mV, 修调值为: {best_trim}")

    # 修调完成后，把 RW_VREF_TRIM 同步到 MTP 配置表（[20:4] ↔ MTP phys 440）
    _sync_vref_trim_to_table(best_trim)

    print("✅ ADC_VREF 测试完成")
    return best_mv

def ADC_IN_Voltage():
    """ADC_IN 测试（菜单 [14]）：UART 控制 DAC 输出电压，ADC 采样并列表。

    流程（参考 OCD2810 的 DAC/ADC 测试）：
      1. 写 [20:9] = 00 00 03 20 进入 ADC_IN 测试模式（TEST_ANA=1100）；
      2. 交互式选择扫描范围（上限固定 2000 mV）与步进（默认 10 mV）；
      3. 每点通过 UART 下发 `spi_cmd_set_val mV <V>`，万用表实测 WO 电压；
      4. 若已配置 ADC 寄存器，触发一次转换并读取 ADC4_DR<13:0>，
         按 `Vadc = code * ADC_VREF / 4096` 换算电压；
      5. 逐点打印表格（设定 / 实测 / DAC 偏差 / ADC 码值 / ADC 电压 / ADC 误差），
         结束后 DAC 回 0，可选导出 xlsx。

    判定：以 DAC 输出最大偏差 ≤ ADC_IN_DAC_TOL_MV 作为线性度判定；
    ADC 列仅在 ADCBSY_ADDR / ADC4_DR_ADDR 配置后有效（否则显示 --）。
    """
    print("\n🚀 执行 ADC_IN（DAC 输出 + ADC 采样）测试 ...")
    _confirm_connection("请确定万用表连接WO")

    # ---- 扫描参数（可交互修改）----
    print(f"   DAC 输出电压上限 {ADC_IN_VMAX_MV} mV（规格 WO 输入不超过 2 V）")
    start = _ask_int("起始电压 mV", ADC_IN_DAC_START_MV, 0, ADC_IN_VMAX_MV)
    stop = _ask_int("结束电压 mV", ADC_IN_VMAX_MV, 0, ADC_IN_VMAX_MV)
    step = _ask_int("步进 mV", ADC_IN_DAC_STEP_MV, 1, ADC_IN_VMAX_MV)
    if start > stop:
        start, stop = stop, start
    points = list(range(start, stop + 1, step))
    if not points:
        print("❌ 扫描点为空，测试终止")
        return None

    adc_on = _adc_available()
    print(f"   扫描 {points[0]} ~ {points[-1]} mV，步进 {step} mV，共 {len(points)} 点，"
          f"单点容差 ±{ADC_IN_DAC_TOL_MV:.1f} mV")
    if adc_on:
        print(f"   ADC 采样已启用：ADC4_DR mask=0x{ADC4_DR_MASK:X}，"
              f"VREF={ADC_IN_VREF_MV:.0f} mV")
    else:
        print("   ⚠️ 未配置 ADCBSY / ADC4_DR 地址，仅做 DAC 输出测量（ADC 列显示 --）")

    i2c = _get_i2c()
    execute_write(i2c, addr=20, index=9, write_data=[0x00, 0x00, 0x03, 0x20])
    _log_read(20, 9)
    time.sleep(0.2)

    lsb_mv = ADC_IN_VREF_MV / 4096.0
    rows, max_dac_err, max_adc_err = [], 0.0, 0.0

    print("\n   " + "-" * 84)
    print(f"   {'设定(mV)':>9} {'实测(mV)':>10} {'DAC偏差':>9} "
          f"{'ADC码值':>8} {'ADC电压(mV)':>12} {'ADC误差(mV)':>12} {'误差(LSB)':>10}")
    print("   " + "-" * 84)

    for vin in points:
        try:
            instr.serial_send_and_receive(f"spi_cmd_set_val mV {int(vin)}\r\n")
        except Exception as e:
            print(f"   ❌ 设置 DAC {vin} mV 失败：{e}")
            rows.append([vin, None, None, None, None, None, None])
            continue
        time.sleep(0.05)

        meas = instr.dmm_measure_single_voltage()
        if meas is None:
            print(f"   ❌ 设定 {vin:4d} mV：万用表测量失败")
            rows.append([vin, None, None, None, None, None, None])
            continue
        dac_err = meas - vin
        max_dac_err = max(max_dac_err, abs(dac_err))

        code = _adc_sample(i2c) if adc_on else None
        if code is None:
            v_adc = adc_err = adc_lsb = None
            code_txt = adc_txt = err_txt = lsb_txt = "--"
        else:
            v_adc = code * lsb_mv
            adc_err = v_adc - meas          # ADC 转换电压与万用表实测的差
            adc_lsb = adc_err / lsb_mv
            max_adc_err = max(max_adc_err, abs(adc_err))
            code_txt = f"{code}"
            adc_txt = f"{v_adc:.3f}"
            err_txt = f"{adc_err:+.3f}"
            lsb_txt = f"{adc_lsb:+.2f}"

        rows.append([vin, round(meas, 3), round(dac_err, 3), code,
                     None if v_adc is None else round(v_adc, 3),
                     None if adc_err is None else round(adc_err, 3),
                     None if adc_lsb is None else round(adc_lsb, 3)])
        print(f"   {vin:9d} {meas:10.3f} {dac_err:+9.3f} "
              f"{code_txt:>8} {adc_txt:>12} {err_txt:>12} {lsb_txt:>10}")

    print("   " + "-" * 84)

    # DAC 回 0，避免测试结束后 WO 仍有电压
    try:
        instr.serial_send_and_receive("spi_cmd_set_val mV 0\r\n")
    except Exception:
        pass

    valid = [r for r in rows if r[1] is not None]
    if not valid:
        print("❌ ADC_IN 测试失败：无有效测量点")
        return None

    print(f"\n   有效点 {len(valid)}/{len(points)}，DAC 最大偏差 {max_dac_err:.3f} mV")
    if adc_on:
        adc_rows = [r for r in rows if r[5] is not None]
        if adc_rows:
            print(f"   ADC 有效点 {len(adc_rows)}/{len(rows)}，"
                  f"ADC 最大误差 {max_adc_err:.3f} mV（{max_adc_err / lsb_mv:.2f} LSB）")

    passed = max_dac_err <= ADC_IN_DAC_TOL_MV
    print(f"{'✅' if passed else '❌'} ADC_IN {'pass' if passed else 'failed'}："
          f"DAC 线性度容差 ±{ADC_IN_DAC_TOL_MV:.1f} mV，实测最大偏差 {max_dac_err:.3f} mV")

    # 可选导出
    try:
        choice = input("\n   是否导出测量表格到 Save_Results? (y/N): ").strip().lower()
    except (EOFError, KeyboardInterrupt):
        choice = ""
    if choice == "y":
        name = time.strftime("ADC_IN_测量数据_%Y%m%d_%H%M%S.xlsx")
        saved = _export_adc_table(rows, name)
        if saved:
            print(f"   ✅ 已导出：{saved}")

    print("✅ ADC_IN 测试完成")
    return max_dac_err

def UVLO_Voltage():
    """UVLO 电压测试。"""
    mv = _voltage_test("UVLO电压", addr=20, index=9, write_data=[0x00, 0x00, 0x00, 0x01])
    _judge_by_standard("UVLO_Voltage", mv)
    return mv

def FR_Voltage():
    """FR 阈值电压测试。"""
    mv = _voltage_test("FR阈值电压", addr=20, index=9, write_data=[0x00, 0x00, 0x00, 0x11])
    _judge_by_standard("FR_Voltage", mv)
    return mv

def PWM_IN_Voltage():
    """PWM 阈值电压测试。"""
    mv = _voltage_test("PWM阈值电压", addr=20, index=9, write_data=[0x00, 0x00, 0x08, 0x00])
    _judge_by_standard("PWM_IN_Voltage", mv)
    return mv

# ---------------------- 电流类 ----------------------

def HOCP_Current():
    """HOCP U相和V相 过流保护测试。"""
    return _current_test("HOCP U相和V相", addr=20, index=9, write_data=[0x00, 0x00, 0x03, 0x75])


def LOCP_Current():
    """LOCP U相和V相 过流保护测试。"""
    return _current_test("LOCP U相和V相", addr=20, index=9, write_data=[0x00, 0x00, 0x03, 0x77])


def IBN_5U_Current():
    """IBN_5U 电流测试。"""
    return _current_test("IBN_5U电流", addr=20, index=9, write_data=[0x00, 0x00, 0x03, 0xA0])


def HOCP_W_Current():
    """HOCP W相 过流保护测试。"""
    return _current_test("HOCP W相", addr=20, index=9, write_data=[0x00, 0x00, 0x03, 0xF5])


def LOCP_W_Current():
    """LOCP W相 过流保护测试。"""
    return _current_test("LOCP W相", addr=20, index=9, write_data=[0x00, 0x00, 0x03, 0xF7])


# ---------------------- 频率 / 温度 / 其他 ----------------------

def OSC_P():
    """OSC 频率测试：保持示波器连接测频 → 判定；不通过时逐点扫描 6 位修调值。

    每次修改 TRIM 后立即重新测量频率（不重复 Auto Scale，保持连接稳定）。
    """
    print("\n🚀 执行 OSC 频率修调测试 ...")
    _confirm_connection("请确定示波器连接UO")

    i2c = _get_i2c()
    execute_write(i2c, addr=20, index=9, write_data=[0x00, 0x00, 0x00, 0x03])
    _log_read(20, 9)
    time.sleep(0.1)

    cur = _osc_trim_read(i2c)
    if cur is None:
        print(" 读取 OSC_TRIM 失败，跳过本次测试")
        return None
    print(f"   当前修调值 = {cur}")

    # 保持示波器连接：确认会话可用，仅在开始时做一次 Auto Scale
    if not _osc_ensure_connected():
        print("❌ 示波器未连接，跳过本次测试")
        return None
    instr.osc_auto_set()
    time.sleep(0.5)

    freq, _period, _duty = _osc_measure_freq("CH1")
    if freq is None:
        print("❌ OSC 频率测量失败，跳过本次测试")
        return None
    print(f"   频率 = {freq:.3f} Hz")

    passed = _judge_osc(freq, extra_note=f"修调 {cur}")
    if passed is True:
        print(f"OSC 测试通过, 测量值: {freq:.3f} Hz, 修调值为: {cur}, 无需修调")
        print("✅ OSC频率 完成")
        return freq

    # 斜率探测：改一次修调值后立即复测（cur 已在最大值时往下探）
    probe = cur + 1 if cur < OSC_TRIM_MAX else cur - 1
    if not _osc_trim_write(i2c, probe):
        print("❌ 写入探测修调值失败，跳过修调")
        return freq
    time.sleep(0.5)
    f_probe, _p, _d = _osc_measure_freq("CH1")
    if f_probe is None:
        print("❌ 斜率探测测量失败，写回原修调值")
        _osc_trim_write(i2c, cur)
        return freq

    step = (f_probe - freq) / (probe - cur)
    print(f"   斜率探测: 修调 {cur}->{probe} 时 "
          f"{freq:.3f}->{f_probe:.3f} Hz，每步 {step:.3f} Hz")

    if abs(step) < 1e-6:
        trim_target = cur
        print("   斜率过小，保持当前修调值")
    else:
        trim_target = cur + round((OSC_TARGET_HZ - freq) / step)
        trim_target = max(0, min(OSC_TRIM_MAX, trim_target))

    lo = max(0, trim_target - OSC_SCAN_SPAN)
    hi = min(OSC_TRIM_MAX, trim_target + OSC_SCAN_SPAN)
    print(f"   目标 {OSC_TARGET_HZ:.0f} Hz，估算修调值 {trim_target}，"
          f"扫描范围 {lo}~{hi} ...")

    best_trim, min_err = cur, abs(freq - OSC_TARGET_HZ)
    for test_trim in range(lo, hi + 1):
        if not _osc_trim_write(i2c, test_trim):
            print(f"   修调值={test_trim}: 写入失败（回读异常），跳过")
            continue
        time.sleep(0.5)
        # 每次修改 TRIM 后立即复测频率
        t_f, _p, _d = _osc_measure_freq("CH1")
        if t_f is None:
            print(f"   修调值={test_trim}: 频率测量失败，跳过")
            continue
        test_err = abs(t_f - OSC_TARGET_HZ)
        mark = ""
        if test_err < min_err:
            best_trim, min_err = test_trim, test_err
            mark = "   ← 当前最优"
        print(f"   修调值={test_trim}: 频率={t_f:.3f} Hz, "
              f"误差={test_err:.3f} Hz{mark}")

    # 写回最优修调值并立即复测、判定
    if not _osc_trim_write(i2c, best_trim):
        print(f"❌ OSC 修调失败：写回修调值 {best_trim} 时回读异常")
        return None
    time.sleep(0.5)
    best_f, _p, _d = _osc_measure_freq("CH1")
    if best_f is None:
        print(f"❌ OSC 修调失败：写回修调值 {best_trim} 后测量失败")
        return None

    passed = _judge_osc(best_f, extra_note=f"修调 {best_trim}")
    result = "成功" if passed is True else "失败"
    print(f"OSC 修调{result}, 测量值: {best_f:.3f} Hz, 修调值为: {best_trim}")

    # 修调完成后，把 RW_OSC_24M_TRIM 同步到 MTP 配置表（[20:4] ↔ MTP phys 440）
    _sync_osc_trim_to_table(best_trim)

    print("✅ OSC频率 完成")
    return best_f


def TSD_TEMP():
    """TSD 温度测试。"""
    print("\n🚀 执行 TSD温度 ...")
    i2c = _get_i2c()
    # TODO: 按手册填写温度读取的 addr / index
    write_data = [0xAA, 0xBB, 0xCC, 0xDD]
    print(f"写入 [20:15] = {' '.join(f'{b:02X}' for b in write_data)}")
    execute_write(i2c, addr=20, index=15, write_data=write_data)
    _log_read(0, 0)
    print("✅ TSD温度 完成")


def COMP0_hy():
    """COMP0 迟滞测试。"""
    print("\n🚀 执行 COMP0迟滞 ...")
    i2c = _get_i2c()
    # TODO: 按手册实现迟滞扫描（调节输入电压 → 读比较器输出 → 计算迟滞）
    _log_read(0, 0)
    print("✅ COMP0迟滞 完成")


def COMP1_hy():
    """COMP1 迟滞测试。"""
    print("\n🚀 执行 COMP1迟滞 ...")
    i2c = _get_i2c()
    # TODO: 按手册实现迟滞扫描
    _log_read(0, 0)
    print("✅ COMP1迟滞 完成")


def COMP2_hy():
    """COMP2 迟滞测试。"""
    print("\n🚀 执行 COMP2迟滞 ...")
    i2c = _get_i2c()
    # TODO: 按手册实现迟滞扫描
    _log_read(0, 0)
    print("✅ COMP2迟滞 完成")


def OUTPUT_leakage_test():
    """输出漏电测试（电流测量）。"""
    print("\n🚀 执行 输出漏电测试 ...")
    ma = instr.dmm_measure_single_current()
    if ma is not None:
        print(f"   漏电流 = {ma} mA")
    else:
        print("   漏电流测量失败")
    print("✅ 输出漏电测试 完成")
    return ma


def UP_output_impedance():
    """上管输出阻抗测试。"""
    print("\n🚀 执行 上管输出阻抗 ...")
    ohm = instr.dmm_measure_single_resistance()
    if ohm is not None:
        print(f"   阻抗 = {ohm} Ω")
    else:
        print("   阻抗测量失败")
    print("✅ 上管输出阻抗 完成")
    return ohm


def down_output_impedance():
    """下管输出阻抗测试。"""
    print("\n🚀 执行 下管输出阻抗 ...")
    ohm = instr.dmm_measure_single_resistance()
    if ohm is not None:
        print(f"   阻抗 = {ohm} Ω")
    else:
        print("   阻抗测量失败")
    print("✅ 下管输出阻抗 完成")
    return ohm

# =====================================================================
# MTP 编程（终端命令循环，基于 instrument.IIC.mtp_ops.MTPController）
#
# MTP 按字节编址，共 512 字节（0~511），每 32 字节为一个扇区（16 个扇区）。
# MTPController 负责把字节地址映射成 (ADDR, INDEX) 寄存器寻址并封装指令时序；
# 这里仿 IICtool 的「MTP测设模式」页，把按钮操作搬到终端命令循环：读字节 /
# 编程字节 / 扇区擦除 / 全片擦除 / 整扇区填值，操作数由操作员在终端逐条输入。
#
# 注意：
#   1. MTP 只能把 bit 由 1 写成 0，编程前目标字节必须已擦除（空白）。
#   2. 扇区擦除会清掉该扇区全部 32 字节（含其中已烧录的数据），执行前需确认。
#   3. 数据字节按 HEX 输入（与 IICtool 的 Byte Data(HEX) 一致），地址按十进制。
# =====================================================================

# 每个字节写入遍数（与 mtp_batch_ops.execute_mtp_burn 的两遍烧录一致）
MTP_PROGRAM_PASSES = 2

# 字节间延时（秒）：给芯片编程留出时间
MTP_PROGRAM_BYTE_DELAY = 0.01


def _mtp_logger(verbose=False):
    """MTPController 的日志回调。

    mtp_ops.program_byte 会逐字节打印，默认过滤掉这类流水日志，
    只保留擦除、进/出测试模式、异常等关键信息。
    """
    if verbose:
        return print

    def _log(msg):
        if str(msg).startswith("MTP: 烧录地址"):
            return
        print(msg)

    return _log


def _get_mtp(verbose=False):
    """构造 MTPController，把读写回调接到 iic_ops 的 execute_write/execute_read。"""
    i2c = _get_i2c()

    def _write(addr, index, write_data, suppress_log=False):
        execute_write(i2c, addr, index, write_data, suppress_log=suppress_log)

    def _read(addr, index):
        return execute_read(i2c, addr, index)

    return MTPController(_write, _read, log_callback=_mtp_logger(verbose))


def _mtp_parse_int(text, name, lo, hi):
    """解析十进制地址/编号（允许 0x 前缀），并做范围检查。"""
    s = str(text).strip()
    val = int(s, 16) if s.lower().startswith("0x") else int(s, 10)
    if not lo <= val <= hi:
        raise ValueError(f"{name} {val} 超出 {lo}~{hi}")
    return val


def _mtp_parse_byte(text, name="数据"):
    """解析 1~2 位 HEX 字节（允许 0x 前缀）。"""
    s = str(text).strip()
    if s.lower().startswith("0x"):
        s = s[2:]
    return IICLogic.parse_hex_byte(s, name)


def _mtp_read_byte_value(mtp, addr):
    """读取一个 MTP 字节，返回 (ok, 字节值或错误信息)。"""
    rd = mtp.read_byte(addr)
    if not rd or len(rd) < 4:
        return False, "回读无有效返回"
    return True, rd[3]


def _mtp_program_one(mtp, addr, val):
    """编程一个字节（写 MTP_PROGRAM_PASSES 遍）并回读校验，返回 (ok, 信息)。"""
    for _ in range(MTP_PROGRAM_PASSES):
        mtp.program_byte(addr, val)
        time.sleep(MTP_PROGRAM_BYTE_DELAY)
    ok, res = _mtp_read_byte_value(mtp, addr)
    if not ok:
        return False, f"地址 {addr} 编程后回读失败（{res}）"
    if res != val:
        return False, f"地址 {addr} 校验不一致：写入 0x{val:02X}，回读 0x{res:02X}"
    return True, f"地址 {addr} = 0x{val:02X}"


# ---------------------- 命令处理 ----------------------
# 每个处理函数签名为 (mtp, args) -> (ok, 提示信息)

def _mtp_cmd_program(mtp, args):
    """p <起始地址> <HEX字节> [HEX字节 ...] —— 编程 1~N 个连续字节并回读校验"""
    if len(args) < 2:
        raise ValueError("用法: p <起始地址> <HEX字节> [HEX字节 ...]，如 p 100 5A 00 FF")
    start = _mtp_parse_int(args[0], "起始地址", 0, 511)
    vals = [_mtp_parse_byte(a) for a in args[1:]]
    end = start + len(vals) - 1
    if end > 511:
        raise ValueError(f"结束地址 {end} 超出 0~511（起始 {start}，共 {len(vals)} 字节）")

    failed = []
    for i, val in enumerate(vals):
        ok, msg = _mtp_program_one(mtp, start + i, val)
        if not ok:
            failed.append(start + i)
            print(f"     ❌ {msg}")
        elif len(vals) <= 8:
            print(f"     ✅ {msg}")
        elif (i + 1) % 8 == 0 or i == len(vals) - 1:
            print(f"     编程进度 {i + 1}/{len(vals)}，失败 {len(failed)} 项")

    if failed:
        return False, f"地址 {start}~{end} 共 {len(vals)} 字节，{len(failed)} 字节失败：{failed}"
    return True, f"地址 {start}~{end} 共 {len(vals)} 字节编程完成"


def _mtp_cmd_read(mtp, args):
    """r <起始地址> [个数] —— 读取 1~512 个连续字节（HEX 输出，每行 16 字节）"""
    if not args:
        raise ValueError("用法: r <起始地址> [个数]，如 r 100 或 r 100 32")
    start = _mtp_parse_int(args[0], "起始地址", 0, 511)
    count = _mtp_parse_int(args[1], "个数", 1, 512) if len(args) > 1 else 1
    if start + count - 1 > 511:
        raise ValueError(f"结束地址 {start + count - 1} 超出 0~511")

    vals, failed = [], []
    for i in range(count):
        ok, res = _mtp_read_byte_value(mtp, start + i)
        if ok:
            vals.append(res)
        else:
            vals.append(None)
            failed.append(start + i)

    if count == 1:
        print(f"     地址 {start} = " + ("回读失败" if vals[0] is None else f"0x{vals[0]:02X}"))
    else:
        for base in range(0, count, 16):
            chunk = vals[base:base + 16]
            cells = ' '.join("--" if v is None else f"{v:02X}" for v in chunk)
            print(f"     {start + base:>3d}: {cells}")

    if failed:
        return False, f"读取 {count} 字节，{len(failed)} 个地址回读失败：{failed[:10]}"
    return True, f"读取地址 {start}~{start + count - 1} 共 {count} 字节完成"


def _mtp_cmd_erase_sector(mtp, args):
    """e <扇区 0~15> —— 擦除整个扇区（32 字节，需确认）"""
    if not args:
        raise ValueError("用法: e <扇区 0~15>，如 e 3（对应字节 96~127）")
    sector = _mtp_parse_int(args[0], "扇区", 0, 15)
    start = sector * 32
    print(f"     扇区 {sector} = 字节 {start}~{start + 31}，其内已烧录数据将被清除")
    if input("     确认擦除？(y/N): ").strip().lower() != "y":
        return False, "已取消扇区擦除"
    mtp.erase_sector(sector)
    return True, f"已下发扇区 {sector} 擦除指令"


def _mtp_cmd_erase_chip(mtp, args):
    """ec —— 全片擦除（清空全部 512 字节，需确认）"""
    if input("     确认全片擦除 MTP（清空全部 512 字节）？(y/N): ").strip().lower() != "y":
        return False, "已取消全片擦除"
    mtp.erase_chip()
    return True, "已下发 MTP 全片擦除指令"


def _mtp_cmd_fill_sector(mtp, args):
    """s <扇区 0~15> <HEX字节> —— 整扇区 32 字节写同一值（不擦除）

    对应 mtp_batch_ops.MTPBatchLogic.write_mtp_sector_data()。
    """
    if len(args) < 2:
        raise ValueError("用法: s <扇区 0~15> <HEX字节>，如 s 3 FF")
    sector = _mtp_parse_int(args[0], "扇区", 0, 15)
    val = _mtp_parse_byte(args[1])
    start = sector * 32
    print(f"     扇区 {sector} = 字节 {start}~{start + 31}，统一写入 0x{val:02X}")

    failed = []
    for i, a in enumerate(range(start, start + 32), start=1):
        ok, msg = _mtp_program_one(mtp, a, val)
        if not ok:
            failed.append(a)
            print(f"     ❌ {msg}")
        elif i % 8 == 0 or i == 32:
            print(f"     填值进度 {i}/32，失败 {len(failed)} 项")

    if failed:
        return False, f"扇区 {sector} 填值有 {len(failed)} 字节失败：{failed}"
    return True, f"扇区 {sector} 共 32 字节全部写入 0x{val:02X} 并校验通过"


def _mtp_cmd_crc(mtp, args):
    """crc <起始地址> <结束地址> [期望HEX] —— 读回字节算 CRC32 并比对（只比对，不烧录）"""
    if len(args) < 2:
        raise ValueError("用法: crc <起始地址> <结束地址> [期望HEX]，如 crc 4 506 0F348CFA")
    lo = _mtp_parse_int(args[0], "起始地址", 0, 511)
    hi = _mtp_parse_int(args[1], "结束地址", 0, 511)
    if lo > hi:
        raise ValueError(f"起始地址 {lo} 不能大于结束地址 {hi}")

    data, failed = bytearray(), []
    for a in range(lo, hi + 1):
        ok, res = _mtp_read_byte_value(mtp, a)
        if ok:
            data.append(res)
        else:
            failed.append(a)
    if failed:
        return False, f"{len(failed)} 个地址回读失败，无法计算 CRC：{failed[:10]}"

    crc = mtp.calculate_crc32(bytes(data))
    if len(args) < 3:
        return True, f"CRC32(地址 {lo}~{hi}，{len(data)} 字节) = 0x{crc:08X}"

    s = args[2].strip()
    if s.lower().startswith("0x"):
        s = s[2:]
    if not re.fullmatch(r'[0-9A-Fa-f]{1,8}', s):
        raise ValueError(f"期望 CRC 必须是 1~8 位 HEX：{args[2]}")
    expect = int(s, 16)
    if expect == crc:
        return True, f"CRC32(地址 {lo}~{hi}) = 0x{crc:08X}，与期望值一致"
    return False, (f"CRC32(地址 {lo}~{hi}) = 0x{crc:08X}，"
                   f"与期望值 0x{expect:08X} 不一致")


def _mtp_cmd_open(mtp, args):
    """o —— Open MTP EN（进入 MTP 测试模式）"""
    mtp.open_mtp_en()
    return True, "已下发 Open MTP EN"


def _mtp_cmd_close(mtp, args):
    """c —— Close MTP EN（退出 MTP 测试模式，此后编程/擦除不会生效）"""
    mtp.close_mtp_en()
    return True, "已下发 Close MTP EN"


# MTP Load 固定写入的寄存器与数据
MTP_LOAD_ADDR = 25
MTP_LOAD_INDEX = 5
MTP_LOAD_DATA = [0x5A, 0x5A, 0x5A, 0x5A]

# 写入该数据的寄存器不回读（MTP 测试模式/加载寄存器）
NO_READBACK_DATA = [0x5A, 0x5A, 0x5A, 0x5A]


def _mtp_cmd_load(mtp, args):
    """load —— MTP Load：写 [25:5] = 5A5A5A5A（不回读）

    固定写 addr=25, index=5, data=5A 5A 5A 5A，用于 MTP 上电/加载。
    5A5A5A5A 属于不回读的寄存器，写完不校验。不带参数。
    """
    addr, index, data = MTP_LOAD_ADDR, MTP_LOAD_INDEX, MTP_LOAD_DATA
    if args:
        return False, "load 命令不带参数（固定写 [25:5] = 5A5A5A5A）"

    mtp.write_i2c(addr, index, list(data))
    time.sleep(MTP_PROGRAM_BYTE_DELAY)

    data_hex = ' '.join(f'{b:02X}' for b in data)
    return True, f"MTP Load 完成：[{addr}:{index}] = {data_hex}（不回读）"


# 命令表: {命令: (说明, 处理函数)}
_MTP_COMMANDS = {
    "p": ("编程 1~N 个连续字节并回读校验", _mtp_cmd_program),
    "r": ("读取 1~512 个连续字节", _mtp_cmd_read),
    "e": ("擦除整个扇区（32 字节）", _mtp_cmd_erase_sector),
    "ec": ("全片擦除（512 字节）", _mtp_cmd_erase_chip),
    "s": ("整扇区 32 字节写同一值（不擦除）", _mtp_cmd_fill_sector),
    "crc": ("读回算 CRC32 并与期望值比对（不烧录）", _mtp_cmd_crc),
    "o": ("Open MTP EN", _mtp_cmd_open),
    "c": ("Close MTP EN", _mtp_cmd_close),
    "load": ("MTP Load：写 [25:5] = 5A5A5A5A（不回读）", _mtp_cmd_load),
}


def _mtp_print_help():
    """打印 MTP 命令帮助（从命令表生成，避免与实际命令脱节）。"""
    print("   ── MTP 命令（地址十进制，数据 HEX）──")
    for cmd, (desc, _) in _MTP_COMMANDS.items():
        print(f"     {cmd:<4s} {desc}")
    print("     h    显示本帮助")
    print("     q    退出（退出时自动 Close MTP EN）")
    print("   例: p 100 5A 00 FF        → 地址 100/101/102 依次写 5A/00/FF")
    print("       p 100 5A 00 FF 12 34  → 空格或逗号分隔，写多少个字节都行")
    print("       r 100 32 | e 3 | crc 4 506 0F348CFA")
    print("       load                  → MTP Load：[25:5] = 5A 5A 5A 5A（不回读）")
    print("   （IIC 寄存器读写已独立到菜单 [33]）")


def MTP_program():
    """MTP 编程：终端命令循环（仿 IICtool 的「MTP测设模式」页）。

    进入后先 Open MTP EN，之后逐条输入命令操作 MTP（编程/擦除/读回等）；
    输入 q 退出时只 Close MTP EN，不再自动执行 MTP Load
    （需要 [25:5] = 5A5A5A5A 时可在循环内手工输入 load）。
    返回 True 表示本次会话所有命令都成功。
    """
    print("\n🚀 执行 MTP_program（终端命令循环）...")
    print(f"   每个字节写 {MTP_PROGRAM_PASSES} 遍，字节间延时 {MTP_PROGRAM_BYTE_DELAY}s，"
          f"编程后自动回读校验")
    print("   退出命令循环时自动 Close MTP EN（不再自动执行 MTP Load）")
    _mtp_print_help()

    if input("   确认进入 MTP 编程？(y/N): ").strip().lower() != "y":
        print("   ⏭️ 已取消")
        return False

    mtp = _get_mtp()
    mtp.open_mtp_en()          # 进入 MTP 测试模式

    failures = 0
    try:
        while True:
            try:
                raw = input("\nMTP> ").strip()
            except (EOFError, KeyboardInterrupt):
                print()
                break
            if not raw:
                continue

            # 逗号也当分隔符，方便从 Excel/CSV 里直接粘贴一串字节
            parts = raw.replace(',', ' ').split()
            cmd, args = parts[0].lower(), parts[1:]
            if cmd in ("q", "quit", "exit"):
                break
            if cmd in ("h", "?", "help"):
                _mtp_print_help()
                continue

            entry = _MTP_COMMANDS.get(cmd)
            if entry is None:
                print(f"   ⚠️  未知命令 '{cmd}'，输入 h 查看可用命令")
                continue

            try:
                ok, msg = entry[1](mtp, args)
            except Exception as e:
                ok, msg = False, f"{type(e).__name__}: {e}"
            print(f"   {'✅' if ok else '❌'} {msg}")
            if not ok:
                failures += 1
    finally:
        mtp.close_mtp_en()     # 退出时无论成败都关掉 MTP EN

    if failures:
        print(f"\n❌ MTP_program 结束：{failures} 条命令未成功")
    else:
        print("\n✅ MTP_program 结束：全部命令执行成功")
    return failures == 0

def MTP_read():
    """MTP_read测试。"""


# =====================================================================
# IIC 寄存器读写（菜单 [33]，独立于 MTP 命令循环）
#
# 只做普通寄存器读写，不打开/关闭 MTP EN，也不涉及 MTP 地址映射。
# 写法与 TESTMODE_EN() 一致：execute_write 写 4 字节，再 execute_read 回读。
# 约定：写入数据为 5A 5A 5A 5A 时不回读（测试模式/加载寄存器）。
# =====================================================================

def _iic_write_reg(addr, index, data):
    """写寄存器 4 字节；5A5A5A5A 不回读，其余回读校验。返回 (ok, 信息)。"""
    i2c = _get_i2c()
    execute_write(i2c, addr=addr, index=index, write_data=data)
    time.sleep(MTP_PROGRAM_BYTE_DELAY)

    data_hex = ' '.join(f'{b:02X}' for b in data)
    if list(data) == NO_READBACK_DATA:
        return True, f"写入 [{addr}:{index}] = {data_hex}（5A5A5A5A 不回读）"

    rd = execute_read(i2c, addr, index)
    if not rd or len(rd) < 4:
        return False, f"已写入 [{addr}:{index}] = {data_hex}，但回读无有效返回"
    rd_hex = ' '.join(f'{b:02X}' for b in rd[:4])
    if list(rd[:4]) != list(data):
        return False, f"回读不一致：[{addr}:{index}] 写入 {data_hex}，回读 {rd_hex}"
    return True, f"写入 [{addr}:{index}] = {data_hex}（回读一致）"


def _iic_read_reg(addr, index, count=1):
    """读寄存器：同一 ADDR 下从 INDEX 起连续读 count 个。返回 (ok, 信息)。"""
    if index + count - 1 > 63:
        raise ValueError(f"INDEX {index} + {count} 个超出 0~63")
    i2c = _get_i2c()
    lines, failed = [], []
    for i in range(count):
        idx = index + i
        rd = execute_read(i2c, addr, idx)
        if rd and len(rd) >= 4:
            lines.append(f"     [{addr}:{idx:2d}] = {' '.join(f'{b:02X}' for b in rd[:4])}")
        else:
            lines.append(f"     [{addr}:{idx:2d}] = 无有效返回")
            failed.append(idx)
    msg = "\n".join(lines)
    if failed:
        return False, f"读取 {count} 个，{len(failed)} 个失败（INDEX {failed}）\n{msg}"
    return True, msg


def _iic_reg_write_handler(args):
    """w <ADDR> <INDEX> <B31> <B23> <B15> <B7>"""
    if len(args) != 6:
        raise ValueError("用法: w <ADDR> <INDEX> <B31> <B23> <B15> <B7>，"
                         "如 w 20 15 5A 5A 5A 5A")
    addr = _mtp_parse_int(args[0], "ADDR", 0, 31)
    index = _mtp_parse_int(args[1], "INDEX", 0, 63)
    data = [_mtp_parse_byte(a) for a in args[2:6]]
    return _iic_write_reg(addr, index, data)


def _iic_reg_read_handler(args):
    """rd <ADDR> <INDEX> [个数]"""
    if len(args) not in (2, 3):
        raise ValueError("用法: rd <ADDR> <INDEX> [个数]，如 rd 20 15 或 rd 20 15 4")
    addr = _mtp_parse_int(args[0], "ADDR", 0, 31)
    index = _mtp_parse_int(args[1], "INDEX", 0, 63)
    count = _mtp_parse_int(args[2], "个数", 1, 64) if len(args) == 3 else 1
    return _iic_read_reg(addr, index, count)


_IIC_COMMANDS = {
    "w": ("写寄存器 4 字节并回读（5A5A5A5A 不回读）：w <ADDR> <INDEX> B31 B23 B15 B7",
          _iic_reg_write_handler),
    "rd": ("读寄存器：rd <ADDR> <INDEX> [个数]", _iic_reg_read_handler),
}


def _iic_print_help():
    print("   ── IIC 寄存器命令（地址/索引十进制，数据 HEX）──")
    for cmd, (desc, _) in _IIC_COMMANDS.items():
        print(f"     {cmd:<3s} {desc}")
    print("     h   显示本帮助")
    print("     q   退出")
    print("   例: w 20 15 5A 5A 5A 5A   → 写 [20:15]（5A5A5A5A 不回读）")
    print("       w 20 9 00 00 00 20    → 写 [20:9] 并回读校验")
    print("       rd 20 15              → 读 [20:15]")
    print("       rd 20 15 4            → 从 [20:15] 起连续读 4 个 INDEX")


def IIC_reg_rw():
    """IIC 寄存器读写（菜单 [33]）。

    独立于 MTP：不打开 MTP EN，直接通过 2361 协议读写任意 (ADDR, INDEX) 寄存器。
    返回 True 表示本次会话所有命令都成功。
    """
    print("\n🚀 执行 IIC_reg_rw（寄存器读写命令循环）...")
    _iic_print_help()
    _get_i2c()          # 未初始化时立即抛出明确错误

    failures = 0
    try:
        while True:
            try:
                raw = input("\nIIC> ").strip()
            except (EOFError, KeyboardInterrupt):
                print()
                break
            if not raw:
                continue

            parts = raw.replace(',', ' ').split()
            cmd, args = parts[0].lower(), parts[1:]
            if cmd in ("q", "quit", "exit"):
                break
            if cmd in ("h", "?", "help"):
                _iic_print_help()
                continue

            entry = _IIC_COMMANDS.get(cmd)
            if entry is None:
                print(f"   ⚠️  未知命令 '{cmd}'，输入 h 查看可用命令")
                continue

            try:
                ok, msg = entry[1](args)
            except Exception as e:
                ok, msg = False, f"{type(e).__name__}: {e}"
            print(f"   {'✅' if ok else '❌'} {msg}")
            if not ok:
                failures += 1
    finally:
        pass

    if failures:
        print(f"\n❌ IIC_reg_rw 结束：{failures} 条命令未成功")
    else:
        print("\n✅ IIC_reg_rw 结束：全部命令执行成功")
    return failures == 0


# =====================================================================
# MTP 全片擦除 + 烧录 + 校验（模板支持 .xlsx / .txt，弹出文件选择框）
# =====================================================================

# 全片擦除后轮询状态寄存器 [31:0] bit0（1=忙，0=就绪）
MTP_ERASE_POLL_DELAY = 0.5
MTP_ERASE_MAX_RETRIES = 60

# CRC 口径（与 MTPBatchLogic.mtp_full_program_with_crc_gui 一致）
MTP_CRC_ADDR_LIMIT = 416            # CRC 覆盖 phys < 416 的声明字节
MTP_CRC_RSV_ADDRS = (507, 508, 509, 510)   # CRC32 小端存放

# Close MTP EN 之后的自检（MTP Load → 片上 CRC 状态 → 测试模式 → 回读修调寄存器）
MTP_POST_LOAD_WAIT = 1.0            # MTP Load 后等待芯片加载/自检完成（与 TESTMODE_EN 一致）
MTP_POST_TESTMODE_ADDR = 20         # 进入测试模式：[20:15] = 5A5A5A5A
MTP_POST_TESTMODE_INDEX = 15
MTP_POST_CRC_ADDR = 21              # 片上自检状态：[21:0] bit2 = 1 → CRC 校验通过
MTP_POST_CRC_INDEX = 0
MTP_POST_CRC_OK_BIT = 2
MTP_POST_VALUE_ADDR = 20            # 修调寄存器 [20:4]，MTP 中对应 phys 440
MTP_POST_VALUE_INDEX = 4
MTP_POST_VALUE_PHYS = VBG_TRIM_MPT_ADDR + VBG_TRIM_MPT_INDEX * 4


def _mtp_select_template():
    """弹出文件选择框，返回所选模板路径；取消或失败返回 None。

    支持 .xlsx 烧录模板与 mtp_config_table.txt 配置表文本。
    """
    try:
        import tkinter as tk
        from tkinter import filedialog
    except Exception as e:
        print(f"   ❌ 无法打开文件选择框（{type(e).__name__}: {e}）")
        return None

    root = tk.Tk()
    root.withdraw()
    root.attributes('-topmost', True)
    try:
        path = filedialog.askopenfilename(
            title="选择 MTP 烧录模板（.xlsx 或 mtp_config_table.txt）",
            filetypes=[("MTP 模板", "*.xlsx *.txt"),
                       ("Excel 文件", "*.xlsx"),
                       ("文本配置表", "*.txt"),
                       ("所有文件", "*.*")],
        )
    finally:
        root.destroy()
    return path or None


def _mtp_txt_int(token, name, lineno):
    """解析文本表里的整数（ADDR 为 0x 前缀 HEX，INDEX/数据为 HEX 或十进制）。"""
    s = str(token).strip()
    try:
        return int(s, 16) if s.lower().startswith("0x") else int(s, 10)
    except ValueError:
        raise ValueError(f"第 {lineno} 行 {name} 无法解析：'{s}'")


def _mtp_parse_config_txt(path):
    """解析 mtp_config_table.txt（由 other_test/mtp/build_mtp_config_txt.py 生成）。

    只读取数据行（以行号开头）：
        行号  ADDR  INDEX  [31:24]  [23:16]  [15:8]  [7:0]  PHYS  CRC?  模块
    说明行、「源表中未烧录的行」（WRITETHENREAD）等自动跳过。

    返回: (buffer, burn_addrs, final_crc)，口径与 xlsx 模板一致：
          burn_addrs = 全部声明字节；CRC32 只覆盖 phys < 416 的声明字节，
          小端写入 buffer[507:511]。
    """
    text = Path(path).read_text(encoding="utf-8", errors="replace")
    buffer = bytearray(512)
    phys_addrs = set()

    for lineno, line in enumerate(text.splitlines(), start=1):
        parts = line.split()
        if len(parts) < 7 or not parts[0].isdigit():
            continue
        try:
            addr = _mtp_txt_int(parts[1], "ADDR", lineno)
            index = _mtp_txt_int(parts[2], "INDEX", lineno)
            b31 = _mtp_txt_int(parts[3], "[31:24]", lineno)
            b23 = _mtp_txt_int(parts[4], "[23:16]", lineno)
            b15 = _mtp_txt_int(parts[5], "[15:8]", lineno)
            b7 = _mtp_txt_int(parts[6], "[7:0]", lineno)
        except ValueError:
            continue        # 非数据行（如 WRITETHENREAD 参考行）直接跳过

        phys = addr + index * 4
        if not 0 <= phys <= 508:
            raise ValueError(f"第 {lineno} 行物理地址 {phys} 超出 0~508")
        buffer[phys] = b7
        buffer[phys + 1] = b15
        buffer[phys + 2] = b23
        buffer[phys + 3] = b31
        phys_addrs.update(range(phys, phys + 4))

    if not phys_addrs:
        raise ValueError("文本模板中没有解析到数据行"
                         "（应含：# ADDR INDEX [31:24] [23:16] [15:8] [7:0] PHYS 列）")

    burn_addrs = sorted(phys_addrs)
    crc_addrs = [a for a in burn_addrs if a < MTP_CRC_ADDR_LIMIT]
    if crc_addrs:
        final_crc, _ = calculate_crc32_iter(bytes(buffer[a] for a in crc_addrs))
    else:
        final_crc = 0xFFFFFFFF
    for i, addr in enumerate(MTP_CRC_RSV_ADDRS):
        buffer[addr] = (final_crc >> (8 * i)) & 0xFF

    # 文件头自带的 CRC32 只用于提示（不影响烧录）
    m = re.search(r"CRC32\s*:\s*0x([0-9A-Fa-f]+)", text)
    if m and int(m.group(1), 16) != final_crc:
        print(f"   ⚠️ 文本表自检：文件头 CRC32 0x{int(m.group(1), 16):08X} "
              f"与解析结果 0x{final_crc:08X} 不一致（表内容被改过但未重新生成？）")

    return buffer, burn_addrs, final_crc


def _mtp_parse_template(path):
    """按扩展名解析模板，返回 (ok, buffer, burn_addrs, final_crc, err)。"""
    if str(path).lower().endswith(".txt"):
        try:
            buffer, burn_addrs, final_crc = _mtp_parse_config_txt(path)
        except Exception as e:
            return False, None, None, None, f"{type(e).__name__}: {e}"
        return True, buffer, burn_addrs, final_crc, None

    return MTPBatchLogic.mtp_full_program_with_crc_gui(
        path, None, None, None, IICLogic.parse_hex_byte, lambda m: None)



def _mtp_wait_erase_ready(mtp, addr=31, index=0,
                          poll_delay=MTP_ERASE_POLL_DELAY,
                          max_retries=MTP_ERASE_MAX_RETRIES):
    """轮询状态寄存器等待擦除完成，返回 True=完成 / False=超时。"""
    for attempt in range(1, max_retries + 1):
        time.sleep(poll_delay)
        try:
            st = mtp.read_i2c(addr, index)
        except Exception as e:
            print(f"   擦除状态轮询第 {attempt} 次异常：{e}")
            continue
        if st and len(st) >= 4:
            if (st[3] & 0x01) == 0:
                print(f"   ✅ 擦除完成（轮询 {attempt} 次，"
                      f"{attempt * poll_delay:.1f}s）")
                return True
        else:
            print(f"   擦除状态轮询第 {attempt} 次无有效返回")
    print(f"   ❌ 擦除超时（已等待 {max_retries * poll_delay:.1f}s）")
    return False


def _mtp_post_burn_check(buffer, burn_addrs):
    """Close MTP EN 之后的自检。

    流程：
      1. MTP Load：写 [25:5] = 5A5A5A5A，让芯片把 MTP 内容加载到寄存器
      2. 读 [21:0]，bit2 = 1 → 片上 CRC 校验通过
      3. 写 [20:15] = 5A5A5A5A 进入测试模式
      4. 读 [20:4]，与烧录到 MTP phys 440 的同一寄存器值比对，判断读写通过

    返回 True 表示 1~4 全部通过；步骤 4 的期望值来自所选模板，
    模板未声明 phys 440 时只要求读回有效数据。
    """
    i2c = _get_i2c()
    ok = True

    # ① MTP Load
    load_hex = ' '.join(f'{b:02X}' for b in MTP_LOAD_DATA)
    print(f"   MTP Load：写入 [{MTP_LOAD_ADDR}:{MTP_LOAD_INDEX}] = {load_hex}（不回读）")
    try:
        execute_write(i2c, addr=MTP_LOAD_ADDR, index=MTP_LOAD_INDEX,
                      write_data=list(MTP_LOAD_DATA))
    except Exception as e:
        print(f"   ❌ MTP Load 失败：{type(e).__name__}: {e}")
        return False
    time.sleep(MTP_POST_LOAD_WAIT)

    # ② 片上 CRC 校验状态：[21:0] bit2 = 1
    status = execute_read(i2c, MTP_POST_CRC_ADDR, MTP_POST_CRC_INDEX)
    if status and len(status) >= 4:
        status_hex = ' '.join(f'{b:02X}' for b in status[:4])
        if status[3] & (1 << MTP_POST_CRC_OK_BIT):
            print(f"   ✅ MTP CRC 校验通过：[{MTP_POST_CRC_ADDR}:{MTP_POST_CRC_INDEX}] = "
                  f"{status_hex}，bit{MTP_POST_CRC_OK_BIT} = 1")
        else:
            print(f"   ❌ MTP CRC 校验未通过：[{MTP_POST_CRC_ADDR}:{MTP_POST_CRC_INDEX}] = "
                  f"{status_hex}，bit{MTP_POST_CRC_OK_BIT} = 0")
            ok = False
    else:
        print(f"   ❌ 读取 [{MTP_POST_CRC_ADDR}:{MTP_POST_CRC_INDEX}] 无有效返回，"
              f"无法判断 CRC 校验")
        ok = False

    # ③ 进入测试模式
    print(f"   测试模式：写入 [{MTP_POST_TESTMODE_ADDR}:{MTP_POST_TESTMODE_INDEX}] = "
          f"5A 5A 5A 5A（不回读）")
    try:
        execute_write(i2c, addr=MTP_POST_TESTMODE_ADDR, index=MTP_POST_TESTMODE_INDEX,
                      write_data=[0x5A, 0x5A, 0x5A, 0x5A])
    except Exception as e:
        print(f"   ❌ 写测试模式失败：{type(e).__name__}: {e}")
        return False

    # ④ 回读 [20:4]（对应 MTP phys 440）
    phys = MTP_POST_VALUE_PHYS
    expected = None
    if all(phys + i in burn_addrs for i in range(4)):
        expected = [buffer[phys + 3], buffer[phys + 2], buffer[phys + 1], buffer[phys]]

    rd = execute_read(i2c, MTP_POST_VALUE_ADDR, MTP_POST_VALUE_INDEX)
    if not rd or len(rd) < 4:
        print(f"   ❌ 读取 [{MTP_POST_VALUE_ADDR}:{MTP_POST_VALUE_INDEX}] 无有效返回，读写失败")
        return False

    rd_hex = ' '.join(f'{b:02X}' for b in rd[:4])
    print(f"   回读 [{MTP_POST_VALUE_ADDR}:{MTP_POST_VALUE_INDEX}] = {rd_hex}"
          f"（MTP phys {phys}）")
    if expected is None:
        print("   ✅ 回读有效，读写通过"
              f"（所选模板未声明 phys {phys}，不做数值比对）")
    elif list(rd[:4]) == expected:
        print(f"   ✅ 与 MTP 烧录值一致（{' '.join(f'{b:02X}' for b in expected)}），读写通过")
    else:
        print(f"   ❌ 与 MTP 烧录值不一致：期望 "
              f"{' '.join(f'{b:02X}' for b in expected)}，读回 {rd_hex}")
        ok = False

    return ok


def MTP_full_program():
    """MTP 全片擦除 + 烧录 + 校验（菜单 [32]）。

    流程：
      1. 弹出文件选择框，选择烧录模板（.xlsx，或 mtp_config_table.txt）
      2. 解析模板并算 CRC（覆盖模板声明且地址 < 416 的字节，小端写入 507~510）
      3. 确认后 Open MTP EN → 全片擦除 → 轮询 [31:0] bit0 等待完成
      4. 两遍烧录（数据区 + CRC 区）→ 逐字节回读校验
      5. 无论成败都 Close MTP EN
      6. 自检：MTP Load → 读 [21:0] bit2 判断 CRC → 写 [20:15] 进测试模式
         → 读 [20:4] 比对烧录值，判断读写通过
    返回 True 表示烧录、校验与自检全部通过。
    """
    print("\n🚀 执行 MTP 全片擦除+烧录+校验 ...")

    path = _mtp_select_template()
    if not path:
        print("   ⏭️ 未选择模板，已取消")
        return False
    print(f"   模板：{path}")

    mtp = _get_mtp()
    ok, buffer, burn_addrs, final_crc, err = _mtp_parse_template(path)
    if not ok:
        print(f"   ❌ 模板解析失败：{err}")
        return False

    crc_bytes = len([a for a in burn_addrs if a < 416])
    print(f"   待烧录 {len(burn_addrs)} 字节（地址 {burn_addrs[0]}~{burn_addrs[-1]}）")
    print(f"   CRC32 = 0x{final_crc:08X}（覆盖地址 <416 的声明字节 {crc_bytes} 个）"
          f"，写入 507~510 小端")
    if input("   确认执行全片擦除并烧录？(y/N): ").strip().lower() != "y":
        print("   ⏭️ 已取消")
        return False

    mtp.open_mtp_en()
    mismatches = []
    try:
        print("   下发全片擦除 ...")
        mtp.erase_chip()
        if not _mtp_wait_erase_ready(mtp):
            return False

        print(f"   开始两遍烧录（共 {len(burn_addrs)} 字节）...")
        MTPBatchLogic.execute_mtp_burn(
            mtp, buffer, burn_addrs, lambda m: print(f"   {m}"))

        print("   逐字节回读校验 ...")
        # 校验日志逐字节输出太长，只保留失败项
        mismatches = MTPBatchLogic.verify_mtp_burn(
            mtp, buffer, burn_addrs,
            lambda m: print(f"   {m}") if ("失败" in m or str(m).startswith("!!")) else None)
    finally:
        mtp.close_mtp_en()

    if mismatches:
        print(f"\n❌ 校验发现 {len(mismatches)} 处不一致：")
        for off, exp, got in mismatches[:20]:
            got_txt = "None" if got is None else f"0x{got:02X}"
            print(f"   地址 {off}: 期望 0x{exp:02X}，读回 {got_txt}")
        if len(mismatches) > 20:
            print(f"   ...（其余 {len(mismatches) - 20} 处略）")
        return False

    # Close MTP EN 之后的自检：MTP Load → [21:0] bit2 → 测试模式 → 回读 [20:4]
    print("\n   开始 Close MTP EN 后自检 ...")
    if not _mtp_post_burn_check(buffer, burn_addrs):
        print("\n❌ 逐字节校验通过，但 MTP Load 后自检未通过")
        return False

    print(f"\n✅ 全片擦除+烧录+校验通过：{len(burn_addrs)} 字节，CRC 0x{final_crc:08X}")
    return True




# =====================================================================
# 测试项映射表 —— main.py 通过此字典动态加载菜单
# 键为菜单选择键（字符串），值为 (显示名称, 测试函数)
# =====================================================================
TEST_ITEMS = {
    "1": ("进入测试模式使能",  TESTMODE_EN),
    "2": ("VBG电压",  VBG_Voltage),
    "3": ("VBG_buffer电压", VBG_buffer_Voltage),
    "4": ("OSC_VREF电压",  OSC_VREF_Voltage),
    "5": ("VPTAT电压",     VPTAT_Voltage),
    "6": ("VCM电压",       VCM_Voltage),
    "7": ("TIA_OUT1电压",  TIA_OUT1_Voltage),
    "8": ("TIA_OUT2电压",  TIA_OUT2_Voltage),
    "9": ("TIA_OUT3电压",  TIA_OUT3_Voltage),
    "10": ("LDO15电压",    LDO15_Voltage),
    "11": ("LDO_3P3电压",  LDO_3P3_Voltage),
    "12": ("VDD2电压",     VDD2_Voltage),
    "13": ("ADC_VREF电压", ADC_VREF_Voltage),
    "14": ("ADC_IN电压",   ADC_IN_Voltage),
    "15": ("HOCP U相和V相", HOCP_Current),
    "16": ("LOCP  U相和V相",  LOCP_Current),
    "17": ("IBN_5U电流",  IBN_5U_Current),
    "18": ("HOCP  W相", HOCP_W_Current),
    "19": ("LOCP  W相",  LOCP_W_Current),
    "20": ("UVLO电压",  UVLO_Voltage),
    "21": ("OSC频率", OSC_P),
    "22": ("TSD温度",  TSD_TEMP),
    "23": ("COMP0迟滞",  COMP0_hy),
    "24": ("COMP1迟滞", COMP1_hy),
    "25": ("COMP2迟滞",  COMP2_hy),
    "26": ("FR阈值电压",  FR_Voltage),
    "27": ("输出漏电测试",  OUTPUT_leakage_test),
    "28": ("上管输出阻抗",  UP_output_impedance),
    "29": ("下管输出阻抗",   down_output_impedance),
    "30": ("PWM阈值电压",  PWM_IN_Voltage),
    "31": ("MTP_program",  MTP_program),
    "32": ("MTP 全片擦除+烧录+校验+自检",  MTP_full_program),
    "33": ("IIC 寄存器读写",  IIC_reg_rw),
}
