import sys
import os
from instrument import instrument_set as instr
# 导入原有测试脚本的所有测试函数
import actions.Test_Item as item

# 构建测试项映射(数字 → (测试项名称, 测试函数))
TEST_ITEMS = {
    "1": ("OS 测试", item.OS_Test),
    "2": ("UVLO_RST 测试", item.UVLO_RSTN_Test),#检测P01拉高电压
    "3": ("OSC33K 测试", item.OSC33K_Test),
    "4": ("OSC24M 测试", item.OSC24M_Test),
    "5": ("INORMAL 测试", item.I_NORMAL_SLEEP_Test),
    "6": ("VBG 测试", item.VBG_Test),
    "7": ("VPTAT 测试", item.VPTAT_Test),
    "8": ("VREF 测试", item.VREF_Test),
    "9": ("DAC0/DAC1 DNL 测试", item.DAC0_DAC1_DNL_Test),
    "10": ("ADC DNL 测试", item.ADC_DNL_Test),
    "11": ("Voffset/VS_ERROR 测试", item.Voffset_VS_ERROR_Test),
    "12": ("PGA+ADC/SH+PGA 测试", item.PGA_ADC_SH_PGA_Test),
    "13": ("COMP 测试", item.COMP_Test),
    # "14": ("BEMF 电阻测试", item.BEMF_Test),
    # "15": ("IO 测试", item.IO_Test),
    "16": ("程序测试", item.Program_Test),
    "17": ("芯片MTP读写测试", item.Chip_MTP_Write_Read_Test),
    "18": ("串口通信演示", item.Serial_Comm_Demo),
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
        # 执行全部测试
        print("\n🚀 开始执行全部测试项...")
        for key in sorted(TEST_ITEMS.keys()):
            if key in ["0", "q"]:
                continue
            name, func = TEST_ITEMS[key]
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

    if len(ch340_ports) == 0:
        print("❌ 未检测到 CH340 串口，程序退出。")
        sys.exit(1)
    elif len(ch340_ports) == 1:
        port = ch340_ports[0][0]
        print(f"检测到 CH340 串口: {port}")
        if not instr.serial_init(port, baudrate=115200):
            print(f"❌ {port} 初始化失败，程序退出。")
            sys.exit(1)
        print(f"✅ 串口初始化成功, 端口: {port}")
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
                if not instr.serial_init(port, baudrate=115200):
                    print(f"❌ {port} 初始化失败，程序退出。")
                    sys.exit(1)
                print(f"✅ 串口初始化成功, 端口: {port}")
            else:
                print("❌ 无效编号，程序退出。")
                sys.exit(1)
        else:
            print("❌ 无效输入，程序退出。")
            sys.exit(1)

    try:
        instr.swd_init()
        instr.dmm_init()
        instr.osc_init()
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