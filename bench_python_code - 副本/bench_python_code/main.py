import sys
import os
import importlib
import inspect
import time

if os.name == 'nt':
    os.system('chcp 65001 > nul')
    sys.stdout.reconfigure(encoding='utf-8')
    sys.stderr.reconfigure(encoding='utf-8')

print("程序正在启动，请稍候...")

from instrument import instrument_set as instr

# =====================================================================
# 芯片选择 —— 切换芯片时只需改下面这一行
#   "ocd2810" -> chips/ocd2810/   当前芯片 OCD2810
#   "OCD5010" -> chips/OCD5010/   另一芯片 OCD5010
#   "chip_b"  -> chips/chip_b/    另一芯片（新建目录即可）
# =====================================================================
CHIP = "OCD2361"
#CHIP = "ocd2810"
#CHIP = "chip_b"

# 根据芯片动态加载对应的模块
print(f"正在加载芯片模块 ({CHIP})...")
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
# 芯片若自定义了 "0" 则使用芯片的定义, 否则 "0" 为执行全部测试
_RUN_ALL_BUILTIN = "0" not in TEST_ITEMS
if _RUN_ALL_BUILTIN:
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
        print(f"  [{key}] → {name}")
    print("=" * 50)
    print("提示: 输入数字选择测试项（多选用空格分隔，如 1 3 5），输入q退出")


def execute_selected_test(choice):
    """执行选中的测试项"""
    if choice == "0" and _RUN_ALL_BUILTIN:
        # 执行全部测试（跳过 TRIM 修调等交互式子菜单）
        print("\n🚀 开始执行全部测试项...")
        for key in sorted(TEST_ITEMS.keys(), key=_menu_key):
            if key in ["0", "q"]:
                continue
            name, func = TEST_ITEMS[key]
            if "TRIM" in name.upper() or "修调" in name:
                print(f"\n⏭️ 跳过 {name}（需手动选择进入）")
                continue
            print(f"\n--- 开始执行: {name} ---")
            try:
                func()
                print(f"--- {name} 执行完成 ---")
            except Exception as e:
                print(f"❌ {name} 执行出错: {e}")
        print("\n🎉 全部测试项执行结束！")
    elif choice in TEST_ITEMS and choice != "q":
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
            print("\n👋 退出程序！")
            sys.exit(0)

        # 按空格拆分为多个选项，过滤空串
        choices = [c for c in raw.split() if c]

        # "0" 全部执行：出现即视为执行全部
        if "0" in choices and _RUN_ALL_BUILTIN:
            execute_selected_test("0")
        else:
            # 过滤无效选项并去重保序
            valid = list(dict.fromkeys(c for c in choices if c in TEST_ITEMS and c != "q"))
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


def _get_cli_port():
    """从命令行读取 --port（IIC 串口），未指定返回 None。

    支持 `python main.py --port COM10` 和 `--port=COM10` 两种写法。
    """
    argv = sys.argv[1:]
    for i, a in enumerate(argv):
        if a == "--port" and i + 1 < len(argv):
            return argv[i + 1]
        if a.startswith("--port="):
            return a.split("=", 1)[1]
    return None


def run_chip_init():
    """按芯片模块声明的初始化方式初始化硬件。

    芯片模块可提供：
      INIT_LABEL     启动提示文字（默认按 SWD 流程）
      POST_INIT_CMD  初始化后发送的板级命令（默认 cmd_gpio_set_595d_test reset，None 表示不发送）
      cleanup()      退出时释放芯片侧资源（如 IIC 串口）
    """
    label = getattr(chip_cfg, "INIT_LABEL", "串口 / SWD / 万用表 / 示波器")
    print(f"正在初始化硬件连接（{label}），请稍候...")

    kwargs = {}
    if "port" in inspect.signature(chip_cfg.init_all).parameters:
        kwargs["port"] = _get_cli_port()
    chip_cfg.init_all(instr, **kwargs)
    time.sleep(0.1)  # 等待设备稳定

    post_cmd = getattr(chip_cfg, "POST_INIT_CMD", "cmd_gpio_set_595d_test reset\r\n")
    if post_cmd:
        instr.serial_send_and_receive(post_cmd)
        time.sleep(0.5)  # 等待芯片稳定


if __name__ == "__main__":
    try:
        run_chip_init()
        main()
    except KeyboardInterrupt:
        instr.dmm_release()
        instr.osc_close()
        instr.smu_close()  # 仅 OCD2361 初始化过源表, 未初始化时为无害空操作
        print("\n\n👋 用户中断操作, 程序退出！")
    except Exception as e:
        instr.dmm_release()
        instr.osc_close()
        instr.smu_close()
        print(f"\n❌ 程序异常: {e}")
        input("按回车键退出...")
    finally:
        # 释放芯片侧资源（如 IIC 串口），再确保普通串口关闭
        cleanup = getattr(chip_cfg, "cleanup", None)
        if cleanup:
            try:
                cleanup()
            except Exception:
                pass
        instr.serial_close()
