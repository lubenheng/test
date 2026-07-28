#include "tim_hal.h"
#include <assert.h>
#include <stddef.h>

/* 静态回调数组 */
static tim_callback_t tim_callback[TIM_INST_MAX] = { NULL };

/* 时钟使能辅助函数 */
static inline void tim_enable_clock(tim_inst_e inst)
{
    switch (inst) {
        case TIM_INST_0: DIGCTRL_2810->CLK_EN0 |= (1UL << 24); break;
        case TIM_INST_1: DIGCTRL_2810->CLK_EN0 |= (1UL << 25); break;
        case TIM_INST_2: DIGCTRL_2810->CLK_EN0 |= (1UL << 26); break;
        case TIM_INST_3: DIGCTRL_2810->CLK_EN0 |= (1UL << 27); break;
        case TIM_INST_4: DIGCTRL_2810->CLK_EN0 |= (1UL << 28); break;
        case TIM_INST_5: DIGCTRL_2810->CLK_EN1 |= (1UL << 21); break;
        case TIM_INST_6: DIGCTRL_2810->CLK_EN1 |= (1UL << 22); break;
        default: break;
    }
}

/* 获取寄存器指针 */
static void* tim_get_reg_ptr(tim_inst_e inst)
{
    switch (inst) {
        case TIM_INST_0: return (void*)TIM0_2810;
        case TIM_INST_1: return (void*)TIM1_2810;
        case TIM_INST_2: return (void*)TIM2_2810;
        case TIM_INST_3: return (void*)TIM3_2810;
        case TIM_INST_4: return (void*)TIM4_2810;
        case TIM_INST_5: return (void*)TIM5_2810;
        case TIM_INST_6: return (void*)TIM6_2810;
        default: return NULL;
    }
}

/* TIM0/TIM5/TIM6 初始化 */
static void tim0_initx(const tim_base_handle_t *htim, uint32_t arr_value)
{
    TIM0_TypeDef_2810 *tim = (TIM0_TypeDef_2810*)tim_get_reg_ptr(htim->instance);
    if (!tim) return;

    // 设置重载值
    tim->RELOAD_VAL.Reload_val = arr_value;
    // 清空计数器
    tim->CUR_VAL.Cur_val = 0;
    // 配置中断使能 (先不使能EN)
    if (htim->enable_update_it) {
        tim->EN.Int_en = 1;
    } else {
        tim->EN.Int_en = 0;
    }
    // 定时器使能位将在 start 中设置
    tim->EN.Timer_en = 0;
}

/* TIM1 初始化 (简单模式: 仅使用基本计数器产生更新中断) */
static void tim1_initx(const tim_base_handle_t *htim, uint32_t arr_value)
{
    TIM1_TypeDef_2810 *tim = (TIM1_TypeDef_2810*)tim_get_reg_ptr(htim->instance);
    if (!tim) return;
				// 1. 停止基本计数器（可选，配置前确保停止）
		tim->CR0.T1BCEN = 0;

		// 2. 设置手动模式（使基本计数器自由运行，不受其他信号复位）
		tim->IER.T1MAME = 1;      // 手动模式使能

		// 3. 选择时钟分频（24 MHz）
		tim->CR3.T1PSC = 0;       // 0: 24MHz

		// 4. 设置重载值 BARR = 23999
		tim->BARR = 23999;        // 注意：BARR 是 16 位寄存器，直接赋值

		// 5. 清除可能的历史中断标志
		tim->SR.T1BOIF = 0;       // 写 0 清除标志

		// 6. 使能基本计数器上溢中断
		tim->IER.T1BOIE = 1;

		// 7. 启动基本计数器
		tim->CR0.T1BCEN = 1;

		// 8. 总体使能 TIM1
		tim->CR0.T1EN = 1;
}

/* TIM2 初始化 */
static void tim2_initx(const tim_base_handle_t *htim, uint32_t arr_value)
{
    TIM2_TypeDef_2810 *tim = (TIM2_TypeDef_2810*)tim_get_reg_ptr(htim->instance);
    if (!tim) return;

    // 先确保定时器停止
    tim->CR1.T2CEN = 0;
		tim->CR0.T2MOD = 1;
    // 配置分频
    uint32_t psc = htim->clock_div & 0x7;
    tim->CR0.T2PSC= psc;

    // 直接写 w 成员，确保写入 (ARR可能是16位寄存器)
		tim->ARR.ARR = arr_value & 0xFFFF;
		
    // 设置输出模式
   
    tim->CR1.T2IFE = htim->enable_update_it ? 1 : 0;
    // 注意：T2CEN 将在 start 中使能，这里保持为0
	   tim->CR1.T2CEN = 1;
}

