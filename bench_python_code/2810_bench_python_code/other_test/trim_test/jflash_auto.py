import os
import sys
import subprocess

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))

JLINK_DIR = os.path.join(SCRIPT_DIR, "JLink_V634h")
JLINK_EXE = os.path.join(JLINK_DIR, "JLink.exe")

CHIP_NAME   = "DRV3_INFO"
JLINK_SPEED = 1000 
JLINK_IF    = "SWD"

READ_BACK_BIN = os.path.join(SCRIPT_DIR, "read_back_flash.bin")
MODIFIED_BIN  = os.path.join(SCRIPT_DIR, "modified_flash.bin")

BASE_ADDR   = 0x03000000
SECTOR_SIZE = 0x100

MODIFY_TASKS = [
    {
        "offset": 0xFC, 
        "bit_start": 0, 
        "bit_len": 6,  
        "value": 30    
    },
]

def _run_jlink_script(script_lines, timeout=120):
    """生成临时脚本文件并用 JLink.exe 执行

    Args:
        script_lines: JLink Commander 命令列表（不含 device/speed/connect/exit）
    Returns:
        (returncode, stdout_text)
    """
    if not os.path.exists(JLINK_EXE):
        print(f"❌ JLink.exe 不存在: {JLINK_EXE}")
        sys.exit(1)

    lines = [
        f"device {CHIP_NAME}",
        f"si {JLINK_IF}",
        f"speed {JLINK_SPEED}",
        "connect",
    ]
    lines.extend(script_lines)
    lines.append("exit")

    script_path = os.path.join(SCRIPT_DIR, "_jlink_script.tmp")
    with open(script_path, "w", newline="\r\n") as f:
        f.write("\n".join(lines))

    cmd = [JLINK_EXE, "-CommanderScript", script_path]
    try:
        result = subprocess.run(
            cmd,
            cwd=JLINK_DIR,
            capture_output=True,
            text=True,
            timeout=timeout,
        )
        return result.returncode, result.stdout
    finally:
        # 清理临时文件
        try:
            os.remove(script_path)
        except OSError:
            pass


def jlink_program(filepath, addr, label=""):
    """通过 JLink Commander 烧录文件到 Flash（自动擦除+编程+校验）"""
    if not os.path.exists(filepath):
        print(f"❌ 文件不存在: {filepath}")
        sys.exit(1)

    tag = f"({label}) " if label else ""
    addr_str = f"0x{addr:08X}"
    print(f"  JLink {tag}烧录: {os.path.basename(filepath)} ({os.path.getsize(filepath)} 字节) → {addr_str}")

    rc, output = _run_jlink_script([
        f"loadbin {filepath}, {addr_str}",
    ])

    # 提取关键日志行
    for line in output.splitlines():
        line_stripped = line.strip()
        if any(kw in line_stripped for kw in
               ("Flash download:", "O.K.", "Failed", "error", "Error", "failed")):
            print(f"    {line_stripped}")

    if rc == 0:
        print(f"  ✅ JLink 烧录完成")
        return True
    else:
        print(f"  ❌ JLink 烧录失败 (返回码: {rc})")
        return False


def jlink_read_flash(start_addr, size, filename):
    """通过 JLink Commander 回读 Flash 内容到二进制文件"""
    addr_str = f"0x{start_addr:08X}"
    size_str = f"0x{size:X}"
    print(f"  JLink 回读 Flash: {addr_str} ({size} 字节)...")

    rc, output = _run_jlink_script([
        f"savebin {filename}, {addr_str}, {size_str}",
    ])

    for line in output.splitlines():
        line_stripped = line.strip()
        if any(kw in line_stripped for kw in
               ("Reading", "Writing", "O.K.", "Failed", "Error")):
            print(f"    {line_stripped}")

    if rc == 0 and os.path.exists(filename):
        print(f"  ✅ 回读完成: {filename} ({os.path.getsize(filename)} 字节)")
        return True
    else:
        print(f"  ❌ 回读失败 (返回码: {rc})")
        return False


