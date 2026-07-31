/*!
 * @file console.h
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 11.5, A conversion should not be performed from pointer to void into
 * pointer to object.
 * This is done by copying the data to a temporary pointer.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 11.8, A cast shall not remove any const or volatile qualification from the type
 * pointed to by a pointer.
 * This is done by copying the data to a temporary pointer.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 13.3, A full expression containing an increment (++) or decrement (--)
 * operator should have no other potential side effects other than that
 * caused by the increment or decrement operator.
 * This way is more efficient.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 16.3, An unconditional break statement shall terminate every switch-clause.
 * This way is more efficient.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 20.7, Expressions resulting from the expansion of macro parameters shall
 * be enclosed in parentheses.
 * correct functionality is verfied by tests.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 4.9, A function should be used in preference to a function-like macro where they are interchangeable.
 * Use macro to speed up program processing
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 15.5, A function should have a single point of exit at the end.
 * This way is more efficient
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 14.2, A for loop shall be well-formed.
 * This way is more efficient
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 11.3, A cast shall not be performed between a pointer to object type and a
 * pointer to a different object type.
 * This is done by copying the data to a temporary pointer.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 12.3, The comma operator should not be used.
 * This way is more efficient
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 11.4, A conversion should not be performed between a pointer to object
 * and an integer type.
 * This is done by copying the data to a temporary pointer.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 18.3, The relational operators >, >=, < and <= shall not be applied to objects
 * of pointer type except where they point into the same object.
 * This way is more efficient
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 18.4, The +, -, += and -= operators should not be applied to an expression
 * of pointer type.
 * Easier to understant by using the +, -, += and -=.  
 *
 */


#include "utility.h"
#include "string.h"
#include "ctype.h"
#include <stddef.h>

#if   defined ( __CC_ARM )
        #include <cmsis_armcc.h>
#elif defined ( __ICCARM__ )
        #include <cmsis_iccarm.h>
#elif defined ( __GNUC__ )
        #include <cmsis_gcc.h>
#elif defined ( __ghs__ )
        #include <cmsis_ghs.h>
#else
#error "invalid compiler"
#endif

/**
 *@brief Return position of first high bit(littile-endian).
 *@param[in]	input : the check number
 *@return		position of first high bit(littile-endian)
 *					- 32 : all bits are low
 *					- 0 : means the input data's bit0 = 1
 *					- 1~31 : position of first high bit(littile-endian)
 */
uint32_t first_high_bit_get(uint32_t input)
{
    uint32_t index = input;

    index = (index - 1U) & ~index;
    index = (index & 0x55555555U) + ((index >> 1U) & 0x55555555U);
    index = (index & 0x33333333U) + ((index >> 2U) & 0x33333333U);
    index = (index & 0x0F0F0F0FU) + ((index >> 4U) & 0x0F0F0F0FU);
    index = (index & 0xFFU) + ((index & 0xFF00U) >> 8U)
    		+ ((index&0xFF0000U) >> 16U) + ((index&0xFF000000U) >> 24U) ;

    return (uint32_t)(index) ;
}

/**
 *@brief Count number of the high bit.
 *@param[in]	index : the check number
 *@return		number of the high bit
 *					- 0 : means input is 0
 *					- 1~32 : number of the high bit
 */
uint32_t high_bits_count(uint32_t index)
{
  uint32_t temp_index = index;
  temp_index = temp_index-((temp_index >> 1U) & 0x55555555U);
  temp_index = (temp_index & 0x33333333U) + ((temp_index >> 2U) & 0x33333333U);
  temp_index = (temp_index + (temp_index >> 4U)) & 0x0F0F0F0FU;
  temp_index = temp_index + (temp_index >> 8U);

  return (temp_index + (temp_index >> 16U)) & 0xFFU;
}

/**
 *@brief Clear area for block 4096 byte
 *@param[in]	addr : clear start address
 *@param[in]	size : clear area size, byte
 *@return		none
 */
void clear_area_for_block_4096(uint32_t addr, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
	{
		REG32(addr+i) = 0U;
		i += 4U;
	}
}

/**
 *@brief Clear area for block 256 byte
 *@param[in]	addr : clear start address
 *@param[in]	size : clear area size, byte
 *@return		none
 */
void clear_area_for_block_256(uint32_t addr, uint32_t size)
{
	for (uint32_t i = 0; i < size; i++)
	{
		REG32(addr+i) = 0U;
		i += 4U;
	}
}

