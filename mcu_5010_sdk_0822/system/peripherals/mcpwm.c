#include "platform_common.h"

#include "mcpwm_type.h"
#include "mcpwm_reg.h"
#include "device_resource.h"
#include "mcpwm.h"



//void MCPWM_Init(MCPWM_InitTypeDef* MCPWM_InitStruct){}
//void MCPWM_StructInit(MCPWM_InitTypeDef* MCPWM_InitStruct){}
void PWMOutputs(FuncState t_state){}
void MCPWM_ClearIRQFlag(MCPWM0CNTx CNTx,uint32_t INT_flag){}
u16 MCPWM_GetIRQFlag(MCPWM0CNTx CNTx,uint32_t INT_flag){return 0;}
//void MCPWM_Clk_Enable(u8 Clk_Enable){}
void MCPWM_DIV_Set(u8 ClockDiv){}
void MCPWM_CNTx_Enable(MCPWM0CNTx CNTx, FuncState state){}
//void MCPWM_CH012_Init(MCPWM_InitTypeDef* MCPWM_InitStruct){}
//void MCPWM_CH345_Init(MCPWM_InitTypeDef* MCPWM_InitStruct){}
//void MCPWM_CH012_IESet(MCPWM_InitTypeDef* MCPWM_InitStruct){}
//void MCPWM_CH345_IESet(MCPWM_InitTypeDef* MCPWM_InitStruct){}
void MCPWM_ModuleClockCmd(FuncState state){}


void MCPWM_Software_Reset()
{
	
	REG32(0x40020020) = 0x1000000; //MCPWM RST

}



/**
 *@brief @b 函数名称:   void MCPWM_StructInit(MCPWM_InitTypeDef* MCPWM_InitStruct)
 *@brief @b 功能描述:   MCPWM结构体初始化
 *@see被引用内容：       无
 *@param输入参数：       MCPWM_InitTypeDef *MCPWM_InitStruct
 *@param输出参数：      无
 *@return返 回 值：     无
 *@note其它说明：       无
 *@warning             无
 *@par 示例代码：
 *@code
            MCPWM_InitTypeDef MCPWM_InitStructure;
            MCPWM_StructInit(&MCPWM_InitStructure);//初始化MCPWM相关结构体
  @endcode
 *@par 修改日志:
 * <table>
 * <tr><th>Date	        <th>Version    <th>Author      <th>Description
 * <tr><td>2019年05月21日 <td>1.0     <td>Howlet      <td>创建
 * </table>
 */
void MCPWM_StructInit(MCPWM_InitTypeDef* MCPWM_InitStruct)
{
    memset(MCPWM_InitStruct, 0, sizeof(MCPWM_InitTypeDef));
}

/**
 *@brief @b 函数名称:   void MCPWM_Init(MCPWM_InitTypeDef *MCPWM_InitStruct)
 *@brief @b 功能描述:   MCPWM初始化函数
 *@see被引用内容：      SYS_ModuleClockCmd()
 *@param输入参数：      MCPWM_InitTypeDef *MCPWM_InitStruct
 *@param输出参数：      无
 *@return返 回 值：     无
 *@note其它说明：       无
 *@warning              无
 *@par 示例代码：
 *@code
            MCPWM_InitTypeDef MCPWM_InitStructure;
            MCPWM_StructInit(&MCPWM_InitStructure);//初始化MCPWM相关结构体
            MCPWM_InitStructure.MCLK_EN = ENABLE;
            MCPWM_InitStructure.CLK_DIV = 0;
            MCPWM_InitStructure.CMP_FLT_CLKDIV = 12;
            MCPWM_InitStructure.BASE_CNT0_EN = ENABLE; 
            MCPWM_InitStructure.TH0 = PWM_PERIOD0; 
            MCPWM_InitStructure.TH00 = -(PWM_PERIOD0 >> 2);
			MCPWM_InitStructure.TH01 = (PWM_PERIOD0 >> 2);
			MCPWM_InitStructure.TH10 = -(PWM_PERIOD0 >> 2);
			MCPWM_InitStructure.TH11 = (PWM_PERIOD0 >> 2);
			MCPWM_InitStructure.TH20 = -(PWM_PERIOD0 >> 2);
			MCPWM_InitStructure.TH21 = (PWM_PERIOD0 >> 2);
			
			MCPWM_InitStructure.MCPWM_WorkModeCH0 = MCPWM0_CENTRAL_PWM_MODE; 
			MCPWM_InitStructure.MCPWM_WorkModeCH1 = MCPWM0_CENTRAL_PWM_MODE; 
			MCPWM_InitStructure.MCPWM_WorkModeCH2 = MCPWM0_CENTRAL_PWM_MODE;
			MCPWM_InitStructure.DeadTimeCH012N = DEADTIME;
			MCPWM_InitStructure.DeadTimeCH012P = DEADTIME;
			MCPWM_InitStructure.CMP_CTRL_CNT0  = DISABLE ;
			MCPWM_InitStructure.EVT_CNT0_EN    = DISABLE ;
			MCPWM_InitStructure.EVT0           = DISABLE ;
			
			MCPWM_InitStructure.TR0_UP_INTV     = DISABLE;
			MCPWM_InitStructure.TR0_T0_UpdateEN = ENABLE ;
			MCPWM_InitStructure.TR0_T1_UpdateEN = DISABLE ;
			MCPWM_InitStructure.TR0_AEC         = DISABLE;
			
			MCPWM_InitStructure.T0_Update0_INT_EN = DISABLE; 
			MCPWM_InitStructure.T1_Update0_INT_EN = ENABLE ; 
			MCPWM_InitStructure.Update1_INT_EN = ENABLE;    
            MCPWM_InitStructure.CH0N_Polarity_INV = DISABLE;           
			MCPWM_InitStructure.CH0P_Polarity_INV = DISABLE;         
			MCPWM_InitStructure.CH1N_Polarity_INV = DISABLE;
			MCPWM_InitStructure.CH1P_Polarity_INV = DISABLE;
			MCPWM_InitStructure.CH2N_Polarity_INV = DISABLE;
			MCPWM_InitStructure.CH2P_Polarity_INV = DISABLE;

			MCPWM_InitStructure.Switch_CH0N_CH0P =  DISABLE;
			MCPWM_InitStructure.Switch_CH1N_CH1P =  DISABLE; 
			MCPWM_InitStructure.Switch_CH2N_CH2P =  DISABLE; 
			MCPWM_InitStructure.CH0P_default_output = MCPWM0_LOW_LEVEL;
			MCPWM_InitStructure.CH0N_default_output = MCPWM0_LOW_LEVEL;
			MCPWM_InitStructure.CH1P_default_output = MCPWM0_LOW_LEVEL;
			MCPWM_InitStructure.CH1N_default_output = MCPWM0_LOW_LEVEL;     
			MCPWM_InitStructure.CH2P_default_output = MCPWM0_LOW_LEVEL;
			MCPWM_InitStructure.CH2N_default_output = MCPWM0_LOW_LEVEL;			
            MCPWM_Init(&MCPWM_InitStructure);

  @endcode
 *@par 修改日志:
 * <table>
 * <tr><th>Date	          <th>Version  <th>Author      <th>Description
 * <tr><td>2023年03月06日 <td>1.0      <td>Zhujie      <td>创建
 * </table>
 */
