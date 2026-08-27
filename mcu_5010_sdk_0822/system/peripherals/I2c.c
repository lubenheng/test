#include "platform_common.h"

#include "i2c_type.h"
#include "i2c_reg.h"
#include "device_resource.h"
#include "i2c.h"


/*******************************************************************************
* Definitions
******************************************************************************/
/** I2C CTRL reg value for initial */
#define I2C_CTRL_INITIAL_VALUE            (0x0U)
/** I2C filter mask */
#define I2C_REG_FILTER_MASK               (0xFU)
/** I2C prescaler initial define */
#define I2C_PRE_INITIAL_VALUE             (0U)
/** I2C clk total initial define */
#define I2C_CLK_TOTAL_INITIAL_VALUE       (0U)
/** I2C baudrate initial define */
#define I2C_BAUDRATE_INITIAL_VALUE        (0U)
/*******************************************************************************
* Private I2C MASTER Control Functions
******************************************************************************/ 
/**
 *@brief Set the baudrate for any subsequent I2C communication except high-speed mode.
 *@param[in] i2c_ptr: point to i2c base address
 *@param[in] baudrate: the baudrate need to be cfg
 *@param[in] clock_freq: the current I2C function clock frequency
 *@param[in] i2c_speed_mode: I2C speed mode
 *@return    i2c master baudrate configuration(except high-speed mode) execute result status:
 *                - STATUS_PARAM_ERROR,
 *                - STATUS_SUCCESS,
 */
static status_t i2cm_drv_baudrate_cfg(I2C_TypeDef *i2c_ptr, uint32_t baudrate, i2c_mode_e i2c_speed_mode, uint32_t clock_freq)      //use
{
    i2c_ptr->IC_SS_SCL_HCNT &= 0X0;
    i2c_ptr->IC_SS_SCL_LCNT &= 0X0;
    i2c_ptr->IC_FS_SCL_HCNT &= 0X0;
    i2c_ptr->IC_FS_SCL_LCNT &= 0X0;
    i2c_ptr->IC_HS_SCL_HCNT &= 0X0;
    i2c_ptr->IC_HS_SCL_LCNT &= 0X0;

    float T_scl = 0.0f;
    float T_hight = 0.0f;
    float T_low = 0.0f;
    float HCNT = 0.0f;
    float LCNT= 0.0f;
    float ic_clk = 0.0f;

    ic_clk = (1.0 / clock_freq) * 1000000000.0;
    T_scl = 1000000000.0 / (float)baudrate;
    T_hight = T_scl / 2.0;
    T_low = T_hight;
    HCNT = (T_hight - 100.0) / ic_clk - 8.0;
    LCNT = (T_low - 200.0) / ic_clk + 5.0;
    INFO("T_scl:%f T_hight:%f T_low:%f HCNT:%f LCNT:%f ic_clk:%f\n", T_scl, T_hight, T_low, HCNT, LCNT, ic_clk);

    i2c_ptr->IC_SS_SCL_HCNT = (uint32_t)HCNT;
    i2c_ptr->IC_SS_SCL_LCNT = (uint32_t)LCNT;
    i2c_ptr->IC_FS_SCL_HCNT = (uint32_t)HCNT;
    i2c_ptr->IC_FS_SCL_LCNT = (uint32_t)LCNT;
    i2c_ptr->IC_HS_SCL_HCNT = (uint32_t)HCNT;
    i2c_ptr->IC_HS_SCL_LCNT = (uint32_t)LCNT;

	return (status_t)0;
}
 
/**
 *@brief Set the baudrate for any subsequent I2C communication only for high-speed mode.
 *@param[in] i2c_ptr: point to i2c base address
 *@param[in] baudrate: the baudrate need to be cfg
 *@param[in] clock_freq: the current I2C function clock frequency
 *@return    i2c master baudrate configuration execute result status:
 *                - STATUS_PARAM_ERROR,
 *                - STATUS_SUCCESS,
 */
