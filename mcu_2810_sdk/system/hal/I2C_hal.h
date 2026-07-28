#ifndef _I2C_HAL_H
#define _I2C_HAL_H
#include "stdint.h"
#include "I2C_Type.h"

extern HAL_I2C_HandleTypeDef hi2c0;
extern HAL_I2C_HandleTypeDef hi2c2;

/* º¯ÊýÉùÃ÷ */
void HAL_I2C_Init(HAL_I2C_HandleTypeDef *hi2c);
void HAL_I2C_MspInit(HAL_I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HAL_I2C_Master_Transmit(HAL_I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t *pData, uint16_t len);
HAL_StatusTypeDef HAL_I2C_SafeMemWrite(HAL_I2C_HandleTypeDef *hi2c, uint8_t addr, uint8_t data);
uint8_t HAL_I2C_SafeMemRead(HAL_I2C_HandleTypeDef *hi2c, uint8_t addr);

HAL_StatusTypeDef HAL_I2C_Master_Transmit_Long(HAL_I2C_HandleTypeDef *hi2c, uint8_t dev_addr, uint8_t *pData, uint16_t len);
HAL_StatusTypeDef HAL_I2C_Master_Transmit_One(HAL_I2C_HandleTypeDef *hi2c, uint8_t dev_addr, uint8_t *pData, uint16_t len);
HAL_StatusTypeDef HAL_I2C_Master_Receive(HAL_I2C_HandleTypeDef *hi2c, uint8_t dev_addr, uint8_t *pData, uint16_t len);


void HAL_I2C_Slave_Transmit(HAL_I2C_HandleTypeDef *hi2c, uint8_t *pData);
void HAL_I2C_Slave_Receive(HAL_I2C_HandleTypeDef *hi2c, uint8_t *pData,uint16_t *len);

#endif 


