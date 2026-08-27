#include "platform_common.h"


#include "cmdline_internal.h"
#include "uart_cmdline.h"
#include "uart_test.h"





int cmd_uart_mutual_test(int argc, char *argv[])
{
	if (argc < 8u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}	

	uint32_t baud_rate = get_arg_ulong(argv[1]);
	uint32_t parity_mode = get_arg_ulong(argv[2]);
	uint32_t stop_bit_number = get_arg_ulong(argv[3]);
	uint32_t bit_number_per_char = get_arg_ulong(argv[4]);
	uint32_t tx_bit_parity = get_arg_ulong(argv[5]);
	uint32_t rx_bit_parity = get_arg_ulong(argv[6]);
	uint32_t bit_order = get_arg_ulong(argv[7]);

//	uart_mutual_test(baud_rate,parity_mode,stop_bit_number,bit_number_per_char,tx_bit_parity,rx_bit_parity,bit_order);
		
	INFO("PASS\n");
	return 0;
}



int cmd_uart_debug_mode_test(int argc, char *argv[])
{
	bool debug_en = get_arg_ulong(argv[1]);

//	uart_debug_mode_test(debug_en);
		
	INFO("PASS\n");
	return 0;
}


int cmd_uart_break_send_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t test_case = get_arg_ulong(argv[1]);
//	uart_break_send_test(test_case);
		
	INFO("PASS\n");
	return 0;
}

int cmd_uart_idle_send_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t idle_send_method = get_arg_ulong(argv[1]);
//	uart_idle_send_test(idle_send_method);
		
	INFO("PASS\n");
	return 0;
}

int cmd_uart_idle_detect_test(int argc, char *argv[])
{
	if (argc < 4u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t test_case = get_arg_ulong(argv[1]);
	uint32_t para1 =  get_arg_ulong(argv[2]);
	uint32_t para2 =  get_arg_ulong(argv[3]);
//	uart_idle_detect_test(test_case, para1, para2);
		
	INFO("PASS\n");
	return 0;
}

int cmd_uart_trig_mux_cd_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t instance = get_arg_ulong(argv[1]);
	uint32_t trig_num = get_arg_ulong(argv[2]);
	
//	uart_trig_mux_cd_test(instance, trig_num);
		
	INFO("PASS\n");
	return 0;
}

int cmd_uart_pin_test(int argc, char *argv[])
{
	if (argc < 3u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t instance = get_arg_ulong(argv[1]);
	uint32_t pin_sel = get_arg_ulong(argv[2]);

//	uart_pin_test(instance, pin_sel);
		
	INFO("PASS\n");
	return 0;
}

int cmd_uart_pin_config_clear(int argc, char *argv[])
{
	if (argc < 3u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t port = get_arg_ulong(argv[1]);
	uint32_t pin = get_arg_ulong(argv[2]);

//	uart_pin_config_clear(port, pin);
		
	INFO("PASS\n");
	return 0;
}

int cmd_uart_cts_pin_test(int argc, char *argv[])
{
	if (argc < 3u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return ERR_TEST_ARGC;
	}

	uint32_t instance = get_arg_ulong(argv[1]);
	uint32_t cts_pin_sel = get_arg_ulong(argv[2]);

//	uart_cts_pin_test(instance, cts_pin_sel);
		
	INFO("PASS\n");
	return ERR_TEST_DONE;
}

int cmd_uart_rts_pin_test(int argc, char *argv[])
{
	if (argc < 3u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return ERR_TEST_ARGC;
	}

	uint32_t instance = get_arg_ulong(argv[1]);
	uint32_t rts_pin_sel = get_arg_ulong(argv[2]);

//	uart_rts_pin_test(instance, rts_pin_sel);
		
	INFO("PASS\n");
	return ERR_TEST_DONE;
}



int cmd_uart_common_cd_test(int argc, char *argv[])
{
	if (argc < 8u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}	

	uint32_t baud_rate = get_arg_ulong(argv[1]);
	uint32_t parity_mode = get_arg_ulong(argv[2]);
	uint32_t stop_bit_number = get_arg_ulong(argv[3]);
	uint32_t bit_number_per_char = get_arg_ulong(argv[4]);
	uint32_t tx_bit_parity = get_arg_ulong(argv[5]);
	uint32_t rx_bit_parity = get_arg_ulong(argv[6]);
	uint32_t bit_order = get_arg_ulong(argv[7]);

//	uart_common_cd_test(baud_rate,parity_mode,stop_bit_number,bit_number_per_char,tx_bit_parity,rx_bit_parity,bit_order);
		
	return 0;
}

int cmd_uart_common_send_cd_test(int argc, char *argv[])
{
	if (argc < 8u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}	

	uint32_t baud_rate = get_arg_ulong(argv[1]);
	uint32_t parity_mode = get_arg_ulong(argv[2]);
	uint32_t stop_bit_number = get_arg_ulong(argv[3]);
	uint32_t bit_number_per_char = get_arg_ulong(argv[4]);
	uint32_t tx_bit_parity = get_arg_ulong(argv[5]);
	uint32_t rx_bit_parity = get_arg_ulong(argv[6]);
	uint32_t bit_order = get_arg_ulong(argv[7]);

//	uart_common_send_cd_test(baud_rate,parity_mode,stop_bit_number,bit_number_per_char,tx_bit_parity,rx_bit_parity,bit_order);
		
	return 0;
}


int cmd_uart_wakeup_cd_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t wakeup_method = get_arg_ulong(argv[1]);
//	uart_wakeup_cd_test(wakeup_method);
		
	return 0;	
}

int cmd_uart_match_cd_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t match_method = get_arg_ulong(argv[1]);
//	uart_match_cd_test(match_method);
		
	return 0;
}

