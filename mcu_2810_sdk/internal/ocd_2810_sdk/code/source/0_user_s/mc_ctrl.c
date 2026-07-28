//==========================================================================//
/*****************************************************************************
*-----------------------------------------------------------------------------
* @file    motro_control.c
* @author  CMS Motor Control Team
* @version 第二代风机平台
* @date    2023年2月
* @brief   
*---------------------------------------------------------------------------//
*****************************************************************************/
//==========================================================================//

//---------------------------------------------------------------------------/
//	include files
//---------------------------------------------------------------------------/
#include "mcu_driver.h"
#include "motor_driver.h"
#include "user.h"

//---------------------------------------------------------------------------/
//	Local pre-processor symbols/macros('#define')
//---------------------------------------------------------------------------/
#define Spd_PwTable_Lens			(12)
//---------------------------------------------------------------------------/
//	Local variable  definitions
//---------------------------------------------------------------------------/

volatile SystStates 		SYSTEM_STATE 			= SYS_INIT;		/* 系统流程状态机 */

volatile MotorStates 		MOTOR_STATE 			= MC_INIT;		/* 电机控制流程状态机 */

volatile Union_Fault		u_Fault_Flag 			= {0};				/* 电机故障指示标志 */

volatile FaultCode			MOTOR_FAULT_CODE  = NOERROR;		/* 电机故障指示标志 */

struct_Basic_Para				Basic 						= {0};				/* 基本参数结构体 */

struct_Falg 						Flag 							= {0};				/* 标志位结构体 */

//struct_Time    					Time 							= {0}; 				/* 时间计数结构体 */

//struct_Count    				Count 						= {0}; 	 			/* 数学计数结构体 */

//struct_MotCtrl_Para			Para 							= {0};				/* 电机控制参数结构体 */

struct_Config						Config						= {0};				/* 系统配置选项结构体 */

//struct_Base_Value				stru_base_value		= {0};				/* 电机基值参数结构体 */	

//struct_ADC_Sample 		 	stru_Sample				= {0};				/* 采样参数结构体 */

struct_FOC_Ctrl					stru_FOC					= {0};				/* FOC控制参数结构体 */

//struct_SpeedRamp				stru_Speed_Ctrl		= {0};				/* 速度控制结构体 */

//struct_MotorRuning      		Motor_Actual      = {0};				/* 电机实际反馈参数 */

//struct_OutLoop 				stru_OutLoop			= {0};

//struct_Smo_Start				stru_SmoStart			= {0};

//struct_BLDCStart_Para			stru_BLDCStart		= {0};

//struct_FaultProtect			stru_Fault				= {0};				/* 故障参数结构体 */

struct_FOC_Start				stru_FOC_Start 	= 	{0};						/* 启动参数结构体 */
struct_Para						stru_Para 	= 	{0};

extern struct_smo_est           stru_smo_est;

//---------------------------------------------------------------------------/
//	Global variable definitions(declared in header file with 'extern')
//---------------------------------------------------------------------------/


#define Length_Table_TempPU		(37)								// 温度查表表格长度
#define	Res_PULLUP						(10)								// 上拉电阻阻值		kΩ						
#define Res2TempPU(R)					( 5 * R * 4096 * 8 /(R + Res_PULLUP)/HW_ADC_REF)	 // 
//const s32 Table_TempPU[Length_Table_TempPU][2] =
//{

