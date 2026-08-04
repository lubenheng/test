#ifndef UART_REG_H
#define UART_REG_H

typedef volatile union
{
    volatile unsigned int Word;
    struct
    {
        unsigned MINVN : 8;
        unsigned MAJVN : 8;
        unsigned FEATURE : 16;
    } Bits;
} LPUART0_VERID_STR;

#define _LPUART0_VERID (*(LPUART0_VERID_STR *)(CMSDK_LPUART0_BASE + 0x00))
#define LPUART0_VERID _LPUART0_VERID.Word
#define LPUART0_VERID_MINVN (_LPUART0_VERID).Bits.MINVN
#define LPUART0_VERID_MAJVN (_LPUART0_VERID).Bits.MAJVN
#define LPUART0_VERID_FEATURE (_LPUART0_VERID).Bits.FEATURE

typedef volatile union
{
    volatile unsigned int Word;
    struct
    {
        unsigned TXFIFOSIZE : 4;
        unsigned RXFIFOSIZE : 4;
        unsigned TXEMPT : 1;
        unsigned TXFULL : 1;
        unsigned RXEMPT : 1;
        unsigned RXFULL : 1;
        unsigned : 20;
    } Bits;
} LPUART0_SR_STR;

#define _LPUART0_SR (*(LPUART0_SR_STR *)(CMSDK_LPUART0_BASE + 0x04))
#define LPUART0_SR _LPUART0_SR.Word
#define LPUART0_SR_TXFIFOSIZE (_LPUART0_SR).Bits.TXFIFOSIZE
#define LPUART0_SR_RXFIFOSIZE (_LPUART0_SR).Bits.RXFIFOSIZE
#define LPUART0_SR_TXEMPT (_LPUART0_SR).Bits.TXEMPT
#define LPUART0_SR_TXFULL (_LPUART0_SR).Bits.TXFULL
#define LPUART0_SR_RXEMPT (_LPUART0_SR).Bits.RXEMPT
#define LPUART0_SR_RXFULL (_LPUART0_SR).Bits.RXFULL

typedef volatile union
{
    volatile unsigned int Word;
    struct
    {
        unsigned TXOVF : 1;
        unsigned TXUDF : 1;
        unsigned : 6;
        unsigned RXOVF : 1;
        unsigned RXUDF : 1;
        unsigned : 6;
        unsigned RFEF : 1;
        unsigned RPEF : 1;
        unsigned RNSF : 1;
        unsigned RDLF : 1;
        unsigned : 8;
        unsigned MA2F : 1;
        unsigned MA1F : 1;
        unsigned RXEDGIF : 1;
        unsigned LBKDIF : 1;
    } Bits;
} LPUART0_IR_STR;

#define _LPUART0_IR (*(LPUART0_IR_STR *)(CMSDK_LPUART0_BASE + 0x08))
#define LPUART0_IR _LPUART0_IR.Word
#define LPUART0_IR_TXOVF (_LPUART0_IR).Bits.TXOVF
#define LPUART0_IR_TXUDF (_LPUART0_IR).Bits.TXUDF
#define LPUART0_IR_RXOVF (_LPUART0_IR).Bits.RXOVF
#define LPUART0_IR_RXUDF (_LPUART0_IR).Bits.RXUDF
#define LPUART0_IR_RFEF (_LPUART0_IR).Bits.RFEF
#define LPUART0_IR_RPEF (_LPUART0_IR).Bits.RPEF
#define LPUART0_IR_RNSF (_LPUART0_IR).Bits.RNSF
#define LPUART0_IR_RDLF (_LPUART0_IR).Bits.RDLF
#define LPUART0_IR_MA2F (_LPUART0_IR).Bits.MA2F
#define LPUART0_IR_MA1F (_LPUART0_IR).Bits.MA1F
#define LPUART0_IR_RXEDGIF (_LPUART0_IR).Bits.RXEDGIF
#define LPUART0_IR_LBKDIF (_LPUART0_IR).Bits.LBKDIF

typedef volatile union
{
    volatile unsigned int Word;
    struct
    {
        unsigned TXOVIE : 1;
        unsigned TXUDIE : 1;
        unsigned TXFEIE : 1;
        unsigned TXFFIE : 1;
        unsigned : 4;
        unsigned RXOVIE : 1;
        unsigned RXUDIE : 1;
        unsigned RXFEIE : 1;
        unsigned RXFFIE : 1;
        unsigned RFEIE : 1;
        unsigned RPEIE : 1;
        unsigned RNSIE : 1;
        unsigned RDLIE : 1;
        unsigned : 4;
        unsigned RXEDGIE : 1;
        unsigned LBKDIE : 1;
    } Bits;
} LPUART0_IER_STR;

