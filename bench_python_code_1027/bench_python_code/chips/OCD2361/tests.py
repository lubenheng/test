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
from contextlib import contextmanager
from pathlib import Path

import numpy as np
import openpyxl
from openpyxl.styles import Alignment, Font, PatternFill

# from bench_python_code.instrument.multimeter.Keysight_34465A import read_keysight_voltage
from instrument import instrument_set as instr
from instrument import result_log
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

# 表内单位 → 实测值单位标签（_UNIT_SCALE 已把限值换算到实测单位，记录时按实测单位写）
_MEASURE_UNIT = {
    "V": "mV", "MV": "mV", "MA": "mA", "UA": "mA", "A": "mA",
    "HZ": "Hz", "KHZ": "Hz", "MHZ": "Hz", "Ω": "Ω", "OHM": "Ω",
}


def _ensure_result_session():
    """首次记录时创建本次的结果 txt（Save_Results/measure_Results/测试结果_<芯片>_<时间>.txt）。"""
    if result_log.current_path() is None:
        path = result_log.start_session(getattr(chip_cfg, "CHIP_LABEL", "chip"))
        print(f"\n📝 测试结果记录：{path}")
    return result_log.current_path()



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


def _judge_by_standard(test_item_name, value, extra_note="", limits=None, typ=None):
    """按校准标准表的 Min./Max. 判定测试项是否通过。

    参数：
        test_item_name: str - 表 A 列的测试项名（与 Excel 中的写法一致）
        value: float/None - 实测值（单位取测量函数返回的单位）
        extra_note: str - 附加到结果行的说明（如修调值）
        limits: tuple/None - (Min, Max) 覆盖值，单位与 value 一致（如 mV）；
                给定时不使用校准表中的 Min./Max.，Typ 仍取自校准表
        typ: float/None - Typ 覆盖值，单位与 value 一致；给定时不使用校准表中的 Typ
    返回：
        True/False - 判定结果；无法判定（无标准/无上下限/测量失败）返回 None
    """
    if value is None:
        print(f"❌ {test_item_name} failed：测量失败（无有效读数）")
        _ensure_result_session()
        result_log.record(test_item_name, None, "", result="FAIL",
                          note=f"测量失败{f'，{extra_note}' if extra_note else ''}")
        return None

    try:
        table = _load_calib_standard()
    except Exception as e:
        print(f"   ⚠️ 读取校准标准失败，跳过判定：{e}")
        _ensure_result_session()
        result_log.record(test_item_name, value, "", result="NO-STD",
                          note=f"读取校准标准失败：{e}")
        return None

    if test_item_name not in table:
        print(f"   ⚠️ 校准标准中未找到测试项 '{test_item_name}'，跳过判定")
        _ensure_result_session()
        result_log.record(test_item_name, value, "", result="NO-STD",
                          note="校准标准中无此项")
        return None

    min_val, typ_val, max_val, unit = table[test_item_name]
    scale = _UNIT_SCALE.get(unit.upper(), 1.0)
    unit_label = _MEASURE_UNIT.get((unit or "").upper(), unit or "")
    if unit and unit.upper() not in _UNIT_SCALE:
        print(f"   ⚠️ 未识别的单位 '{unit}'，按 1:1 处理")

    min_m = None if min_val is None else min_val * scale
    max_m = None if max_val is None else max_val * scale
    typ_m = None if typ_val is None else typ_val * scale
    if limits is not None:
        min_m, max_m = limits
    if typ is not None:
        typ_m = typ
    note = f"，{extra_note}" if extra_note else ""

    if min_m is None and max_m is None:
        print(f"⚠️ {test_item_name} 无判定标准（表中 Min./Max. 均未填），"
              f"实测 {value:.3f}{note}")
        _ensure_result_session()
        result_log.record(test_item_name, value, unit_label, typ_v=typ_m,
                          result="NO-STD", note=extra_note)
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
    _ensure_result_session()
    result_log.record(test_item_name, value, unit_label,
                      min_v=min_m, typ_v=typ_m, max_v=max_m,
                      result="PASS" if passed else "FAIL", note=extra_note)
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


# 电流类测试（IBN 等 µA 级）使用的万用表直流电流档位。
# 34465A 的最低电流档是 100 µA（没有 1 µA / 10 µA 档），
# 所以 µA 级读数一律用 100 µA 档，超量程换 1 mA。
CURRENT_DMM_RANGE = "100uA"
CURRENT_DMM_RANGE_BACKUP = "1mA"


def _current_test(name, addr, index, write_data, range_val=CURRENT_DMM_RANGE):
    """电流类测试：写配置 → 万用表切直流电流档 → 测电流(mA) → 读回校验。

    这类测试项（IBN）校准表未给判定范围，只记录实测值。

    电流都在 µA 级，用 100 µA 档（34465A 的最低电流档；
    1 A 档的分辨力约 10 µA，根本测不出 5 µA），超量程时自动换 1 mA 档重测。
    """
    print(f"\n🚀 执行 {name} ...")
    i2c = _get_i2c()
    execute_write(i2c, addr=addr, index=index, write_data=write_data)

    # 显式切到直流电流档并锁定量程
    if instr.dmm_set_dc_current(range_val):
        print(f"   万用表已切到直流电流档，量程 {range_val}")
    else:
        print(f"   ❌ 万用表未能切到直流电流档（量程 {range_val}），"
              f"继续测量会读到电压，本项记 FAIL")
        _ensure_result_session()
        result_log.record(name, None, "mA", result="FAIL",
                          note="万用表未能切换到直流电流档")
        return None

    ma = instr.dmm_measure_single_current(current_type="DC", range_val=range_val)
    if ma is not None and abs(ma) >= 1e30:      # 9.9E+37 超量程
        print(f"   ⚠️ {range_val} 量程溢出，改用 {CURRENT_DMM_RANGE_BACKUP} 档重测")
        ma = instr.dmm_measure_single_current(current_type="DC",
                                              range_val=CURRENT_DMM_RANGE_BACKUP)
        if ma is not None and abs(ma) >= 1e30:
            ma = None

    if ma is not None:
        print(f"   电流 = {ma * 1000:.3f} µA")
    else:
        print("   电流测量失败")
    _log_read(addr, index)

    _ensure_result_session()
    result_log.record(name, ma, "mA", result="MEASURED" if ma is not None else "FAIL",
                      note="" if ma is not None else "测量失败")

    print(f"✅ {name} 完成")
    return ma


# =====================================================================
# 测试函数 —— 每个测试项对应一个函数
# =====================================================================

def TESTMODE_EN():
    """进入测试模式使能：MTP上电 → MTP Load → (间隔1s) → 测试模式 → 默认偏置档。

    进入测试模式后写一次 RW_TIA_IBIAS_SEL（[20:0] bit[24:23]）= 01，
    之后整个测试过程保持不变（各测试项写 [20:0] 都是读-改-写）。
    """
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
    time.sleep(0.1)

    # 默认偏置档：RW_TIA_IBIAS_SEL = 01（测试全程保持，见 _tia_ibias_sel_ensure）
    print(f"默认偏置档：RW_TIA_IBIAS_SEL（[20:0] bit[24:23]）= "
          f"2'b{TIA_IBIAS_SEL_DEFAULT:02b}")
    _tia_ibias_sel_ensure(i2c)
    _log_read(TIA_IBIAS_SEL_ADDR, TIA_IBIAS_SEL_INDEX)

    print("✅ 进入测试模式使能 完成")

# =====================================================================
# MTP_LOAD
# =====================================================================

def MTP_LOAD():
    """MTP Load → 进入测试模式 → 默认偏置档（与 TESTMODE_EN 一致）。"""
    print("\n🚀 执行MTP_LOAD ...")
    i2c = _get_i2c()
    mtp_load_data = [0x5a, 0x5a, 0x5a, 0x5a]
    print(f"MTP Load：写入 [25:5] = {' '.join(f'{b:02X}' for b in mtp_load_data)}（5A5A5A5A 不回读）")
    execute_write(i2c, addr=25, index=5, write_data=mtp_load_data)

    time.sleep(1)

    testmode_data = [0x5a, 0x5a, 0x5a, 0x5a]
    print(f"测试模式：写入 [20:15] = {' '.join(f'{b:02X}' for b in testmode_data)}（5A5A5A5A 不回读）")
    execute_write(i2c, addr=20, index=15, write_data=testmode_data)
    time.sleep(0.1)

    _tia_ibias_sel_ensure(i2c)

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
OSC_TRIM_SETTLE_S = 0.5    # 改写 RW_OSC_24M_TRIM 后等频率稳定的时间（s）

# OSC 判定范围（Hz）：11620 ~ 11820，Typ 11.72 kHz
OSC_LIMITS_HZ = (11620.0, 11820.0)

# ADC_VREF 目标电压（mV），与校准表 ADC_VREF_Voltage 的 Typ 2.0 V 一致
ADC_VREF_TARGET_MV = 2000.0
ADC_VREF_LIMITS_MV = (1900.0, 2100.0)   # 判定范围，与校准表 Min./Max. 一致
VREF_PROBE_LO = 0          # 斜率探测点 1：修调值 0
VREF_PROBE_HI = 10         # 斜率探测点 2：修调值 10
VREF_SCAN_SPAN = 10        # 估算值不达标时，在 ±10 内扫描

# ---------------------- TIA 输出（TIA_OUT1/2/3，菜单 7/8/9）---------------------
# 规范 2.6~2.8：进测试模式后写 [20:9] 选通道，往输出脚灌 200 mA，万用表测 WO/UO，
# 减去 VCM 电压即为 TIA_OUTn 电压；跨阻 R = 电压 / 200 mA。
# 跨阻由 RW_TIA_RES_SELn 选择（默认 000 → 0.5 Ω → 理论 100 mV）。
TIA_CURRENT_A = 0.2                  # 灌入电流（A），规范 200 mA
TIA_CURRENT_MIN_RATIO = 0.5          # 源表实测电流低于设定值的 50% 视为“没灌进去”
TIA_SMU_VLIMIT_V = 5.0               # 灌流时源表电压限值（V）
TIA_SETTLE_S = 0.5                   # 切通道 / 改修调后的稳定时间（s）
TIA_TARGET_MV = 100.0                # 典型值（跨阻 0.5 Ω × 200 mA）
TIA_LIMITS_MV = (90.0, 110.0)        # 规范 Min./Max.（校准表 Min. 填成 0.9 V，故显式给定）
TIA_VCM_SEL = [0x00, 0x00, 0x01, 0xE0]   # TEST_ANA<3:0> = 0111 → VCM

# RW_TIA_RES_SELn 编码 → 跨阻（Ω），默认 000 = 0.5 Ω
TIA_RES_TABLE = {
    0b000: 0.5, 0b001: 0.75, 0b010: 1.25, 0b011: 2.0,
    0b100: 1.0, 0b101: 1.5,  0b110: 2.5,  0b111: 4.0,
}
TIA_RES_DEFAULT_CODE = 0b000         # 规范默认档位
# 判定范围 = 典型值 ± 10 mV（绝对值）。默认 0.5 Ω 档典型值 100 mV → 90~110 mV，
# 与规范 Min./Max. 一致；选其它跨阻档位时典型值按 R × 200 mA 换算。
TIA_LIMIT_MV = 10.0

# RW_TIA_IBIAS_SEL<1:0>：TIA 输入偏置电流档，位于 [20:0] 的 bit[24:23]。
# 初始化（进入测试模式）时写 01，并在整个测试过程中保持不变：
#   · [20:0] 的其它字段（RW_OCP_CFG / RW_TIA_RES_SELn / COMPn_HYS）一律读-改-写，
#     不会覆盖这两位；
#   · 每个会用到 [20:0] 的测试项开始前都会调 _tia_ibias_sel_ensure() 复核一次，
#     发现被改掉立即写回 01。
TIA_IBIAS_SEL_ADDR = 20
TIA_IBIAS_SEL_INDEX = 0
TIA_IBIAS_SEL_BIT_LO = 23
TIA_IBIAS_SEL_MASK = 0x03 << TIA_IBIAS_SEL_BIT_LO
TIA_IBIAS_SEL_DEFAULT = 0b01

# 测试状态机：写 TIA 通道（[20:9]）之前必须先初始化并启动，否则通道切换不生效
TIA_SM_ADDR = 1
TIA_SM_INIT_INDEX = 32
TIA_SM_INIT_DATA = [0x00, 0x00, 0x00, 0x20]     # [1:32] 初始化状态机
TIA_SM_START_INDEX = 31
TIA_SM_START_DATA = [0x00, 0x00, 0x02, 0x00]    # [1:31] 启动状态机

# TRIM_TIA_RES0/1/2<7:0> 都在 [20:5]：RES0 = [7:0]、RES1 = [15:8]、RES2 = [23:16]
TIA_TRIM_ADDR = 20
TIA_TRIM_INDEX = 5
TIA_TRIM_MAX = 0xFF
TIA_TRIM_MPT_ADDR = 0x1A8            # MTP 表中 (0x1A8, 5) ↔ phys 444
TIA_TRIM_MPT_INDEX = 5

# 通道配置
#   sel      : [20:9] 选通道数据（TEST_ANA<3:0>）
#   src/meas : 灌流脚 / 测量脚
#   trim_lo  : TRIM_TIA_RESn 在 [20:5] 中的低位
#   res_sel  : RW_TIA_RES_SELn 的 (ADDR, INDEX, bit_lo)，三位都在 [20:0] 里连续排布
#              SEL0 = [12:10]、SEL1 = [15:13]、SEL2 = [18:16]
TIA_CHANNELS = {
    1: {"name": "TIA_OUT1", "sel": [0x00, 0x00, 0x01, 0x37], "src": "UO", "meas": "WO",
        "trim_lo": 0, "trim_field": "TRIM_TIA_RES0",
        "res_sel": (20, 0, 10), "res_sel_field": "RW_TIA_RES_SEL0"},
    2: {"name": "TIA_OUT2", "sel": [0x00, 0x00, 0x01, 0x77], "src": "VO", "meas": "WO",
        "trim_lo": 8, "trim_field": "TRIM_TIA_RES1",
        "res_sel": (20, 0, 13), "res_sel_field": "RW_TIA_RES_SEL1"},
    3: {"name": "TIA_OUT3", "sel": [0x00, 0x00, 0x01, 0xB7], "src": "WO", "meas": "UO",
        "trim_lo": 16, "trim_field": "TRIM_TIA_RES2",
        "res_sel": (20, 0, 16), "res_sel_field": "RW_TIA_RES_SEL2"},
}

# 最近一次测得的 VCM（mV）；后续 TIA 测试默认沿用，可输入 m 重测
TIA_VCM_CACHE = {"mv": None}

# ADC_IN（菜单 14）：基准板 DAC 输出 → WO，ADC 采样
# 规格要求 WO 输入不超过 2 V，因此电压上限固定 2000 mV
ADC_IN_VMAX_MV = 2000
ADC_IN_DAC_START_MV = 200       # 默认起点（运行时可在终端修改）
ADC_IN_DAC_STEP_MV = 10         # 默认步进（运行时可在终端修改）
ADC_IN_DAC_TOL_MV = 20.0        # DAC 输出线性度容差（mV）

# ADC 采样寄存器（规范 2.13 节）
# 测试前置条件：ADC 时钟必须已选 16 MHz —— [20:2] 读回 00 00 00 03（MTP phys 432 = 0x03）
ADC_CLK_ADDR = 20
ADC_CLK_INDEX = 2
ADC_CLK_EXPECT = [0x00, 0x00, 0x00, 0x03]
ADC_CLK_MPT_PHYS = 432          # MTP 表中 (0x1A8, 2) → phys 432

# 测试状态机（选 INT 模式 → 启动），必须在写 [20:9] 之前
ADC_SM_INIT_ADDR, ADC_SM_INIT_INDEX = 1, 32
ADC_SM_INIT_DATA = [0x00, 0x43, 0xFD, 0x22]      # 状态机选 INT
ADC_SM_START_ADDR, ADC_SM_START_INDEX = 1, 31
ADC_SM_START_DATA = [0x00, 0x01, 0x02, 0x00]     # 启动状态机

# ADC 测试使能（TEST_ANA<3:0> = 1100）
ADC_TESTMODE_ADDR, ADC_TESTMODE_INDEX = 20, 9
ADC_TESTMODE_DATA = [0x00, 0x00, 0x03, 0x20]

# 通道选择：4 通道
ADC_CH_ADDR, ADC_CH_INDEX = 3, 1
ADC_CH_DATA = [0x00, 0x00, 0x00, 0x10]

# ADC 控制寄存器 [3:0]：初始化（数据右对齐，忽略 adc_error）/ 每次取数前更新
ADC_CTRL_ADDR, ADC_CTRL_INDEX = 3, 0
ADC_CTRL_INIT_DATA = [0x00, 0x00, 0x0D, 0x81]
ADC_CTRL_UPDATE_DATA = [0x00, 0x00, 0x0D, 0xC1]
# 初始化写 00 00 0D 81 后的通过判断：只看最低字节 = 81（其它字节不是写进去就原样读回的）
ADC_CTRL_CHECK_BYTE = 0x81

# ADC 数据寄存器 [3:7]，右对齐 14 位
ADC_DATA_ADDR, ADC_DATA_INDEX = 3, 7
ADC_DATA_MASK = 0x3FFF          # ADC4_DR<13:0>
ADC_SAMPLE_DELAY = 0.002        # 更新后等待 ADC 完成（s），规格要求 ≥1 ms

# 换算满量程：规格 2.13 的 Typ 写作 “4096*Vin/ADC_VREF”，故 LSB = VREF/4096。
# ⚠️ 但数据寄存器是 ADC4_DR<13:0>（14 bit），若实测 code 在 Vin=VREF 时约为 16384，
#    说明满量程是 16384，需要把本值改为 16384（LSB 变 1/4）。
#    测试结束会打印“推断满量程”，与配置值不符时会告警，便于现场确认。
ADC_FULL_SCALE = 4096
ADC_IN_VREF_MV = 2000.0         # 换算用 ADC_VREF（mV），Vadc = code × VREF / ADC_FULL_SCALE
ADC_FS_EST_MIN_MV = 100.0       # 推断满量程时只取 DAC 实测 ≥ 该值的点（避开小信号噪声）

# [3:0] 初始化写 00 00 0D 81 后回读最低字节不是 81 时的恢复流程（修调/状态机重新配置）
ADC_RECOVER_TRIM_ADDR, ADC_RECOVER_TRIM_INDEX = 25, 5
ADC_RECOVER_TRIM_DATA = [0x5A, 0x5A, 0x5A, 0x5A]

# 测量数据导出目录（相对 bench_python_code）与工作表名
ADC_IN_EXPORT_DIR = "Save_Results/measure_Results"
ADC_SHEET_DATA = "ADC采样数据汇总"      # 逐点数据
ADC_SHEET_RESULT = "测试结果"           # 仅最终结果


def _hex(data):
    """字节列表 → '00 00 03 20' 形式的字符串。"""
    return ' '.join(f'{b:02X}' for b in data)


def _adc_check_clock(i2c):
    """检查 ADC 时钟是否已选 16 MHz：[20:2] 应为 00 00 00 03（MTP phys 432 = 0x03）。

    返回 True=满足前置条件；False=不满足（调用方应终止测试）。
    不满足时会请操作员确认是否仍要继续（默认不继续）。
    """
    rd = _log_read(ADC_CLK_ADDR, ADC_CLK_INDEX)
    if not rd or len(rd) < 4:
        print(f"❌ 读取 [{ADC_CLK_ADDR}:{ADC_CLK_INDEX}] 失败，无法确认 ADC 时钟配置")
        return False

    if list(rd[:4]) == ADC_CLK_EXPECT:
        print(f"   ✅ ADC 时钟已选 16 MHz（[{ADC_CLK_ADDR}:{ADC_CLK_INDEX}] = {_hex(rd[:4])}）")
        return True

    print(f"❌ ADC 时钟不是 16 MHz：[{ADC_CLK_ADDR}:{ADC_CLK_INDEX}] = {_hex(rd[:4])}，"
          f"期望 {_hex(ADC_CLK_EXPECT)}")
    print(f"   对应 MTP 物理地址 {ADC_CLK_MPT_PHYS}（(0x1A8, 2)）的 [7:0] 应为 0x03，"
          f"请先把时钟配置烧进 MTP 再测试")
    try:
        ans = input("   仍要继续测试？(y/N): ").strip().lower()
    except (EOFError, KeyboardInterrupt):
        print()
        return False
    if ans != "y":
        return False
    print("   ⚠️  已在时钟未确认的情况下继续，ADC 采样结果可能不准确")
    return True


def _adc_ctrl_recover(i2c):
    """[3:0] 初始化回读最低字节不是 81 时的恢复流程（顺序不可调整）。

      [3:0]=00 00 0C 01
      [1:32]=00 00 00 10
      [25:5]=5A 5A 5A 5A
      [1:31]=00 00 02 00  → 读 [1:32]，最低两位为 10 即可
      [1:31]=00 00 00 00
      [1:32]=00 00 00 20
      [1:31]=00 00 02 00
      [25:5]=5A 5A 5A 5A   → 读 [1:32]，为 21 即可
      [3:0]=00 00 0C 81   → 读 [3:0]，最低字节为 81 即可

    返回 True=恢复成功；False=任一步校验失败。
    """
    def _w(addr, index, data):
        print(f"      写 [{addr}:{index}] = {_hex(data)}")
        execute_write(i2c, addr=addr, index=index, write_data=list(data))
        time.sleep(0.01)

    def _check(addr, index, pred, expect):
        rd = execute_read(i2c, addr, index)
        if not rd or len(rd) < 4:
            print(f"      ❌ 读 [{addr}:{index}] 无有效返回")
            return False
        ok = pred(list(rd[:4]))
        print(f"      读 [{addr}:{index}] = {_hex(rd[:4])}，期望{expect}"
              f"{' ✅' if ok else ' ❌'}")
        return ok

    print("   ⚠️ [3:0] 回读最低字节不是 81，执行恢复流程 ...")

    _w(ADC_CTRL_ADDR, ADC_CTRL_INDEX, [0x00, 0x00, 0x0C, 0x01])
    _w(ADC_SM_INIT_ADDR, ADC_SM_INIT_INDEX, [0x00, 0x00, 0x00, 0x10])
    _w(ADC_RECOVER_TRIM_ADDR, ADC_RECOVER_TRIM_INDEX, ADC_RECOVER_TRIM_DATA)
    _w(ADC_SM_START_ADDR, ADC_SM_START_INDEX, [0x00, 0x00, 0x02, 0x00])
    if not _check(ADC_SM_INIT_ADDR, ADC_SM_INIT_INDEX,
                  lambda d: (d[3] & 0x03) == 0x02, "最低两位为 10"):
        return False

    _w(ADC_SM_START_ADDR, ADC_SM_START_INDEX, [0x00, 0x00, 0x00, 0x00])
    _w(ADC_SM_INIT_ADDR, ADC_SM_INIT_INDEX, [0x00, 0x00, 0x00, 0x20])
    _w(ADC_SM_START_ADDR, ADC_SM_START_INDEX, [0x00, 0x00, 0x02, 0x00])
    _w(ADC_RECOVER_TRIM_ADDR, ADC_RECOVER_TRIM_INDEX, ADC_RECOVER_TRIM_DATA)
    if not _check(ADC_SM_INIT_ADDR, ADC_SM_INIT_INDEX,
                  lambda d: d[3] == 0x21, "为 21"):
        return False

    _w(ADC_CTRL_ADDR, ADC_CTRL_INDEX, [0x00, 0x00, 0x0C, 0x81])
    if not _check(ADC_CTRL_ADDR, ADC_CTRL_INDEX,
                  lambda d: d[3] == 0x81, "最低字节为 81"):
        return False

    print("   ✅ 恢复流程通过")
    return True


def _adc_setup(i2c):
    """ADC 测试初始化：状态机选 INT → 启动 → ADC 测试使能 → 4 通道 → 数据格式。

    必须在时钟检查通过后调用。写 [3:0] = 00 00 0D 81 后会立即回读，
    最低字节 = 81 即通过；不是 81（或读失败）时进入 _adc_ctrl_recover 恢复流程。
    返回 False 表示初始化失败（调用方应终止测试）。
    """
    print(f"   状态机选 INT：  [{ADC_SM_INIT_ADDR}:{ADC_SM_INIT_INDEX}] = {_hex(ADC_SM_INIT_DATA)}")
    execute_write(i2c, addr=ADC_SM_INIT_ADDR, index=ADC_SM_INIT_INDEX,
                  write_data=list(ADC_SM_INIT_DATA))
    time.sleep(0.01)

    print(f"   启动状态机：    [{ADC_SM_START_ADDR}:{ADC_SM_START_INDEX}] = {_hex(ADC_SM_START_DATA)}")
    execute_write(i2c, addr=ADC_SM_START_ADDR, index=ADC_SM_START_INDEX,
                  write_data=list(ADC_SM_START_DATA))
    time.sleep(0.01)

    print(f"   ADC 测试使能：  [{ADC_TESTMODE_ADDR}:{ADC_TESTMODE_INDEX}] = {_hex(ADC_TESTMODE_DATA)}")
    execute_write(i2c, addr=ADC_TESTMODE_ADDR, index=ADC_TESTMODE_INDEX,
                  write_data=list(ADC_TESTMODE_DATA))
    _log_read(ADC_TESTMODE_ADDR, ADC_TESTMODE_INDEX)
    time.sleep(0.05)

    print(f"   选择 4 通道：   [{ADC_CH_ADDR}:{ADC_CH_INDEX}] = {_hex(ADC_CH_DATA)}")
    execute_write(i2c, addr=ADC_CH_ADDR, index=ADC_CH_INDEX, write_data=list(ADC_CH_DATA))
    time.sleep(0.01)

    print(f"   ADC 数据右对齐（忽略 adc_error）："
          f"[{ADC_CTRL_ADDR}:{ADC_CTRL_INDEX}] = {_hex(ADC_CTRL_INIT_DATA)}")
    execute_write(i2c, addr=ADC_CTRL_ADDR, index=ADC_CTRL_INDEX,
                  write_data=list(ADC_CTRL_INIT_DATA))
    time.sleep(0.01)

    # 回读校验：只看最低字节是否 = 81；不一致（或读失败）时走恢复流程
    rd = execute_read(i2c, ADC_CTRL_ADDR, ADC_CTRL_INDEX)
    if not rd or len(rd) < 4:
        print(f"   ❌ 读回 [{ADC_CTRL_ADDR}:{ADC_CTRL_INDEX}] 无有效返回")
        return _adc_ctrl_recover(i2c)
    if rd[3] != ADC_CTRL_CHECK_BYTE:
        print(f"   ⚠️ 读回 [{ADC_CTRL_ADDR}:{ADC_CTRL_INDEX}] = {_hex(rd[:4])}，"
              f"最低字节 {rd[3]:02X} ≠ {ADC_CTRL_CHECK_BYTE:02X}")
        return _adc_ctrl_recover(i2c)
    print(f"   ✅ 读回 [{ADC_CTRL_ADDR}:{ADC_CTRL_INDEX}] = {_hex(rd[:4])}，"
          f"最低字节 = {ADC_CTRL_CHECK_BYTE:02X}（通过）")
    return True


def _adc_sample(i2c, timeout_s=ADC_SAMPLE_DELAY):
    """更新一次 ADC 数据并读取码值。

    先写 [3:0] = 00 00 0D C1（更新 ADC 数据），等待 ≥1 ms 后读 [3:7]，
    取右对齐的低 14 位。失败返回 None。
    """
    try:
        execute_write(i2c, addr=ADC_CTRL_ADDR, index=ADC_CTRL_INDEX,
                      write_data=list(ADC_CTRL_UPDATE_DATA))
        time.sleep(timeout_s)
        rd = execute_read(i2c, ADC_DATA_ADDR, ADC_DATA_INDEX)
        if not rd or len(rd) < 4:
            return None
        word = (rd[0] << 24) | (rd[1] << 16) | (rd[2] << 8) | rd[3]
        return word & ADC_DATA_MASK
    except Exception as e:
        print(f"   ⚠️ ADC 采样异常：{e}")
        return None


