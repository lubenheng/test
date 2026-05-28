#-*- coding: utf_8 -*-
import os
import time
import pyvisa
import csv
import numpy as np

# 全局变量：存储万用表实例（供各函数共享）
dmm_instance = None  

class DMM34465A(object):
    """Keysight 34465A 数字万用表控制类（保留原逻辑）"""
    Measurement_List=[""]
    def __init__(self):
        self.rm = pyvisa.ResourceManager()
        self.resource_str = self._get_keysight_resource()
        self.Terminals="FRON"
        self.ReadBuff=[]
        self.idn_list=[]
        self.Connent_DMM()
        self.ResetDmm()
        self.Finish()

    def _get_keysight_resource(self):
        """自动查找Keysight 34465A的VISA资源地址"""
        resources = self.rm.list_resources()
        for res in resources:
            if "0x0957" in res and "34465A" in res:
                return res
        return "USB0::0x2A8D::0x0301::MY60016524::0::INSTR"

    def Connent_DMM(self,resourceName=""):
        """连接Keysight 34465A"""
        if resourceName:
            self.resource_str = resourceName
        self.my_instrument = self.rm.open_resource(self.resource_str)
        self.my_instrument.read_termination = '\n'
        self.my_instrument.write_termination = '\n'
        self.my_instrument.timeout = 5000
        self.idn_list = self.my_instrument.query("*IDN?").strip().split(",")
        print(f"Connected device: {self.idn_list}")
        if "KEYSIGHT" not in self.idn_list[0] or "34465A" not in self.idn_list[1]:
            print("警告：未检测到Keysight 34465A设备！")

    def ResetDmm(self):
        """重置设备"""
        self.my_instrument.write("*RST")
        self.my_instrument.write("*CLS")
        ret = self.my_instrument.query("*TST?").strip()
        time.sleep(0.2)
        print(f"Self-test result: {ret}")
        if ret == "+0":
            print("Self-test: PASS")
        else:
            print(f"Self-test: FAIL (code: {ret})")
        self.DmmDisplayView('NUMeric')

    def ReadConfig(self):
        """读取当前配置"""
        ret = self.my_instrument.query("CONFigure?")
        return ret

    def SetConfig(self,Measurementype,Range="DEF",Resolution="DEF"):
        """电容测量配置"""
        if Measurementype=="CAP":
            cmd = f"CONF:CAP {Range},{Resolution}" if Range != "DEF" else "CONF:CAP"
            self.my_instrument.write(cmd)
            ret = self.my_instrument.query("READ?")
            return ret

    def ConfigCurrentParameter(self,Measurementype,Range="DEF",Resolution="DEF"):
        """电流测量配置"""
        CMD=f"CONFigure:CURRent:{Measurementype} {Range},{Resolution}" if Range != "DEF" else f"CONFigure:CURRent:{Measurementype}"
        self.my_instrument.write(CMD)

    def ConfigCurrentWithRangeAuto(self,Measurementype,Range="DEF",Resolution="DEF"):
        """自动量程电流配置"""
        CMD="CONFigure:CURRent:DC 10,DEF"
        print("Config",CMD)
        self.my_instrument.write(CMD)

    def MeasureCurrentOneTime(self):
        """单次电流测量"""
        self.my_instrument.write('SAMPle:COUNt 10')
        self.my_instrument.write('INIT')
        self.my_instrument.write("*TRG")
        self.ReadBuff = self.my_instrument.query("FETC?",5).split(",")
        self.ReadBuff = list(map(float, self.ReadBuff))
        return self.ReadBuff

    def MeasuremenDIODE(self):
        """二极管测量"""
        CMD="CONF:DIOD"
        self.my_instrument.write(CMD)

    def MeasuremenFREQuencyORPERiod(self,Measurementype,Range="DEF",Resolution="DEF"):
        """频率/周期测量"""
        Measurementypes=["FREQuency","PERiod","FREQ","PER"]
        if Measurementype in Measurementypes:
            if Measurementype in ["FREQuency","FREQ"]:
                CMD=f"CONFigure:FREQuency {Range},{Resolution}" if Range != "DEF" else "CONFigure:FREQuency"
                self.my_instrument.write(CMD)
            elif Measurementype in ["PERiod","PER"]:
                CMD=f"CONFigure:PERiod {Range},{Resolution}" if Range != "DEF" else "CONFigure:PERiod"
                self.my_instrument.write(CMD)
        else:
            print("Measurementype not support")

    def MeasuremenRESistanceORFRESistance(self,Measurementype,Range="DEF",Resolution="DEF"):
        """电阻/四线电阻测量"""
        Range=str.upper(Range)
        RangeNumber_Dic={
        "1G":	1000000000,
        "100M":	100000000,
        "10M":	10000000,
        "1M":	1000000,
        "100K":	100000,
        "10K":	10000,
        "1K":	1000,
        "100":	100}
        
        Measurementypes=["RESistance","FRESistance","RES","FRES"]
        if Range in RangeNumber_Dic :
            Range=RangeNumber_Dic[Range]
                
        if Measurementype in Measurementypes:
            if Measurementype in ["RESistance","RES"]:
                CMD=f"CONFigure:RESistance {Range},{Resolution}"
                self.my_instrument.write(CMD)
            elif Measurementype in ["FRESistance","FRES"]:
                CMD=f"CONFigure:FRESistance {Range},{Resolution}"
                self.my_instrument.write(CMD)
        else:
            print("Measurementype not support")

    def ConfigVoltageParameter(self,Measurementype,Range="DEF",Resolution="DEF"):
        """电压测量配置"""
        CMD=f"CONFigure:VOLTage:{Measurementype} {Range},{Resolution}" if Range != "DEF" else f"CONFigure:VOLTage:{Measurementype}"
        self.my_instrument.write(CMD)

    def CheckTerminals(self,Terminals):
        """检查端子"""
        TerminalsTypes=["FRON","REAR"]
        Terminals=str.upper(Terminals)
        if Terminals in TerminalsTypes:
            ret = self.my_instrument.query("ROUTe:TERMinals?").strip()
            self.Terminals = ret
            return ret == Terminals
        return False

    def Sample(self):
        """采样配置"""
        self.my_instrument.write("SAMPle:COUNt 50")
        self.my_instrument.write("SAMPle:SOURce TIMer")

    def ReadBuff(self):
        """读取缓存"""
        ret = self.my_instrument.query("READ?",1000)
        print(f"result {ret}")
        return ret

    def Finish (self):
        """清空缓存"""
        if self.ReadBuff:
            del self.ReadBuff[:]

    def savebufftocsv(self,logpath):
        """保存测量数据到CSV"""
        filename = logpath
        with open(filename, "w+", newline='') as csvfile:
            fieldnames = ['Device_ID','VIH','VIL','VOH','VOL', 'PU', 'PD']
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            writer.writeheader()
            Device_Information={
                'Device_ID': self.idn_list[0] if self.idn_list else "Keysight_34465A",
                'VIH': self.idn_list[1] if len(self.idn_list)>1 else "",
                'VIL': self.idn_list[2] if len(self.idn_list)>2 else "",
                'VOH': self.idn_list[3] if len(self.idn_list)>3 else "",
                'VOL': self.idn_list[4] if len(self.idn_list)>4 else "",
                'PU': self.idn_list[5] if len(self.idn_list)>5 else "",
                'PD': self.idn_list[6] if len(self.idn_list)>6 else "",
            }
            writer.writerow(Device_Information)
        
        with open(filename, "a+", newline='') as csvfile:
            fieldnames = ["ReadBuff"]
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            writer.writeheader()
            for buff in self.ReadBuff:
                writer.writerow({"ReadBuff":buff})

    def DmmDisplay(self,displayStr):
        """显示控制"""
        self.my_instrument.write("DISPlay ON")
        displayStr = displayStr[:32]
        CMD=f"DISPlay:TEXT:DATA \"{displayStr}\",1"
        self.my_instrument.write(CMD)

    def DmmDisplayView(self,ViewType):
        """显示视图配置"""
        ViewTypes=["NUMeric","HISTogram","TCHart","METer"]
        ViewType = str.upper(ViewType)
        if ViewType in ViewTypes:
            CMD=f"DISPlay:VIEW {ViewType}"
            self.my_instrument.write(CMD)
        else:
            print(f"Keysight 34465A不支持{ViewType}视图，默认使用NUMeric")
            self.my_instrument.write("DISPlay:VIEW NUMeric")

    def DmmDisplayClear(self):
        """清空显示"""
        self.my_instrument.write("DISPlay:TEXT:CLEar")

    def SetLimit(self,Limit_Low,Limit_Upp):
        """设置上下限"""
        self.LimitClea()
        self.my_instrument.write(f"CALCulate:LIM:LOW {Limit_Low}")
        self.my_instrument.write(f"CALCulate:LIM:UPP {Limit_Upp}")

    def LimitClea(self):
        """清空限值"""
        self.my_instrument.write("CALCulate:LIMit:CLEar")

    def SetLimitONOFF(self,SetLimitONOFF="OFF"):
        """限值开关"""
        LinitStates=["ON","OFF","0","1"]
        if SetLimitONOFF in LinitStates:
            self.my_instrument.write(f"CALCulate:LIM:STATe {SetLimitONOFF}")

    def ReadQuestionable(self):
        """读取可疑状态"""
        return self.my_instrument.query("STATus:QUEStionable:CONDition?")

    def SetTrigerSource(self,Source):
        """触发源配置"""
        SourceTypes=["IMMediate","BUS","EXTernal","INTernal","TIMer"]
        Source = str.upper(Source)
        if Source in SourceTypes:
            self.my_instrument.write(f"TRIG:SOUR {Source}")
            self.my_instrument.write("TRIG:DEL MIN")
        else:
            print(f"不支持{Source}触发源，默认使用IMMediate")
            self.my_instrument.write("TRIG:SOUR IMMediate")

    def SetSampleCount(self,Count):
        """设置采样数"""
        Cmd=f"SAMPle:COUNt {Count}"
        self.my_instrument.write(Cmd)

    def VoltageMeasuremen(self,VoltageType,Count=50,Range="DEF",Resolution="DEF",LimitLow=0,LimitUpp=0):
        """电压测量（核心逻辑）"""
        self.Finish()
        Range=str.upper(Range)
        VoltageType=str.upper(VoltageType)
        RangeNumber_Dic={ 
            "1000V":1000, "100V":100, "10V":10, "1V":1, 
            "100MV":0.1, "10MV":0.01, "1MV":0.001
        }
        Measurementypes=["AC","DC"]
        
        if (VoltageType in Measurementypes) and (Range in RangeNumber_Dic or Range == "DEF"):
            actual_range = RangeNumber_Dic.get(Range, "DEF")
            self.ConfigVoltageParameter(VoltageType, actual_range, Resolution)
            self.SetTrigerSource("BUS")
            self.SetSampleCount(Count)
            self.LimitClea()
            
            if LimitLow!=0 and LimitUpp!=0:
                self.SetLimit(LimitLow, LimitUpp)
                self.SetLimitONOFF("ON")
            
            self.my_instrument.write("ABOR")
            self.my_instrument.write("INIT")
            self.my_instrument.write("*TRG")
            time.sleep(0.1)
            
            self.ReadBuff = self.my_instrument.query("FETC?").split(",")
            self.ReadBuff = list(map(float, self.ReadBuff))
            return self.ReadBuff
        else:
            print("VoltageMeasuremen parameter error")
            return []

    def VoltageQuickMeasure(self):
        """快速电压测量"""
        self.my_instrument.write("ABOR")
        self.my_instrument.write("INIT")
        self.my_instrument.write("*TRG")
        self.ReadBuff = self.my_instrument.query("FETC?").split(",")
        self.ReadBuff = list(map(float, self.ReadBuff))
        return self.ReadBuff

    def Voltage_Measure(self):
        """电压均值测量"""
        self.VoltageQuickMeasure()
        return np.mean(self.ReadBuff) if self.ReadBuff else 0

    def CurrentMeasuremen(self,CurrentType,Count=50,Range="DEF",Resolution="DEF"):
        """电流测量"""
        CurrentTypes=["AC","DC"]
        RangeNumber_Dic={
            "100nA":1e-7, "1uA":1e-6, "10uA":1e-5, "100uA":0.0001,
            "1mA":0.001, "10mA":0.01,"100mA":0.1, "1A":1, "3A":3
        }
        
        if (CurrentType in CurrentTypes) and (Range in RangeNumber_Dic or Range == "DEF"):
            actual_range = RangeNumber_Dic.get(Range, "DEF")
            self.ConfigCurrentParameter(CurrentType, actual_range, Resolution)
            self.SetTrigerSource("BUS")
            self.SetSampleCount(Count)
            self.LimitClea()
            self.SetLimitONOFF("OFF")
            
            self.my_instrument.write("ABOR")
            self.my_instrument.write("INIT")
            self.my_instrument.write("*TRG")
            time.sleep(0.1)
            
            self.ReadBuff = self.my_instrument.query("FETC?",5).split(",")
            self.ReadBuff = list(map(float, self.ReadBuff))
            return self.ReadBuff
        else:
            print("CurrentMeasuremen parameter error")
            return []

    def ResistanceMeasuremen(self,ResisType,Count=50,Range="AUTO",Resolution="DEF"):
        """电阻测量"""
        print("ResistanceMeasuremen",Range,Resolution)
        ResistanceTypes=["RESistance","FRESistance"]
        RangeNumber_Dic={ 
            "AUTO":"AUTO","100":100, "1K":1000, "10K":10000, 
            "100K":100000, "1M":1e6, "10M":1e7, "100M":1e8, "1G":1e9
        }
        
        if (ResisType in ResistanceTypes) and (Range in RangeNumber_Dic):
            actual_range = RangeNumber_Dic[Range]
            CMD=f"CONFigure:{ResisType} {actual_range},{Resolution}"
            print("Config:",CMD)
            self.my_instrument.write(CMD)
            
            self.SetTrigerSource("BUS")
            self.SetSampleCount(Count)
            self.LimitClea()
            self.SetLimitONOFF("OFF")
            
            self.my_instrument.write("ABOR")
            self.my_instrument.write("INIT")
            time.sleep(0.2)
            self.my_instrument.write("*TRG")
            time.sleep(0.1)
            
            self.ReadBuff = self.my_instrument.query("FETC?").split(",")
            self.ReadBuff = list(map(float, self.ReadBuff))
            return self.ReadBuff
        else:
            print("ResistanceMeasuremen parameter error")
            return []

    def AutoTrigger(self):   
        """终止测量"""
        self.my_instrument.write("ABOR")


