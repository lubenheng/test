import sys
import os
import importlib
from instrument import instrument_set as instr
import time

# =====================================================================
# 芯片选择 —— 切换芯片时只需改下面这一行
#   "ocd2810" -> chips/ocd2810/   当前芯片 OCD2810
#   "OCD5010" -> chips/OCD5010/   另一芯片 OCD5010
#   "chip_b"  -> chips/chip_b/    另一芯片（新建目录即可）
# =====================================================================
CHIP = "OCD5010"
#CHIP = "ocd2810"
#CHIP = "chip_b"

# 需要手动执行（含接线确认/交互输入/必须先修调）的测试项名称关键词 —— "执行全部"时自动跳过
#   例: "TRIM"/"修调"   -> 修调项（需手动选择进入）
#       "ADC DNL"       -> ADC DNL 测试（需手动给 GPIO28 接电压、输入采样次数）
MANUAL_KEYWORDS = ("TRIM", "修调", "ADC DNL", "采样验证")

# 根据芯片动态加载对应的模块
try:
    chip_cfg = importlib.import_module(f"chips.{CHIP}.chip")
    item     = importlib.import_module(f"chips.{CHIP}.tests")
except ModuleNotFoundError as e:
    print(f"❌ 芯片模块加载失败 ({CHIP}): {e}")
    print("请检查 chips/ 目录下是否存在对应的芯片模块")
    sys.exit(1)

# 从芯片模块获取测试项映射, 并补充通用控制项
# 各芯片在 tests.py 中定义 TEST_ITEMS = { "1": ("名称", func), ... }
TEST_ITEMS = dict(getattr(item, "TEST_ITEMS", {}))
TEST_ITEMS["0"] = ("执行全部测试", None)   # 全部执行单独处理
TEST_ITEMS["q"] = ("退出程序", None)


def _menu_key(k):
    """菜单排序键: 数字按键数值排序在前, 字母键按字母排序在后"""
    return (0, int(k)) if k.isdigit() else (1, k)


def clear_screen():
    """清屏(兼容Windows/Linux/Mac)"""
    os.system('cls' if os.name == 'nt' else 'clear')


def show_menu():
    """显示交互式菜单"""
    clear_screen()
    print("=" * 50)
    chip_label = getattr(chip_cfg, "CHIP_LABEL", CHIP)
    print(f"          芯片自动化测试项选择工具  [{chip_label}]")
    print("=" * 50)
    for key in sorted(TEST_ITEMS.keys(), key=_menu_key):
        name, _ = TEST_ITEMS[key]
        mark = "  (需手动选择)" if any(kw in name.upper() for kw in MANUAL_KEYWORDS) else ""
        print(f"  [{key}] → {name}{mark}")
    print("=" * 50)
    print("提示: 输入数字选择测试项（多选用空格分隔，如 1 3 5），输入q退出")


def execute_selected_test(choice):
    """执行选中的测试项"""
    if choice == "0":
        # 执行全部测试（跳过 TRIM 修调 / ADC DNL 采样验证等需手动接线与交互的项）
        print("\n🚀 开始执行全部测试项...")
        for key in sorted(TEST_ITEMS.keys(), key=_menu_key):
            if key in ["0", "q"]:
                continue
            name, func = TEST_ITEMS[key]
            if any(kw in name.upper() for kw in MANUAL_KEYWORDS):
                print(f"\n⏭️ 跳过 {name}（需手动选择进入）")
                continue
            print(f"\n--- 开始执行: {name} ---")
            try:
                func()
                print(f"--- {name} 执行完成 ---")
            except Exception as e:
                print(f"❌ {name} 执行出错: {e}")
        print("\n🎉 全部测试项执行结束！")
    elif choice in TEST_ITEMS and choice not in ["0", "q"]:
        # 执行单个测试
        name, func = TEST_ITEMS[choice]
        print(f"\n🚀 开始执行: {name}")
        try:
            func()
            print(f"🎉 {name} 执行完成！")
        except Exception as e:
            print(f"❌ {name} 执行出错: {e}")
    else:
        print("⚠️  无效的选择！")


def main():
    """主程序逻辑"""
    while True:
        show_menu()
        # 获取用户输入（空格分隔多选，如: 1 3 5）
        raw = input("\n请输入选择(数字/q): ").strip().lower()

        if raw == "q":
            print("\n👋 退出程序, 再见！")
            sys.exit(0)

        # 按空格拆分为多个选项，过滤空串
        choices = [c for c in raw.split() if c]

        # "0" 全部执行：出现即视为执行全部
        if "0" in choices:
            execute_selected_test("0")
        else:
            # 过滤无效选项并去重保序
            valid = list(dict.fromkeys(c for c in choices if c in TEST_ITEMS and c not in ["0", "q"]))
            invalid = [c for c in choices if c not in TEST_ITEMS]
            if invalid:
                print(f"⚠️  忽略无效选项: {', '.join(invalid)}")

            if not valid:
                print("⚠️  没有有效的测试项！")
            else:
                for choice in valid:
                    execute_selected_test(choice)

        # 执行完成后等待用户确认, 再返回菜单
        input("\n按回车键返回菜单...")


if __name__ == "__main__":
    # 确保中文显示正常(Windows控制台)
    if os.name == 'nt':
        os.system('chcp 65001 > nul')

    try:
        # 芯片全套初始化（串口 + SWD + 解密 + 万用表 + 示波器）
        chip_cfg.init_all(instr)
        time.sleep(0.1)  # 等待设备稳定
        instr.serial_send_and_receive("cmd_gpio_set_595d_test reset\r\n")
        time.sleep(0.5)  # 等待芯片稳定
        main()
    except KeyboardInterrupt:
        instr.dmm_release()
        instr.osc_close()
        print("\n\n👋 用户中断操作, 程序退出！")
    except Exception as e:
        instr.dmm_release()
        instr.osc_close()
        print(f"\n❌ 程序异常: {e}")
        input("按回车键退出...")
    finally:
        # 确保串口关闭
        instr.serial_close()
