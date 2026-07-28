#include <stdio.h>
#include <string.h>
#include "CMSDK_CM0.h"
#include "system_CMSDK_CM0.h"
#include "core_cm0.h"
#include "Common.h"
#include "wdt.h"
#include "INIT.h"
#include "uart.h"
#include "user_control.h"

#include "mcu_driver.h"
#include "motor_driver.h"
#include "user.h"

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
int16_t Iw_d = 0;
int16_t Udc_d = 0;
int16_t Iu_ADC = 0;
int16_t Iv_ADC = 0;
int16_t Iw_ADC = 0;
#if (UART_CONFIG_MODE == RTT_ON)
    extern RTTStruct rttStruct; // RTT通道数据，在一个通道上传3个数据
#endif						// #if(UART_CONFIG_MODE == RTT_ON)

//-------------------------------------------------------------------/
// number -13 interrupt
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

//-------------------------------------------------------------------/
// number 0 interrupt
void int0_Handler(void)
{
    if(GPIO->int_clr & BIT5)
    {
        GPIO->out |= BIT26;
        GPIO->int_clr &= ~BIT5;
        GPIO->out &= ~BIT26;
    }

}
//-------------------------------------------------------------------/
// number 1 interrupt
void int1_Handler(void)
{
    if(GPIO->int_clr & BIT13)
    {
        GPIO->out |= BIT26;
        GPIO->int_clr &= ~BIT13;
        GPIO->out &= ~BIT26;
    }
}
//-------------------------------------------------------------------/
// number 2 interrupt
void int2_Handler(void)
{
    if(GPIO->int_clr & BIT21)
    {
        GPIO->out |= BIT26;
        GPIO->int_clr &= ~BIT21;
        GPIO->out &= ~BIT26;
    }
}
//-------------------------------------------------------------------/
// number 3 interrupt
void int3_Handler(void)
{
    if(GPIO->int_clr & BIT29)
    {
        GPIO->out |= BIT26;
        GPIO->int_clr &= ~BIT29;
        GPIO->out &= ~BIT26;
    }
}
//-------------------------------------------------------------------/
// number 4 interrupt
void int4_Handler(void)
{
    if(GPIO->p4_int_clr & BIT5)
    {
        GPIO->out |= BIT26;
        GPIO->p4_int_clr &= ~BIT5;
        GPIO->out &= ~BIT26;
    }

}
//-------------------------------------------------------------------/
// number 5 interrupt
void pin_Handler(void)
{
}

//-------------------------------------------------------------------/
// number 6 interrupt
void rtc_Handler(void)
{
    GPIO->out |= BIT26;
    RTC->wo &= ~BIT1;
    GPIO->out &= ~BIT26;

    if(RTC->wo & BIT1)
    {
        GPIO->out |= BIT26;
        RTC->wo &= ~BIT1;
        GPIO->out &= ~BIT26;
    }

}
//-------------------------------------------------------------------/
// number 7 interrupt
// void uart0_Handler(void)
// {
// }

//-------------------------------------------------------------------/
// number 8 interrupt
// void uart1_Handler(void)
// {
// }

//-------------------------------------------------------------------/
// number 9 interrupt
void i2c0_Handler(void)
{
}

//-------------------------------------------------------------------/
// number 10 interrupt
extern volatile uint8_t timer0_done;
extern volatile uint8_t printOn;
void tim0_Handler(void)
{
    TIM0->int_clr = 1;
    timer0_done = 1;
    printOn = 0;
}

//-------------------------------------------------------------------/
// number 11 interrupt
uint8_t hall = 0;
uint8_t hall_r = 0;
uint16_t angl = 0;
void tim1_Handler(void)
{
    //uint32_t tmp;
    //uint8_t idx;

    TIM1->sr &= ~BIT1;
    //    tmp = (((GPIO->in) >> 24) >> 4) & 7;

    //    hall_r = hall;
    //    hall = tmp;
    //    idx = hall - 1;

    //    // 更新角度
    //    tmp = (DRV->cr >> 6) & 1;
    //    // tmp = (FOC->ctrl0 >> 24) & 1;

    //    if (tmp == 0)
    //    {
    //        angl = angl_side1[idx];
    //    }
    //    else
    //    {
    //        angl = angl_side2[idx];
    //    }
    //    FOC->angl = angl;
}

//-------------------------------------------------------------------/
// number 12 interrupt

