/**
 * @file SPI_hal.c
 * @brief SPI 硬件抽象层实现
 *
 * 本文件提供 SPI 外设的 GPIO 初始化、主机/从机初始化、
 * 发送/接收函数以及 UART 引脚复用修改函数。
 * 这些函数为上层命令行测试和 SPI 通信提供底层支持。
 */
#include "SPI_hal.h"
#include "SPI_type.h"   

#include "cmdline_internal.h"
#include "gpio_hal.h"


	 
/**
 * @brief 初始化 SPI 相关 GPIO 引脚和 SPI 外设时钟
 *
 * @note 使用 P0.5 作为 NSS，P0.6 作为 SCLK，P0.7 作为 MISO，P1.0 作为 MOSI。
 *       同时关闭 P0.0 上的模拟功能，打开 SPI 时钟门控。
 */
void SPI_GPIO_Init(void)
{	
		IO_2810->EN2.pad_nss_en = 1;
		IO_2810->EN2.pad_sclk_en = 1;
		IO_2810->EN2.pad_mosi_en = 1;
		IO_2810->EN2.pad_miso_en = 1;
	
		IO_2810 ->ANA.w &= ~(1 << 8);
		DIGCTRL->clk_en0 |= (1 << 11U); //打开spi时钟
	  
		
		gpio_cfg_t 	GPIO_Init_Struct;
	
		GPIO_Init_Struct.alt_func	        =	GPIO_ALT_FUNC_0;
		GPIO_Init_Struct.analog_mode      =	GPIO_ANALOG_DISABLE;
		GPIO_Init_Struct.dir              =	GPIO_DIR_OUTPUT;
		GPIO_Init_Struct.pin              =	GPIO_PIN_0;
		GPIO_Init_Struct.port             =	GPIO_PORT_P1;
		GPIO_Init_Struct.pull             =	GPIO_PULL_UP;
		GPIO_Init_Struct.strength         =	GPIO_STRENGTH_LOW;
	  gpio_init(&GPIO_Init_Struct);	
	
}

spi_reg_t *const volatile SPI = (spi_reg_t *)SPI_BASE;

/**
 * @brief 初始化 SPI 主机模式
 *
 * @param baudrate SPI 波特率，单位 Hz
 * @param cpol 时钟极性（0 或 1）
 * @param cpha 时钟相位（0 或 1）
 *
 * @note 计算公式为 baudrate = SYSCLK/2/(clk_div + 1)，
 *       最小 clk_div 取 1，以满足硬件要求。
 */
void SPI_Master_Init(uint32_t baudrate,uint32_t cpol,uint32_t cpha)
{
		uint32_t div;
	// ======================== SPI 寄存器 主机发送模式 ========================
    SPI->cr1.bits.spien = 0;  // 先关闭SPI

    // 主机模式 + SPI模式0 (CPOL=0, CPHA=0) 
    SPI->cr0.bits.spimst = 1;
    SPI->cr0.bits.cpol   = cpol;
    SPI->cr0.bits.cpha   = cpha;

    // 波特率 = 48M / 2 / (4+1) = 2.4MH 		spi关闭可写>=1
		/*波特率 = SYSCLK/2/(SPI_CLK + 1) 
		例:波特率 4800kHz，则SPI_CLK = (24M/2/2400k) - 1 = 4,即为0x04 */
    div = (24000000 / 2 / baudrate) - 1;

    // 硬件要求：最小必须 >=1
    if(div < 1) div = 1;

    SPI->spi_clk.bits.clk_div = div;

    // 关闭所有中断（单向发送不需要）
    SPI->cr1.bits.int_detail_en = 0xf;
    SPI->cr1.bits.int_en       = 15;

    // 使能 SPI
    SPI->cr1.bits.spien = 1;
		
		//tx_empty
}

/**
 * @brief 初始化 SPI 主机中断模式
 *
 * @param baudrate SPI 波特率，单位 Hz
 * @param cpol 时钟极性（0 或 1）
 * @param cpha 时钟相位（0 或 1）
 *
 * @note 计算公式为 baudrate = SYSCLK/2/(clk_div + 1)，
 *       最小 clk_div 取 1，以满足硬件要求。
 */
