#ifndef TRIM_TEST_H
#define TRIM_TEST_H

#include <stdint.h>

void VBG_Trim_Init(void);
void VBG_Trim_Set(uint8_t trim_value);
void VREF_Trim_Init(void);
void VREF_Trim_Set(uint8_t trim_value);
void SLP_VREF_Trim_Init(void);
void SLP_VREF_Trim_Set(uint8_t trim_value);
void LDO12_Trim_Init(void);
void LDO12_Trim_Set(uint8_t trim_value);
void SLP_LDO12_Trim_Init(void);
void SLP_LDO12_Trim_Set(uint8_t trim_value);
void ADC_VREFH_Trim_Init(void);
void ADC_VREFH_Trim_Set(uint8_t trim_value);
void HSI_Trim_Init(void);
void HSI_Trim_Set(uint8_t trim_value);
void LSI_Trim_Init(void);
void LSI_Trim_Set(uint8_t trim_value);
void trim_test_end(void);
uint8_t read_test_number(void);

#endif

