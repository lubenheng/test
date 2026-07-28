#ifndef CMSDK_H
#define CMSDK_H

#ifndef __IM                                    /*!< Fallback for older CMSIS versions                                         */
  #define __IM   __I
#endif
#ifndef __OM                                    /*!< Fallback for older CMSIS versions                                         */
  #define __OM   __O
#endif
#ifndef __IOM                                   /*!< Fallback for older CMSIS versions                                         */
  #define __IOM  __IO
#endif


typedef enum IRQn
{
/******  Cortex-M0 Processor Exceptions Numbers ***************************************************/

/* ToDo: use this Cortex interrupt numbers if your device is a CORTEX-M0 device                   */
    NonMaskableInt_IRQn           = -14,      /*!<  2 Cortex-M0 Non Maskable Interrupt              */
    HardFault_IRQn                = -13,      /*!<  3 Cortex-M0 Hard Fault Interrupt                */
    SVCall_IRQn                   = -5,       /*!< 11 Cortex-M0 SV Call Interrupt                   */
    PendSV_IRQn                   = -2,       /*!< 14 Cortex-M0 Pend SV Interrupt                   */
    SysTick_IRQn                  = -1,       /*!< 15 Cortex-M0 System Tick Interrupt               */

/******  CMSDK Specific Interrupt Numbers *********************************************************/
    int0_irq        = 0,
    int1_irq        = 1,
    int2_irq        = 2,
    int3_irq        = 3,
    int4_irq        = 4,
    pin_irq         = 5,
    rtc_irq         = 6,
    uart0_irq       = 7,
    uart1_irq       = 8,
    i2c0_irq        = 9,
    tim0_irq        = 10,
    tim1_irq        = 11,
    tim2_irq        = 12,
    tim3_irq        = 13,
    tim4_irq        = 14,
    fgif_irq        = 15,
    dcif_irq        = 16,
    adc_irq         = 17,
    cmp0_irq        = 18,
    cmp1_irq        = 19,
    cmp2_irq        = 20,
    cmp3_irq        = 21,
    cmp4_irq        = 22,
    tim5_irq        = 23,
    spi_irq         = 24,
    tsd_irq         = 25,
    uvlo_irq        = 26,
    fault_irq       = 27,
    i2c1_irq        = 28,
	foc_irq         = 29,
	swd_irq         = 30,
	tim6_irq        = 31,

}IRQn_Type;



/*
 * ==========================================================================
 * ----------- Processor and Core Peripheral Section ------------------------
 * ==========================================================================
 */

/* Configuration of the Cortex-M0 Processor and Core Peripherals */
#define __CM0_REV                 0x0000    /*!< Core Revision r0p0                               */
#define __NVIC_PRIO_BITS          2         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used     */
#define __MPU_PRESENT             0         /*!< MPU present or not                               */



#include "core_cm0.h"                       /* Cortex-M0 processor and core peripherals           */




/******************************************************************************/
/*                Device Specific Peripheral registers structures             */
/******************************************************************************/

#pragma anon_unions


typedef struct
{
    __IO   uint32_t  ds0;   //0  
    __IO   uint32_t  ds1;   //1  
    __IO   uint32_t  pu;    //2  
    __IO   uint32_t  pd;    //3  
    __IO   uint32_t  msk;   //4  
    __IO   uint32_t  pol;   //5  
    __IO   uint32_t  st;    //6  
    __IO   uint32_t  ana;   //7  
    __IO   uint32_t  p4r0;  //8  
    __IO   uint32_t  p4r1;  //9  
    __IO   uint32_t  en;    //a  
    __IO   uint32_t  en2;   //b  
    __IO   uint32_t  drv_ds;//c  
    __IO   uint32_t  dbg;   //d  
    __IO   uint32_t  qspi;  //e  

} IO_TypeDef;



