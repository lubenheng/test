#include "I2C_hal.h"
#include "I2C_type.h"   
#include "I2C_reg.h"
#include "cmdline_internal.h"
#include "gpio_hal.h"
#include "Common.h" 	

/**
 * @file I2C_hal.c
 * @brief I2C 硬件抽象层（HAL）
 *
 * 提供 I2C 主/从初始化、读写接口以及最小的 MSP（平台）初始化。
 * 文件内含两个示例句柄（hi2c0、hi2c2）和对应的内部缓存。
 */

/* 内部缓冲区（供驱动在 CPU 抢占模式下读写硬件缓存使用） */
uint8_t g_i2c0_mem[16];
uint8_t g_i2c1_mem[64];


// I2C0 主机模式
HAL_I2C_HandleTypeDef hi2c0 = {
    .Instance = I2C0,
    .ClockSpeed = 1,
    .OwnAddress = 0x63,
    .MemSize = 32,
    .pMem = g_i2c0_mem,
};

// I2C2 从机模式
HAL_I2C_HandleTypeDef hi2c2 = {
    .Instance = I2C2,
    .ClockSpeed = 1,
    .OwnAddress = 0x03,
    .MemSize = 64,
    .pMem = g_i2c1_mem,
};


	
	
/* 使用顶部定义的内部缓冲区，避免重复定义 */
gpio_cfg_t gpio_cfg_strutct;

/**
 * @brief I2C 平台初始化（GPIO/上拉/时钟）
 * @param hi2c 指向 I2C 句柄（当前实现未使用）
 *
 * 配置 SCL/SDA 引脚复用、上拉以及打开必要的时钟门控。
 */
void HAL_I2C_MspInit(HAL_I2C_HandleTypeDef *hi2c)
{
 //SCLS  P02  A74
	//SDAs  P03  A75
	
	DIGCTRL->clk_en0 |= BIT8; // 
	DIGCTRL->clk_en0 |= BIT12; // 
	
//	IO_2810->EN.pad_scl_en = 1;
//	IO_2810->EN.pad_sda_en = 1;
	IO_2810->PU.Pad0_pu |= (1UL << 2); 
	IO_2810->PU.Pad0_pu |= (1UL << 3); 
	
	IO_2810->EN.pad_scls_en = 1;
	IO_2810->EN.pad_sdas_en = 1;
	INFO_e("I2C2  P02 03 success\r\n");
}


void HAL_I2C_Init(HAL_I2C_HandleTypeDef *hi2c)
{
    I2C_TypeDef *I2Cx = hi2c->Instance;

    HAL_I2C_MspInit(hi2c);

    /** 软复位：清理内部状态机，确保从已知状态开始 */
    I2Cx->I2C_Wo.Data_soft_rst = 1;

    // 去毛刺：防止总线噪声导致毛刺触发（会影响最大速率）
    I2Cx->I2C_En.deglitch_en = 1;  // 启用去毛刺
    I2Cx->I2C_En.Deglitch_cnt = 3; // 去毛刺计数（手册定义）

    // 从机地址：仅在从机模式有效，使用 7-bit 地址
    I2Cx->I2C_En.chip_id = hi2c->OwnAddress & 0x7F;

    // 时钟分频计算：严格按照寄存器文档公式
		// 公式：Mst_clk_div = cpu_clk_MHz / 4 / 0.4(i2c clk) - 1
		// 24MHz 主频 → 24 /4 /0.4 -1 = 14
//		float clk =  (24UL / 4UL /  0.4) - 1UL;  
		float clk =  (24UL / 4UL /  0.4) - 1UL;  
		uint16_t clk_div =(uint16_t)clk;     //400k
		 
		// 限幅 10bit (0~1023)
		if (clk_div > 0x3FF) 	clk_div = 0x3FF; 
				

		I2Cx->I2C_mst.Mst_clk_div = clk_div;

    // 中断使能：默认关闭，用户可按需开启
//    I2Cx->I2C_En.Int_en = 1;
//		NVIC_EnableIRQ(i2c0_irq);
	
    // CPU 不抢占：默认让硬件状态机运行，CPU 不直接占用缓存
    I2Cx->I2C_En.Cpu_occupy = 0;

    hi2c->State = HAL_I2C_STATE_READY;
}


/**
 * @brief I2C 主机发送一段数据（一次性写入长度小于等于内部缓冲大小）
 *
 * @param hi2c I2C 句柄
 * @param dev_addr 目标设备 7-bit 地址
 * @param pData 待发送数据缓冲
 * @param len 要发送的字节数
 *
 * 该函数将数据写入硬件缓存，触发主机状态机发送，并等待完成。
 */
