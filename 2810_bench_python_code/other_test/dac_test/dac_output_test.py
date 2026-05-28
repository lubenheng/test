import sys
import os

# 添加项目根目录到 Python 路径（相对导入方式，支持路径转移）
current_dir = os.path.dirname(os.path.abspath(__file__))
project_root = os.path.dirname(os.path.dirname(current_dir))
if project_root not in sys.path:
    sys.path.insert(0, project_root)

# 导入 Keysight 34465A 函数
try:
    from instrument.multimeter.Keysight_34465A import (
        init_keysight_34465a,
        read_keysight_voltage,
        read_single_voltage,
        read_single_current,
        release_keysight_resource,
        DMM34465A
    )
    print("成功导入 Keysight 34465A 模块")
except ImportError as e:
    print(f"导入 Keysight 34465A 模块失败: {e}")
    # 可以根据需要设置标记或退出
    KEYSIGHT_AVAILABLE = False
else:
    KEYSIGHT_AVAILABLE = True

# 导入串口功能
try:
    from instrument.platform.serial_comm import (
        serial_init,
        serial_send,
        serial_receive,
        serial_close,
        SerialComm
    )
    print("成功导入串口通信模块")
except ImportError as e:
    print(f"导入串口通信模块失败: {e}")
    SERIAL_AVAILABLE = False
else:
    SERIAL_AVAILABLE = True

# 示例使用函数
def test_imports():
    """测试导入是否成功"""
    if KEYSIGHT_AVAILABLE:
        print("Keysight 34465A 模块可用")
        # 这里可以添加测试代码
    else:
        print("Keysight 34465A 模块不可用")

    if SERIAL_AVAILABLE:
        print("串口通信模块可用")
        # 这里可以添加测试代码
    else:
        print("串口通信模块不可用")

# DAC测试示例函数
def dac_output_test_example():
    """
    DAC输出测试
    使用串口控制DAC和万用表测量
    """
    print("=== DAC输出测试示例 ===")

    # 1. 初始化串口（假设DAC通过串口控制）
    if SERIAL_AVAILABLE:
        print("1. 初始化串口...")
        # 根据实际串口号修改，例如 "COM3"
        ser = serial_init(port="COM5", baudrate=115200, timeout=2)
        if ser:
            print("   串口初始化成功")
        else:
            print("   串口初始化失败，请检查连接")
            return
    else:
        print("串口模块不可用，跳过串口测试")
        return

    # 2. 初始化万用表
    if KEYSIGHT_AVAILABLE:
        print("2. 初始化Keysight 34465A万用表...")
        if init_keysight_34465a():
            print("   万用表初始化成功")
        else:
            print("   万用表初始化失败，请检查连接")
            serial_close()
            return
    else:
        print("万用表模块不可用，跳过电压测量")
        # 继续串口测试但不测量

    try:
        # 3. 通过串口设置DAC输出电压（示例命令）
        print("3. 设置DAC输出电压...")
        # 假设DAC命令格式：SET_VOLTAGE <电压值>
        dac_voltage = 2.5  # 设置2.5V
        command = f"SET_VOLTAGE {dac_voltage}\r\n"
        bytes_sent = serial_send(command)
        print(f"   发送命令: {command.strip()}，发送字节数: {bytes_sent}")

        # 等待DAC稳定
        import time
        time.sleep(0.5)

        # 4. 读取DAC响应（如果有）
        response = serial_receive(1024)
        if response:
            print(f"   DAC响应: {response.decode('utf-8', errors='ignore').strip()}")

        # 5. 使用万用表测量实际输出电压
        if KEYSIGHT_AVAILABLE:
            print("4. 使用万用表测量输出电压...")
            # 读取单次DC电压，10V量程
            measured_voltage_mv = read_single_voltage(
                voltage_type="DC",
                range_val="10V",
                resolution=0.00001
            )

            if measured_voltage_mv is not None:
                measured_voltage_v = measured_voltage_mv / 1000.0  # 转换回V
                print(f"   测量电压: {measured_voltage_v:.3f} V")
                print(f"   设置电压: {dac_voltage:.3f} V")
                print(f"   误差: {abs(measured_voltage_v - dac_voltage):.3f} V")

                # 简单判断是否在允许误差范围内
                allowed_error = 0.01  # 10mV误差
                if abs(measured_voltage_v - dac_voltage) <= allowed_error:
                    print("   ✓ DAC输出测试通过")
                else:
                    print("   ✗ DAC输出测试失败，误差过大")
            else:
                print("   电压测量失败")

        # 6. 测试多个电压点
        if KEYSIGHT_AVAILABLE:
            print("5. 测试多个电压点...")
            test_voltages = [1.0, 2.0, 3.0, 4.0, 5.0]
            results = []

            for voltage in test_voltages:
                # 设置DAC电压
                command = f"SET_VOLTAGE {voltage}\r\n"
                serial_send(command)
                time.sleep(0.3)  # 等待稳定

                # 测量电压
                measured_mv = read_single_voltage(voltage_type="DC", range_val="10V")
                if measured_mv is not None:
                    measured_v = measured_mv / 1000.0
                    error = abs(measured_v - voltage)
                    results.append((voltage, measured_v, error))
                    print(f"   设置: {voltage:.3f}V, 测量: {measured_v:.3f}V, 误差: {error:.3f}V")
                else:
                    print(f"   设置: {voltage:.3f}V, 测量失败")

            # 输出测试总结
            if results:
                print("\n测试总结:")
                for set_v, meas_v, err in results:
                    print(f"   {set_v:.3f}V -> {meas_v:.3f}V (误差: {err:.3f}V)")

    finally:
        # 7. 清理资源
        print("6. 清理资源...")
        if KEYSIGHT_AVAILABLE:
            release_keysight_resource()
            print("   万用表资源已释放")

        if SERIAL_AVAILABLE:
            serial_close()
            print("   串口已关闭")

        print("=== 测试完成 ===")