uint16_t TIM2_DR_buff, TIM2_ARR_buff;
void tim2_Handler(void)
{
    if(TIM2->TIM2_CR1_b.T2IP == 1)
    {

        TIM2_DR_buff = TIM2->TIM2_DR;
        TIM2_ARR_buff = TIM2->TIM2_ARR;
//			SEGGER_RTT_Write(1, &rttStruct, sizeof(rttStruct));
        TIM2->TIM2_CR1_b.T2IP = 0;
    }

    if(TIM2->TIM2_CR1_b.T2IF == 1)
    {

//			 rttStruct.data1 = TIM2->TIM2_DR;
//			 rttStruct.data2 = TIM2->TIM2_ARR;
        TIM2_DR_buff = TIM2->TIM2_DR;
        TIM2_ARR_buff = TIM2->TIM2_ARR;
        TIM2->TIM2_DR = 0;
        TIM2->TIM2_CNTR = 0;
        TIM2->TIM2_ARR = 0;
        TIM2->TIM2_CR1_b.T2IF = 0;

    }

    #if(UART_CONFIG_MODE == RTT_ON)
    rttStruct.data1 = TIM2->TIM2_DR;
    rttStruct.data2 = TIM2->TIM2_ARR;

    SEGGER_RTT_Write(1, &rttStruct, sizeof(rttStruct));
    #endif //#if(UART_CONFIG_MODE == RTT_ON)

}

//-------------------------------------------------------------------/
// number 13 interrupt
u8 SpdLoopCnt;
int16_t speed_Ref = 6000;
s32 SpedZoomFilt, FiltCoff = 800;
void tim3_Handler(void) //1ms
{
    //uint32_t tmp;
    if(TIM3->cr1 & 0x20)
    {
        //GPIO->out &= ~BIT26;
        TIM3->cr1 &= ~BIT5;
    }

    //异或翻转IO口
    //GPIO->out ^= BIT26;
    //GPIO->out ^= BIT8;

    TimeFlag_1ms = 1;

    //--------------------------------------------------------------------------/
    // 电机故障检测
    //MC_MotorFault_Check();

    //--------------------------------------------------------------------------/
    // 电机初始状态任务
    //MC_MotorInit_Process();

    //--------------------------------------------------------------------------/
    // 电机转速计算
    MC_MotorSpeed_Calc();

    //--------------------------------------------------------------------------/
    // 电机调速模块控制
    SpedZoomFilt = (s32)(FiltCoff * (s32)g_smo_speed_long  + (s32)(32767 - FiltCoff) * SpedZoomFilt) >> 15;

//    if (MOTOR_STATE == MC_RUN)
//    {

        if(++SpdLoopCnt > TIME_SPEED_LOOP)
        {
            SpdLoopCnt = 0;


            Function_PI_Cal(&stru_PI_SpdLoop, speed_Ref , SpedZoomFilt);
        }
//    }
}

//-------------------------------------------------------------------/
// number 14 interrupt
void tim4_Handler(void)
{
    //uint32_t tmp;
    GPIO->out |= BIT26;

//	if(TIM3->cr1&&0x20==1)
//	{
    //	TIM3->cr1&=~0x20;
//	}
    if(TIM4->cr1 & 0x20)
    {
        //TIM3->cr1|=0xa0;
        TIM4->cr1 &= ~BIT5;
    }

    GPIO->out &= ~BIT26;

}

//-------------------------------------------------------------------/
// number 15 interrupt
void fgif_Handler(void)
{
}

//-------------------------------------------------------------------/
// number 16 interrupt
volatile int16_t visual_scope1 = 0;
volatile int16_t visual_scope2 = 0;
volatile int16_t visual_scope3 = 0;
volatile int16_t visual_scope4 = 0;
uint16_t probe_switch = 2;
extern uint16_t smo_ek_SUM, smo_ek_Cnt, smo_ek_temp;

extern int32_t test11, test22, test33, test44;
extern uint16_t test55;

extern struct_smo_est     stru_smo_est;