# ---------------------- 封装的核心函数 ----------------------
def init_keysight_34465a():
    """
    初始化Keysight 34465A万用表
    返回值：bool - 初始化成功返回True，失败返回False
    """
    global dmm_instance
    try:
        print("开始初始化Keysight 34465A...")
        dmm_instance = DMM34465A()  # 创建万用表实例
        # 检查端子是否为FRON（可选，根据需求调整）
        if not dmm_instance.CheckTerminals("FRON"):
            print("错误：端子检查失败（非FRON端子）！")
            return False
        print("Keysight 34465A初始化成功")
        return True
    except Exception as e:
        print(f"Keysight 34465A初始化失败：{str(e)}")
        return False


def read_keysight_voltage(voltage_type="DC", count=3, range_val="10V", resolution=0.00001, limit_low=0, limit_upp=0):
    """
    读取Keysight 34465A的电压值
    参数：
        voltage_type: str - "AC"或"DC"（默认DC）
        count: int - 采样数（默认3）
        range_val: str - 量程（如"10V"、"100V"，默认10V）
        resolution: float - 分辨率（默认0.00001）
        limit_low: float - 下限（默认0，非0时需配合limit_upp）
        limit_upp: float - 上限（默认0，非0时需配合limit_low）
    返回值：
        list - 电压测量值列表（失败返回空列表）
    """
    global dmm_instance
    if dmm_instance is None:
        print("错误：万用表未初始化！请先调用init_keysight_34465a()")
        return []
    
    try:
        #print(f"开始读取{voltage_type}电压（采样数：{count}，量程：{range_val}）...")
        voltage_data = dmm_instance.VoltageMeasuremen(
            VoltageType=voltage_type,
            Count=count,
            Range=range_val,
            Resolution=resolution,
            LimitLow=limit_low,
            LimitUpp=limit_upp
        )
        #print(f"{voltage_type}电压测量完成，数据：{voltage_data}")
        return voltage_data
    except Exception as e:
        print(f"读取电压失败：{str(e)}")
        return []
    
