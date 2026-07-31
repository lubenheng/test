#ifndef __DMA0_H__
#define __DMA0_H__
 
#ifdef __cplusplus
extern "C" {
#endif
#include "dma_test.h"
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "dma_type.h"
/* Exported types ------------------------------------------------------------*/


#define I2C_DMA_CR_RX_EN    (1 << 0)  /* RDMAE¦Ë */
#define I2C_DMA_CR_TX_EN    (1 << 1)  /* TDMAE¦Ë */
 

void DMA_div_clk_Enable(void);
void DMA_div_clk_Disable(void);

 

void DMA_div_DIR_Init(DMA_Channel_e DMA_chx,dma_direction_t direction);

void DMA_div_srcAddr_Init(DMA_Channel_e DMA_chx, uint32_t srcAddr);
void DMA_div_dstAddr_Init(DMA_Channel_e DMA_chx,uint32_t dstAddr);



//void DMA_div_TransCnt_Init(DMA_Channel_e DMA_chx, uint32_t transferCnt);

void DMA_div_DMAToDst_size_Init(DMA_Channel_e DMA_chx, dma_data_width_t Bitsizeout);
void DMA_div_SrcToDma_size_Init(DMA_Channel_e DMA_chx, dma_data_width_t Bitsizein);

void DMA_div_SrcIncMode_Init(DMA_Channel_e DMA_chx, dma_addr_mode_t periphInc);
void DMA_div_DstIncMode_Init(DMA_Channel_e DMA_chx, dma_addr_mode_t memInc);

void DMA_div_mode_Init(DMA_Channel_e DMA_chx, dma_mode_t mode);
void DMA_div_TriggerSource_Config(DMA_Channel_e DMA_chx,DMA_Trigger_TypeDef dma_trigger);

void DMA_div_chx_Enable(DMA_Channel_e DMA_channel);
void DMA_div_chx_Disable(DMA_Channel_e DMA_channel);

 
status_t DMA_div_priority_Init(DMA_Channel_e DMA_chx, dma_priority_t priority);
 
uint8_t DMA_div_IsBusy(DMA_Channel_e DMA_chx);
void DMA_div_chx_Reset(uint32_t DMA_chx);
int32_t DMA_WaitComplete(DMA_Channel_e DMA_chx, uint32_t timeout_ms);
void DMA_div_SoftwareTrigger(DMA_Channel_e DMA_chx);

void DMA_div_SBURST_Init(DMA_Channel_e DMA_chx, uint32_t burst);
void DMA_div_Maxlen_Set(DMA_Channel_e DMA_chx, uint16_t len);

void DMA_div_TriggerSource_Clean(void);
#endif /* __DMA_H__ */





