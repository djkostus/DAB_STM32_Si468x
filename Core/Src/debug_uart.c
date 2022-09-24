
#include "usart.h"
#include "debug_uart.h"
#include <string.h>


void send_debug_msg(char* msg, uint8_t is_crlf)//void send_debug_msg(char msg[], uint8_t is_crlf)
{
	uint8_t msg_size = strlen(msg);
	HAL_UART_Transmit(&huart3, (uint8_t*)msg, msg_size, 100);

	if(is_crlf)
	{
		uint8_t crlf[] = {13, 10};
		HAL_UART_Transmit(&huart3, crlf, 2, 100);
	}
}

void send_debug_data(uint8_t data)
{
	HAL_UART_Transmit(&huart3, data, 1, 100);
}
