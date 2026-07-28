#include "interrupt_cmdline.h"
#include "utility.h"
#include "softinit.h"
#include "test.h"
#include "Common.h"
#include "gpio_cmdline.h"

/**
 * @file interrupt_cmdline.c
 * @brief 中断初始化辅助与示例 IRQ 处理函数（用于命令行测试）
 *
 * 本文件提供用于配置 GPIO、ADC、比较器、UVLO/TSD 以及定时器的中断
 * 初始化函数，同时包含示例中断处理函数，用于清除中断标志并做简单
 * 的调试动作。由 `cmd_interrupt_test` 调用以在目标平台上验证中断行为。
 *
 * 说明：
 * - GPIO_Pxx_interrupt_init(): 配置指定引脚为输入/输出，并设置中断类型/极性或
 *   P4 的中断映射位。函数会调用 NVIC_EnableIRQ 来允许对应的中断向量。
 * - ADC_interrupt_init(): 使能 ADC 时钟、参考电压、通道掩码，并配置采样周期，
 *   最后设置启动位触发 ADC。会设置 NVIC 的中断使能位（示例为 BIT17）。
 * - COMPx_interrupt_init(): 使能比较器模块时钟，选择比较器工作模式并使能比较器，
 *   同时注册 NVIC 中断向量。CMP_* 寄存器用于控制比较器行为与中断清除。
 * - timerN_init(): 配置对应定时器的时钟/寄存器/重载值，并通过 NVIC 使能中断，
 *   适合在命令行测试中快速验证定时中断触发和处理中断流水线。
 *
 * 注意：这些初始化函数为示例用途，具体寄存器位和中断号请根据芯片手册核对。
 */

/* Simple counter used by timer/interrupt handlers for demo purposes */
int counter = 0;

//int cmd_gpio_toggle_test(int argc, char *argv[])
//{
//	if(argc != 3)
//	{
//		INFO_e("input error \n");
//		return -1;
//	}
//	uint8_t case2 = get_arg_ulong(argv[0]);
//	gpio_port_e GPIO_PORT = (gpio_port_e) get_arg_ulong(argv[1]);
//	gpio_pin_e GPIO_PIN = (gpio_pin_e) get_arg_ulong(argv[2]);
//	GPIO_Output_Init(GPIO_PORT,(gpio_pin_e)GPIO_PIN);
//
//
//	for(uint16_t i = 0; i < 100; i++)
//	{
//		gpio_toggle(GPIO_PORT, GPIO_PIN);

//		delay_ms(1);
//	}
//	INFO_e("GPIO OK\r\n");
//	return 0;
//}


/*
 * 初始化 GPIO P0.5 为输入并使能其中断（int0）。
 * 详细说明：
 * - 将 P0.5 配置为数字输入（GPIO_INput_Init）。
 * - 将 P3.2 配置为输出，用于在中断发生时做指示/示波器探测。
 * - 通过设置 GPIO->int_type 与 GPIO->int_pol 指定触发类型（本例设置为边沿且上升沿触发）。
 * - 调用 NVIC_EnableIRQ/SetPriority 注册并设置中断优先级。
 * 参数: 无
 * 返回: 无
 * 副作用: 修改 GPIO 寄存器和 NVIC 中断使能位。
 */
void GPIO_P05_interrupt_init(void)
{
    GPIO_INput_Init(GPIO_PORT_P0, GPIO_PIN_5);
    GPIO_Output_Init(GPIO_PORT_P3, GPIO_PIN_2);
    GPIO->int_type |= BIT5;//边沿触发
    GPIO->int_pol |= BIT5; //上升沿触发
    //INFO_e("------INTERRUPT--int0_Handler-tset---\r\n");
    NVIC_EnableIRQ(int0_irq);
    NVIC_SetPriority(int0_irq, 1);
}

/*
 * 初始化 GPIO P1.5 为输入并使能其中断（int1）。
 * 详细说明与注意事项同 P0.5 初始化，使用 BIT13 表示 P1.5 对应的中断位。
 */
