#ifndef FLASH_TEST_H
#define FLASH_TEST_H




int flash_write_test(int argc, char *argv[]);
int flash_read_test(int argc, char *argv[]);

int flash_otp_write_test(int argc, char *argv[]);
int flash_otp_read_test(int argc, char *argv[]);

int flash_info_user_write_test(int argc, char *argv[]);
int flash_info_user_read_test(int argc, char *argv[]);

int flash_info_vendor_write_test(int argc, char *argv[]);
int flash_info_vendor_read_test(int argc, char *argv[]);

int flash_otp_reboot_test(int argc, char *argv[]);


int anac_read(int argc, char *argv[]);
int syscfg_read(int argc, char *argv[]);

void code_flash_performance_test(uint32_t test_type, uint32_t cmp1us, uint32_t accessmode);
void data_flash_performance_test(uint32_t test_type, uint32_t cmp1us, uint32_t accessmode);
void code_flash_erase_exception_power_down(void);
void data_flash_erase_exception_power_down(void);
void code_flash_write_exception_power_down(void);
void data_flash_write_exception_power_down(void);
void flash_write_exception_power_down(void);
void flash_write_operation_and_interrupt_test(uint32_t sector_index);
void flash_erase_operation_and_interrupt_test(uint32_t sector_index);
#endif

