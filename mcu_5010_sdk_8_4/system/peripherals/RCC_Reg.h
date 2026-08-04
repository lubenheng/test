#ifndef RCC_REG_H
#define RCC_REG_H


typedef struct
{
    union 
    {
        struct{
            __IO uint32_t MINVN             : 8;  /* bit[7:0]   */
            __IO uint32_t MAJVN             : 8;  /* bit[15:8]  */
            __IO uint32_t FEATURE           : 16; /* bit[31:16] */
        };
        __IO uint32_t Word;
    }VERID_STR; 

    union 
    {
        struct{
            __IO uint32_t PORBF             : 1;  
            __IO uint32_t BORBF             : 1;  
            __IO uint32_t LVDRSTF           : 1;  
            __IO uint32_t RSV_3_7           : 5; 
            __IO uint32_t PINRSTF           : 1; 
            __IO uint32_t SYSRSTREQF        : 1; 
            __IO uint32_t LOCKUPF           : 1; 
            __IO uint32_t RSV_11_15         : 5; 
            __IO uint32_t IWDTF             : 1; 
            __IO uint32_t WWDTF             : 1; 
            __IO uint32_t RSV_18_20         : 3; 
            __IO uint32_t HSEFAILRSTF       : 1; 
            __IO uint32_t PLLULKRSTF        : 1; 
            __IO uint32_t RSV_23            : 1; 
            __IO uint32_t RETRIMF           : 1; 
            __IO uint32_t RSV_25_31         : 1; 
        };
        __IO uint32_t Word;
    }SYSRSTSR;

   union 
    {
        struct{
            __IO uint32_t RSV_0_1           : 2; 
            __IO uint32_t LVDRSTE             : 1;  
            __IO uint32_t RSV_3_9             : 7;  
            __IO uint32_t LOCKUPE           : 1;  
            __IO uint32_t RSV_11_15           : 5; 
            __IO uint32_t IWDTE           : 1; 
            __IO uint32_t RSV_17_20           : 4; 
            __IO uint32_t HSEFAILRSTE           : 1; 
            __IO uint32_t PLLULKRSTE        : 1; 
            __IO uint32_t RSV_23            : 1; 
            __IO uint32_t KEYCODE            : 8; 
        };
        __IO uint32_t Word;
    }SYSRSTE;

    __IO uint32_t RSV_0;

   union 
    {
        struct{
            __IO uint32_t FLC           : 1; 
            __IO uint32_t RSV_1_15             : 15;
            __IO uint32_t GPIOARST           : 1;             
            __IO uint32_t GPIOBRST           : 1;             
            __IO uint32_t GPIOCRST           : 1; 
            __IO uint32_t RSV_19_31             : 13;  

        };
        __IO uint32_t Word;
    }AHBRST;

   union 
    {
        struct{
            __IO uint32_t ADCRST           : 1; 
            __IO uint32_t RSV_1_31             : 31;
        };
        __IO uint32_t Word;
    }ANARST;
   union 
    {
        struct{
            __IO uint32_t HWMATHRST           : 1; 
            __IO uint32_t RSV_1_15             : 15;
            __IO uint32_t IWDTRST           : 1; 
            __IO uint32_t WWDTRST           : 1; 
            __IO uint32_t RSV_16_31           : 14; 
        };
        __IO uint32_t Word;
    }DIGRST;
   union 
    {
        struct{
            __IO uint32_t RSV_0_7           : 8; 
            __IO uint32_t UART0RST             : 1;
            __IO uint32_t UART1RST             : 1;
            __IO uint32_t RSV_10_15           : 6; 
            __IO uint32_t SPI0RST           : 1; 
            __IO uint32_t SPI1RST           : 1; 
            __IO uint32_t RSV_18_23           : 6; 
            __IO uint32_t IIC0RST           : 1; 
            __IO uint32_t IIC1RST           : 1; 
            __IO uint32_t RSV_26_31           : 6; 
        };
        __IO uint32_t Word;
    }COMRST;
   union 
    {
        struct{
            __IO uint32_t RSV_0           : 1; 
            __IO uint32_t TIM1RST             : 1;
            __IO uint32_t TIM2RST             : 1;
            __IO uint32_t TIM3RST             : 1;
            __IO uint32_t TIM4RST             : 1;
            __IO uint32_t TIM5RST             : 1;
            __IO uint32_t TIM6RST             : 1;
            __IO uint32_t TIM7RST             : 1;
            __IO uint32_t TIM8RST             : 1;
            __IO uint32_t TIM9RST             : 1;
            __IO uint32_t TIM10RST             : 1;
            __IO uint32_t TIM11RST             : 1;
            __IO uint32_t TIM12RST             : 1;
            __IO uint32_t TIM13RST             : 1;
            __IO uint32_t TIM14RST             : 1;
            __IO uint32_t TIM15RST             : 1;
            __IO uint32_t RSV_16_31           : 15; 
        };
        __IO uint32_t Word;
    }TIMRST;
    __IO uint32_t RSV_1;
    __IO uint32_t RSV_2;
    __IO uint32_t RSV_3;
   union 
    {
        struct{
            __IO uint32_t HSIEN           : 1; 
            __IO uint32_t RSV_1_30             : 30;
            __IO uint32_t HSIRDY           : 1; 
        };
        __IO uint32_t Word;
    }HSICR;
   union 
    {
        struct{
            __IO uint32_t LSIEN           : 1; 
            __IO uint32_t RSV_1_30             : 30;
            __IO uint32_t LSIRDY           : 1; 
        };
        __IO uint32_t Word;
    }LSICR;

   union 
    {
        struct{
            __IO uint32_t HSEEN           : 1; 
            __IO uint32_t HSEXSE           : 1; 
            __IO uint32_t RSV_2_3           : 2; 
            __IO uint32_t HSEDETE           : 1; 
            __IO uint32_t RSV_5_30             : 26;
            __IO uint32_t HSIRDY           : 1; 
        };
        __IO uint32_t Word;
    }HSECR;
   union 
    {
        struct{
            __IO uint32_t LSEEN           : 1; 
            __IO uint32_t LSEXSE           : 1; 
            __IO uint32_t RSV_2_30             : 29;
            __IO uint32_t LSIRDY           : 1; 
        };
        __IO uint32_t Word;
    }LSECR;

    union 
    {
        struct{
            __IO uint32_t PLLEN           : 1; 
            __IO uint32_t RSV_1_30             : 30;
            __IO uint32_t PLLRDY           : 1; 
        };
        __IO uint32_t Word;
    }PLLCR0;

    union 
    {
        struct{
            __IO uint32_t PLLDIV           : 3; 
            __IO uint32_t RSV_3_7             : 5;
            __IO uint32_t PLLRDY           : 1; 
        };
        __IO uint32_t Word;
    }PLLCR1;
    __IO uint32_t RSV_4;
    __IO uint32_t RSV_5;

    union 
    {
        struct{
            __IO uint32_t SYSCKSW           : 2; 
            __IO uint32_t RSV_2_15             : 14;
            __IO uint32_t HPRE           : 4; 
            __IO uint32_t RSV_20_23             : 4;
            __IO uint32_t PPRE1           : 4; 
            __IO uint32_t RSV_28_31             : 4;


        };
        __IO uint32_t Word;
    }SYSCKCR;
    union 
    {
        struct{
            __IO uint32_t MCOSRC           : 4; 
            __IO uint32_t RSV_4_7             : 4;
            __IO uint32_t MCOPRE           : 4; 
            __IO uint32_t RSV_12_31             : 20;

        };
        __IO uint32_t Word;
    }MCOCR;
    union 
    {
        struct{
            __IO uint32_t FLCCKE           : 1; 
            __IO uint32_t RSV_1_15             : 15;
            __IO uint32_t GPIOACKE           : 1; 
            __IO uint32_t GPIOBCKE           : 1; 
            __IO uint32_t GPIOCCKE           : 1; 
            __IO uint32_t RSV_19_31             : 13;

        };
        __IO uint32_t Word;
    }AHBCKE;
    union 
    {
        struct{
            __IO uint32_t ADCCKE           : 1; 
            __IO uint32_t RSV_1_31             : 31;
        };
        __IO uint32_t Word;
    }ANACKE;
    union 
    {
        struct{
            __IO uint32_t HWMATHCKE           : 1; 
            __IO uint32_t RSV_1_15             : 15;
            __IO uint32_t IWDTCKE           : 1; 
            __IO uint32_t RSV_17_31             : 15;
        };
        __IO uint32_t Word;
    }DIGCKE;
    union 
    {
        struct{
            __IO uint32_t RSV_0_7           : 8; 
            __IO uint32_t UART0CKE             : 1;
            __IO uint32_t UART1CKE             : 1;    
            __IO uint32_t RSV_10_15           : 6; 

            __IO uint32_t SPI0CKE             : 1;
            __IO uint32_t SPI1CKE             : 1;    
            __IO uint32_t RSV_18_23           : 6; 

            __IO uint32_t IIC0CKE             : 1;
            __IO uint32_t IIC1CKE             : 1;    
            __IO uint32_t RSV_26_31           : 6;             
        };
        __IO uint32_t Word;
    }COMCKE;
    union 
    {
        struct{
            __IO uint32_t RSV_0           : 1; 
            __IO uint32_t TIM1CKE             : 1;

            __IO uint32_t RSV_2_12           : 11; 

            __IO uint32_t TIM13CKE             : 1;  
            __IO uint32_t TIM14CKE             : 1;              
            __IO uint32_t TIM15CKE             : 1;              
            __IO uint32_t TIM16CKE             : 1;  
            __IO uint32_t RSV_17_23           : 7;      
            __IO uint32_t MCPWMCKE             : 1;              
            __IO uint32_t RSV_25_31           : 7;                

        };
        __IO uint32_t Word;
    }TIMCKE;
    __IO uint32_t RSV_6;
    union 
    {
        struct{
            __IO uint32_t RSV_0           : 1; 
            __IO uint32_t TIM1CKE             : 1;

            __IO uint32_t RSV_2_12           : 11; 

            __IO uint32_t TIM13CKE             : 1;  
            __IO uint32_t TIM14CKE             : 1;              
            __IO uint32_t TIM15CKE             : 1;              
            __IO uint32_t TIM16CKE             : 1;  
            __IO uint32_t RSV_17_23           : 7;      
            __IO uint32_t MCPWMCKE             : 1;              
            __IO uint32_t RSV_25_31           : 7;               
        };
        __IO uint32_t Word;
    }PCKS0;



} RCC_TypeDef;


#endif
