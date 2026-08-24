#include "platform_common.h"

#include "test.h"

#include "dma_reg.h"
#include "dma_hal.h"
#include "dma_test.h"
#include "cmdline_internal.h"
#include "dma.h"
#include "ODS_5010.h"
#include "i2c_reg.h"
#include "i2c.h"
#include "i2c_test.h"
#include "dma_cmdline.h"

#include "mcpwm_type.h"
#include "mcpwm.h"
#include "test.h"

#include "spi_reg.h"
mcpwm_shadow  stru_mcpwm_shadow1; 
extern uint8_t Rx_buf[bufsize];
I2C_TypeDef *I2C_0  = I2C0;
I2C_TypeDef *I2C_1  = I2C1;
uint32_t recbuff[20] ;
uint32_t sendbuff[20] ;

uint8_t dma_trans_complete = 0;

//mtm
void DMA_IT_M_TO_M_text(DMA_Channel_e dma_channle)
{
	volatile uint8_t cnt= 0;
	for(cnt=0;cnt<20;cnt++)
	{
		sendbuff[cnt] = cnt + 1;
	}
	dma_ch_config_t dma_handle;
 
	/* 2. 填充DMA配置结构体 */
	dma_handle.dma_ch = dma_channle;
	dma_handle.circular_mode = DMA_MODE_NORMAL;           
	dma_handle.dir = DMA_DIR_MEM_TO_MEM;               
	dma_handle.burst_len = DMA_BURST_1_UNIT;              
	/* 3. 地址配置 */
	dma_handle.src_addr = (uint32_t)sendbuff;        
	dma_handle.dst_addr = (uint32_t)recbuff; 					
	
	/* 4. 地址递增模式 */
	dma_handle.src_inc = Addr_Enable;               		  
	dma_handle.dst_inc = Addr_Enable;                  
	
	/* 5. 数据宽度配置 */
	dma_handle.dma_data_width_src_width = DMA_BITSIZE_32BIT;  /* 源数据宽度32位 */
	dma_handle.dma_data_width_dst_width = DMA_BITSIZE_32BIT;  /* 目的数据宽度32位 */

	/* 7. 优先级配置 */
	dma_handle.priority = DMA_PRIORITY_MID;
	
	/* 8. 传输大小 */
	dma_handle.transfer_size = 20;
	
	/* 9. 先失能通道（配置前）*/
	DMA_div_chx_Disable(dma_handle.dma_ch);
	
	/* 10. 配置DMA寄存器 */
	DMA_div_srcAddr_Init(dma_handle.dma_ch, dma_handle.src_addr);
	DMA_div_dstAddr_Init(dma_handle.dma_ch, dma_handle.dst_addr);
	#if 1  //测试配置错误中断
	DMA_div_Maxlen_Set(dma_handle.dma_ch,dma_handle.transfer_size);
#endif
//	DMA_div_TriggerSource_Config(dma_handle.dma_ch, dma_handle.DMA_Trigger_periph);
	DMA_div_SrcIncMode_Init(dma_handle.dma_ch, dma_handle.src_inc);
	DMA_div_DstIncMode_Init(dma_handle.dma_ch, dma_handle.dst_inc);
	DMA_div_DIR_Init(dma_handle.dma_ch, dma_handle.dir);
	DMA_div_mode_Init(dma_handle.dma_ch, dma_handle.circular_mode);
	DMA_div_SBURST_Init(dma_handle.dma_ch, dma_handle.burst_len);
	DMA_div_DMAToDst_size_Init(dma_handle.dma_ch, dma_handle.dma_data_width_dst_width);
	DMA_div_SrcToDma_size_Init(dma_handle.dma_ch, dma_handle.dma_data_width_src_width);
	DMA_div_priority_Init(dma_handle.dma_ch, dma_handle.priority);
	
	HAL_DMA_NVIC_Enable(dma_handle.dma_ch, dma_handle.priority);
	//设置配置错误中断，传输数据完成中断  数据错误中断
	HAL_DMA_IT_Transfer_Set(dma_channle,dma_it_enable);  //6通道任意2通道关联相同触发源触发配置中断
	HAL_DMA_IT_TransError_Set(dma_channle,dma_it_enable);//最大传输大小没设置触发
	HAL_DMA_IT_Configerror_Set(dma_it_enable);
	
	IRQn_Type IRQ_DMA = DMA_IRQn;
	NVIC_SetPriority(IRQ_DMA, dma_handle.priority); /*TIMER13优先级配置 */
	NVIC_ClearPendingIRQ(IRQ_DMA);  /*清除之前的配置*/
	NVIC_EnableIRQ (IRQ_DMA);       /*使能中断*/
	
	
	/* 12. 使能DMA通道（准备就绪，等待I2C触发）*/
	DMA_div_chx_Enable(dma_handle.dma_ch);
	
	INFO("DMA Channel %d initialized for MEM-to-MEM TX (size: %d)\n", 
           dma_channle, dma_handle.transfer_size);
  
	for(cnt=0;cnt<20;cnt++)
	{
		INFO("M_TO_M recbuff[%d] = %d ",cnt ,recbuff[cnt] );
		if(cnt%5==0)	INFO("\n");
		
	}
}


void DMA_IT_I2C_Tx_text(DMA_Channel_e dma_channle,uint32_t slave_addr,uint8_t i2c_speed,uint16_t *buf, uint16_t len)
{ 
	dma_ch_config_t dma_handle;
	i2c0_gpio_init();
	I2C_MspInit(slave_addr,i2c_speed);  //从机0x4E   写入寄存器0x00
	I2C_0->IC_ENABLE.w = 0;
     
    
    // 3. 设置从设备地址
	I2C_0->IC_TAR.IC_TAR = 0x01;
	I2C_0->IC_ENABLE.w = 1;
	/* 2. 填充DMA配置结构体 */
	dma_handle.dma_ch = dma_channle;
	dma_handle.circular_mode = DMA_MODE_NORMAL;           
	dma_handle.dir = DMA_DIR_MEM_TO_PERIPH; 
	dma_handle.DMA_Trigger_periph	= DMA_Trigger_I2C0_TX;//内存--->i2c数据寄存器-->发送到从机
	dma_handle.burst_len = DMA_BURST_1_UNIT;              
	/* 3. 地址配置 */
	dma_handle.src_addr = (uint32_t)buf; 
//	uint32_t reg = (uint32_t)&I2C_0->IC_DATA_CMD.DAT;
	dma_handle.dst_addr = (uint32_t)&(I2C_0->IC_DATA_CMD); //I2C0的数据寄存器 					
	
	/* 4. 地址递增模式 */
	dma_handle.src_inc = Addr_Enable;               		  
	dma_handle.dst_inc = Addr_Disable;                  
	
	/* 5. 数据宽度配置（I2C通常是8位）*/
	dma_handle.dma_data_width_src_width = DMA_BITSIZE_16BIT;  /* 源数据宽度16位 */
	dma_handle.dma_data_width_dst_width = DMA_BITSIZE_16BIT;  /* 目的数据宽度16位 */

	/* 7. 优先级配置 */
	dma_handle.priority = DMA_PRIORITY_MID;
	dma_handle.priority_it = 1;
	/* 8. 传输大小 */
	dma_handle.transfer_size = len;
	
	/* 9. 先失能通道（配置前）*/
	DMA_div_chx_Disable(dma_handle.dma_ch);
	
	/* 10. 配置DMA寄存器 */
	DMA_div_srcAddr_Init(dma_handle.dma_ch, dma_handle.src_addr);
	DMA_div_dstAddr_Init(dma_handle.dma_ch, dma_handle.dst_addr);
	#if 1  //测试配置错误中断
	DMA_div_Maxlen_Set(dma_handle.dma_ch,dma_handle.transfer_size);
#endif

 
	DMA_div_TriggerSource_Config(dma_handle.dma_ch, dma_handle.DMA_Trigger_periph);
	
	DMA_div_SrcIncMode_Init(dma_handle.dma_ch, dma_handle.src_inc);
	DMA_div_DstIncMode_Init(dma_handle.dma_ch, dma_handle.dst_inc);
	DMA_div_DIR_Init(dma_handle.dma_ch, dma_handle.dir);
	DMA_div_mode_Init(dma_handle.dma_ch, dma_handle.circular_mode);
	DMA_div_SBURST_Init(dma_handle.dma_ch, dma_handle.burst_len);			//配置内存与外设需要配置突发数据长度
	DMA_div_DMAToDst_size_Init(dma_handle.dma_ch, dma_handle.dma_data_width_dst_width);
	DMA_div_SrcToDma_size_Init(dma_handle.dma_ch, dma_handle.dma_data_width_src_width);
	DMA_div_priority_Init(dma_handle.dma_ch, dma_handle.priority);//通道优先级
	
	HAL_DMA_NVIC_Enable(dma_handle.dma_ch, dma_handle.priority);
																						//设置配置错误中断，传输数据完成中断  数据错误中断
	HAL_DMA_IT_Transfer_Set(dma_channle,dma_it_enable);  //6通道任意2通道关联相同触发源触发配置中断
	HAL_DMA_IT_TransError_Set(dma_channle,dma_it_enable);//最大传输大小没设置触发
	HAL_DMA_IT_Configerror_Set(dma_it_enable);
	
	IRQn_Type IRQ_DMA = DMA_IRQn;
	NVIC_SetPriority(IRQ_DMA, dma_handle.priority_it); /*中断优先级 */
	NVIC_ClearPendingIRQ(IRQ_DMA);  /*清除之前的配置*/
	NVIC_EnableIRQ (IRQ_DMA);       /*使能中断*/
	
	

	
	
	I2C_DMA_Tx_Enable(I2C_0,1);			//使能i2c的dma发送请求
	I2C_DMA_SetTxLevel(I2C_0,4);			//当 I2C TX FIFO 的空闲空间 ≥ 4 时，会触发 DMA 发送请求
	/* 12. 使能DMA通道（准备就绪，等待I2C触发）*/
	
 
	DMA_div_chx_Enable(dma_handle.dma_ch);//启动dma，
 
	
	
	
	
	while(HAL_DMA_GetRemainingCnt(dma_handle.dma_ch) > 0)
	{
		#ifdef timeout
		static int time_out = 1000;
		if(time_out-- %50== 0)
		{
			while( (I2C_0->IC_STATUS.TFNF) == 0);
			INFO("tx time error\n");
			
			while( (I2C_0->IC_STATUS.TFNF) == 0);
			if(time_out <= 0)	 return ;
		}
		#endif
	}
  //内存---TxFIFO---DMA---I2Cdata_cmd---从机寄存器
	INFO("DMA Channel %d initialized for M-to-P I2C TX (size: %d)\n", 
           dma_channle, dma_handle.transfer_size);
}


void DMA_IT_I2C_Rx_text(DMA_Channel_e dma_channle,uint32_t slave_addr,uint8_t i2c_speed, uint8_t *buf, uint16_t len)
{
	i2c0_gpio_init();
	I2C_MspInit(slave_addr,i2c_speed);  //从机0x4E   写入寄存器0x00
	dma_ch_config_t dma_handle;
	
	/* 2. 填充DMA配置结构体 */
	dma_handle.dma_ch = dma_channle;
	dma_handle.circular_mode = DMA_MODE_NORMAL;           
	dma_handle.dir = DMA_DIR_PERIPH_TO_MEM; 
	dma_handle.DMA_Trigger_periph	= DMA_Trigger_I2C0_RX;//内存--->i2c数据寄存器-->发送到从机
	dma_handle.burst_len = DMA_BURST_1_UNIT;              
	/* 3. 地址配置 */
	dma_handle.dst_addr = (uint32_t)Rx_buf; 
	dma_handle.src_addr = (uint32_t)&(I2C_0->IC_DATA_CMD); //I2C0的数据寄存器 					
	
	/* 4. 地址递增模式 */
	dma_handle.src_inc = Addr_Disable;               		  
	dma_handle.dst_inc = Addr_Enable;                  
	
	/* 5. 数据宽度配置（I2C通常是8位）*/
	dma_handle.dma_data_width_src_width = DMA_BITSIZE_8BIT;  /* 源数据宽度8位 */
	dma_handle.dma_data_width_dst_width = DMA_BITSIZE_8BIT;  /* 目的数据宽度8位 */

	/* 7. 优先级配置 */
	dma_handle.priority = DMA_PRIORITY_MID;
	dma_handle.priority_it = 0;
	/* 8. 传输大小 */
	dma_handle.transfer_size = len;
	
	/* 9. 先失能通道（配置前）*/
	DMA_div_chx_Disable(dma_handle.dma_ch);
	
	/* 10. 配置DMA寄存器 */
	DMA_div_srcAddr_Init(dma_handle.dma_ch, dma_handle.src_addr);
	DMA_div_dstAddr_Init(dma_handle.dma_ch, dma_handle.dst_addr);
	#if 1  //测试配置错误中断
	DMA_div_Maxlen_Set(dma_handle.dma_ch,dma_handle.transfer_size);
#endif

 
	DMA_div_TriggerSource_Config(dma_handle.dma_ch, dma_handle.DMA_Trigger_periph);
	
	DMA_div_SrcIncMode_Init(dma_handle.dma_ch, dma_handle.src_inc);
	DMA_div_DstIncMode_Init(dma_handle.dma_ch, dma_handle.dst_inc);
	DMA_div_DIR_Init(dma_handle.dma_ch, dma_handle.dir);
	DMA_div_mode_Init(dma_handle.dma_ch, dma_handle.circular_mode);
	DMA_div_SBURST_Init(dma_handle.dma_ch, dma_handle.burst_len);			//配置内存与外设需要配置突发数据长度
	DMA_div_DMAToDst_size_Init(dma_handle.dma_ch, dma_handle.dma_data_width_dst_width);
	DMA_div_SrcToDma_size_Init(dma_handle.dma_ch, dma_handle.dma_data_width_src_width);
	DMA_div_priority_Init(dma_handle.dma_ch, dma_handle.priority);//通道优先级
	
	HAL_DMA_NVIC_Enable(dma_handle.dma_ch, dma_handle.priority);
//设置配置错误中断，传输数据完成中断  数据错误中断
	HAL_DMA_IT_Transfer_Set(dma_channle,dma_it_enable);  //6通道任意2通道关联相同触发源触发配置中断
	HAL_DMA_IT_TransError_Set(dma_channle,dma_it_enable);//最大传输大小没设置触发
	HAL_DMA_IT_Configerror_Set(dma_it_enable);
	
	IRQn_Type IRQ_DMA = DMA_IRQn;
	NVIC_SetPriority(IRQ_DMA, dma_handle.priority_it); /*中断优先级 */
	NVIC_ClearPendingIRQ(IRQ_DMA);  /*清除之前的配置*/
	NVIC_EnableIRQ (IRQ_DMA);       /*使能中断*/
	
  I2C_DMA_SetRxLevel(I2C_0,0);		//  
	I2C_DMA_Rx_Enable(I2C_0,4);
	
	
	/* 12. 使能DMA通道（准备就绪，等待I2C触发）*/
	
	// 1. 写寄存器地址（CMD=0，仅传地址，不带读指令）

	DMA_div_chx_Enable(dma_handle.dma_ch);
	I2C_0->IC_DATA_CMD.w = ((uint32_t)I2C_MASTER_CMD_WRITE << 8U) + 0x4F;
  
  for(int i=0; i<len; i++) 
	{
			// 确认TX FIFO未满（TFNF=1）等待dma从fifo搬走   读命令过txfifo
			I2C_0->IC_DATA_CMD.CMD = 1; // 读命令
			while( I2C_0->IC_STATUS.TFNF == 0 ); 
			while( I2C_0->IC_STATUS.RFF == 1 );// 短暂延迟，给DMA时间搬运，释放FIFO空间
			
	}
 
 
	
	while(HAL_DMA_GetRemainingCnt(dma_handle.dma_ch) > 0)
	{
		#ifdef timeout
		static int time_out = 1000;
		if(time_out-- %100== 0)
			{
				INFO("time error\n");
				while( I2C_0->IC_STATUS.TFNF == 0 ); 
				if(time_out<0) break;			
			}
		#endif
	}
 
	INFO("DMA Channel %d initialized for P-to-M I2C RX (size: %d)\n", 
           dma_channle, dma_handle.transfer_size);
}


