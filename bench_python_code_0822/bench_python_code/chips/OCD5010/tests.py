import time
import sys
import random
import os
import json
import openpyxl
from instrument import instrument_set as instr
from chips.OCD5010 import get_data as test_item
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
            _script_dir, "..", "..", "Save_Results", "OCD5010", "item_results.xlsx"
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
    save_dir = os.path.join(_script_dir, "..", "..", "Save_Results", "OCD5010")
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

def save_trim_to_excel(test_name, reg_name, reg_addr, best_trim, passed=None):
    """保存最佳TRIM值到Save_Results/OCD5010/trim_best_value.xlsx, 同测试项覆盖旧值"""
    save_dir = os.path.join(_script_dir, "..", "..", "Save_Results", "OCD5010")
    os.makedirs(save_dir, exist_ok=True)
    excel_path = os.path.join(save_dir, "trim_best_value.xlsx")
    headers = ["测试项名称", "寄存器地址名", "寄存器地址(Hex)", "最佳TRIM值", "测试结果"]
    try:
        if os.path.exists(excel_path):
            wb = openpyxl.load_workbook(excel_path)
            ws = wb.active
            existing_headers = [cell.value for cell in ws[1]]
            if existing_headers != headers:
                wb.close()
                # 旧文件只有4列, 重建表头
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
                    if len(row_data) < 5:
                        row_data.append(None)
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
                found = True
                break
        if not found:
            row_data = [test_name, reg_name, f"0x{reg_addr:08X}", best_trim]
            if passed is not None:
                row_data.append("PASS" if passed else "FAIL")
            ws.append(row_data)
        wb.save(excel_path)
    except Exception as e:
        print(f"保存TRIM值到Excel失败: {e}")

