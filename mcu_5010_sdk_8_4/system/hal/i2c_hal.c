#include "platform_common.h"

#include "i2c_type.h"
#include "i2c_hal.h"
#include "i2c_reg.h"
#include "i2c.h"

/*******************************************************************************
* Definitions
******************************************************************************/
/* i2c interrupt priority */
#define I2C_INTERRUPT_DEFAULT_PRIORITY (0UL)
/* i2c data length for default */
#define I2C_DEFAULT_LENGTH_0           (0U)
#define I2C_DEFAULT_INDEX_0            (0U)
/* i2c watermark config for default */
#define I2C_DEFAULT_TX_WATERMARK_VALUE (0U)
#define I2C_DEFAULT_RX_WATERMARK_VALUE (0U)
/* i2c DATA value in I2C_TDATA for default */
#define I2C_DEFAULT_CMD_DATA_VALUE     (0U)
/* i2c read or read cmd mask */
#define I2C_CMD_RW_MASK                (0x1UL)
/* i2c 10bit address mode header data */
#define I2C_10BIT_ADDR_HEADER          (0xF0U)
#define I2C_10BIT_ADDR_HIGH_MASK       (0x6U)
#define I2C_10BIT_ADDR_LOW_MASK        (0xFFU)
#define I2C_ADDR_RIGHT_SHIFT_7         (7U)
#define I2C_ADDR_LEFT_SHIFT_1          (1U)
#define I2C_CLK_INITIAL_VALUE          (0U)
#define I2C_NVIC_DISABLE               (0UL)
/*******************************************************************************
 * Variables
 ******************************************************************************/
/** Tables to save I2C master IRQ enum numbers defined in CMSIS header file. */
static IRQn_Type const g_i2c_master_irqn[I2C_INSTANCE_MAX_COUNT] = I2C_IRQS;
/** Tables to save I2C master interrupt handle. */
//static I2C_INT_HANDLE g_i2c_master_int_handle[I2C_INSTANCE_MAX_COUNT] = {NULL};
/** Tables to save I2C slave IRQ enum numbers defined in CMSIS header file. */
static IRQn_Type const g_i2c_slave_irqn[I2C_INSTANCE_MAX_COUNT] = I2C_IRQS;
/** Tables to save I2C slave interrupt handle. */
//static I2C_INT_HANDLE g_i2c_slave_int_handle[I2C_INSTANCE_MAX_COUNT] = {NULL};

static i2c_master_hw_t g_i2c_master =             //use
{
    .cmd = {I2C_MASTER_CMD_INVALID, 
            I2C_MASTER_CMD_INVALID, 
            I2C_MASTER_CMD_INVALID, 
            I2C_MASTER_CMD_INVALID, 
            I2C_MASTER_CMD_INVALID},
    .cmd_data = {0U},
    .write_idx = 0U,
    .read_idx = 0U,

    .tx_length = 0U,
    .tx_data = NULL,
    .rx_length = 0U,
    .rx_data = NULL,

    .i2c_transmit_status = true     //默认为写
};

static i2c_slave_hw_t g_i2c_slave = 
{
    .repeat_start_num = 0U,
    
    .tx_length = 0U,
    .tx_data = NULL,
    .rx_length = 0U,
    .rx_data = NULL
};

static inline I2C_TypeDef *i2c_addr_get(uint32_t i2c_instance);      //use

/*******************************************************************************
* Private I2C Control Functions
******************************************************************************/
/**
 *@brief Get i2c base address according to the i2c instance.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@return    i2c_ptr: point to i2c base address
 */
static inline I2C_TypeDef *i2c_addr_get(uint32_t i2c_instance)      //use
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef * const i2c_bases[I2C_INSTANCE_MAX_COUNT] = I2C_BASE_ADDRS;
    I2C_TypeDef *i2c_ptr = i2c_bases[i2c_instance];
    return i2c_ptr;
}

/*******************************************************************************
* Private I2C MASTER Functions
******************************************************************************/ 
/**
 *@brief Handle master end transfer operations when error happens. 
 *         This function will disable interrupt, send stop cmd, reset FIFO, disable I2C master module and reset sw buffer.
 *         Then invoke callback function if installed.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@return    none
 */
#if 0
static void i2cm_error_handle(uint32_t i2c_instance)
{

    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);

     /* disable all interrupt */
     i2cm_drv_int_cfg(i2c_ptr, 0x3fff, false);
     i2cm_end_transfer(i2c_instance, true, true);


    g_i2c_master.tx_data = NULL;
    g_i2c_master.tx_length = I2C_DEFAULT_LENGTH_0;

    g_i2c_master.rx_data = NULL;
    g_i2c_master.rx_length = I2C_DEFAULT_LENGTH_0;

    /* record the error event */
    if (g_i2c_master_int_handle[i2c_instance] != NULL)
    {
        g_i2c_master_int_handle[i2c_instance](I2C_ERROR);
    }

}
#endif

/**
 *@brief This function fills cmd and cmd_data in the hardware FIFO or in the software fifo buffer if hw fifo is full.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@param[in] cmd: the command for the I2C master, select from i2c_master_cmd_e.
 *@param[in] data: the data for the I2C master
 *@return    none
 */
#if 0
static void i2cm_fifo_cmd_cfg(uint32_t i2c_instance, i2c_master_cmd_e cmd, uint8_t data)
{

    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);

    uint16_t tx_fifo_count = i2cm_drv_txfifo_get(i2c_ptr);

    if (tx_fifo_count < HW_FIFO_SIZE)
    {
        i2cm_drv_tdata_send(i2c_ptr, cmd, data);
    }
    else /* hw fifo is full, use sw fifo */
    {
        if (g_i2c_master.write_idx < SW_CMD_FIFO_SIZE)
        {
            g_i2c_master.cmd[g_i2c_master.write_idx] = cmd;
            g_i2c_master.cmd_data[g_i2c_master.write_idx] = data;
            g_i2c_master.write_idx++;
        }
    }
}
#endif

/**
 *@brief Transfers commands from the master software FIFO to the hardware FIFO.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@return    none
 */
