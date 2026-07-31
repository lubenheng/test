#include "platform_common.h"

#include "dma_reg.h"
#include "device_resource.h"
#include "dma_hal.h"
#include "dma.h"
#include "dma_type.h"
#include "dma_reg.h"


DMA_TypeDef *DMA0 = DMA;



void HAL_DMA_chx_Enable(DMA_Channel_e dma_channle)
{
	if( dma_channle > 5 )
	{
		return;
	}
	DMA_div_chx_Enable(dma_channle);
}

void HAL_DMA_chx_Disable(DMA_Channel_e dma_channle)
{
	 
	DMA_div_chx_Disable(dma_channle);
}

 

 


/**
 * @brief DMA通道初始化（I2C发送模式）
 * @param dma_channle DMA通道号
 * @param i2c_ptr I2C基地址指针
 * @param tx_buffer 发送缓冲区地址
 * @param buffer_size 缓冲区大小
 */
void HAL_DMA_I2CTx_chx_Init(DMA_Channel_e dma_channle, I2C_TypeDef *i2c_ptr,	\
									 uint32_t tx_buffer, uint32_t buffer_size)
{
	INFO("%s , %d\r\n",(char *)tx_buffer,buffer_size);
	
	dma_ch_config_t dma_handle;
 
	/* 2. 填充DMA配置结构体 */
	dma_handle.dma_ch = dma_channle;
	dma_handle.circular_mode = DMA_MODE_NORMAL;           /* I2C通常用正常模式，不是循环模式 */
	dma_handle.dir = DMA_DIR_MEM_TO_PERIPH;               /* 内存到外设（I2C发送）*/
	dma_handle.burst_len = DMA_BURST_1_UNIT;              /* I2C通常用单次传输 */
	
	/* 3. 地址配置 */
	dma_handle.src_addr = tx_buffer;                       /* 源地址：内存缓冲区 */
	dma_handle.dst_addr = i2c_ptr->IC_DATA_CMD.DAT; /* 目的地址：I2C数据寄存器 */
	
	/* 4. 地址递增模式 */
	dma_handle.src_inc = Addr_Enable;               		 /* 源地址递增（从内存读取不同数据）*/
	dma_handle.dst_inc = Addr_Disable;                   /* 目的地址固定（I2C数据寄存器）*/
	
	/* 5. 数据宽度配置（I2C通常是8位）*/
	dma_handle.dma_data_width_src_width = DMA_BITSIZE_8BIT;  /* 源数据宽度8位 */
	dma_handle.dma_data_width_dst_width = DMA_BITSIZE_8BIT;  /* 目的数据宽度8位 */
	
	/* 6. 触发源配置（根据I2C选择）*/

	if(i2c_ptr == I2C0)
	{
		dma_handle.DMA_Trigger_periph = DMA_Trigger_I2C0_TX;    /* I2C0发送触发 */
	}
	else if(i2c_ptr == I2C1)
	{
		dma_handle.DMA_Trigger_periph = DMA_Trigger_I2C1_TX;    /* I2C1发送触发 */
	}
	else
	{
		dma_handle.DMA_Trigger_periph = DMA_Trigger_RESERVED;
	}
	
	/* 7. 优先级配置 */
	dma_handle.priority = DMA_PRIORITY_MID;
	
	/* 8. 传输大小 */
	dma_handle.transfer_size = buffer_size;
	
	/* 9. 先失能通道（配置前）*/
	DMA_div_chx_Disable(dma_handle.dma_ch);
	
	/* 10. 配置DMA寄存器 */
	DMA_div_srcAddr_Init(dma_handle.dma_ch, dma_handle.src_addr);
	DMA_div_dstAddr_Init(dma_handle.dma_ch, dma_handle.dst_addr);
	DMA_div_Maxlen_Set(dma_handle.dma_ch,dma_handle.transfer_size);
//	DMA_div_TransCnt_Init(dma_handle.dma_ch, dma_handle.transfer_size);
	DMA_div_TriggerSource_Config(dma_handle.dma_ch, dma_handle.DMA_Trigger_periph);
	DMA_div_SrcIncMode_Init(dma_handle.dma_ch, dma_handle.src_inc);
	DMA_div_DstIncMode_Init(dma_handle.dma_ch, dma_handle.dst_inc);
	DMA_div_DIR_Init(dma_handle.dma_ch, dma_handle.dir);
	DMA_div_mode_Init(dma_handle.dma_ch, dma_handle.circular_mode);
	DMA_div_SBURST_Init(dma_handle.dma_ch, dma_handle.burst_len);
	DMA_div_DMAToDst_size_Init(dma_handle.dma_ch, dma_handle.dma_data_width_dst_width);
	DMA_div_SrcToDma_size_Init(dma_handle.dma_ch, dma_handle.dma_data_width_src_width);
	DMA_div_priority_Init(dma_handle.dma_ch, dma_handle.priority);
	HAL_DMA_NVIC_Enable(dma_handle.dma_ch, dma_handle.priority);
	
	/* 12. 使能DMA通道（准备就绪，等待I2C触发）*/
	DMA_div_chx_Enable(dma_handle.dma_ch);
	
	INFO("DMA Channel %d initialized for I2C TX\n", dma_channle);
}

