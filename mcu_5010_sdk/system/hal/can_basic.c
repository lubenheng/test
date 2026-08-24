/******************************************************************************
 * @version: V2.0.1  demo
 ******************************************************************************/

/*************<start>******************/


/*************<include>****************/

#include "platform_common.h"

#include "can_reg.h"
#include "device_resource.h"
#include "can_hal.h"

#include "can.h"

#if AC78xx
#include "ac78xx_timer.h"
#endif





#if (CAN_SAMPLE_SEL == CAN_BASIC_SAMPLE)
/*************<macro>******************/
/*下面3个配置,可任意配置,但同时只能打开其中一个*/
#define ONLY_RECV_STD_FRM			(0)//只接收标准帧数据
#define ONLY_RECV_EXT_FRM			(1)//只接收扩展帧数据
#define RECV_STD_EXT_BOTH_FRM		(2)//标准与扩展帧都接收
#define RECV_FRM_KIND_SEL			(ONLY_RECV_STD_FRM)

#define CAN_CLEAR_ALL_ID_BIT	(0x60000000)//是清零所有ID位,保留控制过滤ID类型位


/*************<enum>*******************/


/*************<union>******************/


/*************<struct>*****************/


/*************<variable>***************/
CAN_MSG_INFO 			g_sendCANMsgInfo;
CAN_MSG_INFO 			g_recvCANMsgInfo;
CAN_FilterControl 		g_canFilterTab[CAN_MAX_FILTER_NUM] = 
{
	{ 0, 0, 0x00000000, 0x00000000},    
    { 1, 0, 0x00000000, 0x00000000},    
    { 2, 0, 0x00000000, 0x00000000},
    { 3, 0, 0x00000000, 0x00000000},
    { 4, 0, 0x00000000, 0x00000000},
    { 5, 0, 0x00000000, 0x00000000},
    { 6, 0, 0x00000000, 0x00000000},
    { 7, 0, 0x00000000, 0x00000000},
    { 8, 0, 0x00000000, 0x00000000},    
    { 9, 0, 0x00000000, 0x00000000},    
    {10, 0, 0x00000000, 0x00000000},   
    {11, 0, 0x00000000, 0x00000000},
    {12, 0, 0x00000000, 0x00000000},
    {13, 0, 0x00000000, 0x00000000},
    {14, 0, 0x00000000, 0x00000000},
    {15, 0, 0x00000000, 0x00000000},
};
uint32_t 				g_canFilterMask[3] = {0x5FFFFFFF,  0x7FFFFFFF,  0x1FFFFFFF};
//											 只接收标准帧 只接收扩展帧 接收两种数据帧
uint8_t 				g_recvedCANDataRdy = FALSE;
uint8_t 				g_sendCANDataFlag = TRUE;
uint8_t					g_sendCANDataTimeCnt = 0;
uint32_t 				g_sendCANDataTtlTimeCnt = 0;
uint32_t				g_recvStopTimeCnt = 0;
uint8_t					g_sendCANDataBuf[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};

uint8_t 				g_blinkLED2Flag = FALSE;
uint8_t					g_blinkLED3Flag = FALSE;
uint8_t					g_blinkLED2TimeCnt = 0;
uint8_t					g_blinkLED3TimeCnt = 0;


/*************<prototype>**************/
void TIM2_IRQnCallBack(uint8_t lparam);//TIM2的回调函数
int32_t CAN1_IRQnCallBack(uint32_t event, uint32_t wparam, uint32_t lparam);


/**
* @prototype CAN_Basic_Sample(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 CAN basic sample.
*			 CAN基本应用展示.实现数据收发.
*/
void CAN_Basic_Sample(void)
{
#if AC78xx
	GPIO_InitHardwr();
#endif
	CAN_InitHardwr();
	TIMER_InitHardwr();
#if AC78xx	
	while(1)
	{
		CAN_CheckDataDealPrd();
		CAN_CheckLEDBlinkPrd();
	}
#endif
}

