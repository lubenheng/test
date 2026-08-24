import serial
import serial.tools.list_ports
import time

# 常用命令
# system

class class_DCLoaderCtrl:  
    ser_handle = 0
    dll_handle = ""
    UART_BAUDRATE = 9600
    get_id = '*IDN?\n'
    set_remote_ctrl = 'SYST:REM\n'
    # get_sys_version = 'SYST:VERS?\n'
    # get_current_range = 'SOURce:CURRent:RANGe?\n'
    # dll_path="./resource/USBIOX.DLL"
    def __init__(self, com_port = None):
        self.ser_handle = serial.Serial()
        if com_port != None:
            self.ch341_uart_init(self.UART_BAUDRATE, com_port)
        else:
            com_list = self.ch341_uart_ports_enumerate()
            if com_list == []:
                print("no itech com port, please check the com connection! ")
                return
            self.ch341_uart_init(self.UART_BAUDRATE, com_list[0])
        
        

    def loader_it8511a_initial(self):
        self.uart_write(self.set_remote_ctrl)
        print(self.uart_read(self.get_id))        
                    
    def ch341_uart_init(self, uart_baudrate, serial_com):
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

    def ch341_uart_ports_enumerate(self):
        com_list = []
        ports = list(serial.tools.list_ports.comports()) # parse all the serial ports
        for i in range(len(ports)): 
            com_port = ports[i]
            device, description, hwid = list(com_port)
            if (description.lower().find("prolific") != -1):
                com_list += [device]
        return com_list

    def ch341_device_reopen(self, new_baudrate):
        if (self.uart_baudrate_configure == 0): #the uart didnot opened before        
            print("uart didnot opened before")
            return -1
            
        current_comm = self.ser_handle.port
        
        self.ch341_device_close() 
        return self.ch341_uart_init(new_baudrate, current_comm)
        
    def ch341_device_close(self):
        self.ser_handle.close() # close the serial port
        
        if False == self.ser_handle.is_open:
            print(self.ser_handle.name + " port is now closed\n" )  
            return 0            
        return -1
        
    def uart_write(self, str):
        self.ser_handle.write(str.encode('ascii'))

    def uart_read(self, str):
        self.uart_write(str)
        time.sleep(0.001)
        return self.ser_handle.readline().decode('utf-8','ignore')
        
    def it8511_open_input(self):
        str = 'SOURce:INPut 1\n'
        self.uart_write(str) 
        
    def it8511_close_input(self):
        str = 'SOURce:INPut 0\n'
        self.uart_write(str) 
        
    def it8511_set_cv_mode(self):
        str = 'SOURce:MODE VOLTage\n'
        self.uart_write(str)

    def it8511_set_cc_mode(self):
        data = 'SOURce:MODE CURRent\n'
        self.uart_write(data)

    def it8511_set_cr_mode(self):
        data = 'SOURce:MODE RESistance\n'
        self.uart_write(data)
        
    def it8511_set_cv_value(self, cv_value):
        data = 'SOURce:VOLTage '+str(cv_value)+'\n'
        self.uart_write(data)   
        
    def it8511_set_cc_value(self, cc_value):
        data = 'SOURce:CURRent '+str(cc_value)+'\n'
        self.uart_write(data)          

    def it8511_set_cr_value(self,  cr_value):
        data = 'SOURce:RESistance '+str(cr_value)+'\n'
        self.uart_write(data)
        
    def it8511_cc_range(self, range_value): 
        data = 'SOURce:CURR:RANGE '+str(range_value)+'\n'
        self.uart_write(data) 

    def it8511_get_current_range(self):
        data = 'SOURce:CURRent:RANGe?\n'
        print("get_current_range:", self.uart_read(data))        
        
    def it8511_measure_voltage(self):
        str = ' MEASure:VOLT?\n'
        return self.uart_read(str)

    def it8511_measure_current(self):
        str = 'MEAS:CURR?\n'
        return self.uart_read(str)

if __name__ == "__main__":     
    loader = class_DCLoaderCtrl() 
 #   loader.it8511_close_input()
    loader.loader_it8511a_initial()
   # loader.it8511_set_cc_mode()
    loader.it8511_cc_range(0.003*1.5)
    loader.it8511_set_cc_value(0.003)
#    loader.it8511_set_cr_mode()
#    #loader.it8511_cc_range()
#    loader.it8511_set_cr_value(350)
    
    
    loader.it8511_open_input()
#    time.sleep(2)
    current = loader.it8511_measure_current()
    print("current:", current)
    voltage = loader.it8511_measure_voltage()
    print("VOLTAGE:", voltage)

#    loader.it8511_close_input()



