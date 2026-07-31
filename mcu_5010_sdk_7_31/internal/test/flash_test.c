/**************************************************************************//**
 * @file     flash_test.c
 * @brief    Firmware driver for the  crc check  atuo test
 *
 * @note
 * Copyright (C) Silergy Corp.
 *
 ******************************************************************************/
#include "platform_common.h"

#include "test.h"

#include "flash_reg.h"
#include "flash.h"
#include "flash_hal.h"
#include "flash_test.h"
#include "cmdline_internal.h"

//#include "clock_type.h"
#include "gpio_hal.h"
#include "clock_reg.h"


static const module_name_e g_port_module_name_arr[] = {\
  MODULE_PORTA, MODULE_PORTB, MODULE_PORTC};



int anac_read(int argc, char *argv[])
{
	uint32_t reg_addr;
	uint32_t val;
	uint32_t len;
	flash_erc_e ret = ERC_FLASH_NO_ERROR;

	/* Check the validity of parameter count */
	if (argc < 3)
	{
		INFO("wrong cmd_format:%s addr len\n", argv[0]);

		return ERR_TEST_ARGC;
	}

	reg_addr = get_arg_ulong(argv[1]);
	len = get_arg_ulong(argv[2]);
	if (reg_addr&0x03)
	{
        INFO("ERROR:address must be aligned with 4!\n");
        return ERR_TEST_ARGC;
	}

	while (len)
	{
		ret = anac_read_by_ahb(reg_addr, &val, 1);
		
		if(ret != ERC_FLASH_NO_ERROR)
		{
			INFO("ERROR anac_read_by_ahb fail\n");
			return 1;
		}

        INFO("0x%08x:0x%08x\n", (reg_addr + 0x40007C00), val);

        reg_addr += 4;
        len--;
    }

    return ERR_TEST_DONE;
}


int syscfg_read(int argc, char *argv[])
{

	uint32_t reg_addr;
	uint32_t val;
	uint32_t len;
	flash_erc_e ret = ERC_FLASH_NO_ERROR;

	/* Check the validity of parameter count */
	if (argc < 3)
	{
		INFO("wrong cmd_format:%s addr len\n", argv[0]);

		return ERR_TEST_ARGC;
	}

	reg_addr = get_arg_ulong(argv[1]);
	len = get_arg_ulong(argv[2]);
	if (reg_addr&0x03)
	{
        INFO("ERROR:address must be aligned with 4!\n");
        return ERR_TEST_ARGC;
	}

	while (len)
	{
		ret = syscfg_read_by_ahb(reg_addr, &val, 1);
		
		if(ret != ERC_FLASH_NO_ERROR)
		{
			INFO("ERROR syscfg_read_by_ahb fail\n");
			return 1;
		}

        INFO("0x%08x:0x%08x\n", (reg_addr + 0x40020400), val);

        reg_addr += 4;
        len--;
    }

    return ERR_TEST_DONE;
}


int flash_write_test(int argc, char *argv[])
{
	uint32_t reg_addr;
    uint32_t val;
    uint32_t len;
	flash_erc_e ret = ERC_FLASH_NO_ERROR;

	/*argv will be:cmd address len values*/
    len = get_arg_ulong(argv[2]);

	/* Check the validity of parameter count */
	if (argc < (len+3))
    {
        INFO("wrong cmd_format:%s addr len data\n", argv[0]);
        return ERR_TEST_ARGC;
    }

	reg_addr = get_arg_ulong(argv[1]);
	if (reg_addr&0x03)
	{
        INFO("ERROR:address must be aligned with 4!\n");
        return ERR_TEST_ARGC;
	}

    while (len)
    {
        val = get_arg_ulong(argv[argc - len]);
        WRITE_REG_32(reg_addr, val);
		ret = flash_write(MAIN_FLASH, reg_addr, &val, 1);

		if(ret != ERC_FLASH_NO_ERROR)
		{
			INFO("ERROR flash_write fail");
			return ERR_TEST_ARGC;
		}

        reg_addr += 4;
        len--;
    }

    return ERR_TEST_DONE;

}
int flash_read_test(int argc, char *argv[])
{
	uint32_t reg_addr;
	uint32_t val;
	uint32_t len;
	flash_erc_e ret = ERC_FLASH_NO_ERROR;

	/* Check the validity of parameter count */
	if (argc < 3)
	{
		INFO("wrong cmd_format:%s addr len\n", argv[0]);

		return ERR_TEST_ARGC;
	}

	reg_addr = get_arg_ulong(argv[1]);
	len = get_arg_ulong(argv[2]);
	if (reg_addr&0x03)
	{
        INFO("ERROR:address must be aligned with 4!\n");
        return ERR_TEST_ARGC;
	}

	while (len)
	{
		ret = flash_read_by_ahb(MAIN_FLASH, reg_addr, &val, 1);
		
		if(ret != ERC_FLASH_NO_ERROR)
		{
			INFO("ERROR flash_read_by_ahb fail");
			return 1;
		}

        INFO("0x%08x:0x%08x\n", reg_addr, val);

        reg_addr += 4;
        len--;
    }

    return ERR_TEST_DONE;

}


