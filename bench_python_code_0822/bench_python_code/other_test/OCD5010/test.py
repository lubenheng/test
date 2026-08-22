import sys
import time
from pathlib import Path

# 确保 project root 在 sys.path 中（独立运行时需要）
_project_root = str(Path(__file__).parents[2])
if _project_root not in sys.path:
    sys.path.insert(0, _project_root)

from instrument.debug import SWD_CMD



if __name__ == "__main__":
    SWD_CMD.init_all()
    print(f"swd_addr: {hex(SWD_CMD.read_register_32bit(0x40007820))}")
    time.sleep(1)
    print("写入 0x12345678 到 0x40007820")
    SWD_CMD.write_register(0x40007820, 0x12345678)
    time.sleep(1)
    print(f"swd_addr: {hex(SWD_CMD.read_register_32bit(0x40007820))}")
    