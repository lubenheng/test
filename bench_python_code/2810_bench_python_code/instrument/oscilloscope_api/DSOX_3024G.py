# -*- coding: utf-8 -*-
"""
KEYSIGHT DSOX3024G 程控脚本（按需自动缩放版）
仅在每次测量时对目标通道执行 :AUToscale，初始化不做自动设置。
"""

import math
import pyvisa as visa
import time

osc_inst = None
rm = None


def init_oscilloscope(probe_attenuation=10, vertical_scale=5.0, horizontal_scale=1e-6):
    """
    初始化示波器：不做 Auto Scale，仅设置通道、时基和触发。
    """
    global osc_inst, rm
    try:
        rm = visa.ResourceManager()
        osc_address = None

        print("正在搜索示波器...")
        for res in rm.list_resources():
            try:
                temp_inst = rm.open_resource(res)
                temp_inst.timeout = 2000
                idn = temp_inst.query("*IDN?").strip()
                temp_inst.close()
                if any(kw in idn.upper() for kw in ["DSOX", "MSOX", "3034G", "3024G", "3054G"]):
                    osc_address = res
                    print(f"找到示波器：{idn} @ {res}")
                    break
            except Exception:
                continue

        if not osc_address:
            print("❌ 未找到 Keysight DSOX3024G 或兼容示波器！")
            return

        osc_inst = rm.open_resource(osc_address)
        osc_inst.timeout = 15000
        osc_inst.write("*RST")
        time.sleep(1)

        idn = osc_inst.query("*IDN?").strip()
        print(f"✅ 示波器初始化成功：{idn}")

        for ch in ["CHANnel1", "CHANnel2", "CHANnel3", "CHANnel4"]:
            osc_inst.write(f":{ch}:DISPlay ON")
            osc_inst.write(f":{ch}:PROBe {probe_attenuation}")
            osc_inst.write(f":{ch}:COUPling DC")
            osc_inst.write(f":{ch}:SCALe {vertical_scale}")

        osc_inst.write(f":TIMebase:SCALe {horizontal_scale}")

        osc_inst.write(":TRIGger:SWEep AUTO")
        osc_inst.write(":TRIGger:MODE EDGE")
        osc_inst.write(":TRIGger:EDGE:SOURce CHANnel1")
        osc_inst.write(":TRIGger:EDGE:LEVel 2.5")
        time.sleep(0.5)

        osc_inst.write(":MEASure:CLEar")
        print("✅ 基本配置完成（未执行 Auto Scale）")

    except Exception as e:
        print(f"❌ 初始化异常：{str(e)}")
        if osc_inst:
            osc_inst.close()
        if rm:
            rm.close()
        osc_inst = None
        rm = None

def measure_once_auto_set():
    """
    执行示波器 Auto Scale（等同于按下 [Auto Scale] 键）。
    示波器自动评估所有输入信号并调整垂直/水平/触发设置。
    """
    global osc_inst
    if not osc_inst:
        print("❌ 示波器未初始化")
        return
    try:
        osc_inst.write(":AUToscale")
        osc_inst.query("*OPC?")   # 等待操作完成
        time.sleep(0.5)
        #print("✅ Auto Scale 完成")
    except Exception as e:
        print(f"❌ Auto Scale 异常：{str(e)}")


