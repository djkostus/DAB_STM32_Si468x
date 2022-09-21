/*
 * display.c
 *
 *  Created on: Aug 22, 2022
 *      Author: mgostek
 */

#include "display_management.h"
#include <string.h>
#include "eeprom.h"
#include "dab_defs.h"

static char itoa_buffer[64];

static uint8_t display_freeze = 0;


void Display_clear_screen()
{
	ILI9341_Fill_Screen(BLACK);
	ILI9341_Draw_Line(YELLOW, 0, 17, 320, 17);
}

void Display_booting_screen()
{
	ILI9341_Fill_Screen(BLACK);
	ILI9341_Draw_String(95, 40, WHITE, BLACK, "MASTER'S THESIS", 2);
	ILI9341_Draw_String(70, 60, WHITE, BLACK, "DAB+ DEVELOPMENT BOARD", 2);
	ILI9341_Draw_String(130, 80, WHITE, BLACK, "AUTHOR:", 2);
	ILI9341_Draw_String(100, 100, WHITE, BLACK, "MARCIN GOSTEK", 2);
	ILI9341_Draw_String(90, 120, WHITE, BLACK, "THESIS SUPERVISOR:", 2);
	ILI9341_Draw_String(50, 140, WHITE, BLACK, "JACEK KOLODZIEJ, Beng, PhD", 2);
	ILI9341_Draw_String(90, 160, WHITE, BLACK, "AGH KRAKOW, 2022", 2);
	ILI9341_Draw_String(120, 180, WHITE, BLACK, "Booting...", 2);
}

void Display_main_screen_background()
{
	Display_clear_screen();

	ILI9341_Draw_String(5, 1, WHITE, BLACK, "DAB RECEIVER", 2); //Naglowek

	//Station name background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 20, 140, 40);

	//Bar1 background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 45, 140, 65);

	//Bar2 background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 70, 140, 90);

	//BAr3 background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 95, 140, 115);

	//BAr4 background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 120, 140, 140);

	//BAr5 background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 145, 140, 165);

	//BAr6 background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 170, 140, 190);

	//Station picture background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 145, 20, 315, 190);

	//buttons to other menus
	ILI9341_Draw_Filled_Rectangle(ORANGE, 5, 195, 157, 235);
	ILI9341_Draw_Filled_Rectangle(ORANGE, 163, 195, 315, 235);
	ILI9341_Draw_String(50, 207, WHITE, ORANGE, "SETTINGS", 2);
	ILI9341_Draw_String(192, 207, WHITE, ORANGE, "SERVICE LIST", 2);
}

