import os
import sys
import time

current_dir = os.path.dirname(os.path.abspath(__file__))
project_root = os.path.dirname(os.path.dirname(current_dir))
if project_root not in sys.path:
    sys.path.insert(0, project_root)

import instrument.debug.SWD_2810_CMD as SWD
import instrument.debug.pyjlink_dev as jlink_test
import instrument.debug.flash as sa32a12_flash_test

swd_handle_test = jlink_test.JLINK_CLASS(None)
flash0_t = sa32a12_flash_test.class_flash(swd_handle_test, 0)
flash1_t = sa32a12_flash_test.class_flash(swd_handle_test, 1)



def test_swd():
    swd_handle_test.swd_reset()
    swd_handle_test.swd_halt()
    swd_handle_test.swd_run()
    time.sleep(0.001)

    #关闭 WDT
    WDT_ADDR = 0x40000008
    WDT_DISABLE_VAL = 0x00000000
    swd_handle_test.swd_set_mem32(WDT_ADDR, [WDT_DISABLE_VAL])

    time.sleep(1)

    swd_handle_test.swd_halt()
    SWD.write_register(0x40002400, 0x0)
    SWD.write_register(0x40005814, 0x3b67878)
    SWD.write_register(0x40005818, 0x57e40006)
    SWD.write_register(0x40005804, 0x13)
    time.sleep(0.001)
    SWD.write_register(0x40005800, 0x20000)
    SWD.write_register(0x40005820, 0x10)
    time.sleep(0.35)




if __name__ == "__main__":
    SWD.init_all()
