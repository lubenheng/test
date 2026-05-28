print("1\n")
import os, sys
from pathlib import Path
sys.path.append(str(Path(__file__).parent))
print("2\n")
from ctypes import *
print("3\n")
import numpy as np
print("4\n")
import datetime,  subprocess
print("5\n")
sys.path.append('..')
print("6\n")
import pyjlink_dev as jlink
print("7\n")
import flash as sa32a12_flash
print("8\n")
import flash_info as sa32a12_flash_info
print("9\n")
import time
print("10\n")

swd_handle = jlink.JLINK_CLASS(None)
print("11\n")
flash0 = sa32a12_flash.class_flash(swd_handle, 0)
print("12\n")
flash1 = sa32a12_flash.class_flash(swd_handle, 1)
print("13\n")
g_flash = [flash0]
print("14\n")

sector_addr_str = ["0x40000180","0x40000200","0x40000280","0x40000300","0x40000380"]
sector_addr = [int(x, 16) for x in sector_addr_str]

def jlink_command_write(user_input):
    bin_name = "mx02_sdk.bin"
    file_name="download.jlink"
    command ='halt\n'+'loadfile ' +bin_name+' \nhalt\nq\nexit'
    with open(file_name,"w",encoding="utf-8") as f:
        f.write(command)
        f.close()
        
def jlink_loadbin(user_input):
#    win32api.ShellExecute(0, 'open', cmd, '', '', 1)  # 前台打开
    if(int(user_input[1], 16) == 1):
        bin_name = "mx02_sdk.bin"
    else:
        bin_name = "internal.bin"
    file_name="download.jlink"
    command ='halt\n'+'loadfile ' +bin_name+' \nhalt\nq\nexit'
    with open(file_name,"w",encoding="utf-8") as f:
        f.write(command)
        f.close()
    cmd = "jlink.exe -device  SA32B16 -if SWD -speed 4000 -autoconnect 1 -CommandFile download.jlink"
    subprocess.check_call(cmd)
    
def jflash_download(user_input):
    if(user_input[1]== "1"):
        cmd = "JFlash.exe -openprjsa32b16.jflash -openinternal.bin,0x0000000 -erasechip -program -verify -startapp -exit"
    else:
        cmd = f"JFlash.exe -openprjsa32b16.jflash -open{user_input[1]},0x0000000 -program -verify -startapp -exit"
    subprocess.check_call(cmd)

def jlink_erase(user_input):
    cmd="jlink.exe -device  SA32B16 -if SWD -speed 4000 -autoconnect 1 -CommandFile erase.jlink"
    subprocess.check_call(cmd)    
    
def get(user_input):
    print("read_addr = 0x%08x, values = " % int(user_input[1],  16))
    result = swd_handle.swd_get_mem32(int(user_input[1],  16),  int(user_input[2]))
    if (result != []):
        print([hex(data) for data in result])
    else:
        print("fail")

def set(user_input):
    data_list = [int(data, 16) for data in user_input[2:]]
    swd_handle.swd_set_mem32(int(user_input[1],  16),  data_list)

def get_reg_all(user_input):
    print(swd_handle.swd_get_reg_all())
    
def nvic_r(user_input):
    swd_handle.swd_reset()
    
def pin_r(user_input):
    swd_handle.swd_pin_reset()
    
def halt(user_input):
    swd_handle.swd_halt()
    
def step(user_input):
    swd_handle.swd_step()
    
def run(user_input):    
    swd_handle.swd_run()
    
def flash_chip_erase(user_input):
    if(user_input[1] == '0'):
        flash0.chip_erase()
        print('code flash erase done!')
    if(user_input[1] == '1'):
        flash1.chip_erase()
        print('data flash erase done!')

def flash_sector_erase(user_input):
    addr_start = int(user_input[1], 16)
    flash0.chip_sector_erase(addr_start)

    
def flash_read_apb(user_input):
    addr_start = int(user_input[1], 16)
    length = int(user_input[2], 16)

    result = flash0.read(addr_start, length)
        
    if (result != []):
        print([hex(data) for data in result])
    else:
        print("fail")
        
def flash_write(user_input):
    addr_start = int(user_input[1], 16)
    input_data = [int(data, 16) for data in user_input[2:]]
    flash0.write(addr_start, input_data)

def data_flash_write(user_input):
    addr_start = int(user_input[1], 16)
    input_data = [int(data, 16) for data in user_input[2:]]
    flash1.write(addr_start, input_data)
        
def flash_read_ahb(user_input):
    addr_start = int(user_input[1], 16)
    length = int(user_input[2], 16)
    result = flash0.read_by_ahb(addr_start, length)
    if (result != []):
        print([hex(data) for data in result])
    else:
        print("fail") 

def data_flash_read_ahb(user_input):
    addr_start = int(user_input[1], 16)
    length = int(user_input[2], 16)
    result = flash1.read_by_ahb(addr_start, length)
    if (result != []):
        print([hex(data) for data in result])
    else:
        print("fail") 

def dump_by_apb(user_input):
    start_addr = int(user_input[1], 16)
    dump_size = int(user_input[2], 16)
    
    if(dump_size > 1024*512):
        print("max dump_size is 512KB!")
        return
     
    #2 flash unlock
    flash0.unlock()

    rd_data= flash0.read(start_addr, int(dump_size/8))
    bin_file = 'read_by_apb.bin'

    save_fw_rb_data_to_bin(rd_data, bin_file)
    print("save success!")