/**
 * @brief  I2C主机模式初始化，开启DMA功能
 */
void I2C_MspInit(uint32_t slave_addr,uint8_t i2c_speed)
{
    // 1. 关闭I2C，清空配置（先关后配，避免误操作）
    I2C_0->IC_ENABLE.w = 0;
    I2C_0->IC_CON.w = 0;
    
    // 2. 配置I2C核心参数：主机模式、7bit地址、重启使能
    I2C_0->IC_CON.MASTER_MODE = 1;    // 主机模式
    I2C_0->IC_CON.SPEED = i2c_speed; // 速率模式
    I2C_0->IC_CON.IC_10BITADDR_MASTER = 0; // 7bit地址模式
    I2C_0->IC_CON.IC_RESTART_EN = 1;  // 重启使能（支持连续传输）
    I2C_0->IC_CON.IC_SLAVE_DISABLE = 1; // 禁用从模式
    
    // 3. 设置从设备地址
    I2C_0->IC_TAR.IC_TAR = slave_addr;
    
    // 4. 配置SCL高低电平计数（根据PCLK和速率计算，核心！）
		// 正确公式：总周期 = PCLK / 400000 = 80个时钟周期
    // HCNT = 总周期/3 - 4 ≈ 22，LCNT = 总周期*2/3 - 4 ≈ 49
		float T_scl = 0.0f;      // SCL时钟周期（单位：ns）
    float T_hight = 0.0f;    // 高电平时间（单位：ns）
    float T_low = 0.0f;      // 低电平时间（单位：ns）
    float HCNT = 0.0f;       // 高电平计数值
    float LCNT = 0.0f;       // 低电平计数值
    float ic_clk = 0.0f;     // 单个I2C时钟周期（单位：ns）
    float baudrate = 0.0f;
		float h_fix = 0;
		float l_fix = 0;
    ic_clk = (1.0 / I2C_PCLK) * 1000000000.0;
    
		if(i2c_speed) 
		{
			baudrate = 300000UL; //400K DW I2C可能不支持，数据出错
//			HCNT = 0x19;
//			LCNT = 0x23;
		}
		else 
		{
			baudrate = 100000UL;			
		}
		T_scl = 1000000000.0 / (float)baudrate;
		T_hight = T_scl / 2.0;
		T_low = T_hight;
		HCNT = (T_hight - 100.0) / ic_clk - h_fix;
		LCNT = (T_low - 200.0) / ic_clk + l_fix;
		
		
    I2C_0->IC_SS_SCL_HCNT = (uint32_t)HCNT;//h=0x19,l=0x23
    I2C_0->IC_SS_SCL_LCNT = (uint32_t)LCNT;
    I2C_0->IC_FS_SCL_HCNT = (uint32_t)HCNT;
    I2C_0->IC_FS_SCL_LCNT = (uint32_t)LCNT;
    I2C_0->IC_HS_SCL_HCNT = (uint32_t)HCNT;
    I2C_0->IC_HS_SCL_LCNT = (uint32_t)LCNT;
 
//    
    // 6. 开启I2C的DMA收发使能（关键寄存器：IC_DMA_CR）
    I2C_0->IC_DMA_CR.RDMAE = 1; // 开启RX DMA
    I2C_0->IC_DMA_CR.TDMAE = 1; // 开启TX DMA
 
		
    // 7. 使能I2C
    I2C_0->IC_ENABLE.ENABLE = 1;
    // 等待I2C使能完成
    while(I2C_0->IC_ENABLE_STATUS.IC_EN == 0);
}

/**
 * @brief  I2C DMA写数据（主机->从机）
 * @param  reg: 从机内部寄存器地址
 * @param  buf: 发送缓冲区
 * @param  len: 发送长度
 */


void I2C_DMA_SendData(DMA_Channel_e ch,uint8_t reg,uint8_t cmd, uint16_t *buf, uint16_t len)
{
	volatile uint16_t i = 0;
	static uint16_t dma_buf_tx[bufsize+1];   //static防止栈溢出
	dma_buf_tx[0] = ((uint16_t)I2C_MASTER_CMD_WRITE << 8U) + reg;  //第一位数据写寄存器地址
	
	for(i=1;i<len+1;i++)   //961出问题
	{
		dma_buf_tx[i] = ((uint16_t)I2C_MASTER_CMD_WRITE << 8U) + (uint16_t)(i%256);  //填充数据 带写命令
	}
 
	if(0 == cmd) // 写操作：主机->从机
	{
		DMA_div_chx_Disable(ch);
		DMA_div_srcAddr_Init(ch, (uint32_t)dma_buf_tx);
		DMA_div_dstAddr_Init(ch, (uint32_t)&(I2C_0->IC_DATA_CMD));
		DMA_div_Maxlen_Set(ch, len);
		DMA_div_DMAToDst_size_Init(ch, DMA_BITSIZE_16BIT);
		DMA_div_SrcToDma_size_Init(ch, DMA_BITSIZE_16BIT);
			
		while((I2C_0->IC_STATUS.w & (1 << 1)) == 0); 	
		HAL_DMA_chx_Enable(ch);  //不能先开在设置buff
		
		
		while(HAL_DMA_GetRemainingCnt(ch) > 0)
		{
			#ifdef timeout
			static int time_out = 1000;
			if(time_out-- %100== 0)
			{
				INFO("time error\n");
				while( I2C_0->IC_STATUS.TFNF == 0 ); 
				if(time_out<0) break;			
			}
			#endif
		}
		
	}   
}

void I2C_DMA_RecData(DMA_Channel_e ch,uint8_t reg,uint8_t cmd, uint8_t *buf, uint16_t len)
{
	volatile uint16_t i = 0;
	 
  //提前初始化方向
	if(1 == cmd)  
	{
		HAL_DMA_chx_Disable(ch);  
		DMA_div_srcAddr_Init(ch, (uint32_t)&(I2C_0->IC_DATA_CMD));
		DMA_div_dstAddr_Init(ch, (uint32_t)buf); //存
		DMA_div_Maxlen_Set(ch, len);//64
		HAL_DMA_chx_Enable(ch); //开启dma，外设到内存读取到buf
		
		I2C_0->IC_DATA_CMD.w = ((uint32_t)I2C_MASTER_CMD_WRITE << 8U) + reg;  //写入要读取的寄存器地址
		
		for( i=0; i<len; i++) 
		{
			// 确认TX FIFO未满（TFNF=1）等待dma从fifo搬走
			I2C_0->IC_DATA_CMD.CMD = 1; // 读命令
			while((I2C_0->IC_STATUS.w & (1 << 1)) == 0); 
 
			while( I2C_0->IC_STATUS.RFF == 1 );
		}
		
		while(HAL_DMA_GetRemainingCnt(ch) > 0)
		{
			#ifdef timeout
			static int time_out = 10000;
			if(time_out-- %100== 0)
			{
				INFO("time error\n");
				while( I2C_0->IC_STATUS.TFNF == 0 ); 
				if(time_out<0) break;			
			}
		#endif
		}   
	}
}
 

/**********************************************************************
 * @brief  DMA + MCPWM 动态更新配置（单次/循环）
 * @param  dma_channel: DMA 通道号
 * @param  RE_DMA: MCPWM 触发源 
 * @param  Rt: DMA 模式 单次/循环
 * @param  dst_addrs: 目标地址（占空比/死区/周期）
 * @param  buf: 数据缓冲区
 * @param  len: 传输长度
 * @note   禁止多通道绑定同一触发源；每次调用会清空旧配置重新初始化
 **********************************************************************/
void DMA_IT_MCPWM_Config(DMA_Channel_e dma_channel,MCPWM_DMA_CH MCPWM_CH ,MCPWM_RE_DMA RE_DMA,dma_mode_t Rt,MCPWM_DMA_Dst dst_addrs,const uint16_t *buf, uint16_t len)
{		
		//MCPWM已经main初始化，只需要改变th的值,启动对应的DMA使能RE
	  MCPWM0->PRT = (uint32_t ) 0xDEAD;  //解除写保护
		MCPWM0->RE = 0;		//关闭所有DMA使能
		MCPWM0->RE |= (1U << (uint32_t)(RE_DMA) );			 //选择DMA请求使能
		MCPWM0->PRT = (uint32_t ) 0xAAAA;  //写保护
	
    // 1. 定义DMA配置结构体（局部变量初始化，避免脏数据）
    dma_ch_config_t dma_handle;   
		
    /* ====================== 核心参数配置 ====================== */
    // 2. 基础通道与模式配置
    dma_handle.dma_ch              = dma_channel;          // 指定DMA通道
    dma_handle.circular_mode       = Rt;      // 普通模式（非循环）
    dma_handle.dir                 = DMA_DIR_MEM_TO_PERIPH;// 传输方向：内存→外设
    dma_handle.DMA_Trigger_periph  = DMA_Trigger_MCPWM_BCO;// MCPWM 时基0计数器溢出事件  CNT到最大清零瞬间触发DMA搬数据到x,更新pwm参数  
    dma_handle.burst_len           = DMA_BURST_1_UNIT;     // 突发传输长度：1个单元

    // 3. 地址配置（核心：补全MCPWM目标寄存器，此处以TH00为例）
    dma_handle.src_addr            = (uint32_t)buf;        // 源地址：用户传入的缓冲区
		if(dst_addrs == dst_addrs_duty)
		{
			// MCPWM DMA修改占空比初始化
			DMA_MCPWM_Change_duty_Init();
			if(MCPWM_CH == MCPWM_DMA_CH0)
				dma_handle.dst_addr = (uint32_t)&MCPWM0->TH01;  // CH0 占空比寄存器
			else if(MCPWM_CH == MCPWM_DMA_CH1)
				dma_handle.dst_addr = (uint32_t)&MCPWM0->TH11;  // CH1 占空比寄存器
			else if(MCPWM_CH == MCPWM_DMA_CH2)
				dma_handle.dst_addr = (uint32_t)&MCPWM0->TH21;  // CH2 占空比寄存器   
		}
		else if(dst_addrs == dst_addrs_deadtime)
		{
			
			// MCPWM DMA修改死区时间初始化0-32us为周期
			DMA_MCPWM_Change_deadtime_Init();
			
			dma_handle.dst_addr = (uint32_t)&MCPWM0->DTH00;// N  3通道共享
		}
 
    
		
		// 示例1：搬运到CH0_P比较寄存器（动态占空比）TH01 TH00  [0%,100%]->[0,32768]  无保护
		// 示例2：搬运到死区寄存器（动态死区)				DeadTimeCH012P	 8us*deadtime			保护
		// 示例3：搬运到时基周期寄存器（动态周期）	TH0					15位无符号0-32.768Khz  保护
		
		
    // 4. 地址递增模式（内存自增，外设固定）
    dma_handle.src_inc             = Addr_Enable;          // 源地址自增（遍历缓冲区）
    dma_handle.dst_inc             = Addr_Disable;         // 目的地址固定（仅写TH00寄存器）

    // 5. 数据宽度配置（匹配MCPWM 32位寄存器）
    dma_handle.dma_data_width_src_width = DMA_BITSIZE_16BIT; // 源数据宽度：16位
    dma_handle.dma_data_width_dst_width = DMA_BITSIZE_16BIT; // 目的数据宽度：16位

    // 6. 优先级配置（中等优先级，避免抢占核心中断）
    dma_handle.priority            = DMA_PRIORITY_MID;     // DMA通道优先级：中等
    dma_handle.priority_it         = 1;                    // 中断优先级：数值越小优先级越高

    // 7. 传输长度配置（单位：32位数据个数）
    dma_handle.transfer_size       = len;                  // 单次传输数据长度

    /* ====================== 硬件初始化 ====================== */
    // 8. 配置前先失能DMA通道（防止配置过程中误触发）
    DMA_div_chx_Disable(dma_handle.dma_ch);

    DMA_div_TriggerSource_Clean();											//触发源恢复默认状态
		
		// 9. 加载DMA配置参数到硬件寄存器
    HAL_DMA_StructerInit(&dma_handle);

    // 10. 使能DMA中断（NVIC层）
    HAL_DMA_NVIC_Enable(dma_handle.dma_ch, dma_handle.priority);

    // 11. 配置DMA中断类型（传输完成/传输错误/配置错误）
    HAL_DMA_IT_Transfer_Set(dma_channel, dma_it_enable);  // 传输完成中断使能
    HAL_DMA_IT_TransError_Set(dma_channel, dma_it_enable);// 传输错误中断使能
    HAL_DMA_IT_Configerror_Set(dma_it_enable);            // 配置错误中断使能

    // 12. 配置DMA全局中断（NVIC优先级+使能）
    IRQn_Type IRQ_DMA = DMA_IRQn;                         // DMA全局中断号
    NVIC_SetPriority(IRQ_DMA, dma_handle.priority_it);    // 设置中断优先级
    NVIC_ClearPendingIRQ(IRQ_DMA);                        // 清除挂起中断标志
    NVIC_EnableIRQ(IRQ_DMA);         

		// 使能DMA全局中断
		
		MCPWM0->PRT = (uint32_t ) 0xDEAD;  //解除写保护 确保DMA能改变频率和死区
    // 13. 使能DMA通道（配置完成，等待MCPWM触发）
			DMA_div_chx_Enable(dma_handle.dma_ch);
	
		// 读取当前中断标志
		uint32_t reg = MCPWM0->IF0;

		// 判断溢出标志位
		if (reg & 0x01U) 
		{
			static int i = 0;
			i++;
			INFO("mcpwm it\n");
			// 写1清零溢出标志位
			MCPWM0->IF0 = (1U << 0U); 
		}
		uint32_t cnt =0;
		while(HAL_DMA_GetRemainingCnt(dma_handle.dma_ch) > 0)
		{			 
			if(cnt++ > 0xFFFFFU)
			{
				INFO("\n DMA_ch1 time error--->>cnt = %d\n",HAL_DMA_GetRemainingCnt(dma_handle.dma_ch) );
				cnt = 0;
				return ;
			}
			;
		}
		MCPWM0->PRT = (uint32_t ) 0xAAAA;  //写保护
		
		
    // 14. 打印初始化信息 
    INFO("DMA Channel %d initialized for MCPWM (Mem-to-Periph, size: %d * 32bit)\n", 
         dma_channel, dma_handle.transfer_size);
}



