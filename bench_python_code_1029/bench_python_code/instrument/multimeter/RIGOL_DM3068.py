# -*- coding: utf_8 -*-
import os
import time
import pyvisa
import csv
import numpy as np

from instrument.visa_backend import find_resource

# 全局变量：存储万用表实例
dmm_instance = None


def _match_rigol_dm3068(rm, resources):
    for res in resources:
        # 注意: 不能仅凭厂商号("0x1AB1"/"6833")判断, RIGOL 同厂商的其他仪器(如信号发生器DG1000Z)
        # 共用同一厂商号, 会误匹配到别的设备上。必须用型号专属字段区分:
        #   - "DM3068" 为显式型号字符串
        #   - "DM3O"   为该万用表序列号的实际前缀(注意是字母 O, 如 DM3O225000995), 在 VISA 资源串里出现
        upper_res = res.upper()
        if "DM3068" in upper_res or "DM3O" in upper_res:
            return res
    return None


class DM3068(object):
    """RIGOL DM3068 数字万用表控制类（兼容原函数名）"""
    Measurement_List = [""]

    def __init__(self):
        # 依次用默认后端(走系统已装 NI-VISA 等驱动)和 '@py' 纯 Python 后端查找设备,
        # 以"能否实际匹配到 DM3068"为准, 而不是"是否枚举到任意资源"
        self.rm, self.resource_str = find_resource(_match_rigol_dm3068)
        if self.resource_str is None:
            # 默认地址（请根据实际情况修改）
            self.resource_str = "USB0::0x1AB1::0x0E11::DM3A123456789::INSTR"
        self.Terminals = "FRON"
        self.ReadBuff = []
        self.idn_list = []
        self.Connent_DMM()
        self.ResetDmm()
        self.Finish()


    def Connent_DMM(self, resourceName=""):
        """连接 RIGOL DM3068"""
        if resourceName:
            self.resource_str = resourceName
        self.my_instrument = self.rm.open_resource(self.resource_str)
        self.my_instrument.read_termination = '\n'
        self.my_instrument.write_termination = '\n'
        self.my_instrument.timeout = 5000
        self.idn_list = self.my_instrument.query("*IDN?").strip().split(",")
        print(f"Connected device: {self.idn_list}")
        # 不区分大小写判断厂商和型号
        manufacturer = self.idn_list[0].strip().lower()
        model = self.idn_list[1].strip().lower()
        if "rigol" not in manufacturer or "dm3068" not in model:
            print("警告：未检测到 RIGOL DM3068 设备！")
        # 禁止自动关机，防止长时间测试中断连
        try:
            self.my_instrument.write("SYSTem:POWer:DOWN OFF")
        except Exception:
            pass

    def ResetDmm(self):
        """重置设备（跳过自检以避免超时）"""
        self.my_instrument.write("*RST")
        self.my_instrument.write("*CLS")
        # 不执行 *TST? 查询，因为 DM3068 可能需要很长时间或不可用
        print("设备已重置，跳过自检（DM3068 *TST? 可能耗时较长或不可用）")
        time.sleep(0.1)
        self.DmmDisplayView('NUMeric')

    def ReadConfig(self):
        """读取当前配置"""
        ret = self.my_instrument.query("CONFigure?")
        return ret

    def SetConfig(self, Measurementype, Range="DEF", Resolution="DEF"):
        """电容测量配置（DM3068 支持 CAP）"""
        if Measurementype == "CAP":
            cmd = f"CONF:CAP {Range},{Resolution}" if Range != "DEF" else "CONF:CAP"
            self.my_instrument.write(cmd)
            ret = self.my_instrument.query("READ?")
            return ret

    def ConfigCurrentParameter(self, Measurementype, Range="DEF", Resolution="DEF"):
        """电流测量配置"""
        # DM3068 命令: CONF:CURR:DC {range},{resolution}
        cmd = f"CONF:CURR:{Measurementype} {Range},{Resolution}" if Range != "DEF" else f"CONF:CURR:{Measurementype}"
        self.my_instrument.write(cmd)

    def ConfigCurrentWithRangeAuto(self, Measurementype, Range="DEF", Resolution="DEF"):
        """自动量程电流配置"""
        cmd = "CONF:CURR:DC 10,DEF"  # 先设一个最大量程，自动量程由仪器内部处理
        print("Config", cmd)
        self.my_instrument.write(cmd)

    def MeasureCurrentOneTime(self):
        """单次电流测量（多次采样取平均）"""
        self.my_instrument.write('SAMPle:COUNt 10')
        self.my_instrument.write('INIT')
        self.my_instrument.write("*TRG")
        self.ReadBuff = self.my_instrument.query("FETC?", 5).split(",")
        self.ReadBuff = list(map(float, self.ReadBuff))
        return self.ReadBuff

    def MeasuremenDIODE(self):
        """二极管测量"""
        self.my_instrument.write("CONF:DIOD")

    def MeasuremenFREQuencyORPERiod(self, Measurementype, Range="DEF", Resolution="DEF"):
        """频率/周期测量"""
        Measurementypes = ["FREQuency", "PERiod", "FREQ", "PER"]
        if Measurementype in Measurementypes:
            if Measurementype in ["FREQuency", "FREQ"]:
                cmd = f"CONF:FREQ {Range},{Resolution}" if Range != "DEF" else "CONF:FREQ"
                self.my_instrument.write(cmd)
            elif Measurementype in ["PERiod", "PER"]:
                cmd = f"CONF:PER {Range},{Resolution}" if Range != "DEF" else "CONF:PER"
                self.my_instrument.write(cmd)
        else:
            print("Measurementype not support")

    def MeasuremenRESistanceORFRESistance(self, Measurementype, Range="DEF", Resolution="DEF"):
        """电阻/四线电阻测量（DM3068 支持 RES 和 FRES）"""
        Range = str.upper(Range)
        RangeNumber_Dic = {
            "1G": 1000000000,
            "100M": 100000000,
            "10M": 10000000,
            "1M": 1000000,
            "100K": 100000,
            "10K": 10000,
            "1K": 1000,
            "100": 100
        }
        Measurementypes = ["RESistance", "FRESistance", "RES", "FRES"]
        if Range in RangeNumber_Dic:
            Range = RangeNumber_Dic[Range]

        if Measurementype in Measurementypes:
            if Measurementype in ["RESistance", "RES"]:
                cmd = f"CONF:RES {Range},{Resolution}"
                self.my_instrument.write(cmd)
            elif Measurementype in ["FRESistance", "FRES"]:
                cmd = f"CONF:FRES {Range},{Resolution}"
                self.my_instrument.write(cmd)
        else:
            print("Measurementype not support")

    def ConfigVoltageParameter(self, Measurementype, Range="DEF", Resolution="DEF"):
        """电压测量配置"""
        cmd = f"CONF:VOLT:{Measurementype} {Range},{Resolution}" if Range != "DEF" else f"CONF:VOLT:{Measurementype}"
        self.my_instrument.write(cmd)

    def CheckTerminals(self, Terminals):
        """检查端子（FRON/REAR），增加异常处理"""
        TerminalsTypes = ["FRON", "REAR"]
        Terminals = str.upper(Terminals)
        if Terminals not in TerminalsTypes:
            return False
        try:
            # 临时增加超时时间
            original_timeout = self.my_instrument.timeout
            self.my_instrument.timeout = 2000
            ret = self.my_instrument.query("ROUTe:TERMinals?").strip()
            self.my_instrument.timeout = original_timeout
            self.Terminals = ret
            # DM3068 返回 FRONT 或 REAR，映射为 FRON/REAR
            if ret == "FRONT":
                ret = "FRON"
            elif ret == "REAR":
                ret = "REAR"
            return ret == Terminals
        except Exception as e:
            print(f"端子查询失败（{e}），默认认为端子正确")
            return True   # 或根据实际情况返回 False

    def Sample(self):
        """采样配置（使用内部定时器）"""
        self.my_instrument.write("SAMPle:COUNt 50")
        self.my_instrument.write("SAMPle:SOURce TIMer")

    def ReadBuff(self):
        """读取缓存"""
        ret = self.my_instrument.query("READ?", 1000)
        print(f"result {ret}")
        return ret

    def Finish(self):
        """清空缓存"""
        if self.ReadBuff:
            del self.ReadBuff[:]

    def savebufftocsv(self, logpath):
        """保存测量数据到CSV（与设备无关，保留原逻辑）"""
        filename = logpath
        with open(filename, "w+", newline='') as csvfile:
            fieldnames = ['Device_ID', 'VIH', 'VIL', 'VOH', 'VOL', 'PU', 'PD']
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            writer.writeheader()
            Device_Information = {
                'Device_ID': self.idn_list[0] if self.idn_list else "RIGOL_DM3068",
                'VIH': self.idn_list[1] if len(self.idn_list) > 1 else "",
                'VIL': self.idn_list[2] if len(self.idn_list) > 2 else "",
                'VOH': self.idn_list[3] if len(self.idn_list) > 3 else "",
                'VOL': self.idn_list[4] if len(self.idn_list) > 4 else "",
                'PU': self.idn_list[5] if len(self.idn_list) > 5 else "",
                'PD': self.idn_list[6] if len(self.idn_list) > 6 else "",
            }
            writer.writerow(Device_Information)

        with open(filename, "a+", newline='') as csvfile:
            fieldnames = ["ReadBuff"]
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            writer.writeheader()
            for buff in self.ReadBuff:
                writer.writerow({"ReadBuff": buff})

    def DmmDisplay(self, displayStr):
        """显示文本信息"""
        self.my_instrument.write("DISPlay ON")
        displayStr = displayStr[:32]
        cmd = f"DISPlay:TEXT:DATA \"{displayStr}\",1"
        self.my_instrument.write(cmd)

    def DmmDisplayView(self, ViewType):
        """显示视图配置（DM3068 支持部分视图）"""
        ViewTypes = ["NUMeric", "HISTogram", "TCHart", "METer"]
        ViewType = str.upper(ViewType)
        if ViewType in ViewTypes:
            cmd = f"DISPlay:VIEW {ViewType}"
            self.my_instrument.write(cmd)
        else:
            print(f"DM3068 不支持 {ViewType} 视图，默认使用 NUMeric")
            self.my_instrument.write("DISPlay:VIEW NUMeric")

    def DmmDisplayClear(self):
        """清空显示"""
        self.my_instrument.write("DISPlay:TEXT:CLEar")

    def SetLimit(self, Limit_Low, Limit_Upp):
        """设置上下限"""
        self.LimitClea()
        self.my_instrument.write(f"CALCulate:LIMit:LOWer {Limit_Low}")
        self.my_instrument.write(f"CALCulate:LIMit:UPPer {Limit_Upp}")

    def LimitClea(self):
        """清空限值"""
        self.my_instrument.write("CALCulate:LIMit:CLEar")

    def SetLimitONOFF(self, SetLimitONOFF="OFF"):
        """限值开关"""
        LinitStates = ["ON", "OFF", "0", "1"]
        if SetLimitONOFF in LinitStates:
            self.my_instrument.write(f"CALCulate:LIMit:STATe {SetLimitONOFF}")

    def ReadQuestionable(self):
        """读取可疑状态寄存器"""
        return self.my_instrument.query("STATus:QUEStionable:CONDition?")

    def SetTrigerSource(self, Source):
        """触发源配置"""
        SourceTypes = ["IMMediate", "BUS", "EXTernal", "INTernal", "TIMer"]
        Source = str.upper(Source)
        if Source in SourceTypes:
            # DM3068 触发源命令：TRIGger:SOURce
            self.my_instrument.write(f"TRIGger:SOURce {Source}")
            self.my_instrument.write("TRIGger:DELay MIN")
        else:
            print(f"不支持 {Source} 触发源，默认使用 IMMediate")
            self.my_instrument.write("TRIGger:SOURce IMMediate")

    def SetSampleCount(self, Count):
        """设置采样点数"""
        cmd = f"SAMPle:COUNt {Count}"
        self.my_instrument.write(cmd)

    def VoltageMeasuremen(self, VoltageType, Count=50, Range="DEF", Resolution="DEF", LimitLow=0, LimitUpp=0):
        """电压测量（核心逻辑）"""
        self.Finish()
        Range = str.upper(Range)
        VoltageType = str.upper(VoltageType)
        RangeNumber_Dic = {
            "1000V": 1000, "100V": 100, "10V": 10, "1V": 1,
            "100MV": 0.1, "10MV": 0.01, "1MV": 0.001
        }
        Measurementypes = ["AC", "DC"]

        if (VoltageType in Measurementypes) and (Range in RangeNumber_Dic or Range == "DEF"):
            actual_range = RangeNumber_Dic.get(Range, "DEF")
            self.ConfigVoltageParameter(VoltageType, actual_range, Resolution)
            self.SetTrigerSource("BUS")
            self.SetSampleCount(Count)
            self.LimitClea()

            if LimitLow != 0 and LimitUpp != 0:
                self.SetLimit(LimitLow, LimitUpp)
                self.SetLimitONOFF("ON")

            self.my_instrument.write("ABORt")
            self.my_instrument.write("INITiate")
            self.my_instrument.write("*TRG")
            time.sleep(0.1)

            self.ReadBuff = self.my_instrument.query("FETCh?").split(",")
            self.ReadBuff = list(map(float, self.ReadBuff))
            return self.ReadBuff
        else:
            print("VoltageMeasuremen parameter error")
            return []

    def VoltageQuickMeasure(self):
        """快速电压测量（单次触发读取）"""
        self.my_instrument.write("ABORt")
        self.my_instrument.write("INITiate")
        self.my_instrument.write("*TRG")
        self.ReadBuff = self.my_instrument.query("FETCh?").split(",")
        self.ReadBuff = list(map(float, self.ReadBuff))
        return self.ReadBuff

    def Voltage_Measure(self):
        """电压均值测量"""
        self.VoltageQuickMeasure()
        return np.mean(self.ReadBuff) if self.ReadBuff else 0

    def CurrentMeasuremen(self, CurrentType, Count=50, Range="DEF", Resolution="DEF"):
        """电流测量"""
        CurrentTypes = ["AC", "DC"]
        RangeNumber_Dic = {
            "100nA": 1e-7, "1uA": 1e-6, "10uA": 1e-5, "100uA": 0.0001,
            "1mA": 0.001, "10mA": 0.01, "100mA": 0.1, "1A": 1, "3A": 3, "10A": 10
        }

        if (CurrentType in CurrentTypes) and (Range in RangeNumber_Dic or Range == "DEF"):
            actual_range = RangeNumber_Dic.get(Range, "DEF")
            self.ConfigCurrentParameter(CurrentType, actual_range, Resolution)
            self.SetTrigerSource("BUS")
            self.SetSampleCount(Count)
            self.LimitClea()
            self.SetLimitONOFF("OFF")

            self.my_instrument.write("ABORt")
            self.my_instrument.write("INITiate")
            self.my_instrument.write("*TRG")
            time.sleep(0.1)

            self.ReadBuff = self.my_instrument.query("FETCh?", 5).split(",")
            self.ReadBuff = list(map(float, self.ReadBuff))
            return self.ReadBuff
        else:
            print("CurrentMeasuremen parameter error")
            return []

    def ResistanceMeasuremen(self, ResisType, Count=50, Range="AUTO", Resolution="DEF"):
        """电阻测量"""
        print("ResistanceMeasuremen", Range, Resolution)
        ResistanceTypes = ["RESistance", "FRESistance"]
        RangeNumber_Dic = {
            "AUTO": "AUTO", "100": 100, "1K": 1000, "10K": 10000,
            "100K": 100000, "1M": 1e6, "10M": 1e7, "100M": 1e8, "1G": 1e9
        }

        if (ResisType in ResistanceTypes) and (Range in RangeNumber_Dic):
            actual_range = RangeNumber_Dic[Range]
            cmd = f"CONF:{ResisType} {actual_range},{Resolution}"
            print("Config:", cmd)
            self.my_instrument.write(cmd)

            self.SetTrigerSource("BUS")
            self.SetSampleCount(Count)
            self.LimitClea()
            self.SetLimitONOFF("OFF")

            self.my_instrument.write("ABORt")
            self.my_instrument.write("INITiate")
            time.sleep(0.1)
            self.my_instrument.write("*TRG")
            time.sleep(0.05)

            self.ReadBuff = self.my_instrument.query("FETCh?").split(",")
            self.ReadBuff = list(map(float, self.ReadBuff))
            return self.ReadBuff
        else:
            print("ResistanceMeasuremen parameter error")
            return []

    def AutoTrigger(self):
        """终止测量"""
        self.my_instrument.write("ABORt")