def measure_once(channel):
    """
    单次测量指定通道。
    每次调用会先对目标通道执行 :AUToscale，延时 1 秒后测量。
    返回 (频率 Hz, 周期 us, 正占空比 %)；失败返回 (None, None, None)
    """
    #print(f"正在测量 {channel}...")
    global osc_inst
    if not osc_inst:
        print("❌ 示波器未初始化，请先调用 init_oscilloscope()")
        return None, None, None

    # 兼容 "CH1" 格式，内部转为 "CHANnel1"
    ch = channel if channel.startswith("CHANnel") else channel.replace("CH", "CHANnel")
    valid_channels = ["CHANnel1", "CHANnel2", "CHANnel3", "CHANnel4"]
    if ch not in valid_channels:
        print(f"❌ 通道无效！有效通道: CH1~CH4")
        return None, None, None

    try:
        # 3️⃣ 设置测量源
        osc_inst.write(f":MEASure:SOURce {ch}")
        time.sleep(0.3)

        # 4️⃣ 执行测量
        freq_val = osc_inst.query(":MEASure:FREQuency?").strip()
        period_val = osc_inst.query(":MEASure:PERiod?").strip()
        duty_val = osc_inst.query(":MEASure:DUTYcycle?").strip()

        freq_hz = float(freq_val) if freq_val else 0
        period_us = float(period_val) * 1e6 if period_val else 0
        duty_pct = float(duty_val) if duty_val else 0
        #print(f"📝 原始值（{ch}）：频率={freq_hz:.3f} Hz，周期={period_us:.3f} us，占空比={duty_pct:.3f} %")

        freq = float(freq_val) if (-1e6 < float(freq_val) < 1e8 and not math.isinf(float(freq_val))) else None
        period_raw = float(period_val)
        period = period_raw * 1e6 if (0 < period_raw < 1e2 and not math.isinf(period_raw)) else None
        duty = float(duty_val) if (0 <= float(duty_val) <= 100 and not math.isinf(float(duty_val))) else None

        return freq, period, duty

    except Exception as e:
        print(f"❌ 测量异常：{str(e)}")
        try:
            err = osc_inst.query(":SYSTem:ERRor?").strip()
            print(f"   仪器错误：{err}")
        except:
            pass
        return None, None, None


def capture_waveform_data(channel="CH1", points=1000):
    """
    从指定通道捕获波形采样数据点。

    通过 SCPI 命令从示波器 :WAVeform 子系统获取原始波形数据（ASCII 格式），
    并查询 X 轴增量 (XINCrement) 生成时间轴，将时间和电压合并为 NumPy 数组。

    注意：ASCII 格式下 DSOX3024G 直接返回电压值（科学计数法，单位 V），
    因此无需 YINCrement/YORigin 转换。

    参数：
        channel: str - 通道名（"CH1"/"CH2"/"CH3"/"CH4"）
        points: int - 采样点数 (100~10000，默认 1000)

    返回：
        data: numpy.ndarray 或 None
            形状 (N, 2) 的 NumPy 数组，第一列时间(秒)，第二列电压(V)。
            失败返回 None。
    """
    global osc_inst
    if not osc_inst:
        print("❌ 示波器未初始化，请先调用 init_oscilloscope()")
        return None

    ch = channel if channel.startswith("CHANnel") else channel.replace("CH", "CHANnel")
    valid_channels = ["CHANnel1", "CHANnel2", "CHANnel3", "CHANnel4"]
    if ch not in valid_channels:
        print(f"❌ 通道无效！有效通道: CH1~CH4")
        return None

    try:
        # 1. 选择波形数据源
        osc_inst.write(f":WAVeform:SOURce {ch}")
        time.sleep(0.05)

        # 2. 设置采样点数
        actual_points = max(100, min(10000, points))
        osc_inst.write(f":WAVeform:POINts {actual_points}")
        time.sleep(0.05)

        # 3. 设置数据格式为 ASCii（逗号分隔的浮点数 = 直接电压值，单位 V）
        #    ASCII 格式下返回的实际电压值（如 +2.456E+00），无需转换
        osc_inst.write(":WAVeform:FORMat ASCii")
        time.sleep(0.05)

        # 4. 查询 X 轴参数（时间轴）
        xinc_str = osc_inst.query(":WAVeform:XINCrement?").strip()
        xinc = float(xinc_str)                          # 秒/点
        xorg_str = osc_inst.query(":WAVeform:XORigin?").strip()
        xorg = float(xorg_str)                          # 秒

        # 5. 读取波形数据（使用 write + read_raw 避免 ASCII 编码问题）
        osc_inst.write(":WAVeform:DATA?")
        time.sleep(0.1)
        raw_data = osc_inst.read_raw()
        # 将字节数据解码为字符串
        raw_str = raw_data.decode("utf-8", errors="ignore").strip()

        # 解析 "#<digits><length><val1>,<val2>,...,<valN>"
        if not raw_str or not raw_str.startswith("#"):
            print(f"❌ 波形数据返回格式异常: 前40字符={raw_str[:40]}")
            return None
        hash_digits = int(raw_str[1])
        data_start = 2 + hash_digits
        payload = raw_str[data_start:]
        payload = payload.lstrip(",")

        # 将逗号分隔的字符串转换为浮点数（直接就是电压值 V）
        raw_values = []
        for val in payload.split(","):
            val = val.strip()
            if val:
                try:
                    raw_values.append(float(val))
                except ValueError:
                    continue

        n = len(raw_values)
        if n == 0:
            print("❌ 未捕获到有效波形数据点")
            return None

        # 6. 生成时间轴（秒）
        #    ASCII 格式下返回的数值直接就是电压值(V)，无需转换
        import numpy as np
        time_array = xorg + np.arange(n, dtype=float) * xinc
        voltage_array = np.array(raw_values, dtype=float)

        # 合并为 (N, 2) 的 NumPy 数组
        data = np.column_stack((time_array, voltage_array))

        # 显示电压范围用于验证
        vmin = voltage_array.min()
        vmax = voltage_array.max()
        print(f"  📊 从 {ch} 捕获 {n} 个采样点 (时基={xinc*1e6:.3f} us/点)")
        print(f"    电压范围: {vmin:.3f} V ~ {vmax:.3f} V")
        return data

    except Exception as e:
        print(f"❌ 波形数据捕获异常 ({channel}): {e}")
        try:
            err = osc_inst.query(":SYSTem:ERRor?").strip()
            if err and "+0" not in err:
                print(f"   仪器错误: {err}")
        except:
            pass
        return None