int flash_info_user_write_test(int argc, char *argv[])
{
	uint32_t reg_addr;
    uint32_t val;
    uint32_t len;
	flash_erc_e ret = ERC_FLASH_NO_ERROR;

	/*argv will be:cmd address len values*/
    len = get_arg_ulong(argv[2]);

	/* Check the validity of parameter count */
	if (argc < (len+3))
    {
        INFO("wrong cmd_format:%s addr len data\n", argv[0]);
        return ERR_TEST_ARGC;
    }

	reg_addr = get_arg_ulong(argv[1]);
	if (reg_addr&0x03)
	{
        INFO("ERROR:address must be aligned with 4!\n");
        return ERR_TEST_ARGC;
	}

    while (len)
    {
        val = get_arg_ulong(argv[argc - len]);
        //WRITE_REG_32(reg_addr, val);
		//INFO("len=%d, val=%d\n",len,val);
		INFO("len=0x%x, val=0x%x\n", len, val);
		ret = flash_write(INFO_USER_FLASH, reg_addr, &val, 1);

		if(ret != ERC_FLASH_NO_ERROR)
		{
			INFO("ERROR flash_write fail");
			return ERR_TEST_ARGC;
		}

        reg_addr += 4;
        len--;
    }

    return ERR_TEST_DONE;

}
int flash_info_user_read_test(int argc, char *argv[])
{
	uint32_t reg_addr;
	uint32_t reg_addr_info;
	uint32_t val;
	uint32_t len;
	flash_erc_e ret = ERC_FLASH_NO_ERROR;

	/* Check the validity of parameter count */
	if (argc < 3)
	{
		INFO("wrong cmd_format:%s addr len\n", argv[0]);

		return ERR_TEST_ARGC;
	}

	reg_addr = get_arg_ulong(argv[1]);
	len = get_arg_ulong(argv[2]);
	if (reg_addr&0x03)
	{
        INFO("ERROR:address must be aligned with 4!\n");
        return ERR_TEST_ARGC;
	}

	while (len)
	{
		ret = flash_read_by_ahb(INFO_USER_FLASH, reg_addr, &val, 1);
		
		if(ret != ERC_FLASH_NO_ERROR)
		{
			INFO("ERROR flash_read_by_ahb fail\n");
			return 1;
		}
		reg_addr_info = 0x1FFFF800 + reg_addr;
        INFO("0x%08x:0x%08x\n", reg_addr_info, val);

        reg_addr += 4;
        len--;
    }

    return ERR_TEST_DONE;

}


int flash_info_vendor_write_test(int argc, char *argv[])
{
	uint32_t reg_addr;
    uint32_t val;
    uint32_t len;
	flash_erc_e ret = ERC_FLASH_NO_ERROR;

	/*argv will be:cmd address len values*/
    len = get_arg_ulong(argv[2]);

	/* Check the validity of parameter count */
	if (argc < (len+3))
    {
        INFO("wrong cmd_format:%s addr len data\n", argv[0]);
        return ERR_TEST_ARGC;
    }

	reg_addr = get_arg_ulong(argv[1]);
	if (reg_addr&0x03)
	{
        INFO("ERROR:address must be aligned with 4!\n");
        return ERR_TEST_ARGC;
	}

    while (len)
    {
        val = get_arg_ulong(argv[argc - len]);
        
		ret = flash_write(INFO_VENDOR_FLASH, reg_addr, &val, 1);

		if(ret != ERC_FLASH_NO_ERROR)
		{
			INFO("ERROR flash_write fail\n");
			return ERR_TEST_ARGC;
		}

        reg_addr += 4;
        len--;
    }

    return ERR_TEST_DONE;

}
int flash_info_vendor_read_test(int argc, char *argv[])
{
	uint32_t reg_addr;
	uint32_t reg_addr_info;
	uint32_t val;
	uint32_t len;
	flash_erc_e ret = ERC_FLASH_NO_ERROR;

	/* Check the validity of parameter count */
	if (argc < 3)
	{
		INFO("wrong cmd_format:%s addr len\n", argv[0]);

		return ERR_TEST_ARGC;
	}

	reg_addr = get_arg_ulong(argv[1]);
	len = get_arg_ulong(argv[2]);
	if (reg_addr&0x03)
	{
        INFO("ERROR:address must be aligned with 4!\n");
        return ERR_TEST_ARGC;
	}

	while (len)
	{
		ret = flash_read_by_ahb(INFO_VENDOR_FLASH, reg_addr, &val, 1);
		
		if(ret != ERC_FLASH_NO_ERROR)
		{
			INFO("ERROR flash_read_by_ahb fail");
			return 1;
		}
		reg_addr_info = 0x1FFFF810 + reg_addr;
        INFO("0x%08x:0x%08x\n", reg_addr_info, val);

        reg_addr += 4;
        len--;
    }

    return ERR_TEST_DONE;

}


