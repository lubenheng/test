#include "platform_common.h"

#include "uart_reg.h"
//#include "device_resource.h"

#include "uart_hal.h"
#include "uart.h"




/*******************************************************************************
 * Variables
 ******************************************************************************/
/** Table of base addresses for UART instances. */
static UART_TypeDef * const g_uart_bases[UART_INSTANCE_MAX_COUNT] = UART_BASE_ADDRS;
/** Table to save UART IRQ enum numbers defined in <device_name>.h file. */
static const IRQn_Type g_uart_irqn[UART_INSTANCE_MAX_COUNT] = UART_IRQS;
/** Table of runtime state structure for UART instances.*/
static uart_state_t g_uart_state[UART_INSTANCE_MAX_COUNT];
/** Table to save UART clock names */
static const module_name_e g_uart_clk_names[UART_INSTANCE_MAX_COUNT] = UART_CLOCK_NAMES;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/** This function start a transmit by beginning the process of sending data and enabling the interrupt. */ 
static status_t uart_send_data_using_interrupt(uart_instance_e instance,
                                                 const uint8_t *tx_buff,
                                                 uint32_t tx_size);
/** This function finish up a transmit by completing the process of sending data and disabling the interrupt. */ 
static void uart_complete_send_data_using_interrupt(uart_instance_e instance);
/** This function start a receive by beginning the process of receiving data and enabling the interrupt */
static status_t uart_receive_data_using_interrupt(uart_instance_e instance,
                                                    uint8_t *rx_buff,
                                                    uint32_t rx_size);
/** This function finish up a receive by completing the process of receiving data and disabling the interrupt */
static void uart_complete_receive_data_using_interrupt(uart_instance_e instance);
/** This function write data to the data register */
static void uart_put_data(uart_instance_e instance);
/** This function read data from the buffer register */
static void uart_get_data(uart_instance_e instance);
/** This function is rx_full interrupt handler */
static void uart_rx_full_irq_handler(uart_instance_e instance);
/** This function is tx_empty interrupt handler */
static void uart_tx_empty_irq_handler(uart_instance_e instance);
/** This function is tx_complete interrupt handler */
static void uart_tx_complete_irq_handler(uart_instance_e instance);
/** This function is error interrupt handler */
//static void uart_error_irq_handler(uart_instance_e instance);


/*******************************************************************************
* Public UART Control Functions
******************************************************************************/ 
/**
 *@brief initializes the uart configuration structure with default values
 *@param[in] *uart_user_config: pointer to user configuration structure
 *@return    none
 */
void uart_default_config_get(uart_user_config_t *uart_user_config)
{
    DEV_ASSERT(uart_user_config != NULL);

    uart_user_config->baud_rate = 9600U;
    uart_user_config->parity_mode = UART_PARITY_DISABLED;
    uart_user_config->stop_bit_number = UART_ONE_STOP_BIT;
    uart_user_config->bit_number_per_char = UART_8_BITS_PER_CHAR;
	uart_user_config->tx_interrupt_priority = 0U;
	uart_user_config->rx_interrupt_priority = 0U;
}

/**
 *@brief initialize an uart operation instance
 *@param[in] instance: uart instance number
 *@param[in] *uart_user_config: pointer to user configuration structure
 *@return    uart init execute result status:
 *				- STATUS_SUCCESS, initialize the uart successfully
 */
status_t uart_init(uart_instance_e instance, const uart_user_config_t *uart_user_config)
{
    DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
    DEV_ASSERT(uart_user_config != NULL);

    UART_TypeDef *base = g_uart_bases[instance];
    uint32_t uart_source_clock;
	
	module_name_e instance_clk_name = g_uart_clk_names[instance];
	(void)clock_freq_get(instance_clk_name, &uart_source_clock);

    /* Check if current instance is clock gated off. */
    DEV_ASSERT(uart_source_clock > 0U);
    /* For 10 bits per char, parity bit cannot be enabled */
    DEV_ASSERT((uart_user_config->bit_number_per_char != UART_10_BITS_PER_CHAR) ||
               (uart_user_config->parity_mode == UART_PARITY_DISABLED));

	uart_state_t *uart_state_ptr = &(g_uart_state[instance]);
	
    /* Save the transfer information for runtime retrieval */
    uart_state_ptr->bit_number_per_char = uart_user_config->bit_number_per_char;
	uart_state_ptr->autobaud_en = uart_user_config->autobaud_en;

    /* initialize the UART instance */
    //uart_drv_soft_reset(base);

    /* initialize the parameters of the UART config structure with desired data */
    (void)uart_baudrate_set(instance, uart_user_config->baud_rate);

    if (uart_user_config->parity_mode != UART_PARITY_DISABLED)
    {
        uart_drv_bits_number_perchar_set(base, uart_user_config->bit_number_per_char, true);
    }
    else
    {
        uart_drv_bits_number_perchar_set(base, uart_user_config->bit_number_per_char, false);
    }

    uart_drv_parity_mode_set(base, uart_user_config->parity_mode);
    uart_drv_stopbit_number_set(base, uart_user_config->stop_bit_number);
	uart_drv_rx_data_polarity_set(base, uart_user_config->rx_bit_parity);
	uart_drv_tx_data_polarity_set(base, uart_user_config->tx_bit_parity);
	uart_drv_bit_order_set(base, uart_user_config->bit_order);
	//uart_drv_autobaud_enable(base, uart_user_config->autobaud_en); // without this function
	//uart_drv_doze_enable(base, false); // UART without this function 
	
    /* initialize last driver operation status */
    uart_state_ptr->transmit_status = STATUS_SUCCESS;
    uart_state_ptr->receive_status = STATUS_SUCCESS;

	/* Enable UART interrupt */
	if (NVIC_GetEnableIRQ(g_uart_irqn[instance]) == 0U)
	{
		NVIC_EnableIRQ(g_uart_irqn[instance]);
		NVIC_SetPriority(g_uart_irqn[instance], uart_user_config->tx_interrupt_priority);
	}


    return STATUS_SUCCESS;
}

/**
 *@brief shut down the uart by disabling interrupts and transmitter/receiver
 *@param[in] instance: uart instance number
 *@return    uart de_init execute result status:
 *				- STATUS_SUCCESS, shut down the uart successfully
 */
status_t uart_de_init(uart_instance_e instance)
{
    DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);

	
    uint32_t uart_source_clock;
    const UART_TypeDef *base = g_uart_bases[instance];

	module_name_e instance_clk_name = g_uart_clk_names[instance];
	(void)clock_freq_get(instance_clk_name, &uart_source_clock);

    DEV_ASSERT(uart_source_clock > 0U);

    /* Wait until the data is completely shifted out of shift register */
    while (!uart_drv_status_flag_get(base, UART_TX_COMPLETE)) 
    {
    	;
    }
			
    /* Disable UART interrupt. */
    NVIC_DisableIRQ(g_uart_irqn[instance]);

    return STATUS_SUCCESS;
}

/**
 *@brief install callback function for the uart receive
 *@param[in] instance: uart instance number
 *@param[in] function: the uart receive callback function
 *@param[in] callback_param: the uart receive callback parameter pointer
 *@return    none
 */
void uart_rx_callback_install(uart_instance_e instance,
								uart_callback_t function,
								void *callback_param)
{
    DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);

    uart_state_t *uart_state = &(g_uart_state[instance]);

    uart_state->rx_call_back = function;
    uart_state->rx_call_back_param = callback_param;
}

/**
 *@brief install callback function for the uart transmit
 *@param[in] instance: uart instance number
 *@param[in] function: the uart transmit callback function
 *@param[in] callback_param: the uart transmit callback parameter pointer
 *@return    none
 */
void uart_tx_callback_install(uart_instance_e instance,
								uart_callback_t function,
								void *callback_param)
{
    DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);

    uart_state_t *uart_state = &(g_uart_state[instance]);

    uart_state->tx_call_back = function;
    uart_state->tx_call_back_param = callback_param;
}

/**
 *@brief transmit multiple bytes of data using polling method
 *@param[in] instance: uart instance number
 *@param[in] *tx_buff: the buffer pointer which saves the data to be sent
 *@param[in] tx_size: the size of data to be sent in unit of byte
 *@return    uart data send execute result status:
 *				- STATUS_BUSY, uart is busy and send data fail
 *				- STATUS_SUCCESS, uart send data successfully  
 */
