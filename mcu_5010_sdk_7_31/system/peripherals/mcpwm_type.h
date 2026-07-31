#ifndef MCPWM_TYPE_H
#define MCPWM_TYPE_H

//#include "stdint.h"


#define CURRENT_SAMPLE_1SHUNT          1                  /* ?????????????? */
#define CURRENT_SAMPLE_2SHUNT          2                  /* ?????????????? */
#define CURRENT_SAMPLE_3SHUNT          3                  /* ?????????????? */
#define CURRENT_SAMPLE_MOSFET          4                  /* ????????????MOS?????? */

#define P_HIGH__N_HIGH                       0                 /* 预驱预动极性设置 上管高电平有效，下管高电平有效 */
#define P_HIGH__N_LOW                        1                 /* 预驱预动极性设置 上管高电平有效，下管低电平有效 */


//typedef enum {DISABLE = 0, ENABLE = 1} FuncState;


typedef enum{
	E_PHASE_A_B = 0,
	E_PHASE_B_C = 1,
	E_PHASE_A_C = 2,
	E_PHASE_UNUSED
}ENU_ADChannelCFG;			


typedef struct {
	uint16_t  nuTimePhAN;
	uint16_t  nuTimePhA;
	uint16_t  nuTimePhBN;
	uint16_t  nuTimePhB;
	uint16_t  nuTimePhC;
	uint16_t  nuTimePhCN;
}STR_McpwmCmpr,*PSTR_McpwmCmpr;		//	MCPWM 比较值


typedef struct {
	uint16_t  			nuPoint0;	//采样点0
	uint16_t  			nuPoint1;	//采样点1
	ENU_ADChannelCFG	eChannel;	//通道配置
}STR_McpwmTmr,*PSTR_McpwmTmr;		//	AD采样点及通道


typedef struct {	
	STR_McpwmTmr		mMcTmr;		//采样点配置
	STR_McpwmCmpr		mMcPwm;		//PWM宽度
}STR_PubDatMDHD;	

typedef struct {	
	uint16_t TH0;           /**< MCPWM时基0计数器门限值（PWM周期）*/
    uint16_t TH1;           /**< MCPWM时基1计数器门限值（PWM周期）*/
    
	uint16_t TH00;           /**< MCPWM时基0 CH0P比较门限值（PWM占空比）*/
    uint16_t TH01;           /**< MCPWM时基0 CH0N比较门限值（PWM占空比）*/
	uint16_t TH10;           /**< MCPWM时基0 CH1P比较门限值（PWM占空比）*/
    uint16_t TH11;           /**< MCPWM时基0 CH1N比较门限值（PWM占空比）*/
	uint16_t TH20;           /**< MCPWM时基0 CH2P比较门限值（PWM占空比）*/
    uint16_t TH21;           /**< MCPWM时基0 CH2N比较门限值（PWM占空比）*/
	
	
	uint16_t TH30;           /**< MCPWM时基1 CH3P比较门限值（PWM占空比）*/
    uint16_t TH31;           /**< MCPWM时基1 CH3N比较门限值（PWM占空比）*/
	uint16_t TH40;           /**< MCPWM时基1 CH4P比较门限值（PWM占空比）*/
    uint16_t TH41;           /**< MCPWM时基1 CH4N比较门限值（PWM占空比）*/
	uint16_t TH50;           /**< MCPWM时基1 CH5P比较门限值（PWM占空比）*/
    uint16_t TH51;           /**< MCPWM时基1 CH5N比较门限值（PWM占空比）*/
	
	uint16_t DeadTimeCH012N;           /**< CH0N、CH1N、Ch2N死区时间设置　*/
    uint16_t DeadTimeCH012P;           /**< CH0P、CH1P、Ch2P死区时间设置*/
    uint16_t DeadTimeCH345N;           /**< CH3N、CH4N、Ch5N死区时间设置*/
    uint16_t DeadTimeCH345P;           /**< CH3P、CH4P、Ch5P死区时间设置*/
	
}mcpwm_shadow;	


