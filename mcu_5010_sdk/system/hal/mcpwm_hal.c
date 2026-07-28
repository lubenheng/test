#include "platform_common.h"

#include "mcpwm_reg.h"
#include "device_resource.h"
#include "mcpwm_hal.h"
#include "mcpwm.h"

#include "test.h"

MCPWM0_INT_CALLBACK g_mcpwm0_int_callback = NULL;
MCPWM1_INT_CALLBACK g_mcpwm1_int_callback = NULL;


/*******************************************************************************
 函数名称：    void MCPWM_ch012_init(void)
 功能描述：    MCPWM初始化
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2023/11/5      V1.0           Li Tonghua          创建
 *******************************************************************************/
 MCPWM_InitTypeDef MCPWM_InitStructure;
void MCPWM_ch012_init(void)
{
	//MCPWM_InitTypeDef MCPWM_InitStructure;

	u16 prd;
	u16 deadTime;
	u8 clockDiv;
	// PSTR_DrvCfgPara	pParaPtr;

	// pParaPtr	= getCfgParaPtr(0);
	// prd			= getPWMPrd(pParaPtr);
	// deadTime	= getDeadTime(pParaPtr);
	// clockDiv	= pParaPtr->mS_GlobalCfg.m_nuPWMPrsc & 0x03;

	prd = EPWM_PERIOD;
	deadTime = EPWM_DT;
	clockDiv = 0U;

	MCPWM_StructInit(&MCPWM_InitStructure);

	MCPWM_InitStructure.CLK_DIV = clockDiv;		/* MCPWM时钟分频设置 */
	MCPWM_InitStructure.MCLK_EN = ENABLE;		/* 模块时钟开启 */
	MCPWM_InitStructure.IO_CMP_FLT_CLKDIV = 12; /* 急停事件(来自IO口信号)数字滤波器时间设置 */

	MCPWM_InitStructure.AUEN = MCPWM0_ALL_AUPDAT; /*自动加载使能*/

	/* MCPWM0_CNT0	 第一组PWM设置*/

	MCPWM_InitStructure.BASE_CNT0_EN = ENABLE; /* 主计数器开始计数使能开关 */
	MCPWM_InitStructure.TH0 = prd;			   /* 计数周期设置即MCPWM输出周期*/
	
/*begin***********************jack add***********************/
	MCPWM_InitStructure.TH00 =  (uint16_t)(-EPWM_HALFPERIOD);  		//jack add
	MCPWM_InitStructure.TH01 =  EPWM_HALFPERIOD;  			 		//jack add
	MCPWM_InitStructure.TH10 =  (uint16_t)(-EPWM_HALFPERIOD);  		//jack add
	MCPWM_InitStructure.TH11 =  EPWM_HALFPERIOD;   			 		//jack add
	MCPWM_InitStructure.TH20 =  (uint16_t)(-EPWM_HALFPERIOD);  		//jack add
	MCPWM_InitStructure.TH21 =  EPWM_HALFPERIOD;   			 		//jack add
/*end***********************jack add***********************/
							 
	MCPWM_InitStructure.MCPWM_WorkModeCH0 = MCPWM0_CENTRAL_PWM_MODE; /* MCPWM CH0工作模式：中心对齐PWM模式 */
	MCPWM_InitStructure.MCPWM_WorkModeCH1 = MCPWM0_CENTRAL_PWM_MODE; /* 通道工作模式设置，中心对齐或边沿对齐 */
	MCPWM_InitStructure.MCPWM_WorkModeCH2 = MCPWM0_CENTRAL_PWM_MODE;
	MCPWM_InitStructure.DeadTimeCH012N = deadTime; /* 死区时间设置 */
	MCPWM_InitStructure.DeadTimeCH012P = deadTime;

	MCPWM_InitStructure.CMP_CTRL_CNT0 = DISABLE; /* CMP控制CNT0计数使能位 */
	MCPWM_InitStructure.EVT_CNT0_EN = DISABLE;	 /* MCPWM_CNT1外部触发使能位 */
	MCPWM_InitStructure.EVT0 = DISABLE;			 /* 外部触发 */

	MCPWM_InitStructure.TR0_UP_INTV = DISABLE;
	MCPWM_InitStructure.TR0_T0_UpdateEN = ENABLE; /*T0时刻更新使能*/
	MCPWM_InitStructure.TR0_T1_UpdateEN = DISABLE;
	MCPWM_InitStructure.TR0_AEC = DISABLE; /*自动清除MCPWM0_EIF标志位*/

	MCPWM_InitStructure.TMR0 = (u16)(40 - prd); /* MCPWM_TMR0  设置采样点 */
	MCPWM_InitStructure.TMR1 = (u16)(prd - 1);	/* MCPWM_TMR1 设置 */

#if (PRE_DRIVER_POLARITY == P_HIGH__N_LOW)			 /* CHxP 高有效， CHxN低电平有效 */
	MCPWM_InitStructure.CH0N_Polarity_INV = ENABLE;	 /* CH0N通道输出极性设置 | 正常输出或取反输出*/
	MCPWM_InitStructure.CH0P_Polarity_INV = DISABLE; /* CH0P通道输出极性设置 | 正常输出或取反输出 */
	MCPWM_InitStructure.CH1N_Polarity_INV = ENABLE;
	MCPWM_InitStructure.CH1P_Polarity_INV = DISABLE;
	MCPWM_InitStructure.CH2N_Polarity_INV = ENABLE;
	MCPWM_InitStructure.CH2P_Polarity_INV = DISABLE;

	MCPWM_InitStructure.Switch_CH0N_CH0P = DISABLE; /* 通道交换选择设置 | CH0P和CH0N是否选择信号交换 */
	MCPWM_InitStructure.Switch_CH1N_CH1P = DISABLE; /* 通道交换选择设置 */
	MCPWM_InitStructure.Switch_CH2N_CH2P = DISABLE; /* 通道交换选择设置 */

	/* 默认电平设置 默认电平输出不受MCPWM_IO01和MCPWM_IO23的 BIT0、BIT1、BIT8、BIT9、BIT6、BIT14
													 通道交换和极性控制的影响，直接控制通道输出 */
	MCPWM_InitStructure.CH0P_default_output = MCPWM0_LOW_LEVEL;
	MCPWM_InitStructure.CH0N_default_output = MCPWM0_HIGH_LEVEL;
	MCPWM_InitStructure.CH1P_default_output = MCPWM0_LOW_LEVEL;	 /* CH1P对应引脚在空闲状态输出低电平 */
	MCPWM_InitStructure.CH1N_default_output = MCPWM0_HIGH_LEVEL; /* CH1N对应引脚在空闲状态输出高电平 */
	MCPWM_InitStructure.CH2P_default_output = MCPWM0_LOW_LEVEL;
	MCPWM_InitStructure.CH2N_default_output = MCPWM0_HIGH_LEVEL;
#else
#if (PRE_DRIVER_POLARITY == P_HIGH__N_HIGH) /* CHxP 高有效， CHxN高电平有效 */
	MCPWM_InitStructure.CH0N_Polarity_INV = DISABLE; /* CH0N通道输出极性设置 | 正常输出或取反输出*/
	MCPWM_InitStructure.CH0P_Polarity_INV = DISABLE; /* CH0P通道输出极性设置 | 正常输出或取反输出 */
	MCPWM_InitStructure.CH1N_Polarity_INV = DISABLE;
	MCPWM_InitStructure.CH1P_Polarity_INV = DISABLE;
	MCPWM_InitStructure.CH2N_Polarity_INV = DISABLE;
	MCPWM_InitStructure.CH2P_Polarity_INV = DISABLE;

	MCPWM_InitStructure.Switch_CH0N_CH0P = DISABLE; /* 通道交换选择设置 | CH0P和CH0N是否选择信号交换 */
	MCPWM_InitStructure.Switch_CH1N_CH1P = DISABLE; /* 通道交换选择设置 */
	MCPWM_InitStructure.Switch_CH2N_CH2P = DISABLE; /* 通道交换选择设置 */

	/* 默认电平设置 默认电平输出不受MCPWM_IO01和MCPWM_IO23的 BIT0、BIT1、BIT8、BIT9、BIT6、BIT14
													 通道交换和极性控制的影响，直接控制通道输出 */
	MCPWM_InitStructure.CH0P_default_output = MCPWM0_LOW_LEVEL;
	MCPWM_InitStructure.CH0N_default_output = MCPWM0_LOW_LEVEL;
	MCPWM_InitStructure.CH1P_default_output = MCPWM0_LOW_LEVEL; /* CH1P对应引脚在空闲状态输出低电平 */
	MCPWM_InitStructure.CH1N_default_output = MCPWM0_LOW_LEVEL; /* CH1N对应引脚在空闲状态输出高电平 */
	MCPWM_InitStructure.CH2P_default_output = MCPWM0_LOW_LEVEL;
	MCPWM_InitStructure.CH2N_default_output = MCPWM0_LOW_LEVEL;
#endif
#endif

#if (EPWM0_CURRENT_SAMPLE_TYPE == CURRENT_SAMPLE_1SHUNT)
	MCPWM_InitStructure.T0_Update0_INT_EN = DISABLE; /* T0更新事件 中断使能位 */
	MCPWM_InitStructure.T1_Update0_INT_EN = ENABLE;	 /* T1更新事件 中断使能位*/
	MCPWM_InitStructure.Update0_INT_EN = DISABLE;	 /* CNT0 更新事件 中断使能  */
#elif (EPWM0_CURRENT_SAMPLE_TYPE == CURRENT_SAMPLE_2SHUNT)
	MCPWM_InitStructure.T0_Update0_INT_EN = DISABLE; /* T0更新事件 中断使能位 */
	MCPWM_InitStructure.T1_Update0_INT_EN = DISABLE; /* T1更新事件 中断使能位 */
	MCPWM_InitStructure.Update0_INT_EN = DISABLE;	 /* CNT0更新事件中断使能	双电阻不使用PWM中断  */
#elif ((EPWM0_CURRENT_SAMPLE_TYPE == CURRENT_SAMPLE_3SHUNT) || (EPWM0_CURRENT_SAMPLE_TYPE == CURRENT_SAMPLE_MOSFET))
	MCPWM_InitStructure.T0_Update0_INT_EN = DISABLE;//ENABLE; /* T0更新事件 中断使能位 */
	MCPWM_InitStructure.T1_Update0_INT_EN = ENABLE;//DISABLE; /* T1更新事件 中断使能位*/
	MCPWM_InitStructure.Update0_INT_EN = DISABLE;	 /* CNT0 更新事件 中断使能  双电阻不使用PWM中断  */
#endif

	MCPWM_InitStructure.CH0N_FAIL_EN = DISABLE;//ENABLE; 	//jack add
	MCPWM_InitStructure.CH0P_FAIL_EN = DISABLE;//ENABLE;	//jack add
	MCPWM_InitStructure.CH1N_FAIL_EN = DISABLE;//ENABLE;	//jack add
	MCPWM_InitStructure.CH1P_FAIL_EN = DISABLE;//ENABLE;	//jack add
	MCPWM_InitStructure.CH2N_FAIL_EN = DISABLE;//ENABLE;	//jack add
	MCPWM_InitStructure.CH2P_FAIL_EN = DISABLE;//ENABLE;	//jack add

	MCPWM_InitStructure.FAIL0_INPUT_EN = DISABLE; // FAIL_0CAP
	MCPWM_InitStructure.FAIL0_INT_EN = DISABLE;
	MCPWM_InitStructure.FAIL0_Signal_Sel = MCPWM0_FAIL_SEL_CMP; // FAIL_0CAP
	MCPWM_InitStructure.FAIL0_Polarity = MCPWM0_HIGH_LEVEL_ACTIVE;

	MCPWM_InitStructure.FAIL1_INPUT_EN = DISABLE; // FAIL_0CAP
	MCPWM_InitStructure.FAIL1_INT_EN = DISABLE;
	MCPWM_InitStructure.FAIL1_Signal_Sel = MCPWM0_FAIL_SEL_CMP; // FAIL_0CAP
	MCPWM_InitStructure.FAIL1_Polarity = MCPWM0_HIGH_LEVEL_ACTIVE;

	MCPWM_InitStructure.HALT_PRT0 = ENABLE; /* 在接上仿真器debug程序时，暂停MCU运行时，选择各PWM通道正常输出调制信号
														   还是输出默认电平，保护功率器件 ENABLE:正常输出 DISABLE:输出默认电平*/
	MCPWM_InitStructure.FAIL_0CAP = ENABLE; // FAIL01事件发生时 将MCPWM0_CNT0值存入MCPWM0_FCNT 使能

	MCPWM_CH012_Init(&MCPWM_InitStructure);
	
	MCPWM_CH012_IESet(&MCPWM_InitStructure);
	
	
	// registerEPWM0_DrvLevel();
}

