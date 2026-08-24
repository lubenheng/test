#ifndef FLASH_REG_H
#define FLASH_REG_H


#define  FLASH_MAIN_KEY1                    ((uint32_t)0x45670123)  /* bit [31:0] */
#define  FLASH_MAIN_KEY2                    ((uint32_t)0xCDEF89AB)  /* bit [31:0] */

#define  FLASH_INFO_USER_KEY1               ((uint32_t)0x08192A3B)  /* bit [31:0] */
#define  FLASH_INFO_USER_KEY2               ((uint32_t)0x4C5D6E7F)  /* bit [31:0] */

#define  FLASH_INFO_VENDOR_KEY1             ((uint32_t)0xA1B2C3D4)  /* bit [31:0] */
#define  FLASH_INFO_VENDOR_KEY2             ((uint32_t)0x5D6C7B8A)  /* bit [31:0] */

#define  FLASH_OTP_KEY1                    ((uint32_t)0xA1B2C3D4)  /* bit [31:0] */
#define  FLASH_OTP_KEY2                    ((uint32_t)0x5D6C7B8A)  /* bit [31:0] */

/** CTRL offset: 0x0000: MTP ctrl */		
#define FLASH_CTRL_KEY                      ((uint32_t)0x464c4f<<8U)
#define FLASH_OP_MAIN_R                     ((uint32_t)0x00U<<0U)
#define FLASH_OP_INFO_R                     ((uint32_t)0x01U<<0U)
#define FLASH_OP_INFO_W                     ((uint32_t)0x02U<<0U)
#define FLASH_OP_MAIN_W                     ((uint32_t)0x03U<<0U)
#define FLASH_OP_INFO_ERASE                 ((uint32_t)0x04U<<0U)
#define FLASH_OP_MAIN_ERASE                 ((uint32_t)0x05U<<0U)
#define FLASH_OP_CHIP_ERASE                 ((uint32_t)0x06U<<0U)

#define FLASH_WR_ERASE_UNLOCK               ((uint32_t)0x01<<8U)
#define FLASH_WR_ERASE_LOCK                 ((uint32_t)0x00<<8U)

/* R/W addr register*/
#define FLASH_INFO_ADDR_MASK            0x40000000U

/* R/W shadow address choice*/
#define FLASH_SHADOW_ADDR_MASK      0x80000000UL


#define FLASH_CTRL_MODE_MASK                    ((uint32_t)0x0000000f)/* bit [3:0] */
#define FLASH_CTRL_MODE_POS                     ((uint32_t)0x00)/* bit [3:0] */
#define FLASH_CTRL_MODE_SET(x)                  (((uint32_t)(((uint32_t)(x))<<FLASH_CTRL_MODE_POS))&FLASH_CTRL_MODE_MASK)

#define FLASH_CTRL_REFRESH_MASK                 ((uint32_t)0x00000010)/* bit 4 */
#define FLASH_CTRL_REFRESH_POS                  ((uint32_t)0x04)/* bit 4 */
#define FLASH_CTRL_REFRESH_SET(x)               (((uint32_t)(((uint32_t)(x))<<FLASH_CTRL_REFRESH_POS))&FLASH_CTRL_REFRESH_MASK)

#define FLASH_CTRL_RESERVED_6_5_MASK            ((uint32_t)0x00000060)/* bit [6:5] */
#define FLASH_CTRL_RESERVED_6_5_POS             ((uint32_t)0x05)/* bit [6:5] */
#define FLASH_CTRL_RESERVED_6_5_SET(x)          (((uint32_t)(((uint32_t)(x))<<FLASH_CTRL_RESERVED_6_5_POS))&FLASH_CTRL_RESERVED_6_5_MASK)

#define FLASH_CTRL_BSYN_MASK                    ((uint32_t)0x00000080)/* bit 7 */
#define FLASH_CTRL_BSYN_POS                     ((uint32_t)0x07)/* bit 7 */
#define FLASH_CTRL_BSYN_SET(x)                  (((uint32_t)(((uint32_t)(x))<<FLASH_CTRL_BSYN_POS))&FLASH_CTRL_BSYN_MASK)

#define FLASH_CTRL_ENCODE_MASK                  ((uint32_t)0xffffff00)/* bit [31:8] */
#define FLASH_CTRL_ENCODE_POS                   ((uint32_t)0x08)/* bit [31:8] */
#define FLASH_CTRL_ENCODE_SET(x)                (((uint32_t)(((uint32_t)(x))<<FLASH_CTRL_ENCODE_POS))&FLASH_CTRL_ENCODE_MASK)


