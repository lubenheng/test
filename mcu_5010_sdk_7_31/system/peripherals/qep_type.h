#ifndef QEP_TYPE_H
#define QEP_TYPE_H

typedef struct
{
    /**
     * @brief 编码器模式选择 \n
     * @see QEP_Mode_T1 \n
     * @see QEP_Mode_T1_T2 \n
     * @see QEP_Mode_CCWSIGN_UpDown \n
     * @see QEP_Mode_CCWCW_UpDown
     */
    uint32_t QEP_Mode;      // 编码器模式选择
    uint32_t QEP_FE_CNT_EN; // 使能下降沿计算（上升沿始终计数），ENABLE使能 DISABLE失能
    uint32_t QEP_ZNC;       // Z信号清零极性选择：低电平/下降沿清零使能，ENABLE使能 DISABLE失能
    uint32_t QEP_ZPC;       // Z信号清零极性选择：高电平/上升沿清零使能 ，ENABLE使能 DISABLE失能
    uint32_t QEP_ZLC;       // Z信号电平清零QEP计数器使能，ENABLE使能 DISABLE失能
    uint32_t QEP_ZEC;       // Z信号边沿清零QEP计数器使能，ENABLE使能 DISABLE失能
    uint16_t QEP_TH;        // QEP计数门限寄存器
    /**
     * @brief 编码器对应时钟分频系数 \n
     * @see ECD_Clk_Div1  \n
     * @see ECD_Clk_Div2  \n
     * @see ECD_Clk_Div4  \n
     * @see ECD_Clk_Div8  \n
     * @see ECD_Clk_Div16 \n
     * @see ECD_Clk_Div32 \n
     * @see ECD_Clk_Div64 \n
     * @see ECD_Clk_Div128
     */
    uint32_t QEP_ClockDiv; // 时钟分频
    uint32_t QEP_Filter;   // 信号输入滤波
    /**
     * @brief 编码器中断使能 \n
     * @see QEP_IRQEna_None \n
     * @see QEP_IRQEna_UnderFlow \n
     * @see QEP_IRQEna_OverFlow
     */
    uint32_t QEP_IRQEna; // 中断使能
} TIM_QEPInitTypeDef;

/** qep interrupt types*/
typedef enum{
    QEP_UP_0 = 0U,         /*!< 0  上溢出中断标志。高电平有效，写 1 清 0。当计数器计数达到计数门限时，上计数事件触发上溢出中断。 */
    QEP_OF_1,              /*!< 1  下溢出中断标志。高电平有效，写 1 清 0。当计数器计数达到 0 时，下计数事件触发下溢出中断。     */
    QEP_Fault,             /*!< 2  错误中断。     */   
} qep_int_source_e;


#define QEP_CLK_DIV1   0x00 /**<QEP计数器频率1分频*/
#define QEP_CLK_DIV2   0x01 /**<QEP计数器频率2分频*/
#define QEP_CLK_DIV4   0x02 /**<QEP计数器频率4分频*/
#define QEP_CLK_DIV8   0x03 /**<QEP计数器频率8分频*/
#define QEP_CLK_DIV16  0x04 /**<QEP计数器频率16分频*/
#define QEP_CLK_DIV32  0x05 /**<QEP计数器频率32分频*/
#define QEP_CLK_DIV64  0x06 /**<QEP计数器频率64分频*/
#define QEP_CLK_DIV128 0x07 /**<QEP计数器频率128分频*/

/**
 * @brief 编码器中断使能配置定义
 */
#define QEP_IRQEna_None 0         /**<无使能*/
                                  /**
                                   * @brief 编码器中断使能配置定义
                                   */
#define QEP_IRQEna_UnderFlow BIT0 /**<使能下溢中断*/
                                  /**
                                   * @brief 编码器中断使能配置定义
                                   */
#define QEP_IRQEna_OverFlow BIT1  /**<使能上溢中断*/

/**
 * @brief 编码器中断标志定义
 */
#define QEP_IF_UNDERFLOW BIT0        /**<Encoder下溢中断*/
                                     /**
                                      * @brief 编码器中断标志定义
                                      */
#define QEP_IF_OVERFLOW         BIT1 /**<Encoder上溢中断*/

#define QEP_Mode_T1             ((uint32_t)0x00) /*counting on T1*/
#define QEP_Mode_T1_T2          ((uint32_t)0x01) /*counting on T1&T2*/
#define QEP_Mode_CCWSIGN_UpDown ((uint32_t)0x02) /*CCW+SIGN 符号加脉冲信号计数模式, 若需要计数信号的上下沿，则QEP_FE_CNT_EN置1*/
#define QEP_Mode_CCWCW_UpDown   ((uint32_t)0x03) /*CCW+CW   符号加脉冲信号计数模式, 若需要计数信号的上下沿，则QEP_FE_CNT_EN置1*/


/*interrupt handle definition*/
typedef void (*QEP_INT_CALLBACK)(qep_int_source_e);
#endif