/**
  * @brief  配置DMA触发MCPWM更新频率（TH0/TH00/TH01）
  * @param  dma_channel: DMA通道号 DMA_Channel_0~5
  * @param  MCPWM_CH: MCPWM的通道，THx01
  * @param  RE_DMA: MCPWM触发DMA请求源（T0/T1/TMR0等事件）
  * @param  Rt: DMA模式（一次性/循环）
  * @retval 无
  */
void DMA_IT_MCPWM_SetFreq_Config(DMA_Channel_e dma_channel,
                                  MCPWM_DMA_CH MCPWM_CH,
																	const uint16_t *buf, 
                                  dma_mode_t Rt)
{		
	  MCPWM0->PRT = (uint32_t ) 0xDEAD;  //解除写保护
		MCPWM0->RE = 0;		//关闭所有DMA使能
		MCPWM0->RE  =  0xFFFFFFFF;		 
		MCPWM0->PRT = (uint32_t ) 0xAAAA;   
 
    dma_ch_config_t dma_handle;   
		
 
    // 2. 基础通道与模式配置
    dma_handle.dma_ch              = dma_channel;          // 指定DMA通道
    dma_handle.circular_mode       = Rt;      // 普通模式（非循环）
    dma_handle.dir                 = DMA_DIR_MEM_TO_PERIPH;// 传输方向：内存→外设
    dma_handle.DMA_Trigger_periph  = DMA_Trigger_MCPWM_BCO;// MCPWM 时基0计数器溢出事件  CNT到最大清零瞬间触发DMA搬数据到x,更新pwm参数  
    dma_handle.burst_len           = DMA_BURST_1_UNIT;     // 突发传输长度：1个单元

    // 3. 地址配置（核心：补全MCPWM目标寄存器，此处以TH00为例）
    dma_handle.src_addr            = (uint32_t)buf  ;        		 
		dma_handle.dst_addr						 = (uint32_t)&MCPWM0->TH0;  
		
		DMA_MCPWM_Change_cycle_Init();
		
		delay_ms(1000); 
 	
    // 4. 地址递增模式（内存自增，外设固定）
    dma_handle.src_inc             = Addr_Enable;           
    dma_handle.dst_inc             = Addr_Disable;          

    // 5. 数据宽度配置（匹配MCPWM 32位寄存器）
    dma_handle.dma_data_width_src_width = DMA_BITSIZE_16BIT; // 源数据宽度：16位
    dma_handle.dma_data_width_dst_width = DMA_BITSIZE_16BIT; // 目的数据宽度：16位

    // 6. 优先级配置（中等优先级，避免抢占核心中断）
    dma_handle.priority            = DMA_PRIORITY_MID;     // DMA通道优先级：中等
    dma_handle.priority_it         = 1;                    // 中断优先级：数值越小优先级越高

    // 7. 传输长度配置 
    dma_handle.transfer_size       = 1;                  // 单次传输数据长度

    /* ====================== 硬件初始化 ====================== */
    // 8. 配置前先失能DMA通道（防止配置过程中误触发）
    DMA_div_chx_Disable(dma_handle.dma_ch);

    DMA_div_TriggerSource_Clean();											//触发源恢复默认状态
		
		// 9. 加载DMA配置参数到硬件寄存器
    HAL_DMA_StructerInit(&dma_handle);

    // 10. 使能DMA中断（NVIC层）
    HAL_DMA_NVIC_Enable(dma_handle.dma_ch, dma_handle.priority);

    // 11. 配置DMA中断类型（传输完成/传输错误/配置错误）
    HAL_DMA_IT_Transfer_Set(dma_channel, dma_it_enable);  // 传输完成中断使能
    HAL_DMA_IT_TransError_Set(dma_channel, dma_it_enable);// 传输错误中断使能
    HAL_DMA_IT_Configerror_Set(dma_it_enable);            // 配置错误中断使能

    // 12. 配置DMA全局中断（NVIC优先级+使能）
    IRQn_Type IRQ_DMA = DMA_IRQn;                         // DMA全局中断号
    NVIC_SetPriority(IRQ_DMA, dma_handle.priority_it);    // 设置中断优先级
    NVIC_ClearPendingIRQ(IRQ_DMA);                        // 清除挂起中断标志
    NVIC_EnableIRQ(IRQ_DMA);         
 
		
		MCPWM0->PRT = (uint32_t ) 0xDEAD;  //解除写保护 确保DMA能改变频率和死区
		// 13. 使能DMA通道（配置完成，等待MCPWM触发）
		DMA_div_chx_Enable(dma_handle.dma_ch);
 		
		// 读取当前中断标志
		uint32_t reg = MCPWM0->IF0;

		// 判断溢出标志位
		if (reg & 0x01U) 
		{
			static int i = 0;
			i++;
			INFO("mcpwm it\n");
			// 写1清零溢出标志位
			MCPWM0->IF0 = (1U << 0U); 
		}
		uint32_t cnt =0;
		while(HAL_DMA_GetRemainingCnt(dma_handle.dma_ch) > 0)
		{			 
			if(cnt++ > 0xFFFFFU)
			{
				INFO("\n DMA_ch1 time error--->>cnt = %d\n",HAL_DMA_GetRemainingCnt(dma_handle.dma_ch) );
				cnt = 0;
				return ;
			}
			;
		}
		MCPWM0->PRT = (uint32_t ) 0xAAAA;  //写保护
 		
    // 14. 打印初始化信息 
    INFO("DMA Channel %d initialized for MCPWM (Mem-to-Periph, size: %d * 32bit)\n", 
         dma_channel, dma_handle.transfer_size);
}


/**
 * @brief  DMA 内存到内存传输配置 (Memory to Memory)
 * @note   用于软件触发 DMA 将一段内存数据复制到另一段内存
 *         无需外设触发，纯内存拷贝，替代手动 memcpy
 */
void DMA_IT_MCPWM_MemToMem_Config(DMA_Channel_e dma_channel,
																	dma_mode_t Rt,
																	const uint16_t *Txbuf, 
																  uint16_t *Rxbuf,uint16_t len)
{
		dma_ch_config_t dma_handle;   
		MCPWM0->PRT = (uint32_t ) 0xDEAD;  //解除写保护
		MCPWM0->RE = 0;		//关闭所有DMA使能
		MCPWM0->RE  =  0x0000FFFF;	
//RE====bit8--bit15--控制DMA_Trigger_MCPWM_BC1触发源
//bit0---bit7----控制DMA_Trigger_MCPWM_BCO触发源		
		MCPWM0->PRT = (uint32_t ) 0xAAAA;  
		
    // 2. 基础通道与模式配置
    dma_handle.dma_ch              = dma_channel;           // 指定DMA通道
    dma_handle.circular_mode       = Rt;      							// 普通模式（非循环）
    dma_handle.dir                 = DMA_DIR_MEM_TO_PERIPH; // 传输方向：内存→外设
    dma_handle.DMA_Trigger_periph  = DMA_Trigger_MCPWM_BCO; 
    dma_handle.burst_len           = DMA_BURST_1_UNIT;      // 突发传输长度：1个单元

    // 3. 地址配置（核心：补全MCPWM目标寄存器，此处以TH00为例）
    dma_handle.src_addr            = (uint32_t)Txbuf;        		 
		dma_handle.dst_addr						 = (uint32_t)Rxbuf;  
		
		DMA_MCPWM_Change_cycle_Init();			////初始化mcpwm为500hz，50占空比，死区0
 
 	
    // 4. 地址递增模式（内存自增，外设固定）
    dma_handle.src_inc             = Addr_Enable;           
    dma_handle.dst_inc             = Addr_Enable;          

    // 5. 数据宽度配置（匹配MCPWM 32位寄存器）
    dma_handle.dma_data_width_src_width = DMA_BITSIZE_16BIT; // 源数据宽度：16位
    dma_handle.dma_data_width_dst_width = DMA_BITSIZE_16BIT; // 目的数据宽度：16位

    // 6. 优先级配置（中等优先级，避免抢占核心中断）
    dma_handle.priority            = DMA_PRIORITY_MID;     // DMA通道优先级：中等
    dma_handle.priority_it         = 1;                    // 中断优先级：数值越小优先级越高

    // 7. 传输长度配置 
    dma_handle.transfer_size       = len;                  // 单次传输数据长度

    /* ====================== 硬件初始化 ====================== */
    // 8. 配置前先失能DMA通道（防止配置过程中误触发）
    DMA_div_chx_Disable(dma_handle.dma_ch);

    DMA_div_TriggerSource_Clean();												 //触发源恢复默认状态
		
		// 9. 加载DMA配置参数到硬件寄存器
    HAL_DMA_StructerInit(&dma_handle);

    // 10. 使能DMA中断（NVIC层）
    HAL_DMA_NVIC_Enable(dma_handle.dma_ch, dma_handle.priority);

    // 11. 配置DMA中断类型（传输完成/传输错误/配置错误）
    HAL_DMA_IT_Transfer_Set(dma_handle.dma_ch, dma_it_enable);  // 传输完成中断使能
    HAL_DMA_IT_TransError_Set(dma_handle.dma_ch, dma_it_enable);// 传输错误中断使能
    HAL_DMA_IT_Configerror_Set(dma_it_enable);            // 配置错误中断使能

    // 12. 配置DMA全局中断（NVIC优先级+使能）
    IRQn_Type IRQ_DMA = DMA_IRQn;                         // DMA全局中断号
    NVIC_SetPriority(IRQ_DMA, dma_handle.priority_it);    // 设置中断优先级
    NVIC_ClearPendingIRQ(IRQ_DMA);                        // 清除挂起中断标志
    NVIC_EnableIRQ(IRQ_DMA);         

		// 13. 使能DMA通道（配置完成，等待MCPWM触发）
		DMA_div_chx_Enable(dma_handle.dma_ch);
 		
		// 读取当前中断标志
		uint32_t reg = MCPWM0->IF0;

		// 判断溢出标志位
		if (reg & 0x01U) 
		{
			static int i = 0;
			i++;
			INFO("mcpwm it\n");
			// 写1清零溢出标志位
			MCPWM0->IF0 = (1U << 0U); 
		}
		uint32_t cnt =0;
		while(HAL_DMA_GetRemainingCnt(dma_handle.dma_ch) > 0)
		{			 
			if(cnt++ > 0xFFFFFU)
			{
				INFO("\n DMA_ch1 time error--->>cnt = %d\n",HAL_DMA_GetRemainingCnt(dma_handle.dma_ch) );
				cnt = 0;
				return ;
			}
			;
		}
 		
    // 14. 打印初始化信息 
    INFO("DMA Channel %d initialized for MCPWM (Mem-to-Mem, size: %d * 32bit)\n", 
         dma_channel, dma_handle.transfer_size);
}


