# -*- coding: utf-8 -*-

def reverse_bits(x, width):
    r = 0
    for _ in range(width):
        r = (r << 1) | (x & 1)
        x >>= 1
    return r

def reverse_byte(b):
    b = ((b & 0xF0) >> 4) | ((b & 0x0F) << 4)
    b = ((b & 0xCC) >> 2) | ((b & 0x33) << 2)
    b = ((b & 0xAA) >> 1) | ((b & 0x55) << 1)
    return b

def reverse_bits_32(x):
    x = ((x & 0x55555555) << 1) | ((x & 0xAAAAAAAA) >> 1)
    x = ((x & 0x33333333) << 2) | ((x & 0xCCCCCCCC) >> 2)
    x = ((x & 0x0F0F0F0F) << 4) | ((x & 0xF0F0F0F0) >> 4)
    x = ((x & 0x00FF00FF) << 8) | ((x & 0xFF00FF00) >> 8)
    x = ((x & 0x0000FFFF) << 16) | ((x & 0xFFFF0000) >> 16)
    return x

def format_binary(value, width):
    bin_str = f"{value:0{width}b}"
    return ' '.join([bin_str[i:i+4] for i in range(0, len(bin_str), 4)])

def build_spi_payload(rw, addr, index, write_value=0x00000000):
    """
    构造SPI读写负载
    rw: 'read' 或 'write'
    addr: 0~31
    index: 0~63
    write_value: 32位整数，仅在write时有效
    """
    if not (0 <= addr <= 31):
        raise ValueError("addr 必须是 0~31")
    if not (0 <= index <= 63):
        raise ValueError("index 必须是 0~63")

    if rw == 'read':
        rwcode = 0b110
        recv_len = 4
        is_write = False
    elif rw == 'write':
        rwcode = 0b010
        recv_len = 0
        is_write = True
    else:
        raise ValueError("rw 必须是 'read' 或 'write'")

    # 计算逻辑 header
    addr_rev = reverse_bits(addr, 5)
    index_rev = reverse_bits(index, 6)
    header_logical = (rwcode << 13) | (addr_rev << 8) | (index_rev << 2)

    high_byte = (header_logical >> 8) & 0xFF
    low_byte = header_logical & 0xFF
    payload_bytes = [high_byte, low_byte]

    if is_write:
        physical_value = reverse_bits_32(write_value)
        data_bytes = [
            (physical_value >> 24) & 0xFF,
            (physical_value >> 16) & 0xFF,
            (physical_value >> 8) & 0xFF,
            physical_value & 0xFF
        ]
        payload_bytes.extend(data_bytes)
        send_len = 6
    else:
        send_len = 2

    # 组装完整命令
    cmd = bytearray([
        0x07, 0x00, 0x0F,
        0x00, 0x00, 0x01, 0x00, 0x01,
        0x01, 0x00
    ])
    cmd.extend([(send_len >> 8) & 0xFF, send_len & 0xFF])
    cmd.extend([(recv_len >> 8) & 0xFF, recv_len & 0xFF])
    cmd.extend(payload_bytes)
    
    return {
        'cmd': cmd,
        'logical_header': header_logical,
        'is_write': is_write,
        'send_len': send_len,
        'recv_len': recv_len,
        'payload_hex': ' '.join(f"{b:02X}" for b in payload_bytes),
        'cmd_hex': ' '.join(f"{b:02X}" for b in cmd)
    }

if __name__ == '__main__':
    print("="*40)
    print("      SPI 数据构造演示")
    print("="*40)
    
    # 模拟 READ 构造
    addr_r, index_r = 19, 0
    print(f"\n[测试 READ] addr: {addr_r}, index: {index_r}")
    res_read = build_spi_payload('read', addr_r, index_r)
    print(f"逻辑 Header (Binary) : {format_binary(res_read['logical_header'], 16)}")
    print(f"负载 Payload (HEX)   : {res_read['payload_hex']}")
    print(f"完整命令字节 (HEX)   : {res_read['cmd_hex']}")
    
    # 模拟 WRITE 构造
    addr_w, index_w, val_w = 19, 0, 0xAABBCCDD
    print(f"\n[测试 WRITE] addr: {addr_w}, index: {index_w}, value: 0x{val_w:08X}")
    res_write = build_spi_payload('write', addr_w, index_w, val_w)
    print(f"逻辑 Header (Binary) : {format_binary(res_write['logical_header'], 16)}")
    print(f"负载 Payload (HEX)   : {res_write['payload_hex']}")
    print(f"完整命令字节 (HEX)   : {res_write['cmd_hex']}")
    print("\n")