void dcif_Handler(void)
{
    uint32_t tmp;

    if (DRV->sr & 0x10)
    {
        // Driver->DriverDRV_SR_b.DCIF=0;
        DRV->sr &= ~0x10;
    }

//    tmp = GPIO->out;
//    tmp = (tmp >> 8) & 1;
//    if (tmp == 0)
//     	GPIO->out |= BIT8;
//     else
//     	GPIO->out &= ~BIT8;
    //GPIO->out |= BIT8;//P10 = 1

//    GPIO->out |= BIT26;
//    GPIO->out &= ~BIT26;


//    Iu_ADC = (ADCCTRL->adc0) ;
//    Iv_ADC = (ADCCTRL->adc1) ;
//    Iw_ADC = (ADCCTRL->adc4) ;
//    Udc_d = (ADCCTRL->adc2) ;
// 电流读取
// FOC_Read_Current();
//-----------------------------------------------------------------------/
// FOC控制流程

   
 	GPIO->out |= BIT8;
	
    #if (IF_CONFIG == IF_ENABLE)
    FOC_Control_Process();
    #endif
	
	GPIO->out &= ~BIT8;
	
#if 0

	#if 1
    //-----------------------------------------------------------------------/
    // 用户进程
    // FOC_User_Control();
    //---------------------------------- -------------------------------------/
    // 滤波前计算速度
    //g_smo_speed = (FOC->ro_fe3 >> 16) * MOTOR_SPEED_MAX >> 15;


    // 短滤波计算速度
    g_smo_speed_short = (FOC->ro_fe2 & 0xffff) * MOTOR_SPEED_MAX >> 15;

    // 长滤波计算速度
    g_smo_speed_long = (FOC->ro_fe2 >> 16) * MOTOR_SPEED_MAX >> 15;

    Ro_pll_comp = (FOC->angl_comp & 0xFFFF);

    TEMP_Ro.U32 = FOC->ro_angl_auto;
    Ro_angl_before = TEMP_Ro.Words.Low;
    Ro_angl_Foc = TEMP_Ro.Words.Low;
    RoPLL_angl_before = TEMP_Ro.Words.High;

    // 将增加补偿角后的位置赋值给全局变量
    TEMP_Ro.U32 = FOC->ro_angl_after;
    Ro_angl_after = TEMP_Ro.Words.Low;	   // VF或者IF下补偿后角度
    RoPLL_angl_after = TEMP_Ro.Words.High; // SMO下补偿后角度

    TEMP_Ro.U32 = FOC->ro_Id;
    // 将dq轴反馈电流赋值给全局变量
    Ro_Id_fbk = TEMP_Ro.Words.Low;
    Ro_Iq_fbk = TEMP_Ro.Words.High;

    // 将dq轴电流请求值赋值给全局变量
    TEMP_Ro.U32 = FOC->Id_req;
    Id_req = TEMP_Ro.Words.Low;
    Iq_req = TEMP_Ro.Words.High;

    // alpha/beta轴电流
    TEMP_Ro.U32 = FOC->ro_Ialf;
    Ro_Ialf = TEMP_Ro.Words.Low;
    Ro_Ibt = TEMP_Ro.Words.High;

    // 将估计电流赋值给全局变量
    TEMP_Ro.U32 = FOC->Ro_I_est;
    Ro_Ialf_est = TEMP_Ro.Words.Low;
    Ro_Ibt_est = TEMP_Ro.Words.High;

    // 将beta轴滤波前 与 滤波后 的反电势赋值
    TEMP_Ro.U32 = FOC->Ro_emf4;
    Ebt_real_time = TEMP_Ro.Words.Low;
    Ebt_filted = TEMP_Ro.Words.High;

    // 将alpha轴滤波前 与 滤波后 的反电势赋值
    TEMP_Ro.U32 = FOC->Ro_emf3;
    Ealf_real_time = TEMP_Ro.Words.Low;
    Ealf_filted = TEMP_Ro.Words.High;

    // 将滑膜得到的omega赋值给全局变量
    TEMP_Ro.U32 = FOC->ro_fe3;
    Ro_fe3_Fe = TEMP_Ro.Words.High;//((FOC->ro_fe3 >> 16) & 0xFFFF);
    Ro_fe3_Fe_short = (FOC->ro_fe2 & 0xffff);

    TEMP_Ro.U32 = FOC->ro_Ixy_raw;
    Ro_Ix_raw = TEMP_Ro.Words.Low;
    Ro_Iy_raw = TEMP_Ro.Words.High;

    ro_cntU = (FOC->ro_cntU & 0xffff);
    ro_cntV = (FOC->ro_cntV & 0xffff);
    ro_cntW = (FOC->ro_cntW & 0xffff);
    //
	TEMP_Ro.U32 = FOC->ro_Iuv;
    Ro_Iu = TEMP_Ro.Words.Low;
    Ro_Iv = TEMP_Ro.Words.High;

    TEMP_Ro.U32 = FOC->ro_Ivw;
    Ro_Iw = TEMP_Ro.Words.High;

    TEMP_Ro.U32 = FOC->ro_angl;
    svpwm_sector = TEMP_Ro.Words.High;

    Ro_Iu_raw = TEMP_Ro.Words.Low;
    Ro_Iv_raw = TEMP_Ro.Words.High;

    TEMP_Ro.U32 = FOC->ro_Udc_filt;
    Ro_Udc_filt_short = TEMP_Ro.Words.Low;
    Ro_Udc_filt_long = TEMP_Ro.Words.High;
    // TEMP_Ro.U32 = FOC->ro_Ivw;
    // Ro_Iw = TEMP_Ro.Words.High;

    TEMP_Ro.U32 = FOC->ro_Ud;
    Ro_ud1 = TEMP_Ro.Words.Low;
    Ro_uq1 = TEMP_Ro.Words.High;

    TEMP_Ro.U32 = FOC->ro_Ualf;
    Ro_Ualf = TEMP_Ro.Words.Low;
    Ro_Ubt = TEMP_Ro.Words.High;
	
    #endif

    switch (probe_switch)
    {
        case 0:
//		UartView(MOTOR_STATE * 100, Ro_Iu, Ro_Iv, Ro_angl_Foc >> 4);
            UartView(Ro_uq1, ro_cntU, Ro_Udc_filt_short, Ro_ud1);
            break;

        case 1:
            UartView(Id_req, Ro_Id_fbk, Iq_req, Ro_Iq_fbk);
            break;

        case 2:
//		UartView(Ro_ud1, Ro_Id_fbk, Ro_uq1, Ro_Iq_fbk);
            UartView(Ro_ud1, Ro_Udc_filt_short, Ro_uq1, Ro_fe3_Fe);
            break;

        case 3:
            UartView(Ro_Iu, Ealf_real_time, Ealf_filted, RoPLL_angl_after);
            break;

        case 4:
            UartView(stru_Para.Run.theta_compensation_final, MOTOR_STATE * 100, RoPLL_angl_after, Ro_angl_Foc);
            break;

        case 5:
            UartView(g_smo_speed_short, g_smo_speed_long, RoPLL_angl_after >> 4, Ro_angl_Foc >> 4);
            break;

        case 6:
            UartView(Ro_Ialf, Ro_Ialf_est, MOTOR_STATE * 100, Ro_fe3_Fe);
            break;

        case 7:
            UartView(Ro_Ibt >> 1, Ro_Ibt_est >> 1, Ebt_real_time >> 1, Ebt_filted >> 1);
            break;

        case 8:
//		UartView(MOTOR_STATE * 100, Ro_angl_after, test55, test11);
            UartView(Ro_Ix_raw, svpwm_sector, Ro_Iv, Ro_Iy_raw);
            break;

        case 9:
            UartView(Ro_Iu_raw, Ro_Iu, Ro_Iv, Ro_Iv_raw);
            //	UartView(Ro_angl_after, smo_ek_Cnt,Ro_Iu,smo_ek_SUM);
            break;

//	case 1:
//		UartView(Ro_Ialf, Ro_Ialf_est, Ro_Ualf,Ro_Ubt);
//		break;
//	case 2:
//		UartView(Ro_fe3_Fe, Ro_fe3_Fe_short, RoPLL_angl_after >> 4, Ro_angl_Foc >> 4);
//		break;

        default:
            UartView(MOTOR_STATE * 100, Ro_Iu, Ro_Iv, Ro_angl_Foc >> 4);
            break;
    }
#endif

	
    #if (UART_CONFIG_MODE == RTT_ON)

    rttStruct.data1 = Ro_angl_before;
    rttStruct.data2 = RoPLL_angl_before;

    rttStruct.data3 = Ro_Ialf;
    rttStruct.data4 = Ro_Ibt;

    SEGGER_RTT_Write(1, &rttStruct, sizeof(rttStruct));
    #endif // #if(UART_CONFIG_MODE == RTT_ON)
}