void MCPWM_Init(MCPWM_InitTypeDef* MCPWM_InitStruct)
{
    //SYS_ModuleClockCmd(SYS_Module_MCPWM0,ENABLE);//MCPWM时钟使能
    
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */
	
    MCPWM0_TCLK = MCPWM_InitStruct->CLK_DIV                                                            \
	              | (MCPWM_InitStruct->MCLK_EN       << 2 )  | (MCPWM_InitStruct->ZCS_EN        << 3 )   \
	              | (MCPWM_InitStruct->TMR2_TB       << 4 )  | (MCPWM_InitStruct->TMR3_TB       << 5 )   \
	              | (MCPWM_InitStruct->BASE_CNT0_EN  << 6 )  | (MCPWM_InitStruct->BASE_CNT1_EN  << 7 )   \
	              | (MCPWM_InitStruct->EVT_CNT0_EN   << 8 )  | (MCPWM_InitStruct->EVT_CNT1_EN   << 9 )   \
	              | (MCPWM_InitStruct->CMP_CTRL_CNT0 << 10)  | (MCPWM_InitStruct->CMP_CTRL_CNT1 << 11)   ;

    MCPWM0_TMR0 = MCPWM_InitStruct->TMR0;
    MCPWM0_TMR1 = MCPWM_InitStruct->TMR1;
    MCPWM0_TMR2 = MCPWM_InitStruct->TMR2;
    MCPWM0_TMR3 = MCPWM_InitStruct->TMR3;
	
	  MCPWM0_TH0 = MCPWM_InitStruct->TH0;
	  MCPWM0_TH1 = MCPWM_InitStruct->TH1;
						
    MCPWM0_SDCFG =   MCPWM_InitStruct->TR0_UP_INTV              | (MCPWM_InitStruct->TR0_T0_UpdateEN << 4 ) \
                  | (MCPWM_InitStruct->TR0_T1_UpdateEN << 5 )   | (MCPWM_InitStruct->TR0_AEC         << 6 ) \
									| (MCPWM_InitStruct->TR1_UP_INTV     << 8 )   | (MCPWM_InitStruct->TR1_T0_UpdateEN << 12) \
                  | (MCPWM_InitStruct->TR1_T1_UpdateEN << 13)   | (MCPWM_InitStruct->TR1_AEC         << 14) ;

    MCPWM0_DTH00 = MCPWM_InitStruct->DeadTimeCH012N;
    MCPWM0_DTH01 = MCPWM_InitStruct->DeadTimeCH012P;
    MCPWM0_DTH10 = MCPWM_InitStruct->DeadTimeCH345N;
		MCPWM0_DTH11 = MCPWM_InitStruct->DeadTimeCH345P;

    MCPWM0_EIF = 0xffff;
    MCPWM0_IE0 =  MCPWM_InitStruct->T0_Update0_INT_EN         | (MCPWM_InitStruct->T1_Update0_INT_EN  << 1 )   \
		            |(MCPWM_InitStruct->TH00_Match0_INT_EN << 2 ) | (MCPWM_InitStruct->TH01_Match0_INT_EN << 3 )   \
								|(MCPWM_InitStruct->TH10_Match0_INT_EN << 4 ) | (MCPWM_InitStruct->TH11_Match0_INT_EN << 5 )   \
								|(MCPWM_InitStruct->TH20_Match0_INT_EN << 6 ) | (MCPWM_InitStruct->TH21_Match0_INT_EN << 7 )   \
                |(MCPWM_InitStruct->TMR0_Match0_INT_EN << 10) | (MCPWM_InitStruct->TMR1_Match0_INT_EN << 11)   \
								|(MCPWM_InitStruct->TMR2_Match0_INT_EN << 12) | (MCPWM_InitStruct->TMR3_Match0_INT_EN << 13)   \
								|(MCPWM_InitStruct->Update0_INT_EN << 14) ;
								
		MCPWM0_IE1 =  MCPWM_InitStruct->T0_Update1_INT_EN         | (MCPWM_InitStruct->T1_Update1_INT_EN  << 1 )   \
		            |(MCPWM_InitStruct->TH00_Match1_INT_EN << 2 ) | (MCPWM_InitStruct->TH01_Match1_INT_EN << 3 )   \
								|(MCPWM_InitStruct->TH10_Match1_INT_EN << 4 ) | (MCPWM_InitStruct->TH11_Match1_INT_EN << 5 )   \
								|(MCPWM_InitStruct->TH20_Match1_INT_EN << 6 ) | (MCPWM_InitStruct->TH21_Match1_INT_EN << 7 )   \
                |(MCPWM_InitStruct->TMR0_Match1_INT_EN << 10) | (MCPWM_InitStruct->TMR1_Match1_INT_EN << 11)   \
								|(MCPWM_InitStruct->TMR2_Match1_INT_EN << 12) | (MCPWM_InitStruct->TMR3_Match1_INT_EN << 13)   \
								|(MCPWM_InitStruct->Update1_INT_EN << 14) ;

    MCPWM0_EIE = (MCPWM_InitStruct->FAIL0_INT_EN << 4) | (MCPWM_InitStruct->FAIL1_INT_EN << 5)  \
		            |(MCPWM_InitStruct->FAIL2_INT_EN << 6) | (MCPWM_InitStruct->FAIL3_INT_EN << 7)  \
								|(MCPWM_InitStruct->ZCS0_INT_EN  << 8) | (MCPWM_InitStruct->ZCS1_INT_EN  << 9)  ;

    MCPWM0_UPDATE = 0xffff;    /* write whatever value to trigger update */
    
		MCPWM0_EVT0 = MCPWM_InitStruct->EVT0;
		
		MCPWM0_EVT1 = MCPWM_InitStruct->EVT1;
		
		MCPWM0_FLT = MCPWM_InitStruct->IO_CMP_FLT_CLKDIV;
		
		MCPWM0_RE =   MCPWM_InitStruct->TMR0_RE0          | (MCPWM_InitStruct->TMR1_RE0   << 1 ) \
		           | (MCPWM_InitStruct->TMR2_RE0   << 2 ) | (MCPWM_InitStruct->TMR3_RE0   << 3 ) \
							 | (MCPWM_InitStruct->TR0_T0_RE0 << 4 ) | (MCPWM_InitStruct->TR0_T1_RE0 << 5 ) \
							 | (MCPWM_InitStruct->TR1_T0_RE0 << 6 ) | (MCPWM_InitStruct->TR1_T1_RE0 << 7 ) \
							 | (MCPWM_InitStruct->TMR0_RE1   << 8 ) | (MCPWM_InitStruct->TMR1_RE1   << 9 ) \
							 | (MCPWM_InitStruct->TMR2_RE1   << 10) | (MCPWM_InitStruct->TMR3_RE1   << 11) \
							 | (MCPWM_InitStruct->TR0_T0_RE1 << 12) | (MCPWM_InitStruct->TR0_T1_RE1 << 13) \
							 | (MCPWM_InitStruct->TR1_T0_RE1 << 14) | (MCPWM_InitStruct->TR1_T1_RE1 << 15) ;
							 
		MCPWM0_PP   =   MCPWM_InitStruct->IO0_PPE        | (MCPWM_InitStruct->IO1_PPE << 1 )  \
		             | (MCPWM_InitStruct->IO2_PPE << 2 ) | (MCPWM_InitStruct->IO3_PPE << 3 )  \
								 | (MCPWM_InitStruct->IO4_PPE << 4 ) | (MCPWM_InitStruct->IO5_PPE << 5 );
		
		MCPWM0_IO01 =   MCPWM_InitStruct->CH0N_Polarity_INV        | (MCPWM_InitStruct->CH0P_Polarity_INV << 1 ) \
                 | (MCPWM_InitStruct->CH0_NS            << 2 ) | (MCPWM_InitStruct->CH0_PS            << 3 ) \
                 | (MCPWM_InitStruct->CH0N_SCTRLN       << 4 ) | (MCPWM_InitStruct->CH0P_SCTRLP       << 5 ) \
                 | (MCPWM_InitStruct->Switch_CH0N_CH0P  << 6 ) | (MCPWM_InitStruct->MCPWM_WorkModeCH0 << 7 ) \
                 | (MCPWM_InitStruct->CH1N_Polarity_INV << 8 ) | (MCPWM_InitStruct->CH1P_Polarity_INV << 9 ) \
                 | (MCPWM_InitStruct->CH1_NS            << 10) | (MCPWM_InitStruct->CH1_PS            << 11) \
	               | (MCPWM_InitStruct->CH1N_SCTRLN       << 12) | (MCPWM_InitStruct->CH1P_SCTRLP       << 13) \
	               | (MCPWM_InitStruct->Switch_CH1N_CH1P  << 14) | (MCPWM_InitStruct->MCPWM_WorkModeCH1 << 15)  ;

    MCPWM0_IO23 =   MCPWM_InitStruct->CH2N_Polarity_INV        | (MCPWM_InitStruct->CH2P_Polarity_INV << 1 ) \
                 | (MCPWM_InitStruct->CH2_NS            << 2 ) | (MCPWM_InitStruct->CH2_PS            << 3 ) \
                 | (MCPWM_InitStruct->CH2N_SCTRLN       << 4 ) | (MCPWM_InitStruct->CH2P_SCTRLP       << 5 ) \
                 | (MCPWM_InitStruct->Switch_CH2N_CH2P  << 6 ) | (MCPWM_InitStruct->MCPWM_WorkModeCH2 << 7 ) \
                 | (MCPWM_InitStruct->CH3N_Polarity_INV << 8 ) | (MCPWM_InitStruct->CH3P_Polarity_INV << 9 ) \
                 | (MCPWM_InitStruct->CH3_NS            << 10) | (MCPWM_InitStruct->CH3_PS            << 11) \
	               | (MCPWM_InitStruct->CH3N_SCTRLN       << 12) | (MCPWM_InitStruct->CH3P_SCTRLP       << 13) \
	               | (MCPWM_InitStruct->Switch_CH3N_CH3P  << 14) | (MCPWM_InitStruct->MCPWM_WorkModeCH3 << 15)  ;
								 
		MCPWM0_IO45 =   MCPWM_InitStruct->CH4N_Polarity_INV        | (MCPWM_InitStruct->CH4P_Polarity_INV << 1 ) \
                 | (MCPWM_InitStruct->CH4_NS            << 2 ) | (MCPWM_InitStruct->CH4_PS            << 3 ) \
                 | (MCPWM_InitStruct->CH4N_SCTRLN       << 4 ) | (MCPWM_InitStruct->CH4P_SCTRLP       << 5 ) \
                 | (MCPWM_InitStruct->Switch_CH4N_CH4P  << 6 ) | (MCPWM_InitStruct->MCPWM_WorkModeCH4 << 7 ) \
                 | (MCPWM_InitStruct->CH5N_Polarity_INV << 8 ) | (MCPWM_InitStruct->CH5P_Polarity_INV << 9 ) \
                 | (MCPWM_InitStruct->CH5_NS            << 10) | (MCPWM_InitStruct->CH5_PS            << 11) \
	               | (MCPWM_InitStruct->CH5N_SCTRLN       << 12) | (MCPWM_InitStruct->CH5P_SCTRLP       << 13) \
	               | (MCPWM_InitStruct->Switch_CH5N_CH5P  << 14) | (MCPWM_InitStruct->MCPWM_WorkModeCH5 << 15)  ;
								 
		MCPWM0_TH00 = MCPWM_InitStruct->TH00;
		MCPWM0_TH01 = MCPWM_InitStruct->TH01;
		MCPWM0_TH10 = MCPWM_InitStruct->TH10;
		MCPWM0_TH11 = MCPWM_InitStruct->TH11;
		MCPWM0_TH20 = MCPWM_InitStruct->TH20;
		MCPWM0_TH21 = MCPWM_InitStruct->TH21;
		MCPWM0_TH30 = MCPWM_InitStruct->TH30;
		MCPWM0_TH31 = MCPWM_InitStruct->TH31;
		MCPWM0_TH40 = MCPWM_InitStruct->TH40;
		MCPWM0_TH41 = MCPWM_InitStruct->TH41;
		MCPWM0_TH50 = MCPWM_InitStruct->TH50;
		MCPWM0_TH51 = MCPWM_InitStruct->TH51;
		
		MCPWM0_FAIL012 = (MCPWM_InitStruct->FAIL0_Polarity    << 2 ) | (MCPWM_InitStruct->FAIL1_Polarity       << 3)  \
                   | (MCPWM_InitStruct->FAIL0_INPUT_EN    << 4 ) | (MCPWM_InitStruct->FAIL1_INPUT_EN       << 5)  \
                   | (MCPWM_InitStruct->HALT_PRT0         << 7 ) | (MCPWM_InitStruct->FAIL0_Signal_Sel     << 8)  \
                   | (MCPWM_InitStruct->FAIL1_Signal_Sel  << 10) | (MCPWM_InitStruct->FAIL_0CAP            << 15) ;
									
    MCPWM0_FAIL345 = (MCPWM_InitStruct->FAIL2_Polarity    << 2 ) | (MCPWM_InitStruct->FAIL3_Polarity       << 3)  \
                  | (MCPWM_InitStruct->FAIL2_INPUT_EN     << 4 ) | (MCPWM_InitStruct->FAIL3_INPUT_EN       << 5)  \
                  | (MCPWM_InitStruct->HALT_PRT1          << 7 ) | (MCPWM_InitStruct->FAIL2_Signal_Sel     << 8)  \
                  | (MCPWM_InitStruct->FAIL3_Signal_Sel   << 10) | (MCPWM_InitStruct->FAIL_1CAP            << 15) ;
		
		MCPWM0_CH_DEF =  MCPWM_InitStruct->CH0N_default_output         | (MCPWM_InitStruct->CH0P_default_output << 1 ) \
		               |(MCPWM_InitStruct->CH1N_default_output << 2 )  | (MCPWM_InitStruct->CH1P_default_output << 3 ) \
									 |(MCPWM_InitStruct->CH2N_default_output << 4 )  | (MCPWM_InitStruct->CH2P_default_output << 5 ) \
									 |(MCPWM_InitStruct->CH3N_default_output << 6 )  | (MCPWM_InitStruct->CH3P_default_output << 7 ) \
									 |(MCPWM_InitStruct->CH4N_default_output << 8 )  | (MCPWM_InitStruct->CH4P_default_output << 9 ) \
									 |(MCPWM_InitStruct->CH5N_default_output << 10)  | (MCPWM_InitStruct->CH5P_default_output << 11) ;
		
		MCPWM0_CH_MASK =   MCPWM_InitStruct->CH0N_FAIL_EN        | (MCPWM_InitStruct->CH0P_FAIL_EN << 1 )  \
		                | (MCPWM_InitStruct->CH1N_FAIL_EN << 2 ) | (MCPWM_InitStruct->CH1P_FAIL_EN << 3 )  \
										| (MCPWM_InitStruct->CH2N_FAIL_EN << 4 ) | (MCPWM_InitStruct->CH2P_FAIL_EN << 5 )  \
										| (MCPWM_InitStruct->CH3N_FAIL_EN << 6 ) | (MCPWM_InitStruct->CH3P_FAIL_EN << 7 )  \
										| (MCPWM_InitStruct->CH4N_FAIL_EN << 8 ) | (MCPWM_InitStruct->CH4P_FAIL_EN << 9 )  \
										| (MCPWM_InitStruct->CH5N_FAIL_EN << 10) | (MCPWM_InitStruct->CH5P_FAIL_EN << 11)  ;
										
    MCPWM0_PRT = 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */
}

