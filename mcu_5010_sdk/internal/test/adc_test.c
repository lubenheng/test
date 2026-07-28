#include "platform_common.h"

#include "test.h"

#include "ADC_Reg.h"

#include "ADC_Type.h"
#include "ADC.h"
#include "ADC_test.h"
#include "adc_hal.h"

#include "mcpwm_reg.h"
#include "mcpwm_hal.h"
#include "mcpwm.h"
#include "mcpwm_test.h"

#include "gpio_type.h"
#include "gpio_hal.h"

#include "mc_foc.h"
#include "RCC.h"

#include "user_control.h"
	
#include "para_define.h"


#define SW_TIG_MODE (0)
#define HW_TIG_MODE (1)




struct_AdcTest_OL stru_AdcTest_OL =
{
	.STPT_CNT = 5,
	.COVT_CNT = 2,
	.SAPT_CNT = 13,
	.INIT_CNT = 3,
	.DELAY_CNT = 15,	
	
	.INTR_SRC = ADC_IRQSRC_S0CP,
	
	.GroupMode = 2,
	.TigChNum = {3,4,3,1},
	.TigSrc = {0,1,2,3},
	.TigCnt = {-(s16)EPWM_PERIOD /8 + 12,(s16)EPWM_PERIOD/4,(s16)EPWM_PERIOD/2, EPWM_PERIOD*3/4},
	.Seq = {ADC_CH0,ADC_CH1,ADC_CH2,ADC_CH3,	\
					ADC_CH4,ADC_CH5,ADC_CH6,ADC_CH7,	\
					ADC_CH8,ADC_CH9,ADC_CH10,ADC_CH11,	\
					ADC_CH12,ADC_CH13,ADC_CH14,ADC_CH15}
};


void ADC_gpio_init(void)
{
	
	gpio_settings_config_t setting_cfg;
	
	/* PTB7 PIN  */
	setting_cfg.gpio_port = GPIO_B;
	setting_cfg.gpio_pin = GPIO_PIN_7;
	setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
	setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
	setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
	setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
	setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
	gpio_init(&setting_cfg);

//	#if (ADC_STATE_SIG_TEST)
//	{
		setting_cfg.gpio_port = GPIO_B;
		setting_cfg.gpio_pin = GPIO_PIN_8;
		setting_cfg.gpio_alt = GPIO_ALT_FUNC_0;
		setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
		setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
		setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
		setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
		gpio_init(&setting_cfg);		
//	}
//	#endif
	
//	//测试模式IO
//	*(int32_t *)0x40015804 = 0x10;
//	*(int32_t *)0x48000440 = 0x0000fff0;	

	#if (ADC_STATEIO_TEST)
	{
		// READY
//		setting_cfg.gpio_port = GPIO_C;
//		setting_cfg.gpio_pin = GPIO_PIN_3;
//		setting_cfg.gpio_alt = GPIO_ALT_FUNC_14;
//		setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
//		setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//		setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
//		setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//		gpio_init(&setting_cfg);	
		setting_cfg.gpio_port = GPIO_A;
		setting_cfg.gpio_pin = GPIO_PIN_0;
		setting_cfg.gpio_alt = GPIO_ALT_FUNC_14;
		setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
		setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
		setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
		setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
		gpio_init(&setting_cfg);			
		// START
//		setting_cfg.gpio_port = GPIO_C;
//		setting_cfg.gpio_pin = GPIO_PIN_4;
//		setting_cfg.gpio_alt = GPIO_ALT_FUNC_14;
//		setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
//		setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//		setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
//		setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//		gpio_init(&setting_cfg);	
		setting_cfg.gpio_port = GPIO_A;
		setting_cfg.gpio_pin = GPIO_PIN_1;
		setting_cfg.gpio_alt = GPIO_ALT_FUNC_14;
		setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
		setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
		setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
		setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
		gpio_init(&setting_cfg);			
//		// READY
//		setting_cfg.gpio_port = GPIO_B;
//		setting_cfg.gpio_pin = GPIO_PIN_14;
//		setting_cfg.gpio_alt = GPIO_ALT_FUNC_14;
//		setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
//		setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//		setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
//		setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//		gpio_init(&setting_cfg);	
//		// START
//		setting_cfg.gpio_port = GPIO_B;
//		setting_cfg.gpio_pin = GPIO_PIN_15;
//		setting_cfg.gpio_alt = GPIO_ALT_FUNC_14;
//		setting_cfg.gpio_dir = GPIO_DIR_OUTPUT;
//		setting_cfg.pull_cfg = GPIO_PULL_DISABLE;
//		setting_cfg.gpio_strength = GPIO_STRENGTH_MODE_1;
//		setting_cfg.gpio_interrupt_cfg_info.int_handle = NULL;
//		gpio_init(&setting_cfg);		
		
	}
	#endif
}
//-------------------------------------------------------------//
// function test



