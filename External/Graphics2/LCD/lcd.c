#include "lcd.h"

extern SPI_HandleTypeDef hspi1; // Adjust to your SPI handle

_lcd_dev lcddev;
uint16_t POINT_COLOR = BLACK;
uint16_t BACK_COLOR = WHITE;

static void LCD_WriteCmd(uint8_t cmd) {
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &cmd, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

static void LCD_WriteData(uint8_t data) {
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, &data, 1, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

static void LCD_WriteData16(uint16_t data) {
    uint8_t buf[2];
    buf[0] = data >> 8;
    buf[1] = data & 0xFF;
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    HAL_SPI_Transmit(&hspi1, buf, 2, HAL_MAX_DELAY);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}

void LCD_Init(void) {
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_RESET);
    HAL_Delay(100);
    HAL_GPIO_WritePin(LCD_RST_GPIO_Port, LCD_RST_Pin, GPIO_PIN_SET);
    HAL_Delay(100);

    // ILI9341 initialization sequence
    LCD_WriteCmd(0x11); // Sleep out
    HAL_Delay(120);

    LCD_WriteCmd(0x36); // Memory Access Control
    LCD_WriteData(0x48);

    LCD_WriteCmd(0x3A); // Pixel Format
    LCD_WriteData(0x55); // 16-bit color

    LCD_WriteCmd(0x29); // Display on

    lcddev.width = LCD_W;
    lcddev.height = LCD_H;
    lcddev.dir = 0;
}

void LCD_SetWindows(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    LCD_WriteCmd(0x2A); // Column address
    LCD_WriteData(x1 >> 8);
    LCD_WriteData(x1 & 0xFF);
    LCD_WriteData(x2 >> 8);
    LCD_WriteData(x2 & 0xFF);

    LCD_WriteCmd(0x2B); // Row address
    LCD_WriteData(y1 >> 8);
    LCD_WriteData(y1 & 0xFF);
    LCD_WriteData(y2 >> 8);
    LCD_WriteData(y2 & 0xFF);

    LCD_WriteCmd(0x2C); // Memory write
}

void LCD_Clear(uint16_t color) {
    LCD_Fill(0, 0, lcddev.width - 1, lcddev.height - 1, color);
}

void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color) {
    LCD_SetWindows(x, y, x, y);
    LCD_WriteData16(color);
}

void LCD_Fill(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
    LCD_SetWindows(x1, y1, x2, y2);
    uint32_t num = (x2 - x1 + 1) * (y2 - y1 + 1);
    
    HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
    
    uint8_t buf[2];
    buf[0] = color >> 8;
    buf[1] = color & 0xFF;
    
    for (uint32_t i = 0; i < num; i++) {
        HAL_SPI_Transmit(&hspi1, buf, 2, HAL_MAX_DELAY);
    }
    
    HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}
