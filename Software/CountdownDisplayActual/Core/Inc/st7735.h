/*
 * st7735.h
 *
 *  Created on: Aug 16, 2024
 *      Author: blair
 */

#ifndef INC_ST7735_H_
#define INC_ST7735_H_

typedef enum {
 ST7735_NOP = 0x00,
 ST7735_SWRESET = 0x01,
 ST7735_RDDID = 0x04,
 ST7735_RDDST = 0x09,
 ST7735_RDDPM = 0x0A,
 ST7735_RDDMADCTL = 0x0B,
 ST7735_RDDCOLMOD = 0x0C,
 ST7735_RDDIM = 0x0D,
 ST7735_RDDSM = 0x0E,

 ST7735_SLPIN = 0x10,
 ST7735_SLPOUT = 0x11,
 ST7735_PTLON = 0x12,
 ST7735_NORON = 0x13,

 ST7735_INVOFF = 0x20,
 ST7735_INVON = 0x21,
 ST7735_GAMSET = 0x26,
 ST7735_DISPOFF = 0x28,
 ST7735_DISPON = 0x29,
 ST7735_CASET = 0x2A,
 ST7735_RASET = 0x2B,
 ST7735_RAMWR = 0x2C,
 ST7735_RAMRD = 0x2E,

 ST7735_PTLAR = 0x30,
 ST7735_TEOFF = 0x34,
 ST7735_TEON = 0x35,
 ST7735_MADCTL = 0x36,
 ST7735_IDMOFF = 0x38,
 ST7735_IDMON = 0x39,
 ST7735_COLMOD = 0x3A,

 ST7735_FRMCTR1 = 0xB1,
 ST7735_FRMCTR2 = 0xB2,
 ST7735_FRMCTR3 = 0xB3,
 ST7735_INVCTR = 0xB4,
 ST7735_DISSET5 = 0xB6,

 ST7735_PWCTR1 = 0xC0,
 ST7735_PWCTR2 = 0xC1,
 ST7735_PWCTR3 = 0xC2,
 ST7735_PWCTR4 = 0xC3,
 ST7735_PWCTR5 = 0xC4,
 ST7735_VMCTR1 = 0xC5,
 ST7735_VMOFCTR = 0xC7,

 ST7735_WRID2 = 0xD1,
 ST7735_WRID3 = 0xD2,
 ST7735_NVCTR1 = 0xD9,
 ST7735_RDID1 = 0xDA,
 ST7735_RDID2 = 0xDB,
 ST7735_RDID3 = 0xDC,
 ST7735_NVCTR2 = 0xDE,
 ST7735_NVCTR3 = 0xDF,

 ST7735_GAMCTRP1 = 0xE0,
 ST7735_GAMCTRN1 = 0xE1,

 ST7735_EXTCTRL = 0xF0,
 ST7735_PWCTR6 = 0xFC,
 ST7735_VCOM4L = 0xFF
} ST7735_OP;

#define ST7735_DUMMYREAD 0x66

typedef struct {
	ST7735_OP op;
	uint8_t offset;
	uint8_t dataLength;
	uint32_t delay;
} ST7735_JOB;

typedef union {
	struct {
		uint8_t r;
		uint8_t g;
		uint8_t b;
	};
	uint8_t data[3];
} PixelData;

PixelData White = {{0xFF, 0xFF, 0xFF}};
PixelData Red = {{0xFF, 0x00, 0x00}};
PixelData Green = {{0x00, 0xFF, 0x00}};
PixelData Blue = {{0x00, 0x00, 0xFF}};
PixelData Black = {{0x00, 0x00, 0x00}};
PixelData Cyan = {{0x00, 0xFF, 0xFF}};
PixelData Magenta = {{0xFF, 0x00, 0xFF}};
PixelData Yellow = {{0xFF, 0xFF, 0x00}};

typedef struct {
	uint8_t height;
	uint16_t start;
} FontCharacter;

#endif /* INC_ST7735_H_ */
