 /*!
 * @file spi_hal_cfg.c
 */

#include "platform_common.h"
#include "spi_hal.h"

spi_master_config_t spi_master_init_config =
{
	.bits_per_sec = 2000000U,
	.which_pcs = SPI_PCS0,
	.pcs_polarity = SPI_PCS_ACTIVE_LOW,
	.is_pcs_continuous = false, 
    .frame_size = 32U,
	.spi_sck_phase = SPI_SCK_PHASE_1ST_EDGE,
	.spi_sck_polarity = SPI_SCK_ACTIVE_HIGH,
	.lsb_first = true,
	.call_back = NULL,
	.call_back_param = NULL,
	.spi_transfer_width = SPI_SINGLE_BIT,
	.byte_swap = false,
	.pin_cfg = SPI_SDI_IN_SDO_OUT,
	.interrupt_priority = 0U,
};

spi_slave_config_t spi_slave_init_config =
{
	.which_pcs = SPI_PCS0,
	.pcs_polarity = SPI_PCS_ACTIVE_LOW,
	.is_pcs_continuous = false, 
    .frame_size = 32U,
	.spi_sck_phase = SPI_SCK_PHASE_1ST_EDGE,
	.spi_sck_polarity = SPI_SCK_ACTIVE_HIGH,
	.lsb_first = true,
	.call_back = NULL,
	.call_back_param = NULL,
	.spi_transfer_width = SPI_SINGLE_BIT,
	.byte_swap = false,
	.pin_cfg = SPI_SDI_IN_SDO_OUT,
	.interrupt_priority = 0U,
};

