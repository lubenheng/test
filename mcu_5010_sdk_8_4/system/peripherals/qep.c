#include "platform_common.h"

#include "qep_reg.h"
#include "device_resource.h"
#include "qep_hal.h"
#include "qep.h"


/*******************************************************************************
 函数名称：    void TIM_QEPInit(TIM_QEP_TypeDef *TIM_QEPx, TIM_QEPInitTypeDef *TIM_QEPInitStruct)
 功能描述：    QEP初始化
 操作的表：    无
 输入参数：    无
 输出参数：    无
 返 回 值：    无
 其它说明：
 修改日期      版本号          修改人            修改内容
 -----------------------------------------------------------------------------
 2016/06/06      V1.0           cfwu              创建
 *******************************************************************************/

/**
 *@brief @b 函数名称:  void TIM_QEPInit(TIM_QEPTypeDef *QEPx, TIM_QEPInitTypeDef *TIM_QEPInitStruct)
 *@brief @b 功能描述:   编码器初始化函数
 *@see被引用内容：      SYS_ModuleClockCmd();
 *@param输入参数：      QEPx QEP0/QEP1 \n
                       TIM_QEPInitTypeDef *TIM_QEPInitStruct
 *@param输出参数：      无
 *@return返 回 值：     无
 *@note其它说明：       无
 *@warning              无
 *@par 示例代码：
 *@code
                    TIM_QEPInitTypeDef TIM_QEPInitStruct;
                    TIM_QEPStructInit(&TIM_QEPInitStruct);                   // Timer结构体初始化
                    TIM_QEPInitStruct.QEP_FE_CNT_EN = DISABLE;               // 使能下降沿进行计数，ENABLE:使能 DISABLE:失能
                    TIM_QEPInitStruct.QEP_Mode = QEP_Mode_T1;                // 编码器模式选择
                    TIM_QEPInitStruct.QEP_ClockDiv = QEP_CLK_DIV1;           // 编码器时钟分频
                    TIM_QEPInitStruct.QEP_Filter = 0;                        // 输入信号滤波系数
                    TIM_QEPInitStruct.QEP_TH = 65535;                        // 编码器计数周期
                    TIM_QEPInitStruct.QEP_ZEC = ENABLE;                      // Z 信号清零极性选择：低电平/下降沿清零使能
                    TIM_QEPInitStruct.QEP_ZLC = DISABLE;                     // Z 信号清零极性选择：高电平/下降沿清零使能
                    TIM_QEPInitStruct.QEP_ZNC = DISABLE;                     // Z 信号电平清零 QEP 计数器使能
                    TIM_QEPInitStruct.QEP_ZPC = ENABLE;                      // Z 信号边沿清零 QEP 计数器使能
                    TIM_QEPInitStruct.QEP_IRQEna = DISABLE;                  // 中断使能
                    TIM_QEPInit(QEP0, &TIM_QEPInitStruct);
                    TIM_QEPCmd(QEP0, ENABLE);                                // QEP0模块使能
  @endcode
 *@par 修改日志:
 * <table>
 * <tr><th>Date	        <th>Version    <th>Author      <th>Description
  * <tr><td>2023年02月23日 <td>1.0     <td>HuangMG        <td>修改
 * </table>
 */
void TIM_QEPInit(TIM_QEPTypeDef *QEPx, TIM_QEPInitTypeDef *TIM_QEPInitStruct)
{
    //uint32_t tempTimerCfg;

    /* QEP CFG */
    QEPx->CFG.ZEC  = (TIM_QEPInitStruct->QEP_ZEC & 0x1);
    QEPx->CFG.ZLC  = (TIM_QEPInitStruct->QEP_ZLC & 0x1);
    QEPx->CFG.ZPC  = (TIM_QEPInitStruct->QEP_ZPC & 0x1);
    QEPx->CFG.ZNC  = (TIM_QEPInitStruct->QEP_ZNC & 0x1);
    QEPx->CFG.MODE = (TIM_QEPInitStruct->QEP_Mode & 0x3);
    QEPx->CFG.FE_CNT_EN = (TIM_QEPInitStruct->QEP_FE_CNT_EN & 0x1); //CCW+SIGN/CCW+CW 两种模式下，是否在下降沿进行计数（上升沿总是计数）
    QEPx->CFG.CLK_DIV   = TIM_QEPInitStruct->QEP_ClockDiv;
    QEPx->CFG.SAP       = TIM_QEPInitStruct->QEP_Filter; 
    /* QEP TH */
    QEPx->TH.TH  = TIM_QEPInitStruct->QEP_TH;
    /* QEP IE */
    QEPx->IE.UF_IE = (TIM_QEPInitStruct->QEP_IRQEna & 0x1)>>0;  //BIT0
    QEPx->IE.OF_IE = (TIM_QEPInitStruct->QEP_IRQEna & 0x2)>>1;  //BIT1
}

