#ifndef SPI_REG_H
#define SPI_REG_H

/**
 * AndeShape ATCSPI200 SPI Controller Register Definitions
 * Compatible with ARM Cortex-M series
 * Based on ATCSPI200 Data Sheet DS087-16 V1.6
 * Format aligned with DesignWare APB SPI register header
 */


//#define SPI0_BASE_ADDR        0x40003800
//#define SPI1_BASE_ADDR        0x40013800

/* ==============================================
   ATCSPI200 Register Structure Definition
   ============================================== */
typedef struct
{
    /* 0x00: ID and Revision Register */
    union {
        struct {
            __IO uint32_t REVMINOR        : 4;  /* bit[3:0]   Minor revision number */
            __IO uint32_t REVMAJOR        : 8;  /* bit[11:4]  Major revision number */
            __IO uint32_t ID              : 20; /* bit[31:12] ID number for ATCSPI200 */
        };
        __IO uint32_t W;
    } IDREV;

    /* 0x04 ~ 0x0C: Reserved */
    __IO uint32_t RESERVED0[3];

    /* 0x10: SPI Transfer Format Register */
    union {
        struct {
            __IO uint32_t CPHA            : 1;  /* bit[0]     SPI Clock Phase */
            __IO uint32_t CPOL            : 1;  /* bit[1]     SPI Clock Polarity */
            __IO uint32_t SLVMOD         : 1;  /* bit[2]     SPI Master/Slave mode selection */
            __IO uint32_t LSB             : 1;  /* bit[3]     LSB first transfer enable */
            __IO uint32_t MOSIBIDIR       : 1;  /* bit[4]     Bi-directional MOSI enable */
            __IO uint32_t RESERVED_6_5    : 2;  /* bit[6:5]   Reserved */
            __IO uint32_t DATAMERGE       : 1;  /* bit[7]     Data Merge mode enable */
            __IO uint32_t DATALEN         : 5;  /* bit[12:8]  Data unit length (bits) */
            __IO uint32_t RESERVED_15_13  : 3;  /* bit[15:13] Reserved */
            __IO uint32_t ADDRLEN         : 2;  /* bit[17:16] Address length (bytes) */
            __IO uint32_t RESERVED_31_18  : 14; /* bit[31:18] Reserved */
        };
        __IO uint32_t W;
    } TRANSFMT;

    /* 0x14: SPI Direct IO Control Register */
    union {
        struct {
            __IO uint32_t CS_I            : 1;  /* bit[0]     SPI CS input status */
            __IO uint32_t SCLK_I          : 1;  /* bit[1]     SPI SCLK input status */
            __IO uint32_t MOSI_I          : 1;  /* bit[2]     SPI MOSI input status */
            __IO uint32_t MISO_I          : 1;  /* bit[3]     SPI MISO input status */
            __IO uint32_t WP_I            : 1;  /* bit[4]     SPI WP input status */
            __IO uint32_t HOLD_I          : 1;  /* bit[5]     SPI HOLD input status */
            __IO uint32_t RESERVED_7_6    : 2;  /* bit[7:6]   Reserved */
            __IO uint32_t CS_O            : 1;  /* bit[8]     SPI CS output value */
            __IO uint32_t SCLK_O          : 1;  /* bit[9]     SPI SCLK output value */
            __IO uint32_t MOSI_O          : 1;  /* bit[10]    SPI MOSI output value */
            __IO uint32_t MISO_O          : 1;  /* bit[11]    SPI MISO output value */
            __IO uint32_t WP_O            : 1;  /* bit[12]    SPI WP output value */
            __IO uint32_t HOLD_O          : 1;  /* bit[13]    SPI HOLD output value */
            __IO uint32_t RESERVED_15_14  : 2;  /* bit[15:14] Reserved */
            __IO uint32_t CS_OE           : 1;  /* bit[16]    SPI CS output enable */
            __IO uint32_t SCLK_OE         : 1;  /* bit[17]    SPI SCLK output enable */
            __IO uint32_t MOSI_OE         : 1;  /* bit[18]    SPI MOSI output enable */
            __IO uint32_t MISO_OE         : 1;  /* bit[19]    SPI MISO output enable */
            __IO uint32_t WP_OE           : 1;  /* bit[20]    SPI WP output enable */
            __IO uint32_t HOLD_OE         : 1;  /* bit[21]    SPI HOLD output enable */
            __IO uint32_t RESERVED_23_22  : 2;  /* bit[23:22] Reserved */
            __IO uint32_t DIRECTIOEN      : 1;  /* bit[24]    Direct IO mode enable */
            __IO uint32_t RESERVED_31_25  : 7;  /* bit[31:25] Reserved */
        };
        __IO uint32_t W;
    } DIRECTIO;

    /* 0x18 ~ 0x1C: Reserved */
    __IO uint32_t RESERVED2[2];

    /* 0x20: SPI Transfer Control Register */
    union {
        struct {
            __IO uint32_t RDTRANCNT       : 9;  /* bit[8:0]   Read data transfer count */
            __IO uint32_t DUMMYCNT        : 2;  /* bit[10:9]  Dummy cycle count */
            __IO uint32_t TOKENVALUE      : 1;  /* bit[11]    Special token value */
            __IO uint32_t WRTRANCNT       : 9;  /* bit[20:12] Write data transfer count */
            __IO uint32_t TOKENEN         : 1;  /* bit[21]    Special token enable */
            __IO uint32_t DUALQUAD        : 2;  /* bit[23:22] SPI data phase format */
            __IO uint32_t TRANSMODE       : 4;  /* bit[27:24] Transfer mode */
            __IO uint32_t ADDRFMT         : 1;  /* bit[28]    Address phase format */
            __IO uint32_t ADDREN          : 1;  /* bit[29]    Address phase enable */
            __IO uint32_t CMDEN           : 1;  /* bit[30]    Command phase enable */
            __IO uint32_t RESERVED_31     : 1;  /* bit[31]    Reserved */
        };
        __IO uint32_t W;
    } TRANSCTRL;

    /* 0x24: SPI Command Register */
    union {
        struct {
            __IO uint32_t CMD             : 8;  /* bit[7:0]   SPI Command */
            __IO uint32_t RESERVED_31_8   : 24; /* bit[31:8]  Reserved */
        };
        __IO uint32_t W;
    } CMD;

    /* 0x28: SPI Address Register */
    union {
        struct {
            __IO uint32_t ADDR            : 32; /* bit[31:0]  SPI Address */
        };
        __IO uint32_t W;
    } ADDR;

    /* 0x2C: SPI Data Register */
    union {
        struct {
            __IO uint32_t DATA            : 32; /* bit[31:0]  TX/RX Data */
        };
        __IO uint32_t W;
    } DATA;

    /* 0x30: SPI Control Register */
    union {
        struct {
            __IO uint32_t SPIRST          : 1;  /* bit[0]     SPI controller reset */
            __IO uint32_t RXFIFORST       : 1;  /* bit[1]     RX FIFO reset */
            __IO uint32_t TXFIFORST       : 1;  /* bit[2]     TX FIFO reset */
            __IO uint32_t RXDMAEN         : 1;  /* bit[3]     RX DMA enable */
            __IO uint32_t TXDMAEN         : 1;  /* bit[4]     TX DMA enable */
            __IO uint32_t RESERVED_7_5    : 3;  /* bit[7:5]   Reserved */
            __IO uint32_t RXTHRES         : 5;  /* bit[12:8]  RX FIFO threshold */
            __IO uint32_t RESERVED_15_13  : 3;  /* bit[15:13] Reserved */
            __IO uint32_t TXTHRES         : 5;  /* bit[20:16] TX FIFO threshold */
            __IO uint32_t RESERVED_31_21  : 11; /* bit[31:21] Reserved */
        };
        __IO uint32_t W;
    } CTRL;

    /* 0x34: SPI Status Register */
    union {
        struct {
            __IO uint32_t SPIACTIVE       : 1;  /* bit[0]     SPI transfer in progress */
            __IO uint32_t RESERVED_7_1    : 7;  /* bit[7:1]   Reserved */
            __IO uint32_t RXNUM           : 5;  /* bit[12:8]  RX FIFO valid entry count */
            __IO uint32_t RESERVED_13     : 1;  /* bit[13]    Reserved */
            __IO uint32_t RXEMPTY         : 1;  /* bit[14]    RX FIFO empty flag */
            __IO uint32_t RXFULL          : 1;  /* bit[15]    RX FIFO full flag */
            __IO uint32_t TXNUM           : 5;  /* bit[20:16] TX FIFO valid entry count */
            __IO uint32_t RESERVED_21     : 1;  /* bit[21]    Reserved */
            __IO uint32_t TXEMPTY         : 1;  /* bit[22]    TX FIFO empty flag */
            __IO uint32_t TXFULL          : 1;  /* bit[23]    TX FIFO full flag */
            __IO uint32_t RESERVED_31_24  : 8;  /* bit[31:24] Reserved */
        };
        __IO uint32_t W;
    } STATUS;

    /* 0x38: SPI Interrupt Enable Register */
    union {
        struct {
            __IO uint32_t RXFIFOORINTEN   : 1;  /* bit[0]     RX FIFO Overrun interrupt enable */
            __IO uint32_t TXFIFOURINTEN   : 1;  /* bit[1]     TX FIFO Underrun interrupt enable */
            __IO uint32_t RXFIFOINTEN     : 1;  /* bit[2]     RX FIFO threshold interrupt enable */
            __IO uint32_t TXFIFOINTEN     : 1;  /* bit[3]     TX FIFO threshold interrupt enable */
            __IO uint32_t ENDINTEN        : 1;  /* bit[4]     End of transfer interrupt enable */
            __IO uint32_t SLVCMDEN        : 1;  /* bit[5]     Slave command received interrupt enable */
            __IO uint32_t RESERVED_31_6   : 26; /* bit[31:6]  Reserved */
        };
        __IO uint32_t W;
    } INTREN;

    /* 0x3C: SPI Interrupt Status Register */
    union {
        struct {
            __IO uint32_t RXFIFOORINT     : 1;  /* bit[0]     RX FIFO Overrun interrupt status (W1C) */
            __IO uint32_t TXFIFOURINT     : 1;  /* bit[1]     TX FIFO Underrun interrupt status (W1C) */
            __IO uint32_t RXFIFOINT       : 1;  /* bit[2]     RX FIFO threshold interrupt status (W1C) */
            __IO uint32_t TXFIFOINT       : 1;  /* bit[3]     TX FIFO threshold interrupt status (W1C) */
            __IO uint32_t ENDINT          : 1;  /* bit[4]     End of transfer interrupt status (W1C) */
            __IO uint32_t SLVCMDINT       : 1;  /* bit[5]     Slave command received interrupt status (W1C) */
            __IO uint32_t RESERVED_31_6   : 26; /* bit[31:6]  Reserved */
        };
        __IO uint32_t W;
    } INTRST;

    /* 0x40: SPI Interface Timing Register */
    union {
        struct {
            __IO uint32_t SCLK_DIV        : 8;  /* bit[7:0]   SCLK clock divider */
            __IO uint32_t CSHT            : 4;  /* bit[11:8]  CS high minimum time */
            __IO uint32_t CS2SCLK         : 2;  /* bit[13:12] CS to SCLK edge time */
            __IO uint32_t RESERVED_31_14  : 18; /* bit[31:14] Reserved */
        };
        __IO uint32_t W;
    } TIMING;

    /* 0x44 ~ 0x4C: Reserved */
    __IO uint32_t RESERVED3[3];

    /* 0x50: SPI Memory Access Control Register */
    union {
        struct {
            __IO uint32_t MEMRDCMD        : 4;  /* bit[3:0]   Memory mapped read command format */
            __IO uint32_t RESERVED_7_4    : 4;  /* bit[7:4]   Reserved */
            __IO uint32_t MEMCTRLCHG      : 1;  /* bit[8]     Register change in progress flag */
            __IO uint32_t RESERVED_31_9   : 23; /* bit[31:9]  Reserved */
        };
        __IO uint32_t W;
    } MEMCTRL;

    /* 0x54 ~ 0x5C: Reserved */
    __IO uint32_t RESERVED4[3];

    /* 0x60: SPI Slave Status Register */
    union {
        struct {
            __IO uint32_t USR_STATUS      : 16; /* bit[15:0]  User defined status flags */
            __IO uint32_t READY           : 1;  /* bit[16]    Slave ready flag */
            __IO uint32_t OVERRUN         : 1;  /* bit[17]    RX FIFO overrun status (W1C) */
            __IO uint32_t UNDERRUN        : 1;  /* bit[18]    TX FIFO underrun status (W1C) */
            __IO uint32_t RESERVED_31_19  : 13; /* bit[31:19] Reserved */
        };
        __IO uint32_t W;
    } SLVST;

    /* 0x64: SPI Slave Data Count Register */
    union {
        struct {
            __IO uint32_t RCNT            : 9;  /* bit[8:0]   Slave received data count */
            __IO uint32_t RESERVED_15_9   : 7;  /* bit[15:9]  Reserved */
            __IO uint32_t WCNT            : 9;  /* bit[24:16] Slave transmitted data count */
            __IO uint32_t RESERVED_31_25  : 7;  /* bit[31:25] Reserved */
        };
        __IO uint32_t W;
    } SLVDATACNT;

    /* 0x68 ~ 0x78: Reserved */
    __IO uint32_t RESERVED5[5];

    /* 0x7C: Configuration Register */
    union {
        struct {
            __IO uint32_t RXFIFOSIZE      : 2;  /* bit[1:0]   RX FIFO depth */
            __IO uint32_t RESERVED_3_2    : 2;  /* bit[3:2]   Reserved */
            __IO uint32_t TXFIFOSIZE      : 2;  /* bit[5:4]   TX FIFO depth */
            __IO uint32_t RESERVED_7_6    : 2;  /* bit[7:6]   Reserved */
            __IO uint32_t DUALSPI         : 1;  /* bit[8]     Dual I/O SPI support */
            __IO uint32_t QUADSPI         : 1;  /* bit[9]     Quad I/O SPI support */
            __IO uint32_t RESERVED_10     : 1;  /* bit[10]    Reserved */
            __IO uint32_t DIRECTIO        : 1;  /* bit[11]    Direct IO support */
            __IO uint32_t AHBMEM          : 1;  /* bit[12]    AHB memory mapped read support */
            __IO uint32_t EILMMEM         : 1;  /* bit[13]    EILM memory mapped read support */
            __IO uint32_t SLAVE           : 1;  /* bit[14]    SPI Slave mode support */
            __IO uint32_t RESERVED_31_15  : 17; /* bit[31:15] Reserved */
        };
        __IO uint32_t W;
    } CONFIG;

} SPI_TypeDef;


#define TRANSFMT_REG        ((SPI_TypeDef *) (SPI0_BASE + 0x10UL))
#define TRANSCTRL_REG       ((SPI_TypeDef *) (SPI0_BASE + 0x20UL))
#define DATA_REG       ((SPI_TypeDef *) (SPI0_BASE + 0x2CUL))
#define CTRL_REG       ((SPI_TypeDef *) (SPI0_BASE + 0x30UL))
#define STATUS_REG       ((SPI_TypeDef *) (SPI0_BASE + 0x34UL))
#define INTREN_REG       ((SPI_TypeDef *) (SPI0_BASE + 0x38UL))
#define INTRST_REG       ((SPI_TypeDef *) (SPI0_BASE + 0x3CUL))
#define TIMING_REG       ((SPI_TypeDef *) (SPI0_BASE + 0x40UL))
#define CONFIG_REG       ((SPI_TypeDef *) (SPI0_BASE + 0x7CUL))

#endif /* ATCSPI200_REG_H */


