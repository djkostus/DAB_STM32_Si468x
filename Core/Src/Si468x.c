/*
 * Si468x.c
 *
 *  Created on: Aug 22, 2022
 *      Author: mgostek
 */

#include "main.h"
#include "spi.h"
#include "i2c.h"
#include "Si468x.h"
#include "Si468x_defs.h"
#include "patch_mini.h"
#include "patch_full.h"
#include "debug_uart.h"

#define BOOT_WRITE_STEPS 3

uint8_t dab_spi_tx_buffer[SPI_TX_BUFFER_SIZE];
uint8_t dab_spi_rx_buffer[SPI_RX_BUFFER_SIZE];

uint32_t freq_table[47];

RETURN_CODE status = 0;
char itoa_buffer[32];

uint8_t rssi = 0;
uint8_t snr = 0;
uint16_t fib_error_count = 0;
uint8_t valid = 0;
uint8_t acq = 0;
uint8_t acq_int = 0;
uint8_t fic_quality = 0;

uint32_t fic_bit_cnt = 0;
uint32_t fic_err_cnt = 0;

uint8_t started = 0;


void Si468x_init()
{
	send_debug_msg("---------------------------------Si468x init---------------------------------", CRLF_SEND);
	Si468x_reset();
	Si468x_power_up();
	Si468x_bootloader_load_host();
	Si468x_firmware_load_flash(IMAGE_DAB_4_0_5_START_ADDR);
	Si468x_boot();
	Si468x_get_part_info();
	Si468x_get_sys_state();
	Si468x_set_property(SI468x_PROP_DAB_TUNE_FE_CFG, 0x00); //włączanie lub wyłącznie switcha front-end, prawdopodobnie dla dab dać 0x00, dla FM 0x01
	Si468x_set_property(SI468x_PROP_DAB_TUNE_FE_VARM, 0xF8A9); //dla DAB 0xF8A9 lub 0xF784, sprawdzic jak lepiej dziala. Dla FM 0xEDB5.
	Si468x_set_property(SI468x_PROP_DAB_TUNE_FE_VARB, 0x01C6); //dla DAB 0x01C6 lub 0x01D8, sprawdzic jak lepiej dziala. Dla FM 0x01E3.
	Si468x_set_property(SI468x_PROP_DAB_VALID_RSSI_THRESHOLD, 0x05); //prog RSSI od kiedy łapie kanał, default 12
	Si468x_set_property(SI468x_PROP_DAB_XPAD_ENABLE, 0x4005); //określa które featury PAD będą przesyłane do hosta
	Si468x_set_property(SI468x_PROP_DAB_EVENT_MIN_SVRLIST_PERIOD, 0x06); //określa co ile będzie aktualizowana lista usług, x100 ms
	Si468x_set_property(SI468x_PROP_DAB_EVENT_MIN_SVRLIST_PERIOD_RECONFIG, 0x06); //określa co ile będzie aktualizowana lista usług w trakcie rekonfiguracji, x100 ms
	Si468x_set_property(SI468x_PROP_DAB_ACF_ENABLE, 0x01);	//tylko soft mute włączone przy slabym sygnale
	Si468x_set_property(SI468x_PROP_DAB_ACF_MUTE_SIGLOSS_THRESHOLD, 0x05);	//próg wyciszania audio jak sygnal jest utracony, default 0x06
	Si468x_set_property(SI468x_PROP_DAB_ACF_SOFTMUTE_BER_LIMITS, 0xE2C4); //limit BER kiedy soft mute zadziała. Defaultowo 0xE2A6
	Si468x_dab_get_freq_list(); //odczytujemy z ukladu liste czestotliwosci do tablicy
	Si468x_dab_tune_freq(CH_11B);
	Si468x_get_sys_state(); //kontrolnie zeby sprawdzic czy demod dziala

	while(!valid && !acq)
	{
		Si468x_dab_digrad_status();
		HAL_Delay(200);
	}
	Si468x_dab_get_digital_service_list();

//	Si468x_dab_start_digital_service(12966, 0);
//
//	while(1)
//	{
//		Si468x_dab_reset_interrupts();
//		Si468x_dab_digrad_status();
//		if(valid == 1 && acq == 1 && started == 0)
//		{
//			Si468x_dab_start_digital_service(12966, 4);
//			started = 1;
//		}
//		HAL_Delay(500);
//	}
}

