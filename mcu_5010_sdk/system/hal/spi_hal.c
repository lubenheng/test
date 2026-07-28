#include "platform_common.h"

#include "spi_reg.h"
#include "device_resource.h"
#include "spi_hal.h"
#include "spi.h"
#include "spi_type.h"

#if 1

/*******************************************************************************
 * Variables
 ******************************************************************************/
/** Table of base addresses for SPI instances. */
static SPI_TypeDef * const g_spi_bases[SPI_INSTANCE_MAX_COUNT] = SPI_BASE_ADDRS;
/** Tables to save SPI IRQ enum numbers defined in CMSIS header file. */
static const IRQn_Type g_spi_irqn[SPI_INSTANCE_MAX_COUNT] = SPI_IRQS;
/** Tables to save SPI runtime state structure. */
static spi_state_t g_spi_state[SPI_INSTANCE_MAX_COUNT];
/** Table to save SPI clock names */
static const module_name_e g_spi_clk_names[SPI_INSTANCE_MAX_COUNT] = SPI_CLOCK_NAMES;
/** Tables to save SPI interrupt handle. */
static SPI_INT_HANDLE g_spi_int_handle[SPI_INSTANCE_MAX_COUNT] = {NULL};
/** Defines constant value arrays for the baud rate pre-scalar values. */
static const uint32_t baudrate_pre_scaler[] = {1, 2, 4, 8, 16, 32, 64, 128};
/** Tables to save the pointer to SPI pcs negative function. */
static SPI_PCS_NEGATIVE g_spi_pcs_negative_function[SPI_INSTANCE_MAX_COUNT];
/** Tables to save the pointer to SPI pcs active function. */
static SPI_PCS_ACTIVE g_spi_pcs_active_function[SPI_INSTANCE_MAX_COUNT];

/* OCS_Reserved_Start */
bool trigger_enable = false;
bool host_enable = false;
/* OCS_Reserved_End */
 
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/** This function config pin select, and control the data width and direction of spi bus transfer. */ 
static void spi_pin_mode_set(spi_instance_e spi_instance, 
								spi_transfer_width_e spi_transfer_width, 
								spi_pin_config_e pin_cfg);
/** This function fill up the TX FIFO with data. */
static void spi_tx_buffer_write(spi_instance_e spi_instance);
/** This function read all data from RX FIFO. */
static void spi_rx_buffer_read(spi_instance_e spi_instance);
/** This function get base address pointer of SPI instance. */
static SPI_TypeDef *spi_instance_addr_get(spi_instance_e spi_instance);
/* OCS_Reserved_Start */
/** This function is the handle function of receive word complete interrupt. */
static void spi_receive_word_complete_interrupt_handle(spi_instance_e spi_instance);
/** This function is the handle function of receive frame complete interrupt. */
static void spi_receive_frame_complete_interrupt_handle(spi_instance_e spi_instance);
/* OCS_Reserved_End */
/** This function is the handle function of receive data match interrupt. */
static void spi_receive_data_match_interrupt_handle(spi_instance_e spi_instance);
/** This function set the SPI baud rate in bits per second. */
static uint32_t spi_baud_rate_set(spi_instance_e spi_instance,
									uint32_t bits_per_sec,
									uint32_t *pre_scale);
/** This function record some parameter into SPI runtime state for SPI master. */
static void spi_master_state_config(spi_instance_e spi_instance, spi_master_config_t *config);
/** This function configure the transmit command register for SPI master. */
static void spi_master_tcr_reg_config(spi_instance_e spi_instance, 
										spi_master_config_t *config, 
										uint32_t pre_scale);
/** This function get the absolute difference between two numbers. */
static uint32_t abs_dif(uint32_t a, uint32_t b);
/** This function update the SPI runtime state for master. */
static void spi_master_state_set(spi_instance_e spi_instance,
							const uint8_t *send_buffer,
							uint8_t *receive_buffer,
							uint16_t tx_count,
							uint16_t rx_count,
							uint16_t tx_valid_count,
							uint16_t rx_valid_count);
/** This function finish up a transfer. */
static void spi_master_transfer_complete(spi_instance_e spi_instance);
/** This function is interrupt handler function for SPI master. */
static void spi_master_irq_handler(spi_instance_e spi_instance);
/** This function record some parameter into SPI runtime state for SPI slave. */
static void spi_slave_state_config(spi_instance_e spi_instance, spi_slave_config_t *config);
/** This function configure the transmit command register for SPI slave. */
static void spi_slave_tcr_reg_config(spi_instance_e spi_instance, spi_slave_config_t *config);
/** This function update the runtime state for SPI slave. */
static void spi_slave_state_set(spi_instance_e spi_instance,
							const uint8_t *send_buffer,
							uint8_t *receive_buffer,
							uint16_t tx_count,
							uint16_t rx_count,
							uint16_t tx_valid_count,
							uint16_t rx_valid_count);
/** This function is interrupt handler function for SPI slave. */
static void spi_slave_irq_handler(spi_instance_e spi_instance);
/** This function is used to write the TX data. */
static void spi_fill_data_reg(spi_instance_e spi_instance);
/** This function is used to read the RX data. */
static void spi_read_data_reg(spi_instance_e spi_instance);


/*******************************************************************************
* Public SPI Control Functions
******************************************************************************/ 
/**
 *@brief return default configuration for SPI master
 *@param[out] *config: pointer to configuration structure which is filled with default configuration
 *@return     none
 */
void spi_master_default_config_get(spi_master_config_t *config)
{
	DEV_ASSERT(config != NULL);

	config->bits_per_sec = SPI_MASTER_DEFAULT_BITS_PER_SEC;
	config->which_pcs = SPI_PCS0;
	config->pcs_polarity = SPI_PCS_ACTIVE_LOW;
	config->is_pcs_continuous = false; 
    config->frame_size = SPI_DEFAULT_FRAME_SIZE;
	config->spi_sck_phase = SPI_SCK_PHASE_1ST_EDGE;
	config->spi_sck_polarity = SPI_SCK_ACTIVE_HIGH;
	config->lsb_first = true;
	config->call_back = NULL;
	config->call_back_param = NULL;
	config->spi_transfer_width = SPI_SINGLE_BIT;
	config->byte_swap = false;
	config->pin_cfg = SPI_SDI_IN_SDO_OUT;
	config->interrupt_priority = 0U;
}

/**
 *@brief initialize a SPI instance to work as master
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] *config: pointer to configuration structure
 *@return    whether init the SPI instance successfully or not:
 				- STATUS_SUCCESS, initialize the spi successfully
 				- STATUS_ERROR, initialize the spi fail
 */
status_t spi_master_init(spi_instance_e spi_instance, spi_master_config_t *config)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	DEV_ASSERT(config != NULL);

	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);
	status_t error_code;
	uint32_t idx;

	/* Clear the state struct for this instance. */
    uint8_t *clear_struct_ptr = (uint8_t *)spi_state;
    for (idx = 0U; idx < sizeof(spi_state); idx++)
    {
        clear_struct_ptr[idx] = 0U;
    }

	/* init all register to the default vaule */
	spi_drv_soft_reset(spi_ptr);
	/* Set SPI work mode, master or slave */
	(void)spi_drv_master_slave_mode_set(spi_ptr, SPI_MASTER);
	/* config pin select */
	spi_pin_mode_set(spi_instance, config->spi_transfer_width, config->pin_cfg);
	/* Configure watermarks */
	spi_drv_water_marks_set(spi_ptr, SPI_MASTER_DEFAULT_TX_WATER_MARK, SPI_MASTER_DEFAULT_RX_WATER_MARK);
	/* Configure doze mode */
	spi_drv_doze_enable(spi_ptr, false);
	/* Configure debug mode */
	spi_drv_debug_enable(spi_ptr, true);
/* OCS_Reserved_Start */
	if (trigger_enable == true)
	{
		spi_drv_host_mode_set(spi_ptr, true, SPI_HOST_ACTIVE_HIGH, SPI_HOST_FROM_TRIGGER);	
	}
	if (host_enable == true)
	{
		spi_drv_host_mode_set(spi_ptr, true, SPI_HOST_ACTIVE_HIGH, SPI_HOST_FROM_HREQ);	
	}
/* OCS_Reserved_End */
	/* Configure bus for this device.*/ 
	error_code = spi_master_configure_bus(spi_instance, config, NULL);
	if (error_code != STATUS_SUCCESS)
    {
        return error_code;
    }
	
	/* configure interrupt service routine */
	if (0U == NVIC_GetEnableIRQ(g_spi_irqn[(uint8_t)spi_instance]))
	{
		NVIC_EnableIRQ(g_spi_irqn[(uint8_t)spi_instance]);
		NVIC_SetPriority(g_spi_irqn[(uint8_t)spi_instance], config->interrupt_priority);
	}

	/* Finally, enable SPI */
    spi_drv_enable(spi_ptr);

	return error_code;
}

/**
 *@brief shut down a SPI master
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@return    whether shut down the SPI instance successfully or not:
 				- STATUS_SUCCESS, shut down the spi successfully
 */
status_t spi_master_deinit(spi_instance_e spi_instance)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);
	status_t error_code = STATUS_SUCCESS;
	IRQn_Type IRQn;
	uint32_t idx;

	/* Check if a transfer is still in progress */
    DEV_ASSERT(spi_state->is_transfer_in_progress == false);
	
	/* Reset the SPI registers to their default state, inlcuding disabling the SPI */
	spi_drv_soft_reset(spi_ptr);
	/* Disable the interrupt */
	IRQn = g_spi_irqn[(uint8_t)spi_instance];
	NVIC_DisableIRQ(IRQn);

	/* Clear the state struct for this instance. */
    uint8_t *clear_struct_ptr = (uint8_t *)spi_state;
    for (idx = 0U; idx < sizeof(spi_state); idx++)
    {
        clear_struct_ptr[idx] = 0U;
    }

	return error_code;
}

/**
 *@brief configure the SPI master mode bus timing delay options
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] pcs_to_sck: PCS to SCK delay
 *@param[in] sck_to_pcs: SCK to PCS delay
 *@param[in] pcs_to_cmd: PCS to CMD delay
 *@param[in] sck_to_sck: SCK to SCK delay
 *@param[in] sck_stp: SCK setup phase delay
 *@param[in] sck_hld: SCK hold phase delay
 *@return    whether configure the SPI master mode bus timing delay successfully or not:
 				- STATUS_SUCCESS, configure the SPI master mode bus timing delay successfully
 				- STATUS_ERROR, SPI is busy, configure the SPI master mode bus timing delay fail
 */
status_t spi_master_delay_set(spi_instance_e spi_instance, 
								uint8_t pcs_to_sck, 
								uint8_t sck_to_pcs, 
								uint8_t pcs_to_cmd, 
								uint8_t sck_to_sck, 
								uint8_t sck_stp,
								uint8_t sck_hld)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	status_t error_code = STATUS_SUCCESS;

	/* disable the SPI module before setting the delay */
	if (spi_drv_disable(spi_ptr) != STATUS_SUCCESS)
	{
		/* if error is returned, the SPI is busy */
        return STATUS_ERROR;
	}

	spi_drv_timing_set(spi_ptr, pcs_to_sck, sck_to_pcs, pcs_to_cmd, sck_to_sck, sck_stp, sck_hld);
	
	/* Enable module */
    spi_drv_enable(spi_ptr);

	return error_code;
}

/**
 *@brief configure the SPI port physical parameters
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] *config: pointer to configuration structure
 *@param[in] *calculated_baudrate: the calculated baud rate passed back to the user
 *@return    whether configure the SPI port physical parameters successfully or not:
 				- STATUS_SUCCESS, configure the SPI port physical parameters successfully
 				- STATUS_ERROR, configure the SPI port physical parameters fail
 */
status_t spi_master_configure_bus(spi_instance_e spi_instance, 
									spi_master_config_t *config, 
									uint32_t *calculated_baudrate)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	DEV_ASSERT(config != NULL);

	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	uint32_t baudrate;
	uint32_t pre_scale;

	/* disable the SPI module before setting the delay */
	if (spi_drv_disable(spi_ptr) != STATUS_SUCCESS)
	{
		/* if error is returned, the SPI is busy */
        return STATUS_ERROR;
	}

	/* Check the bitcount to make sure it falls within the boundary conditions */
	if ((config->frame_size < SPI_FRAME_SIZE_MIN) || (config->frame_size > SPI_FRAME_SIZE_MAX))
	{
		return STATUS_ERROR;
	}

	/* configure internal state structure for SPI */
	spi_master_state_config(spi_instance, config);
	/* configure the desired PCS polarity */
	(void)spi_drv_pcs_polarity_set(spi_ptr, config->which_pcs, config->pcs_polarity);	
	/* set up the baud rate */
	baudrate = spi_baud_rate_set(spi_instance, config->bits_per_sec, &pre_scale);
	/* Enable sampling point delay */
	spi_drv_sample_point_set(spi_ptr, true);
	/* re-enable the LPSPI module */
	spi_drv_enable(spi_ptr);

	/* If the baud rate return is "0", it means there was an error */
    if (baudrate == 0UL)
    {
        return STATUS_ERROR;
    }
	/* If the user wishes to know the calculated baud rate, then pass it back */
    if (calculated_baudrate != NULL)
    {
        *calculated_baudrate = baudrate;
    }

	spi_master_tcr_reg_config(spi_instance, config, pre_scale);

	return STATUS_SUCCESS;
}

/**
 *@brief select the chip to communicate with
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] which_pcs: select which PCS to program
 *@param[in] pcs_polarity: set PCS as active high or low
 *@return    whether select the chip successfully or not:
 				- STATUS_SUCCESS, select the chip successfully
 				- STATUS_ERROR, the SPI is busy, and select the chip fail
 */
