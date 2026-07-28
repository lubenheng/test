#ifndef INIT_H
#define INIT_H

#include <stdio.h>
#include <stdint.h>
#include "SEGGER_RTT.h"

#define Calib_Time                     (998)  

typedef union
{
    unsigned long U32;
    struct
    {
		unsigned short Low:16;
        unsigned short High:16;
    }Words;
}UNION_U32;

/* Define to prevent recursive inclusion -------------------------------------*/
typedef struct
{
	uint32_t   IuOffsetSum;    //Iu的偏置电压总和
	uint32_t   IvOffsetSum;    //Iv的偏置电压总和
	uint32_t   Iw_busOffsetSum;//Iw或Ibus的偏置电压总和
  uint16_t   IuOffset;       //Iu的偏置电压
  uint16_t   IvOffset;       //Iv的偏置电压
  uint16_t   Iw_busOffset;   //Iw或Ibus的偏置电压
  uint16_t   IuADC;       //Iu的偏置电压
  uint16_t   IvADC;       //Iv的偏置电压
  uint16_t   IwADC;   //Iw或Ibus的偏置电压
  uint16_t   OffsetCount;    //偏置电压采集计数
  uint16_t    OffsetFlag;     //偏置电压结束标志位
}CurrentOffset;

typedef struct _tagRTTStruct
{
	int16_t data1;
	int16_t data2;
	int16_t data3;
	int16_t data4;
}RTTStruct;

void GetCurrentOffset(void);



#endif
