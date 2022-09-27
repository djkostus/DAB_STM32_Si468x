/*
 * Si468x.c
 *
 *  Created on: Aug 22, 2022
 *      Author: mgostek
 */

#include "main.h"
#include "spi.h"
#include "i2c.h"
#include "tim.h"
#include "Si468x.h"
#include "Si468x_defs.h"
#include "patch_mini.h"
#include "patch_full.h"
#include "debug_uart.h"
#include "dab_defs.h"
#include "eeprom.h"
#include "touch.h"

#include <string.h>


#define BOOT_WRITE_STEPS 3												//number of bootloader write steps

#define SIGNAL_CHECK_INTERVAL 50										//interval between signal quality check during scanning
#define VALID_TIMEOUT 6													//timeout for scanning purposes, determines how long to wait for valid and acquire, total timeout = SIGNAL_CHECK_INTERVAL * VALID_TIMEOUT [ms]
#define FIC_Q_TIMEOUT 150												//timeout for scanning purposes, determines how long to wait for achieve FIC Quality = 100, total timeout = SIGNAL_CHECK_INTERVAL * FIC_Q_TIMEOUT [ms]

static uint8_t dab_spi_tx_buffer[SPI_TX_BUFFER_SIZE];					//SPI TX Buffer using to send data to Si468x
static uint8_t dab_spi_rx_buffer[SPI_RX_BUFFER_SIZE];					//SPI RX Buffer using to read data from Si468x

static char itoa_buffer[64];											//buffer using to send debug informations by UART, it is necessary for itoa function

static RETURN_CODE status = 0;											//return status during executing some commands on Si468x

static uint32_t freq_table[48];											//table of frequencies (channels) read from Si468x memory

static dab_digrad_status_t dab_digrad_status;							//struct that contains some metrics informing about signal quality and status

static rd_reply_t rd_reply;												//struct that contains Si468x flags obtained during RD REPLY Command
static dab_events_t dab_events;											//struct that contains info about DAB flags obtained during DAB_GET_EVENT_STATUS command
static time_t time;														//struct that contains data & time obtained from DAB+
static dab_audio_info_t dab_audio_info;									//struct that contains DAB+ audio info
static dab_service_data_reply_header_t dab_service_data_reply_header;	//struct that contains data in dab service data reply header
static dab_ensemble_t ensembles_list[10];								//list of ensembles found during full scan
static dab_service_t services_list[100];								//list of services found during full scan

static dab_management_t dab_management;									//struct that contains some DAB management data

static uint8_t dls_label[129] = "";

static uint8_t slideshow_data[8500];

static uint16_t rssi_hi_res;