typedef struct
{
    __I    uint32_t  in;        //0  
    __IO   uint32_t  out;       //1     
    __IO   uint32_t  out_en;    //2
    __IO   uint32_t  int_en;    //3      
    __IO   uint32_t  int_type;  //4     
    __IO   uint32_t  int_pol;   //5   
    __IO   uint32_t  int_clr;   //6
    __IO   uint32_t  int_edge;  //7
    __IO   uint32_t  p4_cfg;    //8
    __IO   uint32_t  p4_int_set;//9
    __IO   uint32_t  p4_int_clr;//a

} GPIO_TypeDef;



typedef struct
{
    __IO   uint32_t  en;            //0  
    __IO   uint32_t  thr;           //1
    __IO   uint32_t  dat;           //2  
    __IO   uint32_t  int_clr;       //3       
    __IO   uint32_t  baud;          //4    
    __IO   uint32_t  rx_stop_char;  //5

} UART_TypeDef;



typedef struct
{
    __IO   uint32_t  en;         //0  
    __IO   uint32_t  int_clr;    //1  
    __IO   uint32_t  cur_val;    //2
    __IO   uint32_t  reload_val; //3

} TIM0_TypeDef;


typedef struct
{
    __IO   uint32_t  cr0;   //0  
    __IO   uint32_t  cr1;   //1  
    __IO   uint32_t  cr2;   //2
    __IO   uint32_t  cr3;   //3
    __IO   uint32_t  cr4;   //4
    __IO   uint32_t  ier;   //5
    __IO   uint32_t  sr;    //6
    __IO   uint32_t  bcor;  //7
    __IO   uint32_t  dbr1;  //8
    __IO   uint32_t  dbr2;  //9
    __IO   uint32_t  dbr3;  //a
    __IO   uint32_t  dbr4;  //b
    __IO   uint32_t  dbr5;  //c
    __IO   uint32_t  dbr6;  //d
    __IO   uint32_t  dbr7;  //e
    __IO   uint32_t  bcntr; //f
    __IO   uint32_t  bccr;  //10
    __IO   uint32_t  barr;  //11
    __IO   uint32_t  rarr;  //12
    __IO   uint32_t  rcntr; //13
    __IO   uint32_t  ucop;  //14
    __IO   uint32_t  uflp;  //15
    __IO   uint32_t  ures;  //16
    __IO   uint32_t  uign;  //17
    __IO   uint32_t  kf;    //18
    __IO   uint32_t  kr;    //19
    __I    uint32_t  itrip; //1a

} TIM1_TypeDef;


/* =========================================================================================================================== */
/* ================                                          Timer2                                           ================ */
/* =========================================================================================================================== */


/**
  * @brief Timer2 (Timer2)
  */