def read_single_voltage(voltage_type="DC", range_val="10V", resolution=0.00001, 
                        limit_low=0, limit_upp=0):
    """
    读取单次电压的封装函数, 输出为mV（支持灵活配置参数）
    参数：
        voltage_type: str - 电压类型，"DC"（直流，默认）或 "AC"（交流）
        range_val: str - 电压量程（需匹配RangeNumber_Dic中的键，默认"10V"）
                        可选值：1000V/100V/10V/1V/100MV/10MV/1MV
        resolution: float - 测量分辨率（默认0.00001，可根据精度需求调整）
        limit_low: float - 测量下限阈值（默认0，非0时需配合limit_upp使用）
        limit_upp: float - 测量上限阈值（默认0，非0时需配合limit_low使用）
    返回值：
        float/None - 成功返回单次电压值（mV），失败返回None
    """
    # 读取单次电压（核心：count=1 强制仅采样1次，保证单次测量）
    single_voltage = read_keysight_voltage(
        voltage_type=voltage_type,
        count=1,  # 固定为1，确保单次测量（核心逻辑不开放修改）
        range_val=range_val,
        resolution=resolution,
        limit_low=limit_low,
        limit_upp=limit_upp
    )
    # 处理读取结果：转换为mV（原始值为V，乘以1000）
    if single_voltage:
        voltage_value_mv = single_voltage[0] * 1000.0
        #print(f"单次{voltage_type}电压测量完成：{voltage_value_mv:.6f} mV")
    else:
        print(f"单次{voltage_type}电压读取失败（量程：{range_val}）")
        voltage_value_mv = None
    
    return voltage_value_mv


