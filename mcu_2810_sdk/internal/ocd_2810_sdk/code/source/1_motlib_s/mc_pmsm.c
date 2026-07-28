/****************************************************************************/
/**
* @file    mc_pmsm.c
* @author  CMS_Motor_Control_Team
* @version V2.0.0
* @date    01-03-2018
* @brief   PMSM FOC drive functions
**
*****************************************************************************/

/****************************************************************************/
/*	include files
*****************************************************************************/
#include "mcu_driver.h"
#include "motor_driver.h"
#include "user.h"

/****************************************************************************/
/*	Local variable  definitions
*****************************************************************************/




/****************************************************************************
** Function   :  
** Description:  等幅值变换
**
** Author     :  
** Date       :  
*****************************************************************************/
struct_Clark Clarke(struct_Curr_abc Iphase)
{ 
	struct_Clark Clarke_Output;
	


	return(Clarke_Output);
}


/****************************************************************************
** Function   :  
** Description:  
**
** Author     :  
** Date       :  
*****************************************************************************/
struct_Park Park(struct_SinCos SinCosVal, struct_Clark Clarke)
{
	struct_Park Park_Output;
	

	
	return(Park_Output);
}

/****************************************************************************
** Function   :  
** Description:  
**
** Author     :  
** Date       :  
*****************************************************************************/
struct_RevPark RevPark(struct_SinCos SinCosVal, struct_Volt_dq Volt)
{
	struct_RevPark RevPark_Output;
	

	
	return(RevPark_Output);
}

/****************************************************************************
** Function   :  
** Description:  
**
** Author     :  
** Date       :  
*****************************************************************************/
struct_SinCos  FOC_SinCos_Calc(s32 Angel)
{
	u16 hindex;
	struct_SinCos SinCos_Output;


	
	return(SinCos_Output);
	
}

/****************************************************************************
** Function   :  
** Description:  
**
** Author     :  
** Date       :  
*****************************************************************************/
struct_Arctan ArcTan_Cal(s32 hSin,   s32 hCos)
{
	struct_Arctan ArcTan_Output;
	
	
	
	return(ArcTan_Output);
}


/****************************************************************************
** Function   :  
** Description:  
**
** Author     :  
** Date       :  
*****************************************************************************/
s32 FOC_ArcTan_Cal(s32 hSin,  s32 hCos)
{

	u8 pt,index = 0;
	s32 sin,cos;
	s32	Angle = 0;
	

	
	return(Angle);
}


/****************************************************************************
** Function   :  
** Description:  
**
** Author     :  
** Date       :  
*****************************************************************************/
void FOC_CALC_SVPWM(struct_SVPWM *SVM_Compt, struct_RevPark  Volt_alfa_beta)
{
	
}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  :FOC_SinCosCal
* Description    :根据角度值计算正弦和余弦值
* Function Call  :EPWM_IRQHandler中断函数调用
* Input Paragram :角度值
* Return Value   :p_SinCosRegS->Sin p_SinCosRegS->Cos
*-----------------------------------------------------------------------------
******************************************************************************/
void  FOC_SinCosCal( struct_SinCos  *p_SinCosRegS  ,  s32 l_angel )
{

}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : FOC_EPWM_UPDATA
* Description    : 
* Function Call  : EPWM_IRQHandler中断函数调用
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_EPWM_UPDATA(void)
{

}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  :FOC_Calc_DeadComp
* Description    :死区补偿算法			
* Function Call  :EPWM_IRQHandler中断函数调用
* Input Paragram : 三相电流信号ABC，母线电压
* Return Value   : 补偿值VolAlpha VolBeta
*-----------------------------------------------------------------------------
******************************************************************************/
void FOC_DoubleShunt_Calc(struct_SVPWM *SVM_Compt, struct_RevPark  Volt_alfa_beta)
{

}

/******************************** END OF FILE *******************************/