typedef struct {                                /*!< (@ 0x48360000) Timer2 Structure                                           */
  
  union {
    __IOM uint32_t TIM2_CR0;              /*!< (@ 0x00000000) Control Register                                           */
    
    struct {
      __IOM uint32_t T2MOD      : 2;            /*!< [1..0] Mode selection                                                     */
      __IOM uint32_t T2CES      : 1;            /*!< [2..2] Mode selection                                                     */
      __IOM uint32_t T2IRE      : 1;            /*!< [3..3] Mode selection                                                     */
      __IOM uint32_t T2OCM      : 1;            /*!< [4..4] Mode selection                                                     */
      __IOM uint32_t T2PSC      : 2;            /*!< [6..5] Basic counter clock source frequency selection                     */
            uint32_t            : 25;
    } TIM2_CR0_b;
  } ;
  
  union {
    __IOM uint32_t TIM2_CR1;              /*!< (@ 0x00000004) Control Register                                           */
    
    struct {
      __IOM uint32_t T2CEN      : 1;            /*!< [0..0] Basic counter                                                      */
      __IOM uint32_t T2DIR      : 1;            /*!< [1..1] Motor rotation direction                                           */
      __IOM uint32_t T2FE       : 1;            /*!< [2..2] Input signal filtering enables                                     */
      __IOM uint32_t T2IFE      : 1;            /*!< [3..3] The basic counter overflows the interruption                       */
      __IOM uint32_t T2IPE      : 1;            /*!< [4..4] T2IPE                                                              */
      __IOM uint32_t T2IF       : 1;            /*!< [5..5] The basic counter is a signage of the overflow interrupt           */
      __IOM uint32_t T2IP       : 1;            /*!< [6..6] The PWM cycle detects the interrupt mark bit                       */
      __IOM uint32_t T2IR       : 1;            /*!< [7..7] Match the interrupt event marker bit                               */
      __IOM uint32_t cfg_t2iffe : 1;
			__IOM uint32_t cfg_t2ipfe : 1;
			__IOM uint32_t cfg_t2irfe : 1;
      uint32_t            : 21;
    } TIM2_CR1_b;
  } ;
  
  union {
    __IOM uint32_t TIM2_CNTR;             /*!< (@ 0x00000008) Counter count                                              */
    
    struct {
      __IOM uint32_t TIM2_CNTR  : 16;           /*!< [15..0] Counter count                                                     */
            uint32_t            : 16;
    } TIM2_CNTR_b;
  } ;
  
  union {
    __IOM uint32_t TIM2_DR;               /*!< (@ 0x0000000C) TIM2_DR                                                    */
    
    struct {
      __IOM uint32_t TIM2_DR    : 16;           /*!< [15..0] TIM2_DR                                                           */
            uint32_t            : 16;
    } TIM2_DR_b;
  } ;
  
  union {
    __IOM uint32_t TIM2_ARR;              /*!< (@ 0x00000010) Counter count                                              */
    
    struct {
      __IOM uint32_t TIM2_ARR   : 16;           /*!< [15..0] Counter count                                                     */
            uint32_t            : 16;
    } TIM2_ARR_b;
  } ;
} TIM2_TypeDef;                                  /*!< Size = 20 (0x14)                                                          */


typedef struct
{
    __IO   uint32_t  cr0;   //0  
    __IO   uint32_t  cr1;   //1  
    __IO   uint32_t  cntr;  //2
    __IO   uint32_t  dr;    //3
    __IO   uint32_t  arr;   //4

} TIM3_TypeDef;



typedef struct
{
    __IO   uint32_t  cr0;   //0  
    __IO   uint32_t  cr1;   //1  
    __IO   uint32_t  cntr;  //2
    __IO   uint32_t  dr;    //3
    __IO   uint32_t  arr;   //4

} TIM4_TypeDef;

typedef struct
{
    __IO   uint32_t  en;         //0  
    __IO   uint32_t  int_clr;    //1  
    __IO   uint32_t  cur_val;    //2
    __IO   uint32_t  reload_val; //3

} TIM5_TypeDef;

typedef struct
{
    __IO   uint32_t  en;         //0  
    __IO   uint32_t  int_clr;    //1  
    __IO   uint32_t  cur_val;    //2
    __IO   uint32_t  reload_val; //3

} TIM6_TypeDef;

typedef struct
{
    __IO    uint32_t  reload_val;       //0  
    __I     uint32_t  cur_val;          //1
    __IO    uint32_t  en;               //2       
    __IO    uint32_t  rst;              //3
    __I     uint32_t  reload_bak;       //4

} WDT_TypeDef;



typedef struct
{
    __IO uint32_t basic;            //0
    __IO uint32_t read;             //1
    __IO uint32_t write;            //2
    __IO uint32_t delay;            //3
    __IO uint32_t capture;          //4
    __IO uint32_t page;             //5
    __IO uint32_t remap;            //6
    __IO uint32_t mod_bit;          //7
    __IO uint32_t poll_basic;       //8
    __IO uint32_t poll_max_num;     //9
    __I  uint32_t poll_status;      //a
    __IO uint32_t stig_basic;       //b
    __IO uint32_t stig_addr;        //c
    __IO uint32_t stig_rdat_low;    //d
    __IO uint32_t stig_rdat_high;   //e
    __IO uint32_t stig_wdat_low;    //f
    __IO uint32_t stig_wdat_high;   //10
    __IO uint32_t int_mask;         //11
    __IO uint32_t int_status;       //12

} QSPI_TypeDef;