def read_single_current(current_type="DC", range_val="1A", resolution="DEF"):
    """
    读取单次电流值（封装函数），输出单位为mA
    参数：
        current_type: str - 电流类型，"DC"（直流，默认）或 "AC"（交流）
        range_val: str - 电流量程（需匹配RangeNumber_Dic中的键，默认"10mA"）
                        可选值：100nA/1uA/10uA/100uA/1mA/10mA/100mA/1A/3A
        resolution: str/float - 分辨率（默认"DEF"，即默认分辨率）
    返回值：
        float/None - 成功返回单次电流值（mA），失败返回None
    """
    global dmm_instance

    try:
        # 2. 调用电流测量函数，采样数设为1（单次测量）
        current_data = dmm_instance.CurrentMeasuremen(
            CurrentType=current_type,
            Count=1,          # 仅采样1次
            Range=range_val,
            Resolution=resolution
        )
        
        # 3. 处理测量结果并转换单位（A → mA，乘以1000）
        if current_data:
            current_value_a = current_data[0]  # 原始值单位为A
            current_value_ma = current_value_a * 1000.0  # 转换为mA
            #print(f"单次{current_type}电流测量完成：{current_value_ma:.6f} mA")
            return current_value_ma
        else:
            print("单次电流读取失败：返回空数据")
            return None
            
    except Exception as e:
        print(f"单次电流测量异常：{str(e)}")
        return None

