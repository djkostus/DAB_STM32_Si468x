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

void state_machine()
{
	switch(state)
	{
	case main_screen:
		break;
	case stations_list:
		break;
	case signal_info:
		break;
	case scanning:
		break;
	case settings:
		break;
	default:
		break;
	}
}


