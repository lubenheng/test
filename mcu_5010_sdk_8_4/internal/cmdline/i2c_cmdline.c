#include "platform_common.h"


#include "cmdline_internal.h"
#include "i2c_type.h"
#include "i2c_hal.h"
#include "i2c_cmdline.h"
#include "i2c_test.h"

int cmd_i2c_function1_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[0]);
	
	uint32_t para1 =  get_arg_ulong(argv[1]);
	uint32_t para2 =  get_arg_ulong(argv[2]);
	uint32_t para3 =  get_arg_ulong(argv[3]);

	/* add your code */
	
	i2c_func1_test(test_case, para1, para2);
	
		
	INFO("i2c_function1_test PASS\n");
	return 0;
}



int cmd_i2c_function2_test(int argc, char *argv[])
{
	uint32_t test_case = get_arg_ulong(argv[0]);
	
	uint32_t para1 =  get_arg_ulong(argv[1]);
	uint32_t para2 =  get_arg_ulong(argv[2]);
	uint32_t para3 =  get_arg_ulong(argv[3]);

	/* add your code */
	i2c_func2_test(test_case, para1, para2);
		
	INFO("i2c_function2_test PASS\n");
	return 0;

}


unsigned int  i2c_get_char_arg_to_int(char *str)
{
    if (!((strncmp(str,"I2C_STANDARD_MODE",strlen(str))) &&
		(strncmp(str,"I2C_ADDR_MATCH_0_7BIT",strlen(str))) &&		
		(strncmp(str,"false",strlen(str)))) )
	{
		return 0U;
	}
	else if (!((strncmp(str,"I2C_FAST_MODE",strlen(str))) &&
			(strncmp(str,"I2C_ADDR_MATCH_0_10BIT",strlen(str))) &&		
			(strncmp(str,"true",strlen(str)))))
	{
		return 1U;
	}
	else if (!((strncmp(str,"I2C_FASTPLUS_MODE",strlen(str))) &&	
		    (strncmp(str,"I2C_ADDR_MATCH_0_7BIT_OR_1_7BIT",strlen(str)))) )
	{
		return 2U;
	}
	else if (!((strncmp(str,"I2C_HIGHSPEED_MODE",strlen(str))) &&	
		    (strncmp(str,"I2C_ADDR_MATCH_0_10BIT_OR_1_10BIT",strlen(str)))) )
	{
		return 3U;
	}			
	else if (!((strncmp(str,"I2C_ULTRAFAST_MODE",strlen(str))) &&	
		    (strncmp(str,"I2C_ADDR_MATCH_0_7BIT_OR_1_10BIT",strlen(str)))) )
	{
		return 4U;
	}
        else if (!((strncmp(str,"I2C_ADDR_MATCH_0_10BIT_OR_1_7BIT",strlen(str)))) )
	{
		return 5U;
	}
        else if (!((strncmp(str,"I2C_ADDR_MATCH_RANGE_7BIT",strlen(str)))) )
	{
		return 6U;
	}
        else if (!((strncmp(str,"I2C_ADDR_MATCH_RANGE_10BIT",strlen(str)))) )
	{
		return 7U;
	}
	else
	{
		return 100U;
	}

}

int cmd_i2c_enter_master(int argc, char *argv[])      //use
{
	i2c_instance_e i2c_instance;
	i2c_mode_e mode = I2C_STANDARD_MODE;
	uint32_t speed = 100000;
	bool use_dma = false;
	//uint32_t slave_addr = 0x55;
	if (argc < 5u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);
		return ERR_TEST_ARGC;
	}

	i2c_instance = (i2c_instance_e)get_arg_ulong(argv[1]);
	mode = (i2c_mode_e)get_arg_ulong(argv[2]);
	speed = (uint32_t)get_arg_ulong(argv[3]);
	use_dma = (bool)i2c_get_char_arg_to_int(argv[4]);
	/*
	if (mode == I2C_ULTRAFAST_MODE)
	{
		g_i2cm_config.i2c_master_pin_config = I2C_2PIN_PUSH_PULL;
	}
	else
	{
		g_i2cm_config.i2c_master_pin_config = I2C_2PIN_OPEN_DRAIN;
	}
	*/
	g_i2cm_config.master_mode = mode;
	g_i2cm_config.i2c_speed = speed;
	g_i2cm_config.i2c_high_speed = speed;

	if (true == use_dma)
	{
		//                i2c_master_dma_maintest(i2c_instance);
	}
	else
	{
		i2c_master_mode_handle_test(i2c_instance);
	}

	INFO("cmd_i2c_enter_master success\n");

	return ERR_TEST_DONE;
}

