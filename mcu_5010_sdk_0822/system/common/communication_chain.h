#ifndef COMMUNICATION_CHAIN_H
#define COMMUNICATION_CHAIN_H

#define 					BASIC_ON_UART
//#define 					BASIC_ON_SPI
//#define MOD(x)				((x)%(g_kfifo.buffer_size))

typedef void (*communication_cmd_handle_t)(uint8_t *cmd_buffer, uint32_t cmd_total_len);
typedef uint32_t (*communication_cmd_header_handle_t)(uint8_t *cmd_buffer, uint32_t cmd_total_len);


void communication_chain_device_init(uint8_t instance, uint8_t *buffer_rx, uint32_t buffer_size);
void communication_receive_random_length_command(uint8_t cmd_header_len, uint8_t * buffer_store, communication_cmd_handle_t cmd_handle, communication_cmd_header_handle_t cmd_header_handle);
void communication_receive_fixed_length_command(uint8_t cmd_len, uint8_t * buffer_store, communication_cmd_handle_t cmd_handle);

typedef struct
{
	uint8_t *buffer;
	uint32_t buffer_size;
	uint32_t wd_index;
	uint32_t rd_index;
}kfifo_t;

#endif /*COMMUNICATION_CHAIN_H*/