void MCPWM_Clk_Enable(u8 Clk_Enable)
{
	//MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */

	if(Clk_Enable==1)
	{
		MCPWM0_TCLK |= (Clk_Enable << 2);
	}
	else
	{
		MCPWM0_TCLK &= ~(1 << 2);
	}
	MCPWM0_PRT = 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */
}


void MCPWM_CH012_Init(MCPWM_InitTypeDef* MCPWM_InitStruct)
{
	
	 unsigned int reg_value0 = MCPWM0_TCLK;
	 unsigned int reg_value1 = MCPWM0_SDCFG;
	 unsigned int reg_value2 = MCPWM0_EIE;
	 unsigned int reg_value3 = MCPWM0_RE;
	 unsigned int reg_value4 = MCPWM0_PP;
	 unsigned int reg_value5 = MCPWM0_IO23;
	 unsigned int reg_value6 = MCPWM0_CH_DEF;
	 unsigned int reg_value7 = MCPWM0_CH_MASK;
	
	
	// reg_value0 = MCPWM0_TCLK;
	// reg_value1 = MCPWM0_SDCFG;
	// reg_value2 = MCPWM0_EIE;
	// reg_value3 = MCPWM0_RE;
	// reg_value4 = MCPWM0_PP;
	// reg_value5 = MCPWM0_IO23;
	// reg_value6 = MCPWM0_CH_DEF;
	// reg_value7 = MCPWM0_CH_MASK;
	
	

	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */
	
	
	MCPWM0_TCLK |= (ENABLE << 2);	//jack add
	
	reg_value0 = MCPWM0_TCLK;		//jack add

	reg_value0 &= ~(1 << 3) & ~(1 << 8) & ~(1 << 10); // 创建掩码

	reg_value0 |=(MCPWM_InitStruct->ZCS_EN        << 3 )                                              \
					| (MCPWM_InitStruct->EVT_CNT0_EN   << 8 )  | (MCPWM_InitStruct->CMP_CTRL_CNT0 << 10)   ;
    
	MCPWM0_TCLK = reg_value0;
	
    MCPWM0_TMR0 = MCPWM_InitStruct->TMR0;
    MCPWM0_TMR1 = MCPWM_InitStruct->TMR1;
	
	MCPWM0_TH0 = MCPWM_InitStruct->TH0;
	
	reg_value1 &= ~(1) & ~(1 << 4) & ~(1 << 5) & ~(1 << 6); // 创建掩码
    reg_value1 |= MCPWM_InitStruct->TR0_UP_INTV | (MCPWM_InitStruct->TR0_T0_UpdateEN << 4) \
                 | (MCPWM_InitStruct->TR0_T1_UpdateEN << 5) | (MCPWM_InitStruct->TR0_AEC << 6);
    MCPWM0_SDCFG = reg_value1;

    MCPWM0_DTH00 = MCPWM_InitStruct->DeadTimeCH012N;
    MCPWM0_DTH01 = MCPWM_InitStruct->DeadTimeCH012P;

    MCPWM0_EIF = 0xffff;
		
	MCPWM0_FAIL012 = (MCPWM_InitStruct->FAIL0_Polarity    << 2 ) | (MCPWM_InitStruct->FAIL1_Polarity       << 3)  \
                   | (MCPWM_InitStruct->FAIL0_INPUT_EN    << 4 ) | (MCPWM_InitStruct->FAIL1_INPUT_EN       << 5)  \
                   | (MCPWM_InitStruct->HALT_PRT0         << 7 ) | (MCPWM_InitStruct->FAIL0_Signal_Sel     << 8)  \
                   | (MCPWM_InitStruct->FAIL1_Signal_Sel  << 10) | (MCPWM_InitStruct->FAIL_0CAP            << 15) ;
									 
	reg_value2	&= ~(1 << 4) & ~(1 << 5) & ~(1 << 8); // 创建掩码
    reg_value2	|=(MCPWM_InitStruct->FAIL0_INT_EN << 4) | (MCPWM_InitStruct->FAIL1_INT_EN << 5)  \
							 | (MCPWM_InitStruct->ZCS0_INT_EN  << 8) ;
    MCPWM0_EIE	= reg_value2;

    //MCPWM0_UPDATE	= 0xffff;    /* write whatever value to trigger update */
    
	MCPWM0_EVT0	= MCPWM_InitStruct->EVT0;
			
	MCPWM0_FLT	= MCPWM_InitStruct->IO_CMP_FLT_CLKDIV;
		
	reg_value3	&= ~(1) & ~(1 << 1) & ~(1 << 4) & ~(1 << 5) & ~(1 << 8) & ~(1 << 9) & ~(1 << 12) & ~(1 << 13); // 创建掩码
	reg_value3	|= MCPWM_InitStruct->TMR0_RE0          | (MCPWM_InitStruct->TMR1_RE0   << 1 ) \
						 | (MCPWM_InitStruct->TR0_T0_RE0 << 4 ) | (MCPWM_InitStruct->TR0_T1_RE0 << 5 ) \
						 | (MCPWM_InitStruct->TMR0_RE1   << 8 ) | (MCPWM_InitStruct->TMR1_RE1   << 9 ) \
						 | (MCPWM_InitStruct->TR0_T0_RE1 << 12) | (MCPWM_InitStruct->TR0_T1_RE1 << 13) ;
	MCPWM0_RE	= reg_value3;					
			
	reg_value4	&= ~(1) & ~(1 << 1) & ~(1 << 2); // 创建掩码
	reg_value4	|= MCPWM_InitStruct->IO0_PPE        | (MCPWM_InitStruct->IO1_PPE << 1 )  \
	        		| (MCPWM_InitStruct->IO2_PPE << 2 ) ;
    MCPWM0_PP = reg_value4;
		
	MCPWM0_IO01	=	  (MCPWM_InitStruct->CH0N_Polarity_INV      )	| (MCPWM_InitStruct->CH0P_Polarity_INV << 1 )	\
					| (MCPWM_InitStruct->CH0_NS            << 2 )	| (MCPWM_InitStruct->CH0_PS            << 3 )	\
					| (MCPWM_InitStruct->CH0N_SCTRLN       << 4 )	| (MCPWM_InitStruct->CH0P_SCTRLP       << 5 )	\
					| (MCPWM_InitStruct->Switch_CH0N_CH0P  << 6 )	| (MCPWM_InitStruct->MCPWM_WorkModeCH0 << 7 )	\
					| (MCPWM_InitStruct->CH1N_Polarity_INV << 8 )	| (MCPWM_InitStruct->CH1P_Polarity_INV << 9 )	\
					| (MCPWM_InitStruct->CH1_NS            << 10)	| (MCPWM_InitStruct->CH1_PS            << 11)	\
					| (MCPWM_InitStruct->CH1N_SCTRLN       << 12)	| (MCPWM_InitStruct->CH1P_SCTRLP       << 13)	\
					| (MCPWM_InitStruct->Switch_CH1N_CH1P  << 14)	| (MCPWM_InitStruct->MCPWM_WorkModeCH1 << 15);

	reg_value5	&=	~(1) & ~(1 << 1) & ~(1 << 2) & ~(1 << 3) & ~(1 << 4) & ~(1 << 5) & ~(1 << 6) & ~(1 << 7); // 创建掩码
	reg_value5  |=	MCPWM_InitStruct->CH2N_Polarity_INV				| (MCPWM_InitStruct->CH2P_Polarity_INV << 1 )	\
					| (MCPWM_InitStruct->CH2_NS            << 2 )	| (MCPWM_InitStruct->CH2_PS            << 3 )	\
					| (MCPWM_InitStruct->CH2N_SCTRLN       << 4 )	| (MCPWM_InitStruct->CH2P_SCTRLP       << 5 )	\
					| (MCPWM_InitStruct->Switch_CH2N_CH2P  << 6 )	| (MCPWM_InitStruct->MCPWM_WorkModeCH2 << 7 );
    MCPWM0_IO23 = reg_value5;

	MCPWM0_TH00 = MCPWM_InitStruct->TH00;
	MCPWM0_TH01 = MCPWM_InitStruct->TH01;
	MCPWM0_TH10 = MCPWM_InitStruct->TH10;
	MCPWM0_TH11 = MCPWM_InitStruct->TH11;
	MCPWM0_TH20 = MCPWM_InitStruct->TH20;
	MCPWM0_TH21 = MCPWM_InitStruct->TH21;
		
	reg_value6	&= ~(1) & ~(1 << 1) & ~(1 << 2) & ~(1 << 3) & ~(1 << 4) & ~(1 << 5); // 创建掩码
	reg_value6  |=	MCPWM_InitStruct->CH0N_default_output         | (MCPWM_InitStruct->CH0P_default_output << 1 ) \
	           		|(MCPWM_InitStruct->CH1N_default_output << 2 )  | (MCPWM_InitStruct->CH1P_default_output << 3 ) \
					|(MCPWM_InitStruct->CH2N_default_output << 4 )  | (MCPWM_InitStruct->CH2P_default_output << 5 ) ;
	MCPWM0_CH_DEF = reg_value6;

	reg_value7	&= ~(1) & ~(1 << 1) & ~(1 << 2) & ~(1 << 3) & ~(1 << 4) & ~(1 << 5); // 创建掩码
	reg_value7  |= 	MCPWM_InitStruct->CH0N_FAIL_EN        | (MCPWM_InitStruct->CH0P_FAIL_EN << 1 )  \
	            | (MCPWM_InitStruct->CH1N_FAIL_EN << 2 ) | (MCPWM_InitStruct->CH1P_FAIL_EN << 3 )  \
				| (MCPWM_InitStruct->CH2N_FAIL_EN << 4 ) | (MCPWM_InitStruct->CH2P_FAIL_EN << 5 )  ;
	MCPWM0_CH_MASK	= reg_value7;

/**********************************************************/

	MCPWM0_FAIL012 |= BIT6;			//jack add  //MOE 控制 MCPWM CH0/CH1/CH2 通道 P 和 N 输出值。 1:输出 MCPWM 产生的正常信号
	
	MCPWM0_UPDATE	= 0xffff;    /* write whatever value to trigger update */  //jack add
	
	//MCPWM0_TCLK |= 0x000000c0; 	//jack add
	MCPWM0_TCLK |= BIT6; 		    //jack add //BASE_CNT0_EN MCPWM 时基 0 计数器使能开关。 1:使能； 0:关闭。
	
	//MCPWM0_AUEN = MCPWM_InitStruct->AUEN;    // jack add
	//MCPWM0_AUEN = 0x3f;
	MCPWM0_AUEN = (MCPWM0_AUEN & 0xFFFF0000) | (0x07FF & 0x0000FFFF);
/**********************************************************/

	MCPWM0_PRT		= 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */
	
}


