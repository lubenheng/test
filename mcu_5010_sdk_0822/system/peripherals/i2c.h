#ifndef I2C_H
#define I2C_H

#include "i2c_type.h"

/*!
 * @file i2c.h
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 11.4, A conversion should not be performed between a pointer to object and an integer type.
 * A pointer should not be converted into an integer.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Dir 4.9, A function should be used in preference to a function-like macro where they are interchangeable.
 * Use macro to speed up program processing
 *
 */

/*******************************************************************************
* Public I2C MASTER static inline Functions
******************************************************************************/ 
/**
 *@brief        : return the number of words in the transmit FIFO
 *@param[in]    : i2c_ptr: point to i2c base address
 *@return        : the number of tx count in the transmit FIFO
 */
static inline uint16_t i2cm_drv_txfifo_get(I2C_TypeDef *i2c_ptr)      //use
{
//    return (uint16_t)(i2c_ptr->I2CM_FIFO_STAT.TX_COUNT);
	return (uint16_t)(i2c_ptr->IC_TXFLR.TXFLR);
//	return 0;
}

/**
 *@brief        : return the number of words in the received FIFO
 *@param[in]    : i2c_ptr: point to i2c base address
 *@return        : the number of rx count in the received FIFO
 */
static inline uint16_t i2cm_drv_rxfifo_get(I2C_TypeDef *i2c_ptr)      //use
{
//    return (uint16_t)(i2c_ptr->I2CM_FIFO_STAT.RX_COUNT);
	return (uint16_t)(i2c_ptr->IC_RXFLR.RXFLR);
//	return 0;
}

/**
 *@brief        : This function reset all I2C hardware FIFO.
 *@param[in]    : i2c_ptr: point to i2c base address
 *@return        : none
 */
static inline void i2cm_drv_reset_fifo(I2C_TypeDef *i2c_ptr)
{    
//    i2c_ptr->I2CM_CTRL.RESET_TXFIFO = I2C_FUNC_ENABLE;
//    i2c_ptr->I2CM_CTRL.RESET_RXFIFO = I2C_FUNC_ENABLE;

	//
}

/**
 *@brief        : stores cmds and data in the transmit FIFO and increments the FIFO write pointer
 *@param[in]    : i2c_ptr: point to i2c base address
 *@param[in]    : cmd: the command for the I2C master, select from i2c_master_cmd_e.
 *@param[in]    : data: the data for the I2C master
 *@return        : none
 */
static inline void i2cm_drv_tdata_send(I2C_TypeDef *i2c_ptr, i2c_master_cmd_e cmd, uint8_t data)      //use
{
//    i2c_ptr->I2CM_TDATA.w = (((uint32_t)cmd & (uint32_t)0xFF) << 8U) + (uint32_t)data;
	i2c_ptr->IC_DATA_CMD.w = (((uint32_t)(cmd) & (uint32_t)0xff) << 8U) + (uint32_t)(data);
}

/**
 *@brief        : This function get the I2C master watermark.
 *@param[in]    : i2c_ptr: point to i2c base address
 *@return        : the i2c current RX_WATERMARK
 */
static inline uint8_t i2cm_drv_rxwatermark_get(I2C_TypeDef *i2c_ptr)
{
//    return (uint8_t)(i2c_ptr->I2CM_FIFO_CTRL.RX_WATERMARK);
	return 0;
}

/**
 *@brief        : This function de-initializes the I2C master module.
 *@param[in]    : i2c_ptr: point to i2c base address
 *@return        : none
 */
static inline void i2cm_drv_deinit(I2C_TypeDef *i2c_ptr)
{    
//    i2c_ptr->I2CM_CTRL.SOFT_RESET_REQ = I2C_FUNC_ENABLE;
//    i2c_ptr->I2CM_CTRL.MASTER_EN = I2C_FUNC_DISABLE;

	
}

/**
 *@brief        : get the rx data register address 
 *@param[in]    : i2c_ptr: point to i2c base address
 *@return        : the rx data register address
 */    
