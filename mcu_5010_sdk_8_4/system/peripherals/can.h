#ifndef CAN_H
#define CAN_H


/*************<include>****************/
#include "can_type.h"
//#include "gpio.h"


#define  AC78xx   0


#define FALSE						(0U)
#define TRUE						(1U)	
///////////////////debugzone.h//////////////////
#define DEBUG_ZONE_ERROR   1
#define DEBUG_ZONE_INFO    1
#define DEBUG_ZONE_WARNING 1
#define DEBUG_ZONE_DEBUG   0

#define ERROR_NO_EXCUTE  0xFF
 
#define NO_ERROR            0
#define ERROR_EXTI          1
#define ERROR_MALLOC_FAIL   2
///////////////////debugzone.h//////////////////




/*************<macro>******************/
	
/**************************************************************************************************
	主要功能:
	上电后LED2,LED3处于熄灭状态,CAN配置正常模式,可接收可发送数据.
	上电后,CAN以100ms的周期发送数据,连续发送2s时间后停止发送数据2s时间,依次循环.
	当CAN接收到数据时,LED2以50ms的频率闪烁,300ms内没有接收到任何数据,LED2立即熄灭.
	当CAN发送数据时,LED3以50ms的频率闪烁,当停止发送数据时,LED3立即熄灭.
	
	在该例程下,可配置只接收标准数据帧,即将ONLY_RECV_STD_FRM设置为TRUE.
			   可配置只接收扩展数据帧,即将ONLY_RECV_EXT_FRM设置为TRUE.
			   可配置接收标准扩展数据帧,即将RECV_STD_EXT_BOTH_FRM设置为TRUE.
	
	掌握要点:
	如何设置CAN引脚功能;如何设置过滤器;如何计算与设置CAN波特率与采样点.如何在中断中获取CAN数据.如何发送CAN数据.
 **************************************************************************************************/
#define CAN_BASIC_SAMPLE			(1)

/**************************************************************************************************
	主要功能:
	上电后LED2,LED3处于熄灭状态,CAN配置正常模式,CAN以100ms的周期发送数据.
	当CAN接收到数据时,LED2以50ms的频率闪烁,300ms内没有接收到任何数据,LED2立即熄灭.
	当CAN发送数据时,LED3以50ms的频率闪烁,当停止发送数据时,LED3立即熄灭.
	当CAN总线发生故障后BUSOFF时,LED2,LED3同时熄灭;当CAN总线恢复正常后,LED也根据对应功能恢复对应动作.
	
	当BUSOFF发生后,MCU刚开始以50ms的周期恢复CAN 10次,之后以1S的周期进行恢复CAN.
	
	在该例程下,可配置只接收标准数据帧,即将ONLY_RECV_STD_FRM设置为TRUE.
			   可配置只接收扩展数据帧,即将ONLY_RECV_EXT_FRM设置为TRUE.
			   可配置接收标准扩展数据帧,即将RECV_STD_EXT_BOTH_FRM设置为TRUE.
	
	掌握要点:
	软件如何探测BUSOFF,以及CAN BUSOFF后如何恢复CAN总线(偏向于前装车厂规范要求).
 **************************************************************************************************/
#define CAN_BUSOFF_SAMPLE			(2)

/**************************************************************************************************
	主要功能:
	上电后LED2,LED3处于熄灭状态,CAN配置正常模式,CAN以100ms的周期发送数据.
	当CAN发送数据时,LED3以50ms的频率闪烁,发送5s停止发送数据,LED3立即熄灭,MCU进入休眠.
	当CAN接收到数据,MCU被唤醒,继续向MCU发送数据,LED2以50ms的频率闪烁,300ms内没有接收到任何数据,LED2立即熄灭.如果满足睡眠条件,休眠.
	
	在该例程下,可配置只接收标准数据帧,即将ONLY_RECV_STD_FRM设置为TRUE.
			   可配置只接收扩展数据帧,即将ONLY_RECV_EXT_FRM设置为TRUE.
			   可配置接收标准扩展数据帧,即将RECV_STD_EXT_BOTH_FRM设置为TRUE.
	
	掌握要点:
	如何设置CAN唤醒MCU.MCU被唤醒后需要做哪些处理.
	主要是判断MCU休眠后,哪些外设被关闭,被唤醒后,则需要重新初始化这部分需要使用的外设.
 **************************************************************************************************/
