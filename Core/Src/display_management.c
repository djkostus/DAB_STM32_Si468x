/*
 * display.c
 *
 *  Created on: Aug 22, 2022
 *      Author: mgostek
 */

#include "display_management.h"
#include "ILI9341_Driver.h"
#include "stdlib.h"
#include "Si468x.h"

char char_buffer[32];

void DisplayMainScreen()
{
	/* EKRAN GŁÓWNY NASZEGO URZĄDZENIA Refresh the screen to black background */
	ILI9341_Fill_Screen(BLACK);
	ILI9341_Draw_Empty_Rectangle(YELLOW, 5, 25, 315, 235); // żółta obwoluta

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

void DisplayStatusReg()
{
	ILI9341_Draw_String(20, 40, WHITE, BLACK, "   ", 2);
	ILI9341_Draw_String(60, 40, WHITE, BLACK, "   ", 2);
	ILI9341_Draw_String(100, 40, WHITE, BLACK, "   ", 2);
	ILI9341_Draw_String(140, 40, WHITE, BLACK, "   ",  2);
	ILI9341_Draw_String(180, 40, WHITE, BLACK, "   ", 2);
	ILI9341_Draw_String(220, 40, WHITE, BLACK, "   ", 2);


	ILI9341_Draw_String(20, 40, WHITE, BLACK, itoa(get_rx_buffer(0), char_buffer, 10), 2);
	ILI9341_Draw_String(60, 40, WHITE, BLACK, itoa(get_rx_buffer(1), char_buffer, 10), 2);
	ILI9341_Draw_String(100, 40, WHITE, BLACK, itoa(get_rx_buffer(2), char_buffer, 10), 2);
	ILI9341_Draw_String(140, 40, WHITE, BLACK, itoa(get_rx_buffer(3), char_buffer, 10), 2);
	ILI9341_Draw_String(180, 40, WHITE, BLACK, itoa(get_rx_buffer(4), char_buffer, 10), 2);
}

void DisplayOK()
{
	ILI9341_Draw_String(260, 40, WHITE, BLACK, "OK", 2);
}

void DisplayError()
{
	ILI9341_Draw_String(260, 40, WHITE, BLACK, "TO", 2);
}

void ClearOK()
{
	ILI9341_Draw_String(260, 40, WHITE, BLACK, "  ", 2);
}

void DisplayState(char* string)
{
	ILI9341_Draw_String(80, 75, WHITE, BLACK, string, 2);
	ILI9341_Draw_String(190, 75, WHITE, BLACK, "      ", 2);
	ILI9341_Draw_String(250, 75, WHITE, BLACK, "      ", 2);
}


void DisplayFirmwareDownloadStatus(uint32_t byte_cnt)
{
	ILI9341_Draw_String(190, 75, WHITE, BLACK, itoa(byte_cnt, char_buffer, 10), 2);
}

void DisplayFirmwareTotalSize(uint32_t byte_cnt)
{
	ILI9341_Draw_String(250, 75, WHITE, BLACK, itoa(byte_cnt, char_buffer, 10), 2);
}

void DisplayCmdError()
{
	ILI9341_Draw_String(290, 40, WHITE, BLACK, "E", 2);
}

void DisplayDabStatus(uint8_t rssi_val, uint8_t snr_val, uint16_t fib_er_val, uint8_t valid_val, uint8_t acq_val, uint8_t acq_int_val)
{
	ILI9341_Draw_String(20, 75, WHITE, BLACK, "RSSI:" , 2);
	ILI9341_Draw_String(63, 75, WHITE, BLACK, "   ", 2);
	ILI9341_Draw_String(63, 75, WHITE, BLACK, itoa(rssi_val, char_buffer, 10), 2);
	ILI9341_Draw_String(90, 75, WHITE, BLACK, "SNR:" , 2);
	ILI9341_Draw_String(123, 75, WHITE, BLACK, "   ", 2);
	ILI9341_Draw_String(123, 75, WHITE, BLACK, itoa(snr_val, char_buffer, 10), 2);
	ILI9341_Draw_String(160, 75, WHITE, BLACK, "FIB Er:", 2);
	ILI9341_Draw_String(220, 75, WHITE, BLACK, "        ", 2);
	ILI9341_Draw_String(220, 75, WHITE, BLACK, itoa(fib_er_val, char_buffer, 10), 2);
	ILI9341_Draw_String(20, 40, WHITE, BLACK, "Valid:", 2);
	ILI9341_Draw_String(73, 40, WHITE, BLACK, itoa(valid_val, char_buffer, 10), 2);
	ILI9341_Draw_String(90, 40, WHITE, BLACK, "ACQ:", 2);
	ILI9341_Draw_String(123, 40, WHITE, BLACK, itoa(acq_val, char_buffer, 10), 2);
	ILI9341_Draw_String(150, 40, WHITE, BLACK, "ACQ INT:", 2);
	ILI9341_Draw_String(220, 40, WHITE, BLACK, itoa(acq_int_val, char_buffer, 10), 2);
}
