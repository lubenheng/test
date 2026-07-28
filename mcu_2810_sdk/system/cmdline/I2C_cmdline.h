#ifndef I2C_CMDLINE_H
#define I2C_CMDLINE_H
 
#include "stdint.h"
  
int cmd_I2C_Tx_test(int argc, char *argv[]);
int cmd_I2C_Tx_Long_test(int argc, char *argv[]);

int cmd_I2C_Rx_test(int argc, char *argv[]);

int cmd_I2C_Slave_Tx_test(int argc, char *argv[]);
int cmd_I2C_Slave_Rx_test(int argc, char *argv[]);


#define I2C_CMDLINE_MENU \
		{"cmd_I2C_Tx_test",cmd_I2C_Tx_test,  " cmd_I2C_Tx_test"},\
		{"cmd_I2C_Tx_Long_test",cmd_I2C_Tx_Long_test,  " cmd_I2C_Tx_Long_test"},\
		{"cmd_I2C_Rx_test",cmd_I2C_Rx_test,  " cmd_I2C_Rx_test"},\
		{"cmd_I2C_Slave_Tx_test",cmd_I2C_Slave_Tx_test,  " cmd_I2C_Slave_Tx_test"},\
		{"cmd_I2C_Slave_Rx_test",cmd_I2C_Slave_Rx_test,  " cmd_I2C_Slave_Rx_test"},\

#endif