typedef struct
{
	  uint16_t TH0;           /**< MCPWM时基0计数器门限值（PWM周期）*/
    uint16_t TH1;           /**< MCPWM时基1计数器门限值（PWM周期）*/
    
	  uint16_t TH00;           /**< MCPWM时基0 CH0P比较门限值（PWM占空比）*/
    uint16_t TH01;           /**< MCPWM时基0 CH0N比较门限值（PWM占空比）*/
	  uint16_t TH10;           /**< MCPWM时基0 CH1P比较门限值（PWM占空比）*/
    uint16_t TH11;           /**< MCPWM时基0 CH1N比较门限值（PWM占空比）*/
	  uint16_t TH20;           /**< MCPWM时基0 CH2P比较门限值（PWM占空比）*/
    uint16_t TH21;           /**< MCPWM时基0 CH2N比较门限值（PWM占空比）*/
	
	  uint16_t TH30;           /**< MCPWM时基1 CH3P比较门限值（PWM占空比）*/
    uint16_t TH31;           /**< MCPWM时基1 CH3N比较门限值（PWM占空比）*/
	  uint16_t TH40;           /**< MCPWM时基1 CH4P比较门限值（PWM占空比）*/
    uint16_t TH41;           /**< MCPWM时基1 CH4N比较门限值（PWM占空比）*/
	  uint16_t TH50;           /**< MCPWM时基1 CH5P比较门限值（PWM占空比）*/
    uint16_t TH51;           /**< MCPWM时基1 CH5N比较门限值（PWM占空比）*/
	
	  uint8_t CMP_CTRL_CNT1;           /**< 比较器控制计数器1使能 */
	  uint8_t CMP_CTRL_CNT0;           /**< 比较器控制计数器0使能 */
	
	  uint8_t BASE_CNT1_EN;            /**< MCPWM 计数器1使能开关 */
    uint8_t BASE_CNT0_EN;            /**< MCPWM 计数器0使能开关 */
	  
	  uint8_t EVT_CNT1_EN;             /**<*< MCPWM 计数器1外部触发使能 */
	  uint8_t EVT_CNT0_EN;             /**< MCPWM 计数器0外部触发使能 */
	
	  uint8_t TMR3_TB;                 /**< MCPWM TMR3时基（计数器）选择 0：时基0；1：时基1 */
	  uint8_t TMR2_TB;                 /**< MCPWM TMR2时基（计数器）选择 0：时基0；1：时基1 */
	
	  uint8_t ZCS_EN;                  /**< 使能ZCS事件检测 高有效 */
	
	  uint8_t MCLK_EN;                 /**< MCPWM 时钟使能开关 */
	  uint8_t CLK_DIV;                 /**< MCPWM 分频系数 */ 
	
    uint16_t MCPWM_WorkModeCH0;      /**< MCPWM CH0工作模式：边沿对齐/中心对齐 */
    uint16_t MCPWM_WorkModeCH1;      /**< MCPWM CH1工作模式：边沿对齐/中心对齐 */
    uint16_t MCPWM_WorkModeCH2;      /**< MCPWM CH2工作模式：边沿对齐/中心对齐 */
    uint16_t MCPWM_WorkModeCH3;      /**< MCPWM CH3工作模式：边沿对齐/中心对齐 */	
	  uint16_t MCPWM_WorkModeCH4;      /**< MCPWM CH4工作模式：边沿对齐/中心对齐 */
    uint16_t MCPWM_WorkModeCH5;      /**< MCPWM CH5工作模式：边沿对齐/中心对齐 */	
 
    uint16_t TMR0;          /**< PWM触发ADC事件0，时间点设置 */
    uint16_t TMR1;          /**< PWM触发ADC事件1，时间点设置 */
    uint16_t TMR2;          /**< PWM触发ADC事件2，时间点设置 */
    uint16_t TMR3;          /**< PWM触发ADC事件3，时间点设置 */
 
    uint16_t DeadTimeCH012N;           /**< CH0N、CH1N、Ch2N死区时间设置　*/
    uint16_t DeadTimeCH012P;           /**< CH0P、CH1P、Ch2P死区时间设置*/
    uint16_t DeadTimeCH345N;           /**< CH3N、CH4N、Ch5N死区时间设置*/
    uint16_t DeadTimeCH345P;           /**< CH3P、CH4P、Ch5P死区时间设置*/

    uint16_t EVT0;                   /**< 外部事件触发MCPWM_CNT0 (时基0)从0开始使能 */
		uint16_t EVT1;                   /**< 外部事件触发MCPWM_CNT1 (时基1)从0开始使能 */
		
    uint8_t CH0N_Polarity_INV;       /**< CH0N输出极性取反，0:正常输出；1:取反输出 */
    uint8_t CH0P_Polarity_INV;       /**< CH0P输出极性取反，0:正常输出；1:取反输出 */
    uint8_t CH1N_Polarity_INV;       /**< CH1N输出极性取反，0:正常输出；1:取反输出 */
    uint8_t CH1P_Polarity_INV;       /**< CH1P输出极性取反，0:正常输出；1:取反输出 */
    uint8_t CH2N_Polarity_INV;       /**< CH2N输出极性取反，0:正常输出；1:取反输出 */
    uint8_t CH2P_Polarity_INV;       /**< CH2P输出极性取反，0:正常输出；1:取反输出 */
    uint8_t CH3N_Polarity_INV;       /**< CH3N输出极性取反，0:正常输出；1:取反输出 */
    uint8_t CH3P_Polarity_INV;       /**< CH3P输出极性取反，0:正常输出；1:取反输出 */	
		uint8_t CH4N_Polarity_INV;       /**< CH4N输出极性取反，0:正常输出；1:取反输出 */
    uint8_t CH4P_Polarity_INV;       /**< CH4P输出极性取反，0:正常输出；1:取反输出 */
    uint8_t CH5N_Polarity_INV;       /**< CH5N输出极性取反，0:正常输出；1:取反输出 */
    uint8_t CH5P_Polarity_INV;       /**< CH5P输出极性取反，0:正常输出；1:取反输出 */	
   
    uint8_t CH0P_SCTRLP;             /**< 当CH0_PS = 1时，输出到CH0_P通道的值*/
    uint8_t CH0N_SCTRLN;             /**< 当CH0_NS = 1时，输出到CH0_N通道的值*/
    uint8_t CH1P_SCTRLP;             /**< 当CH1_PS = 1时，输出到CH1_P通道的值*/
    uint8_t CH1N_SCTRLN;             /**< 当CH1_NS = 1时，输出到CH1_N通道的值*/
    uint8_t CH2P_SCTRLP;             /**< 当CH2_PS = 1时，输出到CH2_P通道的值*/
    uint8_t CH2N_SCTRLN;             /**< 当CH2_NS = 1时，输出到CH2_N通道的值*/
    uint8_t CH3P_SCTRLP;             /**< 当CH3_PS = 1时，输出到CH3_P通道的值*/
    uint8_t CH3N_SCTRLN;             /**< 当CH3_NS = 1时，输出到CH3_N通道的值*/
		uint8_t CH4P_SCTRLP;             /**< 当CH4_PS = 1时，输出到CH4_P通道的值*/
    uint8_t CH4N_SCTRLN;             /**< 当CH4_NS = 1时，输出到CH4_N通道的值*/
    uint8_t CH5P_SCTRLP;             /**< 当CH5_PS = 1时，输出到CH5_P通道的值*/
    uint8_t CH5N_SCTRLN;             /**< 当CH5_NS = 1时，输出到CH5_N通道的值*/
   
	  uint8_t CH0_PS;                  /**< CH0_P信号来源，1：来自CH0_SCTRLP；0：来自MCPWM实际运行计数器*/
		uint8_t CH0_NS;                  /**< CH0_N信号来源，1：来自CH0_SCTRLN；0：来自MCPWM实际运行计数器*/
		uint8_t CH1_PS;                  /**< CH1_P信号来源，1：来自CH1_SCTRLP；0：来自MCPWM实际运行计数器*/
		uint8_t CH1_NS;                  /**< CH1_N信号来源，1：来自CH1_SCTRLN；0：来自MCPWM实际运行计数器*/
		uint8_t CH2_PS;                  /**< CH2_P信号来源，1：来自CH2_SCTRLP；0：来自MCPWM实际运行计数器*/
		uint8_t CH2_NS;                  /**< CH2_N信号来源，1：来自CH2_SCTRLN；0：来自MCPWM实际运行计数器*/
		uint8_t CH3_PS;                  /**< CH3_P信号来源，1：来自CH3_SCTRLP；0：来自MCPWM实际运行计数器*/
		uint8_t CH3_NS;                  /**< CH3_N信号来源，1：来自CH3_SCTRLN；0：来自MCPWM实际运行计数器*/
		uint8_t CH4_PS;                  /**< CH4_P信号来源，1：来自CH4_SCTRLP；0：来自MCPWM实际运行计数器*/
		uint8_t CH4_NS;                  /**< CH4_N信号来源，1：来自CH4_SCTRLN；0：来自MCPWM实际运行计数器*/
		uint8_t CH5_PS;                  /**< CH5_P信号来源，1：来自CH5_SCTRLP；0：来自MCPWM实际运行计数器*/
		uint8_t CH5_NS;                  /**< CH5_N信号来源，1：来自CH5_SCTRLN；0：来自MCPWM实际运行计数器*/
		
    uint8_t Switch_CH0N_CH0P;        /**< 交换CH0N, CH0P信号输出使能开关 */
    uint8_t Switch_CH1N_CH1P;        /**< 交换CH1N, CH1P信号输出使能开关 */
    uint8_t Switch_CH2N_CH2P;        /**< 交换CH2N, CH2P信号输出使能开关 */
    uint8_t Switch_CH3N_CH3P;        /**< 交换CH3N, CH3P信号输出使能开关 */	
		uint8_t Switch_CH4N_CH4P;        /**< 交换CH2N, CH2P信号输出使能开关 */
    uint8_t Switch_CH5N_CH5P;        /**< 交换CH3N, CH3P信号输出使能开关 */	
																			
    uint8_t TR0_UP_INTV;             /**< MCPWM_CNT0 更新间隔 T0/T1事件次数 */
    uint8_t TR0_T0_UpdateEN;         /**< MCPWM_CNT0 T0事件更新使能 */
    uint8_t TR0_T1_UpdateEN;         /**< MCPWM_CNT0 T1事件更新使能 */
    uint8_t TR0_AEC;                 /**< MCPWM 更新事件是否自动清除Fail0和Fail1事件（恢复CH0CH1CH2输出）  1：使能 */
																		
		uint8_t TR1_UP_INTV;             /**< MCPWM_CNT1 更新间隔 T0/T1事件次数 */
    uint8_t TR1_T0_UpdateEN;         /**< MCPWM_CNT1 T0事件更新使能 */
    uint8_t TR1_T1_UpdateEN;         /**< MCPWM_CNT1 T1事件更新使能 */
    uint8_t TR1_AEC;                 /**< MCPWM 更新事件是否自动清除Fail2和Fail3事件（恢复CH3CH4CH5输出）  1：使能 */
																			 
    uint8_t FAIL0_INPUT_EN;          /**< FAIL0 输入功能使能 */
    uint8_t FAIL1_INPUT_EN;          /**< FAIL1 输入功能使能 */	 
		uint8_t FAIL_0CAP;               /**< FAIL01事件发生时 将MCPWM0_CNT0值存入MCPWM0_FCNT 使能 */	
    uint8_t FAIL0_Signal_Sel;        /**< FAIL0 信号选择，比较器0或GPIO */
    uint8_t FAIL1_Signal_Sel;        /**< FAIL1 信号选择，比较器0或GPIO */	 
    uint8_t FAIL0_Polarity;          /**< FAIL0 信号极性设置，高有效或低有效 */	 
    uint8_t FAIL1_Polarity;          /**< FAIL1 信号极性设置，高有效或低有效 */
    uint8_t HALT_PRT0;               /**<  MCU进入HALT状态, MCPWM(CH0CH1CH2)信号是否正常输出 1正常输出；0强制输出保护值 */
		
		uint8_t FAIL2_INPUT_EN;          /**< FAIL2 输入功能使能 */
    uint8_t FAIL3_INPUT_EN;          /**< FAIL3 输入功能使能 */	 
		uint8_t FAIL_1CAP;               /**< FAIL23事件发生时 将MCPWM0_CNT1值存入MCPWM0_FCNT */	
    uint8_t FAIL2_Signal_Sel;        /**< FAIL2 信号选择，比较器0或GPIO或CLUOUT0或CLUOUT1 */
    uint8_t FAIL3_Signal_Sel;        /**< FAIL3 信号选择，比较器1或GPIO或CLUOUT2或CLUOUT3 */ 
    uint8_t FAIL2_Polarity;          /**< FAIL2 信号极性设置，高有效或低有效 */	 
    uint8_t FAIL3_Polarity;          /**< FAIL3 信号极性设置，高有效或低有效 */
    uint8_t HALT_PRT1;               /**< MCU进入HALT状态, MCPWM(CH3CH4CH5)信号是否正常输出 1正常输出；0强制输出保护值 */

    uint8_t CH0P_default_output;     /**< CH0P MOE为0时或发生FAIL事件时，默认电平输出 */
    uint8_t CH0N_default_output;     /**< CH0N MOE为0时或发生FAIL事件时，默认电平输出 */
    uint8_t CH1P_default_output;     /**< CH1P MOE为0时或发生FAIL事件时，默认电平输出 */
    uint8_t CH1N_default_output;     /**< CH1N MOE为0时或发生FAIL事件时，默认电平输出 */
    uint8_t CH2P_default_output;     /**< CH2P MOE为0时或发生FAIL事件时，默认电平输出 */
    uint8_t CH2N_default_output;     /**< CH2N MOE为0时或发生FAIL事件时，默认电平输出 */
    uint8_t CH3P_default_output;     /**< CH3P MOE为0时或发生FAIL事件时，默认电平输出 */
    uint8_t CH3N_default_output;     /**< CH3N MOE为0时或发生FAIL事件时，默认电平输出 */	
		uint8_t CH4P_default_output;     /**< CH4P MOE为0时或发生FAIL事件时，默认电平输出 */
    uint8_t CH4N_default_output;     /**< CH4N MOE为0时或发生FAIL事件时，默认电平输出 */
    uint8_t CH5P_default_output;     /**< CH5P MOE为0时或发生FAIL事件时，默认电平输出 */
    uint8_t CH5N_default_output;     /**< CH5N MOE为0时或发生FAIL事件时，默认电平输出 */

    uint8_t CH0N_FAIL_EN;     /**< CH0N通道FAIL事件使能 1：MOE为0时或发生FAIL事件时，输出默认电平；0：输出不受影响 */
    uint8_t CH0P_FAIL_EN;     /**< CH0P通道FAIL事件使能 1：MOE为0时或发生FAIL事件时，输出默认电平；0：输出不受影响 */
    uint8_t CH1N_FAIL_EN;     /**< CH1N通道FAIL事件使能 1：MOE为0时或发生FAIL事件时，输出默认电平；0：输出不受影响 */
    uint8_t CH1P_FAIL_EN;     /**< CH1P通道FAIL事件使能 1：MOE为0时或发生FAIL事件时，输出默认电平；0：输出不受影响 */
    uint8_t CH2N_FAIL_EN;     /**< CH2N通道FAIL事件使能 1：MOE为0时或发生FAIL事件时，输出默认电平；0：输出不受影响 */
    uint8_t CH2P_FAIL_EN;     /**< CH2P通道FAIL事件使能 1：MOE为0时或发生FAIL事件时，输出默认电平；0：输出不受影响 */
    uint8_t CH3N_FAIL_EN;     /**< CH3N通道FAIL事件使能 1：MOE为0时或发生FAIL事件时，输出默认电平；0：输出不受影响 */
    uint8_t CH3P_FAIL_EN;     /**< CH3P通道FAIL事件使能 1：MOE为0时或发生FAIL事件时，输出默认电平；0：输出不受影响 */	
	uint8_t CH4N_FAIL_EN;     /**< CH4N通道FAIL事件使能 1：MOE为0时或发生FAIL事件时，输出默认电平；0：输出不受影响 */
    uint8_t CH4P_FAIL_EN;     /**< CH4P通道FAIL事件使能 1：MOE为0时或发生FAIL事件时，输出默认电平；0：输出不受影响 */
    uint8_t CH5N_FAIL_EN;     /**< CH5N通道FAIL事件使能 1：MOE为0时或发生FAIL事件时，输出默认电平；0：输出不受影响 */
    uint8_t CH5P_FAIL_EN;     /**< CH5P通道FAIL事件使能 1：MOE为0时或发生FAIL事件时，输出默认电平；0：输出不受影响 */
		
    uint8_t T0_Update0_INT_EN;        /**< MCPWM0_CNT0 计数到-MCPWMTH (T0更新事件)中断使能 */
    uint8_t T1_Update0_INT_EN;        /**< MCPWM0_CNT0 计数到0        (T1更新事件)中断使能 */ 
		
    uint8_t TMR0_Match0_INT_EN;       /**< MCPWM0_CNT0 计数到 MCPWM0_TMR0 产生的计数事件匹配事件中断使能 */    
    uint8_t TMR1_Match0_INT_EN;       /**< MCPWM0_CNT0 计数到 MCPWM0_TMR1 产生的计数事件匹配事件中断使能 */
    uint8_t TMR2_Match0_INT_EN;       /**< MCPWM0_CNT0 计数到 MCPWM0_TMR2 产生的计数事件匹配事件中断使能 */    
    uint8_t TMR3_Match0_INT_EN;       /**< MCPWM0_CNT0 计数到 MCPWM0_TMR3 产生的计数事件匹配事件中断使能 */   
																			 
    uint8_t TH00_Match0_INT_EN;       /**< MCPWM0_CNT0 计数到 MCPWM0_TH00 产生的计数事件匹配事件中断使能 */    
    uint8_t TH01_Match0_INT_EN;       /**< MCPWM0_CNT0 计数到 MCPWM0_TH01 产生的计数事件匹配事件中断使能 */
    uint8_t TH10_Match0_INT_EN;       /**< MCPWM0_CNT0 计数到 MCPWM0_TH10 产生的计数事件匹配事件中断使能 */    
    uint8_t TH11_Match0_INT_EN;       /**< MCPWM0_CNT0 计数到 MCPWM0_TH11 产生的计数事件匹配事件中断使能 */ 
	uint8_t TH20_Match0_INT_EN;       /**< MCPWM0_CNT0 计数到 MCPWM0_TH20 产生的计数事件匹配事件中断使能 */    
    uint8_t TH21_Match0_INT_EN;       /**< MCPWM0_CNT0 计数到 MCPWM0_TH21 产生的计数事件匹配事件中断使能 */ 
																				
	uint8_t Update0_INT_EN;           /**< MCPWM0_CNT0 计数器0更新中断使能 */
																			
	uint8_t T0_Update1_INT_EN;        /**< MCPWM0_CNT0 计数到-MCPWMTH (T0更新事件)中断使能 */
    uint8_t T1_Update1_INT_EN;        /**< MCPWM0_CNT0 计数到0        (T1更新事件)中断使能 */ 
																				
    uint8_t TMR0_Match1_INT_EN;       /**< MCPWM0_CNT0 计数到 MCPWM0_TMR0 产生的计数事件匹配事件中断使能 */    
    uint8_t TMR1_Match1_INT_EN;       /**< MCPWM0_CNT0 计数到 MCPWM0_TMR1 产生的计数事件匹配事件中断使能 */
    uint8_t TMR2_Match1_INT_EN;       /**< MCPWM0_CNT0 计数到 MCPWM0_TMR2 产生的计数事件匹配事件中断使能 */    
    uint8_t TMR3_Match1_INT_EN;       /**< MCPWM0_CNT0 计数到 MCPWM0_TMR3 产生的计数事件匹配事件中断使能 */   
																				
    uint8_t TH00_Match1_INT_EN;       /**< MCPWM0_CNT0 计数到 MCPWM0_TH00 产生的计数事件匹配事件中断使能 */    
    uint8_t TH01_Match1_INT_EN;       /**< MCPWM0_CNT0 计数到 MCPWM0_TH01 产生的计数事件匹配事件中断使能 */
    uint8_t TH10_Match1_INT_EN;       /**< MCPWM0_CNT0 计数到 MCPWM0_TH10 产生的计数事件匹配事件中断使能 */    
    uint8_t TH11_Match1_INT_EN;       /**< MCPWM0_CNT0 计数到 MCPWM0_TH11 产生的计数事件匹配事件中断使能 */ 
		uint8_t TH20_Match1_INT_EN;       /**< MCPWM0_CNT0 计数到 MCPWM0_TH20 产生的计数事件匹配事件中断使能 */    
    uint8_t TH21_Match1_INT_EN;       /**< MCPWM0_CNT0 计数到 MCPWM0_TH21 产生的计数事件匹配事件中断使能 */ 
																				
		uint8_t Update1_INT_EN;           /**< MCPWM0_CNT0 计数器0更新中断使能 */
		
    uint8_t FAIL0_INT_EN;            /* FAIL0事件中断使能 */
    uint8_t FAIL1_INT_EN;            /* FAIL1事件中断使能 */	
		uint8_t FAIL2_INT_EN;            /* FAIL0事件中断使能 */
    uint8_t FAIL3_INT_EN;            /* FAIL1事件中断使能 */
		
    uint8_t IO_CMP_FLT_CLKDIV;       /**<  GPIO或CMP信号输入滤波时钟设置1-16 */
		
    uint8_t ZCS0_INT_EN;            /**<  ZCS0事件中断使能 数字电源应用 */
    uint8_t ZCS1_INT_EN;            /**<  ZCS1事件中断使能 数字电源应用 */
		
    uint8_t IO0_PPE;                 /**<  IO0推挽模式使能：1使能，0关闭 */
		uint8_t IO1_PPE;                 /**<  IO1推挽模式使能：1使能，0关闭 */
		uint8_t IO2_PPE;                 /**<  IO2推挽模式使能：1使能，0关闭 */
		uint8_t IO3_PPE;                 /**<  IO3推挽模式使能：1使能，0关闭 */
		uint8_t IO4_PPE;                 /**<  IO4推挽模式使能：1使能，0关闭 */
		uint8_t IO5_PPE;                 /**<  IO5推挽模式使能：1使能，0关闭 */
		
		uint8_t TR1_T1_RE1;              /**< MCPWM_CNT1  计数到0 (T1)       事件 DMA请求1 使能：1使能，0关闭 */
		uint8_t TR1_T0_RE1;              /**< MCPWM_CNT1  计数到-MCPWM_TH(T0)事件 DMA请求1 使能：1使能，0关闭 */
		uint8_t TR0_T1_RE1;              /**< MCPWM_CNT0  计数到0 (T1)       事件 DMA请求1 使能：1使能，0关闭 */
		uint8_t TR0_T0_RE1;              /**< MCPWM_CNT0  计数到-MCPWM_TH(T0)事件 DMA请求1 使能：1使能，0关闭 */
		uint8_t TMR3_RE1;                /**< MCPWM_CNT01 计数到MCPWM_TMR3   事件 DMA请求1 使能：1使能，0关闭 */
		uint8_t TMR2_RE1;                /**< MCPWM_CNT01 计数到MCPWM_TMR2   事件 DMA请求1 使能：1使能，0关闭 */
		uint8_t TMR1_RE1;                /**< MCPWM_CNT01 计数到MCPWM_TMR1   事件 DMA请求1 使能：1使能，0关闭 */
		uint8_t TMR0_RE1;                /**< MCPWM_CNT01 计数到MCPWM_TMR0   事件 DMA请求1 使能：1使能，0关闭 */
																			 
		uint8_t TR1_T1_RE0;              /**< MCPWM_CNT1  计数到0 (T1)       事件 DMA请求0 使能：1使能，0关闭 */
		uint8_t TR1_T0_RE0;              /**< MCPWM_CNT1  计数到-MCPWM_TH(T0)事件 DMA请求0 使能：1使能，0关闭 */
		uint8_t TR0_T1_RE0;              /**< MCPWM_CNT0  计数到0 (T1)       事件 DMA请求0 使能：1使能，0关闭 */
		uint8_t TR0_T0_RE0;              /**< MCPWM_CNT0  计数到-MCPWM_TH(T0)事件 DMA请求0 使能：1使能，0关闭 */
		uint8_t TMR3_RE0;                /**< MCPWM_CNT01 计数到MCPWM_TMR3   事件 DMA请求0 使能：1使能，0关闭 */
		uint8_t TMR2_RE0;                /**< MCPWM_CNT01 计数到MCPWM_TMR2   事件 DMA请求0 使能：1使能，0关闭 */
		uint8_t TMR1_RE0;                /**< MCPWM_CNT01 计数到MCPWM_TMR1   事件 DMA请求0 使能：1使能，0关闭 */
		uint8_t TMR0_RE0;                /**< MCPWM_CNT01 计数到MCPWM_TMR0   事件 DMA请求0 使能：1使能，0关闭 */

    uint16_t STT_HYST;               /**<  状态切换延时，12位无符号数 */
    uint16_t ZCS_DELAY;              /**<  ZCS状态停留时间，15位无符号数 数字电源应用 */
		
		uint32_t AUEN;                   /**<  自动更新使能 */
}MCPWM_InitTypeDef;