/*******************************************************************************
 函数名称：    void MCPWM_ch345_init_cfg(void)
 功能描述：    MCPWM初始化
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2023/11/5      V1.0           Li Tonghua          创建
 *******************************************************************************/
MCPWM_InitTypeDef MCPWM_InitStructure_M1;
void MCPWM_ch345_init(void)
{
//	MCPWM_InitTypeDef MCPWM_InitStructure_M1;

	u16 prd;
	u16 deadTime;
	u8	clockDiv;
	//PSTR_DrvCfgPara pParaPtr;

//#if(EPWM1_USED == FUNCTION_ON)

//#if (DRV0_PWM_ID == PLANTFORM_EPWM1)
	// pParaPtr	= getCfgParaPtr(0);
	// prd 		= getPWMPrd(pParaPtr);
	// deadTime	= getDeadTime(pParaPtr);
	// clockDiv	= pParaPtr->mS_GlobalCfg.m_nuPWMPrsc & 0x03;

	prd = EPWM_PERIOD;
	deadTime = EPWM_DT;
	clockDiv = 0U;

//#elif (DRV1_PWM_ID == PLANTFORM_EPWM1)
	// pParaPtr	= getCfgParaPtr(1);
	// prd 		= getPWMPrd(pParaPtr);
	// deadTime	= getDeadTime(pParaPtr);
	// clockDiv	= pParaPtr->mS_GlobalCfg.m_nuPWMPrsc & 0x03;
//#endif


	MCPWM_StructInit(&MCPWM_InitStructure_M1);

	MCPWM_InitStructure_M1.CLK_DIV = clockDiv;				/* MCPWM时钟分频设置 */
	MCPWM_InitStructure_M1.MCLK_EN = ENABLE;				/* 模块时钟开启 */
	MCPWM_InitStructure_M1.IO_CMP_FLT_CLKDIV  = 12; 		/* 急停事件(来自IO口信号来自比较器信号))数字滤波器时间设置 */

	//MCPWM_InitStructure_M1.AUEN = MCPWM0_ALL_AUPDAT;		/*自动加载使能*/


/* MCPWM0_CNT1	 第二组PWM设置*/
	

	/* MCPWM0_CNT1 */
	MCPWM_InitStructure_M1.BASE_CNT1_EN = DISABLE;				  /* 主计数器开始计数使能开关 */
	MCPWM_InitStructure_M1.TH1 = prd;			  /* 计数周期设置即MCPWM输出周期*/
	
	
/*begin***********************jack add***********************/
	MCPWM_InitStructure_M1.TH30 =  (uint16_t)(-EPWM_HALFPERIOD);  		//jack add
	MCPWM_InitStructure_M1.TH31 =  EPWM_HALFPERIOD;  			 		//jack add
	MCPWM_InitStructure_M1.TH40 =  (uint16_t)(-EPWM_HALFPERIOD);  		//jack add
	MCPWM_InitStructure_M1.TH41 =  EPWM_HALFPERIOD;   			 		//jack add
	MCPWM_InitStructure_M1.TH50 =  (uint16_t)(-EPWM_HALFPERIOD);  		//jack add
	MCPWM_InitStructure_M1.TH51 =  EPWM_HALFPERIOD;   			 		//jack add
/*end***********************jack add***********************/

	MCPWM_InitStructure_M1.MCPWM_WorkModeCH3 = MCPWM0_CENTRAL_PWM_MODE; /* MCPWM CH0工作模式：中心对齐PWM模式 */
	MCPWM_InitStructure_M1.MCPWM_WorkModeCH4 = MCPWM0_CENTRAL_PWM_MODE; /* 通道工作模式设置，中心对齐或边沿对齐 */
	MCPWM_InitStructure_M1.MCPWM_WorkModeCH5 = MCPWM0_CENTRAL_PWM_MODE;

	MCPWM_InitStructure_M1.DeadTimeCH345N = deadTime;
	MCPWM_InitStructure_M1.DeadTimeCH345P = deadTime;
	MCPWM_InitStructure_M1.CMP_CTRL_CNT1  = DISABLE ;		/* CMP控制CNT1失能 */
	MCPWM_InitStructure_M1.EVT_CNT1_EN	  = DISABLE;		//ENABLE ;		/* MCPWM_CNT1外部触发时使能 */  //jack changed
	MCPWM_InitStructure_M1.EVT1 		  = DISABLE;		//BIT1 ;			/* MCPWM0_TMR0 事件触发时基 1 开始计数 */	//jack changed

	MCPWM_InitStructure_M1.TR1_UP_INTV	= DISABLE;
	MCPWM_InitStructure_M1.TR1_T0_UpdateEN = ENABLE ;
	MCPWM_InitStructure_M1.TR1_T1_UpdateEN = DISABLE ;
	MCPWM_InitStructure_M1.TR1_AEC		= DISABLE;

	MCPWM_InitStructure_M1.T0_Update1_INT_EN = ENABLE;   /* CNT1 T0更新事件 中断使能 */
	MCPWM_InitStructure_M1.T1_Update1_INT_EN = DISABLE ;  /* CNT1 T1更新事件 中断关闭  */
	MCPWM_InitStructure_M1.Update1_INT_EN 	 = DISABLE;	   /* CNT1 更新事件 中断使能  */


	MCPWM_InitStructure_M1.TMR2 = (u16)(40 - prd); /* MCPWM_TMR2  设置 */
	MCPWM_InitStructure_M1.TMR3 = (u16)(prd - 10);	/* MCPWM_TMR3  设置 */

#if (CNT1_PRE_DRIVER_POLARITY == P_HIGH__N_LOW)                    /* CHxP 高有效， CHxN低电平有效 */
	MCPWM_InitStructure_M1.CH3N_Polarity_INV = ENABLE;		  /* CH0N通道输出极性设置 | 正常输出或取反输出*/
	MCPWM_InitStructure_M1.CH3P_Polarity_INV = DISABLE; 	  /* CH0P通道输出极性设置 | 正常输出或取反输出 */
	MCPWM_InitStructure_M1.CH4N_Polarity_INV = ENABLE;
	MCPWM_InitStructure_M1.CH4P_Polarity_INV = DISABLE;
	MCPWM_InitStructure_M1.CH5N_Polarity_INV = ENABLE;
	MCPWM_InitStructure_M1.CH5P_Polarity_INV = DISABLE;

	MCPWM_InitStructure_M1.Switch_CH3N_CH3P =	DISABLE;		   /* 通道交换选择设置 | CH0P和CH0N是否选择信号交换 */
	MCPWM_InitStructure_M1.Switch_CH4N_CH4P =	DISABLE;		   /* 通道交换选择设置 */
	MCPWM_InitStructure_M1.Switch_CH5N_CH5P =	DISABLE;		   /* 通道交换选择设置 */

	/* 默认电平设置 默认电平输出不受MCPWM_IO01和MCPWM_IO23的 BIT0、BIT1、BIT8、BIT9、BIT6、BIT14
													 通道交换和极性控制的影响，直接控制通道输出 */
	MCPWM_InitStructure_M1.CH3P_default_output = MCPWM0_LOW_LEVEL;
	MCPWM_InitStructure_M1.CH3N_default_output = MCPWM0_HIGH_LEVEL;
	MCPWM_InitStructure_M1.CH4P_default_output = MCPWM0_LOW_LEVEL;	 /* CH1P对应引脚在空闲状态输出低电平 */
	MCPWM_InitStructure_M1.CH4N_default_output = MCPWM0_HIGH_LEVEL;  /* CH1N对应引脚在空闲状态输出高电平 */
	MCPWM_InitStructure_M1.CH5P_default_output = MCPWM0_LOW_LEVEL;
	MCPWM_InitStructure_M1.CH5N_default_output = MCPWM0_HIGH_LEVEL;
#else
#if (CNT1_PRE_DRIVER_POLARITY == P_HIGH__N_HIGH)                    /* CHxP 高有效， CHxN高电平有效 */
	MCPWM_InitStructure_M1.CH3N_Polarity_INV = DISABLE; 	   		/* CH0N通道输出极性设置 | 正常输出或取反输出*/
	MCPWM_InitStructure_M1.CH3P_Polarity_INV = DISABLE; 	  		/* CH0P通道输出极性设置 | 正常输出或取反输出 */
	MCPWM_InitStructure_M1.CH4N_Polarity_INV = DISABLE;
	MCPWM_InitStructure_M1.CH4P_Polarity_INV = DISABLE;
	MCPWM_InitStructure_M1.CH5N_Polarity_INV = DISABLE;
	MCPWM_InitStructure_M1.CH5P_Polarity_INV = DISABLE;

	MCPWM_InitStructure_M1.Switch_CH3N_CH3P =	DISABLE;		   /* 通道交换选择设置 | CH0P和CH0N是否选择信号交换 */
	MCPWM_InitStructure_M1.Switch_CH4N_CH4P =	DISABLE;		   /* 通道交换选择设置 */
	MCPWM_InitStructure_M1.Switch_CH5N_CH5P =	DISABLE;		   /* 通道交换选择设置 */

	/* 默认电平设置 默认电平输出不受MCPWM_IO01和MCPWM_IO23的 BIT0、BIT1、BIT8、BIT9、BIT6、BIT14
													 通道交换和极性控制的影响，直接控制通道输出 */
	MCPWM_InitStructure_M1.CH3P_default_output = MCPWM0_LOW_LEVEL;
	MCPWM_InitStructure_M1.CH3N_default_output = MCPWM0_LOW_LEVEL;
	MCPWM_InitStructure_M1.CH4P_default_output = MCPWM0_LOW_LEVEL;	 /* CH1P对应引脚在空闲状态输出低电平 */
	MCPWM_InitStructure_M1.CH4N_default_output = MCPWM0_LOW_LEVEL;	  /* CH1N对应引脚在空闲状态输出高电平 */
	MCPWM_InitStructure_M1.CH5P_default_output = MCPWM0_LOW_LEVEL;
	MCPWM_InitStructure_M1.CH5N_default_output = MCPWM0_LOW_LEVEL;
#endif


	MCPWM_InitStructure.CH3N_FAIL_EN = DISABLE;//ENABLE; 	//jack add
	MCPWM_InitStructure.CH3P_FAIL_EN = DISABLE;//ENABLE;	//jack add
	MCPWM_InitStructure.CH4N_FAIL_EN = DISABLE;//ENABLE;	//jack add
	MCPWM_InitStructure.CH4P_FAIL_EN = DISABLE;//ENABLE;	//jack add
	MCPWM_InitStructure.CH5N_FAIL_EN = DISABLE;//ENABLE;	//jack add
	MCPWM_InitStructure.CH5P_FAIL_EN = DISABLE;//ENABLE;	//jack add

	MCPWM_InitStructure_M1.FAIL2_INPUT_EN	 = DISABLE ;//FAIL_0CAP
	MCPWM_InitStructure_M1.FAIL2_INT_EN  = DISABLE;
	MCPWM_InitStructure_M1.FAIL2_Signal_Sel = MCPWM0_FAIL_SEL_CMP ;//FAIL_0CAP
	MCPWM_InitStructure_M1.FAIL2_Polarity	 = MCPWM0_HIGH_LEVEL_ACTIVE ;

	MCPWM_InitStructure_M1.FAIL3_INPUT_EN	 = DISABLE;//ENABLE ;//FAIL_0CAP
	MCPWM_InitStructure_M1.FAIL3_INT_EN  = DISABLE;
	MCPWM_InitStructure_M1.FAIL3_Signal_Sel = MCPWM0_FAIL_SEL_CMP ;//FAIL_0CAP
	MCPWM_InitStructure_M1.FAIL3_Polarity	 = MCPWM0_HIGH_LEVEL_ACTIVE ;

	MCPWM_InitStructure_M1.HALT_PRT1		 = DISABLE ;
	MCPWM_InitStructure_M1.FAIL_1CAP		 = DISABLE ;
	
	//TMR2 TMR3选择时间点可以时基0或者1，比较灵活，可以组合成原来的四段式采样
	MCPWM_InitStructure_M1.TMR2_TB	   = 1;/* MCPWM TMR2时基（计数器）选择 0：时基0；1：时基1 */
	MCPWM_InitStructure_M1.TMR3_TB	   = 1;/* MCPWM TMR3时基（计数器）选择 0：时基0；1：时基1 */
	
#endif
	
	MCPWM_CH345_Init(&MCPWM_InitStructure_M1);
	
	MCPWM_CH345_IESet(&MCPWM_InitStructure_M1);
	
	//registerEPWM1_DrvLevel();
	
//#endif
}


