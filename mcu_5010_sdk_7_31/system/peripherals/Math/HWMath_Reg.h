#ifndef MATH_REG_H
#define MATH_REG_H

#include "stdint.h"


//用于定义HWMATH寄存器结构体
typedef volatile union
{
    volatile unsigned int word;
    struct
    {
			unsigned SQRT_RDY				:1;
			unsigned RESERVED_1_3		:3;
			unsigned DIVI_RDY				:1;
			unsigned DIVI_ERR				:1;
			unsigned RESERVED_6_7		:2;
			unsigned SINCOS_RDY			:1;
			unsigned RESERVED_9_31	:23;
    }Bits;
}MATH_VERID_STR;

#define _MATH_VERID                     *(MATH_VERID_STR *)(CMSDK_MATH_BASE + 0X00)
#define MATH_VERID                      (_MATH_VERID).Bits.MINVN
#define MATH_VERID_MINVN                (_MATH_VERID).Bits.MINVN
#define MATH_VERID_MAJVN                (_MATH_VERID).Bits.MAJVN
#define MATH_VERID_FEATURE              (_MATH_VERID).Bits.FEATURE


typedef volatile union
{
    volatile unsigned int word;
    struct
    {
        unsigned EWSERVED_0_31    :32;
    }Bits;
}MATH_IR_STR;

#define _MATH_IR                        *(MATH_IR_STR *)(CMSDK_MATH_BASE + 0X04)
#define MATH_IR                         (_MATH_IR).Word


typedef volatile union
{
    volatile unsigned int word;
    struct
    {
        unsigned EWSERVED_0_31    :32;
    }Bits;
}MATH_IE_STR;

#define _MATH_IE                        *(MATH_IE_STR *)(CMSDK_MATH_BASE + 0X08)
#define MATH_IE                         (_MATH_IE).Word


typedef volatile union
{
    volatile unsigned int word;
    struct
    {
        unsigned SQRTIN    :32;
    }Bits;
}MATH_SQRTRI_STR;

#define _MATH_SQRTRI                    *(MATH_SQRTRI_STR *)(CMSDK_MATH_BASE + 0X0C)
#define MATH_SQRTRI                     (_MATH_SQRTRI).Word
#define MATH_SQRTRI_SQRTRI              (_MATH_SQRTRI).Bits.SQRTIN


typedef volatile union
{
    volatile unsigned int word;
    struct
    {
        unsigned SQRTOUT    :16;
        unsigned RESERVED_16_31    :16;
    }Bits;
}MATH_SQRTRO_STR;

#define _MATH_SQRTRO                    *(MATH_SQRTRO_STR *)(CMSDK_MATH_BASE + 0X10)
#define MATH_SQRTRO                     (_MATH_SQRTRO).Word
#define MATH_SQRTRO_SQRTOUT             (_MATH_SQRTRO).Bits.SQRTOUT


typedef volatile union
{
    volatile unsigned int word;
    struct
    {
        unsigned DIVIDEND    :32;
    }Bits;
}MATH_DIVIDE_STR;

#define _MATH_DIVIDE                    *(MATH_DIVIDE_STR *)(CMSDK_MATH_BASE + 0X14)
#define MATH_DIVIDE                     (_MATH_DIVIDE).Word
#define MATH_DIVIDE_DIVIDEND            (_MATH_DIVIDE).Bits.DIVIDEND


typedef volatile union
{
    volatile unsigned int word;
    struct
    {
        unsigned DIVISOR    :32;
    }Bits;
}MATH_DIVIDS_STR;

#define _MATH_DIVIDS                    *(MATH_DIVIDS_STR *)(CMSDK_MATH_BASE + 0X18)
#define MATH_DIVIDS                     (_MATH_DIVIDS).Word
#define MATH_DIVIDE_DIVISOR             (_MATH_DIVIDS).Bits.DIVISOR


typedef volatile union
{
    volatile unsigned int word;
    struct
    {
        unsigned QUOTIENT    :32;
    }Bits;
}MATH_QUOTI_STR;

#define _MATH_QUOTI                     *(MATH_QUOTI_STR *)(CMSDK_MATH_BASE + 0X1C)
#define MATH_QUOTI                      (_MATH_QUOTI).Word
#define MATH_QUOTI_QUOTIENT             (_MATH_QUOTI).Bits.QUOTIENT


typedef volatile union
{
    volatile unsigned int word;
    struct
    {
        unsigned REMAINDER    :32;
    }Bits;
}MATH_REMAIN_STR;

#define _MATH_REMAIN                    *(MATH_REMAIN_STR *)(CMSDK_MATH_BASE + 0X20)
#define MATH_REMAIN                     (_MATH_REMAIN).Word
#define MATH_REMAIN_REMAINDER           (_MATH_REMAIN).Bits.REMAINDER


