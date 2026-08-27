#ifndef MCPWM_REG_H
#define MCPWM_REG_H


typedef struct
{
    __IO uint32_t TH00      ; /**< 偏移地址: 0x00 MCPWM CH0_P 比较门限值寄存器*/
    __IO uint32_t TH01      ; /**< 偏移地址: 0x04 MCPWM CH0_N 比较门限值寄存器*/
    __IO uint32_t TH10      ; /**< 偏移地址: 0x08 MCPWM CH1_P 比较门限值寄存器*/
    __IO uint32_t TH11      ; /**< 偏移地址: 0x0C MCPWM CH1_N 比较门限值寄存器*/
																																		
    __IO uint32_t TH20      ; /**< 偏移地址: 0x10 MCPWM CH2_P 比较门限值寄存器*/
    __IO uint32_t TH21      ; /**< 偏移地址: 0x14 MCPWM CH2_N 比较门限值寄存器*/
    __IO uint32_t TH30      ; /**< 偏移地址: 0x18 MCPWM CH3_P 比较门限值寄存器*/
    __IO uint32_t TH31      ; /**< 偏移地址: 0x1C MCPWM CH3_N 比较门限值寄存器*/
																																		
    __IO uint32_t TH40      ; /**< 偏移地址: 0x20 MCPWM CH4_P 比较门限值寄存器*/
    __IO uint32_t TH41      ; /**< 偏移地址: 0x24 MCPWM CH4_N 比较门限值寄存器*/
    __IO uint32_t TH50      ; /**< 偏移地址: 0x28 MCPWM CH5_P 比较门限值寄存器*/
    __IO uint32_t TH51      ; /**< 偏移地址: 0x2C MCPWM CH5_N 比较门限值寄存器*/
												
    __IO uint32_t TMR0      ; /**< 偏移地址: 0x30 ADC 采样定时器比较门限 0 寄存器*/
    __IO uint32_t TMR1      ; /**< 偏移地址: 0x34 ADC 采样定时器比较门限 1 寄存器*/
    __IO uint32_t TMR2      ; /**< 偏移地址: 0x38 ADC 采样定时器比较门限 2 寄存器*/
    __IO uint32_t TMR3      ; /**< 偏移地址: 0x3C ADC 采样定时器比较门限 3 寄存器*/
												
    __IO uint32_t TH0       ; /**< 偏移地址: 0x40 MCPWM 时基 0 门限值寄存器*/
    __IO uint32_t TH1       ; /**< 偏移地址: 0x44 MCPWM 时基 1 门限值寄存器*/
    __IO uint32_t CNT0      ; /**< 偏移地址: 0x48 MCPWM 时基 0 计数器寄存器*/
    __IO uint32_t CNT1      ; /**< 偏移地址: 0x4C MCPWM 时基 1 计数器寄存器*/
										
    __IO uint32_t UPDATE    ; /**< 偏移地址: 0x50 MCPWM 加载控制寄存器*/
    __IO uint32_t FCNT      ; /**< 偏移地址: 0x54 MCPWM FAIL 时刻 CNT 值*/
    __IO uint32_t EVT0      ; /**< 偏移地址: 0x58 MCPWM 时基 0 外部触发*/
    __IO uint32_t EVT1      ; /**< 偏移地址: 0x5C MCPWM 时基 1 外部触发*/
												
    __IO uint32_t DTH00     ; /**< 偏移地址: 0x60 MCPWM CH0/1/2 N 通道死区宽度控制寄存器*/
    __IO uint32_t DTH01     ; /**< 偏移地址: 0x64 MCPWM CH0/1/2 P 通道死区宽度控制寄存器*/
    __IO uint32_t DTH10     ; /**< 偏移地址: 0x68 MCPWM CH3/4/5 N 通道死区宽度控制寄存器*/
    __IO uint32_t DTH11     ; /**< 偏移地址: 0x6C MCPWM CH3/4/5 P 通道死区宽度控制寄存器*/
												
    __IO uint32_t FLT       ; /**< 偏移地址: 0x70 MCPWM 滤波时钟分频寄存器*/
    __IO uint32_t SDCFG     ; /**< 偏移地址: 0x74 MCPWM 加载配置寄存器*/
    __IO uint32_t AUEN      ; /**< 偏移地址: 0x78 MCPWM 自动更新使能寄存器*/
    __IO uint32_t TCLK      ; /**< 偏移地址: 0x7C MCPWM 时钟分频控制寄存器*/
										
    __IO uint32_t IE0       ; /**< 偏移地址: 0x80 MCPWM 时基 0 中断控制寄存器*/
    __IO uint32_t IF0       ; /**< 偏移地址: 0x84 MCPWM 时基 0 中断标志位寄存器*/
    __IO uint32_t IE1       ; /**< 偏移地址: 0x88 MCPWM 中断控制寄存器*/
    __IO uint32_t IF1       ; /**< 偏移地址: 0x8C MCPWM 中断标志位寄存器*/
											
    __IO uint32_t EIE       ; /**< 偏移地址: 0x90 MCPWM 异常中断控制寄存器*/
    __IO uint32_t EIF       ; /**< 偏移地址: 0x94 MCPWM 异常中断标志位寄存器*/
    __IO uint32_t RE        ; /**< 偏移地址: 0x98 MCPWM DMA 请求控制寄存器*/
    __IO uint32_t PP        ; /**< 偏移地址: 0x9C MCPWM 推挽模式使能寄存器*/
									
    __IO uint32_t IO01      ; /**< 偏移地址: 0xA0 MCPWM CH0 CH1 IO 控制寄存器*/
    __IO uint32_t IO23      ; /**< 偏移地址: 0xA4 MCPWM CH2 CH3 IO 控制寄存器*/
    __IO uint32_t IO45      ; /**< 偏移地址: 0xA8 MCPWM CH4 CH5 IO 控制寄存器*/
    __IO uint32_t MASK      ; /**< 偏移地址: 0xAC 地址对齐 MCPWM 掩码输出控制寄存器 */
											
    __IO uint32_t FAIL012   ; /**< 偏移地址: 0xB0 MCPWM CH0 CH1 CH2 短路控制寄存器*/
    __IO uint32_t FAIL345   ; /**< 偏移地址: 0xB4 MCPWM CH3 CH4 CH5 短路控制寄存器*/
    __IO uint32_t CH_DEF    ; /**< 偏移地址: 0xB8 MCPWM 短路保护通道输出默认值*/
    __IO uint32_t CH_MSK    ; /**< 偏移地址: 0xBC MCPWM 通道屏蔽寄存器*/
												
    __IO uint32_t PRT       ; /**< 偏移地址: 0xC0 MCPWM 保护寄存器*/
    __IO uint32_t STT_HYST  ; /**< 偏移地址: 0xC8 MCPWM 状态停留延时寄存器*/
    __IO uint32_t ZCS_DELAY ; /**< 偏移地址: 0xCC MCPWM ZCS 状态延时寄存器*/
}MCPWM_TypeDef;

