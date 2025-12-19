#include <stdlib.h>
#include "main.h"

/* BSP LCD driver */
#include "stm32_adafruit_lcd.h"

/* BSP TS driver */
#include "stm32_adafruit_ts.h"

#ifdef osCMSIS
void StartDefaultTask(void const * argument)
#else
void mainApp(void)
#endif
{
  TS_StateTypeDef ts;
  uint16_t boxsize;
  uint16_t oldcolor, currentcolor;

  BSP_LCD_Init();
  BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
  BSP_LCD_Clear(LCD_COLOR_BLACK);
  boxsize = BSP_LCD_GetXSize() / 7;  // Changed from 6 to 7

  BSP_LCD_SetTextColor(LCD_COLOR_RED);
  BSP_LCD_FillRect(0, 0, boxsize, boxsize);
  BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
  BSP_LCD_FillRect(boxsize, 0, boxsize, boxsize);
  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
  BSP_LCD_FillRect(boxsize * 2, 0, boxsize, boxsize);
  BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
  BSP_LCD_FillRect(boxsize * 3, 0, boxsize, boxsize);
  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
  BSP_LCD_FillRect(boxsize * 4, 0, boxsize, boxsize);
  BSP_LCD_SetTextColor(LCD_COLOR_MAGENTA);
  BSP_LCD_FillRect(boxsize * 5, 0, boxsize, boxsize);
  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);

  BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
  BSP_LCD_FillRect(boxsize * 6, 0, boxsize, boxsize);
  BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
  BSP_LCD_DrawRect(boxsize * 6 + 2, 2, boxsize - 4, boxsize - 4);  // Draw X or border

  BSP_LCD_DrawRect(0, 0, boxsize, boxsize);
  currentcolor = LCD_COLOR_RED;

  static uint16_t last_x=0;
  static uint16_t last_y=0;
  static float filtered_x=0;
  static float filtered_y=0;
  static uint32_t last_touch_time=0;
  
  #define FILTER_ALPHA 0.3f  // Lower = more smoothing (0.1-0.5 range)
  #define TOUCH_TIMEOUT 10   // ms - gap to consider new touch vs continuous
  
  while(1)
  {
    BSP_TS_GetState(&ts);
    uint32_t current_time = HAL_GetTick();
    
    if(ts.TouchDetected)
    {
      uint8_t is_new_touch = (current_time - last_touch_time > TOUCH_TIMEOUT);
      
      if(ts.Y < boxsize)
      {
        oldcolor = currentcolor;

        BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
        if (ts.X < boxsize)
        {
          currentcolor = LCD_COLOR_RED;
          BSP_LCD_DrawRect(0, 0, boxsize, boxsize);
        }
        else if (ts.X < boxsize * 2)
        {
          currentcolor = LCD_COLOR_YELLOW;
          BSP_LCD_DrawRect(boxsize, 0, boxsize, boxsize);
        }
        else if (ts.X < boxsize * 3)
        {
          currentcolor = LCD_COLOR_GREEN;
          BSP_LCD_DrawRect(boxsize*2, 0, boxsize, boxsize);
        }
        else if (ts.X < boxsize * 4)
        {
          currentcolor = LCD_COLOR_CYAN;
          BSP_LCD_DrawRect(boxsize*3, 0, boxsize, boxsize);
        }
        else if (ts.X < boxsize * 5)
        {
          currentcolor = LCD_COLOR_BLUE;
          BSP_LCD_DrawRect(boxsize*4, 0, boxsize, boxsize);
        }
        else if (ts.X < boxsize * 6)
        {
          currentcolor = LCD_COLOR_MAGENTA;
          BSP_LCD_DrawRect(boxsize*5, 0, boxsize, boxsize);
        }
        else if (ts.X < boxsize * 7)
        {
          BSP_LCD_Clear(LCD_COLOR_BLACK);
          // Redraw color palette
          BSP_LCD_SetTextColor(LCD_COLOR_RED);
          BSP_LCD_FillRect(0, 0, boxsize, boxsize);
          BSP_LCD_SetTextColor(LCD_COLOR_YELLOW);
          BSP_LCD_FillRect(boxsize, 0, boxsize, boxsize);
          BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
          BSP_LCD_FillRect(boxsize * 2, 0, boxsize, boxsize);
          BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
          BSP_LCD_FillRect(boxsize * 3, 0, boxsize, boxsize);
          BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
          BSP_LCD_FillRect(boxsize * 4, 0, boxsize, boxsize);
          BSP_LCD_SetTextColor(LCD_COLOR_MAGENTA);
          BSP_LCD_FillRect(boxsize * 5, 0, boxsize, boxsize);
          BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
          BSP_LCD_FillRect(boxsize * 6, 0, boxsize, boxsize);
          BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
          BSP_LCD_DrawRect(boxsize * 6 + 2, 2, boxsize - 4, boxsize - 4);
          BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
          BSP_LCD_DrawRect((ts.X / boxsize) * boxsize, 0, boxsize, boxsize);
        }

        if (oldcolor != currentcolor)
        {
          BSP_LCD_SetTextColor(oldcolor);
          if (oldcolor == LCD_COLOR_RED)
            BSP_LCD_FillRect(0, 0, boxsize, boxsize);
          if (oldcolor == LCD_COLOR_YELLOW)
            BSP_LCD_FillRect(boxsize, 0, boxsize, boxsize);
          if (oldcolor == LCD_COLOR_GREEN)
            BSP_LCD_FillRect(boxsize * 2, 0, boxsize, boxsize);
          if (oldcolor == LCD_COLOR_CYAN)
            BSP_LCD_FillRect(boxsize * 3, 0, boxsize, boxsize);
          if (oldcolor == LCD_COLOR_BLUE)
            BSP_LCD_FillRect(boxsize * 4, 0, boxsize, boxsize);
          if (oldcolor == LCD_COLOR_MAGENTA)
            BSP_LCD_FillRect(boxsize * 5, 0, boxsize, boxsize);
        }
        is_new_touch = 1;  // Reset filter on color selection
      }
      else
      {
        // Apply exponential moving average filter
        if(is_new_touch)
        {
          filtered_x = ts.X;
          filtered_y = ts.Y;
          printf("new touch\r\n");
        }
        else
        {
          filtered_x = FILTER_ALPHA * ts.X + (1.0f - FILTER_ALPHA) * filtered_x;
          filtered_y = FILTER_ALPHA * ts.Y + (1.0f - FILTER_ALPHA) * filtered_y;
        }
        
        uint16_t draw_x = (uint16_t)filtered_x;
        uint16_t draw_y = (uint16_t)filtered_y;
        
        BSP_LCD_SetTextColor(currentcolor);
        if(is_new_touch){
          BSP_LCD_FillRect(draw_x, draw_y, 1, 1);
        }
        else{
          BSP_LCD_DrawLine(last_x, last_y, draw_x, draw_y);
        }
        last_x = draw_x;
        last_y = draw_y;
      }
      
      last_touch_time = current_time;
    }
  }
}