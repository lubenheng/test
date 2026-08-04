#include "platform_common.h"

#include "test.h"

#include "HWMath_Reg.h"

#include "HWMath_Type.h"
#include "HW_Math.h"
#include "math.h"
#include "HwMath_test.h"


//uint16_t Angle_Test;
//uint32_t Val_SqrtOut,Val_SqrtIn;
//s32 SinOut,CosOut,Quot,Quot_Std;
//s32 Cos_std,Sin_std,Sin_Err,Cos_Err;
//void HW_Math_All_Function_Test(void)
//{
//  Angle_Test +=1;

//  // SinCos
//  Math_SinCos_Type SinCos_Cal;
//  HWMATH_SINCOS_Cal(Angle_Test,&SinCos_Cal);
//  CosOut = SinCos_Cal.Cos;
//  SinOut = SinCos_Cal.Sin;
//  
//  Sin_std = 32767 * sin((double)Angle_Test*2* 3.1415926 /65535.0);
//  Cos_std = 32767 * cos((double)Angle_Test*2* 3.1415926 /65535.0);				
//  
//  Sin_Err = Sin_std - SinOut;
//  Cos_Err = Cos_std - CosOut;
//  
//  // DIV
//  if(Angle_Test == 0)
//    Angle_Test +=1;
//  Quot = HWMATH_DIV_DIV(Angle_Test*SinOut  ,Angle_Test);
//  Quot_Std = HWMATH_DIV_DIV(Angle_Test*CosOut  ,Angle_Test);

//  // Sqrt
//  Val_SqrtIn = CosOut * CosOut *2 ;
// // Val_SqrtIn = (u32)Angle_Test * Angle_Test ;
//  Val_SqrtOut = HWMATH_SQRT(Val_SqrtIn);


//}