status_t uart_send_data_polling(uart_instance_e instance,
                                const uint8_t *tx_buff,
                                uint32_t tx_size)
{
    DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
    DEV_ASSERT(tx_buff != NULL);

//    UART_TypeDef *base = g_uart_bases[instance];
    uart_state_t *uart_state = &(g_uart_state[instance]);

#if 0
    /* Check the validity of the parameters */
    DEV_ASSERT(tx_size > 0U);
    DEV_ASSERT((uart_state->bit_number_per_char <= UART_8_BITS_PER_CHAR) ||
               ((tx_size & 1U) == 0U));

	/* get the original TX status of UART */
	bool transmit_status = uart_drv_transmit_status_get(base);

    /* Check driver is not busy transmitting data from a previous asynchronous call */
    if (uart_state->is_tx_busy)
    {
        return STATUS_BUSY;
    }

    /* Enable the UART transmitter */
    uart_drv_transmit_enable(base, true);

    while (tx_size > 0U)
    {
        while (!uart_drv_status_flag_get(base, UART_TX_DATA_REG_EMPTY))
        {}

        uart_state->tx_buff = tx_buff;
        uart_put_data(instance);

        if (uart_state->bit_number_per_char <= UART_8_BITS_PER_CHAR)
        {
            ++tx_buff;
            --tx_size;
        }
        else
        {
            ++tx_buff;
            ++tx_buff;
            tx_size -= 2U;
        }
    }
	
	/* Wait for the last byte to be send */
	while (!uart_drv_status_flag_get(base, UART_TX_DATA_REG_EMPTY))
	{}
	
    /* set the original TX status of UART */
	uart_drv_transmit_enable(base, transmit_status);	
#endif
    while (tx_size > 0U)
    {
//        while (!uart_drv_status_flag_get(base, UART_TX_DATA_REG_EMPTY))
//        {}
		
		

        uart_state->tx_buff = tx_buff;
        uart_put_data(instance);

        if (uart_state->bit_number_per_char <= UART_8_BITS_PER_CHAR)
        {
            ++tx_buff;
            --tx_size;
        }
        else
        {
            ++tx_buff;
            ++tx_buff;
            tx_size -= 2U;
        }
    }

    return STATUS_SUCCESS;
}

/**
 *@brief send data out using a non-blocking method
 *@param[in] instance: uart instance number
 *@param[in] *tx_buff: the buffer pointer which saves the data to be sent.
 *@param[in] tx_size: the size of data to be sent in unit of byte
 *@return    uart data send execute result status:
 *				- STATUS_BUSY, uart is busy and send data fail
 *				- STATUS_SUCCESS, uart send data successfully
 */
status_t uart_send_data(uart_instance_e instance,
                             const uint8_t *tx_buff,
                             uint32_t tx_size)
{
    DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
    DEV_ASSERT(tx_buff != NULL);
	DEV_ASSERT(tx_size > 0U);

    status_t status = STATUS_SUCCESS;

		/* Start the transmission process using interrupts */
		status = uart_send_data_using_interrupt(instance, tx_buff, tx_size);
    
    return status;
}

/**
 *@brief return whether the previous interrupt driven transfer is completed
 *@param[in]  instance: uart instance number
 *@param[out] *bytes_remaining: the number of bytes that have not been transferred yet
 *@return     the transmit status:
 *				- STATUS_SUCCESS, uart transmit data successfully
 *				- STATUS_ERROR, an error occurred
 *				- STATUS_BUSY, the transmit is still in progress
 *				- STATUS_TIMEOUT, a timeout was reached
 *				- STATUS_UART_ABORTED, the transmit was aborted
 */
status_t uart_transmit_status_get(uart_instance_e instance, uint32_t *bytes_remaining)
{
    DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
    const uart_state_t *uart_state = &(g_uart_state[instance]);

    if (bytes_remaining != NULL)
    {
        if (uart_state->is_tx_busy)
        {
			*bytes_remaining = uart_state->tx_size;
        }
        else
        {
            *bytes_remaining = 0U;
        }
    }

    return uart_state->transmit_status;
}

/**
 *@brief terminate a non-blocking transmission early
 *@param[in] instance: uart instance number
 *@return    whether the aborting is successful or not:
 *				- STATUS_SUCCESS, the aborting is successful
 */
status_t uart_send_data_abort(uart_instance_e instance)
{
    DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);

    uart_state_t *uart_state = &(g_uart_state[instance]);

    /* Check if a transfer is running. */
    if (!uart_state->is_tx_busy)
    {
        return STATUS_SUCCESS;
    }

    /* Update the tx status */
    uart_state->transmit_status = STATUS_UART_ABORTED;

    /* Stop the running transfer. */
		uart_complete_send_data_using_interrupt(instance);
    
    return STATUS_SUCCESS;
}

/**
 *@brief receive multiple bytes of data using polling method
 *@param[in] instance: uart instance number
 *@param[in] *rx_buff: the buffer pointer which saves the data to be received
 *@param[in] rx_size: size of data need to be received in unit of byte
 *@return    uart receive data execute result status:
 *				- STATUS_SUCCESS, the uart receive data successfully
 *				- STATUS_BUSY, the uart is busy
 *				- STATUS_UART_FRAMING_ERROR, a frame error occurred
 *				- STATUS_UART_NOISE_ERROR, a noise error occurred
 *				- STATUS_UART_PARITY_ERROR, a parity error occurred
 *				- STATUS_UART_RX_OVERRUN, an overrun error occurred
 */
status_t uart_receive_data_polling(uart_instance_e instance,
                                       uint8_t *rx_buff,
                                       uint32_t rx_size)
{
    DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
    DEV_ASSERT(rx_buff != NULL);

    uint8_t tmp_byte;
    status_t ret_val = STATUS_SUCCESS;
    status_t tmp_state = STATUS_SUCCESS;
    UART_TypeDef *base = g_uart_bases[instance];
    uart_state_t *uart_state = &(g_uart_state[instance]);

    /* Check the validity of the parameters */
    DEV_ASSERT(rx_size > 0U);
    DEV_ASSERT((uart_state->bit_number_per_char <= UART_8_BITS_PER_CHAR) ||
               ((rx_size & 1U) == 0U));

    /* Check driver is not busy receiving data from a previous asynchronous call */
    if (uart_state->is_rx_busy)
    {
        return STATUS_BUSY;
    }
	
    /* Enable the UART receiver */
    uart_drv_receive_enable((UART_TypeDef *)base, true);

	/* To wait the uart autobaud success */
	if(true == uart_state->autobaud_en)
	{
		while(!uart_drv_autobaud_status_get(base))
		{;}
		uart_drv_rx_fifo_clear(base);
		(void)uart_drv_status_flag_clear(base, UART_RX_DATA_REG_FULL);
		uart_drv_error_interrupts_flag_clear(base);	
	}

    while (rx_size > 0U)
    {
        while (!uart_drv_status_flag_get(base, UART_RX_DATA_REG_FULL))
        {}

        uart_state->rx_buff = rx_buff;
        uart_get_data(instance);

        if (uart_state->bit_number_per_char <= UART_8_BITS_PER_CHAR)
        {
            ++rx_buff;
            --rx_size;
        }
        else
        {
            ++rx_buff;
            ++rx_buff;
            rx_size -= 2U;
        }

        /* Check for errors on received data */
        if (uart_drv_status_flag_get(base, UART_FRAME_ERR))
        {
            tmp_state = STATUS_UART_FRAMING_ERROR;
            /* Disable the UART receiver */
            uart_drv_receive_enable(base, false);
            /* Clear the flag */
            (void)uart_drv_status_flag_clear(base, UART_FRAME_ERR);
            break;
        }
        if (uart_drv_status_flag_get(base, UART_NOISE_DETECT))
        {
            tmp_state = STATUS_UART_NOISE_ERROR;
            /* Disable the UART receiver */
            uart_drv_receive_enable(base, false);
            /* Clear the flag */
            (void)uart_drv_status_flag_clear(base, UART_NOISE_DETECT);
            break;
        }
        if (uart_drv_status_flag_get(base, UART_PARITY_ERR))
        {
            tmp_state = STATUS_UART_PARITY_ERROR;
            /* Disable the UART receiver */
            uart_drv_receive_enable(base, false);
            /* Clear the flag */
            (void)uart_drv_status_flag_clear(base, UART_PARITY_ERR);
            break;
        }
        if (uart_drv_status_flag_get(base, UART_RX_OVERRUN))
        {
            tmp_state = STATUS_UART_RX_OVERRUN;
            /* Disable the UART receiver */
            uart_drv_receive_enable(base, false);
            /* Clear the flag */
            (void)uart_drv_status_flag_clear(base, UART_RX_OVERRUN);
            break;
        }
    }

    /* Update received status */
    if ((rx_size == 0U) && (tmp_state == STATUS_UART_RX_OVERRUN))
    {
        ret_val = STATUS_SUCCESS;
    }
    else
    {
        ret_val = tmp_state;
    }

    if (ret_val == STATUS_SUCCESS)
    {
        /* Disable the UART receiver */
        uart_drv_receive_enable((UART_TypeDef *)base, false);
    }

    /* Read dummy to clear RX_FULL_FLAG */
    uart_drv_get_char(base, &tmp_byte);

    return ret_val;
}

