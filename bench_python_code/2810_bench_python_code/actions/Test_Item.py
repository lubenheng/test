import time
import sys
import random
import os
import openpyxl
from instrument import instrument_set as instr
from actions import get_data as test_item

#默认上电 VIN = 5V / 0.3A   (目前是测试阶段，仅需上电超过4V不超过5V即可)

"""
1. 该脚本是测试执行脚本, 所有测试项均在test_configs/test.xlsx中定义好测试项名称、测试参数等, 以及在test_configs/register.xlsx中定义好寄存器名称和地址
2. 每个测试项对应一个函数, 函数名可以自定义, 但建议以测试项名称开头, 方便维护和调用
3. 该脚本只是执行测试项, 该自动化测试平台需要在该项目的主目录下的main.py中启动, 通过main.py中的交互式菜单选择需要执行的测试项
4. 每个测试函数需要完成以下工作:
    4.1 从test.xlsx中读取该测试项的测试参数(如Min/Typ/Max等), 以及需要用到的寄存器地址(通过register.xlsx查询)
    4.2 根据测试需求编写测试逻辑, 包括但不限于:
        - 通过instr模块控制测试设备(如示波器、万用表等)进行测量
        - 通过instr模块控制芯片寄存器进行设置
        - 根据测量结果和测试参数判断测试是否通过
    4.3 将测试结果以excel格式或者方便整理数据的格式保存到Save_Results目录下, 方便后续分析
5. 该代码是基于test_configs目录下的OCS-QA-Q2-010-07-OCD2810  Final Test specifications V2.1.0.docx文档中的测试项编写的, 目前已经完成了其中的部分测试项, 
    后续会逐渐完善剩余测试项的自动化脚本
    该脚本不是最终版本, 目前处于测试阶段, 后续会根据测试情况不断优化和完善,如有错误地方请以上面提到的.docx文档为准, 以免造成不必要的麻烦, 
"""

#这里存放全局变量, 某个函数的某个变量需要在其他函数使用可以存入这个字典
global_use = {"LDO_TRIM":0, "LIRC_TRIM":0, "HIRC_TRIM":0, "ADC_TRIM":0, "VREF_TRIM":0, "Voffset_M":0, "Voffset_P":0}

def save_item_result(test_name, passed, actual_value=None):
    """保存测试结果到 Save_Results/item_results.xlsx"""
    save_dir = os.path.join(_script_dir, "..", "Save_Results")
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
    """保存最佳TRIM值到Save_Results/trim_best_value.xlsx, 同测试项覆盖旧值"""
    save_dir = os.path.join(_script_dir, "..", "Save_Results")
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
_register_path = os.path.join(_script_dir, "..", "test_configs", "register.xlsx")
_testdata_path = os.path.join(_script_dir, "..", "test_configs", "test.xlsx")

try:
    df_register = test_item.load_excel_data(_register_path)
    df_test_data = test_item.load_excel_data(_testdata_path)
except Exception as e:
    print(f"Excel数据加载失败: {e}")
    sys.exit(1)

def frange(start, stop, step=1.0):
    current = start
    while current < stop:
        yield current
        current += step

#need to trim
def OSC33K_Test():
    """5.3 OSC33K自动化修调测试"""
    test_item_name = "OSC_33K"
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
        Pad1_ana_en = test_item.get_32bit_reg_address("Pad1_ana_en", df_register)
        RW_TEST0_SEL = test_item.get_32bit_reg_address("RW_TEST0_SEL", df_register)
        RW_TEST0_EN = test_item.get_32bit_reg_address("RW_TEST0_EN", df_register)
        RW_OSC_33K_TRIM = test_item.get_32bit_reg_address("RW_OSC_33K_TRIM", df_register)
    except Exception as e:
        print(f"❌ 寄存器地址获取失败: {e}")
        return
    
    #下面这个注释的意思是需要设置输入电压为5V, 目前没有电源设备接口, 先提示用户手动设置电压, 之后有了电源设备接口了再替换成自动化控制脚本
    #instr.serial_send("spi_cmd_set_val mV 5000\r\n")

    print("请将示波器的CH1探头接入P13引脚, 按下enter键继续...")
    input()
    time.sleep(0.2)
    try:
        #初始化、写入测试指令
        instr.swd_modify_register(Pad1_ana_en, 0x1, 11, 11)
        instr.swd_modify_register(RW_TEST0_SEL, 0x07, 16, 18)
        instr.swd_modify_register(RW_TEST0_EN, 0x1, 15, 15)
        time.sleep(0.1)
    except Exception as e:
        print(f"❌ 寄存器写入/万用表读取失败: {e}")
        return

    typ_val_period = 1000.0 / float(typ_val)   #测试项写的是频率, 需要转换成周期
    print(f"测试目标频率为type_val={typ_val * 1000} Hz, 目标周期为{typ_val_period:.3f} us")

    instr.osc_auto_set()
    time.sleep(1)

    instr.swd_modify_register(RW_OSC_33K_TRIM, 0x40, 16, 22)
    time.sleep(0.2)
    OSC33KA_freq, OSC33KA_period, OSC33KA_duty = instr.osc_measure_frequency("CH1")
    if OSC33KA_freq is None:
        print("❌ 示波器测量 OSC33KA 失败！请检查 CH1 探头是否接好 P13 引脚")
        return
    print(f"OSC33K频率A: {OSC33KA_freq:.3f} Hz, 周期: {OSC33KA_period:.3f} us")

    instr.swd_modify_register(RW_OSC_33K_TRIM, 0x41, 16, 22)
    time.sleep(0.2)
    OSC33KB_freq, OSC33KB_period, OSC33KB_duty = instr.osc_measure_frequency("CH1")
    if OSC33KB_freq is None:
        print("❌ 示波器测量 OSC33KB 失败！请检查 CH1 探头是否接好 P13 引脚")
        return
    print(f"OSC33K频率B: {OSC33KB_freq:.3f} Hz, 周期: {OSC33KB_period:.3f} us")

    delta_osc = OSC33KB_period - OSC33KA_period
    min_error = float('inf')

    if abs(delta_osc) < 1e-6:
        print(f"⚠️ OSC33K修调码线性插值失败(分母为0), 切换到全局遍历模式(0~127)")
        search_range = range(28, 40)
    else:
        TRIMCODE = (typ_val_period - OSC33KA_period) / delta_osc + 64
        TRIM_CALC = round(TRIMCODE)     #四舍五入取整
        search_range = range(TRIM_CALC - 4, TRIM_CALC + 6)
        print(f"OSC33K修调码计算结果: {TRIMCODE:.2f}, 四舍五入取整: {TRIM_CALC}, 搜索范围: {search_range.start} ~ {search_range.stop - 1}")

    for test_trim in search_range:
        if test_trim < 0 or test_trim > 127:
            continue
        instr.swd_modify_register(RW_OSC_33K_TRIM, test_trim, 16, 22)
        time.sleep(0.2)
        #复测，计算test_period与typ_val的绝对误差，取最小值对应的最优trim
        test_freq, test_period, test_duty = instr.osc_measure_frequency("CH1")
        if test_freq is None:
            print(f"  ⚠️ 修调码 {test_trim}: 示波器测量无效，跳过")
            continue
        test_error = abs(test_period - typ_val_period)
        print(f"测试修调码 {test_trim}: 频率={test_freq:.3f} Hz, 周期={test_period:.3f} us, 与目标周期误差={test_error:.3f} us")

        if test_error < min_error:
            min_error = test_error
            best_trim = test_trim

    instr.swd_modify_register(RW_OSC_33K_TRIM, best_trim, 16, 22)
    time.sleep(0.2)
    global_use["LIRC_TRIM"] = RW_OSC_33K_TRIM
    global_use["OSC33K_BEST_TRIM"] = best_trim
    final_freq, final_period, final_duty = instr.osc_measure_frequency("CH1")
    if final_freq is None:
        print("❌ 最终测量失败，无法确认修调结果")
        save_item_result(test_item_name, False, "测量失败")
        return
    print(f"最终OSC33K频率: {final_freq:.3f} Hz, 周期: {final_period:.3f} us")

    final_freq_khz = final_freq / 1000  #注意示波器测试出来是的频率单位是Hz, 需要转换到KHz进行比较
    passed = (final_freq_khz >= min_val and final_freq_khz <= max_val)
    save_trim_to_excel(test_item_name, "RW_OSC_33K_TRIM", RW_OSC_33K_TRIM, best_trim, passed)
    save_item_result(test_item_name, passed, round(final_freq_khz, 3))
    if passed:
        print("OSC33K Test pass")
    else:
        print("OSC33K Test failed")
        print(f"修调范围为: {min_val}~{max_val} {unit}, 实际测量值: {final_freq_khz:.3f} {unit}")
    """保存结果"""

#need to trim
def OSC24M_Test():
    """5.4 OSC24M自动化修调测试"""
    test_item_name = "OSC_24M"
    #获取测试数据、寄存器地址
    row = df_test_data[df_test_data["测试项"] == test_item_name]
    if row.empty:
        print(f"未找到测试项: {test_item_name}")
        return
    try:
        min_val = float(row["Min."].values[0])
        typ_val = float(row["Typ."].values[0])
        max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
        #TARGET_PERIOD = 1000.0 / typ_val
    except (IndexError, ValueError) as e:
        print(f"❌ 测试参数读取失败: {e}")
        return
    
    try:
        Pad1_ana_en = test_item.get_32bit_reg_address("Pad1_ana_en", df_register)
        RW_TEST0_SEL = test_item.get_32bit_reg_address("RW_TEST0_SEL", df_register)
        RW_TEST0_EN = test_item.get_32bit_reg_address("RW_TEST0_EN", df_register)
        RW_OSC_24M_TRIM = test_item.get_32bit_reg_address("RW_OSC_24M_TRIM", df_register)
    except Exception as e:
        print(f"❌ 寄存器地址获取失败: {e}")
        return
    
    #instr.serial_send("spi_cmd_set_val mV 5000\r\n")

    print("请将示波器的CH1探头接入P13引脚, 按下enter键继续...")
    input()
    time.sleep(0.2)
    try:
        #初始化、写入测试指令
        instr.swd_modify_register(Pad1_ana_en, 0x1, 11, 11)
        instr.swd_modify_register(RW_TEST0_SEL, 0x06, 16, 18)
        instr.swd_modify_register(RW_TEST0_EN, 0x1, 15, 15)
        time.sleep(0.1)
    except Exception as e:
        print(f"❌ 寄存器写入/万用表读取失败: {e}")
        return
    
    typ_val_period = 1000.0 / float(typ_val)   #测试项写的是频率, 需要转换成周期
    print(f"测试目标频率为type_val={typ_val * 1000} Hz, 目标周期为{typ_val_period:.3f} us")

    instr.osc_auto_set()
    time.sleep(1)
    #准备测试数据
    instr.swd_modify_register(RW_OSC_24M_TRIM, 0x20, 8, 13)
    time.sleep(0.2)
    OSC24MA_freq, OSC24MA_period, OSC24MA_duty = instr.osc_measure_frequency("CH1")
    if OSC24MA_freq is None:
        print("❌ 示波器测量 OSC24MA 失败！请检查 CH1 探头连接")
        return
    print(f"OSC24M频率A: {OSC24MA_freq:.3f} Hz, 周期: {OSC24MA_period:.3f} us")

    instr.swd_modify_register(RW_OSC_24M_TRIM, 0x21, 8, 13)
    time.sleep(0.2)
    OSC24MB_freq, OSC24MB_period, OSC24MB_duty = instr.osc_measure_frequency("CH1")
    if OSC24MB_freq is None:
        print("❌ 示波器测量 OSC24MB 失败！请检查 CH1 探头连接")
        return
    print(f"OSC24M频率B: {OSC24MB_freq:.3f} Hz, 周期: {OSC24MB_period:.3f} us")

    delta_osc = OSC24MB_period - OSC24MA_period
    min_error = float('inf')

    if abs(delta_osc) < 1e-6:
        print(f"⚠️ OSC24M修调码线性插值失败(分母为0), 切换到全局遍历模式(0~63)")
        search_range = range(30, 38)
    else:
        TRIMCODE = (typ_val_period - OSC24MA_period) / delta_osc + 32.0
        TRIM_CALC = round(TRIMCODE)
        search_range = range(TRIM_CALC - 4, TRIM_CALC + 6)
        print(f"OSC24M修调码计算结果: {TRIMCODE:.2f}, 四舍五入取整: {TRIM_CALC}, 搜索范围: {search_range.start} ~ {search_range.stop - 1}")

    for test_trim in search_range:
        if test_trim < 0 or test_trim > 63:
            continue
        instr.swd_modify_register(RW_OSC_24M_TRIM, test_trim, 8, 13)
        time.sleep(0.2)
        #复测，计算test_period与typ_val的绝对误差，取最小值对应的最优trim
        test_freq, test_period, test_duty = instr.osc_measure_frequency("CH1")
        if test_freq is None:
            print(f"  ⚠️ 修调码 {test_trim}: 示波器测量无效，跳过")
            continue
        test_error = abs(test_period - typ_val_period)
        print(f"测试修调码 {test_trim}: 频率={test_freq:.3f} Hz, 周期={test_period:.3f} us, 与目标周期误差={test_error:.3f} us")

        if test_error < min_error:
            min_error = test_error
            best_trim = test_trim

    instr.swd_modify_register(RW_OSC_24M_TRIM, best_trim, 8, 13)
    time.sleep(0.2)
    global_use["HIRC_TRIM"] = RW_OSC_24M_TRIM
    global_use["OSC24M_BEST_TRIM"] = best_trim
    final_freq, final_period, final_duty = instr.osc_measure_frequency("CH1")
    if final_freq is None:
        print("❌ 最终测量失败，无法确认修调结果")
        save_item_result(test_item_name, False, "测量失败")
        return
    print(f"最终OSC24M频率: {final_freq:.3f} Hz, 周期: {final_period:.3f} us")

    final_freq_khz = final_freq / 1000  #注意示波器测试出来是的频率单位是Hz, 需要转换到KHz进行比较
    passed = (final_freq_khz >= min_val and final_freq_khz <= max_val)
    save_trim_to_excel(test_item_name, "RW_OSC_24M_TRIM", RW_OSC_24M_TRIM, best_trim, passed)
    save_item_result(test_item_name, passed, round(final_freq_khz, 3))
    if passed:
        print("OSC24M Test pass")
    else:
        print("OSC24M Test failed")
        print(f"修调范围为: {min_val}~{max_val} {unit}, 实际测量值: {final_freq_khz:.3f} {unit}")
    """保存结果"""