#define _LPUART0_IER (*(LPUART0_IER_STR *)(CMSDK_LPUART0_BASE + 0x0C))
#define LPUART0_IER _LPUART0_IER.Word
#define LPUART0_IER_TXOVIE (_LPUART0_IER).Bits.TXOVIE
#define LPUART0_IER_TXUDIE (_LPUART0_IER).Bits.TXUDIE
#define LPUART0_IER_TXFEIE (_LPUART0_IER).Bits.TXFEIE
#define LPUART0_IER_TXFFIE (_LPUART0_IER).Bits.TXFFIE
#define LPUART0_IER_RXOVIE (_LPUART0_IER).Bits.RXOVIE
#define LPUART0_IER_RXUDIE (_LPUART0_IER).Bits.RXUDIE
#define LPUART0_IER_RXFEIE (_LPUART0_IER).Bits.RXFEIE
#define LPUART0_IER_RXFFIE (_LPUART0_IER).Bits.RXFFIE
#define LPUART0_IER_RFEIE (_LPUART0_IER).Bits.RFEIE
#define LPUART0_IER_RPEIE (_LPUART0_IER).Bits.RPEIE
#define LPUART0_IER_RNSIE (_LPUART0_IER).Bits.RNSIE
#define LPUART0_IER_RDLIE (_LPUART0_IER).Bits.RDLIE
#define LPUART0_IER_RXEDGIE (_LPUART0_IER).Bits.RXEDGIE
#define LPUART0_IER_LBKDIE (_LPUART0_IER).Bits.LBKDIE

typedef volatile union
{
    volatile unsigned int Word;
    struct
    {
        unsigned BR : 13;
        unsigned : 3;
        unsigned OSR : 5;
        unsigned : 11;
    } Bits;
} LPUART0_BRR_STR;

#define _LPUART0_BRR (*(LPUART0_BRR_STR *)(CMSDK_LPUART0_BASE + 0x10))
#define LPUART0_BRR _LPUART0_BRR.Word
#define LPUART0_BRR_BR (_LPUART0_BRR).Bits.BR
#define LPUART0_BRR_OSR (_LPUART0_BRR).Bits.OSR

typedef volatile union
{
    volatile unsigned int Word;
    struct
    {
        unsigned DATA : 10;
        unsigned : 6;
        unsigned RFE : 1;
        unsigned RPE : 1;
        unsigned RNS : 1;
        unsigned IDL : 1;
        unsigned : 12;
    } Bits;
} LPUART0_DR_STR;

#define _LPUART0_DR (*(LPUART0_DR_STR *)(CMSDK_LPUART0_BASE + 0x14))
#define LPUART0_DR _LPUART0_DR.Word
#define LPUART0_DR_DATA (_LPUART0_DR).Bits.DATA
#define LPUART0_DR_RFE (_LPUART0_DR).Bits.RFE
#define LPUART0_DR_RPE (_LPUART0_DR).Bits.RPE
#define LPUART0_DR_RNS (_LPUART0_DR).Bits.RNS
#define LPUART0_DR_IDL (_LPUART0_DR).Bits.IDL

typedef volatile union
{
    volatile unsigned int Word;
    struct
    {
        unsigned TXE : 1;
        unsigned TXPOL : 1;
        unsigned TXDIR : 1;
        unsigned TXFRAM : 1;
        unsigned TDMAE : 1;
        unsigned TXFE : 1;
        unsigned TXFLUSH : 1;
        unsigned : 1;
        unsigned RXE : 1;
        unsigned RXPOL : 1;
        unsigned : 2;
        unsigned RDMAE : 1;
        unsigned RXFE : 1;
        unsigned RXFLUSH : 1;
        unsigned : 1;
        unsigned PE : 1;
        unsigned PT : 1;
        unsigned M9 : 1;
        unsigned M10 : 1;
        unsigned SBNS : 1;
        unsigned MSB : 1;
        unsigned SWE : 1;
        unsigned : 1;
        unsigned SENBRK : 1;
        unsigned LENBRK : 1;
        unsigned DETBRK : 1;
        unsigned LPM : 1;
        unsigned LPT : 1;
    } Bits;
} LPUART0_CR_STR;

#define _LPUART0_CR (*(LPUART0_CR_STR *)(CMSDK_LPUART0_BASE + 0x18))
#define LPUART0_CR _LPUART0_CR.Word
#define LPUART0_CR_TXE (_LPUART0_CR).Bits.TXE
#define LPUART0_CR_TXPOL (_LPUART0_CR).Bits.TXPOL
#define LPUART0_CR_TXDIR (_LPUART0_CR).Bits.TXDIR
#define LPUART0_CR_TXFRAM (_LPUART0_CR).Bits.TXFRAM
#define LPUART0_CR_TDMAE (_LPUART0_CR).Bits.TDMAE
#define LPUART0_CR_TXFE (_LPUART0_CR).Bits.TXFE
#define LPUART0_CR_TXFLUSH (_LPUART0_CR).Bits.TXFLUSH
#define LPUART0_CR_RXE (_LPUART0_CR).Bits.RXE
#define LPUART0_CR_RXPOL (_LPUART0_CR).Bits.RXPOL
#define LPUART0_CR_RDMAE (_LPUART0_CR).Bits.RDMAE
#define LPUART0_CR_RXFE (_LPUART0_CR).Bits.RXFE
#define LPUART0_CR_RXFLUSH (_LPUART0_CR).Bits.RXFLUSH
#define LPUART0_CR_PE (_LPUART0_CR).Bits.PE
#define LPUART0_CR_PT (_LPUART0_CR).Bits.PT
#define LPUART0_CR_M9 (_LPUART0_CR).Bits.M9
#define LPUART0_CR_M10 (_LPUART0_CR).Bits.M10
#define LPUART0_CR_SBNS (_LPUART0_CR).Bits.SBNS
#define LPUART0_CR_MSB (_LPUART0_CR).Bits.MSB
#define LPUART0_CR_SWE (_LPUART0_CR).Bits.SWE
#define LPUART0_CR_SENBRK (_LPUART0_CR).Bits.SENBRK
#define LPUART0_CR_LENBRK (_LPUART0_CR).Bits.LENBRK
#define LPUART0_CR_DETBRK (_LPUART0_CR).Bits.DETBRK
#define LPUART0_CR_LPM (_LPUART0_CR).Bits.LPM
#define LPUART0_CR_LPT (_LPUART0_CR).Bits.LPT

