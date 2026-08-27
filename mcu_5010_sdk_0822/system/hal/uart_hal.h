#ifndef UART_HAL_H
#define UART_HAL_H


/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define UART_SHIFT (16U)
#define UART_BAUD_REG_ID (1U)
#define UART_STAT_REG_ID (2U)
#define UART_CTRL_REG_ID (3U)
#define UART_DATA_REG_ID (4U)
#define UART_MATCH_REG_ID (5U)
#define UART_FLOWCFG_REG_ID (6U)
#define UART_FIFO_REG_ID (7U)
#define UART_AUTOBAUD_REG_ID (8U)

/** UART status flags */
typedef enum
{
    UART_TX_DATA_REG_EMPTY          = (((uint32_t)UART_STAT_REG_ID << (uint32_t)UART_SHIFT)
                                        | 23U),
                                        /*!< TX data register empty flag, sets when TX buffer is empty */
    UART_TX_COMPLETE                = (((uint32_t)UART_STAT_REG_ID << (uint32_t)UART_SHIFT)
                                        | 22U),
                                        /*!< transmission complete flag, sets when transmission activity complete */
    UART_RX_DATA_REG_FULL           = (((uint32_t)UART_STAT_REG_ID << (uint32_t)UART_SHIFT)
                                        | 21U),
                                        /*!< RX data register full flag, sets when the receive data buffer is full */
    UART_IDLE_LINE_DETECT           = (((uint32_t)UART_STAT_REG_ID << (uint32_t)UART_SHIFT)
                                        | 20U),
                                        /*!< idle line detect flag, sets when idle line detected */
    UART_RX_OVERRUN                 = (((uint32_t)UART_STAT_REG_ID << (uint32_t)UART_SHIFT)
                                        | 19U),
                                        /*!< RX overrun sets if new data is received before data is read */
    UART_MATCH_ADDR_ONE             = (((uint32_t)UART_STAT_REG_ID << (uint32_t)UART_SHIFT)
                                        | 15U),
                                        /*!< address one match flag */
    UART_MATCH_ADDR_TWO             = (((uint32_t)UART_STAT_REG_ID << (uint32_t)UART_SHIFT)
                                        | 14U),                                        
                                        /*!< address two match flag */
    UART_NOISE_DETECT               = (((uint32_t)UART_STAT_REG_ID << (uint32_t)UART_SHIFT)
                                        | 18U),
                                        /*!< RX takes 3 samples of each received bit. If these differ, the flag sets */
    UART_FRAME_ERR                  = (((uint32_t)UART_STAT_REG_ID << (uint32_t)UART_SHIFT)
                                        | 17U),
                                        /*!< frame error flag, sets if logic 0 was detected where stop bit expected */
    UART_PARITY_ERR                 = (((uint32_t)UART_STAT_REG_ID << (uint32_t)UART_SHIFT)
                                        | 16U),
                                        /*!< if parity enabled, sets upon parity error detection */
    UART_LIN_BREAK_DETECT           = (((uint32_t)UART_STAT_REG_ID << (uint32_t)UART_SHIFT)
                                        | 31U),
                                        /*!< LIN break detect interrupt flag, sets when LIN break char detected */
    UART_RX_ACTIVE_EDGE_DETECT      = (((uint32_t)UART_STAT_REG_ID << (uint32_t)UART_SHIFT)
                                        | 30U),
                                        /*!< RX pin active edge interrupt flag, sets when active edge detected */
    UART_RX_ACTIVE                  = (((uint32_t)UART_STAT_REG_ID << (uint32_t)UART_SHIFT)
                                        | 24U),
                                        /*!< receiver active flag (RAF), sets at beginning of valid start bit */
    UART_NOISE_IN_CURRENT_WORD      = (((uint32_t)UART_DATA_REG_ID << (uint32_t)UART_SHIFT)
                                        | 15U),
                                        /*!< NOISY bit, sets if noise detected in current data word */
    UART_PARITY_ERR_IN_CURRENT_WORD = (((uint32_t)UART_DATA_REG_ID << (uint32_t)UART_SHIFT)
                                        | 14U),
                                        /*!< PARITYE bit, sets if noise detected in current data word */
    UART_FIFO_TX_OF                 = (((uint32_t)UART_FIFO_REG_ID << (uint32_t)UART_SHIFT)
                                        | 17U),
                                        /*!< transmitter FIFO buffer overflow */
    UART_FIFO_RX_UF                 = (((uint32_t)UART_FIFO_REG_ID << (uint32_t)UART_SHIFT)
                                        | 16U),
                                        /*!< receiver FIFO buffer underflow */
	UART_AUTO_SUCC					= (((uint32_t)UART_AUTOBAUD_REG_ID << (uint32_t)UART_SHIFT)
										| 26U)
										/*!< receiver auto baud success */
} uart_status_flag_e;