#need to trim
def VBG_Test():
    """5.6 VBG 自动化修调测试"""
    test_item_name = "VBG"
    # 读取VBG测试规格参数
    row = df_test_data[df_test_data["测试项"] == test_item_name]
    if row.empty:
        print(f"未找到测试项: {test_item_name}")
        return False
    try:
        min_val = float(row["Min."].values[0])  # VBG最小值
        typ_val = float(row["Typ."].values[0])  # VBG典型值( 目标值) 
        max_val = float(row["Max."].values[0])  # VBG最大值
        unit = str(row["Unit"].values[0])       # 电压单位( 如V/mV) 
    except (IndexError, ValueError) as e:
        print(f"❌ VBG测试参数读取失败: {e}")
        return False
    
    # 获取VBG相关寄存器地址( 从register.xlsx解析) 
    try:
        Pad2_ana_en = test_item.get_32bit_reg_address("Pad2_ana_en", df_register) 
        RW_TEST1_SEL = test_item.get_32bit_reg_address("RW_TEST1_SEL", df_register)
        RW_TEST1_EN = test_item.get_32bit_reg_address("RW_TEST1_EN", df_register) 
        RW_VBG_TRIM = test_item.get_32bit_reg_address("RW_VBG_TRIM", df_register)
    except Exception as e:
        print(f"❌ VBG寄存器地址获取失败: {e}")
        return False
    
    #instr.serial_send("spi_cmd_set_val mV 5000\r\n")
    print("将ADC模块输出线接入VCC_S引脚, 万用表表笔接入P20引脚, 按下enter键继续...")
    input()
    time.sleep(0.2)

    try:
        # 写入使能/模式配置
        instr.swd_modify_register(Pad2_ana_en, 0xff, 16 ,23)
        instr.swd_modify_register(RW_TEST1_SEL, 0x0, 19, 22)

        # 这里VCC_S输入 1200mV电压
        instr.serial_send("spi_cmd_set_val mV 1200\r\n")
        time.sleep(0.3)

        instr.swd_modify_register(RW_TEST1_EN, 0x1, 16, 16)
        time.sleep(0.1)  # 等待模块稳定
    except Exception as e:
        print(f"❌ VBG寄存器初始化失败: {e}")
        return False
    
    VS = instr.dmm_measure_single_voltage()
    voffset = VS - 1200.0

    # 这里需要检测引脚输出
    instr.swd_modify_register(RW_TEST1_SEL, 0x6, 19, 22)
    time.sleep(0.1)
    
    best_trim = 0
    min_error = 9999.0
    final_vbg = 0.0

    for trim in range(0, 8):  # 3位TRIM: 000~111
        # 写入TRIM值
        instr.swd_modify_register(RW_VBG_TRIM, trim, 24, 26)
        time.sleep(0.1)
        # 测量+计算真实电压
        v_meas = instr.dmm_measure_single_voltage()
        vbg_real = v_meas - voffset
        error = abs(vbg_real - typ_val)

        print(f"TRIM={trim:>2} | 测量={v_meas:>6.2f}mV | 真实={vbg_real:>6.2f}mV | 误差={error:>5.2f}mV")

        # 筛选: 符合规格 + 误差最小
        if min_val <= vbg_real <= max_val and error < min_error:
            min_error = error
            best_trim = trim
            final_vbg = vbg_real

    instr.swd_modify_register(RW_VBG_TRIM, best_trim, 24, 26)
    global_use["VBG_BEST_TRIM"] = best_trim
    passed = (min_val <= final_vbg <= max_val)
    save_trim_to_excel(test_item_name, "RW_VBG_TRIM", RW_VBG_TRIM, best_trim, passed)
    save_item_result(test_item_name, passed, round(final_vbg, 2))
    print(f"\n🎯 VBG 修调完成! 最优TRIM={best_trim} | 最终电压={final_vbg:.2f}mV")
    if passed:
        print("✅ VBG 修调结果: PASS")
        return True
    else:
        print("❌ VBG 修调结果: FAIL")
        return False
    """保存结果"""

#need to trim
def VPTAT_Test():
    """5.6 VPTAT 自动化修调测试"""
    test_item_name = "VPTAT"
    # 读取VPTAT测试规格参数
    row = df_test_data[df_test_data["测试项"] == test_item_name]
    if row.empty:
        print(f"未找到测试项: {test_item_name}")
        return False
    try:
        typ_val = float(row["Typ."].values[0])  # 典型值( 目标值) 
        unit = str(row["Unit"].values[0])       # 电压单位( 如V/mV) 
    except (IndexError, ValueError) as e:
        print(f"❌ VPTAT测试参数读取失败: {e}")
        return False
    
    # 获取VBG相关寄存器地址( 从register.xlsx解析) 
    try:
        Pad2_ana_en = test_item.get_32bit_reg_address("Pad2_ana_en", df_register) 
        RW_TEST1_SEL = test_item.get_32bit_reg_address("RW_TEST1_SEL", df_register)
        RW_TEST1_EN = test_item.get_32bit_reg_address("RW_TEST1_EN", df_register) 
        RW_VPTAT_TRIM = test_item.get_32bit_reg_address("RW_VPTAT_TRIM", df_register)
    except Exception as e:
        print(f"❌ VPTAT寄存器地址获取失败: {e}")
        return False
    
    #instr.serial_send("spi_cmd_set_val mV 5000\r\n")
    print("将ADC模块输出线接入VCC_S引脚, 万用表表笔接入P20引脚, 按下enter键继续...")
    input()
    time.sleep(0.2)

    try:
        # 写入使能/模式配置
        instr.swd_modify_register(Pad2_ana_en, 0xff, 16 ,23)
        instr.swd_modify_register(RW_TEST1_SEL, 0x0, 19, 22)

        # 这里VCC_S输入 700mV电压
        instr.serial_send("spi_cmd_set_val mV 700\r\n")
        time.sleep(0.3)

        instr.swd_modify_register(RW_TEST1_EN, 0x1, 16, 16)
        time.sleep(0.1)  # 等待模块稳定
    except Exception as e:
        print(f"❌ VBG寄存器初始化失败: {e}")
        return False
    
    VS = instr.dmm_measure_single_voltage()
    voffset = VS - 700.0

    # 检测引脚输出
    instr.swd_modify_register(RW_TEST1_SEL, 0x4, 19, 22)
    time.sleep(0.1)
    
    best_trim = 0
    min_error = 9999.0
    final_vptat = 0.0
    
    for trim in range(0, 4):  # 2位TRIM: 00~11
        # 写入TRIM值
        instr.swd_modify_register(RW_VPTAT_TRIM, trim, 28, 29)
        time.sleep(0.1)
        # 测量+计算真实电压
        v_meas = instr.dmm_measure_single_voltage()
        vptat_real = v_meas - voffset
        error = abs(vptat_real - typ_val)

        print(f"TRIM={trim:>2} | 测量={v_meas:>6.2f}mV | 真实={vptat_real:>6.2f}mV | 误差={error:>5.2f}mV")

        # 筛选: 符合规格 + 误差最小
        if vptat_real >= typ_val and error < min_error:
            min_error = error
            best_trim = trim
            final_vptat = vptat_real

    instr.swd_modify_register(RW_VPTAT_TRIM, best_trim, 28, 29)
    global_use["VPTAT_BEST_TRIM"] = best_trim
    passed = (final_vptat >= typ_val)
    save_trim_to_excel(test_item_name, "RW_VPTAT_TRIM", RW_VPTAT_TRIM, best_trim, passed)
    save_item_result(test_item_name, passed, round(final_vptat, 2))
    print(f"\n🎯 VPTAT 修调完成! 最优TRIM={best_trim} | 最终电压={final_vptat:.2f}mV")
    if passed:
        print("✅ VPTAT 修调结果: PASS")
        return True
    else:
        print("❌ VPTAT 修调结果: FAIL")
        return False
    """保存结果"""

#need to trim
def VREF_Test():
    """5.7 VREF 自动化修调测试"""
    test_item_name = "VREF"
    #获取测试数据、寄存器地址
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
        Pad1_ana_en = test_item.get_32bit_reg_address("Pad1_ana_en", df_register)
        RW_VREF_EN = test_item.get_32bit_reg_address("RW_VREF_EN", df_register)
        RW_VHALF_EN = test_item.get_32bit_reg_address("RW_VHALF_EN", df_register)
        RW_VREF_SEL = test_item.get_32bit_reg_address("RW_VREF_SEL", df_register)
        RW_VREF_TRIM = test_item.get_32bit_reg_address("RW_VREF_TRIM", df_register)
        RW_VBG_TRIM = test_item.get_32bit_reg_address("RW_VBG_TRIM", df_register)
    except Exception as e:
        print(f"❌ 寄存器地址获取失败: {e}")
        return

    #instr.serial_send("spi_cmd_set_val mV 5000\r\n")
    print("请将万用表表笔接入P11引脚, 按下enter键继续...")
    input()
    time.sleep(0.2)

    try:
        #初始化、写入测试指令
        instr.swd_modify_register(Pad1_ana_en, 0xff, 8, 15)
        instr.swd_modify_register(RW_VREF_EN, 0x1, 8, 8)
        instr.swd_modify_register(RW_VHALF_EN, 0x1, 7, 7)
        
        instr.swd_modify_register(RW_VREF_SEL, 0x03, 0, 1)
        instr.swd_modify_register(RW_VBG_TRIM, 0x7, 24, 26)
        time.sleep(0.1)

        # 准备所需数据
        instr.swd_modify_register(RW_VREF_TRIM, 0x20, 0, 5)

        time.sleep(0.1)
        VREFA = instr.dmm_measure_single_voltage()
        print(f"初始VREFA测量值: {VREFA:.2f}{unit}")

        instr.swd_modify_register(RW_VREF_TRIM, 0x21, 0, 5)
        time.sleep(0.1)
        VREFB = instr.dmm_measure_single_voltage()
        print(f"初始VREFB测量值: {VREFB:.2f}{unit}")
    except Exception as e:
        print(f"❌ 寄存器写入/万用表读取失败: {e}")
        return

    delta_v = (VREFB - VREFA)   
    if abs(delta_v) < 1e-6:  # 避免除以0
        print(f"⚠️  delta_v={delta_v}{unit}, 修调码计算失败, 使用默认值32")
        best_trim = 32
    else:
        best_trim = int(round((typ_val - VREFA) / delta_v )+32)
        print(f"best_trim: {best_trim}")
        #best_trim = max(0, min(63, raw_trim))  # 强制限定在0~63( 6位TRIM最大值) 


    min_error = float('inf')  # 初始化无穷大, 避免魔法数999
    final_trim = best_trim
    final_voltage = 0

    """"""
    for i in range(-10, 10):
        try:
            trim_candidate = best_trim + i
            instr.swd_modify_register(RW_VREF_TRIM, trim_candidate, 0, 5)
            time.sleep(0.1)
            current_v = instr.dmm_measure_single_voltage()
            print(f"测试TRIM={trim_candidate}: 测量电压={current_v:.2f}{unit}")
            current_error = abs(current_v - typ_val)

            # 如果这个值误差更小, 就记录下来
            if current_error < min_error:
                min_error = current_error
                final_trim = trim_candidate
                final_voltage = current_v

        except Exception as e:
            print(f"⚠️  TRIM={trim_candidate} 测试失败: {e}, 跳过该值")
            continue

    # 将最优调码写入寄存器
    instr.swd_modify_register(RW_VREF_TRIM, final_trim, 0, 5)
    global_use["VREF_TRIM"] = RW_VREF_TRIM
    global_use["VREF_BEST_TRIM"] = final_trim
    global_use["Voffset_P"] = instr.dmm_measure_single_voltage()  # 存储Voffset_P供ADC测试使用
    passed = (min_val <= final_voltage <= max_val)
    save_trim_to_excel(test_item_name, "RW_VREF_TRIM", RW_VREF_TRIM, final_trim, passed)
    save_item_result(test_item_name, passed, round(final_voltage, 2))
    print(f"\n🎯 VREF 修调完成! 最优TRIM={final_trim} | 最终电压={final_voltage:.2f}{unit}")
    if passed:
        print("✅ 测试结果: PASS")
        return True  # 返回测试结果, 方便上层调用
    else:
        print(f"❌ 测试结果: FAIL(范围要求: {min_val}~{max_val}{unit})")
        return False
    """保存结果"""