//初始化mcpwm为500hz，50占空比，TH01为零---CH0_N
void DMA_MCPWM_Change_duty_Init(void)
{
	uint32_t stru_mcpwm_shadow_period_temp;
	
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */
	
	/* 图 14-7MCPWM 时序 TH<n>0 和 TH<n>1-中心对齐模式 */
	MCPWM0_IO01 = CLEAR_BIT(MCPWM0_IO01,BIT7);   //complement mode		通道0选择互补模式
	MCPWM0_IO01 = CLEAR_BIT(MCPWM0_IO01,BIT15);  //complement mode
	MCPWM0_IO23 = CLEAR_BIT(MCPWM0_IO23,BIT7);   //complement mode
	MCPWM0_IO23 = CLEAR_BIT(MCPWM0_IO23,BIT15);  //complement mode
	MCPWM0_IO45 = CLEAR_BIT(MCPWM0_IO45,BIT7);   //complement mode
	MCPWM0_IO45 = CLEAR_BIT(MCPWM0_IO45,BIT15);  //complement mode
	MCPWM0_PP   = 0x00; /* normal mode */
	
	stru_mcpwm_shadow1.TH0 = 500;//设置100hz初始化，不进行分频
	if(stru_mcpwm_shadow1.TH0 == 0U)
	{
		INFO("wrong pwm period\n");
		return;
	}
	else
	{
		stru_mcpwm_shadow_period_temp = (MCU_CLK>>1)/stru_mcpwm_shadow1.TH0;
	}
	
	if(stru_mcpwm_shadow_period_temp > 65535U) /* 当频率太低时将MCPWM的时钟进行8分频 */
	{
		MCPWM0_TCLK |= (BIT0 + BIT1);
		stru_mcpwm_shadow1.TH0 = stru_mcpwm_shadow_period_temp>>3;
	}
	else
	{
		MCPWM0_TCLK &= ~(BIT0 + BIT1);
		stru_mcpwm_shadow1.TH0 = stru_mcpwm_shadow_period_temp;    //频率固定500hz
	} 
		/* mcpwm0 compare parameter */
		stru_mcpwm_shadow1.TH00 = (int16_t)(( -32768* stru_mcpwm_shadow1.TH0 + 16384) >> 15);
		stru_mcpwm_shadow1.TH01 = ( (0*stru_mcpwm_shadow1.TH0 + 16384) >> 15); 
		//初始化为50占空比，后续DMA改变
	
		stru_mcpwm_shadow1.TH10 = (int16_t)(( -32768* stru_mcpwm_shadow1.TH0 + 16384) >> 15);; //P
		stru_mcpwm_shadow1.TH11 = ( (0*stru_mcpwm_shadow1.TH0 + 16384) >> 15);								//N
		stru_mcpwm_shadow1.TH20 = (int16_t)(( -32768* stru_mcpwm_shadow1.TH0 + 16384) >> 15);;
		stru_mcpwm_shadow1.TH21 = ( (0*stru_mcpwm_shadow1.TH0 + 16384) >> 15);
		
		
		/* mcpwm0/1 deadtime parameter */
		stru_mcpwm_shadow1.DeadTimeCH012P = 0;
		stru_mcpwm_shadow1.DeadTimeCH012N = 0;
		
		/*************************************************************************/
		#if 1
		MCPWM0_TH0 = stru_mcpwm_shadow1.TH0 %32768;    //不能比32768还大

		MCPWM0_TH00 = stru_mcpwm_shadow1.TH00;   //TH00 + TH01 ÷2 ÷ 32768等于占空比
		MCPWM0_TH01 = stru_mcpwm_shadow1.TH01;
		MCPWM0_TH10 = stru_mcpwm_shadow1.TH10;
		MCPWM0_TH11 = stru_mcpwm_shadow1.TH11;
		MCPWM0_TH20 = stru_mcpwm_shadow1.TH20;
		MCPWM0_TH21 = stru_mcpwm_shadow1.TH21;
		
		MCPWM0_DTH00 = stru_mcpwm_shadow1.DeadTimeCH012P; // MCPWM CH0/1/2 P 通道死区宽度设置为0
		MCPWM0_DTH01 = stru_mcpwm_shadow1.DeadTimeCH012N; // MCPWM CH0/1/2 N 通道死区宽度设置为0
		#endif
		/*************************************************************************/
		MCPWM0_PRT = 0x000FFFFF; /*enter password to unlock write protection */
}

//初始化mcpwm为500hz，50占空比，死区0
void DMA_MCPWM_Change_deadtime_Init(void)
{
	uint32_t stru_mcpwm_shadow_period_temp;
	
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */
	
	/* 图 14-7MCPWM 时序 TH<n>0 和 TH<n>1-中心对齐模式 */
	MCPWM0_IO01 = CLEAR_BIT(MCPWM0_IO01,BIT7);   //complement mode		通道0选择互补模式
	MCPWM0_IO01 = CLEAR_BIT(MCPWM0_IO01,BIT15);  //complement mode
	MCPWM0_IO23 = CLEAR_BIT(MCPWM0_IO23,BIT7);   //complement mode
	MCPWM0_IO23 = CLEAR_BIT(MCPWM0_IO23,BIT15);  //complement mode
	MCPWM0_IO45 = CLEAR_BIT(MCPWM0_IO45,BIT7);   //complement mode
	MCPWM0_IO45 = CLEAR_BIT(MCPWM0_IO45,BIT15);  //complement mode
	MCPWM0_PP   = 0x00; /* normal mode */
	
	stru_mcpwm_shadow1.TH0 = 500;//设置100hz初始化，不进行分频
	if(stru_mcpwm_shadow1.TH0 == 0U)
	{
		INFO("wrong pwm period\n");
		return;
	}
	else
	{
		stru_mcpwm_shadow_period_temp = (MCU_CLK>>1)/stru_mcpwm_shadow1.TH0;
	}
	
	if(stru_mcpwm_shadow_period_temp > 65535U) /* 当频率太低时将MCPWM的时钟进行8分频 */
	{
		MCPWM0_TCLK |= (BIT0 + BIT1);
		stru_mcpwm_shadow1.TH0 = stru_mcpwm_shadow_period_temp>>3;
	}
	else
	{
		MCPWM0_TCLK &= ~(BIT0 + BIT1);
		stru_mcpwm_shadow1.TH0 = stru_mcpwm_shadow_period_temp;    //频率固定500hz
	} 
		/* mcpwm0 compare parameter */
		stru_mcpwm_shadow1.TH00 = (int16_t)(( -16384* stru_mcpwm_shadow1.TH0 + 16384) >> 15);
		stru_mcpwm_shadow1.TH01 = ( (16384*stru_mcpwm_shadow1.TH0 + 16384) >> 15); 
		//初始化为50占空比，后续DMA改变
	
		stru_mcpwm_shadow1.TH10 = (int16_t)(( -16384* stru_mcpwm_shadow1.TH0 + 16384) >> 15);; //P
		stru_mcpwm_shadow1.TH11 = ( (16384*stru_mcpwm_shadow1.TH0 + 16384) >> 15);								//N
		stru_mcpwm_shadow1.TH20 = (int16_t)(( -16384* stru_mcpwm_shadow1.TH0 + 16384) >> 15);;
		stru_mcpwm_shadow1.TH21 = ( (16384*stru_mcpwm_shadow1.TH0 + 16384) >> 15);
		
		
		/* mcpwm0/1 deadtime parameter */
		stru_mcpwm_shadow1.DeadTimeCH012P = 0*EPWM_Tus;  //以P极为主
		stru_mcpwm_shadow1.DeadTimeCH012N = 0*EPWM_Tus;
		
		/*************************************************************************/
		#if 1
		MCPWM0_TH0 = stru_mcpwm_shadow1.TH0 %32768;    //不能比32768还大

		MCPWM0_TH00 = stru_mcpwm_shadow1.TH00;   //TH00 + TH01 ÷2 ÷ 32768等于占空比
		MCPWM0_TH01 = stru_mcpwm_shadow1.TH01;
		MCPWM0_TH10 = stru_mcpwm_shadow1.TH10;
		MCPWM0_TH11 = stru_mcpwm_shadow1.TH11;
		MCPWM0_TH20 = stru_mcpwm_shadow1.TH20;
		MCPWM0_TH21 = stru_mcpwm_shadow1.TH21;
		
		MCPWM0_DTH00 = stru_mcpwm_shadow1.DeadTimeCH012P; // MCPWM CH0/1/2 P 通道死区宽度设置为0
		MCPWM0_DTH01 = stru_mcpwm_shadow1.DeadTimeCH012N; // MCPWM CH0/1/2 N 通道死区宽度设置为0
		#endif
		/*************************************************************************/
		MCPWM0_PRT = 0x000FFFFF; /*enter password to unlock write protection */
}


//初始化mcpwm为500hz，50占空比，死区0
void DMA_MCPWM_Change_cycle_Init(void)
{
	uint32_t stru_mcpwm_shadow_period_temp;
	
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */
	
	/* 图 14-7MCPWM 时序 TH<n>0 和 TH<n>1-中心对齐模式 */
	MCPWM0_IO01 = CLEAR_BIT(MCPWM0_IO01,BIT7);   //complement mode		通道0选择互补模式
	MCPWM0_IO01 = CLEAR_BIT(MCPWM0_IO01,BIT15);  //complement mode
	MCPWM0_IO23 = CLEAR_BIT(MCPWM0_IO23,BIT7);   //complement mode
	MCPWM0_IO23 = CLEAR_BIT(MCPWM0_IO23,BIT15);  //complement mode
	MCPWM0_IO45 = CLEAR_BIT(MCPWM0_IO45,BIT7);   //complement mode
	MCPWM0_IO45 = CLEAR_BIT(MCPWM0_IO45,BIT15);  //complement mode
	MCPWM0_PP   = 0x00; /* normal mode */
	
	stru_mcpwm_shadow1.TH0 = 500;//设置500hz初始化  
	if(stru_mcpwm_shadow1.TH0 == 0U)
	{
		INFO("wrong pwm period\n");
		return;
	}
	else
	{
		stru_mcpwm_shadow_period_temp = (MCU_CLK>>1)/stru_mcpwm_shadow1.TH0;
	}
	
	if(stru_mcpwm_shadow_period_temp > 65535U) /* 当频率太低时将MCPWM的时钟进行8分频 */
	{
		MCPWM0_TCLK |= (BIT0 + BIT1);
		stru_mcpwm_shadow1.TH0 = stru_mcpwm_shadow_period_temp>>3;
	}
	else
	{
		MCPWM0_TCLK &= ~(BIT0 + BIT1);
		stru_mcpwm_shadow1.TH0 = stru_mcpwm_shadow_period_temp;    
	} 
		/* mcpwm0 compare parameter */
		stru_mcpwm_shadow1.TH00 = (int16_t)(( -16384* stru_mcpwm_shadow1.TH0 + 16384) >> 15);
		stru_mcpwm_shadow1.TH01 = ( (16384*stru_mcpwm_shadow1.TH0 + 16384) >> 15); 
		//初始化为50占空比，后续DMA改变
	
		stru_mcpwm_shadow1.TH10 = (int16_t)(( -16384* stru_mcpwm_shadow1.TH0 + 16384) >> 15);; //P
		stru_mcpwm_shadow1.TH11 = ( (16384*stru_mcpwm_shadow1.TH0 + 16384) >> 15);								//N
		stru_mcpwm_shadow1.TH20 = (int16_t)(( -16384* stru_mcpwm_shadow1.TH0 + 16384) >> 15);;
		stru_mcpwm_shadow1.TH21 = ( (16384*stru_mcpwm_shadow1.TH0 + 16384) >> 15);
		
		
		/* mcpwm0/1 deadtime parameter */
		stru_mcpwm_shadow1.DeadTimeCH012P = 0*EPWM_Tus;  //以P极为主
		stru_mcpwm_shadow1.DeadTimeCH012N = 0*EPWM_Tus;
		
		/*************************************************************************/
		#if 1
		MCPWM0_TH0 = stru_mcpwm_shadow1.TH0 %32768;    //不能比32768还大

		MCPWM0_TH00 = stru_mcpwm_shadow1.TH00;   //TH00 + TH01 ÷2 ÷ 32768等于占空比
		MCPWM0_TH01 = stru_mcpwm_shadow1.TH01;
		MCPWM0_TH10 = stru_mcpwm_shadow1.TH10;
		MCPWM0_TH11 = stru_mcpwm_shadow1.TH11;
		MCPWM0_TH20 = stru_mcpwm_shadow1.TH20;
		MCPWM0_TH21 = stru_mcpwm_shadow1.TH21;
		
		MCPWM0_DTH00 = stru_mcpwm_shadow1.DeadTimeCH012P; // MCPWM CH0/1/2 P 通道死区宽度设置为0
		MCPWM0_DTH01 = stru_mcpwm_shadow1.DeadTimeCH012N; // MCPWM CH0/1/2 N 通道死区宽度设置为0
		#endif
		/*************************************************************************/
		MCPWM0_PRT = 0x000FFFFF; /*enter password to unlock write protection */
}



void MCPWM_Dsiable(uint8_t mcpwm)
{
	if(mcpwm >1) return;
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */
	if(mcpwm == 1)
	{
		MCPWM0->TCLK &=~ (1U << 7);//bit7:mcpwm1
	}
	else if(mcpwm == 0)
	{
		MCPWM0->TCLK &=~ (1U << 6);//bit7:mcpwm1
	}
 
	MCPWM0_PRT = 0x0000AAAA; /*enter password to unlock write protection */
}

void MCPWM_Enable(uint8_t mcpwm)
{
	if(mcpwm >1) return;
	MCPWM0_PRT = 0x0000DEAD; /*enter password to unlock write protection */
	if(mcpwm == 1)
	{
		MCPWM0->TCLK |=  (1U << 7);//bit7:mcpwm1
	}
	else if(mcpwm == 0)
	{
		MCPWM0->TCLK |=  (1U << 6);//bit6:mcpwm0
	}
	
	MCPWM0_PRT = 0x0000AAAA; /*enter password to unlock write protection */
}