typedef volatile union
{
    volatile unsigned int Word;
    struct
    {
        unsigned MATCH1 : 10;
        unsigned : 2;
        unsigned MAEN1 : 1;
        unsigned MAEN2 : 1;
        unsigned : 2;
        unsigned MATCH2 : 10;
        unsigned : 2;
        unsigned MATCFG : 2;
        unsigned : 2;
    } Bits;
} LPUART0_MATCH_STR;

#define _LPUART0_MATCH (*(LPUART0_MATCH_STR *)(CMSDK_LPUART0_BASE + 0x1C))
#define LPUART0_MATCH _LPUART0_MATCH.Word
#define LPUART0_MATCH_MATCH1 (_LPUART0_MATCH).Bits.MATCH1
#define LPUART0_MATCH_MAEN1 (_LPUART0_MATCH).Bits.MAEN1
#define LPUART0_MATCH_MAEN2 (_LPUART0_MATCH).Bits.MAEN2
#define LPUART0_MATCH_MATCH2 (_LPUART0_MATCH).Bits.MATCH2
#define LPUART0_MATCH_MATCFG (_LPUART0_MATCH).Bits.MATCFG

typedef volatile union
{
    volatile unsigned int Word;
    struct
    {
        unsigned RXWATER : 2;
        unsigned : 2;
        unsigned TXWATER : 2;
        unsigned : 2;
        unsigned RTSWATER : 2;
        unsigned : 6;
        unsigned RXCOUNT : 3;
        unsigned : 1;
        unsigned TXCOUNT : 3;
        unsigned : 9;
    } Bits;
} LPUART0_WATER_STR;

#define _LPUART0_WATER (*(LPUART0_WATER_STR *)(CMSDK_LPUART0_BASE + 0x2C))
#define LPUART0_WATER _LPUART0_WATER.Word
#define LPUART0_WATER_RXWATER (_LPUART0_WATER).Bits.RXWATER
#define LPUART0_WATER_TXWATER (_LPUART0_WATER).Bits.TXWATER
#define LPUART0_WATER_RTSWATER (_LPUART0_WATER).Bits.RTSWATER
#define LPUART0_WATER_RXCOUNT (_LPUART0_WATER).Bits.RXCOUNT
#define LPUART0_WATER_TXCOUNT (_LPUART0_WATER).Bits.TXCOUNT





// __IO uint32_t TXOVF              : 1;  /* bit[0]     */
#define UART_TXOVF_INTP_FLAG_MASK                ((uint32_t)0x0)     //bit [0]
#define UART_TXOVF_INTP_FLAG_POS                 ((uint32_t)0x0)     //bit [0]
#define UART_TXOVF_INTP_FLAG_SET(x)              (((uint32_t)(((uint32_t)(x))<<UART_TXOVF_INTP_FLAG_POS))&UART_TXOVF_INTP_FLAG_MASK)

//__IO uint32_t TXUDF              : 1;  /* bit[1]     */
#define UART_TXUDF_INTP_FLAG_MASK                ((uint32_t)0x1)     //bit [1]
#define UART_TXUDF_INTP_FLAG_POS                 ((uint32_t)0x1)     //bit [1]
#define UART_TXUDF_INTP_FLAG_SET(x)              (((uint32_t)(((uint32_t)(x))<<UART_TXUDF_INTP_FLAG_POS))&UART_TXUDF_INTP_FLAG_MASK)

//__IO uint32_t RXOVF              : 1;  /* bit[8]     */
#define UART_RXOVF_INTP_FLAG_MASK                ((uint32_t)0x100)      //bit [8]
#define UART_RXOVF_INTP_FLAG_POS                 ((uint32_t)0x8)        //bit [8]
#define UART_RXOVF_INTP_FLAG_SET(x)              (((uint32_t)(((uint32_t)(x))<<UART_RXOVF_INTP_FLAG_POS))&UART_RXOVF_INTP_FLAG_MASK)

//__IO uint32_t RXUDF              : 1;  /* bit[9]     */
#define UART_RXUDF_INTP_FLAG_MASK                ((uint32_t)0x200)      //bit [9]
#define UART_RXUDF_INTP_FLAG_POS                 ((uint32_t)0x9)        //bit [9]
#define UART_RXUDF_INTP_FLAG_SET(x)              (((uint32_t)(((uint32_t)(x))<<UART_RXUDF_INTP_FLAG_POS))&UART_RXUDF_INTP_FLAG_MASK)



//__IO uint32_t RFEF               : 1;  /* bit[16]    */
#define UART_RFEF_INTP_FLAG_MASK                ((uint32_t)0x10000)      //bit [16]
#define UART_RFEF_INTP_FLAG_POS                 ((uint32_t)0x10)        //bit [16]
#define UART_RFEF_INTP_FLAG_SET(x)              (((uint32_t)(((uint32_t)(x))<<UART_RFEF_INTP_FLAG_POS))&UART_RFEF_INTP_FLAG_MASK)

