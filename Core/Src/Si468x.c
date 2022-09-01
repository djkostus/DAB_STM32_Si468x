/*
 * Si468x.c
 *
 *  Created on: Aug 22, 2022
 *      Author: mgostek
 */

#include "main.h"
#include "spi.h"
#include "i2c.h"
#include "Si468x.h"
#include "Si468x_defs.h"
#include "patch_mini.h"
#include "patch_full.h"

#define BOOT_WRITE_STEPS 3

uint8_t dab_spi_tx_buffer[SPI_TX_BUFFER_SIZE];
uint8_t dab_spi_rx_buffer[SPI_RX_BUFFER_SIZE];

RETURN_CODE status = 0;


void Si468x_init()
{
	Si468x_reset();
	Si468x_power_up();
	Si468x_load_init();
	Si468x_bootloader_load_host();
	Si468x_load_init();
	Si468x_firmware_load_flash();
	Si468x_boot();
	HAL_Delay(3000);
	Si468x_get_part_info();
	HAL_Delay(5000);
	Si468x_get_sys_state();
}

void Si468x_reset()
{
	RESET_PIN_LOW;
	HAL_Delay(100);
	RESET_PIN_HIGH;
	HAL_Delay(200);
}

void Si468x_disable()
{
	RESET_PIN_HIGH;
}