void Si468x_reset()
{
	send_debug_msg("Resetting Si468x...", CRLF_SEND);
	RESET_PIN_LOW;
	HAL_Delay(100);
	RESET_PIN_HIGH;
	HAL_Delay(200);
}

void Si468x_disable()
{
	RESET_PIN_HIGH;
}

void Si468x_power_up()
{
	send_debug_msg("---------------------Si468x Power Up---------------------", CRLF_SEND);
	dab_spi_tx_buffer[0]  = SI468X_CMD_POWER_UP;	//Power up command code
	dab_spi_tx_buffer[1]  = 0x80;					//Disable toggling host interrupt line = 0x00; enable = 0x80
	dab_spi_tx_buffer[2]  = 0x17;					//Clock mode 1 (crystal mode), TR_SIZE = 0x7 for 19.2MHz crystal
	dab_spi_tx_buffer[3]  = 0x50;					//start IBIAS = 800 uA (80 * 10) for 19.2MHz Crystal with 250R startup ESR
	dab_spi_tx_buffer[4]  = 0x00;					//crystal frequency in MHz [7:0], 19.2 MHz
	dab_spi_tx_buffer[5]  = 0xF8;	//F9??				//crystal frequency in MHz [15:8], 19.2 MHz
	dab_spi_tx_buffer[6]  = 0x24;					//crystal frequency in MHz [23:16], 19.2 MHz
	dab_spi_tx_buffer[7]  = 0x01;					//crystal frequency in MHz [31:24], 19.2 MHz
	dab_spi_tx_buffer[8]  = 0x00;					//CTUN = 0 (there's no need to add load capacitance)
	dab_spi_tx_buffer[9]  = 0x10;					//not used - value as in documentation
	dab_spi_tx_buffer[10] = 0x00;					//not used - value as in documentation
	dab_spi_tx_buffer[11] = 0x00;					//not used - value as in documentation
	dab_spi_tx_buffer[12] = 0x00;					//not used - value as in documentation
	dab_spi_tx_buffer[13] = 0x28;					//running IBIAS = 400 uA (80 * 10) for 19.2MHz Crystal with 50R running ESR (recommended: start IBIAS = 2 * running IBIAS)
	dab_spi_tx_buffer[14] = 0x00;					//not used - value as in documentation
	dab_spi_tx_buffer[15] = 0x00;					//not used - value as in documentation

	status = Si468x_write_command(16, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(5, dab_spi_rx_buffer);
	send_debug_msg("Si468x powered up successfully!", CRLF_SEND);
}

void Si468x_load_init()
{
	send_debug_msg("Si468x load init CMD...", CRLF_SEND);
	dab_spi_tx_buffer[0]  = SI468X_CMD_LOAD_INIT;	//Load Init command code
	dab_spi_tx_buffer[1]  = 0x00;					//Disable toggling host interrupt line

	status = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(5, dab_spi_rx_buffer);
}

void Si468x_bootloader_load_host()
{
	send_debug_msg("---------Loading bootloader to Si468x from Host----------", CRLF_SEND);
	Si468x_load_init();
	uint16_t patch_size = sizeof(patch_full);

	dab_spi_tx_buffer[0] = SI468X_CMD_HOST_LOAD;
	dab_spi_tx_buffer[1] = 0x00;
	dab_spi_tx_buffer[2] = 0x00;
	dab_spi_tx_buffer[3] = 0x00;

	uint16_t i, j;

	for(i = 0; i < BOOT_WRITE_STEPS; i++)
	{
		for (j = 0; j < patch_size / BOOT_WRITE_STEPS; j++)
		{
			dab_spi_tx_buffer[4 + j] = patch_full[j + (patch_size / BOOT_WRITE_STEPS) * i];
		}
		status = Si468x_write_command(patch_size / BOOT_WRITE_STEPS + 4, dab_spi_tx_buffer);
		HAL_Delay(1);
		status = Si468x_read_reply(5, dab_spi_rx_buffer);

	}
	send_debug_msg("Bootloader loaded successfully!", CRLF_SEND);
}

void Si468x_firmware_load_flash(uint32_t start_address)
{
	send_debug_msg("---------Writing image to Si468x from SPI Flash----------", CRLF_SEND);
	Si468x_load_init();
	dab_spi_tx_buffer[0]  = SI468X_CMD_FLASH_LOAD;				//Power up command code
	dab_spi_tx_buffer[1]  = 0x00;								//not used - value as in documentation
	dab_spi_tx_buffer[2]  = 0x00;								//not used - value as in documentation
	dab_spi_tx_buffer[3]  = 0x00;								//not used - value as in documentation
	dab_spi_tx_buffer[4]  = start_address & 0xFF;				//Flash start address [7:0] 0x6000
	dab_spi_tx_buffer[5]  = start_address >> 8;					//Flash start address [15:8]
	dab_spi_tx_buffer[6]  = start_address >> 16;				//Flash start address [23:16]
	dab_spi_tx_buffer[7]  = start_address >> 24;				//Flash start address [31:24]
	send_debug_msg("Start Address", CRLF_SEND);
	send_debug_msg(itoa(dab_spi_tx_buffer[4], itoa_buffer, 16), CRLF_SEND);
	send_debug_msg(itoa(dab_spi_tx_buffer[5], itoa_buffer, 16), CRLF_SEND);
	send_debug_msg(itoa(dab_spi_tx_buffer[6], itoa_buffer, 16), CRLF_SEND);
	send_debug_msg(itoa(dab_spi_tx_buffer[7], itoa_buffer, 16), CRLF_SEND);

	dab_spi_tx_buffer[8]  = 0x00;								//not used - value as in documentation
	dab_spi_tx_buffer[9]  = 0x00;								//not used - value as in documentation
	dab_spi_tx_buffer[10] = 0x00;								//not used - value as in documentation
	dab_spi_tx_buffer[11] = 0x00;								//not used - value as in documentation

	status = Si468x_write_command(12, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(5, dab_spi_rx_buffer);
	if(dab_spi_rx_buffer[0] & 0x40)
	{
		send_debug_msg("Command Error!", CRLF_SEND);
	}
	else
	{
		send_debug_msg("Image loaded successfully!", CRLF_SEND);
	}
}

void Si468x_boot()
{
	send_debug_msg("---------------------Booting Si468x----------------------", CRLF_SEND);
	dab_spi_tx_buffer[0]  = SI468X_CMD_BOOT;	//Power up command code
	dab_spi_tx_buffer[1]  = 0x00;				//Value as in documentation

	status  = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(5, dab_spi_rx_buffer);
	if((dab_spi_rx_buffer[3] & 0xC0) == 0xC0)
	{
		send_debug_msg("Si468x booted successfully!", CRLF_SEND);
	}
	else
	{
		send_debug_msg("Si468x boot error!", CRLF_SEND);
	}
}

void Si468x_write_single_byte(uint8_t byte_to_write)
{
	HAL_SPI_Transmit(&hspi1, &byte_to_write, 1, 100);
}

uint8_t Si468x_read_single_byte()
{
	uint8_t data_buffer = 0;
	HAL_SPI_Receive(&hspi1, &data_buffer, 1, 100);
	return data_buffer;
}

RETURN_CODE Si468x_wait_for_CTS(uint16_t timeout)
{
	uint16_t i;
	uint8_t cts_status = 0;

	for(i = 0; i < timeout; i++)
	{
		Si468x_read_multiple(1, &cts_status);

		if((cts_status & 0x80) == 0x80)
		{
			if((cts_status & 0x40) == 0x40)
			{
				send_debug_msg("Command Error during waiting for CTS!", CRLF_SEND);
				return COMMAND_ERROR;
			}
			return SUCCESS;
		}

		// delay function for 1 ms
		HAL_Delay(1);
	}
	send_debug_msg("Timeout error!", CRLF_SEND);
	return TIMEOUT;
}

void Si468x_write_multiple(uint16_t len, uint8_t* data_to_write)
{
	uint16_t i = 0;
	CS_PIN_LOW;
	while(len--)
	{
		Si468x_write_single_byte(data_to_write[i++]);
	}
	CS_PIN_HIGH;
}

void Si468x_read_multiple(uint16_t len, uint8_t* read_buffer)
{
	uint16_t i = 0;
	CS_PIN_HIGH;
	HAL_Delay(3);
	CS_PIN_LOW;
	Si468x_write_single_byte(0x00);
	while(len--)
	{
		read_buffer[i++] = Si468x_read_single_byte();
	}
	CS_PIN_HIGH;
}

RETURN_CODE Si468x_write_command(uint16_t length, uint8_t *buffer)
{
	RETURN_CODE ret = Si468x_wait_for_CTS(POLL_TIMEOUT_MS);

	if((ret == SUCCESS) || (ret == COMMAND_ERROR)) //It is possible that there is a pending command error - we need to be able to send another command to clear out this state
	{
		Si468x_write_multiple(length, buffer);

		ret = Si468x_wait_for_CTS(POLL_TIMEOUT_MS);
	}
	send_debug_msg("Command Sent Successfully!", CRLF_SEND);
	return ret;
}

RETURN_CODE Si468x_read_reply(uint16_t length, uint8_t *buffer)
{
	Si468x_read_multiple(length, buffer);
	return SUCCESS;
}

uint8_t get_rx_buffer(int buffer_pos)
{
	return dab_spi_rx_buffer[buffer_pos];
}

void Si468x_get_sys_state()
{
	send_debug_msg("---------------Getting Si468x system state---------------", CRLF_SEND);
	dab_spi_tx_buffer[0]  = SI468X_CMD_GET_SYS_STATE;	//Get Sys State Command Code
	dab_spi_tx_buffer[1]  = 0x00;						//Value as in documentation

	status = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(5, dab_spi_rx_buffer);
	send_debug_msg("Status: ", CRLF_NO_SEND);
	switch(dab_spi_rx_buffer[4])
	{
		case 0:
			send_debug_msg("Bootloader is active.", CRLF_SEND);
			break;

		case 1:
			send_debug_msg("FM / FMHD is active.", CRLF_SEND);
			break;

		case 2:
			send_debug_msg("DAB / DAB+ is active.", CRLF_SEND);
			break;

		case 3:
			send_debug_msg("TDMB or data only DAB image is active.", CRLF_SEND);
			break;

		case 4:
			send_debug_msg("FM / FMHD Demod is active.", CRLF_SEND);
			break;

		case 5:
			send_debug_msg("AM / AMHD is active.", CRLF_SEND);
			break;

		case 6:
			send_debug_msg("AM / AMHD Demod is active.", CRLF_SEND);
			break;

		case 7:
			send_debug_msg("DAB Demod is active.", CRLF_SEND);
			break;

		default:
			send_debug_msg("Bootloader is active.", CRLF_SEND);
			break;
	}
}

void Si468x_get_part_info()
{
	send_debug_msg("--------------Getting part info about Si468x-------------", CRLF_SEND);
	dab_spi_tx_buffer[0]  = SI468X_CMD_GET_PART_INFO;	//Get Part Info Command Code
	dab_spi_tx_buffer[1]  = 0x00;						//Value as in documentation

	status = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(1);
	Si468x_write_single_byte(0x00);
	status = Si468x_read_reply(10, dab_spi_rx_buffer);
	send_debug_msg("Chip revision: ", CRLF_NO_SEND);
	send_debug_msg(itoa(dab_spi_rx_buffer[4], itoa_buffer, 10), CRLF_SEND);
	send_debug_msg("ROM ID: ", CRLF_NO_SEND);
	send_debug_msg(itoa(dab_spi_rx_buffer[5], itoa_buffer, 10), CRLF_SEND);
	send_debug_msg("Part Number: ", CRLF_NO_SEND);
	uint16_t part_number = (dab_spi_rx_buffer[9] << 8) + dab_spi_rx_buffer[8];
	send_debug_msg(itoa(part_number, itoa_buffer, 10), CRLF_SEND);
}

void Si468x_set_property(uint16_t property_id, uint16_t property_value)
{
	send_debug_msg("-----------------Setting Si468x property value-----------------", CRLF_SEND);
	send_debug_msg("Property ID: ", CRLF_NO_SEND);
	send_debug_msg(itoa(property_id, itoa_buffer, 16), CRLF_SEND);
	send_debug_msg("Property Value to write: ", CRLF_NO_SEND);
	send_debug_msg(itoa(property_value, itoa_buffer, 16), CRLF_SEND);
	dab_spi_tx_buffer[0] = SI468X_CMD_SET_PROPERTY;
	dab_spi_tx_buffer[1] = 0x00;
	dab_spi_tx_buffer[2] = property_id & 0xFF;
	dab_spi_tx_buffer[3] = property_id >> 8;
	dab_spi_tx_buffer[4] = property_value & 0xFF;
	dab_spi_tx_buffer[5] = property_value >> 8;

	status = Si468x_write_command(6, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(5, dab_spi_rx_buffer);
	uint16_t value_temp = Si468x_get_property(property_id);
	if(value_temp == property_value)
	{
		send_debug_msg("Property Set Successfully!", CRLF_SEND);
	}
	else
	{
		send_debug_msg("Property Setting Error!", CRLF_SEND);
	}

}

uint16_t Si468x_get_property(uint16_t property_id)
{
	send_debug_msg("--------------Getting Si468x property value--------------", CRLF_SEND);
	send_debug_msg("Property ID: ", CRLF_NO_SEND);
	send_debug_msg(itoa(property_id, itoa_buffer, 16), CRLF_SEND);
	dab_spi_tx_buffer[0] = SI468X_CMD_GET_PROPERTY;
	dab_spi_tx_buffer[1] = 0x01;
	dab_spi_tx_buffer[2] = property_id & 0xFF;
	dab_spi_tx_buffer[3] = property_id >> 8;

	status = Si468x_write_command(4, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(6, dab_spi_rx_buffer);

	send_debug_msg("Property value: ", CRLF_NO_SEND);
	send_debug_msg(itoa((dab_spi_rx_buffer[5] << 8) + dab_spi_rx_buffer[4], itoa_buffer, 16), CRLF_SEND);

	return (dab_spi_rx_buffer[5] << 8) + dab_spi_rx_buffer[4];
}

void Si468x_dab_get_freq_list()
{
	send_debug_msg("-----------------Getting Frequency List------------------", CRLF_SEND);
	dab_spi_tx_buffer[0]  = SI468X_CMD_DAB_GET_FREQ_LIST;		//Power up command code
	dab_spi_tx_buffer[1]  = 0x00;								//not used - value as in documentation

	status = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(5, dab_spi_rx_buffer);
	if(dab_spi_rx_buffer[0] & 0x40)
	{
		send_debug_msg("Command Error!", CRLF_SEND);
	}
	else
	{
		if(dab_spi_rx_buffer[4])
		{
			uint8_t read_offset = 2;
			uint8_t freq_cnt = dab_spi_rx_buffer[4];
			send_debug_msg("Found ", CRLF_NO_SEND);
			send_debug_msg(itoa(freq_cnt, itoa_buffer, 10), CRLF_NO_SEND);
			send_debug_msg(" frequencies in list.", CRLF_SEND);
			status = Si468x_read_reply(3, dab_spi_rx_buffer);
			status = Si468x_read_reply((freq_cnt + read_offset) * 4, dab_spi_rx_buffer);
			for(int i = 0; i < freq_cnt; i++)
			{
				freq_table[i] = dab_spi_rx_buffer[4 * (i + read_offset)] + (dab_spi_rx_buffer[4 * (i + read_offset) + 1] << 8) + (dab_spi_rx_buffer[4 * (i + read_offset) + 2] << 16) + (dab_spi_rx_buffer[4 * (i + read_offset) + 3] << 24);
				send_debug_msg(itoa(i, itoa_buffer, 10), CRLF_NO_SEND);
				send_debug_msg(": ", CRLF_NO_SEND);
				send_debug_msg(itoa(freq_table[i], itoa_buffer, 10), CRLF_NO_SEND);
				send_debug_msg(" kHz", CRLF_SEND);
			}
		}
		else
		{
			send_debug_msg("Frequency Table is empty.", CRLF_SEND);
		}
	}
}

void Si468x_dab_tune_freq(uint8_t channel)
{
	send_debug_msg("-------------DAB Tune to selected frequency--------------", CRLF_SEND);
	send_debug_msg("Frequency: ", CRLF_NO_SEND);
	send_debug_msg(itoa(freq_table[channel], itoa_buffer, 10), CRLF_NO_SEND);
	send_debug_msg(" kHz", CRLF_SEND);

	dab_spi_tx_buffer[0] = SI468X_CMD_DAB_TUNE_FREQ; 	//dab tune freq command code
	dab_spi_tx_buffer[1] = 0x00;						//padding - as in documentation
	dab_spi_tx_buffer[2] = channel;						//channel ID from table
	dab_spi_tx_buffer[3] = 0x00;						//padding - as in documentation
	dab_spi_tx_buffer[4] = 0x00;						//antcap [7:0]
	dab_spi_tx_buffer[5] = 0x00;						//antcap [15:8]

	status = Si468x_write_command(6, dab_spi_tx_buffer);
	status = Si468x_read_reply(1, dab_spi_rx_buffer);
	if(dab_spi_rx_buffer[0] & 0x40)
	{
		send_debug_msg("Command Error!", CRLF_SEND);
	}

	for(uint16_t i = 0; i < TUNE_TIMEOUT_MS; i++)
	{
		status = Si468x_read_reply(1, dab_spi_rx_buffer);

		if(dab_spi_rx_buffer[0] & 0x01)
		{
			send_debug_msg("Tuned successfully. Time: ", CRLF_NO_SEND);
			send_debug_msg(itoa(i ,itoa_buffer, 10), CRLF_NO_SEND);
			send_debug_msg(" ms.", CRLF_SEND);
			break;
		}
		if(i == TUNE_TIMEOUT_MS - 1)
		{
			send_debug_msg("Tune Timeout exceeded!", CRLF_SEND);
		}
		HAL_Delay(1);
	}
}

void Si468x_dab_reset_interrupts()
{
	send_debug_msg("------------------Clear DAB Interrupts-------------------", CRLF_SEND);

	dab_spi_tx_buffer[0] = SI468X_CMD_DAB_GET_EVENT_STATUS; 	//
	dab_spi_tx_buffer[1] = 0x01;								//Event ACK - clear all DAB event interrupts
	status = Si468x_write_command(2, dab_spi_tx_buffer);
	status = Si468x_read_reply(1, dab_spi_rx_buffer);
	if(dab_spi_rx_buffer[0] & 0x40)
	{
		send_debug_msg("Command Error!", CRLF_SEND);
	}
	else
	{
		send_debug_msg("Clear OK.", CRLF_SEND);
	}
}

void Si468x_dab_digrad_status()
{
	send_debug_msg("----------------Getting DAB Digrad Status----------------", CRLF_SEND);

	dab_spi_tx_buffer[0] = SI468X_CMD_DAB_DIGRAD_STATUS; 	//
	dab_spi_tx_buffer[1] = 0x00;
	status = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(40, dab_spi_rx_buffer);
	rssi = dab_spi_rx_buffer[6];
	snr = dab_spi_rx_buffer[7];
	fib_error_count = (dab_spi_rx_buffer[11] << 8) + dab_spi_rx_buffer[10];
	fic_bit_cnt = dab_spi_rx_buffer[32] + (dab_spi_rx_buffer[33] << 8) + (dab_spi_rx_buffer[34] << 16) + (dab_spi_rx_buffer[35] << 24);
	fic_err_cnt = dab_spi_rx_buffer[36] + (dab_spi_rx_buffer[37] << 8) + (dab_spi_rx_buffer[38] << 16) + (dab_spi_rx_buffer[39] << 24);
	fic_quality = dab_spi_rx_buffer[8];

	if(dab_spi_rx_buffer[5] & 0x01)
	{
		valid = 1;
	}
	else
	{
		valid = 0;
	}

	if(dab_spi_rx_buffer[5] & 0x04)
	{
		acq = 1;
	}
	else
	{
		acq = 0;
	}

	if(dab_spi_rx_buffer[4] & 0x04)
	{
		acq_int = 1;
	}
	else
	{
		acq_int = 0;
	}


	DisplayDabStatus(rssi, snr, valid, acq, fic_bit_cnt, fic_err_cnt, fic_quality);
}

void Si468x_dab_get_digital_service_list()
{
	send_debug_msg("--------------DAB Get Digital Service List---------------", CRLF_SEND);

	uint16_t list_size = 0;
	uint8_t number_of_services = 0;
	uint8_t pd_flag = 0;
	uint32_t srv_ref = 0;
	uint8_t country_id = 0;
	uint8_t p_ty = 0;
	uint8_t number_of_components = 0;
	uint32_t service_id = 0;

	#define LIST_READ_OFFSET 4

	dab_spi_tx_buffer[0] = SI468X_CMD_GET_DIGITAL_SERVICE_LIST; 	//Command Code Start Digital Service
	dab_spi_tx_buffer[1] = 0x00;									//for DAB always 0 - as in documentation

	status = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(2048, dab_spi_rx_buffer);
	if(dab_spi_rx_buffer[0] & 0x40)
	{
		send_debug_msg("Command Error!", CRLF_SEND);
	}
	list_size = dab_spi_rx_buffer[0 + LIST_READ_OFFSET] + (dab_spi_rx_buffer[1 + LIST_READ_OFFSET] << 8);
	send_debug_msg("Service list size: ", CRLF_NO_SEND);
	send_debug_msg(itoa(list_size, itoa_buffer, 10), CRLF_NO_SEND);
	send_debug_msg(" bytes.", CRLF_SEND);

	number_of_services = dab_spi_rx_buffer[4 + LIST_READ_OFFSET];
	send_debug_msg("Number of services: ", CRLF_NO_SEND);
	send_debug_msg(itoa(number_of_services, itoa_buffer, 10), CRLF_SEND);

	//---------test read service info---------------------------------------------------------------------------------------------------------------
	pd_flag = dab_spi_rx_buffer[12 + LIST_READ_OFFSET] & 0x01;
	send_debug_msg("PD Flag: ", CRLF_NO_SEND);
	send_debug_msg(itoa(pd_flag, itoa_buffer, 10), CRLF_SEND);

	p_ty = (dab_spi_rx_buffer[12 + LIST_READ_OFFSET] & 0x3E) >> 1;
	send_debug_msg("Program Type: ", CRLF_NO_SEND);
	send_debug_msg(itoa(p_ty, itoa_buffer, 10), CRLF_SEND);

	number_of_components = dab_spi_rx_buffer[13 + LIST_READ_OFFSET] & 0x0F;
	send_debug_msg("Number of Components: ", CRLF_NO_SEND);
	send_debug_msg(itoa(number_of_components, itoa_buffer, 10), CRLF_SEND);

	switch(p_ty)
	{
		case 0:
			srv_ref = dab_spi_rx_buffer[8 + LIST_READ_OFFSET] + ((dab_spi_rx_buffer[9 + LIST_READ_OFFSET] & 0x0F) << 8);
			country_id = (dab_spi_rx_buffer[9 + LIST_READ_OFFSET] & 0xF0) >> 4;
			service_id = (country_id << 12) + srv_ref;
			break;

		case 1:
			srv_ref = dab_spi_rx_buffer[8 + LIST_READ_OFFSET] + (dab_spi_rx_buffer[9 + LIST_READ_OFFSET] << 8) + ((dab_spi_rx_buffer[10 + LIST_READ_OFFSET] & 0x0F) << 16);
			country_id = (dab_spi_rx_buffer[10 + LIST_READ_OFFSET] & 0xF0) >> 4;
			service_id = (country_id << 20) + srv_ref;
			break;

		default:
			break;
	}
	send_debug_msg("Service Ref: ", CRLF_NO_SEND);
	send_debug_msg(itoa(srv_ref, itoa_buffer, 16), CRLF_SEND);
	send_debug_msg("Country ID: ", CRLF_NO_SEND);
	send_debug_msg(itoa(country_id, itoa_buffer, 16), CRLF_SEND);
	send_debug_msg("Service ID: ", CRLF_NO_SEND);
	send_debug_msg(itoa(service_id, itoa_buffer, 16), CRLF_SEND);


	send_debug_msg("Service Name: ", CRLF_SEND);

	for(uint8_t i = 0; i <= 15; i++)
	{
		send_debug_msg(itoa(dab_spi_rx_buffer[16 + i + LIST_READ_OFFSET], itoa_buffer, 16), CRLF_SEND);
	}

	send_debug_msg("End Of Service Name", CRLF_SEND);

//-------------------test read component info---------------------------------------------------------------------------------------------

	uint8_t tm_id = 0;
	uint8_t sub_ch_id = 0;

	tm_id = (dab_spi_rx_buffer[33] & 0xC0) >> 14;
	send_debug_msg("TM ID : ", CRLF_NO_SEND);
	send_debug_msg(itoa(tm_id, itoa_buffer, 10), CRLF_SEND);

	sub_ch_id = dab_spi_rx_buffer[32] & 0x3F;
	send_debug_msg("Subchannel ID : ", CRLF_NO_SEND);
	send_debug_msg(itoa(sub_ch_id, itoa_buffer, 10), CRLF_SEND);




	for(uint8_t i = 0; i < number_of_services; i++)
	{


	}



}

void Si468x_dab_start_digital_service(uint32_t service_id, uint32_t component_id)
{
	send_debug_msg("----------------DAB Start Digital Service----------------", CRLF_SEND);

	dab_spi_tx_buffer[0] = SI468X_CMD_START_DIGITAL_SERVICE; 	//Command Code Start Digital Service
	dab_spi_tx_buffer[1] = 0x00;								//always 0 - as in documentation
	dab_spi_tx_buffer[2] = 0x00;								//always 0 - as in documentation
	dab_spi_tx_buffer[3] = 0x00;								//always 0 - as in documentation

	dab_spi_tx_buffer[4]  = service_id & 0xFF;					//Service ID [7:0]
	dab_spi_tx_buffer[5]  = service_id >> 8;					//Service ID [15:8]
	dab_spi_tx_buffer[6]  = service_id >> 16;					//Service ID [23:16]
	dab_spi_tx_buffer[7]  = service_id >> 24;					//Service ID [31:24]

	dab_spi_tx_buffer[8]  = component_id & 0xFF;				//Component ID [7:0] Component ID prawdopodobnie nie ma znaczenia
	dab_spi_tx_buffer[9]  = component_id >> 8;					//Component ID [15:8]
	dab_spi_tx_buffer[10]  = component_id >> 16;				//Component ID [23:16]
	dab_spi_tx_buffer[11]  = component_id >> 24;				//Component ID [31:24]

	status = Si468x_write_command(12, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(5, dab_spi_rx_buffer);
	if(dab_spi_rx_buffer[0] & 0x40)
	{
		send_debug_msg("Command Error!", CRLF_SEND);
	}
	else
	{
		send_debug_msg("Service started successfully!", CRLF_SEND);
	}

}
