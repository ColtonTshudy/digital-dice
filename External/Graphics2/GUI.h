#ifndef __GUI_H
#define __GUI_H

#include "main.h"

void GUI_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
void GUI_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void GUI_DrawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void GUI_FillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void GUI_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);
void GUI_FillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color);

#endif
