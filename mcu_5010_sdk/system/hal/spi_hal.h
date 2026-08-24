#ifndef SPI_HAL_H
#define SPI_HAL_H

#include "spi_type.h"

#define SPI_FREQ_MAX 0xFFFFFFFFU
#define SPI_DUMMY_VALUE 0xFFFFFFFFU
#define SPI_MASTER_DEFAULT_BITS_PER_SEC 2000000U
#define SPI_DEFAULT_FRAME_SIZE 32U

#define SPI_DEFAULT_DMA_TX_WAITTIME 100U
#define SPI_FOUR_BIT_PIN_CFG 0x0cU
#define SPI_TWO_BIT_PIN_CFG 0x04U

#define SPI_MASTER_DEFAULT_TX_WATER_MARK 2U
#define SPI_MASTER_DEFAULT_RX_WATER_MARK 0U
#define SPI_SLAVE_DEFAULT_TX_WATER_MARK 3U
#define SPI_SLAVE_DEFAULT_RX_WATER_MARK 0U

#define SPI_FRAME_SIZE_MIN 8U
#define SPI_FRAME_SIZE_MAX 4096U
#define SPI_SCALER_LOW 0U
#define	SPI_SCALER_HIGH 256U

/**
 * @defgroup ingroup_spi_hal Spi Hal
 * @ingroup group_spi_hal
 * @details This section describes the programming interface of the spi module.
 * @{
 */


/** The type of the event which occurred when the callback was invoked */
typedef enum 
{
	SPI_EVENT_RX_FULL = 0U, 	/*!< 0, the spi receive the required number of data */
	SPI_EVENT_TX_EMPTY, 	/*!< 0, the spi send the required number of data */
	SPI_EVENT_TANSFER_FINISH, 	/*!< 1, the spi transfer data finish */
	SPI_EVENT_TRANSMIT_ERROR,	/*!< 2, FIFO underrun occur */
	SPI_EVENT_RECEIVE_ERROR,	/*!< 3, FIFO overrun occur */
	SPI_EVENT_DATA_MATCH,       /*!< 4, data match happened */ 
/* OCS_Reserved_Start */
	SPI_EVENT_WORD_COMPLETE,
	SPI_EVENT_FRAME_COMPLETE,
/* OCS_Reserved_End */
}spi_event_e;

/** SPI status flags */
typedef enum
{
    SPI_TX_DATA_FLAG      = 0x00U,       /*!< TX data flag */
    SPI_RX_DATA_FLAG      = 0x01U,       /*!< RX data flag */
    SPI_WORD_COMPLETE     = 0x08U,       /*!< word complete flag */
    SPI_FRAME_COMPLETE    = 0x09U,       /*!< frame complete flag */
    SPI_TRANSFER_COMPLETE = 0x0aU,       /*!< transfer complete flag */
    SPI_TRANSMIT_ERROR    = 0x0bU,       /*!< transmit error flag (FIFO underrun) */
    SPI_RECEIVE_ERROR     = 0x0cU,       /*!< receive error flag (FIFO overrun) */
    SPI_DATA_MATCH        = 0x0dU,       /*!< data match flag */
    SPI_MODULE_BUSY       = 0x18U,       /*!< module busy flag */
    SPI_ALL_STATUS        = 0x00003F00U, /*!< used for clearing all w1c status flags */
} spi_status_flag_e;

/** SPI master or slave configuration */
typedef enum
{
	SPI_SLAVE  = 0U, /*!< 0, SPI work in slave mode */
    SPI_MASTER = 1U, /*!< 1, SPI work in master mode */
} spi_master_slave_mode_e;

/** SPI pin (SDO and SDI) configuration */
typedef enum
{
    SPI_SDI_IN_SDO_OUT = 0U,     /*!< 0, SPI SDI input, SDO output */
    SPI_SDI_IN_OUT     = 1U,     /*!< 1, SDI is used for both input and output data */
    SPI_SDO_IN_OUT     = 2U,     /*!< 2, SDO is used for both input and output data */
    SPI_SDI_OUT_SDO_IN = 3U      /*!< 3, SPI SDO input, SDI output */
} spi_pin_config_e;

/** SPI transfer width configuration */
typedef enum
{
    SPI_SINGLE_BIT = 0U, /*!< 0, 1-bit shift at a time, data out on SDO, in on SDI (normal mode) */
    SPI_TWO_BIT = 1U,    /*!< 1, 2-bits shift out on SDO/SDI and in on SDO/SDI */
    SPI_FOUR_BIT = 2U    /*!< 2, 4-bits shift out on SDO/SDI/PCS[3:2] and in on SDO/SDI/PCS[3:2] */
} spi_transfer_width_e;

