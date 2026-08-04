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

#ifndef __MCU_BSP_H__
#define __MCU_BSP_H__
#include "bldc_main.h"
#include "bldc_user_config.h"
/****************************************************************************/
/*	Local type definitions('typedef')
*****************************************************************************/


 

/****************************************************************************/
/*	Local pre-processor symbols/macros('#define')
*****************************************************************************/


/****************************************************************************
*(3)
*数字输入端口使用
*
*****************************************************************************/

/*方向*/
#define Input_DirSw               		         	            (GPIO2->DI_f.P4) 
/*按键*/
#define Input_WrenchKey          		           	            (GPIO2->DI_f.P4) 
/*启停开关*/
#define Input_TriggerSw           		         	            (GPIO2->DI_f.P6)

/****************************************************************************/


/****************************************************************************
*(4)
*数字输出端口使用
*
*****************************************************************************/
                         
#define PowerLock               		         	            //(GPIO2->DO_f.P2) 
#define PowerLock_ON             		   			            (1)//(PowerLock = 1)
#define PowerLock_OFF           		    		            (0)//(PowerLock = 0)


#define LED_ON                      	      		            (0)
#define LED_OFF                     				            (1)

#define LED1                       	 	          	            (GPIO1->DO_f.P0)
#define LED1_ON                     	  			            (LED1 = LED_ON)
#define LED1_OFF                    	        	            (LED1 = LED_OFF)
#define LED1_FLASH                  	      		            (LED1 ^=1)

#define LED2                        				            (GPIO4->DO_f.P4)
#define LED2_ON                     	       		            (LED2 = LED_ON)
#define LED2_OFF                    	      		            (LED2 = LED_OFF)
#define LED2_FLASH                  	     		            (LED2 ^=1)

#define LED3                        				            (GPIO3->DO_f.P2)
#define LED3_ON                     	   			            (LED3 = LED_ON)
#define LED3_OFF                    	          	            (LED3 = LED_OFF)
#define LED3_FLASH                  	         	            (LED3 ^=1)

#define LED4                       					            (GPIO1->DO_f.P0)
#define LED4_ON                     	       		            (LED4 = LED_ON)
#define LED4_OFF                    	         	            (LED4 = LED_OFF)
#define LED4_FLASH                  	        	            (LED4 ^=1)

#define LED5                        	   			            (GPIO4->DO_f.P6)
#define LED5_ON                     	     		            (LED5 = LED_ON)
#define LED5_OFF                    	       		            (LED5 = LED_OFF)
#define LED5_FLASH                  	       		            (LED5 ^=1)

#define LED6                        	     		            (GPIO3->DO_f.P5)
#define LED6_ON                     	       		            (LED6 = LED_ON)
#define LED6_OFF                    	         	            (LED6 = LED_OFF)
#define LED6_FLASH                  	         	            (LED6 ^=1)

#define LED_ERROR_ON                	         	            (LED1=LED2=LED3=LED_ON)
#define LED_ERROR_OFF               	          	            (LED1=LED2=LED3=LED_OFF)

/****************************************************************************/


/****************************************************************************
*(5)
*ADC采样端口使用
*
*****************************************************************************/
   
#define ADC1_IBusPeak_Convert         	                        (ADC1_CH_30_MSK)
#define ADC1_IBusPeak_Result            	                        (ADC1_CH_30)

#define ADC1_IBusAvg_Convert     		                        (ADC1_CH_14_MSK)
#define ADC1_IBusAvg_Result            	                        (ADC1_CH_14)

#define ADC1_VBus_Convert           		                    (ADC1_CH_27_MSK)
#define ADC1_VBus_Result               	                        (ADC1_CH_27)

#define ADC1_MosNtc_Convert           	                        (ADC1_CH_7_MSK)/*MOS NTC*/
#define ADC1_MosNtc_Result             	                        (ADC1_CH_7)

#define ADC1_BatNtc_Convert            	                        (ADC1_CH_20_MSK)/*BAT  NTC*/
#define ADC1_BatNtc_Result              	                        (ADC1_CH_20)

#define ADC_Trigger_Convert        		                        (ADC1_CH_25_MSK)
#define ADC_Trigger_Result            	                        (ADC1_CH_25)

#define ADC_LLO_Convert               	                        (ADC1_CH_17_MSK)
#define ADC_LLO_Result                              	        (ADC1_CH_17)

#define ADC_IA_Convert               	                        (ADC1_CH_11_MSK)
#define ADC_IA_Result                              	        	(ADC1_CH_11)
#define ADC_IB_Convert               	                        (ADC1_CH_15_MSK)
#define ADC_IB_Result                              	        	(ADC1_CH_15)

/****************************************************************************/


/****************************************************************************
*(6)
*端电压采样端口(不可更改)
*
*****************************************************************************/
   
#define ADC_U_Convert               	                        (ADC1_CH_8_MSK)
#define ADC_U_Result                  	                        (ADC1_CH_8)	 				                    
#define ADC_V_Convert              		                        (ADC1_CH_9_MSK)
#define ADC_V_Result             		                        (ADC1_CH_9)	 				                    
#define ADC_W_Convert                 	                        (ADC1_CH_10_MSK)
#define ADC_W_Result                 	                        (ADC1_CH_10)	 				                    
#define ADC_N_Convert                 	                        (ADC1_CH_26_MSK)
#define ADC_N_Result                 	                        (ADC1_CH_26)
/****************************************************************************/