#define MCPWM0         ((MCPWM_TypeDef *) MCPWM0_BASE)

// 正确的定义 - 必须使用联合体(union)
typedef union {
    struct {
        __IO uint32_t  reserved_bit0     : 1;   /* bit0  */
        __IO uint32_t  reserved_bit1     : 1;   /* bit1  */
        __IO uint32_t  CH0_NS_bit2       : 1;   /* bit2  */
        __IO uint32_t  CH0_PS_bit3       : 1;   /* bit3  */
        __IO uint32_t  CH0_SCTRLN_bit4   : 1;   /* bit4  */
        __IO uint32_t  CH0_SCTRLP_bit5   : 1;   /* bit5  */
        __IO uint32_t  reserved_bit6     : 1;   /* bit6  */
        __IO uint32_t  reserved_bit7     : 1;   /* bit7  */
        __IO uint32_t  reserved_bit8     : 1;   /* bit8  */
        __IO uint32_t  reserved_bit9     : 1;   /* bit9  */
        __IO uint32_t  CH1_NS_bit10      : 1;   /* bit10  */  
        __IO uint32_t  CH1_PS_bit11      : 1;   /* bit11  */  
        __IO uint32_t  CH1_SCTRLN_bit12  : 1;   /* bit12  */  
        __IO uint32_t  CH1_SCTRLP_bit13  : 1;   /* bit13  */  
        __IO uint32_t  reserved_bit14    : 1;   /* bit14 */
        __IO uint32_t  reserved_bit15    : 1;   /* bit15 */
        __IO uint32_t  reserved_bit16_31 : 1;   /* bit16-31 */
    } bits;
    
    // 必须添加这个，才能作为uint32_t整体访问
    __IO uint32_t w;
} MCPWM0_IO01_BIT32_TypeDef;


// 正确的定义 - 必须使用联合体(union)
typedef union {
    struct {
        __IO uint32_t  reserved_bit0     : 1;   /* bit0  */
        __IO uint32_t  reserved_bit1     : 1;   /* bit1  */
        __IO uint32_t  CH2_NS_bit2       : 1;   /* bit2  */
        __IO uint32_t  CH2_PS_bit3       : 1;   /* bit3  */
        __IO uint32_t  CH2_SCTRLN_bit4   : 1;   /* bit4  */
        __IO uint32_t  CH2_SCTRLP_bit5   : 1;   /* bit5  */
        __IO uint32_t  reserved_bit6     : 1;   /* bit6  */
        __IO uint32_t  reserved_bit7     : 1;   /* bit7  */
        __IO uint32_t  reserved_bit8     : 1;   /* bit8  */
        __IO uint32_t  reserved_bit9     : 1;   /* bit9  */
        __IO uint32_t  CH3_NS_bit10      : 1;   /* bit10  */  
        __IO uint32_t  CH3_PS_bit11      : 1;   /* bit11  */  
        __IO uint32_t  CH3_SCTRLN_bit12  : 1;   /* bit12  */  
        __IO uint32_t  CH3_SCTRLP_bit13  : 1;   /* bit13  */  
        __IO uint32_t  reserved_bit14    : 1;   /* bit14 */
        __IO uint32_t  reserved_bit15    : 1;   /* bit15 */
        __IO uint32_t  reserved_bit16_31 : 1;   /* bit16-31 */
    } bits;
    
    // 必须添加这个，才能作为uint32_t整体访问
    __IO uint32_t w;
} MCPWM0_IO23_BIT32_TypeDef;


// 正确的定义 - 必须使用联合体(union)
typedef union {
    struct {
        __IO uint32_t  reserved_bit0     : 1;   /* bit0  */
        __IO uint32_t  reserved_bit1     : 1;   /* bit1  */
        __IO uint32_t  CH4_NS_bit2       : 1;   /* bit2  */
        __IO uint32_t  CH4_PS_bit3       : 1;   /* bit3  */
        __IO uint32_t  CH4_SCTRLN_bit4   : 1;   /* bit4  */
        __IO uint32_t  CH4_SCTRLP_bit5   : 1;   /* bit5  */
        __IO uint32_t  reserved_bit6     : 1;   /* bit6  */
        __IO uint32_t  reserved_bit7     : 1;   /* bit7  */
        __IO uint32_t  reserved_bit8     : 1;   /* bit8  */
        __IO uint32_t  reserved_bit9     : 1;   /* bit9  */
        __IO uint32_t  CH5_NS_bit10      : 1;   /* bit10  */  
        __IO uint32_t  CH5_PS_bit11      : 1;   /* bit11  */  
        __IO uint32_t  CH5_SCTRLN_bit12  : 1;   /* bit12  */  
        __IO uint32_t  CH5_SCTRLP_bit13  : 1;   /* bit13  */  
        __IO uint32_t  reserved_bit14    : 1;   /* bit14 */
        __IO uint32_t  reserved_bit15    : 1;   /* bit15 */
        __IO uint32_t  reserved_bit16_31 : 1;   /* bit16-31 */
    } bits;
    
    // 必须添加这个，才能作为uint32_t整体访问
    __IO uint32_t w;
} MCPWM0_IO45_BIT32_TypeDef;