def dump_by_ahb(user_input):
    start_addr = int(user_input[1], 16)
    dump_size = int(user_input[2], 16)
    
    if(dump_size > 1024*512):
        print("max dump_size is 512KB!")
        return
    rd_data = swd_handle.swd_get_mem32(start_addr, dump_size>>2)
    bin_file = 'read_by_ahb.bin'
    print("dump data to: " + bin_file)
    save_fw_rb_data_to_bin(rd_data, bin_file)
    print("save success!")


FLASH_PAGE_SIZE_BYTES = 4
FLASH_SIZE = 0x200009-4
def parse_fw(fw_bin_file):
    bin_file_size = os.path.getsize(fw_bin_file)
    bin_file = open(fw_bin_file,  'rb')
    fw_data = (c_ubyte * bin_file_size)()
    for i in range(0, bin_file_size):
        c = bin_file.read(1)
        fw_data[i] = ord(c)
    bin_file.close
    return fw_data

def fw_write_in_word(bin_file = '', start_addr = 0 ):
    fw_data = parse_fw(bin_file)
    
    fw_len = len(fw_data)
    if ((fw_len%FLASH_PAGE_SIZE_BYTES) != 0):
        for i in range(FLASH_PAGE_SIZE_BYTES-(fw_len%FLASH_PAGE_SIZE_BYTES)):
            np.append(fw_data, [0])
#                fw_data.append(0)
        fw_len += FLASH_PAGE_SIZE_BYTES - (fw_len%FLASH_PAGE_SIZE_BYTES)

    fw_data_word_list = []
    fw_offset = 0
    while(fw_offset < fw_len):
        data = int.from_bytes(fw_data[fw_offset : fw_offset+4], byteorder='little', signed=False)
        fw_offset = fw_offset + 4
        fw_data_word_list.append(data)

    address = start_addr
    print(fw_len)

    instance = int(address/FLASH_SIZE)
    pflash = g_flash[instance]
    pflash.unlock()
    pflash.chip_erase()
    print("begin download")
    pflash.write(address, fw_data_word_list)
    pflash.lock()   

def flash_download(user_input):
    start_time = time.time()
    fw_write_in_word(user_input[2],int(user_input[1]))
    end_time = time.time()
    print(end_time - start_time)


def silergy_for_mx02_sector_info_erase_to_default(user_input):#need to check the default value
    code_info_0_31 = [0xFFFF0000] * silergy_for_mx02_sector.used_slot_size
    code_info_0_31[34] = 0xfff0000f
    code_info_0_31[35] = 0xfff0000f
    code_info_0_31[36] = 0x8cca7335
    code_info_0_31[37] = 0x8cca7335

    silergy_for_mx02_sector.sector_erase()
    silergy_for_mx02_sector.write(code_info_0_31)
    silergy_for_mx02_sector.refresh()
    # silergy_for_mx02_sector_info_read(user_input)


def silergy_for_mx02_sector_info_erase(user_input):#need to check the default value
    silergy_for_mx02_sector.sector_erase()
    silergy_for_mx02_sector.refresh()
    # silergy_for_mx02_sector_info_read(user_input)


def silergy_for_mx02_sector_info_read(user_input):
    info0 = silergy_for_mx02_sector.read()
    for i in range(len(info0)):
        print("silergy info slot%d data: 0x%08x"%(i,  info0[i]))


def silergy_for_mx02_sector_shadow_read(user_input):
    info0 = silergy_for_mx02_sector.shadow_reg_read()
    for i in range(len(info0)):
        print("silergy shadow slot%d data: 0x%08x"%(i,  info0[i]))


def silergy_for_mx02_sector_info_change(user_input):
    info_n = int(user_input[1])
    data_start = int(user_input[2])
    data_length = int(user_input[3])
    data_input = int(user_input[4], 16)
    silergy_for_mx02_sector.info_change(info_n, data_start, data_length, data_input)
    silergy_for_mx02_sector.refresh()

    # silergy_for_mx02_sector_info_read(user_input)


def silergy_for_mx02_sector_shadow_change(user_input):
    info_n = int(user_input[1])
    data_start = int(user_input[2])
    data_length = int(user_input[3])
    data_input = int(user_input[4], 16)
    silergy_for_mx02_sector.shadow_reg_change(info_n, data_start, data_length, data_input)


def secure_boot_sector_info_erase_default(user_input):#need to check the default value
    code_info_0_31 = [0xFFFF0000] * secure_boot_sector.used_slot_size
    code_info_0_31[38] = 0x0000ffff
    code_info_0_31[39] = 0x0000ffff
    code_info_0_31[76] = 0x9c866379
    code_info_0_31[77] = 0x9c866379

    secure_boot_sector.sector_erase()
    secure_boot_sector.write(code_info_0_31)
    secure_boot_sector.refresh()
    # secure_boot_sector_info_read(user_input)


def secure_boot_sector_info_erase(user_input):#need to check the default value
    secure_boot_sector.sector_erase()
    secure_boot_sector.refresh()
    # secure_boot_sector_info_read(user_input)