def generate_values(start=0, end=65535, num_points=32):
    """
    生成从start到end的num_points个等间隔值（包含两端）

    Args:
        start: 起始值
        end: 结束值
        num_points: 点数

    Returns:
        list: 生成的值列表
    """
    if num_points <= 1:
        return [start]

    step = (end - start) / (num_points - 1)
    values = [start + i * step for i in range(num_points)]
    # 四舍五入为整数
    values = [round(v) for v in values]
    # 确保第一个和最后一个值精确
    values[0] = start
    values[-1] = end
    return values

def spi_voltage_test():
    """
    SPI电压测试
    发送spi_cmd_set_val Ori {value}命令，读取万用表电压值
    测试32个从0到65535的等间隔值
    """
    import time
    import datetime
    import os

    # 基准电压值（将在测量后设置）
    vref_value = None
    vref_source = None  # 基准电压来源：万用表测量/手动输入

    print("=== SPI电压测试（增强版） ===")
    print("命令格式: spi_cmd_set_val Ori {value}")
    print("参数范围: 0-65535，测试32个等间隔点")
    print("")
    print("测试前准备:")
    print("1. 确保设备已正确连接")
    print("2. 确保串口连接正常")
    print("3. 确保万用表连接正常")
    print("")

    # 1. 提示烧录代码
    print("1. 烧录代码提示")
    print("   请确保设备已烧录正确的固件/代码")
    input("   按回车键继续...")

    # 2. 测量基准电压
    print("\n2. 基准电压测量")
    print("   请使用万用表测量基准电压（参考电压）")
    print("   将测量表笔连接到基准电压测试点")

    # 初始化万用表用于基准电压测量
    if KEYSIGHT_AVAILABLE:
        print("   正在初始化万用表...")
        if init_keysight_34465a():
            print("   万用表初始化成功")
            print("   请确保万用表已正确连接到基准电压测试点")
            input()
            print("   正在测量基准电压...")
            vref_measurements = []
            for i in range(3):  # 测量3次取平均
                vref = read_single_voltage(voltage_type="DC", range_val="10V")
                if vref is not None:
                    vref_measurements.append(vref)  # 单位mV
                    print(f"   第{i+1}次测量: {vref_measurements[-1]:.3f} mV")
                    time.sleep(0.5)

            if vref_measurements:
                vref_value = sum(vref_measurements) / len(vref_measurements)
                vref_source = "万用表测量"
                print(f"   基准电压平均值: {vref_value:.3f} mV")
                print("   使用万用表测量的基准电压值")
            else:
                print("   基准电压测量失败，需要手动输入")
                while True:
                    vref_input = input("   请输入基准电压值(mV): ").strip()
                    try:
                        vref_value = float(vref_input)
                        vref_source = "手动输入"
                        print(f"   使用手动输入的基准电压: {vref_value:.3f} mV")
                        break
                    except ValueError:
                        print("   输入无效，请重新输入")
        else:
            print("   万用表初始化失败，需要手动输入基准电压")
            while True:
                vref_input = input("   请输入基准电压值(mV): ").strip()
                try:
                    vref_value = float(vref_input)
                    vref_source = "手动输入"
                    print(f"   使用手动输入的基准电压: {vref_value:.3f} mV")
                    break
                except ValueError:
                    print("   输入无效，请重新输入")
    else:
        print("   万用表模块不可用，需要手动输入基准电压")
        while True:
            vref_input = input("   请输入基准电压值(mV): ").strip()
            try:
                vref_value = float(vref_input)
                vref_source = "手动输入"
                print(f"   使用手动输入的基准电压: {vref_value:.3f} mV")
                break
            except ValueError:
                print("   输入无效，请重新输入")

    # 释放万用表资源（为DAC测试重新初始化做准备）
    if KEYSIGHT_AVAILABLE:
        print("   释放万用表资源...")
        release_keysight_resource()
        print("   万用表资源已释放")

    # 3. 提示切换表笔到DAC输出
    print("\n3. 表笔切换提示")
    print("   请将万用表表笔从基准电压测试点切换到DAC输出测试点")
    input("   按回车键继续...")

    # 默认终止符
    terminator = '\r\n'  # Windows默认

    # 4. 初始化串口
    if SERIAL_AVAILABLE:
        print("4. 初始化串口...")

        # 显示可用串口
        from instrument.platform.serial_comm import SerialComm
        available_ports = SerialComm.list_serial_ports()
        print(f"   可用串口: {available_ports}")

        # 询问用户串口端口
        port_input = input("   请输入串口号 (如 COM3) 或按回车自动检测: ").strip()
        port = None

        if port_input:
            # 检查输入是否像COM端口（不区分大小写）
            if port_input.upper().startswith('COM'):
                port = port_input
                print(f"   使用指定端口: {port}")
            else:
                print(f"   输入 '{port_input}' 不是有效的COM端口格式，将使用自动检测")
        else:
            print("   使用自动检测")

        # 自动检测串口，波特率115200
        ser = serial_init(port=port, baudrate=115200, timeout=2)
        if ser:
            print(f"   串口初始化成功: {ser.port}")

            # 显示串口配置
            try:
                print(f"   波特率: {ser.ser.baudrate}")
                print(f"   数据位: {ser.ser.bytesize}")
                print(f"   停止位: {ser.ser.stopbits}")
                print(f"   奇偶校验: {ser.ser.parity}")
                print(f"   流控制: {ser.ser.xonxoff}")
            except:
                print("   无法获取串口详细配置")

            # 选择命令终止符
            print("   选择命令终止符...")
            print("   1. \\r\\n (Windows默认)")
            print("   2. \\n (Linux/Unix)")
            print("   3. \\r (Mac)")
            print("   4. 无终止符")
            term_choice = input("   请选择终止符类型 (1-4, 默认1): ").strip()

            if term_choice == '2':
                terminator = '\n'
                print("   使用终止符: \\n")
            elif term_choice == '3':
                terminator = '\r'
                print("   使用终止符: \\r")
            elif term_choice == '4':
                terminator = ''
                print("   使用终止符: 无")
            else:
                terminator = '\r\n'
                print("   使用终止符: \\r\\n")

            # 询问是否先测试串口通信
            test_com = input("   是否先测试串口通信? (y/n, 推荐y): ").strip().lower()
            if test_com == 'y':
                print("   串口通信测试...")
                test_command = f"TEST{terminator}"  # 使用选择的终止符
                print(f"   发送测试命令: {test_command.strip()}")

                # 显示原始字节（十六进制）用于调试
                cmd_bytes = test_command.encode('utf-8')
                hex_str = ' '.join([f'{b:02X}' for b in cmd_bytes])
                print(f"   原始字节(HEX): {hex_str}")

                sent = serial_send(test_command)
                print(f"   发送字节数: {sent}")

                # 尝试接收响应
                import time
                time.sleep(0.1)
                response = serial_receive(1024)
                if response:
                    response_hex = ' '.join([f'{b:02X}' for b in response])
                    response_str = response.decode('utf-8', errors='ignore').strip()
                    print(f"   收到响应: {response_str}")
                    print(f"   响应字节(HEX): {response_hex}")
                else:
                    print("   未收到响应")

                # 测试实际命令
                test_dac = input("   是否测试DAC命令? (y/n): ").strip().lower()
                if test_dac == 'y':
                    test_val = input("   输入测试参数值 (0-65535): ").strip()
                    try:
                        test_val = int(test_val)
                        if 0 <= test_val <= 65535:
                            test_cmd = f"spi_cmd_set_val Ori {test_val}{terminator}"
                            print(f"   发送DAC命令: {test_cmd.strip()}")

                            # 显示原始字节（十六进制）用于调试
                            cmd_bytes = test_cmd.encode('utf-8')
                            hex_str = ' '.join([f'{b:02X}' for b in cmd_bytes])
                            print(f"   原始字节(HEX): {hex_str}")

                            sent = serial_send(test_cmd)
                            print(f"   发送字节数: {sent}")
                            time.sleep(0.1)
                            response = serial_receive(1024)
                            if response:
                                response_hex = ' '.join([f'{b:02X}' for b in response])
                                response_str = response.decode('utf-8', errors='ignore').strip()
                                print(f"   收到响应: {response_str}")
                                print(f"   响应字节(HEX): {response_hex}")
                            else:
                                print("   无响应")
                        else:
                            print("   参数值超出范围")
                    except ValueError:
                        print("   无效的参数值")

                continue_test = input("   继续执行SPI电压测试? (y/n): ").strip().lower()
                if continue_test != 'y':
                    print("   测试中止")
                    serial_close()
                    return
        else:
            print("   串口初始化失败，请检查连接")
            return
    else:
        print("串口模块不可用，无法进行测试")
        return

    # 5. 初始化万用表（用于DAC测试）
    if KEYSIGHT_AVAILABLE:
        print("5. 初始化Keysight 34465A万用表...")
        if init_keysight_34465a():
            print("   万用表初始化成功")
        else:
            print("   万用表初始化失败，请检查连接")
            serial_close()
            return
    else:
        print("万用表模块不可用，无法进行测试")
        serial_close()
        return

    try:
        # 6. 生成32个测试值
        print("6. 生成测试参数...")
        values = generate_values(0, 65535, 32)
        print(f"   生成{len(values)}个测试值: {values}")

        # 7. 准备存储结果
        results = []

        # 8. 循环测试
        print("8. 开始循环测试...")
        for i, val in enumerate(values):
            print(f"   测试 {i+1}/{len(values)}: 参数值={val}")

            # 计算理论电压值
            if vref_value is not None:
                theoretical_voltage = (val / 65535.0) * vref_value  # vref_value单位为mV
                print(f"   理论电压: {theoretical_voltage:.3f} mV")
            else:
                theoretical_voltage = None
                print("   警告: 基准电压未设置，无法计算理论值")

            # 发送SPI命令
            cmd = f"spi_cmd_set_val Ori {val}{terminator}"  # 使用选择的终止符
            print(f"   发送命令: {cmd.strip()}")

            # 显示原始字节（十六进制）用于调试
            cmd_bytes = cmd.encode('utf-8')
            hex_str = ' '.join([f'{b:02X}' for b in cmd_bytes])
            print(f"   原始字节(HEX): {hex_str}")

            sent = serial_send(cmd)
            print(f"   发送字节数: {sent}")

            # 尝试接收响应
            time.sleep(0.05)  # 短暂等待响应
            response = serial_receive(1024)
            if response:
                response_hex = ' '.join([f'{b:02X}' for b in response])
                response_str = response.decode('utf-8', errors='ignore').strip()
                print(f"   收到响应: {response_str}")
                print(f"   响应字节(HEX): {response_hex}")

            if sent <= 0:
                print(f"   发送失败，跳过该点")
                results.append({
                    'param': val,
                    'theoretical_mv': theoretical_voltage,
                    'actual_mv': None,
                    'error_mv': None,
                    'error_percent': None,
                    'status': '发送失败'
                })
                continue

            # 延迟0.5秒
            time.sleep(0.5)

            # 测量DAC芯片输出电压
            print(f"   第{i+1}/{len(values)}次循环：请将万用表连接到DAC芯片输出测试点")
            input("   按回车键开始测量DAC输出电压...")
            dac_voltage = read_single_voltage(voltage_type="DC", range_val="10V")
            if dac_voltage is None:
                print(f"   DAC电压读取失败")
                dac_voltage = None
            else:
                print(f"   DAC输出电压: {dac_voltage:.3f} mV")

            # 测量运放输出电压
            print(f"   第{i+1}/{len(values)}次循环：请将万用表连接到运放输出测试点")
            input("   按回车键开始测量运放输出电压...")
            amp_voltage = read_single_voltage(voltage_type="DC", range_val="10V")
            if amp_voltage is None:
                print(f"   运放电压读取失败")
                amp_voltage = None
            else:
                print(f"   运放输出电压: {amp_voltage:.3f} mV")

            # 确定状态和误差计算
            if dac_voltage is None or amp_voltage is None:
                # 任一测量失败
                status = '读取失败'
                error_mv = None
                error_percent = None
            else:
                # 两个测量都成功
                status = '成功'
                # 计算误差（基于理论电压和运放输出电压？或者DAC输出电压？）
                # 根据原有逻辑，误差是基于理论电压和运放输出电压（actual_mv）
                if theoretical_voltage is not None and theoretical_voltage != 0:
                    error_mv = amp_voltage - theoretical_voltage
                    error_percent = (error_mv / theoretical_voltage) * 100
                    print(f"   误差: {error_mv:.3f} mV ({error_percent:.3f}%)")
                else:
                    error_mv = None
                    error_percent = None
                    status = '成功(无理论值)'

            # 存储结果
            results.append({
                'param': val,
                'theoretical_mv': theoretical_voltage,
                'dac_voltage_mv': dac_voltage,
                'actual_mv': amp_voltage,  # 运放输出电压，保持向后兼容
                'error_mv': error_mv,
                'error_percent': error_percent,
                'status': status
            })

        # 9. 分析结果
        successful_results = [r for r in results if r['actual_mv'] is not None]
        failed_results = [r for r in results if r['actual_mv'] is None]

        # 初始化统计变量（确保在所有代码路径中都有定义）
        actual_voltages = []
        errors = []
        avg_error = None
        max_error = None

        # 10. 打印详细结果表格
        print("\n" + "="*80)
        print("SPI电压测试结果")
        print("="*80)
        print(f"测试点数: {len(results)}")
        print(f"成功: {len(successful_results)} 点")
        print(f"失败: {len(failed_results)} 点")

        if vref_value is not None:
            print(f"基准电压: {vref_value:.3f} mV")
            if vref_source is not None:
                print(f"基准电压来源: {vref_source}")

        if successful_results:
            actual_voltages = [r['actual_mv'] for r in successful_results if r['actual_mv'] is not None]
            if actual_voltages:
                print(f"实际电压范围: {min(actual_voltages):.3f} ~ {max(actual_voltages):.3f} mV")
                print(f"实际平均电压: {sum(actual_voltages)/len(actual_voltages):.3f} mV")

            # 计算平均误差
            errors = [r['error_mv'] for r in successful_results if r['error_mv'] is not None]
            if errors:
                avg_error = sum(errors) / len(errors)
                max_error = max(errors, key=abs)
                print(f"平均误差: {avg_error:.3f} mV")
                print(f"最大误差: {max_error:.3f} mV")

        print("\n详细数据:")
        print("-"*80)
        print(f"{'序号':<4} {'参数值':<8} {'理论值(mV)':<15} {'DAC电压(mV)':<15} {'运放电压(mV)':<15} {'误差(mV)':<15} {'状态':<12}")
        print("-"*80)

        for i, r in enumerate(results):
            param = r['param']
            theoretical = r['theoretical_mv']
            dac_voltage = r['dac_voltage_mv']
            amp_voltage = r['actual_mv']  # 运放电压
            error = r['error_mv']
            status = r['status']

            theoretical_str = f"{theoretical:.3f}" if theoretical is not None else "N/A"
            dac_str = f"{dac_voltage:.3f}" if dac_voltage is not None else "N/A"
            amp_str = f"{amp_voltage:.3f}" if amp_voltage is not None else "N/A"
            error_str = f"{error:.3f}" if error is not None else "N/A"

            print(f"{i+1:<4} {param:<8} {theoretical_str:<15} {dac_str:<15} {amp_str:<15} {error_str:<15} {status:<12}")

        print("-"*80)

        # 11. 自动保存结果到Excel文件（dac_test文件夹）
        print("\n11. 保存结果到Excel文件...")
        current_dir = os.path.dirname(os.path.abspath(__file__))
        timestamp = datetime.datetime.now().strftime('%Y%m%d_%H%M%S')
        excel_filename = f"spi_voltage_test_{timestamp}.xlsx"
        csv_filename = f"spi_voltage_test_{timestamp}.csv"
        excel_filepath = os.path.join(current_dir, excel_filename)
        csv_filepath = os.path.join(current_dir, csv_filename)

        try:
            import pandas as pd

            # 创建DataFrame
            df_data = []
            for i, r in enumerate(results):
                df_data.append({
                    '序号': i + 1,
                    '参数值': r['param'],
                    '基准电压(mV)': f"{vref_value:.3f}" if vref_value is not None else '',
                    '理论电压(mV)': f"{r['theoretical_mv']:.3f}" if r['theoretical_mv'] is not None else '',
                    'DAC电压(mV)': f"{r['dac_voltage_mv']:.3f}" if r['dac_voltage_mv'] is not None else '',
                    '运放电压(mV)': f"{r['actual_mv']:.3f}" if r['actual_mv'] is not None else '',
                    '误差(mV)': f"{r['error_mv']:.3f}" if r['error_mv'] is not None else '',
                    '误差百分比(%)': f"{r['error_percent']:.3f}" if r['error_percent'] is not None else '',
                    '状态': r['status']
                })

            df = pd.DataFrame(df_data)

            # 创建汇总信息
            summary_data = {
                '项目': [
                    '测试时间',
                    '命令格式',
                    '参数范围',
                    '测试点数',
                    '成功点数',
                    '失败点数',
                    '基准电压(mV)',
                    '基准电压来源',
                    '理论电压计算公式',
                    '实际电压范围(mV)',
                    '实际平均电压(mV)',
                    '平均误差(mV)',
                    '最大误差(mV)'
                ],
                '值': [
                    datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
                    'spi_cmd_set_val Ori {value}',
                    '0-65535',
                    len(results),
                    len(successful_results),
                    len(failed_results),
                    f"{vref_value:.3f}" if vref_value is not None else "N/A",
                    vref_source if vref_source is not None else "N/A",
                    '理论电压(mV) = (参数值 / 65535) × 基准电压(mV)',
                    f"{min(actual_voltages):.3f} ~ {max(actual_voltages):.3f}" if successful_results and actual_voltages else "N/A",
                    f"{sum(actual_voltages)/len(actual_voltages):.3f}" if successful_results and actual_voltages else "N/A",
                    f"{avg_error:.3f}" if successful_results and errors else "N/A",
                    f"{max_error:.3f}" if successful_results and errors else "N/A"
                ]
            }
            df_summary = pd.DataFrame(summary_data)

            # 使用ExcelWriter创建Excel文件，包含多个工作表
            with pd.ExcelWriter(excel_filepath, engine='openpyxl') as writer:
                # 写入详细数据
                df.to_excel(writer, sheet_name='详细数据', index=False)

                # 写入汇总信息
                df_summary.to_excel(writer, sheet_name='测试汇总', index=False)

                # 获取worksheet进行格式设置
                worksheet_detail = writer.sheets['详细数据']
                worksheet_summary = writer.sheets['测试汇总']

                # 调整列宽（简单方法）
                for column in worksheet_detail.columns:
                    max_length = 0
                    column_letter = column[0].column_letter
                    for cell in column:
                        try:
                            if len(str(cell.value)) > max_length:
                                max_length = len(str(cell.value))
                        except:
                            pass
                    adjusted_width = min(max_length + 2, 30)
                    worksheet_detail.column_dimensions[column_letter].width = adjusted_width

                for column in worksheet_summary.columns:
                    max_length = 0
                    column_letter = column[0].column_letter
                    for cell in column:
                        try:
                            if len(str(cell.value)) > max_length:
                                max_length = len(str(cell.value))
                        except:
                            pass
                    adjusted_width = min(max_length + 2, 40)
                    worksheet_summary.column_dimensions[column_letter].width = adjusted_width

            print(f"   Excel文件已保存: {excel_filepath}")

            # 同时保存CSV文件作为备份
            df.to_csv(csv_filepath, index=False, encoding='utf-8-sig')
            print(f"   CSV备份文件已保存: {csv_filepath}")

            # 显示文件信息
            import os
            print(f"   Excel文件大小: {os.path.getsize(excel_filepath)} 字节")
            print(f"   CSV文件大小: {os.path.getsize(csv_filepath)} 字节")

        except ImportError as e:
            print(f"   警告: 无法导入pandas/openpyxl，使用CSV格式保存")
            print(f"   错误: {e}")
            # 回退到CSV格式
            try:
                with open(csv_filepath, 'w', encoding='utf-8-sig') as f:
                    # 写入文件头
                    f.write("序号,参数值,基准电压(mV),理论电压(mV),DAC电压(mV),运放电压(mV),误差(mV),误差百分比(%),状态\n")
                    for i, r in enumerate(results):
                        theoretical = r['theoretical_mv']
                        dac_voltage = r['dac_voltage_mv']
                        amp_voltage = r['actual_mv']
                        error = r['error_mv']
                        error_percent = r['error_percent']
                        vref_str = f"{vref_value:.3f}" if vref_value is not None else ''
                        # 格式化数值为三位小数
                        theoretical_str = f"{theoretical:.3f}" if theoretical is not None else ''
                        dac_str = f"{dac_voltage:.3f}" if dac_voltage is not None else ''
                        amp_str = f"{amp_voltage:.3f}" if amp_voltage is not None else ''
                        error_str = f"{error:.3f}" if error is not None else ''
                        error_percent_str = f"{error_percent:.3f}" if error_percent is not None else ''
                        f.write(f"{i+1},{r['param']},{vref_str},{theoretical_str},{dac_str},{amp_str},{error_str},{error_percent_str},{r['status']}\n")
                print(f"   CSV文件已保存: {csv_filepath}")
            except Exception as e2:
                print(f"   CSV保存也失败: {e2}")

        except Exception as e:
            print(f"   保存文件失败: {e}")
            # 尝试简单的CSV保存
            try:
                with open(csv_filepath, 'w', encoding='utf-8-sig') as f:
                    f.write("序号,参数值,基准电压(mV),理论电压(mV),DAC电压(mV),运放电压(mV),误差(mV),误差百分比(%),状态\n")
                    for i, r in enumerate(results):
                        theoretical = r['theoretical_mv']
                        dac_voltage = r['dac_voltage_mv']
                        amp_voltage = r['actual_mv']
                        error = r['error_mv']
                        error_percent = r['error_percent']
                        vref_str = f"{vref_value:.3f}" if vref_value is not None else ''
                        # 格式化数值为三位小数
                        theoretical_str = f"{theoretical:.3f}" if theoretical is not None else ''
                        dac_str = f"{dac_voltage:.3f}" if dac_voltage is not None else ''
                        amp_str = f"{amp_voltage:.3f}" if amp_voltage is not None else ''
                        error_str = f"{error:.3f}" if error is not None else ''
                        error_percent_str = f"{error_percent:.3f}" if error_percent is not None else ''
                        f.write(f"{i+1},{r['param']},{vref_str},{theoretical_str},{dac_str},{amp_str},{error_str},{error_percent_str},{r['status']}\n")
                print(f"   CSV文件已保存（备选）: {csv_filepath}")
            except Exception as e2:
                print(f"   备选保存也失败: {e2}")

    finally:
        # 12. 清理资源
        print("12. 清理资源...")
        if KEYSIGHT_AVAILABLE:
            release_keysight_resource()
            print("   万用表资源已释放")

        if SERIAL_AVAILABLE:
            serial_close()
            print("   串口已关闭")

        print("=== SPI电压测试完成 ===")

# 主函数
def main():
    """主函数：可以选择运行测试"""
    print("DAC输出测试模块")
    print("1. 测试导入")
    print("2. 运行DAC测试示例")
    print("3. 运行SPI电压测试")
    print("4. 退出")

    try:
        choice = input("请选择 (1-4): ").strip()
        if choice == "1":
            test_imports()
        elif choice == "2":
            dac_output_test_example()
        elif choice == "3":
            spi_voltage_test()
        elif choice == "4":
            print("退出")
        else:
            print("无效选择")
    except KeyboardInterrupt:
        print("\n用户中断")
    except Exception as e:
        print(f"发生错误: {e}")

if __name__ == "__main__":
    # 直接运行测试示例，或调用main()进行交互
    # test_imports()  # 简单测试导入
    # dac_output_test_example()  # 运行完整示例
    main()  # 交互式选择
