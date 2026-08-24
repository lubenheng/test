/****************************************************************************/
/** \file main.c
**
**  Htol B test for ODS_5010  (Cortex-M0+)
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

    //STM32_check_test();

    /* 循环下发注册表内所有测试流程的指令，与 A 同步延时 200ms 后执行本侧流程。
       UART1 互测是注册表第一个流程：B 发 "uart1_test" -> A 应答 "uart1_over"，
       一次完整往返同时测 A<->B 两条 UART1 链路，结果统一经 I2C1 上报 STM32 */
    while (1)
    {
        (void)htol_run_all_flows();   /* 逐个下发 cmd -> 等 A 应答 ack -> 执行对应流程 */
        cmdline_handle_polling();
    }
}
