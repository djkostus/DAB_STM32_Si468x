/*
 * eeprom.c
 *
 *  Created on: Sep 15, 2022
 *      Author: Cineq
 */

#include "eeprom.h"
#include "i2c.h"
#include "debug_uart.h"
#include "dab_defs.h"

#define EEPROM_ADDR     0xA0

#define WRITE_TIMEOUT   20

#define ADDR_SIZE 2	//for 24c08 1, for 24c256 2, because of bigger memory size - we must habe 2 bytes for memory Address

static uint32_t last_write;	//variable used to save time of last writing operation in eeprom_wait function

void eeprom_wait(void)
{
    while (HAL_GetTick() - last_write <= WRITE_TIMEOUT);
}

HAL_StatusTypeDef eeprom_read(uint32_t addr, void* data, uint32_t size) //max 64B in one operation due to paging mechanism (for AT24C256)
{
    eeprom_wait();
    return HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDR, addr, 2, data, size, 500);
}

HAL_StatusTypeDef eeprom_write(uint32_t addr, const void* data, uint32_t size) //max 64B in one operation due to paging mechanism (for AT24C256)
{
    HAL_StatusTypeDef rc;

    eeprom_wait();
    rc = HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDR, addr, 2, (void*)data, size, 500);
    last_write = HAL_GetTick();

    return rc;
}

void eeprom_clear_scanning_data()
{
	send_debug_msg("Clear scan data from EEPROM...", CRLF_SEND);

	 uint8_t clear_page_sequence[64];

	 for (uint8_t i = 0; i < 64; i++)
	 {
		 clear_page_sequence[i] = 0xFF;
	 }

	//clear various data connected with scanning

	eeprom_write(PAGE_SIZE * SCAN_RELATED_DATA_PAGE, &clear_page_sequence, sizeof(clear_page_sequence));


	//clear ensembles list
	for(uint8_t i = 0; i < MAX_ENSEMBLES; i++)
	{
		eeprom_write(ENSEMBLES_TABLE_START_ADDR + PAGE_SIZE * i, &clear_page_sequence, sizeof(clear_page_sequence));
	}

	//clear services list
	for(uint8_t i = 0; i < MAX_SERVICES; i++)
	{
		eeprom_write(SERVICES_TABLE_START_ADDR + PAGE_SIZE * i, &clear_page_sequence, sizeof(clear_page_sequence));
	}
	send_debug_msg("Cleared OK.", CRLF_SEND);
}

void eeprom_save_scanning_data(dab_service_t _services_list[], uint8_t _total_services, dab_ensemble_t _ensembles_list[], uint8_t _total_ensembles)
{
	send_debug_msg("Save scan data to EEPROM...", CRLF_SEND);
	//save total ensembles & total services values
	eeprom_write(TOTAL_ENSEMBLES_ADDR, &_total_ensembles, sizeof(_total_ensembles));
	eeprom_write(TOTAL_SERVICES_ADDR, &_total_services, sizeof(_total_services));

	//save ensembles
	for(uint8_t ensembles_index = 0; ensembles_index < _total_ensembles; ensembles_index++)
	{
		eeprom_write(ENSEMBLES_TABLE_START_ADDR + ensembles_index * PAGE_SIZE, &_ensembles_list[ensembles_index], sizeof(dab_ensemble_t));
	}

	//save services
	for(uint8_t services_index = 0; services_index < _total_services; services_index++)
	{
		eeprom_write(SERVICES_TABLE_START_ADDR + services_index * PAGE_SIZE, &_services_list[services_index], sizeof(dab_service_t));
	}

	send_debug_msg("Saved OK.", CRLF_SEND);
}