int flash_otp_write_test(int argc, char *argv[])
{
	uint32_t reg_addr;
    uint32_t val;
    uint32_t len;
	flash_erc_e ret = ERC_FLASH_NO_ERROR;

	/*argv will be:cmd address len values*/
    len = get_arg_ulong(argv[2]);

	/* Check the validity of parameter count */
	if (argc < (len+3))
    {
        INFO("wrong cmd_format:%s addr len data\n", argv[0]);
        return ERR_TEST_ARGC;
    }

	reg_addr = get_arg_ulong(argv[1]);

    while (len)
    {
        val = get_arg_ulong(argv[argc - len]);
        
		ret = flash_write(OTP_FLASH, reg_addr, &val, 1);

		if(ret != ERC_FLASH_NO_ERROR)
		{
			INFO("ERROR flash_write fail\n");
			return ERR_TEST_ARGC;
		}

        reg_addr += 1;
        len--;
    }

    return ERR_TEST_DONE;

}
int flash_otp_read_test(int argc, char *argv[])
{
	uint32_t reg_addr;
	uint32_t val;
	uint32_t len;
	flash_erc_e ret = ERC_FLASH_NO_ERROR;

	/* Check the validity of parameter count */
	if (argc < 3)
	{
		INFO("wrong cmd_format:%s addr len\n", argv[0]);

		return ERR_TEST_ARGC;
	}

	reg_addr = get_arg_ulong(argv[1]);
	len = get_arg_ulong(argv[2]);
	if (reg_addr&0x03)
	{
        INFO("ERROR:address must be aligned with 4!\n");
        return ERR_TEST_ARGC;
	}

	while (len)
	{
		ret = flash_read_by_ahb(MAIN_FLASH, reg_addr, &val, 1);
		
		if(ret != ERC_FLASH_NO_ERROR)
		{
			INFO("ERROR flash_read_by_ahb fail");
			return 1;
		}

        INFO("0x%08x:0x%08x\n", reg_addr, val);

        reg_addr += 4;
        len--;
    }

    return ERR_TEST_DONE;

}


int flash_otp_reboot_test(int argc, char *argv[])
{
	/* 对RETRIM写入0xAB56 芯片boot复位，进行复位reboot */
	INFO("start reboot ... \n");
	RCC->RSTKEY.w = 0x0000AB56;
	
	return 0;
}



/**
 *@brief init gpio port, cause gpio interrupt used
 *@param[in] gpio_port
 *@return    none
 */
static void gpio_clk_init(gpio_port_e gpio_port)
{
	DEV_ASSERT(gpio_port < GPIO_PORT_MAX);
	
	module_clk_cfg_t module_clk_cfg;
	
	module_clk_cfg.module_name = g_port_module_name_arr[gpio_port];
	module_clk_cfg.pclk_dsleep_gating_en = GATING_ON;
	module_clk_cfg.fclk_div = PER_DIVIDE_1;
	(void)clock_module_clk_cfg(&module_clk_cfg);
}

static void gpio_c0_init(void)
{
//	gpio_settings_config_t gpio_settings_config;
//	
//	/* user can modify info here */
//	gpio_settings_config.gpio_port = GPIO_C;
//	gpio_settings_config.gpio_pin = GPIO_PIN_1;
//	gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_1;
//	gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
//	gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
//	gpio_settings_config.gpio_strength = GPIO_STRENGTH_LOW_FAST;
//	gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
//	
//	gpio_init(&gpio_settings_config);
}

static void gpio_a6_init(void)
{
//	gpio_settings_config_t gpio_settings_config;
//	
//	/* user can modify info here */
//	gpio_settings_config.gpio_port = GPIO_A;
//	gpio_settings_config.gpio_pin = GPIO_PIN_6;
//	gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_1;
//	gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
//	gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
//	gpio_settings_config.gpio_strength = GPIO_STRENGTH_LOW_FAST;
//	gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
//	
//	gpio_init(&gpio_settings_config);
}

static void gpio_a1_init(void)
{
//	gpio_settings_config_t gpio_settings_config;
//	
//	/* user can modify info here */
//	gpio_settings_config.gpio_port = GPIO_A;
//	gpio_settings_config.gpio_pin = GPIO_PIN_1;
//	gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_1;
//	gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
//	gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
//	gpio_settings_config.gpio_strength = GPIO_STRENGTH_LOW_FAST;
//	gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
//	
//	gpio_init(&gpio_settings_config);
}

static void gpio_a2_init(void)
{
//	gpio_settings_config_t gpio_settings_config;
//	
//	/* user can modify info here */
//	gpio_settings_config.gpio_port = GPIO_A;
//	gpio_settings_config.gpio_pin = GPIO_PIN_2;
//	gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_1;
//	gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
//	gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
//	gpio_settings_config.gpio_strength = GPIO_STRENGTH_LOW_FAST;
//	gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
//	
//	gpio_init(&gpio_settings_config);
}