int cmd_i2c_enter_slave(int argc, char *argv[])
{

        i2c_instance_e i2c_instance;
        i2c_mode_e mode = I2C_FAST_MODE;
        bool use_10bit = false;
	if (argc < 6u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return ERR_TEST_ARGC;
	}

	i2c_instance = (i2c_instance_e)get_arg_ulong(argv[1]);
	mode = (i2c_mode_e)i2c_get_char_arg_to_int(argv[2]);
        use_10bit = (bool)i2c_get_char_arg_to_int(argv[3]);
        g_i2cs_config.slave_addr0 = (uint16_t)get_arg_ulong(argv[5]);
        
//        if (mode == I2C_ULTRAFAST_MODE)
//        {
//            g_i2cm_config.i2c_master_pin_config = I2C_2PIN_OUTPUT_ONLY;
//        }
//        {
//            g_i2cm_config.i2c_master_pin_config = I2C_2PIN_OPEN_DRAIN;
//        }
        g_i2cs_config.slave_mode = mode;
        g_i2cs_config.addr_10bit = use_10bit;
        
        i2c_slave_test(i2c_instance);
        
        INFO("cmd_i2c_enter_slave success\n");

	return ERR_TEST_DONE;
}

int cmd_i2c_autostop_test(int argc, char *argv[])
{
#if 0
    /* argv will be:cmd i2c_instance slave_id write_data0 write_data1... read_len */
    i2c_instance_e i2c_instance;
    uint32_t test_case = 1U;
	if (argc < 3u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return ERR_TEST_ARGC;
	}

	i2c_instance = (i2c_instance_e)get_arg_ulong(argv[1]);
	test_case = (uint32_t)get_arg_ulong(argv[2]);
        i2c_master_autostop_test(i2c_instance, test_case);
        
        return ERR_TEST_DONE;
#endif
		return 0;
}

int cmd_i2c_hostreq_test(int argc, char *argv[])
{
#if 0
    /* argv will be:cmd i2c_instance slave_id write_data0 write_data1... read_len */
    i2c_instance_e i2c_instance;
    uint32_t test_case = 1U;
    uint32_t e_flag = 1U;
	if (argc < 5u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return ERR_TEST_ARGC;
	}
    i2c_instance = (i2c_instance_e)get_arg_ulong(argv[1]);
    test_case = (uint32_t)get_arg_ulong(argv[2]);
    e_flag = (uint32_t)get_arg_ulong(argv[3]);
    uint32_t s_ic = (uint32_t)get_arg_ulong(argv[4]);
    switch(test_case)
    {
          case 1:
                  i2c_master_hreq_pin_test(i2c_instance, e_flag);
                  break;
          case 2:
                  i2c_master_gpt_trigger_test(i2c_instance, e_flag);
                  break;
          case 3:
                  i2c_master_trgt_trigger_test(i2c_instance, s_ic);
                  break;
          default:
                  break;
    }
    
    INFO("success\n");
        
        return ERR_TEST_DONE;
#endif
	return 0;
}

int cmd_i2cm_datamatch_test(int argc, char *argv[])
{
    /* argv will be:cmd i2c_instance slave_id write_data0 write_data1... read_len */
    i2c_instance_e i2c_instance;
    uint32_t datamatch_cfg = 2U;
	if (argc < 3u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return ERR_TEST_ARGC;
	}
    i2c_instance = (i2c_instance_e)get_arg_ulong(argv[1]);
    datamatch_cfg = (uint32_t)get_arg_ulong(argv[2]);
    
    i2c_master_dmatch_test(i2c_instance, (i2c_master_dmatch_config_e)datamatch_cfg);
    
    INFO("cmd_i2cm_datamatch_test success\n");
        
        return ERR_TEST_DONE;
}

