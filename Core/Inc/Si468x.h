/*
 * Si468x.h
 *
 *  Created on: Aug 22, 2022
 *      Author: mgostek
 */

#ifndef INC_SI468X_H_
#define INC_SI468X_H_

#define SPI_TX_BUFFER_SIZE 4096
#define SPI_RX_BUFFER_SIZE 2048

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

#define TUNE_TIMEOUT_MS 3000

#define IMAGE_DAB_6_0_9_START_ADDR 0x6000
#define IMAGE_DAB_4_0_5_START_ADDR 0x106000
#define IMAGE_FM_4_0_12_START_ADDR 0x86000

//Default DAB Channels

//#define CH_5A 0   // 174928 kHz
//#define CH_5B 1   // 176640 kHz
//#define CH_5C 2   // 178352 kHz
//#define CH_5D 3   // 180064 kHz
//#define CH_6A 4   // 181936 kHz
//#define CH_6B 5   // 183648 kHz
//#define CH_6C 6   // 185360 kHz
//#define CH_6D 7   // 187072 kHz
//#define CH_7A 8   // 188928 kHz
//#define CH_7B 9   // 190640 kHz
//#define CH_7C 10   // 192352 kHz
//#define CH_7D 11   // 194064 kHz
//#define CH_8A 12   // 195936 kHz
//#define CH_8B 13   // 197648 kHz
//#define CH_8C 14   // 199360 kHz
//#define CH_8D 15   // 201072 kHz
//#define CH_9A 16   // 202928 kHz
//#define CH_9B 17   // 204640 kHz
//#define CH_9C 18   // 206352 kHz
//#define CH_9D 19   // 208064 kHz
//#define CH_10A 20   // 209936 kHz
//#define CH_undef_1 21   // 210096 kHz
//#define CH_10B 22   // 211648 kHz
//#define CH_10C 23   // 213360 kHz
//#define CH_10D 24   // 215072 kHz
//#define CH_11A 25   // 216928 kHz
//#define CH_undef_2 26   // 217088 kHz
//#define CH_11B 27   // 218640 kHz
//#define CH_11C 28   // 220352 kHz
//#define CH_11D 29   // 222064 kHz
//#define CH_12A 30   // 223936 kHz
//#define CH_undef_3 31   // 224096 kHz
//#define CH_12B 32   // 225648 kHz
//#define CH_12C 33   // 227360 kHz
//#define CH_12D 34   // 229072 kHz
//#define CH_13A 35   // 230784 kHz
//#define CH_13B 36   // 232496 kHz
//#define CH_13C 37   // 234208 kHz
//#define CH_13D 38   // 235776 kHz
//#define CH_13E 39   // 237488 kHz
//#define CH_13F 40   // 239200 kHz

typedef uint8_t RETURN_CODE;

typedef struct{
	uint8_t subchannel_id;
	uint8_t tm_id;
}dab_component_t;

typedef struct{
	uint8_t name[24];
	uint8_t pd_flag;
	uint8_t p_ty;
	uint32_t srv_ref;
	uint8_t country_id;
	uint32_t service_id;
	uint32_t freq;
	uint8_t freq_id;
	uint8_t number_of_components;
	dab_component_t components[3];
}dab_service_t;

typedef struct{
	uint8_t rssi;
	uint8_t snr;
	uint16_t fib_err_cnt;
	uint8_t valid;
	uint8_t acq;
	uint8_t acq_int;
	uint8_t fic_q;
	uint32_t fic_bit_cnt;
	uint32_t fic_err_cnt;
}sig_metrics_t;

typedef struct{
	uint16_t id;
	uint8_t label[17];
	uint32_t freq;
	uint8_t freq_id;
}dab_ensemble_t;



void Si468x_init();
void Si468x_reset();
void Si468x_disable();
void Si468x_power_up();

void Si468x_load_init();
void Si468x_bootloader_load_host();
void Si468x_firmware_load_flash(uint32_t start_address);
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

void Si468x_dab_get_freq_list();
void Si468x_dab_tune_freq(uint8_t channel);
void Si468x_dab_reset_interrupts();
void Si468x_dab_digrad_status();
void SI468x_dab_get_digital_service_list();
void Si468x_dab_start_digital_service(uint32_t service_id, uint32_t component_id);
uint8_t Si468x_dab_get_ensemble_info();
void Si468x_dab_full_scan();
void Si468x_dab_get_audio_info();
void Si468x_dab_get_event_status();
void Si468x_dab_test_get_ber_info();



void Si468x_dab_get_time();

void Si468x_play_next_station();


#endif /* INC_SI468X_H_ */
