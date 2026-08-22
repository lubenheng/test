import math
import pyvisa as visa
import time

# 全局变量: 保存示波器连接实例, 避免重复初始化
osc_inst = None
rm = None

def init_oscilloscope(probe_attenuation=10, vertical_scale=5.0, horizontal_scale=1e-6):
    """
    示波器初始化函数(无返回值)
    功能: 自动连接示波器、复位、打开所有通道(CH1-CH4)、完成基础测量配置
    :param probe_attenuation: 探头衰减比(1X=1, 10X=10)
    :param vertical_scale: 垂直量程(V/格, 默认5.0V)
    :param horizontal_scale: 水平时基(s/格, 默认1μs对应100KHz信号)
    """
    global osc_inst, rm
    try:
        # 1. 初始化资源管理器
        rm = visa.ResourceManager()
        osc_address = None
        # 筛选泰克示波器USB地址
        for res in rm.list_resources():
            if "USB0::0x0699::" in res:
                osc_address = res
                break
        if not osc_address:
            print("❌ 初始化失败: 未找到TBS2204B示波器!")
            return False

        # 2. 连接示波器
        osc_inst = rm.open_resource(osc_address)
        osc_inst.timeout = 8000
        osc_inst.write("*RST")  # 仪器复位
        time.sleep(1)
        # 验证连接
        idn = osc_inst.query("*IDN?").strip()
        print(f"✅ 示波器初始化成功: {idn}")

        # 3. 打开所有通道(CH1-CH4)
        for ch in ["CH1", "CH2", "CH3", "CH4"]:
            osc_inst.write(f"{ch}:STATE ON")                # 打开通道
            osc_inst.write(f"{ch}:PROBE:GAIN {probe_attenuation}") # 配置探头衰减
            osc_inst.write(f"{ch}:COUPLING DC")             # DC耦合
            osc_inst.write(f"{ch}:SCALE {vertical_scale}")  # 垂直量程

        # 4. 水平时基配置(适配100KHz信号)
        osc_inst.write(f"HORIZONTAL:SCALE {horizontal_scale}")

        # 5. 触发配置(稳定触发100KHz信号)
        osc_inst.write("TRIGGER:A:MODE NORMAL")
        osc_inst.write("TRIGGER:A:LEVEL:CH1 2.5")  # 触发电平中点
        time.sleep(0.5)

        # 6. 自动设置, 等待稳定（必须先 AUTOSet, 再配置测量, 否则 AUTOSet 会清除测量配置）
        osc_inst.write("AUTOSet EXECute")
        osc_inst.query("*OPC?")
        time.sleep(1)

        # 7. 测量配置: 固定测量项1=频率, 测量项2=正占空比, 测量项3=周期
        osc_inst.write("MEASUREMENT:MEAS1:TYPE FREQ")
        osc_inst.write("MEASUREMENT:MEAS1:STATE ON")
        osc_inst.write("MEASUREMENT:MEAS2:TYPE PDUty")
        osc_inst.write("MEASUREMENT:MEAS2:STATE ON")
        osc_inst.write("MEASUREMENT:MEAS3:TYPE PERiod")
        osc_inst.write("MEASUREMENT:MEAS3:STATE ON")

        # 8. 测量参考电平优化(百分比模式)
        osc_inst.write("MEASUREMENT:REFLevel:METHOD PERCent")
        osc_inst.write("MEASUREMENT:REFLevel:PERCent:MID1 50.0")
        osc_inst.write("MEASUREMENT:REFLevel:PERCent:HIGH 90.0")
        osc_inst.write("MEASUREMENT:REFLevel:PERCent:LOW 10.0")
        time.sleep(0.5)
        print("✅ 所有通道已打开, 测量配置完成")
        return True

    except Exception as e:
        print(f"❌ 初始化异常: {str(e)}")
        # 异常关闭资源
        if osc_inst:
            osc_inst.close()
        if rm:
            rm.close()
        osc_inst = None
        rm = None
        return False

def measure_once_auto_set():
    global osc_inst
    if not osc_inst:
        print("❌ 示波器未初始化, 无法执行 AutoSet")
        return
    time.sleep(0.2)
    osc_inst.write("AUTOSet EXECute")
    osc_inst.query("*OPC?")
    time.sleep(1)
    # AUTOSet 会清除测量配置, 需重新配置
    osc_inst.write("MEASUREMENT:MEAS1:TYPE FREQ")
    osc_inst.write("MEASUREMENT:MEAS1:STATE ON")
    osc_inst.write("MEASUREMENT:MEAS2:TYPE PDUty")
    osc_inst.write("MEASUREMENT:MEAS2:STATE ON")
    osc_inst.write("MEASUREMENT:MEAS3:TYPE PERiod")
    osc_inst.write("MEASUREMENT:MEAS3:STATE ON")