#define FLASH_ADDR_FCADDR_L_MASK                ((uint32_t)0x00007fff)/* bit [14:0] */
#define FLASH_ADDR_FCADDR_L_POS                 ((uint32_t)0x00)/* bit [14:0] */
#define FLASH_ADDR_FCADDR_L_SET(x)              (((uint32_t)(((uint32_t)(x))<<FLASH_ADDR_FCADDR_L_POS))&FLASH_ADDR_FCADDR_L_MASK)

#define FLASH_ADDR_RESERVED_29_15_MASK          ((uint32_t)0x3fff8000)/* bit [29:15] */
#define FLASH_ADDR_RESERVED_29_15_POS           ((uint32_t)0x0f)/* bit [29:15] */
#define FLASH_ADDR_RESERVED_29_15_SET(x)        (((uint32_t)(((uint32_t)(x))<<FLASH_ADDR_RESERVED_29_15_POS))&FLASH_ADDR_RESERVED_29_15_MASK)

#define FLASH_ADDR_FCADDR_SEL_MASK              ((uint32_t)0xc0000000)/* bit [31:30] */
#define FLASH_ADDR_FCADDR_SEL_POS               ((uint32_t)0x1e)/* bit [31:30] */
#define FLASH_ADDR_FCADDR_SEL_SET(x)            (((uint32_t)(((uint32_t)(x))<<FLASH_ADDR_FCADDR_SEL_POS))&FLASH_ADDR_FCADDR_SEL_MASK)


#define FLASH_DATA0_DATA0_MASK                  ((uint32_t)0xffffffff)/* bit [31:0] */
#define FLASH_DATA0_DATA0_POS                   ((uint32_t)0x00)/* bit [31:0] */
#define FLASH_DATA0_DATA0_SET(x)                (((uint32_t)(((uint32_t)(x))<<FLASH_DATA0_DATA0_POS))&FLASH_DATA0_DATA0_MASK)


#define FLASH_DATA_ECC_MASK                                 ((uint32_t)0x0000007f)/* bit [6:0] */
#define FLASH_DATA_ECC_POS                              ((uint32_t)0x00)/* bit [6:0] */
#define FLASH_DATA_ECC_SET(x)                           (((uint32_t)(((uint32_t)(x))<<FLASH_DATA_ECC_POS))&FLASH_DATA_ECC_MASK)


#define FLASH_DATA_ECC_RESERVED_31_7_MASK       ((uint32_t)0xffffff80)/* bit [31:7] */
#define FLASH_DATA_ECC_RESERVED_31_7_POS        ((uint32_t)0x07)/* bit [31:7] */
#define FLASH_DATA_ECC_RESERVED_31_7_SET(x)     (((uint32_t)(((uint32_t)(x))<<FLASH_DATA_ECC_RESERVED_31_7_POS))&FLASH_DATA_ECC_RESERVED_31_7_MASK)


#define FLASH_PROT_RESERVED_7_0_MASK            ((uint32_t)0x000000ff)/* bit [7:0] */
#define FLASH_PROT_RESERVED_7_0_POS             ((uint32_t)0x00)/* bit [7:0] */
#define FLASH_PROT_RESERVED_7_0_SET(x)          (((uint32_t)(((uint32_t)(x))<<FLASH_PROT_RESERVED_7_0_POS))&FLASH_PROT_RESERVED_7_0_MASK)

#define FLASH_PROT_UNLOCK_MASK                  ((uint32_t)0x00000100)/* bit 8 */
#define FLASH_PROT_UNLOCK_POS                   ((uint32_t)0x08)/* bit 8 */
#define FLASH_PROT_UNLOCK_SET(x)                (((uint32_t)(((uint32_t)(x))<<FLASH_PROT_UNLOCK_POS))&FLASH_PROT_UNLOCK_MASK)

#define FLASH_PROT_RESERVED_31_9_MASK           ((uint32_t)0xfffffe00)/* bit [31:9] */
#define FLASH_PROT_RESERVED_31_9_POS            ((uint32_t)0x09)/* bit [31:9] */
#define FLASH_PROT_RESERVED_31_9_SET(x)         (((uint32_t)(((uint32_t)(x))<<FLASH_PROT_RESERVED_31_9_POS))&FLASH_PROT_RESERVED_31_9_MASK)


