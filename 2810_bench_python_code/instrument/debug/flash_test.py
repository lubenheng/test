from flash import class_flash_info
from flash import class_code_flash
import sys, time
sys.path.append('..')
import pyjlink_dev as jlink
import xlwings as xw#excel

def save_to_excel(name, data_list):
    app=xw.App(visible = True,add_book = False)
    filepath=name+'.xlsx'
    wb=app.books.add()
    wb.save(filepath)
    
    list_len= len(data_list)
    for i in range(list_len):
        wb.sheets('sheet1').range(chr(ord('A')+i)+'1').options(transpose=True).value= data_list[i]
    wb.save()
    wb.close()
    app.quit() 
    
def cp_test(communication):
    communication.swd_halt()
    communication.swd_reg_write(0x400070fc, 0x5a5a5a5a)
    communication.swd_reg_write(0x40007014, 0x100)
    
    communication.swd_reg_write(0x400070d8, 0x5000000c)
    communication.swd_reg_write(0x400070dc, 0x12080302)
    communication.swd_reg_write(0x40007000, 0x464c4f0b)
    time.sleep(0.001)
    communication.swd_reg_write(0x400070d8, 0x00000000)
    print("0x%x"%communication.swd_reg_read(0x400070d8))
    communication.swd_reg_write(0x40007004, 0x0555aaaa)
    print("0x%x"%communication.swd_reg_read(0x40007004))

def flash_pattern_test(code_flash):
    code_flash.comm.swd_halt()
    start_addr = 0x000
    end_addr = 0xFFFF
    pattern_list= [1, 2,  3,  4,  5]
    
    for pattern in pattern_list:
        print("start test pattern:%d"%pattern)

        code_flash.base_cfg()
        code_flash.chip_erase()
        code_flash.flash_start_addr_write(start_addr, end_addr, pattern)
#        code_flash.write(0x0e00*8, 0, 0)#ecc
        code_flash.flash_start_addr_read(start_addr, end_addr, pattern)
        
        name = 'flash_test_pattern_'+str(pattern)
        data_list=[]
        row_length = 1024
        sector = int((end_addr+1)/row_length/4)
        data = code_flash.read_by_ahb(start_addr, end_addr*2+2)
        for i in range(sector):
            data_list.append(data[i*row_length*8:(((i+1))*row_length*8)])
        save_to_excel(name,  data_list)
        
def data_flash_pattern_test(code_flash):
    code_flash.comm.swd_halt()
    code_flash.data_flash_switch()
    start_addr = 0x000
    end_addr = 0x3FFF
    pattern_list= [1, 2,  3,  4,  5]
    
    for pattern in pattern_list:
        print("start test pattern:%d"%pattern)

        code_flash.base_cfg()
        code_flash.chip_erase()
        code_flash.flash_start_addr_write(start_addr, end_addr, pattern)
#        code_flash.write(0x0e00*8, 0, 0)#ecc
        code_flash.flash_start_addr_read(start_addr, end_addr, pattern)
        
        name = 'data_flash_test_pattern_'+str(pattern)
        data_list=[]
        row_length = 1024
        sector = int((end_addr+1)/row_length/4)
        data = code_flash.read_by_ahb(start_addr+0x20020000, end_addr*2+2)
        for i in range(sector):
            data_list.append(data[i*row_length*8:(((i+1))*row_length*8)])
        save_to_excel(name,  data_list)
            
def flash_fdata_test(code_flash):
    code_flash.comm.swd_halt()
    start_addr = 0x000
    end_addr = 0xffff
    fdata = 0xaaaaaaaa
    fdata0 = fdata
    fdata1 = fdata
    fdata2 = fdata
    code_flash.flash_start_addr_write_fdata(start_addr,  end_addr, fdata0, fdata1, fdata2)
#    code_flash.write(0x0e00*8, 0x00000000, 0x0000000)
    code_flash.flash_start_addr_read_fdata(start_addr,  end_addr)
    name = 'flash_test_fdata_'
    data_list=[]
    row_length = 1024
    sector = int((end_addr-start_addr+1)/row_length/4)

    data = code_flash.read_by_ahb(start_addr, end_addr*2+1)
    for i in range(sector):
        data_list.append(data[i*row_length*8:(((i+1))*row_length*8)])
    save_to_excel(name,  data_list)