//};
//---------------------------------------------------------------------------/
//	Local function prototypes('static')
//---------------------------------------------------------------------------/
/****************************************************************************/
/*	Local variable  definitions
*****************************************************************************/
const int16_t Sin_Cos_Table[256] =
{
0x0000,0x00C9,0x0192,0x025B,0x0324,0x03ED,0x04B6,0x057F,
0x0648,0x0711,0x07D9,0x08A2,0x096A,0x0A33,0x0AFB,0x0BC4,
0x0C8C,0x0D54,0x0E1C,0x0EE3,0x0FAB,0x1072,0x113A,0x1201,
0x12C8,0x138F,0x1455,0x151C,0x15E2,0x16A8,0x176E,0x1833,
0x18F9,0x19BE,0x1A82,0x1B47,0x1C0B,0x1CCF,0x1D93,0x1E57,
0x1F1A,0x1FDD,0x209F,0x2161,0x2223,0x22E5,0x23A6,0x2467,
0x2528,0x25E8,0x26A8,0x2767,0x2826,0x28E5,0x29A3,0x2A61,
0x2B1F,0x2BDC,0x2C99,0x2D55,0x2E11,0x2ECC,0x2F87,0x3041,
0x30FB,0x31B5,0x326E,0x3326,0x33DF,0x3496,0x354D,0x3604,
0x36BA,0x376F,0x3824,0x38D9,0x398C,0x3A40,0x3AF2,0x3BA5,
0x3C56,0x3D07,0x3DB8,0x3E68,0x3F17,0x3FC5,0x4073,0x4121,
0x41CE,0x427A,0x4325,0x43D0,0x447A,0x4524,0x45CD,0x4675,
0x471C,0x47C3,0x4869,0x490F,0x49B4,0x4A58,0x4AFB,0x4B9D,
0x4C3F,0x4CE0,0x4D81,0x4E20,0x4EBF,0x4F5D,0x4FFB,0x5097,
0x5133,0x51CE,0x5268,0x5302,0x539B,0x5432,0x54C9,0x5560,
0x55F5,0x568A,0x571D,0x57B0,0x5842,0x58D3,0x5964,0x59F3,
0x5A82,0x5B0F,0x5B9C,0x5C28,0x5CB3,0x5D3E,0x5DC7,0x5E4F,
0x5ED7,0x5F5D,0x5FE3,0x6068,0x60EB,0x616E,0x61F0,0x6271,
0x62F1,0x6370,0x63EE,0x646C,0x64E8,0x6563,0x65DD,0x6656,
0x66CF,0x6746,0x67BC,0x6832,0x68A6,0x6919,0x698B,0x69FD,
0x6A6D,0x6ADC,0x6B4A,0x6BB7,0x6C23,0x6C8E,0x6CF8,0x6D61,
0x6DC9,0x6E30,0x6E96,0x6EFB,0x6F5E,0x6FC1,0x7022,0x7083,
0x70E2,0x7140,0x719D,0x71F9,0x7254,0x72AE,0x7307,0x735E,
0x73B5,0x740A,0x745F,0x74B2,0x7504,0x7555,0x75A5,0x75F3,
0x7641,0x768D,0x76D8,0x7722,0x776B,0x77B3,0x77FA,0x783F,
0x7884,0x78C7,0x7909,0x794A,0x7989,0x79C8,0x7A05,0x7A41,
0x7A7C,0x7AB6,0x7AEE,0x7B26,0x7B5C,0x7B91,0x7BC5,0x7BF8,
0x7C29,0x7C59,0x7C88,0x7CB6,0x7CE3,0x7D0E,0x7D39,0x7D62,
0x7D89,0x7DB0,0x7DD5,0x7DFA,0x7E1D,0x7E3E,0x7E5F,0x7E7E,
0x7E9C,0x7EB9,0x7ED5,0x7EEF,0x7F09,0x7F21,0x7F37,0x7F4D,
0x7F61,0x7F74,0x7F86,0x7F97,0x7FA6,0x7FB4,0x7FC1,0x7FCD,
0x7FD8,0x7FE1,0x7FE9,0x7FF0,0x7FF5,0x7FF9,0x7FFD,0x7FFE
};

