# -*- coding: utf-8 -*-

from ctypes import *
import sys
import time
from pathlib import Path
sys.path.append(str(Path(__file__).parent))
sys.path.append('.')
sys.path.append('..')
import pyjlink_dev as jlink
from fmu0 import *
from fmu1 import *
JLINK_SERIAL_NUMBER = None

g_reg_dic_list = [FMU0_REG,  FMU1_REG]#PF DF

ENCODE_KEY = 0x464c4f<<8
g_ctrl_d = {
'read' : 0,
'info_read' : 1, 
'info_write':2, 
'write':3, 
'info_sector_erase':4, 
'sector_erase':5, 
'chip_erase':6, 
'info_half_erase':7,
'half_erase':8,
'progreg':10, 
'read_shadow':11, 
'write_shadow':12, 
}

class class_flash():
    swd_handle = ''
    reg_dic = ''
    rw_length = 4 #4byte 1word for mx02
    slot_number = 128 #1 sector include 128 slot
    instance = ''
    flash_size = 128 * 1024 #unit byte

    def __init__(self, swd_handle, instance):
        if (swd_handle == ''):
            JLINK_SERIAL_NUMBER = None
            self.swd_handle = jlink.JLINK_CLASS(JLINK_SERIAL_NUMBER)
        else:
            self.swd_handle = swd_handle
            self.comm = swd_handle
            
        self.instance = instance
        self.reg_dic = g_reg_dic_list[instance]
    
    def reg_change(self, reg_dict, value):
        self.swd_handle.swd_reg_change_dict(reg_dict, value)
        
    def reg_read(self, reg_bit_dic):
        data32 = self.swd_handle.swd_reg_read(reg_bit_dic["address"])
        data = (data32>>reg_bit_dic["start_bit"]) & ((0x1<<reg_bit_dic["length"])-1) 
        
        return data
        
    def unlock(self):
        self.swd_handle.swd_reg_write(0x40007014, 0x100)#unlock
        
    def lock(self):
        self.swd_handle.swd_reg_write(0x40007014, 0x000)#unlock
        
    def flag_clear(self):
        self.swd_handle.swd_reg_write(0x4000701C, 0xFFFFFFFF)#clear all flag    
        
    def wait(self, wait_cnt = 40000):
        wait_idx = 0
        flash_idle_flag = 0
        while (wait_idx < wait_cnt):
            read_data = self.reg_read(self.reg_dic['FCINTSTS_COMPLETE_STS'])
            if (read_data):
                flash_idle_flag = 1
                wait_idx = wait_cnt
            wait_idx = wait_idx + 1
            
        if (flash_idle_flag==0):
            print("flash is always busy!!!!!!!\n")
    
        return flash_idle_flag
        
    def read(self, addr_start, read_length):#length size is byte 
        self.unlock()
        addr = addr_start
        read_data_length = self.rw_length
        addr_end = addr_start+read_length*read_data_length
        read_data = []
        while (addr < addr_end):
            self.flag_clear()
            #print("addr%x",int(addr))
            #flash controller work as word unit,APB read/write need to divide 4
            self.swd_handle.swd_reg_write(0x40007004, int(addr/read_data_length))
            #self.swd_handle.swd_reg_write(0x40007008, 0)
   
            self.swd_handle.swd_reg_write(0x40007000, ENCODE_KEY|(g_ctrl_d['read']))
            time.sleep(0.001)
            read_data0 = self.swd_handle.swd_reg_read(0x40007008)

            read_data.append(read_data0)

            addr = addr + read_data_length
        self.lock()
        return read_data
        
    def read_by_ahb(self, addr_start, read_length): #length size is word 
        read_data = []
