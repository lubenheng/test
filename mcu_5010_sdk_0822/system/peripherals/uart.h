
#ifndef UART_H
#define UART_H
 
 /*******************************************************************************
 * Private UART Static Inline Functions
 ******************************************************************************/ 
 /**
 *@brief enable or disable the uart transmitter
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] tx_en: enable or disable transmitter
 *@return    none
 */
static inline void uart_drv_transmit_enable(UART_TypeDef *uart_ptr, bool tx_en)
{
    uart_ptr->CR_STR.w = (uart_ptr->CR_STR.w & ~UART_CTRL_TXEN_MASK) | ((tx_en ? 1UL : 0UL) << UART_CTRL_TXEN_POS);
	/* enbale TX FIFO */
	uart_ptr->CR_STR.w = (uart_ptr->CR_STR.w & ~UART_CTRL_TX_FIFO_EN_MASK) | ((tx_en ? 1UL : 0UL) << UART_CTRL_TX_FIFO_EN_POS);

	__DSB();
	__ISB();
    /* Wait for the register write operation to complete */
    //while((bool)((uart_ptr->CR_STR.w & UART_CTRL_TXEN_MASK) != 0U) != tx_en) {}
}

/**
 *@brief return the status of uart transmitter
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@return    the status of uart transmitter
 */
static inline bool uart_drv_transmit_status_get(const UART_TypeDef *uart_ptr)
{
	// return (((uart_ptr->CTRL.w >> UART_CTRL_TXEN_POS) & 1U) > 0U);
	 return 0; //delete later
}

 /**
 *@brief enable or disable the uart receiver
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] rx_en: enable or disable receiver
 *@return    none
 */
static inline void uart_drv_receive_enable(UART_TypeDef *uart_ptr, bool rx_en)
{
    uart_ptr->CR_STR.w = (uart_ptr->CR_STR.w & ~UART_CTRL_RXEN_MASK) | ((rx_en ? 1UL : 0UL) << UART_CTRL_RXEN_POS);
	/* enbale RX FIFO */
	uart_ptr->CR_STR.w = (uart_ptr->CR_STR.w & ~UART_CTRL_RX_FIFO_EN_MASK) | ((rx_en ? 1UL : 0UL) << UART_CTRL_RX_FIFO_EN_POS);

	__DSB();
	__ISB();
    /* Wait for the register write operation to complete */
    //while((bool)((uart_ptr->CR_STR.w & UART_CTRL_RXEN_MASK) != 0U) != rx_en) {;}
}

/**
 *@brief set the uart baud rate divider integer part
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] baudrate_div_int: the uart baud rate divider integer part
 *@return    none
 */
static inline void uart_drv_baudrate_divisor_integer_set(UART_TypeDef *uart_ptr, uint32_t baudrate_div_int)
{
	// uart_ptr->BAUD.w = (uart_ptr->BAUD.w & ~UART_BAUD_BAUD_DIV_INT_MASK) | (baudrate_div_int << UART_BAUD_BAUD_DIV_INT_POS);
	uart_ptr->BRR_STR.w = (uart_ptr->BRR_STR.w & ~UART_BAUD_BAUD_DIV_INT_MASK) | (baudrate_div_int << UART_BAUD_BAUD_DIV_INT_POS);
}

/**
 *@brief set the uart baud rate divider fraction part
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] baudrate_div_fra: the uart baud rate divider fraction part
 *@return    none
 */
static inline void uart_drv_baudrate_divisor_fraction_set(UART_TypeDef *uart_ptr, uint32_t baudrate_div_fra)
{
	// uart_ptr->AUTOBAUD.w = (uart_ptr->AUTOBAUD.w & ~UART_AUTOBAUD_BAUD_DIV_FRA_MASK) | (baudrate_div_fra << UART_AUTOBAUD_BAUD_DIV_FRA_POS);
}

/**
 *@brief get the uart baud rate divider integer part
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@return    the uart baud rate divider integer part
 */
static inline uint16_t uart_drv_baudrate_divisor_integer_get(const UART_TypeDef *uart_ptr)
{
//    return ((uint16_t)((uart_ptr->BAUD.w & UART_BAUD_BAUD_DIV_INT_MASK) >> UART_BAUD_BAUD_DIV_INT_POS));
		 return 0; //delete later
}

/**
 *@brief get the uart baud rate divider fraction part
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@return    the uart baud rate divider fraction part
 */