/****************************************************************************
*(8)
*过流比较器配置用于短路保护
*
*****************************************************************************/
                     
#define OCP_ACMP_Set                	                        (ACMP1)/*OCP比较器序号与运放相同才可以使用内部联动*/
#define OCP_ACMP_Possel_Set									    (ACMP_POSSEL_OPAO)
#define OCP_ACMP_Negsel_Set           	                        (ACMP_NEGSEL_VREF_VDD_K)
#define OCP_ACMP_Negsel_Vref           	                        (V_B_COF_13)/*V_B_COF_13*ACMP_NEGSEL_VREF_VDD_K<3.5V   */
#define OCP_ACMP_Filter               	                        (ACMP_NGCLK_65_TSYS)/*比较器电平跳变滤波周期*/

/****************************************************************************/


/****************************************************************************
*(9)
*过零比较器配置(不可更改)
*
*****************************************************************************/
        
#define BEMF_ACMP_Set              		                        (ACMP0)
#define BEMF_U                      	                        (ACMP_POSSEL_P0)
#define BEMF_V                       	                        (ACMP_POSSEL_P1)
#define BEMF_W                        	                        (ACMP_POSSEL_P2)
#define BEMF_ACMP_Filter              	                        (ACMP_NGCLK_65_TSYS)/*比较器电平跳变滤波周期*/

/****************************************************************************/


/****************************************************************************
*(7)
*EPWM频率设置。
*PWM_Duty_MinLimit换算后的时间需要满足ADC硬件转换时间。
*****************************************************************************/
#define PWM_Fre_Clk   											(Fre_MCU/1)
#define PWM_Fre_Base                  	                        (10000)/*4-16KHZ,启动载波频率*/
#define PWM_Fre_High                  	                        (10000)/*4-16KHZ,高速载波频率，如果等于起始载波则不变*/

#define PWM_Period                    	                        (uint32_t)((float)PWM_Fre_Clk/PWM_Fre_Base/2) /*mcu载波计数周期，由频率计算得出*/
#define PWM_Period_Half              	                        (uint32_t)((float)PWM_Fre_Clk/PWM_Fre_Base/4)
#define PWM_DT_Cnt                     	                        (96)/*64 == 1us  死区时间,与主频相关*/
#define PWM_Duty_MinLimit              	                        (320)/*10us*64/2=320 3AD启动需要保证最小10us左右脉宽*/
 
#define PWM_Driver_Up_H_Down_H         	                    	(0ul)/*6N*/
#define PWM_Driver_Up_L_Down_L         	                    	(1ul)/*6P*/
#define PWM_Driver_Up_H_Down_L         	                    	(2ul)/*3N3P*/
#define PWM_Driver_Up_L_Down_H         	                    	(3ul)/*3P3N*/
#define PWM_Driver_Set                	                    	(PWM_Driver_Up_H_Down_H)/*驱动模式选择,根据驱动芯片的类型或外部电路选择*/

#define PWM_Driver_L3H3L2H2L1H1        	                    	(0xAA543210)  
#define PWM_Driver_L3L2L1H3H2H1        	                    	(0xAA531420) 
#define PWM_Driver_Poremap           	                    	(PWM_Driver_L3L2L1H3H2H1)/*根据打线图修改*/

/****************************************************************************/


/****************************************************************************
*(7)
*EPWM频率设置。
*PWM_Duty_MinLimit换算后的时间需要满足ADC硬件转换时间。
*****************************************************************************/
#define CCP_Cap_Clk_Div                 						(CCP_CLK_DIV_64)

#define CCP_Cap_Disable											(0)					
#define CCP_Cap_Enable											(1)					
#define	Config_CCP_Cap											(CCP_Cap_Enable)				
#define CCP_Cap_Module   										(CCP1)								
#define CCP_Cap_CH   											(CCPxA)								

#define CCP_Pwm_Clk_Div                 						(CCP_CLK_DIV_64)

#define CCP_Pwm_Disable				   							(0)					
#define CCP_Pwm_Enable				    						(1)					
#define	Config_CCP_Pwm				    						(CCP_Pwm_Disable)				   
#define CCP_Pwm_Module   			    						(CCP1)								
#define CCP_Pwm_CH   				    						(CCPxB)		
/****************************************************************************/










/****************************************************************************/

/****************************************************************************/




/****************************************************************************/
/*	Global variable definitions(declared in header file with 'extern')
*****************************************************************************/


void BSP_BEMF_ACMP_Config(void);
void BSP_OCP_ACMP_Config(void);



void BSP_CCP_Config(void);

void BSP_CCP_Cap_Config(void);
void BSP_CCP_Pwm_Config(void);
void  BSP_OPA1_Config(void);
void  BSP_OPA0_Config(void);
void BSP_PGA0_Config(void);
void BSP_PGA1_Config(void);
void BSP_PWM_Config(void);
void BSP_SYS_Config(void);
void BSP_TMR0_Config(void);
void BSP_TMR1_Config(void);


void BSP_UART_Config(void);
void VScope_Send(s16 para1,s16 para2, s16 para3,s16 para4);
void BSP_WDT_Config(void);
void BSP_ClearWDT(void);


void BSP_SetInt_Priority(void);
void BSP_Enable_Interrup(void);
void BSP_Disable_Interrup(void);


void BSP_Init_End(void);
void BSP_PowerOn_CheckSelf(void);

void MCU_Config(void);

#endif /* __MCU_CONFIG_H__ */