#if 0
static status_t i2cm_drv_hsbaudrate_cfg(I2C_TypeDef *i2c_ptr, uint32_t baudrate, uint32_t clock_freq)
{

    status_t ret = STATUS_SUCCESS;
    uint32_t prescaler;
    uint32_t clk_total = I2C_CLK_TOTAL_INITIAL_VALUE;
    uint32_t clk_low;
    uint32_t clk_high;
    uint32_t setup_hold;
    uint32_t data_valid_delay;

    if (baudrate != I2C_BAUDRATE_INITIAL_VALUE)
    {
        prescaler = i2c_ptr->I2CM_CFG1.PRESCALE;
        /* compute the real clk number, t = 2^pre*num/clk_freq, t = 1/bit, round to integer. */
        clk_total = (clock_freq + ((baudrate << prescaler) >> 1U)) / (baudrate << prescaler);
    }

    if ((clk_total > CLK_TOTOL) || (clk_total <= CLK_HIGH_MIN))
    {
/* OCS_Reserved_Start */
        INFO("wrong clk_total:%d\n",clk_total);
/* OCS_Reserved_End */
        ret = STATUS_PARAM_ERROR;
    }

    clk_high = (clk_total - CLK_DEFAULT_OFFSET) / 2U;
    clk_low = clk_total - CLK_DEFAULT_OFFSET - clk_high;
    if ((clk_high < CLK_HIGH_MIN) || (clk_low < CLK_LOW_MIN))
    {
/* OCS_Reserved_Start */
        INFO("wrong clk_high:%d\n",clk_high);
        INFO("wrong clk_low:%d\n",clk_low);
/* OCS_Reserved_End */
        ret = STATUS_PARAM_ERROR;
    }

    setup_hold = clk_high;
    data_valid_delay = clk_high >> 1U;
    if ((setup_hold < SETHOLD_MIN) || (data_valid_delay < DATAVD_MIN))
    {
/* OCS_Reserved_Start */
        INFO("wrong setup_hold:%d\n",setup_hold);
        INFO("wrong data_valid_delay:%d\n",data_valid_delay);
/* OCS_Reserved_End */
        ret = STATUS_PARAM_ERROR;
    }

    if (ret == STATUS_SUCCESS)
    {
        i2c_ptr->I2CM_CLK_CFG1.CLKLO = clk_low;
        i2c_ptr->I2CM_CLK_CFG1.CLKHI = clk_high;
        i2c_ptr->I2CM_CLK_CFG1.SETHOLD = setup_hold;
        i2c_ptr->I2CM_CLK_CFG1.DATAVD = data_valid_delay;
    }
    return ret;

}
#endif

/**
 *@brief This function configure i2c master host request function. Note that hreq must set before i2c enable.
 *@param[in] i2c_ptr: point to i2c base address
 *@param[in] enable, true for enable, false for disable hreq function
 *@param[in] hreq_config, point to the configuration of host request
 *@return    none
 */    
void i2cm_drv_hreq_cfg(I2C_TypeDef *i2c_ptr, bool enable, const i2c_master_hreq_config_t *hreq_config)
{
#if  0
    if (enable == true)
    {
        i2c_ptr->I2CM_CFG0.HOST_REQ_POLARITY = (uint32_t)hreq_config->hreq_polarity;
        i2c_ptr->I2CM_CFG0.HOST_REQ_SEL = (uint32_t)hreq_config->hreq_sel;
        
        i2c_ptr->I2CM_CFG0.HOST_REQ_EN = I2C_FUNC_ENABLE;
    }
    else
    {
        i2c_ptr->I2CM_CFG0.HOST_REQ_EN = I2C_FUNC_DISABLE;
    }
#endif
}

/*******************************************************************************
* Public I2C MASTER Control Functions
******************************************************************************/ 
/**
 *@brief Configure i2c master data match function and enable rx discard.
 *                    Received data is discarded unless the the Data Match Flag is set.
 *@param[in] i2c_ptr: point to i2c base address
 *@param[in] dmatch_config: point to the configuration of data match
 *@return    none
 */    
void i2cm_drv_dmatch_cfg(I2C_TypeDef *i2c_ptr, const i2c_master_dmatch_config_t *dmatch_config)
{
#if 0
    i2c_ptr->I2CM_DMATCH.MATCH0 = dmatch_config->data0;
    i2c_ptr->I2CM_DMATCH.MATCH1 = dmatch_config->data1;

    i2c_ptr->I2CM_CFG1.MATCH_CFG = (uint32_t)dmatch_config->dmatch_cfg;
    i2c_ptr->I2CM_INT_EN.DATA_MATCH_EN = I2C_FUNC_ENABLE;

    i2c_ptr->I2CM_CFG0.RX_DISCARD = I2C_FUNC_ENABLE;
#endif
}

/**
 *@brief Configure i2c master pinlow timeout, SCL or SDA remains low for longer than (timeout * 256) cycles.
 *@param[in] i2c_ptr: point to i2c base address
 *@param[in] timeout: the cycles of pinlow detect.
 *@return    none
 */    