# ---------------------- 封装的核心函数（函数名保持不变） ----------------------
def init_keysight_34465a():
    """
    初始化 RIGOL DM3068 万用表（保持原函数名）
    返回值：bool - 初始化成功返回 True，失败返回 False
    """
    global dmm_instance
    try:
        print("开始初始化 RIGOL DM3068...")
        dmm_instance = DM3068()
        # 检查端子是否为 FRON
        if not dmm_instance.CheckTerminals("FRON"):
            print("错误：端子检查失败（非 FRON 端子）！")
            return False
        print("RIGOL DM3068 初始化成功")
        return True
    except Exception as e:
        print(f"RIGOL DM3068 初始化失败：{str(e)}")
        return False


# 上次成功通信时间戳，用于 keepalive 间隔控制
_last_dmm_ok_time = 0

def _dmm_keepalive():
    """发送无害查询保持 VISA/USB 会话活跃，防止长时间空闲断连"""
    global dmm_instance, _last_dmm_ok_time
    if dmm_instance is None:
        return
    now = time.time()
    # 每 15 秒内检查过一次就不重复发（避免过多 SCPI 流量）
    if now - _last_dmm_ok_time < 15:
        return
    try:
        dmm_instance.my_instrument.query("*IDN?")
        _last_dmm_ok_time = now
    except Exception:
        pass  # 静默失败，后续正式测量会触发重连


