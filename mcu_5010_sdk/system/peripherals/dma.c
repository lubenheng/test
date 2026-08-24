#include "platform_common.h"
#include "DMA_Reg.h"
#include "device_resource.h"
#include "DMA_Type.h"
#include "DMA.h"
#include "i2c_reg.h"
//CTRL   IE   IF   
//AHBP1总线


//寄存器控制  封装成函数   底层驱动
/**
 *@brief enable DMA chx
 *@param[in] DMA_chx DMA通道号
 *@return    在该位被置1前必须先设定好数据源寄存器，数据目的寄存器和数据长度寄存器。
 */
void DMA_div_chx_Enable(DMA_Channel_e DMA_chx)
{
	switch(DMA_chx)
	{
		case DMA_CHANNEL_0:   DMA_Type->DMA_CTL0_STR.Bits.CHEN = 1;//DMACH0_CTL.EN = 1;
			break;
		case DMA_CHANNEL_1:		DMA_CH1_ENABLE();//DMACH1_CTL.EN = 1;
			break;
		case DMA_CHANNEL_2:		DMA_CH2_ENABLE();//DMACH2_CTL.EN = 1;
			break;
		case DMA_CHANNEL_3:		DMA_CH3_ENABLE();//DMACH3_CTL.EN = 1;
			break;
		case DMA_CHANNEL_4:		DMA_CH4_ENABLE();//DMACH4_CTL.EN = 1;
			break;
		case DMA_CHANNEL_5:		DMA_CH5_ENABLE();//DMACH5_CTL.EN = 1;
			break;
		case DMA_CHANNEL_MAX: 
			DMA_CH0_ENABLE();
			DMA_CH1_ENABLE();
			DMA_CH2_ENABLE();
			DMA_CH3_ENABLE();
			DMA_CH4_ENABLE();
			DMA_CH5_ENABLE();
			break;
		default:break;
	}
}



/**
 *@brief disable DMA chx
 *@param[in] DMA_chx DMA通道号
 *@return    none
 */
void DMA_div_chx_Disable(DMA_Channel_e DMA_chx)
{
    // TODO: 实现DMA通道失能
    // 例如：DMA->CH[DMA_chx].CFG &= ~DMA_CH_ENABLE;
	switch(DMA_chx)
	{
		case DMA_CHANNEL_0:   DMA_CH0_DISABLE();//DMACH0_CTL.EN = 1;
			break;
		case DMA_CHANNEL_1:		DMA_CH1_DISABLE();//DMACH1_CTL.EN = 1;
			break;
		case DMA_CHANNEL_2:		DMA_CH2_DISABLE();//DMACH2_CTL.EN = 1;
			break;
		case DMA_CHANNEL_3:		DMA_CH3_DISABLE();//DMACH3_CTL.EN = 1;
			break;
		case DMA_CHANNEL_4:		DMA_CH4_DISABLE();//DMACH4_CTL.EN = 1;
			break;
		case DMA_CHANNEL_5:		DMA_CH5_DISABLE();//DMACH5_CTL.EN = 1;
			break;
		case DMA_CHANNEL_MAX: 
			DMA_CH0_DISABLE();
			DMA_CH1_DISABLE();
			DMA_CH2_DISABLE();
			DMA_CH3_DISABLE();
			DMA_CH4_DISABLE();
			DMA_CH5_DISABLE();
			break;
		default:break;
	}
}

/**
 *@brief 传输方向配置
 *@param[in] DMA_chx DMA通道号
 *@param[in] direction 传输方向（DMA_DIR_xxx）
 *@return    none
 */
