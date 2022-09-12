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
#include "dab_defs.h"


#define BOOT_WRITE_STEPS 3							//number of bootloader write steps

#define SIGNAL_CHECK_INTERVAL 50					//interval between signal quality check during scanning
#define VALID_TIMEOUT 4								//timeout for scanning purposes, determines how long to wait for valid and acquire, total timeout = SIGNAL_CHECK_INTERVAL * VALID_TIMEOUT [ms]
#define FIC_Q_TIMEOUT 100							//timeout for scanning purposes, determines how long to wait for achieve FIC Quality = 100, total timeout = SIGNAL_CHECK_INTERVAL * FIC_Q_TIMEOUT [ms]

uint8_t dab_spi_tx_buffer[SPI_TX_BUFFER_SIZE];		//SPI TX Buffer using to send data to Si468x
uint8_t dab_spi_rx_buffer[SPI_RX_BUFFER_SIZE];		//SPI RX Buffer using to read data from Si468x

char itoa_buffer[64];								//buffer using to send debug informations by UART, it is necessary for itoa function

RETURN_CODE status = 0;								//return status during executing some commands on Si468x

uint32_t freq_table[48];							//table of frequencies (channels) read from Si468x memory
uint8_t freq_cnt = 0;								//quantity of frequencies in Si468x memory

sig_metrics_t sig_metrics;							//struct that contains some metrics informing about signal quality and status

dab_flags_t dab_flags;								//struct that contains DAB flags obtained during RD REPLY Command

dab_events_t dab_events;							//struct that contains info about DAB flags obtained during DAB_GET_EVENT_STATUS command

dab_ensemble_t ensembles_list[10];					//list of ensembles found during full scan
dab_service_t services_list[50];					//list of services found during full scan
uint8_t total_services = 0;							//total quantity of services found during full scan
uint8_t total_ensembles = 0;						//total quantity of ensembles found during full scan
uint8_t actual_services = 0;						//actual quantity of services during scanning process, this variable is necessary to save stations in memory

uint32_t actual_freq = 0;							//value of the frequency to which the Si468x is currently tuned in kHz
uint8_t actual_freq_id = 0;							//frequency table index of the frequency to which the Si468x is currently tuned in kHz

uint8_t actual_station = 0;

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
	Si468x_set_property(SI468x_PROP_DAB_TUNE_FE_CFG, 0x01); 						//włączanie lub wyłącznie switcha front-end, prawdopodobnie dla dab dać 0x00, dla FM 0x01
	Si468x_set_property(SI468x_PROP_DAB_TUNE_FE_VARM, 0xF8A9); 						//dla DAB 0xF8A9 lub 0xF784, sprawdzic jak lepiej dziala. Dla FM 0xEDB5.
	Si468x_set_property(SI468x_PROP_DAB_TUNE_FE_VARB, 0x01C6); 						//dla DAB 0x01C6 lub 0x01D8, sprawdzic jak lepiej dziala. Dla FM 0x01E3.
	Si468x_set_property(SI468x_PROP_DAB_VALID_RSSI_THRESHOLD, 0x7); 				//prog RSSI od kiedy łapie kanał, default 12
	Si468x_set_property(SI468x_PROP_DAB_VALID_RSSI_TIME, 0x30); 					//czas po ktorym na podstawie RSSI jest określany Valid, od 0 do 63 ms, default 30 = 0x1e
	Si468x_set_property(SI468x_PROP_DAB_VALID_ACQ_TIME, 0xBB8); 					//czas jaki się czeka na osiągnięcie ACQ, jak nie osiagnie to uznaje ze nie ma sygnalu, od 0 do 4095 ms, default 2000 = 0x7D0
	Si468x_set_property(SI468x_PROP_DAB_XPAD_ENABLE, 0x4005); 						//określa które featury PAD będą przesyłane do hosta
	Si468x_set_property(SI468x_PROP_DAB_EVENT_MIN_SVRLIST_PERIOD, 0x01); 			//określa co ile będzie aktualizowana lista usług, x100 ms. Bylo 6, testowo 1, zeby lepiej czytac nazwy
	Si468x_set_property(SI468x_PROP_DAB_EVENT_MIN_SVRLIST_PERIOD_RECONFIG, 0x01); 	//określa co ile będzie aktualizowana lista usług w trakcie rekonfiguracji, x100 ms
	Si468x_set_property(SI468x_PROP_DAB_ACF_ENABLE, 0x01);							//tylko soft mute włączone przy slabym sygnale
	Si468x_set_property(SI468x_PROP_DAB_ACF_MUTE_SIGLOSS_THRESHOLD, 0x05);			//próg wyciszania audio jak sygnal jest utracony, default 0x06
	Si468x_set_property(SI468x_PROP_DAB_ACF_SOFTMUTE_BER_LIMITS, 0xE2C4); 			//limit BER kiedy soft mute zadziała. Defaultowo 0xE2A6
	Si468x_set_property(SI468x_PROP_DIGITAL_SERVICE_INT_SOURCE, 0x01);				//Enables the DSRVPCKTINT interrupt of the GET_DIGITAL_SERVICE_DATA command

	Si468x_dab_get_freq_list(); 													//odczytujemy z ukladu liste czestotliwosci do tablicy

	Si468x_get_sys_state(); //kontrolnie zeby sprawdzic czy demod dziala

	Si468x_dab_full_scan();

	Si468x_dab_tune_freq(CH_11B);
	HAL_Delay(1000);
	Si468x_dab_get_time();

