# -*- coding: utf-8 -*-

from ctypes import *
import sys
import time
sys.path.append('.')
sys.path.append('..')
import pyjlink_dev as jlink
from fmu0 import *
from fmu1 import *
JLINK_SERIAL_NUMBER = None

write_info_low_dic = {"info_0":0x00000000, 
                    "info_1":0x00000000, 
                    "info_2":0x00000000, 
                    "info_3":0x00000000, 
                    "info_4":0x00000000,
                    "info_5":0xff000000, 
                    "info_6":0x00000000,
                    "info_7":0x00000000,
                    "info_8":0x00000000}
write_info_high_dic = {"info_0":0x07000000, 
                    "info_1":0x00000000, 
                    "info_2":0x00000000, 
                    "info_3":0x00000000, 
                    "info_4":0x00000000,
                    "info_5":0x00000fff, 
                    "info_6":0x00000000,
                    "info_7":0x00000000,
                    "info_8":0x00000000}

write_info_low = list(write_info_low_dic.values())
write_info_high = list(write_info_high_dic.values())

g_info_n =[0, 1, 2, 3, 4, 8]
g_reg_dic_list = [FMU0_REG,  FMU1_REG]#PF DF
g_check_sum0 = [0x72677973, 0x7778797a, 0x72677973]
g_check_sum1 = [0x73696c65, 0x61626364, 0x73696c65]
ENCODE_KEY = 0x464c4f<<8
g_ctrl_d = {
'read' : 0,
'info_read' : 1, 
'info_write':2, 
'write':3, 
'info_erase':4, 
'erase':5, 
'chip_erase':6, 
'info_half_erase':7,
'half_erase':8,
'progreg':10, 
'shadow_read':11, 
'shadow_write':12,
}

class class_code_flash_info():
    swd_handle = ''
    reg_dic = ''
    rw_length = 4 # 4byte 1word
    # info_count = 7
    instance = 0

    def __init__(self, swd_handle, instance, sector_num, info_slot_size):
        if swd_handle == '':
            JLINK_SERIAL_NUMBER = None
            self.swd_handle = jlink.JLINK_CLASS(JLINK_SERIAL_NUMBER)
        else:
            self.swd_handle = swd_handle
            
        self.reg_dic = g_reg_dic_list[instance]
        self.instance = instance
        self.sector_num = sector_num
        self.shadow_offset = (sector_num+1) * 0x80
        self.used_slot_size = info_slot_size

    def reg_change(self, reg_dict, value):
        self.swd_handle.swd_reg_change_dict(reg_dict, value)
        
    def reg_read(self, reg_bit_dic):
        data32 = self.swd_handle.swd_reg_read(reg_bit_dic["address"])
        data = (data32 >> reg_bit_dic["start_bit"]) & ((0x1 << reg_bit_dic["length"])-1)
        return data
        
    def unlock(self):
        # unlock
        self.reg_change(self.reg_dic["FCPROT_RW_UNLOCK"], 1)
        
    def lock(self):
        # lock
        self.reg_change(self.reg_dic["FCPROT_RW_UNLOCK"], 0)
    
    def flag_clear(self):
        # clear all flag
        self.swd_handle.swd_reg_write(self.reg_dic['FCINTSTS_COMPLETE_STS']['address'], 0xFFFFFFFF)
        
    def wait(self, wait_cnt=40000):
        wait_idx = 0
        flash_idle_flag = 0
        while wait_idx < wait_cnt:
            read_data = self.reg_read(self.reg_dic['FCINTSTS_COMPLETE_STS'])
            if read_data:
                flash_idle_flag = 1
                wait_idx = wait_cnt
            wait_idx = wait_idx + 1
            
        if flash_idle_flag == 0:
            print("flash is always busy!!!!!!!\n")
    
        return flash_idle_flag  
        
    def read(self):
        # print("start read\n")
        self.unlock()

        read_info_0 = []
        