void DMA_div_DIR_Init(DMA_Channel_e DMA_chx, dma_direction_t direction)
{
    if (DMA_chx > DMA_CHANNEL_5) return;
		if (direction > DMA_DIR_MEM_TO_MEM ) return ;
	
    switch (DMA_chx)
    {
        case DMA_CHANNEL_0:
            DMA_CH0_CTL->w &= ~(DMA_CTRL_MEM2MEM | DMA_CTRL_MEM2PER);
            if (direction == DMA_DIR_MEM_TO_MEM)
                DMA_CH0_SET_MEM2MEM();
            else if (direction == DMA_DIR_MEM_TO_PERIPH)
                DMA_CH0_SET_MEM2PER();
            else // DMA_DIR_PERIPH_TO_MEM
                DMA_CH0_SET_PER2MEM();
            break;
						
        case DMA_CHANNEL_1:
            DMA_CH1_CTL->w &= ~(DMA_CTRL_MEM2MEM | DMA_CTRL_MEM2PER);
            if (direction == DMA_DIR_MEM_TO_MEM)
                DMA_CH1_CTL->w |= DMA_CTRL_MEM2MEM;
            else if (direction == DMA_DIR_MEM_TO_PERIPH)
                DMA_CH1_CTL->w |= DMA_CTRL_MEM2PER;
            break;
        case DMA_CHANNEL_2:
            DMA_CH2_CTL->w &= ~(DMA_CTRL_MEM2MEM | DMA_CTRL_MEM2PER);
            if (direction == DMA_DIR_MEM_TO_MEM)
                DMA_CH2_CTL->w |= DMA_CTRL_MEM2MEM;
            else if (direction == DMA_DIR_MEM_TO_PERIPH)
                DMA_CH2_CTL->w |= DMA_CTRL_MEM2PER;
            break;
        case DMA_CHANNEL_3:
            DMA_CH3_CTL->w &= ~(DMA_CTRL_MEM2MEM | DMA_CTRL_MEM2PER);
            if (direction == DMA_DIR_MEM_TO_MEM)
                DMA_CH3_CTL->w |= DMA_CTRL_MEM2MEM;
            else if (direction == DMA_DIR_MEM_TO_PERIPH)
                DMA_CH3_CTL->w |= DMA_CTRL_MEM2PER;
            break;
        case DMA_CHANNEL_4:
            DMA_CH4_CTL->w &= ~(DMA_CTRL_MEM2MEM | DMA_CTRL_MEM2PER);
            if (direction == DMA_DIR_MEM_TO_MEM)
                DMA_CH4_CTL->w |= DMA_CTRL_MEM2MEM;
            else if (direction == DMA_DIR_MEM_TO_PERIPH)
                DMA_CH4_CTL->w |= DMA_CTRL_MEM2PER;
            break;
        case DMA_CHANNEL_5:
            DMA_CH5_CTL->w &= ~(DMA_CTRL_MEM2MEM | DMA_CTRL_MEM2PER);
            if (direction == DMA_DIR_MEM_TO_MEM)
                DMA_CH5_CTL->w |= DMA_CTRL_MEM2MEM;
            else if (direction == DMA_DIR_MEM_TO_PERIPH)
                DMA_CH5_CTL->w |= DMA_CTRL_MEM2PER;
            break;
        default:
            break;
    }
}



/**
 *@brief enable DMA clk
 *@param[in] none 
 *@return    none
 */
void DMA_div_clk_Enable(void)
{
//	module_clk_cfg_t module_clk_cfg;
//	uint32_t i2c_clk = 0u;
//	//i2c_clock_init
////	const module_name_e clock_module_name[I2C_INSTANCE_MAX_COUNT] = {MODULE_I2C0, MODULE_I2C1};

////	module_clk_cfg.module_name = clock_module_name[i2c_instance];

//	module_clk_cfg.fclk_sel = APB_CLK;
//	module_clk_cfg.fclk_div = PER_DIVIDE_1;
//	module_clk_cfg.pclk_dsleep_gating_en = GATING_ON;
//	module_clk_cfg.fclk_dsleep_gating_en = GATING_ON;

//	clock_module_clk_cfg(&module_clk_cfg);
//	clock_freq_get(module_clk_cfg.module_name, &i2c_clk);
}

/**
 *@brief disable DMA clk
 *@param[in] none 
 *@return    none
 */
void DMA_div_clk_Disable(void)
{
    // TODO: 实现DMA时钟失能
}

  




/**
 *@brief 源地址配置
 *@param[in] DMA_chx DMA通道号
 *@param[in] srcAddr 源地址
 *@return    none
 */
void DMA_div_srcAddr_Init(DMA_Channel_e DMA_chx, uint32_t srcAddr)
{
    if (DMA_chx > DMA_CHANNEL_5) return;

    switch (DMA_chx)
    {
        case DMA_CHANNEL_0: DMA_CH0_SRC = srcAddr; break;
        case DMA_CHANNEL_1: DMA_CH1_SRC = srcAddr; break;
        case DMA_CHANNEL_2: DMA_CH2_SRC = srcAddr; break;
        case DMA_CHANNEL_3: DMA_CH3_SRC = srcAddr; break;
        case DMA_CHANNEL_4: DMA_CH4_SRC = srcAddr; break;
        case DMA_CHANNEL_5: DMA_CH5_SRC = srcAddr; break;
        default: break;
    }
}



/**
 *@brief 目的地址配置
 *@param[in] DMA_chx DMA通道号
 *@param[in] dstAddr 目的地址
 *@return    none
 */
