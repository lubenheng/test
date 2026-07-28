//==========================================================================//
/*****************************************************************************
*-----------------------------------------------------------------------------
* @file    main.h
* @author  CMS_Motor_Control_Team
* @version 
* @date    2024年6月
* @brief   
*---------------------------------------------------------------------------//
*****************************************************************************/

//==========================================================================//

#ifndef __DATA_DEFINE_H
#define __DATA_DEFINE_H

/*****************************************************************************/
/* Include files */
/*****************************************************************************/
#include <stdint.h>
#include  "base_define.h"
/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define') */
/*****************************************************************************/
#define Flag_1ms									stru_CNTR.Cnt1ms
#define Flag_10ms									stru_CNTR.Cnt10ms
#define Flag_100ms								stru_CNTR.Cnt100ms
#define Flag_500ms								stru_CNTR.Cnt500ms

#define	Fault_Flag								u_Fault_Flag.Flag
#define	Fault_SC									u_Fault_Flag.Bits.ShortCircuit
#define	Fault_OC									u_Fault_Flag.Bits.OverPhaseCurr
#define	Fault_OV									u_Fault_Flag.Bits.OverVolt
#define	Fault_UV									u_Fault_Flag.Bits.UnderVolt
#define	Fault_BLOCK								u_Fault_Flag.Bits.Block
#define	Fault_LossPhase						u_Fault_Flag.Bits.LossPhase
#define	Fault_OT									u_Fault_Flag.Bits.OverTemp
#define	Fault_OIbus								u_Fault_Flag.Bits.OverPhaseCurr
/*****************************************************************************/
/* Global type definitions ('typedef') */
/*****************************************************************************/
typedef enum 
{
	MC_INIT			= 0, 
	MC_CHARGE		= 1, 
	MC_ALIGN		= 2,	
	MC_START		= 3, 
	MC_SW				= 4, 
	MC_RUN			= 5,   
	MC_STOP			= 6, 
}MotorStates;



typedef	union
{
	uint16_t Flag;
	struct
	{
		uint16_t		ShortCircuit 				: 1;				//硬件过流错误标志位  
		uint16_t		OverPhaseCurr 			: 1;				//软件过流错误标志位  
		uint16_t		OverVolt   					: 1;				//过压错误标志位     
		uint16_t		UnderVolt  					: 1;				//欠压错误标志位       
		uint16_t		Block      					: 1;				//堵转错误标志位      
		uint16_t		LossPhase  					: 1;				//缺相错误标志位       
		uint16_t		OverTemp   					: 1;				//过温错误标志位     
		uint16_t		Offset							: 1;				//偏置错误标志位        
		uint16_t		OverIbus      			: 1;				//母线过流标志位       		
		uint16_t                 				: 7;				//保留，可根据其他需求来添加错误标志位，注意不要超过16位，否则需要修改u16 R --> u32 R
	} Bits;	
}Union_Fault;




//---------------------------------------------------------------------------/


typedef struct
{
	//-------------------------------------------------------------------//
	// 母线硬件过流、短路保护
	struct
	{
		uint8_t			ReStartNum;				//设置的重启次数
		uint8_t			ReStartTimes;			// 已重启次数
    uint16_t		ReStartGAP;				//设置重启间隔时间    

	} SC;
	//-------------------------------------------------------------------//
	// 母线电压保护
	struct
	{
		// 启动
		uint16_t		Start_OV_Value;					// 过压保护值
		uint16_t		Start_UV_Value;					// 欠压保护值
		// 运行
		uint16_t		RUN_OV_Value;						// 过压保护值
		uint16_t		RUN_UV_Value;						// 欠压保护值
		// 恢复
		uint16_t		OV_Recover_Value;				// 过压恢复值
		uint16_t		UV_Recover_Value;				// 欠压恢复值		
		
		// 运行掉电
		int16_t			Vbus_1msLast;
		int16_t			Vbus_100msLast;
	} Vbus;
	//-------------------------------------------------------------------//
	// 温度保护
	struct
	{
		int16_t		Over_Temp_Value;			// 过温保护值
		int16_t		Under_Temp_Value;			// 欠温保护值

		int16_t		OT_Recover_Value;			// 过温恢复值
		int16_t		UT_Recover_Value;			// 欠温恢复值		
	} Temp;	
	//-------------------------------------------------------------------//
	// 堵转保护
	struct
	{
		uint8_t			ReStartNum;					//设置的重启次数
		uint8_t			ReStartTimes;				// 已重启次数
    uint16_t		ReStartGAP;     		//设置重启间隔时间    
		
		int32_t			BlockSpeed_Max;			// 堵转保护最大值
		int32_t			BlockSpeed_Min;			// 堵转保护最小值
	} Block;	

}struct_FaultProtect;

