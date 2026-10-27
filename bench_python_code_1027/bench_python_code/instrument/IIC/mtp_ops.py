import time

from crc import calculate_crc32, calculate_crc32_iter

# ==================== 擦除指令与时序（依据《2361 芯片 MTP 操作测试指南》五、MTP 擦除操作）====================
# 全片擦除：向 5bit 地址 10000(=16) / INDEX 0 写 0x1000（apb_wdat[12]=1）产生片擦除脉冲，
#           触发后**强制等待 300 ms** 让硬件完成擦除（手册步骤 3）。
# 扇区擦除：向 00_<Sector>_00000 写 0x800（apb_wdat[11]=1）产生扇区擦除脉冲，随后等待约 100 ms。
CHIP_ERASE_ADDR = 16
CHIP_ERASE_DATA = 0x1000
CHIP_ERASE_WAIT = 0.3
SECTOR_ERASE_DATA = 0x800
SECTOR_ERASE_WAIT = 0.1


class MTPController:
    """
    2361 芯片 MTP (多次可编程存储器) 测试模式操作逻辑类。
    负责所有与 MTP 相关的底层地址映射、指令封装和时序控制，并与具体通信接口解耦。
    """
    def __init__(self, write_func, read_func, log_callback=None):
        self.write_i2c = write_func
        self.read_i2c = read_func
        self.log = log_callback if log_callback else (lambda msg: None)

    def _addr_index_from_mtp(self, mtp_byte_addr):
        addr = (mtp_byte_addr >> 6) & 0x1F
        index = mtp_byte_addr & 0x3F
        return addr, index

    def _convert_32bit_to_bytes(self, val_32):
        return [(val_32 >> 24) & 0xFF, (val_32 >> 16) & 0xFF, (val_32 >> 8) & 0xFF, val_32 & 0xFF]

    def open_mtp_en(self):
        addr = 30
        index = 0
        value = 0x100
        try:
            self.write_i2c(25, 4, self._convert_32bit_to_bytes(0x5a5a5a5a))
            self.write_i2c(20, 15, self._convert_32bit_to_bytes(0x5a5a5a5a))
            self.write_i2c(addr, index, self._convert_32bit_to_bytes(0x5a5a5a5a))
            self.log(f"MTP: Open MTP EN -> addr={addr}, index={index}, value=0x{value:08X}")
        except Exception as e:
            self.log(f"!! Open MTP EN 失败: {e}")

    def close_mtp_en(self):  
        addr = 30
        index = 0
        value = 0x000
        try:
            self.write_i2c(addr, index, self._convert_32bit_to_bytes(0xa5a500a5))
            self.write_i2c(20, 15, self._convert_32bit_to_bytes(0xa5a500a5))
            self.log(f"MTP: Close MTP EN -> addr={addr}, index={index}, value=0x{value:08X}")
        except Exception as e:
            self.log(f"!! Close MTP EN 失败: {e}")

    def erase_sector(self, sector, wait=True):
        """按扇区擦除。

        手册「五、MTP 擦除操作 / 2. 扇区擦除」：向扇区首地址写 0x800
        （apb_wdat[11]=1）产生产生扇区擦除脉冲，随后等待约 100 ms。

        参数：
            sector: int - 扇区号 0~15（每扇区 32 字节）
            wait: bool - True 下发后等待 SECTOR_ERASE_WAIT（默认），
                         False 立即返回（批量流程自己做状态轮询时用）
        """
        if not (0 <= sector <= 15):
             raise ValueError("Sector 必须在 0~15 之间")
        self.log(f"MTP: 擦除扇区 {sector}...")
        mtp_base_addr = sector << 5
        addr, index = self._addr_index_from_mtp(mtp_base_addr)
        self.write_i2c(addr, index, self._convert_32bit_to_bytes(SECTOR_ERASE_DATA))
        if wait:
            time.sleep(SECTOR_ERASE_WAIT)

    def erase_chip(self, wait=True):
        """全片擦除（清空全部 512 字节）。

        手册「五、MTP 擦除操作 / 1. 全片擦除」：
          · 触发：向 10000_000000（ADDR=16, INDEX=0）写 0x1000，即 apb_wdat[12]=1；
          · 完成：触发后**强制等待 300 ms**，硬件在此期间擦除整个 MTP。

        注意：手册未定义「全片擦除完成」的状态位，(31,0) 的状态寄存器
              只用于扇区擦除/编程/读触发命令的返回确认，不能拿来做片擦完成判定。

        参数：
            wait: bool - True 触发后等待 300 ms（默认）；False 只下发指令
        """
        self.log(f"MTP: 下发全片擦除指令 (ADDR={CHIP_ERASE_ADDR}, INDEX=0, "
                 f"data=0x{CHIP_ERASE_DATA:04X})")
        self.write_i2c(CHIP_ERASE_ADDR, 0,
                       self._convert_32bit_to_bytes(CHIP_ERASE_DATA))
        if wait:
            self.log(f"MTP: 强制等待 {CHIP_ERASE_WAIT * 1000:.0f} ms 完成全片擦除")
            time.sleep(CHIP_ERASE_WAIT)

    def wait_erase_ready(self, addr=31, index=0, poll_delay=0.5, max_retries=60,
                         log_callback=None):
        """轮询 (31,0) 状态寄存器 bit0，等待扇区擦除/编程完成。

        ⚠️ 仅适用于扇区擦除/编程这类「命令 + 状态位」时序，
           全片擦除请用 erase_chip() 的固定 300 ms 等待（手册未定义片擦状态位）。

        返回：
            bool - True=状态位已置位（完成）/ False=超时
        """
        for attempt in range(1, max_retries + 1):
            time.sleep(poll_delay)
            try:
                st = self.read_i2c(addr, index)
                if st and len(st) >= 4:
                    busy = st[3] & 0x01
                    if log_callback:
                        log_callback(f"MTP: 状态轮询第 {attempt} 次, "
                                     f"st[3]=0x{st[3]:02X}, bit0={busy}")
                    if busy:
                        if log_callback:
                            log_callback(f"MTP: 状态位就绪（共轮询 {attempt} 次）")
                        return True
                else:
                    if log_callback:
                        log_callback(f"MTP: 状态轮询第 {attempt} 次，读回无效数据: {st}")
            except Exception as e:
                if log_callback:
                    log_callback(f"MTP: 状态轮询异常: {e}")
        if log_callback:
            log_callback(f"!! MTP: 状态轮询超时（已等待 {max_retries * poll_delay:.1f} 秒）")
        return False

    def program_byte(self, mtp_byte_addr, byte_val):
        self.log(f"MTP: 烧录地址 {mtp_byte_addr} 数据 0x{byte_val:02X}")
        addr, index = self._addr_index_from_mtp(mtp_byte_addr)
        wdata = 0x400 | (byte_val & 0xFF)
        wdata_bytes = self._convert_32bit_to_bytes(wdata)
        for _ in range(2):
            self.write_i2c(addr, index, wdata_bytes)

    def read_byte(self, mtp_byte_addr):
        if not (0 <= int(mtp_byte_addr) <= 511):
            raise ValueError("MTP 字节地址必须在 0~511 之间")
        addr, index = self._addr_index_from_mtp(int(mtp_byte_addr))
        try:
            trig_addr = addr
            trig_index = index
            trig_val = 0x200
            try:
                payload = self._convert_32bit_to_bytes(trig_val)
                try:
                    self.write_i2c(trig_addr, trig_index, payload, True)
                except TypeError:
                    try:
                        self.write_i2c(trig_addr, trig_index, payload)
                    except Exception:
                        pass
                except Exception:
                    pass
            except Exception:
                pass

            return self.read_i2c(addr, index)
        except Exception as e:
            self.log(f"!! read_byte 读取失败: {e}")
            return None

    def calculate_crc32(self, data_bytes):
        return calculate_crc32(data_bytes)

    def calculate_crc32_iter(self, data_bytes, poly=0x04C11DB7, init=0xFFFFFFFF):
        return calculate_crc32_iter(data_bytes, poly=poly, init=init)
