/*!
 * @file flash_hal.h
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 4.9, A function should be used in preference to a function-like macro where they are interchangeable.
 * Use macro to speed up program processing
 *
 */

#ifndef FLASH_HAL_H
#define FLASH_HAL_H


#include "flash_type.h"


/* NVR1 silergy use sector  */
#define SILERGY_INFO_SECTOR_SIZE    (512U)      /*uint is byte*/
#define SILERGY_INFO_LOCATION       (SILERGY_INFO_SECTOR_SIZE*1U) /*FW info located in the sector 1*/

/* NVR2 secure boot sector  */
#define BOOT_INFO_SECTOR_SIZE                       (512U)      /*uint is byte*/
#define BOOT_INFO_LOCATION                          (BOOT_INFO_SECTOR_SIZE*2U) /*FW info located in the sector 2*/
#define BOOT_INFO_FW_START_LOCATION                 (BOOT_INFO_LOCATION + (28*4U)) /*Boot Firmware Start Address*/

/* NVR3 firmware version sector */
#define FW_INFO_SECTOR_SIZE                         (512U)      /*uint is byte*/
#define FW_INFO_LOCATION                            (FW_INFO_SECTOR_SIZE*3U) /*FW info located in the sector 3*/

/* mian flash sector operator*/
#define MAIN_FLASH_SECTOR_SIZE               (512U)/* uint is byte */

#define BOOT_FLASH_OPERATION_SECTOR          (240U)/* uint is sector */
#define BOOT_FLASH_OPERATION_LOCATION        (MAIN_FLASH_SECTOR_SIZE*BOOT_FLASH_OPERATION_SECTOR + 0U)/* 120KB start region in number sector 241 */
#define APP_FLASH_OPERATION_SECTOR           (60U)/* uint is sector */
#define APP_FLASH_OPERATION_LOCATION         (MAIN_FLASH_SECTOR_SIZE*APP_FLASH_OPERATION_SECTOR + 0U) /* 30KB start region in number sector 61 */


flash_erc_e flash_write(flash_type_e flash_type, uint32_t addr, const uint32_t *data, uint32_t write_len);
flash_erc_e flash_read_by_apb(flash_type_e flash_type, uint32_t addr, uint8_t* data, uint32_t len);
flash_erc_e flash_read_by_ahb(flash_type_e flash_type, uint32_t addr, uint32_t* data, uint32_t len);
flash_erc_e flash_erase(flash_type_e flash_type, erase_type_e erase_type, uint32_t sector_idx);

flash_erc_e anac_read_by_ahb(uint32_t addr, uint32_t* data, uint32_t len);
flash_erc_e syscfg_read_by_ahb(uint32_t addr, uint32_t* data, uint32_t len);

void flash_ecc_ctl(flash_type_e flash_type, bool ecc_en);
void flash_sleep_ctl(flash_type_e flash_type, bool sleep_en);
void flash_mode_update(uint32_t freq);
void flash_interrupt_cfg(flash_int_config_t* flash_config);


	

#endif/* FLASH_HAL_H */