/* OCS_Reserved_Start */
bool uart_receive_data_polling_for_cmdline(uart_instance_e instance,
									  uint8_t *rx_buff,
									  uint32_t rx_size)
{
  
   UART_TypeDef *base = g_uart_bases[instance];
   uart_state_t *uart_state = &(g_uart_state[instance]);

   /* Enable the UART receiver */
   uart_drv_receive_enable((UART_TypeDef *)base, true);
	
	if(base->SR_STR.RXEMPT == 0U)
   //if (uart_drv_status_flag_get(base, UART_RX_DATA_REG_FULL) == true)
   {
		uart_state->rx_buff = rx_buff;
   		uart_get_data(instance);

   		return true;
   }
   else
   {
		return false;
   }
}
/* OCS_Reserved_End */


/**
 *@brief receive data using a non-blocking method
 *@param[in] instance: uart instance number
 *@param[in] *tx_buff: the buffer pointer which saves the data to be receive
 *@param[in] tx_size: the size of data to be receive in unit of byte
 *@return    whether receive data successful or not:
 *				- STATUS_SUCCESS, the uart receive data successfully
 *				- STATUS_BUSY, the uart is busy
 */
status_t uart_receive_data(uart_instance_e instance,
                                uint8_t *rx_buff,
                                uint32_t rx_size)
{
    DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
    DEV_ASSERT(rx_buff != NULL);
	DEV_ASSERT(rx_size > 0U);

    status_t status = STATUS_SUCCESS;

	/* Start the reception process using interrupts */
	status = uart_receive_data_using_interrupt(instance, rx_buff, rx_size);
    
    return status;
}

/**
 *@brief return whether the previous interrupt driven transfer is completed
 *@param[in]  instance: uart instance number
 *@param[out] *bytes_remaining: the number of bytes that have not been received yet
 *@return     the receive status:
 *				- STATUS_SUCCESS, the receive has completed successfully
 *				- STATUS_BUSY, the receive is still in progress
 *				- STATUS_UART_ABORTED, the receive was aborted
 *				- STATUS_TIMEOUT, a timeout was reached
 *				- STATUS_UART_FRAMING_ERROR, a frame error occurred
 *				- STATUS_UART_NOISE_ERROR, a noise error occurred
 *				- STATUS_UART_PARITY_ERROR, a parity error occurred
 *				- STATUS_UART_RX_OVERRUN, an overrun error occurred
 */
status_t uart_receive_status_get(uart_instance_e instance, uint32_t *bytes_remaining)
{
    DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
    const uart_state_t *uart_state = &(g_uart_state[instance]);

    if (bytes_remaining != NULL)
    {
        if (uart_state->is_rx_busy)
        {
			*bytes_remaining = uart_state->rx_size;
        }
        else
        {
            *bytes_remaining = 0U;
        }
    }

    return uart_state->receive_status;
}

/**
 *@brief terminate a non-blocking receive early
 *@param[in] instance: uart instance number
 *@return    whether the aborting is successful or not:
 *				- terminate the receive successfully
 */
status_t uart_receive_data_abort(uart_instance_e instance)
{
    DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);

    uart_state_t *uart_state = &(g_uart_state[instance]);

    /* Check if a transfer is running. */
    if (!uart_state->is_rx_busy)
    {
        return STATUS_SUCCESS;
    }
    /* Update the rx status */
    uart_state->receive_status = STATUS_UART_ABORTED;
    /* Stop the running transfer. */
		uart_complete_receive_data_using_interrupt(instance);

    return STATUS_SUCCESS;
}

/**
 *@brief configure the uart baud rate
 *@param[in] instance: uart instance number
 *@param[in] baud_rate: the baud rate desired to set
 *@return    whether set baud rate successful or not:
 *				- STATUS_SUCCESS, configure the uart baud rate successfully
 *				- STATUS_BUSY, uart is busy
 */
status_t uart_baudrate_set(uart_instance_e instance, uint32_t baud_rate)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
	
	uint32_t baud_div_integer;
	//uint32_t baud_div_fraction;
	uint32_t temp;
	uint32_t osr = 16U;
    uint32_t uart_source_clock;
	
	uint32_t osr_max = 32U;
	uint32_t osr_min = 8U;
	uint32_t baud_div_int_max = 8191U;
	uint32_t baud_div_int_min = 1U;
	uint32_t baud_div_fra_max = 31U;
	
	UART_TypeDef *base = g_uart_bases[instance];
    const uart_state_t *uart_state_ptr = &(g_uart_state[instance]);
	
	
	/* Check if there is an ongoing transfer */
	if (uart_state_ptr->is_tx_busy == true)
	{
		return STATUS_BUSY;
	}
	if (uart_state_ptr->is_rx_busy == true)
	{
		return STATUS_BUSY;
	}

	/* Get the UART clock as configured in the clock manager */

	module_name_e instance_clk_name = g_uart_clk_names[instance];
	(void)clock_freq_get(instance_clk_name, &uart_source_clock);

    /* Check if current instance is clock gated off. */
    DEV_ASSERT(uart_source_clock > 0U);
    /* Check if the desired baud rate can be configured with the current protocol clock. */
    DEV_ASSERT(uart_source_clock >= (baud_rate * osr_min * baud_div_int_min));
	DEV_ASSERT((uart_source_clock/baud_rate) <= ((osr_max * baud_div_int_max) + baud_div_fra_max));
	
	temp = (uint32_t)(uart_source_clock / baud_rate);
	if(temp < (osr * baud_div_int_min))
	{
		osr = osr_min;
	}

	if(temp > ((osr * baud_div_int_max) + osr -1U))
	{
		osr = osr_max;
	}
	
	baud_div_integer = temp / osr;
	//baud_div_fraction = temp - (baud_div_integer * osr);
	
	/* write the sbr value to the BAUD registers */
	uart_drv_oversample_ratio_set(base, (osr - 1U));
    uart_drv_baudrate_divisor_integer_set(base, baud_div_integer);
	//uart_drv_baudrate_divisor_fraction_set(base, baud_div_fraction);
	
	return STATUS_SUCCESS;
}

void uart_hal_status_rxbusy_set(uart_instance_e instance, bool rx_busy)
{
    uart_state_t *uart_state_ptr = &(g_uart_state[instance]);
    uart_state_ptr->is_rx_busy = rx_busy;
}

/**
 *@brief return the uart baud rate
 *@param[in]  instance: uart instance number
 *@param[out] *baud_rate: the uart configured baud rate
 *@return     none
 */
void uart_baudrate_get(uart_instance_e instance, uint32_t *baud_rate)
{
    DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
	DEV_ASSERT(baud_rate != NULL);

    uint32_t osr;
    uint32_t baud_div_integer;
	uint32_t baud_div_fraction;
    uint32_t uart_source_clock;
    const UART_TypeDef * base = g_uart_bases[instance];

    /* Get the UART clock as configured in the clock manager */
	module_name_e instance_clk_name = g_uart_clk_names[instance];
	(void)clock_freq_get(instance_clk_name, &uart_source_clock);

    osr = (uint32_t)uart_drv_oversample_ratio_get(base);
    baud_div_integer = (uint32_t)uart_drv_baudrate_divisor_integer_get(base);
	baud_div_fraction = (uint32_t)uart_drv_baudrate_divisor_fraction_get(base);

    *baud_rate = (uart_source_clock / (((osr + 1U) * baud_div_integer) + baud_div_fraction));
}

