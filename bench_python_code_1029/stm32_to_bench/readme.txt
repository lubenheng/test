该代码是继电器测试板的主控芯片STM32G431的控制程序，继电器测试板烧录该代码，连接串口即可进行指令交互。
有如下指令：

74HC595D(继电器切换)：
	cmd_gpio_set_595d_test reset		---关闭所有继电器
	cmd_gpio_set_595d_test set 6		---接通bit6继电器
	cmd_gpio_set_595d_test set 31 36	---接通bit31、bit36继电器
	cmd_gpio_set_595d_test set 6 7 0 28 31 32 33 36
									---接通bit6 bit7 bit0 bit28 bit31 bit32 bit33 bit36继电器

DAC输出电压指令：
	spi_cmd_set_val mV 1000			---输出1000mV电压
	spi_cmd_dac_output_set Ori 32768	---输出4095mV电压的一半


示例（第二版继电器阵列板）：我需要将DAC输出的电压通过万用表测量出来：
	使用万用表连接线连接继电器板上的接口，万用表连接线另一端连接万用表。
	1. 发送指令接通bit38（万用表）和bit6（DAC一倍增益输出）。		cmd_gpio_set_595d_test set 6 38
	2. 发送指令输出2000mV电压									spi_cmd_set_val mV 2000