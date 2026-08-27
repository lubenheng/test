#include "platform_common.h"
#include "spi_hal.h"
#include "spi_reg.h"
#include "spi_type.h"


/*******************************************************************************
* Public SPI Control Functions
******************************************************************************/
/**
 *@brief reset the SPI internal logic and registers to their default settings
 *@param[in] *spi_ptr: pointer to SPI base address
 *@return    none
 */
void spi_drv_soft_reset(SPI_TypeDef *spi_ptr)
{
	#if 0
	
    /* Software reset the internal logic */
    spi_ptr->CR.SW_RST = 1U;
    /* Now bring the LPSPI module out of reset and clear CR register */
    spi_ptr->CR.SW_RST = 0U;
	#endif //old version reserved
	
}

/**
 *@brief disable the SPI module
 *@param[in] *spi_ptr: pointer to SPI base address
 *@return    whether disable the SPI successfully or not:
 *				- STATUS_SUCCESS, disable the SPI successfully
 *				- STATUS_BUSY, the SPI is busy
 */
status_t spi_drv_disable(SPI_TypeDef *spi_ptr)
{
	#if 0
    status_t STATUS;

    if ((spi_ptr->SR.w & SPI_SR_BSY_FLAG_MASK) == SPI_SR_BSY_FLAG_MASK)
    {
        STATUS = STATUS_BUSY;
    }
    else
    {
		spi_ptr->CR.SPEN = 0U;
        STATUS = STATUS_SUCCESS;
    }
    
    return STATUS;
	
	#endif //old version reserved
	
	
	return (status_t)0;
}

/**
 *@brief set the SPI work as master or slave
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] mode:
 *				- SPI_MASTER, SPI work in master mode
 *				- SPI_SLAVE, SPI work in slave mode
 *@return    whether set SPI work mode successfully or not:
 *				- STATUS_SUCCESS, set SPI work mode successfully
 *				- STATUS_ERROR, set SPI work mode fail
 */
status_t spi_drv_master_slave_mode_set(SPI_TypeDef *spi_ptr, spi_master_slave_mode_e mode)
{
	#if 0
	
	spi_ptr->CFGR1.MASTER = (uint32_t)mode;
	return STATUS_SUCCESS;
	
	#endif //old version reserved
	
	return (status_t)0;
}

/**
 *@brief flush SPI fifo
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] flush_tx_fifo: flush tx fifo or not
 *@param[in] flush_rx_fifo: flush rx fifo or not
 *@return    none
 */
void spi_drv_flush_fifo_cmd_set(SPI_TypeDef *spi_ptr, bool flush_tx_fifo, bool flush_rx_fifo)
{
	#if 0
	
	spi_ptr->CR.TFF_RST = (flush_tx_fifo ? 1UL : 0UL);
	spi_ptr->CR.RFF_RST = (flush_rx_fifo ? 1UL : 0UL);
	
	#endif

}

/**
 *@brief clear the SPI status flag
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] status_flag: the status flag to clear
 *@return    whether clear the spi status flag successfully or not:
 *				- STATUS_SUCCESS, clear the spi status flag successfully
 */
status_t spi_drv_status_flag_clear(SPI_TypeDef *spi_ptr, spi_status_flag_e status_flag)
{	
#if 0
    if (status_flag == SPI_ALL_STATUS)
    {
        spi_ptr->SR.w = (uint32_t)SPI_ALL_STATUS;
    }
    else
    {
		spi_ptr->SR.w = (spi_ptr->SR.w & (~(uint32_t)SPI_ALL_STATUS)) | (1UL << status_flag);
    }
#endif
	return STATUS_SUCCESS;
}

/**
 *@brief configure the desired SPI PCS polarity
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] which_pcs: select which PCS to program
 *@param[in] pcs_polarity: set PCS as active high or low
 *@return    whether configure the desired SPI PCS polarity successfully or not:
 *				- STATUS_SUCCESS, configure the desired SPI PCS polarity successfully
 */	
status_t spi_drv_pcs_polarity_set(SPI_TypeDef *spi_ptr, spi_pcs_e which_pcs, spi_pcs_polarity_e pcs_polarity)
{	
	#if 0
    if (pcs_polarity == SPI_PCS_ACTIVE_HIGH)
    {
		(spi_ptr->CFGR1.w) |= ((uint32_t)which_pcs << SPI_CFGR1_PCS_POL_POS);	
    }
    else
    {
		(spi_ptr->CFGR1.w) &= ~((uint32_t)which_pcs << SPI_CFGR1_PCS_POL_POS);
    }

	return STATUS_SUCCESS;
	#endif //old version reserved
	
	return (status_t)0;
}



