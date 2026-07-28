#ifndef QSPI_H
#define QSPI_H


void wait4CtrlerReady (void);
void wait4FlashReady (void);
void qspi_func1_init (void);
void xip_4x_init(uint8_t flash_rd_cmd, uint8_t flash_div, uint8_t capture, uint32_t image1_addr);


#endif 
