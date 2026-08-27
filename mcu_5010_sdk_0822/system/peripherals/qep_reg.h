#ifndef QEP_REG_H
#define QEP_REG_H

/* QEP Register Structure */
typedef struct
{
    /**<编码器配置寄存器*/
    /* 0x30: QEP Configuration Register */
    union {
        struct {
            __IO uint32_t ZEC            : 1;  /* bit[0]     1111*/
            __IO uint32_t ZLC            : 1;  /* bit[1]     1111*/
            __IO uint32_t ZPC            : 1;  /* bit[2]     1111*/
            __IO uint32_t ZNC            : 1;  /* bit[3]     1111*/
            __IO uint32_t RESERVED_4_7   : 4;  /* bit[7:4]   1111*/
            __IO uint32_t MODE           : 2;  /* bit[9:8]   1111*/
            __IO uint32_t FE_CNT_EN      : 1;  /* bit[10]    1111*/
            __IO uint32_t RESERVED_11_14 : 4;  /* bit[14:11] 1111*/
            __IO uint32_t EN             : 1;  /* bit[15]    1111*/
            __IO uint32_t CLK_DIV        : 3;  /* bit[18:16] */
            __IO uint32_t RESERVED_19    : 1;  /* bit[19]    */
            __IO uint32_t SAP            : 11; /* bit[30:20] */
            __IO uint32_t RESERVED_31    : 1;  /* bit[31]    */
        };
        __IO uint32_t w;
    } CFG;

    /**<编码器计数门限寄存器*/
    /* 0x34: QEP Threshold Register */
    union {
        struct {
            __IO uint32_t TH             : 16; /* bit[15:0]  1111*/
            __IO uint32_t RESERVED_16_31 : 16; /* bit[31:16] */
        };
        __IO uint32_t w;
    } TH;

    /**<编码器计数值寄存器*/
    /* 0x38: QEP Counter Register */
    union {
        struct {
            __IO uint32_t CNT            : 16; /* bit[15:0]  1111*/
            __IO uint32_t RESERVED_16_31 : 16; /* bit[31:16] 1111*/
        };
        __IO uint32_t w;
    } CNT;

    /**<编码器中断使能*/
    /* 0x3C: QEP Interrupt Enable Register */
    union {
        struct {
            __IO uint32_t UF_IE          : 1;  /* bit[0]     1111*/
            __IO uint32_t OF_IE          : 1;  /* bit[1]     1111*/
            __IO uint32_t RESERVED_2_31  : 30; /* bit[31:2]  1111*/
        };
        __IO uint32_t w;
    } IE;

    /**<编码器中断标志位*/
    /* 0x40: QEP Interrupt Flag Register */
    union {
        struct {
            __IO uint32_t OF_IF         : 1;  /* bit[0]     1111*/
            __IO uint32_t UF_IF         : 1;  /* bit[1]     1111*/
            __IO uint32_t RESERVED_2_31 : 30; /* bit[31:2]  1111*/
        };
        __IO uint32_t w;
    } IF;

}TIM_QEPTypeDef;


#if 0
typedef struct
{
    __IO uint32_t CFG; /**<编码器配置寄存器*/      /* 0x30 */
    __IO uint32_t TH;  /**<编码器计数门限寄存器*/  /* 0x34 */
    __IO uint32_t CNT; /**<编码器计数值寄存器*/    /* 0x38 */
    __IO uint32_t IE;  /**<编码器中断使能*/        /* 0x3C */
    __IO uint32_t IF;  /**<编码器中断标志位*/      /* 0x40 */
} TIM_QEPTypeDef;
#endif

#endif