/* TIM3/TIM4 专用配置 (已有, 略作调整) */
static void tim34_configure(const tim_base_handle_t *htim, uint32_t arr_value)
{
   
    switch (htim->instance) {
        case TIM_INST_3: {
            TIM_TypeDef_2810 *tim = (TIM_TypeDef_2810*)tim_get_reg_ptr(htim->instance);
            if (!tim) return;
            // 配置 CR0
            tim->CR0.MODE = (htim->mode == TIM_MODE_Ouput_ALIGNED) ? 1 : 0;
					  tim->CR0.OPM  = (htim->opm == TIM_OPM_ENABLE) ? 1 : 0;
						tim->CR0.OCM  = (htim->ocm == TIM_OCM_ENABLE) ? 1 : 0;
						tim->CR0.PSC  = htim->clock_div & 0x7;
//						tim->CR0.IRE  = 1;   // 必须
            // DR
            if (htim->enable_compare_it) tim->DR.w = arr_value;
            else tim->DR.w = 1000;
            tim->ARR.w = arr_value;
            // CR1
            uint32_t cr1 = 0;
            cr1 |= (1 << 3); // IFE=1
            if (htim->enable_update_it) cr1 |= (1 << 6); // IP=1 根据原配置
            cr1 |= (1 << 7); // IR=1
            tim->CR1.w = cr1;
            tim->CNTR.w = 0;
            break;
        }
        case TIM_INST_4: {
            TIM_TypeDef_2810 *tim = (TIM_TypeDef_2810*)tim_get_reg_ptr(htim->instance);
            if (!tim) return;
            uint32_t cr0 = 0;
            if (htim->mode == TIM_MODE_Ouput_ALIGNED) cr0 |= (1 << 0);
            if (htim->opm == TIM_OPM_ENABLE) cr0 |= (1 << 1);
            if (htim->ocm == TIM_OCM_ENABLE) cr0 |= (1 << 4);
            uint32_t psc = htim->clock_div & 0x7;
            cr0 |= (psc << 5); // 
            cr0 |= (1 << 3);
            tim->CR0.w = cr0;
            if (htim->enable_compare_it) tim->DR.w = arr_value;
            else tim->DR.w = 1000;
            tim->ARR.w = arr_value;
            uint32_t cr1 = 0;
            cr1 |= (1 << 3);
            tim->CR1.w = cr1;
            tim->CNTR.w = 0;
            break;
        }
        default: break;
    }
}

/* 定时器基础初始化 */
void tim_base_init(tim_base_handle_t *htim)
{
    assert(htim != NULL);
    tim_enable_clock(htim->instance);

    uint32_t arr_value = htim->period;
    if (arr_value == 0) arr_value = 1;

    switch (htim->instance) {
        case TIM_INST_0:
        case TIM_INST_5:
        case TIM_INST_6:
            tim0_initx(htim, arr_value);
            break;
        case TIM_INST_1:
            tim1_initx(htim, arr_value);
            break;
        case TIM_INST_2:
            tim2_initx(htim, arr_value);
            break;
        case TIM_INST_3:
        case TIM_INST_4:
            tim34_configure(htim, arr_value);
            break;
        default:
            assert(0 && "Unsupported timer instance");
            break;
    }
}

void tim_base_deinit(tim_inst_e instance)
{
    void* tim = tim_get_reg_ptr(instance);
    if (!tim) return;
    // 停止定时器，清除使能位
    switch (instance) {
        case TIM_INST_0:
        case TIM_INST_5:
        case TIM_INST_6:
            ((TIM0_TypeDef_2810*)tim)->EN.Timer_en = 0;
            break;
        case TIM_INST_1:
            ((TIM1_TypeDef_2810*)tim)->CR0.T1EN = 0;
            ((TIM1_TypeDef_2810*)tim)->CR0.T1BCEN = 0;
            break;
        case TIM_INST_2:
            ((TIM2_TypeDef_2810*)tim)->CR1.T2CEN = 0;
            break;
        case TIM_INST_3:
        case TIM_INST_4:
            ((TIM_TypeDef_2810*)tim)->CR1.EN = 0;
            break;
        default: break;
    }
    // 注意：时钟不实际关闭，由用户决定
}

void tim_base_start(tim_inst_e instance)
{
    void* tim = tim_get_reg_ptr(instance);
    if (!tim) return;

    switch (instance) {
        case TIM_INST_0:
        case TIM_INST_5:
        case TIM_INST_6:
            ((TIM0_TypeDef_2810*)tim)->EN.Timer_en = 1;
            break;
        case TIM_INST_1:
            ((TIM1_TypeDef_2810*)tim)->CR0.T1BCEN = 1;
            ((TIM1_TypeDef_2810*)tim)->CR0.T1EN = 1;
            break;
        case TIM_INST_2:
            ((TIM2_TypeDef_2810*)tim)->CR1.T2CEN = 1;
            break;
        case TIM_INST_3:
        case TIM_INST_4:
            ((TIM_TypeDef_2810*)tim)->CR1.EN = 1;
            break;
        default: break;
    }
}