status_t spi_pcs_set(spi_instance_e spi_instance, 
						spi_pcs_e which_pcs, 
						spi_pcs_polarity_e pcs_polarity)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	DEV_ASSERT(which_pcs < SPI_PCS_MAX);
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);

	/* disable the SPI module before setting the pcs */
	if (spi_drv_disable(spi_ptr) != STATUS_SUCCESS)
	{
		/* if error is returned, the SPI is busy */
        return STATUS_ERROR;
	}
	/* configure the desired PCS polarity */
	(void)spi_drv_pcs_polarity_set(spi_ptr, which_pcs, pcs_polarity);
	/* Enable module */
    spi_drv_enable(spi_ptr);
	spi_drv_pcs_set(spi_ptr, which_pcs);
	
	return STATUS_SUCCESS;
}

/**
 *@brief perform a SPI master mode transfer
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] *send_buffer: the pointer to the data buffer of the data to send
 *@param[in] *receive_buffer: the pointer to the data buffer of the data to receive
 *@param[in] transfer_byte_count: the number of bytes to send and receive
 *@return    whether transfer data successfully or not:
 				- STATUS_SUCCESS, transfer data successfully
 				- STATUS_BUSY, can not perform transfer because a transfer is already in progress
 				- STATUS_ERROR, transfer data fail
 */
status_t spi_master_transfer(spi_instance_e spi_instance,
								const uint8_t *send_buffer,
								uint8_t *receive_buffer,
								uint16_t transfer_byte_count)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);
	
	/* If the transfer count is zero, then return immediately.*/
    if (transfer_byte_count == (uint16_t)0U)
    {
        return STATUS_SUCCESS;
    }

	/* Check that the spi is not busy. */
    if (spi_drv_status_flag_get(spi_ptr, SPI_MODULE_BUSY))
    {
        return STATUS_BUSY;
    }

	/* Verify if the number of bytes is divided by number of bytes/frame. */
    if ((transfer_byte_count % spi_state->bytes_per_frame) != (uint16_t)0U)
    {
        return STATUS_ERROR;
    }

	/* Clean RX and TX buffers */
    spi_drv_flush_fifo_cmd_set(spi_ptr, true, true);
    /* The second flush command is used to avoid the case when one word is still in shifter. */
    spi_drv_flush_fifo_cmd_set(spi_ptr, true, true);

	if(spi_state->is_pcs_continuous == true)
	{
		spi_drv_cont_cmd_bit_set(spi_ptr, true);
	}
	/* Clear flag of all interrupts sources */
	(void)spi_drv_status_flag_clear(spi_ptr, SPI_ALL_STATUS);
	/* Enable fault interrupts sources */
	spi_drv_interrupt_source_enable(spi_ptr, SPI_TRANSMIT_ERROR, true);
	if (receive_buffer != NULL)
    {
        spi_drv_interrupt_source_enable(spi_ptr, SPI_RECEIVE_ERROR, true);
    }
	
	/* update the spi state for SPI master */
	spi_master_state_set(spi_instance, send_buffer, receive_buffer, transfer_byte_count, transfer_byte_count, transfer_byte_count, transfer_byte_count);

	/* Enable RX interrupt if RX buffer is not NULL. */
		if (spi_state->rx_buff != NULL)
		{
				spi_drv_interrupt_source_enable(spi_ptr, SPI_RX_DATA_FLAG, true);
		}
		/* Enable the TX interrupt. */
		spi_drv_interrupt_source_enable(spi_ptr, SPI_TX_DATA_FLAG, true);
	

	/* Update transfer status */
	spi_state->is_transfer_in_progress = true;
	spi_state->status = SPI_TRANSFER_OK;
	
	return STATUS_SUCCESS;
}
/**
 *@brief perform a SPI master mode transfer and use the normal gpio as pcs pin
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] *send_buffer: the pointer to the data buffer of the data to send
 *@param[in] *receive_buffer: the pointer to the data buffer of the data to receive
 *@param[in] transfer_byte_count: the number of bytes to send and receive
 *@param[in] *pcs_negative_function: the pointer to the function which make the pcs gpio become negative
 *@param[in] *pcs_active_function: the pointer to the function which make the pcs gpio become active
 *@return    whether transfer data successfully or not:
 				- STATUS_SUCCESS, transfer data successfully
 				- STATUS_BUSY, can not perform transfer because a transfer is already in progress
 				- STATUS_ERROR, transfer data fail
 */							
status_t spi_master_transfer_manual_pcs(spi_instance_e spi_instance,
								const uint8_t *send_buffer,
								uint8_t *receive_buffer,
								uint16_t transfer_byte_count,
								SPI_PCS_NEGATIVE pcs_negative_function, 
								SPI_PCS_ACTIVE pcs_active_function)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);
	
	/* If the transfer count is zero, then return immediately.*/
	if (transfer_byte_count == (uint16_t)0U)
	{
		return STATUS_SUCCESS;
	}

	/* Check that the spi is not busy. */
	if (spi_drv_status_flag_get(spi_ptr, SPI_MODULE_BUSY))
	{
		return STATUS_BUSY;
	}

	/* Verify if the number of bytes is divided by number of bytes/frame. */
	if ((transfer_byte_count % spi_state->bytes_per_frame) != (uint16_t)0U)
	{
		return STATUS_ERROR;
	}

	/* Clean RX and TX buffers */
	spi_drv_flush_fifo_cmd_set(spi_ptr, true, true);
	/* The second flush command is used to avoid the case when one word is still in shifter. */
	spi_drv_flush_fifo_cmd_set(spi_ptr, true, true);

	if(spi_state->is_pcs_continuous == true)
	{
		spi_drv_cont_cmd_bit_set(spi_ptr, true);
	}
	/* Clear flag of all interrupts sources */
	(void)spi_drv_status_flag_clear(spi_ptr, SPI_ALL_STATUS);

	/* update the spi state for SPI master */
	spi_master_state_set(spi_instance, send_buffer, receive_buffer, transfer_byte_count, transfer_byte_count, transfer_byte_count, transfer_byte_count);

	/* store the pointer of pcs negative and active function */
	g_spi_pcs_negative_function[(uint8_t)spi_instance] = pcs_negative_function;
	g_spi_pcs_active_function[(uint8_t)spi_instance] = pcs_active_function;

	/* negative the pcs gpio before negative the pcs*/
	g_spi_pcs_negative_function[(uint8_t)spi_instance]();
	
	/* enable the spi frame complete interrupt */
	spi_drv_interrupt_source_enable(spi_ptr, SPI_FRAME_COMPLETE, true);

	/* active the pcs gpio */
	g_spi_pcs_active_function[(uint8_t)spi_instance]();
	/* fill the tx data to begin the transfer */
	spi_fill_data_reg(spi_instance);
	
	/* Update transfer status */
	spi_state->is_transfer_in_progress = true;
	spi_state->status = SPI_TRANSFER_OK;
	
	return STATUS_SUCCESS;
}


/**
 *@brief perform a SPI master mode transfer, and allow the size of receive/transimit data 
 *       to be not equal to the total size of data in this transfer activity.
 *@param[in] spi_instance, spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] *send_buffer, the buffer pointer which saves the data to be sent
 *@param[in] *receive_buffer, the buffer pointer which saves the data to be received
 *@param[in] transfer_byte_count, the total size of data in this transfer activity
 *@param[in] tx_valid_count, the size of data that spi really want to send
 *@param[in] rx_valid_count, the size of data that spi really want to receive
 *@return    error code of function implemented status:
  				- STATUS_SUCCESS, transfer data successfully
 				- STATUS_BUSY, can not perform transfer because a transfer is already in progress
 				- STATUS_ERROR, transfer data fail
 */
status_t spi_master_transfer_data_ex(spi_instance_e spi_instance,
							const uint8_t *send_buffer,
							uint8_t *receive_buffer,
							uint16_t transfer_byte_count,
							uint16_t tx_valid_count,
							uint16_t rx_valid_count)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	DEV_ASSERT(send_buffer != NULL);
	DEV_ASSERT(receive_buffer != NULL);
	DEV_ASSERT(tx_valid_count != (uint16_t)0U);
	DEV_ASSERT(rx_valid_count != (uint16_t)0U);
	DEV_ASSERT((tx_valid_count + rx_valid_count) == transfer_byte_count);
	
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);

	DEV_ASSERT((tx_valid_count % spi_state->bytes_per_frame) == (uint16_t)0U);
	DEV_ASSERT((rx_valid_count % spi_state->bytes_per_frame) == (uint16_t)0U);
	
	/* If the transfer count is zero, then return immediately.*/
    if (transfer_byte_count == (uint16_t)0U)
    {
        return STATUS_SUCCESS;
    }

	/* Check that the spi is not busy. */
    if (spi_drv_status_flag_get(spi_ptr, SPI_MODULE_BUSY))
    {
        return STATUS_BUSY;
    }

	/* Verify if the number of bytes is divided by number of bytes/frame. */
    if ((transfer_byte_count % spi_state->bytes_per_frame) != (uint16_t)0U)
    {
        return STATUS_ERROR;
    }

	/* Clean RX and TX buffers */
    spi_drv_flush_fifo_cmd_set(spi_ptr, true, true);
    /* The second flush command is used to avoid the case when one word is still in shifter. */
    spi_drv_flush_fifo_cmd_set(spi_ptr, true, true);

	if(spi_state->is_pcs_continuous == true)
	{
		spi_drv_cont_cmd_bit_set(spi_ptr, true);
	}
	/* Clear flag of all interrupts sources */
	(void)spi_drv_status_flag_clear(spi_ptr, SPI_ALL_STATUS);
	/* Enable default interrupts sources */
	spi_drv_interrupt_source_enable(spi_ptr, SPI_TRANSMIT_ERROR, true);
	spi_drv_interrupt_source_enable(spi_ptr, SPI_RECEIVE_ERROR, true);
  
	/* update the spi state for SPI master */
	spi_master_state_set(spi_instance, send_buffer, receive_buffer, transfer_byte_count, transfer_byte_count, tx_valid_count, rx_valid_count);

	/* Enable RX interrupt. */
	spi_drv_interrupt_source_enable(spi_ptr, SPI_RX_DATA_FLAG, true);
    /* Enable the TX interrupt. */
    spi_drv_interrupt_source_enable(spi_ptr, SPI_TX_DATA_FLAG, true);
	
	return STATUS_SUCCESS; 
}

/**
 *@brief return whether the previous interrupt driven transfer is completed
 *@param[in]  spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[out] *bytes_remained: pointer to a value that is filled in with the number of bytes that must be received
 *@return     whether the previous interrupt driven transfer is completed or not:
 				- STATUS_SUCCESS, the transfer has completed successfully
 				- STATUS_BUSY, the transfer is still in progress
 */
status_t spi_master_transfer_status_get(spi_instance_e spi_instance, uint32_t *bytes_remained)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);
	
	/* Fill in the bytes transferred.*/
    if (bytes_remained != NULL)
    {
        *bytes_remained = spi_state->rx_count;
    }

	if (spi_state->status == SPI_TRANSFER_OK)
    {
        return (status_t)(spi_state->is_transfer_in_progress ? STATUS_BUSY : STATUS_SUCCESS);
    }
    else
    {
        return STATUS_ERROR;
    }
}

/**
 *@brief terminate a SPI master transfer
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@return    whether terminate the SPI transfer successfully or not:
 				- STATUS_SUCCESS, terminate the SPI transfer successfully
 */
status_t spi_master_transfer_abort(spi_instance_e spi_instance)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);

	/* Transfer finish. */
	spi_master_transfer_complete(spi_instance);
	/* Flush the fifo. */
	spi_drv_flush_fifo_cmd_set(spi_ptr, true, true);
	/* The second flush command is used to avoid the case when one word is still in shifter. */
	spi_drv_flush_fifo_cmd_set(spi_ptr, true, true);
	
	return STATUS_SUCCESS;
}

/**
 *@brief return default configuration for SPI slave
 *@param[out] *config: pointer to configuration structure which is filled with default configuration
 *@return     none
 */
void spi_slave_default_config_get(spi_slave_config_t *config)
{
	DEV_ASSERT(config != NULL);

	config->which_pcs = SPI_PCS0;
	config->pcs_polarity = SPI_PCS_ACTIVE_LOW;
	config->is_pcs_continuous = false; 
    config->frame_size = SPI_DEFAULT_FRAME_SIZE;
	config->spi_sck_phase = SPI_SCK_PHASE_1ST_EDGE;
	config->spi_sck_polarity = SPI_SCK_ACTIVE_HIGH;
	config->lsb_first = true;
	config->call_back = NULL;
	config->call_back_param = NULL;
	config->spi_transfer_width = SPI_SINGLE_BIT;
	config->byte_swap = false;
	config->pin_cfg = SPI_SDI_IN_SDO_OUT;
	config->interrupt_priority = 0U;
}

/**
 *@brief initialize a SPI instance to work as slave
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] *config: pointer to configuration structure.
 *@return    whether init the SPI instance successfully or not:
 				- STATUS_SUCCESS, initialize the spi successfully
 				- STATUS_ERROR, initialize the spi fail
 */
status_t spi_slave_init(spi_instance_e spi_instance, spi_slave_config_t *config)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	DEV_ASSERT(config != NULL);

	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);
	status_t error_code = STATUS_SUCCESS;
	IRQn_Type IRQn;
	uint32_t idx;

	/* Clear the state struct for this instance. */
    uint8_t *clear_struct_ptr = (uint8_t *)spi_state;
    for (idx = 0U; idx < sizeof(spi_state); idx++)
    {
        clear_struct_ptr[idx] = 0U;
    }

	/* init all register to the default vaule */
	spi_drv_soft_reset(spi_ptr);
	/* Set SPI work mode, master or slave */
	(void)spi_drv_master_slave_mode_set(spi_ptr, SPI_SLAVE);
	/* config pin select */
	spi_pin_mode_set(spi_instance, config->spi_transfer_width, config->pin_cfg);
	/* Configure watermarks */
	spi_drv_water_marks_set(spi_ptr, SPI_SLAVE_DEFAULT_TX_WATER_MARK, SPI_SLAVE_DEFAULT_RX_WATER_MARK);
	/* Configure doze mode */
	spi_drv_doze_enable(spi_ptr, false);
	/* Configure debug mode */
	spi_drv_debug_enable(spi_ptr, true);
	/* configure the desired PCS polarity */
	(void)spi_drv_pcs_polarity_set(spi_ptr, config->which_pcs, config->pcs_polarity);
	/* configure internal state structure for SPI */
	spi_slave_state_config(spi_instance, config);
	/* configure tcr register */
	spi_slave_tcr_reg_config(spi_instance, config);
	/* Enable module */
	spi_drv_enable(spi_ptr);

	/* configure interrupt service routine */
	IRQn = g_spi_irqn[(uint8_t)spi_instance];
	if (0U == NVIC_GetEnableIRQ(IRQn))
	{
		NVIC_EnableIRQ(IRQn);
		NVIC_SetPriority(IRQn, config->interrupt_priority);
	}

	return error_code;
}

