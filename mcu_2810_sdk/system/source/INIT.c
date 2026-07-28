#include <stdio.h>
#include <string.h>
#include "CMSDK_CM0.h"
#include "core_cm0.h"
#include "Common.h"
#include "wdt.h"
#include "INIT.h"
#include "uart.h"
#include "user_control.h"
#include  "mc_foc.h"
#include "mcu_driver.h"
#include "motor_driver.h"
#include "user.h"
#include "gpio_hal.h"
#include "stdint.h"
#include "main.h"
UNION_U32 TEMP_Ro;
uint16_t Ro_angl_before, RoPLL_angl_before, Ro_temp, Ro_angl_Foc;
uint16_t Ro_pll_comp;
int16_t g_smo_speed, g_smo_speed_short, g_smo_speed_long;
uint16_t Ro_angl_after, RoPLL_angl_after, Ro_Ix_raw, Ro_Iy_raw;
int16_t Ro_Id_fbk, Ro_Iq_fbk, Ro_Ealf, Ro_Ebt, Ro_Ialf, Ro_Iw, Ro_Ibt, Ro_Iu, Ro_Iv, Ro_ud, Ro_uq, Ro_ud1, Ro_uq1, Ro_Ualf, Ro_Ubt, Ro_Offset_U, Ro_Offset_V, Ro_Offset_W;
int16_t Ro_Ialf_est, Ro_Ibt_est;
int16_t Ro_fe3_Fe_short, Ro_fe3_Fe;
int16_t Cur_PI_Ud, Cur_PI_Uq;
int16_t Id_req, Iq_req;
int16_t Ro_Iu_raw, Ro_Iv_raw;
int16_t Ealf_filted, Ebt_filted, Ealf_real_time, Ebt_real_time;
uint16_t Ro_Udc_filt_short, Ro_Udc_filt_long;
uint16_t ro_cntU, ro_cntV, ro_cntW, svpwm_sector;
int16_t Iu_d = 0;
int16_t Iv_d = 0;
int16_t Ibus_d = 0;
int16_t Udc_d = 0;
int16_t Iu_ADC = 0;
int16_t Iv_ADC = 0;
int16_t Iw_ADC = 0;
uint16_t flag1 = 0;
uint16_t flag2 = 0;
extern int16_t a,b,c,d,e;
int counter1 = 0;
#define N 1024               // 8个周期 × 128点/周期
uint16_t adc6_buf[N];
volatile uint16_t adc_idx = 0;
volatile uint8_t adc_done = 0;







#if (UART_CONFIG_MODE == RTT_ON)
    extern RTTStruct rttStruct; // RTT通道数据，在一个通道上传3个数据
#endif						// #if(UART_CONFIG_MODE == RTT_ON)

//-------------------------------------------------------------------/
/*****************************************************************************
* Function Name  : HardFault_Handler
* Description    :
* Function Call  :
* Input Paragram :
* Return Value   : none
* note           :
* Version        :
******************************************************************************/
void HardFault_Handler(void)
{
    rldWDT(WDT_SHORT_WAIT);
    bootWDTOpen(true);
    printk("HardFault_Handler\n");

    while (1)
    {
        // 等重启
    }
}

/**
 * @brief 外部中断0 服务函数
 */
void int0_Handler(void)
{
    /* TODO: 处理外部中断0事件 */
	a++;
}

/**
 * @brief 外部中断1 服务函数
 */
void int1_Handler(void)
{
    /* TODO: 处理外部中断1事件 */
	b++;
}

/**
 * @brief 外部中断2 服务函数
 */
void int2_Handler(void)
{
    /* TODO: 处理外部中断2事件 */
	c++;
}

/**
 * @brief 外部中断3 服务函数
 */
void int3_Handler(void)
{
    /* TODO: 处理外部中断3事件 */
	d++;
}

/**
 * @brief 外部中断4 服务函数
 */
void int4_Handler(void)
{
    /* TODO: 处理外部中断4事件 */
	e++;
}

/**
 * @brief 引脚唤醒 / 电平变化中断
 */
void pin_Handler(void)
{
    /* TODO: 处理引脚唤醒或键盘中断 */
	
}

/**
 * @brief 实时时钟 (RTC) 中断
 */
void rtc_Handler(void)
{
    /* TODO: 处理RTC闹钟或秒中断 */
}

///**
// * @brief 串口0 中断
// */
//void uart0_Handler(void)
//{
//    /* TODO: 处理UART0收发中断 */
//}