//	Si468x_dab_tune_freq(CH_10B); //CH_11B - PR Kraków, CH_9C - DABCOM Tarnów, CH_10D - PR Kielce,

//	for(uint8_t i = 0; i < 50; i++)
//	{
//		if(services_list[i].service_id == 0x3211)
//		{
//			Si468x_dab_tune_freq(services_list[i].freq_id); //CH_11B - PR Kraków, CH_9C - DABCOM Tarnów, CH_10D - PR Kielce,
//			Si468x_dab_start_digital_service(services_list[i].service_id, services_list[i].components[0].subchannel_id);
//			break;
//		}
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
//	send_debug_msg("Command Sent Successfully!", CRLF_SEND);
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
	if(dab_spi_rx_buffer[0] & 0x40)
	{
		send_debug_msg("Command Error!", CRLF_SEND);
	}
	else
	{
		if(dab_spi_rx_buffer[4])
		{
			uint8_t read_offset = 2;
			freq_cnt = dab_spi_rx_buffer[4];
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
			actual_freq_id = channel;
			actual_freq = freq_table[channel];
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
//	send_debug_msg("----------------Getting DAB Digrad Status----------------", CRLF_SEND);

	dab_spi_tx_buffer[0] = SI468X_CMD_DAB_DIGRAD_STATUS; 	//DAB DIGRAD Status Command Code
	dab_spi_tx_buffer[1] = 0x1A;							//Enable reset FIC PSEUDO BER and FIB Errors
	status = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(40, dab_spi_rx_buffer);
	sig_metrics.rssi = dab_spi_rx_buffer[6];
	sig_metrics.snr = dab_spi_rx_buffer[7];
	sig_metrics.fic_bit_cnt = dab_spi_rx_buffer[32] + (dab_spi_rx_buffer[33] << 8) + (dab_spi_rx_buffer[34] << 16) + (dab_spi_rx_buffer[35] << 24);
	sig_metrics.fic_err_cnt = dab_spi_rx_buffer[36] + (dab_spi_rx_buffer[37] << 8) + (dab_spi_rx_buffer[38] << 16) + (dab_spi_rx_buffer[39] << 24);
	sig_metrics.fic_q = dab_spi_rx_buffer[8];

	if(dab_spi_rx_buffer[5] & 0x01)
	{
		sig_metrics.valid = 1;
	}
	else
	{
		sig_metrics.valid = 0;
	}

	if(dab_spi_rx_buffer[5] & 0x04)
	{
		sig_metrics.acq = 1;
	}
	else
	{
		sig_metrics.acq = 0;
	}

	if(dab_spi_rx_buffer[4] & 0x04)
	{
		sig_metrics.acq_int = 1;
	}
	else
	{
		sig_metrics.acq_int = 0;
	}

	DisplayDabStatus(sig_metrics);
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

	if(dab_spi_rx_buffer[0] & 0x40)
	{
		send_debug_msg("Command Error!", CRLF_SEND);
	}

	list_size = dab_spi_rx_buffer[0 + LIST_READ_OFFSET] + (dab_spi_rx_buffer[1 + LIST_READ_OFFSET] << 8);
	number_of_services = dab_spi_rx_buffer[4 + LIST_READ_OFFSET];

	//-----read services info---------------------------------------------------------------------------------------------------------------
	for(uint8_t service_index = 0; service_index < number_of_services; service_index++)
	{
		services_list[service_index + actual_services].freq = actual_freq;
		services_list[service_index + actual_services].freq_id = actual_freq_id;

		services_list[service_index + actual_services].pd_flag = dab_spi_rx_buffer[12 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] & 0x01;
		services_list[service_index + actual_services].number_of_components = dab_spi_rx_buffer[13 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] & 0x0F;

		//ignore service with PD Flag = 1 - it's data service, not audio service
		if(services_list[service_index + actual_services].pd_flag)
		{
			services_count++;
			components_count += services_list[service_index + actual_services].number_of_components;
			service_index--;
			number_of_services--;
			continue;
		}

		services_list[service_index + actual_services].p_ty = (dab_spi_rx_buffer[12 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] & 0x3E) >> 1;

		//it's not necessary to check pd flag because stations with pd_flag = 1 are ignored now, but it's for future purposes
		switch(services_list[service_index + actual_services].pd_flag)
		{
			case 0:
				services_list[service_index + actual_services].srv_ref = dab_spi_rx_buffer[8 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] + ((dab_spi_rx_buffer[9 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] & 0x0F) << 8);
				services_list[service_index + actual_services].country_id = (dab_spi_rx_buffer[9 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] & 0xF0) >> 4;
				services_list[service_index + actual_services].service_id = (services_list[service_index + actual_services].country_id << 12) + services_list[service_index + actual_services].srv_ref;
				break;

			case 1:
				services_list[service_index + actual_services].srv_ref = dab_spi_rx_buffer[8 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] + (dab_spi_rx_buffer[9 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] << 8) + ((dab_spi_rx_buffer[10 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] & 0x0F) << 16);
				services_list[service_index + actual_services].country_id = (dab_spi_rx_buffer[10 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] & 0xF0) >> 4;
				services_list[service_index + actual_services].service_id = (services_list[service_index + actual_services].country_id << 20) + services_list[service_index + actual_services].srv_ref;
				break;

			default:
				break;
		}

		for(uint8_t name_index = 0; name_index <= 15; name_index++)
		{
			services_list[service_index + actual_services].name[name_index] = dab_spi_rx_buffer[16 + name_index + LIST_READ_OFFSET  + 24 * services_count + 4 * components_count];
			if(services_list[service_index + actual_services].name[name_index] == 0x86)	//Correct "ó" coding: 0x86 -> 0xF3, ó as o: 0x86 -> 0x6F
			{
				services_list[service_index + actual_services].name[name_index] = 0x6F;
			}
		}

		//----read component info---------------------------------------------------------------------------------------------
		for(uint8_t component_index = 0; component_index < services_list[service_index + actual_services].number_of_components; component_index++)
		{
			services_list[service_index + actual_services].components[component_index].tm_id = (dab_spi_rx_buffer[33 + LIST_READ_OFFSET  + 24 * services_count + 4 * components_count] & 0xC0) >> 14;
			services_list[service_index + actual_services].components[component_index].subchannel_id = dab_spi_rx_buffer[32 + LIST_READ_OFFSET + 24 * services_count + 4 * components_count] & 0x3F;
			components_count++;
		}
		services_count++;
	}

	total_services += number_of_services;
	actual_services += number_of_services;

	//display informations obtained during parsing ensemble data

//	send_debug_msg("Services list size: ", CRLF_NO_SEND);
//	send_debug_msg(itoa(list_size, itoa_buffer, 10), CRLF_NO_SEND);
//	send_debug_msg(" bytes.", CRLF_SEND);
//
//	send_debug_msg("Number of services: ", CRLF_NO_SEND);
//	send_debug_msg(itoa(number_of_services, itoa_buffer, 10), CRLF_SEND);
//
//	for(uint8_t i = 0; i < number_of_services; i++)
//	{
//		send_debug_msg("--------------------Next service--------------------------", CRLF_SEND);
//
//		send_debug_msg("Number: ", CRLF_NO_SEND);
//		send_debug_msg(itoa(i, itoa_buffer, 10), CRLF_SEND);
//
//		send_debug_msg("Service Name: ", CRLF_NO_SEND);
//		send_debug_msg((char*)services_list[i].name, CRLF_SEND);
//
//		send_debug_msg("Service ID: 0x", CRLF_NO_SEND);
//		send_debug_msg(itoa(services_list[i].service_id, itoa_buffer, 16), CRLF_SEND);
//
////		send_debug_msg("PD Flag: ", CRLF_NO_SEND);
////		send_debug_msg(itoa(services_list[i].pd_flag, itoa_buffer, 10), CRLF_SEND);
//
//		send_debug_msg("Program Type: ", CRLF_NO_SEND);
//		send_debug_msg(itoa(services_list[i].p_ty, itoa_buffer, 10), CRLF_SEND);
//
//		send_debug_msg("Number of Components: ", CRLF_NO_SEND);
//		send_debug_msg(itoa(services_list[i].number_of_components, itoa_buffer, 10), CRLF_SEND);
//
//		for(uint8_t j = 0; j < services_list[i].number_of_components; j++)
//		{
//			send_debug_msg("----------Next component----------", CRLF_SEND);
//			send_debug_msg("TM ID : ", CRLF_NO_SEND);
//			send_debug_msg(itoa(services_list[i].components[j].tm_id, itoa_buffer, 10), CRLF_SEND);
//			send_debug_msg("Subchannel ID : 0x", CRLF_NO_SEND);
//			send_debug_msg(itoa(services_list[i].components[j].subchannel_id, itoa_buffer, 16), CRLF_SEND);
//		}
//	}
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
	if(dab_spi_rx_buffer[0] & 0x40)
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

	while(!dab_spi_rx_buffer[6])
	{
		status = Si468x_write_command(2, dab_spi_tx_buffer);
		HAL_Delay(2);
		status = Si468x_read_reply(21, dab_spi_rx_buffer);
	}

	if(dab_spi_rx_buffer[0] & 0x40)
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
			ensembles_list[total_ensembles].id = ensemble_id_temp;
			ensembles_list[total_ensembles].freq = actual_freq;
			ensembles_list[total_ensembles].freq_id  =actual_freq_id;

			for(uint8_t i = 0; i < 16; i++)
			{
				ensembles_list[total_ensembles].label[i] = dab_spi_rx_buffer[6 + i];
			}

//			send_debug_msg("Label: ", CRLF_NO_SEND);
//			send_debug_msg((char*)ensembles_list[total_ensembles].label, CRLF_SEND);
//			send_debug_msg("Ensemble ID: ", CRLF_NO_SEND);
//			send_debug_msg(itoa(ensembles_list[total_ensembles].id, itoa_buffer, 16), CRLF_SEND);
//			send_debug_msg("Frequency: ", CRLF_NO_SEND);
//			send_debug_msg(itoa(ensembles_list[total_ensembles].freq, itoa_buffer, 10), CRLF_NO_SEND);
//			send_debug_msg(" kHz", CRLF_SEND);
//			send_debug_msg("Channel: ", CRLF_NO_SEND);
//			send_debug_msg(dab_channels_names[ensembles_list[total_ensembles].freq_id], CRLF_SEND);

			total_ensembles++;
			return 1;
		}
		else
		{
			send_debug_msg("Ensemble not found.", CRLF_SEND);
			return 0;
		}
	}
}