# ---- 数据生成与位修改逻辑 ----
def update_bits(value, start_bit, bit_len, new_value):
    """修改单字节的指定位段，不改变其他位"""
    mask = ((1 << bit_len) - 1) << start_bit
    value_cleared = value & ~mask
    new_value_shifted = (new_value & ((1 << bit_len) - 1)) << start_bit
    return value_cleared | new_value_shifted


def build_flash_data(modify_tasks, target_size=256):
    """创建全零数据 -> 批量按位修改 -> 返回 bytearray"""
    data = bytearray(target_size)  # 全部初始化为 0x00

    for idx, task in enumerate(modify_tasks):
        offset = task["offset"]
        bit_start = task["bit_start"]
        bit_len = task["bit_len"]
        value = task["value"]

        byte_offset = offset + bit_start // 8
        bit_in_byte = bit_start % 8
        end_bit = bit_in_byte + bit_len

        if byte_offset < 0 or byte_offset >= target_size:
            raise ValueError(f"任务{idx}：字节偏移0x{byte_offset:X}超出范围（0~255）")
        if value < 0 or value >= (1 << bit_len):
            raise ValueError(f"任务{idx}：数值{value}超出{bit_len}位范围（0~{(1 << bit_len)-1}）")

        if end_bit <= 8:
            original_byte = data[byte_offset]
            new_byte = update_bits(original_byte, bit_in_byte, bit_len, value)
            data[byte_offset] = new_byte
            print(f"  ✅ 任务{idx+1}：偏移0x{offset:X} bit{bit_start}~{bit_start+bit_len-1} "
                  f"(字节0x{byte_offset:X} bit{bit_in_byte}~{end_bit-1}) "
                  f"原始值0x{original_byte:02X} -> 新值0x{new_byte:02X} "
                  f"(写入十进制{value})")
        else:
            num_bytes = (end_bit + 7) // 8
            if byte_offset + num_bytes > target_size:
                raise ValueError(f"任务{idx}：位范围超出文件范围")

            current_val = 0
            for i in range(num_bytes):
                current_val |= data[byte_offset + i] << (i * 8)

            mask = ((1 << bit_len) - 1) << bit_in_byte
            current_val = (current_val & ~mask) | ((value & ((1 << bit_len) - 1)) << bit_in_byte)

            for i in range(num_bytes):
                data[byte_offset + i] = (current_val >> (i * 8)) & 0xFF

            print(f"  ✅ 任务{idx+1}：偏移0x{offset:X} bit{bit_start}~{bit_start+bit_len-1} "
                  f"(跨{num_bytes}字节) 写入十进制{value}")

    return data


def modify_bin_file(source_bin, target_bin, modify_tasks, target_size=256):
    """从回读文件读取数据 -> 批量按位修改 -> 保存为新文件"""
    with open(source_bin, "rb") as f:
        raw_data = f.read()

    if len(raw_data) < target_size:
        raw_data = raw_data.ljust(target_size, b'\x00')
    elif len(raw_data) > target_size:
        raw_data = raw_data[:target_size]

    modified_data = bytearray(raw_data)

    for idx, task in enumerate(modify_tasks):
        offset = task["offset"]
        bit_start = task["bit_start"]   # 全局位偏移: 从 offset 字节的 bit0 开始算
        bit_len = task["bit_len"]
        value = task["value"]

        # 计算实际操作的字节位置和字节内偏移
        byte_offset = offset + bit_start // 8
        bit_in_byte = bit_start % 8
        end_bit = bit_in_byte + bit_len  # 在起始字节中占用的位数

        if byte_offset < 0 or byte_offset >= target_size:
            raise ValueError(f"任务{idx}：字节偏移0x{byte_offset:X}超出范围（0~255）")
        if value < 0 or value >= (1 << bit_len):
            raise ValueError(f"任务{idx}：数值{value}超出{bit_len}位范围（0~{(1 << bit_len)-1}）")

        if end_bit <= 8:
            # 单字节内修改
            original_byte = modified_data[byte_offset]
            new_byte = update_bits(original_byte, bit_in_byte, bit_len, value)
            modified_data[byte_offset] = new_byte
            print(f"  ✅ 任务{idx+1}完成：偏移0x{offset:X} "
                  f"bit{bit_start}~bit{bit_start+bit_len-1} "
                  f"(字节0x{byte_offset:X} bit{bit_in_byte}~{end_bit-1}) "
                  f"原始值0x{original_byte:02X} -> 新值0x{new_byte:02X} "
                  f"(写入十进制{value})")
        else:
            # 跨字节修改: 按小端序读取多字节整数
            num_bytes = (end_bit + 7) // 8
            if byte_offset + num_bytes > target_size:
                raise ValueError(f"任务{idx}：位范围{bit_start}~{bit_start+bit_len-1} "
                                 f"需要{num_bytes}字节，超出文件范围")

            # 读取当前值（小端序）
            current_val = 0
            for i in range(num_bytes):
                current_val |= modified_data[byte_offset + i] << (i * 8)

            # 修改指定位段
            mask = ((1 << bit_len) - 1) << bit_in_byte
            current_val = (current_val & ~mask) | ((value & ((1 << bit_len) - 1)) << bit_in_byte)

            # 写回（小端序）
            for i in range(num_bytes):
                modified_data[byte_offset + i] = (current_val >> (i * 8)) & 0xFF

            print(f"  ✅ 任务{idx+1}完成：偏移0x{offset:X} "
                  f"bit{bit_start}~bit{bit_start+bit_len-1} "
                  f"(跨{num_bytes}字节，小端序) "
                  f"写入十进制{value})")

    with open(target_bin, "wb") as f:
        f.write(modified_data)

    print(f"\n  保存完成：{source_bin} -> {target_bin} ({os.path.getsize(target_bin)} 字节)")