/** SPI peripheral chip select configuration */
typedef enum
{
    SPI_PCS0 = 1U, /*!< 1, PCS[0] */
    SPI_PCS1 = 2U, /*!< 2, PCS[1] */
    SPI_PCS2 = 4U, /*!< 4, PCS[2] */
    SPI_PCS3 = 8U, /*!< 8, PCS[3] */
    SPI_PCS_MAX,   /*!< invalid PCS */
} spi_pcs_e;

/** SPI PCS polarity configuration */
typedef enum
{
    SPI_PCS_ACTIVE_LOW  = 0U, /*!< 0, PCS is active low (idles high) */
    SPI_PCS_ACTIVE_HIGH = 1U, /*!< 1, PCS is active high (idles low) */		
} spi_pcs_polarity_e;

/** SPI clock signal (SCK) phase configuration */
typedef enum
{
    SPI_SCK_PHASE_1ST_EDGE = 0U, /*!< 0, data captured on SCK 1st edge, changed on 2nd */
    SPI_SCK_PHASE_2ST_EDGE = 1U  /*!< 1, data changed on SCK 1st edge, captured on 2nd */
} spi_sck_phase_e;

/** SPI clock signal (SCK) polarity configuration */
typedef enum
{
    SPI_SCK_ACTIVE_HIGH = 0U, /*!< 0, SCK is active high (idles low) */
    SPI_SCK_ACTIVE_LOW  = 1U  /*!< 1, SCK is active low (idles high) */
} spi_sck_polarity_e;

/** SPI data match configuration */
typedef enum
{
    SPI_MATCH_DISABLE = 0U,                            /*!< 0, disabled match function */
    SPI_FIST_WORD_MATCH0_OR_MATCH1  = 2U,              /*!< 2, the first received word equals to MATCH0 OR MATCH1 */
    SPI_ANY_WORD_MATCH0_OR_MATCH1  = 3U,               /*!< 3, any received word equals to MATCH0 OR MATCH1 */   
    SPI_FIST_MATCH0_AND_SECOND_MATCH1  = 4U,           /*!< 4, the first received word equals to MATCH0 and the second received word equals to MATCH1*/  
    SPI_ANY_MATCH0_AND_NEXT_MATCH1  = 5U,              /*!< 5, any received word equals to MATCH0 and the next received word equals to MATCH1 */     
    SPI_FIST_AND_MATCH1_EQUAL_MATCH0_AND_MATCH1  = 6U, /*!< 6, the result of the first received word AND MATCH1 is equal to the result of MATCH0 AND MATCH1 */  
    SPI_ANY_AND_MATCH1_EQUAL_MATCH0_AND_MATCH1  = 7U,  /*!< 7, any received word AND MATCH1 is equal to the result of MATCH0 ANDMATCH1 */  
} spi_match_fun_e;


 /** host request polarity configuration */
 typedef enum
 {
 	SPI_HOST_ACTIVE_HIGH = 0U, /*!< 0, host request is active high (idles low) */
	SPI_HOST_ACTIVE_LOW        /*!< 1, host request is active low (idles high) */
 } spi_host_polarity_e;

 /** host request source selection */
 typedef enum
 {
	SPI_HOST_FROM_HREQ = 0U, /*!< 0, host request input is from the HREQ pin */
	SPI_HOST_FROM_TRIGGER    /*!< 1, host request input is from the external trigger pin */
 } spi_host_selection_e;

 /** type of error reported by SPI */
 typedef enum
 {
	 SPI_TRANSFER_OK = 0U, /*!< 0, transfer OK */	 
	 SPI_TRANSMIT_FAIL,    /*!< 1, error during transmission */		 
	 SPI_RECEIVE_FAIL      /*!< 2, error during reception */		 
 } spi_transfer_status_e;

/** callback for all peripherals which support SPI features */
typedef void (*spi_callback_t)(void *driver_state, spi_event_e event, void *user_data);

/** definition of interrupt handling function type */
typedef void (*SPI_INT_HANDLE)(spi_instance_e instance);

/** definition of pcs negative handling function type */
typedef void (*SPI_PCS_NEGATIVE)(void);

/** definition of pcs active handling function type */
typedef void (*SPI_PCS_ACTIVE)(void);


