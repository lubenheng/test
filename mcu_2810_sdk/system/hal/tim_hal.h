#ifndef __TIM_HAL_H
#define __TIM_HAL_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "CMSDK_CM0.h"

#pragma anon_unions
#define __IO volatile

/*============================================================================*/
/*                   定时器基地址 (带 _2810 后缀)                             */
/*============================================================================*/
#define TIMER0_BASE_2810        (APB_BASE + 0x3000UL)
#define TIMER1_BASE_2810        (APB_BASE + 0x3400UL)
#define TIMER2_BASE_2810        (APB_BASE + 0x3800UL)
#define TIMER3_BASE_2810        (APB_BASE + 0x3c00UL)
#define TIMER4_BASE_2810        (APB_BASE + 0x4000UL)
#define TIMER5_BASE_2810        (APB_BASE + 0x5C00UL)
#define TIMER6_BASE_2810        (APB_BASE + 0x6000UL)

/* 数字控制基地址 */
#define DIG_CTRL_BASE_2810      0x40002400UL

/* 中断号定义 (与文档一致) */
#define TIM0_IRQn               10
#define TIM1_IRQn               11
#define TIM2_IRQn               12
#define TIM3_IRQn               13
#define TIM4_IRQn               14
#define TIM5_IRQn               23
#define TIM6_IRQn               31

/*============================================================================*/
/*                         枚举类型定义                                      */
/*============================================================================*/
typedef enum {
    TIM_INST_0 = 0,
    TIM_INST_1,
    TIM_INST_2,
    TIM_INST_3,
    TIM_INST_4,
    TIM_INST_5,
    TIM_INST_6,
    TIM_INST_MAX
} tim_inst_e;

typedef enum {
    TIM_CLK_DIV_1 = 0,   /* 不分频 */
    TIM_CLK_DIV_2 = 1,
    TIM_CLK_DIV_4 = 2,
    TIM_CLK_DIV_8 = 3,
    TIM_CLK_DIV_16 = 4,
    TIM_CLK_DIV_32 = 5,
    TIM_CLK_DIV_64 = 6,
    TIM_CLK_DIV_128 = 7
} tim_clock_div_e;

typedef enum {
    TIM_MODE_Input_Capture_ALIGNED = 0,
    TIM_MODE_Ouput_ALIGNED = 1
} tim_mode_e;

typedef enum {
    TIM_OPM_DISABLE = 0,
    TIM_OPM_ENABLE = 1
} tim_opm_e;

typedef enum {
    TIM_OCM_DISABLE = 0,
    TIM_OCM_ENABLE = 1
} tim_ocm_e;

typedef enum {
    TIM_IT_UPDATE = 0x01,
    TIM_IT_CC1    = 0x02
} tim_it_t;

/*============================================================================*/
/*                     TIM0/TIM5/TIM6 寄存器结构体                            */
/* 文档: TIM0/TIM5/TIM6 章节                                                */
/*============================================================================*/
typedef struct {
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Timer_en : 1;   /* 位0: 计数器使能 */
            __IO uint32_t Int_en   : 1;   /* 位1: 中断使能 */
            __IO uint32_t RESERVED : 30;
        };
    } EN;        /* 地址偏移 0x00 */

    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t Int_clr   : 1;   /* 写1清中断 (WO), 读为中断状态 (RO) */
            __IO uint32_t Force_rld : 1;   /* 写1强制重载 (WO) */
            __IO uint32_t RESERVED  : 30;
        };
    } INT_CLR;   /* 地址偏移 0x01 */

    union {
        __IO uint32_t w;
        __IO uint32_t Cur_val;            /* 当前计数值, 只读, 偏移 0x02 */
    } CUR_VAL;

    union {
        __IO uint32_t w;
        __IO uint32_t Reload_val;         /* 重载值, 读写, 偏移 0x03 */
    } RELOAD_VAL;
} TIM0_TypeDef_2810;   /* 同样适用于 TIM5, TIM6 */

