#ifndef UART_TEST_H
#define UART_TEST_H


/** UART specific port selection */
typedef enum
{
	UART_MUX_SEL0 = 0U, /*!< 0, the first group port */
	UART_MUX_SEL1,      /*!< 1, the second group port */
	UART_MUX_SEL2,      /*!< 2, the third group port */
	UART_MUX_SEL3,      /*!< 3, the fourth group port */
	UART_MUX_MAX,       /*!< 4, the invalid group port */
} uart_mux_sel_e;



//other test
void uart_over_run_test(void);
void uart_busy_flag_test(void);
void nmi_interrupt_test(void);
void uart_mutual_test(uint32_t baudrate,
					uint32_t parity_mode,
					uint32_t stop_bit_number,
					uint32_t bit_number_per_char,
					uint32_t tx_bit_parity,
					uint32_t rx_bit_parity,
					uint32_t bit_order);
void uart_break_send_test(uint32_t test_case);
void uart_idle_send_test(uint32_t idle_send_method);
void uart_idle_detect_test(uint32_t test_case, uint32_t para1, uint32_t para2);
void uart_pin_test(uint32_t instance, uint32_t pin_sel);
void uart_pin_config_clear(uint32_t port, uint32_t pin);
void uart_debug_mode_test(bool debug_en);
void uart_rx_rts_receive_test(uint32_t test_case);
void uart_cts_pin_test(uint32_t instance, uint32_t cts_pin_sel);
void uart_rts_pin_test(uint32_t instance, uint32_t rts_pin_sel);



//AUTO TEST
void uart_bit_len_send_cd_test(uint32_t test_case);
void uart_bit_len_receive_cd_test(uint32_t test_case);
void uart_autobaud_cd_test(void);
void uart_re_autobaud_cd_test(void);
void uart_interrupt_cd_test(uint32_t test_case);
void uart_extreme_speed_cd_test(uint32_t test_case);
void uart_fifo_cd_test(uint32_t test_case);
void uart_wakeup_cd_test(uint32_t wakeup_method);
void uart_match_cd_test(uint32_t match_method);
void uart_break_send_cd_test(uint32_t test_case);
void uart_break_receive_cd_test(bool break_detect_en, uint8_t break_send_length);
void uart_single_wire_loop_mode_cd_test(uint32_t mode);
void uart_infrared_mode_cd_test(uint32_t test_case);
void uart_cts_mode_cd_test(uint32_t cts_mode);
void uart_trig_mux_cd_test(uint32_t instance, uint32_t trig_num);
void uart_rts_cd_test(uint32_t test_case, uint32_t para1);
void uart_deepsleep_cd_test(uint32_t test_case, bool pclk_gate, bool fclk_gate, bool doze);


void uart_common_cd_test(uint32_t baudrate,
						uint32_t parity_mode,
						uint32_t stop_bit_number,
						uint32_t bit_number_per_char,
						uint32_t tx_bit_parity,
						uint32_t rx_bit_parity,
						uint32_t bit_order);
void uart_common_send_cd_test(uint32_t baudrate,
						uint32_t parity_mode,
						uint32_t stop_bit_number,
						uint32_t bit_number_per_char,
						uint32_t tx_bit_parity,
						uint32_t rx_bit_parity,
						uint32_t bit_order);




#endif