const uint16_t Q15Sqrt_Table[677] =
{
1448,  2048,  2508,  2896,  3238,  3547,  3831,  4096,  4344,  4579,  4802,  5016,
5221,  5418,  5608,  5792,  5970,  6144,  6312,  6476,  6636,  6792,  6945,  7094,
7240,  7384,  7524,  7662,  7798,  7931,  8062,  8192,  8319,  8444,  8567,  8688,
8808,  8927,  9043,  9158,  9272,  9385,  9496,  9605,  9714,  9821,  9928,  10033,
10137,  10240,  10341,  10442,  10542,  10641,  10739,  10836,  10933,  11028,  11123,  11217,
11310,  11402,  11494,  11585,  11675,  11764,  11853,  11941,  12029,  12116,  12202,  12288,
12373,  12457,  12541,  12624,  12707,  12789,  12871,  12952,  13033,  13113,  13193,  13272,
13351,  13429,  13507,  13584,  13661,  13738,  13814,  13890,  13965,  14040,  14114,  14188,
14262,  14336,  14408,  14481,  14553,  14625,  14697,  14768,  14839,  14909,  14979,  15049,
15119,  15188,  15257,  15325,  15394,  15462,  15529,  15597,  15664,  15730,  15797,  15863,
15929,  15995,  16060,  16125,  16190,  16255,  16319,  16384,  16447,  16511,  16574,  16638,
16700,  16763,  16826,  16888,  16950,  17011,  17073,  17134,  17195,  17256,  17317,  17377,
17438,  17498,  17557,  17617,  17676,  17736,  17795,  17854,  17912,  17971,  18029,  18087,
18145,  18203,  18260,  18317,  18375,  18432,  18488,  18545,  18601,  18658,  18714,  18770,
18826,  18881,  18937,  18992,  19047,  19102,  19157,  19211,  19266,  19320,  19374,  19429,
19482,  19536,  19590,  19643,  19697,  19750,  19803,  19856,  19908,  19961,  20013,  20066,
20118,  20170,  20222,  20274,  20325,  20377,  20428,  20480,  20531,  20582,  20633,  20683,
20734,  20784,  20835,  20885,  20935,  20985,  21035,  21085,  21135,  21184,  21234,  21283,
21332,  21381,  21430,  21479,  21528,  21577,  21625,  21673,  21722,  21770,  21818,  21866,
21914,  21962,  22010,  22057,  22105,  22152,  22199,  22246,  22294,  22341,  22387,  22434,
22481,  22528,  22574,  22620,  22667,  22713,  22759,  22805,  22851,  22897,  22943,  22988,
23034,  23079,  23125,  23170,  23215,  23260,  23305,  23350,  23395,  23440,  23485,  23529,
23574,  23618,  23663,  23707,  23751,  23795,  23839,  23883,  23927,  23971,  24014,  24058,
24102,  24145,  24188,  24232,  24275,  24318,  24361,  24404,  24447,  24490,  24533,  24576,
24618,  24661,  24703,  24746,  24788,  24830,  24872,  24914,  24957,  24999,  25040,  25082,
25124,  25166,  25207,  25249,  25290,  25332,  25373,  25415,  25456,  25497,  25538,  25579,
25620,  25661,  25702,  25742,  25783,  25824,  25864,  25905,  25945,  25986,  26026,  26066,
26106,  26147,  26187,  26227,  26267,  26307,  26346,  26386,  26426,  26465,  26505,  26545,
26584,  26624,  26663,  26702,  26741,  26781,  26820,  26859,  26898,  26937,  26976,  27014,
27053,  27092,  27131,  27169,  27208,  27246,  27285,  27323,  27362,  27400,  27438,  27476,
27514,  27553,  27591,  27629,  27666,  27704,  27742,  27780,  27818,  27855,  27893,  27930,
27968,  28005,  28043,  28080,  28118,  28155,  28192,  28229,  28266,  28303,  28340,  28377,
28414,  28451,  28488,  28525,  28562,  28598,  28635,  28672,  28708,  28745,  28781,  28817,
28854,  28890,  28926,  28963,  28999,  29035,  29071,  29107,  29143,  29179,  29215,  29251,
29287,  29322,  29358,  29394,  29429,  29465,  29501,  29536,  29572,  29607,  29642,  29678,
29713,  29748,  29784,  29819,  29854,  29889,  29924,  29959,  29994,  30029,  30064,  30099,
30134,  30168,  30203,  30238,  30273,  30307,  30342,  30376,  30411,  30445,  30480,  30514,
30548,  30583,  30617,  30651,  30685,  30720,  30754,  30788,  30822,  30856,  30890,  30924,
30958,  30991,  31025,  31059,  31093,  31126,  31160,  31194,  31227,  31261,  31294,  31328,
31361,  31395,  31428,  31461,  31495,  31528,  31561,  31595,  31628,  31661,  31694,  31727,
31760,  31793,  31826,  31859,  31892,  31925,  31957,  31990,  32023,  32056,  32088,  32121,
32154,  32186,  32219,  32251,  32284,  32316,  32349,  32381,  32414,  32446,  32478,  32510,
32543,  32575,  32607,  32639,  32671,  32703,  32735,  32768,  32799,  32831,  32863,  32895,
32927,  32959,  32991,  33023,  33054,  33086,  33118,  33149,  33181,  33212,  33244,  33276,
33307,  33339,  33370,  33401,  33433,  33464,  33495,  33527,  33558,  33589,  33620,  33652,
33683,  33714,  33745,  33776,  33807,  33838,  33869,  33900,  33931,  33962,  33993,  34023,
34054,  34085,  34116,  34146,  34177,  34208,  34238,  34269,  34300,  34330,  34361,  34391,
34422,  34452,  34483,  34513,  34543,  34574,  34604,  34634,  34665,  34695,  34725,  34755,
34785,  34816,  34846,  34876,  34906,  34936,  34966,  34996,  35026,  35056,  35086,  35115,
35145,  35175,  35205,  35235,  35264,  35294,  35324,  35353,  35383,  35413,  35442,  35472,
35501,  35531,  35560,  35590,  35619,  35649,  35678,  35708,  35737,  35766,  35796,  35825,
35854,  35883,  35913,  35942,  35971,  36000,  36029,  36058,  36087,  36116,  36145,  36174,
36203,  36232,  36261,  36290,  36319,  36348,  36377,  36406,  36434,  36463,  36492,  36521,
36549,  36578,  36607,  36635,  36664,  36692,  36721,  36750,  36778,  36807,  36835,  36864,
36892,  36920,  36949,  36977,  37005,  37034,  37062,  37090,  37119,  37147,  37175,  37203,
37231,  37260,  37288,  37316,  37344,  37372,  37400,  37428,  37456,  37484,  37512,  37540,
37568,  37596,  37624,  37652
};