//static void gpio_a3_init(void)
//{
//	gpio_settings_config_t gpio_settings_config;
//	
//	/* user can modify info here */
//	gpio_settings_config.gpio_port = GPIO_A;
//	gpio_settings_config.gpio_pin = GPIO_PIN_3;
//	gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_1;
//	gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
//	gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
//	gpio_settings_config.gpio_strength = GPIO_STRENGTH_LOW_FAST;
//	gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
//	
//	gpio_init(&gpio_settings_config);
//}

static void gpio_a7_init(void)
{
//	gpio_settings_config_t gpio_settings_config;
//	
//	/* user can modify info here */
//	gpio_settings_config.gpio_port = GPIO_A;
//	gpio_settings_config.gpio_pin = GPIO_PIN_7;
//	gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_1;
//	gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
//	gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
//	gpio_settings_config.gpio_strength = GPIO_STRENGTH_LOW_FAST;
//	gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
//	
//	gpio_init(&gpio_settings_config);
}


//static void gpio_handle(uint32_t pin_num)
//{
//        INFO("Enter PTA1 interrupt\n");
//  	gpio_toggle(GPIO_A, GPIO_PIN_1);
//	gpio_toggle(GPIO_A, GPIO_PIN_1);
//}

static void gpio_a0_init(void)
{
//	gpio_settings_config_t gpio_settings_config;
//	
//	/* user can modify info here */
//	gpio_settings_config.gpio_port = GPIO_A;
//	gpio_settings_config.gpio_pin = GPIO_PIN_0;
//	gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_1;
//	gpio_settings_config.gpio_dir = GPIO_DIR_INPUT;
//	gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
//	gpio_settings_config.gpio_strength = GPIO_STRENGTH_LOW_FAST;
//	
//	/* user can modify interrupt handle here */
//	gpio_settings_config.gpio_interrupt_cfg_info.int_handle = gpio_handle;
//	gpio_settings_config.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_RISING_EDGE;
//	gpio_settings_config.gpio_interrupt_cfg_info.priority = 0U;
//        
//	gpio_init(&gpio_settings_config);
}


static void gpio_demo(void)
{
    gpio_clk_init(GPIO_A);
    gpio_a0_init(); /* input interrupt */
    gpio_a1_init();
    gpio_a2_init();
    gpio_a6_init();
    
    gpio_a7_init();
}


void flash_write_operation_and_interrupt_test(uint32_t sector_index)
{
  gpio_demo();
  uint32_t test_sector = sector_index;
  uint32_t write_start_address = (test_sector * 512);
  uint32_t read_start_address = (test_sector * 512);
  uint32_t end_address = (test_sector + 1) * 512;
  uint32_t write_data = 0x5a5a5a5a;

  while (1)
  {

    /* sector erase */
    REG32(0x40007014) = 0x100;
    REG32(0x4000701C) = 0xffffffff;
    REG32(0x40007004) = (test_sector * 128);
    
    REG32(0x40007000) = 0x464c4f05;
    while ((0x04 & REG32(0x4000701C)) != 0x04);
		
		/* write sector */
    REG32(0x40007008) = write_data;
    REG32(0x4000701C) = 0xffffffff;

    REG32(0x40007000) = 0x464c4f03;
    gpio_output_set(GPIO_A, GPIO_PIN_7, GPIO_LOW_LEVEL);
    write_start_address = (test_sector * 512);
    read_start_address = (test_sector * 512);
    while (write_start_address < end_address)
    {
      REG32(0x40007004) = (write_start_address/4);
      /* gpio output*/
      //REG32(0x40010004) = (0x01UL<<3);
      gpio_output_set(GPIO_A, GPIO_PIN_7, GPIO_HIGH_LEVEL);		
      REG32(0x40007000) = 0x464c4f03;
      gpio_output_set(GPIO_A, GPIO_PIN_7, GPIO_LOW_LEVEL);
//      REG32(0x40010004) = (0x0);
//      REG32(0x40010004) = (0x01UL<<3);
      while ((0x04 & REG32(0x4000701C)) != 0x04);
//      REG32(0x40010004) = (0x0);
      REG32(0x4000701C) = 0xffffffff;
      
      REG32(0x40007008) =0;
      REG32(0x40007004) = read_start_address/4;
      REG32(0x40007000) = 0x464c4f00;
      while ((0x04 & REG32(0x4000701C)) != 0x04);
			delay_1us(100);
      uint32_t data_1 = REG32(0x40007008);
        
      if (data_1 != 0x5a5a5a5a)
      {
          INFO("exception read address : 0x%x\n", read_start_address);
          INFO("data exception!!!\n");
          INFO("read_data_apb = 0x%x\n", data_1);
          INFO("read_data_ahb = 0x%x\n", REG32(read_start_address));
          INFO("theory_data_1 = 0x%x\n", 0x5a5a5a5a);
          while(1);
      }
      
      read_start_address+=4;
      write_start_address+=4;
    }
 
  }

}


