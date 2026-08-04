#ifndef GPIO_REG_H
#define GPIO_REG_H


//#include "core_cm0plus.h"

#if 0
//PA------------Start----------------
typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned MINVN      :8;
        unsigned MAJVN      :8;
        unsigned FEATURE    :16;
    } Bits;
} PA_VERID_STR;
#define _PA_VERID           (*(PA_VERID_STR*)(GPIOA_BASE + 0x00)
#define PA_VERID            (_PA_VERID).Word
#define PA_VERID_MINVN      (_PA_VERID).Bits.MINVN
#define PA_VERID_MAJVN      (_PA_VERID).Bits.MAJVN
#define PA_VERID_FEATURE    (_PA_VERID).Bits.FEATURE


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DR0        :1;
        unsigned DR1         :1;
        unsigned DR2         :1;
        unsigned DR3         :1;
        unsigned DR4         :1;
        unsigned DR5         :1;
        unsigned DR6         :1;
        unsigned DR7         :1;
        unsigned DR8         :1;
        unsigned DR9         :1;
        unsigned DR10         :1;
        unsigned DR11         :1;
        unsigned DR12         :1;
        unsigned DR13         :1;
        unsigned DR14         :1;
        unsigned DR15         :1;
        unsigned              :16;
    } Bits;
} PA_DR_STR;
#define _PA_DR          (*(PA_DR_STR*)(GPIOA_BASE + 0x04)
#define PA_DR           (_PA_DR).Word
#define PA_DR0          (_PA_DR).Bits.DR0
#define PA_DR1          (_PA_DR).Bits.DR1
#define PA_DR2          (_PA_DR).Bits.DR2
#define PA_DR3          (_PA_DR).Bits.DR3
#define PA_DR_DR4       (_PA_DR).Bits.DR4
#define PA_DR_DR5       (_PA_DR).Bits.DR5
#define PA_DR_DR6       (_PA_DR).Bits.DR6
#define PA_DR_DR7       (_PA_DR).Bits.DR7
#define PA_DR_DR8       (_PA_DR).Bits.DR8
#define PA_DR_DR9       (_PA_DR).Bits.DR9
#define PA_DR_DR10      (_PA_DR).Bits.DR10
#define PA_DR_DR11      (_PA_DR).Bits.DR11
#define PA_DR_DR12      (_PA_DR).Bits.DR12
#define PA_DR_DR13      (_PA_DR).Bits.DR13
#define PA_DR_DR14      (_PA_DR).Bits.DR14
#define PA_DR_DR15      (_PA_DR).Bits.DR15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DRS0 : 1;
        unsigned DRS1 : 1;
        unsigned DRS2 : 1;
        unsigned DRS3 : 1;
        unsigned DRS4 : 1;
        unsigned DRS5 : 1;
        unsigned DRS6 : 1;
        unsigned DRS7 : 1;
        unsigned DRS8 : 1;
        unsigned DRS9 : 1;
        unsigned DRS10 : 1;
        unsigned DRS11 : 1;
        unsigned DRS12 : 1;
        unsigned DRS13 : 1;
        unsigned DRS14 : 1;
        unsigned DRS15 : 1;
        unsigned : 16;
    } Bits;
} PA_DRS_STR;
#define _PA_DRS      *(PA_DRS_STR*)(GPIOA_BASE + 0x08)
#define PA_DRS       (_PA_DRS).Word
#define PA_DRS_DRS0  (_PA_DRS).Bits.DRS0
#define PA_DRS_DRS1  (_PA_DRS).Bits.DRS1
#define PA_DRS_DRS2  (_PA_DRS).Bits.DRS2
#define PA_DRS_DRS3  (_PA_DRS).Bits.DRS3
#define PA_DRS_DRS4  (_PA_DRS).Bits.DRS4
#define PA_DRS_DRS5  (_PA_DRS).Bits.DRS5
#define PA_DRS_DRS6  (_PA_DRS).Bits.DRS6
#define PA_DRS_DRS7  (_PA_DRS).Bits.DRS7
#define PA_DRS_DRS8  (_PA_DRS).Bits.DRS8
#define PA_DRS_DRS9  (_PA_DRS).Bits.DRS9
#define PA_DRS_DRS10 (_PA_DRS).Bits.DRS10
#define PA_DRS_DRS11 (_PA_DRS).Bits.DRS11
#define PA_DRS_DRS12 (_PA_DRS).Bits.DRS12
#define PA_DRS_DRS13 (_PA_DRS).Bits.DRS13
#define PA_DRS_DRS14 (_PA_DRS).Bits.DRS14
#define PA_DRS_DRS15 (_PA_DRS).Bits.DRS15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DRC0 :1;
        unsigned DRC1 :1;
        unsigned DRC2 :1;
        unsigned DRC3 :1;
        unsigned DRC4 :1;
        unsigned DRC5 :1;
        unsigned DRC6 :1;
        unsigned DRC7 :1;
        unsigned DRC8 :1;
        unsigned DRC9 :1;
        unsigned DRC10 :1;
        unsigned DRC11 :1;
        unsigned DRC12 :1;
        unsigned DRC13 :1;
        unsigned DRC14 :1;
        unsigned DRC15 :1;
        unsigned :16;
    } Bits;
} PA_DRC_STR;
#define _PA_DRC      *( PA_DRC_STR *)(GPIOA_BASE + 0x0C)
#define PA_DRC       (_PA_DRC).Word
#define PA_DRC_DRC0  (_PA_DRC).Bits.DRC0
#define PA_DRC_DRC1  (_PA_DRC).Bits.DRC1
#define PA_DRC_DRC2  (_PA_DRC).Bits.DRC2
#define PA_DRC_DRC3  (_PA_DRC).Bits.DRC3
#define PA_DRC_DRC4  (_PA_DRC).Bits.DRC4
#define PA_DRC_DRC5  (_PA_DRC).Bits.DRC5
#define PA_DRC_DRC6  (_PA_DRC).Bits.DRC6
#define PA_DRC_DRC7  (_PA_DRC).Bits.DRC7
#define PA_DRC_DRC8  (_PA_DRC).Bits.DRC8
#define PA_DRC_DRC9  (_PA_DRC).Bits.DRC9
#define PA_DRC_DRC10 (_PA_DRC).Bits.DRC10
#define PA_DRC_DRC11 (_PA_DRC).Bits.DRC11
#define PA_DRC_DRC12 (_PA_DRC).Bits.DRC12
#define PA_DRC_DRC13 (_PA_DRC).Bits.DRC13
#define PA_DRC_DRC14 (_PA_DRC).Bits.DRC14
#define PA_DRC_DRC15 (_PA_DRC).Bits.DRC15

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DRT0 :1;
        unsigned DRT1 :1;
        unsigned DRT2 :1;
        unsigned DRT3 :1;
        unsigned DRT4 :1;
        unsigned DRT5 :1;
        unsigned DRT6 :1;
        unsigned DRT7 :1;
        unsigned DRT8 :1;
        unsigned DRT9 :1;
        unsigned DRT10 :1;
        unsigned DRT11 :1;
        unsigned DRT12 :1;
        unsigned DRT13 :1;
        unsigned DRT14 :1;
        unsigned DRT15 :1;
        unsigned :16;
    } Bits;
} PA_DRT_STR;
#define _PA_DRT         *(PA_DRT_STR *)(GPIOA_BASE + 0x10)
#define PA_DRT          (_PA_DRT).Word
#define PA_DRT_DRT0     (_PA_DRT).Bits.DRT0
#define PA_DRT_DRT1     (_PA_DRT).Bits.DRT1
#define PA_DRT_DRT2     (_PA_DRT).Bits.DRT2
#define PA_DRT_DRT3     (_PA_DRT).Bits.DRT3
#define PA_DRT_DRT4     (_PA_DRT).Bits.DRT4
#define PA_DRT_DRT5     (_PA_DRT).Bits.DRT5
#define PA_DRT_DRT6     (_PA_DRT).Bits.DRT6
#define PA_DRT_DRT7     (_PA_DRT).Bits.DRT7
#define PA_DRT_DRT8     (_PA_DRT).Bits.DRT8
#define PA_DRT_DRT9     (_PA_DRT).Bits.DRT9
#define PA_DRT_DRT10    (_PA_DRT).Bits.DRT10
#define PA_DRT_DRT11    ( _PA_DRT ).Bits.DRT11
#define PA_DRT_DRT12    ( _PA_DRT ).Bits.DRT12
#define PA_DRT_DRT13    ( _PA_DRT ).Bits.DRT13
#define PA_DRT_DRT14    ( _PA_DRT ).Bits.DRT14
#define PA_DRT_DRT15     ( _PA_DRT ).Bits.DRT15

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned OES0       :1;
        unsigned OES1       :1;
        unsigned OES2       :1;
        unsigned OES3       :1;
        unsigned OES4       :1; 
        unsigned OES5       :1;
        unsigned OES6       :1;
        unsigned OES7       :1;
        unsigned OES8       :1;
        unsigned OES9       :1;
        unsigned OES10       :1;
        unsigned OES11       :1;
        unsigned OES12       :1;
        unsigned OES13       :1;
        unsigned OES14       :1;
        unsigned OES15       :1;
        unsigned             :16;
    } Bits;
} PA_OES_STR;
#define _PA_OES             *( PA_OES_STR *)(GPIOA_BASE + 0x14)
#define PA_OES              ( _PA_OES).Word
#define PA_OES_OES0         ( _PA_OES).Bits.OES0
#define PA_OES_OES1         ( _PA_OES).Bits.OES1
#define PA_OES_OES2         ( _PA_OES).Bits.OES2
#define PA_OES_OES3         ( _PA_OES).Bits.OES3
#define PA_OES_OES4         ( _PA_OES).Bits.OES4
#define PA_OES_OES5         ( _PA_OES).Bits.OES5
#define PA_OES_OES6         ( _PA_OES).Bits.OES6
#define PA_OES_OES7         ( _PA_OES).Bits.OES7
#define PA_OES_OES8         ( _PA_OES).Bits.OES8
#define PA_OES_OES9         ( _PA_OES).Bits.OES9
#define PA_OES_OES10        ( _PA_OES).Bits.OES10
#define PA_OES_OES11        ( _PA_OES).Bits.OES11
#define PA_OES_OES12        ( _PA_OES).Bits.OES12
#define PA_OES_OES13        ( _PA_OES).Bits.OES13
#define PA_OES_OES14        (_PA_OES).Bits.OES14
#define PA_OES_OES15        (_PA_OES).Bits.OES15

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned OEC0 : 1;
        unsigned OEC1 : 1;
        unsigned OEC2 : 1;
        unsigned OEC3 : 1;
        unsigned OEC4 : 1;
        unsigned OEC5 : 1;
        unsigned OEC6 : 1;
        unsigned OEC7 : 1;
        unsigned OEC8 : 1;
        unsigned OEC9 : 1;
        unsigned OEC10 : 1;
        unsigned OEC11 : 1;
        unsigned OEC12 : 1;
        unsigned OEC13 : 1;
        unsigned OEC14 : 1;
        unsigned OEC15 : 1;
        unsigned :16;
    } Bits;
} PA_OEC_STR;

#define _PA_OEC               (*(PA_OEC_STR*)(GPIOA_BASE + 0x18))
#define PA_OEC                (_PA_OEC).Word
#define PA_OEC_OEC0           (_PA_OEC).Bits.OEC0
#define PA_OEC_OEC1           (_PA_OEC).Bits.OEC1
#define PA_OEC_OEC2           (_PA_OEC).Bits.OEC2
#define PA_OEC_OEC3           (_PA_OEC).Bits.OEC3
#define PA_OEC_OEC4           (_PA_OEC).Bits.OEC4
#define PA_OEC_OEC5           (_PA_OEC).Bits.OEC5
#define PA_OEC_OEC6           (_PA_OEC).Bits.OEC6
#define PA_OEC_OEC7           (_PA_OEC).Bits.OEC7
#define PA_OEC_OEC8           (_PA_OEC).Bits.OEC8
#define PA_OEC_OEC9           (_PA_OEC).Bits.OEC9
#define PA_OEC_OEC10          (_PA_OEC).Bits.OEC10
#define PA_OEC_OEC11          (_PA_OEC).Bits.OEC11
#define PA_OEC_OEC12           (_PA_OEC).Bits.OEC12
#define PA_OEC_OEC13           (_PA_OEC).Bits.OEC13
#define PA_OEC_OEC14           (_PA_OEC).Bits.OEC14
#define PA_OEC_OEC15           (_PA_OEC).Bits.OEC15
typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ITES0      :1;
        unsigned ITES1      :1;
        unsigned ITES2      :1;
        unsigned ITES3      :1;
        unsigned ITES4      :1;
        unsigned ITES5      :1;
        unsigned ITES6      :1;
        unsigned ITES7      :1;
        unsigned ITES8      :1;
        unsigned ITES9      :1;
        unsigned ITES10     :1;
        unsigned ITES11     :1;
        unsigned ITES12     :1;
        unsigned ITES13     :1;
        unsigned ITES14     :1;
        unsigned ITES15     :1;
        unsigned            :16;
    } Bits;
} PA_ITES_STR;
#define _PA_ITES             *(PA_ITES_STR *)(GPIOA_BASE + 0x1C)
#define PA_ITES             (_PA_ITES).Word
#define PA_ITES_ITES0       (_PA_ITES).Bits.ITES0
#define PA_ITES_ITES1       (_PA_ITES).Bits.ITES1
#define PA_ITES_ITES2       (_PA_ITES).Bits.ITES2
#define PA_ITES_ITES3       (_PA_ITES).Bits.ITES3
#define PA_ITES_ITES4       (_PA_ITES).Bits.ITES4
#define PA_ITES_ITES5       (_PA_ITES).Bits.ITES5
#define PA_ITES_ITES6       (_PA_ITES).Bits.ITES6
#define PA_ITES_ITES7       (_PA_ITES).Bits.ITES7
#define PA_ITES_ITES8       (_PA_ITES).Bits.ITES8
#define PA_ITES_ITES9       (_PA_ITES).Bits.ITES9
#define PA_ITES_ITES10      (_PA_ITES).Bits.ITES10
#define PA_ITES_ITES11      (_PA_ITES).Bits.ITES11
#define PA_ITES_ITES12      (_PA_ITES).Bits.ITES12
#define PA_ITES_ITES13      (_PA_ITES).Bits.ITES13
#define PA_ITES_ITES14      (_PA_ITES).Bits.ITES14
#define PA_ITES_ITES15      (_PA_ITES).Bits.ITES15

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ITEC0      :1;
        unsigned ITEC1      :1;
        unsigned ITEC2      :1;
        unsigned ITEC3      :1;
        unsigned ITEC4      :1;
        unsigned ITEC5      :1;
        unsigned ITEC6      :1;
        unsigned ITEC7      :1;
        unsigned ITEC8      :1;
        unsigned ITEC9      :1;
        unsigned ITEC10     :1;
        unsigned ITEC11     :1;
        unsigned ITEC12     :1;
        unsigned ITEC13     :1;
        unsigned ITEC14     :1;
        unsigned ITEC15     :1;
        unsigned            :16;
    } Bits;
} PA_ITEC_STR;
#define _PA_ITEC            *(PA_ITEC_STR*)(GPIOA_BASE + 0x20)
#define PA_ITEC             (_PA_ITEC).Word
#define PA_ITEC_ITEC0       (_PA_ITEC).Bits.ITEC0
#define PA_ITEC_ITEC1       (_PA_ITEC).Bits.ITEC1
#define PA_ITEC_ITEC2       (_PA_ITEC).Bits.ITEC2
#define PA_ITEC_ITEC3       (_PA_ITEC).Bits.ITEC3
#define PA_ITEC_ITEC4       (_PA_ITEC).Bits.ITEC4
#define PA_ITEC_ITEC5       (_PA_ITEC).Bits.ITEC5
#define PA_ITEC_ITEC6       (_PA_ITEC).Bits.ITEC6
#define PA_ITEC_ITEC7       (_PA_ITEC).Bits.ITEC7
#define PA_ITEC_ITEC8       (_PA_ITEC).Bits.ITEC8
#define PA_ITEC_ITEC9       (_PA_ITEC).Bits.ITEC9
#define PA_ITEC_ITEC10      (_PA_ITEC).Bits.ITEC10
#define PA_ITEC_ITEC11      (_PA_ITEC).Bits.ITEC11
#define PA_ITEC_ITEC12      (_PA_ITEC).Bits.ITEC12
#define PA_ITEC_ITEC13      (_PA_ITEC).Bits.ITEC13
#define PA_ITEC_ITEC14      (_PA_ITEC).Bits.ITEC14
#define PA_ITEC_ITEC15      (_PA_ITEC).Bits.ITEC15