/*============================================================================*/
/*                     TIM1 寄存器结构体 (高级定时器)                         */
/* 文档: TIM1 章节                                                          */
/*============================================================================*/
typedef struct {
    /* 0x00: CR0 */
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t T1RCEN : 1;   /* 重载计数器使能 */
            __IO uint32_t T1BCEN : 1;   /* 基本计数器使能 */
            __IO uint32_t T1OPS  : 2;   /* 换相触发信号选择 */
            __IO uint32_t T1FORC : 1;   /* 60度强制换相使能 */
            __IO uint32_t T1CFLT : 2;   /* 60度基准时间滤波选择 */
            __IO uint32_t T1RWEN : 1;   /* T1RCEN写使能 */
            __IO uint32_t T1EN   : 1;   /* Timer1总体使能 */
            __IO uint32_t RESERVED : 23;
        };
    } CR0;

    /* 0x01: CR1 */
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t BSEL    : 7;   /* 屏蔽续流角度选择 */
            __IO uint32_t T1BAPE  : 1;   /* BARR自动装载使能 */
            __IO uint32_t RESERVED : 24;
        };
    } CR1;

    /* 0x02: CR2 */
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t CSEL    : 7;   /* 换相角度选择 */
            __IO uint32_t T1BRS   : 1;   /* 基本计数器复位源选择 */
            __IO uint32_t RESERVED : 24;
        };
    } CR2;

    /* 0x03: CR3 */
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t T1INM   : 2;   /* 位置检测信号滤波脉宽 */
            __IO uint32_t T1TIS   : 2;   /* 位置检测信号选择 */
            __IO uint32_t T1PSC   : 3;   /* 计数器时钟分频 */
            __IO uint32_t RESERVED : 25;
        };
    } CR3;

    /* 0x04: CR4 */
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t T1CST   : 3;   /* 换相状态机 */
            __IO uint32_t RESERVED : 29;
        };
    } CR4;

    /* 0x05: IER */
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t T1BDIE  : 1;   /* 屏蔽续流结束中断使能 */
            __IO uint32_t T1PDIE  : 1;   /* CMP/GPIO位置检测中断使能 */
            __IO uint32_t T1WTIE  : 1;   /* 写入时序中断使能 */
            __IO uint32_t T1ROIE  : 1;   /* 重载计数器上溢中断使能 */
            __IO uint32_t T1BOIE  : 1;   /* 基本计数器上溢中断使能 */
            __IO uint32_t T1ADIE  : 1;   /* ADC位置检测中断使能 */
            __IO uint32_t T1MAME  : 1;   /* 手动模式使能 */
            __IO uint32_t T1UPD   : 1;   /* 软件触发数据传输 */
            __IO uint32_t T1BDIFE : 1;
            __IO uint32_t T1PDIFE : 1;
            __IO uint32_t T1WTIFE : 1;
            __IO uint32_t T1ROIFE : 1;
            __IO uint32_t T1BOIFE : 1;
            __IO uint32_t T1ADIFE : 1;
            __IO uint32_t RESERVED : 18;
        };
    } IER;

    /* 0x06: SR */
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t T1BDIF  : 1;   /* 屏蔽续流结束中断标志 (写0清) */
            __IO uint32_t T1PDIF  : 1;   /* CMP/GPIO位置检测中断标志 */
            __IO uint32_t T1WTIF  : 1;   /* 写入时序中断标志 */
            __IO uint32_t T1ROIF  : 1;   /* 重载计数器上溢中断标志 */
            __IO uint32_t T1BOIF  : 1;   /* 基本计数器上溢中断标志 */
            __IO uint32_t T1ADIF  : 1;   /* ADC位置检测中断标志 */
            __IO uint32_t RESERVED : 26;
        };
    } SR;

    /* 0x07: BCOR */
    __IO uint32_t BCOR;      /* 基本计数器计数值滤波值 */
    /* 0x08: DBR1 */
    __IO uint32_t DBR1;
    /* 0x09: DBR2 */
    __IO uint32_t DBR2;
    /* 0x0A: DBR3 */
    __IO uint32_t DBR3;
    /* 0x0B: DBR4 */
    __IO uint32_t DBR4;
    /* 0x0C: DBR5 */
    __IO uint32_t DBR5;
    /* 0x0D: DBR6 */
    __IO uint32_t DBR6;
    /* 0x0E: DBR7 */
    __IO uint32_t DBR7;
    /* 0x0F: BCNTR */
    __IO uint32_t BCNTR;
    /* 0x10: BCCR */
    __IO uint32_t BCCR;
    /* 0x11: BARR */
    __IO uint32_t BARR;
    /* 0x12: RARR */
    __IO uint32_t RARR;
    /* 0x13: RCNTR */
    __IO uint32_t RCNTR;
    /* 0x14: UCOP */
    __IO uint32_t UCOP;
    /* 0x15: UFLP */
    __IO uint32_t UFLP;
    /* 0x16: URES */
    __IO uint32_t URES;
    /* 0x17: UIGN */
    __IO uint32_t UIGN;
    /* 0x18: KF */
    __IO uint32_t KF;
    /* 0x19: KR */
    __IO uint32_t KR;
    /* 0x1A: ITRIP */
    __IO uint32_t ITRIP;
} TIM1_TypeDef_2810;

