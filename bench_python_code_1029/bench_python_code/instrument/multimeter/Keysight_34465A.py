#-*- coding: utf_8 -*-
import os
import time
import pyvisa
import csv
import numpy as np

from instrument.visa_backend import find_resource

# 全局变量：存储万用表实例（供各函数共享）
dmm_instance = None

# =====================================================================
# 电压测量的积分方式：Aperture = Time（而非 NPLC 工频周期数）
#   正常电压测量：SENSe:VOLTage:DC:APERture:ENABled ON
#                 SENSe:VOLTage:DC:APERture <秒>
#   这样积分时间与工频（50/60 Hz）脱钩，换场地/换电源频率时读数不变。
# 所有电压测量路径（单次 / 多次 / 快速扫描）都走这套配置。
# =====================================================================
VOLTAGE_APERTURE_ENABLED = True       # True = Aperture 用 Time；False = 退回 NPLC
VOLTAGE_APERTURE_S = 0.02             # 常规电压测量的积分时间（20 ms）
VOLTAGE_FAST_APERTURE_S = 0.0002      # 扫描类快速测量（200 µs，判高低电平足够）
_APERTURE_MIN_S = 20e-6               # 34465A 直流电压积分时间下限 20 µs
_APERTURE_MAX_S = 1.0                 # 上限 1 s


def _clamp_aperture(seconds):
    """把积分时间限制在仪器允许的 20 µs ~ 1 s 内。"""
    try:
        value = float(seconds)
    except (TypeError, ValueError):
        return VOLTAGE_APERTURE_S
    return max(_APERTURE_MIN_S, min(_APERTURE_MAX_S, value))


def _match_keysight_34465a(rm, resources):
    for res in resources:
        # 厂商号可能是十六进制("0x0957"/"0x2A8D")或十进制("2391", 来自 pyvisa-py 的格式), 同时兼容两种写法
        if "0x0957" in res or "2391" in res or "0x2A8D" in res:
            # 同一厂商号下可能枚举出多个资源串(如 USB 接口的陈旧别名),
            # 必须实际打开并查询 *IDN? 确认是能通信的 34465A, 不能仅凭字符串判断
            try:
                inst = rm.open_resource(res)
                inst.timeout = 2000
                idn = inst.query("*IDN?")
                inst.close()
            except Exception:
                continue
            if "34465A" in idn:
                return res
    return None