#ifdef MCPWM_TEST
// 500Khz PWM 呼吸占空比表 200组（严格沿用原比例）
// 占空比变化：5% → 75% → 5% 循环呼吸
const uint16_t MCPWM_Duty_buff[Duty_len] = {
    // ==================== 上升段：5% → 75%（前100级）====================
    0x8F20,  //  0    5%
    0x9080,  //  1    5.5%
    0x91E0,  //  2    6%
    0x9340,  //  3    6.5%
    0x94A0,  //  4    7%
    0x9600,  //  5    7.5%
    0x9760,  //  6    8%
    0x98C0,  //  7    8.5%
    0x9A20,  //  8    9%
    0x9B80,  //  9    9.5%

    0x9CE0,  // 10   10%
    0x9E40,  // 11   10.5%
    0x9FA0,  // 12   11%
    0xA100,  // 13   11.5%
    0xA260,  // 14   12%
    0xA3C0,  // 15   12.5%
    0xA520,  // 16   13%
    0xA680,  // 17   13.5%
    0xA7E0,  // 18   14%
    0xA940,  // 19   14.5%

    0xAAa0,  // 20   15%
    0xAC00,  // 21   15.5%
    0xAD60,  // 22   16%
    0xAEc0,  // 23   16.5%
    0xB020,  // 24   17%
    0xB180,  // 25   17.5%
    0xB2E0,  // 26   18%
    0xB440,  // 27   18.5%
    0xB5A0,  // 28   19%
    0xB700,  // 29   19.5%

    0xB860,  // 30   20%
    0xB9C0,  // 31   21%
    0xBB20,  // 32   22%
    0xBC80,  // 33   23%
    0xBDE0,  // 34   24%
    0xBF40,  // 35   25%
    0xC0A0,  // 36   26%
    0xC200,  // 37   27%
    0xC360,  // 38   28%
    0xC4C0,  // 39   29%

    0xC620,  // 40   30%
    0xC780,  // 41   31%
    0xC8E0,  // 42   32%
    0xCA40,  // 43   33%
    0xCBA0,  // 44   34%
    0xCD00,  // 45   35%
    0xCE60,  // 46   36%
    0xCFC0,  // 47   37%
    0xD120,  // 48   38%
    0xD280,  // 49   39%

    0xD3E0,  // 50   40%
    0xD540,  // 51   41%
    0xD6A0,  // 52   42%
    0xD800,  // 53   43%
    0xD960,  // 54   44%
    0xDAC0,  // 55   45%
    0xDC20,  // 56   46%
    0xDD80,  // 57   47%
    0xDEE0,  // 58   48%
    0xE040,  // 59   49%

    0xE1A0,  // 60   50%
    0xE300,  // 61   51%
    0xE460,  // 62   52%
    0xE5C0,  // 63   53%
    0xE720,  // 64   54%
    0xE880,  // 65   55%
    0xE9E0,  // 66   56%
    0xEB40,  // 67   57%
    0xECA0,  // 68   58%
    0xEE00,  // 69   59%

    0xEF60,  // 70   60%
    0xF0C0,  // 71   61%
    0xF220,  // 72   62%
    0xF380,  // 73   63%
    0xF4E0,  // 74   64%
    0xF640,  // 75   65%
    0xF7A0,  // 76   66%
    0xF900,  // 77   67%
    0xFA60,  // 78   68%
    0xFBC0,  // 79   69%

    0xFD20,  // 80   70%
    0xFE80,  // 81   71%
    0x0000,  // 82   72%
    0x0280,  // 83   73%
    0x0500,  // 84   74%
    0x3E80,  // 85   75% 峰值
    0x3E80,  // 86   75%
    0x3E80,  // 87   75%
    0x3E80,  // 88   75%
    0x3E80,  // 89   75%

    0x3E80,  // 90   75%
    0x3E80,  // 91   75%
    0x3E80,  // 92   75%
    0x3E80,  // 93   75%
    0x3E80,  // 94   75%
    0x3E80,  // 95   75%
    0x3E80,  // 96   75%
    0x3E80,  // 97   75%
    0x3E80,  // 98   75%
    0x3E80,  // 99   75%

    // ==================== 下降段：75% → 5%（后100级，镜像对称）====================
    0x0500,  // 100  74%
    0x0280,  // 101  73%
    0x0000,  // 102  72%
    0xFE80,  // 103  71%
    0xFD20,  // 104  70%
    0xFBC0,  // 105  69%
    0xFA60,  // 106  68%
    0xF900,  // 107  67%
    0xF7A0,  // 108  66%
    0xF640,  // 109  65%

    0xF4E0,  // 110  64%
    0xF380,  // 111  63%
    0xF220,  // 112  62%
    0xF0C0,  // 113  61%
    0xEF60,  // 114  60%
    0xEE00,  // 115  59%
    0xECA0,  // 116  58%
    0xEB40,  // 117  57%
    0xE9E0,  // 118  56%
    0xE880,  // 119  55%

    0xE720,  // 120  54%
    0xE5C0,  // 121  53%
    0xE460,  // 122  52%
    0xE300,  // 123  51%
    0xE1A0,  // 124  50%
    0xE040,  // 125  49%
    0xDEE0,  // 126  48%
    0xDD80,  // 127  47%
    0xDC20,  // 128  46%
    0xDAC0,  // 129  45%

    0xD960,  // 130  44%
    0xD800,  // 131  43%
    0xD6A0,  // 132  42%
    0xD540,  // 133  41%
    0xD3E0,  // 134  40%
    0xD280,  // 135  39%
    0xD120,  // 136  38%
    0xCFC0,  // 137  37%
    0xCE60,  // 138  36%
    0xCD00,  // 139  35%

    0xCBA0,  // 140  34%
    0xCA40,  // 141  33%
    0xC8E0,  // 142  32%
    0xC780,  // 143  31%
    0xC620,  // 144  30%
    0xC4C0,  // 145  29%
    0xC360,  // 146  28%
    0xC200,  // 147  27%
    0xC0A0,  // 148  26%
    0xBF40,  // 149  25%

    0xBDE0,  // 150  24%
    0xBC80,  // 151  23%
    0xBB20,  // 152  22%
    0xB9C0,  // 153  21%
    0xB860,  // 154  20%
    0xB700,  // 155  19.5%
    0xB5A0,  // 156  19%
    0xB440,  // 157  18.5%
    0xB2E0,  // 158  18%
    0xB180,  // 159  17.5%

    0xB020,  // 160  17%
    0xAEc0,  // 161  16.5%
    0xAD60,  // 162  16%
    0xAC00,  // 163  15.5%
    0xAAa0,  // 164  15%
    0xA940,  // 165  14.5%
    0xA7E0,  // 166  14%
    0xA680,  // 167  13.5%
    0xA520,  // 168  13%
    0xA3C0,  // 169  12.5%

    0xA260,  // 170  12%
    0xA100,  // 171  11.5%
    0x9FA0,  // 172  11%
    0x9E40,  // 173  10.5%
    0x9CE0,  // 174  10%
    0x9B80,  // 175  9.5%
    0x9A20,  // 176  9%
    0x98C0,  // 177  8.5%
    0x9760,  // 178  8%
    0x9600,  // 179  7.5%

    0x94A0,  // 180  7%
    0x9340,  // 181  6.5%
    0x91E0,  // 182  6%
    0x9080,  // 183  5.5%
    0x8F20,  // 184  5%
    0x8F20,  // 185  5%
    0x8F20,  // 186  5%
    0x8F20,  // 187  5%
    0x8F20,  // 188  5%
    0x8F20,  // 189  5%

    0x8F20,  // 190  5%
    0x8F20,  // 191  5%
    0x8F20,  // 192  5%
    0x8F20,  // 193  5%
    0x8F20,  // 194  5%
    0x8F20,  // 195  5%
    0x8F20,  // 196  5%
    0x8F20,  // 197  5%
    0x8F20,  // 198  5%
    0x8F20,  // 199  5%
};

// 死区配置表：数值 = 索引 ×32，递增型
// 格式：[索引]  十六进制值  // 十进制值
const uint16_t Deadtime_buff[Deadtime_len] = {
    0x0000,  //  0     0
    0x0020,  //  1    32
    0x0040,  //  2    64
    0x0060,  //  3    96
    0x0080,  //  4   128
    0x00A0,  //  5   160
    0x00C0,  //  6   192
    0x00E0,  //  7   224
    0x0100,  //  8   256
    0x0120,  //  9   288
    0x0140,  // 10   320
    0x0160,  // 11   352
    0x0180,  // 12   384
    0x01A0,  // 13   416
    0x01C0,  // 14   448
    0x01E0,  // 15   480
    0x0200,  // 16   512
    0x0220,  // 17   544
    0x0240,  // 18   576
    0x0260,  // 19   608
    0x0280,  // 20   640
    0x02A0,  // 21   672
    0x02C0,  // 22   704
    0x02E0,  // 23   736
    0x0300,  // 24   768
    0x0320,  // 25   800
    0x0340,  // 26   832
    0x0360,  // 27   864
    0x0380,  // 28   896
    0x03A0,  // 29   928
    0x03C0,  // 30   960
    0x03E0   // 31   992
};

const uint16_t freq_buff[freq_len] = {
    490,    //  0    32653 Hz    16000000 / 32653 = 490
    2340,   //  1     6837 Hz    16000000 / 6837  = 2340
    4981,   //  2     3212 Hz    16000000 / 3212  = 4981
    8501,   //  3     1882 Hz    16000000 / 1882  = 8501
    12021,  //  4     1331 Hz    16000000 / 1331  = 12021
    15549,  //  5     1029 Hz    16000000 / 1029  = 15549
    19512,  //  6      820 Hz    16000000 / 820   = 19512
    24353,  //  7      657 Hz    16000000 / 657   = 24353
    28300,  //  8      565 Hz    16000000 / 565   = 28300
    32719   //  9      489 Hz    16000000 / 489   = 32719
};

#else
const uint16_t freq_buff[freq_len];
const uint16_t Deadtime_buff[Deadtime_len];
const uint16_t MCPWM_Duty_buff[Duty_len];

#endif

/******************************  定时器    *******************************************/
#include "TIM_Reg.h"
volatile uint32_t* TIM_DMA_CMPX_ADDR[] = {
   &TIMER13->CMP0, &TIMER13->CMP1,
   &TIMER14->CMP0, &TIMER14->CMP1,
   &TIMER15->CMP0, &TIMER15->CMP1,
   &TIMER16->CMP0, &TIMER16->CMP1
};

 


/**********************************************************************
 * @brief DMA 内存→外设传输配置（用于定时器寄存器动态更新）
 * @param dma_channel  DMA 通道编号
 * @param Rt          DMA 工作模式：单次 / 循环
 * @param buf         数据源缓冲区（存放 TH / CMP0 / CMP1 数值）
 * @param dst_addrr   目标寄存器地址索引（TIM_DMA_CMPX_ADDR 数组下标）
 * @param len         传输数据长度（单位：字）
 * @note  1. 同一触发源不可同时绑定多个DMA通道
 *        2. 每次调用会覆盖原有DMA配置
 *        3. 传输方向固定为：内存 → 外设寄存器
 **********************************************************************/
void DMA_IT_TIMER_Config(DMA_Channel_e dma_channel,dma_mode_t Rt ,const uint32_t *buf,timer_cmpx_dst_addrr dst_addrr,uint16_t len)
{			
    // 1. 定义DMA配置结构体（局部变量初始化，避免脏数据）
    dma_ch_config_t dma_handle ;   
		TIMER13->IER.CH0_RE = 1;   //cnt = cmpx发送比较事件
//		TIMER13->IER.CH1_RE = 1;   
//		TIMER13->IER.ZC_RE = 1;    //cnt 到0
		TIMER13->IER.FAIL_RE = 1;  //timer fail事件，定时器异常
 
    /* ====================== 核心参数配置 ====================== */
    // 2. 基础通道与模式配置
    dma_handle.dma_ch              = dma_channel;          // 指定DMA通道
    dma_handle.circular_mode       = Rt;      						// 普通模式（非循环）
    dma_handle.dir                 = DMA_DIR_MEM_TO_PERIPH;// 传输方向：内存→外设
    dma_handle.DMA_Trigger_periph  = DMA_Trigger_TIMER13; 
    dma_handle.burst_len           = DMA_BURST_1_UNIT;     // 突发传输长度：1个单元

    // 3. 地址配置（核心：补全MCPWM目标寄存器，此处以TH00为例）
    dma_handle.src_addr            = (uint32_t)buf;        // 源地址：用户传入的缓冲区
		dma_handle.dst_addr = (uint32_t)TIM_DMA_CMPX_ADDR[dst_addrr]; 				 
	//TH   频率    
	//COMP  占空比
		
    // 4. 地址递增模式（内存自增，外设固定）
    dma_handle.src_inc             = Addr_Enable;          // 源地址自增（遍历缓冲区）
    dma_handle.dst_inc             = Addr_Disable;         // 目的地址固定（仅写TH00寄存器）

    // 5. 数据宽度配置 
    dma_handle.dma_data_width_src_width = DMA_BITSIZE_32BIT; 
    dma_handle.dma_data_width_dst_width = DMA_BITSIZE_32BIT; 

    // 6. 优先级配置（中等优先级，避免抢占核心中断）
    dma_handle.priority            = DMA_PRIORITY_MID;     // DMA通道优先级：中等
    dma_handle.priority_it         = 1;                    // 中断优先级：数值越小优先级越高

    // 7. 传输长度配置 
    dma_handle.transfer_size       = len;                  // 单次传输数据长度

    /* ====================== 硬件初始化 ====================== */
    // 8. 配置前先失能DMA通道（防止配置过程中误触发）
    DMA_div_chx_Disable(dma_handle.dma_ch);

    DMA_div_TriggerSource_Clean();											//触发源恢复默认状态
		
		// 9. 加载DMA配置参数到硬件寄存器
    HAL_DMA_StructerInit(&dma_handle);

    // 10. 使能DMA中断（NVIC层）
    HAL_DMA_NVIC_Enable(dma_handle.dma_ch, dma_handle.priority);

    // 11. 配置DMA中断类型（传输完成/传输错误/配置错误）
    HAL_DMA_IT_Transfer_Set(dma_channel, dma_it_enable);  // 传输完成中断使能
    HAL_DMA_IT_TransError_Set(dma_channel, dma_it_enable);// 传输错误中断使能
    HAL_DMA_IT_Configerror_Set(dma_it_enable);            // 配置错误中断使能

    // 12. 配置DMA全局中断（NVIC优先级+使能）
    IRQn_Type IRQ_DMA = DMA_IRQn;                         // DMA全局中断号
    NVIC_SetPriority(IRQ_DMA, dma_handle.priority_it);    // 设置中断优先级
    NVIC_ClearPendingIRQ(IRQ_DMA);                        // 清除挂起中断标志
    NVIC_EnableIRQ(IRQ_DMA);         
 
		DMA_div_chx_Enable(dma_handle.dma_ch);
	
		 
		uint32_t cnt =0;
		while(HAL_DMA_GetRemainingCnt(dma_handle.dma_ch) > 0)
		{			 
			if(cnt++ > 0xFFFFFU)
			{
 				INFO("\n DMA_ch1 time error--->>cnt = %d\n",HAL_DMA_GetRemainingCnt(dma_handle.dma_ch) );
				cnt = 0;
				return ;
			}
			;
		}
	 
		
		
    // 14. 打印初始化信息 
    INFO("DMA Channel %d initialized for TIMER (Mem-to-Periph, size: %d * 32bit)\n", 
         dma_channel, dma_handle.transfer_size);
}



