//==========================================================================//
/*****************************************************************************
*-----------------------------------------------------------------------------
* @file    mc_ctrl.c
* @author  CMS_Motor_Control_Team
* @version V3.0.0
* @date    2021年3月
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
#include  "base_define.h"

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define') */
/*****************************************************************************/
#define	Fault_Flag								u_Fault_Flag.Flag
#define	Fault_ShortCircuit				u_Fault_Flag.Bits.ShortCircuit					// 硬件过流
#define	Fault_Phase_OC						u_Fault_Flag.Bits.OverPhaseCurr					// 相线软件过流
#define	Fault_Phase_Loss					u_Fault_Flag.Bits.LossPhase							// 缺相
#define	Fault_Ibus_OC							u_Fault_Flag.Bits.Ibus_OC								// 母线均值过流
#define	Fault_StartFail						u_Fault_Flag.Bits.StartFail							// 启动失败
#define	Fault_OFFSET							u_Fault_Flag.Bits.Offset_Err						// 偏置错误
#define	Fault_UT									u_Fault_Flag.Bits.UnderTemp							// 低温保护
#define	Fault_OT									u_Fault_Flag.Bits.OverTemp							// 高温保护
#define	Fault_Block								u_Fault_Flag.Bits.Block									// 堵转
#define	Fault_Vbus_UV							u_Fault_Flag.Bits.Vbus_UV								// 母线欠压
#define	Fault_Vbus_OV							u_Fault_Flag.Bits.Vbus_OV								// 母线过压
#define	Fault_VCC_UV							u_Fault_Flag.Bits.VCC_UV								// 驱动VCC供电欠压
#define	Fault_HALL_Ero						u_Fault_Flag.Bits.Hall_Ero							// 霍尔异常


/*****************************************************************************/
/* Global type definitions ('typedef') */
/*****************************************************************************/

//---------------------------------------------------------------------------/
typedef union
{
    int32_t       dword;
    struct
    {
        uint16_t  lword;
        int16_t   hword;
    }             half;
} Union_DWord;


typedef union
{
    uint32_t word;
    struct
    {
        int16_t id;
        int16_t iq;
    } half;
} Curr_Is_ref_t;



typedef union
{
    uint32_t word;
    struct
    {
        int16_t Ud;
        int16_t Uq;
    } half;
} Udq_req_t;



typedef union
{
    uint32_t word;
    struct
    {
        uint16_t Ud_max;
        uint16_t Ud_min;
    } half;
} Ud_limit_t;


typedef union
{
    uint32_t word;
    struct
    {
        uint16_t Uq_max;
        uint16_t Uq_min;
    } half;
} Uq_limit_t;


typedef union
{
    uint32_t word;
    struct
    {
        int16_t Ualpha;
        int16_t Ubeta;
    } half;
} Ualpha_beta;


typedef union
{
    uint32_t word;
    struct
    {
        int16_t Iu;
        int16_t Iv;
    } half;
} Iuv;

typedef union
{
    uint32_t word;
    struct
    {
        int16_t Ialpha;
        int16_t Ibeta;
    } half;
} Ialpha_beta;


typedef enum
{
    SYS_INIT		= 0,
    SYS_RESTART	= 1,
    SYS_RUN			= 2,
    SYS_FAULT		= 3,
    SYS_STOP		= 4,
    SYS_WAIT		= 5
} SystStates;

//---------------------------------------------------------------------------/
typedef enum
{
    MC_INIT			= 0,
    MC_CHARGE		= 1,
    MC_WIND			= 2,
    MC_ALIGN		= 3,
    MC_START		= 4,
    MC_SW			= 5,
    MC_TRANSITION	= 6,
    MC_RUN			= 7,
    MC_STOP			= 8,
    MC_BRAKE		= 9,
    MC_FAULT		= 10

} MotorStates;

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


} struct_Base_Value;


//---------------------------------------------------------------------------/
typedef struct
{
    int32_t 	RampInc;
    int32_t 	RampDec;
    int32_t 	RampTarget;
    int32_t 	RampOut;
    int32_t 	RampBack;
    int32_t 	RampRef;
} struct_SpeedRamp;

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

} struct_Config;