void DMA_div_dstAddr_Init(DMA_Channel_e DMA_chx, uint32_t dstAddr)
{
    // TODO: 实现目的地址配置  DMA_DESTx_STR
	
	if (DMA_chx > DMA_CHANNEL_5) return;

    switch (DMA_chx)
    {
        case DMA_CHANNEL_0: DMA_CH0_DEST = dstAddr; break;
        case DMA_CHANNEL_1: DMA_CH1_DEST = dstAddr; break;
        case DMA_CHANNEL_2: DMA_CH2_DEST = dstAddr; break;
        case DMA_CHANNEL_3: DMA_CH3_DEST = dstAddr; break;
        case DMA_CHANNEL_4: DMA_CH4_DEST = dstAddr; break;
        case DMA_CHANNEL_5: DMA_CH5_DEST = dstAddr; break;
        default: break;
    }
}


/**
 *@brief 传输次数配置
 *@param[in] DMA_chx DMA通道号
 *@param[in] transferCnt 传输次数
 *@return    none
 */
//void DMA_div_TransCnt_Init(DMA_Channel_e DMA_chx, uint32_t transferCnt)
//{
//    if (DMA_chx > DMA_CHANNEL_5) return;

//    switch (DMA_chx)  //DMA_TCNTx_STR
//    {
//        case DMA_CHANNEL_0: DMA_CH0_TCNT = transferCnt; break;
//        case DMA_CHANNEL_1: DMA_CH1_TCNT = transferCnt; break;
//        case DMA_CHANNEL_2: DMA_CH2_TCNT = transferCnt; break;
//        case DMA_CHANNEL_3: DMA_CH3_TCNT = transferCnt; break;
//        case DMA_CHANNEL_4: DMA_CH4_TCNT = transferCnt; break;
//        case DMA_CHANNEL_5: DMA_CH5_TCNT = transferCnt; break;
//        default: break;
//    }
//}



/**
 *@brief DMA到目的地址数据位宽配置
 *@param[in] DMA_chx DMA通道号
 *@param[in] Bitsizeout 外设数据位宽（如：8位、16位、32位）
 *@return    none
 */
void DMA_div_DMAToDst_size_Init(DMA_Channel_e DMA_chx, dma_data_width_t Bitsizeout)
{	
	//DMACHn_CTL.DSIZE
	if (DMA_chx > DMA_CHANNEL_5) return;

    uint32_t dsize_val = 0;
    switch (Bitsizeout)
    {
        case DMA_BITSIZE_8BIT:  dsize_val = DMA_CTRL_DESIZE_B;  break;//00
        case DMA_BITSIZE_16BIT: dsize_val = DMA_CTRL_DESIZE_HW; break;//01
        case DMA_BITSIZE_32BIT: dsize_val = DMA_CTRL_DESIZE_W;  break;//11
        default: return;
    }

    switch (DMA_chx)
    {
        case DMA_CHANNEL_0:
					DMA_CH0_CTL->w &= ~(DMA_CTRL_DESIZE_B | DMA_CTRL_DESIZE_HW | DMA_CTRL_DESIZE_W);  //清除CTL[8:7]
            DMA_CH0_CTL->w |= dsize_val;	//写1  
            break;
        case DMA_CHANNEL_1:
            DMA_CH1_CTL->w &= ~(DMA_CTRL_DESIZE_B | DMA_CTRL_DESIZE_HW | DMA_CTRL_DESIZE_W);
            DMA_CH1_CTL->w |= dsize_val;
            break;
        case DMA_CHANNEL_2:
            DMA_CH2_CTL->w &= ~(DMA_CTRL_DESIZE_B | DMA_CTRL_DESIZE_HW | DMA_CTRL_DESIZE_W);
            DMA_CH2_CTL->w |= dsize_val;
            break;
        case DMA_CHANNEL_3:
            DMA_CH3_CTL->w &= ~(DMA_CTRL_DESIZE_B | DMA_CTRL_DESIZE_HW | DMA_CTRL_DESIZE_W);
            DMA_CH3_CTL->w |= dsize_val;
            break;
        case DMA_CHANNEL_4:
            DMA_CH4_CTL->w &= ~(DMA_CTRL_DESIZE_B | DMA_CTRL_DESIZE_HW | DMA_CTRL_DESIZE_W);
            DMA_CH4_CTL->w |= dsize_val;
            break;
        case DMA_CHANNEL_5:
            DMA_CH5_CTL->w &= ~(DMA_CTRL_DESIZE_B | DMA_CTRL_DESIZE_HW | DMA_CTRL_DESIZE_W);
            DMA_CH5_CTL->w |= dsize_val;
            break;
        default: break;
    }
}



/**
 *@brief 数据源到DMA控制器数据位宽配置
 *@param[in] DMA_chx DMA通道号
 *@param[in] Bitsizein 内存数据位宽（如：8位、16位、32位）
 *@return    none
 */