void GPIO_P15_interrupt_init(void)
{

    GPIO_INput_Init(GPIO_PORT_P1, GPIO_PIN_5);
    GPIO_Output_Init(GPIO_PORT_P3, GPIO_PIN_2);
    GPIO->int_type |= BIT13;//边沿触发
    GPIO->int_pol |= BIT13; //上升沿触发
    //INFO_e("------INTERRUPT--int1_Handler-tset--\r\n");
    NVIC_EnableIRQ(int1_irq);
    NVIC_SetPriority(int1_irq, 1);

}
/*
 * 初始化 GPIO P2.5 为输入并使能其中断（int2）。
 * 注：本函数设置 GPIO->int_type/ int_pol 的 BIT21，对应 P2.5 的中断映射。
 */
void GPIO_P25_interrupt_init(void)
{

    GPIO_INput_Init(GPIO_PORT_P2, GPIO_PIN_5);
    GPIO_Output_Init(GPIO_PORT_P3, GPIO_PIN_2);
    GPIO->int_type |= BIT21;//边沿触发
    GPIO->int_pol |= BIT21; //上升沿触发
    //INFO_e("------INTERRUPT--int2_Handler-tset---\r\n");
    NVIC_EnableIRQ(int2_irq);
    NVIC_SetPriority(int2_irq, 1);
}

/*
 * 初始化 GPIO P3.5 为输入并使能其中断（int3）。
 * 注：本函数设置 GPIO->int_type/ int_pol 的 BIT29，对应 P3.5 的中断映射。
 */
void GPIO_P35_interrupt_init(void)
{

    GPIO_INput_Init(GPIO_PORT_P3, GPIO_PIN_5);
    GPIO_Output_Init(GPIO_PORT_P3, GPIO_PIN_2);
    GPIO->int_type |= BIT29;//边沿触发
    GPIO->int_pol |= BIT29; //上升沿触发
    //INFO_e("------INTERRUPT--int3_Handler-tset---\r\n");
    NVIC_EnableIRQ(int3_irq);
    NVIC_SetPriority(int3_irq, 1);
}
/*
 * 初始化 GPIO P4.5 为输入并使能其中断（int4）。
 * 说明：P4 的中断映射略有不同，通过 GPIO->p4_int_set 指定映射位（本例设置 BIT21 | BIT29）。
 * P4 中断清除使用 GPIO->p4_int_clr。
 */
void GPIO_P45_interrupt_init(void)
{

    GPIO_INput_Init(GPIO_PORT_P4, GPIO_PIN_5);
    GPIO_Output_Init(GPIO_PORT_P3, GPIO_PIN_2);
    GPIO->p4_int_set |= BIT21 | BIT29;//上升沿触发

    //INFO_e("------INTERRUPT--int4_Handler-tset---\r\n");
    NVIC_EnableIRQ(int4_irq);
    NVIC_SetPriority(int4_irq, 1);
}


void PIN_interrupt_init(void)
{
		GPIO_INput_Init(GPIO_PORT_P0, GPIO_PIN_5);
		IO->msk&=~BIT5;
		NVIC_EnableIRQ(pin_irq);
		
		__WFI();
	
}


/*
 * 初始化 ADC，配置并使能 ADC 中断及基本参数
 * 步骤说明：
 * 1. 通过 DIGCTRL->clk_en0 使能 ADC 外设时钟（BIT20）。
 * 2. 将所用模拟引脚设置为模拟功能（IO->ana 对应位，例如 P1.5 -> BIT13）。
 * 3. 使能 ADC 与 VREF（ANACTRL->EN_b.ADC/VREF）。
 * 4. 在 NVIC 中使能 ADC 中断位（示例使用 NVIC->ISER[0] = BIT17，请根据向量表核对）。
 * 5. 在 ADCCTRL->cr 中设置必要控制位：中断使能、数据对齐、ADC 使能等（BIT1/BIT2/BIT7）。
 * 6. 通过 ADCCTRL->msk 使能要采样的通道掩码（例：BIT0/1/2）。
 * 7. 配置采样周期 ADCCTRL->scyc（本例设置为 7，代表最少采样窗口）。
 * 8. 最后通过 ADCCTRL->cr 的触发位（BIT6）启动 ADC。
 *
 * 注意：不同硬件版本的寄存器位定义可能不同，实际使用时请以芯片手册为准。
 */