//---------------------------------------------------------------------------/
typedef struct
{
    // int32_t 	Vbus;
    // int32_t		Ibus;
    // int32_t		VCtr;
    // int32_t		Power;
    // int32_t		dq_power;
    // int32_t		Speed_Close;
    // int32_t		Speed_Min;
    // int32_t		Speed_Base;
    // int32_t		Speed_Stop;
    // int32_t		Speed_Brake;
    // int32_t		Calb_Speed;
    // int32_t		Calb_SpeedLimit;
    // int32_t		Calb_Speed_Min;
    // int32_t		Calb_Speed_Max;
    // int32_t		Calb_Vbus;
    int16_t		Mech_Speed;
    int16_t		FG_Speed;
    // int32_t 	We;
    // int32_t		Freq_EPWM;
    // int32_t 	Tpwm_us;
    // int32_t 	We_Ke;
    // int32_t		Speed_Ke;
    // float			MotorVolt;
    // float			MotorCurr;
    // float			Ke;
    // uint16_t	ERAMP;

    // int32_t 	Vbus_Check;	//过欠压保护使用（未滤波）
    // int32_t		VTem;
    // int32_t		Target_Power;

    // int32_t				FluxWeBase;
    // int32_t				Vmef;
    // int32_t				Uqmef;

    // int32_t		DeadTime;
    // s32				VbusFilt;
} struct_Basic_Para;

//---------------------------------------------------------------------------/
typedef	union
{
    uint16_t Flag;
    struct
    {
        uint16_t		ShortCircuit 				: 1;				//硬件过流错误标志位
        uint16_t		Block      					: 1;				//堵转错误标志位
        uint16_t		OverPhaseCurr 				: 1;				//软件过流错误标志位
        uint16_t		Vbus_OV   					: 1;				//过压错误标志位
        uint16_t		Vbus_UV  					: 1;				//欠压错误标志位
        uint16_t		LossPhase  					: 1;				//缺相错误标志位
        uint16_t		OverTemp   					: 1;				//过温错误标志位
        uint16_t		UnderTemp   				: 1;				//过温错误标志位
        uint16_t		Offset_Err					: 1;				//偏置错误标志位
        uint16_t		StartFail      				: 1;				//启动错误标志位
        uint16_t		Ibus_OC      				: 1;				//母线过流标志位
        uint16_t		VCC_UV      				: 1;				//母线过流标志位
        uint16_t		Hall_Ero      				: 1;				//霍尔异常
        uint16_t                 					: 3;				//保留，可根据其他需求来添加错误标志位，注意不要超过16位，否则需要修改u16 R --> u32 R
    } Bits;
} Union_Fault;

//---------------------------------------------------------------------------/
typedef struct
{
    Ud_limit_t		Ud_limit;
    Uq_limit_t  	Uq_limit;
    Curr_Is_ref_t	Idq_ref;
    Curr_Is_ref_t   Idq_fbk;
    Udq_req_t		Udq_req;
    Udq_req_t		Udq_fbk;
    Ualpha_beta  	Ro_Ualpha_beta;
    Iuv				Ro_Iuv;
    Ialpha_beta		Ro_Ialpha_beta;


    int16_t 		Ud;
    int16_t 		Uq;
    int16_t 		theta;
    int16_t 		open_loop_omega;
    uint16_t		observer_flag;
    uint16_t		Vs_Square_Max;
    int16_t 		theta_comp;


    // s32			Speed_Ref;
    // s32			Speed_Ref_Min;
    // s32			Curr_Is_Ref;
    // s32			Curr_Iq_Max;
    // s32			Curr_Iq_Min;
    // s32 		Elec_Angle;
    // u32 		MotorRunTime;
    // s32 		Is_beta;
    // s32 		IPD_Angle;
    // u16 		W_Cycle;
    // u16 		I_Cycle;
    // u16 		WRAMP;
    // u16 		IRAMP;
    // u16 		FRAMP;
    // u16 		SRAMP;
    // u16 		ERAMP;
    // s32 		Vs_Max;
    // s32 		Vs_Square_Max;
    // u16 		EPWM_Period;
    // u16 		EPWM_SAMPDLY;
    // u16 		TSAM;
    // u16 		TSHIFT;
    // u16 		TADVANCE;

    // s32			Ksilde;
    // s32			Ke;
    // s32			Kslf;
    // s32			Kslf_L;
    // s32			Kslf_H;
    // s32			KBW;
    // float		Kslf_Bemf;

    // s32			Weakening;
    // s32			Vout;
    // s32			PowerLimit;
    // s32			Elec_We;
    // s32			Offset_Angle;
    // s32			Volt_Uq_Ref;   		//Vq模式

} struct_FOC_Ctrl;

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
} struct_ADC_Sample;

