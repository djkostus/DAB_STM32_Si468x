/*
 * Si468x.h
 *
 *  Created on: Aug 22, 2022
 *      Author: mgostek
 */

#ifndef INC_SI468X_H_
#define INC_SI468X_H_

#define SPI_TX_BUFFER_SIZE 4096
#define SPI_RX_BUFFER_SIZE 64

#define RESET_PIN_HIGH HAL_GPIO_WritePin(DAB_GPIO_RESET_GPIO_Port, DAB_GPIO_RESET_Pin, 1)
#define RESET_PIN_LOW HAL_GPIO_WritePin(DAB_GPIO_RESET_GPIO_Port, DAB_GPIO_RESET_Pin, 0)

#define CS_PIN_LOW HAL_GPIO_WritePin(DAB_GPIO_CS_GPIO_Port, DAB_GPIO_CS_Pin, 0)
#define CS_PIN_HIGH HAL_GPIO_WritePin(DAB_GPIO_CS_GPIO_Port, DAB_GPIO_CS_Pin, 1)

#define SUCCESS 0x00
#define HAL_ERROR		0x01
#define INVALID_INPUT	0x02
#define INVALID_MODE	0x04
#define TIMEOUT			0x08
#define COMMAND_ERROR	0x20
#define UNSUPPORTED_FUNCTION	0x80

#define POLL_TIMEOUT_MS 1000

typedef uint8_t RETURN_CODE;


void Si468x_init();
void Si468x_reset();
void Si468x_disable();
void Si468x_power_up();

void Si468x_load_init();
void Si468x_bootloader_load_host();
void Si468x_firmware_load_flash();
void Si468x_boot();

void Si468x_write_single_byte(uint8_t byte_to_write);
uint8_t Si468x_read_single_byte();

RETURN_CODE Si468x_wait_for_CTS(uint16_t timeout);

void Si468x_write_multiple(uint16_t len, uint8_t* data_to_write);
void Si468x_read_multiple(uint16_t len, uint8_t* read_buffer);

RETURN_CODE Si468x_write_command (uint16_t length, uint8_t *buffer);
RETURN_CODE Si468x_read_reply(uint16_t length, uint8_t *buffer);

uint8_t get_rx_buffer(int buffer_pos);

void Si468x_get_sys_state();
void Si468x_get_part_info();

void Si468x_set_property(uint16_t property_id, uint16_t property_value);
uint16_t Si468x_get_property(uint16_t property_id);


#endif /* INC_SI468X_H_ */
