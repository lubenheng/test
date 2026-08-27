#include "platform_common.h"


#include "cmdline_internal.h"
#include "spi_cmdline.h"
#include "spi_test.h"
#include "spi_type.h"
#include "spi_hal.h"




int cmd_spi_function1_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[0]);
	
	uint32_t para1 =  get_arg_ulong(argv[1]);
	uint32_t para2 =  get_arg_ulong(argv[2]);
	uint32_t para3 =  get_arg_ulong(argv[3]);

	/* add your code */
	
	spi_func1_test(test_case, para1, para2);
	
		
	INFO("spi_function1_test PASS\n");
	return 0;
}



int cmd_spi_function2_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[0]);
	
	uint32_t para1 =  get_arg_ulong(argv[1]);
	uint32_t para2 =  get_arg_ulong(argv[2]);
	uint32_t para3 =  get_arg_ulong(argv[3]);

	/* add your code */
	
	spi_func2_test(test_case, para1, para2);
	
	
	
		
	INFO("spi_function2_test PASS\n");
	return 0;

}

#ifdef SPI_TEST

uint32_t get_arg_u32(char *buf)
{
	if(!strncmp(buf, "SPI0", 4) | !strncmp(buf, "work0", 5) | 
	   !strncmp(buf, "motorola", 8) | !strncmp(buf, "tx_and_rx", 9) |
	   !strncmp(buf, "disable", 7)){
		return 0;
	} else if(!strncmp(buf, "SPI1", 4) | !strncmp(buf, "work1", 5) |
	   !strncmp(buf, "ti", 2) | !strncmp(buf, "tx_only", 7) |
	   !strncmp(buf, "enable", 6)){
		return 1;
	} else if(!strncmp(buf, "work2", 5) | !strncmp(buf, "microwire", 9) |
	   !strncmp(buf, "rx_only", 7)){
		return 2;
	} else if(!strncmp(buf, "work3", 5) | !strncmp(buf, "eeprom", 6) |
	   !strncmp(buf, "tx_rx", 5)){
		return 3;
	} else if(!strncmp(buf, "rx_tx", 5)){
		return 4;
	} else if(!strncmp(buf, "tx_dummy_rx", 11)){
		return 5;
	} else if(!strncmp(buf, "rx_dummy_tx", 11)){
		return 6;
	} else if(!strncmp(buf, "none_data", 9)){
		return 7;
	} else if(!strncmp(buf, "dummy_tx", 8)){
		return 8;
	} else if(!strncmp(buf, "dummy_rx", 8)){
		return 9;
	} else {
		return 200;
	}
}

int spi_cmd_test_init(int argc, char *argv[])
{
	spi_instance_e spi_instance;
	SPI_HandleTypeDef spi_cmd_config;
	uint8_t work;
	if(argc < 4){
		INFO("wrong cmd_format:%s\n", argv[0]);
		return 200;
	}
	spi_instance = (spi_instance_e)get_arg_u32(argv[1]);
	if(spi_instance == SPI_INSTANCE0){
		spi_cmd_config = SPI0_config;
	} else {
		spi_cmd_config = SPI1_config;
	}
	if(!strncmp(argv[2], "master", 6)){
		work = (uint8_t)get_arg_u32(argv[3]);
		if(work == 0){
			spi_cmd_config.spi_master_config.baseConfig.CPOL = 0;
			spi_cmd_config.spi_master_config.baseConfig.CPHA = 0;
		} else if(work == 1){
			spi_cmd_config.spi_master_config.baseConfig.CPOL = 0;
			spi_cmd_config.spi_master_config.baseConfig.CPHA = 1;
		} else if(work == 2){
			spi_cmd_config.spi_master_config.baseConfig.CPOL = 1;
			spi_cmd_config.spi_master_config.baseConfig.CPHA = 0;
		} else if(work == 3){
			spi_cmd_config.spi_master_config.baseConfig.CPOL = 1;
			spi_cmd_config.spi_master_config.baseConfig.CPHA = 1;
		}
		spi_cmd_config.spi_master_config.timingConfig.SCLK_SPEED = (uint32_t)get_arg_ulong(argv[4]);
		if(spi_instance == SPI_INSTANCE0){
			SPI0_config = spi_cmd_config;
		} else {
			SPI1_config = spi_cmd_config;
		}
	spi_hal_master_init(spi_instance, &(SPI0_config.spi_master_config));
	
	} else if(!strncmp(argv[2], "slave", 5)){
		work = (uint8_t)get_arg_u32(argv[3]);
		if(work == 0){
			spi_cmd_config.spi_master_config.baseConfig.CPOL = 0;
			spi_cmd_config.spi_master_config.baseConfig.CPHA = 0;
		} else if(work == 1){
			spi_cmd_config.spi_master_config.baseConfig.CPOL = 0;
			spi_cmd_config.spi_master_config.baseConfig.CPHA = 1;
		} else if(work == 2){
			spi_cmd_config.spi_master_config.baseConfig.CPOL = 1;
			spi_cmd_config.spi_master_config.baseConfig.CPHA = 0;
		} else if(work == 3){
			spi_cmd_config.spi_master_config.baseConfig.CPOL = 1;
			spi_cmd_config.spi_master_config.baseConfig.CPHA = 1;
		}
		spi_cmd_config.spi_master_config.timingConfig.SCLK_SPEED = (uint32_t)get_arg_ulong(argv[4]);
		if(spi_instance == SPI_INSTANCE0){
			SPI0_config = spi_cmd_config;
		} else {
			SPI1_config = spi_cmd_config;
		}
	spi_hal_slave_init(spi_instance, &(SPI0_config.spi_slave_config));
	} else {
		INFO("input error\n");
		return -1;
	}	
	
	INFO("spi_init success\n");
	return 0;
}

