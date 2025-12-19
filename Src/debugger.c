#include "debugger.h"
#include <string.h>

// Minimal RTT implementation
typedef struct {
    const char *name;
    char *buffer;
    uint32_t size;
    volatile uint32_t write_pos;
    volatile uint32_t read_pos;
    uint32_t flags;
} rtt_channel_t;

typedef struct {
    char id[16];  // "SEGGER RTT"
    uint32_t num_up_channels;
    uint32_t num_down_channels;
    rtt_channel_t up[1];    // Up channel (target to host)
    rtt_channel_t down[1];  // Down channel (host to target)
} __attribute__((packed)) rtt_control_block_t;

// Buffer storage - must be in RAM
static char rtt_buffer[1024];

// Control block - must be in RAM, not optimized away
__attribute__((section(".data")))                  // Use .data instead of .rtt
__attribute__((used)) __attribute__((aligned(4)))  // Ensure 4-byte alignment
rtt_control_block_t _SEGGER_RTT;

// Initialize the RTT in your startup code
void rtt_init(void) {
    memcpy(_SEGGER_RTT.id, "SEGGER RTT", 11);
    _SEGGER_RTT.num_up_channels = 1;
    _SEGGER_RTT.num_down_channels = 0;

    _SEGGER_RTT.up[0].name = "Terminal";
    _SEGGER_RTT.up[0].buffer = rtt_buffer;
    _SEGGER_RTT.up[0].size = sizeof(rtt_buffer);
    _SEGGER_RTT.up[0].write_pos = 0;
    _SEGGER_RTT.up[0].read_pos = 0;
    _SEGGER_RTT.up[0].flags = SEGGER_RTT_MODE_BLOCK_IF_FIFO_FULL;
}

// Write function
void rtt_write(const char *data, uint32_t len) {
    // for (uint32_t i = 0; i < len; i++) {
    //     uint32_t write_pos = _SEGGER_RTT.up[0].write_pos;
    //     uint32_t read_pos = _SEGGER_RTT.up[0].read_pos;
    //     uint32_t next_pos = (write_pos + 1) % _SEGGER_RTT.up[0].size;
    //     static uint32_t last_read_pos = 0;
    //     uint32_t this_tick = HAL_GetTick();
    //     static uint32_t debug_message_timeout_tracker = 0;

    //     // Track the last time the debugger read debug message buffer
    //     // If time exceeds DEBUG_MESSAGE_TIMEOUT, don't even write to the buffer.
    //     if (last_read_pos != read_pos) {
    //         debug_message_timeout_tracker = this_tick;
    //     }

    //     // A couple strategies are possible on a write.
    //     // 1. wait infinitely for the buffer to be read
    //     // 2. stop writing to the buffer if it's full (buffer will become stale)

    //     // Wait if buffer full
    //     // while (next_pos == read_pos && this_tick - DEBUG_MESSAGE_TIMEOUT < debug_message_timeout_tracker)
    //     while (next_pos == read_pos)
    //     {
    //         read_pos = _SEGGER_RTT.up[0].read_pos;
    //         this_tick =
    //             HAL_GetTick();  // update time here too, otherwise it could get stuck here if buffer fills too quickly
    //     }

    //     // Skip this byte if buffer is full
    //     // if (next_pos == read_pos)
    //     // {
    //     //     continue; // Drop the data, don't block
    //     // }

    //     _SEGGER_RTT.up[0].buffer[write_pos] = data[i];
    //     _SEGGER_RTT.up[0].write_pos = next_pos;
    //     last_read_pos = read_pos;
    // }
}

/**
 * @brief Pipes printf to RTT (SEGGER debug pipeline)
 *
 * @param file
 * @param ptr
 * @param len
 * @return int
 */
int _write(int file, char *ptr, int len) {
    (void)file;
    rtt_write(ptr, len);
    return len;
}