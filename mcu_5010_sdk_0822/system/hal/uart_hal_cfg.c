/*
 * Copyright (c) silergy Corp .
 * All rights reserved.
 */
 
/*!
 * @file uart_hal_cfg.c
 */

#include "platform_common.h"
#include "uart_hal.h"



uart_user_config_t uart1_init_config = {
  .baud_rate = 2000000U,
  .parity_mode =    UART_PARITY_DISABLED,
  .stop_bit_number = UART_ONE_STOP_BIT,
  .bit_number_per_char = UART_8_BITS_PER_CHAR,
  .tx_bit_parity = UART_POLARITY_NORMAL,
  .rx_bit_parity = UART_POLARITY_NORMAL,
  .bit_order = UART_LSB_FIRST,
  .autobaud_en = false,
  .tx_interrupt_priority = 0U,
  .rx_interrupt_priority = 0U,
};

uart_user_config_t uart0_init_config = {
  .baud_rate = 115200U,
  .parity_mode = UART_PARITY_DISABLED,
  .stop_bit_number = UART_ONE_STOP_BIT,
  .bit_number_per_char = UART_8_BITS_PER_CHAR,
  .tx_bit_parity = UART_POLARITY_NORMAL,
  .rx_bit_parity = UART_POLARITY_NORMAL,
  .bit_order = UART_LSB_FIRST,
  .autobaud_en = false,
  .tx_interrupt_priority = 0U,
  .rx_interrupt_priority = 0U,
};