int spi_cmd_test_write_and_recv_data(int argc, char *argv[])
{
	spi_instance_e spi_instance;
	uint32_t i;
	
	if (argc < 3u){
        ERR("wrong cmd_format:%s slave_id data0 data1...\n", argv[0]);
        return ERR_TEST_ARGC;
    }
	spi_instance = (spi_instance_e)get_arg_u32(argv[1]);
	if(!strncmp(argv[2], "send_recv", 9)){
		send_data_test_buffer.data_len = (uint8_t)get_arg_ulong(argv[3]);
		recv_data_test_buffer.data_len = send_data_test_buffer.data_len;

		for (i=0; i<send_data_test_buffer.data_len; i++){
        	send_data_test_buffer.data[i] = get_arg_ulong(argv[4+i]);
    	}

		//send data func in this
		spi_hal_master_send_recv_data(spi_instance, &send_data_test_buffer, &recv_data_test_buffer);
	} else if(!strncmp(argv[2], "many_send_recv", 14)){
		send_data_test_buffer.data_len = (uint8_t)get_arg_ulong(argv[3]);
		recv_data_test_buffer.data_len = send_data_test_buffer.data_len;

		for (i=0; i<send_data_test_buffer.data_len; i++){
        	send_data_test_buffer.data[i] = i;
    	}
		//send data func in this
		spi_hal_master_send_recv_data(spi_instance, &send_data_test_buffer, &recv_data_test_buffer);
	} else {
		return -1;
	}
	delay_ms(100);
	INFO("recv_data: ");
	for(i = 0; i < recv_data_test_buffer.data_len; i++){
		INFO("%d ", recv_data_test_buffer.data[i]);
	}
	INFO("\n");
	INFO("send data\n");
	return 0;
}

int spi_cmd_transmit_data(int argc, char *argv[])
{
	spi_instance_e spi_instance;
	uint32_t i;
	uint8_t data_len;
	uint32_t slave_data;
	
	if (argc < 3u)
    {
        ERR("wrong cmd_format:%s slave_id data0 data1...\n", argv[0]);
        return ERR_TEST_ARGC;
    }
	spi_instance = (spi_instance_e)get_arg_u32(argv[1]);
	if(!strncmp(argv[2], "send_only", 9)){
		send_data_test_buffer.data_len = (uint8_t)get_arg_ulong(argv[3]);
		for (i=0; i<send_data_test_buffer.data_len; i++)
    	{
    	    send_data_test_buffer.data[i] = get_arg_ulong(argv[4+i]);
    	}
		spi_hal_master_send_data(spi_instance, &send_data_test_buffer);
		INFO("send data\n");
	} else if(!strncmp(argv[2], "master_many_send", 16)){
		send_data_test_buffer.data_len = (uint8_t)get_arg_ulong(argv[3]);
		for (i=0; i<send_data_test_buffer.data_len; i++)
    	{
    	    send_data_test_buffer.data[i] = i;
    	}
		spi_hal_master_send_data(spi_instance, &send_data_test_buffer);
		INFO("send data\n");
	} else if(!strncmp(argv[2], "recv_only", 9)){
		recv_data_test_buffer.data_len = (uint8_t)get_arg_ulong(argv[3]);
		spi_hal_master_recv_data(spi_instance, &recv_data_test_buffer);
		delay_ms(100);
		INFO("recv_data: ");
		for(i = 0; i < recv_data_test_buffer.data_len; i++){
			INFO("%d ", recv_data_test_buffer.data[i]);
		}
		INFO("\n");
	} else if(!strncmp(argv[2], "slave_send", 10)){
		send_data_test_buffer.data_len = (uint8_t)get_arg_ulong(argv[3]);
		for (i=0; i<send_data_test_buffer.data_len; i++)
    	{
    	    send_data_test_buffer.data[i] = get_arg_ulong(argv[4+i]);
    	}
		spi_hal_slave_send_data(spi_instance, &send_data_test_buffer);
		INFO("send data\n");
	} else if(!strncmp(argv[2], "slave_many_send", 15)){
		data_len = (uint8_t)get_arg_ulong(argv[3]);
		slave_data = (uint8_t)get_arg_ulong(argv[4]);
		spi_hal_slave_send_many_data(spi_instance, data_len, slave_data);
		INFO("send slave many data\n");
	} else {
		return -1;
	}
	return 0;
}

