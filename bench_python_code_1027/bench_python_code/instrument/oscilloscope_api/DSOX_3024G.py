# -*- coding: utf-8 -*-
"""
KEYSIGHT DSOX3024G 程控脚本
初始化时完成通道/时基/触发配置并执行 :AUToscale；每次测量前也会对目标通道再执行一次。
"""

import math
import pyvisa as visa
import time

from instrument.visa_backend import find_resource

osc_inst = None
rm = None


def _match_dsox_3024g(rm, resources):
    for res in resources:
        try:
            temp_inst = rm.open_resource(res)
            temp_inst.timeout = 2000
            idn = temp_inst.query("*IDN?").strip()
            temp_inst.close()
            if any(kw in idn.upper() for kw in ["DSOX", "MSOX", "3034G", "3024G", "3054G"]):
                print(f"找到示波器：{idn} @ {res}")
                return res
        except Exception:
            continue
    return None


def init_oscilloscope(probe_attenuation=10, vertical_scale=5.0, horizontal_scale=1e-6,
                      auto_scale=True):
    """
    初始化示波器：设置通道、时基和触发，然后执行 Auto Scale。

    参数：
        probe_attenuation: 探头衰减比（1X=1, 10X=10）
        vertical_scale: 垂直量程（V/格）
        horizontal_scale: 水平时基（s/格）
        auto_scale: True（默认）初始化后执行 :AUToscale；False 则只做基本配置
    """
    global osc_inst, rm
    try:
        print("正在搜索示波器...")
        rm, osc_address = find_resource(_match_dsox_3024g)

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

        if auto_scale:
            print("正在执行 Auto Scale ...")
            osc_inst.write(":AUToscale")
            osc_inst.query("*OPC?")      # 等待自动设置完成
            time.sleep(1)

        # AUToscale 会清除测量配置，放在它之后再清一次保证状态一致
        osc_inst.write(":MEASure:CLEar")
        print("✅ 基本配置完成（执行 Auto Scale）" if auto_scale
              else "✅ 基本配置完成（未执行 Auto Scale）")
        return True

    except Exception as e:
        print(f"❌ 初始化异常：{str(e)}")
        if osc_inst:
            osc_inst.close()
        # 不关闭 rm：ResourceManager 是每个 VISA 库的单例，关掉会把万用表/源表等
        # 其它仪器已打开的会话一起关掉（Invalid session handle）。
        osc_inst = None
        rm = None
        return False

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


def close_oscilloscope():
    global osc_inst, rm
    if osc_inst:
        try: osc_inst.close()
        except: pass
    # 不关闭 rm：它是每个 VISA 库共享的单例，关掉会终结其它仪器的会话
    osc_inst = None
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