/*****************************************************************************
 ** \brief	ADC_MCPWMTrig_Test_Init
 **					ADC MCPWM触发测试初始化
 ** \param [in] 
					none
 ** \return  none
 ** \note	 
*****************************************************************************/


void ADC_MCPWMTrig_Test_Init(void)
{
	//------------------------------------------------------------------------/		
	// ADC INIT
	//enable adc
	ADC_ENABLE(ENABLE);
	// set group mode
	ADC_SetSampleGrpMode(ADC_MCPWM_TRG_GROUP);
	
	// set group number
	ADC_SetGrpChCount(ADC_Sample_Group_0,1);
	ADC_SetGrpChCount(ADC_Sample_Group_1,3);
	ADC_SetGrpChCount(ADC_Sample_Group_2,0);
	ADC_SetGrpChCount(ADC_Sample_Group_3,0);	

	
	// set seq
	ADC_SEQx_CFG(ADC_SEQ0,ADC_CH7);//
	ADC_SEQx_CFG(ADC_SEQ1,ADC_CH1);
	ADC_SEQx_CFG(ADC_SEQ2,ADC_CH2);
	ADC_SEQx_CFG(ADC_SEQ3,ADC_CH3);	
	
	ADC_SEQx_CFG(ADC_SEQ4,ADC_CH4);
	ADC_SEQx_CFG(ADC_SEQ5,ADC_CH5);		
	ADC_SEQx_CFG(ADC_SEQ6,ADC_CH6);
	ADC_SEQx_CFG(ADC_SEQ7,ADC_CH7);	
	
	ADC_SEQx_CFG(ADC_SEQ8,ADC_CH8);
	ADC_SEQx_CFG(ADC_SEQ9,ADC_CH9);		
	ADC_SEQx_CFG(ADC_SEQ10,ADC_CH10);	
	ADC_SEQx_CFG(ADC_SEQ11,ADC_CH11);	
	
	ADC_SEQx_CFG(ADC_SEQ12,ADC_CH12);
	ADC_SEQx_CFG(ADC_SEQ13,ADC_CH13);		
	ADC_SEQx_CFG(ADC_SEQ14,ADC_CH14);	
	ADC_SEQx_CFG(ADC_SEQ15,ADC_CH15);
	// set trigger
	#if (ADC_SOFTTRIG_TEST)
	{
		ADC_SetGrpTrigSource(ADC_SOFTTRIG_GROUP,ADC_GrpTrig_SW_TRIG);
		
		#if (ADC_SOFTTRIG_ERR_TEST)
		{
			ADC_SetGrpTrigSource(ADC_Sample_Group_0,ADC_GrpTrig_SW_TRIG);
			ADC_SetGrpTrigSource(ADC_Sample_Group_1,ADC_GrpTrig_SW_TRIG);
		}
		#endif
	}
	#else
	{
		ADC_SetGrpTrigSource(ADC_Sample_Group_0,ADC_GrpTrig_MCPWM_TRIG0);
		ADC_SetGrpTrigSource(ADC_Sample_Group_1,ADC_GrpTrig_MCPWM_TRIG1);
		ADC_SetGrpTrigSource(ADC_Sample_Group_2,ADC_GrpTrig_MCPWM_TRIG2);
		ADC_SetGrpTrigSource(ADC_Sample_Group_3,ADC_GrpTrig_MCPWM_TRIG3);		
	}
	#endif

	ADC_SetCLK_STPT(4);
	ADC_SetCLK_COVT(2);
	ADC_SetCLK_INIT(0);
	ADC_SetCLK_SAPT(13);
	ADC_SetCLK_TrigDelay(2);			
	//set delay clk
//	ADC_SetCLK_STPT(4);
//	ADC_SetCLK_COVT(2);
//	ADC_SetCLK_INIT(0);
//	ADC_SetCLK_SAPT(13);

//	ADC_SetCLK_INIT(3);			
//	ADC_SetCLK_STPT(5);
//	ADC_SetCLK_COVT(2);
//	ADC_SetCLK_SAPT(13);
//	ADC_SetCLK_TrigDelay(15);		
	//------------------------------------------------------------------------/		
	// MCPWM INIT	
	
	
	MCPWM_ch012_init();
	mcpwm0_gpio_init();
	
	NVIC_SetPriority(MCPWM0_IRQn, 2); /*TIMER0优先级配置 */
	NVIC_ClearPendingIRQ(MCPWM0_IRQn);
	NVIC_EnableIRQ (MCPWM0_IRQn);
	
	// 触发源23设置
	MCPWM0_PRT = 0x0000DEAD;
	// 23选择时基0
	MCPWM0->TCLK &= ~(0x3<<4);
	MCPWM0->TCLK |= (0x0<<4);
	// 自动更新
	MCPWM0->AUEN &= ~(0x3 << 24);
	MCPWM0->AUEN |= (0x3 << 24);
	MCPWM0_PRT		= 0x0000CAFE; 
	
	// 设置采样触发点。

	MCPWM_SetTrigCnt(MCPWM_TIG_TMR0,stru_AdcTest_OL.TigCnt[0]);
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR1,stru_AdcTest_OL.TigCnt[1]);
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR2,stru_AdcTest_OL.TigCnt[2]);
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR3,stru_AdcTest_OL.TigCnt[3]);
	
	ADC_HWTrig_PWM_Config(-EPWM_HALFPERIOD);
	
	
	#if (ADC_Err_Test)
	{
		ADC_Enable_IrqSrc(ADC_IRQSRC_TGT_ERR);
	}
	#endif
}





