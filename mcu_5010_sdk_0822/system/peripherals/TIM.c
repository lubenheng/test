/****************************************************************************/
/** \file Utimer.c
**
**	History:
**	
*****************************************************************************/
/****************************************************************************/
/*	include files
*****************************************************************************/

#include "platform_common.h"
#include "TIM_Reg.h"
#include "device_resource.h"
#include "TIM_Type.h"
#include "TIM.h"



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
 ** \brief	TIMER_Set_CLKDIV	
 **			
 ** \param [in] 
Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
DivCoff:
			TIMER_DIV_1
			TIMER_DIV_2
			TIMER_DIV_4
			TIMER_DIV_8
			TIMER_DIV_16
			TIMER_DIV_32
			TIMER_DIV_64
			TIMER_DIV_128
 ** \return  none
 ** \note	
*****************************************************************************/
void TIMER_Set_CLKDIV(TIMx_TypeDef* Timx,uint32_t DivCoff)
{
	Timx->TIM_CFG.CKDIV = DivCoff;
}

/*****************************************************************************
 ** \brief	Timer_Ch0_En_FallCap	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
 ** \return  none
 ** \note	
*****************************************************************************/
void Timer_Ch0_En_FallCap(TIMx_TypeDef* Timx)
{
	Timx->TIM_CFG.CH0FCE = 1;
}
void Timer_Ch0_En_RiseCap(TIMx_TypeDef* Timx)
{
	Timx->TIM_CFG.CH0RCE = 1;
}

void Timer_Ch0_Disable_FallCap(TIMx_TypeDef* Timx)
{
	Timx->TIM_CFG.CH0FCE = 0;
}
void Timer_Ch0_Disable_RiseCap(TIMx_TypeDef* Timx)
{
	Timx->TIM_CFG.CH0RCE = 0;
}


/*****************************************************************************
 ** \brief	TIMER_Set_Mode	
 **			
 ** \param [in] 
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
Chx:
			TIMERx_CH0
			TIMERx_CH1
Pol:
			TIMERx_IOPOL_POSITIVE
			TIMERx_IOPOL_NEGAGTIVE
 ** \return  none
 ** \note	
*****************************************************************************/

void TIMER_Set_IOPOL(TIMx_TypeDef* Timx,uint8_t Chx,uint32_t Pol)
{
	if(Chx == TIMERx_CH0)
	{
		Timx->TIM_CFG.CH0POL = Pol;
	}
	else
	{
		Timx->TIM_CFG.CH1POL = Pol;
	}
}

/*****************************************************************************
 ** \brief	TIMER_Set_Mode	
 **			
 ** \param [in] 
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
Chx:
			TIMERx_CH0
			TIMERx_CH1
Mode:
			TIMERx_MODE_PWM
			TIMERx_MODE_CAP
 ** \return  none
 ** \note	
*****************************************************************************/

void TIMER_Set_CH_Mode(TIMx_TypeDef* Timx,uint8_t Chx,uint32_t Mode)
{
	if(Chx == TIMERx_CH0)
	{
		Timx->TIM_CFG.CH0M = Mode;
	}
	else
	{
		Timx->TIM_CFG.CH1M = Mode;
	}
}

/*****************************************************************************
 ** \brief	TIMER_Set_InputSrc	
 **			
 ** \param [in] 
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
Chx:
			TIMERx_CH0
			TIMERx_CH1
Input:
			TIMERx_SRC_IO0		
			TIMERx_SRC_IO1		
			TIMERx_SRC_CLU0		
			TIMERx_SRC_CLU1		
			TIMERx_SRC_CLU2		
			TIMERx_SRC_CLU3		
			TIMERx_SRC_CLU4		
			TIMERx_SRC_IO0_XOR_IO1		
 ** \return  none
 ** \note	
*****************************************************************************/