void Si468x_dab_init()
{
	send_debug_msg("-----------------------------Si468x DAB Mode init-----------------------------", CRLF_SEND);
	Si468x_reset();
	Si468x_power_up();
	Si468x_bootloader_load_host();
	Si468x_firmware_load_flash(IMAGE_DAB_6_0_9_START_ADDR);
	Si468x_boot();
	Si468x_get_part_info();
	Si468x_get_sys_state();
	Si468x_set_property(SI468x_PROP_DAB_TUNE_FE_CFG, 0x01); 						//włączanie lub wyłącznie switcha front-end, dla FM dać 0x00 (otwarty), dla DAB 0x01 (zamkniety)
//	Si468x_set_property(SI468x_PROP_DAB_TUNE_FE_VARM, 0xF468); 						//dla DAB 0xF8A9 lub 0xF784, sprawdzic jak lepiej dziala. Dla FM 0xEDB5. Wartość z kalibracji DAB: 0xF468 (-2968)
//	Si468x_set_property(SI468x_PROP_DAB_TUNE_FE_VARB, 0x250); 						//dla DAB 0x01C6 lub 0x01D8, sprawdzic jak lepiej dziala. Dla FM 0x01E3. Wartość z kalibracji DAB: 0x0250 (592)
	Si468x_set_property(SI468x_PROP_DAB_VALID_RSSI_THRESHOLD, 0x5); 				//prog RSSI od kiedy łapie kanał, default 12
	Si468x_set_property(SI468x_PROP_DAB_VALID_RSSI_TIME, 0x30); 					//czas po ktorym na podstawie RSSI jest określany Valid, od 0 do 63 ms, default 30 = 0x1e
	Si468x_set_property(SI468x_PROP_DAB_VALID_ACQ_TIME, 0xBB8); 					//czas jaki się czeka na osiągnięcie ACQ, jak nie osiagnie to uznaje ze nie ma sygnalu, od 0 do 4095 ms, default 2000 = 0x7D0
	Si468x_set_property(SI468x_PROP_DAB_XPAD_ENABLE, 0x4005); 						//określa które featury PAD będą przesyłane do hosta. bylo 0x4005
	Si468x_set_property(SI468x_PROP_DAB_EVENT_MIN_SVRLIST_PERIOD, 0x01); 			//określa co ile będzie aktualizowana lista usług, x100 ms. Bylo 6, testowo 1, zeby lepiej czytac nazwy
	Si468x_set_property(SI468x_PROP_DAB_EVENT_MIN_SVRLIST_PERIOD_RECONFIG, 0x01); 	//określa co ile będzie aktualizowana lista usług w trakcie rekonfiguracji, x100 ms
	Si468x_set_property(SI468x_PROP_DAB_ACF_ENABLE, 0x01);							//tylko soft mute włączone przy slabym sygnale
	Si468x_set_property(SI468x_PROP_DAB_ACF_MUTE_SIGLOSS_THRESHOLD, 0x05);			//próg wyciszania audio jak sygnal jest utracony, default 0x06
	Si468x_set_property(SI468x_PROP_DAB_ACF_SOFTMUTE_BER_LIMITS, 0xE2C4); 			//limit BER kiedy soft mute zadziała. Defaultowo 0xE2A6
	Si468x_set_property(SI468x_PROP_DIGITAL_SERVICE_INT_SOURCE, 0x01);				//Enables the DSRVPCKTINT interrupt of the GET_DIGITAL_SERVICE_DATA command
//	Si468x_dab_get_freq_list(); 													//odczytujemy z ukladu liste czestotliwosci do tablicy
	Si468x_get_sys_state(); //kontrolnie zeby sprawdzic czy demod dziala

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
	dab_spi_tx_buffer[5]  = 0xF8;	//F8 standard	//crystal frequency in MHz [15:8], 19.2 MHz
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
	send_debug_msg("Start Address: 0x", CRLF_NO_SEND);
	send_debug_msg(itoa(dab_spi_tx_buffer[4], itoa_buffer, 16), CRLF_NO_SEND);
	send_debug_msg(itoa(dab_spi_tx_buffer[5], itoa_buffer, 16), CRLF_NO_SEND);
	send_debug_msg(itoa(dab_spi_tx_buffer[6], itoa_buffer, 16), CRLF_NO_SEND);
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
	for(uint16_t i = 0; i < timeout; i++)
	{
		Si468x_read_reply(1, dab_spi_rx_buffer);
		if(rd_reply.cts)
		{
			if(rd_reply.err_cmd)
			{
				send_debug_msg("Command Error during waiting for CTS!", CRLF_SEND);
				return COMMAND_ERROR;
			}
			return SUCCESS;
		}
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
//	send_debug_msg("Command Sent Successfully!", CRLF_SEND);
	return ret;
}

RETURN_CODE Si468x_read_reply(uint16_t length, uint8_t *buffer)
{
	Si468x_read_multiple(length, buffer);
	//fill rd_reply struct with received data to get values of flags, both commands are working
//	rd_reply = *((rd_reply_t*)dab_spi_rx_buffer);
	memcpy((uint8_t*)&rd_reply, (uint8_t*)dab_spi_rx_buffer, sizeof(rd_reply_t));
	return SUCCESS;
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
			send_debug_msg("Unrecognized state.", CRLF_SEND);
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
	if(rd_reply.err_cmd)
	{
		send_debug_msg("Command Error!", CRLF_SEND);
	}
	else
	{
		if(dab_spi_rx_buffer[4])
		{
			uint8_t read_offset = 2;
			dab_management.freq_cnt = dab_spi_rx_buffer[4];
			send_debug_msg("Found ", CRLF_NO_SEND);
			send_debug_msg(itoa(dab_management.freq_cnt, itoa_buffer, 10), CRLF_NO_SEND);
			send_debug_msg(" frequencies in list.", CRLF_SEND);
			status = Si468x_read_reply(3, dab_spi_rx_buffer);
			status = Si468x_read_reply((dab_management.freq_cnt + read_offset) * 4, dab_spi_rx_buffer);
			for(int i = 0; i < dab_management.freq_cnt; i++)
			{
				freq_table[i] = dab_spi_rx_buffer[4 * (i + read_offset)] + (dab_spi_rx_buffer[4 * (i + read_offset) + 1] << 8) + (dab_spi_rx_buffer[4 * (i + read_offset) + 2] << 16) + (dab_spi_rx_buffer[4 * (i + read_offset) + 3] << 24);
				send_debug_msg(itoa(i, itoa_buffer, 10), CRLF_NO_SEND);
				send_debug_msg(": ", CRLF_NO_SEND);
				send_debug_msg(itoa(freq_table[i], itoa_buffer, 10), CRLF_NO_SEND);
				send_debug_msg(" kHz", CRLF_SEND);
			}

			//save to eeprom
			send_debug_msg("Saving to EEPROM...", CRLF_SEND);

			eeprom_write(FREQ_TABLE_SIZE_ADDR, &dab_management.freq_cnt, sizeof(dab_management.freq_cnt));

			for (uint8_t i = 0; i < 3; i++)
			{
				eeprom_write(FREQ_TABLE_START_ADDR + PAGE_SIZE * i, &freq_table[i * PAGE_SIZE / 4], PAGE_SIZE);
			}


			send_debug_msg("Saved OK.", CRLF_SEND);
		}
		else
		{
			send_debug_msg("Frequency Table is empty.", CRLF_SEND);
		}
	}
}

void Si468x_dab_tune_freq(uint8_t channel, uint16_t ant_cap)
{
	send_debug_msg("-------------DAB Tune to selected frequency--------------", CRLF_SEND);
	send_debug_msg("Frequency: ", CRLF_NO_SEND);
	send_debug_msg(itoa(freq_table[channel], itoa_buffer, 10), CRLF_NO_SEND);
	send_debug_msg(" kHz", CRLF_SEND);

	dab_spi_tx_buffer[0] = SI468X_CMD_DAB_TUNE_FREQ; 	//dab tune freq command code
	dab_spi_tx_buffer[1] = 0x00;						//padding - as in documentation
	dab_spi_tx_buffer[2] = channel;						//channel ID from table
	dab_spi_tx_buffer[3] = 0x00;						//padding - as in documentation

	switch(ant_cap)
	{
		case USE_ANT_CAP:
			//use ANT_CAP value from table of best ANTCAP values
			dab_spi_tx_buffer[4] = ant_cap_values[channel] & 0xFF;	//antcap [7:0]
			dab_spi_tx_buffer[5] = ant_cap_values[channel] >> 8;	//antcap [15:8]
			break;
		case NOT_USE_ANT_CAP:
			//use algorithm of automatic setting ant_cap value based on tuning values written to Si4684
			dab_spi_tx_buffer[4] = 0x00;	//antcap [7:0]
			dab_spi_tx_buffer[5] = 0x00;	//antcap [15:8]
			break;
		default:
			//use ant_cap value as argument of this function
			dab_spi_tx_buffer[4] = ant_cap & 0xFF;	//antcap [7:0]
			dab_spi_tx_buffer[5] = ant_cap >> 8;	//antcap [15:8]
			break;
	}

	status = Si468x_write_command(6, dab_spi_tx_buffer);
	status = Si468x_read_reply(1, dab_spi_rx_buffer);
	if(rd_reply.err_cmd)
	{
		send_debug_msg("Command Error!", CRLF_SEND);
	}

	for(uint16_t i = 0; i < TUNE_TIMEOUT_MS; i++)
	{
		status = Si468x_read_reply(1, dab_spi_rx_buffer);

		if(rd_reply.stc_int)
		{
			send_debug_msg("Tuned successfully. Time: ", CRLF_NO_SEND);
			send_debug_msg(itoa(i ,itoa_buffer, 10), CRLF_NO_SEND);
			send_debug_msg(" ms.", CRLF_SEND);
			dab_management.actual_freq_id = channel;
			dab_management.actual_freq = freq_table[channel];
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
	if(rd_reply.err_cmd)
	{
		send_debug_msg("Command Error!", CRLF_SEND);
	}
	else
	{
		send_debug_msg("Clear OK.", CRLF_SEND);
	}
}

dab_digrad_status_t Si468x_dab_digrad_status()
{
//	send_debug_msg("----------------Getting DAB Digrad Status----------------", CRLF_SEND);

	dab_spi_tx_buffer[0] = SI468X_CMD_DAB_DIGRAD_STATUS; 	//DAB DIGRAD Status Command Code
	dab_spi_tx_buffer[1] = 0x1A;							//Enable reset FIC PSEUDO BER and FIB Errors
	status = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(40, dab_spi_rx_buffer);
	if(rd_reply.stc_int)
	{
		memcpy((uint8_t*)&dab_digrad_status, (uint8_t*)&dab_spi_rx_buffer[4], sizeof(dab_digrad_status));	//
		if(dab_digrad_status.snr > 20)
		{
			dab_digrad_status.snr = 0; //snr powyzej 20 nie wystapi, a skrajnie duza wartosc podczas skanowania jest przeklamana
		}
		if(dab_digrad_status.cnr > 54)
		{
			dab_digrad_status.cnr = 0; //cnr powyzej 54 nie wystapi, a skrajnie duza wartosc podczas skanowania jest przeklamana
		}

	}
	return dab_digrad_status;
}

void Si468x_dab_get_digital_service_list()
{
	//List management variables
	uint16_t list_size = 0;				//size of ensemble information list
	uint8_t number_of_services = 0;		//quantity of services found in ensemble
	uint8_t services_count = 0;			//quantity of services in list up to this point, it is necessary to correct data parsing
	uint8_t components_count = 0;		//quantity of components in list up to this point, it is necessary to correct data parsing

	#define LIST_READ_OFFSET 4 			//Offset to make parsing easier, it comes from some padding data before list

	dab_spi_tx_buffer[0] = SI468X_CMD_GET_DIGITAL_SERVICE_LIST; 	//Command Code Start Digital Service
	dab_spi_tx_buffer[1] = 0x00;									//for DAB always 0 - as in documentation

	status = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(500, dab_spi_rx_buffer);

	send_debug_msg("--------------DAB Get Digital Service List---------------", CRLF_SEND);

	if(rd_reply.err_cmd)
	{
		send_debug_msg("Command Error!", CRLF_SEND);
	}

	list_size = dab_spi_rx_buffer[0 + LIST_READ_OFFSET] + (dab_spi_rx_buffer[1 + LIST_READ_OFFSET] << 8);
	number_of_services = dab_spi_rx_buffer[4 + LIST_READ_OFFSET];

	//-----read services info---------------------------------------------------------------------------------------------------------------
	for(uint8_t service_index = 0; service_index < number_of_services; service_index++)
	{
		services_list[service_index + dab_management.actual_services].freq = dab_management.actual_freq;
		services_list[service_index + dab_management.actual_services].freq_id = dab_management.actual_freq_id;

		services_list[service_index + dab_management.actual_services].pd_flag = dab_spi_rx_buffer[12 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] & 0x01;
		services_list[service_index + dab_management.actual_services].number_of_components = dab_spi_rx_buffer[13 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] & 0x0F;

		//ignore service with PD Flag = 1 - it's data service, not audio service
		if(services_list[service_index + dab_management.actual_services].pd_flag)
		{
			services_count++;
			components_count += services_list[service_index + dab_management.actual_services].number_of_components;
			service_index--;
			number_of_services--;
			continue;
		}

		services_list[service_index + dab_management.actual_services].p_ty = (dab_spi_rx_buffer[12 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] & 0x3E) >> 1;

		//it's not necessary to check pd flag because stations with pd_flag = 1 are ignored now, but it's for future purposes
		switch(services_list[service_index + dab_management.actual_services].pd_flag)
		{
			case 0:
				services_list[service_index + dab_management.actual_services].srv_ref = dab_spi_rx_buffer[8 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] + ((dab_spi_rx_buffer[9 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] & 0x0F) << 8);
				services_list[service_index + dab_management.actual_services].country_id = (dab_spi_rx_buffer[9 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] & 0xF0) >> 4;
				services_list[service_index + dab_management.actual_services].service_id = (services_list[service_index + dab_management.actual_services].country_id << 12) + services_list[service_index + dab_management.actual_services].srv_ref;
				break;

			case 1:
				services_list[service_index + dab_management.actual_services].srv_ref = dab_spi_rx_buffer[8 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] + (dab_spi_rx_buffer[9 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] << 8) + ((dab_spi_rx_buffer[10 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] & 0x0F) << 16);
				services_list[service_index + dab_management.actual_services].country_id = (dab_spi_rx_buffer[10 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] & 0xF0) >> 4;
				services_list[service_index + dab_management.actual_services].service_id = (services_list[service_index + dab_management.actual_services].country_id << 20) + services_list[service_index + dab_management.actual_services].srv_ref;
				break;

			default:
				break;
		}


		if(dab_spi_rx_buffer[16 + LIST_READ_OFFSET  + 24 * services_count + 4 * components_count]) //check if name is valid by checking first char of name, it shouldn't be 0
		{
			for(uint8_t name_index = 0; name_index <= 15; name_index++)
			{
				services_list[service_index + dab_management.actual_services].name[name_index] = dab_spi_rx_buffer[16 + name_index + LIST_READ_OFFSET  + 24 * services_count + 4 * components_count];
				if(services_list[service_index + dab_management.actual_services].name[name_index] == 0x86)	//Correct "ó" coding: 0x86 -> 0xF3, ó as o: 0x86 -> 0x6F
				{
					services_list[service_index + dab_management.actual_services].name[name_index] = 0x6F;
				}
			}
		}

		else
		{
			char empty_name[20] = "Unknown Name    ";

			for(uint8_t name_index = 0; name_index <= 15; name_index++)
			{
				services_list[service_index + dab_management.actual_services].name[name_index] = empty_name[name_index];
			}
		}



		//----read component info---------------------------------------------------------------------------------------------
		for(uint8_t component_index = 0; component_index < services_list[service_index + dab_management.actual_services].number_of_components; component_index++)
		{
			services_list[service_index + dab_management.actual_services].components[component_index].tm_id = (dab_spi_rx_buffer[33 + LIST_READ_OFFSET  + 24 * services_count + 4 * components_count] & 0xC0) >> 14;
			services_list[service_index + dab_management.actual_services].components[component_index].subchannel_id = dab_spi_rx_buffer[32 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] & 0x3F;
			components_count++;
		}
		services_count++;
	}

	dab_management.total_services += number_of_services;
	dab_management.actual_services += number_of_services;
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

	dab_spi_tx_buffer[8]  = component_id & 0xFF;				//Component ID [7:0]
	dab_spi_tx_buffer[9]  = component_id >> 8;					//Component ID [15:8]
	dab_spi_tx_buffer[10]  = component_id >> 16;				//Component ID [23:16]
	dab_spi_tx_buffer[11]  = component_id >> 24;				//Component ID [31:24]

	status = Si468x_write_command(12, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(5, dab_spi_rx_buffer);
	if(rd_reply.err_cmd)
	{
		send_debug_msg("Command Error!", CRLF_SEND);
	}
	else
	{
		send_debug_msg("Service started successfully!", CRLF_SEND);
	}
}

uint8_t Si468x_dab_get_ensemble_info()
{
	uint32_t ensemble_id_temp = 0;
	send_debug_msg("------------------DAB Get Ensemble Info------------------", CRLF_SEND);

	dab_spi_tx_buffer[0] = SI468X_CMD_DAB_GET_ENSEMBLE_INFO; 	//Command Code DAB Get Ensemble Info
	dab_spi_tx_buffer[1] = 0x00;								//always 0 - as in documentation
	status = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(2);
	status = Si468x_read_reply(21, dab_spi_rx_buffer);

	//wait for good ensemble name
	while(!dab_spi_rx_buffer[6])
	{
		status = Si468x_write_command(2, dab_spi_tx_buffer);
		HAL_Delay(2);
		status = Si468x_read_reply(21, dab_spi_rx_buffer);
	}

	if(rd_reply.err_cmd)
	{
		send_debug_msg("Command Error!", CRLF_SEND);
		return 0;
	}
	else
	{
		ensemble_id_temp = (dab_spi_rx_buffer[5] << 8) + dab_spi_rx_buffer[4];

		if(ensemble_id_temp)
		{
			send_debug_msg("Ensemble found.", CRLF_SEND);
			ensembles_list[dab_management.total_ensembles].id = ensemble_id_temp;
			ensembles_list[dab_management.total_ensembles].freq = dab_management.actual_freq;
			ensembles_list[dab_management.total_ensembles].freq_id  = dab_management.actual_freq_id;

			for(uint8_t i = 0; i < 16; i++)
			{
				ensembles_list[dab_management.total_ensembles].label[i] = dab_spi_rx_buffer[6 + i];
			}

			dab_management.total_ensembles++;
			return 1;
		}
		else
		{
			send_debug_msg("Ensemble not found.", CRLF_SEND);
			return 0;
		}
	}
}

uint8_t Si468x_dab_full_scan()
{
	send_debug_msg("----------------------DAB Full Scan----------------------", CRLF_SEND);
	dab_management.total_services = 0;
	dab_management.total_ensembles = 0;
	dab_management.actual_services = 0;

	uint8_t valid_timeout = 0;
	uint8_t fic_q_timeout = 0;

	uint8_t _scan_cancel_flag = 0;

	touch_coordinates_t _touch_coordinates;

	for(uint8_t freq_index = 0; freq_index < dab_management.freq_cnt; freq_index++)
	{
		_touch_coordinates = Touch_read();
//		_scan_cancel_flag = get_scan_cancel_flag();

		//handle cancel button
		if(_touch_coordinates.x >  5 && _touch_coordinates.x < 315 && _touch_coordinates.y > 195 && _touch_coordinates.y < 235)
		{
			_scan_cancel_flag = 1;
		}

		if(_scan_cancel_flag)
		{
			break;
		}

//		dab_digrad_status.valid = 0;
//		dab_digrad_status.acq = 0;
//		dab_digrad_status.fic_quality = 0;

		valid_timeout = VALID_TIMEOUT;
		fic_q_timeout = FIC_Q_TIMEOUT;

		Si468x_dab_tune_freq(freq_index, USE_ANT_CAP);

		do
		{
			Si468x_dab_digrad_status();
			valid_timeout--;
			if(!valid_timeout)
			{
				send_debug_msg("Ensemble not found.", CRLF_SEND);
				break;
			}
			HAL_Delay(SIGNAL_CHECK_INTERVAL);
		}while(!dab_digrad_status.valid || !dab_digrad_status.acq);

		if(valid_timeout)
		{
			do
			{
				Si468x_dab_digrad_status();
				fic_q_timeout--;
				if(!fic_q_timeout)
				{
					send_debug_msg("Ensemble not found.", CRLF_SEND);
					break;
				}
				HAL_Delay(SIGNAL_CHECK_INTERVAL);
			}while(dab_digrad_status.fic_quality < 50);
		}

		if(valid_timeout && fic_q_timeout)
		{
			do
			{
				Si468x_dab_get_event_status();
				HAL_Delay(10);
			}while(!dab_events.srv_list || dab_events.srv_list_int);

			if(Si468x_dab_get_ensemble_info())
			{
				Si468x_dab_get_digital_service_list();
			}
		}
		Display_scanning_screen_data(dab_digrad_status, dab_management);
	}

	if(_scan_cancel_flag)
	{
		send_debug_msg("Scanning cancelled.", CRLF_SEND);
		restore_from_eeprom();
		return 0;
	}

	else
	{
		eeprom_clear_scanning_data();

		if(dab_management.total_services)
		{
			eeprom_save_scanning_data(services_list, dab_management.total_services, ensembles_list, dab_management.total_ensembles);
			dab_management.actual_station = 0;
			dab_management.last_station_index = 0;
			eeprom_write(LAST_STATION_INDEX_ADDR, &dab_management.last_station_index, sizeof(dab_management.last_station_index));
		}

		//display info about  ensembles
		send_debug_msg("Ensembles found: ", CRLF_NO_SEND);
		send_debug_msg(itoa(dab_management.total_ensembles, itoa_buffer, 10), CRLF_SEND);

		send_debug_msg("--------------------------------------------------", CRLF_SEND);
		send_debug_msg("| Number", CRLF_NO_SEND);
		send_debug_msg(" | Label          ", CRLF_NO_SEND);
		send_debug_msg("| Frequency ", CRLF_NO_SEND);
		send_debug_msg(" | Channel |", CRLF_SEND);

		for(uint8_t ensembles_index = 0; ensembles_index < dab_management.total_ensembles; ensembles_index++)
		{
			send_debug_msg("| ", CRLF_NO_SEND);
			send_debug_msg(itoa(ensembles_index + 1, itoa_buffer, 10), CRLF_NO_SEND);
			send_debug_msg("      | ", CRLF_NO_SEND);

			send_debug_msg(ensembles_list[ensembles_index].label, CRLF_NO_SEND);
			send_debug_msg("| ", CRLF_NO_SEND);

			send_debug_msg(itoa(ensembles_list[ensembles_index].freq, itoa_buffer, 10), CRLF_NO_SEND);
			send_debug_msg(" kHz | ", CRLF_NO_SEND);

			send_debug_msg(dab_channels_names[ensembles_list[ensembles_index].freq_id], CRLF_NO_SEND);
			if(ensembles_list[ensembles_index].freq_id < 20)
			{
				send_debug_msg(" ", CRLF_NO_SEND);
			}
			send_debug_msg("     |", CRLF_SEND);
		}
		send_debug_msg("--------------------------------------------------", CRLF_SEND);

		//display info about services
		send_debug_msg("Services found: ", CRLF_NO_SEND);
		send_debug_msg(itoa(dab_management.total_services, itoa_buffer, 10), CRLF_SEND);

		send_debug_msg("--------------------------------------------------------------------------------------------------------", CRLF_SEND);
		send_debug_msg("| Number | Name             | Ensemble Name   | Frequency  | Channel | PTY | Service ID | Component ID |", CRLF_SEND);

		for(uint8_t services_index = 0; services_index < dab_management.total_services; services_index++)
		{
			//Number
			send_debug_msg("| ", CRLF_NO_SEND);
			send_debug_msg(itoa(services_index + 1, itoa_buffer, 10), CRLF_NO_SEND);
			if((services_index + 1) < 10)
			{
				send_debug_msg(" ", CRLF_NO_SEND);
			}
			send_debug_msg("     | ", CRLF_NO_SEND);

			//Name
			send_debug_msg(services_list[services_index].name, CRLF_NO_SEND);
			send_debug_msg(" | ", CRLF_NO_SEND);

			//Ensemble Name
			for(uint8_t i = 0; i < dab_management.total_ensembles; i++)
			{
				if(ensembles_list[i].freq_id == services_list[services_index].freq_id)
				{
					send_debug_msg(ensembles_list[i].label, CRLF_NO_SEND);
					break;
				}
			}
			send_debug_msg(" | ", CRLF_NO_SEND);

			//Frequency
			send_debug_msg(itoa(services_list[services_index].freq, itoa_buffer, 10), CRLF_NO_SEND);
			send_debug_msg(" kHz | ", CRLF_NO_SEND);

			//Channel
			send_debug_msg(dab_channels_names[services_list[services_index].freq_id], CRLF_NO_SEND);
			if(services_list[services_index].freq_id < 20)
			{
				send_debug_msg(" ", CRLF_NO_SEND);
			}
			send_debug_msg("     | ", CRLF_NO_SEND);

			//PTY
			send_debug_msg(itoa(services_list[services_index].p_ty, itoa_buffer, 10), CRLF_NO_SEND);
			if(services_list[services_index].p_ty < 10)
			{
				send_debug_msg(" ", CRLF_NO_SEND);
			}
			send_debug_msg("  | ", CRLF_NO_SEND);

			//Service ID
			send_debug_msg("0x", CRLF_NO_SEND);
			send_debug_msg(itoa(services_list[services_index].service_id, itoa_buffer, 16), CRLF_NO_SEND);
			send_debug_msg("     | ", CRLF_NO_SEND);

			//Component ID
			send_debug_msg("0x", CRLF_NO_SEND);
			send_debug_msg(itoa(services_list[services_index].components[0].subchannel_id, itoa_buffer, 16), CRLF_NO_SEND);
			send_debug_msg("          |", CRLF_SEND);
		}
		send_debug_msg("--------------------------------------------------------------------------------------------------------", CRLF_SEND);

		//to check if everything is ok in eeprom
		//  eeprom_show();
		return 1;
	}
}

dab_audio_info_t Si468x_dab_get_audio_info()
{
	send_debug_msg("--------------Getting audio info from Si468x-------------", CRLF_SEND);
	dab_spi_tx_buffer[0] = SI468X_CMD_DAB_GET_AUDIO_INFO;
	dab_spi_tx_buffer[1] = 0x03; 	//	ber_option: 1 = long_term, 0 = short_term, ber_ack: 2 = reset counters, 0 = don't reset counters

	status = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(20, dab_spi_rx_buffer);

	memcpy((uint8_t*)&dab_audio_info, (uint8_t*)&dab_spi_rx_buffer[4], sizeof(dab_audio_info));

	send_debug_msg("Bitrate: ", CRLF_NO_SEND);
	send_debug_msg(itoa(dab_audio_info.audio_bit_rate, itoa_buffer, 10), CRLF_SEND);

	send_debug_msg("Sample rate: ", CRLF_NO_SEND);
	send_debug_msg(itoa(dab_audio_info.audio_sample_rate, itoa_buffer, 10), CRLF_SEND);

	send_debug_msg("XPAD Indicator: ", CRLF_NO_SEND);
	send_debug_msg(itoa(dab_audio_info.audio_xpad_ind, itoa_buffer, 10), CRLF_SEND);

	send_debug_msg("PS Flag: ", CRLF_NO_SEND);
	send_debug_msg(itoa(dab_audio_info.audio_ps_flag, itoa_buffer, 10), CRLF_SEND);

	send_debug_msg("SBR Flag: ", CRLF_NO_SEND);
	send_debug_msg(itoa(dab_audio_info.audio_sbr, itoa_buffer, 10), CRLF_SEND);

	send_debug_msg("Audio Mode: ", CRLF_NO_SEND);
	send_debug_msg(itoa(dab_audio_info.audio_mode, itoa_buffer, 10), CRLF_SEND);

	return dab_audio_info;
}

void Si468x_dab_get_event_status()
{
	send_debug_msg("-----------Getting DAB Event Status from Si468x-------------", CRLF_SEND);
	dab_spi_tx_buffer[0] = SI468X_CMD_DAB_GET_EVENT_STATUS;
	dab_spi_tx_buffer[1] = 0x03;	//+2 = Clears the AUDIO_STATUS error indicators BLK_ERROR and BLK_LOSS of this command, +1 = Clears all pending DAB event interrupt bits

	status = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(9, dab_spi_rx_buffer);

	memcpy((uint8_t*)&dab_events, (uint8_t*)&dab_spi_rx_buffer[4], sizeof(rd_reply_t));
}

void Si468x_dab_get_component_info(uint32_t service_id, uint8_t component_id)
{
	send_debug_msg("-----------Getting DAB Component Info-------------", CRLF_SEND);

	dab_spi_tx_buffer[0] = SI468X_CMD_DAB_GET_COMPONENT_INFO;
	dab_spi_tx_buffer[1] = 0x00;	//always 0 as in documentation
	dab_spi_tx_buffer[2] = 0x00;	//always 0 as in documentation
	dab_spi_tx_buffer[3] = 0x00;	//always 0 as in documentation

	dab_spi_tx_buffer[4]  = service_id & 0xFF;					//Service ID [7:0]
	dab_spi_tx_buffer[5]  = service_id >> 8;					//Service ID [15:8]
	dab_spi_tx_buffer[6]  = service_id >> 16;					//Service ID [23:16]
	dab_spi_tx_buffer[7]  = service_id >> 24;					//Service ID [31:24]

	dab_spi_tx_buffer[8]  = component_id & 0xFF;				//Component ID [7:0]
	dab_spi_tx_buffer[9]  = component_id >> 8;					//Component ID [15:8]
	dab_spi_tx_buffer[10]  = component_id >> 16;				//Component ID [23:16]
	dab_spi_tx_buffer[11]  = component_id >> 24;				//Component ID [31:24]


	status = Si468x_write_command(12, dab_spi_tx_buffer);
	HAL_Delay(5);
	status = Si468x_read_reply(26, dab_spi_rx_buffer);

	uint8_t global_id;
	uint8_t language;
	uint8_t label[16];
	uint16_t char_abbrev;

	language = dab_spi_rx_buffer[6] & 0x3F;

	for(uint8_t idx = 0; idx < 16; idx++)
	{
		label[idx] = dab_spi_rx_buffer[8 + idx];
	}

	char_abbrev = dab_spi_rx_buffer[24] + (dab_spi_rx_buffer[25] << 8);

	send_debug_msg("Language: ", CRLF_NO_SEND);
	send_debug_msg(itoa(language, itoa_buffer, 10), CRLF_SEND);

	send_debug_msg("Label:            ", CRLF_NO_SEND);
	send_debug_msg((char*)label, CRLF_SEND);

	send_debug_msg("Char abbrev mask: ", CRLF_NO_SEND);
	send_debug_msg(itoa(char_abbrev, itoa_buffer, 2), CRLF_SEND);

}

void Si468x_dab_get_digital_service_data()
{
	status = Si468x_read_reply(5, dab_spi_rx_buffer);

	if(rd_reply.d_srv_int)
	{
		dab_spi_tx_buffer[0] = SI468X_CMD_GET_DIGITAL_SERVICE_DATA;
		dab_spi_tx_buffer[1] = 0x01;	//+1 = ACK, +16 = STATUS_ONLY

		status = Si468x_write_command(2, dab_spi_tx_buffer);
		HAL_Delay(5);

		//first read only header to obtain payload data size
		status = Si468x_read_reply(30, dab_spi_rx_buffer);
		memcpy((uint8_t*)&dab_service_data_reply_header, (uint8_t*)&dab_spi_rx_buffer[4], sizeof(dab_service_data_reply_header));


//		if(dab_service_data_reply_header.buff_count)
		{
			send_debug_msg("-----------Getting DAB Service Data-------------", CRLF_SEND);

//			send_debug_msg("DSRV PCKT INT: ", CRLF_NO_SEND);
//			send_debug_msg(itoa(dab_service_data_reply_header.dsrv_pckt_int, itoa_buffer, 10), CRLF_SEND);
//
//			send_debug_msg("DSRV OVFL INT: ", CRLF_NO_SEND);
//			send_debug_msg(itoa(dab_service_data_reply_header.dsrv_ovfl_int, itoa_buffer, 10), CRLF_SEND);
//
//			send_debug_msg("Buffer count: ", CRLF_NO_SEND);
//			send_debug_msg(itoa(dab_service_data_reply_header.buff_count, itoa_buffer, 10), CRLF_SEND);
//
//			send_debug_msg("SRV STATE: ", CRLF_NO_SEND);
//			switch(dab_service_data_reply_header.srv_state)
//			{
//				case 0:
//					send_debug_msg("PLAYING", CRLF_SEND);
//					break;
//				case 1:
//					send_debug_msg("STOPPED", CRLF_SEND);
//					break;
//				case 2:
//					send_debug_msg("OVERFLOW", CRLF_SEND);
//					break;
//				case 3:
//					send_debug_msg("NEW OBJECT", CRLF_SEND);
//					break;
//				case 4:
//					send_debug_msg("PACKET ERRORS", CRLF_SEND);
//					break;
//				default:
//					send_debug_msg("Unknown", CRLF_SEND);
//					break;
//			}
//
//			send_debug_msg("DATA SRC: ", CRLF_NO_SEND);
//			switch(dab_service_data_reply_header.data_src)
//			{
//				case 0:
//					send_debug_msg("DATA SERVICE", CRLF_SEND);
//					break;
//				case 1:
//					send_debug_msg("PAD DATA", CRLF_SEND);
//					break;
//				case 2:
//					send_debug_msg("PAD DLS", CRLF_SEND);
//					break;
//				default:
//					send_debug_msg("Unknown", CRLF_SEND);
//					break;
//			}
//
//			send_debug_msg("DSC TY: ", CRLF_NO_SEND);
//			switch(dab_service_data_reply_header.dsc_ty)
//			{
//				case 0:
//					send_debug_msg("Unspecified", CRLF_SEND);
//					break;
//				case 1:
//					send_debug_msg("TMC", CRLF_SEND);
//					break;
//				case 5:
//					send_debug_msg("TMC/TPEG", CRLF_SEND);
//					break;
//				case 60:
//					send_debug_msg("MOT", CRLF_SEND);
//					break;
//				default:
//					send_debug_msg("Unknown", CRLF_SEND);
//					break;
//			}
//
//			send_debug_msg("BYTE COUNT: ", CRLF_NO_SEND);
//			send_debug_msg(itoa(dab_service_data_reply_header.byte_count, itoa_buffer, 10), CRLF_SEND);
//			send_debug_msg("UA TYPE: ", CRLF_NO_SEND);
//			send_debug_msg(itoa(dab_service_data_reply_header.ua_type, itoa_buffer, 10), CRLF_SEND);
//
//			send_debug_msg("SEGMENT NUM: ", CRLF_NO_SEND);
//			send_debug_msg(itoa(dab_service_data_reply_header.seg_num, itoa_buffer, 10), CRLF_SEND);
//			send_debug_msg("NUMBER OF SEGS: ", CRLF_NO_SEND);
//			send_debug_msg(itoa(dab_service_data_reply_header.num_segs, itoa_buffer, 10), CRLF_SEND);


			//second read full data when we know data length

			if(!dab_service_data_reply_header.dsrv_ovfl_int)
			{
				status = Si468x_read_reply(24 + dab_service_data_reply_header.byte_count, dab_spi_rx_buffer);

				if(dab_service_data_reply_header.dsc_ty == 60)
				{
					send_debug_msg("Slideshow Data: ", CRLF_SEND);

					uint32_t body_size;
					uint16_t header_size;
					uint8_t content_type;
					uint16_t content_sub_type;

					body_size = (dab_spi_rx_buffer[24 + 0] << 20) + (dab_spi_rx_buffer[24 + 1] << 12) + (dab_spi_rx_buffer[24 + 2] << 4) + ((dab_spi_rx_buffer[24 + 3] && 0xF0) >> 4);

					header_size = ((dab_spi_rx_buffer[24 + 3] && 0x0F) << 9) + (dab_spi_rx_buffer[24 + 4] << 1) + ((dab_spi_rx_buffer[24 + 5] && 0x80) >> 7);

					content_type = (dab_spi_rx_buffer[24 + 5] && 0x7E) >> 1;

					content_sub_type = ((dab_spi_rx_buffer[24 + 5] && 0x01) << 8) + (dab_spi_rx_buffer[24 + 6]);

					for(uint16_t i = 0; i < 4096; i++)
					{
						slideshow_data[i] = 0;
					}

					for(uint16_t i = 0; i < dab_service_data_reply_header.byte_count; i++)
					{
						slideshow_data[i] = dab_spi_rx_buffer[24 + i];
					}

					for(uint16_t i = 0; i < dab_service_data_reply_header.byte_count; i++)
					{
//						send_debug_msg(slideshow_data[i]);
						send_debug_msg(itoa(slideshow_data[i], itoa_buffer, 10), CRLF_SEND);
					}
//					send_debug_msg("", CRLF_SEND);
					send_debug_msg("end", CRLF_SEND);
					send_debug_msg("Body Size: ", CRLF_NO_SEND);
					send_debug_msg(itoa(body_size, itoa_buffer, 10), CRLF_SEND);
					send_debug_msg("Header Size: ", CRLF_NO_SEND);
					send_debug_msg(itoa(header_size, itoa_buffer, 10), CRLF_SEND);
					send_debug_msg("Content Type: ", CRLF_NO_SEND);
					send_debug_msg(itoa(content_type, itoa_buffer, 10), CRLF_SEND);
					send_debug_msg("Content SubType: ", CRLF_NO_SEND);
					send_debug_msg(itoa(content_sub_type, itoa_buffer, 10), CRLF_SEND);

				}

				if(dab_service_data_reply_header.data_src == 2)
				{
					send_debug_msg("Dynamic Label Data: ", CRLF_SEND);

					for(uint8_t i = 0; i < 128; i++)
					{
						dls_label[i] = 0;
					}

					uint8_t label_pos = 0;
					for(uint8_t i = 0; i < dab_service_data_reply_header.byte_count; i++)
					{
						dls_label[label_pos] = dab_spi_rx_buffer[24 + 2 + i];
//						send_debug_msg(itoa(dab_spi_rx_buffer[24 + 2 + i], itoa_buffer, 10), CRLF_SEND);

						//polish symbols convert
						//ó
						if(dls_label[label_pos] == 195)
						{
							dls_label[label_pos] = 111;//lub 243
							i++;
						}
						//ą
						if(dls_label[label_pos] == 177)
						{

						}
						//ę
						if(dls_label[label_pos] == 234)
						{

						}
						//ć to c
						if(dls_label[label_pos] == 196)
						{
							dls_label[label_pos] = 99; //lub 230
							i++;
						}
						//ż
						if(dls_label[label_pos] == 191)
						{

						}
						//ź
						if(dls_label[label_pos] == 188)
						{

						}
						//ś
						if(dls_label[label_pos] == 182)
						{

						}
						//ł to l
						if(dls_label[label_pos] == 197)
						{
							dls_label[label_pos] = 108; //lub 179
							i++;
						}
						//ń
						if(dls_label[label_pos] == 243)
						{

						}

						label_pos++;
					}
					send_debug_msg(dls_label, CRLF_SEND);
				}
			}
		}
	}
}


void Si468x_dab_test_get_ber_info()	//póki co odczyt BER nie działa
{
		send_debug_msg("--------------Getting BER info from Si468x---------------", CRLF_SEND);
		dab_spi_tx_buffer[0] = SI468X_CMD_DAB_TEST_GET_BER_INFO;
		dab_spi_tx_buffer[1] = 0x00; 	//	always 0, as in documentation

		status = Si468x_write_command(2, dab_spi_tx_buffer);
		HAL_Delay(1);
		status = Si468x_read_reply(12, dab_spi_rx_buffer);

		dab_digrad_status.fic_bit_cnt = dab_spi_rx_buffer[8] + (dab_spi_rx_buffer[9] << 8) + (dab_spi_rx_buffer[10] << 16) + (dab_spi_rx_buffer[11] << 24);
		dab_digrad_status.fic_err_cnt = dab_spi_rx_buffer[4] + (dab_spi_rx_buffer[5] << 8) + (dab_spi_rx_buffer[6] << 16) + (dab_spi_rx_buffer[7] << 24);

		DisplayDabStatus(dab_digrad_status);
}

void Si468x_dab_get_time()
{
//	send_debug_msg("--------------Getting time from Si468x-------------------", CRLF_SEND);

	//wczesniejesza metoda sprawdzania czy sygnal jest ok
//	do
//	{
//		Si468x_dab_get_event_status();
//		HAL_Delay(10);
//	}while(!dab_events.srv_list || dab_events.srv_list_int);

	if(dab_digrad_status.acq && dab_digrad_status.valid && dab_digrad_status.fic_quality > 90)
	{
		dab_spi_tx_buffer[0] = SI468X_CMD_DAB_GET_TIME;
		dab_spi_tx_buffer[1] = 0x00; 	//0 - local time, 1 - UTC

		status = Si468x_write_command(2, dab_spi_tx_buffer);
		HAL_Delay(1);
		status = Si468x_read_reply(11, dab_spi_rx_buffer);

		memcpy((uint8_t*)&time, (uint8_t*)&dab_spi_rx_buffer[4], sizeof(time));

		Display_time(time);

//		send_debug_msg(itoa(time.hour, itoa_buffer, 10), CRLF_NO_SEND);
//		send_debug_msg(":", CRLF_NO_SEND);
//		send_debug_msg(itoa(time.minute, itoa_buffer, 10), CRLF_NO_SEND);
//		send_debug_msg(":", CRLF_NO_SEND);
//		send_debug_msg(itoa(time.second, itoa_buffer, 10), CRLF_SEND);
//
//		send_debug_msg(itoa(time.day, itoa_buffer, 10), CRLF_NO_SEND);
//		send_debug_msg(".", CRLF_NO_SEND);
//		send_debug_msg(itoa(time.month, itoa_buffer, 10), CRLF_NO_SEND);
//		send_debug_msg(".", CRLF_NO_SEND);
//		send_debug_msg(itoa(time.year, itoa_buffer, 10), CRLF_SEND);
	}
}

void Si468x_set_audio_volume(uint8_t _volume)
{
	Si468x_set_property(SI468x_PROP_AUDIO_ANALOG_VOLUME, _volume);
	dab_management.audio_volume = _volume;
	send_debug_msg("Volume: ", CRLF_NO_SEND);
	send_debug_msg(itoa(dab_management.audio_volume, itoa_buffer, 10), CRLF_NO_SEND);
	send_debug_msg("/63", CRLF_SEND);
	eeprom_write(LAST_VOLUME_ADDR, &_volume, sizeof(_volume));
}

uint16_t Si468x_test_get_rssi()
{

	dab_spi_tx_buffer[0] = SI468X_CMD_TEST_GET_RSSI;
	dab_spi_tx_buffer[1] = 0x00; 	//always 0 - as in documentation

	status = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(6, dab_spi_rx_buffer);

	rssi_hi_res = dab_spi_rx_buffer[4] + (dab_spi_rx_buffer[5] << 8);
	send_debug_msg("RSSI Hi Res: ", CRLF_NO_SEND);
	send_debug_msg(itoa(rssi_hi_res, itoa_buffer, 10), CRLF_SEND);

	return rssi_hi_res;
}



void play_station(uint8_t station_id)
{
	dab_management.actual_station = station_id;
	dab_management.last_station_index = dab_management.actual_station;

	eeprom_write(LAST_STATION_INDEX_ADDR, &dab_management.last_station_index, sizeof(dab_management.last_station_index));

	send_debug_msg("---------------------------------", CRLF_SEND);
	send_debug_msg("Playing Station ", CRLF_NO_SEND);
	send_debug_msg(itoa(dab_management.actual_station + 1, itoa_buffer, 10), CRLF_SEND);
	send_debug_msg("Name: ", CRLF_NO_SEND);
	send_debug_msg(services_list[dab_management.actual_station].name, CRLF_SEND);
	Si468x_dab_tune_freq(services_list[dab_management.actual_station].freq_id, USE_ANT_CAP); //CH_11B - PR Kraków, CH_9C - DABCOM Tarnów, CH_10D - PR Kielce,
	Si468x_dab_get_component_info(services_list[dab_management.actual_station].service_id, services_list[dab_management.actual_station].components[0].subchannel_id);
	Si468x_dab_start_digital_service(services_list[dab_management.actual_station].service_id, services_list[dab_management.actual_station].components[0].subchannel_id);
	Si468x_dab_digrad_status();
	Si468x_dab_get_audio_info();

	Si468x_set_property(SI468x_PROP_DIGITAL_SERVICE_INT_SOURCE, 0x03);	//enable both service interrupts
}

void restore_from_eeprom()
{
	send_debug_msg("--------------Restore from EEPROM memory-------------------", CRLF_SEND);

	//restore frequency table
	eeprom_read(FREQ_TABLE_SIZE_ADDR, &dab_management.freq_cnt, sizeof(dab_management.freq_cnt));
	for (uint8_t i = 0; i < 3; i++)
	{
		eeprom_read(FREQ_TABLE_START_ADDR + PAGE_SIZE * i, &freq_table[i * PAGE_SIZE / 4], PAGE_SIZE);
		HAL_Delay(5);
	}
	//display freq table - check if everything is ok
	send_debug_msg("Found ", CRLF_NO_SEND);
	send_debug_msg(itoa(dab_management.freq_cnt, itoa_buffer, 10), CRLF_NO_SEND);
	send_debug_msg(" frequencies in list.", CRLF_SEND);
	for(int i = 0; i < dab_management.freq_cnt; i++)
	{
		send_debug_msg(itoa(i, itoa_buffer, 10), CRLF_NO_SEND);
		send_debug_msg(": ", CRLF_NO_SEND);
		send_debug_msg(itoa(freq_table[i], itoa_buffer, 10), CRLF_NO_SEND);
		send_debug_msg(" kHz", CRLF_SEND);
	}

	//restore scanning data
	eeprom_read(TOTAL_ENSEMBLES_ADDR, &dab_management.total_ensembles, sizeof(dab_management.total_ensembles));
	eeprom_read(TOTAL_SERVICES_ADDR, &dab_management.total_services, sizeof(dab_management.total_services));

	if(dab_management.total_services != 0xFF && dab_management.total_ensembles != 0xFF)
	{
		eeprom_read(LAST_FREQUENCY_ADDR, &dab_management.actual_freq, sizeof(dab_management.actual_freq));
		eeprom_read(LAST_FREQ_ID_ADDR, &dab_management.actual_freq_id, sizeof(dab_management.actual_freq_id));

		eeprom_read(LAST_STATION_INDEX_ADDR, &dab_management.last_station_index, sizeof(dab_management.last_station_index));
		if(dab_management.last_station_index == 0xFF)
		{
			dab_management.last_station_index = 0;
		}
		dab_management.actual_station = dab_management.last_station_index;

		for(uint8_t i = 0; i < dab_management.total_ensembles; i++)
		{
			eeprom_read(ENSEMBLES_TABLE_START_ADDR + PAGE_SIZE * i, &ensembles_list[i], sizeof(dab_ensemble_t));
		}

		for(uint8_t i = 0; i < dab_management.total_services; i++)
		{
			eeprom_read(SERVICES_TABLE_START_ADDR + PAGE_SIZE * i, &services_list[i], sizeof(dab_service_t));
		}

		//check if everything is ok

		//display freq table
		send_debug_msg("Found ", CRLF_NO_SEND);
		send_debug_msg(itoa(dab_management.freq_cnt, itoa_buffer, 10), CRLF_NO_SEND);
		send_debug_msg(" frequencies in list.", CRLF_SEND);
		for(int i = 0; i < dab_management.freq_cnt; i++)
		{
			send_debug_msg(itoa(i, itoa_buffer, 10), CRLF_NO_SEND);
			send_debug_msg(": ", CRLF_NO_SEND);
			send_debug_msg(itoa(freq_table[i], itoa_buffer, 10), CRLF_NO_SEND);
			send_debug_msg(" kHz", CRLF_SEND);
		}


		//display info about  ensembles
		send_debug_msg("Ensembles found: ", CRLF_NO_SEND);
		send_debug_msg(itoa(dab_management.total_ensembles, itoa_buffer, 10), CRLF_SEND);

		send_debug_msg("--------------------------------------------------", CRLF_SEND);
		send_debug_msg("| Number", CRLF_NO_SEND);
		send_debug_msg(" | Label          ", CRLF_NO_SEND);
		send_debug_msg("| Frequency ", CRLF_NO_SEND);
		send_debug_msg(" | Channel |", CRLF_SEND);

		for(uint8_t ensembles_index = 0; ensembles_index < dab_management.total_ensembles; ensembles_index++)
		{
			send_debug_msg("| ", CRLF_NO_SEND);
			send_debug_msg(itoa(ensembles_index + 1, itoa_buffer, 10), CRLF_NO_SEND);
			send_debug_msg("      | ", CRLF_NO_SEND);

			send_debug_msg(ensembles_list[ensembles_index].label, CRLF_NO_SEND);
			send_debug_msg("| ", CRLF_NO_SEND);

			send_debug_msg(itoa(ensembles_list[ensembles_index].freq, itoa_buffer, 10), CRLF_NO_SEND);
			send_debug_msg(" kHz | ", CRLF_NO_SEND);

			send_debug_msg(dab_channels_names[ensembles_list[ensembles_index].freq_id], CRLF_NO_SEND);
			if(ensembles_list[ensembles_index].freq_id < 20)
			{
				send_debug_msg(" ", CRLF_NO_SEND);
			}
			send_debug_msg("     |", CRLF_SEND);
		}
		send_debug_msg("--------------------------------------------------", CRLF_SEND);

		//display info about services
		send_debug_msg("Services found: ", CRLF_NO_SEND);
		send_debug_msg(itoa(dab_management.total_services, itoa_buffer, 10), CRLF_SEND);

		send_debug_msg("--------------------------------------------------------------------------------------------------------", CRLF_SEND);
		send_debug_msg("| Number | Name             | Ensemble Name   | Frequency  | Channel | PTY | Service ID | Component ID |", CRLF_SEND);

		for(uint8_t services_index = 0; services_index < dab_management.total_services; services_index++)
		{
			//Number
			send_debug_msg("| ", CRLF_NO_SEND);
			send_debug_msg(itoa(services_index + 1, itoa_buffer, 10), CRLF_NO_SEND);
			if((services_index +1) < 10)
			{
				send_debug_msg(" ", CRLF_NO_SEND);
			}
			send_debug_msg("     | ", CRLF_NO_SEND);

			//Name
			send_debug_msg(services_list[services_index].name, CRLF_NO_SEND);
			send_debug_msg(" | ", CRLF_NO_SEND);

			//Ensemble Name
			for(uint8_t i = 0; i < dab_management.total_ensembles; i++)
			{
				if(ensembles_list[i].freq_id == services_list[services_index].freq_id)
				{
					send_debug_msg(ensembles_list[i].label, CRLF_NO_SEND);
					break;
				}
			}
			send_debug_msg(" | ", CRLF_NO_SEND);

			//Frequency
			send_debug_msg(itoa(services_list[services_index].freq, itoa_buffer, 10), CRLF_NO_SEND);
			send_debug_msg(" kHz | ", CRLF_NO_SEND);

			//Channel
			send_debug_msg(dab_channels_names[services_list[services_index].freq_id], CRLF_NO_SEND);
			if(services_list[services_index].freq_id < 20)
			{
				send_debug_msg(" ", CRLF_NO_SEND);
			}
			send_debug_msg("     | ", CRLF_NO_SEND);

			//PTY
			send_debug_msg(itoa(services_list[services_index].p_ty, itoa_buffer, 10), CRLF_NO_SEND);
			if(services_list[services_index].p_ty < 10)
			{
				send_debug_msg(" ", CRLF_NO_SEND);
			}
			send_debug_msg("  | ", CRLF_NO_SEND);

			//Service ID
			send_debug_msg("0x", CRLF_NO_SEND);
			send_debug_msg(itoa(services_list[services_index].service_id, itoa_buffer, 16), CRLF_NO_SEND);
			send_debug_msg("     | ", CRLF_NO_SEND);

			//Component ID
			send_debug_msg("0x", CRLF_NO_SEND);
			send_debug_msg(itoa(services_list[services_index].components[0].subchannel_id, itoa_buffer, 16), CRLF_NO_SEND);
			send_debug_msg("          |", CRLF_SEND);
		}
		send_debug_msg("--------------------------------------------------------------------------------------------------------", CRLF_SEND);

		//restore volume level
		eeprom_read(LAST_VOLUME_ADDR, &dab_management.audio_volume, sizeof(dab_management.audio_volume));
		Si468x_set_audio_volume(dab_management.audio_volume);

		//play last played station
		Si468x_dab_tune_freq(services_list[dab_management.last_station_index].freq_id, USE_ANT_CAP);
		play_station(dab_management.last_station_index);

	}
	else
	{
		send_debug_msg("Service list is empty!", CRLF_SEND);
		dab_management.actual_freq = 0;
		dab_management.actual_freq_id = 0;
		dab_management.total_ensembles = 0;
		dab_management.total_services = 0;
		dab_management.actual_station = 0;
	}
}


dab_digrad_status_t get_digrad_status()
{
	return dab_digrad_status;
}

dab_service_t* get_dab_service_list()
{
	return services_list;
}

dab_ensemble_t* get_dab_ensemble_list()
{
	return ensembles_list;
}

dab_management_t get_dab_management()
{
	return dab_management;
}

char* get_dls_label()
{
	return dls_label;
}

void calibration(uint8_t channel)
{
	send_debug_msg("Performing calibration procedure. Channel: ", CRLF_NO_SEND);
	send_debug_msg(itoa(channel, itoa_buffer, 10), CRLF_NO_SEND);
	send_debug_msg(", ", CRLF_NO_SEND);
	send_debug_msg(dab_channels_names[channel], CRLF_SEND);

	uint32_t average_rssi = 0;

	uint16_t best_rssi_val = 0;

	uint8_t best_ant_cap = 0;

	for(uint8_t ant_cap = 1; ant_cap <= 128; ant_cap++)
	{
		send_debug_msg("Actual ANT_CAP: ", CRLF_NO_SEND);
		send_debug_msg(itoa(ant_cap, itoa_buffer, 10), CRLF_SEND);
		average_rssi = 0;
		Si468x_dab_tune_freq(channel, ant_cap);
		HAL_Delay(60);
		for(uint8_t i = 0; i < 5; i++)
		{
			average_rssi += Si468x_test_get_rssi();
			HAL_Delay(30);
		}
		average_rssi /= 5;

		if(average_rssi > best_rssi_val)
		{
			best_rssi_val = average_rssi;
			best_ant_cap = ant_cap;
		}
	}
	send_debug_msg("Best RSSI: ", CRLF_NO_SEND);
	send_debug_msg(itoa(best_rssi_val, itoa_buffer, 10), CRLF_SEND);
	send_debug_msg("Obtained with ANT_CAP val: ", CRLF_NO_SEND);
	send_debug_msg(itoa(best_ant_cap, itoa_buffer, 10), CRLF_SEND);
}