int spi_cmd_slave_trig_init(int argc, char *argv[])
{
	spi_instance_e spi_instance;
	//uint32_t i;
	//uint8_t data_len;
	//uint32_t slave_data;

	spi_instance = (spi_instance_e)get_arg_u32(argv[1]);
	spi_hal_slave_reset(spi_instance);
	return 0;
}


int spi_cmd_trans_data_phase(int argc, char *argv[])
{
	spi_instance_e spi_instance;
	uint8_t en;
	uint8_t cmd_byte;
	uint8_t addr_len;
	uint32_t addr;
	uint8_t transmode;
	uint8_t dummy_cnt;

	spi_instance = (spi_instance_e)get_arg_u32(argv[1]);
	if(!strncmp(argv[2], "cmd", 3)){
		en = (uint8_t)get_arg_u32(argv[3]);
		cmd_byte = (uint8_t)get_arg_ulong(argv[4]);
		spi_hal_master_change_cmd(spi_instance, en, cmd_byte);
	} else if(!strncmp(argv[2], "addr", 4)){
		en = (uint8_t)get_arg_u32(argv[3]);
		addr_len = (uint8_t)get_arg_ulong(argv[4]);
		addr = (uint32_t)get_arg_ulong(argv[5]);
		spi_hal_master_change_addr(spi_instance, en, addr_len, addr);
	} else if(!strncmp(argv[2], "transmode", 9)){
		transmode = (uint8_t)get_arg_u32(argv[3]);
		if((transmode == 5) || (transmode == 6) || (transmode == 8) || (transmode == 9)){
			dummy_cnt = (uint8_t)get_arg_ulong(argv[4]) - 1;
		} else {
			dummy_cnt = 0;
		}
		spi_hal_transmode_change(spi_instance, transmode, dummy_cnt);
	} else {
		return -1;
	}
	INFO("change successful!!\n");
	return 0;
}

int	cmd_spi_common_test(int argc, char *argv[])
{
	if (argc < 8u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}	

	uint32_t which_pcs = get_arg_ulong(argv[1]);
	uint32_t pcs_polarity = get_arg_ulong(argv[2]);
	uint32_t frame_size = get_arg_ulong(argv[3]);
	uint32_t spi_sck_phase = get_arg_ulong(argv[4]);
	uint32_t spi_sck_polarity = get_arg_ulong(argv[5]);
	uint32_t lsb_first = get_arg_ulong(argv[6]);
	uint32_t byte_swap = get_arg_ulong(argv[7]);


	spi_common_test(which_pcs,pcs_polarity,frame_size,spi_sck_phase,spi_sck_polarity,lsb_first,byte_swap);
	INFO("PASS\n");
	return 0;
}

int	cmd_spi_timing_figure_test(int argc, char *argv[])
{
	uint32_t baud_rate = get_arg_ulong(argv[1]);
	spi_timing_figure_test(baud_rate);
	INFO("PASS\n");
	return 0;
}

int	cmd_spi_trig_mux_cd_test(int argc, char *argv[])
{
	if (argc < 3u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t instance = get_arg_ulong(argv[1]);
	uint32_t trig_num = get_arg_ulong(argv[2]);
	
	spi_trig_mux_cd_test(instance, trig_num);

	INFO("PASS\n");
	return 0;
}

int	cmd_spi_timing_test(int argc, char *argv[])
{
	if (argc < 8u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}
	
	uint32_t SCK2PCS = get_arg_ulong(argv[1]);
	uint32_t PCS2SCK = get_arg_ulong(argv[2]);
	uint32_t SCK2SCK = get_arg_ulong(argv[3]);
	uint32_t PCS2CMD = get_arg_ulong(argv[4]);
	uint32_t SCKHLD = get_arg_ulong(argv[5]);
	uint32_t SCKSTP = get_arg_ulong(argv[6]);
	uint32_t cont_en = get_arg_ulong(argv[7]);
	
	spi_timing_test(SCK2PCS, PCS2SCK, SCK2SCK, PCS2CMD, SCKHLD, SCKSTP, (bool)cont_en);

	INFO("PASS\n");
	return 0;
}


int	cmd_spi_cont_cmd_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t frame_size = get_arg_ulong(argv[1]);

	spi_cont_cmd_test(frame_size);
	
	INFO("PASS\n");
	return 0;
}