static inline uint32_t i2cm_drv_rx_address_get(I2C_TypeDef *i2c_ptr)
{
//    return (uint32_t)(&(i2c_ptr->I2CM_RDATA));
	return 0;
}

/**
 *@brief        : get the tx data register address 
 *@param[in]    : i2c_ptr: point to i2c base address
 *@return        : the tx data register address
 */    
static inline uint32_t i2cm_drv_tx_address_get(I2C_TypeDef *i2c_ptr)
{
//    return (uint32_t)(&(i2c_ptr->I2CM_TDATA));
	return 0;
}

/**
 *@brief        : get the current interrupt status
 *@param[in]    : i2c_ptr: point to the i2c base addr
 *@return        : the i2c interrupt status
 */    
static inline uint32_t i2cm_drv_intsts_get(I2C_TypeDef *i2c_ptr)      //use
{
//    return i2c_ptr->I2CM_STAT.w;
//	return 0;
	return (uint32_t)(i2c_ptr->IC_INTR_STAT.w);
}

/**
 *@brief        : clear the interrupt flag
 *@param[in]    i2c_ptr: point to the i2c base addr
 *@param[in]    clear_sts: the status need to clear
 *@return        : none
 */    
static inline void i2cm_drv_intsts_clear(I2C_TypeDef *i2c_ptr, uint32_t clear_sts)
{
	
//    i2c_ptr->I2CM_STAT.w = clear_sts; 
//    __DSB();
}

/**
 *@brief      : get the current interrupt enable
 *@param[in]  : i2c_ptr: point to the i2c base addr
 *@return     :    the i2c interrupt enable
 */    
static inline uint32_t i2cm_drv_inten_get(I2C_TypeDef *i2c_ptr)      //use
{
//    return i2c_ptr->I2CM_INT_EN.w;
//	return 0;
	return (uint32_t)(i2c_ptr->IC_INTR_MASK.w);
}

/*******************************************************************************
* Public I2C SLAVE static inline Functions
******************************************************************************/ 
/**
 *@brief        : get the current interrupt status
 *@param[in]    : i2c_ptr: point to the i2c base addr
 *@return        : the i2c interrupt status
 */    
static inline uint32_t i2cs_drv_intsts_get(I2C_TypeDef *i2c_ptr)
{
//    return (uint32_t)(i2c_ptr->I2CS_STAT.w);
	return 0;
}

/**
 *@brief        : clear the interrupt flag
 *@param[in]    : i2c_ptr: point to the i2c base addr
 *@param[in]    : clear_sts: the status need to clear
 *@return        : none
 */    
static inline void i2cs_drv_intsts_clear(I2C_TypeDef *i2c_ptr, uint32_t clear_sts)
{
//    i2c_ptr->I2CS_STAT.w = clear_sts; 
//    __DSB();
}

/**
 *@brief        : get the current interrupt enable
 *@param[in]    : i2c_ptr: point to the i2c base addr
 *@return        : the i2c interrupt enable
 */    
static inline uint32_t i2cs_drv_inten_get(I2C_TypeDef *i2c_ptr)
{
//    return i2c_ptr->I2CS_INT_EN.w;
	return 0;
}

/**
 *@brief        : This function de-initializes the I2C slave module.
 *@param[in]    : i2c_ptr: point to i2c base address
 *@return        : none
 */
static inline void i2cs_drv_deinit(I2C_TypeDef *i2c_ptr)
{    
//    i2c_ptr->I2CS_CTRL.SOFT_RESET_REQ = I2C_FUNC_ENABLE;

//    i2c_ptr->I2CS_CTRL.SLAVE_EN = I2C_FUNC_DISABLE;
}

/**
 *@brief        : get the received slave address 
 *@param[in]    : i2c_ptr: point to the i2c base addr
 *@return        : the received slave address 
 */    
static inline uint32_t i2cs_drv_slave_addr_get(I2C_TypeDef *i2c_ptr)
{
//    return (uint32_t)(i2c_ptr->I2CS_ADDR_STAT.RX_ADDR);
	return 0;
}