/*****************************************************************************
 ** \brief	ADCTest_Config_OL
 **					ADC 触发点在线测试
 ** \param [in] 
					none
 ** \return  none
 ** \note	 
*****************************************************************************/
void ADCTest_Config_OL(void)
{
	
	#if (ADC_Test_Mode == ADC_INTR_TEST)
	{
		if(stru_AdcTest_OL.EnINTR)
		{
			gpio_output_set(GPIO_B,GPIO_PIN_7,GPIO_HIGH_LEVEL);
			stru_AdcTest_OL.EnINTR = 0;
			ADC_Enable_IrqSrc(stru_AdcTest_OL.INTR_SRC);			
		}
		
		if(stru_AdcTest_OL.DisINTR)
		{
			gpio_output_set(GPIO_B,GPIO_PIN_7,GPIO_HIGH_LEVEL);
			stru_AdcTest_OL.DisINTR = 0;
			ADC_Disable_IrqSrc(stru_AdcTest_OL.INTR_SRC);			
		}
	}
	#endif
	
	u8 i=0;
	if(stru_AdcTest_OL.update )
	{
		gpio_output_set(GPIO_B,GPIO_PIN_7,GPIO_HIGH_LEVEL);

		// 设置分组模式
		#if (ADC_GROUPMODE_CHANGE_TEST)
		{
			ADC_SetSampleGrpMode((1 << stru_AdcTest_OL.GroupMode) - 1);		
		}
		#endif


		#if (ADC_CONFIG_CNT_TEST)
		{
			ADC_SetCLK_INIT(stru_AdcTest_OL.INIT_CNT);			
			ADC_SetCLK_STPT(stru_AdcTest_OL.STPT_CNT);
			ADC_SetCLK_COVT(stru_AdcTest_OL.COVT_CNT);
			ADC_SetCLK_SAPT(stru_AdcTest_OL.SAPT_CNT);
			ADC_SetCLK_TrigDelay(stru_AdcTest_OL.DELAY_CNT);				
		}
		#endif

		
		#if (ADC_SEQNUM_CHANGE_TEST)
		{
			ADC_SetGrpChCount(ADC_Sample_Group_0,stru_AdcTest_OL.TigChNum[0]);
			ADC_SetGrpChCount(ADC_Sample_Group_1,stru_AdcTest_OL.TigChNum[1]);
			ADC_SetGrpChCount(ADC_Sample_Group_2,stru_AdcTest_OL.TigChNum[2]);
			ADC_SetGrpChCount(ADC_Sample_Group_3,stru_AdcTest_OL.TigChNum[3]);		
		}
		#endif

		
//		// 设置seqx对应通道
		
		#if (ADC_SEQCH_CHANGE_TEST)
		{
			for(i = 0;i < 16;i++)
			{
				ADC_SEQx_CFG(i,stru_AdcTest_OL.Seq[i]);	
			}
		}
		#endif
	
		#if (ADC_TRIGSRC_CHANGE_TEST)
		{
			ADC_SetGrpTrigSource(ADC_Sample_Group_0,stru_AdcTest_OL.TigSrc[0]);
			ADC_SetGrpTrigSource(ADC_Sample_Group_1,stru_AdcTest_OL.TigSrc[1]);
			ADC_SetGrpTrigSource(ADC_Sample_Group_2,stru_AdcTest_OL.TigSrc[2]);
			ADC_SetGrpTrigSource(ADC_Sample_Group_3,stru_AdcTest_OL.TigSrc[3]);		
		}
		#endif

		// 设置MCPWM触发源Cnt值
		#if (ADC_TRIGCNT_CHANGE_TEST)
		{
			MCPWM_SetTrigCnt(MCPWM_TIG_TMR0,stru_AdcTest_OL.TigCnt[0]);
			MCPWM_SetTrigCnt(MCPWM_TIG_TMR1,stru_AdcTest_OL.TigCnt[1]);
			MCPWM_SetTrigCnt(MCPWM_TIG_TMR2,stru_AdcTest_OL.TigCnt[2]);
			MCPWM_SetTrigCnt(MCPWM_TIG_TMR3,stru_AdcTest_OL.TigCnt[3]);		
			
		}
		#endif


		stru_AdcTest_OL.update = 0;
	
	}
	


	#if (ADC_SOFTTRIG_TEST)
	{
		if(stru_AdcTest_OL.SWTigCtrl)
		{
			gpio_output_set(GPIO_B,GPIO_PIN_7,GPIO_HIGH_LEVEL);
			stru_AdcTest_OL.SWTigCtrl = 0;
			ADC_SWTrig();
			gpio_output_set(GPIO_B,GPIO_PIN_7,GPIO_LOW_LEVEL);
		}		
		
		#if (ADC_STATE_SIG_TEST)
		{
			while(!ADC->IR.RDY)
			{
				gpio_output_set(GPIO_B,GPIO_PIN_8,GPIO_HIGH_LEVEL);
			}
			gpio_output_set(GPIO_B,GPIO_PIN_8,GPIO_LOW_LEVEL);			
		}
		#endif
	}
	#endif
	gpio_output_set(GPIO_B,GPIO_PIN_7,GPIO_LOW_LEVEL);
	ADC_GetResult(0);
}




