static inline uint16_t uart_drv_baudrate_divisor_fraction_get(const UART_TypeDef *uart_ptr)
{
    // return ((uint16_t)((uart_ptr->AUTOBAUD.w & UART_AUTOBAUD_BAUD_DIV_FRA_MASK) >> UART_AUTOBAUD_BAUD_DIV_FRA_POS));
		 return 0; //delete later
}

/**
 *@brief set the uart baud rate oversampling ratio
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] over_sample_ratio, the oversampling ratio
 *@return    none
 */
static inline void uart_drv_oversample_ratio_set(UART_TypeDef *uart_ptr, uint32_t over_sample_ratio)
{
	// uart_ptr->BAUD.w = (uart_ptr->BAUD.w & ~UART_BAUD_OSR_MASK) | (over_sample_ratio << UART_BAUD_OSR_POS);

	uart_ptr->BRR_STR.w = (uart_ptr->BRR_STR.w & ~UART_BAUD_OSR_MASK) | (over_sample_ratio << UART_BAUD_OSR_POS);
}

 /**
 *@brief get the uart baud rate oversampling ratio
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@return    the oversampling ratio
 */
static inline uint8_t uart_drv_oversample_ratio_get(const UART_TypeDef *uart_ptr)
{
    // return ((uint8_t)((uart_ptr->BAUD.w & UART_BAUD_OSR_MASK) >> UART_BAUD_OSR_POS));
		 return 0; //delete later
}

/**
 *@brief set the number of stop bits in the uart controller
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] stop_bit_count: number of stop bits
 *@return    none
 */
static inline void uart_drv_stopbit_number_set(UART_TypeDef *uart_ptr, uart_stop_bit_count_e stop_bit_count)
{
    // uart_ptr->BAUD.w = (uart_ptr->BAUD.w & ~UART_BAUD_TWO_STOP_MASK) | ((uint32_t)stop_bit_count << UART_BAUD_TWO_STOP_POS);
	uart_ptr->CR_STR.w = (uart_ptr->CR_STR.w & ~UART_BAUD_TWO_STOP_MASK) | ((uint32_t)stop_bit_count << UART_BAUD_TWO_STOP_POS);
}

/**
 *@brief send an character 
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] data: data to be send
 *@return    none
 */
static inline void uart_drv_put_char(UART_TypeDef *uart_ptr, uint16_t data)
{
//    uart_ptr->DATA.w = (uint32_t)data & 0x3ffU;

#if 1
	while(uart_ptr->SR_STR.TXFULL){};
	uart_ptr->DR_STR.DATA = (((uint32_t)data) & 0x3FFU);
#endif
	
#if 0
	while(uart_ptr->SR_STR.TXEMPT == 0U){};
	uart_ptr->DR_STR.DATA = (((uint32_t)data) & 0x3FFU);
	
#endif
}

/**
 *@brief clear the error flags 
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@return    none
 */
static inline void uart_drv_error_interrupts_flag_clear(UART_TypeDef *uart_ptr)
{
    // uint32_t mask = UART_STAT_RX_OVERRUN_FLAG_MASK |
    //                 UART_STAT_NOISE_FLAG_MASK |
    //                 UART_STAT_STOP_ERR_FLAG_MASK |
    //                 UART_STAT_PAR_ERR_FLAG_MASK;

    // uart_ptr->STAT.w = (uart_ptr->STAT.w & (~FEATURE_UART_STAT_REG_FLAGS_MASK)) | mask;
}

/**
 *@brief clear uart logic and register
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@return    none 
 */
static inline void uart_drv_soft_reset(UART_TypeDef *uart_ptr)
{
	// uart_ptr->SOFTRESET.w = (uart_ptr->SOFTRESET.w & ~UART_SOFTRESET_SOFT_RESET_REQ_MASK) | (1UL << UART_SOFTRESET_SOFT_RESET_REQ_POS);
	// uart_ptr->SOFTRESET.w = (uart_ptr->SOFTRESET.w & ~UART_SOFTRESET_SOFT_RESET_REQ_MASK) | (0UL << UART_SOFTRESET_SOFT_RESET_REQ_POS);
}

/**
 *@brief get the polarity of the receive data
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@return    rx data polarity
 */
static inline bool uart_drv_rx_data_polarity_get(const UART_TypeDef *uart_ptr)
{
    // return (((uart_ptr->STAT.w >> UART_STAT_RX_REVERSE_POS) & 1U) > 0U);
		 return 0; //delete later
}