void MCPWM_CH345_Init(MCPWM_InitTypeDef* MCPWM_InitStruct)
{    
	 unsigned int reg_value0 = MCPWM0_TCLK;
	 unsigned int reg_value1 = MCPWM0_SDCFG;
	 unsigned int reg_value2 = MCPWM0_EIE;
	 unsigned int reg_value3 = MCPWM0_RE;
	 unsigned int reg_value4 = MCPWM0_PP;
	 unsigned int reg_value5 = MCPWM0_IO23;
	 unsigned int reg_value6 = MCPWM0_CH_DEF;
	 unsigned int reg_value7 = MCPWM0_CH_MASK;
	
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */
	
	
	MCPWM0_TCLK |= (ENABLE << 2);	//jack add
	
	reg_value0 = MCPWM0_TCLK;	//jack add

	reg_value0 &= ~(1 << 3) & ~(1 << 4) & ~(1 << 5) & ~(1 << 9) & ~(1 << 11); // 创建掩码
	reg_value0 |=(MCPWM_InitStruct->ZCS_EN	<< 3 )                                              \
	              | (MCPWM_InitStruct->TMR2_TB       << 4 )  | (MCPWM_InitStruct->TMR3_TB       << 5 )   \
	              | (MCPWM_InitStruct->EVT_CNT1_EN   << 9 )  | (MCPWM_InitStruct->CMP_CTRL_CNT1 << 11)   ;
	MCPWM0_TCLK = reg_value0;

	MCPWM0_TMR2 = MCPWM_InitStruct->TMR2;
	MCPWM0_TMR3 = MCPWM_InitStruct->TMR3;

	MCPWM0_TH1 = MCPWM_InitStruct->TH1;
					
	reg_value1 &= ~(1 << 8) & ~(1 << 12) & ~(1 << 13) & ~(1 << 14); // 创建掩码
    reg_value1 |=(MCPWM_InitStruct->TR1_UP_INTV     << 8 )   | (MCPWM_InitStruct->TR1_T0_UpdateEN << 12) \
                 | (MCPWM_InitStruct->TR1_T1_UpdateEN << 13)   | (MCPWM_InitStruct->TR1_AEC         << 14) ;
	MCPWM0_SDCFG = reg_value1;

	MCPWM0_DTH10 = MCPWM_InitStruct->DeadTimeCH345N;
	MCPWM0_DTH11 = MCPWM_InitStruct->DeadTimeCH345P;

	MCPWM0_EIF = 0xffff;
		
    MCPWM0_FAIL345 = (MCPWM_InitStruct->FAIL2_Polarity    << 2 ) | (MCPWM_InitStruct->FAIL3_Polarity       << 3)  \
                  | (MCPWM_InitStruct->FAIL2_INPUT_EN     << 4 ) | (MCPWM_InitStruct->FAIL3_INPUT_EN       << 5)  \
                  | (MCPWM_InitStruct->HALT_PRT1          << 7 ) | (MCPWM_InitStruct->FAIL2_Signal_Sel     << 8)  \
                  | (MCPWM_InitStruct->FAIL3_Signal_Sel   << 10) | (MCPWM_InitStruct->FAIL_1CAP            << 15) ;

	reg_value2 &= ~(1 << 6) & ~(1 << 7) & ~(1 << 9); // 创建掩码
	reg_value2  |=(MCPWM_InitStruct->FAIL2_INT_EN << 6) | (MCPWM_InitStruct->FAIL3_INT_EN << 7)  \
						| (MCPWM_InitStruct->ZCS1_INT_EN  << 9)  ;
    MCPWM0_EIE = reg_value2;

	//MCPWM0_UPDATE = 0xffff0000;    /* write whatever value to trigger update */  //jack update

	MCPWM0_EVT1 = MCPWM_InitStruct->EVT1;

	MCPWM0_FLT = MCPWM_InitStruct->IO_CMP_FLT_CLKDIV;
		
	reg_value3 &= ~(1 << 2) & ~(1 << 3) & ~(1 << 6) & ~(1 << 7) & ~(1 << 10) & ~(1 << 11) & ~(1 << 14) & ~(1 << 15); // 创建掩码
	reg_value3  |=(MCPWM_InitStruct->TMR2_RE0   << 2 ) | (MCPWM_InitStruct->TMR3_RE0   << 3 ) \
						 | (MCPWM_InitStruct->TR1_T0_RE0 << 6 ) | (MCPWM_InitStruct->TR1_T1_RE0 << 7 ) \
						 | (MCPWM_InitStruct->TMR2_RE1   << 10) | (MCPWM_InitStruct->TMR3_RE1   << 11) \
						 | (MCPWM_InitStruct->TR1_T0_RE1 << 14) | (MCPWM_InitStruct->TR1_T1_RE1 << 15) ;
    MCPWM0_RE = reg_value3;

	reg_value4 &= ~(1 << 3) & ~(1 << 4) & ~(1 << 5); // 创建掩码
	reg_value4   |=(MCPWM_InitStruct->IO3_PPE << 3 )  \
					| (MCPWM_InitStruct->IO4_PPE << 4 ) | (MCPWM_InitStruct->IO5_PPE << 5 );
    MCPWM0_PP = reg_value4;

	reg_value5 &= ~(1 << 8) & ~(1 << 9) & ~(1 << 10) & ~(1 << 11) & ~(1 << 12) & ~(1 << 13) & ~(1 << 14) & ~(1 << 15); // 创建掩码
	reg_value5  |=(MCPWM_InitStruct->CH3N_Polarity_INV << 8 ) | (MCPWM_InitStruct->CH3P_Polarity_INV << 9 ) \
	         	| (MCPWM_InitStruct->CH3_NS            << 10) | (MCPWM_InitStruct->CH3_PS            << 11) \
	           	| (MCPWM_InitStruct->CH3N_SCTRLN       << 12) | (MCPWM_InitStruct->CH3P_SCTRLP       << 13) \
				| (MCPWM_InitStruct->Switch_CH3N_CH3P  << 14) | (MCPWM_InitStruct->MCPWM_WorkModeCH3 << 15)  ;
    MCPWM0_IO23 = reg_value5;

	MCPWM0_IO45  =  MCPWM_InitStruct->CH4N_Polarity_INV        | (MCPWM_InitStruct->CH4P_Polarity_INV << 1 ) \
				| (MCPWM_InitStruct->CH4_NS            << 2 ) | (MCPWM_InitStruct->CH4_PS            << 3 ) \
				| (MCPWM_InitStruct->CH4N_SCTRLN       << 4 ) | (MCPWM_InitStruct->CH4P_SCTRLP       << 5 ) \
				| (MCPWM_InitStruct->Switch_CH4N_CH4P  << 6 ) | (MCPWM_InitStruct->MCPWM_WorkModeCH4 << 7 ) \
				| (MCPWM_InitStruct->CH5N_Polarity_INV << 8 ) | (MCPWM_InitStruct->CH5P_Polarity_INV << 9 ) \
				| (MCPWM_InitStruct->CH5_NS            << 10) | (MCPWM_InitStruct->CH5_PS            << 11) \
				| (MCPWM_InitStruct->CH5N_SCTRLN       << 12) | (MCPWM_InitStruct->CH5P_SCTRLP       << 13) \
				| (MCPWM_InitStruct->Switch_CH5N_CH5P  << 14) | (MCPWM_InitStruct->MCPWM_WorkModeCH5 << 15)  ;

	MCPWM0_TH30 = MCPWM_InitStruct->TH30;
	MCPWM0_TH31 = MCPWM_InitStruct->TH31;
	MCPWM0_TH40 = MCPWM_InitStruct->TH40;
	MCPWM0_TH41 = MCPWM_InitStruct->TH41;
	MCPWM0_TH50 = MCPWM_InitStruct->TH50;
	MCPWM0_TH51 = MCPWM_InitStruct->TH51;
		
	reg_value6	&= ~(1 << 6) & ~(1 << 7) & ~(1 << 8) & ~(1 << 9) & ~(1 << 10) & ~(1 << 11); // 创建掩码		
	reg_value6  |=(MCPWM_InitStruct->CH3N_default_output << 6 )  | (MCPWM_InitStruct->CH3P_default_output << 7 ) \
				 | (MCPWM_InitStruct->CH4N_default_output << 8 )  | (MCPWM_InitStruct->CH4P_default_output << 9 ) \
				 | (MCPWM_InitStruct->CH5N_default_output << 10)  | (MCPWM_InitStruct->CH5P_default_output << 11) ;
    MCPWM0_CH_DEF = reg_value6;

	reg_value7 &= ~(1 << 6) & ~(1 << 7) & ~(1 << 8) & ~(1 << 9) & ~(1 << 10) & ~(1 << 11); // 创建掩码		
	reg_value7 |=(MCPWM_InitStruct->CH3N_FAIL_EN << 6 ) | (MCPWM_InitStruct->CH3P_FAIL_EN << 7 )  \
				 | (MCPWM_InitStruct->CH4N_FAIL_EN << 8 ) | (MCPWM_InitStruct->CH4P_FAIL_EN << 9 )  \
				 | (MCPWM_InitStruct->CH5N_FAIL_EN << 10) | (MCPWM_InitStruct->CH5P_FAIL_EN << 11)  ;
	MCPWM0_CH_MASK = reg_value7;

	/**********************************************************/

	MCPWM0_FAIL345 |= BIT6;			//jack add   	//MOE 控制 MCPWM CH3/CH4/CH5 通道 P 和 N 输出值。1:输出 MCPWM 产生的正常信号
	
	MCPWM0_UPDATE = 0xffff0000;    /* write whatever value to trigger update */  //jack update
	
	MCPWM0_TCLK |= BIT7; 			//jack add  	//BASE_CNT1_EN MCPWM 时基 1 计数器使能开关。 1:使能； 0:关闭。
	
	//MCPWM0_AUEN = MCPWM_InitStruct->AUEN;    // jack add
	MCPWM0_AUEN = (MCPWM0_AUEN & 0x0000FFFF) | (0x07FF0000 & 0xFFFF0000);
	/**********************************************************/

	MCPWM0_PRT = 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */
}