def read_keysight_voltage(voltage_type="DC", count=3, range_val="10V", resolution=0.00001, limit_low=0, limit_upp=0, _retry=True):
    """
    读取电压值（保持原函数名）
    参数与原脚本完全一致，内部适配 DM3068
    返回：list[float] — 电压值列表，单位 mV
    """
    global dmm_instance, _last_dmm_ok_time
    if dmm_instance is None:
        print("错误：万用表未初始化！请先调用 init_keysight_34465a()")
        return []

    # keepalive：先唤醒 USB/VISA 会话
    _dmm_keepalive()

    try:
        voltage_data = dmm_instance.VoltageMeasuremen(
            VoltageType=voltage_type,
            Count=count,
            Range=range_val,
            Resolution=resolution,
            LimitLow=limit_low,
            LimitUpp=limit_upp
        )
        _last_dmm_ok_time = time.time()
        # FETCh? 返回值为 V，统一转为 mV
        return [v * 1000.0 for v in voltage_data]
    except Exception as e:
        print(f"读取电压失败：{str(e)}")
        # 会话失效时自动重连一次（避免长时间闲置后 USB/VISA 断连）
        if _retry and ("Invalid session handle" in str(e) or "closed" in str(e).lower()):
            print("🔄 VISA 会话失效，尝试重新初始化万用表...")
            if init_keysight_34465a():
                print("✅ 重新初始化成功，重试电压读取")
                return read_keysight_voltage(
                    voltage_type=voltage_type, count=count, range_val=range_val,
                    resolution=resolution, limit_low=limit_low, limit_upp=limit_upp,
                    _retry=False  # 防止无限重试
                )
            else:
                print("❌ 重新初始化失败")
        return []