/** UART interrupt configuration structure, default settings are 0 (disabled) */
typedef enum
{
    UART_INT_LIN_BREAK_DETECT  = (((uint32_t)UART_BAUD_REG_ID << (uint32_t)UART_SHIFT)
                                   | 15U),  /*!< LIN break detect */
    UART_INT_RX_ACTIVE_EDGE    = (((uint32_t)UART_BAUD_REG_ID << (uint32_t)UART_SHIFT)
                                   | 14U),	/*!< RX active edge */
    UART_INT_TX_DATA_REG_EMPTY = (((uint32_t)UART_CTRL_REG_ID << (uint32_t)UART_SHIFT)
                                   | 23U),	/*!< transmit data register empty */
    UART_INT_TX_COMPLETE       = (((uint32_t)UART_CTRL_REG_ID << (uint32_t)UART_SHIFT)
                                   | 22U),	/*!< transmission complete */
    UART_INT_RX_DATA_REG_FULL  = (((uint32_t)UART_CTRL_REG_ID << (uint32_t)UART_SHIFT)
                                   | 21U),	/*!< receiver data register full */
    UART_INT_IDLE_LINE         = (((uint32_t)UART_CTRL_REG_ID << (uint32_t)UART_SHIFT)
                                   | 20U),	/*!< idle line */
    UART_INT_RX_OVERRUN        = (((uint32_t)UART_CTRL_REG_ID << (uint32_t)UART_SHIFT)
                                   | 27U),	/*!< receiver overrun */
    UART_INT_NOISE_ERR_FLAG    = (((uint32_t)UART_CTRL_REG_ID << (uint32_t)UART_SHIFT)
                                   | 26U),	/*!< noise error flag */
    UART_INT_FRAME_ERR_FLAG    = (((uint32_t)UART_CTRL_REG_ID << (uint32_t)UART_SHIFT)
                                   | 25U),	/*!< frame error flag */
    UART_INT_PARITY_ERR_FLAG   = (((uint32_t)UART_CTRL_REG_ID << (uint32_t)UART_SHIFT)
                                   | 24U),	/*!< parity error flag */
    UART_INT_MATCH_ADDR_ONE    = (((uint32_t)UART_CTRL_REG_ID << (uint32_t)UART_SHIFT)
                                   | 15U),	/*!< match address one flag */
    UART_INT_MATCH_ADDR_TWO    = (((uint32_t)UART_CTRL_REG_ID << (uint32_t)UART_SHIFT)
                                   | 14U),	/*!< match address two flag */
    UART_INT_FIFO_TXOF         = (((uint32_t)UART_FIFO_REG_ID << (uint32_t)UART_SHIFT)
                                   | 9U),	/*!< transmitter FIFO buffer interrupt */
    UART_INT_FIFO_RXUF         = (((uint32_t)UART_FIFO_REG_ID << (uint32_t)UART_SHIFT)
                                   | 8U),	/*!< receiver FIFO buffer interrupt */
	UART_INT_AUTO_BAUD_SUCC	   = (((uint32_t)UART_AUTOBAUD_REG_ID << (uint32_t)UART_SHIFT)
								   | 27U),	/*!< receiver auto baud success interrupt */
} uart_interrupt_e;