//-------------------------------------------------------------//
// INTR test


/*****************************************************************************
 ** \brief	ADC_INTR_Test_Init
 **					中断测试初始化
 ** \param [in] 
					none
 ** \return  none
 ** \note	 
*****************************************************************************/



/*****************************************************************************
 ** \brief	ADC_INTR_Test_Init
 **			
 ** \param [in] 
			Delay_cnt: 0-255
 ** \return  none
 ** \note	 
*****************************************************************************/
void ADC_INTR_Test_Init(void)
{
	//------------------------------------------------------------------------/		
	// ADC INIT
	//enable adc
	ADC_ENABLE(ENABLE);
	// set group mode
	ADC_SetSampleGrpMode(ADC_MCPWM_TRG_GROUP);
	
	// set group number
	ADC_SetGrpChCount(ADC_Sample_Group_0,1);
	ADC_SetGrpChCount(ADC_Sample_Group_1,2);
	ADC_SetGrpChCount(ADC_Sample_Group_2,3);
	ADC_SetGrpChCount(ADC_Sample_Group_3,10);	

	
	// set seq
	ADC_SEQx_CFG(ADC_SEQ0,ADC_CH0);
	ADC_SEQx_CFG(ADC_SEQ1,ADC_CH1);
	ADC_SEQx_CFG(ADC_SEQ2,ADC_CH2);
	ADC_SEQx_CFG(ADC_SEQ3,ADC_CH3);	
	
	ADC_SEQx_CFG(ADC_SEQ4,ADC_CH4);
	ADC_SEQx_CFG(ADC_SEQ5,ADC_CH5);		
	ADC_SEQx_CFG(ADC_SEQ6,ADC_CH6);
	ADC_SEQx_CFG(ADC_SEQ7,ADC_CH7);	
	
	ADC_SEQx_CFG(ADC_SEQ8,ADC_CH8);
	ADC_SEQx_CFG(ADC_SEQ9,ADC_CH9);		
	ADC_SEQx_CFG(ADC_SEQ10,ADC_CH10);	
	ADC_SEQx_CFG(ADC_SEQ11,ADC_CH11);	
	
	ADC_SEQx_CFG(ADC_SEQ12,ADC_CH12);
	ADC_SEQx_CFG(ADC_SEQ13,ADC_CH13);		
	ADC_SEQx_CFG(ADC_SEQ14,ADC_CH14);	
	ADC_SEQx_CFG(ADC_SEQ15,ADC_CH15);
	
	ADC_Set_AWChannel(ADC_SEQ7);
	ADC_Set_AW_HTH(3767);	//2.3V
	ADC_Set_AW_LTH(442);	//0.27V
	
	// set trigger
	ADC_SetGrpTrigSource(ADC_Sample_Group_0,ADC_GrpTrig_MCPWM_TRIG0);
	ADC_SetGrpTrigSource(ADC_Sample_Group_1,ADC_GrpTrig_MCPWM_TRIG1);
	ADC_SetGrpTrigSource(ADC_Sample_Group_2,ADC_GrpTrig_MCPWM_TRIG2);
	ADC_SetGrpTrigSource(ADC_Sample_Group_3,ADC_GrpTrig_MCPWM_TRIG3);	

//	ADC_SetGrpTrigSource(ADC_Sample_Group_0,ADC_GrpTrig_SW_TRIG);
//	ADC_SetGrpTrigSource(ADC_Sample_Group_1,ADC_GrpTrig_SW_TRIG);
//	ADC_SetGrpTrigSource(ADC_Sample_Group_2,ADC_GrpTrig_SW_TRIG);
//	ADC_SetGrpTrigSource(ADC_Sample_Group_3,ADC_GrpTrig_SW_TRIG);	
	//set delay clk
	ADC_SetCLK_STPT(4);
	ADC_SetCLK_COVT(2);
	ADC_SetCLK_INIT(0);
	ADC_SetCLK_SAPT(13);
	ADC_SetCLK_TrigDelay(2);	
	//------------------------------------------------------------------------/		
	// set IrqSrc

//	ADC_Enable_IrqSrc(ADC_IRQSRC_AWD);
//	ADC_Enable_IrqSrc(ADC_IRQSRC_S0CP);
	// 
	NVIC_SetPriority(ADC_IRQn, 1); 
	NVIC_ClearPendingIRQ(ADC_IRQn);
	NVIC_EnableIRQ (ADC_IRQn);	

	//------------------------------------------------------------------------/		
	// ????¡ä£¤¡¤¡é¦Ì? 

	// MCPWM INIT
	MCPWM_ch012_init();
	mcpwm0_gpio_init();

	NVIC_SetPriority(MCPWM0_IRQn, 2);
	NVIC_ClearPendingIRQ(MCPWM0_IRQn);
	NVIC_EnableIRQ (MCPWM0_IRQn);

	// ¡ä£¤¡¤¡é?¡ä23¨¦¨¨??
	MCPWM0_PRT = 0x0000DEAD;
	// 23????¨º¡À?¨´0
	MCPWM0->TCLK &= ~(0x3<<4);
	MCPWM0->TCLK |= (0x0<<4);
	// ¡Á??¡¥?¨¹D?
	MCPWM0->AUEN &= ~(0x3 << 24);
	MCPWM0->AUEN |= (0x3 << 24);
	MCPWM0_PRT		= 0x0000CAFE; 
	
	MCPWM0_PRT = 0x0000DEAD;
	MCPWM0->TH20 = -(EPWM_PERIOD * 204 >> 10);
	MCPWM0->TH21 =  (EPWM_PERIOD * 204 >> 10);
	MCPWM0->TH10 = -(EPWM_PERIOD * 307 >> 10);
	MCPWM0->TH11 =  (EPWM_PERIOD * 307 >> 10);
	MCPWM0->TH00 = -(EPWM_PERIOD * 409 >> 10);
	MCPWM0->TH01 =  (EPWM_PERIOD * 409 >> 10);		
	MCPWM0_PRT		= 0x0000CAFE;
	MCPWM0_PRT = 0xDEAD; MCPWM0_FAIL012 |= BIT6;MCPWM0_PRT = 0x0;
	
	//设置触发点
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR0,stru_AdcTest_OL.TigCnt[0]);
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR1,stru_AdcTest_OL.TigCnt[1]);
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR2,stru_AdcTest_OL.TigCnt[2]);
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR3,stru_AdcTest_OL.TigCnt[3]);
	
	ADC_HWTrig_PWM_Config(EPWM_HALFPERIOD + EPWM_CPMTG);
}