void MCPWM_CH012_IESet(MCPWM_InitTypeDef* MCPWM_InitStruct)
{
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */

	MCPWM0_IE0 =  MCPWM_InitStruct->T0_Update0_INT_EN         | (MCPWM_InitStruct->T1_Update0_INT_EN  << 1 )   \
				|(MCPWM_InitStruct->TH00_Match0_INT_EN << 2 ) | (MCPWM_InitStruct->TH01_Match0_INT_EN << 3 )   \
				|(MCPWM_InitStruct->TH10_Match0_INT_EN << 4 ) | (MCPWM_InitStruct->TH11_Match0_INT_EN << 5 )   \
				|(MCPWM_InitStruct->TH20_Match0_INT_EN << 6 ) | (MCPWM_InitStruct->TH21_Match0_INT_EN << 7 )   \
				|(MCPWM_InitStruct->TMR0_Match0_INT_EN << 10) | (MCPWM_InitStruct->TMR1_Match0_INT_EN << 11)   \
				|(MCPWM_InitStruct->TMR2_Match0_INT_EN << 12) | (MCPWM_InitStruct->TMR3_Match0_INT_EN << 13)   \
				|(MCPWM_InitStruct->Update0_INT_EN << 14) ;
	MCPWM0_PRT = 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */
}