def secure_boot_sector_info_change(user_input):
    info_n = int(user_input[1])
    data_start = int(user_input[2])
    data_length = int(user_input[3])
    data_input = int(user_input[4], 16)

    secure_boot_sector.info_change(info_n, data_start, data_length, data_input)
    secure_boot_sector.refresh()
    # secure_boot_sector_info_read(user_input)


def secure_boot_sector_info_read(user_input):
    info0 = secure_boot_sector.read()
    for i in range(len(info0)):
        print("secure_boot_sector slot%d data: 0x%08x"%(i, info0[i]))


def secure_boot_sector_shadow_read(user_input):
    info0 = secure_boot_sector.shadow_reg_read()
    for i in range(len(info0)):
        print("secure_boot_sector slot%d data: 0x%08x"%(i, info0[i]))


def secure_boot_sector_shadow_change(user_input):
    info_n = int(user_input[1], 16)
    data_start = int(user_input[2])
    data_length = int(user_input[3])
    data_input = int(user_input[4], 16)
    secure_boot_sector.shadow_reg_change(info_n, data_start, data_length, data_input)

def batch_setting(user_input):
    """
    @brief: user can edit multiple commands in user_define.txt, then call batch_setting will
            batch execute these commands.
    @param: user_input: user_define.txt, the commands user want to execute
    @return: command results
    """
    file = open(user_input[1], "r")
    lines = []
    for line in file:
        lines.append(line.strip())
    file.close()
    for line in lines:
        print(line)
        input_line = line.split()
        func_menu_info[input_line[0]][0](input_line)
        time.sleep(0.1)

def get_valid_input(valid_list):
    while True:
        valid_input = input("input your choice:\n").split(" ")
        cmd = valid_input[0]
        if cmd in valid_list:
            return valid_input
        else:
            print(f'bad input: {valid_input}')

def get_menu_info(user_input):
    # show the menu
    print("\n cmdline:")
    for cmd in func_menu_info.keys():
        print(f'      # {cmd} : {func_menu_info[cmd][1]}')
    print("\n")

def split_to_registers(word):
    if not (0 <= word <= 0xFFFFFFFF):
        raise ValueError("ID must be 32 unsigned integer(0-4294967295)")
    
    low_register = word & 0xFFFF
    
    high_register = (word >> 16) &0xFFFF
    
    return high_register,low_register

def customer_defined_ID(user_input):
    ID_1 = int(user_input[1],16)
    ID_1_high, ID_1_low = split_to_registers(ID_1)
    ID_2 = int(user_input[2],16)
    ID_2_high, ID_2_low = split_to_registers(ID_2)
    secure_boot_sector.info_change(20, 0, 16, ID_1_low)
    secure_boot_sector.info_change(22, 0, 16, ID_1_high)
    secure_boot_sector.info_change(24, 0, 16, ID_2_low)
    secure_boot_sector.info_change(26, 0, 16, ID_2_high)
    secure_boot_sector.refresh()

def read_specified_INFO(info_num):
    info = secure_boot_sector.read()
    info_data = info[info_num]&0xffff
    return info_data
    

def read_defined_ID(user_input):
    ID_1_low = read_specified_INFO(20)
    ID_1_high = read_specified_INFO(22)
    ID_1 = (ID_1_high<<16) | ID_1_low
    ID_2_low = read_specified_INFO(24)
    ID_2_high = read_specified_INFO(26)
    ID_2 = (ID_2_high<<16) | ID_2_low
    print(f"ID_1: 0x{ID_1:08x}")
    print(f"ID_2: 0x{ID_2:08x}")
    
def Boot_Firmware_Start_Address_change(user_input):
    address = int(user_input[1], 16)
    secure_boot_sector.info_change(28, 0, 16, address)
    secure_boot_sector.refresh()

def Boot_Firmware_Start_Address_read(user_input):
    address = read_specified_INFO(28)
    print(f"Boot_Firmware_Start_Address is 0x{address:04x}")

def Boot_Firmware_Size_change(user_input):
    size = int(user_input[1], 16)
    secure_boot_sector.info_change(30, 0, 16, size)
    secure_boot_sector.refresh()

def Boot_Firmware_Size_read(user_input):
    address = read_specified_INFO(30)
    print(f"Boot_Firmware_Size is 0x{address:04x}")

def Boot_Firmware_CRC_Check_Mode_sel(user_input):
    mode_sel = int(user_input[1],16)
    secure_boot_sector.info_change(32, 0, 4, mode_sel)
    secure_boot_sector.refresh()

def Boot_Firmware_CRC_Check_Mode_read(user_input):
    address = read_specified_INFO(32)
    address = address&0xf
    print(f"Boot_Firmware_CRC_Check_Mode is {address:01x}")

def Boot_Firmware_CRC32_change(user_input):
    CRC32 = int(user_input[1],16)
    CRC32_high, CRC32_low = split_to_registers(CRC32)
    secure_boot_sector.info_change(34, 0, 16, CRC32_low)
    secure_boot_sector.info_change(36, 0, 16, CRC32_high)
    secure_boot_sector.refresh()

def Boot_Firmware_CRC32_read(user_input):
    CRC32_low = read_specified_INFO(34)
    CRC32_high = read_specified_INFO(36)
    CRC32 = (CRC32_high<<16)|CRC32_low
    print(f"CRC32: 0x{CRC32:08x}")