/**
 *@brief provide the driver with a new tx buffer for continuous transmission
 *@param[in] instance: uart instance number
 *@param[in] *tx_buff: the buffer pointer which saves the data to be sent
 *@param[in] tx_size: the size of data to be sent in unit of byte
 *@return    whether set the tx buffer successfully or not:
 *				- STATUS_SUCCESS, set the tx buffer successfully
 */
status_t uart_tx_buffer_set(uart_instance_e instance,
							const uint8_t *tx_buff,
							uint32_t tx_size)
{
    DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
    DEV_ASSERT(tx_buff != NULL);
    DEV_ASSERT(tx_size > 0U);

    uart_state_t *uart_state = &(g_uart_state[instance]);
    uart_state->tx_buff = tx_buff;
    uart_state->tx_size = tx_size;

    return STATUS_SUCCESS;
}

/**
 *@brief provide the driver with a new rx buffer for continuous reception
 *@param[in] instance: uart instance number
 *@param[in] *rx_buff: the buffer pointer which saves the data to be receive
 *@param[in] rx_size: the size of data to be receive in unit of byte
 *@return    whether set the rx buffer successfully or not:
 *				- STATUS_SUCCESS, set the rx buffer successfully
 */
status_t uart_rx_buffer_set(uart_instance_e instance,
                                uint8_t *rx_buff,
                                uint32_t rx_size)
{
    DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
    DEV_ASSERT(rx_buff != NULL);
    DEV_ASSERT(rx_size > 0U);

    uart_state_t *uart_state = &(g_uart_state[instance]);
    uart_state->rx_buff = rx_buff;
    uart_state->rx_size = rx_size;

    return STATUS_SUCCESS;
}

/**
 *@brief interrupt handler of uart
 *@param[in] instance: uart instance number
 *@return    none
 */
uart_state_t uart_state_test;
volatile uint32_t uart_interrupt_flag = 0;
volatile uint32_t uart_interrupt_flag1 = 0;
void uart_irq_handler(uart_instance_e instance)
{
    DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
 
    UART_TypeDef *base = g_uart_bases[instance];
	const uart_state_t *uart_state = &(g_uart_state[instance]);
    uart_state_test = g_uart_state[instance];

    //uint32_t uart_interrupt_flag = base->IR_STR.w;
	uart_interrupt_flag = base->IR_STR.w;
	
	uart_interrupt_flag1 = REG32(0x40004008);

    /* 清除所有中断标志位（需要手动写1清除） */
    base->IR_STR.w = 0xFFFFFFFFUL;


	/*Handle rx_overrun/frame_error/parity_error/noise_error interrupt */
    //uart_error_irq_handler(instance);

    /* Handle receive data full interrupt */
    //if (uart_drv_interrupt_source_status_get(base, UART_INT_RX_DATA_REG_FULL))
    if( ((uart_interrupt_flag & UART_RXOVF_INTP_FLAG_MASK)>>UART_RXOVF_INTP_FLAG_POS) == 1U )
    {
        //if (uart_drv_status_flag_get(base, UART_RX_DATA_REG_FULL))
        if(base->SR_STR.RXFULL == 1U)
        {
        	if (uart_state->autobaud_en == true) 
        	{
        		if (false == uart_drv_autobaud_status_get(base))
        		{				
					uart_drv_rx_fifo_clear(base);
					uart_drv_error_interrupts_flag_clear(base);
					return;
				}
				else
				{	
					uart_drv_error_interrupts_flag_clear(base);
					uart_drv_error_interrupts_source_enable(base, true);
				}
			}
				
            uart_rx_full_irq_handler(instance);
        }
    }

    /* Handle transmitter data register empty interrupt */
    if (uart_drv_interrupt_source_status_get(base, UART_INT_TX_DATA_REG_EMPTY))
    {
        if (uart_drv_status_flag_get(base, UART_TX_DATA_REG_EMPTY))
        {
            uart_tx_empty_irq_handler(instance);
        }
    }

    /* Handle transmission complete interrupt */
    if (uart_drv_interrupt_source_status_get(base, UART_INT_TX_COMPLETE))
    {
        if (uart_drv_status_flag_get(base, UART_TX_COMPLETE))
        {
            uart_tx_complete_irq_handler(instance);
        }
    }
}

/**
 *@brief specify interrupt handler for a uart instance
 *@param[in] instance: uart instance number
 *@param[in] handle: interrupt handler
 *@return    none
 */
void uart_interrupt_handle_cfg(uart_instance_e instance, UART_INT_HANDLE handle)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
	
	uart_drv_interrupt_handle_cfg(instance, handle);
}

/**
 *@brief clear the autobaud result
 *@param[in] instance: uart instance number
 *@return    none
 */
void uart_autobaud_result_clear(uart_instance_e instance)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
	
	UART_TypeDef *base = g_uart_bases[instance];
	uart_drv_autobaud_result_clear(base);
}

/**
 *@brief make uart enter the standby mode
 *@param[in] instance: uart instance number
 *@param[in] standby_en: whether enter the standby mode or not
 *@return    none
 */
void uart_standby_mode_enter(uart_instance_e instance, bool standby_en)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
	
	UART_TypeDef *base = g_uart_bases[instance];
	uart_drv_standby_mode_enter(base, standby_en);
}

/**
 *@brief enable address matching mode for match address1 and set match address1 
 *@param[in] instance: uart instance number
 *@param[in] match_en: whether enable address matching mode for match address1 or not
 *@param[in] match_addr: match address1
 *@return    none 
 */
void uart_match1_set(uart_instance_e instance, bool match_en, uint32_t match_addr)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
	
	UART_TypeDef *base = g_uart_bases[instance];
	uart_drv_match1_set(base, match_en, match_addr);
}

/**
 *@brief enable address matching mode for match address2 and set match address2 
 *@param[in] instance: uart instance number
 *@param[in] match_en: whether enable address matching mode for match address2 or not
 *@param[in] match_addr: match address2
 *@return    none 
 */
void uart_match2_set(uart_instance_e instance, bool match_en, uint32_t match_addr)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
	
	UART_TypeDef *base = g_uart_bases[instance];
	uart_drv_match2_set(base, match_en, match_addr);
}

/**
 *@brief send a break
 *@param[in] instance: uart instance number
 *@param[in] break_length: UART break character length setting
 *				- UART_BREAK_CHAR_10_BIT_MINIMUM, break length between 9~13 bits
 *				- UART_BREAK_CHAR_13_BIT_MINIMUM, break length between 12~15 bits
 *@return    none
 */
void uart_break_send(uart_instance_e instance, uart_break_char_length_e break_length)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
	
	UART_TypeDef *base = g_uart_bases[instance];
	uart_transmit_enable(instance, true);
	uart_drv_set_break_char_transmit_length(base, break_length);
	uart_drv_break_send(base);
}

/**
 *@brief enable or disable the uart break detect
 *@param[in] instance: uart instance number
 *@param[in] brk_det_en: enable the uart break detect or not
 *@return    none
 */
void uart_break_detect_enable(uart_instance_e instance, bool brk_det_en)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
	
	UART_TypeDef *base = g_uart_bases[instance];
	uart_drv_break_detect_enable(base, brk_det_en);
	uart_receive_enable(instance, brk_det_en);
}

/**
 *@brief configure the uart work as loop mode or single_wire mode
 *@param[in] instance: uart instance number
 *@param[in] work_mode: work mode
 *				- UART_NORMAL_MODE, normal mode
 *				- UART_SINGLE_WIRE_MODE, single-wire mode
 *				- UART_LOOP_MODE, loop back mode 
 *@param[in] tx_dir: the direction of TXD in single_wire mode
 *				- UART_TXD_AS_INPUT, the TXD pin is input
 *				- UART_TXD_AS_OUTPUT, the TXD pin is output
 *@return    none
 */
void uart_work_mode_cfg(uart_instance_e instance, uart_work_mode_e work_mode, uart_tx_direction tx_dir)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
	
	UART_TypeDef *base = g_uart_bases[instance];
	
	if(work_mode == UART_SINGLE_WIRE_MODE)
	{
		uart_drv_loop_singlewire_mode_set(base, true);
		uart_drv_rx_source_select(base, true);
		uart_drv_tx_direction_set(base, tx_dir);
	}
	else if (work_mode == UART_LOOP_MODE)
	{
		uart_drv_loop_singlewire_mode_set(base, true);
		uart_drv_rx_source_select(base, false);
	}
	else
	{
		uart_drv_loop_singlewire_mode_set(base, false);
	}
}

 /**
 *@brief enable or disable the uart transmitter
 *@param[in] instance: uart instance number
 *@param[in] tx_en: enable or disable transmitter
 *@return    none
 */