void MCPWM_CH345_IESet(MCPWM_InitTypeDef* MCPWM_InitStruct)
{
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */								
	MCPWM0_IE1 =  MCPWM_InitStruct->T0_Update1_INT_EN         | (MCPWM_InitStruct->T1_Update1_INT_EN  << 1 )   \
				|(MCPWM_InitStruct->TH00_Match1_INT_EN << 2 ) | (MCPWM_InitStruct->TH01_Match1_INT_EN << 3 )   \
				|(MCPWM_InitStruct->TH10_Match1_INT_EN << 4 ) | (MCPWM_InitStruct->TH11_Match1_INT_EN << 5 )   \
				|(MCPWM_InitStruct->TH20_Match1_INT_EN << 6 ) | (MCPWM_InitStruct->TH21_Match1_INT_EN << 7 )   \
				|(MCPWM_InitStruct->TMR0_Match1_INT_EN << 10) | (MCPWM_InitStruct->TMR1_Match1_INT_EN << 11)   \
				|(MCPWM_InitStruct->TMR2_Match1_INT_EN << 12) | (MCPWM_InitStruct->TMR3_Match1_INT_EN << 13)   \
				|(MCPWM_InitStruct->Update1_INT_EN << 14) ;
	MCPWM0_PRT = 0x0000CAFE;  /* write any value other than 0xDEAD to enable write protection */
}



