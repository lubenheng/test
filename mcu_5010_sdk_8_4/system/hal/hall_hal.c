#include "platform_common.h"

#include "hall_reg.h"
#include "device_resource.h"
#include "hall_hal.h"
#include "hall.h"




HALL_INT_CALLBACK g_hall_int_callback = NULL;

/****************************************************************************
 ** \brief	HALL_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 18
****************************************************************************/
uint16_t g_hall_cnt = 0;
uint16_t g_hall_change_cnt = 0;
uint16_t g_hall_overtime_cnt = 0;
uint16_t g_hall_software_cnt = 0;
void HALL_IRQHandler(void)
{
  volatile uint32_t interrupt_flag =  hall_drv_status_flag_get();
  volatile uint32_t reg = HALL->INFO.HALLFLTDATA;
  
  HALL->LOCK.HALLKEY = HALL_LOCKKEY;
  HALL->IR.w = 0xFFFFFFFFUL;
  HALL->LOCK.HALLKEY = HALL_UNLOCKKEY;

  g_hall_cnt++;


	if (interrupt_flag == 0) // HALL_CHG_IF 霍尔边沿变换中断
	{
		// 目前HALL0/1/2 对应 HALLW/V/U 因此需要将HALL0/2进行交换
		// 以满足 Sector = 1*Hall_U + 2*Hall_V + 4*Hall_W
		// 交换第0位与第2位
		volatile uint32_t t = ((reg >> 2) ^ (reg >> 0)) & 1;
		reg ^= (t << 2) | (t << 0);
		reg &= 0x7;
	}
	else if (interrupt_flag == 1) // HALL_OV_IF 霍尔信号超时中断
	{
		reg = 8;
		g_hall_overtime_cnt++;
	}
	else if (interrupt_flag == 2) // HALL_SW_IF 软件置位霍尔中断
	{
		reg = 9;
		g_hall_software_cnt++;
	}
	else
	{
		reg = 10;	
	}
	
	
#if 1
	if (NULL != g_hall_int_callback)
	{
		g_hall_int_callback((hall_int_source_e)(reg));
		g_hall_change_cnt++;
	}
#endif	

}

/****************************************************************************/
/*	Function implementation - global ('extern') and local('static')
****************************************************************************/
/******************************************************************************
 ** \brief	 HALL_Init   	
 ** \return  none
 ** \note  
 ******************************************************************************/
void HALL_Init()
{
	// uint32_t i=0xffff;
	// //设置div时钟
	// CGC_PER12PeriphClockCmd(CGC_PER12Periph_HALL,ENABLE); 
	// while(i--);

	//HALL时钟分频1分频
	HALL_CLKDiv(HALL_DIV_8);
	
	//二阶滤波
	HALL_FilterLEN_Set(0x100);
	
	//使能75滤波
	HALL_Filter75Enable();
	
	//使能溢出中断
	HALL_OVINT_Enable();
	
	//使能信号变化中断
	HALL_CHGINT_Enable();

	//使能软件触发中断
	HALL_SoftTrigINT_Enable();
	
	//设置门限值
	HALL_Threshod_Set(0xffffffff);
	
	//使能HALL
	HALL_Enable();
    /*中断设置*/
	// NVIC_ClearPendingIRQ(HALL_IRQn);
	// NVIC_EnableIRQ(HALL_IRQn);
	// NVIC_SetPriority(HALL_IRQn,0);
	
	// /*IO HALL IN0设置*/
	// GPIO_PinAFInConfig(HALL0CFG,IO_INCFG_P00_HALLIN0);	
	// GPIO_Init(PORT0,PIN0,INPUT);
	
	// /*IO HALL IN1设置*/
	// GPIO_PinAFInConfig(HALL1CFG,IO_INCFG_P57_HALLIN1);	
	// GPIO_Init(PORT5,PIN7,INPUT);
	
	// /*IO HALL IN2设置*/
	// GPIO_PinAFInConfig(HALL2CFG,IO_INCFG_P56_HALLIN2);	
	// GPIO_Init(PORT5,PIN6,INPUT);
	
}

