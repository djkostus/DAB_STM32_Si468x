/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DAB_GPIO_INTB_Pin GPIO_PIN_4
#define DAB_GPIO_INTB_GPIO_Port GPIOA
#define DAB_GPIO_SCK_Pin GPIO_PIN_5
#define DAB_GPIO_SCK_GPIO_Port GPIOA
#define DAB_GPIO_MISO_Pin GPIO_PIN_6
#define DAB_GPIO_MISO_GPIO_Port GPIOA
#define DAB_GPIO_MOSI_Pin GPIO_PIN_7
#define DAB_GPIO_MOSI_GPIO_Port GPIOA
#define DAB_GPIO_CS_Pin GPIO_PIN_4
#define DAB_GPIO_CS_GPIO_Port GPIOC
#define DAB_GPIO_RESET_Pin GPIO_PIN_5
#define DAB_GPIO_RESET_GPIO_Port GPIOC
#define USER_BUTTON_Pin GPIO_PIN_0
#define USER_BUTTON_GPIO_Port GPIOB
#define TEST_OUT_Pin GPIO_PIN_14
#define TEST_OUT_GPIO_Port GPIOE
#define PC_UART_TX_Pin GPIO_PIN_10
#define PC_UART_TX_GPIO_Port GPIOB
#define PC_UART_RX_Pin GPIO_PIN_11
#define PC_UART_RX_GPIO_Port GPIOB
#define LCD_SCK_Pin GPIO_PIN_13
#define LCD_SCK_GPIO_Port GPIOB
#define LCD_MISO_Pin GPIO_PIN_14
#define LCD_MISO_GPIO_Port GPIOB
#define LCD_MOSI_Pin GPIO_PIN_15
#define LCD_MOSI_GPIO_Port GPIOB
#define LCD_CS_Pin GPIO_PIN_8
#define LCD_CS_GPIO_Port GPIOD
#define LCD_RST_Pin GPIO_PIN_9
#define LCD_RST_GPIO_Port GPIOD
#define LCD_DC_Pin GPIO_PIN_10
#define LCD_DC_GPIO_Port GPIOD
#define LED_GREEN_Pin GPIO_PIN_12
#define LED_GREEN_GPIO_Port GPIOD
#define LED_ORANGE_Pin GPIO_PIN_13
#define LED_ORANGE_GPIO_Port GPIOD
#define LED_RED_Pin GPIO_PIN_14
#define LED_RED_GPIO_Port GPIOD
#define LED_BLUE_Pin GPIO_PIN_15
#define LED_BLUE_GPIO_Port GPIOD
#define BACKLIGHT_PWM_OUT_Pin GPIO_PIN_8
#define BACKLIGHT_PWM_OUT_GPIO_Port GPIOC
#define DAB_GPIO_SDA_Pin GPIO_PIN_9
#define DAB_GPIO_SDA_GPIO_Port GPIOC
#define DAB_GPIO_SCL_Pin GPIO_PIN_8
#define DAB_GPIO_SCL_GPIO_Port GPIOA
#define TOUCH_IRQ_Pin GPIO_PIN_6
#define TOUCH_IRQ_GPIO_Port GPIOD
#define TOUCH_CS_Pin GPIO_PIN_7
#define TOUCH_CS_GPIO_Port GPIOD
#define TOUCH_CLK_Pin GPIO_PIN_3
#define TOUCH_CLK_GPIO_Port GPIOB
#define TOUCH_DO_Pin GPIO_PIN_4
#define TOUCH_DO_GPIO_Port GPIOB
#define TOUCH_DIN_Pin GPIO_PIN_5
#define TOUCH_DIN_GPIO_Port GPIOB
#define EEPROM_SCL_Pin GPIO_PIN_6
#define EEPROM_SCL_GPIO_Port GPIOB
#define EEPROM_SDA_Pin GPIO_PIN_7
#define EEPROM_SDA_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