/* MCPWM输出 对齐方式 */
#define MCPWM0_CENTRAL_PWM_MODE     0  /**<  中心对齐PWM模式 */
#define MCPWM0_EDGE_PWM_MODE        1  /**<  边沿对齐PWM模式 */

#define MCPWM0_HIGH_LEVEL           1  /**< 高电平 */
#define MCPWM0_LOW_LEVEL            0  /**< 高电平 */
																			
#define MCPWM0_HIGH_LEVEL_ACTIVE    0  /**< 高电平有效 */
#define MCPWM0_LOW_LEVEL_ACTIVE     1  /**< 低电平有效 */
																			
#define MCPWM0_FAIL_SEL_CMP         1  /**< Fail事件来源比较器 */
#define MCPWM0_FAIL_SEL_IO          0  /**< Fail事件来源比较器 */

/* MCPWM0_AUEN  MCPWM 自动更新使能寄存器 */
#define MCPWM0_TH00_AUPDATE  BIT0  /**< MCPWM_TH00 自动加载使能 */
#define MCPWM0_TH01_AUPDATE  BIT1  /**< MCPWM_TH01 自动加载使能 */ 
#define MCPWM0_TH10_AUPDATE  BIT2  /**< MCPWM_TH10 自动加载使能 */
#define MCPWM0_TH11_AUPDATE  BIT3  /**< MCPWM_TH11 自动加载使能 */
#define MCPWM0_TH20_AUPDATE  BIT4  /**< MCPWM_TH20 自动加载使能 */
#define MCPWM0_TH21_AUPDATE  BIT5  /**< MCPWM_TH21 自动加载使能 */
#define MCPWM0_TMR0_AUPDATE  BIT8  /**< MCPWM_TMR0 自动加载使能 */
#define MCPWM0_TMR1_AUPDATE  BIT9  /**< MCPWM_TMR1 自动加载使能 */
#define MCPWM0_TH0_AUPDATE   BIT10 /**< MCPWM_TH0 自动加载使能  */
#define MCPWM0_CNT0_AUPDAT   BIT11 /**< MCPWM_CNT0 自动加载使能 */