void flash_erase_operation_and_interrupt_test(uint32_t sector_index)
{
#if 0
  gpio_demo();
  
  uint32_t test_sector = sector_index;
  uint32_t write_start_address = (test_sector * 512);
  uint32_t read_start_address = (test_sector * 512);
  uint32_t addr1 = 0;
  uint32_t addr2 = 60;
  uint32_t addr3 = 80;
  uint32_t data_1 = 0;
  uint32_t write_data = 0x5a5a5a5a;
  
  gpio_output_set(GPIO_A, GPIO_PIN_7, GPIO_LOW_LEVEL);
  while (1)
  {
    
    /* erase sector */
    REG32(0x40007014) = 0x100;
    REG32(0x4000701C) = 0xffffffff;
    REG32(0x40007004) = (test_sector * 128);
    
   // REG32(0x40010004) |= (1 << 4);
    gpio_output_set(GPIO_A, GPIO_PIN_7, GPIO_HIGH_LEVEL);
    REG32(0x40007000) = 0x464c4f05;
    gpio_output_set(GPIO_A, GPIO_PIN_7, GPIO_LOW_LEVEL);
    //REG32(0x40010004) &= ~(1 << 4);
    //REG32(0x40010004) |= (1 << 4);
    while ((0x04 & REG32(0x4000701C)) != 0x04);
    //REG32(0x40010004) &= ~(1 << 4);
    
    /* read slot */
    REG32(0x4000701C) = 0xffffffff;
    REG32(0x40007004) = (test_sector * 128) + addr1;
    REG32(0x40007000) = 0x464c4f00;
    while ((0x04 & REG32(0x4000701C)) != 0x04);
    data_1 = REG32(0x40007008);

    if (data_1 != 0xffffffff)
    {
        INFO("data exception!!!\n");
        INFO("read_s_data_1 = 0x%x\n", data_1);
        INFO("theory_data_1 = 0x%x\n", 0xffffffff);
        while(1);
    }
    
    REG32(0x4000701C) = 0xffffffff;
    REG32(0x40007004) = (test_sector * 128) + addr2;
    REG32(0x40007000) = 0x464c4f00;
    while ((0x04 & REG32(0x4000701C)) != 0x04);
    
    data_1 = REG32(0x40007008);
    if (data_1 != 0xffffffff)
    {
        INFO("data exception!!!\n");
        INFO("read_data_2 = 0x%x\n", data_1);
        INFO("theory_data_2 = 0x%x\n", 0xffffffff);
        while(1);
    }
    
    REG32(0x4000701C) = 0xffffffff;
    REG32(0x40007004) = (test_sector * 128) + addr3;
    REG32(0x40007000) = 0x464c4f00;
    while ((0x04 & REG32(0x4000701C)) != 0x04);
    data_1 = REG32(0x40007008);
    if (data_1 != 0xffffffff)
    {
        INFO("data exception!!!\n");
        INFO("read_data_3 = 0x%x\n", data_1);
        INFO("theory_data_3 = 0x%x\n", 0xffffffff);
        while(1);
    }
      
    /* write slot */
    REG32(0x40007008) = write_data;
    REG32(0x40007004) = (test_sector * 128) + addr1;
    REG32(0x4000701C) = 0xffffffff;
    REG32(0x40007000) = 0x464c4f03;
    while ((0x04 & REG32(0x4000701C)) != 0x04);
  
    REG32(0x4000701C) = 0xffffffff;
    REG32(0x40007004) = (test_sector * 128) + addr2;
    REG32(0x40007000) = 0x464c4f03;
    while ((0x04 & REG32(0x4000701C)) != 0x04);

    REG32(0x4000701C) = 0xffffffff;
    REG32(0x40007004) = (test_sector * 128) + addr3;
    REG32(0x40007000) = 0x464c4f03;
    while ((0x04 & REG32(0x4000701C)) != 0x04);
    
    /* read slot */
    REG32(0x40007004) = (test_sector * 128) + addr1;
    REG32(0x4000701C) = 0xffffffff;
    REG32(0x40007000) = 0x464c4f00;
    while ((0x04 & REG32(0x4000701C)) != 0x04);
    
    data_1 = REG32(0x40007008);
     if (data_1 != write_data) 
    {
        INFO("data exception!!!\n");
        INFO("read_data_1 = 0x%x\n", data_1);
        INFO("theory_data_1 = 0x%x\n", write_data);
        while(1);
    }
     
    REG32(0x40007004) = (test_sector * 128) + addr2;
    REG32(0x4000701C) = 0xffffffff;
    REG32(0x40007000) = 0x464c4f00;
    while ((0x04 & REG32(0x4000701C)) != 0x04);
    
    data_1 = REG32(0x40007008);
    if (data_1 != write_data)
    {
        INFO("data exception!!!\n");
        INFO("read_data_2 = 0x%x\n", data_1);
        INFO("theory_data_2 = 0x%x\n", write_data);
        while(1);
    }
   
    REG32(0x40007004) = (test_sector * 128) + addr3;
    REG32(0x4000701C) = 0xffffffff;

    REG32(0x40007000) = 0x464c4f00;
    while ((0x04 & REG32(0x4000701C)) != 0x04);
    
    data_1 = REG32(0x40007008);
    if (data_1 != write_data)
    {
        INFO("data exception!!!\n");
        INFO("read_data_3 = 0x%x\n", data_1);
        INFO("theory_data_3 = 0x%x\n", write_data);
        while(1);
    } 
  }
#endif
}


