#ifndef SPI_TEST_H
#define SPI_TEST_H

#include "test.h"
#include "spi_type.h"

void spi0_gpio_init(void);

void spi_func1_test(uint32_t test_case, uint32_t para1, uint32_t para2);
void spi_func2_test(uint32_t test_case, uint32_t para1, uint32_t para2);

#ifdef SPI_TEST


extern transmit_data send_data_test_buffer;
extern transmit_data recv_data_test_buffer;

/** SPI specific port selection */
typedef enum
{
	SPI_MUX_SEL0 = 0U, /*!< 0, the first group port */
	SPI_MUX_SEL1,      /*!< 1, the second group port */
	SPI_MUX_SEL2,      /*!< 2, the third group port */
	SPI_MUX_MAX,       /*!< 3, the invalid group port */
} spi_mux_sel_e;

#define BUFFER_SIZE         (24u)  /* word num = BUFFER_SIZE/4*/


void spi_common_test(uint32_t which_pcs,
					uint32_t pcs_polarity,
					uint32_t frame_size,
					uint32_t spi_sck_phase,
					uint32_t spi_sck_polarity,
					uint32_t lsb_first,
					uint32_t byte_swap);
void spi_timing_figure_test(uint32_t baud_rate);// also used fo extreme speed test in asic
void spi_loopback_test(void);

//mannual test
void spi_cont_cmd_test(uint32_t frame_size);
void spi_stall_mode_test(bool stall_en);
void spi_timing_test(uint32_t SCK2PCS, uint32_t PCS2SCK, uint32_t SCK2SCK, uint32_t PCS2CMD, uint32_t SCKHLD, uint32_t SCKSTP, bool cont_en);
void spi_pin_test(uint32_t master_instance, uint32_t master_pin_sel, uint32_t master_pcs, uint32_t slave_instance, uint32_t slave_pin_sel, uint32_t slave_pcs);
void spi_debug_test(bool debug_en);


//aoto test
void spi_common_cd_test(uint32_t which_pcs,
					uint32_t pcs_polarity,
					uint32_t frame_size,
					uint32_t spi_sck_phase,
					uint32_t spi_sck_polarity,
					uint32_t lsb_first,
					uint32_t byte_swap);
void spi_continus_mode_cd_test(uint32_t test_case);
void spi_mask_cd_test(uint32_t test_case);
void spi_circule_fifo_cd_test(void);
void spi_host_request_cd_test(void);
void spi_trig_mux_cd_test(uint32_t instance, uint32_t trig_num);
void spi_match_cd_test(uint32_t match_type, bool match_only, uint32_t match0, uint32_t match1);
void spi_match_only_cd_test(uint32_t test_case);
void spi_remainder_mode_cd_test(uint32_t test_case);
void spi_half_duplex_cd_test(uint32_t mode); 
void spi_deepsleep_cd_test(uint32_t test_case, bool pclk_gate, bool fclk_gate, bool doze_en);
void spi_interrupt_cd_test(uint32_t test_case);
void spi_interrupt_dma_request_all_instance_cd_test(uint32_t spi_instance, bool use_interrupt);

#endif  /* #ifndef SPI_TEST_H */

#endif