/**
 * @brief DMA通道初始化（I2C接收模式）
 * @param dma_channle DMA通道号
 * @param i2c_ptr I2C基地址指针
 * @param tx_buffer 发送缓冲区地址
 * @param buffer_size 缓冲区大小
 */
void HAL_DMA_I2CRx_chx_Init(DMA_Channel_e dma_channle, I2C_TypeDef *i2c_ptr,	\
									const uint32_t *rx_buffer, uint32_t buffer_size)
{
	INFO("%s , %d\r\n",(char *)rx_buffer,buffer_size);
	
	dma_ch_config_t dma_handle;
 
	/* 2. 填充DMA配置结构体 */
	dma_handle.dma_ch = dma_channle;
	dma_handle.circular_mode = DMA_MODE_NORMAL;           /* I2C通常用正常模式，不是循环模式 */
	dma_handle.dir = DMA_DIR_PERIPH_TO_MEM;               /* 内存到外设（I2C发送）*/
	dma_handle.burst_len = DMA_BURST_1_UNIT;              /* I2C通常用单次传输 */
	
	/* 3. 地址配置 */
	dma_handle.src_addr = (uint32_t)&i2c_ptr->IC_DATA_CMD;         /* 目的地址：I2C数据寄存器 */               
	dma_handle.dst_addr = (uint32_t)rx_buffer;									  	/* 源地址：内存缓冲区 */
	
	/* 4. 地址递增模式 */
	dma_handle.src_inc = Addr_Disable;               		 /* 目的地址固定（I2C数据寄存器）*/
	dma_handle.dst_inc = Addr_Enable;                    /* 源地址递增（从内存读取不同数据）*/
	
	/* 5. 数据宽度配置（I2C通常是8位）*/
	dma_handle.dma_data_width_src_width = DMA_BITSIZE_8BIT;  /* 源数据宽度8位 */
	dma_handle.dma_data_width_dst_width = DMA_BITSIZE_8BIT;  /* 目的数据宽度8位 */
	
	/* 6. 触发源配置（根据I2C选择）*/
	/* 根据I2C指针判断是I2C0还是I2C1 */
	if(i2c_ptr == I2C0)
	{
		dma_handle.DMA_Trigger_periph = DMA_Trigger_I2C0_RX;    /* I2C0接收触发 */
	}
	else if(i2c_ptr == I2C1)
	{
		dma_handle.DMA_Trigger_periph = DMA_Trigger_I2C1_RX;    /* I2C1接收触发 */
	}
	else
	{
		dma_handle.DMA_Trigger_periph = DMA_Trigger_RESERVED;
	}
	
	/* 7. 优先级配置 */
	dma_handle.priority = DMA_PRIORITY_MID;
	
	/* 8. 传输大小 */
	dma_handle.transfer_size = buffer_size;
	
	/* 9. 先失能通道（配置前）*/
	DMA_div_chx_Disable(dma_handle.dma_ch);
	
	/* 10. 配置DMA寄存器 */
	DMA_div_srcAddr_Init(dma_handle.dma_ch, dma_handle.src_addr);
	DMA_div_dstAddr_Init(dma_handle.dma_ch, dma_handle.dst_addr);
	DMA_div_Maxlen_Set(dma_handle.dma_ch,dma_handle.transfer_size);
 
	DMA_div_TriggerSource_Config(dma_handle.dma_ch, dma_handle.DMA_Trigger_periph);
	DMA_div_SrcIncMode_Init(dma_handle.dma_ch, dma_handle.src_inc);
	DMA_div_DstIncMode_Init(dma_handle.dma_ch, dma_handle.dst_inc);
	DMA_div_DIR_Init(dma_handle.dma_ch, dma_handle.dir);
	DMA_div_mode_Init(dma_handle.dma_ch, dma_handle.circular_mode);
	DMA_div_SBURST_Init(dma_handle.dma_ch, dma_handle.burst_len);
	DMA_div_DMAToDst_size_Init(dma_handle.dma_ch, dma_handle.dma_data_width_dst_width);
	DMA_div_SrcToDma_size_Init(dma_handle.dma_ch, dma_handle.dma_data_width_src_width);
	DMA_div_priority_Init(dma_handle.dma_ch, dma_handle.priority);
 
	
	/* 12. 使能DMA通道（准备就绪，等待I2C触发）*/
	DMA_div_chx_Enable(dma_handle.dma_ch);
	
	INFO("DMA Channel %d initialized for I2C RX\n", dma_channle);
}



