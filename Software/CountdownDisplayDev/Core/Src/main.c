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
	Rect boundingRect;
} TextElement;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

SPI_HandleTypeDef hspi1;

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
TextElement *textElements;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */
void writeLCD(ST7735_JOB job, uint8_t *data);
void writeLCDColour(PixelData data);
PixelData colourFromHex(uint8_t r, uint8_t g, uint8_t b);
void drawText(uint8_t index);
Rect drawTextCentered(uint8_t x, uint8_t y, const char* text, PixelData colour);
Rect drawCharacter(uint8_t x, uint8_t y, FontCharacter character, PixelData colour);
void updateScreen();
void updateRect(Rect rect);
void addTextElement(const char* text, PixelData colour, Rect rect);
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
  textElements = malloc(sizeof(TextElement));
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
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  uint32_t jobSize = sizeof(jobs) / sizeof(ST7735_JOB);
  for(int i = 0; i < jobSize; i++) {
	  writeLCD(jobs[i], startupData);
  }
  writeLCD((ST7735_JOB) {ST7735_DISPOFF, 0, 0, 100}, 0);
  writeLCDColour(colourFromHex(0x35, 0x35, 0x35)); // Set background colour
  addTextElement("ABC\n123", Magenta, (Rect) {50, 50, 0, 0});
  updateScreen(); // Push screen 2D array to memory
  writeLCD((ST7735_JOB) {ST7735_DISPON, 0, 0, 100}, 0);
  addTextElement("AAA", Cyan, (Rect) {0, 0, 0, 0});
  writeLCD((ST7735_JOB) {ST7735_DISPOFF, 0, 0, 100}, 0);
  for(int i = 0; i < textElementsCount; i++) {
	updateRect(textElements[i].boundingRect);
  }
  writeLCD((ST7735_JOB) {ST7735_DISPON, 0, 0, 100}, 0);
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

  /*AXI clock gating */
  RCC->CKGAENR = 0xFFFFFFFF;

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = 64;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_HSI, RCC_MCODIV_1);
  HAL_RCC_MCOConfig(RCC_MCO2, RCC_MCO2SOURCE_SYSCLK, RCC_MCODIV_1);
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
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x0;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(USB_FS_PWR_EN_GPIO_Port, USB_FS_PWR_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LD1_Pin|LD3_Pin|SPI_DC_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_FS_PWR_EN_Pin */
  GPIO_InitStruct.Pin = USB_FS_PWR_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(USB_FS_PWR_EN_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI_CS_Pin */
  GPIO_InitStruct.Pin = SPI_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(SPI_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : LD1_Pin LD3_Pin */
  GPIO_InitStruct.Pin = LD1_Pin|LD3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : STLINK_RX_Pin STLINK_TX_Pin */
  GPIO_InitStruct.Pin = STLINK_RX_Pin|STLINK_TX_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_FS_OVCR_Pin */
  GPIO_InitStruct.Pin = USB_FS_OVCR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USB_FS_OVCR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : PC9 */
  GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PA8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : USB_FS_ID_Pin */
  GPIO_InitStruct.Pin = USB_FS_ID_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_HS;
  HAL_GPIO_Init(USB_FS_ID_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USB_FS_N_Pin USB_FS_P_Pin */
  GPIO_InitStruct.Pin = USB_FS_N_Pin|USB_FS_P_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI_DC_Pin */
  GPIO_InitStruct.Pin = SPI_DC_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(SPI_DC_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

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

void drawText(uint8_t index) {
	TextElement te = textElements[index];
	uint8_t x = te.boundingRect.x;
	uint8_t y = te.boundingRect.y;
	Rect temp = {x, y, 0, 45};
	size_t textLength = strlen(te.text);
	uint8_t xPos = x;
	uint8_t yPos = y;
	bool draw = true;
	for(int i = 0; i < textLength; i++) {
		char currChar = te.text[i];
		if((xPos - x) > temp.w) {
			temp.w = xPos - x;
		}
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
				xPos = x;
				yPos += 45;
				temp.h += 45;
				break;
			default:
				break;
		}
		if(draw) {
			FontCharacter fc = font[(uint8_t) currChar];
			(void) drawCharacter(xPos, yPos, fc, te.colour);
			xPos += 24;
		}
		draw = true;
	}
	temp.w += 17;
	textElements[index].boundingRect.w = temp.w;
	textElements[index].boundingRect.h = temp.h;
}

Rect drawTextCentered(uint8_t x, uint8_t y, const char* text, PixelData colour) {
	Rect temp = {0, 0, 0, 0};
	return temp;
}

Rect drawCharacter(uint8_t x, uint8_t y, FontCharacter character, PixelData colour) {

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
	return (Rect) {x, y, 32, 45};
}

void updateScreen() {
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
			HAL_SPI_Transmit(&hspi1, screen[i][j].data, 3, HAL_MAX_DELAY);
		}
	}
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
}

void updateRect(Rect rect) {
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
			HAL_SPI_Transmit(&hspi1, screen[i][j].data, 3, HAL_MAX_DELAY);
		}
	}
	HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_SET);
}

void addTextElement(const char* text, PixelData colour, Rect rect) {
	TextElement te = {text, colour, rect};
	textElements[textElementsCount] = te;
	textElements = realloc(textElements, textElementsCount++); // Add another spare element to the end of the array.
}

int __io_putchar(int ch) {
	ITM_SendChar(ch);
	return ch;
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
