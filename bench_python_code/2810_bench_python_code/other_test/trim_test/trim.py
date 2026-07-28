import os
import sys
import openpyxl

from instrument import instrument_set as instr


# 修调项名称 → (offset, bit_start, bit_len) 映射
TRIM_FLASH_MAP = {
    "VBG":     (0xFC, 24, 3),
    "OSC_24M": (0xFC, 0,  6),
    "OSC_33K": (0xFC, 8,  7),
    "VPTAT": (0xFC, 28,  2),
    "VREF": (0xFC, 16,  5),
}


def program_trim_values():
    """从 trim_best_value.xlsx 读取最佳 TRIM 值，调用 jflash_auto 写入 Info 区 Flash"""
    # 定位 Save_Results/trim_best_value.xlsx
    trim_dir = os.path.dirname(os.path.abspath(__file__))
    xlsx_path = os.path.join(trim_dir, "..", "..", "Save_Results", "trim_best_value.xlsx")
    xlsx_path = os.path.normpath(xlsx_path)

    if not os.path.exists(xlsx_path):
        print(f"⚠️  trim_best_value.xlsx 不存在: {xlsx_path}")
        return False

    # 读取 Excel，收集已通过的测试项对应的 trim 值
    wb = openpyxl.load_workbook(xlsx_path)
    ws = wb.active
    tasks = []
    name_to_trim = {}

    for row in ws.iter_rows(min_row=2, values_only=True):
        test_name = row[0]   # 测试项名称（如 "VBG"）
        best_trim = row[3]   # 最佳 TRIM 值
        test_result = row[4]  # E 列：测试结果（PASS/FAIL）
        if test_name in TRIM_FLASH_MAP and best_trim is not None and test_result == "PASS":
            name_to_trim[test_name] = int(best_trim)

    wb.close()

    # VBG 要第一个写入
    for key in ("VBG", "OSC_24M", "OSC_33K", "VPTAT", "VREF"):
        if key not in name_to_trim:
            print(f"  ⚠️  {key} 的 trim 值未在 Excel 中找到，跳过")
            continue
        offset, bit_start, bit_len = TRIM_FLASH_MAP[key]
        tasks.append({"offset": offset, "bit_start": bit_start, "bit_len": bit_len, "value": name_to_trim[key]})

    if not tasks:
        print("⚠️  没有有效的 TRIM 值，跳过 Flash 写入")
        return False

    if trim_dir not in sys.path:
        sys.path.insert(0, trim_dir)
    import jflash_auto

    # 关闭 SWD 释放 JLink probe，避免与 JLink.exe 冲突
    instr.swd_close()

    result = jflash_auto.run_jflash_program(tasks)

    # 重新初始化 SWD
    instr.swd_init()
    if not result:
        print("  ❌ TRIM 值写入 Flash 失败")
    return result


def main():
    """供 main.py 菜单调用的入口，执行修调制写入"""
    success = program_trim_values()
    if success:
        print("\n✅ TRIM 修调值写入完成！")
    else:
        print("\n❌ TRIM 修调值写入失败")
    input("\n按回车键返回菜单...")


if __name__ == "__main__":
    success = program_trim_values()
    sys.exit(0 if success else 1)