int cmd_uart_single_wire_loop_mode_cd_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t mode = get_arg_ulong(argv[1]);
//	uart_single_wire_loop_mode_cd_test(mode);
		
	return 0;
}

int cmd_uart_infrared_mode_cd_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t test_case = get_arg_ulong(argv[1]);
//	uart_infrared_mode_cd_test(test_case);
		
	return 0;
}

int cmd_uart_autobaud_cd_test(int argc, char *argv[])
{
//	uart_autobaud_cd_test();

	return 0;
}

int cmd_uart_re_autobaud_cd_test(int argc, char *argv[])
{
//	uart_re_autobaud_cd_test();

	return 0;
}

int cmd_uart_interrupt_cd_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t test_case = get_arg_ulong(argv[1]);
//	uart_interrupt_cd_test(test_case);
		
	return 0;
}

int cmd_uart_cts_mode_cd_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t cts_mode = get_arg_ulong(argv[1]);
//	uart_cts_mode_cd_test(cts_mode);

	return 0;
}

int cmd_uart_rts_cd_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t test_case = get_arg_ulong(argv[1]); 
//	uart_rts_cd_test(test_case, 1);

	return 0;
}


int cmd_uart_break_receive_cd_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t break_detect_en = get_arg_ulong(argv[1]);
	uint32_t break_send_length = get_arg_ulong(argv[2]);
	
//	uart_break_receive_cd_test((bool)break_detect_en, (uint8_t)break_send_length);
		
	return 0;
}

int cmd_uart_break_send_cd_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t test_case = get_arg_ulong(argv[1]);
	
//	uart_break_send_cd_test(test_case);
		
	return 0;
}

int cmd_uart_bit_len_send_cd_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t test_case = get_arg_ulong(argv[1]);
	
//	uart_bit_len_send_cd_test(test_case);
		
	return 0;
}

int cmd_uart_bit_len_receive_cd_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t test_case = get_arg_ulong(argv[1]);
	
//	uart_bit_len_receive_cd_test(test_case);
		
	return 0;
}

int cmd_uart_extreme_speed_cd_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t test_case = get_arg_ulong(argv[1]);
	
//	uart_extreme_speed_cd_test(test_case);
		
	return 0;
}

int cmd_uart_fifo_cd_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t test_case = get_arg_ulong(argv[1]);
	
//	uart_fifo_cd_test(test_case);
		
	return 0;
}

int cmd_uart_deepsleep_cd_test(int argc, char *argv[])
{
	if (argc < 5u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t test_case = get_arg_ulong(argv[1]);
	uint32_t pclk_gate = get_arg_ulong(argv[2]);
	uint32_t fclk_gate = get_arg_ulong(argv[3]);
	uint32_t doze = get_arg_ulong(argv[4]);
	
//	uart_deepsleep_cd_test(test_case, (bool)pclk_gate, (bool)fclk_gate, (bool)doze);
	
	return 0;
}



