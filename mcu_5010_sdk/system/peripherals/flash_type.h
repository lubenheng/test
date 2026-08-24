#ifndef FLASH_TYPE_H
#define FLASH_TYPE_H

/** falsh instance enum*/
typedef enum
{
    MAIN_FLASH = 0U,
    INFO_USER_FLASH,
    INFO_VENDOR_FLASH,
    OTP_FLASH,
    FLASH_TYPE_MAX
} flash_type_e;

/** flash execute status*/
typedef enum
{
    /* flash error status */
    ERC_FLASH_NO_ERROR = 0U, /*!< flash access success */
    ERC_FLASH_ECC_1BIT = 1U, /*!< read data has one bit error */
    ERC_FLASH_ECC_2BIT = 2U, /*!< read data has two bits error */
    ERC_FLASH_TYPE_INVALID = 3U, /*!< invalid flash type */
    ERC_FLASH_ADDR_INVALID = 4U, /*!< invalid addr */
    ERC_FLASH_LEN_INVALID = 5U, /*!< invalid length */
    ERC_FLASH_OUT_RANGE = 6U, /*!< addr+length is beyond flash range */
    ERC_FLASH_TIME_OUT = 7U, /*!< flash operation is time out */
    ERC_FLASH_CONFLICT = 8U, /*!< access flash is conflict */
    ERC_FLASH_SECTOR_INDEX_INVALID = 9U, /*!< user sector index invalid */
    ERC_INVALID
} flash_erc_e;

/** erase cmd code */
typedef enum
{
    ERASE_SECTOR = 0x5,
    ERASE_CHIP
} erase_type_e;

/** flash interrupt type */
typedef enum
{
    FLASH_INT_TYPE_ERR1BIT = 0U,
    FLASH_INT_TYPE_ERR2BIT,
    FLASH_INT_TYPE_COMPLETE,
    FLASH_INT_TYPE_ALL = 0xF,
    FLASH_INT_TPYE_INVALID,
} flash_int_type_e;


/** flash work in corresponding mode for different system clock freq, refer #FALSH_FAST_FREQ
  * and FALSH_MIDDLE_FREQ and #DATA_FALSH_SLOW_FREQ
  */
typedef enum
{
    MID_MODE = 0U,
    LONG_MODE,
    SHORT_MODE,
    MODE_MAX
} access_mode_e;

typedef void (*FLASH_INT_HANDLE)(uint32_t status);

/** flash interrupt configure settings */
typedef struct
{
    flash_type_e flash_type;
    flash_int_type_e flash_int_type;
    uint32_t priority;
    FLASH_INT_HANDLE int_handle;
} flash_int_config_t;

/** flash firmware config  */
typedef struct
{
    uint32_t firmware_start_address; /** uint is word  */
    uint32_t firmware_start_address_copy; /** uint is word  */
    uint32_t firmware_size; /** uint is word  */
    uint32_t firmware_size_copy; /** uint is word  */
} flash_info_t;

typedef struct
{
    uint32_t data0; /** uint is word  */
    uint32_t data1; /** uint is word  */
    uint32_t data2; /** uint is word  */
    uint32_t data3; /** uint is word  */
} main_flash_t;

#endif