#if 0
static void i2cm_fifo_cmd_send(uint32_t i2c_instance)
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);

    /* get hardware fifo count */
    uint16_t tx_fifo_count = i2cm_drv_txfifo_get(i2c_ptr);    

    /* send sw fifo data */
    while ((g_i2c_master.write_idx != g_i2c_master.read_idx) && (tx_fifo_count < HW_FIFO_SIZE))
    {
        i2cm_drv_tdata_send(i2c_ptr, g_i2c_master.cmd[g_i2c_master.read_idx], \
                                    g_i2c_master.cmd_data[g_i2c_master.read_idx]);

        g_i2c_master.read_idx++;

        /* update hardware fifo count */
        tx_fifo_count = i2cm_drv_txfifo_get(i2c_ptr);    
    }

    if (g_i2c_master.write_idx == g_i2c_master.read_idx)
    {
        /* Reset idx */
        g_i2c_master.write_idx = I2C_DEFAULT_INDEX_0;
        g_i2c_master.read_idx = I2C_DEFAULT_INDEX_0;
    }
}
#endif

/**
 *@brief Fill hardware tx fifo if sw buffer is empty.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@return    none
*/
#if 0
static void i2cm_fifo_data_update(uint32_t i2c_instance)
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);

    /* get hardware fifo count */
    uint16_t tx_fifo_count = i2cm_drv_txfifo_get(i2c_ptr);

    if (g_i2c_master.write_idx == g_i2c_master.read_idx)
    {
        while ((g_i2c_master.tx_length > I2C_DEFAULT_LENGTH_0) && (tx_fifo_count < HW_FIFO_SIZE))
        {
            i2cm_drv_tdata_send(i2c_ptr, I2C_MASTER_CMD_WRITE, g_i2c_master.tx_data[I2C_DEFAULT_INDEX_0]);

            g_i2c_master.tx_data++;
            g_i2c_master.tx_length--;
            
            /* update hardware fifo count */
            tx_fifo_count = i2cm_drv_txfifo_get(i2c_ptr);
        }
    }
    else
    {
        /* sw buffer is not empty */
    }
}
#endif

/**
 *@brief Handle a transmit request for master.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@return    none
 */
#if 0
static void i2cm_transmit_int_handle(uint32_t i2c_instance)
{

    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);

    /* More data in sw buffer needed for transmission */
    if (g_i2c_master.read_idx != g_i2c_master.write_idx)
    {
        i2cm_fifo_cmd_send(i2c_instance);
    }
    else if (g_i2c_master.tx_data != NULL)
    {
        if (g_i2c_master.tx_length == I2C_DEFAULT_LENGTH_0)
        {
            /* There is no more data in buffer, the transmission finish 
            if need to send restart, do not send stop cmd */
            i2cm_end_transfer(i2c_instance, true, false);
            i2cm_drv_int_cfg(i2c_ptr, INT_M_TX_ABRT, false);
            
            if (g_i2c_master_int_handle[i2c_instance] != NULL)
            {
                g_i2c_master_int_handle[i2c_instance](I2C_TX_FINISH);
            }
        }
        else
        {
            /* continue to fill tx fifo */
            i2cm_fifo_data_update(i2c_instance);
        }
    }
    else
    {
        //Interrupts that need to be masked
        i2cm_drv_int_cfg(i2c_ptr, INT_M_TX_EMPTY, false);
    }
}
#endif

/**
 *@brief Handle an interrupt process for i2c master.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@return    none
 */
#if 0
static void i2cm_receive_int_handle(uint32_t i2c_instance)
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);
    uint32_t rx_len = g_i2c_master.rx_length;

    /* Transfer received data from hw fifo to user buffer */
    while ((i2cm_drv_rxfifo_get(i2c_ptr) > I2C_DEFAULT_LENGTH_0) && (rx_len > I2C_DEFAULT_LENGTH_0))
    {
        g_i2c_master.rx_data[I2C_DEFAULT_INDEX_0] = (uint8_t)(i2c_ptr->IC_DATA_CMD.DAT);
        g_i2c_master.rx_data++;
        g_i2c_master.rx_length--;
    }
    
    if (g_i2c_master.rx_length == I2C_DEFAULT_LENGTH_0)
    {
        /* Done receiving */
        i2cm_end_transfer(i2c_instance, true, false);
        i2cm_drv_int_cfg(i2c_ptr, INT_M_RX_UNDER | INT_M_RX_OVER |
            INT_M_ACTIVITY | INT_M_STOP_DET | INT_M_START_DET | 
            INT_M_TX_OVER | INT_M_TX_EMPTY | INT_M_TX_ABRT,
            false);           //need to change
                
        
        if (g_i2c_master_int_handle[i2c_instance] != NULL)
        {
            g_i2c_master_int_handle[i2c_instance](I2C_RX_FINISH);
        }
    }
    else if (g_i2c_master.rx_length <= i2cm_drv_rxwatermark_get(i2c_ptr))   //need to change
    {
        /* Reduce rx watermark to receive the last few bytes */
        i2cm_drv_watermark_set(i2c_ptr, (uint8_t)(g_i2c_master.rx_length - 1U), I2C_RX_WATERMARK);
    }
    else
    {
        /* Continue reception */
    }
}
#endif

/**
 *@brief Handle non-blocking master operation when I2C master interrupt occurs.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@return    none
 */