/*******************************************************************************
 函数名称：    void DebugPWM_OutputFunction(void)
 功能描述：    PWM输出功能调试   输出25%占空比
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2017/11/5      V1.0           Howlet Li          创建
 *******************************************************************************/
void DebugPWM_OutputFunction(void)
{
    MCPWM0_TH00 = -EPWM_HALFPERIOD;
    MCPWM0_TH01 = EPWM_HALFPERIOD;
    MCPWM0_TH10 = -EPWM_HALFPERIOD;
    MCPWM0_TH11 = EPWM_HALFPERIOD;
    MCPWM0_TH20 = -EPWM_HALFPERIOD;
    MCPWM0_TH21 = EPWM_HALFPERIOD;

    PWMOutputs(ENABLE);
    while(1)
    {
    }
}

#if 1 //	(EPWM0_USED == 1)
/*******************************************************************************
 函数名称：    void GetEPWM0_breakInStatus(STR_PubDatHDMD* tS_pHd2Md)
 功能描述：    读取breakin信号
 输入参数：    tS_pHd2Md
 输出参数：    无
 返 回 值：    无
 其它说明：	   查看是否存在过流事件
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2024/5/27     V1.1           TongHua Li         整理
 *******************************************************************************/
// void GetEPWM0_breakInStatus(STR_PubDatHDMD* tS_pHd2Md)
// {
//     tS_pHd2Md->bBreakInStatus = (MCPWM0_EIF & BIT4);
// }

