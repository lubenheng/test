#include "spi.h"
#include "utility.h" 
#include "cmdline_internal.h"
#include "spi_cmdline.h"
#include <string.h>


// 定义 SYNC 引脚（请替换为你的实际 GPIO 端口和引脚）
#define DAC8551_SYNC_PORT   SPI2_CSN_GPIO_Port
#define DAC8551_SYNC_PIN    SPI2_CSN_Pin


#define HC595D_NUM_MAX 5
uint8_t hc595d_data[HC595D_NUM_MAX] = {0};

#define HC595_Port GPIOC
#define HC595_SER GPIO_PIN_13
#define HC595_RCK GPIO_PIN_14
#define HC595_SCK GPIO_PIN_15

// 参考电压 (mV)
#define DAC8551_VREF_MV     4096

// 最大 DAC 值
#define DAC8551_MAX_VALUE   65535

// 实测固定零点偏移（mV），用于软件校准
#define DAC8551_OFFSET_MV   6

/**
 * @brief DAC8551 初始化（主要确保 SYNC 引脚初始状态为高）
 */
void DAC8551_Init(void)
{
    // 拉高 SYNC，准备开始通信
    HAL_GPIO_WritePin(DAC8551_SYNC_PORT, DAC8551_SYNC_PIN, GPIO_PIN_SET);
    // 可选：增加一点延时
    HAL_Delay(1);
}

/**
 * @brief 向 DAC8551 写入 16 位原始数据
 * @param value 0 ~ 65535
 */
void DAC8551_Write(uint16_t value)
{
    uint8_t data[3];

    // 构造 24 位数据帧：前 8 位为 0（6 个无用位 + PD1=0, PD0=0）
    data[0] = 0x00;
    data[1] = (uint8_t)(value >> 8);   // 高 8 位
    data[2] = (uint8_t)(value & 0xFF); // 低 8 位

    // 拉低 SYNC，开始一次传输
    HAL_GPIO_WritePin(DAC8551_SYNC_PORT, DAC8551_SYNC_PIN, GPIO_PIN_RESET);

    // 发送 3 个字节（24 个时钟）
    HAL_SPI_Transmit(&hspi2, data, 3, HAL_MAX_DELAY);

    // 拉高 SYNC，DAC 在第 24 个时钟下降沿更新输出
    HAL_GPIO_WritePin(DAC8551_SYNC_PORT, DAC8551_SYNC_PIN, GPIO_PIN_SET);
}

/**
 * @brief 根据目标电压（mV）设置 DAC 输出
 * @param mV 目标电压，范围 0 ~ VREF_MV (4096 mV)
 */
void DAC8551_SetVoltage_mV(uint16_t mV)
{
    uint16_t value;
    int32_t target_mV = (int32_t)mV - DAC8551_OFFSET_MV;

    if (target_mV < 0) {
        target_mV = 0;
    }

    if (target_mV >= DAC8551_VREF_MV) {
        value = DAC8551_MAX_VALUE;
    } else {
        // 计算公式：value = (target_mV * 65536) / VREF_MV
        // 使用 32 位运算避免溢出
        value = (uint32_t)target_mV * DAC8551_MAX_VALUE / DAC8551_VREF_MV;
    }

    DAC8551_Write(value);
}

int spi_cmd_set_val(int argc, char *argv[])
{
	uint16_t case1 = get_arg_ulong(argv[2]);
    if(!strncmp(argv[1], "V", 1)){
        DAC8551_SetVoltage_mV(case1 * 1000);
    } else if(!strncmp(argv[1], "mV", 2)){
        DAC8551_SetVoltage_mV(case1);
    } else if(!strncmp(argv[1], "Ori", 3)){
        DAC8551_Write(case1);
    } else {
        INFO_e("input error\n");
        return -1;
    }
	INFO_e("input pass\n");
	return 0;	
}





void hc595d_set_data(uint8_t *data)
{
	int8_t i = 0, j = 0;
	for(i = HC595D_NUM_MAX - 1; i >= 0; i--)
	{
		for(j = 7; j >= 0; j--)
		{
			if(((*(data + i))>>j) & 0x01)
			{
				HAL_GPIO_WritePin(HC595_Port, HC595_SER, GPIO_PIN_SET);
			}
			else
			{
				HAL_GPIO_WritePin(HC595_Port, HC595_SER, GPIO_PIN_RESET);
			}
			HAL_Delay(1);
			HAL_GPIO_WritePin(HC595_Port, HC595_SCK, GPIO_PIN_SET);
			HAL_Delay(1);
			HAL_GPIO_WritePin(HC595_Port, HC595_SCK, GPIO_PIN_RESET);
			HAL_Delay(1);
		}
	}
	
	HAL_Delay(1);
	HAL_GPIO_WritePin(HC595_Port, HC595_SER, GPIO_PIN_RESET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(HC595_Port, HC595_RCK, GPIO_PIN_SET);
	HAL_Delay(2);
	HAL_GPIO_WritePin(HC595_Port, HC595_RCK, GPIO_PIN_RESET);
}

void hc595d_reset(void)
{
	memset(hc595d_data, 0, sizeof(hc595d_data));
	hc595d_set_data(hc595d_data);
}

uint8_t hc595d_data_change(uint8_t *data, uint8_t pos)
{
	if(pos >= HC595D_NUM_MAX * 8)
	{
		return 0;
	}
	
	uint8_t byte_pos = pos / 8;
	uint8_t bit_pos = pos % 8;
	
	*(data + byte_pos) ^= (1 << bit_pos);
	
	return 1;
	
}

int cmd_gpio_set_595d_test(int argc, char *argv[])
{
	uint8_t data = 0;
	uint8_t i = 0;
	memset(hc595d_data, 0, sizeof(hc595d_data));
	/* add your code */
	if(strstr(argv[1], "reset") != NULL){
		hc595d_reset();
	} else if(strstr(argv[1], "set") != NULL){
		hc595d_reset();
		HAL_Delay(500);
		for(i = 2; i < argc; i++)
		{
			data = get_arg_ulong(argv[i]);
			hc595d_data_change(hc595d_data, data);
		}
		
		hc595d_set_data(hc595d_data);
	} else {
		INFO_e("Invalid command\n");
		return -1;
	}
	
	INFO_e("PASS\n");
	return 0;

}