///**
// * @brief 串口1 中断
// */
//void uart1_Handler(void)
//{
//    /* TODO: 处理UART1收发中断 */
//}

/**
 * @brief I2C0 中断
 */
void i2c0_Handler(void)
{
    /* TODO: 处理I2C0事件中断 */
}

///**
// * @brief 定时器0 中断
// */
//void tim0_Handler(void)
//{
//    /* TODO: 处理定时器0中断 */
//}

///**
// * @brief 定时器1 中断
// */
//void tim1_Handler(void)
//{
//    /* TODO: 处理定时器1中断 */
//}

///**
// * @brief 定时器2 中断
// */
//void tim2_Handler(void)
//{
//    /* TODO: 处理定时器2中断 */
//}

/**
 * @brief 定时器3 中断
 */
u8 SpdLoopCnt;
int16_t speed_Ref = 6000;
s32 SpedZoomFilt, FiltCoff = 800;
void tim3_Handler(void) //1ms
{
    GPIO->out &= ~BIT7;
    //uint32_t tmp;
    if(TIM3->cr1 & 0x20)
    {
        TIM3->cr1 &= ~BIT5;
    }
		
    #if (Demo_type==Motor_if)

	
		#elif(Demo_type==Motor_vf)

		#else
		
    #endif
		
//    //--------------------------------------------------------------------------/
//    // 电机故障检测
//    //MC_MotorFault_Check();

//    //--------------------------------------------------------------------------/
//    // 电机初始状态任务
//    //MC_MotorInit_Process();

//    //--------------------------------------------------------------------------/
//    // 电机转速计算
    MC_MotorSpeed_Calc();

//    //--------------------------------------------------------------------------/
//    // 电机调速模块控制
//    SpedZoomFilt = (s32)(FiltCoff * (s32)g_smo_speed_long  + (s32)(32767 - FiltCoff) * SpedZoomFilt) >> 15;

////    if (MOTOR_STATE == MC_RUN)
////    {

//        if(++SpdLoopCnt > TIME_SPEED_LOOP)
//        {
//            SpdLoopCnt = 0;


//            Function_PI_Cal(&stru_PI_SpdLoop, speed_Ref , SpedZoomFilt);
//        }
////    }
}


///**
// * @brief 定时器4 中断
// */
//void tim4_Handler(void)
//{
//    /* TODO: 处理定时器4中断 */
//}

/**
 * @brief 电机FG信号 (速度反馈) 中断
 */
void fgif_Handler(void)
{
    /* TODO: 处理FG测速中断 */
}

/**
 * @brief ADC转换完成中断
 */
void adc_Handler(void)
{
	flag2++;
    // 清除中断
    ADCCTRL->cr &= ~BIT0;
    // 取消ADC忙标志位
    #if (Demo_type==Motor_if)

	
		#elif(Demo_type==Motor_vf)

		#else
		
    #endif
	
//    // 收集数据
//    Iu_d = (ADCCTRL->adc0) & 0x7fff;
//    Iv_d = (ADCCTRL->adc1) & 0x7fff;
//    Ibus_d = (ADCCTRL->adc4) & 0x7fff;
//    Udc_d = (ADCCTRL->adc2) & 0x7fff;
// 收集数据
    Iu_d = (ADCCTRL->adc6) & 0x7fff;
    
	if (adc_idx < N) 
		{
        adc6_buf[adc_idx++] = Iu_d;
			
    } 
	else 
		{
        adc_done = 1;
    }

}

/**
 * @brief 比较器0 中断
 */


void cmp0if_Handler(void)
{
    /* TODO: 处理比较器0事件 */
    if(CMP->CMP_SR_b.CMP0IF & 1)
    {

        CMP->CMP_SR_b.CMP0IF = 0;
    }

    counter1++;

//    if(counter1 > 100)
//    {
//        counter1 = 0;
//        NVIC_DisableIRQ(cmp0_irq);
//    }
}

/**
 * @brief 比较器1 中断
 */
void cmp1if_Handler(void)
{
    /* TODO: 处理比较器1事件 */
}

/**
 * @brief 比较器2 中断
 */
void cmp2if_Handler(void)
{
    /* TODO: 处理比较器2事件 */
}

/**
 * @brief 比较器3 中断
 */
void cmp3if_Handler(void)
{
    /* TODO: 处理比较器3事件 */
}

/**
 * @brief 比较器4 中断
 */
