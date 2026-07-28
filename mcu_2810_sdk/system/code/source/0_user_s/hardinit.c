//==========================================================================//
/*****************************************************************************
*-----------------------------------------------------------------------------
* @file    hardinit.c
* @author  CMS Motor Control Team
* @version 第二代风机平台
* @date    2023年2月
* @brief
*---------------------------------------------------------------------------//
*****************************************************************************/
//==========================================================================//

//---------------------------------------------------------------------------/
//	include files
//---------------------------------------------------------------------------/
#include "mcu_driver.h"
#include "motor_driver.h"
#include "user.h"
#include "hardinit.h"
#include 	"main.h"
#include "gpio_reg.h"
//---------------------------------------------------------------------------/
//	Local pre-processor symbols/macros('#define')
//---------------------------------------------------------------------------/

//---------------------------------------------------------------------------/
//	Local variable  definitions
//---------------------------------------------------------------------------/

//---------------------------------------------------------------------------/
//	Global variable definitions(declared in header file with 'extern')
//---------------------------------------------------------------------------/

//---------------------------------------------------------------------------/
//	Local function prototypes('static')
//---------------------------------------------------------------------------/


/****************************************************************************/
/*	Function implementation - global ('extern') and local('static')
*****************************************************************************/

/*****************************************************************************
 ** \brief	系统时钟初始化
 **
 ** \param [in] none
 ** \return  none
 ** \note	64M主频
*****************************************************************************/
void SysClock_Init(void)
{
    //SystemCoreClockUpdate();
}

/*****************************************************************************
 ** \brief	GPIO初始化
 **
 ** \param [in] none
 ** \return  none
 ** \note
*****************************************************************************/
void PORT_Init(void)
{
//	GPIO_PinAFInConfig( P23CFG, IO_OUTCFG_P23_GPIO );

//	GPIO_PinAFOutConfig( P05CFG, IO_OUTCFG_P05_GPIO );
//  GPIO_Init( PORT0, PIN5, OUTPUT );


}

/*****************************************************************************
 ** \brief	EPWM模块初始化，双电阻方案
 **
 ** \param [in] none
 ** \return  none
 ** \note	除了刹车和重映射之外，其余代码不允许改动
*****************************************************************************/
void EPWM_R2_Init(void)
{

}

/*****************************************************************************
 ** \brief	EPWM模块初始化，单电阻方案
 **
 ** \param [in] none
 ** \return  none
 ** \note
*****************************************************************************/
void EPWM_R1_Init(void)
{


}
/*****************************************************************************
 ** \brief	timer3模块初始化
 **
 ** \param [in] none
 ** \return  none
 ** \note
*****************************************************************************/
void timer3_init(uint32_t reload)
{
    DIGCTRL->clk_en0 |= BIT27;
    TIM3->cr0 = 0x71; //0x71 -----3MHz CLK
    TIM3->dr = 1000;
    TIM3->arr = reload;
    TIM3->cr1 = 0x709;

    NVIC_EnableIRQ(tim3_irq);
    NVIC_SetPriority(tim3_irq, 5);
}
/*****************************************************************************
 ** \brief	ADC模块配置
 **
 ** \param [in] none
 ** \return  none
 ** \note
*****************************************************************************/
void ADC_Init(void)
{
    uint32_t tmp;
    //ADC外设时钟
    DIGCTRL->clk_en0 |= BIT20;

    //PGA引脚模拟功能
    //P15  A0_output  ADC_4
    IO->ana |= BIT13;

    ANACTRL->EN_b.ADC = 1;

    ANACTRL->EN_b.VREF = 1;
    ANACTRL->EN_b.VHALF=1;
    // delay_ms(200);

    //P16  A1P
    //P17  A1N
    //P30  A1_output  ADC0

    //P31  A2P
    //P32  A2N
    //P33  A2_output  ADC1

    // 打开CPU侧中断使能
    NVIC->ISER[0] = BIT17;

    // 中断使能
    ADCCTRL->cr |= BIT1;

    // 数据对齐方式（0：右对齐，1：次高位对齐）
    ADCCTRL->cr |= BIT2;

    // ADC使能
    ADCCTRL->cr |= BIT7;


    // ADC采样通道选择(Iu(CH0),Iv(CH1),Iw(CH4),Udc(CH2))
    //  IO->en2 &= ~BIT6;
    ADCCTRL->msk |= BIT0 | BIT1 | BIT2 | BIT4;

    // ADC group0的采样时长（至少0.125us）
    tmp = ADCCTRL->scyc;
    tmp &= ~(0xf << 0);
    tmp |= 7 << 0;
    ADCCTRL->scyc = tmp;

    // ADC group1的采样时长（至少0.125us）
    tmp = ADCCTRL->scyc;
    tmp &= ~(0xf << 4);
    tmp |= 7 << 4;
    ADCCTRL->scyc = tmp;


    NVIC_EnableIRQ(tim3_irq);
    NVIC_SetPriority(tim3_irq, 1);

}