#define CAN_WKUP_SAMPLE				(3)
	
#define CAN_SAMPLE_SEL				(CAN_BASIC_SAMPLE)//选择例程


#define CAN1_RECV_DATA_ID1			(0x220)
#define CAN1_RECV_DATA_ID2			(0x230)
#define CAN1_SEND_DATA_ID1			(0x430)

#define CAN1_RECV_DATA_ID11			(0x18DFF930)
#define CAN1_RECV_DATA_ID12			(0x18BFF930)
#define CAN1_SEND_DATA_ID11			(0x18DEE050)

#define CAN1_TX						(GPIO_PC15)
#define CAN1_RX						(GPIO_PC14)
#define CAN1_STB					(GPIO_PD5)

#define CAN1_TRASCVER_NML			do{GPIO_ResetPinBits(CAN1_STB);}while(0)
#define CAN1_TRASCVER_SLP			do{GPIO_SetPinBits(CAN1_STB);udelay(20);}while(0)


#define CAN2_TX						(GPIO_PC10)
#define CAN2_RX						(GPIO_PC11)
#define CAN2_STB					(GPIO_PB0)

#define CAN2_TRASCVER_NML			do{GPIO_ResetPinBits(CAN2_STB);}while(0)
#define CAN2_TRASCVER_SLP			do{GPIO_SetPinBits(CAN2_STB);udelay(20);}while(0)

/*************<enum>*******************/
typedef enum
{
	CAN_IDE_STD = 0,
	CAN_IDE_EXT
}CAN_IDE_TypeDef;

typedef enum
{
	CAN_RTR_STD = 0,
	CAN_RTR_RMT
}CAN_RTR_TypeDef;

typedef enum
{
	CAN_SEND_MSG_FILED = 0,
	CAN_SEND_MSG_SUCCS
}CAN_SendSts_TypeDef;

typedef enum
{
	FILTER_IDE_STD_ONLY 	= 0,
	FILTER_IDE_EXT_ONLY 	= 1,
	FILTER_IDE_STD_EXT_BOTH	= 2
}CAN_FilterMask_TypeDef;

typedef enum
{
	CAN_FILTER_CODE_MODE	= 0,
	CAN_FILTER_MASK_MODE	= 1,
}CAN_FilterMode_TypeDef;

/*************<union>******************/


/*************<struct>*****************/


/*************<extern>*****************/	


/*************<prototype>**************/
void CAN_InitHardwr(void);
void CAN_SetFilterParam(uint8_t filterNum, uint8_t filterEn, CAN_FilterMode_TypeDef filterMode, CAN_FilterMask_TypeDef filterMask, uint32_t ID);
CAN_SendSts_TypeDef CAN_SendMsg(uint32_t ID, CAN_IDE_TypeDef IDE, CAN_RTR_TypeDef RTR, uint8_t *pDataBuf, uint8_t dataLen);

void TIMER_InitHardwr(void);
void CAN_CheckDataDealPrd(void);
void CAN_CheckLEDBlinkPrd(void);

#if (CAN_SAMPLE_SEL == CAN_BASIC_SAMPLE)
void CAN_Basic_Sample(void);
#elif (CAN_SAMPLE_SEL == CAN_BUSOFF_SAMPLE)
void CAN_BusOff_Sample(void);
void CAN_CheckBusErrStsPrd(void);
#elif (CAN_SAMPLE_SEL == CAN_WKUP_SAMPLE)
void CAN_Wkup_Sample(void);
#endif

//#ifdef __cplusplus
//}
//#endif

#endif /* __CAN_H__ */

/*************<end>********************/


