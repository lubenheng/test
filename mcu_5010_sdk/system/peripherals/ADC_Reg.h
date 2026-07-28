#ifndef ADC_REG_H
#define ADC_REG_H


typedef struct
{
    union 
    {
        struct{
            __IO uint32_t MINVN      : 8;  /* bit[7:0]   */
            __IO uint32_t MAJVN      : 8;  /* bit[15:8]  */
            __IO uint32_t FEATURE    : 16; /* bit[31:16] */
        };
        __IO uint32_t Word;
    }VERID_STR;

    __IO uint32_t DATA[16];
//    __IO uint32_t DATA1;   
//    __IO uint32_t DATA2;
//    __IO uint32_t DATA3;
//    __IO uint32_t DATA4;
//    __IO uint32_t DATA5;
//    __IO uint32_t DATA6;
//    __IO uint32_t DATA7;
//    __IO uint32_t DATA8;
//    __IO uint32_t DATA9;
//    __IO uint32_t DATA10;
//    __IO uint32_t DATA11;   
//    __IO uint32_t DATA12;
//    __IO uint32_t DATA13;
//    __IO uint32_t DATA14;
//    __IO uint32_t DATA15;
    __IO uint32_t RSV0;
    __IO uint32_t TGSR;
    __IO uint32_t TGNR;
	
		
		
		
    union 
    {
        struct{
            __IO uint32_t SEQ0        : 5;  /* bit[4:0]   */
            __IO uint32_t REV_5_7     : 3;  /* bit[7:5]   */
            __IO uint32_t SEQ1        : 5;  /* bit[12:8]   */
            __IO uint32_t REV_13_15   : 3;  /* bit[15:13]   */
            __IO uint32_t SEQ2        : 5;  /* bit[20:16]   */
            __IO uint32_t REV_21_23   : 3;  /* bit[23:21]   */
            __IO uint32_t SEQ3        : 5;  /* bit[28:24]   */
            __IO uint32_t REV_29_31   : 3;  /* bit[31:25]   */
        };
        __IO uint32_t Word;
    }SEQR0;
    union 
    {
        struct{
            __IO uint32_t SEQ4        : 5;  /* bit[4:0]   */
            __IO uint32_t REV_5_7     : 3;  /* bit[7:5]   */
            __IO uint32_t SEQ5        : 5;  /* bit[12:8]   */
            __IO uint32_t REV_13_15   : 3;  /* bit[15:13]   */
            __IO uint32_t SEQ6        : 5;  /* bit[20:16]   */
            __IO uint32_t REV_21_23   : 3;  /* bit[23:21]   */
            __IO uint32_t SEQ7        : 5;  /* bit[28:24]   */
            __IO uint32_t REV_29_31   : 3;  /* bit[31:25]   */
        };
        __IO uint32_t Word;
    }SEQR1;
    union 
    {
        struct{
            __IO uint32_t SEQ8        : 5;  /* bit[4:0]   */
            __IO uint32_t REV_5_7     : 3;  /* bit[7:5]   */
            __IO uint32_t SEQ9        : 5;  /* bit[12:8]   */
            __IO uint32_t REV_13_15   : 3;  /* bit[15:13]   */
            __IO uint32_t SEQ10        : 5;  /* bit[20:16]   */
            __IO uint32_t REV_21_23   : 3;  /* bit[23:21]   */
            __IO uint32_t SEQ11        : 5;  /* bit[28:24]   */
            __IO uint32_t REV_29_31   : 3;  /* bit[31:25]   */
        };
        __IO uint32_t Word;
    }SEQR2;
    union 
    {
        struct{
            __IO uint32_t SEQ12        : 5;  /* bit[4:0]   */
            __IO uint32_t REV_5_7     : 3;  /* bit[7:5]   */
            __IO uint32_t SEQ13        : 5;  /* bit[12:8]   */
            __IO uint32_t REV_13_15   : 3;  /* bit[15:13]   */
            __IO uint32_t SEQ14        : 5;  /* bit[20:16]   */
            __IO uint32_t REV_21_23   : 3;  /* bit[23:21]   */
            __IO uint32_t SEQ15        : 5;  /* bit[28:24]   */
            __IO uint32_t REV_29_31   : 3;  /* bit[31:25]   */
        };
        __IO uint32_t Word;
    }SEQR3;

    __IO uint32_t CR;   
    __IO uint32_t OTR;  
    union 
    {
        struct{
            __IO uint32_t STPT        : 4; 
 
            __IO uint32_t COVT        : 4;  
 
            __IO uint32_t SAPT        : 8;  
            __IO uint32_t INIT   : 8;  
            __IO uint32_t TDELY        : 8;  
        };
        __IO uint32_t Word;
    }TCFG;		
    __IO uint32_t AWTH; 
    __IO uint32_t AWE; 
    union 
    {
        struct{
            __IO uint32_t S0CDIF        : 1; 
						__IO uint32_t S1CDIF        : 1; 
						__IO uint32_t S2CDIF        : 1; 
						__IO uint32_t S3CDIF        : 1; 
            __IO uint32_t REV_4_15			: 12;  
            __IO uint32_t TGNERRIF			: 1; 
            __IO uint32_t TGTERRIF			: 1; 
            __IO uint32_t REV_18_19     : 2;  					
            __IO uint32_t AWDIF					: 1;  
            __IO uint32_t REV_21_30     : 10;  
						__IO uint32_t RDY						: 1;
        };
        __IO uint32_t Word;
    }IR;		
    union 
    {
        struct{
            __IO uint32_t S0CDIE        : 1; 
						__IO uint32_t S1CDIE        : 1; 
						__IO uint32_t S2CDIE        : 1; 
						__IO uint32_t S3CDIE        : 1; 
            __IO uint32_t REV_4_15			: 12;  
            __IO uint32_t TGNERRIE			: 1; 
            __IO uint32_t TGTERRIE			: 1; 
            __IO uint32_t REV_18_19     : 2;  					
            __IO uint32_t AWDIE					: 1;  
            __IO uint32_t REV_21_31     : 11;  
        };
        __IO uint32_t Word;
    }IE;				
    __IO uint32_t RSV1;
    __IO uint32_t CALIB;
} ADC_TypeDef;





#endif
