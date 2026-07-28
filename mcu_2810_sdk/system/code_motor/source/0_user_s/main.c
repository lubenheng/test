#include <stdio.h>
#include <string.h>
#include "CMSDK_CM0.h"
#include "system_CMSDK_CM0.h"
#include "core_cm0.h"
#include "Common.h"
#include "uart.h"
#include "gpio.h"
#include "qspi.h"
#include "wdt.h"
#include "timer.h"
#include "mdu.h"
#include <stdarg.h>
#include "INIT.h"

#include "para_define.h"

#include "hardinit.h"
#include "motor_control.h"

#include "softinit.h"
#include "main.h"
#define DDE_DEBUG_MASK (0)

#if(UART_CONFIG_MODE == RTT_ON)
RTTStruct rttStruct; // RTT通道数据，在一个通道上传3个数据

#define RTT_DOWNBUFFERLEN 100
uint8_t ch0DownBuffer[RTT_DOWNBUFFERLEN] = {0};

#define RTT_UPBUFFERLEN 800
uint8_t ch1UpBuffer[RTT_UPBUFFERLEN] = {0};

#endif	//(UART_CONFIG_MODE == RTT_ON)

// for sim
uint8_t sim_en = 0;

//----------------------------------------------
void _sys_exit(int return_code)
{
label:
    goto label; /* endless loop */
}

//----------------------------------------------
volatile uint8_t printOn = 0;


extern int16_t Iu_d_r;
extern int16_t Iv_d_r;
extern int16_t Iw_d_r;
extern int16_t Udc_d_r;

const char FW_VER[] = {"FW_VER: 0.1"};
const char COMPILER_DATE[] = {"Date: "__DATE__};
const char COMPILER_TIME[] = {"Time: "__TIME__};
float pwm_freq, fe_real_base, fe_line_base, fe_rad_base;
float Uadc_ref, U_att_factor, Iadc_ref, I_amp_factor, Rsample, Ld, Lq, Ts, Rs, Ki, Ku,pub_dem, pub_factor, EK1, EK2, EK3, EK4;
// void tim0_Handler(void)
// {
//     TIM0->int_clr = 1;
//     timer0_done = 1;
//     printOn = 0;
// }

// //----------------------------------------------
// void HardFault_Handler(void)
// {
//     rldWDT(WDT_SHORT_WAIT);
//     bootWDTOpen(true);

//     while (1)
//     {
//         // 等重启
//     }
// }

//----------------------------------------------
// volatile uint8_t adc_usr_busy = 0;

// int16_t Iu_d = 0;
// int16_t Iv_d = 0;
// int16_t Iw_d = 0;
// int16_t Udc_d = 0;

// int16_t Iu_d_r = 0;
// int16_t Iv_d_r = 0;
// int16_t Iw_d_r = 0;
// int16_t Udc_d_r = 0;

// void adc_Handler(void)
// {
//     // 清除中断
//     ADCCTRL->cr &= ~BIT0;

//     // 取消ADC忙标志位
//     adc_usr_busy = 0;

//     // 保留历史值
//     Iu_d_r = Iu_d;
//     Iv_d_r = Iv_d;
//     Iw_d_r = Iw_d;
//     Udc_d_r = Udc_d;

//     // 收集数据
//     Iu_d = (ADCCTRL->adc0) & 0x7fff;
//     Iv_d = (ADCCTRL->adc1) & 0x7fff;
//     Iw_d = (ADCCTRL->adc4) & 0x7fff;
//     Udc_d = (ADCCTRL->adc2) & 0x7fff;
// }

//----------------------------------------------
// angle
// 霍尔对应的中间角
uint16_t angl_arry[6] = {
    32550, // h1,238.8
    54395, // h2,358.8
    43472, // h3,298.8
    10704, // h4,118.8
    21627, // h5,178.8
    65318  // h6,58.8
};

// 顺时针
uint16_t angl_side1[6] = {
    38011, // h5->h1,208.8
    59856, // h3->h2,328.8
    48934, // h1->h3,268.8
    16166, // h6->h4,88.8
    27088, // h4->h5,148.8
    5243,  // h2->h6,28.8
};

// 逆时针
uint16_t angl_side2[6] = {
    38448, // h5->h1,4,420-208.8
    16602, // h3->h2,6,420-328.8
    27525, // h1->h3,5,420-268.8
    60293, // h6->h4,2,420-88.8
    49370, // h4->h5,3,420-148.8
    5680,  // h2->h6,1,420-28.8
};

