# -*- coding: utf-8 -*-

from ctypes import *
import time
import threading

import os

dll_enable = 1#test option
#dll_path="./resource/USBIOX.DLL"

i2c_clk_selection = {"20KHz":0,"100KHz":1,"400KHz":2,"750KHz":3}
download_mode_selection = {"load to registers":0,  "load to otp":1}
RETRY_COUNT_MAX = 200 #max 1s (5ms unit)


mCH341_PACKET_LENGTH = 32           #CH341在EPP/MEM方式下单次读写数据块的最大长度
mCH341A_CMD_I2C_STM_MAX = 32     # I2C接口的命令流单个命令输入输出数据的最大长度
mCH341A_CMD_I2C_STREAM = 0xAA    # I2C接口的命令包,从次字节开始为I2C命令流
mCH341A_CMD_I2C_STM_STA = 0x74   # I2C接口的命令流:产生起始位
mCH341A_CMD_I2C_STM_STO = 0x75   #I2C接口的命令流:产生停止位
mCH341A_CMD_I2C_STM_END = 0x00	  #I2C接口的命令流:命令包提前结束
mCH341A_CMD_I2C_STM_OUT = 0x80	  #I2C接口的命令流:输出数据,位5-位0为长度,后续字节为数据,0长度则只发送一个字节并返回应答
mCH341A_CMD_I2C_STM_IN = 0xC0	  # I2C接口的命令流:输入数据,位5-位0为长度,0长度则只接收一个字节并发送无应答
chip_number = "SY33518B" #  SY33518B as default 
class class_ch341_api:
    dll_handle = 0
    ch341_device_index = 0 #default open the first device
    
    i2c_slave_address = 0
    i2c_speed = "400KHz"
    i2c_download_mode = ""
    
    ch341_opened_flag = 0
    ch341_i2c_init_flag = 0
    
    gpio_direction = 0
    
    chip_number = "SY33518B" #  SY33518B as default 
    
    def __init__(self):
        if (dll_enable):
            #exception will be trigger if use cdll when call USBIOX.DLL ,  use cdll is ok when the dll is compiled by visual studio
            #if you want to know more details, pls refer to this website:https://blog.csdn.net/caimouse/article/details/38395461
            dll_path="USBIOX.DLL"
            self.dll_handle = windll.LoadLibrary(dll_path)
            self.chip_number = chip_number#chip_number
            self.ch341_device_close()
            i2c_init_flag = -1
            print("ch341 device open...", end="")
            if (self.ch341_device_open() == 0): 
                print("success") 
            else:
                print("fail") 
        
            if (i2c_init_flag == -1):
                self.ch341_i2c_init("750KHz")
                i2c_init_flag = 0
            
    def ch341_device_open(self):
        if (self.dll_handle):
            if (self.ch341_opened_flag == 0):
                status = self.dll_handle.USBIO_OpenDevice(self.ch341_device_index)
                if (status != -1):
                    self.ch341_opened_flag = 1
                    self.gpio_direction = 0
                    return 0
                else:
                    return -1
            
    def ch341_device_close(self):
        if (self.dll_handle):
            if (self.ch341_opened_flag == 1):
                self.dll_handle.USBIO_CloseDevice(self.ch341_device_index)
                
                self.ch341_opened_flag = 0
                self.ch341_i2c_init_flag = 0
                self.gpio_direction = 0
    
    def ch341_i2c_slave_address_change(self, new_i2c_slave_id):
        self.i2c_slave_address = new_i2c_slave_id<<1
        
    def ch341_download_mode_change(self, new_download_mode):
        self.i2c_download_mode = new_download_mode
        
    def ch341_download_mode_get(self):
        return self.i2c_download_mode 
        
    def ch341_i2c_init(self, i2c_speed):
        self.i2c_speed = i2c_speed
        if (self.dll_handle):
            if (self.i2c_speed == "20KHz"):
                self.dll_handle.USBIO_SetStream(self.ch341_device_index, 0x80)
            elif (self.i2c_speed == "100KHz"):
                self.dll_handle.USBIO_SetStream(self.ch341_device_index, 0x81)
            elif (self.i2c_speed == "400KHz"):
                self.dll_handle.USBIO_SetStream(self.ch341_device_index, 0x82)
            else:# (self.i2c_speed == "750KHz"):
                self.dll_handle.USBIO_SetStream(self.ch341_device_index, 0x83)     
                
        self.ch341_i2c_init_flag = 1
            
    def ch341_status_get(self):
        return self.ch341_opened_flag, self.ch341_i2c_init_flag

    def i2c_issue_start(self):
        if (self.dll_handle == 0):
            return -1

        buf = ( c_ubyte * mCH341_PACKET_LENGTH)()

        buf[0] = mCH341A_CMD_I2C_STREAM  #command code
        buf[1] = mCH341A_CMD_I2C_STM_STA #generate the start bit
        buf[2] = mCH341A_CMD_I2C_STM_END #end of the current package in advance
        o_len = ( c_ubyte * 1)()
        o_len[0] = 3

        #write the data block
        if (self.dll_handle.USBIO_WriteData(self.ch341_device_index, buf, o_len)):
            return 0
        else:
            return -1
        
    def i2c_issue_stop(self):
        if (self.dll_handle == 0):
            return -1
        
        buf = ( c_ubyte * mCH341_PACKET_LENGTH)()
        buf[0] = mCH341A_CMD_I2C_STREAM   # command code
        buf[1] = mCH341A_CMD_I2C_STM_STO  # stop bit generation
        buf[2] = mCH341A_CMD_I2C_STM_END   # end of the current package in advance
        o_len = ( c_ubyte * 1)()
        o_len[0] = 3
        
        #write the data block
        if (self.dll_handle.USBIO_WriteData(self.ch341_device_index, buf, o_len)):
            return 0
        else:
            return -1
            
    def i2c_outbyte_check_ack(self, out_byte):
        if (self.dll_handle == 0):
            return -1

        buf = ( c_ubyte * mCH341_PACKET_LENGTH)()

        buf[0] = mCH341A_CMD_I2C_STREAM #command code
        buf[1] = mCH341A_CMD_I2C_STM_OUT #bit5 - bit0: length, length 0 bytes; returns only to send a response data
        buf[2] = out_byte
        buf[3] = mCH341A_CMD_I2C_STM_END #end of the current package in advance
        o_len = 4
        i_len = (c_ubyte * 1)()
        i_len[0] = 0
        
        #execute command stream, and then input first output
        if (self.dll_handle.USBIO_WriteRead(self.ch341_device_index,
                            o_len, buf,
                            mCH341A_CMD_I2C_STM_MAX, 1,
                            i_len, buf) ):
            if (i_len[0] and ((buf[i_len[0] - 1] & 0x80) == 0) ):
                return 0
            else:
                #print("i2c nack!!!")
                return -1
        else:
            return -1
            
    def i2c_inblock_ack(self,  in_len, in_buf):
        if (self.dll_handle == 0):
            return -1

        buf = ( c_ubyte * mCH341_PACKET_LENGTH)()
        i_len = ( c_ubyte * 1)()
        i_packet_len = 0
        in_buf_offset = 0
        
        while (in_len):
            if (in_len > mCH341A_CMD_I2C_STM_MAX ):
                i_packet_len = mCH341A_CMD_I2C_STM_MAX
            else:
                i_packet_len = in_len
                
            in_buf_tmp = ( c_ubyte * i_packet_len)()
            
            buf[0] = mCH341A_CMD_I2C_STREAM #command code
            buf[1] = (mCH341A_CMD_I2C_STM_IN | i_packet_len) # input data, bit 5 - bit 0 is the length of
            buf[2] = mCH341A_CMD_I2C_STM_END#end of the current package in advance
            o_len = 3
            i_len[0] = 0

            #execute command stream, and then input first output
            if (self.dll_handle.USBIO_WriteRead( self.ch341_device_index,
                                 o_len, buf,
                                 mCH341A_CMD_I2C_STM_MAX, 1,
                                 i_len, in_buf_tmp) ):
                if (i_len[0] != i_packet_len):
                    return -1
            else:
                return -1
                
            for i in range(i_packet_len):
                in_buf[i+in_buf_offset] = in_buf_tmp[i]
                
            in_buf_offset += i_packet_len
            in_len -= i_packet_len

        return 0
        
    def i2c_inbyte_nack(self,  in_buf):
        if (self.dll_handle == 0):
            return -1

        buf = ( c_ubyte * mCH341_PACKET_LENGTH)()
        i_len = ( c_ubyte * 1)()
        i_len[0] = 0 
        o_len = 3
        
        buf[0] = mCH341A_CMD_I2C_STREAM # command code
        buf[1] = mCH341A_CMD_I2C_STM_IN # input data, bit 5 - bit 0 of length, only to receive a length of 0 bytes and sends no response
        buf[2] = mCH341A_CMD_I2C_STM_END # end of the current package in advance
        
        #execute command stream and then input first output
        if (self.dll_handle.USBIO_WriteRead( self.ch341_device_index,
                             o_len, buf,
                             mCH341A_CMD_I2C_STM_MAX, 1,
                             i_len, buf) ):
            if ( i_len[0] ):
                in_buf[0] = buf[i_len[0] - 1] # data
                return 0
            else:
                return -1
        else:
            return -1

    def ch341_i2c_xfer_check_ack(self, write_data, write_len, read_data, read_len):
        if (self.dll_handle == 0):
            return -1
        
        #start write
        if (write_len>0):
            status = self.i2c_issue_start()
            if ( status < 0 ):
                return status

            wr_buf = ( c_ubyte * (1+write_len))()
            for i in range(write_len):
                wr_buf[1+i] = write_data[i]
            wr_buf[0] = self.i2c_slave_address
            
            for i in range(len(wr_buf)):
                status = self.i2c_outbyte_check_ack(wr_buf[i])
                if (status <0): #i2c nak checked
                    self.i2c_issue_stop()
                    return  status
        
        #start read
        if (read_len > 0):
            status = self.i2c_issue_start()#repeat start
            if ( status < 0 ):
                return status

            #slave_addr + r
            status = self.i2c_outbyte_check_ack(wr_buf[0] | 0x01)
            if (status <0): #i2c nak checked
                self.i2c_issue_stop()
                return  status
                
            read_buf_tmp = ( c_ubyte * (read_len-1))()
            if (read_len > 1):
                status = self.i2c_inblock_ack(len(read_buf_tmp), read_buf_tmp)
                if ( status <0):
                    self.i2c_issue_stop()
                    return status
                for i in range(len(read_buf_tmp)):
                    read_data[i] = read_buf_tmp[i]
                        
            read_buf_last_byte = (c_ubyte * 1)()
            status = self.i2c_inbyte_nack(read_buf_last_byte)
            read_data[len(read_buf_tmp)] = read_buf_last_byte[0]
            if ( status <0):
                self.i2c_issue_stop()
                return status
                
        return self.i2c_issue_stop()
        
    def ch341_i2c_write(self, slave_address, write_data, write_len):
        #self.ch341_i2c_xfer_check_ack(write_data, write_len, 0, 0)
        #return
        
        if (self.dll_handle):
            wr_buf = ( c_ubyte * (1+write_len))()
            
            for i in range(write_len):
                wr_buf[1+i] = write_data[i]
              #1) update the i2c slave address

            #print(wr_buf[1], wr_buf[2])
            wr_buf[0] = slave_address
            
            self.dll_handle.USBIO_StreamI2C(self.ch341_device_index, write_len+1, wr_buf, 0, 0)
  
    def ch341_i2c_read(self, slave_address, register_address, register_len, read_len):          
        if (self.dll_handle):
            wr_buf = ( c_ubyte * (1+register_len))()

            for i in range(register_len):
                wr_buf[1+i] = register_address[i]   
            wr_buf[0] = slave_address
    
            read_data = (c_ubyte * read_len)()
            
            self.dll_handle.USBIO_StreamI2C(self.ch341_device_index, register_len+1,wr_buf,read_len,read_data)
            return read_data
            
def my_delay(delay_time):
    start = time.perf_counter()
    end = time.perf_counter()
    while ((end-start) < delay_time):
        end = time.perf_counter()   
        
if __name__ == "__main__":
    ch341_api = class_ch341_api()
    slave_addr = 0x90
    ch341_api.ch341_i2c_write(slave_addr, [0xD1, 0x01, 0xe7], 3)#D1:internal ref;ref*4; No slew 
    ch341_api.ch341_i2c_write(slave_addr, [0x21, 0x04, 0x00], 3)#21:dac data : 0x, x0 out =xx/256*4*1.21
    my_delay(0.00003)
    ch341_api.ch341_i2c_write(slave_addr, [0xD3, 0x00, 0x10], 3)#D3: load to eeprom 