class DMM34465A(object):
    """Keysight 34465A 数字万用表控制类（保留原逻辑）"""
    Measurement_List=[""]
    def __init__(self):
        self.rm, self.resource_str = find_resource(_match_keysight_34465a)
        if self.resource_str is None:
            self.resource_str = "USB0::0x2A8D::0x0301::MY60016524::0::INSTR"
        self.Terminals="FRON"
        self.ReadBuff=[]
        self.idn_list=[]
        self.Connent_DMM()
        self.ResetDmm()
        self.Finish()

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
        if "KEYSIGHT" not in self.idn_list[0].upper() or "34465A" not in self.idn_list[1]:
            print("警告：未检测到Keysight 34465A设备！")

    def session_ok(self):
        """检查 VISA 会话是否仍可用（发一条 *IDN?），可用返回 True。"""
        try:
            idn = self.my_instrument.query("*IDN?").strip()
            return bool(idn)
        except Exception as e:
            print(f"   万用表会话检查失败：{e}")
            return False

    def Reconnect(self):
        """会话句柄失效时重新建立连接。

        **不关闭 ResourceManager**：pyvisa 的 RM 是每个 VISA 库一个的单例
        （ResourceManager.__new__ 会复用 visa_library.resource_manager），
        rm.close() 会把本进程里所有仪器的会话一起关掉，正是
        「Invalid session handle」的根源。这里只重开自己的 Resource。
        """
        print("   检测到万用表会话失效，正在重新连接 ...")
        try:
            self.my_instrument.close()
        except Exception:
            pass

        # 先尝试用现有 RM 重开；RM 本身失效时再重新查找
        try:
            self.my_instrument = self.rm.open_resource(self.resource_str)
        except Exception as e:
            print(f"   用原 RM 重开失败（{e}），重新查找设备 ...")
            try:
                self.rm, matched = find_resource(_match_keysight_34465a)
                if matched:
                    self.resource_str = matched
                self.my_instrument = self.rm.open_resource(self.resource_str)
            except Exception as e2:
                print(f"   ❌ 万用表重连失败：{e2}")
                return False

        self.my_instrument.read_termination = '\n'
        self.my_instrument.write_termination = '\n'
        self.my_instrument.timeout = 5000
        try:
            self.idn_list = self.my_instrument.query("*IDN?").strip().split(",")
            print(f"   万用表已重新连接：{self.idn_list}")
        except Exception as e:
            print(f"   ❌ 重连后仍无响应：{e}")
            return False
        return True

    def ResetDmm(self):
        """重置设备"""
        self.my_instrument.write("*RST")
        self.my_instrument.write("*CLS")
        ret = self.my_instrument.query("*TST?").strip()
        time.sleep(0.1)
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

    # 34465A 支持的直流电流量程（档位名 → 安培值）。
    # 注意：本机最低电流档是 **100 µA**（规格 DC 100 µA ~ 10 A），
    # 没有 100 nA / 1 µA / 10 µA 档；下发这些值会被仪器以 -222 拒绝，
    # 结果就是「功能没切过去、仍停在电压档」。
    CurrentRange_Dic = {
        "100uA": 1e-4, "1mA": 1e-3, "10mA": 1e-2, "100mA": 0.1,
        "1A": 1, "3A": 3, "10A": 10,
    }

    def _first_error(self):
        """取并清空错误队列，返回第一条非 0 错误（无错误返回 None）。

        会话失效等通信异常**不吞掉**，直接抛给调用方，让它触发重连重试。
        """
        first = None
        for _ in range(10):
            err = self.my_instrument.query("SYSTem:ERRor?").strip()
            if err.startswith("+0") or err.startswith("0,"):
                break
            if first is None:
                first = err
        return first

    def ConfigDCCurrentMeasurement(self, Range="100uA", Resolution="DEF"):
        """把万用表切到【直流电流档】并锁定量程（漏电等 µA 级测量用）。

        34465A 的 CONFigure:CURRent:DC 同时设定「功能 + 量程 + 分辨率」。
        为了不被上一条未完成的测量挡住，先 ABOR；命令被拒时会依次退到
        「不带分辨率」「只切功能自动量程」两种写法重试，最后回读
        SENSe:FUNCtion? / SENSe:CURRent:DC:RANGe? 把实际状态打印出来。

        参数：
            Range: str - 量程：100uA / 1mA / 10mA / 100mA / 1A / 3A / 10A
                        （34465A 最低 100 µA，不支持 1 µA / 10 µA 档）
            Resolution: str/float - 分辨率（默认 DEF）
        返回：
            bool - 回读确认功能为电流返回 True；否则 False（会打印仪器错误）
        """
        rng = None
        for name, value in self.CurrentRange_Dic.items():
            if name.upper() == str(Range).strip().upper():
                rng = value
                break
        if rng is None:
            print(f"错误：万用表不支持电流量程 {Range}"
                  f"（34465A 可选 {'/'.join(self.CurrentRange_Dic)}）")
            return False

        # 会话失效（Invalid session handle）时重连一次再试：这类错误通常是
        # 别的驱动关掉了共享的 ResourceManager 造成的，重开会话即可恢复。
        for attempt in (1, 2):
            try:
                if not self.session_ok():
                    if attempt == 1:
                        if not self.Reconnect():
                            return False
                        continue
                    print("   ❌ 万用表会话不可用（*IDN? 无响应）")
                    return False
                return self._config_dc_current_body(rng, Resolution)
            except Exception as e:
                msg = str(e)
                if attempt == 1 and ("Invalid session" in msg
                                     or "closed" in msg.lower()
                                     or "-1073807346" in msg):
                    print(f"   ⚠️ 万用表会话已失效（{msg}），重新连接后重试 ...")
                    if not self.Reconnect():
                        return False
                    continue
                print(f"切换直流电流档异常：{e}")
                return False
        return False

    def _config_dc_current_body(self, rng, Resolution):
        """ConfigDCCurrentMeasurement 的实际下发部分（量程已换算成安培值）。"""
        self.my_instrument.write("ABORt")
        self._first_error()          # 清掉之前残留的错误

        attempted = []
        for cmd in (f"CONFigure:CURRent:DC {rng},{Resolution}",
                    f"CONFigure:CURRent:DC {rng}",
                    "CONFigure:CURRent:DC"):
            attempted.append(cmd)
            self.my_instrument.write(cmd)
            err = self._first_error()
            if err is None:
                print(f"   万用表：{cmd}")
                break
            print(f"   ⚠️ {cmd} 被拒：{err}")
        else:
            print(f"错误：万用表拒绝切换到直流电流档（试过 {len(attempted)} 种写法）")
            return False

        # 锁定量程（失败不致命：不改也不影响测量，只是可能自动换档）
        try:
            self.my_instrument.write("SENSe:CURRent:DC:RANGe:AUTO OFF")
        except Exception:
            pass
        self.SetTrigerSource("BUS")
        self.SetSampleCount(1)
        self.LimitClea()
        self.SetLimitONOFF("OFF")

        # 回读实际状态：功能 / 量程 / 自动量程
        func = self.my_instrument.query("SENSe:FUNCtion?").strip().upper()
        try:
            rng_now = self.my_instrument.query("SENSe:CURRent:DC:RANGe?").strip()
        except Exception:
            rng_now = "?"
        try:
            auto_now = self.my_instrument.query("SENSe:CURRent:DC:RANGe:AUTO?").strip()
        except Exception:
            auto_now = "?"

        ok = "CURR" in func
        print(f"   万用表回读：功能={func}，电流量程={rng_now} A，"
              f"自动量程={auto_now} → {'直流电流档已生效' if ok else '未切换成功'}")
        if ok:
            self._first_error()
        return ok

    def DrainErrors(self):
        """清空并打印仪器错误队列（只打印第一条非 0 错误）。"""
        try:
            first = self._first_error()
        except Exception:
            return
        if first:
            print(f"⚠️ 万用表错误：{first}")

    def MeasureCurrentOneTime(self):
        """单次电流测量"""
        self.my_instrument.write('SAMPle:COUNt 10')
        self.my_instrument.write('INIT')
        self.my_instrument.write("*TRG")
        self.ReadBuff = self.my_instrument.query("FETC?").split(",")
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

    def ConfigVoltageAperture(self, Measurementype="DC", Aperture=None):
        """把电压测量的积分方式设为 **Aperture = Time**（而非 NPLC）。

        下发的 SCPI（以 DCV 为例）：
            SENSe:VOLTage:DC:APERture:ENABled ON     ← 选 Time 积分
            SENSe:VOLTage:DC:APERture <秒>           ← 积分时间

        必须在 CONFigure 之后调用（CONFigure 会重置积分方式）。
        参数：
            Measurementype: str - "DC" 或 "AC"
            Aperture: float/None - 积分时间（秒）；None 用 VOLTAGE_APERTURE_S
        返回：
            bool - 两条命令都成功返回 True
        """
        if not VOLTAGE_APERTURE_ENABLED:
            return False
        if Aperture is None:
            Aperture = VOLTAGE_APERTURE_S
        seconds = _clamp_aperture(Aperture)
        mtype = str(Measurementype).upper()
        if mtype not in ("DC", "AC"):
            mtype = "DC"

        try:
            # AC 档积分时间最短 1 ms，且部分固件不支持 ENABled 命令，失败时退回只写时间
            self.my_instrument.write(f"SENSe:VOLTage:{mtype}:APERture:ENABled ON")
            self.my_instrument.write(f"SENSe:VOLTage:{mtype}:APERture {seconds}")
            return True
        except Exception as e:
            print(f"   ⚠️ 设置 Aperture=Time（{seconds:g} s）失败：{e}，"
                  f"万用表保持原积分方式")
            return False

    def ConfigVoltageParameter(self,Measurementype,Range="DEF",Resolution="DEF",Aperture=None):
        """电压测量配置（CONFigure + **Aperture = Time**）。

        Aperture 必须在 CONFigure 之后设置（CONFigure 会重置积分方式）。
        """
        CMD=f"CONFigure:VOLTage:{Measurementype} {Range},{Resolution}" if Range != "DEF" else f"CONFigure:VOLTage:{Measurementype}"
        self.my_instrument.write(CMD)
        self.ConfigVoltageAperture(Measurementype, Aperture)

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
        # 34465A 的直流电流量程下限是 100 µA（没有 100nA/1uA/10uA 档），
        # 下发不存在的量程会被仪器拒绝，导致「功能没切过去、读数还是电压」。
        RangeNumber_Dic={
            "100uA":0.0001, "1mA":0.001, "10mA":0.01,
            "100mA":0.1, "1A":1, "3A":3, "10A":10
        }

        if (CurrentType in CurrentTypes) and (Range in RangeNumber_Dic or Range == "DEF"):
            actual_range = RangeNumber_Dic.get(Range, "DEF")
            self.my_instrument.write("ABOR")       # 先结束上一条未完成的测量
            self.ConfigCurrentParameter(CurrentType, actual_range, Resolution)
            self.SetTrigerSource("BUS")
            self.SetSampleCount(Count)
            self.LimitClea()
            self.SetLimitONOFF("OFF")

            self.my_instrument.write("ABOR")
            self.my_instrument.write("INIT")
            self.my_instrument.write("*TRG")
            time.sleep(0.1)

            self.ReadBuff = self.my_instrument.query("FETC?").split(",")
            self.ReadBuff = list(map(float, self.ReadBuff))
            return self.ReadBuff
        else:
            print(f"CurrentMeasuremen parameter error：量程 {Range!r} 不可用 "
                  f"（34465A 可选 {'/'.join(RangeNumber_Dic)} 或 DEF）")
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
            time.sleep(0.1)
            self.my_instrument.write("*TRG")
            time.sleep(0.05)
            
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

    for attempt in range(2):  # 第一次失败后自动重连并重试一次
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
            # FETCh? 返回值为 V，统一转为 mV
            return [v * 1000.0 for v in voltage_data]
        except Exception as e:
            print(f"读取电压失败：{str(e)}")
            is_session_error = "Invalid session handle" in str(e) or "VI_ERROR" in str(e)
            if attempt == 0 and is_session_error:
                try:
                    dmm_instance.Reconnect()
                    continue
                except Exception as re_e:
                    print(f"重连万用表失败：{str(re_e)}")
                    return []
            return []
    return []
    
