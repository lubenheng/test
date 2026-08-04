#ifndef CSA_REG_H
#define CSA_REG_H


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

	

    union
    {
        struct{
            __IO uint32_t CSA_EN      : 1;  /* bit[0]   */
            __IO uint32_t CSA_AMP_EN  : 1;  /* bit[1]   */
            __IO uint32_t CSA_MODE    : 2;  /* bit[3:2] */
            __IO uint32_t CSA_CSP     : 2;  /* bit[5:4] */
            __IO uint32_t CSA_CSN     : 2;  /* bit[7:6] */
            __IO uint32_t CSA_GAIN    : 3;  /* bit[10:8] */   
            __IO uint32_t CSA_DAC_EN  : 1;  /* bit[11]   */   
            __IO uint32_t CSA_REF     : 5;  /* bit[16:12]*/   
            __IO uint32_t             : 3;  /* bit[19:17]*/   
            __IO uint32_t CSA_DAC_REF : 3;  /* bit[22:20]*/   
            __IO uint32_t             : 1;  /* bit[23]   */   
            __IO uint32_t CSA_DAC_OUT : 8;  /* bit[31:24]*/   
        };
        __IO uint32_t Word;
    } CR0;  

} CSA_TypeDef;





#endif
