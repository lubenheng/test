#ifndef DMA_TYPE_H
#define DMA_TYPE_H

/**
 * @brief DMA外设选择枚举（按功能分类）   触发源
 */
typedef enum
{
    /* ===== SPI peripherals ===== */
    DMA_Trigger_SPI0_TX          = 0x00U,  /*!< 00000b: SPI0 transmit */
    DMA_Trigger_SPI0_RX          = 0x01U,  /*!< 00001b: SPI0 receive */
    DMA_Trigger_SPI1_TX          = 0x02U,  /*!< 00010b: SPI1 transmit */
    DMA_Trigger_SPI1_RX          = 0x03U,  /*!< 00011b: SPI1 receive */
    
    /* ===== I2C peripherals ===== */
    DMA_Trigger_I2C0_TX          = 0x04U,  /*!< 00100b: I2C0 transmit */
    DMA_Trigger_I2C0_RX          = 0x05U,  /*!< 00101b: I2C0 receive */
    DMA_Trigger_I2C1_TX          = 0x06U,  /*!< 00110b: I2C1 transmit */
    DMA_Trigger_I2C1_RX          = 0x07U,  /*!< 00111b: I2C1 receive */
    
    /* ===== MCPWM peripherals ===== */
    DMA_Trigger_MCPWM_BCO        = 0x08U,  /*!< 01000b: MCPWM Base0 Counter Overflow */
    DMA_Trigger_MCPWM_BC1        = 0x09U,  /*!< 01001b: MCPWM Base1 Counter Overflow */
    
    /* ===== Timer peripherals ===== */
    DMA_Trigger_TIMER13           = 0x0AU,  /*!< 01010b: TIMER13 */
    DMA_Trigger_TIMER14           = 0x0BU,  /*!< 01011b: TIMER14 */
    DMA_Trigger_TIMER15           = 0x0CU,  /*!< 01100b: TIMER15 */
    DMA_Trigger_TIMER16           = 0x0DU,  /*!< 01101b: TIMER16 */

    /* ===== UART peripherals ===== */
    DMA_Trigger_UART0_TX          = 0x0EU,  /*!< 01110b: UART0 transmit */
    DMA_Trigger_UART0_RX          = 0x0FU,  /*!< 01111b: UART0 receive */
    DMA_Trigger_UART1_TX          = 0x10U,  /*!< 10000b: UART1 transmit */
    DMA_Trigger_UART1_RX          = 0x11U,  /*!< 10001b: UART1 receive */

    /* ===== ADC peripherals ===== */
    DMA_Trigger_ADC               = 0x12U,  /*!< 10010b: ADC */

    /* ===== Reserved values (0x13-0x1F) ===== */
    DMA_Trigger_RESERVED         = 0x1FU,  /*!< Reserved value */
} DMA_Trigger_TypeDef;

 

/**
 * @brief DMA通道号定义
 * @note DMA模块支持6个通道
 */ 
typedef enum
{
	DMA_CHANNEL_0      =    0U,	
	DMA_CHANNEL_1      =    1U,	
	DMA_CHANNEL_2      =    2U,	
	DMA_CHANNEL_3      =    3U,	
	DMA_CHANNEL_4      =    4U,	
	DMA_CHANNEL_5      =    5U,	
	DMA_CHANNEL_MAX    =    6U,	
} DMA_Channel_e;

 


/**
 * @brief DMA通道优先级枚举
 * @note 对应控制寄存器DMACHn_CTL的PL位域[10:9]
 */
typedef enum
{
    DMA_PRIORITY_LOW        = 0x0,  /* 00b: 低优先级 */
    DMA_PRIORITY_MID     		= 0x1,  /* 01b: 中等优先级 */
    DMA_PRIORITY_HIGH       = 0x2,  /* 10b: 高优先级 */
    DMA_PRIORITY_HIGHER  		= 0x3,  /* 11b: 很高优先级 */
} dma_priority_t;

/**
 * @brief DMA数据宽度枚举
 * @note 对应控制寄存器DMACHn_CTL的SSIZE/DSIZE位域[4:3]和[8:7]
 */
typedef enum
{
    DMA_BITSIZE_8BIT     = 0x0,  /* 00b: 1字节 8位	 */
    DMA_BITSIZE_16BIT     = 0x1,   /* 01b: 2字节 */
    DMA_BITSIZE_32BIT     = 0x2,   /* 10b: 4/字节 */
    DMA_BITSIZE_RESERVED = 0x3,  /* 11b: 保留 */
} dma_data_width_t;

/**
 * @brief DMA突发长度枚举
 * @note 对应控制寄存器DMACHn_CTL的SBURST位域[6:5]
 */
typedef enum
{
    DMA_BURST_1_UNIT        = 0x0,  /* 00b: 每次传1个单位 */
    DMA_BURST_4_UNITS       = 0x1,  /* 01b: 每次传4个单位 */
    DMA_BURST_8_UNITS       = 0x2,  /* 10b: 每次传8个单位 */
    DMA_BURST_16_UNITS      = 0x3,  /* 11b: 每次传16个单位 */
} dma_burst_t;

