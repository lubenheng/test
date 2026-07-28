#include "platform_common.h"


#include "cmdline_internal.h"
#include "dma_cmdline.h"
#include "dma_reg.h"
#include "dma_hal.h"
#include "tim_hal.h" 
#include "dma_test.h" 

uint16_t tx_buf[bufsize];  //i2c
uint8_t Rx_buf[bufsize];
uint16_t MCPWM_Tx_buff[MCPWM_LEN];  //mcpwm
uint16_t MCPWM_Rx_buff[MCPWM_LEN]; 

uint32_t TIMER_DMA_Tx_buff[TIMER_LEN] ;   //timer
uint32_t TIMER_DMA_Rx_buff[TIMER_LEN] ; 

uint8_t SPI_DMA_Rx_buff[SPI_LEN] ;    //spi
uint8_t SPI_DMA_Tx_buff[SPI_LEN]; 

uint8_t uart_tx_buf[UART_LEN] = {0}; 
uint8_t uart_rx_buf[UART_LEN]	= {0}; 

extern volatile uint16_t freq_len_it;   //DMA中断数量影响

// ch reg cmd  len  buff
//cmd_dma_sendI2c_test 0(ch) 0x44(reg)   8(len) 0x01 0x02 0x0A 0x02 0x06 0xFF 0xFE 0xF5
//串口原因 只能输入29个数据
int cmd_dma_sendI2c_test(int argc, char *argv[])
{
	uint8_t case2 = get_arg_ulong(argv[0]);
	DMA_Channel_e ch =  (DMA_Channel_e)get_arg_ulong(argv[1]);
	uint8_t reg = get_arg_ulong(argv[2]);
  INFO("reg :%d \n",reg);
	uint16_t datalen = get_arg_ulong(argv[3]);
	
	
	if(datalen > bufsize)
	{
		INFO("argv[3] error\n");
		return 0;
	}
	if(I2C0->IC_ENABLE.ENABLE != 1)
	{
		INFO("not open the I2C  \n");
		return 0;
	}
 
	
	I2C_DMA_SendData(ch,reg,0,tx_buf,datalen);
	 
	INFO("Pass !!\n");
	return 0;	
}

//cmd_dma_recI2c_test ch reg  len 
int cmd_dma_recI2c_test(int argc, char *argv[])
{
	uint8_t case2 = get_arg_ulong(argv[0]);
	DMA_Channel_e ch =  (DMA_Channel_e)get_arg_ulong(argv[1]);
	uint8_t reg = get_arg_ulong(argv[2]);
	uint16_t datalen = get_arg_ulong(argv[3]);
	if(argc != 4)
	{
		INFO("input error\n");
		return 0;
	}
	
	if(datalen > bufsize)
	{
		INFO("argv[3] error\n");
		return 0;
	}
	if(I2C0->IC_ENABLE.ENABLE != 1)
	{
		INFO("not open the I2C  \n");
		return 0;
	}
	memset(Rx_buf,0,sizeof(Rx_buf));
	I2C_DMA_RecData(ch,reg,1,Rx_buf,datalen);  
	 
	INFO("Pass !!\n");
	return 0;	
}


//cmd_dma_enable_test 0 1 
int cmd_dma_enable_test(int argc, char *argv[])
{
	if( argc != 3)
	{
		INFO("input error \n");
		return 0;
	}
	uint8_t case2 = get_arg_ulong(argv[0]);
	uint8_t ch =  get_arg_ulong(argv[1]);
	uint8_t cmd = get_arg_ulong(argv[2]);   //1 enable 0 disable
	
	cmd ? HAL_DMA_chx_Enable((DMA_Channel_e) ch) :HAL_DMA_chx_Disable((DMA_Channel_e) ch);
 
	INFO("Pass !!\n");
	return 0;	
}