// 正确的定义 - 必须使用联合体(union)
typedef union {
    struct {
        __IO uint32_t  CH0_NS_bit0          : 1;   /* bit0  */
        __IO uint32_t  CH0_PS_bit1          : 1;   /* bit1  */
        __IO uint32_t  CH1_NS_bit2          : 1;   /* bit2  */
        __IO uint32_t  CH1_PS_bit3          : 1;   /* bit3  */
        __IO uint32_t  CH2_NS_bit4          : 1;   /* bit4  */
        __IO uint32_t  CH2_PS_bit5          : 1;   /* bit5  */
        __IO uint32_t  reserved_bit6        : 1;   /* bit6  */
        __IO uint32_t  reserved_bit7        : 1;   /* bit7  */
        __IO uint32_t  CH0_SCTRLN_bit8      : 1;   /* bit8  */
        __IO uint32_t  CH0_SCTRLP_bit9      : 1;   /* bit9  */
        __IO uint32_t  CH1_SCTRLN_bit10     : 1;   /* bit10 */
        __IO uint32_t  CH1_SCTRLP_bit11     : 1;   /* bit11 */
        __IO uint32_t  CH2_SCTRLN_bit12     : 1;   /* bit12 */
        __IO uint32_t  CH2_SCTRLP_bit13     : 1;   /* bit13 */
        __IO uint32_t  reserved_bit14       : 1;   /* bit14 */
        __IO uint32_t  reserved_bit15       : 1;   /* bit15 */
        __IO uint32_t  CH3_NS_bit16         : 1;   /* bit16 */
        __IO uint32_t  CH3_PS_bit17         : 1;   /* bit17 */
        __IO uint32_t  CH4_NS_bit18         : 1;   /* bit18 */
        __IO uint32_t  CH4_PS_bit19         : 1;   /* bit19 */
        __IO uint32_t  CH5_NS_bit20         : 1;   /* bit20 */
        __IO uint32_t  CH5_PS_bit21         : 1;   /* bit21 */
        __IO uint32_t  reserved_bit22       : 1;   /* bit22 */
        __IO uint32_t  reserved_bit23       : 1;   /* bit23 */
        __IO uint32_t  CH3_SCTRLN_bit24     : 1;   /* bit24 */
        __IO uint32_t  CH3_SCTRLP_bit25     : 1;   /* bit25 */
        __IO uint32_t  CH4_SCTRLN_bit26     : 1;   /* bit26 */
        __IO uint32_t  CH4_SCTRLP_bit27     : 1;   /* bit27 */
        __IO uint32_t  CH5_SCTRLN_bit28     : 1;   /* bit28 */
        __IO uint32_t  CH5_SCTRLP_bit29     : 1;   /* bit29 */
        __IO uint32_t  reserved_bit30       : 1;   /* bit30 */
        __IO uint32_t  reserved_bit31       : 1;   /* bit31 */
    } bits;
    
    // 必须添加这个，才能作为uint32_t整体访问
    __IO uint32_t w;
} MASK_BIT32_TypeDef;


#define MCPWM0_TH00         REG32(MCPWM0_BASE + 0x00) 
#define MCPWM0_TH01         REG32(MCPWM0_BASE + 0x04)
#define MCPWM0_TH10         REG32(MCPWM0_BASE + 0x08)
#define MCPWM0_TH11         REG32(MCPWM0_BASE + 0x0C)
#define MCPWM0_TH20         REG32(MCPWM0_BASE + 0x10)
#define MCPWM0_TH21         REG32(MCPWM0_BASE + 0x14)
#define MCPWM0_TH30         REG32(MCPWM0_BASE + 0x18)
#define MCPWM0_TH31         REG32(MCPWM0_BASE + 0x1C)
#define MCPWM0_TH40         REG32(MCPWM0_BASE + 0x20)
#define MCPWM0_TH41         REG32(MCPWM0_BASE + 0x24)
#define MCPWM0_TH50         REG32(MCPWM0_BASE + 0x28)
#define MCPWM0_TH51         REG32(MCPWM0_BASE + 0x2C)
#define MCPWM0_TMR0         REG32(MCPWM0_BASE + 0x30)
#define MCPWM0_TMR1         REG32(MCPWM0_BASE + 0x34)
#define MCPWM0_TMR2         REG32(MCPWM0_BASE + 0x38)
#define MCPWM0_TMR3         REG32(MCPWM0_BASE + 0x3C)
#define MCPWM0_TH0          REG32(MCPWM0_BASE + 0x40)
#define MCPWM0_TH1          REG32(MCPWM0_BASE + 0x44)
#define MCPWM0_CNT0         REG32(MCPWM0_BASE + 0x48)
#define MCPWM0_CNT1         REG32(MCPWM0_BASE + 0x4C)
#define MCPWM0_UPDATE       REG32(MCPWM0_BASE + 0x50)
#define MCPWM0_FCNT         REG32(MCPWM0_BASE + 0x54)
#define MCPWM0_EVT0         REG32(MCPWM0_BASE + 0x58)
#define MCPWM0_EVT1         REG32(MCPWM0_BASE + 0x5C)
#define MCPWM0_DTH00        REG32(MCPWM0_BASE + 0x60)
#define MCPWM0_DTH01        REG32(MCPWM0_BASE + 0x64)
#define MCPWM0_DTH10        REG32(MCPWM0_BASE + 0x68)
#define MCPWM0_DTH11        REG32(MCPWM0_BASE + 0x6C)

