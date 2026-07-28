#ifndef MATH_TYPE_H
#define MATH_TYPE_H

#include "stdint.h"


typedef union
{
  uint32_t u32_SinCos;
  struct 
  {
    int16_t Sin;
    int16_t Cos;
  };
}Math_SinCos_Type;
 






#endif