//cmd_dma_modeInit_test 方向 通道 从机地址 速度
int cmd_dma_modeInit_test(int argc, char *argv[])
{
	if( argc != 5)
	{
		INFO("input error \n");
		return 0;
	}
	uint8_t case2 = get_arg_ulong(argv[0]);
	dma_direction_t dir =  (dma_direction_t)get_arg_ulong(argv[1]); //内存内存 内存外设写  外设内存读
	DMA_Channel_e ch = (DMA_Channel_e)get_arg_ulong(argv[2]);  
	uint32_t slave_addr = get_arg_ulong(argv[3]);
	uint8_t speed = get_arg_ulong(argv[4]);
	i2c_clk_enable();
	
  switch(dir)
	{
		case DMA_DIR_PERIPH_TO_MEM:INFO("[DMA_TEST] Start peripheral memory transfer \n" );
		DMA_IT_I2C_Rx_text(ch,slave_addr,speed,Rx_buf,bufsize); 
    break;
		case DMA_DIR_MEM_TO_PERIPH: 
		INFO("[DMA_TEST] Start memory peripheral  transfer \n" );

		DMA_IT_I2C_Tx_text(ch,slave_addr,speed,tx_buf,bufsize);
		
    break;
		case DMA_DIR_MEM_TO_MEM : 
		INFO("[DMA_TEST] Start memory memory DMA transfer test \n" );
		DMA_IT_M_TO_M_text(ch);
		
		break;		/* 2b: 内存到内存 */
	}

	INFO("Pass !!\n");
	return 0;	
}


//cmd_dma_setfifo_test ch Level rx_or_tx 
int cmd_dma_setfifo_test(int argc, char *argv[])
{
	if( argc != 3)
	{
		INFO("input error \n");
		return 0;
	}
	uint8_t case2 = get_arg_ulong(argv[0]);
	DMA_Channel_e ch =  (DMA_Channel_e)get_arg_ulong(argv[1]);
	uint8_t Level = get_arg_ulong(argv[2]);   //1 enable 0 disable
	uint8_t rx_or_tx =  get_arg_ulong(argv[3]);
	HAL_DMA_chx_Disable(ch);
	if(rx_or_tx == 1)  //tx
		I2C_DMA_SetTxLevel(I2C0,Level);			//当 I2C TX FIFO 的空闲空间 ≥ 4 时，会触发 DMA 发送请求
	else 
		I2C_DMA_SetRxLevel(I2C0,Level);			//当 I2C TX FIFO 的空闲空间 ≥ 4 时，会触发 DMA 发送请求
	HAL_DMA_chx_Enable(ch);
	INFO("Pass !!\n");
	return 0;	
}


/**************************************************************************************************
 * 函 数 名：cmd_dma_mcpwm0_test
argv[0] → 命令类型（固定 0）
argv[1] → DMA 通道号
argv[2] → MCPWM 通道号0  1 2 
argv[3] → MCPWM 请求源RE
argv[4] → DMA 模式（0=单次 / 1=循环）
argv[5] → DMA 目的地址  -- 修改占空比0 死区 1
argv[6] → 长度（60 = 60 组占空比数据）
 * 调 用 者：命令行控制台
 * 输    入：argc - 命令参数个数
 *           argv - 命令参数字符串数组
 * 返 回 值：0 - 执行完成
 * 说    明：固定配置为：TH00=0%占空比，通过DMA动态修改TH01实现PWM波形呼吸/渐变效果
 * cmd_dma_mcpwm0_test 0 0 0 0 0 60			DMA通道0,mcpwm0修改占空比
 * cmd_dma_mcpwm0_test 5 2 0 0 1 25     DMA通道5,mcpwm0修改mcpwm通道2 死区时间
 *************************************************************************************************/
