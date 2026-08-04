#ifndef I2C_TYPE_H
#define I2C_TYPE_H

/** the size of hw fifo, unit is word, configures the number of words in the master FIFO to 2^fifosize. */
#define HW_FIFO_SIZE        8U
/** the size of sw cmd fifo, worst case is 5 commands in High-Speed receive with 10-bit address:
   START + master code, REP START + addr_1 + tx, addr_2, REP START + addr_1 + rx, receive command */
#define SW_CMD_FIFO_SIZE    5U

/* clock limitation */
#define CLK_HIGH_MIN        (1UL)
#define CLK_HIGH_MAX        (31UL)
#define CLK_LOW_MIN         (3UL)
#define CLK_LOW_MAX         (CLK_HIGH_MAX)
#define CLK_DEFAULT_OFFSET  (2UL)
#define CLK_TOTOL           (CLK_HIGH_MAX + CLK_LOW_MAX+ CLK_DEFAULT_OFFSET)
#define CLK_DEFAULT_PRE     (1UL)
#define DATAVD_MIN          (1UL)
#define SETHOLD_MIN         (2UL)
#define BUSIDLE_MAX         (0xFFFUL)

/** I2C read command */
#define READ_CMD            (1U)
/** I2C write command */
#define WRITE_CMD           (0U)



/** Clear all Interrupt */
#define I2C_MASTER_CLEAR_ALL_INT            (0x7F00U)
/** Data Match Interrupt */
#define I2C_MASTER_DATA_MATCH_INT           (0x4000U)
/** Pinlow Timeout Interrupt */
#define I2C_MASTER_PIN_LOW_TIMEOUT_INT      (0x2000U)
/** FIFO Error Interrupt */
#define I2C_MASTER_FIFO_ERROR_INT           (0x1000U)
/** Arbitration Lost Interrupt */
#define I2C_MASTER_ARBITRATION_LOST_INT     (0x800U)
/** Nack Detect Interrupt */
#define I2C_MASTER_NACK_DETECT_INT          (0x400U)
/** Stop Detect Interrupt */
#define I2C_MASTER_STOP_DETECT_INT          (0x200U)
/** End Packet Interrupt */
#define I2C_MASTER_END_PACKET_INT           (0x100U)
/** Receive Data Interrupt */
#define I2C_MASTER_RECEIVE_DATA_INT         (0x2U)
/** Transmit Data Interrupt */
#define I2C_MASTER_TRANSMIT_DATA_INT        (0x1U)


/** Clear all Interrupt */
#define I2C_SLAVE_CLEAR_ALL_INT                (0xF00U)
/** SMBus Alert Interrupt */
#define I2C_SLAVE_SMBUS_ALERT_RESPONSE_INT     (0x8000U)
/** General Call Interrupt */
#define I2C_SLAVE_GENERAL_CALL_INT             (0x4000U)
/** Address Match0 Interrupt */
#define I2C_SLAVE_ADDRESS_MATCH_1_INT          (0x2000U)
/** Address Match1 Interrupt */
#define I2C_SLAVE_ADDRESS_MATCH_0_INT          (0x1000U)
/** FIFO Error Interrupt */
#define I2C_SLAVE_FIFO_ERROR_INT               (0x800U)
/** Bit Error Interrupt */
#define I2C_SLAVE_BIT_ERROR_INT                (0x400U)
/** Stop Detect Interrupt */
#define I2C_SLAVE_STOP_DETECT_INT              (0x200U)
/** Repeated Start Interrupt */
#define I2C_SLAVE_REPEATED_START_INT           (0x100U)
/** Transmit ACK Interrupt */
#define I2C_SLAVE_TRANSMIT_ACK_INT             (0x8U)
/** Address Valid Interrupt */
#define I2C_SLAVE_ADDRESS_VALID_INT            (0x4U)
/** Receive Data Interrupt */
#define I2C_SLAVE_RECEIVE_DATA_INT             (0x2U)
/** Transmit Data Interrupt */
#define I2C_SLAVE_TRANSMIT_DATA_INT            (0x1U)

/** For function enable / disable */
#define I2C_FUNC_ENABLE                        (1U)
#define I2C_FUNC_DISABLE                       (0U)

/* i2c watermark config for default */
#define I2C_SLAVE_TX_WATERMARK_DEFAULT_VALUE   (1UL)
#define I2C_SLAVE_TX_WATERMARK_MAX_VALUE       (4UL)

/** Software buffer size for I2C data, unit is byte. */
#define SW_DATA_BUF_SIZE    (64U)
/** Enable i2c flag test. */
#define FLAG_TEST