/*============================================================================*/
/*                     TIM2 寄存器结构体                                      */
/* 文档: TIM2 章节                                                          */
/*============================================================================*/
typedef struct {
    /* 0x00: CR0 */
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t T2MOD   : 2;   /* 模式选择 */
            __IO uint32_t T2CES   : 1;   /* 计数沿选择 */
            __IO uint32_t T2IRE   : 1;   /* 中断使能 (输出比较/脉宽检测/方向改变) */
            __IO uint32_t T2OCM   : 1;   /* 输出模式选择 / QEP模式选择 */
            __IO uint32_t T2PSC   : 3;   /* 时钟分频 */
            __IO uint32_t T2TISRC : 1;   /* QEP/RSD模式选择 */
            __IO uint32_t RESERVED : 23;
        };
    } CR0;

    /* 0x01: CR1 */
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t T2CEN    : 1;   /* 基本计数器使能 */
            __IO uint32_t T2DIR    : 1;   /* 旋转方向 (RO) */
            __IO uint32_t T2FE     : 1;   /* 输入滤波使能 */
            __IO uint32_t T2IFE    : 1;   /* 上溢中断使能 */
            __IO uint32_t T2IPE    : 1;   /* 周期检测/匹配/边沿中断使能 */
            __IO uint32_t T2IF     : 1;   /* 上溢中断标志 (写0清) */
            __IO uint32_t T2IP     : 1;   /* 周期/匹配/边沿中断标志 (写0清) */
            __IO uint32_t T2IR     : 1;   /* 比较/脉宽/方向中断标志 (写0清) */
            __IO uint32_t CFG_T2IFFE : 1;
            __IO uint32_t CFG_T2IPFE : 1;
            __IO uint32_t CFG_T2IRFE : 1;
            __IO uint32_t RESERVED   : 21;
        };
    } CR1;

    /* 0x02: CNTR */
    union {
        __IO uint32_t w;
        __IO uint32_t CNTR;            /* 计数器值 */
    } CNTR;

    /* 0x03: DR */
    union {
        __IO uint32_t w;
        __IO uint32_t DR;              /* 比较值/脉宽计数值/计数个数 */
    } DR;

    /* 0x04: ARR */
    union {
        __IO uint32_t w;
        __IO uint32_t ARR;             /* 重载值/周期计数值 */
    } ARR;
} TIM2_TypeDef_2810;

