import sys
import os
import re
import importlib
import inspect
import time
from pathlib import Path

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
# 通用工具项：把 txt 转成 Excel（测试结果记录 / MTP 配置表都能识别，任意芯片可用）
EXPORT_KEY = "e"
TEST_ITEMS[EXPORT_KEY] = ("txt → Excel（测试结果 / MTP 配置表）", None)
TEST_ITEMS["q"] = ("退出程序", None)


def _menu_key(k):
    """菜单排序键: 数字按键数值排序在前, 字母键按字母排序在后"""
    return (0, int(k)) if k.isdigit() else (1, k)


# 选择项分隔符：空格、逗号、中文逗号、顿号、分号、制表符
_CHOICE_SEP = re.compile(r"[\s,，、;；]+")
# 全角数字 → 半角（避免中文输入法下 "１ ３ ５" 被当成无效选项）
_FULLWIDTH_DIGITS = str.maketrans("０１２３４５６７８９", "0123456789")


def _parse_choices(raw):
    """把一行输入拆成选择项列表。

    支持用空格 / 逗号 / 顿号 / 分号分隔（如 `1 3 5`、`1,3,5`、`1、3、5`），
    并把全角数字转成半角，避免中文输入法造成的“无效选项”。
    """
    return [p for p in _CHOICE_SEP.split(raw.translate(_FULLWIDTH_DIGITS)) if p]


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
    print("提示: 输入数字选择测试项（多选用空格/逗号分隔，如 1 3 5），输入q退出")


def _mtp_txt_to_template(path):
    """MTP 配置表 txt → 烧录模板 Excel（调用 other_test/mtp/build_mtp_template.py）。"""
    mtp_dir = str(Path(__file__).resolve().parent / "other_test" / "mtp")
    if mtp_dir not in sys.path:
        sys.path.append(mtp_dir)
    try:
        import build_mtp_template as builder
    except Exception as e:
        print(f"   ❌ 无法加载 build_mtp_template.py：{type(e).__name__}: {e}")
        return False
    ok, msg = builder.txt_to_template(path)
    print(("   ✅ 已生成烧录模板：" if ok else "   ❌ ") + msg)
    return ok


def _export_results_txt():
    """菜单 [e]：选择 txt → Excel。

    · 测试结果 txt  → 转成「明细 + 汇总」工作簿；
    · mtp_config_table.txt → 调用 build_mtp_template.py 生成烧录模板。
    """
    from instrument import result_log
    print("\n🚀 选择 txt 转换为 Excel")
    path = result_log.current_path()
    if path:
        print(f"   本次会话记录：{path}（默认打开该目录）")

    chosen = result_log.pick_txt(default_dir=result_log.SAVE_ROOT,
                                 title="选择 txt：测试结果记录 或 mtp_config_table.txt")
    if not chosen:
        print("   ⏭️ 未选择文件，已取消")
        return False

    mtp_dir = Path(__file__).resolve().parent / "other_test" / "mtp"
    if str(mtp_dir) not in sys.path:
        sys.path.append(str(mtp_dir))
    try:
        import build_mtp_template as builder
        is_mtp = builder.is_mtp_config_txt(chosen)
    except Exception:
        is_mtp = "mtp_config_table" in Path(chosen).name.lower()

    print(f"   文件：{chosen}")
    if is_mtp:
        print("   识别为 MTP 配置表 → 调用 build_mtp_template.py 生成烧录模板")
        return _mtp_txt_to_template(chosen)

    print("   识别为测试结果记录 → 转换为明细/汇总工作簿")
    ok, msg = result_log.txt_to_excel(chosen)
    if ok:
        print(f"   ✅ 已生成 Excel：{msg}")
    else:
        print(f"   ❌ {msg}")
    return ok


def _run_one(key):
    """执行单个测试项，返回状态字符串。

    返回 'ok'      - 执行完成且未返回 False
         'fail'    - 抛异常，或测试函数返回 False
         'aborted' - 该项被 Ctrl+C / EOF 中断（由调用方决定是否继续后续项）
    """
    if key == EXPORT_KEY:
        return "ok" if _export_results_txt() else "fail"

    name, func = TEST_ITEMS[key]
    print(f"\n🚀 开始执行: {name}")
    try:
        result = func()
    except KeyboardInterrupt:
        print(f"\n⏹️  {name} 已被手动中断")
        return "aborted"
    except Exception as e:
        print(f"❌ {name} 执行出错: {type(e).__name__}: {e}")
        return "fail"

    if result is False:
        print(f"⚠️  {name} 执行完成，但结果未通过（详见上面的判定行）")
        return "fail"
    print(f"🎉 {name} 执行完成！")
    return "ok"


def _print_summary(results):
    """批量执行结束后打印汇总。"""
    if not results:
        return
    ok = [k for k, v in results.items() if v == "ok"]
    fail = [k for k, v in results.items() if v == "fail"]
    aborted = [k for k, v in results.items() if v == "aborted"]
    print("\n" + "-" * 50)
    print(f"本次共执行 {len(results)} 项：通过/完成 {len(ok)}，未通过/出错 {len(fail)}"
          + (f"，中断 {len(aborted)}" if aborted else ""))
    if fail:
        for k in fail:
            print(f"   ❌ [{k}] {TEST_ITEMS[k][0]}")
    if aborted:
        for k in aborted:
            print(f"   ⏹️ [{k}] {TEST_ITEMS[k][0]}")
    print("-" * 50)


