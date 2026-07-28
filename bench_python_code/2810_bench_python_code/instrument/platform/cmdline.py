
#cmdline
import time, sys
import json
import os

sys.path.append('.')
sys.path.append('..')
sys.path.append('../../')

import instrument.platform.ch341_api as i2c_com

mx03_i2c_address = 0x17

class class_cmdline():

    instrument_id_dict = {}
    multimeter_mode_ctrl = {}

    def __init__(self, i2c_handle = ''):
        if i2c_handle == '':
            self.i2c_handle = i2c_com.class_ch341_api()
        else:
            self.i2c_handle = i2c_handle

        with open('./instrument/instrument_info.json', 'r', encoding= 'utf-8') as file:
            instrument_info = json.load(file)

        self.instrument_id_dict = instrument_info["instrument_id_set"]
        self.multimeter_mode_ctrl = instrument_info["multimeter_mode"]
        
    def i2c_read(self, write_data,  read_len):
        slave_address = mx03_i2c_address*2+1
        wr_len = len(write_data)
        return self.i2c_handle.ch341_i2c_read(slave_address, write_data, wr_len, read_len)
        
    def i2c_write(self, write_data):
        write_len = len(write_data)
        slave_address = mx03_i2c_address*2
        self.i2c_handle.ch341_i2c_write(slave_address, write_data, write_len)
        
    def device_relay_off(self,  instrument_name):#device_id 8bit max 255
        cmd = 0
        instrument_id = self.instrument_id_dict[instrument_name]
        write_data = [cmd,  instrument_id]+[0xAA, 0xAA, 0xAA, 0xAA, 0xAA] 
        self.i2c_write(write_data)
        time.sleep(0.1)
        
    def device_relay_on(self,  instrument_name):#device_id 8bit max 255 
        cmd = 1
        instrument_id = self.instrument_id_dict[instrument_name]
        write_data = [cmd,  instrument_id]+[0xAA, 0xAA, 0xAA, 0xAA, 0xAA] 
        self.i2c_write(write_data)
        time.sleep(0.1)

    def channel_b_relay_ctrl(self, relay, status):

        relay_channel_dict = {"K46":9, "K48": 10, "K50": 11, "K52": 12, "K45":13, "K47": 14, "K49":15, "K51": 16}

        if status == "on":
            cmd = 1
        elif status == "off":
            cmd = 0
        write_data = [cmd, relay_channel_dict[relay]]+[0xAA, 0xAA, 0xAA, 0xAA, 0xAA]
        self.i2c_write(write_data)
        time.sleep(0.1)
        
    def channel_relay_ctrl(self, channel):#channel 32bit
        cmd = 2
        write_data = [cmd] + list(channel.to_bytes(4, 'little')) + [0xAA, 0xAA]
        self.i2c_write(write_data) 
        time.sleep(0.1) 
        
    #最小延时  看怎样调到最小延时
    def dac_mode_control(self,  para1,  para2,  channal = 1,  mode = 0,  para3 = 0):
        cmd = 3
        
        if (mode == 0):
            write_data = [cmd,  mode,  para1, para2, para3,  channal]+ [0xAA]
        else:
            write_data = [cmd,  mode,  para1,  para2,  para3, channal]+ [0xAA]
            
        # write_data = [cmd,  mode]+list(para.to_bytes(3, 'big'))+[0xAA]
        self.i2c_write(write_data) 
        time.sleep(0.002)   #用二分法降到最小   所有延时的地方尽量降到最小
        
    def frequence_get(self, average_times):
        cmd = 4 #约定服务号
        freq_length = 4 #4bytes
        freq = 0x5A5A5A5A
        
        write_data = [cmd]+list(average_times.to_bytes(4, 'little')) + [0xAA, 0xAA]
        
        self.i2c_write(write_data)

        time.sleep(0.1)
        
        while(freq == 0x5A5A5A5A):
            freq = int.from_bytes(self.i2c_read([], freq_length), byteorder='big', signed=False)
            time.sleep(0.2)
        
        return freq # 4bytes
    
    def address_get(self):
        cmd = 5 #约定服务号
        write_data = [cmd] + [0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA]
        address = 0x5A5A5A5A
        address_length = 4
        
        self.i2c_write(write_data)
        
        while(address == 0x5A5A5A5A):
            address = int.from_bytes(self.i2c_read([], address_length), byteorder='big', signed=False)
            time.sleep(0.1)
        
        return address
    
    def voltage_get(self,  channel):
        cmd = 6 #约定服务号
        write_data = [cmd] + list(channel.to_bytes(4, 'little'))+ [0xAA, 0xAA]
        voltage = 0x5A5A5A5A
        voltage_length = 4 #4bytes
        
        self.i2c_write(write_data)  
        
        while(voltage == 0x5A5A5A5A):
            voltage = int.from_bytes(self.i2c_read([], voltage_length), byteorder='big', signed=False)
            time.sleep(0.1)
            
        return voltage # 4bytes
        
    def up_down_res_config(self,  res_mode): #note: if up_res_en = 1, the K61 relay is config as pull-up resistor
        cmd = 7 #约定服务号
        up_down_dict = {"down": 0, "up": 1}
        write_data = [cmd,  up_down_dict[res_mode]] + [0xAA, 0xAA, 0xAA, 0xAA, 0xAA] 
        self.i2c_write(write_data)
        time.sleep(0.1)
        
    def loader_control(self,  up_en):
        cmd = 8 #约定服务号
        write_data = [cmd,  up_en] + [0xAA, 0xAA, 0xAA, 0xAA, 0xAA] 
        self.i2c_write(write_data)
        time.sleep(0.1)
        
    def multimeter_control(self,  test_mode):
        cmd = 9 #约定服务号
        voltage_test_en = self.multimeter_mode_ctrl[test_mode]
        write_data = [cmd,  voltage_test_en] + [0xAA, 0xAA, 0xAA, 0xAA, 0xAA] 
        self.i2c_write(write_data)
        time.sleep(0.1)
        
    def dac_b_connect_channel_A(self,  current_test_en):
        cmd = 10 #约定服务号
        write_data = [cmd,  current_test_en] + [0xAA, 0xAA, 0xAA, 0xAA, 0xAA] 
        self.i2c_write(write_data)
        time.sleep(0.1)
        
    def dut_vcc_power_source_select(self,  dac_out_en):
        cmd = 11 #约定服务号
        write_data = [cmd,  dac_out_en] + [0xAA, 0xAA, 0xAA, 0xAA, 0xAA] 
        self.i2c_write(write_data)
        time.sleep(0.1)
    
    def dut_vcc_ctrl(self,  current_test_en):  #换一个名字
        cmd = 12 #约定服务号
        write_data = [cmd,  current_test_en] + [0xAA, 0xAA, 0xAA, 0xAA, 0xAA] 
        self.i2c_write(write_data)
        time.sleep(0.1)
        
    def I2C_connect(self,  enable):
        cmd = 13 #约定服务号
        write_data = [cmd,  enable] + [0xAA, 0xAA, 0xAA, 0xAA, 0xAA] 
        self.i2c_write(write_data)
        time.sleep(0.1)
        
    def SWD_connect(self,  enable):
        cmd = 14 #约定服务号
        write_data = [cmd,  enable] + [0xAA, 0xAA, 0xAA, 0xAA, 0xAA] 
        self.i2c_write(write_data)
        time.sleep(0.1)
    
    def test_gpio_leackage(self, test_vih):
        cmd = 15 #约定服务号
        write_data = [cmd, test_vih] + [0xAA, 0xAA, 0xAA, 0xAA, 0xAA] 
        self.i2c_write(write_data)
        time.sleep(0.1)

    def dac_out_buffer_ctrl(self, b_buf_en, c_buf_en):
        cmd = 16 #约定服务号
        write_data = [cmd, b_buf_en, c_buf_en] + [0xAA, 0xAA, 0xAA, 0xAA] 
        self.i2c_write(write_data)
        time.sleep(0.1)

    def multimeter_com_port_ctrl(self, switch):
        cmd = 17
        write_data = [cmd, switch] + [0xAA, 0xAA, 0xAA, 0xAA, 0xAA] 
        self.i2c_write(write_data)
        time.sleep(0.1)

    def dac_output_voltage(self, voltage, ch, buf_en):
        self.dac_mode_control(para1, para2)
        