typedef volatile union
{
    volatile unsigned int Word;
    struct
    {
        unsigned ITT0   : 3;
        unsigned        : 1;
        unsigned ITT1   : 3;
        unsigned        : 1;
        unsigned ITT2   : 3;
        unsigned        : 1;
        unsigned ITT3   : 3;
        unsigned        : 1;
        unsigned ITT4   : 3;
        unsigned        : 1;
        unsigned ITT5   : 3;
        unsigned        : 1;
        unsigned ITT6   : 3;
        unsigned        : 1;
        unsigned ITT7   : 3;
        unsigned        : 1;
    } Bits;
} PA_ITT0_STR;
#define _PA_ITT0            *(PA_ITT0_STR *)(GPIOA_BASE + 0x24)
#define PA_ITT0             (_PA_ITT0).Word
#define PA_ITT0_ITT00       (_PA_ITT0).Bits.ITT0
#define PA_ITT0_ITT01       (_PA_ITT0).Bits.ITT1
#define PA_ITT0_ITT02       (_PA_ITT0).Bits.ITT2
#define PA_ITT0_ITT03       (_PA_ITT0).Bits.ITT3
#define PA_ITT0_ITT04       (_PA_ITT0).Bits.ITT4
#define PA_ITT0_ITT05       (_PA_ITT0).Bits.ITT5
#define PA_ITT0_ITT06       (_PA_ITT0).Bits.ITT6
#define PA_ITT0_ITT07       (_PA_ITT0).Bits.ITT7

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ITT8       :3;
        unsigned           :1;
        unsigned ITT9       :3;
        unsigned           :1;
        unsigned ITT10      :3;
        unsigned           :1;
        unsigned ITT11      :3;
        unsigned           :1;
        unsigned ITT12      :3;
        unsigned           :1;
        unsigned ITT13      :3;
        unsigned           :1;
        unsigned ITT14      :3;
        unsigned           :1;
        unsigned ITT15      :3;
        unsigned           :1;
    } Bits;
} PA_ITT1_STR;
#define _PA_ITT1            *(PA_ITT1_STR *)(GPIOA_BASE + 0x28)
#define PA_ITT1             (_PA_ITT1).Word
#define PA_ITT1_ITT8        (_PA_ITT1).Bits.ITT8
#define PA_ITT1_ITT9        (_PA_ITT1).Bits.ITT9
#define PA_ITT1_ITT10       (_PA_ITT1).Bits.ITT10
#define PA_ITT1_ITT11       (_PA_ITT1).Bits.ITT11
#define PA_ITT1_ITT12       (_PA_ITT1).Bits.ITT12
#define PA_ITT1_ITT13       (_PA_ITT1).Bits.ITT13
#define PA_ITT1_ITT14       (_PA_ITT1).Bits.ITT14
#define PA_ITT1_ITT15       (_PA_ITT1).Bits.ITT15

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ITTC0      :1;
        unsigned ITTC1      :1;
        unsigned ITTC2      :1;
        unsigned ITTC3      :1;
        unsigned ITTC4      :1;
        unsigned ITTC5      :1;
        unsigned ITTC6      :1;
        unsigned ITTC7      :1;
        unsigned ITTC8      :1;
        unsigned ITTC9      :1;
        unsigned ITTC10     :1;
        unsigned ITTC11     :1;
        unsigned ITTC12     :1;
        unsigned ITTC13     :1;
        unsigned ITTC14     :1;
        unsigned ITTC15     :1;
        unsigned            :16;
    } Bits;
} PA_ITTC_STR;
#define _PA_ITTC            *(PA_ITTC_STR *)(GPIOA_BASE + 0x34)
#define PA_ITTC             (_PA_ITTC).Word
#define PA_ITTC_ITTC0       (_PA_ITTC).Bits.ITTC0
#define PA_ITTC_ITTC1       (_PA_ITTC).Bits.ITTC1
#define PA_ITTC_ITTC2       (_PA_ITTC).Bits.ITTC2
#define PA_ITTC_ITTC3       (_PA_ITTC).Bits.ITTC3
#define PA_ITTC_ITTC4       (_PA_ITTC).Bits.ITTC4
#define PA_ITTC_ITTC5       (_PA_ITTC).Bits.ITTC5
#define PA_ITTC_ITTC6       (_PA_ITTC).Bits.ITTC6
#define PA_ITTC_ITTC7       (_PA_ITTC).Bits.ITTC7
#define PA_ITTC_ITTC8       (_PA_ITTC).Bits.ITTC8
#define PA_ITTC_ITTC9       (_PA_ITTC).Bits.ITTC9
#define PA_ITTC_ITTC10      (_PA_ITTC).Bits.ITTC10
#define PA_ITTC_ITTC11      (_PA_ITTC).Bits.ITTC11
#define PA_ITTC_ITTC12      (_PA_ITTC).Bits.ITTC12
#define PA_ITTC_ITTC13      (_PA_ITTC).Bits.ITTC13
#define PA_ITTC_ITTC14      (_PA_ITTC).Bits.ITTC14
#define PA_ITTC_ITTC15      (_PA_ITTC).Bits.ITTC15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ITS0 :1;
        unsigned ITS1 :1;
        unsigned ITS2 :1;
        unsigned ITS3 :1;
        unsigned ITS4 :1;
        unsigned ITS5 :1;
        unsigned ITS6 :1;
        unsigned ITS7 :1;
        unsigned ITS8 :1;
        unsigned ITS9 :1;
        unsigned ITS10 :1;
        unsigned ITS11 :1;
        unsigned ITS12 :1;
        unsigned ITS13 :1;
        unsigned ITS14 :1;
        unsigned ITS15 :1;
        unsigned       :16;
    } Bits;
} PA_ITSR_STR;
#define _PA_ITSR        *(PA_ITSR_STR*)(GPIOA_BASE + 0x38)
#define PA_ITSR         (_PA_ITSR).Word
#define PA_ITSR_ITS0    (_PA_ITSR).Bits.ITS0
#define PA_ITSR_ITS1    (_PA_ITSR).Bits.ITS1
#define PA_ITSR_ITS2    (_PA_ITSR).Bits.ITS2
#define PA_ITSR_ITS3    (_PA_ITSR).Bits.ITS3
#define PA_ITSR_ITS4    (_PA_ITSR).Bits.ITS4
#define PA_ITSR_ITS5    (_PA_ITSR).Bits.ITS5
#define PA_ITSR_ITS6    (_PA_ITSR).Bits.ITS6
#define PA_ITSR_ITS7    (_PA_ITSR).Bits.ITS7
#define PA_ITSR_ITS8    (_PA_ITSR).Bits.ITS8
#define PA_ITSR_ITS9    (_PA_ITSR).Bits.ITS9
#define PA_ITSR_ITS10   (_PA_ITSR).Bits.ITS10
#define PA_ITSR_ITS11   (_PA_ITSR).Bits.ITS11
#define PA_ITSR_ITS12   (_PA_ITSR).Bits.ITS12
#define PA_ITSR_ITS13   (_PA_ITSR).Bits.ITS13
#define PA_ITSR_ITS14   (_PA_ITSR).Bits.ITS14
#define PA_ITSR_ITS15   (_PA_ITSR).Bits.ITS15

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned AF0    :3;
        unsigned        :1;
        unsigned AF1    :3;
        unsigned        :1;
        unsigned AF2    :3;
        unsigned        :1;
        unsigned AF3    :3;
        unsigned        :1;
        unsigned AF4    :3;
        unsigned        :1;
        unsigned AF5    :3;
        unsigned        :1;
        unsigned AF6    :3;
        unsigned        :1;
        unsigned AF7     :3;
        unsigned         :1;
    } Bits;
} PA_AF0_STR;
#define _PA_AF0     *(PA_AF0_STR*)(GPIOA_BASE+0x3C)
#define PA_AF0      (_PA_AF0).Word
#define PA_AF0_AF0  (_PA_AF0).Bits.AF0
#define PA_AF0_AF1  (_PA_AF0).Bits.AF1
#define PA_AF0_AF2  (_PA_AF0).Bits.AF2
#define PA_AF0_AF3  (_PA_AF0).Bits.AF3
#define PA_AF0_AF4  (_PA_AF0).Bits.AF4
#define PA_AF0_AF5  (_PA_AF0).Bits.AF5
#define PA_AF0_AF6  (_PA_AF0).Bits.AF6
#define PA_AF0_AF7  (_PA_AF0).Bits.AF7


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned AF8    :3;
        unsigned        :1;
        unsigned AF9    :3;
        unsigned        :1;
        unsigned AF10   :3;
        unsigned        :1;
        unsigned AF11   :3;
        unsigned        :1;
        unsigned AF12   :3;
        unsigned        :1;
        unsigned AF13   :3;
        unsigned        :1;
        unsigned AF14   :3;
        unsigned        :1;
        unsigned AF15   :3;
        unsigned        :1;
    } Bits;
} PA_AF1_STR;
#define _PA_AF1         *(PA_AF1_STR *)(GPIOA_BASE + 0x40)
#define PA_AF1          (_PA_AF1).Word
#define PA_AF1_AF8      (_PA_AF1).Bits.AF8
#define PA_AF1_AF9      (_PA_AF1).Bits.AF9
#define PA_AF1_AF10     (_PA_AF1).Bits.AF10
#define PA_AF1_AF11     (_PA_AF1).Bits.AF11
#define PA_AF1_AF12     (_PA_AF1).Bits.AF12
#define PA_AF1_AF13     (_PA_AF1).Bits.AF13
#define PA_AF1_AF14     (_PA_AF1).Bits.AF14
#define PA_AF1_AF15     (_PA_AF1).Bits.AF15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned AFCC0 :1;
        unsigned AFCC1 :1;
        unsigned AFC2 :1;
        unsigned AFC3 :1;
        unsigned AFC4 :1;
        unsigned AFC5 :1;
        unsigned AFC6 :1;
        unsigned AFC7 :1;
        unsigned AFC8 :1;
        unsigned AFC9 :1;
        unsigned AFC10 :1;
        unsigned AFC11 :1;
        unsigned AFC12 :1;
        unsigned AFC13 :1;
        unsigned AFC14 :1;
        unsigned AFC15 :1;
        unsigned       :16;
    } Bits;
} PA_AFC_STR;
#define _PA_AFC         (*PA_AFC_STR*)(GPIOA_BASE + 0x4C)
#define PA_AFC          (_PA_AFC).Word
#define PA_AFC_AFC0     (_PA_AFC).Bits.AFC0
#define PA_AFC_AFC1     (_PA_AFC).Bits.AFC1
#define PA_AFC_AFC2     (_PA_AFC).Bits.AFC2
#define PA_AFC_AFC3     (_PA_AFC).Bits.AFC3
#define PA_AFC_AFC4     (_PA_AFC).Bits.AFC4
#define PA_AFC_AFC5     (_PA_AFC).Bits.AFC5
#define PA_AFC_AFC6     (_PA_AFC).Bits.AFC6
#define PA_AFC_AFC7     (_PA_AFC).Bits.AFC7
#define PA_AFC_AFC8     (_PA_AFC).Bits.AFC8
#define PA_AFC_AFC9     (_PA_AFC).Bits.AFC9
#define PA_AFC_AFC10    (_PA_AFC).Bits.AFC10
#define PA_AFC_AFC11    (_PA_AFC).Bits.AFC11
#define PA_AFC_AFC12    (_PA_AFC).Bits.AFC12
#define PA_AFC_AFC13    (_PA_AFC).Bits.AFC13
#define PA_AFC_AFC14    (_PA_AFC).Bits.AFC14
#define PA_AFC_AFC15    (_PA_AFC).Bits.AFC15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ANA0 :1;
        unsigned ANA1 :1;
        unsigned ANA2 :1;
        unsigned ANA3 :1;
        unsigned ANA4 :1;
        unsigned ANA5 :1;
        unsigned ANA6 :1;
        unsigned ANA7 :1;
        unsigned ANA8 :1;
        unsigned ANA9 :1;
        unsigned ANA10 :1;
        unsigned ANA11 :1;
        unsigned ANA12 :1;
        unsigned ANA13 :1;
        unsigned ANA14 :1;
        unsigned ANA15 :1;
        unsigned       :16;
    } Bits;
} PA_ANA_STR;
#define _PA_ANA         (*(PA_ANA_STR *)(GPIOA_BASE + 0x50)
#define PA_ANA          (_PA_ANA).Word
#define PA_ANA_ANA0     (_PA_ANA).Bits.ANA0
#define PA_ANA_ANA1     (_PA_ANA).Bits.ANA1
#define PA_ANA_ANA2     (_PA_ANA).Bits.ANA2
#define PA_ANA_ANA3     (_PA_ANA).Bits.ANA3
#define PA_ANA_ANA4     (_PA_ANA).Bits.ANA4
#define PA_ANA_ANA5     (_PA_ANA).Bits.ANA5
#define PA_ANA_ANA6     (_PA_ANA).Bits.ANA6
#define PA_ANA_ANA7     (_PA_ANA).Bits.ANA7
#define PA_ANA_ANA8     (_PA_ANA).Bits.ANA8
#define PA_ANA_ANA9     (_PA_ANA).Bits.ANA9
#define PA_ANA_ANA10    (_PA_ANA).Bits.ANA10
#define PA_ANA_ANA11    (_PA_ANA).Bits.ANA11
#define PA_ANA_ANA12    (_PA_ANA).Bits.ANA12
#define PA_ANA_ANA13    (_PA_ANA).Bits.ANA13
#define PA_ANA_ANA14    (_PA_ANA).Bits.ANA14
#define PA_ANA_ANA15    (_PA_ANA).Bits.ANA15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ANAC0 :1;
        unsigned ANAC1 :1;
        unsigned ANAC2 :1;
        unsigned ANAC3 :1;
        unsigned ANAC4 :1;
        unsigned ANAC5 :1;
        unsigned ANAC6 :1;
        unsigned ANAC7 :1;
        unsigned ANAC8 :1;
        unsigned ANAC9 :1;
        unsigned ANAC10 :1;
        unsigned ANAC11 :1;
        unsigned ANAC12 :1;
        unsigned ANAC13 :1;
        unsigned ANAC14 :1;
        unsigned ANAC15 :1;
        unsigned        :16;
    } Bits;
} PA_ANAC_STR;
#define _PA_ANAC            *(PA_ANAC_STR*)(GPIOA_BASE + 0x54)
#define PA_ANAC             (_PA_ANAC).Word
#define PA_ANAC_ANAC0       (_PA_ANAC).Bits.ANAC0
#define PA_ANAC_ANAC1       (_PA_ANAC).Bits.ANAC1
#define PA_ANAC_ANAC2       (_PA_ANAC).Bits.ANAC2
#define PA_ANAC_ANAC3       (_PA_ANAC).Bits.ANAC3
#define PA_ANAC_ANAC4       (_PA_ANAC).Bits.ANAC4
#define PA_ANAC_ANAC5       (_PA_ANAC).Bits.ANAC5
#define PA_ANAC_ANAC6       (_PA_ANAC).Bits.ANAC6
#define PA_ANAC_ANAC7       (_PA_ANAC).Bits.ANAC7
#define PA_ANAC_ANAC8       (_PA_ANAC).Bits.ANAC8
#define PA_ANAC_ANAC9       (_PA_ANAC).Bits.ANAC9
#define PA_ANAC_ANAC10      (_PA_ANAC).Bits.ANAC10
#define PA_ANAC_ANAC11      (_PA_ANAC).Bits.ANAC11
#define PA_ANAC_ANAC12      (_PA_ANAC).Bits.ANAC12
#define PA_ANAC_ANAC13      (_PA_ANAC).Bits.ANAC13
#define PA_ANAC_ANAC14      (_PA_ANAC).Bits.ANAC14
#define PA_ANAC_ANAC15      (_PA_ANAC).Bits.ANAC15


typedef volatile union{
    volatile unsigned int Word;
    struct{
        unsigned PU0 : 1;
        unsigned PU1 : 1;
        unsigned PU2 : 1;
        unsigned PU3 : 1;
        unsigned PU4 : 1;
        unsigned PU5 : 1;
        unsigned PU6 : 1;
        unsigned PU7 : 1;
        unsigned PU8 : 1;
        unsigned PU9 : 1;
        unsigned PU10 : 1;
        unsigned PU11 : 1;
        unsigned PU12 : 1;
        unsigned PU13 : 1;
        unsigned PU14 : 1;
        unsigned PU15 : 1;
        unsigned      : 16;
    } Bits;
} PA_PU_STR;
#define _PA_PU      *(PA_PU_STR *)(GPIOA_BASE + 0x58)
#define PA_PU       (_PA_PU).Word
#define PA_PU_PU0   (_PA_PU).Bits.PU0
#define PA_PU_PU1   (_PA_PU).Bits.PU1
#define PA_PU_PU2   (_PA_PU).Bits.PU2
#define PA_PU_PU3   (_PA_PU).Bits.PU3
#define PA_PU_PU4   (_PA_PU).Bits.PU4
#define PA_PU_PU5   (_PA_PU).Bits.PU5
#define PA_PU_PU6   (_PA_PU).Bits.PU6
#define PA_PU_PU7   (_PA_PU).Bits.PU7
#define PA_PU_PU8   (_PA_PU).Bits.PU8
#define PA_PU_PU9   (_PA_PU).Bits.PU9
#define PA_PU_PU10  (_PA_PU).Bits.PU10
#define PA_PU_PU11  (_PA_PU).Bits.PU11
#define PA_PU_PU12  (_PA_PU).Bits.PU12
#define PA_PU_PU13  (_PA_PU).Bits.PU13
#define PA_PU_PU14  (_PA_PU).Bits.PU14
#define PA_PU_PU15  (_PA_PU).Bits.PU15


typedef volatile union{
    volatile unsigned int Word;
    struct{
        unsigned SPU0 : 1;
        unsigned SPU1 : 1;
        unsigned SPU2 : 1;
        unsigned SPU3 : 1;
        unsigned SPU4 : 1;
        unsigned SPU5 : 1;
        unsigned SPU6 : 1;
        unsigned SPU7 : 1;
        unsigned SPU8 : 1;
        unsigned SPU9 : 1;
        unsigned SPU10 : 1;
        unsigned SPU11 : 1;
        unsigned SPU12 : 1;
        unsigned SPU13 : 1;
        unsigned SPU14 : 1;
        unsigned SPU15 : 1;
        unsigned       : 16;
    } Bits;
} PA_SPU_STR;
#define _PA_SPU         (*(PA_SPU_STR *)(GPIOA_BASE + 0x5C)
#define PA_SPU          (_PA_SPU).Word
#define PA_SPU_SPU0     (_PA_SPU).Bits.SPU0
#define PA_SPU_SPU1     (_PA_SPU).Bits.SPU1
#define PA_SPU_SPU2     (_PA_SPU).Bits.SPU2
#define PA_SPU_SPU3     (_PA_SPU).Bits.SPU3
#define PA_SPU_SPU4     (_PA_SPU).Bits.SPU4
#define PA_SPU_SPU5     (_PA_SPU).Bits.SPU5
#define PA_SPU_SPU6     (_PA_SPU).Bits.SPU6
#define PA_SPU_SPU7     (_PA_SPU).Bits.SPU7
#define PA_SPU_SPU8     (_PA_SPU).Bits.SPU8
#define PA_SPU_SPU9     (_PA_SPU).Bits.SPU9
#define PA_SPU_SPU10    (_PA_SPU).Bits.SPU10
#define PA_SPU_SPU11    (_PA_SPU).Bits.SPU11
#define PA_SPU_SPU12    (_PA_SPU).Bits.SPU12
#define PA_SPU_SPU13    (_PA_SPU).Bits.SPU13
#define PA_SPU_SPU14    (_PA_SPU).Bits.SPU14
#define PA_SPU_SPU15    (_PA_SPU).Bits.SPU15


typedef volatile union{
    volatile unsigned int Word;
    struct{
        unsigned PUC0 : 1;
        unsigned PUC1 : 1;
        unsigned PUC2 : 1;
        unsigned PUC3 : 1;
        unsigned PUC4 : 1;
        unsigned PUC5 : 1;
        unsigned PUC6 : 1;
        unsigned PUC7 : 1;
        unsigned PUC8 : 1;
        unsigned PUC9 : 1;
        unsigned PUC10 : 1;
        unsigned PUC11 : 1;
        unsigned PUC12 : 1;
        unsigned PUC13 : 1;
        unsigned PUC14 : 1;
        unsigned PUC15 : 1;
        unsigned       : 16;
    } Bits;
} PA_PUC_STR;
#define _PA_PUC         (*(PA_PUC_STR *)(GPIOA_BASE + 0x60))
#define PA_PUC          (_PA_PUC).Word
#define PA_PUC_PUC0     (_PA_PUC).Bits.PUC0
#define PA_PUC_PUC1     (_PA_PUC).Bits.PUC1
#define PA_PUC_PUC2     (_PA_PUC).Bits.PUC2
#define PA_PUC_PUC3     (_PA_PUC).Bits.PUC3
#define PA_PUC_PUC4     (_PA_PUC).Bits.PUC4
#define PA_PUC_PUC5     (_PA_PUC).Bits.PUC5
#define PA_PUC_PUC6     (_PA_PUC).Bits.PUC6
#define PA_PUC_PUC7     (_PA_PUC).Bits.PUC7
#define PA_PUC_PUC8     (_PA_PUC).Bits.PUC8
#define PA_PUC_PUC9     (_PA_PUC).Bits.PUC9
#define PA_PUC_PUC10    (_PA_PUC).Bits.PUC10
#define PA_PUC_PUC11    (_PA_PUC).Bits.PUC11
#define PA_PUC_PUC12    (_PA_PUC).Bits.PUC12
#define PA_PUC_PUC13    (_PA_PUC).Bits.PUC13
#define PA_PUC_PUC14    (_PA_PUC).Bits.PUC14
#define PA_PUC_PUC15    (_PA_PUC).Bits.PUC15


typedef volatile union{
    volatile unsigned int Word;
    struct{
        unsigned PD0 : 1;
        unsigned PD1 : 1;
        unsigned PD2 : 1;
        unsigned PD3 : 1;
        unsigned PD4 : 1;
        unsigned PD5 : 1;
        unsigned PD6 : 1;
        unsigned PD7 : 1;
        unsigned PD8 : 1;
        unsigned PD9 : 1;
        unsigned PD10 : 1;
        unsigned PD11 : 1;
        unsigned PD12 : 1;
        unsigned PD13 : 1;
        unsigned PD14 : 1;
        unsigned PD15 : 1;
        unsigned      : 16;
    } Bits;
} PA_PD_STR;
#define _PA_PD      *(PA_PD_STR *)(GPIOA_BASE + 0x64)
#define PA_PD       (_PA_PD).Word
#define PA_PD_PD0   (_PA_PD).Bits.PD0
#define PA_PD_PD1   (_PA_PD).Bits.PD1
#define PA_PD_PD2   (_PA_PD).Bits.PD2
#define PA_PD_PD3   (_PA_PD).Bits.PD3
#define PA_PD_PD4   (_PA_PD).Bits.PD4
#define PA_PD_PD5   (_PA_PD).Bits.PD5
#define PA_PD_PD6   (_PA_PD).Bits.PD6
#define PA_PD_PD7   (_PA_PD).Bits.PD7
#define PA_PD_PD8   (_PA_PD).Bits.PD8
#define PA_PD_PD9   (_PA_PD).Bits.PD9
#define PA_PD_PD10  (_PA_PD).Bits.PD10
#define PA_PD_PD11  (_PA_PD).Bits.PD11
#define PA_PD_PD12  (_PA_PD).Bits.PD12
#define PA_PD_PD13  (_PA_PD).Bits.PD13
#define PA_PD_PD14  (_PA_PD).Bits.PD14
#define PA_PD_PD15  (_PA_PD).Bits.PD15


