import time
import sys
import random
import os
import re
import json
import openpyxl
from instrument import instrument_set as instr
from chips.OCD5010 import get_data as test_item
from chips.OCD5010.gpio_config import init_pa5_pa6_altfunc14
from Save_Results.test_recorder import TestContext
from functools import wraps
from types import SimpleNamespace

# 全局变量：保存当前激活的 TestContext 实例，供 save_item_result 委托调用
_current_test_ctx = None

def test_case(func):
    """装饰器：自动将测试函数包裹在 TestContext 中，捕获 print 输出并记录到独立工作表"""
    @wraps(func)
    def wrapper(*args, **kwargs):
        global _current_test_ctx
        excel_path = os.path.join(
            _output_base_dir, "Save_Results", "OCD5010", "item_results.xlsx"
        )
        with TestContext(excel_path=excel_path) as ctx:
            _current_test_ctx = ctx
            try:
                return func(*args, **kwargs)
            finally:
                _current_test_ctx = None
    return wrapper

#这里存放全局变量, 某个函数的某个变量需要在其他函数使用可以存入这个字典
global_use = {"RW_REF_ITRM":0, "RW_LDO12_VTRM":0, "RW_BG_TTRM":0, "RW_ADC_VREFH_VTRM":0, "RW_REF_VTRM":0, "RW_HSI_FTRM":0, 
              "RW_LSI_FTRM":0, "RW_SLP_REF_VTRM":0, "RW_TS_ITRM":0, "BG_TTRM_BEST":0, "REF_VTRM_BEST":0, "REF_ITRM_BEST":0, 
              "LDO12_VTRM_BEST":0, "SLP12_VTRM_BEST":0,"SLP_REF_VTRM_BEST":0, "ADC_VREFH_VTRM_BEST":0, "HSI_FTRM_BEST":0, 
              "LSI_FTRM_BEST":0, "TS_ITRM_BEST":0}

def save_item_result(test_name, passed, actual_value=None):
    """保存测试结果到 Save_Results/OCD5010/item_results.xlsx"""
    # 如果当前在 TestContext 中运行，委托给 ctx.record_result（自动捕获 print + 保存到独立工作表）
    if _current_test_ctx is not None:
        _current_test_ctx.record_result(
            test_name,
            "pass" if passed else "fail",
            actual_value
        )
        return
    # 回退：直接操作 Excel（无 TestContext 时使用）
    save_dir = os.path.join(_output_base_dir, "Save_Results", "OCD5010")
    os.makedirs(save_dir, exist_ok=True)
    excel_path = os.path.join(save_dir, "item_results.xlsx")
    try:
        if not os.path.exists(excel_path):
            print(f"  ⚠️ item_results.xlsx 不存在: {excel_path}")
            return
        wb = openpyxl.load_workbook(excel_path)
        ws = wb.active
        found = False
        for row in ws.iter_rows(min_row=1, max_col=1, values_only=False):
            if row[0].value == test_name:
                ws.cell(row=row[0].row, column=2, value="PASS" if passed else "FAIL")
                if actual_value is not None:
                    ws.cell(row=row[0].row, column=3, value=actual_value)
                found = True
                break
        if not found:
            print(f"  ⚠️ 未在 item_results.xlsx 中找到测试项: {test_name}")
        wb.save(excel_path)
    except Exception as e:
        print(f"保存测试结果到Excel失败: {e}")

def save_trim_to_excel(test_name, reg_name, reg_addr, best_trim, passed=None, best_vol=None):
    """保存最佳TRIM值到Save_Results/OCD5010/trim_best_value.xlsx, 同测试项覆盖旧值

    best_vol: 可选, 该修调项校准后的实测输出电压 (mV, 浮点)。仅 VREF(vref_1d2v)
        使用 —— 该值作为 ANACINFO0 的内容烧入 OTP (见 program_trim.py / otp_slots)。
    """
    save_dir = os.path.join(_output_base_dir, "Save_Results", "OCD5010")
    os.makedirs(save_dir, exist_ok=True)
    excel_path = os.path.join(save_dir, "trim_best_value.xlsx")
    headers = ["测试项名称", "寄存器地址名", "寄存器地址(Hex)", "最佳TRIM值", "测试结果", "最佳输出电压(mV)"]
    try:
        if os.path.exists(excel_path):
            wb = openpyxl.load_workbook(excel_path)
            ws = wb.active
            existing_headers = [cell.value for cell in ws[1]]
            if existing_headers != headers:
                wb.close()
                # 旧文件只有4/5列, 重建表头 (保留旧行, 缺列补 None)
                old_wb = openpyxl.load_workbook(excel_path)
                old_ws = old_wb.active
                old_rows = list(old_ws.iter_rows(min_row=2, values_only=True))
                old_wb.close()
                os.remove(excel_path)
                wb = openpyxl.Workbook()
                ws = wb.active
                ws.append(headers)
                for old_row in old_rows:
                    row_data = list(old_row)
                    if len(row_data) < 6:
                        row_data.extend([None] * (6 - len(row_data)))
                    ws.append(row_data)
        else:
            wb = openpyxl.Workbook()
            ws = wb.active
            ws.append(headers)
        found = False
        for row in ws.iter_rows(min_row=2, max_col=1, values_only=False):
            if row[0].value == test_name:
                ws.cell(row=row[0].row, column=2, value=reg_name)
                ws.cell(row=row[0].row, column=3, value=f"0x{reg_addr:08X}")
                ws.cell(row=row[0].row, column=4, value=best_trim)
                if passed is not None:
                    ws.cell(row=row[0].row, column=5, value="PASS" if passed else "FAIL")
                if best_vol is not None:
                    ws.cell(row=row[0].row, column=6, value=best_vol)
                found = True
                break
        if not found:
            row_data = [test_name, reg_name, f"0x{reg_addr:08X}", best_trim,
                        ("PASS" if passed else "FAIL") if passed is not None else None,
                        best_vol]
            ws.append(row_data)
        wb.save(excel_path)
    except Exception as e:
        print(f"保存TRIM值到Excel失败: {e}")

# 基于脚本所在目录构造Excel路径, 避免CWD影响
_script_dir = os.path.dirname(os.path.abspath(__file__))

# PyInstaller 打包后 (--onefile), __file__ 指向临时解压目录 (sys._MEIPASS)，
# 只读配置(register.xlsx等)可以继续从该目录读取(需用 --add-data 保持相同相对结构)，
# 但测试结果等需要持久化的输出必须写到 exe 实际所在目录旁边，否则临时目录退出即清空、结果丢失。
if getattr(sys, "frozen", False):
    _output_base_dir = os.path.dirname(sys.executable)
else:
    # 源码运行时统一用项目根目录, 与 trim/program_trim.py、trim/program_records.py 保持一致,
    # 否则修调测试写入的 trim_best_value.xlsx 与烧写时读取的不是同一个文件
    _output_base_dir = os.path.normpath(os.path.join(_script_dir, "..", ".."))

_register_path = os.path.join(_script_dir, "..", "..", "test_configs", "OCD5010", "register.xlsx")
_testdata_path = os.path.join(_script_dir, "..", "..", "test_configs", "OCD5010", "test.xlsx")

# BG_NEW_TRIM / ADC_RMSE_TRIM 的Excel结果统一另存到该目录下, 各自使用独立子文件夹
_CHIP_TEST_RESULTS_DIR = r"D:\destop\Chip_Test_Results"

try:
    df_register = test_item.load_excel_data(_register_path)
    df_test_data = test_item.load_excel_data(_testdata_path)
except Exception as e:
    print(f"Excel数据加载失败: {e}")
    sys.exit(1)

# 加载引脚配置：pin_configs.json → pin_cfg.GPIO5 / pin_cfg.VCC_S 方式调用
_pin_config_path = os.path.join(_script_dir, "..", "..", "test_configs", "OCD5010", "pin_configs.json")
if os.path.exists(_pin_config_path):
    with open(_pin_config_path, encoding='utf-8') as f:
        pin_cfg = SimpleNamespace(**json.load(f))
else:
    print(f"  ⚠️ pin_configs.json 不存在: {_pin_config_path}，将使用空配置")
    pin_cfg = SimpleNamespace()

# 加载仪器通道配置：equip_configs.json → eq_cfg.SCOPE_CH1 / eq_cfg.DAC1_CMD 方式调用
_equip_config_path = os.path.join(_script_dir, "..", "..", "instrument", "equip_configs.json")
if os.path.exists(_equip_config_path):
    with open(_equip_config_path, encoding='utf-8') as f:
        eq_cfg = SimpleNamespace(**json.load(f))
else:
    print(f"  ⚠️ equip_configs.json 不存在: {_equip_config_path}，将使用空配置")
    eq_cfg = SimpleNamespace()


def frange(start, stop, step=1.0):
    current = start
    while current < stop:
        yield current
        current += step

"""-------------测试项请在以下编写----------------"""

@test_case
def REF_VTRM():
    test_item_name = "vref_1d2v"
    row = df_test_data[df_test_data["测试项"] == test_item_name]
    if row.empty:
        print(f"未找到测试项: {test_item_name}")
        return
    try:
        min_val = float(row["Min."].values[0])
        typ_val = float(row["Typ."].values[0])
        max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
        if unit == "V" :
            min_val_vref = min_val * 1000.0
            typ_val_vref = typ_val * 1000.0
            max_val_vref = max_val * 1000.0
    except (IndexError, ValueError) as e:
        print(f"❌ 测试参数读取失败: {e}")
        return
    try:
        RW_ATP_MUX = test_item.get_32bit_reg_address("RW_ATP_MUX", df_register)
        RW_ATP_MUX_EN = test_item.get_32bit_reg_address("RW_ATP_MUX_EN", df_register)

        RW_ANA_REF_EN = test_item.get_32bit_reg_address("RW_ANA_REF_EN", df_register)
        PA5_AFS14_EN = test_item.get_32bit_reg_address("PA5_AFS14_EN", df_register)
        PA6_AFS14_EN = test_item.get_32bit_reg_address("PA6_AFS14_EN", df_register)
        PA_ANA_EN = test_item.get_32bit_reg_address("PA_ANA_EN", df_register)

        RW_DTP_MUX = test_item.get_32bit_reg_address("RW_DTP_MUX", df_register)
        RW_DTP_MUX_EN = test_item.get_32bit_reg_address("RW_DTP_MUX_EN", df_register)

        ANAC_TRM1 = test_item.get_32bit_reg_address("ANAC_TRM1", df_register)
        RW_REF_VTRM = test_item.get_32bit_reg_address("RW_REF_VTRM", df_register)
    except Exception as e:
        print(f"❌ 寄存器地址获取失败: {e}")
        return

    print("Reference 电压修调 — REF_VTRM")

    # 测试前置: 配置 PA5/PA6 复用 ALT_FUNC_14 (开 GPIO 时钟 + 输出方向 + AF14)
    init_pa5_pa6_altfunc14()

    time.sleep(0.1)

    instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
    time.sleep(0.1)
    instr.swd_modify_register(RW_ANA_REF_EN, 0x01, 24, 24)
    time.sleep(0.1)
    instr.swd_modify_register(PA6_AFS14_EN, 0x0E, 24, 27)
    time.sleep(0.1)


    #确认 ATP_MUX 选中 VREF
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 6, 6)
    time.sleep(0.1)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 7, 7)
    time.sleep(0.1)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)
    time.sleep(0.1)
    instr.swd_modify_register(RW_ATP_MUX, 0x02, 0, 4)
    
    time.sleep(0.5)
    trim = instr.swd_read_register(ANAC_TRM1)
    if trim is None:
        print("❌ 读取 ANAC_TRM1 失败 (SWD 异常), 跳过本次修调")
        return
    cur = (trim >> 0) & 0x1f

    v_a = instr.dmm_measure_single_voltage()
    passed = (v_a >= min_val_vref and v_a <= max_val_vref)
    if passed :
        print(f"VREF测试通过,修调范围为: {min_val_vref}~{max_val_vref} mV, 实际测量值: {v_a:.3f} mV, 修调值为：{cur}, 无需修调")
        global_use["RW_REF_VTRM"] = RW_REF_VTRM
        global_use["REF_VTRM_BEST"] = cur
        save_trim_to_excel(test_item_name, "RW_REF_VTRM", RW_REF_VTRM, cur, passed, best_vol=v_a)
        save_item_result(test_item_name, passed, round(v_a, 3))
        instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
        time.sleep(0.5)
        return
    else:
        print(f"VREF测试不通过,修调范围为: {min_val_vref}~{max_val_vref} mV, 实际测量值: {v_a:.3f} mV, 修调值为：{cur}, 需要进行修调")

    new = min(31, cur + 1)
    instr.swd_modify_register(RW_REF_VTRM, new, 0, 4)
    time.sleep(0.5)
    v_b = instr.dmm_measure_single_voltage()

    step = v_b - v_a
    if abs(step) > 0.00001:
        trim_target = round((typ_val_vref - v_a) / step) + cur
        trim_target = max(0, min(31, trim_target))
    else:
        trim_target = cur

    best_trim = trim_target

    min_err = float('inf')

    #复测，在计算出的修调范围内获取最佳修调值
    range_vtrm = range(trim_target - 5, trim_target + 5)
    for test_trim in range_vtrm:
        if test_trim < 0 or test_trim > 31:
            continue
        instr.swd_modify_register(RW_REF_VTRM, test_trim, 0, 4)
        time.sleep(0.5)
        test_vol = instr.dmm_measure_single_voltage()
        test_err = abs(test_vol - typ_val_vref)
        print(f"测试修调值={test_trim}, 测试电压={test_vol}, 与目标周期误差={test_err}mV")

        if test_err < min_err:
            best_trim = test_trim
            min_err = test_err

    instr.swd_modify_register(RW_REF_VTRM, best_trim, 0, 4)
    time.sleep(0.5)
    global_use["RW_REF_VTRM"] = RW_REF_VTRM
    global_use["REF_VTRM_BEST"] = best_trim
    best_vol = (float)(instr.dmm_measure_single_voltage())
    
    passed = (best_vol >= min_val_vref and best_vol <= max_val_vref)
    save_trim_to_excel(test_item_name, "RW_REF_VTRM", RW_REF_VTRM, best_trim, passed, best_vol=best_vol)
    save_item_result(test_item_name, passed, round(best_vol, 3))
    if passed:
        print("REF_VTRM Test pass")
        print(f"修调范围为: {min_val_vref}~{max_val_vref} mV, 实际测量值: {best_vol:.3f} mV, 修调值为：{best_trim}")
    else:
        print("REF_VTRM Test failed")
        print(f"修调范围为: {min_val_vref}~{max_val_vref} mV, 实际测量值: {best_vol:.3f} mV, 修调值为：{best_trim}")
    instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
    time.sleep(1)


@test_case              #wait
def REF_ITRM():
    test_item_name = "ref_10u"
    row = df_test_data[df_test_data["测试项"] == test_item_name]
    if row.empty:
        print(f"未找到测试项: {test_item_name}")
        return
    try:
        min_val = float(row["Min."].values[0])
        typ_val = float(row["Typ."].values[0])
        max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
    except (IndexError, ValueError) as e:
        print(f"❌ 测试参数读取失败: {e}")
        return
    try:
        RW_ANA_EN = test_item.get_32bit_reg_address("RW_ANA_EN", df_register)
        RW_ANA_BG_EN = test_item.get_32bit_reg_address("RW_ANA_BG_EN", df_register)
        RW_ANA_REF_EN = test_item.get_32bit_reg_address("RW_ANA_REF_EN", df_register)
        RW_ATP_MUX = test_item.get_32bit_reg_address("RW_ATP_MUX", df_register)
        RW_ATP_MUX_EN = test_item.get_32bit_reg_address("RW_ATP_MUX_EN", df_register)
        ANAC_TRM1 = test_item.get_32bit_reg_address("ANAC_TRM1", df_register)
        RW_REF_ITRM = test_item.get_32bit_reg_address("RW_REF_ITRM", df_register)
    except Exception as e:
        print(f"❌ 寄存器地址获取失败: {e}")
        return

    print("Reference 电流校准 — REF_ITRM")
    print("在 GPIO5 到地之间接 100KΩ 电阻(10µA×100KΩ=1V)")

    # 测试前置: 配置 PA5/PA6 复用 ALT_FUNC_14 (开 GPIO 时钟 + 输出方向 + AF14)
    init_pa5_pa6_altfunc14()

    instr.serial_send_and_receive(f"cmd_gpio_set_595d_test set {pin_cfg.GPIO5} {eq_cfg.DMM_MODE}\r\n")
    time.sleep(0.5)

    #使能相关模块
    instr.swd_modify_register(RW_ANA_EN, 0x01, 0, 0)
    instr.swd_modify_register(RW_ANA_BG_EN, 0x01, 23, 23)
    instr.swd_modify_register(RW_ANA_REF_EN, 0x01, 24, 24)
    time.sleep(0.1)

    #确认 ATP_MUX 选中 IREF
    instr.swd_modify_register(RW_ATP_MUX, 0b10110, 0, 4)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)

    trim = instr.swd_read_register(ANAC_TRM1)
    if trim is None:
        print("❌ 读取 ANAC_TRM1 失败 (SWD 异常), 跳过本次修调")
        return
    default_ref_itrm = (trim >> 5) & 0xf

    # 测电压 → 换算电流
    r_load = 100000 * 1000  # 100KΩ 因为电压测量是mV为单位，这里还需要再×1000
    v_a = instr.dmm_measure_single_voltage()
    i_a = v_a / r_load

    instr.swd_modify_register(RW_REF_ITRM, default_ref_itrm + 1, 5, 8)
    time.sleep(0.5)
    v_b = instr.dmm_measure_single_voltage()
    i_b = v_b / r_load

    if(i_b - i_a) > 1e-12:
        trim_target = round((typ_val / 1000000 - i_a) / (i_b - i_a)) + default_ref_itrm
        trim_target = max(0, min(15, trim_target))
    else:
        trim_target = default_ref_itrm

    best_trim = trim_target

    min_err = float('inf')

    range_itrm = range(trim_target - 5, trim_target + 5)
    for test_trim in range_itrm:
        if test_trim < 0 or test_trim > 15:
            continue
        instr.swd_modify_register(RW_REF_ITRM, test_trim, 5, 8)
        time.sleep(0.5)
        test_vol = instr.dmm_measure_single_voltage()
        test_cur = test_vol / r_load
        test_err = abs(test_cur - (typ_val / 1000000))
        print(f"测试修调值={test_trim}, 测试电流={test_cur}, 与目标周期误差={test_err}A")

        if test_err < min_err:
            best_trim = test_trim
            min_err = test_err

    instr.swd_modify_register(RW_REF_ITRM, best_trim, 5, 8)
    time.sleep(0.5)
    global_use["RW_REF_ITRM"] = RW_REF_ITRM
    global_use["REF_ITRM_BEST"] = best_trim

    best_vol = instr.dmm_measure_single_voltage()
    best_cur = best_vol / r_load
    passed = (best_cur >= min_val and best_cur <= max_val)
    save_trim_to_excel(test_item_name, "RW_REF_ITRM", RW_REF_ITRM, best_trim, passed)
    save_item_result(test_item_name, passed, round(best_cur, 3))
    if passed:
        print("REF_ITRM Test pass")
    else:
        print("REF_ITRM Test failed")
        print(f"修调范围为: {min_val}~{max_val} {unit}, 实际测量值: {best_cur:.3f} {unit}")
    instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")

@test_case
def SLP_REF_VTRM():
    test_item_name = "vslp"
    row = df_test_data[df_test_data["测试项"] == test_item_name]
    if row.empty:
        print(f"未找到测试项: {test_item_name}")
        return
    try:
        min_val = float(row["Min."].values[0])
        typ_val = float(row["Typ."].values[0])
        max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
        if unit == "V" :
            min_val = min_val * 1000.0
            typ_val = typ_val * 1000.0
            max_val = max_val * 1000.0
    except (IndexError, ValueError) as e:
        print(f"❌ 测试参数读取失败: {e}")
        return
    try:
        PA_ANA_EN = test_item.get_32bit_reg_address("PA_ANA_EN", df_register)
        RW_SLP_REF_EN = test_item.get_32bit_reg_address("RW_SLP_REF_EN", df_register)

        RW_ATP_MUX = test_item.get_32bit_reg_address("RW_ATP_MUX", df_register)
        RW_ATP_MUX_EN = test_item.get_32bit_reg_address("RW_ATP_MUX_EN", df_register)

        ANAC_TRM1 = test_item.get_32bit_reg_address("ANAC_TRM1", df_register)
        RW_SLP_REF_VTRM = test_item.get_32bit_reg_address("RW_SLP_REF_VTRM", df_register)
    except Exception as e:
        print(f"❌ 寄存器地址获取失败: {e}")
        return

    # 测试前置: 配置 PA5/PA6 复用 ALT_FUNC_14 (开 GPIO 时钟 + 输出方向 + AF14)
    init_pa5_pa6_altfunc14()

    #使能相关模块
    instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
    instr.swd_modify_register(RW_SLP_REF_EN, 0x01, 4, 4)
    time.sleep(0.1)

    #确认 ATP_MUX 选中 SLP_REF
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 6, 6)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 7, 7)
    instr.swd_modify_register(RW_ATP_MUX, 0x01, 0, 4)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)

    trim = instr.swd_read_register(ANAC_TRM1)
    if trim is None:
        print("❌ 读取 ANAC_TRM1 失败 (SWD 异常), 跳过本次修调")
        return
    cur = (trim >> 14) & 0x0F

    v_a = instr.dmm_measure_single_voltage()
    passed = (v_a >= min_val and v_a <= max_val)
    if passed :
        print(f"SLP_VREF测试通过,修调范围为: {min_val}~{max_val} mV, 实际测量值: {v_a:.3f} mV, 修调值为：{cur}, 无需修调")
        global_use["RW_SLP_REF_VTRM"] = RW_SLP_REF_VTRM
        global_use["SLP_REF_VTRM_BEST"] = cur
        save_trim_to_excel(test_item_name, "RW_SLP_REF_VTRM", RW_SLP_REF_VTRM, cur, passed)
        save_item_result(test_item_name, passed, round(v_a, 3))
        instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
        time.sleep(0.5)
        return
    else:
        print(f"SLP_VREF测试不通过,修调范围为: {min_val}~{max_val} mV, 实际测量值: {v_a:.3f} mV, 修调值为：{cur}, 需要进行修调")
    
    new = min(15, cur + 1)
    instr.swd_modify_register(RW_SLP_REF_VTRM, new, 14, 17)
    time.sleep(0.5)
    v_b = instr.dmm_measure_single_voltage()

    step = v_b - v_a
    if abs(step) > 0.0001:
        trim_target = round((typ_val - v_a) / step) + cur
        trim_target = max(0, min(15, trim_target))
    else:
        trim_target = cur
    
    best_trim = trim_target

    min_err = float('inf')

    #复测，在计算出的修调范围内获取最佳修调值
    range_vtrm = range(trim_target - 5, trim_target + 5)
    for test_trim in range_vtrm:
        if test_trim < 0 or test_trim > 15:
            continue
        instr.swd_modify_register(RW_SLP_REF_VTRM, test_trim, 14, 17)
        time.sleep(0.5)
        test_vol = instr.dmm_measure_single_voltage()
        test_err = abs(test_vol - typ_val)
        print(f"测试修调值={test_trim}, 测试电压={test_vol}, 与目标周期误差={test_err}mV")

        if test_err < min_err:
            best_trim = test_trim
            min_err = test_err

    instr.swd_modify_register(RW_SLP_REF_VTRM, best_trim, 14, 17)
    time.sleep(0.5)
    global_use["RW_SLP_REF_VTRM"] = RW_SLP_REF_VTRM
    global_use["SLP_REF_VTRM_BEST"] = best_trim

    best_vol = (float)(instr.dmm_measure_single_voltage())
    passed = (best_vol >= min_val and best_vol <= max_val)
    save_trim_to_excel(test_item_name, "RW_SLP_REF_VTRM", RW_SLP_REF_VTRM, best_trim, passed)
    save_item_result(test_item_name, passed, round(best_vol, 3))
    if passed:
        print("SLP_REF_VTRM Test pass")
        print(f"修调范围为: {min_val}~{max_val} mV, 实际测量值: {best_vol:.3f} mV, 修调值为：{best_trim}")
    else:
        print("SLP_REF_VTRM Test failed")
        print(f"修调范围为: {min_val}~{max_val} mV, 实际测量值: {best_vol:.3f} mV, 修调值为：{best_trim}")
    instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
    time.sleep(1)


@test_case
def BG_TTRIM():
    test_item_name = "vbg"
    row = df_test_data[df_test_data["测试项"] == test_item_name]
    if row.empty:
        print(f"未找到测试项: {test_item_name}")
        return
    try:
        min_val = float(row["Min."].values[0])
        typ_val = float(row["Typ."].values[0])
        max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
        if unit == "V" :
            min_val = min_val * 1000.0
            typ_val = typ_val * 1000.0
            max_val = max_val * 1000.0
    except (IndexError, ValueError) as e:
        print(f"❌ 测试参数读取失败: {e}")
        return
    try:

        RW_ATP_MUX = test_item.get_32bit_reg_address("RW_ATP_MUX", df_register)
        RW_ATP_MUX_EN = test_item.get_32bit_reg_address("RW_ATP_MUX_EN", df_register)

        ANAC_TRM2 = test_item.get_32bit_reg_address("ANAC_TRM2", df_register)
        RW_BG_TTRM = test_item.get_32bit_reg_address("RW_BG_TTRM", df_register)
        PA_ANA_EN = test_item.get_32bit_reg_address("PA_ANA_EN", df_register)
        RW_ANA_REF_EN = test_item.get_32bit_reg_address("RW_ANA_REF_EN", df_register)
    except Exception as e:
        print(f"❌ 寄存器地址获取失败: {e}")
        return
    1
    print("VBG 修调 — BG_TTRM")

    # 测试前置: 配置 PA5/PA6 复用 ALT_FUNC_14 (开 GPIO 时钟 + 输出方向 + AF14)
    init_pa5_pa6_altfunc14()
    time.sleep(0.1)

    #使能相关模块 (模拟电源域 + 模拟参考/带隙核心)
    instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
    instr.swd_modify_register(RW_ANA_REF_EN, 0x01, 24, 24)
    time.sleep(0.1)

    #确认 ATP_MUX 选中 VBG
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 6, 6)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 7, 7)


    #读当前 BG_TTRM 默认值(30)
    trim = instr.swd_read_register(ANAC_TRM2)
    if trim is None:
        print("❌ 读取 ANAC_TRM2 失败 (SWD 异常), 跳过本次修调")
        return
    cur = (trim >> 25) & 0x3F

    instr.swd_modify_register(RW_ATP_MUX, 0x02, 0, 4)
    time.sleep(2.5)
    bga_vol = instr.dmm_measure_single_voltage()
    time.sleep(0.5)

    instr.swd_modify_register(RW_ATP_MUX, 0x03, 0, 4)
    time.sleep(0.5)

    bga_vol = instr.dmm_measure_single_voltage()
    passed = (bga_vol >= min_val and bga_vol <= max_val)
    if passed:
        print(f"VBG测试通过,修调范围为: {min_val}~{max_val} mV, 实际测量值: {bga_vol:.3f} mV, 修调值为：{cur}, 无需修调")
        global_use["RW_BG_TTRM"] = RW_BG_TTRM
        global_use["BG_TTRM_BEST"] = cur
        save_trim_to_excel(test_item_name, "RW_BG_TTRM", RW_BG_TTRM, cur, passed)
        save_item_result(test_item_name, passed, round(bga_vol, 3))
        instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
        time.sleep(0.5)
        return
    else:
        print(f"VBG测试不通过,修调范围为: {min_val}~{max_val} mV, 实际测量值: {bga_vol:.3f} mV, 修调值为：{cur}, 需要进行修调")

    new = min(63, cur + 1)
    instr.swd_modify_register(RW_BG_TTRM, new, 25, 30)
    time.sleep(0.5)
    bgb_vol = instr.dmm_measure_single_voltage()

    step = bgb_vol - bga_vol
    if abs(step) > 1e-12:
        trim_target = round((typ_val - bga_vol) / step) + cur
        trim_target = max(0, min(63, trim_target))
    else:
        trim_target = cur

    best_trim = trim_target

    min_error = float('inf')

    #复测，在计算出的修调值范围内获取最佳修调值
    range_ttrm = range(trim_target - 5, trim_target + 5)
    for test_trim in range_ttrm:
        if test_trim < 0 or test_trim > 63:
            continue
        instr.swd_modify_register(RW_BG_TTRM, test_trim, 25, 30)
        time.sleep(0.5)
        test_vol = instr.dmm_measure_single_voltage()
        test_err = abs(test_vol - typ_val)

        print(f"测试修调值={test_trim}, 输出电压={test_vol}mV, 与目标周期误差={test_err}")

        if test_err < min_error:
            min_error = test_err
            best_trim = test_trim
    #写入修调值
    instr.swd_modify_register(RW_BG_TTRM, best_trim, 25, 30)
    time.sleep(0.5)
    global_use["RW_BG_TTRM"] = RW_BG_TTRM
    global_use["BG_TTRM_BEST"] = best_trim
    best_vol = instr.dmm_measure_single_voltage()

    passed = (best_vol >= min_val and best_vol <= max_val)
    save_trim_to_excel(test_item_name, "RW_BG_TTRM", RW_BG_TTRM, best_trim, passed)
    save_item_result(test_item_name, passed, round(best_vol, 3))
    if passed:
        print("BG_TTRIM Test pass")
        print(f"修调范围为: {min_val}~{max_val} {unit}, 实际测量值: {best_vol:.3f} mV, 修调值为：{best_trim}")
    else:
        print("BG_TTRIM Test failed")
        print(f"修调范围为: {min_val}~{max_val} {unit}, 实际测量值: {best_vol:.3f} mV, 修调值为：{best_trim}")
    instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
    time.sleep(1)

def save_bg_new_ttrim_excel(trim_list, temp_list, data, slopes, chip_id=None, ranges=None):
    """保存 BG_NEW_TTRIM 温度扫描数据: 拟合斜率与极差(trim值/斜率/极差三列)放在工作表最前面(第1~3列),
    其后所有trim值汇总在同一个数据工作表中, 每个trim值占用两列(温度/BG输出电压), 列间隔一列;
    另建一个工作表放置每个trim值对应的 温度-电压 曲线图(叠加在同一张散点图中)。

    data: dict[trim] -> dict[temp] -> voltage(mV)
    slopes: dict[trim] -> slope(mV/℃) 或 None
    ranges: dict[trim] -> (该trim值下所有温度点BG输出电压的 最大值-最小值, mV) 或 None
    """
    from openpyxl.chart import ScatterChart, Reference, Series
    from openpyxl.chart.marker import Marker
    from openpyxl.utils import get_column_letter

    ranges = ranges or {}

    # 统一保存到公共结果目录下的 BG_NEW_TRIM 子文件夹, 与 ADC_RMSE_TRIM 分开存放
    save_dir = os.path.join(_CHIP_TEST_RESULTS_DIR, "BG_NEW_TRIM")
    os.makedirs(save_dir, exist_ok=True)
    filename = f"BG_NEW_TTRIM_results_A{chip_id}.xlsx" if chip_id else "BG_NEW_TTRIM_results.xlsx"
    excel_path = os.path.join(save_dir, filename)

    wb = openpyxl.Workbook()
    ws_data = wb.active
    ws_data.title = "BG_NEW_TTRIM_数据"

    n_rows = len(temp_list)
    data_first_row = 2
    data_last_row = data_first_row + n_rows - 1

    # 所有trim值的拟合斜率与极差放在工作表最前面三列(trim值, 拟合斜率, 电压极差), 一个trim一行, 便于统一查看对比
    slope_col_trim, slope_col_value, range_col_value = 1, 2, 3
    ws_data.cell(row=1, column=slope_col_trim, value="trim值")
    ws_data.cell(row=1, column=slope_col_value, value="拟合斜率(mV/℃)")
    ws_data.cell(row=1, column=range_col_value, value="电压极差(mV) = 最大值-最小值")
    for i, trim in enumerate(trim_list):
        slope = slopes.get(trim)
        vrange = ranges.get(trim)
        row = data_first_row + i
        ws_data.cell(row=row, column=slope_col_trim, value=trim)
        ws_data.cell(row=row, column=slope_col_value,
                      value=round(slope, 6) if slope is not None else "N/A")
        ws_data.cell(row=row, column=range_col_value,
                      value=round(vrange, 6) if vrange is not None else "N/A")
    ws_data.column_dimensions[get_column_letter(slope_col_trim)].width = 12
    ws_data.column_dimensions[get_column_letter(slope_col_value)].width = 20
    ws_data.column_dimensions[get_column_letter(range_col_value)].width = 26

    # 每个trim值占2列(温度, 电压), 中间空1列, 共3列一组, 紧跟在斜率/极差三列之后再空1列
    trim_col_start = {}
    trim_data_first_col = range_col_value + 2
    for i, trim in enumerate(trim_list):
        col_temp = trim_data_first_col + i * 3
        trim_col_start[trim] = col_temp
        ws_data.cell(row=1, column=col_temp, value=f"trim{trim}-温度(℃)")
        ws_data.cell(row=1, column=col_temp + 1, value=f"trim{trim}-BG输出电压(mV)")

        for r, temp in enumerate(temp_list):
            vol = data.get(trim, {}).get(temp)
            ws_data.cell(row=data_first_row + r, column=col_temp, value=temp)
            ws_data.cell(row=data_first_row + r, column=col_temp + 1, value=vol)

        for col in (col_temp, col_temp + 1):
            ws_data.column_dimensions[get_column_letter(col)].width = 20

    ws_data.freeze_panes = "A2"

    # 曲线图工作表: 每个trim值一条 温度-电压 曲线, 叠加在同一张散点图中
    ws_chart = wb.create_sheet("BG_NEW_TTRIM_曲线图")

    chart = ScatterChart()
    chart.title = "各trim值 温度-电压 曲线"
    chart.x_axis.title = "温度(℃)"
    chart.y_axis.title = "BG输出电压(mV)"
    chart.x_axis.axPos = "b"
    chart.y_axis.axPos = "l"
    chart.x_axis.delete = False
    chart.y_axis.delete = False
    chart.style = 13
    chart.width, chart.height = 40, 24

    for trim in trim_list:
        col_temp = trim_col_start[trim]
        x_ref = Reference(ws_data, min_col=col_temp, min_row=data_first_row, max_row=data_last_row)
        y_ref = Reference(ws_data, min_col=col_temp + 1, min_row=data_first_row, max_row=data_last_row)
        series = Series(y_ref, x_ref, title=f"trim{trim}")
        series.marker = Marker(symbol="circle", size=4)
        series.graphicalProperties.line.noFill = False
        chart.series.append(series)

    ws_chart.add_chart(chart, "B2")

    try:
        wb.save(excel_path)
        print(f"  BG_NEW_TTRIM 数据已保存: {excel_path}")
    except Exception as e:
        print(f"  ⚠️ 保存 BG_NEW_TTRIM Excel 失败: {e}")