# 基于脚本所在目录构造Excel路径, 避免CWD影响
_script_dir = os.path.dirname(os.path.abspath(__file__))
_register_path = os.path.join(_script_dir, "..", "..", "test_configs", "OCD5010", "register.xlsx")
_testdata_path = os.path.join(_script_dir, "..", "..", "test_configs", "OCD5010", "test.xlsx")

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
        RW_ANA_BG_EN = test_item.get_32bit_reg_address("RW_ANA_BG_EN", df_register)
        PA6_AFS14_EN = test_item.get_32bit_reg_address("PA6_AFS14_EN", df_register)
        PA_ANA_EN = test_item.get_32bit_reg_address("PA_ANA_EN", df_register)

        RW_ATP_MUX = test_item.get_32bit_reg_address("RW_ATP_MUX", df_register)
        RW_ATP_MUX_EN = test_item.get_32bit_reg_address("RW_ATP_MUX_EN", df_register)

        RW_DTP_MUX = test_item.get_32bit_reg_address("RW_DTP_MUX", df_register)
        RW_DTP_MUX_EN = test_item.get_32bit_reg_address("RW_DTP_MUX_EN", df_register)

        ANAC_TRM2 = test_item.get_32bit_reg_address("ANAC_TRM2", df_register)
        RW_BG_TTRM = test_item.get_32bit_reg_address("RW_BG_TTRM", df_register)
    except Exception as e:
        print(f"❌ 寄存器地址获取失败: {e}")
        return
    
    print("VBG 修调 — BG_TTRM")
    
    #使能相关模块
    instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
    instr.swd_modify_register(RW_ANA_BG_EN, 0x01, 23, 23)
    instr.swd_modify_register(PA6_AFS14_EN, 0x0E, 24, 27)
    time.sleep(0.1)

    instr.swd_modify_register(RW_DTP_MUX_EN, 0x01, 13, 13)
    instr.swd_modify_register(RW_DTP_MUX, 0x09, 8, 12)
    time.sleep(0.1)

    instr.serial_send_and_receive(f"cmd_gpio_set_595d_test set {pin_cfg.GPIO6} {eq_cfg.DMM_MODE}\r\n")
    time.sleep(1)

    print(f"GPIO6输出电平为：{instr.dmm_measure_single_voltage()}")

    time.sleep(0.5)
    instr.serial_send_and_receive(f"cmd_gpio_set_595d_test reset\r\n")
    time.sleep(0.5)
    
    
    #确认 ATP_MUX 选中 VBG
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 6, 6)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 7, 7)
    instr.swd_modify_register(RW_ATP_MUX, 0x03, 0, 4)
    

    #读当前 BG_TTRM 默认值(30)
    trim = instr.swd_read_register(ANAC_TRM2)
    cur = (trim >> 25) & 0x3F
    
    bga_vol = instr.dmm_measure_single_voltage()
    passed = (bga_vol >= min_val and bga_vol <= max_val)
    if passed:
        print(f"VBG测试通过,修调范围为: {min_val}~{max_val} mV, 实际测量值: {bga_vol:.3f} mV, 无需修调")
        global_use["RW_BG_TTRM"] = RW_BG_TTRM
        global_use["BG_TTRM_BEST"] = cur
        save_trim_to_excel(test_item_name, "RW_BG_TTRM", RW_BG_TTRM, cur, passed)
        save_item_result(test_item_name, passed, round(bga_vol, 3))
        instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
        time.sleep(0.5)
        return
    else:
        print(f"VBG测试不通过,修调范围为: {min_val}~{max_val} mV, 实际测量值: {bga_vol:.3f} mV, 需要进行修调")

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
    range_ttrm = range(trim_target - 1, trim_target + 1)
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
        print(f"修调范围为: {min_val}~{max_val} {unit}, 实际测量值: {best_vol:.3f} mV")
    else:
        print("BG_TTRIM Test failed")
        print(f"修调范围为: {min_val}~{max_val} {unit}, 实际测量值: {best_vol:.3f} mV")
    instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
    time.sleep(1)

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
        RW_ANA_REF_EN = test_item.get_32bit_reg_address("RW_ANA_REF_EN", df_register)
        PA5_AFS14_EN = test_item.get_32bit_reg_address("PA5_AFS14_EN", df_register)
        PA6_AFS14_EN = test_item.get_32bit_reg_address("PA6_AFS14_EN", df_register)
        PA_ANA_EN = test_item.get_32bit_reg_address("PA_ANA_EN", df_register)

        RW_ATP_MUX = test_item.get_32bit_reg_address("RW_ATP_MUX", df_register)
        RW_ATP_MUX_EN = test_item.get_32bit_reg_address("RW_ATP_MUX_EN", df_register)

        RW_DTP_MUX = test_item.get_32bit_reg_address("RW_DTP_MUX", df_register)
        RW_DTP_MUX_EN = test_item.get_32bit_reg_address("RW_DTP_MUX_EN", df_register)

        ANAC_TRM1 = test_item.get_32bit_reg_address("ANAC_TRM1", df_register)
        RW_REF_VTRM = test_item.get_32bit_reg_address("RW_REF_VTRM", df_register)
    except Exception as e:
        print(f"❌ 寄存器地址获取失败: {e}")
        return

    print("Reference 电压修调 — REF_VTRM")

    #使能相关模块
    instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
    instr.swd_modify_register(RW_ANA_REF_EN, 0x01, 24, 24)
    instr.swd_modify_register(PA6_AFS14_EN, 0x0E, 24, 27)
    time.sleep(0.1)

    instr.swd_modify_register(RW_DTP_MUX_EN, 0x01, 13, 13)
    instr.swd_modify_register(RW_DTP_MUX, 0x0A, 8, 12)
    time.sleep(0.1)

    instr.serial_send_and_receive(f"cmd_gpio_set_595d_test set {pin_cfg.GPIO6} {eq_cfg.DMM_MODE}\r\n")
    time.sleep(1)

    print(f"GPIO6输出电平为：{instr.dmm_measure_single_voltage()}")

    time.sleep(0.5)
    instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
    time.sleep(1)

    #确认 ATP_MUX 选中 VREF
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 6, 6)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 7, 7)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)
    instr.swd_modify_register(RW_ATP_MUX, 0x02, 0, 4)
    time.sleep(0.1)
    trim = instr.swd_read_register(ANAC_TRM1)
    cur = (trim >> 0) & 0x1f

    v_a = instr.dmm_measure_single_voltage()
    passed = (v_a >= min_val_vref and v_a <= max_val_vref)
    if passed :
        print(f"VREF测试通过,修调范围为: {min_val_vref}~{max_val_vref} mV, 实际测量值: {v_a:.3f} mV, 无需修调")
        global_use["RW_REF_VTRM"] = RW_REF_VTRM
        global_use["REF_VTRM_BEST"] = cur
        save_trim_to_excel(test_item_name, "RW_REF_VTRM", RW_REF_VTRM, cur, passed)
        save_item_result(test_item_name, passed, round(v_a, 3))
        instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
        time.sleep(0.5)
        return
    else:
        print(f"VREF测试不通过,修调范围为: {min_val_vref}~{max_val_vref} mV, 实际测量值: {v_a:.3f} mV, 需要进行修调")

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
    range_vtrm = range(trim_target - 3, trim_target + 3)
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
    save_trim_to_excel(test_item_name, "RW_REF_VTRM", RW_REF_VTRM, best_trim, passed)
    save_item_result(test_item_name, passed, round(best_vol, 3))
    if passed:
        print("REF_VTRM Test pass")
        print(f"修调范围为: {min_val_vref}~{max_val_vref} mV, 实际测量值: {best_vol:.3f} mV")
    else:
        print("REF_VTRM Test failed")
        print(f"修调范围为: {min_val_vref}~{max_val_vref} mV, 实际测量值: {best_vol:.3f} mV")
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

    range_itrm = range(trim_target - 3, trim_target + 3)
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

    #使能相关模块
    instr.swd_modify_register(PA_ANA_EN, 0x20, 0, 15)
    instr.swd_modify_register(RW_SLP_REF_EN, 0x01, 4, 4)
    time.sleep(0.1)

    #确认 ATP_MUX 选中 IREF
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x00, 6, 6)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 7, 7)
    instr.swd_modify_register(RW_ATP_MUX, 0x01, 0, 4)
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)

    trim = instr.swd_read_register(ANAC_TRM1)
    cur = (trim >> 14) & 0x0F

    v_a = instr.dmm_measure_single_voltage()
    passed = (v_a >= min_val and v_a <= max_val)
    if passed :
        print(f"SLP_VREF测试通过,修调范围为: {min_val}~{max_val} mV, 实际测量值: {v_a:.3f} mV, 无需修调")
        global_use["RW_SLP_REF_VTRM"] = RW_SLP_REF_VTRM
        global_use["SLP_REF_VTRM_BEST"] = cur
        save_trim_to_excel(test_item_name, "RW_SLP_REF_VTRM", RW_SLP_REF_VTRM, cur, passed)
        save_item_result(test_item_name, passed, round(v_a, 3))
        instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
        time.sleep(0.5)
        return
    else:
        print(f"SLP_VREF测试不通过,修调范围为: {min_val}~{max_val} mV, 实际测量值: {v_a:.3f} mV, 需要进行修调")
    
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
    range_vtrm = range(trim_target - 10, trim_target + 10)
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
        print(f"修调范围为: {min_val}~{max_val} mV, 实际测量值: {best_vol:.3f} mV")
    else:
        print("SLP_REF_VTRM Test failed")
        print(f"修调范围为: {min_val}~{max_val} mV, 实际测量值: {best_vol:.3f} mV")
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
    cur = (trim >> 9) & 0x07

    v_a = instr.dmm_measure_single_voltage()

    passed = (v_a >= min_val and v_a <= max_val)
    if passed :
        print(f"LDO12_VTRM测试通过,修调范围为: {min_val}~{max_val} mV, 实际测量值: {v_a:.3f} mV, 无需修调")
        global_use["RW_LDO12_VTRM"] = RW_LDO12_VTRM
        global_use["LDO12_VTRM_BEST"] = cur
        save_trim_to_excel(test_item_name, "RW_LDO12_VTRM", RW_LDO12_VTRM, cur, passed)
        save_item_result(test_item_name, passed, round(v_a, 3))
        instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
        time.sleep(0.5)
        return
    else:
        print(f"LDO12_VTRM测试不通过,修调范围为: {min_val}~{max_val} mV, 实际测量值: {v_a:.3f} mV, 需要进行修调")

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
        print(f"修调范围为: {min_val}~{max_val} mV, 实际测量值: {best_vol:.3f} mV")
    else:
        print("LDO12_VTRM Test failed")
        print(f"修调范围为: {min_val}~{max_val} mV, 实际测量值: {best_vol:.3f} mV")
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
    cur = (trim >> 12) & 0x3

    v_a = instr.dmm_measure_single_voltage()

    passed = (v_a >= min_val and v_a <= max_val)
    if passed :
        print(f"SLP12_VTRM测试通过,修调范围为: {min_val}~{max_val} mV, 实际测量值: {v_a:.3f} mV, 无需修调")
        global_use["RW_SLP12_VTRM"] = RW_SLP12_VTRM
        global_use["SLP12_VTRM_BEST"] = cur
        save_trim_to_excel(test_item_name, "RW_SLP12_VTRM", RW_SLP12_VTRM, cur, passed)
        save_item_result(test_item_name, passed, round(v_a, 3))
        instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
        time.sleep(0.5)
        return
    else:
        print(f"SLP12_VTRM测试不通过,修调范围为: {min_val}~{max_val} mV, 实际测量值: {v_a:.3f} mV, 需要进行修调")

    new = min(3, cur + 1)
    instr.swd_modify_register(RW_SLP12_VTRM, new, 12, 13)
    time.sleep(0.5)
    v_b = instr.dmm_measure_single_voltage()

    step = v_b - v_a
    if abs(step) > 0.00001:
        trim_target = round((typ_val - v_a) / step) + cur
        trim_target = max(0, min(3, trim_target))
    else:
        trim_target = cur

    best_trim = trim_target

    min_err = float('inf')

    #复测，在计算出的修调范围内获取最佳修调值
    range_vtrm = range(trim_target - 1, trim_target + 1)
    for test_trim in range_vtrm:
        if test_trim < 0 or test_trim > 3:
            continue
        instr.swd_modify_register(RW_SLP12_VTRM, test_trim, 12, 13)
        time.sleep(0.5)
        test_vol = instr.dmm_measure_single_voltage()
        test_err = abs(test_vol - typ_val)
        print(f"测试修调值={test_trim}, 测试电压={test_vol}, 与目标周期误差={test_err}mV")

        if test_err < min_err:
            best_trim = test_trim
            min_err = test_err

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
        print(f"修调范围为: {min_val}~{max_val} mV, 实际测量值: {best_vol:.3f} mV")
    else:
        print("SLP12_VTRM Test failed")
        print(f"修调范围为: {min_val}~{max_val} mV, 实际测量值: {best_vol:.3f} mV")
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
    instr.swd_modify_register(RW_ADC_VREFH, 0x01, 16, 17)
    instr.swd_modify_register(RW_ANA_VREFH_EN, 0x01, 18, 18)
    time.sleep(0.1)

    trim = instr.swd_read_register(ANAC_TRM1)
    cur = (trim >> 18) & 0x7

    v_a = instr.dmm_measure_single_voltage()
    passed = (v_a >= min_val and v_a <= max_val)
    if passed :
        print(f"ADC_VREFH_VTRM测试通过,修调范围为: {min_val}~{max_val} mV, 实际测量值: {v_a:.3f} mV, 无需修调")
        global_use["RW_ADC_VREFH_VTRM"] = RW_ADC_VREFH_VTRM
        global_use["ADC_VREFH_VTRM_BEST"] = cur
        save_trim_to_excel(test_item_name, "RW_ADC_VREFH_VTRM", RW_ADC_VREFH_VTRM, cur, passed)
        save_item_result(test_item_name, passed, round(v_a, 3))
        instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
        time.sleep(0.5)
        return
    else:
        print(f"ADC_VREFH_VTRM测试不通过,修调范围为: {min_val}~{max_val} mV, 实际测量值: {v_a:.3f} mV, 需要进行修调")

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
    range_vtrm = range(trim_target - 7, trim_target + 7)
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
        print(f"修调范围为: {min_val}~{max_val} mV, 实际测量值: {best_vol:.3f} mV")
    else:
        print("ADC_VREFH_VTRM Test failed")
        print(f"修调范围为: {min_val}~{max_val} mV, 实际测量值: {best_vol:.3f} mV")
    instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
    time.sleep(1)

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
    
    
    #使能相关模块
    instr.swd_modify_register(PA6_AFS14_EN, 0x0E, 24, 27)
    instr.swd_modify_register(RW_ANA_HSI_EN, 0x01, 26, 26)

    #确认 ATP_MUX 选中 VREFH
    instr.swd_modify_register(RW_DTP_MUX, 0x13, 8, 12)
    instr.swd_modify_register(RW_DTP_MUX_EN, 0x01, 13, 13)

    trim = instr.swd_read_register(ANAC_TRM2)
    cur = (trim >> 0) & 0x7F

    # 测频率、周期、占空比（测量前先自动设置，适配当前信号频率）
    instr.osc_auto_set()
    f_a, p_a, d_a = instr.osc_measure_frequency("CH1")

    passed = (f_a >= min_val and f_a <= max_val)
    if passed :
        print(f"HSI_FTRM测试通过,修调范围为: {min_val}~{max_val} Hz, 实际测量值: {f_a:.3f} Hz, 无需修调")
        global_use["RW_HSI_FTRM"] = RW_HSI_FTRM
        global_use["HSI_FTRM_BEST"] = cur
        save_trim_to_excel(test_item_name, "RW_HSI_FTRM", RW_HSI_FTRM, cur, passed)
        save_item_result(test_item_name, passed, round(f_a, 3))
        instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
        time.sleep(0.5)
        return
    else:
        print(f"HSI_FTRM测试不通过,修调范围为: {min_val}~{max_val} Hz, 实际测量值: {f_a:.3f} Hz, 需要进行修调")

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

    #复测，在计算出的修调范围内获取最佳修调值
    range_vtrm = range(trim_target - 3, trim_target + 3)
    for test_trim in range_vtrm:
        if test_trim < 0 or test_trim > 127:
            continue
        instr.swd_modify_register(RW_HSI_FTRM, test_trim, 0, 6)
        time.sleep(0.5)
        instr.osc_auto_set()
        t_f, t_p, t_d = instr.osc_measure_frequency("CH1")
        test_err = abs(t_p - typ_target)
        print(f"测试修调值={test_trim}, 测试周期={t_p}us, 与目标周期误差={test_err}us")

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
        print(f"修调范围为: {min_val}~{max_val} Hz, 实际测量值: {b_f:.3f} Hz")
    else:
        print("HSI_FTRM Test failed")
        print(f"修调范围为: {min_val}~{max_val} Hz, 实际测量值: {b_f:.3f} Hz")
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
    
    
    #使能相关模块
    instr.swd_modify_register(PA6_AFS14_EN, 0x0E, 24, 27)
    instr.swd_modify_register(RW_LSI_32K_EN, 0x01, 3, 3)

    #确认 ATP_MUX 选中 VREFH
    instr.swd_modify_register(RW_DTP_MUX, 0x12, 8, 12)
    instr.swd_modify_register(RW_DTP_MUX_EN, 0x01, 13, 13)

    trim = instr.swd_read_register(ANAC_TRM2)
    cur = (trim >> 7) & 0x7F

    # 测频率、周期、占空比（测量前先自动设置，适配当前信号频率）
    instr.osc_auto_set()
    f_a, p_a, d_a = instr.osc_measure_frequency("CH1")
    passed = (f_a >= min_val and f_a <= max_val)
    if passed :
        print(f"LSI_FTRM测试通过,修调范围为: {min_val}~{max_val} Hz, 实际测量值: {f_a:.3f} Hz, 无需修调")
        global_use["RW_LSI_FTRM"] = RW_LSI_FTRM
        global_use["LSI_FTRM_BEST"] = cur
        save_trim_to_excel(test_item_name, "RW_LSI_FTRM", RW_LSI_FTRM, cur, passed)
        save_item_result(test_item_name, passed, round(f_a, 3))
        instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
        time.sleep(0.5)
        return
    else:
        print(f"LSI_FTRM测试不通过,修调范围为: {min_val}~{max_val} Hz, 实际测量值: {f_a:.3f} Hz, 需要进行修调")

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

    #复测，在计算出的修调范围内获取最佳修调值
    range_vtrm = range(trim_target - 3, trim_target + 3)
    for test_trim in range_vtrm:
        if test_trim < 0 or test_trim > 127:
            continue
        instr.swd_modify_register(RW_LSI_FTRM, test_trim, 7, 13)
        time.sleep(0.5)
        instr.osc_auto_set()
        t_f, t_p, t_d = instr.osc_measure_frequency("CH1")
        test_err = abs(t_p - p_target)
        print(f"测试修调值={test_trim}, 测试周期={t_p}us, 与目标周期误差={test_err}us")

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
        print(f"修调范围为: {min_val}~{max_val} Hz, 实际测量值: {b_f:.3f} Hz")
    else:
        print("LSI_FTRM Test failed")
        print(f"修调范围为: {min_val}~{max_val} Hz, 实际测量值: {b_f:.3f} Hz")
    instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
    time.sleep(1)


