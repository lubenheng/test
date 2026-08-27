

#include "bldc_motor_control.h"


//#include "epwm.h"
//#include "timer.h"

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void Pwm_SetPeriodAndTGD(void)
{
//	EPWM->LOCK = EPWM_LOCK_P1B_WRITE_KEY;
//	EPWM->PERIOD[4] = 0xffff & strMCtr.LoadFrePeriod;
//	EPWM->PERIOD[2] = 0xffff & strMCtr.LoadFrePeriod;
//	EPWM->PERIOD[0] = 0xffff & strMCtr.LoadFrePeriod;

//	EPWM->PERIOD[5] = 0xffff & strMCtr.LoadFrePeriod;
//	EPWM->PERIOD[3] = 0xffff & strMCtr.LoadFrePeriod;
//	EPWM->PERIOD[1] = 0xffff & strMCtr.LoadFrePeriod;

//	//TRG ADC1	可以直接在adc1里面考虑pwm触发设置,1<<19为向上计数

//	EPWM->CMPTGD[0] = (0<<19)|(strMCtr.LoadFrePeriod - 4);
//	EPWM->CMPTGD[1] = (1<<19)|(4); 
//	EPWM->LOCK = 0x0;
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void Pwm_SetDuty(uint16_t uiDuty)
{
//	EPWM->LOCK = EPWM_LOCK_P1B_WRITE_KEY;
//    EPWM->CMPADT[0]=(0xffff&(uiDuty)); 
//    EPWM->CMPADT[2]=(0xffff&(uiDuty)); 
//    EPWM->CMPADT[4]=(0xffff&(uiDuty)); 

//    EPWM->CMPADT[1]=(0xffff&(uiDuty)); 
//    EPWM->CMPADT[3]=(0xffff&(uiDuty)); 
//    EPWM->CMPADT[5]=(0xffff&(uiDuty)); 

//    EPWM->CON3 = 0x3F00;
//    EPWM->LOCK = 0x0;
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void Timer_Record_Stop(void)
{
    //TMR_Stop(TMR0);
//    TMR0->CON &= ~(0x80);
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void Timer_Record_INT_Disable(void)
{
//    TMR0->CON &= ~(TMR_CON_TMRIE_Msk);
//    TMR0->ICLR = TMR_ICLR_ICLR_Msk;

//    TMR_DisableOverflowInt(TMR0);
//    TMR_ClearOverflowIntFlag(TMR0);
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void Timer_Record_Start(void)
{    
    //TMR_Start(TMR0);
    //TMR0->CON |= 0x80;
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

uint32_t Timer_Record_Read(void)
{
    //return (0xffffffff - (TMR0->VAL));/*read timer count*/
	return 0;
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void Timer_Record_Reset(void)
{
    //TMR_SetPeriod(TMR0,0xffffffff);
    //TMR0->LOAD = 0xffffffff;/*reset timer count*/
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void Timer_Record_Write(uint32_t timerWrite)
{
//    TMR0->LOAD = (0xffffffff -timerWrite);
//    TMR0->CON |= 0x80;
}

/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void Timer_MC_Stop(void)
{
//    TMR1->CON &= 0x7F;/* stop timer run*/
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void Timer_MC_Start(void)
{
    //TMR1->CON |= 0x80;
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void Timer_MC_INT_Clear(void)
{
    //TMR1->ICLR = TMR_ICLR_ICLR_Msk;
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void Timer_MC_INT_Disable(void)
{
//    TMR1->CON &= ~(TMR_CON_TMRIE_Msk);
//    TMR1->ICLR = TMR_ICLR_ICLR_Msk;
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void Timer_MC_INT_Enable(void)
{
//    TMR1->ICLR = TMR_ICLR_ICLR_Msk;
//    TMR1->CON |= TMR_CON_TMRIE_Msk;
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void Timer_MC_Write(uint32_t timerWrite)
{
    //TMR1->LOAD = timerWrite;
}
/***********************************************************************************************************************
* Function Name:
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/

void Timer_MC_Write_Start(uint32_t timerWrite)
{
//    TMR1->LOAD = timerWrite;
//    TMR1->CON |= 0x80;
}