/**
 *@brief        : write the whole word instead of byte, 
 *                    assembly instruction may read the word before write and cause hardfault
 *@param[in]    : i2c_ptr: point to the i2c base addr
 *@param[in]    : data: the data to be sent
 *@return        : none
 */    
static inline void i2cs_drv_txdata_set(I2C_TypeDef *i2c_ptr, uint32_t data)
{
//     i2c_ptr->I2CS_TDATA.w = data;
}

/**
 *@brief        : get the rx data
 *@param[in]    : i2c_ptr: point to the i2c base addr
 *@return        : the received data 
 */    
static inline uint8_t i2cs_drv_rxdata_get(I2C_TypeDef *i2c_ptr)
{
//    return (uint8_t)(i2c_ptr->I2CS_RDATA.RECEIVE_DATA);
	return 0;
}

/**
 *@brief        : get the rx data register address 
 *@param[in]    : i2c_ptr: point to the i2c base addr
 *@return        : the rx data register address
 */    
static inline uint32_t i2cs_drv_rx_address_get(I2C_TypeDef *i2c_ptr)
{
//    return (uint32_t)(&(i2c_ptr->I2CS_RDATA));
	return 0;
}

/**
 *@brief        : get the tx data register address 
 *@param[in]    : i2c_ptr: point to the i2c base addr
 *@return        : the tx data register address
 */    
static inline uint32_t i2cs_drv_tx_address_get(I2C_TypeDef *i2c_ptr)
{
//    return (uint32_t)(&(i2c_ptr->I2CS_TDATA));
	return 0;
}

/**
 *@brief        : enable tx fifo for slave
 *@param[in]    : i2c_ptr: point to the i2c base addr
 *@param[in]    : tx_watermark: tx watermark for fifo
 *@return       : none
 */    
static inline void i2cs_drv_tx_fifo_enable(I2C_TypeDef *i2c_ptr, uint32_t tx_watermark)
{
//    i2c_ptr->I2CS_FIFO_CTRL.w = (I2C_I2CS_FIFO_CTRL_TX_BYPASS_SET(I2C_FUNC_DISABLE) |
//                                 I2C_I2CS_FIFO_CTRL_TX_WATERMARK_SET(tx_watermark));
}

/*******************************************************************************
* Public I2C MASTER Control Functions
******************************************************************************/ 
void i2cm_drv_int_cfg(I2C_TypeDef *i2c_ptr, uint32_t interrupt, bool enable);
void i2cm_drv_dmatch_cfg(I2C_TypeDef *i2c_ptr, const i2c_master_dmatch_config_t *dmatch_config);
void i2cm_drv_pinlow_cfg(I2C_TypeDef *i2c_ptr, uint32_t timeout);
void i2cm_drv_watermark_set(I2C_TypeDef *i2c_ptr, uint8_t value, i2c_watermark_e watermark);
void i2cm_drv_hreq_cfg(I2C_TypeDef *i2c_ptr, bool enable, const i2c_master_hreq_config_t *hreq_config);
void i2cm_drv_test_send_data(I2C_TypeDef *i2c_ptr, uint8_t cmd, uint8_t data);
status_t i2cm_drv_init(I2C_TypeDef *i2c_ptr, const i2c_master_config_t *i2c_master_config, uint32_t clock_freq);
void i2cm_drv_slv_addr(I2C_TypeDef *i2c_ptr, i2c_master_msg_t *i2c_master_msg);
/*******************************************************************************
* Public I2C SLAVE Control Functions
******************************************************************************/ 
void i2cs_drv_filter_cfg(I2C_TypeDef *i2c_ptr, bool enable, uint8_t filter_sda, uint8_t filter_scl);
void i2cs_drv_int_cfg(I2C_TypeDef *i2c_ptr, uint32_t interrupt, bool enable);
status_t i2cs_drv_init(I2C_TypeDef *i2c_ptr, const i2c_slave_config_t *i2c_slave_config);

#endif /*I2C_H*/