typedef volatile union{
    volatile unsigned int Word;
    struct{
        unsigned PDC0 : 1;
        unsigned PDC1 : 1;
        unsigned PDC2 : 1;
        unsigned PDC3 : 1;
        unsigned PDC4 : 1;
        unsigned PDC5 : 1;
        unsigned PDC6 : 1;
        unsigned PDC7 : 1;
        unsigned PDC8 : 1;
        unsigned PDC9 : 1;
        unsigned PDC10 : 1;
        unsigned PDC11 : 1;
        unsigned PDC12 : 1;
        unsigned PDC13 : 1;
        unsigned PDC14 : 1;
        unsigned PDC15 : 1;
        unsigned       : 16;
    } Bits;
} PA_PDC_STR;
#define _PA_PDC              (*(PA_PDC_STR*)(GPIOA_BASE + 0x68)
#define PA_PDC               (_PA_PDC).Word
#define PA_PDC_PDC0          (_PA_PDC).Bits.PDC0
#define PA_PDC_PDC1          (_PA_PDC).Bits.PDC1
#define PA_PDC_PDC2          (_PA_PDC).Bits.PDC2
#define PA_PDC_PDC3          (_PA_PDC).Bits.PDC3
#define PA_PDC_PDC4          (_PA_PDC).Bits.PDC4
#define PA_PDC_PDC5          (_PA_PDC).Bits.PDC5
#define PA_PDC_PDC6          (_PA_PDC).Bits.PDC6
#define PA_PDC_PDC7          (_PA_PDC).Bits.PDC7
#define PA_PDC_PDC8          (_PA_PDC).Bits.PDC8
#define PA_PDC_PDC9          (_PA_PDC).Bits.PDC9
#define PA_PDC_PDC10         (_PA_PDC).Bits.PDC10
#define PA_PDC_PDC11         (_PA_PDC).Bits.PDC11
#define PA_PDC_PDC12          (_PA_PDC).Bits.PDC12
#define PA_PDC_PDC13          (_PA_PDC).Bits.PDC13
#define PA_PDC_PDC14          (_PA_PDC).Bits.PDC14
#define PA_PDC_PDC15          (_PA_PDC).Bits.PDC15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DRVS0     :3;
        unsigned           :1;
        unsigned DRVS1     :3;
        unsigned           :1;
        unsigned DRVS2     :3;
        unsigned           :1;
        unsigned DRVS3     :3;
        unsigned           :1;
        unsigned DRVS4     :3;
        unsigned           :1;
        unsigned DRVS5     :3;
        unsigned           :1;
        unsigned DRVS6     :3;
        unsigned           :1;
        unsigned DRVS7     :3;
        unsigned           :1;
    } Bits;
} PA_DRVS0_STR;
#define _PA_DRVS0          (*(PA_DRVS0_STR *)(GPIOA_BASE + 0x6C)
#define PA_DRVS0           (_PA_DRVS0).Word
#define PA_DRVS0_DRVS0     (_PA_DRVS0).Bits.DRVS0
#define PA_DRVS0_DRVS1     (_PA_DRVS0).Bits.DRVS1
#define PA_DRVS0_DRVS2     (_PA_DRVS0).Bits.DRVS2
#define PA_DRVS0_DRVS3     (_PA_DRVS0).Bits.DRVS3
#define PA_DRVS0_DRVS4     (_PA_DRVS0).Bits.DRVS4
#define PA_DRVS0_DRVS5     (_PA_DRVS0).Bits.DRVS5
#define PA_DRVS0_DRVS6     (_PA_DRVS0).Bits.DRVS6
#define PA_DRVS0_DRVS7     (_PA_DRVS0).Bits.DRVS7


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DRVS8 :3;
        unsigned        :1;
        unsigned DRVS9 :3;
        unsigned        :1;
        unsigned DRVS10 :3;
        unsigned        :1;
        unsigned DRVS11 :3;
        unsigned        :1;
        unsigned DRVS12 :3;
        unsigned        :1;
        unsigned DRVS13 :3;
        unsigned        :1;
        unsigned DRVS14 :3;
        unsigned        :1;
        unsigned DRVS15 :3;
        unsigned        :1;
    } Bits;
} PA_DRVS1_STR;
#define _PA_DRVS1            *(PA_DRVS1_STR*)(GPIOA_BASE + 0x70)
#define PA_DRVS1            (_PA_DRVS1).Word
#define PA_DRVS1_DRVS8      (_PA_DRVS1).Bits.DRVS8
#define PA_DRVS1_DRVS9      (_PA_DRVS1).Bits.DRVS9
#define PA_DRVS1_DRVS10     (_PA_DRVS1).Bits.DRVS10
#define PA_DRVS1_DRVS11     (_PA_DRVS1).Bits.DRVS11
#define PA_DRVS1_DRVS12     (_PA_DRVS1).Bits.DRVS12
#define PA_DRVS1_DRVS13     (_PA_DRVS1).Bits.DRVS13
#define PA_DRVS1_DRVS14     (_PA_DRVS1).Bits.DRVS14
#define PA_DRVS1_DRVS15     (_PA_DRVS1).Bits.DRVS15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DRVC0 :1;
        unsigned DRVC1 :1;
        unsigned DRVC2 :1;
        unsigned DRVC3 :1;
        unsigned DRVC4 :1;
        unsigned DRVC5 :1;
        unsigned DRVC6 :1;
        unsigned DRVC7 :1;
        unsigned DRVC8 :1;
        unsigned DRVC9 :1;
        unsigned DRVC10 :1;
        unsigned DRVC11 :1;
        unsigned DRVC12 :1;
        unsigned DRVC13 :1;
        unsigned DRVC14 :1;
        unsigned DRVC15 :1;
        unsigned        :16;
    } Bits;
} PA_DRVC_STR;
#define _PA_DRVC        (*(PA_DRVC_STR *)(GPIOA_BASE + 0x7C)
#define PA_DRVC         (_PA_DRVC).Word
#define PA_DRVC_DRVC0   (_PA_DRVC).Bits.DRVC0
#define PA_DRVC_DRVC1   (_PA_DRVC).Bits.DRVC1
#define PA_DRVC_DRVC2   (_PA_DRVC).Bits.DRVC2
#define PA_DRVC_DRVC3   (_PA_DRVC).Bits.DRVC3
#define PA_DRVC_DRVC4   (_PA_DRVC).Bits.DRVC4
#define PA_DRVC_DRVC5   (_PA_DRVC).Bits.DRVC5
#define PA_DRVC_DRVC6   (_PA_DRVC).Bits.DRVC6
#define PA_DRVC_DRVC7   (_PA_DRVC).Bits.DRVC7
#define PA_DRVC_DRVC8   (_PA_DRVC).Bits.DRVC8
#define PA_DRVC_DRVC9   (_PA_DRVC).Bits.DRVC9
#define PA_DRVC_DRVC10  (_PA_DRVC).Bits.DRVC10
#define PA_DRVC_DRVC11  (_PA_DRVC).Bits.DRVC11
#define PA_DRVC_DRVC12  (_PA_DRVC).Bits.DRVC12
#define PA_DRVC_DRVC13  (_PA_DRVC).Bits.DRVC13
#define PA_DRVC_DRVC14  (_PA_DRVC).Bits.DRVC14
#define PA_DRVC_DRVC15  (_PA_DRVC).Bits.DRVC15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned SL0 :1;
        unsigned SL1 :1;
        unsigned SL2 :1;
        unsigned SL3 :1;
        unsigned SL4 :1;
        unsigned SL5 :1;
        unsigned SL6 :1;
        unsigned SL7 :1;
        unsigned SL8 :1;
        unsigned SL9 :1;
        unsigned SL10 :1;
        unsigned SL11 :1;
        unsigned SL12 :1;
        unsigned SL13 :1;
        unsigned SL14 :1;
        unsigned SL15 :1;
        unsigned      :16;
    } Bits;
} PA_SLS_STR;
#define _PA_SLS         *(_PA_SLS_STR*)(GPIOA_BASE+0x80)
#define PA_SLS          (_PA_SLS).Word
#define PA_SLS_SL0      (_PA_SLS).Bits.SL0
#define PA_SLS_SL1      (_PA_SLS).Bits.SL1
#define PA_SLS_SL2      (_PA_SLS).Bits.SL2
#define PA_SLS_SL3      (_PA_SLS).Bits.SL3
#define PA_SLS_SL4      (_PA_SLS).Bits.SL4
#define PA_SLS_SL5      (_PA_SLS).Bits.SL5
#define PA_SLS_SL6      (_PA_SLS).Bits.SL6
#define PA_SLS_SL7      (_PA_SLS).Bits.SL7
#define PA_SLS_SL8      (_PA_SLS).Bits.SL8
#define PA_SLS_SL9      (_PA_SLS).Bits.SL9
#define PA_SLS_SL10     (_PA_SLS).Bits.SL10
#define PA_SLS_SL11     (_PA_SLS).Bits.SL11
#define PA_SLS_SL12     (_PA_SLS).Bits.SL12
#define PA_SLS_SL13     (_PA_SLS).Bits.SL13
#define PA_SLS_SL14     (_PA_SLS).Bits.SL14
#define PA_SLS_SL15     (_PA_SLS).Bits.SL15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned SLC0 :1;
        unsigned SLC1 :1;
        unsigned SLC2 :1;
        unsigned SLC3 :1;
        unsigned SLC4 :1;
        unsigned SLC5 :1;
        unsigned SLC6 :1;
        unsigned SLC7 :1;
        unsigned SLC8 :1;
        unsigned SLC9 :1;
        unsigned SLC10 :1;
        unsigned SLC11 :1;
        unsigned SLC12 :1;
        unsigned SLC13 :1;
        unsigned SLC14 :1;
        unsigned SLC15 :1;
        unsigned       :16;
    } Bits;
} PA_SLC_STR;
#define _PA_SLC         *( PA_SLC_STR *)(GPIOA_BASE + 0x84)
#define PA_SLC          (_PA_SLC).Word
#define PA_SLC_SLC0     (_PA_SLC).Bits.SLC0
#define PA_SLC_SLC1     (_PA_SLC).Bits.SLC1
#define PA_SLC_SLC2     (_PA_SLC).Bits.SLC2
#define PA_SLC_SLC3     (_PA_SLC).Bits.SLC3
#define PA_SLC_SLC4     (_PA_SLC).Bits.SLC4
#define PA_SLC_SLC5     (_PA_SLC).Bits.SLC5
#define PA_SLC_SLC6     (_PA_SLC).Bits.SLC6
#define PA_SLC_SLC7     (_PA_SLC).Bits.SLC7
#define PA_SLC_SLC8     (_PA_SLC).Bits.SLC8
#define PA_SLC_SLC9     (_PA_SLC).Bits.SLC9
#define PA_SLC_SLC10    (_PA_SLC).Bits.SLC10
#define PA_SLC_SLC11    (_PA_SLC).Bits.SLC11
#define PA_SLC_SLC12    (_PA_SLC).Bits.SLC12
#define PA_SLC_SLC13    (_PA_SLC).Bits.SLC13
#define PA_SLC_SLC14    (_PA_SLC).Bits.SLC14
#define PA_SLC_SLC15    (_PA_SLC).Bits.SLC15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ST0 :1;
        unsigned ST1 :1;
        unsigned ST2 :1;
        unsigned ST3 :1;
        unsigned ST4 :1;
        unsigned ST5 :1;
        unsigned ST6 :1;
        unsigned ST7 :1;
        unsigned ST8 :1;
        unsigned ST9 :1;
        unsigned ST10 :1;
        unsigned ST11 :1;
        unsigned ST12 :1;
        unsigned ST13 :1;
        unsigned ST14 :1;
        unsigned ST15 :1;
        unsigned :16;
    } Bits;
} PA_STS_STR;
#define _PA_STS         (*PA_STS_STR *)(GPIOA_BASE + 0x88)
#define PA_STS          (_PA_STS).Word
#define PA_STS_ST0      (_PA_STS).Bits.ST0
#define PA_STS_ST1      (_PA_STS).Bits.ST1
#define PA_STS_ST2      (_PA_STS).Bits.ST2
#define PA_STS_ST3      (_PA_STS).Bits.ST3
#define PA_STS_ST4      (_PA_STS).Bits.ST4
#define PA_STS_ST5      (_PA_STS).Bits.ST5
#define PA_STS_ST6      (_PA_STS).Bits.ST6
#define PA_STS_ST7      (_PA_STS).Bits.ST7
#define PA_STS_ST8      (_PA_STS).Bits.ST8
#define PA_STS_ST9      (_PA_STS).Bits.ST9
#define PA_STS_ST10     (_PA_STS).Bits.ST10
#define PA_STS_ST11     (_PA_STS).Bits.ST11
#define PA_STS_ST12     (_PA_STS).Bits.ST12
#define PA_STS_ST13     (_PA_STS).Bits.ST13
#define PA_STS_ST14     (_PA_STS).Bits.ST14
#define PA_STS_ST15     (_PA_STS).Bits.ST15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned STC0 :1;
        unsigned STC1 :1;
        unsigned STC2 :1;
        unsigned STC3 :1;
        unsigned STC4 :1;
        unsigned STC5 :1;
        unsigned STC6 :1;
        unsigned STC7 :1;
        unsigned STC8 :1;
        unsigned STC9 :1;
        unsigned STC10 :1;
        unsigned STC11 :1;
        unsigned STC12 :1;
        unsigned STC13 :1;
        unsigned STC14 :1;
        unsigned STC15 :1;
        unsigned       :16;
    } Bits;
} PA_STC_STR;
#define _PA_STC         *( PA_STC_STR *)(GPIOA_Base + 0x8C)
#define PA_STC          (_PA_STC).Word
#define PA_STC_STC0     (_PA_STC).Bits.STC0
#define PA_STC_STC1     (_PA_STC).Bits.STC1
#define PA_STC_STC2     (_PA_STC).Bits.STC2
#define PA_STC_STC3     (_PA_STC).Bits.STC3
#define PA_STC_STC4     (_PA_STC).Bits.STC4
#define PA_STC_STC5     (_PA_STC).Bits.STC5
#define PA_STC_STC6     (_PA_STC).Bits.STC6
#define PA_STC_STC7     (_PA_STC).Bits.STC7
#define PA_STC_STC8     (_PA_STC).Bits.STC8
#define PA_STC_STC9     (_PA_STC).Bits.STC9
#define PA_STC_STC10    (_PA_STC).Bits.STC10
#define PA_STC_STC11    (_PA_STC).Bits.STC11
#define PA_STC_STC12    (_PA_STC).Bits.STC12
#define PA_STC_STC13    (_PA_STC).Bits.STC13
#define PA_STC_STC14    (_PA_STC).Bits.STC14
#define PA_STC_STC15    (_PA_STC).Bits.STC15
//PA_------------End----------------



//PB_------------Start----------------
typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned MINVN      :8;
        unsigned MAJVN      :8;
        unsigned FEATURE    :16;
    } Bits;
} PB_VERID_STR;
#define _PB_VERID           (*(PB_VERID_STR*)(GPIOB_BASE + 0x00)
#define PB_VERID            (_PB_VERID).Word
#define PB_VERID_MINVN      (_PB_VERID).Bits.MINVN
#define PB_VERID_MAJVN      (_PB_VERID).Bits.MAJVN
#define PB_VERID_FEATURE    (_PB_VERID).Bits.FEATURE


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DR0        :1;
        unsigned DR1         :1;
        unsigned DR2         :1;
        unsigned DR3         :1;
        unsigned DR4         :1;
        unsigned DR5         :1;
        unsigned DR6         :1;
        unsigned DR7         :1;
        unsigned DR8         :1;
        unsigned DR9         :1;
        unsigned DR10         :1;
        unsigned DR11         :1;
        unsigned DR12         :1;
        unsigned DR13         :1;
        unsigned DR14         :1;
        unsigned DR15         :1;
        unsigned              :16;
    } Bits;
} PB_DR_STR;
#define _PB_DR          (*(PB_DR_STR*)(GPIOB_BASE + 0x04)
#define PB_DR           (_PB_DR).Word
#define PB_DR0          (_PB_DR).Bits.DR0
#define PB_DR1          (_PB_DR).Bits.DR1
#define PB_DR2          (_PB_DR).Bits.DR2
#define PB_DR3          (_PB_DR).Bits.DR3
#define PB_DR_DR4       (_PB_DR).Bits.DR4
#define PB_DR_DR5       (_PB_DR).Bits.DR5
#define PB_DR_DR6       (_PB_DR).Bits.DR6
#define PB_DR_DR7       (_PB_DR).Bits.DR7
#define PB_DR_DR8       (_PB_DR).Bits.DR8
#define PB_DR_DR9       (_PB_DR).Bits.DR9
#define PB_DR_DR10      (_PB_DR).Bits.DR10
#define PB_DR_DR11      (_PB_DR).Bits.DR11
#define PB_DR_DR12      (_PB_DR).Bits.DR12
#define PB_DR_DR13      (_PB_DR).Bits.DR13
#define PB_DR_DR14      (_PB_DR).Bits.DR14
#define PB_DR_DR15      (_PB_DR).Bits.DR15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DRS0 : 1;
        unsigned DRS1 : 1;
        unsigned DRS2 : 1;
        unsigned DRS3 : 1;
        unsigned DRS4 : 1;
        unsigned DRS5 : 1;
        unsigned DRS6 : 1;
        unsigned DRS7 : 1;
        unsigned DRS8 : 1;
        unsigned DRS9 : 1;
        unsigned DRS10 : 1;
        unsigned DRS11 : 1;
        unsigned DRS12 : 1;
        unsigned DRS13 : 1;
        unsigned DRS14 : 1;
        unsigned DRS15 : 1;
        unsigned : 16;
    } Bits;
} PB_DRS_STR;
#define _PB_DRS      *(PB_DRS_STR*)(GPIOB_BASE + 0x08)
#define PB_DRS       (_PB_DRS).Word
#define PB_DRS_DRS0  (_PB_DRS).Bits.DRS0
#define PB_DRS_DRS1  (_PB_DRS).Bits.DRS1
#define PB_DRS_DRS2  (_PB_DRS).Bits.DRS2
#define PB_DRS_DRS3  (_PB_DRS).Bits.DRS3
#define PB_DRS_DRS4  (_PB_DRS).Bits.DRS4
#define PB_DRS_DRS5  (_PB_DRS).Bits.DRS5
#define PB_DRS_DRS6  (_PB_DRS).Bits.DRS6
#define PB_DRS_DRS7  (_PB_DRS).Bits.DRS7
#define PB_DRS_DRS8  (_PB_DRS).Bits.DRS8
#define PB_DRS_DRS9  (_PB_DRS).Bits.DRS9
#define PB_DRS_DRS10 (_PB_DRS).Bits.DRS10
#define PB_DRS_DRS11 (_PB_DRS).Bits.DRS11
#define PB_DRS_DRS12 (_PB_DRS).Bits.DRS12
#define PB_DRS_DRS13 (_PB_DRS).Bits.DRS13
#define PB_DRS_DRS14 (_PB_DRS).Bits.DRS14
#define PB_DRS_DRS15 (_PB_DRS).Bits.DRS15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DRC0 :1;
        unsigned DRC1 :1;
        unsigned DRC2 :1;
        unsigned DRC3 :1;
        unsigned DRC4 :1;
        unsigned DRC5 :1;
        unsigned DRC6 :1;
        unsigned DRC7 :1;
        unsigned DRC8 :1;
        unsigned DRC9 :1;
        unsigned DRC10 :1;
        unsigned DRC11 :1;
        unsigned DRC12 :1;
        unsigned DRC13 :1;
        unsigned DRC14 :1;
        unsigned DRC15 :1;
        unsigned :16;
    } Bits;
} PB_DRC_STR;
#define _PB_DRC      *( PB_DRC_STR *)(GPIOB_BASE + 0x0C)
#define PB_DRC       (_PB_DRC).Word
#define PB_DRC_DRC0  (_PB_DRC).Bits.DRC0
#define PB_DRC_DRC1  (_PB_DRC).Bits.DRC1
#define PB_DRC_DRC2  (_PB_DRC).Bits.DRC2
#define PB_DRC_DRC3  (_PB_DRC).Bits.DRC3
#define PB_DRC_DRC4  (_PB_DRC).Bits.DRC4
#define PB_DRC_DRC5  (_PB_DRC).Bits.DRC5
#define PB_DRC_DRC6  (_PB_DRC).Bits.DRC6
#define PB_DRC_DRC7  (_PB_DRC).Bits.DRC7
#define PB_DRC_DRC8  (_PB_DRC).Bits.DRC8
#define PB_DRC_DRC9  (_PB_DRC).Bits.DRC9
#define PB_DRC_DRC10 (_PB_DRC).Bits.DRC10
#define PB_DRC_DRC11 (_PB_DRC).Bits.DRC11
#define PB_DRC_DRC12 (_PB_DRC).Bits.DRC12
#define PB_DRC_DRC13 (_PB_DRC).Bits.DRC13
#define PB_DRC_DRC14 (_PB_DRC).Bits.DRC14
#define PB_DRC_DRC15 (_PB_DRC).Bits.DRC15

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DRT0 :1;
        unsigned DRT1 :1;
        unsigned DRT2 :1;
        unsigned DRT3 :1;
        unsigned DRT4 :1;
        unsigned DRT5 :1;
        unsigned DRT6 :1;
        unsigned DRT7 :1;
        unsigned DRT8 :1;
        unsigned DRT9 :1;
        unsigned DRT10 :1;
        unsigned DRT11 :1;
        unsigned DRT12 :1;
        unsigned DRT13 :1;
        unsigned DRT14 :1;
        unsigned DRT15 :1;
        unsigned :16;
    } Bits;
} PB_DRT_STR;
#define _PB_DRT         *(PB_DRT_STR *)(GPIOB_BASE + 0x10)
#define PB_DRT          (_PB_DRT).Word
#define PB_DRT_DRT0     (_PB_DRT).Bits.DRT0
#define PB_DRT_DRT1     (_PB_DRT).Bits.DRT1
#define PB_DRT_DRT2     (_PB_DRT).Bits.DRT2
#define PB_DRT_DRT3     (_PB_DRT).Bits.DRT3
#define PB_DRT_DRT4     (_PB_DRT).Bits.DRT4
#define PB_DRT_DRT5     (_PB_DRT).Bits.DRT5
#define PB_DRT_DRT6     (_PB_DRT).Bits.DRT6
#define PB_DRT_DRT7     (_PB_DRT).Bits.DRT7
#define PB_DRT_DRT8     (_PB_DRT).Bits.DRT8
#define PB_DRT_DRT9     (_PB_DRT).Bits.DRT9
#define PB_DRT_DRT10    (_PB_DRT).Bits.DRT10
#define PB_DRT_DRT11    ( _PB_DRT ).Bits.DRT11
#define PB_DRT_DRT12    ( _PB_DRT ).Bits.DRT12
#define PB_DRT_DRT13    ( _PB_DRT ).Bits.DRT13
#define PB_DRT_DRT14    ( _PB_DRT ).Bits.DRT14
#define PB_DRT_DRT15     ( _PB_DRT ).Bits.DRT15

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned OES0       :1;
        unsigned OES1       :1;
        unsigned OES2       :1;
        unsigned OES3       :1;
        unsigned OES4       :1; 
        unsigned OES5       :1;
        unsigned OES6       :1;
        unsigned OES7       :1;
        unsigned OES8       :1;
        unsigned OES9       :1;
        unsigned OES10       :1;
        unsigned OES11       :1;
        unsigned OES12       :1;
        unsigned OES13       :1;
        unsigned OES14       :1;
        unsigned OES15       :1;
        unsigned             :16;
    } Bits;
} PB_OES_STR;
#define _PB_OES             *( PB_OES_STR *)(GPIOB_BASE + 0x14)
#define PB_OES              ( _PB_OES).Word
#define PB_OES_OES0         ( _PB_OES).Bits.OES0
#define PB_OES_OES1         ( _PB_OES).Bits.OES1
#define PB_OES_OES2         ( _PB_OES).Bits.OES2
#define PB_OES_OES3         ( _PB_OES).Bits.OES3
#define PB_OES_OES4         ( _PB_OES).Bits.OES4
#define PB_OES_OES5         ( _PB_OES).Bits.OES5
#define PB_OES_OES6         ( _PB_OES).Bits.OES6
#define PB_OES_OES7         ( _PB_OES).Bits.OES7
#define PB_OES_OES8         ( _PB_OES).Bits.OES8
#define PB_OES_OES9         ( _PB_OES).Bits.OES9
#define PB_OES_OES10        ( _PB_OES).Bits.OES10
#define PB_OES_OES11        ( _PB_OES).Bits.OES11
#define PB_OES_OES12        ( _PB_OES).Bits.OES12
#define PB_OES_OES13        ( _PB_OES).Bits.OES13
#define PB_OES_OES14        (_PB_OES).Bits.OES14
#define PB_OES_OES15        (_PB_OES).Bits.OES15

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned OEC0 : 1;
        unsigned OEC1 : 1;
        unsigned OEC2 : 1;
        unsigned OEC3 : 1;
        unsigned OEC4 : 1;
        unsigned OEC5 : 1;
        unsigned OEC6 : 1;
        unsigned OEC7 : 1;
        unsigned OEC8 : 1;
        unsigned OEC9 : 1;
        unsigned OEC10 : 1;
        unsigned OEC11 : 1;
        unsigned OEC12 : 1;
        unsigned OEC13 : 1;
        unsigned OEC14 : 1;
        unsigned OEC15 : 1;
        unsigned :16;
    } Bits;
} PB_OEC_STR;

