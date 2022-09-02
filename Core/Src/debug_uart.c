
#include "usart.h"
#include "debug_uart.h"

void send_debug_msg(char* msg, uint8_t is_crlf)
{
	uint8_t msg_size = strlen(msg);
	HAL_UART_Transmit(&huart3, msg, msg_size, 100);

	if(is_crlf)
	{
		uint8_t crlf[] = {13, 10};
		HAL_UART_Transmit(&huart3, crlf, 2, 100);
	}
}
