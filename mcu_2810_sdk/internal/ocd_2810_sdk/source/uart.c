#include <stdio.h>
#include <string.h>
#include "CMSDK_CM0.h"
#include "system_CMSDK_CM0.h"
#include "core_cm0.h"
#include "Common.h"
#include "uart.h"
#include "gpio.h"

#include <stdarg.h>


char        uart0TX_str[100]  = {0} ;
uint8_t     uart0TX_strLen          ;
uint8_t     uart0TX_strCnt          ;
uint8_t     uart0TX_almost_full     ;
uint8_t     uart0TX_full            ;
uint8_t     uart0TX_keep_fill       ;


char        uart0RX_str[100]     = {0};
uint32_t    uart0RX_strCnt       = 0;   
uint32_t    uart0RX_strLen       = 0;
uint8_t     uart0RX_almost_full  = 0;
uint8_t     uart0RX_full         = 0;
uint8_t     uart0RX_almost_empty = 0;
uint8_t     uart0RX_empty        = 0;
uint8_t     uart0RX_stop         = 0;

//48 000 000/3 000 000 = 16.0
//24 000 000/1 500 000 = 16.0
//24 000 000/460 800 = 52.08
#define UART0_BAUD_RATE     (float)(52.08) 
#define UART1_BAUD_RATE     (float)(16.0)


//--------------------------------------------------------------------/
//--------------------------UART0-------------------------------------/
//--------------------------------------------------------------------/
void uart0_Handler(void)
{
    uint32_t     tmp;

    
    tmp = UART0->int_clr;
    
    
    // rx
    if ((tmp >> 1) & 1)
    {
        tmp = (UART0->int_clr >> 6);
        uart0RX_almost_full  = tmp        & 1;
        uart0RX_full         = (tmp >> 1) & 1;
        uart0RX_almost_empty = (tmp >> 2) & 1; 
        uart0RX_empty        = (tmp >> 3) & 1;
        uart0RX_stop         = (tmp >> 4) & 1;
        
        UART0->int_clr = 1<<1;
        
        if (uart0RX_full || uart0RX_almost_full || uart0RX_stop)
        {
            while (!uart0RX_empty)
            {
                uart0RX_str[uart0RX_strCnt++] = UART0->dat;
                
                tmp = (UART0->int_clr >> 6);
                uart0RX_empty = (tmp >> 3) & 1;  
            }
        }
    }
}



//------------------------------------------------
void uart0Tx_init (void)
{
    uint32_t    tmp;
    
    //开启时钟门控
    DIGCTRL->clk_en0 |= BIT18;

    //配置uart设备
    tmp  = UART0->thr;
    tmp &= ~(0xff<<0);
    tmp |= 11 | 1<<4;
    UART0->thr  = tmp;

    //波特率460800
//    UART0->baud  = (uint32_t)((float)CPU_CLK/460800 + 0.5);
	UART0->baud  = (uint32_t)((float)UART0_BAUD_RATE + 0.5);
    tmp = UART0->en;
    tmp |= 1;
    UART0->en |= tmp;
    
    //配置uart输出端口
    IO->en |= BIT17;
}



//------------------------------------------------
void uart0Rx_init(void)
{
    uint32_t      tmp;  

    //开启时钟门控
    DIGCTRL->clk_en0 |= BIT18;
    
    //配置uart设备
    tmp = UART0->thr;
    tmp &= ~(0xff<<8);
    tmp |= (11 | 1<<4)<<8;
    UART0->thr   = tmp;

    //波特率460800
    UART0->baud          = (uint32_t)((float)UART0_BAUD_RATE + 0.5);
    UART0->rx_stop_char  = 13;// "\n"
    
    tmp = UART0->en;
    tmp &= ~(0x3f<<8);
    tmp |= 0x2c<<8 | 3<<6;
    UART0->en = tmp;
    
    //全局变量初始化
    uart0RX_strCnt       = 0;
    uart0RX_stop         = 0;
    
    //配置uart输出端口
    IO->en |= BIT19;
    NVIC_EnableIRQ(uart0_irq);
}



//------------------------------------------------
void printk(char *fmt, ...)
{
    uint32_t    tmp;
    va_list     args;

    va_start (args, fmt);
    uart0TX_strLen = vsprintf(uart0TX_str, fmt, args);
    va_end(args);

    uart0TX_strCnt = 0;
    
    tmp = (UART0->int_clr >> 2);
    uart0TX_almost_full  = tmp        & 1;
    uart0TX_full         = (tmp >> 1) & 1;
    
    if ((!uart0TX_almost_full) && (!uart0TX_full) && (uart0TX_strCnt < uart0TX_strLen)) 
        uart0TX_keep_fill = 1;
    else
        uart0TX_keep_fill = 0;

    while (uart0TX_strCnt < uart0TX_strLen)
    {
        if (uart0TX_keep_fill)
            UART0->dat = uart0TX_str[uart0TX_strCnt ++]; 
        else
            __ISB();

        tmp = (UART0->int_clr >> 2);
        uart0TX_almost_full  = tmp        & 1;
        uart0TX_full         = (tmp >> 1) & 1;

        if ((!uart0TX_almost_full) && (!uart0TX_full) && (uart0TX_strCnt < uart0TX_strLen)) 
            uart0TX_keep_fill = 1;
        else
            uart0TX_keep_fill = 0;
    }
}