# ===================== 自动化主流程 =====================
def run_jflash_program(modify_tasks=None):
    """执行 JFlash 自动化主流程：生成数据 -> 擦除写入 -> 清理临时文件

    Args:
        modify_tasks: 可选，位修改任务列表。不传则使用模块级 MODIFY_TASKS。
    Returns:
        True 表示成功，False 表示失败
    """
    if modify_tasks is None:
        modify_tasks = MODIFY_TASKS

    print("=" * 58)
    print("  JLink 自动化 — Info区Flash擦除/回读/修改/写入")
    print("=" * 58)
    print(f"  器件:       {CHIP_NAME}")
    print(f"  JLink:      {JLINK_EXE}")
    print(f"  Info区:     {hex(BASE_ADDR)}  size={hex(SECTOR_SIZE)}")
    print(f"  修改任务数: {len(modify_tasks)}")
    print(f"  数据源:     Python 生成全零 -> 按位修改")
    print(f"  烧录引擎:   JLink Commander + DRV3_INFO.elf Flash算法")

    # ------ 步骤1：生成数据 ------
    print("\n" + "─" * 58)
    print("[1/3] 生成 Flash 数据（全零 -> 按位修改）")
    print("─" * 58)
    flash_data = build_flash_data(modify_tasks)
    with open(MODIFIED_BIN, "wb") as f:
        f.write(flash_data)
    print(f"\n  数据文件: {os.path.basename(MODIFIED_BIN)} ({len(flash_data)} 字节)")

    # ------ 步骤2：擦除并写入 Flash ------
    print("\n" + "─" * 58)
    print("[2/3] 擦除并写入 Flash（自动擦除+编程+校验）")
    print("─" * 58)
    if not jlink_program(MODIFIED_BIN, BASE_ADDR, "数据"):
        return False

    # ========== 回读验证暂屏蔽 ==========
    # 如需回读验证，取消下方注释
    #
    # # 步骤3：回读
    # if not jlink_read_flash(BASE_ADDR, SECTOR_SIZE, READ_BACK_BIN):
    #     sys.exit(1)

    # 清理临时文件
    print("\n" + "─" * 58)
    print("[3/3] 清理临时文件")
    print("─" * 58)
    for tmp_file in [READ_BACK_BIN, MODIFIED_BIN]:
        if os.path.exists(tmp_file):
            os.remove(tmp_file)
            print(f"  删除: {os.path.basename(tmp_file)}")

    print("\n" + "=" * 58)
    print("  ✅ 全部完成！修改数据已写入Info区Flash，掉电不丢失。")
    print("=" * 58)
    return True


if __name__ == "__main__":
    run_jflash_program()