void Display_main_screen_data(dab_service_t* _services_list, dab_ensemble_t* _ensembles_list, dab_management_t _dab_management, dab_audio_info_t _dab_audio_info)
{
	//service Name
	ILI9341_Draw_String(8, 23, WHITE, DARKGREY, _services_list[_dab_management.actual_station].name, 2);

	//Bar1
	ILI9341_Draw_String(8, 48, WHITE, DARKGREY, "", 2);

	//Bar2
	ILI9341_Draw_String(8, 73, WHITE, DARKGREY, "", 2);

	//BAr3
	ILI9341_Draw_String(8, 98, WHITE, DARKGREY, "", 2);

	//BAr4 (bitrate, sample rate)
	if(_dab_audio_info.audio_bit_rate < 100)
	{
		ILI9341_Draw_String(8, 123, WHITE, DARKGREY, " ", 2);
		ILI9341_Draw_String(17, 123, WHITE, DARKGREY, itoa(_dab_audio_info.audio_bit_rate, itoa_buffer, 10), 2);
	}
	else
	{
		ILI9341_Draw_String(8, 123, WHITE, DARKGREY, itoa(_dab_audio_info.audio_bit_rate, itoa_buffer, 10), 2);
	}
	ILI9341_Draw_String(36, 123, WHITE, DARKGREY, "kbps", 2);


	ILI9341_Draw_String(78, 123, WHITE, DARKGREY, itoa(_dab_audio_info.audio_sample_rate, itoa_buffer, 10), 2);
	ILI9341_Draw_String(122, 123, WHITE, DARKGREY, "Hz", 2);

	//BAr5 (Ensemble Name)
	for(uint8_t i = 0; i < _dab_management.total_ensembles; i++)
	{
		if(_ensembles_list[i].freq_id == _services_list[_dab_management.actual_station].freq_id)
		{
			ILI9341_Draw_String(8, 148, WHITE, DARKGREY, _ensembles_list[i].label, 2);
		}
	}

	//BAr6 (Freq, Channel)
	ILI9341_Draw_String(8, 173, WHITE, DARKGREY, itoa(_services_list[_dab_management.actual_station].freq / 1000, itoa_buffer, 10), 2);
	ILI9341_Draw_String(31, 173, WHITE, DARKGREY, ".", 2);
	ILI9341_Draw_String(38, 173, WHITE, DARKGREY, itoa(_services_list[_dab_management.actual_station].freq % 1000 ,itoa_buffer, 10), 2);
	ILI9341_Draw_String(65, 173, WHITE, DARKGREY, "MHz", 2);

	ILI9341_Draw_String(95, 173, WHITE, DARKGREY, "CH", 2);
	ILI9341_Draw_String(115, 173, WHITE, DARKGREY, dab_channels_names[_services_list[_dab_management.actual_station].freq_id], 2);

	//Station picture

}

void Display_main_screen_empty()
{
	//service Name
	ILI9341_Draw_String(8, 23, WHITE, DARKGREY, "No service      ", 2);

	//Bar1
	ILI9341_Draw_String(8, 48, WHITE, DARKGREY, "is playing!     ", 2);

	//Bar2
	ILI9341_Draw_String(8, 73, WHITE, DARKGREY, "Select service  ", 2);

	//BAr3
	ILI9341_Draw_String(8, 98, WHITE, DARKGREY, "from service list", 2);

	//BAr4
	ILI9341_Draw_String(8, 123, WHITE, DARKGREY, "to play.        ", 2);

	//BAr5 (Ensemble Name)

	ILI9341_Draw_String(8, 148, WHITE, DARKGREY, "                ", 2);

	//BAr6 (Freq, Channel)
	ILI9341_Draw_String(8, 173, WHITE, DARKGREY, "                ", 2);

	//Station picture

}


void Display_stations_list_background()
{
	Display_clear_screen();

	ILI9341_Draw_String(5, 1, WHITE, BLACK, "DAB SERVICE LIST", 2); //Naglowek

	//SERVICE 1 background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 20, 285, 50);
	//SERVICE 2 background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 55, 285, 85);
	//SERVICE 3 background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 90, 285, 120);
	//SERVICE 4 background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 125, 285, 155);
	//SERVICE 5 background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 160, 285, 190);

	//up button
	ILI9341_Draw_Filled_Rectangle(ORANGE, 290, 20, 315, 102);
	ILI9341_Draw_String(295, 56, WHITE, ORANGE, "/", 2);
	ILI9341_Draw_String(302, 56, WHITE, ORANGE, "\\", 2);
	//down button
	ILI9341_Draw_Filled_Rectangle(ORANGE, 290, 108, 315, 190);
	ILI9341_Draw_String(295, 146, WHITE, ORANGE, "\\", 2);
	ILI9341_Draw_String(302, 146, WHITE, ORANGE, "/", 2);

	//buttons to other menus
	ILI9341_Draw_Filled_Rectangle(ORANGE, 5, 195, 157, 235);
	ILI9341_Draw_Filled_Rectangle(ORANGE, 163, 195, 315, 235);
	ILI9341_Draw_String(37, 207, WHITE, ORANGE, "MAIN SCREEN", 2);
	ILI9341_Draw_String(195, 207, WHITE, ORANGE, "SIGNAL INFO", 2);

}

