/*!
 * @file utility.h
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 4.9, A function should be used in preference to a function-like macro where they are interchangeable.
 * Use macro to speed up program processing
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.11, When an array with external linkage is declared, its size should be
 * explicitly specified
 * The number of configurations/callback can be zero on the other side C language forbids declaring array of size zero.
 *
 */
#ifndef UTILITY_H
#define UTILITY_H

#include <stdint.h>
#include <stdbool.h>

#define LONG2CHAR(num,i)                (*( (uint8_t *)&(num) + (i) ))
#define INT2CHAR(num,i)                 (*( (uint8_t *)&(num) + (i) ))
#define BOOL2UINT32(x)                 ((x)? 1UL : 0UL)
#define ASSIGN_INT(num,hbyte,lbyte)     {*( (uint8_t *)&num ) = lbyte; \
											*( (uint8_t *)&num+1 ) = hbyte;}
#define ASSIGN_LONG(num,hhbyte,hbyte,lbyte,llbyte)	{*( (uint8_t *)&num  )  = llbyte; \
													*( (uint8_t *)&num+1 ) = lbyte; \
													*( (uint8_t *)&num+2 ) = hbyte; \
 											    	*( (uint8_t *)&num+3 ) = hhbyte;  }
#define ASSIGN_U32BYTE(num, i,byte)     {*( (uint8_t *)&num+i) = byte;}
 											    	
#define ROUND_UP_DIV(x, y)			((((x) + (y)) - 1UL)/(y))
#define ALIGN_MEM(x, y)				(ROUND_UP_DIV(x, y)*(y))			

#define COMMON_UINT8_CONVERT_UINT32(x)   (((uint32_t)x) & 0xFFU)
#define COMMON_UINT16_CONVERT_UINT32(x)   (((uint32_t)x) & 0xFFFFU)

/* config multi fields at one time efficiently */
#define set_multi_bits(reg, mask, dat) do{ \
									uint32_t a = *((__IO uint32_t *)(reg)); \
									a &= ~(mask); \
									a |= ((dat) & (mask)); \
									*((__IO uint32_t *)(reg)) = a; \
									}while(0)

#define REG8(p,i) (*((volatile uint8_t *)((uint32_t)((p)+(i)))))
#define WRITE_REG_32(a,v)	((*((volatile uint32_t *)(a))) = v)
#define REG32(a)			(*((volatile uint32_t *)(a)))
#define BIT_BAND(addr, bit_num) ((addr & 0xF0000000U) + 0x02000000U +((addr&0xFFFFFU)<<5U)|(bit_num<<2U))

/*REV_BYTES_32(): reverses the byte order in unsigned integer value. 
 For example, 0x12345678 becomes 0x78563412*/
#define REV_BYTES_32(a, b)  (b=__REV(a)) 

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

static inline void wordcpy(uint32_t *dest, const uint32_t *src, uint32_t word_cnt)
{
	for (uint32_t i=0; i < word_cnt; i++)
	{
		dest[i] = src[i];
	}
}

/*return majority bit of 3bit data*/
static inline bool majority_3(uint32_t data)
{
  uint32_t temp_data = data;
	temp_data &= 0x7U; /*get lowest 3 bit*/

	return (bool) ((temp_data == 0x3U)||(temp_data > 0x4U)) ? true : false;
}

uint32_t first_high_bit_get(uint32_t index);

uint32_t high_bits_count(uint32_t index);

void clear_area_for_block_4096(uint32_t addr, uint32_t size);
void clear_area_for_block_256(uint32_t addr, uint32_t size);

/* FOR sy_memset_s and sy_memcpy */
typedef int sy_errno_t;
typedef uint32_t  sy_rsize_t;
#define SY_RSIZE_MAX_MEM UINT32_MAX

#define SY_EOK             ( 0 )
#define SY_ESNULLP         ( 400 )       /* null ptr                    */
#define SY_ESZEROL         ( 401 )       /* length is zero              */
#define SY_ESLEMAX         ( 403 )       /* length exceeds RSIZE_MAX    */
#define SY_ESOVRLP         ( 404 )       /* overlap undefined           */
#define SY_ESNOSPC         ( 406 )       /* not enough space for s2     */

#define isdigit(c)      ('0' <= (c) && (c) <= '9')

sy_errno_t sy_memset_s(void *dest, sy_rsize_t dmax, int value, sy_rsize_t n);
sy_errno_t sy_memcpy_s(void *dest, sy_rsize_t dmax, const void *src, sy_rsize_t slen);
unsigned long get_arg_ulong(char* buf);
signed long get_arg_long(char* buf);

#endif /*UTILITY_H*/