void SPI_Master_interrupt_Init(uint32_t baudrate,uint32_t cpol,uint32_t cpha)
{
		uint32_t div;
	// ======================== SPI 寄存器 主机发送模式 ========================
    SPI->cr1.bits.spien = 0;  // 先关闭SPI

    // 主机模式 + SPI模式0 (CPOL=0, CPHA=0) 
    SPI->cr0.bits.spimst = 1;
    SPI->cr0.bits.cpol   = cpol;
    SPI->cr0.bits.cpha   = cpha;

    // 波特率 = 48M / 2 / (4+1) = 2.4MH 		spi关闭可写>=1
		/*波特率 = SYSCLK/2/(SPI_CLK + 1) 
		例:波特率 4800kHz，则SPI_CLK = (24M/2/2400k) - 1 = 4,即为0x04 */
    div = (24000000 / 2 / baudrate) - 1;

    // 硬件要求：最小必须 >=1
    if(div < 1) div = 1;

    SPI->spi_clk.bits.clk_div = div;

    // 打开中断（单向发送不需要）
		
		SPI->cr1.bits.int_detail_en = 0x8;
    SPI->cr1.bits.int_en       = 8;
		__enable_irq();   // 必须加！Cortex-M 内核总中断
		NVIC_EnableIRQ(spi_irq);

    // 使能 SPI
    SPI->cr1.bits.spien = 1;
		
		//tx_empty
}

/**
 * @brief SPI 主机发送数据
 *
 * @param buf 发送数据缓存指针
 * @param len 要发送的字节数
 *
 * @note 该函数将片选拉低，逐字节写入 SPI 数据寄存器，
 *       等待发送完成后清除 SPIIF 标志，最后将片选拉高。
 */
void SPI_SendData(uint8_t *buf, uint16_t len)
{
    // 1. CS 拉低（选中从机）
    SPI->cr1.bits.nssmod = 3;

    // 2. 逐字节发送
    for (uint8_t i = 0; i < len; i++)
    {
        while (!SPI->cr1.bits.tx_empty);
        SPI->spi_dr.bits.data = buf[i];
        while (SPI->cr1.bits.spiif == 0);

        SPI->cr1.bits.spiif = 0;
    }

    // 3. CS 拉高（释放从机）
    SPI->cr1.bits.nssmod = 3;
}

/**
 * @brief SPI 主机读取从机返回数据
 *
 * @param buf 接收数据缓存指针
 * @param len 要读取的字节数
 *
 * @note 主机通过发送 0xFF 生成时钟，从 SPI 数据寄存器读取返回值。
 */
void SPI_Master_ReadData(uint8_t *buf, uint16_t len)
{
    // 1. 拉低 CS，开始通信
    SPI->cr1.bits.nssmod = 2;

    for (uint16_t i = 0; i < len; i++)
    {
        // 等待发送缓冲器空
        while (!SPI->cr1.bits.tx_empty);

        // 主机发 0xFF，获取时钟
        SPI->spi_dr.bits.data = 0xFF;

        // 等待传输完成
        while (SPI->cr0.bits.spibsy == 1);

        // 读取数据
        buf[i] = SPI->spi_dr.bits.data;
    }

    // 2. 拉高 CS，结束接收
    SPI->cr1.bits.nssmod = 3;
}


/**
 * @brief 初始化 SPI 从机模式
 *
 * @param cpol 时钟极性（与主机保持一致）
 * @param cpha 时钟相位（与主机保持一致）
 *
 * @note 从机模式不需要设置波特率，时钟由主机提供；
 *       通过 nssmod 选择 4 线从机模式。
 */
void SPI_Slave_Init(uint32_t cpol,uint32_t cpha)
{
    SPI->cr1.bits.spien = 0;  // 关闭 SPI

    // ===================== 从机模式配置 =====================
    SPI->cr0.bits.spimst = 0;    // 0 = 从机
    SPI->cr0.bits.cpol   = cpol;    // 和主机一致：模式0
    SPI->cr0.bits.cpha   = cpha;

    // ===================== 从机 NSS 必须配置 =====================
    // 从机只能用：01（4线从机）或 00（3线从机）
    SPI->cr1.bits.nssmod = 1;  // 01 = 4线从机模式（NSS 输入）

    // 从机不需要配置时钟！时钟由主机提供
    // SPI->spi_clk.bits.clk_div = 1;  // 从机无效，删掉

    // ===================== 中断配置（从机必须开）=====================
    SPI->cr1.bits.int_detail_en = 0x8;  // 打开所有从机中断
    SPI->cr1.bits.int_en       = 1;     // 打开总中断
		
		//SPI->cr1.bits.nssmod=0;
	
    // 使能SPI
    SPI->cr1.bits.spien = 1;
		__enable_irq();   // 必须加！Cortex-M 内核总中断
		NVIC_EnableIRQ(spi_irq);
}