/** UART number of bits in a character */
typedef enum
{
	UART_7_BITS_PER_CHAR  = 0x0U, /*!< 0, 7-bit data characters */
    UART_8_BITS_PER_CHAR  = 0x1U, /*!< 1, 8-bit data characters */
    UART_9_BITS_PER_CHAR  = 0x2U, /*!< 2, 9-bit data characters */
    UART_10_BITS_PER_CHAR = 0x3U  /*!< 3, 10-bit data characters */
} uart_bit_number_per_char_e;

/** UART data bit polarity configuration */
typedef enum{
	UART_POLARITY_NORMAL = 0U, /*!< 0, data bit polarity is normal */
	UART_POLARITY_REVERSED     /*!< 1, data bit polarity is reversed */
}uart_tx_rx_polarity_e;

/** the width of narrow pulse when modulated signal */
typedef enum{
	UART_1_DIVIDE_OSR_BIT = 0U, /*!< 0, 1/OSR bit */
	UART_2_DIVIDE_OSR_BIT,		/*!< 1, 2/OSR bit */
	UART_3_DIVIDE_OSR_BIT,		/*!< 2, 3/OSR bit */
	UART_4_DIVIDE_OSR_BIT		/*!< 3, 4/OSR bit */
}uart_irda_pulse_e;

/** UART wakeup method */
typedef enum{
	UART_IDLE_WAKE_UP_WITH_IDLE_FLAG = 0U, /*!< 0, idle wakeup and wakeup set idle flag */
	UART_IDLE_WAKE_UP_WITHOUT_IDLE_FLAG,   /*!< 1, idle wakeup and wakeup do not set idle flag */
	UART_BIT_MARK_WAKE_UP,                 /*!< 2, bit mark wakeup */
	UART_MATCH_WAKE_UP,                    /*!< 3, match wakeup */
}uart_wake_up_e;

/** UART parity mode */
typedef enum
{
    UART_PARITY_DISABLED = 0x0U, /*!< 0, parity disabled */
    UART_PARITY_EVEN     = 0x2U, /*!< 2, parity enabled, type even, bit setting: PE|PT = 10 */
    UART_PARITY_ODD      = 0x3U  /*!< 3, parity enabled, type odd,  bit setting: PE|PT = 11 */
} uart_parity_mode_e;

/** UART number of stop bits */
typedef enum
{
    UART_ONE_STOP_BIT = 0x0U, /*!< 0, one stop bit */ 
    UART_TWO_STOP_BIT         /*!< 1, two stop bits */	    
} uart_stop_bit_count_e;
	
/** UART transmit LSB or MSB first */
typedef enum
{
	UART_LSB_FIRST = 0x0U, /*!< 0, UART transmit LSB first */
	UART_MSB_FIRST         /*!< 1, UART transmit MSB first */
}uart_bit_order_e;

/** UART baudrate selection */	
typedef enum
{
	UART_BAUDRATE_4800 = 4800U,     /*!< baudrate is 4800u */
	UART_BAUDRATE_9600 = 9600U,     /*!< baudrate is 9600u */
	UART_BAUDRATE_19200 = 19200U,   /*!< baudrate is 19200u */
	UART_BAUDRATE_115200 = 115200U, /*!< baudrate is 115200u */
	UART_BAUDRATE_380400 = 380400U, /*!< baudrate is 380400u */
	UART_BAUDRATE_460800 = 460800U, /*!< baudrate is 460800u */
	UART_BAUDRATE_921600 = 921600U  /*!< baudrate is 921600u */
}uart_baudrate_e;

/** UART data match type */
typedef enum
{
	UART_BIT_MARK_ADDR_MATCH = 0U, /*!< 0, bit mark address match */
	UART_IDLE_ADDR_MATCH,          /*!< 1, idle address match */
	UART_ADDR_MATCH_ON_OFF,        /*!< 2, address match on/off */
}uart_match_type_e;

/** UART break character length settings for transmit/detect */
typedef enum
{
    UART_BREAK_CHAR_10_BIT_MINIMUM = 0x0U, /*!< 0, break length between 9~13 bits */                                       
    UART_BREAK_CHAR_13_BIT_MINIMUM = 0x1U  /*!< 1, break length between 12~15 bits */                                               
} uart_break_char_length_e;