#define MCPWM0_TH30_AUPDATE  BIT16  /**< MCPWM_TH00 自动加载使能 */
#define MCPWM0_TH31_AUPDATE  BIT17  /**< MCPWM_TH01 自动加载使能 */ 
#define MCPWM0_TH40_AUPDATE  BIT18  /**< MCPWM_TH10 自动加载使能 */
#define MCPWM0_TH41_AUPDATE  BIT19  /**< MCPWM_TH11 自动加载使能 */
#define MCPWM0_TH50_AUPDATE  BIT20  /**< MCPWM_TH20 自动加载使能 */
#define MCPWM0_TH51_AUPDATE  BIT20  /**< MCPWM_TH21 自动加载使能 */
#define MCPWM0_TMR2_AUPDATE  BIT24  /**< MCPWM_TMR0 自动加载使能 */
#define MCPWM0_TMR3_AUPDATE  BIT25  /**< MCPWM_TMR1 自动加载使能 */
#define MCPWM0_TH1_AUPDATE   BIT26  /**< MCPWM_TH0 自动加载使能  */
#define MCPWM0_CNT1_AUPDAT   BIT27  /**< MCPWM_CNT0 自动加载使能 */
#define MCPWM0_ALL_AUPDAT    0xffffffff  /**< 全部自动加载使能 */


