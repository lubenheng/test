#ifndef CSA_TYPE_H
#define CSA_TYPE_H

//#include "stdint.h"

#define CSA_FIXED_CH_MODE               (0x00)
#define CSA_AUTO_DIFF_MODE              (0x01)
#define CSA_AUTO_SINGLE_MODE            (0x02)

#define CSA_FIXED_INPUT_CH0							(0x00)
#define CSA_FIXED_INPUT_CH1							(0x01)
#define CSA_FIXED_INPUT_CH2							(0x02)
#define CSA_FIXED_INPUT_CH3							(0x03)

#define CSA_PGA_GAIN_X1							(0x00)
#define CSA_PGA_GAIN_X2							(0x01)
#define CSA_PGA_GAIN_X4							(0x02)
#define CSA_PGA_GAIN_X8							(0x03)
#define CSA_PGA_GAIN_X12							(0x04)
#define CSA_PGA_GAIN_X16							(0x05)
#define CSA_PGA_GAIN_X32							(0x06)
#define CSA_PGA_GAIN_X64							(0x07)

/* CSA 偏置电压基准选择 (RW_CSA_REF bit[4:2], bit[4]=1 时基准为 vdd33) */
#define CSA_REF_1V5				(0x00 << 2)
#define CSA_REF_2V				(0x01 << 2)
#define CSA_REF_2V5				(0x02 << 2)
#define CSA_REF_3V				(0x03 << 2)
#define CSA_REF_VDD33			(0x04 << 2)

/* CSA 偏置电压分压选择 (RW_CSA_REF bit[1:0]) */
#define CSA_REF_DIV8			(0x00)
#define CSA_REF_DIV6			(0x01)
#define CSA_REF_DIV4			(0x02)
#define CSA_REF_DIV2			(0x03)

/* 组合宏: 生成 CSA 偏置电压配置值 (偏置电压 = 基准 / 分压) */
#define CSA_REF_BUILD(base, div)	((base) | (div))



#endif
