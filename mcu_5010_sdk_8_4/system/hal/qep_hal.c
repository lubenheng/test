#include "platform_common.h"

#include "qep_reg.h"
#include "device_resource.h"
#include "qep_hal.h"
#include "qep.h"


QEP_INT_CALLBACK g_qep_int_callback = NULL;
/****************************************************************************/
/*	Function implementation - global ('extern') and local('static')
****************************************************************************/
/******************************************************************************
 ** \brief	 HALL_Init   	
 ** \return  none
 ** \note  
 ******************************************************************************/
void QEP_Init(void)
{
	TIM_QEPInitTypeDef TIM_QEPInitStruct;

	TIM_QEPStructInit(&TIM_QEPInitStruct);		   /* Timer结构体初始化 */
	TIM_QEPInitStruct.QEP_FE_CNT_EN = ENABLE;	   /* 使能下降沿进行计数，ENABLE:使能 DISABLE:失能 */
	TIM_QEPInitStruct.QEP_Mode = QEP_Mode_T1;      /* 编码器模式选择 */
	TIM_QEPInitStruct.QEP_ClockDiv = QEP_CLK_DIV1; /* 编码器时钟分频 */
	TIM_QEPInitStruct.QEP_Filter = 0;			   /* 输入信号滤波系数 */
	TIM_QEPInitStruct.QEP_TH = 5000;			   /* 编码器计数周期 */
	TIM_QEPInitStruct.QEP_ZEC = ENABLE;			   /* Z 信号清零极性选择：低电平/下降沿清零使能 */
	TIM_QEPInitStruct.QEP_ZLC = DISABLE;		   /* Z 信号清零极性选择：高电平/下降沿清零使能 */
	TIM_QEPInitStruct.QEP_ZPC = ENABLE;			   /* Z 信号边沿清零 QEP 计数器使能 */
	TIM_QEPInitStruct.QEP_ZNC = DISABLE;		   /* Z 信号电平清零 QEP 计数器使能 */
	TIM_QEPInitStruct.QEP_IRQEna = 3;		   	   /* 中断使能 */
	TIM_QEPInit(QEP, &TIM_QEPInitStruct);
	TIM_QEPCmd(QEP, ENABLE); 					   /* QEP0模块使能 */
}


/****************************************************************************
 ** \brief	QEP_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 19
****************************************************************************/
uint16_t qep_interrupt_cnt = 0;
uint16_t qep_downflow_cnt = 0;
uint16_t qep_upflow_cnt = 0;
void QEP_IRQHandler(void)
{
	uint32_t intp_soure = 0;
	volatile int32_t interrupt_flag = ((QEP->IF.w & QEP->IE.w)&0x3);
	
	/* 清除所有的中断标志位 */
	QEP->IF.w = 0x3;

	if(interrupt_flag == 1)//BIT0:OF_IF
	{
		intp_soure = 0;
		qep_downflow_cnt++;
	}
	else if(interrupt_flag == 2)//BIT1:UF_IF
	{
		intp_soure = 1;
		qep_upflow_cnt++;
	}
	else
	{
		intp_soure = 2;
	}
	qep_interrupt_cnt++;

#if(1)
	if (NULL != g_qep_int_callback)
	{
		g_qep_int_callback((qep_int_source_e)(intp_soure));
	}
#endif
}