/*****************************************************************************
 ** \brief	ADC_INTR_Update_OLinMain
 **			
 ** \param [in] 
			Delay_cnt: 0-255
 ** \return  none
 ** \note	 
*****************************************************************************/
s16 ADCDATA_Idea[16]=
{
	2027,
	1355,1352,
	1576,1575,1574,
	2959,2959,2959,2959,
	0,0,0,0,0,0

};
s16 ADCData[16];
#define ABSFUN(x)		((x>0)? (x):(-x))
s16 adcerrbuff =0;
void ADC_Data_Check(void)
{
	
	
	
	for(char i=0;i<16;i++)
	{
		ADCData[i] = ADC_GetResult(i);	
//		adcerrbuff = ABSFUN(ADCData[i]-ADCDATA_Idea[i]);
//		if( adcerrbuff > 20)
//		{
//			stru_AdcTest_OL.ErrFlag =1;
//		}
	}
}



/*****************************************************************************
 ** \brief	ADC_INTR_Update_OLinMain
 **			
 ** \param [in] 	none
		call in ADC INTR
 ** \return  none
 ** \note	 
*****************************************************************************/

void ADC_INTR_Process(void)
{

	
}












//-------------------------------------------------------------//
// APP test


/*****************************************************************************
 ** \brief	UniShunt_ADC_Init
 **			ADC_EN??ADC_RDY???????
 ** \param [in] 
			LTH: 0-4095
 ** \return  none
 ** \note	 HTH > LTH
*****************************************************************************/

