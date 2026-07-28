#ifndef I2C_REG_H
#define I2C_REG_H



typedef struct
{
    /* 0x00: I2C Control Register */
    union {
        struct {
            __IO uint32_t MASTER_MODE                : 1;  /* bit[0]     Master Mode */
            __IO uint32_t SPEED                      : 2;  /* bit[2:1]   Speed */
            __IO uint32_t IC_10BITADDR_SLAVE         : 1;  /* bit[3]     10-bit Addressing in Slave Mode */
            __IO uint32_t IC_10BITADDR_MASTER        : 1;  /* bit[4]     10-bit Addressing in Master Mode */
            __IO uint32_t IC_RESTART_EN              : 1;  /* bit[5]     Restart Enable */
            __IO uint32_t IC_SLAVE_DISABLE           : 1;  /* bit[6]     Slave Disable */
            __IO uint32_t STOP_DET_IFADDRESSED       : 1;  /* bit[7]     Stop Detection if Addressed */
            __IO uint32_t TX_EMPTY_CTRL              : 1;  /* bit[8]     TX Empty Control */
            __IO uint32_t RX_FIFO_FULL_HLD_CTRL      : 1;  /* bit[9]     RX FIFO Full Hold Control */
            __IO uint32_t RESERVED_31_10             : 22; /* bit[31:10] Reserved */
        };
        __IO uint32_t w;
    } IC_CON;

    /* 0x04: I2C Target Address Register */
    union {
        struct {
            __IO uint32_t IC_TAR                     : 10; /* bit[9:0]   Target Address */
            __IO uint32_t GC_OR_START                : 1;  /* bit[10]    General Call or Start Byte */
            __IO uint32_t SPECIAL                    : 1;  /* bit[11]    Special Command */
            __IO uint32_t IC_10BITADDR_MASTER_DYN    : 1;  /* bit[12]    10-bit Addressing Master (Dynamic) */
            __IO uint32_t RESERVED_31_13             : 19; /* bit[31:13] Reserved */
        };
        __IO uint32_t w;
    } IC_TAR;

    /* 0x08: I2C Slave Address Register */
    union {
        struct {
            __IO uint32_t IC_SAR                     : 10; /* bit[9:0]   Slave Address */
            __IO uint32_t RESERVED_31_10             : 22; /* bit[31:10] Reserved */
        };
        __IO uint32_t w;
    } IC_SAR;

    /* 0x0C: I2C High-Speed Master Mode Code Address Register */
    union {
        struct {
            __IO uint32_t IC_HS_MAR                  : 3;  /* bit[2:0]   High-Speed Master Code */
            __IO uint32_t RESERVED_31_3              : 29; /* bit[31:3]  Reserved */
        };
        __IO uint32_t w;
    } IC_HS_MADDR;

    /* 0x10: I2C Data Buffer and Command Register */
    union {
        struct {
            __IO uint32_t DAT                        : 8;  /* bit[7:0]   Data */
            __IO uint32_t CMD                        : 1;  /* bit[8]     Command (Read/Write) */
            __IO uint32_t STOP                       : 1;  /* bit[9]     Stop Condition */
            __IO uint32_t RESTART                    : 1;  /* bit[10]    Restart Condition */
            __IO uint32_t RESERVED_31_11             : 21; /* bit[31:11] Reserved */
        };
        __IO uint32_t w;
    } IC_DATA_CMD;

    /* 0x14: Standard Speed SCL High Count Register */
    __IO uint32_t IC_SS_SCL_HCNT;

    /* 0x18: Standard Speed SCL Low Count Register */
    __IO uint32_t IC_SS_SCL_LCNT;

    /* 0x1C: Fast Speed SCL High Count Register */
    __IO uint32_t IC_FS_SCL_HCNT;

    /* 0x20: Fast Speed SCL Low Count Register */
    __IO uint32_t IC_FS_SCL_LCNT;

    /* 0x24: High-Speed SCL High Count Register */
    __IO uint32_t IC_HS_SCL_HCNT;

    /* 0x28: High-Speed SCL Low Count Register */
    __IO uint32_t IC_HS_SCL_LCNT;

    /* 0x2C: Interrupt Status Register */
    union {
        struct {
            __IO uint32_t R_RX_UNDER                 : 1;  /* bit[0]     RX Underflow */
            __IO uint32_t R_RX_OVER                  : 1;  /* bit[1]     RX Overflow */
            __IO uint32_t R_RX_FULL                  : 1;  /* bit[2]     RX Full */
            __IO uint32_t R_TX_OVER                  : 1;  /* bit[3]     TX Overflow */
            __IO uint32_t R_TX_EMPTY                 : 1;  /* bit[4]     TX Empty */
            __IO uint32_t R_RD_REQ                   : 1;  /* bit[5]     Read Request */
            __IO uint32_t R_TX_ABRT                  : 1;  /* bit[6]     TX Abort */
            __IO uint32_t R_RX_DONE                  : 1;  /* bit[7]     RX Done */
            __IO uint32_t R_ACTIVITY                 : 1;  /* bit[8]     Activity */
            __IO uint32_t R_STOP_DET                 : 1;  /* bit[9]     Stop Detection */
            __IO uint32_t R_START_DET                : 1;  /* bit[10]    Start Detection */
            __IO uint32_t R_GEN_CALL                 : 1;  /* bit[11]    General Call */
            __IO uint32_t R_RESTART_DET              : 1;  /* bit[12]    Restart Detection */
            __IO uint32_t R_MST_ON_HOLD              : 1;  /* bit[13]    Master on Hold */
            __IO uint32_t RESERVED_31_14             : 18; /* bit[31:14] Reserved */
        };
        __IO uint32_t w;
    } IC_INTR_STAT;

    /* 0x30: Interrupt Mask Register */
    union {
        struct {
            __IO uint32_t M_RX_UNDER                 : 1;  /* bit[0]     RX Underflow Mask */
            __IO uint32_t M_RX_OVER                  : 1;  /* bit[1]     RX Overflow Mask */
            __IO uint32_t M_RX_FULL                  : 1;  /* bit[2]     RX Full Mask */
            __IO uint32_t M_TX_OVER                  : 1;  /* bit[3]     TX Overflow Mask */
            __IO uint32_t M_TX_EMPTY                 : 1;  /* bit[4]     TX Empty Mask */
            __IO uint32_t M_RD_REQ                   : 1;  /* bit[5]     Read Request Mask */
            __IO uint32_t M_TX_ABRT                  : 1;  /* bit[6]     TX Abort Mask */
            __IO uint32_t M_RX_DONE                  : 1;  /* bit[7]     RX Done Mask */
            __IO uint32_t M_ACTIVITY                 : 1;  /* bit[8]     Activity Mask */
            __IO uint32_t M_STOP_DET                 : 1;  /* bit[9]     Stop Detection Mask */
            __IO uint32_t M_START_DET                : 1;  /* bit[10]    Start Detection Mask */
            __IO uint32_t M_GEN_CALL                 : 1;  /* bit[11]    General Call Mask */
            __IO uint32_t M_RESTART_DET              : 1;  /* bit[12]    Restart Detection Mask */
            __IO uint32_t M_MST_ON_HOLD              : 1;  /* bit[13]    Master on Hold Mask */
            __IO uint32_t RESERVED_31_14             : 18; /* bit[31:14] Reserved */
        };
        __IO uint32_t w;
    } IC_INTR_MASK;

    /* 0x34: Raw Interrupt Status Register */
    union {
        struct {
            __IO uint32_t RX_UNDER                   : 1;  /* bit[0]     RX Underflow Raw */
            __IO uint32_t RX_OVER                    : 1;  /* bit[1]     RX Overflow Raw */
            __IO uint32_t RX_FULL                    : 1;  /* bit[2]     RX Full Raw */
            __IO uint32_t TX_OVER                    : 1;  /* bit[3]     TX Overflow Raw */
            __IO uint32_t TX_EMPTY                   : 1;  /* bit[4]     TX Empty Raw */
            __IO uint32_t RD_REQ                     : 1;  /* bit[5]     Read Request Raw */
            __IO uint32_t TX_ABRT                    : 1;  /* bit[6]     TX Abort Raw */
            __IO uint32_t RX_DONE                    : 1;  /* bit[7]     RX Done Raw */
            __IO uint32_t ACTIVITY                   : 1;  /* bit[8]     Activity Raw */
            __IO uint32_t STOP_DET                   : 1;  /* bit[9]     Stop Detection Raw */
            __IO uint32_t START_DET                  : 1;  /* bit[10]    Start Detection Raw */
            __IO uint32_t GEN_CALL                   : 1;  /* bit[11]    General Call Raw */
            __IO uint32_t RESTART_DET                : 1;  /* bit[12]    Restart Detection Raw */
            __IO uint32_t MST_ON_HOLD                : 1;  /* bit[13]    Master on Hold Raw */
            __IO uint32_t RESERVED_31_14             : 18; /* bit[31:14] Reserved */
        };
        __IO uint32_t w;
    } IC_RAW_INTR_STAT;

    /* 0x38: Receive FIFO Threshold Level Register */
    union {
        struct {
            __IO uint32_t RX_TL                      : 8;  /* bit[7:0]   RX Threshold Level */
            __IO uint32_t RESERVED_31_8              : 24; /* bit[31:8]  Reserved */
        };
        __IO uint32_t w;
    } IC_RX_TL;

    /* 0x3C: Transmit FIFO Threshold Level Register */
    union {
        struct {
            __IO uint32_t TX_TL                      : 8;  /* bit[7:0]   TX Threshold Level */
            __IO uint32_t RESERVED_31_8              : 24; /* bit[31:8]  Reserved */
        };
        __IO uint32_t w;
    } IC_TX_TL;

    /* 0x40: Clear Combined and Individual Interrupts Register */
    union {
        struct {
            __IO uint32_t CLR_INTR                   : 1;  /* bit[0]     Clear Interrupt */
            __IO uint32_t RESERVED_31_1              : 31; /* bit[31:1]  Reserved */
        };
        __IO uint32_t w;
    } IC_CLR_INTR;

    /* 0x44: Clear RX_UNDER Interrupt Register */
    union {
        struct {
            __IO uint32_t CLR_RX_UNDER               : 1;  /* bit[0]     Clear RX Underflow */
            __IO uint32_t RESERVED_31_1              : 31; /* bit[31:1]  Reserved */
        };
        __IO uint32_t w;
    } IC_CLR_RX_UNDER;

    /* 0x48: Clear RX_OVER Interrupt Register */
    union {
        struct {
            __IO uint32_t CLR_RX_OVER                : 1;  /* bit[0]     Clear RX Overflow */
            __IO uint32_t RESERVED_31_1              : 31; /* bit[31:1]  Reserved */
        };
        __IO uint32_t w;
    } IC_CLR_RX_OVER;

    /* 0x4C: Clear TX_OVER Interrupt Register */
    union {
        struct {
            __IO uint32_t CLR_TX_OVER                : 1;  /* bit[0]     Clear TX Overflow */
            __IO uint32_t RESERVED_31_1              : 31; /* bit[31:1]  Reserved */
        };
        __IO uint32_t w;
    } IC_CLR_TX_OVER;

    /* 0x50: Clear RD_REQ Interrupt Register */
    union {
        struct {
            __IO uint32_t CLR_RD_REQ                 : 1;  /* bit[0]     Clear Read Request */
            __IO uint32_t RESERVED_31_1              : 31; /* bit[31:1]  Reserved */
        };
        __IO uint32_t w;
    } IC_CLR_RD_REQ;

    /* 0x54: Clear TX_ABRT Interrupt Register */
    union {
        struct {
            __IO uint32_t CLR_TX_ABRT                : 1;  /* bit[0]     Clear TX Abort */
            __IO uint32_t RESERVED_31_1              : 31; /* bit[31:1]  Reserved */
        };
        __IO uint32_t w;
    } IC_CLR_TX_ABRT;

    /* 0x58: Clear RX_DONE Interrupt Register */
    union {
        struct {
            __IO uint32_t CLR_RX_DONE                : 1;  /* bit[0]     Clear RX Done */
            __IO uint32_t RESERVED_31_1              : 31; /* bit[31:1]  Reserved */
        };
        __IO uint32_t w;
    } IC_CLR_RX_DONE;

    /* 0x5C: Clear ACTIVITY Interrupt Register */
    union {
        struct {
            __IO uint32_t CLR_ACTIVITY               : 1;  /* bit[0]     Clear Activity */
            __IO uint32_t RESERVED_31_1              : 31; /* bit[31:1]  Reserved */
        };
        __IO uint32_t w;
    } IC_CLR_ACTIVITY;

    /* 0x60: Clear STOP_DET Interrupt Register */
    union {
        struct {
            __IO uint32_t CLR_STOP_DET               : 1;  /* bit[0]     Clear Stop Detection */
            __IO uint32_t RESERVED_31_1              : 31; /* bit[31:1]  Reserved */
        };
        __IO uint32_t w;
    } IC_CLR_STOP_DET;

    /* 0x64: Clear START_DET Interrupt Register */
    union {
        struct {
            __IO uint32_t CLR_START_DET              : 1;  /* bit[0]     Clear Start Detection */
            __IO uint32_t RESERVED_31_1              : 31; /* bit[31:1]  Reserved */
        };
        __IO uint32_t w;
    } IC_CLR_START_DET;

    /* 0x68: Clear GEN_CALL Interrupt Register */
    union {
        struct {
            __IO uint32_t CLR_GEN_CALL               : 1;  /* bit[0]     Clear General Call */
            __IO uint32_t RESERVED_31_1              : 31; /* bit[31:1]  Reserved */
        };
        __IO uint32_t w;
    } IC_CLR_GEN_CALL;

    /* 0x6C: I2C Enable Register */
    union {
        struct {
            __IO uint32_t ENABLE                     : 1;  /* bit[0]     Enable */
            __IO uint32_t ABORT                      : 1;  /* bit[1]     Abort */
            __IO uint32_t RESERVED_31_2              : 30; /* bit[31:2]  Reserved */
        };
        __IO uint32_t w;
    } IC_ENABLE;

    /* 0x70: I2C Status Register */
    union {
        struct {
            __IO uint32_t ACTIVITY                   : 1;  /* bit[0]     Activity Status */
            __IO uint32_t TFNF                       : 1;  /* bit[1]     TX FIFO Not Full */
            __IO uint32_t TFE                        : 1;  /* bit[2]     TX FIFO Empty */
            __IO uint32_t RFNE                       : 1;  /* bit[3]     RX FIFO Not Empty */
            __IO uint32_t RFF                        : 1;  /* bit[4]     RX FIFO Full */
            __IO uint32_t MST_ACTIVITY               : 1;  /* bit[5]     Master Activity */
            __IO uint32_t SLV_ACTIVITY               : 1;  /* bit[6]     Slave Activity */
            __IO uint32_t RESERVED_31_7              : 25; /* bit[31:7]  Reserved */
        };
        __IO uint32_t w;
    } IC_STATUS;

    /* 0x74: Transmit FIFO Level Register */
    union {
        struct {
            __IO uint32_t TXFLR                      : 3;  /* bit[2:0]   TX FIFO Level */
            __IO uint32_t RESERVED_31_3              : 29; /* bit[31:3]  Reserved */
        };
        __IO uint32_t w;
    } IC_TXFLR;

    /* 0x78: Receive FIFO Level Register */
    union {
        struct {
            __IO uint32_t RXFLR                      : 3;  /* bit[2:0]   RX FIFO Level */
            __IO uint32_t RESERVED_31_3              : 29; /* bit[31:3]  Reserved */
        };
        __IO uint32_t w;
    } IC_RXFLR;

    /* 0x7C: SDA Hold Time Length Register */
    union {
        struct {
            __IO uint32_t IC_SDA_TX_HOLD             : 16; /* bit[15:0]  SDA TX Hold Time */
            __IO uint32_t IC_SDA_RX_HOLD             : 8;  /* bit[23:16] SDA RX Hold Time */
            __IO uint32_t RESERVED_31_24             : 8;  /* bit[31:24] Reserved */
        };
        __IO uint32_t w;
    } IC_SDA_HOLD;

    /* 0x80: Transmit Abort Source Register */
    union {
        struct {
            __IO uint32_t ABRT_7B_ADDR_NOACK         : 1;  /* bit[0]     7-bit Address No ACK */
            __IO uint32_t ABRT_10ADDR1_NOACK         : 1;  /* bit[1]     10-bit Address1 No ACK */
            __IO uint32_t ABRT_10ADDR2_NOACK         : 1;  /* bit[2]     10-bit Address2 No ACK */
            __IO uint32_t ABRT_TXDATA_NOACK          : 1;  /* bit[3]     TX Data No ACK */
            __IO uint32_t ABRT_GCALL_NOACK           : 1;  /* bit[4]     General Call No ACK */
            __IO uint32_t ABRT_GCALL_READ            : 1;  /* bit[5]     General Call Read */
            __IO uint32_t ABRT_HS_ACKDET             : 1;  /* bit[6]     HS ACK Detect */
            __IO uint32_t ABRT_SBYTE_ACKDET          : 1;  /* bit[7]     Start Byte ACK Detect */
            __IO uint32_t ABRT_HS_NORSTRT            : 1;  /* bit[8]     HS No Restart */
            __IO uint32_t ABRT_SBYTE_NORSTRT         : 1;  /* bit[9]     Start Byte No Restart */
            __IO uint32_t ABRT_10B_RD_NORSTRT        : 1;  /* bit[10]    10-bit Read No Restart */
            __IO uint32_t ABRT_MASTER_DIS            : 1;  /* bit[11]    Master Disabled */
            __IO uint32_t ARB_LOST                  : 1;  /* bit[12]    Arbitration Lost */
            __IO uint32_t ABRT_SLVFLUSH_TXFIFO       : 1;  /* bit[13]    Slave Flush TX FIFO */
            __IO uint32_t ABRT_SLV_ARBLOST          : 1;  /* bit[14]    Slave Arbitration Lost */
            __IO uint32_t ABRT_SLVRD_INTX           : 1;  /* bit[15]    Slave Read in TX */
            __IO uint32_t ABRT_USER_ABRT            : 1;  /* bit[16]    User Abort */
            __IO uint32_t TX_FLUSH_CNT              : 8;  /* bit[24:17] TX Flush Count */
            __IO uint32_t RESERVED_31_25            : 7;  /* bit[31:25] Reserved */
        };
        __IO uint32_t w;
    } IC_TX_ABRT_SOURCE;

    /* 0x84: Generate Slave Data NACK Register */
    union {
        struct {
            __IO uint32_t NACK                       : 1;  /* bit[0]     Generate NACK */
            __IO uint32_t RESERVED_31_1              : 31; /* bit[31:1]  Reserved */
        };
        __IO uint32_t w;
    } IC_SLV_DATA_NACK_ONLY;

    /* 0x88: DMA Control Register */
    union {
        struct {
            __IO uint32_t RDMAE                      : 1;  /* bit[0]     RX DMA Enable */
            __IO uint32_t TDMAE                      : 1;  /* bit[1]     TX DMA Enable */
            __IO uint32_t RESERVED_31_2              : 30; /* bit[31:2]  Reserved */
        };
        __IO uint32_t w;
    } IC_DMA_CR;

    /* 0x8C: DMA Transmit Data Level Register */
    union {
        struct {
            __IO uint32_t DMATDL                      : 8;  /* bit[7:0]   DMA TX Data Level */
            __IO uint32_t RESERVED_31_8              : 24; /* bit[31:8]  Reserved */
        };
        __IO uint32_t w;
    } IC_DMA_TDLR;

    /* 0x90: DMA Receive Data Level Register */
    union {
        struct {
            __IO uint32_t DMARDL                      : 8;  /* bit[7:0]   DMA RX Data Level */
            __IO uint32_t RESERVED_31_8              : 24; /* bit[31:8]  Reserved */
        };
        __IO uint32_t w;
    } IC_DMA_RDLR;

    /* 0x94: SDA Setup Register */
    union {
        struct {
            __IO uint32_t SDA_SETUP                  : 8;  /* bit[7:0]   SDA Setup Time */
            __IO uint32_t RESERVED_31_8              : 24; /* bit[31:8]  Reserved */
        };
        __IO uint32_t w;
    } IC_SDA_SETUP;

    /* 0x98: ACK General Call Register */
    union {
        struct {
            __IO uint32_t ACK_GEN_CALL               : 1;  /* bit[0]     ACK General Call */
            __IO uint32_t RESERVED_31_1              : 31; /* bit[31:1]  Reserved */
        };
        __IO uint32_t w;
    } IC_ACK_GENERAL_CALL;

    /* 0x9C: Enable Status Register */
    union {
        struct {
            __IO uint32_t IC_EN                      : 1;  /* bit[0]     I2C Enable Status */
            __IO uint32_t SLV_DISABLED_WHILE_BUSY    : 1;  /* bit[1]     Slave Disabled While Busy */
            __IO uint32_t SLV_RX_DATA_LOST           : 1;  /* bit[2]     Slave RX Data Lost */
            __IO uint32_t RESERVED_31_3              : 29; /* bit[31:3]  Reserved */
        };
        __IO uint32_t w;
    } IC_ENABLE_STATUS;

    /* 0xA0: FS Spike Suppression Limit Register */
    union {
        struct {
            __IO uint32_t IC_FS_SPKLEN               : 8;  /* bit[7:0]   FS Spike Length */
            __IO uint32_t RESERVED_31_8              : 24; /* bit[31:8]  Reserved */
        };
        __IO uint32_t w;
    } IC_FS_SPKLEN;

    /* 0xA4: HS Spike Suppression Limit Register */
    union {
        struct {
            __IO uint32_t IC_HS_SPKLEN               : 8;  /* bit[7:0]   HS Spike Length */
            __IO uint32_t RESERVED_31_8              : 24; /* bit[31:8]  Reserved */
        };
        __IO uint32_t w;
    } IC_HS_SPKLEN;

    /* 0xA8: Clear RESTART_DET Interrupt Register */
    union {
        struct {
            __IO uint32_t CLR_RESTART_DET            : 1;  /* bit[0]     Clear Restart Detection */
            __IO uint32_t RESERVED_31_1              : 31; /* bit[31:1]  Reserved */
        };
        __IO uint32_t w;
    } IC_CLR_RESTART_DET;

 /* 0xAC: Reserved */
    __IO uint32_t RESERVED_AC;
	
	/* 0xB0: Reserved */
	__IO uint32_t RESERVED_B0;
	
	/* 0xB4: Reserved */
	__IO uint32_t RESERVED_B4;
	
	/* 0xB8 Reserved */
	__IO uint32_t RESERVED_B8;
	
	/* 0xBC: Reserved */
	__IO uint32_t RESERVED_BC;
	
	/* 0xC0: Reserved */
	__IO uint32_t RESERVED_C0;
	
	/* 0xC4: Reserved */
	__IO uint32_t RESERVED_C4;
	
	/* 0xC8: Reserved */
	__IO uint32_t RESERVED_C8;
	
	/* 0xCC: Reserved */
	__IO uint32_t RESERVED_CC;
	
	/* 0xD0: Reserved */
	__IO uint32_t RESERVED_D0;
	
	/* 0xD4: Reserved */
	__IO uint32_t RESERVED_D4;
	
	/* 0xD8 Reserved */
	__IO uint32_t RESERVED_D8;
	
	/* 0xDC: Reserved */
	__IO uint32_t RESERVED_DC;
	
	/* 0xE0: Reserved */
	__IO uint32_t RESERVED_E0;
	
	/* 0xE4: Reserved */
	__IO uint32_t RESERVED_E4;
	
	/* 0xE8: Reserved */
	__IO uint32_t RESERVED_E8;
	
	/* 0xEC: Reserved */
	__IO uint32_t RESERVED_EC;

    /* 0xF0: Component Parameter Register 1 */
    union {
        struct {
            __IO uint32_t APB_DATA_WIDTH             : 2;  /* bit[1:0]   APB Data Width */
            __IO uint32_t MAX_SPEED_MODE             : 2;  /* bit[3:2]   Max Speed Mode */
            __IO uint32_t HC_COUNT_VALUES            : 1;  /* bit[4]     Hard-coded Count Values */
            __IO uint32_t INTR_IO                    : 1;  /* bit[5]     Interrupt IO */
            __IO uint32_t HAS_DMA                   : 1;  /* bit[6]     Has DMA */
            __IO uint32_t ADD_ENCODED_PARAMS        : 1;  /* bit[7]     Add Encoded Parameters */
            __IO uint32_t RX_BUFFER_DEPTH           : 8;  /* bit[15:8]  RX Buffer Depth */
            __IO uint32_t TX_BUFFER_DEPTH           : 8;  /* bit[23:16] TX Buffer Depth */
            __IO uint32_t RESERVED_31_24            : 8;  /* bit[31:24] Reserved */
        };
        __IO uint32_t w;
    } IC_COMP_PARAM_1;

    /* 0xF4: Component Version Register */
    union {
        struct {
            __IO uint32_t VERSION                   : 32; /* bit[31:0]  Component Version */
        };
        __IO uint32_t w;
    } IC_COMP_VERSION;

    /* 0xF8: Reserved */
    __IO uint32_t RESERVED2;

    /* 0xFC: Component Type Register */
    union {
        struct {
            __IO uint32_t COMP_TYPE                 : 32; /* bit[31:0]  Component Type */
        };
        __IO uint32_t w;
    } IC_COMP_TYPE;

} I2C_TypeDef;



#endif