static void flash_test_gpio_init(void)
{
//		gpio_settings_config_t gpio_settings_config;
//		gpio_settings_config.gpio_port = GPIO_E;
//    gpio_settings_config.gpio_pin = GPIO_PIN_7;
//    gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_1;
//    gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
//    gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
//    gpio_settings_config.gpio_strength = GPIO_STRENGTH_LOW_FAST;
//    gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
//    gpio_settings_config.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_HIGH_LEVEL;
//    gpio_settings_config.gpio_interrupt_cfg_info.priority = 1u;
//    gpio_init(&gpio_settings_config);
}

#pragma arm section code = "tcm_sram"
void data_flash_performance_test(uint32_t test_type, uint32_t cmp1us, uint32_t accessmode)
{
	uint32_t select_value = 0;
	
	flash_test_gpio_init();
	select_value = (REG32(0x40008018) & 0x00fffffc);
	select_value = (select_value | (cmp1us << 24) | (accessmode << 0));
	WRITE_REG_32(0x40008018, select_value);
	
	WRITE_REG_32(0x4000801c, 0xFF);
	WRITE_REG_32(0x40008014, 0x100);
	if (test_type == 0) /*read*/
	{
		WRITE_REG_32(0x40008004, 0x0);
		if ((REG32(0x40014004) & (1 << 7)) == (1 << 7))
		{
			WRITE_REG_32(0x4001F004, 0); //toggle PTE7
		}
		else
		{
			WRITE_REG_32(0x40014004, 0xFF); //toggle PTE7
		}
		WRITE_REG_32(0x40008000, 0x464c4f00);
	}
	else if(test_type == 1) /*write*/
	{
		WRITE_REG_32(0x40008004, 0x0);
		WRITE_REG_32(0x40008008, 0xAAAAAAAA);
		//gpio_toggle(GPIO_E, GPIO_PIN_7);
		if ((REG32(0x40014004) & (1 << 7)) == (1 << 7))
		{
			WRITE_REG_32(0x40014004, 0); //toggle PTE7
		}
		else
		{
			WRITE_REG_32(0x40014004, 0xFF); //toggle PTE7
		}
		WRITE_REG_32(0x40008000, 0x464c4f03);
	}
	else if (test_type == 2) /*chip erase*/
	{
			WRITE_REG_32(0x40008004, 0x0);
		//gpio_toggle(GPIO_E, GPIO_PIN_7);
		if ((REG32(0x40014004) & (1 << 7)) == (1 << 7))
		{
			WRITE_REG_32(0x40014004, 0); //toggle PTE7
		}
		else
		{
			WRITE_REG_32(0x40014004, 0xFF); //toggle PTE7
		}
			WRITE_REG_32(0x40008000, 0x464c4f06);
	}
	else if (test_type == 3) /*sector erase*/
	{
			WRITE_REG_32(0x40008004, 0x0);
		//gpio_toggle(GPIO_E, GPIO_PIN_7);
		if ((REG32(0x40014004) & (1 << 7)) == (1 << 7))
		{
			WRITE_REG_32(0x40014004, 0); //toggle PTE7
		}
		else
		{
			WRITE_REG_32(0x40014004, 0xFF); //toggle PTE7
		}
			WRITE_REG_32(0x40008000, 0x464c4f05);
	}
	else /*ahb read*/
	{
			WRITE_REG_32(0x40014004, 0x0);
			WRITE_REG_32(0x40014004, 0xFF);

			uint32_t read_data = *((volatile uint32_t *)0x20020000);
			__DSB();
	}
	if (test_type <= 3)
	{
		while ((REG32(0x4000801C) & (1 << 2)) != (1 << 2));
	}
	//gpio_toggle(GPIO_E, GPIO_PIN_7);
	if ((REG32(0x40014004) & (1 << 7)) == (1 << 7))
	{
		WRITE_REG_32(0x40014004, 0); //toggle PTE7
	}
	else
	{
		WRITE_REG_32(0x40014004, 0xFF); //toggle PTE7
	}
	WRITE_REG_32(0x40008014, 0x0);
	
	INFO("data flash cmp1us = %d\n", ((REG32(0x40008018) & (0xff000000)) >> 24));
	INFO("data flash accessmode = %d\n", REG32(0x40008018) & (0x3));
}