def flash_pattern_sector_test(code_flash):
    start_addr = 0x000
    end_addr = 0xFFFF
    pattern_list= [1, 2, 3, 4, 5, 6]
    
    code_flash.comm.swd_halt()
    code_flash.base_cfg()
    
    for pattern in pattern_list:
        print("start test pattern:%d"%pattern)
        for sec_num in range(128):
            code_flash.sector_pattern_write(sec_num, pattern)
            
        for sec_num in range(128):
            code_flash.sector_pattern_read(sec_num, pattern)
            
        name = 'flash_test_pattern_'+str(pattern)
        data_list=[]
        row_length = 1024
        sector = int((end_addr - start_addr + 1)/row_length/4)
        data = code_flash.read_by_ahb(start_addr, end_addr*2+2)
        for i in range(sector):
            data_list.append(data[i*row_length*8:(((i+1))*row_length*8)])
        save_to_excel(name,  data_list)
    
def flash_fdata_sector_test(code_flash, fdata = 0xaaaaaaaa):
    code_flash.comm.swd_halt()
    code_flash.code_flash_switch()
    fdata0 = fdata
    fdata1 = fdata
    fdata2 = fdata
    code_flash.base_cfg()
    code_flash.chip_erase()
    
    for sec_num in range(128):
        code_flash.flash_fdata_swrite(sec_num, fdata0, fdata1, fdata2)
#    code_flash.write(0x0e00, 0x0, 0xf0f0f0f0)
#    code_flash.write(0x0e80, 0x0, 0xf0f0f0f0)
    for sec_num in range(128):   
        flag = code_flash.flash_fdata_sread(sec_num, fdata0, fdata1, fdata2)
        if (flag == 0):
            break
    name = 'flash_test_fdata_'+str(fdata)+'_sector'
    data_list=[]
    start_addr = 0
    end_addr = 0xffff
    row_length = 1024
    sector = int((end_addr-start_addr+1)/row_length/4)

    data = code_flash.read_by_ahb(start_addr, end_addr*2+1)
    for i in range(sector):
        data_list.append(data[i*row_length*8:(((i+1))*row_length*8)])
    save_to_excel(name,  data_list)

def data_flash_fdata_sector_test(code_flash, fdata):
    code_flash.comm.swd_halt()
    code_flash.data_flash_switch()
    fdata0 = fdata
    fdata1 = fdata
    fdata2 = fdata
    code_flash.base_cfg()
    code_flash.chip_erase()
    
    for sec_num in range(128):
        code_flash.flash_fdata_swrite(sec_num, fdata0, fdata1, fdata2)
#    code_flash.write(0x0e00, 0x0, 0xf0f0f0f0)
#    code_flash.write(0x0e80, 0x0, 0xf0f0f0f0)
    for sec_num in range(128):   
        flag = code_flash.flash_fdata_sread(sec_num, fdata0, fdata1, fdata2)
        if (flag == 0):
            break
    name = 'data_flash_test_fdata_'+str(fdata)+'_sector'
    data_list=[]
    start_addr = 0x20020000
    end_addr = 0x20027FFF
    row_length = 1024
    sector = int((end_addr-start_addr+1)/row_length/4)

    data = code_flash.read_by_ahb(start_addr, (end_addr-start_addr+1))
    for i in range(sector):
        data_list.append(data[i*row_length*8:(((i+1))*row_length*8)])
    save_to_excel(name,  data_list)

def nvr_cfg_write(com, addr, data):
    com.swd_reg_write(0x400070fc, 0x5a5a5a5a)
    com.swd_reg_write(0x40007014, 0x00000100)
    com.swd_reg_write(0x400070c0, (1<<31) | (1<<26) | (1<<9) | (3<<6))
    com.swd_reg_write(0x40007004, addr)
    com.swd_reg_write(0x40007008, data)
    com.swd_reg_write(0x40007000, 0x464c4f0a)
    time.sleep(0.001)
    com.swd_reg_write(0x400070c0, (1<<31) | (1<<9))