/**
 *@brief shut down a SPI slave
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@return    whether shut down the SPI slave successfully or not:
 				- STATUS_SUCCESS, shut down the SPI successfully
 */
status_t spi_slave_deinit(spi_instance_e spi_instance)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);
	status_t error_code = STATUS_SUCCESS;
	IRQn_Type IRQn;
	uint32_t idx;

	/* Check if a transfer is still in progress */
    DEV_ASSERT(spi_state->is_transfer_in_progress == false);

	/* Reset the SPI registers to their default state, inlcuding disabling the SPI */
	spi_drv_soft_reset(spi_ptr);
	/* Disable the interrupt */
	IRQn = g_spi_irqn[(uint8_t)spi_instance];
	NVIC_DisableIRQ(IRQn);

	/* Clear the state struct for this instance. */
    uint8_t *clear_struct_ptr = (uint8_t *)spi_state;
    for (idx = 0U; idx < sizeof(spi_state); idx++)
    {
        clear_struct_ptr[idx] = 0U;
    }

	return error_code;
}

/**
 *@brief perform a SPI slave mode transfer
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] *send_buffer: the pointer to the data buffer of the data to send
 *@param[in] *receive_buffer: the pointer to the data buffer of the data to receive
 *@param[in] transfer_byte_count: the number of bytes to send and receive
 *@return    whether transfer data successfully or not:
 				- STATUS_SUCCESS, transfer data successfully
 				- STATUS_BUSY, can not perform transfer because a transfer is already in progress
 				- STATUS_ERROR, transfer data fail
 */
status_t spi_slave_transfer(spi_instance_e spi_instance,
								const uint8_t *send_buffer,
								uint8_t *receive_buffer,
								uint16_t transfer_byte_count)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	DEV_ASSERT(!((send_buffer == NULL) && (receive_buffer == NULL)));

	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);

	/* If the transfer count is zero, then return immediately.*/
    if (transfer_byte_count == (uint16_t)0U)
    {
        return STATUS_SUCCESS;
    }

	/* Check if SPI module isn't busy */
    if (spi_state->is_transfer_in_progress == true)
    {
        return STATUS_BUSY;
    }

	/* Verify if the number of bytes is divided by number of bytes/frame. */
    if ((transfer_byte_count % spi_state->bytes_per_frame) != (uint16_t)0U)
    {
        return STATUS_ERROR;
    }

	/* Clean RX and TX buffers */
    spi_drv_flush_fifo_cmd_set(spi_ptr, true, true);
    /* The second flush command is used to avoid the case when one word is still in shifter. */
    spi_drv_flush_fifo_cmd_set(spi_ptr, true, true);
	/* Clear flag of all interrupts sources */
	(void)spi_drv_status_flag_clear(spi_ptr, SPI_ALL_STATUS);
	/* Enable fault interrupts sources */
	spi_drv_interrupt_source_enable(spi_ptr, SPI_TRANSMIT_ERROR, true);
	spi_drv_interrupt_source_enable(spi_ptr, SPI_RECEIVE_ERROR, true);

	spi_slave_state_set(spi_instance, send_buffer, receive_buffer, transfer_byte_count, transfer_byte_count, transfer_byte_count, transfer_byte_count);

	/* Enable interrupts for RX and TX only if it's necessary */
			if (spi_state->tx_buff != NULL)
			{
		spi_drv_interrupt_source_enable(spi_ptr, SPI_TX_DATA_FLAG, true);
			}
			if (spi_state->rx_buff != NULL)
			{
					spi_drv_interrupt_source_enable(spi_ptr, SPI_RX_DATA_FLAG, true);
			}
	
	/* Update transfer status */
	spi_state->is_transfer_in_progress = true;
	spi_state->status = SPI_TRANSFER_OK;
	
	return STATUS_SUCCESS;
}

/**
 *@brief return whether the previous interrupt driven transfer is completed
 *@param[in]  spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[out] *bytes_remained: pointer to a value that is filled in with the number of bytes that must be received
 *@return     whether the previous interrupt driven transfer is completed or not:
 				- STATUS_SUCCESS, the transfer has completed successfully
 				- STATUS_BUSY, the transfer is still in progress
 */
status_t spi_slave_transfer_status_get(spi_instance_e spi_instance, uint32_t *bytes_remained)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);

	/* Fill in the bytes transferred.*/
    if (bytes_remained != NULL)
    {
        *bytes_remained = spi_state->tx_count;
    }

	if (spi_state->status == SPI_TRANSFER_OK)
    {
        return (status_t)(spi_state->is_transfer_in_progress ? STATUS_BUSY : STATUS_SUCCESS);
    }
    else
    {
        return STATUS_ERROR;
    }
}

/**
 *@brief terminates a SPI slave transfer
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@return    whether terminate the SPI transfer successfully or not:
 				- STATUS_SUCCESS, terminate the SPI transfer successfully
 */
static status_t spi_slave_transfer_abort(spi_instance_e spi_instance)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);

	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);

	/* Disable (clear) interrupt requests */
	spi_drv_interrupt_source_enable(spi_ptr, SPI_RX_DATA_FLAG, false);
	spi_drv_interrupt_source_enable(spi_ptr, SPI_TX_DATA_FLAG, false);
	
	/* Disable (clear) all interrupt requests */
	spi_drv_all_interrupts_source_disable(spi_ptr);
	/* Clears all SPI status flag */
	(void)spi_drv_status_flag_clear(spi_ptr, SPI_ALL_STATUS);
	/*wait clearing flag completed*/
	__DSB();

	spi_state->is_transfer_in_progress = false;
	spi_drv_flush_fifo_cmd_set(spi_ptr, true, true);
	/* The second flush command is used to avoid the case when one word is still in shifter. */
	spi_drv_flush_fifo_cmd_set(spi_ptr, true, true);

	return STATUS_SUCCESS;	
}

/**
 *@brief wait the last transfer to complete
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] wait_time: wait time, unit:ms 
 *@return    whether the last transfer is completed in specified time or not
 				- fasle, the last transfer has not been completed in specified time
 				- true, the last transfer has been completed in specified time
 */
bool spi_transfer_wait(spi_instance_e spi_instance, uint32_t wait_time)
{
#if 0
	
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	DEV_ASSERT(wait_time > 0U);
	
	bool wait_ret;
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);

	wait_ret = wait_till_timeout((uint32_t *)&(spi_state->is_transfer_in_progress), 1U, 0U, wait_time);
		
	return wait_ret;
	
#endif //old version reserved
	
	
	return 0;
}

/**
 *@brief set or clear the spi interrupt
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] interrupt_src: the spi interrupt source
 *@param[in] enable:
 *				- true, set a certain interrupt
 *				- false, clear a certain interrupt
 *@return    none
 */
void spi_interrupt_source_enable(spi_instance_e spi_instance, 
									spi_status_flag_e interrupt_src, 
									bool enable)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	
	spi_drv_interrupt_source_enable(spi_ptr, interrupt_src, enable);
}

/**
 *@brief configure the spi work in host mode
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] host_en:
 *				- true, configure the SPI work in host mode
 *				- false, do not configure the SPI work in host mode
 *@param[in] host_pol: host request polarity
 *@param[in] host_sel: host request selection
 *@return    none
 */
void spi_host_mode_set(spi_instance_e spi_instance, 
						bool host_en, 
						spi_host_polarity_e host_pol, 
						spi_host_selection_e host_sel)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	
	spi_drv_host_mode_set(spi_ptr, host_en, host_pol, host_sel);
}

/**
 *@brief configure spi match type
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] match_type:
 *				- SPI_MATCH_DISABLE, disabled match function
 *				- SPI_FIST_WORD_MATCH0_OR_MATCH1, the first received word equals to MATCH0 OR MATCH1
 *				- SPI_ANY_WORD_MATCH0_OR_MATCH1, any received word equals to MATCH0 OR MATCH1
 *				- SPI_FIST_MATCH0_AND_SECOND_MATCH1, the first received word equals to MATCH0 and the second received word equals to MATCH1
 *				- SPI_ANY_MATCH0_AND_NEXT_MATCH1, any received word equals to MATCH0 and the next received word equals to MATCH1
 *				- SPI_FIST_AND_MATCH1_EQUAL_MATCH0_AND_MATCH1, the result of the first received word AND MATCH1 is equal to the result of MATCH0 AND MATCH1
 *				- SPI_ANY_AND_MATCH1_EQUAL_MATCH0_AND_MATCH1, any received word AND MATCH1 is equal to the result of MATCH0 ANDMATCH1.
 *@param[in] match0: match word 0
 *@param[in] match1: match word 1
 *@param[in] match_only: whether set the match only function.
 *				- true, set the match only function
 *				- false, do not set the match only function
 *@return    none
 */	
void spi_match_type_set(spi_instance_e spi_instance, 
						spi_match_fun_e match_type, 
						uint32_t match0, 
						uint32_t match1, 
						bool match_only)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);

	spi_drv_match_type_set(spi_ptr, match_type, match0, match1, match_only);
}

/**
 *@brief configure the SPI work in stall mode
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] stall_en:
 *				- true, configure the SPI work in stall mode
 *				- false, do not configure the SPI work in stall mode
 *@return    none
 */
void spi_stall_mode_set(spi_instance_e spi_instance, func_switch_e stall_en)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);

	spi_drv_stall_mode_set(spi_ptr, ((FUNC_ENABLE == stall_en) ? true : false));
}

/**
 *@brief configure the SPI work in remainder mode
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] remainder_en:
 *				- true, configure the SPI work in remainder mode
 *				- false, do not configure the SPI work in remainder mode
 *@return    none
 */
void spi_remainder_mode_set(spi_instance_e spi_instance, bool remainder_en)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);

	spi_drv_remainder_mode_set(spi_ptr, remainder_en);
}

/**
 *@brief configure the SPI work in loopback mode
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] loopback_en:
 *				- true, configure the SPI work in loopback mode
 *				- false, do not configure the SPI work in loopback mode
 *@return    none
 */
void spi_loopback_mode_set(spi_instance_e spi_instance, bool loopback_en)
{
	DEV_ASSERT(spi_instance < SPI_INSTANCE_MAX_COUNT);
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);

	spi_drv_loopback_mode_set(spi_ptr, loopback_en);
}

/**
 *@brief decide whether the SPI slave will generate chip select signal automatically
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] auto_pcs_en:
 *				- true, SPI slave can generate chip select signal automatically
 *				- false, SPI slave can not generate chip select signal automatically
 *@return    none
 */
void spi_auto_pcs_enable(spi_instance_e spi_instance, bool auto_pcs_en)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);

	spi_drv_auto_pcs_enable(spi_ptr, auto_pcs_en);
}

/**
 *@brief enable circular FIFO to make spi send sevaral bytes data repeatedly
 *       if you want use this function to send data repeatedly, do not set rx_buff to be NULL
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] cir_fifo_en:
 *				- true, enable the circular FIFO
 *				- fasle, disable the circular FIFO
 *@param[in] cir_bytes: the size of data that you want to send repeatedly
 *@return    none
 */
void spi_circule_fifo_enable(spi_instance_e spi_instance, func_switch_e cir_fifo_en, uint16_t cir_bytes)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);
	DEV_ASSERT(cir_bytes > 0U);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);

	DEV_ASSERT(spi_state->bytes_per_frame != 0U);
	DEV_ASSERT((cir_bytes % spi_state->bytes_per_frame) == 0U);
	DEV_ASSERT((cir_bytes / spi_state->bytes_per_frame) < 4U);

	spi_state->circule_fifo_en = ((FUNC_ENABLE == cir_fifo_en) ? true : false);
	spi_state->circule_byte = cir_bytes;
}

/**
 *@brief install callback function for the spi transmit
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] callback: the callback function. 
 *@param[in] callback_param: the pointer of callback parameter
 *@return    none
 */
void spi_callback_install(spi_instance_e spi_instance, spi_callback_t callback, void *callback_param)        		            
{
    DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);

	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);
    spi_state->call_back = callback;
    spi_state->call_back_param = callback_param;
}



/**
 *@brief provide the driver with a new buffer for continuous transmission
 *@param[in] spi_instance: SPI instance number
 *@param[in] *send_buffer: the pointer to the data buffer of the data to send
 *@param[in] *receive_buffer: the pointer to the data buffer of the data to receive
 *@param[in] transfer_byte_count: the number of bytes to send and receive
 *@return    none
 */
void spi_slave_buffer_set(spi_instance_e spi_instance,
                                const uint8_t *send_buff,
                                uint8_t *receive_buffer,
                                uint16_t transfer_byte_count)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);

	spi_slave_state_set(spi_instance, send_buff, receive_buffer, transfer_byte_count, transfer_byte_count, transfer_byte_count, transfer_byte_count);
}

/**
 *@brief provide the driver with a new buffer for continuous transmission
 *@param[in] spi_instance: SPI instance number
 *@param[in] *send_buffer: the pointer to the data buffer of the data to send
 *@param[in] *receive_buffer: the pointer to the data buffer of the data to receive
 *@param[in] transfer_byte_count: the number of bytes to send and receive
 *@return	 none
 */