def prepare_fast_voltage(voltage_type="DC", range_val="10V", aperture_s=None,
                         autozero_off=True, nplc=None):
    """准备快速电压测量：**只配置一次**，之后用 read_fast_voltage() 逐点读取。

    dmm_measure_single_voltage() 每次调用都会重新 CONFigure + INIT + *TRG，
    并且内部固定 sleep 0.1 s，单点约 100~200 ms；扫几百个点时明显偏慢。
    本函数把配置一次性做完（量程 / **Aperture = Time** / 触发源 / 采样数），
    之后每次 read_fast_voltage() 只发一条 READ?（实测 3~5 ms）。

    参数：
        voltage_type: str - "DC"（默认）或 "AC"
        range_val: str - 量程（与 read_single_voltage 的取值一致，默认 "10V"）
        aperture_s: float/None - 积分时间（秒）；None 用 VOLTAGE_FAST_APERTURE_S
                                （默认 200 µs，判高低电平足够）
        autozero_off: bool - True 关闭自动调零（更快，短期测量无影响）
        nplc: float/None - **已废弃**，仅为兼容旧调用保留；Aperture=Time 后不再使用
    返回：
        bool - 配置成功返回 True
    """
    global dmm_instance
    if dmm_instance is None:
        print("错误：万用表未初始化！请先调用init_keysight_34465a()")
        return False

    range_map = {"1000V": 1000, "100V": 100, "10V": 10, "1V": 1,
                 "100MV": 0.1, "10MV": 0.01, "1MV": 0.001}
    rng = range_map.get(str(range_val).upper())
    if rng is None:
        print(f"错误：不支持的量程 {range_val}")
        return False

    if aperture_s is None:
        aperture_s = VOLTAGE_FAST_APERTURE_S
    if nplc is not None:
        print(f"   ℹ️ Aperture=Time 模式，忽略旧参数 nplc={nplc:g}"
              f"（使用 {_clamp_aperture(aperture_s):g} s）")

    try:
        inst = dmm_instance.my_instrument
        dmm_instance.Finish()
        vtype = str(voltage_type).upper()
        inst.write(f"CONFigure:VOLTage:{vtype} {rng},DEF")
        # Aperture = Time（CONFigure 之后设置，否则会被重置）
        dmm_instance.ConfigVoltageAperture(vtype, aperture_s)
        inst.write(f"SENSe:VOLTage:DC:ZERO:AUTO {'OFF' if autozero_off else 'ON'}")
        inst.write("TRIGger:SOURce IMMediate")
        inst.write("SAMPle:COUNt 1")
        time.sleep(0.1)
        print(f"   万用表：Aperture = Time，{_clamp_aperture(aperture_s) * 1e6:.0f} µs"
              f"（量程 {range_val}）")
        return True
    except Exception as e:
        print(f"快速电压测量配置失败：{e}")
        return False