/*****************************************************************************
* Function Name  : FOC_SinCos_Calc
* Description    : 三角函数计算
* Function Call  : 
* Input Paragram : 
* Return Value   : none
* note           : 
* Version        : V0.1    2025/04/13    		
******************************************************************************/
struct_SinCos  FOC_SinCos_Calc(int32_t Angel)
{
	uint16_t hIndex;
	struct_SinCos SinCos_Output;
     
	hIndex = (uint16_t)(Angel + 32800) >> 6;
	
	switch (hIndex & 0x0300) 
	{
		/* 0~90度 */
		case 0x0200:
		SinCos_Output.Sin = Sin_Cos_Table[(uint8_t)(hIndex)];
		SinCos_Output.Cos = Sin_Cos_Table[(uint8_t)(0xFF - (uint8_t)(hIndex))];
		break;
		
		/* 90~180度 */
		case 0x0300:  
		SinCos_Output.Sin = Sin_Cos_Table[(uint8_t)(0xFF - (uint8_t)(hIndex))];
		SinCos_Output.Cos = -Sin_Cos_Table[(uint8_t)(hIndex)];
		break;
		
		/* 180~270度 */
		case 0x0000:
		SinCos_Output.Sin = -Sin_Cos_Table[(uint8_t)(hIndex)];
		SinCos_Output.Cos = -Sin_Cos_Table[(uint8_t)(0xFF - (uint8_t)(hIndex))];
		break;
		
		/* 270~360度 */
		case 0x0100:
		SinCos_Output.Sin =  -Sin_Cos_Table[(uint8_t)(0xFF - (uint8_t)(hIndex))];
		SinCos_Output.Cos =  Sin_Cos_Table[(uint8_t)(hIndex)]; 
		break;

		default:
		break;
	}
	
	return(SinCos_Output);
	
}

//===========================================================================/
//***** definitions  end ****************************************************/
//===========================================================================/



/*****************************************************************************
* Function Name  : Function_PI_Cal
* Description    : PI控制器
* Function Call  : 
* Input Paragram : 
* Return Value   : none
* note           : 
* Version        : V0.1    2025/04/13    新建		Lsy
******************************************************************************/

//struct_PI			stru_PI_SpdLoop ={.Kp = 32787,.Ki = 320};
struct_PI			stru_PI_SpdLoop ={ 
FOC_WKP, 
FOC_WKI,
0,
0,
Phy2Pu_Fun_Iphase(MC_Start_Curr),
0,
Phy2Pu_Fun_Iphase(3)
};
void Function_PI_Cal(struct_PI *PI,s32 Ref, s32 Feedbk)
{
	s32 outbuf;	
	PI->Error = Ref - Feedbk;
	PI->Integral += (PI->Ki >> 3) * PI->Error ;

	if(PI->Integral > (PI->Output_Max<<12) )		PI->Integral = PI->Output_Max<<12 ;
	if(PI->Integral < PI->Output_Min<<12 )		PI->Integral = PI->Output_Min<<12 ;
	

	outbuf = ((PI->Kp >> 3) * PI->Error >> 12) + (PI->Integral >> 12) ;
	if(outbuf > PI->Output_Max )		outbuf = PI->Output_Max ;
	if(outbuf < PI->Output_Min )		outbuf = PI->Output_Min ;	
	PI->Out = outbuf;
}



/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : User_Control
* Description    : 
* Function Call  : 
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/

//===========================================================================/
// 电机运行使能 
void User_Motor_On(void)
{
	//Flag.MC_RunStop = 1;
	
}

//===========================================================================/
// 电机停止 
void User_Motor_Off(void)
{
	//Flag.MC_RunStop = 0;
	
}

//===========================================================================/
// 电机方向控制 
void User_Motor_FRControl(uint8_t Input_Val)
{
	if( Input_Val == FR_CW )
		Config.MotorDir = FR_CW;
	else
		Config.MotorDir = FR_CCW;
	
}

//===========================================================================/
// 调速控制 
void User_MotorSpeed_Set(int32_t Input_Val)
{

}

//===========================================================================/
// 关闭所有功能模块 
void System_Modules_off(void)
{

	
}



