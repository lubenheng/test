#ifndef UART_CMDLINE_H
#define UART_CMDLINE_H


int cmd_uart_mutual_test(int argc, char *argv[]);
int cmd_uart_debug_mode_test(int argc, char *argv[]);
int cmd_uart_break_send_test(int argc, char *argv[]);
int cmd_uart_idle_send_test(int argc, char *argv[]);
int cmd_uart_idle_detect_test(int argc, char *argv[]);
int cmd_uart_trig_mux_cd_test(int argc, char *argv[]);
int cmd_uart_pin_test(int argc, char *argv[]);
int cmd_uart_pin_config_clear(int argc, char *argv[]);
int cmd_uart_wakeup_cd_test(int argc, char *argv[]);
int cmd_uart_match_cd_test(int argc, char *argv[]);
int cmd_uart_single_wire_loop_mode_cd_test(int argc, char *argv[]);
int cmd_uart_infrared_mode_cd_test(int argc, char *argv[]);
int cmd_uart_autobaud_cd_test(int argc, char *argv[]);
int cmd_uart_re_autobaud_cd_test(int argc, char *argv[]);
int cmd_uart_interrupt_cd_test(int argc, char *argv[]);
int cmd_uart_cts_mode_cd_test(int argc, char *argv[]);
int cmd_uart_rts_cd_test(int argc, char *argv[]);
int cmd_uart_break_receive_cd_test(int argc, char *argv[]);
int cmd_uart_break_send_cd_test(int argc, char *argv[]);
int cmd_uart_bit_len_send_cd_test(int argc, char *argv[]);
int cmd_uart_bit_len_receive_cd_test(int argc, char *argv[]);
int cmd_uart_extreme_speed_cd_test(int argc, char *argv[]);
int cmd_uart_fifo_cd_test(int argc, char *argv[]);
int cmd_uart_deepsleep_cd_test(int argc, char *argv[]);
int cmd_uart_cts_pin_test(int argc, char *argv[]);
int cmd_uart_rts_pin_test(int argc, char *argv[]);


int cmd_uart_common_cd_test(int argc, char *argv[]);
int cmd_uart_common_send_cd_test(int argc, char *argv[]);



#define UART_CMDLINE_MENU 	{"cmd_uart_trig_mux_cd_test",	         cmd_uart_trig_mux_cd_test,		"cmd_uart_trig_mux_cd_test"},\
											{"cmd_uart_pin_test",	         cmd_uart_pin_test,		"uart pin test"},\
											{"cmd_uart_pin_config_clear",	 cmd_uart_pin_config_clear,		"uart pin config clear"},\
											{"cmd_uart_mutual_test",		 cmd_uart_mutual_test,			"uart mutual test"},\
											{"cmd_uart_debug_mode_test",		 cmd_uart_debug_mode_test,			"cmd_uart_debug_mode_test"},\
											{"cmd_uart_idle_detect_test",	     cmd_uart_idle_detect_test,		"uart idle detect test"},\
											{"cmd_uart_idle_send_test",	     cmd_uart_idle_send_test,		"uart idle send test"},\
											{"cmd_uart_break_send_test",	 cmd_uart_break_send_test,		"uart break send test"},\
											{"cmd_uart_wakeup_cd_test", 	cmd_uart_wakeup_cd_test,	" cmd_uart_wakeup_cd_test"},\
											{"cmd_uart_match_cd_test", 		cmd_uart_match_cd_test,		" cmd_uart_match_cd_test"},\
											{"cmd_uart_single_wire_loop_mode_cd_test", 		cmd_uart_single_wire_loop_mode_cd_test,		" cmd_uart_single_wire_loop_mode_cd_test"},\
											{"cmd_uart_infrared_mode_cd_test", 		cmd_uart_infrared_mode_cd_test,		" cmd_uart_infrared_mode_cd_test"},\
											{"cmd_uart_autobaud_cd_test", 		cmd_uart_autobaud_cd_test,		" cmd_uart_autobaud_cd_test"},\
											{"cmd_uart_re_autobaud_cd_test", 		cmd_uart_re_autobaud_cd_test,		" cmd_uart_re_autobaud_cd_test"},\
											{"cmd_uart_interrupt_cd_test", 		cmd_uart_interrupt_cd_test,		" cmd_uart_interrupt_cd_test"},\
											{"cmd_uart_cts_mode_cd_test",	         cmd_uart_cts_mode_cd_test,		"cmd_uart_cts_mode_cd_test"},\
											{"cmd_uart_rts_cd_test", 		cmd_uart_rts_cd_test,		" cmd_uart_rts_cd_test"},\
											{"cmd_uart_break_receive_cd_test", 		cmd_uart_break_receive_cd_test,		" cmd_uart_break_receive_cd_test"},\
											{"cmd_uart_break_send_cd_test", 		cmd_uart_break_send_cd_test,		" cmd_uart_break_send_cd_test"},\
											{"cmd_uart_bit_len_send_cd_test", 		cmd_uart_bit_len_send_cd_test,		" cmd_uart_bit_len_send_cd_test"},\
											{"cmd_uart_bit_len_receive_cd_test", 		cmd_uart_bit_len_receive_cd_test,		" cmd_uart_bit_len_receive_cd_test"}, \
											{"cmd_uart_extreme_speed_cd_test", 	cmd_uart_extreme_speed_cd_test,		" cmd_uart_extreme_speed_cd_test"},\
											{"cmd_uart_fifo_cd_test", 			cmd_uart_fifo_cd_test,				" cmd_uart_fifo_cd_test"},\
											{"cmd_uart_cts_pin_test",	         cmd_uart_cts_pin_test,		"cmd_uart_cts_pin_test"},\
											{"cmd_uart_rts_pin_test",	         cmd_uart_rts_pin_test,		"cmd_uart_rts_pin_test"},\
											{"cmd_uart_deepsleep_cd_test", 		cmd_uart_deepsleep_cd_test,		" cmd_uart_deepsleep_cd_test"},\
											{"cmd_uart_common_cd_test", 	cmd_uart_common_cd_test,	" cmd_uart_common_cd_test"},\
											{"cmd_uart_common_send_cd_test", 	cmd_uart_common_send_cd_test,	" cmd_uart_common_send_cd_test"},
											
#endif /* UART_CMDLINE_H */