def read_fast_voltage():
    """读取一次电压（mV），需先调用 prepare_fast_voltage()。
    返回：float/None - 电压值（mV）；会话异常时自动重连并重试一次
    """
    global dmm_instance
    if dmm_instance is None:
        print("错误：万用表未初始化！请先调用init_keysight_34465a()")
        return None

    for attempt in range(2):
        try:
            raw = dmm_instance.my_instrument.query("READ?")
            values = [float(x) for x in str(raw).strip().split(",") if x.strip()]
            if not values:
                return None
            return values[0] * 1000.0        # V → mV
        except Exception as e:
            if attempt == 0 and ("Invalid session handle" in str(e) or "VI_ERROR" in str(e)):
                print(f"万用表会话异常（{e}），尝试重连 ...")
                try:
                    dmm_instance.Reconnect()
                    prepare_fast_voltage()
                    continue
                except Exception as re_e:
                    print(f"重连万用表失败：{re_e}")
                    return None
            print(f"快速电压读取失败：{e}")
            return None
    return None


def finish_fast_voltage():
    """结束快速测量模式，恢复常规配置（自动调零打开、Aperture = Time 20 ms）。"""
    global dmm_instance
    if dmm_instance is None:
        return
    try:
        inst = dmm_instance.my_instrument
        inst.write("SENSe:VOLTage:DC:ZERO:AUTO ON")
        inst.write("CONFigure:VOLTage:DC 10,DEF")
        # 恢复到常规积分时间（仍保持 Aperture = Time）
        dmm_instance.ConfigVoltageAperture("DC", VOLTAGE_APERTURE_S)
        time.sleep(0.1)
    except Exception as e:
        print(f"恢复万用表默认配置失败：{e}")


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
    # 处理读取结果（read_keysight_voltage 已返回 mV）
    if single_voltage:
        voltage_value_mv = single_voltage[0]
    else:
        print(f"单次{voltage_type}电压读取失败（量程：{range_val}）")
        voltage_value_mv = None
    
    return voltage_value_mv


