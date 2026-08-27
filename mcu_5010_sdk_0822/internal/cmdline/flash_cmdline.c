#include "platform_common.h"
#include "gpio_hal.h"
#include "pmu_hal.h"
#include "gpio_reg.h"
#include "flash_test.h"
#include "cmdline_internal.h"



int cmd_anac_read(int argc, char *argv[])
{
	int ret = 0;
	
	ret = anac_read(argc,argv);

	if(ret == 0)
	{
		INFO("cmd_anac_read PASS\n");
	}
	else
	{
		INFO("cmd_anac_read FAIL\n");
	}

	return 0;
}



int cmd_syscfg_read(int argc, char *argv[])
{
	int ret = 0;
	
	ret = syscfg_read(argc,argv);

	if(ret == 0)
	{
		INFO("cmd_syscfg_read PASS\n");
	}
	else
	{
		INFO("cmd_syscfg_read FAIL\n");
	}

	return 0;

}



int cmd_flash_write_test(int argc, char *argv[])
{
	int ret = 0;

	ret = flash_write_test(argc,argv);
	
	if(ret == 0)
	{
		INFO("cmd_flash_write_test PASS\n");
	}
	else
	{
		INFO("cmd_flash_write_test FAIL\n");
	}
		
	return 0;
}
int cmd_flash_read_test(int argc, char *argv[])
{
	int ret = 0;
	
	ret = flash_read_test(argc,argv);

	if(ret == 0)
	{
		INFO("cmd_flash_read_test PASS\n");
	}
	else
	{
		INFO("cmd_flash_read_test FAIL\n");
	}

	return 0;
}


int cmd_flash_info_user_write_test(int argc, char *argv[])
{
	int ret = 0;

	ret = flash_info_user_write_test(argc,argv);
	
	if(ret == 0)
	{
		INFO("cmd_flash_info_user_write_test PASS\n");
	}
	else
	{
		INFO("cmd_flash_info_user_write_test FAIL\n");
	}
		
	return 0;
}
int cmd_flash_info_user_read_test(int argc, char *argv[])
{
	int ret = 0;
	
	ret = flash_info_user_read_test(argc,argv);

	if(ret == 0)
	{
		INFO("cmd_flash_info_user_read_test PASS\n");
	}
	else
	{
		INFO("cmd_flash_info_user_read_test FAIL\n");
	}

	return 0;
}


int cmd_flash_info_vendor_write_test(int argc, char *argv[])
{
	int ret = 0;

	ret = flash_info_vendor_write_test(argc,argv);
	
	if(ret == 0)
	{
		INFO("cmd_flash_info_vendor_write_test PASS\n");
	}
	else
	{
		INFO("cmd_flash_info_vendor_write_test FAIL\n");
	}
		
	return 0;
}
int cmd_flash_info_vendor_read_test(int argc, char *argv[])
{
	int ret = 0;
	
	ret = flash_info_vendor_read_test(argc,argv);

	if(ret == 0)
	{
		INFO("cmd_flash_info_vendor_read_test PASS\n");
	}
	else
	{
		INFO("cmd_flash_info_vendor_read_test FAIL\n");
	}

	return 0;
}


int cmd_flash_otp_write_test(int argc, char *argv[])
{
	int ret = 0;

	ret = flash_otp_write_test(argc,argv);
	
	if(ret == 0)
	{
		INFO("cmd_flash_otp_write_test PASS\n");
	}
	else
	{
		INFO("cmd_flash_otp_write_test FAIL\n");
	}
		
	return 0;
}
/* 通过映射寄存器进行数据读取 */
int cmd_flash_otp_read_test(int argc, char *argv[])
{
	int ret = 0;
	
	ret = flash_otp_read_test(argc,argv);

	if(ret == 0)
	{
		INFO("cmd_flash_otp_read_test PASS\n");
	}
	else
	{
		INFO("cmd_flash_otp_read_test FAIL\n");
	}

	return 0;
}


int cmd_flash_otp_reboot_test(int argc, char *argv[])
{
	flash_otp_reboot_test(argc,argv);
	return 0;
}