//===========================================================================//

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : System_Control
* Description    : 系统状态机流程控制
* Function Call  : 主循环调用
* Input Paragram : 
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void System_Control(void)
{

}




/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : MC_MotorSpeed_Calc
* Description    : 电机转速计算
* Function Call  : 
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void MC_MotorSpeed_Calc(void)
{
	// 滤波前计算速度
	//g_smo_speed = (FOC->ro_fe3 >> 16) * MOTOR_SPEED_MAX >> 15;

	// 短滤波计算速度
	//g_smo_speed_short = (FOC->ro_fe2 & 0xffff) * MOTOR_SPEED_MAX >> 15;

		// 计算电机转速 
	Basic.Mech_Speed = (FOC->ro_fe2 & 0xffff) * MOTOR_SPEED_MAX >> 15;

	// 长滤波计算速度
	//g_smo_speed_long = (FOC->ro_fe2 >> 16) * MOTOR_SPEED_MAX >> 15;

}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : MC_Clear_FaultMessage
* Description    : 清除故障信息
* Function Call  : 
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void MC_Clear_FaultMessage(void)
{

	
}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : ADC0_CALB
* Description    : ADC0通道采样
* Function Call  : 
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void ADC_Sample(void)
{

}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : SysTick_1ms
* Description    : 1ms定时任务
* Function Call  : 
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
//---------------------------------------------------------------------------/
void SysTick_1ms(void)
{

	
}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : MC_Ramp_Control
* Description    : 
* Function Call  : 
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void MC_Ramp_Control(struct_SpeedRamp *Ramp, int32_t Ref)
{
	
}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : Speed_Loop_Ctrl
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void Speed_Loop_Ctrl(void)
{
	// 	// Ibus 限制值计算
	// #if (LIMIT_IBUS_ENABLE &&(!LIMIT_POWER_ENABLE))
	
	// stru_OutLoop.Ibus_Target = stru_OutLoop.Ibus_LimMax_Pu;
	// #endif
	
	// #if (LIMIT_POWER_ENABLE &&(!LIMIT_IBUS_ENABLE))
	
	// stru_OutLoop.Ibus_PwMax_Pu = stru_OutLoop.Pw_LimMax_Pu *_Q15_VAL / Basic.Vbus_Filt;
	
	// stru_OutLoop.Ibus_Target = stru_OutLoop.Ibus_PwMax_Pu;
	// #endif	
	
	// #if (LIMIT_IBUS_ENABLE && LIMIT_POWER_ENABLE )
	
	// stru_OutLoop.Ibus_PwMax_Pu = stru_OutLoop.Pw_LimMax_Pu *_Q15_VAL / Basic.Vbus_Filt;
	
	// stru_OutLoop.Ibus_Target = (stru_OutLoop.Ibus_PwMax_Pu < stru_OutLoop.Ibus_LimMax_Pu) \
	// 													? stru_OutLoop.Ibus_PwMax_Pu : stru_OutLoop.Ibus_LimMax_Pu;
	
	// #endif
	
	// #if (LIMIT_IBUS_ENABLE || LIMIT_POWER_ENABLE)
	// {
	// 	/*--------------------------限制判断--------------------------------*/
	// 	if((Basic.Ibus > stru_OutLoop.Ibus_Target + 100) && (stru_OutLoop.LimitFlag !=2))
	// 		stru_OutLoop.LimDelayCnt ++;

	// 	if(stru_OutLoop.LimDelayCnt > 20)
	// 	{
	// 		stru_OutLoop.QuitDelayCnt = 0;
	// 		stru_OutLoop.LimDelayCnt = 0;
	// 		stru_OutLoop.LimitFlag = 2;
			
	// 		PID_WL.Kp = 16384; //切换功率环PI
	// 		PID_WL.Ki = 200;			
	// 		stru_FOC.WRAMP =	TIME_POWER_LOOP;	
	// 	}
		
	// 	/*---------------------------退出限制判断-------------------------------*/
	// 	if((Basic.Calb_Speed > stru_Speed_Ctrl.RampOut + 200 ) && (stru_OutLoop.LimitFlag ==2))
	// 		stru_OutLoop.QuitDelayCnt ++;
		
	// 	if(stru_OutLoop.QuitDelayCnt > 30)
	// 	{
	// 		stru_OutLoop.QuitDelayCnt = 0;
	// 		stru_OutLoop.LimDelayCnt = 0;
	// 		stru_OutLoop.LimitFlag = 0;
			
	// 		PID_WL.Kp = Para.FOC.Wkp; //切换速度环PI
	// 		PID_WL.Ki = Para.FOC.Wki;			
	// 		stru_FOC.WRAMP =	TIME_SPEED_LOOP;
	// 	}		
	// }
	// #endif
	// 	// 外环控制
	// if(stru_OutLoop.LimitFlag == 2)
	// {			
	// 	// 恒功率控制
	// 	stru_FOC.Curr_Is_Ref = PID_CALC(&PID_WL, stru_OutLoop.Ibus_Target, Basic.Ibus);		
	// }
	// else
	// {			
	// 	// 恒转速控制
	// 	stru_FOC.Curr_Is_Ref = PID_CALC(&PID_WL, stru_Speed_Ctrl.RampOut, Basic.Calb_Speed);	
	// }


}
/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : Power_Loop_Ctrl
* Description    : 功率环/母线电流环控制
* Function Call  : 
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void Power_Loop_Ctrl(void)
{

}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : MC_Speed_Control
* Description    : 速度环 / 功率环 功能
* Function Call  : 
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void  MC_Speed_Control(void)
{
	// //=========================================================================//	
	// // 硬件功率计算
	// Basic.Power = FRACMPY_Q15( Basic.Vbus_Filt, Basic.Ibus );
	// //=========================================================================//	
	// if(MOTOR_STATE == MC_RUN)
	// {
	// 	//=======================================================================//
	// 	//Config.Mode_Svpwm = Config_SVPWM_Mode;
	
	// 	//=========================================================================//
						
	// 	// 调速控制模式，恒转矩/恒转速/恒功率 
	// 	if( ++stru_FOC.W_Cycle >= stru_FOC.WRAMP )
	// 	{
	// 		stru_FOC.W_Cycle = 0;		
	// 		// 调速信号爬坡处理 
	// 		MC_Ramp_Control( &stru_Speed_Ctrl, stru_FOC.Speed_Ref );		
	// 		//=======================================================================//	
	// 		// 力矩环控制 
	// 		#if ( Config_Contorl_Mode == Current_Loop )
	// 		{
	// 			Torque_Loop_Ctrl();
	// 		}
	// 		//=======================================================================//	
	// 		// 转速环控制 
	// 		#elif ( Config_Contorl_Mode == Speed_Loop )
	// 		{
	// 			Speed_Loop_Ctrl();
	// 		}
	// 		//===========================================================================//	
	// 		// 功率环控制 
	// 		#elif ( Config_Contorl_Mode == Power_Loop )
	// 		{
	// 			Power_Loop_Ctrl();
	// 		}
	// 		#endif //end torgue and speed or power loop
	// 		//=======================================================================//		
	// 		// 限制IQ电流输出
	// 		if( stru_FOC.Curr_Is_Ref > stru_FOC.Curr_Iq_Max )	stru_FOC.Curr_Is_Ref = stru_FOC.Curr_Iq_Max;
	// 		if( stru_FOC.Curr_Is_Ref < stru_FOC.Curr_Iq_Min )	stru_FOC.Curr_Is_Ref = stru_FOC.Curr_Iq_Min;
	// 	}
	// 	//===========================================================================//	
	// 	// 弱磁
	// 	#if ( Weaken_Disable == Config_Weak_Mode )
	// 	{
	// 		stru_Curr_dq_ref.Id = 0;
	// 		stru_Curr_dq_ref.Iq = stru_FOC.Curr_Is_Ref;
	// 	}
	// 	#else
	// 	{
	// 		FOC_Control_Weakening();	
	// 	}
	// 	#endif
	// 	//===========================================================================//	
	// 	// 补偿角处理		
	// 	if 	(stru_FOC.Offset_Angle < Para.FOC.OffsetTheta)		stru_FOC.Offset_Angle++;
	// 	if 	(stru_FOC.Offset_Angle > Para.FOC.OffsetTheta)		stru_FOC.Offset_Angle--;
		
	// }
	// //===============================================================================//	
	// // 电机工作在非运行态
	// else
	// {
		
	// 	stru_FOC.W_Cycle = 0;
	// 	PID_WL.Kp = Para.FOC.Wkp;	
	// 	PID_WL.Ki = Para.FOC.Wki;
	// 	stru_Speed_Ctrl.RampInc = SPEED_RAMP_INC;
	// 	stru_Speed_Ctrl.RampDec = SPEED_RAMP_DEC;
		
	// 	#if ( Config_Contorl_Mode == Current_Loop )	
	// 		stru_FOC.WRAMP = 1;
	// 	#elif ( Config_Contorl_Mode == Speed_Loop )
	// 		stru_FOC.WRAMP = TIME_SPEED_LOOP;
	// 	#elif ( Config_Contorl_Mode == Power_Loop )
	// 		stru_FOC.WRAMP = TIME_POWER_LOOP;
	// 	#endif
		
	// 	//外环限制清零		
	// 	stru_OutLoop.LimitFlag    = 0;
	// 	stru_OutLoop.QuitDelayCnt = 0;
	// 	stru_OutLoop.LimDelayCnt  = 0;
	// 	LimDelayCnt2 	 = 0;
	// 	LimDelayCnt3 	 = 0;			
	// }
	// //===============================================================================//	
}


