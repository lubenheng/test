/****************************************************************************/
/** \file hwmath.c
**
**	History:
**	
*****************************************************************************/
/****************************************************************************/
/*	include files
*****************************************************************************/

//#include "stdint.h"


#include "platform_common.h"
#include "HWMath_Reg.h"
#include "device_resource.h"
#include "HWMath_Type.h"
#include "Math_hal.h"
#include "HW_Math.h"
/****************************************************************************/
/*	Local pre-processor symbols/macros('#define')
*****************************************************************************/

/****************************************************************************/
/*	Global variable definitions(declared in header file with 'extern')
*****************************************************************************/

/****************************************************************************/
/*	Local type definitions('typedef')
*****************************************************************************/

/****************************************************************************/
/*	Local variable  definitions('static')
*****************************************************************************/


/****************************************************************************/
/*	Local function prototypes('static')
*****************************************************************************/

/****************************************************************************/
/*	Function implementation - global ('extern') and local('static')
*****************************************************************************/


int32_t HWMATH_DIV_DIV(int32_t x,int32_t y) 
{
  HWMATH->DIVIDE_STR.DIVIDE = x;
  HWMATH->DIVIDS_STR.DIVIDS = y;
  __NOP();

  return HWMATH->QUOTI_STR.QUOTI;
}

int32_t HWMATH_DIV_MOD(int32_t x,int32_t y) 
{
  HWMATH->DIVIDE_STR.DIVIDE = x;
  HWMATH->DIVIDS_STR.DIVIDS = y;
//  while(!HWMATH->IR_STR.DIVI_RDY)
//	{
//		;
//	}
	__NOP();
  return HWMATH->REMAIN_STR.REMAIN;
}


uint32_t HWMATH_SQRT(uint32_t x) 
{
  HWMATH->SQRTRI_STR.SQRTIN = x;
  while(!HWMATH->IR_STR.SQRT_RDY)
	{
		;
	}

  return HWMATH->SQRTRO_STR.SQRTOUT;
}

void HWMATH_SINCOS_Cal(int16_t x,Math_SinCos_Type *y) 
{
   HWMATH->THETA_STR.ANGLE = x;

  __NOP();

  y-> u32_SinCos = HWMATH->SINCOS_STR.Word;

}

int16_t HWMATH_SINCOS_Sin(int16_t x) 
{
   HWMATH->THETA_STR.ANGLE = x;

  __NOP();


  return (HWMATH->SINCOS_STR.SIN);
}

int16_t HWMATH_SINCOS_Cos(int16_t x) 
{
   HWMATH->THETA_STR.ANGLE = x;

  __NOP();


  return (HWMATH->SINCOS_STR.COS);
}




/******************************** END OF FILE *******************************/