def save_waveform_to_csv(channel_data, filename):
    """
    将波形数据保存为 CSV 文件。

    参数：
        channel_data: dict - 通道名到 NumPy 数组的映射
            {"CH1": np.ndarray, "CH2": np.ndarray, ...}
            每个数组形状 (N, 2)，第一列时间(s)，第二列电压(V)
        filename: str - 输出文件名（默认可加 .csv 后缀）

    返回：
        str - 保存的文件路径，失败返回 None
    """
    import numpy as np
    if not filename.endswith(".csv"):
        filename += ".csv"

    try:
        # 确定最大点数
        max_points = 0
        for ch, data in channel_data.items():
            if data is not None and isinstance(data, np.ndarray) and data.ndim == 2:
                max_points = max(max_points, data.shape[0])

        if max_points == 0:
            print("❌ 无有效波形数据可保存")
            return None

        # 构建 CSV 行数据
        rows = []
        # 构建表头: 对于每个通道，依次为 "Time (s)", "Voltage (V)"
        headers = []
        for ch in sorted(channel_data.keys()):
            headers.append(f"时间 (s) - {ch}")
            headers.append(f"电压 (V) - {ch}")
        rows.append(",".join(headers))

        # 写入数据行（对齐各通道数据）
        for i in range(max_points):
            row_vals = []
            for ch in sorted(channel_data.keys()):
                data = channel_data[ch]
                if data is not None and isinstance(data, np.ndarray) and data.ndim == 2 and i < data.shape[0]:
                    row_vals.append(f"{data[i, 0]:.12e}")
                    row_vals.append(f"{data[i, 1]:.12e}")
                else:
                    row_vals.append("")
                    row_vals.append("")
            rows.append(",".join(row_vals))

        csv_content = "\n".join(rows)

        with open(filename, "w", encoding="utf-8") as f:
            f.write(csv_content)

        total_points = sum(data.shape[0] for data in channel_data.values()
                           if data is not None and isinstance(data, np.ndarray) and data.ndim == 2)
        print(f"✅ 波形数据已保存为 {filename}")
        print(f"   共采集 {total_points} 个数据点")
        return filename

    except Exception as e:
        print(f"❌ 保存 CSV 文件失败: {e}")
        return None


def close_oscilloscope():
    global osc_inst, rm
    if osc_inst:
        try: osc_inst.close()
        except: pass
    if rm:
        try: rm.close()
        except: pass
    print("✅ 示波器连接已关闭")


# ------------------- 调用示例 -------------------
if __name__ == "__main__":
    init_oscilloscope()  # 初始化不做 Auto Scale

    if osc_inst:
        # 测量 CH1（仅此时执行 Auto Scale）
        freq, period, duty = measure_once("CH1")
        if freq and period and duty:
            print(f"📊 CH1 测量结果：")
            print(f"   频率 = {freq:.1f} Hz")
            print(f"   周期 = {period:.1f} μs")
            print(f"   正占空比 = {duty:.1f}%")
        else:
            print("❌ CH1 测量失败，请检查信号连接。")

    close_oscilloscope()