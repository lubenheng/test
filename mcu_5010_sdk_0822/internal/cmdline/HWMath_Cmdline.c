#include "platform_common.h"


#include "cmdline_internal.h"
#include "HWMath_Cmdline.h"
#include "HwMath_test.h"
#include "HWMath_Type.h"
#include "HW_Math.h"
#include "math.h"

#define PI 3.14159265358979323846 

#define ABSFUN(in) ((in)>0 ? in : -in)

s32 Abs_Func_de(s32 data)
{
	return (data>0 ? data : -data);
}
/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : 
* Description    : 
* Function Call  : 
* Input Paragram :
Divd
Div

* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
int cmd_hwmath_test_div(int argc, char *argv[])
{
	
	if (argc < 2U)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);
		return 0;
	}
	s32 Divd,Div,Quot,Quto_Std;
	Divd = get_arg_long(argv[1]);
	Div = get_arg_long(argv[2]);
		
	if(Div == 0)
	{
		INFO("The divisor cannot be zero (¡Ñ_¨‘)! \n");
		return 0;
	}
	
	Quot = HWMATH_DIV_DIV(Divd,Div);
	INFO(" Calculation result :%d\n",Quot);
	Quto_Std = Divd/Div;
	INFO("Standard Calculation result :%d\n",Quto_Std);
	
	if(Quot != Quto_Std)
	{
		INFO(" DIV Test Fail (¨i©n¨i)\n");
	}
	else
	{
		INFO(" DIV Test Pass (^¨Œ^)\n");
	}
	return 0;
}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : 
* Description    : 
* Function Call  : 
* Input Paragram :
Divd
Div

* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
int cmd_hwmath_test_mod(int argc, char *argv[])
{
	if (argc < 2U)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);
		return 0;
	}
	s32 Divd,Div,Remain,Remain_Std;
	Divd = get_arg_long(argv[1]);
	Div = get_arg_long(argv[2]);
		
	if(Div == 0)
	{
		INFO("The divisor cannot be zero (¡Ñ_¨‘)! \n");
		return 0;
	}
	
	Remain = HWMATH_DIV_MOD(Divd,Div);
	INFO(" Calculation result :%d\n",Remain);
	Remain_Std = Divd%Div;
	INFO("Standard Calculation result :%d\n",Remain_Std);
	
	if(Remain != Remain_Std)
	{
		INFO(" Mod Test Fail (¨i©n¨i)\n");
	}
	else
	{
		INFO(" Mod Test Pass (^¨Œ^)\n");
	}
	
	return 0;
}
/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : 
* Description    : 
* Function Call  : 
* Input Paragram :
radicand

* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
int cmd_hwmath_test_sqrt(int argc, char *argv[])
{
	if (argc > 2U)
	{
		INFO("(¡Ñ_¨‘) wrong cmd_format:%s\n", argv[0]);
		return 0;
	}
	u32 Square,SqrtCal,SqrtCal_Std;
	
	Square = get_arg_long(argv[1]);
		
//	if(Square < 0)
//	{
//		INFO("Radicand cannot be negative (¡Ñ_¨‘)!\n");
//		return 0;
//	}
	
	SqrtCal = HWMATH_SQRT(Square);
	INFO(" Calculation result :%d\n",SqrtCal);
	SqrtCal_Std = sqrt(Square);
	INFO("Standard Calculation result :%d\n",SqrtCal_Std);
	
	if(SqrtCal != SqrtCal_Std)
	{
		INFO(" SQRT Test Fail (¨i©n¨i)\n");
	}
	else
	{
		INFO(" SQRT Test Pass (^¨Œ^)\n");
	}

	return 0;
}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : 
* Description    : 
* Function Call  : 
* Input Paragram :
Angle

* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
int cmd_hwmath_test_sincos(int argc, char *argv[])
{
	if (argc > 2U)
	{
		INFO("(¡Ñ_¨‘) wrong cmd_format:%s\n", argv[0]);
		return 0;
	}
	s32 Angle,Sin_std,Cos_std;
	
	Angle = get_arg_long(argv[1]);
		
	Math_SinCos_Type SinCos_Cal;
	
	HWMATH_SINCOS_Cal(Angle,&SinCos_Cal);
	
	INFO(" Calculation Sin :%d\n",SinCos_Cal.Sin);
	INFO(" Calculation Cos :%d\n",SinCos_Cal.Cos);
	
	Sin_std = 32767 * sin((double)Angle*2* PI /65535.0);
	Cos_std = 32767 * cos((double)Angle*2* PI /65535.0);	
	
	INFO("Standard Calculation Sin :%d\n",Sin_std);
	INFO("Standard Calculation Cos :%d\n",Cos_std);
	
	if( ABSFUN((int)(Sin_std - SinCos_Cal.Sin) ) > 2 )
	{
		INFO(" Sin Test Fail (¨i©n¨i)\n");
	}
	else
	{
		INFO(" Sin Test Pass (^¨Œ^)\n");
	}

	if( ABSFUN((int)(Cos_std - SinCos_Cal.Cos) ) > 2 )
	{
		INFO(" Cos Test Fail (¨i©n¨i)\n");
	}
	else
	{
		INFO(" Cos Test Pass (^¨Œ^)\n");
	}
	return 0;
}
int cmd_hwmath_test_sin(int argc, char *argv[])
{



	return 0;
}
int cmd_hwmath_test_cos(int argc, char *argv[])
{

	return 0;
}