//---------------------------------------------------------------------------/
typedef struct
{
    u8			MC_RunStop	: 1;
    u8			Speed_Ramp	: 1;
    u8			IPD					: 1;
    u8			Charge			: 1;
    u8			MotorBack		: 1;
    u8			MotorFR;
    u8			MotorFR_Pre;

} struct_Falg;

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

    u32	 		Motor_StartSuccess;


} struct_Time;

//---------------------------------------------------------------------------/
typedef struct
{
    u16  		Charge_Count;
    u32	 		Motor_Restart;

    u16  		TimeBase[5];

} struct_Count;

//---------------------------------------------------------------------------/
typedef struct
{
    int32_t			Ia_max;
    int32_t			Ib_max;
    int32_t			Ic_max;
    //-------------------------------------------------------------------//
    // 母线硬件过流、短路保护
    struct
    {
        uint8_t			ReStartNum;				//设置的重启次数
        uint8_t			ReStartTimes;			// 已重启次数
        uint16_t		ReStartGAP;				//设置重启间隔时间

    } ShortCircuit;
    //-------------------------------------------------------------------//
    // 相线软件过流
    struct
    {
        uint8_t			ReStartNum;				//设置的重启次数
        uint8_t			ReStartTimes;			// 已重启次数
        uint16_t		ReStartGap;				//设置重启间隔时间

        uint16_t		OC_Value;					// 保护值
    } OverPhaseCur;
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
    // 缺相保护
    struct
    {
        uint8_t			ReStartNum;					//设置的重启次数
        uint8_t			ReStartTimes;				// 已重启次数
        uint16_t		ReStartGAP;					//设置重启间隔时间

        uint16_t		Curr_Min;						// 缺相保护值

        uint16_t		PhaseA_LossCnt;
        uint16_t		PhaseB_LossCnt;
        uint16_t		PhaseC_LossCnt;

        int32_t			LPF_Coff;
    } PhaseLoss;
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

    //-------------------------------------------------------------------//
    // 启动失败保护
    struct
    {
        uint8_t			ReStartNum;					//设置的重启次数
        uint8_t			ReStartTimes;				// 已重启次数
        uint16_t		ReStartGAP;     		//设置重启间隔时间

    } Start;
    //-------------------------------------------------------------------//
    // 母线均值电流保护
    struct
    {
        uint8_t			ReStartNum;					//设置的重启次数
        uint8_t			ReStartTimes;				// 已重启次数
        uint16_t		ReStartGAP;     		//设置重启间隔时间

        uint16_t		Protect_Value;				// 设置的保护判别时间
    } Ibus;

} struct_FaultProtect;

//---------------------------------------------------------------------------/
typedef struct
{
    u8		CompleteFlag: 1;				//捕获完成标志
    u8		OverFlowFlag: 1;				//计数溢出标志
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
} struct_Capture;

//---------------------------------------------------------------------------/
typedef struct
{
    u8		SleepFlag: 1;				//
    u16		SleepCount;					//
} struct_Sleep;

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
} STARTPARA;

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
} FOCPARA;

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
} LPFPARA;


//---------------------------------------------------------------------------/
//typedef struct
//{
//	STARTPARA	Start;
//	FOCPARA		FOC;
//	LPFPARA		Lpf;

//}struct_MotCtrl_Para;

//---------------------------------------------------------------------------/
//typedef struct
//{
//	s32 	Offangle;
//	s32		Kslf;
//	s32		Omeg;
//	float		Factor;
//	s32		Ekp;
//	s32		Eki;
//	s32		Fs;
//	s32		Gs;
//}struct_Smo_Start;


//---------------------------------------------------------------------------/