/*****************************************************************************
 ** \brief	PGA模块配置
 **
 ** \param [in] none
 ** \return  none
 ** \note
*****************************************************************************/
void  PGA_init(void)
{
    //PGA外设时钟---无数字时钟

    //PGA引脚模拟功能
    //P13  A0P
    //P14  A0N
    //P15  A0_output
    IO->ana |= BIT11 | BIT12 | BIT13;
    //P16  A1P
    //P17  A1N
    //P30  A1_output

    //P31  A2P
    //P32  A2N
    //P33  A2_output

    // 使能PGA
    ANACTRL->EN_b.AMP0 = 1;
    ANACTRL->EN_b.AMP1 = 1;
    ANACTRL->EN_b.AMP2 = 1;

    // 使能PGA
    ANACTRL->EN_b.PGA0 = 1;
    ANACTRL->EN_b.PGA1 = 1;
    ANACTRL->EN_b.PGA2 = 1;

    //设置运放放大倍数为8倍
    ANACTRL->OTHERS_b.PGA0_GAIN = 2;
    ANACTRL->OTHERS_b.PGA1_GAIN = 2;
    ANACTRL->OTHERS_b.PGA2_GAIN = 2;

}
/*****************************************************************************
 ** \brief	Driver模块配置
 **
 ** \param [in] none
 ** \return  none
 ** \note
*****************************************************************************/

void Driver_init(void)
{
    // 1. 使能相关时钟
    DIGCTRL->clk_en0 |= BIT29;   // DRIVER 时钟
    // 2. 配置 GPIO 引脚为 PWM 输出 (假设 P2.0~P2.5)
    IO->en |= 0x3F;   // 使能 P20~P25 的 TIMER2 功能
    //IO->en |= 0xFFF;   // 使能 P20~P25 的 TIMER2 功能
    // 3. DRIVER 模块配置
    // 设置周期（16K）
    DRV->arr = 48000000.0 / EPWM_FREQ - 1; //PWM载波周期设置
    //  DRV->arr = 2400;               // 48MHz / 2400 = 20kHz（中央对齐）
    DRV->dtr = 11;                 // 死区时间 (11+1)/24MHz = 500ns
    DRV->cmr = 0x03F;             // 六路输出使能，极性默认0
    DRV->cr = BIT0   // OE: 输出使能
              | BIT2   // MESEL: FOC 驱动模式
              | BIT3   // OCS: 占空比来源为 FOC
              | BIT5   // FOCEN: FOC 模块使能
              | BIT7;  // DRVEN: 计数器使能
   // DRV->out = BIT7;               // MOE: 主输出使能
		Brige_Output_Off();
    /*
    0：不产生中断
    1：计数器向上计数时产生中断
    2：计数器向下计数时产生中断
    3：计数器无论向上还是向下，都产生中断
    */
    DRV->sr |= BIT1;
    NVIC_EnableIRQ(dcif_irq);
    NVIC_SetPriority(dcif_irq, 2);

}

