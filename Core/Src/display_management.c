/*
 * display.c
 *
 *  Created on: Aug 22, 2022
 *      Author: mgostek
 */

#include "display_management.h"
#include <string.h>

char char_buffer[32];

uint8_t display_freeze = 0;

void Display_clear_screen()
{
	ILI9341_Fill_Screen(BLACK);
	ILI9341_Draw_Line(YELLOW, 0, 17, 320, 17);
}

void Display_main_screen_background()
{
	Display_clear_screen();

	ILI9341_Draw_Empty_Rectangle(YELLOW, 5, 25, 315, 235); // żółta obwoluta
	ILI9341_Draw_String(65, 5, WHITE, BLACK, "DAB+ Radio Receiver", 2); //Naglowek radia

    /*Nagłówek */
	ILI9341_Draw_String(65,5,WHITE,BLACK,"DAB+ Radio Receiver",2); //Naglowek radia
	ILI9341_Draw_Empty_Rectangle(YELLOW, 15, 35, 305, 60); //Nazwa radia
	//ILI9341_Draw_Empty_Rectangle(YELLOW, 15, 70, 105, 95); //Głośnośc
	ILI9341_Draw_Empty_Rectangle(YELLOW, 15, 70, 305, 95); //Napis bufor
	//ILI9341_Draw_Empty_Rectangle(YELLOW, 185, 70, 305, 95); //Pasek bufora


	ILI9341_Draw_Empty_Rectangle(YELLOW, 15, 110, 105, 160); //volume-
	ILI9341_Draw_Filled_Rectangle(RED, 16, 111, 104, 159); //filled red volume-
	ILI9341_Draw_String(35,127,WHITE,RED,"VOLUME-",2);//Wyswietl stringa

	ILI9341_Draw_Empty_Rectangle(YELLOW, 115, 110, 205, 160); //mute
	ILI9341_Draw_Filled_Rectangle(RED, 116, 111, 204, 159); //filled red mute
	ILI9341_Draw_String(144,127,WHITE,RED,"MUTE",2);//Wyswietl stringa

	ILI9341_Draw_Empty_Rectangle(YELLOW, 215, 110, 305, 160); //volume+
	ILI9341_Draw_Filled_Rectangle(RED, 216, 111, 304, 159);
	ILI9341_Draw_String(234,127,WHITE,RED,"VOLUME+",2);

	ILI9341_Draw_Empty_Rectangle(YELLOW, 15, 170, 105, 220); //MENU
	ILI9341_Draw_Filled_Rectangle(RED, 16, 171, 104, 219);
	ILI9341_Draw_String(45,187,WHITE,RED,"MENU",2);//Wyswietl stringa

	ILI9341_Draw_Empty_Rectangle(YELLOW, 115, 170, 205, 220); //Previous Station
	ILI9341_Draw_Filled_Rectangle(RED, 116, 171, 204, 219);
	ILI9341_Draw_String(129,180,WHITE,RED,"PREVIOUS",2);//Wyswietl stringa
	ILI9341_Draw_String(132,195,WHITE,RED,"STATION",2);//Wyswietl stringa

	ILI9341_Draw_Empty_Rectangle(YELLOW, 215, 170, 305, 220); //Next station
	ILI9341_Draw_Filled_Rectangle(RED, 216, 171, 304, 219);
	ILI9341_Draw_String(245,180,WHITE,RED,"NEXT", 2);//Wyswietl stringa
	ILI9341_Draw_String(232,195,WHITE,RED,"STATION", 2);//Wyswietl stringa

	CS_OFF;
}