void ADC_interrupt_init(void)
{
    uint32_t tmp;
    /* ADC 外设时钟 */
    DIGCTRL->clk_en0 |= BIT20;
    /* PGA/模拟引脚使能（示例使用 P1.5 对应 BIT13） */
    IO->ana |= BIT13;

    ANACTRL->EN_b.ADC = 1;
    ANACTRL->EN_b.VREF = 1;
    /* 在 NVIC 中使能 ADC 中断位（示例为 BIT17） */
    NVIC->ISER[0] = BIT17;
    /* 配置 ADC 控制寄存器：中断/对齐/使能 */
    ADCCTRL->cr |= BIT1;
    ADCCTRL->cr |= BIT2;
    ADCCTRL->cr |= BIT7;
    /* 使能采样通道掩码 */
    ADCCTRL->msk |= BIT0 | BIT1 | BIT2 ;

    /* 配置 group0 采样时长（确保满足最小采样时间） */
    tmp = ADCCTRL->scyc;
    tmp &= ~(0xf << 0);
    tmp |= 7 << 0;
    ADCCTRL->scyc = tmp;

    INFO_e("------INTERRUPT--ADC_Handler-tset---\r\n");

    /* 触发/启动 ADC */
    ADCCTRL->cr |= BIT6;
}
/*
 * 初始化比较器0并使能其中断
 * 说明：
 * - 配置比较器输入所用的 GPIO 引脚为模拟/输入模式。
 * - 使能比较器模块时钟（DIGCTRL->clk_en0）。
 * - 设置比较器控制寄存器以使能中断/选择工作模式（CMP->CMP_CR0/CMP_CR2）。
 * - 在 NVIC 中使能对应比较器中断向量（cmp0_irq）。
 */
void COMP0_interrupt_init(void)
{

    GPIO_INput_Init(GPIO_PORT_P2, GPIO_PIN_0);
    GPIO_INput_Init(GPIO_PORT_P2, GPIO_PIN_1);

    DIGCTRL->clk_en0 |= BIT31;//使能模块时钟
    CMP->CMP_CR0 = 1; //上升沿产生中断

    CMP->CMP_CR2_b.CMP0SEL = 1; //CMP工作模式
	CMP->CMP_CR2_b.CMP0EN = 1; //CMP使能

    INFO_e("------INTERRUPT--COMP0_Handler-tset---\r\n");
    NVIC_EnableIRQ(cmp0_irq);
    NVIC_SetPriority(cmp0_irq, 1);
}
/*
 * 初始化比较器1并使能其中断
 * 说明与 COMP0 类似，注意 CMP 寄存器的通道选择字段可能不同（CMP0SEL 等）。
 */
void COMP1_interrupt_init(void)
{

    GPIO_INput_Init(GPIO_PORT_P2, GPIO_PIN_2);
    GPIO_INput_Init(GPIO_PORT_P2, GPIO_PIN_3);
    DIGCTRL->clk_en0 |= BIT31;//使能模块时钟
    CMP->CMP_CR0_b.CMP1IM = 1; //上升沿产生中断

    CMP->CMP_CR2_b.CMP0SEL = 2; //CMP工作模式
		CMP->CMP_CR2_b.CMP0EN = 1; //CMP使能
	
    INFO_e("------INTERRUPT--COMP1_Handler-tset---\r\n");
    NVIC_EnableIRQ(cmp1_irq);
    NVIC_SetPriority(cmp1_irq, 1);
}
/*
 * 初始化比较器2并使能其中断
 * 请根据具体比较器编号调整 CMP_CR2 的选择位，以及对应的 NVIC 向量号。
 */
void COMP2_interrupt_init(void)
{
    GPIO_INput_Init(GPIO_PORT_P2, GPIO_PIN_4);
    GPIO_INput_Init(GPIO_PORT_P2, GPIO_PIN_5);
    DIGCTRL->clk_en0 |= BIT31;//使能模块时钟
    CMP->CMP_CR0_b.CMP2IM = 1; //上升沿产生中断

    
    CMP->CMP_CR2_b.CMP0SEL = 3; //CMP工作模式
		CMP->CMP_CR2_b.CMP0EN = 1; //CMP使能

    INFO_e("------INTERRUPT--COMP2_Handler-tset---\r\n");
    NVIC_EnableIRQ(cmp2_irq);
    NVIC_SetPriority(cmp2_irq, 1);
}
/*
 * 初始化比较器3并使能其中断
 */