/**
 *@brief diable all SPI interrupts source
 *@param[in] *spi_ptr: pointer to SPI base address
 *@return    none
 */
void spi_drv_all_interrupts_source_disable(SPI_TypeDef *spi_ptr)
{
#if 0
    spi_drv_interrupt_source_enable(spi_ptr, SPI_TX_DATA_FLAG, false);
    spi_drv_interrupt_source_enable(spi_ptr, SPI_RX_DATA_FLAG, false);	
    spi_drv_interrupt_source_enable(spi_ptr, SPI_WORD_COMPLETE, false);
    spi_drv_interrupt_source_enable(spi_ptr, SPI_FRAME_COMPLETE, false);	
    spi_drv_interrupt_source_enable(spi_ptr, SPI_TRANSFER_COMPLETE, false);
    spi_drv_interrupt_source_enable(spi_ptr, SPI_TRANSMIT_ERROR, false);
    spi_drv_interrupt_source_enable(spi_ptr, SPI_RECEIVE_ERROR, false);
    spi_drv_interrupt_source_enable(spi_ptr, SPI_DATA_MATCH, false);		
#endif
}



/**
 *@brief set SPI data match type 
 *@param[in] *spi_ptr: pointer to SPI base address
 *@param[in] match_type: spi match type
 *@param[in] match0: match word 0
 *@param[in] match1: match word 1
 *@param[in] match_only_en: whether set the match only function
 *@return    none
 */	
void spi_drv_match_type_set(SPI_TypeDef *spi_ptr, 
							spi_match_fun_e match_type, 
							uint32_t match0,
							uint32_t match1,
							bool match_only_en)
{
	#if 0
	if(match_type != SPI_MATCH_DISABLE)
	{
		spi_ptr->CFGR1.MAT_CFG = (uint32_t)match_type;
		spi_ptr->MATR0 = match0;
		spi_ptr->MATR1 = match1;
		spi_ptr->CFGR0.MAT_ONLY = (match_only_en ? 1UL : 0UL);
	}
	else
	{
		spi_ptr->CFGR1.MAT_CFG = (uint32_t)match_type;
		spi_ptr->CFGR0.MAT_ONLY = 0U;
	}
	#endif //old version reserved
}


/**
 *@brief SPI master Initialization
 *@param[in] spi_ptr: pointer to SPI base address
 *@param[in] SPI_HandleTypeDef: SPI configuration
 *@param[in] clock_freq: the current SPI function clock frequency
 *@return    none
 */	
void spi_drv_master_init(SPI_TypeDef *spi_ptr, SPI_MasterHandleTypeDef *SPI_config, uint32_t clock_freq)
{
	double sclk_div;

	//init spi and tx fifo and rx fifo
	spi_ptr->CTRL.SPIRST = 1;
	while(1 == (spi_ptr->CTRL.SPIRST));

	spi_ptr->CTRL.RXFIFORST = 1;
	while(1 == (spi_ptr->CTRL.RXFIFORST));

	spi_ptr->CTRL.TXFIFORST = 1;
	while(1 == (spi_ptr->CTRL.TXFIFORST));

	spi_ptr->INTREN.W = 0x00;
	spi_ptr->INTRST.W = 0xff;

	//transmit mode
	spi_ptr->TRANSFMT.CPOL = SPI_config->baseConfig.CPOL;	
	spi_ptr->TRANSFMT.CPHA = SPI_config->baseConfig.CPHA;
	spi_ptr->TRANSFMT.LSB = SPI_config->baseConfig.IsLsbFirst;

	spi_ptr->TRANSFMT.SLVMOD = 0;		//master

	//address/data mode
	spi_ptr->TRANSFMT.ADDRLEN = SPI_config->baseConfig.AddrLenByte;
	spi_ptr->TRANSFMT.DATALEN = SPI_config->baseConfig.DataLenBit;
	spi_ptr->TRANSFMT.DATAMERGE = SPI_config->baseConfig.IsDataMergeEn;
	//spi_ptr->TRANSFMT.MOSIBIDIR = SPI_config->baseConfig.IsMosiBiDirEn;


	//transmit config
	spi_ptr->TRANSCTRL.CMDEN = SPI_config->transConfig.IsCmdEn;
	spi_ptr->TRANSCTRL.ADDREN = SPI_config->transConfig.IsAddrEn;

	spi_ptr->CMD.CMD = SPI_config->transConfig.CmdCode;
	spi_ptr->ADDR.ADDR = SPI_config->transConfig.TransAddr;

	spi_ptr->TRANSCTRL.TRANSMODE = (uint8_t)(SPI_config->transConfig.TransMode);

	spi_ptr->ADDR.ADDR = SPI_config->transConfig.TransAddr;

	//FIFO TH SET
	spi_ptr->CTRL.TXTHRES = SPI_config->fifoDmaConfig.TxThres;
	spi_ptr->CTRL.RXTHRES = SPI_config->fifoDmaConfig.RxThres;

	sclk_div = SPI_config->timingConfig.SCLK_SPEED;
	sclk_div = ((double)clock_freq / sclk_div) / 2.0 - 1.0;
	spi_ptr->TIMING.SCLK_DIV = (uint8_t)sclk_div;

}


