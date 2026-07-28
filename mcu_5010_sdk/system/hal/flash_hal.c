#include "platform_common.h"

#include "flash_type.h"
#include "flash_reg.h"
#include "flash.h"
#include "flash_hal.h"


// /*!
//  * @file flash_hal.c
//  *
//  */


// #include "platform_common.h"
// #include "flash_reg.h"
// #include "flash.h"
// #include "flash_hal.h"


static FLASH_INT_HANDLE g_flash_int_handle[2U] = {NULL, NULL};

/**
 *@brief     code and data flash have the same controller/interface, but controller base address
 *                  is different, thus before write/read/erase flash, first get base address.
 *@param[in] flash_type: CODE_FLASH, DATA_FLASH
 *@return    FLASH_TypeDef*: pointer, points to the base address of flash
 */
static FLASH_TypeDef* flash_base_addr_get(flash_type_e flash_type)
{
    FLASH_TypeDef* const flash_bases = FLASH_PTR;
    FLASH_TypeDef* flash_ptr = flash_bases;
    return flash_ptr;
}


/**
 *@brief     check flash para whether meets the rule
 *@param[in] flash_type: CODE_FLASH, DATA_FLASH
 *@param[in] addr: start address of data
 *@param[in] len: length of write data
 *@return    flash status:
 *                  - ERC_FLASH_LEN_INVALID,
 *                  - ERC_FLASH_OUT_RANGE,
 *                  - ERC_FLASH_NO_ERROR
 */
//static flash_erc_e flash_check_para(flash_type_e flash_type, uint32_t addr, uint32_t len)
//{
//   
//    flash_erc_e flash_erc = ERC_FLASH_NO_ERROR;
//	 uint32_t size = 0UL;
//    UNUSED(flash_erc);
//    UNUSED(size);
//    /* configure size according to code flash, data flash */
//    switch(flash_type)
//    {
//        case MAIN_FLASH:
//            size = MAIN_FLASH_MAX_SIZE;
//            break;
//        case INFO_FLASH:
//            size = INFO_FLASH_MAX_SIZE;
//            break;
//		case OTP_FLASH:
//            size = OTP_FLASH_MAX_SIZE;
//            break;
//        default:
//            flash_erc = ERC_INVALID;
//            break;
//    }
//    if(ERC_FLASH_NO_ERROR != flash_erc)
//    {
//        /* do nothing */
//    }
//    else if(0UL == len)
//    {
//        flash_erc = ERC_FLASH_LEN_INVALID;
//    }
//    else if((addr + len) > size)
//    {
//        flash_erc = ERC_FLASH_OUT_RANGE;
//    }
//    else
//    {
//        /* do nothing */
//    }
//    return flash_erc;
//}


/**
 *@brief write data to flash
 *@param[in] flash_type: CODE_FLASH, DATA_FALSH
 *@param[in] addr: write start addr, unit:word
 *@param[in] *data: pointer, points to the data to be programmed
 *@param[in] len: number of data, unit:word
 *@return    flash write execute result status:
 *                  - ERC_FLASH_LEN_INVALID,
 *                  - ERC_FLASH_OUT_RANGE,
 *                  - ERC_INVALID,
 *                  - ERC_FLASH_ECC_1BIT,
 *                  - ERC_FLASH_ECC_2BIT,
 *                  - ERC_FLASH_TIME_OUT,
 *                  - ERC_FLASH_NO_ERROR
 */