void uart_transmit_enable(uart_instance_e instance, bool tx_en)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
	
	UART_TypeDef *base = g_uart_bases[instance];
	uart_drv_transmit_enable(base, tx_en);
}



/**
 *@brief enable or disable the uart receiver
 *@param[in] instance: uart instance number
 *@param[in] rx_en: enable or disable receiver
 *@return    none
 */
void uart_receive_enable(uart_instance_e instance, bool rx_en)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
	
	UART_TypeDef * base = g_uart_bases[instance];
	uart_drv_receive_enable(base, rx_en);
}

/**
 *@brief configure the TX_CTS
 *@param[in] instance: uart instance number
 *@param[in] tx_cts_en: enable or disable TX_CTS
 *@param[in] tx_cts_src: the source of TX_CTS
 *				- UART_CTS_N, TX_CTS is the inverted UART_CTS_N pin
 *				- UART_RX_MATCH, TX_CTS is the inverted rx match result
 *				- UART_TRIG_INPUT, TX_CTS is the trig_input signal
 *@param[in] cts_sample_time: the sample time of TX_CTS
 *				- UART_START_OF_CHAR, TX_CTS is sampled at the start of each character
 *				- UART_BUS_IDLE, TX_CTS is sampled when transmitter is idle 
 *@return    none
 */
void uart_tx_cts_set(uart_instance_e instance, bool tx_cts_en, uart_tx_cts_source_e tx_cts_src, uart_tx_cts_sample_time_e cts_sample_time)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);

	UART_TypeDef *base = g_uart_bases[instance];
	
	if (false == tx_cts_en)
	{
		uart_drv_tx_cts_en(base, false);
	}
	else
	{
		uart_drv_tx_cts_en(base, true);

		if (UART_CTS_N == tx_cts_src)
		{
			uart_drv_tx_cts_src_set(base, tx_cts_src);
			uart_drv_trig_input_set(base, false);
			uart_drv_tx_cts_cfg(base, cts_sample_time);
		}
		else if (UART_RX_MATCH == tx_cts_src)
		{
			uart_drv_tx_cts_src_set(base, tx_cts_src);
			uart_drv_trig_input_set(base, false);
			uart_drv_tx_cts_cfg(base, cts_sample_time);
		}
		else
		{
			uart_drv_trig_input_set(base, true);
			uart_drv_tx_cts_cfg(base, cts_sample_time);
		}	
	}
}

/**
 *@brief configure the RX_RTS
 *@param[in] instance: uart instance number
 *@param[in] rx_rts_en: enable or disable the RX_RTS
 *@param[in] rx_rts_water_mark: the watermark of RX_RTS
 *@return    none
 */
void uart_rx_rts_set(uart_instance_e instance, bool rx_rts_en, uint8_t rx_rts_water_mark)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
	DEV_ASSERT(rx_rts_water_mark < 4U);

	UART_TypeDef *base = g_uart_bases[instance];

	if (false == rx_rts_en)
	{
		uart_drv_rx_rts_en(base, false);
	}
	else
	{
		uart_drv_rx_rts_en(base, true);
		uart_drv_rx_rts_water_set(base, rx_rts_water_mark);
	}
}

/**
 *@brief configure the TX_RTS
 *@param[in] instance: uart instance number
 *@param[in] tx_rts_en: enable or disable the TX_RTS
 *@param[in] tx_rts_pol: the polarity of TX_RTS
 *				- UART_HIGH_LEVEL_ACTIVE, RTS_N is active when tx rts is high level
 *				- UART_LOW_LEVEL_ACTIVE, RTS_N is active when tx rts is low level 
 *@return    none
 */
void uart_tx_rts_set(uart_instance_e instance, bool tx_rts_en, uart_tx_rts_polarity_e tx_rts_pol)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);

	UART_TypeDef *base = g_uart_bases[instance];

	if (false == tx_rts_en)
	{
		uart_drv_tx_rts_en(base, false);
	}
	else
	{
		/* disable rx_rts before enable tx_rts */
		uart_drv_rx_rts_en(base, false);
		uart_drv_tx_rts_en(base, true);
		uart_drv_tx_rts_polarity_set(base, tx_rts_pol);
	}
}

/**
 *@brief enable or disable various interrupt sources of uart module 
 *@param[in] instance: uart instance number
 *@param[in] int_src: interrupt source
 *				- UART_INT_LIN_BREAK_DETECT,
 *				- UART_INT_RX_ACTIVE_EDGE,
 *				- UART_INT_TX_DATA_REG_EMPTY,
 *				- UART_INT_TX_COMPLETE,
 *				- UART_INT_RX_DATA_REG_FULL,
 *				- UART_INT_IDLE_LINE,
 *				- UART_INT_RX_OVERRUN,
 *				- UART_INT_NOISE_ERR_FLAG,
 *				- UART_INT_FRAME_ERR_FLAG,
 *				- UART_INT_PARITY_ERR_FLAG,
 *				- UART_INT_MATCH_ADDR_ONE,
 *				- UART_INT_MATCH_ADDR_TWO,
 *				- UART_INT_FIFO_TXOF,
 *				- UART_INT_FIFO_RXUF,
 *				- UART_INT_AUTO_BAUD_SUCC,
 *@param[in] int_en: enable or disable interrupt source
 *@return    none. 
 */
void uart_interrupt_source_enable(uart_instance_e instance, uart_interrupt_e int_src, bool interrupt_enable)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
	
	UART_TypeDef *base = g_uart_bases[instance];
	
	uart_drv_interrupt_source_enable(base, int_src, interrupt_enable);
}

/**
 *@brief enable or disable infrared modulation and demodulation
 *@param[in] instance: uart instance number
 *@param[in] irda_en: enable or disable infrared modulation and demodulation
 *@param[in] irda_pulse: the narrow pulse width
 *				- UART_1_DIVIDE_OSR_BIT, narrow pulse is 1/OSR bit 
 *				- UART_2_DIVIDE_OSR_BIT, narrow pulse is 2/OSR bit
 *				- UART_3_DIVIDE_OSR_BIT, narrow pulse is 3/OSR bit
 *				- UART_4_DIVIDE_OSR_BIT, narrow pulse is 4/OSR bit
 *@return    none
 */
void uart_irda_enable(uart_instance_e instance, bool irda_en, uart_irda_pulse_e irda_pulse)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);

	UART_TypeDef *base = g_uart_bases[instance];
	uart_drv_irda_set(base, irda_en, irda_pulse);
}

/**
 *@brief set uart wakeup method
 *@param[in] instance: uart instance number
 *@param[in] wake_up: wakeup method
 *				- UART_IDLE_WAKE_UP_WITH_IDLE_FLAG, idle wakeup and wakeup set idle flag
 *				- UART_IDLE_WAKE_UP_WITHOUT_IDLE_FLAG, idle wakeup and wakeup do not set idle flag
 *				- UART_BIT_MARK_WAKE_UP, bit mark wakeup
 *				- UART_MATCH_WAKE_UP, match wakeup
 *@return    none
 */
void uart_wakeup_cfg(uart_instance_e instance, uart_wake_up_e wake_up)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);

	UART_TypeDef *base = g_uart_bases[instance];

	switch (wake_up)
	{
		case UART_IDLE_WAKE_UP_WITH_IDLE_FLAG:
			uart_drv_wakeup_cfg(base, false);
			uart_drv_wakeup_idel_flag_set(base, true);
			/* disable the MATCH1_EN and MATCH2_EN */
			uart_match1_set(instance, false, 0x00);
			uart_match2_set(instance, false, 0x00);
			/* to ensure the MATCH_CFG not to be 11 */
			uart_match_type_set(instance, UART_BIT_MARK_ADDR_MATCH);
			break;
		case UART_IDLE_WAKE_UP_WITHOUT_IDLE_FLAG:
			uart_drv_wakeup_cfg(base, false);
			uart_drv_wakeup_idel_flag_set(base, false);
			/* disable the MATCH1_EN and MATCH2_EN */
			uart_match1_set(instance, false, 0x00);
			uart_match2_set(instance, false, 0x00);
			/* to ensure the MATCH_CFG not to be 11 */
			uart_match_type_set(instance, UART_BIT_MARK_ADDR_MATCH);
			break;
		case UART_BIT_MARK_WAKE_UP:
			uart_drv_wakeup_cfg(base, true);
			/* to ensure the MATCH_CFG not to be 11 */
			uart_match_type_set(instance, UART_BIT_MARK_ADDR_MATCH);
			break;
		case UART_MATCH_WAKE_UP:
			uart_drv_match_wakeup_set(base);
			break;
		default:
			break;
	}
}

