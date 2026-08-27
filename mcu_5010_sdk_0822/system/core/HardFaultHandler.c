/*!
 * @file HardFaultHandler.c
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.4, A compatible declaration shall be visible when an object or function
 * with external linkage is defined
 * Default IRQHandler symbols are weak symbols declared in platform startup files(.s).
 *
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 4.6, typedefs that indicate size and signedness should be used in place of
 * the basic numerical types.
 * For structure convenience, basic type names are used.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 4.4, Sections of code should not be ¡°commented out¡±
 * Comments in the INFO statement facilitate internal debugging.
 *
 */


#include "ARMCM0plus.h"
#include "platform_common.h"
#include "uart_reg.h"
//#include "device_resource.h"
//#include "core_cm0plus.h" 



/* print MCU environment when hard fault error occur */
void HardFaultHandler(const uint32_t *sp)
{
/* OCS_Reserved_Start */
	INFO("\nHardFault!\n");
	
	uint32_t icsr  = SCB->ICSR;
	uint32_t aircr  = SCB->AIRCR;
	uint32_t scr = SCB->SCR;
	uint32_t ccr  = SCB->CCR;
	uint32_t shp0  = SCB->SHP[0];
	uint32_t shp1  = SCB->SHP[1];
	uint32_t shcsr  = SCB->SHCSR;
	

//	uint32_t r0  = sp[0];
//	uint32_t r1  = sp[1];
//	uint32_t r2  = sp[2];
//	uint32_t r3  = sp[3];
//	uint32_t r12 = sp[4];
//	uint32_t lr  = sp[5];
//	uint32_t pc  = sp[6];
//	uint32_t psr = sp[7];

//	INFO("\n\n********oops!!!**********\nHardFault:\n");
//	INFO("SCB->ICSR   0x%08lx\n", icsr);
//	INFO("SCB->AIRCR   0x%08lx\n", aircr);
//	INFO("SCB->SCR  0x%08lx\n", scr);
//	INFO("SCB->CCR   0x%08lx\n", ccr);
//	INFO("SCB->SHP0   0x%08lx\n", shp0);
//	INFO("SCB->SHP1   0x%08lx\n", shp1);
//	INFO("SCB->SHCSR   0x%08lx\n", shcsr);
//	INFO("\n");

//	INFO("SP          0x%08lx\n", (uint32_t)sp);
//	INFO("R0          0x%08lx\n", r0);
//	INFO("R1          0x%08lx\n", r1);
//	INFO("R2          0x%08lx\n", r2);
//	INFO("R3          0x%08lx\n", r3);
//	INFO("R12         0x%08lx\n", r12);
//	INFO("LR          0x%08lx\n", lr);
//	INFO("PC          0x%08lx\n", pc);
//	INFO("PSR         0x%08lx\n", psr);
/* OCS_Reserved_End */

	while(true){};
}

//void NMI_Handler(void)
//{
//  /* OCS_Reserved_Start */
//	//INFO("NMI_Handler\n");
//  /* OCS_Reserved_End */
//  while(true){};
//}