#define MCPWM0_FLT          REG32(MCPWM0_BASE + 0x70)
#define MCPWM0_SDCFG        REG32(MCPWM0_BASE + 0x74)
#define MCPWM0_AUEN         REG32(MCPWM0_BASE + 0x78)
#define MCPWM0_TCLK         REG32(MCPWM0_BASE + 0x7C)
#define MCPWM0_IE0          REG32(MCPWM0_BASE + 0x80)
#define MCPWM0_IF0          REG32(MCPWM0_BASE + 0x84)
#define MCPWM0_IE1          REG32(MCPWM0_BASE + 0x88)
#define MCPWM0_IF1          REG32(MCPWM0_BASE + 0x8C)
#define MCPWM0_EIE          REG32(MCPWM0_BASE + 0x90)
#define MCPWM0_EIF          REG32(MCPWM0_BASE + 0x94)
#define MCPWM0_RE           REG32(MCPWM0_BASE + 0x98)
#define MCPWM0_PP           REG32(MCPWM0_BASE + 0x9C)
#define MCPWM0_IO01         REG32(MCPWM0_BASE + 0xA0)
#define MCPWM0_IO23         REG32(MCPWM0_BASE + 0xA4)
#define MCPWM0_IO45         REG32(MCPWM0_BASE + 0xA8)
#define MCPWM0_MASK         REG32(MCPWM0_BASE + 0xAC)
#define MCPWM0_MASK_LOW     (*(volatile uint16_t *)(MCPWM0_BASE + 0xAC))
#define MCPWM0_MASK_HIGH    (*(volatile uint16_t *)(MCPWM0_BASE + 0xAE))


#define MCPWM0_FAIL012      REG32(MCPWM0_BASE + 0xB0)
#define MCPWM0_FAIL345      REG32(MCPWM0_BASE + 0xB4)
#define MCPWM0_CH_DEF       REG32(MCPWM0_BASE + 0xB8)
#define MCPWM0_CH_MASK      REG32(MCPWM0_BASE + 0xBC)
#define MCPWM0_PRT          REG32(MCPWM0_BASE + 0xC0) //0x40010CC0
#define MCPWM0_STT_HYST     REG32(MCPWM0_BASE + 0xC8) //0x40010CC8
#define MCPWM0_ZCS_DELAY    REG32(MCPWM0_BASE + 0xCC) //0x40010CCC

#define MCPWM0_CH012_BLDC   REG32(MCPWM0_BASE + 0xCC) //0x40010CCC //待修改地址
#define MCPWM0_CH345_BLDC   REG32(MCPWM0_BASE + 0xCC) //0x40010CCC //待修改地址

//channel-0
 #define MCPWM0_IO01_CH0P_EN_MASK                 ((uint32_t)0x8)       //bit [3]
 #define MCPWM0_IO01_CH0P_EN_POS                  ((uint32_t)0x3)       //bit [3]
 #define MCPWM0_IO01_CH0P_EN_MASK_SET(x)          (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO01_CH0P_EN_POS))&MCPWM0_IO01_CH0P_EN_MASK)

 #define MCPWM0_IO01_CH0P_VALUE_MASK               ((uint32_t)0x20)       //bit [5]
 #define MCPWM0_IO01_CH0P_VALUE_POS                ((uint32_t)0x5)        //bit [5]
 #define MCPWM0_IO01_CH0P_VALUE_MASK_SET(x)        (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO01_CH0P_VALUE_POS))&MCPWM0_IO01_CH0P_VALUE_MASK)

 #define MCPWM0_IO01_CH0N_EN_MASK                 ((uint32_t)0x4)       //bit [2]
 #define MCPWM0_IO01_CH0N_EN_POS                  ((uint32_t)0x2)       //bit [2]
 #define MCPWM0_IO01_CH0N_EN_MASK_SET(x)          (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO01_CH0N_EN_POS))&MCPWM0_IO01_CH0N_EN_MASK)

 #define MCPWM0_IO01_CH0N_VALUE_MASK               ((uint32_t)0x10)       //bit [4]
 #define MCPWM0_IO01_CH0N_VALUE_POS                ((uint32_t)0x4)        //bit [4]
 #define MCPWM0_IO01_CH0N_VALUE_MASK_SET(x)        (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO01_CH0N_VALUE_POS))&MCPWM0_IO01_CH0N_VALUE_MASK)

//channel-1
 #define MCPWM0_IO01_CH1P_EN_MASK                 ((uint32_t)0x800)       //bit [11]
 #define MCPWM0_IO01_CH1P_EN_POS                  ((uint32_t)0xB)       //bit [11]
 #define MCPWM0_IO01_CH1P_EN_MASK_SET(x)          (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO01_CH1P_EN_POS))&MCPWM0_IO01_CH1P_EN_MASK)

 #define MCPWM0_IO01_CH1P_VALUE_MASK               ((uint32_t)0x2000)       //bit [13]
 #define MCPWM0_IO01_CH1P_VALUE_POS                ((uint32_t)0xD)        //bit [13]
 #define MCPWM0_IO01_CH1P_VALUE_MASK_SET(x)        (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO01_CH1P_VALUE_POS))&MCPWM0_IO01_CH1P_VALUE_MASK)

 #define MCPWM0_IO01_CH1N_EN_MASK                 ((uint32_t)0x400)       //bit [10]
 #define MCPWM0_IO01_CH1N_EN_POS                  ((uint32_t)0xA)       //bit [10]
 #define MCPWM0_IO01_CH1N_EN_MASK_SET(x)          (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO01_CH1N_EN_POS))&MCPWM0_IO01_CH1N_EN_MASK)

 #define MCPWM0_IO01_CH1N_VALUE_MASK               ((uint32_t)0x1000)       //bit [12]
 #define MCPWM0_IO01_CH1N_VALUE_POS                ((uint32_t)0xC)        //bit [12]
 #define MCPWM0_IO01_CH1N_VALUE_MASK_SET(x)        (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO01_CH1N_VALUE_POS))&MCPWM0_IO01_CH1N_VALUE_MASK)


