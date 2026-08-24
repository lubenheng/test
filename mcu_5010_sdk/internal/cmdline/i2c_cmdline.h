#ifndef I2C_CMDLINE_H
#define I2C_CMDLINE_H



int cmd_i2c_function1_test(int argc, char *argv[]);
int cmd_i2c_function2_test(int argc, char *argv[]);

int cmd_i2c_enter_master(int argc, char *argv[]);
//int cmd_i2cs_addrmatch_test(int argc, char *argv[]);
int cmd_i2c_enter_slave(int argc, char *argv[]);
int cmd_i2cm_datamatch_test(int argc, char *argv[]);
int cmd_i2c_read(int argc, char *argv[]);
int cmd_i2c_write(int argc, char *argv[]);
int cmd_i2cm_timing_test(int argc, char *argv[]);
// int cmd_i2c_autostop_test(int argc, char *argv[]);
// int cmd_i2c_hostreq_test(int argc, char *argv[]);
// int cmd_i2cs_smbus_gencall_test(int argc, char *argv[]);
// int cmd_i2c_slave_datarefresh(int argc, char *argv[]);
// int cmd_i2c_deepsleep_test(int argc, char *argv[]);
// int cmd_i2c_pressure_test(int argc, char *argv[]);
// int cmd_i2cm_randomread_test(int argc, char *argv[]);
// int cmd_i2c_pinmux_test(int argc, char *argv[]);
// int cmd_i2cs_pinmux_test(int argc, char *argv[]);
// int cmd_i2c_clock_test(int argc, char *argv[]);



#define I2C_CMDLINE_MENU	{"cmd_i2c_enter_master", 	cmd_i2c_enter_master, 		"  cmd_i2c_enter_master"},\
							{"cmd_i2c_enter_slave", 	cmd_i2c_enter_slave, 		"  cmd_i2c_enter_slave"},\
							{"cmd_i2cm_datamatch_test", cmd_i2cm_datamatch_test, 	"  cmd_i2cm_datamatch_test"},\
							{"cmd_i2c_read", 			cmd_i2c_read, 				"  cmd_i2c_read"},\
							{"cmd_i2c_write", 			cmd_i2c_write, 				"  cmd_i2c_write"},\
							{"cmd_i2cm_timing_test", 	cmd_i2cm_timing_test, 		"  cmd_i2cm_timing_test"},\
						 	{"cmd_i2c_function1_test",	cmd_i2c_function1_test,		"  cmd_i2c_function1_test"},\
							{"cmd_i2c_function2_test", 	cmd_i2c_function2_test,		"  cmd_i2c_function2_test"},
											
#endif /* I2C_CMDLINE_H */

//								{"cmd_i2cs_addrmatch_test", cmd_i2cs_addrmatch_test, 	"  cmd_i2cs_addrmatch_test"},