void DMA_IT_TIMER_MemToMem_Config(DMA_Channel_e dma_channel,
																	dma_mode_t Rt,
																	const uint32_t *Txbuf, 
																  uint32_t *Rxbuf,uint16_t len)
{			
    // 1. 定义DMA配置结构体（局部变量初始化，避免脏数据）
    dma_ch_config_t dma_handle ;   
		TIMER13->IER.CH0_RE = 1;   //cnt = cmpx发送比较事件
//		TIMER13->IER.CH1_RE = 1;   
//		TIMER13->IER.ZC_RE = 1;    //cnt 到0
		TIMER13->IER.FAIL_RE = 1;  //timer fail事件，定时器异常
 
    /* ====================== 核心参数配置 ====================== */
    // 2. 基础通道与模式配置
    dma_handle.dma_ch              = dma_channel;          // 指定DMA通道
    dma_handle.circular_mode       = Rt;      						// 普通模式（非循环）
    dma_handle.dir                 = DMA_DIR_MEM_TO_PERIPH;// 传输方向：内存→外设
    dma_handle.DMA_Trigger_periph  = DMA_Trigger_TIMER13; 
    dma_handle.burst_len           = DMA_BURST_1_UNIT;     // 突发传输长度：1个单元

    // 3. 地址配置（核心：补全MCPWM目标寄存器，此处以TH00为例）
    dma_handle.src_addr            = (uint32_t)Txbuf;        // 源地址：用户传入的缓冲区
		dma_handle.dst_addr = (uint32_t)Rxbuf; 				 
 
		
    // 4. 地址递增模式（内存自增，外设固定）
    dma_handle.src_inc             = Addr_Enable;          // 源地址自增（遍历缓冲区）
    dma_handle.dst_inc             = Addr_Enable;         // 目的地址固定（仅写TH00寄存器）

    // 5. 数据宽度配置 
    dma_handle.dma_data_width_src_width = DMA_BITSIZE_32BIT; 
    dma_handle.dma_data_width_dst_width = DMA_BITSIZE_32BIT; 

    // 6. 优先级配置（中等优先级，避免抢占核心中断）
    dma_handle.priority            = DMA_PRIORITY_MID;     // DMA通道优先级：中等
    dma_handle.priority_it         = 1;                    // 中断优先级：数值越小优先级越高

    // 7. 传输长度配置 
    dma_handle.transfer_size       = len;                  // 单次传输数据长度

    /* ====================== 硬件初始化 ====================== */
    // 8. 配置前先失能DMA通道（防止配置过程中误触发）
    DMA_div_chx_Disable(dma_handle.dma_ch);

    DMA_div_TriggerSource_Clean();											//触发源恢复默认状态
		
		// 9. 加载DMA配置参数到硬件寄存器
    HAL_DMA_StructerInit(&dma_handle);

    // 10. 使能DMA中断（NVIC层）
    HAL_DMA_NVIC_Enable(dma_handle.dma_ch, dma_handle.priority);

    // 11. 配置DMA中断类型（传输完成/传输错误/配置错误）
    HAL_DMA_IT_Transfer_Set(dma_channel, dma_it_enable);  // 传输完成中断使能
    HAL_DMA_IT_TransError_Set(dma_channel, dma_it_enable);// 传输错误中断使能
    HAL_DMA_IT_Configerror_Set(dma_it_enable);            // 配置错误中断使能

    // 12. 配置DMA全局中断（NVIC优先级+使能）
    IRQn_Type IRQ_DMA = DMA_IRQn;                         // DMA全局中断号
    NVIC_SetPriority(IRQ_DMA, dma_handle.priority_it);    // 设置中断优先级
    NVIC_ClearPendingIRQ(IRQ_DMA);                        // 清除挂起中断标志
    NVIC_EnableIRQ(IRQ_DMA);         
 
		DMA_div_chx_Enable(dma_handle.dma_ch);
	
		 
		uint32_t cnt =0;
		while(HAL_DMA_GetRemainingCnt(dma_handle.dma_ch) > 0)
		{			 
			if(cnt++ > 0xFFFFFU)
			{
 				INFO("\n DMA_ch1 time error--->>cnt = %d\n",HAL_DMA_GetRemainingCnt(dma_handle.dma_ch) );
				cnt = 0;
				return ;
			}
			;
		}
 	
    // 14. 打印初始化信息 
    INFO("DMA Channel %d initialized for TIMER (Mem-to-Mem, size: %d * 32bit)\n", 
         dma_channel, dma_handle.transfer_size);
}



void DMA_IT_TIMER_PeriphToMem_Config(DMA_Channel_e dma_channel,
																	dma_mode_t Rt,
																  uint32_t *Rxbuf)
{			
    // 1. 定义DMA配置结构体（局部变量初始化，避免脏数据）
    dma_ch_config_t dma_handle ;   
		TIMER13->IER.CH0_RE = 1;   //cnt = cmpx发送比较事件
//		TIMER13->IER.CH1_RE = 1;   
//		TIMER13->IER.ZC_RE = 1;    //cnt 到0
		TIMER13->IER.FAIL_RE = 1;  //timer fail事件，定时器异常
 
    /* ====================== 核心参数配置 ====================== */
    // 2. 基础通道与模式配置
    dma_handle.dma_ch              = dma_channel;          // 指定DMA通道
    dma_handle.circular_mode       = Rt;      						// 普通模式（非循环）
    dma_handle.dir                 = DMA_DIR_PERIPH_TO_MEM;// 传输方向：内存→外设
    dma_handle.DMA_Trigger_periph  = DMA_Trigger_TIMER13; 
    dma_handle.burst_len           = DMA_BURST_1_UNIT;     // 突发传输长度：1个单元

    // 3. 地址配置（核心：补全MCPWM目标寄存器，此处以TH00为例）
    dma_handle.src_addr            = (uint32_t)&TIMER13->CMP1;        // 源地址：用户传入的缓冲区
		dma_handle.dst_addr = (uint32_t)Rxbuf; 				 
 
		
    // 4. 地址递增模式（内存自增，外设固定）
    dma_handle.src_inc             = Addr_Disable;          // 源地址自增（遍历缓冲区）
    dma_handle.dst_inc             = Addr_Enable;         // 目的地址固定（仅写TH00寄存器）

    // 5. 数据宽度配置 
    dma_handle.dma_data_width_src_width = DMA_BITSIZE_32BIT; 
    dma_handle.dma_data_width_dst_width = DMA_BITSIZE_32BIT; 

    // 6. 优先级配置（中等优先级，避免抢占核心中断）
    dma_handle.priority            = DMA_PRIORITY_MID;     // DMA通道优先级：中等
    dma_handle.priority_it         = 1;                    // 中断优先级：数值越小优先级越高

    // 7. 传输长度配置 
    dma_handle.transfer_size       = 1;                   // 单次传输数据长度

    /* ====================== 硬件初始化 ====================== */
    // 8. 配置前先失能DMA通道（防止配置过程中误触发）
    DMA_div_chx_Disable(dma_handle.dma_ch);

    DMA_div_TriggerSource_Clean();											//触发源恢复默认状态
		
		// 9. 加载DMA配置参数到硬件寄存器
    HAL_DMA_StructerInit(&dma_handle);

    // 10. 使能DMA中断（NVIC层）
    HAL_DMA_NVIC_Enable(dma_handle.dma_ch, dma_handle.priority);

    // 11. 配置DMA中断类型（传输完成/传输错误/配置错误）
    HAL_DMA_IT_Transfer_Set(dma_channel, dma_it_enable);  // 传输完成中断使能
    HAL_DMA_IT_TransError_Set(dma_channel, dma_it_enable);// 传输错误中断使能
    HAL_DMA_IT_Configerror_Set(dma_it_enable);            // 配置错误中断使能

    // 12. 配置DMA全局中断（NVIC优先级+使能）
    IRQn_Type IRQ_DMA = DMA_IRQn;                         // DMA全局中断号
    NVIC_SetPriority(IRQ_DMA, dma_handle.priority_it);    // 设置中断优先级
    NVIC_ClearPendingIRQ(IRQ_DMA);                        // 清除挂起中断标志
    NVIC_EnableIRQ(IRQ_DMA);         
 
		DMA_div_chx_Enable(dma_handle.dma_ch);
	
		 
		uint32_t cnt =0;
		while(HAL_DMA_GetRemainingCnt(dma_handle.dma_ch) > 0)
		{			 
			if(cnt++ > 0xFFFFFU)
			{
 				INFO("\n DMA_ch1 time error--->>cnt = %d\n",HAL_DMA_GetRemainingCnt(dma_handle.dma_ch) );
				cnt = 0;
				return ;
			}
			;
		}
 	
    // 14. 打印初始化信息 
    INFO("DMA Channel %d initialized for TIMER (Periph-to-Mem, size: %d * 32bit)\n", 
         dma_channel, dma_handle.transfer_size);
}



/******************************  SPI  MASTER  *******************************************/
#include "spi_test.h"
#include "spi_hal.h"
#include "test.h"
/**
 * @brief SPI0 DMA 收发配置函数
 * @param dma_channel: DMA 通道号
 * @param Rxbuf: SPI 接收缓冲区指针
 * @param Txbuf: SPI 发送缓冲区指针
 * @param len: 传输长度
 * @param spi_mode: 预留 SPI 模式
 * @param spi_addr: SPI_TX / SPI_RX 选择
 */
void DMA_IT_SPI_Config(DMA_Channel_e dma_channel,uint8_t *Rxbuf,uint8_t *Txbuf,uint16_t len,uint8_t spi_mode,spi_addr_e spi_addr,dma_priority_t dma_priority)
{
//		#ifndef SPI_TEST   //没定义spi则手动执行gpio初始化
//		spi0_gpio_init();
//		#endif
    // 定义DMA配置结构体（局部变量初始化，避免脏数据、野值）
    dma_ch_config_t dma_handle ;
		memset(&dma_handle,0,sizeof(dma_handle));
    /* ====================== 1. SPI DMA 硬件使能 ====================== */
    

    /* ====================== 2. DMA 基础配置 ====================== */
    dma_handle.dma_ch              = dma_channel;          // 绑定当前使用的DMA通道
    dma_handle.circular_mode       = DMA_MODE_NORMAL;      // 普通模式，非循环传输
    
    
    dma_handle.burst_len           = DMA_BURST_1_UNIT;     // 单次传输1个数据单元

    /* ====================== 3. 源/目标地址配置 ====================== */
    if(spi_addr == SPI_TX)  // TX 模式：内存 -> SPI->DATA
    {
				dma_handle.DMA_Trigger_periph  = DMA_Trigger_SPI0_TX;   
        dma_handle.src_addr = (uint32_t)Txbuf;             
        dma_handle.dst_addr = (uint32_t)&SPI0->DATA; 
				dma_handle.dir                 = DMA_DIR_MEM_TO_PERIPH;// 默认方向：外设→内存（RX）
				/* ====================== 4. 地址递增模式 ====================== */
				// 外设地址（SPI DATA）固定不变，内存地址（缓冲区）自动递增
				dma_handle.src_inc             = Addr_Enable;         
				dma_handle.dst_inc             = Addr_Disable;   
						/* ====================== 5. 数据位宽配置 ====================== */
				// 与SPI数据位宽保持一致：32bit
				dma_handle.dma_data_width_src_width = DMA_BITSIZE_8BIT;
				dma_handle.dma_data_width_dst_width = DMA_BITSIZE_8BIT;
    }
    else  if(spi_addr == SPI_RX)   // RX 模式：SPI->DATA -> 内存
    {
				dma_handle.DMA_Trigger_periph  = DMA_Trigger_SPI0_RX;   
        dma_handle.src_addr = (uint32_t)&SPI0->DATA;       	    
        dma_handle.dst_addr = (uint32_t)Rxbuf;  
						/* ====================== 4. 地址递增模式 ====================== */
				// 外设地址（SPI DATA）固定不变，内存地址（缓冲区）自动递增
				dma_handle.src_inc             = Addr_Disable;         // 外设地址不自增
				dma_handle.dst_inc             = Addr_Enable;         // 内存地址自增
				dma_handle.dir                 = DMA_DIR_PERIPH_TO_MEM;// 默认方向：外设→内存（RX）
				/* ====================== 5. 数据位宽配置 ====================== */
				// 与SPI数据位宽保持一致：32bit
				dma_handle.dma_data_width_src_width = DMA_BITSIZE_8BIT;
				dma_handle.dma_data_width_dst_width = DMA_BITSIZE_8BIT;
    }

    

    

    /* ====================== 6. 优先级 ====================== */
    dma_handle.priority            = dma_priority;     // DMA通道中等优先级
    dma_handle.priority_it         = 1;                    // 中断优先级

    /* ====================== 7. 传输长度 ====================== */
    dma_handle.transfer_size       = len;                  // 设置本次传输长度

    /* ====================== 8. 配置前先关闭通道 ====================== */
    // 必须先关闭，否则寄存器无法写入
    DMA_div_chx_Disable(dma_handle.dma_ch);

    // 清除所有触发源配置，恢复默认状态，避免通道冲突
    DMA_div_TriggerSource_Clean();

    // 手动配置通道1触发源选择（硬件映射）
//    DMA_Type->DMA_CHCFG1_STR.Bits.CH1SEL = 0x13;

    /* ====================== 9. 加载DMA配置到硬件 ====================== */
    HAL_DMA_StructerInit(&dma_handle);

    /* ====================== 10. DMA 中断配置 ====================== */
    // 使能DMA通道中断（NVIC）
    HAL_DMA_NVIC_Enable(dma_handle.dma_ch, dma_handle.priority);

    // 使能：传输完成中断 + 传输错误中断 + 配置错误中断
    HAL_DMA_IT_Transfer_Set(dma_channel, dma_it_enable);
    HAL_DMA_IT_TransError_Set(dma_channel, dma_it_enable);
    HAL_DMA_IT_Configerror_Set(dma_it_enable);

    // 配置DMA全局中断
    IRQn_Type IRQ_DMA = DMA_IRQn;
    NVIC_SetPriority(IRQ_DMA, dma_handle.priority_it);
    NVIC_ClearPendingIRQ(IRQ_DMA);
    NVIC_EnableIRQ(IRQ_DMA);
		
		
    /* ====================== 11. 启动DMA通道 ====================== */
    DMA_div_chx_Enable(dma_handle.dma_ch);

    /* ====================== 12. 等待DMA传输完成（超时保护） ====================== */
		if(SPI0->TRANSCTRL.TRANSMODE == SPI_TRANS_MODE_WR_ONLY  || SPI0->TRANSCTRL.TRANSMODE == SPI_TRANS_MODE_RD_ONLY)
		{
			SPI0->CMD.CMD = 0x03;//同时收发 只需要发送一次 避免发送两次
			uint32_t cnt = 0;
			while(HAL_DMA_GetRemainingCnt(dma_handle.dma_ch) > 0)
			{
					// 超时判断，防止硬件异常导致死循环
					if(cnt++ > 0xFFFFFU)
					{
						INFO("\n DMA time error--->>cnt = %d\n",HAL_DMA_GetRemainingCnt(dma_handle.dma_ch));
							return;
					}
			}
		}
    

    /* ====================== 13. 初始化完成 ====================== */
    INFO("DMA Channel %d initialized for SPI0 (size: %d * 32bit)\n",
         dma_channel, dma_handle.transfer_size);
}



 

