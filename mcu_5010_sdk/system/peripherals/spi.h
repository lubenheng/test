#ifndef SPI_H
#define SPI_H

#include "spi_reg.h"
#include "spi_type.h"


/*******************************************************************************
* Private SPI Static Inline Functions
******************************************************************************/ 
/**
 *@brief enable SPI module
 *@param[in] *spi_ptr: pointer to SPI base address
 *@return    none
 */
static inline void spi_drv_enable(SPI_TypeDef *spi_ptr)
{
#if 0
	spi_ptr->CR.SPEN = 1U;
	
#endif //old version reserved
}


/**
 *@brief set the spi watermarks 
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] tx_water_mark: tx water mark
 *@param[in] rx_water_mark: rx water mark
 *@return    none
 */	
static inline void spi_drv_water_marks_set(SPI_TypeDef *spi_ptr, uint8_t tx_water_mark, uint8_t rx_water_mark)
{
	#if 0
	spi_ptr->FCR.TFF_WATER = tx_water_mark;
	spi_ptr->FCR.RFF_WATER = rx_water_mark;
	#endif //old version reserved
}

/**
 *@brief get the spi status flag state
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] status_flag:
 *				- SPI_TX_DATA_FLAG, TX data flag
 *				- SPI_RX_DATA_FLAG, RX data flag 
 *				- SPI_WORD_COMPLETE, word complete flag 
 *				- SPI_FRAME_COMPLETE, frame complete flag
 *				- SPI_TRANSFER_COMPLETE, transfer complete flag
 *				- SPI_TRANSMIT_ERROR, transmit error flag 
 *				- SPI_RECEIVE_ERROR, receive error flag
 *				- SPI_DATA_MATCH, data match flag
 *				- SPI_MODULE_BUSY, module busy flag
 *				- SPI_ALL_STATUS, used for clearing all w1c status flags 
 *@return    the state of the status flag:
 *				- true, the flag is asserted.
 *				- false, the flag is not-asserted.
 */
static inline bool spi_drv_status_flag_get(SPI_TypeDef *spi_ptr, spi_status_flag_e status_flag)
{
	#if 0
    return (bool)(1U == (((spi_ptr->SR.w) >> (uint8_t)status_flag) & 1U));
	#endif //old version reserved
	
	return 0;
}

/**
 *@brief set or clear the spi interrupt
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] interrupt_src: the spi interrupt source
 *@param[in] enable:
 *				- true, enable the interrupt source
 *				- false, disable the interrupt source
 *@return    none
 */



/**
 *@brief return if the SPI interrupt request is enabled or disabled
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] interrupt_src: the interrupt source
 *@return    the status of interrupt source:
 *				- true, the interrupt source is enabled
 *				- false, the interrupt source is disabled
 */
static inline bool spi_drv_interrupt_source_status_get(SPI_TypeDef *spi_ptr, spi_status_flag_e interrupt_src)
{
	#if 0
	return (bool)(1U == (((spi_ptr->IER.w) >> (uint8_t)interrupt_src) & 1U));
	#endif //old version reserved
	
	return 0;
}

/**
 *@brief configure a specific SPI delay parameter manually 
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] pcs_to_sck: PCS to SCK delay
 *@param[in] sck_to_pcs: SCK to PCS delay
 *@param[in] pcs_to_cmd: PCS to CMD delay
 *@param[in] sck_to_sck: SCK to SCK delay
 *@param[in] sck_stp: SCK setup phase delay
 *@param[in] sck_hld: SCK hold phase delay
 *@return    none
 */
static inline void spi_drv_timing_set(SPI_TypeDef *spi_ptr, 
										uint32_t pcs_to_sck, 
										uint32_t sck_to_pcs, 
										uint32_t pcs_to_cmd, 
										uint32_t sck_to_sck, 
										uint32_t sck_stp, 
										uint32_t sck_hld)
{
	#if 0
	spi_ptr->CCR0.PCS2SCK = pcs_to_sck;
	spi_ptr->CCR0.SCK2PCS = sck_to_pcs;
	spi_ptr->CCR1.PCS2CMD = pcs_to_cmd;
	spi_ptr->CCR1.SCK2SCK = sck_to_sck;	
	spi_ptr->CCR1.SCKSTP = sck_stp;
	spi_ptr->CCR1.SCKHLD = sck_hld;	
	#endif //old version reserved
}