void COMP3_interrupt_init(void)
{

    GPIO_INput_Init(GPIO_PORT_P2, GPIO_PIN_6);
    GPIO_INput_Init(GPIO_PORT_P2, GPIO_PIN_7);	

    DIGCTRL->clk_en0 |= BIT31;//使能模块时钟
    
	  CMP->CMP_CR0_b.CMP3IM = 1; //上升沿产生中断
		CMP->CMP_CR1_b.CMP3EN = 1;//CMP使能

    INFO_e("------INTERRUPT--COMP3_Handler-tset---\r\n");
    NVIC_EnableIRQ(cmp3_irq);
    NVIC_SetPriority(cmp3_irq, 1);
}
/*
 * 初始化比较器4并使能其中断
 * 对于某些比较器（如 CMP4），事件可能映射到 P4 的中断线路，
 * 因此需要设置 GPIO->p4_int_set/p4_int_clr 进行事件映射和清除。
 */
void COMP4_interrupt_init(void)
{

    GPIO_INput_Init(GPIO_PORT_P1, GPIO_PIN_5);
    GPIO_INput_Init(GPIO_PORT_P3, GPIO_PIN_7);	

    DIGCTRL->clk_en0 |= BIT31;//使能模块时钟
    
	  GPIO->p4_int_set |= BIT7 | BIT23;//上升沿触发
		
		
//		CMP->EVT_FILT_b.EFSRC=1;
//	
    CMP->CMP_CR2_b.CMP4EN = 1;

    INFO_e("------INTERRUPT--COMP4_Handler-tset---\r\n");
    NVIC_EnableIRQ(cmp4_irq);
    NVIC_SetPriority(cmp4_irq, 1);
}
/* Enable UVLO (undervoltage lockout) interrupt and NVIC entry */
void RTC_interrupt_init()
{
	 DIGCTRL->clk_en0 |= BIT17;//使能模块时钟
	 RTC->En|=BIT0|BIT1;
	
	 RTC->int_val=0x5000;
	 NVIC_EnableIRQ(rtc_irq);
	
    INFO_e("------INTERRUPT--RTC_Handler-tset---\r\n");
}
/* Enable UVLO (undervoltage lockout) interrupt and NVIC entry */
void UVLO_interrupt_init()
{
		ANACTRL->INTR_EN|=0X0c;
		ANACTRL->INTR_EN|=BIT17;
	
	
		
	  NVIC_EnableIRQ(uvlo_irq);
	
    INFO_e("------INTERRUPT--UVLO_Handler-tset---\r\n");
}

void fault_interrupt_init()
{

    INFO_e("------INTERRUPT--fault_Handler-tset---\r\n");
	 *(volatile unsigned long *)0x40004001=0x1;
	 *(volatile unsigned long *)0x40004003=0x2;

}

/* Enable TSD (thermal shutdown) interrupt and NVIC entry */
void TSD_interrupt_init()
{
		ANACTRL->INTR_EN|=0X03;
		ANACTRL->INTR_EN|=BIT16;
	
		CMP->TSD_CR=0X80;
		NVIC_EnableIRQ(tsd_irq);
    INFO_e("------INTERRUPT--TSD_Handler-tset---\r\n");
}
/*
 * 配置定时器0的重载值并使能其中断
 * 参数：
 *  - reload: 定时器重载/周期值（按定时器时钟计数）
 * 说明：该函数将 P3.2 作为调试输出，并使能定时器时钟（DIGCTRL->clk_en0），
 * 设置 TIM0->reload_val 并将定时器使能位写入 TIM0->en。
 */
void timer0_interrupt_init(uint32_t reload)
{
    GPIO_Output_Init(GPIO_PORT_P3, GPIO_PIN_2);
    DIGCTRL->clk_en0 |= BIT24;//使能模块时钟
    NVIC_EnableIRQ(tim0_irq);
    TIM0->reload_val = reload;
    TIM0->en         = 3;
    INFO_e("------INTERRUPT--timer0_Handler-tset---\r\n");
}

/*
 * 配置定时器1并使能其中断（设置适中优先级）
 * 参数：reload - 定时器周期值
 * 说明：定时器1 使用 TIM1 寄存器组，通过 TIM1->ier 使能中断源，
 * 并在 NVIC 中将 tim1_irq 置为优先级 1。
 */
