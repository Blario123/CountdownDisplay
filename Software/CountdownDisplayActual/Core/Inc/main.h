/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32g0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdbool.h"
#include "st7735.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct {
	uint8_t x;
	uint8_t y;
	uint8_t w;
	uint8_t h;
} Rect;

typedef struct {
	uint8_t x;
	uint8_t y;
} Point;

typedef struct {
	const char* text;
	PixelData colour;
	Point point;
	bool centered;
} TextElement;
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
void updateBuffer();
void addText(const char* text, PixelData colour, Point pos, bool centered);
void removeText(int index);
int calculateDaysRemaining(RTC_DateTypeDef from, RTC_DateTypeDef to);
char* intToString(int i);
extern int currentDate;
extern RTC_DateTypeDef jan1;
extern RTC_HandleTypeDef hrtc;
extern int daysRemaining;
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SPI_DC_Pin GPIO_PIN_0
#define SPI_DC_GPIO_Port GPIOA
#define SPI_CS_Pin GPIO_PIN_3
#define SPI_CS_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