void DMA_div_SrcToDma_size_Init(DMA_Channel_e DMA_chx, dma_data_width_t Bitsizein)
{
    // TODO: 实现内存数据位宽配置
	if (DMA_chx > DMA_CHANNEL_5) return;

    uint32_t ssize_val = 0;
    switch (Bitsizein)
    {
        case DMA_BITSIZE_8BIT:  ssize_val = DMA_CTRL_SOSIZE_B;  break;
        case DMA_BITSIZE_16BIT: ssize_val = DMA_CTRL_SOSIZE_HW; break;
        case DMA_BITSIZE_32BIT: ssize_val = DMA_CTRL_SOSIZE_W;  break;
        default: return;
    }

    switch (DMA_chx)
    {
        case DMA_CHANNEL_0:
						DMA_CH0_CTL->w &= ~(DMA_CTRL_SOSIZE_B | DMA_CTRL_SOSIZE_HW | DMA_CTRL_SOSIZE_W);//清除CTL_SSIZE[6:5]
            DMA_CH0_CTL->w |= ssize_val;
            break;
        case DMA_CHANNEL_1:
            DMA_CH1_CTL->w &= ~(DMA_CTRL_SOSIZE_B | DMA_CTRL_SOSIZE_HW | DMA_CTRL_SOSIZE_W);
            DMA_CH1_CTL->w |= ssize_val;
            break;
        case DMA_CHANNEL_2:
            DMA_CH2_CTL->w &= ~(DMA_CTRL_SOSIZE_B | DMA_CTRL_SOSIZE_HW | DMA_CTRL_SOSIZE_W);
            DMA_CH2_CTL->w |= ssize_val;
            break;
        case DMA_CHANNEL_3:
            DMA_CH3_CTL->w &= ~(DMA_CTRL_SOSIZE_B | DMA_CTRL_SOSIZE_HW | DMA_CTRL_SOSIZE_W);
            DMA_CH3_CTL->w |= ssize_val;
            break;
        case DMA_CHANNEL_4:
            DMA_CH4_CTL->w &= ~(DMA_CTRL_SOSIZE_B | DMA_CTRL_SOSIZE_HW | DMA_CTRL_SOSIZE_W);
            DMA_CH4_CTL->w |= ssize_val;
            break;
        case DMA_CHANNEL_5:
            DMA_CH5_CTL->w &= ~(DMA_CTRL_SOSIZE_B | DMA_CTRL_SOSIZE_HW | DMA_CTRL_SOSIZE_W);
            DMA_CH5_CTL->w |= ssize_val;
            break;
        default: break;
    }
}

/**
 *@brief 源地址递增方式配置
 *@param[in] DMA_chx DMA通道号
 *@param[in] periphInc 源地址递增方式（使能/失能）1 设置递增
 *@return    none
 */
void DMA_div_SrcIncMode_Init(DMA_Channel_e DMA_chx, dma_addr_mode_t periphInc)
{
	//DMACHn_CTL.SINC
	if (DMA_chx > DMA_CHANNEL_5) return;

    switch (DMA_chx)
    {
        case DMA_CHANNEL_0:
            periphInc ? DMA_CH0_SET_SRC_INC() : DMA_CH0_CLR_SRC_INC();
            break;
        case DMA_CHANNEL_1:
            periphInc ? (DMA_CH1_CTL->w |= DMA_CTRL_SOINC) : (DMA_CH1_CTL->w &= ~DMA_CTRL_SOINC);
            break;
        case DMA_CHANNEL_2:
            periphInc ? (DMA_CH2_CTL->w |= DMA_CTRL_SOINC) : (DMA_CH2_CTL->w &= ~DMA_CTRL_SOINC);
            break;
        case DMA_CHANNEL_3:
            periphInc ? (DMA_CH3_CTL->w |= DMA_CTRL_SOINC) : (DMA_CH3_CTL->w &= ~DMA_CTRL_SOINC);
            break;
        case DMA_CHANNEL_4:
            periphInc ? (DMA_CH4_CTL->w |= DMA_CTRL_SOINC) : (DMA_CH4_CTL->w &= ~DMA_CTRL_SOINC);
            break;
        case DMA_CHANNEL_5:
            periphInc ? (DMA_CH5_CTL->w |= DMA_CTRL_SOINC) : (DMA_CH5_CTL->w &= ~DMA_CTRL_SOINC);
            break;
        default: break;
    }
}

/**
 *@brief 目的地址递增方式配置
 *@param[in] DMA_chx DMA通道号
 *@param[in] memInc 内存地址递增方式（使能/失能）
 *@return    none
 */
