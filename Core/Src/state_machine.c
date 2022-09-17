/*
 * state_machine.c
 *
 *  Created on: Sep 15, 2022
 *      Author: Cineq
 */

#include "state_machine.h"
#include "debug_uart.h"
#include "display_management.h"
#include "Si468x.h"
#include "eeprom.h"
#include "touch.h"

uint8_t state = main_screen;

uint8_t state_change_done = 0;

int srv_list_start_index = 0;

touch_coordinates_t touch_coordinates;

dab_digrad_status_t dab_digrad_status_to_display;
dab_management_t dab_management_to_display;
dab_service_t services_list_to_display[100];
dab_ensemble_t ensembles_list_to_display[10];

uint8_t scan_cancel_flag = 0;

uint8_t scan_complete_flag = 0;

void state_machine()
{
	touch_coordinates = Touch_read();

	switch(state)
	{
	case main_screen:
		if(!state_change_done)
		{
			Display_main_screen_background();
			HAL_Delay(1000); //time to complete refresh display
			state_change_done = 1;
		}

		//left button handling (settings)
		if(touch_coordinates.x >  5 && touch_coordinates.x < 157 && touch_coordinates.y > 195 && touch_coordinates.y < 235)
		{
			state_change_done = 0;
			state = settings;
		}

		//right button handling (signal info)
		if(touch_coordinates.x >  163 && touch_coordinates.x < 315 && touch_coordinates.y > 195 && touch_coordinates.y < 235)
		{
			state_change_done = 0;
			state = signal_info;
		}


		break;

	case services_list_screen:
		if(!state_change_done)
		{
			srv_list_start_index = 0;
			dab_management_to_display = get_dab_management();
			Display_stations_list_background();
			Display_stations_list_data(srv_list_start_index, dab_management_to_display);
			HAL_Delay(1000); //time to complete refresh display
			state_change_done = 1;
		}

		//left button handling (signal info)
		if(touch_coordinates.x > 5 && touch_coordinates.x < 157 && touch_coordinates.y > 195 && touch_coordinates.y < 235)
		{
			state_change_done = 0;
			state = signal_info;
		}

		//right button handling (settings)
		if(touch_coordinates.x > 163 && touch_coordinates.x < 315 && touch_coordinates.y > 195 && touch_coordinates.y < 235)
		{
			state_change_done = 0;
			state = settings;
		}

		//up button handling
		if(touch_coordinates.x > 290 && touch_coordinates.x < 315 && touch_coordinates.y > 20 && touch_coordinates.y < 102)
		{
			if(dab_management_to_display.total_services)
			{
				srv_list_start_index--;
				if(srv_list_start_index > dab_management_to_display.total_services || srv_list_start_index < 0)
				{
					srv_list_start_index = dab_management_to_display.total_services - 1;
				}
				Display_stations_list_data(srv_list_start_index, dab_management_to_display);
			}
		}

		//down button handling
		if(touch_coordinates.x > 290 && touch_coordinates.x < 315 && touch_coordinates.y > 108 && touch_coordinates.y < 190)
		{
			if(dab_management_to_display.total_services)
			{
				srv_list_start_index++;
				if(srv_list_start_index >= dab_management_to_display.total_services)
				{
					srv_list_start_index = 0;
				}
				Display_stations_list_data(srv_list_start_index, dab_management_to_display);
			}
		}

		//Service 1 button handling
//		if(touch_coordinates.x > 163 && touch_coordinates.x < 315 && touch_coordinates.y > 195 && touch_coordinates.y < 235)
//		{
//			state_change_done = 0;
//			state = settings;
//		}

		break;

	case signal_info:
		if(!state_change_done)
		{
			Display_dab_digrad_status_background();
			HAL_Delay(1000); //time to complete refresh display
			state_change_done = 1;
		}
		dab_digrad_status_to_display = Si468x_dab_digrad_status();
		Display_dab_digrad_status_data(dab_digrad_status_to_display);

		//left button handling (main screen)
		if(touch_coordinates.x >  5 && touch_coordinates.x < 157 && touch_coordinates.y > 195 && touch_coordinates.y < 235)
		{
			state_change_done = 0;
			state = main_screen;
		}

		//right button handling (services list)
		if(touch_coordinates.x >  163 && touch_coordinates.x < 315 && touch_coordinates.y > 195 && touch_coordinates.y < 235)
		{
			state_change_done = 0;
			state = services_list_screen;
		}
		break;

	case scanning:
		if(!state_change_done)
		{
			Display_scanning_screen_background();
			HAL_Delay(1000); //time to complete refresh display
			state_change_done = 1;
			scan_complete_flag = Si468x_dab_full_scan();
			if(scan_complete_flag)
			{
//				dab_management_to_display = get_dab_management();
//				Display_scanning_screen_data(dab_digrad_status_to_display, dab_management_to_display);
				Display_scanning_screen_complete();
				scan_complete_flag = 0;
			}
		}
//		dab_digrad_status_to_display = Si468x_dab_digrad_status();
//		Display_scanning_screen_data(dab_digrad_status_to_display);

		//cancel button handling
		if(touch_coordinates.x >  5 && touch_coordinates.x < 315 && touch_coordinates.y > 195 && touch_coordinates.y < 235)
		{
			scan_cancel_flag = 1;
			state_change_done = 0;
			state = settings;
		}

		break;

	case settings:
		if(!state_change_done)
		{
			Display_settings_screen_background();
			HAL_Delay(1000); //time to complete refresh display
			scan_cancel_flag = 0;
			state_change_done = 1;
		}

		//vol+ button handling
		if(touch_coordinates.x > 163 && touch_coordinates.x < 315 && touch_coordinates.y > 60 && touch_coordinates.y < 100)
		{

		}

		//vol- button handling
		if(touch_coordinates.x > 5 && touch_coordinates.x < 157 && touch_coordinates.y > 60 && touch_coordinates.y < 100)
		{

		}

		//backlight+ button handling
		if(touch_coordinates.x > 163 && touch_coordinates.x < 315 && touch_coordinates.y > 105 && touch_coordinates.y < 145)
		{

		}

		//backlight- button handling
		if(touch_coordinates.x > 5 && touch_coordinates.x < 157 && touch_coordinates.y > 105 && touch_coordinates.y < 145)
		{

		}

		//scanning button handling
		if(touch_coordinates.x > 5 && touch_coordinates.x < 157 && touch_coordinates.y > 150 && touch_coordinates.y < 190)
		{
			state_change_done = 0;
			state = scanning;
		}

		//touch calibration button handling
		if(touch_coordinates.x > 163 && touch_coordinates.x < 315 && touch_coordinates.y > 150 && touch_coordinates.y < 190)
		{

		}

		//left button handling (services list)
		if(touch_coordinates.x >  5 && touch_coordinates.x < 157 && touch_coordinates.y > 195 && touch_coordinates.y < 235)
		{
			state_change_done = 0;
			state = services_list_screen;
		}

		//right button handling (main screen)
		if(touch_coordinates.x >  163 && touch_coordinates.x < 315 && touch_coordinates.y > 195 && touch_coordinates.y < 235)
		{
			state_change_done = 0;
			state = main_screen;
		}


		break;

	default:
		break;
	}
}

uint8_t get_scan_cancel_flag()
{
	return scan_cancel_flag;
}