int cmd_dma_mcpwm0_test(int argc, char *argv[])
	{
    // 参数个数校验
    if( argc != 7)
    {
        INFO("input error \n");
        return 0;
    }

    // 打印输入参数，用于调试查看
    for(int i =0;i<argc;i++)
    {
        INFO("argv[%d] : %s \n",i,argv[i]);
    }

    // 先关闭MCPWM0，防止配置期间输出异常波形
    MCPWM_Dsiable(0);

    // 解析命令行输入的配置参数
    uint8_t case2 = get_arg_ulong(argv[0]);              		// 命令类型（固定为0ch）
    DMA_Channel_e ch =  (DMA_Channel_e)get_arg_ulong(argv[1]); 		// DMA通道号
		MCPWM_DMA_CH mcpwmch =  (MCPWM_DMA_CH)get_arg_ulong(argv[2]); 		// mcpwm通道号
    MCPWM_RE_DMA RE_DMA =  (MCPWM_RE_DMA)get_arg_ulong(argv[3]); // MCPWM DMA触发请求源(REQ0/REQ1)
    dma_mode_t DMA_RT =   (dma_mode_t)get_arg_ulong(argv[4]);	// DMA模式：单次/循环
    MCPWM_DMA_Dst DMA_dst =   (MCPWM_DMA_Dst)get_arg_ulong(argv[5]);	// DMA目标地址配置
    uint16_t len =   get_arg_ulong(argv[6]);              	  // DMA数据传输长度（波形点数）

    // 使能MCPWM0开始输出PWM
    MCPWM_Enable(0);
		if(DMA_dst == dst_addrs_duty)  //0			无保护
		{
			if(len > Duty_len)
			{
				INFO("Duty_len error  :%d\n",Duty_len);
				return -1;
			}
			DMA_IT_MCPWM_Config(ch,mcpwmch,RE_DMA,DMA_RT,DMA_dst,MCPWM_Duty_buff,len);
		}
		else if(DMA_dst == dst_addrs_deadtime)
		{
			if(len > Deadtime_len)
			{
				INFO("Deadtime_len error :%d\n",Deadtime_len);
				return -1;
			}
			DMA_IT_MCPWM_Config(ch,mcpwmch,RE_DMA,DMA_RT,DMA_dst,Deadtime_buff,len);		//保护
		}
 // 输出提示信息：500Hz PWM，DMA动态修改TH01占空比
    INFO("MCPWM DMA success 500Hz  !!\n");

    return 0;
}


/**
 * @brief MCPWM 硬件触发 DMA，将 SRAM 中的频率表搬运到 MCPWM 寄存器（更新 TH0）
 * 
 * @details 功能特点：
 *          1. 一次传输 1 个数据，传输完成中断中修改目标地址
 *          2. 由于 THx01 地址连续，开启目的地址自增
 *          3. 循环搬完所有数据后自动停止 DMA
 * 
 * @param argc 参数个数，必须为 5
 * @param argv 参数列表：
 *             argv[0] - 命令类型（固定为 0ch）
 *             argv[1] - DMA 通道号（DMA_Channel_e 类型）
 *             argv[2] - MCPWM 通道号（MCPWM_DMA_CH 类型）
 *             argv[3] - DMA 传输模式（dma_mode_t 类型：单次/循环）
 *             argv[4] - 数据传输长度（波形点数，不能超过 freq_len）
 * 
 * @return 0: 成功; -1: 参数错误
 */
int cmd_dma_mcpwm0_set_freqtest(int argc, char *argv[])
{
		INFO("Turn on the logic analyzer when there is no response \n");
    // 参数个数校验
    if( argc != 5)
    {
        INFO("input error \n");
        return -1;
    }

    // 打印输入参数，用于调试查看
    for(int i =0;i<argc;i++)
    {
        INFO("argv[%d] : %s \n",i,argv[i]);
    }

    // 先关闭MCPWM0，防止配置期间输出异常波形
    MCPWM_Dsiable(0);

    // 解析命令行输入的配置参数
    uint8_t case2 = get_arg_ulong(argv[0]);              		// 命令类型（固定为0ch）
    DMA_Channel_e ch =  (DMA_Channel_e)get_arg_ulong(argv[1]); 		// DMA通道号
		MCPWM_DMA_CH mcpwmch =  (MCPWM_DMA_CH)get_arg_ulong(argv[2]); 		// mcpwm通道
    dma_mode_t DMA_RT =   (dma_mode_t)get_arg_ulong(argv[3]);	// DMA模式：单次/循环
    freq_len_it =   get_arg_ulong(argv[4]);              	  // DMA数据传输长度（波形点数）
    
    if(freq_len_it > freq_len)
		{
			INFO("freq len input error \n");
			return -1;
		}
		MCPWM_Enable(0);
		DMA_IT_MCPWM_SetFreq_Config(ch,mcpwmch,freq_buff,DMA_RT);
    
 
    INFO("MCPWM DMA success   !!\n");

    return 0;
}


