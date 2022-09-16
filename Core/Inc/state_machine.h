/*
 * state_machine.h
 *
 *  Created on: Sep 15, 2022
 *      Author: Cineq
 */

#ifndef INC_STATE_MACHINE_H_
#define INC_STATE_MACHINE_H_

#include "stm32f4xx.h"

enum state{
main_screen,
stations_list,
scanning,
signal_info,
settings
};


void state_machine();




#endif /* INC_STATE_MACHINE_H_ */