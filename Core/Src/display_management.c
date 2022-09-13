/*
 * display.c
 *
 *  Created on: Aug 22, 2022
 *      Author: mgostek
 */

#include "display_management.h"

#define STRING_SPACING 14

char char_buffer[32];

void Display_clear_screen()
{
	ILI9341_Fill_Screen(BLACK);
//	ILI9341_Draw_Empty_Rectangle(YELLOW, 2, 15, 318, 238); // żółta obwoluta
	ILI9341_Draw_Line(YELLOW, 0, 17, 320, 17);
}

void Display_main_screen()
{
	/* EKRAN GŁÓWNY NASZEGO URZĄDZENIA Refresh the screen to black background */
	ILI9341_Fill_Screen(BLACK);
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
	ILI9341_Draw_String(83, 1, WHITE, BLACK, "DAB RECEIVE STATUS", 2); //Naglowek radia

	//Valid Flag Background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 20, 20, 100, 40);
	//Acquire Flag Background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 120, 20, 200, 40);
	//FIC Error Flag Background
	ILI9341_Draw_Filled_Rectangle(DARKGREY, 220, 20, 300, 40);

	ILI9341_Draw_String(5, 20 + 3*STRING_SPACING, WHITE, BLACK, "RSSI:" , 2);
	ILI9341_Draw_String(5, 20 + 4*STRING_SPACING, WHITE, BLACK, "SNR:" , 2);
	ILI9341_Draw_String(5, 20 + 5*STRING_SPACING, WHITE, BLACK, "FIC Quality:" , 2);
	ILI9341_Draw_String(5, 20 + 6*STRING_SPACING, WHITE, BLACK, "CNR:" , 2);
	ILI9341_Draw_String(5, 20 + 7*STRING_SPACING, WHITE, BLACK, "FIB Error Bits:", 2);
	ILI9341_Draw_String(5, 20 + 8*STRING_SPACING, WHITE, BLACK, "Frequency:", 2);
	ILI9341_Draw_String(5, 20 + 9*STRING_SPACING, WHITE, BLACK, "Freq Index:", 2);
	ILI9341_Draw_String(5, 20 + 10*STRING_SPACING, WHITE, BLACK, "FFT Offset:", 2);
	ILI9341_Draw_String(5, 20 + 11*STRING_SPACING, WHITE, BLACK, "ANT Cap Trim:", 2);
	ILI9341_Draw_String(5, 20 + 12*STRING_SPACING, WHITE, BLACK, "CU Level:", 2);
	ILI9341_Draw_String(5, 20 + 13*STRING_SPACING, WHITE, BLACK, "FIC Error Bits:", 2);
	ILI9341_Draw_String(5, 20 + 14*STRING_SPACING, WHITE, BLACK, "FIC Total Bits:", 2);
}

void Display_dab_digrad_status_data(dab_digrad_status_t digrad_status)
{
	//Valid
	if(digrad_status.valid)
	{
		ILI9341_Draw_String(40, 23, GREEN, DARKGREY, "VALID", 2);
	}
	else
	{
		ILI9341_Draw_String(40, 23, RED, DARKGREY, "VALID", 2);
	}
	//ACQ
	if(digrad_status.acq)
	{
		ILI9341_Draw_String(133, 23, GREEN, DARKGREY, "ACQUIRE", 2);
	}
	else
	{
		ILI9341_Draw_String(133, 23, RED, DARKGREY, "ACQUIRE", 2);
	}
	//FIC_ERROR
	if(digrad_status.fic_err)
	{
		ILI9341_Draw_String(225, 23, RED, DARKGREY, "FIC ERROR", 2);
	}
	else
	{
		ILI9341_Draw_String(225, 23, GREEN, DARKGREY, "FIC ERROR", 2);
	}

	//RSSI
	ILI9341_Draw_String(150, 20 + 3*STRING_SPACING, WHITE, BLACK, "   ", 2);
	ILI9341_Draw_String(150, 20 + 3*STRING_SPACING, WHITE, BLACK, itoa(digrad_status.rssi, char_buffer, 10), 2);
	//SNR
	ILI9341_Draw_String(150, 20 + 4*STRING_SPACING, WHITE, BLACK, "   ", 2);
	ILI9341_Draw_String(150, 20 + 4*STRING_SPACING, WHITE, BLACK, itoa(digrad_status.snr, char_buffer, 10), 2);
	//FIC QUALITY
	ILI9341_Draw_String(150, 20 + 5*STRING_SPACING, WHITE, BLACK, "   ", 2);
	ILI9341_Draw_String(150, 20 + 5*STRING_SPACING, WHITE, BLACK, itoa(digrad_status.fic_quality, char_buffer, 10), 2);
	//CNR
	ILI9341_Draw_String(150, 20 + 6*STRING_SPACING, WHITE, BLACK, "   ", 2);
	ILI9341_Draw_String(150, 20 + 6*STRING_SPACING, WHITE, BLACK, itoa(digrad_status.cnr, char_buffer, 10), 2);
	//FIB ERROR COUNT
	ILI9341_Draw_String(150, 20 + 7*STRING_SPACING, WHITE, BLACK, "     ", 2);
	ILI9341_Draw_String(150, 20 + 7*STRING_SPACING, WHITE, BLACK, itoa(digrad_status.fib_err_count, char_buffer, 10), 2);
	//TUNE FREQ
	ILI9341_Draw_String(150, 20 + 8*STRING_SPACING, WHITE, BLACK, itoa(digrad_status.tune_freq, char_buffer, 10), 2);
	//TUNE INDEX
	ILI9341_Draw_String(150, 20 + 9*STRING_SPACING, WHITE, BLACK, "  ", 2);
	ILI9341_Draw_String(150, 20 + 9*STRING_SPACING, WHITE, BLACK, itoa(digrad_status.tune_index, char_buffer, 10), 2);
	//FFT OFFSET
	ILI9341_Draw_String(150, 20 + 10*STRING_SPACING, WHITE, BLACK, "   ", 2);
	ILI9341_Draw_String(150, 20 + 10*STRING_SPACING, WHITE, BLACK, itoa(digrad_status.fft_offset, char_buffer, 10), 2);
	//ANT CAP TRIM
	ILI9341_Draw_String(150, 20 + 11*STRING_SPACING, WHITE, BLACK, "     ", 2);
	ILI9341_Draw_String(150, 20 + 11*STRING_SPACING, WHITE, BLACK, itoa(digrad_status.read_ant_cap, char_buffer, 10), 2);
	//CU LEVEL
	ILI9341_Draw_String(150, 20 + 12*STRING_SPACING, WHITE, BLACK, "     ", 2);
	ILI9341_Draw_String(150, 20 + 12*STRING_SPACING, WHITE, BLACK, itoa(digrad_status.cu_level, char_buffer, 10), 2);
	//FIC ERR CNT
	ILI9341_Draw_String(150, 20 + 13*STRING_SPACING, WHITE, BLACK, "          ", 2);
	ILI9341_Draw_String(150, 20 + 13*STRING_SPACING, WHITE, BLACK, itoa(digrad_status.fic_err_cnt, char_buffer, 10), 2);
	//FIC BIT CNT
	ILI9341_Draw_String(150, 20 + 14*STRING_SPACING, WHITE, BLACK, "          ", 2);
	ILI9341_Draw_String(150, 20 + 14*STRING_SPACING, WHITE, BLACK, itoa(digrad_status.fic_bit_cnt, char_buffer, 10), 2);
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