/**
 * @brief 获取DMA通道剩余传输次数
 */
uint32_t HAL_DMA_GetRemainingCnt(DMA_Channel_e dma_channle)
{
	if(dma_channle > 5) return 0;
 
	switch (dma_channle)
	{
			case DMA_CHANNEL_0: return  (uint32_t)DMA_Type->DMA_TCNT0_STR.DMA_Tcnt0 ;//break;
			case DMA_CHANNEL_1: return  (uint32_t)DMA_Type->DMA_TCNT1_STR.DMA_Tcnt1 ;//break;
			case DMA_CHANNEL_2: return  (uint32_t)DMA_Type->DMA_TCNT2_STR.DMA_Tcnt2 ;//break;
			case DMA_CHANNEL_3: return  (uint32_t)DMA_Type->DMA_TCNT3_STR.DMA_Tcnt3 ;//break;
			case DMA_CHANNEL_4: return  (uint32_t)DMA_Type->DMA_TCNT4_STR.DMA_Tcnt4 ;//break;
			case DMA_CHANNEL_5: return  (uint32_t)DMA_Type->DMA_TCNT5_STR.DMA_Tcnt5 ;//break;
			default: return 0;//break;
	}
}




//封装dma.c的函数 成功能函数
/**
 * @brief 启动I2C DMA发送
 * @param dma_ch DMA通道号
 * @param i2c_ptr I2C指针
 * @param slave_addr 从设备地址
 */
void DMA_I2C_Tx_Start(DMA_Channel_e dma_channle, I2C_TypeDef *i2c_ptr, uint16_t slave_addr)
{
	DMA_div_chx_Enable(dma_channle);
	/* 2. 使能I2C的DMA发送请求 */
	if(i2c_ptr == I2C0)
	{
			 
	}
	else if(i2c_ptr == I2C1)
	{
			 
	}
}
 
 


 

//检查忙碌  传输数据标志
uint8_t HAL_DMA_ISBusy(DMA_Channel_e dma_channle)
{
	return DMA_div_IsBusy( dma_channle);
}




