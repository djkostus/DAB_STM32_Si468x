/*
 * Si468x_defs.h
 *
 *  Created on: Aug 22, 2022
 *      Author: mgostek
 */

#ifndef INC_SI468X_DEFS_H_
#define INC_SI468X_DEFS_H_

//HARDWARE
#define SI468X_RD_REPLY					0x00	//Returns the status byte and data for the last command sent to the device.
#define SI468X_POWER_UP					0x01	//Power-up the device and set system settings.

//BOOTLOADER
#define SI468X_HOST_LOAD				0x04	//Loads an image from the HOST over the command interface
#define SI468X_FLASH_LOAD				0x05	//Loads an image from external FLASH over secondary SPI bus
#define SI468X_LOAD_INIT				0x06	//Prepares the bootloader to receive a new image.
#define SI468X_BOOT						0x07	//Boots the image currently loaded in RAM.

//SHARED
#define SI468X_GET_PART_INFO 			0x08	//Reports basic information about the device.
#define SI468X_GET_SYS_STATE			0x09	//Reports system state information.
#define SI468X_GET_POWER_UP_ARGS		0x0A	//Reports basic information about the device such as arguments used during POWER_UP.
#define SI468X_GET_FUNC_INFO			0x12	//Returns the Function revision information of the device.

//COMMAND AND REPLY HELPERS
#define SI468X_READ_OFFSET				0x10	//Reads a portion of response buffer from an offset.
#define SI468X_WRITE_STORAGE			0x15	//Writes data to the on board storage area at a specified offset.
#define SI468X_READ_STORAGE				0x16	//Reads data from the on board storage area from a specified offset.

//PROPERTY HANDLING
#define SI468X_SET_PROPERTY				0x13	//Sets the value of a property.
#define SI468X_GET_PROPERTY 			0x14	//Gets the value of a property.

//AGC
#define SI468X_GET_AGC_STATUS			0x17	//Reports the status of the AGC.

//TUNE
#define SI468X_





#endif /* INC_SI468X_DEFS_H_ */