/**
 * @brief DMA 内存到内存传输测试（SRAM -> SRAM）
 * 
 * @attention 注意：此模式只能软件触发，无法被 MCPWM 硬件触发
 * 
 * @param argc 参数个数，必须为 4
 * @param argv 参数列表：
 *             argv[0] - 命令类型（固定为 0ch）
 *             argv[1] - DMA 通道号（DMA_Channel_e 类型，范围：DMA_CHANNEL_0 ~ DMA_CHANNEL_5）
 *             argv[2] - DMA 传输模式（dma_mode_t 类型：单次/循环）
 *             argv[3] - 数据传输长度（字节数，不能超过 MCPWM_LEN）
 * 
 * @return 0: 成功; -1: 参数错误
 */
int cmd_dma_mcpwm0_MemToMem_test(int argc, char *argv[])
{
    // 参数个数校验
    if( argc != 4)
    {
        INFO("input error \n");
        return -1;
    }
		
    // 打印输入参数，用于调试查看
    for(int i =0;i<argc;i++)
    {
        INFO("argv[%d] : %s \n",i,argv[i]);
    }
 
    // 解析命令行输入的配置参数
    uint8_t case2 = get_arg_ulong(argv[0]);              		// 命令类型（固定为0ch）
    DMA_Channel_e ch =  (DMA_Channel_e)get_arg_ulong(argv[1]); 		// DMA通道号
    dma_mode_t DMA_RT =   (dma_mode_t)get_arg_ulong(argv[2]);	// DMA模式：单次/循环
    uint16_t len =   get_arg_ulong(argv[3]);              	  // DMA数据传输长度（波形点数）
    memset(MCPWM_Rx_buff,0,sizeof(MCPWM_Rx_buff));
		if( ch > DMA_CHANNEL_5)
    {
        INFO("ch input error \n");
        return -1;
    }
    if(len > MCPWM_LEN)
		{
			INFO("freq len input error \n");
			return -1;
		}
		
		DMA_IT_MCPWM_MemToMem_Config(ch,DMA_RT,MCPWM_Tx_buff,MCPWM_Rx_buff,len);
		for(int i = 0;i<len;i++)
		{
			INFO("MCPWM_Rx_buff[%d] = %d  ",i,MCPWM_Rx_buff[i]);
		}
 
    INFO("\r\nMCPWM DMA MemToMem success   !!\n");

    return 0;
}




//cmd_dma_timer_cmpx_test 1 0 87 1
//修改通道的占空比
int cmd_dma_timer_cmpx_test(int argc, char *argv[])
{
	
	// 参数个数校验
	if( argc != 5)
	{
			INFO("input error \n");
			return -1;
	}
	
	// 打印输入参数，用于调试查看
	for(int i =0;i<argc;i++)
	{
			INFO("argv[%d] : %s \n",i,argv[i]);
	}
	HAL_TIM_DMA_compare_Init();  //B11 B12   JB12 B13
	// 解析命令行输入的配置参数
	uint8_t case2 = get_arg_ulong(argv[0]);              		// 命令类型（固定为0ch）
	DMA_Channel_e ch =  (DMA_Channel_e)get_arg_ulong(argv[1]); 		// DMA通道号
	dma_mode_t DMA_RT =   (dma_mode_t)get_arg_ulong(argv[2]);	// DMA模式：单次/循环
	uint16_t len =   get_arg_ulong(argv[3]);              	  // DMA数据传输长度（波形点数）
	timer_cmpx_dst_addrr dst_addr =   (timer_cmpx_dst_addrr)get_arg_ulong(argv[4]);  
	if( ch > DMA_CHANNEL_5)
	{
			INFO("ch input error \n");
			return -1;
	}
	if(len > TIMER_LEN)
	{
		INFO("freq len input error \n");
		return -1;
	}
	
	DMA_IT_TIMER_Config(ch,DMA_RT,TIMER_DMA_Tx_buff,dst_addr,len);
	 

	INFO("\r\n TIMER  DMA success !!\n");

    return 0;
}