/** SPI runtime state structure */
typedef struct
{
	uint16_t bytes_per_frame;              /*!< number of bits per frame */
	bool is_pcs_continuous;                /*!< option to keep chip select asserted until transfer complete */
	volatile bool is_transfer_in_progress; /*!< true if there is an active transfer */
	const uint8_t *tx_buff;                /*!< the buffer from which transmitted bytes are taken */
	uint8_t *rx_buff;                      /*!< the buffer into which received bytes are placed */
	volatile uint16_t tx_count;            /*!< number of bytes remaining to send  */
	volatile uint16_t rx_count;            /*!< number of bytes remaining to receive */
	volatile uint16_t tx_frame_cnt;        /*!< number of bytes from current frame which were already sent */
	volatile uint16_t rx_frame_cnt;        /*!< number of bytes from current frame which were already received */
	volatile bool lsb_first;               /*!< true if first bit is LSB and false if first bit is MSB */
	spi_transfer_status_e status;          /*!< the current status of SPI */
	spi_callback_t call_back;              /*!< select the callback to transfer complete */
	void *call_back_param;                 /*!< select additional callback parameters if it's necessary */
	uint32_t dummy;                        /*!< This field is used for the cases when TX is NULL and LPSPI is in DMA mode */
	volatile uint16_t tx_valid_count;               /*!< the number of bytes that need to be sent in this transmission */
	volatile uint16_t rx_valid_count;               /*!< the number of bytes that need to be received in this transmission */
	bool circule_fifo_en;                  /*!< true if send data repeatedly */
	volatile uint16_t circule_byte;                 /*!< number of bytes of data sent repeatedly */
} spi_state_t;

/** SPI master configuration structure */
typedef struct
{
	uint32_t bits_per_sec;                   /*!< baud rate in bits per second */    
	spi_pcs_e which_pcs;                     /*!< select which PCS to use */
	spi_pcs_polarity_e pcs_polarity;         /*!< PCS polarity */
	bool is_pcs_continuous;                  /*!< keep PCS asserted until transfer complete */
	uint16_t frame_size;                     /*!< number of bits/frame, minimum is 8-bits */
	spi_sck_phase_e spi_sck_phase;           /*!< select which phase of clock to capture data */
	spi_sck_polarity_e spi_sck_polarity;     /*!< select clock polarity */
	bool lsb_first;                          /*!< option to transmit LSB first */
	spi_callback_t call_back;                /*!< select the callback to transfer complete */
	void *call_back_param;                   /*!< select additional callback parameters if it's necessary */
	spi_transfer_width_e spi_transfer_width; /*!< select SPI transfer width */ 
	bool byte_swap;                          /*!< option to byte swap */
	spi_pin_config_e pin_cfg;                /*!< SPI pin (SDO and SDI) configuration */
	uint32_t interrupt_priority;			 /*!< SPI interrupt poriority*/
} spi_master_config_t;

/** SPI slave configuration structure */
typedef struct
{
	spi_pcs_e which_pcs;                     /*!< select which PCS to use */
	spi_pcs_polarity_e pcs_polarity;         /*!< PCS polarity */
	uint16_t frame_size;                     /*!< number of bits/frame, minimum is 8-bits */
	spi_sck_phase_e spi_sck_phase;           /*!< select which phase of clock to capture data */
	spi_sck_polarity_e spi_sck_polarity;     /*!< select clock polarity */
	bool lsb_first;                          /*!< option to transmit LSB first */
	spi_callback_t call_back;                /*!< select the callback to transfer complete */
	void *call_back_param;                   /*!< select additional callback parameters if it's necessary */
	bool is_pcs_continuous;                  /*!< keep PCS asserted until transfer complete */
	spi_transfer_width_e spi_transfer_width; /*!< select SPI transfer width */ 
	bool byte_swap;                          /*!< option to byte swap */
	spi_pin_config_e pin_cfg;                /*!< SPI pin (SDO and SDI) configuration */
	uint32_t interrupt_priority;			 /*!< SPI interrupt poriority*/
} spi_slave_config_t;


#include "spi_hal_cfg.h"


/*******************************************************************************
* Public SPI Control Functions
******************************************************************************/
void spi_master_default_config_get(spi_master_config_t *config);
status_t spi_master_init(spi_instance_e spi_instance, spi_master_config_t *config);
status_t spi_master_deinit(spi_instance_e spi_instance);
status_t spi_master_delay_set(spi_instance_e spi_instance, 
								uint8_t pcs_to_sck, 
								uint8_t sck_to_pcs, 
								uint8_t pcs_to_cmd, 
								uint8_t sck_to_sck, 
								uint8_t sck_stp,
								uint8_t sck_hld);
status_t spi_master_configure_bus(spi_instance_e spi_instance, 
									spi_master_config_t *config, 
									uint32_t *calculated_baudrate);
status_t spi_pcs_set(spi_instance_e spi_instance, spi_pcs_e which_pcs, spi_pcs_polarity_e pcs_polarity);
status_t spi_master_transfer(spi_instance_e spi_instance,
								const uint8_t *send_buffer,
								uint8_t *receive_buffer,
								uint16_t transfer_byte_count);
