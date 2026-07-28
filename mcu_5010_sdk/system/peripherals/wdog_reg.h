#ifndef WDOG_REG_H
#define WDOG_REG_H

#if 0

/* Independent Watchdog (IWDG) Register Definitions */

/* 0x00: Reload Register */
typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned RL : 32;
    } Bits;
} IWDG_RLR_STR;

#define _IWDG_RLR           (*(IWDG_RLR_STR *)(CMSDK_IWATCHDOG_BASE + 0x00))
#define IWDG_RLR            (_IWDG_RLR).Word
#define IWDG_RLR_RL         (_IWDG_RLR).Bits.RL


/* 0x04: Counter Register */
typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned CNT : 32;
    } Bits;
} IWDG_CNTR_STR;

#define _IWDG_CNTR          (*(IWDG_CNTR_STR *)(CMSDK_IWATCHDOG_BASE + 0x04))
#define IWDG_CNTR           (_IWDG_CNTR).Word
#define IWDG_CNTR_CNT       (_IWDG_CNTR).Bits.CNT


/* 0x08: Control Register */
typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned EN         : 1;
        unsigned RESERVED_1_30 : 30;
        unsigned DBGE       : 1;
    } Bits;
} IWDG_CR_STR;

#define _IWDG_CR            (*(IWDG_CR_STR *)(CMSDK_IWATCHDOG_BASE + 0x08))
#define IWDG_CR             (_IWDG_CR).Word
#define IWDG_CR_EN          (_IWDG_CR).Bits.EN
#define IWDG_CR_DBGE        (_IWDG_CR).Bits.DBGE


/* 0x0C: Key Register */
typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned KEY : 32;
    } Bits;
} IWDG_KR_STR;

#define _IWDG_KR            (*(IWDG_KR_STR *)(CMSDK_IWATCHDOG_BASE + 0x0C))
#define IWDG_KR             (_IWDG_KR).Word
#define IWDG_KR_KEY         (_IWDG_KR).Bits.KEY



/* 0x10: Status Register */
typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned HDF        : 1;
        unsigned RESERVED_1_31 : 31;
    } Bits;
} IWDG_SR_STR;

#define _IWDG_SR            (*(IWDG_SR_STR *)(CMSDK_IWATCHDOG_BASE + 0x10))
#define IWDG_SR             (_IWDG_SR).Word
#define IWDG_SR_HDF         (_IWDG_SR).Bits.HDF


/* 0x3FC: Lock Register */
typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned KEY_LOCK   : 1;
        unsigned KEY        : 31;
    } Bits;
} IWDG_LR_STR;

#define _IWDG_LR            (*(IWDG_LR_STR *)(CMSDK_IWATCHDOG_BASE + 0x3FC))
#define IWDG_LR             (_IWDG_LR).Word
#define IWDG_LR_KEY_LOCK    (_IWDG_LR).Bits.KEY_LOCK
#define IWDG_LR_KEY         (_IWDG_LR).Bits.KEY

#endif


/*RL_STR address is error*/
/* Independent Watchdog (IWDG) Register Structure */
typedef struct
{
    /* 0x00: Reload Register */
    union {
        struct {
            __IO uint32_t RL         : 32; /* bit[31:0]  */
        };
        __IO uint32_t w;
    } RLR;

    /* 0x04: Counter Register */
    union {
        struct {
            __IO uint32_t CNT        : 32; /* bit[31:0]  */
        };
        __IO uint32_t w;
    } CNTR;

    /* 0x08: Control Register */
    union {
        struct {
            __IO uint32_t EN             : 1;  /* bit[0]     */
            __IO uint32_t RESERVED_1_30  : 30; /* bit[30:1]  */
            __IO uint32_t DBGE           : 1;  /* bit[31]    */
        };
        __IO uint32_t w;
    } CR;

    /* 0x0C: Key Register */
    union {
        struct {
            __IO uint32_t KEY        : 32; /* bit[31:0]  */
        };
        __IO uint32_t w;
    } KR;

    /* 0x10: Status Register */
    union {
        struct {
            __IO uint32_t HDF        : 1;  /* bit[0]     */
            __IO uint32_t RESERVED_1_31 : 31; /* bit[31:1]  */
        };
        __IO uint32_t w;
    } SR;

    /* 0x14-0x3F8: Reserved Space */
    __IO uint32_t RESERVED[250]; /* 0x3FC - 0x14 = 0x3E8 bytes = 0xFA * 4 bytes = 250 + 1 = 251 dwords */

    /* 0x3FC: Lock Register */
    union {
        struct {
            __IO uint32_t KEY_LOCK   : 1;  /* bit[0]     */
            __IO uint32_t KEY        : 31; /* bit[31:1]  */
        };
        __IO uint32_t w;
    } LR_STR;

} IWDG_TypeDef;


#if 0
typedef struct
{
    __IO uint32_t RLR; /* 0x00: Reload Register */
    __IO uint32_t CNTR;/* 0x04: Counter Register */
    __IO uint32_t CR; /* 0x08: Control Register */
    __IO uint32_t KR; /* 0x0C: Key Register */
    __IO uint32_t SR; /* 0x10: Status Register */
} IWDG_TypeDef;
#endif

#endif
