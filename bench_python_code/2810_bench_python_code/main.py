import sys
import os
from instrument import instrument_set as instr
# 导入原有测试脚本的所有测试函数
import actions.Test_Item as item
# 导入 TRIM 修调测试模块
from other_test.trim_test import trim

# 构建测试项映射(数字 → (测试项名称, 测试函数))
TEST_ITEMS = {
    "1": ("OSC33K 测试", item.OSC33K_Test),
    "2": ("OSC24M 测试", item.OSC24M_Test),
    "3": ("VBG 测试", item.VBG_Test),
    "4": ("VPTAT 测试", item.VPTAT_Test),
    "5": ("VREF 测试", item.VREF_Test),
    "6": ("DAC0/DAC1 DNL 测试", item.DAC0_DAC1_DNL_Test),
    "7": ("ADC DNL 测试", item.ADC_DNL_Test),
    "8": ("Voffset/VS_ERROR 测试", item.Voffset_VS_ERROR_Test),
    "9": ("PGA+ADC/SH+PGA 测试", item.PGA_ADC_SH_PGA_Test),
    "10": ("COMP 测试", item.COMP_Test),
    "11": ("TRIM 修调测试", trim.main),
    "12": ("ADC_ENOB 测试", item.ADC_ENOB_Test),

    "14": ("FT_ADC 测试", item.FT_ADC_Test),
    "15": ("FT_UVLO_Test 测试", item.FT_UVLO_Test),
    "16": ("FT_PGA_Test 测试", item.FT_PGA_Test),
    "17": ("FT_DAC_Test 测试", item.FT_DAC_Test),
    "18": ("FT_CMP_Test 测试", item.FT_CMP_Test),
    
    "0": ("执行全部测试", None),  # 全部执行单独处理
    "q": ("退出程序", None)
}

def clear_screen():
    """清屏(兼容Windows/Linux/Mac)"""
    os.system('cls' if os.name == 'nt' else 'clear')

def show_menu():
    """显示交互式菜单"""
    clear_screen()
    print("=" * 50)
    print("          芯片自动化测试项选择工具")
    print("=" * 50)
    for key in sorted(TEST_ITEMS.keys()):
        name, _ = TEST_ITEMS[key]
        print(f"  [{key}] → {name}")
    print("=" * 50)
    print("提示: 输入数字选择测试项, 输入q退出")

def execute_selected_test(choice):
    """执行选中的测试项"""
    if choice == "0":
        # 执行全部测试（跳过 TRIM 修调等交互式子菜单）
        print("\n🚀 开始执行全部测试项...")
        for key in sorted(TEST_ITEMS.keys()):
            if key in ["0", "q"]:
                continue
            name, func = TEST_ITEMS[key]
            if func == trim.main:
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
        # 获取用户输入(去除空格、转小写)
        choice = input("\n请输入选择(数字/q): ").strip().lower()
        
        if choice == "q":
            print("\n👋 退出程序, 再见！")
            sys.exit(0)
        
        # 执行选中的测试
        execute_selected_test(choice)
        
        # 执行完成后等待用户确认, 再返回菜单
        input("\n按回车键返回菜单...")

if __name__ == "__main__":
    # 确保中文显示正常(Windows控制台)
    if os.name == 'nt':
        os.system('chcp 65001 > nul')
    # 查找所有 CH340 串口
    ch340_ports = instr.serial_find_ports_by_keyword("CH340")

    serial_ok = False

    if len(ch340_ports) == 0:
        print("❌ 未检测到 CH340 串口")
    elif len(ch340_ports) == 1:
        port = ch340_ports[0][0]
        print(f"检测到 CH340 串口: {port}")
        if instr.serial_init(port, baudrate=115200):
            print(f"✅ 串口初始化成功, 端口: {port}")
            serial_ok = True
        else:
            print(f"❌ {port} 初始化失败")
    else:
        print("\n检测到多个 CH340 串口:")
        print("-" * 40)
        for i, (dev, desc) in enumerate(ch340_ports, 1):
            print(f"  [{i}] {dev} — {desc}")
        print("-" * 40)

        choice = input("\n请选择串口编号: ").strip()
        if choice.isdigit():
            idx = int(choice)
            if 1 <= idx <= len(ch340_ports):
                port = ch340_ports[idx - 1][0]
                print(f"\n正在初始化 {port}...")
                if instr.serial_init(port, baudrate=115200):
                    print(f"✅ 串口初始化成功, 端口: {port}")
                    serial_ok = True
                else:
                    print(f"❌ {port} 初始化失败")
            else:
                print("❌ 无效编号，程序退出。")
                sys.exit(1)
        else:
            print("❌ 无效输入，程序退出。")
            sys.exit(1)

    if not serial_ok:
        resp = input("\n串口初始化失败，是否忽略并继续？(y/N): ").strip().lower()
        if resp != "y":
            print("程序退出。")
            sys.exit(1)
        print("⚠️  已忽略串口初始化失败，后续串口操作可能不可用。")

    try:
        instr.swd_init()
        instr.dmm_init()
        instr.osc_init()
        instr.sg_init()
        main()
    except KeyboardInterrupt:
        instr.dmm_release()
        instr.osc_close()
        instr.sg_close()
        print("\n\n👋 用户中断操作, 程序退出！")
    except Exception as e:
        instr.dmm_release()
        instr.osc_close()
        instr.sg_close()
        print(f"\n❌ 程序异常: {e}")
        input("按回车键退出...")
    finally:
        # 确保串口关闭
        instr.serial_close()