#if 0
static void i2c_master_int_handle(uint32_t i2c_instance)        //need to change
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);

    uint32_t int_flag = i2cm_drv_intsts_get(i2c_ptr);
    uint32_t int_en = i2cm_drv_inten_get(i2c_ptr);
    
    /* clear all int except tx/rx, tx/rx status can not use software to clear */
    i2cm_drv_intsts_clear(i2c_ptr, I2C_MASTER_CLEAR_ALL_INT);


    /* Received NACK interrupt */
    /* if ultra speed mode, do nothing, disable nack detect int all the time */
    if (((int_en&I2C_I2CM_INT_EN_NACK_DETECT_EN_MASK) == I2C_I2CM_INT_EN_NACK_DETECT_EN_MASK) &&    //need to change
        ((int_flag&I2C_I2CM_STAT_NACK_DETECT_FLAG_MASK) == I2C_I2CM_STAT_NACK_DETECT_FLAG_MASK))
    {
        i2cm_drv_int_cfg(i2c_ptr, I2C_MASTER_FIFO_ERROR_INT |
                         I2C_MASTER_ARBITRATION_LOST_INT |
                         I2C_MASTER_NACK_DETECT_INT |
                         I2C_MASTER_TRANSMIT_DATA_INT |
                         I2C_MASTER_RECEIVE_DATA_INT,
                         false);
                                 
        /* End transfer: no stop generation (the module will handle that by itself if needed), reset FIFOs */
        i2cm_end_transfer(i2c_instance, false, true);
    }
    
    /* transmit done interrupt */
    if (((int_en&I2C_I2CM_INT_EN_TRANSMIT_DATA_EN_MASK) == I2C_I2CM_INT_EN_TRANSMIT_DATA_EN_MASK) && 
        ((int_flag&I2C_I2CM_STAT_TRANSMIT_DATA_FLAG_MASK) == I2C_I2CM_STAT_TRANSMIT_DATA_FLAG_MASK))
    {
        i2cm_transmit_int_handle(i2c_instance);
    }
    
    /* receive data interrupt */
    if (((int_en&I2C_I2CM_INT_EN_RECEIVE_DATA_EN_MASK) == I2C_I2CM_INT_EN_RECEIVE_DATA_EN_MASK) && 
        ((int_flag&I2C_I2CM_STAT_RECEIVE_DATA_FLAG_MASK) == I2C_I2CM_STAT_RECEIVE_DATA_FLAG_MASK))
    {
        i2cm_receive_int_handle(i2c_instance);
    }

    /* FIFO error interrupt */
    if (((int_en&I2C_I2CM_INT_EN_FIFO_ERROR_EN_MASK) == I2C_I2CM_INT_EN_FIFO_ERROR_EN_MASK) && 
        ((int_flag&I2C_I2CM_STAT_FIFO_ERROR_FLAG_MASK) == I2C_I2CM_STAT_FIFO_ERROR_FLAG_MASK))
    {
        i2cm_error_handle(i2c_instance);
    }

    /* Arbitration lost interrupt */
    if (((int_en&I2C_I2CM_INT_EN_ARBITRATION_LOST_EN_MASK) == I2C_I2CM_INT_EN_ARBITRATION_LOST_EN_MASK) && 
        ((int_flag&I2C_I2CM_STAT_ARBITRATION_LOST_FLAG_MASK) == I2C_I2CM_STAT_ARBITRATION_LOST_FLAG_MASK))
    {
        /* End transfer: no stop generation (the module will handle that by itself if needed), reset FIFOs */
        i2cm_end_transfer(i2c_instance, false, true);
    }

/* OCS_Reserved_Start */
#ifdef FLAG_TEST
    if (g_i2c_master_int_handle[i2c_instance] != NULL)
    {
        /* data match interrupt */
        if (((int_en&I2C_I2CM_INT_EN_DATA_MATCH_EN_MASK) == I2C_I2CM_INT_EN_DATA_MATCH_EN_MASK) && 
            ((int_flag&I2C_I2CM_STAT_DATA_MATCH_FLAG_MASK) == I2C_I2CM_STAT_DATA_MATCH_FLAG_MASK))
        {
            /* user can add application here */
            g_i2c_master_int_handle[i2c_instance](I2C_DATA_MATCH);
        }
                                
        /* pin low interrupt */
        if (((int_en&I2C_I2CM_INT_EN_PIN_LOW_TIMEOUT_EN_MASK) == I2C_I2CM_INT_EN_PIN_LOW_TIMEOUT_EN_MASK) && 
            ((int_flag&I2C_I2CM_STAT_PIN_LOW_TIMEOUT_FLAG_MASK) == I2C_I2CM_STAT_PIN_LOW_TIMEOUT_FLAG_MASK))
        {
            /* user can add application here */
            g_i2c_master_int_handle[i2c_instance](I2C_PIN_LOW);
        }
    }
#endif

/* OCS_Reserved_End */
}
#endif

/**
 *@brief Enable i2c master interrupt.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@param[in] i2c_master_config: point to master configuration
 *@return    none
 */
static void i2cm_interrupt_init(uint32_t i2c_instance, const i2c_master_config_t *i2c_master_config)      //use
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);

//        if (i2c_master_config->master_mode == I2C_ULTRAFAST_MODE)
//        {
//            /* Do not enable NACK event reporting in ultra-fast mode */
//            i2cm_drv_int_cfg(i2c_ptr, 
//                            I2C_MASTER_FIFO_ERROR_INT | 
//                            I2C_MASTER_ARBITRATION_LOST_INT,
//                            true);
//        }
//        else
//        {
            i2cm_drv_int_cfg(i2c_ptr, 
                //In the interrupt service function, must clear the INT_M_TX_EMPTY flag, otherwise the program will hang after I2C is enabled.
                INT_M_RX_FULL | 
                INT_M_TX_ABRT | INT_M_TX_OVER | INT_M_RX_OVER |
                INT_M_STOP_DET | INT_M_START_DET | INT_M_ACTIVITY,
                            true);
//        }

//    g_i2c_master_int_handle[i2c_instance] = i2c_master_config->master_int_handle;
	if (I2C_NVIC_DISABLE == NVIC_GetEnableIRQ(g_i2c_master_irqn[i2c_instance]))
    {
		NVIC_EnableIRQ(g_i2c_master_irqn[i2c_instance]);
		NVIC_SetPriority(g_i2c_master_irqn[i2c_instance], i2c_master_config->master_interrupt_priority);
    }
}

/**
 *@brief Send start event and slave address parameter, specify the direction of the transfer.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@param[in] i2c_master_msg: point to i2c msg configuration and buffer
 *@param[in] is_read: send read or write cmd
 *@return    none
 */