#if defined ( USE_GCC )
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"
#endif

/**
 * @brief
 *    Sets len bytes starting at dest to the specified value
 *
 * @param[out] dest:   pointer to memory that will be set to value
 * @param[in]  len:    number of bytes to be set
 * @param[in]  value:  byte value
 *
 * @note mem_prim_set provides a memory barrier for modern out-of-order CPU's
 *       to ensure a cache flush or at least a compiler barrier fallback to
 *       ensure that is not optimized away by optimizing compilers.
 */
static void sy_mem_prim_set(void *dest, uint32_t len, uint8_t value)
{
    volatile uint8_t *dp;
    uint32_t count;
    uint32_t lcount;
    bool dp_flag = true;

    uint32_t *lp;
    uint32_t value32;

    count = len;

    dp = (volatile uint8_t *)dest;

    value32 = COMMON_UINT8_CONVERT_UINT32(value) | (COMMON_UINT8_CONVERT_UINT32(value) << 8UL) | 
              (COMMON_UINT8_CONVERT_UINT32(value) << 16UL) | (COMMON_UINT8_CONVERT_UINT32(value) << 24UL);
    
    /*
     * First, do the few bytes to get uint32_t aligned.
     */
    for (; (count > 0UL) && (dp_flag); count--) {
      dp_flag = (((uintptr_t)dp & (sizeof(uint32_t) - 1UL)) != 0UL)?true:false;
        *dp = value;
        dp++;
    }

    /*
     * Then do the uint32_t's, unrolled the loop for performance
     */
    lp = (uint32_t *)dp;

    lcount = count >> 2;

    while (lcount > 0UL) {

        switch (lcount) {
        /* Here we do blocks of 16 words. Once the remaining count
         * drops below 16, take the fast track to finish up.
         * 4 * 16, 16 words = 64 byte per loop */
        default:
            *lp++ = value32;
            *lp++ = value32;
            *lp++ = value32;
            *lp++ = value32;
            *lp++ = value32;
            *lp++ = value32;
            *lp++ = value32;
            *lp++ = value32;
            *lp++ = value32;
            *lp++ = value32;
            *lp++ = value32;
            *lp++ = value32;
            *lp++ = value32;
            *lp++ = value32;
            *lp++ = value32;
            *lp++ = value32;
            lcount -= 16UL;
            break;

        case 15:
            *lp++ = value32; /* FALLTHRU */
        case 14:
            *lp++ = value32; /* FALLTHRU */
        case 13:
            *lp++ = value32; /* FALLTHRU */
        case 12:
            *lp++ = value32; /* FALLTHRU */
        case 11:
            *lp++ = value32; /* FALLTHRU */
        case 10:
            *lp++ = value32; /* FALLTHRU */
        case 9:
            *lp++ = value32; /* FALLTHRU */
        case 8:
            *lp++ = value32; /* FALLTHRU */

        case 7:
            *lp++ = value32; /* FALLTHRU */
        case 6:
            *lp++ = value32; /* FALLTHRU */
        case 5:
            *lp++ = value32; /* FALLTHRU */
        case 4:
            *lp++ = value32; /* FALLTHRU */
        case 3:
            *lp++ = value32; /* FALLTHRU */
        case 2:
            *lp++ = value32; /* FALLTHRU */
        case 1:
            *lp++ = value32; /* FALLTHRU */
            lcount = 0;
            break;
        }
    } /* end 16*word loop */

    /* remaining bytes */
    dp = (uint8_t *)lp;
    count &= (sizeof(uint32_t) - 1UL);
    for (; (count > 0UL); dp++, count--) {
        *dp = value;
    }

    return;
}

#if defined ( USE_GCC )
#pragma GCC diagnostic pop
#endif

