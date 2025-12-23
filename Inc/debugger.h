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

#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c "
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0') 

  