void Display_dab_digrad_status_background()
{
//	Display_clear_screen();

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
	ILI9341_Draw_String(60, 207, WHITE, ORANGE, "MENU L", 2);
	ILI9341_Draw_String(217, 207, WHITE, ORANGE, "MENU R", 2);
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
		ILI9341_Draw_String(50, 48, WHITE, DARKGREY, itoa(digrad_status.rssi, char_buffer, 10), 2);
		ILI9341_Draw_Filled_Rectangle(color, 130, 50, 130 + (digrad_status.rssi * 180)/64, 60);
		ILI9341_Draw_Filled_Rectangle(WHITE, 130 + (digrad_status.rssi * 180)/64, 50, 310, 60);

		//SNR
		if(digrad_status.snr <= 4) 									color = RED;
		if((digrad_status.snr <= 8) && (digrad_status.snr > 4)) 	color = ORANGE;
		if((digrad_status.snr <= 12) && (digrad_status.snr > 8)) 	color = YELLOW;
		if((digrad_status.snr <= 16) && (digrad_status.snr > 12)) 	color = GREENYELLOW;
		if(digrad_status.snr > 16) 									color = GREEN;
		ILI9341_Draw_String(50, 73, WHITE, DARKGREY, "   ", 2);
		ILI9341_Draw_String(50, 73, WHITE, DARKGREY, itoa(digrad_status.snr, char_buffer, 10), 2);
		ILI9341_Draw_Filled_Rectangle(color, 130, 75, 130 + (digrad_status.snr * 180)/20, 85);
		ILI9341_Draw_Filled_Rectangle(WHITE, 130 + (digrad_status.snr * 180)/20, 75, 310, 85);

		//CNR
		if(digrad_status.cnr <= 11) 								color = RED;
		if((digrad_status.cnr <= 22) && (digrad_status.cnr > 11)) 	color = ORANGE;
		if((digrad_status.cnr <= 33) && (digrad_status.cnr > 22)) 	color = YELLOW;
		if((digrad_status.cnr <= 44) && (digrad_status.cnr > 33)) 	color = GREENYELLOW;
		if(digrad_status.cnr > 44) 									color = GREEN;
		ILI9341_Draw_String(50, 98, WHITE, DARKGREY, "   ", 2);
		ILI9341_Draw_String(50, 98, WHITE, DARKGREY, itoa(digrad_status.cnr, char_buffer, 10), 2);
		ILI9341_Draw_Filled_Rectangle(color, 130, 100, 130 + (digrad_status.cnr * 180)/54, 110);
		ILI9341_Draw_Filled_Rectangle(WHITE, 130 + (digrad_status.cnr * 180)/54, 100, 310, 110);

		//FIC QUALITY
		if(digrad_status.fic_quality <= 20) 										color = RED;
		if((digrad_status.fic_quality <= 40) && (digrad_status.fic_quality > 20)) 	color = ORANGE;
		if((digrad_status.fic_quality <= 60) && (digrad_status.fic_quality > 40)) 	color = YELLOW;
		if((digrad_status.fic_quality <= 80) && (digrad_status.fic_quality > 60)) 	color = GREENYELLOW;
		if(digrad_status.fic_quality > 80) 											color = GREEN;
		ILI9341_Draw_String(75, 123, WHITE, DARKGREY, "   ", 2);
		ILI9341_Draw_String(75, 123, WHITE, DARKGREY, itoa(digrad_status.fic_quality, char_buffer, 10), 2);
		ILI9341_Draw_Filled_Rectangle(color, 130, 125, 130 + (digrad_status.fic_quality * 180)/100, 135);
		ILI9341_Draw_Filled_Rectangle(WHITE, 130 + (digrad_status.fic_quality * 180)/100, 125, 310, 135);

		//TUNE FREQ
		ILI9341_Draw_String(91, 148, WHITE, DARKGREY, itoa(digrad_status.tune_freq / 1000, char_buffer, 10), 2);
		ILI9341_Draw_String(120, 148, WHITE, DARKGREY, itoa(digrad_status.tune_freq % 1000, char_buffer, 10), 2);

		//TUNE INDEX
		if(digrad_status.tune_index < 10)
		{
			ILI9341_Draw_String(264, 148, WHITE, DARKGREY, "0", 2);
			ILI9341_Draw_String(273, 148, WHITE, DARKGREY, itoa(digrad_status.tune_index, char_buffer, 10), 2);
		}
		else ILI9341_Draw_String(264, 148, WHITE, DARKGREY, itoa(digrad_status.tune_index, char_buffer, 10), 2);

		//ANT CAP TRIM
		if(cap_val_pf_int < 10)
		{
			ILI9341_Draw_String(79, 173, WHITE, DARKGREY, " ", 2);
			ILI9341_Draw_String(87, 173, WHITE, DARKGREY, itoa(cap_val_pf_int, char_buffer, 10), 2);
		}
		if(cap_val_pf_int >= 10)
		{
			ILI9341_Draw_String(79, 173, WHITE, DARKGREY, itoa(cap_val_pf_int, char_buffer, 10), 2);
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
		ILI9341_Draw_String(215, 173, WHITE, DARKGREY, itoa((digrad_status.fic_err_cnt / digrad_status.fic_bit_cnt), char_buffer, 10), 2);
	}
}

void Display_time(time_t time_val)
{
	//hour and minute
	if(time_val.hour < 10)
	{
		ILI9341_Draw_String(271, 1, WHITE, BLACK, "0", 2);
		ILI9341_Draw_String(280, 1, WHITE, BLACK, itoa(time_val.hour, char_buffer, 10), 2);
	}
	else
	{
		ILI9341_Draw_String(271, 1, WHITE, BLACK, itoa(time_val.hour, char_buffer, 10), 2);
	}

	ILI9341_Draw_String(287, 1, WHITE, BLACK, ":", 2);

	if(time_val.minute < 10)
	{
		ILI9341_Draw_String(295, 1, WHITE, BLACK, "0", 2);
		ILI9341_Draw_String(304, 1, WHITE, BLACK, itoa(time_val.minute, char_buffer, 10), 2);
	}
	else
	{
		ILI9341_Draw_String(295, 1, WHITE, BLACK, itoa(time_val.minute, char_buffer, 10), 2);
	}
	//date
	//todo
}

void Display_init_screen()
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

void Display_show_next_station(dab_service_t _services_list[], uint8_t _actual_station, uint8_t _total_services)
{
	display_freeze = 1;

	ILI9341_Draw_Filled_Rectangle(BLACK, 50, 90, 270, 150);
	ILI9341_Draw_String(55, 95, WHITE, BLACK, "Number:", 2);
	ILI9341_Draw_String(120, 95, WHITE, BLACK, itoa(_actual_station + 1, char_buffer, 10), 2);
	ILI9341_Draw_String(140, 95, WHITE, BLACK, "/", 2);
	ILI9341_Draw_String(148, 95, WHITE, BLACK, itoa(_total_services, char_buffer, 10), 2);
	ILI9341_Draw_String(55, 110, WHITE, BLACK, "Name:", 2);
	ILI9341_Draw_String(100, 110, WHITE, BLACK, _services_list[_actual_station].name, 2);
}

void Display_hide_next_station()
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