void UniShunt_ADC_Init(void)
{
	//------------------------------------------------------------------------/		
	// ADC INIT
	//enable adc
	ADC_ENABLE(ENABLE);
	// set group mode
	ADC_SetSampleGrpMode(SampleGrpMode_3);
	
	// set group number
	ADC_SetGrpChCount(ADC_Sample_Group_0,1);
	ADC_SetGrpChCount(ADC_Sample_Group_1,1);
	ADC_SetGrpChCount(ADC_Sample_Group_2,3);
	ADC_SetGrpChCount(ADC_Sample_Group_3,0);	

	
	// set seq
	ADC_SEQx_CFG(ADC_SEQ0,ADC_CH3);
	
	ADC_SEQx_CFG(ADC_SEQ1,ADC_CH3);
	
	ADC_SEQx_CFG(ADC_SEQ2,ADC_CH5);
	ADC_SEQx_CFG(ADC_SEQ3,ADC_CH6);	
	ADC_SEQx_CFG(ADC_SEQ4,ADC_CH7);

	// set trigger
	ADC_SetGrpTrigSource(ADC_Sample_Group_0,ADC_GrpTrig_MCPWM_TRIG0);
	ADC_SetGrpTrigSource(ADC_Sample_Group_1,ADC_GrpTrig_MCPWM_TRIG1);
	ADC_SetGrpTrigSource(ADC_Sample_Group_2,ADC_GrpTrig_MCPWM_TRIG2);
	ADC_SetGrpTrigSource(ADC_Sample_Group_3,ADC_GrpTrig_MCPWM_TRIG3);	

	//set delay clk
	ADC_SetCLK_STPT(4);
	ADC_SetCLK_COVT(2);
	ADC_SetCLK_INIT(0);
	ADC_SetCLK_SAPT(13);


	//------------------------------------------------------------------------/		
	// MCPWM INIT
	MCPWM_ch012_init();
	mcpwm0_gpio_init();
	
	NVIC_SetPriority(MCPWM0_IRQn, 1); /*TIMER0优先级配置 */
	NVIC_ClearPendingIRQ(MCPWM0_IRQn);
	NVIC_EnableIRQ (MCPWM0_IRQn);
	
	// 触发源23设置
	MCPWM0_PRT = 0x0000DEAD;
	// 23选择时基0
	MCPWM0->TCLK &= ~(0x3<<4);
	MCPWM0->TCLK |= (0x0<<4);
	// 自动更新
	MCPWM0->AUEN &= ~(0x3 << 24);
	MCPWM0->AUEN |= (0x3 << 24);
	MCPWM0_PRT		= 0x0000CAFE; 
	
	// 设置采样触发点。
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR0,500);
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR1,900);
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR2,-((int16_t )EPWM_HALFPERIOD >> 1));
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR3,-((int16_t )EPWM_HALFPERIOD >> 2));
	
	ADC_HWTrig_PWM_Config(0);
}


/*****************************************************************************
 ** \brief	BiShunt_ADC_Init
 **			ADC_EN??ADC_RDY???????
 ** \param [in] 
			LTH: 0-4095
 ** \return  none
 ** \note	 HTH > LTH
*****************************************************************************/

void BiShunt_ADC_Init(void)
{
	//------------------------------------------------------------------------/		
	// ADC INIT
	//enable adc
	ADC_ENABLE(ENABLE);
	// set group mode
	ADC_SetSampleGrpMode(SampleGrpMode_2);
	
	// set group number
	ADC_SetGrpChCount(ADC_Sample_Group_0,2);
	ADC_SetGrpChCount(ADC_Sample_Group_1,3);
	ADC_SetGrpChCount(ADC_Sample_Group_2,0);
	ADC_SetGrpChCount(ADC_Sample_Group_3,0);	

	
	// set seq
	ADC_SEQx_CFG(ADC_SEQ0,ADC_CH0);
	ADC_SEQx_CFG(ADC_SEQ1,ADC_CH1);
	
	ADC_SEQx_CFG(ADC_SEQ2,ADC_CH5);
	ADC_SEQx_CFG(ADC_SEQ3,ADC_CH6);	
	ADC_SEQx_CFG(ADC_SEQ4,ADC_CH7);

	// set trigger
	ADC_SetGrpTrigSource(ADC_Sample_Group_0,ADC_GrpTrig_MCPWM_TRIG0);
	ADC_SetGrpTrigSource(ADC_Sample_Group_1,ADC_GrpTrig_MCPWM_TRIG1);
	ADC_SetGrpTrigSource(ADC_Sample_Group_2,ADC_GrpTrig_MCPWM_TRIG2);
	ADC_SetGrpTrigSource(ADC_Sample_Group_3,ADC_GrpTrig_MCPWM_TRIG3);	

	//set delay clk
	ADC_SetCLK_INIT(3);			
	ADC_SetCLK_STPT(5);
	ADC_SetCLK_COVT(2);
	ADC_SetCLK_SAPT(16);
	ADC_SetCLK_TrigDelay(21);


	//------------------------------------------------------------------------/		
	// MCPWM INIT
	MCPWM_ch012_init();
	mcpwm0_gpio_init();
	
	NVIC_SetPriority(MCPWM0_IRQn, 2); /*TIMER0¨®??¨¨?????? */
	NVIC_ClearPendingIRQ(MCPWM0_IRQn);
	NVIC_EnableIRQ (MCPWM0_IRQn);
	
	// ¡ä£¤¡¤¡é?¡ä23¨¦¨¨??
	MCPWM0_PRT = 0x0000DEAD;
	// 23????¨º¡À?¨´0
	MCPWM0->TCLK &= ~(0x3<<4);
	MCPWM0->TCLK |= (0x0<<4);
	// ¡Á??¡¥?¨¹D?
	MCPWM0->AUEN &= ~(0x3 << 24);
	MCPWM0->AUEN |= (0x3 << 24);
	MCPWM0_PRT		= 0x0000CAFE; 
	
	// ¨¦¨¨??2¨¦?¨´¡ä£¤¡¤¡é¦Ì??¡ê
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR0,EPWM_PERIOD - EPWM_CPMTG);
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR1,0);

	
//	ADC_HWTrig_PWM_Config(EPWM_HALFPERIOD);



}