//__IO uint32_t RPEF               : 1;  /* bit[17]    */
#define UART_RPEF_INTP_FLAG_MASK                ((uint32_t)0x20000)      //bit [17]
#define UART_RPEF_INTP_FLAG_POS                 ((uint32_t)0x11)        //bit [17]
#define UART_RPEF_INTP_FLAG_SET(x)              (((uint32_t)(((uint32_t)(x))<<UART_RPEF_INTP_FLAG_POS))&UART_RPEF_INTP_FLAG_MASK)

//__IO uint32_t RNSF               : 1;  /* bit[18]    */
#define UART_RNSF_INTP_FLAG_MASK                ((uint32_t)0x40000)      //bit [18]
#define UART_RNSF_INTP_FLAG_POS                 ((uint32_t)0x12)         //bit [18]
#define UART_RNSF_INTP_FLAG_SET(x)              (((uint32_t)(((uint32_t)(x))<<UART_RNSF_INTP_FLAG_POS))&UART_RNSF_INTP_FLAG_MASK)

//__IO uint32_t RDLF               : 1;  /* bit[19]    */
#define UART_RDLF_INTP_FLAG_MASK                ((uint32_t)0x80000)      //bit [19]
#define UART_RDLF_INTP_FLAG_POS                 ((uint32_t)0x13)         //bit [19]
#define UART_RDLF_INTP_FLAG_SET(x)              (((uint32_t)(((uint32_t)(x))<<UART_RDLF_INTP_FLAG_POS))&UART_RDLF_INTP_FLAG_MASK)

//__IO uint32_t MA2F               : 1;  /* bit[28]    */
#define UART_MA2F_INTP_FLAG_MASK                ((uint32_t)0x10000000)      //bit [28]
#define UART_MA2F_INTP_FLAG_POS                 ((uint32_t)0x1c)            //bit [28]
#define UART_MA2F_INTP_FLAG_SET(x)              (((uint32_t)(((uint32_t)(x))<<UART_MA2F_INTP_FLAG_POS))&UART_MA2F_INTP_FLAG_MASK)

//__IO uint32_t MA1F               : 1;  /* bit[29]    */
#define UART_MA1F_INTP_FLAG_MASK                ((uint32_t)0x20000000)      //bit [29]
#define UART_MA1F_INTP_FLAG_POS                 ((uint32_t)0x1d)            //bit [29]
#define UART_MA1F_INTP_FLAG_SET(x)              (((uint32_t)(((uint32_t)(x))<<UART_MA1F_INTP_FLAG_POS))&UART_MA1F_INTP_FLAG_MASK)

//__IO uint32_t RXEDGIF            : 1;  /* bit[30]    */
#define UART_RXEDGIF_INTP_FLAG_MASK                ((uint32_t)0x40000000)      //bit [30]
#define UART_RXEDGIF_INTP_FLAG_POS                 ((uint32_t)0x1e)            //bit [30]
#define UART_RXEDGIF_INTP_FLAG_SET(x)              (((uint32_t)(((uint32_t)(x))<<UART_RXEDGIF_INTP_FLAG_POS))&UART_RXEDGIF_INTP_FLAG_MASK)

//__IO uint32_t LBKDIF             : 1;  /* bit[31]    */
#define UART_LBKDIF_INTP_FLAG_MASK                ((uint32_t)0x80000000)      //bit [31]
#define UART_LBKDIF_INTP_FLAG_POS                 ((uint32_t)0x1f)            //bit [31]
#define UART_LBKDIF_INTP_FLAG_SET(x)              (((uint32_t)(((uint32_t)(x))<<UART_LBKDIF_INTP_FLAG_POS))&UART_LBKDIF_INTP_FLAG_MASK)



#define UART_BAUD_OSR_MASK                       ((uint32_t)0x1f0000)//bit [20:16]
#define UART_BAUD_OSR_POS                        ((uint32_t)0x10)//bit [20:16]
#define UART_BAUD_OSR_SET(x)                     (((uint32_t)(((uint32_t)(x))<<UART_BAUD_OSR_POS))&UART_BAUD_OSR_MASK)

//__IO uint32_t BR                  : 13; /* bit[12:0]  */
#define UART_BAUD_BAUD_DIV_INT_MASK              ((uint32_t)0x00001fff)//bit [12:0]
#define UART_BAUD_BAUD_DIV_INT_POS               ((uint32_t)0x00)//bit [12:0]
#define UART_BAUD_BAUD_DIV_INT_SET(x)            (((uint32_t)(((uint32_t)(x))<<UART_BAUD_BAUD_DIV_INT_POS))&UART_BAUD_BAUD_DIV_INT_MASK)


//__IO uint32_t M9                  : 1;  /* bit[18]    */
#define UART_CTRL_M9_MASK                    ((uint32_t)0x40000)//bit 18
#define UART_CTRL_M9_POS                     ((uint32_t)0x12)//bit 18
#define UART_CTRL_M9_SET(x)                  (((uint32_t)(((uint32_t)(x))<<UART_CTRL_M9_POS))&UART_CTRL_M9_MASK)

//__IO uint32_t M10                 : 1;  /* bit[19]    */
#define UART_CTRL_M10_MASK                    ((uint32_t)0x80000)//bit 19
#define UART_CTRL_M10_POS                     ((uint32_t)0x13)   //bit 19
#define UART_CTRL_M10_SET(x)                  (((uint32_t)(((uint32_t)(x))<<UART_CTRL_M10_POS))&UART_CTRL_M10_MASK)