typedef volatile union
{
    volatile unsigned int word;
    struct
    {
        unsigned ANGLE    :16;
        unsigned RESERVED_16_31    :16;
    }Bits;
}MATH_THETA_STR;

#define _MATH_THETA                     *(MATH_THETA_STR *)(CMSDK_MATH_BASE + 0X24)
#define MATH_THETA                      (_MATH_THETA).Word
#define MATH_THETA_ANGLE                (_MATH_THETA).Bits.ANGLE


typedef volatile union
{
    volatile unsigned int word;
    struct
    {
        unsigned SIN    :16;
        unsigned COS    :16;
    }Bits;
}MATH_SINCOS_STR;

#define _MATH_SINCOS                    *(MATH_SINCOS_STR *)(CMSDK_MATH_BASE + 0X28)
#define MATH_SINCOS                     (_MATH_SINCOS).Word
#define MATH_SINCOS_SIN                 (_MATH_SINCOS).Bits.SIN
#define MATH_SINCOS_COS                 (_MATH_SINCOS).Bits.COS


typedef struct
{
    union 
    {
        struct{
            __IO uint32_t MINVN      : 8;  /* bit[7:0]   */
            __IO uint32_t MAJVN      : 8;  /* bit[15:8]  */
            __IO uint32_t FEATURE    : 16; /* bit[31:16] */
        };
        __IO uint32_t Word;
    }VERID_STR;
    
    union 
    {
        struct{
						__IO uint32_t SQRT_RDY				:1;
						__IO uint32_t RSV_1_3					:3;
						__IO uint32_t DIVI_RDY				:1;
						__IO uint32_t DIVI_ERR				:1;
						__IO uint32_t RSV_6_7					:2;
						__IO uint32_t SINCOS_RDY			:1;
						__IO uint32_t RSV_9_31				:23;
        };
        __IO uint32_t Word;
    }IR_STR;

    union 
    {
        struct{
            __IO uint32_t RSV      : 32;  /* bit[31:0]   */
        };
        __IO uint32_t Word;
    }IE_STR;

    union 
    {
        struct{
            __IO uint32_t SQRTIN      : 32;  /* bit[31:0]   */
        };
        __IO uint32_t Word;
    }SQRTRI_STR;

    union 
    {
        struct{
            __IO uint32_t SQRTOUT      : 16;  /* bit[31:0]   */
            __IO uint32_t RSV_16_31      : 16;  /* bit[31:0]   */
        };
        __IO uint32_t Word;
    }SQRTRO_STR;

    union 
    {
        struct{
            __IO uint32_t DIVIDE      : 32;  /* bit[31:0]   */
        };
        __IO uint32_t Word;
    }DIVIDE_STR;
  
    union 
    {
        struct{
            __IO uint32_t DIVIDS      : 32;  /* bit[31:0]   */
        };
        __IO uint32_t Word;
    }DIVIDS_STR;

    union 
    {
        struct{
            __IO uint32_t QUOTI      : 32;  /* bit[31:0]   */
        };
        __IO uint32_t Word;
    }QUOTI_STR;

    union 
    {
        struct{
            __IO uint32_t REMAIN      : 32;  /* bit[31:0]   */
        };
        __IO uint32_t Word;
    }REMAIN_STR;

    union 
    {
        struct{
            __IO uint32_t ANGLE      : 16;  /* bit[31:0]   */
            __IO uint32_t RSV_16_31  : 16;  /* bit[31:0]   */
        };
        __IO uint32_t Word;
    }THETA_STR;

    union 
    {
        struct{
            __IO uint32_t SIN      : 16;  /* bit[31:0]   */
            __IO uint32_t COS  : 16;  /* bit[31:0]   */
        };
        __IO uint32_t Word;
    }SINCOS_STR;

  
} HWMATH_TypeDef;







/**
 * HWMATH结构体基地址定义
 */


//#define MATH_VERID        REG32(TIMER0_BASE + 0x00)
//#define MATH_IR        REG32(TIMER0_BASE + 0x04)
//#define MATH_IE        REG32(TIMER0_BASE + 0x08)
//#define MATH_SQRTRI        REG32(TIMER0_BASE + 0x0C)
//#define MATH_SQRTRO        REG32(TIMER0_BASE + 0x10)
//#define MATH_DIVIDE        REG32(TIMER0_BASE + 0x14)
//#define MATH_DIVIDS        REG32(TIMER0_BASE + 0x14)
//#define MATH_QUOTI        REG32(TIMER0_BASE + 0x1C)
//#define MATH_REMAIN        REG32(TIMER0_BASE + 0x20)
//#define MATH_THETA        REG32(TIMER0_BASE + 0x24)
//#define MATH_SINCOS        REG32(TIMER0_BASE + 0x28)


#endif