int16_t mdu_lpf(int16_t a, int16_t b)
{
    uint32_t tmp;
    int16_t c;

    MDU->a = (a << 16);
    MDU->b = b;
    MDU->trig = 1;

    tmp = (MDU->ctrl >> 31);

    while (tmp == 1)
    {
        tmp = (MDU->ctrl >> 31);
    }

    tmp = MDU->c;
    c = (tmp >> 16);

    return c;
}

//----------------------------------------------
char rx_cmd[50] = {0};

uint8_t uartrx_cmd_sign(uint8_t pos)
{
    char cmd_tmp_c;
    uint8_t cmd_sign;

    cmd_tmp_c = rx_cmd[pos] - 43;

    if (cmd_tmp_c == 0)
        cmd_sign = 0;
    else
        cmd_sign = 1;

    return cmd_sign;
}

int16_t uartrx_cmd_dat(uint8_t pos1, uint8_t pos2, uint8_t sign)
{
    uint8_t cmd_tmp_c;
    uint16_t cmd_dat_tmp;
    int16_t cmd_dat;
    uint8_t cnt;
    uint32_t cnt2;

    cnt = pos2;
    cnt2 = 1;
    cmd_dat_tmp = 0;

    while (cnt >= pos1)
    {
        cmd_tmp_c = rx_cmd[cnt] - 48;
        cmd_dat_tmp = cmd_dat_tmp + cmd_tmp_c * cnt2;
        cnt2 = cnt2 * 10;
        cnt = cnt - 1;
    }

    if (sign == 0)
        cmd_dat = cmd_dat_tmp;
    else
        cmd_dat = -cmd_dat_tmp;

    return cmd_dat;
}

uint32_t uartrx_cmd_dat_hex(uint8_t pos1, uint8_t pos2)
{
    uint8_t cmd_tmp_c;
    uint32_t cmd_dat;
    uint8_t cnt;
    uint32_t cnt2;

    cnt = pos2;
    cnt2 = 1;
    cmd_dat = 0;

    while (cnt >= pos1)
    {
        cmd_tmp_c = rx_cmd[cnt];

        if ((cmd_tmp_c >= 48) && (cmd_tmp_c <= 57))
            cmd_tmp_c = cmd_tmp_c - 48;
        else if ((cmd_tmp_c >= 97) && (cmd_tmp_c <= 102))
            cmd_tmp_c = cmd_tmp_c - 87;
        else if ((cmd_tmp_c >= 65) && (cmd_tmp_c <= 70))
            cmd_tmp_c = cmd_tmp_c - 55;

        cmd_dat = cmd_dat + cmd_tmp_c * cnt2;
        cnt2 = cnt2 * 16;
        cnt = cnt - 1;
    }

    return cmd_dat;
}

//----------------------------------------------
int32_t comp2norm(uint32_t dat_in, uint8_t wid)
{
    int32_t dat_out;
    uint32_t tmp;
    uint8_t rest;

    rest = 32 - wid;
    tmp = (dat_in << rest);
    dat_out = (int32_t)tmp;
    dat_out = (dat_out >> rest);

    return dat_out;
}