//channel-2
 #define MCPWM0_IO23_CH2P_EN_MASK                 ((uint32_t)0x8)       //bit [3]
 #define MCPWM0_IO23_CH2P_EN_POS                  ((uint32_t)0x3)       //bit [3]
 #define MCPWM0_IO23_CH2P_EN_MASK_SET(x)          (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO23_CH2P_EN_POS))&MCPWM0_IO23_CH2P_EN_MASK)

 #define MCPWM0_IO23_CH2P_VALUE_MASK               ((uint32_t)0x20)       //bit [5]
 #define MCPWM0_IO23_CH2P_VALUE_POS                ((uint32_t)0x5)        //bit [5]
 #define MCPWM0_IO23_CH2P_VALUE_MASK_SET(x)        (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO23_CH2P_VALUE_POS))&MCPWM0_IO23_CH2P_VALUE_MASK)

 #define MCPWM0_IO23_CH2N_EN_MASK                 ((uint32_t)0x4)       //bit [2]
 #define MCPWM0_IO23_CH2N_EN_POS                  ((uint32_t)0x2)       //bit [2]
 #define MCPWM0_IO23_CH2N_EN_MASK_SET(x)          (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO23_CH2N_EN_POS))&MCPWM0_IO23_CH2N_EN_MASK)

 #define MCPWM0_IO23_CH2N_VALUE_MASK               ((uint32_t)0x10)       //bit [4]
 #define MCPWM0_IO23_CH2N_VALUE_POS                ((uint32_t)0x4)        //bit [4]
 #define MCPWM0_IO23_CH2N_VALUE_MASK_SET(x)        (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO23_CH2N_VALUE_POS))&MCPWM0_IO23_CH2N_VALUE_MASK)

/////////////////////////////////////////////////////////////////////////////////////

 #define MCPWM0_CH_DEF_CH012_MASK                ((uint32_t)0x3f)       //bit [5:0]
 #define MCPWM0_CH_DEF_CH012_POS                 ((uint32_t)0x0)        //bit [0]
 #define MCPWM0_CH_DEF_CH012_SET(x)              (((uint32_t)(((uint32_t)(x))<<MCPWM0_CH_DEF_CH012_POS))&MCPWM0_CH_DEF_CH012_MASK)

 #define MCPWM0_CH_DEF_CH345_MASK                ((uint32_t)0xfc0)       //bit [11:6]
 #define MCPWM0_CH_DEF_CH345_POS                 ((uint32_t)0x6)         //bit [6]
 #define MCPWM0_CH_DEF_CH345_SET(x)              (((uint32_t)(((uint32_t)(x))<<MCPWM0_CH_DEF_CH345_POS))&MCPWM0_CH_DEF_CH345_MASK)

/////////////////////////////////////////////////////////////////////////////////////
//channel 0
#define CH0P_EN_BIT				(BIT3)
#define CH0P_VALUE_BIT			(BIT5)
#define CH0N_EN_BIT				(BIT2)
#define CH0N_VALUE_BIT			(BIT4)

//channel 1
#define CH1P_EN_BIT				(BIT11)
#define CH1P_VALUE_BIT			(BIT13)
#define CH1N_EN_BIT				(BIT10)
#define CH1N_VALUE_BIT			(BIT12)

//channel 2
#define CH2P_EN_BIT				(BIT3)
#define CH2P_VALUE_BIT			(BIT5)
#define CH2N_EN_BIT				(BIT2)
#define CH2N_VALUE_BIT			(BIT4)
/////////////////////////////////////////////////////////////////////////////////////
//channel-0-p
 #define MCPWM0_IO01_CH0P_INVERSE_MASK            ((uint32_t)0x2)       //bit [1]
 #define MCPWM0_IO01_CH0P_INVERSE_POS             ((uint32_t)0x1)       //bit [1]
 #define MCPWM0_IO01_CH0P_INVERSE_MASK_SET(x)     (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO01_CH0P_INVERSE_POS))&MCPWM0_IO01_CH0P_INVERSE_MASK)

//channel-0-n
 #define MCPWM0_IO01_CH0N_INVERSE_MASK            ((uint32_t)0x1)       //bit [0]
 #define MCPWM0_IO01_CH0N_INVERSE_POS             ((uint32_t)0x0)       //bit [0]
 #define MCPWM0_IO01_CH0N_INVERSE_MASK_SET(x)     (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO01_CH0N_INVERSE_POS))&MCPWM0_IO01_CH0N_INVERSE_MASK)

 //channel-1-p
 #define MCPWM0_IO01_CH1P_INVERSE_MASK            ((uint32_t)0x200)       //bit [9]
 #define MCPWM0_IO01_CH1P_INVERSE_POS             ((uint32_t)0x9)         //bit [9]
 #define MCPWM0_IO01_CH1P_INVERSE_MASK_SET(x)     (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO01_CH1P_INVERSE_POS))&MCPWM0_IO01_CH1P_INVERSE_MASK)

//channel-1-n
 #define MCPWM0_IO01_CH1N_INVERSE_MASK            ((uint32_t)0x100)       //bit [8]
 #define MCPWM0_IO01_CH1N_INVERSE_POS             ((uint32_t)0x8)         //bit [8]
 #define MCPWM0_IO01_CH1N_INVERSE_MASK_SET(x)     (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO01_CH1N_INVERSE_POS))&MCPWM0_IO01_CH1N_INVERSE_MASK)

 //channel-2-p
 #define MCPWM0_IO23_CH2P_INVERSE_MASK            ((uint32_t)0x2)       //bit [1]
 #define MCPWM0_IO23_CH2P_INVERSE_POS             ((uint32_t)0x1)       //bit [1]
 #define MCPWM0_IO23_CH2P_INVERSE_MASK_SET(x)     (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO23_CH2P_INVERSE_POS))&MCPWM0_IO23_CH2P_INVERSE_MASK)

