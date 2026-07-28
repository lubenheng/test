#include "spi_cmdline.h"
#include "utility.h"
#include "softinit.h"
#include "SPI_hal.h"
#include "string.h"

/**
 * @file spi_cmdline.c
 * @brief SPI 命令行测试接口
 *
 * 本文件定义了 SPI 主机发送、读取、从机发送以及 SPI 中断处理函数，
 * 用于命令行模式下对 SPI 通信进行简单验证与调试。
 *
 * 功能总结：
 *  - cmd_spi_test: SPI 主机发送测试
 *  - cmd_spi_read: SPI 主机读取测试
 *  - printf_rxbuff: 打印 SPI 接收缓存内容
 *  - spi_slave_test: SPI 从机发送测试
 *  - spi_Handler: SPI 中断服务函数
 */

#define SPI_LEN 	100

/** SPI 发送缓存，供 cmd_spi_test 和 spi_slave_test 使用 */
uint8_t SPI_Txbuff[SPI_LEN];
/** SPI 接收缓存，供 cmd_spi_read 和打印函数使用 */
uint8_t SPI_Rxbuff[SPI_LEN];

/**
 * @brief SPI 主机发送测试命令
 *
 * @param argc 命令参数数量
 * @param argv 命令参数数组，期望传入如下参数：
 *  - argv[0] : case2，占位参数，当前函数未使用
 *  - argv[1] : 发送长度 len（字节）
 *  - argv[2] : 波特率 baudrate（Hz）
 *  - argv[3] : CPOL，时钟极性
 *  - argv[4] : CPHA，时钟相位
 *
 * @return 0 成功，-1 输入参数错误
 *
 * @note 本函数期望传入至少 5 个参数，以避免访问非法 argv。
 */
int cmd_spi_test(int argc, char *argv[])
{
	uint8_t case2  = get_arg_ulong(argv[0]);
	uint8_t len    = get_arg_ulong(argv[1]);
	uint32_t baurd = get_arg_ulong(argv[2]);  // SPI 波特率
	uint32_t cpol   = get_arg_ulong(argv[3]);  // 时钟极性
	uint32_t cpha   = get_arg_ulong(argv[4]);  // 时钟相位
	if(argc != 5) 
	{
		INFO_e("input error\n");
		return -1;
	}
	for(int i = 0; i < SPI_LEN; i++)
	{
		SPI_Txbuff[i] = i;
	}
	/* 初始化 SPI 引脚配置 */
	SPI_GPIO_Init();
	/* 初始化 SPI 主机模式，设置波特率、时钟极性、时钟相位 */
	SPI_Master_Init(baurd, cpol, cpha);
		
	/* 发送数据缓存中的前 len 字节 */
	SPI_SendData(SPI_Txbuff, len);
	INFO_e("SPI Master tx OK\r\n");
 	
	return 0;
}

 
/**
 * @brief SPI 主机读取测试命令
 *
 * @param argc 命令参数数量
 * @param argv 命令参数数组，期望传入如下参数：
 *  - argv[0] : case2，占位参数，当前函数未使用
 *  - argv[1] : 读取长度 len（字节）
 *  - argv[2] : 波特率 baudrate（Hz）
 *  - argv[3] : CPOL，时钟极性
 *  - argv[4] : CPHA，时钟相位
 *
 * @return 0 成功，-1 输入参数错误
 *
 * @note 本函数期望传入至少 5 个参数，先清空接收缓存，再初始化 SPI GPIO 和主机模式，
 *       最后读取 len 字节数据到 SPI_Rxbuff。
 */
int cmd_spi_read(int argc, char *argv[])
{
	uint8_t case2 = get_arg_ulong(argv[0]);
	uint8_t len = get_arg_ulong(argv[1]);
	uint32_t baurd = get_arg_ulong(argv[2]); 
	uint32_t cpol   = get_arg_ulong(argv[3]);  
	uint32_t cpha   = get_arg_ulong(argv[4]); 
	
	if(argc != 5) 
	{
		INFO_e("input error\n");
		return -1;
	} 
	memset(SPI_Rxbuff, 0, sizeof(SPI_Rxbuff));
	/* 初始化 SPI GPIO 引脚 */
	SPI_GPIO_Init();
	/* 初始化 SPI 主机并设置时钟参数 */
  SPI_Master_Init(baurd, cpol, cpha);
	
  /* 读取 len 字节数据到接收缓存 */
  SPI_Master_ReadData(SPI_Rxbuff, len);
	INFO_e("SPI Master rx OK\r\n");
 	
	return 0;	
}

/**
 * @brief 打印 SPI 接收缓存内容
 *
 * @param argc 命令参数数量
 * @param argv 命令参数数组，argv[1] 为要打印的字节数 len
 *
 * @return 0 成功
 *
 * @note 本函数按顺序输出 SPI_Rxbuff 中前 len 个字节，
 *       每 5 个字节换行显示，便于调试查看读取结果。
 */
int printf_rxbuff(int argc, char *argv[])
{
	uint8_t len = get_arg_ulong(argv[1]);
	for(int i = 0; i < len; i++)
	{
		INFO_e("SPI_Rxbuff[%d] = %d  ", i, SPI_Rxbuff[i]);
		if (i % 5 == 0) INFO_e("\r\n");
	}
 	return 0;
}
 
/**
 * @brief SPI 从机发送测试命令
 *
 * @param argc 命令参数数量
 * @param argv 命令参数数组，期望传入如下参数：
 *  - argv[0] : case2，占位参数，当前函数未使用
 *  - argv[1] : 发送长度 len（字节）
 *  - argv[2] : CPOL，时钟极性
 *  - argv[3] : CPHA，时钟相位
 *
 * @return 0 成功
 *
 * @note 该函数初始化 SPI 从机模式，并从 SPI_Txbuff 中读取前 len 字节发送出去。
 */
int spi_slave_test(int argc, char *argv[])
{
	uint8_t len = get_arg_ulong(argv[1]);
	uint32_t cpol   = get_arg_ulong(argv[2]);  
	uint32_t cpha   = get_arg_ulong(argv[3]); 
	SPI_GPIO_Init();
	SPI_Slave_Init(cpol,cpha);
	for(int i = 0; i < SPI_LEN; i++)
	{
		SPI_Txbuff[i] = i;
	}
	SPI_Slave_SendData(SPI_Txbuff, len);
	return 0;
}

/**
 * @brief SPI 中断服务函数
 *
 * @note 当 SPI 控制寄存器中的 spiif 标志位被置位时，表示 SPI 事务完成或产生中断。
 *       本函数检查该标志位并清除之。
 */
void spi_Handler()
{
	if (SPI->cr1.bits.spiif == 1)
	{
		INFO_e("SPI_\n");
		/* 清除 SPI 中断完成标志 */
		SPI->cr1.bits.spiif = 0;
	}
}

