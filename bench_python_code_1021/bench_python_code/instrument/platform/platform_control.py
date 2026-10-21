#-*- coding: utf-8 -*-
from ctypes import *
import time, sys
sys.path.append('..')
from test_platform_control.ch341_api import *
from test_platform_control.platform_enum import *

dll_path="../resource/USBIOX.DLL"
class platform_control():
    dll_handle = 0
    ch341_device_index = 0 #default open the first device
    i2c_slave_address = 0
    i2c_speed = "400KHz"
    i2c_download_mode = ""
    
    ch341_opened_flag = 0
    ch341_i2c_init_flag = 0
    
    gpio_direction = 0
    
    chip_number = "SY33518B" #  SY33518B as default 
    
    dac1_gain = 2
    
    def __init__(self):
        self.ch341_api = class_ch341_api()
        self.spi_ad5683r_gain(2)
            
    def spi_ad5683r_ramp(self, ramp_step):
        slave_address = 0xE0
        write_data_len = 10
        write_data = (c_ubyte*(write_data_len))()
        write_data[0]=0x09
        
        for i in range(5):
            write_data[i+1] = 0x00
            
        write_data[6] = 0x0f #0x0f mean  auto ramp from 0 to 0xffff
        write_data[7] = int(ramp_step/256)
        write_data[8] = int(ramp_step%256)
        write_data[9] = 0x01 #0x01 mean ad5683r
        
        self.ch341_api.ch341_i2c_write(slave_address, write_data, write_data_len)
     
    def spi_ad5683r_advanced_ramp(self, ramp_step, start, final):
        slave_address = 0xE0
        write_data_len = 10
        write_data = (c_ubyte*(write_data_len))()
        write_data[0] = 0x09
        write_data[1] = int(start/256)
        write_data[2] = int(start%256)
        write_data[3] = int(ramp_step/256)
        write_data[4] = int(ramp_step%256)
        write_data[5] = 0
        if (start<final):
            write_data[6] = 0x0f #0xf mean  auto ramp up
        else:
            if (start == final):
                write_data[6] = 0x03 #0x03 mean set here
            else:
                write_data[6] = 0x0E #0xe mean  auto ramp down
        write_data[7] = int(final/256)
        write_data[8] = int(final%256)
        write_data[9] = 0x01 #0x01 mean ad5683r
        
        self.ch341_api.ch341_i2c_write(slave_address, write_data, write_data_len)   
    def spi_ad5683r_set(self, dac_data_high, dac_data_low):
        slave_address = 0xE0
        write_data_len = 10
        write_data = (c_ubyte*(write_data_len))()
        write_data[0]=0x09
        
        for i in range(5):
            write_data[i+1] = 0x00
            
        write_data[6] = 0x03 #0x03 mean set here
        write_data[7] = dac_data_high
        write_data[8] = dac_data_low
        write_data[9] = 0x01 #0x01 mean ad5683r
        
        self.ch341_api.ch341_i2c_write(slave_address, write_data, write_data_len)
    def spi_ad5683r_gain(self, gain):
        slave_address = 0xE0
        write_data_len = 10
        write_data = (c_ubyte*(write_data_len))()
        write_data[0]=0x09
        
        for i in range(5):
            write_data[i+1] = 0x00
        write_data[6] = 0x04 #0x04 mean control here
        
        if (gain == 2):
            write_data[7] = 0x08 #0x08 mean gain = 1+1
        else :
            write_data[7] = 0x00 #0x00 mean gain = 1
            
        write_data[8] = 0x00
        write_data[9] = 0x01 #0x01 mean ad5683r
        
        self.ch341_api.ch341_i2c_write(slave_address, write_data, write_data_len)
        
    def spi_dac1_set(self, voltage):
        set_code = round(voltage/2.5/self.dac1_gain*65535)
        dac_data_low = set_code&0x00ff
        dac_data_high = (set_code&0xff00)>>8
        self.spi_ad5683r_set(dac_data_high, dac_data_low)
        
        
    def spi_dac7513_set(self, dac_data_high, dac_data_low):
        slave_address = 0xE0
        write_data_len = 10
        write_data = (c_ubyte*(write_data_len))()
        write_data[0]=0x09
        
        for i in range(5):
            write_data[i+1] = 0x00
            
        while (dac_data_high > 0x0f):
            dac_data_high = dac_data_high - 0x10
        write_data[7] = dac_data_high
        write_data[8] = dac_data_low
        write_data[9] = 0x11 #0x11 mean dac7513
        
        self.ch341_api.ch341_i2c_write(slave_address, write_data, write_data_len)
        
    def i2c_relay_init(self, slave_address):#（U2 0x40 ; U1 0x42; U3 0x44）        
        write_data_len = 3
        write_data = [0x02, 00, 00] #set gpio = 0
        self.ch341_api.ch341_i2c_write(slave_address, write_data, write_data_len)
        
        write_data = [0x06, 00, 00] #config gpio as output
        self.ch341_api.ch341_i2c_write(slave_address, write_data, write_data_len)
        self.ch341_api.ch341_i2c_write(slave_address, write_data, write_data_len)
    
    def i2c_relay_set(self, slave_address, P0, P1):#（U2 0x40 ; U1 0x42; U3 0x44） 
        write_data_len = 3
        write_data = [0x02,  P0, P1]
        self.ch341_api.ch341_i2c_write(slave_address, write_data, write_data_len)
        
    def i2c_relay_single_set(self, relay):#（U2 0x40 ; U1 0x42; U3 0x44） 
        write_data_len = 3
        if (relay.ctrl > 0x07):
            P0 = 0
            P1 = 1<<(relay.ctrl-16) 
        else:
            P0 = 1<<relay.ctrl
            P1 = 0
        write_data = [0x02,  P0, P1]
        self.ch341_api.ch341_i2c_write(relay.address, write_data, write_data_len)

if __name__ == "__main__":
    platform_control = platform_control()
    platform_control.i2c_relay_init(Relay_e.U1)
    time.sleep(0.1)
    platform_control.i2c_relay_set(0x42, 0, 4)
#    time.sleep(0.1)
    platform_control.spi_dac1_set(1.5)