/**
* @prototype CAN_CheckDataDealPrd(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 Check CAN data recv or send should be do.
*			 查询是否需要处理CAN数据.
*/
void CAN_CheckDataDealPrd(void)
{
	if ((g_sendCANDataFlag) && (g_sendCANDataTimeCnt > 99))//100ms的周期发送数据
	{
		g_sendCANDataTimeCnt = 0;
		g_blinkLED3Flag   	 = TRUE;
		
		#if (RECV_FRM_KIND_SEL == ONLY_RECV_EXT_FRM)
		CAN_SendMsg(CAN1_SEND_DATA_ID11, CAN_IDE_EXT, CAN_RTR_STD, g_sendCANDataBuf, 8);//发送数据
		#else
		CAN_SendMsg(CAN1_SEND_DATA_ID1, CAN_IDE_STD, CAN_RTR_STD, g_sendCANDataBuf, 8);//发送数据
		#endif
		
		if (g_sendCANDataTtlTimeCnt > 1999)//总发送时间2s,时间到达后,停止发送数据
		{
			g_sendCANDataFlag = FALSE;
			g_sendCANDataTtlTimeCnt = 0;
			
			g_blinkLED3Flag = FALSE;
			#if AC78xx
			LED3_OFF;
			#endif
		}
	}
	else if ((!g_sendCANDataFlag) && (g_sendCANDataTtlTimeCnt > 1999))//停止发送2s后,又重新开始2s的发送周期
	{
		g_sendCANDataFlag = TRUE;
		
		g_sendCANDataTimeCnt = 0;
		g_sendCANDataTtlTimeCnt = 0;
	}
	
	if (g_recvedCANDataRdy)//数据接收后,处理
	{
		//复制数据到处理缓冲区,获取长度,ID信息
		
		g_recvedCANDataRdy = FALSE;
		
		//处理CAN数据
		
		if (g_blinkLED2Flag == FALSE)//只有在当前没有闪烁灯的情况下,启动闪烁
		{
			g_blinkLED2Flag = TRUE;
			g_blinkLED2TimeCnt = 0;
		}
		
		g_recvStopTimeCnt = 0;
	}
	else
	{
		if ((g_blinkLED2Flag) && (g_recvStopTimeCnt > 299))//300ms内没有接收到任何数据,停止闪烁
		{
			g_blinkLED2Flag = FALSE;
			#if AC78xx
			LED2_OFF;
			#endif
		}
	}
}

/**
* @prototype CAN_CheckLEDBlinkPrd(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 Check the led weather to be blinking.
*			 检查LED是否需要闪烁.
*/
void CAN_CheckLEDBlinkPrd(void)
{
	if ((g_blinkLED2Flag) && (g_blinkLED2TimeCnt > 49))//闪烁LED2
	{
		g_blinkLED2TimeCnt = 0;
		
		#if AC78xx
		LED2_TOGGLE;
		#endif
	}
	
	if ((g_blinkLED3Flag) && (g_blinkLED3TimeCnt > 49))//闪烁LED3
	{
		g_blinkLED3TimeCnt = 0;
		
		#if AC78xx
		LED3_TOGGLE;
		#endif
	}
}