/** I2C watermark options */
typedef enum {
    I2C_TX_WATERMARK = 0U,    /*!< I2C TX watermark */
    I2C_RX_WATERMARK,         /*!< I2C RX watermark */
}i2c_watermark_e;

/** I2C master prescaler options */
typedef enum
{
    I2C_PRESCALE_DIV_1 = 0UL,            /*!< Divide by 1 */
    I2C_PRESCALE_DIV_2 = 1UL,            /*!< Divide by 2 */
    I2C_PRESCALE_DIV_4 = 2UL,            /*!< Divide by 4 */
    I2C_PRESCALE_DIV_8 = 3UL,            /*!< Divide by 8 */
    I2C_PRESCALE_DIV_16 = 4UL,           /*!< Divide by 16 */
    I2C_PRESCALE_DIV_32 = 5UL,           /*!< Divide by 32 */
    I2C_PRESCALE_DIV_64 = 6UL,           /*!< Divide by 64 */
    I2C_PRESCALE_DIV_128 = 7UL,          /*!< Divide by 128 */
    I2C_PRESCALE_DIV_INVALID = 8UL       /*!< Invalid prescale */
}i2c_prescale_sel_e;

/** I2C master commands */
typedef enum
{
    I2C_MASTER_CMD_WRITE = 0u,
    I2C_MASTER_CMD_READ,
    I2C_MASTER_CMD_WRITE_STOP,
    I2C_MASTER_CMD_READ_STOP,
    I2C_MASTER_CMD_RESTART_WRITE,
    I2C_MASTER_CMD_RESTART_READ,
    I2C_MASTER_CMD_RESTART_STOP_WRITE,
    I2C_MASTER_CMD_RESTART_STOP_READ,
    I2C_MASTER_CMD_INVALID             /*!< Invalid master command */

#if 0
    I2C_MASTER_CMD_TRANSMIT = 0U,      /*!< Transmit DATA[7:0] */
    I2C_MASTER_CMD_RECEIVE,            /*!< Receive (DATA[7:0] + 1) bytes */
    I2C_MASTER_CMD_STOP,               /*!< Send stop */
    I2C_MASTER_CMD_RECEIVE_DISCARD,    /*!< Receive and discard (DATA[7:0] + 1) bytes */
    I2C_MASTER_CMD_START,              /*!< Send (repeated) START and transmit address in DATA[7:0] */
    I2C_MASTER_CMD_START_NACK,         /*!< Send START and transmit address in DATA[7:0], expect to receive a NACK */
    I2C_MASTER_CMD_START_HS,           /*!< Send START and transmit address in DATA[7:0] in high speed mode */
    I2C_MASTER_CMD_START_NACK_HS,      /*!< Send START and transmit address in DATA[7:0] in high speed mode, expect to receive a NACK */
    I2C_MASTER_CMD_INVALID             /*!< Invalid master command */
#endif
}i2c_master_cmd_e;

/** I2C master host request input selection */
typedef enum
{
    I2C_HREQ_INPUT_PIN = 0U,      /*!< Host request input by pin */
    I2C_HREQ_INPUT_TRIGGER        /*!< Host request input by trigger from trgmux module, please refer #trgmux_hal_module */
}i2c_master_hreq_sel_e;

/** The polarity of the host request input pin */
typedef enum
{
    I2C_HREQ_ACTIVE_LOW = 0U,    /*!< Host request active low  */
    I2C_HREQ_ACTIVE_HIGH         /*!< Host request active high  */
}i2c_master_hreq_polarity_e;


/** The type of the event which occurred when the callback was invoked. */
typedef enum {
    I2C_TX_FINISH = 0U,    /*!< A frame was sent. */
    I2C_RX_FINISH,         /*!< A frame was received. */
    I2C_ERROR,             /*!< A transfer fail because of I2C bus error. */
    I2C_DATA_MATCH,        /*!< I2C master data match interrupt occurs. */
    I2C_PIN_LOW,           /*!< I2C master pin low timeout interrupt occurs. */
    I2C_ADDR_MATCH,        /*!< I2C slave address match interrupt occurs. */
    I2C_SMBUS_ALERT,       /*!< I2C slave SMBUS alert interrupt occurs. */
    I2C_GENERAL_CALL,      /*!< I2C slave general call interrupt occurs. */
    I2C_NO_EVENT           /*!< Nothing happened. */
}i2c_event_e;

