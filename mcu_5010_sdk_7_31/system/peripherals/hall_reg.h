#ifndef HALL_REG_H
#define HALL_REG_H

#if 0
typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned MINVN      :8;
        unsigned MAJVN      :8;
        unsigned FEATURE    :16;
    } Bits;
} POSIF_VERID_STR;
#define _POSIF_VERID          (*( POSIF_VERID_STR *) (CMSDK_POSIF_BASE + 0x00))
#define POSIF_VERID           (_POSIF_VERID).Word
#define POSIF_VERID_MINVN     (_POSIF_VERID).Bits.MINVN
#define POSIF_VERID_MAJVN     (_POSIF_VERID).Bits.MAJVN
#define POSIF_VERID_FEATURE   (_POSIF_VERID).Bits.FEATURE


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned HALLCHGIF           :1;
        unsigned HALLOVFIF           :1;
        unsigned RESERVED_2_31       :30;
    } Bits;
} POSIF_HALLIR_STR;
#define _POSIF_HALLIR         (*( POSIF_HALLIR_STR *) (CMSDK_POSIF_BASE + 0x04))
#define POSIF_HALLIR          (_POSIF_HALLIR).Word
#define POSIF_HALLIR_HALLCHGIF (_POSIF_HALLIR).Bits.HALLCHGIF
#define POSIF_HALLIR_HALLOVFIF (_POSIF_HALLIR).Bits.HALLOVFIF


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned HALLCHGIE           :1;
        unsigned HALLOVFIE           :1;
        unsigned RESERVED_2_31       :30;
    } Bits;
} POSIF_HALLIE_STR;
#define _POSIF_HALLIE         (*( POSIF_HALLIE_STR *) (CMSDK_POSIF_BASE + 0x08))
#define POSIF_HALLIE          (_POSIF_HALLIE).Word
#define POSIF_HALLIE_HALLCHGIE (_POSIF_HALLIE).Bits.HALLCHGIE
#define POSIF_HALLIE_HALLOVFIE (_POSIF_HALLIE).Bits.HALLOVFIE


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned HALLEN     :1;
        unsigned RESERVED_1_2 :2;
        unsigned HALLSW     :1;
        unsigned HALLDIV    :2;
        unsigned RESERVED_6_11 :6;
        unsigned HALLFLT75  :1;
        unsigned RESERVED_13_15 :3;
        unsigned HALLFLTEN  :16;
    } Bits;
} POSIF_HALLCR_STR;
#define _POSIF_HALLCR         (*( POSIF_HALLCR_STR *)(CMSDK_POSIF_BASE + 0x0C))
#define POSIF_HALLCR          (_POSIF_HALLCR).Word
#define POSIF_HALLCR_HALLEN   (_POSIF_HALLCR).Bits.HALLEN
#define POSIF_HALLCR_HALLSW   (_POSIF_HALLCR).Bits.HALLSW
#define POSIF_HALLCR_HALLDIV  (_POSIF_HALLCR).Bits.HALLDIV
#define POSIF_HALLCR_HALLFLT75 (_POSIF_HALLCR).Bits.HALLFLT75
#define POSIF_HALLCR_HALLFLTEN (_POSIF_HALLCR).Bits.HALLFLTEN


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned HALLTH     :32;
    } Bits;
} POSIF_HALLTH_STR;
#define _POSIF_HALLTH         (*( POSIF_HALLTH_STR *)(CMSDK_POSIF_BASE + 0x10))
#define POSIF_HALLTH          (_POSIF_HALLTH).Word
#define POSIF_HALLTH_HALLTH   (_POSIF_HALLTH).Bits.HALLTH


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned HALLCNT    :32;
    } Bits;
} POSIF_HALLCNT_STR;
#define _POSIF_HALLCNT        (*( POSIF_HALLCNT_STR *)(CMSDK_POSIF_BASE + 0x14))
#define POSIF_HALLCNT         (_POSIF_HALLCNT).Word
#define POSIF_HALLCNT_HALLCNT (_POSIF_HALLCNT).Bits.HALLCNT


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned HALLCCR    :32;
    } Bits;
} POSIF_HALLCCR_STR;
#define _POSIF_HALLCCR        (*( POSIF_HALLCCR_STR *)(CMSDK_POSIF_BASE + 0x18))
#define POSIF_HALLCCR         (_POSIF_HALLCCR).Word
#define POSIF_HALLCCR_HALLCCR (_POSIF_HALLCCR).Bits.HALLCCR


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned HALLFLTDATA    :3;
        unsigned RESERVED_3_15  :13;
        unsigned HALLRAWDATA    :3;
        unsigned RESERVED_19_31 :13;
    } Bits;
} POSIF_HALLINFO_STR;
#define _POSIF_HALLINFO       (*( POSIF_HALLINFO_STR *)(CMSDK_POSIF_BASE + 0x1C))
#define POSIF_HALLINFO        (_POSIF_HALLINFO).Word
#define POSIF_HALLINFO_HALLFLTDATA (_POSIF_HALLINFO).Bits.HALLFLTDATA
#define POSIF_HALLINFO_HALLRAWDATA (_POSIF_HALLINFO).Bits.HALLRAWDATA


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned HALLUNLOCK     :1;
        unsigned RESERVED_1_15  :15;
        unsigned HALLKEY       :16;
    } Bits;
} POSIF_HALLLOCK_STR;
#define _POSIF_HALLLOCK       (*( POSIF_HALLLOCK_STR *)(CMSDK_POSIF_BASE + 0x20))
#define POSIF_HALLLOCK        (_POSIF_HALLLOCK).Word
#define POSIF_HALLLOCK_HALLUNLOCK (_POSIF_HALLLOCK).Bits.HALLUNLOCK
#define POSIF_HALLLOCK_HALLKEY    (_POSIF_HALLLOCK).Bits.HALLKEY


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned QEPZEC     :1;
        unsigned QEPZLC     :1;
        unsigned QEPZPC     :1;
        unsigned QEPZNC     :1;
        unsigned RESERVED_4_7 :4;
        unsigned QEPMODE    :2;
        unsigned QEPFCNTEN  :1;
        unsigned RESERVED_11_14 :4;
        unsigned QEPEN      :1;
        unsigned QEPCKDIV   :3;
        unsigned RESERVED_19 :1;
        unsigned QEPSAP     :11;
        unsigned RESERVED_31 :1;
    } Bits;
} POSIF_QEPCFG_STR;
#define _POSIF_QEPCFG         (*( POSIF_QEPCFG_STR *)(CMSDK_POSIF_BASE + 0x30))
#define POSIF_QEPCFG          (_POSIF_QEPCFG).Word
#define POSIF_QEPCFG_QEPZEC   (_POSIF_QEPCFG).Bits.QEPZEC
#define POSIF_QEPCFG_QEPZLC   (_POSIF_QEPCFG).Bits.QEPZLC
#define POSIF_QEPCFG_QEPZPC   (_POSIF_QEPCFG).Bits.QEPZPC
#define POSIF_QEPCFG_QEPZNC   (_POSIF_QEPCFG).Bits.QEPZNC
#define POSIF_QEPCFG_QEPMODE  (_POSIF_QEPCFG).Bits.QEPMODE
#define POSIF_QEPCFG_QEPFCNTEN (_POSIF_QEPCFG).Bits.QEPFCNTEN
#define POSIF_QEPCFG_QEPEN    (_POSIF_QEPCFG).Bits.QEPEN
#define POSIF_QEPCFG_QEPCKDIV (_POSIF_QEPCFG).Bits.QEPCKDIV
#define POSIF_QEPCFG_QEPSAP   (_POSIF_QEPCFG).Bits.QEPSAP


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned QEPTH      :16;
        unsigned RESERVED_16_31 :16;
    } Bits;
} POSIF_QEPTH_STR;
#define _POSIF_QEPTH          (*( POSIF_QEPTH_STR *)(CMSDK_POSIF_BASE + 0x34))
#define POSIF_QEPTH           (_POSIF_QEPTH).Word
#define POSIF_QEPTH_QEPTH     (_POSIF_QEPTH).Bits.QEPTH


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned QEPCNT     :16;
        unsigned RESERVED_16_31 :16;
    } Bits;
} POSIF_QEPCNT_STR;
#define _POSIF_QEPCNT         (*( POSIF_QEPCNT_STR *)(CMSDK_POSIF_BASE + 0x38))
#define POSIF_QEPCNT          (_POSIF_QEPCNT).Word
#define POSIF_QEPCNT_QEPCNT   (_POSIF_QEPCNT).Bits.QEPCNT


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned QEPUDFIE   :1;
        unsigned QEPOVFIE   :1;
        unsigned RESERVED_2_31 :30;
    } Bits;
} POSIF_QEPIE_STR;
#define _POSIF_QEPIE          (*( POSIF_QEPIE_STR *)(CMSDK_POSIF_BASE + 0x3C))
#define POSIF_QEPIE           (_POSIF_QEPIE).Word
#define POSIF_QEPIE_QEPUDFIE  (_POSIF_QEPIE).Bits.QEPUDFIE
#define POSIF_QEPIE_QEPOVFIE  (_POSIF_QEPIE).Bits.QEPOVFIE


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned QEPUDFIF   :1;
        unsigned QEPOVFIF   :1;
        unsigned RESERVED_2_31 :30;
    } Bits;
} POSIF_QEPIF_STR;
#define _POSIF_QEPIF          (*( POSIF_QEPIF_STR *)(CMSDK_POSIF_BASE + 0x40))
#define POSIF_QEPIF           (_POSIF_QEPIF).Word
#define POSIF_QEPIF_QEPUDFIF  (_POSIF_QEPIF).Bits.QEPUDFIF
#define POSIF_QEPIF_QEPOVFIF  (_POSIF_QEPIF).Bits.QEPOVFIF