void i2cm_drv_pinlow_cfg(I2C_TypeDef *i2c_ptr, uint32_t timeout)
{
#if  0
    i2c_ptr->I2CM_CFG3.PINLOW = timeout;
#endif
}

/**
 *@brief This function can enable or disable one or more master interrupt sources specified by the interrupts parameter.
 *@param[in] i2c_ptr: point to i2c base address
 *@param[in] interrupt: the interrupt to cfg：0x0 ~ 0x3fff
                Reference Bit:
                        bit0    INT_M_RX_UNDER   
                        bit1    INT_M_RX_OVER    
                        bit2    INT_M_RX_FULL    
                        bit3    INT_M_TX_OVER    
                        bit4    INT_M_TX_EMPTY   
                        bit5    INT_M_RD_REQ     
                        bit6    INT_M_TX_ABRT    
                        bit7    INT_M_RX_DONE    
                        bit8    INT_M_ACTIVITY   
                        bit9    INT_M_STOP_DET   
                        bit10   INT_M_START_DET  
                        bit11   INT_M_GEN_CALL   
                        bit12   INT_M_RESTART_DET
                        bit13   INT_M_MST_ON_HOLD
 *@param[in] enable: true for enable, false for disable this interrupt
 *@return    none
 */
void i2cm_drv_int_cfg(I2C_TypeDef *i2c_ptr, uint32_t interrupt, bool enable)       //use
{
    if (enable == true)
    {
        i2c_ptr->IC_INTR_MASK.w |= interrupt;
    }
    else
    {
        i2c_ptr->IC_INTR_MASK.w &= (~interrupt);
    }
}

/**
 *@brief This function set the I2C master watermark.
 *@param[in] i2c_ptr: point to i2c base address
 *@param[in] value: the value of watermark
 *@param[in] watermark: one of <I2C_TX_WATERMARK, I2C_RX_WATERMARK>
 *@return    none
 */
void i2cm_drv_watermark_set(I2C_TypeDef *i2c_ptr, uint8_t value, i2c_watermark_e watermark)
{
#if 0
    if (value < HW_FIFO_SIZE)
    {
        if (watermark == I2C_TX_WATERMARK)
        {
            i2c_ptr->I2CM_FIFO_CTRL.TX_WATERMARK = value;
        }
        else /* rx water mark*/
        {
            i2c_ptr->I2CM_FIFO_CTRL.RX_WATERMARK = value;
        }
    }
#endif
}

/**
 *@brief This function initializes the I2C module to a master.
 *@param[in] i2c_ptr: point to i2c base address
 *@param[in] i2c_master_config: point to the configuration of i2c master
 *@param[in] clock_freq, the current I2C function clock freq
 *@return    i2c master initialize execute result status:
 *                - STATUS_PARAM_ERROR,
 *                - STATUS_SUCCESS,
 */
status_t i2cm_drv_init(I2C_TypeDef *i2c_ptr, const i2c_master_config_t *i2c_master_config, uint32_t clock_freq)      //use
{
    status_t ret;
    /* 1. reset and disable I2C master module first */
    i2c_ptr->IC_ENABLE.w &= 0x0;

    //等待控制器完全禁用
    uint32_t timeout = 0xFFFFU;
    while (((i2c_ptr->IC_ENABLE_STATUS.IC_EN) != 0U) && (timeout-- > 0)) {
        // 等待空闲，避免死循环加超时
    }
    if (timeout == 0) {
        return STATUS_PARAM_ERROR; // 禁用失败
    }

    i2c_ptr->IC_CON.w = I2C_CTRL_INITIAL_VALUE;

    /* 2. set master baudrate */
    ret = i2cm_drv_baudrate_cfg(i2c_ptr, i2c_master_config->i2c_speed, i2c_master_config->master_mode, clock_freq);

        if (ret == STATUS_SUCCESS)
        {
            /* 3. set other configuration */
            switch (i2c_master_config->master_mode)
            {
            case I2C_STANDARD_MODE:
                i2c_ptr->IC_CON.SPEED = 1U;
                break;
            case I2C_FAST_MODE:
            case I2C_FASTPLUS_MODE:
                i2c_ptr->IC_CON.SPEED = 2U;
                break;
            case I2C_HIGHSPEED_MODE:
                i2c_ptr->IC_CON.SPEED = 3U;
                break;
            default:
                break;
            }
        i2c_ptr->IC_CON.MASTER_MODE = 1;
        i2c_ptr->IC_CON.IC_SLAVE_DISABLE = 1;
        i2c_ptr->IC_CON.IC_RESTART_EN = 1;
        i2c_ptr->IC_CON.TX_EMPTY_CTRL = 0;
        i2c_ptr->IC_TAR.IC_TAR = 0x5E; // 示例：从机地址0x1E（EEPROM常见地址）
        i2c_ptr->IC_TX_TL.TX_TL = 5;
        i2c_ptr->IC_RX_TL.RX_TL = 0;

        i2c_ptr->IC_INTR_MASK.w &= 0x0;     //屏蔽所有中断（M_TX_EMPTY这个中断没有屏蔽会一直出发中断进入中断服务程序，程序会卡死）

        i2c_ptr->IC_ENABLE.ENABLE = 1;
        timeout = 0xFFFFU;
        while (((i2c_ptr->IC_ENABLE_STATUS.IC_EN) != 1U) && (timeout-- > 0)) {}
        if (timeout == 0) {
            ret = STATUS_PARAM_ERROR;
            INFO("i2c_init error!!\n");
        }
    }
    
    return ret;
}