void DMA_div_DstIncMode_Init(DMA_Channel_e DMA_chx, dma_addr_mode_t dstInc)
{
	//DMACHn_CTL的DINC
	if (DMA_chx > DMA_CHANNEL_5) return;

    switch (DMA_chx)
    {
        case DMA_CHANNEL_0:
            dstInc ? DMA_CH0_SET_DEST_INC() : DMA_CH0_CLR_DEST_INC();
            break;
        case DMA_CHANNEL_1:
            dstInc ? (DMA_CH1_CTL->w |= DMA_CTRL_DEINC) : (DMA_CH1_CTL->w &= ~DMA_CTRL_DEINC);
            break;
        case DMA_CHANNEL_2:
            dstInc ? (DMA_CH2_CTL->w |= DMA_CTRL_DEINC) : (DMA_CH2_CTL->w &= ~DMA_CTRL_DEINC);
            break;
        case DMA_CHANNEL_3:
            dstInc ? (DMA_CH3_CTL->w |= DMA_CTRL_DEINC) : (DMA_CH3_CTL->w &= ~DMA_CTRL_DEINC);
            break;
        case DMA_CHANNEL_4:
            dstInc ? (DMA_CH4_CTL->w |= DMA_CTRL_DEINC) : (DMA_CH4_CTL->w &= ~DMA_CTRL_DEINC);
            break;
        case DMA_CHANNEL_5:
            dstInc ? (DMA_CH5_CTL->w |= DMA_CTRL_DEINC) : (DMA_CH5_CTL->w &= ~DMA_CTRL_DEINC);
            break;
        default: break;
    }
}


/**
 *@brief 传输模式配置
 *@param[in] DMA_chx DMA通道号
 *@param[in] mode 传输模式（如：单次传输、循环传输等）
 *@return    none
 */
void DMA_div_mode_Init(DMA_Channel_e DMA_chx, dma_mode_t mode)
{
	//自动重装控制寄存器 PDMACHn_CTL 中的自动重载控制位 RT被置位时
	if (DMA_chx > DMA_CHANNEL_5) return;
    // RT位控制自动重装（循环模式）
    uint32_t rt_val = (mode == DMA_MODE_CIRCULAR) ? 1 : 0;

    switch (DMA_chx)
    {
        case DMA_CHANNEL_0:
            DMA_CH0_CTL->Bits.RT = rt_val;
						
            break;
        case DMA_CHANNEL_1:
            DMA_CH1_CTL->Bits.RT = rt_val;
            break;
        case DMA_CHANNEL_2:
            DMA_CH2_CTL->Bits.RT = rt_val;
            break;
        case DMA_CHANNEL_3:
            DMA_CH3_CTL->Bits.RT = rt_val;
            break;
        case DMA_CHANNEL_4:
            DMA_CH4_CTL->Bits.RT = rt_val;
            break;
        case DMA_CHANNEL_5:
            DMA_CH5_CTL->Bits.RT = rt_val;
            break;
        default: break;
    }
		
}



/**
 * @brief DMA通道优先级配置
通道空闲状态下进行
DMACHn_CTL.PL
 */
status_t DMA_div_priority_Init(DMA_Channel_e DMA_chx, dma_priority_t priority)
{
	if(DMA_chx > DMA_CHANNEL_5)
	{
			return STATUS_PARAM_ERROR;
	}
	uint32_t pl_val = 0;
	switch (priority)
	{
			case DMA_PRIORITY_LOW:    pl_val = DMA_CTRL_PL_LOW;    break;//[9:10]
			case DMA_PRIORITY_MID:    pl_val = DMA_CTRL_PL_MID;    break;
			case DMA_PRIORITY_HIGH:   pl_val = DMA_CTRL_PL_HIGH;   break;
			case DMA_PRIORITY_HIGHER: pl_val = DMA_CTRL_PL_HIGHER; break;
			default: return STATUS_PARAM_ERROR;
	}
	switch(DMA_chx)
	{
			case DMA_CHANNEL_0:
					DMA_CH0_CTL->w &= ~(DMA_CTRL_PL_LOW | DMA_CTRL_PL_MID | DMA_CTRL_PL_HIGH | DMA_CTRL_PL_HIGHER);
					DMA_CH0_CTL->w |= pl_val;
					break;
			case DMA_CHANNEL_1:
					DMA_CH1_CTL->w &= ~(DMA_CTRL_PL_LOW | DMA_CTRL_PL_MID | DMA_CTRL_PL_HIGH | DMA_CTRL_PL_HIGHER);
					DMA_CH1_CTL->w |= pl_val;
					break;
			case DMA_CHANNEL_2:
					DMA_CH2_CTL->w &= ~(DMA_CTRL_PL_LOW | DMA_CTRL_PL_MID | DMA_CTRL_PL_HIGH | DMA_CTRL_PL_HIGHER);
					DMA_CH2_CTL->w |= pl_val;
					break;
			case DMA_CHANNEL_3:
					DMA_CH3_CTL->w &= ~(DMA_CTRL_PL_LOW | DMA_CTRL_PL_MID | DMA_CTRL_PL_HIGH | DMA_CTRL_PL_HIGHER);
					DMA_CH3_CTL->w |= pl_val;
					break;
			case DMA_CHANNEL_4:
					DMA_CH4_CTL->w &= ~(DMA_CTRL_PL_LOW | DMA_CTRL_PL_MID | DMA_CTRL_PL_HIGH | DMA_CTRL_PL_HIGHER);
					DMA_CH4_CTL->w |= pl_val;
					break;
			case DMA_CHANNEL_5:
					DMA_CH5_CTL->w &= ~(DMA_CTRL_PL_LOW | DMA_CTRL_PL_MID | DMA_CTRL_PL_HIGH | DMA_CTRL_PL_HIGHER);
					DMA_CH5_CTL->w |= pl_val;
					break;
			default:break;
	}
	return STATUS_SUCCESS;
	
}


 
/**
* @brief 检查DMA通道是否忙碌  1忙  DMA_STAT[16:21]
 */
