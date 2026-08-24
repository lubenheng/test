#include "platform_common.h"
#include "gpio_hal.h"
#include "adc_dma_test.h"

/* DMA 中断完成标志: DMA_IRQHandler 中置位, 主循环轮询清零 */
volatile uint8_t g_adc_dma_done = 0U;

/* DMA 单样本缓冲: 每次 DMA 将 ADC_DATA0 结果搬运到此处 */
static uint32_t g_adc_dma_buf[1];

/* 连续采集结果数组(低12位有效) */
static uint32_t g_adc_dma_samples[ADC_SAMPLE_COUNT];

/*----------------------------------------------------------------------------
 *@brief 配置 ADC 输入引脚与指示引脚
 *@param[in] none
 *@return    none
 *@note  PB12 = ADC 输入(GPIO_ALT_FUNC_0);
 *        PTC5 = 转换等待指示(推挽输出), 供示波器观察
 *----------------------------------------------------------------------------*/
void adc_gpio_init(void)
{
    gpio_settings_config_t setting_cfg;

    /* ADC 输入引脚: PB12 */
    setting_cfg.gpio_port     = ADC_DEMO_GPIO_PORT;
    setting_cfg.gpio_pin      = ADC_DEMO_GPIO_PIN;
    setting_cfg.gpio_alt      = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir      = GPIO_DIR_INPUT;
    setting_cfg.pull_cfg      = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);

    /* 转换等待指示引脚: PTC5 (推挽输出) */
    setting_cfg.gpio_port     = GPIO_C;
    setting_cfg.gpio_pin      = GPIO_PIN_5;
    setting_cfg.gpio_alt      = GPIO_ALT_FUNC_0;
    setting_cfg.gpio_dir      = GPIO_DIR_OUTPUT;
    setting_cfg.pull_cfg      = GPIO_PULL_DISABLE;
    setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
    setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
    gpio_init(&setting_cfg);
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
    ADC_ENABLE(ENABLE);

    /* 使能 ADC 的 DMA 请求(ADC_CR.DME_EN) */
    ADC->CR |= (1U << ADC_CR_DME_EN_POS);

    ADC_SetSampleGrpMode(SampleGrpMode_1);
    ADC_SetGrpChCount(ADC_Sample_Group_0, 1);

    ADC_SetGrpTrigSource(ADC_Sample_Group_0, ADC_GrpTrig_SW_TRIG);
    ADC_SEQx_CFG(ADC_SEQ0, ADC_DEMO_CHANNEL);
    ADC_SetCLK_STPT(4);
    ADC_SetCLK_COVT(2);
    ADC_SetCLK_INIT(0);
    ADC_SetCLK_SAPT(13);
    ADC_SetCLK_TrigDelay(2);
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

    INFO("ADC_DMA_DEMO--START\r\n");

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
    INFO("ADC_DMA_DEMO--OK, total %d samples\r\n", (int)sample_cnt);
    for (i = 0U; i < sample_cnt; i++)
    {
        INFO("ADC_DMA[%d] = %d\r\n", i, (int)g_adc_dma_samples[i]);
    }
}