flash_erc_e flash_write(flash_type_e flash_type, uint32_t addr, const uint32_t *data, uint32_t write_len)
{
    flash_erc_e flash_erc;
    // const uint8_t* src_data = (const uint8_t*)data;
    FLASH_TypeDef *flash_ptr = flash_base_addr_get(MAIN_FLASH);

    // 参数合法性检查
    if (data == NULL || write_len == 0)
    {
        return ERC_FLASH_LEN_INVALID; // 空指针/长度为0
    }

    // 校验FLASH类型
    if (flash_type >= FLASH_TYPE_MAX)
    {
        return ERC_FLASH_TYPE_INVALID;
    }

   REG32(0x40020058) = 1U;   // FLCCKE

    if (flash_type == MAIN_FLASH)
    {
        // 校验地址对齐（4字节）
        if ((addr & 0x03UL) != 0)
        {
            return ERC_FLASH_ADDR_INVALID; // 地址未按4字节对齐（FLASH编程要求）
        }

        // 校验地址范围（起始地址+总长度）
        if (addr >= MAIN_FLASH_MAX_SIZE || (addr + write_len * 4) > MAIN_FLASH_MAX_SIZE)
        {
            return ERC_FLASH_OUT_RANGE;
        }

        //REG32(0x40020058) = 1U;   // FLCCKE

        flash_erc = _flash_main_unlock();

        if (flash_erc != ERC_FLASH_NO_ERROR)
        {
            return ERC_FLASH_CONFLICT;
        }

        FLASH_PTR->CR.w = 1; // PG Operation

        for (uint32_t i = 0; i < write_len; i++)
        {
            // 写入1个word数据（FLASH编程必须按字写入）
            *(volatile uint32_t *)(0x08000000 + addr) = data[i]; // data是const指针，仅读取值

            // 地址偏移4字节（1个word）
            addr += 4;
        }
        /*
        判断数据是否写入进去有三种方式
        1.判断PG ERROR
        2.判断最后一次写入的数据
        3.将所有写入的数据读出进行判断
        */

        /* locks the FLASH controller. */
        _flash_main_lock();
    }
    else if(flash_type == INFO_USER_FLASH)
    {
        // 校验地址对齐（4字节）
        if ((addr & 0x03UL) != 0)
        {
            return ERC_FLASH_ADDR_INVALID; // 地址未按4字节对齐（FLASH编程要求）
        }

        // 校验地址范围（起始地址+总长度）
        if (addr >= INFO_USER_FLASH_MAX_SIZE || (addr + write_len * 4) > INFO_USER_FLASH_MAX_SIZE)
        {
            return ERC_FLASH_OUT_RANGE;
        }

        flash_erc = _flash_info_user_unlock();

        if (flash_erc != ERC_FLASH_NO_ERROR)
        {
            return ERC_FLASH_CONFLICT;
        }

        FLASH_PTR->CR.w |= (1<<0); // INFO PG Operation

        for (uint32_t i = 0; i < write_len; i++)
        {
            // 写入1个word数据（FLASH编程必须按字写入）
            *(volatile uint32_t *)(0x1FFFF800 + addr) = data[i]; // data是const指针，仅读取值
            //INFO("user_info_write address:0x%x,val:0x%x\n",(0x1FFFF800 + addr),data[i]);

            // 地址偏移4字节（1个word）
            addr += 4;
        }
        /*
        判断数据是否写入进去有三种方式
        1.判断PG ERROR
        2.判断最后一次写入的数据
        3.将所有写入的数据读出进行判断
        */

        /* locks the FLASH controller. */
        _flash_info_user_lock();

    }
    else if(flash_type == INFO_VENDOR_FLASH)
    {
        // 校验地址对齐（4字节）
        if ((addr & 0x03UL) != 0)
        {
            return ERC_FLASH_ADDR_INVALID; // 地址未按4字节对齐（FLASH编程要求）
        }

         // 校验地址范围（起始地址+总长度）
        if(  (addr >= INFO_VENDOR_FLASH_MAX_SIZE) || ((addr + write_len * 4) > INFO_VENDOR_FLASH_MAX_SIZE))
        {
            return ERC_FLASH_OUT_RANGE;
        }

        flash_erc = _flash_info_vendor_unlock();

        if (flash_erc != ERC_FLASH_NO_ERROR)
        {
            return ERC_FLASH_CONFLICT;
        }

        FLASH_PTR->CR.w = (1<<0); // INFO PG Operation

        for (uint32_t i = 0; i < write_len; i++)
        {
            //INFO("vendor_info_write address:0x%x,val:0x%x\n",(0x1FFFF810 + addr),data[i]);
            // 写入1个word数据（FLASH编程必须按字写入）
            *(volatile uint32_t *)(0x1FFFF810 + addr) = data[i]; // data是const指针，仅读取值

            // 地址偏移4字节（1个word）
            addr += 4;
        }
        /*
        判断数据是否写入进去有三种方式
        1.判断PG ERROR
        2.判断最后一次写入的数据
        3.将所有写入的数据读出进行判断
        */

        /* locks the FLASH controller. */
        _flash_info_vendor_lock();
        
    }
    else if(flash_type == OTP_FLASH)
    {
         // 校验地址范围（起始地址+总长度）
        if (addr >= OTP_FLASH_MAX_SIZE || (addr + write_len * 1) > OTP_FLASH_MAX_SIZE)
        {
            return ERC_FLASH_OUT_RANGE;
        }

        for (uint32_t i = 0; i < write_len; i++)
        {
            flash_erc = _flash_otp_unlock();

            if (flash_erc != ERC_FLASH_NO_ERROR)
            {
                return ERC_FLASH_CONFLICT;
            }
            /* 1.write destination address */
            FLASH_PTR->OTPAR.OTP_ADDRESS = addr&0x7f;
            /* 2.write destination data */
            FLASH_PTR->OTP_DATA_REG = data[i]; // data是const指针，仅读取值
            /* 3.program destination data */
            FLASH_PTR->OTPCR.w = 1; // OTP PG Operation

            INFO("otp_write address:0x%x,val:0x%x\n",(0x8000 + (addr&0x7f)),data[i]);
            // 地址偏移4字节（1个word）
            addr += 1;

            /* locks the FLASH controller. */
            _flash_otp_lock();
        }
        /*
        判断数据是否写入进去有三种方式
        1.判断PG ERROR
        2.判断最后一次写入的数据
        3.将所有写入的数据读出进行判断
        */
    }
    else
    {
        INFO("ERC_FLASH_TYPE_INVALID \n");
    }

    return ERC_FLASH_NO_ERROR;
}