void Si468x_dab_full_scan()
{
	send_debug_msg("----------------------DAB Full Scan----------------------", CRLF_SEND);
	total_services = 0;
	total_ensembles = 0;
	actual_services = 0;

	uint8_t valid_timeout = 0;
	uint8_t fic_q_timeout = 0;

	for(uint8_t freq_index = 0; freq_index < freq_cnt; freq_index++)
	{
		sig_metrics.valid = 0;
		sig_metrics.acq = 0;
		sig_metrics.fic_q = 0;

		valid_timeout = VALID_TIMEOUT;
		fic_q_timeout = FIC_Q_TIMEOUT;

		Si468x_dab_tune_freq(freq_index);
		while(!sig_metrics.valid || !sig_metrics.acq /*|| sig_metrics.fic_q < 100*/)
		{
			Si468x_dab_digrad_status();
			valid_timeout--;
			if(!valid_timeout)
			{
				send_debug_msg("Ensemble not found.", CRLF_SEND);
				break;
			}
			HAL_Delay(SIGNAL_CHECK_INTERVAL);
		}
		if(valid_timeout)
		{
			while(sig_metrics.fic_q < 100)
			{
				Si468x_dab_digrad_status();
				fic_q_timeout--;
				if(!fic_q_timeout)
				{
					send_debug_msg("Ensemble not found.", CRLF_SEND);
					break;
				}
				HAL_Delay(SIGNAL_CHECK_INTERVAL);
			}
		}

		if(valid_timeout && fic_q_timeout)
		{
			while(!dab_events.srv_list || dab_events.srv_list_int)
			{
				Si468x_dab_get_event_status();
				HAL_Delay(10);
			}

			if(Si468x_dab_get_ensemble_info())
			{
				Si468x_dab_get_digital_service_list();
			}
		}
	}

	//display info about  ensembles
	send_debug_msg("Ensembles found: ", CRLF_NO_SEND);
	send_debug_msg(itoa(total_ensembles, itoa_buffer, 10), CRLF_SEND);

	send_debug_msg("--------------------------------------------------", CRLF_SEND);
	send_debug_msg("| Number", CRLF_NO_SEND);
	send_debug_msg(" | Label          ", CRLF_NO_SEND);
	send_debug_msg("| Frequency ", CRLF_NO_SEND);
	send_debug_msg(" | Channel |", CRLF_SEND);

	for(uint8_t ensembles_index = 0; ensembles_index < total_ensembles; ensembles_index++)
	{
		send_debug_msg("| ", CRLF_NO_SEND);
		send_debug_msg(itoa(ensembles_index, itoa_buffer, 10), CRLF_NO_SEND);
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
		send_debug_msg("  |", CRLF_SEND);
	}
	send_debug_msg("--------------------------------------------------", CRLF_SEND);

	//display info about services
	send_debug_msg("Services found: ", CRLF_NO_SEND);
	send_debug_msg(itoa(total_services, itoa_buffer, 10), CRLF_SEND);


	send_debug_msg("--------------------------------------------------------------------------------------------------------", CRLF_SEND);
	send_debug_msg("| Number | Name             | Ensemble Name   | Frequency  | Channel | PTY | Service ID | Component ID |", CRLF_SEND);

	for(uint8_t services_index = 0; services_index < total_services; services_index++)
	{
		//Number
		send_debug_msg("| ", CRLF_NO_SEND);
		send_debug_msg(itoa(services_index, itoa_buffer, 10), CRLF_NO_SEND);
		if(services_index < 10)
		{
			send_debug_msg(" ", CRLF_NO_SEND);
		}
		send_debug_msg("     | ", CRLF_NO_SEND);

		//Name
		if(services_list[services_index].name[0])
		{
			send_debug_msg(services_list[services_index].name, CRLF_NO_SEND);
		}
		else
		{
			send_debug_msg("Unknown name    ", CRLF_NO_SEND);
		}
		send_debug_msg(" | ", CRLF_NO_SEND);

		//Ensemble Name
		for(uint8_t i = 0; i < total_ensembles; i++)
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
		send_debug_msg("  | ", CRLF_NO_SEND);

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

}

void Si468x_dab_get_audio_info()
{
	uint16_t bit_rate, sample_rate;
	uint8_t xpad_indicator, ps_flag, sbr_flag, audio_mode;

	send_debug_msg("--------------Getting audio info from Si468x-------------", CRLF_SEND);
	dab_spi_tx_buffer[0] = SI468X_CMD_DAB_GET_AUDIO_INFO;
	dab_spi_tx_buffer[1] = 0x03; 	//	ber_option: 1 = long_term, 0 = short_term, ber_ack: 2 = reset counters, 0 = don't reset counters

	status = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(20, dab_spi_rx_buffer);

	bit_rate = (dab_spi_rx_buffer[5] << 8) + dab_spi_rx_buffer[4];
	sample_rate = (dab_spi_rx_buffer[7] << 8) + dab_spi_rx_buffer[6];

	xpad_indicator = (dab_spi_rx_buffer[8] & 0x30) >> 4;
	ps_flag = (dab_spi_rx_buffer[8] & 0x08) >> 3;
	sbr_flag = (dab_spi_rx_buffer[8] & 0x04) >> 2;
	audio_mode = dab_spi_rx_buffer[8] & 0x03;

	sig_metrics.fic_bit_cnt = dab_spi_rx_buffer[12] + (dab_spi_rx_buffer[13] << 8) + (dab_spi_rx_buffer[14] << 16) + (dab_spi_rx_buffer[15] << 24);
	sig_metrics.fic_err_cnt = dab_spi_rx_buffer[16] + (dab_spi_rx_buffer[17] << 8) + (dab_spi_rx_buffer[18] << 16) + (dab_spi_rx_buffer[19] << 24);

	DisplayDabStatus(sig_metrics);

	send_debug_msg("Bitrate: ", CRLF_NO_SEND);
	send_debug_msg(itoa(bit_rate, itoa_buffer, 10), CRLF_SEND);

	send_debug_msg("Sample rate: ", CRLF_NO_SEND);
	send_debug_msg(itoa(sample_rate, itoa_buffer, 10), CRLF_SEND);

	send_debug_msg("XPAD Indicator: ", CRLF_NO_SEND);
	send_debug_msg(itoa(xpad_indicator, itoa_buffer, 10), CRLF_SEND);

	send_debug_msg("PS Flag: ", CRLF_NO_SEND);
	send_debug_msg(itoa(ps_flag, itoa_buffer, 10), CRLF_SEND);

	send_debug_msg("SBR Flag: ", CRLF_NO_SEND);
	send_debug_msg(itoa(sbr_flag, itoa_buffer, 10), CRLF_SEND);

	send_debug_msg("Audio Mode: ", CRLF_NO_SEND);
	send_debug_msg(itoa(audio_mode, itoa_buffer, 10), CRLF_SEND);

}

void Si468x_dab_get_event_status()
{
	send_debug_msg("-----------Getting DAB Event Status from Si468x-------------", CRLF_SEND);
	dab_spi_tx_buffer[0] = SI468X_CMD_DAB_GET_EVENT_STATUS;
	dab_spi_tx_buffer[1] = 0x03;	//+2 = Clears the AUDIO_STATUS error indicators BLK_ERROR and BLK_LOSS of this command, +1 = Clears all pending DAB event interrupt bits

	status = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(9, dab_spi_rx_buffer);

	dab_events.recfg_int = (dab_spi_rx_buffer[4] & 0x80 >> 7);
	dab_events.recfg_wrn_int = (dab_spi_rx_buffer[4] & 0x40 >> 6);
	dab_events.audio_int = (dab_spi_rx_buffer[4] & 0x20 >> 5);
	dab_events.anno_int = (dab_spi_rx_buffer[4] & 0x10 >> 4);
	dab_events.oe_serv_int = (dab_spi_rx_buffer[4] & 0x08 >> 3);
	dab_events.serv_link_int = (dab_spi_rx_buffer[4] & 0x04 >> 2);
	dab_events.freq_info_int = (dab_spi_rx_buffer[4] & 0x02 >> 1);
	dab_events.srv_list_int = (dab_spi_rx_buffer[4] & 0x01);

	dab_events.audio = (dab_spi_rx_buffer[5] & 0x20 >> 5);
	dab_events.anno = (dab_spi_rx_buffer[5] & 0x10 >> 4);
	dab_events.oe_serv = (dab_spi_rx_buffer[5] & 0x08 >> 3);
	dab_events.serv_link = (dab_spi_rx_buffer[5] & 0x04 >> 2);
	dab_events.freq_info = (dab_spi_rx_buffer[5] & 0x02 >> 1);
	dab_events.srv_list = (dab_spi_rx_buffer[5] & 0x01);

	dab_events.srv_list_ver_lo = dab_spi_rx_buffer[6];
	dab_events.srv_list_ver_hi = dab_spi_rx_buffer[7];

	dab_events.mute_eng = (dab_spi_rx_buffer[8] & 0x08 >> 3);
	dab_events.sm_eng = (dab_spi_rx_buffer[8] & 0x04 >> 2);
	dab_events.blk_error = (dab_spi_rx_buffer[8] & 0x02 >> 1);
	dab_events.blk_loss = (dab_spi_rx_buffer[8] & 0x01);
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

	send_debug_msg("Global ID: ", CRLF_NO_SEND);
	send_debug_msg(itoa(global_id, itoa_buffer, 10), CRLF_SEND);

	send_debug_msg("Language: ", CRLF_NO_SEND);
	send_debug_msg(itoa(language, itoa_buffer, 10), CRLF_SEND);

	send_debug_msg("Label:            ", CRLF_NO_SEND);
	send_debug_msg((char*)label, CRLF_SEND);

	send_debug_msg("Char abbrev mask: ", CRLF_NO_SEND);
	send_debug_msg(itoa(global_id, itoa_buffer, 2), CRLF_SEND);

}


void Si468x_dab_test_get_ber_info()	//póki co odczyt BER nie działa
{
		send_debug_msg("--------------Getting BER info from Si468x---------------", CRLF_SEND);
		dab_spi_tx_buffer[0] = SI468X_CMD_DAB_TEST_GET_BER_INFO;
		dab_spi_tx_buffer[1] = 0x00; 	//	always 0, as in documentation

		status = Si468x_write_command(2, dab_spi_tx_buffer);
		HAL_Delay(1);
		status = Si468x_read_reply(12, dab_spi_rx_buffer);

		sig_metrics.fic_bit_cnt = dab_spi_rx_buffer[8] + (dab_spi_rx_buffer[9] << 8) + (dab_spi_rx_buffer[10] << 16) + (dab_spi_rx_buffer[11] << 24);
		sig_metrics.fic_err_cnt = dab_spi_rx_buffer[4] + (dab_spi_rx_buffer[5] << 8) + (dab_spi_rx_buffer[6] << 16) + (dab_spi_rx_buffer[7] << 24);

		DisplayDabStatus(sig_metrics);
}

void Si468x_dab_get_time()
{
	uint16_t year;
	uint8_t months, days, hours, minutes, seconds;

	send_debug_msg("--------------Getting time from Si468x-------------------", CRLF_SEND);
	dab_spi_tx_buffer[0] = SI468X_CMD_DAB_GET_TIME;
	dab_spi_tx_buffer[1] = 0x00; 	//0 - local time, 1 - UTC

	status = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(11, dab_spi_rx_buffer);

	year = (dab_spi_rx_buffer[5] << 8) + dab_spi_rx_buffer[4];
	months = dab_spi_rx_buffer[6];
	days = dab_spi_rx_buffer[7];
	hours = dab_spi_rx_buffer[8];
	minutes = dab_spi_rx_buffer[9];
	seconds = dab_spi_rx_buffer[10];

	send_debug_msg(itoa(hours, itoa_buffer, 10), CRLF_NO_SEND);
	send_debug_msg(":", CRLF_NO_SEND);
	send_debug_msg(itoa(minutes, itoa_buffer, 10), CRLF_NO_SEND);
	send_debug_msg(":", CRLF_NO_SEND);
	send_debug_msg(itoa(seconds, itoa_buffer, 10), CRLF_SEND);

	send_debug_msg(itoa(days, itoa_buffer, 10), CRLF_NO_SEND);
	send_debug_msg(".", CRLF_NO_SEND);
	send_debug_msg(itoa(months, itoa_buffer, 10), CRLF_NO_SEND);
	send_debug_msg(".", CRLF_NO_SEND);
	send_debug_msg(itoa(year, itoa_buffer, 10), CRLF_SEND);

}


void Si468x_play_next_station()
{
	send_debug_msg("---------------------------------", CRLF_SEND);
	send_debug_msg("Playing Station ", CRLF_NO_SEND);
	send_debug_msg(itoa(actual_station, itoa_buffer, 10), CRLF_SEND);
	Si468x_dab_tune_freq(services_list[actual_station].freq_id); //CH_11B - PR Kraków, CH_9C - DABCOM Tarnów, CH_10D - PR Kielce,
	Si468x_dab_get_component_info(services_list[actual_station].service_id, services_list[actual_station].components[0].subchannel_id);
	Si468x_dab_start_digital_service(services_list[actual_station].service_id, services_list[actual_station].components[0].subchannel_id);

	Si468x_dab_digrad_status();
	Si468x_dab_get_audio_info();



	actual_station++;
	if(actual_station == total_services)
	{
	  actual_station = 0;
	}
}