/**
 *@brief set the polarity of the receive data
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] polarity: rx data polarity
 *@return    none
 */
static inline void uart_drv_rx_data_polarity_set(UART_TypeDef *uart_ptr, uart_tx_rx_polarity_e polarity)
{
    // uart_ptr->STAT.w = (uart_ptr->STAT.w & (~FEATURE_UART_STAT_REG_FLAGS_MASK & ~UART_STAT_RX_REVERSE_MASK)) | \
    //              ((uint32_t)polarity << UART_STAT_RX_REVERSE_POS);
	uart_ptr->CR_STR.w = (uart_ptr->CR_STR.w & (~UART_STAT_RX_REVERSE_MASK)) | ((uint32_t)polarity << UART_STAT_RX_REVERSE_POS);
}

/**
 *@brief set the polarity of the send data
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] polarity: tx data polarity.
 *@return    none
 */
static inline void uart_drv_tx_data_polarity_set(UART_TypeDef *uart_ptr, uart_tx_rx_polarity_e polarity)
{
    // uart_ptr->CTRL.w = (uart_ptr->CTRL.w & ~UART_CTRL_TX_REVERSE_MASK) | ((uint32_t)polarity << UART_CTRL_TX_REVERSE_POS);
	uart_ptr->CR_STR.w = (uart_ptr->CR_STR.w & ~UART_CTRL_TX_REVERSE_MASK) | ((uint32_t)polarity << UART_CTRL_TX_REVERSE_POS);
}

/**
 *@brief transimit the MSB or LSB first
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] bit_order:
 *				- true, transmit MSB first
 *				- false, transmit LSB first
 *@return    none
 */
static inline void uart_drv_bit_order_set(UART_TypeDef *uart_ptr, uart_bit_order_e bit_order)
{
	// uart_ptr->STAT.w = (uart_ptr->STAT.w & (~FEATURE_UART_STAT_REG_FLAGS_MASK & ~UART_STAT_MSB_EN_MASK)) | \
    //              ((uint32_t)bit_order << UART_STAT_MSB_EN_POS);
	 uart_ptr->CR_STR.w = (uart_ptr->CR_STR.w & (~UART_STAT_MSB_EN_MASK)) | ((uint32_t)bit_order << UART_STAT_MSB_EN_POS);
}
/**
 *@brief enable or disable infrared modulation and demodulation function
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] irda_en: enable or disable infrared modulation and demodulation function
 *@param[in] irda_pulse: the narrow pulse width
 *@return    none
 */
static inline void uart_drv_irda_set(UART_TypeDef *uart_ptr, bool irda_en, uart_irda_pulse_e irda_pulse)
{
	// uart_ptr->FLOWCFG.w = (uart_ptr->FLOWCFG.w & ~UART_FLOWCFG_IRDA_EN_MASK) | ((irda_en ? 1UL : 0UL) << UART_FLOWCFG_IRDA_EN_POS);
	// uart_ptr->FLOWCFG.w = (uart_ptr->FLOWCFG.w & ~UART_FLOWCFG_PULSE_WIDTH_MASK) | ((uint32_t)irda_pulse << UART_FLOWCFG_PULSE_WIDTH_POS);
}

/**
 *@brief whether enter standby mode or not
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] standby_en: whether enter standby mode or not
 *@return    none
 */
static inline void uart_drv_standby_mode_enter(UART_TypeDef *uart_ptr, bool standby_en)
{
	// uart_ptr->CTRL.w = (uart_ptr->CTRL.w & ~UART_CTRL_STANDBY_MASK) | ((standby_en ? 1UL : 0UL) << UART_CTRL_STANDBY_POS);
}

/**
 *@brief configure the wakeup method
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] wakeup_cfg: idle wakeup or bit mark wakeup
 *@return    none 
 */
static inline void uart_drv_wakeup_cfg(UART_TypeDef *uart_ptr, bool wakeup_cfg)
{
	// uart_ptr->CTRL.w = (uart_ptr->CTRL.w & ~UART_CTRL_WAKE_CFG_MASK) | ((wakeup_cfg ? 1UL : 0UL) << UART_CTRL_WAKE_CFG_POS);
}

/**
 *@brief decide whether the idle character detected set the IDLE_FLAG during idle wakeup mode
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] idle_flag_en: whether set IDLE_FLAG or not
 *@return    none
 */
