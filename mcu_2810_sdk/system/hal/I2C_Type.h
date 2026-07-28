#ifndef _I2C_TYPE_H
#define _I2C_TYPE_H
#include "stdint.h"
#include "I2C_reg.h"

 


/* 状态枚举 */
typedef enum {
    HAL_I2C_STATE_RESET             = 0x00,
    HAL_I2C_STATE_READY             = 0x01,
    HAL_I2C_STATE_BUSY              = 0x02,
    HAL_I2C_STATE_MASTER_TX         = 0x03,
    HAL_I2C_STATE_MASTER_RX         = 0x04,
    HAL_I2C_STATE_SLAVE_TX          = 0x05,
    HAL_I2C_STATE_SLAVE_RX          = 0x06
} HAL_I2C_StateTypeDef;

/* 错误枚举 */
typedef enum {
    HAL_I2C_ERROR_NONE              = 0x00,
    HAL_I2C_ERROR_NAK               = 0x01
} HAL_I2C_ErrorTypeDef;

/* I2C 句柄结构体 */
typedef struct {
    I2C_TypeDef                  *Instance;
    uint32_t                     ClockSpeed;
    uint8_t                      OwnAddress;
    uint32_t                     MemSize;
    uint8_t                      *pMem;
    HAL_I2C_StateTypeDef         State;
    HAL_I2C_ErrorTypeDef         ErrorCode;
} HAL_I2C_HandleTypeDef;


typedef enum{
	HAL_ERROR,
	HAL_OK
}HAL_StatusTypeDef;

 
#endif  // _I2C_TYPE_H