/**
 *@brief set the PCS flag to the value of the which_pcs parameter
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] which_pcs: one of <SPI_PCS0, SPI_PCS1, SPI_PCS2, SPI_PCS3>				
 *@return    none
 */
static inline void spi_drv_pcs_set(SPI_TypeDef *spi_ptr, spi_pcs_e which_pcs)
{
	#if 0
	spi_ptr->TCR.PCSSEL = (uint32_t)which_pcs;
	#endif //old version reserved
}

/**
 *@brief configure the SPI SDO/SDI pin configuration mode
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] pin_mode: pin configuration mode
 *@return    whether configure the SPI SDO/SDI pin configuration mode successfully or not:
 *				- STATUS_SUCCESS, configure the SPI SDO/SDI pin configuration mode successfully
 */
static inline status_t spi_drv_pin_mode_cfg(SPI_TypeDef *spi_ptr, uint8_t pin_mode)
{
	#if 0
	spi_ptr->CFGR1.PIN_CFG = pin_mode;

	return STATUS_SUCCESS;
	#endif //old version reserved
	
	return (status_t)0;
}

static inline void spi_drv_interrupt_source_enable(SPI_TypeDef *spi_ptr, spi_status_flag_e interrupt_src, bool enable)
{
	
    if (enable == true)
    {
        //spi_ptr->IER.w |= (1UL << (uint8_t)interrupt_src);
    }
    else
    {
        //spi_ptr->IER.w &= ~(1UL << (uint8_t)interrupt_src);
    }
	
	 //old version reserved
}

/**
 *@brief set the transmit command register parameters.
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] cmd_reg: the value of the transmit command register
 *@return    none
 */
static inline void spi_drv_tcr_reg_set(SPI_TypeDef *spi_ptr, uint32_t cmd_reg)
{
	#if 0
	spi_ptr->TCR.w = cmd_reg;
	#endif //old version reserved
}

/**
 *@brief write data into the TX data buffer 
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] data: the data word to be sent 
 *@return    none.
 */	
static inline void spi_drv_data_write(SPI_TypeDef *spi_ptr, uint32_t data)
{		
	#if 0
    spi_ptr->TDR = data;
	#endif //old version reserved
}

/**
 *@brief read data from the RX data buffer 
 *@param[in] *spi_ptr: pointer to SPI base address
 *@return    the data read from the data buffer
 */	
static inline uint32_t spi_drv_data_read(SPI_TypeDef *spi_ptr)
{	
	#if 0
    return (uint32_t)spi_ptr->RDR;
	#endif //old version reserved
	
	return 0;
}

/**
 *@brief get the number of words currently saved in the transmit FIFO
 *@param[in] *spi_ptr: pointer to SPI base address
 *@return    the number of words in the transmit FIFO
 */
static inline uint8_t spi_drv_transimit_words_number_get(SPI_TypeDef *spi_ptr)
{
	#if 0
	return (uint8_t)spi_ptr->FSR.TFF_CNT;
	#endif //old version reserved
	
	return 0;
}

/**
 *@brief get the number of words currently saved in the receive FIFO
 *@param[in] *spi_ptr: pointer to SPI base address
 *@return    the number of words in the receive FIFO
 */
static inline uint8_t spi_drv_receive_words_number_get(SPI_TypeDef *spi_ptr)
{
	#if 0
	return (uint8_t)spi_ptr->FSR.RFF_CNT;
	#endif //old version reserved
	
	return 0;
}

/**
 *@brief set or clear rx mask bit form TCR register
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] enable:
 *				- true, set rx mask bit
 *				- false, clear rx mask bit
 *@return    none
 */
static inline void spi_drv_rx_mask_set(SPI_TypeDef *spi_ptr, bool enable)
{
	#if 0
	spi_ptr->TCR.RXMASK = (enable ? 1UL : 0UL);
	#endif //old version reserved
}

/**
 *@brief set or clear tx mask bit form TCR register
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] enable:
 *				- true, set rx mask bit
 *				- false, clear rx mask bit
 *@return    none    
 */