def _adc_estimate_full_scale(rows, vref_mv=ADC_IN_VREF_MV, min_mv=ADC_FS_EST_MIN_MV):
    """由实测 (DAC 电压, ADC code) 反推 ADC 满量程，用来核对 LSB 换算是否正确。

    code ≈ FS × Vin / VREF  →  FS ≈ code × VREF / Vin

    只取 |DAC 实测| ≥ min_mv 的点（小信号时 code 量化误差占比太大），取中位数。
    返回 (满量程估计, 参与点数)；数据不足返回 (None, 0)。
    """
    ests = []
    for row in rows:
        vin_meas, code = row[1], row[3]
        if vin_meas is None or code is None or code == 0:
            continue
        if abs(vin_meas) < min_mv:
            continue
        ests.append(code * vref_mv / abs(vin_meas))
    if not ests:
        return None, 0
    ests.sort()
    mid = len(ests) // 2
    fs = ests[mid] if len(ests) % 2 else (ests[mid - 1] + ests[mid]) / 2.0
    return fs, len(ests)


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


# =====================================================================
# 测试模式：直接测量 / TRIM 修调
# VBG、ADC_VREF、OSC、TIA_OUT1/2/3 六个测试项共用同一套选择逻辑：
#   0 直接测量 —— 只测一次并按标准判定，**不改任何修调寄存器**
#   1 TRIM 修调 —— 测一次判定；不达标时搜索最优修调值、写回、复测并同步 MTP 配置表
# TIA_OUT1/2/3 另加：
#   2 直接测量·全部跨阻档位 —— RW_TIA_RES_SELn 依次 000~111 逐档测量并判定，不改修调
# =====================================================================
TEST_MODE_DIRECT = 0
TEST_MODE_TRIM = 1
TEST_MODE_DIRECT_ALL_RES = 2           # 仅 TIA_OUT1/2/3 使用
TEST_MODE_DEFAULT = TEST_MODE_TRIM     # 默认沿用原有行为（不达标就修调）


def _ask_test_mode(name, trim_field=None, default=TEST_MODE_DEFAULT,
                   direct_desc=None, trim_desc=None, extra_modes=None):
    """交互选择测试模式，返回 TEST_MODE_DIRECT / TEST_MODE_TRIM（或 extra_modes 里的值）。

    参数：
        name: str - 测试项显示名（如 "VBG"）
        trim_field: str/None - 修调字段名（如 "VBGTRIM"），用来说明修调对象
        default: int - 回车时的默认模式
        direct_desc: str/None - 直接测量模式的说明（覆盖默认文字）
        trim_desc: str/None - 修调模式的说明（覆盖默认文字）
        extra_modes: list/None - 追加选项 [(值, 标题, 说明), ...]，
                     例如 TIA 的 (TEST_MODE_DIRECT_ALL_RES, "直接测量·全部跨阻档位", ...)

    非法输入按默认模式处理（_ask_int 内部已做范围检查）。
    """
    if direct_desc is None:
        direct_desc = "只测一次并按标准判定，不改修调值"
    if trim_desc is None:
        trim_desc = "判定不通过时自动搜索最优修调值、写回并复测"
        if trim_field:
            trim_desc += f"（修调字段 {trim_field}）"

    choices = [TEST_MODE_DIRECT, TEST_MODE_TRIM]
    print(f"\n   {name} 测试模式：")
    print(f"      0 → 直接测量：{direct_desc}")
    print(f"      1 → TRIM 修调：{trim_desc}")
    for value, title, desc in (extra_modes or []):
        print(f"      {value} → {title}：{desc}")
        choices.append(value)

    mode = _ask_int(f"{name} 测试模式 {min(choices)}~{max(choices)}",
                    default, min(choices), max(choices))
    if mode not in choices:            # 选项不连续时（如 0/1/2 缺项）回落到默认
        print(f"   ⚠️ {mode} 不是有效模式，使用默认 {default}")
        return default
    return mode


def _export_adc_table(rows, path_hint, summary=None):
    """导出 ADC_IN 测量数据为 xlsx（两个工作表），返回保存路径；失败返回 None。

    工作表 "ADC采样数据汇总"（逐点数据）：
        A DAC设置值(mV)
        B DAC测量值(mV)
        C ADC code
        D ADC转换电压(mV)
        E 误差(mV) = DAC测量值 − ADC转换电压
        F LSB      = 误差 / LSB电压

    工作表 "测试结果"（仅最终结果）：测试时间、扫描范围、有效点、
    DAC 最大偏差、ADC 最大误差、满量程自检、判定结论。

    参数：
        rows:    list - 内部行 [设定, DAC实测, DAC偏差, code, ADC电压, 误差, LSB]
        summary: dict/None - 最终结果字段，键见 refresh 处的构造
    """
    try:
        wb = openpyxl.Workbook()

        # ---------- ① ADC采样数据汇总 ----------
        ws = wb.active
        ws.title = ADC_SHEET_DATA
        headers = ["DAC设置值(mV)", "DAC测量值(mV)", "ADC code",
                   "ADC转换电压(mV)", "误差(mV)", "LSB"]
        ws.append(headers)
        for c in range(1, len(headers) + 1):
            cell = ws.cell(1, c)
            cell.font = Font(bold=True)
            cell.alignment = Alignment(horizontal="center", vertical="center")

        for r in rows:
            # rows 内部顺序：[0]设定 [1]DAC实测 [2]DAC偏差 [3]code [4]ADC电压 [5]误差 [6]LSB
            ws.append([r[0], r[1], r[3], r[4], r[5], r[6]])

        for col, width in zip("ABCDEF", (16, 16, 11, 18, 14, 12)):
            ws.column_dimensions[col].width = width
        ws.freeze_panes = "A2"

        # ---------- ② 测试结果（仅最终结果） ----------
        rs = wb.create_sheet(ADC_SHEET_RESULT)
        rs.append(["项目", "内容"])
        for c in (1, 2):
            cell = rs.cell(1, c)
            cell.font = Font(bold=True)
            cell.alignment = Alignment(horizontal="center", vertical="center")

        s = summary or {}
        for key, value in (
            ("测试项", "ADC_IN_Voltage"),
            ("测试时间", s.get("time", time.strftime("%Y-%m-%d %H:%M:%S"))),
            ("扫描范围", s.get("range", "-")),
            ("有效点", s.get("valid", "-")),
            ("DAC 最大偏差", s.get("dac_err", "-")),
            ("DAC 容差", s.get("dac_tol", f"±{ADC_IN_DAC_TOL_MV:.1f} mV")),
            ("ADC 最大误差", s.get("adc_err", "-")),
            ("ADC 满量程", s.get("full_scale", "-")),
            ("ADC_VREF", s.get("vref", f"{ADC_IN_VREF_MV:.0f} mV")),
            ("判定", s.get("verdict", "-")),
        ):
            rs.append([key, value])

        # 判定行着色
        verdict_row = rs.max_row
        verdict = str(s.get("verdict", "")).upper()
        fill = ("C6EFCE" if "PASS" in verdict or "通过" in verdict
                else "FFC7CE" if "FAIL" in verdict or "未通过" in verdict
                else None)
        if fill:
            for c in (1, 2):
                rs.cell(verdict_row, c).fill = PatternFill("solid", fgColor=fill)

        rs.column_dimensions["A"].width = 18
        rs.column_dimensions["B"].width = 46

        out_dir = Path(__file__).resolve().parents[2] / ADC_IN_EXPORT_DIR
        out_dir.mkdir(parents=True, exist_ok=True)
        path = out_dir / path_hint
        wb.save(path)
        return path
    except Exception as e:
        print(f"   ⚠️ 导出 Excel 失败：{type(e).__name__}: {e}")
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


