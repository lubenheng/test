#ifndef ADC_HAL_H
#define ADC_HAL_H



typedef struct {

	// 采样时钟配置
	u8 STPT_CNT;
	u8 COVT_CNT;
	u8 SAPT_CNT;
	u8 INIT_CNT;
	u8 DELAY_CNT;	
	
	
	// 设置分组模式
	u8 GroupMode;

	// 设置每组采样通道数量
	u8 TigChNum[4] ;
	// 设置seqx对应通道
	u8 Seq[16];
	
	// 设置软件or硬件触发
	u8 TigMode;
	// 设置每组硬件触发源
	u8 TigSrc[4];	
	// 设置MCPWM触发源Cnt值
	s16 TigCnt[4]  ;
	
	// 设置看门狗阈值
	u16 AW_CH_EN  ;		
	u16 AW_HTH  ;		
	u16 AW_LTH  ;		


	// 设置ADC中断源
	u32 INTR_SRC;		
	u32	INTR_SRC_DONE;
	u8	EnINTR;
	u8	DisINTR;
	
	// 软件触发标志
	u8 SWTigCtrl;
	
	// 更新控制
	u8 update;
	u8 RstFlag;
	u8 ErrFlag;
}struct_AdcTest_OL;


#endif /*ADC_TEST_H*/