static inline void spi_drv_tx_mask_set(SPI_TypeDef *spi_ptr, bool enable)
{
	#if 0
	spi_ptr->TCR.TXMASK = (enable ? 1UL : 0UL);
	#endif //old version reserved
}

/**
 *@brief set or clear CONT_CMD bit form TCR register
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] enable: set or clear CONT_CMD bit.
 *				- true, set CONT_CMD bit
 *				- false, clear CONT_CMD bit
 *@return    none
 */				  						
static inline void spi_drv_cont_cmd_bit_set(SPI_TypeDef *spi_ptr, bool enable)
{		
	#if 0
	spi_ptr->TCR.CONT_CMD = (enable ? 1UL : 0UL);
	#endif //old version reserved
}

/**
 *@brief configure if the sample point for master devices is delayed
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] sample_point_delay_en: configure if the sampling point is delayed for master devices
 *				- true, the sampling point is delayed for master devices
 *				- false, the sampling point is not delayed for master devices
 *@return    none
 */
static inline void spi_drv_sample_point_set(SPI_TypeDef *spi_ptr, bool sample_point_delay_en)
{
	#if 0
	spi_ptr->CFGR1.SAMP_CLK = (sample_point_delay_en ? 1UL : 0UL);
	#endif //old version reserved
}

/**
 *@brief get the address of receive data register 
 *@param[in] *spi_ptr: pointer to SPI base address
 *@return    the address of receive data register
 */	
static inline uint32_t spi_drv_read_data_address_get(SPI_TypeDef *spi_ptr)
{
	#if 0
    return (uint32_t)(&(spi_ptr->RDR));
	#endif //old version reserved
	
	return 0;
}

/**
 *@brief get the address of transmit data register 
 *@param[in] *spi_ptr: pointer to SPI base address
 *@return    the address of transmit data register
 */	
static inline uint32_t spi_drv_write_data_address_get(SPI_TypeDef *spi_ptr)
{
	#if 0
    return (uint32_t)(&(spi_ptr->TDR));
	#endif //old version reserved
	
	return 0;
}

/**
 *@brief enable or disable remainder mode
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] remainder_en: whether set the remainder mode or not.
 *				- true, enable the remainder mode
 *				- false, disable the remainder mode
 *@return    none
 */
static inline void spi_drv_remainder_mode_set(SPI_TypeDef *spi_ptr, bool remainder_en)
{
#if 0
	spi_ptr->CFGR1.REMDR = (remainder_en ? 1UL : 0UL);	
#endif //old version reserved
}

/**
 *@brief config the loopback mode
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] loopback_en: whether set the loopback mode or not.
 *				- true, enable the loopback mode
 *				- false, disable the loopback mode
 *@return    none
 */
static inline void spi_drv_loopback_mode_set(SPI_TypeDef *spi_ptr, bool loopback_en)
{	
	#if 0
	spi_ptr->CFGR1.LPBKMOD = (loopback_en ? 1UL : 0UL);	
	#endif //old version reserved
}


/**
 *@brief enable or disable stall mode
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] stall_en:
 *				- true, enable the stall mode
 *				- false, disable the stall mode
 *@return    none
 */
static inline void spi_drv_stall_mode_set(SPI_TypeDef *spi_ptr, bool stall_en)
{
	#if 0
	spi_ptr->CFGR1.NO_STALL = (stall_en ? 1UL : 0UL);	
	#endif //old version reserved
}

/**
 *@brief enable or disable host mode
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] host_en: host request enable
 *				- true, enable the host mode
 *				- false, disable the host mode
 *@param[in] host_pol: host request polarity
 *@param[in] host_sel: host request selection
 *@return    none
 */
static inline void spi_drv_host_mode_set(SPI_TypeDef *spi_ptr, bool host_en, spi_host_polarity_e host_pol, spi_host_selection_e host_sel)
{
	#if 0
    spi_ptr->CFGR0.HOST_EN = (host_en ? 1UL : 0UL);
	spi_ptr->CFGR0.HOST_POL = ((SPI_HOST_ACTIVE_LOW == host_pol) ? 1UL : 0UL);
	spi_ptr->CFGR0.HOST_SEL = ((SPI_HOST_FROM_TRIGGER == host_sel) ? 1UL : 0UL);
	spi_ptr->CFGR0.HOST_DIR = 0U;	
	#endif //old version reserved
}

