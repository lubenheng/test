/*!
 * @file flash.c
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 19.2, The union keyword should not be used.
 * Use union to describe the parameter of a register structure better
 *
 */

#include "platform_common.h"
#include "flash_reg.h"
#include "flash.h"

//#include "flash_type.h"
//#include "flash_reg.h"
//#include "device_resource.h"
//#include "flash.h"




/**
 *@brief     check flash operation such as read/write/erase complete
 *@param[in] flash_ptr: pointer, points to the base address of flash
 *@param[in] wait_time: wait access flash complete time
 *@return    wait_ret: flash access status
 */
//static bool flash_access_complete_check(FLASH_TypeDef* flash_ptr, uint32_t wait_time)
//{
//	#if 0
//	
//    bool wait_ret = wait_till_timeout(&flash_ptr->SR.w, FLASH_SR_COMP_MASK, FLASH_SR_COMP_MASK, wait_time);
//    flash_ptr->SR.w = (WRITE_1_CLEAR << FLASH_SR_COMP_POS); /*clear this flag anyway even though wait timeout*/
//    return wait_ret;
//	#endif
//	
//	return (bool)0;
//}

/**
 *@brief     write operation controller
 *@param[in] addr: start address of writed data
 *@param[in] *data: pointer, points to the writed data
 *@param[in] operation_type: write operation
 *@return    flash status: ERC_FLASH_TIME_OUT, ERC_FLASH_NO_ERROR
 */
flash_erc_e flash_write_operation(uint32_t write_addr, uint32_t const* data, uint32_t write_len)
{
    #if 0
    flash_erc_e flash_erc = ERC_FLASH_NO_ERROR;

    for (uint32_t i = 0; i < write_len; i++) 
    {
        // 检查FLASH是否忙
        // if (flash_wait_operation(1000UL) != 0) {
        //     FLASH_CR &= ~FLASH_CR_PG;
        //     flash_lock();
        //     return 5;  // 超时
        // }

        // 写入1个word数据（FLASH编程必须按字写入）
        *(volatile uint32_t *)(0x08000000 + write_addr) = data[i];  // data是const指针，仅读取值
        
        // 等待写入完成
        // uint8_t ret = flash_wait_operation(10000UL);
        // if (ret != 0) {
        //     FLASH_CR &= ~FLASH_CR_PG;
        //     flash_lock();
        //     return (ret == 1) ? 5 : 4;  // 超时/编程错误
        // }

        // 地址偏移4字节（1个word）
        write_addr += 4;
    }

    return flash_erc;
	
	#endif

	#if 0
	
    (void)write_len;
    flash_erc_e flash_erc = ERC_FLASH_NO_ERROR;
    /*clear all flags before access flash*/
    flash_ptr->SR.w = ~0U;
    flash_ptr->ADDR = addr;
    flash_ptr->DATA0 = data[0U];
    /* configure mode: write */
    flash_ptr->CTRL.w = FLASH_CTRL_KEY | FLASH_OP_MAIN_W;
    __DSB();
    __ISB();
    if(false == flash_access_complete_check(flash_ptr, FLASH_WR_TIME_OUT))
    {
        flash_erc = ERC_FLASH_TIME_OUT;
    }
    return flash_erc;
	
	#endif

	
	return (flash_erc_e)0;
}

/**
 *@brief      read opereation controller
 *@param[in]  flash_ptr: pointer, points to the base address of flash
 *@param[in]  addr: start address of writed data
 *@param[out] *data: pointer, points to the readback data
 *@param[in]  operation_type: read operation
 *@return     flash status:
 *                  ERC_FLASH_TIME_OUT,
 *                  ERC_FLASH_ECC_1BIT_ERR,
 *                  ERC_FLASH_ECC_2BIT_ERR,
 *                  ERC_FLASH_NO_ERROR
 */