def Boot_End_Register_change(user_input):
    data = int(user_input[1],16)
    secure_boot_sector.info_change(38, 0, 16, data)
    secure_boot_sector.refresh()

def Boot_End_Register_read(user_input):
    data = read_specified_INFO(38)
    print(f"Boot_End_Register_read is 0x{data:04x}")

def Flash_Debug_Read_Protect_change(user_input):
    num = int(user_input[1])*4 + 52
    start_address = int(user_input[2],16)
    length = int(user_input[3])
    secure_boot_sector.info_change(num, 0, 16, start_address)
    secure_boot_sector.info_change(num+2, 0, 16, length)
    secure_boot_sector.refresh()

def Flash_Debug_Read_Protect_read(user_input):
    num = int(user_input[1])*4 + 52
    start_address = read_specified_INFO(num)
    length = read_specified_INFO(num+2)
    print(f"start_address is 0x{start_address:04x}, length is 0x{length:04x}")

def Flash_Write_Protect_change(user_input):
    num = int(user_input[1])*4 + 68
    start_address = int(user_input[2],16)
    length = int(user_input[3])
    secure_boot_sector.info_change(num, 0, 16, start_address)
    secure_boot_sector.info_change(num+2, 0, 16, length)
    secure_boot_sector.refresh()

def Flash_Write_Protect_read(user_input):
    num = int(user_input[1])*4 + 68
    start_address = read_specified_INFO(num)
    length = read_specified_INFO(num+2)
    print(f"start_address is 0x{start_address:04x}, length is 0x{length:04x}")

def Boot_Sector_Lock_control(user_input):
    lock_control = int(user_input[1],16)
    secure_boot_sector.info_change(0, 0, 4, lock_control)
    secure_boot_sector.refresh()

def Boot_Sector_Lock_Status_read(user_input):
    data = read_specified_INFO(0)
    print(f"Boot_Sector_Lock_Status is 0x{data:01x}")

def NVR2_Password_change(user_input):
    password_1 = int(user_input[1],16)
    password_1_high, password_1_low = split_to_registers(password_1)
    password_2 = int(user_input[2],16)
    password_2_high, password_2_low = split_to_registers(password_2)
    secure_boot_sector.info_change(2, 0, 16, password_1_low)
    secure_boot_sector.info_change(4, 0, 16, password_1_high)
    secure_boot_sector.info_change(6, 0, 16, password_2_low)
    secure_boot_sector.info_change(8, 0, 16, password_2_high)
    secure_boot_sector.refresh()

def NVR2_Password_read(user_input):
    password_1_low = read_specified_INFO(2)
    password_1_high = read_specified_INFO(4)
    password_1 = (password_1_high<<16) | password_1_low
    password_2_low = read_specified_INFO(6)
    password_2_high = read_specified_INFO(8)
    password_2 = (password_2_high<<16) | password_2_low
    print(f"password_1: 0x{password_1:08x}")
    print(f"password_2: 0x{password_2:08x}")

def Version_Sector_Lock_control(user_input):
    NVR_sel = int(user_input[1],16)
    lock_control = int(user_input[2],16)
    secure_boot_sector.info_change(10, 4, 5, NVR_sel)
    secure_boot_sector.info_change(10, 0, 4, lock_control)
    secure_boot_sector.refresh()

def Version_Sector_Lock_read(user_input):
    version_sector_lock = read_specified_INFO(10)
    NVR_sel = (version_sector_lock & 0x1f0)>>4
    lock_control = version_sector_lock & 0xf
    
    print(f"NVR_sel: 0x{NVR_sel:02x}")
    print(f"lock_control: 0x{lock_control:01x}")

def Secure_SWD_Lock_control(user_input):
    lock_control = int(user_input[1],16)
    write_control = int(user_input[2],16)
    secure_boot_sector.info_change(42, 0, 4, lock_control)
    secure_boot_sector.info_change(42, 4, 2, write_control)
    secure_boot_sector.refresh()

def Secure_SWD_Lock_read(user_input):
    SWD_Lock_control = read_specified_INFO(42)
    lock_control = SWD_Lock_control & 0xf
    write_control = (SWD_Lock_control & 0x30)>>4
    print(f"lock_control: 0x{lock_control:01x}")
    print(f"password_2: 0x{write_control:01x}")

def NVR3_to_7_Password_change(user_input):
    password_1 = int(user_input[1],16)
    password_1_high, password_1_low = split_to_registers(password_1)
    password_2 = int(user_input[2],16)
    password_2_high, password_2_low = split_to_registers(password_2)
    secure_boot_sector.info_change(2, 0, 16, password_1_low)
    secure_boot_sector.info_change(4, 0, 16, password_1_high)
    secure_boot_sector.info_change(6, 0, 16, password_2_low)
    secure_boot_sector.info_change(8, 0, 16, password_2_high)
    secure_boot_sector.refresh()

def NVR3_to_7_Password_read(user_input):
    password_1_low = read_specified_INFO(2)
    password_1_high = read_specified_INFO(4)
    password_1 = (password_1_high<<16) | password_1_low
    password_2_low = read_specified_INFO(6)
    password_2_high = read_specified_INFO(8)
    password_2 = (password_2_high<<16) | password_2_low
    print(f"password_1: 0x{password_1:08x}")
    print(f"password_2: 0x{password_2:08x}")
    