/*****************************************************************************
 ** \brief	FOC配置
 **
 ** \param [in] none
 ** \return  none
 ** \note
*****************************************************************************/
extern  u16  OFST_A,OFST_B,OFST_C,OFST_CH_IBUS;
	
void FOC_Init(void)
{
    // angle estimation
    float pwm_freq, fe_real_base, fe_line_base, fe_rad_base;
    float Uadc_ref, U_att_factor, Iadc_ref, I_amp_factor, Rsample, Ld, Lq, Ts, Rs, Ki, Ku,   pub_dem, pub_factor, EK1, EK2, EK3, EK4;
    uint32_t      tmp;
    //-------- FOC配置 -----------
    // 打开FOC时钟
    DIGCTRL->clk_en0 |= BIT10;   // FOC 时钟
    DIGCTRL->clk_en0 |= BIT9;    // MDU 时钟（FOC 依赖）

    #if(UDC_CONVERT_CONFIG == UDC_CONVERT_ENABLE)
    FOC->ctrl0 &= ~BIT21;
    // Udc自动采样并使用
    // 4251 = 24/185*4095*8
    FOC->Udc = 18022; ////0x7000;
    FOC->ctrl0 &= ~BIT20;  //滑膜后再打开
    #else
    //	  FOC->Udc =28597;//24/11/2.5*32768
    FOC->Udc = 20560; //24/7.5/4.5*32768
    #endif
    // 配置直流偏置
      //  FOC->dc_Iuv = (OFST_A) | ((OFST_B)<<16);
    // 配置直流偏置
        FOC->dc_Iuv = (16176) | ((16672)<<16);
        FOC->dc_Iw  = 16383;
//   FOC->ctrl0 |= BIT0;
//   FOC->ctrl0 |= BIT1;
    // Ud/Uq自动  电流环时开启
    #if(IF_CONFIG == IF_ENABLE)
    FOC->ctrl0 &= ~BIT0;
    FOC->ctrl0 &= ~BIT1;
    FOC->Ud_lim = ((uint32_t)(-28000) << 16) | ((uint32_t)(18384) << 0); // Udmin | Udmax
    FOC->Uq_lim = ((uint32_t)(-28000) << 16) | ((uint32_t)(29000) << 0); // Uqmin | Uqmax
    //低位为电压利用率设置，范围为[0,1);高位为过调制利用率设置，最高位电压利用率的1.13倍。通常高位<低位
    FOC->wr_Udq_lim = ((uint32_t)(0.90 * 16384) << 16 ) | (uint32_t)(0.90 * 16384);
    #endif

    // 使用估计角
    // FOC->ctrl0 &= ~BIT2;
    // pll 滑膜锁相环PI
//  FOC->fe_pi = 0x4FFF | (0x2000 << 16); // PLL
    FOC->fe_pi = 0x4000 | (0x1000 << 16); // PLL  美的散装落地扇

    // 估计角度用的是来自哪里的反向电动势：滤波后
    // 注：用arctan方法时，必须开启本选项。因为它对电动势的准确度要求比PLL高很多，电动势不准，会导致电机不转，且电流过流
    FOC->ctrl0 |= BIT6;

    // 反向电动势滤波系数中用的速度来自：长滤波速度
    //FOC->ctrl0 |= BIT7;

    // 估计电流用的是来自哪里的反向电动势：滤波后
    FOC->ctrl0 |= BIT8;

    // APB观察的来自哪里的反向电动势：滤波后
    // FOC->ctrl0 |= BIT9;

    // 使能强制爬坡模式
    // FOC->ctrl0 |= BIT12;

    // 开启电流采样使能
    FOC->ctrl0 |= BIT14;

    // 估计角使能
    FOC->ctrl0 |= BIT15;

    #if(HIGH_SPEED_MODE_CONFIG == HIGH_SPEED_MODE_ENABLE)
    // 手动设置滤波使能
//		FOC->ctrl0 |= BIT16;
//		// 手动滤波系数为2^20
//		//FOC->E_coef_manual = 0x1fffffU;
//		// 手动滤波系数为2^22
//		FOC->E_coef_manual = 0x7fffffU;
    #else
    // 手动设置滤波使能
    FOC->ctrl0 |= BIT16;
    // 手动滤波系数为2^20
    FOC->E_coef_manual = 0x800000U;
    #endif

    // 选择五段式
    // FOC->ctrl0 &= ~BIT17;

    #if (Config_Shunt_Mode != Single_Shunt)
    {
        tmp = FOC->duty_lim;
        tmp &= ~(0x1fff << 16);
        // 占空比最大值为45.8us为三路采样提供时间
        tmp |= (1000 << 16);
        FOC->duty_lim = tmp;
        // 开启duty最大值限制使能
        FOC->ctrl0 |= BIT18;

        // 设置duty最小脉冲消除值
//    tmp = FOC->duty_lim;
//    tmp &= ~(0xff << 0);
//    // 占空比应不小于2us，否则将消除
//    tmp |= (48 << 0);
//    FOC->duty_lim = tmp;

        // 开启duty最小脉冲消除使能
        // FOC->ctrl0 |= BIT19;


        // Udc ADC只是开启采样
        // FOC->ctrl0 &= ~BIT20;
        // 让Udc采样并是否使用
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

        FOC->sample_time |= (6 << 24);
        FOC->sample_time |= (6 << 28);

        //电压利用率
        FOC->wr_Udq_lim = ((uint32_t)(0.88 * 16384) << 16 ) | (uint32_t)(0.88 * 16384);
    }
    #endif

    #if (Config_Shunt_Mode == Single_Shunt)
    //选择电流采样方式
    tmp = FOC->ctrl1;
    tmp &= ~(0x3 << 0);
    // 0:单 1：双 3：三
    tmp |= (0 << 0);
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
        tmp |= 0xa;  // 0us
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
//    tmp = FOC->sample_time;
//    tmp &= ~(0xff << 8);
//    tmp |= ((16 * 24) << 8);
//    FOC->sample_time = tmp;

    // 配置Id/Iq
    FOC->Id_req = ((0 + 0x0600) << 16) | (0x0000 << 0);// Iq | Id

    // 配置Kp/Ki
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
//    FOC->pi_shift |= (0xf<<4); //锁相环PI
//    FOC->pi_shift |= (0x1<<0); //电流环PI
    FOC->pi_shift |= (0x0 << 4); //锁相环PI
    FOC->pi_shift |= (0x1 << 0); //电流环PI
    #else
//    FOC->pi_shift |= (0x4<<4);//锁相环PI
    FOC->pi_shift |= (0x0 << 4); //锁相环PI
    #endif
    // 反向电动势滤波系数的系数: 2*pi*4K*4/60/16K (float)
    fe_rad_base = 2 * 3.1415926 * fe_line_base;
    //FOC->emf_coef = (uint32_t)(fe_rad_base * 32768);
//	FOC->ctrl0 &= ~BIT16;
    FOC->emf_coef = (uint32_t)(fe_rad_base * 8192);
    FOC->emf_coef |= ((uint16_t)Cfg_emf_coef_min << 16);
    // 滑膜系数设定
    FOC->smo_h = (uint32_t)((SMO_KE) | (SMO_KSLIDE << 16));

    // Uadc_ref: ADC采电压时用的参考(V, float)
    Uadc_ref = 4.5 ;

    #if(UDC_CONVERT_CONFIG == UDC_CONVERT_ENABLE)
    U_att_factor = 16;//37;
    #else
    // U_att_factor = 24.0/Uadc_ref;//37;
    U_att_factor = 7.5;//37;
    #endif

    //自动电压折算
    U_att_factor = 7.5;//37;

    // Iadc_ref: ADC采电流是用的参考(V, float)
    Iadc_ref = 4.5;

    // I_amp_factor: 电流放大倍数
    I_amp_factor = 12;

    // Rsample: 电流采样电阻(欧姆, float)
    Rsample = 20e-3;

    // Ld (H, float)
    Ld = MOTOR_LS; // 2.675e-3;

    // Lq (H, float)
    Lq = MOTOR_LS; // 2.675e-3;

    // Ts: PWM周期 (s, float)
    Ts = 1 / pwm_freq;

    // Rs (欧姆，float)
    Rs = MOTOR_RS; // 3.48;

    // 电流单位转换因子 (float)
    // Ki = Iadc_ref / (I_amp_factor * Rsample * 2.0);
    Ki = Iadc_ref / (I_amp_factor * Rsample * 1.0);

    // 电压单位转换因子 (float)
    #if(HIGH_SPEED_MODE_CONFIG == HIGH_SPEED_MODE_ENABLE)
//  Ku = 5.0;//Uadc_ref * U_att_factor;

    Ku = Uadc_ref * U_att_factor / 1.7321; //23

    #else
    Ku = Uadc_ref * U_att_factor / 1.7321;
    #endif
    // 公共分母 (float)
    pub_dem = Ld + Rs * Ts;

    // 公共因子 (float)
    pub_factor = Ts / pub_dem;

    // 计算估计器输出参数 (float)
    #if 0 //老版本滑膜
    EK1 = Ld / pub_dem * 32768.0;
    #else
    EK1 = -Ld / pub_dem * 32768.0;
    #endif
    EK2 =   pub_factor * Ku / Ki;//1.6205    乘0.9/0.8
    EK3 =   pub_factor * Ku / Ki; //0.3508   乘0.9/0.8
    EK4 = (Ld - Lq) * fe_rad_base / pub_dem;

    #if (UDC_CONVERT_CONFIG == UDC_CONVERT_ENABLE)
    FOC->ek12 = (((int16_t)(EK1)) & 0xFFFFU) | (((uint32_t)(EK2 * 8192 )) << 16);
    #else
    FOC->ek12 = (((int16_t)(EK1)) & 0xFFFFU) | (((uint32_t)(EK2 * 8192 )) << 16);
    #endif

    FOC->ek34 = (((uint32_t)(EK3 * 32768)) & 0xFFFFU) | (((uint32_t)(EK4 * 32768)) << 16);

    // 配置KFG
    FOC->kfg = 24e6 / (fe_real_base * 8);

	  FOC->Ud = (((0x1800)) << 16) | (0 << 0);  //Uq给定 VF

	// 配置加速度
#if (IF_CONFIG == IF_ENABLE)
		FOC->ramp1 = (MC_Start_Init_Speed*EPWM_FREQ>>16);//160; //+144;
#else
		FOC->ramp1 = 40;//160; //+144;
#endif

}



