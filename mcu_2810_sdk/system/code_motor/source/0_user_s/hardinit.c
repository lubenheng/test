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
 ** \brief	ADC模块配置
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void ADC_Init(void)
{

  
}

/*****************************************************************************
 ** \brief	PGA0模块配置
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void  PGA0_Init(void)
{
 
	
}

/*****************************************************************************
 ** \brief	PGA1模块配置
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void PGA1_Init(void)
{
 
	
}

/*****************************************************************************
 ** \brief	PGA2模块配置
 **			
 ** \param [in] none
 ** \return  none
 ** \note	
*****************************************************************************/
void PGA2_Init(void)
{
 
	
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
  SysTick->LOAD  = (uint32_t)(MCU_CLK/1000 - 1UL);
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
    //0：RCO时钟(24MHz)  1：PLL时钟(48MHz)
    //DIGCTRL->clk_en0 &= ~BIT14; //0：RCO时钟(24MHz)
    DIGCTRL->clk_en0 |= BIT14;  //1：PLL时钟(48MHz)

    uart1Tx_init();
    //uart1Rx_init()
	
}

void PUM_init(void)
{
	 //-------------------------------------
    PMU->wr_key = 0x55aaaa55;
    //PMU->cfg_normal = 1 << 0 | 3 << 4 | 1 << 7 | 1 << 13 | 1 << 14 | 1 << 15 | 1 << 16;    //DRV2 V.144 
    PMU->cfg_normal = 0<<0 | 1<<2 | 1<<3 | 1<<4 | 0<<5 | 0<<6 | 1<<7 | 1<<8 | 1<<9 | 1<<10; //DRV3 V1.4
    //-------------------------------------
    QSPI->basic |= 1 << 11; // Cache en
}
void HALL_Init(void)
{
		uint32_t tmp;
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

}

void adc_init(void)
{
	uint32_t tmp;
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
}

void MUD_init(void)
{
		uint32_t tmp;
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
}

void EPWM_init(uint32_t arr,uint16_t deadtime)
{
	
	  DRV->arr = arr; //PWM载波周期设置
    DRV->dtr = deadtime; //死区时间
    DRV->cr = 0x98;
    //DRV->out |= 0x2a;
    DRV->out = 0;
    //printk("22\r\n");

}



void foc_init(void)
{
		uint32_t tmp; 
	
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

//	
//    printk("EK2 =%f\r\n", EK2);
//    printk("EK3 =%f\r\n", EK3);
//  
//	printk("MOTOR_RS =%f\r\n", MOTOR_RS);
//    printk("MOTOR_LS =%f\r\n", MOTOR_LS);
//    printk("%s,%s,%s\r\n", FW_VER, COMPILER_DATE, COMPILER_TIME);

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

}

void drive_init(void)
{
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

}

void sample_init(void)
{
	//			//选择电流采样方式
//			tmp = FOC->ctrl1;
//			tmp &= ~(0x3<<0);
//			// 0:单  1：双  3：三
//			tmp |= (0<<0);
//			FOC->ctrl1 = tmp;

//			// 移相边界处理
//			// 0：限制边界 1：下周期补偿
//			FOC->ctrl1 |= 0x00 << 5;	
//			
//			// 窄脉冲限制
//			FOC->duty_lim = 0;
//			
//			// Xus * 24  等待时间与窗口时间
//			// 死区+等待时间 + 采样时间
//			FOC->sample_time = (uint32_t)(3.2 * 24) | ((uint32_t)(2.7*24) << 8);

//			UdcGain_Buff = FOC->wr_Udc_gain;
//			// udc放大倍数 0:X1  1:X2  2:X4  7:X128
//			UdcGain_Buff &= ~(0x03 <<0);
//			UdcGain_Buff |= (0x00 <<0);
//			UdcGain_Buff &= ~(0xffffU <<16);
//			UdcGain_Buff |= (0x00 <<16);		
//			FOC->wr_Udc_gain = UdcGain_Buff;
//			
//			//电压利用率
//			FOC->wr_Udq_lim = ((uint32_t)(0.98 * 16384) <<16 ) | (uint32_t)(0.98 * 16384);	
//		
//			FOC->dc_Iuv = (11328) | ((11328) << 16);
//			FOC->dc_Iw = (11328);	
//			
//			FOC->dc_Iuv = (8100) | (8100 << 16);
//			FOC->dc_Iw = 8100;				



}
/******************************** END OF FILE *******************************/