/**
 *@brief set data match type 
 *@param[in] instance: uart instance number
 *@param[in] match_type: data match type
 *				- UART_BIT_MARK_ADDR_MATCH, bit mark address match
 *				- UART_IDLE_ADDR_MATCH, idle address match
 *				- UART_ADDR_MATCH_ON_OFF, address match on/off
 *@return    none
 */
void uart_match_type_set(uart_instance_e instance, uart_match_type_e match_type)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);

	UART_TypeDef *base = g_uart_bases[instance];
	uart_drv_match_type_set(base, match_type);
}

/**
 *@brief configure the idle length 
 *@param[in] instance: uart instance number
 *@param[in] idle_len: IDLE_FLAG will be set when idle_len idle character have been received
 *				- UART_1_IDLE_CHAR, 1 idle character  
 *				- UART_2_IDLE_CHAR, 2 idle character 
 *				- UART_4_IDLE_CHAR, 4 idle character 
 *				- UART_8_IDLE_CHAR, 8 idle character
 *				- UART_16_IDLE_CHAR, 16 idle character 
 *				- UART_32_IDLE_CHAR, 32 idle character 
 *				- UART_64_IDLE_CHAR, 64 idle character 
 *				- UART_128_IDLE_CHAR, 128 idle character 
 *@param[in] idle_detect_point: the point that start detect idle
 *				- UART_START_BIT, idle detected start after start bit
 *				- UART_STOP_BIT, idle detected start after stop bit
 *@return    none 
 */
void uart_idle_len_set(uart_instance_e instance, uart_idle_len_e idle_len, uart_idle_detect_point_e idle_detect_point)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);

	UART_TypeDef *base = g_uart_bases[instance];
	uart_drv_idle_len_set(base, idle_len);
	uart_drv_idle_point_set(base, idle_detect_point);
}

/**
 *@brief enable or disable the TX FIFO, and set the TX WATERMARK
 *@param[in] instance: uart instance number
 *@param[in] tx_fifo_en: enable or disable the TX FIFO
 *@param[in] tx_water_mark: TX WATERMARK
 *@return    none
 */
void uart_tx_fifo_enable(uart_instance_e instance, bool tx_fifo_en, uint8_t tx_water_mark)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
	DEV_ASSERT(tx_water_mark < 4U);

	UART_TypeDef *base = g_uart_bases[instance];
	uart_drv_tx_fifo_enable(base, tx_fifo_en, tx_water_mark);
}

/**
 *@brief enable or disable the RX FIFO, and set the RX WATERMARK and the RX IDLE_SETFULL
 *@param[in] instance: uart instance number
 *@param[in] rx_fifo_en: enable or disable the RX FIFO
 *@param[in] rx_water_mark: RX WATERMARK
 *@param[in] idle_words: the number of idel words that will set RX_FULL_FLAG when RX FIFO is not empty
 *				- UART_NOT_SET_FULL, disable this function
 *				- UART_1_WORD_SET_FULL, 1 idel word
 *				- UART_2_WORD_SET_FULL, 2 idel words
 *				- UART_4_WORD_SET_FULL, 4 idel words
 *				- UART_8_WORD_SET_FULL, 8 idel words
 *				- UART_16_WORD_SET_FULL, 16 idel words 
 *				- UART_32_WORD_SET_FULL, 32 idel words
 *				- UART_64_WORD_SET_FULL, 64 idel words
 *@return    none
 */
void uart_rx_fifo_enable(uart_instance_e instance, bool rx_fifo_en, uint8_t rx_water_mark, uart_rx_idle_len_set_full_e idle_words)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
	DEV_ASSERT(rx_water_mark < 4U);

	UART_TypeDef *base = g_uart_bases[instance];
	uart_drv_rx_fifo_enable(base, rx_fifo_en, rx_water_mark);
	uart_drv_rx_idle_set_full_en(base, idle_words);
}

/**
 *@brief get the state of a status flag 
 *@param[in] instance: uart instance number
 *@param[in] status_flag: the status flag to query
 *@return    whether the current status flag is set or not 
 */
bool uart_status_flag_get(uart_instance_e instance, uart_status_flag_e status_flag)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
	UART_TypeDef *base = g_uart_bases[instance];

	bool flag = uart_drv_status_flag_get(base, status_flag);
	
	return flag;
}

/**
 *@brief enable or disable the UART doze mode
 *@param[in] instance: uart instance number
 *@param[in] doze_en:
 *				- true, enable the doze mode
 *				- false, disable the doze mode
 *@return    none
 */
void uart_doze_enable(uart_instance_e instance, bool doze_en)
{
	DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
	UART_TypeDef *base = g_uart_bases[instance];

	uart_drv_doze_enable(base, doze_en);
}


/*******************************************************************************
* Private UART Control Functions
******************************************************************************/
/**
 *@brief rx_full interrupt handler
 *@param[in] instance: uart instance number
 *@return    none
 */
static void uart_rx_full_irq_handler(uart_instance_e instance)
{
    uart_state_t *uart_state = &(g_uart_state[instance]);

    /* Get data and put in receive buffer  */
    uart_get_data(instance);

    /* Update the internal state */
    if (uart_state->bit_number_per_char <= UART_8_BITS_PER_CHAR)
    {
        ++uart_state->rx_buff;
        --uart_state->rx_size;
    }
    else
    {
        uart_state->rx_buff = &uart_state->rx_buff[2];
        uart_state->rx_size -= 2U;
    }

    /* Check if this was the last byte in the current buffer */
    if (uart_state->rx_size == 0U)
    {
        /* Invoke callback if there is one (callback may reset the rx buffer for continuous reception) */
        if (uart_state->rx_call_back != NULL)
        {
            uart_state->rx_call_back(uart_state, UART_EVENT_RX_FULL, uart_state->rx_call_back_param);
        }
    }

    /* Finish reception if this was the last byte received */
    if (uart_state->rx_size == 0U)
    {
        /* Complete transfer (disable rx logic) */
        uart_complete_receive_data_using_interrupt(instance);

        /* Invoke callback if there is one */
        if (uart_state->rx_call_back != NULL)
        {
            uart_state->rx_call_back(uart_state, UART_EVENT_END_TRANSFER, uart_state->rx_call_back_param);
        }
    }
}

/**
 *@brief tx_empty interrupt handler
 *@param[in] instance: uart instance number
 *@return    none
 */
static void uart_tx_empty_irq_handler(uart_instance_e instance)
{
    uart_state_t *uart_state = &(g_uart_state[instance]);
    UART_TypeDef *base = g_uart_bases[instance];

    /* Check if there are any more bytes to send */
    if (uart_state->tx_size > 0U)
    {
        /* Transmit the data */
        uart_put_data(instance);

        /* Update the internal state */
        if (uart_state->bit_number_per_char <= UART_8_BITS_PER_CHAR)
        {
            ++uart_state->tx_buff;
            --uart_state->tx_size;
        }
        else
        {
            uart_state->tx_buff = &uart_state->tx_buff[2U];
            uart_state->tx_size -= 2U;
        }

        /* Check if this was the last byte in the current buffer */
        if (uart_state->tx_size == 0U)
        {
            /* Invoke callback if there is one (callback may reset the tx buffer for continuous transmission)*/
            if (uart_state->tx_call_back != NULL)
            {
                uart_state->tx_call_back(uart_state, UART_EVENT_TX_EMPTY, uart_state->tx_call_back_param);
            }

            /* If there's no new data, disable tx empty interrupt and enable transmission complete interrupt */
            if (uart_state->tx_size == 0U)
            {
                uart_drv_interrupt_source_enable(base, UART_INT_TX_DATA_REG_EMPTY, false);
                uart_drv_interrupt_source_enable(base, UART_INT_TX_COMPLETE, true);
            }
        }
    }
}

