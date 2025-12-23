#pragma once

#include "main.h"
#include <string.h>
#include <stdbool.h>

void rtt_init(void);
void rtt_write(const char *, uint32_t);

// From SEGGER RTT specification:
#define SEGGER_RTT_MODE_NO_BLOCK_SKIP 0      // Skip if full
#define SEGGER_RTT_MODE_NO_BLOCK_TRIM 1      // Trim data if full
#define SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL 2 // Block forever

#define DEBUG_MESSAGE_TIMEOUT 1000 // ms until we give up waiting for the debugger