// err·¶Î§²âÊÔ
	int32_t Angle_Test = 0,Angle_Test_1 = 0,Angle_Test_2 = 65535;
	uint32_t Val_SqrtOut,Val_SqrtIn,Sqrt_Std;
	s32 SinOut,CosOut,DivOut,Remain;
	s32 Cos_std,Sin_std,Div_Std,Remain_Std;
	s32 Sin_Err,Cos_Err;
	s32 Sin_Err_Max = 0,Cos_Err_Max= 0,DivErr_Max= 0,RemianErr_Max= 0,SqrtErr_Max= 0;	
int cmd_hwmath_test_Err(int argc, char *argv[])
{
	if (argc > 2U)
	{
		INFO("(¡Ñ_¨‘) wrong cmd_format:%s\n", argv[0]);
		return 0;
	}
	if(get_arg_long(argv[1]) == 1)
	{
		Angle_Test_1 = 0;
		Angle_Test_2 = 65535;
		while(Angle_Test_1 < 65536)
		{
			if(Angle_Test_1 %2 == 1)
			{
				Angle_Test = Angle_Test_1;
			}
			else
			{
				Angle_Test = Angle_Test_2;
			}
			// SinCos
			Math_SinCos_Type SinCos_Cal;
			HWMATH_SINCOS_Cal(Angle_Test,&SinCos_Cal);
			CosOut = (s32)SinCos_Cal.Cos;
			SinOut = (s32)SinCos_Cal.Sin;
			
			Sin_std = 32767 * sin((double)Angle_Test*2* 3.1415926 /65535.0);
			Cos_std = 32767 * cos((double)Angle_Test*2* 3.1415926 /65535.0);				
			
			Sin_Err = Sin_std - SinOut;
			Cos_Err = Cos_std - CosOut;
			if(Sin_Err_Max < Abs_Func_de(Sin_Err))					Sin_Err_Max = Abs_Func_de(Sin_Err);
			if(Cos_Err_Max < Abs_Func_de(Cos_Err))					Cos_Err_Max = Abs_Func_de(Cos_Err);		
			
			// DIV
			if(Angle_Test == 0)
				Angle_Test +=1;
			
			DivOut = HWMATH_DIV_DIV(Angle_Test*SinOut  ,Angle_Test);
			Div_Std = Angle_Test*SinOut / Angle_Test;
			if(DivErr_Max < Abs_Func_de(Div_Std - DivOut))					DivErr_Max = Abs_Func_de(Div_Std - DivOut);		
			
			Remain = HWMATH_DIV_MOD(CosOut*SinOut,Angle_Test);
			Remain_Std = CosOut * SinOut % Angle_Test;
			if(RemianErr_Max < Abs_Func_de(Remain - Remain_Std))					RemianErr_Max = Abs_Func_de(Remain - Remain_Std);	
			
			
			// Sqrt 
			Val_SqrtIn = CosOut * CosOut *2 ;
			Val_SqrtOut = HWMATH_SQRT(Val_SqrtIn);
			Sqrt_Std =  sqrt(Val_SqrtIn);
			if(SqrtErr_Max < Abs_Func_de(Val_SqrtOut - Sqrt_Std))					SqrtErr_Max = Abs_Func_de(Val_SqrtOut - Sqrt_Std);
			
			
			Angle_Test_1 +=1;
			Angle_Test_2 -=1;
		}
		

		INFO(" Sin calculate Error Max :%d\n",Sin_Err_Max);
		INFO(" Cos calculate Error Max :%d\n",Cos_Err_Max);	
		INFO(" DIV calculate Error Max :%d\n",DivErr_Max);		
		INFO(" Mod calculate Error Max :%d\n",RemianErr_Max);		
		INFO(" Sqrt calculate Error Max :%d\n",SqrtErr_Max);	
		INFO(" Error of HwMath calculation has been done (^¨Œ^)\n"); 
	}
	else
	{
		INFO("(¡Ñ_¨‘)! wrong cmd_Para:%s\n", argv[0]);
	}

	return 0;	
	
}