def _linear_fit_slope(xs, ys):
    """最小二乘法计算斜率, 点数不足2个或x无差异时返回None"""
    pts = [(x, y) for x, y in zip(xs, ys) if y is not None]
    if len(pts) < 2:
        return None
    x_bar = sum(p[0] for p in pts) / len(pts)
    y_bar = sum(p[1] for p in pts) / len(pts)
    denom = sum((p[0] - x_bar) ** 2 for p in pts)
    if abs(denom) < 1e-12:
        return None
    numer = sum((p[0] - x_bar) * (p[1] - y_bar) for p in pts)
    return numer / denom


@test_case
def BG_NEW_TRIM():
    """BG_TTRM 全trim值 温度-电压扫描:
    温度外层(用户设置温度腔, 每个温度点只需换温一次), trim内层(63降序遍历到40)
    每个温度点测出所有trim值对应的BG输出电压, 最终按trim值整理为独立工作表,
    并计算该trim值下 温度-电压 的线性拟合斜率(mV/℃)
    """
    try:
        RW_ATP_MUX = test_item.get_32bit_reg_address("RW_ATP_MUX", df_register)
        RW_ATP_MUX_EN = test_item.get_32bit_reg_address("RW_ATP_MUX_EN", df_register)
        RW_BG_TTRM = test_item.get_32bit_reg_address("RW_BG_TTRM", df_register)
        PA_ANA_EN = test_item.get_32bit_reg_address("PA_ANA_EN", df_register)
        RW_ANA_REF_EN = test_item.get_32bit_reg_address("RW_ANA_REF_EN", df_register)
    except Exception as e:
        print(f"❌ 寄存器地址获取失败: {e}")
        return

    try:
        temp_start = float(input("请输入温度起始值(℃): ").strip())
        temp_stop = float(input("请输入温度终止值(℃): ").strip())
        temp_step = float(input("请输入温度步进值(℃): ").strip())
        if temp_step <= 0 or temp_stop < temp_start:
            print("❌ 温度参数不合法(步进需>0, 终止值需≥起始值)")
            return
    except ValueError:
        print("❌ 温度参数输入有误")
        return

    chip_id = input("请输入本次测试的芯片编号 (如 11，对应A11): ").strip() or None

    temp_list = list(frange(temp_start, temp_stop + temp_step / 2.0, temp_step))
    trim_list = list(range(63, 39, -1))

    print(f"温度点: {temp_list}")
    print(f"trim值(63→0): 共{len(trim_list)}个")

    data = {trim: {} for trim in trim_list}

    for temp in temp_list:
        print(f"\n请将温度设置为 {temp}℃, 待温度稳定后按下enter继续......")
        input()

        # 每个温度点开始时重新初始化寄存器配置:
        # 若某温度点导致芯片重启, 寄存器配置会丢失, 需在每个温度点重新配置,
        # 这样即使某个温度点异常也只影响该温度点, 不会影响其他温度点
        init_pa5_pa6_altfunc14()
        time.sleep(0.1)

        #使能相关模块 (模拟电源域 + 模拟参考/带隙核心)
        instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
        instr.swd_modify_register(RW_ANA_REF_EN, 0x01, 24, 24)
        time.sleep(0.1)

        #确认 ATP_MUX 选中 VBG
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 6, 6)
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 7, 7)

        instr.swd_modify_register(RW_ATP_MUX, 0x02, 0, 4)
        print("延时2s")
        time.sleep(3)
        bga_vol = instr.dmm_measure_single_voltage()
        print("延时2s")
        time.sleep(2)
        instr.swd_modify_register(RW_ATP_MUX, 0x03, 0, 4)
        time.sleep(0.5)

        for trim in trim_list:
            instr.swd_modify_register(RW_BG_TTRM, trim, 25, 30)
            time.sleep(0.8)
            vol = instr.dmm_measure_single_voltage()
            data[trim][temp] = vol
            print(f"  温度={temp}℃, trim={trim}, BG输出电压={vol}mV")

            # 读取一次寄存器, 校验配置是否仍在(若芯片重启则回读值与写入值不符)
            readback = instr.swd_read_register(RW_BG_TTRM)
            if readback is None:
                print(f"  ⚠️ SWD读取RW_BG_TTRM失败, 可能芯片已重启, 该温度点数据可能无效")
            else:
                readback_trim = (readback >> 25) & 0x3F
                if readback_trim != trim:
                    print(f"  ⚠️ 回读trim值({readback_trim})与写入值({trim})不一致, 芯片可能已重启, 该温度点数据可能无效")

    slopes = {}
    ranges = {}
    for trim in trim_list:
        xs = temp_list
        ys = [data[trim].get(t) for t in temp_list]
        slopes[trim] = _linear_fit_slope(xs, ys)
        valid_ys = [y for y in ys if y is not None]
        ranges[trim] = (max(valid_ys) - min(valid_ys)) if valid_ys else None

    save_bg_new_ttrim_excel(trim_list, temp_list, data, slopes, chip_id=chip_id, ranges=ranges)

    instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
    time.sleep(1)


@test_case
def LDO12_VTRM():
    test_item_name = "VDD12(normal)"
    row = df_test_data[df_test_data["测试项"] == test_item_name]
    if row.empty:
        print(f"未找到测试项: {test_item_name}")
        return
    try:
        min_val = float(row["Min."].values[0])
        typ_val = float(row["Typ."].values[0])
        max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
        if unit == "V" :
            min_val = min_val * 1000.0
            typ_val = typ_val * 1000.0
            max_val = max_val * 1000.0
    except (IndexError, ValueError) as e:
        print(f"❌ 测试参数读取失败: {e}")
        return
    try:
        PA_ANA_EN = test_item.get_32bit_reg_address("PA_ANA_EN", df_register)
        RW_ANA_LDO12_EN = test_item.get_32bit_reg_address("RW_ANA_LDO12_EN", df_register)

        RW_ATP_MUX = test_item.get_32bit_reg_address("RW_ATP_MUX", df_register)
        RW_ATP_MUX_EN = test_item.get_32bit_reg_address("RW_ATP_MUX_EN", df_register)

        ANAC_TRM1 = test_item.get_32bit_reg_address("ANAC_TRM1", df_register)
        RW_LDO12_VTRM = test_item.get_32bit_reg_address("RW_LDO12_VTRM", df_register)
    except Exception as e:
        print(f"❌ 寄存器地址获取失败: {e}")
        return

    # 测试前置: 配置 PA5/PA6 复用 ALT_FUNC_14 (开 GPIO 时钟 + 输出方向 + AF14)
    init_pa5_pa6_altfunc14()

    #使能相关模块
    instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
    instr.swd_modify_register(RW_ANA_LDO12_EN, 0x01, 25, 25)
    time.sleep(0.1)

    #确认 ATP_MUX 选中 VREF
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 6, 6)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 7, 7)
    instr.swd_modify_register(RW_ATP_MUX, 0x0D, 0, 4)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)
    time.sleep(0.1)

    trim = instr.swd_read_register(ANAC_TRM1)
    if trim is None:
        print("❌ 读取 ANAC_TRM1 失败 (SWD 异常), 跳过本次修调")
        return
    cur = (trim >> 9) & 0x07

    v_a = instr.dmm_measure_single_voltage()

    passed = (v_a >= min_val and v_a <= max_val)
    if passed :
        print(f"LDO12_VTRM测试通过,修调范围为: {min_val}~{max_val} mV, 实际测量值: {v_a:.3f} mV, 修调值为：{cur}, 无需修调")
        global_use["RW_LDO12_VTRM"] = RW_LDO12_VTRM
        global_use["LDO12_VTRM_BEST"] = cur
        save_trim_to_excel(test_item_name, "RW_LDO12_VTRM", RW_LDO12_VTRM, cur, passed)
        save_item_result(test_item_name, passed, round(v_a, 3))
        instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
        time.sleep(0.5)
        return
    else:
        print(f"LDO12_VTRM测试不通过,修调范围为: {min_val}~{max_val} mV, 实际测量值: {v_a:.3f} mV, 修调值为：{cur}, 需要进行修调")

    new = min(7, cur + 1)
    instr.swd_modify_register(RW_LDO12_VTRM, new, 9, 11)
    time.sleep(0.5)
    v_b = instr.dmm_measure_single_voltage()

    step = v_b - v_a
    if abs(step) > 0.00001:
        trim_target = round((typ_val - v_a) / step) + cur
        trim_target = max(0, min(7, trim_target))
    else:
        trim_target = cur

    best_trim = trim_target

    min_err = float('inf')

    #复测，在计算出的修调范围内获取最佳修调值
    range_vtrm = range(trim_target - 3, trim_target + 3)
    for test_trim in range_vtrm:
        if test_trim < 0 or test_trim > 7:
            continue
        instr.swd_modify_register(RW_LDO12_VTRM, test_trim, 9, 11)
        time.sleep(0.5)
        test_vol = instr.dmm_measure_single_voltage()
        test_err = abs(test_vol - typ_val)
        print(f"测试修调值={test_trim}, 测试电压={test_vol}, 与目标周期误差={test_err}mV")

        if test_err < min_err:
            best_trim = test_trim
            min_err = test_err

    instr.swd_modify_register(RW_LDO12_VTRM, best_trim, 9, 11)
    time.sleep(0.5)
    global_use["RW_LDO12_VTRM"] = RW_LDO12_VTRM
    global_use["LDO12_VTRM_BEST"] = best_trim

    best_vol = (float)(instr.dmm_measure_single_voltage())
    passed = (best_vol >= min_val and best_vol <= max_val)
    save_trim_to_excel(test_item_name, "RW_LDO12_VTRM", RW_LDO12_VTRM, best_trim, passed)
    save_item_result(test_item_name, passed, round(best_vol, 3))
    if passed:
        print("LDO12_VTRM Test pass")
        print(f"修调范围为: {min_val}~{max_val} mV, 实际测量值: {best_vol:.3f} mV, 修调值为：{best_trim}")
    else:
        print("LDO12_VTRM Test failed")
        print(f"修调范围为: {min_val}~{max_val} mV, 实际测量值: {best_vol:.3f} mV, 修调值为：{best_trim}")
    instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
    time.sleep(1)


@test_case
def SLP12_VTRM():
    test_item_name = "VDD12(sleep)"
    row = df_test_data[df_test_data["测试项"] == test_item_name]
    if row.empty:
        print(f"未找到测试项: {test_item_name}")
        return
    try:
        min_val = float(row["Min."].values[0])
        typ_val = float(row["Typ."].values[0])
        max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
        if unit == "V" :
            min_val = min_val * 1000.0
            typ_val = typ_val * 1000.0
            max_val = max_val * 1000.0
    except (IndexError, ValueError) as e:
        print(f"❌ 测试参数读取失败: {e}")
        return
    try:
        PA_ANA_EN = test_item.get_32bit_reg_address("PA_ANA_EN", df_register)
        RW_ANA_LDO12_EN = test_item.get_32bit_reg_address("RW_ANA_LDO12_EN", df_register)
        RW_ATP_MUX = test_item.get_32bit_reg_address("RW_ATP_MUX", df_register)
        RW_ATP_MUX_EN = test_item.get_32bit_reg_address("RW_ATP_MUX_EN", df_register)
        ANAC_TRM1 = test_item.get_32bit_reg_address("ANAC_TRM1", df_register)
        RW_SLP12_VTRM = test_item.get_32bit_reg_address("RW_SLP12_VTRM", df_register)
    except Exception as e:
        print(f"❌ 寄存器地址获取失败: {e}")
        return
    

    # 测试前置: 配置 PA5/PA6 复用 ALT_FUNC_14 (开 GPIO 时钟 + 输出方向 + AF14)
    init_pa5_pa6_altfunc14()

    #使能相关模块
    instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
    instr.swd_modify_register(RW_ANA_LDO12_EN, 0x0, 25, 25)
    time.sleep(0.1)

    #确认 ATP_MUX 选中 VREF
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 6, 6)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 7, 7)
    instr.swd_modify_register(RW_ATP_MUX, 0b01101, 0, 4)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)

    trim = instr.swd_read_register(ANAC_TRM1)
    if trim is None:
        print("❌ 读取 ANAC_TRM1 失败 (SWD 异常), 跳过本次修调")
        return
    cur = (trim >> 12) & 0x3

    v_a = instr.dmm_measure_single_voltage()

    passed = (v_a >= min_val and v_a <= max_val)
    if passed :
        print(f"SLP12_VTRM测试通过,修调范围为: {min_val}~{max_val} mV, 实际测量值: {v_a:.3f} mV, 修调值为：{cur}, 无需修调")
        global_use["RW_SLP12_VTRM"] = RW_SLP12_VTRM
        global_use["SLP12_VTRM_BEST"] = cur
        save_trim_to_excel(test_item_name, "RW_SLP12_VTRM", RW_SLP12_VTRM, cur, passed)
        save_item_result(test_item_name, passed, round(v_a, 3))
        instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
        time.sleep(0.5)
        return
    else:
        print(f"SLP12_VTRM测试不通过,修调范围为: {min_val}~{max_val} mV, 实际测量值: {v_a:.3f} mV, 修调值为：{cur}, 需要进行修调")

    # ⚠️ SLP12_VTRM 修调值严禁写入 1：
    #    实测写 1 会导致芯片锁死（需断电才能恢复），因此以下所有写入修调值的地方都跳过 1。
    new = min(3, cur + 1)
    if new == 1:
        new = 2      # 探测值若算出为 1 则跳过，改用 2 探测
    instr.swd_modify_register(RW_SLP12_VTRM, new, 12, 13)
    time.sleep(0.5)
    v_b = instr.dmm_measure_single_voltage()

    step = v_b - v_a
    if abs(step) > 0.00001 and new != cur:
        # 若探测跨过危险值1(cur=0→new=2)，按实际跨越的 LSB 数归一化每步电压
        step_per_lsb = step / (new - cur)
        trim_target = round((typ_val - v_a) / step_per_lsb) + cur
        trim_target = max(0, min(3, trim_target))
    else:
        trim_target = cur

    best_trim = trim_target

    min_err = float('inf')

    #复测，在计算出的修调范围内获取最佳修调值
    range_vtrm = range(trim_target - 2, trim_target + 2)
    for test_trim in range_vtrm:
        if test_trim < 0 or test_trim > 3 or test_trim == 1:   # 1 会锁死芯片，禁止写入
            continue
        instr.swd_modify_register(RW_SLP12_VTRM, test_trim, 12, 13)
        time.sleep(0.5)
        test_vol = instr.dmm_measure_single_voltage()
        test_err = abs(test_vol - typ_val)
        print(f"测试修调值={test_trim}, 测试电压={test_vol}, 与目标周期误差={test_err}mV")

        if test_err < min_err:
            best_trim = test_trim
            min_err = test_err

    # 兜底：即便 best_trim 计算出为 1 也禁止写入（实测该值会锁死芯片，需断电恢复）
    if best_trim == 1:
        best_trim = 2
    instr.swd_modify_register(RW_SLP12_VTRM, best_trim, 12, 13)
    time.sleep(0.5)
    global_use["RW_SLP12_VTRM"] = RW_SLP12_VTRM
    global_use["SLP12_VTRM_BEST"] = best_trim

    best_vol = (float)(instr.dmm_measure_single_voltage())
    passed = (best_vol >= min_val and best_vol <= max_val)
    save_trim_to_excel(test_item_name, "RW_SLP12_VTRM", RW_SLP12_VTRM, best_trim, passed)
    save_item_result(test_item_name, passed, round(best_vol, 3))
    if passed:
        print("SLP12_VTRM Test pass")
        print(f"修调范围为: {min_val}~{max_val} mV, 实际测量值: {best_vol:.3f} mV, 修调值为：{best_trim}")
    else:
        print("SLP12_VTRM Test failed")
        print(f"修调范围为: {min_val}~{max_val} mV, 实际测量值: {best_vol:.3f} mV, 修调值为：{best_trim}")
    instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
    time.sleep(1)


@test_case
def ADC_VREFH_VTRM():
    test_item_name = "vrefH"
    row = df_test_data[df_test_data["测试项"] == test_item_name]
    if row.empty:
        print(f"未找到测试项: {test_item_name}")
        return
    try:
        min_val = float(row["Min."].values[0])
        typ_val = float(row["Typ."].values[0])
        max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
        if unit == "V" :
            min_val = min_val * 1000.0
            typ_val = typ_val * 1000.0
            max_val = max_val * 1000.0
    except (IndexError, ValueError) as e:
        print(f"❌ 测试参数读取失败: {e}")
        return
    try:
        PA_ANA_EN = test_item.get_32bit_reg_address("PA_ANA_EN", df_register)
        RW_ANA_VREFH_EN = test_item.get_32bit_reg_address("RW_ANA_VREFH_EN", df_register)
        PA5_AFS14_EN = test_item.get_32bit_reg_address("PA5_AFS14_EN", df_register)
        RW_ADC_VREFH_ATP = test_item.get_32bit_reg_address("RW_ADC_VREFH_ATP", df_register)
        RW_ADC_VREFH = test_item.get_32bit_reg_address("RW_ADC_VREFH", df_register)

        RW_ATP_MUX = test_item.get_32bit_reg_address("RW_ATP_MUX", df_register)
        RW_ATP_MUX_EN = test_item.get_32bit_reg_address("RW_ATP_MUX_EN", df_register)

        ANAC_TRM1 = test_item.get_32bit_reg_address("ANAC_TRM1", df_register)
        RW_ADC_VREFH_VTRM = test_item.get_32bit_reg_address("RW_ADC_VREFH_VTRM", df_register)
        
    except Exception as e:
        print(f"❌ 寄存器地址获取失败: {e}")
        return


    # 测试前置: 配置 PA5/PA6 复用 ALT_FUNC_14 (开 GPIO 时钟 + 输出方向 + AF14)
    init_pa5_pa6_altfunc14()

    #使能相关模块
    instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
    instr.swd_modify_register(RW_ADC_VREFH_ATP, 0x01, 18, 18)
    time.sleep(0.1)
    

    #确认 ATP_MUX 选中 VREFH
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 6, 6)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 7, 7)
    instr.swd_modify_register(RW_ATP_MUX, 0x06, 0, 4)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)
    instr.swd_modify_register(RW_ADC_VREFH, 0x02, 13, 14)
    instr.swd_modify_register(RW_ADC_VREFH, 0x00, 16, 17)
    instr.swd_modify_register(RW_ANA_VREFH_EN, 0x01, 18, 18)
    time.sleep(0.1)

    trim = instr.swd_read_register(ANAC_TRM1)
    if trim is None:
        print("❌ 读取 ANAC_TRM1 失败 (SWD 异常), 跳过本次修调")
        return
    cur = (trim >> 18) & 0x7

    v_a = instr.dmm_measure_single_voltage()
    passed = (v_a >= min_val and v_a <= max_val)
    if passed :
        print(f"ADC_VREFH_VTRM测试通过,修调范围为: {min_val}~{max_val} mV, 实际测量值: {v_a:.3f} mV, 修调值为：{cur}, 无需修调")
        global_use["RW_ADC_VREFH_VTRM"] = RW_ADC_VREFH_VTRM
        global_use["ADC_VREFH_VTRM_BEST"] = cur
        save_trim_to_excel(test_item_name, "RW_ADC_VREFH_VTRM", RW_ADC_VREFH_VTRM, cur, passed)
        save_item_result(test_item_name, passed, round(v_a, 3))
        instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
        time.sleep(0.5)
        return
    else:
        print(f"ADC_VREFH_VTRM测试不通过,修调范围为: {min_val}~{max_val} mV, 实际测量值: {v_a:.3f} mV, 修调值为：{cur}, 需要进行修调")

    new = min(7, cur + 1)
    instr.swd_modify_register(RW_ADC_VREFH_VTRM, new, 18, 20)
    time.sleep(0.5)
    v_b = instr.dmm_measure_single_voltage()

    step = v_b - v_a
    if abs(step) > 0.000001:
        trim_target = round((typ_val - v_a) / step) + cur
        trim_target = max(0, min(7, trim_target))
    else:
        trim_target = cur

    best_trim = trim_target

    min_err = float('inf')

    #复测，在计算出的修调范围内获取最佳修调值
    range_vtrm = range(trim_target - 5, trim_target + 5)
    for test_trim in range_vtrm:
        if test_trim < 0 or test_trim > 7:
            continue
        instr.swd_modify_register(RW_ADC_VREFH_VTRM, test_trim, 18, 20)
        time.sleep(0.5)
        test_vol = instr.dmm_measure_single_voltage()
        test_err = abs(test_vol - typ_val)
        print(f"测试修调值={test_trim}, 测试电压={test_vol}, 与目标周期误差={test_err}mV")

        if test_err < min_err:
            best_trim = test_trim
            min_err = test_err

    instr.swd_modify_register(RW_ADC_VREFH_VTRM, best_trim, 18, 20)
    time.sleep(0.5)
    global_use["RW_ADC_VREFH_VTRM"] = RW_ADC_VREFH_VTRM
    global_use["ADC_VREFH_VTRM_BEST"] = best_trim

    best_vol = (float)(instr.dmm_measure_single_voltage())
    passed = (best_vol >= min_val and best_vol <= max_val)
    save_trim_to_excel(test_item_name, "RW_ADC_VREFH_VTRM", RW_ADC_VREFH_VTRM, best_trim, passed)
    save_item_result(test_item_name, passed, round(best_vol, 3))
    if passed:
        print("ADC_VREFH_VTRM Test pass")
        print(f"修调范围为: {min_val}~{max_val} mV, 实际测量值: {best_vol:.3f} mV, 修调值为：{best_trim}")
    else:
        print("ADC_VREFH_VTRM Test failed")
        print(f"修调范围为: {min_val}~{max_val} mV, 实际测量值: {best_vol:.3f} mV, 修调值为：{best_trim}")
    instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
    time.sleep(1)