/**
 *@brief read data from flash
 *@param[in]  flash_type: CODE_FLASH, DATA_FALSH
 *@param[in]  addr: read start addr, unit:byte
 *@param[out] *data: pointer, points to the readback data
 *@param[in]  len: number of data, unit:byte
 *@return     flash read execute result status:
 *                  - ERC_FLASH_LEN_INVALID,
 *                  - ERC_FLASH_OUT_RANGE,
 *                  - ERC_INVALID,
 *                  - ERC_FLASH_TIME_OUT,
 *                  - ERC_FLASH_NO_ERROR
 */
flash_erc_e flash_read_by_apb(flash_type_e flash_type, uint32_t addr, uint8_t* data, uint32_t len)
{
    flash_erc_e flash_erc = ERC_FLASH_NO_ERROR;
//    uint32_t target_len = len;
//    uint32_t target_addr = addr;
//    uint8_t* destination_data = data;
//    flash_erc = flash_check_para(flash_type, target_addr, target_len);
//    if(ERC_FLASH_NO_ERROR == flash_erc)
//    {
//        FLASH_TypeDef* flash_ptr = flash_base_addr_get(flash_type);
//        uint8_t single_operation_byte_length = ((INFO_FLASH != flash_type) ? CODE_FLASH_RW_LEN_BYTE : DATA_FLASH_RW_LEN_BYTE);
//        uint8_t flash_slot_mask = ((INFO_FLASH != flash_type) ? (uint8_t)CODE_FLASH_SLOT_MASK : (uint8_t)DATA_FLASH_SLOT_MASK);
//        uint8_t single_convert_length;
//        uint8_t unaligned_byte_idx;
//        uint32_t flash_single_read_buff[CODE_FLASH_RW_LEN + DATA_FLASH_RW_LEN] = {0UL};
//        /* unlocks the FLASH controller. */
//        _flash_main_unlock();
//        while(0UL < target_len)
//        {
//            /* read a slot from flash once */
//            flash_erc = flash_read_operation(flash_ptr, (uint32_t)(target_addr / single_operation_byte_length), flash_single_read_buff,
//                                             single_operation_byte_length);
//            if((ERC_FLASH_TIME_OUT == flash_erc) || (ERC_FLASH_ECC_2BIT == flash_erc))
//            {
//                break;
//            }
//            unaligned_byte_idx = (uint8_t)(target_addr & COMMON_UINT8_CONVERT_UINT32(flash_slot_mask));
//            if(0UL < unaligned_byte_idx) /* unaligned header */
//            {
//                single_convert_length = ((((uint32_t)unaligned_byte_idx + target_len) > (uint32_t)single_operation_byte_length) ?
//                                         (single_operation_byte_length - unaligned_byte_idx) : (uint8_t)target_len);
//                (void)sy_memcpy_s(destination_data, single_convert_length, (((uint8_t*)flash_single_read_buff) + unaligned_byte_idx), single_convert_length);
//            }
//            else if(single_operation_byte_length < target_len) /* aligned header & unaligned tail */
//            {
//                single_convert_length = single_operation_byte_length;
//                (void)sy_memcpy_s(destination_data, single_convert_length, (uint8_t*)flash_single_read_buff, single_convert_length);
//            }
//            else /* aligned header & aligned tail */
//            {
//                single_convert_length = (uint8_t)target_len;
//                (void)sy_memcpy_s(destination_data, single_convert_length, (uint8_t*)flash_single_read_buff, single_convert_length);
//            }
//            target_addr += single_convert_length;
//            destination_data += single_convert_length;
//            target_len -= single_convert_length;
//        }
//        /* locks the FLASH controller. */
//        _flash_main_lock();
//    }
    return flash_erc;
}

