#ifndef __LCD_H
#define __LCD_H

#include "main.h"

// LCD dimensions
#define LCD_W 240
#define LCD_H 320

// Common colors
#define WHITE       0xFFFF
#define BLACK       0x0000
#define BLUE        0x001F
#define RED         0xF800
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define GRAY        0x8430

typedef struct {
    uint16_t width;
    uint16_t height;
    uint8_t dir;
} _lcd_dev;

extern _lcd_dev lcddev;
extern uint16_t POINT_COLOR;
extern uint16_t BACK_COLOR;

void LCD_Init(void);
void LCD_Clear(uint16_t color);
void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
void LCD_Fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_SetWindows(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

#endif