/** The type of the event which occurred when the callback was invoked */
typedef enum
{
    UART_EVENT_RX_FULL      = 0x00U, /*!< 0, RX buffer is full */
    UART_EVENT_TX_EMPTY     = 0x01U, /*!< 1, TX buffer is empty */
    UART_EVENT_END_TRANSFER = 0x02U, /*!< 2, the current transfer is ending */
    UART_EVENT_ERROR        = 0x03U, /*!< 3, an error occured during transfer */
/* OCS_Reserved_Start */
		UART_EVENT_BREAK_DETEC	= 0x04U, /*!< 4, break detect occured */
		UART_EVENT_ACTIVE_EDG		= 0x05U, /*!< 5, active edg detect occured */
		UART_EVENT_IDLE_DETECT	= 0x06U, /*!< 6, idle detect occured */
		UART_EVENT_MATCH_ADDR1	= 0x07U, /*!< 7, match address1 occured */
		UART_EVENT_MATCH_ADDR2	= 0x08U, /*!< 8, match address2 occured */
		UART_EVENT_OVER_FLOW		= 0x09U, /*!< 9, over flow occured */
		UART_EVENT_UNDER_FLOW		= 0x0AU, /*!< 10, under flow occured */
		UART_EVENT_AUTOBAUD			= 0x0BU, /*!< 11, autobaud success occured */
/* OCS_Reserved_End */
} uart_event_e;

/** UART work mode */
typedef enum
{
	UART_NORMAL_MODE = 0U, /*!< 0, normal mode */
	UART_SINGLE_WIRE_MODE, /*!< 1, single-wire mode */
	UART_LOOP_MODE         /*!< 2, loop back mode */
}uart_work_mode_e;

/** the direction of TXD in single_wire mode */
typedef enum
{
	UART_TXD_AS_INPUT = 0U, /*!< 0, the TXD pin is input */
	UART_TXD_AS_OUTPUT      /*!< 1, the TXD pin is output */
}uart_tx_direction;

/** the source of TX_CTS */
typedef enum
{
	UART_CTS_N = 0U, /*!< 0, TX_CTS is the inverted UART_CTS_N pin */
	UART_RX_MATCH,   /*!< 1, TX_CTS is the inverted rx match result */
	UART_TRIG_INPUT  /*!< 2, TX_CTS is the trig_input signal */
}uart_tx_cts_source_e;
	
/** the number of idel words that will set RX_FULL_FLAG when RX FIFO is not empty */
typedef enum
{
	UART_NOT_SET_FULL = 0U,   /*!< 0, disable this function */
	UART_1_WORD_SET_FULL,     /*!< 1, 1 idel word */
	UART_2_WORD_SET_FULL,     /*!< 2, 2 idel words */
	UART_4_WORD_SET_FULL,     /*!< 3, 4 idel words */
	UART_8_WORD_SET_FULL,     /*!< 4, 8 idel words */
	UART_16_WORD_SET_FULL,    /*!< 5, 16 idel words */ 
	UART_32_WORD_SET_FULL,    /*!< 6, 32 idel words */
	UART_64_WORD_SET_FULL,    /*!< 7, 64 idel words */
}uart_rx_idle_len_set_full_e; 

/** IDLE_FLAG will be set when specified number idle character have been received */
typedef enum
{
	UART_1_IDLE_CHAR = 0U, /*!< 0, 1 idle character */
	UART_2_IDLE_CHAR,      /*!< 1, 2 idle characters */
	UART_4_IDLE_CHAR,      /*!< 2, 4 idle characters */
	UART_8_IDLE_CHAR,      /*!< 3, 8 idle characters */
	UART_16_IDLE_CHAR,     /*!< 4, 16 idle characters */
	UART_32_IDLE_CHAR,     /*!< 5, 32 idle characters */
	UART_64_IDLE_CHAR,     /*!< 6, 64 idle characters */
	UART_128_IDLE_CHAR,    /*!< 7, 128 idle characters */
}uart_idle_len_e;

