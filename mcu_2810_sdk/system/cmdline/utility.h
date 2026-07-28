#ifndef __UTILITY_H
#define __UTILITY_H

#include <stdint.h>
#include "cmdline_internal.h"
#ifdef __cplusplus
extern "C" {
#endif
#define REG32(a)			(*((volatile uint32_t *)(a)))
	#define WRITE_REG_32(a,v)	((*((volatile uint32_t *)(a))) = v)
// 字符串转无符号长整型
unsigned long get_arg_ulong(char* buf);

// 字符串转有符号长整型
signed long get_arg_long(char* buf);

#ifdef __cplusplus
}
#endif

#endif