/*****************************************************************************
 ** \brief	BiShunt_ADC_Init
 **			ADC_EN??ADC_RDY???????
 ** \param [in] 
			LTH: 0-4095
 ** \return  none
 ** \note	 HTH > LTH
*****************************************************************************/

void TriShunt_ADC_Init(void)
{
	//------------------------------------------------------------------------/		
	// ADC INIT
	//enable adc
	ADC_ENABLE(ENABLE);
	// set group mode
	ADC_SetSampleGrpMode(SampleGrpMode_2);
	
	// set group number
	ADC_SetGrpChCount(ADC_Sample_Group_0,3);
	ADC_SetGrpChCount(ADC_Sample_Group_1,3);
	ADC_SetGrpChCount(ADC_Sample_Group_2,0);
	ADC_SetGrpChCount(ADC_Sample_Group_3,0);	

	
	// set seq
	ADC_SEQx_CFG(ADC_SEQ0,ADC_CH0);
	ADC_SEQx_CFG(ADC_SEQ1,ADC_CH1);
	ADC_SEQx_CFG(ADC_SEQ2,ADC_CH2);
	
	ADC_SEQx_CFG(ADC_SEQ3,ADC_CH5);
	ADC_SEQx_CFG(ADC_SEQ4,ADC_CH6);	
	ADC_SEQx_CFG(ADC_SEQ5,ADC_CH7);

	// set trigger
	ADC_SetGrpTrigSource(ADC_Sample_Group_0,ADC_GrpTrig_MCPWM_TRIG0);
	ADC_SetGrpTrigSource(ADC_Sample_Group_1,ADC_GrpTrig_MCPWM_TRIG1);
	ADC_SetGrpTrigSource(ADC_Sample_Group_2,ADC_GrpTrig_MCPWM_TRIG2);
	ADC_SetGrpTrigSource(ADC_Sample_Group_3,ADC_GrpTrig_MCPWM_TRIG3);	

	//set delay clk
	ADC_SetCLK_INIT(3);			
	ADC_SetCLK_STPT(5);
	ADC_SetCLK_COVT(2);
	ADC_SetCLK_SAPT(16);
	ADC_SetCLK_TrigDelay(21);


	//------------------------------------------------------------------------/		
	// MCPWM INIT
	MCPWM_ch012_init();
	mcpwm0_gpio_init();
	
	NVIC_SetPriority(MCPWM0_IRQn, 2); /*TIMER0优先级配置 */
	NVIC_ClearPendingIRQ(MCPWM0_IRQn);
	NVIC_EnableIRQ (MCPWM0_IRQn);
	
	// 触发源23设置
	MCPWM0_PRT = 0x0000DEAD;
	// 23选择时基0
	MCPWM0->TCLK &= ~(0x3<<4);
	MCPWM0->TCLK |= (0x0<<4);
	// 自动更新
	MCPWM0->AUEN &= ~(0x3 << 24);
	MCPWM0->AUEN |= (0x3 << 24);
	MCPWM0_PRT		= 0x0000CAFE; 
	
	// 设置采样触发点。
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR0,EPWM_PERIOD - EPWM_CPMTG);
	MCPWM_SetTrigCnt(MCPWM_TIG_TMR1,0);

	
	ADC_HWTrig_PWM_Config(EPWM_HALFPERIOD);
}



//-------------------------------------------------------------//