typedef enum
{
    NOERROR		           = 0,		//无故障
    FAULT_SHORTCIRCUIT	,		// 1 硬件过流故障(刹车保护)
    FAULT_BLOCK	,		// 2 堵转故障
    FAULT_PHASELOSS,		// 3 缺相故障
    FAULT_OVERCURRENT,		// 4 相电流峰值过流故障
    FAULT_OVERIBUSCURRENT,		// 5 母线过流故障
    FAULT_OVERTEMPERATURE,		// 6 NTC过温故障
    FAULT_LOWERTEMPERATURE,		// 7 NTC低温故障
    FAULT_RUNOVERVOLTAGE,		// 8 母线电压运行过压故障
    FAULT_STARTFAIL,		// 9 启动失败故障
    FAULT_RUNUNDERVOLTAGE,		// 10 母线电压运行欠压故障
    FAULT_SAMPLEREF,	 	// 11 采样偏置值异常故障
    FAULT_STARTOVERVOLTAGE,	 	// 12 母线电压启动过压故障
    FAULT_STARTUNDERVOLTAGE,	 	// 13 母线电压启动欠压故障
} FaultCode;

typedef struct
{
    s32			G_Vbus;
    s32			G_Ibus;

    s32			Vbus_10mV;
    s32			Ibus_10mA;
    s32			Power_100mW;
    s32			Speed;

} struct_MotorRuning;

typedef struct
{
    u16			RealVbusCount;				/* 实时电压计数值 */
    u16			TargetVbusCount;			/* 目标电压计数值 */
    u16			FilterTime;						/* 滤波器执行周期 */
    u16			FilterCoefficient;		/* 滤波器系数 */
    s32			Ud;										/* Ud电压补偿输出 */
    s32			Uq;										/* Uq电压补偿输出 */
    s32			Gain;									/* 电压补偿系数 */
    s32			RealVbus;							/* 实时电压 */
    s32			TargetVbus;						/* 目标电压 */

} struct_BusVolt_Comp;


typedef struct
{
    u8  LimitFlag;		//	0 无限制  1 Iq限制   2 Ibus  3 Speedmax 4 speedmin
    u16 LimDelayCnt;
    u16 QuitDelayCnt;

    s32 Spd_LimMax_Pu;
    s32 Spd_LimMin_Pu;

    s32 Pw_LimMax_Pu;
    s32 Pw_LimMin_Pu;
    s32 Ibus_PwMax_Pu;

    s32	Ibus_LimMax_Pu;
    s32 Ibus_LimMin_Pu;
    s32 Ibus_Target;

    s32	Ipha_LimMax_Pu;
    s32	Ipha_LimMin_Pu;

    s32 Power_Limit_Kp;
    s32 Power_Limit_Ki;

    u32 Spd_Limit_Kp;
    u32 Spd_Limit_Ki;
} struct_OutLoop;

typedef struct
{
    uint8_t	SigOnFlag;	// 信号标志  0无用户控制信号 1 有控制信号
    int32_t InputSig;

    int32_t Target_Min;
    int32_t Target_Max;

    int32_t Signal_OFF;
    int32_t Signal_ON;
    int32_t Signal_Max;

    int32_t TargetKe;
    int32_t TargetB;

    int32_t Ctrl_Target;

} struct_UserSet;

typedef struct
{
    u8	RunFlag;
    s32 Elec_Angle;
    u16	Elec_Omega;
    s32	BaseOmega;

    s32 Cur_Is;
    s32	Cur_Is_Max;
    s32	Cur_Is_Min;

    u16	EPWM_Period;
    s32	VsAmp_Max;

} struct_FOC_Start;

typedef struct
{
    struct
    {
        uint16_t	Time;
        uint16_t    Curr;
        uint16_t    Cnt;

    } Align;
    struct
    {
        uint16_t	ACCCycle;

        uint16_t	DeltaCur;

        uint16_t	Final_Curr;

        uint16_t	OmegaMax;
    } Start;
    struct
    {
        uint16_t	sw_to_run_cnt;

        uint16_t	HoldTime;

        int32_t		delta_theta_openloop_to_smo;

        // int32_t		delta_theta_openloop_to_smo1;

        // uint16_t	openloop_theta;

        // uint16_t	smo_theta;
    } Sw;

    struct
    {
        uint16_t	Final_Curr;
        uint16_t    Transition_to_Run_Cnt;
    } Transition;

    struct
    {
        uint16_t 	Ekp;
        uint16_t	Iq_Final_Curr;
        int16_t		Id_Final_Curr;
        int16_t		Id_req;
        int16_t     Compensation;
        uint16_t    Kslf;
        uint16_t 	theta_compensation_final;
        uint16_t    theta_compensation_tmp;
    } Run;

} struct_Para;

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
//---------------------------------------------------------------------------/

