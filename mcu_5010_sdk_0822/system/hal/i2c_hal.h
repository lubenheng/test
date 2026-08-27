#ifndef I2C_HAL_H
#define I2C_HAL_H

/**
 * @defgroup ingroup_i2c_hal I2C Hal
 *
 * @ingroup group_i2c_hal
 *
 * @details This section describes the programming interface of i2c.
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 18.7, Flexible array members shall not be declared
 *
 * @{
 */
 
 #include "i2c_type.h"
 

void i2cm_buffer_init(i2c_master_msg_t *i2c_master_msg);
void i2cm_send_data(uint32_t i2c_instance, i2c_master_msg_t *i2c_master_msg);
void i2cm_receive_data(uint32_t i2c_instance, i2c_master_msg_t* i2c_master_msg);
void i2cm_dmatch_init(uint32_t i2c_instance, const i2c_master_dmatch_config_t *dmatch_config);
uint32_t i2cm_tx_address_get(uint32_t i2c_instance);
uint32_t i2cm_rx_address_get(uint32_t i2c_instance);
status_t i2c_master_init(uint32_t i2c_instance, const i2c_master_config_t* i2c_master_config);
void i2cm_end_transfer(uint32_t i2c_instance, bool stop_cmd, bool reset_fifo);
void i2cm_deinit(uint32_t i2c_instance);
void i2cm_pinlow_init(uint32_t i2c_instance, uint32_t timeout);

void i2cs_buffer_init(i2c_slave_msg_t *i2c_slave_msg);
uint32_t i2cs_tx_address_get(uint32_t i2c_instance);
uint32_t i2cs_rx_address_get(uint32_t i2c_instance);
status_t i2c_slave_init(uint32_t i2c_instance, const i2c_slave_config_t *i2c_slave_config, i2c_slave_msg_t *i2c_slave_msg);
void i2cs_deinit(uint32_t i2c_instance);
void i2cs_filter_cfg(uint32_t i2c_instance, bool enable, uint8_t filter_sda, uint8_t filter_scl);

/** @}*/ /* End of addtogroup ingroup_i2c_hal */

#endif /*I2C_HAL_H*/

