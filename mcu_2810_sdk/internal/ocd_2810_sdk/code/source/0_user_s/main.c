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

// uint16_t    angl_side2[6] = {
//                 48934, //h3->h1,268.8
//                 5243,  //h6->h2,28.8
//                 59856, //h2->h3,328.8
//                 27088, //h5->h4,148.8
//                 38011, //h1->h5,208.8
//                 16166  //h4->h6,88.8
//             };

// uint8_t hall = 0;
// uint8_t hall_r = 0;
// uint16_t angl = 0;

// void tim1_Handler(void)
// {
//     uint32_t tmp;
//     uint8_t idx;

//     TIM1->sr &= ~BIT1;
// //    tmp = (((GPIO->in) >> 24) >> 4) & 7;

// //    hall_r = hall;
// //    hall = tmp;
// //    idx = hall - 1;

// //    // 更新角度
// //    tmp = (DRV->cr >> 6) & 1;
// //    // tmp = (FOC->ctrl0 >> 24) & 1;

// //    if (tmp == 0)
// //    {
// //        angl = angl_side1[idx];
// //    }
// //    else
// //    {
// //        angl = angl_side2[idx];
// //    }
// //    FOC->angl = angl;
// }

//----------------------------------------------
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
	soft_delay_1ms(10000);
	
	PMU->wr_key = 0x55aaaa55;
	PMU->cfg_normal = 0<<0 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 0<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10; //DRV3 V1.4
	
	//uint32_t UdcGain_Buff=0;
    uint32_t cnt;
    uint32_t tmp;
    //short tmp_signed;
    //uint8_t idx;
    //int16_t Id, Iq;
    //int16_t Ud, Uq;
    //int16_t IuDC, IvDC, IwDC;
    //uint8_t stop;

    extern int __Vectors;
    extern int __Vectors_Size;

    // uart rx command
    //uint8_t cmd_sign0, cmd_sign1;
    int16_t cmd_dat0_s;
    uint32_t cmd_dat0_ui, cmd_dat1_ui;
    //int32_t dat0, dat1, dat2;

    // angle estimation
    float pwm_freq, fe_real_base, fe_line_base, fe_rad_base;
    float Uadc_ref, U_att_factor, Iadc_ref, I_amp_factor, Rsample, Ld, Lq, Ts, Rs, Ki, Ku,pub_dem, pub_factor, EK1, EK2, EK3, EK4;
    /*System Init*/
    System_Init();

    //-------------------------------------
    // 选择UART0 时钟  
    //0：RCO时钟(24MHz)  1：PLL时钟(48MHz)
    DIGCTRL->clk_en0 &= ~BIT13;   //0：RCO时钟(24MHz)
    //DIGCTRL->clk_en0 |= BIT13;  //1：PLL时钟(48MHz)


    uart0Tx_init();
    uart0Rx_init();

    //-------------------------------------
    // 选择UART1 时钟  
    //0：RCO时钟(24MHz)  1：PLL时钟(48MHz)
    //DIGCTRL->clk_en0 &= ~BIT14; //0：RCO时钟(24MHz)
    DIGCTRL->clk_en0 |= BIT14;  //1：PLL时钟(48MHz)

    uart1Tx_init();
    //uart1Rx_init();

    // IO->ana &= ~BIT8;
	// GPIO->out_en |= BIT8;
	// GPIO->out    |= BIT8;

    //P03 used as TIMER4, TIMER4 is used as FG PWM output
    // IO->ana      &= ~BIT3;          //disable analog function
    // GPIO->out_en |= BIT3;           //enable digital output 
    // IO->en       |= BIT4;           //pad_tim4_en

    //P1.0 as output toggle GPIO
    IO->ana      &= ~BIT8;              //disable analog function
	GPIO->out_en |= BIT8;               //enable digital output 
	GPIO->out    &= ~BIT8;              //P10 output 0 
	
    //P1.1 as digital input GPIO
	IO->ana        &= ~BIT9;            //disable analog function
	IO->ana        &= ~BIT10;           //disable analog function
	GPIO->out_en   |= BIT10;            //enable digital output
	GPIO->out      &= ~BIT10;           //P12 output 0 
	
