/*
 * Si468x.c
 *
 *  Created on: Aug 22, 2022
 *      Author: mgostek
 */

#include "main.h"
#include "Si468x.h"
#include "Si468x_defs.h"

uint8_t dab_spi_buffer[SPI_BUFFER_SIZE];



void Si468x_reset()
{
	HAL_GPIO_WritePin(DAB_GPIO_RESET_GPIO_Port, DAB_GPIO_RESET_Pin, 0);
	HAL_Delay(100);
	HAL_GPIO_WritePin(DAB_GPIO_RESET_GPIO_Port, DAB_GPIO_RESET_Pin, 1);
	HAL_Delay(100);
}

void Si468x_power_up()
{
	dab_spi_buffer[0] = SI468X_CMD_POWER_UP;	//Power up command code
	dab_spi_buffer[1] = 0x00;					//Disable toggling host interrupt line
	dab_spi_buffer[2] = 0x17;					//Clock mode 1 (crystal mode),


}