void TIMER_Set_InputSrc(TIMx_TypeDef* Timx,uint8_t Chx,uint32_t Input)
{
	if(Chx == TIMERx_CH0)
	{
		Timx->TIM_CFG.CH0CAPSRC = Input;
	}
	else
	{
		Timx->TIM_CFG.CH1CAPSRC = Input;
	}
}


/*****************************************************************************
 ** \brief	Timer_Ch1_En_FallCap	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
 ** \return  none
 ** \note	
*****************************************************************************/

void Timer_Ch1_En_FallCap(TIMx_TypeDef* Timx)
{
	Timx->TIM_CFG.CH1FCE = 1;
}
void Timer_Ch1_En_RiseCap(TIMx_TypeDef* Timx)
{
	Timx->TIM_CFG.CH1RCE = 1;
}
void Timer_Ch1_Disable_FallCap(TIMx_TypeDef* Timx)
{
	Timx->TIM_CFG.CH1FCE = 1;
}
void Timer_Ch1_Disable_RiseCap(TIMx_TypeDef* Timx)
{
	Timx->TIM_CFG.CH1RCE = 1;
}


/*****************************************************************************
 ** \brief	Timer_Ch1_En_FallCap	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
Cks:
			TIMERx_INNERCLK
			TIMERx_OUTSIDECLK

 ** \return  none
 ** \note	
*****************************************************************************/

void Timer_Set_ClkSrc(TIMx_TypeDef* Timx,uint8_t Cks)
{
	Timx->TIM_CFG.CKS  = Cks;
}

/*****************************************************************************
 ** \brief	Timer_Ch1_En_FallCap	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
Rld:
			TIMERx_AUTO_RELOAD
			TIMERx_EXTTRIG_RELOAD

 ** \return  none
 ** \note	
*****************************************************************************/

void Timer_Set_RLMode(TIMx_TypeDef* Timx,uint8_t Rld)
{
	Timx->TIM_CFG.RLD  = Rld;
}

/*****************************************************************************
 ** \brief	Timer_Ch1_En_FallCap	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
Gate:
			TIMERx_STOP_DISABLE
			TIMERx_STOP_ENABLE

 ** \return  none
 ** \note	
*****************************************************************************/

void Timer_Set_GateEN(TIMx_TypeDef* Timx,uint8_t Gate)
{
	Timx->TIM_CFG.GATE  = Gate;
}


/*****************************************************************************
 ** \brief	Timer_Ch1_En_FallCap	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
Gate:
			TIMERx_EXT_START_EN
			TIMERx_EXT_START_DIS

 ** \return  none
 ** \note	
*****************************************************************************/

void Timer_Set_EXT_ON(TIMx_TypeDef* Timx,uint8_t Exon)
{
	Timx->TIM_CFG.ETON  = Exon;
}


/*****************************************************************************
 ** \brief	Timer_Ch1_En_FallCap	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
Dir:
			TIMERx_CNTR_UP
			TIMERx_CNTR_DOWN

 ** \return  none
 ** \note	
*****************************************************************************/

void Timer_Set_CNTR_Dir(TIMx_TypeDef* Timx,uint8_t Dir)
{
	Timx->TIM_CFG.DIR   = Dir;
}

/*****************************************************************************
 ** \brief	Timer_Ch1_En_FallCap	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
Mid:
			TIMERx_SYMMETRIC_MODE
			TIMERx_EDGE_MODE

 ** \return  none
 ** \note	
*****************************************************************************/

void Timer_Set_Center_Mode(TIMx_TypeDef* Timx,uint8_t Mid)
{
	Timx->TIM_CFG.MID    = Mid;
}

/*****************************************************************************
 ** \brief	Timer_Ch1_En_FallCap	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
			

 ** \return  none
 ** \note	
*****************************************************************************/

void Timer_Single_Trig(TIMx_TypeDef* Timx)
{
	Timx->TIM_CFG.OTR    = 1;
}

/*****************************************************************************
 ** \brief	Timer_Set_Shadow	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
SD:	
			TIMERx_SHADOW_REG_DIS
			TIMERx_SHADOW_REG_EN
 ** \return  none
 ** \note	
*****************************************************************************/

