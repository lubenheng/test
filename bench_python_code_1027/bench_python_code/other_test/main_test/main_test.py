"""
OCD2810 芯片 SWD 初始化主程序

仅初始化 SWD 连接 + 芯片解密，不涉及串口/万用表/示波器。
"""

import sys
import os
from pathlib import Path

# ── 将项目根目录加入 sys.path ──
_project_root = str(Path(__file__).resolve().parent.parent.parent)
if _project_root not in sys.path:
    sys.path.insert(0, _project_root)

# ── 引入 instrument 封装层 ──
from instrument import instrument_set as instr


def main():

    # ── SWD + 芯片解密 ──
    print("\n正在初始化 SWD 并执行芯片解密...")
    try:
        from chips.ocd2810.chip import init_swd
        ok = init_swd(instr)
    except Exception as e:
        print(f"❌ SWD 初始化异常: {e}")
        ok = False

    if ok:
        print("✅ SWD 初始化成功，芯片已解密")
    else:
        print("❌ SWD 初始化失败")

    instr.swd_modify_register(0x4000141C, 0x1, 11, 11)
    instr.swd_modify_register(0x40005008, 0x07, 16, 18)
    instr.swd_modify_register(0x40005000, 0x1, 15, 15)

    print("👋 退出")


if __name__ == "__main__":
    main()