def save_adc_rmse_excel(rmse_pts, vrefh_mv, rmse_lsb, dnl1=None, dnl2=None, inl=None, dnl_code=None,
                         fit_slope=None, fit_intercept=None, code_hat_list=None, residual_list=None,
                         chip_id=None, temp_c=None, wb=None):
    """将 ADC_RMSE_TRIM 单个温度点的采样数据写入共享工作簿(数据表 + 折线图，数据点不重叠)。
    每个温度点各占一个数据工作表和一个折线图工作表, 由调用方统一保存为一个 Excel 文件。
    """
    from openpyxl.chart import ScatterChart, Reference, Series
    from openpyxl.chart.marker import Marker
    from openpyxl.drawing.line import LineProperties
    from openpyxl.styles import Font, Alignment, PatternFill
    from openpyxl.utils import get_column_letter

    temp_tag = f"_{temp_c}C".replace(".", "p").replace("-", "n") if temp_c is not None else ""
    data_sheet_name = f"ADC_RMSE_数据{temp_tag}"
    chart_sheet_name = f"ADC_RMSE_折线图{temp_tag}"

    owns_wb = wb is None
    if owns_wb:
        wb = openpyxl.Workbook()
        wb.remove(wb.active)
    # 若共享工作簿中已存在同名工作表(即该温度点之前测过), 先移除再重建, 实现"覆盖更新"
    for existing_name in (data_sheet_name, chart_sheet_name):
        if existing_name in wb.sheetnames:
            del wb[existing_name]
    ws_data = wb.create_sheet(data_sheet_name)

    header = ["序号", "目标电压vin(mV)", "实测电压Vtrue(mV)", "ADC原始值Code(10次均值,0-4095)",
              "拟合预测值Code_hat", "残差e(Code-Code_hat)", "ADC转换电压(mV)"]
    ws_data.append(header)
    header_font = Font(bold=True)
    header_fill = PatternFill(start_color="DCE6F1", end_color="DCE6F1", fill_type="solid")
    for col in range(1, len(header) + 1):
        cell = ws_data.cell(row=1, column=col)
        cell.font = header_font
        cell.fill = header_fill
        cell.alignment = Alignment(horizontal="center")

    for idx, (vin, vol, conv, raw) in enumerate(rmse_pts, start=1):
        code_hat = code_hat_list[idx - 1] if code_hat_list is not None else None
        residual = residual_list[idx - 1] if residual_list is not None else None
        ws_data.append([idx, vin, round(vol, 4), round(raw, 2),
                         round(code_hat, 4) if code_hat is not None else "N/A",
                         round(residual, 4) if residual is not None else "N/A",
                         round(conv, 4)])

    # 列宽自适应，数据清晰可见
    col_widths = [8, 16, 18, 20, 18, 20, 18]
    for i, w in enumerate(col_widths, start=1):
        ws_data.column_dimensions[get_column_letter(i)].width = w
    ws_data.freeze_panes = "A2"

    n_rows = len(rmse_pts)
    data_first_row = 2
    data_last_row = data_first_row + n_rows - 1

    # 汇总信息写在数据表下方，避免与图表/数据重叠；说明文字改用合并单元格横向跨列显示 + 自动换行，
    # 并按文本长度动态计算行高，避免长说明被截断/遮挡
    SUMMARY_NAME_COL_WIDTH, SUMMARY_RESULT_COL_WIDTH, SUMMARY_NOTE_COL_WIDTH = 20, 26, 18
    SUMMARY_NOTE_COL_FIRST, SUMMARY_NOTE_COL_LAST = 3, 10   # C~J 合并区域
    temp_row_offset = 1 if temp_c is not None else 0
    summary_row = data_last_row + 2 + temp_row_offset
    if temp_c is not None:
        ws_data.cell(row=summary_row - 2, column=1, value=f"测试温度(℃): {temp_c}").font = header_font
    ws_data.cell(row=summary_row - 1, column=1, value="指标").font = header_font
    ws_data.cell(row=summary_row - 1, column=2, value="结果").font = header_font
    ws_data.cell(row=summary_row - 1, column=3, value="计算说明").font = header_font
    ws_data.row_dimensions[summary_row - 1].height = 20

    ws_data.column_dimensions["A"].width = max(ws_data.column_dimensions["A"].width or 0, SUMMARY_NAME_COL_WIDTH)
    ws_data.column_dimensions["B"].width = max(ws_data.column_dimensions["B"].width or 0, SUMMARY_RESULT_COL_WIDTH)
    for col in range(SUMMARY_NOTE_COL_FIRST, SUMMARY_NOTE_COL_LAST + 1):
        ws_data.column_dimensions[get_column_letter(col)].width = SUMMARY_NOTE_COL_WIDTH

    merged_note_width_chars = SUMMARY_NOTE_COL_WIDTH * (SUMMARY_NOTE_COL_LAST - SUMMARY_NOTE_COL_FIRST + 1)

    def _write_summary_row(row, name, result, note):
        ws_data.cell(row=row, column=1, value=name).alignment = Alignment(
            horizontal="left", vertical="center", wrap_text=True)
        ws_data.cell(row=row, column=2, value=result).alignment = Alignment(
            horizontal="left", vertical="center", wrap_text=True)
        ws_data.merge_cells(start_row=row, start_column=SUMMARY_NOTE_COL_FIRST,
                             end_row=row, end_column=SUMMARY_NOTE_COL_LAST)
        note_cell = ws_data.cell(row=row, column=SUMMARY_NOTE_COL_FIRST, value=note)
        note_cell.alignment = Alignment(horizontal="left", vertical="center", wrap_text=True)
        # 中文字符按2倍宽估算，按需要的换行数动态设置行高，避免文字被遮挡
        cn_chars = sum(1 for ch in note if ord(ch) > 0x2E80)
        text_width_units = cn_chars * 2 + (len(note) - cn_chars)
        n_lines = max(1, -(-text_width_units // merged_note_width_chars))
        ws_data.row_dimensions[row].height = max(30, n_lines * 15 + 8)

    _write_summary_row(summary_row, "VREFH(mV)", round(vrefh_mv, 4),
                        "VREFH = 旁路VDD33模式下DMM实测的基准电压(mV)")

    _write_summary_row(summary_row + 1, "线性拟合模型",
                        (f"Code = {fit_slope:.6f} * Vtrue + {fit_intercept:.3f}"
                         if fit_slope is not None and fit_intercept is not None else "N/A"),
                        "x=Vtrue实测电压(mV), y=Code ADC原始采样值；最小二乘线性拟合所得直线方程 Code=a+b*Vtrue")

    _write_summary_row(summary_row + 2, "RMSE(LSB)",
                        round(rmse_lsb, 4) if rmse_lsb is not None else "N/A",
                        "以实测电压Vtrue(mV)为x、ADC原始值Code为y做最小二乘线性拟合 Code_hat=b*Vtrue+a；"
                        "残差e_i=Code_i-Code_hat_i；RMSE=sqrt(mean(e_i^2))，单位LSB")

    _write_summary_row(summary_row + 3, "DNL基准点(ADC原始值)",
                        dnl_code if dnl_code is not None else "N/A",
                        "在全部采样点的ADC原始值中，取与2048绝对差最小的那个原始值，记为x")

    _write_summary_row(summary_row + 4, "DNL1",
                        round(dnl1, 4) if dnl1 is not None else "N/A",
                        "若x有多个连续重复采样点，取第一次出现的位置；"
                        "DNL1=(x-上一个ADC原始值)/(x对应实测电压-上一个ADC原始值对应实测电压)")

    _write_summary_row(summary_row + 5, "DNL2",
                        round(dnl2, 4) if dnl2 is not None else "N/A",
                        "若x有多个连续重复采样点，取最后一次出现的位置；"
                        "DNL2=(下一个ADC原始值-x)/(下一个ADC原始值对应实测电压-x对应实测电压)")

    _write_summary_row(summary_row + 6, "INL(mV)",
                        round(inl, 4) if inl is not None else "N/A",
                        "取x第一次出现位置的采样点；INL(mV)=该点ADC转换电压(mV)-该点实测电压(mV)")

    # x轴统一用实测电压Vtrue(mV)：实测数据以散点(仅标记点)展示，线性拟合以直线(无标记)叠加对比
    vtrue_ref = Reference(ws_data, min_col=3, min_row=data_first_row, max_row=data_last_row)   # 实测电压Vtrue
    code_ref = Reference(ws_data, min_col=4, min_row=data_first_row, max_row=data_last_row)    # ADC原始值Code(实测点)
    code_hat_ref = Reference(ws_data, min_col=5, min_row=data_first_row, max_row=data_last_row)  # 拟合预测值Code_hat

    ws_chart = wb.create_sheet(chart_sheet_name)

    chart = ScatterChart()
    chart.title = f"ADC 转移曲线: 实测点 vs 线性拟合{f' ({temp_c}℃)' if temp_c is not None else ''}"
    chart.x_axis.title = "实测电压Vtrue(mV)"
    chart.y_axis.title = "ADC原始值Code"
    chart.x_axis.axPos = "b"     # openpyxl 对 ScatterChart 的坐标轴位置默认值有误，需显式指定 x 轴在底部
    chart.y_axis.axPos = "l"
    chart.x_axis.delete = False
    chart.y_axis.delete = False
    chart.style = 13
    chart.width, chart.height = 40, 24   # cm, 放大清晰显示，避免数据标签重叠

    series_meas = Series(code_ref, vtrue_ref, title="实测点(Code)")
    series_meas.marker = Marker(symbol="circle", size=5)
    series_meas.graphicalProperties.line.noFill = True   # 只显示点，不连线
    chart.series.append(series_meas)

    if code_hat_list is not None:
        series_fit = Series(code_hat_ref, vtrue_ref, title="线性拟合(Code_hat)")
        series_fit.marker = Marker(symbol="none")
        series_fit.graphicalProperties.line = LineProperties(w=20000, solidFill="C00000")  # 红色拟合线
        series_fit.smooth = False
        chart.series.append(series_fit)

    ws_chart.add_chart(chart, "B2")

    if owns_wb:
        save_dir = os.path.join(_CHIP_TEST_RESULTS_DIR, "ADC_RMSE_TRIM")
        os.makedirs(save_dir, exist_ok=True)
        filename = f"adc_rmse_results_A{chip_id}{temp_tag}.xlsx" if chip_id else f"adc_rmse_results{temp_tag}.xlsx"
        excel_path = os.path.join(save_dir, filename)
        try:
            wb.save(excel_path)
            print(f"  ADC_RMSE 数据与折线图已另存: {excel_path}")
        except Exception as e:
            print(f"  ⚠️ 保存 ADC_RMSE 独立 Excel 失败: {e}")


@test_case
def ADC_RMSE_TRIM(descending=False, adc_channel=18):
    """adc_rmse —— ADC 转移曲线 RMSE(均方根误差) 测试

    复用 ADC_CAL_TRIM 的寄存器初始化流程(旁路VDD33 + 通道)，
    在多个电压点上线性扫描采样，用 DMM 实测电压作为真值，
    ADC 转换电压作为测量值，计算整条转移曲线的 RMSE:
        RMSE = sqrt( mean( (转换电压 - 实测电压)^2 ) ) / LSB电压   [单位: LSB]

    支持多温度点测试: 启动时由用户输入若干温度点(如 "-40,25,85")，
    每个温度点各测一条完整的转移曲线(非温度范围扫描，需人工将芯片/温箱
    调至对应温度后按回车确认再继续)。同一芯片编号对应同一个 Excel 文件:
    若文件已存在则在其基础上追加/覆盖本次测试的温度点工作表, 未涉及的温度点
    工作表保持不变; 文件不存在则新建。各温度点的数据表与折线图分别位于独立的工作表中。

    参数:
        descending: False(默认)为电压从小到大扫描；True 为从大到小扫描
        adc_channel: 采样通道号(0~30, 默认18)
    """
    temp_input = input("请输入本次测试的温度点(℃), 多个温度点用空格或逗号分隔, 直接回车则默认使用 -40 25 85 125: ").strip()
    if temp_input:
        temp_list = [t for t in re.split(r"[,\s]+", temp_input) if t]
    else:
        temp_list = ["-40", "25", "85", "125"]

    chip_id = input("请输入本次测试的芯片编号 (如 11，对应A11): ").strip() or None

    n_samples_input = input("请输入每个电压点的ADC采集次数(取平均), 直接回车默认10次: ").strip()
    try:
        n_samples = int(n_samples_input) if n_samples_input else 10
        if n_samples <= 0:
            print("❌ 采集次数需>0, 已使用默认值10")
            n_samples = 10
    except ValueError:
        print("❌ 采集次数输入有误, 已使用默认值10")
        n_samples = 10

    # 若用户输入了 BG trim 值, 在测试开始时先写入 VBG 对应寄存器位域(RW_BG_TTRM, bit[30:25])，
    # 写入后紧跟着执行一次 REF_VTRM 使 Reference 电压重新完成修调(全程只需执行一次，不随温度点重复)。
    # BG/VREF 的 trim 值都保存下来, 之后每个温度点在 init_pa5_pa6_altfunc14() 之后重新写入,
    # 避免该温度点因芯片复位导致这两个寄存器状态丢失。
    RW_BG_TTRM = test_item.get_32bit_reg_address("RW_BG_TTRM", df_register)
    RW_REF_VTRM = test_item.get_32bit_reg_address("RW_REF_VTRM", df_register)
    bg_trim_input = input("请输入 BG 修调值(RW_BG_TTRM, 6bit, 0~63): ").strip()
    bg_trim = int(bg_trim_input, 0) if bg_trim_input else None
    ref_trim = None
    if bg_trim is not None:
        instr.swd_modify_register(RW_BG_TTRM, bg_trim, 25, 30)
        print(f"  已写入 BG trim 值: {bg_trim} -> RW_BG_TTRM")
        REF_VTRM()
        ref_trim = global_use.get("REF_VTRM_BEST")
        if ref_trim is not None:
            print(f"  REF_VTRM 修调完成, 修调值: {ref_trim} -> RW_REF_VTRM (后续每个温度点将重新写入)")

    # 本次运行的数据/图表工作表写入同一个 Excel 工作簿: 若该芯片编号对应的文件已存在,
    # 先加载进来(保留其他温度点已有的工作表), 本次测试的温度点若已存在同名工作表则覆盖更新,
    # 否则新增；不存在则新建一个空工作簿
    save_dir = os.path.join(_CHIP_TEST_RESULTS_DIR, "ADC_RMSE_TRIM")
    os.makedirs(save_dir, exist_ok=True)
    filename = f"adc_rmse_results_A{chip_id}.xlsx" if chip_id else "adc_rmse_results.xlsx"
    excel_path = os.path.join(save_dir, filename)

    if os.path.exists(excel_path):
        try:
            wb = openpyxl.load_workbook(excel_path)
            print(f"  已加载现有 Excel 文件, 将在其基础上新增/覆盖对应温度点工作表: {excel_path}")
        except Exception as e:
            print(f"  ⚠️ 加载现有 Excel 文件失败({e}), 将新建工作簿")
            wb = openpyxl.Workbook()
            wb.remove(wb.active)
    else:
        wb = openpyxl.Workbook()
        wb.remove(wb.active)

    for temp_idx, temp_c in enumerate(temp_list):
        if temp_c is not None:
            input(f"\n===== 温度点 [{temp_idx+1}/{len(temp_list)}] = {temp_c}℃ 测试 =====\n"
                  f"请将温箱/芯片温度设置到 {temp_c}℃ 并等待稳定后, 按回车键继续测试...")
        _adc_rmse_trim_one_temp(descending=descending, adc_channel=adc_channel, temp_c=temp_c, chip_id=chip_id, wb=wb,
                                n_samples=n_samples, bg_trim=bg_trim, ref_trim=ref_trim)

    try:
        wb.save(excel_path)
        print(f"  ADC_RMSE 全部温度点数据与折线图已保存: {excel_path}")
    except Exception as e:
        print(f"  ⚠️ 保存 ADC_RMSE Excel 失败: {e}")


def _adc_rmse_trim_one_temp(descending=False, adc_channel=20, temp_c=None, chip_id=None, wb=None, n_samples=10,
                             bg_trim=None, ref_trim=None):
    """ADC_RMSE_TRIM 单个温度点的完整测试流程(供 ADC_RMSE_TRIM 按温度点循环调用)"""
    # ---------- 寄存器地址 ----------
    RCC_ANACKE  = 0x4002005C   # 模拟时钟使能: bit0 ADCCKE=1（必须）
    ADC_CR      = 0x40012460   # 控制(新版UM 260720): [0]ADC_EN [16]VREFH_EN [21:20]VREFH [5:4]CLK
    ADC_OTR     = 0x40012464   # 软件触发: 写 0x5AA5 产生 ADC_SOC
    ADC_TGSR    = 0x40012448   # 触发源: [19:16]TRIG_EN [3:0]TRIG_SEL0(=8 软件)
    ADC_TGNR    = 0x4001244C   # 组0通道数: [7:0]TGN0
    ADC_SEQR0   = 0x40012450   # 通道选择: [4:0]SEQ0
    ADC_TCFG    = 0x40012468   # 时序: 复位默认(INIT=0x40 不能为0)
    ADC_IR      = 0x40012474   # 状态(只读): [31]RDY [0]S0CDIF
    ADC_DATA0   = 0x40012404   # 序列0转换结果: [11:0]
    ADC_CALIB   = 0x40012480   # 校准配置: [27:16]GAIN(复位0x800) [11:0]OFFSET
    PA_ANA_EN = test_item.get_32bit_reg_address("PA_ANA_EN", df_register)
    RW_ANA_REF_EN = test_item.get_32bit_reg_address("RW_ANA_REF_EN", df_register)
    RW_ADC_VREFH_ATP = test_item.get_32bit_reg_address("RW_ADC_VREFH_ATP", df_register)
    RW_ATP_MUX = test_item.get_32bit_reg_address("RW_ATP_MUX", df_register)
    RW_ATP_MUX_EN = test_item.get_32bit_reg_address("RW_ATP_MUX_EN", df_register)
    RW_ADC_VREFH = test_item.get_32bit_reg_address("RW_ADC_VREFH", df_register)
    RW_ANA_VREFH_EN = test_item.get_32bit_reg_address("RW_ANA_VREFH_EN", df_register)
    RW_ADC_VREFH_BYPS = test_item.get_32bit_reg_address("RW_ADC_VREFH_BYPS", df_register)
    RW_ANA_ADC_EN = test_item.get_32bit_reg_address("RW_ANA_ADC_EN", df_register)

    RW_BG_TTRM = test_item.get_32bit_reg_address("RW_BG_TTRM", df_register)
    RW_REF_VTRM = test_item.get_32bit_reg_address("RW_REF_VTRM", df_register)

    # 测试前置: 配置 PA5/PA6 复用 ALT_FUNC_14 (开 GPIO 时钟 + 输出方向 + AF14)
    init_pa5_pa6_altfunc14()
    time.sleep(0.5)

    # 若该温度点因芯片复位导致寄存器状态丢失, 需在 GPIO 前置配置之后立即重新写入
    # BG/VREF 修调值(由 ADC_RMSE_TRIM 在测试开始时统一修调得到), 保证每个温度点的
    # 参考电压基准一致
    if bg_trim is not None:
        instr.swd_modify_register(RW_BG_TTRM, bg_trim, 25, 30)
    if ref_trim is not None:
        instr.swd_modify_register(RW_REF_VTRM, ref_trim, 0, 4)

    # 使能相关模块
    instr.swd_modify_register(RW_ANA_REF_EN, 0x01, 24, 24)
    time.sleep(0.3)
    instr.swd_modify_register(RW_ANA_ADC_EN, 0x00, 19, 19)
    time.sleep(0.1)

    instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
    time.sleep(0.3)
    instr.swd_modify_register(RW_ADC_VREFH_ATP, 0x01, 18, 18)

    time.sleep(0.3)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 6, 6)
    time.sleep(0.3)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 7, 7)
    time.sleep(0.3)
    instr.swd_modify_register(RW_ATP_MUX, 0x06, 0, 4)
    time.sleep(0.3)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)
    time.sleep(0.3)
    instr.swd_modify_register(RW_ADC_VREFH, 0x02, 13, 14)
    time.sleep(0.3)
    instr.swd_modify_register(RW_ADC_VREFH, 0x00, 16, 17)
    time.sleep(0.3)
    instr.swd_modify_register(RW_ANA_VREFH_EN, 0x01, 18, 18)
    time.sleep(0.3)
    instr.swd_modify_register(RW_ADC_VREFH_BYPS, 0x01, 15, 15)
    time.sleep(0.5)
    print(f" RW_ADC_1 : {instr.swd_read_register(RW_ADC_VREFH_BYPS):08X}")
    vdd_vol = instr.dmm_measure_single_voltage()
    time.sleep(0.5)
    instr.swd_modify_register(RW_ADC_VREFH_BYPS, 0x00, 15, 15)
    print(f" RW_ADC_1 : {instr.swd_read_register(RW_ADC_VREFH_BYPS):08X}")

    # ---------- 0. 固定默认参数: 旁路VDD33, 采样通道由 adc_channel 参数指定 (同 ADC_CAL_TRIM) ----------
    VREFH_mV = vdd_vol
    vrefh_code, vrefh_byps = 0b11, 1
    print(f"  VREFH = 旁路VDD33 ({VREFH_mV:.4f} mV), 采样通道 = {adc_channel}")
    instr.swd_modify_register(RW_ANA_ADC_EN, 0x01, 19, 19)
    time.sleep(0.3)
    instr.swd_modify_register(RCC_ANACKE, 0x01, 0, 0)            # ① 使能 ADC 时钟 ADCCKE=1
    time.sleep(0.3)
    instr.swd_modify_register(ADC_SEQR0, adc_channel, 0, 4)           # ② SEQ0 = 所选通道
    time.sleep(0.3)
    instr.swd_modify_register(ADC_TGNR,   1, 0, 7)               # ② TGN0 = 1（组0 1个通道）
    time.sleep(0.3)
    instr.swd_modify_register(ADC_TGSR,   0b0001, 16, 19)        # ② TRIG_EN = 仅组0
    time.sleep(0.3)
    instr.swd_modify_register(ADC_TGSR,   0b1000,  0,  3)        # ② TRIG_SEL0 = 8 (软件)
    time.sleep(0.3)
    instr.swd_write_register(ADC_TCFG,    0x0A4020F4)            # ② 复位默认时序(INIT≠0)
    time.sleep(0.1)

    CR_VAL = 0x00000031 | (vrefh_code << 20) | 0x00010000  # ADC_EN=1 + CLK=2M + VREFH_EN=1 + VREFH码
    if vrefh_byps:
        CR_VAL |= 0x00020000                    # VREFH_BYPS=1 → 量程=VDD33(≈3.3V)
    instr.swd_write_register(ADC_CR, CR_VAL)
    time.sleep(0.1)
    print(f" RW_ADC_1 : {instr.swd_read_register(RW_ADC_VREFH_BYPS):08X}")
    # ---------- 1. 线性扫描测试电压, 逐点用 DMM 实测 + ADC 转换电压采样 ----------
    # VDD/2 附近 ±5mV 区间内步进 1mV(精细扫描), 其他区域步进 100mV
    v_start, v_stop, v_step = 100, int(VREFH_mV) - 100, 100
    v_start = max(0, min(v_start, int(VREFH_mV)))
    v_stop = max(0, min(v_stop, int(VREFH_mV)))
    if v_stop < v_start:
        v_start, v_stop = v_stop, v_start
    v_mid = int(VREFH_mV / 2)
    fine_start = max(v_start, v_mid - 5)
    fine_stop = min(v_stop, v_mid + 5)

    vin_seq = []
    if fine_stop >= fine_start:
        vin_seq += list(range(v_start, fine_start, max(1, v_step)))
        vin_seq += list(range(fine_start, fine_stop + 1, 1))
        vin_seq += list(range(fine_stop + v_step, v_stop + 1, max(1, v_step)))
    else:
        vin_seq = list(range(v_start, v_stop + 1, max(1, v_step)))

    if descending:
        vin_seq = vin_seq[::-1]

    _SKIP_VOLTAGE_ADC_SWEEP = False

    rmse_pts = []   # (vin, 实测电压, 转换电压, ADC原始值)
    if not _SKIP_VOLTAGE_ADC_SWEEP:
        for i, vin in enumerate(vin_seq):
            instr.serial_send(f"spi_cmd_set_val mV {vin:g}\r\n")
            time.sleep(0.5)
            instr.serial_send(f"cmd_gpio_set_595d_test set 38 6\r\n")
            time.sleep(1)
            vol = instr.dmm_measure_single_voltage()
            time.sleep(0.5)
            instr.serial_send(f"cmd_gpio_set_595d_test reset\r\n")
            time.sleep(0.5)

            # 每个电压点连续采集 n_samples 次ADC原始值, 取平均后作为该点的最终采样结果, 降低单次采样噪声影响
            raw_samples = []
            N_SAMPLES = n_samples
            for s in range(N_SAMPLES):
                instr.swd_write_register(ADC_OTR, 0x5AA5)      # 软件触发一次
                time.sleep(0.5)                              # 等转换完成(2M sps)后再轮询, 减少 J-Link 往返
                done = False
                for _ in range(20):                            # 轮询 IR.S0CDIF (bit0), 超时保护
                    ir_val = instr.swd_read_register(ADC_IR)
                    if ir_val is not None and (ir_val & 0x1):
                        done = True
                        break
                    time.sleep(0.05)
                if not done:
                    print(f"  [{i+1:>3}/{len(vin_seq)}] ⚠️ 第{s+1}/{N_SAMPLES}次转换触发超时, 跳过该次采样")
                    continue
                raw_s = instr.swd_read_register(ADC_DATA0)
                time.sleep(0.05)
                if raw_s is None:
                    print(f"  [{i+1:>3}/{len(vin_seq)}] ⚠️ 第{s+1}/{N_SAMPLES}次读取ADC原始值失败, 跳过该次采样")
                    continue
                raw_samples.append(raw_s & 0xFFF)

            if not raw_samples:
                print(f"  [{i+1:>3}/{len(vin_seq)}] ⚠️ {N_SAMPLES}次采样均失败, 跳过该点")
                continue
            raw_avg = sum(raw_samples) / len(raw_samples)
            conv_mv = raw_avg / 4095.0 * VREFH_mV                  # 转换电压(mV)
            rmse_pts.append((float(vin), float(vol), float(conv_mv), raw_avg))
            print(f"  [{i+1:>3}/{len(vin_seq)}] vin={vin:6d}mV 实测={vol:8.3f}mV 原始ADC值(10次均值)={raw_avg:8.2f} 转换={conv_mv:8.3f}mV")
    else:
        print("  ⚠️ [调试模式] 已屏蔽循环电压测量与ADC采集, rmse_pts 为空")

    # ---------- 2. RMSE 计算(最小二乘线性拟合残差的均方根误差, 单位 LSB) ----------
    # x_i = 实测电压 Vtrue(mV, 高精度真值)，y_i = ADC原始采样值 Code(LSB)
    # 拟合直线: Code_i = a + b*Vtrue_i (a=截距, b=斜率)
    # 残差: e_i = Code_i - Code_hat_i；RMSE = sqrt( mean(e_i^2) )，单位即为 LSB
    lsb_mv = VREFH_mV / 4096.0
    rmse_lsb = None
    fit_slope = None
    fit_intercept = None
    code_hat_list = None
    residual_list = None
    if len(rmse_pts) >= 2:
        xs = [vol for _, vol, _, _ in rmse_pts]           # Vtrue(实测电压 mV)
        ys = [raw for _, _, _, raw in rmse_pts]            # Code(ADC原始值)
        n_pt = len(xs)
        sum_x = sum(xs)
        sum_y = sum(ys)
        sum_xy = sum(x * y for x, y in zip(xs, ys))
        sum_xx = sum(x * x for x in xs)
        denom = n_pt * sum_xx - sum_x * sum_x
        if denom != 0:
            fit_slope = (n_pt * sum_xy - sum_x * sum_y) / denom
            fit_intercept = (sum_y - fit_slope * sum_x) / n_pt
            code_hat_list = [fit_slope * x + fit_intercept for x in xs]
            residual_list = [y - y_hat for y, y_hat in zip(ys, code_hat_list)]
            sse = sum(e ** 2 for e in residual_list)
            rmse_lsb = (sse / n_pt) ** 0.5
            print(f"\n[线性拟合] Code = {fit_slope:.6f} * Vtrue(mV) + {fit_intercept:.3f}")
            print(f"[ADC RMSE] 样本数={n_pt}  RMSE={rmse_lsb:.4f} LSB")
        else:
            print("\n⚠️ 线性拟合失败(分母为0), 无法计算 RMSE")
    else:
        print("\n❌ 有效采样点不足(<2), 无法计算 RMSE")

    # ---------- 2.1 取ADC原始值最接近2048的点, 计算首个DNL1/DNL2/INL ----------
    # rmse_pts 元素: (vin, 实测电压vol, 转换电压conv, ADC原始值raw)
    dnl1 = None
    dnl2 = None
    inl = None
    dnl_code = None
    if rmse_pts:
        target_code = 2048
        raw_list = [raw for _, _, _, raw in rmse_pts]
        min_diff = min(abs(raw - target_code) for raw in raw_list)
        x_val = next(raw for raw in raw_list if abs(raw - target_code) == min_diff)
        dnl_code = x_val

        # 若有多个连续出现的 x_val, 取第一次出现和最后一次出现的下标
        idx_list = [i for i, raw in enumerate(raw_list) if raw == x_val]
        first_idx = idx_list[0]
        last_idx = idx_list[-1]

        if first_idx - 1 >= 0:
            prev_raw = raw_list[first_idx - 1]
            prev_vol = rmse_pts[first_idx - 1][1]
            first_vol = rmse_pts[first_idx][1]
            if (first_vol - prev_vol) != 0:
                dnl1 = (x_val - prev_raw) / (first_vol - prev_vol)

        if last_idx + 1 < len(raw_list):
            next_raw = raw_list[last_idx + 1]
            next_vol = rmse_pts[last_idx + 1][1]
            last_vol = rmse_pts[last_idx][1]
            if (next_vol - last_vol) != 0:
                dnl2 = (next_raw - x_val) / (next_vol - last_vol)

        first_conv = rmse_pts[first_idx][2]
        first_vol_for_inl = rmse_pts[first_idx][1]
        inl = first_conv - first_vol_for_inl

        print(f"\n[DNL/INL] 最接近2048的ADC原始值 x={x_val}")
        print(f"  DNL1 = {dnl1 if dnl1 is None else round(dnl1, 4)}")
        print(f"  DNL2 = {dnl2 if dnl2 is None else round(dnl2, 4)}")
        print(f"  INL  = {round(inl, 4)}")

    # ---------- 2.2 将本温度点数据表 + 折线图写入共享工作簿(每温度点各占独立工作表) ----------
    if rmse_pts:
        save_adc_rmse_excel(rmse_pts, VREFH_mV, rmse_lsb, dnl1, dnl2, inl, dnl_code, fit_slope, fit_intercept,
                             code_hat_list, residual_list, chip_id=chip_id, temp_c=temp_c, wb=wb)

    # ---------- 3. 判定与保存 ----------
    temp_suffix = f"_{temp_c}C" if temp_c is not None else ""
    if rmse_lsb is not None:
        passed = abs(rmse_lsb) <= 2.0     # 规格判定: RMSE ±2 LSB (参考 ADC_DNL 规格)
        save_item_result(f"ADC_RMSE{temp_suffix}", passed, round(rmse_lsb, 3))
        print(f"{'✅ ADC_RMSE 测试通过' if passed else '❌ ADC_RMSE 测试失败'} "
              f"(RMSE {rmse_lsb:.3f} LSB, 规格 ±2 LSB)")

    if dnl1 is not None and dnl2 is not None:
        dnl_passed = abs(dnl1) <= 1.0 and abs(dnl2) <= 1.0     # 规格判定: DNL ±1 LSB
        save_item_result(f"ADC_DNL{temp_suffix}", dnl_passed, round(max(abs(dnl1), abs(dnl2)), 3))
        print(f"{'✅ ADC_DNL 测试通过' if dnl_passed else '❌ ADC_DNL 测试失败'} "
              f"(DNL1={dnl1:.3f} LSB, DNL2={dnl2:.3f} LSB, 规格 ±1 LSB)")

    if inl is not None:
        inl_lsb = inl / lsb_mv
        inl_passed = abs(inl_lsb) <= 2.0     # 规格判定: INL ±2 LSB
        save_item_result(f"ADC_INL{temp_suffix}", inl_passed, round(inl_lsb, 3))
        print(f"{'✅ ADC_INL 测试通过' if inl_passed else '❌ ADC_INL 测试失败'} "
              f"(INL={inl_lsb:.3f} LSB, 规格 ±2 LSB)")

    # ---------- 4. 清理 ----------
    instr.swd_modify_register(RW_ANA_ADC_EN, 0x00, 19, 19)       # 关闭 ADC_EN(仅清该位, 避免整字写破坏 ANAC_ENCR 其他使能位)
    instr.serial_send("cmd_gpio_set_595d_test reset\r\n")        # 只发送不等待响应


@test_case
def ADC_CAL_TRIM(adc_channel=20):
    """test_adc_cal

    参数:
        adc_channel: 采样通道号(0~30, 默认18)
    """
 # ---------- 寄存器地址 ----------
    RCC_ANACKE  = 0x4002005C   # 模拟时钟使能: bit0 ADCCKE=1（必须）
    ANAC_ENCR   = 0x40007800   # 模拟电源/基准使能(可选, 采样全0时可打开)
    ADC_CR      = 0x40012460   # 控制(新版UM 260720): [0]ADC_EN [16]VREFH_EN [21:20]VREFH [5:4]CLK
    ADC_OTR     = 0x40012464   # 软件触发: 写 0x5AA5 产生 ADC_SOC
    ADC_TGSR    = 0x40012448   # 触发源: [19:16]TRIG_EN [3:0]TRIG_SEL0(=8 软件)
    ADC_TGNR    = 0x4001244C   # 组0通道数: [7:0]TGN0
    ADC_SEQR0   = 0x40012450   # 通道选择: [4:0]SEQ0
    ADC_TCFG    = 0x40012468   # 时序: 复位默认(INIT=0x40 不能为0)
    ADC_IR      = 0x40012474   # 状态(只读): [31]RDY [0]S0CDIF
    ADC_DATA0   = 0x40012404   # 序列0转换结果: [11:0]
    ADC_CALIB   = 0x40012480   # 校准配置: [27:16]GAIN(复位0x800) [11:0]OFFSET
    ADC_ORIDAT  = 0x40012488   # 原始校准值
    ADC_CALIBDAT= 0x40012484   # 校准结果
    PA_ANA_EN = test_item.get_32bit_reg_address("PA_ANA_EN", df_register)
    RW_ANA_REF_EN = test_item.get_32bit_reg_address("RW_ANA_REF_EN", df_register)
    RW_ADC_VREFH_ATP = test_item.get_32bit_reg_address("RW_ADC_VREFH_ATP", df_register)
    RW_ATP_MUX = test_item.get_32bit_reg_address("RW_ATP_MUX", df_register)
    RW_ATP_MUX_EN = test_item.get_32bit_reg_address("RW_ATP_MUX_EN", df_register)
    RW_ADC_VREFH = test_item.get_32bit_reg_address("RW_ADC_VREFH", df_register)
    RW_ANA_VREFH_EN = test_item.get_32bit_reg_address("RW_ANA_VREFH_EN", df_register)
    RW_ADC_VREFH_BYPS = test_item.get_32bit_reg_address("RW_ADC_VREFH_BYPS", df_register)
    RW_ANA_ADC_EN = test_item.get_32bit_reg_address("RW_ANA_ADC_EN", df_register)

    # 测试前置: 配置 PA5/PA6 复用 ALT_FUNC_14 (开 GPIO 时钟 + 输出方向 + AF14)
    init_pa5_pa6_altfunc14()
    time.sleep(0.5)

    #使能相关模块
    instr.swd_modify_register(RW_ANA_REF_EN, 0x01, 24, 24)
    instr.swd_modify_register(RW_ANA_ADC_EN, 0x01, 19, 19)
    time.sleep(0.1)

    instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
    instr.swd_modify_register(RW_ADC_VREFH_ATP, 0x01, 18, 18)

    time.sleep(0.1)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 6, 6)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 7, 7)
    instr.swd_modify_register(RW_ATP_MUX, 0x06, 0, 4)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)
    instr.swd_modify_register(RW_ADC_VREFH, 0x02, 13, 14)
    instr.swd_modify_register(RW_ADC_VREFH, 0x00, 16, 17)
    instr.swd_modify_register(RW_ANA_VREFH_EN, 0x01, 18, 18)
    instr.swd_modify_register(RW_ADC_VREFH_BYPS, 0x01, 15, 15)
    time.sleep(1)
    vdd_vol = instr.dmm_measure_single_voltage()
    time.sleep(1)
    instr.swd_modify_register(RW_ADC_VREFH_BYPS, 0x00, 15, 15)

    # ---------- 0. 固定默认参数: 选项5 旁路VDD33, 采样通道由 adc_channel 参数指定 ----------
    VREFH_mV = vdd_vol
    vrefh_code, vrefh_byps = 0b11, 1
    print(f"  VREFH = 旁路VDD33 ({VREFH_mV:.4f} mV), 采样通道 = {adc_channel}")
    

    instr.swd_modify_register(RCC_ANACKE, 0x01, 0, 0)            # ① 使能 ADC 时钟 ADCCKE=1
    instr.swd_modify_register(ADC_SEQR0, adc_channel, 0, 4)           # ② SEQ0 = 所选通道
    instr.swd_modify_register(ADC_TGNR,   1, 0, 7)               # ② TGN0 = 1（组0 1个通道）
    instr.swd_modify_register(ADC_TGSR,   0b0001, 16, 19)        # ② TRIG_EN = 仅组0
    instr.swd_modify_register(ADC_TGSR,   0b1000,  0,  3)        # ② TRIG_SEL0 = 8 (软件)
    instr.swd_write_register(ADC_TCFG,    0x0A4020F4)            # ② 复位默认时序(INIT≠0)
    time.sleep(0.1)

    CR_VAL = 0x00000031 | (vrefh_code << 20) | 0x00010000  # ADC_EN=1 + CLK=2M + VREFH_EN=1 + VREFH码
    if vrefh_byps:
        CR_VAL |= 0x00020000                    # VREFH_BYPS=1 → 量程=VDD33(≈3.3V)
    instr.swd_write_register(ADC_CR, CR_VAL)


    time.sleep(0.1)
    instr.swd_write_register(ADC_CALIB, 0x08000000)
    cal_mV = 500.0
    if cal_mV > VREFH_mV:
        print(f"⚠️ 校准电压 {cal_mV:.4f} mV 大于 VREFH_mV ({VREFH_mV:.4f} mV)，超出参考量程，将按 VREFH_mV 进行校准")
        cal_mV = VREFH_mV
    instr.serial_send(f"spi_cmd_set_val mV {cal_mV:g}\r\n")   # 只发送不等待响应(避免串口读阻塞)
    time.sleep(1)
    instr.serial_send(f"cmd_gpio_set_595d_test set 38 6\r\n")
    time.sleep(1)
    Acl_vol = instr.dmm_measure_single_voltage()
    print(f"实际测量电压为：{Acl_vol:.4f}mV")
    time.sleep(1)
    instr.serial_send(f"cmd_gpio_set_595d_test reset\r\n")
    time.sleep(1)
    instr.swd_write_register(ADC_OTR, 0x5AA5)      # 软件触发一次
    time.sleep(0.5)                              # 等转换完成(2M sps)后再轮询, 减少 J-Link 往返

    mea_ori = []
    for _ in range(20):
        instr.swd_write_register(ADC_OTR, 0x5AA5)      # 软件触发一次
        time.sleep(0.5)                              # 等转换完成(2M sps)后再轮询, 减少 J-Link 往返
        done = False
        for _ in range(20):                            # 轮询 IR.S0CDIF (bit0), 超时保护
            ir_val = instr.swd_read_register(ADC_IR)
            if ir_val is not None and (ir_val & 0x1):
                done = True
                break
            time.sleep(0.05)
        if not done:
            print(" ⚠️ 转换触发超时")
            continue
        raw = instr.swd_read_register(ADC_DATA0)
        time.sleep(0.5)
        raw &= 0xFFF
        conv_mv = raw / 4095.0 * VREFH_mV                  # 转换电压(mV)
        print(f"实测电压为:{Acl_vol:.4f}mV ADC采集电压为:{conv_mv:.4f}mV")
        difer_vol = Acl_vol - conv_mv
        mea_ori.append(difer_vol)
        time.sleep(0.05)
    
    ave_fifer_vol = sum(mea_ori) / len(mea_ori)
    orig_vol = Acl_vol + ave_fifer_vol
    ave_fifer_ori = int(ave_fifer_vol / VREFH_mV * 4095)
    OFFSET = (ave_fifer_ori & 0xFFF)
    GAIN = 0x800
    instr.swd_modify_register(ADC_CALIB, OFFSET, 0, 11)
    instr.swd_modify_register(ADC_CALIB, GAIN, 16, 27)
    print(f"校准值OFFSET为: {OFFSET}")
    # 保存 OFFSET/GAIN 打包值到 trim_best_value.xlsx, 供 program_trim.py 读取后
    # 烧入 OTP 的 ADCTRM word。★ 注意 OTP ADCTRM word 的位布局与 ADC_CALIB
    # 实时寄存器相反: OTP 为 bit0~11=GAIN, bit16~27=OFFSET
    adctrm_word = (GAIN & 0xFFF) | ((OFFSET & 0xFFF) << 16)
    save_trim_to_excel("adc_cal_trim", "ADCTRM", ADC_CALIB, adctrm_word, passed=True)
    time.sleep(0.1)
    CR_VAL |= (1 << 1)
    instr.swd_write_register(ADC_CR, CR_VAL)            #使能校准
    time.sleep(1.5)
    
    instr.swd_write_register(ADC_OTR, 0x5AA5)      # 软件触发一次
    time.sleep(0.5)                              # 等转换完成(2M sps)后再轮询, 减少 J-Link 往返

    done = False
    for _ in range(20):                            # 轮询 IR.S0CDIF (bit0), 超时保护
        ir_val = instr.swd_read_register(ADC_IR)
        if ir_val is not None and (ir_val & 0x1):
            done = True
            break
        time.sleep(0.05)
    if not done:
        print(" ⚠️ 转换触发超时")
        return
    ori = instr.swd_read_register(ADC_ORIDAT)
    raw = instr.swd_read_register(ADC_DATA0)
    time.sleep(0.5)
    ori &= 0xFFF
    raw &= 0xFFF
    conv_mv = raw / 4095.0 * VREFH_mV                  # 校准转换电压(mV)
    conv_ori_mv = ori / 4095.0 * VREFH_mV               # 原始转换电压(mV)

    print(f"实测电压为:{Acl_vol:.4f}mV 校准前的平均采集电压为:{orig_vol:.4f}mV, 校准后的原始电压为: {conv_ori_mv:.4f}mV, 校准后的采集电压为:{conv_mv:.4f}mV")

    # ---------- GAIN 校准: 线性扫描测试电压 (参考 ADC_DNL_Test 线性扫描模式) ----------
    do_gain = "yes"
    if do_gain in ('y', 'yes'):
        g_start, g_stop, g_step = 100, int(VREFH_mV) - 100, 100
        g_start = max(0, min(g_start, int(VREFH_mV)))
        g_stop = max(0, min(g_stop, int(VREFH_mV)))
        if g_stop < g_start:
            g_start, g_stop = g_stop, g_start
        gain_vin_seq = list(range(g_start, g_stop + 1, max(1, g_step)))

        gain_pts = []   # (vin, vol实测, conv_mv转换)
        for i, vin in enumerate(gain_vin_seq):
            instr.serial_send(f"spi_cmd_set_val mV {vin:g}\r\n")
            time.sleep(0.5)
            instr.serial_send(f"cmd_gpio_set_595d_test set 38 6\r\n")
            time.sleep(1)
            vol = instr.dmm_measure_single_voltage()
            time.sleep(1)
            instr.serial_send(f"cmd_gpio_set_595d_test reset\r\n")
            time.sleep(1)

            instr.swd_write_register(ADC_OTR, 0x5AA5)
            time.sleep(0.5)
            done = False
            for _ in range(20):
                ir_val = instr.swd_read_register(ADC_IR)
                if ir_val is not None and (ir_val & 0x1):
                    done = True
                    break
                time.sleep(0.05)
            if not done:
                print(f"  [{i+1:>3}/{len(gain_vin_seq)}] ⚠️ 转换触发超时, 跳过")
                continue
            raw_g = instr.swd_read_register(ADC_DATA0)
            time.sleep(0.05)
            if raw_g is None:
                continue
            raw_g &= 0xFFF
            conv_mv_g = raw_g / 4095.0 * VREFH_mV
            gain_pts.append((float(vin), float(vol), float(conv_mv_g)))
            print(f"  [{i+1:>3}/{len(gain_vin_seq)}] vin={vin:6d}mV 实测={vol:8.3f}mV 转换={conv_mv_g:8.3f}mV")

        def _lsq_slope(xs, ys):
            n = len(xs)
            if n < 2:
                return None
            sx, sy = sum(xs), sum(ys)
            sxy = sum(x * y for x, y in zip(xs, ys))
            sxx = sum(x * x for x in xs)
            denom = n * sxx - sx * sx
            if denom == 0:
                return None
            return (n * sxy - sx * sy) / denom

        if len(gain_pts) >= 2:
            xs_g = [p[0] for p in gain_pts]
            meas_ys = [p[1] for p in gain_pts]
            conv_ys = [p[2] for p in gain_pts]
            slope_meas = _lsq_slope(xs_g, meas_ys)
            slope_conv = _lsq_slope(xs_g, conv_ys)
            time.sleep(0.5)
            reg_get = instr.swd_read_register(ADC_CALIB)
            time.sleep(0.5)
            gain_get = reg_get >> 16 & 0xFFF
            old_a = gain_get / 2048.0
            if slope_meas is not None and slope_conv is not None:
                slope_diff = abs(slope_meas - slope_conv)
                A = (old_a + slope_diff) if slope_meas > slope_conv else (old_a - slope_diff)
                GAIN_input = round(A * 2048)
                GAIN_input = max(0, min(GAIN_input, 0xFFF))
                print(f"\n[GAIN 线性扫描] 实测电压斜率={slope_meas:.6f}  转换电压斜率={slope_conv:.6f}  差值={slope_diff:.6f}")
                print(f"  A = {A:.6f}  →  GAIN(A*2048 四舍五入) = {GAIN_input} (0x{GAIN_input:03X})")

                GAIN = GAIN_input
                instr.swd_modify_register(ADC_CALIB, GAIN, 16, 27)
                # ★ OTP ADCTRM word 位布局与 ADC_CALIB 相反: bit0~11=GAIN, bit16~27=OFFSET
                adctrm_word = (GAIN & 0xFFF) | ((OFFSET & 0xFFF) << 16)
                save_trim_to_excel("adc_cal_trim", "ADCTRM", ADC_CALIB, adctrm_word, passed=True)
                print(f"  ✅ 已写入 GAIN={GAIN}(0x{GAIN:03X}) 到 ADC_CALIB[27:16], 并更新 ADCTRM 打包值 0x{adctrm_word:08X}")
            else:
                print("  ⚠️ 斜率拟合失败(样本不足或分母为0), 未更新 GAIN")
        else:
            print("  ⚠️ 有效采样点不足(<2), 未进行 GAIN 校准")


