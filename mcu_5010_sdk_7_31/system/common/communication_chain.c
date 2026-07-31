/*******************************************************************************
 * Include Files
 ******************************************************************************/
#include "platform_common.h"

#include "device_resource.h"

#if FEATURE_HAS_DMA_ENABLE
#include "dma_hal.h"
#include "dma_reg.h"
#endif

#include "uart_hal.h"
#include "uart_hal_cfg.h"
#include "spi_hal.h"
#include "spi_hal_cfg.h"
#include "list_internal.h"
#include "communication_chain.h"


static kfifo_t g_kfifo;


//the data output the fifo, the rd_idx will change
static void read_buffer(uint8_t *cmd_buffer, uint32_t cmd_total_len)
{
	DEV_ASSERT(cmd_buffer != NULL);
	DEV_ASSERT(cmd_total_len > 0U);
	
	for (uint32_t i = 0U; i < cmd_total_len; i++)
	{
		cmd_buffer[i] = g_kfifo.buffer[(g_kfifo.rd_index+i)%g_kfifo.buffer_size];
	}

	g_kfifo.rd_index = (g_kfifo.rd_index + cmd_total_len)%g_kfifo.buffer_size;
}

static uint32_t get_buffer_wd_idx(void)
{
	const DMA_TypeDef * DMA0_base = DMA0;
	uint32_t bcr;
	bcr = DMA0_base->REG[1U].DSR.BCSR;

	g_kfifo.wd_index = g_kfifo.buffer_size - bcr;
	return (g_kfifo.wd_index);
}

static uint32_t get_buffer_rd_idx(void)
{
	return (g_kfifo.rd_index);
}

#ifdef BASIC_ON_SPI
static void communication_chain_spi_init(uint8_t instance, uint8_t *buffer_rx, uint32_t buffer_szie)
{
	spi_slave_init_config.transfer_type = SPI_USING_DMA;
	spi_slave_init_config.frame_size = 8U;
	spi_slave_init_config.rx_dma_channel = 1U;
	spi_slave_init_config.rx_dma = 0U;
	/* configure spi register */
	spi_slave_init((spi_instance_e)instance, &spi_slave_init_config);
	/* begin to transfer data */
	spi_slave_transfer((spi_instance_e)instance, NULL, buffer_rx, buffer_szie);
}
#endif

#ifdef BASIC_ON_UART
static void communication_chain_uart_init(uint8_t instance, uint8_t *buffer_rx, uint32_t buffer_szie)
{
	uart1_init_config.transfer_type = UART_USING_DMA;
	uart1_init_config.baud_rate = 115200U;
	uart1_init_config.rx_dma_channel = 1U;
	uart1_init_config.rx_dma_instance = 0U;
	
	(void)uart_init((uart_instance_e)instance, &uart1_init_config);
	uart_interrupt_handle_cfg((uart_instance_e)instance, uart_irq_handler);
	//note that modify the uart driver code, do not enable the dma interrupt in uart driver code.
	(void)uart_receive_data((uart_instance_e)instance, buffer_rx, buffer_szie);
}
#endif

void communication_chain_device_init(uint8_t instance, uint8_t *buffer_rx, uint32_t buffer_size)
{
	g_kfifo.buffer = buffer_rx;
	g_kfifo.buffer_size = buffer_size;
	g_kfifo.rd_index = 0U;
	g_kfifo.wd_index = 0U;

#ifdef BASIC_ON_UART
	communication_chain_uart_init(instance, buffer_rx, buffer_size);
#endif

#ifdef BASIC_ON_SPI
	communication_chain_spi_init(instance, buffer_rx, buffer_size);
#endif

}

void communication_receive_random_length_command(uint8_t cmd_header_len, uint8_t *buffer_store, communication_cmd_handle_t cmd_handle, communication_cmd_header_handle_t cmd_header_handle)
{
	uint32_t cmd_data_len = 0U;
	uint32_t buffer_rd_idx = 0U;
	uint32_t buffer_wr_idx = 0U;
	uint32_t rd_to_wr_len = 0U;

	while (true)
	{
		buffer_wr_idx = get_buffer_wd_idx();
		buffer_rd_idx = get_buffer_rd_idx();

		if (buffer_wr_idx >= buffer_rd_idx)
		{
			rd_to_wr_len = buffer_wr_idx-buffer_rd_idx;
		}
		else
		{
			rd_to_wr_len = buffer_wr_idx + g_kfifo.buffer_size - buffer_rd_idx;
		}
		
		if (rd_to_wr_len >= (cmd_header_len+cmd_data_len))
		{
			if (cmd_data_len == 0U)
			{
				cmd_data_len = cmd_header_handle(g_kfifo.buffer, buffer_rd_idx);
			}
			else
			{
				read_buffer(buffer_store, cmd_header_len+cmd_data_len);
				cmd_handle(buffer_store, cmd_header_len+cmd_data_len);
				cmd_data_len = 0U;
			}
		}
	}
}

void communication_receive_fixed_length_command(uint8_t cmd_len, uint8_t *buffer_store, communication_cmd_handle_t cmd_handle)
{
	uint32_t buffer_rd_idx = 0U;
	uint32_t buffer_wr_idx = 0U;
	uint32_t rd_to_wr_len = 0U;

	while (true)
	{
		buffer_wr_idx = get_buffer_wd_idx();

		buffer_rd_idx = get_buffer_rd_idx();


		if (buffer_wr_idx >= buffer_rd_idx)
		{
			rd_to_wr_len = buffer_wr_idx-buffer_rd_idx;
		}
		else
		{
			rd_to_wr_len = buffer_wr_idx + g_kfifo.buffer_size - buffer_rd_idx;
		}
		
		if (rd_to_wr_len >= cmd_len)
		{
			read_buffer(buffer_store, cmd_len);
			cmd_handle(buffer_store, cmd_len);
		}
	}
}


