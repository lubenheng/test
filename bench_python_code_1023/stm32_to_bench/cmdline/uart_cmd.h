#ifndef UART_CMD_H
#define UART_CMD_H

int cmd_uart_set_paramet_test(int argc, char *argv[]);

#define DMA_CMDLINE_MENU \
		{"cmd_uart_set_paramet_test",cmd_uart_set_paramet_test,  " cmd_uart_set_paramet_test"}, \
 



#endif