#define _PB_OEC               (*(PB_OEC_STR*)(GPIOB_BASE + 0x18))
#define PB_OEC                (_PB_OEC).Word
#define PB_OEC_OEC0           (_PB_OEC).Bits.OEC0
#define PB_OEC_OEC1           (_PB_OEC).Bits.OEC1
#define PB_OEC_OEC2           (_PB_OEC).Bits.OEC2
#define PB_OEC_OEC3           (_PB_OEC).Bits.OEC3
#define PB_OEC_OEC4           (_PB_OEC).Bits.OEC4
#define PB_OEC_OEC5           (_PB_OEC).Bits.OEC5
#define PB_OEC_OEC6           (_PB_OEC).Bits.OEC6
#define PB_OEC_OEC7           (_PB_OEC).Bits.OEC7
#define PB_OEC_OEC8           (_PB_OEC).Bits.OEC8
#define PB_OEC_OEC9           (_PB_OEC).Bits.OEC9
#define PB_OEC_OEC10          (_PB_OEC).Bits.OEC10
#define PB_OEC_OEC11          (_PB_OEC).Bits.OEC11
#define PB_OEC_OEC12           (_PB_OEC).Bits.OEC12
#define PB_OEC_OEC13           (_PB_OEC).Bits.OEC13
#define PB_OEC_OEC14           (_PB_OEC).Bits.OEC14
#define PB_OEC_OEC15           (_PB_OEC).Bits.OEC15
typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ITES0      :1;
        unsigned ITES1      :1;
        unsigned ITES2      :1;
        unsigned ITES3      :1;
        unsigned ITES4      :1;
        unsigned ITES5      :1;
        unsigned ITES6      :1;
        unsigned ITES7      :1;
        unsigned ITES8      :1;
        unsigned ITES9      :1;
        unsigned ITES10     :1;
        unsigned ITES11     :1;
        unsigned ITES12     :1;
        unsigned ITES13     :1;
        unsigned ITES14     :1;
        unsigned ITES15     :1;
        unsigned            :16;
    } Bits;
} PB_ITES_STR;
#define _PB_ITES             *(PB_ITES_STR *)(GPIOB_BASE + 0x1C)
#define PB_ITES             (_PB_ITES).Word
#define PB_ITES_ITES0       (_PB_ITES).Bits.ITES0
#define PB_ITES_ITES1       (_PB_ITES).Bits.ITES1
#define PB_ITES_ITES2       (_PB_ITES).Bits.ITES2
#define PB_ITES_ITES3       (_PB_ITES).Bits.ITES3
#define PB_ITES_ITES4       (_PB_ITES).Bits.ITES4
#define PB_ITES_ITES5       (_PB_ITES).Bits.ITES5
#define PB_ITES_ITES6       (_PB_ITES).Bits.ITES6
#define PB_ITES_ITES7       (_PB_ITES).Bits.ITES7
#define PB_ITES_ITES8       (_PB_ITES).Bits.ITES8
#define PB_ITES_ITES9       (_PB_ITES).Bits.ITES9
#define PB_ITES_ITES10      (_PB_ITES).Bits.ITES10
#define PB_ITES_ITES11      (_PB_ITES).Bits.ITES11
#define PB_ITES_ITES12      (_PB_ITES).Bits.ITES12
#define PB_ITES_ITES13      (_PB_ITES).Bits.ITES13
#define PB_ITES_ITES14      (_PB_ITES).Bits.ITES14
#define PB_ITES_ITES15      (_PB_ITES).Bits.ITES15

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ITEC0      :1;
        unsigned ITEC1      :1;
        unsigned ITEC2      :1;
        unsigned ITEC3      :1;
        unsigned ITEC4      :1;
        unsigned ITEC5      :1;
        unsigned ITEC6      :1;
        unsigned ITEC7      :1;
        unsigned ITEC8      :1;
        unsigned ITEC9      :1;
        unsigned ITEC10     :1;
        unsigned ITEC11     :1;
        unsigned ITEC12     :1;
        unsigned ITEC13     :1;
        unsigned ITEC14     :1;
        unsigned ITEC15     :1;
        unsigned            :16;
    } Bits;
} PB_ITEC_STR;
#define _PB_ITEC            *(PB_ITEC_STR*)(GPIOB_BASE + 0x20)
#define PB_ITEC             (_PB_ITEC).Word
#define PB_ITEC_ITEC0       (_PB_ITEC).Bits.ITEC0
#define PB_ITEC_ITEC1       (_PB_ITEC).Bits.ITEC1
#define PB_ITEC_ITEC2       (_PB_ITEC).Bits.ITEC2
#define PB_ITEC_ITEC3       (_PB_ITEC).Bits.ITEC3
#define PB_ITEC_ITEC4       (_PB_ITEC).Bits.ITEC4
#define PB_ITEC_ITEC5       (_PB_ITEC).Bits.ITEC5
#define PB_ITEC_ITEC6       (_PB_ITEC).Bits.ITEC6
#define PB_ITEC_ITEC7       (_PB_ITEC).Bits.ITEC7
#define PB_ITEC_ITEC8       (_PB_ITEC).Bits.ITEC8
#define PB_ITEC_ITEC9       (_PB_ITEC).Bits.ITEC9
#define PB_ITEC_ITEC10      (_PB_ITEC).Bits.ITEC10
#define PB_ITEC_ITEC11      (_PB_ITEC).Bits.ITEC11
#define PB_ITEC_ITEC12      (_PB_ITEC).Bits.ITEC12
#define PB_ITEC_ITEC13      (_PB_ITEC).Bits.ITEC13
#define PB_ITEC_ITEC14      (_PB_ITEC).Bits.ITEC14
#define PB_ITEC_ITEC15      (_PB_ITEC).Bits.ITEC15

typedef volatile union
{
    volatile unsigned int Word;
    struct
    {
        unsigned ITT0   : 3;
        unsigned        : 1;
        unsigned ITT1   : 3;
        unsigned        : 1;
        unsigned ITT2   : 3;
        unsigned        : 1;
        unsigned ITT3   : 3;
        unsigned        : 1;
        unsigned ITT4   : 3;
        unsigned        : 1;
        unsigned ITT5   : 3;
        unsigned        : 1;
        unsigned ITT6   : 3;
        unsigned        : 1;
        unsigned ITT7   : 3;
        unsigned        : 1;
    } Bits;
} PB_ITT0_STR;
#define _PB_ITT0            *(PB_ITT0_STR *)(GPIOB_BASE + 0x24)
#define PB_ITT0             (_PB_ITT0).Word
#define PB_ITT0_ITT00       (_PB_ITT0).Bits.ITT0
#define PB_ITT0_ITT01       (_PB_ITT0).Bits.ITT1
#define PB_ITT0_ITT02       (_PB_ITT0).Bits.ITT2
#define PB_ITT0_ITT03       (_PB_ITT0).Bits.ITT3
#define PB_ITT0_ITT04       (_PB_ITT0).Bits.ITT4
#define PB_ITT0_ITT05       (_PB_ITT0).Bits.ITT5
#define PB_ITT0_ITT06       (_PB_ITT0).Bits.ITT6
#define PB_ITT0_ITT07       (_PB_ITT0).Bits.ITT7

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ITT8       :3;
        unsigned           :1;
        unsigned ITT9       :3;
        unsigned           :1;
        unsigned ITT10      :3;
        unsigned           :1;
        unsigned ITT11      :3;
        unsigned           :1;
        unsigned ITT12      :3;
        unsigned           :1;
        unsigned ITT13      :3;
        unsigned           :1;
        unsigned ITT14      :3;
        unsigned           :1;
        unsigned ITT15      :3;
        unsigned           :1;
    } Bits;
} PB_ITT1_STR;
#define _PB_ITT1            *(PB_ITT1_STR *)(GPIOB_BASE + 0x28)
#define PB_ITT1             (_PB_ITT1).Word
#define PB_ITT1_ITT8        (_PB_ITT1).Bits.ITT8
#define PB_ITT1_ITT9        (_PB_ITT1).Bits.ITT9
#define PB_ITT1_ITT10       (_PB_ITT1).Bits.ITT10
#define PB_ITT1_ITT11       (_PB_ITT1).Bits.ITT11
#define PB_ITT1_ITT12       (_PB_ITT1).Bits.ITT12
#define PB_ITT1_ITT13       (_PB_ITT1).Bits.ITT13
#define PB_ITT1_ITT14       (_PB_ITT1).Bits.ITT14
#define PB_ITT1_ITT15       (_PB_ITT1).Bits.ITT15

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ITTC0      :1;
        unsigned ITTC1      :1;
        unsigned ITTC2      :1;
        unsigned ITTC3      :1;
        unsigned ITTC4      :1;
        unsigned ITTC5      :1;
        unsigned ITTC6      :1;
        unsigned ITTC7      :1;
        unsigned ITTC8      :1;
        unsigned ITTC9      :1;
        unsigned ITTC10     :1;
        unsigned ITTC11     :1;
        unsigned ITTC12     :1;
        unsigned ITTC13     :1;
        unsigned ITTC14     :1;
        unsigned ITTC15     :1;
        unsigned            :16;
    } Bits;
} PB_ITTC_STR;
#define _PB_ITTC            *(PB_ITTC_STR *)(GPIOB_BASE + 0x34)
#define PB_ITTC             (_PB_ITTC).Word
#define PB_ITTC_ITTC0       (_PB_ITTC).Bits.ITTC0
#define PB_ITTC_ITTC1       (_PB_ITTC).Bits.ITTC1
#define PB_ITTC_ITTC2       (_PB_ITTC).Bits.ITTC2
#define PB_ITTC_ITTC3       (_PB_ITTC).Bits.ITTC3
#define PB_ITTC_ITTC4       (_PB_ITTC).Bits.ITTC4
#define PB_ITTC_ITTC5       (_PB_ITTC).Bits.ITTC5
#define PB_ITTC_ITTC6       (_PB_ITTC).Bits.ITTC6
#define PB_ITTC_ITTC7       (_PB_ITTC).Bits.ITTC7
#define PB_ITTC_ITTC8       (_PB_ITTC).Bits.ITTC8
#define PB_ITTC_ITTC9       (_PB_ITTC).Bits.ITTC9
#define PB_ITTC_ITTC10      (_PB_ITTC).Bits.ITTC10
#define PB_ITTC_ITTC11      (_PB_ITTC).Bits.ITTC11
#define PB_ITTC_ITTC12      (_PB_ITTC).Bits.ITTC12
#define PB_ITTC_ITTC13      (_PB_ITTC).Bits.ITTC13
#define PB_ITTC_ITTC14      (_PB_ITTC).Bits.ITTC14
#define PB_ITTC_ITTC15      (_PB_ITTC).Bits.ITTC15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ITS0 :1;
        unsigned ITS1 :1;
        unsigned ITS2 :1;
        unsigned ITS3 :1;
        unsigned ITS4 :1;
        unsigned ITS5 :1;
        unsigned ITS6 :1;
        unsigned ITS7 :1;
        unsigned ITS8 :1;
        unsigned ITS9 :1;
        unsigned ITS10 :1;
        unsigned ITS11 :1;
        unsigned ITS12 :1;
        unsigned ITS13 :1;
        unsigned ITS14 :1;
        unsigned ITS15 :1;
        unsigned       :16;
    } Bits;
} PB_ITSR_STR;
#define _PB_ITSR        *(PB_ITSR_STR*)(GPIOB_BASE + 0x38)
#define PB_ITSR         (_PB_ITSR).Word
#define PB_ITSR_ITS0    (_PB_ITSR).Bits.ITS0
#define PB_ITSR_ITS1    (_PB_ITSR).Bits.ITS1
#define PB_ITSR_ITS2    (_PB_ITSR).Bits.ITS2
#define PB_ITSR_ITS3    (_PB_ITSR).Bits.ITS3
#define PB_ITSR_ITS4    (_PB_ITSR).Bits.ITS4
#define PB_ITSR_ITS5    (_PB_ITSR).Bits.ITS5
#define PB_ITSR_ITS6    (_PB_ITSR).Bits.ITS6
#define PB_ITSR_ITS7    (_PB_ITSR).Bits.ITS7
#define PB_ITSR_ITS8    (_PB_ITSR).Bits.ITS8
#define PB_ITSR_ITS9    (_PB_ITSR).Bits.ITS9
#define PB_ITSR_ITS10   (_PB_ITSR).Bits.ITS10
#define PB_ITSR_ITS11   (_PB_ITSR).Bits.ITS11
#define PB_ITSR_ITS12   (_PB_ITSR).Bits.ITS12
#define PB_ITSR_ITS13   (_PB_ITSR).Bits.ITS13
#define PB_ITSR_ITS14   (_PB_ITSR).Bits.ITS14
#define PB_ITSR_ITS15   (_PB_ITSR).Bits.ITS15

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned AF0    :3;
        unsigned        :1;
        unsigned AF1    :3;
        unsigned        :1;
        unsigned AF2    :3;
        unsigned        :1;
        unsigned AF3    :3;
        unsigned        :1;
        unsigned AF4    :3;
        unsigned        :1;
        unsigned AF5    :3;
        unsigned        :1;
        unsigned AF6    :3;
        unsigned        :1;
        unsigned AF7     :3;
        unsigned         :1;
    } Bits;
} PB_AF0_STR;
#define _PB_AF0     *(PB_AF0_STR*)(GPIOB_BASE+0x3C)
#define PB_AF0      (_PB_AF0).Word
#define PB_AF0_AF0  (_PB_AF0).Bits.AF0
#define PB_AF0_AF1  (_PB_AF0).Bits.AF1
#define PB_AF0_AF2  (_PB_AF0).Bits.AF2
#define PB_AF0_AF3  (_PB_AF0).Bits.AF3
#define PB_AF0_AF4  (_PB_AF0).Bits.AF4
#define PB_AF0_AF5  (_PB_AF0).Bits.AF5
#define PB_AF0_AF6  (_PB_AF0).Bits.AF6
#define PB_AF0_AF7  (_PB_AF0).Bits.AF7


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned AF8    :3;
        unsigned        :1;
        unsigned AF9    :3;
        unsigned        :1;
        unsigned AF10   :3;
        unsigned        :1;
        unsigned AF11   :3;
        unsigned        :1;
        unsigned AF12   :3;
        unsigned        :1;
        unsigned AF13   :3;
        unsigned        :1;
        unsigned AF14   :3;
        unsigned        :1;
        unsigned AF15   :3;
        unsigned        :1;
    } Bits;
} PB_AF1_STR;
#define _PB_AF1         *(PB_AF1_STR *)(GPIOB_BASE + 0x40)
#define PB_AF1          (_PB_AF1).Word
#define PB_AF1_AF8      (_PB_AF1).Bits.AF8
#define PB_AF1_AF9      (_PB_AF1).Bits.AF9
#define PB_AF1_AF10     (_PB_AF1).Bits.AF10
#define PB_AF1_AF11     (_PB_AF1).Bits.AF11
#define PB_AF1_AF12     (_PB_AF1).Bits.AF12
#define PB_AF1_AF13     (_PB_AF1).Bits.AF13
#define PB_AF1_AF14     (_PB_AF1).Bits.AF14
#define PB_AF1_AF15     (_PB_AF1).Bits.AF15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned AFCC0 :1;
        unsigned AFCC1 :1;
        unsigned AFC2 :1;
        unsigned AFC3 :1;
        unsigned AFC4 :1;
        unsigned AFC5 :1;
        unsigned AFC6 :1;
        unsigned AFC7 :1;
        unsigned AFC8 :1;
        unsigned AFC9 :1;
        unsigned AFC10 :1;
        unsigned AFC11 :1;
        unsigned AFC12 :1;
        unsigned AFC13 :1;
        unsigned AFC14 :1;
        unsigned AFC15 :1;
        unsigned       :16;
    } Bits;
} PB_AFC_STR;
#define _PB_AFC         (*PB_AFC_STR*)(GPIOB_BASE + 0x4C)
#define PB_AFC          (_PB_AFC).Word
#define PB_AFC_AFC0     (_PB_AFC).Bits.AFC0
#define PB_AFC_AFC1     (_PB_AFC).Bits.AFC1
#define PB_AFC_AFC2     (_PB_AFC).Bits.AFC2
#define PB_AFC_AFC3     (_PB_AFC).Bits.AFC3
#define PB_AFC_AFC4     (_PB_AFC).Bits.AFC4
#define PB_AFC_AFC5     (_PB_AFC).Bits.AFC5
#define PB_AFC_AFC6     (_PB_AFC).Bits.AFC6
#define PB_AFC_AFC7     (_PB_AFC).Bits.AFC7
#define PB_AFC_AFC8     (_PB_AFC).Bits.AFC8
#define PB_AFC_AFC9     (_PB_AFC).Bits.AFC9
#define PB_AFC_AFC10    (_PB_AFC).Bits.AFC10
#define PB_AFC_AFC11    (_PB_AFC).Bits.AFC11
#define PB_AFC_AFC12    (_PB_AFC).Bits.AFC12
#define PB_AFC_AFC13    (_PB_AFC).Bits.AFC13
#define PB_AFC_AFC14    (_PB_AFC).Bits.AFC14
#define PB_AFC_AFC15    (_PB_AFC).Bits.AFC15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ANA0 :1;
        unsigned ANA1 :1;
        unsigned ANA2 :1;
        unsigned ANA3 :1;
        unsigned ANA4 :1;
        unsigned ANA5 :1;
        unsigned ANA6 :1;
        unsigned ANA7 :1;
        unsigned ANA8 :1;
        unsigned ANA9 :1;
        unsigned ANA10 :1;
        unsigned ANA11 :1;
        unsigned ANA12 :1;
        unsigned ANA13 :1;
        unsigned ANA14 :1;
        unsigned ANA15 :1;
        unsigned       :16;
    } Bits;
} PB_ANA_STR;
#define _PB_ANA         (*(PB_ANA_STR *)(GPIOB_BASE + 0x50)
#define PB_ANA          (_PB_ANA).Word
#define PB_ANA_ANA0     (_PB_ANA).Bits.ANA0
#define PB_ANA_ANA1     (_PB_ANA).Bits.ANA1
#define PB_ANA_ANA2     (_PB_ANA).Bits.ANA2
#define PB_ANA_ANA3     (_PB_ANA).Bits.ANA3
#define PB_ANA_ANA4     (_PB_ANA).Bits.ANA4
#define PB_ANA_ANA5     (_PB_ANA).Bits.ANA5
#define PB_ANA_ANA6     (_PB_ANA).Bits.ANA6
#define PB_ANA_ANA7     (_PB_ANA).Bits.ANA7
#define PB_ANA_ANA8     (_PB_ANA).Bits.ANA8
#define PB_ANA_ANA9     (_PB_ANA).Bits.ANA9
#define PB_ANA_ANA10    (_PB_ANA).Bits.ANA10
#define PB_ANA_ANA11    (_PB_ANA).Bits.ANA11
#define PB_ANA_ANA12    (_PB_ANA).Bits.ANA12
#define PB_ANA_ANA13    (_PB_ANA).Bits.ANA13
#define PB_ANA_ANA14    (_PB_ANA).Bits.ANA14
#define PB_ANA_ANA15    (_PB_ANA).Bits.ANA15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ANAC0 :1;
        unsigned ANAC1 :1;
        unsigned ANAC2 :1;
        unsigned ANAC3 :1;
        unsigned ANAC4 :1;
        unsigned ANAC5 :1;
        unsigned ANAC6 :1;
        unsigned ANAC7 :1;
        unsigned ANAC8 :1;
        unsigned ANAC9 :1;
        unsigned ANAC10 :1;
        unsigned ANAC11 :1;
        unsigned ANAC12 :1;
        unsigned ANAC13 :1;
        unsigned ANAC14 :1;
        unsigned ANAC15 :1;
        unsigned        :16;
    } Bits;
} PB_ANAC_STR;
#define _PB_ANAC            *(PB_ANAC_STR*)(GPIOB_BASE + 0x54)
#define PB_ANAC             (_PB_ANAC).Word
#define PB_ANAC_ANAC0       (_PB_ANAC).Bits.ANAC0
#define PB_ANAC_ANAC1       (_PB_ANAC).Bits.ANAC1
#define PB_ANAC_ANAC2       (_PB_ANAC).Bits.ANAC2
#define PB_ANAC_ANAC3       (_PB_ANAC).Bits.ANAC3
#define PB_ANAC_ANAC4       (_PB_ANAC).Bits.ANAC4
#define PB_ANAC_ANAC5       (_PB_ANAC).Bits.ANAC5
#define PB_ANAC_ANAC6       (_PB_ANAC).Bits.ANAC6
#define PB_ANAC_ANAC7       (_PB_ANAC).Bits.ANAC7
#define PB_ANAC_ANAC8       (_PB_ANAC).Bits.ANAC8
#define PB_ANAC_ANAC9       (_PB_ANAC).Bits.ANAC9
#define PB_ANAC_ANAC10      (_PB_ANAC).Bits.ANAC10
#define PB_ANAC_ANAC11      (_PB_ANAC).Bits.ANAC11
#define PB_ANAC_ANAC12      (_PB_ANAC).Bits.ANAC12
#define PB_ANAC_ANAC13      (_PB_ANAC).Bits.ANAC13
#define PB_ANAC_ANAC14      (_PB_ANAC).Bits.ANAC14
#define PB_ANAC_ANAC15      (_PB_ANAC).Bits.ANAC15