uint8_t DMA_div_IsBusy(DMA_Channel_e DMA_chx)
{
	  if(DMA_chx > DMA_CHANNEL_5)
    {
        return 0;
    }
		
    switch(DMA_chx)
    {
        case DMA_CHANNEL_0: //[16]
            return  DMA_Type->DMA_STAT_STR.DMA_Status = DMA_FLAG_ACT0;
        case DMA_CHANNEL_1:
            return  DMA_Type->DMA_STAT_STR.DMA_Status = DMA_FLAG_ACT1;
        case DMA_CHANNEL_2:
            return  DMA_Type->DMA_STAT_STR.DMA_Status = DMA_FLAG_ACT2;
        case DMA_CHANNEL_3:
            return  DMA_Type->DMA_STAT_STR.DMA_Status = DMA_FLAG_ACT3;
        case DMA_CHANNEL_4:
            return  DMA_Type->DMA_STAT_STR.DMA_Status = DMA_FLAG_ACT4;
        case DMA_CHANNEL_5:
            return  DMA_Type->DMA_STAT_STR.DMA_Status = DMA_FLAG_ACT5;
        default: return 0;
    }
}

	

/**
 * @brief 复位DMA通道
 */
void DMA_div_chx_Reset(uint32_t DMA_chx)
{
	
}

/**
 * @brief DMA通道去初始化
 * @param dma_ctrl DMA控制器号
 * @param dma_ch DMA通道号
 */
void DMA_div_Ch_DeInit( uint32_t DMA_chx)
{
	//将所有设置恢复复位状态
}

/**
 *@brief 软件触发DMA传输（内存到内存模式必需）
 *@param[in] DMA_chx DMA通道号
 *@return    none
 */
void DMA_div_SoftwareTrigger(DMA_Channel_e DMA_chx)
{
	// 配置为内存到内存模式
    DMA_div_DIR_Init(DMA_chx, DMA_DIR_MEM_TO_MEM);
    // 使能通道触发传输
    DMA_div_chx_Enable(DMA_chx);
	//配置成 mtm 使能就开启软件触发
}

/**
 *@brief 清除软件触发
 *@param[in] DMA_chx DMA通道号
 *@return    none
 */
void DMA_div_ClearSoftwareTrigger(DMA_Channel_e DMA_chx)
{
	if (DMA_chx > DMA_CHANNEL_5) return;

    // 失能通道停止传输
    DMA_div_chx_Disable(DMA_chx);
}

 

/**
 *@brief 等待通道传输完成
 *@param[in] dma_ch DMA通道号
 *@param[in] timeout_ms 超时时间
 *@return    0:完成 -1:超时
 */
int32_t DMA_WaitComplete(DMA_Channel_e DMA_chx, uint32_t timeout_ms)
{	
    if(DMA_chx > DMA_CHANNEL_5)
    {
        return -1;
    }
		uint32_t timeout_cnt = 0;
    uint32_t timeout_max = timeout_ms * 1000; // 假设1ms=1000个循环单位
		 // 等待传输完成或超时
    while (DMA_div_IsBusy(DMA_chx))
    {
        if (timeout_cnt++ >= timeout_max)
        {
            return -1; // 超时
        }
    }
    return 0;  /* 传输完成 */
}

/**
 * @brief 配置DMA通道触发源
 * @param[in] DMA_chx DMA通道号
 * @param[in] trigger 触发源
 * @return none
 */
