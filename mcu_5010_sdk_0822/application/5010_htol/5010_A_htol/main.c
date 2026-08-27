/****************************************************************************/
/** \file main.c
**
**  Htol A demo for ODS_5010 (Cortex-M0+)
*****************************************************************************/
#include "platform_common.h"
#include "htol_action.h"


int main(void)
{
    delay_1us(200000);

    flash_clk_enable();
	/* Add Clock Enable Code Here */
    htol_clock_enable();

    htol_init();

    /* 循环等待 B 通过 UART1 下发的测试指令并分发到对应测试流程。
       UART1 互测是注册表第一个流程：B 发 "uart1_test" -> A 应答 "uart1_over"，
       一次完整往返同时测 A<->B 两条 UART1 链路，结果由 B 侧统一经 I2C1 上报 STM32 */
    while (1)
    {
        (void)htol_wait_cmd_and_run();   /* 等待 B 的 cmd -> 回复 ack -> 执行对应流程 */
        cmdline_handle_polling();
    }
}