//设置DMA传输数据完成中断使能
//6通道任意2通道关联相同触发源触发配置中断
void HAL_DMA_IT_Transfer_Set(DMA_Channel_e dma_channle,dma_it_enable_t enable)
{
	if(dma_channle > 5) return; // 通道号范围为0~5
	uint32_t reg = DMA_Type->DMA_MASK_STR.DMA_Mask; // 读取当前寄存器值
	
	if(enable == dma_it_enable) // 使能传输中断
	{
			reg |= (1U << dma_channle);       /* 置位对应MDx位 (bit0-5) */
	}
	else // 禁用传输中断
	{
			reg &= ~(1U << dma_channle);      /* 清除对应MDx位 */
	}
	
	DMA_Type->DMA_MASK_STR.DMA_Mask = reg; // 写回寄存器
	
	__DSB(); // 数据同步屏障，确保寄存器写入完成

}


//设置DMA传输错误中断使能
void HAL_DMA_IT_TransError_Set(DMA_Channel_e dma_channle,uint32_t enable)
{
	if(dma_channle > 5) return;  
	uint32_t reg = DMA_Type->DMA_MASK_STR.DMA_Mask; 
	
	if(enable == dma_it_enable)  
	{
			reg |= (1 << (8U + dma_channle) );       /* 置位对应MDx位 (bit8-13) */
	}
	else  
	{
			reg &= ~(1 << (8U + dma_channle) );      /* 清除对应MDx位 */
	}
	
	DMA_Type->DMA_MASK_STR.DMA_Mask = reg; // 写回寄存器
	
	__DSB(); // 数据同步屏障，确保寄存器写入完成
}



//设置DMA配置错误中断使能
void HAL_DMA_IT_Configerror_Set(uint32_t enable)
{
	uint32_t reg = DMA_Type->DMA_MASK_STR.DMA_Mask;  
	
	if(enable == dma_it_enable)  
	{
			reg |= (1U << 31);       /* 置位对应MDx位 (bit31) */
	}
	else  
	{
			reg &= ~(1U << 31);      /* 清除对应MDx位 */
	}
	
	DMA_Type->DMA_MASK_STR.DMA_Mask = reg; 
	
	__DSB();  
}


//清除DMA传输中断
void HAL_DMA_IT_Transfer_Clean(DMA_Channel_e dma_channle)
{
	if(dma_channle > 5) return;
	DMA_Type->DMA_CLR_STR.DMA_Clr = (1 << dma_channle);
	__DSB();
}


//清除DMA传输错误中断
void HAL_DMA_IT_Error_Clean(DMA_Channel_e dma_channle)
{
	if(dma_channle > 5) return;
	DMA_Type->DMA_CLR_STR.DMA_Clr |= (1 << (8 +dma_channle));
	__DSB();
}


//清除DMA传输错误中断
void HAL_DMA_IT_Configerror_Clean(DMA_Channel_e dma_channle)
{
	if(dma_channle > 5) return;
	DMA_Type->DMA_CLR_STR.DMA_Clr |= (1U << 31);
	__DSB();
}
/**
 * @brief 获取DMA指定通道的状态标志
 * @param dma_channle: DMA通道号（0~5）
 * @param status_type: 要获取的状态类型（DMA_Status_Type_e枚举）
 * @return 非0=对应标志置位；0=对应标志未置位；999=通道号越界
 */