#        print('%x'%self.reg_dic["FCCTRL_CTRL"]['address'])
        for info_idx in range(self.used_slot_size):
            self.flag_clear()
            self.reg_change(self.reg_dic["FCADDR_FCADDR_L"], self.shadow_offset + info_idx)#addr
            self.reg_change(self.reg_dic["FCADDR_FCADDR_H"], 1)#addr 1 :info 2:shadow
            # self.reg_change(self.reg_dic['FCDATA0L_FCDATA0L'],0)
            # self.reg_change(self.reg_dic['FCDATA1L_FCDATA1L'],0)
            self.swd_handle.swd_reg_write(self.reg_dic["FCCTRL_CTRL"]['address'], ENCODE_KEY|(g_ctrl_d['info_read']))
            
            1/self.wait(40000) 
            read_data0 = self.reg_read(self.reg_dic["FCDATA0L_FCDATA0L"])
            # read_data1 = self.reg_read(self.reg_dic["FCDATA1L_FCDATA1L"])
            
            read_info_0.append(read_data0) 
            # read_info_1.append(read_data1)
        
        self.lock()
        return read_info_0

    def write(self, info_0):
         # cal checksum
        write_info_0 = list(info_0)

        self.unlock()
        # print('write.shadow_offset=%#x'%self.shadow_offset)
        # print('self.used_slot_size=%#x'%self.used_slot_size)
#        print('%x'%self.reg_dic["FCCTRL_CTRL"]['address'])
        for info_idx in range(self.used_slot_size):
            self.reg_change(self.reg_dic['FCADDR_FCADDR_L'], self.shadow_offset + info_idx)#addr
            self.reg_change(self.reg_dic['FCADDR_FCADDR_H'], 1)#addr 1 :info 2:shadow
            self.reg_change(self.reg_dic['FCDATA0L_FCDATA0L'], write_info_0[info_idx])
            # self.reg_change(self.reg_dic['FCDATA1L_FCDATA1L'],write_info_1[info_idx])
 
            self.flag_clear()
            
            self.swd_handle.swd_reg_write(self.reg_dic['FCCTRL_CTRL']['address'], ENCODE_KEY|(g_ctrl_d['info_write']))
            1/self.wait(40000) 
            info_idx = info_idx + 1
            
        # print("load info array into the shadow registers\n") 
        self.read() # need to be read before refresh
        self.reg_change(self.reg_dic['FCCTRL_REFRESH'], 1)#refresh to shadow    
        # print("write flash info finish!\n") 
        self.lock()
        
    def change(self, info_n, start_bit, length, data):
        (write_info_0, write_info_1) =  self.read()
        if (start_bit<32):
            clr = ~(((1<<length)-1)<< start_bit)
            set = ((((1<<length)-1)&data)<< start_bit)
            write_info_0[info_n] = write_info_0[info_n]&clr
            write_info_0[info_n] = write_info_0[info_n]|set
        else:
            start_bit = start_bit-32
            clr = ~(((1<<length)-1)<< start_bit)
            set = ((((1<<length)-1)&data)<< start_bit)
            write_info_1[info_n] = write_info_1[info_n]&clr
            write_info_1[info_n] = write_info_1[info_n]|set
        self.sector_erase()
        self.write(write_info_0, write_info_1)

    # invert [31:0] to [63:32] and copy slot_n to slot_n+1
    def info_change(self, info_n, start_bit, length, data):
        (write_info_0) = self.read()
        data_invert = ~data & 0xFFFF
        if start_bit < 16:
            clr = ~((((1 << length)-1) << start_bit) | (((1 << length)-1) << (start_bit+16)))
            set_value = ((((1 << length)-1) & data) << start_bit)
            set_invert = ((((1 << length)-1) & data_invert) << (start_bit+16))
            write_info_0[info_n] = write_info_0[info_n] & clr
            write_info_0[info_n] = write_info_0[info_n] | set_value | set_invert
            write_info_0[info_n+1] = write_info_0[info_n]
            write_info_0[info_n+1] = write_info_0[info_n]
            # print([hex(num) for num in write_info_0])
            # print([hex(num) for num in write_info_1])                  
        self.sector_erase()
        self.write(write_info_0)

    def shadow_reg_write(self, info_0):
        write_info_0 = list(info_0) 

        self.unlock()
        
