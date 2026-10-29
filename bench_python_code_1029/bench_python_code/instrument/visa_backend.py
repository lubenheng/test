# -*- coding: utf-8 -*-
"""
VISA ResourceManager 自动探测辅助函数。

背景: pyvisa.ResourceManager() 默认会查找系统安装的 NI-VISA / Keysight IO Libraries 等商业驱动；
      没装这些驱动的电脑上会直接抛 "Could not locate a VISA implementation"。
      而强制写死 ResourceManager('@py')(纯 Python pyvisa-py 后端, 不需要装任何驱动) 又会导致
      原本靠厂商驱动正常识别设备的电脑, 因为换了后端走 libusb 而认不到设备(USB 接口驱动绑定不同)。

      单纯"默认后端枚举到任意资源就用它"是不够的: 默认后端可能枚举到了别的资源(比如串口),
      但唯独枚举不到目标仪器, 这种情况仍需换 '@py' 后端重新尝试。所以必须以"能否匹配到目标设备"
      为准, 而不是"是否枚举到任意资源"。
"""
import pyvisa


def find_resource(match_fn):
    """
    依次用默认后端(走系统已装的 NI-VISA / IVI 等驱动)和 '@py' 纯 Python 后端尝试查找目标设备,
    以 match_fn(rm, resource_list) -> 匹配到的 resource 字符串或 None 为准进行判断。

    参数:
        match_fn: callable(rm, resources) -> str|None
                  resources 为 list_resources() 结果(list[str]); 传入 rm 是为了支持需要实际
                  open_resource() 并查询 *IDN? 才能确认型号的场景(如信号发生器按 IDN 匹配)。
                  找到目标设备返回资源字符串, 找不到返回 None

    返回:
        (rm, resource_str) - 成功匹配到设备的 ResourceManager 实例和资源地址
        (rm, None)          - 两种后端都尝试过仍未匹配到设备时, 返回最后一次尝试的 rm(供上层按需 fallback)
    """
    last_rm = None
    for backend in (None, '@py'):
        try:
            rm = pyvisa.ResourceManager(backend) if backend else pyvisa.ResourceManager()
        except Exception:
            continue
        last_rm = rm
        try:
            resources = rm.list_resources()
        except Exception:
            resources = ()
        matched = match_fn(rm, resources) if resources else None
        if matched:
            return rm, matched
        # 注意：这里**不能** rm.close()。
        # pyvisa 的 ResourceManager 是「每个 VISA 库一个实例」的单例
        # （ResourceManager.__new__ 会复用 visa_library.resource_manager），
        # 而 ResourceManager.close() 会把它创建过的所有 Resource 一起关闭。
        # 因此这里关掉一个「没匹配到设备」的 RM，会把其它仪器（万用表/示波器/源表）
        # 已经打开的会话一起关掉，之后访问它们就会报
        # "Invalid session handle. The resource might be closed."。
        # 让它保持打开即可：进程退出时 pyvisa 注册的 atexit 会自动关闭。

    return last_rm, None


def get_resource_manager():
    """
    返回一个可用的 pyvisa.ResourceManager, 不关心具体设备(仅需要一个能工作的后端时使用):
      1. 优先尝试默认后端(会使用系统安装的 NI-VISA / IVI 等驱动)
      2. 若默认后端初始化失败, 或初始化成功但枚举不到任何资源, 则回退到纯 Python 的 '@py' 后端
    """
    try:
        rm = pyvisa.ResourceManager()
        if rm.list_resources():
            return rm
        # 不调用 rm.close()：ResourceManager 是每个 VISA 库的单例，
        # 关闭它会连带关闭本进程里其它仪器已打开的会话（见 find_resource 的说明）。
    except Exception:
        pass

    return pyvisa.ResourceManager('@py')