//cmd_dma_timer_MemToMem_test 0 0 80  
int cmd_dma_timer_MemToMem_test(int argc, char *argv[])
{
	// 参数个数校验
	if( argc != 4)
	{
			INFO("input error \n");
			return -1;
	}
	
	// 打印输入参数，用于调试查看
	for(int i =0;i<argc;i++)
	{
			INFO("argv[%d] : %s \n",i,argv[i]);
	}
	HAL_TIM_DMA_compare_Init();  //B11 B12   JB12 B13
	// 解析命令行输入的配置参数
	uint8_t case2 = get_arg_ulong(argv[0]);              		// 命令类型（固定为0ch）
	DMA_Channel_e ch =  (DMA_Channel_e)get_arg_ulong(argv[1]); 		// DMA通道号
	dma_mode_t DMA_RT =   (dma_mode_t)get_arg_ulong(argv[2]);	// DMA模式：单次/循环
	uint16_t len =   get_arg_ulong(argv[3]);              	  // DMA数据传输长度（波形点数）
  memset(TIMER_DMA_Rx_buff,0,sizeof(TIMER_DMA_Rx_buff));
	if( ch > DMA_CHANNEL_5)
	{
			INFO("ch input error \n");
			return -1;
	}
	if(len > TIMER_LEN)
	{
		INFO("freq len input error \n");
		return -1;
	}
	
	DMA_IT_TIMER_MemToMem_Config(ch,DMA_RT, TIMER_DMA_Tx_buff,TIMER_DMA_Rx_buff,len);
	 

	INFO("\r\n TIMER  DMA success !!\n");

    return 0;
}

 
	 
int cmd_dma_spi_rx_test(int argc, char *argv[])
{	  
	// 参数个数校验
	if( argc != 3)
	{
			INFO("input error \n");
			return -1;
	}
	uint8_t case2 = get_arg_ulong(argv[0]);              		// 命令类型（固定为0ch）
	uint16_t len =   get_arg_ulong(argv[1]);              	  // DMA数据传输长度（波形点数）
	uint32_t clk =   get_arg_ulong(argv[2]);
	if(len > SPI_LEN) 
	{
		INFO("len error\n");
		return -1;
	}
	
  memset(SPI_DMA_Rx_buff,0,sizeof(SPI_DMA_Rx_buff));
	
	HAL_DMA_SPI_Rx_Init(SPI_DMA_Rx_buff,clk,len);   //通道1接收spi
	for(int i = 0;i<len;i++)
	{
			INFO("rxdata[%d] = %d  ",i,SPI_DMA_Rx_buff[i]);
	}	
	
	INFO("\r\n SPI Rx  DMA success !!\n");

    return 0;
}

int cmd_dma_spi_tx_test(int argc, char *argv[])
{	  
	// 参数个数校验
	if( argc != 3)
	{
			INFO("input error \n");
			return -1;
	}
	uint8_t case2 = get_arg_ulong(argv[0]);              		// 命令类型（固定为0ch）
	uint16_t len =   get_arg_ulong(argv[1]);              	  // DMA数据传输长度（波形点数）
	uint32_t clk =   get_arg_ulong(argv[2]);
	if(len > SPI_LEN) 
	{
		INFO("len error\n");
		return -1;
	}
	
	for(int i = 0;i<len;i++)
	{
		SPI_DMA_Tx_buff[i] = i;
	}
	
	HAL_DMA_SPI_Tx_Init(SPI_DMA_Tx_buff,clk,len);   //通道1接收spi
	
	
	INFO("\r\n SPI Rx  DMA success !!\n");

    return 0;
}