void timer1_interrupt_init(uint32_t reload)
{

    GPIO_Output_Init(GPIO_PORT_P3, GPIO_PIN_2);
    DIGCTRL->clk_en0 |= BIT25;//使能模块时钟

    TIM1->cr0 |= BIT1 | BIT8;
    TIM1->cr3 |= 0x3;
    TIM1->ier |= BIT4;

    TIM1->barr = reload;

    INFO_e("------INTERRUPT--timer1_Handler-tset---\r\n");

    NVIC_EnableIRQ(tim1_irq);
    NVIC_SetPriority(tim1_irq, 1);
}
/*
 * 配置定时器2寄存器并使能其中断
 * 说明：通过写 TIM2_CR0/CR1/DR/ARR 等寄存器设置时钟分频、计数器初值和自动重载。
 */
void timer2_interrupt_init(uint32_t reload)
{

    GPIO_Output_Init(GPIO_PORT_P3, GPIO_PIN_2);
    DIGCTRL->clk_en0 |= BIT26;//使能模块时钟
    TIM2->TIM2_CR0 = 0x71;

    TIM2->TIM2_CR1 = 0x709;
    TIM2->TIM2_DR = 400;
    TIM2->TIM2_ARR = reload;


    INFO_e("------INTERRUPT--timer2_Handler-tset---\r\n");

    NVIC_EnableIRQ(tim2_irq);
    NVIC_SetPriority(tim2_irq, 1);
}
/*
 * 配置定时器3并使能其中断
 * 备注：TIM3->cr0 = 0x71 表示特定时钟设置（例如 3MHz），请参考芯片手册。
 */
void timer3_interrupt_init(uint32_t reload)
{
    GPIO_Output_Init(GPIO_PORT_P3, GPIO_PIN_2);
    DIGCTRL->clk_en0 |= BIT27;//使能模块时钟
    TIM3->cr0 = 0x71; //0x71 -----3MHz CLK
    TIM3->dr = 1000;
    TIM3->arr = reload;
    TIM3->cr1 = 0x709;

    INFO_e("------INTERRUPT--timer3_Handler-tset---\r\n");


    NVIC_EnableIRQ(tim3_irq);
    NVIC_SetPriority(tim3_irq, 1);
}

/*
 * 配置定时器4并使能其中断（示例约 43ms）
 * 该函数示例性地将 TIM4 寄存器设置为特定计数周期，
 * 实际周期依赖于定时器时钟与重载值。
 */
void timer4_interrupt_init(uint32_t reload) // 43ms
{
    GPIO_Output_Init(GPIO_PORT_P3, GPIO_PIN_2);
    DIGCTRL->clk_en0 |= BIT28;//使能模块时钟
    TIM4->cr0  = 0x71;
    TIM4->dr  =  1000;
    TIM4->arr  = reload;
    TIM4->cr1  = 0x709;

    INFO_e("------INTERRUPT--timer4_Handler-tset---\r\n");

    NVIC_EnableIRQ(tim4_irq);
    NVIC_SetPriority(tim4_irq, 1);

}
/*
 * 配置定时器5并使能其中断
 */
void timer5_interrupt_init(uint32_t reload)
{
    GPIO_Output_Init(GPIO_PORT_P3, GPIO_PIN_2);
    DIGCTRL->clk_en0 |= BIT21;//使能模块时钟
    NVIC_EnableIRQ(tim5_irq);
    TIM5->reload_val = reload;
    TIM5->en         = 3;
    INFO_e("------INTERRUPT--timer5_Handler-tset---\r\n");

}
/*
 * 配置定时器6并使能其中断
 */
void timer6_interrupt_init(uint32_t reload)
{
    GPIO_Output_Init(GPIO_PORT_P3, GPIO_PIN_2);
    DIGCTRL->clk_en0 |= BIT22;//使能模块时钟
    NVIC_EnableIRQ(tim6_irq);
    TIM6->reload_val = reload;
    TIM6->en         = 3;
    INFO_e("------INTERRUPT--timer6_Handler-tset---\r\n");
}

/*
 * 中断测试命令处理函数。
 * argv[0] : test_case（占位，不使用）
 * argv[1] : 中断选择器（1..32），用于使能/配置对应的中断源
 */
