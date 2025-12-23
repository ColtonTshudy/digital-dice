#include "GUI.h"
#include "lcd.h"
#include <stdlib.h>

void GUI_DrawPoint(uint16_t x, uint16_t y, uint16_t color) {
    LCD_DrawPoint(x, y, color);
}

void GUI_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (1) {
        LCD_DrawPoint(x1, y1, color);
        
        if (x1 == x2 && y1 == y2) break;
        
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

void GUI_DrawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    GUI_DrawLine(x1, y1, x2, y1, color);
    GUI_DrawLine(x2, y1, x2, y2, color);
    GUI_DrawLine(x2, y2, x1, y2, color);
    GUI_DrawLine(x1, y2, x1, y1, color);
}

void GUI_FillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    LCD_Fill(x1, y1, x2, y2, color);
}

void GUI_DrawCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    while (x <= y) {
        LCD_DrawPoint(x0 + x, y0 + y, color);
        LCD_DrawPoint(x0 - x, y0 + y, color);
        LCD_DrawPoint(x0 + x, y0 - y, color);
        LCD_DrawPoint(x0 - x, y0 - y, color);
        LCD_DrawPoint(x0 + y, y0 + x, color);
        LCD_DrawPoint(x0 - y, y0 + x, color);
        LCD_DrawPoint(x0 + y, y0 - x, color);
        LCD_DrawPoint(x0 - y, y0 - x, color);

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}

void GUI_FillCircle(uint16_t x0, uint16_t y0, uint16_t r, uint16_t color) {
    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    while (x <= y) {
        for (int i = x0 - x; i <= x0 + x; i++) {
            LCD_DrawPoint(i, y0 + y, color);
            LCD_DrawPoint(i, y0 - y, color);
        }
        for (int i = x0 - y; i <= x0 + y; i++) {
            LCD_DrawPoint(i, y0 + x, color);
            LCD_DrawPoint(i, y0 - x, color);
        }

        if (d < 0) {
            d = d + 4 * x + 6;
        } else {
            d = d + 4 * (x - y) + 10;
            y--;
        }
        x++;
    }
}