def DAC0_DAC1_DNL_Test():
    """5.8 DAC0,DAC1 自动化修调测试"""
    dac0_test_item_name = "DAC0_DNL"
    dac1_test_item_name = "DAC1_DNL"
    dac0_row = df_test_data[df_test_data["测试项"] == dac0_test_item_name]
    dac1_row = df_test_data[df_test_data["测试项"] == dac1_test_item_name]
    if dac0_row.empty and dac1_row.empty:
        print(f"未找到测试项: {dac0_test_item_name} and {dac1_test_item_name}")
        return
    try:
        dac0_min_val = float(dac0_row["Min."].values[0])
        dac0_max_val = float(dac0_row["Max."].values[0])

        dac1_min_val = float(dac1_row["Min."].values[0])
        dac1_max_val = float(dac1_row["Max."].values[0])

        unit = str(dac0_row["Unit"].values[0])
    except (IndexError, ValueError) as e:
        print(f"❌ 测试参数读取失败: {e}")
        return
    
    try:
        Pad1_ana_en = test_item.get_32bit_reg_address("Pad1_ana_en", df_register)
        Pad2_ana_en = test_item.get_32bit_reg_address("Pad2_ana_en", df_register)
        RW_TEST1_SEL = test_item.get_32bit_reg_address("RW_TEST1_SEL", df_register)
        RW_TEST1_EN = test_item.get_32bit_reg_address("RW_TEST1_EN", df_register)
        RW_VREF_EN = test_item.get_32bit_reg_address("RW_VREF_EN", df_register)
        RW_VREF_SEL = test_item.get_32bit_reg_address("RW_VREF_SEL", df_register)
        RW_VREF_TRIM = test_item.get_32bit_reg_address("RW_VREF_TRIM", df_register)
        RW_DAC0_DIN = test_item.get_32bit_reg_address("RW_DAC0_DIN", df_register)
        RW_DAC1_DIN = test_item.get_32bit_reg_address("RW_DAC1_DIN", df_register)
        RW_DAC_EN = test_item.get_32bit_reg_address("RW_DAC_EN", df_register)
    except Exception as e:
        print(f"❌ DAC寄存器地址获取失败: {e}")
        return False
    
    #instr.serial_send("spi_cmd_set_val mV 5000\r\n")
    # print("请将ADC模块输出线接入VCC_S引脚, 万用表表笔接入P20引脚, 按下enter键继续...")
    # input()
    # time.sleep(1)

    #初始化配置
    # 1. 相关引脚和VREF 配置
    instr.swd_modify_register(0x4000141C, 0xff, 8, 15)    # P1模拟使能
    instr.swd_modify_register(0x40005000, 0x01, 8, 8)     # RW_VREF_EN
    # instr.swd_modify_register(0x40005008, 0x3, 0, 1)      # RW_VREF_SEL
    instr.swd_modify_register(0x40005000, 0x01, 7, 7)     # RW_VHALF_EN
    # instr.swd_modify_register(0x4000500C, 0x20, 0, 5)     # RW_VREF_TRIM
    instr.swd_modify_register(0x4000500C, 0x7, 24, 26)    # RW_VBG_TRIM

    instr.swd_modify_register(0x40005000, 0x01, 2, 2)     # RW_DAC_EN
    instr.swd_modify_register(0x40005004, 0x0, 24, 24)    # RW_DAC_MOD
    instr.swd_modify_register(0x4000141C, 0xff, 24, 31)   # P3模拟使能
    instr.swd_modify_register(0x40004408, 0x1, 7, 7)      # COMP 4使能
    instr.swd_modify_register(0x40004410, 0x1, 3, 3)      # COMP 5使能

    instr.swd_modify_register(RW_TEST1_SEL, 0x0, 19, 22)
    print("⚠️ 请用数字万用表连接P11,测量电压为3995~4000, 按回车继续...")
    input()
    print("⚠️ 请保持数字万用表连接P36, 按回车继续...")
    input()
    # """这里需要提示在VCC_S引脚接入2000mV基准电压"""
    # instr.serial_send("spi_cmd_set_val mV 2000\r\n")
    # print("⚠️ 请在VCC_S引脚接入2000mV基准电压后, 按回车继续...")
    # input()

    # instr.swd_modify_register(RW_TEST1_EN, 0x01, 16, 16)
    # time.sleep(0.2)
    # VS = instr.dmm_measure_single_voltage()
    # Voffset_M = VS - 2000.0
    # global_use["Voffset_M"] = Voffset_M     #存入Voffset_M让下一个函数使用

    # instr.swd_modify_register(RW_TEST1_SEL, 0x04, 19, 22)
    # time.sleep(0.1)
    # p20_0100_v = instr.dmm_measure_single_voltage()
    # print(f"📊 SEL=0100B时, P20输出 = {p20_0100_v} mV")

    # instr.swd_modify_register(RW_TEST1_SEL, 0x03, 19, 22)
    # time.sleep(0.1)
    # p20_0011_v = instr.dmm_measure_single_voltage()
    # print(f"📊 SEL=0011B时, P20输出 = {p20_0011_v} mV")

    # 输入DAC0测试步长
    dac0_step_input = input("请输入DAC0 CODE步进值 (1~510, 默认1): ").strip()
    try:
        dac0_step = int(dac0_step_input) if dac0_step_input else 1
        if dac0_step < 1 or dac0_step > 510:
            print(f"步进值超出范围(1~510), 使用默认步进1")
            dac0_step = 1
    except ValueError:
        print(f"输入无效: {dac0_step_input}, 使用默认步进1")
        dac0_step = 1

    dac0_code_list = list(range(1, 512, dac0_step))
    if dac0_code_list[-1] != 511:
        dac0_code_list.append(511)
    dac0_voltage_list = []  # 存储实测电压Vout
    dac0_error_list = []    # 存储误差 Vout - Videal
    dac0_dnl_list = []      # 存储【DNL值】
    dac0_save_data_list = []  # 用于存储DAC0测量数据, 以便保存到Excel
    LSB_IDEAL = 4000.0 / 512  # 理想1 LSB = 7.8125mV
    prev_error = None

    # 2. 循环测试每个码值
    for code in dac0_code_list:
        instr.swd_modify_register(RW_DAC0_DIN, code, 0, 8)
        time.sleep(0.02)

        # 使用万用表测量P36引脚的实际输出电压
        Vout = instr.dmm_measure_single_voltage()
        dac0_voltage_list.append(Vout)  # 保存电压

        # 计算理论电压、误差
        Videal0 = (code / 511) * 4000.0
        error = Vout - Videal0
        dac0_error_list.append(error)

        # 计算DNL = (当前误差 - 上一误差) / 理想1 LSB
        if prev_error is not None:
            dnl = (error - prev_error) / LSB_IDEAL
        else:
            dnl = 0
        dac0_dnl_list.append(dnl)
        prev_error = error

        dac0_save_data_list.append({
            "CODE": code,
            "Vout(mV)": round(Vout, 3),
            "Videal(mV)": round(Videal0, 3),
            "误差(mV)": round(error, 3),
            "DNL(LSB)": round(dnl, 3)
        })

        print(f" CODE={code:>3}, Vout={Vout:>7.3f}mV, Videal={Videal0:>7.3f}mV, 误差={error:>7.3f}mV, DNL={dnl:>7.3f}")

    # 4. 计算最终DNL( 取最大|DNL|) 
    DAC0_DNL = max(abs(dnl) for dnl in dac0_dnl_list)
    dac0_passed = (dac0_min_val <= DAC0_DNL and dac0_max_val >= DAC0_DNL)
    save_item_result(dac0_test_item_name, dac0_passed, round(DAC0_DNL, 3))
    if dac0_passed:
        print(f"\n✅ DAC0 DNL验证通过! 最大|DNL|={DAC0_DNL:.3f} LSB")
    else:
        print(f"\n❌ DAC0 DNL验证失败! 最大|DNL|={DAC0_DNL:.3f} LSB(阈值±{dac0_max_val} LSB)")

    # 提示是否保存DAC0测量数据为xlsx文件到桌面
    dac0_save_choice = input("\n是否保存DAC0测量数据为xlsx文件? (y/n, 默认n): ").strip().lower()
    if dac0_save_choice == 'y' or dac0_save_choice == 'yes':
        dac0_save_filename = input("请输入文件名 (直接回车使用默认名 DAC0_DNL_测量数据): ").strip()
        if not dac0_save_filename:
            dac0_save_filename = "DAC0_DNL_测量数据"
        if not dac0_save_filename.endswith(".xlsx"):
            dac0_save_filename += ".xlsx"
        desktop_path = os.path.join(os.path.expanduser("~"), "Desktop", dac0_save_filename)
        try:
            wb = openpyxl.Workbook()
            ws = wb.active
            ws.title = "DAC0_DNL测量数据"
            headers = list(dac0_save_data_list[0].keys()) if dac0_save_data_list else []
            ws.append(headers)
            for data_row in dac0_save_data_list:
                ws.append([data_row[h] for h in headers])
            ws.append([])
            ws.append(["DAC0 DNL最大|DNL|(LSB)", round(DAC0_DNL, 3)])
            ws.append(["测试结果", "PASS" if dac0_passed else "FAIL"])
            wb.save(desktop_path)
            print(f"✅ DAC0测量数据已保存到: {desktop_path}")
        except Exception as e:
            print(f"❌ 保存文件失败: {e}")

    # instr.swd_modify_register(RW_TEST1_SEL, 0x02, 19, 22)
    # time.sleep(0.2)
    # p20_0010_v = instr.dmm_measure_single_voltage()
    # print(f"📊 SEL=0010B时, P20输出 = {p20_0010_v} mV")
    # Voffset = Voffset_M     #这个 Voffset 需修改

    print("⚠️ 请切换数字万用表连接P37, 按回车继续...")
    input()

    # 输入DAC1测试步长
    dac1_step_input = input("请输入DAC1 CODE步进值 (1~63, 默认1): ").strip()
    try:
        dac1_step = int(dac1_step_input) if dac1_step_input else 1
        if dac1_step < 1 or dac1_step > 63:
            print(f"步进值超出范围(1~63), 使用默认步进1")
            dac1_step = 1
    except ValueError:
        print(f"输入无效: {dac1_step_input}, 使用默认步进1")
        dac1_step = 1

    dac1_code_list = list(range(1, 64, dac1_step))
    if dac1_code_list[-1] != 63:
        dac1_code_list.append(63)
    dac1_voltage_list = []  # 存储实测电压Vout
    dac1_error_list = []    # 存储误差 Vout - Videal
    dac1_dnl_list = []      # 存储【DNL值】
    dac1_save_data_list = []  # 用于存储DAC1测量数据, 以便保存到Excel
    LSB_IDEAL_1 = 4000.0 / 64  # 理想1 LSB = 62.5mV
    prev_error_1 = None

    # 2. 循环测试每个码值
    for code in dac1_code_list:
        instr.swd_modify_register(RW_DAC1_DIN, code, 16, 21)
        time.sleep(0.02)

        # 使用万用表测量实际输出电压
        Vout1 = instr.dmm_measure_single_voltage()
        dac1_voltage_list.append(Vout1)  # 保存电压

        # 计算理论电压、误差
        Videal1 = (code / 64) * 4000.0
        error1 = Vout1 - Videal1
        dac1_error_list.append(error1)

        # 计算DNL = (当前误差 - 上一误差) / 理想1 LSB
        if prev_error_1 is not None:
            dnl = (error1 - prev_error_1) / LSB_IDEAL_1
        else:
            dnl = 0
        dac1_dnl_list.append(dnl)
        prev_error_1 = error1

        dac1_save_data_list.append({
            "CODE": code,
            "Vout(mV)": round(Vout1, 3),
            "Videal(mV)": round(Videal1, 3),
            "误差(mV)": round(error1, 3),
            "DNL(LSB)": round(dnl, 3)
        })

        print(f" CODE={code:>3}, Vout={Vout1:>7.3f}mV, Videal={Videal1:>7.3f}mV, 误差={error1:>7.3f}mV, DNL={dnl:>7.3f}")

    # 4. 计算最终DNL( 取最大|DNL|) 
    DAC1_DNL = max(abs(dnl) for dnl in dac1_dnl_list)
    dac1_passed = (dac1_min_val <= DAC1_DNL and dac1_max_val >= DAC1_DNL)
    save_item_result(dac1_test_item_name, dac1_passed, round(DAC1_DNL, 3))
    if dac1_passed:
        print(f"\n✅ DAC1 DNL验证通过! 最大|DNL|={DAC1_DNL:.3f} LSB")
    else:
        print(f"\n❌ DAC1 DNL验证失败! 最大|DNL|={DAC1_DNL:.3f} LSB(阈值±{dac1_max_val} LSB)")

    # 提示是否保存DAC1测量数据为xlsx文件到桌面
    dac1_save_choice = input("\n是否保存DAC1测量数据为xlsx文件? (y/n, 默认n): ").strip().lower()
    if dac1_save_choice == 'y' or dac1_save_choice == 'yes':
        dac1_save_filename = input("请输入文件名 (直接回车使用默认名 DAC1_DNL_测量数据): ").strip()
        if not dac1_save_filename:
            dac1_save_filename = "DAC1_DNL_测量数据"
        if not dac1_save_filename.endswith(".xlsx"):
            dac1_save_filename += ".xlsx"
        desktop_path = os.path.join(os.path.expanduser("~"), "Desktop", dac1_save_filename)
        try:
            wb = openpyxl.Workbook()
            ws = wb.active
            ws.title = "DAC1_DNL测量数据"
            headers = list(dac1_save_data_list[0].keys()) if dac1_save_data_list else []
            ws.append(headers)
            for data_row in dac1_save_data_list:
                ws.append([data_row[h] for h in headers])
            ws.append([])
            ws.append(["DAC1 DNL最大|DNL|(LSB)", round(DAC1_DNL, 3)])
            ws.append(["测试结果", "PASS" if dac1_passed else "FAIL"])
            wb.save(desktop_path)
            print(f"✅ DAC1测量数据已保存到: {desktop_path}")
        except Exception as e:
            print(f"❌ 保存文件失败: {e}")
    """保存结果"""