//__IO uint32_t PE                  : 1;  /* bit[16]    */
#define UART_CTRL_PAR_EN_MASK                    ((uint32_t)0x10000)//bit 16
#define UART_CTRL_PAR_EN_POS                     ((uint32_t)0x10)//bit 16
#define UART_CTRL_PAR_EN_SET(x)                  (((uint32_t)(((uint32_t)(x))<<UART_CTRL_PAR_EN_POS))&UART_CTRL_PAR_EN_MASK)

//__IO uint32_t PT                  : 1;  /* bit[17]    */
#define UART_CTRL_PAR_ODD_MASK                   ((uint32_t)0x20000)//bit 17
#define UART_CTRL_PAR_ODD_POS                    ((uint32_t)0x11)//bit 17
#define UART_CTRL_PAR_ODD_SET(x)                 (((uint32_t)(((uint32_t)(x))<<UART_CTRL_PAR_ODD_POS))&UART_CTRL_PAR_ODD_MASK)

//__IO uint32_t SBNS                : 1;  /* bit[20]    */
#define UART_BAUD_TWO_STOP_MASK                  ((uint32_t)0x100000)//bit 20
#define UART_BAUD_TWO_STOP_POS                   ((uint32_t)0x14)//bit 20
#define UART_BAUD_TWO_STOP_SET(x)                (((uint32_t)(((uint32_t)(x))<<UART_BAUD_TWO_STOP_POS))&UART_BAUD_TWO_STOP_MASK)

// __IO uint32_t RXPOL               : 1;  /* bit[9]     */
#define UART_STAT_RX_REVERSE_MASK                ((uint32_t)0x200)//bit 9
#define UART_STAT_RX_REVERSE_POS                 ((uint32_t)0x9)//bit 9
#define UART_STAT_RX_REVERSE_SET(x)              (((uint32_t)(((uint32_t)(x))<<UART_STAT_RX_REVERSE_POS))&UART_STAT_RX_REVERSE_MASK)

 //__IO uint32_t TXPOL               : 1;  /* bit[1]     */
#define UART_CTRL_TX_REVERSE_MASK                ((uint32_t)0x1)//bit 1
#define UART_CTRL_TX_REVERSE_POS                 ((uint32_t)0x1)//bit 1
#define UART_CTRL_TX_REVERSE_SET(x)              (((uint32_t)(((uint32_t)(x))<<UART_CTRL_TX_REVERSE_POS))&UART_CTRL_TX_REVERSE_MASK)

// __IO uint32_t MSB                 : 1;  /* bit[21]    */
#define UART_STAT_MSB_EN_MASK                    ((uint32_t)0x00200000)//bit 21
#define UART_STAT_MSB_EN_POS                     ((uint32_t)0x15)//bit 21
#define UART_STAT_MSB_EN_SET(x)                  (((uint32_t)(((uint32_t)(x))<<UART_STAT_MSB_EN_POS))&UART_STAT_MSB_EN_MASK)


// __IO uint32_t TXE                 : 1;  /* bit[0]     */
#define UART_CTRL_TXEN_MASK                      ((uint32_t)0x0)//bit 0
#define UART_CTRL_TXEN_POS                       ((uint32_t)0x0)//bit 0
#define UART_CTRL_TXEN_SET(x)                    (((uint32_t)(((uint32_t)(x))<<UART_CTRL_TXEN_POS))&UART_CTRL_TXEN_MASK)

//__IO uint32_t TXFE                : 1;  /* bit[5]     */
#define UART_CTRL_TX_FIFO_EN_MASK                 ((uint32_t)0x20)//bit 5
#define UART_CTRL_TX_FIFO_EN_POS                  ((uint32_t)0x5)//bit 5
#define UART_CTRL_TX_FIFO_EN_SET(x)               (((uint32_t)(((uint32_t)(x))<<UART_CTRL_TX_FIFO_EN_POS))&UART_CTRL_TX_FIFO_EN_MASK)

//__IO uint32_t RXE                 : 1;  /* bit[8]     */
#define UART_CTRL_RXEN_MASK                      ((uint32_t)0x100)//bit 8
#define UART_CTRL_RXEN_POS                       ((uint32_t)0x8)//bit 8
#define UART_CTRL_RXEN_SET(x)                    (((uint32_t)(((uint32_t)(x))<<UART_CTRL_RXEN_POS))&UART_CTRL_RXEN_MASK)

//__IO uint32_t RXFE                : 1;  /* bit[13]    */
#define UART_CTRL_RX_FIFO_EN_MASK                 ((uint32_t)0x2000)//bit 13
#define UART_CTRL_RX_FIFO_EN_POS                  ((uint32_t)0xd)//bit 13
#define UART_CTRL_RX_FIFO_EN_SET(x)               (((uint32_t)(((uint32_t)(x))<<UART_CTRL_RX_FIFO_EN_POS))&UART_CTRL_RX_FIFO_EN_MASK)

 //__IO uint32_t RXFLUSH             : 1;  /* bit[14]    */
#define UART_CTRL_RXFLUSH_FIFO_CLR_MASK                 ((uint32_t)0x4000)//bit 14
#define UART_CTRL_RXFLUSH_FIFO_CLR_POS                  ((uint32_t)0xe)//bit 14
#define UART_CTRL_RXFLUSH_FIFO_CLR_SET(x)               (((uint32_t)(((uint32_t)(x))<<UART_CTRL_RXFLUSH_FIFO_CLR_POS))&UART_CTRL_RXFLUSH_FIFO_CLR_MASK)


/* LPUART0 Register Structure */		 //pjh add 26-0409
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
    } VERID_STR; 

    /* 0x04: Status Register */
