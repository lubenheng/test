/*******************************************************************************
* Copyright (C) 2019 China Micro Semiconductor Limited Company. All Rights Reserved.
*
* This software is owned and published by:
* CMS LLC, No 2609-10, Taurus Plaza, TaoyuanRoad, NanshanDistrict, Shenzhen, China.
*
* BY DOWNLOADING, INSTALLING OR USING THIS SOFTWARE, YOU AGREE TO BE BOUND
* BY ALL THE TERMS AND CONDITIONS OF THIS AGREEMENT.
*
* This software contains source code for use with CMS
* components. This software is licensed by CMS to be adapted only
* for use in systems utilizing CMS components. CMS shall not be
* responsible for misuse or illegal use of this software for devices not
* supported herein. CMS is providing this software "AS IS" and will
* not be responsible for issues arising from incorrect user implementation
* of the software.
*
* This software may be replicated in part or whole for the licensed use,
* with the restriction that this Disclaimer and Copyright notice must be  
* included with each copy of this software, whether used in part or whole,
* at all times.
*/

 
/****************************************************************************/
/** \file motor_hall.c
**
**	History:
**	
*****************************************************************************/
/****************************************************************************/
/*	include files
*****************************************************************************/
#include "bldc_motor_hall.h"

#include "bldc_motor_config.h"
#include "bldc_motor_state.h"
#include "bldc_motor_control.h"



#include "bldc_public_function.h"
#include "bldc_user_config.h"

#include "bldc_main.h"






uint8_t hall_index[6]={1,5,4,6,2,3};
uint8_t sectorTableOfHall[7]={0,5,3,4,1,0,2};


/****************************************************************************/
/*	Local type definitions('typedef')
*****************************************************************************/

Motor_Sensor_t strSensor;

/****************************************************************************/
/*	Local variable  definitions('static')
*****************************************************************************/


/****************************************************************************/
/*	Local function prototypes('static')
*****************************************************************************/

/****************************************************************************/
/*	Function implementation - global ('extern') and local('static')
****************************************************************************/
/***********************************************************************************************************************
* Function Name:
* Description  : 有感电机需要配置hall 读取
* Arguments  : 
* Return Value : 
***********************************************************************************************************************/

void UC_SensorMode_IOInit(void)
{

//    /*
//    (1)设置 IO功能
//    */
//    // 13 14 15
//    GPIO_CONFIG_IO_MODE(GPIO1,GPIO_PIN_3,GPIO_MODE_INPUT);        /*hall_U*/
//    SYS_SET_IOCFG(IOP13CFG,SYS_IOCFG_P13_GPIO);//  
//    
//    GPIO_CONFIG_IO_MODE(GPIO1,GPIO_PIN_4,GPIO_MODE_INPUT);        /*hall_V*/
//    SYS_SET_IOCFG(IOP14CFG,SYS_IOCFG_P14_GPIO);//  
//    
//    GPIO_CONFIG_IO_MODE(GPIO1,GPIO_PIN_5,GPIO_MODE_INPUT);        /*hall_W*/
//    SYS_SET_IOCFG(IOP15CFG,SYS_IOCFG_P15_GPIO);//  

//    GPIO_EnableFilter(GPIO1,GPIO_PIN_3_MSK,GPIO_FILCLK_DIV_4);
//    GPIO_EnableFilter(GPIO1,GPIO_PIN_4_MSK,GPIO_FILCLK_DIV_4);
//    GPIO_EnableFilter(GPIO1,GPIO_PIN_5_MSK,GPIO_FILCLK_DIV_4);
//            
//    /*
//    (2)设置中断方式  //开启中断
//    */
//    GPIO_EnableInt(GPIO1,GPIO_PIN_3_MSK,GPIO_INT_EDGE_BOTH);
//    GPIO_EnableInt(GPIO1,GPIO_PIN_4_MSK,GPIO_INT_EDGE_BOTH);
//    GPIO_EnableInt(GPIO1,GPIO_PIN_5_MSK,GPIO_INT_EDGE_BOTH);

//    /*
//    (3)设置中断优先级
//    */


}