/**
 * @brief DMA传输方向枚举
 * @note 对应控制寄存器DMACHn_CTL的DIR位[13]
 */
typedef enum
{
    DMA_DIR_PERIPH_TO_MEM   = 0x0,  /* 0b: 外设到内存 */
    DMA_DIR_MEM_TO_PERIPH   = 0x1,  /* 1b: 内存到外设 */
	  DMA_DIR_MEM_TO_MEM      = 0x2,  /* 2b: 内存到内存 */
} dma_direction_t;

/**
 * @brief DMA传输模式枚举
 */
typedef enum
{
    DMA_MODE_NORMAL         = 0x0,  /* 正常模式，完成后停止 */
    DMA_MODE_CIRCULAR       = 0x1,  /* 循环模式，自动重装 */
} dma_mode_t;


/**
 * @brief DMA地址递增模式枚举
 * @note 对应控制寄存器DMACHn_CTL的SINC/DINC位[1]和[2]
 */
typedef enum
{
    Addr_Disable     = 0x0,  /* 0b: 地址固定 */
    Addr_Enable      = 0x1,  /* 1b: 地址递增 */
} dma_addr_mode_t;



/**
 * @brief DMA中断类型枚举
 * @note 对应DMA_MASK和DMA_STAT寄存器
 */
typedef enum
{
    DMA_INT_TRANSFER_COMPLETE   = (1 << 0),  /* 数据传输完成中断 */
    DMA_INT_TRANSFER_ERROR      = (1 << 1),  /* 数据传输错误中断 */
    DMA_INT_CONFIG_ERROR        = (1 << 2),  /* 配置错误中断 */
    DMA_INT_ALL                 = 0x07,      /* 所有中断 */
} dma_int_type_t;

/**
 * @brief DMA中断标志枚举
 * @note 对应DMA_STAT寄存器中的标志位
 */
typedef enum 
{
    DMA_FLAG_IFD0       = (1 << 0),   /* 通道0传输完成中断标志 */
    DMA_FLAG_IFD1       = (1 << 1),   /* 通道1传输完成中断标志 */
    DMA_FLAG_IFD2       = (1 << 2),   /* 通道2传输完成中断标志 */
    DMA_FLAG_IFD3       = (1 << 3),   /* 通道3传输完成中断标志 */
    DMA_FLAG_IFD4       = (1 << 4),   /* 通道4传输完成中断标志 */
    DMA_FLAG_IFD5       = (1 << 5),   /* 通道5传输完成中断标志 */
    
    DMA_FLAG_IFE0       = (1 << 8),   /* 通道0错误中断标志 */
    DMA_FLAG_IFE1       = (1 << 9),   /* 通道1错误中断标志 */
    DMA_FLAG_IFE2       = (1 << 10),  /* 通道2错误中断标志 */
    DMA_FLAG_IFE3       = (1 << 11),  /* 通道3错误中断标志 */
    DMA_FLAG_IFE4       = (1 << 12),  /* 通道4错误中断标志 */
    DMA_FLAG_IFE5       = (1 << 13),  /* 通道5错误中断标志 */
    
    DMA_FLAG_ACT0       = (1 << 16),  /* 通道0活动标志 */
    DMA_FLAG_ACT1       = (1 << 17),  /* 通道1活动标志 */
    DMA_FLAG_ACT2       = (1 << 18),  /* 通道2活动标志 */
    DMA_FLAG_ACT3       = (1 << 19),  /* 通道3活动标志 */
    DMA_FLAG_ACT4       = (1 << 20),  /* 通道4活动标志 */
    DMA_FLAG_ACT5       = (1 << 21),  /* 通道5活动标志 */
    
//    DMA_FLAG_IFME       = (1 << 31U)  /* 配置错误中断标志 */
} dma_flag_t;


///**
// * @brief DMA通道状态标志枚举
// * @note 对应DMA_STAT寄存器中的状态位
// */
//typedef enum
//{
//    DMA_STATUS_TRANSFER_DONE   = 0x01,  /* 传输完成标志 (IFDn) */
//    DMA_STATUS_TRANSFER_ACTIVE = 0x02,  /* 传输活动标志 (ACTn) */
//    DMA_STATUS_CONFIG_ERROR    = 0x04,  /* 配置错误标志 (IFME) */
//    DMA_STATUS_ERROR           = 0x08,  /* 传输错误标志 (IFEn) */
//} dma_status_flag_t;


/**
 * @brief DMA通道配置结构体
 */
