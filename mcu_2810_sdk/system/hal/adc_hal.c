#include "adc_hal.h"
#include "Common.h"          // 包含 BIT 宏、DIGCTRL、IO 等定义
#include "gpio_hal.h"        // 可能包含 GPIO 相关
#include "CMSDK_CM0.h"       // 包含 NVIC 等
#include "timer.h"


/**
 * @brief ADC 初始化（包括 ADC 控制器和模拟前端）
 */
void vf_adc_init(void) 
	{
		uint32_t tmp;
        // ========== 2. ADC 硬件初始化（参考原始代码） ==========
    // 2.1 使能 ADC 时钟（已做）
    // 2.2 配置 ADC 控制器
    ADCCTRL->cr = 0;
    ADCCTRL->cr |= BIT7;        // ADC 使能
    ADCCTRL->cr |= BIT2;        // 数据左对齐（次高位对齐）
    ADCCTRL->cr &= ~BIT1;       // 关闭中断（若需要可开启，但 VF 中不用）

    // 2.3 采样通道掩码：Iu(CH0), Iv(CH1), Iw(CH4), Udc(CH2)（可选）
//    IO->en2 &= ~BIT6;            // 使能 Pad_Udc_en（若需要内部采样）
    ADCCTRL->msk = BIT0 | BIT1 | BIT4 | BIT2;

    // 2.4 采样周期设置（通道组0和1各 2 个 ADC 时钟周期）
    tmp = ADCCTRL->scyc;
    tmp &= ~(0xF << 0);
    tmp |= 2 << 0;
    tmp &= ~(0xF << 4);
    tmp |= 2 << 4;
    ADCCTRL->scyc = tmp;

 

    // 可选：手动触发一次采样验证硬件
    // ADCCTRL->cr |= BIT6;
    // while (ADCCTRL->cr & BIT6);
}
	

