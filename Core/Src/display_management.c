/*
 * display.c
 *
 *  Created on: Aug 22, 2022
 *      Author: mgostek
 */

#include "display_management.h"
#include "ILI9341_Driver.h"
#include "stdlib.h"

char char_buffer[32];
int test_cnt = 0;

void DisplayMainScreen()
{
	/* EKRAN GŁÓWNY NASZEGO URZĄDZENIA Refresh the screen to black background */
	ILI9341_Fill_Screen(BLACK);
	ILI9341_Draw_Empty_Rectangle(YELLOW, 5, 25, 315, 235); // żółta obwoluta

    /*Nagłówek */
	ILI9341_Draw_String(65,5,WHITE,BLACK,"DAB+ Radio Receiver",2); //Naglowek radia
	ILI9341_Draw_Empty_Rectangle(YELLOW, 15, 35, 305, 60); //Nazwa radia
	ILI9341_Draw_Empty_Rectangle(YELLOW, 15, 70, 105, 95); //Głośnośc
	ILI9341_Draw_Empty_Rectangle(YELLOW, 115, 70, 185, 95); //Napis bufor
	ILI9341_Draw_Empty_Rectangle(YELLOW, 185, 70, 305, 95); //Pasek bufora


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
	ILI9341_Draw_String(245,180,WHITE,RED,"NEXT",2);//Wyswietl stringa
	ILI9341_Draw_String(232,195,WHITE,RED,"STATION",2);//Wyswietl stringa

	ILI9341_Draw_String(120, 75, WHITE, BLACK, "Buffer: " , 2);

	CS_OFF;
}

void DisplayTest()
{
	test_cnt++;
	//itoa(100, temp_char, 10);
	ILI9341_Draw_String(20, 40, WHITE, BLACK, itoa(test_cnt, char_buffer, 10), 2);
}