def ADC_DNL_Test():
    """5.9 ADC 测试"""
    adc_test_item_name = "ADC_DNL"
    row = df_test_data[df_test_data["测试项"] == adc_test_item_name]
    if row.empty:
        print("未找到测试项: {adc_test_item_name}")
        return
    try:
        adc_min_val = float(row["Min."].values[0])
        adc_max_val = float(row["Max."].values[0])
        unit = str(row["Unit"].values[0])
    except (IndexError, ValueError) as e:
        print(f"❌ 测试参数读取失败: {e}")
        return
    
    try:
        Pad1_ana_en = test_item.get_32bit_reg_address("Pad1_ana_en", df_register)
        Pad2_ana_en = test_item.get_32bit_reg_address("Pad2_ana_en", df_register)
        RW_ADC_CLK = test_item.get_32bit_reg_address("RW_ADC_CLK", df_register)
        RW_ADC_CTRL_EN = test_item.get_32bit_reg_address("RW_ADC_CTRL_EN", df_register)
        RW_VREF_EN = test_item.get_32bit_reg_address("RW_VREF_EN", df_register)
        RW_VREF_SEL = test_item.get_32bit_reg_address("RW_VREF_SEL", df_register)
        RW_VREF_TRIM = test_item.get_32bit_reg_address("RW_VREF_TRIM", df_register)
        RW_ADC_EN = test_item.get_32bit_reg_address("RW_ADC_EN", df_register)
        cfg_I_sampl_en = test_item.get_32bit_reg_address("cfg_I_sampl_en", df_register)
        CH6EN = test_item.get_32bit_reg_address("CH6EN", df_register)
        ADCBSY = test_item.get_32bit_reg_address("ADCBSY", df_register)
        ADC_SCYC = test_item.get_32bit_reg_address("ADC_SCYC", df_register)
        ADC6_DR = test_item.get_32bit_reg_address("ADC6_DR", df_register)
    except Exception as e:
        print(f"❌ ADC寄存器地址获取失败: {e}")
        return False
    
    #instr.serial_send("spi_cmd_set_val mV 5000\r\n")
    time.sleep(0.2)

    instr.swd_modify_register(Pad1_ana_en, 0xff, 8, 15)
    instr.swd_modify_register(Pad2_ana_en, 0xff, 16, 23)
    instr.swd_modify_register(0x40001424, 0xffffffff, 0, 31)
    instr.swd_modify_register(RW_ADC_CLK, 0x1, 20, 20)
    # instr.swd_modify_register(RW_VREF_SEL, 0x03, 0, 1)
    instr.swd_modify_register(RW_VREF_EN, 0x01, 8, 8)
    # instr.swd_modify_register(RW_VREF_TRIM, global_use["VREF_TRIM"], 0, 5)
    instr.swd_modify_register(RW_ADC_EN, 0x01, 1, 1)
    instr.swd_modify_register(RW_ADC_CTRL_EN, 0x01, 7, 7)
    instr.swd_modify_register(cfg_I_sampl_en, 0x0, 14, 14)
    instr.swd_modify_register(CH6EN, 0x1, 6, 6)
    instr.swd_modify_register(ADC_SCYC, 0x7, 10, 13)

    time.sleep(0.005)

    print("⚠️ 请在P20引脚接入DAC模块输出线后.")
    print("⚠️ 将P20引脚连接数字万用表, 按回车继续...")
    input()

    Guding = 1000

    # 选择测试电压模式
    print("\n请选择ADC DNL测试电压模式:")
    print("1. 固定测试电压 (1000mV)")
    print("2. 随机测试电压 (10组0.1~4.0V)")
    print("3. 线性扫描测试电压 (500mV~4000mV, 步进100mV)")
    choice = input("请输入选项 (1/2/3): ").strip()

    random.seed(42)  # 固定随机种子
    if choice == '1':
        for i in range(1, 10):
            vin_list[i] = 1000
        print(f"已选择固定测试电压模式: {vin_list[0]*1000:.0f}mV, 共{len(vin_list)}个测试点")
    elif choice == '2':
        vin_list = [round(random.uniform(0.5, 4.0), 3) for _ in range(10)]
        print(f"已选择随机测试电压模式: 共{len(vin_list)}个测试点")
        for i, v in enumerate(vin_list):
            print(f"  点{i+1}: {v*1000:.0f}mV")
    else:
        step_input = input("请输入线性扫描步进值 (mV, 默认100mV): ").strip()
        try:
            step_mv = float(step_input) if step_input else 100.0
        except ValueError:
            print(f"输入无效: {step_input}, 使用默认步进100mV")
            step_mv = 100.0
        step_v = step_mv / 1000.0  # 转换为V单位
        vin_list = []
        for i in frange(0.5, 4.0, step_v):
            vin_list.append(i)
        print(f"已选择线性扫描测试电压模式: 500mV~4000mV, 步进{step_mv:.0f}mV, 共{len(vin_list)}个测试点")

    ADC_LSB_list = []
    dnl_errors = []
    save_data_list = []  # 用于存储测量数据, 以便保存到Excel

    for vin in vin_list:
        #print(f"⚠️ 请将P20电压调整为 {vin} mV, 调整完成后按回车继续...")
        #input()
        vin_set = vin * 1000  # 转换为mV单位
        instr.serial_send(f"spi_cmd_set_val mV {int(vin_set)}\r\n")
        time.sleep(0.01)  # 等待电压稳定

        vol = instr.dmm_measure_single_voltage()
        time.sleep(0.01)
        instr.swd_modify_register(ADCBSY, 0x01, 6, 6)
        time.sleep(0.01)
        get_adc_val = instr.swd_read_register(ADC6_DR) & 0xfff

        #计算实际输出电压
        vreal = get_adc_val / 4095 * global_use["Voffset_P"]
       
        #计算DNL误差
        dnl_error = vol - vreal
        ADC_LSB = global_use["Voffset_P"] / 4095
        dnl_error_lsb = dnl_error / ADC_LSB
        dnl_errors.append(dnl_error)
        ADC_LSB_list.append(dnl_error_lsb)
        save_data_list.append({
            "输入电压(mV)": int(vin_set),
            "万用表实测电压(mV)": round(vol, 3),
            "ADC原始数据": get_adc_val,
            "ADC转换电压(mV)": round(vreal, 3),
            "DNL误差(mV)": round(dnl_error, 3),
            "DNL误差(LSB)": round(dnl_error_lsb, 3)
        })
        print(f" ADC原始数据: {get_adc_val} 转换电压: {vreal:.3f} mV | 实测电压: {vol:.2f} mV | DNL误差: {dnl_error_lsb:.3f}")

    adc_dnl_max = max(map(abs, ADC_LSB_list))  # 取最大LSB误差作为最终DNL指标
    print(f"\n📊 ADC DNL最大误差: {adc_dnl_max:.3f}")
    print(f"⚠️ ADC DNL测试结果要求在 {adc_min_val} ~ {adc_max_val} 之间")
    adc_passed = (adc_min_val <= adc_dnl_max <= adc_max_val)
    save_item_result(adc_test_item_name, adc_passed, round(adc_dnl_max, 3))
    if adc_passed:
        print(f"✅ ADC_DNL测试通过! ")
    else:
        print(f"❌ ADC_DNL测试失败! 超出规格范围! ")
    """保存结果"""

    # 提示是否保存测量数据为xlsx文件到桌面
    save_choice = input("\n是否保存测量数据为xlsx文件? (y/n, 默认n): ").strip().lower()
    if save_choice == 'y' or save_choice == 'yes':
        save_filename = input("请输入文件名 (直接回车使用默认名 ADC_DNL_测量数据): ").strip()
        if not save_filename:
            save_filename = "ADC_DNL_测量数据"
        if not save_filename.endswith(".xlsx"):
            save_filename += ".xlsx"
        # 获取桌面路径
        desktop_path = os.path.join(os.path.expanduser("~"), "Desktop", save_filename)
        try:
            wb = openpyxl.Workbook()
            ws = wb.active
            ws.title = "ADC_DNL测量数据"
            # 写入表头
            headers = list(save_data_list[0].keys()) if save_data_list else []
            ws.append(headers)
            # 写入数据行
            for data_row in save_data_list:
                ws.append([data_row[h] for h in headers])
            # 在最后一行下面写入汇总信息
            ws.append([])
            ws.append(["ADC DNL最大误差(LSB)", round(adc_dnl_max, 3)])
            ws.append(["测试结果", "PASS" if adc_passed else "FAIL"])
            wb.save(desktop_path)
            print(f"✅ 测量数据已保存到: {desktop_path}")
        except Exception as e:
            print(f"❌ 保存文件失败: {e}")

    # 测试完成, 关闭通道和ADC
    instr.swd_write_register(CH6EN, 0x00 << 6)
    instr.swd_write_register(RW_ADC_EN, 0x00 << 1)

def Voffset_VS_ERROR_Test():
    """5.10 VCC_S 自动化脚本测试"""
    # 定义测试项名称
    voffset_test_item = "Voffset"
    vs_error_test_item = "VS_ERROR"
    
    # 从测试表格读取规格参数
    row_voffset = df_test_data[df_test_data["测试项"] == voffset_test_item]
    row_vs_error = df_test_data[df_test_data["测试项"] == vs_error_test_item]
    
    if row_voffset.empty or row_vs_error.empty:
        print(f"未找到测试项: {voffset_test_item} 或 {vs_error_test_item}")
        return
    
    try:
        # 读取Voffset_VS规格
        voffset_min = float(row_voffset["Min."].values[0])
        voffset_max = float(row_voffset["Max."].values[0])
        voffset_unit = str(row_voffset["Unit"].values[0])
        # 读取VS_ERROR规格
        vs_error_min = float(row_vs_error["Min."].values[0])
        vs_error_max = float(row_vs_error["Max."].values[0])
        vs_error_unit = str(row_vs_error["Unit"].values[0])
    except (IndexError, ValueError) as e:
        print(f"❌ VCC_S测试参数读取失败: {e}")
        return
    
    # 获取寄存器地址( 与项目统一寄存器名一致) 
    try:
        Pad1_ana_en = test_item.get_32bit_reg_address("Pad1_ana_en", df_register)
        Pad2_ana_en = test_item.get_32bit_reg_address("Pad2_ana_en", df_register)
        RW_TEST1_SEL = test_item.get_32bit_reg_address("RW_TEST1_SEL", df_register)
        RW_ADC_CLK = test_item.get_32bit_reg_address("RW_ADC_CLK", df_register)
        RW_ADC_CTRL_EN = test_item.get_32bit_reg_address("RW_ADC_CTRL_EN", df_register)
        RW_TEST1_EN = test_item.get_32bit_reg_address("RW_TEST1_EN", df_register)
        RW_VREF_EN = test_item.get_32bit_reg_address("RW_VREF_EN", df_register)
        RW_VREF_SEL = test_item.get_32bit_reg_address("RW_VREF_SEL", df_register)
        RW_VREF_TRIM = test_item.get_32bit_reg_address("RW_VREF_TRIM", df_register)
        cfg_I_sampl_en = test_item.get_32bit_reg_address("cfg_I_sampl_en", df_register)
        RW_ADC_EN = test_item.get_32bit_reg_address("RW_ADC_EN", df_register)
        CH10EN = test_item.get_32bit_reg_address("CH10EN", df_register)
        ADCBSY = test_item.get_32bit_reg_address("ADCBSY", df_register)
        ADC_SCYC = test_item.get_32bit_reg_address("ADC_SCYC", df_register)
        ADC10_DR = test_item.get_32bit_reg_address("ADC10_DR", df_register)
    except Exception as e:
        print(f"❌ VCC_S寄存器地址获取失败: {e}")
        return False
    
    #instr.serial_send("spi_cmd_set_val mV 5000\r\n")
    print("请将DAC模块输出线接入VCC_S引脚, 万用表表笔接入P20引脚, 按下enter键继续...")
    input()
    time.sleep(0.2)

    instr.swd_modify_register(Pad1_ana_en, 0xff, 8, 15)
    instr.swd_modify_register(Pad2_ana_en, 0xff, 16, 23)
    instr.swd_modify_register(RW_VREF_SEL, 0x03, 19, 22)
    instr.swd_modify_register(RW_VREF_EN, 0x01, 8, 8)
    instr.swd_modify_register(RW_VREF_TRIM, global_use["VREF_TRIM"], 0, 5)
    instr.swd_modify_register(RW_TEST1_SEL, 0x00, 19, 22)
    time.sleep(0.1)

    instr.serial_send("spi_cmd_set_val mV 3400\r\n")
    print("⚠️ 请在VCC_S引脚接入3400mV电压后, 按回车继续...")
    input()

    instr.swd_modify_register(RW_TEST1_EN, 0x01, 16, 16)
    time.sleep(0.1)
    VS = instr.dmm_measure_single_voltage()
    print(f"📊 万用表测量VCC_S电压 = {VS} mV")
    Voffset= VS - 3400
    voffset_passed = (Voffset >= voffset_min and Voffset <= voffset_max)
    save_item_result(voffset_test_item, voffset_passed, round(Voffset, 2))
    if voffset_passed:
        print(f"✅ Voffset 测试通过")
    else:
        print(f"❌ Voffset 测试失败(规格: {voffset_min}~{voffset_max}{voffset_unit})")

    #开始监测VS电压
    instr.swd_modify_register(RW_TEST1_EN, 0x0, 16, 16)
    instr.swd_modify_register(cfg_I_sampl_en, 0x0, 14, 14)
    instr.swd_modify_register(RW_ADC_CLK, 0x01, 20, 20)
    instr.swd_modify_register(RW_ADC_CTRL_EN, 0x01, 7, 7)
    instr.swd_modify_register(RW_ADC_EN, 0x01, 1, 1)
    instr.swd_modify_register(CH10EN, 0x01, 10, 10)
    time.sleep(0.1)

    instr.swd_modify_register(ADCBSY, 0x01, 6, 6)
    time.sleep(0.1)
    get_adc_val = instr.swd_read_register(ADC10_DR)

    CODEreal = get_adc_val & 0xFFF
    CODEideal = VS / 4000 * 4096
    VS_ERROR = CODEreal - CODEideal
    vs_error_passed = (VS_ERROR >= vs_error_min and VS_ERROR <= vs_error_max)
    save_item_result(vs_error_test_item, vs_error_passed, round(VS_ERROR, 1))
    if vs_error_passed:
        print(f"✅ VS_ERROR 测试通过")
    else:
        print(f"❌ VS_ERROR 测试失败(规格: {vs_error_min}~{vs_error_max}{vs_error_unit})")
    """保存结果"""

    instr.swd_modify_register(RW_ADC_EN, 0x00, 1, 1)
    instr.swd_modify_register(CH10EN, 0x00, 10, 10)
    instr.swd_modify_register(RW_VREF_EN, 0x0, 8, 8)
    instr.swd_modify_register(RW_TEST1_EN, 0x0, 16, 16)