def SWD_Password_change(user_input):
    password_1 = int(user_input[1],16)
    password_1_high, password_1_low = split_to_registers(password_1)
    password_2 = int(user_input[2],16)
    password_2_high, password_2_low = split_to_registers(password_2)
    secure_boot_sector.info_change(44, 0, 16, password_1_low)
    secure_boot_sector.info_change(46, 0, 16, password_1_high)
    secure_boot_sector.info_change(48, 0, 16, password_2_low)
    secure_boot_sector.info_change(50, 0, 16, password_2_high)
    secure_boot_sector.refresh()

def SWD_Password_read(user_input):
    password_1_low = read_specified_INFO(44)
    password_1_high = read_specified_INFO(46)
    password_1 = (password_1_high<<16) | password_1_low
    password_2_low = read_specified_INFO(48)
    password_2_high = read_specified_INFO(50)
    password_2 = (password_2_high<<16) | password_2_low
    print(f"password_1: 0x{password_1:08x}")
    print(f"password_2: 0x{password_2:08x}")

def NVR3_to_7_info_change(user_input):
    sector_num_input = int(user_input[1])
    sector_num = sector_num_input - 3
    line = int(user_input[2])
    data_str = user_input[3:]
    if sector_num<0 or sector_num>4:
        print(f"sector num = {sector_num_input}, error! NVR_num must be 3-7!")
        return 0
    if line > 127:
        print(f"error! start_line must be 0-127")
        return 0
    data= list()
    for item in data_str:
        data.append(int(item, 16))    
    for item in data:
        print(f"write data :0x{item:08x}")
    version_sel[sector_num_input].mx02_flash_info_version_write(sector_addr[sector_num]+line, data)

def NVR3_to_7_info_read(user_input):
    sector_num_input = int(user_input[1])
    sector_num = sector_num_input - 3
    start_line = int(user_input[2])
    read_length = int(user_input[3])
    if sector_num<0 or sector_num>4:
        print(f"NVR_num = {sector_num_input}, error! NVR_num must be 3-7!")
        return 0
    if start_line > 127:
        print(f"error! start_line must be 0-127")
        return 0
    if start_line + read_length > 127:
        print(f"warning! The scope of reading exceeds NVR{sector_num}!")
        return 0
    read_data = []
    read_data = version_sel[sector_num_input].mx02_flash_info_version_read(sector_addr[sector_num]+start_line, read_length)
    i=0
    for item in read_data:
        print(f"read data line {start_line+i}:0x{item:08x}")
        i += 1
    
def NVR3_to_7_info_erase(user_input):
    sector_num_input = int(user_input[1])
    version_sel[sector_num_input].sector_erase()
    print(f"NVR{sector_num_input} erase success!")

def modify_cmd(user_input):
    """
    终端命令: 修改寄存器的指定位段
    用法: modify <reg_addr> <value> <start_bit> <end_bit>
    示例: modify 0x40000814 0x55 0 7
    """
    if len(user_input) < 5:
        print("用法: modify <reg_addr> <value> <start_bit> <end_bit>")
        print("示例: modify 0x40000814 0x55 0 7")
        return
    reg_addr = int(user_input[1], 16)
    value = int(user_input[2], 16)
    start_bit = int(user_input[3])
    end_bit = int(user_input[4])
    modify_register_range(reg_addr, value, start_bit, end_bit)

#Add Code in here

def unlock_mu5_decryption(_=None):
    """
    MU5 解密准备流程( 完全使用 swd_handle 封装接口，不依赖 dll) 
    """
    print("=== MU5 Decryption Unlock Sequence ===")

    # 第一步: 复位并 halt
    print("1. Reset and halt CPU...")
    swd_handle.swd_reset()      # r
    swd_handle.swd_halt()       # h

    # 第二步: go，启动 boot
    print("2. Let CPU run (start boot)...")
    swd_handle.swd_run()        # g

    # 第三步: 等待 10ms( 让 boot 第一句开启 WDT) 
    print("3. Wait 10ms for boot to enable WDT...")
    time.sleep(0.01)

    # 第四步: 关闭 WDT( ⚠️ 请根据 MU5 手册确认地址和值！) 
    WDT_ADDR = 0x40000008       # ← 替换为真实地址
    WDT_DISABLE_VAL = 0x00000000  # ← 替换为正确值
    print(f"4. Disable WDT at 0x{WDT_ADDR:08x}...")
    swd_handle.swd_set_mem32(WDT_ADDR, [WDT_DISABLE_VAL])

    # 第五步: 等待 1 秒，让 boot 完成自动解密
    print("5. Wait 1 second for boot to finish decryption...")
    time.sleep(1.0)

    # 第六步: 再次 halt CPU( 此时已解密) 
    print("6. Halt CPU (decrypted state)...")
    swd_handle.swd_halt()

    print("✅ MU5 decryption unlock completed.\n")


