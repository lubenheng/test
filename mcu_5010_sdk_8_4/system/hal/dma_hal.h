#ifndef DMA_HAL_H
#define DMA_HAL_H

#include "I2c_reg.h"
#include "dma_reg.h"
#include "dma_type.h"

//集成函数库
/* DMA 基本控制函数 */
 
 
void HAL_DMA_chx_Enable(DMA_Channel_e dma_channle);
void HAL_DMA_chx_Disable(DMA_Channel_e dma_channle);

/* DMA 初始化函数 */
status_t HAL_DMA_Init(dma_ch_config_t *hdma);
 
void HAL_DMA_ITchx_Init(DMA_Channel_e dma_channle);

/* I2C DMA 专用初始化函数 */
void HAL_DMA_I2CTx_chx_Init(DMA_Channel_e dma_channle, I2C_TypeDef *i2c_ptr, uint32_t tx_buffer, uint32_t buffer_size);
void HAL_DMA_I2CRx_chx_Init(DMA_Channel_e dma_channle, I2C_TypeDef *i2c_ptr, const uint32_t *rx_buffer, uint32_t buffer_size);

/* DMA 状态和控制函数 */
uint32_t HAL_DMA_GetRemainingCnt(DMA_Channel_e dma_channle);
uint8_t HAL_DMA_ISBusy(DMA_Channel_e dma_channle);
 

/* DMA 中断控制函数 */
void HAL_DMA_IT_Transfer_Set(DMA_Channel_e dma_channle, dma_it_enable_t enable);
void HAL_DMA_IT_TransError_Set(DMA_Channel_e dma_channle, uint32_t enable);
void HAL_DMA_IT_Configerror_Set( uint32_t enable);
void HAL_DMA_IT_Transfer_Clean(DMA_Channel_e dma_channle);
void HAL_DMA_IT_Error_Clean(DMA_Channel_e dma_channle);
void HAL_DMA_IT_Configerror_Clean(DMA_Channel_e dma_channle);

uint32_t HAL_DMA_Get_status(DMA_Channel_e dma_channle, DMA_Status_Type_e status_type);

/* DMA NVIC 中断控制 */
void HAL_DMA_NVIC_Enable(DMA_Channel_e dma_channle, dma_priority_t Priority);
void HAL_DMA_NVIC_Disable(DMA_Channel_e dma_channle, dma_priority_t Priority);

/* I2C DMA 使能控制函数 */
void I2C_DMA_Tx_Enable(I2C_TypeDef *i2c_ptr, uint32_t enable);
void I2C_DMA_Rx_Enable(I2C_TypeDef *i2c_ptr, uint32_t enable);
void I2C_DMA_Config(I2C_TypeDef *i2c_ptr, uint32_t tx_en, uint32_t rx_en);
uint32_t I2C_DMA_GetStatus(I2C_TypeDef *i2c_ptr);

/* I2C FIFO 阈值配置函数 */
void I2C_DMA_SetTxLevel(I2C_TypeDef *i2c_ptr, uint8_t level);
void I2C_DMA_SetRxLevel(I2C_TypeDef *i2c_ptr, uint8_t level);
uint8_t I2C_DMA_GetTxLevel(I2C_TypeDef *i2c_ptr);
uint8_t I2C_DMA_GetRxLevel(I2C_TypeDef *i2c_ptr);
uint8_t I2C_DMA_GetTxLevelDepth(I2C_TypeDef *i2c_ptr);
uint8_t I2C_DMA_GetRxLevelDepth(I2C_TypeDef *i2c_ptr);

/* I2C DMA 完整初始化函数 */
void HAL_I2C_DMA_Tx_FullInit(I2C_TypeDef *i2c_ptr, DMA_Channel_e dma_channle, uint32_t txbuffer, uint16_t buffsize);
void HAL_DMA_StructerInit(dma_ch_config_t *dma_ch_config_set);
#endif