/**
 *@brief read data from flash
 *@param[in]  flash_type: CODE_FLASH, DATA_FALSH
 *@param[in]  addr: read start addr, unit:word
 *@param[out] *data: pointer, points to the readback data
 *@param[in]  len: number of data, unit:byte
 *@return     flash read execute result status:
 *                  - ERC_FLASH_TYPE_INVALID,
 *                  - ERC_FLASH_ADDR_INVALID,
 *                  - ERC_FLASH_OUT_RANGE,
 *                  - ERC_FLASH_LEN_INVALID,
 *                  - ERC_FLASH_NO_ERROR
 */
flash_erc_e flash_read_by_ahb(flash_type_e flash_type, uint32_t addr, uint32_t* data, uint32_t len)
{
    // 1. 初始化返回状态
    flash_erc_e read_erc = ERC_FLASH_NO_ERROR;
    
    // 2. 参数合法性校验
    /* 2.1 校验FLASH类型：仅支持MAIN_FLASH */
    if (flash_type >= FLASH_TYPE_MAX)
    {
        return ERC_FLASH_TYPE_INVALID;
    }

    /* 2.2 校验输出缓冲区：禁止NULL指针 */
    if (data == NULL)
    {
        return ERC_FLASH_LEN_INVALID;  // 无有效缓冲区，归类为长度无效
    }
    
    /* 2.3 校验读取长度：禁止0长度 */
    if (len == 0)
    {
        return ERC_FLASH_LEN_INVALID;
    }

    /* 2.4 校验地址对齐：MAIN_FLASH读取需4字节对齐（word级别） */
    if ((addr & 0x03UL) != 0)
    {
        return ERC_FLASH_ADDR_INVALID;
    }

    if (flash_type == MAIN_FLASH)
    {
        /* 2.5 校验地址范围：确保addr + len*4 不超出MAIN_FLASH范围 */
        if ((addr >= MAIN_FLASH_MAX_SIZE) || ((addr + len * 4) > MAIN_FLASH_MAX_SIZE))
        {
            return ERC_FLASH_OUT_RANGE;
        }

        // 3. 清除历史ECC错误标志
        // flash_clear_ecc_flag();

        // 4. 逐字读取MAIN_FLASH数据（AHB总线直接访问）
        uint32_t flash_abs_addr = 0x08000000 + addr; // 绝对地址
        for (uint32_t i = 0; i < len; i++)
        {
            /* 核心读取操作：按芯片要求的格式读取 */
            data[i] = *(volatile uint32_t *)(flash_abs_addr);

            /* 检查当前读取是否触发ECC错误（2bit错误需立即终止） */
            //read_erc = flash_check_ecc_error();
            //if (read_erc == ERC_FLASH_ECC_2BIT)
            //{
            //    return read_erc; // 2bit ECC错误无法纠正，直接返回
            //}

            /* 地址偏移：1个word = 4字节 */
            flash_abs_addr += 4;
        }
    }
    else if(flash_type == INFO_USER_FLASH)
    {
        /* 2.5 校验地址范围：确保addr + len*4 不超出MAIN_FLASH范围 */
        if ((addr >= INFO_USER_FLASH_MAX_SIZE) || ((addr + len * 4) > INFO_USER_FLASH_MAX_SIZE))
        {
            return ERC_FLASH_OUT_RANGE;
        }

        // 3. 清除历史ECC错误标志
        // flash_clear_ecc_flag();

        // 4. 逐字读取MAIN_FLASH数据（AHB总线直接访问）
        uint32_t flash_abs_addr = 0x1FFFF800 + addr; // 绝对地址
        for (uint32_t i = 0; i < len; i++)
        {
            /* 核心读取操作：按芯片要求的格式读取 */
            data[i] = *(volatile uint32_t *)(flash_abs_addr);

            /* 检查当前读取是否触发ECC错误（2bit错误需立即终止） */
            //read_erc = flash_check_ecc_error();
            //if (read_erc == ERC_FLASH_ECC_2BIT)
            //{
            //    return read_erc; // 2bit ECC错误无法纠正，直接返回
            //}

            /* 地址偏移：1个word = 4字节 */
            flash_abs_addr += 4;
        }
    }
    else if (flash_type == INFO_VENDOR_FLASH)
    {
        /* 2.5 校验地址范围：确保addr + len*4 不超出MAIN_FLASH范围 */
        if ((addr >= INFO_VENDOR_FLASH_MAX_SIZE) || ((addr + len * 4) > INFO_VENDOR_FLASH_MAX_SIZE))
        {
            return ERC_FLASH_OUT_RANGE;
        }

        // 3. 清除历史ECC错误标志
        // flash_clear_ecc_flag();

        // 4. 逐字读取MAIN_FLASH数据（AHB总线直接访问）
        uint32_t flash_abs_addr = 0x1FFFF810 + addr; // 绝对地址
        for (uint32_t i = 0; i < len; i++)
        {
            /* 核心读取操作：按芯片要求的格式读取 */
            data[i] = *(volatile uint32_t *)(flash_abs_addr);

            /* 检查当前读取是否触发ECC错误（2bit错误需立即终止） */
            //read_erc = flash_check_ecc_error();
            //if (read_erc == ERC_FLASH_ECC_2BIT)
            //{
            //    return read_erc; // 2bit ECC错误无法纠正，直接返回
            //}

            /* 地址偏移：1个word = 4字节 */
            flash_abs_addr += 4;
        }
    }
    else if(flash_type == OTP_FLASH)
    { 

    }
    else
    {
        INFO("ERC_FLASH_TYPE_INVALID");
    }
    

    // 5. 返回最终状态（1bit ECC错误可纠正，正常返回但标记错误）
    return read_erc;
}

