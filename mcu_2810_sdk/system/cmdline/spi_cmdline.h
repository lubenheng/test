#ifndef SPI_CMDLINE_H
#define SPI_CMDLINE_H
 


int cmd_spi_test(int argc, char *argv[]);
int cmd_spi_read(int argc, char *argv[]);
int printf_rxbuff(int argc, char *argv[]);
int spi_slave_test(int argc, char *argv[]);

#define SPI_CMDLINE_MENU \
		{"cmd_spi_test",cmd_spi_test,  " cmd_spi_test"}, \
		{"cmd_spi_read",cmd_spi_read,  " cmd_spi_read"}, \
		{"printf_rxbuff",printf_rxbuff,  " printf_rxbuff"}, \
		{"spi_slave_test",spi_slave_test,  " spi_slave_test"}, \

#endif