/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : MC_MotorInit_Process
* Description    : 
* Function Call  : 1ms进程调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void MC_MotorInit_Process(void)
{

}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : UartView
* Description    : 串口监测数据--调试用
* Function Call  : 
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
//uint16_t hUartSum = 0;
//uint8_t  hUartScopeArr[10] = {0};
void UartView(int16_t view1, int16_t view2, int16_t view3, int16_t view4)
{
	uint8_t sum = 0;
	
	sum += (uint8_t)(view1);
	sum += (uint8_t)(view1>>8);
	sum += (uint8_t)(view2);
	sum += (uint8_t)(view2>>8);
	sum += (uint8_t)(view3);
	sum += (uint8_t)(view3>>8);
	sum += (uint8_t)(view4);
	sum += (uint8_t)(view4>>8);
	
	// UART->dat = (uint8_t)(view1);
	// UART0->dat = (uint8_t)(view1>>8);
	// UART0->dat = (uint8_t)(view2);
	// UART0->dat = (uint8_t)(view2>>8);
	// UART0->dat = (uint8_t)(view3);
	// UART0->dat = (uint8_t)(view3>>8);
	// UART0->dat = (uint8_t)(view4);
	// UART0->dat = (uint8_t)(view4>>8);
	// UART0->dat = sum;
	
	UART1->dat = (uint8_t)(view1);
	UART1->dat = (uint8_t)(view1>>8);
	UART1->dat = (uint8_t)(view2);
	UART1->dat = (uint8_t)(view2>>8);
	UART1->dat = (uint8_t)(view3);
	UART1->dat = (uint8_t)(view3>>8);
	UART1->dat = (uint8_t)(view4);
	UART1->dat = (uint8_t)(view4>>8);
	UART1->dat = sum;	
}