static inline void uart_drv_wakeup_idel_flag_set(UART_TypeDef *uart_ptr, bool idle_flag_en)
{
	// uart_ptr->STAT.w = (uart_ptr->STAT.w & (~FEATURE_UART_STAT_REG_FLAGS_MASK & ~UART_STAT_WAKE_SET_IDLE_MASK)) | \
    //              ((idle_flag_en ? 1UL : 0UL) << UART_STAT_WAKE_SET_IDLE_POS);
}

/**
 *@brief configure the idel length
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] idle_len: idel length
 *@return    none
 */
static inline void uart_drv_idle_len_set(UART_TypeDef *uart_ptr, uart_idle_len_e idle_len)
{
	// uart_ptr->CTRL.w = (uart_ptr->CTRL.w & ~UART_CTRL_IDLE_LEN_MASK) | ((uint32_t)idle_len << UART_CTRL_IDLE_LEN_POS);
}

/**
 *@brief set the start time of idle detected 
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] idle_detect_point: the point that start detect the idle
 *@return    none
 */
static inline void uart_drv_idle_point_set(UART_TypeDef *uart_ptr, uart_idle_detect_point_e idle_detect_point)
{
	// uart_ptr->CTRL.w = (uart_ptr->CTRL.w & ~UART_CTRL_IDLE_POINT_MASK) | ((uint32_t)idle_detect_point << UART_CTRL_IDLE_POINT_POS);
}

/**
 *@brief enable address matching mode for match address1 and set match address1. 
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] match_en: whether enable address matching mode for match address1 or not
 *@param[in] match_addr: match address1
 *@return    none
 */
static inline void uart_drv_match1_set(UART_TypeDef *uart_ptr, bool match_en, uint32_t match_addr)
{
	// uart_ptr->BAUD.w = (uart_ptr->BAUD.w & ~UART_BAUD_MATCH1_EN_MASK) | ((match_en ? 1UL : 0UL) << UART_BAUD_MATCH1_EN_POS);
	// uart_ptr->MADDR.w = (uart_ptr->MADDR.w & ~UART_MADDR_MATCH_ADDR1_MASK) | (match_addr << UART_MADDR_MATCH_ADDR1_POS);
}

/**
 *@brief enable address matching mode for match address2 and set match address2 
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] match_en: whether enable address matching mode for match address2 or not
 *@param[in] match_addr: match address2
 *@return    none
 */
static inline void uart_drv_match2_set(UART_TypeDef *uart_ptr, bool match_en, uint32_t match_addr)
{
	// uart_ptr->BAUD.w = (uart_ptr->BAUD.w & ~UART_BAUD_MATCH2_EN_MASK) | ((match_en ? 1UL : 0UL) << UART_BAUD_MATCH2_EN_POS);
	// uart_ptr->MADDR.w = (uart_ptr->MADDR.w & ~UART_MADDR_MATCH_ADDR2_MASK) | (match_addr << UART_MADDR_MATCH_ADDR2_POS);
}
/**
 *@brief set match type 
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] match_type: match type
 *@return    none
 */
static inline void uart_drv_match_type_set(UART_TypeDef *uart_ptr, uart_match_type_e match_type)
{
	// uart_ptr->BAUD.w = (uart_ptr->BAUD.w & ~UART_BAUD_MATCH_CFG_MASK) | ((uint32_t)match_type << UART_BAUD_MATCH_CFG_POS);
}

/**
 *@brief set match wake_up as wakeup method 
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@return    none
 */
static inline void uart_drv_match_wakeup_set(UART_TypeDef *uart_ptr)
{
	// uart_ptr->BAUD.w = (uart_ptr->BAUD.w & ~UART_BAUD_MATCH_CFG_MASK) | (3UL << UART_BAUD_MATCH_CFG_POS);
}

/**
 *@brief set loop/single-wire mode
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] loop_single_mode_en: whether set loop/single-wire mode or not
 *@return    none
 */
static inline void uart_drv_loop_singlewire_mode_set(UART_TypeDef *uart_ptr, bool loop_single_mode_en)
{
	// uart_ptr->CTRL.w = (uart_ptr->CTRL.w & ~UART_CTRL_LOOP_SINGLE_EN_MASK) | ((loop_single_mode_en ? 1UL : 0UL) << UART_CTRL_LOOP_SINGLE_EN_POS);
}

/**
 *@brief select the source for RX 
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] rx_src: the source of RX
 *@return    none 
 */
