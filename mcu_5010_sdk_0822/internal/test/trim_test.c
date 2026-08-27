#include "platform_common.h"
#include "gpio_hal.h"
#include "anac_reg.h"

PORT_TypeDef *gpio_ptr = GPIO_PORTA;
ANAC_TypeDef *anac_ptr = ANA_CONTROL;

uint8_t test_Number = 0;

uint32_t AF0_STR = 0;
uint32_t ANAC_MUX = 0;
uint32_t PA_ANA_STR = 0;
uint32_t ANAC_ENC = 0;
uint32_t ANAC_ADC_TEMP = 0;

/**
 * @brief  VBG（带隙基准电压）Trim 测试初始化
 * @note   配置 PA6 为模拟测试引脚输出，将 DTP/ATP MUX 路由至 VBG 节点（测试项编号 1）
 */
void VBG_Trim_Init(void)
{
    //暂存寄存器初值，结束测试项后复原对应寄存器为初值
    AF0_STR = gpio_ptr->AF0_STR.w;
    ANAC_MUX = anac_ptr->ANAC_MUX.w;

    //寄存器配置
    gpio_ptr->AF0_STR.AF6 = 0x0E;
    anac_ptr->ANAC_MUX.DTP_MUX_EN = 1;
    anac_ptr->ANAC_MUX.DTP_MUX = 0x09;
    delay_1us(1000);
    anac_ptr->ANAC_MUX.ATP_MUX_EN = 1;
    anac_ptr->ANAC_MUX.ATP_MUX = 0x03;
    test_Number = 1;
}

/**
 * @brief  写入 VBG Trim 校准值
 * @param  trim_value  6 位校准值（有效位 [5:0]），超出范围的高位自动屏蔽
 */
void VBG_Trim_Set(uint8_t trim_value)
{
    anac_ptr->ANAC_TRM2.BG_TTRM = trim_value & 0x3f;
}


/**
 * @brief  VREF（模拟参考电压）Trim 测试初始化
 * @note   配置 PA6 为模拟测试引脚输出，将 DTP/ATP MUX 路由至 VREF 节点（测试项编号 2）
 */
void VREF_Trim_Init(void)
{
    //暂存寄存器初值，结束测试项后复原对应寄存器为初值
    AF0_STR = gpio_ptr->AF0_STR.w;
    ANAC_MUX = anac_ptr->ANAC_MUX.w;

    //寄存器配置
    gpio_ptr->AF0_STR.AF6 = 0x0E;
    anac_ptr->ANAC_MUX.DTP_MUX_EN = 1;
    anac_ptr->ANAC_MUX.DTP_MUX = 0x0A;
    delay_1us(1000);
    anac_ptr->ANAC_MUX.ATP_MUX_EN = 1;
    anac_ptr->ANAC_MUX.ATP_MUX = 0x02;
    test_Number = 2;
}

/**
 * @brief  写入 VREF Trim 校准值
 * @param  trim_value  6 位校准值（有效位 [5:0]），超出范围的高位自动屏蔽
 */
void VREF_Trim_Set(uint8_t trim_value)
{
    anac_ptr->ANAC_TRM1.REF_VTRM = trim_value & 0x3f;
}


/**
 * @brief  SLP_VREF（睡眠模式参考电压）Trim 测试初始化
 * @note   使能 SLP_REF，将 ATP MUX 路由至 SLP_VREF 节点（测试项编号 3）
 */
void SLP_VREF_Trim_Init(void)
{
    //暂存寄存器初值，结束测试项后复原对应寄存器为初值
    ANAC_MUX = anac_ptr->ANAC_MUX.w;
    PA_ANA_STR = gpio_ptr->ANA_STR.w;
    ANAC_ENC = anac_ptr->ANAC_ENCR.w;

    //寄存器配置
    gpio_ptr->ANA_STR.ANA = 0x20;
    anac_ptr->ANAC_ENCR.SLP_REF_EN = 1;
    anac_ptr->ANAC_MUX.ATP_MUX_EN = 1;
    anac_ptr->ANAC_MUX.ATP_MUX = 0x01;
    test_Number = 3;
}

/**
 * @brief  写入 SLP_VREF Trim 校准值
 * @param  trim_value  4 位校准值（有效位 [3:0]），超出范围的高位自动屏蔽
 */
void SLP_VREF_Trim_Set(uint8_t trim_value)
{
    anac_ptr->ANAC_TRM1.SLP_REF_VTRM = trim_value & 0x0f;
}