/***********************************************************************************************************************
* Function Name: with hall motor
* Description  : 
* Arguments    : 
* Return Value : 
***********************************************************************************************************************/
void MC_SensorMode_Init(void)
{
    strSensor.ModeEn = SENSOR_MODE;

    strSensor.Hall_A = HALL_A;
    strSensor.Hall_B = HALL_B; 
    strSensor.Hall_C = HALL_C;
    strSensor.hallNow = 1;
    strSensor.hallLast = 1;
    strSensor.HallSector = 1;  
}


/***********************************************************************************************************************
* Function Name:
* Description  : 有感电机启动函数
* Arguments	 : 
* Return Value : 
***********************************************************************************************************************/

void MC_SensorMode_Start(void)
{
//    uint8_t hallData = 0;
//    uint8_t sectorNumOfHall;
//    u16 ucDriver;

//    //Timer_Record_Stop();
//    Timer_Record_Write(0);

//    if(HALL_A_IO) 
//    {
//        hallData |=  strSensor.Hall_A;
//    }

//    if(HALL_B_IO) 
//    {
//        hallData |=  strSensor.Hall_B;
//    }

//    if(HALL_C_IO) 
//    {
//        hallData |=  strSensor.Hall_C;
//    }

//    strSensor.hallNow = hallData;


//    sectorNumOfHall = sectorTableOfHall[hallData];


//    if(strMCtr.gMotorDir == 0)
//    {
//        sectorNumOfHall += 0;
//    }
//    else
//    {
//        sectorNumOfHall += 0;
//    }

//    if(sectorNumOfHall>=6)
//    {
//        sectorNumOfHall = sectorNumOfHall-6;
//    }  


//    strMCtr.RotorSector = sectorNumOfHall;
//    ucDriver=gTwoDriverTab[strMCtr.gMotorDir][strMCtr.RotorSector];
//    MC_PwmMask_Set(ucDriver);

//    strSensor.hallLast = strSensor.hallNow;

//    strMCtr.MotorState = MOTOR_RUN;

}

/***********************************************************************************************************************
* Function Name:
* Description  : 有感电机换相函数
* Arguments	 : 
* Return Value : 
***********************************************************************************************************************/

void MC_SensorMode_HallChange(void)
{

//	uint8_t hallData = 0;
//	uint8_t sectorNumOfHall;
//	u16 ucDriver;


//    Timer_MC_Stop();	

//    
//	
//	if(HALL_A_IO) 
//	{
//		hallData |=  strSensor.Hall_A;
//	}
//	if(HALL_B_IO) 
//	{
//		hallData |=  strSensor.Hall_B;
//	}
//	if(HALL_C_IO) 
//	{
//		hallData |=  strSensor.Hall_C;
//	}

//	strSensor.hallNow = hallData;

//	if((strSensor.hallNow ==0)||(strSensor.hallNow == 7))
//	{
//        //error
//            return;
//	}
//	else
//	{
//        strSensor.HallSector = sectorTableOfHall[hallData];
//	}


//	if(
//	(strSensor.hallNow != strSensor.hallLast)
//	)
//	{
//            if(strMCtr.MotorState == MOTOR_RUN)  // 换相
//            {

//                strMCtr.SectorCtoC_T0Cnt = Timer_Record_Read();
//                Timer_Record_Reset(); 
//                
//                sectorNumOfHall = sectorTableOfHall[hallData];

//                
//                if(strMCtr.gMotorDir == 0)
//                {
//                    sectorNumOfHall += 0;
//                }
//                else
//                {
//                    sectorNumOfHall += 0;
//                }

//                if(sectorNumOfHall>=6)
//                {
//                    sectorNumOfHall = sectorNumOfHall-6;
//                }  

//                strMCtr.SectorCtoCLast_T0Cnt = strMCtr.SectorCtoC_T0Cnt;
//            }


//            if(strMCtr.MotorState == MOTOR_RUN)  
//            {
//                strMCtr.RotorSector = sectorNumOfHall;
//                ucDriver=gTwoDriverTab[strMCtr.gMotorDir][strMCtr.RotorSector];
//                MC_PwmMask_Set(ucDriver);
//            }
//	}

//	strSensor.hallLast = strSensor.hallNow;
			
}