void code_flash_performance_test(uint32_t test_type, uint32_t cmp1us, uint32_t accessmode)
{
	uint32_t select_value = 0;
	
	flash_test_gpio_init();
	
	select_value = (REG32(0x40007018) & 0x00fffffc);
	select_value = (select_value | (cmp1us << 24) | (accessmode << 0));
	WRITE_REG_32(0x40007018, select_value);
	
	WRITE_REG_32(0x40007014, 0x100);
	if (test_type == 0) /*read*/
	{
		WRITE_REG_32(0x40007004, 0x0);
		WRITE_REG_32(0x4000701c, 0xFF);
		//gpio_toggle(GPIO_E, GPIO_PIN_7);
		if ((REG32(0x40014004) & (1 << 7)) == (1 << 7))
		{
			WRITE_REG_32(0x40014004, 0); //toggle PTE7
		}
		else
		{
			WRITE_REG_32(0x40014004, 0xFF); //toggle PTE7
		}
		WRITE_REG_32(0x40007000, 0x464c4f00);
	}
	else if(test_type == 1) /*write*/
	{
		WRITE_REG_32(0x40007004, 0x0);
		WRITE_REG_32(0x40007008, 0xAAAAAAAA);
		WRITE_REG_32(0x4000701c, 0xFF);
		//gpio_toggle(GPIO_E, GPIO_PIN_7);
		if ((REG32(0x40014004) & (1 << 7)) == (1 << 7))
		{
			WRITE_REG_32(0x40014004, 0); //toggle PTE7
		}
		else
		{
			WRITE_REG_32(0x40014004, 0xFF); //toggle PTE7
		}
		WRITE_REG_32(0x40007000, 0x464c4f03);
	}
	else if (test_type == 2) /*chip erase*/
	{
			WRITE_REG_32(0x40007004, 0x0);
			WRITE_REG_32(0x4000701c, 0xFF);
		//gpio_toggle(GPIO_E, GPIO_PIN_7);
		if ((REG32(0x40014004) & (1 << 7)) == (1 << 7))
		{
			WRITE_REG_32(0x40014004, 0); //toggle PTE7
		}
		else
		{
			WRITE_REG_32(0x40014004, 0xFF); //toggle PTE7
		}
			WRITE_REG_32(0x40007000, 0x464c4f06);
	}
	else if (test_type == 3) /*sector erase*/
	{
			WRITE_REG_32(0x40007004, 0x0);
			WRITE_REG_32(0x4000701c, 0xFF);
			//gpio_toggle(GPIO_E, GPIO_PIN_7);
		if ((REG32(0x40014004) & (1 << 7)) == (1 << 7))
		{
			WRITE_REG_32(0x40014004, 0); //toggle PTE7
		}
		else /*ahb read*/
		{
			WRITE_REG_32(0x40014004, 0xFF); //toggle PTE7
		}
			WRITE_REG_32(0x40007000, 0x464c4f05);
	}
	else
	{
			WRITE_REG_32(0x40014004, 0x0);
			WRITE_REG_32(0x40014004, 0xFF);
			uint32_t read_data = *((volatile uint32_t *)0x4);
			__DSB();
	}
	if (test_type <= 3)
	{
		while ((REG32(0x4000701C) & (1 << 2)) != (1 << 2));
	}
	//gpio_toggle(GPIO_E, GPIO_PIN_7);
		if ((REG32(0x40014004) & (1 << 7)) == (1 << 7))
		{
			WRITE_REG_32(0x40014004, 0); //toggle PTE7
		}
		else
		{
			WRITE_REG_32(0x40014004, 0xFF); //toggle PTE7
		}
	WRITE_REG_32(0x40007014, 0x0);
	INFO("code flash cmp1us = %d\n", ((REG32(0x40008018) & (0xfff00000)) >> 24));
	delay_ms(1);
	INFO("code flash accessmode = %d\n", REG32(0x40008018) & (0x3));
}
#pragma arm section code

/* exception power down test */
static void gpio_a0_output_init(void)
{
//	gpio_settings_config_t gpio_settings_config;
//	
//	/* user can modify info here */
//	gpio_settings_config.gpio_port = GPIO_A;
//	gpio_settings_config.gpio_pin = GPIO_PIN_0;
//	gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_1;
//	gpio_settings_config.gpio_dir = GPIO_DIR_OUTPUT;
//	gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
//	gpio_settings_config.gpio_strength = GPIO_STRENGTH_LOW_FAST;
//	gpio_settings_config.gpio_interrupt_cfg_info.int_handle = NULL;
//	
//	gpio_init(&gpio_settings_config);
}


//static void code_flash_erase_exception_power_down_handle(uint32_t pin_num)
//{
//	gpio_toggle(GPIO_C, GPIO_PIN_0);
//	WRITE_REG_32(0x40007000, 0x464c4f05);
//	gpio_toggle(GPIO_C, GPIO_PIN_0);
//}

//static void data_flash_erase_exception_power_down_handle(uint32_t pin_num)
//{
//	gpio_toggle(GPIO_C, GPIO_PIN_0);
//	WRITE_REG_32(0x40008000, 0x464c4f05);
//	gpio_toggle(GPIO_C, GPIO_PIN_0);
//}

//static void code_flash_write_exception_power_down_handle(uint32_t pin_num)
//{
//	gpio_toggle(GPIO_C, GPIO_PIN_0);
//	WRITE_REG_32(0x40007000, 0x464c4f02);
//	gpio_toggle(GPIO_C, GPIO_PIN_0);
//}

