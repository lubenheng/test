#include "platform_common.h"
#include "gpio_hal.h"
#include "adc_dma_test.h"

#define ADC_BASE_ADDR       (0x40012400UL)
#define ADC_CR_ADDR         (ADC_BASE_ADDR + 0x60UL)   /* CR */


/* DMA 中断完成标志: DMA_IRQHandler 中置位, 主循环轮询清零 */
volatile uint8_t g_adc_dma_done = 0U;

/* DMA 单样本缓冲: 每次 DMA 将 ADC_DATA0 结果搬运到此处 */
static uint32_t g_adc_dma_buf[1];

/* 连续采集结果数组(低12位有效) */
static uint32_t g_adc_dma_samples[ADC_SAMPLE_COUNT];


void ADC_ConfigDirect(uint8_t vref_range,
                      uint8_t vrefh_to_vdd33,
                      uint8_t vrefh_en,
                      uint8_t adc_clk_sel,
                      uint8_t adc_en)
{
    uint32_t val = 0;
    val |= ((uint32_t)(vref_range & 0x3) << 20);
    val |= ((uint32_t)(vrefh_to_vdd33 & 0x1) << 17);
    val |= ((uint32_t)(vrefh_en & 0x1) << 16);
    val |= ((uint32_t)(adc_clk_sel & 0x3) << 4);
    val |= ((uint32_t)(adc_en & 0x1) << 0);
    // 其他未用位保持为0
	REG32(ADC_CR_ADDR) = val;
}
/*----------------------------------------------------------------------------
 *@brief ADC 初始化: 软件触发 + DMA 请求使能
 *@param[in] none
 *@return    none
 *@note  ADC_CR.DME_EN(bit2) 置1, 每个序列转换完成自动产生 DMA 请求
 *----------------------------------------------------------------------------*/
void adc_init(void)
{
    ADC_Clk_Enable();
    
    ADC_SetSampleGrpMode(SampleGrpMode_1);
    ADC_SetGrpChCount(ADC_Sample_Group_0, 1);
	ADC_SetGrpChCount(ADC_Sample_Group_1, 0);
	ADC_SetGrpChCount(ADC_Sample_Group_2, 0);
	ADC_SetGrpChCount(ADC_Sample_Group_3, 0);

    ADC_SetGrpTrigSource(ADC_Sample_Group_0, ADC_GrpTrig_SW_TRIG);
	ADC_SetGrpTrigSource(ADC_Sample_Group_1, ADC_GrpTrig_TMR13_TRIG);
	ADC_SetGrpTrigSource(ADC_Sample_Group_2, ADC_GrpTrig_TMR14_TRIG);
	ADC_SetGrpTrigSource(ADC_Sample_Group_3, ADC_GrpTrig_TMR15_TRIG);
	
    ADC_SEQx_CFG(ADC_SEQ0, ADC_CH27);
	ADC_SEQx_CFG(ADC_SEQ1, ADC_CH21);
	ADC_SEQx_CFG(ADC_SEQ2, ADC_CH22);
	ADC_SEQx_CFG(ADC_SEQ3, ADC_CH23);
	ADC_SEQx_CFG(ADC_SEQ4, ADC_CH24);
	ADC_SEQx_CFG(ADC_SEQ5, ADC_CH25);
	ADC_SEQx_CFG(ADC_SEQ6, ADC_CH26);
	ADC_SEQx_CFG(ADC_SEQ7, ADC_CH18);
	ADC_SEQx_CFG(ADC_SEQ8, ADC_CH28);
	ADC_SEQx_CFG(ADC_SEQ9, ADC_CH29);
	ADC_SEQx_CFG(ADC_SEQ10, ADC_CH30);
	ADC_SEQx_CFG(ADC_SEQ11, ADC_CH6);
	ADC_SEQx_CFG(ADC_SEQ12, ADC_CH7);
	ADC_SEQx_CFG(ADC_SEQ13, ADC_CH8);
	ADC_SEQx_CFG(ADC_SEQ14, ADC_CH9);
	ADC_SEQx_CFG(ADC_SEQ15, ADC_CH10);
	
	
    ADC_SetCLK_STPT(10);
//    ADC_SetCLK_COVT(10);
//    ADC_SetCLK_INIT(30);
//    ADC_SetCLK_SAPT(80);
//    ADC_SetCLK_TrigDelay(2);

ADC_ConfigDirect(1,0,1,3,1);

}

