#ifndef PMU_REG_H
#define PMU_REG_H

//PMU_BASE = 4002_0500

typedef struct
{
    /* 0x0: WAKEIR */
    //__IO uint32_t WAKEIR;
    union {
        struct {
            __IO uint32_t LPTMR_IE               : 1;    /* bit[0]     */
            __IO uint32_t GPIO_IE                : 1;    /* bit[1]     */
            __IO uint32_t RETN_IE                : 1;    /* bit[2]     */
            __IO uint32_t Reserver_15_3          : 13;   /* bit[15:3]  */
            __IO uint32_t LPTMR_WAKE_FLAG        : 1;    /* bit[16]    */
            __IO uint32_t GPIO_WAKE_FLAG         : 1;    /* bit[17]    */
            __IO uint32_t RETN_WAKE_FLAG         : 1;    /* bit[18]    */
            __IO uint32_t Reserver_31_19         : 13;   /* bit[31:19] */
        };
        __IO uint32_t w;
    } WAKEIR; 

    /* 0x4: WAKECR */
    //__IO uint32_t WAKECR;
    union {
        struct {
            __IO uint32_t GPIO_IN0_PTA3_EN     : 1;    /* bit[0]     */
            __IO uint32_t GPIO_IN1_PTA4_EN     : 1;    /* bit[1]     */
            __IO uint32_t GPIO_IN2_PTA13_EN    : 1;    /* bit[2]     */
            __IO uint32_t GPIO_IN3_PTA14_EN    : 1;    /* bit[3]     */
            __IO uint32_t GPIO_IN4_PTB2_EN     : 1;    /* bit[4]     */
            __IO uint32_t GPIO_IN5_PTB3_EN     : 1;    /* bit[5]     */
            __IO uint32_t GPIO_IN6_PTB4_EN     : 1;    /* bit[6]     */
            __IO uint32_t GPIO_IN7_PTB5_EN     : 1;    /* bit[7]     */
            __IO uint32_t GPIO_IN8_PTB12_EN    : 1;    /* bit[8]     */
            __IO uint32_t GPIO_IN9_PTB13_EN    : 1;    /* bit[9]     */
            __IO uint32_t GPIO_IN10_PTC3_EN    : 1;    /* bit[10]    */
            __IO uint32_t GPIO_IN11_PTC4_EN    : 1;    /* bit[11]    */
            __IO uint32_t LPTMR_EN             : 1;    /* bit[12]    */
            __IO uint32_t Reserver_15_13       : 3;    /* bit[15:13] */
            
            __IO uint32_t GPIO_IN0_PTA3_POL     : 1;   /* bit[16]    */
            __IO uint32_t GPIO_IN1_PTA4_POL     : 1;   /* bit[17]    */
            __IO uint32_t GPIO_IN2_PTA13_POL    : 1;   /* bit[18]    */
            __IO uint32_t GPIO_IN3_PTA14_POL    : 1;   /* bit[19]    */
            __IO uint32_t GPIO_IN4_PTB2_POL     : 1;   /* bit[20]    */
            __IO uint32_t GPIO_IN5_PTB3_POL     : 1;   /* bit[21]    */
            __IO uint32_t GPIO_IN6_PTB4_POL     : 1;   /* bit[22]    */
            __IO uint32_t GPIO_IN7_PTB5_POL     : 1;   /* bit[23]    */
            __IO uint32_t GPIO_IN8_PTB12_POL    : 1;   /* bit[24]    */
            __IO uint32_t GPIO_IN9_PTB13_POL    : 1;   /* bit[25]    */
            __IO uint32_t GPIO_IN10_PTC3_POL    : 1;   /* bit[26]    */
            __IO uint32_t GPIO_IN11_PTC4_POL    : 1;   /* bit[27]    */
            __IO uint32_t Reserver_31_28        : 4;   /* bit[31:28] */
        };
        __IO uint32_t w;
    } WAKECR; 

    /* 0x8: LPTMRTH */
    __IO uint32_t LPTMRTH;
    /* 0xC: Reserved */
    __I uint32_t Reserved_10C;


    // /* 0x110--0x3EC: Reserved */
    // __I uint32_t Reserved_110_3EC[184];


    // /* 0x3F0: Reserved */
    // __I uint32_t Reserved_3F0;
    // /* 0x3F4: Reserved */
    // __I uint32_t Reserved_3F4;
    // /* 0x3F8: Reserved */
    // __I uint32_t VERID;
    // /* 0x3FC: Reserved */
    // __I uint32_t Reserved_3FC;

} PMU_TypeDef;


#endif