/*******************************************************************************
 函数名称：    void ClrEPWM0_breakInStatus(void)
 功能描述：    清除breakin信号
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：	   
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2024/5/27     V1.1           TongHua Li         整理
 *******************************************************************************/
void ClrEPWM0_breakInStatus(void)
{
    MCPWM0_EIF = (MCPWM0_EIF | BIT4);
}

/*******************************************************************************
 函数名称：    void registerEPWM0_DrvLevel(void)
 功能描述：    记录驱动极性
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：	   
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2024/5/27     V1.1           TongHua Li         整理
 *******************************************************************************/
void registerEPWM0_DrvLevel(void)
{
	//g_hDriverPolarity[0] = MCPWM0_IO01;		/* 读出驱动极性 */
}

/*******************************************************************************
 函数名称：    void EPWM0_OutPut(FuncState t_state) 
 功能描述：    MOE操作函数
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：	   TRUE---- 使能
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2024/5/27     V1.1           TongHua Li         整理
 *******************************************************************************/

void EPWM0_OutPut(FuncState t_state)
{
    MCPWM0_PRT = 0xDEAD;
    if(t_state == DISABLE)
    {
        MCPWM0_FAIL012 &= ~BIT6;
    }
    else
    {
        MCPWM0_FAIL012 |= BIT6;
    }
    MCPWM0_PRT = 0;
}