int cmd_dma_spi_rx_tx_test(int argc, char *argv[])
{	  
	// 参数个数校验
	if( argc != 3)
	{
			INFO("input error \n");
			return -1;
	}
	uint8_t case2 = get_arg_ulong(argv[0]);              			// 命令类型（固定为0ch）
	uint16_t len =   get_arg_ulong(argv[1]);              	  // DMA数据传输长度（波形点数）
	uint32_t clk =   get_arg_ulong(argv[2]);
	if(len > SPI_LEN) 
	{
		INFO("len error\n");
		return -1;
	}
	
  memset(SPI_DMA_Rx_buff,0,sizeof(SPI_DMA_Rx_buff));
	for(int i = 0;i<len;i++)
	{
		SPI_DMA_Tx_buff[i] = i;
	}
	
	HAL_DMA_SPI_Tx_Rx_Init(SPI_DMA_Tx_buff,SPI_DMA_Rx_buff,clk,len);   
	for(int i = 0;i<len;i++)
	{
			INFO("rxdata[%d] = %d  ",i,SPI_DMA_Rx_buff[i]);
	}	
	
	INFO("\r\n SPI Rx_Tx  DMA success !!\n");

    return 0;
}


int cmd_usedma_Reminder_Attention(int argc, char *argv[])
{
	INFO("\r\n Before using DMA, ensure that the CMD command of the related peripheral is enabled.\n ");
	return 0;
}


//==================================== SPI Slave ================================//
int cmd_dma_spi_slave_rx_test(int argc, char *argv[])
{	  
	// 参数个数校验
	if( argc != 2)
	{
			INFO("input error \n");
			return -1;
	}
	uint8_t case2 = get_arg_ulong(argv[0]);              		// 命令类型（固定为0ch）
	uint16_t len =   get_arg_ulong(argv[1]);              	  // DMA数据传输长度（波形点数）
 
	if(len > SPI_LEN) 
	{
		INFO("len error\n");
		return -1;
	}
	
  memset(SPI_DMA_Rx_buff,0,sizeof(SPI_DMA_Rx_buff));
	
	HAL_DMA_SPI_Slave_Rx_Init(SPI_DMA_Rx_buff,len);
 
	INFO("\r\n SPI Slave Rx  DMA success !!\n");
//	while(!dma_trans_complete);		
//	dma_trans_complete = 0;
//	for(int i = SPI_LEN - 10;i<SPI_LEN;i++)
//	{
//		INFO("SPI_DMA_Rx_buff[%d] = %d ",i,SPI_DMA_Rx_buff[i]);
//	}
	
	
    return 0;
}


int cmd_dma_spi_slave_tx_test(int argc, char *argv[])
{	  
	// 参数个数校验
	if( argc != 2)
	{
			INFO("input error \n");
			return -1;
	}
	uint8_t case2 = get_arg_ulong(argv[0]);              		// 命令类型（固定为0ch）
	uint16_t len =   get_arg_ulong(argv[1]);              	  // DMA数据传输长度（波形点数）
 
	if(len > SPI_LEN) 
	{
		INFO("len error\n");
		return -1;
	}
	memset(SPI_DMA_Tx_buff,0,sizeof(SPI_DMA_Tx_buff));
	SPI_DMA_Tx_buff[0] = 0xFF;
	for(int i = 1;i<SPI_LEN;i++)
	{
		SPI_DMA_Tx_buff[i] = i%200;
	}
	
	HAL_DMA_SPI_Slave_Tx_Init(SPI_DMA_Tx_buff,len);
 
	INFO("\r\n SPI Slave Tx  DMA success !!\n");
	
    return 0;
}



