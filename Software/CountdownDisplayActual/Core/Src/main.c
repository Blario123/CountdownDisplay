/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "st7735.h"
#include "font.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
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

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */
PixelData screen[128][160];

FontCharacter font[] = { // A-Z, 0-9 (36 total characters)
		{32, 0},
		{32, 1},
		{32, 2},
		{32, 3},
		{32, 4},
		{32, 5},
		{32, 6},
		{32, 7},
		{32, 8},
		{32, 9},
		{32, 10},
		{32, 11},
		{32, 12},
		{32, 13},
		{32, 14},
		{32, 15},
		{42, 16}, // Q
		{32, 17},
		{32, 18},
		{32, 19},
		{32, 20},
		{32, 21},
		{32, 22},
		{32, 23},
		{32, 24},
		{32, 25},
		{32, 26}, // 0
		{32, 27},
		{32, 28},
		{32, 29},
		{32, 30},
		{32, 31},
		{32, 32},
		{32, 33},
		{32, 34},
		{32, 35},
};

uint8_t textElementsCount = 0;
TextElement textElements[32];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */
void writeLCD(ST7735_JOB job, uint8_t *data);
void writeLCDColour(PixelData data);
PixelData colourFromHex(uint8_t r, uint8_t g, uint8_t b);
void drawText(TextElement te);
void drawCharacter(uint8_t x, uint8_t y, FontCharacter character, PixelData colour);
void updateBuffer();
void updateScreen();
void updateRect(Rect rect);
void addText(const char* text, PixelData colour, Point pos, bool centered);
void removeText(int index);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{

	/* USER CODE BEGIN 1 */
	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */
	uint8_t startupData[] = {
			0x01, 0x2C, 0x2D, // FRMCTR1 // FRMCTR2 // FRMCTR3 (1)
			0x01, 0x2C, 0x2D, // FRMCTR3 (2)
			0x07,			  // INVCTR
			0xA2, 0x02, 0x84, // PWCTR1
			0xC5,			  // PWCTR2
			0x0A, 0x00,		  // PWCTR3
			0x8A, 0x2A,		  // PWCTR4
			0x8A, 0xEE,		  // PWCTR5
			0x0E, 0x0E,		  // VMCTR1
			0x20,			  // MADCTL
			0x06,			  // COLMOD
			0x00, 0x00, 0x00, 0x7F, // RASET
			0x00, 0x00, 0x00, 0x9F, // CASET
			0x02, 0x1C, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2D, 0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10, // GAMCTRP1
			0x03, 0x1D, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D, 0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10, // GAMCTRN1
	};

	ST7735_JOB jobs[] = {
			{ST7735_SWRESET, 0, 0, 150},
			{ST7735_SLPOUT, 0, 0, 500},
			{ST7735_FRMCTR1, 0, 3, 0},
			{ST7735_FRMCTR2, 0, 3, 0},
			{ST7735_FRMCTR3, 0, 6, 0},
			{ST7735_INVCTR, 6, 1, 0},
			{ST7735_PWCTR1, 7, 3, 0},
			{ST7735_PWCTR2, 10, 1, 0},
			{ST7735_PWCTR3, 11, 2, 0},
			{ST7735_PWCTR4, 13, 2, 0},
			{ST7735_PWCTR5, 15, 2, 0},
			{ST7735_VMCTR1, 17, 2, 0},
			{ST7735_INVOFF, 19, 0, 0},
			{ST7735_MADCTL, 19, 1, 0},
			{ST7735_COLMOD, 20, 1, 0},
			{ST7735_RASET, 21, 4, 0},
			{ST7735_CASET, 25, 4, 0},
			{ST7735_GAMCTRP1, 29, 16, 0},
			{ST7735_GAMCTRN1, 45, 16, 0},
			{ST7735_NORON, 51, 0, 10},
			{ST7735_DISPON, 51, 0, 100},
			{ST7735_TEOFF, 51, 0, 0},
	};
	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_RTC_Init();
	MX_SPI1_Init();
	MX_TIM2_Init();
	/* USER CODE BEGIN 2 */
	HAL_Delay(1000);
	uint32_t jobSize = sizeof(jobs) / sizeof(ST7735_JOB);
	for(int i = 0; i < jobSize; i++) {
		writeLCD(jobs[i], startupData);
	}
	writeLCDColour(colourFromHex(0x35, 0x35, 0x35)); // Set background colour
	updateScreen();
	addText("ABC\n123", Magenta, (Point) {50, 50}, true);
	addText("AAA", Cyan, (Point) {0, 0}, false);
	updateBuffer();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	/** Configure the main internal regulator output voltage
	 */
	HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
	RCC_OscInitStruct.LSIState = RCC_LSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
	{
		Error_Handler();
	}
}