/**
 * @brief  LDO12（1.2V LDO）Trim 测试初始化
 * @note   使能 ANA_LDO12，将 ATP MUX 路由至 LDO12 输出节点（测试项编号 4）
 */
void LDO12_Trim_Init(void)
{
    //暂存寄存器初值，结束测试项后复原对应寄存器为初值
    ANAC_MUX = anac_ptr->ANAC_MUX.w;
    PA_ANA_STR = gpio_ptr->ANA_STR.w;
    ANAC_ENC = anac_ptr->ANAC_ENCR.w;

    //寄存器配置
    gpio_ptr->ANA_STR.ANA = 0x20;
    anac_ptr->ANAC_ENCR.ANA_LDO12_EN = 1;
    anac_ptr->ANAC_MUX.ATP_MUX_EN = 1;
    anac_ptr->ANAC_MUX.ATP_MUX = 0x0D;
    test_Number = 4;
}

/**
 * @brief  写入 LDO12 Trim 校准值
 * @param  trim_value  3 位校准值（有效位 [2:0]），超出范围的高位自动屏蔽
 */
void LDO12_Trim_Set(uint8_t trim_value)
{
    anac_ptr->ANAC_TRM1.LDO12_VTRM = trim_value & 0x07;
}


/**
 * @brief  SLP_LDO12（睡眠模式 1.2V LDO）Trim 测试初始化
 * @note   关闭正常模式 ANA_LDO12，将 ATP MUX 路由至 SLP_LDO12 节点（测试项编号 5）
 */
void SLP_LDO12_Trim_Init(void)
{
    //暂存寄存器初值，结束测试项后复原对应寄存器为初值
    ANAC_MUX = anac_ptr->ANAC_MUX.w;
    PA_ANA_STR = gpio_ptr->ANA_STR.w;
    ANAC_ENC = anac_ptr->ANAC_ENCR.w;

    //寄存器配置
    gpio_ptr->ANA_STR.ANA = 0x20;
    anac_ptr->ANAC_ENCR.ANA_LDO12_EN = 0;
    anac_ptr->ANAC_MUX.ATP_MUX_EN = 1;
    anac_ptr->ANAC_MUX.ATP_MUX = 0x0C;
    test_Number = 5;
}

/**
 * @brief  写入 SLP_LDO12 Trim 校准值
 * @param  trim_value  2 位校准值（有效位 [1:0]），超出范围的高位自动屏蔽
 */
void SLP_LDO12_Trim_Set(uint8_t trim_value)
{
    anac_ptr->ANAC_TRM1.SLP12_VTRM = trim_value & 0x03;
}


/**
 * @brief  ADC_VREFH（ADC 高参考电压）Trim 测试初始化
 * @note   使能 ADC_VREFH ATP 输出，将 ATP MUX 路由至 ADC_VREFH 节点，ADC_VREFH 选档 0x02（测试项编号 6）
 */
void ADC_VREFH_Trim_Init(void)
{
    //暂存寄存器初值，结束测试项后复原对应寄存器为初值
    ANAC_MUX = anac_ptr->ANAC_MUX.w;
    PA_ANA_STR = gpio_ptr->ANA_STR.w;
    ANAC_ENC = anac_ptr->ANAC_ENCR.w;
    ANAC_ADC_TEMP = anac_ptr->ANAC_ADC.w;

    //寄存器配置
    gpio_ptr->ANA_STR.ANA = 0x20;
    anac_ptr->ANAC_MUX.ATP_MUX_EN = 1;
    anac_ptr->ANAC_MUX.ATP_MUX = 0x06;
    anac_ptr->ANAC_ADC.ADC_VREFH_ATP = 1;
    anac_ptr->ANAC_ADC.ADC_VREFH = 0x02;
    test_Number = 6;
}

/**
 * @brief  写入 ADC_VREFH Trim 校准值
 * @param  trim_value  3 位校准值（有效位 [2:0]），超出范围的高位自动屏蔽
 */
void ADC_VREFH_Trim_Set(uint8_t trim_value)
{
    anac_ptr->ANAC_TRM1.ADC_VREFH_VTRM = trim_value & 0x07;
}


/**
 * @brief  HSI（高速内部 RC 振荡器）Trim 测试初始化
 * @note   使能 ANA_HSI，将 DTP MUX 路由至 HSI 频率输出节点（测试项编号 7）
 */
