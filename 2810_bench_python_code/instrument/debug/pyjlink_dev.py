# -*- coding: utf-8 -*-

"""
Module implementing MainWindow.
"""
import pylink

#1) install Jlink Driver:JLink_Windows_V634b.exe
#2) pip install pylink-square
#3) pylink user manual : https://pylink.readthedocs.io/en/latest/pylink.html
#4) refer below link to get how to parse the current jlink serial number:https://blog.csdn.net/xbzlxtz/article/details/91490402
JLINK_SERIAL_NUMBER = None  #this serial number may has different value for different JLINK simulator


class JLINK_CLASS:
    
    jlink_serial_number = ''
    jlink_handle = ''
    
    def __init__(self, new_jlink_serial_number):
        self.jlink_serial_number = new_jlink_serial_number
        self.swd_connect()
        
    def swd_connect(self):
        self.jlink_handle = pylink.JLink()
        self.jlink_handle.open(self.jlink_serial_number) #open jlink by the correspond jlink serial number
        self.jlink_handle.set_tif(pylink.enums.JLinkInterfaces.SWD)
        
        self.jlink_handle.connect('Cortex-M0')
        if self.jlink_handle.target_connected() == True:
            #print("JLINK connect success, core_id=",  end='')
            #print(hex(self.jlink_handle.core_id()).upper())
            return 0
        else:
            self.jlink_handle = ''
            return -1
            
    def swd_get_mem32(self,  addr,  len):
        result = []
        if (self.jlink_handle != ''):
            result = self.jlink_handle.memory_read32(addr, len)
            
        return result
            
    def swd_set_mem32(self,  addr,  data_list):
        if (self.jlink_handle != ''):
            self.jlink_handle.memory_write32(addr,  data_list)
            
    #This method resets the target, and by default toggles the RESET and TRST pins.
    def swd_pin_reset(self):
        if (self.jlink_handle != ''):
            #ms (int) – Amount of milliseconds to delay after reset (default: 0)
            #halt (bool) – if the CPU should halt after reset (default: True)
            self.jlink_handle.reset(10,  False)
            
    def swd_halt(self):
        result = -1
        if (self.jlink_handle != ''):
            self.jlink_handle.halt()
            if (self.jlink_handle.halted() == True):
                print("MCU halted now")
                result = 0
            else:
                print("MCU halted fail")
                
        return result
        
    def swd_run(self):
        DHCSR_REG = 0xe000edf0 
        
        DHCSR_KEY = 0xa05f0000
        DHCSR_DEBUGEN_BIT = 0x00000001
        
        DHCSR_DEBUGEN = DHCSR_KEY | DHCSR_DEBUGEN_BIT
        
        if (self.jlink_handle != ''):    
            if (self.jlink_handle.halted() == True):
                self.jlink_handle.memory_write32(DHCSR_REG,  [DHCSR_DEBUGEN])
                
            print("MCU run again")
            
    def swd_get_reg_all(self):
        result = ''
        REGISTERS = [
            'R0', 'R1', 'R2', 'R3', 'R4', 'R5',
            'R6', 'R7', 'R8', 'R9', 'R10', 'R11', 'R12',
            'R13 (SP)', 'R14', 'R15 (PC)']
            
        if (self.swd_halt() == 0):
            register_values = self.jlink_handle.register_read_multiple(REGISTERS)
            for i, data in enumerate(register_values):
                info = REGISTERS[i] + " = " + hex(data) + "\n"
                result += info
                
        self.swd_run() #run mcu again after read all the internal registers' value
        
        return result
        
    def swd_set_reg_pc(self, data = 0x00000000):
        
        if (self.swd_halt() == 0):
            self.jlink_handle.register_write('R15 (PC)', 0x00000000)

        #self.swd_run() #run mcu again after read all the internal registers' value
        
    def swd_reset(self):
        DEMCR_REG = 0xe000edfc 
        AIRCR_REG = 0xe000ed0c
        
        AIRCR_KEY = 0x05fa0000
        AIRCR_SYSRESETREQ_BIT = 0x00000004
        AIRCR_SYSRESETREQ = AIRCR_KEY | AIRCR_SYSRESETREQ_BIT
        
        DEMCR_RUN_AFTER_RESET = 0x00000000
        
        if (self.jlink_handle != ''):    
            """Reset"""
            print("MCU NVIC reset now")
            self.jlink_handle.memory_write32(DEMCR_REG, [DEMCR_RUN_AFTER_RESET])
            self.jlink_handle.memory_write32(AIRCR_REG, [AIRCR_SYSRESETREQ])
            
    def swd_step(self):
        DHCSR_REG = 0xe000edf0
        
        DHCSR_KEY = 0xa05f0000
        DHCSR_DEBUGEN_BIT = 0x00000001
        DHCSR_STEP_BIT = 0x00000004
        DHCSR_STEP = DHCSR_KEY | DHCSR_DEBUGEN_BIT | DHCSR_STEP_BIT
        if (self.jlink_handle != ''):    
            print("MCU step debug")
            self.jlink_handle.memory_write32(DHCSR_REG, [DHCSR_STEP])
        
    def swd_reg_change(self, reg_address,start_bit, length, write_data):
        read_list = self.swd_get_mem32(reg_address, 1)
        read_buf = read_list[0]
        clr = ~(((1<<length)-1)<< start_bit)
        set = ((((1<<length)-1)&write_data)<< start_bit)
        read_buf = read_buf&clr
        data_list = [(read_buf|set)]
        self.swd_set_mem32(reg_address, data_list)
        
    def swd_reg_change_dict(self,change_dict, write_data):
        reg_address =change_dict.get("address")
        start_bit = change_dict.get("start_bit")
        length = change_dict.get("length")
        self.swd_reg_change(reg_address, start_bit, length, write_data)
        
    def swd_reg_write(self,reg_address, write_data):
        data_list = []
        data_list.append(write_data)
        self.swd_set_mem32(reg_address, data_list)
        
    def swd_reg_read(self, reg_address):
        read_list = self.swd_get_mem32(reg_address, 1)
        read_buf = read_list[0]
        return read_buf
    
