def calculate_crc32(data_bytes):
    """
    兼容接口：返回最终 CRC32（LSB 优先实现）。
    """
    crc, _ = calculate_crc32_iter(data_bytes)
    return crc

def calculate_crc32_iter(data_bytes, poly=0x04C11DB7, init=0xFFFFFFFF):
    """
    LSB 优先 CRC-32 计算，返回 (最终CRC, 逐字节中间结果列表)。
    每个中间结果项为 (byte_index_from_1, byte_hex_str, crc_after_this_byte_hex_str)
    """
    data = bytes(data_bytes)
    crc = init
    iter_results = []
    for idx, b in enumerate(data):
        for i in range(8):  # 处理从 bit0 到 bit7（LSB 优先）
            bit = (b >> i) & 1
            crc_msb = (crc >> 31) & 1
            a = crc_msb ^ bit
            crc = (crc << 1) & 0xFFFFFFFF
            if a:
                crc ^= poly
        iter_results.append((idx + 1, hex(b), hex(crc)))
    return crc & 0xFFFFFFFF, iter_results