/*******************************************************************************
 函数名称：    void EPWM0_Charge(u8 t_bPhaseABC) 
 功能描述：    预充电极性控制
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：	   TRUE---- 使能
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2024/5/27     V1.1           TongHua Li         整理
 *******************************************************************************/
void EPWM0_Charge(u8 t_bPhaseABC)
{
	// volatile u16 hDriverPolarity;
	// volatile u16 hDriverPolarity0;
	// MCPWM0_PRT		= 0x0000DEAD;
	
	// if(t_bPhaseABC == 1)			//A相下桥PWM
	// {
	// 	MCPWM0_IO01			= 0x0C08 | g_hDriverPolarity[0];
		
	// 	hDriverPolarity		= MCPWM0_IO23 & 0xFF00;
	// 	hDriverPolarity0	= 0x0C | g_hDriverPolarity[0];
	// 	hDriverPolarity		|= hDriverPolarity0;			
	// 	MCPWM0_IO23		= hDriverPolarity;
	// }
	// else if(t_bPhaseABC == 2)		//B相下桥PWM
	// {
	// 	MCPWM0_IO01		= 0x080C | g_hDriverPolarity[0];
	// }
	// else if(t_bPhaseABC == 3)		//C相下桥PWM
	// {
	// 	MCPWM0_IO01		= 0x0C0C | g_hDriverPolarity[0];

	// 	hDriverPolarity		= MCPWM0_IO23 & 0xFF00;
	// 	hDriverPolarity0	= 0x08 | g_hDriverPolarity[0];
	// 	hDriverPolarity		|= hDriverPolarity0;
	// 	MCPWM0_IO23			= hDriverPolarity;
	// }

	// MCPWM0_PRT		= 0x0000;
}

void RestoreEPWM0Cfg(void)
{
	// volatile u16 hDriverPolarity;
	
	// MCPWM0_PRT 	= 0x0000DEAD;
	// MCPWM0_IO01	= g_hDriverPolarity[0];

	// hDriverPolarity = MCPWM0_IO23 & 0xFF00;
	// hDriverPolarity |= (g_hDriverPolarity[0] & 0x00FF);
	
	// MCPWM0_IO23	= hDriverPolarity;
	// MCPWM0_PRT 	= 0x0000;	
}


/*******************************************************************************
 函数名称：    void InitEPWM0ChargeEnd(void) 
 功能描述：    预充电完成后初始化硬件(PWM和ADC)
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：	   TRUE---- 使能
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2024/5/27     V1.1           TongHua Li         整理
 *******************************************************************************/
void InitEPWM0ChargeEnd(void)
{
	// __disable_irq();
	// //ADC0_init();
	// //MCPWM_ch012_init();			/* PWM初始化 */
	// //ADC0_STATE_RESET();
	// RestoreEPWM0Cfg();				/* EPWM0恢复配置 */
	// __enable_irq();
}

s16 getEpwm0DefaultSampoint0(void)
{
	s16 pwmPeriod;
	pwmPeriod	= MCPWM0_TH0;
	return (100- pwmPeriod);
}

s16 getEpwm0DefaultSampoint1(void)
{
	s16 pwmPeriod;
	pwmPeriod	= MCPWM0_TH0;
	return (800- pwmPeriod);
}



/*******************************************************************************
 函数名称：    void MCPWM0_RegUpdate(void)
 功能描述：    加载MCPWM模块占空比值，加载MCPWM模块ADC触发点寄存器值
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2020/8/5      V1.0           Howlet Li          创建
 *******************************************************************************/