#endif


//1K:0x4000_6400-0x4000_67FF->POSIF

/* HALL Register Structure */
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

    /* 0x04: Hall Sensor Interrupt Status Register */
    union {
        struct {
            __IO uint32_t HALLCHGIF         : 1;  /* bit[0]     */
            __IO uint32_t HALLOVFIF         : 1;  /* bit[1]     */
            __IO uint32_t HALLOVSWTIF       : 1;  /* bit[1]     */
            __IO uint32_t RESERVED_3_31     : 29; /* bit[31:3]  */
        };
        __IO uint32_t w;
    }IR;

    /* 0x08: Hall Sensor Interrupt Enable Register */
    union {
        struct {
            __IO uint32_t HALLCHGIE     : 1;  /* bit[0]     */
            __IO uint32_t HALLOVFIE     : 1;  /* bit[1]     */
            __IO uint32_t HALLSWTIE     : 1;  /* bit[1]     */
            __IO uint32_t RESERVED_3_31 : 29; /* bit[31:3]  */
        };
        __IO uint32_t w;
    } IE;

    /* 0x0C: Hall Sensor Control Register */
    union {
        struct {
            __IO uint32_t HALLEN            : 1;  /* bit[0]     */
            __IO uint32_t RESERVED_1_2      : 2;  /* bit[2:1]   */
            __IO uint32_t HALLSW            : 1;  /* bit[3]     */
            __IO uint32_t HALLDIV           : 2;  /* bit[5:4]   */
            __IO uint32_t RESERVED_6_11     : 6;  /* bit[11:6]  */
            __IO uint32_t HALLFLT75         : 1;  /* bit[12]    */
            __IO uint32_t RESERVED_13_15    : 3;  /* bit[15:13] */
            __IO uint32_t HALLFLTEN         : 16; /* bit[31:16] */
        };
        __IO uint32_t w;
    } CFG;

    /* 0x10: Hall Sensor Threshold Register */
	__IO uint32_t TH;					/* bit[31:0]  */

    /* 0x14: Hall Sensor Counter Register */
    __IO uint32_t CNT;

    /* 0x18: Hall Sensor Capture Compare Register */
    __IO uint32_t WIDTH;                /* bit[31:0]  */

    /* 0x1C: Hall Sensor Information Register */
    union {
        struct {
            __IO uint32_t HALLFLTDATA       : 3;  /* bit[2:0]   */
            __IO uint32_t RESERVED_3_15     : 13; /* bit[15:3]  */
            __IO uint32_t HALLRAWDATA       : 3;  /* bit[18:16] */
            __IO uint32_t RESERVED_19_31    : 13; /* bit[31:19] */
        };
        __IO uint32_t w;
    } INFO;

    /* 0x20: Hall Sensor Lock Register */
    union {
        struct {
            __IO uint32_t HALLUNLOCK     : 1;  /* bit[0]     */
            __IO uint32_t RESERVED_1_15  : 15; /* bit[15:1]  */
            __IO uint32_t HALLKEY        : 16; /* bit[31:16] */ /*write 5A5A unlock */
        };
        __IO uint32_t w;
    } LOCK;

} HALL_TypeDef;