typedef volatile union{
    volatile unsigned int Word;
    struct{
        unsigned PU0 : 1;
        unsigned PU1 : 1;
        unsigned PU2 : 1;
        unsigned PU3 : 1;
        unsigned PU4 : 1;
        unsigned PU5 : 1;
        unsigned PU6 : 1;
        unsigned PU7 : 1;
        unsigned PU8 : 1;
        unsigned PU9 : 1;
        unsigned PU10 : 1;
        unsigned PU11 : 1;
        unsigned PU12 : 1;
        unsigned PU13 : 1;
        unsigned PU14 : 1;
        unsigned PU15 : 1;
        unsigned      : 16;
    } Bits;
} PB_PU_STR;
#define _PB_PU      *(PB_PU_STR *)(GPIOB_BASE + 0x58)
#define PB_PU       (_PB_PU).Word
#define PB_PU_PU0   (_PB_PU).Bits.PU0
#define PB_PU_PU1   (_PB_PU).Bits.PU1
#define PB_PU_PU2   (_PB_PU).Bits.PU2
#define PB_PU_PU3   (_PB_PU).Bits.PU3
#define PB_PU_PU4   (_PB_PU).Bits.PU4
#define PB_PU_PU5   (_PB_PU).Bits.PU5
#define PB_PU_PU6   (_PB_PU).Bits.PU6
#define PB_PU_PU7   (_PB_PU).Bits.PU7
#define PB_PU_PU8   (_PB_PU).Bits.PU8
#define PB_PU_PU9   (_PB_PU).Bits.PU9
#define PB_PU_PU10  (_PB_PU).Bits.PU10
#define PB_PU_PU11  (_PB_PU).Bits.PU11
#define PB_PU_PU12  (_PB_PU).Bits.PU12
#define PB_PU_PU13  (_PB_PU).Bits.PU13
#define PB_PU_PU14  (_PB_PU).Bits.PU14
#define PB_PU_PU15  (_PB_PU).Bits.PU15


typedef volatile union{
    volatile unsigned int Word;
    struct{
        unsigned SPU0 : 1;
        unsigned SPU1 : 1;
        unsigned SPU2 : 1;
        unsigned SPU3 : 1;
        unsigned SPU4 : 1;
        unsigned SPU5 : 1;
        unsigned SPU6 : 1;
        unsigned SPU7 : 1;
        unsigned SPU8 : 1;
        unsigned SPU9 : 1;
        unsigned SPU10 : 1;
        unsigned SPU11 : 1;
        unsigned SPU12 : 1;
        unsigned SPU13 : 1;
        unsigned SPU14 : 1;
        unsigned SPU15 : 1;
        unsigned       : 16;
    } Bits;
} PB_SPU_STR;
#define _PB_SPU         (*(PB_SPU_STR *)(GPIOB_BASE + 0x5C)
#define PB_SPU          (_PB_SPU).Word
#define PB_SPU_SPU0     (_PB_SPU).Bits.SPU0
#define PB_SPU_SPU1     (_PB_SPU).Bits.SPU1
#define PB_SPU_SPU2     (_PB_SPU).Bits.SPU2
#define PB_SPU_SPU3     (_PB_SPU).Bits.SPU3
#define PB_SPU_SPU4     (_PB_SPU).Bits.SPU4
#define PB_SPU_SPU5     (_PB_SPU).Bits.SPU5
#define PB_SPU_SPU6     (_PB_SPU).Bits.SPU6
#define PB_SPU_SPU7     (_PB_SPU).Bits.SPU7
#define PB_SPU_SPU8     (_PB_SPU).Bits.SPU8
#define PB_SPU_SPU9     (_PB_SPU).Bits.SPU9
#define PB_SPU_SPU10    (_PB_SPU).Bits.SPU10
#define PB_SPU_SPU11    (_PB_SPU).Bits.SPU11
#define PB_SPU_SPU12    (_PB_SPU).Bits.SPU12
#define PB_SPU_SPU13    (_PB_SPU).Bits.SPU13
#define PB_SPU_SPU14    (_PB_SPU).Bits.SPU14
#define PB_SPU_SPU15    (_PB_SPU).Bits.SPU15


typedef volatile union{
    volatile unsigned int Word;
    struct{
        unsigned PUC0 : 1;
        unsigned PUC1 : 1;
        unsigned PUC2 : 1;
        unsigned PUC3 : 1;
        unsigned PUC4 : 1;
        unsigned PUC5 : 1;
        unsigned PUC6 : 1;
        unsigned PUC7 : 1;
        unsigned PUC8 : 1;
        unsigned PUC9 : 1;
        unsigned PUC10 : 1;
        unsigned PUC11 : 1;
        unsigned PUC12 : 1;
        unsigned PUC13 : 1;
        unsigned PUC14 : 1;
        unsigned PUC15 : 1;
        unsigned       : 16;
    } Bits;
} PB_PUC_STR;
#define _PB_PUC         (*(PB_PUC_STR *)(GPIOB_BASE + 0x60))
#define PB_PUC          (_PB_PUC).Word
#define PB_PUC_PUC0     (_PB_PUC).Bits.PUC0
#define PB_PUC_PUC1     (_PB_PUC).Bits.PUC1
#define PB_PUC_PUC2     (_PB_PUC).Bits.PUC2
#define PB_PUC_PUC3     (_PB_PUC).Bits.PUC3
#define PB_PUC_PUC4     (_PB_PUC).Bits.PUC4
#define PB_PUC_PUC5     (_PB_PUC).Bits.PUC5
#define PB_PUC_PUC6     (_PB_PUC).Bits.PUC6
#define PB_PUC_PUC7     (_PB_PUC).Bits.PUC7
#define PB_PUC_PUC8     (_PB_PUC).Bits.PUC8
#define PB_PUC_PUC9     (_PB_PUC).Bits.PUC9
#define PB_PUC_PUC10    (_PB_PUC).Bits.PUC10
#define PB_PUC_PUC11    (_PB_PUC).Bits.PUC11
#define PB_PUC_PUC12    (_PB_PUC).Bits.PUC12
#define PB_PUC_PUC13    (_PB_PUC).Bits.PUC13
#define PB_PUC_PUC14    (_PB_PUC).Bits.PUC14
#define PB_PUC_PUC15    (_PB_PUC).Bits.PUC15


typedef volatile union{
    volatile unsigned int Word;
    struct{
        unsigned PD0 : 1;
        unsigned PD1 : 1;
        unsigned PD2 : 1;
        unsigned PD3 : 1;
        unsigned PD4 : 1;
        unsigned PD5 : 1;
        unsigned PD6 : 1;
        unsigned PD7 : 1;
        unsigned PD8 : 1;
        unsigned PD9 : 1;
        unsigned PD10 : 1;
        unsigned PD11 : 1;
        unsigned PD12 : 1;
        unsigned PD13 : 1;
        unsigned PD14 : 1;
        unsigned PD15 : 1;
        unsigned      : 16;
    } Bits;
} PB_PD_STR;
#define _PB_PD      *(PB_PD_STR *)(GPIOB_BASE + 0x64)
#define PB_PD       (_PB_PD).Word
#define PB_PD_PD0   (_PB_PD).Bits.PD0
#define PB_PD_PD1   (_PB_PD).Bits.PD1
#define PB_PD_PD2   (_PB_PD).Bits.PD2
#define PB_PD_PD3   (_PB_PD).Bits.PD3
#define PB_PD_PD4   (_PB_PD).Bits.PD4
#define PB_PD_PD5   (_PB_PD).Bits.PD5
#define PB_PD_PD6   (_PB_PD).Bits.PD6
#define PB_PD_PD7   (_PB_PD).Bits.PD7
#define PB_PD_PD8   (_PB_PD).Bits.PD8
#define PB_PD_PD9   (_PB_PD).Bits.PD9
#define PB_PD_PD10  (_PB_PD).Bits.PD10
#define PB_PD_PD11  (_PB_PD).Bits.PD11
#define PB_PD_PD12  (_PB_PD).Bits.PD12
#define PB_PD_PD13  (_PB_PD).Bits.PD13
#define PB_PD_PD14  (_PB_PD).Bits.PD14
#define PB_PD_PD15  (_PB_PD).Bits.PD15


typedef volatile union{
    volatile unsigned int Word;
    struct{
        unsigned PDC0 : 1;
        unsigned PDC1 : 1;
        unsigned PDC2 : 1;
        unsigned PDC3 : 1;
        unsigned PDC4 : 1;
        unsigned PDC5 : 1;
        unsigned PDC6 : 1;
        unsigned PDC7 : 1;
        unsigned PDC8 : 1;
        unsigned PDC9 : 1;
        unsigned PDC10 : 1;
        unsigned PDC11 : 1;
        unsigned PDC12 : 1;
        unsigned PDC13 : 1;
        unsigned PDC14 : 1;
        unsigned PDC15 : 1;
        unsigned       : 16;
    } Bits;
} PB_PDC_STR;
#define _PB_PDC              (*(PB_PDC_STR*)(GPIOB_BASE + 0x68)
#define PB_PDC               (_PB_PDC).Word
#define PB_PDC_PDC0          (_PB_PDC).Bits.PDC0
#define PB_PDC_PDC1          (_PB_PDC).Bits.PDC1
#define PB_PDC_PDC2          (_PB_PDC).Bits.PDC2
#define PB_PDC_PDC3          (_PB_PDC).Bits.PDC3
#define PB_PDC_PDC4          (_PB_PDC).Bits.PDC4
#define PB_PDC_PDC5          (_PB_PDC).Bits.PDC5
#define PB_PDC_PDC6          (_PB_PDC).Bits.PDC6
#define PB_PDC_PDC7          (_PB_PDC).Bits.PDC7
#define PB_PDC_PDC8          (_PB_PDC).Bits.PDC8
#define PB_PDC_PDC9          (_PB_PDC).Bits.PDC9
#define PB_PDC_PDC10         (_PB_PDC).Bits.PDC10
#define PB_PDC_PDC11         (_PB_PDC).Bits.PDC11
#define PB_PDC_PDC12          (_PB_PDC).Bits.PDC12
#define PB_PDC_PDC13          (_PB_PDC).Bits.PDC13
#define PB_PDC_PDC14          (_PB_PDC).Bits.PDC14
#define PB_PDC_PDC15          (_PB_PDC).Bits.PDC15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DRVS0     :3;
        unsigned           :1;
        unsigned DRVS1     :3;
        unsigned           :1;
        unsigned DRVS2     :3;
        unsigned           :1;
        unsigned DRVS3     :3;
        unsigned           :1;
        unsigned DRVS4     :3;
        unsigned           :1;
        unsigned DRVS5     :3;
        unsigned           :1;
        unsigned DRVS6     :3;
        unsigned           :1;
        unsigned DRVS7     :3;
        unsigned           :1;
    } Bits;
} PB_DRVS0_STR;
#define _PB_DRVS0          (*(PB_DRVS0_STR *)(GPIOB_BASE + 0x6C)
#define PB_DRVS0           (_PB_DRVS0).Word
#define PB_DRVS0_DRVS0     (_PB_DRVS0).Bits.DRVS0
#define PB_DRVS0_DRVS1     (_PB_DRVS0).Bits.DRVS1
#define PB_DRVS0_DRVS2     (_PB_DRVS0).Bits.DRVS2
#define PB_DRVS0_DRVS3     (_PB_DRVS0).Bits.DRVS3
#define PB_DRVS0_DRVS4     (_PB_DRVS0).Bits.DRVS4
#define PB_DRVS0_DRVS5     (_PB_DRVS0).Bits.DRVS5
#define PB_DRVS0_DRVS6     (_PB_DRVS0).Bits.DRVS6
#define PB_DRVS0_DRVS7     (_PB_DRVS0).Bits.DRVS7


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DRVS8 :3;
        unsigned        :1;
        unsigned DRVS9 :3;
        unsigned        :1;
        unsigned DRVS10 :3;
        unsigned        :1;
        unsigned DRVS11 :3;
        unsigned        :1;
        unsigned DRVS12 :3;
        unsigned        :1;
        unsigned DRVS13 :3;
        unsigned        :1;
        unsigned DRVS14 :3;
        unsigned        :1;
        unsigned DRVS15 :3;
        unsigned        :1;
    } Bits;
} PB_DRVS1_STR;
#define _PB_DRVS1           *(PB_DRVS1_STR*)(GPIOB_BASE + 0x70)
#define PB_DRVS1            (_PB_DRVS1).Word
#define PB_DRVS1_DRVS8      (_PB_DRVS1).Bits.DRVS8
#define PB_DRVS1_DRVS9      (_PB_DRVS1).Bits.DRVS9
#define PB_DRVS1_DRVS10     (_PB_DRVS1).Bits.DRVS10
#define PB_DRVS1_DRVS11     (_PB_DRVS1).Bits.DRVS11
#define PB_DRVS1_DRVS12     (_PB_DRVS1).Bits.DRVS12
#define PB_DRVS1_DRVS13     (_PB_DRVS1).Bits.DRVS13
#define PB_DRVS1_DRVS14     (_PB_DRVS1).Bits.DRVS14
#define PB_DRVS1_DRVS15     (_PB_DRVS1).Bits.DRVS15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DRVC0 :1;
        unsigned DRVC1 :1;
        unsigned DRVC2 :1;
        unsigned DRVC3 :1;
        unsigned DRVC4 :1;
        unsigned DRVC5 :1;
        unsigned DRVC6 :1;
        unsigned DRVC7 :1;
        unsigned DRVC8 :1;
        unsigned DRVC9 :1;
        unsigned DRVC10 :1;
        unsigned DRVC11 :1;
        unsigned DRVC12 :1;
        unsigned DRVC13 :1;
        unsigned DRVC14 :1;
        unsigned DRVC15 :1;
        unsigned        :16;
    } Bits;
} PB_DRVC_STR;
#define _PB_DRVC        (*(PB_DRVC_STR *)(GPIOB_BASE + 0x7C)
#define PB_DRVC         (_PB_DRVC).Word
#define PB_DRVC_DRVC0   (_PB_DRVC).Bits.DRVC0
#define PB_DRVC_DRVC1   (_PB_DRVC).Bits.DRVC1
#define PB_DRVC_DRVC2   (_PB_DRVC).Bits.DRVC2
#define PB_DRVC_DRVC3   (_PB_DRVC).Bits.DRVC3
#define PB_DRVC_DRVC4   (_PB_DRVC).Bits.DRVC4
#define PB_DRVC_DRVC5   (_PB_DRVC).Bits.DRVC5
#define PB_DRVC_DRVC6   (_PB_DRVC).Bits.DRVC6
#define PB_DRVC_DRVC7   (_PB_DRVC).Bits.DRVC7
#define PB_DRVC_DRVC8   (_PB_DRVC).Bits.DRVC8
#define PB_DRVC_DRVC9   (_PB_DRVC).Bits.DRVC9
#define PB_DRVC_DRVC10  (_PB_DRVC).Bits.DRVC10
#define PB_DRVC_DRVC11  (_PB_DRVC).Bits.DRVC11
#define PB_DRVC_DRVC12  (_PB_DRVC).Bits.DRVC12
#define PB_DRVC_DRVC13  (_PB_DRVC).Bits.DRVC13
#define PB_DRVC_DRVC14  (_PB_DRVC).Bits.DRVC14
#define PB_DRVC_DRVC15  (_PB_DRVC).Bits.DRVC15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned SL0 :1;
        unsigned SL1 :1;
        unsigned SL2 :1;
        unsigned SL3 :1;
        unsigned SL4 :1;
        unsigned SL5 :1;
        unsigned SL6 :1;
        unsigned SL7 :1;
        unsigned SL8 :1;
        unsigned SL9 :1;
        unsigned SL10 :1;
        unsigned SL11 :1;
        unsigned SL12 :1;
        unsigned SL13 :1;
        unsigned SL14 :1;
        unsigned SL15 :1;
        unsigned      :16;
    } Bits;
} PB_SLS_STR;
#define _PB_SLS         *(_PB_SLS_STR*)(GPIOB_BASE+0x80)
#define PB_SLS          (_PB_SLS).Word
#define PB_SLS_SL0      (_PB_SLS).Bits.SL0
#define PB_SLS_SL1      (_PB_SLS).Bits.SL1
#define PB_SLS_SL2      (_PB_SLS).Bits.SL2
#define PB_SLS_SL3      (_PB_SLS).Bits.SL3
#define PB_SLS_SL4      (_PB_SLS).Bits.SL4
#define PB_SLS_SL5      (_PB_SLS).Bits.SL5
#define PB_SLS_SL6      (_PB_SLS).Bits.SL6
#define PB_SLS_SL7      (_PB_SLS).Bits.SL7
#define PB_SLS_SL8      (_PB_SLS).Bits.SL8
#define PB_SLS_SL9      (_PB_SLS).Bits.SL9
#define PB_SLS_SL10     (_PB_SLS).Bits.SL10
#define PB_SLS_SL11     (_PB_SLS).Bits.SL11
#define PB_SLS_SL12     (_PB_SLS).Bits.SL12
#define PB_SLS_SL13     (_PB_SLS).Bits.SL13
#define PB_SLS_SL14     (_PB_SLS).Bits.SL14
#define PB_SLS_SL15     (_PB_SLS).Bits.SL15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned SLC0 :1;
        unsigned SLC1 :1;
        unsigned SLC2 :1;
        unsigned SLC3 :1;
        unsigned SLC4 :1;
        unsigned SLC5 :1;
        unsigned SLC6 :1;
        unsigned SLC7 :1;
        unsigned SLC8 :1;
        unsigned SLC9 :1;
        unsigned SLC10 :1;
        unsigned SLC11 :1;
        unsigned SLC12 :1;
        unsigned SLC13 :1;
        unsigned SLC14 :1;
        unsigned SLC15 :1;
        unsigned       :16;
    } Bits;
} PB_SLC_STR;
#define _PB_SLC         *( PB_SLC_STR *)(GPIOB_BASE + 0x84)
#define PB_SLC          (_PB_SLC).Word
#define PB_SLC_SLC0     (_PB_SLC).Bits.SLC0
#define PB_SLC_SLC1     (_PB_SLC).Bits.SLC1
#define PB_SLC_SLC2     (_PB_SLC).Bits.SLC2
#define PB_SLC_SLC3     (_PB_SLC).Bits.SLC3
#define PB_SLC_SLC4     (_PB_SLC).Bits.SLC4
#define PB_SLC_SLC5     (_PB_SLC).Bits.SLC5
#define PB_SLC_SLC6     (_PB_SLC).Bits.SLC6
#define PB_SLC_SLC7     (_PB_SLC).Bits.SLC7
#define PB_SLC_SLC8     (_PB_SLC).Bits.SLC8
#define PB_SLC_SLC9     (_PB_SLC).Bits.SLC9
#define PB_SLC_SLC10    (_PB_SLC).Bits.SLC10
#define PB_SLC_SLC11    (_PB_SLC).Bits.SLC11
#define PB_SLC_SLC12    (_PB_SLC).Bits.SLC12
#define PB_SLC_SLC13    (_PB_SLC).Bits.SLC13
#define PB_SLC_SLC14    (_PB_SLC).Bits.SLC14
#define PB_SLC_SLC15    (_PB_SLC).Bits.SLC15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ST0 :1;
        unsigned ST1 :1;
        unsigned ST2 :1;
        unsigned ST3 :1;
        unsigned ST4 :1;
        unsigned ST5 :1;
        unsigned ST6 :1;
        unsigned ST7 :1;
        unsigned ST8 :1;
        unsigned ST9 :1;
        unsigned ST10 :1;
        unsigned ST11 :1;
        unsigned ST12 :1;
        unsigned ST13 :1;
        unsigned ST14 :1;
        unsigned ST15 :1;
        unsigned :16;
    } Bits;
} PB_STS_STR;
#define _PB_STS         (*PB_STS_STR *)(GPIOB_BASE + 0x88)
#define PB_STS          (_PB_STS).Word
#define PB_STS_ST0      (_PB_STS).Bits.ST0
#define PB_STS_ST1      (_PB_STS).Bits.ST1
#define PB_STS_ST2      (_PB_STS).Bits.ST2
#define PB_STS_ST3      (_PB_STS).Bits.ST3
#define PB_STS_ST4      (_PB_STS).Bits.ST4
#define PB_STS_ST5      (_PB_STS).Bits.ST5
#define PB_STS_ST6      (_PB_STS).Bits.ST6
#define PB_STS_ST7      (_PB_STS).Bits.ST7
#define PB_STS_ST8      (_PB_STS).Bits.ST8
#define PB_STS_ST9      (_PB_STS).Bits.ST9
#define PB_STS_ST10     (_PB_STS).Bits.ST10
#define PB_STS_ST11     (_PB_STS).Bits.ST11
#define PB_STS_ST12     (_PB_STS).Bits.ST12
#define PB_STS_ST13     (_PB_STS).Bits.ST13
#define PB_STS_ST14     (_PB_STS).Bits.ST14
#define PB_STS_ST15     (_PB_STS).Bits.ST15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned STC0 :1;
        unsigned STC1 :1;
        unsigned STC2 :1;
        unsigned STC3 :1;
        unsigned STC4 :1;
        unsigned STC5 :1;
        unsigned STC6 :1;
        unsigned STC7 :1;
        unsigned STC8 :1;
        unsigned STC9 :1;
        unsigned STC10 :1;
        unsigned STC11 :1;
        unsigned STC12 :1;
        unsigned STC13 :1;
        unsigned STC14 :1;
        unsigned STC15 :1;
        unsigned       :16;
    } Bits;
} PB_STC_STR;
#define _PB_STC         *( PB_STC_STR *)(GPIOB_Base + 0x8C)
#define PB_STC          (_PB_STC).Word
#define PB_STC_STC0     (_PB_STC).Bits.STC0
#define PB_STC_STC1     (_PB_STC).Bits.STC1
#define PB_STC_STC2     (_PB_STC).Bits.STC2
#define PB_STC_STC3     (_PB_STC).Bits.STC3
#define PB_STC_STC4     (_PB_STC).Bits.STC4
#define PB_STC_STC5     (_PB_STC).Bits.STC5
#define PB_STC_STC6     (_PB_STC).Bits.STC6
#define PB_STC_STC7     (_PB_STC).Bits.STC7
#define PB_STC_STC8     (_PB_STC).Bits.STC8
#define PB_STC_STC9     (_PB_STC).Bits.STC9
#define PB_STC_STC10    (_PB_STC).Bits.STC10
#define PB_STC_STC11    (_PB_STC).Bits.STC11
#define PB_STC_STC12    (_PB_STC).Bits.STC12
#define PB_STC_STC13    (_PB_STC).Bits.STC13
#define PB_STC_STC14    (_PB_STC).Bits.STC14
#define PB_STC_STC15    (_PB_STC).Bits.STC15
//PB_------------End----------------