def read_single_voltage(voltage_type="DC", range_val="10V", resolution=0.00001,
                        limit_low=0, limit_upp=0):
    """
    读取单次电压值，输出单位为 mV（保持原函数名）
    """
    single_voltage = read_keysight_voltage(
        voltage_type=voltage_type,
        count=1,
        range_val=range_val,
        resolution=resolution,
        limit_low=limit_low,
        limit_upp=limit_upp
    )
    if single_voltage:
        return single_voltage[0]  # read_keysight_voltage 已返回 mV
    else:
        print(f"单次{voltage_type}电压读取失败（量程：{range_val}）")
        return None


def read_single_current(current_type="DC", range_val="1A", resolution="DEF"):
    """
    读取单次电流值，输出单位为 mA（保持原函数名）
    """
    global dmm_instance
    if dmm_instance is None:
        print("错误：万用表未初始化！")
        return None

    try:
        current_data = dmm_instance.CurrentMeasuremen(
            CurrentType=current_type,
            Count=1,
            Range=range_val,
            Resolution=resolution
        )
        if current_data:
            current_value_ma = current_data[0] * 1000.0
            return current_value_ma
        else:
            print("单次电流读取失败：返回空数据")
            return None
    except Exception as e:
        print(f"单次电流测量异常：{str(e)}")
        return None


