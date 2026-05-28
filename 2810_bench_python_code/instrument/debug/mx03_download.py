import os, sys
from ctypes import *
import numpy as np
import datetime,  subprocess
sys.path.append('..')
import mx03_download_for_re.flash as mcu_flash
import mx03_download_for_re.pyjlink_dev as pyjlink



class class_mx03_download():
    
    def __init__(self):
        JLINK_SERIAL_NUMBER = None
        self.communication = pyjlink.JLINK_CLASS(JLINK_SERIAL_NUMBER)
        self.comm = mcu_flash.class_code_flash(self.communication)
        self.flash_info = mcu_flash.class_flash_info(self.communication)


    def flash_info_change(self):
        self.flash_info.change(0, 58,  1,  1 ) #force flash 
    
    def parse_fw_to_512k(self,  fw_bin_file):
        if(fw_bin_file == ""):
            file_name = ''
            current_path = os.getcwd()
            dirs = os.listdir(current_path)
            for i in dirs:
                if (os.path.splitext(i)[0] == "mx03_sdk"):
                    file_name = os.path.splitext(i)[0]
                    break
                    
            if (file_name == ''):
                return 0, ""
                
            parse_file = current_path + "/"+file_name+'.bin'
            
        else:
            parse_file = fw_bin_file
       
        bin_file_size = os.path.getsize(parse_file)
        print("original fw size is:", bin_file_size)
        
        size = 512 * 1024
        if (bin_file_size > size):
            return 0, ""
            
        elif (bin_file_size == size):
            bin_file = open(parse_file,  'rb')
            fw_data = (c_ubyte * size)()
            for i in range(0, bin_file_size):
                c = bin_file.read(1)
                fw_data[i] = ord(c)
            bin_file.close
            return fw_data, parse_file
            
        elif (bin_file_size < size):
            bin_file = open(parse_file,  'rb')
            fw_data = (c_ubyte * size)()
            for i in range(0, bin_file_size):
                c = bin_file.read(1)
                fw_data[i] = ord(c)
            for i in range(0, size - bin_file_size):
                if(0 == (i%2)):
                    fw_data[bin_file_size + i] = 0x55
                elif(1 == (i%2)):
                    fw_data[bin_file_size + i] = 0xaa
            bin_file.close
            self.save_fw_rb_data_to_bin(fw_data, file_name +  "_new_512k" + '.bin')
            return fw_data,  current_path + "/" + file_name +  "_new_512k" + '.bin'

    # convert  the fw data array to bin.
    def save_fw_rb_data_to_bin(self,  fw_rd_data, save_bin_file):
        fw_data_list = []
        for i in range(len(fw_rd_data)):
            fw_data_list += [fw_rd_data[i]]
        outData = np.reshape(fw_data_list, [1, len(fw_data_list)])
        outData = outData.astype(np.uint8)
        bin_file = os.getcwd()+"/" + save_bin_file
        outData.tofile(bin_file) 

    def convert_word_to_byte(self,  data_in_word):
        size = len(data_in_word) * 4
        data_in_byte =  ( c_ubyte * size)()
        for i in range(len(data_in_word)):
            temp = data_in_word[i].to_bytes(4, byteorder='little', signed=False)
            for j in range(4):
                data_in_byte[i*4 + j] = temp[j]

        return data_in_byte

    def fw_read_in_total(self,  fw_len):
        self.comm.unlock()
        fw_rd= self.comm.read_by_ahb(0,  fw_len//4)
        self.comm.lock() 
        
        fw_rd_data =  ( c_ubyte * fw_len)()
        for i in range(len(fw_rd)):
            temp = fw_rd[i].to_bytes(4, byteorder='little', signed=False)
            for j in range(4):
                fw_rd_data[i*4 + j] = temp[j]

        return fw_rd_data
        
    def fw_write_in_word(self,  ADDR,  fw_data , fw_offset = 0):
        fw_len = len(fw_data)
        
        if ((fw_len%8) != 0):
            for i in range(8-(fw_len%8)):
                fw_data.append(0)
            fw_len =8 + fw_len - (fw_len%8)

        fw_data_word_list = []
        while(fw_offset < fw_len):
            DATA0 = int.from_bytes(fw_data[fw_offset : fw_offset+4], byteorder='little', signed=False)
            fw_offset = fw_offset +4
            fw_data_word_list.append(DATA0)
        
        fw_offset = 0
    #    start = datetime.datetime.now()
        self.comm.unlock()
    #    fw_len = 1024*8
        for i in range(len(fw_data_word_list)//2):
            self.comm.write(ADDR + fw_offset, fw_data_word_list[i*2], fw_data_word_list[i*2 +1])
            fw_offset = fw_offset +8
        self.comm.lock() 
    #    elapsed = (datetime.datetime.now() - start)
    #    print("Time used:", elapsed)
        

    def jlink_command_write(self,  bin_name = "mx03_sdk.bin"):
        file_name="download.jlink"
        command ="loadfile "+bin_name+"\nhalt\nq\nexit"
        with open(file_name,"w",encoding="utf-8") as f:
            f.write(command)
            f.close()
        
    def jlink_loadbin(self):
    #    win32api.ShellExecute(0, 'open', cmd, '', '', 1)  # 前台打开
        cmd="jlink.exe -device  SA32xx -if SWD -speed 4000 -autoconnect 1 -CommandFile download.jlink"
        subprocess.check_call(cmd)
    #    time.sleep(10)
        
    def flash_loop_back_one_time_method2(self, test_fw_path, fw_data):
        chip_earse_flag = self.comm.chip_erase()
        if (chip_earse_flag == 0):
            print("chip erase fail")
            return 0
        size = 512 * 1024
        self.jlink_command_write(test_fw_path)
        self.jlink_loadbin()
        fw_rd_data = self.fw_read_in_total(size)
        
        for i in range(len(fw_data)):
            if (fw_rd_data[i] != fw_data[i]):
#                print("the expect data: 0x%x" %fw_data[i])
#                print("the read data: 0x%x" %fw_rd_data[i])
                return 0
        return 1
        
    def readout_fw_and_compare(self,  selected_file):
        if(selected_file == ""):
            file_name = ''
            current_path = os.getcwd()
            dirs = os.listdir(current_path)
            for i in dirs:
                if (os.path.splitext(i)[0] == "mx03_sdk_new_512k"):
                    file_name = os.path.splitext(i)[0]
                    break

            if (file_name == ''):
                print("Error:Cannot find mx03_sdk_new_512k.bin in current directory!")
                return

            selected_file = current_path + "/"+file_name+'.bin'

        print("1) Start parse original fw....",end="")
        fw_data, fw_path = self.parse_fw_to_512k(selected_file)
        if (fw_path == ""):
            print("Error:parse mx03_sdk_new_512k.bin fail!")
            return
        
        fw_rd_data = self.fw_read_in_total(len(fw_data))
        
    #    start compare
        for i in range(len(fw_data)):
            if (fw_rd_data[i] != fw_data[i]):
                print("compare fail!!!")
                self.save_fw_rb_data_to_bin(fw_rd_data, "fw_readout.bin")
                return    
        print("compare success")

    def jlink_download(self,  selected_file,  flash_addr = 0):
        print("1) Start parse fw....",end="")
        if (selected_file == ""):
            fw_data,normal_fw_path = self.parse_fw_to_512k("")
        else:
            fw_data,normal_fw_path = self.parse_fw_to_512k(selected_file)
        if (normal_fw_path == ""):
            print("Error:Cannot find any bin file or fw size is larger than 512k!")
            return
        
        print("2)Start chip erase....",  end="")
        chip_earse_flag = self.comm.chip_erase()
        if(chip_earse_flag):
            print("erase flash ok!")
        else:
            print("erase flash fail!")
            return

        print("3) Start download....",  end="")
        start = datetime.datetime.now()
#        self.fw_write_in_word(flash_addr,  fw_data)
        self.jlink_command_write(normal_fw_path)
        self.jlink_loadbin()
        elapsed = (datetime.datetime.now() - start)
        print("flash download finish, Time used:",  elapsed)

    def code_flash_recycle_test(self,  loop_cnt = 1000):
        fw_data1 = 0x55555555
        fw_data2 = 0xaaaaaaaa
        loop_index = 0
        size = 512 * 1024
        fw_rd_data = (c_ubyte * size)()
        
        loop_start_time =  datetime.datetime.now()
        while (loop_index < loop_cnt):
            start = datetime.datetime.now()
            flag = self.flash_fdata_sector_test(fw_data1)

            if (flag == 0):
                print("code flash recycle test fail at loop:",  loop_index)
                fw_rd_data = self.fw_read_in_total( size)
                self.save_fw_rb_data_to_bin(fw_rd_data, "%dth_code_flash_loop_test_0x55_readout.bin"%loop_index)
                return
                
            flag = self.flash_fdata_sector_test(fw_data2)
            if (flag == 0):
                print("code flash recycle test fail at loop:",   loop_index)
                fw_rd_data = self.fw_read_in_total( size)
                self.save_fw_rb_data_to_bin(fw_rd_data, "%dth_code_flash_loop_test_0xaa_readout.bin"%loop_index)
                return
                
            loop_index = loop_index +1
            elapsed = (datetime.datetime.now() - start)
            print("the %dth code flash loop cost time is:"%loop_index, elapsed)

        loop_end_time =  datetime.datetime.now()
        print("the total time of all code flash loop cost is:", loop_end_time - loop_start_time)
        print("code flash recycle test success!")
    
    def flash_fdata_sector_test(self, fdata = 0xaaaaaaaa):
#        self.communication.swd_halt()
        fdata0 = fdata
        fdata1 = fdata
        fdata2 = fdata
        self.comm.code_flash_switch()
        self.comm.base_cfg()
        self.comm.chip_erase()
        
        for sec_num in range(128):
            self.comm.flash_fdata_swrite(sec_num, fdata0, fdata1, fdata2)
    #        
        for sec_num in range(128):   
            flag = self.comm.flash_fdata_sread(sec_num, fdata0, fdata1, fdata2)
        return flag
      
    def data_flash_fdata_sector_test(self, fdata = 0xaaaaaaaa):
#        self.communication.swd_halt()
        fdata0 = fdata
        fdata1 = fdata
        fdata2 = fdata
        self.comm.data_flash_switch()
        self.comm.base_cfg()
        self.comm.chip_erase()
        
        for sec_num in range(128):
            self.comm.flash_fdata_swrite(sec_num, fdata0, fdata1, fdata2)
    #        
        for sec_num in range(128):   
            flag = self.comm.flash_fdata_sread(sec_num, fdata0, fdata1, fdata2)
        return flag
    
    
    def data_flash_recycle_test(self,  loop_cnt = 1000):
        fw_data1 = 0x55555555
        fw_data2 = 0xaaaaaaaa
        loop_index = 0
        size = 512 * 1024
        fw_rd_data_in_byte = (c_ubyte * size)()
        
        loop_start_time =  datetime.datetime.now()
        while (loop_index < loop_cnt):
            start = datetime.datetime.now()
            flag = self.data_flash_fdata_sector_test(fw_data1)
           
            if (flag == 0):
                print("data flash recycle test fail at loop:",  loop_index)
                fw_rd_data_in_word = self.comm.read_by_ahb(0x20020000, 32768)
                fw_rd_data_in_byte = self.convert_word_to_byte(fw_rd_data_in_word)
                self.save_fw_rb_data_to_bin(fw_rd_data_in_byte, "%dth_data_flash_loop_test_0x55_readout.bin"%loop_index)
                return
                
            flag = self.data_flash_fdata_sector_test(fw_data2)
            if (flag == 0):
                print("data_flash recycle test fail at loop:",   loop_index)
                fw_rd_data_in_word = self.comm.read_by_ahb(0x20020000, 32768)
                fw_rd_data_in_byte = self.convert_word_to_byte(fw_rd_data_in_word)
                self.save_fw_rb_data_to_bin(fw_rd_data_in_word, "%dth_data_flash_loop_test_0xaa_readout.bin"%loop_index)
                return
                
            loop_index = loop_index +1
            elapsed = (datetime.datetime.now() - start)
            print("the %dth data flash loop cost time is:"%loop_index, elapsed)

        loop_end_time =  datetime.datetime.now()
        print("the total time of all data flash loop cost is:", loop_end_time - loop_start_time)
        print("data flash recycle test success!")
    
        
if __name__ == "__main__":
    try:     
        Object = class_mx03_download()

#        Object.communication.swd_reg_write(0x40001004,  0x02ef5d03)#clock 120M
#        Object.communication.swd_reg_write(0x40001000,  0x08000122)#clock 120M


        Object.communication.swd_reg_write(0x40001004,  0x20827830)#clock 120M
        Object.communication.swd_reg_write(0x40001000,  0x02)#clock 120M
        print("CODE VISION 1.5!!!")

        
        while(1):
            download_selection = input("input your test mode:\n \
            0: flash info change \n \
            1: download fw(other parameters: bin_file_path) \n \
            2: read out and compare fw(other parameters: bin_file_path)\n \
            3: code flash recycle test(the loop times) \n \
            4: data flash recycle test(the loop times) \n \
            5: code flash recycle test and data flash recycle test(the loop times) \n \
            6: stop dubug:\n>")
            download_selection = download_selection.split(" ")
            if (download_selection[0] == "0"):
                Object.flash_info_change()
            elif (download_selection[0] == "1"):
                if (len(download_selection) > 1):
                    Object.jlink_download(download_selection[1].replace('\\','/'))
                else:
                    Object.jlink_download("")
            elif (download_selection[0] == "2"):
                if (len(download_selection) > 1):
                    Object.readout_fw_and_compare(download_selection[1].replace('\\','/'))
                else:
                    Object.readout_fw_and_compare("")

            elif(download_selection[0] == "3"): 
                if (len(download_selection) > 1):
                    Object.code_flash_recycle_test(int(download_selection[1]))
                else:
                    Object.code_flash_recycle_test()
            
            elif(download_selection[0] == "4"): 
                if (len(download_selection) > 1):
                    Object.data_flash_recycle_test(int(download_selection[1]))
                else:
                    Object.data_flash_recycle_test()
                    
            elif(download_selection[0] == "5"): 
                if (len(download_selection) > 1):
                    Object.code_flash_recycle_test(int(download_selection[1]))
                    Object.data_flash_recycle_test(int(download_selection[1]))
                else:
                    Object.code_flash_recycle_test()
                    Object.data_flash_recycle_test()
                    
            elif(download_selection[0] == "6"): 
                break
            
    except Exception  as e :
        print(e)
        print("fail!")
        input()
    pass