@test_case
def DAC_ADC_VOLTAGE_SWEEP(adc_channel=20):
    """DAC_ADC_VOLTAGE_SWEEP —— DAC 输出扫描 + ADC 采集(开启ADC校准)长时间数据采集

    流程: 用户输入 DAC 初始值/终值/步进值(16位, 0~65535, 对应 spi_cmd_set_val Ori <val>)，
    每个点先用数字万用表(DMM)实测输出电压，再触发 ADC 采集原始值并换算成电压，
    打印 实测电压/ADC原始值/ADC转换电压，并逐行写入 CSV 文件(边测边写，适合长时间大数据量采集)。

    参数:
        adc_channel: 采样通道号(0~30, 默认18)
    """
    # ---------- 寄存器地址(与 ADC_CAL_TRIM/ADC_RMSE_TRIM 一致) ----------
    RCC_ANACKE  = 0x4002005C
    ADC_CR      = 0x40012460
    ADC_OTR     = 0x40012464
    ADC_TGSR    = 0x40012448
    ADC_TGNR    = 0x4001244C
    ADC_SEQR0   = 0x40012450
    ADC_TCFG    = 0x40012468
    ADC_IR      = 0x40012474
    ADC_DATA0   = 0x40012404
    ADC_CALIB   = 0x40012480

    PA_ANA_EN = test_item.get_32bit_reg_address("PA_ANA_EN", df_register)
    RW_ANA_REF_EN = test_item.get_32bit_reg_address("RW_ANA_REF_EN", df_register)
    RW_ADC_VREFH_ATP = test_item.get_32bit_reg_address("RW_ADC_VREFH_ATP", df_register)
    RW_ATP_MUX = test_item.get_32bit_reg_address("RW_ATP_MUX", df_register)
    RW_ATP_MUX_EN = test_item.get_32bit_reg_address("RW_ATP_MUX_EN", df_register)
    RW_ADC_VREFH = test_item.get_32bit_reg_address("RW_ADC_VREFH", df_register)
    RW_ANA_VREFH_EN = test_item.get_32bit_reg_address("RW_ANA_VREFH_EN", df_register)
    RW_ADC_VREFH_BYPS = test_item.get_32bit_reg_address("RW_ADC_VREFH_BYPS", df_register)
    RW_ANA_ADC_EN = test_item.get_32bit_reg_address("RW_ANA_ADC_EN", df_register)

    # ---------- 用户输入 DAC 初始值/终值/步进值(16位, 0~65535) ----------
    def _read_dac_int(prompt, default=None):
        while True:
            raw = input(prompt).strip()
            if raw == "" and default is not None:
                return default
            try:
                val = int(raw)
            except ValueError:
                print("  ⚠️ 请输入整数(0~65535)")
                continue
            if val < 0 or val > 65535:
                print("  ⚠️ 超出16位范围, 请输入 0~65535 之间的值")
                continue
            return val

    dac_start = _read_dac_int("请输入 DAC 初始值 (0~65535): ")
    dac_stop = _read_dac_int("请输入 DAC 终值 (0~65535): ")
    dac_step = _read_dac_int("请输入 DAC 步进值 (1~65535): ")
    if dac_step <= 0:
        print("❌ 步进值必须大于0")
        return
    if dac_stop < dac_start:
        dac_start, dac_stop = dac_stop, dac_start

    # ---------- 硬件前置初始化(旁路VDD33作VREFH参考 + 开启ADC校准) ----------
    init_pa5_pa6_altfunc14()
    time.sleep(0.5)

    instr.swd_modify_register(RW_ANA_REF_EN, 0x01, 24, 24)
    instr.swd_modify_register(RW_ANA_ADC_EN, 0x01, 19, 19)
    time.sleep(0.1)

    instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
    instr.swd_modify_register(RW_ADC_VREFH_ATP, 0x01, 18, 18)
    time.sleep(0.1)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 6, 6)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 7, 7)
    instr.swd_modify_register(RW_ATP_MUX, 0x06, 0, 4)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)
    instr.swd_modify_register(RW_ADC_VREFH, 0x02, 13, 14)
    instr.swd_modify_register(RW_ADC_VREFH, 0x00, 16, 17)
    instr.swd_modify_register(RW_ANA_VREFH_EN, 0x01, 18, 18)
    instr.swd_modify_register(RW_ADC_VREFH_BYPS, 0x01, 15, 15)
    time.sleep(1)
    VREFH_mV = instr.dmm_measure_single_voltage()
    time.sleep(1)
    instr.swd_modify_register(RW_ADC_VREFH_BYPS, 0x00, 15, 15)

    vrefh_code, vrefh_byps = 0b11, 1
    print(f"  VREFH = 旁路VDD33 ({VREFH_mV:.4f} mV), 采样通道 = {adc_channel}")

    instr.swd_modify_register(RCC_ANACKE, 0x01, 0, 0)
    instr.swd_modify_register(ADC_SEQR0, adc_channel, 0, 4)
    instr.swd_modify_register(ADC_TGNR,   1, 0, 7)
    instr.swd_modify_register(ADC_TGSR,   0b0001, 16, 19)
    instr.swd_modify_register(ADC_TGSR,   0b1000,  0,  3)
    instr.swd_write_register(ADC_TCFG,    0x0A4020F4)
    time.sleep(0.1)

    CR_VAL = 0x00000031 | (vrefh_code << 20) | 0x00010000
    if vrefh_byps:
        CR_VAL |= 0x00020000
    CR_VAL |= (1 << 1)   # 开启ADC校准(使用已写入 ADC_CALIB 的 OFFSET/GAIN)
    instr.swd_write_register(ADC_CR, CR_VAL)
    time.sleep(0.1)

    # ---------- 新建大数据量存储文件(CSV, 逐行写入+flush, 适合长时间采集) ----------
    save_dir = os.path.join(_output_base_dir, "Save_Results", "OCD5010")
    os.makedirs(save_dir, exist_ok=True)
    timestamp = time.strftime("%Y%m%d_%H%M%S")
    csv_path = os.path.join(save_dir, f"dac_adc_sweep_{timestamp}.csv")

    import csv
    f = open(csv_path, "w", newline="", encoding="utf-8-sig")
    writer = csv.writer(f)
    header = ["序号", "DAC设定值(0-65535)", "实测电压Vtrue(mV)", "ADC原始值Code(0-4095)", "ADC转换电压(mV)"]
    col_widths = [6, 18, 18, 20, 18]
    writer.writerow([h.rjust(w) for h, w in zip(header, col_widths)])
    f.flush()

    print(f"数据将写入: {csv_path}")
    print(f"DAC 扫描范围: {dac_start} ~ {dac_stop}, 步进: {dac_step}")

    idx = 0
    try:
        dac_val = dac_start
        while dac_val <= dac_stop:
            idx += 1
            instr.serial_send(f"spi_cmd_set_val Ori {dac_val}\r\n")
            time.sleep(0.5)

            # 先接通万用表测量通路，实测电压，再断开通路
            instr.serial_send(f"cmd_gpio_set_595d_test set 38 6\r\n")
            time.sleep(1)
            v_true = instr.dmm_measure_single_voltage()
            time.sleep(1)
            instr.serial_send(f"cmd_gpio_set_595d_test reset\r\n")
            time.sleep(1)

            # 再进行 ADC 采集
            instr.swd_write_register(ADC_OTR, 0x5AA5)
            time.sleep(0.5)
            done = False
            for _ in range(20):
                ir_val = instr.swd_read_register(ADC_IR)
                if ir_val is not None and (ir_val & 0x1):
                    done = True
                    break
                time.sleep(0.05)
            if not done:
                print(f"  [{idx}] DAC={dac_val} ⚠️ ADC转换触发超时, 跳过")
                dac_val += dac_step
                continue

            raw = instr.swd_read_register(ADC_DATA0)
            if raw is None:
                print(f"  [{idx}] DAC={dac_val} ⚠️ 读取ADC原始值失败, 跳过")
                dac_val += dac_step
                continue
            raw &= 0xFFF
            conv_mv = raw / 4095.0 * VREFH_mV

            print(f"[{idx}] DAC={dac_val:6d}  实测电压Vtrue={v_true:9.4f} mV  "
                  f"ADC原始值Code={raw:4d}  ADC转换电压={conv_mv:9.4f} mV")

            writer.writerow([
                str(idx).rjust(col_widths[0]),
                str(dac_val).rjust(col_widths[1]),
                f"{v_true:.4f}".rjust(col_widths[2]),
                str(raw).rjust(col_widths[3]),
                f"{conv_mv:.4f}".rjust(col_widths[4]),
            ])
            f.flush()

            dac_val += dac_step
    finally:
        f.close()
        instr.swd_modify_register(RW_ANA_ADC_EN, 0x00, 19, 19)
        print(f"采集完成, 共 {idx} 个数据点, 数据已保存至: {csv_path}")
        save_item_result("DAC_ADC_VOLTAGE_SWEEP", True, idx)


@test_case
def ADC_LOOP_READ(adc_channel=20, dac_step=3, half_range_mv=1.2, adc_samples=1024,
                   ori_lo=15883, ori_center=15902, ori_hi=15921):
    """ADC_LOOP_READ —— DAC(Ori码)扫描 + 每点ADC采集1024次取平均, 绘制散点图

    流程: 目标中心电压取ADC量程的一半(VDD_H = VREFH_mV/2), 起始/终止Ori码不假设固定的
    Ori-mV换算公式, 而是用DMM实测反馈闭环校准(两点估算斜率+逐步逼近)得到, 使输出电压
    分别逼近 VDD_H-half_range_mv 与 VDD_H+half_range_mv；校准完成后在该Ori码范围内
    按 dac_step 步进扫描, 每点先用DMM实测一次输出电压, 再触发ADC连续采集 adc_samples
    次取平均, 换算成电压。所有数据(Ori码/实测电压/ADC平均原始值/ADC转换电压)保存到
    同一个Excel文档, 并绘制散点图。
    ★ 经实测确认, DAC Ori码与输出电压的对应关系稳定, 故默认不再现场用DMM闭环校准,
    统一固定使用: 下限Ori=15883(998.8mV) / 基准Ori=15902(1000mV) / 上限Ori=15921(1001.2mV)。
    若需要重新校准, 传入 ori_lo/ori_center/ori_hi=None 即可恢复现场DMM闭环校准。

    参数:
        adc_channel: 采样通道号(0~30, 默认20)
        dac_step: DAC Ori码步进值(默认3)
        half_range_mv: 以VDD_H为中心的扫描半范围(mV, 默认1.2)
        adc_samples: 每个DAC点ADC采集次数取平均(默认1024)
        ori_lo/ori_center/ori_hi: 实测已确认的固定Ori码(默认15883/15902/15921), 传None则改为现场DMM闭环校准
    """
    # ---------- 寄存器地址(与 ADC_CAL_TRIM/ADC_RMSE_TRIM 一致) ----------
    RCC_ANACKE  = 0x4002005C
    ADC_CR      = 0x40012460
    ADC_OTR     = 0x40012464
    ADC_TGSR    = 0x40012448
    ADC_TGNR    = 0x4001244C
    ADC_SEQR0   = 0x40012450
    ADC_TCFG    = 0x40012468
    ADC_IR      = 0x40012474
    ADC_DATA0   = 0x40012404

    PA_ANA_EN = test_item.get_32bit_reg_address("PA_ANA_EN", df_register)
    RW_ANA_REF_EN = test_item.get_32bit_reg_address("RW_ANA_REF_EN", df_register)
    RW_ADC_VREFH_ATP = test_item.get_32bit_reg_address("RW_ADC_VREFH_ATP", df_register)
    RW_ATP_MUX = test_item.get_32bit_reg_address("RW_ATP_MUX", df_register)
    RW_ATP_MUX_EN = test_item.get_32bit_reg_address("RW_ATP_MUX_EN", df_register)
    RW_ADC_VREFH = test_item.get_32bit_reg_address("RW_ADC_VREFH", df_register)
    RW_ANA_VREFH_EN = test_item.get_32bit_reg_address("RW_ANA_VREFH_EN", df_register)
    RW_ADC_VREFH_BYPS = test_item.get_32bit_reg_address("RW_ADC_VREFH_BYPS", df_register)
    RW_ANA_ADC_EN = test_item.get_32bit_reg_address("RW_ANA_ADC_EN", df_register)
    RW_ADC_IBIAS = test_item.get_32bit_reg_address("RW_ADC_IBIAS", df_register)
    RW_ADC_VREFH_IBIAS = test_item.get_32bit_reg_address("RW_ADC_VREFH_IBIAS", df_register)

    # ---------- 硬件前置初始化(旁路VDD33作VREFH参考 + 开启ADC校准) ----------
    init_pa5_pa6_altfunc14()
    time.sleep(0.4)

    instr.swd_modify_register(RW_ANA_REF_EN, 0x01, 24, 24)
    instr.swd_modify_register(RW_ANA_ADC_EN, 0x01, 19, 19)
    time.sleep(0.08)

    instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
    instr.swd_modify_register(RW_ADC_VREFH_ATP, 0x01, 18, 18)
    time.sleep(0.08)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 6, 6)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 7, 7)
    instr.swd_modify_register(RW_ATP_MUX, 0x06, 0, 4)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)
    instr.swd_modify_register(RW_ADC_VREFH, 0x02, 13, 14)
    instr.swd_modify_register(RW_ADC_VREFH, 0x00, 16, 17)
    instr.swd_modify_register(RW_ANA_VREFH_EN, 0x01, 18, 18)
    instr.swd_modify_register(RW_ADC_VREFH_BYPS, 0x01, 15, 15)
    time.sleep(0.8)
    VREFH_mV = instr.dmm_measure_single_voltage()
    time.sleep(0.8)
    instr.swd_modify_register(RW_ADC_VREFH_BYPS, 0x00, 15, 15)

    vrefh_code, vrefh_byps = 0b11, 0
    print(f"  VREFH = 3V量程 (RW_ADC_VREFH bit13:14=11, BYPS=0), 采样通道 = {adc_channel}")

    instr.swd_modify_register(RCC_ANACKE, 0x01, 0, 0)
    instr.swd_modify_register(ADC_SEQR0, adc_channel, 0, 4)
    instr.swd_modify_register(ADC_TGNR,   1, 0, 7)
    instr.swd_modify_register(ADC_TGSR,   0b0001, 16, 19)
    instr.swd_modify_register(ADC_TGSR,   0b1000,  0,  3)
    instr.swd_write_register(ADC_TCFG,    0x0A4020F4)
    time.sleep(0.08)

    CR_VAL = 0x00000031 | (vrefh_code << 20) | 0x00010000
    if vrefh_byps:
        CR_VAL |= 0x00020000
    instr.swd_write_register(ADC_CR, CR_VAL)
    time.sleep(0.08)

    # ---------- 用万用表实测反馈, 校准DAC Ori码使输出电压逼近目标电压(闭环校准, 不假设固定的Ori-mV换算公式) ----------
    def _set_ori_and_measure(ori_val):
        ori_val = max(0, min(65535, int(round(ori_val))))
        instr.serial_send(f"spi_cmd_set_val Ori {ori_val}\r\n")
        time.sleep(0.4)
        instr.serial_send(f"cmd_gpio_set_595d_test set 38 6\r\n")
        time.sleep(0.8)
        vol = instr.dmm_measure_single_voltage()
        time.sleep(0.4)
        instr.serial_send(f"cmd_gpio_set_595d_test reset\r\n")
        time.sleep(0.4)
        return ori_val, vol

    def _calibrate_ori_for_target(target_mv, ori_guess, tol_mv=1.0, max_iter=15):
        """用两点实测估算斜率(mV/code), 再逐步逼近target_mv, 直到误差<tol_mv或达到最大迭代次数"""
        ori_a, vol_a = _set_ori_and_measure(ori_guess)
        probe_step = max(1, dac_step * 50)
        ori_b, vol_b = _set_ori_and_measure(ori_guess + probe_step)

        ori_cur, vol_cur = ori_b, vol_b
        for _ in range(max_iter):
            err = target_mv - vol_cur
            if abs(err) <= tol_mv:
                break
            slope = (vol_b - vol_a) / (ori_b - ori_a) if (ori_b - ori_a) != 0 else None
            if not slope:
                break
            ori_next = ori_cur + err / slope
            ori_a, vol_a = ori_cur, vol_cur
            ori_cur, vol_cur = _set_ori_and_measure(ori_next)
            ori_b, vol_b = ori_a, vol_a
        return ori_cur, vol_cur

    VDD_H = VREFH_mV / 2.0   # 校准输出电压取ADC量程的一半

    if ori_center is None:
        print(f"  目标中心电压 VDD_H = {VDD_H:.4f} mV, 开始用DMM实测闭环校准DAC Ori码...")
        ori_center, vol_center = _calibrate_ori_for_target(VDD_H, ori_guess=32768)
        print(f"  ✅ 中心点校准完成: Ori={ori_center}, 实测电压={vol_center:.4f} mV (目标{VDD_H:.4f} mV)")
    else:
        print(f"  使用已知中心点 Ori={ori_center} (目标{VDD_H:.4f} mV), 跳过DMM闭环校准")

    target_lo = VDD_H - half_range_mv
    target_hi = VDD_H + half_range_mv
    if ori_lo is None:
        ori_lo, vol_lo = _calibrate_ori_for_target(target_lo, ori_guess=ori_center)
        print(f"  ✅ 下限校准完成: Ori={ori_lo}, 实测电压={vol_lo:.4f} mV (目标{target_lo:.4f} mV)")
    else:
        print(f"  使用已知下限点 Ori={ori_lo} (目标{target_lo:.4f} mV), 跳过DMM闭环校准")
    if ori_hi is None:
        ori_hi, vol_hi = _calibrate_ori_for_target(target_hi, ori_guess=ori_center)
        print(f"  ✅ 上限校准完成: Ori={ori_hi}, 实测电压={vol_hi:.4f} mV (目标{target_hi:.4f} mV)")
    else:
        print(f"  使用已知上限点 Ori={ori_hi} (目标{target_hi:.4f} mV), 跳过DMM闭环校准")

    ori_start, ori_stop = min(ori_lo, ori_hi), max(ori_lo, ori_hi)
    print(f"  DAC扫描范围: Ori {ori_start} ~ {ori_stop} (步进 {dac_step})")
    print(f"  每点ADC采集 {adc_samples} 次取平均")

    idx = 0
    samples = []   # (序号, Ori码, 实测电压v_true, ADC原始值均值raw_avg, ADC转换电压conv_mv, ADC原始值列表raw_list)
    try:
        ori_val = ori_start
        while ori_val <= ori_stop:
            idx += 1
            instr.serial_send(f"spi_cmd_set_val Ori {ori_val}\r\n")
            time.sleep(0.5)

            # 实测电压只测一次
            instr.serial_send(f"cmd_gpio_set_595d_test set 38 6\r\n")
            time.sleep(0.1)
            v_true = instr.dmm_measure_single_voltage()
            time.sleep(0.5)
            instr.serial_send(f"cmd_gpio_set_595d_test reset\r\n")
            time.sleep(0.5)

            # ADC连续采集 adc_samples 次取平均
            raw_list = []
            for _ in range(adc_samples):
                instr.swd_write_register(ADC_OTR, 0x5AA5)
                done = False
                for _ in range(20):
                    ir_val = instr.swd_read_register(ADC_IR)
                    if ir_val is not None and (ir_val & 0x1):
                        done = True
                        break
                if not done:
                    continue
                raw = instr.swd_read_register(ADC_DATA0)
                if raw is None:
                    continue
                raw_list.append(raw & 0xFFF)
                time.sleep(0.001)

            raw_cnt = len(raw_list)
            if raw_cnt == 0:
                print(f"[{idx}] Ori={ori_val} ⚠️ {adc_samples}次ADC采集均失败, 跳过该点")
                ori_val += dac_step
                continue

            raw_avg = sum(raw_list) / raw_cnt
            conv_mv = raw_avg / 4095.0 * VREFH_mV
            samples.append((idx, ori_val, v_true, raw_avg, conv_mv, raw_list))
            print(f"[{idx}] Ori={ori_val:6d}  实测电压Vtrue={v_true:9.4f} mV  "
                  f"ADC原始值均值(共{raw_cnt}次)={raw_avg:8.2f}  ADC转换电压={conv_mv:9.4f} mV")

            ori_val += dac_step
    except KeyboardInterrupt:
        print(f"\n采集已手动停止, 共采集 {idx} 个点")
    finally:
        instr.serial_send(f"cmd_gpio_set_595d_test reset\r\n")
        instr.swd_modify_register(RW_ANA_ADC_EN, 0x00, 19, 19)

        excel_path = None
        if samples:
            try:
                excel_path = save_adc_loop_read_excel(samples, VDD_H, VREFH_mV, adc_channel)
            except Exception as e:
                print(f"  ⚠️ 保存 ADC_LOOP_READ 散点图 Excel 失败: {e}")

        if samples:
            diffs = [s[4] - s[2] for s in samples]
            max_dev = max(abs(d) for d in diffs)
            mean_dev = sum(diffs) / len(diffs)
            print(f"共采集 {len(samples)} 个有效点, ADC转换电压相对实测电压的最大偏差={max_dev:.4f}mV, 平均偏差={mean_dev:+.4f}mV")
            if excel_path:
                print(f"  数据与散点图已保存: {excel_path}")

        save_item_result("ADC_LOOP_READ", True, idx)


def save_adc_loop_read_excel(samples, center_mv, vrefh_mv, adc_channel):
    """将 ADC_LOOP_READ 的采集数据(序号/Ori码/实测电压/ADC均值/ADC转换电压)写入同一个Excel文档, 并绘制散点图"""
    from openpyxl.chart import ScatterChart, Reference, Series
    from openpyxl.chart.marker import Marker
    from openpyxl.drawing.line import LineProperties

    wb = openpyxl.Workbook()
    ws = wb.active
    ws.title = "ADC_LOOP_READ_数据"

    header = ["序号", "DAC设定值Ori(0-65535)", "实测电压Vtrue(mV)", "ADC原始值均值Code(0-4095)", "ADC转换电压(mV)"]
    ws.append(header)
    for idx, ori_val, v_true, raw_avg, conv_mv, raw_list in samples:
        ws.append([idx, ori_val, v_true, raw_avg, conv_mv])

    # 每个点的ADC原始采集值(1024次)明细, 单独一个sheet, 每列=一个DAC设定值点, 每行=第几次采集
    ws_raw = wb.create_sheet("ADC_LOOP_READ_原始采集值")
    ws_raw.append(["采集次序"] + [f"Ori={ori_val}" for idx, ori_val, v_true, raw_avg, conv_mv, raw_list in samples])
    n_samples = len(samples[0][5])
    for i in range(n_samples):
        ws_raw.append([f"第{i+1}次"] + [raw_list[i] for idx, ori_val, v_true, raw_avg, conv_mv, raw_list in samples])

    n = len(samples)
    first_row, last_row = 2, 1 + n

    ws_chart = wb.create_sheet("ADC_LOOP_READ_散点图")

    chart = ScatterChart()
    chart.title = f"ADC转换电压 vs 实测电压 (中心电压VDD_H={center_mv:.4f}mV, 通道={adc_channel})"
    chart.x_axis.title = "序号"
    chart.y_axis.title = "电压(mV)"
    chart.x_axis.axPos = "b"
    chart.y_axis.axPos = "l"
    chart.x_axis.delete = False
    chart.y_axis.delete = False
    chart.style = 13
    chart.width, chart.height = 40, 24

    conv_vals = [s[4] for s in samples]
    vtrue_vals = [s[2] for s in samples]
    y_min, y_max = min(conv_vals + vtrue_vals), max(conv_vals + vtrue_vals)
    y_span = y_max - y_min
    if y_span <= 0:
        y_span = max(abs(y_max), 1.0) * 0.01
    major_unit = y_span / 20.0
    chart.y_axis.majorUnit = major_unit
    chart.y_axis.scaling.min = y_min - major_unit
    chart.y_axis.scaling.max = y_max + major_unit

    idx_ref = Reference(ws, min_col=1, min_row=first_row, max_row=last_row)
    conv_ref = Reference(ws, min_col=5, min_row=first_row, max_row=last_row)
    vtrue_ref = Reference(ws, min_col=3, min_row=first_row, max_row=last_row)

    series_meas = Series(conv_ref, idx_ref, title="ADC转换电压(采样点)")
    series_meas.marker = Marker(symbol="circle", size=2)
    series_meas.graphicalProperties.line.noFill = True
    chart.series.append(series_meas)

    series_vtrue = Series(vtrue_ref, idx_ref, title="实测电压Vtrue(中心线)")
    series_vtrue.marker = Marker(symbol="none")
    series_vtrue.graphicalProperties.line = LineProperties(w=20000, solidFill="C00000")
    series_vtrue.smooth = False
    chart.series.append(series_vtrue)

    ws_chart.add_chart(chart, "B2")

    save_dir = os.path.join(_output_base_dir, "Save_Results", "OCD5010")
    os.makedirs(save_dir, exist_ok=True)
    timestamp = time.strftime("%Y%m%d_%H%M%S")
    excel_path = os.path.join(save_dir, f"adc_loop_read_{timestamp}.xlsx")
    wb.save(excel_path)
    return excel_path


def _adc_calib_set_ori_and_measure(ori_val):
    """设置DAC Ori码并用DMM实测一次输出电压(供闭环校准使用)"""
    ori_val = max(0, min(65535, int(round(ori_val))))
    instr.serial_send(f"spi_cmd_set_val Ori {ori_val}\r\n")
    time.sleep(0.4)
    instr.serial_send(f"cmd_gpio_set_595d_test set 38 6\r\n")
    time.sleep(0.8)
    vol = instr.dmm_measure_single_voltage()
    time.sleep(0.4)
    instr.serial_send(f"cmd_gpio_set_595d_test reset\r\n")
    time.sleep(0.4)
    return ori_val, vol


def _adc_calib_ori_for_target(target_mv, ori_guess, dac_step, tol_mv=1.0, max_iter=15):
    """用两点实测估算斜率(mV/code), 再逐步逼近target_mv, 直到误差<tol_mv或达到最大迭代次数"""
    ori_a, vol_a = _adc_calib_set_ori_and_measure(ori_guess)
    probe_step = max(1, dac_step * 50)
    ori_b, vol_b = _adc_calib_set_ori_and_measure(ori_guess + probe_step)

    ori_cur, vol_cur = ori_b, vol_b
    for _ in range(max_iter):
        err = target_mv - vol_cur
        if abs(err) <= tol_mv:
            break
        slope = (vol_b - vol_a) / (ori_b - ori_a) if (ori_b - ori_a) != 0 else None
        if not slope:
            break
        ori_next = ori_cur + err / slope
        ori_a, vol_a = ori_cur, vol_cur
        ori_cur, vol_cur = _adc_calib_set_ori_and_measure(ori_next)
        ori_b, vol_b = ori_a, vol_a
    return ori_cur, vol_cur