/**
 *@brief read data from flash
 *@param[in]  addr: read start addr, unit:word
 *@param[out] *data: pointer, points to the readback data
 *@param[in]  len: number of data, unit:byte
 *@return     flash read execute result status:
 *                  - ERC_FLASH_TYPE_INVALID,
 *                  - ERC_FLASH_ADDR_INVALID,
 *                  - ERC_FLASH_OUT_RANGE,
 *                  - ERC_FLASH_LEN_INVALID,
 *                  - ERC_FLASH_NO_ERROR
 */
flash_erc_e syscfg_read_by_ahb(uint32_t addr, uint32_t* data, uint32_t len)
{
    // 1. 初始化返回状态
    flash_erc_e read_erc = ERC_FLASH_NO_ERROR;
    
    // 2. 参数合法性校验
    /* 2.1 校验输出缓冲区：禁止NULL指针 */
    if (data == NULL)
    {
        return ERC_FLASH_LEN_INVALID;  // 无有效缓冲区，归类为长度无效
    }
    
    /* 2.2 校验读取长度：禁止0长度 */
    if (len == 0)
    {
        return ERC_FLASH_LEN_INVALID;
    }

    /* 2.3 校验地址对齐：MAIN_FLASH读取需4字节对齐（word级别） */
    if ((addr & 0x03UL) != 0)
    {
        return ERC_FLASH_ADDR_INVALID;
    }

    /* 2.5 校验地址范围：确保addr + len*4 不超出MAIN_FLASH范围 */
    if (addr >= 0x400)
    {
        return ERC_FLASH_OUT_RANGE;
    }

    // 3. 逐字读取MAIN_FLASH数据（AHB总线直接访问）
    uint32_t flash_abs_addr = 0x40020400 + addr; // 绝对地址
    for (uint32_t i = 0; i < len; i++)
    {
        /* 核心读取操作：按芯片要求的格式读取 */
        data[i] = *(volatile uint32_t *)(flash_abs_addr);

        /* 地址偏移：1个word = 4字节 */
        flash_abs_addr += 4;
    }

    // 4. 返回最终状态（1bit ECC错误可纠正，正常返回但标记错误）
    return read_erc;
}

