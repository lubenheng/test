#ifndef ADC_TEST_H
#define ADC_TEST_H

#define ADC_Normal_Sample_Test	(0)
#define ADC_INTR_TEST						(1)
#define ADC_APP_TEST						(2)
#define ADC_Test_Mode						(ADC_Normal_Sample_Test)


//??¨¨?2¨¦?¨´¡Á¨¦
#define ADC_MCPWM_TRG_GROUP			(SampleGrpMode_4)				// ¨¢?¡Á¨¦¡ä£¤¡¤¡é

#define	ADC_CONFIG_CNT_TEST			(0)
#define	ADC_STATEIO_TEST				(0)
#define	ADC_STATE_SIG_TEST			(0)
#define	ADC_SOFTTRIG_TEST				(0)
#define ADC_SOFTTRIG_GROUP			(ADC_Sample_Group_0)
#define	ADC_SOFTTRIG_ERR_TEST		(0)

#define	ADC_MCPWM_TRG_TEST			(0)


#define	ADC_SEQNUM_CHANGE_TEST				(1)
#define	ADC_GROUPMODE_CHANGE_TEST			(1)
#define	ADC_TRIGCNT_CHANGE_TEST				(1)
#define	ADC_SEQCH_CHANGE_TEST					(0)
#define	ADC_TRIGSRC_CHANGE_TEST				(0)

#define ADC_Err_Test		(1)





void ADC_gpio_init(void);
void ADC_MCPWMTrig_Test_Init(void);
void ADC_SWTrig_Test_Online(void);

void ADCTest_Config_OL(void);

void ADC_TrigSW_HW2SW_Config(void);
void ADC_TrigSW_SW2HW_Config(void);
void ADC_HWTrig_PWM_Config(int InitCnt);



void ADC_Test_Init(void);
#endif /*ADC_TEST_H*/