def _tia_trim_read(i2c, channel):
    """读取 [20:5] 中第 channel 路 TRIM_TIA_RESn<7:0>，失败返回 None。"""
    bit_lo = TIA_CHANNELS[channel]["trim_lo"]
    rd = _log_read(TIA_TRIM_ADDR, TIA_TRIM_INDEX)
    if not rd or len(rd) < 4:
        return None
    return rd[3 - bit_lo // 8] & 0xFF


def _tia_trim_write(i2c, channel, trim):
    """把第 channel 路的 8 位 TRIM_TIA_RESn 写入 [20:5]，保留其它两路。"""
    bit_lo = TIA_CHANNELS[channel]["trim_lo"]
    return _write_word_field(i2c, TIA_TRIM_ADDR, TIA_TRIM_INDEX,
                             bit_lo, 0xFF << bit_lo, trim & TIA_TRIM_MAX)


def _tia_res_sel_read(i2c, channel):
    """读取第 channel 路 RW_TIA_RES_SELn<2:0>，返回 (code, 跨阻Ω) 或 (None, None)。"""
    spec = TIA_CHANNELS[channel]
    res_sel = spec.get("res_sel")
    if not res_sel:
        return None, None
    addr, index, bit_lo = res_sel
    rd = _log_read(addr, index)
    if not rd or len(rd) < 4:
        return None, None
    word = (rd[0] << 24) | (rd[1] << 16) | (rd[2] << 8) | rd[3]
    code = (word >> bit_lo) & 0x07
    return code, TIA_RES_TABLE.get(code)


def _tia_ibias_sel_read(i2c=None):
    """读取 RW_TIA_IBIAS_SEL（[20:0] 的 bit[24:23]），失败返回 None。"""
    i2c = i2c or _get_i2c()
    rd = execute_read(i2c, TIA_IBIAS_SEL_ADDR, TIA_IBIAS_SEL_INDEX)
    if not rd or len(rd) < 4:
        return None
    word = (rd[0] << 24) | (rd[1] << 16) | (rd[2] << 8) | rd[3]
    return (word >> TIA_IBIAS_SEL_BIT_LO) & 0x03


def _tia_ibias_sel_write(i2c, code):
    """把 2 位 RW_TIA_IBIAS_SEL 写入 [20:0]（读-改-写，保留其它字段）。"""
    return _write_word_field(i2c, TIA_IBIAS_SEL_ADDR, TIA_IBIAS_SEL_INDEX,
                             TIA_IBIAS_SEL_BIT_LO, TIA_IBIAS_SEL_MASK,
                             code & 0x03)


def _tia_ibias_sel_ensure(i2c=None, code=TIA_IBIAS_SEL_DEFAULT, quiet=False):
    """确保 RW_TIA_IBIAS_SEL = code（默认 01），返回 True=已是目标值。

    初始化（进入测试模式）时写一次；之后每个用到 [20:0] 的测试项开始前再复核一次。
    读不到 [20:0] 时返回 False（不盲写，避免把其它字段写坏）。
    """
    i2c = i2c or _get_i2c()
    cur = _tia_ibias_sel_read(i2c)
    if cur is None:
        print(f"   ⚠️ 读取 RW_TIA_IBIAS_SEL（[{TIA_IBIAS_SEL_ADDR}:"
              f"{TIA_IBIAS_SEL_INDEX}] bit[24:23]）失败，无法确认/写入")
        return False

    if cur == code:
        if not quiet:
            print(f"   RW_TIA_IBIAS_SEL = 2'b{cur:02b}（保持）")
        return True

    if not _tia_ibias_sel_write(i2c, code):
        print(f"   ⚠️ 写入 RW_TIA_IBIAS_SEL = 2'b{code:02b} 失败")
        return False

    back = _tia_ibias_sel_read(i2c)
    if back != code:
        print(f"   ⚠️ RW_TIA_IBIAS_SEL 写入后回读为 "
              f"{'读取失败' if back is None else f'2\'b{back:02b}'}，与期望 "
              f"2'b{code:02b} 不一致")
        return False
    if not quiet:
        print(f"   RW_TIA_IBIAS_SEL：2'b{cur:02b} → 2'b{code:02b}"
              f"（[20:0] bit[24:23]）")
    return True


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


def _tia_res_sel_write(i2c, channel, code):
    """写第 channel 路 RW_TIA_RES_SELn<2:0>（读-改-写，保留寄存器其它字段）。

    寄存器位置由 TIA_CHANNELS 的 res_sel 给出；未配置时返回 False（不写）。
    """
    res_sel = TIA_CHANNELS[channel].get("res_sel")
    if not res_sel:
        return False
    addr, index, bit_lo = res_sel
    return _write_word_field(i2c, addr, index, bit_lo, 0x07 << bit_lo, code & 0x07)


#: _tia_select_res 的“用户取消”返回值（与“位置未知/读取失败”区分）
_TIA_RES_CANCEL = object()


def _tia_select_res(i2c, channel):
    """交互式选择跨阻档位：打印编码表，回车沿用当前值，输入 0~7 改档。

    返回：
        (code, 阻值Ω)   - 选定的档位
        (None, None)    - 寄存器位置未知或读取失败，调用方按默认 0.5 Ω 继续
        (_TIA_RES_CANCEL, None) - 用户输入 q 取消，调用方应中止本次测试
    """
    spec = TIA_CHANNELS[channel]
    field = spec["res_sel_field"]
    if not spec.get("res_sel"):
        print(f"   {field}：规范未给出寄存器位置，跳过阻值选择（按默认 0.5 Ω 判定）")
        return None, None

    cur_code, cur_r = _tia_res_sel_read(i2c, channel)
    if cur_code is None:
        print(f"   {field}：读取失败，跳过阻值选择（按默认 0.5 Ω 判定）")
        return None, None

    print(f"\n   当前跨阻档位 {field} = 3'b{cur_code:03b} → {cur_r} Ω")
    print("   可选档位：")
    for code, r in sorted(TIA_RES_TABLE.items()):
        mark = " ← 当前" if code == cur_code else ""
        print(f"      {code:03b} → {r:<4g} Ω   （200 mA 理论 {r * TIA_CURRENT_A * 1000:5.1f} mV）{mark}")

    while True:
        try:
            raw = input(f"   选择档位 0~7（回车保持 {cur_code:03b}，q 取消）: ").strip().lower()
        except (EOFError, KeyboardInterrupt):
            print()
            return _TIA_RES_CANCEL, None
        if not raw:
            return cur_code, cur_r
        if raw in ("q", "quit", "exit"):
            print("   ⏭️ 已取消")
            return _TIA_RES_CANCEL, None
        try:
            code = int(raw, 2) if raw.strip("01") == "" and len(raw) <= 3 else int(raw, 10)
        except ValueError:
            print("   ⚠️ 请输入 0~7（或 3 位二进制，如 010），回车保持当前值")
            continue
        if not 0 <= code <= 7:
            print("   ⚠️ 档位必须在 0~7 之间")
            continue
        r = TIA_RES_TABLE[code]
        if not _tia_res_sel_write(i2c, channel, code):
            print(f"   ❌ 写入 {field} 失败（回读异常），保持当前档位")
            return None, None
        readback_code, readback_r = _tia_res_sel_read(i2c, channel)
        if readback_code == code:
            print(f"   ✅ 已设置 {field} = 3'b{code:03b} → {r} Ω（回读一致）")
        else:
            print(f"   ⚠️ 写入后回读为 3'b{readback_code:03b}，与期望 3'b{code:03b} 不一致")
            if readback_code is not None:
                return readback_code, readback_r
        return code, r


def _tia_limits(target_mv=TIA_TARGET_MV):
    """TIA 判定范围：典型值 ± TIA_LIMIT_MV（默认 100 ± 10 → 90~110 mV）。"""
    return target_mv - TIA_LIMIT_MV, target_mv + TIA_LIMIT_MV


def _judge_tia(channel, value, extra_note="", target_mv=TIA_TARGET_MV):
    """按「典型值 ± TIA_LIMIT_MV」判定第 channel 路 TIA 电压，Typ 取自校准表。

    目标值（典型值）默认 100 mV（0.5 Ω × 200 mA）；选了别的跨阻档位时按
    R × 200 mA 换算，判定范围仍是 ±10 mV（绝对值，不随档位等比缩放）。
    """
    name = TIA_CHANNELS[channel]["name"]
    limits = _tia_limits(target_mv)
    return _judge_by_standard(f"{name}_Voltage", value, extra_note=extra_note,
                              limits=limits, typ=target_mv)


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
        txt_builder.DST.parent.mkdir(parents=True, exist_ok=True)
        txt_builder.DST.write_text(body, encoding='utf-8')
        txt_note = f"，已重新生成 {txt_builder.DST.relative_to(txt_builder.ROOT)}"
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


def _sync_tia_trim_to_table(channel, trim):
    """把修调后的 TRIM_TIA_RESn（[20:5] 的对应字节）同步到 MTP 配置表。"""
    spec = TIA_CHANNELS[channel]
    bit_lo = spec["trim_lo"]
    return _sync_mtp_word_field(TIA_TRIM_MPT_ADDR, TIA_TRIM_MPT_INDEX,
                                bit_lo, 0xFF << bit_lo, trim & TIA_TRIM_MAX,
                                spec["trim_field"])


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
    """VBG 电压测试：可选择「直接测量」或「TRIM 修调」。

    测试模式（_ask_test_mode）：
      0 直接测量 —— 写 [20:9] 选 VBG 通道 → 万用表测 WO → 按标准判定，
                     **不修改 VBGTRIM**；
      1 TRIM 修调 —— 测一次判定；已达标则不修调；不达标时扫描 3 位 VBGTRIM
                     （0~7），取与目标 1250 mV 偏差最小者写回、复测判定，
                     并把该值同步到 MTP 配置表（[20:4] ↔ phys 440）。
    """
    print("\n🚀 执行 VBG 电压测试 ...")
    print(f"   判定范围 {VBG_LIMITS_MV[0]:.0f} ~ {VBG_LIMITS_MV[1]:.0f} mV，"
          f"修调目标 {VBG_TARGET_MV:.0f} mV")
    _confirm_connection("请确定万用表连接WO")

    i2c = _get_i2c()
    cur = _vbg_trim_read(i2c)
    if cur is None:
        print(" 读取 VBGTRIM 失败，跳过本次测试")
        return None
    print(f"   当前修调值 = {cur}")

    mode = _ask_test_mode(
        "VBG", trim_field="VBGTRIM",
        direct_desc=f"只测一次并按标准判定，不改 VBGTRIM（当前 {cur}）")

    mv = _voltage_test("VBG电压", addr=20, index=9, write_data=[0x00, 0x00, 0x00, 0x20])
    time.sleep(0.5)

    if mv is None:
        print("❌ VBG 电压测量失败，跳过本次测试")
        return None

    passed = _judge_vbg(mv, extra_note=f"修调 {cur}")
    if mode == TEST_MODE_DIRECT:
        if passed is True:
            print(f"VBG 直接测量通过，测量值: {mv:.3f} mV，修调值为: {cur}（未修改）")
        else:
            print(f"VBG 直接测量未通过，测量值: {mv:.3f} mV，修调值为: {cur}（未修改）")
        print("✅ VBG 测试完成")
        return mv

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


# ---------------------- TIA 输出（菜单 7/8/9）---------------------
# 规范 2.6~2.8：源表灌 200 mA → 万用表测输出脚 → 减去 VCM → 修调 TRIM_TIA_RESn

def _smu_ensure(reason="本测试需要源表提供电流，无法继续"):
    """确保源表可用；初始化被跳过/失败时尝试重新初始化。返回 True=可用。"""
    idn = instr.smu_get_idn()
    if not idn:
        print("   ⚠️  源表未连接（初始化可能被跳过或失败），尝试重新初始化 ...")
        if not instr.smu_init():
            print(f"   ❌ 源表初始化失败：{reason}")
            return False
        idn = instr.smu_get_idn()
        if not idn:
            print("   ❌ 源表重新初始化后仍无响应，无法继续")
            return False

    print(f"   源表：{idn}")
    return True


def _tia_smu_ensure():
    """确保源表可用（TIA 专用提示语）。"""
    return _smu_ensure("TIA 测试必须由源表灌 200 mA，无法继续")


def _psu_ensure(reason="本测试需要可编程电源调压，无法继续"):
    """确保可编程电源（DP832A）可用；初始化被跳过/失败时尝试重新初始化。

    注意：psu_init() 默认不复位、不改动任何通道输出，可安全用于测试过程中重连。

    返回：True=可用，False=不可用
    """
    idn = instr.psu_get_idn()
    if not idn:
        print("   ⚠️  电源未连接（初始化可能被跳过或失败），尝试重新初始化 ...")
        if not instr.psu_init():
            print(f"   ❌ 电源初始化失败：{reason}")
            return False
        idn = instr.psu_get_idn()
        if not idn:
            print("   ❌ 电源重新初始化后仍无响应，无法继续")
            return False

    print(f"   电源：{idn}")
    return True


def _dmm_ensure(reason="本测试需要万用表测电压，无法继续"):
    """确保万用表可用（会话活着）；初始化被跳过/失败/会话失效时尝试重连。

    只判断 dmm_instance 是否为 None 是不够的：别的驱动（如示波器/电源初始化失败）
    若关掉了共享的 ResourceManager，万用表对象还在但 VISA 会话已失效，
    之后每次读写都会报 "Invalid session handle"。这里用 *IDN? 实际探活。

    返回 True=可用。
    """
    dmm_mod = getattr(instr, "_dmm", None)
    if getattr(dmm_mod, "dmm_instance", None) is not None:
        if instr.dmm_check_session():
            return True
        print("   ⚠️ 万用表会话已失效 → 重新初始化 ...")
    else:
        print("   ⚠️  万用表未连接（初始化可能被跳过或失败）→ 尝试重新初始化 ...")

    if not instr.dmm_init():
        print(f"   ❌ 万用表初始化失败：{reason}")
        return False
    if getattr(dmm_mod, "dmm_instance", None) is None:
        print("   ❌ 万用表重新初始化后仍无会话，无法继续")
        return False
    if not instr.dmm_check_session():
        print("   ❌ 万用表重新初始化后会话仍不可用，无法继续")
        return False
    return True


@contextmanager
def _tia_current_source():
    """源表灌流上下文：电流源 200 mA / 电压档 / 限压 5 V / 上电 → 校验 → 退出必关输出。

    只下发五步（instrument_set.smu_source_current_on）：
        :SOURce:FUNCtion CURR
        :SENSe:FUNCtion "VOLTage"
        :SOURce:CURRent:VLIMit 5
        :SOURce:CURRent 0.2
        :OUTPut:STATe ON
    之后回读输出状态与源表实测 V/I，确认 200 mA 真的灌进被测脚。

    任何一步失败都抛 RuntimeError（由 _tia_test 捕获打印），避免“看起来测了、
    其实一点电流都没灌进去”的假数据进入判定和 MTP 表。
    """
    if not instr.smu_source_current_on(TIA_CURRENT_A, voltage_limit=TIA_SMU_VLIMIT_V):
        raise RuntimeError("源表上电失败（终端已打印具体是哪条 SCPI 被拒）")
    instr.smu_print_state("   源表配置回读：")

    try:
        state = instr.smu_output_state()
        if state is not True:
            raise RuntimeError(f"源表输出未打开（:OUTPut:STATe? = {state}）")

        time.sleep(TIA_SETTLE_S)

        # 回读源表自己的 V/I：确认 200 mA 真的灌出去了
        v_smu, i_smu = instr.smu_measure()
        if v_smu is None or i_smu is None:
            print("   ⚠️  源表回读失败（无法确认输出），继续测量 ...")
        else:
            print(f"   源表输出：V = {v_smu:.4f} V，I = {i_smu * 1000:.3f} mA"
                  f"（限压 {TIA_SMU_VLIMIT_V:.1f} V）")
            if abs(i_smu) < TIA_CURRENT_A * TIA_CURRENT_MIN_RATIO:
                if abs(v_smu) >= TIA_SMU_VLIMIT_V * 0.9:
                    hint = ("电压已顶到限压，回路不通——"
                            "检查被测脚是否接触、继电器/通道是否切对")
                elif abs(v_smu) < 0.05:
                    hint = "源表端电压≈0，输出可能仍处于关闭态"
                else:
                    hint = "回路电流明显偏小，检查接线与限压设置"
                raise RuntimeError(f"源表没有灌出 {TIA_CURRENT_A * 1000:.0f} mA"
                                   f"（实测 {i_smu * 1000:.3f} mA）：{hint}")

        yield
    finally:
        try:
            if not instr.smu_output_off():
                print("   ⚠️  源表关闭输出返回失败，请手动确认面板 OUTPUT 已灭")
        except Exception as e:
            print(f"   ⚠️ 关闭源表输出异常：{e}")


def _trim_bisect_search(measure, target, trim_max, initial=None):
    """二分搜索使测值最接近 target 的修调 code（TIA / OSC 共用）。

    先测 0 / trim_max 两端判断测值随 code 的单调方向（规范是 code↑ 测值↑，
    但按实测方向自适应），再二分收敛到跨过 target 的两个 code，
    补测这两个 code 的邻近点，最后取偏差最小者。

    measure 内部负责「写修调值 + 等稳定 + 测量」，返回 None 表示该 code
    测量失败（遇到 None 会提前结束二分，已测到的点仍参与最终比较）。

    参数：
        measure:  callable(code) -> 测值 或 None
        target:   float - 目标值（mV / Hz，与 measure 返回同单位）
        trim_max: int - 修调字段最大值（TIA 8 bit = 255，OSC 6 bit = 63）
        initial:  dict - 已经测过的 {code: 测值}，参与比较但不重复测量
    返回：
        (best_code, best_value)；一次都没测到有效值返回 (None, None)
    """
    results = dict(initial or {})

    def ev(code):
        if code not in results:
            results[code] = measure(code)
        return results[code]

    # 方向判定；端点测量失败时按“code↑ 测值↑”处理（initial 里的点兜底）
    v_lo, v_hi = ev(0), ev(trim_max)
    increasing = True if None in (v_lo, v_hi) else v_hi >= v_lo
    if v_lo is not None and v_hi is not None:
        print(f"   二分方向：code↑ → 测值{'↑' if increasing else '↓'}"
              f"（code 0 = {v_lo:.3f}，code {trim_max} = {v_hi:.3f}）")
    else:
        print("   二分方向：端点测量不全，按 code↑ → 测值↑ 处理")

    lo, hi = 0, trim_max
    while lo <= hi:
        mid = (lo + hi) // 2
        val = ev(mid)
        if val is None:
            print(f"   修调值={mid} 测量失败，结束二分")
            break
        if (val < target) == increasing:    # 测值偏低且方向为正 → 加大 code
            lo = mid + 1
        else:
            hi = mid - 1

    # 收敛点两侧的邻近 code 再补测，避免平台区/非严格单调时选错
    for code in (hi - 1, hi, lo, lo + 1):
        if 0 <= code <= trim_max:
            ev(code)

    valid = {c: v for c, v in results.items() if v is not None}
    if not valid:
        return None, None
    best_code = min(valid, key=lambda c: (abs(valid[c] - target), c))
    return best_code, valid[best_code]


def _tia_trim_search(measure, target=TIA_TARGET_MV, initial=None):
    """二分搜索使 TIA 电压最接近 target 的 8 位修调 code。

    实现见 _trim_bisect_search()；这里的 trim_max 固定为 TIA_TRIM_MAX(255)。
    """
    return _trim_bisect_search(measure, target, TIA_TRIM_MAX, initial)


def _tia_vcm(i2c):
    """取 VCM 电压（mV）：已记录则沿用（可输入 m 重测），否则测量并记录。"""
    cached = TIA_VCM_CACHE["mv"]
    if cached is not None:
        print(f"   已记录 VCM = {cached:.3f} mV")
        try:
            ans = input("   回车沿用该 VCM，输入 m 重新测量: ").strip().lower()
        except (EOFError, KeyboardInterrupt):
            print()
            ans = ""
        if ans != "m":
            return cached

    if not _confirm_connection("请确定万用表接 WO（测 VCM），源表输出保持关闭"):
        return None
    execute_write(i2c, addr=20, index=9, write_data=TIA_VCM_SEL)
    _log_read(20, 9)
    time.sleep(TIA_SETTLE_S)
    vcm = instr.dmm_measure_single_voltage()
    if vcm is None:
        print("❌ VCM 测量失败，跳过本次测试")
        return None
    TIA_VCM_CACHE["mv"] = vcm
    print(f"   VCM = {vcm:.3f} mV（已记录）")
    return vcm


def _tia_state_machine_init(i2c):
    """初始化并启动测试状态机：先 [1:32]，再 [1:31]。

    必须在写 TIA 通道 [20:9] 之前执行，否则通道切换不会生效。
    返回 True（写操作本身无回读校验，异常会直接抛出）。
    """
    init_hex = ' '.join(f'{b:02X}' for b in TIA_SM_INIT_DATA)
    print(f"   初始化状态机：[{TIA_SM_ADDR}:{TIA_SM_INIT_INDEX}] = {init_hex}")
    execute_write(i2c, addr=TIA_SM_ADDR, index=TIA_SM_INIT_INDEX,
                  write_data=list(TIA_SM_INIT_DATA))
    time.sleep(0.01)   # 寄存器写间隔

    start_hex = ' '.join(f'{b:02X}' for b in TIA_SM_START_DATA)
    print(f"   启动状态机：  [{TIA_SM_ADDR}:{TIA_SM_START_INDEX}] = {start_hex}")
    execute_write(i2c, addr=TIA_SM_ADDR, index=TIA_SM_START_INDEX,
                  write_data=list(TIA_SM_START_DATA))
    time.sleep(0.01)   # 寄存器写间隔
    return True


def _tia_sweep_all_res(channel, i2c, vcm, sel):
    """直接测量·全部跨阻档位：RW_TIA_RES_SELn 依次 000~111，逐档测量并判定。

    - 通道 [20:9] 只写一次，源表全程保持灌流（不反复开关输出）；
    - 每档：写档位 → 回读校验 → 稳定 → 万用表测 meas_pin → 减 VCM；
    - 判定范围仍为「R × 200 mA ± 10 mV」，每档单独判定并单独写入结果记录；
    - 扫描结束恢复成扫描前的档位；**全程不改 TRIM_TIA_RESn**。

    返回最后一档的有效电压（mV）；无任何有效测量时返回 None。
    """
    spec = TIA_CHANNELS[channel]
    name, meas_pin = spec["name"], spec["meas"]
    field = spec["res_sel_field"]
    ma = TIA_CURRENT_A * 1000.0

    if not spec.get("res_sel"):
        print(f"   ⚠️ {field}：规范未给出寄存器位置，无法扫描全部档位")
        return None

    orig_code, _ = _tia_res_sel_read(i2c, channel)
    codes = sorted(TIA_RES_TABLE)
    orig_txt = "未知" if orig_code is None else "3'b{:03b}".format(orig_code)
    print(f"\n   全部跨阻档位扫描：{field} 依次 3'b000 ~ 3'b111，共 {len(codes)} 档"
          f"（扫描前 {orig_txt}）")
    print(f"   源表持续往 {spec['src']} 灌 {ma:.0f} mA，万用表测 {meas_pin}")

    execute_write(i2c, addr=20, index=9, write_data=sel)
    _log_read(20, 9)
    time.sleep(TIA_SETTLE_S)

    print("   " + "-" * 80)
    print(f"   {'档位':>4} {'跨阻(Ω)':>8} {'典型(mV)':>9} {'测量(mV)':>10} "
          f"{'实测R(Ω)':>10} {'偏差(mV)':>10}")
    print("   " + "-" * 80)

    rows, last_mv = [], None
    try:
        with _tia_current_source():
            for code in codes:
                r_cfg = TIA_RES_TABLE[code]
                target_mv = r_cfg * ma

                if not _tia_res_sel_write(i2c, channel, code):
                    print(f"   {code:03b} ❌ 写 {field} 失败（回读异常），跳过该档")
                    continue
                back, _ = _tia_res_sel_read(i2c, channel)
                if back != code:
                    print(f"   {code:03b} ⚠️ 回读 3'b{back:03b} 与写入 3'b{code:03b} "
                          f"不一致，跳过该档")
                    continue
                time.sleep(TIA_SETTLE_S)

                raw = instr.dmm_measure_single_voltage()
                if raw is None:
                    print(f"   {code:03b} ❌ {meas_pin} 电压测量失败，跳过该档")
                    continue
                mv = raw - vcm
                r_meas = mv / ma
                print(f"   {code:03b} {r_cfg:8.2f} {target_mv:9.1f} {mv:10.3f} "
                      f"{r_meas:10.4f} {mv - target_mv:+10.3f}")

                # 逐档判定并写入结果记录（备注里带档位，同名的多行不会互相覆盖）
                passed = _judge_tia(
                    channel, mv, target_mv=target_mv,
                    extra_note=f"{field}=3'b{code:03b}（{r_cfg} Ω），R={r_meas:.4f} Ω")
                rows.append((code, r_cfg, target_mv, mv, r_meas, passed))
                last_mv = mv
    except RuntimeError as e:
        print(f"❌ {name} 中止：{e}")
    except Exception as e:
        print(f"❌ {name} 测试异常：{type(e).__name__}: {e}")
    finally:
        # 扫完恢复到扫描前的档位，避免把芯片留在最后一个测过的档位上
        if orig_code is not None:
            if _tia_res_sel_write(i2c, channel, orig_code):
                print(f"   已恢复 {field} = 3'b{orig_code:03b}")
            else:
                print(f"   ⚠️ 恢复 {field} = 3'b{orig_code:03b} 失败")

    print("   " + "-" * 80)
    if not rows:
        print(f"❌ {name} 全部档位扫描失败：没有任何有效测量点")
        return None

    ok_rows = [r for r in rows if r[5] is True]
    print(f"   扫描有效 {len(rows)}/{len(codes)} 档，判定通过 {len(ok_rows)} 档")
    for code, r_cfg, target_mv, mv, r_meas, passed in rows:
        verdict = "PASS" if passed is True else ("NO-STD" if passed is None else "FAIL")
        print(f"      3'b{code:03b} {r_cfg:>4g} Ω：{mv:9.3f} mV"
              f"（典型 {target_mv:6.1f}，偏差 {mv - target_mv:+7.3f} mV）→ {verdict}")

    ref = ok_rows or rows
    best = min(ref, key=lambda r: abs(r[3] - r[2]))
    mark = "✅ 通过档位中" if ok_rows else "⚠️ 无通过档位，最接近典型值的是"
    print(f"   {mark} 3'b{best[0]:03b} → {best[1]} Ω：{best[3]:.3f} mV"
          f"（偏差 {best[3] - best[2]:+.3f} mV）")

    print(f"✅ {name} 全部跨阻档位扫描完成")
    return last_mv


def _tia_test(channel):
    """TIA_OUTn 电压测试（规范 2.6~2.8，菜单 7/8/9），可选择「直接测量」或「TRIM 修调」。

    流程：
      0. 选择测试模式：
         0 直接测量 —— 测一次并按**典型值 ±10 mV** 判定，**不改 TRIM_TIA_RESn**；
         2 直接测量·全部跨阻档位 —— RW_TIA_RES_SELn 依次 000~111 逐档测量并判定，
             不改 TRIM_TIA_RESn（见 _tia_sweep_all_res）；
         1 TRIM 修调 —— 判定不通过时二分搜索最优修调值、写回复测，并同步 MTP；
      1. 取 VCM 电压：已记录就沿用，否则写 [20:9] = 00 00 01 E0 测 WO 并记录
      2. 初始化状态机 [1:32]、启动状态机 [1:31]
      3. 选择跨阻档位 RW_TIA_RES_SELn（[20:0] 的 [12:10] / [15:13] / [18:16]）
      4. 写 TIA 通道 [20:9]，源表往 src_pin 灌 200 mA（电压限值 5 V），
         万用表测 meas_pin，TIA_OUTn 电压 = meas_pin − VCM
         跨阻 R = TIA_OUTn 电压 / 200 mA
      5. 判定：典型值 = R × 200 mA，判定范围 = 典型值 ± 10 mV
         （默认 0.5 Ω 档即 90~110 mV）；修调模式下不通过时二分搜索
         TRIM_TIA_RESn<7:0>（[20:5] 的 [7:0] / [15:8] / [23:16]）
         （规范：电压偏大 → code 调小，偏小 → code 调大）
      6. 修调模式：写回最优 code 复测并判定，最后同步到 MTP 配置表（[20:5] ↔ phys 444）；
         若所有修调值都不在范围内，取与典型值偏差最小的 code（记录备注中标明）
    """
    spec = TIA_CHANNELS[channel]
    name, sel = spec["name"], spec["sel"]
    src_pin, meas_pin = spec["src"], spec["meas"]
    field = spec["trim_field"]
    ma = TIA_CURRENT_A * 1000.0

    print(f"\n🚀 执行 {name} 电压测试 ...")
    print(f"   判定范围 = 典型值 ±{TIA_LIMIT_MV:.0f} mV"
          f"（0.5 Ω 档即 {TIA_LIMITS_MV[0]:.0f} ~ {TIA_LIMITS_MV[1]:.0f} mV）")
    print(f"   源表往 {src_pin} 灌 {ma:.0f} mA（限压 {TIA_SMU_VLIMIT_V:.1f} V），"
          f"万用表测 {meas_pin}，减去 VCM 得到 {name} 电压")

    mode = _ask_test_mode(
        name, trim_field=field,
        direct_desc=f"测一次并按典型值 ±{TIA_LIMIT_MV:.0f} mV 判定，不改 {field}",
        extra_modes=[(TEST_MODE_DIRECT_ALL_RES, "直接测量·全部跨阻档位",
                      f"{spec['res_sel_field']} 依次 000~111 逐档测量并判定，不改 {field}")])

    i2c = _get_i2c()

    # ⓪ 确认源表在线（初始化被跳过/失败时尝试重建连接）
    if not _tia_smu_ensure():
        return None

    # ① VCM（此时源表不输出）
    vcm = _tia_vcm(i2c)
    if vcm is None:
        return None

    # ② 选通道 + 灌流
    if meas_pin != "WO":
        note = f"请确定源表接 {src_pin}、万用表改接 {meas_pin}"
    else:
        note = f"请确定源表接 {src_pin}、万用表接 {meas_pin}"
    if not _confirm_connection(note):
        return None

    # 状态机初始化 + 启动（必须在写 TIA 通道 [20:9] 之前）
    _tia_state_machine_init(i2c)

    # 跨阻档位选择（RW_TIA_RES_SELn，规范默认 000 = 0.5 Ω）
    # 先复核 [20:0] 的 RW_TIA_IBIAS_SEL = 01，后面的档位写入是读-改-写，不会动这两位
    _tia_ibias_sel_ensure(i2c)

    # 模式 2：不选单一档位，直接依次扫描全部 8 个跨阻档位
    if mode == TEST_MODE_DIRECT_ALL_RES:
        return _tia_sweep_all_res(channel, i2c, vcm, sel)

    sel_code, r_cfg = _tia_select_res(i2c, channel)
    if sel_code is _TIA_RES_CANCEL:
        return None
    if sel_code is None:
        r_cfg = TIA_RES_TABLE[TIA_RES_DEFAULT_CODE]
        sel_code = TIA_RES_DEFAULT_CODE

    execute_write(i2c, addr=20, index=9, write_data=sel)
    _log_read(20, 9)
    time.sleep(TIA_SETTLE_S)

    # 典型值（目标电压）随档位换算：V = R × 200 mA；判定范围 = 典型值 ± 10 mV
    target_mv = r_cfg * ma
    limit_lo, limit_hi = _tia_limits(target_mv)
    print(f"   {spec['res_sel_field']} = 3'b{sel_code:03b} → {r_cfg} Ω，"
          f"典型值 {target_mv:.1f} mV（判定 {limit_lo:.1f} ~ {limit_hi:.1f} mV）")

    try:
        with _tia_current_source():
            raw = instr.dmm_measure_single_voltage()
            if raw is None:
                print("❌ 输出脚电压测量失败，跳过本次测试")
                return None
            mv = raw - vcm
            r_meas = mv / ma          # mV / mA = Ω
            print(f"   {meas_pin} = {raw:.3f} mV，"
                  f"{name} = {meas_pin} − VCM = {mv:.3f} mV")
            print(f"   跨阻 R = {mv:.3f} mV / {ma:.0f} mA = {r_meas:.4f} Ω"
                  + (f"（配置值 {r_cfg} Ω，偏差 {r_meas - r_cfg:+.4f} Ω）"
                     if r_cfg is not None else ""))

            cur = _tia_trim_read(i2c, channel)
            if cur is None:
                print(f"   读取 {field} 失败，跳过修调")
                _judge_tia(channel, mv, target_mv=target_mv)
                return mv
            print(f"   当前 {field} = {cur}")

            passed = _judge_tia(channel, mv, extra_note=f"{field}={cur}",
                                target_mv=target_mv)
            if mode == TEST_MODE_DIRECT:
                if passed is True:
                    print(f"{name} 直接测量通过，测量值: {mv:.3f} mV"
                          f"（R={r_meas:.4f} Ω），修调值为: {cur}（未修改）")
                else:
                    print(f"{name} 直接测量未通过，测量值: {mv:.3f} mV"
                          f"（R={r_meas:.4f} Ω），修调值为: {cur}（未修改）")
                print(f"✅ {name} 测试完成")
                return mv

            if passed is True:
                print(f"{name} 测试通过, 测量值: {mv:.3f} mV（R={r_meas:.4f} Ω）, "
                      f"修调值为: {cur}, 无需修调")
                print(f"✅ {name} 测试完成")
                return mv

            def measure_at(code):
                """写修调 code 后测 TIA 电压（mV），失败返回 None。"""
                if not _tia_trim_write(i2c, channel, code):
                    print(f"   修调值={code:3d}: 写入失败（回读异常），跳过")
                    return None
                time.sleep(TIA_SETTLE_S)
                val_raw = instr.dmm_measure_single_voltage()
                if val_raw is None:
                    print(f"   修调值={code:3d}: 电压测量失败，跳过")
                    return None
                val = val_raw - vcm
                print(f"   修调值={code:3d}: 电压={val:8.3f} mV, "
                      f"R={val / ma:7.4f} Ω, "
                      f"误差={abs(val - target_mv):7.3f} mV")
                return val

            print(f"   开始二分搜索 {field}（0~{TIA_TRIM_MAX}，"
                  f"code 增大电压增大/减小均可）...")
            best_code, best_scanned = _tia_trim_search(measure_at, target=target_mv,
                                                       initial={cur: mv})
            if best_code is None:
                print(f"❌ {name} 修调失败：扫描期间没有测到有效电压")
                return None

            # 全部修调值都落在判定范围外时，仍取偏差最小的那个（最近值）
            nearest_only = not (limit_lo <= best_scanned <= limit_hi)
            if nearest_only:
                print(f"   ⚠️ 所有修调值均不在判定范围 {limit_lo:.1f} ~ {limit_hi:.1f} mV 内，"
                      f"取最近值 code={best_code}（{best_scanned:.3f} mV，"
                      f"偏差 {best_scanned - target_mv:+.3f} mV）")

            # 写回最优修调值并复测
            if not _tia_trim_write(i2c, channel, best_code):
                print(f"❌ {name} 修调失败：写回修调值 {best_code} 时回读异常")
                return None
            time.sleep(TIA_SETTLE_S)
            best_raw = instr.dmm_measure_single_voltage()
            if best_raw is None:
                print(f"❌ {name} 修调失败：写回修调值 {best_code} 后测量失败")
                return None
            best_vol = best_raw - vcm
    except RuntimeError as e:
        print(f"❌ {name} 中止：{e}")
        return None
    except Exception as e:
        print(f"❌ {name} 测试异常：{type(e).__name__}: {e}")
        return None

    note = f"{field}={best_code}" + ("，所有修调值均超差，取最近值" if nearest_only else "")
    passed = _judge_tia(channel, best_vol, extra_note=note, target_mv=target_mv)
    result = "成功" if passed is True else "失败"
    print(f"{name} 修调{result}, 测量值: {best_vol:.3f} mV"
          f"（R={best_vol / ma:.4f} Ω）, 修调值为: {best_code}"
          f"（偏差 {best_vol - target_mv:+.3f} mV）")

    # 修调完成后，把 TRIM_TIA_RESn 同步到 MTP 配置表（[20:5] ↔ MTP phys 444）
    _sync_tia_trim_to_table(channel, best_code)

    print(f"✅ {name} 测试完成")
    return best_vol


def TIA_OUT1_Voltage():
    """TIA_OUT1：源表给 UO 灌 200 mA、万用表测 WO，减 VCM 得电压，判定/修调 TRIM_TIA_RES0。"""
    return _tia_test(1)


def TIA_OUT2_Voltage():
    """TIA_OUT2：源表给 VO 灌 200 mA、万用表测 WO，减 VCM 得电压，判定/修调 TRIM_TIA_RES1。"""
    return _tia_test(2)


def TIA_OUT3_Voltage():
    """TIA_OUT3：源表给 WO 灌 200 mA、万用表测 UO，减 VCM 得电压，判定/修调 TRIM_TIA_RES2。"""
    return _tia_test(3)


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
    """ADC_VREF 电压测试：可选择「直接测量」或「TRIM 修调」，**默认直接测量**。

    测试模式（回车 = 0 直接测量）：
      0 直接测量 —— 写 [20:9] = 00 00 02 E0 选 ADC_VREF 通道 → 万用表测 WO →
                     按标准判定，**不修改 RW_VREF_TRIM**；
      1 TRIM 修调 —— 测一次判定；已达标则不修调；不达标时按下述流程修调：

    修调流程：
      1. 写修调值 0 读电压 V1，写修调值 10 读电压 V2，得到每步电压变化；
      2. 按 (2000 - V1) / 每步 估算使电压到 2000 mV 的修调值并实测；
      3. 实测仍在判定范围外时，在估算值 ±10 内扫描取最优，写回并再判定。

    RW_VREF_TRIM 位于 [20:4] 的 bit[7:0]，与 VBGTRIM / RW_OSC_24M_TRIM 共用寄存器，
    写入采用读-改-写，不会破坏其它字段。
    """
    print("\n🚀 执行 ADC_VREF 电压测试 ...")
    print(f"   判定范围 {ADC_VREF_LIMITS_MV[0]:.0f} ~ {ADC_VREF_LIMITS_MV[1]:.0f} mV，"
          f"修调目标 {ADC_VREF_TARGET_MV:.0f} mV")
    _confirm_connection("请确定万用表连接WO")

    i2c = _get_i2c()
    cur = _vref_trim_read(i2c)
    if cur is None:
        print(" 读取 RW_VREF_TRIM 失败，跳过本次测试")
        return None
    print(f"   当前修调值 = {cur}")

    mode = _ask_test_mode(
        "ADC_VREF", trim_field="RW_VREF_TRIM", default=TEST_MODE_DIRECT,
        direct_desc=f"只测一次并按标准判定，不改 RW_VREF_TRIM（当前 {cur}）")

    # TEST_ANA<3:0> = 1011（ADC_VREF 通道）；原 00 00 02 A0 是 VDD2 的 1010
    mv = _voltage_test("ADC_VREF电压", addr=20, index=9, write_data=[0x00, 0x00, 0x02, 0xE0])
    time.sleep(0.5)

    if mv is None:
        print("❌ ADC_VREF 电压测量失败，跳过本次测试")
        return None

    passed = _judge_vref(mv, extra_note=f"修调 {cur}")
    if mode == TEST_MODE_DIRECT:
        if passed is True:
            print(f"ADC_VREF 直接测量通过，测量值: {mv:.3f} mV，修调值为: {cur}（未修改）")
        else:
            print(f"ADC_VREF 直接测量未通过，测量值: {mv:.3f} mV，修调值为: {cur}（未修改）")
        print("✅ ADC_VREF 测试完成")
        return mv

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
    """ADC_IN 测试（菜单 [14]）：DAC 从 W 脚输入电压，ADC 采样并列表。

    流程（寄存器顺序不能改）：
      1. **前置检查**：读 [20:2]，必须为 00 00 00 03（ADC 时钟 16 MHz，
         MTP phys 432 = 0x03）；不满足时提示并中止；
      2. 状态机选 INT：[1:32] = 00 43 FD 22 → 启动：[1:31] = 00 01 02 00；
      3. ADC 测试使能：[20:9] = 00 00 03 20（TEST_ANA = 1100）；
      4. 选择 4 通道：[3:1] = 00 00 00 10；数据右对齐、忽略 adc_error：
         [3:0] = 00 00 0D 81，写完立即回读，**最低字节 = 81 即通过**；
         若回读最低字节不是 81，执行恢复流程（[3:0]=00 00 0C 01 → [1:32]=00 00 00 10 →
         [25:5]=5A5A5A5A → [1:31]=00 00 02 00，读 [1:32] 低两位应为 10 →
         [1:31]=00 00 00 00 → [1:32]=00 00 00 20 → [1:31]=00 00 02 00 →
         [25:5]=5A5A5A5A，读 [1:32] 应为 21 → [3:0]=00 00 0C 81，
         读 [3:0] 最低字节应为 81）；恢复失败则测试终止；
      5. 交互式选择扫描范围（上限固定 2000 mV）与步进（默认 10 mV）；
      6. 每点：UART 下发 `spi_cmd_set_val mV <V>` 设 DAC → 万用表实测 WO 电压 →
         写 [3:0] = 00 00 0D C1 更新 ADC → 读 [3:7] 得 ADC code；
         按 `Vadc = code × ADC_VREF / ADC_FULL_SCALE` 换算，逐点打印
         `DAC实测 | ADC code | ADC转换电压`；
         扫完用实测数据反推满量程做 LSB 换算自检（见 _adc_estimate_full_scale）；
      7. 结束后 DAC 回 0，记录结果，可选导出 xlsx。

    判定：DAC 输出最大偏差 ≤ ADC_IN_DAC_TOL_MV。
    """
    print("\n🚀 执行 ADC_IN（DAC 输出 + ADC 采样）测试 ...")
    _confirm_connection("请确定万用表连接WO")

    i2c = _get_i2c()

    # ---- ① 前置检查：ADC 时钟必须已选 16 MHz ----
    print("\n   检查 ADC 时钟配置 ...")
    if not _adc_check_clock(i2c):
        print("❌ ADC 时钟前置检查未通过，测试终止")
        _ensure_result_session()
        result_log.record("ADC_IN_Voltage", None, "mV", result="FAIL",
                          note="ADC 时钟未选 16 MHz（[20:2] ≠ 00 00 00 03）")
        return None

    # ---- 扫描参数（可交互修改）----
    print(f"\n   DAC 输出电压上限 {ADC_IN_VMAX_MV} mV（规格 WO 输入不超过 2 V）")
    start = _ask_int("起始电压 mV", ADC_IN_DAC_START_MV, 0, ADC_IN_VMAX_MV)
    stop = _ask_int("结束电压 mV", ADC_IN_VMAX_MV, 0, ADC_IN_VMAX_MV)
    step = _ask_int("步进 mV", ADC_IN_DAC_STEP_MV, 1, ADC_IN_VMAX_MV)
    if start > stop:
        start, stop = stop, start
    points = list(range(start, stop + 1, step))
    if not points:
        print("❌ 扫描点为空，测试终止")
        return None

    print(f"   扫描 {points[0]} ~ {points[-1]} mV，步进 {step} mV，共 {len(points)} 点，"
          f"单点容差 ±{ADC_IN_DAC_TOL_MV:.1f} mV")

    # ---- ② ~ ④ ADC 初始化 ----
    print("\n   ADC 初始化 ...")
    if not _adc_setup(i2c):
        print("❌ ADC 初始化失败：[3:0] 回读最低字节不是 81 且恢复流程未通过，测试终止")
        _ensure_result_session()
        result_log.record("ADC_IN_Voltage", None, "mV", result="FAIL",
                          note="ADC 初始化失败：[3:0] 回读最低字节不是 81，恢复流程未通过")
        return None

    lsb_mv = ADC_IN_VREF_MV / ADC_FULL_SCALE
    print(f"   ADC 换算：Vadc = code × {ADC_IN_VREF_MV:.0f} mV / {ADC_FULL_SCALE}"
          f"（1 LSB = {lsb_mv:.4f} mV）")
    rows, max_dac_err, max_adc_err = [], 0.0, 0.0

    print("\n   " + "-" * 84)
    print(f"   {'设定(mV)':>9} {'DAC测量值':>11} {'DAC偏差':>9} "
          f"{'ADC code':>9} {'ADC电压(mV)':>12} {'误差(DAC-ADC)':>14} {'LSB':>9}")
    print("   " + "-" * 84)

    for vin in points:
        try:
            # 只发不等固定长度应答：send_and_receive 会阻塞到串口超时（2 s）
            instr.serial_send_command(f"spi_cmd_set_val mV {int(vin)}\r\n")
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

        # 每次 DAC 电压变化后：先更新 ADC 数据，再读码值
        code = _adc_sample(i2c)
        if code is None:
            v_adc = adc_err = adc_lsb = None
            code_txt = adc_txt = err_txt = lsb_txt = "--"
        else:
            v_adc = code * lsb_mv
            adc_err = meas - v_adc          # 误差 = DAC测量值 − ADC转换电压
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
        print(f"   {vin:9d} {meas:11.3f} {dac_err:+9.3f} "
              f"{code_txt:>9} {adc_txt:>12} {err_txt:>14} {lsb_txt:>9}")

        # 每一次测量都写入结果记录：实测值列放 ADC code，备注里带上电压换算
        _ensure_result_session()
        point_note = f"设定 {vin} mV，DAC测量 {meas:.3f} mV"
        if code is not None:
            point_note += (f"，ADC电压 {v_adc:.3f} mV，"
                           f"误差(DAC−ADC) {adc_err:+.3f} mV（{adc_lsb:+.2f} LSB）")
        else:
            point_note += "，ADC 读取失败"
        result_log.record(f"ADC_IN_{vin}mV", code, "code",
                          result="MEASURED" if code is not None else "FAIL",
                          note=point_note)

    print("   " + "-" * 84)

    # DAC 回 0，避免测试结束后 WO 仍有电压
    try:
        instr.serial_send_command("spi_cmd_set_val mV 0\r\n")
    except Exception:
        pass

    valid = [r for r in rows if r[1] is not None]
    if not valid:
        print("❌ ADC_IN 测试失败：无有效测量点")
        _ensure_result_session()
        result_log.record("ADC_IN_Voltage", None, "mV", result="FAIL", note="无有效测量点")
        return None

    adc_rows = [r for r in rows if r[3] is not None]
    print(f"\n   有效点 {len(valid)}/{len(points)}，DAC 最大偏差 {max_dac_err:.3f} mV")
    if adc_rows:
        print(f"   ADC 有效点 {len(adc_rows)}/{len(rows)}，"
              f"ADC 最大误差 {max_adc_err:.3f} mV（{max_adc_err / lsb_mv:.2f} LSB）")
    else:
        print("   ⚠️ 所有点都未读到 ADC code，请检查 [3:7] 与状态机配置")

    # LSB 换算自检：用实测数据反推满量程，确认 ADC_FULL_SCALE / VREF 用得对不对
    fs_est, fs_pts = _adc_estimate_full_scale(rows)
    if fs_est is not None:
        ratio = fs_est / ADC_FULL_SCALE
        print(f"   LSB 换算自检：推断满量程 ≈ {fs_est:.0f}"
              f"（配置 {ADC_FULL_SCALE}，比值 {ratio:.2f}，{fs_pts} 点）")
        if 0.8 <= ratio <= 1.25:
            print("   ✅ LSB 换算与实测一致")
        elif 3.2 <= ratio <= 5.0:
            print(f"   ⚠️ 实测约为配置值的 {ratio:.1f} 倍：数据寄存器可能是 14 bit 满量程，"
                  f"请把 ADC_FULL_SCALE 改为 16384 后重测（当前 ADC 电压/误差列会偏大 {ratio:.1f} 倍）")
        else:
            print(f"   ⚠️ 推断满量程与配置相差 {ratio:.2f} 倍，请核对 ADC_VREF、"
                  f"数据右对齐设置与通道选择")

    passed = max_dac_err <= ADC_IN_DAC_TOL_MV
    print(f"{'✅' if passed else '❌'} ADC_IN {'pass' if passed else 'failed'}："
          f"DAC 线性度容差 ±{ADC_IN_DAC_TOL_MV:.1f} mV，实测最大偏差 {max_dac_err:.3f} mV")
    _ensure_result_session()
    note = f"有效点 {len(valid)}/{len(points)}"
    if adc_rows:
        note += f"，ADC 最大误差 {max_adc_err:.3f} mV（{max_adc_err / lsb_mv:.2f} LSB）"
    if fs_est is not None:
        note += f"，满量程推断 {fs_est:.0f}（配置 {ADC_FULL_SCALE}）"
    result_log.record("ADC_IN_Voltage", max_dac_err, "mV",
                      min_v=None, max_v=ADC_IN_DAC_TOL_MV,
                      result="PASS" if passed else "FAIL", note=note)

    # 逐点数据自动存档（不再询问，保证每一次测量值都留档）
    name = time.strftime("ADC_IN_测量数据_%Y%m%d_%H%M%S.xlsx")
    adc_max_lsb = max_adc_err / lsb_mv if adc_rows else None
    summary = {
        "time": time.strftime("%Y-%m-%d %H:%M:%S"),
        "range": f"{points[0]} ~ {points[-1]} mV，步进 {step} mV，共 {len(points)} 点",
        "valid": f"{len(valid)}/{len(points)}（ADC 有效 {len(adc_rows)}）",
        "dac_err": f"{max_dac_err:.3f} mV",
        "dac_tol": f"±{ADC_IN_DAC_TOL_MV:.1f} mV",
        "adc_err": (f"{max_adc_err:.3f} mV（{adc_max_lsb:.2f} LSB）"
                    if adc_rows else "无有效 ADC 数据"),
        "full_scale": (f"配置 {ADC_FULL_SCALE}，实测推断 {fs_est:.0f}（比值 {fs_est / ADC_FULL_SCALE:.2f}）"
                       if fs_est is not None else f"配置 {ADC_FULL_SCALE}（数据不足，未推断）"),
        "vref": f"{ADC_IN_VREF_MV:.0f} mV（1 LSB = {lsb_mv:.4f} mV）",
        "verdict": "PASS" if passed else "FAIL",
    }
    saved = _export_adc_table(rows, name, summary)
    if saved:
        print(f"   ✅ 测量数据已保存（工作表：{ADC_SHEET_DATA} / {ADC_SHEET_RESULT}）：{saved}")
    else:
        print("   ⚠️ 测量数据保存失败（txt 记录不受影响）")

    print("✅ ADC_IN 测试完成")
    return max_dac_err


# ===== UVLO 电压（规范 2.19，菜单 [20]）=====
# 测试方法：进测试模式 → 写 [20:9] 选 UVLO 通道 → **万用表接 UO**、电源 CH1 给芯片供电：
#   ① 起始 3.3 V（UO 应为高电平）→ 慢慢降压，UO 首次变低时的电源电压 = UVLO_L；
#   ② 再从 UVLO_L 慢慢升压，UO 首次重新变高时的电源电压 = UVLO_H。
# 全程**保持电源输出打开，只改电压**；结束时恢复到默认 3.3 V。
# 规范 Typ：UVLO_H = 2.15 V、UVLO_L = 1.95 V（校准表未给 Min./Max.，只记录不判定）。

UVLO_SEL = [0x00, 0x00, 0x00, 0x01]      # TEST_LOGIC=0000 / TEST_ANA_EN=0
UVLO_PSU_CHANNEL = 1                     # DP832A CH1 给芯片供电

UVLO_START_V = 3.3                       # 起始电压，也是测试结束后的恢复值
UVLO_MAX_V = 5.0                         # 允许的最高电压（安全上限，绝不越界）
UVLO_MIN_V = 1.0                         # 扫描下限，避免把芯片压到完全掉电
UVLO_STEP_COARSE_V = 0.1                 # 粗扫步进
UVLO_STEP_FINE_V = 0.01                  # 细扫步进（即 UVLO_L / UVLO_H 的分辨力）
UVLO_SETTLE_S = 0.2                      # 每次改电压后的稳定时间
UVLO_LOGIC_HIGH_MV = 1000.0              # UO ≥ 该值判为高电平（UVLO 点时高电平约 2 V）
UVLO_DMM_RANGE = "10V"                   # 万用表电压量程
UVLO_TYP_L_MV = 1950.0                   # 规范 Typ：UVLO_L = 1.95 V
UVLO_TYP_H_MV = 2150.0                   # 规范 Typ：UVLO_H = 2.15 V


def _uvlo_set_voltage(volts):
    """设置 UVLO 供电电压（保持输出开关不变），返回回读到的设定电压或 None。

    电压被强制限制在 0 ~ UVLO_MAX_V（5 V），越界直接抛异常而不是下发指令。
    """
    if not (0.0 <= volts <= UVLO_MAX_V + 1e-9):
        raise ValueError(f"UVLO 电压 {volts:.3f} V 超出 0~{UVLO_MAX_V:.1f} V")
    if not instr.psu_set_voltage(UVLO_PSU_CHANNEL, round(volts, 3)):
        print(f"   ❌ 设置电源 CH{UVLO_PSU_CHANNEL} = {volts:.3f} V 失败")
        return None
    time.sleep(UVLO_SETTLE_S)
    readback = instr.psu_get_voltage(UVLO_PSU_CHANNEL)
    return readback if readback is not None else round(volts, 3)


def _uvlo_is_high(level_mv):
    """UO 电平（mV）是否判为高（level 为 None 时返回 False）。"""
    return level_mv is not None and level_mv >= UVLO_LOGIC_HIGH_MV


def _uvlo_step(volts, tag=""):
    """设电压 → 等稳定 → 万用表读 UO 电压，返回 (实际电压, 电平mV)；设置失败返回 (None, None)。"""
    actual = _uvlo_set_voltage(volts)
    if actual is None:
        return None, None

    level = instr.dmm_measure_single_voltage(voltage_type="DC", range_val=UVLO_DMM_RANGE)
    if level is None:                       # 单次读不到再试一次，排除偶发通信错误
        level = instr.dmm_measure_single_voltage(voltage_type="DC",
                                                 range_val=UVLO_DMM_RANGE)

    if level is None:
        state = "测量失败"
        level_txt = "——"
    else:
        state = "高电平" if _uvlo_is_high(level) else "低电平"
        level_txt = f"{level:8.1f} mV"
    print(f"   {actual:6.3f} V{tag} → UO = {level_txt}（{state}）")
    return actual, level


def UVLO_Voltage():
    """UVLO 电压测试（规范 2.19，菜单 [20]）。

    流程：
      1. 写 [20:9] = 00 00 00 01 选通 UVLO 测试通道；
      2. 确认电源 CH1 输出已打开（未开则打开，之后全程只改电压不断电）；
      3. 设 3.3 V，**万用表读 UO**，必须为高电平（否则接线/配置有问题，直接报错）；
      4. 按 UVLO_STEP_COARSE_V 逐级降压，找到第一次低电平的区间；
      5. 再按 UVLO_STEP_COARSE_V 升压，找到第一次恢复高电平的区间；
         UVLO 有回差（释放电压 > 关断电压），粗扫只能定位区间，用于后续细扫；
      6. 区间内按 UVLO_STEP_FINE_V 细扫升压 → UVLO_H；
         再复位到高电平后细扫降压 → UVLO_L（此时芯片处于“高”状态，
         第一次低电平才是真实下降阈值）；
      7. 无论成败，finally 中恢复电源电压到 UVLO_START_V（3.3 V），输出保持打开。

    返回：
        UVLO_H（mV）—— 主判定值；测不到返回 None。UVLO_L 与 UVLO_H
        同时写进结果记录（测试项名 UVLO_Voltage，备注里区分 L/H）。
    """
    print("\n🚀 执行 UVLO 电压测试 ...")
    print(f"   规范 Typ：UVLO_H {UVLO_TYP_H_MV / 1000:.2f} V、UVLO_L {UVLO_TYP_L_MV / 1000:.2f} V")
    print(f"   起始/恢复电压 {UVLO_START_V:.1f} V，最高 {UVLO_MAX_V:.1f} V，"
          f"粗扫 {UVLO_STEP_COARSE_V * 1000:.0f} mV / 细扫 {UVLO_STEP_FINE_V * 1000:.0f} mV")
    _confirm_connection(
        f"请确定万用表接 UO、电源 CH{UVLO_PSU_CHANNEL} 给芯片供电")

    i2c = _get_i2c()
    execute_write(i2c, addr=20, index=9, write_data=list(UVLO_SEL))
    _log_read(20, 9)
    time.sleep(0.1)

    # ① 电源在线
    if not _psu_ensure("UVLO 需要电源连续调压，无法继续"):
        return None

    # ② 输出保持打开：测试期间只改电压，不断电
    if instr.psu_output_state(UVLO_PSU_CHANNEL) is not True:
        print(f"   电源 CH{UVLO_PSU_CHANNEL} 当前未输出，测试期间需要供电 → 打开输出")
        if not instr.psu_output_on(UVLO_PSU_CHANNEL):
            print(f"   ❌ 无法打开电源 CH{UVLO_PSU_CHANNEL} 输出，跳过本次测试")
            return None

    # ③ 万用表（读 UO）
    if not _dmm_ensure("UVLO 需要万用表判断 UO 高/低电平，无法继续"):
        return None

    uvlo_l = uvlo_h = None
    try:
        # 起始电压下先确认 UO 为高电平
        actual = _uvlo_set_voltage(UVLO_START_V)
        if actual is None:
            print("❌ 设置起始电压失败，跳过本次测试")
            return None
        start_level = instr.dmm_measure_single_voltage(voltage_type="DC",
                                                       range_val=UVLO_DMM_RANGE)
        if start_level is None:
            print("❌ 起始电压下万用表读不到 UO 电压，跳过本次测试")
            return None
        print(f"   {actual:6.3f} V（起始）→ UO = {start_level:8.1f} mV"
              f"（{'高电平' if _uvlo_is_high(start_level) else '低电平'}）")
        if not _uvlo_is_high(start_level):
            print(f"   ❌ 起始电压 {actual:.3f} V 下 UO 不是高电平——"
                  f"请检查 [20:9] = 00 00 00 01 配置、万用表表笔接 UO 与 GND")
            return None

        # ④ 粗扫降压：找到第一次低电平
        v = actual
        last_high = v
        low_at = None
        while v - UVLO_STEP_COARSE_V >= UVLO_MIN_V - 1e-9:
            v = round(v - UVLO_STEP_COARSE_V, 4)
            actual, level = _uvlo_step(v)
            if actual is None:
                break
            if level is None:
                continue                    # 电平读不到：跳过该点，继续降压
            if not _uvlo_is_high(level):
                low_at = actual
                break
            last_high = actual

        if low_at is None:
            print(f"   ❌ 降到 {v:.3f} V 仍为高电平，未找到 UVLO_L"
                  f"（已到扫描下限 {UVLO_MIN_V:.1f} V）")
        else:
            print(f"   粗扫定位到 ({last_high:.3f}, {low_at:.3f}) V 区间")

            # ⑤ 粗扫回升：UVLO 有回差（释放电压 > 关断电压），翻低后必须回到释放点以上
            #    才会重新变高，所以先升压找到第一次恢复高电平的区间。
            print("   粗扫升压，寻找恢复高电平的区间 ...")
            v = low_at
            last_low = v
            high_at = None
            while v + UVLO_STEP_COARSE_V <= UVLO_MAX_V + 1e-9:
                v = round(v + UVLO_STEP_COARSE_V, 4)
                actual_v, level = _uvlo_step(v, "（回升粗扫↑）")
                if actual_v is None:
                    break
                if level is None:
                    continue
                if _uvlo_is_high(level):
                    high_at = actual_v
                    break
                last_low = actual_v

            if high_at is None:
                print(f"   ❌ 升到 {v:.3f} V 仍未恢复高电平，未找到 UVLO_H")
            else:
                print(f"   粗扫升压定位到 ({last_low:.3f}, {high_at:.3f}) V 区间")

                # ⑥ 细扫升压定位 UVLO_H：必须先回到“低”状态再往上走。
                #    UVLO 有回差，粗扫升压后芯片已经是高电平，直接细扫会一直读到高，
                #    故先降到已知的低电平点 low_at，再按细步进升压。
                reset_v, level = _uvlo_step(low_at, "（复位到低电平）")
                if reset_v is None or _uvlo_is_high(level):
                    reset_v = None
                    for cand in (round(low_at - UVLO_STEP_COARSE_V, 4), UVLO_MIN_V):
                        if cand < UVLO_MIN_V - 1e-9:
                            continue
                        actual_v, level = _uvlo_step(cand, "（复位到低电平）")
                        if actual_v is not None and level is not None \
                                and not _uvlo_is_high(level):
                            reset_v = actual_v
                            break

                if reset_v is None:
                    print("   无法复位到低电平，UVLO_H 沿用粗扫结果")
                    uvlo_h = high_at
                else:
                    print(f"   细扫升压定位 UVLO_H（{reset_v:.3f} → {high_at:.3f} V）...")
                    fv = reset_v
                    while fv + UVLO_STEP_FINE_V <= high_at + 1e-9:
                        fv = round(fv + UVLO_STEP_FINE_V, 4)
                        actual_v, level = _uvlo_step(fv, "（细扫↑）")
                        if actual_v is None:
                            break
                        if level is None:
                            continue
                        if _uvlo_is_high(level):
                            uvlo_h = actual_v
                            break
                    if uvlo_h is None:
                        print("   细扫未取得有效结果，沿用粗扫高电平点")
                        uvlo_h = high_at

                # ⑦ 细扫降压定位 UVLO_L：先复位到“高”状态，再往下走
                reset_v = None
                for cand in (high_at, UVLO_START_V):
                    actual_v, level = _uvlo_step(cand, "（复位到高电平）")
                    if actual_v is not None and _uvlo_is_high(level):
                        reset_v = actual_v
                        break
                if reset_v is None:
                    print("   无法复位到高电平，UVLO_L 沿用粗扫结果")
                    uvlo_l = low_at
                else:
                    print(f"   细扫降压定位 UVLO_L（{reset_v:.3f} → {low_at:.3f} V）...")
                    fv = reset_v
                    while fv - UVLO_STEP_FINE_V >= low_at - 1e-9:
                        fv = round(fv - UVLO_STEP_FINE_V, 4)
                        actual_v, level = _uvlo_step(fv, "（细扫↓）")
                        if actual_v is None:
                            break
                        if level is None:
                            continue
                        if not _uvlo_is_high(level):
                            uvlo_l = actual_v
                            break
                    if uvlo_l is None:
                        print("   细扫未取得有效结果，沿用粗扫低电平点")
                        uvlo_l = low_at

    finally:
        # ⑦ 恢复默认电压，输出保持打开（测试要求全程不断电）
        restored = _uvlo_set_voltage(UVLO_START_V)
        if restored is not None:
            print(f"   已恢复电源电压 {restored:.3f} V（输出保持打开）")
        else:
            print(f"   ⚠️  恢复电源电压 {UVLO_START_V:.1f} V 失败，请手动确认")

    # ⑧ 汇总 + 记录
    print("\n   UVLO 结果：")
    for label, value_v, typ_mv in (("UVLO_L", uvlo_l, UVLO_TYP_L_MV),
                                   ("UVLO_H", uvlo_h, UVLO_TYP_H_MV)):
        if value_v is None:
            print(f"      {label} = 未测到（Typ {typ_mv / 1000:.2f} V）")
        else:
            print(f"      {label} = {value_v:.3f} V（Typ {typ_mv / 1000:.2f} V，"
                  f"偏差 {(value_v * 1000 - typ_mv):+.0f} mV）")
    if uvlo_l is not None and uvlo_h is not None:
        print(f"      回差 UVLO_H − UVLO_L = {(uvlo_h - uvlo_l) * 1000:+.0f} mV")

    _ensure_result_session()
    for label, value_v, typ_mv, note in (
        ("UVLO_L", uvlo_l, UVLO_TYP_L_MV,
         f"UVLO_L：{UVLO_START_V:.1f} V 降压至该电压 UO 首次变低"),
        ("UVLO_H", uvlo_h, UVLO_TYP_H_MV,
         f"UVLO_H：自 UVLO_L 升压至该电压 UO 首次恢复高电平"),
    ):
        if value_v is None:
            result_log.record("UVLO_Voltage", None, "mV", typ_v=typ_mv,
                              result="FAIL", note=f"{note}；未测到")
        else:
            result_log.record("UVLO_Voltage", value_v * 1000.0, "mV", typ_v=typ_mv,
                              result="NO-STD", note=note)

    if uvlo_h is None or uvlo_l is None:
        print("❌ UVLO 测试未取得完整的 UVLO_L / UVLO_H")
        return None

    print(f"✅ UVLO 测试完成：UVLO_L = {uvlo_l:.3f} V，UVLO_H = {uvlo_h:.3f} V")
    return uvlo_h * 1000.0

# ===== FR / PWM 阈值电压（规范 2.25 / 2.29，菜单 26 / 30）=====
# 两个测试项流程相同，只是「被扫描的输入脚」和寄存器配置不同，
# 观察 UO 都用**万用表**测直流电平（阈值判定是电平，不是波形）：
#   FR ：写 [20:9] = 00 00 00 11（TEST_LOGIC=1000）→ 电源 CH2 扫描 FR 脚；
#   PWM：先写 [20:9] = 00 00 08 00（PWM_TH_EN=1），再把 [21:0] 的 exit_tm 写 1
#        **退出测试模式** → 电源 CH2 扫描 PWM 脚。
#        退出测试模式后 UO 若是开关波形，直流档读到的是平均电压，同样可用于判高低。
# 扫描方法：
#   FR  —— 升压：从低电平起点往上扫，步进按施加电压分档
#          （<1000 mV → 10 mV、1000~2000 mV → 5 mV、≥2000 mV → 1 mV）；
#          连续 5 次读到高电平（≥3000 mV）时，记下这段连续高电平的起始电压为 FR_H；
#          降压：自**降压起点**（可单独设置，默认 1200 mV，不跟随 FR_H）用同一套
#          分档步进往下；连续 5 次读到低电平（<100 mV）时，
#          记下这段连续低电平的起始电压为 FR_L。
#   PWM —— 0 V 起粗扫到 2 V，命中区间内再做 1 mV 细扫（窗口宽，用 UVLO 式粗+细）。
#   *_H − *_L 即该输入的迟滞。

THRESH_START_V = 0.0            # 扫描起点
THRESH_MAX_V = 2.0              # 扫描上限（Typ 1.15 V，留足余量；绝不越界）
THRESH_STEP_DEFAULT_MV = 10     # 默认粗扫步进 10 mV（可运行时调整）
THRESH_STEP_MAX_MV = 100        # 粗扫步进上限（避免一步跨过整个回差带）
THRESH_FINE_STEP_MV = 1         # 细扫步进固定 1 mV（阈值分辨力）
THRESH_SETTLE_S = 0.25          # 每次改电压后的稳定时间
THRESH_LOGIC_HIGH_MV = 2000.0   # UO ≥ 该值判为高电平（高电平约 3.3 V）
THRESH_CONFIRM_SAMPLES = 2      # 连续读到几次目标电平才确认翻转
THRESH_MAX_POINTS = 4000        # 单方向扫描点数上限（防止步进过小死循环）

# FR：TEST_LOGIC=1000 → 0x11
FR_SEL = [0x00, 0x00, 0x00, 0x11]
FR_PSU_CHANNEL = 2              # DP832A CH2 给 FR 脚扫描电压
FR_DMM_RANGE = "10V"
FR_TYP_H_MV = 1150.0            # 规范 Typ：FR_H = 1.15 V
FR_TYP_L_MV = 950.0             # 规范 Typ：FR_L = 0.95 V
# FR 默认扫描范围（运行时都可改）：升压 900 → 1200 mV 找 FR_H，
# 再自**降压起点** 1200 mV 降压到 900 mV 找 FR_L。步进不询问，按施加电压自动分档。
FR_UP_START_MV = 900
FR_UP_STOP_MV = 1200
FR_DOWN_START_MV = 1200         # 降压段起点（降压从这里往下扫，不再固定从 FR_H 开始）
FR_DOWN_STOP_MV = 900

# FR 电平判据与扫描步进（规范 2.25 的测量方法）
FR_LOGIC_HIGH_MV = 3000.0       # 高电平：UO ≥ 3000 mV
FR_LOGIC_LOW_MV = 100.0         # 低电平：UO < 100 mV
FR_CONFIRM_SAMPLES = 5          # 连续 5 次读到同一种电平才确认翻转
FR_CONFIRM_MAX_READS = 20       # 同一电压点最多读 20 次（期间电平抖一下就重新计数）
# 步进按**施加电压**分档，升压 / 降压两个方向完全相同：
#   < 1000 mV → 10 mV；1000 ~ 2000 mV → 5 mV；≥ 2000 mV → 1 mV
FR_COARSE_STEP_MV = 10          # 电压 < FR_STEP_MID_ABOVE_MV
FR_MID_STEP_MV = 5              # FR_STEP_MID_ABOVE_MV ~ FR_STEP_FINE_ABOVE_MV
FR_FINE_STEP_MV = 1             # 电压 ≥ FR_STEP_FINE_ABOVE_MV
FR_STEP_MID_ABOVE_MV = 1000     # 到 1000 mV 起换 5 mV
FR_STEP_FINE_ABOVE_MV = 2000    # 到 2000 mV 起换 1 mV
FR_STEP_DESC = (f"{FR_COARSE_STEP_MV}/{FR_MID_STEP_MV}/{FR_FINE_STEP_MV} mV"
                f"（{FR_STEP_MID_ABOVE_MV}/{FR_STEP_FINE_ABOVE_MV} mV 分档）")
FR_MAX_POINTS = 4000            # 单方向点数上限（防止异常时死循环）

# 扫描速度（FR 是逐点扫描，节拍直接决定整个测试耗时）
FR_SETTLE_S = 0.05              # 每次改电源电压后的稳定时间（不用 0.25 s 的通用值）
FR_FAST_APERTURE_S = 0.0002     # 万用表快速模式积分时间（200 µs，单点约 3~5 ms）
THRESH_RANGE_MAX_MV = 5000      # 输入电压上限（电源 CH1/CH2 最高 30 V，这里按 5 V 限制）

# PWM：PWM_TH_EN（[20:9] 的 bit[11]）+ exit_tm（[21:0] 的 bit[0]）
PWM_SEL = [0x00, 0x00, 0x08, 0x00]
PWM_EXIT_TM_ADDR = 21
PWM_EXIT_TM_INDEX = 0
PWM_EXIT_TM_DATA = [0x00, 0x00, 0x00, 0x01]
PWM_PSU_CHANNEL = 2             # DP832A CH2 给 PWM 脚扫描电压
PWM_DMM_RANGE = "10V"           # 万用表直流电压档观察 UO 电平
PWM_TYP_H_MV = 1150.0           # 规范 Typ：PWM_H = 1.15 V
PWM_TYP_L_MV = 950.0            # 规范 Typ：PWM_L = 0.95 V


def _bias_set_voltage(channel, volts, settle=THRESH_SETTLE_S):
    """设置电源通道电压 → 等稳定 → 回读实测电压(V)。

    回读失败时退回设定值；设置失败返回 None。电压强制限制在 0~5 V。
    """
    if not (0.0 <= volts <= 5.0 + 1e-9):
        raise ValueError(f"偏置电压 {volts:.3f} V 超出 0~5 V")
    if not instr.psu_set_voltage(channel, round(volts, 3)):
        print(f"   ❌ 设置电源 CH{channel} = {volts:.3f} V 失败")
        return None
    time.sleep(settle)
    actual = instr.psu_measure_voltage(channel)
    return actual if actual is not None else round(volts, 3)


def _fr_set_voltage(channel, volts, settle=FR_SETTLE_S):
    """FR 扫描用的快速设压：设电压 → 短稳定时间，**不做 SCPI 回读**。

    普通 _bias_set_voltage 每点要 sleep 0.25 s 再加一次 psu_measure_voltage
    查询（一次串口往返），几百个点的扫描会被这些开销拖慢十几倍。
    FR 只需要知道「设了多少」，直接返回设定值即可；稳定时间由 FR_SETTLE_S 控制。

    返回设定电压（V）；设置失败返回 None。
    """
    if not (0.0 <= volts <= 5.0 + 1e-9):
        raise ValueError(f"偏置电压 {volts:.3f} V 超出 0~5 V")
    if not instr.psu_set_voltage(channel, round(volts, 4)):
        print(f"   ❌ 设置电源 CH{channel} = {volts:.4f} V 失败")
        return None
    if settle > 0:
        time.sleep(settle)
    return round(volts, 4)


def _fr_read_level(fast=False):
    """读观察脚电平（mV），失败返回 None。

    fast=True 时走万用表快速通道（dmm_read_fast_voltage，单点 3~5 ms）；
    读失败或未启用快速模式时退回普通单次测量（约 100 ms）。
    """
    if fast:
        val = instr.dmm_read_fast_voltage()
        if val is not None:
            return val
    return instr.dmm_measure_single_voltage(voltage_type="DC", range_val=FR_DMM_RANGE)


def _bias_restore(channel, turned_on):
    """扫描结束后的安全恢复：电压回 0 V；本测试打开的通道才关闭输出。"""
    try:
        instr.psu_set_voltage(channel, 0.0)
        print(f"   已把电源 CH{channel} 恢复到 0 V")
    except Exception as e:
        print(f"   ⚠️ 恢复电源 CH{channel} 电压失败：{e}")
    if turned_on:
        try:
            instr.psu_output_off(channel)
            print(f"   已关闭电源 CH{channel} 输出（本测试打开的）")
        except Exception as e:
            print(f"   ⚠️ 关闭电源 CH{channel} 输出失败：{e}")


def _restore_psu_limit(channel, prev_limit, changed):
    """把电源通道限流恢复到测试前的值（只有确实改过且原值已知时才动）。"""
    if not changed or prev_limit is None:
        return
    try:
        if instr.psu_set_current(channel, prev_limit):
            print(f"   已恢复电源 CH{channel} 限流 {prev_limit:.3f} A")
        else:
            print(f"   ⚠️ 恢复电源 CH{channel} 限流失败，请手动改回 {prev_limit:.3f} A")
    except Exception as e:
        print(f"   ⚠️ 恢复电源 CH{channel} 限流异常：{e}")


def _threshold_is_high(mv):
    """观察脚读数（mV）是否判为高电平（None 返回 False）。"""
    return mv is not None and mv >= THRESH_LOGIC_HIGH_MV


def _threshold_point(set_voltage, read_mv, volts, watch, tag=""):
    """一个扫描点：设电压 → 读观察脚（失败重试一次）。返回 (实际电压, 电平mV)。"""
    actual = set_voltage(volts)
    if actual is None:
        return None, None
    mv = read_mv()
    if mv is None:
        mv = read_mv()
    state = "测量失败" if mv is None else ("高电平" if _threshold_is_high(mv) else "低电平")
    mv_txt = "——" if mv is None else f"{mv:8.1f} mV"
    print(f"   {actual:6.3f} V{tag} → {watch} = {mv_txt}（{state}）")
    return actual, mv


def _threshold_confirmed(read_mv, want_high):
    """连续 THRESH_CONFIRM_SAMPLES 次读到目标电平才认为真的翻转（抑制单点噪声）。"""
    for _ in range(THRESH_CONFIRM_SAMPLES - 1):
        mv = read_mv()
        if mv is None:
            mv = read_mv()
        if mv is None:
            return False
        if _threshold_is_high(mv) != want_high:
            return False
    return True


def _threshold_coarse(set_voltage, read_mv, start_v, stop_v, step_v,
                      want_high, watch, tag=""):
    """粗扫：自 start_v 按 step_v（正=升压 / 负=降压）走到 stop_v，找首个目标电平点。

    返回 (hit_v, last_opposite_v)：
        hit_v           - 首个确认目标电平的电压；未找到返回 None
        last_opposite_v - 命中前最后一个「反状态」点（细扫从这里复位）；
                          起点本身处于反状态，故初值为 start_v
    """
    v = start_v
    last_opposite = start_v
    points = 0
    while points < THRESH_MAX_POINTS:
        nxt = round(v + step_v, 4)
        if (stop_v - nxt) * step_v < 0:          # 越过扫描终点
            break
        v = nxt
        points += 1

        actual, level = _threshold_point(set_voltage, read_mv, v, watch, tag)
        if actual is None:
            print("   ❌ 设置电压失败，停止粗扫")
            break
        if level is None:
            continue
        if _threshold_is_high(level) == want_high:
            if _threshold_confirmed(read_mv, want_high):
                return actual, last_opposite
            print(f"   {actual:.3f} V 复测未确认，继续扫描")
            continue
        last_opposite = actual
    return None, last_opposite


def _threshold_reset(set_voltage, read_mv, start_v, direction, step_v, limit_v,
                     want_high, watch):
    """把芯片复位到确定的目标电平状态，返回复位成功的电压；失败返回 None。

    粗扫命中后芯片停在「翻转后」的状态，回差带内把电压设回原读数点未必能复位，
    所以从 start_v 起沿 direction（+1 升压 / -1 降压）按粗步进逐点走出去，
    直到读到 want_high 状态为止（UVLO / COMP 的细扫也是同样的处理）。
    走到 limit_v（0 V 或扫描上限）仍不复位返回 None。
    """
    state = "高电平" if want_high else "低电平"
    v = start_v
    points = 0
    while points < THRESH_MAX_POINTS:
        actual, level = _threshold_point(set_voltage, read_mv, v, watch, "（复位）")
        points += 1
        if actual is not None and level is not None \
                and _threshold_is_high(level) == want_high:
            return actual

        nxt = round(v + direction * step_v, 4)
        if (limit_v - nxt) * direction < 0:      # 已到扫描边界
            break
        v = nxt
    print(f"   ⚠️ 走到 {v:.3f} V 仍无法复位到{state}")
    return None


def _threshold_fine(set_voltage, read_mv, reset_v, hit_v, from_high, want_high,
                    watch, tag="", coarse_step_v=None):
    """细扫：先用粗步进快进到翻转区附近，再以 1 mV 逼近。

    from_high 是复位点当前的电平状态（升压测 *_H 时为低，降压测 *_L 时为高）。
    从复位点沿方向按粗步进快进，只要仍保持 from_high 就继续；碰上终点或
    提前翻转时退回最后一个同状态点，改用 1 mV 走到 want_high 的第一个点。
    这样远离阈值的那一段（如迟滞带 200 mV）不会按 1 mV 逐点白等。

    返回首个目标电平的电压；未找到返回 None。
    """
    fine_v = THRESH_FINE_STEP_MV / 1000.0
    coarse_v = coarse_step_v if coarse_step_v else fine_v
    direction = 1 if hit_v >= reset_v else -1

    if abs(hit_v - reset_v) < fine_v - 1e-9:
        return None

    # ① 粗步快进：保持复位点的电平，停在翻转点之前
    #    注意不能走到/越过 hit_v：一旦越过真实阈值，回差会让芯片停在翻转后的状态，
    #    再退回去也读不回原电平（这正是细扫必须从「确定同状态」的点单调逼近的原因）。
    v = reset_v
    points = 0
    while points < THRESH_MAX_POINTS:
        nxt = round(v + direction * coarse_v, 4)
        if (nxt - hit_v) * direction >= -1e-9:    # 到/过终点，交给 1 mV 细扫
            break
        actual, level = _threshold_point(set_voltage, read_mv, nxt, watch, "（预扫）")
        points += 1
        if actual is None:
            print("   ❌ 设置电压失败，停止细扫")
            return None
        if level is None:
            print("   ⚠️ 预扫读数失败，转入 1 mV 细扫")
            break
        if _threshold_is_high(level) != from_high:
            hit_v = actual                        # 提前翻转：真实阈值在 v ~ nxt 之间
            break
        v = actual                                # 仍保持原状态，继续快进

    # ② 1 mV 细扫到翻转点
    while (v - hit_v) * direction <= 1e-9 and points < THRESH_MAX_POINTS:
        v = round(v + direction * fine_v, 4)
        points += 1

        actual, level = _threshold_point(set_voltage, read_mv, v, watch, tag)
        if actual is None:
            print("   ❌ 设置电压失败，停止细扫")
            return None
        if level is None:
            continue
        if _threshold_is_high(level) == want_high:
            if _threshold_confirmed(read_mv, want_high):
                return actual
            print(f"   {actual:.3f} V 复测未确认，继续细扫")
    return None


def _fr_point(set_voltage, read_mv, volts, watch, tag=""):
    """FR 扫描的一个点：设电压 → 读观察脚（失败重试一次）。返回 (实际电压, 电平mV)。"""
    actual = set_voltage(volts)
    if actual is None:
        return None, None
    mv = read_mv()
    if mv is None:
        mv = read_mv()
    if mv is None:
        state = "测量失败"
    elif mv >= FR_LOGIC_HIGH_MV:
        state = "高电平"
    elif mv < FR_LOGIC_LOW_MV:
        state = "低电平"
    else:
        state = "中间电平"
    mv_txt = "——" if mv is None else f"{mv:8.1f} mV"
    print(f"   {actual:6.3f} V{tag} → {watch} = {mv_txt}（{state}）")
    return actual, mv


def _fr_confirm(read_mv, want_high):
    """在当前电压点继续读数，直到**连续** FR_CONFIRM_SAMPLES 次读到目标电平。

    同一电压点最多读 FR_CONFIRM_MAX_READS 次；中途读到反状态就重新计数，
    这样偶发的一次噪声不会把整个测量点作废。
    返回 True=已确认；False=读满次数仍未连续确认（继续往后扫描）。
    """
    run = 0
    reads = []
    for _ in range(FR_CONFIRM_MAX_READS):
        mv = read_mv()
        if mv is None:
            mv = read_mv()
        if mv is None:
            print("   ⚠️ 复测读数失败，按未确认处理，继续扫描")
            return False
        reads.append(mv)
        ok = mv >= FR_LOGIC_HIGH_MV if want_high else mv < FR_LOGIC_LOW_MV
        run = run + 1 if ok else 0
        if run >= FR_CONFIRM_SAMPLES:
            print(f"   连续 {FR_CONFIRM_SAMPLES} 次确认（最近读数 "
                  + "、".join(f"{x:.1f}" for x in reads[-FR_CONFIRM_SAMPLES:]) + " mV）")
            return True
    print(f"   读 {len(reads)} 次仍未出现连续 {FR_CONFIRM_SAMPLES} 次同电平，继续扫描")
    return False


def _fr_step_mv(volts_mv):
    """按施加电压分档取步进（升压 / 降压相同）。

    < 1000 mV → 10 mV；1000 ~ 2000 mV → 5 mV；≥ 2000 mV → 1 mV。
    """
    if volts_mv >= FR_STEP_FINE_ABOVE_MV:
        return FR_FINE_STEP_MV
    if volts_mv >= FR_STEP_MID_ABOVE_MV:
        return FR_MID_STEP_MV
    return FR_COARSE_STEP_MV


def _fr_scan(set_voltage, read_mv, watch, up_start_mv, up_limit_mv,
             down_start_mv, down_limit_mv):
    """FR 阈值扫描：升压找 FR_H，再自「降压起点」降压找 FR_L。

    升压：自 up_start_mv 按电压分档步进（<1000 mV→10 mV，1000~2000→5 mV，
          ≥2000→1 mV）；连续 FR_CONFIRM_SAMPLES 次读到 ≥ FR_LOGIC_HIGH_MV，
          取这段连续高电平的**第一次高电平**所在电压为 FR_H。
    降压：自 down_start_mv（**不是** FR_H，由操作员指定）用同一套分档步进往下；
          连续 FR_CONFIRM_SAMPLES 次读到 < FR_LOGIC_LOW_MV，取这段连续低电平的
          **第一次低电平**所在电压为 FR_L。
          降压起点必须先确认处于高电平，否则芯片已翻低、测不到 FR_L，
          此时直接放弃 FR_L 并提示把起点改到 FR_H 以上。

    返回 (FR_H 伏特, FR_L 伏特)；未测到的一段为 None。
    """
    # ① 起点必须是低电平，否则无法测「低 → 高」的翻转点
    actual, mv = _fr_point(set_voltage, read_mv, up_start_mv / 1000.0, watch, "（起点）")
    if actual is None:
        print("   ❌ 设置起始电压失败，跳过本次测试")
        return None, None
    if mv is None:
        print("   ❌ 起始电压下读不到观察脚电平，跳过本次测试")
        return None, None
    if mv >= FR_LOGIC_LOW_MV:
        print(f"   ❌ 起始 {actual:.3f} V 时 {watch} = {mv:.1f} mV 不是低电平"
              f"（需 < {FR_LOGIC_LOW_MV:.0f} mV），请把升压起点改到阈值以下")
        return None, None

    # ② 升压找 FR_H
    print(f"\n   升压找 FR_H：{up_start_mv} → {up_limit_mv} mV，"
          f"步进 {FR_STEP_DESC}，连续 {FR_CONFIRM_SAMPLES} 次高电平确认")
    v_mv = up_start_mv
    fr_h_v = None
    for _ in range(FR_MAX_POINTS):
        nxt = v_mv + _fr_step_mv(v_mv)
        if nxt > up_limit_mv:
            break
        v_mv = nxt
        actual, mv = _fr_point(set_voltage, read_mv, v_mv / 1000.0, watch, "（升压）")
        if actual is None:
            print("   ❌ 设置电压失败，停止升压扫描")
            break
        if mv is None:
            continue
        if mv >= FR_LOGIC_HIGH_MV and _fr_confirm(read_mv, True):
            fr_h_v = actual
            print(f"   ✅ FR_H = {fr_h_v:.3f} V"
                  f"（首次连续 {FR_CONFIRM_SAMPLES} 次 ≥ {FR_LOGIC_HIGH_MV:.0f} mV）")
            break

    if fr_h_v is None:
        print(f"   ❌ 升到 {up_limit_mv} mV 仍没有连续 {FR_CONFIRM_SAMPLES} 次高电平，"
              f"FR_H 未测到")
        return None, None

    # ③ 自「降压起点」降压找 FR_L
    print(f"\n   降压找 FR_L：{down_start_mv} → {down_limit_mv} mV，"
          f"步进 {FR_STEP_DESC}，连续 {FR_CONFIRM_SAMPLES} 次低电平确认")
    # 先把电压设到降压起点并确认当前电平（迟滞器件停在升压后的状态）
    _actual, start_lv = _fr_point(set_voltage, read_mv, down_start_mv / 1000.0,
                                  watch, "（降压起点）")
    if _actual is None:
        print("   ❌ 设置降压起点电压失败，FR_L 未测到")
        return fr_h_v, None
    if start_lv is None:
        print("   ⚠️ 降压起点读不到电平，继续往下扫")
    elif start_lv < FR_LOGIC_HIGH_MV:
        # 起点已不在高电平：迟滞已翻转，再往下扫得到的不是 FR_L，直接放弃这一段
        print(f"   ❌ 降压起点 {down_start_mv} mV 时 {watch} = {start_lv:.1f} mV "
              f"不是高电平（FR_H = {fr_h_v * 1000:.0f} mV）——起点低于 FR_H 时芯片"
              f"已经翻低，测不到 FR_L；请把降压起点改到 FR_H 以上")
        return fr_h_v, None

    v_mv = down_start_mv
    fr_l_v = None
    for _ in range(FR_MAX_POINTS):
        nxt = v_mv - _fr_step_mv(v_mv)
        if nxt < down_limit_mv:
            break
        v_mv = nxt
        actual, mv = _fr_point(set_voltage, read_mv, v_mv / 1000.0, watch, "（降压）")
        if actual is None:
            print("   ❌ 设置电压失败，停止降压扫描")
            break
        if mv is None:
            continue
        if mv < FR_LOGIC_LOW_MV and _fr_confirm(read_mv, False):
            fr_l_v = actual
            print(f"   ✅ FR_L = {fr_l_v:.3f} V"
                  f"（首次连续 {FR_CONFIRM_SAMPLES} 次 < {FR_LOGIC_LOW_MV:.0f} mV）")
            break

    if fr_l_v is None:
        print(f"   ❌ 降到 {down_limit_mv} mV 仍没有连续 {FR_CONFIRM_SAMPLES} 次低电平，"
              f"FR_L 未测到")
    return fr_h_v, fr_l_v


def _ask_fr_range(name, up_start, up_limit, down_start, down_limit):
    """交互式设置 FR 扫描范围：升压起点 / 升压上限 / 降压起点 / 降压终止（mV）。

    步进不再询问（按施加电压自动分档，见 _fr_step_mv）。
    参数不合法时整组回退到默认值。
    返回 (up_start_mv, up_limit_mv, down_start_mv, down_limit_mv)
    """
    print(f"\n   {name} 扫描范围（回车沿用默认值；步进自动 {FR_STEP_DESC}）：")
    a = _ask_int("升压起点电压(mV)", up_start, 0, THRESH_RANGE_MAX_MV)
    b = _ask_int("升压上限电压(mV)", up_limit, 1, THRESH_RANGE_MAX_MV)
    d = _ask_int("降压起点电压(mV)", down_start, 1, THRESH_RANGE_MAX_MV)
    c = _ask_int("降压终止电压(mV)", down_limit, 0, THRESH_RANGE_MAX_MV)

    if a >= b or c >= d:
        print(f"   ⚠️ 范围不合法（需 升压起点 < 升压上限，降压终止 < 降压起点），"
              f"改用默认值 升压 {up_start} → {up_limit}、"
              f"降压 {down_start} → {down_limit} mV")
        return up_start, up_limit, down_start, down_limit

    print(f"   扫描：升压 {a} → {b} mV 找 FR_H；自降压起点 {d} mV 降到 {c} mV "
          f"找 FR_L；步进 {FR_STEP_DESC}，连续 {FR_CONFIRM_SAMPLES} 次同电平确认")
    return a, b, d, c


def _ask_threshold_range(name, up_start, up_stop, step, down_stop):
    """交互式设置扫描范围：升压起点 / 升压终点 / 步进 / 降压终点（单位 mV）。

    回车沿用默认值；参数不合法（起点 ≥ 终点、降压终点 ≥ 升压终点）时
    打印提示并整组回退到默认值，避免下发一份自相矛盾的扫描计划。

    返回 (up_start_mv, up_stop_mv, step_mv, down_stop_mv)
    """
    print(f"\n   {name} 扫描范围（回车沿用默认值）：")
    a = _ask_int("升压起点电压(mV)", up_start, 0, THRESH_RANGE_MAX_MV)
    b = _ask_int("升压终止电压(mV)", up_stop, 1, THRESH_RANGE_MAX_MV)
    s = _ask_int("扫描步进(mV)", step, 1, THRESH_STEP_MAX_MV)
    c = _ask_int("降压终止电压(mV)", down_stop, 0, THRESH_RANGE_MAX_MV)

    if a >= b or c >= b:
        print(f"   ⚠️ 范围不合法（需 升压起点 < 升压终点，降压终点 < 升压终点），"
              f"改用默认值 {up_start} → {up_stop} → {down_stop} mV")
        return up_start, up_stop, step, down_stop

    print(f"   扫描：升压 {a} → {b} mV 找高电平；降压 {b} → {c} mV 找低电平；"
          f"步进 {s} mV，UO ≥ {THRESH_LOGIC_HIGH_MV:.0f} mV 判高电平")
    return a, b, s, c


def _threshold_scan(set_voltage, read_mv, watch, label, up_start_mv, up_stop_mv,
                    step_mv, down_stop_mv, fine_scan=False):
    """升压 / 降压扫描求 *_H / *_L（流程参考 UVLO_Voltage）。

    参数（电压都是 mV，运行时由操作员选择）：
        up_start_mv  - 升压段起点（此刻观察脚必须是低电平）
        up_stop_mv   - 升压段终点，也是降压段的起点
        step_mv      - 扫描步进（升 / 降压相同）
        down_stop_mv - 降压段终点
        fine_scan    - True 时在粗扫区间内再按 1 mV 细扫（UVLO 式），
                       False 则直接用 step_mv 的分辨力

    流程：
      ① 起点确认低电平 → ② 升压找到 UO 由低变高的点；
      ③ 需要时复位到确定低电平后 1 mV 细扫 → *_H；
      ④ 自 up_stop_mv（细扫时为 *_H）降压找 UO 由高变低的点；
      ⑤ 需要时复位到确定高电平后 1 mV 细扫 → *_L。

    返回 (high_v, low_v)；未测到的一段为 None。
    """
    up_start_v = up_start_mv / 1000.0
    up_stop_v = up_stop_mv / 1000.0
    down_stop_v = down_stop_mv / 1000.0
    step_v = step_mv / 1000.0

    # ① 起点：必须为低电平，否则无法测“低 → 高”的翻转点
    actual, level = _threshold_point(set_voltage, read_mv, up_start_v, watch, "（起点）")
    if actual is None:
        print("   ❌ 设置起始电压失败，跳过本次测试")
        return None, None
    if level is None:
        print("   ❌ 起始电压下读不到观察脚电平，跳过本次测试")
        return None, None
    if _threshold_is_high(level):
        print(f"   ❌ 起始 {actual:.3f} V 时 {watch} 已是高电平，无法扫描 {label}_H"
              f"（请把升压起点改到阈值以下）")
        return None, None

    # ② 升压（粗扫）
    high_at, last_low = _threshold_coarse(set_voltage, read_mv, up_start_v,
                                          up_stop_v, step_v, True, watch)
    if high_at is None:
        print(f"   ❌ 升到 {up_stop_v:.3f} V 仍未找到 {watch} 高电平，{label}_H 未测到"
              f"（可调高升压终止电压）")
        return None, None

    if not fine_scan:
        high_v = high_at
        print(f"   升压翻转点 {label}_H = {high_v:.3f} V（步进 {step_mv} mV），开始降压 ...")
    else:
        print(f"   粗扫升压定位到 {last_low:.3f} → {high_at:.3f} V 区间")
        # ③ 细扫升压定位 *_H：先复位到确定低电平，避免回差带内误判
        reset_v = _threshold_reset(
            set_voltage, read_mv,
            last_low if last_low is not None else up_start_v,
            -1, step_v, up_start_v, False, watch)
        if reset_v is None:
            print(f"   无法复位到低电平，{label}_H 沿用粗扫结果")
            high_v = high_at
        else:
            print(f"   细扫升压定位 {label}_H（{reset_v:.3f} → {high_at:.3f} V，"
                  f"步进 {THRESH_FINE_STEP_MV} mV）...")
            high_v = _threshold_fine(set_voltage, read_mv, reset_v, high_at, False, True,
                                     watch, "（细扫↑）", step_v)
            if high_v is None:
                print("   细扫未取得有效结果，沿用粗扫高电平点")
                high_v = high_at
        print(f"   升压阈值 {label}_H = {high_v:.3f} V，开始降压 ...")

    # ④ 降压：自升压段终点（细扫时为精确 *_H）往下走，起点必为高电平
    down_start_v = high_v if fine_scan else up_stop_v
    low_at, last_high = _threshold_coarse(set_voltage, read_mv, down_start_v,
                                          down_stop_v, -step_v, False, watch, "（降压）")
    if low_at is None:
        print(f"   ❌ 降到 {down_stop_v:.3f} V 仍未找到 {watch} 低电平，{label}_L 未测到"
              f"（可调低降压终止电压）")
        return high_v, None

    if not fine_scan:
        low_v = low_at
        print(f"   降压翻转点 {label}_L = {low_v:.3f} V（步进 {step_mv} mV）")
        return high_v, low_v

    print(f"   粗扫降压定位到 {last_high:.3f} → {low_at:.3f} V 区间")
    # ⑤ 细扫降压定位 *_L：先复位到确定高电平
    reset_v = _threshold_reset(
        set_voltage, read_mv,
        last_high if last_high is not None else down_start_v,
        1, step_v, up_stop_v, True, watch)
    if reset_v is None:
        print(f"   无法复位到高电平，{label}_L 沿用粗扫结果")
        return high_v, low_at

    print(f"   细扫降压定位 {label}_L（{reset_v:.3f} → {low_at:.3f} V，"
          f"步进 {THRESH_FINE_STEP_MV} mV）...")
    low_v = _threshold_fine(set_voltage, read_mv, reset_v, low_at, True, False,
                            watch, "（细扫↓）", step_v)
    if low_v is None:
        print("   细扫未取得有效结果，沿用粗扫低电平点")
        low_v = low_at
    return high_v, low_v


def _record_threshold_pair(item_name, label, high_v, low_v, typ_h, typ_l, scan_desc):
    """把 *_H / *_L 各记一条结果（校准表未给 Min./Max.，判定为 NO-STD）。"""
    _ensure_result_session()
    for suffix, value_v, typ in (("_H", high_v, typ_h), ("_L", low_v, typ_l)):
        note = f"{label}{suffix}：{scan_desc}"
        if value_v is None:
            result_log.record(item_name, None, "mV", typ_v=typ,
                              result="FAIL", note=f"{note}；未测到")
        else:
            result_log.record(item_name, value_v * 1000.0, "mV", typ_v=typ,
                              result="NO-STD", note=note)


def _print_threshold_result(label, high_v, low_v, typ_h, typ_l):
    """打印 *_H / *_L 与回差。"""
    print(f"\n   {label} 结果：")
    for suffix, value_v, typ in (("_H", high_v, typ_h), ("_L", low_v, typ_l)):
        if value_v is None:
            print(f"      {label}{suffix} = 未测到（Typ {typ / 1000:.2f} V）")
        else:
            print(f"      {label}{suffix} = {value_v:.3f} V（Typ {typ / 1000:.2f} V，"
                  f"偏差 {(value_v * 1000 - typ):+.0f} mV）")
    if high_v is not None and low_v is not None:
        print(f"      回差 {label}_H − {label}_L = {(high_v - low_v) * 1000:+.0f} mV")


def _threshold_report(item_name, label, high_v, low_v, typ_h, typ_l, scan_desc):
    """汇总打印 + 写结果记录，返回是否两项都测到。"""
    _print_threshold_result(label, high_v, low_v, typ_h, typ_l)
    _record_threshold_pair(item_name, label, high_v, low_v, typ_h, typ_l, scan_desc)
    if high_v is None or low_v is None:
        print(f"❌ {label} 阈值测试未取得完整的 {label}_H / {label}_L")
        return False
    print(f"✅ {label} 阈值测试完成：{label}_H = {high_v:.3f} V，{label}_L = {low_v:.3f} V")
    return True


def FR_Voltage():
    """FR 阈值电压测试（规范 2.25，菜单 [26]）。

    流程：
      1. 写 [20:9] = 00 00 00 11（TEST_LOGIC=1000）选通 FR 测试通道；
      2. 万用表接 UO，电源 CH2 接 FR 脚；
      3. 交互设置扫描范围（回车用默认值）：升压起点 900 mV、升压上限 1200 mV、
         降压起点 1200 mV、降压终止 900 mV；步进按施加电压自动分档：
         <1000 mV → 10 mV、1000~2000 mV → 5 mV、≥2000 mV → 1 mV（升降压相同）；
      4. **升压**：自起点按分档步进往上扫描，
         连续 5 次读到高电平（≥3000 mV）时，取这段连续高电平的起始电压为 **FR_H**；
      5. **降压**：自**降压起点**（不是 FR_H，可单独设置）按同一套分档步进往下，
         连续 5 次读到低电平（<100 mV）时，取这段连续低电平的起始电压为 **FR_L**；
      6. 扫描期间万用表切快速模式、每点只等 FR_SETTLE_S，保证速度；
         finally 恢复万用表默认配置并把 CH2 恢复到 0 V。

    返回：
        FR_H（mV）；未测到返回 None（FR_L 同时写进结果记录）。
    """
    print("\n🚀 执行 FR 阈值电压测试 ...")
    print(f"   规范 Typ：FR_H {FR_TYP_H_MV / 1000:.2f} V、FR_L {FR_TYP_L_MV / 1000:.2f} V")
    _confirm_connection(f"请确定万用表接 UO、电源 CH{FR_PSU_CHANNEL} 接 FR 脚")

    i2c = _get_i2c()
    execute_write(i2c, addr=20, index=9, write_data=list(FR_SEL))
    _log_read(20, 9)
    time.sleep(0.1)

    if not _psu_ensure("FR 阈值扫描需要电源 CH2 连续调压，无法继续"):
        return None
    if not _dmm_ensure("FR 阈值需要万用表判断 UO 高/低电平，无法继续"):
        return None

    up_start_mv, up_stop_mv, down_start_mv, down_stop_mv = _ask_fr_range(
        "FR", FR_UP_START_MV, FR_UP_STOP_MV, FR_DOWN_START_MV, FR_DOWN_STOP_MV)

    turned_on = False
    if instr.psu_output_state(FR_PSU_CHANNEL) is not True:
        print(f"   电源 CH{FR_PSU_CHANNEL} 当前未输出，测试期间需要调压 → 打开输出")
        if not instr.psu_output_on(FR_PSU_CHANNEL):
            print("   ❌ 无法打开电源输出，跳过本次测试")
            return None
        turned_on = True
        time.sleep(1.5)

    fr_h = fr_l = None
    fast_dmm = False
    try:
        # 万用表切快速模式（只配置一次，之后逐点 READ? 约 3~5 ms）
        fast_dmm = instr.dmm_prepare_fast_voltage(
            voltage_type="DC", range_val=FR_DMM_RANGE,
            aperture_s=FR_FAST_APERTURE_S)
        if fast_dmm:
            print(f"   万用表快速模式（Aperture = Time "
                  f"{FR_FAST_APERTURE_S * 1e6:.0f} µs，单点约 3~5 ms），"
                  f"电平切换稳定 {FR_SETTLE_S * 1000:.0f} ms/点")
        else:
            print("   ⚠️ 万用表快速模式不可用，退回普通单次测量（约 100 ms/点）")

        fr_h, fr_l = _fr_scan(
            lambda v: _fr_set_voltage(FR_PSU_CHANNEL, v),
            lambda: _fr_read_level(fast_dmm),
            "UO", up_start_mv, up_stop_mv, down_start_mv, down_stop_mv)
    finally:
        if fast_dmm:
            try:
                instr.dmm_finish_fast_voltage()
                print("   万用表已恢复默认配置")
            except Exception as e:
                print(f"   ⚠️ 恢复万用表默认配置异常：{e}")
        _bias_restore(FR_PSU_CHANNEL, turned_on)

    scan_desc = (f"升压 {up_start_mv}→{up_stop_mv} mV 找 FR_H，"
                 f"自降压起点 {down_start_mv} mV 降到 {down_stop_mv} mV 找 FR_L，"
                 f"步进 {FR_STEP_DESC}，连续 {FR_CONFIRM_SAMPLES} 次同电平确认")
    ok = _threshold_report("FR_Voltage", "FR", fr_h, fr_l,
                           FR_TYP_H_MV, FR_TYP_L_MV, scan_desc)
    return fr_h * 1000.0 if ok else None


def PWM_IN_Voltage():
    """PWM 阈值电压测试（规范 2.29，菜单 [30]）。

    流程：
      1. 写 [20:9] = 00 00 08 00（PWM_TH_EN=1）；
      2. 写 [21:0] = 00 00 00 01（exit_tm=1）**退出测试模式**；
      3. **万用表**接 UO（直流电压档），电源 CH2 接 PWM 脚；
      4. 选择粗扫步进（默认 10 mV）；
      5. 从 0 V 按粗扫步进升压定位区间，再按 1 mV 细扫，UO 首次变高 → PWM_H；
      6. 自 PWM_H 粗扫降压定位区间，再按 1 mV 细扫，UO 首次变低 → PWM_L；
      7. finally 把 CH2 恢复到 0 V（本测试打开的通道关闭输出）。

    注意：exit_tm=1 之后芯片已退出测试模式，继续其它测试项前需重新执行
    [1] 进入测试模式使能。

    返回：
        PWM_H（mV）；未测到返回 None（PWM_L 同时写进结果记录）。
    """
    print("\n🚀 执行 PWM 阈值电压测试 ...")
    print(f"   规范 Typ：PWM_H {PWM_TYP_H_MV / 1000:.2f} V、PWM_L {PWM_TYP_L_MV / 1000:.2f} V")
    _confirm_connection(f"请确定万用表接 UO、电源 CH{PWM_PSU_CHANNEL} 接 PWM 脚")

    i2c = _get_i2c()
    execute_write(i2c, addr=20, index=9, write_data=list(PWM_SEL))
    _log_read(20, 9)
    time.sleep(0.1)

    # 退出测试模式：PWM 阈值只在正常模式下体现
    exit_hex = ' '.join(f'{b:02X}' for b in PWM_EXIT_TM_DATA)
    print(f"   退出测试模式：写入 [{PWM_EXIT_TM_ADDR}:{PWM_EXIT_TM_INDEX}] = {exit_hex}（exit_tm=1）")
    execute_write(i2c, addr=PWM_EXIT_TM_ADDR, index=PWM_EXIT_TM_INDEX,
                  write_data=list(PWM_EXIT_TM_DATA))
    _log_read(PWM_EXIT_TM_ADDR, PWM_EXIT_TM_INDEX)
    time.sleep(0.1)

    if not _psu_ensure("PWM 阈值扫描需要电源 CH2 连续调压，无法继续"):
        print("   ⚠️ 芯片已退出测试模式（exit_tm=1），继续其它测试项前请先执行 [1]")
        return None
    if not _dmm_ensure("PWM 阈值需要万用表判断 UO 高/低电平，无法继续"):
        print("   ⚠️ 芯片已退出测试模式（exit_tm=1），继续其它测试项前请先执行 [1]")
        return None

    step_mv = _ask_int("PWM 粗扫步进(mV)", THRESH_STEP_DEFAULT_MV, 1, THRESH_STEP_MAX_MV)
    print(f"   起始 {THRESH_START_V:.1f} V，最高 {THRESH_MAX_V:.1f} V，"
          f"粗扫 {step_mv} mV / 细扫 {THRESH_FINE_STEP_MV} mV，"
          f"万用表 {PWM_DMM_RANGE} 档读 UO")

    turned_on = False
    if instr.psu_output_state(PWM_PSU_CHANNEL) is not True:
        print(f"   电源 CH{PWM_PSU_CHANNEL} 当前未输出，测试期间需要调压 → 打开输出")
        if not instr.psu_output_on(PWM_PSU_CHANNEL):
            print("   ❌ 无法打开电源输出，跳过本次测试")
            return None
        turned_on = True
        time.sleep(1.5)

    pwm_h = pwm_l = None
    try:
        pwm_h, pwm_l = _threshold_scan(
            lambda v: _bias_set_voltage(PWM_PSU_CHANNEL, v),
            lambda: instr.dmm_measure_single_voltage(voltage_type="DC", range_val=PWM_DMM_RANGE),
            "UO", "PWM",
            0, int(THRESH_MAX_V * 1000), step_mv, int(THRESH_START_V * 1000),
            fine_scan=True)
    finally:
        _bias_restore(PWM_PSU_CHANNEL, turned_on)

    scan_desc = (f"升压 0→{THRESH_MAX_V * 1000:.0f} mV 粗扫 {step_mv} mV + "
                 f"{THRESH_FINE_STEP_MV} mV 细扫找高电平；降压找低电平")
    ok = _threshold_report("PWM_IN_Voltage", "PWM", pwm_h, pwm_l,
                           PWM_TYP_H_MV, PWM_TYP_L_MV, scan_desc)
    print("   ⚠️ 芯片已退出测试模式（exit_tm=1），继续其它测试项前请先执行 [1] 进入测试模式使能")
    return pwm_h * 1000.0 if ok else None


# ---------------------- 电流类 ----------------------

# ===== HOCP / LOCP 过流保护（规范 2.14 / 2.15 / 2.17 / 2.18）=====
# 测试方法：进测试模式后写 [20:9] 选通测试通道（TEST_LOGIC / TEST_ANA），
# 再从某一路输出脚抽（HOCP）或灌（LOCP）电流，用万用表观察另一路输出脚电平：
# 从 0 mA 开始逐步加大电流，电平由低变高时对应的电流即保护点。
# 规范注明：实测值为真实保护阈值的 1/3，档位由 RW_OCP_CFG 决定。
# **源表抽/灌电流上限 = OCP_MAX_MA（300 mA）**：默认档（0.6 A → 实测 200 mA）留有余量；
# 若 RW_OCP_CFG 改到 1.2 A 档（理论实测 400 mA），扫到 300 mA 也不会翻转，测试会明确提示。

# 测试通道选择（规范给出的 [20:9] 写入值）
HOCP_UV_SEL = [0x00, 0x00, 0x03, 0x75]   # HOCP：TEST_LOGIC=1010 / TEST_ANA=1101（U、V 相共用）
HOCP_W_SEL = [0x00, 0x00, 0x03, 0xF5]    # HOCP：TEST_LOGIC=1010 / TEST_ANA=1111（W 相）
LOCP_UV_SEL = [0x00, 0x00, 0x03, 0x77]   # LOCP：TEST_LOGIC=1011 / TEST_ANA=1101（U、V 相共用）
LOCP_W_SEL = [0x00, 0x00, 0x03, 0xF7]    # LOCP：TEST_LOGIC=1011 / TEST_ANA=1111（W 相）

# RW_OCP_CFG：[20:0] 的 bit[9:8] —— 过流保护阈值档位
#   0 → 0.6 A（默认）；1 → 1.2 A
# 实测值为真实值的 1/3，故理论实测：0 → 200 mA，1 → 400 mA
OCP_CFG_ADDR = 20
OCP_CFG_INDEX = 0
OCP_CFG_BIT_LO = 8
OCP_CFG_MASK = 0x03 << OCP_CFG_BIT_LO
OCP_CFG_INFO = {
    0: {"real_ma": 600.0, "measured_ma": 200.0, "label": "0.6 A"},
    1: {"real_ma": 1200.0, "measured_ma": 400.0, "label": "1.2 A"},
}
OCP_CFG_DEFAULT_CODE = 0                 # 默认配置（0 → 0.6 A → 实测 200 mA）

# 电流扫描参数
OCP_START_MA = 0.0                       # 起始电流
OCP_MAX_MA = 300.0                       # 源表抽/灌电流上限（规范默认档实测 200 mA，留 100 mA 余量）
OCP_COARSE_STEP_MA = 10.0                # 粗扫步进
OCP_FINE_STEP_MA = 1.0                   # 翻转区间内的细扫步进（即保护点的分辨力）
OCP_STEP_SETTLE_S = 0.05                 # 每改一次电流后的稳定时间
OCP_CONFIG_SETTLE_S = 0.2                # 写完测试通道后的稳定时间
OCP_LOGIC_HIGH_MV = 2000.0               # 输出脚“高电平”判定阈值（可随供电电平调整）
OCP_CONFIRM_SAMPLES = 2                  # 连续读到几次高电平才认为翻转
OCP_SMU_VLIMIT_V = 5.0                   # 源表限压（输出脚高电平 ≈ VM）
OCP_PRINT_EVERY_MA = 20.0                # 粗扫时每隔多少 mA 打印一行（翻转点与细扫必打）


def _ocp_cfg_read(i2c):
    """读 RW_OCP_CFG（[20:0] bit[9:8]），返回 (code, info)。

    读取失败返回 (None, None)；读到未定义编码时 info 为 None、code 原样返回。
    """
    rd = execute_read(i2c, OCP_CFG_ADDR, OCP_CFG_INDEX)
    if not rd or len(rd) < 4:
        print(f"   ⚠️  读取 RW_OCP_CFG [{OCP_CFG_ADDR}:{OCP_CFG_INDEX}] 失败")
        return None, None
    word = (rd[0] << 24) | (rd[1] << 16) | (rd[2] << 8) | rd[3]
    code = (word >> OCP_CFG_BIT_LO) & 0x03
    info = OCP_CFG_INFO.get(code)
    if info is None:
        print(f"   ⚠️  RW_OCP_CFG = {code}（未定义编码），只能记录实测值")
    return code, info


@contextmanager
def _ocp_smu_output():
    """源表电流源输出上下文：0 mA 上电 → 退出时先回零再关输出。

    抽/灌方向由调用方在 _ocp_sweep_scan 里用电流正负号控制，
    这里只负责「0 电流上电」与「确保关断」。
    """
    if not instr.smu_set_current(0.0, voltage_limit=OCP_SMU_VLIMIT_V):
        raise RuntimeError("源表设置为 0 mA 电流源失败")
    if not instr.smu_output_on():
        raise RuntimeError("源表输出打开失败")
    try:
        state = instr.smu_output_state()
        if state is not True:
            raise RuntimeError(f"源表输出未打开（:OUTPut:STATe? = {state}）")
        yield
    finally:
        # 先回零再关输出：保护动作后输出脚可能已被源表拉低，尽早撤掉电流
        try:
            instr.smu_set_current(0.0, voltage_limit=OCP_SMU_VLIMIT_V)
        except Exception:
            pass
        try:
            if not instr.smu_output_off():
                print("   ⚠️  源表关闭输出返回失败，请手动确认面板 OUTPUT 已灭")
        except Exception as e:
            print(f"   ⚠️  关闭源表输出异常：{e}")


def _ocp_sweep_scan(src_pin, meas_pin, sink=True):
    """在 src_pin 上从小到大改变电流，直到 meas_pin 电压由低变高。

    电流由源表电流源输出：抽电流（sink=True）用负电流，灌电流用正电流。
    先按 OCP_COARSE_STEP_MA 粗扫找到翻转区间，再按 OCP_FINE_STEP_MA 细扫，
    取「第一次读到高电平」的电流值作为保护点。

    调用前需已用 _ocp_smu_output() 打开输出（本函数从当前状态开始）。

    返回：
        (ma, info) - ma 为保护点电流(mA)，未翻转返回 None；
                     info = dict(baseline_mv, last_low_ma, first_high_ma, max_mv)
    """
    sign = -1.0 if sink else 1.0
    action = "抽" if sink else "灌"
    info = {"baseline_mv": None, "last_low_ma": None,
            "first_high_ma": None, "max_mv": None}

    def apply_ma(ma):
        # 硬上限保护：任何情况下都不让源表抽/灌电流超过 OCP_MAX_MA
        if abs(ma) > OCP_MAX_MA + 1e-9:
            raise RuntimeError(f"{action}电流 {abs(ma):.1f} mA 超过上限 "
                               f"{OCP_MAX_MA:.0f} mA，已拒绝下发")
        if not instr.smu_set_current(sign * ma / 1000.0,
                                     voltage_limit=OCP_SMU_VLIMIT_V):
            raise RuntimeError(f"源表设置 {action} 电流 {ma:.1f} mA 失败")
        time.sleep(OCP_STEP_SETTLE_S)

    def read_mv():
        return instr.dmm_measure_single_voltage(voltage_type="DC", range_val="10V")

    def is_high(mv):
        return mv is not None and mv >= OCP_LOGIC_HIGH_MV

    def confirmed_high(ma):
        """连续 OCP_CONFIRM_SAMPLES 次读到高电平才算翻转（抑制单次噪声）。"""
        for _ in range(OCP_CONFIRM_SAMPLES - 1):
            mv = read_mv()
            if not is_high(mv):
                print(f"      {ma:.1f} mA 复测 {mv if mv is None else f'{mv:.1f}'} mV，"
                      f"未确认翻转，继续扫描")
                return False
        return True

    # 0 mA 基线：应为低电平（否则测试通道/接线不对，先报出来再继续）
    apply_ma(OCP_START_MA)
    base_mv = read_mv()
    info["baseline_mv"] = base_mv
    if base_mv is None:
        print(f"   ⚠️  {meas_pin} 基线电压测量失败，本次扫描可能不可靠")
    else:
        print(f"   {OCP_START_MA:.1f} mA（{action} {src_pin}）→ {meas_pin} = {base_mv:.1f} mV")
        if is_high(base_mv):
            print(f"   ❌ 0 mA 时 {meas_pin} 已是高电平（{base_mv:.1f} mV ≥ "
                  f"{OCP_LOGIC_HIGH_MV:.0f} mV），无法测保护点——"
                  f"请检查测试通道 [20:9] 配置、{meas_pin} 接线与阈值设置")
            return None, info

    # ① 粗扫：找到第一次翻高的电流（按 OCP_PRINT_EVERY_MA 节流打印）
    ma = OCP_START_MA + OCP_COARSE_STEP_MA
    last_low_ma = OCP_START_MA
    high_ma = None
    while ma <= OCP_MAX_MA + 1e-9:
        apply_ma(ma)
        mv = read_mv()
        if mv is not None:
            info["max_mv"] = mv if info["max_mv"] is None else max(info["max_mv"], mv)
        high = is_high(mv) and confirmed_high(ma)
        if high or ma % OCP_PRINT_EVERY_MA < OCP_COARSE_STEP_MA:
            print(f"   {ma:6.1f} mA（{action} {src_pin}）→ {meas_pin} = "
                  f"{'测量失败' if mv is None else f'{mv:8.1f} mV'}")
        if high:
            high_ma = ma
            break
        last_low_ma = ma
        ma += OCP_COARSE_STEP_MA

    if high_ma is None:
        print(f"   ❌ 扫到 {OCP_MAX_MA:.0f} mA 仍未翻转，未找到保护点")
        info["last_low_ma"] = last_low_ma
        return None, info

    info["last_low_ma"] = last_low_ma

    # ② 细扫：在 (last_low, high) 之间逐点找第一次翻高
    fine = last_low_ma + OCP_FINE_STEP_MA
    while fine < high_ma - 1e-9:
        apply_ma(fine)
        mv = read_mv()
        if mv is not None:
            info["max_mv"] = mv if info["max_mv"] is None else max(info["max_mv"], mv)
        print(f"   {fine:6.1f} mA（细扫）→ {meas_pin} = "
              f"{'测量失败' if mv is None else f'{mv:8.1f} mV'}")
        if is_high(mv) and confirmed_high(fine):
            info["first_high_ma"] = fine
            return fine, info
        fine += OCP_FINE_STEP_MA

    info["first_high_ma"] = high_ma
    return high_ma, info


def _ocp_protection_test(title, sel, phases, item_name, sink=True):
    """HOCP / LOCP 公共流程：选通道 → 逐相扫电流找电平翻转点 → 记录。

    参数：
        title:     str - 打印用标题（如 "HOCP U相和V相"）
        sel:       list - 写入 [20:9] 的 4 字节测试通道配置
        phases:    list[(src_pin, meas_pin)] - 每一相：从 src_pin 抽/灌电流、
                   观察 meas_pin 电平
        item_name: str - 写入结果记录的测试项名（与校准标准表 A 列一致）
        sink:      bool - True 抽电流（HOCP，源表输出负电流），False 灌电流（LOCP）
    返回：
        dict - {src_pin: 保护点 mA 或 None}；源表不可用返回 None
    """
    action = "抽" if sink else "灌"
    print(f"\n🚀 执行 {title} ...")

    # ⓪ 源表在线检查（初始化被跳过/失败时尝试重建连接）
    if not _smu_ensure(f"{title} 必须由源表提供扫描电流，无法继续"):
        return None

    i2c = _get_i2c()

    # ⓪ 复核 [20:0] 的 RW_TIA_IBIAS_SEL = 01（测试全程保持，写 [20:0] 一律读-改-写）
    _tia_ibias_sel_ensure(i2c)

    # ① 读 OCP_CFG，算出理论实测值
    code, cfg = _ocp_cfg_read(i2c)
    if code is None:
        code = OCP_CFG_DEFAULT_CODE
        cfg = OCP_CFG_INFO[code]
        print("   RW_OCP_CFG 读取失败，按默认档位处理："
              f"{code} → {cfg['label']}，理论实测 {cfg['measured_ma']:.0f} mA")
    elif cfg is not None:
        print(f"   RW_OCP_CFG = {code} → 真实阈值 {cfg['label']}，"
              f"理论实测 {cfg['measured_ma']:.0f} mA（实测值为真实值的 1/3）")
    typ_ma = cfg["measured_ma"] if cfg else None

    # 理论实测值超出扫描上限时提前提示：扫到 OCP_MAX_MA 也不会翻转
    print(f"   扫描上限 {OCP_MAX_MA:.0f} mA（源表{action}电流不超过该值），"
          f"粗扫 {OCP_COARSE_STEP_MA:.0f} mA / 细扫 {OCP_FINE_STEP_MA:.0f} mA")
    if typ_ma is not None and typ_ma > OCP_MAX_MA + 1e-9:
        print(f"   ⚠️ 当前 RW_OCP_CFG={code} 的理论实测值 {typ_ma:.0f} mA 已超过扫描上限 "
              f"{OCP_MAX_MA:.0f} mA，本次扫描不会找到保护点；"
              f"如需测该档请把 OCP_MAX_MA 调大或把 RW_OCP_CFG 改回默认档")

    # ② 选通测试通道
    sel_hex = ' '.join(f'{b:02X}' for b in sel)
    print(f"   测试通道：[20:9] = {sel_hex}")
    execute_write(i2c, addr=20, index=9, write_data=list(sel))
    _log_read(20, 9)
    time.sleep(OCP_CONFIG_SETTLE_S)

    # ③ 逐相扫描
    results = {}
    for src_pin, meas_pin in phases:
        print(f"\n   --- {src_pin} {action}电流，观察 {meas_pin} 电平 ---")
        if not _confirm_connection(
                f"请确定源表 LO 接 GND、HI 接 {src_pin}，万用表接 {meas_pin}"):
            results[src_pin] = None
            continue
        try:
            with _ocp_smu_output():
                ma, info = _ocp_sweep_scan(src_pin, meas_pin, sink=sink)
        except Exception as e:
            print(f"   ❌ {src_pin} 扫描异常：{type(e).__name__}: {e}")
            ma, info = None, {}

        results[src_pin] = ma
        if ma is None:
            note = (f"{src_pin} 相，未找到保护点"
                    + (f"，扫描至 {OCP_MAX_MA:.0f} mA 仍未翻转" if not info else ""))
            if cfg:
                note += f"；OCP_CFG={code}（理论实测 {typ_ma:.0f} mA）"
            _ensure_result_session()
            result_log.record(item_name, None, "mA", typ_v=typ_ma,
                              result="FAIL", note=note)
            print(f"   ❌ {src_pin}：未找到保护点")
            continue

        real_ma = ma * 3.0
        note = f"{src_pin} 相"
        if cfg:
            note += (f"，OCP_CFG={code}（{cfg['label']}，理论实测 {typ_ma:.0f} mA，"
                     f"偏差 {ma - typ_ma:+.1f} mA）")
        else:
            note += f"，OCP_CFG={code}（未定义编码）"
        note += f"，真实阈值≈{real_ma:.0f} mA"

        print(f"   ✅ {src_pin}：{meas_pin} 在 {ma:.1f} mA 翻高"
              f"（真实阈值≈{real_ma:.0f} mA"
              + (f"，理论实测 {typ_ma:.0f} mA" if typ_ma is not None else "") + "）")
        _ensure_result_session()
        result_log.record(item_name, ma, "mA", typ_v=typ_ma, result="NO-STD", note=note)

    # ④ 汇总
    print(f"\n   {title} 汇总：")
    for src_pin, _meas in phases:
        ma = results.get(src_pin)
        print(f"      {src_pin} → {'未测到' if ma is None else f'{ma:.1f} mA'}"
              + (f"（真实≈{ma * 3:.0f} mA）" if ma is not None else ""))

    ok = all(results.get(src_pin) is not None for src_pin, _ in phases)
    print(f"{'✅' if ok else '❌'} {title} 完成")
    return results


def HOCP_Current():
    """HOCP U相和V相 过流保护测试（规范 2.14，菜单 [15]）。

    写测试通道 [20:9] = 00 00 03 75，分别从 UO、VO 抽电流，
    万用表观察 WO 电平；WO 由低变高时的电流即该相 HOCP。
    """
    return _ocp_protection_test(
        title="HOCP U相和V相",
        sel=HOCP_UV_SEL,
        phases=[("UO", "WO"), ("VO", "WO")],
        item_name="HOCP_Current",
    )


def LOCP_Current():
    """LOCP U相和V相 过流保护测试（规范 2.15，菜单 [16]）。

    写测试通道 [20:9] = 00 00 03 77，分别从 UO、VO 灌入电流，
    万用表观察 WO 电平；WO 由低变高时的电流即该相 LOCP。
    """
    return _ocp_protection_test(
        title="LOCP U相和V相",
        sel=LOCP_UV_SEL,
        phases=[("UO", "WO"), ("VO", "WO")],
        item_name="LOCP_Current",
        sink=False,
    )


def IBN_5U_Current():
    """IBN_5U 电流测试。"""
    return _current_test("IBN_5U电流", addr=20, index=9, write_data=[0x00, 0x00, 0x03, 0xA0])


def HOCP_W_Current():
    """HOCP W相 过流保护测试（规范 2.17，菜单 [18]）。

    写测试通道 [20:9] = 00 00 03 F5，从 WO 抽电流，万用表观察 UO 电平。
    """
    return _ocp_protection_test(
        title="HOCP W相",
        sel=HOCP_W_SEL,
        phases=[("WO", "UO")],
        item_name="HOCP_W_Current",
    )


def LOCP_W_Current():
    """LOCP W相 过流保护测试（规范 2.18，菜单 [19]）。

    写测试通道 [20:9] = 00 00 03 F7，从 WO 灌入电流，万用表观察 UO 电平。
    """
    return _ocp_protection_test(
        title="LOCP W相",
        sel=LOCP_W_SEL,
        phases=[("WO", "UO")],
        item_name="LOCP_W_Current",
        sink=False,
    )


# ---------------------- 频率 / 温度 / 其他 ----------------------

# ===== OSC 频率（规范 2.20，菜单 [21]）=====
# OSC_CHANGE_EN：寄存器 [20:3] 的 bit[0]。测试开始时交互选择 0 / 1 并写入，
# 直接测量与 TRIM 修调都只在该状态下进行，测试结束恢复成选择前的值。
OSC_CHANGE_ADDR = 20
OSC_CHANGE_INDEX = 3
OSC_CHANGE_BIT_LO = 0
OSC_CHANGE_MASK = 0x01 << OSC_CHANGE_BIT_LO

# 测试模式：0 = 直接测量（在所选 OSC_CHANGE_EN 状态下测一次频率）；1 = TRIM 修调
# 与其它修调项共用 TEST_MODE_* 常量（_ask_test_mode）
OSC_MODE_DIRECT = TEST_MODE_DIRECT
OSC_MODE_TRIM = TEST_MODE_TRIM


def _osc_change_en_read(i2c):
    """读 OSC_CHANGE_EN（[20:3] bit[0]），失败返回 None。"""
    rd = execute_read(i2c, OSC_CHANGE_ADDR, OSC_CHANGE_INDEX)
    if not rd or len(rd) < 4:
        print(f"   ⚠️  读取 [{OSC_CHANGE_ADDR}:{OSC_CHANGE_INDEX}] 失败，无法读取 OSC_CHANGE_EN")
        return None
    word = (rd[0] << 24) | (rd[1] << 16) | (rd[2] << 8) | rd[3]
    return (word >> OSC_CHANGE_BIT_LO) & 0x01


def _osc_change_en_write(i2c, enable):
    """写 OSC_CHANGE_EN（读-改-写，保留 [20:3] 其它字段），写入后回读确认。"""
    ok = _write_word_field(i2c, OSC_CHANGE_ADDR, OSC_CHANGE_INDEX,
                           OSC_CHANGE_BIT_LO, OSC_CHANGE_MASK, 1 if enable else 0)
    if not ok:
        print(f"   ❌ 写 OSC_CHANGE_EN = {1 if enable else 0} 失败（读回异常）")
        return False
    readback = _osc_change_en_read(i2c)
    if readback != (1 if enable else 0):
        print(f"   ⚠️  写入后回读 OSC_CHANGE_EN = {readback}，与期望 {1 if enable else 0} 不一致")
    return True


def _osc_ask_change_en(i2c):
    """交互选择 OSC_CHANGE_EN（[20:3] bit[0]）工作状态：0 / 1。

    打印当前值，回车沿用当前值；选定后写 [20:3] bit0（读-改-写）并回读确认。
    返回 (en, orig_en)：en = 选定值（写入失败返回 None），orig_en = 选择前的值
    （用于测试结束恢复；读取失败时为 None）。
    """
    orig_en = _osc_change_en_read(i2c)
    orig_txt = "读取失败（按 0 处理）" if orig_en is None else str(orig_en)
    default = 0 if orig_en is None else orig_en

    print("\n   OSC_CHANGE_EN（[20:3] bit[0]）工作状态：")
    print("      0 → OSC_CHANGE_EN = 0")
    print("      1 → OSC_CHANGE_EN = 1")
    en = _ask_int(f"OSC_CHANGE_EN 0/1（当前 {orig_txt}）", default, 0, 1)

    if not _osc_change_en_write(i2c, en):
        return None, orig_en
    time.sleep(OSC_TRIM_SETTLE_S)
    print(f"   ✅ 已置 OSC_CHANGE_EN = {en}（[{OSC_CHANGE_ADDR}:{OSC_CHANGE_INDEX}] "
          f"bit[{OSC_CHANGE_BIT_LO}]）")
    return en, orig_en


def _osc_measure_direct(i2c, cur_trim, en, sweep=False):
    """直接测量模式：测一次频率并按标准判定。

    sweep=False：只测当前 OSC_CHANGE_EN = en 这一态（调用方已写好 en）；
    sweep=True ：OSC_CHANGE_EN 依次置 0 → 1 各测一次，测完把 EN 恢复成 en。

    返回 {EN: 频率Hz}（测失败的那一态为 None）。
    """
    states = (0, 1) if sweep else (en,)
    results = {}
    try:
        for cur_en in states:
            print(f"\n   --- OSC_CHANGE_EN = {cur_en} ---")
            if sweep:
                if not _osc_change_en_write(i2c, cur_en):
                    results[cur_en] = None
                    _ensure_result_session()
                    result_log.record("OSC_P", None, "Hz", result="FAIL",
                                      note=f"OSC_CHANGE_EN={cur_en}，寄存器写入失败；"
                                           f"修调 {cur_trim}")
                    continue
                time.sleep(OSC_TRIM_SETTLE_S)

            freq, _period, _duty = _osc_measure_freq("CH1")
            if freq is None:
                print(f"   ❌ OSC_CHANGE_EN={cur_en} 时频率测量失败")
                results[cur_en] = None
                _ensure_result_session()
                result_log.record("OSC_P", None, "Hz", result="FAIL",
                                  note=f"OSC_CHANGE_EN={cur_en}，频率测量失败；"
                                       f"修调 {cur_trim}")
                continue

            results[cur_en] = freq
            print(f"   频率 = {freq:.3f} Hz")
            _judge_osc(freq, extra_note=f"OSC_CHANGE_EN={cur_en}，修调 {cur_trim}")
    finally:
        # 扫完两态后恢复到进入直接测量时的状态（外层再恢复成测试前的值）
        if sweep and en != states[-1]:
            if _osc_change_en_write(i2c, en):
                print(f"\n   已恢复 OSC_CHANGE_EN = {en}")
            else:
                print(f"   ⚠️  恢复 OSC_CHANGE_EN = {en} 失败")

    if sweep:
        print("\n   OSC 直接测量汇总：")
        for cur_en in (0, 1):
            f = results.get(cur_en)
            print(f"      OSC_CHANGE_EN={cur_en} → "
                  + ("测量失败" if f is None else f"{f:.3f} Hz"))
        if results.get(0) is not None and results.get(1) is not None:
            print(f"      两态差值 = {results[1] - results[0]:+.3f} Hz")
    print(f"✅ OSC频率（直接测量）完成，OSC_CHANGE_EN = {en if not sweep else '0/1 两态'}")
    return results


def OSC_P():
    """OSC 频率测试（规范 2.20，菜单 [21]），两种模式二选一。

    进入方式：
      0. 确认示波器连接并只做一次 Auto Scale（之后保持连接，不再重复自动设置）；
      1. 写 [20:9] = 00 00 00 03 选通 OSC 输出，读当前 RW_OSC_24M_TRIM；
      2. 选择 OSC_CHANGE_EN（[20:3] bit[0]）工作状态：0 或 1（回车沿用当前值），
         写 [20:3] bit0 并回读确认；TRIM 修调只在该状态下进行，
         测试结束恢复成选择前的值；
      3. 询问测试模式：

    **模式 0：直接测量（默认）**
      再选测量范围：只测所选那一态，或 OSC_CHANGE_EN 依次置 0、1 各测一次
      （默认两态都测，测完恢复到所选态）。每次测量都按 OSC_LIMITS_HZ
      （11620~11820 Hz）判定，记录备注标注 EN 值。返回 {EN: 频率Hz}。

    **模式 1：TRIM 修调**（与 TIA_OUT 共用 _trim_bisect_search）
      在所选 OSC_CHANGE_EN 状态下测当前频率 → 判定，通过则结束；不通过时先测
      code 0 / 63 两端判断方向 → 二分收敛到跨过目标 11.72 kHz 的两个 code →
      补测邻近点 → 取偏差最小者 → 写回、复测判定，最后把 RW_OSC_24M_TRIM
      同步到 MTP 配置表。返回最终频率。

    RW_OSC_24M_TRIM 位于 [20:4] 的 bit[13:8]，与 VBGTRIM 等共用寄存器，读-改-写。
    """
    print("\n🚀 执行 OSC 频率测试 ...")
    print(f"   判定范围 {OSC_LIMITS_HZ[0]:.0f} ~ {OSC_LIMITS_HZ[1]:.0f} Hz，"
          f"目标 {OSC_TARGET_HZ:.0f} Hz")
    _confirm_connection("请确定示波器连接UO")

    i2c = _get_i2c()
    execute_write(i2c, addr=20, index=9, write_data=[0x00, 0x00, 0x00, 0x03])
    _log_read(20, 9)
    time.sleep(0.1)

    cur = _osc_trim_read(i2c)
    if cur is None:
        print("❌ 读取 OSC_TRIM 失败，跳过本次测试")
        return None
    print(f"   当前修调值 = {cur}")

    # 保持示波器连接：确认会话可用，仅在开始时做一次 Auto Scale
    if not _osc_ensure_connected():
        print("❌ 示波器未连接，跳过本次测试")
        return None
    instr.osc_auto_set()
    time.sleep(OSC_TRIM_SETTLE_S)

    # ---- 选择 OSC_CHANGE_EN 工作状态（两种模式都只在该状态下测量）----
    en, orig_en = _osc_ask_change_en(i2c)
    if en is None:
        print("❌ OSC_CHANGE_EN 设置失败，跳过本次测试")
        return None

    try:
        return _osc_test_body(i2c, cur, en)
    finally:
        # 测完恢复成选择前的值
        if orig_en is not None and orig_en != en:
            if _osc_change_en_write(i2c, orig_en):
                print(f"\n   已恢复 OSC_CHANGE_EN = {orig_en}")
            else:
                print("   ⚠️ 恢复 OSC_CHANGE_EN 失败，请手动确认")


def _osc_test_body(i2c, cur, en):
    """OSC 测试主体（假定 OSC_CHANGE_EN 已置 en）：选模式 → 直接测量 / TRIM 修调。"""
    mode = _ask_test_mode(
        "OSC", trim_field="RW_OSC_24M_TRIM", default=OSC_MODE_DIRECT,
        direct_desc=(f"在 OSC_CHANGE_EN = {en} 下测一次并按标准判定，"
                     f"不改 RW_OSC_24M_TRIM（当前 = {cur}）"),
        trim_desc=(f"在 OSC_CHANGE_EN = {en} 下二分搜索 RW_OSC_24M_TRIM，"
                   f"使频率接近 {OSC_TARGET_HZ:.0f} Hz"))

    if mode == OSC_MODE_DIRECT:
        # 直接测量范围：只测所选那一态，或 0/1 依次各测一次
        print("\n   OSC 直接测量范围：")
        print(f"      0 → 只测 OSC_CHANGE_EN = {en}")
        print("      1 → OSC_CHANGE_EN 依次置 0、1 各测一次（测完恢复到 "
              f"{en}）")
        sweep = _ask_int("直接测量范围 0/1", 1, 0, 1) == 1
        return _osc_measure_direct(i2c, cur, en, sweep=sweep)

    # ================= 模式 1：TRIM 修调 =================
    freq, _period, _duty = _osc_measure_freq("CH1")
    if freq is None:
        print("❌ OSC 频率测量失败，跳过本次测试")
        return None
    print(f"   频率 = {freq:.3f} Hz")

    passed = _judge_osc(freq, extra_note=f"OSC_CHANGE_EN={en}，修调 {cur}")
    if passed is True:
        print(f"OSC 测试通过, 测量值: {freq:.3f} Hz, 修调值为: {cur}, "
              f"OSC_CHANGE_EN = {en}, 无需修调")
        print("✅ OSC频率 完成")
        return freq

    def measure_at(code):
        """写修调 code 后测频率（Hz）；写入失败或测频失败返回 None。"""
        if not _osc_trim_write(i2c, code):
            print(f"   修调值={code:3d}: 写入失败（回读异常），跳过")
            return None
        time.sleep(OSC_TRIM_SETTLE_S)
        f, _p, _d = _osc_measure_freq("CH1")
        if f is None:
            print(f"   修调值={code:3d}: 频率测量失败，跳过")
            return None
        print(f"   修调值={code:3d}: 频率={f:9.3f} Hz, "
              f"误差={abs(f - OSC_TARGET_HZ):7.3f} Hz")
        return f

    print(f"   开始二分搜索 RW_OSC_24M_TRIM（0~{OSC_TRIM_MAX}，"
          f"OSC_CHANGE_EN = {en}）...")
    best_trim, _best_f = _trim_bisect_search(
        measure_at, target=OSC_TARGET_HZ, trim_max=OSC_TRIM_MAX,
        initial={cur: freq})

    if best_trim is None:
        print("❌ OSC 修调失败：扫描期间没有测到有效频率，写回原修调值")
        _osc_trim_write(i2c, cur)
        return None

    # 写回最优修调值并立即复测、判定
    if not _osc_trim_write(i2c, best_trim):
        print(f"❌ OSC 修调失败：写回修调值 {best_trim} 时回读异常")
        return None
    time.sleep(OSC_TRIM_SETTLE_S)
    best_f, _p, _d = _osc_measure_freq("CH1")
    if best_f is None:
        print(f"❌ OSC 修调失败：写回修调值 {best_trim} 后测量失败")
        return None

    passed = _judge_osc(best_f, extra_note=f"OSC_CHANGE_EN={en}，修调 {best_trim}")
    result = "成功" if passed is True else "失败"
    print(f"OSC 修调{result}, 测量值: {best_f:.3f} Hz, 修调值为: {best_trim}"
          f"（原修调值 {cur} → {best_trim}，"
          f"频率 {freq:.3f} → {best_f:.3f} Hz，OSC_CHANGE_EN = {en}）")

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


# ===== 比较器迟滞（规范 2.22 ~ 2.24，菜单 23 / 24 / 25）=====
# 测试方法（以 COMP0 为例）：
#   进测试模式 → 写 [20:9] 选通比较器测试通道 → 电源给 VO 加 1 V 固定电压 →
#   DAC 从低电平起点（默认 950 mV，可设置）以 **1 mV** 步进升压扫描 UO，
#   万用表观察 WO：
#     · WO 读到 > 3000 mV 后在同一电压点连续确认 5 次仍为高电平，
#       这段连续高电平的起始电压 − 1 V = 正向迟滞 Vhys+
#     · 再从**降压起点**（默认 1050 mV，可单独设置）以 1 mV 步进往回降压，
#       WO 读到 < 100 mV 后同样连续确认 5 次，
#       1 V − 这段连续低电平的起始电压 = 负向迟滞 Vhys-
#   迟滞档位由 COMPn_HYS 配置（[20:0] 的 [1:0] / [3:2] / [5:4]）；
#   规范 Typ：档位 00 → ±5 mV（即 Vhys+ = +5 mV、Vhys- = -5 mV）。

COMP_HYS_ADDR = 20
COMP_HYS_INDEX = 0
# 迟滞档位 → 理论迟滞绝对值（mV）
COMP_HYS_INFO = {
    0: 5.0,
    1: 10.0,
    2: 15.0,
    3: 20.0,
}
# 各比较器的测试通道 / 迟滞字段位置
# 各比较器的测试通道 / 迟滞字段位置 / 接线：
#   COMP0：DAC 扫描 UO，电源给 VO 加 1 V，万用表观察 WO
#   COMP1：DAC 扫描 VO，电源给 WO 加 1 V，万用表观察 UO
#   COMP2：DAC 扫描 WO，电源给 UO 加 1 V，万用表观察 VO
COMP_CHANNELS = {
    0: {"name": "COMP0", "sel": [0x00, 0x00, 0x00, 0x07],
        "bit_lo": 0, "item": "COMP0_hy",       # TEST_LOGIC=0011
        "dac": "UO", "watch": "WO", "fixed": "VO"},
    1: {"name": "COMP1", "sel": [0x00, 0x00, 0x00, 0x09],
        "bit_lo": 2, "item": "COMP1_hy",       # TEST_LOGIC=0100
        "dac": "VO", "watch": "UO", "fixed": "WO"},
    2: {"name": "COMP2", "sel": [0x00, 0x00, 0x00, 0x0B],
        "bit_lo": 4, "item": "COMP2_hy",       # TEST_LOGIC=0101
        "dac": "WO", "watch": "VO", "fixed": "UO"},
}
COMP_HYS_MASK = 0x03

# 比较器测试前置：先初始化并启动测试状态机（写 [1:32] → [1:31]），
# 与 TIA 测试同样的两拍时序；[20:9] 的通道选择必须在这之后写才生效。
COMP_SM_ADDR = 1
COMP_SM_INIT_INDEX = 32
COMP_SM_INIT_DATA = [0x00, 0x43, 0xFD, 0x33]     # [1:32] 初始化状态机
COMP_SM_START_INDEX = 31
COMP_SM_START_DATA = [0x00, 0x01, 0x02, 0x00]    # [1:31] 启动状态机

# 固定电压与供电通道（三个比较器共用同一路电源通道，接各自的 fixed 脚）
COMP_FIXED_V = 1.0                    # 未参与扫描的那一路固定 1 V
COMP_PSU_CHANNEL = 2                  # DP832A CH2

# 扫描参数
COMP_SCAN_START_MV = 950              # 升压扫描起点（被扫描脚的 DAC 电压，运行时可选）
COMP_SCAN_STOP_MV = 1050              # 升压扫描终点
COMP_SCAN_DOWN_START_MV = 1050        # 降压扫描起点（默认沿用升压终点，运行时可选）
COMP_SCAN_STEP_MV = 1                 # 扫描步进：固定 1 mV（阈值分辨力）
COMP_STEP_PERIOD_S = 0.1              # 每个扫描点的总节拍（DAC 设置 + 测量 + 补足到该时间）
COMP_DAC_SETTLE_S = 0.01              # 设完 DAC 后的最小等待（节拍内已包含）
COMP_WATCH_HIGH_MV = 3000.0           # 观察脚 > 该值判为高电平（高电平 ≈ 3.3 V）
COMP_WATCH_LOW_MV = 100.0             # 观察脚 < 该值判为低电平（低电平 ≈ 0 V）
COMP_CONFIRM_SAMPLES = 5              # 同一电压点连续 5 次同电平才确认翻转
COMP_CONFIRM_MAX_READS = 20           # 同一电压点最多读 20 次（中途抖一下就重新计数）
COMP_MAX_POINTS = 4000                # 单方向扫描点数上限（防止异常时死循环）
COMP_DMM_RANGE = "10V"                # 万用表电压量程
COMP_FAST_APERTURE_S = 0.0002         # 扫描时万用表积分时间（Aperture = Time，200 µs）
COMP_HYS_TOL_RATIO = 0.10             # 判定容差：各档位理论值 ±10%（与校准表 4.5~5.5 一致）


def _comp_state(level_mv):
    """把观察脚读数（mV）归类为 'high' / 'low' / 'mid' / None。

    · > COMP_WATCH_HIGH_MV（3000 mV）→ 高电平
    · < COMP_WATCH_LOW_MV（100 mV）  → 低电平
    · 两者之间 → 'mid'（翻转过渡区，既不算高也不算低，不用于判定翻转点）
    · 读数为 None（测量失败）→ None
    """
    if level_mv is None:
        return None
    if level_mv > COMP_WATCH_HIGH_MV:
        return "high"
    if level_mv < COMP_WATCH_LOW_MV:
        return "low"
    return "mid"


def _comp_hys_read(i2c, comp_id):
    """读 COMPn_HYS 档位编码（[20:0] 的对应 2 bit），失败返回 None。"""
    spec = COMP_CHANNELS[comp_id]
    rd = execute_read(i2c, COMP_HYS_ADDR, COMP_HYS_INDEX)
    if not rd or len(rd) < 4:
        print(f"   ⚠️  读取 [{COMP_HYS_ADDR}:{COMP_HYS_INDEX}] 失败，无法读取迟滞档位")
        return None
    word = (rd[0] << 24) | (rd[1] << 16) | (rd[2] << 8) | rd[3]
    return (word >> spec["bit_lo"]) & COMP_HYS_MASK


def _comp_hys_write(i2c, comp_id, code):
    """写 COMPn_HYS 档位（读-改-写，保留 [20:0] 其它字段），返回 True/False。"""
    spec = COMP_CHANNELS[comp_id]
    mask = COMP_HYS_MASK << spec["bit_lo"]
    ok = _write_word_field(i2c, COMP_HYS_ADDR, COMP_HYS_INDEX,
                           spec["bit_lo"], mask, code & COMP_HYS_MASK)
    if ok:
        readback = _comp_hys_read(i2c, comp_id)
        if readback != (code & COMP_HYS_MASK):
            print(f"   ⚠️  写入后回读档位为 {readback}，与期望 {code} 不一致")
    return ok


def _comp_ask_hys(name, current):
    """交互式选择迟滞档位：回车沿用当前值，输入 0~3 改档。返回编码。"""
    print(f"\n   当前 {name}_HYS 档位 = {current}（±{COMP_HYS_INFO.get(current, '?')} mV）")
    print("   可选档位：")
    for code, mv in COMP_HYS_INFO.items():
        mark = " ← 当前" if code == current else ""
        print(f"      {code:2d}（{code:02b}）→ ±{mv:.0f} mV{mark}")
    raw = _ask_int(f"选择迟滞档位 0~3", current, 0, 3)
    return raw


def _comp_dac_set(mv):
    """设置 DAC 输出（mV），并等待最短稳定时间。

    用 serial_send_command（只发不等固定长度应答）而不是 serial_send_and_receive：
    后者内部 ser.read(256) 会阻塞到串口超时（默认 2 s），每点白等 2 s。
    """
    instr.serial_send_command(f"spi_cmd_set_val mV {int(round(mv))}\r\n")
    time.sleep(COMP_DAC_SETTLE_S)


def _comp_watch_mv():
    """万用表读观察脚电压（mV），失败返回 None。

    优先走快速通道（dmm_read_fast_voltage，单点 3~5 ms）；
    驱动不支持或还没准备好时退回普通单次测量（约 100 ms）。
    """
    fast = getattr(instr, "dmm_read_fast_voltage", None)
    if fast is not None:
        val = fast()
        if val is not None:
            return val
    return instr.dmm_measure_single_voltage(voltage_type="DC", range_val=COMP_DMM_RANGE)


def _comp_step(mv):
    """一个扫描点：设 DAC → 读观察脚 → 把节拍补足到 COMP_STEP_PERIOD_S，返回读数。

    整个点（DAC 设置 + 稳定 + 测量）控制在 COMP_STEP_PERIOD_S（默认 0.1 s）内，
    测量快于节拍时补 sleep，测量慢于节拍时不额外等待。
    """
    t0 = time.perf_counter()
    _comp_dac_set(mv)
    level = _comp_watch_mv()
    elapsed = time.perf_counter() - t0
    if elapsed < COMP_STEP_PERIOD_S:
        time.sleep(COMP_STEP_PERIOD_S - elapsed)
    return level


def _comp_confirm(want, watch):
    """在当前电压点连续读数，直到**连续** COMP_CONFIRM_SAMPLES 次达到 want 状态。

    同一电压点最多读 COMP_CONFIRM_MAX_READS 次；中途读到别的状态（含中间态、
    测量失败）就重新计数，这样偶发的一次噪声不会把整个测量点作废。
    返回 True=已确认；False=读满次数仍未连续确认（调用方继续往后扫描）。
    """
    labels = {"high": "高电平", "low": "低电平", "mid": "中间态", None: "测量失败"}
    run = 0
    reads = []
    for _ in range(COMP_CONFIRM_MAX_READS):
        level = _comp_watch_mv()
        reads.append(level)
        run = run + 1 if _comp_state(level) == want else 0
        if run >= COMP_CONFIRM_SAMPLES:
            tail = "、".join("——" if x is None else f"{x:.0f}" for x in reads[-COMP_CONFIRM_SAMPLES:])
            print(f"      连续 {COMP_CONFIRM_SAMPLES} 次{labels[want]}确认"
                  f"（最近读数 {tail} mV）")
            return True
    print(f"      读 {len(reads)} 次仍未连续 {COMP_CONFIRM_SAMPLES} 次{labels[want]}，继续扫描")
    return False


def _comp_scan_edge(start_mv, stop_mv, want, watch, tag=""):
    """自 start_mv 沿 stop_mv 方向按 COMP_SCAN_STEP_MV 逐点扫描，找翻转点。

    每点设 DAC → 读一次观察脚；读到 want 状态时在**同一电压点**连续读数，
    连续 COMP_CONFIRM_SAMPLES 次仍是 want 才把这个电压记为翻转点
    （即“第一次达到该电平”的电压）。

    返回 (hit_mv, last_opposite_mv)：
        hit_mv           - 首次连续确认 want 的电压；未找到返回 None
        last_opposite_mv - 命中前最后一个确定的反状态电压（诊断用）
    """
    opposite = "low" if want == "high" else "high"
    step = COMP_SCAN_STEP_MV if stop_mv >= start_mv else -COMP_SCAN_STEP_MV
    labels = {"high": "高电平", "low": "低电平", "mid": "中间态", None: "测量失败"}

    mv = start_mv
    last_opposite = None
    for _ in range(COMP_MAX_POINTS):
        level = _comp_step(mv)
        state = _comp_state(level)
        print(f"   {mv:5d} mV{tag} → {watch} = "
              f"{'——' if level is None else f'{level:8.1f} mV'}（{labels[state]}）")
        if state == want and _comp_confirm(want, watch):
            return mv, last_opposite
        if state == opposite:
            last_opposite = mv

        nxt = mv + step
        if (stop_mv - nxt) * step < 0:      # 越过扫描终点
            break
        mv = nxt
    return None, last_opposite


def _comp_state_machine_init(i2c):
    """初始化并启动比较器测试状态机：先 [1:32]，再 [1:31]。

    必须在写 [20:9] 选通测试通道之前执行，否则通道切换不会生效。
    """
    init_hex = ' '.join(f'{b:02X}' for b in COMP_SM_INIT_DATA)
    print(f"   初始化状态机：[{COMP_SM_ADDR}:{COMP_SM_INIT_INDEX}] = {init_hex}")
    execute_write(i2c, addr=COMP_SM_ADDR, index=COMP_SM_INIT_INDEX,
                  write_data=list(COMP_SM_INIT_DATA))
    time.sleep(0.01)   # 寄存器写间隔

    start_hex = ' '.join(f'{b:02X}' for b in COMP_SM_START_DATA)
    print(f"   启动状态机：  [{COMP_SM_ADDR}:{COMP_SM_START_INDEX}] = {start_hex}")
    execute_write(i2c, addr=COMP_SM_ADDR, index=COMP_SM_START_INDEX,
                  write_data=list(COMP_SM_START_DATA))
    time.sleep(0.01)   # 寄存器写间隔


def _comp_hysteresis_test(comp_id):
    """COMPn 迟滞测试公共流程（规范 2.22 ~ 2.24）。

    三个比较器流程完全一致，只有「哪一路被扫描 / 哪一路固定 / 哪一路观察」不同，
    接线由 COMP_CHANNELS[comp_id] 的 dac / fixed / watch 给出：
        COMP0：扫 UO，固定 VO，看 WO
        COMP1：扫 VO，固定 WO，看 UO
        COMP2：扫 WO，固定 UO，看 VO

    流程：
      0. 初始化并启动测试状态机：[1:32] = 00 43 FD 33 → [1:31] = 00 01 02 00；
      1. 写 [20:9] 选通该比较器的测试通道；
      2. 读当前 COMPn_HYS 档位，交互选择档位（默认沿用当前），写回并回读；
      3. 电源 CH2 给 fixed 脚加 1 V 固定电压，测试期间保持输出；
      4. 升压扫描：DAC 从低电平起点（默认 950 mV，可设置）以 **1 mV** 步进
         走到 1050 mV，万用表观察 watch 脚；读到 > 3000 mV 后在同一电压点连续确认
         **5 次**，把这段连续高电平的起始电压记为上升沿 → Vhys+ = 上升沿 − 1 V；
      5. 降压扫描：自**降压起点**（默认等于升压终点 1050 mV，可单独设置）以
         **1 mV** 步进往回走，读到 < 100 mV 后同样连续确认 5 次，
         把这段连续低电平的起始电压记为下降沿 → Vhys- = 1 V − 下降沿；
      6. 按档位理论值 ±10% 判定并记录，最后 DAC 回 0、恢复电源通道状态。

    返回：
        (vhys_plus_mv, vhys_minus_mv)；未测到返回 None
    """
    spec = COMP_CHANNELS[comp_id]
    name, sel, bit_lo, item_name = spec["name"], spec["sel"], spec["bit_lo"], spec["item"]
    dac_pin, watch_pin, fixed_pin = spec["dac"], spec["watch"], spec["fixed"]

    print(f"\n🚀 执行 {name} 迟滞测试（规范 2.{22 + comp_id}）...")
    print(f"   {dac_pin} 由 DAC 扫描（升压 {COMP_SCAN_START_MV}~{COMP_SCAN_STOP_MV} mV、"
          f"降压起点 {COMP_SCAN_DOWN_START_MV} mV，均可设置），"
          f"{fixed_pin} 固定 {COMP_FIXED_V:.1f} V（电源 CH{COMP_PSU_CHANNEL}），"
          f"万用表观察 {watch_pin}")

    i2c = _get_i2c()

    # ⓪ 状态机初始化 + 启动（必须在写 [20:9] 之前）
    _comp_state_machine_init(i2c)

    # ① 选通测试通道
    sel_hex = ' '.join(f'{b:02X}' for b in sel)
    print(f"   测试通道：[20:9] = {sel_hex}")
    execute_write(i2c, addr=20, index=9, write_data=list(sel))
    _log_read(20, 9)
    time.sleep(0.1)

    # ② 迟滞档位（写 [20:0] 前复核 RW_TIA_IBIAS_SEL = 01，读-改-写不会破坏它）
    _tia_ibias_sel_ensure(i2c)
    cur_code = _comp_hys_read(i2c, comp_id)
    if cur_code is None:
        cur_code = 0
    code = _comp_ask_hys(name, cur_code)
    typ_mv = COMP_HYS_INFO.get(code, 5.0)
    if code == cur_code:
        print(f"   沿用现有档位 {code}（理论 ±{typ_mv:.0f} mV）")
    else:
        if not _comp_hys_write(i2c, comp_id, code):
            print(f"   ❌ 写入 {name}_HYS = {code} 失败（回读异常），跳过本次测试")
            return None
        print(f"   已设置 {name}_HYS = {code}（理论 ±{typ_mv:.0f} mV）")
    _log_read(COMP_HYS_ADDR, COMP_HYS_INDEX)

    # ③ 扫描范围（步进固定 1 mV，不再询问）
    start_mv = _ask_int("升压起始电压 mV", COMP_SCAN_START_MV, 0, 3000)
    stop_mv = COMP_SCAN_STOP_MV
    down_start_mv = _ask_int("降压起始电压 mV", COMP_SCAN_DOWN_START_MV, 0, 3000)
    if down_start_mv <= start_mv:
        print(f"   ⚠️ 降压起始 {down_start_mv} mV 不大于升压起始 {start_mv} mV，"
              f"改用默认 {COMP_SCAN_DOWN_START_MV} mV")
        down_start_mv = COMP_SCAN_DOWN_START_MV
    print(f"   升压 {start_mv} → {stop_mv} mV 找上升沿；"
          f"降压 {down_start_mv} → {start_mv} mV 找下降沿；"
          f"步进 {COMP_SCAN_STEP_MV} mV，连续 {COMP_CONFIRM_SAMPLES} 次同电平确认")
    print(f"   判定：{watch_pin} > {COMP_WATCH_HIGH_MV:.0f} mV 为高电平、"
          f"< {COMP_WATCH_LOW_MV:.0f} mV 为低电平")

    # ④ 仪器：电源 CH2 + 万用表
    if not _psu_ensure(f"{name} 迟滞需要电源给 {fixed_pin} 加固定电压，无法继续"):
        return None
    if not _dmm_ensure(f"{name} 迟滞需要万用表观察 {watch_pin}，无法继续"):
        return None

    _confirm_connection(
        f"请确定 DAC 输出接 {dac_pin}、电源 CH{COMP_PSU_CHANNEL} 接 {fixed_pin}、"
        f"万用表接 {watch_pin}")

    psu_was_on = instr.psu_output_state(COMP_PSU_CHANNEL) is True
    psu_prev_v = instr.psu_get_voltage(COMP_PSU_CHANNEL)
    started = False
    fast_dmm = False
    rise_mv = fall_mv = None

    try:
        # ④b 万用表切快速模式（只配置一次，逐点 READ? 约 3~5 ms）
        #     积分方式固定 Aperture = Time（驱动默认 200 µs）
        fast_dmm = instr.dmm_prepare_fast_voltage(
            voltage_type="DC", range_val=COMP_DMM_RANGE,
            aperture_s=COMP_FAST_APERTURE_S)
        if fast_dmm:
            print(f"   万用表已切快速模式（Aperture = Time "
                  f"{COMP_FAST_APERTURE_S * 1e6:.0f} µs，单点约 3~5 ms），"
                  f"节拍 {COMP_STEP_PERIOD_S * 1000:.0f} ms/点")
        else:
            print(f"   ⚠️  万用表快速模式不可用，退回普通单次测量（约 100 ms/点）")

        # ⑤ 加固定电压并保持输出
        if not instr.psu_set_voltage(COMP_PSU_CHANNEL, COMP_FIXED_V):
            print(f"   ❌ 设置电源 CH{COMP_PSU_CHANNEL} = {COMP_FIXED_V:.1f} V 失败")
            return None
        if not psu_was_on:
            print(f"   打开电源 CH{COMP_PSU_CHANNEL} 输出（测试结束后恢复为关闭）")
            if not instr.psu_output_on(COMP_PSU_CHANNEL):
                print(f"   ❌ 无法打开电源 CH{COMP_PSU_CHANNEL} 输出")
                return None
        started = True
        readback = instr.psu_get_voltage(COMP_PSU_CHANNEL)
        print(f"   {fixed_pin} = {readback if readback is not None else COMP_FIXED_V:.3f}"
              f" V（输出 {'ON' if instr.psu_output_state(COMP_PSU_CHANNEL) else 'OFF'}）")

        # ⑥ 升压：从低电平起点按 1 mV 逐点扫描，找上升沿
        print(f"\n   升压扫描 {dac_pin}（{start_mv} → {stop_mv} mV，"
              f"步进 {COMP_SCAN_STEP_MV} mV），寻找 {watch_pin} 连续 "
              f"{COMP_CONFIRM_SAMPLES} 次高电平 ...")
        labels = {"high": "高电平", "low": "低电平", "mid": "中间态", None: "测量失败"}
        start_level = _comp_step(start_mv)
        start_state = _comp_state(start_level)
        print(f"   起点 {start_mv} mV：{watch_pin} = "
              f"{'——' if start_level is None else f'{start_level:.1f} mV'}"
              f"（{labels[start_state]}）")
        if start_level is None:
            print(f"   ❌ 起点 {start_mv} mV 下读不到 {watch_pin}，未测得 Vhys+")
        elif start_state != "low":
            print(f"   ❌ 起点 {start_mv} mV 不是低电平——请降低扫描起始电压，"
                  f"或确认 {fixed_pin} 已加 {COMP_FIXED_V:.1f} V、接线与测试通道正确")
        else:
            rise_mv, _ = _comp_scan_edge(start_mv + COMP_SCAN_STEP_MV,
                                         stop_mv, "high", watch_pin, "（升压）")
            if rise_mv is None:
                print(f"   ❌ 扫到 {stop_mv} mV，{watch_pin} 仍未连续 "
                      f"{COMP_CONFIRM_SAMPLES} 次高电平，未测得 Vhys+")
            else:
                print(f"   上升沿 = {rise_mv} mV（Vhys+ = {rise_mv} − "
                      f"{COMP_FIXED_V * 1000:.0f} = {rise_mv - COMP_FIXED_V * 1000:+.1f} mV）")

        # ⑦ 降压：自「降压起始电压」按 1 mV 逐点往回扫，找下降沿
        if rise_mv is not None:
            print(f"\n   降压扫描 {dac_pin}（{down_start_mv} → {start_mv} mV，"
                  f"步进 {COMP_SCAN_STEP_MV} mV），寻找 {watch_pin} 连续 "
                  f"{COMP_CONFIRM_SAMPLES} 次低电平 ...")
            level = _comp_step(down_start_mv)
            state = _comp_state(level)
            print(f"   起点 {down_start_mv} mV：{watch_pin} = "
                  f"{'——' if level is None else f'{level:.1f} mV'}（{labels[state]}）")
            if level is None:
                print(f"   ❌ 降压起点 {down_start_mv} mV 下读不到 {watch_pin}，未测得 Vhys-")
            elif state != "high":
                print(f"   ❌ 降压起点 {down_start_mv} mV 不是高电平"
                      f"（上升沿 {rise_mv} mV）——起点低于下降阈值时芯片已翻低，"
                      f"测不到 Vhys-；请把降压起始电压改到上升沿以上")
            else:
                fall_mv, _ = _comp_scan_edge(down_start_mv - COMP_SCAN_STEP_MV,
                                             start_mv, "low", watch_pin, "（降压）")
                if fall_mv is None:
                    print(f"   ❌ 降回 {start_mv} mV，{watch_pin} 仍未连续 "
                          f"{COMP_CONFIRM_SAMPLES} 次低电平，未测得 Vhys-")
                else:
                    print(f"   下降沿 = {fall_mv} mV（Vhys- = "
                          f"{COMP_FIXED_V * 1000:.0f} − {fall_mv} = "
                          f"{COMP_FIXED_V * 1000 - fall_mv:+.1f} mV）")

    finally:
        # ⑧ 收尾：万用表恢复默认配置、DAC 回 0、电源通道恢复原状
        if fast_dmm:
            try:
                instr.dmm_finish_fast_voltage()
                print("   万用表已恢复默认配置（自动调零 ON、Aperture = Time 20 ms）")
            except Exception as e:
                print(f"   ⚠️  恢复万用表默认配置异常：{e}")
        try:
            _comp_dac_set(0)
            print(f"   DAC 已回 0 mV")
        except Exception as e:
            print(f"   ⚠️  DAC 回 0 失败：{e}")
        if started:
            try:
                if psu_was_on and psu_prev_v is not None:
                    instr.psu_set_voltage(COMP_PSU_CHANNEL, psu_prev_v)
                    print(f"   已恢复电源 CH{COMP_PSU_CHANNEL} 电压 {psu_prev_v:.3f} V（保持输出）")
                elif not psu_was_on:
                    if instr.psu_output_off(COMP_PSU_CHANNEL):
                        print(f"   已关闭电源 CH{COMP_PSU_CHANNEL} 输出（测试前为关闭）")
                    else:
                        print(f"   ⚠️  关闭电源 CH{COMP_PSU_CHANNEL} 失败，请手动确认")
            except Exception as e:
                print(f"   ⚠️  恢复电源状态异常：{e}")

    if rise_mv is None or fall_mv is None:
        print(f"\n❌ {name} 迟滞测试未取得完整的 Vhys+ / Vhys-")
        _ensure_result_session()
        note = (f"{name}，档位 {code}（理论 ±{typ_mv:.0f} mV）；"
                f"Vhys+={'未测到' if rise_mv is None else f'{rise_mv - COMP_FIXED_V * 1000:+.0f} mV'}，"
                f"Vhys-={'未测到' if fall_mv is None else f'{COMP_FIXED_V * 1000 - fall_mv:+.0f} mV'}")
        result_log.record(item_name, None, "mV", typ_v=typ_mv, result="FAIL", note=note)
        return None

    # ⑨ 计算与判定（按规范口径：Vhys± 都取正值，负向表示“低于 1 V 的那一侧”）
    base_mv = COMP_FIXED_V * 1000.0
    vhys_plus = rise_mv - base_mv        # 正向迟滞 = 上升沿 − 固定电压
    vhys_minus = base_mv - fall_mv       # 负向迟滞 = 固定电压 − 下降沿
    print(f"\n   {name} 结果（{dac_pin} 上升沿 {rise_mv} mV、下降沿 {fall_mv} mV，"
          f"{fixed_pin} = {COMP_FIXED_V:.1f} V）：")
    print(f"      Vhys+ = {rise_mv} − {base_mv:.0f} = {vhys_plus:+.1f} mV"
          f"（门限 {base_mv + vhys_plus:.0f} mV）")
    print(f"      Vhys- = {base_mv:.0f} − {fall_mv} = {vhys_minus:+.1f} mV"
          f"（门限 {base_mv - vhys_minus:.0f} mV，即 −{vhys_minus:.1f} mV 方向）")
    print(f"      迟滞带宽 = {vhys_plus + vhys_minus:.1f} mV（档位 {code} 理论带宽 "
          f"{2 * typ_mv:.0f} mV）")

    tol = COMP_HYS_TOL_RATIO
    limits = (typ_mv * (1 - tol), typ_mv * (1 + tol))
    note = f"{name}，档位 {code}（理论 ±{typ_mv:.0f} mV），上升沿 {rise_mv} mV / 下降沿 {fall_mv} mV"
    passed_plus = _judge_by_standard(item_name, vhys_plus, extra_note=note + "，Vhys+",
                                     limits=limits, typ=typ_mv)
    passed_minus = _judge_by_standard(item_name, vhys_minus, extra_note=note + "，Vhys-",
                                      limits=limits, typ=typ_mv)
    ok = (passed_plus is True) and (passed_minus is True)
    print(f"{'✅' if ok else '❌'} {name} 迟滞测试完成")
    return vhys_plus, vhys_minus


def COMP0_hy():
    """COMP0 迟滞测试（规范 2.22，菜单 [23]）。

    DAC 扫描 UO（950 → 1050 mV，起点可选，步进 1 mV），电源 CH2 给 VO 加 1 V，
    万用表观察 WO：连续 5 次高电平（>3000 mV）的起始点记 Vhys+ = UO − 1 V；
    再自**降压起始电压**（默认 1050 mV，可单独设置）往回扫，连续 5 次低电平
    （<100 mV）的起始点记 Vhys- = 1 V − UO。
    迟滞档位由 COMP0_HYS（[20:0] bit[1:0]）配置。
    """
    return _comp_hysteresis_test(0)


def COMP1_hy():
    """COMP1 迟滞测试（规范 2.23，菜单 [24]）。

    与 COMP0 流程相同，接线不同：DAC 扫描 **VO**，电源 CH2 给 **WO** 加 1 V，
    万用表观察 **UO**；WO→UO 翻转点对应 VO 的上升/下降沿。
    迟滞档位由 COMP1_HYS（[20:0] bit[3:2]）配置。
    """
    return _comp_hysteresis_test(1)


def COMP2_hy():
    """COMP2 迟滞测试（规范 2.24，菜单 [25]）。

    与 COMP0 流程相同，接线不同：DAC 扫描 **WO**，电源 CH2 给 **UO** 加 1 V，
    万用表观察 **VO**；UO→VO 翻转点对应 WO 的上升/下降沿。
    迟滞档位由 COMP2_HYS（[20:0] bit[5:4]）配置。
    """
    return _comp_hysteresis_test(2)


# ===== 输出漏电 / 输出阻抗（规范 2.26 ~ 2.28，菜单 27 / 28 / 29）=====
# 漏电：测试模式写 TEST_LOGIC=1001 让功率管全关，电源 CH2 经万用表（电流档）
#   分别给 UO / VO / WO 加 5 V 和 0 V，读到的电流即关断漏电流。
# 阻抗：TEST_LOGIC=1010（上管开）从上管抽 200 mA，Vo 为脚电压，
#   Rdson = (VM − Vo) / 200 mA；TEST_LOGIC=1011（下管开）往下管灌 200 mA，
#   Rdson = Vo / 200 mA。VM 取电源 CH1 实测电压（规范按 5 V）。

# 漏电通道：[20:9] = 00 00 00 13（TEST_LOGIC<3:0> = 1001）。
# 规范 2.26 的命令表把写入值误印成 00 00 00 07（与 COMP0 相同），以测试条件文字的
# 1001 为准（已与硬件确认）。
LEAKAGE_SEL = [0x00, 0x00, 0x00, 0x13]      # TEST_LOGIC=1001
LEAKAGE_PSU_CHANNEL = 2                      # DP832A CH2 提供 5 V / 0 V 偏置
LEAKAGE_PINS = ("UO", "VO", "WO")
LEAKAGE_BIAS_V = (5.0, 0.0)                  # 每个脚分别加 5 V、0 V
LEAKAGE_SETTLE_S = 0.3                       # 改偏置后的稳定时间
# 34465A 的最低电流档是 **100 µA**（规格 100 µA ~ 10 A），没有 10 µA 档；
# 100 µA 档在 6.5 位下的分辨力约 0.1 nA，测 1 µA 漏电足够。
LEAKAGE_DMM_RANGE = "100uA"
LEAKAGE_DMM_RANGE_BACKUP = "1mA"             # 超量程时自动换档
LEAKAGE_MAX_UA = 1.0                         # 规范 2.26：漏电流 Max 1 µA
# 测试期间把 CH2 限流设小：万用表电流档串在回路里，万一接线短路（或被测脚击穿），
# 大电流会直接流过电流档。该限流只需覆盖输出脚电容的充电电流。
LEAKAGE_PSU_CURRENT_LIMIT_A = 0.05           # 50 mA

IMPEDANCE_SEL_UP = [0x00, 0x00, 0x00, 0x15]    # TEST_LOGIC=1010（上管）
IMPEDANCE_SEL_DOWN = [0x00, 0x00, 0x00, 0x17]  # TEST_LOGIC=1011（下管）
IMPEDANCE_PINS = ("UO", "VO", "WO")
IMPEDANCE_CURRENT_A = 0.2                      # 抽 / 灌 200 mA
IMPEDANCE_SMU_VLIMIT_V = 6.0                   # 源表限压（上管回路接近 VM=5 V）
IMPEDANCE_SETTLE_S = 0.3
IMPEDANCE_CURRENT_TOL_RATIO = 0.05             # 实测电流与 200 mA 的允许偏差
IMPEDANCE_PSU_CHANNEL = 1                      # 芯片供电（读实际 VM，用于上管公式）
IMPEDANCE_DEFAULT_VM_V = 5.0                   # 读不到 CH1 时按规范 5 V 计算


def _leak_measure_ua():
    """万用表测漏电流，返回 (µA, 量程)；10 µA 档溢出时自动换 100 µA 档。

    34465A 超量程时返回 ±9.9E+37，这里按「溢出」处理并换档重测；
    返回 (None, None) 表示两个量程都没读到有效值（通信失败），调用方需记 FAIL。
    """
    for rng in (LEAKAGE_DMM_RANGE, LEAKAGE_DMM_RANGE_BACKUP):
        ma = instr.dmm_measure_single_current(current_type="DC", range_val=rng)
        if ma is None:
            print(f"   ⚠️ {rng} 档读数失败"
                  f"{'，改用备份档重试' if rng == LEAKAGE_DMM_RANGE else '，无可用读数'}")
            continue
        if abs(ma) >= 1e30:                 # 9.9E+37 = 超量程
            print(f"   ⚠️ {rng} 量程溢出"
                  + ("，改用 " + LEAKAGE_DMM_RANGE_BACKUP + " 档"
                     if rng == LEAKAGE_DMM_RANGE else "，无可用读数"))
            continue
        return ma * 1000.0, rng
    return None, None


def OUTPUT_leakage_test():
    """输出漏电测试（规范 2.26，菜单 [27]）。

    流程：
      1. 写 [20:9] = 00 00 00 13（TEST_LOGIC=1001）关断输出功率管；
      2. 电源 CH2(+) 串接万用表电流端后接被测脚，CH2(−) 接 GND；
      3. 对 UO / VO / WO 依次加 5 V 和 0 V，记录每个条件下的漏电流；
      4. 按规范 Max 1 µA 判定；finally 把 CH2 恢复到 0 V（本测试打开的关闭输出）。

    安全措施：
      · 先把 CH2 电压降到 **0 V** 再打开输出，之后每次换脚前也先降到 0 V，
        避免带电插拔；结束时恢复原来的限流值；
      · 测试期间 CH2 限流设成 LEAKAGE_PSU_CURRENT_LIMIT_A（50 mA），
        万一接线短路，流过万用表电流档的电流被限制在安全范围。

    说明：万用表电流档串在回路里，会在回路中引入一点压降，所以「实测偏置」
    读到的是电源端电压，与脚上电压差一个电流档的 burden 电压（µA 级别可忽略）。
    记录值单位 µA，正值表示电流从电源流向被测脚。

    返回：
        {(pin, bias_v): uA}；全部测量失败返回 None。
    """
    print("\n🚀 执行 输出漏电测试 ...")
    print("   方法：测试模式下关断功率管，电源 CH2 经万用表电流档给每个输出脚"
          "加 5 V / 0 V，读到的电流即关断漏电流")
    print(f"   判定：|I| ≤ {LEAKAGE_MAX_UA:.1f} µA（规范 2.26）")

    i2c = _get_i2c()
    execute_write(i2c, addr=20, index=9, write_data=list(LEAKAGE_SEL))
    _log_read(20, 9)
    time.sleep(0.1)

    if not _psu_ensure("输出漏电需要电源 CH2 给输出脚加偏置，无法继续"):
        return None
    if not _dmm_ensure("输出漏电需要万用表测电流，无法继续"):
        return None

    # 万用表切到直流电流档并锁定量程（µA 级读数，自动换档会带来跳变）
    if instr.dmm_set_dc_current(LEAKAGE_DMM_RANGE):
        print(f"   万用表已切到直流电流档，量程 {LEAKAGE_DMM_RANGE}")
    else:
        print(f"   ❌ 万用表未能切到直流电流档（量程 {LEAKAGE_DMM_RANGE}），"
              f"此时读数会是电压而不是电流，本项终止")
        _ensure_result_session()
        result_log.record("OUTPUT_leakage_test", None, "µA", result="FAIL",
                          note="万用表未能切换到直流电流档，未测量")
        return None

    ch = LEAKAGE_PSU_CHANNEL

    # 先落地 0 V：之前的测试可能把 CH2 留在 1 V（COMP）等电压上，
    # 若带着电压打开输出再让操作员接线，等于带电插拔万用表电流档。
    if _bias_set_voltage(ch, 0.0, settle=LEAKAGE_SETTLE_S) is None:
        print(f"   ❌ 无法把电源 CH{ch} 设到 0 V，跳过本次测试")
        return None

    # 限流改小并记住原值，结束时恢复
    prev_limit = instr.psu_get_current(ch)
    limit_changed = False
    if prev_limit is None or prev_limit > LEAKAGE_PSU_CURRENT_LIMIT_A + 1e-9:
        if instr.psu_set_current(ch, LEAKAGE_PSU_CURRENT_LIMIT_A):
            limit_changed = True
            print(f"   电源 CH{ch} 限流设为 {LEAKAGE_PSU_CURRENT_LIMIT_A * 1000:.0f} mA"
                  f"（原 {('未知' if prev_limit is None else f'{prev_limit:.3f} A')}，"
                  f"防止接线短路时大电流流过万用表电流档）")
        else:
            print(f"   ⚠️ 设置 CH{ch} 限流失败，继续测试（请注意接线，勿短路）")
    else:
        print(f"   电源 CH{ch} 现有 {prev_limit * 1000:.0f} mA 限流已足够小，保持不变")

    turned_on = False
    if instr.psu_output_state(ch) is not True:
        print(f"   电源 CH{ch} 当前未输出 → 打开输出（0 V，测试结束若为本测试"
              f"打开则关闭）")
        if not instr.psu_output_on(ch):
            print("   ❌ 无法打开电源输出，跳过本次测试")
            _restore_psu_limit(ch, prev_limit, limit_changed)
            return None
        turned_on = True
        time.sleep(1.5)

    results = {}
    try:
        for pin in LEAKAGE_PINS:
            # 换脚前先回 0 V，再让操作员重新接线
            if _bias_set_voltage(ch, 0.0, settle=LEAKAGE_SETTLE_S) is None:
                print("   ❌ 无法把偏置电压降到 0 V，停止后续引脚")
                break
            if not _confirm_connection(
                    f"请把电源 CH{ch}(+) 串接万用表电流端后接到 {pin}，"
                    f"CH{ch}(−) 接 GND（接线时偏置为 0 V）"):
                print("   ⚠️ 操作被中断，停止后续引脚")
                break

            for bias_v in LEAKAGE_BIAS_V:
                actual = _bias_set_voltage(ch, bias_v, settle=LEAKAGE_SETTLE_S)
                if actual is None:
                    print(f"   ❌ {pin} 加 {bias_v:.0f} V 失败")
                    continue

                ua, rng = _leak_measure_ua()
                if ua is None:
                    print(f"   ❌ {pin} @ {actual:.3f} V：电流测量失败")
                    _ensure_result_session()
                    result_log.record("OUTPUT_leakage_test", None, "µA",
                                      result="FAIL",
                                      note=f"{pin} 加 {bias_v:.0f} V；电流测量失败")
                    continue

                passed = abs(ua) <= LEAKAGE_MAX_UA
                results[(pin, bias_v)] = ua
                print(f"   {pin} @ {actual:6.3f} V → I = {ua:+9.3f} µA"
                      f"（量程 {rng}，{'PASS' if passed else 'FAIL'}）")
                _ensure_result_session()
                result_log.record("OUTPUT_leakage_test", ua, "µA",
                                  max_v=LEAKAGE_MAX_UA,
                                  result="PASS" if passed else "FAIL",
                                  note=f"{pin} 加 {bias_v:.0f} V（实测偏置 {actual:.3f} V），"
                                       f"量程 {rng}")
    finally:
        _bias_restore(ch, turned_on)
        _restore_psu_limit(ch, prev_limit, limit_changed)

    print("\n   漏电流汇总：")
    for pin in LEAKAGE_PINS:
        line = [f"      {pin}："]
        for bias_v in LEAKAGE_BIAS_V:
            ua = results.get((pin, bias_v))
            line.append(f"{bias_v:.0f} V → {'未测到' if ua is None else f'{ua:+.3f} µA'}")
        print("  ".join(line))

    if not results:
        print("❌ 输出漏电测试未取得任何有效读数")
        return None
    print("✅ 输出漏电测试 完成")
    return results


def _impedance_pin_measure(pin, sink, vm_v):
    """对一个输出脚抽/灌 200 mA 并计算 Rdson。返回 (ohm, 脚电压Vo, i_a)；失败返回 (None,...)。

    调用前源表输出必须关闭；本函数上电测量后**一定**关闭输出（换脚前不能带电）。
    """
    sign = -1.0 if sink else 1.0
    action = "抽" if sink else "灌"

    def _fail(msg):
        print(f"   ❌ {pin}：{msg}")
        return None, None, None

    try:
        if not instr.smu_set_current(sign * IMPEDANCE_CURRENT_A,
                                     voltage_limit=IMPEDANCE_SMU_VLIMIT_V):
            return _fail("源表设置电流失败")
        if not instr.smu_output_on():
            return _fail("源表输出打开失败")
        time.sleep(IMPEDANCE_SETTLE_S)

        v, i = instr.smu_measure()
        if v is None or i is None:
            return _fail("源表读数失败")
    finally:
        try:
            instr.smu_output_off()
        except Exception as e:
            print(f"   ⚠️ 关闭源表输出异常：{e}")

    i_err = i - sign * IMPEDANCE_CURRENT_A
    if abs(i_err) > IMPEDANCE_CURRENT_A * IMPEDANCE_CURRENT_TOL_RATIO:
        print(f"   ⚠️ {pin} 实测电流 {i * 1000:+.1f} mA 与 {action} 200 mA 偏差较大，"
              f"结果可能不可靠（检查接线/回路）")

    # v 是源表端电压，即被测脚电压 Vo；上管的压降是 VM − Vo，下管的压降就是 Vo
    drop_v = (vm_v - v) if sink else v
    ohm = drop_v / IMPEDANCE_CURRENT_A
    if drop_v < 0:
        print(f"   ⚠️ {pin} 计算得到负压降（Vo = {v:.4f} V，VM = {vm_v:.3f} V），"
              f"检查供电电压与接线")
    extra = f"，VM = {vm_v:.3f} V，VM−Vo = {drop_v:.4f} V" if sink else ""
    print(f"   {pin} {action} {abs(i) * 1000:.1f} mA → Vo = {v:.4f} V{extra}，"
          f"Rdson = {ohm:.4f} Ω")
    return ohm, v, i


def _output_impedance_test(item_name, label, sel, sink, action):
    """上管 / 下管输出阻抗测试公共流程（规范 2.27 / 2.28）。

    参数：
        item_name - 校准表中的测试项名（UP_output_impedance / down_output_impedance）
        label     - 显示名（上管 / 下管）
        sel       - [20:9] 写入值（TEST_LOGIC=1010 / 1011）
        sink      - True=抽 200 mA（上管），False=灌 200 mA（下管）
        action    - "抽" / "灌"
    """
    print(f"\n🚀 执行 {label}输出阻抗测试 ...")
    print(f"   方法：测试模式下对 UO / VO / WO 分别{action} 200 mA，"
          f"记录各脚电压 Vo 后计算 Rdson")
    if sink:
        print(f"   上管公式：Rdson = (VM − Vo) / 200 mA，VM 取电源 CH{IMPEDANCE_PSU_CHANNEL} 实测值")
    else:
        print("   下管公式：Rdson = Vo / 200 mA")

    i2c = _get_i2c()
    execute_write(i2c, addr=20, index=9, write_data=list(sel))
    _log_read(20, 9)
    time.sleep(0.1)

    if not _smu_ensure(f"{label}输出阻抗需要源表{action} 200 mA，无法继续"):
        return None

    # 上管公式需要 VM：读芯片供电的实际电压（读不到按规范 5 V）
    vm_v = IMPEDANCE_DEFAULT_VM_V
    if sink:
        try:
            measured = instr.psu_measure_voltage(IMPEDANCE_PSU_CHANNEL)
        except Exception as e:
            measured = None
            print(f"   ⚠️ 读取电源 CH{IMPEDANCE_PSU_CHANNEL} 电压异常：{e}")
        if measured is None:
            print(f"   ⚠️ 读不到电源 CH{IMPEDANCE_PSU_CHANNEL} 电压，按规范 "
                  f"{IMPEDANCE_DEFAULT_VM_V:.1f} V 计算")
        else:
            vm_v = measured
            print(f"   芯片供电 VM = {vm_v:.3f} V")
            if abs(vm_v - IMPEDANCE_DEFAULT_VM_V) > 0.1:
                print(f"   ⚠️ VM 不是规范假设的 {IMPEDANCE_DEFAULT_VM_V:.1f} V，"
                      f"已按实测值计算（请确认供电设置）")

    results = {}
    for pin in IMPEDANCE_PINS:
        if not _confirm_connection(f"请把源表接到 {pin}（本测试对 {pin} {action} 200 mA）"):
            print("   ⚠️ 操作被中断，停止后续引脚")
            break

        ohm, _vo, i = _impedance_pin_measure(pin, sink, vm_v)
        if ohm is None:
            _ensure_result_session()
            result_log.record(item_name, None, "Ω", result="FAIL",
                              note=f"{pin} {action} 200 mA 测量失败")
            continue

        results[pin] = ohm
        note = (f"{pin} {action} {abs(i) * 1000:.1f} mA，"
                f"Vo = {(_vo) * 1000:.1f} mV" + (f"，VM = {vm_v:.3f} V" if sink else ""))
        _judge_by_standard(item_name, ohm, extra_note=note)

    print(f"\n   {label}输出阻抗汇总：")
    for pin in IMPEDANCE_PINS:
        ohm = results.get(pin)
        print(f"      {pin}：{'未测到' if ohm is None else f'{ohm:.4f} Ω'}")

    if not results:
        print(f"❌ {label}输出阻抗测试未取得任何有效读数")
        return None
    print(f"✅ {label}输出阻抗 完成")
    return results


def UP_output_impedance():
    """上管输出阻抗测试（规范 2.27，菜单 [28]）。

    写 [20:9] = 00 00 00 15（TEST_LOGIC=1010）打开上管，
    分别对 UO / VO / WO 抽 200 mA，Rdson = (VM − Vo) / 200 mA。
    """
    return _output_impedance_test("UP_output_impedance", "上管",
                                  IMPEDANCE_SEL_UP, sink=True, action="抽")


def down_output_impedance():
    """下管输出阻抗测试（规范 2.28，菜单 [29]）。

    写 [20:9] = 00 00 00 17（TEST_LOGIC=1011）打开下管，
    分别对 UO / VO / WO 灌 200 mA，Rdson = Vo / 200 mA。
    """
    return _output_impedance_test("down_output_impedance", "下管",
                                  IMPEDANCE_SEL_DOWN, sink=False, action="灌")


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
    return True, "已下发 MTP 全片擦除指令（ADDR=16 / 0x1000，已等待 300 ms）"


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

# 全片擦除：指令与时序见 MTPController.erase_chip()（ADDR=16 / 0x1000 / 固定等待 300 ms）。
# 擦除后逐字节读回确认空白：MTP 只能把 bit 由 1 写成 0，擦完应全为 0xFF。
MTP_ERASE_BLANK_VALUE = 0xFF

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
    # MTP 配置表 txt 与烧录模板都放在 Save_Results/MTP_Table 下
    save_dir = result_log.MTP_DIR
    try:
        kwargs = {
            "title": "选择 MTP 烧录模板（.xlsx 或 mtp_config_table.txt）",
            "filetypes": [("MTP 模板", "*.xlsx *.txt"),
                          ("Excel 文件", "*.xlsx"),
                          ("文本配置表", "*.txt"),
                          ("所有文件", "*.*")],
        }
        if save_dir.exists():
            kwargs["initialdir"] = str(save_dir)   # txt / 模板默认生成在 MTP_Table
        path = filedialog.askopenfilename(**kwargs)
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



def _mtp_verify_chip_erased(mtp, addr_lo=0, addr_hi=511):
    """读回 MTP 确认全片擦除是否真的生效。

    为什么不再轮询 (31,0)：手册「五、MTP 擦除操作」规定全片擦除是
    「写 0x1000 触发 → 强制等待 300 ms」，并未定义片擦完成状态位；
    (31,0) 的 bit0 只用于扇区擦除/编程/读触发命令的应答确认，
    拿它等全片擦除会一直读不到期望值，误报「擦除超时」。

    这里改为直接读回校验：MTP 只能把 bit 由 1 写成 0，擦除后应为 0xFF。
      · 字节 0~3 手册标注「用途特殊、不在普通可擦扇区列表内」，
        只打印其值供参考，不计入失败；
      · 4~511 任一字节非 0xFF 或读回失败，即判擦除未完成。

    返回：
        (ok, dirty, failed)
            dirty: list[(addr, value)] - 非空白的字节
            failed: list[addr]         - 读回失败的字节
    """
    total = addr_hi - addr_lo + 1
    dirty, failed = [], []
    special = {}

    for addr in range(addr_lo, addr_hi + 1):
        ok, val = _mtp_read_byte_value(mtp, addr)
        if not ok:                      # 读回失败重试一次，排除偶发通信错误
            ok, val = _mtp_read_byte_value(mtp, addr)
        if not ok:
            failed.append(addr)
        elif addr < 4:
            special[addr] = val
        elif val != MTP_ERASE_BLANK_VALUE:
            dirty.append((addr, val))

    if dirty or failed:
        if dirty:
            print(f"   ❌ 擦除不完整：{len(dirty)} 字节非空白")
            for addr, val in dirty[:20]:
                print(f"      地址 {addr}: 0x{val:02X}（应为 0x{MTP_ERASE_BLANK_VALUE:02X}）")
            if len(dirty) > 20:
                print(f"      ...（其余 {len(dirty) - 20} 处略）")
        if failed:
            print(f"   ❌ {len(failed)} 字节读回失败，无法确认擦除结果：{failed[:20]}"
                  + ("..." if len(failed) > 20 else ""))
    else:
        print(f"   ✅ 全片擦除确认通过：字节 {max(addr_lo, 4)}~{addr_hi} 全部为 "
              f"0x{MTP_ERASE_BLANK_VALUE:02X}（共 {total - len(special)} 字节）")

    if special:
        print("   ℹ️  字节 0~3 属手册标注的特殊字节，仅供参考："
              + " ".join(f"[{a}]=0x{v:02X}" for a, v in sorted(special.items())))

    return (not dirty and not failed), dirty, failed


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

    # 进入测试模式后恢复默认偏置档 RW_TIA_IBIAS_SEL = 01（[20:0] bit[24:23]）
    _tia_ibias_sel_ensure(i2c)

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
      3. 确认后 Open MTP EN → 全片擦除（ADDR=16 / 0x1000，固定等待 300 ms）
         → 读回全部 512 字节确认空白 0xFF
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
        print("   下发全片擦除（ADDR=16 / 0x1000，按手册固定等待 300 ms）...")
        mtp.erase_chip()          # 内部按手册地址下发并等待 300 ms
        print("   读回确认全片已擦除（空白 0xFF）...")
        if not _mtp_verify_chip_erased(mtp):
            print("   ⚠️  擦除检查未通过，已放弃烧录（避免把新数据烧进未擦除的区域）")
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