typedef struct
{
    __IO   uint32_t  remap;             //0      
    __IO   uint32_t  sys;               //1    
    __IO   uint32_t  clk_en0;           //2 
    __I    uint32_t  rsv0[7];           //3~9
    __IO   uint32_t  rst_info;          //a

} DIGCTRL_TypeDef;



typedef struct
{
    __IO   uint32_t  cr;    //0
    __IO   uint32_t  msk;   //1
    __IO   uint32_t  scyc;  //2
    __I    uint32_t  adc0;  //3
    __I    uint32_t  adc1;  //4
    __I    uint32_t  adc2;  //5
    __I    uint32_t  adc3;  //6
    __I    uint32_t  adc4;  //7
    __I    uint32_t  adc5;  //8
    __I    uint32_t  adc6;  //9
    __I    uint32_t  adc7;  //a
    __I    uint32_t  adc8;  //b
    __I    uint32_t  adc9;  //c
    __I    uint32_t  adc10; //d
    __I    uint32_t  adc11; //e
    __I    uint32_t  err;   //f

} ADCCTRL_TypeDef;



//typedef struct
//{
//    __IO   uint32_t  a0;    //0
//    __IO   uint32_t  a1;    //1
//    __IO   uint32_t  a2;    //2
//    __IO   uint32_t  a3;    //3
//    __IO   uint32_t  a4;    //4
//    __IO   uint32_t  a5;    //5
//    __IO   uint32_t  a6;    //6
//    __IO   uint32_t  a7;    //7
//    __IO   uint32_t  cfg;   //8

//} ADCINF_TypeDef;
typedef struct
{
    __IO   uint32_t  ctrl;  //0
    __IO   uint32_t  ch0_7; //1
    __IO   uint32_t  ch8_15;//2
    __IO   uint32_t  tim;   //3

} ADCINF_TypeDef;


typedef struct
{
    __IO   uint32_t  cfg_normal;    //0  
    __IO   uint32_t  cfg_sleep;     //1  
    __IO   uint32_t  cfg_dly;       //2
    __IO   uint32_t  rsv[2];        //3,4     
    __O    uint32_t  wr_key;        //5 

} PMU_TypeDef;



typedef struct
{
    __IO   uint32_t  ctrl;  //0  
    __IO   uint32_t  a;     //1  
    __IO   uint32_t  b;     //2
    __IO   uint32_t  z;     //3
    __O    uint32_t  trig;  //4     
    __I    uint32_t  c;     //5 
    __I    uint32_t  d;     //6 

} MDU_TypeDef;

typedef struct
{
    __IO   uint32_t  En;    //0  
    __IO   uint32_t  ld_val;    //1  
    __IO   uint32_t  int_val;   //2
    __IO   uint32_t  cur_cnt;   //3     
    __IO   uint32_t  wo;   //4 

} RTC_TypeDef;

typedef struct
{
    __IO   uint32_t  cr;    //0  
    __IO   uint32_t  sr;    //1  
    __IO   uint32_t  out;   //2
    __IO   uint32_t  cmr;   //3     
    __IO   uint32_t  arr;   //4 
    __IO   uint32_t  comr;  //5 
    __IO   uint32_t  dr;    //6 
    __IO   uint32_t  dtr;   //7 
    __IO   uint32_t  cntr;  //8 

} DRV_TypeDef;