def _adc_sweep_core(dac_step, center_mv, half_range_mv, adc_samples, vrefh_mv_conv,
                     ADC_OTR, ADC_IR, ADC_DATA0, label="",
                     ori_lo=None, ori_center=None, ori_hi=None):
    """按已知/闭环校准得到的中心+上下限Ori码, 按dac_step步进扫描, 每点ADC采集adc_samples次取平均。
    若传入 ori_lo/ori_center/ori_hi(实测已知固定值), 则跳过DMM闭环校准直接使用, 节省测试时间;
    否则现场用DMM实测反馈闭环校准(两点估算斜率+逐步逼近)求出。
    返回samples列表: [(序号, Ori码, 实测电压v_true, ADC原始值均值raw_avg, ADC转换电压conv_mv, ADC原始值列表raw_list), ...]
    """
    if ori_center is None:
        print(f"  [{label}] 目标中心电压 = {center_mv:.4f} mV, 开始DMM闭环校准DAC Ori码...")
        ori_center, vol_center = _adc_calib_ori_for_target(center_mv, ori_guess=32768, dac_step=dac_step)
        print(f"  [{label}] ✅ 中心点校准: Ori={ori_center}, 实测={vol_center:.4f} mV (目标{center_mv:.4f} mV)")
    else:
        print(f"  [{label}] 使用已知中心点 Ori={ori_center} (目标{center_mv:.4f} mV), 跳过DMM闭环校准")

    target_lo = center_mv - half_range_mv
    target_hi = center_mv + half_range_mv
    if ori_lo is None:
        ori_lo, vol_lo = _adc_calib_ori_for_target(target_lo, ori_guess=ori_center, dac_step=dac_step)
        print(f"  [{label}] ✅ 下限校准: Ori={ori_lo}, 实测={vol_lo:.4f} mV (目标{target_lo:.4f} mV)")
    else:
        print(f"  [{label}] 使用已知下限点 Ori={ori_lo} (目标{target_lo:.4f} mV), 跳过DMM闭环校准")
    if ori_hi is None:
        ori_hi, vol_hi = _adc_calib_ori_for_target(target_hi, ori_guess=ori_center, dac_step=dac_step)
        print(f"  [{label}] ✅ 上限校准: Ori={ori_hi}, 实测={vol_hi:.4f} mV (目标{target_hi:.4f} mV)")
    else:
        print(f"  [{label}] 使用已知上限点 Ori={ori_hi} (目标{target_hi:.4f} mV), 跳过DMM闭环校准")

    ori_start, ori_stop = min(ori_lo, ori_hi), max(ori_lo, ori_hi)
    print(f"  [{label}] DAC扫描范围: Ori {ori_start} ~ {ori_stop} (步进 {dac_step}), 每点ADC采集{adc_samples}次")

    idx = 0
    samples = []
    ori_val = ori_start
    while ori_val <= ori_stop:
        idx += 1
        instr.serial_send(f"spi_cmd_set_val Ori {ori_val}\r\n")
        time.sleep(0.4)

        instr.serial_send(f"cmd_gpio_set_595d_test set 38 6\r\n")
        time.sleep(0.8)
        v_true = instr.dmm_measure_single_voltage()
        time.sleep(0.4)
        instr.serial_send(f"cmd_gpio_set_595d_test reset\r\n")
        time.sleep(0.4)

        raw_list = []
        for _ in range(adc_samples):
            instr.swd_write_register(ADC_OTR, 0x5AA5)
            done = False
            for _ in range(20):
                ir_val = instr.swd_read_register(ADC_IR)
                if ir_val is not None and (ir_val & 0x1):
                    done = True
                    break
            if not done:
                continue
            raw = instr.swd_read_register(ADC_DATA0)
            if raw is None:
                continue
            raw_list.append(raw & 0xFFF)

        raw_cnt = len(raw_list)
        if raw_cnt == 0:
            print(f"  [{label}][{idx}] Ori={ori_val} ⚠️ {adc_samples}次ADC采集均失败, 跳过该点")
            ori_val += dac_step
            continue

        raw_avg = sum(raw_list) / raw_cnt
        conv_mv = raw_avg / 4095.0 * vrefh_mv_conv
        samples.append((idx, ori_val, v_true, raw_avg, conv_mv, raw_list))
        print(f"  [{label}][{idx}] Ori={ori_val:6d}  Vtrue={v_true:9.4f} mV  "
              f"ADC均值(共{raw_cnt}次)={raw_avg:8.2f}  ADC转换电压={conv_mv:9.4f} mV")

        ori_val += dac_step

    return samples


def save_adc_range_ibias_excel(range_name, center_mv, configs):
    """将某量程下各电流档的ADC扫描结果保存到同一个Excel, 每档3个sheet:
    <label>_数据 / <label>_原始采集值(采集次数为列) / <label>_散点图
    configs: [{"label":str, "samples":[...], "vrefh_mv":float}, ...]
    """
    from openpyxl.chart import ScatterChart, Reference, Series
    from openpyxl.chart.marker import Marker
    from openpyxl.drawing.line import LineProperties

    wb = openpyxl.Workbook()
    wb.remove(wb.active)

    for cfg in configs:
        label = cfg["label"]
        samples = cfg["samples"]
        vrefh_mv = cfg["vrefh_mv"]
        if not samples:
            continue

        ws = wb.create_sheet(f"{label}_数据"[:31])
        ws.append(["序号", "DAC设定值Ori(0-65535)", "实测电压Vtrue(mV)", "ADC原始值均值Code(0-4095)", "ADC转换电压(mV)"])
        for idx, ori_val, v_true, raw_avg, conv_mv, raw_list in samples:
            ws.append([idx, ori_val, v_true, raw_avg, conv_mv])
        n = len(samples)
        first_row, last_row = 2, 1 + n

        ws_raw = wb.create_sheet(f"{label}_原始采集值"[:31])
        ws_raw.append(["采集次序"] + [f"Ori={ori_val}" for idx, ori_val, v_true, raw_avg, conv_mv, raw_list in samples])
        n_samples = len(samples[0][5])
        for i in range(n_samples):
            ws_raw.append([f"第{i+1}次"] + [raw_list[i] for idx, ori_val, v_true, raw_avg, conv_mv, raw_list in samples])

        ws_chart = wb.create_sheet(f"{label}_散点图"[:31])
        chart = ScatterChart()
        chart.title = f"{range_name} {label} ADC转换电压 vs 实测电压 (中心{center_mv:.4f}mV, VREFH基准={vrefh_mv:.4f}mV)"
        chart.x_axis.title = "序号"
        chart.y_axis.title = "电压(mV)"
        chart.x_axis.axPos = "b"
        chart.y_axis.axPos = "l"
        chart.x_axis.delete = False
        chart.y_axis.delete = False
        chart.style = 13
        chart.width, chart.height = 40, 24

        conv_vals = [s[4] for s in samples]
        vtrue_vals = [s[2] for s in samples]
        y_min, y_max = min(conv_vals + vtrue_vals), max(conv_vals + vtrue_vals)
        y_span = y_max - y_min
        if y_span <= 0:
            y_span = max(abs(y_max), 1.0) * 0.01
        major_unit = y_span / 20.0
        chart.y_axis.majorUnit = major_unit
        chart.y_axis.scaling.min = y_min - major_unit
        chart.y_axis.scaling.max = y_max + major_unit

        idx_ref = Reference(ws, min_col=1, min_row=first_row, max_row=last_row)
        conv_ref = Reference(ws, min_col=5, min_row=first_row, max_row=last_row)
        vtrue_ref = Reference(ws, min_col=3, min_row=first_row, max_row=last_row)

        series_meas = Series(conv_ref, idx_ref, title="ADC转换电压(采样点)")
        series_meas.marker = Marker(symbol="circle", size=2)
        series_meas.graphicalProperties.line.noFill = True
        chart.series.append(series_meas)

        series_vtrue = Series(vtrue_ref, idx_ref, title="实测电压Vtrue(中心线)")
        series_vtrue.marker = Marker(symbol="none")
        series_vtrue.graphicalProperties.line = LineProperties(w=20000, solidFill="C00000")
        series_vtrue.smooth = False
        chart.series.append(series_vtrue)

        ws_chart.add_chart(chart, "B2")

    save_dir = os.path.join(_output_base_dir, "Save_Results", "OCD5010")
    os.makedirs(save_dir, exist_ok=True)
    timestamp = time.strftime("%Y%m%d_%H%M%S")
    safe_range = range_name.replace(".", "p").replace(" ", "")
    excel_path = os.path.join(save_dir, f"adc_range_{safe_range}_{timestamp}.xlsx")
    wb.save(excel_path)
    return excel_path


@test_case
def ADC_MULTI_RANGE_IBIAS_TEST(adc_channel=20, dac_step=3, half_range_mv=1.2, adc_samples=1024, center_mv=1000.0,
                                ori_lo=15883, ori_center=15902, ori_hi=15921):
    """ADC 多量程 x 多电流档 测试 —— 沿用 ADC_LOOP_READ 的ADC采集逻辑

    量程(5个):
        VDD:  RW_ADC_VREFH_BYPS bit15=1
        1.5V/2V/2.5V/3V: RW_ADC_VREFH_BYPS bit15=0, RW_ADC_VREFH bit13:14=00/01/10/11

    电流档:
        VDD量程:      RW_ADC_IBIAS bit11=0/1, 共2档
        其余4个量程:  RW_ADC_IBIAS bit11=0/1 (2档) x RW_ADC_VREFH_IBIAS bit16:17=00/01/10/11 (4档), 共8档

    每写一次电流档/量程寄存器都会回读确认。每个电流档统一基准中心电压 center_mv(默认1000mV),
    扫描范围 center_mv±half_range_mv(默认1.2mV), 每点ADC采集 adc_samples 次取平均。
    ★ 经实测确认, 各量程/电流档下DAC Ori码与输出电压的对应关系一致, 故不再逐档用DMM闭环校准,
    统一固定使用: 下限Ori=15883(998.8mV) / 基准Ori=15902(1000mV) / 上限Ori=15921(1001.2mV)。
    若需要重新校准, 传入 ori_lo/ori_center/ori_hi=None 即可恢复逐档DMM闭环校准。
    每个量程的所有电流档结果保存到同一个Excel文档(每档3个sheet: xxx_数据/xxx_原始采集值/xxx_散点图)。

    参数:
        adc_channel: 采样通道号(0~30, 默认20)
        dac_step: DAC Ori码步进值(默认3)
        half_range_mv: 以center_mv为中心的扫描半范围(mV, 默认1.2)
        adc_samples: 每个DAC点ADC采集次数取平均(默认1024)
        center_mv: 统一基准中心电压(mV, 默认1000)
        ori_lo/ori_center/ori_hi: 实测已确认的固定Ori码(默认15883/15902/15921), 传None则改为现场DMM闭环校准
    """
    RCC_ANACKE  = 0x4002005C
    ADC_CR      = 0x40012460
    ADC_OTR     = 0x40012464
    ADC_TGSR    = 0x40012448
    ADC_TGNR    = 0x4001244C
    ADC_SEQR0   = 0x40012450
    ADC_TCFG    = 0x40012468
    ADC_IR      = 0x40012474
    ADC_DATA0   = 0x40012404

    PA_ANA_EN = test_item.get_32bit_reg_address("PA_ANA_EN", df_register)
    RW_ANA_REF_EN = test_item.get_32bit_reg_address("RW_ANA_REF_EN", df_register)
    RW_ADC_VREFH_ATP = test_item.get_32bit_reg_address("RW_ADC_VREFH_ATP", df_register)
    RW_ATP_MUX = test_item.get_32bit_reg_address("RW_ATP_MUX", df_register)
    RW_ATP_MUX_EN = test_item.get_32bit_reg_address("RW_ATP_MUX_EN", df_register)
    RW_ADC_VREFH = test_item.get_32bit_reg_address("RW_ADC_VREFH", df_register)
    RW_ANA_VREFH_EN = test_item.get_32bit_reg_address("RW_ANA_VREFH_EN", df_register)
    RW_ADC_VREFH_BYPS = test_item.get_32bit_reg_address("RW_ADC_VREFH_BYPS", df_register)
    RW_ANA_ADC_EN = test_item.get_32bit_reg_address("RW_ANA_ADC_EN", df_register)
    RW_ADC_IBIAS = test_item.get_32bit_reg_address("RW_ADC_IBIAS", df_register)
    RW_ADC_VREFH_IBIAS = test_item.get_32bit_reg_address("RW_ADC_VREFH_IBIAS", df_register)

    # ---------- 硬件公共前置初始化 ----------
    init_pa5_pa6_altfunc14()
    time.sleep(0.4)
    instr.swd_modify_register(RW_ANA_REF_EN, 0x01, 24, 24)
    instr.swd_modify_register(RW_ANA_ADC_EN, 0x01, 19, 19)
    time.sleep(0.08)
    instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
    instr.swd_modify_register(RW_ADC_VREFH_ATP, 0x01, 18, 18)
    time.sleep(0.08)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 6, 6)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 7, 7)
    instr.swd_modify_register(RW_ATP_MUX, 0x06, 0, 4)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)
    instr.swd_modify_register(RW_ANA_VREFH_EN, 0x01, 18, 18)

    instr.swd_modify_register(RCC_ANACKE, 0x01, 0, 0)
    instr.swd_modify_register(ADC_SEQR0, adc_channel, 0, 4)
    instr.swd_modify_register(ADC_TGNR,   1, 0, 7)
    instr.swd_modify_register(ADC_TGSR,   0b0001, 16, 19)
    instr.swd_modify_register(ADC_TGSR,   0b1000,  0,  3)
    instr.swd_write_register(ADC_TCFG,    0x0A4020F4)
    time.sleep(0.08)

    def _confirm_bits(addr, value, start_bit, end_bit, label):
        instr.swd_modify_register(addr, value, start_bit, end_bit)
        time.sleep(0.04)
        reg_val = instr.swd_read_register(addr)
        print(f"adc_reg_value:{reg_val:08X}")
        mask = (1 << (end_bit - start_bit + 1)) - 1
        if reg_val is None:
            print(f"    ⚠️ {label} 回读失败")
            return False
        actual = (reg_val >> start_bit) & mask
        ok = (actual == value)
        flag = "✅" if ok else "⚠️ 不匹配"
        print(f"    {flag} {label} 写入={value}, 回读={actual}")
        return ok

    def _set_adc_cr(vrefh_code, vrefh_byps):
        cr_val = 0x00000031 | (vrefh_code << 20) | 0x00010000
        if vrefh_byps:
            cr_val |= 0x00020000
        instr.swd_write_register(ADC_CR, cr_val)
        time.sleep(0.08)

    all_excel_paths = []

    # ==================== 量程1: VDD (RW_ADC_VREFH_BYPS bit15=1) ====================
    print("\n" + "=" * 60)
    print("  量程 VDD (RW_ADC_VREFH_BYPS bit15=1)")
    print("=" * 60)
    _confirm_bits(RW_ADC_VREFH_BYPS, 0x01, 15, 15, "RW_ADC_VREFH_BYPS[15](VDD量程选择)")
    instr.swd_modify_register(RW_ADC_VREFH, 0x02, 13, 14)
    instr.swd_modify_register(RW_ADC_VREFH, 0x00, 16, 17)
    time.sleep(0.8)
    vdd_vrefh_mv = instr.dmm_measure_single_voltage()
    time.sleep(0.16)
    print(f"  实测 VDD (VREFH基准) = {vdd_vrefh_mv:.4f} mV")

    vdd_configs = []
    for ibias_bit in (0, 1):
        label = f"IBIAS{ibias_bit}"
        print(f"\n  -- VDD量程 电流档 {label} (RW_ADC_IBIAS bit11={ibias_bit}) --")
        _confirm_bits(RW_ADC_IBIAS, ibias_bit, 11, 11, "RW_ADC_IBIAS[11]")
        _set_adc_cr(vrefh_code=0b11, vrefh_byps=1)
        samples = _adc_sweep_core(dac_step, center_mv, half_range_mv, adc_samples, vdd_vrefh_mv,
                                    ADC_OTR, ADC_IR, ADC_DATA0, label=f"VDD/{label}",
                                    ori_lo=ori_lo, ori_center=ori_center, ori_hi=ori_hi)
        vdd_configs.append({"label": label, "samples": samples, "vrefh_mv": vdd_vrefh_mv})

    if any(c["samples"] for c in vdd_configs):
        try:
            p = save_adc_range_ibias_excel("VDD", center_mv, vdd_configs)
            all_excel_paths.append(p)
            print(f"  ✅ VDD量程数据已保存: {p}")
        except Exception as e:
            print(f"  ⚠️ VDD量程Excel保存失败: {e}")

    # ==================== 量程2~5: 1.5V/2V/2.5V/3V (RW_ADC_VREFH_BYPS bit15=0) ====================
    ranges = [
        {"name": "1.5V", "code": 0b00, "nominal_mv": 1500.0},
        {"name": "2V",   "code": 0b01, "nominal_mv": 2000.0},
        {"name": "2.5V", "code": 0b10, "nominal_mv": 2500.0},
        {"name": "3V",   "code": 0b11, "nominal_mv": 3000.0},
    ]

    for rng in ranges:
        print("\n" + "=" * 60)
        print(f"  量程 {rng['name']} (RW_ADC_VREFH_BYPS bit15=0, RW_ADC_VREFH bit13:14={rng['code']:02b})")
        print("=" * 60)
        _confirm_bits(RW_ADC_VREFH_BYPS, 0x00, 15, 15, "RW_ADC_VREFH_BYPS[15](量程选择)")
        _confirm_bits(RW_ADC_VREFH, rng["code"], 13, 14, "RW_ADC_VREFH[13:14](量程码)")
        vrefh_mv_conv = rng["nominal_mv"]
        print(f"  该量程为内部固定基准, 采用标称值 {vrefh_mv_conv:.1f} mV 作为ADC转换基准")

        rng_configs = []
        for ibias_bit in (0, 1):
            _confirm_bits(RW_ADC_IBIAS, ibias_bit, 11, 11, "RW_ADC_IBIAS[11]")
            for vi_code in (0b00, 0b01, 0b10, 0b11):
                label = f"IB{ibias_bit}_VI{vi_code}"
                print(f"\n  -- {rng['name']} 电流档 {label} "
                      f"(RW_ADC_IBIAS bit11={ibias_bit}, RW_ADC_VREFH_IBIAS bit16:17={vi_code:02b}) --")
                _confirm_bits(RW_ADC_VREFH_IBIAS, vi_code, 16, 17, "RW_ADC_VREFH_IBIAS[16:17]")
                _set_adc_cr(vrefh_code=rng["code"], vrefh_byps=0)
                samples = _adc_sweep_core(dac_step, center_mv, half_range_mv, adc_samples, vrefh_mv_conv,
                                            ADC_OTR, ADC_IR, ADC_DATA0, label=f"{rng['name']}/{label}",
                                            ori_lo=ori_lo, ori_center=ori_center, ori_hi=ori_hi)
                rng_configs.append({"label": label, "samples": samples, "vrefh_mv": vrefh_mv_conv})

        if any(c["samples"] for c in rng_configs):
            try:
                p = save_adc_range_ibias_excel(rng["name"], center_mv, rng_configs)
                all_excel_paths.append(p)
                print(f"  ✅ {rng['name']}量程数据已保存: {p}")
            except Exception as e:
                print(f"  ⚠️ {rng['name']}量程Excel保存失败: {e}")

    instr.swd_modify_register(RW_ANA_ADC_EN, 0x00, 19, 19)

    print("\n" + "=" * 60)
    print(f"  全部量程测试完成, 共生成 {len(all_excel_paths)} 个Excel文件:")
    for p in all_excel_paths:
        print(f"    {p}")
    print("=" * 60)

    save_item_result("ADC_MULTI_RANGE_IBIAS_TEST", True, len(all_excel_paths))