def read_keysight_resistance(resistance_type="RESistance", count=3, range_val="AUTO", resolution="DEF"):
    """
    读取电阻值（多采样点）。

    参数：
        resistance_type: str - "RESistance"（2线）或 "FRESistance"（4线）
        count: int - 采样数
        range_val: str - 量程（"AUTO" / "100" / "1K" / "10K" / "100K" / "1M" / "10M" / "100M" / "1G"）
        resolution: str - 分辨率（默认 "DEF"）
    返回：
        list[float] — 电阻值列表，单位 Ω（Ohm），失败返回空列表
    """
    global dmm_instance, _last_dmm_ok_time
    if dmm_instance is None:
        print("错误：万用表未初始化！请先调用 init_keysight_34465a()")
        return []

    # keepalive：保持 USB/VISA 活跃
    _dmm_keepalive()

    try:
        resistance_data = dmm_instance.ResistanceMeasuremen(
            ResisType=resistance_type,
            Count=count,
            Range=range_val,
            Resolution=resolution
        )
        _last_dmm_ok_time = time.time()
        # FETCh? 返回值为 Ω，直接返回
        return resistance_data
    except Exception as e:
        print(f"读取电阻失败：{str(e)}")
        if "Invalid session handle" in str(e) or "closed" in str(e).lower():
            print("🔄 VISA 会话失效，尝试重新初始化万用表...")
            if init_keysight_34465a():
                print("✅ 重新初始化成功，重试电阻读取")
                return read_keysight_resistance(
                    resistance_type=resistance_type, count=count,
                    range_val=range_val, resolution=resolution
                )
            else:
                print("❌ 重新初始化失败")
        return []