/**
 *@brief read data from flash
 *@param[in]  addr: read start addr, unit:word
 *@param[out] *data: pointer, points to the readback data
 *@param[in]  len: number of data, unit:byte
 *@return     flash read execute result status:
 *                  - ERC_FLASH_TYPE_INVALID,
 *                  - ERC_FLASH_ADDR_INVALID,
 *                  - ERC_FLASH_OUT_RANGE,
 *                  - ERC_FLASH_LEN_INVALID,
 *                  - ERC_FLASH_NO_ERROR
 */
flash_erc_e anac_read_by_ahb(uint32_t addr, uint32_t* data, uint32_t len)
{
    // 1. 初始化返回状态
    flash_erc_e read_erc = ERC_FLASH_NO_ERROR;
    
    // 2. 参数合法性校验
    /* 2.1 校验输出缓冲区：禁止NULL指针 */
    if (data == NULL)
    {
        return ERC_FLASH_LEN_INVALID;  // 无有效缓冲区，归类为长度无效
    }
    
    /* 2.2 校验读取长度：禁止0长度 */
    if (len == 0)
    {
        return ERC_FLASH_LEN_INVALID;
    }

    /* 2.3 校验地址对齐：MAIN_FLASH读取需4字节对齐（word级别） */
    if ((addr & 0x03UL) != 0)
    {
        return ERC_FLASH_ADDR_INVALID;
    }

    /* 2.5 校验地址范围：确保addr + len*4 不超出MAIN_FLASH范围 */
    if (addr >= 0x50)
    {
        return ERC_FLASH_OUT_RANGE;
    }

    // 3. 逐字读取MAIN_FLASH数据（AHB总线直接访问）
    uint32_t flash_abs_addr = 0x40007C00 + addr; // 绝对地址
    for (uint32_t i = 0; i < len; i++)
    {
        /* 核心读取操作：按芯片要求的格式读取 */
        data[i] = *(volatile uint32_t *)(flash_abs_addr);

        /* 地址偏移：1个word = 4字节 */
        flash_abs_addr += 4;
    }

    // 4. 返回最终状态（1bit ECC错误可纠正，正常返回但标记错误）
    return read_erc;
}