/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source) */
/*****************************************************************************/
extern volatile Union_Fault				u_Fault_Flag;
extern volatile SystStates 				SYSTEM_STATE;
extern volatile MotorStates 			MOTOR_STATE;
//extern struct_Base_Value					stru_base_value;
//extern struct_SpeedRamp						stru_Speed_Ctrl;
//extern struct_ADC_Sample 					stru_Sample;
extern struct_FOC_Ctrl						stru_FOC;
extern struct_Basic_Para					Basic;
//extern struct_Config							Config;
extern struct_Falg 								Flag;

extern struct_PI			stru_PI_SpdLoop;

//extern struct_Time    						Time;
//extern struct_Count   						Count;

//extern struct_MotCtrl_Para				Para;
//extern struct_Capture							Capture;
//extern volatile FaultCode			   	MOTOR_FAULT_CODE;
//extern struct_MotorRuning					Motor_Actual;
//extern struct_BusVolt_Comp				stru_BusVolt_Comp;

//extern int32_t                    wObseFs;
//extern int32_t                    wObseGs;
//extern Union_DWord								IbusFilt_Q32;
//extern struct_Smo_Start						stru_SmoStart;
//extern struct_OutLoop 						stru_OutLoop;
//extern struct_FaultProtect				stru_Fault;
//extern struct_UserSet    					UserPowerSet;

extern struct_FOC_Start						stru_FOC_Start;
extern struct_Para							stru_Para;
extern uint16_t 							probe_switch;
extern uint8_t 								Sw_to_Transition_Flag;
extern uint8_t 								Transition_to_Run_Flag;
extern uint8_t 								Start_to_Sw_Flag;
/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source) */
/*****************************************************************************/
void	MC_BaseValue_Init(void);
void 	MC_MotorInit_Process(void);
void 	System_Control(void);
void 	MC_MotorFault_Check(void);
void Get_ADC_Offset(void);
void 	MC_MotorSpeed_Calc(void);
void 	MC_Speed_Control(void);
void 	MC_Clear_FaultMessage(void);
void 	SysTick_1ms(void);
void 	UartView(s16 view1, s16 view2, s16 view3, s16 view4);
void 	User_Motor_On(void);
void 	User_Motor_Off(void);
void 	User_Motor_FRControl(u8 Input_Val);
void 	User_MotorSpeed_Set(s32 Input_Val);
void 	System_Modules_off(void);
void 	User_Speed_Control(void);
void 	User_Speed_Capture(void);
void 	User_CapMode2_Handle(void);
void 	User_Speed_Out(void);
void 	CCP_Capture_Init(void);
void 	CCP_PWM_Init(void);
void	CCP_PWM_GPIO(uint8_t bMode, uint8_t bLevle);
void 	User_Sleep_Manage(void);
void	Motor_Actual_Calculate(void);
void	GET_ADC_REF_VAL(void);
void	GET_ADC0_REF_VAL(void);
void  WDT_Restart(void);
void 	GET_BANDGAP_VAL(void);
void FOC_BusVolt_Compensation(s32 RealVoltage);

void OutLoop_ParaInit(void);
void Power_Loop_Ctrl(void);
void Ibus_Loop_Ctrl(void);
void Speed_Loop_Ctrl(void);
void VQ_Loop_Ctrl(void);
void CurrLoop_Ctrl(void);
void OutLoop_CtrlInit(void);

void MC_Faultara_Init(void);
void Fault_Clear_RestartTimes(void);
void Fault_ReStart_Process(void);
void Fault_Show_Source(void);
void Sw_to_Transition_Check(void);
void Transition_to_Run_Check(void);
u8 MC_KeBlock_Cal(s32 FiltRatio, s32 Factor);

void Function_PI_Cal(struct_PI *PI, s32 Ref, s32 Feedbk);


void FOC_RotorPosition_Init(void);
#endif	/* __MOTOR_CONTROL_H__ */