flash_erc_e flash_read_operation(FLASH_TypeDef* flash_ptr, uint32_t addr, uint32_t* data, uint32_t read_len)
{
	#if 0
	
    (void)read_len;
    flash_erc_e flash_erc;
    flash_ptr->ADDR = addr;
    /*clear all flags before access flash*/
    flash_ptr->SR.w = ~0U;
    /* configure mode: read */
    flash_ptr->CTRL.w = FLASH_CTRL_KEY | FLASH_OP_MAIN_R;
    __DSB();
    __ISB();
    if(false == flash_access_complete_check(flash_ptr, FLASH_WR_TIME_OUT))
    {
        flash_erc = ERC_FLASH_TIME_OUT;
    }
    else
    {
        /* wait flash idle status, judge whether read data has error.
         * if ERR1BIT_STS set, means read data has one bit error
         */
        data[0] = flash_ptr->DATA0;
        if((flash_ptr->SR.w & FLASH_SR_ECC1E_MASK) == FLASH_SR_ECC1E_MASK) /* if ERR1BIT_STS set, means read data has one bit error */
        {
            flash_ptr->SR.w = (WRITE_1_CLEAR << FLASH_SR_ECC1E_POS);
            flash_erc = ERC_FLASH_ECC_1BIT;
        }
        else if((flash_ptr->SR.w & FLASH_SR_ECC2E_MASK) == FLASH_SR_ECC2E_MASK) /* if ERR2BIT_STS set, means read data has two bit error */
        {
            flash_ptr->SR.w = (WRITE_1_CLEAR << FLASH_SR_ECC2E_POS);
            flash_erc = ERC_FLASH_ECC_2BIT;
        }
        else
        {
            flash_erc = ERC_FLASH_NO_ERROR;
        }
    }
    return flash_erc;
	
	#endif
	
	
	return (flash_erc_e)0;
}


/**
 *@brief      erase opereation controller
 *@param[in]  flash_ptr: pointer, points to the base address of flash
 *@param[in]  addr: distinguish different areas to erase
 *@param[in]  erase_type: erase command
 *@param[in]  wait_time: maximum execution time
 *@return     flash status:
 *                  ERC_FLASH_NO_ERROR
 *                  ERC_FLASH_TIME_OUT
 */
flash_erc_e flash_erase_operation(FLASH_TypeDef* flash_ptr, uint32_t addr, erase_type_e erase_type, uint32_t wait_time)
{
#if 0
    flash_erc_e flash_erc = ERC_FLASH_NO_ERROR;
    flash_ptr->ADDR = addr;
    /* clear all flags before access flash */
    flash_ptr->SR.w = ~0U;
    /* configure mode: erase */
    flash_ptr->CTRL.w = (FLASH_CTRL_KEY | COMMON_UINT8_CONVERT_UINT32(erase_type));
    __DSB();
    __ISB();
    if(false == flash_access_complete_check(flash_ptr, wait_time))
    {
        flash_erc = ERC_FLASH_TIME_OUT;
    }
    return flash_erc;
#endif
	
	return (flash_erc_e)0;
}

/**
  *@brief     flash interrupt configure
  *@param[in] flash_ptr: pointer, points to the base address of flash
  *@param[in] flash_int_type: flash interrupt type
  *@return    none
  */
void flash_drv_interrupt_cfg(FLASH_TypeDef* flash_ptr, flash_int_type_e flash_int_type)
{
#if 0
	
    DEV_ASSERT(flash_int_type <= FLASH_INT_TYPE_ALL);
    if(flash_int_type < FLASH_INT_TYPE_ALL)
    {
        flash_ptr->SR.w = 0x1UL << flash_int_type;
        flash_ptr->INTEN.w |= 0x1UL << flash_int_type;
    }
    else
    {
        flash_ptr->SR.w = (FLASH_SR_ECC1E_MASK | FLASH_SR_ECC2E_MASK | FLASH_SR_COMP_MASK);
        flash_ptr->INTEN.w = COMMON_UINT8_CONVERT_UINT32(FLASH_INT_TYPE_ALL);
    }
	
#endif
}


/**
 *@brief     change flash work mode, according to sys clock frequency
 *@param[in] freq: sys freq, unit: Hz
 *              ACCESS MODE      FLASH
 *               FAST               FALSH_FAST_FREQ
 *               MID                FALSH_MIDDLE_FREQ
 *               SLOW             FALSH_SLOW_FREQ
 *@return    none
 */