/**
 *@brief SPI slave Initialization
 *@param[in] spi_ptr: pointer to SPI base address
 *@param[in] SPI_HandleTypeDef: SPI configuration
 *@param[in] clock_freq: the current SPI function clock frequency
 *@return    none
 */
void spi_drv_slave_init(SPI_TypeDef *spi_ptr, SPI_SlaveHandleTypeDef *SPI_config)
{
	//init spi and tx fifo and rx fifo
	spi_ptr->CTRL.SPIRST = 1;
	while(1 == (spi_ptr->CTRL.SPIRST));

	spi_ptr->CTRL.RXFIFORST = 1;
	while(1 == (spi_ptr->CTRL.RXFIFORST));

	spi_ptr->CTRL.TXFIFORST = 1;
	while(1 == (spi_ptr->CTRL.TXFIFORST));

	spi_ptr->INTREN.W = 0x00;
	spi_ptr->INTRST.W = 0xff;

	//transmit mode
	spi_ptr->TRANSFMT.CPOL = SPI_config->baseConfig.CPOL;	
	spi_ptr->TRANSFMT.CPHA = SPI_config->baseConfig.CPHA;
	spi_ptr->TRANSFMT.LSB = SPI_config->baseConfig.IsLsbFirst;

	spi_ptr->TRANSFMT.SLVMOD = 1;		//must to need set frist
	spi_ptr->TRANSFMT.DATALEN = SPI_config->baseConfig.DataLenBit;
	spi_ptr->TRANSFMT.DATAMERGE = SPI_config->baseConfig.IsDataMergeEn;

	spi_ptr->CTRL.TXTHRES = SPI_config->fifoDmaConfig.TxThres;
	spi_ptr->CTRL.RXTHRES = SPI_config->fifoDmaConfig.RxThres;

	//spi_ptr->SLVST.READY = 1;

}


/**
 *@brief SPI Initialization
 *@param[in] spi_ptr: pointer to SPI base address
 *@param[in] sel_int: one or more interrupt:
 			--INT_RX_FIFO_OR--INT_TX_FIFO_UR--INT_RX_FIFO_TH
			--INT_TX_FIFO_TH--INT_TRANS_END--INT_SLV_CMD--
 *@param[in] en: enable or disable		
 *@return    none
 */	
void spi_drv_interrupt_en(SPI_TypeDef *spi_ptr, uint8_t sel_int, bool en)
{
	if(true == en){
		spi_ptr->INTREN.W |= sel_int;
	} else {
		spi_ptr->INTREN.W &= ~sel_int;
	}
}

/**
 *@brief clean one or more interrupt flag
 *@param[in] spi_ptr: pointer to SPI base address
 *@param[in] sel_int: one or more interrupt:
 			--INT_RX_FIFO_OR--INT_TX_FIFO_UR--INT_RX_FIFO_TH
			--INT_TX_FIFO_TH--INT_TRANS_END--INT_SLV_CMD--
 *@return    none
 */	
void spi_drv_interrupt_flag_clean(SPI_TypeDef *spi_ptr, uint8_t sel_int)
{
	spi_ptr->INTRST.W = (uint32_t)(sel_int);		//All interrupt flag bits are cleared by writing 1
}

/**
 *@brief SPI Initialization
 *@param[in] spi_ptr: pointer to SPI base address
 *@param[in] SPI_HandleTypeDef: SPI configuration
 *@return    none
 */	