/**
 *@brief erase operation controller
 *@param[in] flash_type: CODE_FLASH, DATA_FLASH
 *@param[in] erase_type:
 *              - ERASE_CHIP: chip erase
 *              - ERASE_SECTOR: sector erase
 *@param[in] sector_idx: sector series number, start from 0
 *@return    flash erase execute result status:
 *                  - ERC_FLASH_TYPE_INVALID,
 *                  - ERC_FLASH_TIME_OUT,
 *                  - ERC_FLASH_NO_ERROR,
 */
flash_erc_e flash_erase(flash_type_e flash_type, erase_type_e erase_type, uint32_t sector_idx)
{
    flash_erc_e flash_erc = ERC_FLASH_NO_ERROR;
//    uint32_t sector_slot_num = 0UL;
//    uint32_t flash_sector_num = 0UL;
//    if(FLASH_TYPE_MAX <= flash_type)
//    {
//        flash_erc = ERC_FLASH_TYPE_INVALID;
//    }
//    else if(MAIN_FLASH == flash_type)
//    {
//        sector_slot_num = CODE_FLASH_SLOT_NUMBER;
//        flash_sector_num = CODE_FLASH_SECTOR_NUMBER;
//    }
//    else
//    {
//        sector_slot_num = DATA_FLASH_SLOT_NUMBER;
//        flash_sector_num = DATA_FLASH_SECTOR_NUMBER;
//    }
//    if(ERC_FLASH_NO_ERROR != flash_erc)
//    {
//        /* do nothing */
//    }
//    else if(sector_idx >= flash_sector_num)
//    {
//        flash_erc = ERC_FLASH_SECTOR_INDEX_INVALID;
//    }
//    else
//    {
//        FLASH_TypeDef* flash_ptr = flash_base_addr_get(flash_type);
//        uint32_t wait_time = 0UL;
//        uint32_t addr = 0UL;
//        /* set erased first addr according to erase type */
//        switch(erase_type)
//        {
//            case ERASE_CHIP:
//                addr = 0U;
//                wait_time = FLASH_CHIP_ERASE_TIME_OUT;
//                break;
//            case ERASE_SECTOR:
//                addr = (sector_idx * sector_slot_num);
//                wait_time = FLASH_SECTOR_ERASE_TIME_OUT;
//                break;
//            default:
//                flash_erc = ERC_FLASH_TYPE_INVALID;
//                break;
//        }
//        if(ERC_FLASH_NO_ERROR == flash_erc)
//        {
//            _flash_main_unlock();
//            flash_erc = flash_erase_operation(flash_ptr, addr, erase_type, wait_time);
//            _flash_main_lock();
//        }
//    }
    return flash_erc;
}


/**
 *@brief control flash ecc function
 *@param[in] flash_type: CODE_FLASH, DATA_FALSH
 *@return    none
 */
void flash_ecc_ctl(flash_type_e flash_type, bool ecc_en)
{
    FLASH_TypeDef* flash_ptr = flash_base_addr_get(flash_type);
    flash_drv_ecc_ctl(flash_ptr, ecc_en);
}


/**
 *@brief     control flash sleep function
 *@param[in] flash_type: CODE_FLASH, DATA_FALSH
 *@param[in] sleep_en: control flash enter low power
 *@return    none
 */
void flash_sleep_ctl(flash_type_e flash_type, bool sleep_en)
{
    FLASH_TypeDef* flash_ptr = flash_base_addr_get(flash_type);
    flash_drv_sleep_ctl(flash_ptr, sleep_en);
}