void i2cm_drv_test_send_data(I2C_TypeDef *i2c_ptr, uint8_t cmd, uint8_t data)
{
#if 0
    while(i2c_ptr->IC_STATUS.TFNF != 1);
    uint32_t reg_data = ((uint32_t)(cmd) << 8) | (uint32_t)(data);
    i2c_ptr->IC_DATA_CMD.w = (((uint32_t)(I2C_MASTER_CMD_WRITE) << 8U) + (uint32_t)(0x1));
    i2c_ptr->IC_DATA_CMD.w = (((uint32_t)(I2C_MASTER_CMD_WRITE) << 8U) + (uint32_t)(0x2));
    i2c_ptr->IC_DATA_CMD.w = (((uint32_t)(I2C_MASTER_CMD_WRITE) << 8U) + (uint32_t)(0x3));

    while(i2c_ptr->IC_STATUS.MST_ACTIVITY == 1);
#endif
}

void i2cm_drv_slv_addr(I2C_TypeDef *i2c_ptr, i2c_master_msg_t *i2c_master_msg)      //use
{
    // 仅当I2C使能时，才先关闭（避免重复Disable清空FIFO）
    if(i2c_ptr->IC_ENABLE.ENABLE == 1)
    {
        i2c_ptr->IC_ENABLE.ENABLE = 0;
        // 等待I2C完全关闭，避免配置异常
        while(i2c_ptr->IC_ENABLE_STATUS.IC_EN == 1);
    }
    if(i2c_master_msg->addr_10bit == true){     //10bit address
        i2c_ptr->IC_CON.IC_10BITADDR_MASTER = 1;
    } else {                                    //7bitaddress
        i2c_ptr->IC_CON.IC_10BITADDR_MASTER = 0;
    }
    i2c_ptr->IC_TAR.IC_TAR = (0x3ff & (i2c_master_msg->slave_addr));
    i2c_ptr->IC_ENABLE.ENABLE = 1;
}

/*******************************************************************************
* Public I2C SLAVE Control Functions
******************************************************************************/
/**
 *@brief This function configure i2c slave filter SCL&SDA function. Note that filter must set before i2c enable.
 *@param[in] i2c_ptr: point to i2c base address
 *@param[in] enable: true for enable, false for disable the filter function
 *@param[in] filter_sda: The filter latency is FILTSDA+3 cycles, the cycle is from i2c function clock.
 *@param[in] filter_scl: The filter latency is FILTSCL+3 cycles, the cycle is from i2c function clock.
 *@return    none
 */
void i2cs_drv_filter_cfg(I2C_TypeDef *i2c_ptr, bool enable, uint8_t filter_sda, uint8_t filter_scl)
{
#if 0
    if (enable == true)
    {
        i2c_ptr->I2CS_CFG1.FILTSDA = (((uint32_t)filter_sda)&I2C_REG_FILTER_MASK);
        i2c_ptr->I2CS_CFG1.FILTSCL = (((uint32_t)filter_scl)&I2C_REG_FILTER_MASK);
        
        i2c_ptr->I2CS_CTRL.FILT_EN = I2C_FUNC_ENABLE;
    }
    else
    {
        i2c_ptr->I2CS_CTRL.FILT_EN = I2C_FUNC_DISABLE;
    }
#endif
}

/**
 *@brief This function can enable or disable one or more slave interrupt sources 
 *                     specified by the interrupts parameter.
 *@param[in] i2c_ptr: point to i2c base address
 *@param[in] interrupt: the interrupt to cfg
 *@param[in] enable: true for enable, false for disable the interrupt
 *@return    none
 */
