#ifndef _SPI_HAL_H
#define _SPI_HAL_H
#include "stdint.h"
#include "SPI_reg.h"
extern spi_reg_t *const volatile SPI;
//P05 NSS  P06 SCLK  P07 MISO  P10 MOSI ====¹¦ÄÜ3
void SPI_GPIO_Init(void);
void SPI_Master_Init(uint32_t baudrate,uint32_t cpol,uint32_t cpha);
void SPI_Master_interrupt_Init(uint32_t baudrate,uint32_t cpol,uint32_t cpha);
void SPI_SendByte(uint8_t data);

void SPI_Send(uint8_t *buf, uint16_t len);

void SPI_Uart_modified(void);
void SPI_SendData(uint8_t *buf, uint16_t len);

void SPI_Master_ReadData(uint8_t *buf, uint16_t len);


void SPI_Slave_Init(uint32_t cpol,uint32_t cpha);
void SPI_Slave_SendData(uint8_t *buf, uint16_t len);
uint8_t SPI_Slave_ReadData(void);

#endif