def set_dc_current_range(range_val="100uA", resolution="DEF"):
    """
    把万用表切到【直流电流档】并锁定量程（模块级封装）。

    参数：
        range_val: str - 量程，可选 100uA/1mA/10mA/100mA/1A/3A/10A
                          （34465A 最低 100 µA，没有 1 µA / 10 µA 档）
        resolution: str/float - 分辨率（默认 DEF）
    返回值：
        bool - 成功切到直流电流档返回 True
    """
    global dmm_instance
    if dmm_instance is None:
        print("错误：万用表未初始化，无法设置直流电流档")
        return False
    return dmm_instance.ConfigDCCurrentMeasurement(range_val, resolution)


def check_session():
    """
    检查万用表会话是否可用（发一条 *IDN?），失效时自动重连一次。

    返回值：
        bool - True=会话可用（或重连成功），False=不可用
    """
    global dmm_instance
    if dmm_instance is None:
        print("错误：万用表未初始化！")
        return False
    if dmm_instance.session_ok():
        return True
    return dmm_instance.Reconnect()


def set_voltage_aperture(seconds=None, voltage_type="DC"):
    """
    把万用表电压测量的积分方式设为 **Aperture = Time** 并指定积分时间（模块级封装）。

    直接下发（不重新 CONFigure，保持当前功能/量程）：
        SENSe:VOLTage:<DC|AC>:APERture:ENABled ON
        SENSe:VOLTage:<DC|AC>:APERture <秒>

    参数：
        seconds: float/None - 积分时间（秒，自动限制在 20 µs ~ 1 s）；None 用 VOLTAGE_APERTURE_S
        voltage_type: str - "DC"（默认）或 "AC"
    返回值：
        bool - 成功返回 True
    """
    global dmm_instance
    if dmm_instance is None:
        print("错误：万用表未初始化，无法设置积分时间")
        return False
    return dmm_instance.ConfigVoltageAperture(voltage_type, seconds)