/** the point that start detect idle */
typedef enum
{
	UART_START_BIT = 0U, /*!< 0, idle detected start after start bit */
	UART_STOP_BIT,       /*!< 1, idle detected start after stop bit */	
}uart_idle_detect_point_e;

/** the polarity of TX_RTS */
typedef enum
{
	UART_HIGH_LEVEL_ACTIVE = 0U, /*!< 0, RTS_N is active when tx rts is high level */
	UART_LOW_LEVEL_ACTIVE,       /*!< 1, RTS_N is active when tx rts is low level */
}uart_tx_rts_polarity_e;

/** the sample time of TX_CTS */
typedef enum
{
	UART_START_OF_CHAR = 0U, /*!< 0, TX_CTS is sampled at the start of each character */
	UART_BUS_IDLE,           /*!< 1, TX_CTS is sampled when transmitter is idle */
}uart_tx_cts_sample_time_e;

/** callback for all peripherals which support UART features */
typedef void (*uart_callback_t)(void *driver_state, uart_event_e event, void *user_data);

/** definition of interrupt handling function type */
typedef void (*UART_INT_HANDLE)(uart_instance_e instance);

/** UART runtime state structure */
typedef struct
{
    const uint8_t *tx_buff;                         /*!< the buffer of data being sent */              
    uint8_t *rx_buff;                               /*!< the buffer of received data */                  
    volatile uint32_t tx_size;                      /*!< the remaining number of bytes to be transmitted */           
    volatile uint32_t rx_size;                      /*!< the remaining number of bytes to be received */          
    volatile bool is_tx_busy;                       /*!< true if there is an active transmit */           
    volatile bool is_rx_busy;                       /*!< true if there is an active receive */          
    uart_bit_number_per_char_e bit_number_per_char; /*!< number of bits in a char (8/9/10) */
    uart_callback_t rx_call_back;                   /*!< callback to invoke for data receive */                                                  
    void *rx_call_back_param;                       /*!< receive callback parameter pointer */       
    uart_callback_t tx_call_back;                   /*!< callback to invoke for data send */                                           
    void *tx_call_back_param;                       /*!< transmit callback parameter pointer */       
	bool autobaud_en;                      			/*!< enable autobaud function or not  */        
    volatile status_t transmit_status;              /*!< status of last driver transmit operation */
    volatile status_t receive_status;               /*!< status of last driver receive operation */
} uart_state_t;


/** UART configuration structure */
typedef struct
{
    uint32_t baud_rate;                             /*!< UART baud rate */                         
    uart_parity_mode_e parity_mode;                 /*!< parity mode, disabled, even, odd */            
    uart_stop_bit_count_e stop_bit_number;          /*!< number of stop bits, 1 stop bit or 2 stop bits */       
    uart_bit_number_per_char_e bit_number_per_char; /*!< number of bits in a character (8-default, 9 or 10) */                                                  
	uart_tx_rx_polarity_e tx_bit_parity;            /*!< UART TX data bit polarity */         
	uart_tx_rx_polarity_e rx_bit_parity;            /*!< UART RX data bit polarity */           
	uart_bit_order_e bit_order;                     /*!< option to transmit LSB or MSB first */
	bool autobaud_en;                      /*!< enable autobaud function or not  */   
	uint32_t tx_interrupt_priority;					/*!< UART TX interrupt poriority*/
	uint32_t rx_interrupt_priority;					/*!< UART TX interrupt poriority*/
} uart_user_config_t;


#include "uart_hal_cfg.h"


/*******************************************************************************
* Public UART Control Functions
******************************************************************************/
void uart_default_config_get(uart_user_config_t *uart_user_config);
status_t uart_init(uart_instance_e instance, const uart_user_config_t *uart_user_config);
status_t uart_de_init(uart_instance_e instance);
void uart_rx_callback_install(uart_instance_e instance,
								uart_callback_t function,
								void *callback_param);
void uart_tx_callback_install(uart_instance_e instance,
								uart_callback_t function,
								void *callback_param);