HAL_StatusTypeDef HAL_I2C_Master_Transmit(
                                           HAL_I2C_HandleTypeDef *hi2c, 
                                           uint8_t dev_addr, uint8_t *pData, uint16_t len)
{
    I2C_TypeDef *I2Cx = hi2c->Instance;
    hi2c->State = HAL_I2C_STATE_MASTER_TX;

    // 1. 先等硬件状态机完全空闲，同时清掉上一次的中断标志
    while (I2Cx->I2C_mst_ro.mst_state != 0);
    I2Cx->I2C_Wo.Int_clr = 1;  

    // 2. 进入主机模式，关闭从机
    I2Cx->I2C_En.Slv_en = 0;
    I2Cx->I2C_En.Mst_en = 1;

    

    // 4. 配置主机通信 
    I2Cx->I2C_mst.Mst_i2c_addr_num  = 1;    // 带内部地址
    I2Cx->I2C_mst.Mst_i2c_dat_num   = len;
    I2Cx->I2C_Mst_addr.w            = dev_addr ;
    I2Cx->I2C_mst.Mst_i2c_wr        = 1;

    // 5. 触发通信
    I2Cx->I2C_Wo.Mst_trigger_wo = 1;

		// 3. CPU 抢占，写数据到硬件缓存
    I2Cx->I2C_En.Cpu_occupy = 1;
    for (uint16_t i = 0; i < len ; i++)//&& i < hi2c->MemSize
    {
        I2Cx->I2C_memOpt.Cpu_addr = i;
        I2Cx->I2C_memOpt.Cpu_wr_dat = pData[i];
        I2Cx->I2C_memOpt.Cpu_wr = 1;
        I2Cx->I2C_memOpt.Cpu_wr = 0;
    }
    I2Cx->I2C_En.Cpu_occupy = 0;
		
    // 6. 等待通信完成，同时检查状态
    while (I2Cx->I2C_mst_ro.mst_state != 0);

    // 7. 检查是否收到NAK
    if (I2Cx->I2C_mst_ro.Mst_int_idx == 1)
    {
        hi2c->ErrorCode = HAL_I2C_ERROR_NAK;
        I2Cx->I2C_Wo.Int_clr = 1;
        hi2c->State = HAL_I2C_STATE_READY;
        INFO_e("NAK error\r\n");
        return HAL_ERROR;
    }

    // 8. 清中断，恢复状态
    I2Cx->I2C_Wo.Int_clr = 1;
    hi2c->State = HAL_I2C_STATE_READY;
    return HAL_OK;
}



/**
 * @brief I2C 主机发送长数据（分段发送）
 *
 * @param hi2c I2C 句柄
 * @param dev_addr 目标设备地址
 * @param pData 待发送数据指针
 * @param len 总发送字节数
 *
 * 当数据长度超过硬件缓存（hi2c->MemSize）时，函数将分片写入并逐段触发发送，
 * 直至全部数据发送完成或发生错误（NAK）。
 */
HAL_StatusTypeDef HAL_I2C_Master_Transmit_Long(HAL_I2C_HandleTypeDef *hi2c, uint8_t dev_addr, uint8_t *pData, uint16_t len)
{
    I2C_TypeDef *I2Cx = hi2c->Instance;
    uint16_t remaining_len = len;   // 剩余多少字节没发
    uint16_t current_pos = 0;       // 当前发送到第几个字节
    uint16_t send_len;              // 本次要发的长度（每次最多16）

    hi2c->State = HAL_I2C_STATE_MASTER_TX;
		// 4. 配置主机
		I2Cx->I2C_mst.Mst_i2c_addr_num  = 0;
		
		I2Cx->I2C_Mst_addr.w            = dev_addr << 1;
		I2Cx->I2C_mst.Mst_i2c_wr        = 1;
    // 循环分片发送，直到发完所有数据
    while (remaining_len > 0)
    {
        send_len = (remaining_len > hi2c->MemSize) ? hi2c->MemSize : remaining_len;

        // 1. 等空闲 + 清上次中断
        while (I2Cx->I2C_mst_ro.mst_state != 0);
        I2Cx->I2C_Wo.Int_clr = 1;

        // 2. 主机模式
        I2Cx->I2C_En.Slv_en = 0;
        I2Cx->I2C_En.Mst_en = 1;

        // 3. 写当前这一段数据到硬件缓存（从0地址开始写）
        I2Cx->I2C_En.Cpu_occupy = 1;
        for (uint16_t i = 0; i < send_len; i++)
        {
            I2Cx->I2C_memOpt.Cpu_addr = i;
            I2Cx->I2C_memOpt.Cpu_wr_dat = pData[current_pos + i];
            I2Cx->I2C_memOpt.Cpu_wr = 1;
            I2Cx->I2C_memOpt.Cpu_wr = 0;  //必须要
        }
        I2Cx->I2C_En.Cpu_occupy = 0;

        I2Cx->I2C_mst.Mst_i2c_dat_num   = send_len;

        // 5. 触发
        I2Cx->I2C_Wo.Mst_trigger_wo = 1;   //执行一次只发送I2Cx->I2C_mst.Mst_i2c_dat_num 个字节

        // 6. 等待完成
        while (I2Cx->I2C_mst_ro.mst_state != 0);

        // 7. 检查 NAK
        if (I2Cx->I2C_mst_ro.Mst_int_idx == 1)
        {
            hi2c->ErrorCode = HAL_I2C_ERROR_NAK;
            I2Cx->I2C_Wo.Int_clr = 1;
            hi2c->State = HAL_I2C_STATE_READY;
            INFO_e("NAK error\r\n");
            return HAL_ERROR;
        }
        I2Cx->I2C_Wo.Int_clr = 1;

        
        current_pos += send_len;		//更新偏移
        remaining_len -= send_len;  //更新剩余
    }

    hi2c->State = HAL_I2C_STATE_READY;
    return HAL_OK;
}


 