#        print('%x'%self.reg_dic["FCCTRL_CTRL"]['address'])
        for info_idx in range(self.used_slot_size):
            self.reg_change(self.reg_dic['FCADDR_FCADDR_L'], self.shadow_offset + info_idx)#addr
            self.reg_change(self.reg_dic['FCADDR_FCADDR_H'], 2)#addr 1 :info 2:shadow
            self.reg_change(self.reg_dic['FCDATA0L_FCDATA0L'],write_info_0[info_idx])

            self.flag_clear()
            
            self.swd_handle.swd_reg_write(self.reg_dic['FCCTRL_CTRL']['address'], ENCODE_KEY|(g_ctrl_d['shadow_write']))
            1/self.wait(40000) 
            info_idx = info_idx + 1
            
        self.lock()
        # print("write shadow registers finish!\n")    
        
    def shadow_reg_read(self):
        # print("start read\n")
        self.unlock()

        read_info_0 = []

        # print('%x'%self.shadow_offset)
        for info_idx in range(self.used_slot_size):
            self.flag_clear()
            self.reg_change(self.reg_dic["FCADDR_FCADDR_L"], self.shadow_offset + info_idx)#addr
            self.reg_change(self.reg_dic["FCADDR_FCADDR_H"], 2)#addr 1 :info 2:shadow
            # self.reg_change(self.reg_dic['FCDATA0L_FCDATA0L'],0)

            self.swd_handle.swd_reg_write(self.reg_dic["FCCTRL_CTRL"]['address'], ENCODE_KEY|(g_ctrl_d['shadow_read']))
            
            1/self.wait(40000) 
            read_data0 = self.reg_read(self.reg_dic["FCDATA0L_FCDATA0L"])
            # read_data1 = self.reg_read(self.reg_dic["FCDATA1L_FCDATA1L"])
            read_info_0.append(read_data0) 
            # read_info_1.append(read_data1)
        self.lock()
        return read_info_0

    # write_info_0，start_bit 是修改的起始位，length是修改的长度，data是要修改的值
    def shadow_reg_change(self, info_n, start_bit, length, data):
        write_info_0 = self.shadow_reg_read()
        # invert bit16-31
        for index in range(len(write_info_0)):
            write_info_0[index] = write_info_0[index]&0xFFFF
            write_info_0[index] |= (((~write_info_0[index])&0xFFFF)<<16)

        data_invert = ~data & 0xFFFF
        if start_bit < 16:
            clr = ~((((1 << length) - 1) << start_bit) | (((1 << length) - 1) << (start_bit + 16)))
            set_value = ((((1 << length) - 1) & data) << start_bit)
            set_invert = ((((1 << length) - 1) & data_invert) << (start_bit + 16))
            write_info_0[info_n] = write_info_0[info_n] & clr
            write_info_0[info_n] = write_info_0[info_n] | set_value | set_invert
            write_info_0[info_n+1] = write_info_0[info_n]
            write_info_0[info_n+1] = write_info_0[info_n]
        # print([hex(num) for num in write_info_0])

        self.shadow_reg_write(write_info_0)

    def sector_erase(self): 
        #info erase...
        self.unlock()
        self.flag_clear()

        self.reg_change(self.reg_dic["FCADDR_FCADDR_L"], self.shadow_offset)#addr
        self.reg_change(self.reg_dic["FCADDR_FCADDR_H"], 1)#addr[30-31] bit30=1 :info   bit32=1 :shadow
        self.swd_handle.swd_reg_write(self.reg_dic['FCCTRL_CTRL']['address'], ENCODE_KEY|(g_ctrl_d['info_erase']))
        
        1/self.wait(40000)         
            
        self.lock()  

    def refresh(self):
        self.unlock()
        self.read() # need to be read before refresh
        self.reg_change(self.reg_dic['FCCTRL_REFRESH'], 1)#refresh to shadow  
        self.lock()
    
        
    def mx02_flash_info_version_write(self, addr, data_list):
        self.unlock()
        for data in data_list:
            self.flag_clear()
            self.swd_handle.swd_reg_write(0x40007004, int(addr))
            self.swd_handle.swd_reg_write(0x40007008, int(data))
            self.swd_handle.swd_reg_write(0x40007000, ENCODE_KEY|(g_ctrl_d['info_write']))

            addr = addr + 1
            #1/self.wait(10000)
            time.sleep(0.001)
        self.lock()
    
    def mx02_flash_info_version_read(self, addr_start, read_length):#length size is byte 
        self.unlock()
        addr = addr_start
        read_data_length = self.rw_length
        addr_end = addr_start+read_length
        read_data = []
        while (addr < addr_end):
            self.flag_clear()
            #print("addr%x",int(addr))
            #flash controller work as word unit,APB read/write need to divide 4
            self.swd_handle.swd_reg_write(0x40007004, int(addr))
            #self.swd_handle.swd_reg_write(0x40007008, 0)
   
            self.swd_handle.swd_reg_write(0x40007000, ENCODE_KEY|(g_ctrl_d['info_read']))
            time.sleep(0.005)
            read_data0 = self.swd_handle.swd_reg_read(0x40007008)

            read_data.append(read_data0)

            addr = addr + 1
        self.lock()
        return read_data
    
    def chip_sector_erase(self,addr):
        write_length = self.rw_length #byte
        self.unlock()
        self.flag_clear()
        #self.swd_handle.swd_reg_write(0x40007004, 0)
        self.swd_handle.swd_reg_write(0x40007004, int(addr/write_length))
        # print(self.reg_dic['FCCTRL_CTRL']['address'])
        self.swd_handle.swd_reg_write(0x40007000, ENCODE_KEY|(g_ctrl_d['info_erase']))
        print("flash info erase success!\n")
        #1/self.wait(40000)
        time.sleep(0.02)
        #check_flag = 1
        #check_flag = self.erase_check()

        self.lock()
    # def refresh(self):
    #     self.unlock()
    #     self.read() # need to be read before refresh
    #     self.reg_change(self.reg_dic['FCCTRL_REFRESH'], 1)#refresh to shadow  
    #     self.lock()
    