/**
 *@brief tx_complete interrupt handler
 *@param[in] instance: uart instance number
 *@return    none
 */
static void uart_tx_complete_irq_handler(uart_instance_e instance)
{
    uart_state_t *uart_state = &(g_uart_state[instance]);

    if (uart_state->tx_size == 0U)
    {
				/* Complete the interrupt based transfer */
				uart_complete_send_data_using_interrupt(instance);
        /* Invoke callback if there is one */
        if (uart_state->tx_call_back != NULL)
        {
            uart_state->tx_call_back(uart_state, UART_EVENT_END_TRANSFER, uart_state->tx_call_back_param);
        }
    }
}

/**
 *@brief error interrupt handler
 *@param[in] instance: uart instance number
 *@return    none
 */
//static void uart_error_irq_handler(uart_instance_e instance)
//{
//    uart_state_t *uart_state = &(g_uart_state[instance]);
//    UART_TypeDef *base = g_uart_bases[instance];

//    /* Handle receive overrun interrupt */
//	if (uart_drv_interrupt_source_status_get(base, UART_INT_RX_OVERRUN))
//	{
//		if (uart_drv_status_flag_get(base, UART_RX_OVERRUN))
//    	{
//	        /* Update the internal status */
//	        uart_state->receive_status = STATUS_UART_RX_OVERRUN;
//					/* Complete the transfer */
//					uart_complete_receive_data_using_interrupt(instance);
//	        
//	        /* Invoke callback if there is one */
//	        if (uart_state->rx_call_back != NULL)
//	        {
//	            uart_state->rx_call_back(uart_state, UART_EVENT_ERROR, uart_state->rx_call_back_param);
//	        }

//	        /* Clear the flags */
//	        uart_drv_error_interrupts_flag_clear(base);
//    	}
//	}
//	
//    if (uart_drv_interrupt_source_status_get(base, UART_INT_FRAME_ERR_FLAG))
//    {
//		/* Handle framing error interrupt */
//	    if (uart_drv_status_flag_get(base, UART_FRAME_ERR))
//	    {
//	        /* Update the internal status */
//	        uart_state->receive_status = STATUS_UART_FRAMING_ERROR;
//					/* Complete the transfer */
//					uart_complete_receive_data_using_interrupt(instance);
//	        
//	        /* Invoke callback if there is one */
//	        if (uart_state->rx_call_back != NULL)
//	        {
//	            uart_state->rx_call_back(uart_state, UART_EVENT_ERROR, uart_state->rx_call_back_param);
//	        }

//	        /* Clear the flags */
//	        uart_drv_error_interrupts_flag_clear(base);
//	    }
//	}

//	if (uart_drv_interrupt_source_status_get(base, UART_INT_PARITY_ERR_FLAG))
//	{
//		/* Handle parity error interrupt */
//	    if (uart_drv_status_flag_get(base, UART_PARITY_ERR))
//	    {
//	        /* Update the internal status */
//	        uart_state->receive_status = STATUS_UART_PARITY_ERROR;
//					/* Complete the transfer  */
//					uart_complete_receive_data_using_interrupt(instance);
//	        /* Invoke callback if there is one */
//	        if (uart_state->rx_call_back != NULL)
//	        {
//	            uart_state->rx_call_back(uart_state, UART_EVENT_ERROR, uart_state->rx_call_back_param);
//	        }

//	        /* Clear the flags */
//	        uart_drv_error_interrupts_flag_clear(base);
//	    }
//	}

//	if (uart_drv_interrupt_source_status_get(base, UART_INT_NOISE_ERR_FLAG))
//	{
//		/* Handle noise error interrupt */
//	    if (uart_drv_status_flag_get(base, UART_NOISE_DETECT))
//	    {
//	        /* Update the internal status */
//	        uart_state->receive_status = STATUS_UART_NOISE_ERROR;
//					/* Complete transfer  */
//					uart_complete_receive_data_using_interrupt(instance);
//	        /* Invoke callback if there is one */
//	        if (uart_state->rx_call_back != NULL)
//	        {
//	            uart_state->rx_call_back(uart_state, UART_EVENT_ERROR, uart_state->rx_call_back_param);
//	        }

//	        /* Clear the flags */
//	        uart_drv_error_interrupts_flag_clear(base);
//	    }
//	} 
//}

/**
 *@brief start a transmit by beginning the process of sending data and enabling the interrupt
 *@param[in] instance: uart instance number
 *@param[in] *tx_buff: the buffer pointer which saves the data to be sent
 *@param[in] tx_size: the size of data to be sent in unit of byte
 *@return    whether start the transmit successfully or not:
 *				- STATUS_SUCCESS, start the transmit successfully
 *				- STATUS_BUSY, the uart is busy
 */
static status_t uart_send_data_using_interrupt(uart_instance_e instance,
                                                 const uint8_t *tx_buff,
                                                 uint32_t tx_size)
{
    DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
    DEV_ASSERT(tx_buff != NULL);

    UART_TypeDef *base = g_uart_bases[instance];
    uart_state_t *uart_state = &(g_uart_state[instance]);

    /* Check it's not busy transmitting data from a previous function call */
    if (uart_state->is_tx_busy)
    {
        return STATUS_BUSY;
    }

    /* Check the validity of the parameters */
    DEV_ASSERT(tx_size > 0U);
    DEV_ASSERT((uart_state->bit_number_per_char <= UART_8_BITS_PER_CHAR) ||
               ((tx_size & 1U) == 0U));

    /* initialize the module driver state structure */
    uart_state->tx_buff = tx_buff;
    uart_state->tx_size = tx_size;
    uart_state->is_tx_busy = true;
    uart_state->transmit_status = STATUS_BUSY;

    /* Enable the UART transmitter */
    uart_drv_transmit_enable(base, true);

    /* Enable tx empty interrupt */
    uart_drv_interrupt_source_enable(base, UART_INT_TX_DATA_REG_EMPTY, true);

    return STATUS_SUCCESS;
}


/**
 *@brief finish up a transmit by completing the process of sending data and disabling the interrupt
 *@param[in] instance: uart instance number
 *@return    none
 */
static void uart_complete_send_data_using_interrupt(uart_instance_e instance)
{
    DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);

    UART_TypeDef *base = g_uart_bases[instance];
    uart_state_t *uart_state = &(g_uart_state[instance]);

    if (uart_state->transmit_status == STATUS_BUSY)
    {
        /* If the transfer is completed, update the transmit status */
        uart_state->transmit_status = STATUS_SUCCESS;
    }
    else
    {
        /* If the transfer is aborted or timed out, disable tx empty interrupt */
        uart_drv_interrupt_source_enable(base, UART_INT_TX_DATA_REG_EMPTY, false);
    }

    /* Disable transmission complete interrupt */
    uart_drv_interrupt_source_enable(base, UART_INT_TX_COMPLETE, false);

    /* Disable transmitter */
    uart_drv_transmit_enable(base, false);

    /* Update the internal busy flag */
    uart_state->is_tx_busy = false;
}

/**
 *@brief start a receive by beginning the process of receiving data and enabling the interrupt
 *@param[in] instance: uart instance number
 *@param[in] *rx_buff: the buffer pointer which saves the data to be receive
 *@param[in] rx_size: the size of data to be receive in unit of byte
 *@return    whether start the receive successfully or not:
 *				- STATUS_SUCCESS, start the receive successfully
 *				- STATUS_BUSY, the uart is busy
 */
static status_t uart_receive_data_using_interrupt(uart_instance_e instance,
                                                    uint8_t *rx_buff,
                                                    uint32_t rx_size)
{
    DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);
    DEV_ASSERT(rx_buff != NULL);

    uart_state_t *uart_state = &(g_uart_state[instance]);
    UART_TypeDef *base = g_uart_bases[instance];

    /* Check it's not busy receiving data from a previous function call */
    if (uart_state->is_rx_busy)
    {
        return STATUS_BUSY;
    }

    /* Check the validity of the parameters */
    DEV_ASSERT(rx_size > 0U);
    DEV_ASSERT((uart_state->bit_number_per_char <= UART_8_BITS_PER_CHAR) ||
               ((rx_size & 1U) == 0U));

    /* Initialize the module driver state struct to indicate transfer in progress
     * and with the buffer and byte count data. */
    uart_state->is_rx_busy = true;
    uart_state->rx_buff = rx_buff;
    uart_state->rx_size = rx_size;
    uart_state->receive_status = STATUS_BUSY;

    /* Enable the receiver */
    uart_drv_receive_enable(base, true);

    /* Enable autobaud interrupts */
	if(true != uart_state->autobaud_en)
	{
		uart_drv_error_interrupts_source_enable(base, true);
	}

    /* Enable receive data full interrupt */
    //uart_drv_interrupt_source_enable(base, UART_INT_RX_DATA_REG_FULL, true);
	base->IER_STR.RXOVIE = 1U; 
	

    return STATUS_SUCCESS;
}

