/*
 * eeprom_addresses.h
 *
 *  Created on: Sep 15, 2022
 *      Author: Cineq
 */

#ifndef INC_EEPROM_ADDRESSES_H_
#define INC_EEPROM_ADDRESSES_H_

#define PAGE_SIZE 32 //32 for 24c64, 64 for 24c256

//maximum number of ensembles and services
#define MAX_ENSEMBLES 10
#define MAX_SERVICES 100

//page 0 - 0...63 - various data for 24c256
//page 0...1 - 0...63 - various data for 24c64
#define LAST_LCD_BRIGTHNESS_ADDR 10
#define LAST_VOLUME_ADDR 11
#define LAST_MODE_ADDR 12

#define FREQ_TABLE_SIZE_ADDR 63

//page 1...3 - 64...255 - frequency table for 24c256
//page 2...7 - 64...255 - frequency table for 24c64
#define FREQ_TABLE_START_ADDR 64

//page 4 - 256...319 - various data connected with scanning for 24c256
//page 8...9 - 256...319 - various data connected with scanning for 24c64
#define SCAN_RELATED_DATA_PAGE 8

#define LAST_FREQUENCY_ADDR 256
#define LAST_FREQ_ID_ADDR 260
#define TOTAL_ENSEMBLES_ADDR 261
#define TOTAL_SERVICES_ADDR 262
#define LAST_STATION_INDEX_ADDR 263

//page 5...14 - 320...959 - ensembles list for 24c256
//page 10...28 - 320...959 - ensembles list for 24c64
#define ENSEMBLES_TABLE_START_ADDR 320

//page 15...114 - 960...7359 - stations list for 24c256
//page 30...229 - 960...7359 - stations list for 24c64
#define SERVICES_TABLE_START_ADDR 960

#endif /* INC_EEPROM_ADDRESSES_H_ */