status_t spi_master_transfer_data_ex(spi_instance_e spi_instance,
										const uint8_t *send_buffer, 
										uint8_t *receive_buffer, 
										uint16_t transfer_byte_count, 
										uint16_t tx_valid_count, 
										uint16_t rx_valid_count);
status_t spi_master_transfer_status_get(spi_instance_e spi_instance, uint32_t *bytes_remained);
void spi_slave_default_config_get(spi_slave_config_t *config);
status_t spi_slave_init(spi_instance_e spi_instance, spi_slave_config_t *config);
status_t spi_slave_deinit(spi_instance_e spi_instance);
status_t spi_slave_transfer(spi_instance_e spi_instance,
								const uint8_t *send_buffer,
								uint8_t *receive_buffer,
								uint16_t transfer_byte_count);
status_t spi_slave_transfer_status_get(spi_instance_e spi_instance, uint32_t *bytes_remained);
bool spi_transfer_wait(spi_instance_e spi_instance, uint32_t wait_time);
void spi_interrupt_source_enable(spi_instance_e spi_instance, spi_status_flag_e interrupt_src, bool enable);
void spi_host_mode_set(spi_instance_e spi_instance, 
						bool host_en, 
						spi_host_polarity_e host_pol, 
						spi_host_selection_e host_sel);
void spi_match_type_set(spi_instance_e spi_instance, 
						spi_match_fun_e match_type, 
						uint32_t match0, 
						uint32_t match1, 
						bool match_only);
void spi_stall_mode_set(spi_instance_e spi_instance, func_switch_e stall_en);
void spi_remainder_mode_set(spi_instance_e spi_instance, bool remainder_en);
void spi_loopback_mode_set(spi_instance_e spi_instance, bool loopback_en);
void spi_auto_pcs_enable(spi_instance_e spi_instance, bool auto_pcs_en);
void spi_circule_fifo_enable(spi_instance_e spi_instance, func_switch_e cir_fifo, uint16_t cir_bytes);
void spi_callback_install(spi_instance_e spi_instance, spi_callback_t callback, void *callback_param);
void spi_interrupt_handle_cfg(spi_instance_e spi_instance, SPI_INT_HANDLE handle);

status_t spi_master_transfer_manual_pcs(spi_instance_e spi_instance,
								const uint8_t *send_buffer,
								uint8_t *receive_buffer,
								uint16_t transfer_byte_count,
								SPI_PCS_NEGATIVE pcs_negative_function, 
								SPI_PCS_ACTIVE pcs_active_function);

void spi_master_irq_handler_manual_pcs(spi_instance_e spi_instance);

void spi_slave_buffer_set(spi_instance_e spi_instance,
                                const uint8_t *send_buffer,
                                uint8_t *receive_buffer,
                                uint16_t transfer_byte_count);
void spi_master_buffer_set(spi_instance_e spi_instance,
                                const uint8_t *send_buffer,
                                uint8_t *receive_buffer,
                                uint16_t transfer_byte_count);

/** @}*/ /* End of addtogroup ingroup_spi_hal */

extern SPI_HandleTypeDef SPI0_config;
extern SPI_HandleTypeDef SPI1_config;

void spi_hal_master_send_recv_data(spi_instance_e spi_instance, transmit_data *send_data_buffer, transmit_data *recv_data_buffer);
void spi_hal_master_init(spi_instance_e spi_instance, SPI_MasterHandleTypeDef *spi_hal_config);
void spi_hal_slave_init(spi_instance_e spi_instance, SPI_SlaveHandleTypeDef *spi_hal_config);
void spi_hal_slave_send_data(spi_instance_e spi_instance, transmit_data *send_data_buffer);
void spi_hal_slave_send_many_data(spi_instance_e spi_instance, uint8_t data_len, uint32_t data);
void spi_hal_master_recv_data(spi_instance_e spi_instance, transmit_data *recv_data_buffer);
void spi_hal_master_send_data(spi_instance_e spi_instance, transmit_data *send_data_buffer);

void spi_hal_master_change_cmd(spi_instance_e spi_instance, const uint8_t en, const uint8_t cmd);
void spi_hal_master_change_addr(spi_instance_e spi_instance, const uint8_t en, const uint8_t addr_len, const uint32_t addr);
void spi_hal_transmode_change(spi_instance_e spi_instance, uint8_t mode, uint8_t dummy_cnt);
void spi_hal_slave_reset(spi_instance_e spi_instance);
void slave_data_buffer(void);
void spi_hal_slave_recv_buf_set(uint8_t *data_buf);

#endif/* SPI_HAL_H */