/**
 *@brief enable or disable the doze mode
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] doze_en:
 *				- true, enable the doze mode
 *				- false, disable the doze mode
 *@return    none
 */
static inline void spi_drv_doze_enable(SPI_TypeDef *spi_ptr, bool doze_en)
{
	#if 0
	spi_ptr->CR.DOZ_EN = (doze_en ? 1UL : 0UL);
	#endif //old version reserved
}

/**
 *@brief enable or disable the debug mode
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] debug_en:
 *				- true, enable the debug mode
 *				- false, disable the debug mode
 *@return    none.
 */
static inline void spi_drv_debug_enable(SPI_TypeDef *spi_ptr, bool debug_en)
{
	#if 0
	spi_ptr->CR.DBG_EN = (debug_en ? 1UL : 0UL);
	#endif //old version reserved
}

/**
 *@brief enable or disable the pcs automatic generation function
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] auto_pcs_en: enable or disable the pcs automatic generation function.
 *				- true, enable the pcs automatic generation function
 *				- false, disable the pcs automatic generation function
 *@return    none
 */
static inline void spi_drv_auto_pcs_enable(SPI_TypeDef *spi_ptr, bool auto_pcs_en)
{
	#if 0
	spi_ptr->CFGR1.AUTO_PCS = (auto_pcs_en ? 1UL : 0UL);
	#endif //old version reserved
}

/**
 *@brief enable or disable the circular FIFO
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] circule_fifo_en: enable or disable the circular FIFO
 *				- true, enable the circular FIFO
 *				- false, disable the circular FIFO
 *@return    none
 */
static inline void spi_drv_circule_fifo_enable(SPI_TypeDef *spi_ptr, bool circule_fifo_en)
{
	#if 0
	spi_ptr->CFGR0.CIR_FIFO = (circule_fifo_en ? 1UL : 0UL);
	#endif //old version reserved
}

/**
 *@brief wait for the SPI transfer complete flag assert.
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] wait_time: the time to wait
 *@return    none
 */
static inline void spi_drv_transfer_complete_polling(SPI_TypeDef *spi_ptr, uint32_t wait_time)
{
	#if 0
	(void)wait_till_timeout(&(spi_ptr->SR.w), SPI_SR_TRANS_FLAG_MASK, SPI_SR_TRANS_FLAG_MASK, wait_time);
	#endif //old version reserved
}


/*******************************************************************************
* Public SPI Control Functions
******************************************************************************/ 
void spi_drv_soft_reset(SPI_TypeDef *spi_ptr);
status_t spi_drv_disable(SPI_TypeDef *spi_ptr);
status_t spi_drv_master_slave_mode_set(SPI_TypeDef *spi_ptr, spi_master_slave_mode_e mode);
void spi_drv_flush_fifo_cmd_set(SPI_TypeDef *spi_ptr, bool flush_tx_fifo, bool flush_rx_fifo);
status_t spi_drv_status_flag_clear(SPI_TypeDef *spi_ptr, spi_status_flag_e status_flag);
status_t spi_drv_pcs_polarity_set(SPI_TypeDef *spi_ptr, spi_pcs_e which_pcs, spi_pcs_polarity_e pcs_polarity);
void spi_drv_all_interrupts_source_disable(SPI_TypeDef *spi_ptr);
void spi_drv_match_type_set(SPI_TypeDef *spi_ptr, 
								spi_match_fun_e match_type, 
								uint32_t match0,
								uint32_t match1,
								bool match_only_en);

/**
 *@brief get the RX FIFO Overrun interrupt flag status.
 *@param[in] spi_ptr: pointer to SPI base address
 *@return    RX FIFO Overrun interrupt flag
 */	
uint8_t spi_drv_interrupt_flag_rxfifo_or(SPI_TypeDef *spi_ptr)
{
	return (uint8_t)(spi_ptr->INTRST.RXFIFOORINT);
}

/**
 *@brief get the TX FIFO Underrun interrupt flag status.
 *@param[in] spi_ptr: pointer to SPI base address
 *@return    TX FIFO Underrun interrupt flag
 */	