int cmd_dma_spi_slave_tx_rx_test(int argc, char *argv[])
{	  
	// 参数个数校验
	if( argc != 2)
	{
			INFO("input error \n");
			return -1;
	}
	uint8_t case2 = get_arg_ulong(argv[0]);              		// 命令类型（固定为0ch）
	uint16_t len =   get_arg_ulong(argv[1]);              	  // DMA数据传输长度（波形点数）
 
	if(len > SPI_LEN) 
	{
		INFO("len error\n");
		return -1;
	}
	memset(SPI_DMA_Tx_buff,0,sizeof(SPI_DMA_Tx_buff));
	memset(SPI_DMA_Rx_buff,0,sizeof(SPI_DMA_Rx_buff));
	SPI_DMA_Tx_buff[0] = 0xFF;
	for(int i = 1;i<SPI_LEN;i++)
	{
		SPI_DMA_Tx_buff[i] = i%200;
	}
	
	HAL_DMA_SPI_Slave_Rx_Tx_Init(SPI_DMA_Tx_buff,SPI_DMA_Rx_buff,len);
 
	INFO("\r\n SPI Slave Tx and Rx DMA success !!\n");
	
    return 0;
}


int cmd_dma_spi_slave_output_info_rxbuff(int argc, char *argv[])
{
	uint16_t len =   get_arg_ulong(argv[1]); 
	for(uint16_t i = 0;i<(len%5);i++)
	{
		INFO("SPI_DMA_Rx_buff[%d] = %d ",i,SPI_DMA_Rx_buff[i]);
	}
	INFO("\r\n");
	for(int i = len - 10;i<len;i++)
	{
		INFO("SPI_DMA_Rx_buff[%d] = %d ",i,SPI_DMA_Rx_buff[i]);
	}
	
	
	return 0;
}

//==================================== UART ================================//

int cmd_dma_uart_tx_test(int argc, char *argv[])
{	  
	// 参数个数校验
	if( argc != 3)
	{
			INFO("input error \n");
			return -1;
	}
	uint8_t case2 = get_arg_ulong(argv[0]);              			// 命令类型（固定为0ch）
	DMA_Channel_e ch =   (DMA_Channel_e)get_arg_ulong(argv[1]);              	  // DMA数据传输长度（波形点数）
	uint16_t len =   get_arg_ulong(argv[2]);
	if(len > UART_LEN) 
	{
		INFO("len error UART_LEN ->64\n");
		return -1;
	}
  for(int i = 0;i<len;i++)
	{
		uart_tx_buf[i] = i;
	}
	
	DMA_IT_UART_Config(ch,uart_rx_buf,uart_tx_buf,len,UART_TX,DMA_PRIORITY_MID,  DMA_MODE_NORMAL);
 
	
	INFO("\r\n UART  Tx DMA success !!\n");

    return 0;
}


int cmd_dma_uart_rx_test(int argc, char *argv[])
{	  
	// 参数个数校验
	if( argc != 3)
	{
			INFO("input error \n");
			return -1;
	}
	uint8_t case2 = get_arg_ulong(argv[0]);              			// 命令类型（固定为0ch）
	DMA_Channel_e ch =   (DMA_Channel_e)get_arg_ulong(argv[1]);              	  // DMA数据传输长度（波形点数）
	uint16_t len =   get_arg_ulong(argv[2]);
	if(len > UART_LEN) 
	{
		INFO("len error UART_LEN ->64\n");
		return -1;
	}
  memset(uart_rx_buf,0,sizeof(uart_rx_buf));
	uint8_t pass = 1;
	DMA_IT_UART_Config(ch,uart_rx_buf,0,len,UART_RX,DMA_PRIORITY_MID,DMA_MODE_CIRCULAR);
  INFO("\r\n UART  Rx DMA success !!\n");
	
	while(pass)
	{
		if(dma_trans_complete)
		{		
			for(int i =0;i<len;i++)
			{
				INFO("%c",uart_rx_buf[i]);
			}
			for(int i =0;i<len;i++)
			{
				INFO("\r\n uart_rx_buf[%d] = %d  ",i,uart_rx_buf[i]);
				if(i%10 == 0 )INFO("\n");
			}
			INFO("\r\n 【input uart_rx_buf[0] = 0xFF break】\r\n");
			
			if(uart_rx_buf[0] == 0xFF) 
			{
				pass = 0;
				INFO("break \r\n");    //发送必须关闭自动发送回车换行
			}
			memset(uart_rx_buf,0,sizeof(uart_rx_buf));
			dma_trans_complete = 0;
		}
	}

    return 0;
}

