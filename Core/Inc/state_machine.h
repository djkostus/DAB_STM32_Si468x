/*
 * state_machine.h
 *
 *  Created on: Sep 15, 2022
 *      Author: Cineq
 */

#ifndef INC_STATE_MACHINE_H_
#define INC_STATE_MACHINE_H_

#include "stm32f4xx.h"

enum system_state{
main_screen,
services_list_screen,
scanning,
signal_info,
settings
};

enum play_state{
	not_playing,
	playing
};

void state_machine();

uint8_t get_scan_cancel_flag();


#endif /* INC_STATE_MACHINE_H_ */