void Si468x_power_up()
{
	DisplayState("Power Up...");
	dab_spi_tx_buffer[0]  = SI468X_CMD_POWER_UP;	//Power up command code
	dab_spi_tx_buffer[1]  = 0x80;					//Disable toggling host interrupt line = 0x00; enable = 0x80
	dab_spi_tx_buffer[2]  = 0x17;					//Clock mode 1 (crystal mode), TR_SIZE = 0x7 for 19.2MHz crystal
	dab_spi_tx_buffer[3]  = 0x50;					//start IBIAS = 800 uA (80 * 10) for 19.2MHz Crystal with 250R startup ESR
	dab_spi_tx_buffer[4]  = 0x00;					//crystal frequency in MHz [7:0], 19.2 MHz
	dab_spi_tx_buffer[5]  = 0xF8;	//F9??				//crystal frequency in MHz [15:8], 19.2 MHz
	dab_spi_tx_buffer[6]  = 0x24;					//crystal frequency in MHz [23:16], 19.2 MHz
	dab_spi_tx_buffer[7]  = 0x01;					//crystal frequency in MHz [31:24], 19.2 MHz
	dab_spi_tx_buffer[8]  = 0x00;					//CTUN = 0 (there's no need to add load capacitance)
	dab_spi_tx_buffer[9]  = 0x10;					//not used - value as in documentation
	dab_spi_tx_buffer[10] = 0x00;					//not used - value as in documentation
	dab_spi_tx_buffer[11] = 0x00;					//not used - value as in documentation
	dab_spi_tx_buffer[12] = 0x00;					//not used - value as in documentation
	dab_spi_tx_buffer[13] = 0x28;					//running IBIAS = 400 uA (80 * 10) for 19.2MHz Crystal with 50R running ESR (recommended: start IBIAS = 2 * running IBIAS)
	dab_spi_tx_buffer[14] = 0x00;					//not used - value as in documentation
	dab_spi_tx_buffer[15] = 0x00;					//not used - value as in documentation

	status = Si468x_write_command(16, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(5, dab_spi_rx_buffer);
}

void Si468x_load_init()
{
	dab_spi_tx_buffer[0]  = SI468X_CMD_LOAD_INIT;	//Load Init command code
	dab_spi_tx_buffer[1]  = 0x00;					//Disable toggling host interrupt line

	status = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(5);
	status = Si468x_read_reply(5, dab_spi_rx_buffer);

	DisplayStatusReg();
}

void Si468x_bootloader_load_host()
{
	ClearOK();
	DisplayState("Boot Write ");
	uint16_t patch_size = sizeof(patch_full);
	DisplayFirmwareTotalSize(patch_size);

	dab_spi_tx_buffer[0] = SI468X_CMD_HOST_LOAD;
	dab_spi_tx_buffer[1] = 0x00;
	dab_spi_tx_buffer[2] = 0x00;
	dab_spi_tx_buffer[3] = 0x00;

	uint16_t i, j;

	for(i = 0; i < BOOT_WRITE_STEPS; i++)
	{
		for (j = 0; j < patch_size / BOOT_WRITE_STEPS; j++)
		{
			dab_spi_tx_buffer[4 + j] = patch_full[j + (patch_size / BOOT_WRITE_STEPS) * i];
		}
		status = Si468x_write_command(patch_size / BOOT_WRITE_STEPS + 4, dab_spi_tx_buffer);
		HAL_Delay(5);
		status = Si468x_read_reply(5, dab_spi_rx_buffer);
		DisplayStatusReg();
		DisplayFirmwareDownloadStatus(j + (patch_size / BOOT_WRITE_STEPS) * i);
	}

	DisplayOK();
}

void Si468x_firmware_load_flash()
{
	DisplayState("Image Write");
	dab_spi_tx_buffer[0]  = SI468X_CMD_FLASH_LOAD;	//Power up command code
	dab_spi_tx_buffer[1]  = 0x00;					//not used - value as in documentation
	dab_spi_tx_buffer[2]  = 0x00;					//not used - value as in documentation
	dab_spi_tx_buffer[3]  = 0x00;					//not used - value as in documentation
	dab_spi_tx_buffer[4]  = 0x00;					//Flash start address [7:0] 0x6000
	dab_spi_tx_buffer[5]  = 0x60;					//Flash start address [15:8]
	dab_spi_tx_buffer[6]  = 0x00;					//Flash start address [23:16]
	dab_spi_tx_buffer[7]  = 0x00;					//Flash start address [31:24]
	dab_spi_tx_buffer[8]  = 0x00;					//not used - value as in documentation
	dab_spi_tx_buffer[9]  = 0x00;					//not used - value as in documentation
	dab_spi_tx_buffer[10] = 0x00;					//not used - value as in documentation
	dab_spi_tx_buffer[11] = 0x00;					//not used - value as in documentation

	Si468x_write_command(12, dab_spi_tx_buffer);
	HAL_Delay(5);
	status = Si468x_read_reply(5, dab_spi_rx_buffer);
	DisplayStatusReg();
}

void Si468x_boot()
{
	DisplayState("Booting... ");
	dab_spi_tx_buffer[0]  = SI468X_CMD_BOOT;	//Power up command code
	dab_spi_tx_buffer[1]  = 0x00;				//Value as in documentation

	status  = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(300);
	status = Si468x_read_reply(5, dab_spi_rx_buffer);
	DisplayStatusReg();
}

void Si468x_write_single_byte(uint8_t byte_to_write)
{
	HAL_SPI_Transmit(&hspi1, &byte_to_write, 1, 100);
}

uint8_t Si468x_read_single_byte()
{
	uint8_t data_buffer = 0;
	HAL_SPI_Receive(&hspi1, &data_buffer, 1, 100);
	return data_buffer;
}

RETURN_CODE Si468x_wait_for_CTS(uint16_t timeout)
{
	uint16_t i;
	uint8_t cts_status = 0;

	for(i = 0; i < timeout; i++)
	{
		Si468x_read_multiple(1, &cts_status);

		if((cts_status & 0x80) == 0x80)
		{
			if((cts_status & 0x40) == 0x40)
			{
				return COMMAND_ERROR;
			}
			return SUCCESS;
		}

		// delay function for 1 ms
		HAL_Delay(1);
	}
	return TIMEOUT;
}

void Si468x_write_multiple(uint16_t len, uint8_t* data_to_write)
{
	uint16_t i = 0;
	CS_PIN_LOW;
	while(len--)
	{
		Si468x_write_single_byte(data_to_write[i++]);
	}
	CS_PIN_HIGH;
}

void Si468x_read_multiple(uint16_t len, uint8_t* read_buffer)
{
	uint16_t i = 0;
	CS_PIN_HIGH;
	HAL_Delay(10);
	CS_PIN_LOW;
	Si468x_write_single_byte(0x00);
	while(len--)
	{
		read_buffer[i++] = Si468x_read_single_byte();
	}
	CS_PIN_HIGH;
}

RETURN_CODE Si468x_write_command(uint16_t length, uint8_t *buffer)
{
	RETURN_CODE ret = Si468x_wait_for_CTS(POLL_TIMEOUT_MS);

	if((ret == SUCCESS) || (ret == COMMAND_ERROR)) //It is possible that there is a pending command error - we need to be able to send another command to clear out this state
	{
		Si468x_write_multiple(length, buffer);

		ret = Si468x_wait_for_CTS(POLL_TIMEOUT_MS);
	}
	return ret;
}

RETURN_CODE Si468x_read_reply(uint16_t length, uint8_t *buffer)
{
	Si468x_read_multiple(length, buffer);
	return SUCCESS;
}

uint8_t get_rx_buffer(int buffer_pos)
{
	return dab_spi_rx_buffer[buffer_pos];
}

void Si468x_get_sys_state()
{
	DisplayState("Get sys stat");
	dab_spi_tx_buffer[0]  = SI468X_CMD_GET_SYS_STATE;	//Get Sys State Command Code
	dab_spi_tx_buffer[1]  = 0x00;						//Value as in documentation

	status = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(5, dab_spi_rx_buffer);
	DisplayStatusReg();
}

void Si468x_get_part_info()
{
	DisplayState("Get part info");
	dab_spi_tx_buffer[0]  = SI468X_CMD_GET_PART_INFO;	//Get Part Info Command Code
	dab_spi_tx_buffer[1]  = 0x00;						//Value as in documentation

	status = Si468x_write_command(2, dab_spi_tx_buffer);
	HAL_Delay(1);
	status = Si468x_read_reply(10, dab_spi_rx_buffer);
	DisplayStatusReg();

	HAL_Delay(5000);
	DisplayState("Show part info");
	dab_spi_rx_buffer[0] = dab_spi_rx_buffer[4];
	dab_spi_rx_buffer[1] = dab_spi_rx_buffer[5];
	dab_spi_rx_buffer[2] = dab_spi_rx_buffer[8];
	dab_spi_rx_buffer[3] = dab_spi_rx_buffer[9];
	DisplayStatusReg();
}