void Display_stations_list_data(int start_station_index, dab_management_t _dab_management, dab_service_t* _dab_service_list)
{
	if(_dab_management.total_services)
	{
		for(uint8_t i = 0; i < 5; i++)
		{
			if((i + start_station_index) > (_dab_management.total_services - 1))
			{
				start_station_index = 0 - i;
			}

			if((i + start_station_index + 1) < 10)
			{
				ILI9341_Draw_String(8, 28 + 35 * i, WHITE, DARKGREY, " ", 2);
				ILI9341_Draw_String(17, 28 + 35 * i, WHITE, DARKGREY, itoa(i + start_station_index + 1, itoa_buffer, 10), 2);
			}
			else
			{
				ILI9341_Draw_String(8, 28 + 35 * i, WHITE, DARKGREY, itoa(i + start_station_index + 1, itoa_buffer, 10), 2);
			}

			ILI9341_Draw_String(24, 28 + 35 * i, WHITE, DARKGREY, ".", 2);

			ILI9341_Draw_String(34, 28 + 35 * i, WHITE, DARKGREY, _dab_service_list[i + start_station_index].name, 2);
			ILI9341_Draw_String(171, 28 + 35 * i, WHITE, DARKGREY, dab_channels_names[_dab_service_list[i + start_station_index].freq_id], 2);
			ILI9341_Draw_String(202, 28 + 35 * i, WHITE, DARKGREY, itoa(_dab_service_list[i + start_station_index].freq / 1000, itoa_buffer, 10), 2);
			ILI9341_Draw_String(225, 28 + 35 * i, WHITE, DARKGREY, ".", 2);
			ILI9341_Draw_String(232, 28 + 35 * i, WHITE, DARKGREY, itoa(_dab_service_list[i + start_station_index].freq % 1000, itoa_buffer, 10), 2);
			ILI9341_Draw_String(259, 28 + 35 * i, WHITE, DARKGREY, "MHz", 2);
		}
	}

	else
	{
		ILI9341_Draw_String(8, 28, WHITE, DARKGREY, "Service list is empty!", 2);
		ILI9341_Draw_String(8, 63, WHITE, DARKGREY, "Select DAB BAND SCAN", 2);
		ILI9341_Draw_String(8, 98, WHITE, DARKGREY, "from SETTINGS screen", 2);
		ILI9341_Draw_String(8, 133, WHITE, DARKGREY, "to find services available", 2);
		ILI9341_Draw_String(8, 168, WHITE, DARKGREY, "in your localization.", 2);
	}

}

void Display_scanning_screen_background()
{
	Display_clear_screen();

	ILI9341_Draw_String(5, 1, WHITE, BLACK, "DAB SCANNING", 2); //Naglowek

	//SCANNING DAB STRING background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 20, 315, 40);
	ILI9341_Draw_String(80, 23, WHITE, DARKGREY, "Scanning DAB Band...", 2);

	//Scanning status bar background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 45, 315, 65);

	//Found ensembles background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 70, 315, 90);
	ILI9341_Draw_String(10, 73, WHITE, DARKGREY, "Ensembles found:", 2);

	//Found stations background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 95, 315, 115);
	ILI9341_Draw_String(10, 98, WHITE, DARKGREY, "Services found:", 2);

	//Actual freq ID
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 120, 315, 140);
	ILI9341_Draw_String(10, 123, WHITE, DARKGREY, "Frequency ID:", 2);

	//Freq value
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 145, 315, 165);
	ILI9341_Draw_String(10, 148, WHITE, DARKGREY, "Frequency:", 2);
	ILI9341_Draw_String(120, 148, WHITE, DARKGREY, ".", 2);
	ILI9341_Draw_String(158, 148, WHITE, DARKGREY, "MHz", 2);


	//channel name
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 170, 315, 190);
	ILI9341_Draw_String(10, 173, WHITE, DARKGREY, "Channel:", 2);

	//Cancel Button
	ILI9341_Draw_Filled_Rectangle(ORANGE, 5, 195, 315, 235);
	ILI9341_Draw_String(130, 207, WHITE, ORANGE, "CANCEL", 2);

}