void spi_master_buffer_set(spi_instance_e spi_instance,
								const uint8_t *send_buff,
								uint8_t *receive_buffer,
								uint16_t transfer_byte_count)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);

	spi_master_state_set(spi_instance, send_buff, receive_buffer, transfer_byte_count, transfer_byte_count, transfer_byte_count, transfer_byte_count);
}


/*******************************************************************************
* Private SPI Control Functions
******************************************************************************/
/**
 *@brief config pin select, and control the data width and direction of spi bus transfer. 
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] spi_transfer_width:
 *				- SPI_SINGLE_BIT, 1-bit shift at a time, data out on SDO, in on SDI (normal mode)
 *				- SPI_TWO_BIT, 2-bits shift out on SDO/SDI and in on SDO/SDI
 *				- SPI_FOUR_BIT, 4-bits shift out on SDO/SDI/PCS[3:2] and in on SDO/SDI/PCS[3:2]
 *@param[in] pin_cfg:
 *				- SPI_SDI_IN_SDO_OUT, SPI SDI input, SDO output
 *				- SPI_SDI_IN_OUT, SDI is used for both input and output data
 *				- SPI_SDO_IN_OUT, SDO is used for both input and output data
 *				- SPI_SDI_OUT_SDO_IN, SPI SDO input, SDI output
 *@return    none
 */
static void spi_pin_mode_set(spi_instance_e spi_instance, 
								spi_transfer_width_e spi_transfer_width, 
								spi_pin_config_e pin_cfg)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	uint8_t pin_mode;

	if (spi_transfer_width == SPI_FOUR_BIT)
	{
		pin_mode = SPI_FOUR_BIT_PIN_CFG;
		(void)spi_drv_pin_mode_cfg(spi_ptr, pin_mode);/* For 4-bit parallel transfer*/
	}
	else if (spi_transfer_width == SPI_TWO_BIT)
	{
		pin_mode = SPI_TWO_BIT_PIN_CFG;	
		(void)spi_drv_pin_mode_cfg(spi_ptr, pin_mode);/* For 2-bit parallel transfer*/		
	}
	else
	{
		pin_mode = (uint8_t)pin_cfg;
    	(void)spi_drv_pin_mode_cfg(spi_ptr, pin_mode);			
	}
}

/**
 *@brief get base address pointer of spi instance
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@return    base address pointer of spi instance
 */
static SPI_TypeDef *spi_instance_addr_get(spi_instance_e spi_instance)
{
	SPI_TypeDef *spi_ptr = NULL;
	
	if ((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT)
	{
		spi_ptr = g_spi_bases[(uint8_t)spi_instance];
	}

	return spi_ptr;
}


/* OCS_Reserved_Start */
/**
 *@brief the handle function of receive word complete interrupt 
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@return    none
 */
static void spi_receive_word_complete_interrupt_handle(spi_instance_e spi_instance)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);
	
	if(spi_drv_interrupt_source_status_get(spi_ptr, SPI_WORD_COMPLETE))
	{
		if (spi_drv_status_flag_get(spi_ptr, SPI_WORD_COMPLETE))
		{
			(void)spi_drv_status_flag_clear(spi_ptr, SPI_WORD_COMPLETE);
			__DSB();
	
			if (spi_state->call_back != NULL)
			{
				spi_state->call_back(spi_state, SPI_EVENT_WORD_COMPLETE, spi_state->call_back_param);
			}
		}
	}
}

/**
 *@brief the handle function of receive frame complete interrupt 
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@return    none
 */
static void spi_receive_frame_complete_interrupt_handle(spi_instance_e spi_instance)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);

	if(spi_drv_interrupt_source_status_get(spi_ptr, SPI_FRAME_COMPLETE))
	{
		if (spi_drv_status_flag_get(spi_ptr, SPI_FRAME_COMPLETE))
		{
			(void)spi_drv_status_flag_clear(spi_ptr, SPI_FRAME_COMPLETE);
			__DSB();
	
			if (spi_state->call_back != NULL)
			{
				spi_state->call_back(spi_state, SPI_EVENT_FRAME_COMPLETE, spi_state->call_back_param);
			}
		}
	}
}
/* OCS_Reserved_End */
/**
 *@brief the handle function of receive data match interrupt 
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@return    none
 */
static void spi_receive_data_match_interrupt_handle(spi_instance_e spi_instance)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);

	if(spi_drv_interrupt_source_status_get(spi_ptr, SPI_DATA_MATCH))
	{
		if (spi_drv_status_flag_get(spi_ptr, SPI_DATA_MATCH))
		{
			(void)spi_drv_status_flag_clear(spi_ptr, SPI_DATA_MATCH);
			__DSB();

			if (spi_state->call_back != NULL)
			{
				spi_state->call_back(spi_state, SPI_EVENT_DATA_MATCH, spi_state->call_back_param);
			}
		}
	}
}

/**
 *@brief fill up the TX FIFO with data
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@return    none
 */
static void spi_tx_buffer_write(spi_instance_e spi_instance)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);	
	
    uint32_t word_to_send;
    uint16_t num_of_bytes;
	uint8_t transimit_words = spi_drv_transimit_words_number_get(spi_ptr);
	uint8_t available_space = (uint8_t)(4U - transimit_words);

	if (true == spi_state->circule_fifo_en)
	{
		spi_drv_circule_fifo_enable(spi_ptr, true);
	}
	
	while(available_space != 0U)
	{
		if (spi_state->is_pcs_continuous == true)
        {
            if(spi_state->tx_count == 1U)
            {
                /* disable continuous PCS */
                spi_drv_cont_cmd_bit_set(spi_ptr, false);
                spi_state->tx_count  = 0U;
                break;
            }
        }

		/* Get the number of bytes which can be written in a single 32 bits word. */
		if ((spi_state->bytes_per_frame - spi_state->tx_frame_cnt) <= (uint16_t)4U)
		{
			num_of_bytes = (uint16_t)(spi_state->bytes_per_frame - spi_state->tx_frame_cnt);
		}
		else
		{
			if(spi_state->tx_count >= 4U)
			{
				num_of_bytes = 4U;
			}
			else
			{
				num_of_bytes = spi_state->tx_count;
			}
		}
		
		word_to_send = 0U;
		
		if (spi_state->tx_buff != NULL && spi_state->tx_valid_count != 0U)
		{
			switch(num_of_bytes)  
			{
				case 1U:
					word_to_send = (uint32_t)(*((const uint8_t *)(spi_state->tx_buff)));
					spi_state->tx_buff += sizeof(uint8_t);
					break;
				case 2U:
					word_to_send = (uint32_t)(*((const uint16_t *)(spi_state->tx_buff)));  
					spi_state->tx_buff += sizeof(uint16_t);
					break;
				case 3U:
					word_to_send = ((uint32_t)(*((const uint16_t *)(spi_state->tx_buff))));
					spi_state->tx_buff += sizeof(uint16_t);
					word_to_send = word_to_send | (((uint32_t)(*((const uint8_t *)(spi_state->tx_buff)))) << 16U);
					spi_state->tx_buff += sizeof(uint8_t);
					break;
				default:
					word_to_send = (uint32_t)(*((const uint32_t *)(spi_state->tx_buff)));
					spi_state->tx_buff += sizeof(uint32_t);
					break;
			}	
		}

		spi_drv_data_write(spi_ptr, word_to_send);
		spi_state->tx_frame_cnt = (uint16_t)((spi_state->tx_frame_cnt + num_of_bytes) % spi_state->bytes_per_frame);
		
		/* Update tx_count */
		spi_state->tx_count = (uint16_t)(spi_state->tx_count - num_of_bytes);

		if(spi_state->tx_valid_count > 0U)
		{
			spi_state->tx_valid_count = (uint16_t)(spi_state->tx_valid_count - num_of_bytes);
		}

		if (true == spi_state->circule_fifo_en)
		{
			spi_state->circule_byte = spi_state->circule_byte - num_of_bytes;
			if (spi_state->circule_byte == 0U)
			{
				(void)spi_drv_status_flag_clear(spi_ptr, SPI_TX_DATA_FLAG);
				__DSB();
				spi_drv_interrupt_source_enable(spi_ptr, SPI_TX_DATA_FLAG, false);
				spi_drv_interrupt_source_enable(spi_ptr, SPI_TRANSMIT_ERROR, false);
				spi_state->tx_count = 0U;
				break;
			}
		}
		
		if (spi_state->tx_count == 0U)
		{
			break;
		}
		
		available_space = (uint8_t)(available_space - 1U);
	}
}

/**
 *@brief read all data from RX FIFO
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@return    none
 */
static void spi_rx_buffer_read(spi_instance_e spi_instance)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance); 
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);
	uint32_t received_word;
	uint16_t num_of_bytes;
	uint16_t j;
	uint8_t data_num_read = spi_drv_receive_words_number_get(spi_ptr);

	while (data_num_read != 0U)
	{
		received_word = spi_drv_data_read(spi_ptr);

		if ((spi_state->bytes_per_frame - spi_state->rx_frame_cnt) <= (uint16_t)4U)
		{
			num_of_bytes = (uint16_t)(spi_state->bytes_per_frame - spi_state->rx_frame_cnt);
		}
		else
		{
			if(spi_state->rx_count >= 4U)
			{
				num_of_bytes = 4U;
			}
			else
			{
				num_of_bytes = spi_state->rx_count;
			}
		}

		if(spi_state->rx_count <= spi_state->rx_valid_count)
		{
			for (j = 0U; j < num_of_bytes; j++)
			{
				*(spi_state->rx_buff) = (uint8_t)(received_word >> (j * 8U));			
				spi_state->rx_buff++;
			}
		}

		spi_state->rx_frame_cnt = (uint16_t)((spi_state->rx_frame_cnt + num_of_bytes) % spi_state->bytes_per_frame);
		spi_state->rx_count = (uint16_t)(spi_state->rx_count - num_of_bytes);
		
		if (spi_state->rx_count == 0U)
		{
			break;
		}
    	data_num_read = (uint8_t)(data_num_read - 1U);
	}
}

/**
 *@brief record some parameter into SPI runtime state for SPI master 
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] *config: pointer to configuration structure
 *@return    none
 */
static void spi_master_state_config(spi_instance_e spi_instance, spi_master_config_t *config)
{
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);
	
	spi_state->bytes_per_frame = (uint16_t)((config->frame_size + 7U) / 8U);
	
	if (spi_state->bytes_per_frame == 3U)
    {
        spi_state->bytes_per_frame = 4U;
    }
	/* Due to some limitations all frames bigger than 4 bytes/frame must be composed only from 4 bytes chunks. */
	if (spi_state->bytes_per_frame > 4U)
    {
        spi_state->bytes_per_frame = (((spi_state->bytes_per_frame - 1U) / 4U) + 1U) * 4U;
    }
	spi_state->is_pcs_continuous = config->is_pcs_continuous;
	spi_state->is_transfer_in_progress = false;
	spi_state->lsb_first = config->lsb_first;
	spi_state->call_back = config->call_back;
	spi_state->call_back_param = config->call_back_param;
	/* When TX is null the value sent on the bus will be 0 */
    spi_state->dummy = 0U;
	spi_state->circule_fifo_en = false;
	spi_state->circule_byte = 0U;
}

/**
 *@brief configure the transmit command register for SPI master
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] *config: pointer to configuration structure
 *@param[in] pre_scale: prescaler value of SPI function clock
 *@return    none
 */
static void spi_master_tcr_reg_config(spi_instance_e spi_instance, spi_master_config_t *config, uint32_t pre_scale)
{
#if 0
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	
	bool tx_mask = false;
	bool rx_mask = false;
	uint32_t command_register;

	/* To ensure the RXMASK bit or TXMASK bit be set when SPI work in half-duplex mode */
	if(config->spi_transfer_width != SPI_SINGLE_BIT)
	{
		rx_mask = true;
	}

	command_register = (((uint32_t)config->spi_sck_polarity << SPI_TCR_CPOL_POS) |
                 ((uint32_t)config->spi_sck_phase << SPI_TCR_CPHA_POS) |
	             (pre_scale << SPI_TCR_PRESC_POS) |
	             ((config->lsb_first ? 1UL : 0UL) << SPI_TCR_LSBF_POS) |
	             ((config->byte_swap ? 1UL : 0UL)<< SPI_TCR_BYSW_POS) |
	             ((config->is_pcs_continuous ? 1UL : 0UL) << SPI_TCR_CONT_EN_POS) |
	             (0UL << SPI_TCR_CONT_CMD_POS) |
	             ((rx_mask ? 1UL : 0UL) << SPI_TCR_RXMASK_POS) |
                 ((tx_mask ? 1UL : 0UL) << SPI_TCR_TXMASK_POS) |
                 ((uint32_t)config->spi_transfer_width << SPI_TCR_WIDTH_POS) |
                 ((uint32_t)config->which_pcs << SPI_TCR_PCSSEL_POS) |                       
                 ((uint32_t)(config->frame_size - 1U) << SPI_TCR_DFS_POS));
    
	spi_drv_tcr_reg_set(spi_ptr, command_register);
	delay_1us(10U);
#endif
}

/**
 *@brief set the SPI baud rate in bits per second
 *@param[in]  spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in]  bits_per_sec: baud rate
 *@param[out] *pre_scale: pointer to prescaler value of SPI function clock
 *@return     the actual calculated baud rate
 */
static uint32_t spi_baud_rate_set(spi_instance_e spi_instance,
									uint32_t bits_per_sec,
									uint32_t *pre_scale)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	uint32_t source_clock;
	uint32_t best_freq = SPI_FREQ_MAX;
    uint32_t best_scaler = 0U;
    uint32_t best_pre_scaler = 0U;
    uint32_t freq1;
    uint32_t freq2;
    uint32_t scaler;
    uint32_t pre_scaler;
    uint32_t low, high;
    uint32_t temp_best_freq;
    uint32_t temp_best_scaler;
	uint32_t SCKSTP;
	uint32_t SCKHLD;