def PGA_ADC_SH_PGA_Test():
    """5.11 PGA+ADC& SH+PGA测试"""
    # 定义测试项名称
    VAZ0_test_item = "VAZ0"
    VAZ1_test_item = "VAZ1"
    VAZ2_test_item = "VAZ2"
    IUerror_test_item = "IUerror"
    VUerror_test_item = "VUerror"
    IVerror_test_item = "IVerror"
    VVerror_test_item = "VVerror"
    IWerror_test_item = "IWerror"
    VWerror_test_item = "VWerror"

    # 从测试表格读取规格参数
    row_VAZ0 = df_test_data[df_test_data["测试项"] == VAZ0_test_item]
    row_VAZ1 = df_test_data[df_test_data["测试项"] == VAZ1_test_item]
    row_VAZ2 = df_test_data[df_test_data["测试项"] == VAZ2_test_item]
    row_IUerror = df_test_data[df_test_data["测试项"] == IUerror_test_item]
    row_VUerror = df_test_data[df_test_data["测试项"] == VUerror_test_item]
    row_IVerror = df_test_data[df_test_data["测试项"] == IVerror_test_item]
    row_VVerror = df_test_data[df_test_data["测试项"] == VVerror_test_item]
    row_IWerror = df_test_data[df_test_data["测试项"] == IWerror_test_item]
    row_VWerror = df_test_data[df_test_data["测试项"] == VWerror_test_item]

    if row_VAZ0.empty or row_VAZ1.empty or row_VAZ2.empty or row_IUerror.empty \
        or row_VUerror.empty or row_IVerror.empty or row_VVerror.empty or \
            row_IWerror.empty or row_VWerror.empty:
        print(f"未找到测试项")
        return

    try:
        VAZ0_min = float(row_VAZ0["Min."].values[0]) * 1000
        VAZ0_max = float(row_VAZ0["Max."].values[0]) * 1000
        VAZ1_min = float(row_VAZ1["Min."].values[0]) * 1000
        VAZ1_max = float(row_VAZ1["Max."].values[0]) * 1000
        VAZ2_max = float(row_VAZ2["Max."].values[0]) * 1000
        VAZ2_min = float(row_VAZ2["Min."].values[0]) * 1000
        IUerror_max = float(row_IUerror["Max."].values[0])
        VUerror_max = float(row_VUerror["Max."].values[0])
        IVerror_max = float(row_IVerror["Max."].values[0])
        VVerror_max = float(row_VVerror["Max."].values[0])
        IWerror_max = float(row_IWerror["Max."].values[0])
        VWerror_max = float(row_VWerror["Max."].values[0])


    except (IndexError, ValueError) as e:
        print(f"❌ PGA+ADC/SH+PGA测试参数读取失败: {e}")
        return

    try:
        Pad1_ana_en = test_item.get_32bit_reg_address("Pad1_ana_en", df_register)
        Pad2_ana_en = test_item.get_32bit_reg_address("Pad2_ana_en", df_register)
        Pad3_ana_en = test_item.get_32bit_reg_address("Pad3_ana_en", df_register)
        RW_ADC_CLK = test_item.get_32bit_reg_address("RW_ADC_CLK", df_register)
        RW_ADC_CTRL_EN = test_item.get_32bit_reg_address("RW_ADC_CTRL_EN", df_register)
        RW_VREF_EN = test_item.get_32bit_reg_address("RW_VREF_EN", df_register)
        RW_VREF_SEL = test_item.get_32bit_reg_address("RW_VREF_SEL", df_register)
        RW_VREF_TRIM = test_item.get_32bit_reg_address("RW_VREF_TRIM", df_register)
        RW_TEST1_EN = test_item.get_32bit_reg_address("RW_TEST1_EN", df_register)
        RW_TEST1_SEL = test_item.get_32bit_reg_address("RW_TEST1_SEL", df_register)
        RW_AMP_EN = test_item.get_32bit_reg_address("RW_AMP_EN", df_register)
        RW_PGA_EN = test_item.get_32bit_reg_address("RW_PGA_EN", df_register)
        RW_PGA_GAIN = test_item.get_32bit_reg_address("RW_PGA_GAIN", df_register)
        cfg_I_sampl_en = test_item.get_32bit_reg_address("cfg_I_sampl_en", df_register)
        CH10EN = test_item.get_32bit_reg_address("CH10EN", df_register)
        RW_ADC_EN = test_item.get_32bit_reg_address("RW_ADC_EN", df_register)
        ADCBSY = test_item.get_32bit_reg_address("ADCBSY", df_register)
        ADC_SCYC = test_item.get_32bit_reg_address("ADC_SCYC", df_register)
        cfg_az_en = test_item.get_32bit_reg_address("cfg_az_en", df_register)
        ana_az0_dat = test_item.get_32bit_reg_address("ana_az0_dat", df_register)
        ana_az1_dat = test_item.get_32bit_reg_address("ana_az1_dat", df_register)
        ana_az2_dat = test_item.get_32bit_reg_address("ana_az2_dat", df_register)

        ADC4_DR = test_item.get_32bit_reg_address("ADC4_DR", df_register)
        ADC0_DR = test_item.get_32bit_reg_address("ADC0_DR", df_register)
        ADC1_DR = test_item.get_32bit_reg_address("ADC1_DR", df_register)

    except Exception as e:
        print(f"❌ PGA+ADC/SH+PGA寄存器地址获取失败: {e}")
        return False
    
    #instr.serial_send("spi_cmd_set_val mV 5000\r\n")
    time.sleep(0.2)

    instr.swd_modify_register(Pad1_ana_en, 0xff, 8, 15)
    instr.swd_modify_register(Pad2_ana_en, 0xff, 16, 23)
    instr.swd_modify_register(Pad3_ana_en, 0xff, 24, 31)
    instr.swd_modify_register(RW_VREF_EN, 1, 8, 8)
    instr.swd_modify_register(RW_VREF_SEL, 0x03, 19, 22)
    instr.swd_modify_register(RW_VREF_TRIM, global_use["VREF_TRIM"], 23, 23)
    instr.swd_modify_register(RW_AMP_EN, 0x07, 3, 5)
    instr.swd_modify_register(RW_PGA_EN, 0x07, 11, 13)
    instr.swd_modify_register(RW_PGA_GAIN, 0x15, 8, 13)
    instr.swd_modify_register(RW_ADC_CLK, 0x01, 20, 20)
    instr.swd_modify_register(RW_ADC_CTRL_EN, 0x01, 7, 7)
    instr.swd_modify_register(cfg_I_sampl_en, 0x00, 14, 14)
    instr.swd_modify_register(RW_ADC_EN, 0x01, 1, 1)
    instr.swd_modify_register(CH10EN, 0x13, 0, 11)
    time.sleep(0.05)

    print("请将万用表接入P20引脚, 按回车继续...")
    input()
    instr.swd_modify_register(cfg_az_en, 0x01, 16, 16)
    time.sleep(0.000002)
    instr.swd_modify_register(RW_TEST1_EN, 0x01, 16, 16)

    #第一项测试项: VAZ0 测试P20输出电压
    print("请在P13、P14分别接入0V / -0.1V电压后, 按回车继续...")
    input()
    instr.swd_modify_register(RW_TEST1_SEL, 0x0C, 19, 22)
    instr.swd_modify_register(RW_TEST1_EN, 0x0, 16, 16)
    time.sleep(0.1)
    instr.swd_modify_register(ADCBSY, 0x01, 6, 6)
    ana_az0_data = instr.swd_read_register(ana_az0_dat) & 0xfff
    instr.swd_modify_register(RW_TEST1_EN, 0x1, 16, 16)
    time.sleep(0.1)
    print("VAZ0:读取P20输出电压")
    VAZ0 = instr.dmm_measure_single_voltage()
    vaz0_passed = (VAZ0 >= VAZ0_min and VAZ0 <= VAZ0_max)
    save_item_result(VAZ0_test_item, vaz0_passed, round(VAZ0, 2))
    if vaz0_passed:
        print("VAZ0 测试项通过")
    else:
        print("VAZ0 测试项不通过")
    """保存结果"""

    #第二项测试项: VAZ1 测试P20输出电压
    print("请在P16、P17分别接入0V / -0.1V电压后, 按回车继续...")
    input()
    instr.swd_modify_register(RW_TEST1_SEL, 0x0D, 19, 22)
    instr.swd_modify_register(RW_TEST1_EN, 0x0, 16, 16)
    time.sleep(0.1)
    instr.swd_modify_register(ADCBSY, 0x01, 6, 6)
    ana_az1_data = instr.swd_read_register(ana_az1_dat) & 0xfff
    instr.swd_modify_register(RW_TEST1_EN, 0x1, 16, 16)
    time.sleep(0.1)
    print("VAZ1:读取P20输出电压")
    VAZ1 = instr.dmm_measure_single_voltage()
    vaz1_passed = (VAZ1 >= VAZ1_min and VAZ1 <= VAZ1_max)
    save_item_result(VAZ1_test_item, vaz1_passed, round(VAZ1, 2))
    if vaz1_passed:
        print("VAZ1 测试项通过")
    else:
        print("VAZ1 测试项不通过")
    """保存结果"""


    #第三项测试项: VAZ2 测试P20输出电压
    print("请在P31、P32分别接入0V / -0.1V电压后, 按回车继续...")
    input()
    instr.swd_modify_register(RW_TEST1_SEL, 0x0E, 19, 22)
    instr.swd_modify_register(RW_TEST1_EN, 0x0, 16, 16)
    time.sleep(0.1)
    instr.swd_modify_register(ADCBSY, 0x01, 6, 6)
    ana_az2_data = instr.swd_read_register(ana_az2_dat) & 0xfff
    instr.swd_modify_register(RW_TEST1_EN, 0x1, 16, 16)
    time.sleep(0.1)
    print("VAZ2:读取P20输出电压")
    VAZ2 = instr.dmm_measure_single_voltage()
    vaz2_passed = (VAZ2 >= VAZ2_min and VAZ2 <= VAZ2_max)
    save_item_result(VAZ2_test_item, vaz2_passed, round(VAZ2, 2))
    if vaz2_passed:
        print("VAZ2 测试项通过")
    else:
        print("VAZ2 测试项不通过")
    """保存结果"""

    
    # 后续测试项
    """这里需要提示向三组引脚输入电压"""
    print("请分别在P13、P14和P16、P17和P31、P32接入0V / -0.1V电压后, 按回车继续...")
    input()
    
