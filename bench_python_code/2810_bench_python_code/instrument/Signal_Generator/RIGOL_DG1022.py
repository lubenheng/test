"""
RigolDG1022 - PyVISA 控制 RIGOL DG1022 函数/任意波形发生器

该类封装了通过 PyVISA 控制 RIGOL DG1022 的常用操作，
包括波形设置、频率/幅度/偏移设置、输出控制以及频率扫描。

使用示例:
    with RigolDG1022("TCPIP0::192.168.1.100::INSTR") as dg:
        dg.set_waveform(1, "SIN")
        dg.set_frequency(1, 1000)
        dg.set_amplitude(1, 5.0, "VPP")
        dg.set_offset(1, 0)
        dg.enable_output(1, True)
"""

import pyvisa
import time
import math
import logging

logger = logging.getLogger(__name__)


# 支持的波形类型映射表: RIGOL DG1022 的 SCPI 命令后缀
WAVEFORM_MAP = {
    "SIN":     "SINusoid",
    "SINE":    "SINusoid",
    "SQUARE":  "SQUare",
    "SQR":     "SQUare",
    "RAMP":    "RAMP",
    "PULSE":   "PULSe",
    "NOISE":   "NOISe",
    "DC":      "DC",
    "USER":    "USER",
    "ARB":     "USER",
}

# 支持的幅度单位
AMPLITUDE_UNITS = {"VPP", "VRMS", "DBM"}


class RigolDG1022Error(Exception):
    """RIGOL DG1022 操作异常基类"""
    pass