/**
 * @brief I2C 主机读取从机数据
 *
 * @param hi2c I2C 句柄
 * @param dev_addr 目标设备地址
 * @param pData 接收缓冲
 * @param len 要读取的字节数（函数会根据 hi2c->MemSize 限制长度）
 * @return HAL_StatusTypeDef 成功返回 HAL_OK，失败返回 HAL_ERROR
 *
 * 说明：函数先根据设备协议可能进行写命令（例如写寄存器地址），
 * 然后触发读流程并在完成后将接收数据拷贝到 pData。
 */
HAL_StatusTypeDef HAL_I2C_Master_Receive(HAL_I2C_HandleTypeDef *hi2c, uint8_t dev_addr, uint8_t *pData, uint16_t len)
{
    I2C_TypeDef *I2Cx = hi2c->Instance;
    uint16_t i;

    // 限制最大接收 16 字节
    if (len > hi2c->MemSize)
    {
        len = hi2c->MemSize;
    }
		//超过32字节 读取会覆盖第一个数据
 
    hi2c->State = HAL_I2C_STATE_MASTER_RX;
	   
		
    // 2. 主机模式
    I2Cx->I2C_En.Slv_en = 0;
    I2Cx->I2C_En.Mst_en = 1;
		
    // 3. 配置读操作
    I2Cx->I2C_mst.Mst_i2c_wr        = 1;        // 写
    I2Cx->I2C_mst.Mst_i2c_addr_num = 0;        // 无内部地址
    I2Cx->I2C_mst.Mst_i2c_dat_num  = len;      // 要读的长度
    I2Cx->I2C_Mst_addr.w           = dev_addr ;   
		// 4. 触发读取
    I2Cx->I2C_Wo.Mst_trigger_wo = 1;   //开始  写从机地址  写从机寄存器地址  === 重新开始==
    
		//====================================================================
		// 3. 配置读操作
    I2Cx->I2C_mst.Mst_i2c_wr        = 0;        // 0 = 读操作
    I2Cx->I2C_mst.Mst_i2c_addr_num = 1;        // 带地址
    I2Cx->I2C_mst.Mst_i2c_dat_num  = len;      // 要读的长度
    I2Cx->I2C_Mst_addr.w           = dev_addr ;
		I2Cx->I2C_Wo.Mst_trigger_wo = 1;  //重新开始  发送从机地址 +读
		
		
		
    // 6. 检查是否出错
    if (I2Cx->I2C_mst_ro.Mst_int_idx == 1)
    {
        I2Cx->I2C_Wo.Int_clr = 1;
        hi2c->State = HAL_I2C_STATE_READY;
        INFO_e("RX NAK error\r\n");
        return HAL_ERROR;
    }
		
    // 5. 等待接收完成
    while (I2Cx->I2C_mst_ro.mst_state != 0);
		
    I2Cx->I2C_En.Cpu_occupy = 1;
    for (i = 0; i < len; i++)
    {
        I2Cx->I2C_memOpt.Cpu_addr = i;  // 从 0 地址开始读
        pData[i] = I2Cx->I2C_Cpu_rd_dat.Cpu_rd_dat;
    }
    I2Cx->I2C_En.Cpu_occupy = 0;
 	
		// 7. 清中断，结束
    I2Cx->I2C_Wo.Int_clr = 1;
    hi2c->State = HAL_I2C_STATE_READY;
    return HAL_OK;
}



 
/*
START → 主机发【从机读地址】→ 从机ACK →
从机发数据1 → 主机ACK →
从机发数据2 → 主机ACK →
...
从机发数据16 → 主机NACK → STOP
*/

