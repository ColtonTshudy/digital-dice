#include "app_main.h"

// App variables
struct Statistics stats;

// Display variables
TS_StateTypeDef ts;
uint16_t boxsize;
uint16_t oldcolor, currentcolor;

void setup()
{
  rtt_init();
  printf("Hello, world!\r\n");

  initStats(&stats, 6, 2);
  // stats.roll_counts[2] = 0;
  // stats.roll_counts[3] = 2;
  // stats.roll_counts[4] = 3;
  // stats.roll_counts[5] = 4;
  // stats.roll_counts[6] = 5;
  // stats.roll_counts[7] = 6;
  // stats.roll_counts[8] = 5;
  // stats.roll_counts[9] = 4;
  // stats.roll_counts[10] = 3;
  // stats.roll_counts[11] = 2;
  // stats.roll_counts[12] = 1;
  // stats.number_of_rolls = 36;

  for(uint8_t i = 0; i < 13; i++)
    {
      printf("Rolls on [%u]: %u\r\n", i, stats.roll_counts[i]);
    }

  BSP_LCD_Init();
  BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize());
  BSP_LCD_Clear(LCD_COLOR_WHITE);
}

void loop()
{
  heartbeat();

  // stats.roll_counts[7]++;
  // stats.number_of_rolls++;
  
  if(ts.TouchDetected){
    setRandSeed(generateRandSeed());
    roll(&stats);
    generateGraph(&stats);
    HAL_Delay(2000);
  }
//   

//   for(uint8_t i = 0; i < 13; i++)
//     {
//       printf("Rolls on [%u]: %u\r\n", i, stats.roll_counts[i]);
//     }
}

void generateGraph(struct Statistics *stats)
{
  uint16_t total_rolls = 0;
  uint16_t max_count = 0;

  // Calculate total rolls
  for(uint8_t i = stats->min_roll; i <= stats->max_roll; i++)
    {
      total_rolls += stats->roll_counts[i];
    }

  if(total_rolls == 0)
    return;

  // Find max count (equivalent to max in lua)
  for(uint8_t i = stats->min_roll; i <= stats->max_roll; i++)
    {
      if(max_count < stats->roll_counts[i])
        max_count = stats->roll_counts[i];
    }

  // max = max/dice_rolls, then max = 20/max
  float max_normalized = (float)max_count / total_rolls;
  float scale = 20.0f / max_normalized;

  // Starting position
  uint16_t y_pos = 40;
  uint16_t bar_unit_width = 8; // Width per "|" character
  uint16_t bar_height = 12;
  uint16_t line_spacing = 16;

  // Clear screen
  BSP_LCD_Clear(LCD_COLOR_BLACK);

  // Draw graph for each value
  for(uint8_t i = stats->min_roll; i <= stats->max_roll; i++)
    {
      float measured = (float)stats->roll_counts[i] / total_rolls;
      float expected = stats->expect[i];

      uint16_t measuredBars = (uint16_t)(measured * scale);
      uint16_t expectBars = (uint16_t)(expected * scale);
      int16_t count = measuredBars;

      // Display the number label
      char label[8];
      sprintf(label, "%d:", i);
      BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
      BSP_LCD_DisplayStringAt(5, y_pos, (uint8_t *)label, LEFT_MODE);

      uint16_t x_pos = 35;

      // Draw 20 bars
      for(uint8_t j = 1; j <= 20; j++)
        {
          // Set color when we start printing measured bars
          if(measuredBars == count)
            {
              if(measured - expected > 0.01f)
                {
                  BSP_LCD_SetTextColor(LCD_COLOR_GREEN);
                }
              else if(measured - expected < -0.01f)
                {
                  BSP_LCD_SetTextColor(LCD_COLOR_RED);
                }
              else
                {
                  BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
                }
            }

          // Switch to gray when count reaches 0
          if(count == 0)
            {
              BSP_LCD_SetTextColor(LCD_COLOR_DARKGRAY);
            }

          // Draw bar if condition is met
          if(count > 0 || (count == 0 && measuredBars < expectBars)
             || (count < 0 && (measuredBars - count) < expectBars))
            {
              BSP_LCD_FillRect(x_pos, y_pos, bar_unit_width - 1, bar_height);
            }

          count--;
          x_pos += bar_unit_width;
        }

      // Display the count at the end
      sprintf(label, " %d", stats->roll_counts[i]);
      BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
      BSP_LCD_DisplayStringAt(x_pos + 5, y_pos, (uint8_t *)label, LEFT_MODE);

      y_pos += line_spacing;
    }
}

void heartbeat()
{
  uint16_t blink_duration = 500;
  static uint32_t last_time = 0;
  uint32_t this_time = HAL_GetTick();
  if(this_time - last_time > blink_duration)
    {
      HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
      last_time = this_time;
      printf("Heartbeat\r\n");
    }
}