if __name__ == "__main__":
    jlink_instance = JLINK_CLASS(None)
    
    if (jlink_instance.jlink_handle == ''):
        print("JLINK connect fail")
    else:
#        #1 memory r/w test
#        jlink_instance.swd_set_mem32(0x20004000,  [0x11223344])
#        result = jlink_instance.swd_get_mem32(0x20004000,  4)
#        
#        #2 CPU register read
#        print(jlink_instance.swd_get_reg_all())
        while (1):
            swd_options = input("input your swd option:\n \
            get: read memory(other parameters: add len) \n \
            set: write memory(other parameters: add data_list)\n \
            get_reg_all: show all of the cortex-m registers value \n \
            nvic_r: reset mcu by NVIC system reset\n \
            pin_r: reset mcu by toggle the swd RST pin\n \
            halt: mcu halt\n \
            step: mcu step\n \
            pc: mcu pc\n \
            run: mcu run\n:>")
            swd_options = swd_options.split(" ")
            if (swd_options[0] == "get"):
                print("read_addr = 0x%08x, values = " % int(swd_options[1],  16))
                result = jlink_instance.swd_get_mem32(int(swd_options[1],  16),  int(swd_options[2]))
                if (result != []):
                    print([hex(data) for data in result])
                else:
                    print("fail")
            elif (swd_options[0] == "set"):
                data_list = [int(data, 16) for data in swd_options[2:]]
                jlink_instance.swd_set_mem32(int(swd_options[1],  16),  data_list)
            elif (swd_options[0] == "get_reg_all"):
                print(jlink_instance.swd_get_reg_all())
            elif (swd_options[0] == "nvic_r"):
                jlink_instance.swd_reset()
            elif (swd_options[0] == "pin_r"):
                jlink_instance.swd_pin_reset()
            elif (swd_options[0] == "halt"):
                jlink_instance.swd_halt()
            elif (swd_options[0] == "step"):
                jlink_instance.swd_step()
            elif (swd_options[0] == "run"):
                jlink_instance.swd_run()
            elif (swd_options[0] == "pc"):
                jlink_instance.swd_run()
            else:
                print("un-supported cmd")
            
            
