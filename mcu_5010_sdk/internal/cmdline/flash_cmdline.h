#ifndef FLASH_CMDLINE_H
#define FLASH_CMDLINE_H

int cmd_flash_performance_test(int argc, char *argv[]);
int cmd_flash_erase_operator_interrupt_test(int argc, char *argv[]);
int cmd_flash_write_operator_interrupt_test(int argc, char *argv[]);
int cmd_flash_deepsleep_test(int argc, char *argv[]);
int cmd_flash_erase_exception_power_down_test(int argc, char *argv[]);
int cmd_flash_write_exception_power_down_test(int argc, char *argv[]);

int cmd_flash_write_test(int argc, char *argv[]);
int cmd_flash_read_test(int argc, char *argv[]);
int cmd_flash_info_user_write_test(int argc, char *argv[]);
int cmd_flash_info_user_read_test(int argc, char *argv[]);

int cmd_flash_info_vendor_write_test(int argc, char *argv[]);
int cmd_flash_info_vendor_read_test(int argc, char *argv[]);
int cmd_flash_otp_write_test(int argc, char *argv[]);
int cmd_flash_otp_read_test(int argc, char *argv[]);

int cmd_flash_otp_reboot_test(int argc, char *argv[]);


int cmd_flash_otp_reboot_test(int argc, char *argv[]);

int cmd_anac_read(int argc, char *argv[]);
int cmd_syscfg_read(int argc, char *argv[]);

#define FLASH_CMDLINE_MENU 		{"cmd_flash_write_test",	      		  cmd_flash_write_test,								"	flash_write:reg_address,len,val0,val1..."},\
								{"cmd_flash_read_test", 		  		  cmd_flash_read_test,								"	flash_read:reg_address,len"},\
								{"cmd_flash_info_user_write_test",	      cmd_flash_info_user_write_test,					"	flash_write:reg_address,len,val0,val1..."},\
								{"cmd_flash_info_user_read_test", 		  cmd_flash_info_user_read_test,					"	flash_read:reg_address,len"},\
								{"cmd_flash_info_vendor_write_test",	  cmd_flash_info_vendor_write_test,					"	flash_write:reg_address,len,val0,val1..."},\
								{"cmd_flash_info_vendor_read_test", 	  cmd_flash_info_vendor_read_test,					"	flash_read:reg_address,len"},\
								{"cmd_flash_otp_write_test",	      	  cmd_flash_otp_write_test,							"	flash_write:reg_address,len,val0,val1..."},\
								{"cmd_flash_otp_read_test", 		 	  cmd_flash_otp_read_test,							"	flash_read:reg_address,len"},\
								{"cmd_flash_otp_reboot_test", 		 	  cmd_flash_otp_reboot_test,						"	none"},\
								{"cmd_anac_read", 		 	 			  cmd_anac_read,									"	flash_read:reg_address,len"},\
								{"cmd_syscfg_read", 		 	 		  cmd_syscfg_read,									"	flash_read:reg_address,len"},\

								// {"cmd_flash_performance_test",				 		 cmd_flash_performance_test,						"	none"},\
								// {"cmd_flash_erase_operator_interrupt_test",		     cmd_flash_erase_operator_interrupt_test,			"	none"},\
								// {"cmd_flash_write_operator_interrupt_test",		     cmd_flash_write_operator_interrupt_test,			"	none"},\
								// {"cmd_flash_deepsleep_test",		     			 cmd_flash_deepsleep_test,							"	none"},\
								// {"cmd_flash_erase_exception_power_down_test",		 cmd_flash_erase_exception_power_down_test,			"	none"},\
								// {"cmd_flash_write_exception_power_down_test",		 cmd_flash_write_exception_power_down_test,			"	none"},\



#endif /*CMDLINE_H*/