void MCPWM0_RegUpdate(STR_PubDatMDHD* tS_pMdToHd)
{
//	MCPWM0_TH00 = tS_pMdToHd->mMcPwm.nuTimePhBN;
//    MCPWM0_TH01 = tS_pMdToHd->mMcPwm.nuTimePhB;
//    
//    MCPWM0_TH10 = tS_pMdToHd->mMcPwm.nuTimePhAN;
//    MCPWM0_TH11 = tS_pMdToHd->mMcPwm.nuTimePhA;
	
	  MCPWM0_TH00 = tS_pMdToHd->mMcPwm.nuTimePhAN;
    MCPWM0_TH01 = tS_pMdToHd->mMcPwm.nuTimePhA;
    
    MCPWM0_TH10 = tS_pMdToHd->mMcPwm.nuTimePhBN;
    MCPWM0_TH11 = tS_pMdToHd->mMcPwm.nuTimePhB;

    MCPWM0_TH20 = tS_pMdToHd->mMcPwm.nuTimePhCN;
    MCPWM0_TH21 = tS_pMdToHd->mMcPwm.nuTimePhC;

    MCPWM0_TMR0 = tS_pMdToHd->mMcTmr.nuPoint0;	// 第一个采样点
    MCPWM0_TMR1 = tS_pMdToHd->mMcTmr.nuPoint1;	// 第二个采样点	
}
#endif

#if	1 //(EPWM1_USED == 1)
/*******************************************************************************
 函数名称：    void GetEPWM0_breakInStatus(STR_PubDatHDMD* tS_pHd2Md)
 功能描述：    读取breakin信号
 输入参数：    tS_pHd2Md
 输出参数：    无
 返 回 值：    无
 其它说明：	   查看是否存在过流事件
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2024/5/27     V1.1           TongHua Li         整理
 *******************************************************************************/
//void GetEPWM1_breakInStatus(STR_PubDatHDMD* tS_pHd2Md)
//{
//    tS_pHd2Md->bBreakInStatus = (MCPWM0_EIF & BIT7);
//}

/*******************************************************************************
 函数名称：    void ClrEPWM1_breakInStatus(void)
 功能描述：    清除breakin信号
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：	   
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2024/5/27     V1.1           TongHua Li         整理
 *******************************************************************************/
void ClrEPWM1_breakInStatus(void)
{
    MCPWM0_EIF = (MCPWM0_EIF | BIT7);
}

/*******************************************************************************
 函数名称：    void registerEPWM1_DrvLevel(void)
 功能描述：    记录驱动极性
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：	   
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2024/5/27     V1.1           TongHua Li         整理
 *******************************************************************************/
void registerEPWM1_DrvLevel(void)
{
	// #if (PLANTFORM_DRV_MODULE_NUM == 1)
	// g_hDriverPolarity[0] = MCPWM0_IO45;		/* 读出驱动极性 */
	// #else
	// g_hDriverPolarity[1] = MCPWM0_IO45;		/* 读出驱动极性 */
	// #endif
}

/*******************************************************************************
 函数名称：    void EPWM1_OutPut(FuncState t_state) 
 功能描述：    MOE操作函数
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：	   TRUE---- 使能
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2024/5/27     V1.1           TongHua Li         整理
 *******************************************************************************/
void EPWM1_OutPut(FuncState t_state)
{
    MCPWM0_PRT = 0xDEAD;
    if(t_state == DISABLE)
    {
        MCPWM0_FAIL345 &= ~BIT6;
    }
    else
    {
        MCPWM0_FAIL345 |= BIT6;
    }
    MCPWM0_PRT = 0; 
}

/*******************************************************************************
 函数名称：    void EPWM1_Charge(u8 t_bPhaseABC) 
 功能描述：    预充电极性控制
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：	   TRUE---- 使能
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2024/5/27     V1.1           TongHua Li         整理
 *******************************************************************************/
void EPWM1_Charge(u8 t_bPhaseABC)
{
	// volatile u16 hDriverPolarity;
	// volatile u16 hDriverPolarity0;
	// MCPWM0_PRT		= 0x0000DEAD;
	
	// if(t_bPhaseABC == 1)			//A相下桥PWM
	// {
	// 	hDriverPolarity		= MCPWM0_IO23 & 0xFF;
	// 	hDriverPolarity0	= g_hDriverPolarity[1] & 0xFF00;
	// 	hDriverPolarity0	= 0x0800 | hDriverPolarity0;
	// 	hDriverPolarity		|= hDriverPolarity0;			
	// 	MCPWM0_IO23		= hDriverPolarity;		
	// 	MCPWM0_IO45		= 0x0C0C | g_hDriverPolarity[1];
	// }
	// else if(t_bPhaseABC == 2)		//B相下桥PWM
	// {
	// 	hDriverPolarity		= MCPWM0_IO23 & 0xFF;
	// 	hDriverPolarity0	= g_hDriverPolarity[1] & 0xFF00;
	// 	hDriverPolarity0	= 0x0C00 | hDriverPolarity0;
	// 	hDriverPolarity		|= hDriverPolarity0;	
	// 	MCPWM0_IO23			= hDriverPolarity;
	// 	MCPWM0_IO45			= 0x0C08 | g_hDriverPolarity[1];
	// }
	// else if(t_bPhaseABC == 3)		//C相下桥PWM
	// {
	// 	hDriverPolarity		= MCPWM0_IO23 & 0xFF;
	// 	hDriverPolarity0	= g_hDriverPolarity[1] & 0xFF00;
	// 	hDriverPolarity0	= 0x0C00 | hDriverPolarity0;
	// 	hDriverPolarity		|= hDriverPolarity0;	
	// 	MCPWM0_IO23			= hDriverPolarity;
	// 	MCPWM0_IO45			= 0x080C | g_hDriverPolarity[1];
	// }

	// MCPWM0_PRT		= 0x0000;
}

void RestoreEPWM1Cfg(void)
{
	// volatile u16 hDriverPolarity;
	// volatile u16 hDriverPolarity0;

	// MCPWM0_PRT 	= 0x0000DEAD;
	
	// hDriverPolarity = MCPWM0_IO23 & 0x00FF;

	// hDriverPolarity0 = (g_hDriverPolarity[1] & 0xFF00);
	// hDriverPolarity |= hDriverPolarity0;	
	// MCPWM0_IO23	= hDriverPolarity;
	
	// MCPWM0_IO45	= g_hDriverPolarity[1];
	// MCPWM0_PRT 	= 0x0000;	
}



