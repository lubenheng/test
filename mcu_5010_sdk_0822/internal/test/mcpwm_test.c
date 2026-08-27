#include "platform_common.h"

#include "test.h"

//#include "mcpwm_type.h"
#include "mcpwm_reg.h"
#include "mcpwm.h"
#include "mcpwm_hal.h"

#include "mcpwm_test.h"
#include "cmdline_internal.h"

#include "gpio_type.h"
#include "gpio_hal.h"

#include "hall_reg.h"
#include "qep_reg.h"
//#include "hall_hal.h"

//三相FOC电机控制
#include "mc_foc.h"
#include "base_define.h"

//三相BLDC电机控制
#include "bldc_motor_control.h"
#include "bldc_motor_state.h"
#include "bldc_motor_mlib.h"



STR_PubDatMDHD mcpwm0_pMdToHd;
STR_PubDatMDHD mcpwm1_pMdToHd;

mcpwm_shadow  stru_mcpwm_shadow;



extern uint16_t g_hall_cnt; 



uint16_t mcpwm0_duty_control_interface = 4096; 
uint16_t mcpwm0_callback_test_cnt = 0;
uint16_t mcpwm0_dead_time_p = EPWM_DT;
uint16_t mcpwm0_dead_time_n = EPWM_DT;
volatile uint32_t hall_value1 = 0;
uint16_t MC_RunStop = 0;
void mcpwm0_callback_duty_test(mcpwm0_int_source_e source)
{
	gpio_output_set(GPIO_C,GPIO_PIN_9,GPIO_HIGH_LEVEL);
	
	mcpwm0_callback_test_cnt++;

#if 0	
	uint16_t duty_a,duty_b,duty_c;

	mcpwm0_pMdToHd.mMcTmr.eChannel = E_PHASE_A_B;  //采样点配置
	mcpwm0_pMdToHd.mMcTmr.nuPoint0 = MCPWM0_TH0;	//采样点配置为最高点
	mcpwm0_pMdToHd.mMcTmr.nuPoint1 = MCPWM0_TH0;	//采样点配置为最高点
	
	duty_a = mcpwm0_duty_control_interface;
	duty_b = mcpwm0_duty_control_interface;
	duty_c = mcpwm0_duty_control_interface;
	
	mcpwm0_pMdToHd.mMcPwm.nuTimePhAN = -(duty_a * EPWM_PERIOD>>15);
	mcpwm0_pMdToHd.mMcPwm.nuTimePhA  =   duty_a * EPWM_PERIOD>>15;
	
	mcpwm0_pMdToHd.mMcPwm.nuTimePhBN = -(duty_b * EPWM_PERIOD>>15);
	mcpwm0_pMdToHd.mMcPwm.nuTimePhB  =   duty_b * EPWM_PERIOD>>15;
	
	mcpwm0_pMdToHd.mMcPwm.nuTimePhCN = -(duty_c * EPWM_PERIOD>>15);
	mcpwm0_pMdToHd.mMcPwm.nuTimePhC  =   duty_c * EPWM_PERIOD>>15;
	
	
  MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */
	
	MCPWM0_DTH00 = mcpwm0_dead_time_p; // MCPWM CH0/1/2 P 通道死区宽度设置为0
	MCPWM0_DTH01 = mcpwm0_dead_time_n; // MCPWM CH0/1/2 N 通道死区宽度设置为0
	
  MCPWM0_RegUpdate(&mcpwm0_pMdToHd);

	MCPWM0_PRT		= 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */
	
#endif

#if  1 
	/* 三相FOC电机控制任务 */
	FOC_Control_Process();
	
#else
	/* 三相BLDC电机控制任务 */
	//MC_DutyBuf_Set();  				/*调速函数*/
    //MC_UC_Update();					/*电机层返回数据*/
    //MC_Machine();						/*电机层状态机*/
	//mcpwm0_HPWM_LON_openloop_test();
	//mcpwm0_HPWM_LON_openloop_mask_test();
	//mcpwm0_complement_HPWM_LON_openloop_test();
	//mcpwm0_complement_HPWM_LON_openloop_mask_test();
	mcpwm0_HPWM_LON_hall_uvw_test();

#endif

	gpio_output_set(GPIO_C,GPIO_PIN_9,GPIO_LOW_LEVEL);

#if 0
	hall_value1 = 1*GPIO_HALL_U + 2*GPIO_HALL_V + 4*GPIO_HALL_W;

	// 交换第0位与第2位
	volatile uint32_t reg = HALL->INFO.HALLFLTDATA;
	volatile uint32_t t = ((reg >> 2) ^ (reg >> 0)) & 1;
	reg ^= (t << 2) | (t << 0);
	reg &= 0x7;
	
	UartView(hall_value1*100, ((HALL->WIDTH>>4)&0xffff), (HALL->INFO.HALLRAWDATA)*100, (reg*100));
	UartView(reg*100, ((HALL->WIDTH>>4)&0xffff), (HALL->INFO.HALLRAWDATA)*100, (HALL->INFO.HALLFLTDATA*100));
#endif

	//UartView(QEP->CNT.CNT,0,0,0);
	UartView(mcpwm0_callback_test_cnt, mcpwm0_callback_test_cnt, mcpwm0_callback_test_cnt, mcpwm0_callback_test_cnt);
}

uint16_t mcpwm1_duty_control_interface = 16384; 
uint16_t mcpwm1_callback_test_cnt = 0;
uint16_t mcpwm1_dead_time_p = EPWM_DT;
uint16_t mcpwm1_dead_time_n = EPWM_DT;
void mcpwm1_callback_duty_test(mcpwm1_int_source_e source)
{
    mcpwm1_callback_test_cnt++;
	
	uint16_t duty_a,duty_b,duty_c;

	mcpwm1_pMdToHd.mMcTmr.eChannel = E_PHASE_A_B;  	//采样点配置
	mcpwm1_pMdToHd.mMcTmr.nuPoint0 = MCPWM0_TH1;	//采样点配置为最高点
	mcpwm1_pMdToHd.mMcTmr.nuPoint1 = MCPWM0_TH1;	//采样点配置为最高点
	
	duty_a = mcpwm1_duty_control_interface;
	duty_b = mcpwm1_duty_control_interface;
	duty_c = mcpwm1_duty_control_interface;
	
	mcpwm1_pMdToHd.mMcPwm.nuTimePhAN = -(duty_a * EPWM_PERIOD>>15);
	mcpwm1_pMdToHd.mMcPwm.nuTimePhA = duty_a * EPWM_PERIOD >> 15;

	mcpwm1_pMdToHd.mMcPwm.nuTimePhBN = -(duty_b * EPWM_PERIOD >> 15);
	mcpwm1_pMdToHd.mMcPwm.nuTimePhB = duty_b * EPWM_PERIOD >> 15;

	mcpwm1_pMdToHd.mMcPwm.nuTimePhCN = -(duty_c * EPWM_PERIOD >> 15);
	mcpwm1_pMdToHd.mMcPwm.nuTimePhC = duty_c * EPWM_PERIOD >> 15;

	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */

	MCPWM0_DTH10 = mcpwm0_dead_time_p; // MCPWM CH0/1/2 P 通道死区宽度设置为0
	MCPWM0_DTH11 = mcpwm0_dead_time_n; // MCPWM CH0/1/2 N 通道死区宽度设置为0

	MCPWM1_RegUpdate(&mcpwm0_pMdToHd);

	MCPWM0_PRT = 0x0000CAFE; /* write any value other than 0xDEAD to enable write protection */
}


/*
将第一次初始化的值记录到结构体stru_mcpwm_shadow
*/
void mcpwm_soft_init_copy()
{
	/* mcpwm0/1 period parameter */
	stru_mcpwm_shadow.TH0 = MCPWM0_TH0;
	stru_mcpwm_shadow.TH1 = MCPWM0_TH1;
	
	/* mcpwm0/1 deadtime parameter */
	stru_mcpwm_shadow.DeadTimeCH012P = (MCPWM0_DTH01 & 0x3FF);
	stru_mcpwm_shadow.DeadTimeCH012N = (MCPWM0_DTH00 & 0x3FF);
	stru_mcpwm_shadow.DeadTimeCH345P = (MCPWM0_DTH11 & 0x3FF);
	stru_mcpwm_shadow.DeadTimeCH345N = (MCPWM0_DTH10 & 0x3FF);
	
	/* mcpwm0 compare parameter */
	stru_mcpwm_shadow.TH00 = MCPWM0_TH00;
	stru_mcpwm_shadow.TH01 = MCPWM0_TH01;
	stru_mcpwm_shadow.TH10 = MCPWM0_TH10;
	stru_mcpwm_shadow.TH11 = MCPWM0_TH11;
	stru_mcpwm_shadow.TH20 = MCPWM0_TH20;
	stru_mcpwm_shadow.TH21 = MCPWM0_TH21;
	
	/* mcpwm1 compare parameter */
	stru_mcpwm_shadow.TH30 = MCPWM0_TH30;
	stru_mcpwm_shadow.TH31 = MCPWM0_TH31;
	stru_mcpwm_shadow.TH40 = MCPWM0_TH40;
	stru_mcpwm_shadow.TH41 = MCPWM0_TH41;
	stru_mcpwm_shadow.TH50 = MCPWM0_TH50;
	stru_mcpwm_shadow.TH51 = MCPWM0_TH51;
}

/*
argv[1]:  0->mcpwm0,1->mcpmw1
argv[2]:  period,unit:Hz
argc[3]:  TH00/TH30,duty cycle,[0%,100%]->[0,32768]
argc[4]:  TH01/TH31
argc[5]:  TH10/TH40
argc[6]:  TH11/TH41
argc[7]:  TH20/TH50
argc[8]:  TH21/TH51
argc[9]:  DTH01_P/DTH11_P,Uint:us
argc[10]: DTH00_N/DTH10_N,Uint:us
*/

void mcpwm_parameter_set(int argc, char *argv[])
{
	uint32_t pwm_instance;
	uint32_t stru_mcpwm_shadow_period_temp;
	
	if (argc < 11U)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);
		return;
	}
	
	