#define MCPWM0CNT0_T0_IRQ_IF       BIT0  /**< 计数器0计数值回到-MCPWM_TH中断标志位 */
#define MCPWM0CNT0_T1_IRQ_IF       BIT1  /**< 计数器0计数值回到0中断标志位 */
#define MCPWM0CNT0_TH00_IRQ_IF     BIT2  /**< 计数器0计数值回到MCPWM_TH00中断标志位 */
#define MCPWM0CNT0_TH01_IRQ_IF     BIT3  /**< 计数器0计数值回到MCPWM_TH01中断标志位 */
#define MCPWM0CNT0_TH10_IRQ_IF     BIT4  /**< 计数器0计数值回到MCPWM_TH10中断标志位 */
#define MCPWM0CNT0_TH11_IRQ_IF     BIT5  /**< 计数器0计数值回到MCPWM_TH11中断标志位 */
#define MCPWM0CNT0_TH20_IRQ_IF     BIT6  /**< 计数器0计数值回到MCPWM_TH20中断标志位 */
#define MCPWM0CNT0_TH21_IRQ_IF     BIT7  /**< 计数器0计数值回到MCPWM_TH21中断标志位 */

#define MCPWM0CNT0_TMR0_IRQ_IF     BIT10  /**< 计数器0计数值回到MCPWM_TMR0中断标志位 */
#define MCPWM0CNT0_TMR1_IRQ_IF     BIT11  /**< 计数器0计数值回到MCPWM_TMR1中断标志位 */
#define MCPWM0CNT0_TMR2_IRQ_IF     BIT12  /**< 计数器0计数值回到MCPWM_TMR2中断标志位 */
#define MCPWM0CNT0_TMR3_IRQ_IF     BIT13  /**< 计数器0计数值回到MCPWM_TMR3中断标志位 */
#define MCPWM0CNT0_UP_IRQ_IF       BIT14    /**< 计数器0更新中断标志位 */

