"""
chip_b 芯片参数与初始化接口（模板）

新芯片参照此文件实现自己的初始化流程。
"""
from instrument.IIC.i2c_comm import I2CController
from instrument.IIC.utils import DEFAULT_CONFIG, load_config, save_config
from instrument.platform.init_helper import check_init
from instrument.platform.serial_init import serial_find_and_init
# =====================================================================
# 芯片标识（显示在 main.py 菜单标题中）
# =====================================================================
CHIP_LABEL = "OCD2361"

# main.py 启动提示与初始化后动作
# UART 用于控制基准板（DAC 输出、继电器切换），复位命令让继电器回到初始状态
INIT_LABEL = "IIC / UART / 万用表 / 示波器 / 源表"
POST_INIT_CMD = "cmd_gpio_set_595d_test reset\r\n"

# =====================================================================
# JLink / 串口参数（按实际芯片修改）
# =====================================================================
JLINK_DEVICE = "YOUR_DEVICE_NAME"
JLINK_SPEED = 1000
SERIAL_KEYWORD = "CH340"
SERIAL_BAUDRATE = 115200

# 模块级 IIC 控制器实例，初始化后供测试项复用
_i2c = None


def init_all(instr, port=None):
    """芯片全套初始化（main.py 入口调用）

    OCD2361 初始化 IIC + UART + 万用表 + 示波器 + 源表，不连接 SWD。
    IIC 失败时仍会初始化其余部分，由操作员决定是否忽略。

    参数:
        instr: instrument_set 模块引用
        port: str - IIC 串口号；None 则从 i2c_config.json 读取
    返回:
        bool - 全部初始化成功返回 True
    """
    print(f"🔌 正在初始化 {CHIP_LABEL}...")
    try:
        i2c = _init_i2c(port)
        check_init("IIC", i2c is not None)
    except Exception as e:
        print(f"❌ IIC 初始化失败：{e}")
        check_init("IIC", False)   # 允许操作员忽略后继续初始化仪器

    _init_instruments(instr)
    return True


def _init_instruments(instr):
    """初始化 UART / 万用表 / 示波器 / 源表，失败时可选择忽略继续。"""
    check_init("UART", serial_find_and_init(keyword=SERIAL_KEYWORD,
                                            baudrate=SERIAL_BAUDRATE))
    check_init("万用表", instr.dmm_init(),
               confirm_prompt=f"当前万用表驱动：{instr._dmm.__name__.split('.')[-1]}，"
                             "请确认与实际连接的设备一致")
    check_init("示波器", instr.osc_init(),
               confirm_prompt=f"当前示波器驱动：{instr._osc.__name__.split('.')[-1]}，"
                             "请确认与实际连接的设备一致")
    check_init("源表", instr.smu_init(),
               confirm_prompt="当前源表驱动：KEITHLEY 2460，请确认与实际连接的设备一致")


def cleanup():
    """释放芯片侧资源（main.py 退出时调用）：关闭 IIC 串口。"""
    global _i2c
    if _i2c is not None:
        try:
            _i2c.close()
        except Exception:
            pass
        _i2c = None


def get_i2c():
    """返回已初始化的 IIC 控制器实例（未初始化返回 None）。"""
    return _i2c


def _init_i2c(port=None):
    """创建并打开 I2CController，返回实例（失败抛异常）。

    参照 IICtool.py 的设备初始化流程：打开串口、下发 I2C 初始化命令、设置 IIC 速率。

    参数:
        port: str - IIC 串口号（如 "COM3"）；None 则从配置读取
    返回:
        I2CController - 初始化完成的 IIC 控制器实例
    """
    global _i2c

    # IIC 设备初始化（端口从命令行传入，未指定时回退到配置）
    config = load_config()
    if not port:
        port = str(config.get("port", DEFAULT_CONFIG["port"])).strip()
    port = str(port).strip()
    if not port:
        raise ValueError("串口号不能为空（请通过 --port 传入，或配置 i2c_config.json）")

    slave_addr_str = str(config.get("slave_addr", DEFAULT_CONFIG["slave_addr"])).strip()
    slave_addr = int(slave_addr_str, 16) if slave_addr_str else 0x52
    iic_speed = str(config.get("iic_speed", DEFAULT_CONFIG["iic_speed"])).strip() or "100k"

    # 若已存在旧实例，先关闭释放
    if _i2c is not None:
        try:
            _i2c.close()
        except Exception:
            pass
        _i2c = None

    _i2c = I2CController(port, slave_addr)
    _i2c.open()
    _i2c.init_i2c()
    _i2c.set_iic_speed(iic_speed)

    # 保存配置，下次运行可复用
    try:
        save_config(port, f"0x{slave_addr:02X}", iic_speed=iic_speed)
    except Exception:
        pass

    print(f"✅ IIC 初始化完成：port={port}, speed={iic_speed}, slave=0x{slave_addr:02X}")
    return _i2c


def IIC_init_all(instr, port=None):
    """IIC 芯片全套初始化（独立调用入口，行为与 init_all 一致）

    参数:
        instr: instrument_set 模块引用
        port: str - IIC 串口号（如 "COM3"），从命令行传入；None 则从配置读取
    返回:
        I2CController - 初始化完成的 IIC 控制器实例
    """
    print(f"🔌 正在初始化 {CHIP_LABEL} (IIC) ...")
    i2c = _init_i2c(port)
    _init_instruments(instr)
    return i2c

