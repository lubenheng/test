import os
import sys
import openpyxl

# 修调项名称 → (offset, bit_start, bit_len) 映射
TRIM_FLASH_MAP = {
    "VBG":     (0xFC, 24, 3),
    "OSC_24M": (0xFC, 0,  6),
    "OSC_33K": (0xFC, 8,  7),
    "VREF":    (0xFC, 16, 6),
    "VPTAT":   (0xFC, 28, 2),
}


def read_trim_values(xlsx_path):
    """从 trim_best_value.xlsx 读取 VBG / OSC_24M / OSC_33K 的最佳 TRIM 值"""
    if not os.path.exists(xlsx_path):
        print(f"❌ 文件不存在: {xlsx_path}")
        return None

    wb = openpyxl.load_workbook(xlsx_path)
    ws = wb.active
    name_to_trim = {}

    for row in ws.iter_rows(min_row=2, values_only=True):
        test_name = row[0]
        best_trim = row[3]
        if test_name in TRIM_FLASH_MAP and best_trim is not None:
            name_to_trim[test_name] = int(best_trim)

    wb.close()
    return name_to_trim


def main():
    script_dir = os.path.dirname(os.path.abspath(__file__))

    # 定位 Save_Results/ocd2810/trim_best_value.xlsx
    xlsx_path = os.path.normpath(os.path.join(script_dir, "..", "..", "..", "Save_Results", "ocd2810", "trim_best_value.xlsx"))

    print("=" * 50)
    print("  从 trim_best_value.xlsx 读取 TRIM 值并烧录")
    print("=" * 50)
    print(f"  Excel: {xlsx_path}")

    name_to_trim = read_trim_values(xlsx_path)
    if name_to_trim is None:
        sys.exit(1)

    if not name_to_trim:
        print("⚠️  Excel 中未找到有效的 TRIM 值（需要 VBG / OSC_24M / OSC_33K / VREF / VPTAT）")
        sys.exit(1)

    # 构建烧录任务（VBG 第一个写入）
    tasks = []
    for key in ("OSC_24M", "OSC_33K", "VREF", "VBG", "VPTAT"):
        if key not in name_to_trim:
            print(f"  ⚠️  {key} 未在 Excel 中找到，跳过")
            continue
        offset, bit_start, bit_len = TRIM_FLASH_MAP[key]
        tasks.append({"offset": offset, "bit_start": bit_start, "bit_len": bit_len, "value": name_to_trim[key]})
        print(f"  ▶ {key} trim={name_to_trim[key]} -> 0x{offset:X} bit{bit_start}~{bit_start+bit_len-1}")

    if not tasks:
        print("⚠️  没有有效的 TRIM 值，退出")
        sys.exit(1)

    print(f"\n  共 {len(tasks)} 个修调项，准备烧录至 Info 区 Flash ...\n")

    # 导入 jflash_auto 并执行烧录
    if script_dir not in sys.path:
        sys.path.insert(0, script_dir)
    import jflash_auto

    success = jflash_auto.run_jflash_program(tasks)
    if success:
        print("\n✅ TRIM 值全部烧录完成")
    else:
        print("\n❌ 烧录失败")
        sys.exit(1)


if __name__ == "__main__":
    main()