void DMA_div_TriggerSource_Config(DMA_Channel_e DMA_chx,DMA_Trigger_TypeDef dma_trigger)
{	 
	if (DMA_chx > DMA_CHANNEL_5) return;

    // 通过CHCFG寄存器绑定外设触发源
    switch (DMA_chx)
    {
        case DMA_CHANNEL_0:
						DMA_Type->DMA_CHCFG1_STR.Bits.CH0SEL = dma_trigger;
            break;
        case DMA_CHANNEL_1:
						DMA_Type->DMA_CHCFG1_STR.Bits.CH1SEL = dma_trigger;
            break;
        case DMA_CHANNEL_2:
						DMA_Type->DMA_CHCFG1_STR.Bits.CH2SEL = dma_trigger;
            break;
        case DMA_CHANNEL_3:
					DMA_Type->DMA_CHCFG1_STR.Bits.CH3SEL = dma_trigger;
            break;
        case DMA_CHANNEL_4:
						DMA_Type->DMA_CHCFG2_STR.Bits.CH4SEL = dma_trigger;
            break;
        case DMA_CHANNEL_5:
						DMA_Type->DMA_CHCFG2_STR.Bits.CH5SEL = dma_trigger;
            break;
        default: break;
    }
}



 


void DMA_IT_Enable(DMA_Channel_e DMA_chx)
{
	if (DMA_chx > DMA_CHANNEL_5) return;

    // 使能对应通道传输中断（DMA_MASK寄存器）
    switch (DMA_chx)
    {
        case DMA_CHANNEL_0: DMA_CH0_ENABLE_INT(); break;
        case DMA_CHANNEL_1: DMA_CH1_ENABLE_INT(); break;
        case DMA_CHANNEL_2: DMA_CH2_ENABLE_INT(); break;
        case DMA_CHANNEL_3: DMA_CH3_ENABLE_INT(); break;
        case DMA_CHANNEL_4: DMA_CH4_ENABLE_INT(); break;
        case DMA_CHANNEL_5: DMA_CH5_ENABLE_INT(); break;
        default: break;
    }
}

void DMA_IT_Disable(DMA_Channel_e DMA_chx)
{
	//DMA_CLR	DMA中断清除寄存器
	if (DMA_chx > DMA_CHANNEL_5) return;

    // 使能对应通道传输中断（DMA_MASK寄存器）
    switch (DMA_chx)
    {
        case DMA_CHANNEL_0: DMA_CH0_DISABLE_INT(); break;
        case DMA_CHANNEL_1: DMA_CH1_DISABLE_INT(); break;
        case DMA_CHANNEL_2: DMA_CH2_DISABLE_INT(); break;
        case DMA_CHANNEL_3: DMA_CH3_DISABLE_INT(); break;
        case DMA_CHANNEL_4: DMA_CH4_DISABLE_INT(); break;
        case DMA_CHANNEL_5: DMA_CH5_DISABLE_INT(); break;
        default: break;
    }
}


uint32_t DMA_Read_SrcAddr(DMA_Channel_e DMA_chx)
{
//	DMACHn_CTL.SCUR
//	return DMAx->DMACHn_CTL.SCUR
	if (DMA_chx > DMA_CHANNEL_5) return 0;

    // 读取当前源地址（SoCurr寄存器）
    switch (DMA_chx)
    {
        case DMA_CHANNEL_0: return DMA_CH0_SOCURR;
        case DMA_CHANNEL_1: return DMA_CH1_SOCURR;
        case DMA_CHANNEL_2: return DMA_CH2_SOCURR;
        case DMA_CHANNEL_3: return DMA_CH3_SOCURR;
        case DMA_CHANNEL_4: return DMA_CH4_SOCURR;
        case DMA_CHANNEL_5: return DMA_CH5_SOCURR;
        default: return 0;
    }
}

uint32_t DMA_Read_DstAddr(DMA_Channel_e DMA_chx)
{
//	DMACHn_CTL.DCUR
//	return DMAx->DMACHn_CTL.DCUR
	if (DMA_chx > DMA_CHANNEL_5) return 0;

    // 读取当前源地址（DCUR寄存器）
    switch (DMA_chx)
    {
        case DMA_CHANNEL_0: return DMA_CH0_DECURR;
        case DMA_CHANNEL_1: return DMA_CH1_DECURR;
        case DMA_CHANNEL_2: return DMA_CH2_DECURR;
        case DMA_CHANNEL_3: return DMA_CH3_DECURR;
        case DMA_CHANNEL_4: return DMA_CH4_DECURR;
        case DMA_CHANNEL_5: return DMA_CH5_DECURR;
        default: return 0;
    }
}