#if 0
int cmd_i2cs_addrmatch_test(int argc, char *argv[])
{
    /* argv will be:cmd i2c_instance slave_id write_data0 write_data1... read_len */
	/*eg: cmd_i2cs_addrmatch_test 0 0x44 data0 data1 */
    i2c_instance_e i2c_instance;
    uint32_t addrmatch_cfg = 0U;
    
	if (argc < 5u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);
		return ERR_TEST_ARGC;
	}
    i2c_instance = (i2c_instance_e)get_arg_ulong(argv[1]);
    addrmatch_cfg = (uint32_t)get_arg_ulong(argv[2]);
    uint32_t addr0 = (uint32_t)get_arg_ulong(argv[3]);
    uint32_t addr1 = (uint32_t)get_arg_ulong(argv[4]);
    
    i2c_slave_addrmatch_test(i2c_instance, (i2c_slave_addr_config_e)addrmatch_cfg, addr0, addr1);
    
    INFO("cmd_i2cs_addrmatch_test success\n");
        
     return ERR_TEST_DONE;
}
#endif

int cmd_i2cs_smbus_gencall_test(int argc, char *argv[])
{
#if 0
    /* argv will be:cmd i2c_instance slave_id write_data0 write_data1... read_len */
    i2c_instance_e i2c_instance;
    uint32_t test_case = 0U;
    
	if (argc < 3u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return ERR_TEST_ARGC;
	}
    i2c_instance = (i2c_instance_e)get_arg_ulong(argv[1]);
    test_case = (uint32_t)get_arg_ulong(argv[2]);
    
    if (test_case == 0U)
    {
          i2c_slave_smbus_test(i2c_instance);
    }
    else
    {
          i2c_slave_generalcall_test(i2c_instance);
    }
    
    INFO("success\n");
        
     return ERR_TEST_DONE;
#endif
		return 0;
}

int cmd_i2c_read(int argc, char *argv[])      //use
{
    i2c_instance_e i2c_instance;
	if (argc < 4u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return ERR_TEST_ARGC;
	}

	i2c_instance = (i2c_instance_e)get_arg_ulong(argv[1]);
	g_i2c_master_msg.slave_addr = (uint16_t)get_arg_ulong(argv[2]);
	//g_i2c_master_msg.slave_addr = get_arg_addr(argv[2]);
	g_i2c_master_msg.slave_reg_addr = (uint16_t)get_arg_ulong(argv[3]);			//获取从机寄存器地址（8位寄存器地址,在发送的时候已经强转为8位）
	
	
	g_i2c_master_msg.rec_data_len = get_arg_ulong(argv[4]);//the last parameter is read length

        
        if (g_i2c_master_msg.slave_addr >= 256 )
        {
            g_i2c_master_msg.addr_10bit = true;
        }
        else
        {
            g_i2c_master_msg.addr_10bit = false;
        }

	if (g_i2c_master_msg.rec_data_len > sizeof(g_i2c_master_msg.rec_data))
	{
		ERR("read length is out of the limitation\n");
		return ERR_TEST_ARGC;
	}


//    INFO("send_len:%d\n",g_i2c_master_msg.send_data_len);
//    INFO("rec_len:%d\n",g_i2c_master_msg.rec_data_len);

	for (uint32_t i=0; i<SW_DATA_BUF_SIZE; i++)
	{
		g_i2c_master_msg.rec_data[i] = 0U;
	}
		/* 0) initialize data buffer first */
	i2cm_buffer_init(&g_i2c_master_msg);
	i2cm_receive_data(i2c_instance, &g_i2c_master_msg);

	delay_1us(1000);
	INFO("rx_data: ");
	for(uint8_t i = 0; i < (g_i2c_master_msg.rec_data_len); i++){
		INFO("%d ", g_i2c_master_msg.rec_data[i]);
	}
	INFO("\n");

	return ERR_TEST_DONE;
}