def read_single_resistance(resistance_type="RESistance", range_val="AUTO", resolution="DEF"):
    """
    单次电阻测量，返回 Ω。

    参数：
        resistance_type: str - "RESistance"（2线）或 "FRESistance"（4线）
        range_val: str - 量程（"AUTO" / "100" / "1K" / "10K" / ...）
        resolution: str - 分辨率（默认 "DEF"）
    返回：
        float/None — 电阻值（Ω），失败返回 None
    """
    resistance_data = read_keysight_resistance(
        resistance_type=resistance_type,
        count=1,
        range_val=range_val,
        resolution=resolution
    )
    if resistance_data:
        return resistance_data[0]
    else:
        print(f"单次电阻读取失败（量程：{range_val}）")
        return None


def release_keysight_resource():
    """
    释放万用表资源（保持原函数名）
    """
    global dmm_instance
    if dmm_instance is None:
        print("警告：万用表实例为空，无需释放")
        return True

    try:
        print("开始释放 RIGOL DM3068 资源...")
        dmm_instance.AutoTrigger()
        dmm_instance.my_instrument.close()
        # 不关闭 rm：ResourceManager 是每个 VISA 库共享的单例，
        # 关掉会把其它仪器（示波器/源表/电源）的会话一起终结。
        dmm_instance = None
        print("RIGOL DM3068 资源释放成功")
        return True
    except Exception as e:
        print(f"释放资源失败：{str(e)}")
        return False


# ---------------------- 品牌中立的别名函数（兼容切换模式） ----------------------
def init_dmm():
    """
    初始化万用表（品牌中立名，兼容 Keysight/RIGOL 切换）
    返回值：bool - 初始化成功返回 True，失败返回 False
    """
    return init_keysight_34465a()


def read_voltage(voltage_type="DC", count=3, range_val="10V", resolution=0.00001, limit_low=0, limit_upp=0):
    """
    读取电压（品牌中立名）
    参数与 read_keysight_voltage 完全一致
    返回：list[float] — 电压值列表，单位 mV
    """
    return read_keysight_voltage(
        voltage_type=voltage_type, count=count, range_val=range_val,
        resolution=resolution, limit_low=limit_low, limit_upp=limit_upp
    )


def read_resistance(resistance_type="RESistance", count=3, range_val="AUTO", resolution="DEF"):
    """
    读取电阻（品牌中立名）
    返回：list[float] — 电阻值列表，单位 Ω
    """
    return read_keysight_resistance(
        resistance_type=resistance_type, count=count,
        range_val=range_val, resolution=resolution
    )


def release_dmm():
    """
    释放万用表资源（品牌中立名）
    返回值：bool - 释放成功返回 True，失败返回 False
    """
    return release_keysight_resource()


# ---------------------- 主函数（示例） ----------------------
def main():
    """主函数：演示初始化、测量、释放资源"""
    if not init_keysight_34465a():
        return

    # 示例：读取 3 次 DC 电压，量程 10V
    dc_data = read_keysight_voltage(voltage_type="DC", count=3, range_val="10V")
    if dc_data:
        print(f"DC 电压平均值：{np.mean(dc_data):.3f} mV")

    # 示例：单次电压读数（mV）
    """
    single_mv = read_single_voltage(voltage_type="DC", range_val="10V")
    if single_mv is not None:
        print(f"单次电压：{single_mv:.3f} mV")
    """
    

    # 示例：单次电流读数（mA）
    """
    single_ma = read_single_current(current_type="DC", range_val="1A")
    if single_ma is not None:
        print(f"单次电流：{single_ma:.3f} mA")
    """
    
    release_keysight_resource()


if __name__ == "__main__":
    main()