status_t uart_send_data_polling(uart_instance_e instance, const uint8_t *tx_buff, uint32_t tx_size);
status_t uart_send_data(uart_instance_e instance,
							const uint8_t *tx_buff,
							uint32_t tx_size);
status_t uart_transmit_status_get(uart_instance_e instance, uint32_t *bytes_remaining);							 
status_t uart_send_data_abort(uart_instance_e instance);
status_t uart_receive_data_polling(uart_instance_e instance, uint8_t *rx_buff, uint32_t rx_size);
/* OCS_Reserved_Start */
bool uart_receive_data_polling_for_cmdline(uart_instance_e instance, uint8_t *rx_buff, uint32_t rx_size);
/* OCS_Reserved_End */
status_t uart_receive_data(uart_instance_e instance,
                                uint8_t *rx_buff,
                                uint32_t rx_size);
status_t uart_receive_status_get(uart_instance_e instance, uint32_t *bytes_remaining);
status_t uart_receive_data_abort(uart_instance_e instance);
status_t uart_baudrate_set(uart_instance_e instance, uint32_t baud_rate);

void uart_hal_status_rxbusy_set(uart_instance_e instance, bool rx_busy);

void uart_baudrate_get(uart_instance_e instance, uint32_t *baud_rate);
status_t uart_tx_buffer_set(uart_instance_e instance,
                                const uint8_t *tx_buff,
                                uint32_t tx_size);
status_t uart_rx_buffer_set(uart_instance_e instance,
                                uint8_t *rx_buff,
                                uint32_t rx_size);												
void uart_irq_handler(uart_instance_e instance);
void uart_interrupt_handle_cfg(uart_instance_e instance, UART_INT_HANDLE handle);
void uart_autobaud_result_clear(uart_instance_e instance);
void uart_standby_mode_enter(uart_instance_e instance, bool standby_en);
void uart_match1_set(uart_instance_e instance, bool match_en, uint32_t match_addr);
void uart_match2_set(uart_instance_e instance, bool match_en, uint32_t match_addr);
void uart_break_send(uart_instance_e instance, uart_break_char_length_e break_length);
void uart_break_detect_enable(uart_instance_e instance, bool brk_det_en);
void uart_work_mode_cfg(uart_instance_e instance, uart_work_mode_e work_mode, uart_tx_direction tx_dir);
void uart_transmit_enable(uart_instance_e instance, bool tx_en);
void uart_receive_enable(uart_instance_e instance, bool rx_en);
void uart_tx_cts_set(uart_instance_e instance, bool tx_cts_en, uart_tx_cts_source_e tx_cts_src, uart_tx_cts_sample_time_e cts_sample_time);
void uart_rx_rts_set(uart_instance_e instance, bool rx_rts_en, uint8_t rx_rts_water);
void uart_tx_rts_set(uart_instance_e instance, bool tx_rts_en, uart_tx_rts_polarity_e tx_rts_pol);
void uart_interrupt_source_enable(uart_instance_e instance, uart_interrupt_e int_src, bool interrupt_enable);
void uart_irda_enable(uart_instance_e instance, bool irda_en, uart_irda_pulse_e irda_pulse);
void uart_wakeup_cfg(uart_instance_e instance, uart_wake_up_e wake_up);
void uart_match_type_set(uart_instance_e instance, uart_match_type_e match_type);
void uart_idle_len_set(uart_instance_e instance, uart_idle_len_e idle_len, uart_idle_detect_point_e idle_detect_point);
void uart_tx_fifo_enable(uart_instance_e instance, bool tx_fifo_en, uint8_t tx_water);
void uart_rx_fifo_enable(uart_instance_e instance, bool rx_fifo_en, uint8_t rx_water, uart_rx_idle_len_set_full_e idle_words);
bool uart_status_flag_get(uart_instance_e instance, uart_status_flag_e status_flag);
void uart_doze_enable(uart_instance_e instance, bool doze_en);

void uart0_rx_empty_wait(void);
void UartView(int16_t view1, int16_t view2, int16_t view3, int16_t view4);



/** @}*/ /* End of addtogroup ingroup_spi_hal */

#endif /* UART_HAL_H__ */


