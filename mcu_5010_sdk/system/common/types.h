/*!
 * @file types.h
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 4.9, A function should be used in preference to a function-like macro where they are interchangeable.
 * Use macro to speed up program processing
 *
 */

#ifndef TYPES_H
#define TYPES_H



#if   defined ( __CC_ARM )
#define TCM_SRAM_DATA_ATTRIBUTES		__attribute__(( section( "tcm_sram")))
#define TCM_SRAM_FUNC_ATTRIBUTES		__attribute__(( section( "tcm_sram")))

#define AHB_SRAM_DATA_ATTRIBUTES		__attribute__(( section( "ahb_sram")))
#define AHB_SRAM_FUNC_ATTRIBUTES		__attribute__(( section( "ahb_sram")))
#elif defined ( __ICCARM__ )
#define AHB_SRAM_DATA_ATTRIBUTES		__attribute__(( section( "ahb_data_sram")))
#define AHB_SRAM_FUNC_ATTRIBUTES		__attribute__(( section( "ahb_func_sram")))
#else
#error "invalid compiler"
#endif


typedef unsigned char UINT8;
typedef unsigned short UINT16;
typedef unsigned int UINT32;
typedef unsigned long long UINT64;
typedef char INT8;
typedef short INT16;
typedef int INT32;
typedef long LONG32;
typedef float FLOAT32;
typedef double FLOAT64;

typedef signed long long s64;
typedef unsigned long long u64;
typedef int32_t  s32;
typedef int16_t s16;
typedef int8_t  s8;

typedef const int32_t sc32;  /*!< Read Only */
typedef const int16_t sc16;  /*!< Read Only */
typedef const int8_t sc8;   /*!< Read Only */

//typedef __IO int32_t  vs32;
//typedef __IO int16_t  vs16;
//typedef __IO int8_t   vs8;

//typedef __I int32_t vsc32;  /*!< Read Only */
//typedef __I int16_t vsc16;  /*!< Read Only */
//typedef __I int8_t vsc8;   /*!< Read Only */

typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef const uint32_t uc32;  /*!< Read Only */
typedef const uint16_t uc16;  /*!< Read Only */
typedef const uint8_t uc8;   /*!< Read Only */


typedef enum {DISABLE = 0, ENABLE = 1} FuncState;
typedef enum {RESET_HALL = 0, SET_HALL = !RESET_HALL} FlagStatus, ITStatus;
typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;


#define IS_FUNCTIONAL_STATE(STATE) (((STATE) == DISABLE) || ((STATE) == ENABLE))

typedef enum
{
	FUNC_DISABLE = 0,
	FUNC_ENABLE
}func_switch_e;

typedef float float32_t;

#define WRITE_1_CLEAR	1U
#define UNUSED(x)	(void)x
	

#define BIT0    (1UL << 0)   // 第0位
#define BIT1    (1UL << 1)   // 第1位
#define BIT2    (1UL << 2)   // 第2位
#define BIT3    (1UL << 3)   // 第3位
#define BIT4    (1UL << 4)   // 第4位
#define BIT5    (1UL << 5)   // 第5位
#define BIT6    (1UL << 6)   // 第6位
#define BIT7    (1UL << 7)   // 第7位
#define BIT8    (1UL << 8)   // 第8位
#define BIT9    (1UL << 9)   // 第9位
#define BIT10   (1UL << 10)  // 第10位
#define BIT11   (1UL << 11)  // 第11位
#define BIT12   (1UL << 12)  // 第12位
#define BIT13   (1UL << 13)  // 第13位
#define BIT14   (1UL << 14)  // 第14位
#define BIT15   (1UL << 15)  // 第15位
#define BIT16   (1UL << 16)  // 第16位
#define BIT17   (1UL << 17)  // 第17位
#define BIT18   (1UL << 18)  // 第18位
#define BIT19   (1UL << 19)  // 第19位
#define BIT20   (1UL << 20)  // 第20位
#define BIT21   (1UL << 21)  // 第21位
#define BIT22   (1UL << 22)  // 第22位
#define BIT23   (1UL << 23)  // 第23位
#define BIT24   (1UL << 24)  // 第24位
#define BIT25   (1UL << 25)  // 第25位
#define BIT26   (1UL << 26)  // 第26位
#define BIT27   (1UL << 27)  // 第27位
#define BIT28   (1UL << 28)  // 第28位
#define BIT29   (1UL << 29)  // 第29位
#define BIT30   (1UL << 30)  // 第30位
#define BIT31   (1UL << 31)  // 第31位
/**
 * @brief 设置指定位为1
 * @usage: REG |= BITx;
 */
#define SET_BIT(reg, bit)    ((reg) |= (bit))

/**
 * @brief 清除指定位为0
 * @usage: REG &= ~BITx;
 */
#define CLEAR_BIT(reg, bit)  ((reg) &= ~(bit))

/**
 * @brief 切换指定位状态
 * @usage: REG ^= BITx;
 */
#define TOGGLE_BIT(reg, bit) ((reg) ^= (bit))


#define 	_Q10(A) 												(s32)((A)*1024)												// Q10 format
#define 	_Q12(A) 												(s32)((A)*4096)												// Q12 format
#define 	_Q15(A) 												(s32)((A)*32768)											// Q15 format
#define 	_PI()													(3.1415926 * 1)	
#define 	_2PI()													(3.1415926 * 2)	
#define 	_SQRT_3													(1.732051)
#define 	_SQRT_2													(1.414214)
#define 	_Q16_VAL												(65536)
#define 	_Q15_VAL												(32768)	
#define 	_Q12_VAL												(4096)	



#endif /*TYPES_H*/