/**
 * @brief SPI 从机发送数据
 *
 * @param buf 发送数据缓存指针
 * @param len 发送字节数
 *
 * @note 从机发送数据时，主机提供时钟，当前函数仅向 DR 写入数据即可。
 */
void SPI_Slave_SendData(uint8_t *buf, uint16_t len)
{
	for(int i = 0; i < len; i++)
	{
    // 1. 等待发送缓冲器空
    while (!SPI->cr1.bits.tx_empty);

    // 2. 把数据写入 DR，主机时钟到来时自动发送
    SPI->spi_dr.bits.data = buf[i];
	}
}

/**
 * @brief SPI 从机读取接收数据
 *
 * @return 读取到的数据字节
 *
 * @note 该函数直接从 DR 寄存器读取数据，用于从机收到主机发送的数据。
 */
uint8_t SPI_Slave_ReadData(void)
{
    return SPI->spi_dr.bits.data;
}

/**
 * @brief 修改 UART0 引脚复用为 SPI 复用方案
 *
 * @note 本函数将 P0.2/P0.3 配置为 UART0 TX/RX 输出模式，
 *       并打开 UART0 以及相关时钟。适用于需要在 SPI 模式下重用 UART0 引脚的场景。
 */
void SPI_Uart_modified(void)
{
	gpio_cfg_t 	GPIO_Init_Struct;
	
	GPIO_Init_Struct.alt_func	        =	GPIO_ALT_FUNC_0;
	GPIO_Init_Struct.analog_mode      =	GPIO_ANALOG_DISABLE;
	GPIO_Init_Struct.dir              =	GPIO_DIR_OUTPUT;
	GPIO_Init_Struct.pin              =	GPIO_PIN_2;
	GPIO_Init_Struct.port             =	GPIO_PORT_P0;
	GPIO_Init_Struct.pull             =	GPIO_PULL_DISABLE;
	GPIO_Init_Struct.strength         =	GPIO_STRENGTH_LOW;
	gpio_init(&GPIO_Init_Struct);
	GPIO_Init_Struct.alt_func	        =	GPIO_ALT_FUNC_0;
	GPIO_Init_Struct.analog_mode      =	GPIO_ANALOG_DISABLE;
	GPIO_Init_Struct.dir              =	GPIO_DIR_OUTPUT;
	GPIO_Init_Struct.pin              =	GPIO_PIN_3;
	GPIO_Init_Struct.port             =	GPIO_PORT_P0;
	GPIO_Init_Struct.pull             =	GPIO_PULL_DISABLE;
	GPIO_Init_Struct.strength         =	GPIO_STRENGTH_LOW;
	gpio_init(&GPIO_Init_Struct);
	
	
	uint32_t tmp;
	DIGCTRL->clk_en0 |= 0x00002000U; // 1：PLL时钟(48MHz)
	// 开启时钟门控
	DIGCTRL->clk_en0 |= (1 << 11U); //打开spi时钟
	DIGCTRL->clk_en0 |= 0x00040000U;
	// 配置uart设备
	tmp = UART0->thr;
	tmp &= ~(0xff << 0);
	tmp |= 11 | 1 << 4;
	UART0->thr = tmp;
 
	UART0->baud = (uint32_t)((float)208 + 0.5);
	tmp = UART0->en;
	tmp |= 1;
	UART0->en |= tmp;

//	IO_2810->EN.pad_txd2s_en = 1;
//	IO_2810->EN.pad_rxd2s_en = 1;

//	UART0->en &=~0x02;
//	UART0->en &=~0x80;    
	
	//Rx
	uint32_t      tmp1;  
 
	tmp1 = UART0->thr;
	tmp1 &= ~(0xff<<8);
	tmp1 |= (11 | 1<<4)<<8;
	UART0->thr   = tmp1;

	tmp1 = UART0->en;
	tmp1 &= ~(0x3f<<8);
	tmp1 |= 0x2c<<8 | 1<<6 	;
	UART0->en = tmp1;
	
	IO_2810->EN.pad_txds_en = 1;
	IO_2810->EN.pad_rxds_en = 1;
}