int cmd_flash_performance_test(int argc, char *argv[])
{
		uint32_t flash_type = get_arg_ulong(argv[1]);
		uint32_t test_type 	= get_arg_ulong(argv[2]);
		uint32_t cmp1us	= get_arg_ulong(argv[3]);
		uint32_t accessmode	= get_arg_ulong(argv[4]);
		if (flash_type == 0) /* data flash */
		{
			data_flash_performance_test(test_type, cmp1us, accessmode);
		}
		else if(flash_type == 1) /* code flash */
		{
			code_flash_performance_test(test_type, cmp1us, accessmode);
		}
		else
		{
			INFO("Flash type error!!\n");
		}
	
		return ERR_TEST_DONE;	
}
	
int cmd_flash_deepsleep_test(int argc, char *argv[])
{  
#if 0	
		uint32_t flash_type = get_arg_ulong(argv[1]);
		uint32_t dpd_test_type 	= get_arg_ulong(argv[2]);
		uint32_t select_value = 0;
		uint32_t flash_base_address;
	
	
		if (flash_type == 0) /* code flash */
		{
			flash_base_address = 0x40007018; 
		}
		else if(flash_type == 1)
		{
			flash_base_address = 0x40008018;
		}
		else
		{
			INFO("input param flash_type error!!!\n");
			return ERR_TEST_FAIL;
		}
		
		if ((dpd_test_type == 0) || (dpd_test_type == 1))
		{
			/* don't enter deepsleep*/
			select_value = ((REG32(flash_base_address) & (~(3 << 21))));
			WRITE_REG_32(flash_base_address, select_value);
		}
		else if (dpd_test_type == 2)
		{
			/* enter deepsleep*/
			select_value = (REG32(flash_base_address) & (~(3 << 21)) & (~(1 << 2)));
			select_value = (select_value | (1 << 21));
			WRITE_REG_32(flash_base_address, select_value);
		}
		else if (dpd_test_type == 3)
		{
			/* enter sleep*/
			select_value = (REG32(flash_base_address) & (~(3 << 21)) & (~(1 << 2)));
			select_value = (select_value | (1 << 22));
			WRITE_REG_32(flash_base_address, select_value);
		}
		else if (dpd_test_type == 4)
		{
			/* enter sleep & fast wakeup*/
			select_value = (REG32(flash_base_address) & (~(3 << 21)) & (~(1 << 2)));
			select_value = (select_value | (1 << 22) | (1 << 2));
			WRITE_REG_32(flash_base_address, select_value);
		}
		else
		{
			INFO("input param dpd_test_type error!!!\n");
			return ERR_TEST_FAIL;
		}
		
		if ((dpd_test_type == 0) || (dpd_test_type == 2))
		{
			pmu_deepsleep_now();
		}
		else if ((dpd_test_type == 1) || (dpd_test_type == 3) || (dpd_test_type == 4))
		{
			pmu_sleep_now();
		}
		
		return ERR_TEST_DONE;	
#endif

	return 0;
}


int cmd_flash_write_operator_interrupt_test(int argc, char *argv[])
{
	uint32_t test_sector = get_arg_ulong(argv[1]);
	flash_write_operation_and_interrupt_test(test_sector);

	return ERR_TEST_DONE;	
}

int cmd_flash_erase_operator_interrupt_test(int argc, char *argv[])
{
	uint32_t test_sector = get_arg_ulong(argv[1]);
	flash_erase_operation_and_interrupt_test(test_sector);

	return ERR_TEST_DONE;	
}


int cmd_flash_erase_exception_power_down_test(int argc, char *argv[])
{
	uint32_t flash_type = get_arg_ulong(argv[1]);
	if (flash_type == 0)
	{
		code_flash_erase_exception_power_down();
	}
	else
	{
		data_flash_erase_exception_power_down();
	}
	
	return ERR_TEST_DONE;	
}

int cmd_flash_write_exception_power_down_test(int argc, char *argv[])
{
	uint32_t flash_type = get_arg_ulong(argv[1]);
	if (flash_type == 0)
	{
		code_flash_write_exception_power_down();
	}
	else
	{
		data_flash_write_exception_power_down();
	}
	
	return ERR_TEST_DONE;	
}



















