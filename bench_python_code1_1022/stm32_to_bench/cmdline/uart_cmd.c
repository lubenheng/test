#include "uart_cmd.h"
#include "utility.h" 
#include "cmdline_internal.h"

int cmd_uart_set_paramet_test(int argc, char *argv[])
{
	uint8_t case2 = get_arg_ulong(argv[1]);
	INFO_e("%d  ",case2);
	return 0;	
}