@test_case
def ADC_DNL_Test(adc_channel=20):
    """ADC DNL 测试 —— 软件 ADC 循环单通道采样（交互量程/通道 + 外部 DAC 电压）

    实现：交互选择参考电压量程(CR[21:20])与采样通道(SEQR0[4:0]) + ADC 初始化(4步)
    + 外部 DAC 设置输入电压(串口: cmd_gpio_set_595d_test set 38 7 开通路 + spi_cmd_set_val mV)
    + 软件触发(OTR=0x5AA5) + 轮询完成(IR.S0CDIF) + 读 DATA[0] 的循环采样。
    暂不包含：DNL 判定、DMM 实测、结果保存等（后续步骤逐步补充）。
    寄存器地址已与 SFR / ADC_Reg.h / ODS_5010.h 三方核对；CR 位域按新版 UM 260720。

    参数:
        adc_channel: 采样通道号(0~30, 默认18)，交互输入为空时使用此值
    """
    # ---------- 寄存器地址 ----------
    RCC_ANACKE  = 0x4002005C   # 模拟时钟使能: bit0 ADCCKE=1（必须）
    ANAC_ENCR   = 0x40007800   # 模拟电源/基准使能(可选, 采样全0时可打开)
    ADC_CR      = 0x40012460   # 控制(新版UM 260720): [0]ADC_EN [16]VREFH_EN [21:20]VREFH [5:4]CLK
    ADC_OTR     = 0x40012464   # 软件触发: 写 0x5AA5 产生 ADC_SOC
    ADC_TGSR    = 0x40012448   # 触发源: [19:16]TRIG_EN [3:0]TRIG_SEL0(=8 软件)
    ADC_TGNR    = 0x4001244C   # 组0通道数: [7:0]TGN0
    ADC_SEQR0   = 0x40012450   # 通道选择: [4:0]SEQ0
    ADC_TCFG    = 0x40012468   # 时序: 复位默认(INIT=0x40 不能为0)
    ADC_IR      = 0x40012474   # 状态(只读): [31]RDY [0]S0CDIF
    ADC_DATA0   = 0x40012404   # 序列0转换结果: [11:0]
    ADC_CALIBDAT= 0x40012484   # 校准结果
    ADC_CALIB   = 0x40012480   # 校准配置: [27:16]GAIN(复位0x800) [11:0]OFFSET

    # CHSEQ 通道名映射（新版 UM 260720 表 19-2）
    CH_NAME = {0: "CSA_VSEN", 1: "CSA_VSEN", 2: "CSA_VSEN", 3: "CSA_VLPF",
               4: "PB12", 5: "PB13", 6: "PB14", 7: "PC3", 8: "PC4", 9: "PC5",
               10: "PC6", 11: "PC7", 12: "PC8", 13: "VDD33_VSEN", 14: "TJ_REF",
               15: "VREF_1P20", 16: "PA0", 17: "PA1", 18: "PA2", 19: "PA3",
               20: "PA4", 21: "PA5", 22: "PA6", 23: "PA7", 24: "PA8", 25: "PA9",
               26: "PA10", 27: "PA11", 28: "PA12", 29: "PA13", 30: "PA14"}

    # ADC 通道(CHSEQ) → bench_pin_bit 序号（来源 ADC通道_socket_bench.xlsx）
    # 用于 cmd_gpio_set_595d_test set <bench_pin_bit> <DAC编号> 开启外部 DAC 通路
    BENCH_PIN_BIT = {18: 5, 21: 15, 22: 31, 23: 11, 24: 4, 25: 20, 26: 3,
                     27: 27, 28: 12, 29: 26, 30: 19, 6: 13, 7: 29, 8: 22,
                     9: 28, 10: 21}

    print("=" * 58)
    print("  ADC 软件采样验证 —— 单通道循环采样")
    print("=" * 58)


    """new add"""
    PA_ANA_EN = test_item.get_32bit_reg_address("PA_ANA_EN", df_register)
    RW_ANA_REF_EN = test_item.get_32bit_reg_address("RW_ANA_REF_EN", df_register)
    RW_ADC_VREFH_ATP = test_item.get_32bit_reg_address("RW_ADC_VREFH_ATP", df_register)
    RW_ATP_MUX = test_item.get_32bit_reg_address("RW_ATP_MUX", df_register)
    RW_ATP_MUX_EN = test_item.get_32bit_reg_address("RW_ATP_MUX_EN", df_register)
    RW_ADC_VREFH = test_item.get_32bit_reg_address("RW_ADC_VREFH", df_register)
    RW_ANA_VREFH_EN = test_item.get_32bit_reg_address("RW_ANA_VREFH_EN", df_register)
    RW_ADC_VREFH_BYPS = test_item.get_32bit_reg_address("RW_ADC_VREFH_BYPS", df_register)
    RW_ANA_ADC_EN = test_item.get_32bit_reg_address("RW_ANA_ADC_EN", df_register)
    RW_ADC_BUF_TP = test_item.get_32bit_reg_address("RW_ADC_BUF_TP", df_register)
    RW_ATP_TG_EN = test_item.get_32bit_reg_address("RW_ATP_TG_EN", df_register)

    # 测试前置: 配置 PA5/PA6 复用 ALT_FUNC_14 (开 GPIO 时钟 + 输出方向 + AF14)
    init_pa5_pa6_altfunc14()
    time.sleep(0.5)

    #使能相关模块
    instr.swd_modify_register(RW_ANA_REF_EN, 0x01, 24, 24)
    instr.swd_modify_register(RW_ANA_ADC_EN, 0x01, 19, 19)
    time.sleep(0.1)

    instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
    instr.swd_modify_register(RW_ADC_VREFH_ATP, 0x01, 18, 18)

    time.sleep(0.1)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 6, 6)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 7, 7)
    instr.swd_modify_register(RW_ATP_MUX, 0x06, 0, 4)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)
    instr.swd_modify_register(RW_ADC_VREFH, 0x02, 13, 14)
    instr.swd_modify_register(RW_ADC_VREFH, 0x00, 16, 17)
    instr.swd_modify_register(RW_ANA_VREFH_EN, 0x01, 18, 18)
    #instr.swd_modify_register(RW_ADC_BUF_TP, 0x00, 12, 12)
    #instr.swd_modify_register(RW_ATP_TG_EN, 0x01, 6, 6)
    #time.sleep(0.1)
    instr.swd_modify_register(RW_ADC_VREFH_BYPS, 0x01, 15, 15)
    time.sleep(1)
    vdd_vol = instr.dmm_measure_single_voltage()
    time.sleep(1)
    instr.swd_modify_register(RW_ADC_VREFH_BYPS, 0x00, 15, 15)
    #time.sleep(1)
    #instr.swd_modify_register(RW_ATP_MUX, 0b11111, 0, 4)
    #time.sleep(1)

    # ---------- 0. 交互式选择参考量程与采样通道 ----------
    VREFH_OPT = {  # 选项: (VREFH码[21:20], 量程mV, 是否旁路VDD33)
        "1": (0b00, 1500, 0), "2": (0b01, 2000, 0),
        "3": (0b10, 2500, 0), "4": (0b11, 3000, 0),
        "5": (0b11, vdd_vol, 1),   # 旁路 VDD33(≈3.3V): VREFH_BYPS[17]=1, 量程=VDD33
    }
    print("\n请选择 ADC 参考电压量程 (VREFH):")
    print(f"  1. 1.5V   2. 2.0V   3. 2.5V   4. 3.0V   5. 旁路VDD33({vdd_vol:.4f} mV)")
    vrefh_code, VREFH_mV, vrefh_byps = VREFH_OPT.get(input("请输入选项(1/2/3/4/5): ").strip(), (0b11, 3000, 0))
    mode = "旁路VDD33" if vrefh_byps else "内部VREFH"
    print(f"  已选择量程: {VREFH_mV} mV ({mode})")

    try:
        ch = int(input(f"请输入 ADC 采样通道号 (0~30, 默认{adc_channel}): ").strip() or str(adc_channel))
        ch = 0 if ch < 0 else (30 if ch > 30 else ch)
    except ValueError:
        ch = adc_channel
    ADC_CH = ch
    ch_name = CH_NAME.get(ADC_CH, "ADC_CH%02d" % ADC_CH)
    print(f"  已选择通道: ADC_CH{ADC_CH} ({ch_name})")
    """
    这里需要注意一下，如果使用了连接FPC软线的socket板子，那GPIO2（ADC_IN18)默认是接了DAC_X1的，不需要配置
    如果是Htol的板子，那默认接的是GPIO4（ADC_IN20)DAC_X1的，不需要配置。
    这里默认是接了连接FPC软线的socket板子。选择了GPIO2（ADC_IN18)作为采样通道，其他的均需配置对应的继电器
    """

    # ---------- 1. 初始化 ADC 通路（4 步，CPU halt 下 SWD 可完成） ----------
    instr.swd_modify_register(RCC_ANACKE, 0x01, 0, 0)            # ① 使能 ADC 时钟 ADCCKE=1
    instr.swd_modify_register(ADC_SEQR0, ADC_CH, 0, 4)           # ② SEQ0 = 所选通道
    instr.swd_modify_register(ADC_TGNR,   1, 0, 7)               # ② TGN0 = 1（组0 1个通道）
    instr.swd_modify_register(ADC_TGSR,   0b0001, 16, 19)        # ② TRIG_EN = 仅组0
    instr.swd_modify_register(ADC_TGSR,   0b1000,  0,  3)        # ② TRIG_SEL0 = 8 (软件)
    instr.swd_write_register(ADC_TCFG,    0x0A4020F4)            # ② 复位默认时序(INIT≠0)
    time.sleep(0.1)
    # ③ 按所选量程写 CR：ADC_EN + CLK=2M + VREFH_EN(bit16) + VREFH码 + (旁路时 VREFH_BYPS[17]=1)
    CR_VAL = 0x00000031 | (vrefh_code << 20) | 0x00010000 | (1 << 1) # ADC_EN=1 + CLK=2M + VREFH_EN=1 + VREFH码
    if vrefh_byps:
        CR_VAL |= 0x00020000                    # VREFH_BYPS=1 → 量程=VDD33(≈3.3V)
    instr.swd_write_register(ADC_CR, CR_VAL)
    # (可选) 模拟电源域上电, 参考 pga_demo
    # instr.swd_write_register(ANAC_ENCR, 0x03840001)
    
    time.sleep(0.1)
    print(f"ADC_CALIB = 0x{instr.swd_read_register(ADC_CALIB):08X}")
    # ---------- 2. 电压模式选择(与2810一致) + 打开外部 DAC 通路 ----------
    # 打开通路：cmd_gpio_set_595d_test set 后每个参数为一个独立通路编号，可一次打开多条：
    #   38=万用表测量通路；7=DAC2→母线；<bench_pin_bit>=母线→所选 ADC 通道

    DAC_GAIN = 1
    if DAC_GAIN == 1:
        dac_bit = BENCH_PIN_BIT.get(ADC_CH)
        if dac_bit is not None:
            instr.serial_send(f"cmd_gpio_set_595d_test set 6 {dac_bit}\r\n")
        else:
            print(f"⚠️ ADC_CH{ADC_CH}({ch_name}) 无 bench_pin_bit 映射，未开启母线→通道通路。")
        time.sleep(0.2)
    elif DAC_GAIN == 2:
        dac_bit = BENCH_PIN_BIT.get(ADC_CH)
        if dac_bit is not None :
            instr.serial_send(f"cmd_gpio_set_595d_test set 7 {dac_bit}\r\n")
        else:
            print(f"⚠️ ADC_CH{ADC_CH}({ch_name}) 无 bench_pin_bit 映射，未开启母线→通道通路。")
        time.sleep(0.2)

    else:
        print("input error")
    
    
    """
    # DAC1 增益：输出 = 设置电压×2（DAC1 为×1）；目标输入电压上限 = 量程 95%
    DAC_GAIN = 1
    vin_max = int(VREFH_mV)
    """
    
    vin_max = int(VREFH_mV)

    print("\n请选择 ADC DNL 测试电压模式:")
    print("1. 固定测试电压")
    print("2. 随机测试电压 (10组循环)")
    print("3. 线性扫描测试电压")
    choice = input("请输入选项 (1/2/3): ").strip()
    random.seed(42)

    # 总采样点数：每个电压采样一次，连续采样 N 次（无"点"概念）
    try:
        N = int(input("请输入总采样点数 (默认 100): ").strip() or "100")
    except ValueError:
        N = 100
    N = max(1, min(N, 5000))

    if choice == '1':
        # 固定测试电压：固定电压值，采样 N 次
        try:
            fix_mv = int(input("请输入固定测试电压 (mV, 默认1000): ").strip() or "1000")
        except ValueError:
            fix_mv = 1000
        fix_mv = max(0, min(fix_mv, vin_max))
        vin_seq = [fix_mv] * N
    elif choice == '2':
        # 随机测试电压：10 组随机电压循环，每个电压采样一次，连续到 N 次
        rnd_list = [int(round(random.uniform(vin_max * 0.05, vin_max), -1)) for _ in range(10)]
        vin_seq = [rnd_list[i % 10] for i in range(N)]
    else:
        # 线性扫描：起始/终点/步进生成电压序列，循环采样到 N 次
        try:
            v_start = int(input("请输入起始电压 (mV, 默认150): ").strip() or "150")
            v_stop  = int(input(f"请输入终点电压 (mV, 默认{vin_max}): ").strip() or str(vin_max))
            step    = int(input("请输入步进值 (mV, 默认100): ").strip() or "100")
        except ValueError:
            v_start, v_stop, step = 150, vin_max, 100
        v_start = max(0, min(v_start, vin_max))
        v_stop  = max(0, min(v_stop, vin_max))
        if v_stop < v_start:
            v_start, v_stop = v_stop, v_start
        sweep = list(range(v_start, v_stop + 1, max(1, step)))
        vin_seq = [sweep[i % len(sweep)] for i in range(N)]
    print(f"  (量程 {VREFH_mV/1000.0:.1f}V, 目标输入上限 {vin_max} mV; DAC2 实际输出 = 设置值×2; 总采样 {N} 次)")

    # 实测间隔：万用表每 N 点实测一次（0=不实测只换算[快速]，1=每点，默认0）
    # ⚠️ DMM 实测(VISA通信)每次可达数百ms，为主要耗时项，默认关闭以保采样速度
    try:
        dmm_interval = int(input("请输入万用表实测间隔 (0=不实测[快速], 1=每点, 默认0): ").strip() or "0")
    except ValueError:
        dmm_interval = 0
    dmm_interval = max(0, dmm_interval)
    last_vol = None



    # ---------- 3. 连续采样（每个电压采样一次，共 N 次） ----------
    all_raw = []
    for i, vin in enumerate(vin_seq):
        # 设置 DAC2：目标输入电压 / 2（2 倍增益），上限 VREFH×0.95/2
        dac_mv = max(0, min(vin // DAC_GAIN, int(VREFH_mV / DAC_GAIN)))
        instr.serial_send(f"spi_cmd_set_val mV {dac_mv}\r\n")   # 只发送不等待响应(避免串口读阻塞)
        time.sleep(0.5)                               # DAC 输出/母线建立
        # 万用表实测（按间隔；间隔>1 时中间点沿用最近实测值）
        if dmm_interval > 0 and (i % dmm_interval == 0):
            instr.serial_send(f"cmd_gpio_set_595d_test set 38 6\r\n")
            time.sleep(1)
            last_vol = instr.dmm_measure_single_voltage()
            time.sleep(1)
            instr.serial_send(f"cmd_gpio_set_595d_test reset\r\n")
            time.sleep(1)
        vol = last_vol
        instr.swd_write_register(ADC_OTR, 0x5AA5)      # 软件触发一次
        time.sleep(0.5)                              # 等转换完成(2M sps)后再轮询, 减少 J-Link 往返

        done = False
        for _ in range(20):                            # 轮询 IR.S0CDIF (bit0), 超时保护
            ir_val = instr.swd_read_register(ADC_IR)
            if ir_val is not None and (ir_val & 0x1):
                done = True
                break
            time.sleep(0.05)
        if not done:
            print(f"  [{i+1:>4}/{N}] ⚠️ 转换触发超时")
            continue
        raw = instr.swd_read_register(ADC_DATA0)
        time.sleep(0.5)

        """
        done = False
        for _ in range(100):                            # 轮询 IR.S0CDIF (bit0), 超时保护
            ir_val = instr.swd_read_register(ADC_IR)
            if ir_val is not None and ((ir_val >> 9) & 0x1):
                done = True
                break
            time.sleep(0.05)
        if done == False:
            print(f"  [{i+1:>4}/{N}] ⚠️ 校准触发超时")
            continue
        """
        
        cal = instr.swd_read_register(ADC_CALIBDAT)

        if cal is None:
            print(f"  [{i+1:>4}/{N}] ⚠️ 读取 ADC_DATA0 失败 (SWD 异常), 跳过该点")
            continue
        if raw is None:
            print(f"  [{i+1:>4}/{N}] ⚠️ 读取 ADC_DATA0 失败 (SWD 异常), 跳过该点")
            continue
        print(f" raw: {raw:#X} cal: {cal:#X}")
        raw &= 0xFFF
        cal = (cal >> 11) & 0xFFF
        
        conv_mv = raw / 4095.0 * VREFH_mV                  # 转换电压(mV)
        calv_mv = cal / 4095.0 * VREFH_mV
        all_raw.append({"seq": i + 1, "vin": vin, "dac_mv": dac_mv, "raw": raw, "vol": vol})
        # 打印: 计数 / adc_code / 转换电压mV / 实测电压mV
        if dmm_interval > 0:
            print(f"  [{i+1:>4}/{N}] adc={raw:4d} 转换={conv_mv:7.2f}mV 实测={vol:7.2f}mV 校准={calv_mv:7.2f}mV")
        else:
            print(f"  [{i+1:>4}/{N}] adc={raw:4d} 转换={conv_mv:7.2f}mV 校准={calv_mv:7.2f}mV")
        time.sleep(0.002)

    # ---------- 4. 统计汇总 ----------
    if all_raw:
        raws = [r["raw"] for r in all_raw]
        vols = [r["vol"] for r in all_raw if r["vol"] is not None]
        print("\n[采样统计]")
        print(f"  有效样本 : {len(raws)}")
        print(f"  Min      : {min(raws)} (0x{min(raws):03X})")
        print(f"  Max      : {max(raws)} (0x{max(raws):03X})")
        print(f"  平均     : {sum(raws) / len(raws):.2f}")
        print(f"  峰峰值   : {max(raws) - min(raws)} LSB")
        if vols:
            print(f"  实测电压 : {min(vols):.2f}~{max(vols):.2f} mV (平均 {sum(vols)/len(vols):.2f} mV)")
            # 校验：由实测电压反推 VREFH 实际值（若已知实际输入）
        print(f"  目标电压 : {min(r['vin'] for r in all_raw)}~{max(r['vin'] for r in all_raw)} mV")
    else:
        print(f"\n❌ 未获取到任何有效采样值，请检查 ADC_CH{ADC_CH} ({ch_name}) 通路与寄存器配置。")

    # ---------- 5. DNL / INL 计算（转移曲线法） ----------
    dnl_max = inl_max = 0.0
    dnl_points = []   # (code1, code2, DNL)
    inl_points = []   # (code2, INL)
    dnl_by_code = {}  # code1 → DNL (供 xlsx 第 7 列)
    inl_by_code = {}  # code2 → INL (供 xlsx 第 8 列)
    if all_raw:
        lsb_mv = VREFH_mV / 4096.0                       # 1 LSB 对应电压 (12bit)
        # 输入电压优先用实测(vol)，未实测时用目标电压(vin)
        pts = [(r["vol"] if r["vol"] is not None else float(r["vin"]), r["raw"]) for r in all_raw]
        pts.sort(key=lambda x: x[1])                     # 按 adc_code 排序
        print("\n[DNL 逐点结果]")
        acc = 0.0
        for i in range(len(pts) - 1):
            v1, c1 = pts[i]
            v2, c2 = pts[i + 1]
            dc = c2 - c1
            if dc <= 0:
                continue
            dnl = ((v2 - v1) / lsb_mv) / dc - 1.0        # 相邻 code 步距归一化偏差
            acc += dnl
            dnl_points.append((c1, c2, dnl))
            inl_points.append((c2, acc))
            dnl_by_code[c1] = dnl
            inl_by_code[c2] = acc
            print(f"  code {c1}→{c2}: DNL={dnl:+.3f} LSB")
        if dnl_points:
            dnl_max = max(dnl_points, key=lambda x: abs(x[2]))[2]
            inl_max = max(inl_points, key=lambda x: abs(x[1]))[1]
        print(f"  DNL max : {dnl_max:+.3f} LSB")       # INL 只保存不打印

    # ---------- 5b. 线性拟合 / 残差 / R² / 端点线性 / 最佳直线线性 (仅线性扫描模式 choice=='3') ----------
    # 说明: 以 (目标电压 vin, adc_code) 为样本, 因 y 单位即为 code(=LSB), 拟合/端点直线的偏差可直接以 LSB 计。
    #   - 最佳直线线性(BFSL): 偏差 = adc_code 与最小二乘拟合直线的差值 (即残差)
    #   - 端点线性(Endpoint): 偏差 = adc_code 与首末两点连线的差值
    fit_slope = fit_intercept = None
    ep_slope = ep_intercept = None
    r_squared = None
    resid_variance = None
    bfsl_linearity = ep_linearity = None
    fit_by_seq = {}   # seq → (BFSL拟合值, BFSL残差, 端点直线值, 端点线性偏差)
    if choice == '3' and all_raw:
        xs = [float(r["vin"]) for r in all_raw]
        ys = [float(r["raw"]) for r in all_raw]
        n_pt = len(xs)
        sum_x = sum(xs)
        sum_y = sum(ys)
        sum_xy = sum(x * y for x, y in zip(xs, ys))
        sum_xx = sum(x * x for x in xs)
        denom = n_pt * sum_xx - sum_x * sum_x
        if n_pt >= 2 and denom != 0:
            # ---- 最佳拟合直线 (BFSL, 最小二乘) ----
            fit_slope = (n_pt * sum_xy - sum_x * sum_y) / denom
            fit_intercept = (sum_y - fit_slope * sum_x) / n_pt
            mean_y = sum_y / n_pt
            resid_sq_sum = 0.0
            ss_tot = 0.0
            bfsl_max = 0.0
            for r, x, y in zip(all_raw, xs, ys):
                y_fit = fit_slope * x + fit_intercept
                resid = y - y_fit
                resid_sq_sum += resid * resid
                ss_tot += (y - mean_y) ** 2
                bfsl_max = max(bfsl_max, abs(resid))
                fit_by_seq[r["seq"]] = [y_fit, resid, None, None]
            resid_variance = resid_sq_sum / n_pt
            r_squared = 1.0 - resid_sq_sum / ss_tot if ss_tot > 0 else 1.0
            bfsl_linearity = bfsl_max

            # ---- 端点直线 (首末两点连线) ----
            x_first, y_first = xs[0], ys[0]
            x_last, y_last = xs[-1], ys[-1]
            if x_last != x_first:
                ep_slope = (y_last - y_first) / (x_last - x_first)
                ep_intercept = y_first - ep_slope * x_first
                ep_max = 0.0
                for r, x, y in zip(all_raw, xs, ys):
                    y_ep = ep_slope * x + ep_intercept
                    dev_ep = y - y_ep
                    ep_max = max(ep_max, abs(dev_ep))
                    fit_by_seq[r["seq"]][2] = y_ep
                    fit_by_seq[r["seq"]][3] = dev_ep
                ep_linearity = ep_max

            print(f"\n[线性拟合] adc_code = {fit_slope:.6f} * vin(mV) + {fit_intercept:.3f}  (R²={r_squared:.6f})")
            print(f"  残差方差(BFSL)   : {resid_variance:.6f}")
            print(f"  最佳直线线性(BFSL): {bfsl_linearity:+.3f} LSB")
            if ep_linearity is not None:
                print(f"  端点线性(Endpoint): {ep_linearity:+.3f} LSB")

            # ---- 实测电压(DMM) vs 转换电压(adc_code反推) 线性拟合斜率 ----
            vm_slope = vm_intercept = vm_r2 = vm_slope_diff = None
            vm_pts = [(r["vol"], r["raw"] / 4095.0 * VREFH_mV) for r in all_raw if r["vol"] is not None]
            if len(vm_pts) >= 2:
                vxs = [p[0] for p in vm_pts]
                vys = [p[1] for p in vm_pts]
                n_v = len(vxs)
                s_x = sum(vxs); s_y = sum(vys)
                s_xy = sum(x * y for x, y in zip(vxs, vys))
                s_xx = sum(x * x for x in vxs)
                v_denom = n_v * s_xx - s_x * s_x
                if v_denom != 0:
                    vm_slope = (n_v * s_xy - s_x * s_y) / v_denom
                    vm_intercept = (s_y - vm_slope * s_x) / n_v
                    mean_vy = s_y / n_v
                    ss_res = sum((y - (vm_slope * x + vm_intercept)) ** 2 for x, y in zip(vxs, vys))
                    ss_tot_v = sum((y - mean_vy) ** 2 for y in vys)
                    vm_r2 = 1.0 - ss_res / ss_tot_v if ss_tot_v > 0 else 1.0
                    vm_slope_diff = vm_slope - 1.0    # 理想情况下 转换电压=实测电压, 斜率应为1
                    print(f"\n[实测电压-转换电压 拟合] 转换电压(mV) = {vm_slope:.6f} * 实测电压(mV) + {vm_intercept:.3f}  (R²={vm_r2:.6f})")
                    print(f"  斜率与理想值(1.0)之差 : {vm_slope_diff:+.6f}  ({vm_slope_diff*100:+.3f}%)")
            else:
                print("\n[实测电压-转换电压 拟合] 无实测电压数据(未开启万用表实测), 跳过")

    # ---------- 6. 保存测量数据 (xlsx) 到桌面 ----------
    if all_raw:
        save_choice = input("\n是否保存测量数据为 xlsx 文件? (y/n, 默认n): ").strip().lower()
        if save_choice in ('y', 'yes'):
            save_filename = input("请输入文件名 (默认 ADC_DNL_测量数据): ").strip() or "ADC_DNL_测量数据"
            if not save_filename.endswith(".xlsx"):
                save_filename += ".xlsx"
            save_path = os.path.join(os.path.expanduser("~"), "Desktop", save_filename)
            try:
                wb = openpyxl.Workbook()
                ws = wb.active
                ws.title = "ADC_DNL数据"
                headers = ["计数", "目标电压(mV)", "DAC2设置(mV)", "adc_code", "转换电压(mV)", "实测电压(mV)", "DNL(LSB)", "INL(LSB)"]
                if choice == '3':
                    headers += ["BFSL拟合值(adc_code)", "BFSL残差(LSB)", "端点直线值(adc_code)", "端点线性偏差(LSB)"]
                ws.append(headers)
                for r in all_raw:
                    conv_mv = r["raw"] / 4095.0 * VREFH_mV
                    row = [r["seq"], r["vin"], r["dac_mv"], r["raw"], round(conv_mv, 3),
                           round(r["vol"], 3) if r["vol"] is not None else "",
                           round(dnl_by_code.get(r["raw"]), 3) if r["raw"] in dnl_by_code else "",
                           round(inl_by_code.get(r["raw"]), 3) if r["raw"] in inl_by_code else ""]
                    if choice == '3':
                        if r["seq"] in fit_by_seq:
                            y_fit, resid, y_ep, dev_ep = fit_by_seq[r["seq"]]
                            row += [round(y_fit, 3), round(resid, 3),
                                    round(y_ep, 3) if y_ep is not None else "",
                                    round(dev_ep, 3) if dev_ep is not None else ""]
                        else:
                            row += ["", "", "", ""]
                    ws.append(row)
                ws.append([])
                ws.append(["DNL max(LSB)", round(dnl_max, 3)])
                ws.append(["INL max(LSB)", round(inl_max, 3)])
                ws.append(["量程(mV)", VREFH_mV])
                ws.append(["通道", "ADC_CH%d(%s)" % (ADC_CH, ch_name)])
                ws.append(["总采样点数", len(all_raw)])

                # ---- 线性扫描模式: 汇总统计单独放一个工作表, 便于查看 ----
                if choice == '3' and fit_slope is not None:
                    ws_sum = wb.create_sheet("汇总统计")
                    ws_sum.column_dimensions["A"].width = 26
                    ws_sum.column_dimensions["B"].width = 18
                    ws_sum.append(["指标", "数值"])
                    ws_sum.append(["DNL max(LSB)", round(dnl_max, 3)])
                    ws_sum.append(["INL max(LSB)", round(inl_max, 3)])
                    ws_sum.append(["量程(mV)", VREFH_mV])
                    ws_sum.append(["通道", "ADC_CH%d(%s)" % (ADC_CH, ch_name)])
                    ws_sum.append(["总采样点数", len(all_raw)])
                    ws_sum.append([])
                    ws_sum.append(["── BFSL 最小二乘拟合 ──"])
                    ws_sum.append(["拟合斜率", round(fit_slope, 6)])
                    ws_sum.append(["拟合截距", round(fit_intercept, 3)])
                    ws_sum.append(["R²", round(r_squared, 6)])
                    ws_sum.append(["残差方差", round(resid_variance, 6)])
                    ws_sum.append(["最佳直线线性(BFSL, LSB)", round(bfsl_linearity, 3)])
                    if ep_linearity is not None:
                        ws_sum.append([])
                        ws_sum.append(["── 端点直线 ──"])
                        ws_sum.append(["端点直线斜率", round(ep_slope, 6)])
                        ws_sum.append(["端点直线截距", round(ep_intercept, 3)])
                        ws_sum.append(["端点线性(Endpoint, LSB)", round(ep_linearity, 3)])
                    if vm_slope is not None:
                        ws_sum.append([])
                        ws_sum.append(["── 实测电压 vs 转换电压 拟合 ──"])
                        ws_sum.append(["拟合斜率", round(vm_slope, 6)])
                        ws_sum.append(["拟合截距", round(vm_intercept, 3)])
                        ws_sum.append(["R²", round(vm_r2, 6)])
                        ws_sum.append(["斜率与理想值(1.0)之差", round(vm_slope_diff, 6)])

                # ---- 线性扫描模式: 每个折线图放在独立工作表, 并放大显示 ----
                if choice == '3' and fit_slope is not None:
                    from openpyxl.chart import ScatterChart, Series, Reference

                    n_rows = len(all_raw)
                    data_first_row = 2
                    data_last_row = data_first_row + n_rows - 1
                    CHART_W, CHART_H = 30, 18   # cm, 放大显示

                    x_ref = Reference(ws, min_col=2, min_row=data_first_row, max_row=data_last_row)      # 目标电压(mV)
                    y_ref = Reference(ws, min_col=4, min_row=data_first_row, max_row=data_last_row)       # adc_code
                    y_fit_ref = Reference(ws, min_col=9, min_row=data_first_row, max_row=data_last_row)   # BFSL拟合值
                    y_ep_ref = Reference(ws, min_col=11, min_row=data_first_row, max_row=data_last_row)    # 端点直线值
                    resid_ref = Reference(ws, min_col=10, min_row=data_first_row, max_row=data_last_row)  # BFSL残差
                    code_ref = Reference(ws, min_col=4, min_row=data_first_row, max_row=data_last_row)     # adc_code
                    dnl_ref = Reference(ws, min_col=7, min_row=data_first_row, max_row=data_last_row)      # DNL
                    inl_ref = Reference(ws, min_col=8, min_row=data_first_row, max_row=data_last_row)      # INL

                    # 图1: 转移曲线 (adc_code实测 + BFSL拟合 + 端点直线)
                    ws_chart1 = wb.create_sheet("图1_转移曲线拟合")
                    chart = ScatterChart()
                    chart.title = "ADC 转移曲线 (线性扫描) 与 BFSL/端点直线拟合"
                    chart.x_axis.title = "目标电压(mV)"
                    chart.y_axis.title = "adc_code"
                    chart.style = 13
                    chart.width, chart.height = CHART_W, CHART_H

                    series_raw = Series(y_ref, x_ref, title="adc_code 实测")
                    series_raw.marker.symbol = "circle"
                    series_raw.graphicalProperties.line.noFill = True

                    series_fit = Series(y_fit_ref, x_ref, title="BFSL拟合")
                    series_fit.marker.symbol = "none"
                    series_fit.smooth = False

                    series_ep = Series(y_ep_ref, x_ref, title="端点直线")
                    series_ep.marker.symbol = "none"
                    series_ep.smooth = False

                    chart.series.append(series_raw)
                    chart.series.append(series_fit)
                    if ep_linearity is not None:
                        chart.series.append(series_ep)
                    ws_chart1.add_chart(chart, "B2")

                    # 图2: BFSL 残差
                    ws_chart2 = wb.create_sheet("图2_BFSL残差")
                    resid_chart = ScatterChart()
                    resid_chart.title = "BFSL 残差 (Residual)"
                    resid_chart.x_axis.title = "目标电压(mV)"
                    resid_chart.y_axis.title = "残差(LSB)"
                    resid_chart.style = 13
                    resid_chart.width, resid_chart.height = CHART_W, CHART_H
                    series_resid = Series(resid_ref, x_ref, title="残差")
                    series_resid.marker.symbol = "circle"
                    series_resid.graphicalProperties.line.noFill = True
                    resid_chart.series.append(series_resid)
                    ws_chart2.add_chart(resid_chart, "B2")

                    # 图3: 端点线性偏差
                    if ep_linearity is not None:
                        dev_ep_ref = Reference(ws, min_col=12, min_row=data_first_row, max_row=data_last_row)  # 端点线性偏差
                        ws_chart3 = wb.create_sheet("图3_端点线性偏差")
                        ep_chart = ScatterChart()
                        ep_chart.title = "端点线性偏差 (Endpoint Deviation)"
                        ep_chart.x_axis.title = "目标电压(mV)"
                        ep_chart.y_axis.title = "偏差(LSB)"
                        ep_chart.style = 13
                        ep_chart.width, ep_chart.height = CHART_W, CHART_H
                        series_dev_ep = Series(dev_ep_ref, x_ref, title="端点线性偏差")
                        series_dev_ep.marker.symbol = "circle"
                        series_dev_ep.graphicalProperties.line.noFill = True
                        ep_chart.series.append(series_dev_ep)
                        ws_chart3.add_chart(ep_chart, "B2")

                    # 图4: DNL vs adc_code
                    ws_chart4 = wb.create_sheet("图4_DNL")
                    dnl_chart = ScatterChart()
                    dnl_chart.title = "DNL (Differential Nonlinearity)"
                    dnl_chart.x_axis.title = "adc_code"
                    dnl_chart.y_axis.title = "DNL(LSB)"
                    dnl_chart.style = 13
                    dnl_chart.width, dnl_chart.height = CHART_W, CHART_H
                    series_dnl = Series(dnl_ref, code_ref, title="DNL")
                    series_dnl.marker.symbol = "circle"
                    series_dnl.graphicalProperties.line.noFill = True
                    dnl_chart.series.append(series_dnl)
                    ws_chart4.add_chart(dnl_chart, "B2")

                    # 图5: INL vs adc_code
                    ws_chart5 = wb.create_sheet("图5_INL")
                    inl_chart = ScatterChart()
                    inl_chart.title = "INL (Integral Nonlinearity)"
                    inl_chart.x_axis.title = "adc_code"
                    inl_chart.y_axis.title = "INL(LSB)"
                    inl_chart.style = 13
                    inl_chart.width, inl_chart.height = CHART_W, CHART_H
                    series_inl = Series(inl_ref, code_ref, title="INL")
                    series_inl.marker.symbol = "circle"
                    series_inl.graphicalProperties.line.noFill = True
                    inl_chart.series.append(series_inl)
                    ws_chart5.add_chart(inl_chart, "B2")

                # ---- 公式说明工作表：列出各指标中文含义与计算公式 ----
                ws_doc = wb.create_sheet("公式说明")
                ws_doc.column_dimensions["A"].width = 22
                ws_doc.column_dimensions["B"].width = 60
                ws_doc.column_dimensions["C"].width = 60
                ws_doc.append(["指标", "中文含义", "计算公式"])
                ws_doc.append([
                    "转换电压(mV)",
                    "由 adc_code 按理想满量程比例反推出的理论电压",
                    "转换电压 = raw / 4095 × VREFH",
                ])
                ws_doc.append([
                    "DNL (差分非线性)",
                    "相邻两个采样点(按adc_code从小到大排序后)之间，实测电压变化量换算成LSB后，相对理想1个LSB步进的偏差",
                    "DNL(i) = ((V2-V1) / (VREFH/4096)) / (code2-code1) - 1",
                ])
                ws_doc.append([
                    "INL (积分非线性)",
                    "从起始点到当前点，逐步DNL的累加值，代表该点相对理想直线的累计非线性误差(转移曲线法)",
                    "INL(code_i) = Σ DNL(1..i)",
                ])
                if choice == '3':
                    ws_doc.append([
                        "BFSL拟合斜率/截距",
                        "以目标电压vin为自变量、adc_code为因变量做最小二乘线性回归得到的最佳拟合直线",
                        "adc_code = 拟合斜率 × vin + 拟合截距 (最小二乘法)",
                    ])
                    ws_doc.append([
                        "R²",
                        "拟合优度，衡量BFSL直线对采样点的拟合程度，越接近1越好",
                        "R² = 1 - Σ(残差²) / Σ(y-均值)²",
                    ])
                    ws_doc.append([
                        "BFSL残差(LSB)",
                        "每个采样点实测的adc_code，与该点按BFSL拟合直线预测出的adc_code之差",
                        "残差(i) = y_i - (拟合斜率×x_i + 拟合截距)",
                    ])
                    ws_doc.append([
                        "残差方差",
                        "所有残差平方的平均值，衡量整体离散程度",
                        "方差 = Σ(残差²) / n",
                    ])
                    ws_doc.append([
                        "最佳直线线性(BFSL, LSB)",
                        "所有残差绝对值中的最大值，代表实际转移曲线偏离最佳拟合直线的最大LSB数",
                        "BFSL线性 = max(|残差|)",
                    ])
                    ws_doc.append([
                        "端点直线斜率/截距",
                        "仅用首尾两个采样点连线得到的参考直线",
                        "端点斜率 = (y_last-y_first)/(x_last-x_first); 端点截距 = y_first - 端点斜率×x_first",
                    ])
                    ws_doc.append([
                        "端点线性偏差(LSB)",
                        "每点adc_code与端点直线预测值之差；其最大绝对值即“端点线性”",
                        "偏差(i) = y_i - (端点斜率×x_i + 端点截距)",
                    ])
                    ws_doc.append([
                        "实测-转换 拟合斜率",
                        "以DMM实测电压为自变量、转换电压(理论值)为因变量做线性回归，理想情况下斜率应为1",
                        "转换电压 = 拟合斜率 × 实测电压 + 拟合截距",
                    ])
                    ws_doc.append([
                        "斜率与理想值(1.0)之差",
                        "实测-转换拟合斜率偏离理想值1.0的程度，反映实际ADC增益/DMM量程误差",
                        "斜率差 = 拟合斜率 - 1.0",
                    ])
                ws_doc.append([])
                ws_doc.append(["注: DNL/INL计算前需先按adc_code从小到大排序取相邻点，与表格原始采样顺序(seq)不同。"])

                wb.save(save_path)
                print(f"✅ 测量数据已保存到: {save_path}")
            except Exception as e:
                print(f"❌ 保存文件失败: {e}")

    # ---------- 7. 保存测试结果 (item_results.xlsx) ----------
    if all_raw:
        # 规格判定: DNL/INL ±2 LSB (参考 2810)
        adc_passed = (abs(dnl_max) <= 2.0 and abs(inl_max) <= 2.0)
        save_item_result("ADC_DNL", adc_passed, round(dnl_max, 3))
        print(f"\n{'✅ ADC_DNL 测试通过' if adc_passed else '❌ ADC_DNL 测试失败'} "
              f"(DNL {dnl_max:+.3f} / INL {inl_max:+.3f} LSB, 规格 ±2 LSB)")

    # ---------- 8. 清理（关闭 ADC，复位继电器通路含 DAC） ----------
    instr.swd_modify_register(RW_ANA_ADC_EN, 0x00, 19, 19)       # 关闭 ADC_EN(仅清该位, 避免整字写破坏 ANAC_ENCR 其他使能位)
    instr.serial_send("cmd_gpio_set_595d_test reset\r\n")        # 只发送不等待响应



@test_case              #wait
def CSA_OS_VTRM():
    test_item_name = "gain"
    row = df_test_data[df_test_data["测试项"] == test_item_name]
    if row.empty:
        print(f"未找到测试项: {test_item_name}")
        return
    try:
        min_val = float(row["Min."].values[0])
        typ_val = float(row["Typ."].values[0])
        max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
    except (IndexError, ValueError) as e:
        print(f"❌ 测试参数读取失败: {e}")
        return
    try:
        PA_ANA_EN = test_item.get_32bit_reg_address("PA_ANA_EN", df_register)
        RW_CSA_EN = test_item.get_32bit_reg_address("RW_CSA_EN", df_register)
        RW_CSA_CAL_EN = test_item.get_32bit_reg_address("RW_CSA_CAL_EN", df_register)

        RW_CSA_GAIN = test_item.get_32bit_reg_address("RW_CSA_GAIN", df_register)
        RW_CSA_REF = test_item.get_32bit_reg_address("RW_CSA_REF", df_register)

        RW_ATP_MUX = test_item.get_32bit_reg_address("RW_ATP_MUX", df_register)
        RW_ATP_MUX_EN = test_item.get_32bit_reg_address("RW_ATP_MUX_EN", df_register)

        ANAC_TRM1 = test_item.get_32bit_reg_address("ANAC_TRM1", df_register)
        RW_CSA_OS_VTRM = test_item.get_32bit_reg_address("RW_CSA_OS_VTRM", df_register)
        
    except Exception as e:
        print(f"❌ 寄存器地址获取失败: {e}")
        return
    
    print("CSA (电流检测放大器) 测试")

    # 测试前置: 配置 PA5/PA6 复用 ALT_FUNC_14 (开 GPIO 时钟 + 输出方向 + AF14)
    init_pa5_pa6_altfunc14()

    #instr.serial_send_and_receive(f"cmd_gpio_set_595d_test set {pin_cfg.GPIO5} {eq_cfg.DMM_MODE}\r\n")
    instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
    instr.swd_modify_register(RW_ATP_MUX, 0x09, 0, 4)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)
    instr.swd_modify_register(RW_CSA_EN, 0x01, 17, 17)
    time.sleep(0.1)
    instr.swd_modify_register(RW_CSA_GAIN, 0x01, 5, 7)
    instr.swd_modify_register(RW_CSA_REF, 0x01, 8, 12)
    time.sleep(0.1)

    instr.serial_send_and_receive(f"cmd_gpio_set_595d_test set {pin_cfg.GPIO5} {eq_cfg.DMM_MODE}\r\n")
    time.sleep(0.5)
    print(f"A_CSA_VSEN的电压为: {instr.dmm_measure_single_voltage()} mV")
    instr.swd_modify_register(RW_CSA_CAL_EN, 0x01, 13, 13)


@test_case
def HSI_FTRM():
    test_item_name = "freq_32M"
    row = df_test_data[df_test_data["测试项"] == test_item_name]
    if row.empty:
        print(f"未找到测试项: {test_item_name}")
        return
    try:
        min_val = float(row["Min."].values[0])
        typ_val = float(row["Typ."].values[0])
        max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
        if unit == "MHz" :
            min_val = min_val * 1000000.0
            typ_val = typ_val * 1000000.0
            max_val = max_val * 1000000.0
        elif unit == "KHz" :
            min_val = min_val * 1000.0
            typ_val = typ_val * 1000.0
            max_val = max_val * 1000.0
    except (IndexError, ValueError) as e:
        print(f"❌ 测试参数读取失败: {e}")
        return
    try:
        PA6_AFS14_EN = test_item.get_32bit_reg_address("PA6_AFS14_EN", df_register)
        RW_ANA_HSI_EN = test_item.get_32bit_reg_address("RW_ANA_HSI_EN", df_register)

        RW_DTP_MUX = test_item.get_32bit_reg_address("RW_DTP_MUX", df_register)
        RW_DTP_MUX_EN = test_item.get_32bit_reg_address("RW_DTP_MUX_EN", df_register)

        ANAC_TRM2 = test_item.get_32bit_reg_address("ANAC_TRM2", df_register)
        RW_HSI_FTRM = test_item.get_32bit_reg_address("RW_HSI_FTRM", df_register)
    except Exception as e:
        print(f"❌ 寄存器地址获取失败: {e}")
        return


    # 测试前置: 配置 PA5/PA6 复用 ALT_FUNC_14 (开 GPIO 时钟 + 输出方向 + AF14)
    init_pa5_pa6_altfunc14()

    #使能相关模块
    instr.swd_modify_register(PA6_AFS14_EN, 0x0E, 24, 27)
    instr.swd_modify_register(RW_ANA_HSI_EN, 0x01, 26, 26)

    #确认 DTP_MUX 选中 HSI
    instr.swd_modify_register(RW_DTP_MUX, 0x13, 8, 12)
    instr.swd_modify_register(RW_DTP_MUX_EN, 0x01, 13, 13)

    trim = instr.swd_read_register(ANAC_TRM2)
    if trim is None:
        print("❌ 读取 ANAC_TRM2 失败 (SWD 异常), 跳过本次修调")
        return
    cur = (trim >> 0) & 0x7F

    # 测频率、周期、占空比（测量前先自动设置，适配当前信号频率）
    instr.osc_auto_set()
    f_a, p_a, d_a = instr.osc_measure_frequency("CH1")

    passed = (f_a >= min_val and f_a <= max_val)
    if passed :
        print(f"HSI_FTRM测试通过,修调范围为: {min_val}~{max_val} Hz, 实际测量值: {f_a:.3f} Hz, 修调值为：{cur}, 无需修调")
        global_use["RW_HSI_FTRM"] = RW_HSI_FTRM
        global_use["HSI_FTRM_BEST"] = cur
        save_trim_to_excel(test_item_name, "RW_HSI_FTRM", RW_HSI_FTRM, cur, passed)
        save_item_result(test_item_name, passed, round(f_a, 3))
        instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
        time.sleep(0.5)
        return
    else:
        print(f"HSI_FTRM测试不通过,修调范围为: {min_val}~{max_val} Hz, 实际测量值: {f_a:.3f} Hz, 修调值为：{cur}, 需要进行修调")

    new = min(127, cur + 1)
    instr.swd_modify_register(RW_HSI_FTRM, new, 0, 6)
    time.sleep(0.5)
    instr.osc_auto_set()
    f_b, p_b, d_b = instr.osc_measure_frequency("CH1")


    typ_target = (float)(1.0 / typ_val * 1000000.0)

    step = p_b - p_a
    if abs(step) > 0.000001:
        trim_target = round((typ_target - p_a) / step) + cur
        trim_target = max(0, min(127, trim_target))
    else:
        trim_target = cur

    best_trim = trim_target

    min_err = float('inf')

    instr.osc_auto_set()
    #复测，在计算出的修调范围内获取最佳修调值
    range_vtrm = range(trim_target - 5, trim_target + 5)
    for test_trim in range_vtrm:
        if test_trim < 0 or test_trim > 127:
            continue
        instr.swd_modify_register(RW_HSI_FTRM, test_trim, 0, 6)
        time.sleep(0.5)
        t_f, t_p, t_d = instr.osc_measure_frequency("CH1")
        test_err = abs(t_p - typ_target)
        print(f"测试修调值={test_trim}, 测试频率={t_f}Hz, 测试周期={t_p}us, 与目标周期误差={test_err}us")

        if test_err < min_err:
            best_trim = test_trim
            min_err = test_err

    instr.swd_modify_register(RW_HSI_FTRM, best_trim, 0, 6)
    time.sleep(0.5)
    global_use["RW_HSI_FTRM"] = RW_HSI_FTRM
    global_use["HSI_FTRM_BEST"] = best_trim

    instr.osc_auto_set()
    b_f, b_p, b_d = instr.osc_measure_frequency("CH1")
    passed = (b_f >= min_val and b_f <= max_val)
    save_trim_to_excel(test_item_name, "RW_HSI_FTRM", RW_HSI_FTRM, best_trim, passed)
    save_item_result(test_item_name, passed, round(b_f, 3))
    if passed:
        print("HSI_FTRM Test pass")
        print(f"修调范围为: {min_val}~{max_val} Hz, 实际测量值: {b_f:.3f} Hz, 修调值为：{best_trim}")
    else:
        print("HSI_FTRM Test failed")
        print(f"修调范围为: {min_val}~{max_val} Hz, 实际测量值: {b_f:.3f} Hz, 修调值为：{best_trim}")
    instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
    time.sleep(1)


@test_case
def LSI_FTRM():
    test_item_name = "freq_32K"
    row = df_test_data[df_test_data["测试项"] == test_item_name]
    if row.empty:
        print(f"未找到测试项: {test_item_name}")
        return
    try:
        min_val = float(row["Min."].values[0])
        typ_val = float(row["Typ."].values[0])
        max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
        if unit == "MHz" :
            min_val = min_val * 1000000.0
            typ_val = typ_val * 1000000.0
            max_val = max_val * 1000000.0
        elif unit == "KHz" :
            min_val = min_val * 1000.0
            typ_val = typ_val * 1000.0
            max_val = max_val * 1000.0
    except (IndexError, ValueError) as e:
        print(f"❌ 测试参数读取失败: {e}")
        return
    try:
        PA6_AFS14_EN = test_item.get_32bit_reg_address("PA6_AFS14_EN", df_register)
        RW_LSI_32K_EN = test_item.get_32bit_reg_address("RW_LSI_32K_EN", df_register)

        RW_DTP_MUX = test_item.get_32bit_reg_address("RW_DTP_MUX", df_register)
        RW_DTP_MUX_EN = test_item.get_32bit_reg_address("RW_DTP_MUX_EN", df_register)
        
        ANAC_TRM2 = test_item.get_32bit_reg_address("ANAC_TRM2", df_register)
        RW_LSI_FTRM = test_item.get_32bit_reg_address("RW_LSI_FTRM", df_register)
    except Exception as e:
        print(f"❌ 寄存器地址获取失败: {e}")
        return


    # 测试前置: 配置 PA5/PA6 复用 ALT_FUNC_14 (开 GPIO 时钟 + 输出方向 + AF14)
    init_pa5_pa6_altfunc14()

    #使能相关模块
    instr.swd_modify_register(PA6_AFS14_EN, 0x0E, 24, 27)
    instr.swd_modify_register(RW_LSI_32K_EN, 0x01, 3, 3)

    #确认 ATP_MUX 选中 VREFH
    instr.swd_modify_register(RW_DTP_MUX, 0x12, 8, 12)
    instr.swd_modify_register(RW_DTP_MUX_EN, 0x01, 13, 13)

    trim = instr.swd_read_register(ANAC_TRM2)
    if trim is None:
        print("❌ 读取 ANAC_TRM2 失败 (SWD 异常), 跳过本次修调")
        return
    cur = (trim >> 7) & 0x7F

    # 测频率、周期、占空比（测量前先自动设置，适配当前信号频率）
    instr.osc_auto_set()
    f_a, p_a, d_a = instr.osc_measure_frequency("CH1")
    passed = (f_a >= min_val and f_a <= max_val)
    if passed :
        print(f"LSI_FTRM测试通过,修调范围为: {min_val}~{max_val} Hz, 实际测量值: {f_a:.3f} Hz, 修调值为：{cur}, 无需修调")
        global_use["RW_LSI_FTRM"] = RW_LSI_FTRM
        global_use["LSI_FTRM_BEST"] = cur
        save_trim_to_excel(test_item_name, "RW_LSI_FTRM", RW_LSI_FTRM, cur, passed)
        save_item_result(test_item_name, passed, round(f_a, 3))
        instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
        time.sleep(0.5)
        return
    else:
        print(f"LSI_FTRM测试不通过,修调范围为: {min_val}~{max_val} Hz, 实际测量值: {f_a:.3f} Hz, 修调值为：{cur}, 需要进行修调")

    new = min(127, cur + 1)
    instr.swd_modify_register(RW_LSI_FTRM, new, 7, 13)
    time.sleep(0.5)
    instr.osc_auto_set()
    f_b, p_b, d_b = instr.osc_measure_frequency("CH1")

    p_target = (float)(1.0 / typ_val * 1000000.0)

    step = p_b - p_a
    if abs(step) > 0.000001:
        trim_target = round((p_target - p_a) / step) + cur
        trim_target = max(0, min(127, trim_target))
    else:
        trim_target = cur

    best_trim = trim_target

    min_err = float('inf')

    instr.osc_auto_set()
    #复测，在计算出的修调范围内获取最佳修调值
    range_vtrm = range(trim_target - 10, trim_target + 10)
    for test_trim in range_vtrm:
        if test_trim < 0 or test_trim > 127:
            continue
        instr.swd_modify_register(RW_LSI_FTRM, test_trim, 7, 13)
        time.sleep(0.5)
        
        t_f, t_p, t_d = instr.osc_measure_frequency("CH1")
        test_err = abs(t_p - p_target)
        print(f"测试修调值={test_trim}, 测试频率={t_f}Hz, 测试周期={t_p}us, 与目标周期误差={test_err}us")

        if test_err < min_err:
            best_trim = test_trim
            min_err = test_err

    instr.swd_modify_register(RW_LSI_FTRM, best_trim, 7, 13)
    time.sleep(0.5)
    global_use["RW_LSI_FTRM"] = RW_LSI_FTRM
    global_use["LSI_FTRM_BEST"] = best_trim

    instr.osc_auto_set()
    b_f, b_p, b_d = instr.osc_measure_frequency("CH1")
    passed = (b_f >= min_val and b_f <= max_val)
    save_trim_to_excel(test_item_name, "RW_LSI_FTRM", RW_LSI_FTRM, best_trim, passed)
    save_item_result(test_item_name, passed, round(b_f, 3))
    if passed:
        print("LSI_FTRM Test pass")
        print(f"修调范围为: {min_val}~{max_val} Hz, 实际测量值: {b_f:.3f} Hz, 修调值为：{best_trim}")
    else:
        print("LSI_FTRM Test failed")
        print(f"修调范围为: {min_val}~{max_val} Hz, 实际测量值: {b_f:.3f} Hz, 修调值为：{best_trim}")
    instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
    time.sleep(1)


@test_case
def PLL_CKO_VS_VIN_Test():
    """PLL_CKO 频率随输入电压变化关系测试（遍历 RW_PLL_VCO[14:12] = 111~000 全部8种取值）
    配置参考 other_test/OCD5010/test.py 的 PLL_CKO_TRIM_Test；用户输入电压范围与步进，
    每个 RW_PLL_VCO 取值下，对每个电压点先由DAC输出并经万用表实测电压，再切到ATP通路后由示波器测频率，
    最终结果保存到独立 Excel(数据表 + 折线图工作表, 每个 RW_PLL_VCO 取值一条曲线)。
    """
    from openpyxl.chart import ScatterChart, Reference, Series
    from openpyxl.chart.marker import Marker
    from openpyxl.drawing.line import LineProperties
    from openpyxl.styles import Font, Alignment, PatternFill
    from openpyxl.utils import get_column_letter

    RW_PLL_VCO_COLORS = ["1F77B4", "FF7F0E", "2CA02C", "D62728",
                          "9467BD", "8C564B", "E377C2", "7F7F7F"]

    # ---------- 1. 前置配置 (与 other_test/OCD5010/test.py PLL_CKO_TRIM_Test 一致) ----------
    init_pa5_pa6_altfunc14()
    instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
    time.sleep(1)
    instr.swd_modify_register(0x40020058, 0x01, 16, 16)
    time.sleep(0.1)
    instr.swd_modify_register(0x40007C00, 0x01, 21, 21)
    time.sleep(0.1)
    instr.swd_modify_register(0x48000050, 0x20, 0, 15)
    time.sleep(0.1)
    instr.swd_modify_register(0x40007C1C, 0x01, 5, 5)
    time.sleep(0.1)
    instr.swd_modify_register(0x40007C1C, 0x01, 6, 6)
    time.sleep(0.1)
    instr.swd_modify_register(0x40007C1C, 0b10001, 0, 4)
    time.sleep(0.1)
    instr.swd_modify_register(0x40007C0C, 0x01, 19, 19)
    time.sleep(0.1)
    instr.swd_modify_register(0x40007C1C, 0x01, 13, 13)     # 使能DTP_MUX
    time.sleep(0.1)
    instr.swd_modify_register(0x40007C1C, 0b10000, 8, 12)   # 选择PLL_CKO_DTP
    time.sleep(0.1)
    instr.swd_modify_register(0x40007C0C, 0x01, 17, 17)
    time.sleep(0.1)
    instr.swd_modify_register(0x40007C0C, 0x01, 19, 19)
    time.sleep(0.1)
    instr.swd_modify_register(0x40007C0C, 0x03, 3, 6)
    time.sleep(0.1)
    # RW_PLL_VCO[14:12] 默认 111，在下方逐个取值遍历，此处不再固定写入

    time.sleep(1)   # 配置完成后延时1s

    # ---------- 2. 用户输入电压范围与步进 ----------
    try:
        v_start = float(input("请输入起始电压(mV): ").strip())
        v_end = float(input("请输入终止电压(mV): ").strip())
        v_step = float(input("请输入步进值(mV): ").strip())
    except ValueError:
        print("❌ 输入无效，测试终止")
        return
    if v_step == 0:
        print("❌ 步进值不能为0，测试终止")
        return
    if (v_end - v_start) * v_step < 0:
        v_step = -v_step   # 自动纠正步进方向，保证能从起始遍历到终止

    vin_list = []
    v = v_start
    if v_step > 0:
        while v <= v_end + 1e-9:
            vin_list.append(round(v, 4))
            v += v_step
    else:
        while v >= v_end - 1e-9:
            vin_list.append(round(v, 4))
            v += v_step

    print(f"共 {len(vin_list)} 个测试点: {vin_list}")

    # ---------- 3. 遍历 RW_PLL_VCO = 0b111 ~ 0b000，每个取值下逐点步进 ----------
    all_results = {}   # {vco_code: [(idx, set_vol, real_vol, freq, period, duty), ...]}
    for vco_code in range(0b111, -1, -1):
        print(f"\n===== 设置 RW_PLL_VCO = {vco_code:03b} =====")
        instr.swd_modify_register(0x40007C0C, vco_code, 12, 14)
        time.sleep(0.5)

        results = []
        for idx, set_vol in enumerate(vin_list, start=1):
            instr.serial_send(f"spi_cmd_set_val mV {set_vol}\r\n")
            time.sleep(0.5)

            instr.serial_send(f"cmd_gpio_set_595d_test set 38 6\r\n")
            time.sleep(2)
            real_vol = instr.dmm_measure_single_voltage()
            print(f"[VCO={vco_code:03b}][{idx}/{len(vin_list)}] 设置电压={set_vol} mV, 实测电压={real_vol:.3f} mV")

            time.sleep(1)
            instr.serial_send(f"cmd_gpio_set_595d_test set 15 6\r\n")
            time.sleep(1.5)
            if idx == 1:
                instr.osc_auto_set()
                time.sleep(3)
            freq, period, duty = instr.osc_measure_frequency("CH1")
            if freq is not None:
                print(f"    实测频率={freq:.3f} Hz, 周期={period}us, 占空比={duty}")
            else:
                print("    ⚠️ 频率测量失败")

            results.append((idx, set_vol, real_vol, freq, period, duty))

        all_results[vco_code] = results

    instr.serial_send("cmd_gpio_set_595d_test reset\r\n")

    if not any(all_results.values()):
        print("⚠️ 无有效测试数据，跳过Excel保存")
        return

    # ---------- 4. 保存到Excel: 数据表 + 折线图工作表(每个RW_PLL_VCO取值一条曲线) ----------
    save_dir = os.path.join(_output_base_dir, "Save_Results", "OCD5010")
    os.makedirs(save_dir, exist_ok=True)
    excel_path = os.path.join(save_dir, "PLL_CKO_vs_Vin.xlsx")

    wb = openpyxl.Workbook()
    ws_data = wb.active
    ws_data.title = "PLL_CKO_数据"

    header_font = Font(bold=True)
    header_fill = PatternFill(start_color="DCE6F1", end_color="DCE6F1", fill_type="solid")
    header = ["RW_PLL_VCO", "序号", "设置电压(mV)", "实测电压(mV)", "频率(Hz)", "周期(us)", "占空比"]
    ws_data.append(header)
    for col in range(1, len(header) + 1):
        cell = ws_data.cell(row=1, column=col)
        cell.font = header_font
        cell.fill = header_fill
        cell.alignment = Alignment(horizontal="center")

    # 记录每个 vco_code 对应的数据行范围, 用于后续画图引用
    vco_row_ranges = {}
    for vco_code in range(0b111, -1, -1):
        results = all_results.get(vco_code, [])
        if not results:
            continue
        first_row = ws_data.max_row + 1
        for idx, set_vol, real_vol, freq, period, duty in results:
            ws_data.append([
                f"{vco_code:03b}", idx, set_vol,
                round(real_vol, 4) if real_vol is not None else "",
                round(freq, 4) if freq is not None else "",
                round(period, 4) if period is not None else "",
                round(duty, 4) if duty is not None else "",
            ])
        last_row = ws_data.max_row
        vco_row_ranges[vco_code] = (first_row, last_row)

    col_widths = [12, 8, 16, 16, 16, 14, 12]
    for i, w in enumerate(col_widths, start=1):
        ws_data.column_dimensions[get_column_letter(i)].width = w
    ws_data.freeze_panes = "A2"

    ws_chart = wb.create_sheet("PLL_CKO_折线图")
    chart = ScatterChart()
    chart.title = "PLL_CKO 频率 vs 输入电压 (按 RW_PLL_VCO 分组)"
    chart.x_axis.title = "实测电压(mV)"
    chart.y_axis.title = "频率(Hz)"
    chart.x_axis.axPos = "b"
    chart.y_axis.axPos = "l"
    chart.x_axis.delete = False
    chart.y_axis.delete = False
    chart.style = 13
    chart.width, chart.height = 42, 26   # cm, 放大清晰显示，容纳多条曲线图例

    for i, vco_code in enumerate(range(0b111, -1, -1)):
        if vco_code not in vco_row_ranges:
            continue
        first_row, last_row = vco_row_ranges[vco_code]
        vol_ref = Reference(ws_data, min_col=4, min_row=first_row, max_row=last_row)   # 实测电压
        freq_ref = Reference(ws_data, min_col=5, min_row=first_row, max_row=last_row)  # 频率

        series = Series(freq_ref, vol_ref, title=f"RW_PLL_VCO={vco_code:03b}")
        series.marker = Marker(symbol="circle", size=6)
        color = RW_PLL_VCO_COLORS[i % len(RW_PLL_VCO_COLORS)]
        series.graphicalProperties.line = LineProperties(w=20000, solidFill=color)
        series.marker.graphicalProperties.solidFill = color
        series.smooth = False
        chart.series.append(series)

    ws_chart.add_chart(chart, "B2")

    wb.save(excel_path)
    print(f"✅ 测试数据已保存至: {excel_path}")


@test_case
def TJ_TTRM():
    test_item_name = "OTW"
    row = df_test_data[df_test_data["测试项"] == test_item_name]
    if row.empty:
        print(f"未找到测试项: {test_item_name}")
        return
    try:
        min_val = float(row["Min."].values[0])
        typ_val = float(row["Typ."].values[0])
        max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
    except (IndexError, ValueError) as e:
        print(f"❌ 测试参数读取失败: {e}")
        return
    try:
        PA_ANA_EN = test_item.get_32bit_reg_address("PA_ANA_EN", df_register)
        RW_ANA_TJ_EN = test_item.get_32bit_reg_address("RW_ANA_TJ_EN", df_register)
        RW_ANA_NG_EN = test_item.get_32bit_reg_address("RW_ANA_NG_EN", df_register)
        RW_TJ_OTW_NG_EN = test_item.get_32bit_reg_address("RW_TJ_OTW_NG_EN", df_register)
        RW_ATP_MUX = test_item.get_32bit_reg_address("RW_ATP_MUX", df_register)
        RW_ATP_MUX_EN = test_item.get_32bit_reg_address("RW_ATP_MUX_EN", df_register)
        ANAC_TRM2 = test_item.get_32bit_reg_address("ANAC_TRM2", df_register)
        RW_TJ_TTRM = test_item.get_32bit_reg_address("RW_TJ_TTRM", df_register)
    except Exception as e:
        print(f"❌ 寄存器地址获取失败: {e}")
        return
    
    print("TJ (结温) OTW 阈值修调")

    try:
        temp_start = float(input("请输入起始温度（℃）：").strip())
        temp_stop = float(input("请输入终止温度（℃）：").strip())
        temp_step = float(input("请输入温度步进值（℃）：").strip())
        if temp_step <= 0 or temp_stop < temp_start:
            print(" 输入参数无效，终止测试")
            return
    except ValueError:
        print("输入参数有误")
        return

    # 测试前置: 配置 PA5/PA6 复用 ALT_FUNC_14 (开 GPIO 时钟 + 输出方向 + AF14)
    init_pa5_pa6_altfunc14()
    time.sleep(0.5)
    instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
    time.sleep(0.1)
    instr.swd_modify_register(RW_ANA_TJ_EN, 0x01, 15, 15)
    time.sleep(0.1)
    #确认 ATP_MUX 选中 VREF
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 6, 6)
    time.sleep(0.1)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 7, 7)
    time.sleep(0.1)
    instr.swd_modify_register(RW_ATP_MUX, 0b10011, 0, 4)
    time.sleep(0.1)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)

    chip_id = input("请输入本次测试的芯片编号 (如 11，对应A11): ").strip() or None

    temp_list = list(frange(temp_start, temp_stop + temp_step / 2.0, temp_step))
    temp_vol_list = []

    print(f"ANAC_EN: {instr.swd_read_register(0x40007C00, True)}")
    print(f"ANAC_TEMP: {instr.swd_read_register(0x40007C04, True)}")
    print(f"ANAC_MUX: {instr.swd_read_register(0x40007C1C, True)}")

    for temp in temp_list:
        print(f"请将温度设置为{temp}℃，然后按下enter继续")
        input()
        time.sleep(0.1)
        temp_vol = instr.dmm_measure_single_voltage()
        print(f"温度：{temp}℃  TJ_Vref：{temp_vol}mV")
        temp_vol_list.append(temp_vol)

    save_tj_ttrm_excel(temp_list, temp_vol_list, chip_id=chip_id)


