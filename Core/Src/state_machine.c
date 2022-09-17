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

uint8_t srv_list_start_index = 0;

touch_coordinates_t touch_coordinates;

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

	case services_list:
		if(!state_change_done)
		{
			srv_list_start_index = 0;
			Display_stations_list_background();
			Display_stations_list_data(srv_list_start_index);
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
			srv_list_start_index--;
			if(srv_list_start_index > total_services && srv_list_start_index < 0)
			{
				srv_list_start_index = total_services - 1;
			}

		}

		//down button handling
		if(touch_coordinates.x > 290 && touch_coordinates.x < 315 && touch_coordinates.y > 108 && touch_coordinates.y < 190)
		{
			srv_list_start_index++;
			if(srv_list_start_index >= total_services)
			{
				srv_list_start_index = 0;
			}

		}

		break;

	case signal_info:
		if(!state_change_done)
		{
			Display_dab_digrad_status_background();
			HAL_Delay(1000); //time to complete refresh display
			state_change_done = 1;
		}

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
			state = services_list;
		}



		break;

	case scanning:
		if(!state_change_done)
		{
			Display_scanning_screen_background();
			HAL_Delay(1000); //time to complete refresh display
			state_change_done = 1;
		}

		//cancel button handling
		if(touch_coordinates.x >  5 && touch_coordinates.x < 315 && touch_coordinates.y > 195 && touch_coordinates.y < 235)
		{

		}

		break;

	case settings:
		if(!state_change_done)
		{
			Display_settings_screen_background();
			HAL_Delay(1000); //time to complete refresh display
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

		}

		//touch calibration button handling
		if(touch_coordinates.x > 163 && touch_coordinates.x < 315 && touch_coordinates.y > 150 && touch_coordinates.y < 190)
		{

		}

		//left button handling (services list)
		if(touch_coordinates.x >  5 && touch_coordinates.x < 157 && touch_coordinates.y > 195 && touch_coordinates.y < 235)
		{
			state_change_done = 0;
			state = services_list;
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