/**
* @prototype CAN_InitHardwr(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 Initalize CAN module.
*			 初始化CAN模块.
*/
void CAN_InitHardwr(void)
{
	CAN_Config canConfig = {0};
	CAN_BaudrateConfig canBandrateConfig = {0};
#if AC78xx	
	GPIO_SetFunc(CAN1_TX, GPIO_FUNC_1);//设置CAN1引脚复用功能
	GPIO_SetFunc(CAN1_RX, GPIO_FUNC_1);
	GPIO_SetDir(CAN1_STB, GPIO_OUTPUT);//设置CAN1收发器控制为IO控制
	CAN1_TRASCVER_NML;//使能CAN收发器
#endif
	
	/*
	设置波特率为500K,采样点为81.25%.
	tSeg1 = (S_SEG_1 + 2); tSeg2 = (S_SEG_2 + 1).
	BandRate 	= (48M / (S_PRESC + 1) / ((S_SEG_1 + 2) + (S_SEG_2 + 1)))
	SamplePoint = (tSeg1 / (tSeg1 + tSeg2)).
	在已经知道波特率与采样点的情况下,经过计算后:tSeg1 = 13,tSeg2 = 3.
	 */
	canBandrateConfig.S_PRESC 	= 5;//分频后时钟为8M.
	canBandrateConfig.S_SEG_1 	= 11;
	canBandrateConfig.S_SEG_2 	= 2;
	canBandrateConfig.S_SJW 	= 2;//满足S_SJW <= tSeg2即可.
	
	/* 
	16个过滤器中,可随意选择使用哪个,并制定其过滤模式,制定其过滤ID的类型.
	如果一个过滤器只需要接收一个ID,直接可将其设置CODE模式,然后根据ID类型设置过滤类型.
	如果一个过滤器需要接收多个ID，将其设置为MASK模式,并根据这几个ID的实际类型设置过滤ID类型,如果既有STD又有EXT,则应选择FILTER_IDE_STD_EXT_BOTH.
	
	此处加宏是为了更清楚的说明各种设置是否有效,在实际应用中,可将各种组合放在一起使用.
	 */
	#if (RECV_FRM_KIND_SEL == ONLY_RECV_STD_FRM)
	CAN_SetFilterParam( 0, 1, CAN_FILTER_CODE_MODE, FILTER_IDE_STD_ONLY, CAN1_RECV_DATA_ID1);//使用CODE模式只接收STD帧
	CAN_SetFilterParam( 1, 1, CAN_FILTER_CODE_MODE, FILTER_IDE_STD_ONLY, CAN1_RECV_DATA_ID2);//使用CODE模式只接收STD帧
	#endif
	#if (RECV_FRM_KIND_SEL == ONLY_RECV_EXT_FRM)
	CAN_SetFilterParam( 0, 1, CAN_FILTER_CODE_MODE, FILTER_IDE_EXT_ONLY, CAN1_RECV_DATA_ID11);//使用CODE模式只接收EXT帧
	CAN_SetFilterParam( 1, 1, CAN_FILTER_CODE_MODE, FILTER_IDE_EXT_ONLY, CAN1_RECV_DATA_ID12);//使用CODE模式只接收EXT帧
	#endif
	#if (RECV_FRM_KIND_SEL == RECV_STD_EXT_BOTH_FRM)
	CAN_SetFilterParam( 0, 1, CAN_FILTER_MASK_MODE, FILTER_IDE_STD_EXT_BOTH, (CAN1_RECV_DATA_ID1 & CAN1_RECV_DATA_ID11));//使用MASK模式接收两种帧
	CAN_SetFilterParam( 1, 1, CAN_FILTER_MASK_MODE, FILTER_IDE_STD_EXT_BOTH, (CAN1_RECV_DATA_ID2 & CAN1_RECV_DATA_ID12));//使用MASK模式接收两种帧
	#endif
	
	canConfig.interruptEnable = TRUE;//使能中断
	canConfig.canMode = CAN_MODE_NORMAL;
	canConfig.autoReset = TRUE;//CAN BUSOFF后,自动恢复
	canConfig.filterList = g_canFilterTab;//赋值过滤器设置list
	
	CAN_SetEventCallBack(CAN1, CAN1_IRQnCallBack);
	CAN_Initialize(CAN1, &canConfig, &canBandrateConfig);
}

/**
* @prototype CAN_SetFilterParam(uint8_t filterNum, uint8_t filterEn, CAN_FilterMode_TypeDef filterMode, CAN_FilterMask_TypeDef filterMask, uint32_t ID)
*
* @param[in] filterNum:过滤器编码,0到15可用.
* @param[in] filterEn:过滤器使能状态,1使能,0禁能
* @param[in] filterMode:CAN_FilterMode_TypeDef已定义.
* @param[in] filterMask:参考CAN_FilterMask_TypeDef.
* @param[in] ID
* @return	 void
*
* @brief  	 Initalize CAN filter.
*			 初始化CAN过滤器,当filterMode选择FILTER_CODE_MODE时,每个过滤器过滤一个ID,否则,使用MASK模式,过滤的多个ID的组合.
*/
void CAN_SetFilterParam(uint8_t filterNum, uint8_t filterEn, CAN_FilterMode_TypeDef filterMode, CAN_FilterMask_TypeDef filterMask, uint32_t ID)
{
	g_canFilterTab[filterNum].enable = filterEn;
	g_canFilterTab[filterNum].code = ID;
	
	if (filterMode == CAN_FILTER_MASK_MODE)
	{
		g_canFilterTab[filterNum].mask = (g_canFilterMask[filterMask] & ~ID);//进行掩码屏蔽
	}
	else
	{
		g_canFilterTab[filterNum].mask = (g_canFilterMask[filterMask] & CAN_CLEAR_ALL_ID_BIT);//进行ID屏蔽,就是ID必须与设置的ID一模一样,才接收
	}
}

