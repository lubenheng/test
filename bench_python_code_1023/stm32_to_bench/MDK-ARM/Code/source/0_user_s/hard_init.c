#include "hard_init.h"
#include "adc.h"
#include "spi.h"
#include "stm32g4xx_hal_adc.h"
#include "stm32g4xx_hal_tim.h"
#include "stm32g4xx_hal_tim_ex.h"
#include "tim.h"

uint16_t adc_reg_buf[REGULAR_CH_NUM];    // 规则组
uint16_t adc_inj_buf[INJECTED_CH_NUM];   // 注入组// LED 配置表
static const uint16_t LED_PIN[] = {
   0, LED1_Pin, LED2_Pin, LED3_Pin, LED4_Pin, LED5_Pin
};
static GPIO_TypeDef* LED_PORT[] = {
   NULL, LED1_GPIO_Port, LED2_GPIO_Port, LED3_GPIO_Port, LED4_GPIO_Port, LED5_GPIO_Port
};

/**
  * @brief  点亮指定 LED
  * @param  led: 0~4 对应 LED1~LED5
  * @retval None
  */
void LED_Off(int led)
{
    if (led < 1 || led > 5) return;
    HAL_GPIO_WritePin(LED_PORT[led], LED_PIN[led], GPIO_PIN_RESET);
}

/**
  * @brief  熄灭指定 LED
  */
void LED_On(int led)
{
    if (led < 0 || led > 5) return;
    HAL_GPIO_WritePin(LED_PORT[led], LED_PIN[led], GPIO_PIN_SET);
}

/**
  * @brief  翻转 LED
  */
void LED_Toggle(int led)
{
    if (led < 0 || led >= 5) return;
    HAL_GPIO_TogglePin(LED_PORT[led], LED_PIN[led]);
}
/**
  * @brief  初始化adc2中断和规则采用DMA
  */
void mc_adc_init(void)
{
    __HAL_ADC_ENABLE_IT(&hadc2,ADC_IT_JEOC);
	HAL_ADCEx_InjectedStart(&hadc2);
	 HAL_ADC_Start_DMA(&hadc2, (uint32_t*)adc_reg_buf, ADC_REGULAR_BUFFER_SIZE);
}
/**
  * @brief  初始化PWM
  */
void mc_PWM_init(void)
{
    HAL_TIM_Base_Init(&htim8);
    //开启PWN初始化
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_3);
    HAL_TIM_PWM_Start(&htim8, TIM_CHANNEL_4);//ADC trigge

    HAL_TIMEx_PWMN_Start(&htim8, TIM_CHANNEL_1);
    HAL_TIMEx_PWMN_Start(&htim8, TIM_CHANNEL_2);
    HAL_TIMEx_PWMN_Start(&htim8, TIM_CHANNEL_3);

    __HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_1,2125);
    __HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_2,2125);
    __HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_3,2125);
		__HAL_TIM_SET_COMPARE(&htim8,TIM_CHANNEL_4,200);	
}
/**
  * @brief  初始化刹车中断
  */
void mc_break_init(void)
{
    __HAL_TIM_ENABLE_IT(&htim8, TIM_IT_BREAK);  
}

void As5047p_SPI_init(void)
{
    // AS5047P编码器SPI初始化
    // SPI2已在MX_SPI2_Init中配置：16位数据、主模式、CPOL=0/CPHA=1

    // 设置CS片选为高电平（未选中状态）
    HAL_GPIO_WritePin(SPI2_CSN_GPIO_Port, SPI2_CSN_Pin, GPIO_PIN_SET);

    // 延时等待AS5047P上电稳定（建议至少10ms）
    HAL_Delay(10);

    // 可选：读取NOP寄存器验证通信
    // AS5047P需要两次传输：第一次发送命令，第二次读取数据
    uint16_t nop_cmd = 0xC000;  // NOP命令（读0x0000地址）
    uint16_t dummy = 0x0000;
    uint16_t response = 0;

    // 第一次传输：发送NOP命令
    HAL_GPIO_WritePin(SPI2_CSN_GPIO_Port, SPI2_CSN_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi2, (uint8_t*)&nop_cmd, (uint8_t*)&dummy, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SPI2_CSN_GPIO_Port, SPI2_CSN_Pin, GPIO_PIN_SET);

    // 延时至少350ns（这里用1us保险）
    for(volatile int i = 0; i < 20; i++);

    // 第二次传输：读取上次命令的响应
    HAL_GPIO_WritePin(SPI2_CSN_GPIO_Port, SPI2_CSN_Pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi2, (uint8_t*)&dummy, (uint8_t*)&response, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(SPI2_CSN_GPIO_Port, SPI2_CSN_Pin, GPIO_PIN_SET);

    // AS5047P初始化完成
    // 如需读取角度，使用命令0xFFFF（读ANGLECOM寄存器）
}
/**
  * @brief  定时器中断初始化
  */
void mc_TIM_init(void)
{
	HAL_TIM_Base_Init(&htim15);
	HAL_TIM_Base_Start_IT(&htim15);
}

void System_Init()
{
	mc_PWM_init();
	mc_break_init();
	mc_adc_init();
	mc_TIM_init();
	As5047p_SPI_init();


}
