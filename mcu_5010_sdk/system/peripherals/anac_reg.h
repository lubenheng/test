#ifndef ANAC_REG_H
#define ANAC_REG_H

/**
 * @brief ANAC使能控制寄存器 (ANAC_ENCR)
 * 偏移地址：0x00
 * 复位值：0x00000000U
 * 读写属性：可读可写
 */
typedef struct
{
    typedef union
    {
        struct
        {
            __IO uint32_t ANA_EN            : 1;        /* bit0:  模拟模块总使能 (RW)
                                                         0: 关闭模拟模块 1: 使能模拟模块 */
            __IO uint32_t DSLP_EN           : 1;        /* bit1:  休眠模式使能 (RW)
                                                         0: 正常模式 1: 休眠模式 */
            __IO uint32_t ISOLAT_EN         : 1;        /* bit2:  隔离使能 (RW)
                                                         0: 关闭隔离 1: 使能隔离 */
            __IO uint32_t LSI_32K_EN        : 1;        /* bit3:  32K低速内部振荡器使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t SLP_REF_EN        : 1;        /* bit4:  休眠参考电压使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t VDD12_POR_EN      : 1;        /* bit5:  VDD12上电复位使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t VDD33_POR_EN      : 1;        /* bit6:  VDD33上电复位使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t VDD33_BOR_EN      : 1;        /* bit7:  VDD33掉电复位使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t LDO12_SHDN        : 1;        /* bit8:  LDO12关断控制 (RW)
                                                         0: 正常工作 1: 关断 */
            __IO uint32_t DSLP_LAT_ANA      : 1;        /* bit9:  休眠模拟延迟使能 (RW)
                                                         0: 关闭延迟 1: 使能延迟 */
            __IO uint32_t DSLP_LAT_GPIO     : 1;        /* bit10: 休眠GPIO延迟使能 (RW)
                                                         0: 关闭延迟 1: 使能延迟 */
            __IO uint32_t DSLP_OFF_V12D     : 1;        /* bit11: 休眠V12D关闭使能 (RW)
                                                         0: 不关闭 1: 关闭 */
            __IO uint32_t RSTN_FW_EN        : 1;        /* bit12: 固件复位使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t RSTN_BOR33_EN     : 1;        /* bit13: VDD33掉电复位使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t ANA_PVD_EN        : 1;        /* bit14: 模拟电源检测使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t ANA_TJ_EN         : 1;        /* bit15: 模拟温度检测使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t ANA_TS_EN         : 1;        /* bit16: 模拟温度传感器使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t ANA_CSA_EN        : 1;        /* bit17: 模拟电流传感放大器使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t ANA_VREFH_EN      : 1;        /* bit18: 模拟高参考电压使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t ANA_ADC_EN        : 1;        /* bit19: 模拟ADC使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t ANA_HSE_EN        : 1;        /* bit20: 模拟高速外部时钟使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t ANA_PLL_EN        : 1;        /* bit21: 模拟PLL使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t ANA_NG_EN         : 1;        /* bit22: 模拟故障检测使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t ANA_BG_EN         : 1;        /* bit23: 模拟带隙基准使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t ANA_REF_EN        : 1;        /* bit24: 模拟参考电压使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t ANA_LDO12_EN      : 1;        /* bit25: 模拟LDO12使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t ANA_HSI_EN        : 1;        /* bit26: 模拟高速内部时钟使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t RW_EN_RSV         : 4;        /* bit27-30: 保留位 (RW) - 建议写0 */
            __IO uint32_t RESERVED_31       : 1;        /* bit31:    保留位 - 只读，值为0 */
        };
        __IO uint32_t w; /* 32位寄存器整体访问 */
    } ANAC_ENCR;

    /**
     * @brief ANAC温度控制寄存器 (ANAC_TEMP)
     * 偏移地址：0x04
     * 复位值：0x00000000U
     * 读写属性：可读可写
     */
    typedef union
    {
        struct
        {
            __IO uint32_t TJ_OTS            : 2;        /* bit0-1:  温度过高阈值设置 (RW)
                                                         00: 最低阈值 11: 最高阈值 */
            __IO uint32_t TJ_OTS_NG_EN      : 1;        /* bit2:    温度过高故障检测使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t TJ_OTS_NG         : 2;        /* bit3-4:  温度过高故障配置 (RW)
                                                         00: 无操作 11: 触发中断 */
            __IO uint32_t TJ_OTS_LAT_CLR    : 1;        /* bit5:    温度过高锁存清除 (RW)
                                                         0: 不清除 1: 清除锁存 */
            __IO uint32_t TJ_OTS_LAT_POL    : 1;        /* bit6:    温度过高锁存极性 (RW)
                                                         0: 低电平有效 1: 高电平有效 */
            __IO uint32_t RESERVED_7        : 1;        /* bit7:    保留位 - 只读，值为0 */
            __IO uint32_t TJ_OTW            : 2;        /* bit8-9:  温度警告阈值设置 (RW)
                                                         00: 最低阈值 11: 最高阈值 */
            __IO uint32_t TJ_OTW_NG_EN      : 1;        /* bit10:   温度警告故障检测使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t TJ_OTW_NG         : 2;        /* bit11-12:温度警告故障配置 (RW)
                                                         00: 无操作 11: 触发中断 */
            __IO uint32_t TJ_OTW_LAT_CLR    : 1;        /* bit13:   温度警告锁存清除 (RW)
                                                         0: 不清除 1: 清除锁存 */
            __IO uint32_t TJ_OTW_LAT_POL    : 1;        /* bit14:   温度警告锁存极性 (RW)
                                                         0: 低电平有效 1: 高电平有效 */
            __IO uint32_t RESERVED_15       : 2;        /* bit15-16:保留位 - 只读，值为0 */
            __IO uint32_t TS_ISRC           : 2;        /* bit17-18:温度传感器电流源设置 (RW)
                                                         00: 最小电流 11: 最大电流 */
            __IO uint32_t TS_GPIO5_EN       : 1;        /* bit19:   温度传感器GPIO5使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t TS_GPIO6_EN       : 1;        /* bit20:   温度传感器GPIO6使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t TEMP_RSV          : 2;        /* bit21-22:保留位 (RW) - 建议写0 */
            __IO uint32_t RESERVED_22_31    : 10;       /* bit23-31:保留位 - 只读，值为0 */
        };
        __IO uint32_t w; /* 32位寄存器整体访问 */
    } ANAC_TEMP;

    /**
     * @brief ANAC电源检测寄存器 (ANAC_PVD)
     * 偏移地址：0x08
     * 复位值：0x00000000U
     * 读写属性：可读可写
     */
    typedef union
    {
        struct
        {
            __IO uint32_t VDD33_PVD_VTH     : 3;        /* bit0-2:  VDD33电源检测阈值 (RW)
                                                         000: 最低阈值 111: 最高阈值 */
            __IO uint32_t VDD33_PVD_NG_EN   : 1;        /* bit3:    VDD33电源检测故障使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t VDD33_PVD_NG      : 2;        /* bit4-5:  VDD33电源检测故障配置 (RW)
                                                         00: 无操作 11: 触发中断 */
            __IO uint32_t VDD33_PVD_LAT_CLR : 1;        /* bit6:    VDD33电源检测锁存清除 (RW)
                                                         0: 不清除 1: 清除锁存 */
            __IO uint32_t VDD33_PVD_LAT_POL : 1;        /* bit7:    VDD33电源检测锁存极性 (RW)
                                                         0: 低电平有效 1: 高电平有效 */
            __IO uint32_t VDD33_BOR_VTH     : 3;        /* bit8-10: VDD33掉电复位阈值 (RW)
                                                         000: 最低阈值 111: 最高阈值 */
            __IO uint32_t VDD33_BOR_NG_EN   : 1;        /* bit11:   VDD33掉电复位故障使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t VDD33_BOR_NG      : 2;        /* bit12-13:VDD33掉电复位故障配置 (RW)
                                                         00: 无操作 11: 触发复位 */
            __IO uint32_t VDD33_BOR_LAT_CLR : 1;        /* bit14:   VDD33掉电复位锁存清除 (RW)
                                                         0: 不清除 1: 清除锁存 */
            __IO uint32_t VDD33_BOR_LAT_POL : 1;        /* bit15:   VDD33掉电复位锁存极性 (RW)
                                                         0: 低电平有效 1: 高电平有效 */
            __IO uint32_t PVD_RSV           : 2;        /* bit16-17:保留位 (RW) - 建议写0 */
            __IO uint32_t RESERVED_18_31    : 14;       /* bit18-31:保留位 - 只读，值为0 */
        };
        __IO uint32_t w; /* 32位寄存器整体访问 */
    } ANAC_PVD;

    /**
     * @brief ANAC时钟控制寄存器 (ANAC_CLK)
     * 偏移地址：0x0C
     * 复位值：0x00000000U
     * 读写属性：可读可写
     */
    typedef union
    {
        struct
        {
            __IO uint32_t PLL_REF_SWAP      : 1;        /* bit0:    PLL参考时钟切换 (RW)
                                                         0: 正常时钟 1: 备用时钟 */
            __IO uint32_t PLL_REF_SEL       : 1;        /* bit1:    PLL参考时钟选择 (RW)
                                                         0: 内部时钟 1: 外部时钟 */
            __IO uint32_t PLL_REF_DIV       : 1;        /* bit2:    PLL参考时钟分频 (RW)
                                                         0: 不分频 1: 2分频 */
            __IO uint32_t PLL_NDIV          : 4;        /* bit3-6:  PLL分频系数 (RW)
                                                         0000: 1分频 1111: 16分频 */
            __IO uint32_t PLL_ICP           : 5;        /* bit7-11: PLL充电电流设置 (RW)
                                                         00000: 最小电流 11111: 最大电流 */
            __IO uint32_t PLL_VCO           : 3;        /* bit12-14:PLL压控振荡器设置 (RW)
                                                         000: 最低频率 111: 最高频率 */
            __IO uint32_t PLL_CKO_DIV       : 2;        /* bit15-16:PLL输出时钟分频 (RW)
                                                         00: 1分频 11: 4分频 */
            __IO uint32_t PLL_CKO_TP_EN     : 1;        /* bit17:   PLL输出测试点使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t PLL_CKO_TP_SEL    : 1;        /* bit18:   PLL输出测试点选择 (RW)
                                                         0: 选择CKO 1: 选择其他时钟 */
            __IO uint32_t PLL_VC_TP_EN      : 1;        /* bit19:   PLL电压控制测试点使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t PLL_ERR_NG_EN     : 1;        /* bit20:   PLL错误故障检测使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t PLL_ERR_NG        : 2;        /* bit21-22:PLL错误故障配置 (RW)
                                                         00: 无操作 11: 触发中断 */
            __IO uint32_t PLL_ERR_LAT_CLR   : 1;        /* bit23:   PLL错误锁存清除 (RW)
                                                         0: 不清除 1: 清除锁存 */
            __IO uint32_t PLL_ERR_LAT_POL   : 1;        /* bit24:   PLL错误锁存极性 (RW)
                                                         0: 低电平有效 1: 高电平有效 */
            __IO uint32_t HXT_XO_OFF        : 1;        /* bit25:   外部晶振关闭 (RW)
                                                         0: 正常工作 1: 关闭 */
            __IO uint32_t HXT_DS            : 2;        /* bit26-27:外部晶振驱动强度 (RW)
                                                         00: 最低强度 11: 最高强度 */
            __IO uint32_t LSI_IBIAS         : 1;        /* bit28:   低速内部振荡器偏置电流 (RW)
                                                         0: 低电流 1: 高电流 */
            __IO uint32_t RESERVED_29_31    : 3;        /* bit29-31:保留位 - 只读，值为0 */
        };
        __IO uint32_t w; /* 32位寄存器整体访问 */
    } ANAC_CLK;

    /**
     * @brief ANAC电流传感放大器1寄存器 (ANAC_CSA1)
     * 偏移地址：0x10
     * 复位值：0x00000000U
     * 读写属性：可读可写
     */
    typedef union
    {
        struct
        {
            __IO uint32_t CSA_AMP_EN        : 1;        /* bit0:    CSA放大器使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t CSA_CSP           : 2;        /* bit1-2:  CSA正极输入选择 (RW)
                                                         00: 输入1 11: 输入4 */
            __IO uint32_t CSA_CSN           : 2;        /* bit3-4:  CSA负极输入选择 (RW)
                                                         00: 输入1 11: 输入4 */
            __IO uint32_t CSA_GAIN          : 3;        /* bit5-7:  CSA增益设置 (RW)
                                                         000: 1倍增益 111: 8倍增益 */
            __IO uint32_t CSA_REF           : 5;        /* bit8-12: CSA参考电压设置 (RW)
                                                         00000: 最低电压 11111: 最高电压 */
            __IO uint32_t CSA_CAL_EN        : 1;        /* bit13:   CSA校准使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t CSA_DAC_EN        : 1;        /* bit14:   CSA DAC使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t CSA_DAC_OUT       : 8;        /* bit15-22:CSA DAC输出值 (RW)
                                                         0x00: 最小值 0xFF: 最大值 */
            __IO uint32_t CSA_DAC_REF       : 3;        /* bit23-25:CSA DAC参考电压 (RW)
                                                         000: 最低电压 111: 最高电压 */
            __IO uint32_t CSA1_RSV          : 4;        /* bit26-29:保留位 (RW) - 建议写0 */
            __IO uint32_t RESERVED_30_31    : 2;        /* bit30-31:保留位 - 只读，值为0 */
        };
        __IO uint32_t w; /* 32位寄存器整体访问 */
    } ANAC_CSA1;

    /**
     * @brief ANAC电流传感放大器2寄存器 (ANAC_CSA2)
     * 偏移地址：0x14
     * 复位值：0x00000000U
     * 读写属性：可读可写
     */
    typedef union
    {
        struct
        {
            __IO uint32_t CSA_OC_EN         : 1;        /* bit0:    CSA过流检测使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t CSA_OC_HYS        : 3;        /* bit1-3:  CSA过流迟滞设置 (RW)
                                                         000: 最低迟滞 111: 最高迟滞 */
            __IO uint32_t CSA_OC_SEL        : 3;        /* bit4-6:  CSA过流阈值选择 (RW)
                                                         000: 最低阈值 111: 最高阈值 */
            __IO uint32_t CSA_OC_NG_EN      : 1;        /* bit7:    CSA过流故障使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t CSA_OC_NG         : 2;        /* bit8-9:  CSA过流故障配置 (RW)
                                                         00: 无操作 11: 触发中断 */
            __IO uint32_t CSA_OC_LAT_CLR    : 1;        /* bit10:   CSA过流锁存清除 (RW)
                                                         0: 不清除 1: 清除锁存 */
            __IO uint32_t CSA_OC_LAT_POL    : 1;        /* bit11:   CSA过流锁存极性 (RW)
                                                         0: 低电平有效 1: 高电平有效 */
            __IO uint32_t CSA_LPF_GPIO      : 1;        /* bit12:   CSA低通滤波器GPIO使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t CSA_LPF_SEL       : 1;        /* bit13:   CSA低通滤波器选择 (RW)
                                                         0: 滤波器1 1: 滤波器2 */
            __IO uint32_t CSA_LPF           : 2;        /* bit14-15:CSA低通滤波器参数 (RW)
                                                         00: 最低截止频率 11: 最高截止频率 */
            __IO uint32_t CSA_ATP_MUX       : 2;        /* bit16-17:CSA模拟测试点选择 (RW)
                                                         00: 测试点1 11: 测试点4 */
            __IO uint32_t CSA2_RSV          : 4;        /* bit18-21:保留位 (RW) - 建议写0 */
            __IO uint32_t RESERVED_22_31    : 10;       /* bit22-31:保留位 - 只读，值为0 */
        };
        __IO uint32_t w; /* 32位寄存器整体访问 */
    } ANAC_CSA2;

    /**
     * @brief ANAC ADC控制寄存器 (ANAC_ADC)
     * 偏移地址：0x18
     * 复位值：0x00000000U
     * 读写属性：可读可写
     */
    typedef union
    {
        struct
        {
            __IO uint32_t ADC_START         : 1;        /* bit0:    ADC启动控制 (RW)
                                                         0: 停止转换 1: 启动转换 */
            __IO uint32_t ADC_MODE          : 1;        /* bit1:    ADC工作模式 (RW)
                                                         0: 单次转换 1: 连续转换 */
            __IO uint32_t ADC_MUX_EN        : 1;        /* bit2:    ADC多路选择器使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t ADC_MUX           : 5;        /* bit3-7:  ADC通道选择 (RW)
                                                         00000: 通道0 11111: 通道31 */
            __IO uint32_t ADC_CLK           : 2;        /* bit8-9:  ADC时钟分频 (RW)
                                                         00: 1分频 11: 4分频 */
            __IO uint32_t ADC_VALID         : 1;        /* bit10:   ADC有效标志使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t ADC_IBIAS         : 1;        /* bit11:   ADC偏置电流 (RW)
                                                         0: 低电流 1: 高电流 */
            __IO uint32_t ADC_BUF_TP        : 1;        /* bit12:   ADC缓冲器测试点使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t ADC_VREFH         : 2;        /* bit13-14:ADC高参考电压选择 (RW)
                                                         00: 1.2V 11: 3.3V */
            __IO uint32_t ADC_VREFH_BYPS    : 1;        /* bit15:   ADC高参考电压旁路 (RW)
                                                         0: 不旁路 1: 旁路 */
            __IO uint32_t ADC_VREFH_ATP     : 1;        /* bit16:   ADC高参考电压测试点 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t ADC_RSV           : 2;        /* bit17-18:保留位 (RW) - 建议写0 */
            __IO uint32_t RESERVED_20_31    : 12;       /* bit19-31:保留位 - 只读，值为0 */
        };
        __IO uint32_t w; /* 32位寄存器整体访问 */
    } ANAC_ADC;

    /**
     * @brief ANAC多路选择器寄存器 (ANAC_MUX)
     * 偏移地址：0x1C
     * 复位值：0x00000000U
     * 读写属性：可读可写
     */
    typedef union
    {
        struct
        {
            __IO uint32_t ATP_MUX           : 5;        /* bit0-4:  模拟测试点选择 (RW)
                                                         00000: 测试点0 11111: 测试点31 */
            __IO uint32_t ATP_MUX_EN        : 1;        /* bit5:    模拟测试点使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t ATP_TG_EN         : 1;        /* bit6:    模拟测试点触发使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t ATP_BUF_EN        : 1;        /* bit7:    模拟测试点缓冲器使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t DTP_MUX           : 5;        /* bit8-12: 数字测试点选择 (RW)
                                                         00000: 测试点0 11111: 测试点31 */
            __IO uint32_t DTP_MUX_EN        : 1;        /* bit13:   数字测试点使能 (RW)
                                                         0: 关闭 1: 使能 */
            __IO uint32_t MUX_RSV           : 1;        /* bit14:   保留位 (RW) - 建议写0 */
            __IO uint32_t RESERVED_16_31    : 16;       /* bit15-31:保留位 - 只读，值为0 */
        };
        __IO uint32_t w; /* 32位寄存器整体访问 */
    } ANAC_MUX;

    /**
     * @brief ANAC微调寄存器1 (ANAC_TRM1)
     * 偏移地址：0x20
     * 复位值：0x00000000U
     * 读写属性：可读可写 (微调参数，建议出厂配置)
     */
    typedef union
    {
        struct
        {
            __IO uint32_t REF_VTRM          : 5;        /* bit0-4:  参考电压微调 (RW)
                                                         00000: 最小值 11111: 最大值 */
            __IO uint32_t REF_ITRM          : 4;        /* bit5-8:  参考电流微调 (RW)
                                                         0000: 最小值 1111: 最大值 */
            __IO uint32_t LDO12_VTRM        : 3;        /* bit9-11: LDO12电压微调 (RW)
                                                         000: 最小值 111: 最大值 */
            __IO uint32_t SLP12_VTRM        : 2;        /* bit12-13:休眠LDO12电压微调 (RW)
                                                         00: 最小值 11: 最大值 */
            __IO uint32_t SLP_REF_VTRM      : 4;        /* bit14-17:休眠参考电压微调 (RW)
                                                         0000: 最小值 1111: 最大值 */
            __IO uint32_t ADC_VREFH_VTRM    : 3;        /* bit18-20:ADC高参考电压微调 (RW)
                                                         000: 最小值 111: 最大值 */
            __IO uint32_t CSA_OS_VTRM       : 5;        /* bit21-25:CSA失调电压微调 (RW)
                                                         00000: 最小值 11111: 最大值 */
            __IO uint32_t RESERVED_26_31    : 6;        /* bit26-31:保留位 - 只读，值为0 */
        };
        __IO uint32_t w; /* 32位寄存器整体访问 */
    } ANAC_TRM1;

    /**
     * @brief ANAC微调寄存器2 (ANAC_TRM2)
     * 偏移地址：0x24
     * 复位值：0x00000000U
     * 读写属性：可读可写 (微调参数，建议出厂配置)
     */
    typedef union
    {
        struct
        {
            __IO uint32_t HSI_FTRM          : 7;        /* bit0-6:  HSI频率微调 (RW)
                                                         0000000: 最小值 1111111: 最大值 */
            __IO uint32_t LSI_FTRM          : 7;        /* bit7-13: LSI频率微调 (RW)
                                                         0000000: 最小值 1111111: 最大值 */
            __IO uint32_t TJ_TTRM           : 5;        /* bit14-18:温度传感器温度微调 (RW)
                                                         00000: 最小值 11111: 最大值 */
            __IO uint32_t TS_ITRM           : 6;        /* bit19-24:温度传感器电流微调 (RW)
                                                         000000: 最小值 111111: 最大值 */
            __IO uint32_t BG_TTRM           : 6;        /* bit25-30:带隙基准温度微调 (RW)
                                                         000000: 最小值 111111: 最大值 */
            __IO uint32_t RESERVED_31       : 1;        /* bit31:   保留位 - 只读，值为0 */
        };
        __IO uint32_t w; /* 32位寄存器整体访问 */
    } ANAC_TRM2;
     
    /* 0x28: INFO_0 */
    __IO uint32_t INFO_0;

    /* 0x2C: INFO_1 */
    __IO uint32_t INFO_1;

    /* 0x30: INFO_2 */
    __IO uint32_t INFO_2;

    /* 0x34: INFO_3 */
    __IO uint32_t INFO_3;


    /* 0x38: Reserved */
    __IO uint32_t Reserved_38;

    /* 0x3C: Reserved */
    __IO uint32_t Reserved_3C;



    /**
     * @brief ANAC中断寄存器 (ANAC_IR)
     * 偏移地址：0x40
     * 复位值：0x00000000U
     * 读写属性：读清除 (读取1清除中断标志)
     */
    typedef union
    {
        struct
        {
            __IO uint32_t IR_VDD33_PVD_NG   : 1;        /* bit0:    VDD33电源检测中断标志 (RC)
                                                         0: 无中断 1: 有中断，读取1清除 */
            __IO uint32_t IR_VDD33_BOR_NG   : 1;        /* bit1:    VDD33掉电复位中断标志 (RC)
                                                         0: 无中断 1: 有中断，读取1清除 */
            __IO uint32_t IR_TJ_OTW_NG      : 1;        /* bit2:    温度警告中断标志 (RC)
                                                         0: 无中断 1: 有中断，读取1清除 */
            __IO uint32_t IR_TJ_OTS_NG      : 1;        /* bit3:    温度过高中断标志 (RC)
                                                         0: 无中断 1: 有中断，读取1清除 */
            __IO uint32_t IR_CSA_OC_NG      : 1;        /* bit4:    CSA过流中断标志 (RC)
                                                         0: 无中断 1: 有中断，读取1清除 */
            __IO uint32_t RESERVED_5_31     : 27;       /* bit5-31: 保留位 - 只读，值为0 */
        };
        __IO uint32_t w; /* 32位寄存器整体访问 */
    } ANAC_IR;

    /**
     * @brief ANAC只读状态寄存器 (ANAC_RO)
     * 偏移地址：0x44
     * 复位值：0x00000000U
     * 读写属性：只读
     */
    typedef union
    {
        struct
        {
            __I uint32_t VDD_RSTN           : 1;        /* bit0:    VDD复位状态 (RO)
                                                         0: 复位中 1: 复位完成 */
            __I uint32_t PLL_RDY            : 1;        /* bit1:    PLL就绪状态 (RO)
                                                         0: 未就绪 1: 就绪 */
            __I uint32_t REF_RDY            : 1;        /* bit2:    参考电压就绪状态 (RO)
                                                         0: 未就绪 1: 就绪 */
            __I uint32_t BG_RDY             : 1;        /* bit3:    带隙基准就绪状态 (RO)
                                                         0: 未就绪 1: 就绪 */
            __I uint32_t PLL_ERR_LAT        : 1;        /* bit4:    PLL错误锁存状态 (RO)
                                                         0: 无错误 1: 有错误 */
            __I uint32_t VDD33_PVD_LAT      : 1;        /* bit5:    VDD33电源检测锁存状态 (RO)
                                                         0: 无故障 1: 有故障 */
            __I uint32_t VDD33_BOR_LAT      : 1;        /* bit6:    VDD33掉电复位锁存状态 (RO)
                                                         0: 无故障 1: 有故障 */
            __I uint32_t TJ_OTW_LAT         : 1;        /* bit7:    温度警告锁存状态 (RO)
                                                         0: 无警告 1: 有警告 */
            __I uint32_t TJ_OTS_LAT         : 1;        /* bit8:    温度过高锁存状态 (RO)
                                                         0: 无过高 1: 有过高 */
            __I uint32_t CSA_OC_LAT         : 1;        /* bit9:    CSA过流锁存状态 (RO)
                                                         0: 无过流 1: 有过流 */
            __I uint32_t RESERVED_10_11     : 2;        /* bit10-11:保留位 - 只读，值为0 */
            __I uint32_t VER_CODE           : 4;        /* bit12-15:版本号 (RO)
                                                         0000: 版本0 1111: 版本15 */
            __I uint32_t RESERVED_16_31     : 16;       /* bit16-31:保留位 - 只读，值为0 */
        };
        __I uint32_t w; /* 32位寄存器整体访问 (只读) */
    } ANAC_RO;
} ANAC_TypeDef;

#endif /* ANAC_REG_H */