#if 0
typedef struct
{
    __IO uint32_t VERID; /* 0x00: Version ID Register */
    __IO uint32_t IR;    /* 0x04: Hall Sensor Interrupt Status Register */
    __IO uint32_t IE;    /* 0x08: Hall Sensor Interrupt Enable Register */
    __IO uint32_t CFG;   /* 0x0C: Hall Sensor Control Register */
    __IO uint32_t TH;   /* 0x10: Hall Sensor Threshold Register */
    __IO uint32_t CNT;  /* 0x14: Hall Sensor Counter Register */
    __IO uint32_t CCR;  /* 0x18: Hall Sensor Capture Compare Register */
    __IO uint32_t INFO; /* 0x1C: Hall Sensor Information Register */
    __IO uint32_t LOCK;/* 0x20: Hall Sensor Lock Register */
} HALL_TypeDef;
 #endif



 
/* =========================================================================================================================== */
/* ================                                            HALL                                            ================ */
/* =========================================================================================================================== */
/* ========================================================  CFG ============================================================== */
#define	HALL_CFG_SWIE_Pos	                              (2)
#define HALL_CFG_SWIE_Msk	                              (0x1UL<<HALL_CFG_SWIE_Pos)
#define	HALL_CFG_OVIE_Pos	                              (1)
#define HALL_CFG_OVIE_Msk	                              (0x1UL<<HALL_CFG_OVIE_Pos)
#define	HALL_CFG_CHGIE_Pos	                            (0)
#define HALL_CFG_CHGIE_Msk	                            (0x1UL<<HALL_CFG_CHGIE_Pos)
#define	HALL_CFG_HALLEN_Pos	                            (0)
#define HALL_CFG_HALLEN_Msk	                            (0x1UL<<HALL_CFG_HALLEN_Pos)
#define	HALL_CFG_FIT75_Pos	                            (12)
#define HALL_CFG_FIT75_Msk	                            (0x1UL<<HALL_CFG_FIT75_Pos)
#define	HALL_CFG_CLKDIV_Pos	                            (4)
#define HALL_CFG_CLKDIV_Msk	                            (0x3UL<<HALL_CFG_CLKDIV_Pos)
#define	HALL_CFG_FITLEN_Pos	                            (16)
#define HALL_CFG_FITLEN_Msk	                            (0xFFFFUL<<HALL_CFG_FITLEN_Pos)
/* ========================================================  INFO ======================================================== */
#define	HALL_INFO_RAWDATA_Pos	                        (16)
#define HALL_INFO_RAWDATA_Msk	                        (0x7UL<<HALL_INFO_RAWDATA_Pos)
#define	HALL_INFO_FITDATA_Pos	                        (8)
#define HALL_INFO_FITDATA_Msk	                        (0x7UL<<HALL_INFO_FITDATA_Pos)
#define	HALL_INFO_OVIF_Pos	                            (1)
#define HALL_INFO_OVIF_Msk	                            (0x1UL<<HALL_INFO_OVIF_Pos)
#define	HALL_INFO_CHGIF_Pos	                            (0)
#define HALL_INFO_CHGIF_Msk	                            (0x1UL<<HALL_INFO_CHGIF_Pos)
/* ========================================================  INFO ======================================================== */
#define	HALL_CLRF_SETSWF_Pos	                        (3)
#define HALL_CLRF_SETSWF_Msk	                        (0x1UL<<HALL_CLRF_SETSWF_Pos)
#define	HALL_CLRF_CLROVF_Pos	                        (1)
#define HALL_CLRF_CLROVF_Msk	                        (0x1UL<<HALL_CLRF_CLROVF_Pos)
#define	HALL_CLRF_CLRCHGF_Pos	                        (0)
#define HALL_CLRF_CLRCHGF_Msk	                        (0x1UL<<HALL_CLRF_CLRCHGF_Pos)

#endif