/*****************************************************************************
 ** \brief	DAC配置
 **
 ** \param [in] none
 ** \return  none
 ** \note
*****************************************************************************/
void DAC_Init(void)
{


}

/*****************************************************************************
 ** \brief	ACMP0模块配置
 **
 ** \param [in] none
 ** \return  none
 ** \note	用于EPWM刹车保护
*****************************************************************************/
void ACMP0_Init(void)
{


}

/*****************************************************************************
 ** \brief	ACMP1模块配置
 **
 ** \param [in] none
 ** \return  none
 ** \note	用于EPWM刹车保护
*****************************************************************************/
void ACMP1_Init(void)
{


}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  :CCP捕获模式0配置
* Description    :需要将相应的IO配置成CCP口，其它不需要改动
* Function Call  :程序初始化调用
* Input Paragram :无
* Return Value   :无
*-----------------------------------------------------------------------------
******************************************************************************/
void CCP_Capture_Init(void)
{

}

/*****************************************************************************
*-----------------------------------------------------------------------------
* Function Name  :CCP PWM模块配置
* Description    :需要将相应的IO配置成CCP口，其它不需要改动
* Function Call  :程序初始化调用
* Input Paragram :无
* Return Value   :无
*-----------------------------------------------------------------------------
******************************************************************************/
void CCP_PWM_Init(void)
{


}