/*****************************************************************************
* Function Name  : Temp_Table_Search
* Description    : 温度查表
* Function Call  : 10ms
* Input Paragram : 
* Return Value   : none
* note           : 
* Version        : V0.1    2025/07/28    新建				Lsy
******************************************************************************/

int32_t Temp_Table_Search(int32_t Vtem)
{
	return 0;
}		
/*****************************************************************************
 ** \brief Motor_Actual_Calculate
 **			
 ** \param [in] none
 ** \return  none
 ** \note	电机实际参数计算(电压，电流，功率，转速)(电压、电流=实际值*100) //2021-08-02
*****************************************************************************/
void Motor_Actual_Calculate(void)
{

}


/*****************************************************************************
* Function Name  : Fault_Clear_RestartTimes
* Description    : 清除重启次数
* Function Call  : 
* Input Paragram : 
* Return Value   : none
* note           : 
* Version        : V0.1    2024/07/05    新建				Nmg,Lsy
******************************************************************************/
void Fault_Clear_RestartTimes(void)
{

}
	
/*****************************************************************************
* Function Name  : Fault_ReStart_Process
* Description    : 故障重启处理
* Function Call  : 主函数1ms任务
* Input Paragram : 
* Return Value   : none
* note           : 
* Version        : V0.1    2024/07/05    新建				Nmg,Lsy
******************************************************************************/
void Fault_ReStart_Process(void)
{
	


}


/*****************************************************************************
* Function Name  : Fault_Show_Source
* Description    : 按照优先级显示故障代码
* Function Call  : 主函数10ms任务调用
* Input Paragram : 
* Return Value   : none
* note           : 
* Version        : V0.1    2024/07/05    新建				Nmg,Lsy
******************************************************************************/
void Fault_Show_Source(void)
{

	
}


/*****************************************************************************
* Function Name  : Sw_to_Transition_Check
* Description    : 判断SW状态切换到RUN状态
* Function Call  : EPWM高频中断调用
* Input Paragram : 
* Return Value   : none
* note           : 
* Version        : V0.1    2024/07/05    新建				Nmg,Lsy
******************************************************************************/
// extern uint16_t g_smo_speed;
extern uint16_t g_smo_speed_short;