/* OCS_Reserved_Start */	
#ifdef ASIC_MODE
/* OCS_Reserved_End */
	module_name_e instance_clk_name = g_spi_clk_names[(uint8_t)spi_instance];
	(void)clock_freq_get(instance_clk_name, &source_clock);
/* OCS_Reserved_Start */
#else
	source_clock = HIRC_FREQ/2;
#endif
/* OCS_Reserved_End */

	/* search for the best_scaler and best_pre_scaler*/
	for (pre_scaler = 0; pre_scaler < 8U; pre_scaler++)
    {
        low = SPI_SCALER_LOW;
        high = SPI_SCALER_HIGH;

        do
        {
            scaler = ((low + high) / 2U);
            freq1 = source_clock / (baudrate_pre_scaler[pre_scaler] * (scaler + 2UL));

            if (abs_dif(bits_per_sec, best_freq) > abs_dif(bits_per_sec, freq1))
            {
                best_freq = freq1;
            }
            if (freq1 < bits_per_sec)
            {
                high = scaler;
            }
            else
            {
                low = scaler;
            }
        }
        while((high - low) > 1U);

        /* Evaluate last 2 scaler values */
        freq1 = source_clock / (baudrate_pre_scaler[pre_scaler] * (low + 2UL));
        freq2 = source_clock / (baudrate_pre_scaler[pre_scaler] * (high + 2UL));

        if (abs_dif(bits_per_sec, freq1) > abs_dif(bits_per_sec, freq2))
        {
            temp_best_freq = freq2;
            temp_best_scaler = high;
        }
        else
        {
            temp_best_freq = freq1;
            temp_best_scaler = low;
        }

        if (abs_dif(bits_per_sec, best_freq) >= abs_dif(bits_per_sec, temp_best_freq))
        {
            best_freq = temp_best_freq;
            best_scaler = temp_best_scaler;
            best_pre_scaler = pre_scaler;
        }

        /* If current frequency is equal to target frequency  stop the search */
        if(best_freq == bits_per_sec)
        {
            break;
        }
    }

	SCKHLD = best_scaler / 2U;
	SCKSTP = best_scaler - SCKHLD;

	spi_drv_timing_set(spi_ptr, best_scaler, best_scaler, best_scaler, best_scaler, SCKSTP, SCKHLD);
	
	/* return the best prescaler value for user to use later */
    *pre_scale = best_pre_scaler;

	/* return the actual calculated baud rate */
    return best_freq;
}

/**
 *@brief get the absolute difference between two numbers 
 *@param[in] a: number one.
 *@param[in] b: number two.
 *@return    the absolute difference between two numbers
 */
static uint32_t abs_dif(uint32_t a, uint32_t b)
{
    uint32_t abs_ret;
    if (a > b)
    {
        abs_ret = a - b;
    }
    else
    {
        abs_ret = b - a;
    }
    
    return abs_ret;
}

/**
 *@brief update the SPI runtime state for master
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] *send_buffer: the buffer pointer which saves the data to be sent
 *@param[in] *receive_buffer: the buffer pointer which saves the data to be received
 *@param[in] tx_count: the total tx size of data of this transfer activity
 *@param[in] rx_count: the total rx size of data of this transfer activity
 *@param[in] tx_valid_count: the size of data that spi really want to send
 *@param[in] rx_valid_count: the size of data that spi really want to receive
 *@return    none
 */
static void spi_master_state_set(spi_instance_e spi_instance,
							const uint8_t *send_buffer,
							uint8_t *receive_buffer,
							uint16_t tx_count,
							uint16_t rx_count,
							uint16_t tx_valid_count,
							uint16_t rx_valid_count)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);

	if (receive_buffer == NULL)
    {
        spi_state->rx_count = 0U;
		spi_drv_rx_mask_set(spi_ptr, true);
    }
    else
    {
        spi_state->rx_count = rx_count;
		spi_drv_rx_mask_set(spi_ptr, false);
    }
	spi_state->tx_buff = send_buffer;
	spi_state->rx_buff = receive_buffer;
	spi_state->tx_count = tx_count;
	/*For continuous mode an extra word must be written to negate the PCS */
    if (spi_state->is_pcs_continuous == true)
    {
        spi_state->tx_count++;
    }
	spi_state->tx_valid_count = tx_valid_count;
	spi_state->rx_valid_count = rx_valid_count;
	spi_state->tx_frame_cnt = 0U;
	spi_state->rx_frame_cnt = 0U;
}


/**
 *@brief finish up a transfer
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@return    none
 */
static void spi_master_transfer_complete(spi_instance_e spi_instance)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);

	/* The transfer is complete.*/
    spi_state->is_transfer_in_progress = false;
	
	/* Disable (clear) interrupt requests */
	spi_drv_interrupt_source_enable(spi_ptr, SPI_RX_DATA_FLAG, false);
	spi_drv_interrupt_source_enable(spi_ptr, SPI_TX_DATA_FLAG, false);
	
	/* Disable (clear) all interrupt requests */
	spi_drv_all_interrupts_source_disable(spi_ptr);
	/* Clears all SPI status flag */
	(void)spi_drv_status_flag_clear(spi_ptr, SPI_ALL_STATUS);
	/*wait clearing flag completed*/
	__DSB();

	if (spi_state->call_back != NULL)
    {
        spi_state->call_back(spi_state, SPI_EVENT_TANSFER_FINISH, spi_state->call_back_param);
    }
}

/**
 *@brief record some parameter into SPI runtime state for SPI slave 
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] *config: pointer to configuration structure
 *@return    none
 */
static void spi_slave_state_config(spi_instance_e spi_instance, spi_slave_config_t *config)
{
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);

	spi_state->bytes_per_frame = (uint16_t)((config->frame_size + 7U) / 8U);
	/* Due to DMA limitations frames of 3 bytes/frame will be internally handled as 4 bytes/frame. */
	if (spi_state->bytes_per_frame == 3U)
    {
        spi_state->bytes_per_frame = 4U;
    }
	/* Due to some limitations all frames bigger than 4 bytes/frame must be composed only from 4 bytes chunks. */
	if (spi_state->bytes_per_frame > 4U)
    {
        spi_state->bytes_per_frame = (((spi_state->bytes_per_frame - 1U) / 4U) + 1U) * 4U;
    }

	spi_state->is_pcs_continuous = config->is_pcs_continuous;
	spi_state->is_transfer_in_progress = false;
	spi_state->lsb_first = config->lsb_first;
	spi_state->call_back = config->call_back;
	spi_state->call_back_param = config->call_back_param;
	/* When TX is null the value sent on the bus will be 0xFF */
    spi_state->dummy = SPI_DUMMY_VALUE;
	spi_state->circule_fifo_en = false;
	spi_state->circule_byte = 0U;
}

/**
 *@brief configure the transmit command register for SPI slave 
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] *config: pointer to configuration structure
 *@return    none
 */
static void spi_slave_tcr_reg_config(spi_instance_e spi_instance, spi_slave_config_t *config)
{
#if 0
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	bool tx_mask = false;
	bool rx_mask = false;
	uint32_t command_register;

	/* To ensure the RXMASK bit or TXMASK bit be set when SPI work in half-duplex mode */
	if(config->spi_transfer_width != SPI_SINGLE_BIT)
	{
		tx_mask = true;
	}

	command_register = (((uint32_t)config->spi_sck_polarity << SPI_TCR_CPOL_POS) |
	                 ((uint32_t)config->spi_sck_phase << SPI_TCR_CPHA_POS) |
	                 (0UL << SPI_TCR_PRESC_POS) |
	                 ((config->lsb_first ? 1UL : 0UL) << SPI_TCR_LSBF_POS) |
	                 ((config->byte_swap ? 1UL : 0UL)<< SPI_TCR_BYSW_POS) |
	                 ((config->is_pcs_continuous ? 1UL : 0UL) << SPI_TCR_CONT_EN_POS) |
	                 (0UL << SPI_TCR_CONT_CMD_POS) |
	                 ((rx_mask ? 1UL : 0UL) << SPI_TCR_RXMASK_POS) |
                     ((tx_mask ? 1UL : 0UL) << SPI_TCR_TXMASK_POS) |
	                 ((uint32_t)config->spi_transfer_width << SPI_TCR_WIDTH_POS) |
	                 ((uint32_t)config->which_pcs << SPI_TCR_PCSSEL_POS) |                       
	                 ((uint32_t)(config->frame_size - 1U) << SPI_TCR_DFS_POS));
	
	spi_drv_tcr_reg_set(spi_ptr, command_register);
	delay_1us(10);
#endif
}

/**
 *@brief update the runtime state for SPI slave
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] *send_buffer: the buffer pointer which saves the data to be sent
 *@param[in] *receive_buffer: the buffer pointer which saves the data to be received
 *@param[in] tx_count: the total tx size of data of this transfer activity
 *@param[in] rx_count: the total rx size of data of this transfer activity
 *@param[in] tx_valid_count: the size of data that spi really want to send
 *@param[in] rx_valid_count: the size of data that spi really want to receive
 *@return    none
 */
static void spi_slave_state_set(spi_instance_e spi_instance,
							const uint8_t *send_buffer,
							uint8_t *receive_buffer,
							uint16_t tx_count,
							uint16_t rx_count,
							uint16_t tx_valid_count,
							uint16_t rx_valid_count)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);
	
	spi_state->tx_buff = send_buffer;
	spi_state->rx_buff = receive_buffer;

	if (send_buffer == NULL)
	{
		spi_state->tx_count = 0U;
		spi_drv_tx_mask_set(spi_ptr, true);
	}
	else
	{
		spi_state->tx_count = tx_count;
		spi_drv_tx_mask_set(spi_ptr, false);
	}
	
	if (receive_buffer == NULL)
	{
		spi_state->rx_count = 0U;
		spi_drv_rx_mask_set(spi_ptr, true);
	}
	else
	{
		spi_state->rx_count = rx_count;
		spi_drv_rx_mask_set(spi_ptr, false);
	}
	
	spi_state->tx_valid_count = tx_valid_count;
	spi_state->rx_valid_count = rx_valid_count;
	spi_state->tx_frame_cnt = 0U;
	spi_state->rx_frame_cnt = 0U;
	spi_state->is_pcs_continuous = false;
}



/**
 *@brief interrupt handler function for SPI master
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@return    none
 */
static void spi_master_irq_handler(spi_instance_e spi_instance)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);

	/* If an error is detected the transfer will be aborted */
	if (spi_drv_status_flag_get(spi_ptr, SPI_TRANSMIT_ERROR) && (spi_state->tx_buff != NULL))
	{
		(void)spi_master_transfer_abort(spi_instance);
		(void)spi_drv_status_flag_clear(spi_ptr, SPI_TRANSMIT_ERROR);
		spi_state->status = SPI_TRANSMIT_FAIL;

		if (spi_state->call_back != NULL)
		{
			spi_state->call_back(spi_state, SPI_EVENT_TRANSMIT_ERROR, spi_state->call_back_param);
		}
/* OCS_Reserved_Start */
		INFO("spi master underrun occur!\n");
/* OCS_Reserved_End */
        return;
	}

	if (spi_drv_status_flag_get(spi_ptr, SPI_RECEIVE_ERROR) && (spi_state->rx_buff != NULL))
	{
		(void)spi_master_transfer_abort(spi_instance);
		(void)spi_drv_status_flag_clear(spi_ptr, SPI_RECEIVE_ERROR);
		spi_state->status = SPI_RECEIVE_FAIL;

		if (spi_state->call_back != NULL)
		{
			spi_state->call_back(spi_state, SPI_EVENT_RECEIVE_ERROR, spi_state->call_back_param);
		}
/* OCS_Reserved_Start */
		INFO("spi master overrun occur!\n");
/* OCS_Reserved_End */
        return;
	}

/* OCS_Reserved_Start */
	/* Handle receive word complete interrupt */
	spi_receive_word_complete_interrupt_handle(spi_instance);
	/* Handle receive frame complete interrupt */
	spi_receive_frame_complete_interrupt_handle(spi_instance);
/* OCS_Reserved_End */
	/* Handle receive data match interrupt */
	spi_receive_data_match_interrupt_handle(spi_instance);

	/* Receive data */
	if (spi_drv_status_flag_get(spi_ptr, SPI_RX_DATA_FLAG))
	{
		if (spi_state->rx_count != (uint16_t)0U)
		{
		    spi_rx_buffer_read(spi_instance);
		}
	}

	/* Transmit data */
    if (spi_drv_status_flag_get(spi_ptr, SPI_TX_DATA_FLAG))
    {
        if ((spi_state->tx_count != (uint16_t)0U))
        {
            spi_tx_buffer_write(spi_instance);
        }
    }

	if (spi_state->tx_count == (uint16_t)0U)
	{
		spi_drv_interrupt_source_enable(spi_ptr, SPI_TX_DATA_FLAG, false);
        spi_drv_interrupt_source_enable(spi_ptr, SPI_TRANSFER_COMPLETE, true);
        /* Check if we're done with this transfer.*/
        if (spi_state->rx_count == (uint16_t)0U)
        {
        	if (spi_drv_status_flag_get(spi_ptr, SPI_TRANSFER_COMPLETE) == true)
            {
				if (spi_state->call_back != NULL)
				{
					spi_state->call_back(spi_state, SPI_EVENT_TX_EMPTY, spi_state->call_back_param);
				}

				if ((spi_state->tx_count == (uint16_t)0 )&& (spi_state->rx_count == (uint16_t)0))
				{
					spi_master_transfer_complete(spi_instance);
				}
				else
				{
					/* clear all flag. */
					(void)spi_drv_status_flag_clear(spi_ptr, SPI_ALL_STATUS);
					/* disable the transfer complete interrupt. */
	        		spi_drv_interrupt_source_enable(spi_ptr, SPI_TRANSFER_COMPLETE, false);

					/* Enable RX interrupt if RX buffer is not NULL. */
				    if (spi_state->rx_buff != NULL)
				    {
				        spi_drv_interrupt_source_enable(spi_ptr, SPI_RX_DATA_FLAG, true);
				    }
				    /* Enable the TX interrupt. */
				    spi_drv_interrupt_source_enable(spi_ptr, SPI_TX_DATA_FLAG, true);
				}
            }
        }
	}

}