#define FLASH_SEL_AMODE_MASK                    ((uint32_t)0x00000003)/* bit [1:0] */
#define FLASH_SEL_AMODE_POS                     ((uint32_t)0x00)/* bit [1:0] */
#define FLASH_SEL_AMODE_SET(x)                  (((uint32_t)(((uint32_t)(x))<<FLASH_SEL_AMODE_POS))&FLASH_SEL_AMODE_MASK)

#define FLASH_SEL_FWE_MASK                      ((uint32_t)0x00000004)/* bit 2 */
#define FLASH_SEL_FWE_POS                       ((uint32_t)0x02)/* bit 2 */
#define FLASH_SEL_FWE_SET(x)                    (((uint32_t)(((uint32_t)(x))<<FLASH_SEL_FWE_POS))&FLASH_SEL_FWE_MASK)

#define FLASH_SEL_RESERVED_3_MASK               ((uint32_t)0x00000008)/* bit 3 */
#define FLASH_SEL_RESERVED_3_POS                ((uint32_t)0x03)/* bit 3 */
#define FLASH_SEL_RESERVED_3_SET(x)             (((uint32_t)(((uint32_t)(x))<<FLASH_SEL_RESERVED_3_POS))&FLASH_SEL_RESERVED_3_MASK)

#define FLASH_SEL_GDCNT_MASK                    ((uint32_t)0x000000f0)/* bit [7:4] */
#define FLASH_SEL_GDCNT_POS                     ((uint32_t)0x04)/* bit [7:4] */
#define FLASH_SEL_GDCNT_SET(x)                  (((uint32_t)(((uint32_t)(x))<<FLASH_SEL_GDCNT_POS))&FLASH_SEL_GDCNT_MASK)

#define FLASH_SEL_LWCNT_MASK                    ((uint32_t)0x00000700)/* bit [10:8] */
#define FLASH_SEL_LWCNT_POS                     ((uint32_t)0x08)/* bit [10:8] */
#define FLASH_SEL_LWCNT_SET(x)                  (((uint32_t)(((uint32_t)(x))<<FLASH_SEL_LWCNT_POS))&FLASH_SEL_LWCNT_MASK)

#define FLASH_SEL_RESERVED_20_11_MASK           ((uint32_t)0x001ff800)/* bit [20:11] */
#define FLASH_SEL_RESERVED_20_11_POS            ((uint32_t)0x0b)/* bit [20:11] */
#define FLASH_SEL_RESERVED_20_11_SET(x)         (((uint32_t)(((uint32_t)(x))<<FLASH_SEL_RESERVED_20_11_POS))&FLASH_SEL_RESERVED_20_11_MASK)

#define FLASH_SEL_DPD_MASK                      ((uint32_t)0x00200000)/* bit 21 */
#define FLASH_SEL_DPD_POS                       ((uint32_t)0x15)/* bit 21 */
#define FLASH_SEL_DPD_SET(x)                    (((uint32_t)(((uint32_t)(x))<<FLASH_SEL_DPD_POS))&FLASH_SEL_DPD_MASK)

#define FLASH_SEL_DOZE_MASK                     ((uint32_t)0x00400000)/* bit 22 */
#define FLASH_SEL_DOZE_POS                      ((uint32_t)0x16)/* bit 22 */
#define FLASH_SEL_DOZE_SET(x)                   (((uint32_t)(((uint32_t)(x))<<FLASH_SEL_DOZE_POS))&FLASH_SEL_DOZE_MASK)

#define FLASH_SEL_ECC_MASK                      ((uint32_t)0x00800000)/* bit 23 */
#define FLASH_SEL_ECC_POS                       ((uint32_t)0x17)/* bit 23 */
#define FLASH_SEL_ECC_SET(x)                    (((uint32_t)(((uint32_t)(x))<<FLASH_SEL_ECC_POS))&FLASH_SEL_ECC_MASK)

#define FLASH_SEL_C1US_MASK                     ((uint32_t)0xff000000)/* bit [31:24] */
#define FLASH_SEL_C1US_POS                      ((uint32_t)0x18)/* bit [31:24] */
#define FLASH_SEL_C1US_SET(x)                   (((uint32_t)(((uint32_t)(x))<<FLASH_SEL_C1US_POS))&FLASH_SEL_C1US_MASK)


#define FLASH_SR_ECC1E_MASK                     ((uint32_t)0x00000001)/* bit 0 */
#define FLASH_SR_ECC1E_POS                      ((uint32_t)0x00)/* bit 0 */
#define FLASH_SR_ECC1E_SET(x)                   (((uint32_t)(((uint32_t)(x))<<FLASH_SR_ECC1E_POS))&FLASH_SR_ECC1E_MASK)