void tim_base_stop(tim_inst_e instance)
{
    void* tim = tim_get_reg_ptr(instance);
    if (!tim) return;

    switch (instance) {
        case TIM_INST_0:
        case TIM_INST_5:
        case TIM_INST_6:
            ((TIM0_TypeDef_2810*)tim)->EN.Timer_en = 0;
            break;
        case TIM_INST_1:
            ((TIM1_TypeDef_2810*)tim)->CR0.T1BCEN = 0;
            ((TIM1_TypeDef_2810*)tim)->CR0.T1EN = 0;
            break;
        case TIM_INST_2:
            ((TIM2_TypeDef_2810*)tim)->CR1.T2CEN = 0;
            break;
        case TIM_INST_3:
        case TIM_INST_4:
            ((TIM_TypeDef_2810*)tim)->CR1.EN = 0;
            break;
        default: break;
    }
}

void tim_base_start_it(tim_inst_e instance, uint32_t it_mask)
{
    // 中断已在初始化时配置使能，只需启动定时器
    tim_base_start(instance);
}

void tim_base_stop_it(tim_inst_e instance, uint32_t it_mask)
{
    tim_base_stop(instance);
}

void tim_it_clear_flag(tim_inst_e instance, uint32_t it_mask)
{
    void* tim = tim_get_reg_ptr(instance);
    if (!tim) return;

    switch (instance) {
        case TIM_INST_0:
        case TIM_INST_5:
        case TIM_INST_6:
            if (it_mask & TIM_IT_UPDATE) {
                ((TIM0_TypeDef_2810*)tim)->INT_CLR.Int_clr = 1;  // 写1清中断
            }
            break;
        case TIM_INST_1:
            if (it_mask & TIM_IT_UPDATE) {
                ((TIM1_TypeDef_2810*)tim)->SR.T1BOIF = 0;  // 写0清标志
            }
            break;
        case TIM_INST_2:
            if (it_mask & TIM_IT_UPDATE) {
                ((TIM2_TypeDef_2810*)tim)->CR1.T2IF = 0;
            }
            break;
        case TIM_INST_3:
        case TIM_INST_4:
            if (it_mask & TIM_IT_UPDATE) {
                ((TIM_TypeDef_2810*)tim)->CR1.IF = 0;  // 写0清
            }
            if (it_mask & TIM_IT_CC1) {
                ((TIM_TypeDef_2810*)tim)->CR1.IR = 0;
            }
            break;
        default: break;
    }
}

bool tim_it_get_flag(tim_inst_e instance, uint32_t it_mask)
{
    void* tim = tim_get_reg_ptr(instance);
    if (!tim) return false;

    uint32_t flags = 0;
    switch (instance) {
        case TIM_INST_0:
        case TIM_INST_5:
        case TIM_INST_6:
            if (((TIM0_TypeDef_2810*)tim)->INT_CLR.Int_clr) flags |= TIM_IT_UPDATE;
            break;
        case TIM_INST_1:
            if (((TIM1_TypeDef_2810*)tim)->SR.T1BOIF) flags |= TIM_IT_UPDATE;
            break;
        case TIM_INST_2:
            if (((TIM2_TypeDef_2810*)tim)->CR1.T2IF) flags |= TIM_IT_UPDATE;
            break;
        case TIM_INST_3:
        case TIM_INST_4:
            if (((TIM_TypeDef_2810*)tim)->CR1.IF) flags |= TIM_IT_UPDATE;
            if (((TIM_TypeDef_2810*)tim)->CR1.IR) flags |= TIM_IT_CC1;
            break;
        default: break;
    }
    return (flags & it_mask) != 0;
}

void tim_register_callback(tim_inst_e instance, tim_callback_t callback)
{
    if (instance < TIM_INST_MAX) {
        tim_callback[instance] = callback;
    }
}

void tim_irq_handler(tim_inst_e instance)
{
    uint32_t active_flags = 0;
    void* tim = tim_get_reg_ptr(instance);
    if (!tim) return;

    switch (instance) {
        case TIM_INST_0:
        case TIM_INST_5:
        case TIM_INST_6:
            if (((TIM0_TypeDef_2810*)tim)->INT_CLR.Int_clr) {
                active_flags |= TIM_IT_UPDATE;
                ((TIM0_TypeDef_2810*)tim)->INT_CLR.Int_clr = 1; // 清标志
            }
            break;
        case TIM_INST_1:
            if (((TIM1_TypeDef_2810*)tim)->SR.T1BOIF) {
                active_flags |= TIM_IT_UPDATE;
                ((TIM1_TypeDef_2810*)tim)->SR.T1BOIF = 0;
            }
            break;
        case TIM_INST_2:
            if (((TIM2_TypeDef_2810*)tim)->CR1.T2IF) {
                active_flags |= TIM_IT_UPDATE;
                ((TIM2_TypeDef_2810*)tim)->CR1.T2IF = 0;
            }
            break;
        case TIM_INST_3:
        case TIM_INST_4:
            {
                TIM_TypeDef_2810 *t = (TIM_TypeDef_2810*)tim;
                if (t->CR1.IF) {
                    active_flags |= TIM_IT_UPDATE;
                    t->CR1.IF = 0;
                }
                if (t->CR1.IR) {
                    active_flags |= TIM_IT_CC1;
                    t->CR1.IR = 0;
                }
            }
            break;
        default: break;
    }

    if (active_flags && tim_callback[instance]) {
        tim_callback[instance](instance);
    }
}
