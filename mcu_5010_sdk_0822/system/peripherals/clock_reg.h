#ifndef CLOCK_REG_H
#define CLOCK_REG_H

/* SYSRSTE 写保护密钥 (bit[31:24])，写入功能位时必须与功能位同步写入以解锁本寄存器。
 * TODO: 具体密钥值需按 datasheet/寄存器手册确认，此处为占位值。 */
#define RCC_SYSRSTE_KEYCODE   0xAB56

/* RCC (Reset and Clock Control) Register Structure */
typedef struct
{
    /* 0x00: Version ID Register */
    union {
        struct {
            __IO uint32_t MINVN      : 8;  /* bit[7:0]   小版本号 (Minor Version Number), ro */
            __IO uint32_t MAJVN      : 8;  /* bit[15:8]  主版本号 (Major Version Number), ro */
            __IO uint32_t FEATURE    : 16; /* bit[31:16] 特性标识位，标识 IP 支持的功能集合, ro */
        };
        __IO uint32_t w;
    } VERID;

    /* 0x04: System Reset Status Register */
    union {
        struct {
            __IO uint32_t PORBF             : 1;  /* bit[0]     上电复位标志 (Power-On Reset), rw1c */
            __IO uint32_t BORBF             : 1;  /* bit[1]     欠压复位标志 (Brown-Out Reset), rw1c */
            __IO uint32_t LVDRSTF           : 1;  /* bit[2]     低电压检测复位标志 (LVD Reset), rw1c */
            __IO uint32_t RESERVED_3_7      : 5;  /* bit[7:3]   保留 */
            __IO uint32_t PINRSTF           : 1;  /* bit[8]     外部引脚复位标志 (NRST Pin Reset), rw1c */
            __IO uint32_t SYSRSTREQF        : 1;  /* bit[9]     软件系统复位请求标志, rw1c */
            __IO uint32_t LOCKUPF           : 1;  /* bit[10]    CPU Core Lockup 复位标志, rw1c */
            __IO uint32_t RESERVED_11_15    : 5;  /* bit[15:11] 保留 */
            __IO uint32_t IWDTF             : 1;  /* bit[16]    独立看门狗复位标志 (IWDT Reset), rw1c */
            __IO uint32_t RESERVED_17_20    : 4;  /* bit[20:17] 保留 */
            __IO uint32_t HSEFAILRSTF       : 1;  /* bit[21]    HSE 时钟失效复位标志, rw1c */
            __IO uint32_t PLLULKRSTF        : 1;  /* bit[22]    PLL 失锁复位标志, rw1c */
            __IO uint32_t RESERVED_23       : 1;  /* bit[23]    保留 */
            __IO uint32_t RETRIMF           : 1;  /* bit[24]    Re-trim 复位标志, rw1c */
            __IO uint32_t RESERVED_25_31    : 7;  /* bit[31:25] 保留 */
        };
        __IO uint32_t w;
    } SYSRSTSR;

    /* 0x08: System Reset Enable Register */
    union {
        struct {
            __IO uint32_t RESERVED_0_1   : 2;  /* bit[1:0]   保留 */
            __IO uint32_t LVDRSTE        : 1;  /* bit[2]     LVD 复位使能, rw */
            __IO uint32_t RESERVED_3_9   : 7;  /* bit[9:3]   保留 */
            __IO uint32_t LOCKUPE        : 1;  /* bit[10]    CPU Core Lockup 复位使能, rw */
            __IO uint32_t RESERVED_11_15 : 5;  /* bit[15:11] 保留 */
            __IO uint32_t IWDTE          : 1;  /* bit[16]    独立看门狗复位使能, rw，默认 1 */
            __IO uint32_t RESERVED_17_20 : 4;  /* bit[20:17] 保留 */
            __IO uint32_t HSEFAILRSTE    : 1;  /* bit[21]    HSE 失效复位使能, rw */
            __IO uint32_t PLLULKRSTE     : 1;  /* bit[22]    PLL 失锁复位使能, rw */
            __IO uint32_t RESERVED_23    : 1;  /* bit[23]    保留 */
            __IO uint32_t KEYCODE        : 8;  /* bit[31:24] 写保护密钥 (只写)，与功能位同步写入以解锁本寄存器 */
        };
        __IO uint32_t w;
    } SYSRSTE;

    /* 0x0C: RSTKEY */
    //__IO uint32_t RESERVED_0C;
    union {
        struct {
            __IO uint32_t RETRIM            : 16;  /* bit[15:0]  写入 0xAB56 触发芯片 boot 复位并重新执行校准 (reboot), wo */
            __IO uint32_t RESERVED_16_31    : 16;  /* bit[31:16] 保留 */
        };
        __IO uint32_t w;
    } RSTKEY;

    /* 0x10: AHB Reset Register */
    union {
        struct {
            __IO uint32_t FLC               : 1;  /* bit[0]     Flash 控制器复位, wo */
            __IO uint32_t RESERVED_1_15     : 15; /* bit[15:1]  保留 */
            __IO uint32_t GPIOARST          : 1;  /* bit[16]    GPIO PA 模块复位，1=复位，0=复位释放, wo */
            __IO uint32_t GPIOBRST          : 1;  /* bit[17]    GPIO PB 模块复位，1=复位，0=复位释放, wo */
            __IO uint32_t GPIOCRST          : 1;  /* bit[18]    GPIO PC 模块复位，1=复位，0=复位释放, wo */
            __IO uint32_t RESERVED_19_31    : 13; /* bit[31:19] 保留 */
        };
        __IO uint32_t w;
    } AHBRSSTR;

    /* 0x14: Analog Reset Register */
    union {
        struct {
            __IO uint32_t ADC_RST       : 1;  /* bit[0]     ADC 模块复位，1=复位，0=复位释放, wo */
            __IO uint32_t RESERVED_1_31 : 31; /* bit[31:1]  保留 */
        };
        __IO uint32_t w;
    } ANARST;

    /* 0x18: Digital Reset Register */
    union {
        struct {
            __IO uint32_t MATHRST        : 1;  /* bit[0]     MATH 模块复位, wo */
            __IO uint32_t HALLRST        : 1;  /* bit[1]     HALL 模块复位控制位, wo */
            __IO uint32_t QEPRST         : 1;  /* bit[2]     QEP 模块复位控制位, wo */
            __IO uint32_t RESERVED_3_15  : 13; /* bit[15:3]  保留 */
            __IO uint32_t IWDTRST        : 1;  /* bit[16]    IWDT 模块复位，1=复位，0=复位释放, wo */
            __IO uint32_t RESERVED_17_31 : 15; /* bit[31:17] 保留 */
        };
        __IO uint32_t w;
    } DIGRST;

    /* 0x1C: Communication Reset Register */
    union {
        struct {
            __IO uint32_t RESERVED_0_7      : 8;  /* bit[7:0]   保留 */
            __IO uint32_t UART0RST          : 1;  /* bit[8]     UART0 模块复位，1=复位，0=复位释放, wo */
            __IO uint32_t UART1RST          : 1;  /* bit[9]     UART1 模块复位，1=复位，0=复位释放, wo */
            __IO uint32_t RESERVED_10_15    : 6;  /* bit[15:10] 保留 */
            __IO uint32_t SPI0RST           : 1;  /* bit[16]    SPI0 模块复位，1=复位，0=复位释放, wo */
            __IO uint32_t SPI1RST           : 1;  /* bit[17]    SPI1 模块复位，1=复位，0=复位释放, wo */
            __IO uint32_t RESERVED_18_23    : 6;  /* bit[23:18] 保留 */
            __IO uint32_t IIC0RST           : 1;  /* bit[24]    IIC0 模块复位，1=复位，0=复位释放, wo */
            __IO uint32_t IIC1RST           : 1;  /* bit[25]    IIC1 模块复位，1=复位，0=复位释放, wo */
            __IO uint32_t RESERVED_26_31    : 6;  /* bit[31:26] 保留 */
        };
        __IO uint32_t w;
    } COMRST;

    /* 0x20: Timer Reset Register */
    union {
        struct {
            __IO uint32_t RESERVED_0_12     : 13; /* bit[12:0]  保留 */
            __IO uint32_t TIM13RST          : 1;  /* bit[13]    TIM13 模块复位控制位，1=复位，0=复位释放, wo */
            __IO uint32_t TIM14RST          : 1;  /* bit[14]    TIM14 模块复位控制位，1=复位，0=复位释放, wo */
            __IO uint32_t TIM15RST          : 1;  /* bit[15]    TIM15 模块复位控制位，1=复位，0=复位释放, wo */
            __IO uint32_t TIM16RST          : 1;  /* bit[16]    TIM16 模块复位控制位，1=复位，0=复位释放, wo */
            __IO uint32_t RESERVED_17_23    : 7;  /* bit[23:17] 保留 */
            __IO uint32_t MCPWM0RST         : 1;  /* bit[24]    MCPWM 模块复位控制位，1=复位，0=复位释放, wo */
            __IO uint32_t RESERVED_25_31    : 7;  /* bit[31:25] 保留 */
        };
        __IO uint32_t w;
    } TIMRST;

    __IO uint32_t RESERVED_24;  /* 0x24: Reserved */
    __IO uint32_t RESERVED_28;  /* 0x28: Reserved */
    __IO uint32_t RESERVED_2C;  /* 0x2C: Reserved */

    /* 0x30: HSI Control Register */
    union {
        struct {
            __IO uint32_t HSIEN         : 1;  /* bit[0]     内部 32MHz RC 时钟使能，1=使能，0=关闭, rw */
            __IO uint32_t RESERVED_1_30 : 30; /* bit[30:1]  保留 */
            __IO uint32_t HSIRDY        : 1;  /* bit[31]    内部 32MHz RC 时钟就绪标志，1=已稳定, ro */
        };
        __IO uint32_t w;
    } HSICR;

    /* 0x34: LSI Control Register */
    union {
        struct {
            __IO uint32_t LSIEN          : 1;  /* bit[0]     内部 32KHz RC 时钟使能，1=使能，0=关闭, rw */
            __IO uint32_t RESERVED_1_30  : 30; /* bit[30:1]  保留 */
            __IO uint32_t LSIRDY         : 1;  /* bit[31]    内部 32KHz RC 时钟就绪标志，1=已稳定, ro */
        };
        __IO uint32_t w;
    } LSICR_STR;

    /* 0x38: HSE Control Register */
    union {
        struct {
            __IO uint32_t HSEEN         : 1;  /* bit[0]     HSE 晶振使能，1=使能，0=关闭, rw */
            __IO uint32_t HSEXSE        : 1;  /* bit[1]     HSE 旁路使能，1=外部时钟输入模式, rw */
            __IO uint32_t RESERVED_2_3  : 2;  /* bit[3:2]   保留 */
            __IO uint32_t HSEDETE       : 1;  /* bit[4]     HSE 故障检测使能, rw */
            __IO uint32_t RESERVED_5_30 : 26; /* bit[30:5]  保留 */
            __IO uint32_t HSERDY        : 1;  /* bit[31]    HSE 就绪标志，1=晶振已稳定, ro */
        };
        __IO uint32_t w;
    } HSECR;

    /* 0x3C: LSE Control Register */
    union {
        struct {
            __IO uint32_t LSEEN         : 1;  /* bit[0]     LSE 晶振使能，1=使能，0=关闭, rw */
            __IO uint32_t LSEXSE        : 1;  /* bit[1]     LSE 旁路使能，1=外部时钟输入模式, rw */
            __IO uint32_t RESERVED_2_30 : 29; /* bit[30:2]  保留 */
            __IO uint32_t LSERDY        : 1;  /* bit[31]    LSE 就绪标志，1=晶振已稳定, ro */
        };
        __IO uint32_t w;
    } LSECR;

    /* 0x40: PLL Control Register 0 */
    union {
        struct {
            __IO uint32_t PLLEN             : 1;  /* bit[0]     PLL 使能，1=使能，0=关闭, rw */
            __IO uint32_t RSV_1_14          : 14; /* bit[14:1]  保留 */
            __IO uint32_t PLLRDY            : 1;  /* bit[15]    PLL 已使能就绪标志, ro */
            __IO uint32_t RO_PLL_ERR_LAT    : 1;  /* bit[16]    PLL 错误锁存标志, ro */
            __IO uint32_t RO_PLL_RDY        : 1;  /* bit[17]    PLL 锁定就绪标志, ro */
            __IO uint32_t RSV_18_31         : 14; /* bit[31:18] 保留 */
        };
        __IO uint32_t w;
    } PLLCR0;

    /* 0x44: PLL Control Register 1 */
    union {
        struct {
            __IO uint32_t PLLDIV         : 3;  /* bit[2:0]   PLL 输出分频系数, rw */
            __IO uint32_t RESERVED_3_7   : 5;  /* bit[7:3]   保留 */
            __IO uint32_t PLLMULT        : 5;  /* bit[12:8]  5位倍频系数, rw */
            __IO uint32_t RESERVED_13_31 : 19; /* bit[31:13] 保留 */
        };
        __IO uint32_t w;
    } PLLCR1;

    __IO uint32_t RESERVED_48;  /* 0x48: Reserved */
    __IO uint32_t RESERVED_4C;  /* 0x4C: Reserved */


    /* 0x50: System Clock Control Register */
    union {
        struct {
            __IO uint32_t SYSCKSW        : 2;  /* bit[1:0]   系统时钟源选择，00=内部32MHz, 01=外部晶振, 10/11=内部PLL, rw */
            __IO uint32_t RESERVED_2_15  : 14; /* bit[15:2]  保留 */
            __IO uint32_t HPRE           : 4;  /* bit[19:16] PCLK分频系数，PCLK=FCLK/(PPRE+1), rw */
            __IO uint32_t RESERVED_20_23 : 4;  /* bit[23:20] 保留 */
            __IO uint32_t PPRE           : 4;  /* bit[27:24] FCLK分频系数，FCLK=SYSCLK/(HPRE+1), rw */
            __IO uint32_t RESERVED_28_31 : 4;  /* bit[31:28] 保留 */
        };
        __IO uint32_t w;
    } SYSCKCR;

    /* 0x54: Microcontroller Clock Output Control Register */
    union {
        struct {
            __IO uint32_t MCOSRC         : 4;  /* bit[3:0]   MCO 时钟源选择：000=系统时钟, 010=内部32MHz, 011=内部PLL, 100=外部晶振, 101=内部32KHz, rw */
            __IO uint32_t RESERVED_4_7   : 4;  /* bit[7:4]   保留 */
            __IO uint32_t MCOPRE         : 4;  /* bit[11:8]  MCO 分频系数，输出频率 = 源频率 / 2^MCOPRE, rw */
            __IO uint32_t RESERVED_12_31 : 20; /* bit[31:12] 保留 */
        };
        __IO uint32_t w;
    } MCOCR_STR;

    /* 0x58: AHB Clock Enable Register */
    union {
        struct {
            __IO uint32_t FLC_CLK_EN         : 1;  /* bit[0]     Flash 控制器时钟使能, rw */
            __IO uint32_t RESERVED_1_15      : 15; /* bit[15:1]  保留 */
            __IO uint32_t GPIOA_CLK_EN       : 1;  /* bit[16]    GPIO PA 时钟使能, rw */
            __IO uint32_t GPIOB_CLK_EN       : 1;  /* bit[17]    GPIO PB 时钟使能, rw */
            __IO uint32_t GPIOC_CLK_EN       : 1;  /* bit[18]    GPIO PC 时钟使能, rw */
            __IO uint32_t RESERVED_19_31     : 13; /* bit[31:19] 保留 */
        };
        __IO uint32_t w;
    } AHB_CLK_EN;

    /* 0x5C: Analog Clock Enable Register */
    union {
        struct {
            __IO uint32_t ADC0_CLK_EN       : 1;  /* bit[0]     ADC0 时钟使能，1=使能，0=关闭, rw */
            __IO uint32_t RESERVED_1_31     : 31; /* bit[31:1]  保留 */
        };
        __IO uint32_t w;
    } ANA_CLK_EN;

    /* 0x60: Digital Clock Enable Register */
    union {
        struct {
            __IO uint32_t MATH_CLK_EN        : 1;  /* bit[0]     MATH 模块时钟使能，1=使能，0=关闭, rw */
            __IO uint32_t HALL_CLK_EN        : 1;  /* bit[1]     HALL 模块时钟使能，1=使能，0=关闭, rw */
            __IO uint32_t QEP_CLK_EN         : 1;  /* bit[2]     QEP 模块时钟使能，1=使能，0=关闭, rw */
            __IO uint32_t RESERVED_3_15      : 13; /* bit[15:3]  保留 */
            __IO uint32_t IWDT_CLK_EN        : 1;  /* bit[16]    IWDT 时钟使能，1=使能，0=关闭, rw */
            __IO uint32_t RESERVED_17_31     : 15; /* bit[31:17] 保留 */
        };
        __IO uint32_t w;
    } DIG_CLK_EN;

    /* 0x64: Communication Clock Enable Register */
    union {
        struct {
            __IO uint32_t RESERVED_0_7       : 8;  /* bit[7:0]   保留 */
            __IO uint32_t UART0_CLK_EN       : 1;  /* bit[8]     UART0 时钟使能, rw */
            __IO uint32_t UART1_CLK_EN       : 1;  /* bit[9]     UART1 时钟使能, rw */
            __IO uint32_t RESERVED_10_15     : 6;  /* bit[15:10] 保留 */
            __IO uint32_t SPI0_CLK_EN        : 1;  /* bit[16]    SPI0 时钟使能, rw */
            __IO uint32_t SPI1_CLK_EN        : 1;  /* bit[17]    SPI1 时钟使能, rw */
            __IO uint32_t RESERVED_18_23     : 6;  /* bit[23:18] 保留 */
            __IO uint32_t IIC0_CLK_EN        : 1;  /* bit[24]    IIC0 时钟使能, rw */
            __IO uint32_t IIC1_CLK_EN        : 1;  /* bit[25]    IIC1 时钟使能, rw */
            __IO uint32_t RESERVED_26_31     : 6;  /* bit[31:26] 保留 */
        };
        __IO uint32_t w;
    } COM_CLK_EN;

    /* 0x68: Timer Clock Enable Register  */
    union {
        struct {
            __IO uint32_t RESERVED_0_12      : 13; /* bit[12:0]  保留 */
            __IO uint32_t TIM13_CLK_EN       : 1;  /* bit[13]    TIM13 时钟使能, rw */
            __IO uint32_t TIM14_CLK_EN       : 1;  /* bit[14]    TIM14 时钟使能, rw */
            __IO uint32_t TIM15_CLK_EN       : 1;  /* bit[15]    TIM15 时钟使能, rw */
            __IO uint32_t TIM16_CLK_EN       : 1;  /* bit[16]    TIM16 时钟使能, rw */
            __IO uint32_t RESERVED_17_23     : 7;  /* bit[23:17] 保留 */
            __IO uint32_t MCPWM0_CLK_EN      : 1;  /* bit[24]    MCPWM0 时钟使能, rw */
            __IO uint32_t RESERVED_25_31     : 7;  /* bit[31:25] 保留 */
        };
        __IO uint32_t w;
    } TIM_CLK_EN;

    __IO uint32_t RESERVED_6C;  /* 保留寄存器 (offset: 0x06C) */

    /* PCKS0 - 外设时钟源选择寄存器 0 (offset: 0x070, default: 0x00000000) */
    union
    {
        struct{
            __IO uint32_t ADC0CKS           : 2;  /* bit[1:0]   ADC 时钟源：01=HSI CLK, 10=EOSC CLK, 11=PLL CLK */
            __IO uint32_t RESERVED_2_3      : 2;  /* bit[3:2]   保留 */
            __IO uint32_t MCPWM0CKS         : 2;  /* bit[5:4]   MCPWM 时钟源：01=HSI CLK, 10=EOSC CLK, 11=PLL CLK */
            __IO uint32_t RESERVED_6_7      : 2;  /* bit[7:6]   保留 */
            __IO uint32_t CAN0CKS           : 2;  /* bit[9:8]   CAN 时钟源：01=HSI CLK, 10=EOSC CLK, 11=PLL CLK */
            __IO uint32_t RESERVED_10_31    : 22; /* bit[31:10] 保留 */
        };
        __IO uint32_t Word;
    }PCKS0;


} RCC_TypeDef;


#endif