/**
 * @brief RTC Initialization Function
 * @param None
 * @retval None
 */
static void MX_RTC_Init(void)
{

	/* USER CODE BEGIN RTC_Init 0 */

	/* USER CODE END RTC_Init 0 */

	/* USER CODE BEGIN RTC_Init 1 */

	/* USER CODE END RTC_Init 1 */

	/** Initialize RTC Only
	 */
	hrtc.Instance = RTC;
	hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
	hrtc.Init.AsynchPrediv = 127;
	hrtc.Init.SynchPrediv = 255;
	hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
	hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
	hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
	hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
	hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
	if (HAL_RTC_Init(&hrtc) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN RTC_Init 2 */

	/* USER CODE END RTC_Init 2 */

}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void)
{

	/* USER CODE BEGIN SPI1_Init 0 */

	/* USER CODE END SPI1_Init 0 */

	/* USER CODE BEGIN SPI1_Init 1 */

	/* USER CODE END SPI1_Init 1 */
	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 7;
	hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
	if (HAL_SPI_Init(&hspi1) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN SPI1_Init 2 */

	/* USER CODE END SPI1_Init 2 */

}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void)
{

	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = {0};
	TIM_MasterConfigTypeDef sMasterConfig = {0};

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 1000;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 4294967295;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
	{
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
	{
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
	{
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	/* USER CODE BEGIN MX_GPIO_Init_1 */
	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, SPI_DC_Pin|SPI_CS_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : SPI_DC_Pin SPI_CS_Pin */
	GPIO_InitStruct.Pin = SPI_DC_Pin|SPI_CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/* USER CODE BEGIN MX_GPIO_Init_2 */
	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void writeLCD(ST7735_JOB job, uint8_t *data) {
	HAL_GPIO_WritePin(SPI_DC_GPIO_Port, SPI_DC_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &job.op, 1, HAL_MAX_DELAY);
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
	if(job.dataLength != 0) {
		HAL_GPIO_WritePin(SPI_DC_GPIO_Port, SPI_DC_Pin, GPIO_PIN_SET);
		for(int i = job.offset; i < job.offset + job.dataLength; i++) {
			HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
			HAL_SPI_Transmit(&hspi1, data + i, 1, HAL_MAX_DELAY);
			HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
		}
	}
}

void writeLCDColour(PixelData data) {
	for(int i = 0; i < 128; i++) {
		for(int j = 0; j < 160; j++) {
			screen[i][j] = data;
		}
	}
}

PixelData colourFromHex(uint8_t r, uint8_t g, uint8_t b) {
	return (PixelData) {{r, g, b}};
}

void drawText(TextElement te) {
	// Determine the longest line of text
	int xOffset = 0;
	if(te.centered) {
		char* newLinePos = index(te.text, '\n');
		int longestLine = 0;
		// If word has no newlines, take the current text length
		if(newLinePos == NULL) {
			longestLine = strlen(te.text);
		} else {
			int tempCount = 0;
			// Iterate through the string gathering the longest distance between two newlines
			for(int i = 0; i < strlen(te.text) + 1; i++) {
				if(te.text[i] != '\n' && te.text[i] != 0x00) {
					tempCount++;
				} else {
					if(tempCount > longestLine) {
						longestLine = tempCount;
					}
					tempCount = 0;
				}
			}
		}
		int longestWidth = 24 * longestLine;
		xOffset = longestWidth / 2;
	}
	size_t textLength = strlen(te.text);
	uint8_t xPos = te.point.x - xOffset;
	uint8_t yPos = te.point.y;
	bool draw = true;
	for(int i = 0; i < textLength; i++) {
		char currChar = te.text[i];
		switch(currChar) {
		case 'A' ... 'Z':
		currChar -= 'A';
		break;
		case '0' ... '9':
		currChar -= '0';
		currChar += 26;
		break;
		case '\n':
			draw = false;
			xPos = te.point.x;
			yPos += 45;
			break;
		default:
			break;
		}
		if(draw) {
			FontCharacter fc = font[(uint8_t) currChar];
			drawCharacter(xPos, yPos, fc, te.colour);
			xPos += 24;
		}
		draw = true;
	}
}

void drawCharacter(uint8_t x, uint8_t y, FontCharacter character, PixelData colour) {
	uint16_t dataPos = character.start * 96;
	if(character.start > 16) {
		dataPos += 30; // 10 rows * 3 bytes
	}
	for(int i = 0; i < character.height; i++) {
		for(int j = 0; j < 3; j++) { // Iterate through each byte
			for(int k = 7; k > -1; k--) { // Iterate through the bits of the byte
				if((fontData[dataPos + j] & (0b1 << k)) > 0) {
					uint8_t xPos = x + ((j * 8) - k);
					uint8_t yPos = i + y;
					if(xPos > 1 && xPos < 128 && yPos > 0 && yPos < 160) {
						screen[xPos][yPos] = colour;
					}
				}
			}
		}
		dataPos += 3;
	}
}

void updateBuffer() {
	for(int i = 0; i < textElementsCount; i++) {
		drawText(textElements[i]);
	}
	updateScreen();
}

void updateScreen() {
	writeLCD((ST7735_JOB) {ST7735_DISPOFF, 0, 0, 100}, 0);
	uint8_t width[4] = {0x00, 0x00, 0x00, 0x7F}; // Max width and height
	uint8_t height[4] = {0x00, 0x00, 0x00, 0x9F};
	ST7735_JOB j = {ST7735_RASET, 0, 4, 0};
	writeLCD(j, width);
	j = (ST7735_JOB) {ST7735_CASET, 0, 4, 0};
	writeLCD(j, height);
	j = (ST7735_JOB) {ST7735_RAMWR, 0, 0, 0};
	writeLCD(j, NULL);
	HAL_GPIO_WritePin(SPI_DC_GPIO_Port, SPI_DC_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
	for(int i = 0; i < 128; i++) {
		for(int j = 0; j < 160; j++) {
			if(HAL_SPI_Transmit(&hspi1, screen[i][j].data, 3, HAL_MAX_DELAY) != HAL_OK) {
				Error_Handler();
			}
		}
	}
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
	writeLCD((ST7735_JOB) {ST7735_DISPON, 0, 0, 100}, 0);
}

void updateRect(Rect rect) {
	writeLCD((ST7735_JOB) {ST7735_DISPOFF, 0, 0, 100}, 0);
	uint8_t width[4] = {0x00, rect.x, 0x00, rect.x + rect.w}; // Bounding rect of object
	uint8_t height[4] = {0x00, rect.y, 0x00, rect.y + rect.h - 1};
	ST7735_JOB j = {ST7735_RASET, 0, 4, 0};
	writeLCD(j, width);
	j = (ST7735_JOB) {ST7735_CASET, 0, 4, 0};
	writeLCD(j, height);
	j = (ST7735_JOB) {ST7735_RAMWR, 0, 0, 0};
	writeLCD(j, NULL);
	HAL_GPIO_WritePin(SPI_DC_GPIO_Port, SPI_DC_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);
	for(int i = rect.x; i < rect.x + rect.w; i++) {
		for(int j = rect.y; j < rect.y + rect.h; j++) {
			HAL_SPI_Transmit(&hspi1, screen[i][j].data, 3, 10);
		}
	}
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
	writeLCD((ST7735_JOB) {ST7735_DISPON, 0, 0, 100}, 0);
}

void addText(const char* text, PixelData colour, Point pos, bool centered) {
	TextElement te = {text, colour, pos, centered};
	textElements[textElementsCount++] = te;
}

void removeText(int index) {
	if(index > textElementsCount) {
		return;
	}
	for(int i = index; i < textElementsCount; i++) {
		textElements[i] = textElements[i + 1];
	}
}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
