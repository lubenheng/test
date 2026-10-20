#ifndef SPI_CMDLINE_H
#define SPI_CMDLINE_H

#include <stdint.h>

void DAC8551_Init(void);
void DAC8551_Write(uint16_t value);
void DAC8551_SetVoltage_mV(uint16_t mV);
void hc595d_reset(void);
int spi_cmd_set_val(int argc, char *argv[]);
int cmd_gpio_set_595d_test(int argc, char *argv[]);

#define SPI_CMDLINE_MENU \
		{"spi_cmd_set_val",spi_cmd_set_val, " spi_cmd_set_val"}, \
		{"cmd_gpio_set_595d_test",cmd_gpio_set_595d_test, " cmd_gpio_set_595d_test"}, \

#endif