void Timer_Set_Shadow(TIMx_TypeDef* Timx,uint8_t SD)
{
	Timx->TIM_CFG.SHADOW    = SD;
}
/*****************************************************************************
 ** \brief	Timer_Set_Shadow	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16

 ** \return  none
 ** \note	
*****************************************************************************/
void Timer_Reg_Update(TIMx_TypeDef* Timx)
{
	Timx->TIM_CFG.UPDATE    = 1;
}

/*****************************************************************************
 ** \brief	Timer_Enable_Cap0_Clr	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16

 ** \return  none
 ** \note	
*****************************************************************************/
void Timer_Enable_Cap0_Clr(TIMx_TypeDef* Timx)
{
	Timx->TIM_CFG.CCLR0E    = 1;
}
void Timer_Disable_Cap0_Clr(TIMx_TypeDef* Timx)
{
	Timx->TIM_CFG.CCLR0E    = 0;
}
/*****************************************************************************
 ** \brief	Timer_Enable_Cap1_Clr	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16

 ** \return  none
 ** \note	
*****************************************************************************/

void Timer_Enable_Cap1_Clr(TIMx_TypeDef* Timx)
{
	Timx->TIM_CFG.CCLR1E    = 1;
}
void Timer_Disable_Cap1_Clr(TIMx_TypeDef* Timx)
{
	Timx->TIM_CFG.CCLR1E    = 0;
}


/*****************************************************************************
 ** \brief	Timer_Enable	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16

 ** \return  none
 ** \note	
*****************************************************************************/

void Timer_Enable(TIMx_TypeDef* Timx)
{
	Timx->TIM_CFG.EN     = 1;
}
void Timer_Disable(TIMx_TypeDef* Timx)
{
	Timx->TIM_CFG.EN     = 0;
}

/*****************************************************************************
 ** \brief	Timer_Enable	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
Th:0-(2^32 -1)
 ** \return  none
 ** \note	
*****************************************************************************/
void Timer_Set_ThVal(TIMx_TypeDef* Timx,uint32_t Th)
{
	//Timx->TH = Th - 1;
	Timx->TH = Th;
}
/*****************************************************************************
 ** \brief	Timer_Set_Comp0_Val	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
Com0:0-(2^32 -1)
 ** \return  none
 ** \note	
*****************************************************************************/
void Timer_Set_Comp0_Val(TIMx_TypeDef* Timx,uint32_t Com0)
{
	Timx->CMP0 = Com0;
}
/*****************************************************************************
 ** \brief	Timer_Set_Comp1_Val	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
Com1:0-(2^32 -1)
 ** \return  none
 ** \note	
*****************************************************************************/
void Timer_Set_Comp1_Val(TIMx_TypeDef* Timx,uint32_t Com1)
{
	Timx->CMP1 = Com1;
}

/*****************************************************************************
 ** \brief	Timer_Set_Comp1_Val	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
Evt:
			TIMER13_CH0_CAPCOM_EVT
			TIMER13_CH1_CAPCOM_EVT
			TIMER14_CH0_CAPCOM_EVT
			TIMER14_CH1_CAPCOM_EVT
			TIMER15_CH0_CAPCOM_EVT
			TIMER15_CH1_CAPCOM_EVT
			TIMER16_CH0_CAPCOM_EVT
			TIMER16_CH1_CAPCOM_EVT
			MCPWM_TADC0_COM_EVT
			MCPWM_TADC1_COM_EVT
			MCPWM_TADC2_COM_EVT
			MCPWM_TADC3_COM_EVT

 ** \return  none
 ** \note	
*****************************************************************************/
void Timer_Set_EvtSrc(TIMx_TypeDef* Timx,uint32_t Evt)
{
	Timx->EVTSRC = Evt;
}