class class_flash_info():# only for mx03 now
    swd_handle = ''
    reg_dic = ''
    rw_length = 8 #8byte 2word 
    #info_count = 7
    instance = 0
    info_idx_list = [0, 1, 2, 3, 4, 5, 6, 7 ,8]

    def __init__(self, swd_handle, instance):
        if (swd_handle == ''):
            JLINK_SERIAL_NUMBER = None
            self.swd_handle = jlink.JLINK_CLASS(JLINK_SERIAL_NUMBER)
        else:
            self.swd_handle = swd_handle
            
        self.reg_dic = g_reg_dic_list[instance]
        self.instance = instance
    def reg_change(self, reg_dict, value):
        self.swd_handle.swd_reg_change_dict(reg_dict, value)
        
    def reg_read(self, reg_bit_dic):
        data32 = self.swd_handle.swd_reg_read(reg_bit_dic["address"])
        data = (data32>>reg_bit_dic["start_bit"])& ((0x1<<reg_bit_dic["length"])-1) 
        return data
        
    def unlock(self):
        self.reg_change(self.reg_dic["FCPROT_RW_UNLOCK"], 1)#unlock
        
    def lock(self):
        self.reg_change(self.reg_dic["FCPROT_RW_UNLOCK"], 0)#lock 
    
    def flag_clear(self):
        self.swd_handle.swd_reg_write(self.reg_dic['FCINTSTS_COMPLETE_STS']['address'], 0xFFFFFFFF)#clear all flag
        
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
        
    def read(self):
#        print("start read\n")
        self.unlock()

        read_info_0 = []
        read_info_1 = []
        info_idx_list = self.info_idx_list[:-1]
        # print(info_idx_list)