//-------------------------------------------------------------------/
// number 17 interrupt
volatile uint8_t adc_usr_busy = 0;



int16_t Iu_d_r = 0;
int16_t Iv_d_r = 0;
int16_t Iw_d_r = 0;
int16_t Udc_d_r = 0;
void adc_Handler(void)
{
    // 清除中断
    ADCCTRL->cr &= ~BIT0;

    // 取消ADC忙标志位
    adc_usr_busy = 0;

    // 保留历史值
    Iu_d_r = Iu_d;
    Iv_d_r = Iv_d;
    Iw_d_r = Iw_d;
    Udc_d_r = Udc_d;

    // 收集数据
    Iu_d = (ADCCTRL->adc0) & 0x7fff;
    Iv_d = (ADCCTRL->adc1) & 0x7fff;
    Iw_d = (ADCCTRL->adc4) & 0x7fff;
    Udc_d = (ADCCTRL->adc2) & 0x7fff;
}

//-------------------------------------------------------------------/
// number 18 interrupt
void cmp0if_Handler(void)
{
}

//-------------------------------------------------------------------/
// number 19 interrupt
void cmp1if_Handler(void)
{
}

//-------------------------------------------------------------------/
// number 20 interrupt
void cmp2if_Handler(void)
{
}

//-------------------------------------------------------------------/
// number 21 interrupt
void cmp3if_Handler(void)
{
}

