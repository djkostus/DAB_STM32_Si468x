/*
 * eeprom.h
 *
 *  Created on: Sep 15, 2022
 *      Author: Cineq
 */

#ifndef INC_EEPROM_H_
#define INC_EEPROM_H_

#pragma once

#include "stm32f4xx.h"
#include "eeprom_addresses.h"
#include "Si468x.h"


// Delay necessary to complete writing operation.
// This function introduces a delay automatically, only when it is necessary
void eeprom_wait();

// Read data from EEPROM memory
// addr - start adress
// data - buffer for read data
// size - number of bytes to read
// return - HAL_OK if reading operation done successfully, otherwise returns error
// maximum 64B on each read operation due to paging (for AT24C256)
// maximum 32B on each read operation due to paging (for AT24C64)
HAL_StatusTypeDef eeprom_read(uint32_t addr, void* data, uint32_t size);


// Write data to EEPROM memory
// addr - start address
// data - buffer for data to write
// size - number of bytes to write
// return - HAL_OK if writing operation done successfully, otherwise returns error
// maximum 64B on each write operation due to paging (for AT24C256)
// maximum 32B on each write operation due to paging (for AT24C64)
HAL_StatusTypeDef eeprom_write(uint32_t addr, const void* data, uint32_t size);

//this function clears scanning data (saved stations, ensembles, number of stations and number of ensebles)
void eeprom_clear_scanning_data();

//this function save scanning data (saved stations, ensembles, number of stations and number of ensebles)
void eeprom_save_scanning_data(dab_service_t _services_list[], uint8_t _total_services, dab_ensemble_t _ensembles_list[], uint8_t _total_ensembles);

//this functions displays EEPROM data by UART debug port to check if everything is OK
void eeprom_show();

//erase whole eeprom
void eeprom_erase();
void eeprom_erase_2();

void check_eeprom_erased();

#endif /* INC_EEPROM_H_ */