int cmd_i2c_write(int argc, char *argv[])      //use
{
	uint32_t i;
    i2c_instance_e i2c_instance;

    /* argv will be:cmd i2c_instance slave_id data0 data1...*/
	if (argc < 3u)
    {
        ERR("wrong cmd_format:%s slave_id data0 data1...\n", argv[0]);
        return ERR_TEST_ARGC;
    }

	if (argc > (sizeof(g_i2c_master_msg.send_data)+3u))
	{
		ERR("write length is out of the limitation\n");
		return ERR_TEST_ARGC;
	}

	i2c_instance = (i2c_instance_e)get_arg_ulong(argv[1]);
	g_i2c_master_msg.slave_addr = (uint16_t)get_arg_ulong(argv[2]);
    g_i2c_master_msg.send_data_len = (uint32_t)get_arg_ulong(argv[3]);
        
        if (g_i2c_master_msg.slave_addr >= 256 )
        {
            g_i2c_master_msg.addr_10bit = true;
        }
        else
        {
            g_i2c_master_msg.addr_10bit = false;
        }


    for (i=0; i<g_i2c_master_msg.send_data_len; i++)
    {
        g_i2c_master_msg.send_data[i] = get_arg_ulong(argv[4+i]);
    }

    for (uint32_t i=0; i<SW_DATA_BUF_SIZE; i++)
	{
		g_i2c_master_msg.rec_data[i] = 0U;
	}  
    
	i2cm_send_data(i2c_instance, &g_i2c_master_msg);

	return ERR_TEST_DONE;
}

int cmd_i2c_slave_datarefresh(int argc, char *argv[])
{
#if 0
	uint32_t i;
    i2c_instance_e i2c_instance;

    /* argv will be:cmd i2c_instance slave_id data0 data1...*/
	if (argc < 3u)
    {
        ERR("wrong cmd_format:%s slave_id data0 data1...\n", argv[0]);
        return ERR_TEST_ARGC;
    }

	i2c_instance = (i2c_instance_e)get_arg_ulong(argv[1]);
	g_i2c_slave_msg.send_data_len = (uint32_t)get_arg_ulong(argv[2]);


    for (i=0; i<g_i2c_slave_msg.send_data_len; i++)
    {
        g_i2c_slave_msg.send_data[i] = get_arg_ulong(argv[3+i]);
    }

    i2c_slave_init(i2c_instance, &g_i2cs_config, &g_i2c_slave_msg);

	return ERR_TEST_DONE;
#endif
	return 0;
}

int cmd_i2c_deepsleep_test(int argc, char *argv[])
{
#if 0
              
        REG32(0x40001020) = 0x101;
				REG32(0x40001024) = 0x101;
        INFO("stop\n");
        pmu_deepsleep_now();
        
        return ERR_TEST_DONE;
#endif
		return 0;
}

int cmd_i2c_pressure_test(int argc, char *argv[])
{
#if 0
        if (argc < 3u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return ERR_TEST_ARGC;
	}

	bool master_en = (bool)get_arg_ulong(argv[1]);
	i2c_instance_e i2c_instance = (i2c_instance_e)get_arg_ulong(argv[2]);
	if (master_en)
	{
		i2c_master_randomnum_trans_test(i2c_instance);
	}
	else
	{
		i2c_slave_randomnum_trans_test(i2c_instance);
	}	
  INFO("success\n");


	return ERR_TEST_DONE;
#endif
	return 0;
}

int cmd_i2cm_randomread_test(int argc, char *argv[])
{
#if 0
        i2c_instance_e i2c_instance;
	if (argc < 2u)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return ERR_TEST_ARGC;
	}

	i2c_instance = (i2c_instance_e)get_arg_ulong(argv[1]);
        g_i2cm_config.master_code = I2C_FASTPLUS_MODE;
        g_i2cm_config.i2c_speed = 1100000;
        i2c_master_mode_handle_test(i2c_instance);
        
        g_i2c_master_msg.send_data_len = 1u;
        g_i2c_master_msg.rec_data_len = 4u;
        g_i2c_master_msg.slave_addr = 0x17;
        for (uint32_t i=5; i<15; i++)
        {
              g_i2c_master_msg.send_data[0] = i;
              i2cm_send_data(i2c_instance, &g_i2c_master_msg);
              i2cm_receive_data(i2c_instance, &g_i2c_master_msg);
              delay_ms(10);
        }
        
        
        INFO("success\n");


	return ERR_TEST_DONE;