def platform_init(cmdline_handle):
    # cmdline_handle.dac_mode_control(0x80, 0xff, 0x01)
    # time.sleep(0.1)
    cmdline_handle.SWD_connect(1)

    # cmdline_handle.channel_b_relay_ctrl("K46", "on")
    
if __name__ == "__main__":  
    os.chdir('../../')
    i2c_handle = i2c_com.class_ch341_api() 
    cmdline = class_cmdline(i2c_handle)
    platform_init(cmdline)

    cmdline.device_relay_off("freq_measure")

    # cmdline.dac_b_connect_channel_A(1)

    # cmdline.channel_relay_ctrl(9)

    # cmdline.dac_mode_control(0x40, 0xff, 0x2)

    # time.sleep(3)

    # cmdline.dac_mode_control(0xff, 0xff, 0x01)
    
    # print(1<<1)
    
    # for i in range(255):
    #     cmdline.dac_mode_control(0xff-i, 0xff, 6)
    #     time.sleep(0.5)
    
#    cmdline.channel_relay_ctrl(29)
    
#    device_id = 1
#    cmdline.device_relay_open(device_id)
    
#    freq = cmdline.frequence_get(50000)
#    print("the frequence is", freq)

#    cmdline.led_control()

#    voltage = cmdline. voltage_get(0)
#    print("the voltage is", voltage)
    
#    cmdline.dac_mode_control(0, 0x400001)
#    cmdline.dac_mode_control(0xff,  0xff)

#    cmdline.device_relay_on(5)
#    cmdline.device_relay_off(6)
#    cmdline.channel_relay_ctrl(0x00000002)

#    address = cmdline.address_get()
#    print("the address is", address)

 #   cmdline.dac_mode_control(0xff, 0xff, 0x01)

#    cmdline.SWD_connect(1)

#    device_id = 6
#    cmdline.device_relay_on(device_id)
#    cmdline.device_relay_off(device_id)
    
#    cmdline.up_down_control(1)

#    cmdline.loader_control(1)

#    cmdline.multimeter_control(1)

#    cmdline.dut_vcc_current_test(0) 

#    cmdline.dut_vcc_power_source_select(1)

#    cmdline.random_current_test(0)

#    cmdline.channel_relay_ctrl(37)

#    dac_mode_control(self,  para1,  para2,  channal = 1,  mode = 0,  para3 = 0)
#    cmdline.dac_mode_control(0xff, 0xff, 0x01)
#    time.sleep(0.5)
#    cmdline.SWD_connect(1)
#    cmdline.led_control()


    



