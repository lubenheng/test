#ifndef MDU_H
#define MDU_H

#include "CMSDK_CM0.h"

uint32_t MDU_UnsignedMultiplyFun(uint16_t M1, uint16_t M2);
uint32_t MDU_SignedMultiplyFun(uint16_t M1, uint16_t M2);
uint32_t MDU_LeftSignedMultiplyFun(uint16_t M1, uint16_t M2);
int16_t  MDU_LpfFun(int16_t a, int16_t b,int16_t k);
uint32_t MDU_UnsignedDivFun(uint32_t dividend, uint16_t divisor);
void  MDU_ParkFun(int16_t X0, int16_t Y0,uint16_t theta);
void  MDU_ArctanFun(int16_t X0, int16_t Y0);



#endif