/** I2C operation mode. */      //use
typedef enum
{
    //I2C_STANDARD_MODE = 0x0U,    /*!< Standard mode reference rate: 100 kbit/s */
	I2C_STANDARD_MODE = 0x1U,    /*!< Standard mode reference rate: 100 kbit/s */
    I2C_FAST_MODE,               /*!< Fast mode reference rate: 400 kbit/s */
    I2C_FASTPLUS_MODE,           /*!< FastPlus mode reference rate: 1 Mbit/s */
    I2C_HIGHSPEED_MODE,          /*!< High Speed mode reference rate: 3.4 Mbit/s */
    //I2C_ULTRAFAST_MODE           /*!< output-only push-pull function, reference rate: 5 Mbit/s */
}i2c_mode_e; 

/** I2C master pin configuration. */
typedef enum
{
    I2C_2PIN_OPEN_DRAIN = 0U,      /*!< 2-pin open drain mode */
    I2C_2PIN_OUTPUT_ONLY,          /*!< 2-pin output only mode (ultra-fast mode) */        
    I2C_2PIN_PUSH_PULL,            /*!< 2-pin push-pull mode */
    I2C_4PIN_PUSH_PULL,            /*!< 4-pin push-pull mode */
    I2C_2PIN_OPEN_DRAIN_SLAVE,     /*!< 2-pin open drain mode with separate LPI2C slave */
    I2C_2PIN_OUTPUT_ONLY_SLAVE,    /*!< 2-pin output only mode (ultra-fast mode) with separate LPI2C slave */
    I2C_2PIN_PUSH_PULL_SLAVE,      /*!< 2-pin push-pull mode with separate LPI2C slave */
    I2C_4PIN_PUSH_PULL_INVERTED,   /*!< 4-pin push-pull mode (inverted outputs) */
}i2c_master_pin_config_e;

/** I2C master data match feature configuration. */
typedef enum
{
    I2C_DMATCH_DISABLE = 0U,  /*!< data match feature disable */
    I2C_DMATCH_RESERVED,      /*!< reserved */
    I2C_DMATCH_0OR1,          /*!< 1st data word equals MATCH0 or MATCH1*/
    I2C_DMATCH_NORN1,         /*!< any data word equals MATCH0 or MATCH1*/
    I2C_DMATCH_0AND1,         /*!< 1st data word equals MATCH0 and 2nd data word equals MATCH1*/
    I2C_DMATCH_NANDN1,        /*!< any data word equals MATCH0 and next data word equals MATCH1*/
    I2C_DMATCH_0,             /*!< 1st data word and MATCH1 equals MATCH0 and MATCH1*/
    I2C_DMATCH_N,             /*!< any data word and MATCH1 equals MATCH0 and MATCH1*/
}i2c_master_dmatch_config_e;

/** I2C slave address match configuration. */
typedef enum
{
    I2C_ADDR_MATCH_0_7BIT = 0U,            /*!< Address match 0 (7-bit) */
    I2C_ADDR_MATCH_0_10BIT,                /*!< Address match 0 (10-bit) */
    I2C_ADDR_MATCH_0_7BIT_OR_1_7BIT,       /*!< Address match 0 (7-bit) or Address match 1 (7-bit) */
    I2C_ADDR_MATCH_0_10BIT_OR_1_10BIT,     /*!< Address match 0 (10-bit) or Address match 1 (10-bit) */
    I2C_ADDR_MATCH_0_7BIT_OR_1_10BIT,      /*!< Address match 0 (7-bit) or Address match 1 (10-bit) */
    I2C_ADDR_MATCH_0_10BIT_OR_1_7BIT,      /*!< Address match 0 (10-bit) or Address match 1 (7-bit) */
    I2C_ADDR_MATCH_RANGE_7BIT,             /*!< From Address match 0 (7-bit) to Address match 1 (7-bit) */
    I2C_ADDR_MATCH_RANGE_10BIT             /*!< From Address match 0 (10-bit) to Address match 1 (10-bit) */
}i2c_slave_addr_config_e;

typedef void (*I2C_INT_HANDLE)(i2c_event_e i2c_event);

/** I2C master host request configuration */
typedef struct
{
    bool enable;
    
    i2c_master_hreq_sel_e hreq_sel;                /*!< I2C master host request input selection */
    i2c_master_hreq_polarity_e hreq_polarity;      /*!< The polarity of the host request input pin */
}i2c_master_hreq_config_t;

/** I2C master internal structure */
typedef struct
{   
    /* cmd related parameter */
    i2c_master_cmd_e cmd[SW_CMD_FIFO_SIZE];        /*!< command buffer */
    uint8_t cmd_data[SW_CMD_FIFO_SIZE];   /*!< data buffer */
    uint8_t write_idx;                    /*!< the index of write process */
    uint8_t read_idx;                     /*!< the index of read process */

    /* tx data related parameter */
    volatile uint32_t tx_length;
    uint8_t *tx_data;
    
    /* rx data related parameter */
    volatile uint32_t rx_length;
    uint8_t *rx_data;

    /*i2c is write or read*/
    bool i2c_transmit_status;       //true:write   false:read
}i2c_master_hw_t;

