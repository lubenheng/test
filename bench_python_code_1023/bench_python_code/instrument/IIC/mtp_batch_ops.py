import os
import time
from openpyxl import Workbook, load_workbook
from openpyxl.styles import Font
from iic_ops import IICLogic
from crc import calculate_crc32, calculate_crc32_iter


class MTPBatchLogic:
    """
    负责 MTP 的相关批量 Excel 导出生成、全片烧录、批量执行等耗时业务逻辑。
    所有方法均为静态方法，通过回调与 GUI 及硬件层解耦。
    """

    @staticmethod
    def mtp_full_program_with_crc(filename, execute_write_cb, execute_read_cb, log_cb, progress_cb, msg_cb):
        try:
            wb = load_workbook(filename, data_only=True)
            ws = wb.active
            expected_format = ["ADDR", "INDEX", "[31:24]", "[23:16]", "[15:8]", "[7:0]"]
            headers = [str(ws.cell(row=1, column=c).value).strip().upper() if ws.cell(row=1, column=c).value else "" for c in range(1, 7)]
            if headers != expected_format:
                return False, f"表头不匹配。预期: {expected_format}\n实际: {headers}"

            records = []
            for r in range(2, ws.max_row + 1):
                addr_v = ws.cell(row=r, column=1).value
                if addr_v is None or str(addr_v).strip() == "":
                    continue
                try:
                    addr = int(addr_v)
                    index = int(ws.cell(row=r, column=2).value)
                    b31 = IICLogic.parse_hex_byte(ws.cell(row=r, column=3).value, "[31:24]")
                    b23 = IICLogic.parse_hex_byte(ws.cell(row=r, column=4).value, "[23:16]")
                    b15 = IICLogic.parse_hex_byte(ws.cell(row=r, column=5).value, "[15:8]")
                    b7 = IICLogic.parse_hex_byte(ws.cell(row=r, column=6).value, "[7:0]")
                    records.append((addr, index, [b31, b23, b15, b7]))
                except Exception as e:
                    return False, f"第 {r} 行解析错误: {e}"

            if not records:
                return False, "文件中没有有效的数据记录"

            addr_groups = {}
            for addr, index, data in records:
                if addr not in addr_groups:
                    addr_groups[addr] = {}
                addr_groups[addr][index] = data

            progress_cb(0, len(addr_groups)*10, "验证成功，准备烧录...")

            def mtp_retry_exec(cmd_name, cmd_seq, delay, exp_v, exp_m, max_r=5):
                for _ in range(max_r):
                    for iw in cmd_seq:
                        execute_write_cb(iw["addr"], iw["index"], iw["data"])
                    time.sleep(delay)
                    st = execute_read_cb(31, 0)
                    if st is None: continue
                    if (st[3] & exp_m) == exp_v:
                        return st
                return None

            total_sectors = len(addr_groups)
            current_sector = 0

            for addr, data_dict in sorted(addr_groups.items()):
                log_cb(f"开始处理 Sector ADDR={addr} ...")
                indices = list(data_dict.keys())
                min_idx = min(indices)
                max_idx = max(indices)

                cmd_erase = [{"addr": addr, "index": 0, "data": [0,0,8,0]}]
                st = mtp_retry_exec("ERASE", cmd_erase, 0.5, 0x01, 0x01, 5)
                if not st:
                    return False, f"ADDR={addr} ERASE (擦除) 失败，状态寄存器超时"
                log_cb(f"ADDR={addr} ERASE 成功，状态: {[hex(x) for x in st]}")
                current_sector += 1
                progress_cb(current_sector*10 - 7, total_sectors*10, f"擦除 ADDR={addr} 成功")

                for idx in range(min_idx, max_idx + 1):
                    if idx in data_dict:
                        val = data_dict[idx]
                        execute_write_cb(addr, idx, val)
                        time.sleep(0.01)
                log_cb(f"ADDR={addr} 内部 SRAM 填充完成 (Index {min_idx}~{max_idx})")
                current_sector += 1
                progress_cb(current_sector*10 - 4, total_sectors*10, f"填充 SRAM ADDR={addr} 完成")

                cmd_write = [{"addr": addr, "index": 0, "data": [0,0,4,0]}]
                st_wr = mtp_retry_exec("WRITE", cmd_write, 0.5, 0x01, 0x01, 5)
                if not st_wr:
                    return False, f"ADDR={addr} WRITE (写指令) 失败，状态寄存器超时"
                log_cb(f"ADDR={addr} WRITE 成功，状态: {[hex(x) for x in st_wr]}")
                current_sector += 1
                progress_cb(current_sector*10 - 2, total_sectors*10, f"烧录 ADDR={addr} 成功")

                cmd_read = [{"addr": addr, "index": 0, "data": [0,0,2,0]}]
                st_rd = mtp_retry_exec("READ", cmd_read, 0.05, 0x00, 0x01, 5)
                if not st_rd:
                    return False, f"ADDR={addr} READ (回读准备) 失败，状态寄存器不符合预期"

                readback_data = {}
                for idx in range(min_idx, max_idx + 1):
                    if idx in data_dict:
                        rd_val = execute_read_cb(addr, idx)
                        if rd_val:
                            readback_data[idx] = rd_val
                        else:
                            return False, f"回读 ADDR={addr} INDEX={idx} 失败"

                log_cb(f"ADDR={addr} 烧录完成")

                current_sector += 1
                progress_cb(current_sector*10, total_sectors*10, f"校验 ADDR={addr} 完成")

            return True, "全流程烧录与校验成功！"
        except Exception as e:
            return False, f"烧录流程异常: {str(e)}"

    @staticmethod
    def generate_mtp_template(filename):
        try:
            wb = Workbook()
            ws = wb.active
            ws.title = "MTP批量模板"
            headers = ["OPERATION", "ADDR(基地址)", "INDEX(Word偏移)", "[31:24]", "[23:16]", "[15:8]", "[7:0]", "物理地址(计算)"]
            ws.append(headers)
            note_row = [
                "READ/PROGRAM/ERASE_SECTOR",
                "如 0~511 (MTP基地址)",
                "Word偏移(1代表偏4字节)",
                "对应物理地址 ADDR+INDEX*4 +3 (HEX)",
                "对应物理地址 ADDR+INDEX*4 +2 (HEX)",
                "对应物理地址 ADDR+INDEX*4 +1 (HEX)",
                "对应物理地址 ADDR+INDEX*4 +0 (HEX)",
                "自动公式: =B列+C列*4"
            ]
            ws.append(note_row)
            current_row = 3
            for i in range(1, 79):
                ws.append(["PROGRAM", 0, i, "00", "00", "00", "00", f"=B{current_row}+C{current_row}*4"])
                current_row += 1
            header_font = Font(bold=True)
            for col in range(1, len(headers) + 1):
                ws.cell(row=1, column=col).font = header_font
            for col in ws.columns:
                max_length = 0
                column = col[0].column_letter
                for cell in col:
                    try:
                        if len(str(cell.value)) > max_length:
                            max_length = len(str(cell.value))
                    except:
                        pass
                ws.column_dimensions[column].width = min(max_length + 2, 25)
            wb.save(filename)
            return True, None
        except Exception as e:
            return False, str(e)

    @staticmethod
    def mtp_full_program_with_crc_gui(filename, mtp_ctrl, execute_write_cb, execute_read_cb,
                                       parse_hex_byte_cb, log_cb):
        """从 Excel 读取 MTP 数据，计算 CRC32，构建 512 字节 buffer。

        CRC 口径：
          · 算法：crc.py 的 calculate_crc32_iter（LSB 优先）
          · 范围：模板中 PROGRAM 声明且 phys < 416 的字节，按地址升序拼接
          · phys >= 416 的声明行仍会返回在烧录清单中（照常烧录），但不参与 CRC

        返回: (success, buffer, burn_addrs, final_crc, error_msg)
              burn_addrs = 模板声明的全部字节地址（含 >=416），供烧录与校验使用
        """
        try:
            wb = load_workbook(filename, data_only=True)
            if len(wb.sheetnames) > 1 and "MTP批量模板" in wb.sheetnames:
                ws = wb["MTP批量模板"]
            else:
                ws = wb.active
            rows = list(ws.iter_rows(min_row=3, values_only=True))
            buffer = bytearray(512)
            phys_addrs_set = set()
            for idx, row in enumerate(rows, start=3):
                if all(cell is None or str(cell).strip() == "" for cell in row[:3]):
                    break
                op_str = str(row[0]).strip().upper() if row[0] else ""
                if op_str == "PROGRAM":
                    addr_mod = int(row[1]) if row[1] is not None else 0
                    index_mod = int(row[2]) if row[2] is not None else 0
                    phys_addr = addr_mod + (index_mod * 4)
                    if phys_addr < 0 or phys_addr > 511:
                        continue
                    b31 = parse_hex_byte_cb(row[3], "[31:24]")
                    b23 = parse_hex_byte_cb(row[4], "[23:16]")
                    b15 = parse_hex_byte_cb(row[5], "[15:8]")
                    b7 = parse_hex_byte_cb(row[6], "[7:0]")
                    buffer[phys_addr] = b7
                    buffer[phys_addr + 1] = b15
                    buffer[phys_addr + 2] = b23
                    buffer[phys_addr + 3] = b31
                    phys_addrs_set.update([phys_addr, phys_addr + 1, phys_addr + 2, phys_addr + 3])

            # 烧录清单：模板声明的全部字节（含 >=416）
            burn_addrs = sorted(x for x in phys_addrs_set if 0 <= x <= 511)
            # CRC 输入：声明字节中 phys < 416 的部分，按地址升序
            crc_addrs = [x for x in burn_addrs if x < 416]
            if crc_addrs:
                final_crc, _ = calculate_crc32_iter(bytes(buffer[x] for x in crc_addrs))
            else:
                final_crc = 0xFFFFFFFF
            buffer[507] = final_crc & 0xFF
            buffer[508] = (final_crc >> 8) & 0xFF
            buffer[509] = (final_crc >> 16) & 0xFF
            buffer[510] = (final_crc >> 24) & 0xFF

            return True, buffer, burn_addrs, final_crc, None
        except Exception as e:
            return False, None, None, None, str(e)

    @staticmethod
    def execute_mtp_burn(mtp_ctrl, buffer, phys_addrs_sorted, log_cb, progress_cb=None):
        """执行两遍全片烧录（含 CRC 区域 507~510）。

        返回: None（烧录过程中通过 log_cb 输出状态）
        """
        total_bytes = (len(phys_addrs_sorted) + 4) * 2
        current_byte = 0
        for pass_idx in range(2):
            log_cb(f"*** 开始第 {pass_idx+1} 遍全片烧录 (共 {len(phys_addrs_sorted)} 字节) ***")
            for offset in phys_addrs_sorted:
                mtp_ctrl.program_byte(offset, buffer[offset])
                current_byte += 1
                if progress_cb and current_byte % 10 == 0:
                    progress_cb(current_byte, total_bytes)
            for offset in range(507, 511):
                mtp_ctrl.program_byte(offset, buffer[offset])
                current_byte += 1
                if progress_cb:
                    progress_cb(current_byte, total_bytes)

    @staticmethod
    def verify_mtp_burn(mtp_ctrl, buffer, phys_addrs_sorted, log_cb, progress_cb=None):
        """读回已写入的字节（含 CRC 区域）并比对。

        返回: list of (offset, expected, actual) 不一致项
        """
        verify_offsets = list(phys_addrs_sorted) if phys_addrs_sorted else []
        for off in range(507, 511):
            if off not in verify_offsets:
                verify_offsets.append(off)

        mismatches = []
        for i, off in enumerate(verify_offsets, start=1):
            try:
                rv = mtp_ctrl.read_byte(off)
                read_val = rv[3] if (rv and isinstance(rv, (list, tuple)) and len(rv) >= 4) else None
            except Exception as re_err:
                read_val = None
                log_cb(f"!! 读回失败: addr={off}, err={re_err}")
            expected = buffer[off]
            if read_val is None or read_val != expected:
                mismatches.append((off, expected, read_val))
                rv_text = 'None' if read_val is None else f'0x{read_val:02X}'
                log_cb(f"!! 校验失败: addr={off}, 期望=0x{expected:02X}, 读回={rv_text}")
            else:
                log_cb(f"校验通过: addr={off} = 0x{expected:02X}")
            if progress_cb:
                progress_cb(i, len(verify_offsets))
        return mismatches

    @staticmethod
    def import_and_execute_mtp_batch(filename, mtp_ctrl, log_cb, progress_cb=None):
        """导入 MTP 批量操作 Excel，解析 START/END 间的操作行并执行。

        返回: (success, total_count, success_count, error_msg)
        """
        try:
            wb = load_workbook(filename, data_only=True)
            ws = wb.active
            rows = list(ws.iter_rows(min_row=3, values_only=True))
            if not rows:
                return False, 0, 0, "Excel 中无有效数据行（从第3行开始）"

            start_row = None
            end_row = None
            for i, row in enumerate(rows, start=3):
                first = str(row[0]).strip() if row[0] is not None else ""
                if first.upper() == 'START':
                    start_row = i + 1
                    break
            if start_row is None:
                start_row = 3
            for i in range(start_row, len(rows) + 3):
                r = rows[i - 3]
                first = str(r[0]).strip() if r[0] is not None else ""
                if first.upper() == 'END':
                    end_row = i - 1
                    break
            if end_row is None:
                end_row = len(rows) + 2

            success_count = 0
            total_count = 0
            valid_rows = []
            for i in range(start_row, end_row + 1):
                row = rows[i - 3]
                first = str(row[0]).strip() if row[0] is not None else ""
                if first == "" or first.startswith('#') or first.startswith('//') or first.startswith('注'):
                    continue
                valid_rows.append((i, row))

            for task_idx, (excel_row, row) in enumerate(valid_rows, start=1):
                total_count += 1
                if progress_cb:
                    progress_cb(task_idx, len(valid_rows))
                try:
                    op_str = str(row[0]).strip().upper() if row[0] else ""
                    if op_str not in ["READ", "PROGRAM", "ERASE_SECTOR"]:
                        raise ValueError(f"操作类型必须是 READ / PROGRAM / ERASE_SECTOR，当前值: '{op_str}'")
                    addr_mod = int(row[1]) if row[1] is not None else 0
                    index_mod = int(row[2]) if row[2] is not None else 0
                    phys_addr = addr_mod + (index_mod * 4)
                    if not (0 <= phys_addr <= 511):
                        raise ValueError(f"物理地址必须在 0~511，当前: {phys_addr}")
                    log_cb(f"--- MTP批量第 {total_count} 条 (Excel 第 {excel_row} 行，物理地址 {phys_addr}) ---")
                    if op_str == "ERASE_SECTOR":
                        sector = phys_addr >> 5
                        mtp_ctrl.erase_sector(sector)
                        success_count += 1
                    elif op_str == "READ":
                        v0 = mtp_ctrl.read_byte(phys_addr)
                        v1 = mtp_ctrl.read_byte(phys_addr + 1)
                        v2 = mtp_ctrl.read_byte(phys_addr + 2)
                        v3 = mtp_ctrl.read_byte(phys_addr + 3)
                        b0 = v0[3] if v0 else 0
                        b1 = v1[3] if v1 else 0
                        b2 = v2[3] if v2 else 0
                        b3 = v3[3] if v3 else 0
                        log_cb(f"MTP 读取 4 bytes @物理地址{phys_addr}: [{b3:02X} {b2:02X} {b1:02X} {b0:02X}]")
                        success_count += 1
                    elif op_str == "PROGRAM":
                        b31 = IICLogic.parse_hex_byte(row[3], "[31:24]")
                        b23 = IICLogic.parse_hex_byte(row[4], "[23:16]")
                        b15 = IICLogic.parse_hex_byte(row[5], "[15:8]")
                        b7 = IICLogic.parse_hex_byte(row[6], "[7:0]")
                        for pass_idx in range(2):
                            for offset, val in [
                                (phys_addr, b7),
                                (phys_addr + 1, b15),
                                (phys_addr + 2, b23),
                                (phys_addr + 3, b31)
                            ]:
                                mtp_ctrl.program_byte(offset, val)
                        success_count += 1
                except Exception as e:
                    error_msg = f"第 {excel_row} 行执行失败: {str(e)}"
                    log_cb(f"!! {error_msg}")
                    return False, total_count, success_count, error_msg

            return True, total_count, success_count, None
        except Exception as e:
            return False, 0, 0, str(e)

    @staticmethod
    def write_mtp_sector_data(mtp_ctrl, sector, byte_val, log_cb):
        """向指定扇区（32 字节）全部写入同一值。

        返回: (success, error_msg)
        """
        try:
            if not (0 <= sector <= 15):
                return False, "扇区必须在0~15之间"
            start_addr = sector * 32
            end_addr = start_addr + 31
            log_cb(f"开始向扇区 {sector} (地址 {start_addr}~{end_addr}) 统一写入数据 0x{byte_val:02X}...")
            for addr in range(start_addr, end_addr + 1):
                mtp_ctrl.program_byte(addr, byte_val)
            log_cb(f"=== 扇区 {sector} 批量写入完成 ===")
            return True, None
        except Exception as e:
            return False, str(e)

    @staticmethod
    def read_and_export_mtp_sector(mtp_ctrl, sector, filename, log_cb):
        """逐字节读取指定扇区数据并导出为 Excel。

        返回: (success, error_msg)
        """
        try:
            if not (0 <= sector <= 15):
                return False, "扇区必须在0~15之间"
            results = []
            start_addr = sector * 32
            end_addr = start_addr + 31
            log_cb(f"开始读取扇区 {sector} 数据 (地址 {start_addr}~{end_addr}) ...")
            for byte_addr in range(start_addr, end_addr + 1):
                dt = mtp_ctrl.read_byte(byte_addr)
                val = dt[3] if dt and len(dt) >= 4 else 0
                results.append(val)
            log_cb(f"=== 扇区 {sector} 读取完毕 ===")

            wb = Workbook()
            ws = wb.active
            ws.title = f"Sector_{sector}"
            headers = ["操作", "基地址(ADDR)", "Word偏移(INDEX)", "[31:24]", "[23:16]", "[15:8]", "[7:0]", "物理起始地址"]
            ws.append(headers)
            font_bold = Font(bold=True)
            for c in range(1, 9):
                ws.cell(row=1, column=c).font = font_bold
            base_mtp_addr = sector << 5
            try:
                addr_base, index_base = mtp_ctrl._addr_index_from_mtp(base_mtp_addr)
            except Exception:
                addr_base, index_base = 0, 0
            for w in range(8):
                phys_addr = start_addr + (w * 4)
                idx_mod = (index_base + w) & 0x3F
                b0 = results[w * 4]
                b1 = results[w * 4 + 1]
                b2 = results[w * 4 + 2]
                b3 = results[w * 4 + 3]
                ws.append([
                    "READ", addr_base, idx_mod,
                    f"{b3:02X}", f"{b2:02X}", f"{b1:02X}", f"{b0:02X}",
                    phys_addr
                ])
            wb.save(filename)
            log_cb(f"成功将扇区 {sector} 数据导出至: {filename}")
            return True, None
        except Exception as e:
            return False, str(e)

    @staticmethod
    def read_mtp_and_export_template(mtp_ctrl, filename, log_cb, progress_cb=None):
        """读取 MTP 地址 4~315 的全部数据，导出为 MTP 模板 Excel，附加 CRC 区域。

        返回: (success, total_count, success_count, error_msg)
        """
        headers = ["OPERATION", "ADDR(基地址)", "INDEX(Word偏移)", "[31:24]", "[23:16]", "[15:8]", "[7:0]", "物理地址(计算)"]
        try:
            wb = Workbook()
            ws = wb.active
            ws.title = "MTP批量模板"
            ws.append(headers)
            note_row = [
                "READ/PROGRAM/ERASE_SECTOR",
                "如 0~511 (MTP基地址)",
                "Word偏移(1代表偏4字节)",
                "对应物理地址 ADDR+INDEX*4 +3 (HEX)",
                "对应物理地址 ADDR+INDEX*4 +2 (HEX)",
                "对应物理地址 ADDR+INDEX*4 +1 (HEX)",
                "对应物理地址 ADDR+INDEX*4 +0 (HEX)",
                "自动公式: =B列+C列*4"
            ]
            ws.append(note_row)
            header_font = Font(bold=True)
            for col in range(1, len(headers) + 1):
                ws.cell(row=1, column=col).font = header_font

            total_count = 0
            success_count = 0
            total_tasks = 79
            current_row = 3
            read_buffer = bytearray()

            for index_mod in range(1, 79):
                if progress_cb:
                    progress_cb(index_mod, total_tasks)
                addr_mod = 0
                phys_addr = addr_mod + (index_mod * 4)
                total_count += 1
                log_cb(f"导出 MTP 读取: ADDR={addr_mod}, INDEX={index_mod} (物理地址={phys_addr})")
                try:
                    v0 = mtp_ctrl.read_byte(phys_addr)
                    v1 = mtp_ctrl.read_byte(phys_addr + 1)
                    v2 = mtp_ctrl.read_byte(phys_addr + 2)
                    v3 = mtp_ctrl.read_byte(phys_addr + 3)
                    b0 = v0[3] if v0 else 0
                    b1 = v1[3] if v1 else 0
                    b2 = v2[3] if v2 else 0
                    b3 = v3[3] if v3 else 0
                    read_buffer.extend([b0, b1, b2, b3])
                    ws.append(["PROGRAM", addr_mod, index_mod, f"{b3:02X}", f"{b2:02X}", f"{b1:02X}", f"{b0:02X}",
                               f"=B{current_row}+C{current_row}*4"])
                    current_row += 1
                    success_count += 1
                except Exception as e:
                    log_cb(f"!! MTP读取导出失败: ADDR={addr_mod}, INDEX={index_mod}, err={e}")
                    ws.append(["READ", addr_mod, index_mod, "", "", "", "", f"=B{current_row}+C{current_row}*4"])
                    read_buffer.extend([0, 0, 0, 0])
                    current_row += 1

            ws.append([])
            addr_mod = 507
            index_mod = 0
            phys_addr = addr_mod + (index_mod * 4)
            total_count += 1
            log_cb(f"导出 MTP CRC: ADDR={addr_mod}, INDEX={index_mod} (物理地址={phys_addr})")
            try:
                v0 = mtp_ctrl.read_byte(phys_addr)
                v1 = mtp_ctrl.read_byte(phys_addr + 1)
                v2 = mtp_ctrl.read_byte(phys_addr + 2)
                v3 = mtp_ctrl.read_byte(phys_addr + 3)
                b0 = v0[3] if v0 else 0
                b1 = v1[3] if v1 else 0
                b2 = v2[3] if v2 else 0
                b3 = v3[3] if v3 else 0
                ws.append(["PROGRAM(存储的CRC)", addr_mod, index_mod, f"{b3:02X}", f"{b2:02X}", f"{b1:02X}", f"{b0:02X}",
                           f"=B{current_row}+C{current_row}*4"])
                current_row += 1
                success_count += 1
            except Exception as e:
                log_cb(f"!! MTP CRC读取导出失败: err={e}")
                ws.append(["READ(CRC)", addr_mod, index_mod, "", "", "", "", f"=B{current_row}+C{current_row}*4"])
                current_row += 1

            crc = 0xFFFFFFFF
            for byte in read_buffer:
                crc ^= (byte << 24)
                for _ in range(8):
                    if crc & 0x80000000:
                        crc = ((crc << 1) ^ 0x04C11DB7) & 0xFFFFFFFF
                    else:
                        crc = (crc << 1) & 0xFFFFFFFF
            ws.append(["CALCULATED(本次数据算得的新CRC)", "", "",
                       f"{(crc >> 24) & 0xFF:02X}", f"{(crc >> 16) & 0xFF:02X}",
                       f"{(crc >> 8) & 0xFF:02X}", f"{crc & 0xFF:02X}",
                       "此处为前面4~315字节读出结果运算得到"])

            for col in ws.columns:
                max_length = 0
                column = col[0].column_letter
                for cell in col:
                    try:
                        cell_len = len(str(cell.value)) if cell.value is not None else 0
                        if cell_len > max_length:
                            max_length = cell_len
                    except Exception:
                        pass
                ws.column_dimensions[column].width = min(max_length + 2, 25)
            wb.save(filename)
            return True, total_count, success_count, None
        except Exception as e:
            return False, 0, 0, str(e)