def COMP_Test():
    """ 5.12 COMP 自动修调测试"""
    # 定义测试项名称
    COMP0HYS_test_item = "COMP0HYS"
    COMP1HYS_test_item = "COMP1HYS"
    COMP2HYS_test_item = "COMP2HYS"
    COMP4HYS_test_item = "COMP4HYS"
    COMP5HYS_test_item = "COMP5HYS"
    COMP0HYS2_test_item = "COMP0HYS2"
    COMP1HYS2_test_item = "COMP1HYS2"
    COMP2HYS2_test_item = "COMP2HYS2"
    
    # 从测试表格读取规格参数
    row_COMP0HYS = df_test_data[df_test_data["测试项"] == COMP0HYS_test_item]
    row_COMP1HYS = df_test_data[df_test_data["测试项"] == COMP1HYS_test_item]
    row_COMP2HYS = df_test_data[df_test_data["测试项"] == COMP2HYS_test_item]
    row_COMP4HYS = df_test_data[df_test_data["测试项"] == COMP4HYS_test_item]
    row_COMP5HYS = df_test_data[df_test_data["测试项"] == COMP5HYS_test_item]
    row_COMP0HYS2 = df_test_data[df_test_data["测试项"] == COMP0HYS2_test_item]
    row_COMP1HYS2 = df_test_data[df_test_data["测试项"] == COMP1HYS2_test_item]
    row_COMP2HYS2 = df_test_data[df_test_data["测试项"] == COMP2HYS2_test_item]
    
    if row_COMP0HYS.empty and row_COMP1HYS.empty \
        and row_COMP2HYS.empty and row_COMP4HYS.empty \
        and row_COMP5HYS.empty  and row_COMP0HYS2.empty \
        and row_COMP1HYS2.empty  and row_COMP2HYS2.empty:
        print(f"未找到测试项")
        return
    
    try:
        # 读取COMPx规格
        COMP0HYS_min = float(row_COMP0HYS["Min."].values[0])
        COMP0HYS_max = float(row_COMP0HYS["Max."].values[0])
        COMP1HYS_min = float(row_COMP1HYS["Min."].values[0])
        COMP1HYS_max = float(row_COMP1HYS["Max."].values[0])
        COMP2HYS_max = float(row_COMP2HYS["Max."].values[0])
        COMP2HYS_min = float(row_COMP2HYS["Min."].values[0])
        COMP4HYS_max = float(row_COMP4HYS["Max."].values[0])
        COMP4HYS_min = float(row_COMP4HYS["Min."].values[0])
        COMP5HYS_max = float(row_COMP5HYS["Max."].values[0])
        COMP5HYS_min = float(row_COMP5HYS["Min."].values[0])
        COMP0HYS2_max = float(row_COMP0HYS2["Max."].values[0])
        COMP0HYS2_min = float(row_COMP0HYS2["Min."].values[0])
        COMP1HYS2_max = float(row_COMP1HYS2["Max."].values[0])
        COMP1HYS2_min = float(row_COMP1HYS2["Min."].values[0])
        COMP2HYS2_max = float(row_COMP2HYS2["Max."].values[0])
        COMP2HYS2_min = float(row_COMP2HYS2["Min."].values[0])

    except (IndexError, ValueError) as e:
        print(f"❌ VCC_S测试参数读取失败: {e}")
        return
    
    # 获取寄存器地址
    try:
        Pad1_ana_en = test_item.get_32bit_reg_address("Pad1_ana_en", df_register)
        cmp0en = test_item.get_32bit_reg_address("cmp0en", df_register)
        cmp4en = test_item.get_32bit_reg_address("cmp4en", df_register)
        Cmp5en = test_item.get_32bit_reg_address("Cmp5en", df_register)
        cmp0mod = test_item.get_32bit_reg_address("cmp0mod", df_register)
        cmp0hys = test_item.get_32bit_reg_address("cmp0hys", df_register)
        cmp1hys = test_item.get_32bit_reg_address("cmp1hys", df_register)
        Cmp2hys = test_item.get_32bit_reg_address("Cmp2hys", df_register)
        RW_TEST0_SEL = test_item.get_32bit_reg_address("RW_TEST0_SEL", df_register)
        RW_TEST0_EN = test_item.get_32bit_reg_address("RW_TEST0_EN", df_register)
        cmpsel = test_item.get_32bit_reg_address("cmpsel", df_register)
        cmp5out = test_item.get_32bit_reg_address("cmp5out", df_register)

    except Exception as e:
        print(f"❌ VCC_S寄存器地址获取失败: {e}")
        return False
    
    # 开始测试前配置 COMP 使能和时钟使能
    instr.swd_modify_register(0x40005000, 0x1, 6, 6)    # RW_AMP_CMP_EN
    instr.swd_modify_register(0x40005000, 0x1, 15, 15)    # RW_AMP_CMP_EN
    instr.swd_modify_register(0x40002408, 0x1, 31, 31)  # cmp_CLK_EN
    
    #instr.serial_send("spi_cmd_set_val mV 5000\r\n")

    time.sleep(0.2)

    instr.swd_modify_register(Pad1_ana_en, 0xffffff, 8, 31)
    instr.swd_modify_register(cmp0en, 0x01, 0, 0)
    instr.swd_modify_register(cmp4en, 0x01, 7, 7)
    instr.swd_modify_register(Cmp5en, 0x01, 3, 3)
    instr.swd_modify_register(cmp0mod, 0x02, 5, 6)

    # ====================================================================
    # COMP 测试选择: 输入01245选择对应COMP, 默认全部执行
    # ====================================================================
    print("\n请选择需要测试的COMP (输入编号如 01245, 直接回车默认全部执行):")
    comp_sel = input("COMP选择: ").strip()
    if comp_sel == "":
        comp_sel = "01245"   # 默认全部
    selected = set(comp_sel)

    # ====================================================================
    # COMP0 测试: 记录P20电压步进过程中P13引脚电平翻转时的P20电压值
    # ====================================================================
    if "0" in selected:
        print("COMP0测试")
        print("万用表表笔接入P13引脚")
        print("请将DAC模块输出线接入P20引脚,固定2.5V电压接p21引脚 按回车继续...")
        input()
        instr.swd_modify_register(0x40005008, 0x01, 16, 18)
        instr.swd_modify_register(RW_TEST0_SEL, 0x01, 15, 15)
        print("\n请选择CMP0迟滞电压:")
        print("  0: 无迟滞")
        print("  1: ±2.5mV")
        print("  2: -5mV")
        print("  3: ±5mV")
        print("  4: +5mV")
        print("  5: -10mV")
        print("  6: +10mV")
        print("  7: ±10mV")
        hys_choice = input("请输入迟滞选项 (0-7): ").strip()
        try:
            hys_val = int(hys_choice) & 0x7
        except ValueError:
            print(f"输入无效: {hys_choice}, 使用默认+5mV(选项4)")
            hys_val = 4
        instr.swd_modify_register(0x40004404, hys_val, 0, 2)
        time.sleep(0.05)

        cmp0_scan_range = list(range(2480, 2521)) + list(range(2521, 2480, -1))
        for i in cmp0_scan_range:
            instr.serial_send(f"spi_cmd_set_val mV {i}\r\n")
            time.sleep(0.1)
            vol_test = instr.dmm_measure_single_voltage()
            cmp0_reg = instr.swd_read_register(0x40004418)
            cmp0_bit = (cmp0_reg >> 0) & 0x1
            print(f"  DAC电压={i}mV, 电压表读数={vol_test:.2f}mV，cmp0比较结果：{cmp0_bit}")

    if "1" in selected:
        print("COMP1测试")
        print("万用表表笔接入P13引脚")
        print("请将DAC模块输出线接入P22引脚,固定2.5V电压接p23引脚 按回车继续...")
        input()

        instr.swd_modify_register(RW_TEST0_SEL, 0x02, 16, 18)
        instr.swd_modify_register(RW_TEST0_SEL, 0x01, 15, 15)
        print("\n请选择CMP1迟滞电压:")
        print("  0: 无迟滞")
        print("  1: ±2.5mV")
        print("  2: -5mV")
        print("  3: ±5mV")
        print("  4: +5mV")
        print("  5: -10mV")
        print("  6: +10mV")
        print("  7: ±10mV")
        hys_choice = input("请输入迟滞选项 (0-7): ").strip()
        try:
            hys_val = int(hys_choice) & 0x7
        except ValueError:
            print(f"输入无效: {hys_choice}, 使用默认+5mV(选项4)")
            hys_val = 4
        instr.swd_modify_register(0x40004404, hys_val, 7, 9)
        time.sleep(0.05)

        cmp1_scan_range = list(range(2480, 2521)) + list(range(2521, 2480, -1))
        for i in cmp1_scan_range:
            instr.serial_send(f"spi_cmd_set_val mV {i}\r\n")
            time.sleep(0.1)
            vol_test = instr.dmm_measure_single_voltage()
            cmp1_reg = instr.swd_read_register(0x40004418)
            cmp1_bit = (cmp1_reg >> 1) & 0x1
            print(f"  DAC电压={i}mV, 电压表读数={vol_test:.2f}mV，cmp1比较结果：{cmp1_bit}")

    if "2" in selected:
        print("COMP2测试")
        print("万用表表笔接入P13引脚")
        print("请将DAC模块输出线接入P24引脚,固定2.5V电压接p25引脚 按回车继续...")
        input()
        instr.swd_modify_register(RW_TEST0_SEL, 0x03, 16, 18)
        instr.swd_modify_register(RW_TEST0_SEL, 0x01, 15, 15)
        print("\n请选择CMP2迟滞电压:")
        print("  0: 无迟滞")
        print("  1: ±2.5mV")
        print("  2: -5mV")
        print("  3: ±5mV")
        print("  4: +5mV")
        print("  5: -10mV")
        print("  6: +10mV")
        print("  7: ±10mV")
        hys_choice = input("请输入迟滞选项 (0-7): ").strip()
        try:
            hys_val = int(hys_choice) & 0x7
        except ValueError:
            print(f"输入无效: {hys_choice}, 使用默认+5mV(选项4)")
            hys_val = 4
        instr.swd_modify_register(0x40004404, hys_val, 10, 12)
        time.sleep(0.05)

        cmp2_scan_range = list(range(2480, 2521)) + list(range(2521, 2480, -1))
        for i in cmp2_scan_range:
            instr.serial_send(f"spi_cmd_set_val mV {i}\r\n")
            time.sleep(0.1)
            vol_test = instr.dmm_measure_single_voltage()
            cmp2_reg = instr.swd_read_register(0x40004418)
            cmp2_bit = (cmp2_reg >> 2) & 0x1
            print(f"  DAC电压={i}mV, 电压表读数={vol_test:.2f}mV，cmp2比较结果：{cmp2_bit}")

    if "4" in selected:
        print("COMP4测试")
        print("无电压输出, 通过寄存器读取比较结果")
        print("请将DAC模块输出线接入P37引脚, 固定2.5V电压接P15引脚 按回车继续...")
        input()
        instr.swd_modify_register(RW_TEST0_SEL, 0x01, 15, 15)
        instr.swd_modify_register(cmpsel, 0x03, 0, 2)
        # 测试开始前清除比较结果位
        instr.swd_modify_register(0x40004410, 0x0, 7, 7)  # 清除cmp4out位

        for i in range(2450, 2551):
            instr.serial_send(f"spi_cmd_set_val mV {i}\r\n")
            time.sleep(0.1)
            cmp4_reg = instr.swd_read_register(0x40004410)
            cmp4_bit = (cmp4_reg >> 7) & 0x1
            print(f"  DAC电压={i}mV, 寄存器值={cmp4_bit}")
            if cmp4_bit == 1:
                COMP4H = i
                break
        for i in range(2551, 2449, -1):
            instr.serial_send(f"spi_cmd_set_val mV {i}\r\n")
            time.sleep(0.1)
            cmp4_reg = instr.swd_read_register(0x40004410)
            cmp4_bit = (cmp4_reg >> 7) & 0x1
            print(f"  DAC电压={i}mV, 寄存器值={cmp4_bit}")
            if cmp4_bit == 0:
                COMP4L = i
                break
        COMP4HYS = COMP4H - COMP4L
        print(f"COMP4HYS: {COMP4HYS}")
        comp4_passed = (COMP4HYS >= COMP4HYS_min and COMP4HYS <= COMP4HYS_max)
        save_item_result(COMP4HYS_test_item, comp4_passed, COMP4HYS)
        if comp4_passed:
            print("COMP4HYS test pass")
        else:
            print("COMP4HYS test no pass")
        """保存结果"""

    if "5" in selected:
        print("COMP5测试")
        print("无电压输出, 通过寄存器读取比较结果")
        print("请将DAC模块输出线接入P26引脚,固定2.5V电压接P36引脚 按回车继续...")
        input()
        instr.swd_modify_register(RW_TEST0_SEL, 0x01, 15, 15)
        # 测试开始前清除比较结果位
        instr.swd_modify_register(0x40004410, 0x0, 6, 6)  # 清除cmp5out位

        for i in range(2450, 2551):
            instr.serial_send(f"spi_cmd_set_val mV {i}\r\n")
            time.sleep(0.1)
            cmp5_reg = instr.swd_read_register(0x40004410)
            cmp5_bit = (cmp5_reg >> 6) & 0x1
            print(f"  DAC电压={i}mV, 寄存器值={cmp5_bit}")
            if cmp5_bit == 1:
                COMP5H = i
                break
        for i in range(2551, 2449, -1):
            instr.serial_send(f"spi_cmd_set_val mV {i}\r\n")
            time.sleep(0.1)
            cmp5_reg = instr.swd_read_register(0x40004410)
            cmp5_bit = (cmp5_reg >> 6) & 0x1
            print(f"  DAC电压={i}mV, 寄存器值={cmp5_bit}")
            if cmp5_bit == 0:
                COMP5L = i
                break
        COMP5HYS = COMP5H - COMP5L
        print(f"COMP5HYS: {COMP5HYS}")
        comp5_passed = (COMP5HYS >= COMP5HYS_min and COMP5HYS <= COMP5HYS_max)
        save_item_result(COMP5HYS_test_item, comp5_passed, COMP5HYS)
        if comp5_passed:
            print("COMP5HYS test pass")
        else:
            print("COMP5HYS test no pass")
        """保存结果"""

# 串口通信演示函数, 该函数不参与自动化测试，仅作为示例展示如何在测试中使用串口通信功能
def Serial_Comm_Demo():
    """串口通信演示函数, 展示如何在测试中使用串口"""
    print("\n🚀 串口通信演示开始...")

    # 使用全局串口实例发送数据
    result = instr.serial_send("AT+TEST\r\n")
    if result > 0:
        print(f"✅ 串口数据发送成功, 发送字节数: {result}")

        # 接收响应
        time.sleep(0.1)
        response = instr.serial_receive()
        if response:
            print(f"📨 收到串口响应: {response.decode('utf-8', 'ignore')}")
        else:
            print("⚠️  未收到串口响应")
    else:
        print("❌ 串口数据发送失败")

    print("🎉 串口通信演示结束")