#if 0
static void i2cm_address_arrange(uint32_t i2c_instance, i2c_master_msg_t *i2c_master_msg, bool is_read)
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);
    uint8_t address;
    i2c_master_cmd_e i2c_master_cmd = I2C_MASTER_CMD_RESTART_WRITE;     //need to change


    if (i2c_master_msg->addr_10bit == true)
    {
        /* First address byte: 1111 0XXD, where XX are bits 10 and 9 of address */
        address = (uint8_t)(I2C_10BIT_ADDR_HEADER + ((i2c_master_msg->slave_addr >> I2C_ADDR_RIGHT_SHIFT_7) & I2C_10BIT_ADDR_HIGH_MASK) + WRITE_CMD);
        i2cm_fifo_cmd_cfg(i2c_instance, i2c_master_cmd, address);
        
        /* Second address byte: address low 8bit */
        address = (uint8_t)(i2c_master_msg->slave_addr & I2C_10BIT_ADDR_LOW_MASK);
        i2cm_fifo_cmd_cfg(i2c_instance, I2C_MASTER_CMD_WRITE, address);
        
        if (is_read == true)
        {
            /* 10bit address - must send repeated start and resend first address byte */
            address = (uint8_t)(I2C_10BIT_ADDR_HEADER + ((i2c_master_msg->slave_addr >> I2C_ADDR_RIGHT_SHIFT_7) & I2C_10BIT_ADDR_HIGH_MASK) + READ_CMD);
            i2cm_fifo_cmd_cfg(i2c_instance, i2c_master_cmd, address);
        }
    }
    else /* 7bit address + rw cmd */
    {
        if (is_read == true)
        {
            address = (uint8_t)((i2c_master_msg->slave_addr << I2C_ADDR_LEFT_SHIFT_1) + READ_CMD);
        }
        else
        {
            address = (uint8_t)((i2c_master_msg->slave_addr << I2C_ADDR_LEFT_SHIFT_1) + WRITE_CMD);
        }
        
        i2cm_fifo_cmd_cfg(i2c_instance, i2c_master_cmd, address);
    }
}
#endif

/*******************************************************************************
* Public I2C MASTER Functions
******************************************************************************/ 
/**
 *@brief Initialize sw tx/rx data buffer.
 *@param[in] i2c_master_msg: point to master msg config and sw buffer
 *@return    none
 */
void i2cm_buffer_init(i2c_master_msg_t *i2c_master_msg)      //use
{
    DEV_ASSERT(i2c_master_msg != NULL);
    /* initialize tx data buffer */
    g_i2c_master.tx_length = i2c_master_msg->send_data_len;
    g_i2c_master.tx_data = i2c_master_msg->send_data;
    
    /* initialize rx data buffer */
    g_i2c_master.rx_length = i2c_master_msg->rec_data_len;
    g_i2c_master.rx_data = i2c_master_msg->rec_data;
}

/**
 *@brief Enable dmatch int, disable receive data int, configure i2c master data match function.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@param[in] dmatch_config: point to the configuration of data match
 *@return    none
 */    
void i2cm_dmatch_init(uint32_t i2c_instance, const i2c_master_dmatch_config_t *dmatch_config)
{
    /*
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);

    i2cm_drv_int_cfg(i2c_ptr, I2C_MASTER_DATA_MATCH_INT, true);
    i2cm_drv_int_cfg(i2c_ptr, I2C_MASTER_RECEIVE_DATA_INT, false);
    i2cm_drv_dmatch_cfg(i2c_ptr, dmatch_config);
    */
}

/**
 *@brief Enable pinlow int, the situation SCL or SDA remains low for longer than 
 *                (timeout * 256) cycles will trigger an int, the cycle is up to i2c function clock.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@param[in] timeout: the actual detect time is (timeout * 256) cycles
 *@return    none
 */    
void i2cm_pinlow_init(uint32_t i2c_instance, uint32_t timeout)
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);

    i2cm_drv_pinlow_cfg(i2c_ptr, timeout);
    i2cm_drv_int_cfg(i2c_ptr, I2C_MASTER_PIN_LOW_TIMEOUT_INT, true);
}

/**
 *@brief Initialize i2c clock, pin function and sw data buf.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@param[in] i2c_master_config, point to master configuration
 *@return      i2c master initialize execute result status:
 *                - STATUS_PARAM_ERROR,
 *                - STATUS_ERROR,
 *                - STATUS_SUCCESS,
 */
status_t i2c_master_init(uint32_t i2c_instance, const i2c_master_config_t *i2c_master_config)      //use
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    status_t ret;
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);
    uint32_t i2c_clk = I2C_CLK_INITIAL_VALUE;
    /* 1) reset sw buffer idx */
    g_i2c_master.write_idx = I2C_DEFAULT_INDEX_0;
    g_i2c_master.read_idx = I2C_DEFAULT_INDEX_0;

    /* 2) init i2c hardware */
/* OCS_Reserved_Start */
#ifdef ASIC_MODE
/* OCS_Reserved_End */
		const module_name_e clock_module_name[I2C_INSTANCE_MAX_COUNT] = {MODULE_I2C0, MODULE_I2C1, MODULE_I2C2};
    (void)clock_freq_get(clock_module_name[i2c_instance], &i2c_clk);
/* OCS_Reserved_Start */
#else
    i2c_clk = HIRC_FREQ;
    const module_name_e clock_module_name[I2C_INSTANCE_MAX_COUNT] = {MODULE_I2C0, MODULE_I2C1};
    (void)clock_freq_get(clock_module_name[i2c_instance], &i2c_clk);
		
#endif
/* OCS_Reserved_End */
	
    ret = i2cm_drv_init(i2c_ptr, i2c_master_config, i2c_clk);
    if (ret == STATUS_SUCCESS)
    {
        /* 3) init i2c interrupt */
        i2cm_interrupt_init(i2c_instance, i2c_master_config);
    }
    i2cm_drv_test_send_data(i2c_ptr, 0, 0x45);
    return ret;
}

/**
 *@brief This function reset hardware FIFO and de-initializes the I2C module, reset sw buffer, disable NVIC.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@return    none
 */
void i2cm_deinit(uint32_t i2c_instance)
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);
    
    i2cm_drv_reset_fifo(i2c_ptr);
    i2cm_drv_deinit(i2c_ptr);
    
    g_i2c_master.tx_data = NULL;
    g_i2c_master.tx_length = I2C_DEFAULT_LENGTH_0;
    
    g_i2c_master.rx_data = NULL;
    g_i2c_master.rx_length = I2C_DEFAULT_LENGTH_0;

    NVIC_DisableIRQ(g_i2c_master_irqn[i2c_instance]);
}

