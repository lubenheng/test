//==========================================================================//
/*****************************************************************************
*-----------------------------------------------------------------------------
* @file    softinit.c
* @author  CMS Motor Control Team
* @version 第二代风机平台
* @date    2023年2月
* @brief   
*---------------------------------------------------------------------------//
**********************************************1******************************/
//==========================================================================//

//---------------------------------------------------------------------------/
//	include files
//---------------------------------------------------------------------------/
#include "mcu_driver.h"
#include "motor_driver.h"
#include "user.h"
#include "softinit.h"
//---------------------------------------------------------------------------/
//	Local pre-processor symbols/macros('#define')
//---------------------------------------------------------------------------/

//---------------------------------------------------------------------------/
//	Local variable  definitions
//---------------------------------------------------------------------------/

//---------------------------------------------------------------------------/
//	Global variable definitions(declared in header file with 'extern')
//---------------------------------------------------------------------------/
ByteFlag Time_Flag;

//---------------------------------------------------------------------------/
//	Local function prototypes('static')
//---------------------------------------------------------------------------/

/*****************************************************************************
 ** \brief	MC_SampPara_Init
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void MC_SampPara_Init(void)
{

	
}

/*****************************************************************************
 ** \brief	MC_FaultPara_Init
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void MC_FaultPara_Init(void)
{		
}

/*****************************************************************************
 ** \brief	MC_CtrltPara_Init
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void MC_CtrltPara_Init(void)
{	

}

/*****************************************************************************
 ** \brief	MC_BasePara_Init
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void MC_BasePara_Init(void)
{


}

/*****************************************************************************
 ** \brief	MC_FOCPara_Init
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void MC_FOCPara_Init(void)
{	
	//----------------------------------------------------------------------------/

	stru_FOC_Start.Elec_Omega = MC_Start_Init_Speed*EPWM_FREQ>>16;


	// 参数	
	stru_Para.Align.Time = (uint16_t)((uint32_t)MC_ALIGN_TIME * EPWM_FREQ /1000);
	stru_Para.Align.Curr = Phy2Pu_Fun_Iphase(MC_ALIGN_D_AXIS_CURRENT);
	stru_Para.Align.Cnt = 0;
	
	stru_Para.Start.OmegaMax = MC_Start_End_Speed * MOTOR_PAIRS *_Q16_VAL / ( 60 * EPWM_FREQ);
	stru_Para.Start.ACCCycle = MC_Start_TIME * EPWM_FREQ / stru_Para.Start.OmegaMax/1000;
	stru_Para.Start.Final_Curr = Phy2Pu_Fun_Iphase(MC_Start_Curr);
	stru_Para.Start.DeltaCur = (stru_Para.Start.Final_Curr - stru_Para.Align.Curr) / stru_Para.Start.OmegaMax;

	stru_Para.Transition.Final_Curr = Phy2Pu_Fun_Iphase(MC_Transition_Current);
	stru_Para.Transition.Transition_to_Run_Cnt = 0;
	
	stru_Para.Run.Iq_Final_Curr = Phy2Pu_Fun_Iphase(MC_Iq_Run_Current);
	stru_Para.Run.Id_Final_Curr = 0;//Phy2Pu_Fun_Iphase(MC_Id_Run_Current);
	stru_Para.Run.Id_req = 0;
	stru_Para.Run.Kslf = MC_Run_Kslf;
	stru_Para.Run.Compensation = 0;
	stru_Para.Run.theta_compensation_tmp = MC_Run_Theta_Compensation;
	
}

/*****************************************************************************
 ** \brief	MC_LPFPara_Init
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void MC_LPFPara_Init(void)
{

}

/*****************************************************************************
 ** \brief	SpeedRampInit
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void MC_SpeedRamp_Init(void)
{

}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : MC_BaseValue_Init
* Description    : 
* Function Call  : 初始化调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void MC_BaseValue_Init(void)
{

}

/*****************************************************************************
 ** \brief	MC_UserPowerSet_Init
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void MC_UserPowerSet_Init(void)
{

}

/*****************************************************************************
 ** \brief	MC_Para_Init
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void MC_Para_Init(void)
{
	// MC_CtrltPara_Init();	
	// MC_SampPara_Init();	
	// MC_FaultPara_Init();
	// MC_BasePara_Init();		
	 MC_FOCPara_Init();	
	// MC_LPFPara_Init();		
	// MC_SpeedRamp_Init();
	// MC_BaseValue_Init();
	// MC_UserPowerSet_Init();
}

/*****************************************************************************
 ** \brief	__EI
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void __EI_Set(void)
{

}

/*****************************************************************************
 ** \brief	DI
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void __DI_Set(void)
{

}

/*****************************************************************************
 ** \brief	
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void DelayTime(uint32_t delay)
{
	for( ; delay > 0; delay -- ) 
	{
		__nop();
	}
}

/*****************************************************************************
 ** \brief	
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void DelayTime_ms(uint32_t delay)
{
	uint32_t i,j;
	
	for( i = 0; i < delay; i ++ )
	{
		for( j = 0; j < 10000; j++ ) //1ms,64M
		{
			;
		}
	}
}

/*****************************************************************************
 ** \brief	void ADC_ClearAllInt_Flag(void)
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void ADC_ClearAllInt_Flag(void)
{

}

/*****************************************************************************
 ** \brief	ADC_TGSAMP_CONFIG
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void ADC_TGSAMP_CONFIG(void)
{

}

/*****************************************************************************
 ** \brief	GET_ADC_IBUS_REF_VAL
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void GET_ADC_IBUS_REF_VAL(void)
{

}

/*****************************************************************************
 ** \brief	GET_ADC_BG_REF_VAL
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
extern void ADC_Init(void);
void GET_ADC_BG_REF_VAL(void)
{


}

/*****************************************************************************
 ** \brief	GET_ADC_REF_VAL
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
#define OFFSET_ERO_MAX  				(0.15*4096 / HW_ADC_REF)
#define ADval_Ip_OffSet  				(HW_AMP_REF_IP * 4096 / HW_ADC_REF)
#define ADval_Ibus_OffSet  			(HW_AMP_REF_IBUS * 4096 / HW_ADC_REF)
void GET_ADC_REF_VAL(void)
{

}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  :BLDC_BEMF_Start_Config
* Description    :
* Function Call  :
* Input Paragram :无
* Return Value   :无
*-----------------------------------------------------------------------------
******************************************************************************/
void  BLDC_BEMF_Start_Config(void)
{

}


/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : BEMF_ParaInit
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void BEMF_ParaInit(void)
{
	
}
/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_BEMF_Init
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_BEMF_Init(void)
{

}

/*****************************************************************************
 ** \brief	GET_ADC0_REF_VAL
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void GET_BANDGAP_VAL(void)
{

}

 void Clear_rx_fifo(void)
 {
	uint32_t tmp;
	// 将RX FIFO中残留数据先清除 （必须在这儿清，因为引入残留的就是上一句话）
    tmp = (UART0->int_clr >> 9) & 1;
    while (tmp == 0)
    {
        tmp = UART0->dat;
        tmp = (UART0->int_clr >> 9) & 1;
    }

    printk("FOC angle trace.\r\n");
 
 
 }
/******************************** END OF FILE *******************************/