typedef struct
{
		//-----------AD值到标幺值Q10--------------------/
    int32_t AD2PU_Vbus; 	
		int32_t AD2PU_Ibus; 	
    int32_t AD2PU_Ip; 	
		int32_t	AD2PU_Vctrl;

		//----------物理值到标幺转换系数Q10--------------------/
		int32_t Phy2PU_Vbus; 
		int32_t Phy2PU_Ibus; 
		int32_t Phy2PU_Power; 

		int32_t Phy2PU_Iphase; 
		int32_t Phy2PU_Speed; 	
		
		int32_t	Phy2PU_Vctrl;

		//----------标幺值到物理值Q15---------------------- 
		int32_t PU2Phy_Vbus; 
		int32_t PU2Phy_Ibus; 
		int32_t PU2Phy_Power; 	
		
		int32_t PU2Phy_Iphase; 
		
		int32_t PU2Phy_Vctrl; 		
}struct_Coff;

typedef struct
{
	s32 		ADVal[10];
	s32 		OFST[10];

	s32			G_Ip; 
	s32			G_Ib; 
	s32			G_Vb; 
	s32			G_Vc; 
	s32			G_Sp; 
	s32			G_Pw;
	s32			G_Power;
}struct_ADC_Sample;
//---------------------------------------------------------------------------/
typedef struct
{
    int32_t Vbus100mV;
    int32_t Ibus10mA;
    int32_t MecSpeed;   
    int32_t Power100mW;
		int16_t	Vctrl_10mV;
		int32_t Ip_peak_10mA;
}struct_PhyValue;       

typedef struct
{
    int32_t Vbus;
    int32_t Ibus;
    int32_t MecSpd;                 
		int32_t Power;
		int32_t Vctrl;

}Struct_PU_Value;
typedef struct
{
	Struct_PU_Value		Pu_Val;
	Struct_PU_Value		PuVal_F;
	
	struct_PhyValue		PhyVal;
}struct_Measure;



typedef struct 
{ 	
  s32 Kp;
  s32 Ki;
	
  s32 Error;	
  s32 Integral;
  s32 Out;

  s32 Output_Min;     
  s32 Output_Max;     

	
} struct_PI;

typedef struct
{
	int32_t Ia;
	int32_t Ib;
	int32_t Ic;
}struct_Curr_abc;

typedef struct
{
	int32_t 	Alpha;
	int32_t 	Beta;
	
}struct_Clark;

typedef struct
{
	int32_t 	Id;
	int32_t 	Iq;
	
}struct_Park;

typedef struct
{
	int32_t 	Alpha;
	int32_t 	Beta;
	
}struct_RevPark;
typedef struct
{
	int32_t 	Cos;
	int32_t 	Sin;
	
}struct_SinCos;

typedef struct 
{
	int32_t 	Ud;
	int32_t 	Uq;
	
}struct_Volt_dq;


typedef struct
{
	uint8_t  	Sector;
	int32_t 	Tu;
	int32_t 	Tv;
	int32_t 	Tw;
	int32_t 	T1;
	int32_t 	T2;
	int32_t 	Tmax;	
	
	s32			Ta;
	s32			Tb;
	s32			Tc;
	s32			T0;	
}struct_SVPWM;

typedef struct
{
	u8	RunFlag;
	s32 Elec_Angle;
	s32	Elec_Omega;
	s32	BaseOmega;
	
	s32 Cur_Is;
	s32	Cur_Is_Max;
	s32	Cur_Is_Min;
	
	u16	EPWM_Period;
	s32	VsAmp_Max;
	
}struct_FOC;


typedef struct
{
	u32	ChargeState;
	u32	ALIGNState;
	u32	StartState;
	u32	SWState;	
	u8	Cnt1ms				;
	u8	Cnt10ms				;
	u8	Cnt100ms			;	
	u16	Cnt500ms			;
	
}struct_Cntr;

typedef struct
{
	uint16_t		DeltaCnt;					// 加速间隔周期
	int16_t			DeltaVolt;

	int16_t			OmegaEMax;					//    拖动角速度
	int16_t			OmegaE;	
	
	int16_t			DragVolt;	
	int32_t			Volt_Final;
	
}struct_Test;


//typedef struct 
//{ 	
//	uint8_t	SigOnFlag;						// 信号标志  0无控制信号 1 有控制信号
//	int32_t InputSig;							// 控制信号值
//	
//	int32_t Target_Min;
//	int32_t Target_Max;
//	
//	int32_t Signal_OFF;
//	int32_t Signal_ON;
//	int32_t Signal_Max;
//	
//	int32_t TargetKe;
//	int32_t TargetB;
//	
//	int32_t Target;							// 控制目标值