/**
 *@brief Perform a non-blocking send transaction on the I2C bus.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@param[in] i2c_master_msg: point to i2c msg configuration and buffer
 *@return    none
 */
void i2cm_send_data(uint32_t i2c_instance, i2c_master_msg_t *i2c_master_msg)      //use
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);

    /* 0) initialize data buffer first */
    i2cm_buffer_init(i2c_master_msg);
    
    g_i2c_master.i2c_transmit_status = true;
    /* 1) Initiate communication: fill cmd to tx fifo */
    i2cm_drv_slv_addr(i2c_ptr, i2c_master_msg);
    i2c_ptr->IC_INTR_MASK.M_TX_EMPTY = 1;
//    i2cm_address_arrange(i2c_instance, i2c_master_msg, false);
    
    /* 2) fill data to tx fifo */
    //i2cm_fifo_data_update(i2c_instance);
    
    /* 3) set watermark to 0U to send all data in FIFO */
    //i2cm_drv_watermark_set(i2c_ptr, I2C_DEFAULT_TX_WATERMARK_VALUE, I2C_TX_WATERMARK);
    
    /* 4) enable tx interrupt */
    i2cm_drv_int_cfg(i2c_ptr, I2C_MASTER_FIFO_ERROR_INT |           //need to change
                              I2C_MASTER_ARBITRATION_LOST_INT |
                              I2C_MASTER_TRANSMIT_DATA_INT,
                              true);
}

/**
 *@brief Perform a non-blocking receive transaction on the I2C bus.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@param[out] i2c_master_msg: point to i2c msg configuration and buffer
 *@return    none
 */
void i2cm_receive_data(uint32_t i2c_instance, i2c_master_msg_t *i2c_master_msg)      //use
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);

    /* 0) initialize data buffer first */
    i2cm_buffer_init(i2c_master_msg);
    g_i2c_master.i2c_transmit_status = false;
    g_i2c_master.tx_length = g_i2c_master.rx_length;
    
    /* 1) Initiate communication */
    i2cm_drv_slv_addr(i2c_ptr, i2c_master_msg);
    //i2cm_address_arrange(i2c_instance, i2c_master_msg, true);
    i2cm_drv_tdata_send(i2c_ptr, I2C_MASTER_CMD_WRITE, (uint8_t)i2c_master_msg->slave_reg_addr);      //发送要读取的寄存器地址
    i2c_ptr->IC_INTR_MASK.M_TX_EMPTY = 1;
    i2c_ptr->IC_INTR_MASK.M_RX_FULL = 1;

//    i2cm_drv_tdata_send(i2c_ptr, I2C_MASTER_CMD_WRITE, 0x4F);      //发送要读取的寄存器地址
    /* 2) Queue receive command for receive data bytes */
    //i2cm_fifo_cmd_cfg(i2c_instance, I2C_MASTER_CMD_READ, (uint8_t)(i2c_master_msg->rec_data_len-1U));
    
    /* 3) set rx fifo watermark */
    //i2cm_drv_watermark_set(i2c_ptr, I2C_DEFAULT_RX_WATERMARK_VALUE, I2C_RX_WATERMARK);
    
    /* 4) disable tx interrupt and enable rx interrupt to avoid restart and stop conflict */
    //i2cm_drv_int_cfg(i2c_ptr, I2C_MASTER_TRANSMIT_DATA_INT, false);             //need to change
    //i2cm_drv_int_cfg(i2c_ptr, I2C_MASTER_FIFO_ERROR_INT |                       //need to change
    //                          I2C_MASTER_ARBITRATION_LOST_INT |
    //                          I2C_MASTER_RECEIVE_DATA_INT,
    //                          true);
}

/**
 *@brief Get the tx register address.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@return      the tx register address
 */    
uint32_t i2cm_tx_address_get(uint32_t i2c_instance)
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);
    uint32_t tx_addr = i2cm_drv_tx_address_get(i2c_ptr);
    return tx_addr;
}

/**
 *@brief Get the rx register address.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@return      the rx register address
 */    
uint32_t i2cm_rx_address_get(uint32_t i2c_instance)
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);
    uint32_t rx_addr = i2cm_drv_rx_address_get(i2c_ptr);
    return rx_addr;
}

/**
 *@brief Ends current transmission or reception.
 *@param[in] i2c_ptr: point to i2c base address
 *@param[in] stop_cmd: true, send stop cmd; false, do not send stop cmd
 *@param[in] reset_fifo: true, reset I2C fifo, false, do not reset I2C fifo.
 *@return    none
 */
void i2cm_end_transfer(uint32_t i2c_instance, bool stop_cmd, bool reset_fifo)
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);

    if (stop_cmd == true)
    {
        i2cm_drv_tdata_send(i2c_ptr, I2C_MASTER_CMD_WRITE_STOP, I2C_DEFAULT_CMD_DATA_VALUE);
    }

    if (reset_fifo == true)
    {
        i2cm_drv_reset_fifo(i2c_ptr);
    }
}











/*******************************************************************************
* Private I2C SLAVE Functions
******************************************************************************/ 
/**
 *@brief Enable i2cm interrupt.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@param[in] i2c_slave_config: point to salve configuration
 *@return    none
 */
#if 0
static void i2cs_interrupt_init(uint32_t i2c_instance, const i2c_slave_config_t *i2c_slave_config)
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);

    {
        i2cs_drv_int_cfg(i2c_ptr, I2C_SLAVE_BIT_ERROR_INT |
                        I2C_SLAVE_FIFO_ERROR_INT |
                        I2C_SLAVE_STOP_DETECT_INT |
                        I2C_SLAVE_REPEATED_START_INT |
                        I2C_SLAVE_ADDRESS_VALID_INT |
                        I2C_SLAVE_RECEIVE_DATA_INT |
                        I2C_SLAVE_TRANSMIT_DATA_INT,
                        true);
    }
    
		if (I2C_NVIC_DISABLE == NVIC_GetEnableIRQ(g_i2c_slave_irqn[i2c_instance]))
    {
				g_i2c_slave_int_handle[i2c_instance] = i2c_slave_config->slave_int_handle;
				NVIC_EnableIRQ(g_i2c_slave_irqn[i2c_instance]);
    }
}
#endif

