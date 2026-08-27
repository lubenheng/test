//==========================================================================//
/*****************************************************************************
*-----------------------------------------------------------------------------
* @file    mc_ctrl.c
* @author  7
* @version V3.0.0
* @date    
* @brief   
*---------------------------------------------------------------------------//
*****************************************************************************/
//==========================================================================//

#ifndef  __MOTOR_CONTROL_H__
#define  __MOTOR_CONTROL_H__

/*****************************************************************************/
/* Include files */
/*****************************************************************************/
#include 	<stdint.h>
//#include  "base_define.h"
#include "types.h"

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define') */
/*****************************************************************************/
#define Fault_Flag		    				u_Fault_Flag.Byte
#define	Fault_ShortCircuit				u_Fault_Flag.Bits.Bit0
#define	Fault_UnderVoltage				u_Fault_Flag.Bits.Bit7	
#define	Fault_StartFail						u_Fault_Flag.Bits.Bit6 
#define	Fault_OverVoltage					u_Fault_Flag.Bits.Bit5 
#define	Fault_NTC_Temperature			u_Fault_Flag.Bits.Bit4 
#define	Fault_OverCurrent					u_Fault_Flag.Bits.Bit3 
#define	Fault_PhaseLoss						u_Fault_Flag.Bits.Bit2 
#define	Fault_Block								u_Fault_Flag.Bits.Bit1



/*****************************************************************************/
/* Global type definitions ('typedef') */
/*****************************************************************************/

//---------------------------------------------------------------------------/

typedef enum 
{
	SYS_INIT		= 0,
	SYS_RUN			= 1,
	SYS_FAULT		= 2,
	SYS_STOP		= 3,
	SYS_WAIT		= 4
}SystStates;

//---------------------------------------------------------------------------/
typedef enum 
{
	MC_INIT			= 0, 
	MC_CHARGE		= 1, 
	MC_WIND			= 2,
	MC_ALIGN		= 3,	
	MC_START		= 4, 
	MC_SW				= 5, 
	MC_RUN			= 6,   
	MC_STOP			= 7, 
	MC_BRAKE		= 8,
	MC_FAULT		= 9
}MotorStates;

//---------------------------------------------------------------------------/
typedef struct
{
		//----------------------------------------------------------------------/
		float			Ub;
		float			Ib;
		float			Rb;	
		float			Lb;	
		float			Fb;	
		float			Tb;	
		float			Wb;	
	
		//----------------------------------------------------------------------/	
		float			Us;
		float			Rs;	
		float			Ls;
		float			Ts;	
		float			Poles;
	
		//----------------------------------------------------------------------/	
		int32_t		Rp;
		int32_t		Lp;	
		int32_t		Tp;		
		int32_t		Fp;	
	
		//----------------------------------------------------------------------/
		int32_t		B;
		int32_t		C;	
		int32_t		G;		
		
		//----------------------------------------------------------------------/
		int32_t		Fs;
		int32_t		Gs;	
	
		
}struct_Base_Value;


//---------------------------------------------------------------------------/
typedef struct 
{
		int32_t 	RampInc;
		int32_t 	RampDec;
		int32_t 	RampTarget;
		int32_t 	RampOut;
		int32_t 	RampBack;
		int32_t 	RampRef;
}struct_SpeedRamp;

//---------------------------------------------------------------------------/
typedef struct 
{
		uint8_t		Mode_Control;
		uint8_t		Mode_Shunt;
		uint8_t		Mode_Svpwm;
		uint8_t		Mode_Wind;
		uint8_t		MotorDir;
		uint8_t		Mode_IPD;
		uint8_t		Mode_Weeken;	
		uint8_t		Mode_Start;			
	
}struct_Config;