int cmd_interrupt_test(int argc, char *argv[])
{
    uint32_t test_case = get_arg_ulong(argv[0]);

    uint32_t para1 =  get_arg_ulong(argv[1]);

    /* Enable/configure the selected interrupt based on para1 */
    switch (para1)
    {
        case 1://int0_Handler
            GPIO_P05_interrupt_init();
            break;

        case 2://int1_Handler
            GPIO_P15_interrupt_init();
            break;

        case 3://int2_Handler
            GPIO_P25_interrupt_init();
            break;

        case 4://int3_Handler
            GPIO_P35_interrupt_init();
            break;

        case 5://int4_Handler
            GPIO_P45_interrupt_init();
            break;

        case 6://pin_Handler
						PIN_interrupt_init();
            break;

        case 7://rtc_Handler
						RTC_interrupt_init();
            break;

        case 8://uart0_Handler

            break;

        case 9://uart1_Handler

            break;

        case 10://i2c0_Handler

            break;

        case 11://tim0_Handler
            timer0_interrupt_init(3000);
            break;

        case 12://tim1_Handler
            timer1_interrupt_init(3000);
            break;

        case 13://tim2_Handler
            timer2_interrupt_init(3000);
            break;

        case 14://tim3_Handler
            timer3_interrupt_init(3000);
            break;

        case 15://tim4_Handler
            timer4_interrupt_init(3000);
            break;

        case 16://fgif_Handler
						
            break;

        case 17://dcif_Handler

            break;

        case 18://adc_Handler
            ADC_interrupt_init();
            break;

        case 19://cmp0if_Handler
            COMP0_interrupt_init();
            break;

        case 20://cmp1if_Handler
            COMP1_interrupt_init();
            break;

        case 21://cmp2if_Handler
            COMP2_interrupt_init();
            break;

        case 22://cmp3if_Handler
						COMP3_interrupt_init();
            break;

        case 23://cmp4if_Handler
						COMP4_interrupt_init();
            break;

        case 24://tim5_Handler
            timer5_interrupt_init(3000);
            break;

        case 25://spi_Handler
						
            break;

        case 26://tsd_Handler
						TSD_interrupt_init();
            break;

        case 27://uvlo_Handler
						UVLO_interrupt_init();
            break;

        case 28://fault_Handler
						fault_interrupt_init();
            break;

        case 29://i2c1_Handler

            break;

        case 30://foc_Handler
						
            break;

        case 31://swd_Handler

            break;

        case 32://tim6_Handler
            timer6_interrupt_init(3000);
            break;

        default:
            INFO_e("input num is error!!\n");
            break;
    }

    // INFO_e("PASS\n");
    return 0;
}





#ifdef interrupt_TEST
/**
 * @brief 外部中断0 服务函数
 */
void int0_Handler(void)
{
    /* TODO: 处理外部中断0事件 */
    if(GPIO->int_clr & BIT5)
    {
        GPIO->out |= BIT26;
        GPIO->int_clr &= ~BIT5;
        GPIO->out &= ~BIT26;
    }

}

/**
 * @brief 外部中断1 服务函数
 */
void int1_Handler(void)
{
    /* TODO: 处理外部中断1事件 */
    if(GPIO->int_clr & BIT13)
    {
        GPIO->out |= BIT26;
        GPIO->int_clr &= ~BIT13;
        GPIO->out &= ~BIT26;
    }
}

/**
 * @brief 外部中断2 服务函数
 */
void int2_Handler(void)
{
    /* TODO: 处理外部中断2事件 */
    if(GPIO->int_clr & BIT21)
    {
        GPIO->out |= BIT26;
        GPIO->int_clr &= ~BIT21;
        GPIO->out &= ~BIT26;
    }
}

/**
 * @brief 外部中断3 服务函数
 */
void int3_Handler(void)
{
    /* TODO: 处理外部中断3事件 */
    if(GPIO->int_clr & BIT29)
    {
        GPIO->out |= BIT26;
        GPIO->int_clr &= ~BIT29;
        GPIO->out &= ~BIT26;
    }
}

/**
 * @brief 外部中断4 服务函数
 */
void int4_Handler(void)
{
    /* TODO: 处理外部中断4事件 */
    if(GPIO->p4_int_clr & BIT5)
    {
        GPIO->out |= BIT26;
        GPIO->p4_int_clr &= ~BIT5;
        GPIO->out &= ~BIT26;
    }
}

/**
 * @brief 引脚唤醒 / 电平变化中断
 */