#        print('%x'%self.reg_dic["FCCTRL_CTRL"]['address'])
        for info_idx in info_idx_list:
            
            self.flag_clear()
            self.reg_change(self.reg_dic["FCADDR_FCADDR_L"], info_idx)#addr
            self.reg_change(self.reg_dic["FCADDR_FCADDR_H"], 1)#addr 1 :info 2:shadow
            self.reg_change(self.reg_dic['FCDATA0L_FCDATA0L'],0)
            self.reg_change(self.reg_dic['FCDATA1L_FCDATA1L'],0)
            self.swd_handle.swd_reg_write(self.reg_dic["FCCTRL_CTRL"]['address'], ENCODE_KEY|(g_ctrl_d['info_read']))
            
            1/self.wait(40000) 
            read_data0 = self.reg_read(self.reg_dic["FCDATA0L_FCDATA0L"])
            read_data1 = self.reg_read(self.reg_dic["FCDATA1L_FCDATA1L"])
            
            read_info_0.append(read_data0) 
            read_info_1.append(read_data1)
        
        self.lock()
        return read_info_0, read_info_1
            
    def write(self, info_0,info_1):
         #cal checksum
        write_info_0 = list(info_0) 
        write_info_1 = list(info_1)
        checksum_DATA0 = g_check_sum0[self.instance]
        checksum_DATA1 = g_check_sum1[self.instance]
        for data in write_info_0:
            checksum_DATA0 = checksum_DATA0^data
        for data in write_info_1:
            checksum_DATA1 = checksum_DATA1^data
        write_info_0.append(checksum_DATA0) 
        write_info_1.append(checksum_DATA1)

            
        self.unlock()
        info_idx_list = self.info_idx_list
#        print('%x'%self.reg_dic["FCCTRL_CTRL"]['address'])
        for info_idx in info_idx_list:
            # print(info_idx)
            self.reg_change(self.reg_dic['FCADDR_FCADDR_L'], info_idx)#addr
            self.reg_change(self.reg_dic['FCADDR_FCADDR_H'], 1)#addr 1 :info 2:shadow
            self.reg_change(self.reg_dic['FCDATA0L_FCDATA0L'],write_info_0[info_idx])
            self.reg_change(self.reg_dic['FCDATA1L_FCDATA1L'],write_info_1[info_idx])

            self.flag_clear()
            
            self.swd_handle.swd_reg_write(self.reg_dic['FCCTRL_CTRL']['address'], ENCODE_KEY|(g_ctrl_d['info_write']))
            1/self.wait(40000) 
            
        print("load info array into the shadow registers\n") 
        self.reg_change(self.reg_dic['FCCTRL_REFRESH'], 1)#refresh to shadow
        print("write flash info finish!\n") 
        self.lock()
        
        
    def change(self, info_n, start_bit,length, data):
        (write_info_0, write_info_1) =  self.read()
        if (start_bit<32):
            clr = ~(((1<<length)-1)<< start_bit)
            set = ((((1<<length)-1)&data)<< start_bit)
            write_info_0[info_n] = write_info_0[info_n]&clr
            write_info_0[info_n] = write_info_0[info_n]|set
        else:
            start_bit = start_bit-32
            clr = ~(((1<<length)-1)<< start_bit)
            set = ((((1<<length)-1)&data)<< start_bit)
            write_info_1[info_n] = write_info_1[info_n]&clr
            write_info_1[info_n] = write_info_1[info_n]|set
        self.erase()
        self.write(write_info_0, write_info_1)
        
    def shadow_reg_write(self, info_0, info_1):
        write_info_0 = list(info_0) 
        write_info_1 = list(info_1) 
         #cal checksum
        checksum_DATA0 = g_check_sum0[self.instance]
        checksum_DATA1 = g_check_sum1[self.instance]
        for data in write_info_0:
            checksum_DATA0 = checksum_DATA0^data
        for data in write_info_1:
            checksum_DATA1 = checksum_DATA1^data

        write_info_0.append(checksum_DATA0) 
        write_info_1.append(checksum_DATA1)

            
        self.unlock()
        info_idx_list = self.info_idx_list + [self.info_idx_list[-1]+1]