#define FLASH_SR_ECC2E_MASK                     ((uint32_t)0x00000002)/* bit 1 */
#define FLASH_SR_ECC2E_POS                      ((uint32_t)0x01)/* bit 1 */
#define FLASH_SR_ECC2E_SET(x)                   (((uint32_t)(((uint32_t)(x))<<FLASH_SR_ECC2E_POS))&FLASH_SR_ECC2E_MASK)

#define FLASH_SR_COMP_MASK                      ((uint32_t)0x00000004)/* bit 2 */
#define FLASH_SR_COMP_POS                       ((uint32_t)0x02)/* bit 2 */
#define FLASH_SR_COMP_SET(x)                    (((uint32_t)(((uint32_t)(x))<<FLASH_SR_COMP_POS))&FLASH_SR_COMP_MASK)

#define FLASH_SR_RESERVED_4_3_MASK              ((uint32_t)0x00000018)/* bit [4:3] */
#define FLASH_SR_RESERVED_4_3_POS               ((uint32_t)0x03)/* bit [4:3] */
#define FLASH_SR_RESERVED_4_3_SET(x)            (((uint32_t)(((uint32_t)(x))<<FLASH_SR_RESERVED_4_3_POS))&FLASH_SR_RESERVED_4_3_MASK)

#define FLASH_SR_TPEG_MASK                      ((uint32_t)0x00000020)/* bit 5 */
#define FLASH_SR_TPEG_POS                       ((uint32_t)0x05)/* bit 5 */
#define FLASH_SR_TPEG_SET(x)                    (((uint32_t)(((uint32_t)(x))<<FLASH_SR_TPEG_POS))&FLASH_SR_TPEG_MASK)

#define FLASH_SR_RESERVED_31_6_MASK             ((uint32_t)0xffffffc0)/* bit [31:6] */
#define FLASH_SR_RESERVED_31_6_POS              ((uint32_t)0x06)/* bit [31:6] */
#define FLASH_SR_RESERVED_31_6_SET(x)           (((uint32_t)(((uint32_t)(x))<<FLASH_SR_RESERVED_31_6_POS))&FLASH_SR_RESERVED_31_6_MASK)


#define FLASH_INTEN_ENECC1E_MASK                ((uint32_t)0x00000001)/* bit 0 */
#define FLASH_INTEN_ENECC1E_POS                 ((uint32_t)0x00)/* bit 0 */
#define FLASH_INTEN_ENECC1E_SET(x)              (((uint32_t)(((uint32_t)(x))<<FLASH_INTEN_ENECC1E_POS))&FLASH_INTEN_ENECC1E_MASK)

#define FLASH_INTEN_ENECC2E_MASK                ((uint32_t)0x00000002)/* bit 1 */
#define FLASH_INTEN_ENECC2E_POS                 ((uint32_t)0x01)/* bit 1 */
#define FLASH_INTEN_ENECC2E_SET(x)              (((uint32_t)(((uint32_t)(x))<<FLASH_INTEN_ENECC2E_POS))&FLASH_INTEN_ENECC2E_MASK)

#define FLASH_INTEN_ENCOMP_MASK                 ((uint32_t)0x00000004)/* bit 2 */
#define FLASH_INTEN_ENCOMP_POS                  ((uint32_t)0x02)/* bit 2 */
#define FLASH_INTEN_ENCOMP_SET(x)               (((uint32_t)(((uint32_t)(x))<<FLASH_INTEN_ENCOMP_POS))&FLASH_INTEN_ENCOMP_MASK)

#define FLASH_INTEN_RESERVED_31_3_MASK          ((uint32_t)0xfffffff8)/* bit [31:3] */
#define FLASH_INTEN_RESERVED_31_3_POS           ((uint32_t)0x03)/* bit [31:3] */
#define FLASH_INTEN_RESERVED_31_3_SET(x)        (((uint32_t)(((uint32_t)(x))<<FLASH_INTEN_RESERVED_31_3_POS))&FLASH_INTEN_RESERVED_31_3_MASK)