/**
 *@brief write the TX data when control the pcs manually
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@return    none
 */
static void spi_fill_data_reg(spi_instance_e spi_instance)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);	
	
    uint32_t word_to_send;
    uint16_t num_of_bytes;
	
	uint16_t available_space = (spi_state->bytes_per_frame + 3U)/4U;

	while(available_space != 0U)
	{
		/* Get the number of bytes which can be written in a single 32 bits word. */
		if ((spi_state->bytes_per_frame - spi_state->tx_frame_cnt) <= (uint16_t)4U)
		{
			num_of_bytes = (uint16_t)(spi_state->bytes_per_frame - spi_state->tx_frame_cnt);
		}
		else
		{
			if(spi_state->tx_count >= 4U)
			{
				num_of_bytes = 4U;
			}
			else
			{
				num_of_bytes = spi_state->tx_count;
			}
		}
		
		word_to_send = 0U;
		
		if (spi_state->tx_buff != NULL && spi_state->tx_valid_count != 0U)
		{
			switch(num_of_bytes)  
			{
				case 1U:
					word_to_send = (uint32_t)(*((const uint8_t *)(spi_state->tx_buff)));
					spi_state->tx_buff += sizeof(uint8_t);
					break;
				case 2U:
					word_to_send = (uint32_t)(*((const uint16_t *)(spi_state->tx_buff)));  
					spi_state->tx_buff += sizeof(uint16_t);
					break;
				case 3U:
					word_to_send = (uint32_t)(*((const uint16_t *)(spi_state->tx_buff)));
					spi_state->tx_buff += sizeof(uint16_t);
					word_to_send = word_to_send | ((uint32_t)(*((const uint8_t *)(spi_state->tx_buff))) << 16U);
					spi_state->tx_buff += sizeof(uint8_t);
					break;
				default:
					word_to_send = (uint32_t)(*((const uint32_t *)(spi_state->tx_buff)));
					spi_state->tx_buff += sizeof(uint32_t);
					break;
			}	
		}

		spi_drv_data_write(spi_ptr, word_to_send);
		spi_state->tx_frame_cnt = (uint16_t)((spi_state->tx_frame_cnt + num_of_bytes) % spi_state->bytes_per_frame);
		
		/* Update tx_count */
		spi_state->tx_count = (uint16_t)(spi_state->tx_count - num_of_bytes);

		if(spi_state->tx_valid_count > 0U)
		{
			spi_state->tx_valid_count = (uint16_t)(spi_state->tx_valid_count - num_of_bytes);
		}

		if (spi_state->tx_count == 0U)
		{
			break;
		}
		
		available_space = (uint16_t)(available_space - 1U);
	}
}

/**
 *@brief read the RX data when control the pcs manually
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@return    none
 */
static void spi_read_data_reg(spi_instance_e spi_instance)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance); 
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);
	uint32_t received_word;
	uint16_t num_of_bytes;
	uint16_t j;
	uint8_t data_num_read = spi_drv_receive_words_number_get(spi_ptr);

	while (data_num_read != 0U)
	{
		received_word = spi_drv_data_read(spi_ptr);

		if (((spi_state->bytes_per_frame) - (spi_state->rx_frame_cnt)) <= (uint16_t)4U)
		{
			num_of_bytes = (uint16_t)(spi_state->bytes_per_frame - spi_state->rx_frame_cnt);
		}
		else
		{
			if(spi_state->rx_count >= 4U)
			{
				num_of_bytes = 4U;
			}
			else
			{
				num_of_bytes = spi_state->rx_count;
			}
		}

		if(spi_state->rx_count <= spi_state->rx_valid_count)
		{
			for (j = 0U; j < num_of_bytes; j++)
			{
				*(spi_state->rx_buff) = (uint8_t)(received_word >> (j * 8U));			
				spi_state->rx_buff++;
			}
		}

		spi_state->rx_frame_cnt = (uint16_t)((spi_state->rx_frame_cnt + num_of_bytes) % spi_state->bytes_per_frame);
		spi_state->rx_count = (uint16_t)(spi_state->rx_count - num_of_bytes);
		
		if (spi_state->rx_count == 0U)
		{
			break;
		}
    	data_num_read = (uint8_t)(data_num_read - 1U);
	}	

}

/**
 *@brief interrupt handler function for SPI master when control the pcs manually
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@return    none
 */
void spi_master_irq_handler_manual_pcs(spi_instance_e spi_instance)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);

	/* Receive data */
	if (spi_drv_status_flag_get(spi_ptr, SPI_FRAME_COMPLETE))
	{
		(void)spi_drv_status_flag_clear(spi_ptr, SPI_FRAME_COMPLETE);
		__DSB();

		/* negative the pcs gpio */
		g_spi_pcs_negative_function[(uint8_t)spi_instance]();
		/* read the rx data register*/
		spi_read_data_reg(spi_instance);

		if (spi_state->rx_count == (uint16_t)0U)
        {
			spi_master_transfer_complete(spi_instance);
        }
		else
		{
			/* active the pcs gpio */
			g_spi_pcs_active_function[(uint8_t)spi_instance]();
			/* fill the tx data register*/
			spi_fill_data_reg(spi_instance);
		}
	}
}

/**
 *@brief interrupt handler function for SPI slave
 *@param[in] spi_instance: spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@return    none
 */
static void spi_slave_irq_handler(spi_instance_e spi_instance)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_state_t *spi_state = &(g_spi_state[(uint8_t)spi_instance]);

	/* If an error is detected the transfer will be aborted */
	if (spi_drv_status_flag_get(spi_ptr, SPI_TRANSMIT_ERROR) && (spi_state->tx_buff != NULL))
	{
		/* terminates a SPI slave transfer */
		(void)spi_slave_transfer_abort(spi_instance);
		spi_state->status = SPI_TRANSMIT_FAIL;

		if (spi_state->call_back != NULL)
		{
			spi_state->call_back(spi_state, SPI_EVENT_TRANSMIT_ERROR, spi_state->call_back_param);
		}
/* OCS_Reserved_Start */
		INFO("spi slave underrun occur!\n");
/* OCS_Reserved_End */
        return;
	}

	if (spi_drv_status_flag_get(spi_ptr, SPI_RECEIVE_ERROR) && (spi_state->rx_buff != NULL))
	{
		/* terminates a SPI slave transfer */
		(void)spi_slave_transfer_abort(spi_instance);
		spi_state->status = SPI_RECEIVE_FAIL;

		if (spi_state->call_back != NULL)
		{
			spi_state->call_back(spi_state, SPI_EVENT_RECEIVE_ERROR, spi_state->call_back_param);
		}
/* OCS_Reserved_Start */
		INFO("spi slave overrun occur!\n");
/* OCS_Reserved_End */
        return;
	}

/* OCS_Reserved_Start */
		/* Handle receive word complete interrupt */
		spi_receive_word_complete_interrupt_handle(spi_instance);
		/* Handle receive frame complete interrupt */
		spi_receive_frame_complete_interrupt_handle(spi_instance);
		/* Handle receive data match interrupt */
		spi_receive_data_match_interrupt_handle(spi_instance);
/* OCS_Reserved_End */

	/* Receive data */
	if (spi_drv_status_flag_get(spi_ptr, SPI_RX_DATA_FLAG))
	{
		if (spi_state->rx_count != (uint16_t)0U)
		{
		    spi_rx_buffer_read(spi_instance);
		}
	}

	/* Transmit data */
    if (spi_drv_status_flag_get(spi_ptr, SPI_TX_DATA_FLAG))
    {
        if ((spi_state->tx_count != (uint16_t)0U))
        {
            spi_tx_buffer_write(spi_instance);
        }
    }

	/* If all bytes are sent disable interrupt TDF */
	if (spi_state->tx_count == (uint16_t)0U)
	{
		spi_drv_interrupt_source_enable(spi_ptr, SPI_TX_DATA_FLAG, false);
	}

	/* If all bytes are received disable interrupt RDF */
	if (spi_state->rx_count == (uint16_t)0U)
    {
        spi_drv_interrupt_source_enable(spi_ptr, SPI_RX_DATA_FLAG, false);
    }

	if (spi_state->rx_count == (uint16_t)0U)
	{
		if (spi_state->tx_count == (uint16_t)0U)
		{
			if (spi_state->call_back != NULL)
		    {
		        spi_state->call_back(spi_state, SPI_EVENT_RX_FULL, spi_state->call_back_param);
		    }

			if ((spi_state->rx_count == (uint16_t)0) && (spi_state->tx_count == (uint16_t)0))
			{
				/* Disable fault interrupts sources */
	            spi_drv_interrupt_source_enable(spi_ptr, SPI_TRANSMIT_ERROR, false);
	            spi_drv_interrupt_source_enable(spi_ptr, SPI_RECEIVE_ERROR, false);

				/* finish a SPI slave transfer */
				(void)spi_slave_transfer_abort(spi_instance);

				if (spi_state->call_back != NULL)
			    {
			        spi_state->call_back(spi_state, SPI_EVENT_TANSFER_FINISH, spi_state->call_back_param);
			    }

				/* Update internal state of the transfer */
				spi_state->is_transfer_in_progress = false;
			}
			else
			{
				/* Enable interrupts for RX and TX only if it's necessary */
		        if (spi_state->tx_buff != NULL)
		        {
					spi_drv_interrupt_source_enable(spi_ptr, SPI_TX_DATA_FLAG, true);
		        }
		        if (spi_state->rx_buff != NULL)
		        {
		            spi_drv_interrupt_source_enable(spi_ptr, SPI_RX_DATA_FLAG, true);
		        }	
			}
		}
	}



}

/**
 *@brief the handle function of SPI for master and slave.
 *@param[in] spi_instance, spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@return    none
 */
static void spi_irq_handler(spi_instance_e spi_instance)
{

    SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
    if (spi_drv_ismaster(spi_ptr))
    {
        /* Master mode.*/
        spi_master_irq_handler(spi_instance);
    }
    else
    {
        /* Slave mode.*/
        spi_slave_irq_handler(spi_instance);
    }
}

/**
 *@brief specify interrupt handler for a SPI instance
 *@param[in] spi_instance: SPI instance number
 *@param[in] handle: interrupt handler
 *@return    none
 */
void spi_interrupt_handle_cfg(spi_instance_e spi_instance, SPI_INT_HANDLE handle)
{
	DEV_ASSERT((uint8_t)spi_instance < SPI_INSTANCE_MAX_COUNT);

	g_spi_int_handle[(uint8_t)spi_instance] = handle;
}

#endif

uint8_t slave_data_recv_buffer[DATA_LENGTH] = {0};
uint8_t slave_data_send_buffer[DATA_LENGTH] = {0};

//slave_send_num、slave_recv_num 每次SPI通讯完成之后都要归0，重新指向缓冲区首地址
uint8_t slave_send_num = 0;
uint8_t slave_recv_num = 0;

void slave_data_buffer(void)
{
	uint16_t i;
	for(i = 0; i < DATA_LENGTH; i++){
		if((i % 2) == 0)
			slave_data_send_buffer[i] = 9;
		else
			slave_data_send_buffer[i] = 10;
	}
}


uint8_t *slave_send_data_p = NULL;
uint8_t *slave_recv_data_p = NULL;

