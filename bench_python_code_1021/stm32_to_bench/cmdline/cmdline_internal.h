#ifndef CMDLINE_INTERNAL_H
#define CMDLINE_INTERNAL_H
#include "stm32g4xx_hal.h"
 
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// ��־��ӡ����
#define MSG_INFO

#ifdef MSG_INFO
#define INFO_e(...)    (void)printf(__VA_ARGS__)
#else
#define INFO_e(...)
#endif


// �����д����붨��
#define CMDLINE_BAD_CMD         (-1)  // ��Ч����
#define CMDLINE_TOO_MANY_ARGS   (-2)  // ����̫��
#define CMDLINE_TOO_FEW_ARGS    (-3)  // ����̫��


// �����ָ������
typedef int (*pfn_cmdline)(int argc, char* argv[]);

// ������ṹ�壺������ + �������� + ����˵��
typedef struct {
    const char* cmd;
    pfn_cmdline pfn_cmd;
    const char* help;
} cmdline_entry;


// USART1 �����г�ʼ��
void Uart_cmdline_Init(void);

// ��������ѯ��������while(1)���ã�
void cmdline_handle_polling(void);
int cmd_help(int argc, char *argv[]);
#ifdef __cplusplus
}
#endif
 
#endif /*CMDLINE_INTERNAL_H*/