void flash_drv_mode_update(uint32_t freq)
{
#if 0
    FLASH_TypeDef* flash_ptr = CODE_FLASH_PTR;
    /* 00:fast mode, 01:slow mode, 10:mid mode */
    flash_ptr->SEL.C1US = (freq / 1000000U) - 1U;
    if(freq <= FALSH_FAST_FREQ)
    {

/* OCS_Reserved_Start */			
#if (CHIP_REVISION == REVISION_A)
/* OCS_Reserved_End */
			flash_ptr->SEL.AMODE = COMMON_UINT8_CONVERT_UINT32(LONG_MODE);
 /* OCS_Reserved_Start */
#else
			
			if(freq >= FALSH_FAST_FREQ)
			{
					flash_ptr->SEL.AMODE = COMMON_UINT8_CONVERT_UINT32(LONG_MODE);
			}
			else if(freq >= FALSH_MIDDLE_FREQ)
			{
					flash_ptr->SEL.AMODE = COMMON_UINT8_CONVERT_UINT32(MID_MODE);
			}
			else
			{
					flash_ptr->SEL.AMODE = COMMON_UINT8_CONVERT_UINT32(SHORT_MODE);
			}
			
#endif
/* OCS_Reserved_End */
    }
		
    flash_ptr = DATA_FLASH_PTR;
    /* 00/11:medium mode, 01:long mode, 10:slow mode */
    flash_ptr->SEL.C1US = (freq / 1000000U) - 1U;
    if(freq <= FALSH_FAST_FREQ)
    {
/* OCS_Reserved_Start */
#if (CHIP_REVISION == REVISION_A)
/* OCS_Reserved_End */
			flash_ptr->SEL.AMODE = COMMON_UINT8_CONVERT_UINT32(LONG_MODE);
/* OCS_Reserved_Start */
#else
			if(freq >= FALSH_FAST_FREQ)
			{
					flash_ptr->SEL.AMODE = COMMON_UINT8_CONVERT_UINT32(LONG_MODE);
			}
			else if(freq >= FALSH_MIDDLE_FREQ)
			{
					flash_ptr->SEL.AMODE = COMMON_UINT8_CONVERT_UINT32(MID_MODE);
			}
			else
			{
					flash_ptr->SEL.AMODE = COMMON_UINT8_CONVERT_UINT32(SHORT_MODE);
			}
#endif
/* OCS_Reserved_End */
    }
#endif
}


/**
 *@brief     locks the flash program erase & write controller
 *@param[in] flash_ptr: pointer, points to the base address of flash
 *@return    none
 */
void _flash_main_lock()
{
	#if 1
    //flash_ptr->PROT.w = FLASH_WR_ERASE_LOCK;
     FLASH_PTR->KEYR = 0;
	#endif
}

void _flash_info_user_lock()
{
	#if 1
    //flash_ptr->PROT.w = FLASH_WR_ERASE_LOCK;
    FLASH_PTR->OPTKEYR = 0;
	#endif
}

void _flash_info_vendor_lock()
{
	#if 1
    //flash_ptr->PROT.w = FLASH_WR_ERASE_LOCK;
    FLASH_PTR->OTPKEY = 0;
	#endif
}

void _flash_otp_lock()
{
	#if 1
    //flash_ptr->PROT.w = FLASH_WR_ERASE_LOCK;
    FLASH_PTR->OTPKEY = 0;
	#endif
    
}


/**
 *@brief     unlocks the flash program erase&write controller
 *@param[in] flash_ptr: pointer, points to the base address of flash
 *@return    none
 */
flash_erc_e _flash_main_unlock()
{
    //flash_ptr->PROT.w = FLASH_WR_ERASE_UNLOCK;
    FLASH_PTR->KEYR = FLASH_MAIN_KEY1;
    FLASH_PTR->KEYR = FLASH_MAIN_KEY2;

    /* unlock seccuss bit 16 change to 0 */
    if( FLASH_PTR->CR.LOCK == 0)
    {
        return ERC_FLASH_NO_ERROR;
    }
    else/* unlock fail bit 16 equal to 1 */
    {
        return ERC_FLASH_CONFLICT;
    }

}