void Display_scanning_screen_data(dab_digrad_status_t _digrad_status, dab_management_t _dab_management)
{
	//Scanning status bar
	ILI9341_Draw_Filled_Rectangle(GREEN, 10, 50, 10 + (300 * (_digrad_status.tune_index + 1)) / _dab_management.freq_cnt, 60);
	ILI9341_Draw_Filled_Rectangle(WHITE, 10 + (300 * (_digrad_status.tune_index + 1)) / _dab_management.freq_cnt, 50, 310, 60);

	//Found ensembles background
	ILI9341_Draw_String(145, 73, WHITE, DARKGREY, itoa(_dab_management.total_ensembles, itoa_buffer, 10), 2);

	//Found stations background
	ILI9341_Draw_String(135, 98, WHITE, DARKGREY, itoa(_dab_management.total_services, itoa_buffer, 10), 2);

	//Actual freq ID
	ILI9341_Draw_String(118, 123, WHITE, DARKGREY, itoa(_digrad_status.tune_index + 1, itoa_buffer, 10), 2);
	ILI9341_Draw_String(137, 123, WHITE, DARKGREY, "/", 2);
	ILI9341_Draw_String(147, 123, WHITE, DARKGREY, itoa(_dab_management.freq_cnt, itoa_buffer, 10), 2);

	//Freq value
	ILI9341_Draw_String(95, 148, WHITE, DARKGREY, itoa(_digrad_status.tune_freq / 1000, itoa_buffer, 10), 2);
	ILI9341_Draw_String(129, 148, WHITE, DARKGREY, itoa(_digrad_status.tune_freq % 1000, itoa_buffer, 10), 2);

	//channel name
	ILI9341_Draw_String(80, 173, WHITE, DARKGREY, "       ", 2);
	ILI9341_Draw_String(80, 173, WHITE, DARKGREY, dab_channels_names[_digrad_status.tune_index], 2);


}

void Display_scanning_screen_complete()
{
	//SCANNING DAB STRING background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 20, 315, 40);
	ILI9341_Draw_String(80, 23, WHITE, DARKGREY, "Scanning complete!     ", 2);

	//Cancel Button switches to OK button
	ILI9341_Draw_Filled_Rectangle(ORANGE, 5, 195, 315, 235);
	ILI9341_Draw_String(130, 207, WHITE, ORANGE, "  OK  ", 2);

}