/*
 *@brief change flash work mode, according to sys clock frequency
 *@param[in] freq: sys freq, unit: Hz
 *@return    none
 */
void flash_mode_update(uint32_t freq)
{
    flash_drv_mode_update(freq);
}


/**
 *@brief flash interrupt init
 *@param  *flash_config: pointer to the struct of flash interrupt configurations
 *@return none
 */
void flash_interrupt_cfg(flash_int_config_t* flash_config)
{
#if 0
    DEV_ASSERT(flash_config != NULL);
    DEV_ASSERT(flash_config->flash_type < FLASH_TYPE_MAX);
    DEV_ASSERT(flash_config->flash_int_type < FLASH_INT_TPYE_INVALID);
    const IRQn_Type flash_irqn[FLASH_INSTANCE_MAX_COUNT] = FLASH_IRQS;
    FLASH_TypeDef* flash_ptr = flash_base_addr_get(flash_config->flash_type);
    g_flash_int_handle[flash_config->flash_type] = flash_config->int_handle;
    flash_drv_interrupt_cfg(flash_ptr, flash_config->flash_int_type);
    if(NVIC_GetEnableIRQ(flash_irqn[flash_config->flash_type]) == 0UL)
    {
        NVIC_EnableIRQ(flash_irqn[flash_config->flash_type]);
        NVIC_SetPriority(flash_irqn[flash_config->flash_type], flash_config->priority);
    }
#endif
}


/**
 *@brief code flash interrupt isr
 *@param  none
 *@return none
 */
void CODE_FLASH_IRQHandler(void)
{
    uint32_t intsts;
    uint32_t status = 0U;
    intsts = get_flash_intst(MAIN_FLASH);
    CODE_FLASH_PTR->SR.w = intsts;
    if((intsts & FLASH_SR_ECC1E_MASK) == FLASH_SR_ECC1E_MASK)
    {
        status = COMMON_UINT8_CONVERT_UINT32(ERC_FLASH_ECC_1BIT);
    }
    else if((intsts & FLASH_SR_ECC2E_MASK) == FLASH_SR_ECC2E_MASK)
    {
        status = COMMON_UINT8_CONVERT_UINT32(ERC_FLASH_ECC_2BIT);
    }
    else if((intsts & FLASH_SR_COMP_MASK) == FLASH_SR_COMP_MASK)
    {
        status = COMMON_UINT8_CONVERT_UINT32(ERC_FLASH_NO_ERROR);
    }
    else
    {
        /* do nothing */
        UNUSED(status);
    }
    if(NULL != g_flash_int_handle[0U])
    {
        g_flash_int_handle[0U](status);
    }
}

/**
 *@brief data flash interrupt isr
 *@param  none
 *@return none
 */
void DATA_FLASH_IRQHandler(void)
{
    uint32_t intsts ;
    uint32_t status = 0U;
    UNUSED(status);
    intsts = get_flash_intst(INFO_USER_FLASH);
    DATA_FLASH_PTR->SR.w = intsts;
    if(FLASH_SR_ECC1E_MASK == (intsts & (FLASH_SR_ECC1E_MASK)))
    {
        status = COMMON_UINT8_CONVERT_UINT32(ERC_FLASH_ECC_1BIT);
    }
    else if((intsts & FLASH_SR_ECC2E_MASK) == FLASH_SR_ECC2E_MASK)
    {
        status = COMMON_UINT8_CONVERT_UINT32(ERC_FLASH_ECC_2BIT);
    }
    else if((intsts & FLASH_SR_COMP_MASK) == FLASH_SR_COMP_MASK)
    {
        status = COMMON_UINT8_CONVERT_UINT32(ERC_FLASH_NO_ERROR);
    }
    else
    {
        /* do nothing */
        UNUSED(status);
    }
    if(NULL != g_flash_int_handle[1U])
    {
        g_flash_int_handle[1U](status);
    }
}