#define MCPWM0CNT1_T0_IRQ_IF       BIT0  /**< 计数器0计数值回到-MCPWM_TH中断标志位 */
#define MCPWM0CNT1_T1_IRQ_IF       BIT1  /**< 计数器0计数值回到0中断标志位 */
#define MCPWM0CNT1_TH30_IRQ_IF     BIT2  /**< 计数器0计数值回到MCPWM_TH00中断标志位 */
#define MCPWM0CNT1_TH31_IRQ_IF     BIT3  /**< 计数器0计数值回到MCPWM_TH01中断标志位 */
#define MCPWM0CNT1_TH40_IRQ_IF     BIT4  /**< 计数器0计数值回到MCPWM_TH10中断标志位 */
#define MCPWM0CNT1_TH41_IRQ_IF     BIT5  /**< 计数器0计数值回到MCPWM_TH11中断标志位 */
#define MCPWM0CNT1_TH50_IRQ_IF     BIT6  /**< 计数器0计数值回到MCPWM_TH20中断标志位 */
#define MCPWM0CNT1_TH51_IRQ_IF     BIT7  /**< 计数器0计数值回到MCPWM_TH21中断标志位 */

#define MCPWM0CNT1_TMR2_IRQ_IF     BIT12  /**< 计数器0计数值回到MCPWM_TMR2中断标志位 */
#define MCPWM0CNT1_TMR3_IRQ_IF     BIT13  /**< 计数器0计数值回到MCPWM_TMR3中断标志位 */
#define MCPWM0CNT1_UP_IRQ_IF       BIT14    /**< 计数器0更新中断标志位 */

#define MCPWM0_FAIL0_IRQ_IF         BIT19 /**< MCPWMCH0_P和CH0N同时有效中断标志位 */
#define MCPWM0_FAIL1_IRQ_IF         BIT20 /**< MCPWMCH0_P和CH0N同时有效中断标志位 */
#define MCPWM0_FAIL2_IRQ_IF         BIT21 /**< MCPWMCH0_P和CH0N同时有效中断标志位 */
#define MCPWM0_FAIL3_IRQ_IF         BIT22 /**< MCPWMCH0_P和CH0N同时有效中断标志位 */
#define MCPWM0_ZCS0_IRQ_IF          BIT23 /**< MCPWMCH0_P和CH0N同时有效中断标志位 */
#define MCPWM0_ZCS1_IRQ_IF          BIT24 /**< MCPWMCH0_P和CH0N同时有效中断标志位 */

