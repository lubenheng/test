/****************************************************************************/
/** \file hwmath.c
**
**	History:
**	
*****************************************************************************/
/****************************************************************************/
/*	include files
*****************************************************************************/

//#include "stdint.h"


#include "platform_common.h"
#include "ADC_Reg.h"
#include "device_resource.h"
#include "ADC_Type.h"
#include "ADC.h"
int16_t ADCDATA;
/****************************************************************************/
/*	Local pre-processor symbols/macros('#define')
*****************************************************************************/

/****************************************************************************/
/*	Global variable definitions(declared in header file with 'extern')
*****************************************************************************/

/****************************************************************************/
/*	Local type definitions('typedef')
*****************************************************************************/

/****************************************************************************/
/*	Local variable  definitions('static')
*****************************************************************************/


/****************************************************************************/
/*	Local function prototypes('static')
*****************************************************************************/

/****************************************************************************/
/*	Function implementation - global ('extern') and local('static')
*****************************************************************************/


/*****************************************************************************
 ** \brief	ADC_ENABLE
 **			ADC使能
 ** \param [in] 
			ENABLE DISABLE
 ** \return  none
 ** \note	
*****************************************************************************/
void ADC_ENABLE(u8 state)
{
	ADC->CR &= ~(0x01 << 0);
	ADC->CR |= (0x01 << 0);
}

/*****************************************************************************
 ** \brief	ADC_SetSampleGrpMode	
 **			ADC采样分组配置
 ** \param [in] 
state:
			SampleGrpMode_1
			SampleGrpMode_2
			SampleGrpMode_3
			SampleGrpMode_4
 ** \return  none
 ** \note	
*****************************************************************************/
void ADC_SetSampleGrpMode	(uint32_t state)
{
	ADC->TGSR &= ~(0xF << ADC_TGSR_SampleGrpMode_Pos);
	ADC->TGSR |= (state << ADC_TGSR_SampleGrpMode_Pos);
	
}


/*****************************************************************************
 ** \brief	ADC_SetSampleGrpMode	
 **			ADC采样分组配置
 ** \param [in] 
state:
			SampleGrpMode_1
			SampleGrpMode_2
			SampleGrpMode_3
			SampleGrpMode_4
 ** \return  none
 ** \note	
*****************************************************************************/
void ADC_SetSampleGrp_Mode	(uint32_t GroupCh, u8 state)
{
	
	
}

/*****************************************************************************
 ** \brief	ADC_SetGrpTrigSource	
 **			ADC采样分组触发源设置
 ** \param [in] 
		SampGrp:
			ADC_Sample_Group_0
			ADC_Sample_Group_1
			ADC_Sample_Group_2
			ADC_Sample_Group_3

		TriSrc:
			ADC_GrpTrig_SW_TRIG
			ADC_GrpTrig_TMR16_TRIG
			ADC_GrpTrig_TMR15_TRIG
			ADC_GrpTrig_TMR14_TRIG
			ADC_GrpTrig_TMR13_TRIG
			ADC_GrpTrig_MCPWM_TRIG3
			ADC_GrpTrig_MCPWM_TRIG2
			ADC_GrpTrig_MCPWM_TRIG1
			ADC_GrpTrig_MCPWM_TRIG0
 ** \return  none
 ** \note	
*****************************************************************************/
void ADC_SetGrpTrigSource	(uint32_t SampGrp,uint32_t TriSrc)
{
	ADC->TGSR &= ~(0xF << (SampGrp * 4));
	ADC->TGSR |= ((TriSrc & 0xF) << (SampGrp * 4));
}

/*****************************************************************************
 ** \brief	ADC_SetGrpChCount	
 **			ADC采样分组通道数量设置
 ** \param [in] 
		SampGrp:
			ADC_Sample_Group_0
			ADC_Sample_Group_1
			ADC_Sample_Group_2
			ADC_Sample_Group_3

		ChannelNum:
			0 - 16
 ** \return  none
 ** \note	
*****************************************************************************/
void ADC_SetGrpChCount	(uint32_t SampGrp,uint32_t ChannelNum)
{
	ADC->TGNR &= ~(0x1F << (SampGrp * 8));
	ADC->TGNR |= ((ChannelNum & 0x1F) << (SampGrp * 8));
}


/*****************************************************************************
 ** \brief	ADC_SWTrig	
 **			ADC采样分组通道数量设置
 ** \param [in] 		none
 ** \return  none
 ** \note	
*****************************************************************************/
void ADC_SWTrig	(void)
{
	ADC->OTR = 0x5AA5; //写入数据为0x5AA5时，产生一次软件触发 
}


/*****************************************************************************
 ** \brief	ADC_SEQx_CFG
 **			设置ADC序列通道
 ** \param [in] 
			Sqex: ADC_SEQ0 - ADC_SEQ15
			Chx : ADC_CH0 -  ADC_CH31
 ** \return  none
 ** \note	
*****************************************************************************/
void ADC_SEQx_CFG(uint32_t Sqex,uint32_t Chx)
{
	u8 Grop,order;
	Grop = (Sqex >> 2) & 0x3;
	order = Sqex & 0x3;
	
	*(int32_t *)(&(ADC->SEQR0) + Grop) &= ~(0x1F << order *8);
	*(int32_t *)(&(ADC->SEQR0) + Grop) |= (Chx << order *8);	
}

/*****************************************************************************
 ** \brief	ADC_GetResult
 **			读取ADC序列x转换结果
 ** \param [in] 
			Sqex: ADC_SEQ0 - ADC_SEQ15
			
 ** \return  12 bits Value
 ** \note	
*****************************************************************************/
uint32_t ADC_GetResult(u8 Sqex)
{
	ADCDATA=ADC->DATA[Sqex];
	return (ADC->DATA[Sqex]);
	
}


