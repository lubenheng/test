#ifndef DMA_CMDLINE_H
#define DMA_CMDLINE_H
#include "dma_type.h"
#include "test.h"



int cmd_dma_modeInit_test(int argc, char *argv[]);
int cmd_dma_sendI2c_test(int argc, char *argv[]);
int cmd_dma_recI2c_test(int argc, char *argv[]);
int cmd_dma_enable_test(int argc, char *argv[]);
int cmd_dma_setfifo_test(int argc, char *argv[]); 


int cmd_dma_mcpwm0_test(int argc, char *argv[]);
int cmd_dma_mcpwm0_set_freqtest(int argc, char *argv[]);
int cmd_dma_mcpwm0_MemToMem_test(int argc, char *argv[]);


int cmd_dma_timer_cmpx_test(int argc, char *argv[]);
int cmd_dma_timer_MemToMem_test(int argc, char *argv[]);
 
int cmd_dma_spi_rx_test(int argc, char *argv[]);
int cmd_dma_spi_tx_test(int argc, char *argv[]);
int cmd_dma_spi_rx_tx_test(int argc, char *argv[]);

int cmd_dma_spi_slave_rx_test(int argc, char *argv[]);
int cmd_dma_spi_slave_tx_test(int argc, char *argv[]);
int cmd_dma_spi_slave_tx_rx_test(int argc, char *argv[]);
int cmd_dma_spi_slave_output_info_rxbuff(int argc, char *argv[]);


int cmd_usedma_Reminder_Attention(int argc, char *argv[]);

int cmd_dma_uart_tx_test(int argc, char *argv[]);
int cmd_dma_uart_rx_test(int argc, char *argv[]);

#define DMA_CMDLINE_MENU  \
		{"cmd_dma_modeInit_test",cmd_dma_modeInit_test,  " cmd_dma_modeInit_test"}, \
		{"cmd_dma_recI2c_test",  cmd_dma_recI2c_test,  "  cmd_dma_recI2c_test"},\
    {"cmd_dma_sendI2c_test", cmd_dma_sendI2c_test, "  cmd_dma_sendI2c_test"}, \
		\
    {"cmd_dma_enable_test",  cmd_dma_enable_test,  "  cmd_dma_enable_test"}, \
    {"cmd_dma_setfifo_test",  cmd_dma_setfifo_test,  "  cmd_dma_setfifo_test"}, \
		\
		\
		{"cmd_dma_mcpwm0_test",  cmd_dma_mcpwm0_test,  "  cmd_dma_mcpwm0_test"}, \
		{"cmd_dma_mcpwm0_set_freqtest",  cmd_dma_mcpwm0_set_freqtest,  "  cmd_dma_mcpwm0_set_freqtest"}, \
		{"cmd_dma_mcpwm0_MemToMem_test",  cmd_dma_mcpwm0_MemToMem_test,  "  cmd_dma_mcpwm0_MemToMem_test"}, \
		\
		{"cmd_dma_timer_cmpx_test",  cmd_dma_timer_cmpx_test,  "  cmd_dma_timer_cmpx_test"},	\
		{"cmd_dma_timer_MemToMem_test",  cmd_dma_timer_MemToMem_test,  "  cmd_dma_timer_MemToMem_test"},\
		\
		{"cmd_dma_spi_rx_test",  cmd_dma_spi_rx_test,  "  cmd_dma_spi_rx_test"},\
		{"cmd_dma_spi_tx_test",  cmd_dma_spi_tx_test,  "  cmd_dma_spi_tx_test"},\
		{"cmd_dma_spi_rx_tx_test",  cmd_dma_spi_rx_tx_test,  "  cmd_dma_spi_rx_tx_test"},\
		{"cmd_dma_spi_slave_rx_test",  cmd_dma_spi_slave_rx_test,  "  cmd_dma_spi_slave_rx_test"},\
		{"cmd_dma_spi_slave_tx_test",  cmd_dma_spi_slave_tx_test,  "  cmd_dma_spi_slave_tx_test"},\
		{"cmd_dma_spi_slave_tx_rx_test",  cmd_dma_spi_slave_tx_rx_test,  "  cmd_dma_spi_slave_tx_rx_test"},\
		{"cmd_dma_spi_slave_output_info_rxbuff",  cmd_dma_spi_slave_output_info_rxbuff,  "  cmd_dma_spi_slave_output_info_rxbuff"},\
		{"cmd_usedma_Reminder_Attention",  cmd_usedma_Reminder_Attention,  "  ¡¾  cmd_usedma_Reminder_Attention ¡¿"},\
		\
		{"cmd_dma_uart_tx_test",  cmd_dma_uart_tx_test,  "  cmd_dma_uart_tx_test"},\
		{"cmd_dma_uart_rx_test",  cmd_dma_uart_rx_test,  "  cmd_dma_uart_rx_test"},
		
		
		
		
#endif /* DMA_CMDLINE_H */