#endif
	return 0;
}
int cmd_i2c_pinmux_test(int argc, char *argv[])
{
#if 0
	i2c_instance_e i2c_instance;
	if (argc < 3)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return ERR_TEST_ARGC;
	}

	i2c_instance = (i2c_instance_e)get_arg_ulong(argv[1]);
	uint32_t i2c_group = (uint32_t)get_arg_ulong(argv[2]);
	
	g_i2cm_config.master_mode = I2C_FASTPLUS_MODE;
	g_i2cm_config.i2c_speed = 1000000;
	g_i2cm_config.i2c_high_speed = 1000000;
	
	i2c_pinmux_test(i2c_instance, i2c_group);
        
	INFO("success\n");

	return ERR_TEST_DONE;
#endif
	return 0;
}

int cmd_i2cs_pinmux_test(int argc, char *argv[])
{
#if 0
		i2c_instance_e i2c_instance;
	
		if (argc < 3)
		{
				INFO("wrong cmd_format:%s\n", argv[0]);

				return ERR_TEST_ARGC;
		}

		i2c_instance = (i2c_instance_e)get_arg_ulong(argv[1]);
		uint32_t i2c_group = (uint32_t)get_arg_ulong(argv[2]);
		g_i2cs_config.slave_addr0 = 0x17;
		g_i2cs_config.slave_mode = I2C_FASTPLUS_MODE;

		i2cs_pinmux_test(i2c_instance, i2c_group);

		INFO("success\n");

		return ERR_TEST_DONE;
#endif
	return 0;
}

int cmd_i2c_clock_test(int argc, char *argv[])
{
#if 0
		if (argc < 4)
		{
				INFO("wrong cmd_format:%s\n", argv[0]);

				return ERR_TEST_ARGC;
		}

		i2c_instance_e i2c_instance = (i2c_instance_e)get_arg_ulong(argv[1]);
		bool use_apb = (bool)i2c_get_char_arg_to_int(argv[2]);
		uint32_t clock_div = (uint32_t)get_arg_ulong(argv[3]);
		uint32_t i2c_speed = (uint32_t)get_arg_ulong(argv[4]);

		i2c_clock_test(i2c_instance, use_apb, clock_div, i2c_speed);

		INFO("success\n");

		return ERR_TEST_DONE;
#endif
		return 0;
}

int cmd_i2cm_timing_test(int argc, char *argv[])
{
	if (argc < 4)
	{
		INFO("wrong cmd_format:%s\n", argv[0]);

		return ERR_TEST_ARGC;
	}

	i2c_instance_e i2c_instance = (i2c_instance_e)get_arg_ulong(argv[1]);
	bool repeat_en = (bool)get_arg_ulong(argv[2]);
	uint32_t speed = (uint32_t)get_arg_ulong(argv[3]);
	g_i2cm_config.master_code = I2C_FAST_MODE;
	g_i2cm_config.i2c_speed = speed;
	i2c_master_mode_handle_test(i2c_instance);
	
	if (speed >= 2000000)
	{
			g_i2cm_config.master_code = I2C_HIGHSPEED_MODE;
			g_i2cm_config.i2c_high_speed = speed;
			g_i2cm_config.i2c_master_pin_config = I2C_2PIN_PUSH_PULL;
	}
	else
	{
			g_i2cm_config.master_code = I2C_FAST_MODE;
			g_i2cm_config.i2c_master_pin_config = I2C_2PIN_OPEN_DRAIN;
	}
	
	if (speed >= 5000000)
	{
			g_i2cm_config.i2c_high_speed = speed;
			g_i2cm_config.i2c_master_pin_config = I2C_2PIN_PUSH_PULL;
			//g_i2cm_config.master_code = I2C_ULTRAFAST_MODE;
	}
	
	g_i2c_master_msg.send_data_len = 1u;
	g_i2c_master_msg.rec_data_len = 4u;
	g_i2c_master_msg.slave_addr = 0x17;
	if (repeat_en)
	{
				g_i2c_master_msg.send_data[0] = 0x55;
				i2cm_send_data(i2c_instance, &g_i2c_master_msg);
				i2cm_receive_data(i2c_instance, &g_i2c_master_msg);
				delay_ms(10);
	}
	else
	{
				g_i2c_master_msg.send_data[0] = 0x55;
				i2cm_send_data(i2c_instance, &g_i2c_master_msg);
				i2cm_end_transfer(i2c_instance, true, false);
				i2cm_send_data(i2c_instance, &g_i2c_master_msg);
				delay_ms(10);
	}
        
	INFO("cmd_i2cm_timing_test success\n");


	return ERR_TEST_DONE;
}
