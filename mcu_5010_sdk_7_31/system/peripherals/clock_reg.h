#ifndef CLOCK_REG_H
#define CLOCK_REG_H

/* RCC (Reset and Clock Control) Register Structure */
typedef struct
{
    /* 0x00: Version ID Register */
    union {
        struct {
            __IO uint32_t MINVN      : 8;  /* bit[7:0]   */
            __IO uint32_t MAJVN      : 8;  /* bit[15:8]  */
            __IO uint32_t FEATURE    : 16; /* bit[31:16] */
        };
        __IO uint32_t w;
    } VERID;

    /* 0x04: System Reset Status Register */
    union {
        struct {
            __IO uint32_t PORBF          : 1;  /* bit[0]     */
            __IO uint32_t BORBF          : 1;  /* bit[1]     */
            __IO uint32_t LVDRSTF        : 1;  /* bit[2]     */
            __IO uint32_t RESERVED_3_7   : 5;  /* bit[7:3]   */
            __IO uint32_t PINRSTF        : 1;  /* bit[8]     */
            __IO uint32_t SYSRSTREQF     : 1;  /* bit[9]     */
            __IO uint32_t LOCKUPF        : 1;  /* bit[10]    */
            __IO uint32_t RESERVED_11_15 : 5;  /* bit[15:11] */
            __IO uint32_t IWDTF          : 1;  /* bit[16]    */
            __IO uint32_t WWDTF          : 1;  /* bit[17]    */
            __IO uint32_t HSEFAILRSTF    : 1;  /* bit[18]    */
            __IO uint32_t PULLULKRSTF    : 1;  /* bit[19]    */
            __IO uint32_t RESERVED_23    : 1;  /* bit[20]    */
            __IO uint32_t RETRIMF        : 1;  /* bit[21]    */
            __IO uint32_t RESERVED_25_31 : 7;  /* bit[31:22] */
        };
        __IO uint32_t w;
    } SYSRSTSR;

    /* 0x08: System Reset Enable Register */
    union {
        struct {
            __IO uint32_t RESERVED_0_1   : 2;  /* bit[1:0]   */
            __IO uint32_t LVDRSTE        : 1;  /* bit[2]     */
            __IO uint32_t RESERVED_3_9   : 7;  /* bit[9:3]   */
            __IO uint32_t LOCKUPE        : 1;  /* bit[10]    */
            __IO uint32_t RESERVED_11_15 : 5;  /* bit[15:11] */
            __IO uint32_t IWDTE          : 1;  /* bit[16]    */
            __IO uint32_t RESERVED_17_20 : 4;  /* bit[20:17] */
            __IO uint32_t HSEFAILRSTE    : 1;  /* bit[21]    */
            __IO uint32_t PLLULKRSTE     : 1;  /* bit[22]    */
            __IO uint32_t RESERVED_23    : 1;  /* bit[23]    */
            __IO uint32_t KEYCODE        : 8;  /* bit[31:24] */
        };
        __IO uint32_t w;
    } SYSRSTE;

    /* 0x0C: RSTKEY */
    //__IO uint32_t RESERVED_0C;
    union {
        struct {
            __IO uint32_t RETRIM            : 16;  /* bit[15:0]   */
            __IO uint32_t RESERVED_16_31    : 16;  /* bit[31:16]  */
        };
        __IO uint32_t w;
    } RSTKEY;

    /* 0x10: AHB Reset Register */
    union {
        struct {
            __IO uint32_t FLC               : 1;  /* bit[0]     */
            __IO uint32_t RESERVED_1_15     : 15; /* bit[15:1]  */
            __IO uint32_t GPIOARST          : 1;  /* bit[16]    */
            __IO uint32_t GPIOBRST          : 1;  /* bit[17]    */
            __IO uint32_t GPIOCRST          : 1;  /* bit[18]    */
            __IO uint32_t RESERVED_19_31    : 13; /* bit[31:19] */
        };
        __IO uint32_t w;
    } AHBRSSTR;

    /* 0x14: Analog Reset Register */
    union {
        struct {
            __IO uint32_t ADC_RST       : 1;  /* bit[0]     */
            __IO uint32_t RESERVED_1_31 : 31; /* bit[31:1]  */
        };
        __IO uint32_t w;
    } ANARST;

    /* 0x18: Digital Reset Register */
    union {
        struct {
            __IO uint32_t MATHRST        : 1;  /* bit[0]     */
            __IO uint32_t HALLRST        : 1;  /* bit[1]     */
            __IO uint32_t QEPRST         : 1;  /* bit[2]     */
            __IO uint32_t RESERVED_3_15  : 13; /* bit[15:3]  */
            __IO uint32_t IWDTRST        : 1;  /* bit[16]    */
            __IO uint32_t WWDTRST        : 1;  /* bit[17]    */
            __IO uint32_t RESERVED_18_31 : 14; /* bit[31:18] */
        };
        __IO uint32_t w;
    } DIGRST;

    /* 0x1C: Communication Reset Register */
    union {
        struct {
            __IO uint32_t RESERVED_0_7      : 8;  /* bit[7:0]   */
            __IO uint32_t UART0RST          : 1;  /* bit[8]     */
            __IO uint32_t UART1RST          : 1;  /* bit[9]     */
            __IO uint32_t RESERVED_10_15    : 6;  /* bit[15:10] */
            __IO uint32_t SPI0RST           : 1;  /* bit[16]    */
            __IO uint32_t SPI1RST           : 1;  /* bit[17]    */
            __IO uint32_t RESERVED_18_23    : 6;  /* bit[23:18] */
            __IO uint32_t IIC0RST           : 1;  /* bit[24]    */
            __IO uint32_t IIC1RST           : 1;  /* bit[25]    */
            __IO uint32_t RESERVED_26_31    : 6;  /* bit[31:26] */
        };
        __IO uint32_t w;
    } COMRST;

    /* 0x20: Timer Reset Register */
    union {
        struct {
            __IO uint32_t RESERVED_0_12     : 13; /* bit[12:0]  */
            __IO uint32_t TIM13RST          : 1;  /* bit[13]    */
            __IO uint32_t TIM14RST          : 1;  /* bit[14]    */
            __IO uint32_t TIM15RST          : 1;  /* bit[15]    */
            __IO uint32_t TIM16RST          : 1;  /* bit[16]    */
            __IO uint32_t RESERVED_17_23    : 7;  /* bit[23:17] */
            __IO uint32_t MCPWM0RST         : 1;  /* bit[24]    */
            __IO uint32_t RESERVED_25_31    : 7;  /* bit[31:25] */
        };
        __IO uint32_t w;
    } TIMRST;

    __IO uint32_t RESERVED_24;  /* 0x24: Reserved */
    __IO uint32_t RESERVED_28;  /* 0x28: Reserved */
    __IO uint32_t RESERVED_2C;  /* 0x2C: Reserved */

    /* 0x30: HSI Control Register */
    union {
        struct {
            __IO uint32_t HSIEN         : 1;  /* bit[0]     */
            __IO uint32_t RESERVED_1_30 : 30; /* bit[30:1]  */
            __IO uint32_t HSIRDY        : 1;  /* bit[31]    */
        };
        __IO uint32_t w;
    } HSICR;

    /* 0x34: LSI Control Register */
    union {
        struct {
            __IO uint32_t LSIEN          : 1;  /* bit[0]     */
            __IO uint32_t RESERVED_1_30  : 30; /* bit[30:1]  */
            __IO uint32_t LSIRDY         : 1;  /* bit[31]    */
        };
        __IO uint32_t w;
    } LSICR_STR;

    /* 0x38: HSE Control Register */
    union {
        struct {
            __IO uint32_t HSEEN         : 1;  /* bit[0]     */
            __IO uint32_t HSEXSE        : 1;  /* bit[1]     */
            __IO uint32_t RESERVED_2_3  : 2;  /* bit[3:2]   */
            __IO uint32_t HSEDETE       : 1;  /* bit[4]     */
            __IO uint32_t RESERVED_5_30 : 26; /* bit[30:5]  */
            __IO uint32_t HSERDY        : 1;  /* bit[31]    */
        };
        __IO uint32_t w;
    } HSECR;

    /* 0x3C: LSE Control Register */
    union {
        struct {
            __IO uint32_t LSEEN         : 1;  /* bit[0]     */
            __IO uint32_t LSEXSE        : 1;  /* bit[1]     */
            __IO uint32_t RESERVED_2_30 : 29; /* bit[30:2]  */
            __IO uint32_t LSERDY        : 1;  /* bit[31]    */
        };
        __IO uint32_t w;
    } LSECR;

    /* 0x40: PLL Control Register 0 */
    union {
        struct {
            __IO uint32_t PLLEN         : 1;  /* bit[0]     */
            __IO uint32_t RESERVED_1_30 : 30; /* bit[30:1]  */
            __IO uint32_t PLLRDY        : 1;  /* bit[31]    */
        };
        __IO uint32_t w;
    } PLLCR0;

    /* 0x44: PLL Control Register 1 */
    union {
        struct {
            __IO uint32_t PLLDIV         : 3;  /* bit[2:0]   */
            __IO uint32_t RESERVED_3_7   : 5;  /* bit[7:3]   */
            __IO uint32_t PLLMULT        : 5;  /* bit[12:8]  */
            __IO uint32_t RESERVED_13_31 : 19; /* bit[31:13] */
        };
        __IO uint32_t w;
    } PLLCR1;

    __IO uint32_t RESERVED_48;  /* 0x48: Reserved */
    __IO uint32_t RESERVED_4C;  /* 0x4C: Reserved */


    /* 0x50: System Clock Control Register */
    union {
        struct {
            __IO uint32_t SYSCKSW        : 2;  /* bit[1:0]   */
            __IO uint32_t RESERVED_2_15  : 14; /* bit[15:2]  */
            __IO uint32_t HPRE           : 4;  /* bit[19:16] */
            __IO uint32_t RESERVED_20_23 : 4;  /* bit[23:20] */
            __IO uint32_t PPRE           : 4;  /* bit[27:24] */
            __IO uint32_t RESERVED_28_31 : 4;  /* bit[31:28] */
        };
        __IO uint32_t w;
    } SYSCKCR;

    /* 0x54: Microcontroller Clock Output Control Register */
    union {
        struct {
            __IO uint32_t MCOSRC         : 4;  /* bit[3:0]   */
            __IO uint32_t RESERVED_4_7   : 4;  /* bit[7:4]   */
            __IO uint32_t MCOPRE         : 4;  /* bit[11:8]  */
            __IO uint32_t RESERVED_12_31 : 20; /* bit[31:12] */
        };
        __IO uint32_t w;
    } MCOCR_STR;

    /* 0x58: AHB Clock Enable Register */
    union {
        struct {
            __IO uint32_t FLC_CLK_EN         : 1;  /* bit[0]     */
            __IO uint32_t RESERVED_1_15      : 15; /* bit[15:1]  */
            __IO uint32_t GPIOA_CLK_EN       : 1;  /* bit[16]    */
            __IO uint32_t GPIOB_CLK_EN       : 1;  /* bit[17]    */
            __IO uint32_t GPIOC_CLK_EN       : 1;  /* bit[18]    */
            __IO uint32_t RESERVED_19_31     : 13; /* bit[31:19] */
        };
        __IO uint32_t w;
    } AHB_CLK_EN;

    /* 0x5C: Analog Clock Enable Register */
    union {
        struct {
            __IO uint32_t ADC0_CLK_EN       : 1;  /* bit[0]     */
            __IO uint32_t RESERVED_1_31     : 31; /* bit[31:1]  */
        };
        __IO uint32_t w;
    } ANA_CLK_EN;

    /* 0x60: Digital Clock Enable Register */
    union {
        struct {
            __IO uint32_t MATH_CLK_EN        : 1;  /* bit[0]     */
            __IO uint32_t HALL_CLK_EN        : 1;  /* bit[1]     */
            __IO uint32_t QEP_CLK_EN         : 1;  /* bit[2]     */
            __IO uint32_t RESERVED_3_15      : 13; /* bit[15:3]  */
            __IO uint32_t IWDT_CLK_EN        : 1;  /* bit[16]    */
            __IO uint32_t RESERVED_17_31     : 15; /* bit[31:17] */
        };
        __IO uint32_t w;
    } DIG_CLK_EN;

    /* 0x64: Communication Clock Enable Register */
    union {
        struct {
            __IO uint32_t RESERVED_0_7       : 8;  /* bit[7:0]   */
            __IO uint32_t UART0_CLK_EN       : 1;  /* bit[8]     */
            __IO uint32_t UART1_CLK_EN       : 1;  /* bit[9]     */
            __IO uint32_t RESERVED_10_15     : 6;  /* bit[15:10] */
            __IO uint32_t SPI0_CLK_EN        : 1;  /* bit[16]    */
            __IO uint32_t SPI1_CLK_EN        : 1;  /* bit[17]    */
            __IO uint32_t RESERVED_18_23     : 6;  /* bit[23:18] */
            __IO uint32_t IIC0_CLK_EN        : 1;  /* bit[24]    */
            __IO uint32_t IIC1_CLK_EN        : 1;  /* bit[25]    */
            __IO uint32_t RESERVED_26_31     : 6;  /* bit[31:26] */
        };
        __IO uint32_t w;
    } COM_CLK_EN;

    /* 0x68: Timer Clock Enable Register  */
    union {
        struct {
            __IO uint32_t RESERVED_0_12      : 13; /* bit[12:0]  */
            __IO uint32_t TIM13_CLK_EN       : 1;  /* bit[13]    */
            __IO uint32_t TIM14_CLK_EN       : 1;  /* bit[14]    */
            __IO uint32_t TIM15_CLK_EN       : 1;  /* bit[15]    */
            __IO uint32_t TIM16_CLK_EN       : 1;  /* bit[16]    */
            __IO uint32_t RESERVED_17_23     : 7;  /* bit[23:17] */
            __IO uint32_t MCPWM0_CLK_EN      : 1;  /* bit[24]    */
            __IO uint32_t RESERVED_25_31     : 7;  /* bit[31:25] */
        };
        __IO uint32_t w;
    } TIM_CLK_EN;

} RCC_TypeDef;


#endif