SPI_HandleTypeDef SPI0_config = {
	.spi_master_config = {
		// 基础传输格式配置（对应0x10 TRANSFMT寄存器）
    	.baseConfig = {
    	    .CPOL = 0,                    // SPI Mode0：空闲时钟低电平
    	    .CPHA = 0,                    // SPI Mode0：奇数时钟边沿采样
    	    .IsLsbFirst = 0,              // 标准SPI：MSB优先传输
    	    .AddrLenByte = 1,             // 2字节地址（适配绝大多数SPI Flash）
    	    .DataLenBit = 7,              // 8bit数据位宽（标准字节传输）
    	    .IsDataMergeEn = 0,           // 开启数据合并（32位FIFO读写，提升效率）
    	    .IsMosiBiDirEn = 0,           // 常规单向MOSI/MISO，不开启双向
    	    .IoMode = SPI_IO_MODE_SINGLE  // 标准单IO模式
    	},
    	// 传输控制默认配置（对应0x20 TRANSCTRL寄存器，收发时可动态修改）
    	.transConfig = {
    	    .IsCmdEn = 0,                 // 使能命令阶段
    	    .IsAddrEn = 0,                // 使能地址阶段
    	    .AddrFmtSameAsData = 0,       // 地址阶段固定单IO模式
    	    .TransMode = SPI_TRANS_MODE_WR_SYNC, // 默认读写模式
    	    .IsTokenEn = 0,               // 禁用读传输令牌
    	    .TokenValue = 0,
    	    .DummyCnt = 0,                // 默认无虚拟周期
    	    .WrTranCnt = 15, // 默认最大数据长度，收发时动态修改
    	    .RdTranCnt = 15,
    	    .CmdCode = 0x00,              // 命令值，收发时动态赋值
    	    .TransAddr = 0x3       // 传输地址，收发时动态赋值
    	},
    	// FIFO与DMA配置（对应0x30 CTRL寄存器）
    	.fifoDmaConfig = {
    	    .TxThres = 4, // TX FIFO阈值
    	    .RxThres = 1, // RX FIFO阈值
    	    .IsTxDmaEn = 0,               // 标准初始化禁用TX DMA，轮询模式
    	    .IsRxDmaEn = 0                // 标准初始化禁用RX DMA，轮询模式
    	},
    	// 接口时序配置（对应0x40 TIMING寄存器，仅主机需配置）
    	.timingConfig = {
    	    .Cs2Sclk = 0,                 // CS到SCLK最小间隔：1个SCLK半周期
    	    .CsHoldTime = 2,              // CS高电平保持时间：3个SCLK半周期
    	    .SCLK_SPEED = 1000000               // SCLK分频：源时钟/32，可根据源时钟动态调整
    	                                  // 例：100MHz源时钟 → 3.125MHz SCLK，兼容绝大多数外设
    	},
    	// 存储器映射访问配置（对应0x50 MEMCTRL寄存器）
    	.memMapConfig = {
    	    .MemReadCmd = 0x00            // 默认3字节地址常规读命令（0x03），对应手册Table17
    	},
#ifdef ATCSPI200_DIRECT_IO_SUPPORT
    	// 直接IO配置（默认禁用，常规SPI通信无需开启）
    	.directIoConfig = {
    	    .IsDirectIOEn = 0,            // 禁用直接IO，由硬件自动控制SPI引脚
    	    .CsOe = 1,                    // CS输出使能
    	    .SclkOe = 1,                  // SCLK输出使能
    	    .MosiOe = 1,                  // MOSI输出使能
    	    .MisoOe = 0,                  // MISO输入，关闭输出使能
    	    .WpOe = 1,
    	    .HoldOe = 1,
    	    .CsOut = 1,                   // CS默认高电平（无效）
    	    .SclkOut = 0,
    	    .MosiOut = 0,
    	    .MisoOut = 0,
    	    .WpOut = 1,
    	    .HoldOut = 1
    	}
#endif
	},
	.spi_slave_config = {
		.baseConfig = {
        	.CPOL = 0,                    // SPI Mode0
        	.CPHA = 0,                    // SPI Mode0
        	.IsLsbFirst = 0,              // MSB优先
        	.DataLenBit = 7,              // 8bit数据位宽
        	.IsDataMergeEn = 0,           // 数据合并
        	.IsMosiBiDirEn = 0,           // 单向MOSI/MISO
        	.IoMode = SPI_IO_MODE_SINGLE  // 单IO模式
    	},
    	// 传输控制配置（从机仅需同步IO模式，其余由主机控制）
    		.transConfig = {
    	    .IoMode = SPI_IO_MODE_SINGLE  // 与baseConfig保持一致
    	},
    	// FIFO与DMA配置（对应0x30 CTRL寄存器）
    	.fifoDmaConfig = {
    	    .TxThres = 4, // TX FIFO阈值
    	    .RxThres = 1, // RX FIFO阈值
    	    .IsTxDmaEn = 0,               // 标准初始化禁用TX DMA
    	    .IsRxDmaEn = 0                // 标准初始化禁用RX DMA
    	},
#ifdef ATCSPI200_SLAVE_SUPPORT
    // 从机核心状态配置（对应0x60 SLVST寄存器）
    		.coreConfig = {
        	.UserStatus = 0x1234,        // 用户自定义状态，主机可通过0x05命令读取
        	.IsReady = 0                  // 初始化未就绪，配置完成后置1
		},
#endif
#ifdef ATCSPI200_DIRECT_IO_SUPPORT
    // 直接IO配置（从机必须禁用！避免干扰主机总线控制）
    		.directIoConfig = {
        	.IsDirectIOEn = 0             // 禁用直接IO，总线完全由主机控制
		}
#endif
	}
};

SPI_HandleTypeDef SPI1_config = {
	.spi_master_config = {
		// 基础传输格式配置（对应0x10 TRANSFMT寄存器）
    	.baseConfig = {
    	    .CPOL = 0,                    // SPI Mode0：空闲时钟低电平
    	    .CPHA = 0,                    // SPI Mode0：奇数时钟边沿采样
    	    .IsLsbFirst = 0,              // 标准SPI：MSB优先传输
    	    .AddrLenByte = 1,             // 2字节地址（适配绝大多数SPI Flash）
    	    .DataLenBit = 7,              // 8bit数据位宽（标准字节传输）
    	    .IsDataMergeEn = 0,           // 开启数据合并（32位FIFO读写，提升效率）
    	    .IsMosiBiDirEn = 0,           // 常规单向MOSI/MISO，不开启双向
    	    .IoMode = SPI_IO_MODE_SINGLE  // 标准单IO模式
    	},
    	// 传输控制默认配置（对应0x20 TRANSCTRL寄存器，收发时可动态修改）
    	.transConfig = {
    	    .IsCmdEn = 0,                 // 使能命令阶段
    	    .IsAddrEn = 0,                // 使能地址阶段
    	    .AddrFmtSameAsData = 0,       // 地址阶段固定单IO模式
    	    .TransMode = SPI_TRANS_MODE_WR_SYNC, // 默认读写模式
    	    .IsTokenEn = 0,               // 禁用读传输令牌
    	    .TokenValue = 0,
    	    .DummyCnt = 0,                // 默认无虚拟周期
    	    .WrTranCnt = 15, // 默认最大数据长度，收发时动态修改
    	    .RdTranCnt = 15,
    	    .CmdCode = 0x00,              // 命令值，收发时动态赋值
    	    .TransAddr = 0x3       // 传输地址，收发时动态赋值
    	},
    	// FIFO与DMA配置（对应0x30 CTRL寄存器）
    	.fifoDmaConfig = {
    	    .TxThres = 4, // TX FIFO阈值
    	    .RxThres = 1, // RX FIFO阈值
    	    .IsTxDmaEn = 0,               // 标准初始化禁用TX DMA，轮询模式
    	    .IsRxDmaEn = 0                // 标准初始化禁用RX DMA，轮询模式
    	},
    	// 接口时序配置（对应0x40 TIMING寄存器，仅主机需配置）
    	.timingConfig = {
    	    .Cs2Sclk = 0,                 // CS到SCLK最小间隔：1个SCLK半周期
    	    .CsHoldTime = 2,              // CS高电平保持时间：3个SCLK半周期
    	    .SCLK_SPEED = 1000000               // SCLK分频：源时钟/32，可根据源时钟动态调整
    	                                  // 例：100MHz源时钟 → 3.125MHz SCLK，兼容绝大多数外设
    	},
    	// 存储器映射访问配置（对应0x50 MEMCTRL寄存器）
    	.memMapConfig = {
    	    .MemReadCmd = 0x00            // 默认3字节地址常规读命令（0x03），对应手册Table17
    	},
#ifdef ATCSPI200_DIRECT_IO_SUPPORT
    	// 直接IO配置（默认禁用，常规SPI通信无需开启）
    	.directIoConfig = {
    	    .IsDirectIOEn = 0,            // 禁用直接IO，由硬件自动控制SPI引脚
    	    .CsOe = 1,                    // CS输出使能
    	    .SclkOe = 1,                  // SCLK输出使能
    	    .MosiOe = 1,                  // MOSI输出使能
    	    .MisoOe = 0,                  // MISO输入，关闭输出使能
    	    .WpOe = 1,
    	    .HoldOe = 1,
    	    .CsOut = 1,                   // CS默认高电平（无效）
    	    .SclkOut = 0,
    	    .MosiOut = 0,
    	    .MisoOut = 0,
    	    .WpOut = 1,
    	    .HoldOut = 1
    	}
#endif
	},
	.spi_slave_config = {
		.baseConfig = {
        	.CPOL = 0,                    // SPI Mode0
        	.CPHA = 0,                    // SPI Mode0
        	.IsLsbFirst = 0,              // MSB优先
        	.DataLenBit = 7,              // 8bit数据位宽
        	.IsDataMergeEn = 0,           // 数据合并
        	.IsMosiBiDirEn = 0,           // 单向MOSI/MISO
        	.IoMode = SPI_IO_MODE_SINGLE  // 单IO模式
    	},
    	// 传输控制配置（从机仅需同步IO模式，其余由主机控制）
    		.transConfig = {
    	    .IoMode = SPI_IO_MODE_SINGLE  // 与baseConfig保持一致
    	},
    	// FIFO与DMA配置（对应0x30 CTRL寄存器）
    	.fifoDmaConfig = {
    	    .TxThres = 4, // TX FIFO阈值
    	    .RxThres = 1, // RX FIFO阈值
    	    .IsTxDmaEn = 0,               // 标准初始化禁用TX DMA
    	    .IsRxDmaEn = 0                // 标准初始化禁用RX DMA
    	},
#ifdef ATCSPI200_SLAVE_SUPPORT
    // 从机核心状态配置（对应0x60 SLVST寄存器）
    		.coreConfig = {
        	.UserStatus = 0x1234,        // 用户自定义状态，主机可通过0x05命令读取
        	.IsReady = 0                  // 初始化未就绪，配置完成后置1
		},
#endif
#ifdef ATCSPI200_DIRECT_IO_SUPPORT
    // 直接IO配置（从机必须禁用！避免干扰主机总线控制）
    		.directIoConfig = {
        	.IsDirectIOEn = 0             // 禁用直接IO，总线完全由主机控制
		}
#endif
	}
};

//Mark the data in the current send buffer that is about to be sent to the TX FIFO
transmit_data_index send_data_index = {	
	.data_current = NULL,
	.data_len = 0,
	.data_index = 0
};
//Mark the data currently received from the RX FIFO and store it in the receive buffer
transmit_data_index recv_data_index = {
	.data_current = NULL,
	.data_len = 0,
	.data_index = 0
};

/**************************************************************** master ************************************************************/
#if 1
/**
 *@brief Initialize send data buffer
 *@param[in] data_buffer: The starting address of the buffer structure variable to be initialized
 *@return    none
 */
void spi_hal_send_data_buffer_init(spi_instance_e spi_instance, transmit_data *data_buffer)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);

	//Point the real-time data structure member to the send buffer
	send_data_index.data_current = data_buffer->data;
	send_data_index.data_len = data_buffer->data_len;
	send_data_index.data_index = 0;

	//clean tx fifo
	spi_ptr->CTRL.TXFIFORST = 1;

	spi_ptr->TRANSCTRL.WRTRANCNT = data_buffer->data_len - 1;
	
}

/**
 *@brief Initialize recv data buffer
 *@param[in] data_buffer: The starting address of the buffer structure variable to be initialized
 *@return    none
 */
void spi_hal_recv_data_buffer_init(spi_instance_e spi_instance, transmit_data *data_buffer)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);

	recv_data_index.data_current = data_buffer->data;
	recv_data_index.data_len = data_buffer->data_len;
	recv_data_index.data_index = 0;

	//clean rx fifo
	spi_ptr->CTRL.RXFIFORST = 1;

	spi_ptr->TRANSCTRL.RDTRANCNT = data_buffer->data_len - 1;

}

/**
 *@brief Send and receive data in send-receive mode
 *@param[in] spi_instance, spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] send_data_buffer: Send Data Buffer
 *@param[in] recv_data_buffer: Recv Data Buffer
 *@return    none
 */
void spi_hal_master_send_recv_data(spi_instance_e spi_instance, transmit_data *send_data_buffer, transmit_data *recv_data_buffer)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	SPI0_config.spi_master_config.transConfig.TransMode = SPI_TRANS_MODE_WR_SYNC;
	spi_ptr->TRANSCTRL.TRANSMODE = (uint8_t)SPI0_config.spi_master_config.transConfig.TransMode;
	spi_hal_send_data_buffer_init(spi_instance, send_data_buffer);
	spi_hal_recv_data_buffer_init(spi_instance, recv_data_buffer);

	
	
	//Waiting for the last transfer to complete
	while(spi_drv_trans_status(spi_ptr) == 1);		

	//just open tx_fifo_th_interrupt and rx_fifo_th_interrupt
	spi_ptr->CMD.CMD = 0x04;
	spi_drv_interrupt_en(spi_ptr, INT_RX_FIFO_TH | INT_TX_FIFO_TH | INT_TRANS_END, true);
	//spi_drv_interrupt_en(spi_ptr,INT_TX_FIFO_TH | INT_TRANS_END, true);
	
	//spi_drv_interrupt_en(spi_ptr, INT_TX_FIFO_TH, true);
}

/**
 *@brief Send data in send mode
 *@param[in] spi_instance, spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] send_data_buffer: Send Data Buffer
 *@return    none
 */
void spi_hal_master_send_data(spi_instance_e spi_instance, transmit_data *send_data_buffer)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	SPI0_config.spi_master_config.transConfig.TransMode = SPI_TRANS_MODE_WR_ONLY;
	spi_ptr->TRANSCTRL.TRANSMODE = (uint8_t)SPI0_config.spi_master_config.transConfig.TransMode;
	spi_hal_send_data_buffer_init(spi_instance, send_data_buffer);

	//Waiting for the last transfer to complete
	while(spi_drv_trans_status(spi_ptr) == 1);

	//just open tx_fifo_th_interrupt and rx_fifo_th_interrupt
	spi_ptr->CMD.CMD = 0x02;

	spi_drv_interrupt_en(spi_ptr, INT_TX_FIFO_TH | INT_TRANS_END, true);
}



/**
 *@brief  receive data in receive mode
 *@param[in] spi_instance, spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] recv_data_buffer: Recv Data Buffer
 *@return    none
 */
void spi_hal_master_recv_data(spi_instance_e spi_instance, transmit_data *recv_data_buffer)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);

	SPI0_config.spi_master_config.transConfig.TransMode = SPI_TRANS_MODE_RD_ONLY;
	spi_ptr->TRANSCTRL.TRANSMODE = (uint8_t)SPI0_config.spi_master_config.transConfig.TransMode;
	
	spi_hal_recv_data_buffer_init(spi_instance, recv_data_buffer);

	//Waiting for the last transfer to complete
	while(spi_drv_trans_status(spi_ptr) == 1);


	spi_ptr->CMD.CMD = 0x03;
	spi_drv_interrupt_en(spi_ptr, INT_RX_FIFO_TH | INT_TRANS_END, true);
	//spi_drv_interrupt_en(spi_ptr, INT_TRANS_END, true);

}

/**
 *@brief SPI master init
 *@param[in] spi_instance, spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@return    none
 */