//-------------------------------------------------------------------/
// number 22 interrupt
void cmp4if_Handler(void)
{
}

//-------------------------------------------------------------------/
// number 23 interrupt
void tim5_Handler(void)
{
    //  GPIO->out |= BIT26;
    TIM5->int_clr = 1;
    //	GPIO->out &= ~BIT26;
}

//-------------------------------------------------------------------/
// number 24 interrupt
void spi_Handler(void)
{
    //	GPIO->out |= BIT26;
    TIM6->int_clr = 1;
    //GPIO->out &= ~BIT26;
}

//-------------------------------------------------------------------/
// number 25 interrupt
void tsd_Handler(void)
{
}

//-------------------------------------------------------------------/
// number 26 interrupt
void uvlo_Handler(void)
{
}

//-------------------------------------------------------------------/
// number 27 interrupt
void fault_Handler(void)
{
}

//-------------------------------------------------------------------/
// number 28 interrupt
void i2c1_Handler(void)
{
}

//-------------------------------------------------------------------/
// number 29 interrupt
void foc_Handler(void)
{
}

//-------------------------------------------------------------------/
// number 30 interrupt
void swd_Handler(void)
{
}

//-------------------------------------------------------------------/
// number 31 interrupt
void tim6_Handler(void)
{
}


CurrentOffset mcCurOffset;
void GetCurrentOffset(void)
{
    for (mcCurOffset.OffsetFlag = 0; mcCurOffset.OffsetFlag < 1024; mcCurOffset.OffsetFlag++)
    {
        GPIO->out |= BIT25;
        GPIO->out |= BIT24;
        __nop();
        __nop();
        __nop();
        __nop();
        __nop();
        GPIO->out &= ~BIT25;
        GPIO->out &= ~BIT24;
        __nop();
        __nop();
        __nop();
        __nop();
        __nop();

        ADCCTRL->cr |= BIT6; // Ê¹ÄÜADC

        while (ADCCTRL->cr == BIT6)
        {
        }

        mcCurOffset.IuOffsetSum += ((ADCCTRL->adc0 & 0x7ff8));
        mcCurOffset.IuOffset = mcCurOffset.IuOffsetSum >> 4;
        mcCurOffset.IuOffsetSum -= mcCurOffset.IuOffset;

        mcCurOffset.IvOffsetSum += ((ADCCTRL->adc1 & 0x7ff8));
        mcCurOffset.IvOffset = mcCurOffset.IvOffsetSum >> 4;
        mcCurOffset.IvOffsetSum -= mcCurOffset.IvOffset;

        mcCurOffset.Iw_busOffsetSum += ((ADCCTRL->adc4 & 0x7ff8));
        mcCurOffset.Iw_busOffset = mcCurOffset.Iw_busOffsetSum >> 4;
        mcCurOffset.Iw_busOffsetSum -= mcCurOffset.Iw_busOffset;
    }

    FOC->dc_Iuv = (mcCurOffset.IuOffset) | ((mcCurOffset.IvOffset) << 16);
    FOC->dc_Iw = (mcCurOffset.Iw_busOffset);

    GPIO->out &= ~BIT24;
    GPIO->out &= ~BIT25;

    IO->en2 |= BIT12;
    IO->en2 |= BIT13;

    // Driver->DriverDRV_SR_b.DCIM=2;
    ADCCTRL->msk = 0;

    printk("mcCurOffset.IuOffset = %d\r\n", mcCurOffset.IuOffset);
    printk("mcCurOffset.IvOffset = %d\r\n", mcCurOffset.IvOffset);
    printk("mcCurOffset.IwOffset = %d\r\n", mcCurOffset.Iw_busOffset);
}