//PC_------------Start----------------
typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned MINVN      :8;
        unsigned MAJVN      :8;
        unsigned FEATURE    :16;
    } Bits;
} PC_VERID_STR;
#define _PC_VERID           (*(PC_VERID_STR*)(GPIOC_BASE + 0x00)
#define PC_VERID            (_PC_VERID).Word
#define PC_VERID_MINVN      (_PC_VERID).Bits.MINVN
#define PC_VERID_MAJVN      (_PC_VERID).Bits.MAJVN
#define PC_VERID_FEATURE    (_PC_VERID).Bits.FEATURE


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DR0        :1;
        unsigned DR1         :1;
        unsigned DR2         :1;
        unsigned DR3         :1;
        unsigned DR4         :1;
        unsigned DR5         :1;
        unsigned DR6         :1;
        unsigned DR7         :1;
        unsigned DR8         :1;
        unsigned DR9         :1;
        unsigned DR10         :1;
        unsigned DR11         :1;
        unsigned DR12         :1;
        unsigned DR13         :1;
        unsigned DR14         :1;
        unsigned DR15         :1;
        unsigned              :16;
    } Bits;
} PC_DR_STR;
#define _PC_DR          (*(PC_DR_STR*)(GPIOC_BASE + 0x04)
#define PC_DR           (_PC_DR).Word
#define PC_DR0          (_PC_DR).Bits.DR0
#define PC_DR1          (_PC_DR).Bits.DR1
#define PC_DR2          (_PC_DR).Bits.DR2
#define PC_DR3          (_PC_DR).Bits.DR3
#define PC_DR_DR4       (_PC_DR).Bits.DR4
#define PC_DR_DR5       (_PC_DR).Bits.DR5
#define PC_DR_DR6       (_PC_DR).Bits.DR6
#define PC_DR_DR7       (_PC_DR).Bits.DR7
#define PC_DR_DR8       (_PC_DR).Bits.DR8
#define PC_DR_DR9       (_PC_DR).Bits.DR9
#define PC_DR_DR10      (_PC_DR).Bits.DR10
#define PC_DR_DR11      (_PC_DR).Bits.DR11
#define PC_DR_DR12      (_PC_DR).Bits.DR12
#define PC_DR_DR13      (_PC_DR).Bits.DR13
#define PC_DR_DR14      (_PC_DR).Bits.DR14
#define PC_DR_DR15      (_PC_DR).Bits.DR15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DRS0 : 1;
        unsigned DRS1 : 1;
        unsigned DRS2 : 1;
        unsigned DRS3 : 1;
        unsigned DRS4 : 1;
        unsigned DRS5 : 1;
        unsigned DRS6 : 1;
        unsigned DRS7 : 1;
        unsigned DRS8 : 1;
        unsigned DRS9 : 1;
        unsigned DRS10 : 1;
        unsigned DRS11 : 1;
        unsigned DRS12 : 1;
        unsigned DRS13 : 1;
        unsigned DRS14 : 1;
        unsigned DRS15 : 1;
        unsigned : 16;
    } Bits;
} PC_DRS_STR;
#define _PC_DRS      *(PC_DRS_STR*)(GPIOC_BASE + 0x08)
#define PC_DRS       (_PC_DRS).Word
#define PC_DRS_DRS0  (_PC_DRS).Bits.DRS0
#define PC_DRS_DRS1  (_PC_DRS).Bits.DRS1
#define PC_DRS_DRS2  (_PC_DRS).Bits.DRS2
#define PC_DRS_DRS3  (_PC_DRS).Bits.DRS3
#define PC_DRS_DRS4  (_PC_DRS).Bits.DRS4
#define PC_DRS_DRS5  (_PC_DRS).Bits.DRS5
#define PC_DRS_DRS6  (_PC_DRS).Bits.DRS6
#define PC_DRS_DRS7  (_PC_DRS).Bits.DRS7
#define PC_DRS_DRS8  (_PC_DRS).Bits.DRS8
#define PC_DRS_DRS9  (_PC_DRS).Bits.DRS9
#define PC_DRS_DRS10 (_PC_DRS).Bits.DRS10
#define PC_DRS_DRS11 (_PC_DRS).Bits.DRS11
#define PC_DRS_DRS12 (_PC_DRS).Bits.DRS12
#define PC_DRS_DRS13 (_PC_DRS).Bits.DRS13
#define PC_DRS_DRS14 (_PC_DRS).Bits.DRS14
#define PC_DRS_DRS15 (_PC_DRS).Bits.DRS15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DRC0 :1;
        unsigned DRC1 :1;
        unsigned DRC2 :1;
        unsigned DRC3 :1;
        unsigned DRC4 :1;
        unsigned DRC5 :1;
        unsigned DRC6 :1;
        unsigned DRC7 :1;
        unsigned DRC8 :1;
        unsigned DRC9 :1;
        unsigned DRC10 :1;
        unsigned DRC11 :1;
        unsigned DRC12 :1;
        unsigned DRC13 :1;
        unsigned DRC14 :1;
        unsigned DRC15 :1;
        unsigned :16;
    } Bits;
} PC_DRC_STR;
#define _PC_DRC      *( PC_DRC_STR *)(GPIOC_BASE + 0x0C)
#define PC_DRC       (_PC_DRC).Word
#define PC_DRC_DRC0  (_PC_DRC).Bits.DRC0
#define PC_DRC_DRC1  (_PC_DRC).Bits.DRC1
#define PC_DRC_DRC2  (_PC_DRC).Bits.DRC2
#define PC_DRC_DRC3  (_PC_DRC).Bits.DRC3
#define PC_DRC_DRC4  (_PC_DRC).Bits.DRC4
#define PC_DRC_DRC5  (_PC_DRC).Bits.DRC5
#define PC_DRC_DRC6  (_PC_DRC).Bits.DRC6
#define PC_DRC_DRC7  (_PC_DRC).Bits.DRC7
#define PC_DRC_DRC8  (_PC_DRC).Bits.DRC8
#define PC_DRC_DRC9  (_PC_DRC).Bits.DRC9
#define PC_DRC_DRC10 (_PC_DRC).Bits.DRC10
#define PC_DRC_DRC11 (_PC_DRC).Bits.DRC11
#define PC_DRC_DRC12 (_PC_DRC).Bits.DRC12
#define PC_DRC_DRC13 (_PC_DRC).Bits.DRC13
#define PC_DRC_DRC14 (_PC_DRC).Bits.DRC14
#define PC_DRC_DRC15 (_PC_DRC).Bits.DRC15

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DRT0 :1;
        unsigned DRT1 :1;
        unsigned DRT2 :1;
        unsigned DRT3 :1;
        unsigned DRT4 :1;
        unsigned DRT5 :1;
        unsigned DRT6 :1;
        unsigned DRT7 :1;
        unsigned DRT8 :1;
        unsigned DRT9 :1;
        unsigned DRT10 :1;
        unsigned DRT11 :1;
        unsigned DRT12 :1;
        unsigned DRT13 :1;
        unsigned DRT14 :1;
        unsigned DRT15 :1;
        unsigned :16;
    } Bits;
} PC_DRT_STR;
#define _PC_DRT         *(PC_DRT_STR *)(GPIOC_BASE + 0x10)
#define PC_DRT          (_PC_DRT).Word
#define PC_DRT_DRT0     (_PC_DRT).Bits.DRT0
#define PC_DRT_DRT1     (_PC_DRT).Bits.DRT1
#define PC_DRT_DRT2     (_PC_DRT).Bits.DRT2
#define PC_DRT_DRT3     (_PC_DRT).Bits.DRT3
#define PC_DRT_DRT4     (_PC_DRT).Bits.DRT4
#define PC_DRT_DRT5     (_PC_DRT).Bits.DRT5
#define PC_DRT_DRT6     (_PC_DRT).Bits.DRT6
#define PC_DRT_DRT7     (_PC_DRT).Bits.DRT7
#define PC_DRT_DRT8     (_PC_DRT).Bits.DRT8
#define PC_DRT_DRT9     (_PC_DRT).Bits.DRT9
#define PC_DRT_DRT10    (_PC_DRT).Bits.DRT10
#define PC_DRT_DRT11    ( _PC_DRT ).Bits.DRT11
#define PC_DRT_DRT12    ( _PC_DRT ).Bits.DRT12
#define PC_DRT_DRT13    ( _PC_DRT ).Bits.DRT13
#define PC_DRT_DRT14    ( _PC_DRT ).Bits.DRT14
#define PC_DRT_DRT15     ( _PC_DRT ).Bits.DRT15

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned OES0       :1;
        unsigned OES1       :1;
        unsigned OES2       :1;
        unsigned OES3       :1;
        unsigned OES4       :1; 
        unsigned OES5       :1;
        unsigned OES6       :1;
        unsigned OES7       :1;
        unsigned OES8       :1;
        unsigned OES9       :1;
        unsigned OES10       :1;
        unsigned OES11       :1;
        unsigned OES12       :1;
        unsigned OES13       :1;
        unsigned OES14       :1;
        unsigned OES15       :1;
        unsigned             :16;
    } Bits;
} PC_OES_STR;
#define _PC_OES             *( PC_OES_STR *)(GPIOC_BASE + 0x14)
#define PC_OES              ( _PC_OES).Word
#define PC_OES_OES0         ( _PC_OES).Bits.OES0
#define PC_OES_OES1         ( _PC_OES).Bits.OES1
#define PC_OES_OES2         ( _PC_OES).Bits.OES2
#define PC_OES_OES3         ( _PC_OES).Bits.OES3
#define PC_OES_OES4         ( _PC_OES).Bits.OES4
#define PC_OES_OES5         ( _PC_OES).Bits.OES5
#define PC_OES_OES6         ( _PC_OES).Bits.OES6
#define PC_OES_OES7         ( _PC_OES).Bits.OES7
#define PC_OES_OES8         ( _PC_OES).Bits.OES8
#define PC_OES_OES9         ( _PC_OES).Bits.OES9
#define PC_OES_OES10        ( _PC_OES).Bits.OES10
#define PC_OES_OES11        ( _PC_OES).Bits.OES11
#define PC_OES_OES12        ( _PC_OES).Bits.OES12
#define PC_OES_OES13        ( _PC_OES).Bits.OES13
#define PC_OES_OES14        (_PC_OES).Bits.OES14
#define PC_OES_OES15        (_PC_OES).Bits.OES15

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned OEC0 : 1;
        unsigned OEC1 : 1;
        unsigned OEC2 : 1;
        unsigned OEC3 : 1;
        unsigned OEC4 : 1;
        unsigned OEC5 : 1;
        unsigned OEC6 : 1;
        unsigned OEC7 : 1;
        unsigned OEC8 : 1;
        unsigned OEC9 : 1;
        unsigned OEC10 : 1;
        unsigned OEC11 : 1;
        unsigned OEC12 : 1;
        unsigned OEC13 : 1;
        unsigned OEC14 : 1;
        unsigned OEC15 : 1;
        unsigned :16;
    } Bits;
} PC_OEC_STR;

#define _PC_OEC               (*(PC_OEC_STR*)(GPIOC_BASE + 0x18))
#define PC_OEC                (_PC_OEC).Word
#define PC_OEC_OEC0           (_PC_OEC).Bits.OEC0
#define PC_OEC_OEC1           (_PC_OEC).Bits.OEC1
#define PC_OEC_OEC2           (_PC_OEC).Bits.OEC2
#define PC_OEC_OEC3           (_PC_OEC).Bits.OEC3
#define PC_OEC_OEC4           (_PC_OEC).Bits.OEC4
#define PC_OEC_OEC5           (_PC_OEC).Bits.OEC5
#define PC_OEC_OEC6           (_PC_OEC).Bits.OEC6
#define PC_OEC_OEC7           (_PC_OEC).Bits.OEC7
#define PC_OEC_OEC8           (_PC_OEC).Bits.OEC8
#define PC_OEC_OEC9           (_PC_OEC).Bits.OEC9
#define PC_OEC_OEC10          (_PC_OEC).Bits.OEC10
#define PC_OEC_OEC11          (_PC_OEC).Bits.OEC11
#define PC_OEC_OEC12           (_PC_OEC).Bits.OEC12
#define PC_OEC_OEC13           (_PC_OEC).Bits.OEC13
#define PC_OEC_OEC14           (_PC_OEC).Bits.OEC14
#define PC_OEC_OEC15           (_PC_OEC).Bits.OEC15
typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ITES0      :1;
        unsigned ITES1      :1;
        unsigned ITES2      :1;
        unsigned ITES3      :1;
        unsigned ITES4      :1;
        unsigned ITES5      :1;
        unsigned ITES6      :1;
        unsigned ITES7      :1;
        unsigned ITES8      :1;
        unsigned ITES9      :1;
        unsigned ITES10     :1;
        unsigned ITES11     :1;
        unsigned ITES12     :1;
        unsigned ITES13     :1;
        unsigned ITES14     :1;
        unsigned ITES15     :1;
        unsigned            :16;
    } Bits;
} PC_ITES_STR;
#define _PC_ITES             *(PC_ITES_STR *)(GPIOC_BASE + 0x1C)
#define PC_ITES             (_PC_ITES).Word
#define PC_ITES_ITES0       (_PC_ITES).Bits.ITES0
#define PC_ITES_ITES1       (_PC_ITES).Bits.ITES1
#define PC_ITES_ITES2       (_PC_ITES).Bits.ITES2
#define PC_ITES_ITES3       (_PC_ITES).Bits.ITES3
#define PC_ITES_ITES4       (_PC_ITES).Bits.ITES4
#define PC_ITES_ITES5       (_PC_ITES).Bits.ITES5
#define PC_ITES_ITES6       (_PC_ITES).Bits.ITES6
#define PC_ITES_ITES7       (_PC_ITES).Bits.ITES7
#define PC_ITES_ITES8       (_PC_ITES).Bits.ITES8
#define PC_ITES_ITES9       (_PC_ITES).Bits.ITES9
#define PC_ITES_ITES10      (_PC_ITES).Bits.ITES10
#define PC_ITES_ITES11      (_PC_ITES).Bits.ITES11
#define PC_ITES_ITES12      (_PC_ITES).Bits.ITES12
#define PC_ITES_ITES13      (_PC_ITES).Bits.ITES13
#define PC_ITES_ITES14      (_PC_ITES).Bits.ITES14
#define PC_ITES_ITES15      (_PC_ITES).Bits.ITES15

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ITEC0      :1;
        unsigned ITEC1      :1;
        unsigned ITEC2      :1;
        unsigned ITEC3      :1;
        unsigned ITEC4      :1;
        unsigned ITEC5      :1;
        unsigned ITEC6      :1;
        unsigned ITEC7      :1;
        unsigned ITEC8      :1;
        unsigned ITEC9      :1;
        unsigned ITEC10     :1;
        unsigned ITEC11     :1;
        unsigned ITEC12     :1;
        unsigned ITEC13     :1;
        unsigned ITEC14     :1;
        unsigned ITEC15     :1;
        unsigned            :16;
    } Bits;
} PC_ITEC_STR;
#define _PC_ITEC            *(PC_ITEC_STR*)(GPIOC_BASE + 0x20)
#define PC_ITEC             (_PC_ITEC).Word
#define PC_ITEC_ITEC0       (_PC_ITEC).Bits.ITEC0
#define PC_ITEC_ITEC1       (_PC_ITEC).Bits.ITEC1
#define PC_ITEC_ITEC2       (_PC_ITEC).Bits.ITEC2
#define PC_ITEC_ITEC3       (_PC_ITEC).Bits.ITEC3
#define PC_ITEC_ITEC4       (_PC_ITEC).Bits.ITEC4
#define PC_ITEC_ITEC5       (_PC_ITEC).Bits.ITEC5
#define PC_ITEC_ITEC6       (_PC_ITEC).Bits.ITEC6
#define PC_ITEC_ITEC7       (_PC_ITEC).Bits.ITEC7
#define PC_ITEC_ITEC8       (_PC_ITEC).Bits.ITEC8
#define PC_ITEC_ITEC9       (_PC_ITEC).Bits.ITEC9
#define PC_ITEC_ITEC10      (_PC_ITEC).Bits.ITEC10
#define PC_ITEC_ITEC11      (_PC_ITEC).Bits.ITEC11
#define PC_ITEC_ITEC12      (_PC_ITEC).Bits.ITEC12
#define PC_ITEC_ITEC13      (_PC_ITEC).Bits.ITEC13
#define PC_ITEC_ITEC14      (_PC_ITEC).Bits.ITEC14
#define PC_ITEC_ITEC15      (_PC_ITEC).Bits.ITEC15

typedef volatile union
{
    volatile unsigned int Word;
    struct
    {
        unsigned ITT0   : 3;
        unsigned        : 1;
        unsigned ITT1   : 3;
        unsigned        : 1;
        unsigned ITT2   : 3;
        unsigned        : 1;
        unsigned ITT3   : 3;
        unsigned        : 1;
        unsigned ITT4   : 3;
        unsigned        : 1;
        unsigned ITT5   : 3;
        unsigned        : 1;
        unsigned ITT6   : 3;
        unsigned        : 1;
        unsigned ITT7   : 3;
        unsigned        : 1;
    } Bits;
} PC_ITT0_STR;
#define _PC_ITT0            *(PC_ITT0_STR *)(GPIOC_BASE + 0x24)
#define PC_ITT0             (_PC_ITT0).Word
#define PC_ITT0_ITT00       (_PC_ITT0).Bits.ITT0
#define PC_ITT0_ITT01       (_PC_ITT0).Bits.ITT1
#define PC_ITT0_ITT02       (_PC_ITT0).Bits.ITT2
#define PC_ITT0_ITT03       (_PC_ITT0).Bits.ITT3
#define PC_ITT0_ITT04       (_PC_ITT0).Bits.ITT4
#define PC_ITT0_ITT05       (_PC_ITT0).Bits.ITT5
#define PC_ITT0_ITT06       (_PC_ITT0).Bits.ITT6
#define PC_ITT0_ITT07       (_PC_ITT0).Bits.ITT7

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ITT8       :3;
        unsigned           :1;
        unsigned ITT9       :3;
        unsigned           :1;
        unsigned ITT10      :3;
        unsigned           :1;
        unsigned ITT11      :3;
        unsigned           :1;
        unsigned ITT12      :3;
        unsigned           :1;
        unsigned ITT13      :3;
        unsigned           :1;
        unsigned ITT14      :3;
        unsigned           :1;
        unsigned ITT15      :3;
        unsigned           :1;
    } Bits;
} PC_ITT1_STR;
#define _PC_ITT1            *(PC_ITT1_STR *)(GPIOC_BASE + 0x28)
#define PC_ITT1             (_PC_ITT1).Word
#define PC_ITT1_ITT8        (_PC_ITT1).Bits.ITT8
#define PC_ITT1_ITT9        (_PC_ITT1).Bits.ITT9
#define PC_ITT1_ITT10       (_PC_ITT1).Bits.ITT10
#define PC_ITT1_ITT11       (_PC_ITT1).Bits.ITT11
#define PC_ITT1_ITT12       (_PC_ITT1).Bits.ITT12
#define PC_ITT1_ITT13       (_PC_ITT1).Bits.ITT13
#define PC_ITT1_ITT14       (_PC_ITT1).Bits.ITT14
#define PC_ITT1_ITT15       (_PC_ITT1).Bits.ITT15

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ITTC0      :1;
        unsigned ITTC1      :1;
        unsigned ITTC2      :1;
        unsigned ITTC3      :1;
        unsigned ITTC4      :1;
        unsigned ITTC5      :1;
        unsigned ITTC6      :1;
        unsigned ITTC7      :1;
        unsigned ITTC8      :1;
        unsigned ITTC9      :1;
        unsigned ITTC10     :1;
        unsigned ITTC11     :1;
        unsigned ITTC12     :1;
        unsigned ITTC13     :1;
        unsigned ITTC14     :1;
        unsigned ITTC15     :1;
        unsigned            :16;
    } Bits;
} PC_ITTC_STR;
#define _PC_ITTC            *(PC_ITTC_STR *)(GPIOC_BASE + 0x34)
#define PC_ITTC             (_PC_ITTC).Word
#define PC_ITTC_ITTC0       (_PC_ITTC).Bits.ITTC0
#define PC_ITTC_ITTC1       (_PC_ITTC).Bits.ITTC1
#define PC_ITTC_ITTC2       (_PC_ITTC).Bits.ITTC2
#define PC_ITTC_ITTC3       (_PC_ITTC).Bits.ITTC3
#define PC_ITTC_ITTC4       (_PC_ITTC).Bits.ITTC4
#define PC_ITTC_ITTC5       (_PC_ITTC).Bits.ITTC5
#define PC_ITTC_ITTC6       (_PC_ITTC).Bits.ITTC6
#define PC_ITTC_ITTC7       (_PC_ITTC).Bits.ITTC7
#define PC_ITTC_ITTC8       (_PC_ITTC).Bits.ITTC8
#define PC_ITTC_ITTC9       (_PC_ITTC).Bits.ITTC9
#define PC_ITTC_ITTC10      (_PC_ITTC).Bits.ITTC10
#define PC_ITTC_ITTC11      (_PC_ITTC).Bits.ITTC11
#define PC_ITTC_ITTC12      (_PC_ITTC).Bits.ITTC12
#define PC_ITTC_ITTC13      (_PC_ITTC).Bits.ITTC13
#define PC_ITTC_ITTC14      (_PC_ITTC).Bits.ITTC14
#define PC_ITTC_ITTC15      (_PC_ITTC).Bits.ITTC15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ITS0 :1;
        unsigned ITS1 :1;
        unsigned ITS2 :1;
        unsigned ITS3 :1;
        unsigned ITS4 :1;
        unsigned ITS5 :1;
        unsigned ITS6 :1;
        unsigned ITS7 :1;
        unsigned ITS8 :1;
        unsigned ITS9 :1;
        unsigned ITS10 :1;
        unsigned ITS11 :1;
        unsigned ITS12 :1;
        unsigned ITS13 :1;
        unsigned ITS14 :1;
        unsigned ITS15 :1;
        unsigned       :16;
    } Bits;
} PC_ITSR_STR;
#define _PC_ITSR        *(PC_ITSR_STR*)(GPIOC_BASE + 0x38)
#define PC_ITSR         (_PC_ITSR).Word
#define PC_ITSR_ITS0    (_PC_ITSR).Bits.ITS0
#define PC_ITSR_ITS1    (_PC_ITSR).Bits.ITS1
#define PC_ITSR_ITS2    (_PC_ITSR).Bits.ITS2
#define PC_ITSR_ITS3    (_PC_ITSR).Bits.ITS3
#define PC_ITSR_ITS4    (_PC_ITSR).Bits.ITS4
#define PC_ITSR_ITS5    (_PC_ITSR).Bits.ITS5
#define PC_ITSR_ITS6    (_PC_ITSR).Bits.ITS6
#define PC_ITSR_ITS7    (_PC_ITSR).Bits.ITS7
#define PC_ITSR_ITS8    (_PC_ITSR).Bits.ITS8
#define PC_ITSR_ITS9    (_PC_ITSR).Bits.ITS9
#define PC_ITSR_ITS10   (_PC_ITSR).Bits.ITS10
#define PC_ITSR_ITS11   (_PC_ITSR).Bits.ITS11
#define PC_ITSR_ITS12   (_PC_ITSR).Bits.ITS12
#define PC_ITSR_ITS13   (_PC_ITSR).Bits.ITS13
#define PC_ITSR_ITS14   (_PC_ITSR).Bits.ITS14
#define PC_ITSR_ITS15   (_PC_ITSR).Bits.ITS15

typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned AF0    :3;
        unsigned        :1;
        unsigned AF1    :3;
        unsigned        :1;
        unsigned AF2    :3;
        unsigned        :1;
        unsigned AF3    :3;
        unsigned        :1;
        unsigned AF4    :3;
        unsigned        :1;
        unsigned AF5    :3;
        unsigned        :1;
        unsigned AF6    :3;
        unsigned        :1;
        unsigned AF7     :3;
        unsigned         :1;
    } Bits;
} PC_AF0_STR;
#define _PC_AF0     *(PC_AF0_STR*)(GPIOC_BASE+0x3C)
#define PC_AF0      (_PC_AF0).Word
#define PC_AF0_AF0  (_PC_AF0).Bits.AF0
#define PC_AF0_AF1  (_PC_AF0).Bits.AF1
#define PC_AF0_AF2  (_PC_AF0).Bits.AF2
#define PC_AF0_AF3  (_PC_AF0).Bits.AF3
#define PC_AF0_AF4  (_PC_AF0).Bits.AF4
#define PC_AF0_AF5  (_PC_AF0).Bits.AF5
#define PC_AF0_AF6  (_PC_AF0).Bits.AF6
#define PC_AF0_AF7  (_PC_AF0).Bits.AF7


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned AF8    :3;
        unsigned        :1;
        unsigned AF9    :3;
        unsigned        :1;
        unsigned AF10   :3;
        unsigned        :1;
        unsigned AF11   :3;
        unsigned        :1;
        unsigned AF12   :3;
        unsigned        :1;
        unsigned AF13   :3;
        unsigned        :1;
        unsigned AF14   :3;
        unsigned        :1;
        unsigned AF15   :3;
        unsigned        :1;
    } Bits;
} PC_AF1_STR;
#define _PC_AF1         *(PC_AF1_STR *)(GPIOC_BASE + 0x40)
#define PC_AF1          (_PC_AF1).Word
#define PC_AF1_AF8      (_PC_AF1).Bits.AF8
#define PC_AF1_AF9      (_PC_AF1).Bits.AF9
#define PC_AF1_AF10     (_PC_AF1).Bits.AF10
#define PC_AF1_AF11     (_PC_AF1).Bits.AF11
#define PC_AF1_AF12     (_PC_AF1).Bits.AF12
#define PC_AF1_AF13     (_PC_AF1).Bits.AF13
#define PC_AF1_AF14     (_PC_AF1).Bits.AF14
#define PC_AF1_AF15     (_PC_AF1).Bits.AF15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned AFCC0 :1;
        unsigned AFCC1 :1;
        unsigned AFC2 :1;
        unsigned AFC3 :1;
        unsigned AFC4 :1;
        unsigned AFC5 :1;
        unsigned AFC6 :1;
        unsigned AFC7 :1;
        unsigned AFC8 :1;
        unsigned AFC9 :1;
        unsigned AFC10 :1;
        unsigned AFC11 :1;
        unsigned AFC12 :1;
        unsigned AFC13 :1;
        unsigned AFC14 :1;
        unsigned AFC15 :1;
        unsigned       :16;
    } Bits;
} PC_AFC_STR;
#define _PC_AFC         (*PC_AFC_STR*)(GPIOC_BASE + 0x4C)
#define PC_AFC          (_PC_AFC).Word
#define PC_AFC_AFC0     (_PC_AFC).Bits.AFC0
#define PC_AFC_AFC1     (_PC_AFC).Bits.AFC1
#define PC_AFC_AFC2     (_PC_AFC).Bits.AFC2
#define PC_AFC_AFC3     (_PC_AFC).Bits.AFC3
#define PC_AFC_AFC4     (_PC_AFC).Bits.AFC4
#define PC_AFC_AFC5     (_PC_AFC).Bits.AFC5
#define PC_AFC_AFC6     (_PC_AFC).Bits.AFC6
#define PC_AFC_AFC7     (_PC_AFC).Bits.AFC7
#define PC_AFC_AFC8     (_PC_AFC).Bits.AFC8
#define PC_AFC_AFC9     (_PC_AFC).Bits.AFC9
#define PC_AFC_AFC10    (_PC_AFC).Bits.AFC10
#define PC_AFC_AFC11    (_PC_AFC).Bits.AFC11
#define PC_AFC_AFC12    (_PC_AFC).Bits.AFC12
#define PC_AFC_AFC13    (_PC_AFC).Bits.AFC13
#define PC_AFC_AFC14    (_PC_AFC).Bits.AFC14
#define PC_AFC_AFC15    (_PC_AFC).Bits.AFC15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ANA0 :1;
        unsigned ANA1 :1;
        unsigned ANA2 :1;
        unsigned ANA3 :1;
        unsigned ANA4 :1;
        unsigned ANA5 :1;
        unsigned ANA6 :1;
        unsigned ANA7 :1;
        unsigned ANA8 :1;
        unsigned ANA9 :1;
        unsigned ANA10 :1;
        unsigned ANA11 :1;
        unsigned ANA12 :1;
        unsigned ANA13 :1;
        unsigned ANA14 :1;
        unsigned ANA15 :1;
        unsigned       :16;
    } Bits;
} PC_ANA_STR;
#define _PC_ANA         (*(PC_ANA_STR *)(GPIOC_BASE + 0x50)
#define PC_ANA          (_PC_ANA).Word
#define PC_ANA_ANA0     (_PC_ANA).Bits.ANA0
#define PC_ANA_ANA1     (_PC_ANA).Bits.ANA1
#define PC_ANA_ANA2     (_PC_ANA).Bits.ANA2
#define PC_ANA_ANA3     (_PC_ANA).Bits.ANA3
#define PC_ANA_ANA4     (_PC_ANA).Bits.ANA4
#define PC_ANA_ANA5     (_PC_ANA).Bits.ANA5
#define PC_ANA_ANA6     (_PC_ANA).Bits.ANA6
#define PC_ANA_ANA7     (_PC_ANA).Bits.ANA7
#define PC_ANA_ANA8     (_PC_ANA).Bits.ANA8
#define PC_ANA_ANA9     (_PC_ANA).Bits.ANA9
#define PC_ANA_ANA10    (_PC_ANA).Bits.ANA10
#define PC_ANA_ANA11    (_PC_ANA).Bits.ANA11
#define PC_ANA_ANA12    (_PC_ANA).Bits.ANA12
#define PC_ANA_ANA13    (_PC_ANA).Bits.ANA13
#define PC_ANA_ANA14    (_PC_ANA).Bits.ANA14
#define PC_ANA_ANA15    (_PC_ANA).Bits.ANA15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ANAC0 :1;
        unsigned ANAC1 :1;
        unsigned ANAC2 :1;
        unsigned ANAC3 :1;
        unsigned ANAC4 :1;
        unsigned ANAC5 :1;
        unsigned ANAC6 :1;
        unsigned ANAC7 :1;
        unsigned ANAC8 :1;
        unsigned ANAC9 :1;
        unsigned ANAC10 :1;
        unsigned ANAC11 :1;
        unsigned ANAC12 :1;
        unsigned ANAC13 :1;
        unsigned ANAC14 :1;
        unsigned ANAC15 :1;
        unsigned        :16;
    } Bits;
} PC_ANAC_STR;
#define _PC_ANAC            *(PC_ANAC_STR*)(GPIOC_BASE + 0x54)
#define PC_ANAC             (_PC_ANAC).Word
#define PC_ANAC_ANAC0       (_PC_ANAC).Bits.ANAC0
#define PC_ANAC_ANAC1       (_PC_ANAC).Bits.ANAC1
#define PC_ANAC_ANAC2       (_PC_ANAC).Bits.ANAC2
#define PC_ANAC_ANAC3       (_PC_ANAC).Bits.ANAC3
#define PC_ANAC_ANAC4       (_PC_ANAC).Bits.ANAC4
#define PC_ANAC_ANAC5       (_PC_ANAC).Bits.ANAC5
#define PC_ANAC_ANAC6       (_PC_ANAC).Bits.ANAC6
#define PC_ANAC_ANAC7       (_PC_ANAC).Bits.ANAC7
#define PC_ANAC_ANAC8       (_PC_ANAC).Bits.ANAC8
#define PC_ANAC_ANAC9       (_PC_ANAC).Bits.ANAC9
#define PC_ANAC_ANAC10      (_PC_ANAC).Bits.ANAC10
#define PC_ANAC_ANAC11      (_PC_ANAC).Bits.ANAC11
#define PC_ANAC_ANAC12      (_PC_ANAC).Bits.ANAC12
#define PC_ANAC_ANAC13      (_PC_ANAC).Bits.ANAC13
#define PC_ANAC_ANAC14      (_PC_ANAC).Bits.ANAC14
#define PC_ANAC_ANAC15      (_PC_ANAC).Bits.ANAC15


typedef volatile union{
    volatile unsigned int Word;
    struct{
        unsigned PU0 : 1;
        unsigned PU1 : 1;
        unsigned PU2 : 1;
        unsigned PU3 : 1;
        unsigned PU4 : 1;
        unsigned PU5 : 1;
        unsigned PU6 : 1;
        unsigned PU7 : 1;
        unsigned PU8 : 1;
        unsigned PU9 : 1;
        unsigned PU10 : 1;
        unsigned PU11 : 1;
        unsigned PU12 : 1;
        unsigned PU13 : 1;
        unsigned PU14 : 1;
        unsigned PU15 : 1;
        unsigned      : 16;
    } Bits;
} PC_PU_STR;
#define _PC_PU      *(PC_PU_STR *)(GPIOC_BASE + 0x58)
#define PC_PU       (_PC_PU).Word
#define PC_PU_PU0   (_PC_PU).Bits.PU0
#define PC_PU_PU1   (_PC_PU).Bits.PU1
#define PC_PU_PU2   (_PC_PU).Bits.PU2
#define PC_PU_PU3   (_PC_PU).Bits.PU3
#define PC_PU_PU4   (_PC_PU).Bits.PU4
#define PC_PU_PU5   (_PC_PU).Bits.PU5
#define PC_PU_PU6   (_PC_PU).Bits.PU6
#define PC_PU_PU7   (_PC_PU).Bits.PU7
#define PC_PU_PU8   (_PC_PU).Bits.PU8
#define PC_PU_PU9   (_PC_PU).Bits.PU9
#define PC_PU_PU10  (_PC_PU).Bits.PU10
#define PC_PU_PU11  (_PC_PU).Bits.PU11
#define PC_PU_PU12  (_PC_PU).Bits.PU12
#define PC_PU_PU13  (_PC_PU).Bits.PU13
#define PC_PU_PU14  (_PC_PU).Bits.PU14
#define PC_PU_PU15  (_PC_PU).Bits.PU15


typedef volatile union{
    volatile unsigned int Word;
    struct{
        unsigned SPU0 : 1;
        unsigned SPU1 : 1;
        unsigned SPU2 : 1;
        unsigned SPU3 : 1;
        unsigned SPU4 : 1;
        unsigned SPU5 : 1;
        unsigned SPU6 : 1;
        unsigned SPU7 : 1;
        unsigned SPU8 : 1;
        unsigned SPU9 : 1;
        unsigned SPU10 : 1;
        unsigned SPU11 : 1;
        unsigned SPU12 : 1;
        unsigned SPU13 : 1;
        unsigned SPU14 : 1;
        unsigned SPU15 : 1;
        unsigned       : 16;
    } Bits;
} PC_SPU_STR;
#define _PC_SPU         (*(PC_SPU_STR *)(GPIOC_BASE + 0x5C)
#define PC_SPU          (_PC_SPU).Word
#define PC_SPU_SPU0     (_PC_SPU).Bits.SPU0
#define PC_SPU_SPU1     (_PC_SPU).Bits.SPU1
#define PC_SPU_SPU2     (_PC_SPU).Bits.SPU2
#define PC_SPU_SPU3     (_PC_SPU).Bits.SPU3
#define PC_SPU_SPU4     (_PC_SPU).Bits.SPU4
#define PC_SPU_SPU5     (_PC_SPU).Bits.SPU5
#define PC_SPU_SPU6     (_PC_SPU).Bits.SPU6
#define PC_SPU_SPU7     (_PC_SPU).Bits.SPU7
#define PC_SPU_SPU8     (_PC_SPU).Bits.SPU8
#define PC_SPU_SPU9     (_PC_SPU).Bits.SPU9
#define PC_SPU_SPU10    (_PC_SPU).Bits.SPU10
#define PC_SPU_SPU11    (_PC_SPU).Bits.SPU11
#define PC_SPU_SPU12    (_PC_SPU).Bits.SPU12
#define PC_SPU_SPU13    (_PC_SPU).Bits.SPU13
#define PC_SPU_SPU14    (_PC_SPU).Bits.SPU14
#define PC_SPU_SPU15    (_PC_SPU).Bits.SPU15


typedef volatile union{
    volatile unsigned int Word;
    struct{
        unsigned PUC0 : 1;
        unsigned PUC1 : 1;
        unsigned PUC2 : 1;
        unsigned PUC3 : 1;
        unsigned PUC4 : 1;
        unsigned PUC5 : 1;
        unsigned PUC6 : 1;
        unsigned PUC7 : 1;
        unsigned PUC8 : 1;
        unsigned PUC9 : 1;
        unsigned PUC10 : 1;
        unsigned PUC11 : 1;
        unsigned PUC12 : 1;
        unsigned PUC13 : 1;
        unsigned PUC14 : 1;
        unsigned PUC15 : 1;
        unsigned       : 16;
    } Bits;
} PC_PUC_STR;
#define _PC_PUC         (*(PC_PUC_STR *)(GPIOC_BASE + 0x60))
#define PC_PUC          (_PC_PUC).Word
#define PC_PUC_PUC0     (_PC_PUC).Bits.PUC0
#define PC_PUC_PUC1     (_PC_PUC).Bits.PUC1
#define PC_PUC_PUC2     (_PC_PUC).Bits.PUC2
#define PC_PUC_PUC3     (_PC_PUC).Bits.PUC3
#define PC_PUC_PUC4     (_PC_PUC).Bits.PUC4
#define PC_PUC_PUC5     (_PC_PUC).Bits.PUC5
#define PC_PUC_PUC6     (_PC_PUC).Bits.PUC6
#define PC_PUC_PUC7     (_PC_PUC).Bits.PUC7
#define PC_PUC_PUC8     (_PC_PUC).Bits.PUC8
#define PC_PUC_PUC9     (_PC_PUC).Bits.PUC9
#define PC_PUC_PUC10    (_PC_PUC).Bits.PUC10
#define PC_PUC_PUC11    (_PC_PUC).Bits.PUC11
#define PC_PUC_PUC12    (_PC_PUC).Bits.PUC12
#define PC_PUC_PUC13    (_PC_PUC).Bits.PUC13
#define PC_PUC_PUC14    (_PC_PUC).Bits.PUC14
#define PC_PUC_PUC15    (_PC_PUC).Bits.PUC15


typedef volatile union{
    volatile unsigned int Word;
    struct{
        unsigned PD0 : 1;
        unsigned PD1 : 1;
        unsigned PD2 : 1;
        unsigned PD3 : 1;
        unsigned PD4 : 1;
        unsigned PD5 : 1;
        unsigned PD6 : 1;
        unsigned PD7 : 1;
        unsigned PD8 : 1;
        unsigned PD9 : 1;
        unsigned PD10 : 1;
        unsigned PD11 : 1;
        unsigned PD12 : 1;
        unsigned PD13 : 1;
        unsigned PD14 : 1;
        unsigned PD15 : 1;
        unsigned      : 16;
    } Bits;
} PC_PD_STR;
#define _PC_PD      *(PC_PD_STR *)(GPIOC_BASE + 0x64)
#define PC_PD       (_PC_PD).Word
#define PC_PD_PD0   (_PC_PD).Bits.PD0
#define PC_PD_PD1   (_PC_PD).Bits.PD1
#define PC_PD_PD2   (_PC_PD).Bits.PD2
#define PC_PD_PD3   (_PC_PD).Bits.PD3
#define PC_PD_PD4   (_PC_PD).Bits.PD4
#define PC_PD_PD5   (_PC_PD).Bits.PD5
#define PC_PD_PD6   (_PC_PD).Bits.PD6
#define PC_PD_PD7   (_PC_PD).Bits.PD7
#define PC_PD_PD8   (_PC_PD).Bits.PD8
#define PC_PD_PD9   (_PC_PD).Bits.PD9
#define PC_PD_PD10  (_PC_PD).Bits.PD10
#define PC_PD_PD11  (_PC_PD).Bits.PD11
#define PC_PD_PD12  (_PC_PD).Bits.PD12
#define PC_PD_PD13  (_PC_PD).Bits.PD13
#define PC_PD_PD14  (_PC_PD).Bits.PD14
#define PC_PD_PD15  (_PC_PD).Bits.PD15