//    union {
//        struct {
//            __IO uint32_t TXFIFOSIZE         : 5;  /* bit[4:0]    发送FIFO深度（0~31） */
//            __IO uint32_t RESERVED_7_5       : 3;  /* bit[7:5]    保留位 */
//            __IO uint32_t RXFIFOSIZE         : 5;  /* bit[12:8]   接收FIFO深度（0~31） */
//            __IO uint32_t RESERVED_15_13     : 3;  /* bit[15:13]  保留位 */
//            __IO uint32_t TXEMPT             : 1;  /* bit[16]     发送FIFO空标志 */
//            __IO uint32_t TXFULL             : 1;  /* bit[17]     发送FIFO满标志 */
//            __IO uint32_t RXEMPT             : 1;  /* bit[18]     接收FIFO空标志 */
//            __IO uint32_t RXFULL             : 1;  /* bit[19]     接收FIFO满标志 */
//            __IO uint32_t RESERVED_31_20     : 12; /* bit[31:20]  保留位 */
//        };
//        __IO uint32_t w;
//    } SR_STR; 
		union {
        struct {
            __IO uint32_t TXFIFOSIZE         : 5;  /* bit[4:0]    发送FIFO深度（0~31） */
            __IO uint32_t RESERVED_7_5       : 3;  /* bit[7:5]    保留位 */
            __IO uint32_t RXFIFOSIZE         : 5;  /* bit[12:8]   接收FIFO深度（0~31） */
            __IO uint32_t RESERVED_15_13     : 3;  /* bit[15:13]  保留位 */
            __IO uint32_t TXEMPT             : 1;  /* bit[16]     发送FIFO空标志 */
            __IO uint32_t TXFULL             : 1;  /* bit[17]     发送FIFO满标志 */
            __IO uint32_t RXEMPT             : 1;  /* bit[18]     接收FIFO空标志 */
            __IO uint32_t RXFULL             : 1;  /* bit[19]     接收FIFO满标志 */
            __IO uint32_t TXDWF    			     : 1; /* bit[20]    */
					  __IO uint32_t RXDWF    			     : 1; /* bit[21]    */
        };
        __IO uint32_t w;
    } SR_STR; 
    /* 0x08: Interrupt Register */
//    union {
//        struct {
//            __IO uint32_t TXOVF              : 1;  /* bit[0]     发送溢出中断标志 */
//            __IO uint32_t TXUDF              : 1;  /* bit[1]     发送欠载中断标志 */
//            __IO uint32_t RESERVED_7_2       : 6;  /* bit[7:2]   保留位 */
//            __IO uint32_t RXOVF              : 1;  /* bit[8]     接收溢出中断标志 */
//            __IO uint32_t RXUDF              : 1;  /* bit[9]     接收欠载中断标志 */
//            __IO uint32_t RESERVED_15_10     : 6;  /* bit[15:10] 保留位 */
//            __IO uint32_t RFEF               : 1;  /* bit[16]    接收帧错误标志 */
//            __IO uint32_t RPEF               : 1;  /* bit[17]    接收奇偶错误标志 */
//            __IO uint32_t RNSF               : 1;  /* bit[18]    接收噪声错误标志 */
//            __IO uint32_t RDLF               : 1;  /* bit[19]    接收数据丢失标志 */
//            __IO uint32_t RESERVED_27_20     : 8;  /* bit[27:20] 保留位 */
//            __IO uint32_t MA2F               : 1;  /* bit[28]    第二匹配标志 */
//            __IO uint32_t MA1F               : 1;  /* bit[29]    第一匹配标志 */
//            __IO uint32_t RXEDGIF            : 1;  /* bit[30]    接收边沿中断标志 */
//            __IO uint32_t LBKDIF             : 1;  /* bit[31]    回环检测中断标志 */
//        };
//        __IO uint32_t w;
//    } IR_STR; 
		union {
        struct {
          __IO uint32_t TXOVF    		  	: 1;  /* bit[0]  发送FIFO OverFlow标志位 */
					__IO uint32_t TXUDF     		  : 1;  /* bit[1]  发送FIFO UnderFlow标志位 */
					__IO uint32_t RESERVED_3_2		: 2; /* bit[3:2] 保留位 */
					__IO uint32_t TXDF       			: 1;  /* bit[4]  发送FIFO到达水位标志位 */
					__IO uint32_t RESERVED_7_5		: 3; /* bit[7:5] 保留位 */
					__IO uint32_t RXOVF     			: 1;  /* bit[8]  接收FIFO OverFlow标志位 */
					__IO uint32_t RXUDF     			: 1;  /* bit[9]  接收FIFO UnderFlow标志位 */
					__IO uint32_t RESERVED_11_10	:2;/* bit[11:10] 保留位 */
					__IO uint32_t RRXDF      			: 1;  /* bit[12] 接收FIFO到达水位标志位 */
					__IO uint32_t RESERVED_15_13	:3;/* bit[15:13] 保留位 */
					__IO uint32_t RFEF      			: 1;  /* bit[16] 接收数据帧格式错误标志位 */
					__IO uint32_t RPEF       		  : 1;  /* bit[17] 接收数据校验错误标志位 */
					__IO uint32_t RNSF            : 1;  /* bit[18] 接收数据干扰标志位 */
					__IO uint32_t RESERVED_30_19  :12;/* bit[30:19] 保留位（覆盖bit19~bit30所有灰色Res） */
					__IO uint32_t LBKDF      	   	: 1;  /* bit[31] 检测到LIN帧头标志位 */
        };
        __IO uint32_t w;
    } IR_STR;
    /* 0x0C: Interrupt Enable Register */