void HAL_DMA_SPI_Tx_Init(uint8_t *Txbuf,uint32_t spi_clk,uint16_t len)
{
//	#ifndef SPI_TEST   //没定义spi则手动执行gpio初始化
//	spi0_gpio_init();
//	#endif
	SPI_TypeDef *spi_ptr = SPI0;
	 
 
 
	spi_ptr->CTRL.SPIRST = 1;		//复位
	while(1 == (spi_ptr->CTRL.SPIRST));

	spi_ptr->CTRL.RXFIFORST = 1;//接收fifo复位
	while(1 == (spi_ptr->CTRL.RXFIFORST));

	spi_ptr->CTRL.TXFIFORST = 1;//发送fifo复位
	while(1 == (spi_ptr->CTRL.TXFIFORST));

	//FIFO TH SET
	spi_ptr->CTRL.TXTHRES = 1;  //设置发送接收fifo阈值
	spi_ptr->CTRL.RXTHRES = 1;
 
	// 开启 SPI0 接收/发送 DMA 请求
	spi_ptr->CTRL.RXDMAEN = 1;
	spi_ptr->CTRL.TXDMAEN = 1;
 
	//transmit mode
	spi_ptr->TRANSFMT.CPOL = 0;	  //模式0
	spi_ptr->TRANSFMT.CPHA = 0;

	spi_ptr->TRANSFMT.SLVMOD = 0;		//主模式

	//address/data mode
	spi_ptr->TRANSFMT.ADDRLEN = 1; //地址长度
	spi_ptr->TRANSFMT.DATALEN = 8-1; //数据长度  bit位配合DMA bit位
	spi_ptr->TRANSFMT.DATAMERGE = 0; //数据合并模式
 


	//transmit config
	spi_ptr->TRANSCTRL.CMDEN = 0; //禁用命令
	spi_ptr->TRANSCTRL.ADDREN = 0; //禁用地址相位

	spi_ptr->TRANSCTRL.WRTRANCNT = len - 1;  //spi发送传输数据单元数量
	spi_ptr->TRANSCTRL.TRANSMODE = SPI_TRANS_MODE_WR_ONLY;  //只读模式
	
	
	spi_ptr->TIMING.SCLK_DIV = 0;
	spi_ptr->ADDR.ADDR = 0x3;

	
	spi_ptr->TIMING.SCLK_DIV = ( HIRC_FREQ / spi_clk) / 2.0 - 1.0;
						 
	
	
	DMA_IT_SPI_Config(DMA_CHANNEL_0,0,Txbuf,len,0,SPI_TX,DMA_PRIORITY_MID);
}
 

void HAL_DMA_SPI_Rx_Init(uint8_t *Rxbuf,uint32_t spi_clk,uint16_t len)
{	
//	#ifndef SPI_TEST   //没定义spi则手动执行gpio初始化
//	spi0_gpio_init();
//	#endif
	SPI_TypeDef *spi_ptr = SPI0;
	 
	uint32_t sclk_div;
 
	spi_ptr->CTRL.SPIRST = 1;		//复位
	while(1 == (spi_ptr->CTRL.SPIRST));

	spi_ptr->CTRL.RXFIFORST = 1;//接收fifo复位
	while(1 == (spi_ptr->CTRL.RXFIFORST));

	spi_ptr->CTRL.TXFIFORST = 1;//发送fifo复位
	while(1 == (spi_ptr->CTRL.TXFIFORST));

	//FIFO TH SET
	spi_ptr->CTRL.TXTHRES = 4;  //设置发送接收fifo阈值
	spi_ptr->CTRL.RXTHRES = 1;
 
	// 开启 SPI0 接收/发送 DMA 请求
    SPI0->CTRL.RXDMAEN = 1;
    SPI0->CTRL.TXDMAEN = 1;

    // RX FIFO 阈值：收到1字节就触发 DMA 请求
    SPI0->CTRL.RXTHRES = 1;
//	spi_ptr->INTREN.W = 0x00;
//	spi_ptr->INTRST.W = 0xff;

	//transmit mode
	spi_ptr->TRANSFMT.CPOL = 0;	  //模式0
	spi_ptr->TRANSFMT.CPHA = 0;

	spi_ptr->TRANSFMT.SLVMOD = 0;		//主模式

	//address/data mode
	spi_ptr->TRANSFMT.ADDRLEN = 1; //地址长度
	spi_ptr->TRANSFMT.DATALEN = 8-1; //数据长度  bit位配合DMA bit位
	spi_ptr->TRANSFMT.DATAMERGE = 0; //数据合并模式
 


	//transmit config
	spi_ptr->TRANSCTRL.CMDEN = 0; //禁用命令
	spi_ptr->TRANSCTRL.ADDREN = 0; //禁用地址相位

	spi_ptr->TRANSCTRL.RDTRANCNT = len - 1;  //spi传输数据单元数量
	spi_ptr->TRANSCTRL.TRANSMODE = SPI_TRANS_MODE_RD_ONLY;  //只读模式
 
	
	
	spi_ptr->ADDR.ADDR = 0x3;

	
	spi_ptr->TIMING.SCLK_DIV = 0;					 
	sclk_div = spi_clk;
	sclk_div = ((double)HIRC_FREQ / sclk_div) / 2.0 - 1.0;
	spi_ptr->TIMING.SCLK_DIV = (uint8_t)sclk_div;
 
 
	DMA_IT_SPI_Config(DMA_CHANNEL_1,Rxbuf,0,len,0,SPI_RX,DMA_PRIORITY_MID);
 
}

void HAL_DMA_SPI_Tx_Rx_Init(uint8_t *Txbuf,uint8_t *Rxbuf,uint32_t spi_clk,uint16_t len)
{
//	#ifndef SPI_TEST
//	spi0_gpio_init();
//	#endif
	//	spi0_gpio_init();
	SPI_TypeDef *spi_ptr = SPI0;
	 
//	uint32_t sclk_div = 0;
 
	spi_ptr->CTRL.SPIRST = 1;		//复位
	while(1 == (spi_ptr->CTRL.SPIRST));

	spi_ptr->CTRL.RXFIFORST = 1;//接收fifo复位
	while(1 == (spi_ptr->CTRL.RXFIFORST));

	spi_ptr->CTRL.TXFIFORST = 1;//发送fifo复位
	while(1 == (spi_ptr->CTRL.TXFIFORST));

	//FIFO TH SET
	spi_ptr->CTRL.TXTHRES = 1;  //设置发送接收fifo阈值 小于这个fifo就会将数据搬运过去
	spi_ptr->CTRL.RXTHRES = 1;
 
	
 
	//transmit mode
	spi_ptr->TRANSFMT.CPOL = 0;	  //模式0
	spi_ptr->TRANSFMT.CPHA = 0;

	spi_ptr->TRANSFMT.SLVMOD = 0;		//主模式

	//address/data mode
	spi_ptr->TRANSFMT.ADDRLEN = 1; //地址长度
	spi_ptr->TRANSFMT.DATALEN = 8-1; //数据长度  bit位配合DMA bit位
	spi_ptr->TRANSFMT.DATAMERGE = 0; //数据合并模式
 


	//transmit config
	spi_ptr->TRANSCTRL.CMDEN = 0; //禁用命令
	spi_ptr->TRANSCTRL.ADDREN = 0; //禁用地址相位

	spi_ptr->TRANSCTRL.WRTRANCNT = len - 1;  //spi发送传输数据单元数量
	spi_ptr->TRANSCTRL.TRANSMODE = SPI_TRANS_MODE_WR_SYNC;  //同时读写
	spi_ptr->TRANSCTRL.RDTRANCNT = len - 1;  //spi读取传输数据单元数量
	
	spi_ptr->TIMING.SCLK_DIV = 0;
	spi_ptr->ADDR.ADDR = 0x3;

//	sclk_div = spi_clk;
 
	spi_ptr->TIMING.SCLK_DIV = ( HIRC_FREQ / spi_clk ) / 2.0 - 1.0;
	
	DMA_IT_SPI_Config(DMA_CHANNEL_0,0,Txbuf,len,0,SPI_TX,DMA_PRIORITY_HIGH); //发送优先级高，接收优先级低
	DMA_IT_SPI_Config(DMA_CHANNEL_1,Rxbuf,0,len,0,SPI_RX,DMA_PRIORITY_MID);  //配置双通道同时进行，通道0发送spi，通道1读取spi
	// 开启 SPI0 接收/发送 DMA 请求
	spi_ptr->CTRL.RXDMAEN = 1;
	spi_ptr->CTRL.TXDMAEN = 1;
	SPI0->CMD.CMD = 0x03;  //同时收发只发送一次
	uint32_t cnt = 0;
	while(HAL_DMA_GetRemainingCnt(DMA_CHANNEL_0) > 0)
	{
			// 超时判断，防止硬件异常导致死循环
			if(cnt++ > 0xFFFFFU)
			{
				INFO("\n DMA_ch0 time error--->>cnt = %d\n",HAL_DMA_GetRemainingCnt(DMA_CHANNEL_0));
				cnt = 0;
					return;
			}
	}
	while(HAL_DMA_GetRemainingCnt(DMA_CHANNEL_1) > 0)
	{
			// 超时判断，防止硬件异常导致死循环
			if(cnt++ > 0xFFFFFU)
			{
				INFO("\n DMA_ch1 time error--->>cnt = %d\n",HAL_DMA_GetRemainingCnt(DMA_CHANNEL_1));
				cnt = 0;
					return;
			}
	}		
}

/******************************  SPI  slave  *******************************************/
void DMA_IT_SPI_SLAVE_Config(DMA_Channel_e dma_channel,uint8_t *Rxbuf,uint8_t *Txbuf,uint16_t len,uint8_t spi_mode,spi_addr_e spi_addr,dma_priority_t dma_priority)
{
//		#ifndef SPI_TEST   //没定义spi则手动执行gpio初始化
//		spi0_gpio_init();
//		#endif
    // 定义DMA配置结构体（局部变量初始化，避免脏数据、野值）
    dma_ch_config_t dma_handle ;
		memset(&dma_handle,0,sizeof(dma_handle));
    /* ====================== 1. SPI DMA 硬件使能 ====================== */
    SPI_TypeDef *spi_ptr = SPI0;

    /* ====================== 2. DMA 基础配置 ====================== */
    dma_handle.dma_ch              = dma_channel;        
    dma_handle.circular_mode       = DMA_MODE_CIRCULAR;     
    
    
    dma_handle.burst_len           = DMA_BURST_1_UNIT;     // 单次传输1个数据单元

    /* ====================== 3. 源/目标地址配置 ====================== */
    if(spi_addr == SPI_TX)  // TX 模式：内存 -> SPI->DATA
    {
				dma_handle.DMA_Trigger_periph  = DMA_Trigger_SPI0_TX;   
        dma_handle.src_addr = (uint32_t)Txbuf;             
        dma_handle.dst_addr = (uint32_t)&SPI0->DATA; 
				dma_handle.dir                 = DMA_DIR_MEM_TO_PERIPH;// 默认方向：外设→内存（RX）
				/* ====================== 4. 地址递增模式 ====================== */
				// 外设地址（SPI DATA）固定不变，内存地址（缓冲区）自动递增
				dma_handle.src_inc             = Addr_Enable;         
				dma_handle.dst_inc             = Addr_Disable;   
						/* ====================== 5. 数据位宽配置 ====================== */
				// 与SPI数据位宽保持一致 
				dma_handle.dma_data_width_src_width = DMA_BITSIZE_8BIT;
				dma_handle.dma_data_width_dst_width = DMA_BITSIZE_8BIT;
    }
    else  if(spi_addr == SPI_RX)   // RX 模式：SPI->DATA -> 内存
    {
				dma_handle.DMA_Trigger_periph  = DMA_Trigger_SPI0_RX;   
        dma_handle.src_addr = (uint32_t)&SPI0->DATA;       	    
        dma_handle.dst_addr = (uint32_t)Rxbuf;  
						/* ====================== 4. 地址递增模式 ====================== */
				// 外设地址（SPI DATA）固定不变，内存地址（缓冲区）自动递增
				dma_handle.src_inc             = Addr_Disable;         // 外设地址不自增
				dma_handle.dst_inc             = Addr_Enable;         // 内存地址自增
				dma_handle.dir                 = DMA_DIR_PERIPH_TO_MEM;// 默认方向：外设→内存（RX）
				/* ====================== 5. 数据位宽配置 ====================== */
				// 与SPI数据位宽保持一致 
				dma_handle.dma_data_width_src_width = DMA_BITSIZE_8BIT;
				dma_handle.dma_data_width_dst_width = DMA_BITSIZE_8BIT;
    }

    

    

    /* ====================== 6. 优先级 ====================== */
    dma_handle.priority            = dma_priority;     // DMA通道中等优先级
    dma_handle.priority_it         = 1;                    // 中断优先级

    /* ====================== 7. 传输长度 ====================== */
    dma_handle.transfer_size       = len;                  // 设置本次传输长度

    /* ====================== 8. 配置前先关闭通道 ====================== */
    // 必须先关闭，否则寄存器无法写入
    DMA_div_chx_Disable(dma_handle.dma_ch);

    // 清除所有触发源配置，恢复默认状态，避免通道冲突
    DMA_div_TriggerSource_Clean();

    // 手动配置通道1触发源选择（硬件映射）
//    DMA_Type->DMA_CHCFG1_STR.Bits.CH1SEL = 0x13;

    /* ====================== 9. 加载DMA配置到硬件 ====================== */
    HAL_DMA_StructerInit(&dma_handle);

    /* ====================== 10. DMA 中断配置 ====================== */
    // 使能DMA通道中断（NVIC）
    HAL_DMA_NVIC_Enable(dma_handle.dma_ch, dma_handle.priority);

    // 使能：传输完成中断 + 传输错误中断 + 配置错误中断
    HAL_DMA_IT_Transfer_Set(dma_channel, dma_it_enable);
    HAL_DMA_IT_TransError_Set(dma_channel, dma_it_enable);
    HAL_DMA_IT_Configerror_Set(dma_it_enable);

    // 配置DMA全局中断
    IRQn_Type IRQ_DMA = DMA_IRQn;
    NVIC_SetPriority(IRQ_DMA, dma_handle.priority_it);
    NVIC_ClearPendingIRQ(IRQ_DMA);
    NVIC_EnableIRQ(IRQ_DMA);
		

    /* ====================== 11. 启动DMA通道 ====================== */
    DMA_div_chx_Enable(dma_handle.dma_ch);

    /* ====================== 12. 等待DMA传输完成（超时保护） ====================== */
    INFO("\n DMA time error--->>cnt = %d\n",HAL_DMA_GetRemainingCnt(dma_handle.dma_ch));
    

    /* ====================== 13. 初始化完成 ====================== */
    INFO("DMA Channel %d initialized for SPI0 (size: %d * 32bit)\n",
         dma_channel, dma_handle.transfer_size);
}