// MCPWM_IF0 位定义
#define MCPWM_IF0_UP_IF           BIT14  ///< 时基0更新事件MCPWM0_TH0/MCPWM0_TH00~MCPWM0_TH21/MCPWM0_TMR等寄存器更新到MCPWM实际运行系统的中断事件。
#define MCPWM_IF0_TMR3_IF         BIT13  ///< MCPWM0_CNT0等于MCPWM0_TMR3中断事件
#define MCPWM_IF0_TMR2_IF         BIT12  ///< MCPWM0_CNT0等于MCPWM0_TMR2中断事件
#define MCPWM_IF0_TMR1_IF         BIT11  ///< MCPWM0_CNT0等于MCPWM0_TMR1中断事件
#define MCPWM_IF0_TMR0_IF         BIT10  ///< MCPWM0_CNT0等于MCPWM0_TMR0中断事件
#define MCPWM_IF0_TH21_IF         BIT7   ///< MCPWM0_CNT0等于MCPWM0_TH21中断事件
#define MCPWM_IF0_TH20_IF         BIT6   ///< MCPWM0_CNT0等于MCPWM0_TH20中断事件
#define MCPWM_IF0_TH11_IF         BIT5   ///< MCPWM0_CNT0等于MCPWM0_TH11中断事件
#define MCPWM_IF0_TH10_IF         BIT4   ///< MCPWM0_CNT0等于MCPWM0_TH10中断事件
#define MCPWM_IF0_TH01_IF         BIT3   ///< MCPWM0_CNT0等于MCPWM0_TH01中断事件
#define MCPWM_IF0_TH00_IF         BIT2   ///< MCPWM0_CNT0等于MCPWM0_TH00中断事件
#define MCPWM_IF0_T1_IF           BIT1   ///< t1事件，MCPWM0_CNT0等于0中断事件
#define MCPWM_IF0_T0_IF           BIT0   ///< t0事件，MCPWM0_CNT0等于MCPWM0_TH中断事件

// MCPWM_IF1 位定义
#define MCPWM_IF1_UP_IF           BIT14  ///< 时基1更新事件MCPWM0_TH0/MCPWM0_TH00~MCPWM0_TH21/MCPWM0_TMR等寄存器更新到MCPWM实际运行系统的中断事件。
#define MCPWM_IF1_TMR3_IF         BIT13  ///< MCPWM0_CNT1等于MCPWM0_TMR3中断事件
#define MCPWM_IF1_TMR2_IF         BIT12  ///< MCPWM0_CNT1等于MCPWM0_TMR2中断事件
#define MCPWM_IF1_TMR1_IF         BIT11  ///< MCPWM0_CNT1等于MCPWM0_TMR1中断事件
#define MCPWM_IF1_TMR0_IF         BIT10  ///< MCPWM0_CNT1等于MCPWM0_TMR0中断事件
#define MCPWM_IF1_TH51_IF         BIT7   ///< MCPWM0_CNT1等于MCPWM0_TH51中断事件
#define MCPWM_IF1_TH50_IF         BIT6   ///< MCPWM0_CNT1等于MCPWM0_TH50中断事件
#define MCPWM_IF1_TH41_IF         BIT5   ///< MCPWM0_CNT1等于MCPWM0_TH41中断事件
#define MCPWM_IF1_TH40_IF         BIT4   ///< MCPWM0_CNT1等于MCPWM0_TH40中断事件
#define MCPWM_IF1_TH31_IF         BIT3   ///< MCPWM0_CNT1等于MCPWM0_TH31中断事件
#define MCPWM_IF1_TH30_IF         BIT2   ///< MCPWM0_CNT1等于MCPWM0_TH30中断事件
#define MCPWM_IF1_T1_IF           BIT1   ///< t1事件，MCPWM0_CNT1等于0中断事件
#define MCPWM_IF1_T0_IF           BIT0   ///< t0事件，MCPWM0_CNT1等于MCPWM0_TH中断事件

// MCPWM_EIF 位定义
#define MCPWM_EIF_ZCS1            BIT9   ///< 时基1 ZCS中断源事件。
#define MCPWM_EIF_ZCS0            BIT8   ///< 时基0 ZCS中断源事件。
#define MCPWM_EIF_FAIL3           BIT7   ///< FAIL3中断源事件。
#define MCPWM_EIF_FAIL2           BIT6   ///< FAIL2中断源事件。
#define MCPWM_EIF_FAIL1           BIT5   ///< FAIL1中断源事件。
#define MCPWM_EIF_FAIL0           BIT4   ///< FAIL0中断源事件。

#define MCPWM_TIG_TMR0           	(0)   // 采样触发点0
#define MCPWM_TIG_TMR1           	(1)   // 采样触发点1
#define MCPWM_TIG_TMR2           	(2)   // 采样触发点2
#define MCPWM_TIG_TMR3           	(3)   // 采样触发点3

/**< 定义MCPWM定时器序号*/
typedef enum 
{
		MCPWM0CNT0 = 0,     /**< MCPWM0计数器0 */
		MCPWM0CNT1 = 1,     /**< MCPWM0计数器1 */
}MCPWM0CNTx;