void Display_dab_digrad_status_background()
{
	Display_clear_screen();

	ILI9341_Draw_String(5, 1, WHITE, BLACK, "DAB RECEIVE STATUS", 2); //Naglowek

	//Valid Flag Background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 20, 105, 40);
	//Acquire Flag Background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 110, 20, 210, 40);
	//FIC Error Flag Background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 215, 20, 315, 40);
	//RSSI Background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 45, 315, 65);
	ILI9341_Draw_String(10, 48, WHITE, DARKGREY, "RSSI", 2);
	ILI9341_Draw_String(90, 48, WHITE, DARKGREY, "dBuV", 2);
	//SNR Background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 70, 315, 90);
	ILI9341_Draw_String(10, 73, WHITE, DARKGREY, "SNR", 2);
	ILI9341_Draw_String(105, 73, WHITE, DARKGREY, "dB", 2);
	//CNR Background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 95, 315, 115);
	ILI9341_Draw_String(10, 98, WHITE, DARKGREY, "CNR", 2);
	ILI9341_Draw_String(105, 98, WHITE, DARKGREY, "dB", 2);
	//FIC Quality Background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 120, 315, 140);
	ILI9341_Draw_String(10, 123, WHITE, DARKGREY, "QUALITY", 2);
	ILI9341_Draw_String(110, 123, WHITE, DARKGREY, "%", 2);
	//Frequency info background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 145, 315, 165);
	ILI9341_Draw_String(10, 148, WHITE, DARKGREY, "FREQUENCY", 2);
	ILI9341_Draw_String(113, 148, WHITE, DARKGREY, ".", 2);
	ILI9341_Draw_String(148, 148, WHITE, DARKGREY, "MHz", 2);
	ILI9341_Draw_String(183, 148, WHITE, DARKGREY, "FREQ", 2);
	ILI9341_Draw_String(219, 148, WHITE, DARKGREY, "INDEX", 2);
	ILI9341_Draw_String(283, 148, WHITE, DARKGREY, "/", 2);
	ILI9341_Draw_String(293, 148, WHITE, DARKGREY, "40", 2);
	//Other info background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 170, 315, 190);
	ILI9341_Draw_String(10, 173, WHITE, DARKGREY, "TRIM CAP", 2);
	ILI9341_Draw_String(125, 173, WHITE, DARKGREY, "pF", 2);
	ILI9341_Draw_String(180, 173, WHITE, DARKGREY, "BER", 2);
	//buttons to other menus
	ILI9341_Draw_Filled_Rectangle(ORANGE, 5, 195, 157, 235);
	ILI9341_Draw_Filled_Rectangle(ORANGE, 163, 195, 315, 235);
	ILI9341_Draw_String(35, 207, WHITE, ORANGE, "SERVICE LIST", 2);
	ILI9341_Draw_String(205, 207, WHITE, ORANGE, "SETTINGS", 2);
}

