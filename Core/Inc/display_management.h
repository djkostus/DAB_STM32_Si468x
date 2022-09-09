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

void DisplayMainScreen();

void DisplayStatusReg();

void DisplayOK();
void DisplayError();

extern void ClearOK();

void DisplayFirmwareDownloadStatus(uint32_t byte_cnt);
void DisplayFirmwareTotalSize(uint32_t byte_cnt);

void DisplayState(char* string);

void DisplayCmdError();

void DisplayBooting();

void DisplayDabStatus(sig_metrics_t sig_metrics_val);

#endif /* INC_DISPLAY_H_ */
