#ifndef DMA_TEST_H
#define DMA_TEST_H
#include "dma_type.h"
#include "mcpwm_reg.h"
#include "mcpwm_type.h"
#include "test.h"


#ifdef MCPWM_TEST
	#define MCPWM_LEN 100
	#define Duty_len 	200
	#define Deadtime_len 32
	#define freq_len 10
#else
	#define MCPWM_LEN 5
	#define Duty_len 	10
	#define Deadtime_len 10
	#define freq_len 10
#endif

#ifdef TIM_TEST
#define TIMER_LEN 100
#else
#define TIMER_LEN 10
#endif 


#ifdef SPI_TEST
#define SPI_LEN 511			//WRTRANCNT 最大511  超过Tx异常
#else
#define SPI_LEN 10
#endif

#ifdef I2C_TEST 
	#define bufsize  1024   //1024可以 读  rxfifo阈值设置为0
#else 
	#define bufsize  10   //在不使用i2c+dma的时候降低内存占用
#endif

#define UART_LEN  64

// I2C配I2C_TypeDef *I2C_1置参数
#define I2C_SLAVE_ADDR   0x4E    // 从设备7bit地址（左移1位，硬件自动处理R/W）
#define I2C_SPEED_MODE   1       // 0:标准速(100K) 1:快速速(400K) 2:高速(3.4M)
#define I2C_PCLK         32000000UL// APB时钟频率(Hz)，如32MHz
 
 
// I2C FIFO阈值（可选，默认8即可）
#define I2C_TX_TL        4
#define I2C_RX_TL        2 

typedef enum{
	TIMER13_CMP0_addrr,
	TIMER13_CMP1_addrr,
 
	TIMER14_CMP0_addrr,
	TIMER14_CMP1_addrr,
 
	TIMER15_CMP0_addrr,
	TIMER15_CMP1_addrr,
 
	TIMER16_CMP0_addrr,
	TIMER16_CMP1_addrr,
}timer_cmpx_dst_addrr;

typedef enum{
	TIMER13_TH_addrr,
	TIMER14_TH_addrr,
	TIMER15_TH_addrr,
	TIMER16_TH_addrr,
}timer_th_dst_addrr;

typedef enum{
	SPI_TX,
	SPI_RX
}spi_addr_e;

typedef enum{
	UART_TX,
	UART_RX
}uart_addr_e;


extern const uint16_t MCPWM_Duty_buff[Duty_len];
extern const uint16_t Deadtime_buff[Deadtime_len];
extern const uint16_t freq_buff[freq_len];

extern uint8_t uart_tx_buf[UART_LEN]; 
extern uint8_t uart_rx_buf[UART_LEN];

extern uint8_t SPI_DMA_Rx_buff[SPI_LEN] ;    //spi
extern uint8_t SPI_DMA_Tx_buff[SPI_LEN]; 


extern uint8_t dma_trans_complete;
//extern UART_TypeDef *uart1;
//extern UART_TypeDef *uart0;

void dma_func1_test(uint32_t test_case, uint32_t para1, uint32_t para2);
void dma_func2_test(uint32_t test_case, uint32_t para1, uint32_t para2);


void DMA_IT_M_TO_M_text(DMA_Channel_e dma_channle);
void I2C_MspInit(uint32_t slave_addr,uint8_t i2c_speed);
void I2C_DMA_SendData(DMA_Channel_e ch,uint8_t reg,uint8_t cmd, uint16_t *buf, uint16_t len);
void I2C_DMA_RecData(DMA_Channel_e ch,uint8_t reg,uint8_t cmd, uint8_t *buf, uint16_t len);

void DMA_IT_I2C_Tx_text(DMA_Channel_e dma_channle,uint32_t slave_addr,uint8_t i2c_speed,uint16_t *buf, uint16_t len);
void DMA_IT_I2C_Rx_text(DMA_Channel_e dma_channle,uint32_t slave_addr,uint8_t i2c_speed, uint8_t *buf, uint16_t len);



void DMA_IT_MCPWM_Config(DMA_Channel_e dma_channel,
													MCPWM_DMA_CH MCPWM_CH,MCPWM_RE_DMA RE_DMA,dma_mode_t Rt,MCPWM_DMA_Dst dst_addrs,
													const uint16_t *buf, uint16_t len);
void DMA_IT_MCPWM_SetFreq_Config(DMA_Channel_e dma_channel,MCPWM_DMA_CH MCPWM_CH ,const uint16_t *buf, dma_mode_t Rt);
void DMA_IT_MCPWM_MemToMem_Config(DMA_Channel_e dma_channel,	dma_mode_t Rt,const uint16_t *Txbuf,uint16_t *Rxbuf,uint16_t len);

void DMA_MCPWM_Change_duty_Init(void);
void DMA_MCPWM_Change_deadtime_Init(void);	
void DMA_MCPWM_Change_cycle_Init(void);

	
void MCPWM_Dsiable(uint8_t mcpwm);
void MCPWM_Enable(uint8_t mcpwm);


void DMA_IT_TIMER_Config(DMA_Channel_e dma_channel,dma_mode_t Rt ,const uint32_t *buf,timer_cmpx_dst_addrr dst_addrr,uint16_t len);
void DMA_IT_TIMER_MemToMem_Config(DMA_Channel_e dma_channel,
																	dma_mode_t Rt,
																	const uint32_t *Txbuf, 
																  uint32_t *Rxbuf,uint16_t len);
void DMA_IT_TIMER_PeriphToMem_Config(DMA_Channel_e dma_channel,
																	dma_mode_t Rt,
																  uint32_t *Rxbuf);



void DMA_IT_SPI_Config(DMA_Channel_e dma_channel,uint8_t *Rxbuf,uint8_t *Txbuf,uint16_t len,uint8_t spi_mode,spi_addr_e spi_addr,dma_priority_t dma_priority)		;
void HAL_DMA_SPI_Tx_Init(uint8_t *Txbuf,uint32_t spi_clk,uint16_t len);
void HAL_DMA_SPI_Rx_Init(uint8_t *Rxbuf,uint32_t spi_clk,uint16_t len);
void HAL_DMA_SPI_Tx_Rx_Init(uint8_t *Txbuf,uint8_t *Rxbuf,uint32_t spi_clk,uint16_t len);

void HAL_DMA_SPI_Slave_Tx_Init(uint8_t *Txbuf, uint16_t len);
void HAL_DMA_SPI_Slave_Rx_Init(uint8_t *Rxbuf,uint16_t len);
void HAL_DMA_SPI_Slave_Rx_Tx_Init(uint8_t *Txbuf,uint8_t *Rxbuf,uint16_t len);

void DMA_IT_SPI_SLAVE_Config(DMA_Channel_e dma_channel,uint8_t *Rxbuf,uint8_t *Txbuf,uint16_t len,uint8_t spi_mode,spi_addr_e spi_addr,dma_priority_t dma_priority);


void DMA_IT_UART_Config(DMA_Channel_e dma_channel,uint8_t *Rxbuf,uint8_t *Txbuf,uint16_t len,uart_addr_e uart_addr,dma_priority_t dma_priority,dma_mode_t dma_mode);

#endif