@test_case              #wait
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
    instr.serial_send_and_receive(f"cmd_gpio_set_595d_test set {pin_cfg.GPIO5} {eq_cfg.DMM_MODE}\r\n")
    time.sleep(0.5)
    
    TJ_REF_ADC = 0b10011
    TJ_OTW_ATP = 0b10100

    
    #确认 ATP_MUX 选中 VREFH
    
    instr.swd_modify_register(RW_ATP_MUX_EN, 0x01, 5, 5)
    time.sleep(0.1)
    trim = instr.swd_read_register(ANAC_TRM2)
    cur = (trim >> 14) & 0x1F

    print("请将温度设置为20℃, 待温度稳定后按下enter继续......")
    input()
    instr.swd_modify_register(RW_ATP_MUX, TJ_REF_ADC, 0, 4)
    time.sleep(0.1)
    v_ref_20c = instr.dmm_measure_single_voltage() * 1000
    instr.swd_modify_register(RW_ATP_MUX, TJ_OTW_ATP, 0, 4)
    time.sleep(0.1)
    v_otw_20c = instr.dmm_measure_single_voltage() * 1000

    print("请将温度设置为50℃, 待温度稳定后按下enter继续......")
    input()
    instr.swd_modify_register(RW_ATP_MUX, TJ_REF_ADC, 0, 4)
    time.sleep(0.1)
    v_ref_50c = instr.dmm_measure_single_voltage() * 1000
    instr.swd_modify_register(RW_ATP_MUX, TJ_OTW_ATP, 0, 4)
    time.sleep(0.1)
    v_otw_50c = instr.dmm_measure_single_voltage() * 1000
    
    slope_otw = (v_otw_50c - v_otw_20c) / 30  # V/℃ (30℃温差)
    slope_ref = (v_ref_50c - v_ref_20c) / 30 

    # 在 T1 点的差值
    delta = v_ref_20c - v_otw_20c
    denom = slope_otw - slope_ref
    if abs(denom) > 1e-10:
        trigger_t = 20 + delta / denom
    else:
        trigger_t = 20  # 退化为默认

    if abs(trigger_t - typ_val) < 5.0:
        print(f"\n  ✓ 当前 OTW 触发温度 {trigger_t:.1f}℃ 已接近目标 {typ_val}℃, 无需修调")
        return cur
    

    # 改为 +1 后重复
    new = min(31, cur + 1)
    instr.swd_modify_register(RW_TJ_TTRM, new, 14, 18)
    print(f"\n修改 TJ_TTRM = {new} 重测...")

    instr.swd_modify_register(RW_ATP_MUX, TJ_REF_ADC, 0, 4)
    time.sleep(0.1)
    v_ref_20b = instr.dmm_measure_single_voltage() * 1000
    instr.swd_modify_register(RW_ATP_MUX, TJ_OTW_ATP, 0, 4)
    time.sleep(0.1)
    v_otw_20b = instr.dmm_measure_single_voltage() * 1000
    delta_20b = v_ref_20b - v_otw_20b
    slope_diff = slope_otw - slope_ref
    if abs(slope_diff) > 1e-10:
        trigger_t_b = 20 + delta_20b / slope_diff
    else:
        trigger_t_b = trigger_t

    step = trigger_t_b - trigger_t  # ℃/LSB

    if abs(step) < 0.5:
        print("  ⚠️ 修调步长过小( < 0.5℃/LSB ), 保持默认值")
        trim_target = cur
    else:
        trim_target = round((typ_val - trigger_t) / step) + cur
        trim_target = max(0, min(31, trim_target))

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
    instr.serial_send_and_receive(f"cmd_gpio_set_595d_test set {pin_cfg.GPIO5} {eq_cfg.DMM_MODE}\r\n")
    time.sleep(0.5)

    print("GPIO6需要串联一个10K电阻到地，然后按下enter继续......")
    input()
    instr.swd_modify_register(RW_TS_GPIO5_EN, 0x01, 18, 18)
    instr.swd_modify_register(RW_TS_GPIO6_EN, 0x01, 19, 19)
    time.sleep(0.1)

    trim = instr.swd_read_register(ANAC_TRM2)
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

    range_itrm = range(trim_target - 3, trim_target + 3)
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


