from crc import calculate_crc32, calculate_crc32_iter


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

    def erase_sector(self, sector):
        if not (0 <= sector <= 15):
             raise ValueError("Sector 必须在 0~15 之间")
        self.log(f"MTP: 擦除扇区 {sector}...")
        mtp_base_addr = sector << 5
        addr, index = self._addr_index_from_mtp(mtp_base_addr)
        self.write_i2c(addr, index, self._convert_32bit_to_bytes(0x800))

    def erase_chip(self):
        self.log("MTP: 下发全片擦除指令")
        self.write_i2c(0, 0, self._convert_32bit_to_bytes(0x1000))

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