void Display_dab_digrad_status_data(dab_digrad_status_t digrad_status)
{
	uint32_t color;
	uint32_t cap_val_ff = digrad_status.read_ant_cap * 250;
	uint8_t cap_val_pf_int = cap_val_ff / 1000;
	uint16_t cap_val_pf_frac = cap_val_ff % 1000;

	if(!display_freeze)
	{

		//Valid
		if(digrad_status.valid)
		{
			ILI9341_Draw_String(35, 23, GREEN, DARKGREY, "VALID", 2);
		}
		else ILI9341_Draw_String(35, 23, RED, DARKGREY, "VALID", 2);
		//ACQ
		if(digrad_status.acq)
		{
			ILI9341_Draw_String(133, 23, GREEN, DARKGREY, "ACQUIRE", 2);
		}
		else ILI9341_Draw_String(133, 23, RED, DARKGREY, "ACQUIRE", 2);
		//FIC_ERROR
		if(digrad_status.fic_err)
		{
			ILI9341_Draw_String(228, 23, RED, DARKGREY, "FIC ERROR", 2);
		}
		else ILI9341_Draw_String(228, 23, GREEN, DARKGREY, "FIC ERROR", 2);


		//RSSI
		if(digrad_status.rssi <= 13) 								color = RED;
		if((digrad_status.rssi <= 26) && (digrad_status.rssi > 13)) color = ORANGE;
		if((digrad_status.rssi <= 39) && (digrad_status.rssi > 26)) color = YELLOW;
		if((digrad_status.rssi <= 52) && (digrad_status.rssi > 39)) color = GREENYELLOW;
		if(digrad_status.rssi > 52) 								color = GREEN;
		ILI9341_Draw_String(50, 48, WHITE, DARKGREY, "   ", 2);
		ILI9341_Draw_String(50, 48, WHITE, DARKGREY, itoa(digrad_status.rssi, itoa_buffer, 10), 2);
		ILI9341_Draw_Filled_Rectangle(color, 130, 50, 130 + (digrad_status.rssi * 180)/64, 60);
		ILI9341_Draw_Filled_Rectangle(WHITE, 130 + (digrad_status.rssi * 180)/64, 50, 310, 60);

		//SNR
		if(digrad_status.snr <= 4) 									color = RED;
		if((digrad_status.snr <= 8) && (digrad_status.snr > 4)) 	color = ORANGE;
		if((digrad_status.snr <= 12) && (digrad_status.snr > 8)) 	color = YELLOW;
		if((digrad_status.snr <= 16) && (digrad_status.snr > 12)) 	color = GREENYELLOW;
		if(digrad_status.snr > 16) 									color = GREEN;
		ILI9341_Draw_String(50, 73, WHITE, DARKGREY, "   ", 2);
		ILI9341_Draw_String(50, 73, WHITE, DARKGREY, itoa(digrad_status.snr, itoa_buffer, 10), 2);
		ILI9341_Draw_Filled_Rectangle(color, 130, 75, 130 + (digrad_status.snr * 180)/20, 85);
		ILI9341_Draw_Filled_Rectangle(WHITE, 130 + (digrad_status.snr * 180)/20, 75, 310, 85);

		//CNR
		if(digrad_status.cnr <= 11) 								color = RED;
		if((digrad_status.cnr <= 22) && (digrad_status.cnr > 11)) 	color = ORANGE;
		if((digrad_status.cnr <= 33) && (digrad_status.cnr > 22)) 	color = YELLOW;
		if((digrad_status.cnr <= 44) && (digrad_status.cnr > 33)) 	color = GREENYELLOW;
		if(digrad_status.cnr > 44) 									color = GREEN;
		ILI9341_Draw_String(50, 98, WHITE, DARKGREY, "   ", 2);
		ILI9341_Draw_String(50, 98, WHITE, DARKGREY, itoa(digrad_status.cnr, itoa_buffer, 10), 2);
		ILI9341_Draw_Filled_Rectangle(color, 130, 100, 130 + (digrad_status.cnr * 180)/54, 110);
		ILI9341_Draw_Filled_Rectangle(WHITE, 130 + (digrad_status.cnr * 180)/54, 100, 310, 110);

		//FIC QUALITY
		if(digrad_status.fic_quality <= 20) 										color = RED;
		if((digrad_status.fic_quality <= 40) && (digrad_status.fic_quality > 20)) 	color = ORANGE;
		if((digrad_status.fic_quality <= 60) && (digrad_status.fic_quality > 40)) 	color = YELLOW;
		if((digrad_status.fic_quality <= 80) && (digrad_status.fic_quality > 60)) 	color = GREENYELLOW;
		if(digrad_status.fic_quality > 80) 											color = GREEN;
		ILI9341_Draw_String(75, 123, WHITE, DARKGREY, "   ", 2);
		ILI9341_Draw_String(75, 123, WHITE, DARKGREY, itoa(digrad_status.fic_quality, itoa_buffer, 10), 2);
		ILI9341_Draw_Filled_Rectangle(color, 130, 125, 130 + (digrad_status.fic_quality * 180)/100, 135);
		ILI9341_Draw_Filled_Rectangle(WHITE, 130 + (digrad_status.fic_quality * 180)/100, 125, 310, 135);

		//TUNE FREQ
		ILI9341_Draw_String(91, 148, WHITE, DARKGREY, itoa(digrad_status.tune_freq / 1000, itoa_buffer, 10), 2);
		ILI9341_Draw_String(120, 148, WHITE, DARKGREY, itoa(digrad_status.tune_freq % 1000, itoa_buffer, 10), 2);

		//TUNE INDEX
		if(digrad_status.tune_index < 10)
		{
			ILI9341_Draw_String(264, 148, WHITE, DARKGREY, "0", 2);
			ILI9341_Draw_String(273, 148, WHITE, DARKGREY, itoa(digrad_status.tune_index, itoa_buffer, 10), 2);
		}
		else ILI9341_Draw_String(264, 148, WHITE, DARKGREY, itoa(digrad_status.tune_index, itoa_buffer, 10), 2);

		//ANT CAP TRIM
		if(cap_val_pf_int < 10)
		{
			ILI9341_Draw_String(79, 173, WHITE, DARKGREY, " ", 2);
			ILI9341_Draw_String(87, 173, WHITE, DARKGREY, itoa(cap_val_pf_int, itoa_buffer, 10), 2);
		}
		if(cap_val_pf_int >= 10)
		{
			ILI9341_Draw_String(79, 173, WHITE, DARKGREY, itoa(cap_val_pf_int, itoa_buffer, 10), 2);
		}
		ILI9341_Draw_String(95, 173, WHITE, DARKGREY, ".", 2);
		switch(cap_val_pf_frac)
		{
		case 250:
			ILI9341_Draw_String(103, 173, WHITE, DARKGREY, "25", 2);
			break;
		case 500:
			ILI9341_Draw_String(103, 173, WHITE, DARKGREY, "50", 2);
			break;
		case 750:
			ILI9341_Draw_String(103, 173, WHITE, DARKGREY, "75", 2);
			break;
		default:
			ILI9341_Draw_String(103, 173, WHITE, DARKGREY, "00", 2);
			break;
		}

		//BER
		ILI9341_Draw_String(215, 173, WHITE, DARKGREY, itoa((digrad_status.fic_err_cnt / digrad_status.fic_bit_cnt), itoa_buffer, 10), 2);
	}
}

