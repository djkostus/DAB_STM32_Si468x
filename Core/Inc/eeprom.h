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


// Opóźnienie niezbędne do zakończenia zapisu
// Funkcja wprowadza automatycznie opóźnienie tylko, gdy jest ono potrzebne
void eeprom_wait();

// Odczyt danych z pamięci EEPROM
// addr - początkowy adres danych
// data - bufor na odczytane dane
// size - liczba bajtów do odczytania
// return - HAL_OK jeśli odczyt zakończony sukcesem, wpp. błąd
//max 64B naraz z uwagi na stronicowanie (dla AT24C256)
HAL_StatusTypeDef eeprom_read(uint32_t addr, void* data, uint32_t size);


// Zapis danych do pamięci EEPROM
// addr - początkowy adres danych
// data - bufor na dane zapisu
// size - liczba bajtów do zapisania
// return - HAL_OK jeśli zapis zakończony sukcesem, wpp. błąd
//max 64B naraz z uwagi na stronicowanie (dla AT24C256)
HAL_StatusTypeDef eeprom_write(uint32_t addr, const void* data, uint32_t size);

void eeprom_clear_scanning_data();
void eeprom_save_scanning_data(dab_service_t _services_list[], uint8_t _total_services, dab_ensemble_t _ensembles_list[], uint8_t _total_ensembles);

void eeprom_show();
#endif /* INC_EEPROM_H_ */