static inline void uart_drv_rx_source_select(UART_TypeDef *uart_ptr, bool rx_src)
{
	// uart_ptr->CTRL.w = (uart_ptr->CTRL.w & ~UART_CTRL_RX_SRC_SEL_MASK) | ((rx_src ? 1UL : 0UL) << UART_CTRL_RX_SRC_SEL_POS);
}

/**
 *@brief set the direction of TXD in single-wire mode 
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] tx_dir: the direction of TXD
 *@return    none
 */
static inline void uart_drv_tx_direction_set(UART_TypeDef *uart_ptr, uart_tx_direction tx_dir)
{
	// uart_ptr->CTRL.w = (uart_ptr->CTRL.w & ~UART_CTRL_TX_DIR_MASK) | ((uint32_t)tx_dir << UART_CTRL_TX_DIR_POS);
}

/**
 *@brief set UART break character length 
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] brk_len: UART break character length setting
 *@return    none 
 */
static inline void uart_drv_set_break_char_transmit_length(UART_TypeDef *uart_ptr, uart_break_char_length_e brk_len)
{
    // uart_ptr->STAT.w = (uart_ptr->STAT.w & (~FEATURE_UART_STAT_REG_FLAGS_MASK & ~UART_STAT_BRK_LEN_MASK)) | \
    //              ((uint32_t)brk_len << UART_STAT_BRK_LEN_POS);
}

/**
 *@brief enable UART break detect 
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] brk_det_en: enable UART break detect or not
 *@return    none 
 */
static inline void uart_drv_break_detect_enable(UART_TypeDef *uart_ptr, bool brk_det_en)
{
    // uart_ptr->STAT.w = (uart_ptr->STAT.w & (~FEATURE_UART_STAT_REG_FLAGS_MASK & ~UART_STAT_BRK_DET_EN_MASK)) | \
    //              ((brk_det_en ? 1UL : 0UL) << UART_STAT_BRK_DET_EN_POS);
}

/**
 *@brief set break character transmission in queue mode. 
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@return    none 
 */
static inline void uart_drv_queue_break_field(UART_TypeDef *uart_ptr)
{
	// uart_ptr->DATA.w = UART_DATA_STOP_ERR_DATA_SET(1U);
}

/**
 *@brief enable or disable the autobaud function of uart
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] autobaud_en: whether enable the autobaud function or not
 *@return    none
 */
static inline void uart_drv_autobaud_enable(UART_TypeDef *uart_ptr, bool autobaud_en)
{
	// uart_ptr->AUTOBAUD.w = (uart_ptr->AUTOBAUD.w & (~FEATURE_UART_AUTOBAUD_REG_FLAGS_MASK & ~UART_AUTOBAUD_AUTOBAUD_EN_MASK)) | \
    //              ((autobaud_en ? 1UL : 0UL) << UART_AUTOBAUD_AUTOBAUD_EN_POS);
}

/**
 *@brief reaturn whether the uart have autobaud successfully or not
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@return    the autobaud result
 */
static inline bool uart_drv_autobaud_status_get(const UART_TypeDef *uart_ptr)
{
	// return (((uart_ptr->AUTOBAUD.w >> UART_AUTOBAUD_AUTOBAUD_COMP_POS) & 1U) > 0U);
		 return 0; //delete later
}

/**
 *@brief clear the autobaud result of last time to autobaud again
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@return    none
 */
static inline void uart_drv_autobaud_result_clear(UART_TypeDef *uart_ptr)
{
	// uart_ptr->AUTOBAUD.w = (uart_ptr->AUTOBAUD.w & (~FEATURE_UART_AUTOBAUD_REG_FLAGS_MASK & ~UART_AUTOBAUD_CLR_AUTOBAUD_MASK)) | \
    //              (1UL << UART_AUTOBAUD_CLR_AUTOBAUD_POS);
	// uart_ptr->AUTOBAUD.w = (uart_ptr->AUTOBAUD.w & (~FEATURE_UART_AUTOBAUD_REG_FLAGS_MASK & ~UART_AUTOBAUD_CLR_AUTOBAUD_MASK)) | \
    //              (0UL << UART_AUTOBAUD_CLR_AUTOBAUD_POS);
}

/**
 *@brief enable the interrupt source of autobaud
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] autobaud_int_en: enable or disable the interrupt
 *@return    none
 */