typedef volatile union{
    volatile unsigned int Word;
    struct{
        unsigned PDC0 : 1;
        unsigned PDC1 : 1;
        unsigned PDC2 : 1;
        unsigned PDC3 : 1;
        unsigned PDC4 : 1;
        unsigned PDC5 : 1;
        unsigned PDC6 : 1;
        unsigned PDC7 : 1;
        unsigned PDC8 : 1;
        unsigned PDC9 : 1;
        unsigned PDC10 : 1;
        unsigned PDC11 : 1;
        unsigned PDC12 : 1;
        unsigned PDC13 : 1;
        unsigned PDC14 : 1;
        unsigned PDC15 : 1;
        unsigned       : 16;
    } Bits;
} PC_PDC_STR;
#define _PC_PDC              (*(PC_PDC_STR*)(GPIOC_BASE + 0x68)
#define PC_PDC               (_PC_PDC).Word
#define PC_PDC_PDC0          (_PC_PDC).Bits.PDC0
#define PC_PDC_PDC1          (_PC_PDC).Bits.PDC1
#define PC_PDC_PDC2          (_PC_PDC).Bits.PDC2
#define PC_PDC_PDC3          (_PC_PDC).Bits.PDC3
#define PC_PDC_PDC4          (_PC_PDC).Bits.PDC4
#define PC_PDC_PDC5          (_PC_PDC).Bits.PDC5
#define PC_PDC_PDC6          (_PC_PDC).Bits.PDC6
#define PC_PDC_PDC7          (_PC_PDC).Bits.PDC7
#define PC_PDC_PDC8          (_PC_PDC).Bits.PDC8
#define PC_PDC_PDC9          (_PC_PDC).Bits.PDC9
#define PC_PDC_PDC10         (_PC_PDC).Bits.PDC10
#define PC_PDC_PDC11         (_PC_PDC).Bits.PDC11
#define PC_PDC_PDC12          (_PC_PDC).Bits.PDC12
#define PC_PDC_PDC13          (_PC_PDC).Bits.PDC13
#define PC_PDC_PDC14          (_PC_PDC).Bits.PDC14
#define PC_PDC_PDC15          (_PC_PDC).Bits.PDC15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DRVS0     :3;
        unsigned           :1;
        unsigned DRVS1     :3;
        unsigned           :1;
        unsigned DRVS2     :3;
        unsigned           :1;
        unsigned DRVS3     :3;
        unsigned           :1;
        unsigned DRVS4     :3;
        unsigned           :1;
        unsigned DRVS5     :3;
        unsigned           :1;
        unsigned DRVS6     :3;
        unsigned           :1;
        unsigned DRVS7     :3;
        unsigned           :1;
    } Bits;
} PC_DRVS0_STR;
#define _PC_DRVS0          (*(PC_DRVS0_STR *)(GPIOC_BASE + 0x6C)
#define PC_DRVS0           (_PC_DRVS0).Word
#define PC_DRVS0_DRVS0     (_PC_DRVS0).Bits.DRVS0
#define PC_DRVS0_DRVS1     (_PC_DRVS0).Bits.DRVS1
#define PC_DRVS0_DRVS2     (_PC_DRVS0).Bits.DRVS2
#define PC_DRVS0_DRVS3     (_PC_DRVS0).Bits.DRVS3
#define PC_DRVS0_DRVS4     (_PC_DRVS0).Bits.DRVS4
#define PC_DRVS0_DRVS5     (_PC_DRVS0).Bits.DRVS5
#define PC_DRVS0_DRVS6     (_PC_DRVS0).Bits.DRVS6
#define PC_DRVS0_DRVS7     (_PC_DRVS0).Bits.DRVS7


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DRVS8 :3;
        unsigned        :1;
        unsigned DRVS9 :3;
        unsigned        :1;
        unsigned DRVS10 :3;
        unsigned        :1;
        unsigned DRVS11 :3;
        unsigned        :1;
        unsigned DRVS12 :3;
        unsigned        :1;
        unsigned DRVS13 :3;
        unsigned        :1;
        unsigned DRVS14 :3;
        unsigned        :1;
        unsigned DRVS15 :3;
        unsigned        :1;
    } Bits;
} PC_DRVS1_STR;
#define _PC_DRVS1           *(PC_DRVS1_STR*)(GPIOC_BASE + 0x70)
#define PC_DRVS1            (_PC_DRVS1).Word
#define PC_DRVS1_DRVS8      (_PC_DRVS1).Bits.DRVS8
#define PC_DRVS1_DRVS9      (_PC_DRVS1).Bits.DRVS9
#define PC_DRVS1_DRVS10     (_PC_DRVS1).Bits.DRVS10
#define PC_DRVS1_DRVS11     (_PC_DRVS1).Bits.DRVS11
#define PC_DRVS1_DRVS12     (_PC_DRVS1).Bits.DRVS12
#define PC_DRVS1_DRVS13     (_PC_DRVS1).Bits.DRVS13
#define PC_DRVS1_DRVS14     (_PC_DRVS1).Bits.DRVS14
#define PC_DRVS1_DRVS15     (_PC_DRVS1).Bits.DRVS15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned DRVC0 :1;
        unsigned DRVC1 :1;
        unsigned DRVC2 :1;
        unsigned DRVC3 :1;
        unsigned DRVC4 :1;
        unsigned DRVC5 :1;
        unsigned DRVC6 :1;
        unsigned DRVC7 :1;
        unsigned DRVC8 :1;
        unsigned DRVC9 :1;
        unsigned DRVC10 :1;
        unsigned DRVC11 :1;
        unsigned DRVC12 :1;
        unsigned DRVC13 :1;
        unsigned DRVC14 :1;
        unsigned DRVC15 :1;
        unsigned        :16;
    } Bits;
} PC_DRVC_STR;
#define _PC_DRVC        (*(PC_DRVC_STR *)(GPIOC_BASE + 0x7C)
#define PC_DRVC         (_PC_DRVC).Word
#define PC_DRVC_DRVC0   (_PC_DRVC).Bits.DRVC0
#define PC_DRVC_DRVC1   (_PC_DRVC).Bits.DRVC1
#define PC_DRVC_DRVC2   (_PC_DRVC).Bits.DRVC2
#define PC_DRVC_DRVC3   (_PC_DRVC).Bits.DRVC3
#define PC_DRVC_DRVC4   (_PC_DRVC).Bits.DRVC4
#define PC_DRVC_DRVC5   (_PC_DRVC).Bits.DRVC5
#define PC_DRVC_DRVC6   (_PC_DRVC).Bits.DRVC6
#define PC_DRVC_DRVC7   (_PC_DRVC).Bits.DRVC7
#define PC_DRVC_DRVC8   (_PC_DRVC).Bits.DRVC8
#define PC_DRVC_DRVC9   (_PC_DRVC).Bits.DRVC9
#define PC_DRVC_DRVC10  (_PC_DRVC).Bits.DRVC10
#define PC_DRVC_DRVC11  (_PC_DRVC).Bits.DRVC11
#define PC_DRVC_DRVC12  (_PC_DRVC).Bits.DRVC12
#define PC_DRVC_DRVC13  (_PC_DRVC).Bits.DRVC13
#define PC_DRVC_DRVC14  (_PC_DRVC).Bits.DRVC14
#define PC_DRVC_DRVC15  (_PC_DRVC).Bits.DRVC15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned SL0 :1;
        unsigned SL1 :1;
        unsigned SL2 :1;
        unsigned SL3 :1;
        unsigned SL4 :1;
        unsigned SL5 :1;
        unsigned SL6 :1;
        unsigned SL7 :1;
        unsigned SL8 :1;
        unsigned SL9 :1;
        unsigned SL10 :1;
        unsigned SL11 :1;
        unsigned SL12 :1;
        unsigned SL13 :1;
        unsigned SL14 :1;
        unsigned SL15 :1;
        unsigned      :16;
    } Bits;
} PC_SLS_STR;
#define _PC_SLS         *(_PC_SLS_STR*)(GPIOC_BASE+0x80)
#define PC_SLS          (_PC_SLS).Word
#define PC_SLS_SL0      (_PC_SLS).Bits.SL0
#define PC_SLS_SL1      (_PC_SLS).Bits.SL1
#define PC_SLS_SL2      (_PC_SLS).Bits.SL2
#define PC_SLS_SL3      (_PC_SLS).Bits.SL3
#define PC_SLS_SL4      (_PC_SLS).Bits.SL4
#define PC_SLS_SL5      (_PC_SLS).Bits.SL5
#define PC_SLS_SL6      (_PC_SLS).Bits.SL6
#define PC_SLS_SL7      (_PC_SLS).Bits.SL7
#define PC_SLS_SL8      (_PC_SLS).Bits.SL8
#define PC_SLS_SL9      (_PC_SLS).Bits.SL9
#define PC_SLS_SL10     (_PC_SLS).Bits.SL10
#define PC_SLS_SL11     (_PC_SLS).Bits.SL11
#define PC_SLS_SL12     (_PC_SLS).Bits.SL12
#define PC_SLS_SL13     (_PC_SLS).Bits.SL13
#define PC_SLS_SL14     (_PC_SLS).Bits.SL14
#define PC_SLS_SL15     (_PC_SLS).Bits.SL15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned SLC0 :1;
        unsigned SLC1 :1;
        unsigned SLC2 :1;
        unsigned SLC3 :1;
        unsigned SLC4 :1;
        unsigned SLC5 :1;
        unsigned SLC6 :1;
        unsigned SLC7 :1;
        unsigned SLC8 :1;
        unsigned SLC9 :1;
        unsigned SLC10 :1;
        unsigned SLC11 :1;
        unsigned SLC12 :1;
        unsigned SLC13 :1;
        unsigned SLC14 :1;
        unsigned SLC15 :1;
        unsigned       :16;
    } Bits;
} PC_SLC_STR;
#define _PC_SLC         *( PC_SLC_STR *)(GPIOC_BASE + 0x84)
#define PC_SLC          (_PC_SLC).Word
#define PC_SLC_SLC0     (_PC_SLC).Bits.SLC0
#define PC_SLC_SLC1     (_PC_SLC).Bits.SLC1
#define PC_SLC_SLC2     (_PC_SLC).Bits.SLC2
#define PC_SLC_SLC3     (_PC_SLC).Bits.SLC3
#define PC_SLC_SLC4     (_PC_SLC).Bits.SLC4
#define PC_SLC_SLC5     (_PC_SLC).Bits.SLC5
#define PC_SLC_SLC6     (_PC_SLC).Bits.SLC6
#define PC_SLC_SLC7     (_PC_SLC).Bits.SLC7
#define PC_SLC_SLC8     (_PC_SLC).Bits.SLC8
#define PC_SLC_SLC9     (_PC_SLC).Bits.SLC9
#define PC_SLC_SLC10    (_PC_SLC).Bits.SLC10
#define PC_SLC_SLC11    (_PC_SLC).Bits.SLC11
#define PC_SLC_SLC12    (_PC_SLC).Bits.SLC12
#define PC_SLC_SLC13    (_PC_SLC).Bits.SLC13
#define PC_SLC_SLC14    (_PC_SLC).Bits.SLC14
#define PC_SLC_SLC15    (_PC_SLC).Bits.SLC15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned ST0 :1;
        unsigned ST1 :1;
        unsigned ST2 :1;
        unsigned ST3 :1;
        unsigned ST4 :1;
        unsigned ST5 :1;
        unsigned ST6 :1;
        unsigned ST7 :1;
        unsigned ST8 :1;
        unsigned ST9 :1;
        unsigned ST10 :1;
        unsigned ST11 :1;
        unsigned ST12 :1;
        unsigned ST13 :1;
        unsigned ST14 :1;
        unsigned ST15 :1;
        unsigned :16;
    } Bits;
} PC_STS_STR;
#define _PC_STS         (*PC_STS_STR *)(GPIOC_BASE + 0x88)
#define PC_STS          (_PC_STS).Word
#define PC_STS_ST0      (_PC_STS).Bits.ST0
#define PC_STS_ST1      (_PC_STS).Bits.ST1
#define PC_STS_ST2      (_PC_STS).Bits.ST2
#define PC_STS_ST3      (_PC_STS).Bits.ST3
#define PC_STS_ST4      (_PC_STS).Bits.ST4
#define PC_STS_ST5      (_PC_STS).Bits.ST5
#define PC_STS_ST6      (_PC_STS).Bits.ST6
#define PC_STS_ST7      (_PC_STS).Bits.ST7
#define PC_STS_ST8      (_PC_STS).Bits.ST8
#define PC_STS_ST9      (_PC_STS).Bits.ST9
#define PC_STS_ST10     (_PC_STS).Bits.ST10
#define PC_STS_ST11     (_PC_STS).Bits.ST11
#define PC_STS_ST12     (_PC_STS).Bits.ST12
#define PC_STS_ST13     (_PC_STS).Bits.ST13
#define PC_STS_ST14     (_PC_STS).Bits.ST14
#define PC_STS_ST15     (_PC_STS).Bits.ST15


typedef volatile union {
    volatile unsigned int Word;
    struct {
        unsigned STC0 :1;
        unsigned STC1 :1;
        unsigned STC2 :1;
        unsigned STC3 :1;
        unsigned STC4 :1;
        unsigned STC5 :1;
        unsigned STC6 :1;
        unsigned STC7 :1;
        unsigned STC8 :1;
        unsigned STC9 :1;
        unsigned STC10 :1;
        unsigned STC11 :1;
        unsigned STC12 :1;
        unsigned STC13 :1;
        unsigned STC14 :1;
        unsigned STC15 :1;
        unsigned       :16;
    } Bits;
} PC_STC_STR;
#define _PC_STC         *( PC_STC_STR *)(GPIOC_BASE + 0x8C)
#define PC_STC          (_PC_STC).Word
#define PC_STC_STC0     (_PC_STC).Bits.STC0
#define PC_STC_STC1     (_PC_STC).Bits.STC1
#define PC_STC_STC2     (_PC_STC).Bits.STC2
#define PC_STC_STC3     (_PC_STC).Bits.STC3
#define PC_STC_STC4     (_PC_STC).Bits.STC4
#define PC_STC_STC5     (_PC_STC).Bits.STC5
#define PC_STC_STC6     (_PC_STC).Bits.STC6
#define PC_STC_STC7     (_PC_STC).Bits.STC7
#define PC_STC_STC8     (_PC_STC).Bits.STC8
#define PC_STC_STC9     (_PC_STC).Bits.STC9
#define PC_STC_STC10    (_PC_STC).Bits.STC10
#define PC_STC_STC11    (_PC_STC).Bits.STC11
#define PC_STC_STC12    (_PC_STC).Bits.STC12
#define PC_STC_STC13    (_PC_STC).Bits.STC13
#define PC_STC_STC14    (_PC_STC).Bits.STC14
#define PC_STC_STC15    (_PC_STC).Bits.STC15
//PC_------------End----------------

#endif

#define PORTA_ALTFUNC_OUTALT0_MASK               ((uint32_t)0x0000000F)/* bit [3:0] */
#define PORTA_INTTYPE_INTTYPE0_MASK              ((uint32_t)0x0000000F)/* bit [3:0] */

typedef struct {
    union{
        struct{
            __IO uint32_t  MINVN                 : 8;  /* bit [7:0] */
            __IO uint32_t  MAJVN                 : 8;  /* bit [15:8] */
            __IO uint32_t  FEATURE               : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }VERID_STR;   /* 0x00 */
   
    union{
        struct
        {
            __IO  uint32_t DR_STR                  : 16;  /* bit [15:0] */
            __IO  uint32_t RESERVED_Bit_31_16      : 16;  /* bit [31:16] */
        };
        __IO uint32_t w;
    }DR_STR  ;/* 0x04 */

    union{
        struct{
            __IO uint32_t  DRS                      : 16;  /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16       : 16;  /* bit [31:16] */
        };
        __IO uint32_t w;
    }DRS_STR;  /* 0x08 */

    union{
        struct{
            __IO uint32_t  DRC                       : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16        : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }DRC_STR;  /* 0x0C */

    union{
        struct{
            __IO uint32_t  DRT                  : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }DRT_STR;  /* 0x10 */

    union{
        struct{
            __IO uint32_t  OES                  : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }OES_STR;  /* 0x14 */

    union{
        struct{
            __IO uint32_t  OEC                  : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }OEC_STR;  /* 0x18 */

    union{
        struct{
            __IO uint32_t  ITES                 : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }ITES_STR;  /* 0x1C */

    union{
        struct{
            __IO uint32_t  ITEC                 : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }ITEC_STR;  /* 0x20 */

    union{
        struct{
            __IO uint32_t  ITT0                 : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_3       : 1; /* bit [3]   */
            __IO uint32_t  ITT1                 : 3; /* bit [6:4] */
            __IO uint32_t  RESERVED_BIT_7       : 1; /* bit [7]   */
            __IO uint32_t  ITT2                 : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_11      : 1; /* bit [11]  */
            __IO uint32_t  ITT3                 : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_15      : 1; /* bit [15]  */
            __IO uint32_t  ITT4                 : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_19      : 1; /* bit [19]  */
            __IO uint32_t  ITT5                 : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_23      : 1; /* bit [23]  */
            __IO uint32_t  ITT6                 : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_27      : 1; /* bit [27]  */
            __IO uint32_t  ITT7                 : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_31      : 1; /* bit [31]  */
        };
        __IO uint32_t w;
    }ITT0_STR;  /* 0x24 */

    union{
        struct{
            __IO uint32_t  ITT8                 : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_3       : 1; /* bit [3]   */
            __IO uint32_t  ITT9                 : 3; /* bit [6:4] */
            __IO uint32_t  RESERVED_BIT_7       : 1; /* bit [7]   */
            __IO uint32_t  ITT10                : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_11      : 1; /* bit [11]  */
            __IO uint32_t  ITT11                : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_15      : 1; /* bit [15]  */
            __IO uint32_t  ITT12                : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_19      : 1; /* bit [19]  */
            __IO uint32_t  ITT13                : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_23      : 1; /* bit [23]  */
            __IO uint32_t  ITT14                : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_27      : 1; /* bit [27]  */
            __IO uint32_t  ITT15                : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_31      : 1; /* bit [31]  */
        };
        __IO uint32_t w;
    }ITT1_STR;  /* 0x28 */
     
    uint32_t RESERVED2C;    /* 0x2C */
    
    uint32_t RESERVED30;    /* 0x30 */

    union{
        struct{
            __IO uint32_t  ITTC                 : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }ITTC_STR;  /* 0x34 */
    
    union{
        struct{
            __IO uint32_t  ITS                  : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }ITSR_STR;  /* 0x38 */
    
    union{
        struct{
            __IO uint32_t  AF0                  : 4; /* bit [3:0]   */
            __IO uint32_t  AF1                  : 4; /* bit [7:4]   */
            __IO uint32_t  AF2                  : 4; /* bit [11:8]  */
            __IO uint32_t  AF3                  : 4; /* bit [15:12] */
            __IO uint32_t  AF4                  : 4; /* bit [19:16] */
            __IO uint32_t  AF5                  : 4; /* bit [23:20] */
            __IO uint32_t  AF6                  : 4; /* bit [27:24] */
            __IO uint32_t  AF7                  : 4; /* bit [31:28] */
        };
        __IO uint32_t w;
    }AF0_STR;  /* 0x3C */

    union{
        struct{
            __IO uint32_t  AF8                  : 4; /* bit [3:0]   */
            __IO uint32_t  AF9                  : 4; /* bit [7:4]   */
            __IO uint32_t  AF10                 : 4; /* bit [11:8]  */
            __IO uint32_t  AF11                 : 4; /* bit [15:12] */
            __IO uint32_t  AF12                 : 4; /* bit [19:16] */
            __IO uint32_t  AF13                 : 4; /* bit [23:20] */
            __IO uint32_t  AF14                 : 4; /* bit [27:24] */
            __IO uint32_t  AF15                 : 4; /* bit [31:28] */
        };
        __IO uint32_t w;
    }AF1_STR;  /* 0x40 */

    uint32_t RESERVED44;   /* 0x44 */

    uint32_t RESERVED48;   /* 0x48 */
    
    union{
        struct{
            __IO uint32_t  AFC                  : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }AFC_STR;  /* 0x4C */
    
    union{
        struct{
            __IO uint32_t  ANA                  : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }ANA_STR;  /* 0x50 */
    
    union{
        struct{
            __IO uint32_t  ANAC                 : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }ANAC_STR;  /* 0x54 */
    
    union{
        struct{
            __IO uint32_t  PU                 : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }PU_STR;  /* 0x58 */
    
    // union{
    //     struct{
    //         __IO uint32_t  SPU                  : 16; /* bit [15:0] */
    //         __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
    //     };
    //     __IO uint32_t w;
    // }SPU_STR;  /* 0x5C */

    union{
        struct{
            __IO uint32_t  PUC                  : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }PUC_STR;  /* 0x5C */

    union{
        struct{
            __IO uint32_t  PD                   : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }PD_STR;  /* 0x60 */

    union{
        struct{
            __IO uint32_t  PDC                  : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }PDC_STR;  /* 0x64 */

    union{
        struct{
            __IO uint32_t  DRVS0                : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_3       : 1; /* bit [3]   */
            __IO uint32_t  DRVS1                : 3; /* bit [6:4] */
            __IO uint32_t  RESERVED_BIT_7       : 1; /* bit [7]   */
            __IO uint32_t  DRVS2                : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_11      : 1; /* bit [11]  */
            __IO uint32_t  DRVS3                : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_15      : 1; /* bit [15]  */
            __IO uint32_t  DRVS4                : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_19      : 1; /* bit [19]  */
            __IO uint32_t  DRVS5                : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_23      : 1; /* bit [23]  */
            __IO uint32_t  DRVS6                : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_27      : 1; /* bit [27]  */
            __IO uint32_t  DRVS7                : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_31      : 1; /* bit [31]  */
        };
        __IO uint32_t w;
    }DRVS0_STR;  /* 0x68 */

    union{
        struct{
            __IO uint32_t  DRVS8                : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_3       : 1; /* bit [3]   */
            __IO uint32_t  DRVS9                : 3; /* bit [6:4] */
            __IO uint32_t  RESERVED_BIT_7       : 1; /* bit [7]   */
            __IO uint32_t  DRVS10               : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_11      : 1; /* bit [11]  */
            __IO uint32_t  DRVS11               : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_15      : 1; /* bit [15]  */
            __IO uint32_t  DRVS12               : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_19      : 1; /* bit [19]  */
            __IO uint32_t  DRVS13               : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_23      : 1; /* bit [23]  */
            __IO uint32_t  DRVS14               : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_27      : 1; /* bit [27]  */
            __IO uint32_t  DRVS15               : 3; /* bit [2:0] */
            __IO uint32_t  RESERVED_BIT_31      : 1; /* bit [31]  */
        };
        __IO uint32_t w;
    }DRVS1_STR;  /* 0x6C */

    uint32_t RESERVED74;   /* 0x70 */

    uint32_t RESERVED78;   /* 0x74 */
    
    union{
        struct{
            __IO uint32_t  DRVC                 : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }DRVC_STR;  /* 0x78 */

    union{
        struct{
            __IO uint32_t  SL                   : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }SLS_STR;  /* 0x7C */

    union{
        struct{
            __IO uint32_t  SLC                  : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }SLC_STR;  /* 0x80 */

    union{
        struct{
            __IO uint32_t  ST                   : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }STS_STR;  /* 0x84 */

    union{
        struct{
            __IO uint32_t  STC                  : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }STC_STR;  /* 0x88 */

    union{
        struct{
            __IO uint32_t  IES                  : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }IES_STR;  /* 0x8C */

    union{
        struct{
            __IO uint32_t  IEC                  : 16; /* bit [15:0] */
            __IO uint32_t  RESERVED_BIT_31_16   : 16; /* bit [31:16] */
        };
        __IO uint32_t w;
    }IEC_STR;  /* 0x90 */

}PORT_TypeDef;


#define GPIO_A_DATA         REG32(GPIOA_BASE + 0x04) 
#define GPIO_B_DATA         REG32(GPIOB_BASE + 0x04)
#define GPIO_C_DATA         REG32(GPIOC_BASE + 0x04)

#define GPIO_HALL_U         ((GPIO_A_DATA&BIT14)>>14) //PTA14
#define GPIO_HALL_V         ((GPIO_A_DATA&BIT13)>>14) //PTA13
#define GPIO_HALL_W         ((GPIO_A_DATA&BIT12)>>12) //PTA12

#endif

/************************ (C) COPYRIGHT OCS SEMICONDUCTOR *****END OF FILE****/