/**
 *@brief @b 函数名称:   void TIM_QEPStructInit(TIM_QEPInitTypeDef *TIM_QEPInitStruct)
 *@brief @b 功能描述:   编码器结构体初始化
 *@see被引用内容：      无
 *@param输入参数：      TIM_QEPInitTypeDef *TIM_QEPInitStruct
 *@param输出参数：      无
 *@return返 回 值：     无
 *@note其它说明：       无
 *@warning              无
 *@par 示例代码：
 *@code
                    TIM_QEPInitTypeDef TIM_QEPInitStruct;
                    TIM_QEPStructInit(&TIM_QEPInitStruct);//编码器结构体初始化
  @endcode
 *@par 修改日志:
 * <table>
 * <tr><th>Date	        <th>Version    <th>Author      <th>Description
 * <tr><td>2023年02月23日 <td>1.0      <td>HuangMG        <td>修改
 * </table>
 */
void TIM_QEPStructInit(TIM_QEPInitTypeDef *TIM_QEPInitStruct)
{
    TIM_QEPInitStruct->QEP_Mode      = QEP_Mode_T1;
    TIM_QEPInitStruct->QEP_ClockDiv  = 0;
    TIM_QEPInitStruct->QEP_FE_CNT_EN = 0;
    TIM_QEPInitStruct->QEP_TH        = 0;
    TIM_QEPInitStruct->QEP_ZEC       = 0;
    TIM_QEPInitStruct->QEP_ZLC       = 0;
    TIM_QEPInitStruct->QEP_ZNC       = 0;
    TIM_QEPInitStruct->QEP_ZPC       = 0;
    TIM_QEPInitStruct->QEP_Filter    = 0;
    TIM_QEPInitStruct->QEP_IRQEna    = 0;
}

/**
 *@brief @b 函数名称:   void TIM_QEPCmd(TIM_QEPTypeDef *QEPx, FuncState state)
 *@brief @b 功能描述:   编码器QEPx使能和停止
 *@see被引用内容：      无
 *@param输入参数：      QEPx：QEP0/QEP1 \n
                       FuncState state：使能与失能
 *@param输出参数：      无
 *@return返 回 值：     无
 *@note其它说明：       无
 *@warning             无
 *@par 示例代码：
 *@code
                TIM_QEPCmd(ECD0, ENABLE); // ECD0模块使能
  @endcode
 *@par 修改日志:
 * <table>
 * <tr><th>Date	        <th>Version    <th>Author      <th>Description
 * <tr><td>2023年02月23日 <td>1.0      <td>HuangMG        <td>修改
 * </table>
 */
void TIM_QEPCmd(TIM_QEPTypeDef *QEPx, FuncState state)
{
    if (state != DISABLE)
    {
        QEPx->CFG.w |= BIT15;
    }
    else
    {
        QEPx->CFG.w &= ~BIT15;
    }
}

/**
 *@brief @b 函数名称:   uint16_t TIM_QEP_GetCount(TIM_QEPTypeDef *QEPx)
 *@brief @b 功能描述:   编码器计数值
 *@see被引用内容：       无
 *@param输入参数：       QEPx：QEP0/QEP1 \n
 *@param输出参数：       无
 *@return返 回 值：      无
 *@note其它说明：        无
 *@warning              无
 *@par 示例代码：
 *@code
                      uint32_t QEP0_Value = 0;
                QEP0_Value = TIM_QEP_GetCount(QEP0); //获取编码器0计数值
  @endcode
 *@par 修改日志:
 * <table>
 * <tr><th>Date	        <th>Version    <th>Author      <th>Description
 * <tr><td>2023年02月23日 <td>1.0      <td>HuangMG        <td>修改
 * </table>
 */
