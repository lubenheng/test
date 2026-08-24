#ifndef ADC_DMA_TEST_H
#define ADC_DMA_TEST_H

#include "platform_common.h"
#include "ADC_Reg.h"
#include "ADC_Type.h"
#include "adc.h"
#include "dma.h"
#include "dma_hal.h"
#include "dma_type.h"

/*----------------------------------------------------------------------------
   ADC + DMA demo 配置
 *----------------------------------------------------------------------------*/
#define ADC_DEMO_CHANNEL    ADC_CH7
#define ADC_DEMO_GPIO_PORT  GPIO_B
#define ADC_DEMO_GPIO_PIN   GPIO_PIN_12
#define ADC_SAMPLE_COUNT    100U

/* DMA 通道分配 */
#define ADC_DMA_CHANNEL     DMA_CHANNEL_0

/* ADC_DATA0 数据寄存器地址(DMA 源地址), 每次转换结果写入此处 */
#define ADC_DATA0_ADDR      ((uint32_t)(&ADC->DATA[0]))

/* ADC_CR.DME_EN: ADC DMA 请求使能位 */
#define ADC_CR_DME_EN_POS   (2U)

/* DMA 中断完成标志: DMA_IRQHandler 中置位, 主循环中轮询并清零 */
extern volatile uint8_t g_adc_dma_done;

/* 函数声明 */
void adc_gpio_init(void);
void adc_init(void);
void adc_dma_init(void);
void adc_dma_demo(void);

#endif