//--------------------------------------------------------------------/
//--------------------------UART1-------------------------------------/
//--------------------------------------------------------------------/
void uart1_Handler(void)
{
    //uint32_t     tmp;

    
    //tmp = UART1->int_clr;
    
    
    // rx
    // if ((tmp >> 1) & 1)
    // {
    //     tmp = (UART0->int_clr >> 6);
    //     uart0RX_almost_full  = tmp        & 1;
    //     uart0RX_full         = (tmp >> 1) & 1;
    //     uart0RX_almost_empty = (tmp >> 2) & 1; 
    //     uart0RX_empty        = (tmp >> 3) & 1;
    //     uart0RX_stop         = (tmp >> 4) & 1;
        
    //     UART0->int_clr = 1<<1;
        
    //     if (uart0RX_full || uart0RX_almost_full || uart0RX_stop)
    //     {
    //         while (!uart0RX_empty)
    //         {
    //             uart0RX_str[uart0RX_strCnt++] = UART0->dat;
                
    //             tmp = (UART0->int_clr >> 6);
    //             uart0RX_empty = (tmp >> 3) & 1;  
    //         }
    //     }
    // }
}



//------------------------------------------------
void uart1Tx_init (void)
{
    uint32_t    tmp;
    
    //开启时钟门控
    DIGCTRL->clk_en0 |= BIT19;

    //配置uart设备
    tmp  = UART1->thr;
    tmp &= ~(0xff<<0);
    tmp |= 11 | 1<<4;
    UART1->thr  = tmp;

    //波特率460800
//    UART1->baud  = (uint32_t)((float)CPU_CLK/460800 + 0.5);
	UART1->baud  = (uint32_t)((float)UART1_BAUD_RATE + 0.5);
    tmp = UART1->en;
    tmp |= 1;
    UART1->en |= tmp;
    
    //配置uart输出端口 ???? 需要更改  //暂未定义
    IO->en |= BIT21;
}



//------------------------------------------------
void uart1Rx_init(void)
{
    uint32_t      tmp;  

    //开启时钟门控
    DIGCTRL->clk_en0 |= BIT19;
    
    //配置uart设备
    tmp = UART1->thr;
    tmp &= ~(0xff<<8);
    tmp |= (11 | 1<<4)<<8;
    UART1->thr   = tmp;

    //波特率460800
    UART1->baud          = (uint32_t)((float)UART1_BAUD_RATE + 0.5);
    UART1->rx_stop_char  = 13;// "\n"
    
    tmp = UART1->en;
    tmp &= ~(0x3f<<8);
    tmp |= 0x2c<<8 | 3<<6;
    UART1->en = tmp;
    
    //全局变量初始化
    // uart1RX_strCnt       = 0;   //暂未定义
    // uart1RX_stop         = 0;   //暂未定义
     
    //配置uart输出端口 ???? 需要更改  //暂未定义
    //IO->en |= BIT19;
    NVIC_EnableIRQ(uart1_irq);
}



//------------------------------------------------
void printk_uart1(char *fmt, ...)
{
    // uint32_t    tmp;
    // va_list     args;

    // va_start (args, fmt);
    // uart0TX_strLen = vsprintf(uart0TX_str, fmt, args);
    // va_end(args);

    // uart0TX_strCnt = 0;
    
    // tmp = (UART0->int_clr >> 2);
    // uart0TX_almost_full  = tmp        & 1;
    // uart0TX_full         = (tmp >> 1) & 1;
    
    // if ((!uart0TX_almost_full) && (!uart0TX_full) && (uart0TX_strCnt < uart0TX_strLen)) 
    //     uart0TX_keep_fill = 1;
    // else
    //     uart0TX_keep_fill = 0;

    // while (uart0TX_strCnt < uart0TX_strLen)
    // {
    //     if (uart0TX_keep_fill)
    //         UART0->dat = uart0TX_str[uart0TX_strCnt ++]; 
    //     else
    //         __ISB();

    //     tmp = (UART0->int_clr >> 2);
    //     uart0TX_almost_full  = tmp        & 1;
    //     uart0TX_full         = (tmp >> 1) & 1;

    //     if ((!uart0TX_almost_full) && (!uart0TX_full) && (uart0TX_strCnt < uart0TX_strLen)) 
    //         uart0TX_keep_fill = 1;
    //     else
    //         uart0TX_keep_fill = 0;
    // }
}