uint8_t spi_drv_interrupt_flag_txfifo_ur(SPI_TypeDef *spi_ptr)
{
	return (uint8_t)(spi_ptr->INTRST.TXFIFOURINT);
}

/**
 *@brief get the RX FIFO Threshold interrupt flag status.
 *@param[in] spi_ptr: pointer to SPI base address
 *@return    RX FIFO Threshold interrupt flag
 */	
uint8_t spi_drv_interrupt_flag_rxfifo_th(SPI_TypeDef *spi_ptr)
{
	return (uint8_t)(spi_ptr->INTRST.RXFIFOINT);
}

/**
 *@brief get the TX FIFO Threshold interrupt flag status.
 *@param[in] spi_ptr: pointer to SPI base address
 *@return    TX FIFO Threshold interrupt flag
 */	
uint8_t spi_drv_interrupt_flag_txfifo_th(SPI_TypeDef *spi_ptr)
{
	return (uint8_t)(spi_ptr->INTRST.TXFIFOINT);
}

/**
 *@brief get the End of SPI Transfer interrupt flag status.
 *@param[in] spi_ptr: pointer to SPI base address
 *@return    End of SPI Transfer interrupt flag
 */	
uint8_t spi_drv_interrupt_flag_trans_end(SPI_TypeDef *spi_ptr)
{
	return (uint8_t)(spi_ptr->INTRST.ENDINT);
}

/**
 *@brief get the Slave Command Interrupt flag status.
 *@param[in] spi_ptr: pointer to SPI base address
 *@return    Slave Command Interrupt flag
 */	
uint8_t spi_drv_interrupt_flag_slave_cmd(SPI_TypeDef *spi_ptr)
{
	return (uint8_t)(spi_ptr->INTRST.SLVCMDINT);
}

/**
 *@brief Get the current transmission status of the SPI to understand whether the SPI is transmitting data
 *@param[in] spi_ptr: pointer to SPI base address
 *@return    0:SPI did not transmit data ---- 1:SPI transmit data
 */	
uint8_t spi_drv_trans_status(SPI_TypeDef *spi_ptr)
{
	return (uint8_t)(spi_ptr->STATUS.SPIACTIVE);
}

/**
 *@brief return whether the SPI is work in master mode
 *@param[in] *spi_ptr: pointer to SPI base address
 *@return    whether the SPI is work in master mode or not:
 *				- true, the SPI is work in master mode
 *				- false, the SPI is work in slave mode
 */
static inline bool spi_drv_ismaster(SPI_TypeDef *spi_ptr)
{
	return (bool)(0U == spi_ptr->TRANSFMT.SLVMOD);
}

/**
 *@brief Read the DATA register to get the data inside the RX FIFO
 *@param[in] spi_ptr: pointer to SPI base address
 *@return    Data retrieved from RX FIFO
 */	
uint8_t spi_drv_recv_data(SPI_TypeDef *spi_ptr)
{
	return (uint8_t)(spi_ptr->DATA.W);
}

/**
 *@brief Send the data to be transmitted to the TX FIFO by writing it to the DATA register
 *@param[in] spi_ptr: pointer to SPI base address
 *@return    Data send to TX FIFO
 */
void spi_drv_send_data(SPI_TypeDef *spi_ptr, uint8_t data)
{
	spi_ptr->DATA.W = (uint32_t)data;
}

/**
 *@brief Check if SPI RX FIFO is not empty
 *@param[in] spi_ptr: pointer to SPI base address
 *@return    1:have data --- 0: no data
 */
uint8_t spi_drv_rxfifo_not_empty(SPI_TypeDef *spi_ptr)
{
	return (uint8_t)(spi_ptr->STATUS.RXEMPTY == 0 ? 1 : 0);
}



void spi_drv_master_init(SPI_TypeDef *spi_ptr, SPI_MasterHandleTypeDef *SPI_config, uint32_t clock_freq);
void spi_drv_slave_init(SPI_TypeDef *spi_ptr, SPI_SlaveHandleTypeDef *SPI_config);
void spi_drv_interrupt_en(SPI_TypeDef *spi_ptr, uint8_t sel_int, bool en);
void spi_drv_interrupt_flag_clean(SPI_TypeDef *spi_ptr, uint8_t sel_int);

#endif/* SPI_H */