def execute_selected_test(choice, results=None):
    """执行选中的测试项。

    results: dict/None - 传入时把本次执行的每项状态累加进去（供 _print_summary 汇总）
    """
    results = results if results is not None else {}
    if choice == "0" and _RUN_ALL_BUILTIN:
        # 执行全部测试（跳过 TRIM 修调等交互式子菜单）
        print("\n🚀 开始执行全部测试项...")
        for key in sorted(TEST_ITEMS.keys(), key=_menu_key):
            if key in ("0", "q", EXPORT_KEY):
                continue
            name, _ = TEST_ITEMS[key]
            if "TRIM" in name.upper() or "修调" in name:
                print(f"\n⏭️ 跳过 {name}（需手动选择进入）")
                continue
            status = _run_one(key)
            results[key] = status
            if status == "aborted":
                print("⏹️  已停止后续测试项")
                break
        print("\n🎉 全部测试项执行结束！")
    elif choice in TEST_ITEMS and choice != "q":
        status = _run_one(choice)
        results[choice] = status
    else:
        print("⚠️  无效的选择！")
    return results


def main():
    """主程序逻辑"""
    while True:
        show_menu()
        # 获取用户输入（空格/逗号分隔多选，如: 1 3 5）
        raw = input("\n请输入选择(数字/q): ").strip().lower()

        choices = _parse_choices(raw)
        if not choices:
            print("⚠️  没有输入任何选择！")
            input("\n按回车键返回菜单...")
            continue

        # q 退出：与其他选项同时输入时也直接退出，避免“输入了 q 却还在跑”
        if "q" in choices:
            if len(choices) > 1:
                print(f"⚠️  输入中包含 q，忽略其它选项 {[c for c in choices if c != 'q']}，直接退出")
            print("\n👋 退出程序！")
            sys.exit(0)

        # "0" 全部执行：出现即视为执行全部（同时输入其它项时提示已忽略）
        if "0" in choices and _RUN_ALL_BUILTIN:
            others = [c for c in choices if c != "0"]
            if others:
                print(f"⚠️  选择 0 表示执行全部测试，已忽略其它选项 {others}")
            results = execute_selected_test("0")
            _print_summary(results)
        else:
            # 过滤无效选项并去重保序
            valid = list(dict.fromkeys(c for c in choices if c in TEST_ITEMS))
            invalid = [c for c in choices if c not in TEST_ITEMS]
            if invalid:
                print(f"⚠️  忽略无效选项: {', '.join(invalid)}")

            if not valid:
                print("⚠️  没有有效的测试项！")
            else:
                results = {}
                for choice in valid:
                    execute_selected_test(choice, results)
                    if results.get(choice) == "aborted":
                        print("⏹️  已停止后续测试项")
                        break
                if len(valid) > 1:
                    _print_summary(results)

        # 提示本次测试结果记录文件（有记录时）
        try:
            from instrument import result_log
            path, count = result_log.session_summary()
            if path and count:
                print(f"\n📝 测试结果已记录 {count} 条：{path}")
                print("   （菜单输入 e 可把该 txt 转成 Excel）")
        except Exception:
            pass

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


def _has_cli_flag(flag):
    """命令行是否带某个开关（如 --skip-default-test）。"""
    for a in sys.argv[1:]:
        if a == flag or a.startswith(flag + "="):
            return True
    return False


def run_startup_test():
    """初始化完成后自动执行芯片声明的默认测试项（默认「进入测试模式使能」）。

    芯片模块通过 chip.py 的 AUTO_RUN_TEST 指定 tests.py 中的函数名；
    设为 None 则不执行。命令行加 --skip-default-test 可临时跳过。
    执行失败只提示、不退出，操作员仍可正常进入菜单手动操作。
    """
    name = getattr(chip_cfg, "AUTO_RUN_TEST", None)
    if not name:
        return
    if _has_cli_flag("--skip-default-test"):
        print(f"\n⏭️  已按命令行参数跳过默认测试项 {name}")
        return

    func = getattr(item, name, None)
    if not callable(func):
        print(f"\n⚠️  chip.py 声明 AUTO_RUN_TEST = {name!r}，"
              f"但 tests.py 中没有同名函数，已跳过")
        return

    label = next((v[0] for k, v in TEST_ITEMS.items() if v[1] is func), name)
    print(f"\n🚀 初始化完成，自动执行默认测试项：{label}")
    try:
        result = func()
    except KeyboardInterrupt:
        print(f"\n⏹️  {label} 已被手动中断，进入菜单")
        return
    except Exception as e:
        print(f"❌ {label} 执行出错: {type(e).__name__}: {e}")
        print("   （可忽略：进入菜单后仍可手动选择该项重新执行）")
        return

    if result is False:
        print(f"⚠️  {label} 执行完成，但结果未通过")
    else:
        print(f"✅ {label} 已完成")


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
        run_startup_test()   # 芯片声明的默认测试项（OCD2361：进入测试模式使能）
        main()
    except KeyboardInterrupt:
        instr.dmm_release()
        instr.osc_close()
        instr.smu_close()  # 仅 OCD2361 初始化过源表, 未初始化时为无害空操作
        instr.psu_close()  # 仅关闭本会话由程序打开的电源通道, 未初始化时为无害空操作
        print("\n\n👋 用户中断操作, 程序退出！")
    except Exception as e:
        instr.dmm_release()
        instr.osc_close()
        instr.smu_close()
        instr.psu_close()
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