//------------------------------------------------
extern struct_FOC_Start				stru_FOC_Start;
extern struct_Para					stru_Para;
int main(void)
{
    PMU->wr_key = 0x55AAAA55;
    PMU->cfg_normal = 0<<0 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 0<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10; // DRV3 V1.4

	//uint32_t UdcGain_Buff=0;
//		   int16_t cmd_dat0_s;
//    uint32_t cmd_dat0_ui, cmd_dat1_ui;
    
    /*System Init*/
    System_Init();
////		gpio_outpp_test();
//	GPIO_input_test();
//	IO_interputp_test();
//	NVIC_EnableIRQ(int1_irq);
//	NVIC_SetPriority(int1_irq,0);
//	NVIC->ISER[0] = BIT11;

    //------------------------------------- 
    UART0_Init();
		

    //-------------------------------------
    UERT1_init();

  
	
	
	
#if (DDE_DEBUG_MASK == 0)
	#if(UART_CONFIG_MODE == RTT_ON)
    SEGGER_RTT_ConfigUpBuffer(1, "JSCope_i2i2i2i2", ch1UpBuffer, RTT_UPBUFFERLEN, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
	#endif//(UART_CONFIG_MODE == RTT_ON)
#endif

   PUM_init();

#if (DDE_DEBUG_MASK == 0)
    Clear_rx_fifo();
#endif

#if (DDE_DEBUG_MASK == 0)
    //-------- 等500ms再采样 -----------
    if (sim_en == 0)
    {
        timer0_wait(5e-1 * CPU_CLK);
    }
#endif

  HALL_Init();

    //------- ADC初始化之IO配置 -----------


	timer3_init(3000);//1ms  3MHz----10000次------1ms


			adc_init();
//			MUD_init();
		EPWM_init(48000000.0/EPWM_FREQ - 1,12);
    GetCurrentOffset();		
#if (DDE_DEBUG_MASK == 0)
    printk("ACCCycle  = %d\r\n",stru_Para.Start.ACCCycle);
    printk("OmegaMax  = %d\r\n",stru_Para.Start.OmegaMax);

    printk("Iu_d  = %d\r\n", Iu_d_r);
    printk("Iv_d  = %d\r\n", Iv_d_r);
    printk("Iw_d  = %d\r\n", Iw_d_r);
    printk("Udc_d = %d\r\n", Udc_d_r);
    printk("---------\r\n");
#endif
    //    printk("Iu_d  = %d\r\n", (tmp>>8));
    //    printk("Iv_d  = %d\r\n", (tmp2>>8));
    //    printk("Iw_d  = %d\r\n", (tmp3>>8));

		foc_init();
//    //------- DRIVER配置 -----------
		drive_init();
    /*
    0：不产生中断
     1：计数器向上计数时产生中断
    2：计数器向下计数时产生中断
    3：计数器无论向上还是向下，都产生中断
    */
    DRV->sr |= BIT1; 
    NVIC_EnableIRQ(dcif_irq);
    NVIC_SetPriority(dcif_irq, 3);

    

//------ 根据霍尔探知初始角度 --------
//    tmp = (((GPIO->in) >> 24) >> 4) & 7;
//    hall = tmp;
//    idx = hall - 1;
//    angl = angl_arry[idx];
//    FOC->angl = angl;

    // Ud/Uq
    // FOC->Ud = 0 | ((0+(Udc_d_r>>2)) << 16);
#if (UDC_CONVERT_CONFIG == UDC_CONVERT_ENABLE)
	FOC->Ud = 0 | ((0 + (0x0400)) << 16);  //Uq给定 VF
#else
	FOC->Ud = (((0x1800)) << 16) | (0 << 0);  //Uq给定 VF
#endif
	//FOC->Ud = (0x0400) | ((0x0) << 16);  	  //Ud给定 预定位
    //FOC->Ud = 0 | ((0 + (0x2000)) << 16);
    //	FOC->Ud = 0 | ((0+(0x1d50)) << 16);//1d50
    //    //配置加速度
    //    FOC->ramp1 = +190;

#if (ALIAN_CONFIG == ALIAN_ENABLE)
	FOC->Ud = ((0x0) << 16)|(0x0800 << 0);  	 //Ud给定预定位,Ud = 0;
	FOC->angl = 0*65536/360;
	// 配置加速度
    FOC->ramp1 = 0; //+144;
#else
	// 配置加速度
#if (IF_CONFIG == IF_ENABLE)
		FOC->ramp1 = (MC_Start_Init_Speed*EPWM_FREQ>>16);//160; //+144;
#else
		FOC->ramp1 = 40;//160; //+144;
#endif

#endif
	
//	
//    // 配置加速度
//    FOC->ramp1 = 100; //+144;
    // 设定加速度
    FOC->ramp2 = (5 << 0) | (250 << 16) | (10 << 24);



    MDU->z = (uint16_t)((1.0 / 700) * 32768);
    //stop = 0;
		
		
		#if (Config_Shunt_Mode == Single_Shunt)
		{
			sample_init();
		}
		#else
		{
		
		}
		#endif

		// Udc采样屏蔽
//		FOC->ctrl0 &= ~(0x3 << 20);
//		FOC->ctrl0 |= (0x3 << 20);
  printk("20260104---\r\n");

    while (1)
    {

#if (DDE_DEBUG_MASK == 0)
//			uart_process_command();
//			UartView(11000, 0, FOC->ro_angl_after & 0xFFFF, FOC->ro_angl_after >>16);
//			
#endif
    } // while(1)
} // main