void cmp4if_Handler(void)
{
    /* TODO: 处理比较器4事件 */
}

/**
 * @brief 定时器5 中断
 */
void tim5_Handler(void)
{
    /* TODO: 处理定时器5中断 */
}

/**
 * @brief SPI 中断
 */
void spi_Handler(void)
{
    /* TODO: 处理SPI收发中断 */
}

/**
 * @brief 过温保护 (TSD) 中断
 */
void tsd_Handler(void)
{
    /* TODO: 处理过温关机中断 */
}

/**
 * @brief 欠压锁定 (UVLO) 中断
 */
void uvlo_Handler(void)
{
    /* TODO: 处理欠压锁定中断 */
}

/**
 * @brief 综合故障中断 (硬件故障信号)
 */
void fault_Handler(void)
{
    /* TODO: 处理故障中断，执行保护停机 */
}

/**
 * @brief I2C1 中断
 */
void i2c1_Handler(void)
{
    /* TODO: 处理I2C1事件中断 */
}

/**
 * @brief FOC硬件加速器中断 (如SVPWM/角度更新完成)
 */
void foc_Handler(void)
{
    /* TODO: 处理FOC控制中断，更新PWM占空比 */
}

/**
 * @brief 调试接口 (SWD) 中断
 */
void swd_Handler(void)
{
    /* TODO: 处理调试事件中断 */
}

/**
 * @brief 定时器6 中断
 */
void tim6_Handler(void)
{
    /* TODO: 处理定时器6中断 */
}


extern uint16_t smo_ek_SUM, smo_ek_Cnt, smo_ek_temp;
extern int32_t test11, test22, test33, test44;
extern uint16_t test55;
uint16_t probe_switch = 1;
extern struct_smo_est     stru_smo_est;
uint16_t adc_usr_busy;
/**
 * @brief driver 事件中断
 */