/**
 * @brief
 *    Sets the first n bytes starting at dest to the specified value,
 *    but maximal dmax bytes.
 *
 * @param[out]  dest:   pointer to memory that will be set to the value
 * @param[in]   dmax:   maximum number of bytes to be written
 * @param[in]   value:  byte value to be written
 * @param[in]   n:      number of bytes to be set
 *
 * @pre  dest shall not be a null pointer.
 * @pre  dmax and n shall not be greater than RSIZE_MAX_MEM or sizeof(dest).
 * @pre  value shall not be greater than 255.
 * @pre  dmax may not be smaller than n.
 *
 * @note The behavior is undefined if the size of the character
 *       array pointed to by dest < count <= dmax; in other words, an
 *       erroneous value of dmax does not expose the impending buffer
 *       overflow.
 * @note sy_memset_s provides a memory barrier for modern out-of-order CPU's
 *       to ensure a cache flush or at least a compiler barrier fallback to
 *       ensure that is not optimized away by optimizing compilers.
 *
 * @return  If there is a runtime-constraints violation, and if dest is not a
 * null pointer, and if dmax is not too large, then, before reporting the
 * runtime-constraints violation, sy_memset_s() copies dmax bytes to the
 * destination.
 *       - SY_EOK:         when operation is successful or n = 0
 *       - SY_ESNULLP:     when dest is NULL pointer
 *       - SY_ESLEMAX:     when dmax/n > RSIZE_MAX_MEM or value > 255
 *       - SY_ESNOSPC:     when dmax < n
 */
sy_errno_t sy_memset_s(void *dest, sy_rsize_t dmax, int value, sy_rsize_t n)
{
    sy_errno_t err;
    sy_rsize_t temp_n = n;

		if (dest == NULL)
		{
				return SY_ESNULLP;
		}

#if defined ( __ICCARM__ )
        #pragma diag_suppress=Pa084
#endif
		if (dmax > SY_RSIZE_MAX_MEM)
		{
				return SY_ESLEMAX;
		}
#if defined ( __ICCARM__ )
        #pragma diag_warning=Pa084
#endif

    if (temp_n == 0UL) { /* since C11 temp_n=0 is allowed */
        return SY_EOK;
    }

    if (value > 255) {
        return SY_ESLEMAX;
    }

    err = SY_EOK;
	
#if defined ( __ICCARM__ )
    #pragma diag_suppress=Pa084
#endif
    if (temp_n > dmax) {
        err = (temp_n > SY_RSIZE_MAX_MEM) ? SY_ESLEMAX : SY_ESNOSPC;
        temp_n = dmax;
    }
	
#if defined ( __ICCARM__ )
    #pragma diag_warning=Pa084
#endif

    sy_mem_prim_set(dest, temp_n, (uint8_t)value);

    return err;
}


/**
 * @brief
 *    Moves at most len of bytes from src to dest.
 *    Dest may overlap with src.
 *
 * @param[out] dest:     pointer to the memory that will be replaced by src.
 * @param[in]  src:      pointer to the memory that will be copied to dest
 * @param[in]  len:      maximum number bytes of src that can be copied
 *
 */
static void sy_mem_prim_move(void *dest, const void *src, uint32_t len) {

#define wsize sizeof(uint32_t)
#define wmask (wsize - 1UL)

    uint8_t *dp = (uint8_t *)dest;
    const uint8_t *sp = (uint8_t *)src;

    uint32_t tsp;
    uint32_t temp_len = len;
    /*
     * Determine if we need to copy forward or backward (overlap)
     */
    if ((uintptr_t)dp < (uintptr_t)sp) {
        /*
         * Copy forward.
         */

        /*
         * get a working copy of src for bit operations
         */
        tsp = (uintptr_t)sp;

        /*
         * Try to align both operands.  This cannot be done
         * unless the low bits match.
         */
        if (0UL != ((tsp | (uintptr_t)dp) & ((uint32_t)wmask) & 0xFFU)) {
            /*
             * determine how many bytes to copy to align operands
             */
            if (((tsp ^ (uintptr_t)dp) & wmask) || (temp_len < wsize)) {
                tsp = temp_len;
            } else {
                tsp = wsize - (tsp & wmask);
            }

            temp_len -= tsp;

            /*
             * make the alignment
             */
            do {
                *dp = *sp;
                dp++;
                sp++;
                --tsp;
            } while (tsp > 0UL);
        }

        /*
         * Now copy, then mop up any trailing bytes.
         */
        tsp = temp_len / wsize;

        if (tsp > 0UL) {

            do {
                *(uint32_t *)dp = *(uint32_t *)sp;

                sp += wsize;
                dp += wsize;
                --tsp;
            } while (tsp > 0UL);
        }

        /*
         * copy over the remaining bytes and we're done
         */
        tsp = temp_len & wmask;

        if (tsp > 0UL) {
            do {
                *dp = *sp;
                dp++;
                sp++;
                --tsp;
            } while (tsp > 0UL);
        }
    } else {
        /*
         * This section is used to copy backwards, to handle any
         * overlap.  The alignment requires (tps&wmask) bytes to
         * align.
         */

        /*
         * go to end of the memory to copy
         */
        sp += temp_len;
        dp += temp_len;

        /*
         * get a working copy of src for bit operations
         */
        tsp = (uintptr_t)sp;

        /*
         * Try to align both operands.
         */
        if (((tsp | (uintptr_t)dp) & wmask) != 0UL) {

            if ((((tsp ^ (uintptr_t)dp) & wmask) != 0UL) || (temp_len <= wsize)) {
                tsp = temp_len;
            } else {
                tsp &= wmask;
            }

            temp_len -= tsp;

            /*
             * make the alignment
             */
            do {
                --dp;
                --sp;
                *dp = *sp;
                --tsp;
            } while (tsp > 0UL);
        }

        /*
         * Now copy in uint32_t units, then mop up any trailing bytes.
         */
        tsp = temp_len / wsize;

        if (tsp > 0UL) {
            do {
                sp -= wsize;
                dp -= wsize;

                *(uint32_t *)dp = *(uint32_t *)sp;
                --tsp;
            } while (tsp > 0UL);
        }

        /*
         * copy over the remaining bytes and we're done
         */
        tsp = temp_len & wmask;
        if (tsp > 0UL) {
            tsp = temp_len & wmask;
            do {
              --dp;
              --sp;
                *dp = *sp;
                --tsp;
            } while (tsp > 0UL);
        }
    }

    return;
}

