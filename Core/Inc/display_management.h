/*
 * display_management.h
 *
 *  Created on: Aug 22, 2022
 *      Author: mgostek
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "main.h"
#include "Si468x.h"
#include "ILI9341_Driver.h"
#include "stdlib.h"

void Display_clear_screen();

void Display_main_screen();

void Display_dab_digrad_status_background();
void Display_dab_digrad_status_data(dab_digrad_status_t digrad_status);

void Display_init_screen();

#endif /* INC_DISPLAY_H_ */
