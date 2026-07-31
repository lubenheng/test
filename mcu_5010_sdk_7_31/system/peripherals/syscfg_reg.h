#ifndef SYSCFG_REG_H
#define SYSCFG_REG_H


typedef struct
{
    /* 0x00: UUID0 */
    __I uint32_t UUID0;
    /* 0x04: UUID1 */
    __I uint32_t UUID1;
    /* 0x08: UUID2 */
    __I uint32_t UUID2;
    /* 0x0C: UUID3 */
    __I uint32_t UUID3;


    /* 0x10: USER0 */
    __I uint32_t USER0;
    /* 0x14: USER1 */
    __I uint32_t USER1;
    /* 0x18: USER2 */
    __I uint32_t USER2;
    /* 0x1C: USER3 */
    __I uint32_t USER3;


    /* 0x20: TAG */
    __I uint32_t TAG;
    /* 0x24: FLASH_SIZE */
    __I uint32_t VENDOR_FLASH_SIZE;
    /* 0x28: SRAM_SIZE */
    __I uint32_t VENDOR_SRAM_SIZE;

    /* 0x2C: GPIO0 */
		//__I uint32_t GPIO0;
    union {
        struct {
            __I uint32_t GPIOA_ENABLE      : 16;  /* bit[15:0]   */
            __I uint32_t GPIOB_ENABLE      : 16;  /* bit[31:16]  */
        };
        __I uint32_t w;
    } GPIO0; 

    
    /* 0x30: Reserved */
    //__I uint32_t GPIO1;
    union {
        struct {
            __I uint32_t GPIOC_ENABLE      : 16;  /* bit[15:0]   */
            __I uint32_t DISCODE           : 16;  /* bit[31:16]  */
        };
        __IO uint32_t w;
    } GPIO1; 

    /* 0x34: Reserved */
    __I uint32_t Reserved_34;
    /* 0x38: Reserved */
    __I uint32_t Reserved_38;
    /* 0x3C: Reserved */
    __I uint32_t Reserved_3C;

    // /* 0x40: Reserved */
    // __I uint32_t Reserved_40;    
    // /* 0x44: Reserved */
    // __I uint32_t Reserved_44;

    // /* 0x48--0xFC: Reserved */
    // __I uint32_t Reserved_48_FC[46];


    // /* 0x100: WAKEIR */
    // //__IO uint32_t WAKEIR;
    // union {
    //     struct {
    //         __IO uint32_t LPTMR_IE               : 1;    /* bit[0]     */
    //         __IO uint32_t GPIO_IE                : 1;    /* bit[1]     */
    //         __IO uint32_t RETN_IE                : 1;    /* bit[2]     */
    //         __IO uint32_t Reserver_15_3          : 13;   /* bit[15:3]  */
    //         __IO uint32_t LPTMR_WAKE             : 1;    /* bit[16]    */
    //         __IO uint32_t GPIO_WAKE              : 1;    /* bit[17]    */
    //         __IO uint32_t RETN_WAKE              : 1;    /* bit[18]    */
    //         __IO uint32_t Reserver_31_19         : 13;   /* bit[31:19] */
    //     };
    //     __IO uint32_t w;
    // } WAKEIR; 

    // /* 0x104: WAKECR */
    // //__IO uint32_t WAKECR;
    // union {
    //     struct {
    //         __IO uint32_t GPIO_IN0_PTA3_EN     : 1;    /* bit[0]     */
    //         __IO uint32_t GPIO_IN1_PTA4_EN     : 1;    /* bit[1]     */
    //         __IO uint32_t GPIO_IN2_PTA13_EN    : 1;    /* bit[2]     */
    //         __IO uint32_t GPIO_IN3_PTA14_EN    : 1;    /* bit[3]     */
    //         __IO uint32_t GPIO_IN4_PTB2_EN     : 1;    /* bit[4]     */
    //         __IO uint32_t GPIO_IN5_PTB3_EN     : 1;    /* bit[5]     */
    //         __IO uint32_t GPIO_IN6_PTB4_EN     : 1;    /* bit[6]     */
    //         __IO uint32_t GPIO_IN7_PTB5_EN     : 1;    /* bit[7]     */
    //         __IO uint32_t GPIO_IN8_PTB12_EN    : 1;    /* bit[8]     */
    //         __IO uint32_t GPIO_IN9_PTB13_EN    : 1;    /* bit[9]     */
    //         __IO uint32_t GPIO_IN10_PTC3_EN    : 1;    /* bit[10]    */
    //         __IO uint32_t GPIO_IN11_PTC4_EN    : 1;    /* bit[11]    */
    //         __IO uint32_t GPIO_LPTMR_EN        : 1;    /* bit[12]    */
    //         __IO uint32_t Reserver_15_13       : 3;    /* bit[15:13] */
            
    //         __IO uint32_t GPIO_IN0_PTA3_POL     : 1;   /* bit[16]    */
    //         __IO uint32_t GPIO_IN1_PTA4_POL     : 1;   /* bit[17]    */
    //         __IO uint32_t GPIO_IN2_PTA13_POL    : 1;   /* bit[18]    */
    //         __IO uint32_t GPIO_IN3_PTA14_POL    : 1;   /* bit[19]    */
    //         __IO uint32_t GPIO_IN4_PTB2_POL     : 1;   /* bit[20]    */
    //         __IO uint32_t GPIO_IN5_PTB3_POL     : 1;   /* bit[21]    */
    //         __IO uint32_t GPIO_IN6_PTB4_POL     : 1;   /* bit[22]    */
    //         __IO uint32_t GPIO_IN7_PTB5_POL     : 1;   /* bit[23]    */
    //         __IO uint32_t GPIO_IN8_PTB12_POL    : 1;   /* bit[24]    */
    //         __IO uint32_t GPIO_IN9_PTB13_POL    : 1;   /* bit[25]    */
    //         __IO uint32_t GPIO_IN10_PTC3_POL    : 1;   /* bit[26]    */
    //         __IO uint32_t GPIO_IN11_PTC4_POL    : 1;   /* bit[27]    */
    //         __IO uint32_t Reserver_31_28        : 4;   /* bit[31:28] */
    //     };
    //     __IO uint32_t w;
    // } WAKECR; 

    // /* 0x108: LPTMRTH */
    // __IO uint32_t LPTMRTH;
    // /* 0x10C: Reserved */
    // __I uint32_t Reserved_10C;


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

} SYSCFG_TypeDef;

#endif /* SYSCFG_REG_H */