uint32_t HAL_DMA_Get_status(DMA_Channel_e dma_channle, DMA_Status_Type_e status_type)
{
     // 1. 通道号合法性校验
    if(dma_channle > 5) 
    {
        return 999; // 越界返回特殊值
    }

    // 2. 读取状态寄存器（只读寄存器，安全读取）
    uint32_t reg = DMA_Type->DMA_STAT_STR.DMA_Status;   
    uint32_t bit = 0;  // 改为uint32_t，避免截断

    // 3. 根据状态类型获取对应位（增加日志调试）
    switch(status_type)  // 推荐用switch，逻辑更清晰
    {
        case DMA_STATUS_TRANS_COMPLETE: // 传输完成（IFDx，bit0~5）
            bit = reg & 0x3F; //00111111
						bit = (bit >> dma_channle) & 0x01;
            break;
        
        case DMA_STATUS_DATA_ERROR:     // 数据错误（IFEx，bit8~13）
            bit = reg & 0x3F00;			//00111111 0000 0000
						bit = (bit >> (8 + dma_channle ) ) & 0x01; //111111 & 
            break;
        
        case DMA_STATUS_TRANS_FLAG:     // 传输标志（TFx，bit16~21）
            bit = reg & 0x3F0000;				//00111111 0000 0000 0000 0000
						bit = bit >> (16 + dma_channle) & 0x01;
            break;
				
				case DMA_STATUS_CONFIG_ERROR:     // 配置错误标志（TFx，bit31）           
						bit = (reg >> (31U)) & 0x01;
            break;
        
        default: // 无效状态类型（必加！）
            bit = 0;
            // 调试：打印错误值，定位问题
            INFO("Error: invalid status_type = %d\n", status_type);
            break;
    }
		return bit;
}

void HAL_DMA_NVIC_Enable(DMA_Channel_e dma_channle,dma_priority_t Priority)
{
	//设置dma数据传输中断允许使能
	HAL_DMA_IT_Transfer_Set(dma_channle,dma_it_enable);
	IRQn_Type IRQ_DMA = DMA_IRQn;
	NVIC_SetPriority(IRQ_DMA, Priority); /*TIMER13优先级配置 */
	NVIC_ClearPendingIRQ(IRQ_DMA);  /*清除之前的配置*/
	NVIC_EnableIRQ (IRQ_DMA);       /*使能中断*/
}

void HAL_DMA_NVIC_Disable(DMA_Channel_e dma_channle,dma_priority_t Priority)
{
	//设置dma数据传输中断允许使能
	HAL_DMA_IT_Transfer_Set(dma_channle,dma_it_disable);
	IRQn_Type IRQ_DMA = DMA_IRQn;
	NVIC_SetPriority(IRQ_DMA, Priority); /*TIMER13优先级配置 */
	NVIC_ClearPendingIRQ(IRQ_DMA);  /*清除之前的配置*/
	NVIC_DisableIRQ (IRQ_DMA);       /*使能中断*/
}




/******************************I2C  DMA使能***************/
/* 1. 使能/失能 I2C 发送 DMA */
void I2C_DMA_Tx_Enable(I2C_TypeDef *i2c_ptr, uint32_t enable)
{
    if(enable)
		{
        i2c_ptr->IC_DMA_CR.TDMAE = 1;  /* 使能发送 DMA */
		}
		else
		{
        i2c_ptr->IC_DMA_CR.TDMAE = 0;  /* 失能发送 DMA */
    }
    __DSB();
}

/* 2-> 使能/失能 I2C 接收 DMA */
void I2C_DMA_Rx_Enable(I2C_TypeDef *i2c_ptr, uint32_t enable)
{
    if(enable)
		{
        i2c_ptr->IC_DMA_CR.RDMAE = 1;  /* 使能接收 DMA */
		}
    else
		{
        i2c_ptr->IC_DMA_CR.RDMAE = 0;  /* 失能接收 DMA */
    }
    __DSB();
}

/* 3-> 同时配置发送和接收 DMA */
void I2C_DMA_Config(I2C_TypeDef *i2c_ptr, uint32_t tx_en, uint32_t rx_en)
{
    uint32_t reg_val = 0;
    
    if(tx_en) reg_val |= (1U << 1);  /* TDMAE */
    if(rx_en) reg_val |= (1U << 0);  /* RDMAE */
    
    i2c_ptr->IC_DMA_CR.w = reg_val;
    __DSB();
}


/* 4-> 获取当前 DMA 使能状态 */
uint32_t I2C_DMA_GetStatus(I2C_TypeDef *i2c_ptr)
{
    return i2c_ptr->IC_DMA_CR.w;
}