void pin_Handler(void)
{
    /* TODO: 处理引脚唤醒或键盘中断 */
	 INFO_e("------INTERRUPT--pin_INTERRUPT-Trigger---\r\n");
	
}

/**
 * @brief 实时时钟 (RTC) 中断
 */
void rtc_Handler(void)
{
    /* TODO: 处理RTC闹钟或秒中断 */
		
	 RTC->wo|=BIT0;
	 INFO_e("------INTERRUPT--RTC_INTERRUPT-Trigger---\r\n");
    counter++;

    if(counter > 100)
    {
        counter = 0;
        NVIC_DisableIRQ(rtc_irq);
    }
	
}

/**
 * @brief 串口0 中断
 */
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



/**
 * @brief 定时器0 中断
 */
void tim0_Handler(void)
{
    /* TODO: 处理定时器0中断 */
    GPIO->out |= BIT26;
    TIM0->int_clr = 1;
    GPIO->out &= ~BIT26;
    counter++;

    if(counter > 100)
    {
        counter = 0;
        NVIC_DisableIRQ(tim0_irq);
    }

}

/**
 * @brief 定时器1 中断
 */


void tim1_Handler(void)
{
    /* TODO: 处理定时器1中断 */
    if(TIM1->sr & BIT4)
    {
        GPIO->out |= BIT26;
        TIM1->sr &= ~BIT4;
    }

    GPIO->out &= ~BIT26;
    counter++;

    if(counter > 100)
    {
        counter = 0;
        NVIC_DisableIRQ(tim1_irq);
    }

}


/**
 * @brief 定时器2 中断
 */

void tim2_Handler(void)
{

    /* TODO: 处理定时器2中断 */
    if(TIM2->TIM2_CR1 & 0x20)
    {
        GPIO->out |= BIT26;
        TIM2->TIM2_CR1 &= ~BIT5;
    }

    GPIO->out &= ~BIT26;
    counter++;

    if(counter > 100)
    {
        counter = 0;
        NVIC_DisableIRQ(tim2_irq);
    }

}

/**
 * @brief 定时器3 中断
 */
void tim3_Handler(void)
{
    /* TODO: 处理定时器3中断 */
    if(TIM3->cr1 & 0x20)
    {
        GPIO->out |= BIT26;
        TIM3->cr1 &= ~BIT5;
    }

    GPIO->out &= ~BIT26;
    counter++;

    if(counter > 100)
    {
        counter = 0;
        NVIC_DisableIRQ(tim3_irq);
    }
}

/**
 * @brief 定时器4 中断
 */
void tim4_Handler(void)
{
    /* TODO: 处理定时器4中断 */
    GPIO->out |= BIT26;

    if(TIM4->cr1 & 0x20)
    {
        TIM4->cr1 &= ~BIT5;
    }

    GPIO->out &= ~BIT26;
    counter++;

    if(counter > 100)
    {
        counter = 0;
        NVIC_DisableIRQ(tim4_irq);
    }
}

/**
 * @brief 电机FG信号 (速度反馈) 中断
 */
void fgif_Handler(void)
{
    /* TODO: 处理FG测速中断 */

	
}

/**
 * @brief 直流侧保护中断 (如直流过流/过压)
 */
void dcif_Handler(void)
{
    /* TODO: 处理直流侧中断 */

}

/**
 * @brief ADC转换完成中断
 */
void adc_Handler(void)
{
    /* TODO: 处理ADC中断，读取采样结果 */

    // 清除中断
    ADCCTRL->cr &= ~BIT0;
    INFO_e("------INTERRUPT--ADC_INTERRUPT-Trigger---\r\n");
}

/**
 * @brief 比较器0 中断
 */
void cmp0if_Handler(void)
{
    /* TODO: 处理比较器0事件 */
    if(CMP->CMP_SR_b.CMP0IF & 1)
    {

        INFO_e("------INTERRUPT--CMP0_INTERRUPT-Trigger---\r\n");
        CMP->CMP_SR_b.CMP0IF = 0;
    }

    counter++;

    if(counter > 100)
    {
        counter = 0;
        NVIC_DisableIRQ(cmp0_irq);
    }

}

/**
 * @brief 比较器1 中断
 */
