#ifndef _SPI_REG_H
#define _SPI_REG_H
#include "stdint.h"
 

#define SPI_BASE         (0x40001000UL)
 
// 解决匿名结构体/联合体报错（关键！）
//#pragma anon_unions
/* 寄存器读写权限定义（标准CMSIS） */
//#define __I  const volatile    /* 只读  */
//#define __O  volatile          /* 只写  */
//#define __IO volatile          /* 读写  */

typedef struct {
    /* 0x00: CR0 控制寄存器0 (32位) */
    union {
        volatile uint32_t dword;
        struct {
            uint32_t rx_empty : 1;    /* bit0  接收缓冲器空 RO */
            uint32_t sr_empty : 1;    /* bit1  移位寄存器空 RO */
            uint32_t nssin    : 1;    /* bit2  NSS实时信号 RO */
            uint32_t slvsel   : 1;    /* bit3  NSS片选标志 RO */
            uint32_t cpol     : 1;    /* bit4  时钟空闲电平 WR */
            uint32_t cpha     : 1;    /* bit5  时钟相位 WR */
            uint32_t spimst   : 1;    /* bit6  主/从模式 WR */
            uint32_t spibsy   : 1;    /* bit7  SPI忙标志 RO */
            uint32_t resv     : 24;   /* bit8~31 保留位 */
        } bits;
    } cr0;

    /* 0x04: CR1 控制寄存器1 (32位) */
    union {
        volatile uint32_t dword;
        struct {
            uint32_t spien        : 1;  /* bit0  SPI使能 WR */
            uint32_t tx_empty     : 1;  /* bit1  发送缓冲器空 RO */
            uint32_t nssmod       : 2;  /* bit2~3 NSS模式配置 WR */
            uint32_t unread       : 1;  /* bit4  未读中断标志 RW/清0 */
            uint32_t mst_off      : 1;  /* bit5  主机冲突标志 RW/清0 */
            uint32_t wr_conflict  : 1;  /* bit6  写冲突标志 RW/清0 */
            uint32_t spiif        : 1;  /* bit7  传输完成中断标志 RW/清0 */
            uint32_t int_detail_en: 4;  /* bit8~11 中断源使能 WR */
            uint32_t int_en       : 4;  /* bit12~15 中断线使能 WR */
            uint32_t resv         : 16; /* bit16~31 保留位 */
        } bits;
    } cr1;

    /* 0x08: spi_clk 波特率寄存器 (32位) */
    union {
        volatile uint32_t dword;
        struct {
            uint32_t clk_div : 8;      /* bit0~7 波特率配置 */
            uint32_t resv    : 24;     /* bit8~31 保留位 */
        } bits;
    } spi_clk;

    /* 0x0C: spi_dr 数据寄存器 (32位) */
    union {
        volatile uint8_t dword;
        struct {
            uint8_t data : 8;         /* bit0~7 数据 */
          
        } bits;
    } spi_dr;

} spi_reg_t;

/* 定义SPI操作指针 */
//#define SPI ((spi_reg_t *) SPI_BASE)
//spi_reg_t *const SPI = (spi_reg_t *)SPI_BASE; 
 




#endif  // _I2C_REG_H
