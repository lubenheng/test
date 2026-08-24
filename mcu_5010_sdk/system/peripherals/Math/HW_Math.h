#ifndef MATH_H
#define MATH_H

//#include "stdint.h"

int32_t HWMATH_DIV_DIV(int32_t x,int32_t y) ;

int32_t HWMATH_DIV_MOD(int32_t x,int32_t y) ;



uint32_t HWMATH_SQRT(uint32_t x);
void HWMATH_SINCOS_Cal(int16_t x,Math_SinCos_Type *y);
int16_t HWMATH_SINCOS_Sin(int16_t x)  ;
int16_t HWMATH_SINCOS_Cos(int16_t x) ;


#endif
