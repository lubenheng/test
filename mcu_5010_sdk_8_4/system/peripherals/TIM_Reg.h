#ifndef TIMx_REG_H
#define TIMx_REG_H


typedef struct
{
    union
    {
        struct{
            __IO uint32_t CH0RCE    : 1;  // Bit0
            __IO uint32_t CH0FCE    : 1;  // Bit1
            __IO uint32_t CH0M      : 1;  // Bit2
            __IO uint32_t CH0POL    : 1;  // Bit3
            __IO uint32_t CH0CAPSRC : 4;  // Bit4-7
            __IO uint32_t CH1RCE    : 1;  // Bit8
            __IO uint32_t CH1FCE    : 1;  // Bit9
            __IO uint32_t CH1M      : 1;  // Bit10
            __IO uint32_t CH1POL    : 1;  // Bit11
            __IO uint32_t CH1CAPSRC : 4;  // Bit12-15
            __IO uint32_t CKS       : 1;  // Bit16
            __IO uint32_t RLD       : 1;  // Bit17
            __IO uint32_t GATE      : 1;  // Bit18
            __IO uint32_t ETON      : 1;  // Bit19
            __IO uint32_t CKDIV     : 3;  // Bit20-22
            __IO uint32_t DIR       : 1;  // Bit23
            __IO uint32_t MID       : 1;  // Bit24
            __IO uint32_t OTR       : 1;  // Bit25
            __IO uint32_t SHADOW    : 1;  // Bit26
            __IO uint32_t UPDATE    : 1;  // Bit27
            __IO uint32_t CCLR0E    : 1;  // Bit28
            __IO uint32_t CCLR1E    : 1;  // Bit29
            __IO uint32_t Res       : 1;  // Bit30
            __IO uint32_t EN        : 1;  // Bit31

        };
        __IO uint32_t word;
    } TIM_CFG;
		
    __IO uint32_t TH;	
    __IO uint32_t CNT;	
    __IO uint32_t CMP0;	
    __IO uint32_t CMP1;	
    __IO uint32_t EVTSRC;	
    __IO uint32_t FILT;	
		
		
    union
    {
        struct{
					__IO uint32_t CH0IE    	: 1;  // Bit0
					__IO uint32_t CH1IE    	: 1;  // Bit1
					__IO uint32_t ZCIE      : 1;  // Bit2
					__IO uint32_t FAIE    	: 1;  // Bit3
					__IO uint32_t RSV_4_7       : 4;  // Bit[7:4]: 未使用
					__IO uint32_t CH0_RE        : 1;  // Bit8: 通道0 DMA请求使能
					__IO uint32_t CH1_RE        : 1;  // Bit9: 通道1 DMA请求使能
					__IO uint32_t ZC_RE         : 1;  // Bit10: 过0 DMA请求使能
					__IO uint32_t FAIL_RE       : 1;  // Bit11: FAIL事件DMA请求使能
					__IO uint32_t RSV_12_31     : 20; // Bit[31:12]: 未使用

        };
        __IO uint32_t word;
    } IER;		
		
    union
    {
        struct{
            __IO uint32_t CH0IF    : 1;  // Bit0
            __IO uint32_t CH1IF    : 1;  // Bit1
            __IO uint32_t ZCIF      : 1;  // Bit2
            __IO uint32_t FAIF    : 1;  // Bit3
						__IO uint32_t RSV_4_31	: 28;

        };
        __IO uint32_t word;
    } IFR;			
	
    union
    {
        struct{
            __IO uint32_t CH0IF    		: 1;  // Bit0
            __IO uint32_t CH1IF    		: 1;  // Bit1
            __IO uint32_t ZCIF      	: 1;  // Bit2
						__IO uint32_t RSV_3_5			: 3;					
						__IO uint32_t MOE    			: 1;  
						__IO uint32_t HALTPRT    	: 1;  
						__IO uint32_t CH0DEF    	: 1;  
						__IO uint32_t CH1DEF    	: 1;  
						__IO uint32_t RSV_10_31		: 22;

        };
        __IO uint32_t word;
    } FAE;			
				
		
} TIMx_TypeDef;


#endif