/* 1-> 配置发送 FIFO 阈值 */
void I2C_DMA_SetTxLevel(I2C_TypeDef *i2c_ptr, uint8_t level)
{
    if(level > 0xFF) return;  /* 8位寄存器 */
    
    i2c_ptr->IC_DMA_TDLR.DMATDL = level;
    __DSB();
    
//    INFO("I2C DMA TX level set to %d\n", level);
}

/* 2-> 配置接收 FIFO 阈值 */
void I2C_DMA_SetRxLevel(I2C_TypeDef *i2c_ptr, uint8_t level)
{
    if(level > 0xFF) return;
    
    i2c_ptr->IC_DMA_RDLR.DMARDL = level;
    __DSB();
    
    INFO("I2C DMA RX level set to %d\n", level);
}

/* 3-> 获取当前发送 FIFO 阈值 */
uint8_t I2C_DMA_GetTxLevel(I2C_TypeDef *i2c_ptr)
{
    return i2c_ptr->IC_DMA_TDLR.DMATDL;
}

/* 4-> 获取当前接收 FIFO 阈值 */
uint8_t I2C_DMA_GetRxLevel(I2C_TypeDef *i2c_ptr)
{
    return i2c_ptr->IC_DMA_RDLR.DMARDL;
}

 

/*  获取当前发送 FIFO 深度 */
uint8_t I2C_DMA_GetTxLevelDepth(I2C_TypeDef *i2c_ptr)
{
    return i2c_ptr->IC_COMP_PARAM_1.TX_BUFFER_DEPTH;
}

/*  获取当前接收 FIFO 深度 */
uint8_t I2C_DMA_GetRxLevelDepth(I2C_TypeDef *i2c_ptr)
{
    return i2c_ptr->IC_COMP_PARAM_1.RX_BUFFER_DEPTH;
}


void HAL_I2C_DMA_Tx_FullInit(I2C_TypeDef *i2c_ptr,DMA_Channel_e dma_channle,uint32_t txbuffer,uint16_t buffsize)
{
	uint8_t Level = 4;
	HAL_DMA_I2CTx_chx_Init(dma_channle, i2c_ptr,txbuffer, buffsize);  //dma i2c tx
	I2C_DMA_SetTxLevel(i2c_ptr,Level);			//配置i2c的fifo阈值 空闲空间大于阈值触发dma发送请求
	I2C_DMA_Tx_Enable(i2c_ptr,ENABLE);			//使能DMA  i2c发送
}

void HAL_DMA_StructerInit(dma_ch_config_t *dma_ch_config_set)
{
	DMA_div_srcAddr_Init(dma_ch_config_set->dma_ch,dma_ch_config_set->src_addr);
	DMA_div_dstAddr_Init(dma_ch_config_set->dma_ch,dma_ch_config_set->dst_addr);
	DMA_div_Maxlen_Set(dma_ch_config_set->dma_ch,dma_ch_config_set->transfer_size);
	DMA_div_TriggerSource_Config(dma_ch_config_set->dma_ch,dma_ch_config_set->DMA_Trigger_periph);
	DMA_div_SrcIncMode_Init(dma_ch_config_set->dma_ch,dma_ch_config_set->src_inc);
	DMA_div_DstIncMode_Init(dma_ch_config_set->dma_ch,dma_ch_config_set->dst_inc);
	DMA_div_DIR_Init(dma_ch_config_set->dma_ch,dma_ch_config_set->dir);
	DMA_div_mode_Init(dma_ch_config_set->dma_ch,dma_ch_config_set->circular_mode);
//	DMA_div_SBURST_Init(dma_ch_config_set->dma_ch,dma_ch_config_set->burst_len);
	DMA_div_DMAToDst_size_Init(dma_ch_config_set->dma_ch,dma_ch_config_set->dma_data_width_dst_width);
	DMA_div_SrcToDma_size_Init(dma_ch_config_set->dma_ch,dma_ch_config_set->dma_data_width_src_width);
	DMA_div_priority_Init(dma_ch_config_set->dma_ch,dma_ch_config_set->priority);
//	DMA_div_chx_Enable(dma_ch_config_set->dma_ch);
}




