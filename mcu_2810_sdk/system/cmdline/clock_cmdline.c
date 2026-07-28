#include "clock_cmdline.h"
#include "utility.h" 
#include "softinit.h"
//各功能的使能
#include "gpio_reg.h"
 

// 时钟使能位定义（严格按照你提供的表格）
#define CLK_SUPER_CLK_EN_Pos        (0U)
#define CLK_SUPER_CLK_EN_Msk        (1UL << CLK_SUPER_CLK_EN_Pos)

#define CLK_JLINK_CLK_EN_Pos        (1U)
#define CLK_JLINK_CLK_EN_Msk        (1UL << CLK_JLINK_CLK_EN_Pos)

#define CLK_AHBCLK_HALF_EN_Pos      (2U)
#define CLK_AHBCLK_HALF_EN_Msk      (1UL << CLK_AHBCLK_HALF_EN_Pos)

#define CLK_RCOCLK_EN_Pos           (3U)
#define CLK_RCOCLK_EN_Msk           (1UL << CLK_RCOCLK_EN_Pos)

#define CLK_I2C0_CLK_EN_Pos         (8U)
#define CLK_I2C0_CLK_EN_Msk         (1UL << CLK_I2C0_CLK_EN_Pos)

#define CLK_MDU_CLK_EN_Pos          (9U)
#define CLK_MDU_CLK_EN_Msk          (1UL << CLK_MDU_CLK_EN_Pos)

#define CLK_FOC_CLK_EN_Pos          (10U)
#define CLK_FOC_CLK_EN_Msk          (1UL << CLK_FOC_CLK_EN_Pos)

#define CLK_SPI_CLK_EN_Pos          (11U)
#define CLK_SPI_CLK_EN_Msk          (1UL << CLK_SPI_CLK_EN_Pos)

#define CLK_I2C1_CLK_EN_Pos         (12U)
#define CLK_I2C1_CLK_EN_Msk         (1UL << CLK_I2C1_CLK_EN_Pos)

#define CLK_UART0_CLK_SEL_Pos       (13U)
#define CLK_UART0_CLK_SEL_Msk       (1UL << CLK_UART0_CLK_SEL_Pos)

#define CLK_UART1_CLK_SEL_Pos       (14U)
#define CLK_UART1_CLK_SEL_Msk       (1UL << CLK_UART1_CLK_SEL_Pos)

#define CLK_WDT_CLK_EN_Pos          (16U)
#define CLK_WDT_CLK_EN_Msk          (1UL << CLK_WDT_CLK_EN_Pos)

#define CLK_RTC_CLK_EN_Pos          (17U)
#define CLK_RTC_CLK_EN_Msk          (1UL << CLK_RTC_CLK_EN_Pos)

#define CLK_UART0_CLK_EN_Pos        (18U)
#define CLK_UART0_CLK_EN_Msk        (1UL << CLK_UART0_CLK_EN_Pos)

#define CLK_UART1_CLK_EN_Pos        (19U)
#define CLK_UART1_CLK_EN_Msk        (1UL << CLK_UART1_CLK_EN_Pos)

#define CLK_ADC_CLK_EN_Pos          (20U)
#define CLK_ADC_CLK_EN_Msk          (1UL << CLK_ADC_CLK_EN_Pos)

#define CLK_TIM5_CLK_EN_Pos         (21U)
#define CLK_TIM5_CLK_EN_Msk         (1UL << CLK_TIM5_CLK_EN_Pos)

#define CLK_TIM6_CLK_EN_Pos         (22U)
#define CLK_TIM6_CLK_EN_Msk         (1UL << CLK_TIM6_CLK_EN_Pos)

#define CLK_TIM0_CLK_EN_Pos         (24U)
#define CLK_TIM0_CLK_EN_Msk         (1UL << CLK_TIM0_CLK_EN_Pos)

#define CLK_TIM1_CLK_EN_Pos         (25U)
#define CLK_TIM1_CLK_EN_Msk         (1UL << CLK_TIM1_CLK_EN_Pos)

#define CLK_TIM2_CLK_EN_Pos         (26U)
#define CLK_TIM2_CLK_EN_Msk         (1UL << CLK_TIM2_CLK_EN_Pos)

#define CLK_TIM3_CLK_EN_Pos         (27U)
#define CLK_TIM3_CLK_EN_Msk         (1UL << CLK_TIM3_CLK_EN_Pos)

#define CLK_TIM4_CLK_EN_Pos         (28U)
#define CLK_TIM4_CLK_EN_Msk         (1UL << CLK_TIM4_CLK_EN_Pos)

#define CLK_DRIVER_CLK_EN_Pos       (29U)
#define CLK_DRIVER_CLK_EN_Msk       (1UL << CLK_DRIVER_CLK_EN_Pos)

#define CLK_PID_CLK_EN_Pos          (30U)
#define CLK_PID_CLK_EN_Msk          (1UL << CLK_PID_CLK_EN_Pos)