def get_voltage_aperture(voltage_type="DC"):
    """
    回读当前电压积分配置（模块级封装）。

    返回：
        dict/None - {"enabled": bool, "aperture_s": float, "nplc": float}；
                    读取失败返回 None
    """
    global dmm_instance
    if dmm_instance is None:
        return None
    mtype = str(voltage_type).upper()
    if mtype not in ("DC", "AC"):
        mtype = "DC"
    inst = dmm_instance.my_instrument
    try:
        en_text = inst.query(f"SENSe:VOLTage:{mtype}:APERture:ENABled?").strip()
        ap_text = inst.query(f"SENSe:VOLTage:{mtype}:APERture?").strip()
        try:
            nplc_text = inst.query(f"SENSe:VOLTage:{mtype}:NPLC?").strip()
        except Exception:
            nplc_text = ""
        return {
            "enabled": en_text.upper().lstrip("+").startswith("1"),
            "aperture_s": float(ap_text),
            "nplc": float(nplc_text) if nplc_text else None,
        }
    except Exception as e:
        print(f"回读积分配置失败：{e}")
        return None


def read_single_current(current_type="DC", range_val="1A", resolution="DEF"):
    """
    读取单次电流值（封装函数），输出单位为mA
    参数：
        current_type: str - 电流类型，"DC"（直流，默认）或 "AC"（交流）
        range_val: str - 电流量程（需匹配RangeNumber_Dic中的键，默认"1A"）
                        可选值：100uA/1mA/10mA/100mA/1A/3A/10A
                        （34465A 最低 100 µA，没有 1 µA / 10 µA 档）
        resolution: str/float - 分辨率（默认"DEF"，即默认分辨率）
    返回值：
        float/None - 成功返回单次电流值（mA），失败返回None

    会话失效（Invalid session handle）时自动重连并重试一次——这类错误通常是
    某个驱动关掉了共享的 ResourceManager 造成的，重开会话即可恢复。
    """
    global dmm_instance
    if dmm_instance is None:
        print("错误：万用表未初始化！")
        return None

    for attempt in range(2):
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
                return current_value_ma
            print("单次电流读取失败：返回空数据")
            return None

        except Exception as e:
            msg = str(e)
            is_session_error = ("Invalid session" in msg or "VI_ERROR" in msg
                                or "closed" in msg.lower())
            if attempt == 0 and is_session_error:
                print(f"单次电流测量异常：{msg} → 重新连接万用表后重试")
                try:
                    if dmm_instance.Reconnect():
                        continue
                except Exception as re_e:
                    print(f"重连万用表失败：{re_e}")
                return None
            print(f"单次电流测量异常：{msg}")
            return None
    return None

def release_keysight_resource():
    """
    释放Keysight 34465A资源（关闭本万用表连接）
    返回值：bool - 释放成功返回True，失败返回False

    只关闭万用表自己的 Resource，**不关闭 ResourceManager**：
    RM 是每个 VISA 库一个的单例，关闭它会把示波器/源表/电源的会话一起终结。
    """
    global dmm_instance
    if dmm_instance is None:
        print("警告：万用表实例为空，无需释放")
        return True

    try:
        print("开始释放Keysight 34465A资源...")
        try:
            dmm_instance.AutoTrigger()  # 终止测量
        except Exception:
            pass
        dmm_instance.my_instrument.close()  # 关闭设备连接（不关 RM）
        dmm_instance = None  # 清空实例
        print("Keysight 34465A资源释放成功")
        return True
    except Exception as e:
        print(f"释放资源失败：{str(e)}")
        return False


def read_keysight_resistance(resistance_type="RESistance", count=3, range_val="AUTO", resolution="DEF"):
    """
    读取电阻值（多采样点）。

    参数：
        resistance_type: str - "RESistance"（2线）或 "FRESistance"（4线）
        count: int - 采样数
        range_val: str - 量程（"AUTO" / "100" / "1K" / "10K" / "100K" / "1M" / "10M" / "100M" / "1G"）
        resolution: str - 分辨率（默认 "DEF"）
    返回：
        list[float] — 电阻值列表，单位 Ω，失败返回空列表
    """
    global dmm_instance
    if dmm_instance is None:
        print("错误：万用表未初始化！")
        return []

    try:
        resistance_data = dmm_instance.ResistanceMeasuremen(
            ResisType=resistance_type,
            Count=count,
            Range=range_val,
            Resolution=resolution
        )
        return resistance_data
    except Exception as e:
        print(f"读取电阻失败：{str(e)}")
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
        print(f"DC电压平均值：{dc_voltage_mean:.3f} mV")
    
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