static inline void uart_drv_autobaud_success_int_enable(UART_TypeDef *uart_ptr, bool autobaud_int_en)
{
	// uart_ptr->AUTOBAUD.w = (uart_ptr->AUTOBAUD.w & (~FEATURE_UART_AUTOBAUD_REG_FLAGS_MASK & ~UART_AUTOBAUD_AUTO_SUCC_INTEN_MASK)) | \
    //              ((autobaud_int_en ? 1UL : 0UL) << UART_AUTOBAUD_AUTO_SUCC_INTEN_POS);
}

/**
 *@brief return the status of autobaud success flag
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@return    the status of autobaud success flag
 */
static inline bool uart_drv_autobaud_success_flag_get(const UART_TypeDef *uart_ptr)
{
	// return (((uart_ptr->AUTOBAUD.w >> UART_AUTOBAUD_AUTO_SUCC_FLAG_POS) & 1U) > 0U);
		 return 0; //delete later
}

/**
 *@brief clear the uart flag of autobaud successful
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@return    none
 */
static inline void uart_drv_autobaud_success_flag_clear(UART_TypeDef *uart_ptr)
{
	// uart_ptr->AUTOBAUD.w = uart_ptr->AUTOBAUD.w;
}

/**
 *@brief enable the TX FIFO and set the TX WARTERMARK
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] tx_fifo_en: enable or disable the tx fifo
 *@param[in] tx_water_mark: TX WARTERMARK
 *@return    none
 */
static inline void uart_drv_tx_fifo_enable(UART_TypeDef *uart_ptr, bool tx_fifo_en, uint8_t tx_water_mark)
{
	// uart_ptr->FIFO.w = (uart_ptr->FIFO.w & (~FEATURE_UART_FIFO_REG_FLAGS_MASK & ~UART_FIFO_TX_FIFO_EN_MASK)) | \
    //              ((tx_fifo_en ? 1UL : 0UL) << UART_FIFO_TX_FIFO_EN_POS);
	// uart_ptr->WATERMARK.w = (uart_ptr->WATERMARK.w & ~UART_WATERMARK_TXWATER_MASK) | ((uint32_t)tx_water_mark << UART_WATERMARK_TXWATER_POS);
}

/**
 *@brief clear the TX FIFO
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@return    none
 */
static inline void uart_drv_tx_fifo_clear(UART_TypeDef *uart_ptr)
{
	// uart_ptr->FIFO.w = (uart_ptr->FIFO.w & (~FEATURE_UART_FIFO_REG_FLAGS_MASK & ~UART_FIFO_CLR_TXFIFO_MASK)) | \
    //              (1UL << UART_FIFO_CLR_TXFIFO_POS);
	// uart_ptr->FIFO.w = (uart_ptr->FIFO.w & (~FEATURE_UART_FIFO_REG_FLAGS_MASK & ~UART_FIFO_CLR_TXFIFO_MASK)) | \
    //              (1UL << UART_FIFO_CLR_TXFIFO_POS);
}

/**
 *@brief clear the RX FIFO
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@return    none
 */
static inline void uart_drv_rx_fifo_clear(UART_TypeDef *uart_ptr)
{
	// uart_ptr->FIFO.w = (uart_ptr->FIFO.w & (~FEATURE_UART_FIFO_REG_FLAGS_MASK & ~UART_FIFO_CLR_RXFIFO_MASK)) | \
    //              (1UL << UART_FIFO_CLR_RXFIFO_POS);

	 uart_ptr->CR_STR.w = (uart_ptr->CR_STR.w & (~UART_CTRL_RXFLUSH_FIFO_CLR_MASK)) | ((uint32_t)1U << UART_CTRL_RXFLUSH_FIFO_CLR_POS);
}  

/**
 *@brief enable or disable the RX FIFO and set RX WARTERMARK
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] rx_fifo_en: enable or disable the RX FIFO
 *@param[in] rx_water_mark: RX WARTERMARK
 *@return    none
 */
static inline void uart_drv_rx_fifo_enable(UART_TypeDef *uart_ptr, bool rx_fifo_en, uint8_t rx_water_mark)
{
	// uart_ptr->FIFO.w = (uart_ptr->FIFO.w & (~FEATURE_UART_FIFO_REG_FLAGS_MASK & ~UART_FIFO_RX_FIFO_EN_MASK)) | \
    //              ((rx_fifo_en ? 1UL : 0UL) << UART_FIFO_RX_FIFO_EN_POS);
	// uart_ptr->WATERMARK.w = (uart_ptr->WATERMARK.w & ~UART_WATERMARK_RXWATER_MASK) | ((uint32_t)rx_water_mark << UART_WATERMARK_RXWATER_POS);
}

