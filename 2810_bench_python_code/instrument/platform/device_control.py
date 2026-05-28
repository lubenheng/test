# -*- coding:utf-8 -*-
__author__ = 'yue.chen'

import os 
import sys
sys.path.append('../')
sys.path.append('../../')

from instrument.multimeter.Agitek_34461A_USB import *
from instrument.debug.jlink import *

from instrument.platform.ch341_api import *
from instrument.platform.cmdline import *

from instrument.loader.Loader_IT8511A_serial import *

import instrument.platform.device_config as dev_id_set

def platform_init(auto_test_debug = 0):
    print('init platform')
    
    cmdline_handle = class_cmdline()
    cmdline_handle.dac_mode_control(0x00, 0x00, 0x0f)
    time.sleep(0.1)
    # cmdline_handle.dac_mode_control(0x80, 0xff, 0x01)
    cmdline_handle.SWD_connect(1)
    if auto_test_debug == 0:
        multimeter_handle = DMM34461()
        loader_handle = class_DCLoaderCtrl()
        loader_handle.loader_it8511a_initial()
    else:
        multimeter_handle = 0
        loader_handle = 0
    jlink_handle = JLINK_CLASS(None)

    return jlink_handle, cmdline_handle, multimeter_handle, loader_handle

'''max output 8.25V'''
def board_dac_output_config(cmdline_handle, dac_ch, voltage, buf_en):
    # print('set voltage: %f'%voltage)
    cmdline_handle.dac_out_buffer_ctrl(dac_ch, buf_en)
    # 0x80ff -> 5.03V
    voltage_code = int((voltage/5.032)*0x80ff)
    # print(voltage_code)
    cmdline_handle.dac_mode_control(voltage_code >> 8, voltage_code&0xff, 1<<dac_ch)
    time.sleep(0.1)


def test_board_dac_out_noise(cmdline_handle):
    cmdline_handle.device_relay_on(dev_id_set.EXTERNAL_A_CHANNEL_0)
    cmdline_handle.device_relay_on(dev_id_set.DAC_C_OUT)
    board_dac_output_config(cmdline_handle, 2, 5, 1)
    cmdline_handle.channel_relay_ctrl(15)

if __name__ == '__main__':
    print('unit test')
    os.chdir('../')
    local_jlink_handle, local_cmdline_handle, local_multimeter_handle, local_loader_handle = platform_init(1)

    board_dac_output_config(local_cmdline_handle, 1, 5.0, 0)

    # local_cmdline_handle.device_relay_on(1)
    # local_cmdline_handle.device_relay_on(3)
    # local_cmdline_handle.dac_mode_control(0x80, 0xff, 0x01)
    # local_jlink_handle.swd_connect()

    # local_jlink_handle.swd_halt()
    
    # board_dac_output_config(local_cmdline_handle, 0, 0.002, 1)

