#ifndef SPI_CMDLINE_H
#define SPI_CMDLINE_H



int cmd_spi_function1_test(int argc, char *argv[]);
int cmd_spi_function2_test(int argc, char *argv[]);

int spi_cmd_test_init(int argc, char *argv[]);
int spi_cmd_test_write_and_recv_data(int argc, char *argv[]);
int spi_cmd_transmit_data(int argc, char *argv[]);
int spi_cmd_trans_data_phase(int argc, char *argv[]);
int spi_cmd_slave_trig_init(int argc, char *argv[]);


#ifdef SPI_TEST

int	cmd_spi_common_test(int argc, char *argv[]);
int	cmd_spi_timing_figure_test(int argc, char *argv[]);
int	cmd_spi_trig_mux_cd_test(int argc, char *argv[]);
int	cmd_spi_timing_test(int argc, char *argv[]);
int	cmd_spi_cont_cmd_test(int argc, char *argv[]);
int	cmd_spi_mask_cd_test(int argc, char *argv[]);

int	cmd_spi_half_duplex_cd_test(int argc, char *argv[]);
int	cmd_spi_pin_test(int argc, char *argv[]);
int	cmd_spi_interrupt_cd_test(int argc, char *argv[]);
int	cmd_spi_debug_test(int argc, char *argv[]);
int	cmd_spi_common_cd_test(int argc, char *argv[]);
int	cmd_spi_circule_fifo_cd_test(int argc, char *argv[]);
int	cmd_spi_match_cd_test(int argc, char *argv[]);
int	cmd_spi_remainder_mode_cd_test(int argc, char *argv[]);
int	cmd_spi_stall_mode_test(int argc, char *argv[]);
int	cmd_spi_continus_mode_cd_test(int argc, char *argv[]);
int	cmd_spi_host_request_cd_test(int argc, char *argv[]);
int	cmd_spi_match_only_cd_test(int argc, char *argv[]);
int	cmd_spi_deepsleep_cd_test(int argc, char *argv[]);
int	cmd_spi_interrupt_dma_request_all_instance_cd_test(int argc, char *argv[]);

#endif


#ifdef False
#define SPI_CMDLINE_MENU 	{"cmd_spi_function1_test",	cmd_spi_function1_test,	"  cmd_spi_function1_test"},\
							{"cmd_spi_function2_test", 	cmd_spi_function2_test,	"  cmd_spi_function2_test"},\
							{"cmd_spi_common_test",	cmd_spi_common_test,    " cmd spi common test"},\
							{"cmd_spi_timing_figure_test",	cmd_spi_timing_figure_test,    " cmd_spi_timing_figure_test"},\
							{"cmd_spi_trig_mux_cd_test",	cmd_spi_trig_mux_cd_test,    " cmd_spi_trig_mux_cd_test"},\
							{"cmd_spi_timing_test",	cmd_spi_timing_test,    " cmd spi timing mode test"},\
							{"cmd_spi_cont_cmd_test",	cmd_spi_cont_cmd_test,    " cmd spi continus cmd test"},\
							{"cmd_spi_mask_cd_test",	cmd_spi_mask_cd_test,    " cmd_spi_mask_cd_test"},\
							{"cmd_spi_half_duplex_cd_test",	cmd_spi_half_duplex_cd_test,    " cmd_spi_half_duplex_cd_test"},\
							{"cmd_spi_pin_test",	cmd_spi_pin_test,    " cmd spi pin test"},\
							{"cmd_spi_interrupt_cd_test",	cmd_spi_interrupt_cd_test,    " cmd_spi_interrupt_cd_test"},\
							{"cmd_spi_debug_test",	cmd_spi_debug_test,    " cmd_spi_debug_test"},\
							{"cmd_spi_common_cd_test",	cmd_spi_common_cd_test,    " cmd_spi_common_cd_test"},\
							{"cmd_spi_circule_fifo_cd_test",	cmd_spi_circule_fifo_cd_test,    " cmd_spi_circule_fifo_cd_test"},\
							{"cmd_spi_match_cd_test",	cmd_spi_match_cd_test,    " cmd_spi_match_cd_test"},\
							{"cmd_spi_remainder_mode_cd_test",	cmd_spi_remainder_mode_cd_test,    " cmd_spi_remainder_mode_cd_test"},\
							{"cmd_spi_stall_mode_test",	cmd_spi_stall_mode_test,    " cmd_spi_stall_mode_test"},\
							{"cmd_spi_continus_mode_cd_test",	cmd_spi_continus_mode_cd_test,    " cmd_spi_continus_mode_cd_test"},\
							{"cmd_spi_host_request_cd_test",	cmd_spi_host_request_cd_test,    " cmd_spi_host_request_cd_test"},\
							{"cmd_spi_match_only_cd_test",	cmd_spi_match_only_cd_test,    " cmd_spi_match_only_cd_test"},\
                            {"cmd_spi_interrupt_dma_request_all_instance_cd_test",	cmd_spi_interrupt_dma_request_all_instance_cd_test,    " cmd_spi_interrupt_dma_request_all_instance_cd_test"},\
							{"cmd_spi_deepsleep_cd_test",	cmd_spi_deepsleep_cd_test,    " cmd_spi_deepsleep_cd_test"},
#else

#define SPI_CMDLINE_MENU 	{"cmd_spi_function1_test",	cmd_spi_function1_test,	"  cmd_spi_function1_test"},\
							{"cmd_spi_function2_test", 	cmd_spi_function2_test,	"  cmd_spi_function2_test"},\
							{"spi_cmd_test_init", 	spi_cmd_test_init,	"  spi_cmd_test_init"},\
							{"spi_cmd_transmit_data", 	spi_cmd_transmit_data,	"  spi_cmd_transmit_data"},\
							{"spi_cmd_trans_data_phase", 	spi_cmd_trans_data_phase,	"  spi_cmd_trans_data_phase"},\
							{"spi_cmd_slave_trig_init", 	spi_cmd_slave_trig_init,	"  spi_cmd_slave_trig_init"},\
							{"spi_cmd_test_write_and_recv_data",	spi_cmd_test_write_and_recv_data,	"  spi_cmd_test_write_and_recv_data"},
#endif
							
							
											
#endif /* SPI_CMDLINE_H */