class RigolDG1022:
    """
    RIGOL DG1022 函数/任意波形发生器控制类

    通过 PyVISA 连接仪器，使用 SCPI 命令进行控制。
    支持上下文管理器协议 (with 语句) 自动释放资源。
    """

    def __init__(self, visa_address: str, timeout: int = 5000, reset: bool = False):
        """
        初始化 RigolDG1022 对象。

        通过 VISA 地址连接仪器，设置超时，并切换到远程控制模式。

        参数:
            visa_address: str - VISA 资源地址，例如:
                - "TCPIP0::192.168.1.100::INSTR"      (LAN 连接)
                - "USB0::0x1AB1::0x0640::DG1D151800001::INSTR" (USB 连接)
                - "GPIB0::1::INSTR"                    (GPIB 连接)
            timeout: int - VISA I/O 超时时间（毫秒），默认 5000ms
            reset: bool - 是否在初始化时复位仪器（*RST），默认 False

        异常:
            pyvisa.errors.VisaIOError: 连接失败
            RigolDG1022Error: 仪器响应异常
        """
        self.visa_address = visa_address
        self._resource = None    # PyVISA resource 对象
        self._connected = False

        # 连接仪器
        try:
            rm = pyvisa.ResourceManager()
            self._resource = rm.open_resource(visa_address)
            self._resource.timeout = timeout
            logger.info("已连接到 RIGOL DG1022 @ %s", visa_address)
        except pyvisa.errors.VisaIOError as e:
            raise RigolDG1022Error(
                f"无法连接到 {visa_address}: {e}"
            ) from e

        # 查询仪器标识，确认连接的是 DG1022
        try:
            idn = self._query("*IDN?")
            if "DG1022" not in idn:
                logger.warning("连接的设备可能不是 DG1022 (IDN=%s)", idn)
            else:
                logger.info("仪器标识: %s", idn.strip())
        except Exception as e:
            self.close()
            raise RigolDG1022Error(f"查询仪器标识失败: {e}") from e

        # 设置远程控制模式
        try:
            self._write(":SYSTem:REMote")
            logger.debug("已设置远程控制模式")
        except Exception as e:
            self.close()
            raise RigolDG1022Error(f"设置远程模式失败: {e}") from e

        # 可选: 复位仪器
        if reset:
            self._write("*RST")
            time.sleep(0.5)

        self._connected = True

    # ------------------------------------------------------------------
    # 内部 VISA 读写方法
    # ------------------------------------------------------------------

    def _write(self, command: str):
        """
        向仪器发送 SCPI 命令（写操作）。

        参数:
            command: str - SCPI 命令字符串

        异常:
            RigolDG1022Error: 写入失败
        """
        if self._resource is None:
            raise RigolDG1022Error("仪器未连接")
        try:
            self._resource.write(command)
        except pyvisa.errors.VisaIOError as e:
            raise RigolDG1022Error(
                f"SCPI 写入失败: {command} -> {e}"
            ) from e

    def _query(self, command: str) -> str:
        """
        向仪器发送 SCPI 查询命令，并返回响应字符串。

        SCPI 查询命令以 '?' 结尾，仪器返回响应 + 换行符。
        此方法自动去除首尾空白字符。

        参数:
            command: str - SCPI 查询命令

        返回:
            str - 仪器响应内容（去除首尾空白）

        异常:
            RigolDG1022Error: 查询失败
        """
        if self._resource is None:
            raise RigolDG1022Error("仪器未连接")
        try:
            response = self._resource.query(command)
            return response.strip()
        except pyvisa.errors.VisaIOError as e:
            raise RigolDG1022Error(
                f"SCPI 查询失败: {command} -> {e}"
            ) from e

    # ------------------------------------------------------------------
    # 公共方法
    # ------------------------------------------------------------------

    def set_waveform(self, channel: int = 1, type: str = "SIN") -> None:
        """
        设置指定通道的输出波形类型。

        使用 :SOURce[1|2]:APPLy:<WaveType> 命令同时设置波形类型，
        该命令也会清除该通道上之前的相位/频率/幅度设定，但会保留
        当前频率和幅度值（除非通过此命令的参数重新指定）。

        参数:
            channel: int - 通道号 (1 或 2)
            type: str - 波形类型，支持:
                - "SIN" / "SINE"    : 正弦波
                - "SQUARE" / "SQR"  : 方波
                - "RAMP"            : 锯齿波
                - "PULSE"           : 脉冲波
                - "NOISE"           : 噪声
                - "DC"              : 直流
                - "USER" / "ARB"    : 任意波

        异常:
            RigolDG1022Error: 参数错误或命令执行失败
            ValueError: 波形类型不支持
        """
        # 参数校验
        if channel not in (1, 2):
            raise ValueError(f"通道号必须为 1 或 2，收到: {channel}")

        wave_upper = type.upper()
        if wave_upper not in WAVEFORM_MAP:
            raise ValueError(
                f"不支持的波形类型: {type}。"
                f"支持的类型: {', '.join(sorted(set(WAVEFORM_MAP.values())))}"
            )

        scpi_wave = WAVEFORM_MAP[wave_upper]

        # :SOURce[1|2]:APPLy:<WaveType>
        # 功能：选择指定通道的波形类型并应用，同时可选设置频率、幅度、偏移。
        # 例：:SOURce1:APPLy:SINusoid   — 通道1输出正弦波
        cmd = f":SOURce{channel}:APPLy:{scpi_wave}"
        self._write(cmd)
        logger.info("通道%d 波形设置为 %s", channel, scpi_wave)

    def set_frequency(self, channel: int = 1, freq: float = 1000) -> None:
        """
        设置指定通道的输出频率。

        使用 :SOURce[1|2]:FREQuency:FIXed 命令。
        注意：必须先设置波形类型，频率设置才能生效。

        参数:
            channel: int - 通道号 (1 或 2)
            freq: float - 频率值（单位：Hz），范围取决于波形类型:
                - 正弦波: 1 μHz ~ 25 MHz
                - 方波:   1 μHz ~ 10 MHz
                - 锯齿波: 1 μHz ~ 200 kHz
                - 脉冲波: 1 μHz ~ 10 MHz
                - 噪声/直流: 无频率设置

        异常:
            RigolDG1022Error: 命令执行失败
            ValueError: 参数无效
        """
        if channel not in (1, 2):
            raise ValueError(f"通道号必须为 1 或 2，收到: {channel}")
        if freq <= 0:
            raise ValueError(f"频率必须大于 0，收到: {freq}")

        # :SOURce[1|2]:FREQuency:FIXed <freq>
        # 功能：设置通道的输出频率（Hz）
        # 例：:SOURce1:FREQuency:FIXed 1000  — 通道1频率设为 1000 Hz
        cmd = f":SOURce{channel}:FREQuency:FIXed {freq}"
        self._write(cmd)
        logger.info("通道%d 频率设置为 %.6f Hz", channel, freq)

    def set_amplitude(self, channel: int = 1, amplitude: float = 1.0,
                      unit: str = "VPP") -> None:
        """
        设置指定通道的输出幅度。

        先设置幅度单位 (:SOURce[1|2]:VOLTage:UNIT)，再设置幅度值。
        幅度单位支持 VPP（峰峰值）、VRMS（有效值）、DBM（分贝毫瓦）。

        参数:
            channel: int - 通道号 (1 或 2)
            amplitude: float - 幅度值
            unit: str - 幅度单位，可选:
                - "VPP"  : 峰峰值（默认）
                - "VRMS" : 有效值
                - "DBM"  : 分贝毫瓦

        异常:
            RigolDG1022Error: 命令执行失败
            ValueError: 参数无效
        """
        if channel not in (1, 2):
            raise ValueError(f"通道号必须为 1 或 2，收到: {channel}")
        if amplitude < 0:
            raise ValueError(f"幅度不能为负数，收到: {amplitude}")

        unit_upper = unit.upper()
        if unit_upper not in AMPLITUDE_UNITS:
            raise ValueError(
                f"不支持的幅度单位: {unit}。"
                f"支持的单位: {', '.join(sorted(AMPLITUDE_UNITS))}"
            )

        # :SOURce[1|2]:VOLTage:UNIT {VPP|VRMS|DBM}
        # 功能：设置幅度的单位
        # 例：:SOURce1:VOLTage:UNIT VPP  — 使用峰峰值单位
        cmd_unit = f":SOURce{channel}:VOLTage:UNIT {unit_upper}"
        self._write(cmd_unit)

        # :SOURce[1|2]:VOLTage[:LEVel][:IMMediate][:AMPLitude] <amplitude>
        # 功能：设置通道的输出幅度值（使用当前设定的单位）
        # 例：:SOURce1:VOLTage 5.0  — 通道1幅度设为 5.0 VPP
        cmd_ampl = f":SOURce{channel}:VOLTage {amplitude}"
        self._write(cmd_ampl)

        logger.info("通道%d 幅度设置为 %.4f %s", channel, amplitude, unit_upper)

    def set_offset(self, channel: int = 1, offset: float = 0) -> None:
        """
        设置指定通道的直流偏移电压。

        使用 :SOURce[1|2]:VOLTage:OFFSet 命令。
        偏移范围与当前幅度设置相关，通常为 ±(2×幅度 - 0.2)Vp-p 范围内。

        参数:
            channel: int - 通道号 (1 或 2)
            offset: float - 直流偏移电压（单位：V）

        异常:
            RigolDG1022Error: 命令执行失败
            ValueError: 参数无效
        """
        if channel not in (1, 2):
            raise ValueError(f"通道号必须为 1 或 2，收到: {channel}")

        # :SOURce[1|2]:VOLTage:OFFSet <offset>
        # 功能：设置输出波形的直流偏移电压（V）
        # 例：:SOURce1:VOLTage:OFFSet 0.5  — 通道1偏移设为 +0.5V
        cmd = f":SOURce{channel}:VOLTage:OFFSet {offset}"
        self._write(cmd)
        logger.info("通道%d 直流偏移设置为 %.4f V", channel, offset)

    def enable_output(self, channel: int = 1, on: bool = True) -> None:
        """
        打开或关闭指定通道的输出。

        使用 :OUTPut[1|2]:STATe 命令控制通道输出的启用/禁用。

        参数:
            channel: int - 通道号 (1 或 2)
            on: bool - True 打开输出，False 关闭输出

        异常:
            RigolDG1022Error: 命令执行失败
            ValueError: 参数无效
        """
        if channel not in (1, 2):
            raise ValueError(f"通道号必须为 1 或 2，收到: {channel}")

        state = "ON" if on else "OFF"

        # :OUTPut[1|2]:STATe {ON|OFF|1|0}
        # 功能：启用或禁用指定通道的输出继电器
        # 例：:OUTPut1:STATe ON   — 打开通道1输出
        #     :OUTPut1:STATe OFF  — 关闭通道1输出
        cmd = f":OUTPut{channel}:STATe {state}"
        self._write(cmd)
        logger.info("通道%d 输出 %s", channel, "打开" if on else "关闭")

    def sweep_frequency(self, channel: int,
                        start_freq: float,
                        stop_freq: float,
                        points: int = 50,
                        log_scale: bool = True) -> None:
        """
        对指定通道执行频率扫描（扫频）操作。

        扫描原理:
            1. 设置扫描起始/截止频率 (:SOURce[1|2]:SWEep:FREQuency:STARt/:STOP)
            2. 设置频率间隔方式 (:SOURce[1|2]:SWEep:FREQuency:SPACing LOG/LIN)
            3. 根据扫描点数计算扫描时间并设置
               (:SOURce[1|2]:SWEep:SWEepTime)
            4. 启用扫描模式 (:SOURce[1|2]:SWEep:STATe ON)

        注意:
            - 扫描模式下波形类型通常设为 RAMP（锯齿波）或 SIN（正弦波）
            - 在启用扫描前请确保已通过 set_waveform() 设置波形
            - 此方法不会自动设置波形类型，请在调用前手动设置

        参数:
            channel: int - 通道号 (1 或 2)
            start_freq: float - 扫描起始频率（Hz）
            stop_freq: float - 扫描截止频率（Hz）
            points: int - 扫描点数（决定扫描速度），点数越多扫描越慢
            log_scale: bool - True 对数扫描 (LOG)，False 线性扫描 (LIN)

        异常:
            RigolDG1022Error: 命令执行失败
            ValueError: 参数无效
        """
        if channel not in (1, 2):
            raise ValueError(f"通道号必须为 1 或 2，收到: {channel}")
        if start_freq <= 0:
            raise ValueError(f"起始频率必须大于 0，收到: {start_freq}")
        if stop_freq <= 0:
            raise ValueError(f"截止频率必须大于 0，收到: {stop_freq}")
        if start_freq >= stop_freq:
            raise ValueError(
                f"起始频率 ({start_freq} Hz) 必须小于截止频率 ({stop_freq} Hz)"
            )
        if points < 2:
            raise ValueError(f"扫描点数至少为 2，收到: {points}")

        spacing = "LOG" if log_scale else "LIN"

        # ---- 1. 设置扫描起始频率 ----
        # :SOURce[1|2]:SWEep:FREQuency:STARt <freq>
        # 功能：设置频率扫描的起始频率（Hz）
        cmd_start = f":SOURce{channel}:SWEep:FREQuency:STARt {start_freq}"
        self._write(cmd_start)

        # ---- 2. 设置扫描截止频率 ----
        # :SOURce[1|2]:SWEep:FREQuency:STOP <freq>
        # 功能：设置频率扫描的截止频率（Hz）
        cmd_stop = f":SOURce{channel}:SWEep:FREQuency:STOP {stop_freq}"
        self._write(cmd_stop)

        # ---- 3. 设置频率间隔方式（对数/线性） ----
        # :SOURce[1|2]:SWEep:FREQuency:SPACing {LIN|LOG}
        # 功能：设置频率扫描的间隔方式
        #   - LIN: 线性间隔，频率按等差数列变化
        #   - LOG: 对数间隔，频率按等比数列变化（更适合宽频率范围）
        cmd_spacing = f":SOURce{channel}:SWEep:FREQuency:SPACing {spacing}"
        self._write(cmd_spacing)

        # ---- 4. 计算并设置扫描时间 ----
        # 扫描总时间 = 点数 × 每个点的驻留时间（这里取 0.1s/point）
        # 可根据实际需要调整驻留时间
        dwell_time_per_point = 0.1  # 秒
        sweep_time = points * dwell_time_per_point

        # :SOURce[1|2]:SWEep:SWEepTime <seconds>
        # 功能：设置一次完整扫描所需的时间（秒）
        # 例：:SOURce1:SWEep:SWEepTime 5.0  — 扫描时间为 5 秒
        cmd_time = f":SOURce{channel}:SWEep:SWEepTime {sweep_time}"
        self._write(cmd_time)

        # ---- 5. 启用扫描模式 ----
        # :SOURce[1|2]:SWEep:STATe {ON|OFF}
        # 功能：启用或禁用频率扫描模式
        # 启用后，仪器将按设定的起始/截止频率和时间进行自动扫描
        cmd_sweep_on = f":SOURce{channel}:SWEep:STATe ON"
        self._write(cmd_sweep_on)

        logger.info(
            "通道%d 频率扫描已启动: %s→%s Hz, %s 扫描, %d 点, 周期 %.1f s",
            channel, start_freq, stop_freq, spacing, points, sweep_time,
        )

    def stop_sweep(self, channel: int = 1) -> None:
        """
        停止指定通道的频率扫描，恢复固定频率输出模式。

        使用 :SOURce[1|2]:SWEep:STATe OFF 禁用扫描。

        参数:
            channel: int - 通道号 (1 或 2)
        """
        if channel not in (1, 2):
            raise ValueError(f"通道号必须为 1 或 2，收到: {channel}")
        cmd = f":SOURce{channel}:SWEep:STATe OFF"
        self._write(cmd)
        logger.info("通道%d 扫描已停止", channel)

    # ------------------------------------------------------------------
    # 查询方法
    # ------------------------------------------------------------------

    def query_waveform(self, channel: int = 1) -> str:
        """
        查询指定通道当前的波形类型。

        使用 :SOURce[1|2]:FUNCtion? 查询。

        参数:
            channel: int - 通道号 (1 或 2)

        返回:
            str - 当前波形类型字符串（如 "SIN", "SQU" 等）
        """
        if channel not in (1, 2):
            raise ValueError(f"通道号必须为 1 或 2，收到: {channel}")
        cmd = f":SOURce{channel}:FUNCtion?"
        return self._query(cmd)

    def query_frequency(self, channel: int = 1) -> float:
        """
        查询指定通道当前的输出频率。

        使用 :SOURce[1|2]:FREQuency:FIXed? 查询。

        参数:
            channel: int - 通道号 (1 或 2)

        返回:
            float - 当前频率值（Hz）
        """
        if channel not in (1, 2):
            raise ValueError(f"通道号必须为 1 或 2，收到: {channel}")
        cmd = f":SOURce{channel}:FREQuency:FIXed?"
        return float(self._query(cmd))

    def query_amplitude(self, channel: int = 1) -> float:
        """
        查询指定通道当前的输出幅度。

        使用 :SOURce[1|2]:VOLTage? 查询。

        参数:
            channel: int - 通道号 (1 或 2)

        返回:
            float - 当前幅度值（单位由当前设置决定）
        """
        if channel not in (1, 2):
            raise ValueError(f"通道号必须为 1 或 2，收到: {channel}")
        cmd = f":SOURce{channel}:VOLTage?"
        return float(self._query(cmd))

    def query_offset(self, channel: int = 1) -> float:
        """
        查询指定通道当前的直流偏移。

        使用 :SOURce[1|2]:VOLTage:OFFSet? 查询。

        参数:
            channel: int - 通道号 (1 或 2)

        返回:
            float - 当前直流偏移电压（V）
        """
        if channel not in (1, 2):
            raise ValueError(f"通道号必须为 1 或 2，收到: {channel}")
        cmd = f":SOURce{channel}:VOLTage:OFFSet?"
        return float(self._query(cmd))

    # ------------------------------------------------------------------
    # 资源管理 / 上下文管理器
    # ------------------------------------------------------------------

    def close(self) -> None:
        """
        关闭仪器连接并释放 VISA 资源。

        安全操作:
            1. 关闭所有通道的输出
            2. 关闭 VISA 连接

        此方法可多次调用而不会出错。
        """
        if not self._connected or self._resource is None:
            return

        try:
            # 关闭所有通道输出，确保仪器进入安全状态
            for ch in (1, 2):
                try:
                    self.enable_output(ch, on=False)
                except Exception:
                    pass  # 关闭输出失败不影响整体释放

            # 关闭 VISA 资源
            self._resource.close()
            logger.info("RIGOL DG1022 连接已关闭")

        except Exception as e:
            logger.warning("关闭连接时出现异常: %s", e)

        finally:
            self._connected = False
            self._resource = None

    def __enter__(self):
        """
        上下文管理器入口: 返回 self 实例。

        使用示例:
            with RigolDG1022("TCPIP0::192.168.1.100::INSTR") as dg:
                dg.set_waveform(1, "SIN")
        """
        return self

    def __exit__(self, exc_type, exc_val, exc_tb):
        """
        上下文管理器出口: 自动关闭仪器连接。

        无论 with 块中是否发生异常，都会调用 close() 释放资源。
        """
        self.close()
        # 返回 False 表示异常不会被抑制，交由上层处理
        return False

    def __del__(self):
        """
        析构函数: 对象被垃圾回收时自动释放资源。
        """
        self.close()


# =====================================================================
# 使用示例 (直接运行此脚本时)
# =====================================================================
if __name__ == "__main__":
    # 通道1: 正弦波 1kHz, 1Vpp, 0V 偏移
    addr = "USB0::0x1AB1::0x0642::DG1ZA253904766::INSTR"
    with RigolDG1022(addr) as dg:
        dg.set_waveform(1, "SIN")
        dg.set_frequency(1, 1000)
        dg.set_amplitude(1, 1.0, "VPP")
        dg.set_offset(1, 0)
        dg.enable_output(1, True)
        input("通道1 已输出: 正弦波 1kHz 1Vpp 0V偏移\n按 Enter 键关闭输出...")