#define CLK_CMP_CLK_EN_Pos          (31U)
#define CLK_CMP_CLK_EN_Msk          (1UL << CLK_CMP_CLK_EN_Pos)
 

int cmd_clock_test(int argc, char *argv[])
{
	uint8_t case2 = get_arg_ulong(argv[0]);
	uint8_t switch_clock = get_arg_ulong(argv[1]); 
	
	if(switch_clock == 1)
	{
		
		Clock_Open();
		INFO_e("Clock Open\r\n");
	}
	else
	{
		Clock_Close();
		INFO_e("Clock Close\r\n");
	}
	
	
	 
	return 0;	
}
 

 

//void Clock_Open(void)
//{
//	/*
//	foc
//	driver
//	adc
//	cmp
//	timer1 
//	timer2
//	tim3
//	tim4
//	tim0/5/6
//	mdu
//	pid
//	watchdog
//	rtc
//	i2c0/i2c1
//	uart0/uart1
//	spi
//	
//	*/
// 
// 
//}

void Clock_Close(void)
{
    // 关闭 所有外设时钟 
	
				uint32_t temp 	= (
        CLK_SUPER_CLK_EN_Msk      |
        CLK_JLINK_CLK_EN_Msk      |
        CLK_AHBCLK_HALF_EN_Msk    |
        CLK_RCOCLK_EN_Msk         |
        CLK_I2C0_CLK_EN_Msk       |
        CLK_MDU_CLK_EN_Msk        |
        CLK_FOC_CLK_EN_Msk        |
        CLK_SPI_CLK_EN_Msk        |
        CLK_I2C1_CLK_EN_Msk       |
        CLK_WDT_CLK_EN_Msk        |
        CLK_RTC_CLK_EN_Msk        |
        CLK_UART0_CLK_EN_Msk      |
        CLK_UART1_CLK_EN_Msk      |
        CLK_ADC_CLK_EN_Msk        |
        CLK_TIM5_CLK_EN_Msk       |
        CLK_TIM6_CLK_EN_Msk       |
        CLK_TIM0_CLK_EN_Msk       |
        CLK_TIM1_CLK_EN_Msk       |
        CLK_TIM2_CLK_EN_Msk       |
        CLK_TIM3_CLK_EN_Msk       |
        CLK_TIM4_CLK_EN_Msk       |
        CLK_DRIVER_CLK_EN_Msk     |
        CLK_PID_CLK_EN_Msk        |
        CLK_CMP_CLK_EN_Msk        
    );
		 DIGCTRL->clk_en0 &= ~temp;
		 
		Clock_Close_All();
}


void Clock_Open(void)
{
    // 打开 所有外设时钟 
		
			uint32_t temp = (
			CLK_SUPER_CLK_EN_Msk      |
			CLK_JLINK_CLK_EN_Msk      |
			CLK_AHBCLK_HALF_EN_Msk    |
			CLK_RCOCLK_EN_Msk         |
			CLK_I2C0_CLK_EN_Msk       |
			CLK_MDU_CLK_EN_Msk        |
			CLK_FOC_CLK_EN_Msk        |
			CLK_SPI_CLK_EN_Msk        |
			CLK_I2C1_CLK_EN_Msk       |
			CLK_WDT_CLK_EN_Msk        |
			CLK_RTC_CLK_EN_Msk        |
			CLK_UART0_CLK_EN_Msk      |
			CLK_UART1_CLK_EN_Msk      |
			CLK_ADC_CLK_EN_Msk        |
			CLK_TIM5_CLK_EN_Msk       |
			CLK_TIM6_CLK_EN_Msk       |
			CLK_TIM0_CLK_EN_Msk       |
			CLK_TIM1_CLK_EN_Msk       |
			CLK_TIM2_CLK_EN_Msk       |
			CLK_TIM3_CLK_EN_Msk       |
			CLK_TIM4_CLK_EN_Msk       |
			CLK_DRIVER_CLK_EN_Msk     |
			CLK_PID_CLK_EN_Msk        |
			CLK_CMP_CLK_EN_Msk        
	);
	DIGCTRL->clk_en0 |=temp;
//	Clock_Open_All();
}


void Clock_Close_All(void)
{
    // 全寄存器清 0（除了保留位也会清，根据需求使用）
    DIGCTRL->clk_en0 = 0x00000000;
}

void Clock_Open_All(void)
{
    // 全寄存器清 0（除了保留位也会清，根据需求使用）
    DIGCTRL->clk_en0 = 0xFFFFFFFF;
}




void module_enable(uint8_t cmd)
{
	if(cmd == 1)
	{
		IO_2810->EN.w = 0xFFFFFFFF; //open all
		INFO_e("module enable\r\n");
	}
	else
	{
		IO_2810->EN.w = ~0xFFFFFFFF; //open all
		INFO_e("module disable\r\n");
	}
	
}