//channel-2-n
 #define MCPWM0_IO23_CH2N_INVERSE_MASK            ((uint32_t)0x1)       //bit [0]
 #define MCPWM0_IO23_CH2N_INVERSE_POS             ((uint32_t)0x0)       //bit [0]
 #define MCPWM0_IO23_CH2N_INVERSE_MASK_SET(x)     (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO23_CH2N_INVERSE_POS))&MCPWM0_IO23_CH2N_INVERSE_MASK)

 //channel-3-p
 #define MCPWM0_IO23_CH3P_INVERSE_MASK            ((uint32_t)0x200)       //bit [9]
 #define MCPWM0_IO23_CH3P_INVERSE_POS             ((uint32_t)0x9)         //bit [9]
 #define MCPWM0_IO23_CH3P_INVERSE_MASK_SET(x)     (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO23_CH3P_INVERSE_POS))&MCPWM0_IO23_CH3P_INVERSE_MASK)

//channel-3-n
 #define MCPWM0_IO23_CH3N_INVERSE_MASK            ((uint32_t)0x100)       //bit [8]
 #define MCPWM0_IO23_CH3N_INVERSE_POS             ((uint32_t)0x8)         //bit [8]
 #define MCPWM0_IO23_CH3N_INVERSE_MASK_SET(x)     (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO23_CH3N_INVERSE_POS))&MCPWM0_IO23_CH3N_INVERSE_MASK)

  //channel-4-p
 #define MCPWM0_IO45_CH4P_INVERSE_MASK            ((uint32_t)0x2)       //bit [1]
 #define MCPWM0_IO45_CH4P_INVERSE_POS             ((uint32_t)0x1)       //bit [1]
 #define MCPWM0_IO45_CH4P_INVERSE_MASK_SET(x)     (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO45_CH4P_INVERSE_POS))&MCPWM0_IO45_CH4P_INVERSE_MASK)

//channel-4-n
 #define MCPWM0_IO45_CH4N_INVERSE_MASK            ((uint32_t)0x1)       //bit [0]
 #define MCPWM0_IO45_CH4N_INVERSE_POS             ((uint32_t)0x0)       //bit [0]
 #define MCPWM0_IO45_CH4N_INVERSE_MASK_SET(x)     (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO45_CH4N_INVERSE_POS))&MCPWM0_IO45_CH4N_INVERSE_MASK)

 //channel-5-p
 #define MCPWM0_IO45_CH5P_INVERSE_MASK            ((uint32_t)0x200)       //bit [9]
 #define MCPWM0_IO45_CH5P_INVERSE_POS             ((uint32_t)0x9)         //bit [9]
 #define MCPWM0_IO45_CH5P_INVERSE_MASK_SET(x)     (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO45_CH5P_INVERSE_POS))&MCPWM0_IO45_CH5P_INVERSE_MASK)