//---------------------------------------------------------------------------/
typedef struct
{
		int32_t 	Vbus;
		int32_t		Ibus;
		int32_t		VCtr;
		int32_t		Power;	
		int32_t		dq_power;	
		int32_t		Speed_Close;	
		int32_t		Speed_Min;
		int32_t		Speed_Base;
		int32_t		Speed_Stop;	
		int32_t		Speed_Brake;	
		int32_t		Calb_Speed;
		int32_t		Calb_SpeedLimit;
		int32_t		Calb_Speed_Min;
		int32_t		Calb_Speed_Max;	
		int32_t		Calb_Vbus;
		int32_t		Mech_Speed;
		int32_t		FG_Speed;
		int32_t 	We;
		int32_t		Freq_EPWM;
		int32_t 	Tpwm_us;
		int32_t 	We_Ke;	
		int32_t		Speed_Ke;
		float			MotorVolt;
		float			MotorCurr;	
		float			Ke;
		uint16_t	ERAMP;
		
		int32_t 	Vbus_Check;	//过欠压保护使用（未滤波）
		int32_t		VTem;		
		int32_t		Target_Power;
		
		s32				VbusFilt;
}struct_Basic_Para;

//---------------------------------------------------------------------------/
typedef	union
{
	unsigned char Byte;
	struct
	{
		unsigned char	Bit0	:1;
		unsigned char	Bit1	:1;
		unsigned char	Bit2	:1;
		unsigned char	Bit3	:1;
		unsigned char	Bit4	:1;
		unsigned char	Bit5	:1;
		unsigned char	Bit6	:1;
		unsigned char	Bit7	:1;
	}Bits;
}Union_Fault;

//---------------------------------------------------------------------------/
typedef struct
{
	s32			Speed_Ref; 
	s32			Speed_Ref_Min; 	
	s32			Curr_Is_Ref; 
	s32			Curr_Iq_Max;
	s32			Curr_Iq_Min;
	s32 		Elec_Angle;
	u32 		MotorRunTime;	
	s32 		Is_beta;
	s32 		IPD_Angle;
	u16 		W_Cycle;
	u16 		I_Cycle;	
	u16 		WRAMP;
	u16 		IRAMP;
	u16 		FRAMP;
	u16 		SRAMP;	
	u16 		ERAMP;		
	s32 		Vs_Max;
	s32 		Vs_Square_Max;
	u16 		EPWM_Period;
	u16 		EPWM_SAMPDLY;
	u16 		TSAM;
	u16 		TSHIFT;
	u16 		TADVANCE;

	s32			Ksilde;
	s32			Ke;	
	s32			Kslf;
	s32			Kslf_L;
	s32			Kslf_H;	
	s32			KBW;		
	float		Kslf_Bemf;	

	s32			Weakening;
	s32			Vout;
	s32			PowerLimit;
	s32			Elec_We;
	s32			Offset_Angle;
	s32			Volt_Uq_Ref;   		//Vq模式
	s32			CurrentLimit; 		//2022-06-27		
	s32			CurrentLimit1; 		//2022-10-18
	s32			CurrentLimitFlag; 		//2022-10-18	
	s32			CurrentLimitCount; 		//2022-10-18
}struct_FOC_Ctrl;

//---------------------------------------------------------------------------/
typedef struct
{
	u8  		IP_CH;
	s32 		ADVal[10];
	s32 		ARR[10];
	s32 		REF[10];
	s32			G_Ip; 
	s32			G_Ib; 
	s32			G_Vb; 
	s32			G_Vc; 
	s32			G_Sp; 
	s32			G_Pw;
	s32			G_Power;
	s32			G_BG;
	s32			G_Vbemf;	
}struct_ADC_Sample;

//---------------------------------------------------------------------------/
typedef struct
{
	u8			MC_RunStop	:1;
	u8			Speed_Ramp	:1;
	u8			IPD					:1;	
	u8			Charge			:1;
	u8			MotorBack		:1;
	u8			MotorFR;
	u8			MotorFR_Pre;
	u8			CurrentOffSet;
}struct_Flag;

//---------------------------------------------------------------------------/
typedef struct
{
	u8	 		Sample_Chenal;
	u16  		Charge_Count;
	u16	 		Motor_Restart;
	u16			FG_Count;
	u32	 		Motor_Stophold;
	u32	 		Motor_StartFail;	
	u32	 		Motor_Brake;
	u32	 		Motor_Block;	
	u32	 		Motor_PowerDown;
	u32	 		Motor_Runing;
	u32	 		Motor_Limit_Curent;
	u32	 		Motor_StartSuccess;	

}struct_Time;