//    union {
//        struct {
//            __IO uint32_t TXOVIE             : 1;  /* bit[0]     */
//            __IO uint32_t TXUDIE             : 1;  /* bit[1]     */
//            __IO uint32_t TXFEIE             : 1;  /* bit[2]     */
//            __IO uint32_t TXFFIE             : 1;  /* bit[3]     */
//            __IO uint32_t RESERVED_7_4       : 4;  /* bit[7:4]   */
//            __IO uint32_t RXOVIE             : 1;  /* bit[8]     */

//            __IO uint32_t RXUDIE             : 1;  /* bit[9]     */
//            __IO uint32_t RXFEIE             : 1;  /* bit[10]    */
//            __IO uint32_t RXFFIE             : 1;  /* bit[11]    */
//            __IO uint32_t RESERVED_19_16     : 4;  /* bit[15:12] */

//            __IO uint32_t RFEIE              : 1;  /* bit[16]    */
//            __IO uint32_t RPEIE              : 1;  /* bit[17]    */
//            __IO uint32_t RNSIE              : 1;  /* bit[18]    */
//            __IO uint32_t RDLIE              : 1;  /* bit[19]    */
//            __IO uint32_t RESERVED_29_20     : 10; /* bit[29:20] */
//           
//            __IO uint32_t RXEDGIE            : 1;  /* bit[20]    */
//            __IO uint32_t LBKDIE             : 1;  /* bit[21]    */
//        };
//        __IO uint32_t w;
//    } IER_STR; 
		/* 0x0C: Interrupt Enable Register - 严格对照表格位域 */
		union {
				struct {
						__IO uint32_t TXOVIE    				: 1;  /* bit[0]  发送FIFO OverFlow中断使能 */
						__IO uint32_t TXUDIE    				: 1;  /* bit[1]  发送FIFO UnderFlow中断使能 */
						__IO uint32_t TXFEIE    				: 1;  /* bit[2]  发送FIFO Empty中断使能 */
						__IO uint32_t TXFFIE    				: 1;  /* bit[3]  发送FIFO Full中断使能 */
						__IO uint32_t TXDFFIE   				: 1;  /* bit[4]  发送FIFO到达水位线中断使能 */
						__IO uint32_t RESERVED_7_5			: 3; /* bit[7:5] 保留位 */
						__IO uint32_t RXOVIE    				: 1;  /* bit[8]  接收FIFO OverFlow中断使能 */
						__IO uint32_t RXUDIE    				: 1;  /* bit[9]  接收FIFO UnderFlow中断使能 */
						__IO uint32_t RXFEIE    				: 1;  /* bit[10] 接收FIFO Empty中断使能 */
						__IO uint32_t RXFFIE    				: 1;  /* bit[11] 接收FIFO Full中断使能 */
						__IO uint32_t RXDFFIE   				: 1;  /* bit[12] 接收FIFO到达水位线中断使能 */
						__IO uint32_t RESERVED_15_13		: 3;/* bit[15:13] 保留位 */
						__IO uint32_t RFEIE     				: 1;  /* bit[16] 接收数据帧格式错误中断使能 */
						__IO uint32_t RPEIE     				: 1;  /* bit[17] 接收数据校验错误中断使能 */
						__IO uint32_t RNSIE     				: 1;  /* bit[18] 接收数据干扰中断使能 */
						__IO uint32_t RESERVED_30_19		:12;/* bit[30:19] 保留位 */
						__IO uint32_t LBKDIE     				: 1;  /* bit[31] LIN帧头检测中断使能 */
				};
				__IO uint32_t w;
		} IER_STR;	
    /* 0x10: Baud Rate Register */
    union {
        struct {
            __IO uint32_t BR                  : 13; /* bit[12:0]  波特率分频系数（0~8191） */
            __IO uint32_t RESERVED_15_13      : 3;  /* bit[15:13] 保留位 */
            __IO uint32_t OSR                 : 5;  /* bit[20:16] 过采样率（0~31） */
            __IO uint32_t RESERVED_31_21      : 11; /* bit[31:21] 保留位 */
        };
        __IO uint32_t w;
    } BRR_STR; 



    /* 0x14: Data Register */