void i2cs_drv_int_cfg(I2C_TypeDef *i2c_ptr, uint32_t interrupt, bool enable)
{
    if (enable == true)
    {
        i2c_ptr->IC_INTR_MASK.w |= interrupt;
    }
    else
    {
        i2c_ptr->IC_INTR_MASK.w &= (~interrupt);
    }
}

/**
 *@brief This function initializes the I2C module to a slave.
 *       Reference: DW_apb_i2c Databook, chapter 3.8.1.1 "Slave Mode Operation / Initial Configuration".
 *       1) disable I2C -> 2) write slave address (IC_SAR) -> 3) write IC_CON (slave-only)
 *       -> 4) enable I2C.
 *@param[in] i2c_ptr: point to i2c base address
 *@param[in] i2c_slave_config: point to the configuration of i2c slave
 *@return    i2c slave initialize execute result status:
 *                    - STATUS_PARAM_ERROR,
 *                    - STATUS_ERROR,
 *                    - STATUS_SUCCESS,
 */
status_t i2cs_drv_init(I2C_TypeDef *i2c_ptr, const i2c_slave_config_t *i2c_slave_config)
{
    status_t ret = STATUS_ERROR;
    uint32_t timeout;

    DEV_ASSERT(i2c_ptr != NULL);
    DEV_ASSERT(i2c_slave_config != NULL);

    /* 1. reset and disable I2C module first */
    i2c_ptr->IC_ENABLE.w &= 0x0;

    /* wait until the controller is completely disabled */
    timeout = 0xFFFFU;
    while (((i2c_ptr->IC_ENABLE_STATUS.IC_EN) != 0U) && (timeout-- > 0)) {}
    if (timeout == 0)
    {
        return STATUS_PARAM_ERROR;      /* disable failed */
    }

    /* 2. configure slave address (only writable while I2C is disabled) */
    /* for 7-bit addressing only IC_SAR[6:0] is used */
    if (i2c_slave_config->addr_10bit == true)
    {
        i2c_ptr->IC_SAR.IC_SAR = (uint32_t)(i2c_slave_config->slave_addr0 & 0x3FFU);
    }
    else
    {
        i2c_ptr->IC_SAR.IC_SAR = (uint32_t)(i2c_slave_config->slave_addr0 & 0x7FU);
    }

    /* 3. configure operating mode: slave-only */
    i2c_ptr->IC_CON.w = I2C_CTRL_INITIAL_VALUE;
    i2c_ptr->IC_CON.MASTER_MODE = 0U;               /* bit0: master disabled (must be 0 with slave enabled) */
    i2c_ptr->IC_CON.IC_SLAVE_DISABLE = 0U;          /* bit6: slave enabled */
    i2c_ptr->IC_CON.IC_RESTART_EN = 1U;             /* bit5: restart supported */
    i2c_ptr->IC_CON.IC_10BITADDR_SLAVE = (i2c_slave_config->addr_10bit == true) ? 1U : 0U;  /* bit3: 7/10-bit addressing */
    i2c_ptr->IC_CON.STOP_DET_IFADDRESSED = 1U;      /* bit7: STOP_DET interrupt only when addressed */
    i2c_ptr->IC_CON.RX_FIFO_FULL_HLD_CTRL = 1U;     /* bit9: hold SCL low when RX FIFO is full (clock stretching) */

    switch (i2c_slave_config->slave_mode)
    {
    case I2C_STANDARD_MODE:
        i2c_ptr->IC_CON.SPEED = 1U;
        break;
    case I2C_FAST_MODE:
    case I2C_FASTPLUS_MODE:
        i2c_ptr->IC_CON.SPEED = 2U;
        break;
    case I2C_HIGHSPEED_MODE:
        i2c_ptr->IC_CON.SPEED = 3U;
        break;
    default:
        i2c_ptr->IC_CON.SPEED = 2U;
        break;
    }

    /* 4. mask all interrupts (enable specific ones after init) */
    i2c_ptr->IC_INTR_MASK.w &= 0x0;

    /* 5. enable I2C slave */
    i2c_ptr->IC_ENABLE.ENABLE = 1U;
    timeout = 0xFFFFU;
    while (((i2c_ptr->IC_ENABLE_STATUS.IC_EN) != 1U) && (timeout-- > 0)) {}
    if (timeout == 0)
    {
        INFO("i2c slave init error!!\n");
        return STATUS_PARAM_ERROR;      /* enable failed */
    }

    ret = STATUS_SUCCESS;
    return ret;
}

