#pragma once

#include "app_stats.h"
#include "debugger.h"
#include "stm32f4xx_hal.h"
// #include "stm32_adafruit_lcd.h"
// #include "stm32_adafruit_ts.h"
#include "lcd.h"
#include "touch.h"
#include "gui.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void setup();
void loop();
void heartbeat();
void generateGraph(struct Statistics*);
uint32_t getRandSeed();