/*
 * state_machine.c
 *
 *  Created on: Sep 15, 2022
 *      Author: Cineq
 */

#include "state_machine.h"
#include "debug_uart.h"
#include "display_management.h"

void state_machine()
{
	Display_clear_screen();
	Display_dab_digrad_status_background();

//	HAL_TIM_Base_Start_IT(&htim10);	//enable this timer = enable continuously show signal info
}