/*****************************************************************************
 ** \brief	ADC_Task_CallBack_InPWM
 **			ADC test test in PWM
 ** \param [in] 
			LTH: 0-4095
 ** \return  none
 ** \note	 HTH > LTH
*****************************************************************************/

void ADC_Task_CallBack_InPWM(void)
{

	FOC_Control_Process();
	
	ADCTest_Config_OL();
}

/*****************************************************************************
 ** \brief	ADC_HWTrig_PWM_Config
 **					ADC 硬件触发配置
 ** \param [in] 
					none
 ** \return  none
 ** \note	 
*****************************************************************************/
void ADC_HWTrig_PWM_Config(int InitCnt)
{
	//1.stop mcpwm cntr
	MCPWM0_TCLK &= ~(0x03<<6);
	
	//2.set mcpwm tmrx
//	ADCTest_Config_OL();
	//3.adc rst
//	RCC_Reset_ADC();
	
	//4.set mcpwm tmrx,CNT value,updata
	MCPWM0_CNT0 = (uint16_t)(InitCnt);
	MCPWM0_CNT1 = (uint16_t)(InitCnt);
	
	MCPWM0_UPDATE |= (0x01<<11);
	MCPWM0_UPDATE |= (0x01<<27);
	
	//5.enable mcpwm cntr
	MCPWM0_TCLK |= (0x01<<6);

}

/*****************************************************************************
 ** \brief	ADC_TrigSW_SW2HW_Config
 **					软件触发切硬件触发
 ** \param [in] 
					none
 ** \return  none
 ** \note	 
*****************************************************************************/
void ADC_TrigSW_SW2HW_Config(void)
{
	//1.stop mcpwm cntr
	MCPWM0_TCLK &= ~(0x03<<6);
	//2.change trig source
	ADCTest_Config_OL();
	//3.set mcpwm tmrx,CNT value,updata
	MCPWM0_CNT0 = (uint16_t)(-EPWM_HALFPERIOD);
	MCPWM0_CNT1 = (uint16_t)(-EPWM_HALFPERIOD);
	MCPWM0_UPDATE |= (0x01<<11);
	MCPWM0_UPDATE |= (0x01<<27);
	//4.adc rst
	RCC_Reset_ADC();
	//5.enable mcpwm cntr
	MCPWM0_TCLK |= (0x01<<6);

}

/*****************************************************************************
 ** \brief	ADC_TrigSW_HW2SW_Config
 **					硬件触发切软件触发
 ** \param [in] 
					none
 ** \return  none
 ** \note	 
*****************************************************************************/
void ADC_TrigSW_HW2SW_Config(void)
{
	//1.change trig state
	ADCTest_Config_OL();

	//2.adc rst
	RCC_Reset_ADC();

}


















//-------------------------------------------------------------//
// final

/*****************************************************************************
 ** \brief	ADC_SetCLK_INIT
 **			ADC_EN??ADC_RDY???????
 ** \param [in] 
			LTH: 0-4095
 ** \return  none
 ** \note	 HTH > LTH
*****************************************************************************/

void ADC_Test_Init(void)
{
	#if (ADC_Test_Mode == ADC_Normal_Sample_Test)
	{
		ADC_MCPWMTrig_Test_Init();
	}
	#elif (ADC_Test_Mode == ADC_INTR_TEST)
	{
		ADC_INTR_Test_Init();
	}
	#elif (ADC_Test_Mode == ADC_APP_TEST)
	{
		
		#if (Single_Shunt == Config_Shunt_Mode)
		{
			UniShunt_ADC_Init();
			
			ADC_SetCLK_INIT(3);			
			ADC_SetCLK_STPT(5);
			ADC_SetCLK_COVT(2);
			ADC_SetCLK_SAPT(13);
			ADC_SetCLK_TrigDelay(15);		
		}

		#elif (Double_Shunt == Config_Shunt_Mode)
		{
			BiShunt_ADC_Init();
			
			ADC_SetCLK_INIT(3);			
			ADC_SetCLK_STPT(5);
			ADC_SetCLK_COVT(2);
			ADC_SetCLK_SAPT(21);
			ADC_SetCLK_TrigDelay(21);			
		}
		#else
		{
			TriShunt_ADC_Init();
			
//			ADC_SetCLK_INIT(3);			
//			ADC_SetCLK_STPT(5);
//			ADC_SetCLK_COVT(2);
//			ADC_SetCLK_SAPT(21);
//			ADC_SetCLK_TrigDelay(21);					
			
			ADC_SetCLK_INIT(3);			
			ADC_SetCLK_STPT(5);
			ADC_SetCLK_COVT(2);
			ADC_SetCLK_SAPT(27);
			ADC_SetCLK_TrigDelay(30);						
		}
		

		#endif
	}	
	#endif
}