void Display_settings_screen_background()
{
	Display_clear_screen();

	ILI9341_Draw_String(5, 1, WHITE, BLACK, "SYSTEM SETTINGS", 2); //Naglowek

	//actual vol & backlight background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 20, 315, 55);
	ILI9341_Draw_String(55, 22, WHITE, DARKGREY, "VOLUME", 2);
	ILI9341_Draw_String(206, 22, WHITE, DARKGREY, "BACKLIGHT", 2);

	//Volume UP/DOWN buttons
	ILI9341_Draw_Filled_Rectangle(ORANGE, 5, 60, 157, 100);
	ILI9341_Draw_Filled_Rectangle(ORANGE, 163, 60, 315, 100);
	ILI9341_Draw_String(54, 72, WHITE, ORANGE, "VOLUME -", 2);
	ILI9341_Draw_String(211, 72, WHITE, ORANGE, "VOLUME +", 2);

	//BACKLIGHT +/- buttons
	ILI9341_Draw_Filled_Rectangle(ORANGE, 5, 105, 157, 145);
	ILI9341_Draw_Filled_Rectangle(ORANGE, 163, 105, 315, 145);
	ILI9341_Draw_String(35, 117, WHITE, ORANGE, "BACKLIGHT -", 2);
	ILI9341_Draw_String(193, 117, WHITE, ORANGE, "BACKLIGHT +", 2);

	//scan & touch calibrate buttons
	ILI9341_Draw_Filled_Rectangle(ORANGE, 5, 150, 157, 190);
	ILI9341_Draw_Filled_Rectangle(ORANGE, 163, 150, 315, 190);
	ILI9341_Draw_String(32, 162, WHITE, ORANGE, "SCAN DAB BAND", 2);
	ILI9341_Draw_String(168, 162, WHITE, ORANGE, "TOUCH CALIBRATION", 2);

	//buttons to other menus
	ILI9341_Draw_Filled_Rectangle(ORANGE, 5, 195, 157, 235);
	ILI9341_Draw_Filled_Rectangle(ORANGE, 163, 195, 315, 235);
	ILI9341_Draw_String(37, 207, WHITE, ORANGE, "SIGNAL INFO", 2);
	ILI9341_Draw_String(195, 207, WHITE, ORANGE, "MAIN SCREEN", 2);

}

void Display_settings_screen_data(dab_management_t _dab_management)
{
	//volume bar
	ILI9341_Draw_Filled_Rectangle(WHITE, 12 + (_dab_management.audio_volume * 140) / 63, 40, 152, 50);
	ILI9341_Draw_Filled_Rectangle(GREEN, 12, 40, 12 + (_dab_management.audio_volume * 140) / 63, 50);

	//backlight bar
	ILI9341_Draw_Filled_Rectangle(GREEN, 168, 40, 308, 50);
}

