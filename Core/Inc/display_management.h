/*
 * display_management.h
 *
 *  Created on: Aug 22, 2022
 *      Author: mgostek
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "main.h"
#include "Si468x.h"
#include "ILI9341_Driver.h"
#include "stdlib.h"

void Display_clear_screen();

void Display_main_screen();

void Display_dab_digrad_status_background();
void Display_dab_digrad_status_data(dab_digrad_status_t digrad_status);

void Display_time(time_t time_val);

void Display_init_screen();

void Display_show_next_station(dab_service_t _services_list, uint8_t _actual_station, uint8_t _total_services);
void Display_hide_next_station();

#endif /* INC_DISPLAY_H_ */