#        print('%x'%self.reg_dic["FCCTRL_CTRL"]['address'])
        for info_idx in info_idx_list:
            self.reg_change(self.reg_dic['FCADDR_FCADDR_L'], g_info_n[info_idx])#addr
            self.reg_change(self.reg_dic['FCADDR_FCADDR_H'], 2)#addr 1 :info 2:shadow
            self.reg_change(self.reg_dic['FCDATA0L_FCDATA0L'],write_info_0[info_idx])
            self.reg_change(self.reg_dic['FCDATA1L_FCDATA1L'],write_info_1[info_idx])

            self.flag_clear()
            
            self.swd_handle.swd_reg_write(self.reg_dic['FCCTRL_CTRL']['address'], ENCODE_KEY|(g_ctrl_d['info_write']))
            1/self.wait(40000) 
            info_idx = info_idx + 1
            
        self.lock()
        print("write shadow registers finish!\n")    
        
    def shadow_reg_read(self):
#        print("start read\n")
        self.unlock()

        read_info_0 = []
        read_info_1 = []
        info_idx_list =  self.info_idx_list
#        print('%x'%self.reg_dic["FCCTRL_CTRL"]['address'])
        for info_idx in info_idx_list:
            self.flag_clear()
            self.reg_change(self.reg_dic["FCADDR_FCADDR_L"], info_idx)#addr
            self.reg_change(self.reg_dic["FCADDR_FCADDR_H"], 2)#addr 1 :info 2:shadow
            self.reg_change(self.reg_dic['FCDATA0L_FCDATA0L'],0)
            self.reg_change(self.reg_dic['FCDATA1L_FCDATA1L'],0)

            self.swd_handle.swd_reg_write(self.reg_dic["FCCTRL_CTRL"]['address'], ENCODE_KEY|(g_ctrl_d['info_read']))
            
            1/self.wait(40000) 
            read_data0 = self.reg_read(self.reg_dic["FCDATA0L_FCDATA0L"])
            read_data1 = self.reg_read(self.reg_dic["FCDATA1L_FCDATA1L"])
  
            
            read_info_0.append(read_data0) 
            read_info_1.append(read_data1)
            
        self.lock()
        return read_info_0, read_info_1
        
    def shadow_reg_change(self,write_info_0, write_info_1, info_n, start_bit,length, data):#write_info_0，write_info_1是读出来的值，info_n是修改的info位置，
    #start_bit 是修改的起始位，length是修改的长度，data是要修改的值
        if (start_bit<32):
            clr = ~(((1<<length)-1)<< start_bit)
            set = ((((1<<length)-1)&data)<< start_bit)
            write_info_0[info_n] = write_info_0[info_n]&clr
            write_info_0[info_n] = write_info_0[info_n]|set
        else:
            start_bit = start_bit-32
            clr = ~(((1<<length)-1)<< start_bit)
            set = ((((1<<length)-1)&data)<< start_bit)
            write_info_1[info_n] = write_info_1[info_n]&clr
            write_info_1[info_n] = write_info_1[info_n]|set
            
        self.shadow_reg_write(write_info_0,write_info_1)
 
    def erase(self): 
        #info erase...
        self.unlock()
        self.flag_clear()
        self.swd_handle.swd_reg_write(self.reg_dic['FCADDR_FCADDR_L']['address'],0)
        self.swd_handle.swd_reg_write(self.reg_dic['FCCTRL_CTRL']['address'], ENCODE_KEY|(g_ctrl_d['info_erase']))
        
        1/self.wait(40000)         
            
        self.lock()        
def flash_info_read(user_input):
    info0 ,  info1 = manu_info.read()
    for i in range(len(info0)):
        print("read info%d data: 0x%08x_%08x\n"%(i,  info1[i], info0[i])) 
        
def user_info_read(user_input):
    info0 ,  info1 = user_info.read()
    for i in range(len(info0)):
        print("read info%d data: 0x%08x_%08x\n"%(i,  info1[i], info0[i]))    
        
def flash_info_change(user_input):
    info_n = int(user_input[1], 16)
    data0 = int(user_input[2], 16)
    data1 = int(user_input[3], 16)
    manu_info.change(info_n, 0, 32, data0)
    manu_info.change(info_n, 32, 32, data1)
    flash_info_read(user_input)
    
