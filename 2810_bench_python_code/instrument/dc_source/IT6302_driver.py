import serial
import time

# 常用命令
power_supply_port = 'COM36'
# system
get_id = '*idn?\n'
set_remote_ctrl = 'SYST:REM\n'
get_sys_version = 'SYST:VERS?\n'


class DCSourceCtrl:
    ser_handle = 0
    dll_handle = ""
    UART_BAUDRATE = 9600
    get_id = '*IDN?\n'
    set_remote_ctrl = 'SYST:REM\n'
    status = 0
    def __init__(self, com_port):
        self.com = com_port
        self.ser_handle = serial.Serial()
        self.uart_init(self.UART_BAUDRATE, self.com)

        id_query_str = self.send_cmd(get_id)

        if 'IT6302' not in id_query_str:
            print('init IT6302 fail')
            self.ser_handle.close()
            status = 1

        # print(self.get_cmd_return())

    def uart_init(self, uart_baudrate, serial_com):
        self.uart_baudrate_configure = uart_baudrate        
        serial_open_status = -1
        self.ser_handle.port = serial_com
        self.ser_handle.baudrate = uart_baudrate
        self.ser_handle.timeout = 2 # read or write timeout (unit is seconds )

        self.ser_handle.open()
        self.ser_handle.reset_input_buffer()
        self.ser_handle.reset_output_buffer()

        if self.ser_handle._port is None:
            print("Error, serial port can't be opened. Check port and try again.")
            
        if self.ser_handle.is_open:
            serial_open_status = 0
            #print (self.ser_handle.port + " opened with baud rate = " + str(self.ser_handle.baudrate))

        return serial_open_status

    def uart_write(self, cmd_str):
        self.ser_handle.write(cmd_str.encode('ascii'))


    def send_cmd(self, cmd_str):
        self.uart_write(cmd_str)
        time.sleep(0.001)
        return self.ser_handle.readline().decode('utf-8','ignore')

    def get_cmd_return(self):
        return self.com.readline().decode('utf-8','ignore')

    
    def dc_source_it6302_initial(self):
        self.uart_write(self.set_remote_ctrl)
        print(self.send_cmd(self.get_id))     

    def dc_source_it6302_close_all(self):
        self.uart_write('OUTPut 0\n')
    
    def dc_source_it6302_open_all(self):
        self.uart_write('OUTPut 1\n')

    def query_state(self):
        read_back = self.send_cmd('OUTPut:STATe?\n')
        return int(read_back)

    def query_channel_voltage(self):
        self.uart_write('INSTrument CH1\n')
        read_back = self.send_cmd('VOLTage?\n')
        ch1_vol = float(read_back)
        self.uart_write('INSTrument CH2\n')
        read_back = self.send_cmd('VOLTage?\n')
        ch2_vol = float(read_back)
        self.uart_write('INSTrument CH3\n')
        read_back = self.send_cmd('VOLTage?\n')
        ch3_vol = float(read_back)

        return [ch1_vol, ch2_vol, ch3_vol]

    def dc_source_it6302_channel_ctrl(self, channel, ctrl):
        ctrl_cmd_dict = {"on": "CHANnel:OUTPut 1\n",
                        "off": "CHANnel:OUTPut 0\n"}
        self.uart_write('INSTrument CH'+str(channel) +'\n')
        self.uart_write(ctrl_cmd_dict[ctrl])

    def dc_source_measure_current(self):
        ret_str = self.send_cmd('MEASure:CURRent? ALL\n')
        print(type(ret_str))

        chan_list = ret_str.split(',')

        return chan_list

    def dc_source_output_paralle_control(self, switch):
        
        self.send_cmd('OUTPut:PARalle ')
        