// extern uint16_t g_smo_speed_long;
void Sw_to_Transition_Check(void)
{
	int16_t sw_to_run_speed_error;
	
	
	sw_to_run_speed_error =  MC_Start_End_Speed - g_smo_speed_short;

	if(sw_to_run_speed_error < 0){sw_to_run_speed_error = (-sw_to_run_speed_error);}

	if(sw_to_run_speed_error <  Sw_To_Run_Error_Threshold)
	{
		stru_Para.Sw.sw_to_run_cnt++;
	}
	else
	{
		stru_Para.Sw.sw_to_run_cnt = 0;
	}
	if(stru_Para.Sw.sw_to_run_cnt > Sw_To_Run_Cnt_Threshold)
	{
		stru_Para.Sw.sw_to_run_cnt = Sw_To_Run_Cnt_Threshold + 1;

        //stru_Para.Sw.delta_theta_openloop_to_smo = (FOC->ro_angl_auto & 0xFFFF)- (FOC->ro_angl_auto>>16);
		
		stru_Para.Sw.delta_theta_openloop_to_smo = (FOC->ro_angl_after & 0xFFFF)- (FOC->ro_angl_after>>16);
		//开环时最终使用的角度为开环角度，低16位
	
		if(stru_Para.Sw.delta_theta_openloop_to_smo < -32767)
		{
			stru_Para.Sw.delta_theta_openloop_to_smo += 65535;
		}
		else if(stru_Para.Sw.delta_theta_openloop_to_smo > 32767)
		{
			stru_Para.Sw.delta_theta_openloop_to_smo -= 65535;
		}
		else
		{
			//do nothing
		}
		
		stru_Para.Run.theta_compensation_final = ((uint16_t)stru_Para.Sw.delta_theta_openloop_to_smo);


		// if(stru_Para.Sw.delta_theta_openloop_to_smo < 0)
		// {
		// 	stru_Para.Run.theta_compensation_final = 65536 + stru_Para.Sw.delta_theta_openloop_to_smo;
		// }
		// else
		// {
		// 	//do nothing
		// }

		#if 0
		//高16bit 保持为0；低16bit 
		FOC->angl_comp = (0x0<<16) | (stru_Para.Run.theta_compensation_final<< 0);
	
		

		//电流初值赋值
		stru_FOC.Idq_ref.half.iq = (uint16_t)(FOC->Id_req>>16);

		
		// 切换到SMO观测器
		FOC->ctrl0 &= ~BIT2;

		// 切换到RUN状态
		MOTOR_STATE = MC_TRANSITION;
		#endif
	}

}

/*****************************************************************************
* Function Name  : Transition_to_Run_Check
* Description    : 判断SW状态切换到Transition状态
* Function Call  : EPWM高频中断调用
* Input Paragram : 
* Return Value   : none
* note           : 
* Version        : V0.1    2024/07/05    新建				Nmg,Lsy
******************************************************************************/
void Transition_to_Run_Check(void)
{
	if (++stru_Para.Transition.Transition_to_Run_Cnt > MC_TRANSITION_COMPENSATION_CNT)
	{
		stru_Para.Transition.Transition_to_Run_Cnt = 0;
		if (stru_Para.Sw.delta_theta_openloop_to_smo > 0)
			stru_Para.Sw.delta_theta_openloop_to_smo--;
		else if (stru_Para.Sw.delta_theta_openloop_to_smo < 0)
			stru_Para.Sw.delta_theta_openloop_to_smo++;
		else
			stru_Para.Sw.delta_theta_openloop_to_smo = 0;
	}
	
	stru_Para.Run.theta_compensation_final = (uint16_t)stru_Para.Sw.delta_theta_openloop_to_smo;

	// 高16bit 保持为0；低16bit
	FOC->angl_comp = (0x0 << 16) | (stru_Para.Run.theta_compensation_final << 0);

//	if (stru_Para.Sw.delta_theta_openloop_to_smo == 0)
	{
		//电流初值赋值
		stru_FOC.Idq_ref.half.iq = (uint16_t)(FOC->Id_req>>16);
		
		#if(HIGH_SPEED_MODE_CONFIG == HIGH_SPEED_MODE_ENABLE)
		//FOC->fe_pi = 0x00200020;
		//FOC->fe_pi = 0x00200004;
		//FOC->fe_pi = (((0x20) << 16) | ((0x4) << 0))<<2U;
		
		//FOC->fe_pi = (((0x80) << 16) | ((0x10) << 0));  //2025-12-11
		#endif
		
		//切换到RUN状态
	 	MOTOR_STATE = MC_RUN;
	}
}

 void FOC_RotorPosition_Init()
 {

	 
	 
	 
 }
//===========================================================================//
//*********END   ************************************************************//
//===========================================================================//