/** mcpwm0 interrupt types*/
typedef enum{
    MCPWM0_BIT0_T0_IF = 0U,          /*!< 0, t0 事件， MCPWM0_CNT0 等于 MCPWM0_TH 中断事件。1，发生； 0，没发生。写 1 清零。*/
    MCPWM0_BIT1_T1_IF,               /*!< 1, t1 事件， MCPWM0_CNT0 等于 0 中断事件。 */
    MCPWM0_BIT2_TH00_IF,             /*!< 2, MCPWM0_CNT0 等于 MCPWM0_TH00 中断事件。*/
    MCPWM0_BIT3_TH01_IF,             /*!< 3, MCPWM0_CNT0 等于 MCPWM0_TH01 中断事件。*/
    MCPWM0_BIT4_TH10_IF,             /*!< 4, MCPWM0_CNT0 等于 MCPWM0_TH10 中断事件。*/
    MCPWM0_BIT5_TH11_IF,             /*!< 5, MCPWM0_CNT0 等于 MCPWM0_TH11 中断事件。*/
    MCPWM0_BIT6_TH20_IF,             /*!< 6, MCPWM0_CNT0 等于 MCPWM0_TH20 中断事件。*/
    MCPWM0_BIT7_TH21_IF,             /*!< 7, MCPWM0_CNT0 等于 MCPWM0_TH21 中断事件。 */
    MCPWM0_BIT8_Reserved,            /*!< 8, 未使用 */
    MCPWM0_BIT9_Reserved,            /*!< 9, 未使用 */
    MCPWM0_BIT10_TMR0_IF,            /*!< 10, MCPWM0_CNT0 等于 MCPWM0_TMR0 中断事件。*/
    MCPWM0_BIT11_TMR1_IF,            /*!< 11, MCPWM0_CNT0 等于 MCPWM0_TMR1 中断事件。*/
    MCPWM0_BIT12_TMR2_IF,            /*!< 12, MCPWM0_CNT0 等于 MCPWM0_TMR2 中断事件。*/
    MCPWM0_BIT13_TMR3_IF,            /*!< 13, MCPWM0_CNT0 等于 MCPWM0_TMR3 中断事件。*/
    MCPWM0_BIT14_UP_IF,              /*!< 14, 时基 0 更新事件MCPWM0_TH0/MCPWM0_TH00~MCPWM0_TH21/MCPWM0_TMR 等寄存器更新到 MCPWM 实际运行系统的中断事件。*/
    MCPWM0_BIT15_Reserved,           /*!< 15,未使用 */
    MCPWM0_BIT16_Reserved,           /*!< 16,未使用 */
    MCPWM0_BIT17_Reserved,           /*!< 17,未使用 */
    MCPWM0_BIT18_Reserved,           /*!< 18,未使用 */
    MCPWM0_BIT19_Reserved,           /*!< 19,未使用 */
    MCPWM0_BIT20_FAIL0_IF,           /*!< 20,FAIL0 中断源事件 */
    MCPWM0_BIT21_FAIL1_IF,           /*!< 21,FAIL1 中断源事件 */
    MCPWM0_BIT22_FAIL2_IF,           /*!< 22,FAIL2 中断源事件 */
    MCPWM0_BIT23_FAIL3_IF,           /*!< 23,FAIL3 中断源事件 */
    MCPWM0_BIT24_ZCS0_IF,            /*!< 24,时基 0 ZCS 中断源事件 */
    MCPWM0_BIT25_ZCS1_IF,            /*!< 25,时基 1 ZCS 中断源事件 */
    MCPWM0_BIT26_Fault,              /*!< 26,未使用 */
} mcpwm0_int_source_e;


/** mcpwm1 interrupt types*/
typedef enum{
    MCPWM1_BIT0_T0_IF = 0U,          /*!< 0, T0 事件， MCPWM0_CNT1 等于 MCPWM0_TH 中断事件。1，发生； 0，没发生。写 1 清零。*/
    MCPWM1_BIT1_T1_IF,               /*!< 1, T1 事件， MCPWM0_CNT1 等于 0 中断事件。 */
    MCPWM1_BIT2_TH30_IF,             /*!< 2, MCPWM0_CNT1 等于 MCPWM0_TH30 中断事件。*/
    MCPWM1_BIT3_TH31_IF,             /*!< 3, MCPWM0_CNT1 等于 MCPWM0_TH31 中断事件。*/
    MCPWM1_BIT4_TH40_IF,             /*!< 4, MCPWM0_CNT1 等于 MCPWM0_TH40 中断事件。*/
    MCPWM1_BIT5_TH41_IF,             /*!< 5, MCPWM0_CNT1 等于 MCPWM0_TH41 中断事件。*/
    MCPWM1_BIT6_TH50_IF,             /*!< 6, MCPWM0_CNT1 等于 MCPWM0_TH50 中断事件。*/
    MCPWM1_BIT7_TH51_IF,             /*!< 7, MCPWM0_CNT1 等于 MCPWM0_TH51 中断事件。 */
    MCPWM1_BIT8_Reserved,            /*!< 8, 未使用 */
    MCPWM1_BIT9_Reserved,            /*!< 9, 未使用 */
    MCPWM1_BIT10_Reserved,           /*!< 10, 未使用 */
    MCPWM1_BIT11_Reserved,           /*!< 11, 未使用 */
    MCPWM1_BIT12_TMR2_IF,            /*!< 10, MCPWM0_CNT1 等于 MCPWM0_TMR2 中断事件。*/
    MCPWM1_BIT13_TMR3_IF,            /*!< 11, MCPWM0_CNT1 等于 MCPWM0_TMR3 中断事件。*/
    MCPWM1_BIT14_UP_IF,              /*!< 14, 时基 1 更新事件MCPWM0_TH1/MCPWM0_TH30~MCPWM0_TH51/MCPWM0_TMR 等寄存器更新到 MCPWM 实际运行系统的中断事件。*/
    MCPWM1_BIT15_Reserved,           /*!< 15,未使用 */
    MCPWM1_BIT16_Reserved,           /*!< 16,未使用 */
    MCPWM1_BIT17_Reserved,           /*!< 17,未使用 */
    MCPWM1_BIT18_Reserved,           /*!< 18,未使用 */
    MCPWM1_BIT19_Reserved,           /*!< 19,未使用 */
    MCPWM1_BIT20_FAIL0_IF,           /*!< 20,FAIL0 中断源事件 */
    MCPWM1_BIT21_FAIL1_IF,           /*!< 21,FAIL1 中断源事件 */
    MCPWM1_BIT22_FAIL2_IF,           /*!< 22,FAIL2 中断源事件 */
    MCPWM1_BIT23_FAIL3_IF,           /*!< 23,FAIL3 中断源事件 */
    MCPWM1_BIT24_ZCS0_IF,            /*!< 24,时基 0 ZCS 中断源事件 */
    MCPWM1_BIT25_ZCS1_IF,            /*!< 25,时基 1 ZCS 中断源事件 */
    MCPWM1_BIT26_Fault,              /*!< 26,未使用 */
} mcpwm1_int_source_e;


typedef enum
{
  IO_BKIN0 = 0U,                  /*!< 0, IO口 */
  CAMP1_OUTPUT,                      /*!< 1, 比较器1原始输出 */
  CLUOUT0,                           /*!< 2, CLUOUT0 */
  CLUOUT1,                           /*!< 3, CLUOUT1 */   
}Fail0_Source_e;


typedef enum
{
  IO_BKIN1 = 0U,                  /*!< 0, IO口 */
  CAMP0_OUTPUT,                      /*!< 1, 比较器1原始输出 */
  CLUOUT2,                           /*!< 2, CLUOUT2 */
  CLUOUT3,                           /*!< 3, CLUOUT3 */
}Fail1_Source_e;



/*interrupt handle definition*/
typedef void (*MCPWM0_INT_CALLBACK)(mcpwm0_int_source_e);
typedef void (*MCPWM1_INT_CALLBACK)(mcpwm1_int_source_e);


#endif
