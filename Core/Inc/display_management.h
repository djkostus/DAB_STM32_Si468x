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

void Display_booting_screen();

void Display_main_screen_background();
void Display_main_screen_data(dab_service_t _services_list[], dab_ensemble_t _ensembles_list[], dab_management_t _dab_management, dab_audio_info_t _dab_audio_info);
void Display_main_screen_dls(uint8_t* _dls_label);
void Display_main_screen_empty();

void Display_stations_list_background();
void Display_stations_list_data(int start_station_index, dab_management_t _dab_management, dab_service_t* _dab_service_list);

void Display_scanning_screen_background();
void Display_scanning_screen_data(dab_digrad_status_t _digrad_status, dab_management_t _dab_management);
void Display_scanning_screen_complete();

void Display_dab_digrad_status_background();
void Display_dab_digrad_status_data(dab_digrad_status_t digrad_status);

void Display_settings_screen_background();
void Display_settings_screen_data(dab_management_t _dab_management);

void Display_time(time_t time_val);

void Display_show_station(dab_service_t _services_list[], uint8_t station, uint8_t _total_services);
void Display_hide_station();
void Display_show_empty_station();





#endif /* INC_DISPLAY_H_ */