/*******************************************************************************
 函数名称：    void InitEPWM1ChargeEnd(void) 
 功能描述：    预充电完成后初始化硬件(PWM和ADC)
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：	   TRUE---- 使能
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2024/5/27     V1.1           TongHua Li         整理
 *******************************************************************************/
void InitEPWM1ChargeEnd(void)
{
// 	__disable_irq();   					//  关掉全部中断，会影响另一个ad进中断
// 	//ADC1_init();
//     //MCPWM_ch345_init();				/* PWM初始化 */
// 	//ADC1_STATE_RESET();
// 	RestoreEPWM1Cfg();					/* EPWM1恢复配置 */
// 	__enable_irq();
 }

s16 getEpwm1DefaultSampoint0(void)
{
	s16 pwmPeriod;
	pwmPeriod	= MCPWM0_TH1;
	return (100- pwmPeriod);
}

s16 getEpwm1DefaultSampoint1(void)
{
	s16 pwmPeriod;
	pwmPeriod	= MCPWM0_TH1;
	return (800- pwmPeriod);
}

/*******************************************************************************
 函数名称：    void MCPWM1_RegUpdate(void)
 功能描述：    加载MCPWM模块占空比值，加载MCPWM模块ADC触发点寄存器值
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2020/8/5      V1.0           Howlet Li          创建
 *******************************************************************************/
void MCPWM1_RegUpdate(STR_PubDatMDHD* tS_pMdToHd)
{
	MCPWM0_TH30 = tS_pMdToHd->mMcPwm.nuTimePhBN;
    MCPWM0_TH31 = tS_pMdToHd->mMcPwm.nuTimePhB;
    
    MCPWM0_TH40 = tS_pMdToHd->mMcPwm.nuTimePhAN;
    MCPWM0_TH41 = tS_pMdToHd->mMcPwm.nuTimePhA;

    MCPWM0_TH50 = tS_pMdToHd->mMcPwm.nuTimePhCN;
    MCPWM0_TH51 = tS_pMdToHd->mMcPwm.nuTimePhC;

    MCPWM0_TMR2 = tS_pMdToHd->mMcTmr.nuPoint0;	// 第一个采样点
    MCPWM0_TMR3 = tS_pMdToHd->mMcTmr.nuPoint1;	// 第二个采样点	
}

#endif


/*******************************************************************************
 函数名称：    void MCPWM_CH_DEFAULT(uint16_t pwm_instance,bool enable,uint16_t value)
 功能描述：    根据输入的mcpwm_timer 0/1 判断是否需要进行引脚默认输出
 输入参数：    mcpwm_timer0或mcpwm_timer1,是否默认输出，默认输出值（只取低6位）
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2025/12/15      V1.0         Jack          创建
 *******************************************************************************/
void MCPWM_CH_DEFAULT(uint32_t mcpwm_timer,bool enable,uint32_t value)
{
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */
	if(mcpwm_timer == 0U)
	{
		if(enable == true)
		{
			MCPWM0_FAIL012 &= ~BIT6;	//0:输出MCPWM0_CH_DEF设置的CHxN_DEFAULT和CHxP_DEFAULT    1:输出 MCPWM 产生的正常信号

			MCPWM0_CH_DEF = (MCPWM0_CH_DEF & ~MCPWM0_CH_DEF_CH012_MASK) | MCPWM0_CH_DEF_CH012_SET(value);
		}
		else
		{
			MCPWM0_FAIL012 |= BIT6;	//0:输出MCPWM0_CH_DEF设置的CHxN_DEFAULT和CHxP_DEFAULT    1:输出 MCPWM 产生的正常信号
		}
	}
	else if(mcpwm_timer == 1U)
	{
		if(enable == true)
		{
			MCPWM0_FAIL345 &= ~BIT6;	//0:输出MCPWM0_CH_DEF设置的CHxN_DEFAULT和CHxP_DEFAULT    1:输出 MCPWM 产生的正常信号

			MCPWM0_CH_DEF = (MCPWM0_CH_DEF & ~MCPWM0_CH_DEF_CH345_MASK) | MCPWM0_CH_DEF_CH345_SET(value);
		}
		else
		{
			MCPWM0_FAIL345 |= BIT6;	//0:输出MCPWM0_CH_DEF设置的CHxN_DEFAULT和CHxP_DEFAULT    1:输出 MCPWM 产生的正常信号
		}
	}
	else
	{
		//do nothing 
	}

	MCPWM0_PRT = 0; /*enter password to lock write protection */

	return;
}