//	
//}struct_User;

//typedef struct 
//{ 	
//	struct
//	{
//		s32	Time;
//		s32 Curr;

//	}Align;
//	struct
//	{
//		s32 ACCCycle;
//		
//		s32	DeltaCur;
//		
//		s32	Final_Curr;
//		s32	OmegaMax;
//		
//	}Start;
//	struct
//	{
//		u32	HoldTime;
//		

//	}SW;
//	struct
//	{
//			s32 Ekp;
//	}Run;	
//	
//}struct_Para;


typedef struct
{	
	int32_t 	Valpha;	
	int32_t 	Vbeta;	
	int32_t 	Ialpha;
	int32_t 	Ibeta;	
	int32_t 	Ealpha;		
	int32_t 	Ebeta;		
	int32_t 	Zalpha;		
	int32_t 	Zbeta;	
	int32_t 	Est_Ialpha;		
	int32_t 	Est_Ibeta;
	int32_t		IalphaError;
	int32_t		IbetaError;

	int32_t		PI_Error;
	int32_t		PI_Kp;
	int32_t		PI_Ki;
	int32_t		PI_Ui;
	int32_t 	PI_Out;
	int32_t		PI_Upper;
	int32_t		PI_Lower;
	
	int32_t 	Fs;
	int32_t 	Gs;
	
	int32_t		E0;
	int32_t		Kslide;
	int32_t		Kslf;

	int32_t		We;
	int32_t		Angle;		

}struct_smo_est;

/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source) */
extern struct_PI								stru_PI_ID;
extern struct_PI								stru_PI_IQ;
extern struct_SinCos	  				stru_SinCos;
extern struct_Curr_abc					stru_Curr_abc;
extern struct_Clark	  					stru_Curr_alphabeta; 
extern struct_Park		  				stru_Curr_dq; 				
extern struct_Park		  				stru_Curr_dq_ref; 		
extern struct_Volt_dq	  				stru_Volt_dq; 		
extern struct_RevPark	  				stru_Volt_alphabeta; 
extern struct_SVPWM        			stru_SVM_Calc;

extern struct_PI								stru_PI_OutLoop;
extern struct_Coff							stru_Coff;
extern struct_ADC_Sample				stru_Sample;
extern struct_Measure						stru_Measure;
extern struct_FOC								stru_FOC;
extern volatile MotorStates 		MOTOR_STATE; 
extern struct_Cntr							stru_CNTR;
extern struct_Test							stru_Test;
extern volatile Union_Fault			u_Fault_Flag;

extern struct_User							stru_User;
extern struct_Para							stru_Para;

extern struct_smo_est						stru_smo;
/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source) */
/*****************************************************************************/
void System_Init(void);
void DelayTime_ms(unsigned int delay);
int32_t HWDIVSQRT_SQRT(int32_t x);
int32_t HWDIV_DIV(int32_t x, int32_t y);

struct_Park Park(struct_SinCos SinCosVal, struct_Clark Clarke);
struct_Clark Clarke(struct_Curr_abc Iphase);
struct_RevPark RevPark(struct_SinCos SinCosVal, struct_Volt_dq Volt);
struct_SinCos  FOC_SinCos_Calc(int32_t Angel);
void FOC_CurrenLoop_Cal(void);
void FOC_SVPWM_Calc(struct_SVPWM *SVM_Compt, struct_RevPark  Volt_alfa_beta);

void Motor_ParaInit(void);
void Motor_CtrlInit(void);
void Function_PI_Cal(struct_PI *PI,s32 Ref, s32 Feedbk);
void Get_ADC_Offset(void);
void FOC_Read_Current(void);
void MC_OutLoop_Ctrl(void);
void MC_Speed_Cal(void);
void ADC_Sample_MidFre(void);
void TimeTask_Proccess(void);
void MC_Fault_Detection(void);



void FOC_Control_Process(void);


void UartView(s16 view1,s16 view2,s16 view3,s16 view4);
void UserCtrl_Init(void);
void User_Speed_Ctrl(void);


void FOC_SMO_Angle_Calc(struct_smo_est *pHandle);
void smo_Ctrl_Init(void);
void SMO_ParaInit(void);
/*****************************************************************************/
/* Intrinsic function definition ('__inline', definition in Header File) */
/*****************************************************************************/


/*****************************************************************************/
/* Macro Definition function definition ( definition in Header File) */
/*****************************************************************************/

#endif

/******************************** END OF FILE *******************************/