//channel-5-n
 #define MCPWM0_IO45_CH5N_INVERSE_MASK            ((uint32_t)0x100)       //bit [8]
 #define MCPWM0_IO45_CH5N_INVERSE_POS             ((uint32_t)0x8)         //bit [8]
 #define MCPWM0_IO45_CH5N_INVERSE_MASK_SET(x)     (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO45_CH5N_INVERSE_POS))&MCPWM0_IO45_CH5N_INVERSE_MASK)

 /////////////////////////////////////////////////////////////////////////////////////
  //channel-0-p-n
 #define MCPWM0_IO01_CH0NP_SWAP_MASK            ((uint32_t)0x40)       //bit [6]
 #define MCPWM0_IO01_CH0NP_SWAP_POS             ((uint32_t)0x6)        //bit [6]
 #define MCPWM0_IO01_CH0NP_SWAP_MASK_SET(x)     (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO01_CH0NP_SWAP_POS))&MCPWM0_IO01_CH0NP_SWAP_MASK)

   //channel-1-p-n
 #define MCPWM0_IO01_CH1NP_SWAP_MASK            ((uint32_t)0x4000)       //bit [14]
 #define MCPWM0_IO01_CH1NP_SWAP_POS             ((uint32_t)0xE)          //bit [14]
 #define MCPWM0_IO01_CH1NP_SWAP_MASK_SET(x)     (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO01_CH1NP_SWAP_POS))&MCPWM0_IO01_CH1NP_SWAP_MASK)

   //channel-2-p-n
 #define MCPWM0_IO23_CH2NP_SWAP_MASK            ((uint32_t)0x40)       //bit [6]
 #define MCPWM0_IO23_CH2NP_SWAP_POS             ((uint32_t)0x6)        //bit [6]
 #define MCPWM0_IO23_CH2NP_SWAP_MASK_SET(x)     (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO23_CH2NP_SWAP_POS))&MCPWM0_IO23_CH2NP_SWAP_MASK)

   //channel-3-p-n
 #define MCPWM0_IO23_CH3NP_SWAP_MASK            ((uint32_t)0x4000)       //bit [14]
 #define MCPWM0_IO23_CH3NP_SWAP_POS             ((uint32_t)0xE)          //bit [14]
 #define MCPWM0_IO23_CH3NP_SWAP_MASK_SET(x)     (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO23_CH3NP_SWAP_POS))&MCPWM0_IO23_CH3NP_SWAP_MASK)

    //channel-4-p-n
 #define MCPWM0_IO45_CH4NP_SWAP_MASK            ((uint32_t)0x40)       //bit [6]
 #define MCPWM0_IO45_CH4NP_SWAP_POS             ((uint32_t)0x6)        //bit [6]
 #define MCPWM0_IO45_CH4NP_SWAP_MASK_SET(x)     (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO45_CH4NP_SWAP_POS))&MCPWM0_IO45_CH4NP_SWAP_MASK)

   //channel-5-p-n
 #define MCPWM0_IO45_CH5NP_SWAP_MASK            ((uint32_t)0x4000)       //bit [14]
 #define MCPWM0_IO45_CH5NP_SWAP_POS             ((uint32_t)0xE)          //bit [14]
 #define MCPWM0_IO45_CH5NP_SWAP_MASK_SET(x)     (((uint32_t)(((uint32_t)(x))<<MCPWM0_IO45_CH5NP_SWAP_POS))&MCPWM0_IO45_CH5NP_SWAP_MASK)


 /////////////////////////////////////////////////////////////////////////////////////MCPWM0_FAIL012
 #define MCPWM0_FAIL1_SEL_MASK                  ((uint32_t)0xC00)        //bit [11:10]
 #define MCPWM0_FAIL1_SEL_POS                   ((uint32_t)0xA)          //bit [10]
 #define MCPWM0_FAIL1_SEL_MASK_SET(x)           (((uint32_t)(((uint32_t)(x))<<MCPWM0_FAIL1_SEL_POS))&MCPWM0_FAIL1_SEL_MASK)

 #define MCPWM0_FAIL0_SEL_MASK                  ((uint32_t)0x300)        //bit [9:8]
 #define MCPWM0_FAIL0_SEL_POS                   ((uint32_t)0x8)          //bit [8]
 #define MCPWM0_FAIL0_SEL_MASK_SET(x)           (((uint32_t)(((uint32_t)(x))<<MCPWM0_FAIL0_SEL_POS))&MCPWM0_FAIL0_SEL_MASK)

 #define MCPWM0_HALT_PRT_MASK                  ((uint32_t)0x80)         //bit [7]
 #define MCPWM0_HALT_PRT_POS                   ((uint32_t)0x7)          //bit [7]
 #define MCPWM0_HALT_PRT_MASK_SET(x)           (((uint32_t)(((uint32_t)(x))<<MCPWM0_HALT_PRT_POS))&MCPWM0_HALT_PRT_MASK)

 #define MCPWM0_MOE_MASK                       ((uint32_t)0x40)         //bit [6]
 #define MCPWM0_MOE_POS                        ((uint32_t)0x6)          //bit [6]
 #define MCPWM0_MOE_MASK_SET(x)                (((uint32_t)(((uint32_t)(x))<<MCPWM0_MOE_POS))&MCPWM0_MOE_MASK)

 #define MCPWM0_FAIL1_EN_MASK                  ((uint32_t)0x20)          //bit [5]
 #define MCPWM0_FAIL1_EN_POS                   ((uint32_t)0x5)           //bit [5]
 #define MCPWM0_FAIL1_EN_MASK_SET(x)           (((uint32_t)(((uint32_t)(x))<<MCPWM0_FAIL1_EN_POS))&MCPWM0_FAIL1_EN_MASK)

 #define MCPWM0_FAIL0_EN_MASK                  ((uint32_t)0x10)          //bit [4]
 #define MCPWM0_FAIL0_EN_POS                   ((uint32_t)0x4)           //bit [4]
 #define MCPWM0_FAIL0_EN_MASK_SET(x)           (((uint32_t)(((uint32_t)(x))<<MCPWM0_FAIL0_EN_POS))&MCPWM0_FAIL0_EN_MASK)

 #define MCPWM0_FAIL1_POL_MASK                 ((uint32_t)0x8)          //bit [3]
 #define MCPWM0_FAIL1_POL_POS                  ((uint32_t)0x3)           //bit [3]
 #define MCPWM0_FAIL1_POL_MASK_SET(x)          (((uint32_t)(((uint32_t)(x))<<MCPWM0_FAIL1_POL_POS))&MCPWM0_FAIL1_POL_MASK)

 #define MCPWM0_FAIL0_POL_MASK                 ((uint32_t)0x4)          //bit [2]
 #define MCPWM0_FAIL0_POL_POS                  ((uint32_t)0x2)           //bit [2]
 #define MCPWM0_FAIL0_POL_MASK_SET(x)          (((uint32_t)(((uint32_t)(x))<<MCPWM0_FAIL0_POL_POS))&MCPWM0_FAIL0_POL_MASK)
 
 /////////////////////////////////////////////////////////////////////////////////////MCPWM0_FAIL345
 #define MCPWM1_FAIL3_SEL_MASK                  ((uint32_t)0xC00)        //bit [11:10]
 #define MCPWM1_FAIL3_SEL_POS                   ((uint32_t)0xA)          //bit [10]
 #define MCPWM1_FAIL3_SEL_MASK_SET(x)           (((uint32_t)(((uint32_t)(x))<<MCPWM1_FAIL3_SEL_POS))&MCPWM1_FAIL3_SEL_MASK)
							
 #define MCPWM1_FAIL2_SEL_MASK                  ((uint32_t)0x300)        //bit [9:8]
 #define MCPWM1_FAIL2_SEL_POS                   ((uint32_t)0x8)          //bit [8]
 #define MCPWM1_FAIL2_SEL_MASK_SET(x)           (((uint32_t)(((uint32_t)(x))<<MCPWM1_FAIL2_SEL_POS))&MCPWM1_FAIL2_SEL_MASK)
							
 #define MCPWM1_HALT_PRT_MASK                  ((uint32_t)0x80)         //bit [7]
 #define MCPWM1_HALT_PRT_POS                   ((uint32_t)0x7)          //bit [7]
 #define MCPWM1_HALT_PRT_MASK_SET(x)           (((uint32_t)(((uint32_t)(x))<<MCPWM1_HALT_PRT_POS))&MCPWM1_HALT_PRT_MASK)
							
 #define MCPWM1_MOE_MASK                       ((uint32_t)0x40)         //bit [6]
 #define MCPWM1_MOE_POS                        ((uint32_t)0x6)          //bit [6]
 #define MCPWM1_MOE_MASK_SET(x)                (((uint32_t)(((uint32_t)(x))<<MCPWM1_MOE_POS))&MCPWM1_MOE_MASK)
							
 #define MCPWM1_FAIL3_EN_MASK                  ((uint32_t)0x20)          //bit [5]
 #define MCPWM1_FAIL3_EN_POS                   ((uint32_t)0x5)           //bit [5]
 #define MCPWM1_FAIL3_EN_MASK_SET(x)           (((uint32_t)(((uint32_t)(x))<<MCPWM1_FAIL3_EN_POS))&MCPWM1_FAIL3_EN_MASK)
							
 #define MCPWM1_FAIL2_EN_MASK                  ((uint32_t)0x10)          //bit [4]
 #define MCPWM1_FAIL2_EN_POS                   ((uint32_t)0x4)           //bit [4]
 #define MCPWM1_FAIL2_EN_MASK_SET(x)           (((uint32_t)(((uint32_t)(x))<<MCPWM1_FAIL2_EN_POS))&MCPWM1_FAIL2_EN_MASK)
							
 #define MCPWM1_FAIL3_POL_MASK                 ((uint32_t)0x8)          //bit [3]
 #define MCPWM1_FAIL3_POL_POS                  ((uint32_t)0x3)          //bit [3]
 #define MCPWM1_FAIL3_POL_MASK_SET(x)          (((uint32_t)(((uint32_t)(x))<<MCPWM1_FAIL3_POL_POS))&MCPWM1_FAIL3_POL_MASK)
							
 #define MCPWM1_FAIL2_POL_MASK                 ((uint32_t)0x4)          //bit [2]
 #define MCPWM1_FAIL2_POL_POS                  ((uint32_t)0x2)           //bit [2]
 #define MCPWM1_FAIL2_POL_MASK_SET(x)          (((uint32_t)(((uint32_t)(x))<<MCPWM1_FAIL2_POL_POS))&MCPWM1_FAIL2_POL_MASK)
 
 
 ///////////////////////////////////////////////////////////////////////////////////////MCPWM0_SDCFG
 #define MCPWM0_SDCFG_TR1_AEC_MASK             ((uint32_t)0x4000)       //bit [14]
 #define MCPWM0_SDCFG_TR1_AEC_POS              ((uint32_t)0xE)          //bit [14]
 #define MCPWM0_SDCFG_TR1_AEC_MASK_SET(x)      (((uint32_t)(((uint32_t)(x))<<MCPWM0_SDCFG_TR1_AEC_POS))&MCPWM0_SDCFG_TR1_AEC_MASK)

 #define MCPWM0_SDCFG_TR1_T1_UEN_MASK          ((uint32_t)0x2000)       //bit [13]
 #define MCPWM0_SDCFG_TR1_T1_UEN_POS           ((uint32_t)0xD)          //bit [13]
 #define MCPWM0_SDCFG_TR1_T1_UEN_MASK_SET(x)   (((uint32_t)(((uint32_t)(x))<<MCPWM0_SDCFG_TR1_T1_UEN_POS))&MCPWM0_SDCFG_TR1_T1_UEN_MASK)

 #define MCPWM0_SDCFG_TR1_T0_UEN_MASK          ((uint32_t)0x1000)       //bit [12]
 #define MCPWM0_SDCFG_TR1_T0_UEN_POS           ((uint32_t)0xC)          //bit [12]
 #define MCPWM0_SDCFG_TR1_T0_UEN_MASK_SET(x)   (((uint32_t)(((uint32_t)(x))<<MCPWM0_SDCFG_TR1_T0_UEN_POS))&MCPWM0_SDCFG_TR1_T0_UEN_MASK)

 #define MCPWM0_SDCFG_TR0_AEC_MASK             ((uint32_t)0x40)         //bit [6]
 #define MCPWM0_SDCFG_TR0_AEC_POS              ((uint32_t)0x6)          //bit [6]
 #define MCPWM0_SDCFG_TR0_AEC_MASK_SET(x)      (((uint32_t)(((uint32_t)(x))<<MCPWM0_SDCFG_TR0_AEC_POS))&MCPWM0_SDCFG_TR0_AEC_MASK)

 #define MCPWM0_SDCFG_TR0_T1_UEN_MASK          ((uint32_t)0x20)         //bit [5]
 #define MCPWM0_SDCFG_TR0_T1_UEN_POS           ((uint32_t)0x5)          //bit [5]
 #define MCPWM0_SDCFG_TR0_T1_UEN_MASK_SET(x)   (((uint32_t)(((uint32_t)(x))<<MCPWM0_SDCFG_TR0_T1_UEN_POS))&MCPWM0_SDCFG_TR0_T1_UEN_MASK)

 #define MCPWM0_SDCFG_TR0_T0_UEN_MASK          ((uint32_t)0x10)         //bit [4]
 #define MCPWM0_SDCFG_TR0_T0_UEN_POS           ((uint32_t)0x4)          //bit [4]
 #define MCPWM0_SDCFG_TR0_T0_UEN_MASK_SET(x)   (((uint32_t)(((uint32_t)(x))<<MCPWM0_SDCFG_TR0_T0_UEN_POS))&MCPWM0_SDCFG_TR0_T0_UEN_MASK)
  /////////////////////////////////////////////////////////////////////////////////////
#endif /* MCPWM_REG_H */


/************************ (C) COPYRIGHT OCS SEMICONDUCTOR *****END OF FILE****/