def read_register_32bit(reg_addr, return_hex=False):    #read one register address
    """
    读取指定地址的 32bit 寄存器/内存值
    :param reg_addr: 寄存器地址，支持格式: 
                     - 16进制字符串: "0x40000180" / "40000180"
                     - 十进制整数: 1073741184 (对应 0x40000180)
    :param return_hex: 是否返回十六进制格式( False 返回十进制整数) 
    :return: 32bit 寄存器值( 读取失败返回 None) 
    """
    # 步骤1: 地址格式统一转换为十进制整数
    try:
        if isinstance(reg_addr, str):
            # 处理 16 进制字符串( 带/不带 0x 前缀) 
            if reg_addr.startswith(("0x", "0X")):
                addr = int(reg_addr, 16)
            else:
                # 默认为 16 进制字符串( 如 "40000180") 
                addr = int(reg_addr, 16)
        elif isinstance(reg_addr, int):
            # 直接使用十进制整数地址
            addr = reg_addr
        else:
            raise TypeError("地址仅支持字符串( 16进制) 或整数类型")
    except Exception as e:
        print(f"地址格式错误: {e}")
        return None

    # 步骤2: 调用 SWD 底层接口读取 32bit 数据( 长度固定为 1) 
    try:
        # 底层核心逻辑: 读取指定地址的 1 个 32bit 数据
        result = swd_handle.swd_get_mem32(addr, 1)
        if not result:
            print(f"读取地址 0x{addr:08x} 失败")
            return None

        # 步骤3: 返回结果( 按需返回十六进制/十进制) 
        reg_value = result[0]  # 读取长度为1，取第一个值
        if return_hex:
            return hex(reg_value)  # 如 0x12345678
        else:
            return reg_value  # 如 305419896

    except Exception as e:
        print(f"读取寄存器失败: {e}")
        return None

def write_register(reg_addr, reg_values):       #write one register 
    """
    封装寄存器写入逻辑, 对接底层set函数
    :param reg_addr: 寄存器地址(整数)16进制/十进制均可, 如0x40001C0C 或 1073745932
    :param reg_values: 要写入的值( 单个十进制整数 或 十进制整数列表, 如10000 或 [10000, 20000] )
    """
    # 步骤1: 将寄存器地址转为带0x的16进制字符串( 适配set函数的解析逻辑) 
    addr_str = f"0x{reg_addr:08x}"  # 格式化为8位16进制字符串，如0x40001c0c
    
    # 步骤2: 将十进制值转为带0x的16进制字符串( 支持单个值/多个值) 
    if not isinstance(reg_values, list):
        reg_values = [reg_values]  # 统一转为列表，兼容单个值场景
    data_str_list = [f"0x{val:x}" for val in reg_values]  # 十进制转16进制字符串，如10000→0x2710
    
    # 步骤3: 构造set函数需要的user_input格式( 模拟命令行输入的拆分列表) 
    user_input = ["set", addr_str] + data_str_list
    
    # 步骤4: 调用底层set函数完成写入
    #set(["set", "0x40000814", "0x55aaaa55"])
    #print("已解锁写保护，正在写入寄存器...")
    #time.sleep(0.1)  #入后稍作等待，确保操作完成
    set(user_input)
    
"""终端使用示例: modify 0x40000814 0x55 0 7"""
def modify_register_range(reg_addr, value, start_bit, end_bit):
    """
    【按位段修改寄存器】
    :param reg_addr: 寄存器地址
    :param value: 要写入的数值
    :param start_bit: 要修改的最低位
    :param end_bit: 要修改的最高位
    """
    current_val = read_register_32bit(reg_addr)
    if current_val is None:
        print("读取寄存器失败！")
        return

    if not (0 <= start_bit <= end_bit <= 31):
        print("位号错误！必须 0~31")
        return

    # 自动计算掩码
    bit_length = end_bit - start_bit + 1
    mask = ((1 << bit_length) - 1) << start_bit
    shifted_value = (value << start_bit) & mask

    # 修改并写回
    new_val = (current_val & ~mask) | shifted_value
    write_register(reg_addr, new_val)
    #print(f"✅ 修改成功: 位{start_bit}~{end_bit} = {value}")


# def code_flash_write_main_by_apb(user_input):
#     start_addr = int(user_input[1])
#     data_str = user_input[3:]
    
#     for item in data_str:
#         print(f"write data :0x{item:08x}")

#     flash_instance = flash0 
    
#     print(" Start download...", end="")
#     # 3 start flash sector erase
#     start_sector_index = int(start_addr/512)
#     sector_num = int(len(data_str)/512 +1)
#     # print(start_sector_index)
#     # print(sector_num)

#     flash_instance.sector_erase(start_sector_index, sector_num)

#     result = flash_instance.read(start_addr, 1)
#     if (result != []):
#         print([hex(data) for data in result])

#     ##input_data = [int(data, 16) for data in fw_data]
#     # code_flash.write(start_addr, fw_data)
#     address = start_addr

#     instance = int(address/FLASH_SIZE)
#     pflash = g_flash[instance]
#     pflash.unlock()
#     pflash.chip_erase()
#     print("begin download")
#     pflash.write(address, fw_data_word_list)
#     pflash.lock()   

#     result = flash_instance.read(start_addr, 1)
#     if (result != []):
#         print([hex(data) for data in result])

#     print("Flash write Done")
#     print("\n")