/**
 *@brief finish up a receive by completing the process of receiving data and disabling the interrupt
 *@param[in] instance: uart instance number
 *@return    none
 */
static void uart_complete_receive_data_using_interrupt(uart_instance_e instance)
{
    DEV_ASSERT((uint8_t)instance < UART_INSTANCE_MAX_COUNT);

    uint8_t tmp_byte;
    uart_state_t *uart_state = &(g_uart_state[instance]);
    UART_TypeDef *base = g_uart_bases[instance];

    /* Disable receiver */
    uart_drv_receive_enable(base, false);

    /* Disable error interrupts */
    uart_drv_error_interrupts_source_enable(base, false);

    /* Read dummy to clear RX_FULL_FLAG */
    uart_drv_get_char(base, &tmp_byte);

    /* Disable receive data full interrupt. */
    uart_drv_interrupt_source_enable(base, UART_INT_RX_DATA_REG_FULL, false);

    /* Update the information of the module driver state */
    uart_state->is_rx_busy = false;
    if (uart_state->receive_status == STATUS_BUSY)
    {
        uart_state->receive_status = STATUS_SUCCESS;
    }
}


/**
 *@brief write data to the data register
 *@param[in] instance: uart instance number
 *@return    none
 */
static void uart_put_data(uart_instance_e instance)
{
    const uart_state_t *uart_state = &(g_uart_state[instance]);
    UART_TypeDef *base = g_uart_bases[instance];
    uint16_t data;
    const uint8_t *tx_buff = uart_state->tx_buff;

    if (uart_state->bit_number_per_char <= UART_8_BITS_PER_CHAR)
    {
        /* Transmit the data */
        data = (uint16_t)(*tx_buff);
        uart_drv_put_char(base, data);
    }
    else if (uart_state->bit_number_per_char == UART_9_BITS_PER_CHAR)
    {
        /* Create a 16-bits integer from two bytes */
        data = (uint16_t)(*tx_buff);
        ++tx_buff;
        data |= (uint16_t)(((uint16_t)(*tx_buff)) << 8U);

        /* Transmit the data */
        uart_drv_put_char(base, data);
    }
    else
    {
        /* Create a 16-bits integer from two bytes */
        data = (uint16_t)(*tx_buff);
        ++tx_buff;
        data |= (uint16_t)(((uint16_t)(*tx_buff)) << 8U);

        /* Transmit the data */
        uart_drv_put_char(base, data);
    }
}

/**
 *@brief read data from the buffer register
 *@param[in] instance: uart instance number
 *@return    none
 */
static void uart_get_data(uart_instance_e instance)
{
    const uart_state_t *uart_state = &(g_uart_state[instance]);
	const UART_TypeDef *base = g_uart_bases[instance];
    uint16_t data;
    uint8_t *rx_buff = uart_state->rx_buff;

    if (uart_state->bit_number_per_char == UART_7_BITS_PER_CHAR)
    {
        /* Receive the data */
        uart_drv_get_char(base, rx_buff);
		/* only get the 7 bits data */
		*rx_buff = *rx_buff & 0x7FU;
    }
	
	else if (uart_state->bit_number_per_char == UART_8_BITS_PER_CHAR)
	{
		/* Receive the data */
        uart_drv_get_char(base, rx_buff);
	}
    else if (uart_state->bit_number_per_char == UART_9_BITS_PER_CHAR)
    {
        /* Receive the data */
        uart_drv_get_char9_10(base, &data);

        /* Write the least significant bits to the receive buffer */
        *rx_buff = (uint8_t)(data & 0xFFU);
        ++rx_buff;
        /* Write the ninth bit to the subsequent byte in the rx buffer */
        *rx_buff = (uint8_t)(data >> 8U) & 0x1U;
    }
    else
    {
        /* Receive the data */
        uart_drv_get_char9_10(base, &data);

        /* Write the least significant bits to the receive buffer */
        *rx_buff = (uint8_t)(data & 0xFFU);
        ++rx_buff;
        /* Write the ninth and tenth bits to the subsequent byte in the rx buffer */
        *rx_buff = (uint8_t)(data >> 8U) & 0x3U;
    }
}

void uart0_rx_empty_wait()
{
	const UART_TypeDef *base = g_uart_bases[0];
	
	while(base->SR_STR.RXEMPT == 0U)
	{
		uint16_t data = (base->DR_STR.DATA&0xFF);
	}
	
}


/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  : UartView
* Description    : 串口监测数据--调试用
* Function Call  : 
* Input Paragram :
* Return Value   : 
*-----------------------------------------------------------------------------
******************************************************************************/
inline void UartView(int16_t view1, int16_t view2, int16_t view3, int16_t view4)
{
	UART_TypeDef *base = g_uart_bases[UART_INSTANCE1];
	
	uint8_t sum = 0;

	sum += (uint8_t)(view1);
	sum += (uint8_t)(view1>>8);
	sum += (uint8_t)(view2);
	sum += (uint8_t)(view2>>8);
	sum += (uint8_t)(view3);
	sum += (uint8_t)(view3>>8);
	sum += (uint8_t)(view4);
	sum += (uint8_t)(view4>>8);
	
	//while(base->SR_STR.TXFULL){};
	base->DR_STR.DATA = (uint8_t)(view1);
	//while(base->SR_STR.TXFULL){};
	base->DR_STR.DATA = (uint8_t)(view1>>8);
	//while(base->SR_STR.TXFULL){};
	base->DR_STR.DATA = (uint8_t)(view2);
	//while(base->SR_STR.TXFULL){};
	base->DR_STR.DATA = (uint8_t)(view2>>8);
	//while(base->SR_STR.TXFULL){};
	base->DR_STR.DATA = (uint8_t)(view3);
	//while(base->SR_STR.TXFULL){};
	base->DR_STR.DATA = (uint8_t)(view3>>8);
	//while(base->SR_STR.TXFULL){};
	base->DR_STR.DATA = (uint8_t)(view4);
	//while(base->SR_STR.TXFULL){};
	base->DR_STR.DATA = (uint8_t)(view4>>8);
	//while(base->SR_STR.TXFULL){};
	base->DR_STR.DATA = sum;	
}


#ifdef UART
/**
 *@brief send data used for INFO FUNCTION
 *@param[in] ch: the data to send
 *@param[in] f: 
 *@return    the data to send
 */
int fputc(int ch, FILE *f)
{
	UNUSED(f);
	UART_TypeDef *base = g_uart_bases[UART_CONSOLE_INSTANCE];
	
	uart_drv_fput_char(base, (uint8_t)ch);

	return ch;
}

#if __ICCARM__
/**
 *@brief send data used for INFO FUNCTION
 *@param[in] file: file handle
 *@param[in] ptr: pointer to the data to send
 *@param[in] len: length of the data to send
 *@return    the data to send
 */
int __write(int file, char *ptr, int len)
{
    UNUSED(file);

    UART_TypeDef *base = g_uart_bases[UART_CONSOLE_INSTANCE];

    for(int i = 0; i < len; i++)
    {
        uart_drv_fput_char(base, (uint8_t)(ptr[i]));
    }

    return len;
}
#endif

#if defined(USE_GCC) || defined(USE_GHS)
/**
 *@brief send data used for sy_printf
 *@param[in] character: the data to send
 *@return    void
 */
void _putchar(char character)
{
	UART_TypeDef *base = g_uart_bases[UART_CONSOLE_INSTANCE];
	
	uint32_t tmp_d = character;
	
	tmp_d &= 0xFFU;
	
	uart_drv_fput_char(base, (uint8_t)tmp_d);
}
#endif /* USE_GCC USE_GHS */
#endif/*UART*/
