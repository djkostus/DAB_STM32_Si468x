/*
 * leds.c
 *
 *  Created on: Aug 22, 2022
 *      Author: mgostek
 */

#include "leds.h"
#include "main.h"

uint8_t led_number = 1;


void Leds_Control_Blink()
{
	led_number++;
		if(led_number == 5)
		{
			led_number = 1;
		}

	switch(led_number)
	{
	case 1:
		HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, 1); //ziel
		HAL_GPIO_WritePin(LED_ORANGE_GPIO_Port, LED_ORANGE_Pin, 0); //pom
		HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, 0); //czer
		HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, 0); //nieb
		break;
	case 2:
		HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, 0); //ziel
		HAL_GPIO_WritePin(LED_ORANGE_GPIO_Port, LED_ORANGE_Pin, 1); //pom
		HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, 0); //czer
		HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, 0); //nieb
		break;
	case 3:
		HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, 0); //ziel
		HAL_GPIO_WritePin(LED_ORANGE_GPIO_Port, LED_ORANGE_Pin, 0); //pom
		HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, 1); //czer
		HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, 0); //nieb
		break;
	case 4:
		HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, 0); //ziel
		HAL_GPIO_WritePin(LED_ORANGE_GPIO_Port, LED_ORANGE_Pin, 0); //pom
		HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, 0); //czer
		HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, 1); //nieb
		break;
	default:

		break;
	}
}