func_menu_info = {
    # cmd : [function, example or info]
    'get': [get, "read memory(other parameters: add data_list)"],
    'set': [set, "write memory(other parameters: add data_list)"],
    'modify': [modify_cmd, "modify register bit field (para: reg_addr value start_bit end_bit)"],
    'get_reg_all': [get_reg_all, "show all of the cortex-m registers value"],
    'nvic_r': [nvic_r, "reset mcu by NVIC system reset"],
    'pin_r': [pin_r, "reset mcu by toggle the swd RST pin"],
    'halt': [halt, "halt CPU"],
    'h': [halt, "halt CPU"],
    'step': [step, "mcu step"],
    'go': [run, "mcu run"],
    'g': [run, "mcu step"],
    'UNLOCK_MU5': [lambda _: unlock_mu5_decryption(), "Unlock MU5 decryption sequence (for encrypted chips)"],

    # flash chip erase
    "0": [flash_chip_erase, "chip erase flash, 0: code_code_flash; 1:data_flash"],
    "chip_erase": [flash_chip_erase, "chip erase flash, 0: code_code_flash; 1:data_flash"],
    "sector_erase":[flash_sector_erase, "sector erase flash, sector address"],

    # read flash by apb
    "1": [flash_read_apb, "read flash by 2(para: ahb_start_addr;num(unit:4 words))"],
    "read": [flash_read_apb, "read flash by apb(para: ahb_start_addr;num(unit:4 words))"],
    #"1-1": [data_read_apb, "read flash by apb(para: start_addr from 0;num(unit:2 words))"],
    #"data_read": [data_read_apb, "read flash by apb(para: start_addr from 0;num(unit:2 words))"],

    # flash write
    '2-0': [flash_write, "write code flash (para: ahb_start_addr write data 4 word)"],
    'write': [flash_write, "write flash (para: ahb_start_addr write data 4 word)"],
    '2-1' : [data_flash_write, "write data flash (para: start_addr from 0; write data 2 word)"],
    #'#2-1': [data_write, "write data flash (para: start_addr from 0; write data 2 word)"],
    #'data_write': [data_write, "write data flash (para: start_addr from 0; write data 2 word)"],

    # flash read by ahb
    '3-0': [flash_read_ahb, "read flash by ahb((para: ahb_start_addr;num(unit:words))"],
    #'read_h': [flash_read_ahb, "read flash by ahb((para: ahb_start_addr;num(unit:words))"],
    '3-1' :[data_flash_read_ahb, "read data_flash by ahb((para: ahb_start_addr;num(unit:words))"],

    # silergy_for_mx02_sector operation
    '4-0': [silergy_for_mx02_sector_info_erase_to_default, "erase silergy_for_mx02_sector to default"],
    '4-1': [silergy_for_mx02_sector_info_erase, "erase silergy_for_mx02_sector"],
    '4-2': [silergy_for_mx02_sector_info_change, "silergy_for_mx02_sector_info_change slot_num start_bit length data_input"],
    '4-3': [silergy_for_mx02_sector_info_read, "read silergy_for_mx02_sector info"],
    '4-4': [silergy_for_mx02_sector_shadow_read, "read silergy_for_mx02_sector shadow"],
    '4-5': [silergy_for_mx02_sector_shadow_change, "silergy_for_mx02_sector_shadow_change  slot_num start_bit length data_input"],

    # secure_boot_sector operation
    "5-0": [secure_boot_sector_info_erase_default, "erase secure_boot_sector to default"],
    "5-1": [secure_boot_sector_info_erase, "erase secure_boot_sector"],
    "5-2": [secure_boot_sector_info_change, "secure_boot_sector_info_change  slot_num start_bit length data_input"],
    "5-3": [secure_boot_sector_info_read, "read  secure_boot_sector info"],
    "5-4": [secure_boot_sector_shadow_read, "read  secure_boot_sector shadow"],
    '5-5': [secure_boot_sector_shadow_change, "secure_boot_sector_shadow_change slot_num start_bit length data_input"],

    #customer defined ID
    "6-1"  : [customer_defined_ID ,"define_customer_ID  (para: ID_0(1_word) , ID_1(1_word)"],
    "6-2"  : [read_defined_ID, "read_defined_ID"],
    
    #Boot Firmware
    "7-0"  : [Boot_Firmware_Start_Address_change, "change Boot_Firmware_Start_Address  (para: address(16bit))"],
    "7-1"  : [Boot_Firmware_Start_Address_read,   "read_Boot_Firmware_Start_Address    "],
    "7-2"  : [Boot_Firmware_Size_change,  "change Boot_Firmware_Size  (para: size(16bit)"],
    "7-3"  : [Boot_Firmware_Size_read,  "change Boot_Firmware_read"],
    "7-4"  : [Boot_Firmware_CRC_Check_Mode_sel,   "select Boot_Firmware_CRC_Check_Mode,  (para: 0/1/2)"],
    "7-5"  : [Boot_Firmware_CRC_Check_Mode_read,   "read Boot_Firmware_CRC_Check_Mode"],
    "7-6"  : [Boot_Firmware_CRC32_change,   "change Boot_Firmware_CRC32,  (para: CRC32(1 word))"],
    "7-7"  : [Boot_Firmware_CRC32_read,   "read Boot_Firmware_CRC32 "],
    "7-8"  : [Boot_End_Register_change,  "change Boot_End_Register (para: size(16bit))"],
    "7-9"  : [Boot_End_Register_read,  "read Boot_End_Register"],
    
    #Flash_Debug_Read_Protect and Flash_Write_Protect
    "8-0"  : [Flash_Debug_Read_Protect_change, "change Flash_Debug_Read_Protect  (para: num(0-1), start_address(16bit), length(16bit))"],
    "8-1"  : [Flash_Debug_Read_Protect_read, "read Flash_Debug_Read_Protect  (para: num(0-3))"],
    "8-2"  : [Flash_Write_Protect_change, "change Flash_Write_Protect_change  (para: num(0-3), start_address(16bit), length(16bit))"],
    "8-3"  : [Flash_Write_Protect_read, "read Flash_Write_Protect_read  (para: num(0-3))"],
    
    # dump and flash write by file
    '9-1': [jlink_command_write, "jlink_command_write"],
    '9-2': [jlink_loadbin, "jlink_loadbin"],
    '9-3': [jflash_download, "jflash_download"],
    '9-4': [flash_download, "code flash write by apb (para: start_addr, selected_bin_file)"],
    '9-5': [dump_by_apb, "code flash dump by apb (para: start_addr, length(unit:byte))"],
    '9-6': [dump_by_ahb, "code flash dump by ahb (para: start_addr, length(unit:byte))"],

    # boot_sector_lock \ version_sector_lock \ Secure_SWD_lock
    '10-0' : [Boot_Sector_Lock_control, "control Boot_Sector_Lock (para: control_data(4bit))"],
    '10-1' : [Boot_Sector_Lock_Status_read, "read Boot_Sector_Lock_Status "],
    '10-2' : [Version_Sector_Lock_control, "control Version_Sector_Lock, (para: NVR_sel(bit0-4 seperately ctl NVR3-7), lock_control(4bit))"],
    '10-3' : [Version_Sector_Lock_read, "read Version_Sector_Lock"],
    '10-4' : [Secure_SWD_Lock_control, "control Secure_SWD_Lock, (para: lock_control(4bit) , write_control(2bit))"],
    '10-5' : [Secure_SWD_Lock_read, "read Secure_SWD_Lock"],
    
    # change NVR2\NVR3-7 password\swd password
    '11-0' : [NVR2_Password_change , "change NVR2_Password (para: password_1(1word) , password_2(1word))"],
    '11-1' : [NVR2_Password_read,    "read NVR2_Password"],
    '11-2' : [NVR3_to_7_Password_change , "change NVR3_to_7_Password (para: password_1(1word) , password_2(1word))"],
    '11-3' : [NVR3_to_7_Password_read,    "read NVR3_to_7_Password"],
    '11-4' : [SWD_Password_change,    "change SWD_Password (para: password_1(1word) , password_2(1word))"],
    '11-5' : [SWD_Password_read,    "read SWD_Password"],

    # batch setting
    '12-1': [batch_setting, "batch execute commands"],

    # change NVR3-7 info
    '13-0': [NVR3_to_7_info_change, "change NVR3_to_7_info  (para: NVR_num(3-7), line(0-127), data(1word))"],
    '13-1': [NVR3_to_7_info_read, "read NVR3_to_7_info  (para: NVR_num(3-7), line(0-127), read_length)"],
    '13-2': [NVR3_to_7_info_erase, "erase NVR3_to_7_info, (para: NVR_num(3-7)) "],
    
    # show menu info
    'help': [get_menu_info, "show menu info"],
    
    # internal test cmd
    # '14-0': [code_flash_write_main_by_apb, "code_flash_write_main_by_apb (para: start_addr, selected_bin_file)"],
    # '14-1': [data_flash_write_main_by_apb, "data_flash_write_main_by_apb (para: start_addr, selected_bin_file)"],
    # '14-2': [code_flash_erase_main_by_apb, "code_flash_erase_main_by_apb (para:)"],
    # '14-3': [data_flash_erase_main_by_apb, "data_flash_erase_main_by_apb (para:)"]
    
}