def release_keysight_resource():
    """
    释放Keysight 34465A资源（关闭连接）
    返回值：bool - 释放成功返回True，失败返回False
    """
    global dmm_instance
    if dmm_instance is None:
        print("警告：万用表实例为空，无需释放")
        return True
    
    try:
        print("开始释放Keysight 34465A资源...")
        dmm_instance.AutoTrigger()  # 终止测量
        dmm_instance.my_instrument.close()  # 关闭设备连接
        dmm_instance.rm.close()  # 关闭资源管理器
        dmm_instance = None  # 清空实例
        print("Keysight 34465A资源释放成功")
        return True
    except Exception as e:
        print(f"释放资源失败：{str(e)}")
        return False


# ---------------------- 主函数（统一调用） ----------------------
def main():
    """主函数：按流程调用初始化、读电压、释放资源"""
    # 1. 初始化万用表
    init_success = init_keysight_34465a()
    if not init_success:
        return
    
    # 2. 读取电压（可多次调用，按需调整参数）
    # 示例1：读取DC电压（3次采样，10V量程）
    dc_voltage_data = read_keysight_voltage(
        voltage_type="DC",
        count=3,
        range_val="10V",
        limit_low=0,
        limit_upp=5.5
    )
    if dc_voltage_data:
        dc_voltage_mean = np.mean(dc_voltage_data)
        print(f"DC电压平均值：{dc_voltage_mean:.6f} V")
    
    # 示例2：读取AC电压（5次采样，100V量程）
    # ac_voltage_data = read_keysight_voltage(
    #     voltage_type="AC",
    #     count=5,
    #     range_val="100V"
    # )
    # if ac_voltage_data:
    #     ac_voltage_mean = np.mean(ac_voltage_data)
    #     print(f"AC电压平均值：{ac_voltage_mean:.6f} V")
    
    # 3. 释放资源
    release_keysight_resource()


# 程序入口
if __name__ == "__main__":
    main()