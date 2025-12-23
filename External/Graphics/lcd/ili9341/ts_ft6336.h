/*
 * FT6336 touch driver STM32F4
 * I2C capacitive touch controller
 * Converted from XPT2046 driver by Roberto Benjami
 */

//=============================================================================

/* Debug printf - define DEBUG_FT6336 to enable */
#ifdef DEBUG_FT6336
  #define FT6336_DEBUG(fmt, ...) printf("[FT6336] " fmt "\r\n", ##__VA_ARGS__)
#else
  #define FT6336_DEBUG(fmt, ...)
#endif

/* I2C select (1, 2, 3)
   - 1..3: hardware I2C driver */
#define TS_I2C            2

/* I2C speed (0=100kHz, 1=400kHz) */
#define TS_I2C_SPEED      1

/* I2C pins alternative function assign (0..15) */
#define TS_I2C_AFR        4

/* Touch controller pins assign (A..K, 0..15)
   - SCL, SDA pins assign is locked to hardware I2C */
#define TS_SCL            B, 10
#define TS_SDA            B, 3
#define TS_RST            B, 15  /* Reset pin - if not used leave as X, 0 */
#define TS_IRQ            B, 13  /* Interrupt pin - if not used leave as X, 0 */

/* FT6336 I2C address (7-bit: 0x38) */
#define FT6336_I2C_ADDR   0x38

/* Maximum touch points supported */
#define TS_MAX_POINTS     2