//static void data_flash_write_exception_power_down_handle(uint32_t pin_num)
//{
//	gpio_toggle(GPIO_C, GPIO_PIN_0);
//	WRITE_REG_32(0x40008000, 0x464c4f02);
//	gpio_toggle(GPIO_C, GPIO_PIN_0);
//}

static void code_flash_erase_exception_gpio_a1_input_init(void)
{
//	gpio_settings_config_t gpio_settings_config;
//	
//	/* user can modify info here */
//	gpio_settings_config.gpio_port = GPIO_A;
//	gpio_settings_config.gpio_pin = GPIO_PIN_1;
//	gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_1;
//	gpio_settings_config.gpio_dir = GPIO_DIR_INPUT;
//	gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
//	gpio_settings_config.gpio_strength = GPIO_STRENGTH_LOW_FAST;
//	
//	/* user can modify interrupt handle here */
//	gpio_settings_config.gpio_interrupt_cfg_info.int_handle = code_flash_erase_exception_power_down_handle;
//	gpio_settings_config.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_RISING_EDGE;
//	gpio_settings_config.gpio_interrupt_cfg_info.priority = 1U;
//	
//	gpio_init(&gpio_settings_config);
}

static void data_flash_erase_exception_gpio_a1_input_init(void)
{
//	gpio_settings_config_t gpio_settings_config;
//	
//	/* user can modify info here */
//	gpio_settings_config.gpio_port = GPIO_A;
//	gpio_settings_config.gpio_pin = GPIO_PIN_1;
//	gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_1;
//	gpio_settings_config.gpio_dir = GPIO_DIR_INPUT;
//	gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
//	gpio_settings_config.gpio_strength = GPIO_STRENGTH_LOW_FAST;
//	
//	/* user can modify interrupt handle here */
//	gpio_settings_config.gpio_interrupt_cfg_info.int_handle = data_flash_erase_exception_power_down_handle;
//	gpio_settings_config.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_RISING_EDGE;
//	gpio_settings_config.gpio_interrupt_cfg_info.priority = 1U;
//	
//	gpio_init(&gpio_settings_config);
}

static void code_flash_write_exception_gpio_a1_input_init(void)
{
//	gpio_settings_config_t gpio_settings_config;
//	
//	/* user can modify info here */
//	gpio_settings_config.gpio_port = GPIO_A;
//	gpio_settings_config.gpio_pin = GPIO_PIN_1;
//	gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_1;
//	gpio_settings_config.gpio_dir = GPIO_DIR_INPUT;
//	gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
//	gpio_settings_config.gpio_strength = GPIO_STRENGTH_LOW_FAST;
//	
//	/* user can modify interrupt handle here */
//	gpio_settings_config.gpio_interrupt_cfg_info.int_handle = code_flash_write_exception_power_down_handle;
//	gpio_settings_config.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_RISING_EDGE;
//	gpio_settings_config.gpio_interrupt_cfg_info.priority = 1U;
//	
//	gpio_init(&gpio_settings_config);
}

static void data_flash_write_exception_gpio_a1_input_init(void)
{
//	gpio_settings_config_t gpio_settings_config;
//	
//	/* user can modify info here */
//	gpio_settings_config.gpio_port = GPIO_A;
//	gpio_settings_config.gpio_pin = GPIO_PIN_1;
//	gpio_settings_config.gpio_alt = GPIO_ALT_FUNC_1;
//	gpio_settings_config.gpio_dir = GPIO_DIR_INPUT;
//	gpio_settings_config.pull_cfg = GPIO_PULL_DISABLE;
//	gpio_settings_config.gpio_strength = GPIO_STRENGTH_LOW_FAST;
//	
//	/* user can modify interrupt handle here */
//	gpio_settings_config.gpio_interrupt_cfg_info.int_handle = data_flash_write_exception_power_down_handle;
//	gpio_settings_config.gpio_interrupt_cfg_info.interrupt_type = GPIO_INT_TYPE_RISING_EDGE;
//	gpio_settings_config.gpio_interrupt_cfg_info.priority = 1U;
//	
//	gpio_init(&gpio_settings_config);
}



void code_flash_erase_exception_power_down(void)
{
	gpio_clk_init(GPIO_A);
	gpio_clk_init(GPIO_C);
	gpio_a0_output_init();
	code_flash_erase_exception_gpio_a1_input_init();
	gpio_c0_init();
}

void data_flash_erase_exception_power_down(void)
{
	gpio_clk_init(GPIO_A);
	gpio_clk_init(GPIO_C);
	gpio_a0_output_init();
	data_flash_erase_exception_gpio_a1_input_init();
	gpio_c0_init();
}

void code_flash_write_exception_power_down(void)
{
	gpio_clk_init(GPIO_A);
	gpio_clk_init(GPIO_C);
	gpio_a0_output_init();
	code_flash_write_exception_gpio_a1_input_init();
	gpio_c0_init();
}

void data_flash_write_exception_power_down(void)
{
	gpio_clk_init(GPIO_A);
	gpio_clk_init(GPIO_C);
	gpio_a0_output_init();
	data_flash_write_exception_gpio_a1_input_init();
	gpio_c0_init();
}