uint16_t TIM_QEP_GetCount(TIM_QEPTypeDef *QEPx)
{
    return QEPx->CNT.CNT;
}

/**
 *@brief @b 函数名称:   void QEP_ClearIRQFlag(TIM_QEPTypeDef *QEPx,uint32_t tempFlag)
 *@brief @b 功能描述:   清除QEP中断标志
 *@see被引用内容：       无
 *@param输入参数：       QEPx：QEP0/QEP1 \n
                        tempFlag：
 * <table>              <tr><th>QEP_IF_UNDERFLOW   <td>Encoder下溢中断
                        <tr><th>QEP_IF_OVERFLOW    <td>Encoder上溢中断
 * </table>
 *@param输出参数：       无
 *@return返 回 值：      无
 *@note其它说明：        无
 *@warning              无
 *@par 示例代码：
 *@code
                if(QEP_GetIRQFlag(QEP0,QEP_IF_OVERFLOW)) //获取编码器上溢中断标志
                {
                    QEP_ClearIRQFlag(QEP0,QEP_IF_OVERFLOW);//清除编码器上溢中断标志
                }
  @endcode
 *@par 修改日志:
 * <table>
 * <tr><th>Date	        <th>Version    <th>Author      <th>Description
 * <tr><td>2023年02月23日 <td>1.0      <td>HuangMG        <td>修改
 * </table>
 */
void QEP_ClearIRQFlag(TIM_QEPTypeDef *QEPx, uint32_t tempFlag)
{
    QEPx->IF.w = tempFlag;
}

/**
 *@brief @b 函数名称:   u16 QEP_GetIRQFlag(TIM_QEPTypeDef *QEPx, u32 QEP_IF_x)
 *@brief @b 功能描述:   获取QEP中断标志
 *@see被引用内容：       无
 *@param输入参数：       QEPx：QEP0/QEP1 \n
                        QEP_IF_x：
 * <table>              <tr><th>QEP_IF_UNDERFLOW   <td>Encoder下溢中断
                        <tr><th>QEP_IF_OVERFLOW    <td>Encoder上溢中断
 * </table>
 *@param输出参数：       无
 *@return返 回 值：      无
 *@note其它说明：        无
 *@warning              只有对应中断使能后，改为才能读取，如果对应中断未使能，读取结果一直为0
 *@par 示例代码：
 *@code
                      if(QEP_GetIRQFlag(QEP0,QEP_IF_OVERFLOW)) //获取上溢中断标志
                {
                }
  @endcode
 *@par 修改日志:
 * <table>
 * <tr><th>Date	        <th>Version    <th>Author      <th>Description
 * <tr><td>2023年02月23日 <td>1.0      <td>HuangMG        <td>修改
 * </table>
 */
u16 QEP_GetIRQFlag(TIM_QEPTypeDef *QEPx, u32 QEP_IF_x)
{
    if (QEPx->IF.w & QEPx->IE.w & QEP_IF_x)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/**
 *@brief @b 函数名称:   u16 QEP_GetFlag(TIM_QEPTypeDef *QEPx, u32 QEP_IF_x)
 *@brief @b 功能描述:   获取QEP中断标志
 *@see被引用内容：       无
 *@param输入参数：       QEPx：QEP0/QEP1 \n
                        QEP_IF_x：
 * <table>              <tr><th>QEP_IF_UNDERFLOW   <td>Encoder下溢中断
                        <tr><th>QEP_IF_OVERFLOW    <td>Encoder上溢中断
 * </table>
 *@param输出参数：       无
 *@return返 回 值：      无
 *@note其它说明：        无
 *@warning              不需要对应中断使能，也可以正常读取中断标志位
 *@par 示例代码：
 *@code
                      if(QEP_GetFlag(QEP0,QEP_IF_OVERFLOW)) //获取上溢中断标志
                {
                }
  @endcode
 *@par 修改日志:
 * <table>
 * <tr><th>Date	        <th>Version    <th>Author      <th>Description
 * <tr><td>2023年02月23日 <td>1.0      <td>HuangMG        <td>修改
 * </table>
 */
u16 QEP_GetFlag(TIM_QEPTypeDef *QEPx, u32 QEP_IF_x)
{
    if (QEPx->IF.w & QEP_IF_x)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