/**
* @prototype Sdk_CAN_SendMsg(uint32_t ID, CAN_IDE_TypeDef IDE, CAN_RTR_TypeDef RTR, uint8_t *pDataBuf, uint8_t dataLen)
*
* @param[in] ID: send data CAN ID. 
* @param[in] IDE: Choose between CAN_IDE_STD and CAN_IDE_EXT.
* @param[in] RTR: Choose between CAN_RTR_STD and CAN_RTR_RMT.
* @param[in] pDataBuf: The pointer of the data to be sended.
* @param[in] dataLen: Send data length.
* @return	 CAN_SendSts_TypeDef,send fail or success.
*
* @brief  	 Send CAN message.
*			 发送CAN数据.
*/
CAN_SendSts_TypeDef CAN_SendMsg(uint32_t ID, CAN_IDE_TypeDef IDE, CAN_RTR_TypeDef RTR, uint8_t *pDataBuf, uint8_t dataLen)
{
	g_sendCANMsgInfo.ID = ID;
	g_sendCANMsgInfo.IDE = IDE;
	g_sendCANMsgInfo.RTR = RTR;
	g_sendCANMsgInfo.DLC = dataLen;
	memcpy(g_sendCANMsgInfo.Data, pDataBuf, dataLen);
	
	if(!CAN_IsTransmitBusy(CAN1, TRANSMIT_SECONDARY))
	{
		if (!CAN_MessageSend(CAN1, &g_sendCANMsgInfo, TRANSMIT_SECONDARY))
		{
			return CAN_SEND_MSG_SUCCS;
		}
		else
		{
			return CAN_SEND_MSG_FILED;
		}
	}
	
	return CAN_SEND_MSG_FILED;
}

/**
* @prototype TIMR_InitHardwr(void)
*
* @param[in] void
* @return	 void
*
* @brief  	 Initalize TIM2 module.
*			 初始化TIM2模块.
*/
void TIMER_InitHardwr(void)
{
#if AC78xx
	
	TIMER_ConfigType timerConfig = {0};
	
	timerConfig.loadValue = (48000000 / (1000 - 1));//定时器时钟只能是外设总线时钟
	timerConfig.linkMode = 0;//不使用多定时器进行链接
	timerConfig.interruptEn = 1;//使能中断
	timerConfig.timerEn = 1;//使能定时器
	
	TIMER_SetCallback(TIMER2,TIM2_IRQnCallBack);//设置中断回调函数
	TIMER_Init(TIMER2,&timerConfig);
	
#endif
}

/**
* @prototype TIM2_IRQnCallBack(uint8_t lparam)
*
* @param[in] ...
* @return	 ...
*
* @brief  	 TIM2 module interrupt handler.
*			 TIM2中断处理函数.
*/
void TIM2_IRQnCallBack(uint8_t lparam)
{
#if AC78xx
	if (TIMER_GetIntFlag(TIMER2))
    {
        TIMER_ClrIntFlag(TIMER2);
		
		if ((g_blinkLED2Flag) && (g_blinkLED2TimeCnt < 0xFF))
		{
			g_blinkLED2TimeCnt++;
		}
		
		if ((g_blinkLED3Flag) && (g_blinkLED3TimeCnt < 0xFF))
		{
			g_blinkLED3TimeCnt++;
		}
		
		if ((g_sendCANDataFlag) && (g_sendCANDataTimeCnt < 0xFF))
		{
			g_sendCANDataTimeCnt++;
		}
		
		if ((g_blinkLED2Flag) && (g_recvStopTimeCnt < 0xFFFF))
		{
			g_recvStopTimeCnt++;
		}
		
		g_sendCANDataTtlTimeCnt++;
	}
#endif
}

/**
* @prototype CAN1_IRQnCallBack(uint32_t event, uint32_t wparam, uint32_t lparam)
*
* @param[in] ...
* @return	 ...
*
* @brief  	 CAN1 module interrupt handler.
*			 CAN1中断处理函数.
*/
int32_t CAN1_IRQnCallBack(uint32_t event, uint32_t wparam, uint32_t lparam)
{
	if (event & CAN_EVENT_RECVMSG)
	{
        /* 防止接收中断未及时响应，出现接收到多帧报文才进入一次中断的情况，
        * 通过while的方式轮询完所有的缓存，若担心安全问题，可增加while计数
        * 超时。
        */
		//if (CAN_IsMsgInReceiveBuf((CAN_Type*)lparam))
		while (CAN_IsMsgInReceiveBuf((CAN_Type*)lparam))
        {
            CAN_MessageRead((CAN_Type*)lparam, &g_recvCANMsgInfo);
			
			g_recvedCANDataRdy = TRUE;
        }
	}
	
	return 1;
}

#endif
/*************<end>********************/