/**
 *@brief send a break
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@return    none
 */
static inline void uart_drv_break_send(UART_TypeDef *uart_ptr)
{
	// uart_ptr->DATA.w = UART_DATA_STOP_ERR_DATA_SET(1U);
}

/**
 *@brief set the number of idle words that will set RX_FULL_FLAG when rx fifo is not empty
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] idle_words: the number of idle words
 *@return    none
 */
static inline void uart_drv_rx_idle_set_full_en(UART_TypeDef *uart_ptr, uart_rx_idle_len_set_full_e idle_words)
{
	// uart_ptr->FIFO.w = (uart_ptr->FIFO.w & (~FEATURE_UART_FIFO_REG_FLAGS_MASK & ~UART_FIFO_RXIDLE_SETFULL_EN_MASK)) | \
    //              ((uint32_t)idle_words << UART_FIFO_RXIDLE_SETFULL_EN_POS);
}

/**
 *@brief configure the function of trig_input
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] tx_cts_en: trig_input used as tx_cts or not
 *@return    none
 */
static inline void uart_drv_trig_input_set(UART_TypeDef *uart_ptr, bool tx_cts_en)
{
	// if (true == tx_cts_en)
	// {
	// 	uart_ptr->TRIGCFG.w = (uart_ptr->TRIGCFG.w & ~UART_TRIGCFG_TRIG_CONFIG_MASK) | (2UL << UART_TRIGCFG_TRIG_CONFIG_POS);
	// }
	// else
	// {
	// 	uart_ptr->TRIGCFG.w = (uart_ptr->TRIGCFG.w & ~UART_TRIGCFG_TRIG_CONFIG_MASK) | (0UL << UART_TRIGCFG_TRIG_CONFIG_POS);
	// }
}

/**
 *@brief enable or disable the TX_CTS
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] tx_cts_en: whether enable the TX_CTS or not
 *@return    none
 */
static inline void uart_drv_tx_cts_en(UART_TypeDef *uart_ptr, bool tx_cts_en)
{
	// uart_ptr->FLOWCFG.w = (uart_ptr->FLOWCFG.w & ~UART_FLOWCFG_TX_CTS_EN_MASK) | ((tx_cts_en ? 1UL : 0UL) << UART_FLOWCFG_TX_CTS_EN_POS);
}

/**
 *@brief set the source of TX_CTS
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] src: the source of TX_CTS
 *@return    none
 */
static inline void uart_drv_tx_cts_src_set(UART_TypeDef *uart_ptr, uart_tx_cts_source_e src)
{
	// uart_ptr->FLOWCFG.w = (uart_ptr->FLOWCFG.w & ~UART_FLOWCFG_TX_CTS_SRC_MASK) | ((uint32_t)src << UART_FLOWCFG_TX_CTS_SRC_POS);
}

/**
 *@brief configure the sampled time of TX_CTS
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] cts_sample_time: the sampled time of TX_CTS
 *@return    none
 */
static inline void uart_drv_tx_cts_cfg(UART_TypeDef *uart_ptr, uart_tx_cts_sample_time_e cts_sample_time)
{
	// uart_ptr->FLOWCFG.w = (uart_ptr->FLOWCFG.w & ~UART_FLOWCFG_TX_CTS_CFG_MASK) | ((uint32_t)cts_sample_time << UART_FLOWCFG_TX_CTS_CFG_POS);
}

/**
 *@brief enable or disable the RX_RTS
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] rx_rts_en: whether enable the RX_RTS or not
 *@return    none
 */
static inline void uart_drv_rx_rts_en(UART_TypeDef *uart_ptr, bool rx_rts_en)
{
	// uart_ptr->FLOWCFG.w = (uart_ptr->FLOWCFG.w & ~UART_FLOWCFG_RX_RTS_EN_MASK) | ((rx_rts_en ? 1UL : 0UL) << UART_FLOWCFG_RX_RTS_EN_POS);
}

/**
 *@brief enable or disable the TX_RTS
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] tx_rts_en: whether enable the TX_RTS or not
 *@return    none
 */
static inline void uart_drv_tx_rts_en(UART_TypeDef *uart_ptr, bool tx_rts_en)
{
	// uart_ptr->FLOWCFG.w = (uart_ptr->FLOWCFG.w & ~UART_FLOWCFG_TX_RTS_EN_MASK) | ((tx_rts_en ? 1UL : 0UL) << UART_FLOWCFG_TX_RTS_EN_POS);
}

