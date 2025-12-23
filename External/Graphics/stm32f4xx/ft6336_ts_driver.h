/*
 * FT6336 Driver Adapter for STM32 Adafruit Touchscreen Interface
 */

#ifndef __FT6336_TS_DRIVER_H
#define __FT6336_TS_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* TS_DrvTypeDef structure definition */
typedef struct
{
  void     (*Init)(uint16_t);
  uint16_t (*ReadID)(uint16_t);
  void     (*Reset)(uint16_t);
  void     (*Start)(uint16_t);
  uint8_t  (*DetectTouch)(uint16_t);
  void     (*GetXY)(uint16_t, uint16_t*, uint16_t*);
  void     (*EnableIT)(uint16_t);
  void     (*ClearIT)(uint16_t);
  uint8_t  (*GetITStatus)(uint16_t);
  void     (*DisableIT)(uint16_t);
} TS_DrvTypeDef;

/* Export driver pointer */
extern TS_DrvTypeDef *ts_drv;

/* Export calibration matrix */
extern int32_t ts_cindex[];

#ifdef __cplusplus
}
#endif

#endif /* __FT6336_TS_DRIVER_H */
