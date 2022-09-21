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

uint8_t system_state = main_screen;
uint8_t playing_state = playing;

uint8_t state_change_done = 0;

int srv_list_start_index = 0;

touch_coordinates_t touch_coordinates;

dab_digrad_status_t dab_digrad_status_to_display;
dab_management_t dab_management_to_display;
dab_service_t* services_list_to_display;
dab_ensemble_t* ensembles_list_to_display;
dab_audio_info_t dab_audio_info_to_display;

uint8_t scan_cancel_flag = 0;

uint8_t scan_complete_flag = 0;

void state_machine()
{
	if(state_change_done)
	{
		touch_coordinates = Touch_read();
	}
	else
	{
		touch_coordinates.x = 0;
		touch_coordinates.y = 0;
	}

	dab_management_to_display = get_dab_management();
	dab_digrad_status_to_display = Si468x_dab_digrad_status();

	services_list_to_display = get_dab_service_list();
	ensembles_list_to_display = get_dab_ensemble_list();
//	dab_audio_info_to_display = Si468x_dab_get_audio_info();
	Si468x_dab_get_time();

	switch(system_state)
	{
	case main_screen:

		if(!state_change_done)
		{
			if(!dab_management_to_display.total_services)
			{
				playing_state = not_playing;
			}
			Display_main_screen_background();
			dab_audio_info_to_display = Si468x_dab_get_audio_info();
			state_change_done = 1;
		}

		else
		{
			if(playing_state == playing)
			{
				Display_main_screen_data(services_list_to_display, ensembles_list_to_display, dab_management_to_display, dab_audio_info_to_display);
			}
			else
			{
				Display_main_screen_empty();
			}
		}

		//left button handling (settings)
		if(touch_coordinates.x >  5 && touch_coordinates.x < 157 && touch_coordinates.y > 195 && touch_coordinates.y < 235)
		{
			state_change_done = 0;
			system_state = settings;
		}

		//right button handling (service list)
		if(touch_coordinates.x >  163 && touch_coordinates.x < 315 && touch_coordinates.y > 195 && touch_coordinates.y < 235)
		{
			state_change_done = 0;
			system_state = services_list_screen;
		}

		break;

	case services_list_screen:
		if(!state_change_done)
		{
//			srv_list_start_index = 0;
			Display_stations_list_background();
			Display_stations_list_data(srv_list_start_index, dab_management_to_display, services_list_to_display);
			state_change_done = 1;
		}

		//left button handling (main)
		if(touch_coordinates.x > 5 && touch_coordinates.x < 157 && touch_coordinates.y > 195 && touch_coordinates.y < 235)
		{
			state_change_done = 0;
			system_state = main_screen;
		}

		//right button handling (signal info)
		if(touch_coordinates.x > 163 && touch_coordinates.x < 315 && touch_coordinates.y > 195 && touch_coordinates.y < 235)
		{
			state_change_done = 0;
			system_state = signal_info;
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
				Display_stations_list_data(srv_list_start_index, dab_management_to_display, services_list_to_display);
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
				Display_stations_list_data(srv_list_start_index, dab_management_to_display, services_list_to_display);
			}
		}

		//Service 1 button handling
		if(touch_coordinates.x > 5 && touch_coordinates.x < 285 && touch_coordinates.y > 20 && touch_coordinates.y < 50)
		{
			if(dab_management_to_display.total_services)
			{
				state_change_done = 0;
				play_station(srv_list_start_index);
				playing_state = playing;
				system_state = main_screen;
			}
		}

		//Service 2 button handling
		if(touch_coordinates.x > 5 && touch_coordinates.x < 285 && touch_coordinates.y > 55 && touch_coordinates.y < 85)
		{
			if(dab_management_to_display.total_services)
			{
				state_change_done = 0;

				if(srv_list_start_index + 1 >= dab_management_to_display.total_services)
				{
					play_station(srv_list_start_index + 1 - dab_management_to_display.total_services);
				}
				else
				{
					play_station(srv_list_start_index + 1);
				}

				playing_state = playing;
				system_state = main_screen;
			}
		}

		//Service 3 button handling
		if(touch_coordinates.x > 5 && touch_coordinates.x < 285 && touch_coordinates.y > 90 && touch_coordinates.y < 120)
		{
			if(dab_management_to_display.total_services)
			{
				state_change_done = 0;

				if(srv_list_start_index + 2 >= dab_management_to_display.total_services)
				{
					play_station(srv_list_start_index + 2 - dab_management_to_display.total_services);
				}
				else
				{
					play_station(srv_list_start_index + 2);
				}

				playing_state = playing;
				system_state = main_screen;
			}
		}

		//Service 4 button handling
		if(touch_coordinates.x > 5 && touch_coordinates.x < 285 && touch_coordinates.y > 125 && touch_coordinates.y < 155)
		{
			if(dab_management_to_display.total_services)
			{
				state_change_done = 0;

				if(srv_list_start_index + 3 >= dab_management_to_display.total_services)
				{
					play_station(srv_list_start_index + 3 - dab_management_to_display.total_services);
				}
				else
				{
					play_station(srv_list_start_index + 3);
				}

				playing_state = playing;
				system_state = main_screen;
			}
		}

		//Service 5 button handling
		if(touch_coordinates.x > 5 && touch_coordinates.x < 285 && touch_coordinates.y > 160 && touch_coordinates.y < 190)
		{
			if(dab_management_to_display.total_services)
			{
				state_change_done = 0;

				if(srv_list_start_index + 4 >= dab_management_to_display.total_services)
				{
					play_station(srv_list_start_index + 4 - dab_management_to_display.total_services);
				}
				else
				{
					play_station(srv_list_start_index + 4);
				}

				playing_state = playing;
				system_state = main_screen;
			}
		}

		break;

	case signal_info:
		if(!state_change_done)
		{
			Display_dab_digrad_status_background();
			state_change_done = 1;
		}

		Display_dab_digrad_status_data(dab_digrad_status_to_display);

		//left button handling (services list)
		if(touch_coordinates.x >  5 && touch_coordinates.x < 157 && touch_coordinates.y > 195 && touch_coordinates.y < 235)
		{
			state_change_done = 0;
			system_state = services_list_screen;
		}

		//right button handling (settings)
		if(touch_coordinates.x >  163 && touch_coordinates.x < 315 && touch_coordinates.y > 195 && touch_coordinates.y < 235)
		{
			state_change_done = 0;
			system_state = settings;
		}
		break;

	case scanning:
		if(!state_change_done)
		{
			playing_state = not_playing;
			Display_scanning_screen_background();
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
			system_state = settings;
		}

		break;

	case settings:
		if(!state_change_done)
		{
			Display_settings_screen_background();
			scan_cancel_flag = 0;
			state_change_done = 1;
		}

		Display_settings_screen_data(dab_management_to_display);

		//vol+ button handling
		if(touch_coordinates.x > 163 && touch_coordinates.x < 315 && touch_coordinates.y > 60 && touch_coordinates.y < 100)
		{
			dab_management_to_display.audio_volume += 2;
			if(dab_management_to_display.audio_volume > 63)
			{
				dab_management_to_display.audio_volume = 63;
			}
			Si468x_set_audio_volume(dab_management_to_display.audio_volume);
		}

		//vol- button handling
		if(touch_coordinates.x > 5 && touch_coordinates.x < 157 && touch_coordinates.y > 60 && touch_coordinates.y < 100)
		{
			dab_management_to_display.audio_volume -= 2;
			if((dab_management_to_display.audio_volume < 0) || (dab_management_to_display.audio_volume > 63))
			{
				dab_management_to_display.audio_volume = 0;
			}
			Si468x_set_audio_volume(dab_management_to_display.audio_volume);
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
			system_state = scanning;
		}

		//touch calibration button handling
		if(touch_coordinates.x > 163 && touch_coordinates.x < 315 && touch_coordinates.y > 150 && touch_coordinates.y < 190)
		{

		}

		//left button handling (signal info)
		if(touch_coordinates.x >  5 && touch_coordinates.x < 157 && touch_coordinates.y > 195 && touch_coordinates.y < 235)
		{
			state_change_done = 0;
			system_state = signal_info;
		}

		//right button handling (main screen)
		if(touch_coordinates.x >  163 && touch_coordinates.x < 315 && touch_coordinates.y > 195 && touch_coordinates.y < 235)
		{
			state_change_done = 0;
			system_state = main_screen;
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