//	while (1)
//	{
//		tmp = GPIO->in;
//		tmp = (tmp >> 9) & 1;
//		if (tmp == 0)
//			GPIO->out &= ~BIT10;
//		else
//			GPIO->out |= BIT10;
//	}
    	
	
	
	
#if (DDE_DEBUG_MASK == 0)
	#if(UART_CONFIG_MODE == RTT_ON)
    SEGGER_RTT_ConfigUpBuffer(1, "JSCope_i2i2i2i2", ch1UpBuffer, RTT_UPBUFFERLEN, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
	#endif//(UART_CONFIG_MODE == RTT_ON)
#endif

    //-------------------------------------
    PMU->wr_key = 0x55aaaa55;
    //PMU->cfg_normal = 1 << 0 | 3 << 4 | 1 << 7 | 1 << 13 | 1 << 14 | 1 << 15 | 1 << 16;    //DRV2 V.144 
    PMU->cfg_normal = 0<<0 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 0<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10; //DRV3 V1.4
    //-------------------------------------
    QSPI->basic |= 1 << 11; // Cache en

#if (DDE_DEBUG_MASK == 0)
    // 将RX FIFO中残留数据先清除 （必须在这儿清，因为引入残留的就是上一句话）
    tmp = (UART0->int_clr >> 9) & 1;
    while (tmp == 0)
    {
        tmp = UART0->dat;
        tmp = (UART0->int_clr >> 9) & 1;
    }

    printk("FOC angle trace.\r\n");
#endif

#if (DDE_DEBUG_MASK == 0)
    //-------- 等500ms再采样 -----------
    if (sim_en == 0)
    {
        timer0_wait(5e-1 * CPU_CLK);
    }
#endif

    //-------- 打通HALL通道 -------- ---
    // IO HALL使能
    IO->en |= BIT8 | BIT10 | BIT12;
    IO->ana &= ~(BIT28 | BIT29 | BIT30);

    // 读取HALL信息
    // tmp = (((GPIO->in) >> 24) >> 4) & 7;
    // printk("hall: %d\r\n",tmp);

    //------ 启动TIMER1的HALL相关中断 -----------
    // 打开timer1时钟
    DIGCTRL->clk_en0 |= BIT25;

    // 开启timer1使能
    TIM1->cr0 |= BIT8;

    // 信号输入沿记性和比较器使能选择
    tmp = TIM1->dbr7;
    tmp &= ~(0x7 << 12);
    tmp |= 0x7 << 12;
    TIM1->dbr7 = tmp;

    // 配置换相状态机
    TIM1->cr4 = 7;

    // 打开pdie中断使能
    TIM1->ier |= BIT1 | BIT9;

    // 在CPU侧打开中断使能
    NVIC->ISER[0] = BIT11;

    //------- ADC初始化之IO配置 -----------
    // Iu: CH0(P30)
    // Iv: CH1(P33)
    // Iw: CH4(P15)
    // Udc:CH2(P25)

    // Udc选择CH2
    // IO->en2 &= ~BIT6;

    IO->ana &= ~(0xffU << 24);
    GPIO->out_en |= BIT26;
    GPIO->out_en |= BIT25;
    GPIO->out_en |= BIT24;

	//timer2_init(9000);

	timer3_init(3000);//1ms  3MHz----10000次------1ms

	//timer4_init();//43ms


    //		IO->en2 |= BIT12;
    //		IO->en2 |= BIT13;

    //------- ADC初始化之7616终端配置 -----------
//    // 配置ADC截取范围
//    ADCINF->cfg = 3;

//    // 等待AD7616可以用
//    tmp = ((ADCINF->cfg) >> 31) & 1;
//#if (DDE_DEBUG_MASK == 0)
//    while (tmp == 0)
//    {
//        tmp = ((ADCINF->cfg) >> 31) & 1;
//    }
//#endif
//    // 更改Udc(CH2)量程，Udc被测端电压是1.87V，所以量程改为2.5V
//    // 电流量程是5V
//    //    ADCINF->a4  = 2 | (2<<2) | (1<<4) | (3<<6);
//    //    ADCINF->a5  = 2 | (3<<2) | (3<<4) | (3<<6);
//    //    ADCINF->a6  = 3 | (3<<2) | (1<<4) | (3<<6);

//    ADCINF->a4 = 0xAA;
//    ADCINF->a5 = 0xAA; //  VREF=5
//    ADCINF->a6 = 0xAA;
//    ADCINF->a7 = 0xAA;

//    ////读取量程
//    // tmp = ADCINF->a4;
//    // printk("ADC range = 0x%x\r\n", tmp);

//    // 修改ADC过采样数量
//    ADCINF->a2 = (1 << 2);

		
		
		
		
		
    //------- ADC初始化之ADC控制器配置 -----------
    // 打开ADC时钟
    DIGCTRL->clk_en0 |= BIT20;

    // 打开CPU侧中断使能
    NVIC->ISER[0] = BIT17;

    // 中断使能
    ADCCTRL->cr |= BIT1;

    // 数据对齐方式（0：右对齐，1：次高位对齐）
    ADCCTRL->cr |= BIT2;

    // ADC使能
    ADCCTRL->cr |= BIT7;


    // ADC采样通道选择(Iu(CH0),Iv(CH1),Iw(CH4),Udc(CH2))
    IO->en2 &= ~BIT6;
	  ADCCTRL->msk |= BIT0 | BIT1 | BIT4 | BIT2;

    // ADC group0的采样时长（至少0.125us）
    tmp = ADCCTRL->scyc;
    tmp &= ~(0xf << 0);
    tmp |= 2 << 0;
    ADCCTRL->scyc = tmp;

    // ADC group1的采样时长（至少0.125us）
    tmp = ADCCTRL->scyc;
    tmp &= ~(0xf << 4);
    tmp |= 2 << 4;
    ADCCTRL->scyc = tmp;
    printk("11.\r\n");

    //------- ADC初始化之7882终端配置 -----------
		
//		 ADCINF->ch0_7 = 0|1 << 4|5 << 8|4 << 12|2 << 16|5 << 20|8 << 24|9 << 24;
//		 ADCINF->ch8_15 = 10|7 << 4|5 << 8|11 << 12|12 << 16|13 << 20|14 << 24|15 << 24;
//		
//		 ADCINF->tim =  3|2<<8|1<<16;
//		 printk("ADCINF->ch0_7  = %d\r\n",ADCINF->ch0_7);
//		 printk("ADCINF->ch8_15  = %d\r\n",ADCINF->ch8_15);
//		 ADCINF->ctrl|=BIT8;	//采样触发方式
//		 
//		 ADCINF->ctrl|=1;
//		 timer0_wait(25e-3*CPU_CLK);
//		
//		 ADCINF->ctrl&=~BIT2;

//		ADCCTRL->cr|=BIT6;	

     ADCINF->ch0_7 = 0|1 << 4|5 << 8|4 << 12|3 << 16|5 << 20|8 << 24|9 << 24;
		 ADCINF->ch8_15 = 10|7 << 4|5 << 8|11 << 12|12 << 16|13 << 20|14 << 24|15 << 24;
		
		 ADCINF->tim =  3|2<<8|1<<16;
		 printk("ADCINF->ch0_7  = %d\r\n",ADCINF->ch0_7);
		 printk("ADCINF->ch8_15  = %d\r\n",ADCINF->ch8_15);
		 ADCINF->ctrl|=BIT8;	//采样触发方式
		 
		 ADCINF->ctrl|=1;
		 timer0_wait(25e-3*CPU_CLK);
		
		 ADCINF->ctrl&=~BIT2;

		ADCCTRL->cr|=BIT6;	

    //------- MDU配置 -----------
    // 打开MDU时钟
    DIGCTRL->clk_en0 |= BIT9;

    // MDU使能
    MDU->ctrl |= BIT0;

    // 选择计算模式为LPF
    tmp = MDU->ctrl;
    tmp &= ~(0x7 << 4);
    tmp |= (3 << 4);
    MDU->ctrl = tmp;

    // 配置K，百分之一
    MDU->z = 256;

    //-------- LPF滤波 -----------
    //DRV->arr = 3000 - 1;
    //DRV->arr = 3000 - 1;
    //printk("11\r\n");
    DRV->arr = 48000000.0/EPWM_FREQ - 1; //PWM载波周期设置
    DRV->dtr = 12; //死区时间
    DRV->cr = 0x98;
    //DRV->out |= 0x2a;
    DRV->out = 0;
    //printk("22\r\n");
    GetCurrentOffset();
    DRV->out = 0;
    //printk("33\r\n");
    //    tmp  = 0;
    //    tmp2 = 0;
    //    tmp3 = 0;

    //    for (cnt=0; cnt<256; cnt++)
    //    {
    //        //设置ADC忙标志位
    //        adc_usr_busy = 1;

    //        //触发ADC采样
    //        ADCCTRL->cr |= BIT6;

    //        //等待ADC采样结束
    //        while(adc_usr_busy == 1)
    //        {
    //        }
    //
    //        //LPF滤波，赋初值
    //        if (cnt > 0)
    //        {
    //            Iu_d_r  = mdu_lpf(Iu_d_r,  Iu_d);
    //            Iv_d_r  = mdu_lpf(Iv_d_r,  Iv_d);
    //            Iw_d_r  = mdu_lpf(Iw_d_r,  Iw_d);
    //            Udc_d_r = mdu_lpf(Udc_d_r, Udc_d);
    //        }

    //        tmp  = tmp  + Iu_d;
    //        tmp2 = tmp2 + Iv_d;
    //        tmp3 = tmp3 + Iw_d;
    //    }
		
//		FOC->dc_Iuv = (11328) | ((11328) << 16);
//	  FOC->dc_Iw = (11328);
		
		
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

    //-------- FOC配置 -----------
    // 打开FOC时钟
    DIGCTRL->clk_en0 |= BIT10;

    // 配置Udc
    //    FOC->Udc = Udc_d_r;
	
	#if(UDC_CONVERT_CONFIG == UDC_CONVERT_ENABLE)
	 
	 FOC->ctrl0 &= ~BIT21;   
    // Udc自动采样并使用
 
	// 4251 = 24/185*4095*8
   FOC->Udc = 18022; ////0x7000;
	
   FOC->ctrl0 &= ~BIT20;  //滑膜后再打开
    // 双三下Udc采样位置
   // FOC->ctrl0 |= BIT22;
	  printk("FOC->ctrl0  = %d\r\n",FOC->ctrl0);
		
	#else
	//FOC->Udc = 18022; //4251; ////0x7000;
//	FOC->Udc = 19661; //4251; ////0x7000;

	//	FOC->Udc = 26214; 
		FOC->Udc = 32767;
	#endif

    //    //配置直流偏置
    //    FOC->dc_Iuv = (Iu_d_r) | ((Iv_d_r)<<16);
    //    FOC->dc_Iw  = Iw_d_r;

    // 配置直流偏置
    //    FOC->dc_Iuv = (16383) | ((16383)<<16);
    //    FOC->dc_Iw  = 16383;

    // Ud/Uq自动  电流环时开启
	 #if(IF_CONFIG == IF_ENABLE)
   FOC->ctrl0 &= ~BIT0;
   FOC->ctrl0 &= ~BIT1;
	 FOC->Ud_lim = ((uint32_t)(-28000) << 16)|((uint32_t)(18384) << 0); // Udmin | Udmax 
	 FOC->Uq_lim = ((uint32_t)(-28000) << 16)|((uint32_t)(29000) << 0); // Uqmin | Uqmax
     //低位为电压利用率设置，范围为[0,1);高位为过调制利用率设置，最高位电压利用率的1.13倍。通常高位<低位
	 FOC->wr_Udq_lim = ((uint32_t)(0.90 * 16384) <<16 ) | (uint32_t)(0.90 * 16384);
	 #endif

    // 使用估计角
    // FOC->ctrl0 &= ~BIT2;

    // 估计角使用SMO方式
    // FOC->ctrl0 |= BIT3;

    // pll 滑膜锁相环PI
			//   FOC->fe_pi = 0x4FFF | (0x2000 << 16); // PLL 2025/10/27  镲窗玻璃   
//	   FOC->fe_pi = 0x1FFF | (0x1000 << 16); // PLL 2025/10/27  镲窗玻璃    
	  // FOC->fe_pi = 0x4000 | (0x800 << 16); // PLL 2025/10/27  格智美落地扇
//			FOC->fe_pi = 0x1100 | (0x100 << 16); // PLL 2025/12/13  美的散装落地扇
//		FOC->fe_pi = 0x400 | (0x100 << 16); // PLL 2025/12/13  美的散装落地扇
//			FOC->fe_pi = 0x1400 | (0x100 << 16); // PLL 2025/12/13  美的散装落地扇
   FOC->fe_pi = 0x4FFF | (0x2000 << 16); // PLL 2025/10/27 
//			FOC->fe_pi = 0x4000 | (0x1000 << 16); // PLL 2025/12/13  美的散装落地扇
//			FOC->fe_pi = 0x2000 | (0x200 << 16); // PLL 2025/12/13  美的散装落地扇
    // 无感角度使用arctan得到的角度（注：反向电动势必须经过短滤波后才能使用）
    // FOC->ctrl0 |= BIT4;

    // 无感速度使用arctan得到的速度（注：反向电动势必须经过短滤波后才能使用）
    // FOC->ctrl0 |= BIT5;

    // 估计角度用的是来自哪里的反向电动势：滤波后
    // 注：用arctan方法时，必须开启本选项。因为它对电动势的准确度要求比PLL高很多，电动势不准，会导致电机不转，且电流过流
    FOC->ctrl0 |= BIT6;

    // 反向电动势滤波系数中用的速度来自：长滤波速度
     //FOC->ctrl0 |= BIT7;

    // 估计电流用的是来自哪里的反向电动势：滤波后
	FOC->ctrl0 |= BIT8;

    // APB观察的来自哪里的反向电动势：滤波后
    // FOC->ctrl0 |= BIT9;

    // 开启对估计的速度进行限制
    // FOC->ctrl0 |= BIT10;

    // 速度限制策略，用HOLD
    // FOC->ctrl0 |= BIT11;

    // 使能强制爬坡模式
    // FOC->ctrl0 |= BIT12;

    // 无感切换方式为场景4
    // FOC->ctrl0 |= BIT13;

    // 开启电流采样使能
    FOC->ctrl0 |= BIT14;

    // 估计角使能
    FOC->ctrl0 |= BIT15;

    #if(HIGH_SPEED_MODE_CONFIG == HIGH_SPEED_MODE_ENABLE)
		// 手动设置滤波使能
		FOC->ctrl0 |= BIT16;

		// 手动滤波系数为2^20
		//FOC->E_coef_manual = 0x1fffffU;
		// 手动滤波系数为2^22
		FOC->E_coef_manual = 0x7fffffU;
	#else
		// 手动设置滤波使能
	//	FOC->ctrl0 |= BIT16;

		// 手动滤波系数为2^20
		FOC->E_coef_manual = 0x1fffffU;
    #endif
	

   
//     FOC->ctrl0 &= ~BIT16;

    // 选择五段式
    // FOC->ctrl0 &= ~BIT17;

	#if (Config_Shunt_Mode != Single_Shunt)
	{
//    // 配置duty最大值限制
//    tmp = FOC->duty_lim;
//    tmp &= ~(0x1fff << 16);
//    // 占空比最大值为43.5us，空余19us，为三路采样提供时间
//    tmp |= (1044 << 16);
//    FOC->duty_lim= tmp;

    // 开启duty最大值限制使能
    FOC->ctrl0 |= BIT18;

    // 设置duty最小脉冲消除值
    tmp = FOC->duty_lim;
    tmp &= ~(0xff << 0);
    // 占空比应不小于2us，否则将消除
    tmp |= (48 << 0);
    FOC->duty_lim = tmp;

    // 开启duty最小脉冲消除使能
    // FOC->ctrl0 |= BIT19;

    // Udc自动采样并使用
    // FOC->ctrl0 &= ~BIT20;

    // 让Udc采样
     FOC->ctrl0 &= ~BIT21;

    // 双三下Udc采样位置
    // FOC->ctrl0 |= BIT22;

    // 强迫估计器速度跟随ddir
    // FOC->ctrl0 |= BIT24;

    // 选择电流采样方式
    tmp = FOC->ctrl1;
    tmp &= ~(0x3 << 0);
    tmp |= (1 << 0);
    FOC->ctrl1 = tmp;

    // 配置双三下采样严格程度
    tmp = FOC->ctrl1;
    tmp &= ~(0x3 << 2);
    tmp |= (2 << 2);
    FOC->ctrl1 = tmp;

    // 开启死区补偿
    // FOC->ctrl1 |= BIT4;

    // 开启采样保持器（双三），波形溢出处理（单）
    FOC->ctrl1 |= BIT5;

    // 电流间隔采样（双三），放宽用户ADC范围（单）
    // FOC->ctrl1 |= BIT6;

    // 开启三路采样使能
    // FOC->ctrl1 |= BIT7;

    // 开启双三下采样保持电路
    FOC->ctrl1 |= BIT8;

    // 让用户ADC与电流电压采样平等竞争
    // FOC->ctrl1 |= BIT9;

    FOC->sample_time |= (5 << 24);
    FOC->sample_time |= (5 << 28);
			
	
			
			
	//电压利用率
	FOC->wr_Udq_lim = ((uint32_t)(0.9 * 16384) <<16 ) | (uint32_t)(0.9 * 16384);	
	}
	#endif
		
	#if (Config_Shunt_Mode == Single_Shunt)		
    //选择电流采样方式
    tmp = FOC->ctrl1;
    tmp &= ~(0x3<<0);
		// 0:单 1：双 3：三
    tmp |= (0<<0);
    FOC->ctrl1 = tmp;
	#endif

    // 设置Udc短滤波系数
    tmp = FOC->Udc_coef;
    tmp &= ~(0xffff << 0);
    tmp |= (3277 << 0); // 0.1
    FOC->Udc_coef = tmp;

    // 设置Udc长滤波系数
    tmp = FOC->Udc_coef;
    tmp &= ~(0xffffU << 16);
    tmp |= (33 << 16); 			// 0.001
    FOC->Udc_coef = tmp;

    // 双三下，PWM下降后等多长时间才能采样
    #if(Config_Shunt_Mode == Double_Shunt)
    {
        tmp = FOC->sample_time;
        tmp &= ~(0xff << 0);
//        tmp |= (0x80) | ((2 * 24) << 0); // 3.3us
//			  tmp |= 0xe8;  // 1us
//			  tmp |= 0xd0;  // 2us	
			  tmp |= 0x0;  // 0us				
        FOC->sample_time = tmp;
    }
    #else //单电阻采样配置
    {
        tmp = FOC->sample_time;
        tmp &= ~(0xff << 0);
        tmp |= ((4 * 24) << 0); // 4us
        FOC->sample_time = tmp;
    }
    #endif

    // 设定采样窗口时间 (16us)
    tmp = FOC->sample_time;
    tmp &= ~(0xff << 8);
    tmp |= ((16 * 24) << 8);
    FOC->sample_time = tmp;

    // 配置Id/Iq
    FOC->Id_req = ((0 + 0x0600) << 16) | (0x0000 << 0);// Iq | Id

    // 配置Kp/Ki
    // 0x7000, 0x0000, 77ma, unmove
    // 0x6000, 0x0000, 72ma, unmove
    // 0x5000, 0x0000, 260ma, unmove
    // 0x4000, 0x0000, 260ma, unmove
    // 0x3000, 0x0000, 330ma, unmove
    // 0x2000, 0x0000, 79ma, unmove
    // 0x1000, 0x0000, 133ma, unmove
    // 0x0c00, 0x0000, 55ma, rise very slowly
    // 0x0800, 0x0000, 55ma, from slow to high, then from high to slow, finally stop
    // 0x0400, 0x0000, 55ma, unmove
    // 0x00c0, 0x0000, 55ma, unmove

    // 0x0000, 0x0800, 55ma, rise very slow, can move high, can't move low, 74ma->1000rpm->0x9c
    // 0x0000, 0x0400, 55ma, unmove
    // 0x0000, 0x0100, 55ma, unmove
    // 0x0000, 0x00c0, 55ma, unmove

    // 0x7000, 0x0800, run then stop
    // 0x4000, 0x0800, run then stop
    // 0x2000, 0x0800, can contrl, 0x5c->970rpm->the fast->77ma
    // 0x1000, 0x0800, can contrl smoothly, 0x40->970rpm->the fast->88ma

    //FOC->pi = 0x1000 | (0x0800 << 16);
	FOC->pi = (START_QKI << 16) | START_QKP;

    // 配置PI单位移动策略
//    tmp = FOC->pi_shift;
//    tmp &= ~(0xf << 0);
//    tmp |= (4 << 0);
//    FOC->pi_shift = tmp;

    // 角度补偿
    FOC->angl_comp = 0; 

    // 配置估计速度短滤波系数
    tmp = FOC->fe_coef;
    tmp &= ~(0xffff << 0);
    tmp |= ((uint32_t)(10.0 / 10 * 32768) << 0);
    FOC->fe_coef = tmp;

    // 配置估计速度长滤波系数
    tmp = FOC->fe_coef;
    tmp &= ~(0xffffU << 16);
    tmp |= ((uint32_t)(100.0 / 100 * 32768) << 16);
    FOC->fe_coef = tmp;

		//MOTOR_PAIRS=3
		//EPWM_FREQ=16000
	  //MOTOR_SPEED_MAX=16000
    // 速度基准: 4K*4/60/16K (float)
    pwm_freq = EPWM_FREQ;
    fe_real_base =  MOTOR_SPEED_MAX * MOTOR_PAIRS  / 60;
    fe_line_base = fe_real_base / pwm_freq;
    FOC->fe_base = (uint32_t)(fe_line_base * 32768);
	
	
	 FOC->fe_coef = ((0x80) << 16) | ((0x80) << 0); //0x80----16khz 10Hz

    // 配置速度PI单位移动策略
#if(HIGH_SPEED_MODE_CONFIG == HIGH_SPEED_MODE_ENABLE)
    FOC->pi_shift |= (0x0<<4); //锁相环PI
    FOC->pi_shift |= (0x1<<0); //电流环PI
		
#else
		FOC->pi_shift |= (0x0<<4);//锁相环PI
		//FOC->pi_shift |= (0x4<<0); //电流环PI
#endif

    // 反向电动势滤波系数的系数: 2*pi*4K*4/60/16K (float)
    fe_rad_base = 2 * 3.1415926 * fe_line_base;
    //FOC->emf_coef = (uint32_t)(fe_rad_base * 32768);
	/*20250929 attention: 目前大于500 闭环会死掉 ！！！*/
//	FOC->emf_coef = (uint32_t)(100);
//	FOC->emf_coef = (uint32_t)(25);

	FOC->ctrl0 &= ~BIT16;
  FOC->emf_coef = (uint32_t)(fe_rad_base*8192);
	FOC->emf_coef |= ((uint16_t)Cfg_emf_coef_min<<16);

    // 滑膜系数设定
	FOC->smo_h = (uint32_t)((SMO_KE)|(SMO_KSLIDE <<16));
	//FOC->smo_h = 0x04cb1800;

    // Uadc_ref: ADC采电压时用的参考(V, float)
	Uadc_ref = 2.5;
	
	#if(UDC_CONVERT_CONFIG == UDC_CONVERT_ENABLE)
	  U_att_factor = 16;//37; 
	#else
		U_att_factor = 16;
	#endif
	
	//手动电压折算
    //U_att_factor = 24.0/Uadc_ref;//37;
		
		//自动电压折算
// 		U_att_factor = 16;//37;
 		
    // Iadc_ref: ADC采电流是用的参考(V, float)
    Iadc_ref = 2.5;

    // I_amp_factor: 电流放大倍数
    I_amp_factor = 5;

    // Rsample: 电流采样电阻(欧姆, float)
    Rsample = 20e-3;

    // EMF_factor: 反电动势常数 (V/Krpm, float)
    // EMF_factor = 12.25;
    //EMF_factor = K_EMF;

    // Phy_Krpm: 最大物理速度（考虑极对数, Krpm, float）
    //Phy_Krpm = 3; // 1.3;

    // Ld (H, float)
    Ld = MOTOR_LS; // 2.675e-3;

    // Lq (H, float)
    Lq = MOTOR_LS; // 2.675e-3;

    // Ts: PWM周期 (s, float)
    Ts = 1 / pwm_freq;

    // Rs (欧姆，float)
    Rs = MOTOR_RS; // 3.48;

    // 电流单位转换因子 (float)
  //  Ki = Iadc_ref / (I_amp_factor * Rsample * 2.0);
    Ki = Iadc_ref / (I_amp_factor * Rsample * 1.0);

    // 电压单位转换因子 (float)
#if(HIGH_SPEED_MODE_CONFIG == HIGH_SPEED_MODE_ENABLE)
   Ku = 5.0;//Uadc_ref * U_att_factor;
	 
	 Ku1 = Uadc_ref * U_att_factor/1.7321;//23
	 
//	 SMO_KU_SW = Ku1-Ku;
	//	Ku = Uadc_ref * U_att_factor;
#else
   //  Ku = Uadc_ref * U_att_factor;
		 Ku = Uadc_ref * U_att_factor/1.7321;
#endif
	printk("Ku =%f\r\n", Ku);
	printk("Ki =%f\r\n", Ki);

    // 反电动势单位转换因子 (float)
    //Ke = EMF_factor * Phy_Krpm;

    // 公共分母 (float)
    pub_dem = Ld + Rs * Ts;

    // 公共因子 (float)
    pub_factor = Ts / pub_dem;
	

    // 计算估计器输出参数 (float)
	#if 0 //老版本滑膜
		EK1 = Ld / pub_dem*32768.0; 
	#else
		EK1 = -Ld / pub_dem*32768.0;
	#endif
	printk("EK1 =%f\r\n", Ld / pub_dem);
//    EK2 = 0.6667* pub_factor * Ku / Ki;//1.6205    乘0.9/0.8
//    EK3 = 0.6667* pub_factor * Ku / Ki; //0.3508   乘0.9/0.8
    EK2 =  pub_factor * Ku / Ki;//1.6205    乘0.9/0.8
    EK3 =  pub_factor * Ku / Ki; //0.3508   乘0.9/0.8
//    EK2 = 0.3334* pub_factor * Ku / Ki;//1.6205    乘0.9/0.8
//    EK3 = 0.3334* pub_factor * Ku / Ki; //0.3508   乘0.9/0.8
    // EK3 = EK2;
		
//		SMO_EK_SW= EK3-EK2;
//		u16_SMO_EK_SW=((uint32_t)(SMO_EK_SW * 8192 ));
		
    EK4 = (Ld - Lq) * fe_rad_base / pub_dem;

	
    printk("EK2 =%f\r\n", EK2);
    printk("EK3 =%f\r\n", EK3);
  
	printk("MOTOR_RS =%f\r\n", MOTOR_RS);
    printk("MOTOR_LS =%f\r\n", MOTOR_LS);
    printk("%s,%s,%s\r\n", FW_VER, COMPILER_DATE, COMPILER_TIME);

#if (UDC_CONVERT_CONFIG == UDC_CONVERT_ENABLE)
		FOC->ek12 = (((int16_t)(EK1))&0xFFFFU) | (((uint32_t)(EK2 * 8192 )) << 16);
#else
	FOC->ek12 = (((int16_t)(EK1))&0xFFFFU) | (((uint32_t)(EK2 * 8192 )) << 16);
#endif
	
    FOC->ek34 = (((uint32_t)(EK3 * 32768))&0xFFFFU) | (((uint32_t)(EK4 * 32768)) << 16);

    // 配置KFG
    FOC->kfg = 24e6 / (fe_real_base * 8);


    ////场景4：
    // FOC->fe_max  = (0x7f<<0) | (3000<<16);
    // FOC->fe_ramp = (10<<0)  | (2000<<16);
    // FOC->ctrl0  |= BIT28 | BIT29 | BIT30 | BIT31;

    //------- DRIVER配置 -----------
    // 打开Driver时钟
    DIGCTRL->clk_en0 |= BIT29;

    // 设置为FOC驱动模式
    DRV->cr |= BIT2;

    // 占空比来源设置为FOC
    DRV->cr |= BIT3;

    // 使能下溢事件更新占空比
    DRV->cr |= BIT4;

    // FOC模块使能
    DRV->cr |= BIT5;

    // 打开主输出使能
    DRV->out |= BIT7;

    // 打开输出使能
    DRV->cmr |= 0x3f;

    // 设置周期（16K）
    //DRV->arr = 3000;
    DRV->arr = 48000000.0/EPWM_FREQ -1; //PWM载波周期设置

    // 设置死区时间（1us）死区时间 = (DTR + 1)*T 例: DTR = 11，则死区时间 = 12*41.67ns = 500ns DTR值以时钟24MHz计算。
    DRV->dtr = 10; // 1us; //23--1us   11-0.5us

    // 打开基本计数器
    DRV->cr |= BIT7;

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

    //--------- main ------------------
    // Icnt = 0;
    // IuDC = Iu_d_r;
    // IvDC = Iv_d_r;
    // IwDC = Iw_d_r;
    // my_cnt = 1e4;

    MDU->z = (uint16_t)((1.0 / 700) * 32768);
    //stop = 0;
		
		
		#if (Config_Shunt_Mode == Single_Shunt)
		{
//			//选择电流采样方式
//			tmp = FOC->ctrl1;
//			tmp &= ~(0x3<<0);
//			// 0:单  1：双  3：三
//			tmp |= (0<<0);
//			FOC->ctrl1 = tmp;

			// 移相边界处理
			// 0：限制边界 1：下周期补偿
			FOC->ctrl1 |= 0x00 << 5;	
			
			// 窄脉冲限制
			FOC->duty_lim = 0;
			
			// Xus * 24  等待时间与窗口时间
			// 死区+等待时间 + 采样时间
			FOC->sample_time = (uint32_t)(3.2 * 24) | ((uint32_t)(2.7*24) << 8);

			UdcGain_Buff = FOC->wr_Udc_gain;
			// udc放大倍数 0:X1  1:X2  2:X4  7:X128
			UdcGain_Buff &= ~(0x03 <<0);
			UdcGain_Buff |= (0x00 <<0);
			UdcGain_Buff &= ~(0xffffU <<16);
			UdcGain_Buff |= (0x00 <<16);		
			FOC->wr_Udc_gain = UdcGain_Buff;
			
			//电压利用率
			FOC->wr_Udq_lim = ((uint32_t)(0.98 * 16384) <<16 ) | (uint32_t)(0.98 * 16384);	
		
			FOC->dc_Iuv = (11328) | ((11328) << 16);
			FOC->dc_Iw = (11328);	
			
//			FOC->dc_Iuv = (8100) | (8100 << 16);
//			FOC->dc_Iw = 8100;				
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
        if (uart0RX_stop)
        {
            // read RX
            uart0RX_stop = 0;
            uart0RX_strCnt = uart0RX_strCnt - 1;

            for (cnt = 0; cnt < uart0RX_strCnt; cnt++)
            {
                rx_cmd[cnt] = uart0RX_str[cnt];
            }
            rx_cmd[uart0RX_strCnt] = 0;

            // ---------- analze command --------------------
            // 读寄存器: [rd xx(hex)]
            if (strncmp(rx_cmd, "rd ", 3) == 0)
            {
                cmd_dat0_ui = uartrx_cmd_dat_hex(3, 4);
                tmp = *(((uint32_t *)FOC) + cmd_dat0_ui);
                printk("rd 0x%x: 0x%08x\r\n", cmd_dat0_ui, tmp);
            }

            // 把所有寄存器都打出来: [rd_all xx(hex) xx(hex)]
            // else if (strncmp(rx_cmd, "rd_all ", 7) == 0)
            // {
            //     cmd_dat0_ui = uartrx_cmd_dat_hex(7, 8);
            //     cmd_dat1_ui = uartrx_cmd_dat_hex(10, 11);

            //     for (cnt = cmd_dat0_ui; cnt <= cmd_dat1_ui; cnt++)
            //     {
            //         tmp = *(((uint32_t *)FOC) + cnt);
            //         printk("rd 0x%x: 0x%08x\r\n", cnt, tmp);
            //     }
            // }

            // 写寄存器: [wr xx(hex) xxxxxxxx(hex)]
            else if (strncmp(rx_cmd, "wr ", 3) == 0)
            {
                cmd_dat0_ui = uartrx_cmd_dat_hex(3, 4);
                cmd_dat1_ui = uartrx_cmd_dat_hex(6, 13);
                tmp = *(((uint32_t *)FOC) + cmd_dat0_ui);
                printk("rd 0x%x: 0x%08x\r\n", cmd_dat0_ui, tmp);
                *(((uint32_t *)FOC) + cmd_dat0_ui) = cmd_dat1_ui;
                printk("wr 0x%x: 0x%08x\r\n", cmd_dat0_ui, cmd_dat1_ui);
            }

            // // 读寄存器: [rv xx(hex)]
            // if (strncmp(rx_cmd, "rv ", 3) == 0)
            // {
            //     cmd_dat0_ui = uartrx_cmd_dat_hex(3, 4);
            //     tmp = *(((uint32_t *)DRV) + cmd_dat0_ui);
            //     printk("rv 0x%x: 0x%08x\r\n", cmd_dat0_ui, tmp);
            // }

            // // 写寄存器: [wv xx(hex) xxxxxxxx(hex)]
            // else if (strncmp(rx_cmd, "wv ", 3) == 0)
            // {
            //     cmd_dat0_ui = uartrx_cmd_dat_hex(3, 4);
            //     cmd_dat1_ui = uartrx_cmd_dat_hex(6, 13);
            //     tmp = *(((uint32_t *)DRV) + cmd_dat0_ui);
            //     printk("rd 0x%x: 0x%08x\r\n", cmd_dat0_ui, tmp);
            //     *(((uint32_t *)DRV) + cmd_dat0_ui) = cmd_dat1_ui;
            //     printk("wv 0x%x: 0x%08x\r\n", cmd_dat0_ui, cmd_dat1_ui);
            // }
            

            // FOC使能: [en x]
            // else if (strncmp(rx_cmd, "en ", 3) == 0)
            // {
            //     cmd_dat0_s = uartrx_cmd_dat(3, 3, 0);

            //     if (cmd_dat0_s == 0)
            //         DRV->cr &= ~BIT5;
            //     else
            //         DRV->cr |= BIT5;

            //     printk("en=%d\r\n", cmd_dat0_s);
            // }

            //电机转动方向: [dir x]
            // else if (strncmp(rx_cmd, "dir ", 4) == 0)
            // {
            //     cmd_dat0_s = uartrx_cmd_dat(4, 4, 0);

            //     if (cmd_dat0_s == 0)
            //         DRV->cr &= ~BIT6;
            //     else
            //         DRV->cr |= BIT6;

            //     printk("dir=%d\r\n", cmd_dat0_s);
            // }

            // 写寄存器: [cmd xx(hex) xxxxxxxx(hex)]
            else if (strncmp(rx_cmd, "cmd ", 4) == 0)
            {
                cmd_dat0_ui = uartrx_cmd_dat_hex(4, 5);
                cmd_dat1_ui = uartrx_cmd_dat_hex(7, 14);
                printk("cmd 0x%x: 0x%08x\r\n", cmd_dat0_ui, cmd_dat1_ui);
            }
			
            // FOC使能: [motor x] x=0/1/.../9
            else if (strncmp(rx_cmd, "motor ", 6) == 0)
            {
                cmd_dat0_s = uartrx_cmd_dat(6, 6, 0);

                if(cmd_dat0_s == 0)
                {
                    Flag.MC_RunStop = 0;
                }
                else if (cmd_dat0_s == 1)
                {
                    Flag.MC_RunStop = 1;
                }
                // else if (cmd_dat0_s == 2)
                // {

                // }
                // else if (cmd_dat0_s == 3)
                // {

                // }
                // else if (cmd_dat0_s == 4)//4--->5
                // {
                //     if(  Start_to_Sw_Flag == 0){ Start_to_Sw_Flag = 1U;}
                //     else{ Start_to_Sw_Flag = 0;}    
                // }
               
                // else if (cmd_dat0_s == 5)//5--->6
                // {
                //     if( Sw_to_Transition_Flag == 0){Sw_to_Transition_Flag = 1U;}
                //     else{Sw_to_Transition_Flag = 0;}          
                // }
                // else if (cmd_dat0_s == 6)//6--->7
                // {
                //     if(Transition_to_Run_Flag == 0U){Transition_to_Run_Flag = 1U;}
                //     else{Transition_to_Run_Flag = 0;}
                // }
                else 
				{
					 //do nothing
				}
                printk("motor =%d\r\n", cmd_dat0_s);
				
				
            }
            // 切换示波器: [probe x] x=0/1/.../9
            else if (strncmp(rx_cmd, "probe ", 6) == 0)
            {
                cmd_dat0_s = uartrx_cmd_dat(6, 6, 0);
                //切换示波器通道
                probe_switch = cmd_dat0_s;

                printk("probe =%d\r\n", cmd_dat0_s);
				
				
            }
            else //rx_cmd[uart0RX_strCnt]
            {
                //do nothing
            }

            //--------------------------------------------
            uart0RX_strCnt = 0; // RX lenth initial
        } // UART RX process

#endif
    } // while(1)
} // main