/* HALL Register Structure */
typedef struct
{
    /* 0x00: Version ID Register(read only) */
    union {
        struct {
            __IO uint32_t MINVN      : 8;  /* bit[7:0]   */
            __IO uint32_t MAJVN      : 8;  /* bit[15:8]  */
            __IO uint32_t FEATURE    : 16; /* bit[31:16] */
        };
        __IO uint32_t w;
    } VERID;

    /* 0x04: ACR Register */
    union {
        struct {
            __IO uint32_t NWS               : 3;  /* bit[2:0]     */
            __IO uint32_t RESERVED_3_31     : 29; /* bit[31:3]    */
        };
        __IO uint32_t w;
    }ACR;

    /* 0x08: KEYR Register(write only) */
	__IO uint32_t KEYR;					    /* bit[31:0]  */

    /* 0x0C: OPTKEYR Register(write only) */
	__IO uint32_t OPTKEYR;					/* bit[31:0]  */


    /* 0x10: SR Register */
    union {
        struct {
            __IO uint32_t PEGRR             : 1;  /* bit[0]      */
            __IO uint32_t RESERVED_1_7      : 7;  /* bit[7:1]    */
            __IO uint32_t OPTPGERR          : 1;  /* bit[8]      */
            __IO uint32_t RESERVED_9_15     : 7;  /* bit[15:9]   */
            __IO uint32_t EOP               : 1;  /* bit[16]     */
            __IO uint32_t RESERVED_17_30    : 14; /* bit[30:16]  */
            __IO uint32_t BSY               : 1;  /* bit[31]     */
        };
        __IO uint32_t w;
    }SR;

    /* 0x14: IER Register */
    union {
        struct {
            __IO uint32_t PEGRRIE           : 1;  /* bit[0]      */
            __IO uint32_t RESERVED_1_7      : 7;  /* bit[7:1]    */
            __IO uint32_t OPTPGERRIE        : 1;  /* bit[8]      */
            __IO uint32_t RESERVED_9_15     : 7;  /* bit[15:9]   */
            __IO uint32_t EOPIE             : 1;  /* bit[16]     */
            __IO uint32_t RESERVED_17_31    : 15; /* bit[31:17]  */
        };
        __IO uint32_t w;
    }IER;

    /* 0x18: Control Register */
    union {
        struct {
            __IO uint32_t PG                : 1;  /* bit[0]      */
            __IO uint32_t RESERVED_1_7      : 7;  /* bit[7:1]    */
            __IO uint32_t OPTPG             : 1;  /* bit[8]      */
            __IO uint32_t RESERVED_9_15     : 7;  /* bit[15:9]   */
            __IO uint32_t LOCK              : 1;  /* bit[16]     */
            __IO uint32_t RESERVED_17_23    : 7;  /* bit[23:17]  */
            __IO uint32_t OPTLOCK           : 1;  /* bit[24]     */
            __IO uint32_t RESERVED_25_31    : 7;  /* bit[31:25]  */
        };
        __IO uint32_t w;
    }CR;

    __IO uint32_t RESERVED_1C;  /* 0x1C: Reserved */
    __IO uint32_t RESERVED_20;  /* 0x20: Reserved */

    /* 0x24: destination Register(read only) */
    __IO uint32_t DR;					/* bit[31:0]  */

    __IO uint32_t RESERVED_28;  /* 0x28: Reserved */
    __IO uint32_t RESERVED_2C;  /* 0x2C: Reserved */
    __IO uint32_t RESERVED_30;  /* 0x30: Reserved */
    __IO uint32_t RESERVED_34;  /* 0x34: Reserved */
    __IO uint32_t RESERVED_38;  /* 0x38: Reserved */
    __IO uint32_t RESERVED_3C;  /* 0x3C: Reserved */

    /* 0x40: OTP destination Register */
    union {
        struct {
            __IO uint32_t OTPPG                : 1;   /* bit[0]       */
            __IO uint32_t RESERVED_1_15        : 15;  /* bit[15:1]    */
            __IO uint32_t OTPLOCK              : 1;   /* bit[16]      */
            __IO uint32_t RESERVED_16_31       : 15;  /* bit[31:17]   */
        };
        __IO uint32_t w;
    }OTPCR;

    /* 0x44: OTP address Register */
    union {
        struct {
            __IO uint32_t OTP_ADDRESS                : 7;   /* bit[6:0]       */
            __IO uint32_t RESERVED_7_31        : 25;  /* bit[31:7]   */
        };
        __IO uint32_t w;
    }OTPAR;

    /* 0x48: OTP data Register */
    __IO uint32_t OTP_DATA_REG;					/* bit[31:0]  */

    /* 0x4C: OTP key Register(write only) */
    __IO uint32_t OTPKEY;					/* bit[31:0]  */

} FLASH_TypeDef;

#endif