int	cmd_spi_mask_cd_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[1]);
	
	spi_mask_cd_test(test_case);
	
	INFO("PASS\n");

	return 0;
}


int	cmd_spi_half_duplex_cd_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[1]);

	spi_half_duplex_cd_test(test_case);
	
	INFO("PASS\n");
	return 0;
}

int	cmd_spi_pin_test(int argc, char *argv[])
{
	if (argc < 7u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t master_instance = get_arg_ulong(argv[1]);
	uint32_t master_pin_sel = get_arg_ulong(argv[2]);
	uint32_t master_pcs = get_arg_ulong(argv[3]);
	uint32_t slave_instance = get_arg_ulong(argv[4]);
	uint32_t slave_pin_sel = get_arg_ulong(argv[5]);
	uint32_t slave_pcs = get_arg_ulong(argv[6]);

	spi_pin_test(master_instance, master_pin_sel, master_pcs, slave_instance, slave_pin_sel, slave_pcs);
	
	INFO("PASS\n");
	return 0;
}


int	cmd_spi_interrupt_cd_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t test_case = get_arg_ulong(argv[1]);
	
	spi_interrupt_cd_test(test_case);
	
	INFO("PASS\n");
	return 0;
}

int	cmd_spi_debug_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t debug_en = get_arg_ulong(argv[1]);
	
	spi_debug_test((bool)debug_en);
	
	INFO("PASS\n");
	return 0;
}



int	cmd_spi_common_cd_test(int argc, char *argv[])
{
	uint32_t which_pcs = get_arg_ulong(argv[1]);
	uint32_t pcs_polarity = get_arg_ulong(argv[2]);
	uint32_t frame_size = get_arg_ulong(argv[3]);
	uint32_t spi_sck_phase = get_arg_ulong(argv[4]);
	uint32_t spi_sck_polarity = get_arg_ulong(argv[5]);
	uint32_t lsb_first = get_arg_ulong(argv[6]);
	uint32_t byte_swap = get_arg_ulong(argv[7]);

	spi_common_cd_test(which_pcs,pcs_polarity,frame_size,spi_sck_phase,spi_sck_polarity,lsb_first,byte_swap);

	return 0;	
}

int	cmd_spi_circule_fifo_cd_test(int argc, char *argv[])
{
	spi_circule_fifo_cd_test();
	
	return 0;
}

int	cmd_spi_match_cd_test(int argc, char *argv[])
{
	if (argc < 5u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t match_type = get_arg_ulong(argv[1]);
	uint32_t match_only = get_arg_ulong(argv[2]);
	uint32_t match0 = get_arg_ulong(argv[3]);
	uint32_t match1 = get_arg_ulong(argv[4]);
	
	spi_match_cd_test(match_type, (bool)match_only, match0, match1);

	return 0;
}

int	cmd_spi_remainder_mode_cd_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t test_case = get_arg_ulong(argv[1]);
	spi_remainder_mode_cd_test(test_case);
	
	return 0;
}

int	cmd_spi_stall_mode_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t stall_en = get_arg_ulong(argv[1]);
	spi_stall_mode_test(stall_en);

	return 0;
}

int	cmd_spi_continus_mode_cd_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t test_case = get_arg_ulong(argv[1]);
	spi_continus_mode_cd_test(test_case);
	
	return 0;
}

int	cmd_spi_host_request_cd_test(int argc, char *argv[])
{
	spi_host_request_cd_test();

	return 0;
}


int	cmd_spi_match_only_cd_test(int argc, char *argv[])
{
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return 200;
	}

	uint32_t test_case = get_arg_ulong(argv[1]);
	spi_match_only_cd_test(test_case);
	
	return 0;
}

int	cmd_spi_deepsleep_cd_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[1]);
	uint32_t pclk_gate = get_arg_ulong(argv[2]);
	uint32_t fclk_gate = get_arg_ulong(argv[3]);
	uint32_t doze_en = get_arg_ulong(argv[4]);
	
	spi_deepsleep_cd_test(test_case, (bool)pclk_gate, (bool)fclk_gate, (bool)doze_en);
	
	return 0;
}


int	cmd_spi_interrupt_dma_request_all_instance_cd_test(int argc, char *argv[])
{
	uint32_t spi_instance = get_arg_ulong(argv[1]);
	uint32_t use_interrupt = get_arg_ulong(argv[2]);

	spi_interrupt_dma_request_all_instance_cd_test(spi_instance, (bool)use_interrupt);

	return 0;
}
#endif  /* #ifdef SPI_TEST */