typedef struct
{   
    bool addr_10bit;
    uint32_t repeat_start_num;
    
    /* tx data related parameter */
    volatile uint32_t tx_length;
    uint8_t *tx_data;
    
    /* rx data related parameter */
    volatile uint32_t rx_length;
    uint8_t *rx_data;
}i2c_slave_hw_t;

/** I2C master data match specific configuration. */
typedef struct
{
    uint8_t data0;    /*!< match data0 */
    uint8_t data1;    /*!< match data1 */

    i2c_master_dmatch_config_e dmatch_cfg;    /*!< data match feature configuration, please refer #i2c_master_dmatch_config_e */
}i2c_master_dmatch_config_t;

/** I2C master message structure */           //use
typedef struct
{
    bool addr_10bit;                        /*!< Selects 7-bit or 10-bit slave address */
    uint16_t slave_addr;                    /*!< Slave address */
    uint16_t slave_reg_addr;                /* Slave register address */

    uint8_t send_data[SW_DATA_BUF_SIZE];    /*!< Transmit data buffer */
    uint32_t send_data_len;                 /*!< Transmit data length exclude slave ID */

    uint8_t rec_data[SW_DATA_BUF_SIZE];     /*!< Receive data buffer */
    uint32_t rec_data_len;                  /*!< Receive data length */
}i2c_master_msg_t;

/** I2C master configuration structure */      //use
typedef struct
{
    i2c_mode_e master_mode;                            /*!< I2C master mode selection */
    i2c_master_pin_config_e i2c_master_pin_config;     /*!< I2C master pin configuration selection */

    uint32_t i2c_speed;                                /*!< Baudrate to use with current slave device */
    uint32_t i2c_high_speed;                           /*!< Baudrate only for High-speed mode. */
    uint8_t master_code;                               /*!< Valid range: 0-7, only for High-speed mode. */

    I2C_INT_HANDLE master_int_handle;                  /*!< I2C master interrupt handle */
        uint32_t master_interrupt_priority;            /*!< I2C master interrupt priority*/
}i2c_master_config_t;

/** I2C slave message structure */
typedef struct
{    
    uint8_t send_data[SW_DATA_BUF_SIZE];    /*!< Transmit data buffer */
    uint32_t send_data_len;                 /*!< Transmit data length exclude slave ID */

    uint8_t rec_data[SW_DATA_BUF_SIZE];     /*!< Receive data buffer */
    uint32_t rec_data_len;                  /*!< Receive data length */
}i2c_slave_msg_t;

/** I2C slave configuration structure */
typedef struct
{
    i2c_mode_e slave_mode;                  /*!< I2C slave mode selection */

    bool addr_10bit;                        /*!< Selects 7-bit or 10-bit slave address */
    i2c_slave_addr_config_e addr_config;    /*!< I2C slave address match configuration */
    uint16_t slave_addr0;                   /*!< Match address0 */
    uint16_t slave_addr1;                   /*!< Match address1 */

    I2C_INT_HANDLE slave_int_handle;        /*!< I2C slave interrupt handle */
    uint32_t slave_interrupt_priority;  /*!< I2C slave interrupt priority*/
}i2c_slave_config_t;

/*use*/
#define INT_M_RX_UNDER          (0x01U)    /*RX Underflow Mask */
#define INT_M_RX_OVER           (0x02U)    /*RX Overflow Mask */
#define INT_M_RX_FULL           (0x04U)    /*RX Full Mask */
#define INT_M_TX_OVER           (0x08U)    /*TX Overflow Mask */
#define INT_M_TX_EMPTY          (0x10U)    /*TX Empty Mask */
#define INT_M_RD_REQ            (0x20U)    /*Read Request Mask */
#define INT_M_TX_ABRT           (0x40U)    /*TX Abort Mask */
#define INT_M_RX_DONE           (0x80U)    /*RX Done Mask */
#define INT_M_ACTIVITY          (0x100U)    /*Activity Mask */
#define INT_M_STOP_DET          (0x200U)    /*Stop Detection Mask */
#define INT_M_START_DET         (0x400U)   /*Start Detection Mask */
#define INT_M_GEN_CALL          (0x800U)   /*General Call Mask */
#define INT_M_RESTART_DET       (0x1000U)   /*Restart Detection Mask */
#define INT_M_MST_ON_HOLD       (0x2000U)   /*Master on Hold Mask */

#endif/* I2C_TYPE_H */

