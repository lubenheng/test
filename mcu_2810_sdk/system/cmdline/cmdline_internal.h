#ifndef CMDLINE_INTERNAL_H
#define CMDLINE_INTERNAL_H

/* 移植需要的文件   
cmdline_internal.c.h

utility.c.h=====非必须
test.c.h=====非必须
 
*/

#include "CMSDK_CM0.h"  
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// 日志打印开关
#define MSG_INFO

#ifdef MSG_INFO
#define INFO_e(...)    (void)printf(__VA_ARGS__)
#else
#define INFO_e(...)
#endif


// 命令行错误码定义
#define CMDLINE_BAD_CMD         (-1)  // 无效命令
#define CMDLINE_TOO_MANY_ARGS   (-2)  // 参数太多
#define CMDLINE_TOO_FEW_ARGS    (-3)  // 参数太少

#define argc_num 32

#define ERR_TEST_DONE		0		/* test done */
#define ERR_TEST_FAIL		1		/* test fail */
#define ERR_TEST_ARGC		200		/* argc error */
#define ERR_TEST_PARA		201		/* nvalid parameter */


// 命令函数指针类型
typedef int (*pfn_cmdline)(int argc, char* argv[]);

// 命令表结构体：命令名 + 处理函数 + 帮助说明
typedef struct {
    const char* cmd;
    pfn_cmdline pfn_cmd;
    const char* help;
} cmdline_entry;


// USART1 命令行初始化
void Uart_cmdline_Init(void);

// 命令行轮询处理（在while(1)调用）
void cmdline_handle_polling(void);
int cmd_help(int argc, char *argv[]);

int get_version(int argc, char *argv[]);
int cmd_mem_read(int argc, char *argv[]);
int cmd_mem_write(int argc, char *argv[]);

#ifdef __cplusplus
}
#endif
 
#endif /*CMDLINE_INTERNAL_H*/