void HSI_Trim_Init(void)
{
    //暂存寄存器初值，结束测试项后复原对应寄存器为初值
    AF0_STR = gpio_ptr->AF0_STR.w;
    ANAC_MUX = anac_ptr->ANAC_MUX.w;
    ANAC_ENC = anac_ptr->ANAC_ENCR.w;

    //寄存器配置
    gpio_ptr->AF0_STR.AF6 = 0x0E;
    anac_ptr->ANAC_MUX.DTP_MUX_EN = 1;
    anac_ptr->ANAC_MUX.DTP_MUX = 0x13;
    anac_ptr->ANAC_ENCR.ANA_HSI_EN = 1;
    test_Number = 7;
}

/**
 * @brief  写入 HSI Trim 校准值
 * @param  trim_value  7 位频率校准值（有效位 [6:0]），超出范围的高位自动屏蔽
 */
void HSI_Trim_Set(uint8_t trim_value)
{
    anac_ptr->ANAC_TRM2.HSI_FTRM = trim_value & 0x7f;
}


/**
 * @brief  LSI（低速内部 RC 振荡器）Trim 测试初始化
 * @note   使能 ANA_LSI，将 DTP MUX 路由至 LSI 频率输出节点（测试项编号 8）
 */
void LSI_Trim_Init(void)
{
    //暂存寄存器初值，结束测试项后复原对应寄存器为初值
    AF0_STR = gpio_ptr->AF0_STR.w;
    ANAC_MUX = anac_ptr->ANAC_MUX.w;
    ANAC_ENC = anac_ptr->ANAC_ENCR.w;

    //寄存器配置
    gpio_ptr->AF0_STR.AF6 = 0x0E;
    anac_ptr->ANAC_MUX.DTP_MUX_EN = 1;
    anac_ptr->ANAC_MUX.DTP_MUX = 0x12;
    anac_ptr->ANAC_ENCR.LSI_32K_EN = 1;
    test_Number = 8;
}

/**
 * @brief  写入 LSI Trim 校准值
 * @param  trim_value  8 位频率校准值（全部位有效，无屏蔽）
 */
void LSI_Trim_Set(uint8_t trim_value)
{
    anac_ptr->ANAC_TRM2.LSI_FTRM = trim_value;
}


/**
 * @brief  结束当前 Trim 测试项，恢复所有被修改的寄存器为初始值
 * @note   必须在每个 Trim 测试项完成后调用，防止寄存器配置影响后续正常运行
 */
void trim_test_end(void)
{
    //每个trim测试项结束之后必须调用该函数让被调用的寄存器恢复初始值
    switch (test_Number)
    {
    case 1:
        gpio_ptr->AF0_STR.w = AF0_STR;
        anac_ptr->ANAC_MUX.w = ANAC_MUX;
        break;
    
    case 2:
        gpio_ptr->AF0_STR.w = AF0_STR;
        anac_ptr->ANAC_MUX.w = ANAC_MUX;
        break;

    case 3:
        anac_ptr->ANAC_MUX.w = ANAC_MUX;
        gpio_ptr->ANA_STR.w = PA_ANA_STR;
        anac_ptr->ANAC_ENCR.w = ANAC_ENC;
        break;

    case 4:
        anac_ptr->ANAC_MUX.w = ANAC_MUX;
        gpio_ptr->ANA_STR.w = PA_ANA_STR;
        anac_ptr->ANAC_ENCR.w = ANAC_ENC;
        break;

    case 5:
        anac_ptr->ANAC_MUX.w = ANAC_MUX;
        gpio_ptr->ANA_STR.w = PA_ANA_STR;
        anac_ptr->ANAC_ENCR.w = ANAC_ENC;
        break;

    case 6:
        anac_ptr->ANAC_MUX.w = ANAC_MUX;
        gpio_ptr->ANA_STR.w = PA_ANA_STR;
        anac_ptr->ANAC_ENCR.w = ANAC_ENC;
        anac_ptr->ANAC_ADC.w = ANAC_ADC_TEMP;
        break;

    case 7:
        anac_ptr->ANAC_MUX.w = ANAC_MUX;
        gpio_ptr->ANA_STR.w = PA_ANA_STR;
        anac_ptr->ANAC_ENCR.w = ANAC_ENC;
        break;

    case 8:
        anac_ptr->ANAC_MUX.w = ANAC_MUX;
        gpio_ptr->ANA_STR.w = PA_ANA_STR;
        anac_ptr->ANAC_ENCR.w = ANAC_ENC;
        break;

    default:
        
        break;
    }
    test_Number = 0;
}

/**
 * @brief  读取当前正在执行的 Trim 测试项编号
 * @return 当前测试项编号（1~8），0 表示无进行中的测试项
 */
uint8_t read_test_number(void)
{
    return test_Number;
}