/**
 *@brief Handle slave end transfer operations when error happens. 
 *          This function will disable interrupt, disable I2C slave module and reset sw buffer.
 *          Then invoke callback function if installed.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@return    none
 */
#if 0
static void i2c_slave_error_handle(uint32_t i2c_instance)
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);
	
    /* disable all interrupts */
    i2cs_drv_int_cfg(i2c_ptr, I2C_SLAVE_BIT_ERROR_INT |
                    I2C_SLAVE_FIFO_ERROR_INT |
                    I2C_SLAVE_STOP_DETECT_INT |
                    I2C_SLAVE_REPEATED_START_INT |
                    I2C_SLAVE_ADDRESS_VALID_INT |
                    I2C_SLAVE_RECEIVE_DATA_INT |
                    I2C_SLAVE_TRANSMIT_DATA_INT,
                    false);
                          
    /* disable slave instance */
    i2cs_drv_deinit(i2c_ptr);

    /* reset sw buffer */
    g_i2c_slave.tx_data = NULL;
    g_i2c_slave.tx_length = I2C_DEFAULT_LENGTH_0;
    
    g_i2c_slave.rx_data = NULL;
    g_i2c_slave.rx_length = I2C_DEFAULT_LENGTH_0;
    
    /* record the error event */
    if (g_i2c_slave_int_handle[i2c_instance] != NULL)
    {
        g_i2c_slave_int_handle[i2c_instance](I2C_ERROR);
    }
}
#endif

/**
 *@brief Handle an address valid event for slave.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@return    none
 */
#if 0
 static void i2c_slave_receive_address_handle(uint32_t i2c_instance)
{

    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);

    /* Address Valid Flag is cleared by reading the address status register */
    uint32_t receive_addr = i2cs_drv_slave_addr_get(i2c_ptr);

    if ((receive_addr & I2C_CMD_RW_MASK) == WRITE_CMD)
    {
        
    }
    else /* read cmd */
    {
				i2cs_drv_int_cfg(i2c_ptr, I2C_SLAVE_TRANSMIT_DATA_INT, true);
    }
}
#endif

/**
 *@brief Handle a transmit data event for slave.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@return    none
 */
#if 0
 static void i2c_slave_transmit_handle(uint32_t i2c_instance)
{

    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);

    if (g_i2c_slave.tx_length == I2C_DEFAULT_LENGTH_0)
    {
        i2cs_drv_int_cfg(i2c_ptr, I2C_SLAVE_TRANSMIT_DATA_INT, false);
        if (g_i2c_slave_int_handle[i2c_instance] != NULL)
        {
            g_i2c_slave_int_handle[i2c_instance](I2C_TX_FINISH);
        }
    }
    else
    {
        i2cs_drv_txdata_set(i2c_ptr, g_i2c_slave.tx_data[I2C_DEFAULT_INDEX_0]);
        g_i2c_slave.tx_data++;
        g_i2c_slave.tx_length--;
    }
}
#endif

/**
 *@brief Handle a receive data event for slave.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@return    none
 */
#if 0
 static void i2c_slave_receive_handle(uint32_t i2c_instance)
{

    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);

    g_i2c_slave.rx_data[I2C_DEFAULT_INDEX_0] = i2cs_drv_rxdata_get(i2c_ptr);
    
    g_i2c_slave.rx_data++;
    g_i2c_slave.rx_length--;

    /* receive finish */
    if (g_i2c_slave.rx_length == I2C_DEFAULT_LENGTH_0)
    {
        if (g_i2c_slave_int_handle[i2c_instance] != NULL)
        {
            g_i2c_slave_int_handle[i2c_instance](I2C_RX_FINISH);
        }
    }

}
#endif

/**
 *@brief Handle an interrupt process for i2c slave.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@return    none
 */