/**
 * @brief I2C 从机被动发送数据（主机发读请求时响应）
 * @param hi2c I2C 句柄
 * @param pData 指向要发送的 32 字节数据缓冲（硬件寄存器范围）
 *
 * 将数据写入从机发送缓存，等待主机读取触发并完成发送。
 */
void HAL_I2C_Slave_Transmit(HAL_I2C_HandleTypeDef *hi2c, uint8_t *pData)
{
		I2C_TypeDef *I2Cx = hi2c->Instance;
    uint16_t i;

    I2Cx->I2C_En.Mst_en = 0;
    I2Cx->I2C_En.Slv_en = 1;

	    // 中断使能：默认关闭，用户可按需开启
//    I2Cx->I2C_En.Int_en = 1;
//		NVIC_EnableIRQ(i2c0_irq);

	
    I2Cx->I2C_mst.Mst_i2c_wr        = 1;        
    I2Cx->I2C_mst.Mst_i2c_addr_num = 1;         
  
    // 把要发送的数据写入从机发送缓存
		//slv_mem_add   范围0-1F，，，，手册写该寄存器8位
    I2Cx->I2C_En.Cpu_occupy = 1;
    for (i = 0; i < 32; i++)
    {
        I2Cx->I2C_memOpt.Cpu_addr = i;    				//主机读的是这个寄存器地址   cpu_addr 6位寄存器
        I2Cx->I2C_memOpt.Cpu_wr_dat = pData[i];   //数据反复写入0-1F寄存器
        I2Cx->I2C_memOpt.Cpu_wr = 1;
        I2Cx->I2C_memOpt.Cpu_wr = 0;
    }
    I2Cx->I2C_En.Cpu_occupy = 0;
		INFO_e("I2C0 Slave waiting...\r\n");
    while ( I2Cx->I2C_Wo.Int_clr == 0);
 
    // 4. 等待发送完成
    while (I2Cx->I2C_slv_ro.Slv_state != 0);

    // 清中断
    I2Cx->I2C_Wo.Int_clr = 1;
}



//主机写入  从机mcu读取
/*
START → 主机发【从机写地址】→ 从机自动 ACK →
主机发【寄存器地址】→ 从机自动 ACK →
主机发【数据1】→ 从机自动 ACK →
主机发【数据2】→ 从机自动 ACK →
...
主机发 STOP → 从机结束接收

*/
/**
 * @brief I2C 从机接收主机写入的数据
 * @param hi2c I2C 句柄
 * @param pData 接收缓冲，函数会把硬件缓存数据拷贝到 pData
 * @param len 输出实际接收字节数
 *
 * 该函数在检测到主机完成写操作后，将接收长度和数据读取出来。
 */
void HAL_I2C_Slave_Receive(HAL_I2C_HandleTypeDef *hi2c, uint8_t *pData,uint16_t *len)
{
		I2C_TypeDef *I2Cx = hi2c->Instance;
    uint16_t i;
		uint16_t rxlen = 0;
	

    I2Cx->I2C_En.Mst_en = 0;
    I2Cx->I2C_En.Slv_en = 1;

    I2Cx->I2C_mst.Mst_i2c_wr        = 0;        
    I2Cx->I2C_mst.Mst_i2c_addr_num = 0;         
  
    while ( I2Cx->I2C_Wo.Int_clr == 0);   	//等待主机操作起中断标志
		rxlen = I2Cx->I2C_slv_ro.slv_byt_len;		//读取主机操作的数据长度
		*len = rxlen;
		I2Cx->I2C_En.Cpu_occupy = 1;
    for (i = 0; i < rxlen; i++)
    {
        I2Cx->I2C_memOpt.Cpu_addr = i;  // 主机和这里必须一样 
        pData[i] = I2Cx->I2C_Cpu_rd_dat.Cpu_rd_dat;
    }
    I2Cx->I2C_En.Cpu_occupy = 0;
		
    while ( I2Cx->I2C_Wo.Int_clr == 0);
 
    
    while (I2Cx->I2C_slv_ro.Slv_state != 0);

    // 清中断
    I2Cx->I2C_Wo.Int_clr = 1;
}




/**
 * @brief I2C0 中断处理（示例）
 *
 * 当前仅打印占位信息，真实项目中应根据中断源解析状态并清中断位。
 */
void i2c0_Handler(void)
{
	if(  I2C0->I2C_Wo.Int_clr==1)
	{
		I2C0->I2C_Wo.Int_clr=1;
	  INFO_e("i2c0--interrupt0--trigger \r\n");	
		
	
	}
}
 
/**
 * @brief I2C1 中断处理（示例）
 */
void i2c1_Handler(void)
{
  INFO_e("error interrupt1 \r\n");
}