def init_all():
    """封装初始化逻辑的函数"""
    global swd_handle, silergy_for_mx02_sector, secure_boot_sector, version_sel
    
    # SWD连接
    swd_handle.jlink_handle.close()
    swd_handle.swd_connect()
    
    # 各类扇区对象初始化
    silergy_for_mx02_sector = sa32a12_flash_info.class_code_flash_info(swd_handle, 0, 0, 64)
    secure_boot_sector = sa32a12_flash_info.class_code_flash_info(swd_handle, 0, 1, 82)
    
    version_3_sector = sa32a12_flash_info.class_code_flash_info(swd_handle, 0, 2, 128)
    version_4_sector = sa32a12_flash_info.class_code_flash_info(swd_handle, 0, 3, 128)
    version_5_sector = sa32a12_flash_info.class_code_flash_info(swd_handle, 0, 4, 128)
    version_6_sector = sa32a12_flash_info.class_code_flash_info(swd_handle, 0, 5, 128)
    version_7_sector = sa32a12_flash_info.class_code_flash_info(swd_handle, 0, 6, 128)
    
    version_sel = {3:version_3_sector,4:version_4_sector,5:version_5_sector,6:version_6_sector,7:version_7_sector}
    
    # 显示菜单
    #get_menu_info([""])
    return swd_handle

if __name__ == "__main__":

    init_all()
    time.sleep(0.5)  # 等待初始化完成( 如有必要) 
    unlock_mu5_decryption()

    while True:
        user_input = get_valid_input(func_menu_info.keys())  # input a valid cmdline
        try:
            func_menu_info[user_input[0]][0](user_input)  # execute the cmd
        except Exception as e:
            print(e)