#if 0
 static void i2c_slave_int_handle(uint32_t i2c_instance)
{

    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);
    bool stop_cmd, repear_start;
    uint32_t int_flag = i2cs_drv_intsts_get(i2c_ptr);
    uint32_t int_en = i2cs_drv_inten_get(i2c_ptr);
    
    /* Check for error event */
    if ((int_flag&I2C_I2CS_STAT_BIT_ERR_FLAG_MASK) == I2C_I2CS_STAT_BIT_ERR_FLAG_MASK)
    {
        i2cs_drv_intsts_clear(i2c_ptr, I2C_I2CS_STAT_BIT_ERR_FLAG_MASK);
        i2c_slave_error_handle(i2c_instance);
    }
    else if ((int_flag&I2C_I2CS_STAT_FIFO_ERR_FLAG_MASK) == I2C_I2CS_STAT_FIFO_ERR_FLAG_MASK)
    {        
        /* In Ultra-Fast mode clock stretching is disabled, so it is possible to get
        this event if the slave can't keep up */
        i2cs_drv_intsts_clear(i2c_ptr, I2C_I2CS_STAT_FIFO_ERR_FLAG_MASK);
        i2c_slave_error_handle(i2c_instance);
    }
    else
    {
        /* Check for repeated start or stop condition */
        stop_cmd = ((i2cs_drv_intsts_get(i2c_ptr) & I2C_I2CS_STAT_STOP_DETECT_FLAG_MASK) == I2C_I2CS_STAT_STOP_DETECT_FLAG_MASK);
        repear_start = ((i2cs_drv_intsts_get(i2c_ptr) & I2C_I2CS_STAT_REPERT_START_FLAG_MASK) == I2C_I2CS_STAT_REPERT_START_FLAG_MASK);

        if (repear_start == true)
        {
            g_i2c_slave.repeat_start_num++;

            /* 10 bit addr must have a restart */
            if ((g_i2c_slave.repeat_start_num == 1U) && (g_i2c_slave.addr_10bit == true))
            {
                repear_start = false;
                i2cs_drv_intsts_clear(i2c_ptr, I2C_I2CS_STAT_REPERT_START_FLAG_MASK);
            }
        }

        if ((stop_cmd == true) || (repear_start == true))
        {
            i2cs_drv_intsts_clear(i2c_ptr, I2C_I2CS_STAT_REPERT_START_FLAG_MASK);
            i2cs_drv_intsts_clear(i2c_ptr, I2C_I2CS_STAT_STOP_DETECT_FLAG_MASK);
            /* transfer finish, do nothing */
        }
        else
        {
            if (((int_en&I2C_I2CS_INT_EN_ADDR_VALID_EN_MASK) == I2C_I2CS_INT_EN_ADDR_VALID_EN_MASK) && 
                 ((int_flag&I2C_I2CS_STAT_ADDR_VALID_FLAG_MASK) == I2C_I2CS_STAT_ADDR_VALID_FLAG_MASK))
            {                
                i2c_slave_receive_address_handle(i2c_instance);
            }

            if (((int_en&I2C_I2CS_INT_EN_TRANSMIT_DATA_EN_MASK) == I2C_I2CS_INT_EN_TRANSMIT_DATA_EN_MASK) && 
                ((int_flag&I2C_I2CS_STAT_TRANSMIT_DATA_FLAG_MASK) == I2C_I2CS_STAT_TRANSMIT_DATA_FLAG_MASK))
            {
                i2c_slave_transmit_handle(i2c_instance);
            }
            
            if (((int_en&I2C_I2CS_INT_EN_RECEIVE_DATA_EN_MASK) == I2C_I2CS_INT_EN_RECEIVE_DATA_EN_MASK) && 
                 ((int_flag&I2C_I2CS_STAT_RECEIVE_DATA_FLAG_MASK) == I2C_I2CS_STAT_RECEIVE_DATA_FLAG_MASK))
            {
                i2c_slave_receive_handle(i2c_instance);
            }
/* OCS_Reserved_Start */
#ifdef FLAG_TEST
            if (g_i2c_slave_int_handle[i2c_instance] != NULL)
            {
                if (((int_en&I2C_I2CS_INT_EN_ADDR_MATCH0_EN_MASK) == I2C_I2CS_INT_EN_ADDR_MATCH0_EN_MASK) && 
                    ((int_flag&I2C_I2CS_STAT_ADDR_MATCH0_FLAG_MASK) == I2C_I2CS_STAT_ADDR_MATCH0_FLAG_MASK))
                {
                    g_i2c_slave_int_handle[i2c_instance](I2C_ADDR_MATCH);
                }
                    
                if (((int_en&I2C_I2CS_INT_EN_ADDR_MATCH1_EN_MASK) == I2C_I2CS_INT_EN_ADDR_MATCH1_EN_MASK) && 
                    ((int_flag&I2C_I2CS_STAT_ADDR_MATCH1_FLAG_MASK) == I2C_I2CS_STAT_ADDR_MATCH1_FLAG_MASK))
                {
                    g_i2c_slave_int_handle[i2c_instance](I2C_ADDR_MATCH);
                }
                    
                if (((int_en&I2C_I2CS_INT_EN_GENERAL_CALL_EN_MASK) == I2C_I2CS_INT_EN_GENERAL_CALL_EN_MASK) && 
                    ((int_flag&I2C_I2CS_STAT_GENERAL_CALL_FLAG_MASK) == I2C_I2CS_STAT_GENERAL_CALL_FLAG_MASK))
                {
                    g_i2c_slave_int_handle[i2c_instance](I2C_GENERAL_CALL);
                }
                    
                if (((int_en&I2C_I2CS_INT_EN_SMBUS_ALERT_REP_EN_MASK) == I2C_I2CS_INT_EN_SMBUS_ALERT_REP_EN_MASK) && 
                  ((int_flag&I2C_I2CS_STAT_SMBUS_ALERT_REP_FLAG_MASK) == I2C_I2CS_STAT_SMBUS_ALERT_REP_FLAG_MASK))
                {
                    g_i2c_slave_int_handle[i2c_instance](I2C_SMBUS_ALERT);
                }
            }
#endif
/* OCS_Reserved_End */
        }
    }
}
#endif

/**
 *@brief Initialize sw tx/rx data buffer.
 *@param[in] i2c_slave_msg: point to slave msg config and sw buffer
 *@return    none
 */
void i2cs_buffer_init(i2c_slave_msg_t *i2c_slave_msg)
{    
    /* initialize tx data buffer */
    g_i2c_slave.tx_length = i2c_slave_msg->send_data_len;
    g_i2c_slave.tx_data = i2c_slave_msg->send_data;

    /* initialize rx data buffer */
    g_i2c_slave.rx_length = i2c_slave_msg->rec_data_len;
    g_i2c_slave.rx_data = i2c_slave_msg->rec_data;
}

/**
 *@brief This function configure i2c slave filter SCL&SDA function. Note that filter must set before i2c enable.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@param[in] enable: true for enable, false for disable the filter function
 *@param[in] filter_sda: The filter latency is FILTSDA+3 cycles, the cycle is from i2c function clock.
 *@param[in] filter_scl: The filter latency is FILTSCL+3 cycles, the cycle is from i2c function clock.
 *@return    none
 */
void i2cs_filter_cfg(uint32_t i2c_instance, bool enable, uint8_t filter_sda, uint8_t filter_scl)
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);

    i2cs_drv_filter_cfg(i2c_ptr, enable, filter_sda, filter_scl);
}

/**
 *@brief The function initialize i2c clock, pin function and sw data buf for slave.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@param[in] i2c_slave_config: point to the configuration of i2c slave
 *@param[in] i2c_slave_msg: point to slave msg config and sw buffer
 *@return    none
 */
#if 0
status_t i2c_slave_init(uint32_t i2c_instance, const i2c_slave_config_t *i2c_slave_config, i2c_slave_msg_t *i2c_slave_msg)
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    
    status_t ret;
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);
    i2cs_buffer_init(i2c_slave_msg);
    ret = i2cs_drv_init(i2c_ptr, i2c_slave_config);
	
		i2cs_drv_tx_fifo_enable(i2c_ptr, I2C_SLAVE_TX_WATERMARK_DEFAULT_VALUE);
		for (uint32_t i=0; i<I2C_SLAVE_TX_WATERMARK_MAX_VALUE; i++)
		{
						i2cs_drv_txdata_set(i2c_ptr, *g_i2c_slave.tx_data);
						g_i2c_slave.tx_data++;
						g_i2c_slave.tx_length--;
		}            

		g_i2c_slave.addr_10bit = i2c_slave_config->addr_10bit;
		i2cs_interrupt_init(i2c_instance, i2c_slave_config);
		
    return ret;
}
#endif

