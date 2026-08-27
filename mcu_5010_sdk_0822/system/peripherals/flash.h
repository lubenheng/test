/*!
 * @file flash.h
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 19.2, The union keyword should not be used.
 * Use union to describe the parameter of a register structure better
 *
 */

#ifndef FLASH_H
#define FLASH_H

#include "flash_type.h"
#include "flash_reg.h"


void _flash_main_lock(void);
void _flash_info_user_lock(void);
void _flash_info_vendor_lock(void);
void _flash_otp_lock(void);

flash_erc_e _flash_main_unlock(void);
flash_erc_e _flash_info_user_unlock(void);
flash_erc_e _flash_info_vendor_unlock(void);
flash_erc_e _flash_otp_unlock(void);


void _flash_info_refresh(FLASH_TypeDef* flash_ptr);

void flash_drv_ecc_ctl(FLASH_TypeDef* flash_ptr, bool ecc_en);
void flash_drv_sleep_ctl(FLASH_TypeDef* flash_ptr, bool sleep_en);
uint32_t get_flash_intst(flash_type_e flash_type);


flash_erc_e flash_write_operation(uint32_t addr, uint32_t const* data, uint32_t write_len);
flash_erc_e flash_read_operation(FLASH_TypeDef* flash_ptr, uint32_t addr, uint32_t* data, uint32_t read_len);
flash_erc_e flash_erase_operation(FLASH_TypeDef* flash_ptr, uint32_t addr, erase_type_e erase_type, uint32_t wait_time);
void flash_drv_interrupt_cfg(FLASH_TypeDef* flash_ptr, flash_int_type_e flash_int_type);
void flash_drv_mode_update(uint32_t freq);

#endif /*FLASH_H*/