//	INFO("mcpwm_test argc= %d\n", argc);
	
	for(uint16_t i=0;i< argc;i++)
	{
		INFO("argv[%d]:%s\n", i,argv[i]);

	}
	
	
	pwm_instance = get_arg_ulong(argv[1]);
	
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */

	/* 图 14-7MCPWM 时序 TH<n>0 和 TH<n>1-中心对齐模式 */
	MCPWM0_IO01 = CLEAR_BIT(MCPWM0_IO01,BIT7);   //complement mode
	MCPWM0_IO01 = CLEAR_BIT(MCPWM0_IO01,BIT15);  //complement mode
	MCPWM0_IO23 = CLEAR_BIT(MCPWM0_IO23,BIT7);   //complement mode
	MCPWM0_IO23 = CLEAR_BIT(MCPWM0_IO23,BIT15);  //complement mode
	MCPWM0_IO45 = CLEAR_BIT(MCPWM0_IO45,BIT7);   //complement mode
	MCPWM0_IO45 = CLEAR_BIT(MCPWM0_IO45,BIT15);  //complement mode
	MCPWM0_PP   = 0x00; /* normal mode */

	if(pwm_instance == 0)
	{
		stru_mcpwm_shadow.TH0 = get_arg_ulong(argv[2]);
		if(stru_mcpwm_shadow.TH0 == 0U)
		{
			INFO("wrong pwm period\n");
			return;
		}
		else
		{
			stru_mcpwm_shadow_period_temp = (MCU_CLK>>1)/stru_mcpwm_shadow.TH0;
		}
		
		if(stru_mcpwm_shadow_period_temp > 65535U) /* 当频率太低时将MCPWM的时钟进行8分频 */
		{
			MCPWM0_TCLK |= (BIT0 + BIT1);
			stru_mcpwm_shadow.TH0 = stru_mcpwm_shadow_period_temp>>3;
    }
		else
		{
			MCPWM0_TCLK &= ~(BIT0 + BIT1);
			stru_mcpwm_shadow.TH0 = stru_mcpwm_shadow_period_temp;
		}
				
		/* mcpwm0 compare parameter */
		stru_mcpwm_shadow.TH00 = ((get_arg_long(argv[3])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH01 = ((get_arg_long(argv[4])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH10 = ((get_arg_long(argv[5])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH11 = ((get_arg_long(argv[6])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH20 = ((get_arg_long(argv[7])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH21 = ((get_arg_long(argv[8])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		
		
		/* mcpwm0/1 deadtime parameter */
		stru_mcpwm_shadow.DeadTimeCH012P = get_arg_ulong(argv[9])*EPWM_Tus;
		stru_mcpwm_shadow.DeadTimeCH012N = get_arg_ulong(argv[10])*EPWM_Tus;
		
		/*************************************************************************/
		#if 1
		MCPWM0_TH0 = stru_mcpwm_shadow.TH0;
	
		MCPWM0_TH00 = stru_mcpwm_shadow.TH00;
		MCPWM0_TH01 = stru_mcpwm_shadow.TH01;
		MCPWM0_TH10 = stru_mcpwm_shadow.TH10;
		MCPWM0_TH11 = stru_mcpwm_shadow.TH11;
		MCPWM0_TH20 = stru_mcpwm_shadow.TH20;
		MCPWM0_TH21 = stru_mcpwm_shadow.TH21;
		
		MCPWM0_DTH00 = stru_mcpwm_shadow.DeadTimeCH012P; // MCPWM CH0/1/2 P 通道死区宽度设置为0
		MCPWM0_DTH01 = stru_mcpwm_shadow.DeadTimeCH012N; // MCPWM CH0/1/2 N 通道死区宽度设置为0
		#endif
		/*************************************************************************/
		
		
	}
	else if(pwm_instance == 1U)
	{
		stru_mcpwm_shadow.TH1 = get_arg_ulong(argv[2]);
		if(stru_mcpwm_shadow.TH1 == 0U)
		{
			INFO("wrong pwm period\n");
    return;
}
		else
		{
			stru_mcpwm_shadow_period_temp = (MCU_CLK>>1)/stru_mcpwm_shadow.TH1;
		}	

		if(stru_mcpwm_shadow_period_temp > 65535U) /* 当频率太低时将MCPWM的时钟进行8分频 */
		{
			MCPWM0_TCLK |= (BIT0 + BIT1);
			stru_mcpwm_shadow.TH1 = stru_mcpwm_shadow_period_temp>>3;
		}
		else
		{
			MCPWM0_TCLK &= ~(BIT0 + BIT1);
			stru_mcpwm_shadow.TH1 = stru_mcpwm_shadow_period_temp;
		}
				
		/* mcpwm0 compare parameter */
		stru_mcpwm_shadow.TH30 = ((get_arg_long(argv[3])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH31 = ((get_arg_long(argv[4])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH40 = ((get_arg_long(argv[5])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH41 = ((get_arg_long(argv[6])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH50 = ((get_arg_long(argv[7])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH51 = ((get_arg_long(argv[8])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		
		/* mcpwm0/1 deadtime parameter */
		stru_mcpwm_shadow.DeadTimeCH345P = get_arg_ulong(argv[9])*EPWM_Tus;
		stru_mcpwm_shadow.DeadTimeCH345N = get_arg_ulong(argv[10])*EPWM_Tus;
		
		/*************************************************************************/
		#if 1
		MCPWM0_TH1 = stru_mcpwm_shadow.TH1;
	
		MCPWM0_TH30 = stru_mcpwm_shadow.TH30;
		MCPWM0_TH31 = stru_mcpwm_shadow.TH31;
		MCPWM0_TH40 = stru_mcpwm_shadow.TH40;
		MCPWM0_TH41 = stru_mcpwm_shadow.TH41;
		MCPWM0_TH50 = stru_mcpwm_shadow.TH50;
		MCPWM0_TH51 = stru_mcpwm_shadow.TH51;
		
		MCPWM0_DTH10 = stru_mcpwm_shadow.DeadTimeCH345P; // MCPWM CH0/1/2 P 通道死区宽度设置为0
		MCPWM0_DTH11 = stru_mcpwm_shadow.DeadTimeCH345N; // MCPWM CH0/1/2 N 通道死区宽度设置为0
		
#endif
		/*************************************************************************/
	}
	else
	{
		INFO("argv[0] mcpwm instance wrong. \n");
}

	MCPWM0_PRT		= 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */

	INFO("mcpwm_parameter_set run success! \n");

}


volatile MCPWM0_IO01_BIT32_TypeDef g_mcpwm_io01_reg = {0};
volatile MCPWM0_IO23_BIT32_TypeDef g_mcpwm_io23_reg = {0};
volatile MCPWM0_IO45_BIT32_TypeDef g_mcpwm_io45_reg = {0};

volatile MASK_BIT32_TypeDef g_mcpwm_mask_combine  = {0}; //通过寄存器组合得到得到的mask寄存器值
volatile MASK_BIT32_TypeDef g_mcpwm_mask_original = {0}; //新需求增加的mask寄存器值
#define BLDC_DIR  	(0)
volatile uint32_t BLDC_CNT = 100;
void mcpwm0_HPWM_LON_openloop_test()
{
	static uint16_t bldc_mcpwm0_cnt = 0;
	static uint8_t sector = 0;

	uint32_t reg_IO01_temp = 0U;
	uint32_t reg_IO23_temp = 0U;
	
	bldc_mcpwm0_cnt++;
	
	
	if(bldc_mcpwm0_cnt == BLDC_CNT)
	{
		bldc_mcpwm0_cnt = 0;
		sector = gNextSectorTab[BLDC_DIR][sector];
	}
	else if(bldc_mcpwm0_cnt > BLDC_CNT)
	{
		bldc_mcpwm0_cnt = 0;
	}
	else
	{
	
	}
	
	//MC_PwmMask_Set(gTwoDriverTab[0][sector);  
 	
	reg_IO01_temp = MCPWM0_IO01; 
	reg_IO23_temp = MCPWM0_IO23;

	reg_IO01_temp |= CH0P_EN_BIT|CH0N_EN_BIT|CH1P_EN_BIT|CH1N_EN_BIT;
	reg_IO01_temp &= ~(CH0P_VALUE_BIT|CH0N_VALUE_BIT|CH1P_VALUE_BIT|CH1N_VALUE_BIT);

	reg_IO23_temp |= CH2P_EN_BIT|CH2N_EN_BIT;
	reg_IO23_temp &= ~(CH2P_VALUE_BIT|CH2N_VALUE_BIT);
	
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */

	if(BLDC_DIR == 0)  //0：正转 1：反转
	{
		switch (sector)
		{
		case 0: //(AH_PWM | BL)// DRIVE_A_B, // 驱动AB读取C相，CB变为AC，C从高下降到低 
			CLEAR_BIT(reg_IO01_temp,CH0P_EN_BIT);
			SET_BIT(reg_IO01_temp,CH1N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;
		case 1: //(AH_PWM | CL) // DRIVE_A_C, // 驱动AC读取B相，AB变为BC，B从低上升到高 
			CLEAR_BIT(reg_IO01_temp,CH0P_EN_BIT);
			SET_BIT(reg_IO23_temp,CH2N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;
		case 2: //(BH_PWM | CL) // DRIVE_B_C, // 驱动BC读取A相，AC变为BA，A从高下降到低 
			CLEAR_BIT(reg_IO01_temp,CH1P_EN_BIT);
			SET_BIT(reg_IO23_temp,CH2N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;
		case 3: //(BH_PWM | AL) // DRIVE_B_A, // 驱动BA读取C相，BC变为CA，C从低上升到高  
			CLEAR_BIT(reg_IO01_temp,CH1P_EN_BIT);
			SET_BIT(reg_IO01_temp,CH0N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;
		case 4: //(CH_PWM | AL) // DRIVE_C_A, // 驱动CA读取B相，BA变为CB，B从高下降到低 
			CLEAR_BIT(reg_IO23_temp,CH2P_EN_BIT);
			SET_BIT(reg_IO01_temp,CH0N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;
		case 5: //(CH_PWM | BL)// DRIVE_C_B, // 驱动CB读取A相，CA变为AB，A从低上升到高 
			CLEAR_BIT(reg_IO23_temp,CH2P_EN_BIT);
			SET_BIT(reg_IO01_temp,CH1N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;

		default:
			break;
		}
	}
	else// 1：反转    
	{
		switch (sector)
		{

		case 0: // DRIVE_B_A, //
			//(BH_PWM | AL) // DRIVE_B_A, // 驱动BA读取C相，BC变为CA，C从低上升到高  
			CLEAR_BIT(reg_IO01_temp,CH1P_EN_BIT);
			SET_BIT(reg_IO01_temp,CH0N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;
		case 1: // DRIVE_C_A,
			//(CH_PWM | AL) // DRIVE_C_A, // 驱动CA读取B相，BA变为CB，B从高下降到低 
			CLEAR_BIT(reg_IO23_temp,CH2P_EN_BIT);
			SET_BIT(reg_IO01_temp,CH0N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;
		case 2: // DRIVE_C_B,
			//(CH_PWM | BL)// DRIVE_C_B, // 驱动CB读取A相，CA变为AB，A从低上升到高 
			CLEAR_BIT(reg_IO23_temp,CH2P_EN_BIT);
			SET_BIT(reg_IO01_temp,CH1N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;
		case 3: // DRIVE_A_B,
			//(AH_PWM | BL)// DRIVE_A_B, // 驱动AB读取C相，CB变为AC，C从高下降到低 
			CLEAR_BIT(reg_IO01_temp,CH0P_EN_BIT);
			SET_BIT(reg_IO01_temp,CH1N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;
		case 4: // DRIVE_A_C,
			//(AH_PWM | CL) // DRIVE_A_C, // 驱动AC读取B相，AB变为BC，B从低上升到高 
			CLEAR_BIT(reg_IO01_temp,CH0P_EN_BIT);
			SET_BIT(reg_IO23_temp,CH2N_VALUE_BIT);
			
			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;
		case 5: // DRIVE_B_C
			//(BH_PWM | CL) // DRIVE_B_C, // 驱动BC读取A相，AC变为BA，A从高下降到低 
			CLEAR_BIT(reg_IO01_temp,CH1P_EN_BIT);
			SET_BIT(reg_IO23_temp,CH2N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;

		default:
			break;
		}
	}

	MCPWM0_PRT		= 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */

	mcpwm_mask_control_combine();
	UartView((g_mcpwm_mask_combine.w&0xFFFF),(g_mcpwm_mask_original.w&0xFFFF), ((g_mcpwm_mask_combine.w>>16)&0xFFFF), ((g_mcpwm_mask_original.w>>16)&0xFFFF));
}

void mcpwm0_HPWM_LON_openloop_mask_test()
{
	static uint16_t bldc_mcpwm0_cnt = 0;
	static uint8_t sector = 0;
	
	
	bldc_mcpwm0_cnt++;
	
	
	if(bldc_mcpwm0_cnt == BLDC_CNT)
	{
		bldc_mcpwm0_cnt = 0;
		sector = gNextSectorTab[BLDC_DIR][sector];
	}
	else if(bldc_mcpwm0_cnt > BLDC_CNT)
	{
		bldc_mcpwm0_cnt = 0;
	}
	else
	{
	
	}
	
 	MC_PwmMask_Set(gTwoDriverTab[BLDC_DIR][sector]);  
	
	if(MC_RunStop){Brige_Output_On();}
	else{Brige_Output_Off();}
//	mcpwm_mask_control_combine();
//	UartView((g_mcpwm_mask_combine.w&0xFFFF),(g_mcpwm_mask_original.w&0xFFFF), ((g_mcpwm_mask_combine.w>>16)&0xFFFF), ((g_mcpwm_mask_original.w>>16)&0xFFFF));
}

int16_t bldc_hall_mcpwm0_cnt = 0;

void mcpwm0_HPWM_LON_hall_uvw_test()
{
	volatile uint8_t sector = 0;
	
	bldc_hall_mcpwm0_cnt++;
	
	sector = 1*GPIO_HALL_U + 2*GPIO_HALL_V + 4*GPIO_HALL_W;

	sector = g_Hall_NextSectorTab[BLDC_DIR][sector];
	
	MC_PwmMask_Set(gTwoDriverTab[0][sector]);  

	if(MC_RunStop){Brige_Output_On();}
	else{Brige_Output_Off();}
}



/* 有感HALL 控制通过HALL硬件模块中断进行BLDC电机控制 */
uint16_t hall_motor_enable_cnt0 = 0;
uint16_t hall_motor_enable_cnt1 = 0;
void mcpwm0_bldc_hall_uvw_test(uint16_t hall_motor_enable, uint32_t motor_sector, uint16_t duty)
{
	volatile uint8_t sector = 0;
//	if (hall_motor_enable == 1)
//	{
		Brige_Output_On();
		hall_motor_enable_cnt1++;

		sector = g_Hall_NextSectorTab[BLDC_DIR][motor_sector];

		MC_PwmMask_Set(gTwoDriverTab[0][sector]);

		if (duty >= 32768)
		{
			duty = 32768;
		}

		mcpwm0_pMdToHd.mMcPwm.nuTimePhAN = -(duty * EPWM_PERIOD >> 15);
		mcpwm0_pMdToHd.mMcPwm.nuTimePhA = duty * EPWM_PERIOD >> 15;

		mcpwm0_pMdToHd.mMcPwm.nuTimePhBN = -(duty * EPWM_PERIOD >> 15);
		mcpwm0_pMdToHd.mMcPwm.nuTimePhB = duty * EPWM_PERIOD >> 15;

		mcpwm0_pMdToHd.mMcPwm.nuTimePhCN = -(duty * EPWM_PERIOD >> 15);
		mcpwm0_pMdToHd.mMcPwm.nuTimePhC = duty * EPWM_PERIOD >> 15;

		MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */

		MCPWM0_DTH00 = mcpwm0_dead_time_p; // MCPWM CH0/1/2 P 通道死区宽度设置为0
		MCPWM0_DTH01 = mcpwm0_dead_time_n; // MCPWM CH0/1/2 N 通道死区宽度设置为0

		MCPWM0_RegUpdate(&mcpwm0_pMdToHd);

		MCPWM0_PRT = 0x0000CAFE; /* write any value other than 0xDEAD to enable write protection */
//	}
//	else
//	{
//		MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */
//		INFO("before:  %x\n",REG32(0x40010CB0));
//		delay_1us(200);
//		MCPWM0_FAIL012 &= ~BIT6;
//		INFO("after:  %x\n",REG32(0x40010CB0));
//		delay_1us(200);
//		MCPWM0_CH_DEF = (MCPWM0_CH_DEF & ~MCPWM0_CH_DEF_CH012_MASK) | MCPWM0_CH_DEF_CH012_SET(0x0); 
//		MCPWM0_PRT = 0x0;
//		hall_motor_enable_cnt0++;
//	}
}



void mcpwm0_complement_HPWM_LON_openloop_test()
{
	static uint16_t bldc_mcpwm0_cnt = 0;
	static uint8_t sector = 0;
	
	uint32_t reg_IO01_temp = 0U;
	uint32_t reg_IO23_temp = 0U;
	
	
	bldc_mcpwm0_cnt++;
	//sector = 0;

	if(bldc_mcpwm0_cnt == BLDC_CNT)
	{
		bldc_mcpwm0_cnt = 0;
		sector = gNextSectorTab[BLDC_DIR][sector];
    }
	else if(bldc_mcpwm0_cnt > BLDC_CNT)
		{
		bldc_mcpwm0_cnt = 0;
		}
	else
	{
	
	}
	
	//MC_PwmMask_Set(gTwoDriverTab[0][sector]);  
 	
	reg_IO01_temp = MCPWM0_IO01; 
	reg_IO23_temp = MCPWM0_IO23;

	reg_IO01_temp |= CH0P_EN_BIT|CH0N_EN_BIT|CH1P_EN_BIT|CH1N_EN_BIT;
	reg_IO01_temp &= ~(CH0P_VALUE_BIT|CH0N_VALUE_BIT|CH1P_VALUE_BIT|CH1N_VALUE_BIT);

	reg_IO23_temp |= CH2P_EN_BIT|CH2N_EN_BIT;
	reg_IO23_temp &= ~(CH2P_VALUE_BIT|CH2N_VALUE_BIT);
	
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */

	if(BLDC_DIR == 0)  //0：正转 1：反转
	{
		switch (sector)
		{
		case 0: //(AH_PWM | BL)// DRIVE_A_B, // 驱动AB读取C相，CB变为AC，C从高下降到低 
			CLEAR_BIT(reg_IO01_temp,CH0P_EN_BIT);
			CLEAR_BIT(reg_IO01_temp,CH0N_EN_BIT);
			SET_BIT(reg_IO01_temp,CH1N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;
		case 1: //(AH_PWM | CL) // DRIVE_A_C, // 驱动AC读取B相，AB变为BC，B从低上升到高 
			CLEAR_BIT(reg_IO01_temp,CH0P_EN_BIT);
			CLEAR_BIT(reg_IO01_temp,CH0N_EN_BIT);
			SET_BIT(reg_IO23_temp,CH2N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;
		case 2: //(BH_PWM | CL) // DRIVE_B_C, // 驱动BC读取A相，AC变为BA，A从高下降到低 
			CLEAR_BIT(reg_IO01_temp,CH1P_EN_BIT);
			CLEAR_BIT(reg_IO01_temp,CH1N_EN_BIT);
			SET_BIT(reg_IO23_temp,CH2N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;
		case 3: //(BH_PWM | AL) // DRIVE_B_A, // 驱动BA读取C相，BC变为CA，C从低上升到高  
			CLEAR_BIT(reg_IO01_temp,CH1P_EN_BIT);
			CLEAR_BIT(reg_IO01_temp,CH1N_EN_BIT);
			SET_BIT(reg_IO01_temp,CH0N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;
		case 4: //(CH_PWM | AL) // DRIVE_C_A, // 驱动CA读取B相，BA变为CB，B从高下降到低 
			CLEAR_BIT(reg_IO23_temp,CH2P_EN_BIT);
			CLEAR_BIT(reg_IO23_temp,CH2N_EN_BIT);
			SET_BIT(reg_IO01_temp,CH0N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;
		case 5: //(CH_PWM | BL)// DRIVE_C_B, // 驱动CB读取A相，CA变为AB，A从低上升到高 
			CLEAR_BIT(reg_IO23_temp,CH2P_EN_BIT);
			CLEAR_BIT(reg_IO23_temp,CH2N_EN_BIT);
			SET_BIT(reg_IO01_temp,CH1N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;

		default:
			break;
		}
	}
	else// 1：反转    
	{
		switch (sector)
		{

		case 0: // DRIVE_B_A, //
			//(BH_PWM | AL) // DRIVE_B_A, // 驱动BA读取C相，BC变为CA，C从低上升到高  
			CLEAR_BIT(reg_IO01_temp,CH1P_EN_BIT);
			CLEAR_BIT(reg_IO01_temp,CH1N_EN_BIT);
			SET_BIT(reg_IO01_temp,CH0N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;
		case 1: // DRIVE_C_A,
			//(CH_PWM | AL) // DRIVE_C_A, // 驱动CA读取B相，BA变为CB，B从高下降到低 
			CLEAR_BIT(reg_IO23_temp,CH2P_EN_BIT);
			CLEAR_BIT(reg_IO23_temp,CH2N_EN_BIT);
			SET_BIT(reg_IO01_temp,CH0N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;
		case 2: // DRIVE_C_B,
			//(CH_PWM | BL)// DRIVE_C_B, // 驱动CB读取A相，CA变为AB，A从低上升到高 
			CLEAR_BIT(reg_IO23_temp,CH2P_EN_BIT);
			CLEAR_BIT(reg_IO23_temp,CH2N_EN_BIT);
			SET_BIT(reg_IO01_temp,CH1N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;
		case 3: // DRIVE_A_B,
			//(AH_PWM | BL)// DRIVE_A_B, // 驱动AB读取C相，CB变为AC，C从高下降到低 
			CLEAR_BIT(reg_IO01_temp,CH0P_EN_BIT);
			CLEAR_BIT(reg_IO01_temp,CH0N_EN_BIT);
			SET_BIT(reg_IO01_temp,CH1N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;
		case 4: // DRIVE_A_C,
			//(AH_PWM | CL) // DRIVE_A_C, // 驱动AC读取B相，AB变为BC，B从低上升到高 
			CLEAR_BIT(reg_IO01_temp,CH0P_EN_BIT);
			CLEAR_BIT(reg_IO01_temp,CH0N_EN_BIT);
			SET_BIT(reg_IO23_temp,CH2N_VALUE_BIT);
			
			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;
		case 5: // DRIVE_B_C
			//(BH_PWM | CL) // DRIVE_B_C, // 驱动BC读取A相，AC变为BA，A从高下降到低 
			CLEAR_BIT(reg_IO01_temp,CH1P_EN_BIT);
			CLEAR_BIT(reg_IO01_temp,CH1N_EN_BIT);
			SET_BIT(reg_IO23_temp,CH2N_VALUE_BIT);

			MCPWM0_IO01 = (reg_IO01_temp&0xFFFF);
			MCPWM0_IO23 = (reg_IO23_temp&0xFFFF);
			break;

		default:
			break;
}
	}
	MCPWM0_PRT		= 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */

	// mcpwm_mask_control_combine();
	// UartView((g_mcpwm_mask_combine.w&0xFFFF),(g_mcpwm_mask_original.w&0xFFFF), ((g_mcpwm_mask_combine.w>>16)&0xFFFF), ((g_mcpwm_mask_original.w>>16)&0xFFFF));
}

void mcpwm0_complement_HPWM_LON_openloop_mask_test()
{
	static uint16_t bldc_mcpwm0_cnt = 0;
	static uint8_t sector = 0;
	
	
	bldc_mcpwm0_cnt++;
	//sector = 0;
	
	if(bldc_mcpwm0_cnt == BLDC_CNT)
	{
		bldc_mcpwm0_cnt = 0;
		sector = gNextSectorTab[BLDC_DIR][sector];
	}
	else if(bldc_mcpwm0_cnt > BLDC_CNT)
	{
		bldc_mcpwm0_cnt = 0;
        }
	else
	{
	
	}
	
	MC_PwmMask_Set(gTwoDriverTab[0][sector]);  

// 	mcpwm_mask_control_combine();
// 	UartView((g_mcpwm_mask_combine.w&0xFFFF),(g_mcpwm_mask_original.w&0xFFFF), ((g_mcpwm_mask_combine.w>>16)&0xFFFF), ((g_mcpwm_mask_original.w>>16)&0xFFFF));
 }

 

/* 测试非对称PWM不包含死区 */
void  mcpwm0_callback_asym_pwm_without_deadtime_test()
{

}


/* 测试非对称PWM，含有死区 */
void  mcpwm0_callback_asym_pwm_with_deadtime_test()
{

}
/* SVPWM变化控制周期 */
void mcpwm0_callback_period_test()
{

}


/* 7/5段式SVPWM测试 */
void mcpwm0_callback_5_svpwm_test()
{
   
}


/* SVPWM电机控制时产生引脚或者内部故障 */
void mcpwm0_callback_fault_test()
{

}

void mcpwm0_callback_bldc_HPWM_LON_test()
{

}

void mcpmw0_callback_bldc_HON_LPWM_test()
{

}

void mcpmw0_callback_bldc_HPWM_LPWM_test()
{

}



void mcpwm0_HON_LPWM_test()
{
	

}


void mcpwm0_complement_HON_LPWM_test()
{
	

}





void mcpwm_func1_test(uint32_t test_case, uint32_t para1, uint32_t para2)
{




}	



void mcpwm_func2_test(uint32_t test_case, uint32_t para1, uint32_t para2)
{




}	

void mcpwm_mask_control_combine()
{
	g_mcpwm_mask_original.w = MCPWM0_MASK;
	g_mcpwm_io01_reg.w	= MCPWM0_IO01;
	g_mcpwm_io23_reg.w  = MCPWM0_IO23;
	g_mcpwm_io45_reg.w  = MCPWM0_IO45;

	g_mcpwm_mask_combine.bits.CH0_NS_bit0 = g_mcpwm_io01_reg.bits.CH0_NS_bit2;
	g_mcpwm_mask_combine.bits.CH0_PS_bit1 = g_mcpwm_io01_reg.bits.CH0_PS_bit3;
	g_mcpwm_mask_combine.bits.CH1_NS_bit2 = g_mcpwm_io01_reg.bits.CH1_NS_bit10;
	g_mcpwm_mask_combine.bits.CH1_PS_bit3 = g_mcpwm_io01_reg.bits.CH1_PS_bit11;
	g_mcpwm_mask_combine.bits.CH2_NS_bit4 = g_mcpwm_io23_reg.bits.CH2_NS_bit2;
	g_mcpwm_mask_combine.bits.CH2_PS_bit5 = g_mcpwm_io23_reg.bits.CH2_PS_bit3;

	g_mcpwm_mask_combine.bits.CH3_NS_bit16 = g_mcpwm_io23_reg.bits.CH3_NS_bit10;
	g_mcpwm_mask_combine.bits.CH3_PS_bit17 = g_mcpwm_io23_reg.bits.CH3_PS_bit11;
	g_mcpwm_mask_combine.bits.CH4_NS_bit18 = g_mcpwm_io45_reg.bits.CH4_NS_bit2;
	g_mcpwm_mask_combine.bits.CH4_PS_bit19 = g_mcpwm_io45_reg.bits.CH4_PS_bit3;
	g_mcpwm_mask_combine.bits.CH5_NS_bit20 = g_mcpwm_io45_reg.bits.CH5_NS_bit10;
	g_mcpwm_mask_combine.bits.CH5_PS_bit21 = g_mcpwm_io45_reg.bits.CH5_PS_bit11;

	//--------------------------------------------------------------------------------------------------//

	g_mcpwm_mask_combine.bits.CH0_SCTRLN_bit8 = g_mcpwm_io01_reg.bits.CH0_SCTRLN_bit4;
	g_mcpwm_mask_combine.bits.CH0_SCTRLP_bit9 = g_mcpwm_io01_reg.bits.CH0_SCTRLP_bit5;
	g_mcpwm_mask_combine.bits.CH1_SCTRLN_bit10 = g_mcpwm_io01_reg.bits.CH1_SCTRLN_bit12;
	g_mcpwm_mask_combine.bits.CH1_SCTRLP_bit11 = g_mcpwm_io01_reg.bits.CH1_SCTRLP_bit13;
	g_mcpwm_mask_combine.bits.CH2_SCTRLN_bit12 = g_mcpwm_io23_reg.bits.CH2_SCTRLN_bit4;
	g_mcpwm_mask_combine.bits.CH2_SCTRLP_bit13 = g_mcpwm_io23_reg.bits.CH2_SCTRLP_bit5;

	g_mcpwm_mask_combine.bits.CH3_SCTRLN_bit24 = g_mcpwm_io23_reg.bits.CH3_SCTRLN_bit12;
	g_mcpwm_mask_combine.bits.CH3_SCTRLP_bit25 = g_mcpwm_io23_reg.bits.CH3_SCTRLP_bit13;
	g_mcpwm_mask_combine.bits.CH4_SCTRLN_bit26 = g_mcpwm_io45_reg.bits.CH4_SCTRLN_bit4;
	g_mcpwm_mask_combine.bits.CH4_SCTRLP_bit27 = g_mcpwm_io45_reg.bits.CH4_SCTRLP_bit5;
	g_mcpwm_mask_combine.bits.CH5_SCTRLN_bit28 = g_mcpwm_io45_reg.bits.CH5_SCTRLN_bit12;
	g_mcpwm_mask_combine.bits.CH5_SCTRLP_bit29 = g_mcpwm_io45_reg.bits.CH5_SCTRLP_bit13;

}


void mcpwm0_gpio_init(void)
{
	gpio_settings_config_t setting_cfg;

	/* MCPWM_CH0P, PTA6 PIN  */
	setting_cfg.gpio_port = GPIO_A;
	setting_cfg.gpio_pin = GPIO_PIN_6;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_15;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);


	/* MCPWM_CH1P, PTA7 PIN  */
	setting_cfg.gpio_port = GPIO_A;
	setting_cfg.gpio_pin = GPIO_PIN_7;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_15;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);
	
	
	/* MCPWM_CH2P, PTA8 PIN  */
	setting_cfg.gpio_port = GPIO_A;
	setting_cfg.gpio_pin = GPIO_PIN_8;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_15;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);
	
	
	
	/* MCPWM_CH0N, PTA9 PIN  */
	setting_cfg.gpio_port = GPIO_A;
	setting_cfg.gpio_pin = GPIO_PIN_9;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_15;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);
	
	/* MCPWM_CH1N, PTA10 PIN  */
	setting_cfg.gpio_port = GPIO_A;
	setting_cfg.gpio_pin = GPIO_PIN_10;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_15;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);
	
	/* MCPWM_CH2N, PTA11 PIN  */
	setting_cfg.gpio_port = GPIO_A;
	setting_cfg.gpio_pin = GPIO_PIN_11;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_15;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);
	
}


void mcpwm1_gpio_init(void)
{
	gpio_settings_config_t setting_cfg;

#if 0 /* First Group MCPWM1 GPIO Config */

	/* MCPWM_CH5N, PTA15 PIN  */
	setting_cfg.gpio_port = GPIO_A;
	setting_cfg.gpio_pin = GPIO_PIN_15;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_5;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);

	/* MCPWM_CH4N, PTB0 PIN  */
	setting_cfg.gpio_port = GPIO_B;
	setting_cfg.gpio_pin = GPIO_PIN_0;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_5;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);
	
	/* MCPWM_CH3N, PTB1 PIN  */
	setting_cfg.gpio_port = GPIO_B;
	setting_cfg.gpio_pin = GPIO_PIN_1;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_5;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);
	
	
	
	/* MCPWM_CH5P, PTB2 PIN  */
	setting_cfg.gpio_port = GPIO_B;
	setting_cfg.gpio_pin = GPIO_PIN_2;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_5;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);
	
	/* MCPWM_CH4P, PTB3 PIN  */
	setting_cfg.gpio_port = GPIO_B;
	setting_cfg.gpio_pin = GPIO_PIN_3;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_5;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);
	
	/* MCPWM_CH3P, PTB4 PIN  */
	setting_cfg.gpio_port = GPIO_B;
	setting_cfg.gpio_pin = GPIO_PIN_4;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_5;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);	

#else /* Second Group MCPWM1 GPIO Config */


	/* MCPWM_CH0N, PTB13 PIN  */
	setting_cfg.gpio_port = GPIO_B;
	setting_cfg.gpio_pin = GPIO_PIN_13;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_6;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);
	
	
	/* MCPWM_CH0P, PTB14 PIN  */
	setting_cfg.gpio_port = GPIO_B;
	setting_cfg.gpio_pin = GPIO_PIN_14;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_6;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);


	/* MCPWM_CH1N, PTB15 PIN  */
	setting_cfg.gpio_port = GPIO_B;
	setting_cfg.gpio_pin = GPIO_PIN_15;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_6;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);


	/* MCPWM_CH1P, PTC0 PIN  */
	setting_cfg.gpio_port = GPIO_C;
	setting_cfg.gpio_pin = GPIO_PIN_0;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_6;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);
	
	/* MCPWM_CH2N, PTC1 PIN  */
	setting_cfg.gpio_port = GPIO_C;
	setting_cfg.gpio_pin = GPIO_PIN_1;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_6;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);	
	
	
	/* MCPWM_CH2P, PTC2 PIN  */
	setting_cfg.gpio_port = GPIO_C;
	setting_cfg.gpio_pin = GPIO_PIN_2;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_6;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);
	
#endif

}

void mcpwm_fault_gpio_init(void)
{
	gpio_settings_config_t setting_cfg;


	/* MCPWM_BRK0, PTA3 PIN  */
	setting_cfg.gpio_port = GPIO_A;
	setting_cfg.gpio_pin = GPIO_PIN_3;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_8;
	setting_cfg.gpio_dir = GPIO_DIR_INPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);

	/* MCPWM_BRK1, PTA4 PIN  */
	setting_cfg.gpio_port = GPIO_A;
	setting_cfg.gpio_pin = GPIO_PIN_4;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_8;
	setting_cfg.gpio_dir = GPIO_DIR_INPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);
}

/*
argv[1]:  0->mcpwm0,1->mcpmw1
argv[2]:  interrupt source(mcpwm0_int_source_e)
*/
void mcpwm_interrupt_source_test(int argc, char *argv[])
{
	uint32_t pwm_instance;
	uint32_t interrupt_source_en;
	
	// if (argc < 11U)
	// {
	// 	INFO("wrong cmd_format:%s\n", argv[0]);
	// 	return;
	// }
	
	for(uint16_t i=0;i< argc;i++)
	{
		INFO("argv[%d]:%s\n", i,argv[i]);

	}
	
	
	pwm_instance = get_arg_ulong(argv[1]);
	interrupt_source_en = get_arg_ulong(argv[2]);

	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */
	if(pwm_instance == 0)
	{
		if(interrupt_source_en >15 ){MCPWM0_EIE = (BIT0 << (interrupt_source_en - 16));}
		else{MCPWM0_IE0 = (BIT0 << interrupt_source_en);}
		INFO("mcpwm0_interrupt_source run success! \n");
	}
	else if(pwm_instance == 1U)
	{
		if(interrupt_source_en >15 ){MCPWM0_EIE = (BIT0 << (interrupt_source_en-16));}
		else{MCPWM0_IE1 = (BIT0 << interrupt_source_en);}
		INFO("mcpwm1_interrupt_source run success! \n");
	}
	else
	{
		INFO("argv[0] mcpwm instance wrong. \n");
	}

	MCPWM0_PRT = 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */

	

}

void mcpwm0_interrupt_test_callback(mcpwm0_int_source_e source)
{
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */
	
	MCPWM0_IE0 = 0;  /* disable all mcpwm0 interrupt */
	switch (source)
	{
	case MCPWM0_BIT0_T0_IF:
		INFO("mcpwm0 T0_IE\n");
		break;
	case MCPWM0_BIT1_T1_IF:
		INFO("mcpwm0 T1_IE\n");
		break;
	case MCPWM0_BIT2_TH00_IF:
		INFO("mcpwm0 TH00\n");
		break;
	case MCPWM0_BIT3_TH01_IF:
		INFO("mcpwm0 TH01\n");
		break;
	case MCPWM0_BIT4_TH10_IF:
		INFO("mcpwm0 TH10\n");
		break;
	case MCPWM0_BIT5_TH11_IF:
		INFO("mcpwm0 TH11\n");
		break;
	case MCPWM0_BIT6_TH20_IF:
		INFO("mcpwm0 TH20\n");
		break;
	case MCPWM0_BIT7_TH21_IF:
		INFO("mcpwm0 TH21\n");
		break;
	case MCPWM0_BIT10_TMR0_IF:
		INFO("mcpwm0 TMR0\n");
		break;
	case MCPWM0_BIT11_TMR1_IF:
		INFO("mcpwm0 TMR1\n");
		break;
	case MCPWM0_BIT12_TMR2_IF:
		INFO("mcpwm0 TMR2\n");
		break;
	case MCPWM0_BIT13_TMR3_IF:
		INFO("mcpwm0 TMR3\n");
		break;
	case MCPWM0_BIT14_UP_IF:
		INFO("mcpwm0 UP_IF\n");
		break;
	case MCPWM0_BIT20_FAIL0_IF:
		INFO("mcpwm0 FAIL0_IF\n");
		break;
	case MCPWM0_BIT21_FAIL1_IF:
		INFO("mcpwm0 FAIL1_IF\n");
		break;
	case MCPWM0_BIT22_FAIL2_IF:
		INFO("mcpwm0 FAIL2_IF\n");
		break;
	case MCPWM0_BIT23_FAIL3_IF:
		INFO("mcpwm0 FAIL3_IF\n");
		break;
	case MCPWM0_BIT24_ZCS0_IF:
		INFO("mcpwm0 ZCS0_IF\n");
		break;
	case MCPWM0_BIT25_ZCS1_IF:
		INFO("mcpwm0 ZCS1_IF\n");
		break;

	default:
		INFO("mcpwm0 wrong\n");
		break;
	}
	MCPWM0_PRT = 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */
}

void mcpwm1_interrupt_test_callback(mcpwm1_int_source_e source)
{
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */
	
	MCPWM0_IE1 = 0;  /* disable all mcpwm1 interrupt */
	switch (source)
	{
	case MCPWM1_BIT0_T0_IF:
		INFO("mcpwm1 T0_IE\n");
		break;
	case MCPWM1_BIT1_T1_IF:
		INFO("mcpwm1 T1_IE\n");
		break;
	case MCPWM1_BIT2_TH30_IF:
		INFO("mcpwm1 TH30\n");
		break;
	case MCPWM1_BIT3_TH31_IF:
		INFO("mcpwm1 TH31\n");
		break;
	case MCPWM1_BIT4_TH40_IF:
		INFO("mcpwm1 TH40\n");
		break;
	case MCPWM1_BIT5_TH41_IF:
		INFO("mcpwm1 TH41\n");
		break;
	case MCPWM1_BIT6_TH50_IF:
		INFO("mcpwm1 TH50\n");
		break;
	case MCPWM1_BIT7_TH51_IF:
		INFO("mcpwm1 TH51\n");
		break;
	case MCPWM1_BIT12_TMR2_IF:
		INFO("mcpwm1 TMR2\n");
		break;
	case MCPWM1_BIT13_TMR3_IF:
		INFO("mcpwm1 TMR3\n");
		break;
	case MCPWM1_BIT14_UP_IF:
		INFO("mcpwm1 UP_IF\n");
		break;
	case MCPWM1_BIT20_FAIL0_IF:
		INFO("mcpwm1 FAIL0_IF\n");
		break;
	case MCPWM1_BIT21_FAIL1_IF:
		INFO("mcpwm1 FAIL1_IF\n");
		break;
	case MCPWM1_BIT22_FAIL2_IF:
		INFO("mcpwm1 FAIL2_IF\n");
		break;
	case MCPWM1_BIT23_FAIL3_IF:
		INFO("mcpwm1 FAIL3_IF\n");
		break;
	case MCPWM1_BIT24_ZCS0_IF:
		INFO("mcpwm1 ZCS0_IF\n");
		break;
	case MCPWM1_BIT25_ZCS1_IF:
		INFO("mcpwm1 ZCS1_IF\n");
		break;
	default:
		INFO("mcpwm1 wrong\n");
		break;
	}
	MCPWM0_PRT = 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */
}


/* 
argv[1](channel) : channel0------channel5 
argv[2](p/n)	 : (0->channel N) or  (1->chanenl P)
argv[3](inverse) : 0->normal signal output    1->inverse signal output
*/
void mcpwm_channel_n_p_inverse_test(int argc, char *argv[])
{
	uint32_t channel;
	uint32_t channel_n_p;     //(0->channel N) or  (1->chanenl P)
	uint32_t inverse;         // 0->normal signal output    1->inverse signal output

	if (argc < 4U)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);
		return;
	}
	for(uint16_t i=0;i< argc;i++){INFO("argv[%d]:%s\n", i,argv[i]);}

	channel = get_arg_ulong(argv[1]);
	channel_n_p = get_arg_ulong(argv[2]);
	inverse = get_arg_ulong(argv[3]);

	mcpwm_channel_n_p_inverse_set(channel,channel_n_p,inverse);

	INFO("mcpwm_channel_n_p_inverse run success! \n");
}

/* 
argv[1](channel) : channel0------channel5 
argv[2](swap)    : 0->not swap channel p/n    1->swap channel p/n
*/
void mcpwm_channel_swap_test(int argc, char *argv[])
{
	uint32_t channel;
	uint32_t channel_swap;    	   //0->not swap channel p/n    1->swap channel p/n

	if (argc < 3U)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);
		return;
	}
	for(uint16_t i=0;i< argc;i++){INFO("argv[%d]:%s\n", i,argv[i]);}
	
	channel = get_arg_ulong(argv[1]);
	channel_swap = get_arg_ulong(argv[2]);

	mcpwm_channel_swap_set(channel,channel_swap);

	INFO("mcpwm_channel_swap_set run success! \n");
}

/* 
argv[1](channel) : 0->mcpwm0    1->mcpmw1
argv[2](bool)    : 0->disable defualt value    1->enable default value
argv[3](value)   : [bit5/bit4...bit2/bit1/bit0] last 6 bit for chanel2p/2n/1p/1n/0p/0n  or  chanel5p/5n/4p/4n/3p/3n
*/
void mcpwm_output_default_value_test(int argc, char *argv[])
{
	uint32_t pwm_instance;
	uint32_t default_enable;
	uint32_t default_value;  
	
	if (argc < 4U)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);
		return;
	}
	
	for(uint16_t i=0; i<argc; i++){INFO("argv[%d]:%s\n", i,argv[i]);}

	pwm_instance = get_arg_ulong(argv[1]);
	default_enable = get_arg_ulong(argv[2]);
	default_value = get_arg_ulong(argv[3]);
	
	INFO("default_enable:%d\n", default_enable);
	INFO("default_value:%d\n", default_value);

	if(default_enable == 0)
	{
		MCPWM_CH_DEFAULT(pwm_instance,false,default_value);
	}
	else
	{
		MCPWM_CH_DEFAULT(pwm_instance,true,default_value);
	}

	INFO("mcpwm_output_default_value run success! \n");
}

void mcpwm_center_align_test(int argc, char *argv[])
{
	uint32_t pwm_instance;
	uint32_t stru_mcpwm_shadow_period_temp;
	
	if (argc < 11U)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);
		return;
	}
	
	
//	INFO("mcpwm_test argc= %d\n", argc);
	
	for(uint16_t i=0;i< argc;i++)
	{
		INFO("argv[%d]:%s\n", i,argv[i]);
	}
	
	
	pwm_instance = get_arg_ulong(argv[1]);
	
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */

	/* 图 14-7MCPWM 时序 TH<n>0 和 TH<n>1-中心对齐模式 */
	MCPWM0_IO01 = CLEAR_BIT(MCPWM0_IO01,BIT7);   //complement mode
	MCPWM0_IO01 = CLEAR_BIT(MCPWM0_IO01,BIT15);  //complement mode
	MCPWM0_IO23 = CLEAR_BIT(MCPWM0_IO23,BIT7);   //complement mode
	MCPWM0_IO23 = CLEAR_BIT(MCPWM0_IO23,BIT15);  //complement mode
	MCPWM0_IO45 = CLEAR_BIT(MCPWM0_IO45,BIT7);   //complement mode
	MCPWM0_IO45 = CLEAR_BIT(MCPWM0_IO45,BIT15);  //complement mode
	MCPWM0_PP   = 0x00; /* 关闭推挽模式使能 */

	if(pwm_instance == 0)
	{
		stru_mcpwm_shadow.TH0 = get_arg_ulong(argv[2]);
		if(stru_mcpwm_shadow.TH0 == 0U)
		{
			INFO("wrong pwm period\n");
			return;
		}
		else
		{
			stru_mcpwm_shadow_period_temp = (MCU_CLK>>1)/stru_mcpwm_shadow.TH0;
		}
		
		if(stru_mcpwm_shadow_period_temp > 65535U) /* 当频率太低时将MCPWM的时钟进行8分频 */
		{
			MCPWM0_TCLK |= (BIT0 + BIT1);
			stru_mcpwm_shadow.TH0 = stru_mcpwm_shadow_period_temp>>3;
		}
		else
		{
			MCPWM0_TCLK &= ~(BIT0 + BIT1);
			stru_mcpwm_shadow.TH0 = stru_mcpwm_shadow_period_temp;
		}
				
		/* mcpwm0 compare parameter */
		stru_mcpwm_shadow.TH00 = ((get_arg_long(argv[3])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH01 = ((get_arg_long(argv[4])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH10 = ((get_arg_long(argv[5])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH11 = ((get_arg_long(argv[6])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH20 = ((get_arg_long(argv[7])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH21 = ((get_arg_long(argv[8])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		
		
		/* mcpwm0/1 deadtime parameter */
		stru_mcpwm_shadow.DeadTimeCH012P = get_arg_ulong(argv[9])*EPWM_Tus;
		stru_mcpwm_shadow.DeadTimeCH012N = get_arg_ulong(argv[10])*EPWM_Tus;
		
		/*************************************************************************/
		#if 1
		MCPWM0_TH0 = stru_mcpwm_shadow.TH0;
	
		MCPWM0_TH00 = stru_mcpwm_shadow.TH00;
		MCPWM0_TH01 = stru_mcpwm_shadow.TH01;
		MCPWM0_TH10 = stru_mcpwm_shadow.TH10;
		MCPWM0_TH11 = stru_mcpwm_shadow.TH11;
		MCPWM0_TH20 = stru_mcpwm_shadow.TH20;
		MCPWM0_TH21 = stru_mcpwm_shadow.TH21;
		
		MCPWM0_DTH00 = stru_mcpwm_shadow.DeadTimeCH012P; // MCPWM CH0/1/2 P 通道死区宽度设置为0
		MCPWM0_DTH01 = stru_mcpwm_shadow.DeadTimeCH012N; // MCPWM CH0/1/2 N 通道死区宽度设置为0
		#endif
		/*************************************************************************/
		
		
	}
	else if(pwm_instance == 1U)
	{
		stru_mcpwm_shadow.TH1 = get_arg_ulong(argv[2]);
		if(stru_mcpwm_shadow.TH1 == 0U)
		{
			INFO("wrong pwm period\n");
			return;
		}
		else
		{
			stru_mcpwm_shadow_period_temp = (MCU_CLK>>1)/stru_mcpwm_shadow.TH1;
		}	

		if(stru_mcpwm_shadow_period_temp > 65535U) /* 当频率太低时将MCPWM的时钟进行8分频 */
		{
			MCPWM0_TCLK |= (BIT0 + BIT1);
			stru_mcpwm_shadow.TH1 = stru_mcpwm_shadow_period_temp>>3;
		}
		else
		{
			MCPWM0_TCLK &= ~(BIT0 + BIT1);
			stru_mcpwm_shadow.TH1 = stru_mcpwm_shadow_period_temp;
		}
				
		/* mcpwm0 compare parameter */
		stru_mcpwm_shadow.TH30 = ((get_arg_long(argv[3])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH31 = ((get_arg_long(argv[4])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH40 = ((get_arg_long(argv[5])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH41 = ((get_arg_long(argv[6])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH50 = ((get_arg_long(argv[7])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH51 = ((get_arg_long(argv[8])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		
		/* mcpwm0/1 deadtime parameter */
		stru_mcpwm_shadow.DeadTimeCH345P = get_arg_ulong(argv[9])*EPWM_Tus;
		stru_mcpwm_shadow.DeadTimeCH345N = get_arg_ulong(argv[10])*EPWM_Tus;
		
		/*************************************************************************/
		#if 1
		MCPWM0_TH1 = stru_mcpwm_shadow.TH1;
	
		MCPWM0_TH30 = stru_mcpwm_shadow.TH30;
		MCPWM0_TH31 = stru_mcpwm_shadow.TH31;
		MCPWM0_TH40 = stru_mcpwm_shadow.TH40;
		MCPWM0_TH41 = stru_mcpwm_shadow.TH41;
		MCPWM0_TH50 = stru_mcpwm_shadow.TH50;
		MCPWM0_TH51 = stru_mcpwm_shadow.TH51;
		
		MCPWM0_DTH10 = stru_mcpwm_shadow.DeadTimeCH345P; // MCPWM CH0/1/2 P 通道死区宽度设置为0
		MCPWM0_DTH11 = stru_mcpwm_shadow.DeadTimeCH345N; // MCPWM CH0/1/2 N 通道死区宽度设置为0
		
		#endif
		/*************************************************************************/
	}
	else
	{
		INFO("argv[0] mcpwm instance wrong. \n");
	}

	MCPWM0_PRT		= 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */

	INFO("mcpwm_center_align_test run success! \n");

}

void mcpwm_center_align_push_pull_test(int argc, char *argv[])
{
	uint32_t pwm_instance;
	uint32_t stru_mcpwm_shadow_period_temp;
	
	if (argc < 11U)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);
		return;
	}
	
	
//	INFO("mcpwm_test argc= %d\n", argc);
	
	for(uint16_t i=0;i< argc;i++)
	{
		INFO("argv[%d]:%s\n", i,argv[i]);
	}
	
	
	pwm_instance = get_arg_ulong(argv[1]);
	
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */
	
	/* 图 14-8 MCPWM 时序 TH<n>0 和 TH<n>1-中心对齐推挽模式 */
	MCPWM0_IO01 = CLEAR_BIT(MCPWM0_IO01,BIT7);   //complement mode
	MCPWM0_IO01 = CLEAR_BIT(MCPWM0_IO01,BIT15);  //complement mode
	MCPWM0_IO23 = CLEAR_BIT(MCPWM0_IO23,BIT7);   //complement mode
	MCPWM0_IO23 = CLEAR_BIT(MCPWM0_IO23,BIT15);  //complement mode
	MCPWM0_IO45 = CLEAR_BIT(MCPWM0_IO45,BIT7);   //complement mode
	MCPWM0_IO45 = CLEAR_BIT(MCPWM0_IO45,BIT15);  //complement mode
	MCPWM0_PP   = 0x3F; /* 打开推挽模式使能信号 */

	if(pwm_instance == 0)
	{
		stru_mcpwm_shadow.TH0 = get_arg_ulong(argv[2]);
		if(stru_mcpwm_shadow.TH0 == 0U)
		{
			INFO("wrong pwm period\n");
			return;
		}
		else
		{
			stru_mcpwm_shadow_period_temp = (MCU_CLK>>1)/stru_mcpwm_shadow.TH0;
		}
		
		if(stru_mcpwm_shadow_period_temp > 65535U) /* 当频率太低时将MCPWM的时钟进行8分频 */
		{
			MCPWM0_TCLK |= (BIT0 + BIT1);
			stru_mcpwm_shadow.TH0 = stru_mcpwm_shadow_period_temp>>3;
		}
		else
		{
			MCPWM0_TCLK &= ~(BIT0 + BIT1);
			stru_mcpwm_shadow.TH0 = stru_mcpwm_shadow_period_temp;
		}
				
		/* mcpwm0 compare parameter */
		stru_mcpwm_shadow.TH00 = ((get_arg_long(argv[3])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH01 = ((get_arg_long(argv[4])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH10 = ((get_arg_long(argv[5])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH11 = ((get_arg_long(argv[6])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH20 = ((get_arg_long(argv[7])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH21 = ((get_arg_long(argv[8])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		
		
		/* mcpwm0/1 deadtime parameter */
		stru_mcpwm_shadow.DeadTimeCH012P = get_arg_ulong(argv[9])*EPWM_Tus;
		stru_mcpwm_shadow.DeadTimeCH012N = get_arg_ulong(argv[10])*EPWM_Tus;
		
		/*************************************************************************/
		#if 1
		MCPWM0_TH0 = stru_mcpwm_shadow.TH0;
	
		MCPWM0_TH00 = stru_mcpwm_shadow.TH00;
		MCPWM0_TH01 = stru_mcpwm_shadow.TH01;
		MCPWM0_TH10 = stru_mcpwm_shadow.TH10;
		MCPWM0_TH11 = stru_mcpwm_shadow.TH11;
		MCPWM0_TH20 = stru_mcpwm_shadow.TH20;
		MCPWM0_TH21 = stru_mcpwm_shadow.TH21;
		
		MCPWM0_DTH00 = stru_mcpwm_shadow.DeadTimeCH012P; // MCPWM CH0/1/2 P 通道死区宽度设置为0
		MCPWM0_DTH01 = stru_mcpwm_shadow.DeadTimeCH012N; // MCPWM CH0/1/2 N 通道死区宽度设置为0
		#endif
		/*************************************************************************/
		
		
	}
	else if(pwm_instance == 1U)
	{
		stru_mcpwm_shadow.TH1 = get_arg_ulong(argv[2]);
		if(stru_mcpwm_shadow.TH1 == 0U)
		{
			INFO("wrong pwm period\n");
			return;
		}
		else
		{
			stru_mcpwm_shadow_period_temp = (MCU_CLK>>1)/stru_mcpwm_shadow.TH1;
		}	

		if(stru_mcpwm_shadow_period_temp > 65535U) /* 当频率太低时将MCPWM的时钟进行8分频 */
		{
			MCPWM0_TCLK |= (BIT0 + BIT1);
			stru_mcpwm_shadow.TH1 = stru_mcpwm_shadow_period_temp>>3;
		}
		else
		{
			MCPWM0_TCLK &= ~(BIT0 + BIT1);
			stru_mcpwm_shadow.TH1 = stru_mcpwm_shadow_period_temp;
		}
				
		/* mcpwm0 compare parameter */
		stru_mcpwm_shadow.TH30 = ((get_arg_long(argv[3])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH31 = ((get_arg_long(argv[4])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH40 = ((get_arg_long(argv[5])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH41 = ((get_arg_long(argv[6])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH50 = ((get_arg_long(argv[7])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH51 = ((get_arg_long(argv[8])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		
		/* mcpwm0/1 deadtime parameter */
		stru_mcpwm_shadow.DeadTimeCH345P = get_arg_ulong(argv[9])*EPWM_Tus;
		stru_mcpwm_shadow.DeadTimeCH345N = get_arg_ulong(argv[10])*EPWM_Tus;
		
		/*************************************************************************/
		#if 1
		MCPWM0_TH1 = stru_mcpwm_shadow.TH1;
	
		MCPWM0_TH30 = stru_mcpwm_shadow.TH30;
		MCPWM0_TH31 = stru_mcpwm_shadow.TH31;
		MCPWM0_TH40 = stru_mcpwm_shadow.TH40;
		MCPWM0_TH41 = stru_mcpwm_shadow.TH41;
		MCPWM0_TH50 = stru_mcpwm_shadow.TH50;
		MCPWM0_TH51 = stru_mcpwm_shadow.TH51;
		
		MCPWM0_DTH10 = stru_mcpwm_shadow.DeadTimeCH345P; // MCPWM CH0/1/2 P 通道死区宽度设置为0
		MCPWM0_DTH11 = stru_mcpwm_shadow.DeadTimeCH345N; // MCPWM CH0/1/2 N 通道死区宽度设置为0
		
		#endif
		/*************************************************************************/
	}
	else
	{
		INFO("argv[0] mcpwm instance wrong. \n");
	}

	MCPWM0_PRT		= 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */

	INFO("mcpwm_center_align_push_pull_test run success! \n");

}

void mcpwm_edge_align_test(int argc, char *argv[])
{
	uint32_t pwm_instance;
	uint32_t stru_mcpwm_shadow_period_temp;
	
	if (argc < 11U)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);
		return;
	}
	
	
//	INFO("mcpwm_test argc= %d\n", argc);
	
	for(uint16_t i=0;i< argc;i++)
	{
		INFO("argv[%d]:%s\n", i,argv[i]);
	}
	
	
	pwm_instance = get_arg_ulong(argv[1]);
	
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */

	/* 图 14-9MCPWM 时序边沿对齐模式 */
	MCPWM0_IO01 = SET_BIT(MCPWM0_IO01,BIT7);   //edge_align mode
	MCPWM0_IO01 = SET_BIT(MCPWM0_IO01,BIT15);  //edge_align mode
	MCPWM0_IO23 = SET_BIT(MCPWM0_IO23,BIT7);   //edge_align mode
	MCPWM0_IO23 = SET_BIT(MCPWM0_IO23,BIT15);  //edge_align mode
	MCPWM0_IO45 = SET_BIT(MCPWM0_IO45,BIT7);   //edge_align mode
	MCPWM0_IO45 = SET_BIT(MCPWM0_IO45,BIT15);  //edge_align mode
	MCPWM0_PP   = 0x00; /* normal mode */

	if(pwm_instance == 0)
	{
		stru_mcpwm_shadow.TH0 = get_arg_ulong(argv[2]);
		if(stru_mcpwm_shadow.TH0 == 0U)
		{
			INFO("wrong pwm period\n");
			return;
		}
		else
		{
			stru_mcpwm_shadow_period_temp = (MCU_CLK>>1)/stru_mcpwm_shadow.TH0;
		}
		
		if(stru_mcpwm_shadow_period_temp > 65535U) /* 当频率太低时将MCPWM的时钟进行8分频 */
		{
			MCPWM0_TCLK |= (BIT0 + BIT1);
			stru_mcpwm_shadow.TH0 = stru_mcpwm_shadow_period_temp>>3;
		}
		else
		{
			MCPWM0_TCLK &= ~(BIT0 + BIT1);
			stru_mcpwm_shadow.TH0 = stru_mcpwm_shadow_period_temp;
		}
				
		/* mcpwm0 compare parameter */
		stru_mcpwm_shadow.TH00 = ((get_arg_long(argv[3])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH01 = ((get_arg_long(argv[4])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH10 = ((get_arg_long(argv[5])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH11 = ((get_arg_long(argv[6])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH20 = ((get_arg_long(argv[7])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH21 = ((get_arg_long(argv[8])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		
		
		/* mcpwm0/1 deadtime parameter */
		stru_mcpwm_shadow.DeadTimeCH012P = get_arg_ulong(argv[9])*EPWM_Tus;
		stru_mcpwm_shadow.DeadTimeCH012N = get_arg_ulong(argv[10])*EPWM_Tus;
		
		/*************************************************************************/
		#if 1
		MCPWM0_TH0 = stru_mcpwm_shadow.TH0;
	
		MCPWM0_TH00 = stru_mcpwm_shadow.TH00;
		MCPWM0_TH01 = stru_mcpwm_shadow.TH01;
		MCPWM0_TH10 = stru_mcpwm_shadow.TH10;
		MCPWM0_TH11 = stru_mcpwm_shadow.TH11;
		MCPWM0_TH20 = stru_mcpwm_shadow.TH20;
		MCPWM0_TH21 = stru_mcpwm_shadow.TH21;
		
		MCPWM0_DTH00 = stru_mcpwm_shadow.DeadTimeCH012P; // MCPWM CH0/1/2 P 通道死区宽度设置为0
		MCPWM0_DTH01 = stru_mcpwm_shadow.DeadTimeCH012N; // MCPWM CH0/1/2 N 通道死区宽度设置为0
		#endif
		/*************************************************************************/
		
		
	}
	else if(pwm_instance == 1U)
	{
		stru_mcpwm_shadow.TH1 = get_arg_ulong(argv[2]);
		if(stru_mcpwm_shadow.TH1 == 0U)
		{
			INFO("wrong pwm period\n");
			return;
		}
		else
		{
			stru_mcpwm_shadow_period_temp = (MCU_CLK>>1)/stru_mcpwm_shadow.TH1;
		}	

		if(stru_mcpwm_shadow_period_temp > 65535U) /* 当频率太低时将MCPWM的时钟进行8分频 */
		{
			MCPWM0_TCLK |= (BIT0 + BIT1);
			stru_mcpwm_shadow.TH1 = stru_mcpwm_shadow_period_temp>>3;
		}
		else
		{
			MCPWM0_TCLK &= ~(BIT0 + BIT1);
			stru_mcpwm_shadow.TH1 = stru_mcpwm_shadow_period_temp;
		}
				
		/* mcpwm0 compare parameter */
		stru_mcpwm_shadow.TH30 = ((get_arg_long(argv[3])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH31 = ((get_arg_long(argv[4])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH40 = ((get_arg_long(argv[5])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH41 = ((get_arg_long(argv[6])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH50 = ((get_arg_long(argv[7])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH51 = ((get_arg_long(argv[8])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		
		/* mcpwm0/1 deadtime parameter */
		stru_mcpwm_shadow.DeadTimeCH345P = get_arg_ulong(argv[9])*EPWM_Tus;
		stru_mcpwm_shadow.DeadTimeCH345N = get_arg_ulong(argv[10])*EPWM_Tus;
		
		/*************************************************************************/
		#if 1
		MCPWM0_TH1 = stru_mcpwm_shadow.TH1;
	
		MCPWM0_TH30 = stru_mcpwm_shadow.TH30;
		MCPWM0_TH31 = stru_mcpwm_shadow.TH31;
		MCPWM0_TH40 = stru_mcpwm_shadow.TH40;
		MCPWM0_TH41 = stru_mcpwm_shadow.TH41;
		MCPWM0_TH50 = stru_mcpwm_shadow.TH50;
		MCPWM0_TH51 = stru_mcpwm_shadow.TH51;
		
		MCPWM0_DTH10 = stru_mcpwm_shadow.DeadTimeCH345P; // MCPWM CH0/1/2 P 通道死区宽度设置为0
		MCPWM0_DTH11 = stru_mcpwm_shadow.DeadTimeCH345N; // MCPWM CH0/1/2 N 通道死区宽度设置为0
		
		#endif
		/*************************************************************************/
	}
	else
	{
		INFO("argv[0] mcpwm instance wrong. \n");
	}

	MCPWM0_PRT		= 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */

	INFO("mcpwm_edge_align_test run success! \n");

}

void mcpwm_edge_align_push_pull_test(int argc, char *argv[])
{
	uint32_t pwm_instance;
	uint32_t stru_mcpwm_shadow_period_temp;
	
	if (argc < 11U)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);
		return;
	}
	
	
//	INFO("mcpwm_test argc= %d\n", argc);
	
	for(uint16_t i=0;i< argc;i++)
	{
		INFO("argv[%d]:%s\n", i,argv[i]);
	}
	
	
	pwm_instance = get_arg_ulong(argv[1]);
	
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */

	
	/* 图 14-10MCPWM 时序 TH<n>0 和 TH<n>1 边沿对齐推挽模式  */
	MCPWM0_IO01 = SET_BIT(MCPWM0_IO01,BIT7);   //edge_align mode
	MCPWM0_IO01 = SET_BIT(MCPWM0_IO01,BIT15);  //edge_align mode
	MCPWM0_IO23 = SET_BIT(MCPWM0_IO23,BIT7);   //edge_align mode
	MCPWM0_IO23 = SET_BIT(MCPWM0_IO23,BIT15);  //edge_align mode
	MCPWM0_IO45 = SET_BIT(MCPWM0_IO45,BIT7);   //edge_align mode
	MCPWM0_IO45 = SET_BIT(MCPWM0_IO45,BIT15);  //edge_align mode
	MCPWM0_PP   = 0x3F; /* push-pull mode */

	if(pwm_instance == 0)
	{
		stru_mcpwm_shadow.TH0 = get_arg_ulong(argv[2]);
		if(stru_mcpwm_shadow.TH0 == 0U)
		{
			INFO("wrong pwm period\n");
			return;
		}
		else
		{
			stru_mcpwm_shadow_period_temp = (MCU_CLK>>1)/stru_mcpwm_shadow.TH0;
		}
		
		if(stru_mcpwm_shadow_period_temp > 65535U) /* 当频率太低时将MCPWM的时钟进行8分频 */
		{
			MCPWM0_TCLK |= (BIT0 + BIT1);
			stru_mcpwm_shadow.TH0 = stru_mcpwm_shadow_period_temp>>3;
		}
		else
		{
			MCPWM0_TCLK &= ~(BIT0 + BIT1);
			stru_mcpwm_shadow.TH0 = stru_mcpwm_shadow_period_temp;
		}
				
		/* mcpwm0 compare parameter */
		stru_mcpwm_shadow.TH00 = ((get_arg_long(argv[3])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH01 = ((get_arg_long(argv[4])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH10 = ((get_arg_long(argv[5])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH11 = ((get_arg_long(argv[6])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH20 = ((get_arg_long(argv[7])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		stru_mcpwm_shadow.TH21 = ((get_arg_long(argv[8])*stru_mcpwm_shadow.TH0 + 16384)>>15);
		
		
		/* mcpwm0/1 deadtime parameter */
		stru_mcpwm_shadow.DeadTimeCH012P = get_arg_ulong(argv[9])*EPWM_Tus;
		stru_mcpwm_shadow.DeadTimeCH012N = get_arg_ulong(argv[10])*EPWM_Tus;
		
		/*************************************************************************/
		#if 1
		MCPWM0_TH0 = stru_mcpwm_shadow.TH0;
	
		MCPWM0_TH00 = stru_mcpwm_shadow.TH00;
		MCPWM0_TH01 = stru_mcpwm_shadow.TH01;
		MCPWM0_TH10 = stru_mcpwm_shadow.TH10;
		MCPWM0_TH11 = stru_mcpwm_shadow.TH11;
		MCPWM0_TH20 = stru_mcpwm_shadow.TH20;
		MCPWM0_TH21 = stru_mcpwm_shadow.TH21;
		
		MCPWM0_DTH00 = stru_mcpwm_shadow.DeadTimeCH012P; // MCPWM CH0/1/2 P 通道死区宽度设置为0
		MCPWM0_DTH01 = stru_mcpwm_shadow.DeadTimeCH012N; // MCPWM CH0/1/2 N 通道死区宽度设置为0
		#endif
		/*************************************************************************/
		
		
	}
	else if(pwm_instance == 1U)
	{
		stru_mcpwm_shadow.TH1 = get_arg_ulong(argv[2]);
		if(stru_mcpwm_shadow.TH1 == 0U)
		{
			INFO("wrong pwm period\n");
			return;
		}
		else
		{
			stru_mcpwm_shadow_period_temp = (MCU_CLK>>1)/stru_mcpwm_shadow.TH1;
		}	

		if(stru_mcpwm_shadow_period_temp > 65535U) /* 当频率太低时将MCPWM的时钟进行8分频 */
		{
			MCPWM0_TCLK |= (BIT0 + BIT1);
			stru_mcpwm_shadow.TH1 = stru_mcpwm_shadow_period_temp>>3;
		}
		else
		{
			MCPWM0_TCLK &= ~(BIT0 + BIT1);
			stru_mcpwm_shadow.TH1 = stru_mcpwm_shadow_period_temp;
		}
				
		/* mcpwm0 compare parameter */
		stru_mcpwm_shadow.TH30 = ((get_arg_long(argv[3])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH31 = ((get_arg_long(argv[4])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH40 = ((get_arg_long(argv[5])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH41 = ((get_arg_long(argv[6])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH50 = ((get_arg_long(argv[7])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		stru_mcpwm_shadow.TH51 = ((get_arg_long(argv[8])*stru_mcpwm_shadow.TH1 + 16384)>>15);
		
		/* mcpwm0/1 deadtime parameter */
		stru_mcpwm_shadow.DeadTimeCH345P = get_arg_ulong(argv[9])*EPWM_Tus;
		stru_mcpwm_shadow.DeadTimeCH345N = get_arg_ulong(argv[10])*EPWM_Tus;
		
		/*************************************************************************/
		#if 1
		MCPWM0_TH1 = stru_mcpwm_shadow.TH1;
	
		MCPWM0_TH30 = stru_mcpwm_shadow.TH30;
		MCPWM0_TH31 = stru_mcpwm_shadow.TH31;
		MCPWM0_TH40 = stru_mcpwm_shadow.TH40;
		MCPWM0_TH41 = stru_mcpwm_shadow.TH41;
		MCPWM0_TH50 = stru_mcpwm_shadow.TH50;
		MCPWM0_TH51 = stru_mcpwm_shadow.TH51;
		
		MCPWM0_DTH10 = stru_mcpwm_shadow.DeadTimeCH345P; // MCPWM CH0/1/2 P 通道死区宽度设置为0
		MCPWM0_DTH11 = stru_mcpwm_shadow.DeadTimeCH345N; // MCPWM CH0/1/2 N 通道死区宽度设置为0
		
		#endif
		/*************************************************************************/
	}
	else
	{
		INFO("argv[0] mcpwm instance wrong. \n");
	}

	MCPWM0_PRT		= 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */

	INFO("mcpwm_edge_align_push_pull_test run success! \n");
}

/* 
argv[1](instance)     : 0->mcpwm0  					1->mcpwm1
argv[2](FAIL1_SEL)    : 0->IO BKIN1    1->比较器1    2-CLUOUT2    3->CLUOUT3
argv[3](FAIL0_SEL)    : 0->IO BKIN0    1->比较器0    2-CLUOUT0    3->CLUOUT1
argv[4](FAIL1_EN)     : 0->关闭 	   1->使能
argv[5](FAIL0_EN)     : 0->关闭 	   1->使能
argv[6](FAIL1_POL)    : 0->信号极性正常输入(高电平有效)       1->信号极性取反输入(低电平有效)
argv[7](FAIL0_POL)    : 0->信号极性正常输入(高电平有效)       1->信号极性取反输入(低电平有效)
argv[8](Recovery)     : 0->关闭自动故障清除       1->使能自动故障清除
argv[9](TR_T1_UEN)	  : t1（过零）事件更新使能 		0->关闭       1->使能 
argv[10](TR_T0_UEN)   : t0（过零）事件更新使能 		0->关闭       1->使能 
argv[11](FLT_CLKDIV)  : 系统时钟 / (FLT_CLKDIV + 1)。分频范围是 1-256
------------------------1-2-3-4-5-6-7-8-9-10-11
cmd_mcpwm_io_fault_test 0 0 0 1 1 1 1 1 1 1  0 
*/
void mcpwm_io_fault_test(int argc, char *argv[])
{
	uint32_t pwm_instance;

	uint32_t FAIL1_SEL, FAIL0_SEL;
	uint32_t Fail1_EN, Fail0_EN;
	uint32_t Fail1_Polarity, Fail0_Polarity;
	uint32_t TR_AEC,FLT_CLKDIV,TR_T1_UEN,TR_T0_UEN;

	if (argc < 12U)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);
		return;
	}
	
	for(uint16_t i=0;i< argc;i++)
	{
		INFO("argv[%d]:%s\n", i,argv[i]);
	}
	
	pwm_instance     = get_arg_ulong(argv[1]);
	FAIL1_SEL		 = get_arg_ulong(argv[2]); 		/* FAIL1 来源选择 */
	FAIL0_SEL		 = get_arg_ulong(argv[3]); 		/* FAIL0 来源选择 */
	Fail1_EN 	     = get_arg_ulong(argv[4]); 		/* FAIL1 输入使能。 1:使能； 0:关闭 */
	Fail0_EN 	     = get_arg_ulong(argv[5]); 		/* FAIL0 输入使能。 1:使能； 0:关闭 */
	Fail1_Polarity 	 = get_arg_ulong(argv[6]);  	/* FAIL1 极性选择。 1:信号极性取反输入，信号输入低为有效电平； 0:信号极性正常输入，信号输入高为有效电平 */
	Fail0_Polarity 	 = get_arg_ulong(argv[7]);  	/* FAIL0 极性选择。 1:信号极性取反输入有效电平； 			  0:信号极性正常输入，信号输入高为有效电平。 */
	TR_AEC  	     = get_arg_ulong(argv[8]);     /* 0->关闭自动故障清除       1->使能自动故障清除 */

	TR_T1_UEN  	     = get_arg_ulong(argv[9]);     /* t1（过零）事件更新使能 0->关闭       1->使能 */
	TR_T0_UEN  	     = get_arg_ulong(argv[10]);    /* t0（过零）事件更新使能 0->关闭       1->使能 */
	
	/*FAIL 信号输入的滤波时钟分频寄存器，基于系统时钟分频，影响 MCPWM0_FAIL[1:0]。计算公式如下:
	系统时钟 / (FLT_CLKDIV + 1)。分频范围是 1-256。*/
	FLT_CLKDIV  	 = get_arg_ulong(argv[11]);  
	
	mcpwm_fault_gpio_init();
	
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */
	
	if(pwm_instance	== 0)
	{
		MCPWM0_FAIL012 = (MCPWM0_FAIL012&~MCPWM0_FAIL1_SEL_MASK)|MCPWM0_FAIL1_SEL_MASK_SET(FAIL1_SEL);
		MCPWM0_FAIL012 = (MCPWM0_FAIL012&~MCPWM0_FAIL0_SEL_MASK)|MCPWM0_FAIL0_SEL_MASK_SET(FAIL0_SEL);

		MCPWM0_FAIL012 = (MCPWM0_FAIL012&~MCPWM0_FAIL1_EN_MASK)|MCPWM0_FAIL1_EN_MASK_SET(Fail1_EN);
		MCPWM0_FAIL012 = (MCPWM0_FAIL012&~MCPWM0_FAIL0_EN_MASK)|MCPWM0_FAIL0_EN_MASK_SET(Fail0_EN);

		MCPWM0_FAIL012 = (MCPWM0_FAIL012&~MCPWM0_FAIL1_POL_MASK)|MCPWM0_FAIL1_POL_MASK_SET(Fail1_Polarity);
		MCPWM0_FAIL012 = (MCPWM0_FAIL012&~MCPWM0_FAIL0_POL_MASK)|MCPWM0_FAIL0_POL_MASK_SET(Fail0_Polarity);	
		
		MCPWM0_SDCFG = (MCPWM0_SDCFG&~MCPWM0_SDCFG_TR0_AEC_MASK)|MCPWM0_SDCFG_TR0_AEC_MASK_SET(TR_AEC);

		MCPWM0_SDCFG = (MCPWM0_SDCFG&~MCPWM0_SDCFG_TR0_T1_UEN_MASK)|MCPWM0_SDCFG_TR0_T1_UEN_MASK_SET(TR_T1_UEN);
		MCPWM0_SDCFG = (MCPWM0_SDCFG&~MCPWM0_SDCFG_TR0_T0_UEN_MASK)|MCPWM0_SDCFG_TR0_T0_UEN_MASK_SET(TR_T0_UEN);

		MCPWM0_FLT = FLT_CLKDIV;
		
		INFO("mcpwm0_io_fault_test run success! \n");
	}
	else
	{
		MCPWM0_FAIL345 = (MCPWM0_FAIL345&~MCPWM1_FAIL3_SEL_MASK)|MCPWM1_FAIL3_SEL_MASK_SET(FAIL1_SEL);
		MCPWM0_FAIL345 = (MCPWM0_FAIL345&~MCPWM1_FAIL2_SEL_MASK)|MCPWM1_FAIL2_SEL_MASK_SET(FAIL0_SEL);
							
		MCPWM0_FAIL345 = (MCPWM0_FAIL345&~MCPWM1_FAIL3_EN_MASK)|MCPWM1_FAIL3_EN_MASK_SET(Fail1_EN);
		MCPWM0_FAIL345 = (MCPWM0_FAIL345&~MCPWM1_FAIL2_EN_MASK)|MCPWM1_FAIL2_EN_MASK_SET(Fail0_EN);
							 
		MCPWM0_FAIL345 = (MCPWM0_FAIL345&~MCPWM1_FAIL3_POL_MASK)|MCPWM1_FAIL3_POL_MASK_SET(Fail1_Polarity);
		MCPWM0_FAIL345 = (MCPWM0_FAIL345&~MCPWM1_FAIL2_POL_MASK)|MCPWM1_FAIL2_POL_MASK_SET(Fail0_Polarity);	
		
		MCPWM0_SDCFG = (MCPWM0_SDCFG&~MCPWM0_SDCFG_TR1_AEC_MASK)|MCPWM0_SDCFG_TR1_AEC_MASK_SET(TR_AEC);

		MCPWM0_SDCFG = (MCPWM0_SDCFG&~MCPWM0_SDCFG_TR1_T1_UEN_MASK)|MCPWM0_SDCFG_TR1_T1_UEN_MASK_SET(TR_T1_UEN);
		MCPWM0_SDCFG = (MCPWM0_SDCFG&~MCPWM0_SDCFG_TR1_T0_UEN_MASK)|MCPWM0_SDCFG_TR1_T0_UEN_MASK_SET(TR_T0_UEN);

		MCPWM0_FLT = FLT_CLKDIV;
		
		INFO("mcpwm1_io_fault_test run success! \n");
		
	}
	MCPWM0_PRT		= 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */

}