#define SY_CHK_OVRLP_BUTSAME(dp, dlen, sp, slen)       \
(((uintptr_t)dp > (uintptr_t)sp) &&                    \
 ((uintptr_t)dp < (uintptr_t)(sp + slen))) ||          \
	(((uintptr_t)dp < (uintptr_t)sp) &&                  \
	 ((uintptr_t)sp < (uintptr_t)(dp + dlen)))

/**
 * @brief
 *    This function copies at most slen bytes from src to dest, up to
 *    dmax resp. sizeof(dest).
 *    If slen is zero, the function does nothing.
 *
 * @param[out] dest:   pointer to the memory that will be replaced by src.
 * @param[in]  dmax:   maximum length of the resulting dest, in bytes
 * @param[in]  src:    pointer to the memory that will be copied to dest
 * @param[in]  slen:   maximum number bytes of src that can be copied
 *
 * @pre  Neither dest nor src shall be a null pointer.
 * @pre  dmax shall not be 0.
 * @pre  dmax shall not be greater than RSIZE_MAX_MEM or size of dest.
 * @pre  slen shall not be greater than RSIZE_MAX_MEM or size of src.
 * @pre  slen shall not be greater than dmax.
 * @pre  Copying shall not take place between regions that overlap.
 *
 * @return  If there is a runtime-constraint violation, the memcpy_s function
 *          stores zeros in the first dmax bytes of the region pointed to
 *          - by dest if dest is not a null pointer and slen is valid.
 *          - SY_EOK:         when operation is successful or slen = 0
 *          - SY_ESNULLP:     when dest/src is NULL POINTER
 *          - SY_ESZEROL:     when dmax = 0
 *          - SY_ESLEMAX:     when dmax/slen > RSIZE_MAX_MEM
 *          - SY_ESNOSPC:     when dmax < slen
 *          - SY_ESOVRLP:     when src memory overlaps dst
 *
 */

sy_errno_t sy_memcpy_s(void *dest, sy_rsize_t dmax, const void *src, sy_rsize_t slen)
{
    uint8_t *dp;
    const uint8_t *sp;

		sy_errno_t error;

    if (slen == 0UL) { /* Since C11 slen=0 is allowed */
        return SY_EOK;
    }

		if (dest == NULL)
		{
			return SY_ESNULLP;
		}

		if (src == NULL)
		{
			return SY_ESNULLP;
		}

		if (dmax == 0UL)
		{
			return SY_ESZEROL;
		}
#if defined ( __ICCARM__ )
        #pragma diag_suppress=Pa084
#endif		
		if (dmax > SY_RSIZE_MAX_MEM)
		{
			return SY_ESLEMAX;
		}
#if defined ( __ICCARM__ )		
        #pragma diag_warning=Pa084
#endif
#if defined ( __ICCARM__ )	
    #pragma diag_suppress=Pa084	
#endif	
    if (slen > dmax) {
        error = (slen > SY_RSIZE_MAX_MEM) ? SY_ESLEMAX : SY_ESNOSPC;
        return error;
    }
#if defined ( __ICCARM__ )	
    #pragma diag_warning=Pa084
#endif
    dp = (uint8_t *)dest;
    sp = (uint8_t *)src;

    /* overlap is disallowed, but allow dest==src */
    if (SY_CHK_OVRLP_BUTSAME(dp, dmax, sp, slen)) {
        sy_mem_prim_set(dp, dmax, 0);

        return SY_ESOVRLP;
    }

    /*
     * now perform the copy
     */
    sy_mem_prim_move(dp, sp, slen);

    return SY_EOK;
}

