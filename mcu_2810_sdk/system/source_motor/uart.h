#ifndef UART_H
#define UART_H


extern char        uart0TX_str[100]     ;
extern uint8_t     uart0TX_strLen       ;
extern uint8_t     uart0TX_strCnt       ;
extern uint8_t     uart0TX_almost_full  ;
extern uint8_t     uart0TX_full         ;
extern uint8_t     uart0TX_keep_fill    ;

extern char        uart0RX_str[100]     ;
extern uint32_t    uart0RX_strCnt       ;   
extern uint32_t    uart0RX_strLen       ;
extern uint8_t     uart0RX_almost_full  ;
extern uint8_t     uart0RX_full         ;
extern uint8_t     uart0RX_almost_empty ;
extern uint8_t     uart0RX_empty        ;
extern uint8_t     uart0RX_stop         ;


void printk(char *fmt, ...);
void uart0Tx_init(void);
void uart0Rx_init(void);

void uart1Tx_init(void);
void uart1Rx_init(void);
void printk_uart1(char *fmt, ...);
void uart_process_command(void);
#endif  
