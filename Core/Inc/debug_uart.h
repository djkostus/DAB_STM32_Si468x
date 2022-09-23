#ifndef INC_DEBUG_UART_H_
#define INC_DEBUG_UART_H_

#define CRLF_SEND 1
#define CRLF_NO_SEND 0

void send_debug_msg(char* msg, uint8_t is_crlf);//void send_debug_msg(char msg[], uint8_t is_crlf);

void send_debug_data(uint8_t data);



#endif /* INC_DEBUG_UART_H_ */