def save_tj_ttrm_excel(temp_list, vol_list, chip_id=None):
    """保存 TJ_TTRM 温度扫描数据(温度-电压), 并绘制折线图"""
    from openpyxl.chart import LineChart, Reference
    from openpyxl.utils import get_column_letter

    save_dir = os.path.join(_CHIP_TEST_RESULTS_DIR, "TJ_TTRM")
    os.makedirs(save_dir, exist_ok=True)
    filename = f"TJ_TTRM_results_A{chip_id}.xlsx" if chip_id else "TJ_TTRM_results.xlsx"
    excel_path = os.path.join(save_dir, filename)

    wb = openpyxl.Workbook()
    ws_data = wb.active
    ws_data.title = "TJ_TTRM_数据"

    ws_data.cell(row=1, column=1, value="温度(℃)")
    ws_data.cell(row=1, column=2, value="TJ_Vref(mV)")
    data_first_row = 2
    for i, (temp, vol) in enumerate(zip(temp_list, vol_list)):
        row = data_first_row + i
        ws_data.cell(row=row, column=1, value=temp)
        ws_data.cell(row=row, column=2, value=vol)
    ws_data.column_dimensions[get_column_letter(1)].width = 14
    ws_data.column_dimensions[get_column_letter(2)].width = 16
    ws_data.freeze_panes = "A2"

    data_last_row = data_first_row + len(temp_list) - 1

    ws_chart = wb.create_sheet("TJ_TTRM_曲线图")
    chart = LineChart()
    chart.title = "温度-TJ_Vref 曲线"
    chart.x_axis.title = "温度(℃)"
    chart.y_axis.title = "TJ_Vref(mV)"
    chart.style = 13
    chart.width, chart.height = 30, 18

    vol_ref = Reference(ws_data, min_col=2, min_row=1, max_row=data_last_row)
    cats_ref = Reference(ws_data, min_col=1, min_row=data_first_row, max_row=data_last_row)
    chart.add_data(vol_ref, titles_from_data=True)
    chart.set_categories(cats_ref)
    ws_chart.add_chart(chart, "B2")

    try:
        wb.save(excel_path)
        print(f"✅ TJ_TTRM 数据已保存: {excel_path}")
    except Exception as e:
        print(f"⚠️ 保存 TJ_TTRM Excel 失败: {e}")


@test_case              #wait
def TS_ITRM():
    """修调目标: GPIO6 接 10KΩ 电阻到地, 调 TS_ITRM 使电压 = 0.2V
    (电流 = 0.2V/10KΩ = 20µA)。
    """
    test_item_name = "I output"
    row = df_test_data[df_test_data["测试项"] == test_item_name]
    if row.empty:
        print(f"未找到测试项: {test_item_name}")
        return
    try:
        min_val = float(row["Min."].values[0])
        typ_val = float(row["Typ."].values[0])
        max_val = float(row["Max."].values[0])
        #数据转换，测试项里写的是电流，串联一个10K电阻转换成电压测量
        min_val = 10000 * min_val / 1000
        typ_val = 10000 * typ_val / 1000
        max_val = 10000 * max_val / 1000
        unit = str(row["Unit"].values[0])
    except (IndexError, ValueError) as e:
        print(f"❌ 测试参数读取失败: {e}")
        return
    try:
        RW_ANA_EN = test_item.get_32bit_reg_address("RW_ANA_EN", df_register)
        RW_ANA_TS_EN = test_item.get_32bit_reg_address("RW_ANA_TS_EN", df_register)
        RW_TS_GPIO6_EN = test_item.get_32bit_reg_address("RW_TS_GPIO6_EN", df_register)
        RW_TS_GPIO5_EN = test_item.get_32bit_reg_address("RW_TS_GPIO5_EN", df_register)
        RW_ATP_MUX = test_item.get_32bit_reg_address("RW_ATP_MUX", df_register)
        RW_ATP_MUX_EN = test_item.get_32bit_reg_address("RW_ATP_MUX_EN", df_register)
        ANAC_TRM2 = test_item.get_32bit_reg_address("ANAC_TRM2", df_register)
        RW_TS_ITRM = test_item.get_32bit_reg_address("RW_TS_ITRM", df_register)
    except Exception as e:
        print(f"❌ 寄存器地址获取失败: {e}")
        return
    
    print("TS (温度传感器) 电流修调")

    # 测试前置: 配置 PA5/PA6 复用 ALT_FUNC_14 (开 GPIO 时钟 + 输出方向 + AF14)
    init_pa5_pa6_altfunc14()

    instr.serial_send_and_receive(f"cmd_gpio_set_595d_test set {pin_cfg.GPIO5} {eq_cfg.DMM_MODE}\r\n")
    time.sleep(0.5)

    print("GPIO6需要串联一个10K电阻到地，然后按下enter继续......")
    input()
    instr.swd_modify_register(RW_TS_GPIO5_EN, 0x01, 18, 18)
    instr.swd_modify_register(RW_TS_GPIO6_EN, 0x01, 19, 19)
    time.sleep(0.1)

    trim = instr.swd_read_register(ANAC_TRM2)
    if trim is None:
        print("❌ 读取 ANAC_TRM2 失败 (SWD 异常), 跳过本次修调")
        return
    cur = (trim >> 19) & 0x3F

    v_a = instr.dmm_measure_single_voltage()
    new = min(63, cur + 1)

    instr.swd_modify_register(RW_TS_ITRM, new, 19, 24)
    time.sleep(0.1)
    v_b = instr.dmm_measure_single_voltage()

    step = v_b - v_a
    if abs(step) > 0.001:
        trim_target = round((typ_val - v_a) / step) + cur
        trim_target = max(0, min(63, trim_target))
    else:
        trim_target = cur
    
    best_trim = trim_target

    min_err = float('inf')

    range_itrm = range(trim_target - 5, trim_target + 5)
    for test_trim in range_itrm:
        if test_trim < 0 or test_trim > 63:
            continue
        instr.swd_modify_register(RW_TS_ITRM, test_trim, 19, 24)
        time.sleep(0.5)
        test_vol = instr.dmm_measure_single_voltage()
        test_err = abs(test_vol - typ_val)
        print(f"测试修调值={test_trim}, 测试电压={test_vol}, 与目标周期误差={test_err}A")

        if test_err < min_err:
            best_trim = test_trim
            min_err = test_err

    instr.swd_modify_register(RW_TS_ITRM, best_trim, 19, 24)
    time.sleep(0.5)
    global_use["RW_TS_ITRM"] = RW_TS_ITRM
    global_use["TS_ITRM_BEST"] = best_trim

    best_vol = instr.dmm_measure_single_voltage()
    passed = (best_vol >= min_val and best_vol <= max_val)
    save_trim_to_excel(test_item_name, "RW_TS_ITRM", RW_TS_ITRM, best_trim, passed)
    save_item_result(test_item_name, passed, round(best_vol, 3))
    if passed:
        print("TS_ITRM Test pass")
    else:
        print("TS_ITRM Test failed")
        print(f"修调范围为: {min_val}~{max_val} {unit}, 实际测量值: {best_vol:.3f} mV")
    instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")