def flash_shadow_change(user_input):
    info_n = int(user_input[1], 16)
    data0 = int(user_input[2], 16)
    data1 = int(user_input[3], 16)
    read_info_0, read_info_1 =  manu_info.shadow_reg_read()
    manu_info.shadow_reg_change(read_info_0, read_info_1 , info_n, 0, 32, data0)
    manu_info.shadow_reg_change(read_info_0, read_info_1 , info_n, 32, 32, data1)

def user_shadow_change(user_input):
    info_n = int(user_input[1], 16)
    data0 = int(user_input[2], 16)
    data1 = int(user_input[3], 16)
    read_info_0, read_info_1 =  user_info.shadow_reg_read()
    user_info.shadow_reg_change(read_info_0, read_info_1 , info_n, 0, 32, data0)
    user_info.shadow_reg_change(read_info_0, read_info_1 , info_n, 32, 32, data1)
    
def flash_info_write(user_input):
    confirm = input("please input y to comfirm\n")
    if (confirm == 'y'):
        manu_info.erase()
        manu_info.write(write_info_low, write_info_high)  

def swap_enable(user_input):
    read_info_0, read_info_1 = user_info.read()
    info_n = 0 
    data0 = 0x7|read_info_0[0]
    user_info.change(info_n, 0, 32, data0)
    user_info_read(user_input)
    
def swap_A(user_input):
    read_info_0, read_info_1 = user_info.read()
    info_n = 0 
    data0 = read_info_0[0]&(0xFFFFFF0F)
    user_info.change(info_n, 0, 32, data0)
    user_info_read(user_input)
    
def swap_B(user_input):  
    read_info_0, read_info_1 = user_info.read()
    info_n = 0 
    data0 = (0x7<<4)|read_info_0[0]
    user_info.change( info_n, 0, 32, data0)
    user_info_read(user_input)
    
def user_info_change(user_input):
    info_n = int(user_input[1], 16)
    data0 = int(user_input[2], 16)
    data1 = int(user_input[3], 16)
    user_info.change(info_n, 0, 32, data0)
    user_info.change(info_n, 32, 32, data1)
    user_info_read(user_input)
    
def fpga_erase(user_input):
    write_info_0 = [0]*10
    write_info_1 = [0]*10
    user_info.erase()
    user_info.write(write_info_0, write_info_1)
    user_info_read(user_input)
    
    write_info_0[6] = 0xff000000
    write_info_1[6] = 0x0000ffff
    
    manu_info.erase()
    manu_info.write(write_info_0, write_info_1)
    flash_info_read(user_input)
    
if __name__ == "__main__":
    swd_handle = jlink.JLINK_CLASS(None)
    swd_handle.swd_halt()
    manu_info = class_flash144_info(swd_handle, 0)
    print(id(manu_info))
    user_info = class_flash144_info(swd_handle, 1)
    print(id(user_info))
    data_info = class_flash144_info(swd_handle, 2)
    print(id(data_info))
    print("input your choice:\n \
                0: erase(fgpa) to default \n \
                1: read manufacturer info \n \
                2: change manufacturer info: para (info_n data0 data1) \n \
                3: change manufacturer shadow reg: para (info_n data0 data1) \n \
                4: manufacturer info write \n \
                5: swap info enable \n \
                6: swap info A \n \
                7: swap info B \n \
                8: read user info \n \
                9: change user info: para (info_n data0 data1) \n \
                10: change user shadow: para (info_n data0 data1) \n \
                others and invalid input: exit\n")
    while(1):
        user_input = input('>>')
        user_input = user_input.split(" ")       
        user_input += ["0"]*3 
        switch = {
                    "0": fpga_erase, 
                    "1": flash_info_read,
                    "2": flash_info_change,
                    "3": flash_shadow_change,
                    "4": flash_info_write, 
                    "5": swap_enable, 
                    "6": swap_A, 
                    "7": swap_B, 
                    "8": user_info_read, 
                    "9": user_info_change, 
                    '10':user_shadow_change, 
        }
        try:
            switch[user_input[0]](user_input)
        except Exception as e:
            print(e)