@test_case
def ADC_DNL_Test():
    """ADC DNL 测试 —— 软件 ADC 循环单通道采样（交互量程/通道 + 外部 DAC 电压）

    实现：交互选择参考电压量程(CR[21:20])与采样通道(SEQR0[4:0]) + ADC 初始化(4步)
    + 外部 DAC 设置输入电压(串口: cmd_gpio_set_595d_test set 38 7 开通路 + spi_cmd_set_val mV)
    + 软件触发(OTR=0x5AA5) + 轮询完成(IR.S0CDIF) + 读 DATA[0] 的循环采样。
    暂不包含：DNL 判定、DMM 实测、结果保存等（后续步骤逐步补充）。
    寄存器地址已与 SFR / ADC_Reg.h / ODS_5010.h 三方核对；CR 位域按新版 UM 260720。
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

    # ---------- 0. 交互式选择参考量程与采样通道 ----------
    VREFH_OPT = {  # 选项: (VREFH码[21:20], 量程mV, 是否旁路VDD33)
        "1": (0b00, 1500, 0), "2": (0b01, 2000, 0),
        "3": (0b10, 2500, 0), "4": (0b11, 3000, 0),
        "5": (0b11, 3300, 1),   # 旁路 VDD33(≈3.3V): VREFH_BYPS[17]=1, 量程=VDD33
    }
    print("\n请选择 ADC 参考电压量程 (VREFH):")
    print("  1. 1.5V   2. 2.0V   3. 2.5V   4. 3.0V   5. 旁路VDD33(≈3.3V, 默认3V)")
    vrefh_code, VREFH_mV, vrefh_byps = VREFH_OPT.get(input("请输入选项(1/2/3/4/5): ").strip(), (0b11, 3000, 0))
    mode = "旁路VDD33" if vrefh_byps else "内部VREFH"
    print(f"  已选择量程: {VREFH_mV} mV ({mode})")

    try:
        ch = int(input("请输入 ADC 采样通道号 (0~30, 默认23=PA7): ").strip() or "23")
        ch = 0 if ch < 0 else (30 if ch > 30 else ch)
    except ValueError:
        ch = 23
    ADC_CH = ch
    ch_name = CH_NAME.get(ADC_CH, "ADC_CH%02d" % ADC_CH)
    print(f"  已选择通道: ADC_CH{ADC_CH} ({ch_name})")

    # ---------- 1. 初始化 ADC 通路（4 步，CPU halt 下 SWD 可完成） ----------
    instr.swd_modify_register(RCC_ANACKE, 0x01, 0, 0)            # ① 使能 ADC 时钟 ADCCKE=1
    instr.swd_modify_register(ADC_SEQR0, ADC_CH, 0, 4)           # ② SEQ0 = 所选通道
    instr.swd_modify_register(ADC_TGNR,   1, 0, 7)               # ② TGN0 = 1（组0 1个通道）
    instr.swd_modify_register(ADC_TGSR,   0b0001, 16, 19)        # ② TRIG_EN = 仅组0
    instr.swd_modify_register(ADC_TGSR,   0b1000,  0,  3)        # ② TRIG_SEL0 = 8 (软件)
    instr.swd_write_register(ADC_TCFG,    0x0A4020F4)            # ② 复位默认时序(INIT≠0)
    # ③ 按所选量程写 CR：ADC_EN + CLK=2M + VREFH_EN(bit16) + VREFH码 + (旁路时 VREFH_BYPS[17]=1)
    CR_VAL = 0x00000031 | (vrefh_code << 20) | 0x00010000
    if vrefh_byps:
        CR_VAL |= 0x00020000                    # VREFH_BYPS=1 → 量程=VDD33(≈3.3V)
    instr.swd_write_register(ADC_CR, CR_VAL)
    # (可选) 模拟电源域上电, 参考 pga_demo
    # instr.swd_write_register(ANAC_ENCR, 0x03840001)
    time.sleep(0.1)

    # ---------- 2. 电压模式选择(与2810一致) + 打开外部 DAC 通路 ----------
    # 打开通路：cmd_gpio_set_595d_test set 后每个参数为一个独立通路编号，可一次打开多条：
    #   38=万用表测量通路；7=DAC2→母线；<bench_pin_bit>=母线→所选 ADC 通道
    dac_bit = BENCH_PIN_BIT.get(ADC_CH)
    if dac_bit is not None:
        instr.serial_send(f"cmd_gpio_set_595d_test set 38 7 {dac_bit}\r\n")
    else:
        instr.serial_send("cmd_gpio_set_595d_test set 38 7\r\n")  # 仅万用表 + DAC→母线
        print(f"⚠️ ADC_CH{ADC_CH}({ch_name}) 无 bench_pin_bit 映射，未开启母线→通道通路。")
    time.sleep(0.2)

    # DAC2 增益：输出 = 设置电压×2（DAC1 为×1）；目标输入电压上限 = 量程 95%
    DAC_GAIN = 2
    vin_max = int(VREFH_mV * 0.95)

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
        dac_mv = max(0, min(vin // DAC_GAIN, int(VREFH_mV * 0.95 / DAC_GAIN)))
        instr.serial_send(f"spi_cmd_set_val mV {dac_mv}\r\n")   # 只发送不等待响应(避免串口读阻塞)
        time.sleep(0.005)                               # DAC 输出/母线建立
        # 万用表实测（按间隔；间隔>1 时中间点沿用最近实测值）
        if dmm_interval > 0 and (i % dmm_interval == 0):
            last_vol = instr.dmm_measure_single_voltage()
        vol = last_vol
        instr.swd_write_register(ADC_OTR, 0x5AA5)      # 软件触发一次
        time.sleep(0.001)                              # 等转换完成(2M sps)后再轮询, 减少 J-Link 往返
        done = False
        for _ in range(20):                            # 轮询 IR.S0CDIF (bit0), 超时保护
            if instr.swd_read_register(ADC_IR) & 0x1:
                done = True
                break
            time.sleep(0.0005)
        if not done:
            print(f"  [{i+1:>4}/{N}] ⚠️ 触发超时")
            continue
        raw = instr.swd_read_register(ADC_DATA0) & 0xFFF
        conv_mv = raw / 4095.0 * VREFH_mV                  # 转换电压(mV)
        all_raw.append({"seq": i + 1, "vin": vin, "dac_mv": dac_mv, "raw": raw, "vol": vol})
        # 打印: 计数 / adc_code / 转换电压mV / 实测电压mV
        if dmm_interval > 0:
            print(f"  [{i+1:>4}/{N}] adc={raw:4d} 转换={conv_mv:7.2f}mV 实测={vol:7.2f}mV")
        else:
            print(f"  [{i+1:>4}/{N}] adc={raw:4d} 转换={conv_mv:7.2f}mV")
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
                ws.append(headers)
                for r in all_raw:
                    conv_mv = r["raw"] / 4095.0 * VREFH_mV
                    ws.append([r["seq"], r["vin"], r["dac_mv"], r["raw"], round(conv_mv, 3),
                               round(r["vol"], 3) if r["vol"] is not None else "",
                               round(dnl_by_code.get(r["raw"]), 3) if r["raw"] in dnl_by_code else "",
                               round(inl_by_code.get(r["raw"]), 3) if r["raw"] in inl_by_code else ""])
                ws.append([])
                ws.append(["DNL max(LSB)", round(dnl_max, 3)])
                ws.append(["INL max(LSB)", round(inl_max, 3)])
                ws.append(["量程(mV)", VREFH_mV])
                ws.append(["通道", "ADC_CH%d(%s)" % (ADC_CH, ch_name)])
                ws.append(["总采样点数", len(all_raw)])
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
    instr.swd_modify_register(ADC_CR, 0x00, 0, 0)                # 关闭 ADC_EN
    instr.serial_send("cmd_gpio_set_595d_test reset\r\n")        # 只发送不等待响应


# =====================================================================
# 导入修调值写入模块
# =====================================================================
from chips.OCD5010.trim.program_trim import program_trim_values as _program_trim_values

def PROGRAM_TRIM():
    """将修调测试获得的最佳值烧写到 OTP 槽位 (自动判定烧写次数, 4 次机会)"""
    _program_trim_values()

# =====================================================================
# 测试项映射表 —— 供 main.py 动态加载菜单
# 每个芯片定义自己的测试项列表，main.py 根据 CHIP 变量自动切换
# 格式: { key: (显示名称, 测试函数) }
#     其中 key 为菜单选择键（数字字符串）
#     测试函数为无参可调用对象
# =====================================================================

TEST_ITEMS = {
    "1":  ("BG_TTRIM 修调",        BG_TTRIM),
    "2":  ("REF_VTRM 修调",        REF_VTRM),
    "3":  ("SLP_REF_VTRM 修调",    SLP_REF_VTRM),
    #"3":  ("REF_ITRM 修调",        REF_ITRM),
    "4":  ("LDO12_VTRM 修调",      LDO12_VTRM),
    "5":  ("SLP12_VTRM 修调",      SLP12_VTRM),
    "6":  ("ADC_VREFH_VTRM 修调",  ADC_VREFH_VTRM),
    #"8":  ("CSA_OS_VTRM 测试",     CSA_OS_VTRM),
    "7":  ("HSI_FTRM 修调",        HSI_FTRM),
    "8": ("LSI_FTRM 修调",        LSI_FTRM),
    #"11": ("TJ_TTRM 测试",         TJ_TTRM),
    #"12": ("TS_ITRM 修调",         TS_ITRM),
    "9": ("写入修调值到Flash",     PROGRAM_TRIM),
    "10": ("GPIO联通性和功能测试",  GPIO_toggle_test),
    "11": ("ADC DNL 测试(采样验证)",  ADC_DNL_Test),
}