//    union {
//        struct {
//            __IO uint32_t DATA                : 10; /* bit[9:0]   收发数据（支持9/10位） */
//            __IO uint32_t RESERVED_15_10      : 6;  /* bit[15:10] 保留位 */
//            __IO uint32_t RFE                 : 1;  /* bit[16]    接收帧错误状态 */
//            __IO uint32_t RPE                 : 1;  /* bit[17]    接收奇偶错误状态 */
//            __IO uint32_t RNS                 : 1;  /* bit[18]    接收噪声错误状态 */
//            __IO uint32_t IDL                 : 1;  /* bit[19]    空闲帧检测状态 */
//            __IO uint32_t RESERVED_31_20      : 12; /* bit[31:20] 保留位 */
//        };
//        __IO uint32_t w;
//    } DR_STR; 
		union {
        struct {
            __IO uint32_t DATA                : 10; /* bit[9:0]   收发数据（支持9/10位） */
 
            __IO uint32_t RESERVED_31_10      : 22; /* bit[31:20] 保留位 */
        };
        __IO uint32_t w;
    } DR_STR; 
    /* 0x18: Control Register */
    union {
        struct {
            __IO uint32_t TXE                 : 1;  /* bit[0]     */
            __IO uint32_t TXPOL               : 1;  /* bit[1]     */
            __IO uint32_t TXDIR               : 1;  /* bit[2]     */
            __IO uint32_t TXFRAM              : 1;  /* bit[3]     */
            __IO uint32_t TDMAE               : 1;  /* bit[4]     */

            __IO uint32_t TXFE                : 1;  /* bit[5]     */
            __IO uint32_t TXFLUSH             : 1;  /* bit[6]     */
            __IO uint32_t RESERVED_7          : 1;  /* bit[7]     */
            __IO uint32_t RXE                 : 1;  /* bit[8]     */
            __IO uint32_t RXPOL               : 1;  /* bit[9]     */
            __IO uint32_t RESERVED_11_10      : 2;  /* bit[11:10] */
            __IO uint32_t RDMAE               : 1;  /* bit[12]    */
            __IO uint32_t RXFE                : 1;  /* bit[13]    */
            __IO uint32_t RXFLUSH             : 1;  /* bit[14]    */
            __IO uint32_t RESERVED_15         : 1;  /* bit[15]    */
            __IO uint32_t PE                  : 1;  /* bit[16]    */
            __IO uint32_t PT                  : 1;  /* bit[17]    */
            __IO uint32_t M9                  : 1;  /* bit[18]    */
            __IO uint32_t M10                 : 1;  /* bit[19]    */
            __IO uint32_t SBNS                : 1;  /* bit[20]    */
            __IO uint32_t MSB                 : 1;  /* bit[21]    */
            __IO uint32_t SWE                 : 1;  /* bit[22]    */
            __IO uint32_t RESERVED_23         : 1;  /* bit[23]    */
            __IO uint32_t SENBRK              : 1;  /* bit[24]    */
            __IO uint32_t LENBRK              : 1;  /* bit[25]    */
            __IO uint32_t DETBRK              : 1;  /* bit[26]    */
            __IO uint32_t RESERVED_30_27      : 3;  /* bit[30:27] */
            __IO uint32_t LPM                 : 1;  /* bit[31]    */
           
        };
        __IO uint32_t w;
    } CR_STR; //

    /* 0x1C: Match Register */
//    union {
//        struct {
//            __IO uint32_t MATCH1              : 10; /* bit[9:0]   */
//            __IO uint32_t RESERVED_11_10      : 2;  /* bit[11:10] */
//            __IO uint32_t MAEN1               : 1;  /* bit[12]    */
//            __IO uint32_t MAEN2               : 1;  /* bit[13]    */
//            __IO uint32_t RESERVED_15_14      : 2;  /* bit[15:14] */
//            __IO uint32_t MATCH2              : 1; /* bit[16] */
//            __IO uint32_t RESERVED_18_17      : 2;  /* bit[18:17] */
//            __IO uint32_t MATCFG              : 2;  /* bit[20:19] */
//            __IO uint32_t RESERVED_31_21      : 11;  /* bit[31:21] */
//        };
//        __IO uint32_t w;
//    } MATCH_STR; 
		__IO uint32_t RESERVED_16;  /* 0x1C: Reserved */
    __IO uint32_t RESERVED_20;  /* 0x20: Reserved */
    __IO uint32_t RESERVED_24;  /* 0x24: Reserved */
    __IO uint32_t RESERVED_28;  /* 0x28: Reserved */

    /* 0x2C: Watermark Register */
//    union {
//        struct {
//            __IO uint32_t RXWATER             : 2;  /* bit[1:0]   */
//            __IO uint32_t RESERVED_3_2        : 2;  /* bit[3:2]   */
//            __IO uint32_t TXWATER             : 2;  /* bit[5:4]   */
//            __IO uint32_t RESERVED_7_6        : 2;  /* bit[7:6]   */
//            __IO uint32_t RTSWATER            : 2;  /* bit[9:8]   */
//            __IO uint32_t RESERVED_15_10      : 6;  /* bit[15:10] */
//            __IO uint32_t RXCOUNT             : 3;  /* bit[18:16] */
//            __IO uint32_t RESERVED_19         : 1;  /* bit[19]    */
//            __IO uint32_t TXCOUNT             : 3;  /* bit[22:20] */
//            __IO uint32_t RESERVED_31_23      : 9;  /* bit[31:23] */
//        };
//        __IO uint32_t w;
//    } WATER_STR; 
			union {   
        struct {
            __IO uint32_t TXWATER             : 5;  /* bit[4:0]   发送FIFO水位阈值 */
            __IO uint32_t RESERVED_7_5        : 3;  /* bit[7:5]   保留位 */
            __IO uint32_t TXSIZE              : 5;  /* bit[12:8]  发送FIFO深度 */
            __IO uint32_t RESERVED_15_13      : 3;  /* bit[15:13] 保留位 */
            __IO uint32_t RXWATER             : 5;  /* bit[20:16] 接收FIFO水位阈值 */
            __IO uint32_t RESERVED_23_21      : 3;  /* bit[23:21] 保留位 */
            __IO uint32_t RXSIZE              : 5;  /* bit[28:24] 接收FIFO深度 */
            __IO uint32_t RESERVED_31_29      : 3;  /* bit[31:29] 保留位 */
        };
        __IO uint32_t w;
    } WATER_STR;
} UART_TypeDef;


#endif /* UART_REG_H */