//---------------------------------------------------------------------------/
typedef struct
{
	u16  		Charge_Count;
	u32	 		Motor_Restart;

	u16  		TimeBase[5];

}struct_Count;

//---------------------------------------------------------------------------/
typedef struct
{
	u8			Block_Cnt;
	u8			PhaseLoss_Cnt;
	u8			StartFail_Cnt;
	u8			Brake_Cnt;
	u8			TorgueLoss_Cnt;
	u8			Brake;
	u8			Braking;
	u16			PhaseA_LossCnt;
	u16			PhaseB_LossCnt;
	u16			PhaseC_LossCnt;
	u16			Phaseloss_Value;
	u32			StartOverVoltage_Value;
	u32			StartLowerVoltage_Value;
	u32			RunOverVoltage_Value;
	u32			RunLowerVoltage_Value;
	u32			OV_Recover_Value;
	u32			UV_Recover_Value;
	s32			OverCurrent_Value;

	s32			Ia_max;
	s32			Ib_max;
	s32			Ic_max;	
	
	float		Block_Fault_K1;
	float		Block_Fault_K2;	
	
	s32			Over_Temperature_Value;
	s32			Over_Temperature_Recover_Value;
	s32			Lower_Temperature_Value;
	s32			Lower_Temperature_Recover_Value;	

	u16			BLDC_PhaseA_LossCnt;
	u16			BLDC_PhaseB_LossCnt;
	u16			BLDC_PhaseC_LossCnt;
}struct_Fault;

//---------------------------------------------------------------------------/
typedef struct 
{
	u8		CompleteFlag:1;				//捕获完成标志
	u8		OverFlowFlag:1;				//计数溢出标志
	u16		CaptureCount;					//采集顺序
	u16 	WeakenTime;						//溢出电平拉低消抖计时
	u16 	HeightenTime;					//溢出电平拉高消抖计时
	u32		CaptureValue[4];			//捕获的值
	u32 	OverFlowTime;					//溢出时间计数	
	u32		CaptureTimeL;					//捕获的低电平时间
	u32		CaptureTimeH;					//捕获的高电平时间	
	
	u32		Frequency;						//频率
	u32		DutyCycle;						//占空比
	u32		FrequencyOld;					//频率旧值
	u32		DutyCycleOld;					//占空比旧值
	u32		FrequencyTemp;				//频率缓冲
	u32		DutyCycleTemp;				//占空比缓冲

	u32		FG_SpeedOld;					//速度旧值
	u16		FG_SpeedMin;					//FG输出速度最小值
	u16		FG_Period;            //FG输出周期
	u32		CaptureTimeCycle;			//捕获的高低电平时间	
}struct_Capture;

//---------------------------------------------------------------------------/
typedef struct 
{
	u8		SleepFlag:1;				//
	u16		SleepCount;					//
}struct_Sleep;

//---------------------------------------------------------------------------/

//---------------------------------------------------------------------------/
typedef struct 
{
	u8		mode;
	u16		rpm_init;
	u16		rpm_final;
	s32 	align_current;
	u32		align_time;
	u32		align_addtime;
	u32		align_holdtime;
	u32		align_add_k;	
	s32		run_current_init;
	s32		run_current_final;
	u32		run_time;
	s32		Dkp;
	s32		Dki;
	s32		Dout_max;
	s32		Dout_min;
	s32		Qkp;
	s32		Qki;
	s32		Qout_max;
	s32		Qout_min;
	s32		Ekp;
	s32		Eki;
	s32		Eout_max;
	s32		Eout_min;
	s32		EInt_max;
	s32		EInt_min;
	s32		Etheta;
	u32		Hold_Time;
}STARTPARA;

//---------------------------------------------------------------------------/
typedef struct 
{
	s32		Dkp;
	s32		Dki;
	s32		Dout_max;
	s32		Dout_min;
	s32		Qkp;
	s32		Qki;
	s32		Qout_max;
	s32		Qout_min;
	s32		Wkp;
	s32		Wki;
	s32		Wkd;
	s32		Wout_max;
	s32		Wout_min;
	s32		OffsetTheta;
	s32		Ekp;
	s32		Eki;
	s32		SMO_Gs;
	s32		SMO_Fs;	
	s32		Fc_LPF;	
}FOCPARA;