void dcif_Handler(void)
{
    if (DRV->sr & 0x10)
    {

        DRV->sr &= ~0x10;
    }
	flag2++;
	 ADCCTRL->cr |= BIT6;
	
//    #if (Demo_type==Motor_if)
//        #if (IF_CONFIG == IF_ENABLE)
//        FOC_Control_Process();
//        #endif
//		#endif
//		#if(Demo_type==Motor_smo)
//        #if (IF_CONFIG == IF_ENABLE)
//        FOC_Control_Process();
//        #endif
//    #endif
//    #if 1
//    //-----------------------------------------------------------------------/
    // 用户进程
    // FOC_User_Control();
    //---------------------------------- -------------------------------------/
    // 滤波前计算速度
    //g_smo_speed = (FOC->ro_fe3 >> 16) * MOTOR_SPEED_MAX >> 15;


//    // 短滤波计算速度
//    g_smo_speed_short = (FOC->ro_fe2 & 0xffff) * MOTOR_SPEED_MAX >> 15;

//    // 长滤波计算速度
//    g_smo_speed_long = (FOC->ro_fe2 >> 16) * MOTOR_SPEED_MAX >> 15;

//    Ro_pll_comp = (FOC->angl_comp & 0xFFFF);

//    TEMP_Ro.U32 = FOC->ro_angl_auto;
//    Ro_angl_before = TEMP_Ro.Words.Low;
//    Ro_angl_Foc = TEMP_Ro.Words.Low;
//    RoPLL_angl_before = TEMP_Ro.Words.High;

//    // 将增加补偿角后的位置赋值给全局变量
//    TEMP_Ro.U32 = FOC->ro_angl_after;
//    Ro_angl_after = TEMP_Ro.Words.Low;	   // VF或者IF下补偿后角度
//    RoPLL_angl_after = TEMP_Ro.Words.High; // SMO下补偿后角度

//    TEMP_Ro.U32 = FOC->ro_Id;
//    // 将dq轴反馈电流赋值给全局变量
//    Ro_Id_fbk = TEMP_Ro.Words.Low;
//    Ro_Iq_fbk = TEMP_Ro.Words.High;

//    // 将dq轴电流请求值赋值给全局变量
//    TEMP_Ro.U32 = FOC->Id_req;
//    Id_req = TEMP_Ro.Words.Low;
//    Iq_req = TEMP_Ro.Words.High;

//    // alpha/beta轴电流
//    TEMP_Ro.U32 = FOC->ro_Ialf;
//    Ro_Ialf = TEMP_Ro.Words.Low;
//    Ro_Ibt = TEMP_Ro.Words.High;

//    // 将估计电流赋值给全局变量
//    TEMP_Ro.U32 = FOC->Ro_I_est;
//    Ro_Ialf_est = TEMP_Ro.Words.Low;
//    Ro_Ibt_est = TEMP_Ro.Words.High;

//    // 将beta轴滤波前 与 滤波后 的反电势赋值
//    TEMP_Ro.U32 = FOC->Ro_emf4;
//    Ebt_real_time = TEMP_Ro.Words.Low;
//    Ebt_filted = TEMP_Ro.Words.High;

//    // 将alpha轴滤波前 与 滤波后 的反电势赋值
//    TEMP_Ro.U32 = FOC->Ro_emf3;
//    Ealf_real_time = TEMP_Ro.Words.Low;
//    Ealf_filted = TEMP_Ro.Words.High;

//    // 将滑膜得到的omega赋值给全局变量
//    TEMP_Ro.U32 = FOC->ro_fe3;
//    Ro_fe3_Fe = TEMP_Ro.Words.High;//((FOC->ro_fe3 >> 16) & 0xFFFF);
//    Ro_fe3_Fe_short = (FOC->ro_fe2 & 0xffff);

//    TEMP_Ro.U32 = FOC->ro_Ixy_raw;
//    Ro_Ix_raw = TEMP_Ro.Words.Low;
//    Ro_Iy_raw = TEMP_Ro.Words.High;

//    ro_cntU = (FOC->ro_cntU & 0xffff);
//    ro_cntV = (FOC->ro_cntV & 0xffff);
//    ro_cntW = (FOC->ro_cntW & 0xffff);
//    //
//    TEMP_Ro.U32 = FOC->ro_Iuv;
//    Ro_Iu = TEMP_Ro.Words.Low;
//    Ro_Iv = TEMP_Ro.Words.High;

//    TEMP_Ro.U32 = FOC->ro_Ivw;
//    Ro_Iw = TEMP_Ro.Words.High;

//    TEMP_Ro.U32 = FOC->ro_angl;
//    svpwm_sector = TEMP_Ro.Words.High;

//    Ro_Iu_raw = TEMP_Ro.Words.Low;
//    Ro_Iv_raw = TEMP_Ro.Words.High;

//    TEMP_Ro.U32 = FOC->ro_Udc_filt;
//    Ro_Udc_filt_short = TEMP_Ro.Words.Low;
//    Ro_Udc_filt_long = TEMP_Ro.Words.High;

//    TEMP_Ro.U32 = FOC->ro_Ud;
//    Ro_ud1 = TEMP_Ro.Words.Low;
//    Ro_uq1 = TEMP_Ro.Words.High;

//    TEMP_Ro.U32 = FOC->ro_Ualf;
//    Ro_Ualf = TEMP_Ro.Words.Low;
//    Ro_Ubt = TEMP_Ro.Words.High;

//    #endif

//    switch (probe_switch)
//    {
//        case 0:
//            UartView(Ro_uq1, ro_cntU, Ro_Udc_filt_short, Ro_ud1);
//            break;

//        case 1:
//            UartView(Id_req, Ro_Id_fbk, Iq_req, Ro_Iq_fbk);
//            break;

//        case 2:
//            UartView(Ro_ud1, Ro_Udc_filt_short, Ro_uq1, Ro_fe3_Fe);
//            break;

//        case 3:
//            UartView(Ro_Iu, Ealf_real_time, Ealf_filted, RoPLL_angl_after);
//            break;

//        case 4:
//            UartView(stru_Para.Run.theta_compensation_final, MOTOR_STATE * 100, RoPLL_angl_after, Ro_angl_Foc);
//            break;

//        case 5:
//            UartView(g_smo_speed_short, g_smo_speed_long, RoPLL_angl_after >> 4, Ro_angl_Foc >> 4);
//            break;

//        case 6:
//            UartView(Ro_Ialf, Ro_Ibt, MOTOR_STATE * 100, Ro_fe3_Fe);
//            break;

//        case 7:
//            UartView(Ro_Ibt >> 1, Ro_Ibt_est >> 1, Ebt_real_time >> 1, Ebt_filted >> 1);
//            break;

//        case 8:
//            UartView(Ro_Ix_raw, svpwm_sector, Ro_Iv, Ro_Iy_raw);
//            break;

//        case 9:
//            UartView(Ro_Iu_raw, Ro_Iu, Ro_Iv, Ro_Iw);
//            break;
//    }
}