/**
 *@brief This function de-initializes the I2C module, reset sw buffer, disable NVIC.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@return    none
 */
void i2cs_deinit(uint32_t i2c_instance)
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);
    i2cs_drv_deinit(i2c_ptr);


    g_i2c_slave.tx_data = NULL;
    g_i2c_slave.tx_length = I2C_DEFAULT_LENGTH_0;

    g_i2c_slave.rx_data = NULL;
    g_i2c_slave.rx_length = I2C_DEFAULT_LENGTH_0;

    NVIC_DisableIRQ(g_i2c_slave_irqn[i2c_instance]);
}

/**
 *@brief Get the tx register address.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@return    the tx register address
 */    
uint32_t i2cs_tx_address_get(uint32_t i2c_instance)
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);
    uint32_t tx_addr = i2cs_drv_tx_address_get(i2c_ptr);

    return tx_addr;
}

/**
 *@brief Get the rx register address.
 *@param[in] i2c_instance: one of <I2C_INSTANCE0, I2C_INSTANCE1>
 *@return      the rx register address
 */    
uint32_t i2cs_rx_address_get(uint32_t i2c_instance)
{
    DEV_ASSERT(i2c_instance < I2C_INSTANCE_MAX_COUNT);
    
    I2C_TypeDef *i2c_ptr = i2c_addr_get(i2c_instance);
    uint32_t rx_addr = i2cs_drv_rx_address_get(i2c_ptr);
    
    return rx_addr;
}

/**
*@brief  Implementation of I2C0_IRQHandler.
*@param  none
*@return none
*/

void I2C0_IRQHandler(void)      //use
{    
		I2C_TypeDef *i2c_ptr = i2c_addr_get((uint32_t)0);

		
        if(i2c_ptr->IC_INTR_STAT.R_TX_EMPTY){
            if(g_i2c_master.tx_length == 0){    //当缓冲区为空时关闭FIFO为空中断
                i2c_ptr->IC_INTR_MASK.M_TX_EMPTY = 0;
                (void)i2c_ptr->IC_CLR_INTR; // 清除全部可清除标志位
				return;
            }
            if(g_i2c_master.i2c_transmit_status == true){  
                i2cm_drv_tdata_send(i2c_ptr, I2C_MASTER_CMD_WRITE, g_i2c_master.tx_data[I2C_DEFAULT_INDEX_0]);
            } else {
                i2cm_drv_tdata_send(i2c_ptr, I2C_MASTER_CMD_READ, g_i2c_master.tx_data[I2C_DEFAULT_INDEX_0]);
            }
            g_i2c_master.tx_data++;
            g_i2c_master.tx_length--;
        }

        
        
        if(i2c_ptr->IC_INTR_STAT.R_RX_FULL){    //接收缓冲区有数据
            *(g_i2c_master.rx_data) = i2c_ptr->IC_DATA_CMD.DAT;
            if(g_i2c_master.rx_length == 0){    //收到指定长度的数据
                i2c_ptr->IC_INTR_MASK.M_RX_FULL = 0;
                (void)i2c_ptr->IC_CLR_INTR; // 清除全部可清除标志位
                return;
            }
            g_i2c_master.rx_data++;
            g_i2c_master.rx_length--;
            
        }
        (void)i2c_ptr->IC_CLR_INTR; // 清除全部可清除标志位
#if 0

		I2C_TypeDef *i2c_ptr = i2c_addr_get((uint32_t)0);
		(void)i2c_ptr->IC_CLR_INTR; // 清除全部可清除标志位
		(void)i2c_ptr->IC_CLR_RX_UNDER; // 清除RX_UNDER标志位
		if (i2c_ptr->IC_CON.MASTER_MODE == 1)
		{
			i2c_master_int_handle((uint32_t)0);
		}
		else
		{
			i2c_slave_int_handle((uint32_t)0);
		}
#endif
}

/**
*@brief  Implementation of I2C1_IRQHandler.
*@param  none
*@return none
*/
void I2C1_IRQHandler(void)      //use
{    
    I2C_TypeDef *i2c_ptr = i2c_addr_get((uint32_t)0);
    if(i2c_ptr->IC_INTR_STAT.R_TX_EMPTY){
        if(g_i2c_master.tx_length == 0){    //当缓冲区为空时关闭FIFO为空中断
            i2c_ptr->IC_INTR_MASK.M_TX_EMPTY = 0;
            (void)i2c_ptr->IC_CLR_INTR; // 清除全部可清除标志位
			return;
        }
        if(g_i2c_master.i2c_transmit_status == true){  
            i2cm_drv_tdata_send(i2c_ptr, I2C_MASTER_CMD_WRITE, g_i2c_master.tx_data[I2C_DEFAULT_INDEX_0]);
        } else {
            i2cm_drv_tdata_send(i2c_ptr, I2C_MASTER_CMD_READ, g_i2c_master.tx_data[I2C_DEFAULT_INDEX_0]);
        }
        g_i2c_master.tx_data++;
        g_i2c_master.tx_length--;
    }
    
    
    if(i2c_ptr->IC_INTR_STAT.R_RX_FULL){    //接收缓冲区有数据
        *(g_i2c_master.rx_data) = i2c_ptr->IC_DATA_CMD.DAT;
        if(g_i2c_master.rx_length == 0){    //收到指定长度的数据
            i2c_ptr->IC_INTR_MASK.M_RX_FULL = 0;
            (void)i2c_ptr->IC_CLR_INTR; // 清除全部可清除标志位
            return;
        }
        g_i2c_master.rx_data++;
        g_i2c_master.rx_length--;
        
    }
    (void)i2c_ptr->IC_CLR_INTR; // 清除全部可清除标志位
#if 0
    I2C_TypeDef *i2c_ptr = i2c_addr_get((uint32_t)1);
		if (i2c_ptr->IC_CON.MASTER_MODE == 1)
		{
			i2c_master_int_handle((uint32_t)1);
		}
		else
		{
			i2c_slave_int_handle((uint32_t)1);
		}
#endif
}