/**
 *@brief set the polarity of TX_RTS
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] tx_rts_pol: the polarity of TX_RTS
 *@return    none
 */
static inline void uart_drv_tx_rts_polarity_set(UART_TypeDef *uart_ptr, uart_tx_rts_polarity_e tx_rts_pol)
{
	// uart_ptr->FLOWCFG.w = (uart_ptr->FLOWCFG.w & ~UART_FLOWCFG_TX_RTS_POL_MASK) | ((uint32_t)tx_rts_pol << UART_FLOWCFG_TX_RTS_POL_POS);
}

/**
 *@brief set the watermark of RX_RTS
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@param[in] rx_rts_water_mark: the watermark of RX_RTS
 *@return    none
 */
static inline void uart_drv_rx_rts_water_set(UART_TypeDef *uart_ptr, uint8_t rx_rts_water_mark)
{
	// uart_ptr->FLOWCFG.w = (uart_ptr->FLOWCFG.w & ~UART_FLOWCFG_RX_RTS_WATER_MASK) | ((uint32_t)rx_rts_water_mark << UART_FLOWCFG_RX_RTS_WATER_POS);
}

/**
 *@brief get the result of autobaud
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@return    none
 */
static inline uint32_t uart_drv_autobaud_result_get(const UART_TypeDef *uart_ptr)
{
	//  return ((uint32_t)((uart_ptr->AUTOBAUD.w & UART_AUTOBAUD_MEASURE_RES_MASK) >> UART_AUTOBAUD_MEASURE_RES_POS));
		 return 0; //delete later
}

/**
 *@brief get the size of data in RX FIFO
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@return    none
 */
static inline uint8_t uart_drv_rx_size_get(const UART_TypeDef *uart_ptr)
{
	//  return ((uint8_t)((uart_ptr->WATERMARK.w & UART_WATERMARK_RXSIZE_MASK) >> UART_WATERMARK_RXSIZE_POS));
		 return 0; //delete later
}

/**
 *@brief get the size of data in TX FIFO
 *@param[in] *uart_ptr: point to the base address of uart instance
 *@return    none
 */
static inline uint8_t uart_drv_tx_size_get(const UART_TypeDef *uart_ptr)
{
	//  return ((uint8_t)((uart_ptr->WATERMARK.w & UART_WATERMARK_TXSIZE_MASK) >> UART_WATERMARK_TXSIZE_POS));
		 return 0; //delete later
}

/**
 *@brief enable or disable the UART doze mode
 *@param[in] *uart_ptr: pointer to UART base address
 *@param[in] doze_en:
 *				- true, enable the doze mode
 *				- false, disable the doze mode
 *@return    none
 */
static inline void uart_drv_doze_enable(UART_TypeDef *uart_ptr, bool doze_en)
{
	// uart_ptr->CTRL.w = (uart_ptr->CTRL.w & ~UART_CTRL_SLEEP_DIS_MASK) | ((doze_en ? 1UL : 0UL) << UART_CTRL_SLEEP_DIS_POS);
}


/*******************************************************************************
* Public UART Control Functions
******************************************************************************/ 
void uart_drv_bits_number_perchar_set(UART_TypeDef *uart_ptr, uart_bit_number_per_char_e bit_number_per_char, bool parity_en);
void uart_drv_parity_mode_set(UART_TypeDef *uart_ptr, uart_parity_mode_e parity_mode_type);
void uart_drv_interrupt_source_enable(UART_TypeDef *uart_ptr, uart_interrupt_e int_src, bool enable);
bool uart_drv_interrupt_source_status_get(const UART_TypeDef *uart_ptr, uart_interrupt_e int_src);
void uart_drv_get_char(const UART_TypeDef *uart_ptr, uint8_t *read_data);
void uart_drv_get_char9_10(const UART_TypeDef *uart_ptr, uint16_t *read_data);
bool uart_drv_status_flag_get(const UART_TypeDef *uart_ptr, uart_status_flag_e status_flag);
status_t uart_drv_status_flag_clear(UART_TypeDef *uart_ptr, uart_status_flag_e status_flag);
void uart_drv_error_interrupts_source_enable(UART_TypeDef *uart_ptr, bool enable);
void uart_drv_fput_char(UART_TypeDef *uart_ptr, uint8_t ch);
void uart_drv_interrupt_handle_cfg(uart_instance_e instance, UART_INT_HANDLE handle);

#endif /* UART_H */
