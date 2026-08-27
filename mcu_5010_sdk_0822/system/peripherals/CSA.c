/****************************************************************************/
/** \file CSA.c
**
**	History:
**	
*****************************************************************************/
/****************************************************************************/
/*	include files
*****************************************************************************/

//#include "stdint.h"


#include "platform_common.h"
#include "CSA_Reg.h"
#include "device_resource.h"
#include "CSA_Type.h"
#include "CSA.h"
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
 ** \brief	CSA_ENABLE
 **			CSA使能
 ** \param [in] 
			ENABLE DISABLE
 ** \return  none
 ** \note	
*****************************************************************************/
void CSA_ENABLE(u8 state)
{
	CSA->MISC_CSACR0.RW_ANA_CSA_EN = state;

}

/*****************************************************************************
 ** \brief	CSA_InputCH_SWMode	
 **			CSA输入通道切换模式
 ** \param [in] 
state:
			CSA_FIXED_CH_MODE
			CSA_AUTO_DIFF_MODE
			CSA_AUTO_SINGLE_MODE
 ** \return  none
 ** \note	
*****************************************************************************/
void CSA_InputCH_SWMode	(uint32_t state)
{
	CSA->MISC_CSACR0.RW_CSA_MODE = state;
}

/*****************************************************************************
 ** \brief	CSA_InputCH_SWMode	
 **			CSA输入通道切换模式
 ** \param [in] 
CHSEL:
			CSA_FIXED_INPUT_CH0
			CSA_FIXED_INPUT_CH1
			CSA_FIXED_INPUT_CH2
 ** \return  none
 ** \note	
*****************************************************************************/
void CSA_SET_InputCH(uint32_t CHSEL)
{
	CSA->MISC_CSACR0.RW_CSA_CSP = CHSEL;
	CSA->MISC_CSACR0.RW_CSA_CSN = CHSEL;
}

/*****************************************************************************
 ** \brief	CSA_SET_PGAGAIN	
 **			CSA增益选择
 ** \param [in] 
coeff:
CSA_PGA_GAIN_X1
CSA_PGA_GAIN_X2
CSA_PGA_GAIN_X4
CSA_PGA_GAIN_X8
CSA_PGA_GAIN_X12
CSA_PGA_GAIN_X16
CSA_PGA_GAIN_X32
CSA_PGA_GAIN_X64

 ** \return  none
 ** \note	
*****************************************************************************/
void CSA_SET_PGAGAIN(uint32_t coeff)
{
	CSA->MISC_CSACR0.RW_CSA_GAIN = coeff;
}

/*****************************************************************************
 ** \brief	CSA_SET_REF	
 **			CSA偏置电压设置
 ** \param [in] 
 ref:
			RW_CSA_REF: bit[16:12], 取值 0x00~0x1F
 ** \return  none
 ** \note	
 *****************************************************************************/
void CSA_SET_REF(uint32_t ref)
{
	CSA->MISC_CSACR0.RW_CSA_REF = ref;
}