/*****************************************************************************
 ** \brief	CCP_PWM_GPIO
 **
 ** \param [in] bMode  0:IO口配置为CCP模式  1:IO口配置为GPIO输出模式，并输出为低
 **           	bMode  0:GPIO输出模式，输出为低  1:GPIO输出模式，输出为高
 ** \return  none
 ** \note	FG信号处理
*****************************************************************************/
void CCP_PWM_GPIO(uint8_t bMode, uint8_t bLevle)
{
}

/*****************************************************************************
 ** \brief	硬件触发器配置
 **
 ** \param [in] none
 ** \return  none
 ** \note	无需改动
*****************************************************************************/
void HWDIV_Init(void)
{


}

/*****************************************************************************
 ** \brief	HWDIV_Div
 **			实现 "/" (取商)
 ** \param [in] x:被除数
 **				y:除数
 ** \return  none
 ** \note
*****************************************************************************/
int32_t HWDIV_Div(int32_t x, int32_t y)
{
    return 0;
}

/*****************************************************************************
 ** \brief	UART模块配置
 **
 ** \param [in] none
 ** \return  none
 ** \note	该配置主要用于调试，用户可配置为通信
*****************************************************************************/
void UART0_Init(void)
{
    // 选择UART0 时钟
    //0：RCO时钟(24MHz)  1：PLL时钟(48MHz)
    DIGCTRL->clk_en0 &= ~BIT13;   //0：RCO时钟(24MHz)
    //DIGCTRL->clk_en0 |= BIT13;  //1：PLL时钟(48MHz)


    uart0Tx_init();
    uart0Rx_init();

}