/*****************************************************************************
 ** \brief	Timer_Set_Comp1_Val	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
Coeff:
			0-255


 ** \return  none
 ** \note	
*****************************************************************************/
void Timer_Set_FiltCoeff(TIMx_TypeDef* Timx,uint32_t Coeff)
{
	Timx->FILT = Coeff;
}

/*****************************************************************************
 ** \brief	Timer_IRQ_Enable	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
Irq:	
			TIMERx_IRQ_CH0
			TIMERx_IRQ_CH1
			TIMERx_IRQ_ZERO
			TIMERx_IRQ_FAIL

 ** \return  none
 ** \note	
*****************************************************************************/
void Timer_IRQ_Enable(TIMx_TypeDef* Timx,uint32_t Irq)
{
	Timx->IER.word &= ~Irq;
	Timx->IER.word |= Irq;
}

/*****************************************************************************
 ** \brief	Timer_IRQ_Disable	
 **			
 ** \param [in] none
 Timx:
			TIMER13
			TIMER14
			TIMER15
			TIMER16
Irq:	
			TIMERx_IRQ_CH0
			TIMERx_IRQ_CH1
			TIMERx_IRQ_ZERO
			TIMERx_IRQ_FAIL

 ** \return  none
 ** \note	
*****************************************************************************/

void Timer_IRQ_Disable(TIMx_TypeDef* Timx,uint32_t Irq)
{
	Timx->IER.word &= ~(0x01 << Irq);
	Timx->IER.word |= (0x01 << Irq);
}

/**
 *@brief	 Get all timer interrupt flags
 *@param[in] Timx:
				TIMER13
				TIMER14
				TIMER15
				TIMER16
 *@return    all flag,Flag bits corresponding to the return value
 			 bit0:TIMERx_IRQ_FLAG_CH0
			 bit1:TIMERx_IRQ_FLAG_CH1
			 bit2:TIMERx_IRQ_FLAG_ZERO
			 bit3:TIMERx_IRQ_FLAG_FAIL
 */
uint8_t Timer_drv_IRQ_Status_Get(TIMx_TypeDef* Timx)
{
	//获取所有标志位状态
	uint8_t status = (Timx->IFR.word & 0xf);

	//清除所有标志位
	Timx->IFR.word = 0xffffffffUL;
	return status;
}

uint8_t Timer_drv_IRQ_Status_En_Get(TIMx_TypeDef* Timx)
{
	//获取所有标志位状态
	uint8_t status = (Timx->IER.word & 0xf);

	return status;
}

/**
 *@brief	 Disable all TIM interrupts and clear the flag bits
 *@param[in]  Timx:
				TIMER13
				TIMER14
				TIMER15
				TIMER16
 *@return    none
 */
void Timer_drv_IRQ_Disable(TIMx_TypeDef* Timx)
{
	Timx->IER.word &= 0x0;
	Timx->IFR.word &= 0x0;
}

/**
 *@brief	 Clear all settings of the TIM
 *@param[in]  Timx:
				TIMER13
				TIMER14
				TIMER15
				TIMER16
 *@return    none
 */
void Timer_drv_Setting_Clean(TIMx_TypeDef* Timx)
{
	Timx->TIM_CFG.word &= 0x0;
	Timx->TH &= 0x0;
	Timx->CNT &= 0x0;
	Timx->CMP0 &= 0x0;
	Timx->CMP1 &= 0x0;
	Timx->EVTSRC &= 0x0;
}

void Timer_drv_Setting_Clean_all(TIMx_TypeDef* Timx)
{
	Timx->TIM_CFG.word &= 0x0;
	Timx->TH &= 0x0;
	Timx->CNT &= 0x0;
	Timx->CMP0 &= 0x0;
	Timx->CMP1 &= 0x0;
	Timx->EVTSRC &= 0x0;
	Timx->FILT &= 0x0;
	Timx->IER.word &= 0x0;
	Timx->IFR.word &= 0x0;
	Timx->FAE.word &= 0x0;
}