def ADC_ENOB_Test():
    """
    5.13 ADC ENOB (Effective Number of Bits) 测试.
    
    原理:
        1. 信号发生器输出正弦波 -> ADC 采样 (通过 CH6, ADC6_DR)
        2. ADC 原始值 -> 转换为电压
        3. 对 ADC 采样数据进行 FFT 频谱分析
        4. 从频谱计算 SNR (信噪比，排除直流和低次谐波)
        5. ENOB = (SNR_dB - 1.76) / 6.02
    
    参考 ADC_DNL_Test() 的寄存器配置测试框架.
    """
    # ====================================================================
    # 1. 读取测试规格参数 (从 test.xlsx)
    # ====================================================================
    enob_test_item_name = "ADC_ENOB"
    row = df_test_data[df_test_data["测试项"] == enob_test_item_name]
    if not row.empty:
        try:
            enob_min_val = float(row["Min."].values[0])
            enob_typ_val = float(row["Typ."].values[0])
            enob_max_val = float(row["Max."].values[0])
            enob_unit = str(row["Unit"].values[0])
            print(f"📋 测试规格: Min={enob_min_val} {enob_unit}, Typ={enob_typ_val} {enob_unit}, Max={enob_max_val} {enob_unit}")
        except (IndexError, ValueError):
            print(f"⚠️ ADC_ENOB 测试参数读取不完全, 使用默认规格")
            enob_min_val, enob_typ_val, enob_max_val, enob_unit = 0, 10, 16, "bit"
    else:
        print(f"⚠️ 未找到测试项 {enob_test_item_name}, 使用默认规格")
        enob_min_val, enob_typ_val, enob_max_val, enob_unit = 0, 10, 16, "bit"

    # ====================================================================
    # 2. 获取寄存器地址 (与 ADC_DNL_Test 一致)
    # ====================================================================
    try:
        Pad1_ana_en = test_item.get_32bit_reg_address("Pad1_ana_en", df_register)
        Pad2_ana_en = test_item.get_32bit_reg_address("Pad2_ana_en", df_register)
        RW_ADC_CLK = test_item.get_32bit_reg_address("RW_ADC_CLK", df_register)
        RW_ADC_CTRL_EN = test_item.get_32bit_reg_address("RW_ADC_CTRL_EN", df_register)
        RW_VREF_EN = test_item.get_32bit_reg_address("RW_VREF_EN", df_register)
        RW_VREF_SEL = test_item.get_32bit_reg_address("RW_VREF_SEL", df_register)
        RW_VREF_TRIM = test_item.get_32bit_reg_address("RW_VREF_TRIM", df_register)
        RW_ADC_EN = test_item.get_32bit_reg_address("RW_ADC_EN", df_register)
        cfg_I_sampl_en = test_item.get_32bit_reg_address("cfg_I_sampl_en", df_register)
        CH6EN = test_item.get_32bit_reg_address("CH6EN", df_register)
        ADCBSY = test_item.get_32bit_reg_address("ADCBSY", df_register)
        ADC_SCYC = test_item.get_32bit_reg_address("ADC_SCYC", df_register)
        ADC6_DR = test_item.get_32bit_reg_address("ADC6_DR", df_register)
    except Exception as e:
        print(f"❌ ADC寄存器地址获取失败: {e}")
        return

    print("\n" + "=" * 60)
    print("🔬 ADC ENOB (有效位数) 测试")
    print("=" * 60)

    # ====================================================================
    # 3. 提示输入信号发生器参数并初始化
    # ====================================================================
    print("\n--- 信号发生器基本参数设置 ---")
    ch_input = input("请输入通道号 (1 或 2, 默认 1): ").strip()
    try:
        channel = int(ch_input) if ch_input else 1
        if channel not in (1, 2):
            print(f"通道号超出范围(1~2), 使用默认通道 1")
            channel = 1
    except ValueError:
        print(f"输入无效: {ch_input}, 使用默认通道 1")
        channel = 1

    print("  SIN    - 正弦波 (推荐用于ENOB测试)")
    wave_input = input("请输入波形类型 (默认 SIN): ").strip().upper()
    if not wave_input:
        wave_input = "SIN"
    valid_waveforms = ["SIN", "SINE", "SQUARE", "SQR", "RAMP", "PULSE", "NOISE", "DC"]
    if wave_input not in valid_waveforms:
        print(f"不支持的波形类型: {wave_input}, 使用默认 SIN")
        wave_input = "SIN"

    freq_input = input("请输入频率 (Hz, 默认 1000): ").strip()
    try:
        frequency = float(freq_input) if freq_input else 1000.0
        if frequency <= 0:
            frequency = 1000.0
    except ValueError:
        frequency = 1000.0

    ampl_input = input("请输入幅度 (Vpp, 默认 1.0): ").strip()
    try:
        amplitude = float(ampl_input) if ampl_input else 1.0
        if amplitude < 0:
            amplitude = 1.0
    except ValueError:
        amplitude = 1.0

    offset_input = input("请输入直流偏移 (V, 默认 0): ").strip()
    try:
        offset = float(offset_input) if offset_input else 0.0
    except ValueError:
        offset = 0.0

    print("\n" + "-" * 40)
    print("📋 信号发生器参数汇总:")
    print(f"  通道:      CH{channel}")
    print(f"  波形类型:  {wave_input}")
    print(f"  频率:      {frequency:.4f} Hz")
    print(f"  幅度:      {amplitude:.4f} Vpp")
    print(f"  直流偏移:  {offset:.4f} V")
    print("-" * 40)

    print("\n--- 正在初始化信号发生器 ---")
    try:
        if instr.sg_init():
            instr.sg_set_waveform(channel=channel, type=wave_input)
            time.sleep(0.1)
            instr.sg_set_frequency(channel=channel, freq=frequency)
            time.sleep(0.1)
            instr.sg_set_amplitude(channel=channel, amplitude=amplitude, unit="VPP")
            time.sleep(0.1)
            instr.sg_set_offset(channel=channel, offset=offset)
            time.sleep(0.1)
            instr.sg_enable_output(channel=channel, on=True)
            print(f"✅ 信号发生器已输出: {wave_input} {frequency:.4f}Hz, {amplitude:.4f}Vpp")
        else:
            print("❌ 信号发生器初始化失败")
            return
    except Exception as e:
        print(f"❌ 信号发生器配置失败: {e}")
        return

    # ====================================================================
    # 4. 配置 ADC 模块寄存器 (参照 ADC_DNL_Test)
    # ====================================================================
    print("\n--- 配置 ADC 模块 ---")
    try:
        instr.swd_modify_register(Pad1_ana_en, 0xff, 8, 15)
        instr.swd_modify_register(Pad2_ana_en, 0xff, 16, 23)
        instr.swd_modify_register(0x40001424, 0xffffffff, 0, 31)
        instr.swd_modify_register(RW_ADC_CLK, 0x1, 20, 20)
        instr.swd_modify_register(RW_VREF_EN, 0x01, 8, 8)
        instr.swd_modify_register(RW_ADC_EN, 0x01, 1, 1)
        instr.swd_modify_register(RW_ADC_CTRL_EN, 0x01, 7, 7)
        instr.swd_modify_register(cfg_I_sampl_en, 0x0, 14, 14)
        instr.swd_modify_register(CH6EN, 0x1, 6, 6)
        instr.swd_modify_register(ADC_SCYC, 0x7, 10, 13)
        time.sleep(0.05)
        print("✅ ADC 模块配置完成")
    except Exception as e:
        print(f"❌ ADC 模块配置失败: {e}")
        return

    # ====================================================================
    # 5. 采样参数: 固定 5 个周期, 每周期采样点数为 2 的幂
    #    总采样点数 N = 每周期点数 × 5, 需为 2 的幂
    #    取 per = N // 5 为每周期采样点数(向上取整到 2 的幂)
    # ====================================================================
    print("\n--- FFT 采样参数 (5 个周期) ---")
    if frequency <= 0:
        print("❌ 频率无效")
        return

    # 选取合适的每周期采样点数: 使 5 周期总点数为 2 的幂
    # 理想的每周期采样点数在 64~4096 之间
    samples_per_cycle = 0
    N = 0
    for n_pow in [6, 7, 8, 9, 10, 11, 12]:  # 每周期 2^6=64 ~ 2^12=4096
        spc_candidate = 2 ** n_pow
        ns = spc_candidate * 5
        # 检查总点数是否为 2 的幂
        if ns & (ns - 1) == 0:
            samples_per_cycle = spc_candidate
            N = ns
            break

    if N == 0:
        # 没有完美匹配, 使用 512 点/周期 × 5 = 2560, 补零到 4096
        samples_per_cycle = 512
        N = 4096  # FFT 点数, 补零到 4096

    # 每周期采样点数
    fs = frequency * samples_per_cycle  # 采样率
    ts = 1.0 / fs                        # 采样间隔
    capture_time_s = N * ts              # 总采集时长
    print(f"  信号频率:      {frequency:.2f} Hz")
    print(f"  每周期采样点数: {samples_per_cycle} 点")
    print(f"  周期数:        5")
    print(f"  总采样点数:    {N} 点 (FFT 点数)")
    print(f"  等效采样率:    {fs:.2f} Hz")
    print(f"  采样间隔:      {ts*1e6:.3f} us")
    print(f"  采集时长:      {capture_time_s * 1000:.2f} ms")
    print(f"  频率分辨率:    {fs/N:.4f} Hz")
    print(f"  实际周期数:    {N / samples_per_cycle:.1f}")
    # 提示接线
    print("\n⚠️ 请在 P20 (ADC CH6) 引脚接入信号发生器输出信号")
    print("请确认输入信号幅值不超过 VREF 范围, 按回车开始采样...")
    input()

    # ====================================================================
    # 6. 采集 N 点 ADC 数据
    # ====================================================================
    print(f"\n--- 采集 {N} 点 ADC 数据 ---")
    ADC_raw_list = []
    ADC_voltage_list = []

    # 获取 VREF
    vref_mv = global_use.get("Voffset_P", 0)
    if vref_mv is None or vref_mv == 0:
        vref_mv = 4000.0
        print(f"  ⚠️ Voffset_P 未设置, 使用默认 VREF = {vref_mv:.0f} mV")
    else:
        print(f"  ℹ️  使用 VREF = {vref_mv:.2f} mV")

    # 等间隔采样 N 点
    for i in range(N):
        instr.swd_modify_register(ADCBSY, 0x01, 6, 6)
        # 等待采样完成
        if ts > 0.000001:
            time.sleep(ts)
        else:
            time.sleep(0.000001)
        get_adc_val = instr.swd_read_register(ADC6_DR) & 0xfff
        vreal = get_adc_val / 4095.0 * vref_mv
        ADC_raw_list.append(get_adc_val)
        ADC_voltage_list.append(vreal)

    adc_min = min(ADC_raw_list)
    adc_max = max(ADC_raw_list)
    adc_avg = sum(ADC_raw_list) / N
    print(f"\n📊 ADC 数据采集完成 ({N} 点):")
    print(f"  原始值: min={adc_min}, max={adc_max}, avg={adc_avg:.1f}")
    print(f"  电压范围: {min(ADC_voltage_list):.2f} ~ {max(ADC_voltage_list):.2f} mV")

    # ====================================================================
    # 7. FFT 分析计算 ENOB
    # ====================================================================
    print("\n--- FFT 频谱分析 ---")
    try:
        import numpy as np

        # 将 ADC 电压数据转为 numpy 数组 (单位 V)
        adc_voltage_v = np.array(ADC_voltage_list) / 1000.0
        vol_peak = amplitude / 2.0  # 理论信号幅值 (V)

        # 去除直流分量
        adc_ac = adc_voltage_v - np.mean(adc_voltage_v)

        # 应用 Hanning 窗
        window = np.hanning(N)
        adc_windowed = adc_ac * window

        # 执行 FFT
        fft_vals = np.fft.fft(adc_windowed, n=N)
        fft_mag = np.abs(fft_vals[:N // 2])

        # 频率轴
        freq_axis = np.arange(N // 2) * fs / N

        # 找到基频位置 (信号频率)
        fund_bin = int(round(frequency / (fs / N)))
        if fund_bin < 1:
            fund_bin = 1
        if fund_bin >= N // 2:
            fund_bin = N // 2 - 1
        print(f"  基频 bin: {fund_bin} (频率={fund_bin * fs / N:.2f} Hz)")

        # 计算基频附近 3 个 bin 的信号功率 (包含主瓣)
        half_bin = 3  # 主瓣宽度
        sig_start = max(1, fund_bin - half_bin)
        sig_end = min(N // 2 - 1, fund_bin + half_bin)
        signal_power = np.sum(fft_mag[sig_start:sig_end] ** 2)

        # 计算谐波 bin (2~5 次谐波)
        harm_bins = set()
        for h in range(2, 6):
            h_bin = int(round(h * fund_bin))
            if h_bin < N // 2:
                for b in range(max(1, h_bin - 2), min(N // 2 - 1, h_bin + 2)):
                    harm_bins.add(b)

        # 噪声功率 = 总功率 - DC - 信号 - 谐波 (DC=bin 0)
        total_power = np.sum(fft_mag[1:] ** 2)  # 排除 DC
        harmonic_power = 0
        for b in harm_bins:
            if b < len(fft_mag):
                harmonic_power += fft_mag[b] ** 2

        noise_power = total_power - signal_power - harmonic_power
        if noise_power <= 0:
            noise_power = total_power * 0.01  # 避免除 0 或负数

        # 计算 SINAD (Signal-to-Noise-and-Distortion Ratio)
        sinad = signal_power / (noise_power + harmonic_power)

        # 计算 SNR (Signal-to-Noise Ratio, 排除谐波)
        snr = signal_power / noise_power

        # 转换为 dB
        sinad_db = 10 * np.log10(sinad)
        snr_db = 10 * np.log10(snr)

        # ENOB = (SINAD_dB - 1.76) / 6.02
        enob = (sinad_db - 1.76) / 6.02

        # 理论位数 (VREF 精度)
        theoretical_bits = 12  # 12 位 ADC

        print(f"\n📊 FFT 频谱分析结果:")
        print(f"  FFT 点数:         {N}")
        print(f"  信号功率 (dB):     {10*np.log10(signal_power):.2f}")
        print(f"  噪声功率 (dB):     {10*np.log10(noise_power):.2f}")
        print(f"  谐波功率 (dB):     {10*np.log10(harmonic_power):.2f}")
        print(f"  SNR:               {snr_db:.2f} dB")
        print(f"  SINAD:             {sinad_db:.2f} dB")
        print(f"  ENOB (有效位数):   {enob:.2f} bit")
        print(f"  理论位数:          {theoretical_bits} bit")
        print(f"  最高有效位(理论):  {theoretical_bits:.1f} bit (理想 12 位 ADC)")

        # 判断是否通过规格
        passed = (enob >= enob_min_val and enob <= enob_max_val)
        if passed:
            print(f"\n✅ ENOB 测试通过: {enob:.2f} bit (规格: {enob_min_val}~{enob_max_val} bit)")
        else:
            print(f"\n❌ ENOB 测试失败: {enob:.2f} bit (规格: {enob_min_val}~{enob_max_val} bit)")

        save_item_result(enob_test_item_name, passed, round(enob, 2))

    except ImportError:
        print("❌ 需要 numpy 支持 FFT 分析")
        save_item_result(enob_test_item_name, False, "缺少 numpy")
        instr.swd_write_register(CH6EN, 0x00 << 6)
        instr.swd_write_register(RW_ADC_EN, 0x00 << 1)
        return

    # ====================================================================
    # 8. 保存 ADC 采样数据和 FFT 结果到 XLSX
    # ====================================================================
    save_choice = input("\n是否保存 ADC 采样数据和 FFT 结果为 XLSX 文件? (y/n, 默认 n): ").strip().lower()
    if save_choice == 'y' or save_choice == 'yes':
        save_filename = input("请输入文件名 (直接回车使用默认名 ADC_ENOB_结果): ").strip()
        if not save_filename:
            save_filename = "ADC_ENOB_结果"
        if not save_filename.endswith(".xlsx"):
            save_filename += ".xlsx"
        desktop_path = os.path.join(os.path.expanduser("~"), "Desktop", save_filename)
        try:
            wb = openpyxl.Workbook()

            # Sheet1: 采样数据
            ws1 = wb.active
            ws1.title = "ADC采样数据"
            ws1.append(["采样序号", "ADC原始值", "ADC转换电压(mV)", "ADC转换电压(V)"])
            for i in range(N):
                ws1.append([i, ADC_raw_list[i], round(ADC_voltage_list[i], 3), round(ADC_voltage_list[i] / 1000.0, 6)])

            # Sheet2: FFT 分析结果
            ws2 = wb.create_sheet("FFT分析结果")
            ws2.append(["参数", "值", "单位"])
            ws2.append(["FFT点数", N, ""])
            ws2.append(["信号频率", frequency, "Hz"])
            ws2.append(["等效采样率", round(fs, 2), "Hz"])
            ws2.append(["频率分辨率", round(fs / N, 4), "Hz"])
            ws2.append(["信号功率", round(10 * np.log10(signal_power), 2), "dB"])
            ws2.append(["噪声功率", round(10 * np.log10(noise_power), 2), "dB"])
            ws2.append(["谐波功率", round(10 * np.log10(harmonic_power), 2), "dB"])
            ws2.append(["SNR", round(snr_db, 2), "dB"])
            ws2.append(["SINAD", round(sinad_db, 2), "dB"])
            ws2.append(["ENOB", round(enob, 2), "bit"])
            ws2.append(["测试结果", "PASS" if passed else "FAIL", ""])
            ws2.append([])

            # 频谱数据
            ws2.append(["频率 (Hz)", "幅度"])
            for j in range(min(len(freq_axis), 500)):  # 最多 500 行
                ws2.append([round(freq_axis[j], 2), round(fft_mag[j], 6)])

            wb.save(desktop_path)
            print(f"✅ 数据已保存为 XLSX: {desktop_path}")
        except Exception as e:
            print(f"❌ 保存文件失败: {e}")

    # ====================================================================
    # 9. 关闭 ADC 通道
    # ====================================================================
    try:
        instr.swd_write_register(CH6EN, 0x00 << 6)
        instr.swd_write_register(RW_ADC_EN, 0x00 << 1)
        print("  ADC 通道已关闭")
    except Exception as e:
        print(f"  ⚠️  关闭 ADC 通道异常: {e}")





def FT_UVLO_Test():
  
        instr.swd_modify_register(0x4000141C , 0x01, 11, 11)
        instr.swd_modify_register(0x40005008 , 0x05, 16, 18)
        instr.swd_modify_register(0x40005000 , 0x01, 15, 15)
      




def FT_ADC_Test():
        instr.swd_modify_register(0x4000141c  , 0xff, 8, 15)
        instr.swd_modify_register(0x4000141c  , 0xff, 16, 23)
        instr.swd_modify_register(0x40001424  , 0xffffffff, 0, 31)
        instr.swd_modify_register(0x40002408  , 0x01, 20, 20)
        instr.swd_modify_register(0x40005000  , 0x01, 8, 8)
        instr.swd_modify_register(0x40005000  , 0x01, 1, 1)        
        instr.swd_modify_register(0x40000c00  , 0x01, 7, 7)
        instr.swd_modify_register(0x40004c00  , 0x00, 14, 14)        
        instr.swd_modify_register(0x40000c04  , 0x01, 6, 6)
        instr.swd_modify_register(0x40000c08  , 0x07, 10, 13)
        instr.swd_modify_register(0x40000c00  , 0x01, 6, 6)
        time.sleep(0.00001)
        get_adc_val = instr.swd_read_register(0x40000c24) & 0xfff

        #计算实际输出电压
        vreal = (get_adc_val / 4095)*4000
        print(f" 转换电压: {vreal:.3f} mV ")

        instr.swd_modify_register(0x40000c04  , 0x00, 6, 6)


def FT_PGA_Test():
    """
    FT（Final Test）PGA+ADC 测试项
    功能：配置PGA（可编程增益放大器）和ADC模块，循环读取5个ADC通道的采样值，
          并转换为实际电压输出，用于验证PGA和ADC链路是否正常工作。
          采样周期可由用户输入设定，按 Ctrl+C 停止采样。
    
    测试流程：
        1. ADC 模块初始化配置（模拟引脚使能、VREF、时钟、通道使能等）
        2. PGA 增益配置（AMP0/AMP1/AMP2 增益设置、输入输出选择）
        3. 提示用户输入采样周期后，进入循环采样模式
        4. 每轮循环触发ADC采样，读取5个通道（ADC0~ADC4）数据并转换为电压值
    """
    # =========================================================================
    # 第1步：ADC 模块初始化配置
    # 配置模拟引脚使能、参考电压VREF、时钟、控制使能、通道使能、采样周期等
    # =========================================================================
    instr.swd_modify_register(0x4000141c, 0xff, 8, 15)      # Pad1 模拟功能使能（P1[7:0]）
    instr.swd_modify_register(0x4000141c, 0xff, 16, 23)     # Pad2 模拟功能使能（P2[7:0]）
    instr.swd_modify_register(0x40001424, 0xffffffff, 0, 31) # 全部引脚模拟使能
    instr.swd_modify_register(0x40002408, 0x01, 20, 20)     # RW_ADC_CLK: ADC时钟选择
    instr.swd_modify_register(0x40005000, 0x01, 8, 8)       # RW_VREF_EN: VREF电压基准使能
    instr.swd_modify_register(0x40005000, 0x01, 7, 7)       # RW_VHALF_EN: VHALF半压使能
    # instr.swd_modify_register(0x40005008, 0x3, 0, 1)      # RW_VREF_SEL（已注释，使用默认值）
    instr.swd_modify_register(0x40005000, 0x01, 1, 1)       # RW_ADC_EN: ADC模块使能
    instr.swd_modify_register(0x40000c00, 0x01, 7, 7)       # RW_ADC_CTRL_EN: ADC控制使能
    instr.swd_modify_register(0x40004c00, 0x00, 14, 14)     # cfg_I_sampl_en: 电流采样禁用
    instr.swd_modify_register(0x40000c04, 0x1f, 0, 5)       # CH6EN: 通道6使能（bit[5:0]=111111，使能通道0~5）
    instr.swd_modify_register(0x40000c08, 0x07, 10, 13)     # ADC_SCYC: ADC采样周期配置
    instr.swd_modify_register(0x40002408, 0x01, 0, 0)     # 门控都打开
    instr.swd_modify_register(0x40004c04, 0x01, 5, 5)       # 双电阻模拟采样保持器开启
    instr.swd_modify_register(0x40004c04, 0x01, 0, 1)       # 双电阻
    instr.swd_modify_register(0x40004c04, 0x01, 8, 8)     # 采样保持
    # instr.swd_modify_register(0x40004c04, 0x00, 16, 16)     # az_en

    # =========================================================================
    # 第2步：PGA（可编程增益放大器）配置
    # 配置3个PGA放大器的增益参数，以及输入输出引脚映射关系
    # =========================================================================
    instr.swd_modify_register(0x40005000, 0x07, 3, 5)       # PGA0 增益控制（AMP0 gain=7）16
    instr.swd_modify_register(0x40005000, 0x07, 11, 13)     # PGA1 增益控制（AMP1 gain=7）
    instr.swd_modify_register(0x40005008, 0x02, 8, 9)       # PGA2 stage0 增益控制（gain=2）
    instr.swd_modify_register(0x40005008, 0x02, 10, 11)     # PGA2 stage1 增益控制（gain=2）
    instr.swd_modify_register(0x40005008, 0x02, 12, 13)     # PGA2 stage2 增益控制（gain=2）

    # PGA 放大器输入输出引脚对应关系：
    #   P13(P) P14(N) --> AMP0 正向/负向输入端 --> P15 AMP0 输出
    #   P16(P) P17(N) --> AMP1 正向/负向输入端 --> P30 AMP1 输出
    #   P31(P) P32(N) --> AMP2 正向/负向输入端 --> P33 AMP2 输出

    # =========================================================================
    # 第3步：提示用户输入采样参数（采样周期和总采样点数）
    # =========================================================================
    # --- 输入采样周期 ---
    period_input = input("请输入ADC循环采样周期（秒，默认0.5s）: ").strip()
    try:
        sample_period = float(period_input) if period_input else 0.5
        if sample_period <= 0:
            print("采样周期必须大于0，使用默认值0.5s")
            sample_period = 0.5
    except ValueError:
        print(f"输入无效: {period_input}，使用默认值0.5s")
        sample_period = 0.5

    # --- 输入总采样点数（达到后自动停止，0或不输入表示无限循环，按 Ctrl+C 停止） ---
    total_input = input("请输入总采样点数（正整数，0或不输入表示无限循环，按 Ctrl+C 停止）: ").strip()
    try:
        total_samples = int(total_input) if total_input else 0
        if total_samples < 0:
            print("总采样点数不能为负数，使用无限循环模式")
            total_samples = 0
    except ValueError:
        print(f"输入无效: {total_input}，使用无限循环模式")
        total_samples = 0

    if total_samples > 0:
        print(f"ADC循环采样周期: {sample_period:.3f}s，总采样点数: {total_samples}\n")
    else:
        print(f"ADC循环采样周期: {sample_period:.3f}s，无限循环模式，按 Ctrl+C 停止采样\n")

    sample_count = 0  # 当前采样计数
    # 存储所有采样数据的列表，每个元素为一个字典，包含序号、各通道原始值和电压值
    sample_data_list = []

    try:
        # =========================================================================
        # 第4步：ADC 循环采样
        # 每轮循环：触发采样 -> 读取5个通道的12位原始值 -> 转换为电压 -> 打印输出 -> 存储数据
        # 转换公式：Vreal = (ADC_code / 4095) * 4500   (12位ADC，参考电压4500mV)
        # =========================================================================
        while True:
            # --- 检查是否达到设定采样点数 ---
            if total_samples > 0 and sample_count >= total_samples:
                print(f"\n已达到设定采样点数 {total_samples}，自动停止采样。")
                break

            # --- 触发ADC采样开始 ---
            instr.swd_modify_register(0x40000c00, 0x01, 6, 6)  # CH6EN: 启动ADC通道6采样
            time.sleep(0.00001)                                  # 等待ADC转换完成（10us）

            # --- 读取5个ADC通道的12位原始数据（掩码0xFFF仅保留低12位） ---
            get_adc0_val = instr.swd_read_register(0x40000c0c) & 0xfff   # ADC0_DR: ADC通道0数据寄存器
            get_adc1_val = instr.swd_read_register(0x40000c10) & 0xfff   # ADC1_DR: ADC通道1数据寄存器
            get_adc2_val = instr.swd_read_register(0x40000c14) & 0xfff   # ADC2_DR: ADC通道2数据寄存器
            get_adc3_val = instr.swd_read_register(0x40000c18) & 0xfff   # ADC3_DR: ADC通道3数据寄存器
            get_adc4_val = instr.swd_read_register(0x40000c1c) & 0xfff   # ADC4_DR: ADC通道4数据寄存器

            # --- ADC原始值转换为实际电压（mV） ---
            vreal0 = (get_adc0_val / 4095) * 4500
            vreal1 = (get_adc1_val / 4095) * 4500
            vreal2 = (get_adc2_val / 4095) * 4500
            vreal3 = (get_adc3_val / 4095) * 4500
            vreal4 = (get_adc4_val / 4095) * 4500

            sample_count += 1
            # --- 存储采样数据 ---
            sample_data_list.append({
                "序号": sample_count,
                "ADC0_RAW": get_adc0_val, "ADC0_mV": round(vreal0, 3),
                "ADC1_RAW": get_adc1_val, "ADC1_mV": round(vreal1, 3),
                "ADC2_RAW": get_adc2_val, "ADC2_mV": round(vreal2, 3),
                "ADC3_RAW": get_adc3_val, "ADC3_mV": round(vreal3, 3),
                "ADC4_RAW": get_adc4_val, "ADC4_mV": round(vreal4, 3),
            })

            print(f"[#{sample_count:>4}] 转换电压: {vreal0:.3f} mV, {vreal1:.3f} mV, {vreal2:.3f} mV, {vreal3:.3f} mV, {vreal4:.3f} mV")

            # --- 等待设定的采样周期后再进行下一次采样 ---
            time.sleep(sample_period)

    except KeyboardInterrupt:
        # 用户按下 Ctrl+C 停止采样
        print(f"\n\n采样已停止，共完成 {sample_count} 次采样。")

    # =========================================================================
    # 第5步：保存采样数据为 Excel 文件到桌面
    # =========================================================================
    if sample_data_list:
        save_choice = input("\n是否保存采样数据为xlsx文件到桌面? (y/n, 默认y): ").strip().lower()
        if save_choice not in ('n', 'no'):
            save_filename = input("请输入文件名（直接回车使用默认名 PGA_ADC_采样数据）: ").strip()
            if not save_filename:
                save_filename = "PGA_ADC_采样数据"
            if not save_filename.endswith(".xlsx"):
                save_filename += ".xlsx"
            desktop_path = os.path.join(os.path.expanduser("~"), "Desktop", save_filename)
            try:
                wb = openpyxl.Workbook()
                ws = wb.active
                ws.title = "PGA_ADC采样数据"
                # 写入表头
                headers = list(sample_data_list[0].keys())
                ws.append(headers)
                # 写入所有采样数据行
                for data_row in sample_data_list:
                    ws.append([data_row[h] for h in headers])
                # 在末尾写入汇总信息
                ws.append([])
                ws.append(["采样周期(s)", sample_period])
                ws.append(["总采样点数", sample_count])
                ws.append(["停止方式", "达到设定点数自动停止" if (total_samples > 0 and sample_count >= total_samples) else "用户手动停止(Ctrl+C)"])
                wb.save(desktop_path)
                print(f"✅ 采样数据已保存到桌面: {desktop_path}")
            except Exception as e:
                print(f"❌ 保存文件失败: {e}")
    else:
        print("没有采样数据，跳过保存。")


def FT_DAC_Test():
  
        # 1. 相关引脚和VREF 配置
    instr.swd_modify_register(0x4000141C, 0xff, 8, 15)    # P1模拟使能
    instr.swd_modify_register(0x40005000, 0x01, 8, 8)     # RW_VREF_EN
    instr.swd_modify_register(0x40005008, 0x3, 0, 1)      # RW_VREF_SEL
    instr.swd_modify_register(0x40005000, 0x01, 7, 7)     # RW_VHALF_EN
    instr.swd_modify_register(0x4000500C, 0x20, 0, 5)     # RW_VREF_TRIM
    instr.swd_modify_register(0x4000500C, 0x7, 24, 26)    # RW_VBG_TRIM

    instr.swd_modify_register(0x40005000, 0x01, 2, 2)     # RW_DAC_EN
    instr.swd_modify_register(0x40005004, 0x0, 24, 24)    # RW_DAC_MOD
    instr.swd_modify_register(0x4000141C, 0xff, 24, 31)   # P3模拟使能
    instr.swd_modify_register(0x40004408, 0x1, 7, 7)      # COMP 4使能
    instr.swd_modify_register(0x40004410, 0x1, 3, 3)      # COMP 5使能

    instr.swd_modify_register(0x40005008, 0x0, 19, 22)
    instr.swd_modify_register(0x40005004 , 0xFF, 0, 8)
    instr.swd_modify_register(0x40005004 , 0xFF, 16, 21)
    time.sleep(0.02)



def FT_CMP_Test():
  
        # 1. 相关引脚和VREF 配置
    instr.swd_modify_register(0x4000141C, 0xffffff, 8, 31)    # P1模拟使能
    instr.swd_modify_register(0x40004408, 0x01, 0, 0)
    instr.swd_modify_register(0x40004408, 0x01, 7, 7)
    instr.swd_modify_register(0x40004410, 0x01, 3, 3)
    instr.swd_modify_register(0x40004408, 0x02, 5, 6)
    instr.swd_modify_register(0x40005000, 0x01, 15, 15)
    instr.swd_modify_register(0x40005000, 0x01, 6, 6)
    instr.swd_modify_register(0x40002408, 0x01, 31, 31)

    instr.swd_modify_register(0x40004404, 0x04, 0, 2)
    instr.swd_modify_register(0x40004404, 0x04, 7, 9)
    instr.swd_modify_register(0x40004404, 0x04, 10, 12)

    instr.swd_modify_register(0x40005008, 0x01, 16, 18)
    
# //COMP0输出结果到P13，Pad_c0ns_en引脚映射
# P21 接固定电压 2.5V
# P20 接可编程步进电压源（范围 2.49V ~ 2.51V，步进 1mV）

#     instr.swd_modify_register(0x40005008, 0x02, 16, 18)
#     time.sleep(0.1)

# #  //COMP1输出结果到P13，
# # P23 接固定电压 2.5V，
# # P22 接可编程步进电压源（范围 2.49V ~ 2.51V，步进 1mV）

#     instr.swd_modify_register(0x40005008, 0x03, 16, 18)

#  //COMP2输出结果到P13，
# P25接固定电压 2.5V，
# P24 接可编程步进电压源（范围 2.49V ~ 2.51V，步进 1mV）

    time.sleep(0.001)
    instr.swd_read_register(0x40004418)
    # time.sleep(0.1)
    # instr.swd_read_register(0x40004410)