/*----------------------------------------------------------------------------
 *@brief 配置 DMA 通道0: ADC_DATA0 -> RAM 单样本缓冲 (中断驱动连续采集)
 *@param[in] none
 *@return    none
 *@note  通道0 绑定外设 ADC(CHnSEL=0x12), 外设请求驱动传输;
 *       MAX=1 + RT=1(自动重载): 每次 ADC 转换产生 DMA 请求,
 *       DMA 搬运 1 个 32bit 样本后自动重载, 等待下一次触发;
 *       每次传输完成触发 DMA 中断(IFD0), 主循环处理样本并再次触发.
 *----------------------------------------------------------------------------*/
void adc_dma_init(void)
{
    dma_ch_config_t dma_cfg;

    memset(&dma_cfg, 0, sizeof(dma_cfg));

    dma_cfg.dma_ch                   = ADC_DMA_CHANNEL;
    dma_cfg.DMA_Trigger_periph       = DMA_Trigger_ADC;         /* CH0SEL = 0x12 */
    dma_cfg.src_addr                 = ADC_DATA0_ADDR;          /* ADC_DATA0 */
    dma_cfg.dst_addr                 = (uint32_t)g_adc_dma_buf; /* 单样本缓冲 */
    dma_cfg.src_inc                  = Addr_Disable;            /* 外设地址固定 */
    dma_cfg.dst_inc                  = Addr_Disable;            /* 单缓冲, 目的固定 */
    dma_cfg.dir                      = DMA_DIR_PERIPH_TO_MEM;   /* 外设->内存 */
    dma_cfg.burst_len                = DMA_BURST_1_UNIT;        /* 单次突发 */
    dma_cfg.dma_data_width_src_width = DMA_BITSIZE_32BIT;       /* 32bit */
    dma_cfg.dma_data_width_dst_width = DMA_BITSIZE_32BIT;       /* 32bit */
    dma_cfg.transfer_size            = 1U;                      /* 每次仅搬 1 个样本 */
    dma_cfg.circular_mode            = DMA_MODE_CIRCULAR;       /* RT=1 自动重载, 连续采集 */
    dma_cfg.priority                 = DMA_PRIORITY_MID;

    DMA_div_chx_Disable(ADC_DMA_CHANNEL);      /* 配置前先失能 */
    DMA_div_TriggerSource_Clean();             /* 恢复触发源默认值 */
    HAL_DMA_StructerInit(&dma_cfg);            /* 初始化 DMA 寄存器 */

    /* 使能 DMA 通道0 传输完成中断 + NVIC (DMA_IRQn=20) */
    HAL_DMA_NVIC_Enable(ADC_DMA_CHANNEL, dma_cfg.priority);
}

/*----------------------------------------------------------------------------
 *@brief ADC+DMA 连续采集 demo: 中断驱动 + 主循环软件触发
 *@param[in] none
 *@return    none
 *@note  流程:
 *       DMA 传输完成 -> DMA_IRQHandler 置 g_adc_dma_done
 *       主循环: 检测标志 -> 处理样本 -> ADC_SWTrig() 立即触发下一次
 *       采集满 ADC_SAMPLE_COUNT 次后停止并打印结果.
 *----------------------------------------------------------------------------*/
void adc_dma_demo(void)
{
    uint32_t i;
    uint32_t sample_cnt = 0U;

    /* 使能 DMA 通道(RT=1 自动重载), 并触发第一次转换 */
    g_adc_dma_done = 0U;
    DMA_div_chx_Enable(ADC_DMA_CHANNEL);
	
    ADC_SWTrig();

    /* 主循环: 中断置标志 -> 处理数据 -> 立即启动下一次触发 */
    while (sample_cnt < ADC_SAMPLE_COUNT)
    {
        if (g_adc_dma_done != 0U)
        {
            g_adc_dma_done = 0U;

            /* 处理数据: 从 DMA 单样本缓冲读取本次转换结果(低12位) */
            g_adc_dma_samples[sample_cnt] = g_adc_dma_buf[0] & 0xFFFU;
            sample_cnt++;

            /* 立即启动下一次软件触发 */
            ADC_SWTrig();
        }
    }

    /* 采集完成, 停止 DMA 通道并打印结果 */
    DMA_div_chx_Disable(ADC_DMA_CHANNEL);
   
    for (i = 0U; i < sample_cnt; i++)
    {
        INFO("ADC_DMA[%d] = %d\r\n", i, (int)g_adc_dma_samples[i]);
    }
}