/* 
argv[1](channel) : channel0------channel5 
argv[2](p/n)	 : (0->channel N) or  (1->chanenl P)
argv[3](inverse) : 0->normal signal output    1->inverse signal output
*/
void mcpwm_channel_n_p_inverse_set(uint32_t channel,uint32_t channel_n_p,uint32_t inverse)
{
	MCPWM0_PRT = 0x0000DEAD;  /*enter password to unlock write protection */
	
	switch (channel)
	{
	case 0:
		if(channel_n_p)  //Channel0-P
		{
			MCPWM0_IO01 = (MCPWM0_IO01&~MCPWM0_IO01_CH0P_INVERSE_MASK)|MCPWM0_IO01_CH0P_INVERSE_MASK_SET(inverse);
		}
		else   //Channel0-N
		{
			MCPWM0_IO01 = (MCPWM0_IO01&~MCPWM0_IO01_CH0N_INVERSE_MASK)|MCPWM0_IO01_CH0N_INVERSE_MASK_SET(inverse);
		}
		break;
	case 1:
		if(channel_n_p)  //Channel1-P
		{
			MCPWM0_IO01 = (MCPWM0_IO01&~MCPWM0_IO01_CH1P_INVERSE_MASK)|MCPWM0_IO01_CH1P_INVERSE_MASK_SET(inverse);
		}
		else   //Channel1-N
		{
			MCPWM0_IO01 = (MCPWM0_IO01&~MCPWM0_IO01_CH1N_INVERSE_MASK)|MCPWM0_IO01_CH1N_INVERSE_MASK_SET(inverse);
		}
		break;
	case 2:
		if(channel_n_p)  //Channel2-P
		{
			MCPWM0_IO23 = (MCPWM0_IO23&~MCPWM0_IO23_CH2P_INVERSE_MASK)|MCPWM0_IO23_CH2P_INVERSE_MASK_SET(inverse);
		}
		else   //Channel2-N
		{
			MCPWM0_IO23 = (MCPWM0_IO23&~MCPWM0_IO01_CH0NP_SWAP_MASK)|MCPWM0_IO23_CH2N_INVERSE_MASK_SET(inverse);
		}
		break;
	case 3:
		if(channel_n_p)  //Channel3-P
		{
			MCPWM0_IO23 = (MCPWM0_IO23&~MCPWM0_IO23_CH3P_INVERSE_MASK)|MCPWM0_IO23_CH3P_INVERSE_MASK_SET(inverse);
		}
		else   //Channel3-N
		{
			MCPWM0_IO23 = (MCPWM0_IO23&~MCPWM0_IO23_CH3N_INVERSE_MASK)|MCPWM0_IO23_CH3N_INVERSE_MASK_SET(inverse);
		}
		break;
	case 4:
		if(channel_n_p)  //Channel4-P
		{
			MCPWM0_IO45 = (MCPWM0_IO45&~MCPWM0_IO45_CH4P_INVERSE_MASK)|MCPWM0_IO45_CH4P_INVERSE_MASK_SET(inverse);
		}
		else   //Channel4-N
		{
			MCPWM0_IO45 = (MCPWM0_IO45&~MCPWM0_IO45_CH4N_INVERSE_MASK)|MCPWM0_IO45_CH4N_INVERSE_MASK_SET(inverse);
		}
		break;
	case 5:
		if(channel_n_p)  //Channel5-P
		{
			MCPWM0_IO45 = (MCPWM0_IO45&~MCPWM0_IO45_CH5P_INVERSE_MASK)|MCPWM0_IO45_CH5P_INVERSE_MASK_SET(inverse);
		}
		else   //Channel5-N
		{
			MCPWM0_IO45 = (MCPWM0_IO45&~MCPWM0_IO45_CH5N_INVERSE_MASK)|MCPWM0_IO45_CH5N_INVERSE_MASK_SET(inverse);
		}
		break;
	
	default:
		break;
	}

	MCPWM0_PRT		= 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */	
}


/* 
argv[1](channel) : channel0------channel5 
argv[2](swap)    : 0->not swap channel p/n    1->swap channel p/n
*/
void mcpwm_channel_swap_set(uint32_t channel,uint32_t channel_swap)
{
	MCPWM0_PRT = 0x0000DEAD;  /*enter password to unlock write protection */

	switch (channel)
	{
	case 0:
		MCPWM0_IO01 = (MCPWM0_IO01&~MCPWM0_IO01_CH0NP_SWAP_MASK)|MCPWM0_IO01_CH0NP_SWAP_MASK_SET(channel_swap);
		break;
	case 1:
		MCPWM0_IO01 = (MCPWM0_IO01&~MCPWM0_IO01_CH1NP_SWAP_MASK)|MCPWM0_IO01_CH1NP_SWAP_MASK_SET(channel_swap);
		break;
	case 2:
		MCPWM0_IO23 = (MCPWM0_IO23&~MCPWM0_IO23_CH3NP_SWAP_MASK)|MCPWM0_IO23_CH2NP_SWAP_MASK_SET(channel_swap);
		break;
	case 3:
		MCPWM0_IO23 = (MCPWM0_IO23&~MCPWM0_IO23_CH3NP_SWAP_MASK)|MCPWM0_IO23_CH3NP_SWAP_MASK_SET(channel_swap);
		break;
	case 4:
		MCPWM0_IO45 = (MCPWM0_IO45&~MCPWM0_IO45_CH5NP_SWAP_MASK)|MCPWM0_IO45_CH4NP_SWAP_MASK_SET(channel_swap);
		break;
	case 5:
		MCPWM0_IO45 = (MCPWM0_IO45&~MCPWM0_IO45_CH5NP_SWAP_MASK)|MCPWM0_IO45_CH5NP_SWAP_MASK_SET(channel_swap);
		break;

	default:
		break;
	}

	MCPWM0_PRT		= 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */	
}

uint16_t mcpwm0_intp_cnt = 0;

void ADC_Task_CallBack_InPWM(void);

void MCPWM0_IRQHandler(void)
{

	mcpwm0_intp_cnt++;
	
	mcpwm0_int_source_e mcpwm0_interrupt_sourece;

	/* 1. 获取中断源 */
	mcpwm0_interrupt_sourece = mcpwm0_drv_status_flag_get();
	
	uint32_t interrupt_flag = MCPWM0_IF0;
	
	/* 2. 清除所有中断标志位 */
	MCPWM0_IF0 = 0xFFFFFFFFUL;
	MCPWM0_EIF = 0x000003F0UL;
	
	
#if 1
	if(interrupt_flag == ((uint32_t)0x1U<<((uint32_t)mcpwm0_interrupt_sourece)) )
	{
		//只有一个中断产生
	}
	else
	{
		//同时有多个中断产生，但是只反馈低位到高位查找第一个为1的位
	}

	/* 3. 中断回调函数 */
	if(NULL != g_mcpwm0_int_callback)
	{
		g_mcpwm0_int_callback(mcpwm0_interrupt_sourece);
	}
#endif	
	//MCPWM0_Call_Back();
	
	#ifdef ADC_TEST
		ADC_Task_CallBack_InPWM();
	#endif
}

/****************************************************************************
 ** \brief	MCPWM2_Handler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 7
****************************************************************************/
uint16_t mcpwm1_intp_cnt = 0;
void MCPWM1_IRQHandler(void)
{
	mcpwm1_intp_cnt++;
	

	mcpwm1_int_source_e mcpwm1_interrupt_sourece;

	/* 1. 获取中断源 */
	mcpwm1_interrupt_sourece = mcpwm1_drv_status_flag_get();
	
	uint32_t interrupt_flag = MCPWM0_IF1;
	
	/* 2. 清除所有中断标志位 */
	MCPWM0_IF1 = 0xFFFFFFFFUL;
	MCPWM0_EIF = 0x000003F0UL;
	
#if 1
	if(interrupt_flag == ((uint32_t)0x1U<<((uint32_t)mcpwm1_interrupt_sourece)) )
	{
		//只有一个中断产生
	}
	else
	{
		//同时有多个中断产生，但是只反低位到高位查找第一个为1的位
	}

	/* 3. 中断回调函数 */
	if(NULL != g_mcpwm1_int_callback)
	{
		g_mcpwm1_int_callback(mcpwm1_interrupt_sourece);
	}
#endif	
	
	
}