def cp_test_mode_exit(com):
    com.swd_reg_write(0x400070d8, 0xf0000000)
    com.swd_reg_write(0x400070dc, 0)
    com.swd_reg_write(0x40007000, 0x464c4f0b)
    time.sleep(0.001)
    com.swd_reg_write(0x400070d8, 0)
    time.sleep(0.001)
    
def cp_test_mode_enter(com, tm0,  tm1):
    com.swd_reg_write(0x400070d8, tm0)
    com.swd_reg_write(0x400070dc, tm1)
    com.swd_reg_write(0x40007000, 0x464c4f0b)
    time.sleep(0.001)
    
def cp_flash_unclock(com):
    com.swd_reg_write(0x400070fc, 0x5a5a5a5a)
    com.swd_reg_write(0x40007014, 0x00000100)
    com.swd_reg_write(0x40007018, 0x77000042)
    
def cp_timing_selftrim(com):
    com.swd_halt()
    cp_flash_unclock(com)
    com.swd_reg_write(0x40001004,  0x02b4ddd7)#pll out 90M 
    com.swd_reg_change(0x40001000, 1, 1,1)# PLL to SYS
    print("trim start")
    trim_code_list = [7,6, 5, 4, 3, 2, 1, 0, 0xf, 0xe, 0xd, 0xc, 0xb, 0xa, 0x9, 0x8]
    for trim_code in trim_code_list:
        nvr_cfg_write(com, 0, (trim_code<<3)|0xff07)
        print(trim_code)
        cp_test_mode_enter(com, 0x10000000, 0x00000017)
        com.swd_reg_write(0x400070d8, 0x00000000)
        com.swd_reg_write(0x400070e0, 0x00010000)
        time.sleep(0.001)
        com.swd_reg_change(0x400070c0, 11 , 1 , 1)#clk90_en
        a = int(input())
        if (a== 1):
            return
        com.swd_reg_change(0x400070c0, 11 , 1 , 0)#clk90_dis
        com.swd_reg_write(0x400070e0, 0x00000000)
        cp_test_mode_exit(com)
    print("res trim start")
    nvr_cfg_write(com, 0, 8<<3)
    res_trim_code_list = [6, 5, 4, 3]        
    for res_trim_code in res_trim_code_list:
        nvr_cfg_write(com, 3,  (res_trim_code<<8)|0xd8ff)
        print(res_trim_code)
        cp_test_mode_enter(com, 0x10000000, 0x00000017)
        com.swd_reg_write(0x400070d8, 0x00000000)
        com.swd_reg_write(0x400070e0, 0x00010000)
        time.sleep(0.001)
        com.swd_reg_change(0x400070c0, 11 , 1 , 1)#clk90_en
        a = int(input())
        if (a== 1):
            return
        com.swd_reg_change(0x400070c0, 11 , 1 , 0)#clk90_dis
        com.swd_reg_write(0x400070e0, 0x00000000)
        cp_test_mode_exit(com)
        
def write_read_test(code_flash):
    code_flash.chip_erase()
    DATA = [0x5555aaaa, 0xffffffff, 0x0000000, 0x5555aaaa]
#    DATA = 0x5a5a5a5a
    code_flash.write(0x0, *DATA)
    code_flash.write(0x10, *DATA)
    n = 4
    print('\nahb:')
    for i in (code_flash.read_by_ahb(0x0, 4*n)):
        print('%#x '%i,end="")
    print('\napb:')
    for i in(code_flash.read(0x0, n)):
        print('%#x '%i,end="")
    print('\n')
    
def write_flash_by_swd(link_handle, data):
    link_handle.swd_reg_write(0x4000_8014, 0x0000_0100)   
    link_handle.swd_reg_write(0x4000_8004, 0x0000_0004)   
    link_handle.swd_reg_write(0x4000_8008, data)   
    link_handle.swd_reg_write(0x4000_800C, data)   
    link_handle.swd_reg_write(0x4000_803C, data)   
    
if __name__ == "__main__":
    link_handle = jlink.JLINK_CLASS(None)
    flash = class_code_flash(link_handle)
    write_read_test(flash)
