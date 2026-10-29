#ifndef __UTILITY_H
#define __UTILITY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// 字符串转无符号长整型
unsigned long get_arg_ulong(char* buf);

// 字符串转有符号长整型
signed long get_arg_long(char* buf);

#ifdef __cplusplus
}
#endif

#endif