uint16_t DMA_Read_Cnt(DMA_Channel_e DMA_chx)
{
  //	return DMAx->DMACHn_CTL.TCNT
	if (DMA_chx > DMA_CHANNEL_5) return 0;

    // 读取当前源地址（DCUR寄存器）
    switch (DMA_chx)
    {
        case DMA_CHANNEL_0: return DMA_Type->DMA_TCNT0_STR.DMA_Tcnt0;
        case DMA_CHANNEL_1: return DMA_CH1_TCNT;
        case DMA_CHANNEL_2: return DMA_CH2_TCNT;
        case DMA_CHANNEL_3: return DMA_CH3_TCNT;
        case DMA_CHANNEL_4: return DMA_CH4_TCNT;
        case DMA_CHANNEL_5: return DMA_CH5_TCNT;
        default: return 0;
    }
}

void DMA_Clr_ITerror(DMA_Channel_e DMA_chx)
{
	//DMA_CLR
	//数据错误中断CME  配置错误中断CEn  数据传输中断CDn
	if (DMA_chx > DMA_CHANNEL_5) return;

    // 清除对应通道中断错误标志（写1清除）
    switch (DMA_chx)
    {
        case DMA_CHANNEL_0: DMA_CH0_CLEAR_FLAG(); break;//DMA_CLR |= DMA_CLEAR0;
        case DMA_CHANNEL_1: DMA_CLR |= DMA_CLEAR1; break;
        case DMA_CHANNEL_2: DMA_CLR |= DMA_CLEAR2; break;
        case DMA_CHANNEL_3: DMA_CLR |= DMA_CLEAR3; break;
        case DMA_CHANNEL_4: DMA_CLR |= DMA_CLEAR4; break;
        case DMA_CHANNEL_5: DMA_CLR |= DMA_CLEAR5; break;
        default: break;
    }
}
 

//突发数据长度
void DMA_div_SBURST_Init(DMA_Channel_e DMA_chx, uint32_t burst)
{
	 /* 参数检查 */
    if(DMA_chx > 5)
    {
        INFO("Error: Invalid DMA channel %u\n", DMA_chx);
        return;
    }

    if(burst > 3)
    {
        INFO("Error: Invalid SBURST value %u (must be 0-3)\n", burst);
        return;
    }
		#define DMA_CTL_SBURST_Pos     5U   /* SBURST位偏移 */
    #define DMA_CTL_SBURST_Msk     (0x3UL << DMA_CTL_SBURST_Pos)  /* 位掩码 */
		/* 读取当前控制寄存器值 */
    uint32_t ctl_val = DMA_Type->DMA_CTL0_STR.Bits.SBURST;
    
    /* 清除SBURST位域 */
    ctl_val &= ~DMA_CTL_SBURST_Msk;
    
    /* 设置新的SBURST值 */
    ctl_val |= (burst << DMA_CTL_SBURST_Pos) & DMA_CTL_SBURST_Msk;
    
    /* 写回控制寄存器 */
    DMA_Type->DMA_CTL0_STR.Bits.SBURST = ctl_val;
    
    /* 数据同步屏障 */
    __DSB();
	//DMACHn_CTL  SBURST
}

//最大传输数据长度
void DMA_div_Maxlen_Set(DMA_Channel_e DMA_chx, uint16_t len)
{
	if(DMA_chx > 5)
	{
			INFO("Error: Invalid DMA channel %u\n", DMA_chx);
			return;
	}
	switch (DMA_chx)
	{
			case DMA_CHANNEL_0: DMA_Type->DMA_MAX0_STR.DMA_Max0 = len; break;//DMA_CLR |= DMA_CLEAR0;
			case DMA_CHANNEL_1: DMA_Type->DMA_MAX1_STR.DMA_Max1 = len; break;
			case DMA_CHANNEL_2: DMA_Type->DMA_MAX2_STR.DMA_Max2 = len; break;
			case DMA_CHANNEL_3: DMA_Type->DMA_MAX3_STR.DMA_Max3 = len; break;
			case DMA_CHANNEL_4: DMA_Type->DMA_MAX4_STR.DMA_Max4 = len; break;
			case DMA_CHANNEL_5: DMA_Type->DMA_MAX5_STR.DMA_Max5 = len; break;
			default: break;
	}
}


/**
 * @brief 清除所有触发源恢复默认，防止配置错误
 * @param[in] DMA_chx DMA通道号
 * @param[in] trigger 触发源
 * @return none
 */
void DMA_div_TriggerSource_Clean(void)
{	 
	DMA_Type->DMA_CHCFG1_STR.w = 0x03020100U;
  DMA_Type->DMA_CHCFG2_STR.w = 0x03020504U;  //恢复默认
}