void Display_time(time_t time_val)
{
	//hour and minute
	if(time_val.year && time_val.month)
	{
		if(time_val.hour < 10)
		{
			ILI9341_Draw_String(271, 1, WHITE, BLACK, "0", 2);
			ILI9341_Draw_String(280, 1, WHITE, BLACK, itoa(time_val.hour, itoa_buffer, 10), 2);
		}
		else
		{
			ILI9341_Draw_String(271, 1, WHITE, BLACK, itoa(time_val.hour, itoa_buffer, 10), 2);
		}

		ILI9341_Draw_String(287, 1, WHITE, BLACK, ":", 2);

		if(time_val.minute < 10)
		{
			ILI9341_Draw_String(295, 1, WHITE, BLACK, "0", 2);
			ILI9341_Draw_String(304, 1, WHITE, BLACK, itoa(time_val.minute, itoa_buffer, 10), 2);
		}
		else
		{
			ILI9341_Draw_String(295, 1, WHITE, BLACK, itoa(time_val.minute, itoa_buffer, 10), 2);
		}
	}
	//date
	//todo
}

void Display_show_station(dab_service_t _services_list[], uint8_t _actual_station, uint8_t _total_services)
{
	display_freeze = 1;

	ILI9341_Draw_Filled_Rectangle(BLACK, 50, 90, 270, 150);
	ILI9341_Draw_String(55, 95, WHITE, BLACK, "Number:", 2);
	ILI9341_Draw_String(120, 95, WHITE, BLACK, itoa(_actual_station + 1, itoa_buffer, 10), 2);
	ILI9341_Draw_String(140, 95, WHITE, BLACK, "/", 2);
	ILI9341_Draw_String(148, 95, WHITE, BLACK, itoa(_total_services, itoa_buffer, 10), 2);
	ILI9341_Draw_String(55, 110, WHITE, BLACK, "Name:", 2);
	ILI9341_Draw_String(100, 110, WHITE, BLACK, _services_list[_actual_station].name, 2);
}

void Display_hide_station()
{
	display_freeze = 0;

	ILI9341_Draw_Filled_Rectangle(BLACK, 50, 90, 270, 150);

	//wyswietlanie ponownie tylko tego, co wyskakujace okno zaslonilo

	//SNR Background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 70, 315, 90);
	ILI9341_Draw_String(10, 73, WHITE, DARKGREY, "SNR", 2);
	ILI9341_Draw_String(105, 73, WHITE, DARKGREY, "dB", 2);
	//CNR Background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 95, 315, 115);
	ILI9341_Draw_String(10, 98, WHITE, DARKGREY, "CNR", 2);
	ILI9341_Draw_String(105, 98, WHITE, DARKGREY, "dB", 2);
	//FIC Quality Background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 120, 315, 140);
	ILI9341_Draw_String(10, 123, WHITE, DARKGREY, "QUALITY", 2);
	ILI9341_Draw_String(110, 123, WHITE, DARKGREY, "%", 2);
	//Frequency info background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 5, 145, 315, 165);
	ILI9341_Draw_String(10, 148, WHITE, DARKGREY, "FREQUENCY", 2);
	ILI9341_Draw_String(113, 148, WHITE, DARKGREY, ".", 2);
	ILI9341_Draw_String(148, 148, WHITE, DARKGREY, "MHz", 2);
	ILI9341_Draw_String(183, 148, WHITE, DARKGREY, "FREQ", 2);
	ILI9341_Draw_String(219, 148, WHITE, DARKGREY, "INDEX", 2);
	ILI9341_Draw_String(283, 148, WHITE, DARKGREY, "/", 2);
	ILI9341_Draw_String(293, 148, WHITE, DARKGREY, "40", 2);

}

void Display_show_empty_station()
{
	display_freeze = 1;

	ILI9341_Draw_Filled_Rectangle(BLACK, 50, 90, 270, 150);
	ILI9341_Draw_String(55, 95, WHITE, BLACK, "Service list is empty!", 2);
}