void eeprom_show()
{
	char itoa_buffer[64];
	uint8_t _total_ensembles;
	dab_ensemble_t _ensemble;
	uint8_t _total_services;
	dab_service_t _service;

	send_debug_msg("----------------Read from EEPROM----------------------", CRLF_SEND);

	eeprom_read(TOTAL_ENSEMBLES_ADDR, &_total_ensembles, sizeof(_total_ensembles));
	eeprom_read(TOTAL_SERVICES_ADDR, &_total_services, sizeof(_total_services));
	if((_total_ensembles == 0xFF) || (_total_services == 0xFF))
	{
		_total_ensembles = 0;
		_total_services = 0;
		send_debug_msg("List is empty.", CRLF_NO_SEND);
	}

	else
	{
		//display info about  ensembles
		send_debug_msg("Ensembles in EEPROM: ", CRLF_NO_SEND);
		send_debug_msg(itoa(_total_ensembles, itoa_buffer, 10), CRLF_SEND);

		send_debug_msg("--------------------------------------------------", CRLF_SEND);
		send_debug_msg("| Number", CRLF_NO_SEND);
		send_debug_msg(" | Label          ", CRLF_NO_SEND);
		send_debug_msg("| Frequency ", CRLF_NO_SEND);
		send_debug_msg(" | Channel |", CRLF_SEND);

		for(uint8_t ensembles_index = 0; ensembles_index < _total_ensembles; ensembles_index++)
		{
			eeprom_read(ENSEMBLES_TABLE_START_ADDR + ensembles_index * PAGE_SIZE, &_ensemble, sizeof(dab_ensemble_t));

			send_debug_msg("| ", CRLF_NO_SEND);
			send_debug_msg(itoa(ensembles_index, itoa_buffer, 10), CRLF_NO_SEND);
			send_debug_msg("      | ", CRLF_NO_SEND);

			send_debug_msg(_ensemble.label, CRLF_NO_SEND);
			send_debug_msg("| ", CRLF_NO_SEND);

			send_debug_msg(itoa(_ensemble.freq, itoa_buffer, 10), CRLF_NO_SEND);
			send_debug_msg(" kHz | ", CRLF_NO_SEND);

			send_debug_msg(dab_channels_names[_ensemble.freq_id], CRLF_NO_SEND);
			if(_ensemble.freq_id < 20)
			{
				send_debug_msg(" ", CRLF_NO_SEND);
			}
			send_debug_msg("  |", CRLF_SEND);
		}
		send_debug_msg("--------------------------------------------------", CRLF_SEND);

		//display info about services
		send_debug_msg("Services in EEPROM: ", CRLF_NO_SEND);
		send_debug_msg(itoa(_total_services, itoa_buffer, 10), CRLF_SEND);

		send_debug_msg("--------------------------------------------------------------------------------------------------------", CRLF_SEND);
		send_debug_msg("| Number | Name             | Ensemble Name   | Frequency  | Channel | PTY | Service ID | Component ID |", CRLF_SEND);

		for(uint8_t services_index = 0; services_index < _total_services; services_index++)
		{
			eeprom_read(SERVICES_TABLE_START_ADDR + services_index * PAGE_SIZE, &_service, sizeof(dab_service_t));

			//Number
			send_debug_msg("| ", CRLF_NO_SEND);
			send_debug_msg(itoa(services_index, itoa_buffer, 10), CRLF_NO_SEND);
			if(services_index < 10)
			{
				send_debug_msg(" ", CRLF_NO_SEND);
			}
			send_debug_msg("     | ", CRLF_NO_SEND);

			//Name
			if(_service.name[0])
			{
				send_debug_msg(_service.name, CRLF_NO_SEND);
			}
			else
			{
				send_debug_msg("Unknown name    ", CRLF_NO_SEND);
			}
			send_debug_msg(" | ", CRLF_NO_SEND);

			//Ensemble Name
			for(uint8_t i = 0; i < _total_ensembles; i++)
			{
				eeprom_read(ENSEMBLES_TABLE_START_ADDR + PAGE_SIZE * i, &_ensemble, sizeof(dab_ensemble_t));

				if(_ensemble.freq_id == _service.freq_id)
				{
					send_debug_msg(_ensemble.label, CRLF_NO_SEND);
					break;
				}
			}
			send_debug_msg(" | ", CRLF_NO_SEND);

			//Frequency
			send_debug_msg(itoa(_service.freq, itoa_buffer, 10), CRLF_NO_SEND);
			send_debug_msg(" kHz | ", CRLF_NO_SEND);

			//Channel
			send_debug_msg(dab_channels_names[_service.freq_id], CRLF_NO_SEND);
			if(_service.freq_id < 20)
			{
				send_debug_msg(" ", CRLF_NO_SEND);
			}
			send_debug_msg("  | ", CRLF_NO_SEND);

			//PTY
			send_debug_msg(itoa(_service.p_ty, itoa_buffer, 10), CRLF_NO_SEND);
			if(_service.p_ty < 10)
			{
				send_debug_msg(" ", CRLF_NO_SEND);
			}
			send_debug_msg("  | ", CRLF_NO_SEND);

			//Service ID
			send_debug_msg("0x", CRLF_NO_SEND);
			send_debug_msg(itoa(_service.service_id, itoa_buffer, 16), CRLF_NO_SEND);
			send_debug_msg("     | ", CRLF_NO_SEND);

			//Component ID
			send_debug_msg("0x", CRLF_NO_SEND);
			send_debug_msg(itoa(_service.components[0].subchannel_id, itoa_buffer, 16), CRLF_NO_SEND);
			send_debug_msg("          |", CRLF_SEND);
		}
		send_debug_msg("--------------------------------------------------------------------------------------------------------", CRLF_SEND);
	}
}