@test_case
def Reread_test(test_select=0):
    """
    写入后回读测试
    test_select: 0=全部执行, 1=VREF, 2=VREF_SLP, 3=VBG, 4=LDO12, 5=LDO12_SLP, 6=ADC_VREFH, 7=HSI, 8=LSI
    """
    try:
        row = df_test_data[df_test_data["测试项"] == "vbg"]
        bg_min_val = float(row["Min."].values[0])
        bg_typ_val = float(row["Typ."].values[0])
        bg_max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
        if unit == "V" :
            bg_min_val = bg_min_val * 1000.0
            bg_typ_val = bg_typ_val * 1000.0
            bg_max_val = bg_max_val * 1000.0

        row = df_test_data[df_test_data["测试项"] == "vref_1d2v"]
        vref_min_val = float(row["Min."].values[0])
        vref_typ_val = float(row["Typ."].values[0])
        vref_max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
        if unit == "V" :
            vref_min_val = vref_min_val * 1000.0
            vref_typ_val = vref_typ_val * 1000.0
            vref_max_val = vref_max_val * 1000.0

        row = df_test_data[df_test_data["测试项"] == "vslp"]
        vref_slp_min_val = float(row["Min."].values[0])
        vref_slp_typ_val = float(row["Typ."].values[0])
        vref_slp_max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
        if unit == "V" :
            vref_slp_min_val = vref_slp_min_val * 1000.0
            vref_slp_typ_val = vref_slp_typ_val * 1000.0
            vref_slp_max_val = vref_slp_max_val * 1000.0

        row = df_test_data[df_test_data["测试项"] == "VDD12(normal)"]
        ldo_min_val = float(row["Min."].values[0])
        ldo_typ_val = float(row["Typ."].values[0])
        ldo_max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
        if unit == "V" :
            ldo_min_val = ldo_min_val * 1000.0
            ldo_typ_val = ldo_typ_val * 1000.0
            ldo_max_val = ldo_max_val * 1000.0

        row = df_test_data[df_test_data["测试项"] == "VDD12(sleep)"]
        ldo_slp_min_val = float(row["Min."].values[0])
        ldo_slp_typ_val = float(row["Typ."].values[0])
        ldo_slp_max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
        if unit == "V" :
            ldo_slp_min_val = ldo_slp_min_val * 1000.0
            ldo_slp_typ_val = ldo_slp_typ_val * 1000.0
            ldo_slp_max_val = ldo_slp_max_val * 1000.0

        row = df_test_data[df_test_data["测试项"] == "vrefH"]
        adc_vref_min_val = float(row["Min."].values[0])
        adc_vref_typ_val = float(row["Typ."].values[0])
        adc_vref_max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
        if unit == "V" :
            adc_vref_min_val = adc_vref_min_val * 1000.0
            adc_vref_typ_val = adc_vref_typ_val * 1000.0
            adc_vref_max_val = adc_vref_max_val * 1000.0

        row = df_test_data[df_test_data["测试项"] == "freq_32M"]
        hsi_min_val = float(row["Min."].values[0])
        hsi_typ_val = float(row["Typ."].values[0])
        hsi_max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
        if unit == "MHz" :
            hsi_min_val = hsi_min_val * 1000000.0
            hsi_typ_val = hsi_typ_val * 1000000.0
            hsi_max_val = hsi_max_val * 1000000.0
        elif unit == "KHz" :
            hsi_min_val = hsi_min_val * 1000.0
            hsi_typ_val = hsi_typ_val * 1000.0
            hsi_max_val = hsi_max_val * 1000.0

        row = df_test_data[df_test_data["测试项"] == "freq_32K"]
        lsi_min_val = float(row["Min."].values[0])
        lsi_typ_val = float(row["Typ."].values[0])
        lsi_max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
        if unit == "MHz" :
            lsi_min_val = lsi_min_val * 1000000.0
            lsi_typ_val = lsi_typ_val * 1000000.0
            lsi_max_val = lsi_max_val * 1000000.0
        elif unit == "KHz" :
            lsi_min_val = lsi_min_val * 1000.0
            lsi_typ_val = lsi_typ_val * 1000.0
            lsi_max_val = lsi_max_val * 1000.0
    except (IndexError, ValueError) as e:
            print(f"❌ 测试参数读取失败: {e}")
            return

    try:
        RW_ATP_MUX = test_item.get_32bit_reg_address("RW_ATP_MUX", df_register)
        RW_ATP_MUX_EN = test_item.get_32bit_reg_address("RW_ATP_MUX_EN", df_register)

        RW_ANA_REF_EN = test_item.get_32bit_reg_address("RW_ANA_REF_EN", df_register)
        PA5_AFS14_EN = test_item.get_32bit_reg_address("PA5_AFS14_EN", df_register)

        RW_DTP_MUX = test_item.get_32bit_reg_address("RW_DTP_MUX", df_register)
        RW_DTP_MUX_EN = test_item.get_32bit_reg_address("RW_DTP_MUX_EN", df_register)

        PA_ANA_EN = test_item.get_32bit_reg_address("PA_ANA_EN", df_register)
        RW_SLP_REF_EN = test_item.get_32bit_reg_address("RW_SLP_REF_EN", df_register)
        RW_ANA_LDO12_EN = test_item.get_32bit_reg_address("RW_ANA_LDO12_EN", df_register)
        RW_ANA_VREFH_EN = test_item.get_32bit_reg_address("RW_ANA_VREFH_EN", df_register)
        RW_ANA_HSI_EN = test_item.get_32bit_reg_address("RW_ANA_HSI_EN", df_register)
        RW_LSI_32K_EN = test_item.get_32bit_reg_address("RW_LSI_32K_EN", df_register)

        PA6_AFS14_EN = test_item.get_32bit_reg_address("PA6_AFS14_EN", df_register)

        ANAC_TRM1 = test_item.get_32bit_reg_address("ANAC_TRM1", df_register)
        ANAC_TRM2 = test_item.get_32bit_reg_address("ANAC_TRM2", df_register)

        RW_ADC_VREFH_ATP = test_item.get_32bit_reg_address("RW_ADC_VREFH_ATP", df_register)
        RW_ADC_VREFH = test_item.get_32bit_reg_address("RW_ADC_VREFH", df_register)

    except Exception as e:
        print(f"❌ 寄存器地址获取失败: {e}")
        return

    # 测试前置: 配置 PA5/PA6 复用 ALT_FUNC_14 (开 GPIO 时钟 + 输出方向 + AF14)
    init_pa5_pa6_altfunc14()
    time.sleep(0.5)
    trim1 = instr.swd_read_register(ANAC_TRM1)
    time.sleep(0.5)
    trim2 = instr.swd_read_register(ANAC_TRM2)
    time.sleep(0.5)

    #使能相关模块
    instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
    instr.swd_modify_register(RW_ANA_REF_EN, 0x01, 24, 24)
    instr.swd_modify_register(PA6_AFS14_EN, 0x0E, 24, 27)
    time.sleep(0.1)


    """------------- VREF Test -------------"""
    if test_select in (0, 1):
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 6, 6)
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 7, 7)
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)
        instr.swd_modify_register(RW_ATP_MUX, 0x02, 0, 4)
        time.sleep(0.5)

        vref_trim = (trim1 >> 0) & 0x1f
        vref_vol = instr.dmm_measure_single_voltage()
        passed = (vref_vol >= vref_min_val and vref_vol <= vref_max_val)
        if passed:
            print(f"VREF测试通过,修调范围为: {vref_min_val}~{vref_max_val} mV, 实际测量值: {vref_vol:.3f} mV, 修调值为：{vref_trim}")
        else:
            print(f"VREF测试不通过,修调范围为: {vref_min_val}~{vref_max_val} mV, 实际测量值: {vref_vol:.3f} mV, 修调值为：{vref_trim}")
        time.sleep(0.5)
        print("--------------------------------------------------------")



    """------------- VREF_SLP Test -------------"""
    if test_select in (0, 2):
        instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
        instr.swd_modify_register(RW_SLP_REF_EN, 0x01, 4, 4)
        time.sleep(0.1)
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 6, 6)
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 7, 7)
        instr.swd_modify_register(RW_ATP_MUX, 0x01, 0, 4)
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)
        time.sleep(0.5)

        vref_slp_trim = (trim1 >> 14) & 0x0F
        vref_slp_vol = instr.dmm_measure_single_voltage()
        passed = (vref_slp_vol >= vref_slp_min_val and vref_slp_vol <= vref_slp_max_val)
        if passed:
            print(f"VREF_SLP测试通过,修调范围为: {vref_slp_min_val}~{vref_slp_max_val} mV, 实际测量值: {vref_slp_vol:.3f} mV, 修调值为：{vref_slp_trim}")
        else:
            print(f"VREF_SLP测试不通过,修调范围为: {vref_slp_min_val}~{vref_slp_max_val} mV, 实际测量值: {vref_slp_vol:.3f} mV, 修调值为：{vref_slp_trim}")
        time.sleep(0.5)
        print("--------------------------------------------------------")



    """------------- VBG Test -------------"""
    if test_select in (0, 3):
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 6, 6)
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 7, 7)

        instr.swd_modify_register(RW_ATP_MUX, 0x02, 0, 4)
        time.sleep(2.5)
        bga_vol = instr.dmm_measure_single_voltage()
        time.sleep(0.5)

        instr.swd_modify_register(RW_ATP_MUX, 0x03, 0, 4)
        time.sleep(0.5)

        vbg_trim = (trim2 >> 25) & 0x3F
        bga_vol = instr.dmm_measure_single_voltage()
        passed = (bga_vol >= bg_min_val and bga_vol <= bg_max_val)
        if passed:
            print(f"VBG测试通过,修调范围为: {bg_min_val}~{bg_max_val} mV, 实际测量值: {bga_vol:.3f} mV, 修调值为：{vbg_trim}")
        else:
            print(f"VBG测试不通过,修调范围为: {bg_min_val}~{bg_max_val} mV, 实际测量值: {bga_vol:.3f} mV, 修调值为：{vbg_trim}")
        time.sleep(0.5)
        print("--------------------------------------------------------")


    """------------- LDO12 Test -------------"""
    if test_select in (0, 4):
        instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
        instr.swd_modify_register(RW_ANA_LDO12_EN, 0x01, 25, 25)
        time.sleep(0.1)
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 6, 6)
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 7, 7)
        instr.swd_modify_register(RW_ATP_MUX, 0x0D, 0, 4)
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)
        time.sleep(0.1)

        ldo_trim = (trim1 >> 9) & 0x07
        ldo_vol = instr.dmm_measure_single_voltage()
        passed = (ldo_vol >= ldo_min_val and ldo_vol <= ldo_max_val)
        if passed:
            print(f"LDO12测试通过,修调范围为: {ldo_min_val}~{ldo_max_val} mV, 实际测量值: {ldo_vol:.3f} mV, 修调值为：{ldo_trim}")
        else:
            print(f"LDO12测试不通过,修调范围为: {ldo_min_val}~{ldo_max_val} mV, 实际测量值: {ldo_vol:.3f} mV, 修调值为：{ldo_trim}")
        time.sleep(0.5)
        print("--------------------------------------------------------")


    """------------- LDO12_SLP Test -------------"""
    if test_select in (0, 5):
        instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
        instr.swd_modify_register(RW_ANA_LDO12_EN, 0x0, 25, 25)
        time.sleep(0.1)
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 6, 6)
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 7, 7)
        instr.swd_modify_register(RW_ATP_MUX, 0b01101, 0, 4)
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)

        ldo_slp_trim = (trim1 >> 12) & 0x3
        ldo_slp_vol = instr.dmm_measure_single_voltage()
        passed = (ldo_slp_vol >= ldo_slp_min_val and ldo_slp_vol <= ldo_slp_max_val)
        if passed:
            print(f"LDO12_SLP测试通过,修调范围为: {ldo_slp_min_val}~{ldo_slp_max_val} mV, 实际测量值: {ldo_slp_vol:.3f} mV, 修调值为：{ldo_slp_trim}")
        else:
            print(f"LDO12_SLP测试不通过,修调范围为: {ldo_slp_min_val}~{ldo_slp_max_val} mV, 实际测量值: {ldo_slp_vol:.3f} mV, 修调值为：{ldo_slp_trim}")
        time.sleep(0.5)
        print("--------------------------------------------------------")


    """------------- ADC_VREFH Test -------------"""
    if test_select in (0, 6):
        instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
        instr.swd_modify_register(RW_ADC_VREFH_ATP, 0x01, 18, 18)
        time.sleep(0.1)
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 6, 6)
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 7, 7)
        instr.swd_modify_register(RW_ATP_MUX, 0x06, 0, 4)
        instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)
        instr.swd_modify_register(RW_ADC_VREFH, 0x02, 13, 14)
        instr.swd_modify_register(RW_ADC_VREFH, 0x00, 16, 17)
        instr.swd_modify_register(RW_ANA_VREFH_EN, 0x01, 18, 18)
        time.sleep(0.1)

        adc_vref_trim = (trim1 >> 18) & 0x7
        adc_vref_vol = instr.dmm_measure_single_voltage()
        passed = (adc_vref_vol >= adc_vref_min_val and adc_vref_vol <= adc_vref_max_val)
        if passed:
            print(f"ADC_VREFH测试通过,修调范围为: {adc_vref_min_val}~{adc_vref_max_val} mV, 实际测量值: {adc_vref_vol:.3f} mV, 修调值为：{adc_vref_trim}")
        else:
            print(f"ADC_VREFH测试不通过,修调范围为: {adc_vref_min_val}~{adc_vref_max_val} mV, 实际测量值: {adc_vref_vol:.3f} mV, 修调值为：{adc_vref_trim}")
        time.sleep(0.5)
        print("--------------------------------------------------------")


    """------------- HSI Test -------------"""
    if test_select in (0, 7):
        instr.swd_modify_register(PA6_AFS14_EN, 0x0E, 24, 27)
        instr.swd_modify_register(RW_ANA_HSI_EN, 0x01, 26, 26)
        instr.swd_modify_register(RW_DTP_MUX, 0x13, 8, 12)
        instr.swd_modify_register(RW_DTP_MUX_EN, 0x01, 13, 13)
        time.sleep(0.5)

        hsi_trim = (trim2 >> 0) & 0x7F
        # 测频率、周期、占空比（测量前先自动设置，适配当前信号频率）
        instr.osc_auto_set()
        time.sleep(1.5)
        hsi_f, hsi_p, hsi_d = instr.osc_measure_frequency("CH1")
        passed = (hsi_f >= hsi_min_val and hsi_f <= hsi_max_val)
        if passed:
            print(f"HSI_FTRM测试通过,修调范围为: {hsi_min_val}~{hsi_max_val} Hz, 实际测量值: {hsi_f:.3f} Hz, 修调值为：{hsi_trim}")
        else:
            print(f"HSI_FTRM测试不通过,修调范围为: {hsi_min_val}~{hsi_max_val} Hz, 实际测量值: {hsi_f:.3f} Hz, 修调值为：{hsi_trim}")
        time.sleep(0.5)
        print("--------------------------------------------------------")


    """------------- LSI Test -------------"""
    if test_select in (0, 8):
        instr.swd_modify_register(PA6_AFS14_EN, 0x0E, 24, 27)
        instr.swd_modify_register(RW_LSI_32K_EN, 0x01, 3, 3)
        instr.swd_modify_register(RW_DTP_MUX, 0x12, 8, 12)
        instr.swd_modify_register(RW_DTP_MUX_EN, 0x01, 13, 13)

        lsi_trim = (trim2 >> 7) & 0x7F
        # 测频率、周期、占空比（测量前先自动设置，适配当前信号频率）
        instr.osc_auto_set()
        time.sleep(1.5)
        lsi_f, lsi_p, lsi_d = instr.osc_measure_frequency("CH1")
        passed = (lsi_f >= lsi_min_val and lsi_f <= lsi_max_val)
        if passed:
            print(f"LSI_FTRM测试通过,修调范围为: {lsi_min_val}~{lsi_max_val} Hz, 实际测量值: {lsi_f:.3f} Hz, 修调值为：{lsi_trim}")
        else:
            print(f"LSI_FTRM测试不通过,修调范围为: {hsi_min_val}~{hsi_max_val} Hz, 实际测量值: {lsi_f:.3f} Hz, 修调值为：{lsi_trim}")
    time.sleep(0.5)
    print("--------------------------------------------------------")


def GPIO_toggle_test():
    #跳过PA13和PA14，因为PA13和PA14是UART调试接口
    #跳过PA15和PB0，因为PA15和PB0是SWD接口
    port = 'A'
    pin = 2                         # 从PA2开始
    for num in range(1, 34):        # num = 1 ~ 33，共33次
        if num == 12:               # A段结束，切换到B，从PB1开始
            port = 'B'
            pin = 1
        if num == 27:               # B段结束，切换到C，从PC0开始
            port = 'C'
            pin = 0
        print(f"now is port:{port} pin:{pin}, enter space start")
        input()
        instr.serial_send_and_receive(f"cmd_gpio_toggle {port} {pin}\r\n")
        pin = pin + 1               # 每次发送后递增引脚号



# =====================================================================
# 导入修调值写入模块
# =====================================================================
from chips.OCD5010.trim.program_trim import program_trim_values as _program_trim_values

def PROGRAM_TRIM():
    """将修调测试获得的最佳值烧写到 OTP 槽位 (自动判定烧写次数, 4 次机会)"""
    _program_trim_values()

def Reread_test_select():
    """写入后复位测试 —— 交互选择测试项"""
    print("\n请选择测试项 (0=全部执行):")
    print("  1. VREF Test")
    print("  2. VREF_SLP Test")
    print("  3. VBG Test")
    print("  4. LDO12 Test")
    print("  5. LDO12_SLP Test")
    print("  6. ADC_VREFH Test")
    print("  7. HSI Test")
    print("  8. LSI Test")
    try:
        sel = int(input("请输入选项 (0~8, 默认0): ").strip() or "0")
        sel = max(0, min(8, sel))
    except ValueError:
        sel = 0
    Reread_test(test_select=sel)


def SWD_INIT():
    """SWD 初始化 —— 芯片断电/复位后无需重启程序，选择此项即可重新连接芯片

    场景：测试过程中芯片偶尔需要断电，断电后 SWD (J-Link) 连接会断开，
    重新上电后只需选择本项即可重连，无需退出程序重新初始化所有设备。
    """
    print("正在进行 SWD 初始化，请确保芯片已重新上电...")
    ok = instr.swd_init()
    time.sleep(0.2)
    if not ok:
        print("❌ SWD 初始化失败，请检查 J-Link 连接与芯片供电后重试")
        return
    # 通过读取一个已知寄存器验证连接是否真正建立
    # (swd_init 内部即使 connect 失败也不一定会抛异常，需用寄存器读确认)
    try:
        anac_trm1 = test_item.get_32bit_reg_address("ANAC_TRM1", df_register)
        trim = instr.swd_read_register(anac_trm1)
        if trim is not None:
            print(f"✅ SWD 连接正常，ANAC_TRM1 = 0x{trim:08X}")
        else:
            print("⚠️ SWD 已初始化，但寄存器读取失败，请确认芯片已上电后再次初始化")
    except Exception as e:
        print(f"⚠️ SWD 已初始化，但验证读取异常: {e}")


# =====================================================================
# Trim 值扫描 —— 排查哪些修调值会使芯片锁死
# 仅写 RAM 寄存器(ANAC_TRM1/2)，断电即恢复，不会烧录 Flash/OTP
# =====================================================================
# 扫描表: (字段名, 所在寄存器, 起始位, 结束位, 位宽, 默认值, 对应测试项选择) —— 与 register.xlsx 一致
# 只扫描 Reread_test_select 里有对应测试项的 trim 字段（其余字段不测）:
# 1=VREF, 2=VREF_SLP, 3=VBG, 4=LDO12, 5=LDO12_SLP, 6=ADC_VREFH, 7=HSI, 8=LSI
_TRIM_SWEEP_TABLE = [
    ("RW_REF_VTRM",       "ANAC_TRM1", 0,  4,  5,  15, 1),
    ("RW_REF_ITRM",       "ANAC_TRM1", 5,  8,  4,   6, 1),
    ("RW_LDO12_VTRM",     "ANAC_TRM1", 9, 11,  3,   3, 4),
    ("RW_SLP12_VTRM",     "ANAC_TRM1", 12, 13,  2,   2, 5),
    ("RW_SLP_REF_VTRM",   "ANAC_TRM1", 14, 17,  4,   8, 2),
    ("RW_ADC_VREFH_VTRM", "ANAC_TRM1", 18, 20,  3,   4, 6),
    ("RW_HSI_FTRM",       "ANAC_TRM2", 0,  6,  7,  58, 7),
    ("RW_LSI_FTRM",       "ANAC_TRM2", 7, 13,  7,  70, 8),
    ("RW_BG_TTRM",        "ANAC_TRM2", 25, 30,  6,  30, 3),
]

# 断点续扫状态: 芯片锁死/用户中断后断电重启，再跑本项会从记录位置继续
_trim_sweep_progress = {"field": 0, "value": 0}
_trim_sweep_locked = []   # 累计锁死点 [(字段名, 值), ...]
_TRIM_SWEEP_REPORT_PATH = os.path.join(_output_base_dir, "Save_Results", "OCD5010", "trim_sweep_report.txt")


def _trim_sweep_enable_test(sel):
    """按 Reread_test_select 的方式使能对应测试项模块（仅寄存器配置，不做测量）。

    使能步骤与 Reread_test() 及各独立修调函数保持一致，保证扫描能复现真实测试的锁死条件
    （实测单独写 trim 值芯片不会锁死，必须先把对应测试项模块使能后再写才会触发）。
    sel: 1=VREF, 2=VREF_SLP, 3=VBG, 4=LDO12, 5=LDO12_SLP, 6=ADC_VREFH, 7=HSI, 8=LSI
    （仅扫描 Reread_test_select 中有对应测试项的 trim 字段）
    """
    def _addr(name):
        return test_item.get_32bit_reg_address(name, df_register)

    init_pa5_pa6_altfunc14()
    pa_ana_en = _addr("PA_ANA_EN")
    atp_mux = _addr("RW_ATP_MUX")
    atp_mux_en = _addr("RW_ATP_MUX_EN")
    # 公共使能（与 Reread_test 顶部一致）
    instr.swd_modify_register(pa_ana_en, 0x20, 0, 15)
    instr.swd_modify_register(_addr("RW_ANA_REF_EN"), 0x01, 24, 24)
    instr.swd_modify_register(_addr("PA6_AFS14_EN"), 0x0E, 24, 27)
    time.sleep(0.1)

    if sel == 1:        # VREF → RW_REF_VTRM / RW_REF_ITRM
        instr.swd_modify_register(atp_mux_en, 0x00, 6, 6)
        instr.swd_modify_register(atp_mux_en, 0x01, 7, 7)
        instr.swd_modify_register(atp_mux_en, 0x01, 5, 5)
        instr.swd_modify_register(atp_mux, 0x02, 0, 4)
    elif sel == 2:      # VREF_SLP → RW_SLP_REF_VTRM
        instr.swd_modify_register(pa_ana_en, 0x20, 0, 15)
        instr.swd_modify_register(_addr("RW_SLP_REF_EN"), 0x01, 4, 4)
        time.sleep(0.1)
        instr.swd_modify_register(atp_mux_en, 0x00, 6, 6)
        instr.swd_modify_register(atp_mux_en, 0x01, 7, 7)
        instr.swd_modify_register(atp_mux, 0x01, 0, 4)
        instr.swd_modify_register(atp_mux_en, 0x01, 5, 5)
    elif sel == 3:      # VBG → RW_BG_TTRM
        instr.swd_modify_register(atp_mux_en, 0x01, 5, 5)
        instr.swd_modify_register(atp_mux_en, 0x00, 6, 6)
        instr.swd_modify_register(atp_mux_en, 0x01, 7, 7)
        instr.swd_modify_register(atp_mux, 0x02, 0, 4)
    elif sel == 4:      # LDO12 → RW_LDO12_VTRM
        instr.swd_modify_register(pa_ana_en, 0x20, 0, 15)
        instr.swd_modify_register(_addr("RW_ANA_LDO12_EN"), 0x01, 25, 25)
        time.sleep(0.1)
        instr.swd_modify_register(atp_mux_en, 0x01, 6, 6)
        instr.swd_modify_register(atp_mux_en, 0x00, 7, 7)
        instr.swd_modify_register(atp_mux, 0x0D, 0, 4)
        instr.swd_modify_register(atp_mux_en, 0x01, 5, 5)
    elif sel == 5:      # LDO12_SLP → RW_SLP12_VTRM (关键: 休眠LDO12使能后才可能锁死)
        instr.swd_modify_register(pa_ana_en, 0x20, 0, 15)
        instr.swd_modify_register(_addr("RW_ANA_LDO12_EN"), 0x0, 25, 25)
        time.sleep(0.1)
        instr.swd_modify_register(atp_mux_en, 0x00, 6, 6)
        instr.swd_modify_register(atp_mux_en, 0x01, 7, 7)
        instr.swd_modify_register(atp_mux, 0b01101, 0, 4)
        instr.swd_modify_register(atp_mux_en, 0x01, 5, 5)
    elif sel == 6:      # ADC_VREFH → RW_ADC_VREFH_VTRM
        instr.swd_modify_register(pa_ana_en, 0x20, 0, 15)
        instr.swd_modify_register(_addr("RW_ADC_VREFH_ATP"), 0x01, 18, 18)
        time.sleep(0.1)
        instr.swd_modify_register(atp_mux_en, 0x00, 6, 6)
        instr.swd_modify_register(atp_mux_en, 0x01, 7, 7)
        instr.swd_modify_register(atp_mux, 0x06, 0, 4)
        instr.swd_modify_register(atp_mux_en, 0x01, 5, 5)
        instr.swd_modify_register(_addr("RW_ADC_VREFH"), 0x02, 13, 14)
        instr.swd_modify_register(_addr("RW_ADC_VREFH"), 0x01, 16, 17)
        instr.swd_modify_register(_addr("RW_ANA_VREFH_EN"), 0x01, 18, 18)
    elif sel == 7:      # HSI → RW_HSI_FTRM
        instr.swd_modify_register(_addr("PA6_AFS14_EN"), 0x0E, 24, 27)
        instr.swd_modify_register(_addr("RW_ANA_HSI_EN"), 0x01, 26, 26)
        instr.swd_modify_register(_addr("RW_DTP_MUX"), 0x13, 8, 12)
        instr.swd_modify_register(_addr("RW_DTP_MUX_EN"), 0x01, 13, 13)
    elif sel == 8:      # LSI → RW_LSI_FTRM
        instr.swd_modify_register(_addr("PA6_AFS14_EN"), 0x0E, 24, 27)
        instr.swd_modify_register(_addr("RW_LSI_32K_EN"), 0x01, 3, 3)
        instr.swd_modify_register(_addr("RW_DTP_MUX"), 0x12, 8, 12)
        instr.swd_modify_register(_addr("RW_DTP_MUX_EN"), 0x01, 13, 13)
    else:
        print(f"   ⚠️ 未知测试项选择: {sel}, 仅写 trim 值不使能模块")
    time.sleep(0.3)


def TRIM_VALUE_SWEEP():
    """Trim 值扫描 —— 逐个把修调值写入对应区域，检测哪些值会使芯片锁死

    依次将每个 trim 字段从 0 写到字段最大值：写入 → 回读验证芯片是否存活。
    若某个值导致芯片锁死（SWD 无响应）立即停止并记录该危险值；
    芯片断电重启后可再次运行本项，从断点继续扫描。全部扫完且芯片存活时恢复原始值。
    """
    global _trim_sweep_locked

    # 加载历史锁死点（跨程序运行持久化）
    if os.path.exists(_TRIM_SWEEP_REPORT_PATH):
        try:
            with open(_TRIM_SWEEP_REPORT_PATH, encoding="utf-8") as f:
                lines = [ln.strip() for ln in f if ln.strip()]
            loaded = set(_trim_sweep_locked)
            for ln in lines:
                if " = " in ln:
                    nm, _, vv = ln.partition(" = ")
                    try:
                        loaded.add((nm, int(vv)))
                    except ValueError:
                        pass
            _trim_sweep_locked = sorted(loaded)
        except Exception:
            pass

    # 若上一轮已全部扫完，则重新开始
    if _trim_sweep_progress["field"] >= len(_TRIM_SWEEP_TABLE):
        _trim_sweep_progress["field"] = 0
        _trim_sweep_progress["value"] = 0

    print("=" * 62)
    print("🔍 Trim 值扫描: 逐个写入修调值(0~字段最大值), 检测会导致芯片锁死的危险值")
    print("   每个字段先按 Reread_test_select 使能对应测试项, 再逐个写 trim 值检测锁死")
    print("   仅写 RAM 寄存器，断电即恢复，不会烧录；发现锁死即停止，重跑可断点续扫")
    print("=" * 62)
    if _trim_sweep_locked:
        print("📌 历史锁死点:")
        for nm, vv in _trim_sweep_locked:
            print(f"   💀 {nm} = {vv}")
    if _trim_sweep_progress["field"] > 0 or _trim_sweep_progress["value"] > 0:
        fname = _TRIM_SWEEP_TABLE[_trim_sweep_progress["field"]][0]
        print(f"⏩ 断点续扫: 从字段 {fname} 值 {_trim_sweep_progress['value']} 继续")

    try:
        anac_trm1 = test_item.get_32bit_reg_address("ANAC_TRM1", df_register)
        anac_trm2 = test_item.get_32bit_reg_address("ANAC_TRM2", df_register)
    except Exception as e:
        print(f"❌ 寄存器地址获取失败: {e}")
        return
    reg_addr = {"ANAC_TRM1": anac_trm1, "ANAC_TRM2": anac_trm2}

    # 先复位一次，从干净状态开始
    try:
        instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
    except Exception:
        pass
    time.sleep(0.5)

    # 记录扫描前原始值，供全部扫完且存活时恢复
    orig = {}
    for reg, addr in reg_addr.items():
        v = instr.swd_read_register(addr)
        if v is None:
            print(f"❌ 读取 {reg} 失败 (SWD 异常)，请确认芯片已上电且连接正常")
            return
        orig[reg] = v

    start_field = _trim_sweep_progress["field"]
    start_value = _trim_sweep_progress["value"]
    last_written = None      # (字段索引, 字段名, 值): 上一次写入且确认存活的值，用于延迟锁死归因
    locked_this_run = False

    def _record_lock(nm, vv, fi, vi):
        """记录锁死点：内存列表 + 断点进度 + 报告文件"""
        nonlocal locked_this_run
        if (nm, vv) not in _trim_sweep_locked:
            _trim_sweep_locked.append((nm, vv))
        _trim_sweep_progress["field"] = fi
        _trim_sweep_progress["value"] = vi
        locked_this_run = True
        try:
            os.makedirs(os.path.dirname(_TRIM_SWEEP_REPORT_PATH), exist_ok=True)
            with open(_TRIM_SWEEP_REPORT_PATH, "a", encoding="utf-8") as f:
                f.write(f"{nm} = {vv}\n")
        except Exception:
            pass
        print(f"   断点已记录: 断电重启后重跑，从字段 {nm} 值 {vi} 继续")

    try:
        for fi, (name, reg, start_bit, end_bit, width, default, sel) in enumerate(_TRIM_SWEEP_TABLE):
            if fi < start_field:
                continue
            max_val = (1 << width) - 1
            v0 = max(start_value, 0) if fi == start_field else 0
            if v0 > max_val:
                # 本字段剩余值为空（上一个值已锁死），进入下一字段
                _trim_sweep_progress["field"] = fi + 1
                _trim_sweep_progress["value"] = 0
                print(f"   ↪ 字段 {name} 无可扫描剩余值，进入下一字段")
                continue
            mask = (1 << width) - 1
            print(f"\n--- 扫描字段 {fi+1}/{len(_TRIM_SWEEP_TABLE)}: {name}[{end_bit}:{start_bit}] "
                  f"({reg}, 0x{reg_addr[reg]:08X}) 范围 0~{max_val}, 默认 {default}, 测试项={sel} ---")
            # 先按 Reread_test_select 使能对应测试项，再逐个写 trim 值检测锁死
            _trim_sweep_enable_test(sel)
            for val in range(v0, max_val + 1):
                # 写入前先确认芯片存活（上一值的延迟锁死会在这里暴露）
                pre = instr.swd_read_register(reg_addr[reg])
                if pre is None:
                    if last_written is not None:
                        pfi, pnm, pvv = last_written
                        print(f"💀 芯片锁死! 写入 {name} = {val} 前已无响应，判定上一写入值 {pnm} = {pvv} 为锁死点")
                        _record_lock(pnm, pvv, pfi, pvv + 1)
                    else:
                        print(f"💀 芯片初始即无响应，请检查芯片供电与 J-Link 连接")
                    return
                try:
                    instr.swd_modify_register(reg_addr[reg], val, start_bit, end_bit)
                except Exception as e:
                    print(f"💀 芯片锁死! {name} = {val}, 写入时 SWD 异常: {e}")
                    _record_lock(name, val, fi, val + 1)
                    return
                time.sleep(0.2)
                rd = instr.swd_read_register(reg_addr[reg])
                if rd is None:
                    print(f"💀 芯片锁死! {name} = {val}, 写入后回读无响应")
                    _record_lock(name, val, fi, val + 1)
                    return
                field_val = (rd >> start_bit) & mask
                note = "" if field_val == val else f" (回读字段={field_val})"
                print(f"   值 {val}: ✅ 存活{note}")
                last_written = (fi, name, val)
                _trim_sweep_progress["field"] = fi
                _trim_sweep_progress["value"] = val + 1
            # 该字段全部值扫描完且存活
            print(f"   ✅ 字段 {name} {'全部值' if v0 == 0 else f'剩余值({v0}~{max_val})'} 均未锁死")
            _trim_sweep_progress["field"] = fi + 1
            _trim_sweep_progress["value"] = 0
            # 复位芯片恢复默认修调值，避免上一字段极端值干扰下一字段
            try:
                instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
            except Exception:
                pass
            time.sleep(0.5)
    except KeyboardInterrupt:
        fname = _TRIM_SWEEP_TABLE[min(_trim_sweep_progress["field"], len(_TRIM_SWEEP_TABLE) - 1)][0]
        print(f"\n⏹️ 用户中断，断点已保存: 从字段 {fname} 值 {_trim_sweep_progress['value']} 继续")
    except Exception as e:
        print(f"❌ 扫描异常: {e}")

    if locked_this_run:
        return

    # 全部扫描完成：末尾再确认一次存活（捕捉最后一值的延迟锁死）
    alive = all(instr.swd_read_register(a) is not None for a in reg_addr.values())
    if not alive and last_written is not None:
        pfi, pnm, pvv = last_written
        print(f"💀 芯片锁死! 最后一值 {pnm} = {pvv} 延迟锁死")
        _record_lock(pnm, pvv, pfi, pvv + 1)
        return
    if alive:
        for reg, v in orig.items():
            try:
                instr.swd_write_register(reg_addr[reg], v)
            except Exception:
                pass
        # 复位芯片，清理最后一个测试项的模块使能状态
        try:
            instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
        except Exception:
            pass
        print("\n✅ 扫描完成，已恢复修调寄存器原始值并复位芯片")
    else:
        print("\n⚠️ 芯片无响应，请断电重启后继续扫描")

    if _trim_sweep_locked:
        print(f"\n💀 已发现 {len(_trim_sweep_locked)} 个锁死点，详见上方/报告文件:")
        for nm, vv in _trim_sweep_locked:
            print(f"   {nm} = {vv}")
    else:
        print(f"\n🎉 全部 {len(_TRIM_SWEEP_TABLE)} 个 trim 字段扫描完成，均未导致芯片锁死")


# =====================================================================
# 测试项映射表 —— 供 main.py 动态加载菜单
# 每个芯片定义自己的测试项列表，main.py 根据 CHIP 变量自动切换
# 格式: { key: (显示名称, 测试函数) }
#     其中 key 为菜单选择键（数字字符串）
#     测试函数为无参可调用对象
# =====================================================================

# =====================================================================
# 顺序执行列表 —— 放入列表的测试项按顺序执行，注释/移除则不执行
# =====================================================================
SEQUENCE_TESTS = [
    REF_VTRM,
    SLP_REF_VTRM,
    LDO12_VTRM,
    SLP12_VTRM,
    ADC_VREFH_VTRM,
    HSI_FTRM,
    LSI_FTRM,
]

def RUN_SEQUENCE():
    """按 SEQUENCE_TESTS 列表顺序执行测试项，单项异常不影响后续项"""
    total = len(SEQUENCE_TESTS)
    if total == 0:
        print("SEQUENCE_TESTS 为空，无测试项可执行")
        return
    summary = []
    for idx, func in enumerate(SEQUENCE_TESTS, 1):
        name = getattr(func, "__name__", str(func))
        print(f"\n========== [{idx}/{total}] 开始执行: {name} ==========")
        try:
            func()
            summary.append((name, "完成"))
        except Exception as e:
            print(f"[{name}] 执行异常: {e}")
            summary.append((name, f"异常: {e}"))
    print("\n========== 顺序执行汇总 ==========")
    for name, status in summary:
        print(f"  {name}: {status}")

TEST_ITEMS = {
    "0": ("顺序执行(SEQUENCE_TESTS列表)",   RUN_SEQUENCE),
    #"1":  ("BG_TTRIM 修调",        BG_TTRIM),
    "1":  ("REF_VTRM 修调",        REF_VTRM),
    "2":  ("SLP_REF_VTRM 修调",    SLP_REF_VTRM),
    #"3":  ("REF_ITRM 修调",        REF_ITRM),
    "3":  ("LDO12_VTRM 修调",      LDO12_VTRM),
    "4":  ("SLP12_VTRM 修调",      SLP12_VTRM),
    "5":  ("ADC_VREFH_VTRM 修调",  ADC_VREFH_VTRM),
    #"8":  ("CSA_OS_VTRM 测试",     CSA_OS_VTRM),
    "6":  ("HSI_FTRM 修调",        HSI_FTRM),
    "7": ("LSI_FTRM 修调",        LSI_FTRM),
    "22": ("TJ_TTRM 测试",         TJ_TTRM),
    #"12": ("TS_ITRM 修调",         TS_ITRM),
    "8": ("写入修调值到Flash",     PROGRAM_TRIM),
    "9":("写入后复位测试",         Reread_test_select),
    "10": ("GPIO联通性和功能测试",  GPIO_toggle_test),
    "11": ("ADC DNL 测试(采样验证)",  ADC_DNL_Test),
    "12": ("ADC 校准调试",          ADC_CAL_TRIM),
    "13":  ("BG_TTRIM 修调",        BG_TTRIM),
    "14": ("SWD初始化(断电后重连)",  SWD_INIT),
    "15": ("Trim值扫描(查锁死值)",  TRIM_VALUE_SWEEP),
    "16": ("ADC RMSE 计算",  ADC_RMSE_TRIM),
    "17": ("DAC ADC 电压扫描",  DAC_ADC_VOLTAGE_SWEEP),
    "18": ("PLL_CKO 频率随输入电压变化关系",  PLL_CKO_VS_VIN_Test),
    "19": ("BG全量扫描线性度采集",  BG_NEW_TRIM),
    "20": ("ADC 循环采集(打印)",   ADC_LOOP_READ),
    "21": ("ADC 多量程x多电流档测试",   ADC_MULTI_RANGE_IBIAS_TEST),
    

    
    
    

}