typedef struct
{
    __IO   uint32_t  ctrl0;         //0  
    __IO   uint32_t  ctrl1;         //1  
    __IO   uint32_t  Ud;            //2  
    __IO   uint32_t  Ud_plus;       //3 
    __IO   uint32_t  Ud_lim;        //4
    __IO   uint32_t  Uq_lim;        //5
    __IO   uint32_t  Udc;           //6
    __IO   uint32_t  gain;          //7
    __IO   uint32_t  Udc_coef;      //8
    __IO   uint32_t  duty_lim;      //9
    __IO   uint32_t  sample_time;   //a
    __IO   uint32_t  dc_Iuv;        //b
    __IO   uint32_t  dc_Iw;         //c
    __IO   uint32_t  Id_req;        //d
    __IO   uint32_t  pi;            //e
    __IO   uint32_t  pi_shift;      //f

    __IO   uint32_t  angl;          //10
    __IO   uint32_t  ramp1;         //11
    __IO   uint32_t  ramp2;         //12
    __IO   uint32_t  angl_comp;     //13
    __IO   uint32_t  fe_coef;       //14
    __IO   uint32_t  fe_base;       //15
	  __IO   uint32_t  fe_max;      //16
    __IO   uint32_t  fe_ramp;       //17
    __IO   uint32_t  fe_pi;         //18
    __IO   uint32_t  emf_coef;      //19
    __IO   uint32_t  smo_h;         //1a
    __IO   uint32_t  ek12;          //1b
    __IO   uint32_t  ek34;          //1c
    __IO   uint32_t  kfg;           //1d
    __IO   uint32_t  dglch;         //1e
    __IO   uint32_t  E_coef_manual; //1f

    __O    uint32_t  wo;            //20
    __I    uint32_t  ro_small;      //21
    __I    uint32_t  Ro_az0;        //22 
    __I    uint32_t  Ro_az12;       //23 			
    __I    uint32_t  Ro_emf4;       //24
    __I    uint32_t  Ro_emf3;       //25		
    __I    uint32_t  ro_atan;       //26
    __I    uint32_t  ro_fe3;        //27 删除了该寄存器[15:0]短滤波 Fe_short
    __I    uint32_t  ro_fe2;        //28
    __I    uint32_t  ro_emf2;       //29 删除了该寄存器
    __I    uint32_t  ro_emf1;       //2a
    __I    uint32_t  Ro_I_est;      //2b
    __I    uint32_t  ro_angl_auto;  //2c
    __I    uint32_t  ro_fe;         //2d 删除了该寄存器
    __I    uint32_t  ro_aac;        //2e 删除了该寄存器[15：0]
    __I    uint32_t  ro_angl;       //2f 删除了该寄存器[15：0]

    __I    uint32_t  ro_U23;        //30 删除了该寄存器
    __I    uint32_t  ro_U12;        //31
    __I    uint32_t  ro_Udc_filt;   //32
    __I    uint32_t  ro_Udex;       //33
    __I    uint32_t  ro_Iuv_raw;    //34
    __I    uint32_t  ro_Ud;         //35
    __I    uint32_t  ro_Ualf;       //36
    __I    uint32_t  ro_Udq2;       //37
    __I    uint32_t  ro_Udq;        //38
    __I    uint32_t  ro_Id;         //39
    __I    uint32_t  ro_Ialf;       //3a
    __I    uint32_t  ro_Iw_max;     //3b
    __I    uint32_t  ro_Iuv_max;    //3c
    __I    uint32_t  ro_Ivw;        //3d 删除了该寄存器[15：0]
    __I    uint32_t  ro_Iuv;        //3e
    __I    uint32_t  ro_Iuvw;       //3f 删除了该寄存器
	
	__I    uint32_t  ro_fuvw;       //40 删除了该寄存器
	__I    uint32_t  ro_fuv;        //41
	__I    uint32_t  ro_fvw;        //42 删除了该寄存器[12：0]
	__I    uint32_t  ro_cntU;       //43
	__I    uint32_t  ro_cntV;       //44
	__I    uint32_t  ro_cntW;       //45
	__I    uint32_t  ro_angl_after; //46
	__I    uint32_t  ro_Iuv_raw_before;//47
	__I    uint32_t  ro_Ixy_raw;    //48
	__IO   uint32_t  Rev_49;        //49
	__IO   uint32_t  Rev_4a;        //4a
	__IO   uint32_t  Rev_4b;        //4b
	__IO   uint32_t  Rev_4c;        //4c
	__IO   uint32_t  Rev_4d;        //4d
	__IO   uint32_t  Rev_4e;        //4e
	__IO   uint32_t  Rev_4f;        //4f

	__IO   uint32_t  wr_Udc_gain;    //50
	__IO   uint32_t  wr_Udq_lim;     //51

} FOC_TypeDef;