void cmp1if_Handler(void)
{
    /* TODO: 处理比较器1事件 */
    if(CMP->CMP_SR_b.CMP1IF & 1)
    {

        INFO_e("------INTERRUPT--CMP1_INTERRUPT-Trigger---\r\n");
        CMP->CMP_SR_b.CMP1IF = 0;
    }

    counter++;

    if(counter > 100)
    {
        counter = 0;
        NVIC_DisableIRQ(cmp1_irq);
    }
}

/**
 * @brief 比较器2 中断
 */
void cmp2if_Handler(void)
{
    /* TODO: 处理比较器2事件 */
    if(CMP->CMP_SR_b.CMP2IF & 1)
    {

        INFO_e("------INTERRUPT--CMP2_INTERRUPT-Trigger---\r\n");
        CMP->CMP_SR_b.CMP2IF = 0;
    }

    counter++;

    if(counter > 100)
    {
        counter = 0;
        NVIC_DisableIRQ(cmp2_irq);
    }

}

/**
 * @brief 比较器3 中断
 */
void cmp3if_Handler(void)
{
    /* TODO: 处理比较器3事件 */
	    if(CMP->CMP_SR_b.CMP3IF & 1)
    {

        INFO_e("------INTERRUPT--CMP3_INTERRUPT-Trigger---\r\n");
        CMP->CMP_SR_b.CMP3IF = 0;
    }

    counter++;

    if(counter > 100)
    {
        counter = 0;
        NVIC_DisableIRQ(cmp3_irq);
    }
}

/**
 * @brief 比较器4 中断
 */
void cmp4if_Handler(void)
{
    /* TODO: 处理比较器4事件 */
	   if(GPIO->p4_int_clr & BIT7)
    {
       
        GPIO->p4_int_clr &= ~BIT7;
        INFO_e("------INTERRUPT--CMP4_INTERRUPT-Trigger---\r\n");
    }
		  counter++;
    if(counter > 100)
    {
        counter = 0;
        NVIC_DisableIRQ(cmp4_irq);
    }
}

/**
 * @brief 定时器5 中断
 */
void tim5_Handler(void)
{
    /* TODO: 处理定时器5中断 */
    GPIO->out |= BIT26;
    TIM5->int_clr = 1;
    GPIO->out &= ~BIT26;
    counter++;

    if(counter > 100)
    {
        counter = 0;
        NVIC_DisableIRQ(tim5_irq);
    }
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
	 if(ANACTRL->INTR_b.int_status_raw&BIT0)
    {
      	ANACTRL->INTR_b.int_status_raw&= ~BIT0;
	
        INFO_e("------INTERRUPT--tsd_INTERRUPT-Trigger---\r\n");
    }
		counter++;
    if(counter > 100)
    {
        counter = 0;
        NVIC_DisableIRQ(tsd_irq);
    }
}

/**
 * @brief 欠压锁定 (UVLO) 中断
 */
void uvlo_Handler(void)
{
	 INFO_e("------INTERRUPT--uvlo_INTERRUPT-Trigger---\r\n");
	
    /* TODO: 处理欠压锁定中断 */
	 if(ANACTRL->INTR_b.int_status_raw&BIT1)
    {
      	ANACTRL->INTR_b.int_status_raw&= ~BIT1;
        GPIO->p4_int_clr &= ~BIT7;
        INFO_e("------INTERRUPT--uvlo_INTERRUPT-Trigger---\r\n");
    }
		  counter++;
    if(counter > 100)
    {
        counter = 0;
        NVIC_DisableIRQ(uvlo_irq);
    }
	
	
}

	void HardFault_Handler(void)
{
//    rldWDT(WDT_SHORT_WAIT);
//    bootWDTOpen(true);
//    printk("HardFault_Handler\n");

	
	 INFO_e("------INTERRUPT--HardFault_Handler-tset---\r\n");
//    while (1)
//    {
//        // 等重启
//    }
}

/**
 * @brief 综合故障中断 (硬件故障信号)
 */
void fault_Handler(void)
{
	   INFO_e("------INTERRUPT--fault_Handler-tset---\r\n");
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
    GPIO->out |= BIT26;
    TIM6->int_clr = 1;
    GPIO->out &= ~BIT26;
    counter++;

    if(counter > 100)
    {
        counter = 0;
        NVIC_DisableIRQ(tim6_irq);
    }
}

#endif











//i2c1_Handler