mcpwm0_int_source_e mcpwm0_drv_status_flag_get()
{
	volatile uint32_t interrupt_flag = ((MCPWM0_IF0 & MCPWM0_IE0) | ((MCPWM0_EIE& MCPWM0_EIF)<<16));;

	// 方法1: 从低位到高位查找第一个为1的位
	// int find_first_set_bit_low_to_high(uint16_t value)

	for (int i = 0U; i < 27U; i++)
	{
		if (interrupt_flag & (1 << i))
		{
			return (mcpwm0_int_source_e)(i); // 返回位位置（0-15）
		}
	}
	return MCPWM0_BIT26_Fault; // 没有找到为1的位
}


mcpwm1_int_source_e mcpwm1_drv_status_flag_get()
{
	volatile uint32_t interrupt_flag = ((MCPWM0_IF1 & MCPWM0_IE1) | ((MCPWM0_EIE& MCPWM0_EIF)<<16));

	// 方法1: 从低位到高位查找第一个为1的位
	// int find_first_set_bit_low_to_high(uint16_t value)

	for (int i = 0U; i < 27U; i++)
	{
		if (interrupt_flag & (1 << i))
		{
			return (mcpwm1_int_source_e)(i); // 返回位位置（0-15）
		}
	}
	return MCPWM1_BIT26_Fault; // 没有找到为1的位
}

/*****************************************************************************
 ** \brief	ADC_ChEnableThreshDet
 **			使能通道结果检测
 ** \param [in] 
			TMRx: MCPWM_TIG_TMR0 
						MCPWM_TIG_TMR1
						MCPWM_TIG_TMR2
						MCPWM_TIG_TMR3
			TrigCnt: -TH~TH

 ** \return  none
 ** \note	
*****************************************************************************/
void MCPWM_SetTrigCnt(uint32_t TMRx,int16_t TrigCnt)
{
	MCPWM0_PRT = 0x0000DEAD;
	
	TMRx &= 0x3;
	*(int32_t *)(&(MCPWM0->TMR0) +  TMRx) = TrigCnt;
	
	MCPWM0_PRT		= 0x0000CAFE; 
}


	
	
	