/**
 *@brief convert a string to a hexadecimal number
 *@param[in] *cp: point to start address of string
 *@param[in] **endp: 
 *@param[in] base: hexadecimal type
 *@return    number of type unsigned long
 */
static unsigned long cmd_data_strtoul(const char* cp, char** endp, unsigned int base)
{
	unsigned long result = 0;
	unsigned long value;

	if ( *cp == '0' )
	{
		cp++;
		if ( ( *cp == 'x' ) && isxdigit( cp[1] ) )
		{
			base = 16;
			cp++;
		}

		if ( !base )
		{
			base = 8;
		}
	}

	if ( !base )
	{
		base = 10;
	}

	while ( isxdigit( *cp ) &&
		( value = isdigit( *cp ) ? *cp - '0' :
		( islower( *cp ) ? toupper( *cp ) : *cp ) - 'A' + 10 ) < base )
	{
		result = result * base + value;
		cp++;
	}

	if ( endp )
	{
		*endp = ( char* )cp;
	}

	return result;
}

/**
 *@brief convert string to numbers
 *@param[in] *buf: the buffer of string 
 *@return    number of type unsigned long
 */
unsigned long get_arg_ulong(char* buf)
{
	unsigned long ret = 0;
	int base = 10;
	char *ptr = buf;

	if (!strncmp(buf,"0x",2) || !strncmp(buf,"0X",2)) {
		base = 16;
		ptr = buf+2;
	}
	ret = cmd_data_strtoul((const char*)ptr, 0, base);

	return ret;
}


// 支持有符号的字符串转long函数
static long cmd_data_strtol(const char* cp, char** endp, unsigned int base)
{
    long result = 0;
    int sign = 1;  // 符号位，1表示正数，-1表示负数
    int value;

    // 跳过空白字符
    while (isspace((unsigned char)*cp)) {
        cp++;
    }

    // 处理符号
    if (*cp == '-') {
        sign = -1;
        cp++;
    } else if (*cp == '+') {
        cp++;
    }

    // 处理进制前缀
    if (*cp == '0') {
        cp++;
        if ((*cp == 'x' || *cp == 'X') && isxdigit((unsigned char)cp[1])) {
            base = 16;
            cp++;
        } else if (!base) {
            base = 8;
        }
    }

    if (!base) {
        base = 10;
    }

    // 转换数字
    while (isxdigit((unsigned char)*cp)) {
        if (isdigit((unsigned char)*cp)) {
            value = *cp - '0';
        } else if (islower((unsigned char)*cp)) {
            value = toupper((unsigned char)*cp) - 'A' + 10;
        } else {
            value = *cp - 'A' + 10;
        }
        
        if (value >= (int)base) {
            break;
        }
        
        // 检查溢出
//        if (result > (LONG_MAX - value) / base) {
//            // 溢出处理
//            if (endp) {
//                *endp = (char*)cp;
//            }
//            return (sign > 0) ? LONG_MAX : LONG_MIN;
//        }
//        
        result = result * base + value;
        cp++;
    }

    if (endp) {
        *endp = (char*)cp;
    }

    return sign * result;
}

// 封装函数，支持正负号和进制前缀
signed long get_arg_long(char* buf)
{
    long ret = 0;
    int base = 10;
    char *ptr = buf;
    int sign = 1;

    // 跳过空白字符
    while (isspace((unsigned char)*ptr)) {
        ptr++;
    }

    // 处理符号
    if (*ptr == '-') {
        sign = -1;
        ptr++;
    } else if (*ptr == '+') {
        ptr++;
    }

    // 处理16进制前缀
    if (!strncasecmp(ptr, "0x", 2)) {
        base = 16;
        ptr += 2;
    }

    ret = cmd_data_strtol(ptr, 0, base);
    
    // 应用符号
    if (sign < 0 && ret > 0) {
        ret = -ret;
    }
	
	return ret;
}