/*============================================================================*/
/*                     TIM3/TIM4 寄存器结构体 (已有, 保持兼容)                */
/* 文档: TIM3, TIM4 章节                                                    */
/*============================================================================*/
typedef struct {
    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t MODE   : 1;
            __IO uint32_t OPM    : 1;
            __IO uint32_t RSV0   : 1;
            __IO uint32_t IRE    : 1;
            __IO uint32_t OCM    : 1;
            __IO uint32_t PSC    : 3;
        };
    } CR0;

    union {
        __IO uint32_t w;
        struct {
            __IO uint32_t EN     : 1;
            __IO uint32_t INM    : 2;
            __IO uint32_t IFE    : 1;
            __IO uint32_t IPE    : 1;
            __IO uint32_t IF     : 1;
            __IO uint32_t IP     : 1;
            __IO uint32_t IR     : 1;
            __IO uint32_t iffe   : 1;
            __IO uint32_t ipfe   : 1;
            __IO uint32_t irfe   : 1;
        };
    } CR1;

    union {
        __IO uint32_t w;
        __IO uint32_t CNTR;
    } CNTR;

    union {
        __IO uint32_t w;
        __IO uint32_t DR;
    } DR;

    union {
        __IO uint32_t w;
        __IO uint32_t ARR;
    } ARR;
} TIM_TypeDef_2810;   /* 用于 TIM3, TIM4 */

/*============================================================================*/
/*                    数字控制寄存器结构体                                    */
/*============================================================================*/
typedef struct {
    __IO uint32_t REMAP;      // 0x00
    __IO uint32_t SYS;        // 0x04
    __IO uint32_t CLK_EN0;    // 0x08
    __IO uint32_t CLK_EN1;    // 0x0C
} DIG_CTRL_TypeDef;

/*============================================================================*/
/*                    寄存器指针宏 (带 _2810 后缀)                            */
/*============================================================================*/
#define TIM0_2810    ((TIM0_TypeDef_2810 *)TIMER0_BASE_2810)
#define TIM1_2810    ((TIM1_TypeDef_2810  *)TIMER1_BASE_2810)
#define TIM2_2810    ((TIM2_TypeDef_2810 *)TIMER2_BASE_2810)
#define TIM3_2810    ((TIM_TypeDef_2810 *)TIMER3_BASE_2810)
#define TIM4_2810    ((TIM_TypeDef_2810 *)TIMER4_BASE_2810)
#define TIM5_2810    ((TIM0_TypeDef_2810 *)TIMER5_BASE_2810)
#define TIM6_2810    ((TIM0_TypeDef_2810 *)TIMER6_BASE_2810)

#define DIGCTRL_2810 ((DIG_CTRL_TypeDef *)DIG_CTRL_BASE_2810)

/*============================================================================*/
/*                    定时器基础句柄结构体                                    */
/*============================================================================*/
typedef struct {
    tim_inst_e      instance;
    uint32_t        period;         /* 自动重载值 ARR */
    uint32_t        prescaler;      /* 保留兼容性 */
    tim_clock_div_e clock_div;
    tim_mode_e      mode;           /* 仅 TIM3/TIM4 有效 */
    tim_opm_e       opm;
    tim_ocm_e       ocm;
    bool            enable_update_it;
    bool            enable_compare_it;
} tim_base_handle_t;

typedef void (*tim_callback_t)(tim_inst_e instance);

/* 函数声明 */
void tim_base_init(tim_base_handle_t *htim);
void tim_base_deinit(tim_inst_e instance);
void tim_base_start(tim_inst_e instance);
void tim_base_stop(tim_inst_e instance);
void tim_base_start_it(tim_inst_e instance, uint32_t it_mask);
void tim_base_stop_it(tim_inst_e instance, uint32_t it_mask);
void tim_it_clear_flag(tim_inst_e instance, uint32_t it_mask);
bool tim_it_get_flag(tim_inst_e instance, uint32_t it_mask);
void tim_register_callback(tim_inst_e instance, tim_callback_t callback);
void tim_irq_handler(tim_inst_e instance);

#endif
