#ifndef CSA_REG_H
#define CSA_REG_H


typedef struct
{
	__IO uint32_t RESERV1;
	
	union
	{
			struct{
					__IO uint32_t RO_CSA_OC_LAT     : 1;  /* bit[0]  CSA过流锁存标志 */
					__IO uint32_t RO_TJ_OTS_LAT     : 1;  /* bit[1]  OTS过温关断锁存标志 */
					__IO uint32_t RO_TJ_OTW_LAT     : 1;  /* bit[2]  OTW高温预警锁存标志 */
					__IO uint32_t RESERVED0         :13;  /* bit[15:3] 保留 */
					__IO uint32_t IR_CSA_OC_NG      : 1;  /* bit[16] CSA过流实时index信号 */
					__IO uint32_t IR_TJ_OTS_NG      : 1;  /* bit[17] OTS过温关断实时index信号 */
					__IO uint32_t IR_TJ_OTW_NG      : 1;  /* bit[18] OTW高温预警实时index信号 */
					__IO uint32_t RESERVED1         :13;  /* bit[31:19] 保留 */
			};
			__IO uint32_t Word;
	} MISC_IR;

	__IO uint32_t RESERV2;
	__IO uint32_t RESERV3;
	union
	{
			struct{
					__IO uint32_t RW_ANA_CSA_EN     : 1;  /* bit[0]     CSA电流采样模块使能 */
					__IO uint32_t RW_CSA_AMP_EN     : 1;  /* bit[1]     CSA运放使能 */
					__IO uint32_t RW_CSA_MODE       : 2;  /* bit[3:2]   CSA与ADC MUX联动模式 */
					__IO uint32_t RW_CSA_CSP        : 2;  /* bit[5:4]   同相采样点通道选择 */
					__IO uint32_t RW_CSA_CSN        : 2;  /* bit[7:6]   反相采样点通道选择 */
					__IO uint32_t RW_CSA_GAIN       : 3;  /* bit[10:8]  CSA增益配置 */
					__IO uint32_t RW_CSA_DAC_EN     : 1;  /* bit[11]    DAC使能 */
					__IO uint32_t RW_CSA_REF        : 5;  /* bit[16:12] CSA偏置电压配置 */
					__IO uint32_t RESERVED0         : 3;  /* bit[19:17] 保留 */
					__IO uint32_t RW_CSA_DAC_REF    : 3;  /* bit[22:20] DAC量程配置 */
					__IO uint32_t RESERVED1         : 1;  /* bit[23]    保留 */
					__IO uint32_t RW_CSA_DAC_OUT    : 8;  /* bit[31:24] DAC输入信号 */
			};
			__IO uint32_t Word;
	} MISC_CSACR0;

	union
	{
			struct{
					__IO uint32_t RW_CSA_OC_EN        : 1;  /* bit[0]     CSA OC检测使能 */
					__IO uint32_t RESERVED0           : 3;  /* bit[3:1]   保留 */
					__IO uint32_t RW_CSA_OC_SEL       : 3;  /* bit[6:4]   OC检测信号选择 */
					__IO uint32_t RESERVED1           : 1;  /* bit[7]     保留 */
					__IO uint32_t RW_CSA_OC_HYS       : 3;  /* bit[10:8]  OC检测迟滞配置 */
					__IO uint32_t RESERVED2           : 1;  /* bit[11]    保留 */
					__IO uint32_t RW_CSA_OC_NG_EN     : 1;  /* bit[12]    OC指示信号消抖使能 */
					__IO uint32_t RESERVED3           : 1;  /* bit[13]    保留 */
					__IO uint32_t RW_CSA_OC_NG        : 2;  /* bit[15:14] OC指示信号消抖时长 */
					__IO uint32_t RW_CSA_LPF_GPIO      : 1;  /* bit[16]    CSA LPF外接滤波电容路径使能 */
					__IO uint32_t RW_CSA_LPF_SEL      : 1;  /* bit[17]    CSA输出片外滤波使能 */
					__IO uint32_t RW_CSA_LPF          : 2;  /* bit[19:18] CSA输出滤波截止频率 */
					__IO uint32_t RESERVED4           :10;  /* bit[30:20] 保留 */
					__IO uint32_t IR_CSA_OC_NG        : 1;  /* bit[31]    CSA过流报警实时Index信号(RO) */
			};
			__IO uint32_t Word;
	} MISC_CSACR1;
	
	union
	{
			struct{
					__IO uint32_t RW_ANA_TJ_EN        : 1;  /* bit[0]     内部温度检测模块使能 */
					__IO uint32_t RW_TJ_OTS_NG_EN     : 1;  /* bit[1]     过温关断信号消抖使能 */
					__IO uint32_t RW_TJ_OTW_NG_EN     : 1;  /* bit[2]     过温报警信号消抖使能 */
					__IO uint32_t RESERVED0           : 1;  /* bit[3]     保留 */
					__IO uint32_t RW_TJ_OTS           : 2;  /* bit[5:4]   过温关断触发温度配置 */
					__IO uint32_t TW_TJ_OTS_NG        : 2;  /* bit[7:6]   过温关断信号消抖时长 */
					__IO uint32_t RW_TJ_OTW           : 2;  /* bit[9:8]   过温警报触发温度配置 */
					__IO uint32_t RW_TJ_OTW_NG        : 2;  /* bit[11:10] 过温警报信号消抖时长 */
					__IO uint32_t RESERVED1           : 4;  /* bit[15:12] 保留 */
					__IO uint32_t RW_ANA_TS_EN        : 1;  /* bit[16]    外部温度检测模块使能 */
					__IO uint32_t RW_TS_GPIO5_EN      : 1;  /* bit[17]    TS电流输出至GPIO5使能(NTC) */
					__IO uint32_t RW_TS_GPIO6_EN      : 1;  /* bit[18]    TS电流输出至GPIO6使能(NTC) */
					__IO uint32_t RESERVED2           :13;  /* bit[31:19] 保留 */
			};
			__IO uint32_t Word;
	} MISC_TEMPCR;

} CSA_TypeDef;


#endif