#pragma no_anon_unions




/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/

/* AHB peripherals */
#define APB_BASE          (0x40000000UL)
#define GPIO_BASE         (0x40010000UL)


/* APB peripherals */
#define WDT_BASE          (APB_BASE + 0x0000UL)
#define RTC_BASE          (APB_BASE + 0x0400UL)
#define PMU_BASE          (APB_BASE + 0x0800UL)
#define ADCCTRL_BASE      (APB_BASE + 0x0c00UL)
#define IO_BASE           (APB_BASE + 0x1400UL)
#define UART0_BASE        (APB_BASE + 0x1800UL)
#define UART1_BASE        (APB_BASE + 0x1c00UL)
#define DIGCTRL_BASE      (APB_BASE + 0x2400UL)
#define TIM0_BASE         (APB_BASE + 0x3000UL)
#define TIM1_BASE         (APB_BASE + 0x3400UL)
#define TIM2_BASE         (APB_BASE + 0x3800UL)
#define TIM3_BASE         (APB_BASE + 0x3c00UL)
#define TIM4_BASE         (APB_BASE + 0x4000UL)
#define MDU_BASE          (APB_BASE + 0x4800UL)
#define QSPI_BASE         (APB_BASE + 0x9000UL)
#define ADCINF_BASE       (APB_BASE + 0x9800UL)
#define DRV_BASE          (APB_BASE + 0x2800UL)
#define FOC_BASE          (APB_BASE + 0x4c00UL)

#define TIM5_BASE         (APB_BASE + 0x5C00UL)
#define TIM6_BASE         (APB_BASE + 0x6000UL)
/******************************************************************/
/*                         Peripheral declaration                 */
/******************************************************************/
#define GPIO              ((GPIO_TypeDef *)         GPIO_BASE       )
#define WDT               ((WDT_TypeDef  *)         WDT_BASE   	    )
#define PMU               ((PMU_TypeDef  *)         PMU_BASE   	    )
#define IO                ((IO_TypeDef *)           IO_BASE  	    )
#define UART0             ((UART_TypeDef *)         UART0_BASE 		)
#define UART1             ((UART_TypeDef *)         UART1_BASE 		)
#define TIM0              ((TIM0_TypeDef *)         TIM0_BASE       )
#define TIM1              ((TIM1_TypeDef *)         TIM1_BASE       )
#define TIM2              ((TIM2_TypeDef *)         TIM2_BASE       )
#define TIM3              ((TIM3_TypeDef *)         TIM3_BASE       )	
#define TIM4              ((TIM4_TypeDef *)         TIM4_BASE       )
#define TIM5              ((TIM5_TypeDef *)         TIM5_BASE       )
#define TIM6              ((TIM6_TypeDef *)         TIM6_BASE       )
#define MDU               ((MDU_TypeDef *)          MDU_BASE        )	
#define DIGCTRL           ((DIGCTRL_TypeDef *)      DIGCTRL_BASE    )   
#define QSPI              ((QSPI_TypeDef *)         QSPI_BASE  		)
#define ADCCTRL           ((ADCCTRL_TypeDef *)      ADCCTRL_BASE    )
#define ADCINF            ((ADCINF_TypeDef *)       ADCINF_BASE     )
#define DRV               ((DRV_TypeDef *)          DRV_BASE        )
#define FOC               ((FOC_TypeDef *)          FOC_BASE        )
#define RTC               ((RTC_TypeDef *)          RTC_BASE        )


#endif  /* CMSDK_H */