//---------------------------------------------------------------------------/
typedef struct 
{	

	s32		Samp_Ibus;
	s32		Samp_Vbus;
	s32		Samp_Vctrl;
	s32		Samp_Vtem;
	s32		Speed_Val;
	s32		Power_Val;
	s32		PhaseK;
}LPFPARA;


//---------------------------------------------------------------------------/
typedef struct	
{
	STARTPARA	Start;
	FOCPARA		FOC;
	LPFPARA		Lpf;

}struct_MotCtrl_Para;

//---------------------------------------------------------------------------/

typedef struct
{
	int32_t			G_Vbus;
	int32_t			G_Ibus;
	int32_t			G_Power;
	int32_t			Vbus;
	int32_t			Ibus;
	int32_t			Power;
	int32_t			Pre_Power;
	int32_t			Speed;	
}struct_MotorRuning;


typedef struct
{
	uint16_t		DeltaCnt;					// ?ó?ù?????ü?ú
	int16_t			Delta_Ref;

	int16_t			OmegaEMax;					//    í??ˉ???ù?è
	int16_t			OmegaE;	
	
	int16_t			Drag_Ref;	
	int32_t			Ref_Final;
	
}struct_Test;

//---------------------------------------------------------------------------/

/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source) */
/*****************************************************************************/
extern volatile Union_Fault				u_Fault_Flag;
extern volatile SystStates 				SYSTEM_STATE;
extern volatile MotorStates 			MOTOR_STATE;
extern struct_Base_Value					stru_base_value;
extern struct_SpeedRamp						stru_Speed_Ctrl;
extern struct_ADC_Sample 					stru_Sample;
extern struct_FOC_Ctrl						stru_FOC;
extern struct_Basic_Para					Basic;
extern struct_Config							Config;
extern struct_Flag 								Flag;
extern struct_Time    						Time; 
extern struct_Count   						Count; 
extern struct_Fault 							Fault;
extern struct_MotCtrl_Para				Para;
extern struct_Capture							Capture;

extern struct_MotorRuning					Motor_Actual;

extern uint32_t                   wMotorLockSwitchTime;

extern volatile uint8_t           bStartFailFlagLast;
extern int32_t                    wStartCurrentNext;
extern int32_t                    wStartFailTimeFrist;
extern int32_t                    wStartFailTimeNext;
extern int32_t                    wObseFs;
extern int32_t                    wObseGs;
extern struct_Test							stru_Test;
/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source) */
/*****************************************************************************/
void	MC_BaseValue_Init(void);
void 	MC_MotorInit_Process(void);
void 	System_Control(void);
void 	MC_MotorFault_Check(void);
void 	MC_MotorSpeed_Calc(void);
void  MC_Speed_Control(void);
void 	MC_Clear_FaultMessage(void);
void 	SysTick_1ms(void);
//void 	UartView(s16 view1,s16 view2,s16 view3,s16 view4);
void 	User_Motor_On(void);
void 	User_Motor_Off(void);
void 	User_Motor_FRControl(u8 Input_Val);
void 	User_MotorSpeed_Set(s32 Input_Val);
void 	System_Modules_off(void);
void 	User_Speed_Control(void);
void 	User_Speed_Capture(void);
void 	User_Speed_Out(void);
void 	CCP_Capture_Init(void);
void 	CCP_PWM_Init(void);
void	CCP_PWM_GPIO(uint8_t bMode,uint8_t bLevle);
void 	User_Sleep_Manage(void);
void	Motor_Actual_Calculate(void);
void	GET_ADC1_REF_VAL(void);
void	GET_ADC0_REF_VAL(void);
void Motor_IBUS_REF_Calibration(void);

void Comp_LowCTNR_Cal(void);

void OutLoop_ParaInit(void);
void Power_Loop_Ctrl(void);
void Ibus_Loop_Ctrl(void);
void Speed_Loop_Ctrl(void);
void VQ_Loop_Ctrl(void);
void CurrLoop_Ctrl(void);
void OutLoop_CtrlInit(void);
s32 IbusRef_RealTime_Cal(s32 InputAd);
void IbusRef_InitDelta_Sample(void);
#endif	/* __MOTOR_CONTROL_H__ */