#        self.comm.swd_reg_write(0x4000000C, 0)#un-remap
        read_data= self.swd_handle.swd_get_mem32(addr_start,  read_length)
        return read_data
        
    def erase_check(self):
        # self.base_cfg()
        # for sec_num in range(128):
        #     check_flag = self.sector_pattern_read(sec_num, 1)
        #     if (check_flag == 0):
        #         self.base_cfg_recover()
        #         return check_flag
        # self.base_cfg_recover()

        read_length = self.flash_size >> 2
        addr_start = self.instance*read_length
        
        read_data = self.read_by_ahb(addr_start, read_length)

        for data in read_data:
            if (data != 0xffffffff) :
                print(hex(data))
                return 0
        check_flag = 1

        return check_flag  
        
    def chip_erase(self):
        self.unlock()
        self.flag_clear()
        self.swd_handle.swd_reg_write(0x40007004, 0)
        # print(self.reg_dic['FCCTRL_CTRL']['address'])
        self.swd_handle.swd_reg_write(0x40007000, ENCODE_KEY|(g_ctrl_d['chip_erase']))

        #1/self.wait(40000)
        time.sleep(0.02)
        #check_flag = 1
        #check_flag = self.erase_check()

        self.lock()
    
    def write(self, addr, data_list):
        write_length = self.rw_length #byte
        self.unlock()
        for data in data_list:
            self.flag_clear()
            self.swd_handle.swd_reg_write(0x40007004, int(addr/write_length))
            self.swd_handle.swd_reg_write(0x40007008, int(data))
            self.swd_handle.swd_reg_write(0x40007000, ENCODE_KEY|(g_ctrl_d['write']))

            addr = addr + write_length
            #1/self.wait(10000)
            time.sleep(0.001)
        self.lock()
    
    def chip_sector_erase(self,addr):
        write_length = self.rw_length #byte
        self.unlock()
        self.flag_clear()
        #self.swd_handle.swd_reg_write(0x40007004, 0)
        self.swd_handle.swd_reg_write(0x40007004, int(addr/write_length))
        # print(self.reg_dic['FCCTRL_CTRL']['address'])
        self.swd_handle.swd_reg_write(0x40007000, ENCODE_KEY|(g_ctrl_d['sector_erase']))
        print("flash sector erase success!\n")
        #1/self.wait(40000)
        time.sleep(0.02)
        #check_flag = 1
        #check_flag = self.erase_check()

        self.lock()
    
    def sector_erase(self, start_sector, sector_num):
        self.unlock()
        self.flag_clear()
        slot_number = self.slot_number #byte      
        sector_idx = start_sector
        print("start erase sector!")
        while (sector_idx < (start_sector + sector_num)):
            self.swd_handle.swd_reg_write(self.reg_dic['FCADDR_FCADDR_L']['address'], sector_idx*slot_number)
            self.swd_handle.swd_reg_write(self.reg_dic['FCDATA0H_FCDATA0H']['address'], 0)
            self.swd_handle.swd_reg_write(self.reg_dic['FCDATA0L_FCDATA0L']['address'], 0)
            self.swd_handle.swd_reg_write(self.reg_dic['FCDATA1H_FCDATA1H']['address'], 0)
            self.swd_handle.swd_reg_write(self.reg_dic['FCDATA1L_FCDATA1L']['address'], 0)
            # print(self.reg_dic['FCCTRL_CTRL']['address'])
            self.swd_handle.swd_reg_write(self.reg_dic['FCCTRL_CTRL']['address'], ENCODE_KEY|(g_ctrl_d['sector_erase']))
     
            1/self.wait(40000) 
                      
            # flag = self.sector_pattern_read(sector_idx, 1)
            # if (flag == 1):
            #     print("sector erase success!\n")
            # else:
            #     print("sector erase fail!\n")

            sector_idx = sector_idx + 1
          
        self.lock()
        
def flash0_chip_erase(user_input):
    flash0.chip_erase()
    print("chip erase success!\n")

def flash1_chip_erase(user_input):
    flash1.chip_erase()
    print("chip erase success!\n")


    
def flash0_read(user_input):
    addr_start = int(user_input[1], 16)
    length = int(user_input[2], 16)
    result = flash0.read(addr_start, length)
    if (result != []):
        print([hex(data) for data in result])
    else:
        print("fail")
        
def flash0_write(user_input):
    addr_start = int(user_input[1], 16)
    input_data = [int(data, 16) for data in user_input[2:]]
#    addr_start = 0
#    input_data = [1, 2, 3, 4]
    flash0.write(addr_start, input_data)
    
def flash1_write(user_input):
    addr_start = int(user_input[1], 16)
    input_data = [int(data, 16) for data in user_input[2:]]
#    addr_start = 0
#    input_data = [1, 2, 3, 4]
    flash1.write(addr_start, input_data)   
    
def flash0_read_ahb(user_input):
    addr_start = int(user_input[1], 16)
    length = int(user_input[2], 16)
    result = flash0.read_by_ahb(addr_start, length)
    if (result != []):
        print([hex(data) for data in result])
    else:
        print("fail")
        
def flash1_read(user_input):
    addr_start = int(user_input[1], 16)
    length = int(user_input[2], 16)
    result = flash1.read(addr_start, length)
    if (result != []):
        print([hex(data) for data in result])
    else:
        print("fail")
        
def flash_read_ahb(user_input):
    addr_start = int(user_input[1], 16)
    length = int(user_input[2], 16)
    result = flash1.read_by_ahb(addr_start, length)
    if (result != []):
        print([hex(data) for data in result])
    else:
        print("fail") 
        
if __name__ == "__main__":
    swd_handle = jlink.JLINK_CLASS(None)
    swd_handle.swd_halt()
    flash0 = class_flash_144(swd_handle, 0)
    flash1 = class_flash_144(swd_handle, 1)
    while(1):
        user_input = input("input your choice:\n \
                1: flash 0 chip_erase \n \
                2: flash 1 chip_erase \n \
                3: flash 0 read by apb(para: start_addr num(unit:4 words)) \n \
                4: flash 1 read by apb(para: start_addr num(unit:4 words)) \n \
                5: flash 0 write by apb(para: start_addr  write_list) \n \
                6: flash 1 write by apb(para: start_addr  write_list) \n \
                7: flash reaad by apb(para: start_addr  num(unit:word)) \n \
                others and invalid input: exit\n \
                \n>")
        user_input = user_input.split(" ")       
        user_input += ["0"]*3 
        switch = {
                    "1": flash0_chip_erase,
                    "2": flash1_chip_erase,
                    "3": flash0_read, 
                    "4": flash1_read, 
                    '5': flash0_write, 
                    '6': flash1_write, 
                    '7': flash_read_ahb, 
        }
        try:
            switch[user_input[0]](user_input)
        except Exception as e:
            print(e)
            print("invalid input!")