//MCU 作为从机SPI读取数据  将数据读到SRAM
void HAL_DMA_SPI_Slave_Tx_Init(uint8_t *Txbuf,uint16_t len)
{
//	#ifndef SPI_TEST   //没定义spi则手动执行gpio初始化
//	spi0_gpio_init();
//	#endif
	SPI_TypeDef *spi_ptr = SPI0; 
 //init spi and tx fifo and rx fifo
			spi_ptr->CTRL.SPIRST = 1;
	  spi_ptr->CTRL.RXFIFORST = 1;
	  spi_ptr->CTRL.TXFIFORST = 1;
 
 
	spi_ptr->TRANSFMT.SLVMOD = 1;		//must to need set frist
	spi_ptr->TRANSFMT.DATALEN = 7;
	spi_ptr->TRANSFMT.LSB = 1;
	spi_ptr->TRANSFMT.DATAMERGE = 0;
	
  spi_ptr->CTRL.TXTHRES = 1;
	spi_ptr->CTRL.RXTHRES = 4; 
 
	
	spi_ptr->TRANSCTRL.TRANSMODE = SPI_TRANS_MODE_WR_ONLY;   
	spi_ptr->TRANSCTRL.WRTRANCNT = len - 1;      //主机发送的长度必须和这个一样，不然乱序
	spi_ptr->TRANSCTRL.DUMMYCNT = 1;   
	
	
	DMA_IT_SPI_SLAVE_Config(DMA_CHANNEL_0,0,Txbuf,len,0,SPI_TX,DMA_PRIORITY_MID);   //发送数据到spi数据寄存器
	spi_ptr->CTRL.TXDMAEN = 1;
}


 
void HAL_DMA_SPI_Slave_Rx_Init(uint8_t *Rxbuf,uint16_t len)
{
//	#ifndef SPI_TEST   //没定义spi则手动执行gpio初始化
//	spi0_gpio_init();
//	#endif
	SPI_TypeDef *spi_ptr = SPI0; 
 //init spi and tx fifo and rx fifo
			spi_ptr->CTRL.SPIRST = 1;
	  spi_ptr->CTRL.RXFIFORST = 1;
	  spi_ptr->CTRL.TXFIFORST = 1;
 
 
	spi_ptr->TRANSFMT.SLVMOD = 1;		//must to need set frist
	spi_ptr->TRANSFMT.DATALEN = 7;
	spi_ptr->TRANSFMT.LSB = 1;
	spi_ptr->TRANSFMT.DATAMERGE = 0;
	
  spi_ptr->CTRL.TXTHRES = 1;
	spi_ptr->CTRL.RXTHRES = 4; 
 
	
	spi_ptr->TRANSCTRL.TRANSMODE = SPI_TRANS_MODE_RD_ONLY;   
	spi_ptr->TRANSCTRL.WRTRANCNT = len - 1;      //主机发送的长度必须和这个一样，不然乱序
	spi_ptr->TRANSCTRL.DUMMYCNT = 1;   
	
	
	DMA_IT_SPI_SLAVE_Config(DMA_CHANNEL_1,Rxbuf,0,len,0,SPI_RX,DMA_PRIORITY_MID);   //发送数据到spi数据寄存器
	spi_ptr->CTRL.RXDMAEN = 1;
}


 
void HAL_DMA_SPI_Slave_Rx_Tx_Init(uint8_t *Txbuf,uint8_t *Rxbuf,uint16_t len)
{
//	#ifndef SPI_TEST   //没定义spi则手动执行gpio初始化
//	spi0_gpio_init();
//	#endif
	SPI_TypeDef *spi_ptr = SPI0; 
	static uint16_t LEN1=0;
	LEN1 = len;
 //init spi and tx fifo and rx fifo
	spi_ptr->CTRL.SPIRST = 1;
	spi_ptr->CTRL.RXFIFORST = 1;
	spi_ptr->CTRL.TXFIFORST = 1;
 
 
	spi_ptr->TRANSFMT.SLVMOD = 1;		//must to need set frist
	spi_ptr->TRANSFMT.DATALEN = 7;
	spi_ptr->TRANSFMT.LSB = 1;
	spi_ptr->TRANSFMT.DATAMERGE = 0;
	
  spi_ptr->CTRL.TXTHRES = 2;
	spi_ptr->CTRL.RXTHRES = 8; 
 
	
	spi_ptr->TRANSCTRL.TRANSMODE = SPI_TRANS_MODE_WR_SYNC;  
	spi_ptr->TRANSCTRL.RDTRANCNT = (LEN1 - 1)%0x1FF;      //主机发送的长度必须和这个一样，不然乱序
	spi_ptr->TRANSCTRL.DUMMYCNT = 1;   					 //必须加，，不然第一个数据被吞======Slave 模式下数据线默认拉高，，会将第一个数据第一位bit拉高
	spi_ptr->TRANSCTRL.WRTRANCNT = (LEN1 - 1)%0x1FF; 
	//寄存器9位，最大0x1FF  511
	
	DMA_IT_SPI_SLAVE_Config(DMA_CHANNEL_0,0,Txbuf,len,0,SPI_TX,DMA_PRIORITY_HIGH);    
	DMA_IT_SPI_SLAVE_Config(DMA_CHANNEL_1,Rxbuf,0,len,0,SPI_RX,DMA_PRIORITY_LOW);    //通道1 Rx  通道0 Tx
	spi_ptr->CTRL.RXDMAEN = 1;
	spi_ptr->CTRL.TXDMAEN = 1;
}











//====================================   UART  ==================================//
#include "uart_reg.h"
UART_TypeDef *uart1 = UART1;
UART_TypeDef *uart0 = UART0;
//白色接收 绿色发送  ch343
void DMA_IT_UART_Config(DMA_Channel_e dma_channel,uint8_t *Rxbuf,uint8_t *Txbuf,uint16_t len,uart_addr_e uart_addr,dma_priority_t dma_priority,dma_mode_t dma_mode)
{
#ifndef UART_TEST   //没定义spi则手动执行gpio初始化
//		uart1_gpio_init(); 
#endif
	// 定义DMA配置结构体（局部变量初始化，避免脏数据、野值）
	
	uart1->CR_STR.RXFE = 1;	//使能接收FIFO
	uart1->CR_STR.RXE = 1;
  
	uart1->CR_STR.TDMAE = 1;
	uart1->CR_STR.RDMAE = 1;	//使能DMA
	
	uart1->IR_STR.TXUDF = 1; //TXFIFO最后一个数据读取完成，并发送
	uart1->IR_STR.RXUDF = 1;  
 
	
	uart1->WATER_STR.TXWATER = 1;  // 发送水位 = 1（FIFO≤1时触发DMA）
	uart1->WATER_STR.RXWATER = 1;
	
	dma_ch_config_t dma_handle ;
	memset(&dma_handle,0,sizeof(dma_handle));
	/* ====================== 1. SPI DMA 硬件使能 ====================== */
 

	/* ====================== 2. DMA 基础配置 ====================== */
	dma_handle.dma_ch              = dma_channel;          // 绑定当前使用的DMA通道
	dma_handle.circular_mode       = dma_mode;      // 普通模式，非循环传输
 
	dma_handle.burst_len           = DMA_BURST_1_UNIT;     // 单次传输1个数据单元

	/* ====================== 3. 源/目标地址配置 ====================== */
	if(uart_addr == UART_TX)   
	{
			dma_handle.DMA_Trigger_periph  = DMA_Trigger_UART1_TX;   
			dma_handle.src_addr = (uint32_t)Txbuf;             
			dma_handle.dst_addr = (uint32_t)&uart1->DR_STR;   //十位，传输16bit半字
		
			dma_handle.dir     = DMA_DIR_MEM_TO_PERIPH; 
			/* ====================== 4. 地址递增模式 ====================== */
			// 外设地址（SPI DATA）固定不变，内存地址（缓冲区）自动递增
			dma_handle.src_inc             = Addr_Enable;         
			dma_handle.dst_inc             = Addr_Disable;   
					/* ====================== 5. 数据位宽配置 ====================== */

			
	}
	else  if(uart_addr == UART_RX)   // RX 模式 
	{
			dma_handle.DMA_Trigger_periph  = DMA_Trigger_UART1_RX;   
			dma_handle.src_addr = (uint32_t)&uart1->DR_STR;      	    
			dma_handle.dst_addr = (uint32_t)Rxbuf;  
					/* ====================== 4. 地址递增模式 ====================== */
			// 外设地址（SPI DATA）固定不变，内存地址（缓冲区）自动递增
			dma_handle.src_inc             = Addr_Disable;         // 外设地址不自增
			dma_handle.dst_inc             = Addr_Enable;         // 内存地址自增
			dma_handle.dir                 = DMA_DIR_PERIPH_TO_MEM;// 默认方向：外设→内存（RX）
			
	}

	/* ====================== 5. 数据位宽配置 ====================== */
		 
			dma_handle.dma_data_width_src_width = DMA_BITSIZE_8BIT;
			dma_handle.dma_data_width_dst_width = DMA_BITSIZE_8BIT;

	/* ====================== 6. 优先级 ====================== */
	dma_handle.priority            = dma_priority;     // DMA通道中等优先级
	dma_handle.priority_it         = 1;                    // 中断优先级

	/* ====================== 7. 传输长度 ====================== */
	dma_handle.transfer_size       = len;                  // 设置本次传输长度

	/* ====================== 8. 配置前先关闭通道 ====================== */
	// 必须先关闭，否则寄存器无法写入
	DMA_div_chx_Disable(dma_handle.dma_ch);

	// 清除所有触发源配置，恢复默认状态，避免通道冲突
	DMA_div_TriggerSource_Clean();



	/* ====================== 9. 加载DMA配置到硬件 ====================== */
	HAL_DMA_StructerInit(&dma_handle);

	/* ====================== 10. DMA 中断配置 ====================== */
	// 使能DMA通道中断（NVIC）
	HAL_DMA_NVIC_Enable(dma_handle.dma_ch, dma_handle.priority);

	// 使能：传输完成中断 + 传输错误中断 + 配置错误中断
	HAL_DMA_IT_Transfer_Set(dma_channel, dma_it_enable);
	HAL_DMA_IT_TransError_Set(dma_channel, dma_it_enable);
	HAL_DMA_IT_Configerror_Set(dma_it_enable);

	// 配置DMA全局中断
	IRQn_Type IRQ_DMA = DMA_IRQn;
	NVIC_SetPriority(IRQ_DMA, dma_handle.priority_it);
	NVIC_ClearPendingIRQ(IRQ_DMA);
	NVIC_EnableIRQ(IRQ_DMA);
	
	
	/* ====================== 11. 启动DMA通道 ====================== */
	DMA_div_chx_Enable(dma_handle.dma_ch);

	/* ====================== 12. 等待DMA传输完成（超时保护） ====================== */
	if(uart_addr == UART_TX)
	{
		uint32_t cnt = 0;
		while(HAL_DMA_GetRemainingCnt(dma_handle.dma_ch) > 0)
		{
				// 超时判断，防止硬件异常导致死循环
				if(cnt++ > 0xFFFFFU)
				{
					INFO("\n DMA time error--->>cnt = %d\n",HAL_DMA_GetRemainingCnt(dma_handle.dma_ch));
						return;
				}
		}
	}
	

	/* ====================== 13. 初始化完成 ====================== */
	INFO("DMA Channel %d initialized for UART (size: %d * 32bit)\n",
			 dma_channel, dma_handle.transfer_size);
}

