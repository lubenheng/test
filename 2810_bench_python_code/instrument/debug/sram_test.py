import pyjlink_dev as jlink

def sram_test(swd_handle, sram_start, sram_end):
    sram_addr = sram_start
    while(sram_addr < sram_end):
        swd_handle.swd_reg_write(sram_addr, sram_addr)
        sram_addr = sram_addr + 4

    sram_addr = sram_start
    while(sram_addr < sram_end):
        read_data = swd_handle.swd_reg_read(sram_addr)
        if (read_data != sram_addr):
            print("ERROR: sram_test failed")
            print("Read data")
            print(hex(read_data))
            print("Expected data")
            print(hex(sram_addr))
            return
        sram_addr = sram_addr + 4

    print("sram_test passed")
    

if __name__ == "__main__":
    swd_handle = jlink.JLINK_CLASS(None)
    swd_handle.swd_halt()
    
    sram0_start = 0x1FFF_C000
    sram0_end = 0x2000_0000
    sram1_start = 0x2000_0000
    sram1_end = 0x2000_4000
    sram2_start = 0x2000_4000
    sram2_end = 0x2000_C000
    sram_test(swd_handle, sram0_start, sram0_end)
    sram_test(swd_handle, sram1_start, sram1_end)
    sram_test(swd_handle, sram2_start, sram2_end)