void spi_hal_master_init(spi_instance_e spi_instance)
{
	uint32_t spi_clk = HIRC_FREQ;
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	const module_name_e clock_module_name[SPI_INSTANCE_MAX_COUNT] = {MODULE_SPI0, MODULE_SPI1};
	(void)clock_freq_get(clock_module_name[spi_instance], &spi_clk);
	if(spi_instance == SPI_INSTANCE0){
		spi_drv_master_init(spi_ptr, (SPI0_config.spi_master_config), spi_clk);
	} else {
		spi_drv_master_init(spi_ptr, (SPI1_config.spi_master_config), spi_clk);
	}
	
	spi_drv_interrupt_en(spi_ptr, INT_TRANS_END, true);		//close all interrupt
}

/**
 *@brief enable or disable cmd
 *@param[in] spi_instance, spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] en, 0:disable  --  1: enable
 *@param[in] cmd, if enable, should input cmd
 *@return    none
 */
void spi_hal_master_change_cmd(spi_instance_e spi_instance, const uint8_t en, const uint8_t cmd)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_ptr->TRANSCTRL.CMDEN = en;
	spi_ptr->CMD.CMD = cmd;
}

/**
 *@brief enable or disable addr
 *@param[in] spi_instance, spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] en, 0:disable  --  1: enable
 *@param[in] addr_len, if enable, should input addr length
 *@param[in] addr, if enable, should input addr
 *@return    none
 */
void spi_hal_master_change_addr(spi_instance_e spi_instance, const uint8_t en, const uint8_t addr_len, const uint32_t addr)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_ptr->TRANSCTRL.ADDREN = 0;
	spi_ptr->TRANSFMT.ADDRLEN = addr_len;
	spi_ptr->ADDR.ADDR = addr;
	spi_ptr->TRANSCTRL.ADDREN = en;
}

#endif


/**************************************************************** slave ************************************************************/
#if 1

/**
 *@brief Replace the data sent by the slave
 *@param[in] spi_instance, spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] send_data_buffer: New data sent from the slave device
 *@return    none
 */
void spi_hal_slave_send_data(spi_instance_e spi_instance, transmit_data *send_data_buffer)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	for(int i; i < send_data_buffer->data_len; i++){
		slave_data_send_buffer[i] = send_data_buffer->data[i];
	}
	slave_send_data_p = slave_data_send_buffer;
	slave_send_num = 0;
	//将原先的TX FIFO里的数据清空，重新预填充新数据(在中断里填充)
	spi_ptr->CTRL.TXFIFORST = 1;
}


/**
 *@brief Slave Batch send data
 *@param[in] spi_instance, spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] data_len:send data length
 *@param[in] data:send data
 *@return    none
 */
void spi_hal_slave_send_many_data(spi_instance_e spi_instance, uint8_t data_len, uint32_t data)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	for(int i = 0; i < data_len; i++){
		if((i % 2) == 0)
			slave_data_send_buffer[i] = data;
		else
			slave_data_send_buffer[i] = data + 1;
	}
	slave_send_data_p = slave_data_send_buffer;
	slave_send_num = 0;
	//将原先的TX FIFO里的数据清空，重新预填充新数据(在中断里填充)
	spi_ptr->CTRL.TXFIFORST = 1;
}

/**
 *@brief SPI slave init
 *@param[in] spi_instance, spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@return    none
 */
void spi_hal_slave_init(spi_instance_e spi_instance)
{

	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);

	//slave init
	if(spi_instance == SPI_INSTANCE0){
		spi_drv_slave_init(spi_ptr, (SPI0_config.spi_slave_config));
	} else {
		spi_drv_slave_init(spi_ptr, (SPI1_config.spi_slave_config));
	}
	memset(&slave_data_recv_buffer, 0, sizeof(slave_data_recv_buffer));
	slave_recv_data_p = slave_data_recv_buffer;
	slave_send_data_p = slave_data_send_buffer;
	//open interrupt : rx fifo th  /  tx fifo th  /  slave cmd  /  tansmit end  
	//spi_drv_interrupt_en(spi_ptr, INT_RX_FIFO_TH | INT_SLV_CMD | INT_TRANS_END | INT_TX_FIFO_TH | INT_TX_FIFO_UR | INT_RX_FIFO_OR, true);
	spi_drv_interrupt_en(spi_ptr, INT_SLV_CMD | INT_RX_FIFO_TH | INT_TX_FIFO_TH, true);
	delay_ms(100);
	slave_recv_num = 0;
	slave_send_num = 0;
}

#endif




/**
 *@brief the handle function of SPI for master and slave.
 *@param[in] spi_instance, spi instance number, must be less than SPI_INSTANCE_MAX_COUNT
 *@param[in] mode, transmode
 *@param[in] dummy_cnt, if having dummy, need to set dummy length
 *@return    none
 */
void spi_hal_transmode_change(spi_instance_e spi_instance, uint8_t mode, uint8_t dummy_cnt)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	spi_ptr->TRANSCTRL.TRANSMODE = mode;
	spi_ptr->TRANSCTRL.DUMMYCNT = dummy_cnt;
}

void spi_hal_slave_reset(spi_instance_e spi_instance)
{
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(spi_instance);
	slave_send_data_p = slave_data_send_buffer;
	slave_recv_data_p = slave_data_recv_buffer;
	spi_ptr->CTRL.TXFIFORST = 1;
    while(spi_ptr->CTRL.TXFIFORST);
    spi_ptr->CTRL.RXFIFORST = 1;
    while(spi_ptr->CTRL.RXFIFORST);
}

/****************************************************************************
 ** \brief	SPI0_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 4
****************************************************************************/
uint32_t tx_under_num = 0;
uint32_t rx_over_num = 0;
void SPI0_IRQHandler(void)
{
	
	SPI_TypeDef *spi_ptr = spi_instance_addr_get(SPI_INSTANCE0);
    if (0U == spi_ptr->TRANSFMT.SLVMOD)
    {
        /* Master mode.*/
    	if(spi_drv_interrupt_flag_rxfifo_th(spi_ptr) == 1){		//rx_fifo_th_interrupt

			if(recv_data_index.data_index == recv_data_index.data_len){
				spi_drv_interrupt_en(spi_ptr, INT_RX_FIFO_TH, false);
			} else {
				*recv_data_index.data_current = spi_drv_recv_data(spi_ptr);
				recv_data_index.data_current++;
				recv_data_index.data_index++;
			}

			spi_drv_interrupt_flag_clean(spi_ptr, INT_RX_FIFO_TH);	//need to clean flag
		}

		if(spi_drv_interrupt_flag_txfifo_th(spi_ptr) == 1){
			
			if(send_data_index.data_index == (send_data_index.data_len)){
				spi_drv_interrupt_en(spi_ptr, INT_TX_FIFO_TH, false);
			} else {
				spi_drv_send_data(spi_ptr, *send_data_index.data_current);
				send_data_index.data_current++;
				send_data_index.data_index++;
			}

			spi_drv_interrupt_flag_clean(spi_ptr, INT_TX_FIFO_TH);	//need to clean flag
		}
		if(spi_drv_interrupt_flag_trans_end(spi_ptr) == 1){
			spi_drv_interrupt_flag_clean(spi_ptr, INT_TRANS_END);
		}
    }
    else		/* Slave mode.*/
    {
		if(spi_drv_interrupt_flag_txfifo_th(spi_ptr) == 1){
			/*
        	spi_drv_send_data(spi_ptr, *slave_send_data_p);
			slave_send_data_p++;
			*/
			spi_drv_send_data(spi_ptr, slave_data_send_buffer[slave_send_num++]);
			spi_drv_interrupt_flag_clean(spi_ptr, INT_TX_FIFO_TH);	//need to clean flag
		}

      	if(spi_drv_interrupt_flag_rxfifo_th(spi_ptr) == 1){		//rx_fifo_th_interrupt
			/*
        	*slave_recv_data_p = spi_drv_recv_data(spi_ptr);
			slave_recv_data_p++;
			*/
			slave_data_recv_buffer[slave_recv_num++] = spi_drv_recv_data(spi_ptr);
			spi_drv_interrupt_flag_clean(spi_ptr, INT_RX_FIFO_TH);	//need to clean flag
		}

		if(spi_drv_interrupt_flag_slave_cmd(spi_ptr) == 1){
			spi_drv_interrupt_flag_clean(spi_ptr, INT_SLV_CMD);
		
		}
		
		/*

		if(spi_drv_interrupt_flag_trans_end(spi_ptr) == 1){
			spi_drv_interrupt_flag_clean(spi_ptr, INT_TRANS_END);
		
		}
		if(spi_drv_interrupt_flag_slave_cmd(spi_ptr) == 1){			// slave mode recv cmd
			spi_drv_interrupt_flag_clean(spi_ptr, INT_SLV_CMD);
		}
		if(spi_drv_interrupt_flag_txfifo_ur(spi_ptr) == 1){
			tx_under_num++;
			spi_drv_interrupt_flag_clean(spi_ptr, INT_TX_FIFO_UR);
		}
		if(spi_drv_interrupt_flag_rxfifo_or(spi_ptr) == 1){
			rx_over_num++;
			spi_drv_interrupt_flag_clean(spi_ptr, INT_RX_FIFO_OR);
		}
		*/
    }
	/*
     if (NULL == g_spi_int_handle[SPI_INSTANCE0])
	 {
	 	spi_irq_handler(SPI_INSTANCE0);
	 }
	 else
	 {
	 	g_spi_int_handle[SPI_INSTANCE0](SPI_INSTANCE0);
	 }
	*/
	
}

/****************************************************************************
 ** \brief	SPI1_IRQHandler
 **
 ** \param [in]  none   
 ** \return none
 ** \note 	number 5
****************************************************************************/
uint16_t spi1_interrupt_cnt = 0;
void SPI1_IRQHandler(void)
{

	SPI_TypeDef *spi_ptr = spi_instance_addr_get(SPI_INSTANCE1);
    if (0U == spi_ptr->TRANSFMT.SLVMOD)
    {
        /* Master mode.*/
    	if(spi_drv_interrupt_flag_rxfifo_th(spi_ptr) == 1){		//rx_fifo_th_interrupt

			if(recv_data_index.data_index == recv_data_index.data_len){
				spi_drv_interrupt_en(spi_ptr, INT_RX_FIFO_TH, false);
			} else {
				*recv_data_index.data_current = spi_drv_recv_data(spi_ptr);
				recv_data_index.data_current++;
				recv_data_index.data_index++;
			}

			spi_drv_interrupt_flag_clean(spi_ptr, INT_RX_FIFO_TH);	//need to clean flag
		}

		if(spi_drv_interrupt_flag_txfifo_th(spi_ptr) == 1){
			
			if(send_data_index.data_index == (send_data_index.data_len)){
				spi_drv_interrupt_en(spi_ptr, INT_TX_FIFO_TH, false);
			} else {
				spi_drv_send_data(spi_ptr, *send_data_index.data_current);
				send_data_index.data_current++;
				send_data_index.data_index++;
			}

			spi_drv_interrupt_flag_clean(spi_ptr, INT_TX_FIFO_TH);	//need to clean flag
		}
		if(spi_drv_interrupt_flag_trans_end(spi_ptr) == 1){
			spi_drv_interrupt_flag_clean(spi_ptr, INT_TRANS_END);
		}
    }
    else		/* Slave mode.*/
    {
		if(spi_drv_interrupt_flag_txfifo_th(spi_ptr) == 1){
			/*
        	spi_drv_send_data(spi_ptr, *slave_send_data_p);
			slave_send_data_p++;
			*/
			spi_drv_send_data(spi_ptr, slave_data_send_buffer[slave_send_num++]);
			spi_drv_interrupt_flag_clean(spi_ptr, INT_TX_FIFO_TH);	//need to clean flag
		}

      	if(spi_drv_interrupt_flag_rxfifo_th(spi_ptr) == 1){		//rx_fifo_th_interrupt
			/*
        	*slave_recv_data_p = spi_drv_recv_data(spi_ptr);
			slave_recv_data_p++;
			*/
			slave_data_recv_buffer[slave_recv_num++] = spi_drv_recv_data(spi_ptr);
			spi_drv_interrupt_flag_clean(spi_ptr, INT_RX_FIFO_TH);	//need to clean flag
		}

		if(spi_drv_interrupt_flag_slave_cmd(spi_ptr) == 1){
			spi_drv_interrupt_flag_clean(spi_ptr, INT_SLV_CMD);
		
		}
		
		/*

		if(spi_drv_interrupt_flag_trans_end(spi_ptr) == 1){
			spi_drv_interrupt_flag_clean(spi_ptr, INT_TRANS_END);
		
		}
		if(spi_drv_interrupt_flag_slave_cmd(spi_ptr) == 1){			// slave mode recv cmd
			spi_drv_interrupt_flag_clean(spi_ptr, INT_SLV_CMD);
		}
		if(spi_drv_interrupt_flag_txfifo_ur(spi_ptr) == 1){
			tx_under_num++;
			spi_drv_interrupt_flag_clean(spi_ptr, INT_TX_FIFO_UR);
		}
		if(spi_drv_interrupt_flag_rxfifo_or(spi_ptr) == 1){
			rx_over_num++;
			spi_drv_interrupt_flag_clean(spi_ptr, INT_RX_FIFO_OR);
		}
		*/
    }
	/*
     if (NULL == g_spi_int_handle[SPI_INSTANCE0])
	 {
	 	spi_irq_handler(SPI_INSTANCE0);
	 }
	 else
	 {
	 	g_spi_int_handle[SPI_INSTANCE0](SPI_INSTANCE0);
	 }
	*/
    //spi1_interrupt_cnt++;
    
    // if (NULL == g_spi_int_handle[SPI_INSTANCE1])
	// {
	// 	spi_irq_handler(SPI_INSTANCE1);
	// }
	// else
	// {
	// 	g_spi_int_handle[SPI_INSTANCE1](SPI_INSTANCE1);
	// }
	
}