typedef struct
{
		DMA_Channel_e  dma_ch;					/* 通道 */
    uint32_t src_addr;              /* 源基地址 */
    uint32_t dst_addr;              /* 目的基地址 */
    uint16_t transfer_size;         /* 传输数据量（最大65535） */
    dma_data_width_t dma_data_width_src_width;     /* 源数据宽度 */
    dma_data_width_t dma_data_width_dst_width;     /* 目的数据宽度 */
    dma_burst_t burst_len;          /* 突发传输长度 */
    dma_addr_mode_t src_inc;        /* 源地址递增模式 */
    dma_addr_mode_t dst_inc;        /* 目的地址递增模式 */
    dma_priority_t priority;        /* 通道优先级 */
    dma_mode_t circular_mode ;    		 /* 循环模式 */
    dma_direction_t dir;               /* 传输方向 */
    DMA_Trigger_TypeDef DMA_Trigger_periph;        /* 关联外设 */
		uint32_t priority_it;							/* 中断优先级 */
} dma_ch_config_t;


/**
 * @brief DMA传输模式枚举
 */
typedef enum
{
    dma_it_disable         = 0x0,  /* 正常模式，完成后停止 */
    dma_it_enable       = 0x1,  /* 循环模式，自动重装 */
} dma_it_enable_t;


typedef enum {
    DMA_STATUS_TRANS_COMPLETE	= 0,  // 传输完成中断标志（IFD0~IFD5，bit0~5）
    DMA_STATUS_DATA_ERROR 		= 1,   // 数据错误中断标志（IFE0~IFE5，bit8~13）
    DMA_STATUS_TRANS_FLAG   	= 2,	  // 传输数据标志（TF0~TF5，bit16~21）
		DMA_STATUS_CONFIG_ERROR   = 3    /* 配置错误标志 (IFME) */
} DMA_Status_Type_e;




typedef enum {
    DMA_TMR0_RE0,                /**< MCPWM_CNT01 计数到MCPWM_TMR0   事件 DMA请求0 使能：1使能，0关闭 */
    DMA_TMR1_RE0,                /**< MCPWM_CNT01 计数到MCPWM_TMR1   事件 DMA请求0 使能：1使能，0关闭 */
    DMA_TMR2_RE0,                /**< MCPWM_CNT01 计数到MCPWM_TMR2   事件 DMA请求0 使能：1使能，0关闭 */
    DMA_TMR3_RE0,                /**< MCPWM_CNT01 计数到MCPWM_TMR3   事件 DMA请求0 使能：1使能，0关闭 */
    DMA_TR0_T0_RE0,              /**< MCPWM_CNT0  计数到-MCPWM_TH(T0)事件 DMA请求0 使能：1使能，0关闭 */
    DMA_TR0_T1_RE0,              /**< MCPWM_CNT0  计数到0 (T1)       事件 DMA请求0 使能：1使能，0关闭 */
    DMA_TR1_T0_RE0,              /**< MCPWM_CNT1  计数到-MCPWM_TH(T0)事件 DMA请求0 使能：1使能，0关闭 */
    DMA_TR1_T1_RE0,              /**< MCPWM_CNT1  计数到0 (T1)       事件 DMA请求0 使能：1使能，0关闭 */

    DMA_TMR0_RE1,                /**< MCPWM_CNT01 计数到MCPWM_TMR0   事件 DMA请求1 使能：1使能，0关闭 */
    DMA_TMR1_RE1,                /**< MCPWM_CNT01 计数到MCPWM_TMR1   事件 DMA请求1 使能：1使能，0关闭 */
    DMA_TMR2_RE1,                /**< MCPWM_CNT01 计数到MCPWM_TMR2   事件 DMA请求1 使能：1使能，0关闭 */
    DMA_TMR3_RE1,                /**< MCPWM_CNT01 计数到MCPWM_TMR3   事件 DMA请求1 使能：1使能，0关闭 */
    DMA_TR0_T0_RE1,              /**< MCPWM_CNT0  计数到-MCPWM_TH(T0)事件 DMA请求1 使能：1使能，0关闭 */
    DMA_TR0_T1_RE1,              /**< MCPWM_CNT0  计数到0 (T1)       事件 DMA请求1 使能：1使能，0关闭 */
    DMA_TR1_T0_RE1,              /**< MCPWM_CNT1  计数到-MCPWM_TH(T0)事件 DMA请求1 使能：1使能，0关闭 */
    DMA_TR1_T1_RE1,              /**< MCPWM_CNT1  计数到0 (T1)       事件 DMA请求1 使能：1使能，0关闭 */
} MCPWM_RE_DMA;

typedef enum{
	dst_addrs_duty,
	dst_addrs_deadtime,
	dst_addrs_cycle
}MCPWM_DMA_Dst;

typedef enum{
	MCPWM_DMA_CH0,
	MCPWM_DMA_CH1,
	MCPWM_DMA_CH2,
	MCPWM_DMA_CH3,
	MCPWM_DMA_CH4,
	MCPWM_DMA_CH5,
}MCPWM_DMA_CH;

 
 
//常量定义
#endif