def measure_once(channel, max_retries=3):
    """
    单次测量函数(仅需输入通道)
    :param channel: 测量通道(如"CH1"/"CH2"/"CH3"/"CH4")
    :param max_retries: 测量无效时最大重试次数(默认3)
    :return: (频率(float):Hz, 周期(float):us, 正占空比(float)):%；测量失败返回(None, None, None)
    """
    global osc_inst
    # 校验示波器是否初始化
    if not osc_inst:
        print("❌ 请先调用init_oscilloscope()初始化示波器!")
        return None, None, None
    # 校验通道格式
    if channel not in ["CH1", "CH2", "CH3", "CH4"]:
        print("❌ 通道无效!请输入CH1/CH2/CH3/CH4")
        return None, None, None

    try:
        # 切换测量源为目标通道
        osc_inst.write(f"MEASUREMENT:MEAS1:SOURCE {channel}")  # 频率源
        osc_inst.write(f"MEASUREMENT:MEAS2:SOURCE {channel}")  # 占空比源
        osc_inst.write(f"MEASUREMENT:MEAS3:SOURCE {channel}")  # 周期源
        time.sleep(0.3)  # 等待配置生效

        freq_val = duty_val = period_val = None

        for attempt in range(max_retries):
            # 读取测量值
            freq_val = osc_inst.query("MEASUREMENT:MEAS1:VALUE?").strip()
            duty_val = osc_inst.query("MEASUREMENT:MEAS2:VALUE?").strip()
            period_val = osc_inst.query("MEASUREMENT:MEAS3:VALUE?").strip()

            # 检查是否为示波器无效值 9.91E+37
            is_valid = True
            for val in (freq_val, duty_val, period_val):
                try:
                    v = float(val)
                    if v > 1e8 or math.isinf(v):
                        is_valid = False
                        break
                except (ValueError, TypeError):
                    is_valid = False
                    break

            if is_valid:
                break  # 测量有效, 退出重试循环

            if attempt < max_retries - 1:
                print(f"  ⚠️ 测量值无效(9.9E37), 等待重试({attempt + 2}/{max_retries})...")
                time.sleep(0.5)  # 等待更长时间再重试

        # 打印原始测量值(过滤掉 9.9E37 避免视觉干扰)
        freq_hz = float(freq_val) if freq_val else 0
        period_us = float(period_val) * 1e6 if period_val else 0
        duty_pct = float(duty_val) if duty_val else 0

        if freq_hz < 1e8 and 0 <= duty_pct <= 100 and period_us < 1e8:
            print(f"📝 原始测量值({channel}): 频率={freq_hz:.3f} Hz, 占空比={duty_pct:.3f} %, 周期={period_us:.3f} us")
        else:
            print(f"⚠️ 示波器测量({channel}): 信号不稳定或未检测到有效波形 (重试{max_retries}次后仍无效)")

        # 转换数值, 过滤示波器错误码9.91E+37(无效值)
        freq = float(freq_val) if (float(freq_val) < 1e8 and not math.isinf(float(freq_val))) else None
        duty = float(duty_val) if (0 <= float(duty_val) <= 100 and not math.isinf(float(duty_val))) else None
        period = float(period_val) * 1e6 if (float(period_val) < 1e8 and not math.isinf(float(period_val))) else None  # 转换为us

        return freq, period, duty

    except ValueError as ve:
        print(f"❌ 数据读取失败: 数值格式错误 - {str(ve)}")
        return None, None, None
    except Exception as e:
        print(f"❌ 测量异常: {str(e)}")
        return None, None, None

def close_oscilloscope():
    """关闭示波器连接(程序结束时调用)"""
    global osc_inst, rm
    if osc_inst:
        osc_inst.close()
    if rm:
        rm.close()
    print("✅ 示波器连接已关闭")

# ------------------- 调用示例 -------------------
if __name__ == "__main__":
    # 1. 初始化示波器(仅需调用一次)
    init_oscilloscope(probe_attenuation=10, vertical_scale=5.0)

    # 2. 单次测量: 获取频率+周期+占空比
    if osc_inst:
        # 测量CH1
        freq, period, duty = measure_once("CH1")
        if freq and period and duty:
            print(f"📊 CH1 单次测量: ")
            print(f"   频率={freq:.1f}Hz, 周期={period:.1f}μs, 正占空比={duty:.1f}%")
        else:
            print("❌ CH1 测量失败")

        # 测量其他通道示例
        # freq_ch2, period_ch2, duty_ch2 = measure_once("CH2")
        # if freq_ch2 and period_ch2 and duty_ch2:
        #     print(f"📊 CH2 单次测量: ")
        #     print(f"   频率={freq_ch2:.1f}Hz, 周期={period_ch2:.1f}μs, 正占空比={duty_ch2:.1f}%")

    # 3. 程序结束关闭连接
    close_oscilloscope()