flash_erc_e _flash_info_user_unlock()
{
    //flash_ptr->PROT.w = FLASH_WR_ERASE_UNLOCK;
    FLASH_PTR->OPTKEYR = FLASH_INFO_USER_KEY1;
    FLASH_PTR->OPTKEYR = FLASH_INFO_USER_KEY2;
	
    /* unlock seccuss bit 24 change to 0 */
    if( FLASH_PTR->CR.OPTLOCK == 0)
    {
        return ERC_FLASH_NO_ERROR;
    }
    else/* unlock fail bit 24 equal to 1 */
    {
        return ERC_FLASH_CONFLICT;
    }
}

flash_erc_e _flash_info_vendor_unlock()
{
    //flash_ptr->PROT.w = FLASH_WR_ERASE_UNLOCK;
    FLASH_PTR->OTPKEY = FLASH_INFO_VENDOR_KEY1;
    FLASH_PTR->OTPKEY = FLASH_INFO_VENDOR_KEY2;

#if 1  //jack 删除
    /* unlock seccuss bit 16 change to 0 */
    if(FLASH_PTR->OTPCR.OTPLOCK == 0)
    {
        return ERC_FLASH_NO_ERROR;
    }
    else/* unlock fail bit 16 equal to 1 */
    {
		INFO("_flash_info_vendor_unlock fail\n");
        return ERC_FLASH_CONFLICT;
    }
#else //调试使用

    return ERC_FLASH_NO_ERROR;

#endif
}

flash_erc_e _flash_otp_unlock()
{
    //flash_ptr->PROT.w = FLASH_WR_ERASE_UNLOCK;
    FLASH_PTR->OTPKEY = FLASH_OTP_KEY1;
    FLASH_PTR->OTPKEY = FLASH_OTP_KEY2;
	
	#if 1 //删除
    /* unlock seccuss bit 16 change to 0 */
    if(FLASH_PTR->OTPCR.OTPLOCK == 0)
    {
        //INFO("_flash_otp_unlock success\n");
        return ERC_FLASH_NO_ERROR;
    }
    else/* unlock fail bit 16 equal to 1 */
    {
        INFO("_flash_otp_unlock fail\n");
        return ERC_FLASH_CONFLICT;
    }
	#else
	return ERC_FLASH_NO_ERROR;
	#endif
}


/**
 *@brief     make info value into effect
 *@param[in] flash_ptr: pointer, points to the base address of flash
 *@return    none
 */
void _flash_info_refresh(FLASH_TypeDef* flash_ptr)
{
	#if 0
    flash_ptr->CTRL.REFRESH = COMMON_UINT8_CONVERT_UINT32(1UL);
	#endif
}


/**
 *@brief     control flash ecc function
 *@param[in] flash_ptr: pointer, points to the base address of flash
 *@param[in] ecc_en: control ecc error check
 *@return    none
 */
void flash_drv_ecc_ctl(FLASH_TypeDef* flash_ptr, bool ecc_en)
{
	#if 0
    flash_ptr->SEL.ECC = ecc_en ? 1UL : 0UL;
	#endif
}

/**
 *@brief     control flash sleep function
 *@param[in] flash_ptr: pointer, points to the base address of flash
 *@param[in] sleep_en: control flash enter low power
 *@return    none
 */
void flash_drv_sleep_ctl(FLASH_TypeDef* flash_ptr, bool sleep_en)
{
	#if 0
	
    flash_ptr->SEL.DPD = sleep_en ? 1UL : 0UL;
	
	#endif
}

/**
 *@brief     get flash interrupt status
 *@param[in] flash_type: CODE_FLASH, DATA_FALSH
 *@return    intsts:flash interrupt status
 */
uint32_t get_flash_intst(flash_type_e flash_type)
{
    uint32_t intsts;
    if(flash_type == MAIN_FLASH)
    {
        intsts = CODE_FLASH_PTR->SR.w;
    }
    else
    {
        intsts = DATA_FLASH_PTR->SR.w;
    }
    return intsts;
}