/*****************************************************************************
 ** \brief	ADC_ChEnableThreshDet
 **			使能通道结果检测
 ** \param [in] 
			Sqex: ADC_SEQ0 - ADC_SEQ15
 ** \return  none
 ** \note	
*****************************************************************************/
void ADC_Set_AWChannel(uint32_t Sqex)
{
	ADC->AWE &= ~(0x01 << Sqex);
	ADC->AWE |= (0x01 << Sqex);
}

/*****************************************************************************
 ** \brief	ADC_Set_AW_HTH
 **			设置模拟看门狗上阈值
 ** \param [in] 
			HTH: 0-4095
 ** \return  none
 ** \note	 HTH > LTH
*****************************************************************************/
void ADC_Set_AW_HTH(uint32_t HTH)
{
	ADC->AWTH &= ~(0xFFF << 16);
	ADC->AWTH |= (HTH << 16);
}
/*****************************************************************************
 ** \brief	ADC_Set_AW_LTH
 **			设置模拟看门狗上阈值
 ** \param [in] 
			LTH: 0-4095
 ** \return  none
 ** \note	 HTH > LTH
*****************************************************************************/
void ADC_Set_AW_LTH(uint32_t LTH)
{
	ADC->AWTH &= ~(0xFFF );
	ADC->AWTH |= (LTH );
}



/*****************************************************************************
 ** \brief	ADC_SetCLK_STPT
 **			设置start高电平时间
 ** \param [in] 
			STPT_cnt: 0-15
 ** \return  none
 ** \note	 
*****************************************************************************/
void ADC_SetCLK_STPT(uint32_t STPT_cnt)
{
	ADC->TCFG.STPT = STPT_cnt;

}
/*****************************************************************************
 ** \brief	ADC_SetCLK_COVT
 **			CONV上升沿到ADC_MUX切换时间配置
 ** \param [in] 
			COVT_cnt: 0-15
 ** \return  none
 ** \note	 
*****************************************************************************/
void ADC_SetCLK_COVT(uint32_t COVT_cnt)
{
	ADC->TCFG.COVT = COVT_cnt;
}

/*****************************************************************************
 ** \brief	ADC_SetCLK_SAPT
 **			转换完成后读取等待时间
 ** \param [in] 
			SAPT_cnt: 13-255
 ** \return  none
 ** \note	 
*****************************************************************************/
void ADC_SetCLK_SAPT(uint32_t SAPT_cnt)
{
	if(SAPT_cnt < 0xC)
		SAPT_cnt = 0xC;
	ADC->TCFG.SAPT = SAPT_cnt;
}

/*****************************************************************************
 ** \brief	ADC_SetCLK_INIT
 **			ADC_EN到ADC_RDY时间配置
 ** \param [in] 
			INIT_cnt: 0-255
 ** \return  none
 ** \note	 
*****************************************************************************/
void ADC_SetCLK_INIT(uint32_t INIT_cnt)
{
	ADC->TCFG.INIT = INIT_cnt;
}
/*****************************************************************************
 ** \brief	ADC_SetCLK_TrigDelay
 **			Delay time before 1st sample in every group。
 ** \param [in] 
			Delay_cnt: 0-255
 ** \return  none
 ** \note	 
*****************************************************************************/
void ADC_SetCLK_TrigDelay(uint32_t Delay_cnt)
{
	ADC->TCFG.TDELY = Delay_cnt;
}

/*****************************************************************************
 ** \brief	ADC_Get_AllState
 **			
 ** \param [in] none
			
 ** \return  none
 ** \note	 
*****************************************************************************/
uint32_t ADC_Get_AllState(void)
{
	return (ADC->IR.Word);
}

/*****************************************************************************
 ** \brief	ADC_Enable_IrqSrc
 **			
 ** \param [in] 
IrqSrc:
		ADC_IRQSRC_S0CP
		ADC_IRQSRC_S1CP
		ADC_IRQSRC_S2CP
		ADC_IRQSRC_S3CP
		ADC_IRQSRC_TGN_ERR
		ADC_IRQSRC_TGT_ERR
		ADC_IRQSRC_AWD

 ** \return  none
 ** \note	 
*****************************************************************************/
void ADC_Enable_IrqSrc(uint32_t IrqSrc)
{
	ADC->IE.Word &= ~(0x01 << IrqSrc);
	ADC->IE.Word |= (0x01 << IrqSrc);
}
/*****************************************************************************
 ** \brief	ADC_Disable_IrqSrc
 **			
 ** \param [in] 
IrqSrc:
		ADC_IRQSRC_S0CP
		ADC_IRQSRC_S1CP
		ADC_IRQSRC_S2CP
		ADC_IRQSRC_S3CP
		ADC_IRQSRC_TGN_ERR
		ADC_IRQSRC_TGT_ERR
		ADC_IRQSRC_AWD

 ** \return  none
 ** \note	 
*****************************************************************************/
void ADC_Disable_IrqSrc(uint32_t IrqSrc)
{
	ADC->IE.Word &= ~(0x01 << IrqSrc);
}
/*****************************************************************************
 ** \brief	ADC_Get_IrqSrc
 **			
 ** \param [in] 
IrqSrc:
		ADC_IRQSRC_S0CP
		ADC_IRQSRC_S1CP
		ADC_IRQSRC_S2CP
		ADC_IRQSRC_S3CP
		ADC_IRQSRC_TGN_ERR
		ADC_IRQSRC_TGT_ERR
		ADC_IRQSRC_AWD

 ** \return  none
 ** \note	 
*****************************************************************************/
uint32_t ADC_Get_IrqSrc(void)
{
	return (ADC->IR.Word);
}