/*****************************************************************************
 ** \brief	SysTick_Init
 **
 ** \param [in] none
 ** \return  none
 ** \note	无需改动
*****************************************************************************/
void SysTick_Init(void)
{
    // 系统定时器--1ms时间配置
    SysTick->LOAD  = (uint32_t)(MCU_CLK / 1000 - 1UL);
    SysTick->VAL   = 0UL;
    SysTick->CTRL  = 0x00000005;

}

/*****************************************************************************
 ** \brief	TMR0模块配置
 **
 ** \param [in] none
 ** \return  none
 ** \note	用于hall平均角速度计算，不允许改动
*****************************************************************************/
void TMR0_Iint(void)
{


}

/*****************************************************************************
 ** \brief	TMR1模块配置
 **
 ** \param [in] none
 ** \return  none
 ** \note
*****************************************************************************/
void TMR1_Init(void)
{


}

/*****************************************************************************
 ** \brief	ADCLDO_Init
 **
 ** \param [in] none
 ** \return  none
 ** \note	无需改动
*****************************************************************************/
void ADCLDO_Init(void)
{



}

/*****************************************************************************
 ** \brief	系统的初始化配置，保护模块初始化，变量参数初始化等
 **
 ** \param [in] none
 ** \return  none
 ** \note		基本的模块初始化顺序不允许改动，允许增加其他模块功能
*****************************************************************************/
void System_Init(void)
{

    // 控制参数初始化
    MC_Para_Init();


}
void UART1_init(void)
{
    // 选择UART1 时钟
    uint32_t tmp;
    DIGCTRL->clk_en0 |= BIT14; // 1：PLL时钟(48MHz)
    // 开启时钟门控
    DIGCTRL->clk_en0 |= BIT19;
    // 配置uart设备
    tmp = UART1->thr;
    tmp &= ~(0xff << 0);
    tmp |= 11 | 1 << 4;
    UART1->thr = tmp;
    // 波特率300000
    UART1->baud = (uint32_t)((float)16.5);
    tmp = UART1->en;
    tmp |= 1;
    UART1->en |= tmp;
    IO_2810->EN.pad_txd2s_en = 1;
}


/******************************** END OF FILE *******************************/
