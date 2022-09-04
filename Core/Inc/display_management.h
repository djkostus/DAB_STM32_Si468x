/*
 * display_management.h
 *
 *  Created on: Aug 22, 2022
 *      Author: mgostek
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "main.h"

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

void DisplayDabStatus(uint8_t rssi_val, uint8_t snr_val, uint16_t fib_er_val, uint8_t valid_val, uint8_t acq_val, uint8_t acq_int_val);

#endif /* INC_DISPLAY_H_ */
